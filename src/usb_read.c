
/**
 * Copyright (c) 2025 MVK
 * SPDX-License-Identifier: MIT
 * 22-Apr-2025
 */

#include "usb_read.h"
#include "LCD_Test.h"

const char F_TIME[] = "TIME";
const char F_IP_ADDRESS[] = "IP_ADDRESS";
const char F_UPTIME[] = "UPTIME";
const char F_LOAD[] = "LOAD";
const char F_CPU_TEMP[] = "CPU_TEMP";
const char F_CHARGE[] = "CHARGE";
const char F_UPS_TIME[] = "UPS_TIME";
//
const char F_ON_BATTERY[] = "ON_BATTERY";								// bool
const char F_BATTERY_PERCENT[] = "BATTERY_PERCENT";						// int
const char F_NET_STATUS[] = "NET_STATUS";								// bool
const char F_TIME_REMAINING_OR_TO_FULL[] = "TIME_REMAINING_OR_TO_FULL"; // int
const char F_PROCESS_NAME[] = "PROCESS_NAME";							// string
const char F_PROCESS_STATUS[] = "PROCESS_STATUS";						// bool

extern char time[];
extern char ip_address[];
extern char uptime[];
extern char load[];
extern char cpu_temp[];
extern char charge[];
extern char ups_time[];
//
extern bool on_battery;
extern int battery_percent;
extern bool net_status;
extern int time_remaining_or_to_full;
extern char process_name[];
extern bool process_status;

const char CMD_READY[] = ":READY:";
const char CMD_OK[] = ":OK:";
const char CMD_FINISH[] = ":FINISH:";
const char CMD_RECEIVED[] = ":RECEIVED:";
const char CMD_RESEND[] = ":RESEND:";
const char CMD_RESET[] = ":RESET:";
const char CMD_ON[] = ":ON:";
const char CMD_OFF[] = ":OFF:";

extern bool display_toggle;
extern int display_dim[];
extern int display_dim_idx;
extern int display_dim_max;

extern bool output_ready_flag;

typedef enum
{
	s_NOP,
	S_WAIT,
	S_RECEIVE,
	S_PROCESSING
} st; // str.state

enum
{
	MAX_FIELDS = 26
}; // MAX. JSON FIELDS

typedef struct
{
	char str[MAX_LEN];
	int i;
	st state;
	alarm_id_t timeout_timer_id;
} str_t;

bool copy_values(char *json_str)
{
	json_t pool[MAX_FIELDS];

	json_t const *parent = json_create(json_str, pool, MAX_FIELDS);
	if (parent)
	{ // No error checking, of-course!
		// printf("Appears valid JSON [%d]: %s\n", str.state, str.str);
		json_t const *p_time = json_getProperty(parent, F_TIME);
		json_t const *p_ip_address = json_getProperty(parent, F_IP_ADDRESS);
		json_t const *p_uptime = json_getProperty(parent, F_UPTIME);
		json_t const *p_load = json_getProperty(parent, F_LOAD);
		json_t const *p_cpu_temp = json_getProperty(parent, F_CPU_TEMP);
		json_t const *p_charge = json_getProperty(parent, F_CHARGE);
		json_t const *p_ups_time = json_getProperty(parent, F_UPS_TIME);
		json_t const *p_on_battery = json_getProperty(parent, F_ON_BATTERY);
		json_t const *p_battery_percent = json_getProperty(parent, F_BATTERY_PERCENT);
		json_t const *p_net_status = json_getProperty(parent, F_NET_STATUS);
		json_t const *p_time_remaining_or_to_full = json_getProperty(parent, F_TIME_REMAINING_OR_TO_FULL);
		json_t const *p_process_name = json_getProperty(parent, F_PROCESS_NAME);
		json_t const *p_process_status = json_getProperty(parent, F_PROCESS_STATUS);

		char const *r_time = json_getValue(p_time);
		char const *r_ip_address = json_getValue(p_ip_address);
		char const *r_uptime = json_getValue(p_uptime);
		char const *r_load = json_getValue(p_load);
		char const *r_cpu_temp = json_getValue(p_cpu_temp);
		char const *r_charge = json_getValue(p_charge);
		char const *r_ups_time = json_getValue(p_ups_time);
		bool r_on_battery = json_getBoolean(p_on_battery);
		int r_battery_percent = json_getInteger(p_battery_percent);
		bool r_net_status = json_getBoolean(p_net_status);
		int r_time_remaining_or_to_full = json_getInteger(p_time_remaining_or_to_full);
		char const *r_process_name = json_getValue(p_process_name);
		bool r_process_status = json_getBoolean(p_process_status);

		if (!output_ready_flag) // copy new values only if previous values are not already displayed
		{
			strncpy(time, r_time, MAX_STRING_LENGTH - 1);
			strncpy(ip_address, r_ip_address, MAX_STRING_LENGTH - 1);
			strncpy(uptime, r_uptime, MAX_STRING_LENGTH - 1);
			strncpy(load, r_load, MAX_STRING_LENGTH - 1);
			strncpy(cpu_temp, r_cpu_temp, MAX_STRING_LENGTH - 1);
			strncpy(charge, r_charge, MAX_STRING_LENGTH - 1);
			strncpy(ups_time, r_ups_time, MAX_STRING_LENGTH - 1);
			on_battery = r_on_battery;
			battery_percent = r_battery_percent;
			net_status = r_net_status;
			time_remaining_or_to_full = r_time_remaining_or_to_full;
			strncpy(process_name, r_process_name, MAX_STRING_LENGTH - 1);
			process_status = r_process_status;

			output_ready_flag = true;
		}

		return true;
	}

	return false;
}

