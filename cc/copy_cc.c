#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
int main (void) {

  //set constants to corresponding GPIO pin
  static const int OZERO = 2;
  static const int OONE = 3;
  static const int OTWO = 4;
  static const int OTHREE = 14;
  static const int OFOUR = 15;
  static const int OFIVE = 17;
  static const int OSIX = 18;
  static const int OSEVEN = 27;

  static const int IZERO = 21;
  static const int IONE = 20;
  static const int ITWO = 26;
  static const int ITHREE = 16;
  static const int IFOUR = 19;
  static const int IFIVE = 13;
  static const int ISIX = 12;

  int v0, v1, v2, v3, v4, v5, v6;
  int failed = 0;

  //set pins to input or output
  wiringPiSetupGpio () ;  //this allows setting directly to corr. GPIO pin
  pinMode (OZERO, OUTPUT);
  pinMode (OONE, OUTPUT);
  pinMode (OTWO, OUTPUT);
  pinMode (OTHREE, OUTPUT);
  pinMode (OFOUR, OUTPUT);
  pinMode (OFIVE, OUTPUT);
  pinMode (OSIX, OUTPUT);
  pinMode (OSEVEN, OUTPUT);
 
  pinMode (IZERO, INPUT);
  pinMode (IONE, INPUT);
  pinMode (ITWO, INPUT);
  pinMode (ITHREE, INPUT);
  pinMode (IFOUR, INPUT);
  pinMode (IFIVE, INPUT);
  pinMode (ISIX, INPUT);
 
  //set up logfile for write
  FILE *f = fopen("logfile.txt", "w");
  if (f == NULL)
  {
    printf("Error opening logfile!\n");
    exit(1);
  }
 
  //turn on all pins for to check if cable is connected
  digitalWrite (OZERO, HIGH);
  digitalWrite (OONE, HIGH);
  digitalWrite (OTWO, HIGH);
  digitalWrite (OTHREE, HIGH);
  digitalWrite (OFOUR, HIGH);
  digitalWrite (OFIVE, HIGH);
  digitalWrite (OSIX, HIGH);
  digitalWrite (OSEVEN, HIGH);
 
for (;;){ 
  //wait till a cable is connected
  while(1){ 
    delay(50);
    v0 = digitalRead (IZERO);
    v1 = digitalRead (IONE);
    v2 = digitalRead (ITWO);
    v3 = digitalRead (ITHREE);
    v4 = digitalRead (IFOUR);
    v5 = digitalRead (IFIVE);
    v6 = digitalRead (ISIX);
    if (v0 && v1 && v2 && v3 && v4 && v5 && v6 ){ 
	fprintf(f, "Cable detected!  Starting test.");
        //turn off all pins to continue check 
        digitalWrite (OZERO, LOW);
  	digitalWrite (OONE, LOW);
  	digitalWrite (OTWO, LOW);
  	digitalWrite (OTHREE, LOW);
  	digitalWrite (OFOUR, LOW);
  	digitalWrite (OFIVE, LOW);
  	digitalWrite (OSIX, LOW);
  	digitalWrite (OSEVEN, LOW);
	break;
    }
  }

  //test pin zero
  digitalWrite (OZERO, HIGH);
  delay(50);
  v0 = digitalRead (IZERO);
  v1 = digitalRead (IONE);
  v2 = digitalRead (ITWO);
  v3 = digitalRead (ITHREE);
  v4 = digitalRead (IFOUR);
  v5 = digitalRead (IFIVE);
  v6 = digitalRead (ISIX);
  //IZERO should be HIGH
  if (v0 && !v1 && !v2 && !v3 && !v4 && !v5 && !v6 ){
    fprintf (f, "Successfully tested output pin OZERO.\n");
  } else {
    failed = 1;
    fprintf (f, "****Problem with pin OZERO!****\n");
  }
  //testing...
  fprintf (f, "v0:%d, v1:%d,  v2:%d,  v3:%d,  v4:%d,  v5:%d,  v6:%d \n", v0,v1,v2,v3,v4,v5,v6);
  //delay(2000);
  digitalWrite (OZERO, LOW);
 
  //test pin one
  digitalWrite (OONE, HIGH);
  delay(50);
  v0 = digitalRead (IZERO);
  v1 = digitalRead (IONE);
  v2 = digitalRead (ITWO);
  v3 = digitalRead (ITHREE);
  v4 = digitalRead (IFOUR);
  v5 = digitalRead (IFIVE);
  v6 = digitalRead (ISIX);
  //IZERO should be HIGH
  if (!v0 && v1 && !v2 && !v3 && !v4 && !v5 && !v6 ){
    fprintf (f, "Successfully tested output pin OONE\n");
  } else {
    failed = 1;
    fprintf (f, "****Problem with pin OONE!****\n");
  }
  //testing...
  fprintf (f, "v0:%d, v1:%d,  v2:%d,  v3:%d,  v4:%d,  v5:%d,  v6:%d \n", v0,v1,v2,v3,v4,v5,v6);
  //delay(2000);
  digitalWrite (OONE, LOW);
 
  //test pin two *
  digitalWrite (OTWO, HIGH); //*
  delay(50);
  v0 = digitalRead (IZERO);
  v1 = digitalRead (IONE);
  v2 = digitalRead (ITWO);
  v3 = digitalRead (ITHREE);
  v4 = digitalRead (IFOUR);
  v5 = digitalRead (IFIVE);
  v6 = digitalRead (ISIX);
  //IZERO should be HIGH
  if (!v0 && !v1 && v2 && !v3 && !v4 && !v5 && !v6 ){  //*
    fprintf (f, "Successfully tested output pin OTWO\n");  //*
  } else {
    failed = 1;
    fprintf (f, "****Problem with pin OTWO!****\n");
  }
  //testing...
  fprintf (f, "v0:%d, v1:%d,  v2:%d,  v3:%d,  v4:%d,  v5:%d,  v6:%d \n", v0,v1,v2,v3,v4,v5,v6);
  //delay(2000);
  digitalWrite (OTWO, LOW); //*
 
  //test pin three *
  digitalWrite (OTHREE, HIGH); //*
  delay(50);
  v0 = digitalRead (IZERO);
  v1 = digitalRead (IONE);
  v2 = digitalRead (ITWO);
  v3 = digitalRead (ITHREE);
  v4 = digitalRead (IFOUR);
  v5 = digitalRead (IFIVE);
  v6 = digitalRead (ISIX);
  //ITHREE should be HIGH  *
  if (!v0 && !v1 && !v2 && v3 && !v4 && !v5 && !v6 ){  //*
    fprintf (f, "Successfully tested output pin OTHREE\n");  //*
  } else {
    failed = 1;
    fprintf (f, "****Problem with pin OTHREE!****\n");
  }
  //testing...
  fprintf (f, "v0:%d, v1:%d,  v2:%d,  v3:%d,  v4:%d,  v5:%d,  v6:%d \n", v0,v1,v2,v3,v4,v5,v6);
  //delay(2000);
  digitalWrite (OTHREE, LOW); //*
 
  //test pin four*
  digitalWrite (OFOUR, HIGH); //*
  delay(50);
  v0 = digitalRead (IZERO);
  v1 = digitalRead (IONE);
  v2 = digitalRead (ITWO);
  v3 = digitalRead (ITHREE);
  v4 = digitalRead (IFOUR);
  v5 = digitalRead (IFIVE);
  v6 = digitalRead (ISIX);
  //IZERO should be HIGH  *
  if (v0 && !v1 && !v2 && !v3 && !v4 && !v5 && !v6 ){  //*
    fprintf (f, "Successfully tested output pin OFOUR\n");  //*
  } else {
    failed = 1;
    fprintf (f, "****Problem with pin OFOUR!****\n");
  }
  //testing...
  fprintf (f, "v0:%d, v1:%d,  v2:%d,  v3:%d,  v4:%d,  v5:%d,  v6:%d \n", v0,v1,v2,v3,v4,v5,v6);
  //delay(2000);
  digitalWrite (OFOUR, LOW); //*

  //test pin five*
  digitalWrite (OFIVE, HIGH); //*
  delay(50);
  v0 = digitalRead (IZERO);
  v1 = digitalRead (IONE);
  v2 = digitalRead (ITWO);
  v3 = digitalRead (ITHREE);
  v4 = digitalRead (IFOUR);
  v5 = digitalRead (IFIVE);
  v6 = digitalRead (ISIX);
  //IFOUR should be HIGH  *
  if (!v0 && !v1 && !v2 && !v3 && v4 && !v5 && !v6 ){  //*
    fprintf (f, "Successfully tested output pin OFIVE\n");  //*
  } else {
    failed = 1;
    fprintf (f, "****Problem with pin OFIVE!****\n");
  }
  //testing...
  fprintf (f, "v0:%d, v1:%d,  v2:%d,  v3:%d,  v4:%d,  v5:%d,  v6:%d \n", v0,v1,v2,v3,v4,v5,v6);
  //delay(2000);
  digitalWrite (OFIVE, LOW); //*

  //test output pin six*
  digitalWrite (OSIX, HIGH); //*
  delay(50);
  v0 = digitalRead (IZERO);
  v1 = digitalRead (IONE);
  v2 = digitalRead (ITWO);
  v3 = digitalRead (ITHREE);
  v4 = digitalRead (IFOUR);
  v5 = digitalRead (IFIVE);
  v6 = digitalRead (ISIX);
  //IFIVE should be HIGH  *
  if (!v0 && !v1 && !v2 && !v3 && !v4 && v5 && !v6 ){  //*
    fprintf (f, "Successfully tested output pin OSIX\n");  //*
  } else {
    failed = 1;
    fprintf (f, "****Problem with pin OSIX!****\n");
  }
  //testing...
  fprintf (f, "v0:%d, v1:%d,  v2:%d,  v3:%d,  v4:%d,  v5:%d,  v6:%d \n", v0,v1,v2,v3,v4,v5,v6);
  //delay(2000);
  digitalWrite (OSIX, LOW); //*

  //test output pin seven*
  digitalWrite (OSEVEN, HIGH); //*
  delay(50);
  v0 = digitalRead (IZERO);
  v1 = digitalRead (IONE);
  v2 = digitalRead (ITWO);
  v3 = digitalRead (ITHREE);
  v4 = digitalRead (IFOUR);
  v5 = digitalRead (IFIVE);
  v6 = digitalRead (ISIX);
  //ISIX should be HIGH  *
  if (!v0 && !v1 && !v2 && !v3 && !v4 && !v5 && v6 ){  //*
    fprintf (f, "Successfully tested output pin OSEVEN\n");  //*
  } else {
    failed = 1;
    fprintf (f, "****Problem with pin OSEVEN!****\n");
  }
  //testing...
  fprintf (f, "v0:%d, v1:%d,  v2:%d,  v3:%d,  v4:%d,  v5:%d,  v6:%d \n", v0,v1,v2,v3,v4,v5,v6);
  //delay(2000);
  digitalWrite (OSEVEN, LOW); //*

  //check if there was a failure, break loop and close program if so
  if (failed) { break; }

}

  fclose(f);
return 0 ;
}
