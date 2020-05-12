#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <signal.h>
#include <time.h>

int i, j;  //loop variables

void timestamp( FILE *file )
{
    time_t ltime; /* calendar time */
    ltime=time(NULL); /* get current cal time */
    fprintf(file, "Test started at:  %s",asctime( localtime(&ltime) ) );
}

//handle interrupt signal ( ^C ) from the user
volatile sig_atomic_t interrupt_f= 0;
void handle_control_c(int sig){ // can be called asynchronously
  interrupt_f= 1; // set flag
}

//LED control functions
void red_off(){ system("rpi3-gpiovirtbuf s 135 0"); }
void red_on(){ system("rpi3-gpiovirtbuf s 135 1"); }
void green_off(){ system("/opt/vc/bin/vcmailbox 0x00038041 8 8 130 0"); }
void green_on(){ system("/opt/vc/bin/vcmailbox 0x00038041 8 8 130 1"); }

//int check_pin(int output_test_pin, int input_test_pin, const int *input_arr, FILE *fp ) 
int check_pin(int output_tp_ref, const int *output_arr, int input_tp_ref, const int *input_arr, FILE *fp ) 
{
  int testing_pin_read;
  int r[6];  //array to hold the rest of the test pin reads
  int val[7]; //array to hold read values for log
  int read_loop_offset = 0;
  int result;  //zero = cable good, 1 = bad connection, 2 = short
  //these variables added for code readability
  int output_test_pin = output_arr[ output_tp_ref ];
  int input_test_pin = input_arr[ input_tp_ref ];

  digitalWrite (output_test_pin, HIGH);
  delay(50);
  for ( j = 0; j < 7; j++){
    val[j] = digitalRead( input_arr[j] );
    if(input_test_pin == input_arr[j]){
	testing_pin_read = digitalRead( input_arr[j] );
	read_loop_offset = 1;
    } else {
	r[ j - read_loop_offset ] = digitalRead( input_arr[j] );
    }
  }
  if( testing_pin_read && !( r[0] || r[1] || r[2] || r[3] || r[4] || r[5] )){
    result = 0;
  } else if( !testing_pin_read ){
    fprintf (fp, "BAD CONNECTION --     Out# %d  In# %d\n", output_tp_ref, input_tp_ref);
    fprintf (fp, "p0:%d p1:%d  p2:%d  p3:%d  p4:%d  p5:%d  p6:%d \n", val[0],val[1],val[2],val[3],val[4],val[5],val[6]);
    result = 1;
  } else {
    fprintf (fp, "SHORT --              Out#%d In#%d\n", output_tp_ref, input_tp_ref);
    fprintf (fp, "p0:%d p1:%d  p2:%d  p3:%d  p4:%d  p5:%d  p6:%d \n", val[0],val[1],val[2],val[3],val[4],val[5],val[6]);
    result = 2;
  }
  digitalWrite( output_test_pin, LOW );
  return result;
}

int main (void) {

  //set constants to corresponding GPIO pin
  static const int out[8] = { 2, 3, 4, 14, 15, 17, 18, 27 };
  static const int in[7] = { 21, 20, 26, 16, 19, 13, 12 };

  //array to hold read values for log file
  int v[7];
  int bad_conn_f, short_f;  //initialize flags
  
  //array to hold test results
  int results[8];

  //register signals
  signal(SIGINT, handle_control_c);

  //set pins to input or output
  wiringPiSetupGpio () ;  //this allows setting directly to corr. GPIO pin

  //set up cable checker pins for output/input
  for (i = 0; i < 8; i++) { pinMode ( out[i], OUTPUT); }
  for (j = 0; j < 7; j++) { pinMode ( in[j], INPUT); }

  //set up logfile for appending
  FILE *f = fopen("/home/pi/cable_checker/logfile.txt", "a");
  //FILE *f = fopen("logfile.txt", "a");
  if (f == NULL) {
    printf("Error opening logfile!\n");
    exit(1);
  }

for (;;){   //keep checking cables untill interrupt (^C)

  //reset flags
  bad_conn_f = 0;
  short_f = 0;

  while(1){ //wait till a cable is connected
    //turn on all pins for to check if cable is connected
    for (i = 0; i < 8; i++){ digitalWrite( out[i], HIGH); }
    green_off();
    delay(500);
    green_on();
    delay(500);

    //read pins
    for (j = 0; j < 7; j++){ v[j] = digitalRead ( in[j] ); }
    if (v[0] || v[1] || v[2] || v[3] || v[4] || v[5] || v[6] ){ 
	green_off();
        //turn off all pins to continue check 
	for (i = 0; i < 8; i++){  digitalWrite ( out[i], LOW ); }
	delay(500);   //this is to make sure the user has time to connect the cable all the way
	break;
    }
    //clean up and end program on (^C)
    if (interrupt_f) { 
      green_off();
      fclose(f);
      printf("**Interrupt caught!\n");
      exit(1);
    }
  }
  timestamp(f);
  //check_pin(int output_tp_ref, int *output_arr, int input_tp_ref, int *input_arr, FILE *fp )
  results[0] = check_pin( 0, out, 0, in, f );
  results[1] = check_pin( 1, out, 1, in, f );
  results[2] = check_pin( 2, out, 2, in, f );
  results[3] = check_pin( 3, out, 3, in, f );
  results[4] = check_pin( 4, out, 0, in, f );
  results[5] = check_pin( 5, out, 4, in, f );
  results[6] = check_pin( 6, out, 5, in, f );
  results[7] = check_pin( 7, out, 6, in, f );

  //clean up and end program on (^C)
  if (interrupt_f) { 
    green_off();
    fclose(f);
    printf("**Interrupt caught!\n");
    exit(1);
  }

  for (i = 0; i < 8; i++){ 
    if( !results[i] ){ continue; } //pin passed test
    else {
	if( results[i] == 1 ){ bad_conn_f = 1; }
	else{ short_f = 1; }
    }
  }

  //turn on all pins for to check if cable is still connected
  for( i = 0; i < 8; i++){ digitalWrite (out[i], HIGH); }
  //control loops/indicator blinks
  if (short_f) {
    //wait until cable is unplugged
    do {
      	//blink pattern for short - flashing red
	red_on();
	delay(500);
	red_off();
	delay(500);
	//read pins
	for (j = 0; j < 7; j++){ v[j] = digitalRead (in[j]); }
    } while (v[0] || v[1] || v[2] || v[3] || v[4] || v[5] || v[6] );
    fprintf (f, "SHORT\n\n");
    //force write to logfile-- I believe the many system() calls are keeping the writes from happening immediately
    fflush(f);
  } else if (bad_conn_f) {
    //blink pattern for bad connection - solid red
    red_on();
    do {   //wait until cable is unplugged
	delay(500);
	for (j = 0; j < 7; j++){ v[j] = digitalRead (in[j]); }
    } while (v[0] || v[1] || v[2] || v[3] || v[4] || v[5] || v[6] );
    fprintf (f, "BAD CONNECTION\n\n");
    fflush(f);
    red_off();
  } else {
    //blink pattern for GOOD cable - solid green
    green_on();
    do {  //wait until cable is unplugged
	delay(500);
	for (j = 0; j < 7; j++){ v[j] = digitalRead (in[j]); }
    } while (v[0] || v[1] || v[2] || v[3] || v[4] || v[5] || v[6] );
    fprintf (f, "Cable Check is GOOD.\n\n");
    fflush(f);
    green_off();
  } 
  //end main loop! - going back to Ready Loop
}

  fclose(f);
return 0 ;
}





