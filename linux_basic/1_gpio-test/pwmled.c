#include <wiringPi.h>
#include <softPwm.h>
#include <stdio.h>
#include <stdlib.h>
#define C_RESOLUTION 255 // Define the resolution for PWM
int ledPwmControl(int gpio) {
    pinMode(gpio, OUTPUT);  // Set GPIO pin as PWM output
    softPwmCreate(gpio, 0, C_RESOLUTION); // Initialize PWM with a range of 0 to 255

    for(int i = 0; i < C_RESOLUTION*5; i++) {
        softPwmWrite(gpio, i&C_RESOLUTION);  // Set PWM to maximum value
        delay(5);                // Wait for 5 milliseconds
    }
    softPwmWrite(gpio, 0);  // Turn off PWM
    return 0;
}

int main(int argc, char *argv[]) {

    int gno;
    if(argc <2){
        printf("usage: %s <GPIO pin number>\n", argv[0]);
        return EXIT_FAILURE;
    }
    gno = atoi(argv[1]);
    wiringPiSetup();  // Initialize wiringPi
    ledPwmControl(gno);  // Control the LED on the specified GPIO pin

    return 0;
}
