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

#ifndef LIBSTM32F4_H_
#define LIBSTM32F4_H_

#include "stm32f4xx_hal.h"
#include "stm32f4_discovery.h"
#include "arm_math.h"

#include "system.h"

using namespace std;
using namespace system_stm32f4;

#define ADCx_CLK_ENABLE(x) do { \
	  switch ((uint32_t)x) { \
	    case (uint32_t)ADC1: \
	      __HAL_RCC_ADC1_CLK_ENABLE (); \
	      break; \
	    case (uint32_t)ADC2: \
	      __HAL_RCC_ADC2_CLK_ENABLE (); \
	      break; \
	    case (uint32_t)ADC3: \
	      __HAL_RCC_ADC3_CLK_ENABLE (); \
	      break; \
	    default: \
	      break; \
	  } \
} while (0)

#define DMAx_CLK_ENABLE(x) do { \
	  switch ((uint32_t)x) { \
	    case (uint32_t)DMA1: \
	      __HAL_RCC_DMA1_CLK_ENABLE (); \
	      break; \
	    case (uint32_t)DMA2: \
	      __HAL_RCC_DMA2_CLK_ENABLE (); \
	      break; \
	    default: \
	      break; \
	  } \
} while (0)

#define TIMx_CLK_ENABLE(x) do { \
	  switch ((uint32_t)x) { \
	    case (uint32_t)TIM1: \
	      __HAL_RCC_TIM1_CLK_ENABLE (); \
	      break; \
	    case (uint32_t)TIM2: \
	      __HAL_RCC_TIM2_CLK_ENABLE (); \
	      break; \
	    case (uint32_t)TIM3: \
	      __HAL_RCC_TIM3_CLK_ENABLE (); \
	      break; \
	    case (uint32_t)TIM4: \
	      __HAL_RCC_TIM4_CLK_ENABLE (); \
	      break; \
	    case (uint32_t)TIM5: \
	      __HAL_RCC_TIM5_CLK_ENABLE (); \
	      break; \
	    case (uint32_t)TIM6: \
	      __HAL_RCC_TIM6_CLK_ENABLE (); \
	      break; \
	    case (uint32_t)TIM7: \
	      __HAL_RCC_TIM7_CLK_ENABLE (); \
	      break; \
	    case (uint32_t)TIM8: \
	      __HAL_RCC_TIM8_CLK_ENABLE (); \
	      break; \
	    case (uint32_t)TIM9: \
	      __HAL_RCC_TIM9_CLK_ENABLE (); \
	      break; \
	    case (uint32_t)TIM10: \
	      __HAL_RCC_TIM10_CLK_ENABLE (); \
	      break; \
	    case (uint32_t)TIM11: \
	      __HAL_RCC_TIM11_CLK_ENABLE (); \
	      break; \
	    case (uint32_t)TIM12: \
	      __HAL_RCC_TIM12_CLK_ENABLE (); \
	      break; \
	    case (uint32_t)TIM13: \
	      __HAL_RCC_TIM13_CLK_ENABLE (); \
	      break; \
	    case (uint32_t)TIM14: \
	      __HAL_RCC_TIM14_CLK_ENABLE (); \
	      break; \
	    default: \
	      break; \
	  }  \
} while (0)
    
#define GPIOx_CLK_ENABLE(x) do { \
	  switch ((uint32_t)x) { \
	    case (uint32_t)GPIOA: \
	      __HAL_RCC_GPIOA_CLK_ENABLE (); \
	      break; \
	    case (uint32_t)GPIOB: \
	      __HAL_RCC_GPIOB_CLK_ENABLE (); \
	      break; \
	    case (uint32_t)GPIOC: \
	      __HAL_RCC_GPIOC_CLK_ENABLE (); \
	      break; \
	    case (uint32_t)GPIOD: \
	      __HAL_RCC_GPIOD_CLK_ENABLE (); \
	      break; \
	    case (uint32_t)GPIOE: \
	      __HAL_RCC_GPIOE_CLK_ENABLE (); \
	      break; \
	    case (uint32_t)GPIOF: \
	      __HAL_RCC_GPIOF_CLK_ENABLE (); \
	      break; \
	    case (uint32_t)GPIOG: \
	      __HAL_RCC_GPIOG_CLK_ENABLE (); \
	      break; \
	    case (uint32_t)GPIOH: \
	      __HAL_RCC_GPIOH_CLK_ENABLE (); \
	      break; \
	    case (uint32_t)GPIOI: \
	      __HAL_RCC_GPIOI_CLK_ENABLE (); \
	      break; \
	    default: \
	      break; \
	  } \
} while (0)

#endif // LIBSTM32F4_H_
