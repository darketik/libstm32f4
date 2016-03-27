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
	uPrescaler = (uint32_t) ((SystemCoreClock / 2) / clock) - 1;

	TIMx_CLK_ENABLE (this->TIMx);
	GPIOx_CLK_ENABLE (this->GPIOx);

	GPIO_InitStruct.Pin = ti1 | ti2;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.Alternate = af;

	HAL_GPIO_Init (GPIOx, &GPIO_InitStruct);

	TIMx_Handle.Instance = TIMx;
	TIMx_Handle.Init.Prescaler = uPrescaler;
	TIMx_Handle.Init.CounterMode = TIM_COUNTERMODE_UP;
	TIMx_Handle.Init.Period = period - 1; 
	TIMx_Handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;

	TIMx_Handle.Instance = TIMx;
	TIM_Encoder_InitStruct.EncoderMode = TIM_ENCODERMODE_TI12;
	TIM_Encoder_InitStruct.IC1Polarity = TIM_ICPOLARITY_BOTHEDGE;
	TIM_Encoder_InitStruct.IC1Selection = TIM_ICSELECTION_DIRECTTI;
	TIM_Encoder_InitStruct.IC1Prescaler = TIM_ICPSC_DIV1;
	TIM_Encoder_InitStruct.IC1Filter = 0xf;
	TIM_Encoder_InitStruct.IC2Polarity = TIM_ICPOLARITY_BOTHEDGE;
	TIM_Encoder_InitStruct.IC2Selection = TIM_ICSELECTION_DIRECTTI;
	TIM_Encoder_InitStruct.IC2Prescaler = TIM_ICPSC_DIV1;
	TIM_Encoder_InitStruct.IC2Filter = 0xf;

	if (HAL_TIM_Encoder_Init (&TIMx_Handle, &TIM_Encoder_InitStruct) != HAL_OK) {
	    Error_Handler ();
	}

	if (HAL_TIM_Encoder_Start(&TIMx_Handle, TIM_CHANNEL_ALL) != HAL_OK) {
	    Error_Handler ();
	}
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
