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
//

#include "adc_pot_tim_trig.h"

namespace adc_pot_tim_trig {

    void AdcPotTimTrig::init (void) {
	// init GPIO clock
	GPIOx_CLK_ENABLE (GPIOx);

	// init ADCx clock
	ADCx_CLK_ENABLE (ADCx);

	// init DMAx CLK, streams, link DMA HAndle, set priority and enable transfer
	// complete IT on the streams. 
	DMAx_CLK_ENABLE (DMAx);

	GPIO_InitStruct.Pin = this->gpio_pin;
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

	HAL_GPIO_Init (this->GPIOx, &this->GPIO_InitStruct);

	DMAx_Handle.Instance = this->dma_stream;
	DMAx_Handle.Init.Channel = this->dma_channel;
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

	if (HAL_DMA_Init (&this->DMAx_Handle) != HAL_OK) {
	    Error_Handler ();
	}

	// link DMA handle to ADC Handle
	__HAL_LINKDMA (&this->ADCx_Handle, DMA_Handle, this->DMAx_Handle);

	// NVIC for DMA transvfer complete IT
	HAL_NVIC_SetPriority (this->dma_irqn, 0, 0);
	HAL_NVIC_EnableIRQ (this->dma_irqn);

	// config ADCx 
	ADCx_Handle.Instance = this->ADCx;
	ADCx_Handle.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
	ADCx_Handle.Init.Resolution = ADC_RESOLUTION_12B;
	ADCx_Handle.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	ADCx_Handle.Init.ScanConvMode = DISABLE;
	ADCx_Handle.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
	ADCx_Handle.Init.ContinuousConvMode = DISABLE;
	ADCx_Handle.Init.NbrOfConversion = 1;
	ADCx_Handle.Init.DiscontinuousConvMode = DISABLE;
	ADCx_Handle.Init.NbrOfDiscConversion = 0;
	ADCx_Handle.Init.ExternalTrigConv = this->adc_ext_trig;
	ADCx_Handle.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;
	ADCx_Handle.Init.DMAContinuousRequests = ENABLE;

	// initialize ADCx 
	if (HAL_ADC_Init (&this->ADCx_Handle) != HAL_OK) {
	    Error_Handler ();
	}

	// configure ADCx regular channel
	ADCx_ChannelConf.Channel = this->adc_channel;
	ADCx_ChannelConf.Rank = 1;
	// APB2_CLK = HCLK/2 (84 MHz)
	// ADC_CLK = APB2_CLK/4 = 21MHz
	// Total Conversion Time = (ADC_RES + Sampling time) / ADC_CLK
	// 	      		 = 6µs
	// 
	// carefull with internal measurement channels
	// sampling time constraints must be respected 
	// (sampling time can be adjusted in function of 
	// ADC clock frequency and sampling time setting)
	ADCx_ChannelConf.SamplingTime = this->adc_sample_time; 
	ADCx_ChannelConf.Offset = 0;

	// initialiaze ADCx Channel
	if (HAL_ADC_ConfigChannel (&this->ADCx_Handle, &this->ADCx_ChannelConf) != HAL_OK) {
	    Error_Handler ();
	}

	// Start ADCx with DMAx
	if (HAL_ADC_Start_DMA (&this->ADCx_Handle, (uint32_t*)&(this->value) , 1) != HAL_OK) {
	    Error_Handler ();
	}
    }



    ADC_HandleTypeDef * AdcPotTimTrig::getAdcHandle (void) {
	return &(this->ADCx_Handle);
    }



    float32_t AdcPotTimTrig::getVoltage(void) {
	float32_t value_v;
	uint32_t value_adc_i;

	static const float32_t adc_max_value_12b = 4096.0f;
	static float32_t v_per_bit;

	v_per_bit = this->supply / adc_max_value_12b;
	value_adc_i = this->value;
	value_v = value_adc_i * v_per_bit;
	value_v /= 1000.0f;

	return value_v;
    }



    uint32_t AdcPotTimTrig::getAdcValue(void) {
	return (this->value);
    }

} // namespace adc_pot_tim_trig 



#ifdef __cplusplus
extern "C" {
#endif 

    //+ void HAL_ADC_MspInit (ADC_HandleTypeDef* hadc) {
    //+ }

#ifdef __cplusplus
}
#endif
