#define BUTTON_WPI      1
#define LED_WPI         2
#define BUZZER_WPI      3
#define CDS_WPI    4


int wpi2bcm[] = {
    17, //0
    18, //1
    27, //2
    22, //3
    23, //4
    24, //5
    25, //6
};

int mySegments[] = {
    21, // A
    22, // B
    23, // C
    24, // D
    25, // E
    26, // F
    27, // G
    -1, // DP
};


int mySeg2Bcm[] = {
    5, // A
    6, // B
    13, // C
    19, // D
    26, // E
    12, // F
    16, // G
    -1, // DP
};