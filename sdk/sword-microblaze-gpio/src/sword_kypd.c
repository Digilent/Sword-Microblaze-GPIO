/*
 * sword_kypd.c
 *
 *  Created on: Jan 22, 2018
 *      Author: arthur
 */

/*
 * This file instantiates a simple library for interacting with the Sword board's 25-button keypad.
 * */

#include "sword_kypd.h"
#include "sleep.h"

u32 Kypd_hammingWeight(u32 data);

// Calculate the sum of the bits of data.
u32 Kypd_hammingWeight(u32 data)
{
	data = data - ((data >> 1) & 0x55555555);
	data = (data & 0x33333333) + ((data >> 2) & 0x33333333);
	data = (data & 0x0f0f0f0f) + ((data >> 4) & 0x0f0f0f0f);
	data = (data & 0x00ff00ff) + ((data >> 8) & 0x00ff00ff);
	data = (data & 0x0000ffff) + ((data >> 16) & 0x0000ffff);
	return data;
}

//return the number of buttons pressed, which button is pressed if only one, and the raw 25 bits of data shifted in.
Kypd_data Kypd_getData(XGpio *InstancePtr, u32 channel)
{
	u32 row;
	u32 data;
	u32 i;
	Kypd_data state;

	data = 0;
	for (row = 0; row < 5; row++)
	{
		XGpio_SetDataDirection(InstancePtr, channel, 0x3FF & ~(0b100000 << row));
		XGpio_DiscreteClear(InstancePtr, channel, 0b100000 << row);
		usleep(1);
		data = (data >> 5) | ((XGpio_DiscreteRead(InstancePtr, channel) & 0x1F) << 20);
		usleep(1);
	}
	state.raw_data = data;
	state.num_buttons = Kypd_hammingWeight((~data) & 0x1FFFFFF);
	if (state.num_buttons == 1)
	{
		for (i=0; i<25; i++)
			if (((data >> i) & 0x1) == 0) {
				state.button_pressed_row = i / 5;
				state.button_pressed_col = i % 5;
			}
	}

	return state;
}
