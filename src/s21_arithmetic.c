#include "s21_decimal.h"

/// @brief Проверяет децимал на 0
/// @param check децимал
/// @return 0 - равен нулю, 1 больше нуля
int s21_check_zero(s21_decimal check) {
  int flag = 1;
  if (check.bits[0] == 0 && check.bits[1] == 0 && check.bits[2] == 0) {
    flag = 0;
  }
  return flag;
}

/// @brief Получает степень из децимала для конвертации в большой
/// @param small децимал
/// @return степень
uint16_t s21_scale(s21_decimal small) {
  uint16_t scale = 0;
  scale = (small.bits[3] & Scale_Bits) >> 16;
  return scale;
}

/// @brief Конвертирует из обычного децимала в большой
/// @param small обычный децимал
/// @param big большой децимал для записи
void s21_convert_big(s21_decimal small, big_decimal *big) {
  for (int i = 0; i <= 2; i++) {
    big->bits[i] = small.bits[i] & Four_Bits_1;
  }
  big->scale = s21_scale(small);
}

/// @brief Конвертирует из большого децимала в обычный
/// @param small обычный децимал для записи результата
/// @param big большой децимал
void s21_convert_small(s21_decimal *small, big_decimal big) {
  for (int i = 0; i <= 2; i++) {
    small->bits[i] = big.bits[i] & Four_Bits_1;
  }
  small->bits[3] = big.scale;
  small->bits[3] = (small->bits[3] << 16) & Scale_Bits;
}

/// @brief Смотрит если переполнения 32 битов и перебрасывает их в старший
/// разряд
/// @param value децимал
/// @return
void s21_check_overflow(big_decimal *value) {
  uint64_t overflow = 0;
  for (int i = 0; i <= 6; i++) {
    value->bits[i] += overflow;
    overflow = value->bits[i] >> 32;
    value->bits[i] &= Four_Bits_1;
  }
}

/// @brief Увеличивает степень на 1
/// @param value децимал
void s21_scale_add(big_decimal *value) {
  for (int i = 0; i <= 6; i++) {
    value->bits[i] *= 10;
  }
  value->scale++;
}

/// @brief Уменьшает степень на 1
/// @param value децимал
/// @return отброшенное число(остаток)
uint64_t s21_scale_sub(big_decimal *value) {
  uint64_t remains = 0;
  for (int i = 6; i >= 0; i--) {
    value->bits[i] += (remains << 32);
    remains = value->bits[i] % 10;
    value->bits[i] /= 10;
  }
  value->scale--;
  return remains;
}

/// @brief Подготавливает большой децимал  на конвертацию, делает банковское
/// округление, если надо
/// @param value большой децимал
/// @return 1 - всё ок, 0 - переполнение
int s21_normal(big_decimal *value) {
  uint64_t remains = 0;
  int remains_flag = 0;
  int overflow = 1;
  while ((value->bits[6] != 0 || value->bits[5] != 0 || value->bits[4] != 0 ||
          value->bits[3] != 0) &&
             (value->scale != 0) ||
         (value->scale > 28)) {
    remains = s21_scale_sub(value);
    if (remains > 0) {
      remains_flag = 1;
    }
  }
  if (remains > 5) {
    value->bits[0]++;
  } else if (remains == 5) {
    if (remains_flag == 0) {
      if (value->bits[0] % 2 != 0) {
        value->bits[0]++;
      }
    } else {
      value->bits[0]++;
    }
  }

  if (value->bits[6] != 0 || value->bits[5] != 0 || value->bits[4] != 0 ||
      value->bits[3] != 0) {
    overflow = 0;
  }
  return overflow;
}

int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  int flag_error = without_error;
  if (!s21_check_zero(value_1)) {
    for (int i = 0; i <= 3; i++) {
      result->bits[i] = value_2.bits[i];
    }
  } else if (!s21_check_zero(value_2)) {
    for (int i = 0; i <= 3; i++) {
      result->bits[i] = value_1.bits[i];
    }
  } else {
    s21_add_main(value_1, value_2, result, &flag_error);
  }
  return flag_error;
}

