#include <wiringPi.h>
#include <stdio.h>




int main(void){
    wiringPiSetup();  // Initialize wiringPi
    printf("WiringPi setup complete.\n");

    // Example: Set GPIO pin 17 as input and read its value
    pinMode(17, INPUT);
    pullUpDnControl(, PUD_OFF);  // Disable pull-up/down control
    printf("GPIO pin 17 set as output.\n");
    return 0;
}