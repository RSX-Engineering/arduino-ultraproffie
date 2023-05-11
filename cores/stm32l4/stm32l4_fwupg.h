#ifndef _STM32L4_FWUPG_H
#define _STM32L4_FWUPG_H

#include <stdint.h>
#include "FS.h"
//#ifdef __cplusplus
//extern "C" {
//#endif

bool stm32l4_ll_fwpgr(const char* path, uint32_t offset, uint32_t size);

//#ifdef __cplusplus
//} // extern "C"
//#endif

#endif /* _STM32L4_FWUPG_H */