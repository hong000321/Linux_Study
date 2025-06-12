#include <wiringPi.h>
#include <stdio.h>
#include "myPin.h"

int int2segment(int a){
    switch(a){
        case 0: return 0b00111111; // 0
        case 1: return 0b00000110; // 1
        case 2: return 0b01011011; // 2
        case 3: return 0b01001111; // 3
        case 4: return 0b01100110; // 4
        case 5: return 0b01101101; // 5
        case 6: return 0b01111101; // 6
        case 7: return 0b00000111; // 7
        case 8: return 0b01111111; // 8
        case 9: return 0b01101111; // 9
        default: return -1;       // Invalid input
    }
}


void displaySegment(int value) {
    int segment = int2segment(value);
    if (segment == -1) {
        printf("Invalid segment value: %d\n", value);
        return;
    }

    for (int i = 0; i < 7; i++) {
        digitalWrite(mySegments[i], (segment >> i) & 0x01);
    }
}


int main(void) {
    wiringPiSetup();  // Initialize wiringPi
    
    pinMode(mySegments[0], OUTPUT); // A
    pinMode(mySegments[1], OUTPUT); // B
    pinMode(mySegments[2], OUTPUT); // C
    pinMode(mySegments[3], OUTPUT); // D
    pinMode(mySegments[4], OUTPUT); // E
    pinMode(mySegments[5], OUTPUT); // F
    pinMode(mySegments[6], OUTPUT); // G
    int value;
    while(1){
        scanf("%d", &value);
        displaySegment(value);
        delay(10); // Wait for 2 seconds
    }
    
    printf("Segments A, B, D, and F are turned on.\n");



    return 0;
}