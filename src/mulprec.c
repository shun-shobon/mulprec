#include "mulprec.h"

#include <inttypes.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

void set_sign(num_t *num, sign_t sign) { num->sign = sign; }
sign_t get_sign(const num_t *num) { return num->sign; }

void print_num(const num_t *num) {
  int64_t tmp[num->len];
  for (int32_t i = 0; i < num->len; i++)
    tmp[i] = num->n[i];

  int64_t buf[(size_t)ceil(log10(NUM_BASE) / log10(NUM_DEC_MAX) * num->len)];
  int32_t idx = 0;

  int32_t start = num->len - 1;
  while (start >= 0) {
    int64_t carry = 0;
    for (int32_t i = start; i >= 0; i--) {
      int64_t val = carry * NUM_BASE + tmp[i];
      tmp[i] = val / NUM_DEC_MAX;
      carry = val % NUM_DEC_MAX;
    }

    buf[idx++] = carry;

    while (tmp[start] == 0)
      start--;
  }

  if (num->sign == SIGN_NEG)
    putchar('-');
  printf("%" PRId64, buf[idx - 1]);
  for (int32_t i = idx - 2; i >= 0; i--)
    printf("%0" TO_STRING(NUM_BASE_DIGIT) PRId64, buf[i]);
}

stat_t input_num(const char *src, num_t *dst) {
  if (src[0] == '-') {
    set_sign(dst, SIGN_NEG);
    src++;
  } else {
    set_sign(dst, SIGN_POS);
  }

  int32_t len = (int32_t)strlen(src);

  dst->len = 0;

  int64_t tmp[(size_t)ceil((double)len / NUM_BASE_DIGIT)];
  int32_t idx = 0;
  for (int32_t i = len - 1; i >= 0; i -= NUM_BASE_DIGIT) {
    int64_t val = 0;
    int64_t pow = 1;
    bool brk = false;
    for (int32_t j = 0; j < NUM_BASE_DIGIT; j++) {
      if (i - j < 0) {
        brk = true;
        break;
      }
      val += (int64_t)(src[i - j] - '0') * pow;
      pow *= 10;
    }
    if (dst->len == NUM_LEN)
      return STAT_OVERFLOW;
    tmp[idx++] = val;
    if (brk)
      break;
  }

  int32_t start = idx - 1;
  while (start >= 0) {
    int64_t carry = 0;
    for (int32_t i = start; i >= 0; i--) {
      int64_t val = carry * NUM_DEC_MAX + tmp[i];
      tmp[i] = val / NUM_BASE;
      carry = val % NUM_BASE;
    }

    dst->n[dst->len++] = carry;
    if (dst->len == NUM_LEN)
      return STAT_OVERFLOW;

    while (tmp[start] == 0)
      start--;
  }

  return STAT_OK;
}

void copy_num(const num_t *src, num_t *dst) {
  set_sign(dst, get_sign(src));

  dst->len = src->len;

  for (int32_t i = 0; i < src->len; i++) {
    dst->n[i] = src->n[i];
  }
}

bool is_zero(const num_t *num) {
  if (get_sign(num) == SIGN_NEG)
    return false;

  for (int32_t i = 0; i < num->len; i++) {
    if (num->n[i] != 0)
      return false;
  }

  return true;
}

stat_t shift_left(const num_t *in, num_t *out, int32_t digit) {
  if (NUM_LEN < in->len + digit)
    return STAT_OVERFLOW;

  for (int32_t i = in->len - 1; i >= 0; i--) {
    out->n[i + digit] = in->n[i];
  }

  for (int32_t i = 0; i < digit; i++) {
    out->n[i] = 0;
  }

  out->len = in->len + digit;

  return STAT_OK;
}

stat_t shift_right(const num_t *in, num_t *out, int32_t digit) {
  for (int32_t i = digit; i < in->len; i++) {
    out->n[i - digit] = in->n[i];
  }

  out->len = in->len - digit;

  if (out->len < 1) {
    out->n[0] = 0;
    out->len = 1;
  }

  return STAT_OK;
}

stat_t bit_shift_left(const num_t *in, num_t *out, int32_t bit) {
  int32_t shift_digit = bit / NUM_BASE_POW_2;
  int32_t shift_bit = bit % NUM_BASE_POW_2;
  int32_t mask_bit = (1 << NUM_BASE_POW_2) - 1;

  guard_stat(shift_left(in, out, shift_digit));

  if (out->len < NUM_LEN)
    out->n[out->len] = 0;
  for (int32_t i = out->len - 1; i >= 0; i--) {
    out->n[i] <<= shift_bit;
    int64_t amount = out->n[i] & mask_bit;
    int64_t overflow = (out->n[i] ^ amount) >> NUM_BASE_POW_2;
    if (i == out->len - 1 && out->len < NUM_LEN)
      return STAT_OVERFLOW;
    out->n[i + 1] |= overflow;
    out->n[i] = amount;
  }

  if (out->n[out->len] != 0)
    out->len++;

  return STAT_OK;
}

