// Copyright 2015 Florent Rotenberg.
//
// Author: Florent Rotenberg (florent.rotenberg@gmail.com)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
// 
// See http://creativecommons.org/licenses/MIT/ for more information.
//
// -----------------------------------------------------------------------------
//
// API Driver for RGB LED.
// RED - PC1
// GREEN - PC2
// BLUS - PC4


#ifndef LED_RGB_H_
#define LED_RGB_H_

#include "libstm32f4.h"
#include "class_utils.h"

namespace led_rgb {

    class LedRgb {
    public:
      LedRgb (GPIO_TypeDef *GpioBank,
	      uint16_t RedPin,
	      uint16_t GreenPin,
	      uint16_t BluePin) {
	  this->GpioBank = GpioBank;
	  this->RedPin = RedPin;
	  this->GreenPin = GreenPin;
	  this->BluePin = BluePin;
      }
      ~LedRgb () {}

      void init (void);
      void SetRed (uint8_t val);
      void SetGreen (uint8_t val);
      void SetBlue (uint8_t val);
      void On (void);
      void Off (void);

    private:
      uint8_t red;
      uint8_t green;
      uint8_t blue;

      GPIO_TypeDef *GpioBank;
      uint16_t RedPin;
      uint16_t GreenPin;
      uint16_t BluePin;

      DISALLOW_COPY_AND_ASSIGN (LedRgb);
    };

} // namespace led_rgb 

#endif // LED_RGB_H_
