#ifdef __cplusplus
extern "C" {
#endif

#include "stm32l4_bor.h"
#include "stm32l4_wiring_private.h"

#define FLASH_FLAG_SR_ERRORS      (FLASH_SR_OPERR   | FLASH_SR_PROGERR | FLASH_SR_WRPERR | \
                                   FLASH_SR_PGAERR  | FLASH_SR_SIZERR  | FLASH_SR_PGSERR | \
                                   FLASH_SR_MISERR  | FLASH_SR_FASTERR | FLASH_SR_RDERR  | \
                                   FLASH_SR_OPTVERR)

#define FLASH_FLAG_ECCR_ERRORS    (FLASH_ECCR_ECCD | FLASH_ECCR_ECCC)

#define FLASH_GET_FLAG(__FLAG__)          ((((__FLAG__) & FLASH_FLAG_ECCR_ERRORS) != 0U)     ? \
                (READ_BIT(FLASH->ECCR, (__FLAG__)) != 0U) : \
                (READ_BIT(FLASH->SR,   (__FLAG__)) != 0U))

#define FLASH_CLEAR_FLAG(__FLAG__)        do { if(((__FLAG__) & FLASH_FLAG_ECCR_ERRORS) != 0U) { SET_BIT(FLASH->ECCR, ((__FLAG__) & FLASH_FLAG_ECCR_ERRORS)); }\
                                                     if(((__FLAG__) & ~(FLASH_FLAG_ECCR_ERRORS)) != 0U) { WRITE_REG(FLASH->SR, ((__FLAG__) & ~(FLASH_FLAG_ECCR_ERRORS))); }\
                                                   } while(0)
                                                   


static bool obWaitLastOperation(uint32_t timeout)
{
    uint32_t timeStamp = millis();
    uint32_t error;

    while(READ_BIT(FLASH->SR, FLASH_SR_BSY) != 0U)
    {
        if(millis() - timeStamp > timeout)
        return false;
    }

    error = (FLASH->SR & FLASH_FLAG_SR_ERRORS);

    if(error != 0u)
    {
        //Clear error programming flags
        FLASH_CLEAR_FLAG(error);
        return false;
    }

    // Check FLASH End of Operation flag
    if (FLASH_GET_FLAG(FLASH_SR_EOP))
    {
        // Clear FLASH End of Operation pending bit
        FLASH_CLEAR_FLAG(FLASH_SR_EOP);
    }
    return true;
}

static bool obLaunch(void)
{
  // Set the bit to force the option byte reloading */
  SET_BIT(FLASH->CR, FLASH_CR_OBL_LAUNCH);
  // Wait for last operation to be completed
  return(obWaitLastOperation((uint32_t)100));
}


static bool obUnlock(void)
{
  stm32l4_flash_unlock();   // unlock flash 
  if(READ_BIT(FLASH->CR, FLASH_CR_OPTLOCK) != 0U)
  {
    // Authorizes the Option Byte register programming
    WRITE_REG(FLASH->OPTKEYR, 0x08192A3B);  // FLASH_OPTKEY1
    WRITE_REG(FLASH->OPTKEYR, 0x4C5D6E7F);  // FLASH_OPTKEY2    
    return(obWaitLastOperation((uint32_t)100));
  }
  else
  {
    return false;
  }
  
  return true;
}

static void obLock(void)
{
  // Set the OPTLOCK Bit to lock the FLASH Option Byte Registers access
  SET_BIT(FLASH->CR, FLASH_CR_OPTLOCK);

}



void stm32l4_bor_set(uint8_t lvl)
{   
    bool status;
    if (lvl > BOR_LEVEL4) return;   // invalid level
    if(stm32l4_bor_get() != lvl)             // set only if neccesary
    {
        obUnlock();
        if(!obWaitLastOperation(100)) return;
        // Configure the option bytes register
        MODIFY_REG(FLASH->OPTR, FLASH_OPTR_BOR_LEV, (lvl << FLASH_OPTR_BOR_LEV_Pos) );
        // Set OPTSTRT Bit */
        SET_BIT(FLASH->CR, FLASH_CR_OPTSTRT);
        // Wait for last operation to be completed 
        obWaitLastOperation(100);
        // If the option byte program operation is completed, disable the OPTSTRT Bit
        CLEAR_BIT(FLASH->CR, FLASH_CR_OPTSTRT);

        obLaunch();
    }

}

uint8_t stm32l4_bor_get(void)
{     
    return ((READ_REG(FLASH->OPTR) & 0x700)>> 8);
}


#ifdef __cplusplus
} // extern "C"
#endif