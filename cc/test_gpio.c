#include <stdlib.h>
#include <stdio.h>
#include <wiringPi.h>
int main (void) {

//  wiringPiSetupGpio () ;  //this allows setting directly to corr. GPIO pin
 // pinMode (47, OUTPUT); //this should control the green ACT LED
//  system("echo none > /sys/class/leds/led0/trigger");
  for(;;){
//digitalWrite(47, HIGH);
system("rpi3-gpiovirtbuf s 135 1");
delay(200);
//digitalWrite(47, LOW);
system("rpi3-gpiovirtbuf s 135 0");
delay(200);


}


  return 0;
}
