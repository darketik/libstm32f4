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
// API Driver for ADC on temperature sensor of stm32f407.
//

#include "adc_temp.h"

namespace adc_temp {

    void AdcTemp::init (void) {
	// config ADCx 
	ADCx_Handle.Instance = ADCx;
	ADCx_Handle.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
	ADCx_Handle.Init.Resolution = ADC_RESOLUTION_12B;
	ADCx_Handle.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	ADCx_Handle.Init.ScanConvMode = DISABLE;
	ADCx_Handle.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
	ADCx_Handle.Init.ContinuousConvMode = ENABLE;
	ADCx_Handle.Init.NbrOfConversion = 1;
	ADCx_Handle.Init.DiscontinuousConvMode = DISABLE;
	ADCx_Handle.Init.NbrOfDiscConversion = 0;
	ADCx_Handle.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	//+ ADCx_Handle.Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_T1_CC1;
	ADCx_Handle.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
	ADCx_Handle.Init.DMAContinuousRequests = ENABLE;

	// initialize ADCx 
	if (HAL_ADC_Init (&ADCx_Handle) != HAL_OK) {
	    Error_Handler ();
	}

	// configure ADCx regular channel
	ADCx_ChannelConf.Channel = ADC_CHANNEL_TEMPSENSOR;
	ADCx_ChannelConf.Rank = 1;
	// APB2_CLK = HCLK/2 (84 MHz)
	// ADC_CLK = APB2_CLK/4 = 21MHz
	// Total Time = (ADC_RES + Sampling time) / ADC_CLK
	// 	      = 6µs
	// 
	// carefull with internal measurement channels
	// sampling time constraints must be respected 
	// (sampling time can be adjusted in function of 
	// ADC clock frequency and sampling time setting)
	ADCx_ChannelConf.SamplingTime = ADC_SAMPLETIME_112CYCLES; 
	ADCx_ChannelConf.Offset = 0;

	// initialiaze ADCx Channel
	if (HAL_ADC_ConfigChannel (&ADCx_Handle, &ADCx_ChannelConf) != HAL_OK) {
	    Error_Handler ();
	}

	// Start ADCx with DMAx
	if (HAL_ADC_Start_DMA (&ADCx_Handle, (uint32_t*)&(this->temp) , 1) != HAL_OK) {
	    Error_Handler ();
	}
    }



    ADC_HandleTypeDef * AdcTemp::getAdcHandle (void) {
	return &(this->ADCx_Handle);
    }



    float32_t AdcTemp::getTemp(void) {
	// temp(C) = (Vsense - V25) / Avg_slope + 25
	// Vsense is in mV
	// Avg_slope = 25mV/C (they say 2.5mV/C in datasheet, i think its wrong)
	// V25 = 0.76V 
	float32_t temp_c;
	float32_t vsense;
	uint32_t temp_adc_i;

	const static float32_t avg_slope = 25.0f; //2.5mV/C seems wrong
	const static float32_t v_25 = 760.0f;
	const static float32_t v_powersupply = 3000.0f;
	//+ const static float adc_max_value_12b = 0xfff;
	const static float32_t adc_max_value_12b = 4096.0f;
	const static float32_t v_per_bit = v_powersupply / adc_max_value_12b;
	const static float32_t temp_offset = 25.0f;

	temp_adc_i = this->temp;
	// transform adc measured 12b int value into volt
	vsense = (float)temp_adc_i * v_per_bit;
	// apply temperature in C formula
	temp_c = ((vsense - v_25) / avg_slope) + temp_offset;

	return temp_c;
    }



    uint32_t AdcTemp::getAdcValue(void) {
	return (this->temp);
    }

} // namespace adc_temp 



#ifdef __cplusplus
extern "C" {
#endif 

    void HAL_ADC_MspInit (ADC_HandleTypeDef* hadc) {
	static DMA_HandleTypeDef DMAx_Handle;

	// init ADCx clock
	ADCx_CLK_ENABLE ();

	// init DMAx CLK, 2 streams, link DMA HAndle, set priority and enable transfer
	// complete IT on the 2 streams. 
	// config DMAx 
	ADCx_DMA_CLK_ENABLE ();

	DMAx_Handle.Instance = ADCx_DMA_STREAM;
	DMAx_Handle.Init.Channel = ADCx_DMA_CHANNEL;
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

	// initialize DMAx 
	if (HAL_DMA_Init (&DMAx_Handle) != HAL_OK) {
	    Error_Handler ();
	}

	// link DMA handle to ADC Handle
	__HAL_LINKDMA (hadc, DMA_Handle, DMAx_Handle);

	// NVIC for DMA transvfer complete IT
	HAL_NVIC_SetPriority (ADCx_DMA_IRQn, 3, 0);
	HAL_NVIC_EnableIRQ (ADCx_DMA_IRQn);
    }




    void HAL_ADC_MspDeInit (ADC_HandleTypeDef* hadc) {
	ADCx_FORCE_RESET ();
	ADCx_RELEASE_RESET ();
    }

#ifdef __cplusplus
}
#endif
