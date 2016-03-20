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

#ifndef TIM_BASE_H_
#define TIM_BASE_H_

#include "stm32f4xx_hal.h"
#include "class_utils.h"
#include "system.h"

namespace tim_base {

#define TIMx_CLK_ENABLE	__HAL_RCC_TIM3_CLK_ENABLE
#define TIMx_IRQHandler	TIM3_IRQHandler
#define TIMx_IRQn	TIM3_IRQn

    class TimBase {
    public:
      TimBase(TIM_TypeDef * TIMx,
	      uint32_t TIMx_clock,
	      uint32_t TIMx_period) {
	  this->TIMx = TIMx;
	  this->TIMx_clock = TIMx_clock;
	  this->TIMx_period = TIMx_period;
      }
      ~TimBase() { }

      void init (void);
      TIM_HandleTypeDef * getHandle (void);

    private:
      TIM_TypeDef * TIMx;
      uint32_t TIMx_clock; 
      uint32_t TIMx_period; 
      TIM_HandleTypeDef TIMx_Handle;
      TIM_MasterConfigTypeDef TIMx_MasterConfig_InitStruct;

      DISALLOW_COPY_AND_ASSIGN (TimBase);
    };

} // namespace tim_base 

#endif // TIM_BASE_H_
