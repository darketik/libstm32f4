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
// API Driver for quadrature encoder sensor with stm32f407.

#ifndef QUAD_ENCODER_H_
#define QUAD_ENCODER_H_

#include "libstm32f4.h"
#include "class_utils.h"

namespace quad_encoder {

    class QuadEncoder {
    public:
      QuadEncoder (GPIO_TypeDef * GPIOx,
		   uint16_t ti1,
		   uint16_t ti2,
		   uint8_t af,
		   DMA_TypeDef * DMAx,
		   DMA_Stream_TypeDef * dma_stream [2],
		   uint32_t dma_channel [2],
		   IRQn_Type dma_irqn [2],
		   TIM_TypeDef * TIMx,
		   uint32_t period,
		   uint32_t clock,
		   IRQn_Type tim_irqn
      ) {
	  this->GPIOx = GPIOx;
	  this->ti1 = ti1;
	  this->ti2 = ti2;
	  this->af = af;

	  this->DMAx = DMAx;
	  this->dma_stream[0] = dma_stream[0];
	  this->dma_stream[1] = dma_stream[1];
	  this->dma_channel[0] = dma_channel[0];
	  this->dma_channel[1] = dma_channel[1];
	  this->dma_irqn[0] = dma_irqn[0];
	  this->dma_irqn[1] = dma_irqn[1];

	  this->TIMx = TIMx;
	  this->period = period;
	  this->clock = clock;
	  this->tim_irqn = tim_irqn;
	  encoder_value = 64;
	  old_counter= 0x8000;
      }
      ~QuadEncoder () { }

      void init (void);
      TIM_HandleTypeDef * getTimHandle (void);
      uint16_t getCounter (void);
      uint32_t getPosition (void);

    private:
      __IO uint32_t ic1;
      __IO uint32_t ic2;
      uint16_t encoder_value;
      uint16_t old_counter;

      GPIO_InitTypeDef  GPIO_InitStruct;
      GPIO_TypeDef * GPIOx;
      uint16_t ti1;
      uint16_t ti2;
      uint8_t af;

      DMA_HandleTypeDef DMAx_Handle[2];
      DMA_TypeDef * DMAx;
      DMA_Stream_TypeDef * dma_stream [2];
      uint32_t dma_channel [2];
      IRQn_Type dma_irqn [2];

      TIM_HandleTypeDef TIMx_Handle;
      TIM_Encoder_InitTypeDef TIM_Encoder_InitStruct;
      TIM_TypeDef * TIMx;
      uint32_t period;
      uint32_t clock;
      IRQn_Type tim_irqn;

      DISALLOW_COPY_AND_ASSIGN (QuadEncoder);
    };

} // namespace quad_encoder 

#endif // QUAD_ENCODER_H_
