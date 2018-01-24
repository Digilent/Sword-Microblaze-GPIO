/*
 * sword_sseg.c
 *
 *  Created on: Jan 22, 2018
 *      Author: arthur
 */

/*
 * This file instantiates a simple library for interacting with the Sword board's 7-segment display.
 * */

#include "sword_sseg.h"
#include "sleep.h"

void SSeg_setState(XGpio *InstancePtr, u32 channel, SSeg_data data) {
	u32 i, sdo;

	for (i=0; i<64; i++)
	{
		if (((data.data[i/8] >> (i%8)) & 0x1) != 0) // if the packet bit corresponding to i is high
			sdo = SSEG_SDO_MASK; // set the sdo data bit
		else
			sdo = 0x0;
		XGpio_DiscreteWrite(InstancePtr, channel, sdo); // negedge clk, set shift data out, pull enable low
		usleep(1);
		// posedge clk, data is shifted into the shift registers. maintain shift data out
		XGpio_DiscreteWrite(InstancePtr, channel, SSEG_SCLK_MASK | sdo);
		usleep(1);
	}

	XGpio_DiscreteWrite(InstancePtr, channel, SSEG_EN_MASK | SSEG_SCLK_MASK); // set enable high
}

SSeg_data SSeg_hex2Data(u32 sseg_hex, u8 digit_enable, u8 dp_enable)
{
	u8 i;
	SSeg_data packet;

	for (i=0; i<8; i++)
	{
		if (((digit_enable >> i) & 0x1) == 0)
			packet.data[i] = 0;
		else switch ((sseg_hex >> (i << 2)) & 0xF)
		{
		case 0x0: packet.data[i] = 0b11111100 | ((dp_enable >> i) & 0x1); break;
		case 0x1: packet.data[i] = 0b01100000 | ((dp_enable >> i) & 0x1); break;
		case 0x2: packet.data[i] = 0b11011010 | ((dp_enable >> i) & 0x1); break;
		case 0x3: packet.data[i] = 0b11110010 | ((dp_enable >> i) & 0x1); break;
		case 0x4: packet.data[i] = 0b01100110 | ((dp_enable >> i) & 0x1); break;
		case 0x5: packet.data[i] = 0b10110110 | ((dp_enable >> i) & 0x1); break;
		case 0x6: packet.data[i] = 0b10111110 | ((dp_enable >> i) & 0x1); break;
		case 0x7: packet.data[i] = 0b11100000 | ((dp_enable >> i) & 0x1); break;
		case 0x8: packet.data[i] = 0b11111110 | ((dp_enable >> i) & 0x1); break;
		case 0x9: packet.data[i] = 0b11110110 | ((dp_enable >> i) & 0x1); break;
		case 0xA: packet.data[i] = 0b11101110 | ((dp_enable >> i) & 0x1); break;
		case 0xb: packet.data[i] = 0b00111110 | ((dp_enable >> i) & 0x1); break;
		case 0xC: packet.data[i] = 0b10011100 | ((dp_enable >> i) & 0x1); break;
		case 0xd: packet.data[i] = 0b01111010 | ((dp_enable >> i) & 0x1); break;
		case 0xE: packet.data[i] = 0b10011110 | ((dp_enable >> i) & 0x1); break;
		case 0xF: packet.data[i] = 0b10001110 | ((dp_enable >> i) & 0x1); break;
		default: packet.data[i] = 0;
		}
	}
	return packet;
}