void s21_add_main(s21_decimal value_1, s21_decimal value_2, s21_decimal *result,
                  int *flag_error) {
  big_decimal big_value_1 = {0, 0, 0, 0, 0, 0, 0};
  big_decimal big_value_2 = {0, 0, 0, 0, 0, 0, 0};
  big_decimal big_result = {0, 0, 0, 0, 0, 0, 0};
  int path = s21_sign_path(value_1, value_2);
  s21_convert_big(value_1, &big_value_1);
  s21_convert_big(value_2, &big_value_2);
  s21_equalizing_scale(&big_value_1, &big_value_2);
  big_result.scale = big_value_1.scale;
  int sign_result = s21_sign_result(big_value_1, big_value_2, path);
  if (path == 1) {
    s21_path_1_or_2_add(big_value_1, big_value_2, &big_result);
  } else if (path == 2) {
    s21_path_1_or_2_add(big_value_1, big_value_2, &big_result);
  } else if (path == 3) {
    s21_path_3_or_4_add(&big_value_1, &big_value_2, &big_result);
  } else if (path == 4) {
    s21_path_3_or_4_add(&big_value_1, &big_value_2, &big_result);
  }
  s21_check_overflow(&big_result);
  if (s21_normal(&big_result)) {
    s21_convert_small(result, big_result);
    if (sign_result == 0) {
      result->bits[3] &= Scale_Bits;
    } else {
      result->bits[3] |= MINUS;
    }
    *flag_error = without_error;
  } else {
    if (sign_result == 0) {
      *flag_error = too_big_number;
    } else {
      *flag_error = too_small_number;
    }
  }
}

/// @brief Вычисляет знак результата сложения
/// @param value_1 децимал 1
/// @param value_2 децимал 2
/// @param path путь сложения
/// @return 0 = плюс, 1 = минус
int s21_sign_result(big_decimal value_1, big_decimal value_2, int path) {
  int sign = 2;
  if (path == 1) {
    sign = 0;
  } else if (path == 2) {
    sign = 1;
  } else if (path == 3) {
    if (s21_srav_mantis(value_1, value_2) == 1) {
      sign = 1;
    } else if (s21_srav_mantis(value_1, value_2) == 2) {
      sign = 0;
    } else if ((s21_srav_mantis(value_1, value_2) == 0)) {
      sign = 0;
    }
  } else if (path == 4) {
    if (s21_srav_mantis(value_1, value_2) == 1) {
      sign = 0;
    } else if (s21_srav_mantis(value_1, value_2) == 2) {
      sign = 1;
    } else if ((s21_srav_mantis(value_1, value_2) == 0)) {
      sign = 0;
    }
  }
  return sign;
}

/// @brief Производит вычитание из большего децимала меньший
/// @param value_1 большой децимал 1
/// @param value_2 большой децимал 2
/// @param result большой децимал для записи результата
void s21_path_3_or_4_add(big_decimal *value_1, big_decimal *value_2,
                         big_decimal *result) {
  int srav = s21_srav_mantis(*value_1, *value_2);
  if (srav == 1) {
    for (int i = 0; i <= 6; i++) {
      value_2->bits[i] = ~value_2->bits[i] & Four_Bits_1;
    }
  } else if (srav == 2) {
    for (int i = 0; i <= 6; i++) {
      value_1->bits[i] = ~value_1->bits[i] & Four_Bits_1;
    }
  } else if (srav == 0) {
    for (int i = 0; i <= 6; i++) {
      result->bits[i] = 0;
    }
    result->scale = 0;
  }
  if (srav == 1 || srav == 2) {
    for (int i = 0; i <= 6; i++) {
      result->bits[i] = value_1->bits[i] + value_2->bits[i];
    }
    result->bits[0] += 1;
  }
}

/// @brief Производит сложение децимало с одинаковыми знаками
/// @param value_1 большой децимал 1
/// @param value_2 большой децимал 2
/// @param result  большой децимал для записи результата
void s21_path_1_or_2_add(big_decimal value_1, big_decimal value_2,
                         big_decimal *result) {
  for (int i = 0; i <= 6; i++) {
    result->bits[i] = value_1.bits[i] + value_2.bits[i];
  }
}

