/*
 * Copyright (c) 2016 Thomas Roell.  All rights reserved.
 * Copyright (c) 2019 Fredrik Hubinette.  All rights reserved.
 *  // TODO ADD COPYRIGHT MAYBE
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal with the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 *  1. Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimers.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimers in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of Thomas Roell, nor the names of its contributors
 *     may be used to endorse or promote products derived from this Software
 *     without specific prior written permission.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
 * CONTRIBUTORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * WITH THE SOFTWARE.
 */

#include "Arduino.h"
#include "stm32l4_wiring_private.h"

#define PWM_INSTANCE_TIM1      0
#define PWM_INSTANCE_TIM2      1
#define PWM_INSTANCE_TIM15     2
#define PWM_INSTANCE_TIM16     3

/*
 * Pins descriptions
 */

//EmptySerial SerialStub;
// PROFFIE LITE CONFIGURATION 
extern const PinDescription g_APinDescription[NUM_TOTAL_PINS] =
{
  // 0, RESERVED FOR OPAMP VIN  DO NOT USE YET - IT MAY HAVE DIFFERENT CAPABILITIES BUT WE WILL RESTRICT IT FOR NOW  
  { GPIOA, GPIO_PIN_MASK(GPIO_PIN_PA0),  GPIO_PIN_PA0,             0,                                                PWM_INSTANCE_NONE,  PWM_CHANNEL_NONE, ADC_INPUT_NONE    },
  // 1, ADC USED FOR MONITORING BATTTERY LEVEL - IT MAY HAVE DIFFERENT CAPABILITIES BUT WE WILL RESTRICT IT FOR NOW
  { GPIOA, GPIO_PIN_MASK(GPIO_PIN_PA1),  GPIO_PIN_PA1,             (PIN_ATTR_ADC),                                   PWM_INSTANCE_NONE,  PWM_CHANNEL_NONE, ADC_INPUT_6       },
  // 2, RESERVED FOR QUADSPI_CS  - DO NOT USE
  { GPIOA, GPIO_PIN_MASK(GPIO_PIN_PA2),  GPIO_PIN_PA2,             0,                                                PWM_INSTANCE_NONE,  PWM_CHANNEL_NONE, ADC_INPUT_NONE    },
  // 3, RESERVED FOR QUADSPI_CK - DO NOT USE
  { GPIOA, GPIO_PIN_MASK(GPIO_PIN_PA3),  GPIO_PIN_PA3,             0,                                                PWM_INSTANCE_NONE,  PWM_CHANNEL_NONE, ADC_INPUT_NONE    },
  // 4, DAC USED FOR BATTERY MONITOR - IT MAY HAVE DIFFERENT CAPABILITIES BUT WE WILL RESTRICT IT FOR NOW
  { GPIOA, GPIO_PIN_MASK(GPIO_PIN_PA4),  GPIO_PIN_PA4,             (PIN_ATTR_DAC),                                   PWM_INSTANCE_NONE,  PWM_CHANNEL_NONE, ADC_INPUT_NONE    },
  // 5, LED Channel 1 
  { GPIOA, GPIO_PIN_MASK(GPIO_PIN_PA5),  GPIO_PIN_PA5_TIM2_CH1,    (PIN_ATTR_PWM),                                   PWM_INSTANCE_TIM2,  PWM_CHANNEL_1,    ADC_INPUT_NONE    },
  // 6, RESERVED FOR QUADSPI_IO3 - DO NOT USE
  { GPIOA, GPIO_PIN_MASK(GPIO_PIN_PA6),  GPIO_PIN_PA6,             0,                                                PWM_INSTANCE_NONE,  PWM_CHANNEL_NONE, ADC_INPUT_NONE    },
  // 7, RESERVED FOR QUADSPI_IO2 - DO NOT USE
  { GPIOA, GPIO_PIN_MASK(GPIO_PIN_PA7),  GPIO_PIN_PA7,             0,                                                PWM_INSTANCE_NONE,  PWM_CHANNEL_NONE, ADC_INPUT_NONE    },
  // 8, SAI-BCLK-PA8
  { GPIOA, GPIO_PIN_MASK(GPIO_PIN_PA8),  GPIO_PIN_PA8,             0,                                                PWM_INSTANCE_NONE,  PWM_CHANNEL_NONE, ADC_INPUT_NONE    },
  // 9, SAI-LRCLK-PA9
  { GPIOA, GPIO_PIN_MASK(GPIO_PIN_PA9),  GPIO_PIN_PA9,             0,                                                PWM_INSTANCE_NONE,  PWM_CHANNEL_NONE, ADC_INPUT_NONE    },
  // 10, SAI-DATA-PA10
  { GPIOA, GPIO_PIN_MASK(GPIO_PIN_PA10), GPIO_PIN_PA10,            0,                                                PWM_INSTANCE_NONE,  PWM_CHANNEL_NONE, ADC_INPUT_NONE    },
  // 11, Button 1
  { GPIOA, GPIO_PIN_MASK(GPIO_PIN_PA11), GPIO_PIN_PA11,            (PIN_ATTR_EXTI),                                  PWM_INSTANCE_NONE,  PWM_CHANNEL_NONE, ADC_INPUT_NONE    },
  // 12, Button 2
  { GPIOA, GPIO_PIN_MASK(GPIO_PIN_PA12), GPIO_PIN_PA12,            (PIN_ATTR_EXTI),                                  PWM_INSTANCE_NONE,  PWM_CHANNEL_NONE, ADC_INPUT_NONE    },
  // 13, unsed for now 
  { GPIOA, GPIO_PIN_MASK(GPIO_PIN_PA15), GPIO_PIN_PA15,            (PIN_ATTR_EXTI),                     			 PWM_INSTANCE_NONE,  PWM_CHANNEL_NONE, ADC_INPUT_NONE    },
  // 14. RESERVED FOR QUADSPI_IO1 - DO NOT USE 
  { GPIOB, GPIO_PIN_MASK(GPIO_PIN_PB0),  GPIO_PIN_PB0,             0,                                                PWM_INSTANCE_NONE,  PWM_CHANNEL_NONE, ADC_INPUT_NONE    },
  // 15. RESERVED FOR QUADSPI_IO0 - DO NOT USE
  { GPIOB, GPIO_PIN_MASK(GPIO_PIN_PB1),  GPIO_PIN_PB1,             0,                                                PWM_INSTANCE_NONE,  PWM_CHANNEL_NONE, ADC_INPUT_NONE    },
  // 16. VOUT ENABLE (FOR STRING BLADE )
  { GPIOB, GPIO_PIN_MASK(GPIO_PIN_PB2),  GPIO_PIN_PB2,             0,                                                PWM_INSTANCE_NONE,  PWM_CHANNEL_NONE, ADC_INPUT_NONE    },
  // 17, LED Channel 2 
  { GPIOB, GPIO_PIN_MASK(GPIO_PIN_PB3),  GPIO_PIN_PB3_TIM2_CH2,    (PIN_ATTR_PWM),                                   PWM_INSTANCE_TIM2,  PWM_CHANNEL_2,    ADC_INPUT_NONE    },
  // 18. AMP ENABLE 
  { GPIOB, GPIO_PIN_MASK(GPIO_PIN_PB4),  GPIO_PIN_PB4,             0,                                                PWM_INSTANCE_NONE,  PWM_CHANNEL_NONE, ADC_INPUT_NONE    },
  // 19. I2C_SCL 
  { GPIOB, GPIO_PIN_MASK(GPIO_PIN_PB6),  GPIO_PIN_PB6,             (PIN_ATTR_EXTI),                                  PWM_INSTANCE_NONE,  PWM_CHANNEL_NONE, ADC_INPUT_NONE    },
  // 20. Charge state - active low  
  { GPIOB, GPIO_PIN_MASK(GPIO_PIN_PB7),  GPIO_PIN_PB7,             (PIN_ATTR_EXTI),                                  PWM_INSTANCE_NONE,  PWM_CHANNEL_NONE, ADC_INPUT_NONE    },
  // 21, DAT2 led string data 2
  { GPIOB, GPIO_PIN_MASK(GPIO_PIN_PB8),  GPIO_PIN_PB8_TIM16_CH1,   (PIN_ATTR_EXTI|PIN_ATTR_PWM),                     PWM_INSTANCE_TIM16, PWM_CHANNEL_1,    ADC_INPUT_NONE    },
  // 22, I2C_SDA
  { GPIOB, GPIO_PIN_MASK(GPIO_PIN_PB9),  GPIO_PIN_PB9,             (PIN_ATTR_EXTI),                                  PWM_INSTANCE_NONE,  PWM_CHANNEL_NONE, ADC_INPUT_NONE    },
  // 23, LED Channel 3 
  { GPIOB, GPIO_PIN_MASK(GPIO_PIN_PB10), GPIO_PIN_PB10_TIM2_CH3,   (PIN_ATTR_PWM),                                   PWM_INSTANCE_TIM2,  PWM_CHANNEL_3,    ADC_INPUT_NONE    },
  // 24, LED Channel 4 
  { GPIOB, GPIO_PIN_MASK(GPIO_PIN_PB11), GPIO_PIN_PB11_TIM2_CH4,   (PIN_ATTR_PWM),                                   PWM_INSTANCE_TIM2,  PWM_CHANNEL_4,    ADC_INPUT_NONE    },
  // 25, RESERVED FOR SPI2_SCK - DO NOT USE 
  { GPIOB, GPIO_PIN_MASK(GPIO_PIN_PB13), GPIO_PIN_PB13_TIM15_CH1N, (PIN_ATTR_PWM),                                  PWM_INSTANCE_TIM15,  PWM_CHANNEL_1, ADC_INPUT_NONE    },
  // 26, RESERVED FOR SAI MCLK - DO NOT USE
  { GPIOB, GPIO_PIN_MASK(GPIO_PIN_PB14), GPIO_PIN_PB14_TIM15_CH1,  (PIN_ATTR_PWM),                                  PWM_INSTANCE_TIM15,  PWM_CHANNEL_1, ADC_INPUT_NONE    },
  // 27, PIXEL LED DATA - SPI2-MOSI
  { GPIOB, GPIO_PIN_MASK(GPIO_PIN_PB15), GPIO_PIN_PB15_TIM15_CH2,  (PIN_ATTR_EXTI|PIN_ATTR_PWM),                     PWM_INSTANCE_TIM15, PWM_CHANNEL_2,    ADC_INPUT_NONE    },
  // 28, ACC INT_1
  { GPIOC, GPIO_PIN_MASK(GPIO_PIN_PC13), GPIO_PIN_PC13,            (PIN_ATTR_EXTI),                                  PWM_INSTANCE_NONE,  PWM_CHANNEL_NONE, ADC_INPUT_NONE    },
  // 29, Vbat
  {0,		0,								0,							0,												  0, 				 0,				18},
  // 30, Vref in
  {0,		0,								0,							0,												  0, 				 0,				0},
  // 31, Charge Enable 
  { GPIOB, GPIO_PIN_MASK(GPIO_PIN_PB5),  GPIO_PIN_PB5,             (PIN_ATTR_EXTI),                                  PWM_INSTANCE_NONE,  PWM_CHANNEL_NONE, ADC_INPUT_NONE    },
  // 32, Empty
  {0,		0,								0,							0,												  0, 				 0,				ADC_INPUT_NONE},
};

