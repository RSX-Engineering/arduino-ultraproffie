/*
 * Copyright (c) 2016-2017 Thomas Roell.  All rights reserved.
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

#include <stdio.h>
#include "stm32l4xx.h"

#include "armv7m.h"

#include "stm32l4_system.h"
#include "stm32l4_usbd_cdc.h"

#include "usbd_cdc.h"
#include "usbd_webusb.h"

extern void USBD_Detach(void);
extern void USBD_RegisterCallbacks(void(*sof_callback)(void), void(*suspend_callback)(void), void(*resume_callback)(void));

typedef struct _stm32l4_usbd_cdc_device_t {
  struct _USBD_HandleTypeDef     *USBD;
  volatile uint8_t               rx_busy;
  volatile uint8_t               tx_busy;
  volatile uint8_t               tx_flush;
  volatile uint8_t               control;
  volatile uint8_t               suspended;
  uint32_t                       timeout;
  volatile stm32l4_usbd_cdc_info_t cdc_info;
  stm32l4_usbd_cdc_t             *instances[USBD_CDC_INSTANCE_COUNT];
} stm32l4_usbd_cdc_device_t;

static stm32l4_usbd_cdc_device_t stm32l4_usbd_cdc_device;
static stm32l4_usbd_cdc_device_t stm32l4_usbd_webusb_device;

static void stm32l4_usbd_cdc_sof_callback2(stm32l4_usbd_cdc_device_t* device) {
  stm32l4_usbd_cdc_t *usbd_cdc = device->instances[0];
  
  if (device->timeout) 
  {
    if (device->timeout == 1)
    {
      stm32l4_system_dfu();
    }
    else
    {
      device->timeout--;
    }
  }
  
  if (usbd_cdc && (usbd_cdc->state > USBD_CDC_STATE_INIT))
  {
    if (usbd_cdc->events & USBD_CDC_EVENT_SOF)
    {
      (*usbd_cdc->callback)(usbd_cdc->context, USBD_CDC_EVENT_SOF);
    }
  }
}

static void stm32l4_usbd_cdc_sof_callback() {
  stm32l4_usbd_cdc_sof_callback2(&stm32l4_usbd_cdc_device);
  stm32l4_usbd_cdc_sof_callback2(&stm32l4_usbd_webusb_device);
}

static void stm32l4_usbd_cdc_suspend_callback2(stm32l4_usbd_cdc_device_t* device) {
  stm32l4_usbd_cdc_t *usbd_cdc = device->instances[0];
  
  if (usbd_cdc && (usbd_cdc->state > USBD_CDC_STATE_INIT))
  {
    usbd_cdc->state = USBD_CDC_STATE_SUSPENDED;
  }
  
  device->suspended = 1;
  
  if (device->tx_busy && !device->tx_flush)
  {
    device->tx_flush = 1;
    
    if (usbd_cdc && (usbd_cdc->events & USBD_CDC_EVENT_TRANSMIT))
    {
      (*usbd_cdc->callback)(usbd_cdc->context, USBD_CDC_EVENT_TRANSMIT);
    }
  }
}

static void stm32l4_usbd_cdc_suspend_callback() {
  stm32l4_usbd_cdc_suspend_callback2(&stm32l4_usbd_cdc_device);
  stm32l4_usbd_cdc_suspend_callback2(&stm32l4_usbd_webusb_device);
}

static void stm32l4_usbd_cdc_resume_callback2(stm32l4_usbd_cdc_device_t* device) {
  stm32l4_usbd_cdc_t *usbd_cdc = device->instances[0];
  
  device->suspended = 0;
  
  if (usbd_cdc && (usbd_cdc->state == USBD_CDC_STATE_SUSPENDED))
  {
    usbd_cdc->state = device->control ? USBD_CDC_STATE_READY : USBD_CDC_STATE_RESET;
  }
}

static void stm32l4_usbd_cdc_resume_callback() {
  stm32l4_usbd_cdc_resume_callback2(&stm32l4_usbd_cdc_device);
  stm32l4_usbd_cdc_resume_callback2(&stm32l4_usbd_webusb_device);
}

static void stm32l4_usbd_cdc_setrxbuffer(stm32l4_usbd_cdc_device_t* device) {
#if !defined(STM32L431xx)
  stm32l4_usbd_cdc_t *usbd_cdc = device->instances[0];
  if (device == &stm32l4_usbd_cdc_device) {
    USBD_CDC_SetRxBuffer(device->USBD, &usbd_cdc->rx_data[usbd_cdc->rx_write]);
    USBD_CDC_ReceivePacket(device->USBD);
  } else if (device == &stm32l4_usbd_webusb_device) {
    USBD_WEBUSB_SetRxBuffer(device->USBD, &usbd_cdc->rx_data[usbd_cdc->rx_write]);
    USBD_WEBUSB_ReceivePacket(device->USBD);
  }

  device->rx_busy = 1;
#endif

}

static void stm32l4_usbd_cdc_init2(stm32l4_usbd_cdc_device_t* device, USBD_HandleTypeDef *USBD) {
  stm32l4_usbd_cdc_t *usbd_cdc = device->instances[0];
  
  device->USBD = USBD;
  device->rx_busy = 0;
  device->tx_busy = 0;
  device->tx_flush = 0;
  device->control = 0;
  device->suspended = 0;
  device->timeout = 0;
  
  device->cdc_info.dwDTERate = 115200;
  device->cdc_info.bCharFormat = 0;
  device->cdc_info.bParityType = 0;
  device->cdc_info.bDataBits = 8;
  device->cdc_info.lineState = 0;
  
  if (usbd_cdc && (usbd_cdc->state > USBD_CDC_STATE_INIT)) {
    usbd_cdc->state = USBD_CDC_STATE_RESET;
    stm32l4_usbd_cdc_setrxbuffer(device);
  }
  
  USBD_RegisterCallbacks(stm32l4_usbd_cdc_sof_callback,
			 stm32l4_usbd_cdc_suspend_callback,
			 stm32l4_usbd_cdc_resume_callback);
}


static void stm32l4_usbd_cdc_deinit2(stm32l4_usbd_cdc_device_t* device) {
  stm32l4_usbd_cdc_t *usbd_cdc = device->instances[0];
  
  if (usbd_cdc)
  {
    usbd_cdc->state = USBD_CDC_STATE_RESET;
  }
  
  USBD_RegisterCallbacks(NULL, NULL, NULL);
  
  device->control = 0;
  device->suspended = 0;
  
  if (device->tx_busy && !device->tx_flush)
  {
    if (usbd_cdc && (usbd_cdc->events & USBD_CDC_EVENT_TRANSMIT))
    {
      (*usbd_cdc->callback)(usbd_cdc->context, USBD_CDC_EVENT_TRANSMIT);
    }
  }
  
  device->rx_busy = 0;
  device->tx_busy = 0;
  device->tx_flush = 0;
  
  device->USBD = NULL;
}

static void stm32l4_usbd_cdc_control2(stm32l4_usbd_cdc_device_t* device, uint8_t command, uint8_t *data, uint16_t length) {
  stm32l4_usbd_cdc_t *usbd_cdc = device->instances[0];
  
  if (command == USBD_CDC_GET_LINE_CODING)
  {
    data[0] = (uint8_t)(device->cdc_info.dwDTERate >> 0);
    data[1] = (uint8_t)(device->cdc_info.dwDTERate >> 8);
    data[2] = (uint8_t)(device->cdc_info.dwDTERate >> 16);
    data[3] = (uint8_t)(device->cdc_info.dwDTERate >> 24);
    data[4] = device->cdc_info.bCharFormat;
    data[5] = device->cdc_info.bParityType;
    data[6] = device->cdc_info.bDataBits;     
  }
  else
  {
    if (command == USBD_CDC_SET_LINE_CODING)
    {
      device->cdc_info.dwDTERate   = (uint32_t)((data[0] << 0) | (data[1] << 8) | (data[2] << 16) | (data[3] << 24));
      device->cdc_info.bCharFormat = data[4];
      device->cdc_info.bParityType = data[5];
      device->cdc_info.bDataBits   = data[6];
    }
    
    if (command == USBD_CDC_SET_CONTROL_LINE_STATE)
    {
      device->cdc_info.lineState = (uint16_t)((data[2] << 0) | (data[3] << 8));
    }
    
    if ((command == USBD_CDC_SET_LINE_CODING) || (command == USBD_CDC_SET_CONTROL_LINE_STATE))
    {
      if (!device->control)
      {
	device->control = 1;
	
	if (usbd_cdc && (usbd_cdc->state == USBD_CDC_STATE_RESET))
	{
	  usbd_cdc->state = USBD_CDC_STATE_READY;
	}
      }
      
      if ((device->cdc_info.dwDTERate == 1200) && !(device->cdc_info.lineState & 1))
      {
	/* start reset timer */
	device->timeout = 250;
      }
      else
      {
	/* stop reset timer */
	device->timeout = 0;
      }
    }
  }
}