/// @brief Показывает номер пути пойдет сложение в зависимости от знаков
/// @param value_1 поданный децимал 1
/// @param value_2 поданный децимал 2
/// @return 1 - оба положительные; 2 - оба отрицательные; 3 - первое
/// отрицательное; 4 - второе отрицательное
int s21_sign_path(s21_decimal value_1, s21_decimal value_2) {
  int value_1_sign, value_2_sign;
  int path = 0;
  value_1_sign = s21_get_sign(value_1);
  value_2_sign = s21_get_sign(value_2);
  if (value_1_sign == 0 && value_2_sign == 0) {
    path = 1;
  } else if (value_1_sign == 1 && value_2_sign == 1) {
    path = 2;
  } else if (value_1_sign == 1 && value_2_sign == 0) {
    path = 3;
  } else if (value_1_sign == 0 && value_2_sign == 1) {
    path = 4;
  }
  return path;
}

/// @brief Выравнивает степени 2 больший децималов от меньшего к большему
/// @param big_value_1 1 большой децимал
/// @param big_value_2 2 большой децимал
void s21_equalizing_scale(big_decimal *big_value_1, big_decimal *big_value_2) {
  if (big_value_1->scale < big_value_2->scale) {
    while (big_value_1->scale < big_value_2->scale) {
      s21_scale_add(big_value_1);
      s21_check_overflow(big_value_1);
    }
  }
  if (big_value_1->scale > big_value_2->scale) {
    while (big_value_1->scale > big_value_2->scale) {
      s21_scale_add(big_value_2);
      s21_check_overflow(big_value_2);
    }
  }
}

/// @brief сравнение мантис 2 децималов
/// @param value_1 первый децимал
/// @param value_2 второй децимал
/// @return 0-они равны 1-первый больше 2-второй больше
int s21_srav_mantis(big_decimal value_1, big_decimal value_2) {
  int flag = 0;
  for (int i = 6; i >= 0; i--) {
    if (value_1.bits[i] > value_2.bits[i]) {
      flag = 1;
      break;
    } else if (value_1.bits[i] < value_2.bits[i]) {
      flag = 2;
      break;
    }
  }
  return flag;
}

