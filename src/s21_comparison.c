#include "s21_comparison.h"

#include "s21_decimal.h"

/// @brief заполняет сртроку знаками конца строки
/// @param input строка, которую нужно занулить
/// @param size размер строки
void s21_clear(char* input, int size) {
  for (int i = 0; i < size; ++i) {
    input[i] = 0;
  }
}

/// @brief добавляет нули в конец строки
/// @param left строка, поступившая как сравневоемое значение
/// @param right строка, поступившая как значение с которым сравниват
void s21_addZeroes(char* left, char* right) {
  int len_l = strlen(left), len_r = strlen(right);
  char string_left[SIZE_ONE], string_right[SIZE_ONE];
  s21_clear(string_left, SIZE_ONE);
  s21_clear(string_right, SIZE_ONE);
  for (int i = 0; i < SIZE_ONE - len_l - 1; i++) strcat(string_left, "0");
  for (int i = 0; i < SIZE_ONE - len_r - 1; i++) strcat(string_right, "0");
  strcat(string_left, left);
  strcat(string_right, right);
  strcpy(left, string_left);
  strcpy(right, string_right);
}

/// @brief разворачивает строку
/// @param string строка
void s21_reverseString(char* string) {
  int i = 0, last_char = strlen(string) - 1;
  while (i < last_char) {
    char temp = string[i];
    string[i] = string[last_char];
    string[last_char] = temp;
    i++;
    last_char--;
  }
}

/// @brief Приводит децимал к бинарному виду
/// @param value децимал
/// @param binaryOutput строка в бинароном виде
void s21_decimalMantisToBinary(s21_decimal value, char* binaryOutput) {
  char binaryLocal[SIZE_BIN];
  s21_clear(binaryLocal, SIZE_BIN);
  for (int countBit = 0; countBit <= 2; countBit++) {
    char stack[SIZE_THRD];
    s21_clear(stack, SIZE_THRD);
    for (int i = 0; i < SIZE_THRD - 1; ++i) stack[i] = '0';
    int i = 0;
    while (value.bits[countBit]) {
      if (value.bits[countBit] % 2 == 1) {
        stack[i] = '1';
      } else {
        stack[i] = '0';
      }
      ++i;
      value.bits[countBit] /= 2;
    }
    strcat(binaryLocal, stack);
  }
  s21_reverseString(binaryLocal);
  binaryLocal[SIZE_BIN - 1] = 0;
  strcpy(binaryOutput, binaryLocal);
}

/// @brief Находит знак + или -
/// @param value 3 bits из которого берется знак
/// @return возвращает 0 если +, 1 если -
int s21_getSign(int value) {
  unsigned char intToChars[4];
  int sign = 0;
  memcpy(intToChars, &value, sizeof(char) * 4);
  intToChars[3] == 0 ? (sign = 0) : (sign = 1);
  return sign;
}

/// @brief Ищет колличество знаков после запятой
/// @param value 3 bits из которого берется положение запятой
/// @return возвращает число знаков
int s21_getScale(int value) {
  unsigned char intToChars[4];
  int scale = 0;
  memcpy(intToChars, &value, sizeof(char) * 4);
  scale = intToChars[2];
  return scale;
}

/// @brief Сложение двух десятичных строк
/// @param left строка, поступившая как сравневоемое значение
/// @param right строка, поступившая как значение с которым сравнивать
/// @param result строка, которая получилась после сложения
void s21_baseTenCharArrayAdd(char* left, char* right, char* result) {
  s21_addZeroes(left, right);
  short_int result_local[SIZE_ONE] = {0};
  s21_clear(result_local, SIZE_ONE);
  for (int i = 0; i < SIZE_ONE - 1; i++) {
    result_local[i] = (left[i] - '0') + (right[i] - '0');
  }
  for (int i = SIZE_ONE - 1; i > 0; i--) {
    if (result_local[i] > 9) {
      result_local[i - 1] += 1;
      result_local[i] -= 10;
    }
  }
  int started = 0, j = 0;
  for (int i = 0; i < SIZE_ONE - 1; ++i) {
    if (result_local[i] != 0) started = 1;
    if (started) {
      result[j] = result_local[i] + '0';
      ++j;
    }
  }
}

