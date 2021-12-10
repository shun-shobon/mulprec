#ifndef FFT_H
#define FFT_H

#include "stdbool.h"
#include "stdint.h"

extern void setup_fft(void);
extern void ufft(int64_t *, int32_t);
extern void iufft(int64_t *, int32_t);
extern int32_t get_convolution_size(int32_t, int32_t);
extern void convolution(const int64_t *, int32_t, const int64_t *, int32_t,
                        int64_t *, int32_t);

#endif // FFT_H
