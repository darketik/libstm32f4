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
//

#include "led_rgb_pwm.h"

namespace led_rgb_pwm {

    void LedRgbPwm::init (void) {
	uint32_t uPrescaler = 0;

	// PWM config 
	// - freq is determined by ARR
	// - duty cycle is determined by CCRx
	//
	// To find prescaler value:
	// TIMx is on APB1 and APB1 clk_div = 4 
	// so TIMx CLK = 2 * APB1_CLK  (because apb1 clk_div not equal to 1)
	// or APB1_CLK = HCLK / 4
	// so TIMx_CLK = HCLK / 2 = SystemCoreClock / 2
	// Prescaler = (TIMx_CLK / TIMx counter clock) - 1
	//
	// PWM freq computation
	// PWM_Freq = TIMx_CLK / ARR
	SystemCoreClock = HAL_RCC_GetHCLKFreq ();
	uPrescaler = (uint32_t) ((SystemCoreClock / 2) / TIMx_clock) - 1;

	// configure timer
	TIMx_Handle.Instance = TIMx;
	TIMx_Handle.Init.Prescaler = uPrescaler;
	TIMx_Handle.Init.CounterMode = TIM_COUNTERMODE_UP;
	TIMx_Handle.Init.Period = pwm_period - 1; 
	TIMx_Handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;

	TIMx_CLK_ENABLE (TIMx);
	GPIOx_CLK_ENABLE (GPIOx);

	GPIO_InitTypeDef  GPIO_InitStruct;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
	GPIO_InitStruct.Alternate = TIMx_af;
	GPIO_InitStruct.Pin = red_gpio | green_gpio | blue_gpio;
	HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);

	// below function uses user definition of HAL_TIM_PWM_MspInit
	if (HAL_TIM_PWM_Init (&TIMx_Handle) != HAL_OK) {
	    Error_Handler ();
	}

	// configure PWM channels
	TIMx_OC_InitStruct.OCMode = TIM_OCMODE_PWM1;
	TIMx_OC_InitStruct.OCPolarity = TIM_OCPOLARITY_HIGH;
	TIMx_OC_InitStruct.OCFastMode = TIM_OCFAST_DISABLE;
	TIMx_OC_InitStruct.OCNPolarity = 0; // dummy to bypass assert_param check
	TIMx_OC_InitStruct.OCNIdleState = 0; // dummy to bypass assert_param check
	TIMx_OC_InitStruct.OCIdleState = 0; // dummy to bypass assert_param check
	TIMx_OC_InitStruct.Pulse = this->duty_cycle; 

	if (HAL_TIM_PWM_ConfigChannel (&TIMx_Handle, &TIMx_OC_InitStruct, TIM_CHANNEL_1) != HAL_OK)
	  {
	    Error_Handler ();
	  }
	if (HAL_TIM_PWM_ConfigChannel (&TIMx_Handle, &TIMx_OC_InitStruct, TIM_CHANNEL_3) != HAL_OK)
	  {
	    Error_Handler ();
	  }
	if (HAL_TIM_PWM_ConfigChannel (&TIMx_Handle, &TIMx_OC_InitStruct, TIM_CHANNEL_4) != HAL_OK)
	  {
	    Error_Handler ();
	  }

	// start PWM channels generation
	if (HAL_TIM_PWM_Start (&TIMx_Handle, TIM_CHANNEL_1) != HAL_OK)
	  {
	    Error_Handler ();
	  }
	if (HAL_TIM_PWM_Start (&TIMx_Handle, TIM_CHANNEL_3) != HAL_OK)
	  {
	    Error_Handler ();
	  }
	if (HAL_TIM_PWM_Start (&TIMx_Handle, TIM_CHANNEL_4) != HAL_OK)
	  {
	    Error_Handler ();
	  }
    }

    // TODO: change duty cycle at a precise end of PWM period
    void LedRgbPwm::setColorDutyCycle (uint32_t color, uint8_t val) {
	switch (color) 
	  {
	  case (RED):
	      {
		TIMx -> CCR1 = (uint32_t) val;
	      }
	    break;

	  case (GREEN):
	      {
		TIMx -> CCR3 = (uint32_t) val;
	      }
	    break;

	  case (BLUE):
	      {
		TIMx -> CCR4 = (uint32_t) val;
	      }
	    break;

	  default: 
	    break;
	  }
    }

    void LedRgbPwm::on (uint32_t color) {
	switch (color) 
	  {
	  case (RED):
	      {
		if (HAL_TIM_PWM_Start (&TIMx_Handle, TIM_CHANNEL_1) != HAL_OK)
		  {
		    Error_Handler ();
		  }
	      }
	    break;

	  case (GREEN):
	      {
		if (HAL_TIM_PWM_Start (&TIMx_Handle, TIM_CHANNEL_3) != HAL_OK)
		  {
		    Error_Handler ();
		  }
	      }
	    break;

	  case (BLUE):
	      {
		if (HAL_TIM_PWM_Start (&TIMx_Handle, TIM_CHANNEL_4) != HAL_OK)
		  {
		    Error_Handler ();
		  }
	      }
	    break;

	  default: 
	    break;
	  }
    }

    void LedRgbPwm::off (uint32_t color) {
	switch (color) 
	  {
	  case (RED):
	      {
		if (HAL_TIM_PWM_Stop (&TIMx_Handle, TIM_CHANNEL_1) != HAL_OK)
		  {
		    Error_Handler ();
		  }
	      }
	    break;

	  case (GREEN):
	      {
		if (HAL_TIM_PWM_Stop (&TIMx_Handle, TIM_CHANNEL_3) != HAL_OK)
		  {
		    Error_Handler ();
		  }
	      }
	    break;

	  case (BLUE):
	      {
		if (HAL_TIM_PWM_Stop (&TIMx_Handle, TIM_CHANNEL_4) != HAL_OK)
		  {
		    Error_Handler ();
		  }
	      }
	    break;

	  default: 
	    break;
	  }
    }


} // namespace led_rgb_pwm

#ifdef __cplusplus
 extern "C" {
#endif 

//+ void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim)
//+ {

//+ }

#ifdef __cplusplus
}
#endif