/// @brief Преобразование из двоичной системы в десятичную
/// @param binary строка в двоичном виде
/// @param output получившаяся десятичная строка
void s21_binaryToBaseTen(char* binary, char* output) {
  s21_reverseString(binary);
  char sum[SIZE_ONE];
  s21_clear(sum, SIZE_ONE);
  strcpy(sum, "0");
  for (int i = 0; i < SIZE_BIN; ++i) {
    if (binary[i] == '1') {
      char sila_dvuh_local[OVER_SIZE];
      s21_clear(sila_dvuh_local, OVER_SIZE);
      strcpy(sila_dvuh_local, sily_dvuh[i]);
      char sum_temp[SIZE_ONE];
      s21_clear(sum_temp, SIZE_ONE);
      s21_baseTenCharArrayAdd(sum, sila_dvuh_local, sum_temp);
      strcpy(sum, sum_temp);
    }
  }
  strcpy(output, sum);
}

/// @brief Отделяет целую часть числа от дробной
/// @param tochka количество знаков после запятой
/// @param stroka десятичная строка
/// @param integral целая часть
/// @param fractional дробная часть
/// @return возвращает 1, если колличество знаков после запятой не входит в
/// диапозон от 0 до 28, иначе 0
int s21_postavit_tochku(int tochka, char* stroka, char* integral,
                        char* fractional) {
  int error = 1;
  if (tochka >= 0 && tochka <= 28) {
    error = 0;
    char temp[SIZE_BTEN];
    s21_clear(temp, SIZE_BTEN);
    int len = strlen(stroka);
    if (len > 30) printf("\nPEREVAL V STROKE!\n");
    if (30 - 1 - len < 0) printf("\nSCHAS BUDET SEGFAULT!!\n");
    for (int i = 0; i < 30 - 1 - len; i++) strcat(temp, "0");
    strcat(temp, stroka);
    char temp2[SIZE_BTEN];
    s21_clear(temp2, SIZE_BTEN);
    strncat(temp2, temp, 29 - tochka);
    strcat(temp2, ".");
    strcat(temp2, temp + 29 - tochka);

    strncpy(integral, temp, 29 - tochka);
    strcpy(fractional, temp + 29 - tochka);

    s21_clearCharZeroes(integral);

    strcpy(stroka, temp2);
  }
  return error;
}

/// @brief Удаляет нули из начала целой части
/// @param input строка из которой нужно удалить нули
void s21_clearCharZeroes(char* input) {
  int started = 0;
  char temp[SIZE_BTEN];
  s21_clear(temp, SIZE_BTEN);
  for (int i = 0; input[i] != '\0'; i++) {
    if (input[i] != '0') started = 1;
    if (started) {
      char temp_for_one_char[2];
      temp_for_one_char[0] = input[i];
      temp_for_one_char[1] = '\0';
      strcat(temp, temp_for_one_char);
    }
  }
  strcpy(input, temp);
}
/// @brief Сравнивает знаки
/// @param value_1 первый децимал
/// @param value_2 второй децимал
/// @return если знаки одинаковые возвращает kEqual, если знаки отличаются, то
/// возвращает либо kLess, либо kGreater
int s21_checkSign(s21_decimal value_1, s21_decimal value_2) {
  int result = kEqual;
  int left_sign = s21_getSign(value_1.bits[3]);
  int right_sign = s21_getSign(value_2.bits[3]);
  int decimal_mantis_sum = value_1.bits[0] + value_1.bits[1] + value_1.bits[2] +
                           value_2.bits[0] + value_2.bits[1] + value_2.bits[2];
  if (left_sign > right_sign) result = kLess;
  if (left_sign < right_sign) result = kGreater;
  if (decimal_mantis_sum == 0) result = kEqual;
  return result;
}

