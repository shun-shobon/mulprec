#include <stdint.h>
#include <stdio.h>
#include <sys/time.h>

#include "mulprec.h"

#define N 3350
#define SHIFT 125

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

stat_t update_nck(int32_t n) {
  if (n == 0) {
    guard_stat(set_int(1, &nck_memo[0]));
    return STAT_OK;
  }

  num_t old = nck_memo[0];
  int32_t loop = (n / 2) + 1;
  for (int32_t i = 1; i < loop; i++) {
    num_t new;
    if (n % 2 != 0 || n / 2 != i) {
      guard_stat(add_num(&old, get_nck(n - 1, i), &new));
      copy_num(get_nck(n - 1, i), &old);
    } else {
      guard_stat(add_num(&old, &old, &new));
    }

    copy_num(&new, get_nck(n, i));
  }

  return STAT_OK;
}

stat_t calc_x(int32_t n, num_t *out) {
  num_t upper = ZERO_NUM;
  upper.len = SHIFT + 1;
  upper.n[SHIFT] = 1;

  num_t lower = ZERO_NUM;
  num_t tmp;

  num_t left;
  lower.n[0] = 4 * n + 1;
  guard_stat(div_num(&upper, &lower, &left, &tmp));

  num_t right;
  lower.n[0] = 4 * n + 3;
  guard_stat(div_num(&upper, &lower, &right, &tmp));

  guard_stat(add_num(&left, &right, out));

  return STAT_OK;
}

stat_t calc_delta_x(int32_t n, num_t *out) {
  guard_stat(calc_x(n, &x_memo[n]));

  if (n == 0) {
    copy_num(&x_memo[0], &delta_x_memo[0]);
    copy_num(&x_memo[0], out);
    return STAT_OK;
  }

  guard_stat(update_nck(n - 1));

  num_t sum = ZERO_NUM;
  for (int32_t i = 0; i < n; i++) {
    num_t *coefficient = get_nck(n - 1, i);

    num_t item;
    guard_stat(mul_num(coefficient, &x_memo[i + 1], &item));

    if (n % 2 != 0) {
      if (i % 2 == 0) {
        guard_stat(add_num(&sum, &item, &sum));
      } else {
        guard_stat(sub_num(&sum, &item, &sum));
      }
    } else {
      if (i % 2 == 0) {
        guard_stat(sub_num(&sum, &item, &sum));
      } else {
        guard_stat(add_num(&sum, &item, &sum));
      }
    }
  }

  guard_stat(sub_num(&sum, &delta_x_memo[n - 1], &delta_x_memo[n]));
  copy_num(&delta_x_memo[n], out);

  return STAT_OK;
}

stat_t calc_pi(num_t *out) {
  num_t two;
  guard_stat(set_int(2, &two));

  num_t sqrt2_inv;
  guard_stat(calc_sqrt2_inv(SHIFT, &sqrt2_inv));

  num_t coefficient;
  guard_stat(mul_num(&two, &sqrt2_inv, &coefficient));

  num_t sum = ZERO_NUM;
  for (int32_t i = 0; i < N; i++) {
    if (((i + 1) * 100) % N == 0) {
      fprintf(stderr, "\r計算中... %d%%", ((i + 1) * 100) / N);
      fflush(stderr);
    }

    num_t ans;
    guard_stat(calc_delta_x(i, &ans));

    guard_stat(bit_shift_right(&ans, &ans, i));

    if (i % 2 == 0) {
      guard_stat(add_num(&sum, &ans, &sum));
    } else {
      guard_stat(sub_num(&sum, &ans, &sum));
    }
  }

  guard_stat(mul_num(&coefficient, &sum, out));

  guard_stat(shift_right(out, out, SHIFT));

  return STAT_OK;
}

int main(void) {
  struct timeval start;
  gettimeofday(&start, NULL);

  num_t pi;
  stat_t stat = calc_pi(&pi);
  if (stat != STAT_OK) {
    switch (stat) {
    case STAT_OVERFLOW:
      fprintf(stderr, "\nエラー: オーバーフロー\n");
      exit(EXIT_FAILURE);
    case STAT_ZERO_DIV:
      fprintf(stderr, "\nエラー: 0除算\n");
      exit(EXIT_FAILURE);
    case STAT_UNKNOWN:
    default:
      fprintf(stderr, "\nエラー: 不明\n");
      exit(EXIT_FAILURE);
    }
  }

  fprintf(stderr, "\n実行完了\n");

  print_num(&pi);
  printf("\n");

  struct timeval end;
  gettimeofday(&end, NULL);

  double time = (double)(end.tv_sec - start.tv_sec) +
                ((double)(end.tv_usec - start.tv_usec) / 1000000);
  fprintf(stderr, "実行時間: %.6lf秒\n", time);

  return 0;
}
