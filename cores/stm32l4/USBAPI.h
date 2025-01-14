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

#if defined(USBCON) // USBCONN
#pragma once

#include "HardwareSerial.h"
#include "stm32l4_usbd_cdc.h"

#define USB_TYPE_NONE        0
#define USB_TYPE_CDC         1
#define USB_TYPE_CDC_MSC     2
#define USB_TYPE_CDC_HID     3
#define USB_TYPE_CDC_MSC_HID 4
#define USB_TYPE_CDC_DAP     5
#define USB_TYPE_CDC_MSC_DAP 6
#define USB_TYPE_CDC_WEBUSB  7
#define USB_TYPE_CDC_MSC_WEBUSB 8

#if (USB_TYPE == USB_TYPE_CDC)
#define USB_CLASS USBD_CDC_Initialize
#define USB_CLASS_CDC
#endif
#if (USB_TYPE == USB_TYPE_CDC_MSC)
#define USB_CLASS USBD_CDC_MSC_Initialize
#define USB_CLASS_CDC
#define USB_CLASS_MSC
#endif
#if (USB_TYPE == USB_TYPE_CDC_HID)
#define USB_CLASS USBD_CDC_HID_Initialize
#define USB_CLASS_CDC
#define USB_CLASS_HID
#endif
#if (USB_TYPE == USB_TYPE_CDC_MSC_HID)
#define USB_CLASS USBD_CDC_MSC_HID_Initialize
#define USB_CLASS_CDC
#define USB_CLASS_MSC
#define USB_CLASS_HID
#endif
#if (USB_TYPE == USB_TYPE_CDC_DAP)
#define USB_CLASS USBD_CDC_DAP_Initialize
#define USB_CLASS_CDC
#define USB_CLASS_DAP
#endif
#if (USB_TYPE == USB_TYPE_CDC_MSC_DAP)
#define USB_CLASS USBD_CDC_MSC_DAP_Initialize
#define USB_CLASS_CDC
#define USB_CLASS_MSC
#define USB_CLASS_DAP
#endif
#if (USB_TYPE == USB_TYPE_CDC_WEBUSB)
#define USB_CLASS USBD_CDC_WEBUSB_Initialize
#define USB_CLASS_CDC
#define USB_CLASS_WEBUSB
#endif
#if (USB_TYPE == USB_TYPE_CDC_MSC_WEBUSB)
#define USB_CLASS USBD_CDC_MSC_WEBUSB_Initialize
#define USB_CLASS_CDC
#define USB_CLASS_MSC
#define USB_CLASS_WEBUSB
#endif

#ifdef USB_CLASS_WEBUSB

template<int LEN>
struct __attribute__((packed)) WebUSBLandingPage {
  uint8_t length, descriptor_type, https;
  char url[LEN];
};

// For internal use
#define SetWebUSBLandingPage2(HTTPS, STR, ATTRS)				\
WebUSBLandingPage<sizeof(STR)> WEBUSB_landingpage ATTRS = { sizeof(STR) - sizeof("") + 3, 3, HTTPS, STR }

// Use this one
#define SetWebUSBLandingPage(HTTPS, STR)  SetWebUsbLandingPate2(HTTPS,STR,ATTRS)

#endif


class USBDeviceClass
{
public:
    USBDeviceClass() {};

    // USB Device API
    void init();
    bool attach();
    bool detach();
    void poll();
    
    bool connected();
    bool configured();
    bool suspended();

private:
    bool initialized;
};

extern USBDeviceClass USBDevice;

#define CDC_RX_BUFFER_SIZE 256
#define CDC_TX_BUFFER_SIZE 256

class CDC_BASE : public HardwareSerial
{
public:
    void begin(unsigned long baudRate);
    void begin(unsigned long baudrate, uint16_t config);
    void end(void);

    int available(void);
    int availableForWrite(void);
    int peek(void);
    
    int read(void);
    void flush(void);
    size_t write(uint8_t);
    size_t write(const uint8_t *buffer, size_t size);
    using Print::write; // pull in write(str) from Print

    operator bool();

    // These return the settings specified by the USB host for the
    // serial port. These aren't really used, but are offered here
    // in case a sketch wants to act on these settings.
    uint32_t baud();
    uint8_t stopbits();
    uint8_t paritytype();
    uint8_t numbits();
    bool dtr();
    bool rts();

    enum {
	ONE_STOP_BIT = 0,
	ONE_AND_HALF_STOP_BIT = 1,
	TWO_STOP_BITS = 2,
    };
    enum {
	NO_PARITY = 0,
	ODD_PARITY = 1,
	EVEN_PARITY = 2,
	MARK_PARITY = 3,
	SPACE_PARITY = 4,
    };