static void stm32l4_usbd_cdc_rx_ready2(stm32l4_usbd_cdc_device_t* device, uint8_t *data, uint32_t length) {
  stm32l4_usbd_cdc_t *usbd_cdc = device->instances[0];
  
  device->rx_busy = 0;
  
  if (usbd_cdc && (usbd_cdc->state > USBD_CDC_STATE_INIT))
  {
    usbd_cdc->rx_write += length;
    
    armv7m_atomic_add(&usbd_cdc->rx_count, length);
    
    if ((usbd_cdc->rx_write + USBD_CDC_DATA_MAX_PACKET_SIZE) > usbd_cdc->rx_size)
    {
      usbd_cdc->rx_wrap = usbd_cdc->rx_write;
      usbd_cdc->rx_write = 0;
    }
    
    if (usbd_cdc->state != USBD_CDC_STATE_RESET)
    {
      if ((usbd_cdc->rx_wrap - usbd_cdc->rx_count) >= USBD_CDC_DATA_MAX_PACKET_SIZE)
      {
	stm32l4_usbd_cdc_setrxbuffer(device);
      }
    }
    
    if (usbd_cdc->events & USBD_CDC_EVENT_RECEIVE)
    {
      (*usbd_cdc->callback)(usbd_cdc->context, USBD_CDC_EVENT_RECEIVE);
    }
  }
}

