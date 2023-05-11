#ifndef _STM32L4_BOR_H_
#define _STM32L4_BOR_H_

#include "Arduino.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BOR_LEVEL0 0x00
#define BOR_LEVEL1 0x01
#define BOR_LEVEL2 0x02
#define BOR_LEVEL3 0x03
#define BOR_LEVEL4 0x04

void stm32l4_bor_set(uint8_t lvl);
uint8_t stm32l4_bor_get();

#ifdef __cplusplus
} // extern "C"
#endif

#endif