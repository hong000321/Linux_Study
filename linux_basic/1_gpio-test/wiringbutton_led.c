#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>

#define BUTTON_GPIO 0
int ledControl(int gpio){
    int i;
    pinMode(gpio, OUTPUT);
    pinMode(BUTTON_GPIO, INPUT);  // Assuming GPIO pin 1 is used for a button

    while(1){
        if(digitalRead(BUTTON_GPIO) == LOW) {
            printf("Button is pressed, starting LED control...\n");
            digitalWrite(gpio, HIGH);  // Turn the LED on
            delay(100);
        } else {
            printf("Button is not pressed, exiting...\n");
            digitalWrite(gpio, LOW);   // Turn the LED off
            delay(100);
        }
    }
    return 0;
}

int main(int argc, char *argv[]) {
    int gno;

    if(argc < 2) {
        fprintf(stderr, "Usage: %s <GPIO pin number>\n", argv[0]);
        return EXIT_FAILURE;
    }

    gno = atoi(argv[1]);
    wiringPiSetup();  // Initialize wiringPi
    ledControl(gno);  // Control the LED on the specified GPIO pin
    return 0;
}
