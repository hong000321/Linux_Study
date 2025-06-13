#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include "myPin.h"


int ledControl(int gpio){
    int i;
    pinMode(gpio, OUTPUT);
    for (i = 0; i < 10; i++) {
        digitalWrite(gpio, HIGH);  // Turn the LED on
        delay(500);                 // Wait for 500 milliseconds
        digitalWrite(gpio, LOW);   // Turn the LED off
        delay(500);                 // Wait for 500 milliseconds
    }
    return 0;
}

int main(int argc, char *argv[]) {
    int gno;

    if(argc < 2) {
        fprintf(stderr, "Usage: %s <GPIO pin number>\n", argv[0]);
        return EXIT_FAILURE;
    }

    gno = atoi(argv[1]); // LED_WPI
    wiringPiSetup();  // Initialize wiringPi
    ledControl(gno);  // Control the LED on the specified GPIO pin
    return 0;
}