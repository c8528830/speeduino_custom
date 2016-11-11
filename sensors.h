#ifndef SENSORS_H
#define SENSORS_H

// The following are alpha values for the ADC filters. 
// Their values are from 0 to 255 with 0 being no filtering and 255 being maximum
#define ADCFILTER_TPS  128
#define ADCFILTER_CLT  180
#define ADCFILTER_IAT  180
#define ADCFILTER_O2  128
#define ADCFILTER_BAT  128

#define BARO_MIN      87
#define BARO_MAX      108

volatile byte flexCounter = 0;
byte MaxAnChannel;

//#define FREE_RUNNING  //This enable the ADC converter after each conversion on ADC interrupt routine

#define ANALOG_ISR  //Comment this line to disable the ADC interrupt routine
#if defined(ANALOG_ISR)
  #if defined(__AVR_ATmega1281__) || defined(__AVR_ATmega2561__)
    int AnChannel[7];
  #else
    int AnChannel[15];
  #endif
#endif

/*
 * Simple low pass IIR filter macro for the analog inputs
 * This is effectively implementing the smooth filter from http://playground.arduino.cc/Main/Smooth
 * But removes the use of floats and uses 8 bits of fixed precision. 
 */
#define ADC_FILTER(input, alpha, prior) (((long)input * (256 - alpha) + ((long)prior * alpha))) >> 8

void instanteneousMAPReading();
void readMAP();
void flexPulse();

unsigned int tempReading;

#if defined(ANALOG_ISR)
//Analog ISR interrupt routine
ISR(ADC_vect)
{
  byte nChannel;
  int result = ADCL | (ADCH << 8);
  
  ADCSRA = 0x6E;  // ADC disabled by clearing bit 7(ADEN)
  nChannel = ADMUX & 0x07;
  #if defined(__AVR_ATmega1281__) || defined(__AVR_ATmega2561__)
    if (nChannel==7) { ADMUX = 0x40; }
  #elif defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
    if(ADCSRB & 0x08) { nChannel+=8; }  //8 to 15
    if (nChannel==7) //channel 7
    { 
      ADMUX = 0x40;
      ADCSRB = 0x08; //Set MUX5 bit
    }
    else if((nChannel==MaxAnChannel) || (nChannel==15)) //Limit the analog reads to the maximum used in board
    {
      ADMUX = 0x40; //channel 0
      ADCSRB = 0x00; //clear MUX5 bit
    }
  #endif
    else { ADMUX++; }
  AnChannel[nChannel] = result;
  
  #if defined(FREE_RUNNING)
    ADCSRA = 0xEE;  // ADC conversion and interrupt enabled by setting bit 7(ADEN)
  #endif
}
#endif

#endif // SENSORS_H
