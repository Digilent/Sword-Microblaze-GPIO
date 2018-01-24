#include "xil_types.h"
#include "stdio.h"
#include "xil_printf.h"
#include "xil_cache.h"
#include "xparameters.h"
#include "xgpio.h"
#include "sleep.h"
#include "sword_kypd.h"
#include "sword_sseg.h"
#include "sseg_loop.h"

XGpio gpio[3];
//Device/Channel macros for AXI GPIO controllers.
#define LED &gpio[0], 1
#define SWITCH &gpio[0], 2
#define KYPD &gpio[1], 1
#define SSEG &gpio[1], 2
#define RGB_LED &gpio[2], 1

int main() {
	u32 i, counter, switch_state, rgb_select = 1, sseg_loop_idx, rgb_duty;
	Kypd_data new_state, old_state;

	// Enable memory caches
	Xil_ICacheEnable();
	Xil_DCacheEnable();

	//initialize GPIO controllers
	for (i=0; i<XPAR_XGPIO_NUM_INSTANCES; i=i+1)
		XGpio_Initialize(&gpio[i], i);
	XGpio_SetDataDirection(KYPD, 0x3FF); // set all keypad tristates to input
	XGpio_SetDataDirection(LED, 0x0000); // set all led tristates to output
	XGpio_SetDataDirection(SWITCH, 0xFFFF); // set all switch tristates to input
	XGpio_SetDataDirection(RGB_LED, 0x00); // set all rgb led tristates to output
	XGpio_SetDataDirection(SSEG, 0x0); // set all 7-segment tristates to output
	XGpio_DiscreteWrite(SSEG, SSEG_SCLK_MASK); // initialize 7-segment outputs, disable the seven segment

	xil_printf("Welcome to the Sword Microblaze GPIO Demo!\r\n");
	xil_printf("This terminal will display 25-button keypad button presses.\r\n");

	counter = 0;
	old_state = Kypd_getData(KYPD);
	sseg_loop_idx = 0;
	SSeg_setState(SSEG, sseg_loop[sseg_loop_idx]);

	rgb_select = 0b100;
	while (1) {
		//Switches & LEDs:
		// read the state of the 16 switches, then copy that state to the 16 LEDs.
		switch_state = XGpio_DiscreteRead(SWITCH);
		XGpio_DiscreteWrite(LED, switch_state);

		//Keypad:
		// read the state of the keypad, if the state has changed and only one button is pressed,
		// tell the user which button was pressed.
		if (counter % 100 == 99) {
			new_state = Kypd_getData(KYPD); // reading the state of the keypad takes ~10us
			if (new_state.raw_data != old_state.raw_data && new_state.num_buttons == 1)
			{
				xil_printf("Button R%dC%d Pressed!\r\n", new_state.button_pressed_row, new_state.button_pressed_col);
			}
			old_state = new_state;
		}

		//7-Segment Display:
		// iterate through the 7-segment loop lookup table
		if (counter % 10000 == 9999) {
			if (sseg_loop_idx >= 35)
				sseg_loop_idx = 0;
			else
				sseg_loop_idx++;
			SSeg_setState(SSEG, sseg_loop[sseg_loop_idx]); // setting the 7-segment state takes ~129us
		}

		//RGB LEDs:
		// apply a simple PWM on the RGB LEDs
		rgb_duty = (counter / 250) % 250;
		if (counter % 250 == 0) {
			XGpio_DiscreteWrite(RGB_LED, (rgb_select << 3) | (~rgb_select & 0x7));
		} else if (counter % 250 == rgb_duty) {
			XGpio_DiscreteWrite(RGB_LED, 0x00);
		}

		// change the colors displayed on the RGB LEDs at the
		// LED16 iterates through red -> green -> blue
		// LED17 iterates through teal -> magenta -> yellow
		if (counter % 62500 == 62499) {
			if (rgb_select == 0b001)
				rgb_select = 0b100;
			else
				rgb_select = rgb_select >> 1;
		}

		if (counter >= 999999)
			counter = 0;
		else
			counter++;
		usleep(1);
	}
	//disable memory caches
	Xil_ICacheDisable();
	Xil_DCacheDisable();
}
