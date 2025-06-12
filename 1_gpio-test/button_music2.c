#include <wiringPi.h>
#include <softPwm.h>
#include <stdio.h>
#include <stdlib.h>
#include "song.h"
#include "myPin.h"
#define C_RESOLUTION 255 // Define the resolution for PWM
#define SPK_PIN BUZZER_WPI // Define the GPIO pin for the speaker


#if 0 // 반짝반짝 작은별별
int notes[] = {
    391, 391, 440, 391, 391, 330, 330,
    391, 391, 330, 330, 294, 294, 294, 0,
    391, 391, 440, 440, 391, 391, 330, 330,
    391, 330, 294, 330, 262, 262, 262, 0
};

int timing[] = {
    500, 500, 500, 500, 500, 500, 1000,
    500, 500, 500, 500, 500, 500, 1000,
    500, 500, 500, 500, 500, 500, 1000,
    500, 500, 500, 500, 500, 500, 1000,
    500
};

#elif 0// 진격거
int notes[] = {
    // 1절
    392, 440, 494, 392,  392, 440, 494, 392,  
    392, 440, 494, 523, 494, 440, 392,

    // 코러스
    392, 440, 494, 523, 494, 440, 392,
    392, 440, 494, 523, 494, 440, 392,
    440, 494, 523, 587, 659, 587, 523, 494,
    0 // 끝 쉼표
};

int timing[] = {
    // 1절
    555, 555, 555, 555,  555, 555, 555, 555,
    555, 555, 555, 555, 555, 555, 1110,

    // 코러스
    555, 555, 555, 555, 555, 555, 1110,
    555, 555, 555, 555, 555, 555, 1110,
    555, 555, 555, 555, 555, 555, 555, 1110,
    555 // 끝 쉼표
};

#endif

int musicPlay() {
    int i;
    softToneCreate(SPK_PIN);  // Initialize GPIO pin for tone generation
    for(i = 0; i < sizeof(notes)/sizeof(notes[0]); i++) {
        softToneWrite(SPK_PIN, notes[i]);
        delay(timing[i]*3);  // Wait for 500 milliseconds before playing the next note
    }
    return 0;
}
int main(int argc, char *argv[]) {
    wiringPiSetup();  // Initialize wiringPi
    musicPlay();  // Play music on GPIO pin 1
    return 0;
}
