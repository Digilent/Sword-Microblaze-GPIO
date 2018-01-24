/*
 * sword_kypd.h
 *
 *  Created on: Jan 22, 2018
 *      Author: arthur
 */

#ifndef SRC_SWORD_KYPD_H_
#define SRC_SWORD_KYPD_H_

/*
 * This file declares a simple library for interacting with the Sword board's 25-button keypad.
 * */

#include "xgpio.h"
#include "xil_types.h"

typedef struct {
	u32 raw_data;
	u32 num_buttons;
	u8 button_pressed_row;
	u8 button_pressed_col;
} Kypd_data;

Kypd_data Kypd_getData(XGpio *InstancePtr, u32 channel);

#endif /* SRC_SWORD_KYPD_H_ */