extern const unsigned int g_PWMInstances[PWM_INSTANCE_COUNT] = {
     TIMER_INSTANCE_TIM1,
     TIMER_INSTANCE_TIM2,
     TIMER_INSTANCE_TIM15,
     TIMER_INSTANCE_TIM16,
 };

//extern const unsigned int g_PWMInstances[PWM_INSTANCE_COUNT] = {
//    TIMER_INSTANCE_TIM2,
//};

//extern const stm32l4_uart_pins_t g_SerialPins = { GPIO_PIN_PB7_USART1_RX, GPIO_PIN_PB6_USART1_TX, GPIO_PIN_NONE, GPIO_PIN_NONE };
//extern const unsigned int g_SerialInstance = UART_INSTANCE_USART1;
//extern const unsigned int g_SerialMode = 0;

extern const stm32l4_uart_pins_t g_SerialPins = { GPIO_PIN_PA10_USART1_RX, GPIO_PIN_PA9_USART1_TX, GPIO_PIN_NONE, GPIO_PIN_NONE };
extern const unsigned int g_SerialInstance = UART_INSTANCE_USART1;
extern const unsigned int g_SerialMode = 0;

// // Serial1Pins is the same as Neopixels 2 & LED 3. - DO NOT USE !!!! 
// extern const stm32l4_uart_pins_t g_Serial1Pins = { GPIO_PIN_PB11_USART3_RX, GPIO_PIN_PB10_USART3_TX, GPIO_PIN_NONE, GPIO_PIN_NONE };
// extern const unsigned int g_Serial1Instance = UART_INSTANCE_USART3;
// extern const unsigned int g_Serial1Mode = 0;