int s21_sub(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  int flag_error = without_error;
  s21_decimal invert_1 = value_1;
  s21_decimal invert_2 = value_2;
  int sign_1 = s21_get_sign(value_1);
  int sign_2 = s21_get_sign(value_2);
  s21_decimal negativ = {0};
  if (sign_1 == 0 && sign_2 == 0) {
    invert_2.bits[3] |= MINUS;
    flag_error = s21_add(value_1, invert_2, result);
    if (s21_is_less_or_equal(value_1, value_2)) {
      result->bits[3] |= MINUS;
    }
  } else if (sign_1 == 1 && sign_2 == 0) {
    value_1.bits[3] &= Scale_Bits;
    flag_error = s21_add(value_1, value_2, result);
    s21_negative(value_1, &negativ);
    if (!s21_is_less_or_equal(negativ, value_2) ||
        s21_is_less_or_equal(value_1, value_2)) {
      result->bits[3] |= MINUS;
    }
  } else if (sign_1 == 1 && sign_2 == 1) {
    invert_1.bits[3] &= Scale_Bits;
    flag_error = s21_add(invert_1, value_2, result);
    if (s21_is_less_or_equal(value_1, value_2)) {
      result->bits[3] |= MINUS;
    } else {
      result->bits[3] &= Scale_Bits;
    }
  } else if (sign_1 == 0 && sign_2 == 1) {
    value_2.bits[3] &= Scale_Bits;
    flag_error = s21_add(value_1, value_2, result);
  }
  if (result->bits[0] == 0 && result->bits[1] == 0 && result->bits[2] == 0) {
    result->bits[3] = 0;
  }

  return flag_error;
}
/// @brief Аналог у множения на - 1:
/// @param value децимал
/// @param result децимал *-1
void s21_negative(s21_decimal value, s21_decimal *result) {
  if (s21_get_sign(value)) {
    result->bits[3] = value.bits[3] | MINUS;
  } else {
    result->bits[3] = value.bits[3] & Scale_Bits;
  }
  for (int i = 0; i <= 2; i++) {
    result->bits[i] = value.bits[i];
  }
}
int s21_mul(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  int flag_error = without_error;
  if (!s21_check_zero(value_1) || !s21_check_zero(value_2)) {
    for (int i = 0; i <= 3; i++) {
      result->bits[i] = 0;
    }
  } else {
    s21_mul_main(value_1, value_2, result, &flag_error);
  }
  return flag_error;
}
void s21_mul_main(s21_decimal value_1, s21_decimal value_2, s21_decimal *result,
                  int *flag_error) {
  big_decimal big_value_1 = {0, 0, 0, 0, 0, 0, 0};
  big_decimal big_value_2 = {0, 0, 0, 0, 0, 0, 0};
  big_decimal big_result = {0, 0, 0, 0, 0, 0, 0};
  int value_1_sign, value_2_sign;
  value_1_sign = s21_get_sign(value_1);
  value_2_sign = s21_get_sign(value_2);
  s21_convert_big(value_1, &big_value_1);
  s21_convert_big(value_2, &big_value_2);
  big_result.scale = s21_scale(value_1) + s21_scale(value_2);
  big_result.bits[0] = big_value_1.bits[0] * big_value_2.bits[0];
  big_result.bits[1] = (big_value_1.bits[1] * big_value_2.bits[0]) +
                       (big_value_1.bits[0] * big_value_2.bits[1]);
  big_result.bits[2] = (big_value_1.bits[0] * big_value_2.bits[2]) +
                       (big_value_1.bits[1] * big_value_2.bits[1]) +
                       (big_value_1.bits[2] * big_value_2.bits[0]);
  big_result.bits[3] = (big_value_1.bits[1] * big_value_2.bits[2]) +
                       (big_value_1.bits[2] * big_value_2.bits[1]);
  big_result.bits[4] = big_value_1.bits[2] * big_value_2.bits[2];
  s21_check_overflow(&big_result);
  if (s21_normal(&big_result)) {
    s21_convert_small(result, big_result);
    if ((value_1_sign == 1 && value_2_sign == 0) ||
        (value_1_sign == 0 && value_2_sign == 1)) {
      result->bits[3] = result->bits[3] | MINUS;
    }
  } else {
    if ((value_1_sign == 1 && value_2_sign == 0) ||
        (value_1_sign == 0 && value_2_sign == 1)) {
      *flag_error = too_small_number;
    } else {
      *flag_error = too_big_number;
    }
  }
}

int s21_div(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  int flag_error = without_error;
  if (!s21_check_zero(value_1)) {
    for (int i = 0; i <= 3; i++) {
      result->bits[i] = 0;
    }
  } else if (!s21_check_zero(value_2)) {
    flag_error = div_null;
  } else {
    s21_div_main(value_1, value_2, result, &flag_error);
  }
  return flag_error;
}

void s21_div_main(s21_decimal value_1, s21_decimal value_2, s21_decimal *result,
                  int *flag_error) {
  big_decimal big_value_1 = {0, 0, 0, 0, 0, 0, 0};
  big_decimal big_value_2 = {0, 0, 0, 0, 0, 0, 0};
  big_decimal big_result = {0, 0, 0, 0, 0, 0, 0};
  int value_1_sign, value_2_sign;
  value_1_sign = s21_get_sign(value_1);
  value_2_sign = s21_get_sign(value_2);
  value_1.bits[3] &= Scale_Bits;
  value_2.bits[3] &= Scale_Bits;
  s21_convert_big(value_1, &big_value_1);
  s21_convert_big(value_2, &big_value_2);
  if (big_value_1.scale <= big_value_2.scale) {
    big_result.scale = 0;
  } else {
    big_result.scale = big_value_1.scale - big_value_2.scale;
  }
  s21_equalizing_scale(&big_value_1, &big_value_2);
  big_decimal remains = big_value_1;
  if (s21_cycle_shift(big_value_1, big_value_2, &big_result, &remains)) {
    while (s21_big_check_zero(remains) && big_result.scale <= 29) {
      s21_scale_add(&remains);
      s21_scale_add(&big_result);
      s21_cycle_shift(big_value_1, big_value_2, &big_result, &remains);
    }
    s21_normal(&big_result);
    s21_convert_small(result, big_result);
    if ((value_1_sign == 1 && value_2_sign == 0) ||
        (value_1_sign == 0 && value_2_sign == 1)) {
      result->bits[3] |= MINUS;
    } else {
      result->bits[3] &= Scale_Bits;
    }
  } else {
    if ((value_1_sign == 1 && value_2_sign == 0) ||
        (value_1_sign == 0 && value_2_sign == 1)) {
      *flag_error = too_small_number;
    } else {
      *flag_error = too_big_number;
    }
  }
}

