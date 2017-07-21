#ifndef CANCOMMS_H
#define CANCOMMS_H
//These are the page numbers that the Tuner Studio serial protocol uses to transverse the different map and config pages.
#define veMapPage    1

uint8_t currentcanCommand;
uint8_t currentCanPage = 1;//Not the same as the speeduino config page numbers
uint8_t nCanretry = 0;      //no of retrys
uint8_t cancmdfail = 0;     //command fail yes/no
uint8_t canlisten = 0;
uint8_t Lbuffer[8];         //8 byte buffer to store incomng can data
uint8_t Gdata[9];
uint8_t Glow, Ghigh;

#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
  HardwareSerial &CANSerial = Serial3;
#elif defined(CORE_STM32)
  SerialUART &CANSerial = Serial2;
  //HardwareSerial &CANSerial = Serial2;
#elif defined(CORE_TEENSY)
  HardwareSerial &CANSerial = Serial2;
#endif

void canCommand();//This is the heart of the Command Line Interpeter.  All that needed to be done was to make it human readable.
void sendCancommand(uint8_t cmdtype , uint16_t canadddress, uint8_t candata1, uint8_t candata2, uint16_t paramgroup);

#endif // CANCOMMS_H