static void stm32l4_usbd_cdc_tx_done2(stm32l4_usbd_cdc_device_t* device) {
  stm32l4_usbd_cdc_t *usbd_cdc = device->instances[0];
  
  device->tx_busy = 0;
  
  if (device->tx_flush)
  {
    device->tx_flush = 0;
  }
  else
  {
    if (usbd_cdc && (usbd_cdc->state > USBD_CDC_STATE_INIT))
    {
      if (usbd_cdc->events & USBD_CDC_EVENT_TRANSMIT)
      {
	(*usbd_cdc->callback)(usbd_cdc->context, USBD_CDC_EVENT_TRANSMIT);
      }
    }
  }
}

static void stm32l4_usbd_cdc_init(USBD_HandleTypeDef *USBD) {
  stm32l4_usbd_cdc_init2(&stm32l4_usbd_cdc_device, USBD);
}
static void stm32l4_usbd_cdc_deinit(void) { stm32l4_usbd_cdc_deinit2(&stm32l4_usbd_cdc_device); }
static void stm32l4_usbd_cdc_control(uint8_t command, uint8_t *data, uint16_t length) {
  stm32l4_usbd_cdc_control2(&stm32l4_usbd_cdc_device, command, data, length);
}
static void stm32l4_usbd_cdc_rx_ready(uint8_t *data, uint32_t length) {
  stm32l4_usbd_cdc_rx_ready2(&stm32l4_usbd_cdc_device, data, length);
}
static void stm32l4_usbd_cdc_tx_done() { stm32l4_usbd_cdc_tx_done2(&stm32l4_usbd_cdc_device); }

