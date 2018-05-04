//////////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Fabio Di Bernardini and www.fablaquila.org
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

//////////////////////////////////////////////////////////////////////
// SimpleMakey
// The simplest DIY Makey-Makey like device
//
// This sketch was tested on most ATmega 32U4 based Arduino devices.
// This pin mapping is useful: https://www.arduino.cc/en/Hacking/PinMapping32u4
//
//                                                                                  DIYMORE      SPARKFUN   ARDUINO
// PIN FUNCTION                   PIN                       PIN IDE NAME  SS MICRO  DM MICRO-AU  PRO MICRO  LEONARDO
// ------------------------------|-------------------------|-------------|---------|------------|----------|--------|           
// (PCINT1/SCLK) PB1              SCK                       SCK           X         X            X          X
// (PDI/PCINT2/MOSI) PB2          MOSI                      MOSI          X         X            X          X
// (PDO/PCINT3/MISO) PB3          MISO                      MISO          X         X            X          X
// (OC0B/SCL/INT0) PD0            Digital pin 3 (SCL)(PWM)  3             X         X            X          X
// (SDA/INT1) PD1                 Digital pin 2 (SDA)       2             X         X            X          X
// (RX D1/AIN1/INT2) PD2          Digital pin 0 (RX)        0             X         X            X          X
// (TXD1/INT3) PD3                Digital pin 1 (TX)        1             X         X            X          X
// (PCINT5/OC1A/#OC4B/ADC12) PB5  Digital Pin 9 (PWM)       9             X         X            X          X
// (PCINT6/OC1B/OC4B/ADC13) PB6   Digital Pin 10 (PWM)      10            X         X            X          X
// (ADC7/TDI) PF7                 Analog In 0               A0            X         X            X          X
// (ADC6/TDO) PF6                 Analog In 1               A1            X         X            X          X
// (ADC5/TMS) PF5                 Analog In 2               A2            X         X            X          X
// (PCINT7/OC0A/OC1C/#RTS) PB7    Digital pin 11 (PWM)      11            X         X                       X
// (T0/OC4D/ADC10) PD7            Digital Pin 6 (PWM)       6             X                      X          X
// PE6 (INT.6/AIN0)               Digital pin 7             7                                    X          X
// (ICP1/ADC8) PD4                Digital pin 4             4                                    X          X
// (ADC11/PCINT4) PB4             Digital pin 8             8                                    X          X
// (OC3A/#0C4A) PC6               Digital Pin 5 (PWM)       5                                    X          X
// (ADC4/TCK) PF4                 Analog In 3               A3                                   X          X
// (T1/#OC4D/ADC9) PD6            Digital pin 12            12                                              X
// (ICP3/CLK0/)0C4A) PC7          Digital Pin 13 (PWM)      13                                              X
// (ADC1) PF1                     Analog In 4               A4                                              X
// (ADC0) PF0                     Analog In 5               A5                                              X

//////////////////////////////////////////////////////////////////////
// CONFIGURATION

// The thresold value that determines the touch.
// If unwanted key will appear, then decrease sensitivity,
// otherwise if no key appear then increase sensitivity.
// THRESHOLD: Number between 0 (less sensitive) to 1 (more sensitive).
#define THRESHOLD .5

// Instead of use an external 22MOhm pullup resistor as Makey-Makey, we use internal pullup
// resistor + pin capacitance driven by a software generated pwm swtching signal.
// CHARGE_TIME: Value between 0 to 255. The time the pullup resistor charge the capacitor.
#define CHARGE_TIME 0 // 0 more sensitive - 255 less sensitive

#include "Keyboard.h"

class TouchKeyboard {

  public:
  byte _pin;
  byte _key;
  bool _stateTouched = false;
  byte threshold;
  byte value;

  TouchKeyboard(byte pin, char key) {
    _pin = pin;
    _key = key;
    threshold = this->dischargeTime() * THRESHOLD;
    
  };

// Here we don't need optimization due to empty for loops used as precise delay.
#pragma GCC push_options
#pragma GCC optimize ("O0")
  byte dischargeTime() {
    pinMode(_pin, INPUT_PULLUP);
    for (value = 0; value < CHARGE_TIME; value++) {};
    pinMode(_pin, INPUT);
    for (value = 0; value<255; value++) {
      if ( !digitalRead(_pin) ) break;
    };
    return value; 
  };
#pragma GCC pop_options

