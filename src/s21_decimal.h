#ifndef SRC_S21_DECIMAL_H
#define SRC_S21_DECIMAL_H

#include <float.h>
#include <limits.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "s21_consts.h"

typedef enum {
  without_error = 0,
  too_big_number = 1,
  too_small_number = 2,
  div_null = 3
} arithmetic_errors;

typedef enum { false_comparison = 0, true_comparison = 1 } comparison;
typedef enum { ok_convert = 0, error_convert = 1 } convertation;
typedef enum { ok_other = 0, error_other = 1 } other;

// арифметические операторы
int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
int s21_sub(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
int s21_mul(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
int s21_div(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
int s21_mod(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);

// операторы сравнения
int s21_is_less(s21_decimal value_1, s21_decimal value_2);
int s21_is_less_or_equal(s21_decimal value_1, s21_decimal value_2);
int s21_is_greater(s21_decimal value_1, s21_decimal value_2);
int s21_is_greater_or_equal(s21_decimal value_1, s21_decimal value_2);
int s21_compare(s21_decimal value_1, s21_decimal value_2);
int s21_is_not_equal(s21_decimal value_1, s21_decimal value_2);
int s21_is_equal(s21_decimal value_1, s21_decimal value_2);

// преобразователи
int s21_from_int_to_decimal(int src, s21_decimal *dst);
int s21_from_float_to_decimal(float src, s21_decimal *dst);
int s21_from_decimal_to_int(s21_decimal src, int *dst);
int s21_from_decimal_to_float(s21_decimal src, float *dst);

//другие функции
int s21_round(s21_decimal decimal, s21_decimal *result);
int s21_floor(s21_decimal decimal, s21_decimal *result);
int s21_truncate(s21_decimal decimal, s21_decimal *result);
int s21_negate(s21_decimal decimal, s21_decimal *result);

// вспомогательные функции
void s21_splitFloat(float num, long int *integerPart, int *decimalPartLength);
int s21_get_exp(s21_decimal src);
void s21_zero(s21_decimal *dst);
u_short s21_add_mantissa(s21_decimal decimal_1, s21_decimal decimal_2,
                         s21_decimal *result);
u_short s21_sub_mantissa_logic(s21_decimal decimal_1, s21_decimal decimal_2,
                               s21_decimal *result);
s21_decimal s21_div_mantissa(s21_decimal decimal_1, s21_decimal decimal_2,
                             s21_decimal *result);
s21_decimal s21_div_mantissa_logic(s21_decimal decimal_1, s21_decimal decimal_2,
                                   s21_decimal *result);
u_short s21_left_shift(s21_decimal *decimal);
u_short s21_compare_mantissa(s21_decimal decimal_1, s21_decimal decimal_2);
void s21_inverse_mantissa(s21_decimal *decimal);
void s21_zeroing_decimal(s21_decimal *result);
u_short s21_decimal_is_zero(s21_decimal decimal);
u_short s21_add_int(u_int value_1, u_int value_2, u_int *result);
void s21_get_shift_left_overflow(u_int value, u_short position,
                                 st_overflow *overflow);
u_short s21_get_sign(s21_decimal decimal);
void s21_set_sign(s21_decimal *decimal, u_short sign);
u_short s21_get_bit(s21_decimal decimal, u_short position);
u_short s21_set_bit(s21_decimal *decimal, u_short bit, u_short position);
u_short s21_get_bit_int(u_int value, u_short position);
void s21_set_bit_int(u_int *value, u_short bit, u_short position);
u_short s21_compare_bits(u_short bit_1, u_short bit_2);
u_int s21_extract_degree(s21_decimal decimal);
void s21_degree_transformer_down(s21_decimal *decimal);
int s21_rounding_logic(s21_decimal *decimal,
                       int (*callback)(s21_decimal *, u_short, u_int));
int s21_truncate_callback(s21_decimal *decimal, u_short sign, u_int exp);
int s21_round_callback(s21_decimal *decimal, u_short sign, u_int exp);
int s21_floor_callback(s21_decimal *decimal, u_short sign, u_int exp);
void s21_add_main(s21_decimal value_1, s21_decimal value_2, s21_decimal *result,
                  int *flag_error);
void s21_mul_main(s21_decimal value_1, s21_decimal value_2, s21_decimal *result,
                  int *flag_error);
void s21_equalizing_scale(big_decimal *big_value_1, big_decimal *big_value_2);
void s21_div_main(s21_decimal value_1, s21_decimal value_2, s21_decimal *result,
                  int *flag_error);
int s21_srav_mantis(big_decimal value_1, big_decimal value_2);
int s21_cycle_shift(big_decimal value_1, big_decimal value_2,
                    big_decimal *result, big_decimal *remains);
void s21_set_1(big_decimal *value, int i);
void s21_sub_for_div(big_decimal value_1, big_decimal value_2,
                     big_decimal *result);
void s21_shift(big_decimal *value, int coef_shift);
int s21_big_check_zero(big_decimal value);
void s21_negative(s21_decimal value, s21_decimal *result);
int s21_sign_path(s21_decimal value_1, s21_decimal value_2);
void s21_path_1_or_2_add(big_decimal value_1, big_decimal value_2,
                         big_decimal *result);
void s21_path_3_or_4_add(big_decimal *value_1, big_decimal *value_2,
                         big_decimal *result);
int s21_sign_result(big_decimal value_1, big_decimal value_2, int path);
void s21_addZeroes(char *left, char *right);
void s21_clear(char *input, int size);
void s21_reverseString(char *string);
void s21_decimalMantisToBinary(s21_decimal value, char *binaryOutput);
int s21_getSign(int value);
int s21_getScale(int value);
void s21_baseTenCharArrayAdd(char *left, char *right, char *result);
void s21_binaryToBaseTen(char *binary, char *output);
void s21_clearCharZeroes(char *input);
int s21_postavit_tochku(int tochka, char *stroka, char *integral,
                        char *fractional);
int s21_compare(s21_decimal value_1, s21_decimal value_2);
int s21_checkSign(s21_decimal value_1, s21_decimal value_2);
int s21_checkLen(int left_len, int right_len, int both_negative);
int s21_checkNumber(const char *left_digits, const char *right_digits,
                    int both_negative);
void s21_equalizeFractional(char *left_fractional, char *right_fractional);

#endif  // SRC_S21_DECIMAL_H_
