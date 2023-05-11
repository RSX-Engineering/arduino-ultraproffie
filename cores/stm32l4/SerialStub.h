
#pragma once

#include "HardwareSerial.h"


class SerialStub : public HardwareSerial
{
	public: 
	SerialStub();
	SerialStub(bool a);
	~SerialStub(){};
  
    void begin(unsigned long baudRate);
    void begin(unsigned long baudrate, uint16_t config);
    void end();
    int available(void);
    int peek(void);
    int read(void);
	size_t read(uint8_t *buffer, size_t size);
    void flush(void);
    //size_t write(uint8_t);
	size_t write(const uint8_t data);
    size_t write(const uint8_t *buffer, size_t size);
    using Print::write; // pull in write(str) and write(buf, size) from Print
    operator bool(){something = 0; return false;}
	private:
	uint8_t something;
};

extern SerialStub EmptySerial;

