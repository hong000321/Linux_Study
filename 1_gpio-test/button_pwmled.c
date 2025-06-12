#include <wiringPi.h>
#include <softPwm.h>
#include <stdio.h>
#include <stdlib.h>
#define C_RESOLUTION 255 // Define the resolution for PWM
int ledPwmControl(int gpio) {
    pinMode(gpio, OUTPUT);  // Set GPIO pin as PWM output
    softPwmCreate(gpio, 0, C_RESOLUTION); // Initialize PWM with a range of 0 to 255
    pinMode(1, INPUT);  // Assuming GPIO pin 1 is used for a button
    while(1){
        if(digitalRead(1) == HIGH) {
            printf("Button is pressed, starting LED control...\n");
            softPwmWrite(gpio, C_RESOLUTION/3);  // Set PWM to maximum value
        }
        else {
            printf("Button is not pressed, exiting...\n");
            softPwmWrite(gpio, 0);  // Turn off PWM
            delay(100);  // Wait for 100 milliseconds before checking again
        }
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
