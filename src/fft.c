#include "fft.h"

#include <stdint.h>

static void swap(int64_t *a, int64_t *b) {
  int64_t tmp = *a;
  *a = *b;
  *b = tmp;
}

static int64_t minv(int64_t a, int64_t mod) {
  int64_t n = a;
  int64_t m = mod;
  int64_t u = 1;
  int64_t v = 0;
  while (m != 0) {
    int64_t t = n / m;
    n -= t * m;
    swap(&n, &m);
    u -= t * v;
    swap(&u, &v);
  }
  u %= mod;
  if (u < 0)
    u += mod;
  return u;
}

static int64_t msub(int64_t a, int64_t b, int64_t mod) {
  int64_t val = a - b;
  if (val < 0)
    val += mod;
  return val;
}

static int64_t mdiv(int64_t a, int64_t b, int64_t mod) {
  return a * minv(b, mod) % mod;
}

static int64_t mpow(int64_t a, int64_t n, int64_t mod) {
  int64_t res = 1;
  while (n > 0) {
    if (n & 1) {
      res = res * a % mod;
    }
    a = a * a % mod;
    n >>= 1;
  }

  return res;
}

static const int64_t MOD = 2013265921;
static const int64_t DIV_LIMIT = 27;
static int64_t ROOT[DIV_LIMIT + 1];
static int64_t ROOT_INV[DIV_LIMIT + 1];
static const int64_t PRIMITIVE_ROOT = 137;

void setup_fft(void) {
  ROOT[DIV_LIMIT] =
      mpow(PRIMITIVE_ROOT, (MOD - 1) / mpow(2, DIV_LIMIT, MOD), MOD);
  ROOT_INV[DIV_LIMIT] = minv(ROOT[DIV_LIMIT], MOD);
  for (int64_t i = DIV_LIMIT - 1; i >= 0; i--) {
    ROOT[i] = ROOT[i + 1] * ROOT[i + 1] % MOD;
    ROOT_INV[i] = ROOT_INV[i + 1] * ROOT_INV[i + 1] % MOD;
  }
}

static int32_t log2(int32_t n) {
  int32_t ans = 0;
  while (!(n & 1)) {
    n >>= 1;
    ans++;
  }
  return ans;
}

void ufft(int64_t *f, int32_t n) {
  int32_t m = n;

  while (m > 1) {
    for (int32_t i = 0; i < n / m; i++) {
      int64_t zeta = ROOT[log2(m)];
      int64_t now = 1;
      for (int32_t j = 0; j < m / 2; j++) {
        int64_t l = f[i * m + j];
        int64_t r = f[i * m + j + m / 2];
        f[i * m + j] = (l + r) % MOD;
        f[i * m + j + m / 2] = msub(l, r, MOD) * now % MOD;
        now = now * zeta % MOD;
      }
    }
    m /= 2;
  }
}

void iufft(int64_t *f, int32_t n) {
  int32_t m = 2;

  while (m <= n) {
    for (int32_t i = 0; i < n / m; i++) {
      int64_t zeta = ROOT_INV[log2(m)];
      int64_t now = 1;
      for (int32_t j = 0; j < m / 2; j++) {
        int64_t l = f[i * m + j];
        int64_t r = (f[i * m + j + m / 2] * now) % MOD;
        f[i * m + j] = (l + r) % MOD;
        f[i * m + j + m / 2] = msub(l, r, MOD);
        now = now * zeta % MOD;
      }
    }
    m *= 2;
  }

  for (int64_t i = 0; i < n; i++) {
    f[i] = mdiv(f[i], n, MOD);
  }
}

int32_t get_convolution_size(int32_t f_len, int32_t g_len) {
  int32_t size = 1;
  while (size < f_len + g_len)
    size *= 2;
  return size;
}

// WARNING: f_len + g_len < 2^23 を前提とする
void convolution(const int64_t *f, int32_t f_len, const int64_t *g,
                 int32_t g_len, int64_t *out, int32_t n) {
  int64_t nf[n];
  int64_t ng[n];

  for (int32_t i = 0; i < n; i++) {
    nf[i] = (i < f_len) ? f[i] : 0;
    ng[i] = (i < g_len) ? g[i] : 0;
  }

  ufft(nf, n);
  ufft(ng, n);

  for (int32_t i = 0; i < n; i++) {
    out[i] = nf[i] * ng[i] % MOD;
  }

  iufft(out, n);
}