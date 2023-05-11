
#include "stm32l4_fwupg.h"
#include "FS.h"
#include "stm32l4_flash.h"
#include "stm32l4_exti.h"
#include "stm32l4_wiring_private.h"
#include "stm32l4_nvic.h"
#include "armv7m_systick.h" 
	
//#ifdef __cplusplus
//extern "C" {
//#endif

bool FWUpdrage_STANDALONE(const char* path, uint32_t offset, uint32_t size);

bool __attribute__((optimize("O0"))) stm32l4_ll_fwpgr(const char* path, uint32_t offset, uint32_t size)
{   

    NVIC_CatchIRQ(SysTick_IRQn, (uint32_t)SysTick_Handler_RAM);
    FWUpdrage_STANDALONE(path, offset, offset);

    return true;     
}

/*  @brief  :   Upgrade FW from file 
*   @param  :   void 
*   @retval :   void 
*/
bool  PF_MOVE_TO_RAM_ATT FWUpdrage_STANDALONE(const char* path, uint32_t offset, uint32_t size)             // PF_MOVE_TO_RAM_ATT
{
    File file1;
    bool result = false;
    uint8_t bufferToRead[2048];
    int bytesRead = 0, available = 0;
    uint32_t addr = 0x8000000;
    uint32_t bytesWritten = 0;
    // String path=(BIN_FILE_NAME);

    // 1.Mount if neccesary and make sure that path bin exists 
    // result = Begin();                                       // mount flash  
    result = DOSFS.exists(path);                            // make sure that path exists

    if(result == true)                                      // we have a valid path 
    {   
        // 2. Open for read th file 
        file1 = DOSFS.open(path, "r");     // open file for     
        if(!file1) return false;                            // do not trigger FW because we could not made the FW update
        if((offset + size) > file1.size()) return false;  
        if(!file1.seek(offset)) return false; 
        // 3. Make suret that we have bytes available 
        available = file1.available();                      // check if we have bytes to read
        if(available < size) return false;
        // 4.  Unlock and trigger mass erase  
        stm32l4_flash_unlock();                             // unlock flash
        stm32l4_flash_MassErase();                          // erase entire chip 

        while(available > 0 || bytesWritten < size)
        {   
            int bytesToRead;
            int mod8;

            if(available >= 2048)
                bytesToRead = 2048;
            else 
                bytesToRead = available;
            // Read from file 
            bytesRead = file1.read(bufferToRead, bytesToRead);        
            if(bytesRead != bytesToRead)
            {
                result = false;                                         // something happened , we do not read the desired bytes
                break; 
            }
            mod8 = bytesRead % 8;
            if(mod8)
            {
                int fillUntil = bytesRead + (8 - mod8);
                for(uint16_t i = bytesRead; i < fillUntil; i++)         // fill the rest of mod 8 bytes to zero 
                    bufferToRead[i] = 0;
                bytesRead = fillUntil;
            }
            // Write the flash with the content from file 
            result = stm32l4_flash_program(addr, bufferToRead, bytesRead);  
            addr += bytesRead;              
            bytesWritten += bytesRead;
            // update available bytes 
            available = file1.available();                              // update available bytes 
        }

        stm32l4_flash_lock();                                           // lock flash

        __DSB();                                                        // Ensure all outstanding memory accesses included
                                                                        // buffered write are completed before reset
        SCB->AIRCR  = ((0x5FAUL << SCB_AIRCR_VECTKEY_Pos) |             // Trigger reset 
                        SCB_AIRCR_SYSRESETREQ_Msk);
        __DSB();                                                        // Ensure completion of memory access

        for(;;) 
        {                                                               // wait until reset
            __NOP();
        }                                                     
        
    }

    return result; 
}

//#ifdef __cplusplus
//}
//#endif