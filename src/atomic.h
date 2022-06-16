//
// Created by a.taraskin on 20.10.2021.
//

#ifndef CORE_SRC_ATOMIC_H_
#define CORE_SRC_ATOMIC_H_

#include <core_cm4.h>
#include <stdint.h>
#include <stdbool.h>

typedef char a_mux;
// Захватить (заблокировать) ресурс
__STATIC_INLINE void a_mux_lock(a_mux *ptr) {
	do {
		while (__LDREXB(ptr));          // ждем освобождение ресурса
	} while (__STREXB(1, ptr));  // пытаемся захватить (заблокировать) ресурс
}

__STATIC_INLINE bool a_mux_lock_try(a_mux *ptr) {
	return (__LDREXB(ptr) == 0 && __STREXB(1, ptr) == 0);
}

// Освободить (разблокировать) ресурс
__STATIC_INLINE void a_mux_unlock(a_mux *ptr) {
	*ptr = 0;        // освобождаем ресурс
	__DSB();
}

__STATIC_INLINE uint32_t a_fetch_add(uint32_t *ptr, uint32_t value) {
	uint32_t oldValue, newValue;
	do {
		oldValue = __LDREXW(ptr);
		newValue = oldValue + value;
	} while (__STREXW(newValue, ptr));
	return oldValue;
}

__STATIC_INLINE uint32_t a_add_fetch(uint32_t *ptr, uint32_t value) {
	uint32_t oldValue, newValue;
	do {
		oldValue = __LDREXW(ptr);
		newValue = oldValue + value;
	} while (__STREXW(newValue, ptr));
	return newValue;
}

/*
// переменная, которую нужно атомарно модифицировать
volatile uint32_t value;
uint32_t oldValue;
uint32_t newValue;
do
{
  // запоминаем старое значение
          oldValue = value;

  // производим длинное вычисление
          newValue = SomeLongOperation(oldValue);

  // пытаемся атомарно заменить исходное значение
  // если его кто-то изменил без нас - перечитываем и вычисляем снова
}while(!CompareExchange((uint32_t *)&value, oldValue, newValue));
 */
__STATIC_INLINE bool a_compare_exchange(uint32_t *ptr, uint32_t oldValue, uint32_t newValue) {
	// эксклюзивно читаем значение переменной и сравниваем со старым значением
	if (__LDREXW(ptr) == oldValue)
		// пытаемся эксклюзивно записать в переменную новое значение
		return __STREXW(newValue, ptr) == 0;
	// кто-то изменил ячейку до нас
	__CLREX();
	return false;
}

#endif //CORE_SRC_ATOMIC_H_