stat_t bit_shift_right(const num_t *in, num_t *out, int32_t bit) {
  int32_t shift_digit = bit / NUM_BASE_POW_2;
  int32_t shift_bit = bit % NUM_BASE_POW_2;
  int32_t mask_bit = (1 << shift_bit) - 1;

  guard_stat(shift_right(in, out, shift_digit));

  for (int32_t i = out->len - 1; i >= 1; i--) {
    int64_t overflow = out->n[i] & mask_bit;
    out->n[i - 1] |= overflow << NUM_BASE_POW_2;
    out->n[i] >>= shift_bit;
  }
  out->n[0] >>= shift_bit;

  if (out->n[out->len - 1] == 0)
    out->len--;

  return STAT_OK;
}

stat_t set_int(int64_t in, num_t *out) {
  *out = ZERO_NUM;

  if (in < 0)
    set_sign(out, SIGN_NEG);

  int64_t tmp = in >= 0 ? in : -in;
  for (int32_t i = 0; i < NUM_LEN; i++) {
    out->n[i] = tmp % NUM_BASE;
    tmp /= NUM_BASE;

    if (tmp == 0) {
      out->len = i + 1;
      break;
    }
  }

  if (tmp == 0) {
    return STAT_OK;
  } else {
    out->len = NUM_LEN;
    return STAT_OVERFLOW;
  }
}

static stat_t fix_num(num_t *num) {
  // 通常の繰り上がり/繰り下がり処理
  for (int32_t i = 0; i < num->len - 1; i++) {
    if (NUM_BASE <= num->n[i]) {
      int64_t carry = num->n[i] / NUM_BASE;
      num->n[i] -= carry * NUM_BASE;
      num->n[i + 1] += carry;
    }
    if (num->n[i] < 0) {
      int64_t borrow = (-num->n[i] - 1) / NUM_BASE + 1;
      num->n[i] += borrow * NUM_BASE;
      num->n[i + 1] -= borrow;
    }
  }

  // 最終桁の繰り上がり/繰り下がり処理
  // XXX: 合ってるか不明
  while (num->len < NUM_LEN &&
         (num->n[num->len - 1] < 0 || NUM_BASE <= num->n[num->len - 1])) {
    if (NUM_BASE <= num->n[num->len - 1]) {
      int64_t carry = num->n[num->len - 1] / NUM_BASE;
      num->n[num->len - 1] -= carry * NUM_BASE;
      num->n[num->len] = carry;
    }
    if (num->n[num->len - 1] < 0) {
      int64_t borrow = (-num->n[num->len] - 1) / NUM_BASE + 1;
      num->n[num->len - 1] += borrow * NUM_BASE;
      num->n[num->len] = -borrow;
    }
    num->len++;
  }

  // 最終桁の0を消し去る
  while (1 < num->len && num->n[num->len - 1] == 0)
    num->len--;

  // オーバーフロー/アンダーフロー判定
  if (num->len <= NUM_LEN ||
      (0 <= num->n[NUM_LEN - 1] && num->n[NUM_LEN - 1] < NUM_BASE))
    return STAT_OK;
  else
    return STAT_OVERFLOW;
}

static ord_t comp_num_nat(const num_t *a, const num_t *b) {
  if (a->len > b->len)
    return ORD_GT;
  if (a->len < b->len)
    return ORD_LT;

  for (int32_t i = a->len - 1; i >= 0; i--) {
    if (a->n[i] > b->n[i])
      return ORD_GT;
    if (a->n[i] < b->n[i])
      return ORD_LT;
  }

  return ORD_EQ;
}

ord_t comp_num(const num_t *a, const num_t *b) {
  // a >= 0, b < 0 => a > b
  if (get_sign(a) == SIGN_POS && get_sign(b) == SIGN_NEG)
    return ORD_GT;
  // a < 0, b >= 0 => a < b
  if (get_sign(a) == SIGN_NEG && get_sign(b) == SIGN_POS)
    return ORD_LT;
  // a >= 0, b >= 0
  if (get_sign(a) == SIGN_POS && get_sign(b) == SIGN_POS)
    return comp_num_nat(a, b);
  // a < 0, b < 0
  if (get_sign(a) == SIGN_NEG && get_sign(b) == SIGN_NEG) {
    return comp_num_nat(b, a);
  }

  return ORD_EQ;
}

