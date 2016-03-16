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

//TODO:
// - check sampling time config in ADCx_ChannelConf face to TempSensor
// constraint in datasheet
// - Define a sampling rate to process tempsensor measure and configure it
// using a timer trigger for the ADC

#ifndef ADC_TEMP_H_
#define ADC_TEMP_H_

#include "stm32f4xx_hal.h"
#include "class_utils.h"
#include "arm_math.h"
#include "system.h"

namespace adc_temp {

#define ADCx_DMA_STREAM		DMA2_Stream0
#define ADCx_DMA_CHANNEL	DMA_CHANNEL_0
#define ADCx_DMA_CLK_ENABLE 	__HAL_RCC_DMA2_CLK_ENABLE
#define ADCx_DMA_IRQn		DMA2_Stream0_IRQn
#define ADCx_DMA_IRQHandler	DMA2_Stream0_IRQHandler

#define ADCx 			ADC1
#define ADCx_CLK_ENABLE 	__HAL_RCC_ADC1_CLK_ENABLE
#define ADCx_CHANNEL		ADC_CHANNEL_TEMPSENSOR
#define ADCx_FORCE_RESET 	__HAL_RCC_ADC_FORCE_RESET
#define ADCx_RELEASE_RESET 	__HAL_RCC_ADC_RELEASE_RESET
#define ADCx_IRQHandler	 	ADC_IRQHandler

    class AdcTemp {
    public:
      AdcTemp() { }
      ~AdcTemp() { }

      void init (void);
      ADC_HandleTypeDef * getAdcHandle(void);
      q15_t getTemp(void);
      uint16_t getAdcValue(void);

    private:
      __IO uint16_t temp;
      ADC_HandleTypeDef ADCx_Handle;
      ADC_ChannelConfTypeDef ADCx_ChannelConf;

      DISALLOW_COPY_AND_ASSIGN (AdcTemp);
    };

} // namespace adc_temp 

#endif // ADC_TEMP_H_
