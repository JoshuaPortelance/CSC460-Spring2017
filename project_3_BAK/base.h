#ifndef _BASE_H
#define _BASE_H

#include <stdint.h>

int DEBUG = 0;
int i; //looper
int MAXINPUT = 50;

//PRESETS
uint16_t joyMax = 1023;
uint16_t joyMin = 0;

//Pins
uint8_t rightJoyYPin    = 10;  // 88 A9
uint8_t rightJoyXPin    = 9; // 87 A10
uint8_t leftJoyYPin     = 12; // 86 A11
uint8_t leftJoyXPin     = 11; // 85 A12
// int rightJoySWPin    = 53; // pin 22
// int leftJoySWPin  = 23;

//Initialization
uint32_t rightXAxisCenter = 0;
uint32_t rightYAxisCenter = 0;
uint32_t leftXAxisCenter  = 0;
uint32_t leftYAxisCenter  = 0;

//Roomba movement INIT
int radiusRoomba = 0;
int speedRoomba  = 0;

// Pan/Tilt speed INIT
int panSpeed  = 0;
int tiltSpeed = 0;

//Laser INIT
int laserState      = 0;   //0 is off, 1 is on.

// Joystick Axis
int rightXAxis      = 0;
int rightYAxis      = 0;
int leftXAxis       = 0;
int leftYAxis       = 0;

//Prototypes
void bt_sendData();
void getSpeeds();
void rightJoySwitch();
void checkJoysticks();
void getJoyCenter();
void init_LED();
void initADC();
void initLaserSwitch();
void initialize();
uint16_t readADC(uint8_t channel);
void Blink();
void a();
void a_main();

////////////////////////////////////////////////////////////////////////////////
//Reference Material

// 1 output, 0 input

/* PORT LIST
 **** Analog
 * A9       rightJoyXPin
 * A10      rightJoyYPin
 * A11      leftJoyYPin
 * A12      leftJoyXPin
 reverse order
 DDRF = 0b11111111    analog port 0-7
 DDRK = 0b11111111    analog port 8-15

 **** DGTL 22-53
 * DGTL18 TX1 Bluetooth send SERIAL1
 * DGTL19 RX1 Bluetooth recieve SERIAL1
 * DGTL22   rightJoySWPin
 * DGTL23   leftJoySWPin //NOT IN USE
 * TEST PINS:
 * DGTL30 bt_getData
 * DGTL31 bt_sendData
 * DGTL32 checkLeftJoyAxis
 * DGTL33 checkRightJoyAxis
 * DGTL34 getLaserSpeed
 * DGTL35 getRoombaSpeed
 * DGTL36 LCDprint
 * DGTL37 rightJoySwitch
 */
/*
DDRF&DDRK is analog



https://docs.google.com/spreadsheets/d/16MIFE4ITEisEDUBh3H4A7WZga1Sm1Pm-igS3r0A58L8/pub?gid=0
https://www.arduino.cc/en/uploads/Hacking/PinMap2560big.png



void digitalWrite(uint8_t pin, uint8_t val)
{
        uint8_t timer = digitalPinToTimer(pin);
        uint8_t bit = digitalPinToBitMask(pin);
        uint8_t port = digitalPinToPort(pin);
        volatile uint8_t *out;

        if (port == NOT_A_PIN) return;

        // If the pin that support PWM output, we need to turn it off
        // before doing a digital write.
        if (timer != NOT_ON_TIMER) turnOffPWM(timer);

        out = portOutputRegister(port);

        uint8_t oldSREG = SREG;
        cli();

        if (val == LOW) {
                *out &= ~bit;
        } else {
                *out |= bit;
        }

        SREG = oldSREG;
}

int digitalRead(uint8_t pin)
{
        uint8_t timer = digitalPinToTimer(pin);
        uint8_t bit = digitalPinToBitMask(pin);
        uint8_t port = digitalPinToPort(pin);

        if (port == NOT_A_PIN) return LOW;

        // If the pin that support PWM output, we need to turn it off
        // before getting a digital reading.
        if (timer != NOT_ON_TIMER) turnOffPWM(timer);

        if (*portInputRegister(port) & bit) return HIGH;
        return LOW;
}

int analogRead(uint8_t pin)
{
        uint8_t low, high;

#if defined(analogPinToChannel)
#if defined(__AVR_ATmega32U4__)
        if (pin >= 18) pin -= 18; // allow for channel or pin numbers
#endif
        pin = analogPinToChannel(pin);
#elif defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
        if (pin >= 54) pin -= 54; // allow for channel or pin numbers
#elif defined(__AVR_ATmega32U4__)
        if (pin >= 18) pin -= 18; // allow for channel or pin numbers
#elif defined(__AVR_ATmega1284__) || defined(__AVR_ATmega1284P__) || defined(__AVR_ATmega644__) || defined(__AVR_ATmega644A__) || defined(__AVR_ATmega644P__) || defined(__AVR_ATmega644PA__)
        if (pin >= 24) pin -= 24; // allow for channel or pin numbers
#else
        if (pin >= 14) pin -= 14; // allow for channel or pin numbers
#endif

#if defined(ADCSRB) && defined(MUX5)
        // the MUX5 bit of ADCSRB selects whether we're reading from channels
        // 0 to 7 (MUX5 low) or 8 to 15 (MUX5 high).
        ADCSRB = (ADCSRB & ~(1 << MUX5)) | (((pin >> 3) & 0x01) << MUX5);
#endif

        // set the analog reference (high two bits of ADMUX) and select the
        // channel (low 4 bits).  this also sets ADLAR (left-adjust result)
        // to 0 (the default).
#if defined(ADMUX)
#if defined(__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
        ADMUX = (analog_reference << 4) | (pin & 0x07);
#else
        ADMUX = (analog_reference << 6) | (pin & 0x07);
#endif
#endif

        // without a delay, we seem to read from the wrong channel
        //delay(1);

#if defined(ADCSRA) && defined(ADCL)
        // start the conversion
        sbi(ADCSRA, ADSC);

        // ADSC is cleared when the conversion finishes
        while (bit_is_set(ADCSRA, ADSC));

        // we have to read ADCL first; doing so locks both ADCL
        // and ADCH until ADCH is read.  reading ADCL second would
        // cause the results of each conversion to be discarded,
        // as ADCL and ADCH would be locked when it completed.
        low  = ADCL;
        high = ADCH;
#else
        // we dont have an ADC, return 0
        low  = 0;
        high = 0;
#endif

        // combine the two bytes
        return (high << 8) | low;
}

*/

#endif /* _BASE_H */
