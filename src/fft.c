#include "fft.h"

#include <stdint.h>

#include "util.h"

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

#define NTT_LEN 3
static ntt_t ntts[NTT_LEN] = {
    {2013265921, 27, 137},
    {469762049, 26, 30},
    {1811939329, 26, 136},
};

static void calc_roots(ntt_t *ntt) {
  ntt->root = malloc_safe(sizeof(int64_t) * (ntt->div_limit + 1));
  ntt->root_inv = malloc_safe(sizeof(int64_t) * (ntt->div_limit + 1));

  ntt->root[ntt->div_limit] =
      mpow(ntt->primitive_root,
           (ntt->mod - 1) / mpow(2, ntt->div_limit, ntt->mod), ntt->mod);
  ntt->root_inv[ntt->div_limit] = minv(ntt->root[ntt->div_limit], ntt->mod);
  for (int32_t i = (int32_t)ntt->div_limit - 1; i >= 0; i--) {
    ntt->root[i] = ntt->root[i + 1] * ntt->root[i + 1] % ntt->mod;
    ntt->root_inv[i] = ntt->root_inv[i + 1] * ntt->root_inv[i + 1] % ntt->mod;
  }
}

void setup_fft(void) {
  for (int32_t i = 0; i < NTT_LEN; i++)
    calc_roots(&ntts[i]);
}

static int32_t log2(int32_t n) {
  int32_t ans = 0;
  while (!(n & 1)) {
    n >>= 1;
    ans++;
  }
  return ans;
}

void ufft(int64_t *f, int32_t n, ntt_t *ntt) {
  int32_t m = n;

  while (m > 1) {
    for (int32_t i = 0; i < n / m; i++) {
      int64_t zeta = ntt->root[log2(m)];
      int64_t now = 1;
      for (int32_t j = 0; j < m / 2; j++) {
        int64_t l = f[i * m + j];
        int64_t r = f[i * m + j + m / 2];
        f[i * m + j] = (l + r) % ntt->mod;
        f[i * m + j + m / 2] = msub(l, r, ntt->mod) * now % ntt->mod;
        now = now * zeta % ntt->mod;
      }
    }
    m /= 2;
  }
}

void iufft(int64_t *f, int32_t n, ntt_t *ntt) {
  int32_t m = 2;

  while (m <= n) {
    for (int32_t i = 0; i < n / m; i++) {
      int64_t zeta = ntt->root_inv[log2(m)];
      int64_t now = 1;
      for (int32_t j = 0; j < m / 2; j++) {
        int64_t l = f[i * m + j];
        int64_t r = (f[i * m + j + m / 2] * now) % ntt->mod;
        f[i * m + j] = (l + r) % ntt->mod;
        f[i * m + j + m / 2] = msub(l, r, ntt->mod);
        now = now * zeta % ntt->mod;
      }
    }
    m *= 2;
  }

  for (int64_t i = 0; i < n; i++)
    f[i] = mdiv(f[i], n, ntt->mod);
}

int32_t get_convolution_size(int32_t f_len, int32_t g_len) {
  int32_t size = 1;
  while (size < f_len + g_len)
    size *= 2;
  return size;
}

void convolution_internal(const int64_t *f, int32_t f_len, const int64_t *g,
                          int32_t g_len, int64_t *out, int32_t n, ntt_t *ntt) {
  int64_t nf[n];
  int64_t ng[n];

  for (int32_t i = 0; i < n; i++) {
    nf[i] = (i < f_len) ? f[i] : 0;
    ng[i] = (i < g_len) ? g[i] : 0;
  }

  ufft(nf, n, ntt);
  ufft(ng, n, ntt);

  for (int32_t i = 0; i < n; i++) {
    out[i] = nf[i] * ng[i] % ntt->mod;
  }

  iufft(out, n, ntt);
}

static int64_t mod(int64_t a, int64_t m) {
  int64_t ans = a % m;
  if (ans < 0)
    ans += m;
  return ans;
}

// XXX: 何やってるのかよくわからない
static int64_t garner(const int64_t *r, int64_t *m, int32_t n) {
  m[n] = INT64_MAX;
  int64_t coeffs[n + 1];
  int64_t constants[n + 1];
  for (int32_t i = 0; i <= n; i++) {
    coeffs[i] = 1;
    constants[i] = 0;
  }
  for (int32_t i = 0; i < n; i++) {
    int64_t t = mod((r[i] - constants[i]) * minv(coeffs[i], m[i]), m[i]);
    for (int32_t j = i + 1; j < n + 1; j++) {
      constants[j] = (constants[j] + t * coeffs[j]) % m[j];
      coeffs[j] = (coeffs[j] * m[i]) % m[j];
    }
  }

  return constants[n];
}

void convolution(const int64_t *f, int32_t f_len, const int64_t *g,
                 int32_t g_len, int64_t *out, int32_t n) {
  int64_t outs[NTT_LEN][n];

  for (int32_t i = 0; i < NTT_LEN; i++) {
    convolution_internal(f, f_len, g, g_len, outs[i], n, &ntts[i]);
  }

  int64_t r[NTT_LEN];
  int64_t m[NTT_LEN + 1];
  for (int32_t j = 0; j < NTT_LEN; j++)
    m[j] = ntts[j].mod;

  // FIXME: CPUキャッシュに乗らないので改善の余地あり
  for (int32_t i = 0; i < n; i++) {
    for (int32_t j = 0; j < NTT_LEN; j++)
      r[j] = outs[j][i];
    out[i] = garner(r, m, NTT_LEN);
  }
}