/// @brief Сравнивает длинну стоки
/// @param left_len длинна первой сравнивоемой строки
/// @param right_len длина второй сравниваемой строки
/// @param both_negative 0 или 1 в зависимости от того, являются ли оба децимала
/// отрицательными
/// @return если строки одинаковые возвращает kEqual, если строки отличаются, то
/// возвращает либо kLess, либо kGreater, меняет значения возвращаемых значений,
/// если оба числа отрицательные
int s21_checkLen(int left_len, int right_len, int both_negative) {
  int result = kEqual;
  if (left_len < right_len) {
    if (!both_negative) result = kLess;
    if (both_negative) result = kGreater;
  } else if (left_len > right_len) {
    if (!both_negative) result = kGreater;
    if (both_negative) result = kLess;
  }
  return result;
}

/// @brief Сравнивает числа
/// @param left_digits первая сравнивоемая строка
/// @param right_digits вторая сравнивоемая строка
/// @param both_negative 0 или 1 в зависимости от того, являются ли оба децимала
/// отрицательными
/// @return если все цифры одинаковые возвращает kEqual, если строки отличаются,
/// то возвращает либо kLess, либо kGreater, меняет значения возвращаемых
/// значений, если оба числа отрицательные
int s21_checkNumber(const char* left_digits, const char* right_digits,
                    int both_negative) {
  int result = kEqual;
  int loop_break = 0;
  for (int i = 0; left_digits[i] != '\0' && loop_break == 0; ++i) {
    if (left_digits[i] < right_digits[i]) {
      if (!both_negative) result = kLess;
      if (both_negative) result = kGreater;
      loop_break = 1;
    } else if (left_digits[i] > right_digits[i]) {
      if (!both_negative) result = kGreater;
      if (both_negative) result = kLess;
      loop_break = 1;
    }
  }
  return result;
}

/// @brief Добавляет нули в конец дробной части, чтобы сравнять их
/// @param left_fractional первая дробная часть
/// @param right_fractional вторая дроная часть
void s21_equalizeFractional(char* left_fractional, char* right_fractional) {
  int left_len = strlen(left_fractional);
  int right_len = strlen(right_fractional);
  if (left_len > right_len) {
    for (int i = right_len; i < left_len; i++) strcat(right_fractional, "0");
  } else if (left_len < right_len)
    for (int i = left_len; i < right_len; i++) strcat(left_fractional, "0");
}

/// @brief Все сравнение и преобразование происходит здесь
/// @param value_1 первый децимал
/// @param value_2 второй децимал
/// @return если все цифры одинаковые возвращает kEqual, если строки отличаются,
/// то возвращает либо kLess, либо kGreater, меняет значения возвращаемых
/// значений, если оба числа отрицательные, возвращает kError, если что-то пошло
/// не так
int s21_compare(s21_decimal value_1, s21_decimal value_2) {
  int result = 0;
  char left_binary_string[SIZE_BIN], right_binary_string[SIZE_BIN];
  s21_clear(left_binary_string, SIZE_BIN);
  s21_clear(right_binary_string, SIZE_BIN);
  char left_base_ten_string[SIZE_ONE], right_base_ten_string[SIZE_ONE];
  s21_clear(left_base_ten_string, SIZE_ONE);
  s21_clear(right_base_ten_string, SIZE_ONE);
  int both_neg = 0;

  int left_sign = s21_getSign(value_1.bits[3]);
  int right_sign = s21_getSign(value_2.bits[3]);
  if ((left_sign + right_sign) == 2) both_neg = 1;

  int left_scale = s21_getScale(value_1.bits[3]);
  int right_scale = s21_getScale(value_2.bits[3]);
  if ((left_scale >= 0 && left_scale <= 28) &&
      (right_scale >= 0 && right_scale <= 28)) {
    s21_decimalMantisToBinary(value_1, left_binary_string);
    s21_decimalMantisToBinary(value_2, right_binary_string);
    s21_binaryToBaseTen(left_binary_string, left_base_ten_string);
    s21_binaryToBaseTen(right_binary_string, right_base_ten_string);
    char l_integral[SIZE_BTEN], r_integral[SIZE_BTEN];
    s21_clear(l_integral, SIZE_BTEN);
    s21_clear(r_integral, SIZE_BTEN);
    char l_fractional[SIZE_BTEN], r_fractional[SIZE_BTEN];
    s21_clear(l_fractional, SIZE_BTEN);
    s21_clear(r_fractional, SIZE_BTEN);
    s21_postavit_tochku(left_scale, left_base_ten_string, l_integral,
                        l_fractional);
    s21_postavit_tochku(right_scale, right_base_ten_string, r_integral,
                        r_fractional);
    int l_integr_len = strlen(l_integral);
    int r_integr_len = strlen(r_integral);
    if (s21_checkSign(value_1, value_2) == kLess) {
      result = kLess;
    } else if (s21_checkSign(value_1, value_2) == kGreater) {
      result = kGreater;
    } else {
      if (s21_checkLen(l_integr_len, r_integr_len, both_neg) == kLess) {
        result = kLess;
      } else if (s21_checkLen(l_integr_len, r_integr_len, both_neg) ==
                 kGreater) {
        result = kGreater;
      } else {
        int integral_compare =
            s21_checkNumber(l_integral, r_integral, both_neg);
        if (integral_compare == kLess) {
          result = kLess;
        } else if (integral_compare == kGreater) {
          result = kGreater;
        } else {
          s21_equalizeFractional(l_fractional, r_fractional);
          int fract_compare =
              s21_checkNumber(l_fractional, r_fractional, both_neg);
          if (fract_compare == kLess) {
            result = kLess;
          } else if (fract_compare == kGreater) {
            result = kGreater;
          } else {
            result = kEqual;
          }
        }
      }
    }
  } else {
    result = kError;
  }
  return result;
}

