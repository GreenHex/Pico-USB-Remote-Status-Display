/**
 * multicore.c
 * Copyright (c) 2025 MVK
 * 23-Apr-2025
 *
 */

#include "check_keys.h"

extern UWORD *BlackImage;

extern uint8_t keyA;
extern uint8_t keyB;
extern uint8_t keyX;
extern uint8_t keyY;

extern uint8_t up;
extern uint8_t dowm;
extern uint8_t left;
extern uint8_t right;
extern uint8_t ctrl;

bool display_toggle = true;
int display_dim[] = {0, 10, 25, 50, 100};
int display_dim_idx = (sizeof(display_dim) / sizeof(display_dim[0])) - 1;
int display_dim_max = (sizeof(display_dim) / sizeof(display_dim[0])) - 1;

void check_keys_init(void)
{
	SET_Infrared_PIN(keyA);
	SET_Infrared_PIN(keyB);
	SET_Infrared_PIN(keyX);
	SET_Infrared_PIN(keyY);

	SET_Infrared_PIN(up);
	SET_Infrared_PIN(dowm);
	SET_Infrared_PIN(left);
	SET_Infrared_PIN(right);
	SET_Infrared_PIN(ctrl);
}

void check_keys(void)
{
	if (!DEV_Digital_Read(keyA))
	{
		display_dim_idx = (display_toggle = !display_toggle) ? display_dim_max : 0;
		set_brightness(display_dim[display_dim_idx]);
		sleep_ms(500);
	}
	else if (!DEV_Digital_Read(keyB))
	{
		if (display_dim_idx >= display_dim_max)
		{
			display_dim_idx = 0;
			display_toggle = false;
		}
		else
		{
			++display_dim_idx;
			display_toggle = true;
		}
		set_brightness(display_dim[display_dim_idx]);
		sleep_ms(500);
	}
}
