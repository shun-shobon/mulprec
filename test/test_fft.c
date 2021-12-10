#include "fft.h"

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>

void print_array(const int64_t *f, int64_t n) {
  for (uint32_t i = 0; i < n; i++) {
    printf("%lld", f[i]);
    if (i != n - 1)
      printf(" ");
  }

  printf("\n");
}

int main(void) {
  setup_fft();

  int32_t n, m;
  scanf("%" SCNd32, &n);
  scanf("%" SCNd32, &m);

  int64_t f[n], g[m];

  for (int32_t i = 0; i < n; i++)
    scanf("%" SCNd64, &f[i]);
  for (int32_t i = 0; i < m; i++)
    scanf("%" SCNd64, &g[i]);

  int32_t size = get_convolution_size(n, n);
  int64_t out[size];

  convolution(f, n, g, n, out, size);
  print_array(out, n + n - 1);
}