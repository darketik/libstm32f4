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
// API Driver for Base Timer of stm32f407.
//

#include "tim_base.h"

namespace tim_base {

    void TimBase::init (void) {
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
	uPrescaler = (uint32_t) ((SystemCoreClock / 2) / TIMx_clock) - 1;

	// configure timer
	TIMx_Handle.Instance = TIMx;
	TIMx_Handle.Init.Prescaler = uPrescaler;
	TIMx_Handle.Init.CounterMode = TIM_COUNTERMODE_UP;
	TIMx_Handle.Init.Period = TIMx_period - 1; 
	TIMx_Handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;

	TIMx_CLK_ENABLE ();

	// NVIC for TIMx IT
	HAL_NVIC_SetPriority (TIMx_IRQn, 3, 1);
	HAL_NVIC_EnableIRQ (TIMx_IRQn);

	// below function uses user definition of HAL_TIM_Base_MspInit
	if (HAL_TIM_Base_Init (&TIMx_Handle) != HAL_OK) {
	    Error_Handler ();
	}

	TIMx_MasterConfig_InitStruct.MasterOutputTrigger = TIM_TRGO_UPDATE;
	TIMx_MasterConfig_InitStruct.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	HAL_TIMEx_MasterConfigSynchronization (&TIMx_Handle, &TIMx_MasterConfig_InitStruct);

	if (HAL_TIM_Base_Start_IT(&TIMx_Handle) != HAL_OK) {
	    Error_Handler ();
	}
    }


    TIM_HandleTypeDef * TimBase::getHandle (void) {
	return &(this->TIMx_Handle);
    }


} // namespace tim_base 



#ifdef __cplusplus
extern "C" {
#endif 

    //+ void HAL_TIM_Base_MspInit (TIM_HandleTypeDef *htim)	
    //+   {
    //+   
    //+   }

#ifdef __cplusplus
}
#endif
