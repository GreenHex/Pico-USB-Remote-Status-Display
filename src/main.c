/**
 * main.c
 * Copyright (c) 2025 MVK
 * 24-Apr-2025
 */

#include "LCD_Test.h"
#include "LCD_1in3.h"
#include "pico/multicore.h"

#include <stdbool.h>
#include <stdio.h> //printf()
#include <stdlib.h>
#include "pico/stdio.h"
#include "pico/stdlib.h"
#include "string.h"
#include "usb_read.h"
#include "check_keys.h"

#define LINE_WIDTH_ADJUSTMENT 5
#define LINE_HEIGHT_ADJUSTMENT 5

extern void check_keys(void);

UWORD *BlackImage;
uint8_t keyA = 15;
uint8_t keyB = 17;
uint8_t keyX = 19;
uint8_t keyY = 21;

uint8_t up = 2;
uint8_t dowm = 18;
uint8_t left = 16;
uint8_t right = 20;
uint8_t ctrl = 3;

char time[MAX_STRING_LENGTH] = "\0";
char ip_address[MAX_STRING_LENGTH] = "\0";
char uptime[MAX_STRING_LENGTH] = "\0";
char load[MAX_STRING_LENGTH] = "\0";
char cpu_temp[MAX_STRING_LENGTH] = "\0";
char charge[MAX_STRING_LENGTH] = "\0";
char ups_time[MAX_STRING_LENGTH] = "\0";
//
bool on_battery = false;
int battery_percent = 0;
bool net_status = false;
int time_remaining_or_to_full = 0;
char process_name[MAX_STRING_LENGTH] = "\0";
bool process_status = false;

bool output_ready_flag = false;

int main(void)
{
	stdio_usb_init();

	if (DEV_Module_Init() != 0)
	{
		return -1;
	}

	LCD_1IN3_Init(HORIZONTAL);
	LCD_1IN3_Clear(WHITE);

	// LCD_SetBacklight(1023);
	UDOUBLE Imagesize = LCD_1IN3_HEIGHT * LCD_1IN3_WIDTH * 2;

	if ((BlackImage = (UWORD *)malloc(Imagesize)) == NULL)
	{
		printf("Failed to apply for black memory...\r\n");
		exit(0);
	}
	// /*1.Create a new image cache named IMAGE_RGB and fill it with white*/
	Paint_NewImage((UBYTE *)BlackImage, LCD_1IN3.WIDTH, LCD_1IN3.HEIGHT, 0, WHITE);
	Paint_SetScale(65);
	Paint_Clear(WHITE);
	Paint_SetRotate(ROTATE_0);
	Paint_Clear(WHITE);
	set_brightness(10);

	bool on_battery = true;

	multicore_reset_core1();
	multicore_launch_core1(read_proc);

	check_keys_init();

	UWORD batt_colour = BLACK;

	while (1)
	{
		if (output_ready_flag)
		{

			if (battery_percent > 50)
				batt_colour = GREEN;
			else if (battery_percent > 20)
				batt_colour = YELLOW;
			else
				batt_colour = RED;

			Paint_Clear(BLACK);
			Paint_DrawRectangle(2, 2, LCD_1IN3_WIDTH - 2, 28, WHITE, DOT_PIXEL_1X1, DRAW_FILL_FULL);
			Paint_DrawString_EN((LCD_1IN3_WIDTH - strlen(time) * Font20.Width) / 2, 7, time, &Font20, BLACK, WHITE);
			//
			Paint_DrawString_EN(5, 37, "IP Address", &Font12, MAGENTA, BLACK);
			Paint_DrawString_EN(LCD_1IN3_WIDTH - strlen(ip_address) * Font20.Width - LINE_WIDTH_ADJUSTMENT, 57, ip_address, &Font20, WHITE, BLACK);
			Paint_DrawLine(5, 57 + Font20.Height + LINE_HEIGHT_ADJUSTMENT, LCD_1IN3_WIDTH - 5, 57 + Font20.Height + LINE_HEIGHT_ADJUSTMENT, WHITE, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
			//
			Paint_DrawString_EN(5, 87, "Uptime", &Font12, MAGENTA, BLACK);
			Paint_DrawString_EN(LCD_1IN3_WIDTH - strlen(uptime) * Font20.Width - LINE_WIDTH_ADJUSTMENT, 107, uptime, &Font20, WHITE, BLACK);
			Paint_DrawLine(5, 107 + Font20.Height + LINE_HEIGHT_ADJUSTMENT, LCD_1IN3_WIDTH - 5, 107 + Font20.Height + LINE_HEIGHT_ADJUSTMENT, WHITE, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
			//
			Paint_DrawString_EN(5, 137, "CPU Load", &Font12, MAGENTA, BLACK);
			Paint_DrawString_EN(LCD_1IN3_WIDTH / 2 - strlen(load) * Font20.Width - LINE_WIDTH_ADJUSTMENT, 157, load, &Font20, WHITE, BLACK);
			//
			Paint_DrawString_EN(5 + LCD_1IN3_WIDTH / 2, 137, !on_battery ? "On Battery" : "Battery", &Font12, MAGENTA, BLACK);
			Paint_DrawString_EN(LCD_1IN3_WIDTH - strlen(charge) * Font20.Width - LINE_WIDTH_ADJUSTMENT, 157, charge, &Font20, batt_colour, BLACK);
			Paint_DrawLine(5, 157 + Font20.Height + LINE_HEIGHT_ADJUSTMENT, LCD_1IN3_WIDTH / 2, 157 + Font20.Height + LINE_HEIGHT_ADJUSTMENT, WHITE, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
			Paint_DrawLine(LCD_1IN3_WIDTH / 2, 157 + Font20.Height + LINE_HEIGHT_ADJUSTMENT, LCD_1IN3_WIDTH - 5, 157 + Font20.Height + LINE_HEIGHT_ADJUSTMENT, WHITE, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
			//
			Paint_DrawString_EN(5, 187, "CPU Temperature", &Font12, MAGENTA, BLACK);
			Paint_DrawString_EN(LCD_1IN3_WIDTH / 2 - strlen(cpu_temp) * Font20.Width - LINE_WIDTH_ADJUSTMENT, 207, cpu_temp, &Font20, WHITE, BLACK);
			//
			Paint_DrawString_EN(5 + LCD_1IN3_WIDTH / 2, 187, !on_battery ? "Time To Empty" : "Time To Full", &Font12, MAGENTA, BLACK);
			Paint_DrawString_EN(LCD_1IN3_WIDTH - strlen(ups_time) * Font20.Width - LINE_WIDTH_ADJUSTMENT, 207, ups_time, &Font20, !on_battery & (time_remaining_or_to_full < 20) ? RED : WHITE, BLACK);
			Paint_DrawLine(5, 207 + Font20.Height + LINE_HEIGHT_ADJUSTMENT, LCD_1IN3_WIDTH - 5, 207 + Font20.Height + LINE_HEIGHT_ADJUSTMENT, WHITE, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
			//
			Paint_DrawLine(LCD_1IN3_WIDTH / 2, 132, LCD_1IN3_WIDTH / 2, 240 - 8, WHITE, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
			//
			if (!net_status)
			{
				Paint_DrawChar(10, 57, '~', &Font20, BLACK, GREEN);
			}
			else
			{
				Paint_DrawChar(10, 57, 'X', &Font20, BLACK, RED);
			}
			output_ready_flag = false;
		}
		check_keys();

		LCD_1IN3_Display(BlackImage);

		sleep_ms(100);
	}

	free(BlackImage);
	BlackImage = NULL;

	DEV_Module_Exit();
	return 0;
}