    // STM32L4 EXTENSTION: non-blocking multi-byte read
    size_t read(uint8_t *buffer, size_t size);

    // STM32L4 EXTENSTION: asynchronous write with callback
    bool write(const uint8_t *buffer, size_t size, void(*callback)(void));
    bool done(void);

    // STM32L4 EXTENSTION: asynchronous receive
    void onReceive(void(*callback)(void));

    // STM32L4 EXTENSTION: enable/disabe blocking writes
    void blockOnOverrun(bool enable);

protected:
    struct  _stm32l4_usbd_cdc_t _usbd_cdc;
    bool _blocking;
    uint8_t _rx_data[CDC_RX_BUFFER_SIZE];
    uint8_t _tx_data[CDC_TX_BUFFER_SIZE];
    volatile uint16_t _tx_write;
    volatile uint16_t _tx_read;
    volatile uint32_t _tx_count;
    volatile uint32_t _tx_size;

    const uint8_t *_tx_data2;
    volatile uint32_t _tx_size2;

    volatile uint32_t _tx_timeout;

    void (*_completionCallback)(void);
    void (*_receiveCallback)(void);

    static void _event_callback(void *context, uint32_t events);
    void EventCallback(uint32_t events);
};

class CDC : public CDC_BASE {
public:
  explicit CDC(bool serialEvent);
};

extern CDC Serial;

class WEBUSB : public CDC_BASE {
public:
  WEBUSB();
};

extern WEBUSB WebUSBSerial;

#define MOUSE_LEFT 1
#define MOUSE_RIGHT 2
#define MOUSE_MIDDLE 4
#define MOUSE_ALL (MOUSE_LEFT | MOUSE_RIGHT | MOUSE_MIDDLE)

class MouseClass
{
public:
    MouseClass(void);
    void begin(void);
    void end(void);
    void click(uint8_t b = MOUSE_LEFT);
    void move(signed char x, signed char y, signed char wheel = 0);	
    void press(uint8_t b = MOUSE_LEFT);		// press LEFT by default
    void release(uint8_t b = MOUSE_LEFT);	// release LEFT by default
    bool isPressed(uint8_t b = MOUSE_LEFT);	// check LEFT by default

private:
    uint8_t _buttons;
    void buttons(uint8_t b);
};

extern MouseClass Mouse;

#define KEY_LEFT_CTRL		0x80
#define KEY_LEFT_SHIFT		0x81
#define KEY_LEFT_ALT		0x82
#define KEY_LEFT_GUI		0x83
#define KEY_RIGHT_CTRL		0x84
#define KEY_RIGHT_SHIFT		0x85
#define KEY_RIGHT_ALT		0x86
#define KEY_RIGHT_GUI		0x87

#define KEY_UP_ARROW		0xDA
#define KEY_DOWN_ARROW		0xD9
#define KEY_LEFT_ARROW		0xD8
#define KEY_RIGHT_ARROW		0xD7
#define KEY_BACKSPACE		0xB2
#define KEY_TAB			0xB3
#define KEY_RETURN		0xB0
#define KEY_ESC			0xB1
#define KEY_INSERT		0xD1
#define KEY_DELETE		0xD4
#define KEY_PAGE_UP		0xD3
#define KEY_PAGE_DOWN		0xD6
#define KEY_HOME		0xD2
#define KEY_END			0xD5
#define KEY_CAPS_LOCK		0xC1
#define KEY_F1			0xC2
#define KEY_F2			0xC3
#define KEY_F3			0xC4
#define KEY_F4			0xC5
#define KEY_F5			0xC6
#define KEY_F6			0xC7
#define KEY_F7			0xC8
#define KEY_F8			0xC9
#define KEY_F9			xCA
#define KEY_F10			0xCB
#define KEY_F11			0xCC
#define KEY_F12			0xCD

//	Low level key report: up to 6 keys and shift, ctrl etc at once
typedef struct
{
    uint8_t modifiers;
    uint8_t reserved;
    uint8_t keys[6];
} KeyReport;

class KeyboardClass: public Print
{
public:
    KeyboardClass(void);
    void begin(void);
    void end(void);
    virtual size_t write(uint8_t k);
    virtual size_t press(uint8_t k);
    virtual size_t release(uint8_t k);
    virtual void releaseAll(void);
private:
    KeyReport _keyReport;
    void sendReport(KeyReport* keys);
};

extern KeyboardClass Keyboard;
#endif  // USBCONN