/// @param value_1 первый децимал
/// @param value_2 второй децимал
/// @return возвращает 1, если s21_compare возвращает kLess, иначе 0
int s21_is_less(s21_decimal value_1, s21_decimal value_2) {
  int result = false_comparison;
  if (s21_compare(value_1, value_2) == kLess) result = true_comparison;
  return result;
}

/// @brief Проверяет, является ли первый децимал меньше или равным второму
/// @param value_1 первый децимал
/// @param value_2 второй децимал
/// @return возвращает 1, если s21_compare возвращает kLess или kEqual,
/// иначе 0
int s21_is_less_or_equal(s21_decimal value_1, s21_decimal value_2) {
  int result = false_comparison;
  int compare_result = s21_compare(value_1, value_2);
  if (compare_result == kLess || compare_result == kEqual) {
    result = true_comparison;
  }
  return result;
}

/// @brief Проверяет, является ли первый децимал больше второго
/// @param value_1 первый децимал
/// @param value_2 второй децимал
/// @return возвращает 1, если s21_compare возвращает kGreater, иначе 0
int s21_is_greater(s21_decimal value_1, s21_decimal value_2) {
  int result = false_comparison;
  if (s21_compare(value_1, value_2) == kGreater) result = true_comparison;
  return result;
}

/// @brief Проверяет, является ли первый децимал больше или равным второму
/// @param value_1 первый децимал
/// @param value_2 второй децимал
/// @return возвращает 1, если s21_compare возвращает kGreater или kEqual,
/// иначе 0
int s21_is_greater_or_equal(s21_decimal value_1, s21_decimal value_2) {
  int result = false_comparison;
  int compare_result = s21_compare(value_1, value_2);
  if (compare_result == kGreater || compare_result == kEqual) {
    result = true_comparison;
  }
  return result;
}

/// @brief Проверяет числа на не эквивалентность
/// @param value_1 первый децимал
/// @param value_2 второй децимал
/// @return возвращает 1, если s21_is_equal возвращает false_comparison, иначе 0
int s21_is_not_equal(s21_decimal value_1, s21_decimal value_2) {
  int result = (!(s21_is_equal(value_1, value_2)));
  return result;
}

/// @brief Проверяет числа на эквивалентность
/// @param value_1 первый децимал
/// @param value_2 второй децимал
/// @return возвращает 1, если s21_compare возвращает kEqual, иначе 0
int s21_is_equal(s21_decimal value_1, s21_decimal value_2) {
  int result = false_comparison;
  if (s21_compare(value_1, value_2) == kEqual) result = true_comparison;
  return result;
}
