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
// API Driver for lcd 16x4 display.
//

#include "lcd16x2_api.h"

namespace lcd16x2_api {

#define HD44780_CLEAR                          0x01

#define HD44780_HOME                           0x02

#define HD44780_ENTRY_MODE                     0x04
        #define HD44780_EM_SHIFT_CURSOR        0x00
        #define HD44780_EM_SHIFT_DISPLAY       0x01
        #define HD44780_EM_DECREMENT           0x00
        #define HD44780_EM_INCREMENT           0x02

#define HD44780_DISPLAY_ONOFF                  0x08
        #define HD44780_DISPLAY_OFF            0x00
        #define HD44780_DISPLAY_ON             0x04
        #define HD44780_CURSOR_OFF             0x00
        #define HD44780_CURSOR_ON              0x02
        #define HD44780_CURSOR_NOBLINK     	   0x00
        #define HD44780_CURSOR_BLINK           0x01

#define HD44780_DISPLAY_CURSOR_SHIFT 					 0x10
        #define HD44780_SHIFT_CURSOR           0x00
        #define HD44780_SHIFT_DISPLAY          0x08
        #define HD44780_SHIFT_LEFT             0x00
        #define HD44780_SHIFT_RIGHT            0x04

#define HD44780_FUNCTION_SET                   0x20
        #define HD44780_FONT5x7                0x00
        #define HD44780_FONT5x10               0x04
        #define HD44780_ONE_LINE               0x00
        #define HD44780_TWO_LINE               0x08
        #define HD44780_4_BIT                  0x00
        #define HD44780_8_BIT                  0x10

#define HD44780_CGRAM_SET                      0x40
#define HD44780_DDRAM_SET                      0x80

#define PROGRESSPIXELS_PER_CHAR									5

static const uint8_t CustomChar [] =
{
	0x00, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x00, // 0. 0/5 full progress block
	0x00, 0x1F, 0x10, 0x10, 0x10, 0x10, 0x1F, 0x00, // 1. 1/5 full progress block
	0x00, 0x1F, 0x18, 0x18, 0x18, 0x18, 0x1F, 0x00, // 2. 2/5 full progress block
	0x00, 0x1F, 0x1C, 0x1C, 0x1C, 0x1C, 0x1F, 0x00, // 3. 3/5 full progress block
	0x00, 0x1F, 0x1E, 0x1E, 0x1E, 0x1E, 0x1F, 0x00, // 4. 4/5 full progress block
	0x00, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x00, // 5. 5/5 full progress block
};


void Lcd::Init (void) {
	/* Init GPIO pins used for LCD interface */
	/* TODO find a way to make it generic to any GPIO bank */
	__HAL_RCC_GPIOE_CLK_ENABLE();

	GPIO_InitStruct.Pin = RsPin | RwPin | EnPin | Db4Pin \
											| Db5Pin | Db6Pin | Db7Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
	
  HAL_GPIO_Init(GpioBank, &GPIO_InitStruct);

	/* Initialize state of the LCD pins */
  HAL_GPIO_WritePin(GpioBank, RsPin	| RwPin	| EnPin \
														| Db4Pin | Db5Pin	| Db6Pin | Db7Pin
														, GPIO_PIN_RESET); 

	/* Wait 100ms for the internal initialisation of the display */
	HAL_Delay (100);

	/* Initialise display before busy flag is usable */
	WriteCmd4bits (0x03);
	HAL_Delay (5);

	WriteCmd4bits (0x03);
	/* TODO Shall be 100µs */
	HAL_Delay (5);

	WriteCmd4bits (0x03);
	HAL_Delay (5);

	/* Function set DL=0 - 4 bits */
	WriteCmd4bits (0x02);
	HAL_Delay (1);

	/* Function set DL=0 - 4 bits, N=1 - 2 lines, F=0 - 5*8 dots */
	WriteCmd (HD44780_FUNCTION_SET | HD44780_4_BIT | HD44780_TWO_LINE | HD44780_FONT5x7);

	/* Display Off */
	WriteCmd (HD44780_DISPLAY_ONOFF | HD44780_DISPLAY_OFF);

	/* Display Clear */
	Clear ();

	/* Entry mode set - Increment */
	WriteCmd (HD44780_ENTRY_MODE | HD44780_EM_INCREMENT);

	/* Display On */
	WriteCmd (HD44780_DISPLAY_ONOFF | HD44780_DISPLAY_ON);

	/* Add Custom characters in CGRAM */ 
	AddCustomChar(0, 0, CustomChar);
	AddCustomChar(1, 1, CustomChar);
	AddCustomChar(2, 2, CustomChar);
	AddCustomChar(3, 3, CustomChar);
	AddCustomChar(4, 4, CustomChar);
	AddCustomChar(5, 5, CustomChar);
}

void Lcd::Clear (void) {
	WriteCmd (HD44780_CLEAR);
}

void Lcd::Print (const char * str) {
	while (*str) 
			WriteData (*str++);
}

void Lcd::PrintXY (const char * str, uint8_t x, uint8_t y) {
		Locate (x, y);
		Print (str);
}

void Lcd::RetHome () {
	WriteCmd (HD44780_HOME);
}

// x range [0:15]
// y range [0:1], 0 for first line, 1 for second line
void Lcd::Locate (uint8_t x, uint8_t y) {
	WriteCmd (HD44780_DDRAM_SET | (x + (0x40 * y)));

}

void Lcd::ProgressBar (uint32_t progress, uint32_t max_progress, uint8_t lcd_char_length) {

	uint32_t i;
	uint32_t pixelprogress;
	uint8_t c;

	// draw a progress bar displaying (progress / maxprogress)
	// starting from the current cursor position
	// with a total length of "length" characters
	// ***note, LCD chars 0-5 must be programmed as the bar characters
	// char 0 = empty ... char 5 = full

	// total pixel length of bargraph equals length*PROGRESSPIXELS_PER_CHAR;
	// pixel length of bar itself is
	pixelprogress = ((progress * ((uint32_t)lcd_char_length * (uint32_t)PROGRESSPIXELS_PER_CHAR)) / max_progress);
	
	// print exactly "length" characters
	for(i=0; i<(uint32_t)lcd_char_length; ++i)
	{
		// check if this is a full block, or partial or empty
		// (uint16_t) cast is needed to avoid sign comparison warning
		if( ((i*(uint32_t)PROGRESSPIXELS_PER_CHAR)+5) > pixelprogress )
		{
			// this is a partial or empty block
			if( ((i*(uint32_t)PROGRESSPIXELS_PER_CHAR)) > pixelprogress )
			{
				// this is an empty block
				// use space character?
				c = 0;
			}
			else
			{
				// this is a partial block
				c = pixelprogress % PROGRESSPIXELS_PER_CHAR;
			}
		}
		else
		{
			// this is a full block
			c = 5;
		}
		
		// write character to display
		WriteData (c);
	}

}

void Lcd::AddCustomChar(uint8_t lcd_char_num, uint8_t rom_char_num, const uint8_t * const rom_char_array) {
	uint8_t saveDDRAMAddr;
	uint8_t i;
	saveDDRAMAddr = ReadBusyFlag () & 0x7F;

	for (i=0; i<8; ++i) {
		WriteCmd (HD44780_CGRAM_SET | ((lcd_char_num << 3) + i));
		WriteData (*(rom_char_array + (rom_char_num << 3) + i));
	}

	WriteCmd (HD44780_DDRAM_SET | saveDDRAMAddr);
}

void Lcd::WriteCmd (uint8_t cmd) {
	HAL_GPIO_WritePin (GpioBank, RsPin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin (GpioBank, RwPin, GPIO_PIN_RESET);

	/* TODO replace by busy flag check */ 
	WriteCmd4bits (cmd >> 4);  	// high nibble
	WriteCmd4bits (cmd & 0x0F);	// low nibble
	while (ReadBusyFlag() & 0x80);
}

void Lcd::WriteData (uint8_t data) {
	HAL_GPIO_WritePin (GpioBank, RsPin, GPIO_PIN_SET);
	HAL_GPIO_WritePin (GpioBank, RwPin, GPIO_PIN_RESET);

	/* TODO replace by busy flag check */ 
	WriteCmd4bits (data >> 4);  	// high nibble
	WriteCmd4bits (data & 0x0F);	// low nibble
	while (ReadBusyFlag() & 0x80);
} 

/* Busy flag cannot be used */
void Lcd::WriteCmd4bits (uint8_t cmd) {
	HAL_GPIO_WritePin (GpioBank, EnPin, GPIO_PIN_SET);
	HAL_GPIO_WritePin (GpioBank, Db7Pin, (cmd & 0x08) != 0 ? GPIO_PIN_SET : GPIO_PIN_RESET);
	HAL_GPIO_WritePin (GpioBank, Db6Pin, (cmd & 0x04) != 0 ? GPIO_PIN_SET : GPIO_PIN_RESET);
	HAL_GPIO_WritePin (GpioBank, Db5Pin, (cmd & 0x02) != 0 ? GPIO_PIN_SET : GPIO_PIN_RESET);
	HAL_GPIO_WritePin (GpioBank, Db4Pin, (cmd & 0x01) != 0 ? GPIO_PIN_SET : GPIO_PIN_RESET);
	HAL_GPIO_WritePin (GpioBank, EnPin, GPIO_PIN_RESET);
}

uint8_t Lcd::ReadBusyFlag (void) {
	uint8_t status = 0;

	GPIO_InitStruct.Pin =  Db4Pin | Db5Pin | Db6Pin | Db7Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
  HAL_GPIO_Init(GpioBank, &GPIO_InitStruct);

	HAL_GPIO_WritePin (GpioBank, RsPin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin (GpioBank, RwPin, GPIO_PIN_SET);
	status |= ReadNibble() << 4;
	status |= ReadNibble();

	HAL_GPIO_WritePin (GpioBank, RwPin, GPIO_PIN_RESET);
	GPIO_InitStruct.Pin =  Db4Pin | Db5Pin | Db6Pin | Db7Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
  HAL_GPIO_Init(GpioBank, &GPIO_InitStruct);

	return status;
}

uint8_t Lcd::ReadNibble (void) {
	uint8_t val = 0;
	HAL_GPIO_WritePin (GpioBank, EnPin, GPIO_PIN_SET);
	val |= HAL_GPIO_ReadPin(GpioBank, Db4Pin);
	val |= HAL_GPIO_ReadPin(GpioBank, Db5Pin) << 1;
	val |= HAL_GPIO_ReadPin(GpioBank, Db6Pin) << 2;
	val |= HAL_GPIO_ReadPin(GpioBank, Db7Pin) << 3;
	HAL_GPIO_WritePin (GpioBank, EnPin, GPIO_PIN_RESET);
	return val;
}

} // namespace lcd16x2_api 
