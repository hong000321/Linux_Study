#include <wiringPi.h>
#include <stdio.h>
#include "myPin.h"

#define SW BUTTON_WPI
#define CDS CDS_WPI
#define LED LED_WPI


int cdsControl(){
    int i;
    pinMode(SW, INPUT);  // Set switch pin as input
    pinMode(CDS, INPUT);  // Set CDS pin as input
    pinMode(LED, OUTPUT); // Set LED pin as output

    for(;;){
        if(digitalRead(CDS) == HIGH){
            digitalWrite(LED, LOW); // Turn off LED after 1 second
            delay(100);
            
        }else{
            digitalWrite(LED, HIGH); // Turn on LED if CDS is high
            delay(100);
        }
    }
    return 0;
}


int main(int argc, char *argv[]) {
    wiringPiSetup();  // Initialize wiringPi
    cdsControl();  // Control the CDS and LED based on CDS input
    return 0;
}