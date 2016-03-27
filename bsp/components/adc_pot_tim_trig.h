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
// API Driver for ADC potentiometer.

// For sampling rate = 5 kHz, Timer clock can be 84MHz on TIMx APB1 with 
// ARR = 16800

#ifndef ADC_POT_TIM_TRIG_H_
#define ADC_POT_TIM_TRIG_H_

#include "libstm32f4.h"
#include "class_utils.h"

namespace adc_pot_tim_trig {

    class AdcPotTimTrig {
    public:
      AdcPotTimTrig (ADC_TypeDef * ADCx,
		     uint32_t adc_channel,
		     uint32_t adc_ext_trig,
		     uint32_t adc_sample_time,
		     DMA_TypeDef * DMAx,
		     DMA_Stream_TypeDef * dma_stream,
		     uint32_t dma_channel,
		     IRQn_Type dma_irqn,
		     GPIO_TypeDef * GPIOx,
		     uint16_t gpio_pin,
		     float32_t supply) {
	  this->ADCx = ADCx;
	  this->adc_channel = adc_channel;
	  this->adc_ext_trig = adc_ext_trig;
	  this->adc_sample_time = adc_sample_time;
	  this->DMAx = DMAx;
	  this->dma_stream = dma_stream;
	  this->dma_channel = dma_channel;
	  this->dma_irqn = dma_irqn;
	  this->GPIOx = GPIOx;
	  this->gpio_pin = gpio_pin;
	  this->supply = supply;
      }
      ~AdcPotTimTrig () { }

      void init (void);
      ADC_HandleTypeDef * getAdcHandle (void);
      float32_t getVoltage (void);
      uint32_t getAdcValue (void);

    private:
      __IO uint32_t value;
      float32_t supply;

      ADC_HandleTypeDef ADCx_Handle;
      ADC_ChannelConfTypeDef ADCx_ChannelConf;
      ADC_TypeDef * ADCx;
      uint32_t adc_channel;
      uint32_t adc_ext_trig;
      uint32_t adc_sample_time;

      DMA_HandleTypeDef DMAx_Handle;
      DMA_TypeDef * DMAx;
      DMA_Stream_TypeDef * dma_stream;
      uint32_t dma_channel;
      IRQn_Type dma_irqn;

      GPIO_InitTypeDef  GPIO_InitStruct;
      GPIO_TypeDef * GPIOx;
      uint16_t gpio_pin;

      DISALLOW_COPY_AND_ASSIGN (AdcPotTimTrig);
    };

} // namespace adc_pot_tim_trig 

#endif // ADC_POT_TIM_TRIG_H_
