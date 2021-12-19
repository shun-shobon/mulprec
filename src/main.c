#include <stdint.h>
#include <stdio.h>
#include <sys/time.h>

#include "mulprec.h"

#define N 3400
#define SHIFT 130

static num_t x_memo[N + 1];
static num_t delta_x_memo[N + 1];
static num_t nck_memo[N / 2];

num_t *get_nck(int32_t n, int32_t k) {
  if (n / 2 < k) {
    return &nck_memo[n - k];
  } else {
    return &nck_memo[k];
  }
}

void update_nck(int32_t n) {
  if (n == 0) {
    set_int(1, &nck_memo[0]);
    return;
  }

  num_t old = nck_memo[0];
  int32_t loop = (n / 2) + 1;
  for (int32_t i = 1; i < loop; i++) {
    num_t new;
    if (n % 2 != 0 || n / 2 != i) {
      add_num(&old, get_nck(n - 1, i), &new);
      copy_num(get_nck(n - 1, i), &old);
    } else {
      add_num(&old, &old, &new);
    }

    copy_num(&new, get_nck(n, i));
  }
}

void calc_x(int32_t n, num_t *out) {
  num_t upper = ZERO_NUM;
  upper.len = SHIFT + 1;
  upper.n[SHIFT] = 1;

  num_t lower = ZERO_NUM;
  num_t tmp;

  num_t left;
  lower.n[0] = 4 * n + 1;
  div_num(&upper, &lower, &left, &tmp);

  num_t right;
  lower.n[0] = 4 * n + 3;
  div_num(&upper, &lower, &right, &tmp);

  add_num(&left, &right, out);
}

void calc_delta_x(int32_t n, num_t *out) {
  calc_x(n, &x_memo[n]);

  if (n == 0) {
    copy_num(&x_memo[0], &delta_x_memo[0]);
    copy_num(&x_memo[0], out);
    return;
  }

  update_nck(n - 1);

  num_t sum = ZERO_NUM;
  for (int32_t i = 0; i < n; i++) {
    num_t *coefficient = get_nck(n - 1, i);

    num_t item;
    mul_num(coefficient, &x_memo[i + 1], &item);

    if (n % 2 != 0) {
      if (i % 2 == 0) {
        add_num(&sum, &item, &sum);
      } else {
        sub_num(&sum, &item, &sum);
      }
    } else {
      if (i % 2 == 0) {
        sub_num(&sum, &item, &sum);
      } else {
        add_num(&sum, &item, &sum);
      }
    }
  }

  sub_num(&sum, &delta_x_memo[n - 1], &delta_x_memo[n]);
  copy_num(&delta_x_memo[n], out);
}

int main(void) {
  struct timeval start;
  gettimeofday(&start, NULL);

  num_t two;
  set_int(2, &two);

  num_t sqrt2_inv;
  calc_sqrt2_inv(SHIFT, &sqrt2_inv);

  num_t coefficient;
  mul_num(&two, &sqrt2_inv, &coefficient);

  num_t sum = ZERO_NUM;
  for (int32_t i = 0; i < N; i++) {
    if (((i + 1) * 100) % N == 0) {
      fprintf(stderr, "\r計算中... %d%%", ((i + 1) * 100) / N);
      fflush(stderr);
    }

    num_t ans;
    calc_delta_x(i, &ans);

    num_t tmp_div, tmp_mod;
    for (int32_t j = 0; j < i; j++) {
      div_num(&ans, &two, &tmp_div, &tmp_mod);
      copy_num(&tmp_div, &ans);
    }

    if (i % 2 == 0) {
      add_num(&sum, &ans, &sum);
    } else {
      sub_num(&sum, &ans, &sum);
    }
  }

  num_t pi;
  mul_num(&coefficient, &sum, &pi);

  shift_right(&pi, &pi, SHIFT);

  fprintf(stderr, "\n実行完了\n");

  print_num(&pi);
  printf("\n");

  struct timeval end;
  gettimeofday(&end, NULL);

  double time = (double)(end.tv_sec - start.tv_sec) +
                ((end.tv_usec - start.tv_usec) / 100000.0);
  fprintf(stderr, "実行時間: %.6lf秒\n", time);

  return 0;
}
