#ifndef __WAVHEADER_H
#define __WAVHEADER_H

typedef struct {
    unsigned char riffID[4];        // "RIFF"
    unsigned long riffLen;
    unsigned char waveID[4];        // "WAVE"
    unsigned char fmtID[4];        // "fmt "
    unsigned long fmtLen;        // 16 for PCM
    unsigned short fmtTag;    // 1 for PCM
    unsigned short nChannels;    // 1 for mono, 2 for stereo
    unsigned long sampleRate; // Sample rate
    unsigned long avgBytesPerSec; // SampleRate * nChannels * BitsPerSample/8
    unsigned short nblockAlign; // nChannels * BitsPerSample/8
    unsigned short bitsPerSample; // 8, 16, 24, or 32 bits
    unsigned char dataID[4];    // "data"
    unsigned long dataLen;        // Number of bytes in the data section
} WAVHEADER;


#endif // __WAVHEADER_H