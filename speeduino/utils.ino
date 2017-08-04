/*
  Speeduino - Simple engine management for the Arduino Mega 2560 platform
  Copyright (C) Josh Stewart
  A full copy of the license may be found in the projects root directory
*/


/*
  Returns how much free dynamic memory exists (between heap and stack)
  This function is one big MISRA violation. MISRA advisories forbid directly poking at memory addresses, however there is no other way of determining heap size on embedded systems.
*/
#include "utils.h"

uint16_t freeRam ()
{
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
  extern int __heap_start, *__brkval;
  uint16_t v;

  return (uint16_t) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);

#elif defined(CORE_TEENSY)
  uint32_t stackTop;
  uint32_t heapTop;

  // current position of the stack.
  stackTop = (uint32_t) &stackTop;

  // current position of heap.
  void* hTop = malloc(1);
  heapTop = (uint32_t) hTop;
  free(hTop);

  // The difference is the free, available ram.
  return (uint16_t)stackTop - heapTop;
#elif defined(CORE_STM32)
  char top = 't';
  return &top - reinterpret_cast<char*>(sbrk(0));

#endif
}

void setPinMapping(byte boardID)
{
  //This is dumb, but it'll do for now to get things compiling
  switch (boardID)
  {
    case 0:
      //Pin mappings as per the v0.1 shield
      pinInjector1 = 8; //Output pin injector 1 is on
      pinInjector2 = 9; //Output pin injector 2 is on
      pinInjector3 = 11; //Output pin injector 3 is on
      pinInjector4 = 10; //Output pin injector 4 is on
      pinInjector5 = 12; //Output pin injector 5 is on
      pinCoil1 = 6; //Pin for coil 1
      pinCoil2 = 7; //Pin for coil 2
      pinCoil3 = 12; //Pin for coil 3
      pinCoil4 = 13; //Pin for coil 4
      pinCoil5 = 14; //Pin for coil 5
      pinTrigger = 2; //The CAS pin
      pinTrigger2 = 3; //The CAS pin
      pinTPS = A0; //TPS input pin
      pinMAP = A1; //MAP sensor pin
      pinIAT = A2; //IAT sensor pin
      pinCLT = A3; //CLS sensor pin
      pinO2 = A4; //O2 Sensor pin
      pinIdle1 = 46; //Single wire idle control
      pinIdle2 = 47; //2 wire idle control
      pinStepperDir = 16; //Direction pin  for DRV8825 driver
      pinStepperStep = 17; //Step pin for DRV8825 driver
      pinFan = 47; //Pin for the fan output
      pinFuelPump = 4; //Fuel pump output
      pinTachOut = 49; //Tacho output pin
      pinFlex = 19; // Flex sensor (Must be external interrupt enabled)
      break;
    case 1:
      //Pin mappings as per the v0.2 shield
      pinInjector1 = 8; //Output pin injector 1 is on
      pinInjector2 = 9; //Output pin injector 2 is on
      pinInjector3 = 10; //Output pin injector 3 is on
      pinInjector4 = 11; //Output pin injector 4 is on
      pinInjector5 = 12; //Output pin injector 5 is on
      pinCoil1 = 28; //Pin for coil 1
      pinCoil2 = 24; //Pin for coil 2
      pinCoil3 = 40; //Pin for coil 3
      pinCoil4 = 36; //Pin for coil 4
      pinCoil5 = 34; //Pin for coil 5 PLACEHOLDER value for now
      pinTrigger = 20; //The CAS pin
      pinTrigger2 = 21; //The Cam Sensor pin
      pinTPS = A2; //TPS input pin
      pinMAP = A3; //MAP sensor pin
      pinIAT = A0; //IAT sensor pin
      pinCLT = A1; //CLS sensor pin
      pinO2 = A8; //O2 Sensor pin
      pinBat = A4; //Battery reference voltage pin
      pinDisplayReset = 48; // OLED reset pin
      pinTachOut = 49; //Tacho output pin
      pinIdle1 = 30; //Single wire idle control
      pinIdle2 = 31; //2 wire idle control
      pinStepperDir = 16; //Direction pin  for DRV8825 driver
      pinStepperStep = 17; //Step pin for DRV8825 driver
      pinFan = 47; //Pin for the fan output
      pinFuelPump = 4; //Fuel pump output
      pinFlex = 2; // Flex sensor (Must be external interrupt enabled)
      break;
    case 2:
      //Pin mappings as per the v0.3 shield
      pinInjector1 = 8; //Output pin injector 1 is on
      pinInjector2 = 9; //Output pin injector 2 is on
      pinInjector3 = 10; //Output pin injector 3 is on
      pinInjector4 = 11; //Output pin injector 4 is on
      pinInjector5 = 12; //Output pin injector 5 is on
      pinCoil1 = 28; //Pin for coil 1
      pinCoil2 = 24; //Pin for coil 2
      pinCoil3 = 40; //Pin for coil 3
      pinCoil4 = 36; //Pin for coil 4
      pinCoil5 = 34; //Pin for coil 5 PLACEHOLDER value for now
      pinTrigger = 19; //The CAS pin
      pinTrigger2 = 18; //The Cam Sensor pin
      pinTPS = A2;//TPS input pin
      pinMAP = A3; //MAP sensor pin
      pinIAT = A0; //IAT sensor pin
      pinCLT = A1; //CLS sensor pin
      pinO2 = A8; //O2 Sensor pin
      pinBat = A4; //Battery reference voltage pin
      pinDisplayReset = 48; // OLED reset pin
      pinTachOut = 49; //Tacho output pin
      pinIdle1 = 5; //Single wire idle control
      pinIdle2 = 53; //2 wire idle control
      pinBoost = 7; //Boost control
      pinVVT_1 = 6; //Default VVT output
      pinFuelPump = 4; //Fuel pump output
      pinStepperDir = 16; //Direction pin  for DRV8825 driver
      pinStepperStep = 17; //Step pin for DRV8825 driver
      pinStepperEnable = 26; //Enable pin for DRV8825
      pinFan = A13; //Pin for the fan output
      pinLaunch = 12; //Can be overwritten below
      pinFlex = 2; // Flex sensor (Must be external interrupt enabled)

      #if defined(CORE_TEENSY)
        pinTrigger = 23;
        pinStepperDir = 33;
        pinStepperStep = 34;
        pinCoil1 = 31;
        pinTachOut = 28;
        pinFan = 27;
        pinCoil4 = 21;
        pinCoil3 = 30;
        pinO2 = A22;
      #endif
      break;

    case 3:
      //Pin mappings as per the v0.4 shield
      pinInjector1 = 8; //Output pin injector 1 is on
      pinInjector2 = 9; //Output pin injector 2 is on
      pinInjector3 = 10; //Output pin injector 3 is on
      pinInjector4 = 11; //Output pin injector 4 is on
      pinInjector5 = 12; //Output pin injector 5 is on
      pinCoil1 = 40; //Pin for coil 1
      pinCoil2 = 38; //Pin for coil 2
      pinCoil3 = 52; //Pin for coil 3
      pinCoil4 = 50; //Pin for coil 4
      pinCoil5 = 34; //Pin for coil 5 PLACEHOLDER value for now
      pinTrigger = 19; //The CAS pin
      pinTrigger2 = 18; //The Cam Sensor pin
      pinTPS = A2;//TPS input pin
      pinMAP = A3; //MAP sensor pin
      pinIAT = A0; //IAT sensor pin
      pinCLT = A1; //CLS sensor pin
      pinO2 = A8; //O2 Sensor pin
      pinBat = A4; //Battery reference voltage pin
      pinDisplayReset = 48; // OLED reset pin
      pinTachOut = 49; //Tacho output pin  (Goes to ULN2803)
      pinIdle1 = 5; //Single wire idle control
      pinIdle2 = 6; //2 wire idle control
      pinBoost = 7; //Boost control
      pinVVT_1 = 4; //Default VVT output
      pinFuelPump = 45; //Fuel pump output  (Goes to ULN2803)
      pinStepperDir = 16; //Direction pin  for DRV8825 driver
      pinStepperStep = 17; //Step pin for DRV8825 driver
      pinStepperEnable = 24; //Enable pin for DRV8825
      pinFan = 47; //Pin for the fan output (Goes to ULN2803)
      pinLaunch = 12; //Can be overwritten below
      pinFlex = 2; // Flex sensor (Must be external interrupt enabled)

      #if defined(CORE_TEENSY)
        pinTrigger = 23;
        pinTrigger2 = 35;
        pinStepperDir = 33;
        pinStepperStep = 34;
        pinCoil1 = 31;
        pinTachOut = 28;
        pinFan = 27;
        pinCoil4 = 29;
        pinCoil3 = 30;
        pinO2 = A22;
      #elif defined(STM32F4)
        pinInjector1 = PE11; //Output pin injector 1 is on
        pinInjector2 = PE12; //Output pin injector 2 is on
        pinInjector3 = PE13; //Output pin injector 3 is on
        pinInjector4 = PE14; //Output pin injector 4 is on
        pinInjector5 = PE15; //Output pin injector 4 is on
        pinCoil1 = PB10; //Pin for coil 1
        pinCoil2 = PB11; //Pin for coil 2
        pinCoil3 = PB12; //Pin for coil 3
        pinCoil4 = PB13; //Pin for coil 4
        pinCoil5 = PB14; //Pin for coil 5
        pinTPS = A0; //TPS input pin
        pinMAP = A1; //MAP sensor pin
        pinIAT = A2; //IAT sensor pin
        pinCLT = A3; //CLS sensor pin
        pinO2 = A4; //O2 Sensor pin
        pinBat = A5; //Battery reference voltage pin
        pinBaro = A6;        
        pinStepperDir = PD8; //Direction pin  for DRV8825 driver
        pinStepperStep = PB15; //Step pin for DRV8825 driver
        pinStepperEnable = PD9; //Enable pin for DRV8825
        pinDisplayReset = PE1; // OLED reset pin
        pinFan = PE2; //Pin for the fan output
        pinFuelPump = PA6; //Fuel pump output
        pinTachOut = PA7; //Tacho output pin
        //external interrupt enabled pins
        pinFlex = PC4; // Flex sensor (Must be external interrupt enabled)
        pinTrigger = PC5; //The CAS pin
        pinTrigger2 = PC6; //The Cam Sensor pin
      #elif defined(CORE_STM32)
        //http://docs.leaflabs.com/static.leaflabs.com/pub/leaflabs/maple-docs/0.0.12/hardware/maple-mini.html#master-pin-map
        //pins 23, 24 and 33 couldn't be used
        pinInjector1 = 15; //Output pin injector 1 is on
        pinInjector2 = 16; //Output pin injector 2 is on
        pinInjector3 = 17; //Output pin injector 3 is on
        pinInjector4 = 18; //Output pin injector 4 is on
        pinCoil1 = 19; //Pin for coil 1
        pinCoil2 = 20; //Pin for coil 2
        pinCoil3 = 21; //Pin for coil 3
        pinCoil4 = 26; //Pin for coil 4
        pinCoil5 = 27; //Pin for coil 5
        pinTPS = A0; //TPS input pin
        pinMAP = A1; //MAP sensor pin
        pinIAT = A2; //IAT sensor pin
        pinCLT = A3; //CLS sensor pin
        pinO2 = A4; //O2 Sensor pin
        pinBat = A5; //Battery reference voltage pin
        pinStepperDir = 12; //Direction pin  for DRV8825 driver
        pinStepperStep = 13; //Step pin for DRV8825 driver
        pinStepperEnable = 14; //Enable pin for DRV8825
        pinDisplayReset = 2; // OLED reset pin
        pinFan = 1; //Pin for the fan output
        pinFuelPump = 0; //Fuel pump output
        pinTachOut = 31; //Tacho output pin
        //external interrupt enabled pins
        pinFlex = 32; // Flex sensor (Must be external interrupt enabled)
        pinTrigger = 25; //The CAS pin
        pinTrigger2 = 22; //The Cam Sensor pin
        pinBaro = pinMAP;
      #endif
      break;

    case 9:
      //Pin mappings as per the MX5 PNP shield
      pinInjector1 = 11; //Output pin injector 1 is on
      pinInjector2 = 10; //Output pin injector 2 is on
      pinInjector3 = 8; //Output pin injector 3 is on
      pinInjector4 = 9; //Output pin injector 4 is on
      pinInjector5 = 14; //Output pin injector 5 is on
      pinCoil1 = 39; //Pin for coil 1
      pinCoil2 = 41; //Pin for coil 2
      pinCoil3 = 35; //Pin for coil 3
      pinCoil4 = 37; //Pin for coil 4
      pinCoil5 = 34; //Pin for coil 5 PLACEHOLDER value for now
      pinTrigger = 19; //The CAS pin
      pinTrigger2 = 18; //The Cam Sensor pin
      pinTPS = A2;//TPS input pin
      pinMAP = A5; //MAP sensor pin
      pinIAT = A0; //IAT sensor pin
      pinCLT = A1; //CLS sensor pin
      pinO2 = A3; //O2 Sensor pin
      pinBat = A4; //Battery reference voltage pin
      pinDisplayReset = 48; // OLED reset pin
      pinTachOut = 49; //Tacho output pin  (Goes to ULN2803)
      pinIdle1 = 2; //Single wire idle control
      pinBoost = 4;
      pinIdle2 = 4; //2 wire idle control (Note this is shared with boost!!!)
      pinFuelPump = 37; //Fuel pump output  (Goes to ULN2803)
      pinStepperDir = 16; //Direction pin  for DRV8825 driver
      pinStepperStep = 17; //Step pin for DRV8825 driver
      pinFan = 47; //Pin for the fan output (Goes to ULN2803)
      pinLaunch = 12; //Can be overwritten below
      pinFlex = 3; // Flex sensor (Must be external interrupt enabled)
      break;

    case 10:
      //Pin mappings for user turtanas PCB
      pinInjector1 = 4; //Output pin injector 1 is on
      pinInjector2 = 5; //Output pin injector 2 is on
      pinInjector3 = 6; //Output pin injector 3 is on
      pinInjector4 = 7; //Output pin injector 4 is on
      pinInjector5 = 8; //Placeholder only - NOT USED
      pinInjector6 = 9; //Placeholder only - NOT USED
      pinInjector7 = 10; //Placeholder only - NOT USED
      pinInjector8 = 11; //Placeholder only - NOT USED
      pinCoil1 = 24; //Pin for coil 1
      pinCoil2 = 28; //Pin for coil 2
      pinCoil3 = 36; //Pin for coil 3
      pinCoil4 = 40; //Pin for coil 4
      pinCoil5 = 34; //Pin for coil 5 PLACEHOLDER value for now
      pinTrigger = 18; //The CAS pin
      pinTrigger2 = 19; //The Cam Sensor pin
      pinTPS = A2;//TPS input pin
      pinMAP = A3; //MAP sensor pin
      pinMAP2 = A8; //MAP2 sensor pin
      pinIAT = A0; //IAT sensor pin
      pinCLT = A1; //CLS sensor pin
      pinO2 = A4; //O2 Sensor pin
      pinBat = A7; //Battery reference voltage pin
      pinDisplayReset = 48; // OLED reset pin
      pinSpareTemp1 = A6;
      pinSpareTemp2 = A5;
      pinTachOut = 41; //Tacho output pin transistori puuttuu 2n2222 tähän ja 1k 12v
      pinFuelPump = 42; //Fuel pump output 2n2222
      pinFan = 47; //Pin for the fan output
      pinTachOut = 49; //Tacho output pin
      pinFlex = 3; // Flex sensor (Must be external interrupt enabled)
      pinBoost = 5;
      pinIdle1 = 6;
      break;

    case 20:
      //Pin mappings as per the Plazomat In/Out shields Rev 0.1
      pinInjector1 = 8; //Output pin injector 1 is on
      pinInjector2 = 9; //Output pin injector 2 is on
      pinInjector3 = 10; //Output pin injector 3 is on
      pinInjector4 = 11; //Output pin injector 4 is on
      pinInjector5 = 12; //Output pin injector 5 is on
      pinCoil1 = 28; //Pin for coil 1
      pinCoil2 = 24; //Pin for coil 2
      pinCoil3 = 40; //Pin for coil 3
      pinCoil4 = 36; //Pin for coil 4
      pinCoil5 = 34; //Pin for coil 5 PLACEHOLDER value for now
      pinSpareOut1 = 4; //Spare LSD Output 1(PWM)
      pinSpareOut2 = 5; //Spare LSD Output 2(PWM)
      pinSpareOut3 = 6; //Spare LSD Output 3(PWM)
      pinSpareOut4 = 7; //Spare LSD Output 4(PWM)
      pinSpareOut5 = 50; //Spare LSD Output 5(digital)
      pinSpareOut6 = 52; //Spare LSD Output 6(digital)
      pinTrigger = 20; //The CAS pin
      pinTrigger2 = 21; //The Cam Sensor pin
      pinSpareTemp2 = A15; //spare Analog input 2
      pinSpareTemp1 = A14; //spare Analog input 1
      pinO2 = A8; //O2 Sensor pin
      pinBat = A4; //Battery reference voltage pin
      pinMAP = A3; //MAP sensor pin
      pinTPS = A2;//TPS input pin
      pinCLT = A1; //CLS sensor pin
      pinIAT = A0; //IAT sensor pin
      pinFan = 47; //Pin for the fan output
      pinFuelPump = 4; //Fuel pump output
      pinTachOut = 49; //Tacho output pin
      break;

    case 30:
      //Pin mappings as per the dazv6 shield
      pinInjector1 = 8; //Output pin injector 1 is on
      pinInjector2 = 9; //Output pin injector 2 is on
      pinInjector3 = 10; //Output pin injector 3 is on
      pinInjector4 = 11; //Output pin injector 4 is on
      pinInjector5 = 12; //Output pin injector 5 is on
      pinCoil1 = 40; //Pin for coil 1
      pinCoil2 = 38; //Pin for coil 2
      pinCoil3 = 50; //Pin for coil 3
      pinCoil4 = 52; //Pin for coil 4
      pinCoil5 = 34; //Pin for coil 5 PLACEHOLDER value for now
      pinTrigger = 19; //The CAS pin
      pinTrigger2 = 18; //The Cam Sensor pin
      pinTrigger3 = 17; // cam sensor 2 pin
      pinTPS = A2;//TPS input pin
      pinMAP = A3; //MAP sensor pin
      pinIAT = A0; //IAT sensor pin
      pinCLT = A1; //CLS sensor pin
      pinO2 = A8; //O2 Sensor pin
      pinO2_2 = A9; //O2 sensor pin (second sensor)
      pinBat = A4; //Battery reference voltage pin
      pinDisplayReset = 48; // OLED reset pin
      pinTachOut = 49; //Tacho output pin
      pinIdle1 = 5; //Single wire idle control
      pinFuelPump = 45; //Fuel pump output
      pinStepperDir = 20; //Direction pin  for DRV8825 driver
      pinStepperStep = 21; //Step pin for DRV8825 driver
      pinSpareHOut1 = 4; // high current output spare1
      pinSpareHOut2 = 6; // high current output spare2
      pinBoost = 7;
      pinSpareLOut1 = 43; //low current output spare1
      pinSpareLOut2 = 47;
      pinSpareLOut3 = 49;
      pinSpareLOut4 = 51;
      pinSpareLOut5 = 53;
      pinFan = 47; //Pin for the fan output
      break;

    default:
      //Pin mappings as per the v0.2 shield
      pinInjector1 = 8; //Output pin injector 1 is on
      pinInjector2 = 9; //Output pin injector 2 is on
      pinInjector3 = 10; //Output pin injector 3 is on
      pinInjector4 = 11; //Output pin injector 4 is on
      pinInjector5 = 12; //Output pin injector 5 is on
      pinCoil1 = 28; //Pin for coil 1
      pinCoil2 = 24; //Pin for coil 2
      pinCoil3 = 40; //Pin for coil 3
      pinCoil4 = 36; //Pin for coil 4
      pinCoil5 = 34; //Pin for coil 5 PLACEHOLDER value for now
      pinTrigger = 20; //The CAS pin
      pinTrigger2 = 21; //The Cam Sensor pin
      pinTPS = A2; //TPS input pin
      pinMAP = A3; //MAP sensor pin
      pinIAT = A0; //IAT sensor pin
      pinCLT = A1; //CLS sensor pin
      pinO2 = A8; //O2 Sensor pin
      pinBat = A4; //Battery reference voltage pin
      pinStepperDir = 16; //Direction pin  for DRV8825 driver
      pinStepperStep = 17; //Step pin for DRV8825 driver
      pinDisplayReset = 48; // OLED reset pin
      pinFan = 47; //Pin for the fan output
      pinFuelPump = 4; //Fuel pump output
      pinTachOut = 49; //Tacho output pin
      break;
  }

  //Setup any devices that are using selectable pins
  if ( (configPage3.launchPin != 0) && (configPage3.launchPin < BOARD_NR_GPIO_PINS) ) { pinLaunch = configPage3.launchPin; }
  if ( (configPage2.ignBypassPin != 0) && (configPage2.ignBypassPin < BOARD_NR_GPIO_PINS) ) { pinIgnBypass = configPage2.ignBypassPin; }
  if ( (configPage1.tachoPin != 0) && (configPage1.tachoPin < BOARD_NR_GPIO_PINS) ) { pinTachOut = configPage1.tachoPin; }
  if ( (configPage2.fuelPumpPin != 0) && (configPage2.fuelPumpPin < BOARD_NR_GPIO_PINS) ) { pinFuelPump = configPage2.fuelPumpPin; }
  if ( (configPage4.fanPin != 0) && (configPage4.fanPin < BOARD_NR_GPIO_PINS) ) { pinFan = configPage4.fanPin; }
  if ( (configPage3.boostPin != 0) && (configPage3.boostPin < BOARD_NR_GPIO_PINS) ) { pinBoost = configPage3.boostPin; }
  if ( (configPage3.vvtPin != 0) && (configPage3.vvtPin < BOARD_NR_GPIO_PINS) ) { pinVVT_1 = configPage3.vvtPin; }
  if ( (configPage3.useExtBaro != 0) && (configPage3.baroPin < BOARD_NR_GPIO_PINS) ) { pinBaro = configPage3.baroPin + A0; }

  //Finally, set the relevant pin modes for outputs
  pinMode(pinCoil1, OUTPUT);
  pinMode(pinCoil2, OUTPUT);
  pinMode(pinCoil3, OUTPUT);
  pinMode(pinCoil4, OUTPUT);
  pinMode(pinCoil5, OUTPUT);
  pinMode(pinInjector1, OUTPUT);
  pinMode(pinInjector2, OUTPUT);
  pinMode(pinInjector3, OUTPUT);
  pinMode(pinInjector4, OUTPUT);
  pinMode(pinInjector5, OUTPUT);
  pinMode(pinTachOut, OUTPUT);
  pinMode(pinIdle1, OUTPUT);
  pinMode(pinIdle2, OUTPUT);
  pinMode(pinFuelPump, OUTPUT);
  pinMode(pinIgnBypass, OUTPUT);
  pinMode(pinFan, OUTPUT);
  pinMode(pinStepperDir, OUTPUT);
  pinMode(pinStepperStep, OUTPUT);
  pinMode(pinStepperEnable, OUTPUT);
  pinMode(pinBoost, OUTPUT);
  pinMode(pinVVT_1, OUTPUT);

  inj1_pin_port = portOutputRegister(digitalPinToPort(pinInjector1));
  inj1_pin_mask = digitalPinToBitMask(pinInjector1);
  inj2_pin_port = portOutputRegister(digitalPinToPort(pinInjector2));
  inj2_pin_mask = digitalPinToBitMask(pinInjector2);
  inj3_pin_port = portOutputRegister(digitalPinToPort(pinInjector3));
  inj3_pin_mask = digitalPinToBitMask(pinInjector3);
  inj4_pin_port = portOutputRegister(digitalPinToPort(pinInjector4));
  inj4_pin_mask = digitalPinToBitMask(pinInjector4);
  inj5_pin_port = portOutputRegister(digitalPinToPort(pinInjector5));
  inj5_pin_mask = digitalPinToBitMask(pinInjector5);

  ign1_pin_port = portOutputRegister(digitalPinToPort(pinCoil1));
  ign1_pin_mask = digitalPinToBitMask(pinCoil1);
  ign2_pin_port = portOutputRegister(digitalPinToPort(pinCoil2));
  ign2_pin_mask = digitalPinToBitMask(pinCoil2);
  ign3_pin_port = portOutputRegister(digitalPinToPort(pinCoil3));
  ign3_pin_mask = digitalPinToBitMask(pinCoil3);
  ign4_pin_port = portOutputRegister(digitalPinToPort(pinCoil4));
  ign4_pin_mask = digitalPinToBitMask(pinCoil4);
  ign5_pin_port = portOutputRegister(digitalPinToPort(pinCoil5));
  ign5_pin_mask = digitalPinToBitMask(pinCoil5);

  tach_pin_port = portOutputRegister(digitalPinToPort(pinTachOut));
  tach_pin_mask = digitalPinToBitMask(pinTachOut);
  pump_pin_port = portOutputRegister(digitalPinToPort(pinFuelPump));
  pump_pin_mask = digitalPinToBitMask(pinFuelPump);

  //And for inputs
  pinMode(pinMAP, INPUT);
  pinMode(pinO2, INPUT);
  pinMode(pinO2_2, INPUT);
  pinMode(pinTPS, INPUT);
  pinMode(pinIAT, INPUT);
  pinMode(pinCLT, INPUT);
  pinMode(pinBat, INPUT);
  pinMode(pinBaro, INPUT);
  pinMode(pinTrigger, INPUT);
  pinMode(pinTrigger2, INPUT);
  pinMode(pinTrigger3, INPUT);
  pinMode(pinFlex, INPUT_PULLUP); //Standard GM / Continental flex sensor requires pullup
  if (configPage3.lnchPullRes == true) {
    pinMode(pinLaunch, INPUT_PULLUP);
  }
  else {
    pinMode(pinLaunch, INPUT);  //If Launch Pull Resistor is not set make input float.
  }

  //These must come after the above pinMode statements
  triggerPri_pin_port = portInputRegister(digitalPinToPort(pinTrigger));
  triggerPri_pin_mask = digitalPinToBitMask(pinTrigger);
  triggerSec_pin_port = portInputRegister(digitalPinToPort(pinTrigger2));
  triggerSec_pin_mask = digitalPinToBitMask(pinTrigger2);

  #if defined(CORE_STM32)
  #else
    //Set default values
    digitalWrite(pinMAP, HIGH);
    //digitalWrite(pinO2, LOW);
    digitalWrite(pinTPS, LOW);
  #endif
}