  bool isTouched() {
    return this->dischargeTime() < this->threshold;
  };

  void raiseEvents() {
    if ( _stateTouched ) {
      if ( !isTouched() ) {
        _stateTouched = false;
//        Serial.print(_pin);
//        Serial.print(" - ");
//        Serial.println("onRelease");
        Keyboard.release(_key);
      };
    } else {
      if ( isTouched() ) {
        _stateTouched = true;
//        Serial.print(_pin);
//        Serial.print(" - ");
//        Serial.println("onTouch");
        Keyboard.press(_key);
      };
    };
  };
};

TouchKeyboard *tbSCK;
TouchKeyboard *tbMOSI;
TouchKeyboard *tbMISO;
TouchKeyboard *tb3;
TouchKeyboard *tb2;
TouchKeyboard *tb0;
TouchKeyboard *tb1;
TouchKeyboard *tb9;
TouchKeyboard *tb10;
TouchKeyboard *tbA0;
TouchKeyboard *tbA1;
TouchKeyboard *tbA2;
TouchKeyboard *tb11;
TouchKeyboard *tb6;
TouchKeyboard *tb7;
TouchKeyboard *tb4;
TouchKeyboard *tb8;
TouchKeyboard *tb5;
TouchKeyboard *tbA3;
TouchKeyboard *tb12;
TouchKeyboard *tb13;
TouchKeyboard *tbA4;
TouchKeyboard *tbA5;

void setup() {
  Keyboard.begin();
  //Serial.begin(2000000);
  tbSCK = new TouchKeyboard(SCK, KEY_UP_ARROW);
  tbMOSI = new TouchKeyboard(MOSI, KEY_DOWN_ARROW);
  tbMISO = new TouchKeyboard(MISO, KEY_LEFT_ARROW);
  tb3 = new TouchKeyboard(3, KEY_RIGHT_ARROW);
  tb2 = new TouchKeyboard(2, ' ');
  tb0 = new TouchKeyboard(0, 'W');
  tb1 = new TouchKeyboard(1, 'A');
  tb9 = new TouchKeyboard(9, 'S');
  tb10 = new TouchKeyboard(10, 'D');
  tbA0 = new TouchKeyboard(A0, '0');
  tbA1 = new TouchKeyboard(A1, '1');
  tbA2 = new TouchKeyboard(A2, '2');
  tb11 = new TouchKeyboard(11, '3');
  tb6 = new TouchKeyboard(6, '4');
  tb7 = new TouchKeyboard(7, '5');
  tb4 = new TouchKeyboard(4, '6');
  tb8 = new TouchKeyboard(8, '7');
  tb5 = new TouchKeyboard(5, '8');
  tbA3 = new TouchKeyboard(A3, '9');
  tb12 = new TouchKeyboard(12, 'I');
  tb13 = new TouchKeyboard(13, 'J');
  tbA4 = new TouchKeyboard(A4, 'K');
  tbA5 = new TouchKeyboard(A5, 'L');
}

void loop() {
  tbSCK->raiseEvents();
  tbMOSI->raiseEvents();
  tbMISO->raiseEvents();
  tb3->raiseEvents();
  tb2->raiseEvents();
  tb0->raiseEvents();
  tb1->raiseEvents();
  tb9->raiseEvents();
  tb10->raiseEvents();
  tbA0->raiseEvents();
  tbA1->raiseEvents();
  tbA2->raiseEvents();
  tb11->raiseEvents();
  tb6->raiseEvents();
  tb7->raiseEvents();
  tb4->raiseEvents();
  tb8->raiseEvents();
  tb5->raiseEvents();
  tbA3->raiseEvents();
  tb12->raiseEvents();
  tb13->raiseEvents();
  tbA4->raiseEvents();
  tbA5->raiseEvents();
}
