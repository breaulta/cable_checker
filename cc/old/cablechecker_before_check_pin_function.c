#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <signal.h>

int i, j;  //loop variables

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

int check_pin(int output_test_pin, int input_test_pin, const int *input_arr, FILE *fp ) {

  int testing_pin_read;
  int r[6];  //array to hold the rest of the pin reads
  int read_loop_offset = 0;
  int result;  //zero = cable good, 1 = bad connection, 2 = short

  //testing...
  int val[7];

  digitalWrite (output_test_pin, HIGH);
  delay(50);

  //testing
  for( j = 0; j < 7; j++){
	val[j] = digitalRead( input_arr[j] );
  }

  for ( j = 0; j < 7; j++){
    if(input_test_pin == input_arr[j]){
	testing_pin_read = digitalRead( input_arr[j] );
	read_loop_offset = 1;
    } else {
	r[ j - read_loop_offset ] = digitalRead( input_arr[j] );
    }
  }
  if( testing_pin_read && !( r[0] || r[1] || r[2] || r[3] || r[4] || r[5] )){
    fprintf (fp, "**Pin OZERO is GOOD.**\n");
    result = 0;
  } else if( !testing_pin_read ){
    fprintf (fp, "!~!!Looks like pin OZERO has a BAD CONNECTION!!~!\n");
    result = 1;
  } else {
    fprintf (fp, "!~!!Looks like pin OZERO has a SHORT!!~!\n");
    result = 2;
  }
  digitalWrite( output_test_pin, LOW );

  //testing
  fprintf (fp, "v0:%d, v1:%d,  v2:%d,  v3:%d,  v4:%d,  v5:%d,  v6:%d \n", val[0],val[1],val[2],val[3],val[4],val[5],val[6]);
  fprintf (fp, "testing_pin_read:%d, r0:%d, r1:%d, r2:%d, r3:%d, r4:%d, r5:%d \n\n", testing_pin_read, r[0], r[1], r[2], r[3], r[4], r[5] );


  return result;
}

