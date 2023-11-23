#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>
#include	<complex>
typedef void* Descriptor;

typedef enum {
    Sr_48kHz = 0,
    Sr_96kHz,
    Sr_192kHz,
    Sr_384kHz,
    Sr_768kHz,
    Sr_1536kHz,
    Sr_1920kHz,
    Sr_2560kHz,
    Sr_3072kHz,
} SampleRateIndex;

typedef bool (*pCallbackRx)(std::complex<float> *, uint32_t, bool, void *);


#endif // COMMON_H