// // Don't use, would interfere with LED Channel 2 and I2C CL - DO NOT USE !!!! 
// extern const stm32l4_uart_pins_t g_Serial2Pins = { GPIO_PIN_PA10_USART1_RX, GPIO_PIN_PA9_USART1_TX, GPIO_PIN_NONE, GPIO_PIN_NONE };
// extern const unsigned int g_Serial2Instance = UART_INSTANCE_USART1;
// extern const unsigned int g_Serial2Mode = 0;

// // RX/TX, please use this one.- DO NOt USE 
// extern const stm32l4_uart_pins_t g_Serial3Pins = { GPIO_PIN_PA15_USART2_RX, GPIO_PIN_PA2_USART2_TX, GPIO_PIN_NONE, GPIO_PIN_NONE };
// extern const unsigned int g_Serial3Instance = UART_INSTANCE_USART2;
// extern const unsigned int g_Serial3Mode = UART_MODE_RX_DMA;

// extern const stm32l4_spi_pins_t g_SPIPins = { GPIO_PIN_PB15_SPI2_MOSI, GPIO_PIN_PB14_SPI2_MISO, GPIO_PIN_PB13_SPI2_SCK, GPIO_PIN_NONE };
// extern const unsigned int g_SPIInstance = SPI_INSTANCE_SPI2;
// extern const unsigned int g_SPIMode = SPI_MODE_RX_DMA | SPI_MODE_TX_DMA | SPI_MODE_RX_DMA_SECONDARY | SPI_MODE_TX_DMA_SECONDARY;
// SPI - PIXEL LED InterFACE
extern const stm32l4_spi_pins_t g_SPIPins = { GPIO_PIN_PA7_SPI1_MOSI, GPIO_PIN_PA6_SPI1_MISO, GPIO_PIN_PA1_SPI1_SCK, GPIO_PIN_NONE };
extern const unsigned int g_SPIInstance = SPI_INSTANCE_SPI1;
extern const unsigned int g_SPIMode = SPI_MODE_RX_DMA | SPI_MODE_TX_DMA | SPI_MODE_RX_DMA_SECONDARY | SPI_MODE_TX_DMA_SECONDARY;

