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
//

#include "led_rgb.h"

namespace led_rgb {

#define GPIOx_CLK_ENABLE __HAL_RCC_GPIOC_CLK_ENABLE

void LedRgb::init (void) {
	GPIO_InitTypeDef  GPIO_InitStruct;
	
	/* Init GPIO pins used for LCD interface */
	/* TODO find a way to make it generic to any GPIO bank */
	GPIOx_CLK_ENABLE ();

	GPIO_InitStruct.Pin = RedPin | GreenPin | BluePin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
	
  HAL_GPIO_Init(GpioBank, &GPIO_InitStruct);

	/* Initialize state of the LCD pins */
	Off ();

	/* Wait 100ms */
	HAL_Delay (100);
}

void LedRgb::SetRed (uint8_t val) {
  HAL_GPIO_WritePin(GpioBank, RedPin, GPIO_PIN_RESET); 
}

void LedRgb::SetGreen (uint8_t val) {
  HAL_GPIO_WritePin(GpioBank, GreenPin, GPIO_PIN_RESET); 
}

void LedRgb::SetBlue (uint8_t val) {
  HAL_GPIO_WritePin(GpioBank, BluePin, GPIO_PIN_RESET); 
}

void LedRgb::On (void) {
  HAL_GPIO_WritePin(GpioBank, RedPin | GreenPin | BluePin, GPIO_PIN_RESET); 
}

void LedRgb::Off (void) {
  HAL_GPIO_WritePin(GpioBank, RedPin | GreenPin | BluePin, GPIO_PIN_SET); 
}


} // namespace led_rgb 