const USBD_CDC_ItfTypeDef stm32l4_usbd_cdc_interface = {
    stm32l4_usbd_cdc_init,
    stm32l4_usbd_cdc_deinit,
    stm32l4_usbd_cdc_control,
    stm32l4_usbd_cdc_rx_ready,
    stm32l4_usbd_cdc_tx_done,
};

static void stm32l4_usbd_webusb_init(USBD_HandleTypeDef *USBD) {
  stm32l4_usbd_cdc_init2(&stm32l4_usbd_webusb_device, USBD);
}
static void stm32l4_usbd_webusb_deinit(void) { stm32l4_usbd_cdc_deinit2(&stm32l4_usbd_webusb_device); }
static void stm32l4_usbd_webusb_control(uint8_t command, uint8_t *data, uint16_t length) {
  stm32l4_usbd_cdc_control2(&stm32l4_usbd_webusb_device, command, data, length);
}
static void stm32l4_usbd_webusb_rx_ready(uint8_t *data, uint32_t length) {
  stm32l4_usbd_cdc_rx_ready2(&stm32l4_usbd_webusb_device, data, length);
}
static void stm32l4_usbd_webusb_tx_done() { stm32l4_usbd_cdc_tx_done2(&stm32l4_usbd_webusb_device); }

const USBD_CDC_ItfTypeDef stm32l4_usbd_webusb_interface = {
    stm32l4_usbd_webusb_init,
    stm32l4_usbd_webusb_deinit,
    stm32l4_usbd_webusb_control,
    stm32l4_usbd_webusb_rx_ready,
    stm32l4_usbd_webusb_tx_done,
};

bool stm32l4_usbd_cdc_create(stm32l4_usbd_cdc_t *usbd_cdc)
{
    usbd_cdc->state = USBD_CDC_STATE_INIT;

    usbd_cdc->rx_data  = NULL;
    usbd_cdc->rx_size  = 0;
    usbd_cdc->rx_read  = 0;
    usbd_cdc->rx_write = 0;
    usbd_cdc->rx_wrap  = 0;
    usbd_cdc->rx_count = 0;

    usbd_cdc->callback = NULL;
    usbd_cdc->context = NULL;
    usbd_cdc->events = 0;

    stm32l4_usbd_cdc_device_t* device = &stm32l4_usbd_cdc_device;
    device->instances[0] = usbd_cdc;
    usbd_cdc->device = (void *)device;

    return true;
}

bool stm32l4_usbd_cdc_create_webusb(stm32l4_usbd_cdc_t *usbd_cdc)
{
    usbd_cdc->state = USBD_CDC_STATE_INIT;

    usbd_cdc->rx_data  = NULL;
    usbd_cdc->rx_size  = 0;
    usbd_cdc->rx_read  = 0;
    usbd_cdc->rx_write = 0;
    usbd_cdc->rx_wrap  = 0;
    usbd_cdc->rx_count = 0;

    usbd_cdc->callback = NULL;
    usbd_cdc->context = NULL;
    usbd_cdc->events = 0;

    stm32l4_usbd_cdc_device_t* device = &stm32l4_usbd_webusb_device;
    device->instances[0] = usbd_cdc;
    usbd_cdc->device = (void *)device;

    return true;
}

bool stm32l4_usbd_cdc_destroy(stm32l4_usbd_cdc_t *usbd_cdc)
{
    if (usbd_cdc->state != USBD_CDC_STATE_INIT)
    {
	return false;
    }
    stm32l4_usbd_cdc_device_t* device = (stm32l4_usbd_cdc_device_t*)usbd_cdc;
    device->instances[0] = NULL;

    return true;
}

