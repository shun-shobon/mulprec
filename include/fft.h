#ifndef FFT_H
#define FFT_H

#include "stdbool.h"
#include "stdint.h"

// mod = 2^dev_limit + 1
typedef struct {
  int64_t mod;
  int64_t div_limit;
  int64_t primitive_root;
  int64_t *root;
  int64_t *root_inv;
} ntt_t;

extern void setup_fft(void);
extern void ufft(int64_t *, int32_t, ntt_t *);
extern void iufft(int64_t *, int32_t, ntt_t *);
extern int32_t get_convolution_size(int32_t, int32_t);
extern void convolution(const int64_t *, int32_t, const int64_t *, int32_t,
                        int64_t *, int32_t);

#endif // FFT_H