// Please do not use.
extern const stm32l4_spi_pins_t g_SPI1Pins = { GPIO_PIN_PB5_SPI3_MOSI, GPIO_PIN_PB4_SPI3_MISO, GPIO_PIN_PB3_SPI3_SCK, GPIO_PIN_NONE };
extern const unsigned int g_SPI1Instance = SPI_INSTANCE_SPI3;
extern const unsigned int g_SPI1Mode = SPI_MODE_RX_DMA | SPI_MODE_TX_DMA | SPI_MODE_RX_DMA_SECONDARY | SPI_MODE_TX_DMA_SECONDARY;

// Please do not use.
extern const stm32l4_spi_pins_t g_SPI2Pins = { GPIO_PIN_PB15_SPI2_MOSI, GPIO_PIN_PB14_SPI2_MISO, GPIO_PIN_PB13_SPI2_SCK, GPIO_PIN_NONE };
extern const unsigned int g_SPI2Instance = SPI_INSTANCE_SPI2;
extern const unsigned int g_SPI2Mode = 0;

// Main I2C
extern const stm32l4_i2c_pins_t g_WirePins = { GPIO_PIN_PB6_I2C1_SCL, GPIO_PIN_PB9_I2C1_SDA };
extern const unsigned int g_WireInstance = I2C_INSTANCE_I2C1;
extern const unsigned int g_WireMode = I2C_MODE_RX_DMA;

//extern const stm32l4_i2c_pins_t g_WirePins = { GPIO_PIN_PB8_I2C1_SCL, GPIO_PIN_PB9_I2C1_SDA };
//extern const unsigned int g_WireInstance = I2C_INSTANCE_I2C1;
//extern const unsigned int g_WireMode = I2C_MODE_RX_DMA;

// Please do not use.
extern const stm32l4_i2c_pins_t g_Wire1Pins = { GPIO_PIN_PB13_I2C2_SCL, GPIO_PIN_PB14_I2C2_SDA };
extern const unsigned int g_Wire1Instance = I2C_INSTANCE_I2C2;
extern const unsigned int g_Wire1Mode = 0;

// I2S
extern const stm32l4_sai_pins_t g_SAIPins = { GPIO_PIN_PA8_SAI1_SCK_A, GPIO_PIN_PA9_SAI1_FS_A, GPIO_PIN_PA10_SAI1_SD_A, GPIO_PIN_PB14_SAI1_MCLK_A };
extern const unsigned int g_SAIInstance = SAI_INSTANCE_SAI1A;
extern const unsigned int g_SAIMode = SAI_MODE_DMA | SAI_MODE_DMA_SECONDARY;

// Please do not use.
extern const stm32l4_sai_pins_t g_SAI1Pins = { GPIO_PIN_PB3_SAI1_SCK_B, GPIO_PIN_PB6_SAI1_FS_B, GPIO_PIN_PB5_SAI1_SD_B, GPIO_PIN_PB4_SAI1_MCLK_B };
extern const unsigned int g_SAI1Instance = SAI_INSTANCE_SAI1B;
extern const unsigned int g_SAI1Mode = SAI_MODE_DMA | SAI_MODE_DMA_SECONDARY;