/// @brief Устанавливает 1 в заданный индекс
/// @param value большой децимал куда будет производится вставка
/// @param i индекс вставки
void s21_set_1(big_decimal *value, int i) {
  int shift_32 = i / 32;
  int shift_remains = i % 32;
  value->bits[shift_32] |= 1 << shift_remains;
}

/// @brief Проверяется большой децимал на 0
/// @param value большой децимал для проверки
/// @return 0 - равень нулю, 1- не равен нулю
int s21_big_check_zero(big_decimal value) {
  int flag = 0;
  for (int i = 0; i <= 6; i++) {
    if (value.bits[i] != 0) {
      flag = 1;
      break;
    }
  }
  return flag;
}

/// @brief Вычитание для большого децимала
/// @param value_1 уменьшаемое
/// @param value_2 вычитаемое
/// @param result результат
void s21_sub_for_div(big_decimal value_1, big_decimal value_2,
                     big_decimal *result) {
  for (int i = 0; i <= 6; i++) {
    if (value_1.bits[i] < value_2.bits[i]) {
      value_1.bits[i] += (value_1.bits[i + 1] % 10) << 32;
      value_1.bits[i + 1] = value_1.bits[i + 1] / 10;
    }
  }
  big_decimal tmp_result = {0, 0, 0, 0, 0, 0};
  for (int i = 0; i <= 6; i++) {
    value_2.bits[i] = (~value_2.bits[i]);
  }
  for (int i = 0; i <= 6; i++) {
    tmp_result.bits[i] = (value_1.bits[i] + value_2.bits[i]) + 1;
  }
  for (int i = 0; i <= 6; i++) {
    result->bits[i] = tmp_result.bits[i];
  }
}

/// @brief Сдвигает мантису большого децимала на индекс
/// @param value большой децимал для сдвига
/// @param coef_shift индекс сдвига
void s21_shift(big_decimal *value, int coef_shift) {
  int shift_32 = coef_shift / 32;
  int shift_remains = coef_shift % 32;
  for (int j = 0; j < shift_32; j++) {
    for (int i = 6; i >= 0; i--) {
      value->bits[i] = value->bits[i] << 32;
    }
    s21_check_overflow(value);
  }
  for (int i = 6; i >= 0; i--) {
    value->bits[i] = value->bits[i] << shift_remains;
  }
  s21_check_overflow(value);
}

/// @brief Цикл сдвигов и вычитания для деления
/// @param value_1 делимое
/// @param value_2  делитель
/// @param result  результат
/// @param remains остаток
/// @return 1 все ок, 0 переполнение
int s21_cycle_shift(big_decimal value_1, big_decimal value_2,
                    big_decimal *result, big_decimal *remains) {
  int flag_error = 1;
  big_decimal tmp_value_2 = value_2;
  for (int i = 96; (i >= 0) && (s21_big_check_zero(*remains)); i--) {
    if (s21_srav_mantis(*remains, value_2) == 2) {
      break;
    }
    s21_shift(&tmp_value_2, i);
    int z = s21_srav_mantis(*remains, tmp_value_2);
    if (z == 1 && i == 96) {
      flag_error = 0;
      break;
    } else if (z == 0 || z == 1) {
      s21_sub_for_div(*remains, tmp_value_2, remains);
      s21_set_1(result, i);
    }
    tmp_value_2 = value_2;
  }
  return flag_error;
}
