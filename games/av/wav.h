#ifndef WAV_H
#define WAV_H

#ifdef __cplusplus
extern "C"{
#endif

/* structure to store WAV header */
typedef struct{
    char          rID[4];
    unsigned long rLen;
    char          wID[4];
    char          fID[4];
    unsigned long fLen;
    unsigned int  wFormatTag;
    unsigned int  nChannels;
    unsigned long nSamplesPerSec;
    unsigned long nAvgBytesPerSec;
    unsigned int  nBlockAlign;
    unsigned int  nFormatSpecific;
    unsigned long sLength;
} WAVHEADER;

#ifdef __cplusplus
}
#endif

#endif
