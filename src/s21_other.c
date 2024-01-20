#include "s21_decimal.h"

int s21_floor(s21_decimal decimal, s21_decimal *result) {
  int error = (result) ? s21_rounding_logic(&decimal, s21_floor_callback) : 1;
  if (result) *result = decimal;
  return error;
}

int s21_floor_callback(s21_decimal *decimal, u_short sign, u_int exp) {
  int error = 0;
  s21_decimal remainder =
      s21_div_mantissa(*decimal, decimal_pow_of_ten[exp], decimal);
  if (sign && !s21_decimal_is_zero(remainder))
    error = s21_add_mantissa(*decimal, decimal_pow_of_ten[0], decimal);
  return error;
}

int s21_truncate(s21_decimal decimal, s21_decimal *result) {
  int error =
      (result) ? s21_rounding_logic(&decimal, s21_truncate_callback) : 1;
  if (result) *result = decimal;
  return error;
}

int s21_truncate_callback(s21_decimal *decimal, u_short sign, u_int exp) {
  int error = 0;
  s21_div_mantissa(*decimal, decimal_pow_of_ten[exp], decimal);
  return error;
}

int s21_round(s21_decimal decimal, s21_decimal *result) {
  int error = (result) ? s21_rounding_logic(&decimal, s21_round_callback) : 1;
  if (result) *result = decimal;
  return error;
}

int s21_rounding_logic(s21_decimal *decimal,
                       int (*callback)(s21_decimal *, u_short, u_int)) {
  u_int exp = s21_extract_degree(*decimal);
  u_short sign = s21_get_sign(*decimal);
  int error = callback(decimal, sign, exp);
  s21_set_sign(decimal, sign);
  return error;
}

int s21_round_callback(s21_decimal *decimal, u_short sign, u_int exp) {
  int error = 0;
  u_int tmp_rem = 0;
  for (u_int i = 0; i < exp; i++) {
    s21_decimal remainder =
        s21_div_mantissa(*decimal, decimal_pow_of_ten[1], decimal);
    tmp_rem = ((remainder.bits[0] + tmp_rem) >= 5);
  }
  if (tmp_rem)
    error = s21_add_mantissa(*decimal, decimal_pow_of_ten[0], decimal);
  return error;
}

int s21_negate(s21_decimal decimal, s21_decimal *result) {
  int error = 0;
  if (result) {
    s21_set_sign(&decimal, !s21_get_sign(decimal));
    *result = decimal;
  } else {
    error = 1;
  }
  return error;
}

u_short s21_add_mantissa(s21_decimal decimal_1, s21_decimal decimal_2,
                         s21_decimal *result) {
  u_short remainder = 0;

  s21_zeroing_decimal(result);

  for (int i = 0; i < 3; i++) {
    u_int old_remainder = remainder;

    remainder =
        s21_add_int(decimal_1.bits[i], decimal_2.bits[i], &result->bits[i]);

    remainder += s21_add_int(result->bits[i], old_remainder, &result->bits[i]);
  }

  return remainder;
}

u_short s21_sub_mantissa(s21_decimal decimal_1, s21_decimal decimal_2,
                         s21_decimal *result) {
  s21_zeroing_decimal(result);

  return (s21_compare_mantissa(decimal_1, decimal_2) == 3)
             ? s21_sub_mantissa_logic(decimal_2, decimal_1, result) + 1
             : s21_sub_mantissa_logic(decimal_1, decimal_2, result);
}

u_short s21_sub_mantissa_logic(s21_decimal decimal_1, s21_decimal decimal_2,
                               s21_decimal *result) {
  s21_inverse_mantissa(&decimal_1);
  s21_add_mantissa(decimal_1, decimal_2, result);
  s21_inverse_mantissa(result);

  return 0;
}

s21_decimal s21_div_mantissa(s21_decimal decimal_1, s21_decimal decimal_2,
                             s21_decimal *result) {
  s21_zeroing_decimal(result);

  return (s21_compare_mantissa(decimal_1, decimal_2) == 3)
             ? decimal_1
             : s21_div_mantissa_logic(decimal_1, decimal_2, result);
}