/*
  This function retuns a pulsewidth time (in us) using a either Alpha-N or Speed Density algorithms, given the following:
  REQ_FUEL
  VE: Lookup from the main MAP vs RPM fuel table
  MAP: In KPa, read from the sensor
  GammaE: Sum of Enrichment factors (Cold start, acceleration). This is a multiplication factor (Eg to add 10%, this should be 110)
  injDT: Injector dead time. The time the injector take to open minus the time it takes to close (Both in uS)
  TPS: Throttle position (0% to 100%)

  This function is called by PW_SD and PW_AN for speed0density and pure Alpha-N calculations respectively.
*/
unsigned int PW(int REQ_FUEL, byte VE, byte MAP, int corrections, int injOpen)
{
  //Standard float version of the calculation
  //return (REQ_FUEL * (float)(VE/100.0) * (float)(MAP/100.0) * (float)(TPS/100.0) * (float)(corrections/100.0) + injOpen);
  //Note: The MAP and TPS portions are currently disabled, we use VE and corrections only
  uint16_t iVE, iCorrections;
  uint16_t iMAP = 100;
  uint16_t iAFR = 147;

  //100% float free version, does sacrifice a little bit of accuracy, but not much.
  iVE = ((unsigned int)VE << 7) / 100;
  if ( configPage1.multiplyMAP == true ) {
    iMAP = ((unsigned int)MAP << 7) / currentStatus.baro;  //Include multiply MAP (vs baro) if enabled
  }
  if ( (configPage1.includeAFR == true) && (configPage3.egoType == 2)) {
    iAFR = ((unsigned int)currentStatus.O2 << 7) / currentStatus.afrTarget;  //Include AFR (vs target) if enabled
  }
  iCorrections = (corrections << 7) / 100;


  unsigned long intermediate = ((long)REQ_FUEL * (long)iVE) >> 7; //Need to use an intermediate value to avoid overflowing the long
  if ( configPage1.multiplyMAP == true ) {
    intermediate = (intermediate * (unsigned long)iMAP) >> 7;
  }
  if ( (configPage1.includeAFR == true) && (configPage3.egoType == 2) ) {
    intermediate = (intermediate * (unsigned long)iAFR) >> 7;  //EGO type must be set to wideband for this to be used
  }
  intermediate = (intermediate * (unsigned long)iCorrections) >> 7;
  if (intermediate != 0)
  {
    //If intermeditate is not 0, we need to add the opening time (0 typically indicates that one of the full fuel cuts is active)
    intermediate += injOpen; //Add the injector opening time
    if ( intermediate > 65535)
    {
      intermediate = 65535;  //Make sure this won't overflow when we convert to uInt. This means the maximum pulsewidth possible is 65.535mS
    }
  }
  return (unsigned int)(intermediate);

}

//Convenience functions for Speed Density and Alpha-N
unsigned int PW_SD(int REQ_FUEL, byte VE, byte MAP, int corrections, int injOpen)
{
  return PW(REQ_FUEL, VE, MAP, corrections, injOpen);
  //return PW(REQ_FUEL, VE, 100, corrections, injOpen);
}

unsigned int PW_AN(int REQ_FUEL, byte VE, byte TPS, int corrections, int injOpen)
{
  return PW(REQ_FUEL, VE, currentStatus.MAP, corrections, injOpen);
}
