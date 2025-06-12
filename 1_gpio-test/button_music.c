#include <wiringPi.h>
#include <softPwm.h>
#include <stdio.h>
#include <stdlib.h>
#include "myPin.h"
#define C_RESOLUTION 255 // Define the resolution for PWM
#define SPK_PIN BUZZER_WPI // Define the GPIO pin for the speaker

int notes[] = {
    391, 391, 440, 391, 391, 329.63, 329.63,\
    391, 391, 329,63, 329,63, 293.66, 293.66, 293.66, 0,\
    391, 391, 440, 440, 391, 391, 329.63, 329.63,\
    391, 329.63, 293.66, 329.63, 261.63, 261.63,261.63, 0
};
int musicPlay() {
    int i;
    softToneCreate(SPK_PIN);  // Initialize GPIO pin for tone generation
    for(i = 0; i < sizeof(notes)/sizeof(notes[0]); i++) {
        softToneWrite(SPK_PIN, notes[i]);
        delay(280);  // Wait for 500 milliseconds before playing the next note
    }
    return 0;
}

int main(int argc, char *argv[]) {
    wiringPiSetup();  // Initialize wiringPi
    musicPlay();  // Play music on GPIO pin 1
    return 0;
}