s21_decimal s21_div_mantissa_logic(s21_decimal decimal_1, s21_decimal decimal_2,
                                   s21_decimal *result) {
  s21_decimal start_decimal_2 = decimal_2;
  while (s21_compare_mantissa(decimal_1, start_decimal_2) != 3) {
    u_short exp = 0;
    s21_decimal tmp = decimal_2 = start_decimal_2;
    s21_decimal tmp_result = {{1, 0, 0, 0}};

    for (u_short err = 0; s21_compare_mantissa(decimal_1, tmp) != 3 && !err;
         exp++) {
      decimal_2 = tmp;
      err = s21_left_shift(&tmp);
    }
    for (; exp > 1; exp--) {
      s21_left_shift(&tmp_result);
    }

    s21_add_mantissa(*result, tmp_result, result);
    s21_sub_mantissa(decimal_1, decimal_2, &decimal_1);
  }
  return decimal_1;
}

u_short s21_left_shift(s21_decimal *decimal) {
  st_overflow overflow = {0};
  for (int i = 0; i < 3; i++) {
    st_overflow tmp = overflow;
    overflow.shifts = 0;

    s21_get_shift_left_overflow(decimal->bits[i], 1, &overflow);
    s21_add_int(decimal->bits[i] << 1, tmp.shifts, &decimal->bits[i]);
  }
  return overflow.shifts;
}

u_short s21_add_int(u_int value_1, u_int value_2, u_int *result) {
  u_short remainder = 0;
  *result = 0;

  for (int i = 0; i < 32; i++) {
    u_short bit_1 = s21_get_bit_int(value_1, i);
    u_short bit_2 = s21_get_bit_int(value_2, i);

    s21_set_bit_int(result, remainder ^ bit_1 ^ bit_2, i);

    remainder = (remainder + bit_1 + bit_2 > 1);
  }

  return remainder;
}

u_short s21_compare_mantissa(s21_decimal decimal_1, s21_decimal decimal_2) {
  u_short result = 1;
  for (int i = 95; i >= 0 && result == 1; i--) {
    u_short bit_1 = s21_get_bit(decimal_1, i);
    u_short bit_2 = s21_get_bit(decimal_2, i);
    result = s21_compare_bits(bit_1, bit_2);
  }
  return result;
}

u_short s21_get_sign(s21_decimal decimal) {
  return s21_get_bit(decimal, SIGN_POSITION);
}

void s21_set_sign(s21_decimal *decimal, u_short sign) {
  s21_set_bit(decimal, sign, SIGN_POSITION);
}

u_short s21_get_bit(s21_decimal decimal, u_short position) {
  return ((ARR_NUMBER(position)) > 3)
             ? 2
             : 1 & (decimal.bits[ARR_NUMBER(position)] >> (position % 32));
}

u_short s21_set_bit(s21_decimal *decimal, u_short bit, u_short position) {
  return (position > 127) ? 2
         : (bit) ? (decimal->bits[ARR_NUMBER(position)] |= (1 << position))
                 : (decimal->bits[ARR_NUMBER(position)] &= ~(1 << position));
}

u_short s21_get_bit_int(u_int value, u_short position) {
  return (1 & (value >> position));
}

void s21_set_bit_int(u_int *value, u_short bit, u_short position) {
  (bit) ? (*value |= (1 << position)) : (*value &= ~(1 << position));
}

u_short s21_compare_bits(u_short bit_1, u_short bit_2) {
  return (bit_1 < bit_2) ? 3 : (bit_1 > bit_2) ? 2 : (bit_1 == bit_2);
}

void s21_inverse_mantissa(s21_decimal *decimal) {
  for (int i = 0; i < 3; i++) {
    decimal->bits[i] = ~decimal->bits[i];
  }
}

void s21_zeroing_decimal(s21_decimal *result) {
  for (int i = 0; i < 4; i++) {
    result->bits[i] = 0;
  }
}

u_short s21_decimal_is_zero(s21_decimal decimal) {
  return (!decimal.bits[0] && !decimal.bits[1] && !decimal.bits[2]);
}

void s21_get_shift_left_overflow(u_int value, u_short position,
                                 st_overflow *overflow) {
  u_int tmp = 0;
  for (int i = 31; i >= 31 - position + 1; i--) {
    tmp <<= 1;
    s21_set_bit_int(&tmp, s21_get_bit_int(value, i), 0);
  }
  s21_add_int(overflow->shifts, tmp, &overflow->shifts);
}

u_int s21_extract_degree(s21_decimal decimal) {
  s21_degree_transformer_down(&decimal);
  return decimal.bits[3];
}

void s21_degree_transformer_down(s21_decimal *decimal) {
  s21_set_sign(decimal, SIGN_POSITIVE);
  decimal->bits[3] >>= EXPONENT_POSITION;
}