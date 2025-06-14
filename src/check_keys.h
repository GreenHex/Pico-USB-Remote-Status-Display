/**
 * check_keys.h
 * Copyright (c) 2025 MVK
 * 23-Apr-2025
 */

#ifndef _CHECK_KEYS_H_
#define _CHECK_KEYS_H_

#include "LCD_Test.h"
#include "LCD_1in3.h"
#include "pico/multicore.h"
#include "DEV_Config.h"

void check_keys_init(void);
void check_keys(void);

#endif // _CHECK_KEYS_H_
