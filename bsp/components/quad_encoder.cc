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
//

#include "quad_encoder.h"

namespace quad_encoder {

    void QuadEncoder::init (void) {
	uint32_t uPrescaler = 0;

	TIMx_CLK_ENABLE (this->TIMx);
	GPIOx_CLK_ENABLE (this->GPIOx);
	DMAx_CLK_ENABLE (this->DMAx);

	GPIO_InitStruct.Pin = this->ti1 | this->ti2;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	GPIO_InitStruct.Alternate = this->af;
	HAL_GPIO_Init (this->GPIOx, &this->GPIO_InitStruct);

	DMAx_Handle.Init.Direction = DMA_PERIPH_TO_MEMORY;
	DMAx_Handle.Init.PeriphInc = DMA_PINC_DISABLE;
	DMAx_Handle.Init.MemInc = DMA_MINC_ENABLE;
	DMAx_Handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
	DMAx_Handle.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
	DMAx_Handle.Init.Mode = DMA_CIRCULAR;
	DMAx_Handle.Init.Priority = DMA_PRIORITY_HIGH;
	DMAx_Handle.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
	DMAx_Handle.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_HALFFULL;
	DMAx_Handle.Init.MemBurst = DMA_MBURST_SINGLE;
	DMAx_Handle.Init.PeriphBurst = DMA_PBURST_SINGLE;
	DMAx_Handle.Instance = this->dma_stream;
	DMAx_Handle.Init.Channel = this->dma_channel;
	if (HAL_DMA_Init (&this->DMAx_Handle) != HAL_OK) {
	    Error_Handler ();
	}
	// link DMA handle to TIMx Handle
	__HAL_LINKDMA (&this->TIMx_Handle, hdma[TIM_DMA_ID_CC1], this->DMAx_Handle);

	// NVIC for DMA transfer complete IT
	HAL_NVIC_SetPriority (this->dma_irqn, 0, 0);
	HAL_NVIC_EnableIRQ (this->dma_irqn);

	// To find prescaler value:
	// TIMx is on APB1 and APB1 clk_div = 4 
	// so TIMx CLK = 2 * APB1_CLK  (because apb1 clk_div not equal to 1)
	// or APB1_CLK = HCLK / 4 (42MHz)
	// so TIMx_CLK = HCLK / 2 = SystemCoreClock / 2 (84MHz)
	// Prescaler = (TIMx_CLK / TIMx counter clock) - 1
	//
	// Tick frequency:
	// Freq = TIMx_CLK / ARR(TIMx_period)
	SystemCoreClock = HAL_RCC_GetHCLKFreq ();
	uPrescaler = (uint32_t) ((SystemCoreClock / 2) / this->clock) - 1;

	TIMx_Handle.Instance = this->TIMx;
	TIMx_Handle.Init.Prescaler = uPrescaler;
	TIMx_Handle.Init.CounterMode = TIM_COUNTERMODE_UP;
	TIMx_Handle.Init.Period = this->period - 1; 
	TIMx_Handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;

	TIM_Encoder_InitStruct.EncoderMode = TIM_ENCODERMODE_TI1;
	TIM_Encoder_InitStruct.IC1Polarity = TIM_ICPOLARITY_RISING;
	TIM_Encoder_InitStruct.IC1Selection = TIM_ICSELECTION_DIRECTTI;
	TIM_Encoder_InitStruct.IC1Prescaler = TIM_ICPSC_DIV1;
	TIM_Encoder_InitStruct.IC1Filter = 0x0;
	TIM_Encoder_InitStruct.IC2Polarity = TIM_ICPOLARITY_RISING;
	TIM_Encoder_InitStruct.IC2Selection = TIM_ICSELECTION_DIRECTTI;
	TIM_Encoder_InitStruct.IC2Prescaler = TIM_ICPSC_DIV1;
	TIM_Encoder_InitStruct.IC2Filter = 0x0;

	if (HAL_TIM_Encoder_Init (&this->TIMx_Handle, &this->TIM_Encoder_InitStruct) != HAL_OK) {
	    Error_Handler ();
	}

	__HAL_TIM_SET_COUNTER (&this->TIMx_Handle, 0x8000);

	if (HAL_TIM_Encoder_Start_DMA (&TIMx_Handle, TIM_CHANNEL_1, (uint32_t*)&ic1, (uint32_t*)&ic1, 1) != HAL_OK) {
	    Error_Handler ();
	}
    }


   TIM_HandleTypeDef * QuadEncoder::getTimHandle (void) {
	return &this->TIMx_Handle;
    }


   uint16_t QuadEncoder::getCounter (void) {
	return __HAL_TIM_GET_COUNTER (&this->TIMx_Handle);
    }


    uint32_t QuadEncoder::getPosition (void) {
	uint32_t _temp;

	//+ if (this->TIMx_Handle.Channel ==  HAL_TIM_ACTIVE_CHANNEL_1)
	_temp = this->ic1;
	//+ else 
	//+ _temp = this->ic2;

	if (_temp > this->old_counter) {
	    if (this->encoder_value != 127) {
		this->encoder_value++;
		this->old_counter = _temp;
	    }
	    else {
		this->old_counter = 0x8000;
		__HAL_TIM_SET_COUNTER (&this->TIMx_Handle, 0x8000);
	    }
	}
	else {
	    if (_temp < this->old_counter) {
		if (this->encoder_value != 0) {
		    this->encoder_value--;
		    this->old_counter = _temp;
		}
		else {
		    this->old_counter = 0x8000;
		    __HAL_TIM_SET_COUNTER (&this->TIMx_Handle, 0x8000);
		}
	    }
	}

	return this->encoder_value;
    }

} // namespace quad_encoder 



#ifdef __cplusplus
extern "C" {
#endif 

    //+ void HAL_TIM_Encoder_MspInit (ADC_HandleTypeDef* hadc) {
    //+     
    //+ }

#ifdef __cplusplus
}
#endif
