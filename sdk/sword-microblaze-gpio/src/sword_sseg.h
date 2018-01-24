/*
 * sword_sseg.h
 *
 *  Created on: Jan 22, 2018
 *      Author: arthur
 */

/*
 * This file declares a simple library for interacting with the Sword board's 7-segment display.
 * */

#ifndef SRC_SWORD_SSEG_H_
#define SRC_SWORD_SSEG_H_

#include "xgpio.h"
#include "xil_types.h"

#define SSEG_SCLK_MASK 0x1
#define SSEG_EN_MASK 0x2
#define SSEG_SDO_MASK 0x4

typedef struct {
	u8 data[8];
} SSeg_data;

SSeg_data SSeg_hex2Data(u32 sseg_hex, u8 digit_enable, u8 dp_enable);
void SSeg_setState(XGpio *InstancePtr, u32 channel, SSeg_data data);

#endif /* SRC_SWORD_SSEG_H_ */