static stat_t add_num_nat(const num_t *a, const num_t *b, num_t *out) {
  int32_t len = max(a->len, b->len);

  for (int32_t i = 0; i < len; i++) {
    out->n[i] = index_or_zero(a, i) + index_or_zero(b, i);
  }

  out->len = len;

  return fix_num(out);
}

static stat_t sub_num_nat(const num_t *a, const num_t *b, num_t *out) {
  int32_t len = max(a->len, b->len);

  for (int32_t i = 0; i < len; i++) {
    out->n[i] = index_or_zero(a, i) - index_or_zero(b, i);
  }

  out->len = len;

  return fix_num(out);
}

static stat_t mul_num_nat(const num_t *a, const num_t *b, num_t *out) {
  num_t tmp = ZERO_NUM;
  tmp.len = a->len + b->len - 1;

  for (int32_t i = 0; i < b->len; i++) {
    for (int32_t j = 0; j < a->len; j++) {
      tmp.n[i + j] += a->n[j] * b->n[i];
    }
  }

  copy_num(&tmp, out);

  return fix_num(out);
}

static stat_t div_num_single_nat(const num_t *a, const num_t *b, num_t *div,
                                 num_t *mod) {
  if (b->n[0] == 0)
    return STAT_ZERO_DIV;

  int64_t carry = 0;

  for (int32_t i = a->len - 1; i >= 0; i--) {
    int64_t val = carry * NUM_BASE + a->n[i];
    div->n[i] = val / b->n[0];
    carry = val % b->n[0];
  }

  div->len = a->len;
  if (div->n[div->len - 1] == 0 && div->len != 1)
    div->len--;

  mod->len = 1;
  mod->n[0] = carry;

  return STAT_OK;
}

static stat_t div_num_nat(const num_t *a, const num_t *b, num_t *div,
                          num_t *mod) {
  if (b->len == 1)
    return div_num_single_nat(a, b, div, mod);

  if (is_zero(b))
    return STAT_ZERO_DIV;

  num_t tmp = ZERO_NUM;

  copy_num(a, mod);
  while (true) {
    if (comp_num_nat(mod, b) == ORD_LT)
      break;

    guard_stat(sub_num_nat(mod, b, mod));

    guard_stat(increment_num(&tmp, &tmp));
  }

  copy_num(&tmp, div);

  return STAT_OK;
}

stat_t add_num(const num_t *a, const num_t *b, num_t *out) {
  // a >= 0, b < 0 => a - |b|
  if (get_sign(a) == SIGN_POS && get_sign(b) == SIGN_NEG) {
    // |a| > |b| => a - |b|
    if (comp_num_nat(a, b) == ORD_GT) {
      stat_t stat = sub_num_nat(a, b, out);
      set_sign(out, SIGN_POS);
      return stat;
    }
    // |a| < |b| => -(a - |b|)
    else {
      stat_t stat = sub_num_nat(b, a, out);
      set_sign(out, SIGN_NEG);
      return stat;
    }
  }
  // a < 0, b >= 0 => b - |a|
  if (get_sign(a) == SIGN_NEG && get_sign(b) == SIGN_POS) {
    // b > |a| => b - |a|
    if (comp_num_nat(b, a) == ORD_GT) {
      stat_t stat = sub_num_nat(b, a, out);
      set_sign(out, SIGN_POS);
      return stat;
    }
    // b < |a| => -(|a| - b)
    else {
      stat_t stat = sub_num_nat(a, b, out);
      set_sign(out, SIGN_NEG);
      return stat;
    }
  }
  // a < 0, b < 0 => -(|a| + |b|)
  if (get_sign(a) == SIGN_NEG && get_sign(b) == SIGN_NEG) {
    stat_t stat = add_num_nat(a, b, out);
    set_sign(out, SIGN_NEG);
    return stat;
  }

  stat_t stat = add_num_nat(a, b, out);
  set_sign(out, SIGN_POS);
  return stat;
}

stat_t sub_num(const num_t *a, const num_t *b, num_t *out) {
  // a >= 0, b < 0 => a + |b|
  if (get_sign(a) == SIGN_POS && get_sign(b) == SIGN_NEG) {
    stat_t stat = add_num_nat(a, b, out);
    set_sign(out, SIGN_POS);
    return stat;
  }
  // a < 0, b >= 0 => -(|a| + b)
  if (get_sign(a) == SIGN_NEG && get_sign(b) == SIGN_POS) {
    stat_t stat = add_num_nat(a, b, out);
    set_sign(out, SIGN_NEG);
    return stat;
  }
  // a < 0, b < 0 => |b| - |a|
  if (get_sign(a) == SIGN_NEG && get_sign(b) == SIGN_NEG) {
    // |b| > |a| => |b| - |a|
    if (comp_num_nat(b, a) == ORD_GT) {
      stat_t stat = sub_num_nat(b, a, out);
      set_sign(out, SIGN_POS);
      return stat;
    }
    // |b| < |a| => -(|a| - |b|)
    else {
      stat_t stat = sub_num_nat(a, b, out);
      set_sign(out, SIGN_NEG);
      return stat;
    }
  }
  // a < b => -(b - a)
  if (comp_num(a, b) == ORD_LT) {
    stat_t stat = sub_num_nat(b, a, out);
    set_sign(out, SIGN_NEG);
    return stat;
  }

  stat_t stat = sub_num_nat(a, b, out);
  set_sign(out, SIGN_POS);
  return stat;
}

