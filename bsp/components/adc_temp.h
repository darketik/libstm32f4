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

#ifndef ADC_TEMP_H_
#define ADC_TEMP_H_

#include "libstm32f4.h"
#include "class_utils.h"

namespace adc_temp {

    class AdcTemp {
    public:
      AdcTemp (ADC_TypeDef * ADCx,
		     uint32_t adc_channel,
		     uint32_t adc_sample_time,
		     DMA_TypeDef * DMAx,
		     DMA_Stream_TypeDef * dma_stream,
		     uint32_t dma_channel,
		     IRQn_Type dma_irqn) {
	  this->ADCx = ADCx;
	  this->adc_channel = adc_channel;
	  this->adc_sample_time = adc_sample_time;
	  this->DMAx = DMAx;
	  this->dma_stream = dma_stream;
	  this->dma_channel = dma_channel;
	  this->dma_irqn = dma_irqn;
      }
      ~AdcTemp () { }

      void init (void);
      ADC_HandleTypeDef * getAdcHandle (void);
      float32_t getTemp (void);
      uint32_t getAdcValue (void);

    private:
      __IO uint32_t temp;

      ADC_HandleTypeDef ADCx_Handle;
      ADC_ChannelConfTypeDef ADCx_ChannelConf;
      ADC_TypeDef * ADCx;
      uint32_t adc_channel;
      uint32_t adc_sample_time;

      DMA_HandleTypeDef DMAx_Handle;
      DMA_TypeDef * DMAx;
      DMA_Stream_TypeDef * dma_stream;
      uint32_t dma_channel;
      IRQn_Type dma_irqn;

      DISALLOW_COPY_AND_ASSIGN (AdcTemp);
    };

} // namespace adc_temp 

#endif // ADC_TEMP_H_
