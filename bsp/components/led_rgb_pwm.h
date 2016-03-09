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
// API Driver for RGB LED driven in PWM mode.

//TODO:
// - transform the class to be able to drive multiple rgb led simultenaously
// - provide generic value for PWM period and PWM max duty cycle value
// - provide a function to ramp color intensity with a logarithm speed

#ifndef LED_RGB_H_
#define LED_RGB_H_

#include "stm32f4xx_hal.h"
#include "class_utils.h"
#include "system.h"

namespace led_rgb_pwm {

// FIXME: remove this defines by a case depending on tiumer and gpio
#define TIMx_CLK_ENABLE __HAL_RCC_TIM3_CLK_ENABLE
#define TIMx_PWM_DEFAULT_DUTY_CYCLE 255 // CCR value for a TIMER channel - 50% 
#define LED_RGB_GPIOx_CLK_ENABLE __HAL_RCC_GPIOC_CLK_ENABLE

// TODO: define a enum in place of defines 
#define RED 0
#define GREEN 1
#define BLUE 2

class LedRgbPwm {
	public:
		LedRgbPwm (GPIO_TypeDef * GPIOx,
						uint16_t red_gpio,
						uint16_t green_gpio,
						uint16_t blue_gpio,
						TIM_TypeDef * TIMx,
						uint8_t TIMx_af,
						uint32_t TIMx_clock,
						uint32_t pwm_period, // TIMER ARR - PWM freq ~= 39kHz (256)
						uint32_t red_dc,
						uint32_t green_dc,
						uint32_t blue_dc) { 
				this->GPIOx = GPIOx;
				this->red_gpio = red_gpio;
				this->green_gpio = green_gpio;
				this->blue_gpio = blue_gpio;
				this->TIMx = TIMx;
				this->TIMx_af = TIMx_af;
				this->TIMx_clock = TIMx_clock;
				this->pwm_period = pwm_period;
				this->red_pwm_duty_cycle = red_dc;
				this->green_pwm_duty_cycle = green_dc;
				this->blue_pwm_duty_cycle = blue_dc;
		}
		//+ LedRgbPwm () { }
		~LedRgbPwm () { }

		void init (void);
		void setColorDutyCycle (uint32_t color, uint8_t val);
		void on (uint32_t color);
		void off (uint32_t color);
		
	private:
		GPIO_TypeDef * GPIOx;
		uint16_t red_gpio;
		uint16_t green_gpio;
		uint16_t blue_gpio;

		TIM_TypeDef * TIMx;
		uint8_t TIMx_af;
		uint32_t TIMx_clock;
		uint32_t pwm_period;
		uint32_t red_pwm_duty_cycle;
		uint32_t green_pwm_duty_cycle;
		uint32_t blue_pwm_duty_cycle;

		TIM_HandleTypeDef TIMx_Handle;
		TIM_OC_InitTypeDef TIMx_OC_InitStruct;

		DISALLOW_COPY_AND_ASSIGN (LedRgbPwm);
};

} // namespace led_rgb 

#endif // LED_RGB_H_
