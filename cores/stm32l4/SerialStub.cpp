#include "SerialStub.h"

SerialStub::SerialStub() {something = 0;}
SerialStub::SerialStub(bool a) {something = 0;}
void SerialStub::begin(unsigned long baudRate) {something = 0;}
void SerialStub::begin(unsigned long baudrate, uint16_t config) {something = 0;}
void SerialStub::end() {something = 0;}
int SerialStub::available(void) {something = 0;  return 0;}
int SerialStub::peek(void) {something = 0;  return 0;}
int SerialStub::read(void) {something = 0;  return 0;}
size_t SerialStub::read(uint8_t *buffer, size_t size){return size;}
void SerialStub::flush(void) {something = 0;}
size_t SerialStub::write(const uint8_t data){something = 0;  return 1;}
size_t SerialStub::write(const uint8_t *buffer, size_t size){something = 0;  return size;}
SerialStub EmptySerial(false);