stat_t mul_num(const num_t *a, const num_t *b, num_t *out) {
  // a >= 0, b < 0 => -(a * |b|)
  if (get_sign(a) == SIGN_POS && get_sign(b) == SIGN_NEG) {
    stat_t stat = mul_num_nat(a, b, out);
    set_sign(out, SIGN_NEG);
    return stat;
  }
  // a < 0, b >= 0 => -(|a| * |b|)
  if (get_sign(a) == SIGN_NEG && get_sign(b) == SIGN_POS) {
    stat_t stat = mul_num_nat(a, b, out);
    set_sign(out, SIGN_NEG);
    return stat;
  }
  // a < 0, b < 0 => |a| * |b|
  if (get_sign(a) == SIGN_NEG && get_sign(b) == SIGN_NEG) {
    stat_t stat = mul_num_nat(a, b, out);
    set_sign(out, SIGN_POS);
    return stat;
  }

  stat_t stat = mul_num_nat(a, b, out);
  set_sign(out, SIGN_POS);
  return stat;
}

stat_t div_num(const num_t *a, const num_t *b, num_t *div, num_t *mod) {
  // a >= 0, b < 0 => -(a / |b|), a % |b|
  if (get_sign(a) == SIGN_POS && get_sign(b) == SIGN_NEG) {
    stat_t stat = div_num_nat(a, b, div, mod);
    set_sign(div, SIGN_NEG);
    return stat;
  }
  // a < 0, b >= 0 => -(|a| / b), -(|a| % b)
  if (get_sign(a) == SIGN_NEG && get_sign(b) == SIGN_POS) {
    stat_t stat = div_num_nat(a, b, div, mod);
    set_sign(div, SIGN_NEG);
    set_sign(mod, SIGN_NEG);
    return stat;
  }
  // a < 0, b < 0 => |a| / |b|, -(|a| % |b|)
  if (get_sign(a) == SIGN_NEG && get_sign(b) == SIGN_NEG) {
    stat_t stat = div_num_nat(a, b, div, mod);
    set_sign(mod, SIGN_NEG);
    return stat;
  }

  stat_t stat = div_num_nat(a, b, div, mod);
  set_sign(div, SIGN_POS);
  set_sign(mod, SIGN_POS);
  return stat;
}

stat_t increment_num(const num_t *in, num_t *out) {
  return add_num(in, &ONE_NUM, out);
}

stat_t pow_num(const num_t *a, int64_t b, num_t *out) {
  num_t pow;
  copy_num(a, &pow);

  guard_stat(set_int(1, out));
  while (b > 0) {
    if (b & 1) {
      guard_stat(mul_num(out, &pow, out));
    }
    guard_stat(mul_num(&pow, &pow, &pow));
    b >>= 1;
  }

  return STAT_OK;
}

stat_t calc_sqrt2_inv(int32_t digit, num_t *out) {
  num_t x = ZERO_NUM;
  num_t two = ZERO_NUM;
  num_t three = ZERO_NUM;

  x.len = digit + 1;
  x.n[digit] = 1;

  two.n[0] = 2;

  three.len = digit + 1;
  three.n[digit] = 3;

  while (true) {
    num_t new_x;

    num_t tmp;
    guard_stat(mul_num(&x, &x, &tmp));
    guard_stat(mul_num(&tmp, &two, &tmp));
    guard_stat(shift_right(&tmp, &tmp, digit));

    guard_stat(sub_num(&three, &tmp, &tmp));

    guard_stat(mul_num(&tmp, &x, &tmp));

    guard_stat(bit_shift_right(&tmp, &new_x, digit * NUM_BASE_POW_2 + 1));

    if (comp_num(&x, &new_x) == ORD_EQ)
      break;

    copy_num(&new_x, &x);
  }

  int32_t n = (int32_t)ceil(log10(NUM_BASE) * digit);
  num_t ten;
  guard_stat(set_int(10, &ten));
  num_t pow;
  guard_stat(pow_num(&ten, n, &pow));

  guard_stat(mul_num(&x, &pow, &x));
  guard_stat(shift_right(&x, out, digit));
  set_sign(out, SIGN_POS);
  return STAT_OK;
}