bool stm32l4_usbd_cdc_enable(stm32l4_usbd_cdc_t *usbd_cdc, uint8_t *rx_data, uint16_t rx_size, uint32_t option, stm32l4_usbd_cdc_callback_t callback, void *context, uint32_t events)
{
    if (usbd_cdc->state != USBD_CDC_STATE_INIT)
    {
	return false;
    }
    stm32l4_usbd_cdc_device_t* device = (stm32l4_usbd_cdc_device_t*)usbd_cdc->device;

    usbd_cdc->rx_data  = rx_data;
    usbd_cdc->rx_size  = rx_size;
    usbd_cdc->rx_read  = 0;
    usbd_cdc->rx_write = 0;
    usbd_cdc->rx_wrap  = rx_size;
    usbd_cdc->rx_count = 0;

    usbd_cdc->option = option;

    usbd_cdc->callback = callback;
    usbd_cdc->context = context;
    usbd_cdc->events = events;

    if (device->USBD)
    {
	if (device->suspended)
	{
	    usbd_cdc->state = USBD_CDC_STATE_SUSPENDED;
	}
	else
	{
	    usbd_cdc->state = device->control ? USBD_CDC_STATE_READY : USBD_CDC_STATE_RESET;
	}

	stm32l4_usbd_cdc_setrxbuffer(device);
    }
    else
    {
	usbd_cdc->state = USBD_CDC_STATE_RESET;
    }

    return true;
}

bool stm32l4_usbd_cdc_disable(stm32l4_usbd_cdc_t *usbd_cdc)
{
    if (usbd_cdc->state < USBD_CDC_STATE_READY)
    {
	return false;
    }

    usbd_cdc->state = USBD_CDC_STATE_INIT;

    usbd_cdc->rx_data  = NULL;
    usbd_cdc->rx_size  = 0;
    usbd_cdc->rx_read  = 0;
    usbd_cdc->rx_write = 0;
    usbd_cdc->rx_wrap  = 0;
    usbd_cdc->rx_count = 0;

    usbd_cdc->callback = NULL;
    usbd_cdc->context = NULL;
    usbd_cdc->events = 0;

    return true;
}

bool stm32l4_usbd_cdc_notify(stm32l4_usbd_cdc_t *usbd_cdc, stm32l4_usbd_cdc_callback_t callback, void *context, uint32_t events)
{
    if (usbd_cdc->state < USBD_CDC_STATE_READY)
    {
	return false;
    }

    usbd_cdc->events = 0;

    usbd_cdc->callback = callback;
    usbd_cdc->context = context;
    usbd_cdc->events = events;

    return true;
}

unsigned int stm32l4_usbd_cdc_receive(stm32l4_usbd_cdc_t *usbd_cdc, uint8_t *rx_data, uint16_t rx_count)
{
    uint32_t rx_total, rx_size, rx_read, rx_wrap;

    if (usbd_cdc->state < USBD_CDC_STATE_READY)
    {
	return false;
    }
    stm32l4_usbd_cdc_device_t* device = (stm32l4_usbd_cdc_device_t*)usbd_cdc->device;

    rx_size = usbd_cdc->rx_count;

    if (rx_count > rx_size)
    {
	rx_count = rx_size;
    }

    rx_total = rx_count;

    rx_read = usbd_cdc->rx_read;
    rx_wrap = usbd_cdc->rx_wrap;
    rx_size = rx_total;

    if ((rx_read + rx_size) > rx_wrap)
    {
	rx_size = rx_wrap - rx_read;
    }

    memcpy(rx_data, &usbd_cdc->rx_data[rx_read], rx_size);

    rx_read += rx_size;
    rx_total -= rx_size;

    if (rx_read == rx_wrap)
    {
	rx_read = 0;

	usbd_cdc->rx_wrap = usbd_cdc->rx_size;
    }

    usbd_cdc->rx_read = rx_read;

    armv7m_atomic_sub(&usbd_cdc->rx_count, rx_size);

    if (rx_total)
    {
	memcpy(rx_data + rx_size, &usbd_cdc->rx_data[rx_read], rx_total);

	rx_read += rx_total;

	usbd_cdc->rx_read = rx_read;

	armv7m_atomic_sub(&usbd_cdc->rx_count, rx_total);
    }

    if (!device->rx_busy && (usbd_cdc->state != USBD_CDC_STATE_RESET))
    {
	if ((usbd_cdc->rx_wrap - usbd_cdc->rx_count) >= USBD_CDC_DATA_MAX_PACKET_SIZE)
	{
	  stm32l4_usbd_cdc_setrxbuffer(device);
	}
    }

    return rx_count;
}

