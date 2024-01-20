#include "s21_decimal.h"

/// @brief Переводит int в decimal
/// @param src Значение int
/// @param dst Значение decimal
/// @return код \n - 0 - OK \n- 1 - ошибка конвертации
int s21_from_int_to_decimal(int src, s21_decimal *dst) {
  int result = ok_convert;
  if (abs(src) > INT_MAX) {
    result = error_convert;
  }

  s21_zero(dst);

  if (src < 0) {
    dst->bits[3] = MINUS;
    src = -src;
  }

  dst->bits[0] = src;
  return result;
}

/// @brief Переводит decimal в int
/// @param src Значение decimal
/// @param dst Значение int
/// @return код \n - 0 - OK \n- 1 - ошибка конвертации
int s21_from_decimal_to_int(s21_decimal src, int *dst) {
  int result = ok_convert;
  // *dst = 0;
  int exp = s21_get_exp(src);

  if (!dst || exp > 28) {
    result = error_convert;
  }
  long double num =
      src.bits[0] + src.bits[1] * pow(2, 32) + src.bits[2] * pow(2, 64);

  if (s21_get_sign(src) == 1) {
    // src.bits[3] = MINUS;
    num = -num;
  }

  if (num >= -INT_MAX || num <= INT_MAX) {
    num /= pow(10, exp);
    *dst = (int)num;
  }
  return result;
}

/// @brief Переводит float в decimal
/// @param src Значение float
/// @param dst Значение decimal
/// @return код \n - 0 - OK \n- 1 - ошибка конвертации
int s21_from_float_to_decimal(float src, s21_decimal *dst) {
  int result = ok_convert;

  if ((fabs(src) > DBL_MAX || fabs(src) == INFINITY)) {
    // zero(dst);
    result = error_convert;
  }

  if (dst) {
    s21_zero(dst);

    long int integerPart = 0;
    int decimalPartLength = 0;
    s21_splitFloat(src, &integerPart, &decimalPartLength);
    s21_from_int_to_decimal(integerPart, dst);

    if (src < 0) {
      // src = -(src);
      dst->bits[3] = MINUS;
    }

    dst->bits[3] += decimalPartLength << 16;

  } else {
    result = error_convert;
  }

  return result;
}

/// @brief Переводит decimal в float
/// @param src Значение decimal
/// @param dst Значение float
/// @return код \n - 0 - OK \n- 1 - ошибка конвертации
int s21_from_decimal_to_float(s21_decimal src, float *dst) {
  int result = ok_convert;
  // *dst = 0;
  int exp = s21_get_exp(src);

  if (!dst || exp > 28) {
    result = error_convert;
  }
  long double num =
      src.bits[0] + src.bits[1] * pow(2, 32) + src.bits[2] * pow(2, 64);

  if (s21_get_sign(src) == 1) {
    // src.bits[3] = MINUS;
    num = -num;
  }

  if (num >= -FLT_MAX || num <= FLT_MAX) {
    num /= pow(10, exp);
    *dst = (float)num;
  }
  return result;
}

/// @brief Получаем значение экспоненты
/// @param src Значение decimal
/// @return Значение экспоненты
int s21_get_exp(s21_decimal src) { return (src.bits[3] >> 16) & 0x7FFF; }

/// @brief Вычисление целой и дробной части числа
/// @param num Число
/// @param integerPart Целая часть
/// @param decimalPartLength Дробная часть
void s21_splitFloat(float num, long int *integerPart, int *decimalPartLength) {
  *integerPart = (long int)num;
  *decimalPartLength = 0;

  while (
      num - ((float)(*integerPart) / (long int)(pow(10, *decimalPartLength))) !=
      0) {
    (*decimalPartLength)++;
    *integerPart = num * (long int)(pow(10, *decimalPartLength));
  }
}

/// @brief Обнуление битов decimal
/// @param dst Значение для обнуления
void s21_zero(s21_decimal *dst) {
  dst->bits[0] = dst->bits[1] = dst->bits[2] = dst->bits[3] = 0;
}