int64_t timer_callback(alarm_id_t id, void *user_data)
{
	str_t *str = (str_t *)user_data;
	str->i = 0;
	str->str[str->i] = 0;
	str->state = S_WAIT;

	return 0;
}

void timer_cancel(alarm_id_t *id, void *user_data)
{
	str_t *str = (str_t *)user_data;

	if (str->timeout_timer_id)
	{
		cancel_alarm(str->timeout_timer_id);
		str->timeout_timer_id = 0;
		*id = 0;
	}
}

void timer_reset(alarm_id_t id, void *user_data)
{
	; // to be implemented
}

void read_callback(void *ptr)
{
	str_t *str = (str_t *)ptr;
	str->str[str->i++] = stdio_getchar_timeout_us(100);
	str->str[str->i] = 0;

	return;
}

void send_str(const char *str)
{
	sleep_ms(100);
	stdio_puts(str);
	stdio_flush();
	fflush(stdin);
	fflush(stdout);

	return;
}

void read_proc(void)
{
	stdio_init_all();
	// stdio_usb_init();

	str_t str;
	str.i = 0;
	str.str[str.i] = 0;
	str.state = S_WAIT;
	str.timeout_timer_id = 0;
	char json_str[MAX_LEN] = "\0";
	output_ready_flag = false;

	while (1)
	{
		while (!stdio_usb_connected()) // no sense putting this here
		{
			set_brightness(display_dim[display_dim_idx = 0]);
			Paint_Clear(BLACK);
			sleep_ms(100);
		}

		str.i = 0;
		str.str[str.i] = 0;
		str.state = S_WAIT;

		stdio_set_chars_available_callback(read_callback, &str);

		set_brightness(display_dim[display_dim_idx = 1]);
		Paint_Clear(WHITE);

		while (1)
		{
			if (!strcmp(CMD_RESET, &(str.str[str.i - strlen(CMD_RESET)])))
			{
				str.i = 0;
				str.str[str.i] = 0;
				str.state = S_WAIT;
				send_str(CMD_OK);
			}
			else if (!strcmp(CMD_ON, &(str.str[str.i - strlen(CMD_ON)])))
			{
				set_brightness(5);
				Paint_Clear(WHITE);
				str.i = 0;
				str.str[str.i] = 0;
				str.state = S_WAIT;
				send_str(CMD_OK);
			}
			else if (!strcmp(CMD_OFF, &(str.str[str.i - strlen(CMD_OFF)])))
			{
				set_brightness(0);
				Paint_Clear(BLACK);
				str.i = 0;
				str.str[str.i] = 0;
				str.state = S_WAIT;
				send_str(CMD_OK);
			}

			switch (str.state)
			{
			case S_RECEIVE:
				if (!strcmp(CMD_FINISH, &(str.str[str.i - strlen(CMD_FINISH)])))
				{
					str.str[str.i - strlen(CMD_FINISH)] = 0;
					// stdio_set_chars_available_callback(NULL, NULL);
					str.state = S_PROCESSING;
					send_str(CMD_RECEIVED);
				}
				break;
			case S_PROCESSING:
				// printf("Just got [%d]: %s\n", str.state, str.str);
				strncpy(json_str, str.str, MAX_LEN - 1);
				json_str[MAX_LEN - 1] = 0;
				str.i = 0;
				str.str[str.i] = 0;
				str.state = S_WAIT;

				sleep_ms(100);
				if (!copy_values(json_str))
				{
					send_str(CMD_RESEND);
				}
				break;

			case S_WAIT:
			default:
				if (!strcmp(CMD_READY, &(str.str[str.i - strlen(CMD_READY)])))
				{
					str.i = 0;
					str.str[str.i] = 0;
					str.state = S_RECEIVE;
					send_str(CMD_OK);
				}
				break;
			}
#if 0
			printf("\nPICO says [%d]: %s\n", str.state, str.str);
			printf("TIME: %s\n", time);
			printf("IP Address: %s\n", ip_address);
			printf("Uptime: %s\n", uptime);
			printf("Load: %s\n", load);
			printf("Temp: %s\n", cpu_temp);
			printf("Charge: %s\n", charge);
			printf("UPS Time: %s\n", ups_time);
			printf("On battery: %b\n", on_battery);
			printf("Batt percent: %d\n", battery_percent);
			printf("Net is: %b\n", net_status);
			printf("Time RotF: %d\n", time_remaining_or_to_full);
			printf("Process [%s]: %b\n", process_name, process_status);
#else
			sleep_ms(100);
			stdio_putchar('.');
			stdio_flush();
			fflush(stdin);
			fflush(stdout);
#endif
		}
	}

	return;
}