unsigned int stm32l4_usbd_cdc_count(stm32l4_usbd_cdc_t *usbd_cdc)
{
    if (usbd_cdc->state < USBD_CDC_STATE_READY)
    {
	return 0;
    }

    return usbd_cdc->rx_count;
}

int stm32l4_usbd_cdc_peek(stm32l4_usbd_cdc_t *usbd_cdc)
{
    if (usbd_cdc->state < USBD_CDC_STATE_READY)
    {
	return -1;
    }

    if (!usbd_cdc->rx_count)
    {
	return -1;
    }

    return usbd_cdc->rx_data[usbd_cdc->rx_read];
}

bool stm32l4_usbd_cdc_transmit(stm32l4_usbd_cdc_t *usbd_cdc, const uint8_t *tx_data, uint32_t tx_count)
{
#if !defined(STM32L431xx)
    int status = 1;

    /* QTG_FS interrupts need to be disabled while calling
     * USBD_CDC_TransmitPacket. It seems that on short 
     * transmits HAL_PCD_EP_Transmit()/USB_EPStartXfer()
     * can take a completion interrupt before being done
     * setting up the transfer completele. I don't want
     * to mess around in the USB/HAL code, so the simple
     * WAR is to avoid the race condition by blocking
     * the interrupt till the transfer setup is complete.
     */

#if defined(STM32L476xx) || defined(STM32L496xx)
    NVIC_DisableIRQ(OTG_FS_IRQn);
#elif !defined(STM32L431xx)
    NVIC_DisableIRQ(USB_IRQn);
#endif

    stm32l4_usbd_cdc_device_t* device = (stm32l4_usbd_cdc_device_t*)usbd_cdc->device;
    if ((usbd_cdc->state != USBD_CDC_STATE_READY) || device->tx_busy)
    {
#if defined(STM32L476xx) || defined(STM32L496xx)
	NVIC_EnableIRQ(OTG_FS_IRQn);
#elif !defined(STM32L431xx)
	NVIC_EnableIRQ(USB_IRQn);
#endif

	return false;
    }
    else
    {
	device->tx_busy = 1;

	if (device == &stm32l4_usbd_cdc_device) {
	  USBD_CDC_SetTxBuffer(device->USBD, tx_data, tx_count);
	  status = USBD_CDC_TransmitPacket(device->USBD);
	} else if (device == &stm32l4_usbd_webusb_device) {
	  USBD_WEBUSB_SetTxBuffer(device->USBD, tx_data, tx_count);
	  status = USBD_WEBUSB_TransmitPacket(device->USBD);
	}
	
#if defined(STM32L476xx) || defined(STM32L496xx)
	NVIC_EnableIRQ(OTG_FS_IRQn);
#elif !defined(STM32L431xx)
	NVIC_EnableIRQ(USB_IRQn);
#endif
	
	if (status != 0)
	{
	    return false;
	}
	
	return true;
    }
#else 
	return true;
#endif
}

bool stm32l4_usbd_cdc_done(stm32l4_usbd_cdc_t *usbd_cdc)
{
    stm32l4_usbd_cdc_device_t* device = (stm32l4_usbd_cdc_device_t*)usbd_cdc->device;
    return ((usbd_cdc->state == USBD_CDC_STATE_READY) && !device->tx_busy);
}

void stm32l4_usbd_cdc_poll(stm32l4_usbd_cdc_t *usbd_cdc)
{
    if (usbd_cdc->state >= USBD_CDC_STATE_READY)
    {
#if defined(STM32L476xx) || defined(STM32L496xx)
	OTG_FS_IRQHandler();
#elif !defined(STM32L431xx)
	USB_IRQHandler();
#endif
    }
}

volatile stm32l4_usbd_cdc_info_t* stm32l4_usbd_cdc_info(stm32l4_usbd_cdc_t *usbd_cdc) {
    stm32l4_usbd_cdc_device_t* device = (stm32l4_usbd_cdc_device_t*)usbd_cdc->device;
   return &device->cdc_info;
}
