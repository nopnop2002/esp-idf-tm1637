/**
 * @file app_main.c
 * @brief Example application for the TM1637 LED segment display
 */

#include <stdio.h>
#include <stdbool.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <time.h>
#include <sys/time.h>
#include <esp_system.h>
#include <driver/gpio.h>
#include <esp_log.h>

#include "sdkconfig.h"
#include "tm1637.h"

#define TAG "app"

const gpio_num_t LED_CLK = CONFIG_TM1637_CLK_PIN;
const gpio_num_t LED_DTA = CONFIG_TM1637_DIO_PIN;

void tm1637_task(void * arg)
{
	tm1637_led_t * lcd = tm1637_init(LED_CLK, LED_DTA);

#if 0
	tm1637_set_brightness(lcd, 7);
	while (true) {
		tm1637_set_segment_fixed(lcd, lcd->segment_idx[0], 0xFF);
		tm1637_set_segment_fixed(lcd, lcd->segment_idx[1], 0xFF);
		tm1637_set_segment_fixed(lcd, lcd->segment_idx[2], 0xFF);
		tm1637_set_segment_fixed(lcd, lcd->segment_idx[3], 0xFF);
		tm1637_set_segment_fixed(lcd, lcd->segment_idx[4], 0xFF);
		tm1637_set_segment_fixed(lcd, lcd->segment_idx[5], 0xFF);
		vTaskDelay(100);
	}
#endif

	while (true) {
		// Test segment control
		uint8_t seg_data[] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20};
		for (uint8_t x=0; x<32; ++x)
		{
			uint8_t v_seg_data = seg_data[x%6];
			tm1637_set_segment_fixed(lcd, lcd->segment_idx[0], v_seg_data);
			tm1637_set_segment_fixed(lcd, lcd->segment_idx[1], v_seg_data);
			tm1637_set_segment_fixed(lcd, lcd->segment_idx[2], v_seg_data);
			tm1637_set_segment_fixed(lcd, lcd->segment_idx[3], v_seg_data);
			tm1637_set_segment_fixed(lcd, lcd->segment_idx[4], v_seg_data);
			tm1637_set_segment_fixed(lcd, lcd->segment_idx[5], v_seg_data);
			vTaskDelay(10);
		}

		// Test brightness
		for (int x=0; x<7; x++) {
			tm1637_set_brightness(lcd, x);
			tm1637_set_segment_fixed(lcd, lcd->segment_idx[0], 0xFF);
			tm1637_set_segment_fixed(lcd, lcd->segment_idx[1], 0xFF);
			tm1637_set_segment_fixed(lcd, lcd->segment_idx[2], 0xFF);
			tm1637_set_segment_fixed(lcd, lcd->segment_idx[3], 0xFF);
			tm1637_set_segment_fixed(lcd, lcd->segment_idx[4], 0xFF);
			tm1637_set_segment_fixed(lcd, lcd->segment_idx[5], 0xFF);
			vTaskDelay(30);
		}
		vTaskDelay(100);

		// Test display integer number
		tm1637_set_number(lcd, 1, true, 0x00); // 0001
		vTaskDelay(100);
		tm1637_set_number(lcd, 12, true, 0x00); // 0012
		vTaskDelay(100);
		tm1637_set_number(lcd, 123, true, 0x00); // 0123
		vTaskDelay(100);
		tm1637_set_number(lcd, 1234, true, 0x00); // 1234
		vTaskDelay(100);
		if (lcd->segment_max == 6) {
			tm1637_set_number(lcd, 12345, true, 0x00); // 12345
			vTaskDelay(100);
			tm1637_set_number(lcd, 123456, true, 0x00); // 123456
			vTaskDelay(100);
		}

		tm1637_set_number(lcd, 1, false, 0x00); // ____1
		vTaskDelay(100);
		tm1637_set_number(lcd, 12, false, 0x00); // ____12
		vTaskDelay(100);
		tm1637_set_number(lcd, 123, false, 0x00); // ___123
		vTaskDelay(100);
		tm1637_set_number(lcd, 1234, false, 0x00); // __1234
		vTaskDelay(100);
		if (lcd->segment_max == 6) {
			tm1637_set_number(lcd, 12345, false, 0x00); // _12345
			vTaskDelay(100);
			tm1637_set_number(lcd, 123456, false, 0x00); // 123456
			vTaskDelay(100);
		}

		tm1637_set_number(lcd, -1, true, 0x00); // -001
		vTaskDelay(100);
		tm1637_set_number(lcd, -12, true, 0x00); // -012
		vTaskDelay(100);
		tm1637_set_number(lcd, -123, true, 0x00); // -123
		vTaskDelay(100);
		if (lcd->segment_max == 6) {
			tm1637_set_number(lcd, -1234, true, 0x00); // -1234
			vTaskDelay(100);
			tm1637_set_number(lcd, -12345, true, 0x00); // -12345
			vTaskDelay(100);
		}

		tm1637_set_number(lcd, -1, false, 0x00); // ____-1
		vTaskDelay(100);
		tm1637_set_number(lcd, -12, false, 0x00); // ___-12
		vTaskDelay(100);
		tm1637_set_number(lcd, -123, false, 0x00); // __-123
		vTaskDelay(100);
		if (lcd->segment_max == 6) {
			tm1637_set_number(lcd, -1234, false, 0x00); // _-1234
			vTaskDelay(100);
			tm1637_set_number(lcd, -12345, false, 0x00); // -12345
			vTaskDelay(100);
		}

#if CONFIG_TM1637_DOT_SEGMENT
		uint16_t dot_position;
		// Test display floating number
		dot_position = 0x08;
		if (lcd->segment_max == 6) dot_position = 0x20;
		tm1637_set_number(lcd, 1, true, dot_position); // 0.001
		vTaskDelay(100);
		tm1637_set_number(lcd, 12, true, dot_position); // 0.012
		vTaskDelay(100);
		tm1637_set_number(lcd, 123, true, dot_position); // 0.123
		vTaskDelay(100);
		if (lcd->segment_max == 6) {
			tm1637_set_number(lcd, 1234, true, dot_position); // 0.1234
			vTaskDelay(100);
			tm1637_set_number(lcd, 12345, true, dot_position); // 0.12345
			vTaskDelay(100);
		}

		if (lcd->segment_max == 4) {
			tm1637_set_number(lcd, 1234, true, 0x08); // 1.234
			vTaskDelay(100);
			tm1637_set_number(lcd, 1234, true, 0x04); // 12.34
			vTaskDelay(100);
			tm1637_set_number(lcd, 1234, true, 0x02); // 123.4
			vTaskDelay(100);
			tm1637_set_number(lcd, 1234, true, 0x00); // 1234
			vTaskDelay(100);
		} else {
			tm1637_set_number(lcd, 123456, true, 0x20); // 1.23456
			vTaskDelay(100);
			tm1637_set_number(lcd, 123456, true, 0x10); // 12.3456
			vTaskDelay(100);
			tm1637_set_number(lcd, 123456, true, 0x08); // 123.456
			vTaskDelay(100);
			tm1637_set_number(lcd, 123456, true, 0x04); // 1234.56
			vTaskDelay(100);
			tm1637_set_number(lcd, 123456, true, 0x02); // 12345.6
			vTaskDelay(100);
			tm1637_set_number(lcd, 123456, true, 0x00); // 123456
			vTaskDelay(100);

		}

		if (lcd->segment_max == 4) {
			tm1637_set_number(lcd, -123, true, 0x08); // -.123
			vTaskDelay(100);
			tm1637_set_number(lcd, -123, false, 0x04); // -1.23
			vTaskDelay(100);
			tm1637_set_number(lcd, -123, false, 0x02); // -12.3
			vTaskDelay(100);
			tm1637_set_number(lcd, -123, false, 0x00); // -123
			vTaskDelay(100);
		} else {
			tm1637_set_number(lcd, -12345, true, 0x20); // -.12345
			vTaskDelay(100);
			tm1637_set_number(lcd, -12345, true, 0x10); // -1.2345
			vTaskDelay(100);
			tm1637_set_number(lcd, -12345, true, 0x08); // -12.345
			vTaskDelay(100);
			tm1637_set_number(lcd, -12345, false, 0x04); // -123.45
			vTaskDelay(100);
			tm1637_set_number(lcd, -12345, false, 0x02); // -1234.5
			vTaskDelay(100);
			tm1637_set_number(lcd, -12345, false, 0x00); // -12345
			vTaskDelay(100);
		}
#endif

		// Test display text
		if (lcd->segment_max == 4) {
			tm1637_set_segment_ascii(lcd, "PLAY");
		} else {
			tm1637_set_segment_ascii(lcd, " PLAY ");
		}
		vTaskDelay(100);
		tm1637_set_segment_ascii(lcd, "1234567890");
		vTaskDelay(100);
		tm1637_set_segment_ascii(lcd, "IP 192.168.10.20");
		vTaskDelay(100);
		if (lcd->segment_max == 4) {
			tm1637_set_segment_ascii(lcd, "STOP");
		} else {
			tm1637_set_segment_ascii(lcd, " STOP ");
		}
		vTaskDelay(100);

#if CONFIG_TM1637_CLOCK_SEGMENT
		tm1637_set_segment_ascii_with_time(lcd, "1234", 0x40, 1000);
#else
		if (lcd->segment_max == 4) {
			tm1637_set_segment_ascii_with_time(lcd, "1234", 0x00, 1000);
		} else {
			tm1637_set_segment_ascii_with_time(lcd, "123456", 0x00, 1000);
		}
#endif
		vTaskDelay(100);
	} // end while
}

void app_main()
{
	xTaskCreate(&tm1637_task, "tm1637_task", 1024*4, NULL, 5, NULL);
}