int main (void) {

  //testing?  pin zero result from function
  int zero_result;

  //set constants to corresponding GPIO pin
  static const int out[8] = { 2, 3, 4, 14, 15, 17, 18, 27 };
  static const int in[7] = { 21, 20, 26, 16, 19, 13, 12 };

  //array to hold read values for log file
  int v[7];
  int bad_conn_f, short_f;  //initialize flags

  //register signals
  signal(SIGINT, handle_control_c);

  //set pins to input or output
  wiringPiSetupGpio () ;  //this allows setting directly to corr. GPIO pin

  //set up cable checker pins for output/input
  for (i = 0; i < 8; i++) { pinMode ( out[i], OUTPUT); }
  for (j = 0; j < 7; j++) { pinMode ( in[j], INPUT); }

  //set up logfile for appending
  //FILE *f = fopen("/home/pi/cable_checker/logfile.txt", "a");
  FILE *f = fopen("logfile.txt", "a");
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
	fprintf(f, "Cable detected!  Starting test.\n");
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
//check_pin(int output_test_pin, int input_test_pin, int *input_arr, FILE *fp )

  //check pin out[0] against pin in[0] -- 
  zero_result = check_pin( out[0], in[0], in, f );
  
  //Main Test Block
  //test pin OZERO
  //digitalWrite (out[0], HIGH);
  //delay(50);
  //for (j = 0; j < 7; j++){  //read input pins
//	v[j] = digitalRead ( in[j] );
 // }
  // in[0]/v[0] should be HIGH
 // if (v[0] && !v[1] && !v[2] && !v[3] && !v[4] && !v[5] && !v[6] ){
 //   fprintf (f, "**Pin OZERO is GOOD.**\n");
 // } else if ( v[0] && (v[1] || v[2] || v[3] || v[4] || v[5] || v[6])) {
 //   fprintf (f, "!~!!Looks like pin OZERO has a SHORT!!~!\n");
  //  short_f = 1;
  //} else {
  //  fprintf (f, "!~!!Looks like pin OZERO has a BAD CONNECTION!!~!\n");
  //  bad_conn_f = 1;
 // }
  //record pin data to logfile
 // fprintf (f, "v0:%d, v1:%d,  v2:%d,  v3:%d,  v4:%d,  v5:%d,  v6:%d \n", v[0],v[1],v[2],v[3],v[4],v[5],v[6]);
 // digitalWrite (out[0], LOW);
 
  //test pin OONE
  digitalWrite (out[1], HIGH);
  delay(50);
  for (j = 0; j < 7; j++){
	v[j] = digitalRead ( in[j] );
  }
  //IONE/v1 should be HIGH
  if (!v[0] && v[1] && !v[2] && !v[3] && !v[4] && !v[5] && !v[6] ){
    fprintf (f, "**Pin OONE is GOOD.**\n");
  } else if ( v[1] && (v[0] || v[2] || v[3] || v[4] || v[5] || v[6])) {
    fprintf (f, "!~!!Looks like pin OONE has a SHORT!!~!\n");
    short_f = 1;
  } else {
    fprintf (f, "!~!!Looks like pin OONE has a BAD CONNECTION!!~!\n");
    bad_conn_f = 1;
  }
  fprintf (f, "v0:%d, v1:%d,  v2:%d,  v3:%d,  v4:%d,  v5:%d,  v6:%d \n", v[0],v[1],v[2],v[3],v[4],v[5],v[6]);
  digitalWrite (out[1], LOW);
 
  //test pin OTWO 
  digitalWrite (out[2], HIGH); 
  delay(50);
  for (j = 0; j < 7; j++){ 
        v[j] = digitalRead ( in[j] );
  }
  //ITWO/v2 should be HIGH
  if (!v[0] && !v[1] && v[2] && !v[3] && !v[4] && !v[5] && !v[6] ){ 
    fprintf (f, "**Pin OTWO is GOOD.**\n");
  } else if ( v[2] && (v[1] || v[0] || v[3] || v[4] || v[5] || v[6])) {
    fprintf (f, "!~!!Looks like pin OTWO has a SHORT!!~!\n");
    short_f = 1;
  } else {
    fprintf (f, "!~!!Looks like pin OTWO has a BAD CONNECTION!!~!\n");
    bad_conn_f = 1;
  }
  fprintf (f, "v0:%d, v1:%d,  v2:%d,  v3:%d,  v4:%d,  v5:%d,  v6:%d \n", v[0],v[1],v[2],v[3],v[4],v[5],v[6]);
  digitalWrite (out[2], LOW); 
 
  //test pin OTHREE
  digitalWrite (out[3], HIGH); 
  delay(50);
  for (j = 0; j < 7; j++){
        v[j] = digitalRead ( in[j] );
  }
  //ITHREE/v3 should be HIGH
  if (!v[0] && !v[1] && !v[2] && v[3] && !v[4] && !v[5] && !v[6] ){
    fprintf (f, "**Pin OTHREE is GOOD.**\n");
  } else if ( v[3] && (v[1] || v[2] || v[0] || v[4] || v[5] || v[6])) {
    fprintf (f, "!~!!Looks like pin OTHREE has a SHORT!!~!\n");
    short_f = 1;
  } else {
    fprintf (f, "!~!!Looks like pin OTHREE has a BAD CONNECTION!!~!\n");
    bad_conn_f = 1;
  }
  fprintf (f, "v0:%d, v1:%d,  v2:%d,  v3:%d,  v4:%d,  v5:%d,  v6:%d \n", v[0],v[1],v[2],v[3],v[4],v[5],v[6]);
  digitalWrite (out[3], LOW); 
 
  //test pin OFOUR*******
  digitalWrite (out[4], HIGH);
  delay(50);
  for (j = 0; j < 7; j++){
        v[j] = digitalRead ( in[j] );
  }
  //***IZERO/v0 should be HIGH-This is the double gnd pin on the JAE connector
  if (v[0] && !v[1] && !v[2] && !v[3] && !v[4] && !v[5] && !v[6] ){
    fprintf (f, "**Pin OFOUR is GOOD.**\n");
  } else if ( v[0] && (v[1] || v[2] || v[3] || v[4] || v[5] || v[6])) {
    fprintf (f, "!~!!Looks like pin OFOUR has a SHORT!!~!\n");
    short_f = 1;
  } else {
    fprintf (f, "!~!!Looks like pin OFOUR has a BAD CONNECTION!!~!\n");
    bad_conn_f = 1;
  }
  fprintf (f, "v0:%d, v1:%d,  v2:%d,  v3:%d,  v4:%d,  v5:%d,  v6:%d \n", v[0],v[1],v[2],v[3],v[4],v[5],v[6]);
  digitalWrite (out[4], LOW);

  //test pin OFIVE
  digitalWrite (out[5], HIGH);
  delay(50);
  for (j = 0; j < 7; j++){
        v[j] = digitalRead ( in[j] );
  }
  //IFOUR/v4 should be HIGH  
  if (!v[0] && !v[1] && !v[2] && !v[3] && v[4] && !v[5] && !v[6] ){
    fprintf (f, "**Pin OFIVE is GOOD.**\n");
  } else if ( v[4] && (v[1] || v[2] || v[3] || v[0] || v[5] || v[6])) {
    fprintf (f, "!~!!Looks like pin OFIVE has a SHORT!!~!\n");
    short_f = 1;
  } else {
    fprintf (f, "!~!!Looks like pin OFIVE has a BAD CONNECTION!!~!\n");
    bad_conn_f = 1;
  }
  fprintf (f, "v0:%d, v1:%d,  v2:%d,  v3:%d,  v4:%d,  v5:%d,  v6:%d \n", v[0],v[1],v[2],v[3],v[4],v[5],v[6]);
  digitalWrite (out[5], LOW);

  //test output pin six
  digitalWrite (out[6], HIGH);
  delay(50);
  for (j = 0; j < 7; j++){
        v[j] = digitalRead ( in[j] );
  }
  //IFIVE/v5 should be HIGH  
  if (!v[0] && !v[1] && !v[2] && !v[3] && !v[4] && v[5] && !v[6] ){
    fprintf (f, "**Pin OSIX is GOOD.**\n");
  } else if ( v[5] && (v[1] || v[2] || v[3] || v[4] || v[0] || v[6])) {
    fprintf (f, "!~!!Looks like pin OSIX has a SHORT!!~!\n");
    short_f = 1;
  } else {
    fprintf (f, "!~!!Looks like pin OSIX has a BAD CONNECTION!!~!\n");
    bad_conn_f = 1;
  }
  fprintf (f, "v0:%d, v1:%d,  v2:%d,  v3:%d,  v4:%d,  v5:%d,  v6:%d \n", v[0],v[1],v[2],v[3],v[4],v[5],v[6]);
  digitalWrite (out[6], LOW);

  //test pin OSEVEN
  digitalWrite (out[7], HIGH);
  delay(50);
  for (j = 0; j < 7; j++){
        v[j] = digitalRead ( in[j] );
  }
  //ISIX/v6 should be HIGH
  if (!v[0] && !v[1] && !v[2] && !v[3] && !v[4] && !v[5] && v[6] ){
    fprintf (f, "**Pin OSEVEN is GOOD.**\n");
  } else if ( v[6] && (v[1] || v[2] || v[3] || v[4] || v[5] || v[0])) {
    fprintf (f, "!~!!Looks like pin OSEVEN has a SHORT!!~!\n");
    short_f = 1;
  } else {
    fprintf (f, "!~!!Looks like pin OSEVEN has a BAD CONNECTION!!~!\n");
    bad_conn_f = 1;
  }
  fprintf (f, "v0:%d, v1:%d,  v2:%d,  v3:%d,  v4:%d,  v5:%d,  v6:%d \n", v[0],v[1],v[2],v[3],v[4],v[5],v[6]);
  digitalWrite (out[7], LOW);

  //clean up and end program on (^C)
  if (interrupt_f) { 
    green_off();
    fclose(f);
    printf("**Interrupt caught!\n");
    exit(1);
  }

  //turn on all pins for to check if cable is connected
  for (i = 0; i < 8; i++){ digitalWrite (out[i], HIGH); }
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
    fprintf (f, "@X@ Looks like there was a SHORT. X@X\n\n");
    //force write to logfile-- I believe the many system() calls are keeping the writes from happening immediately
    fflush(f);
  } else if (bad_conn_f) {
    //blink pattern for bad connection - solid red
    red_on();
    do {   //wait until cable is unplugged
	delay(500);
	for (j = 0; j < 7; j++){ v[j] = digitalRead (in[j]); }
    } while (v[0] || v[1] || v[2] || v[3] || v[4] || v[5] || v[6] );
    fprintf (f, "@X@ Looks like there was a BAD CONNECTION. X@X\n\n");
    fflush(f);
    red_off();
  } else {
    //blink pattern for GOOD cable - solid green
    green_on();
    do {  //wait until cable is unplugged
	delay(500);
	for (j = 0; j < 7; j++){ v[j] = digitalRead (in[j]); }
    } while (v[0] || v[1] || v[2] || v[3] || v[4] || v[5] || v[6] );
    fprintf (f, "$$$CABLE CHECK IS GOOD!$$$\n\n");
    fflush(f);
    green_off();
  } 
  //end main loop! - going back to Ready Loop
}

  fclose(f);
return 0 ;
}





