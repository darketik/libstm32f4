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
// API Driver for ADC temperature sensor of stm32f407.

// For sampling rate = 5 kHz, Timer clock can be 84MHz on TIM3 APB1 with 
// ARR = 16800

#ifndef ADC_POT_TIM_TRIG_H_
#define ADC_POT_TIM_TRIG_H_

#include "stm32f4xx_hal.h"
#include "class_utils.h"
#include "arm_math.h"
#include "system.h"

namespace adc_pot_tim_trig {

#define ADCx_DMA_STREAM		DMA2_Stream0
#define ADCx_DMA_CHANNEL	DMA_CHANNEL_0
#define ADCx_DMA_CLK_ENABLE 	__HAL_RCC_DMA2_CLK_ENABLE
#define ADCx_DMA_IRQn		DMA2_Stream0_IRQn
#define ADCx_DMA_IRQHandler	DMA2_Stream0_IRQHandler

#define ADCx_CHANNEL_GPIOx_CLK_ENABLE  	__HAL_RCC_GPIOC_CLK_ENABLE		
#define ADCx_GPIO_PIN			GPIO_PIN_1 			
#define ADCx_GPIO_PORT			GPIOC		
#define ADCx_CHANNEL			ADC_CHANNEL_11

#define ADCx 			ADC1
#define ADCx_CLK_ENABLE 	__HAL_RCC_ADC1_CLK_ENABLE
#define ADCx_FORCE_RESET 	__HAL_RCC_ADC_FORCE_RESET
#define ADCx_RELEASE_RESET 	__HAL_RCC_ADC_RELEASE_RESET
#define ADCx_IRQHandler	 	ADC_IRQHandler

    class AdcPotTimTrig {
    public:
      AdcPotTimTrig (float32_t supply
		     //+ GPIO_TypeDef *gpio_bank,
		     //+ uint16_t pin,
		     //+ uint16_t adc_channel
		     ) {
	  this->supply = supply;
	  //+ this->gpio_bank = gpio_bank;
	  //+ this->gpio_pin = pin;
	  //+ this->adc_channel = adc_channel;
      }
      ~AdcPotTimTrig () { }

      void init (void);
      ADC_HandleTypeDef * getAdcHandle(void);
      float32_t getVoltage(void);
      uint32_t getAdcValue(void);

    private:
      __IO uint32_t value;
      float32_t supply;
      //+ GPIO_TypeDef *gpio_bank,
      //+ uint16_t pin,
      //+ uint16_t adc_channel;

      ADC_HandleTypeDef ADCx_Handle;
      ADC_ChannelConfTypeDef ADCx_ChannelConf;

      DISALLOW_COPY_AND_ASSIGN (AdcPotTimTrig);
    };

} // namespace adc_pot_tim_trig 

#endif // ADC_POT_TIM_TRIG_H_
