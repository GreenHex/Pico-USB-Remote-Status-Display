/**
 * usb_read.h
 * Copyright (c) 2025 MVK
 * 24-Apr-2025
 */

/*
{
"TIME": "10:56 23-01-2025",
"IP_ADDRESS": "192.168.5.23",
"UPTIME": "15 d, 10 h",
"LOAD": "5%",
"CPU_TEMP": "88\"C",
"CHARGE": "53%",
"UPS_TIME": "23 min"
"ON_BATTERY": "false",
"BATTERY_PERCENT": "10",
"NET_STATUS": "true",
"TIME_REMAINING_OR_TO_FULL": "57",
"PROCESS_NAME": "LCD",
"PROCESS_STATUS": "false"
}

{"TIME": "10:56 23-01-2025"}:FINISH:
*/

#ifndef _USB_READ_H_
#define _USB_READ_H_

#ifndef PICO_STDIO_USB_CONNECT_WAIT_TIMEOUT_MS
#define PICO_STDIO_USB_CONNECT_WAIT_TIMEOUT_MS (5000)
#endif

#ifndef PICO_STDIO_USB_POST_CONNECT_WAIT_DELAY_MS
#define PICO_STDIO_USB_POST_CONNECT_WAIT_DELAY_MS (1500)
#endif

// #include "pico/stdio_usb.h"
#include "/home/mvk/.pico-sdk/sdk/2.1.1/src/rp2_common/pico_stdio_usb/include/pico/stdio_usb.h"
#include "pico/stdlib.h"
#include "pico/stdio.h"
#include <stdio.h>
#include <string.h>
#include "tiny-json.h"

#define MAX_STRING_LENGTH 32
#define MAX_LEN 1024
#define CMD_LEN 10
#define TIMEOUT_TIME 10000

int64_t timer_callback(alarm_id_t id, void *user_data);
void read_callback(void *ptr);
void read_proc(void);

#endif // _USB_READ_H_
