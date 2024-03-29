/**
 * ESP-32 IDF library for control TM1637 LED 7-Segment display
 *
 * Author: Petro <petro@petro.ws>
 *
 * Project homepage: https://github.com/petrows/esp-32-tm1637
 * Example: https://github.com/petrows/esp-32-tm1637-example
 *
 */

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <esp32/rom/ets_sys.h>

#include "tm1637.h"
#include "symbols.h"

#define TM1637_ADDR_AUTO  0x40
#define TM1637_ADDR_FIXED 0x44
#define TM1637_AUTO_DELAY 300000

static void tm1637_start(tm1637_led_t * led);
static void tm1637_stop(tm1637_led_t * led);
static void tm1637_send_byte(tm1637_led_t * led, uint8_t byte);
static void tm1637_delay();

static inline float nearestf(float val,int precision) {
	int scale = pow(10,precision);
	return roundf(val * scale) / scale;
}

void tm1637_start(tm1637_led_t * led)
{
	// Send start signal
	// Both outputs are expected to be HIGH beforehand
	gpio_set_level(led->m_pin_dta, 0);
	tm1637_delay();
}

void tm1637_stop(tm1637_led_t * led)
{
	// Send stop signal
	// CLK is expected to be LOW beforehand
	gpio_set_level(led->m_pin_dta, 0);
	tm1637_delay();
	gpio_set_level(led->m_pin_clk, 1);
	tm1637_delay();
	gpio_set_level(led->m_pin_dta, 1);
	tm1637_delay();
}

void tm1637_send_byte(tm1637_led_t * led, uint8_t byte)
{
	for (uint8_t i=0; i<8; ++i)
	{
		gpio_set_level(led->m_pin_clk, 0);
		tm1637_delay();
		gpio_set_level(led->m_pin_dta, byte & 0x01); // Send current bit
		byte >>= 1;
		tm1637_delay();
		gpio_set_level(led->m_pin_clk, 1);
		tm1637_delay();
	}

	// The TM1637 signals an ACK by pulling DIO low from the falling edge of
	// CLK after sending the 8th bit, to the next falling edge of CLK.
	// DIO needs to be set as input during this time to avoid having both
	// chips trying to drive DIO at the same time.
	gpio_set_direction(led->m_pin_dta, GPIO_MODE_INPUT);
	gpio_set_level(led->m_pin_clk, 0); // TM1637 starts ACK (pulls DIO low)
	tm1637_delay();
	gpio_set_level(led->m_pin_clk, 1);
	tm1637_delay();
	gpio_set_level(led->m_pin_clk, 0); // TM1637 ends ACK (releasing DIO)
	tm1637_delay();
	gpio_set_direction(led->m_pin_dta, GPIO_MODE_OUTPUT);
}

void tm1637_delay()
{
	//ets_delay_us(3);
	ets_delay_us(1);
}

// PUBLIC PART:

tm1637_led_t * tm1637_init(gpio_num_t pin_clk, gpio_num_t pin_data) {
	tm1637_led_t * led = (tm1637_led_t *) malloc(sizeof(tm1637_led_t));
	led->m_pin_clk = pin_clk;
	led->m_pin_dta = pin_data;
	//led->m_brightness = 0x07;
	led->m_brightness = CONFIG_TM1637_BRIGHTNESS;;
	// Set CLK to low during DIO initialization to avoid sending a start signal by mistake
	gpio_set_direction(pin_clk, GPIO_MODE_OUTPUT);
	gpio_set_level(pin_clk, 0);
	tm1637_delay();
	gpio_set_direction(pin_data, GPIO_MODE_OUTPUT);
	gpio_set_level(pin_data, 1);
	tm1637_delay();
	gpio_set_level(pin_clk, 1);
	tm1637_delay();
	return led;
}

void tm1637_set_brightness(tm1637_led_t * led, uint8_t level)
{
	if (level > 0x07) { level = 0x07; } // Check max level
	led->m_brightness = level;
}

void tm1637_set_segment_ascii(tm1637_led_t * led, char * text)
{
	if (strlen(text) <= 4) {
		// show fix segment
		for (int segment_idx=0;segment_idx<strlen(text);segment_idx++) {
			int c = text[segment_idx];
			uint8_t seg_data = ascii_symbols[c];
			// printf("text[%d]=%d seg_data=0x%x\n", segment_idx, c, seg_data);
			tm1637_set_segment_fixed(led, segment_idx, seg_data);
		}
	} else {
		// show sliding segment
		uint8_t segments[4] = {0,0,0,0};

		for (int i=0;i<strlen(text);i++) {
			segments[0] = segments[1];
			segments[1] = segments[2];
			segments[2] = segments[3];
			int c = text[i];
			segments[3] = ascii_symbols[c];
			tm1637_set_segment_auto(led, segments, 4);
			ets_delay_us(TM1637_AUTO_DELAY);
		}
		for (int i=0;i<4;i++) {
			segments[0] = segments[1];
			segments[1] = segments[2];
			segments[2] = segments[3];
			segments[3] = 0;
			tm1637_set_segment_auto(led, segments, 4);
		}
	}
}

void tm1637_set_segment_ascii_with_time(tm1637_led_t * led, char * text, const uint8_t dot_position, int time)
{
	char _text[5];
	int _text_length = strlen(text);
	memset(_text, 0, sizeof(_text));
	if (_text_length == 4) {
		strcpy(_text, text);
	} else if (_text_length < 4) {
		memcpy(&_text[4-_text_length], text, _text_length);
	} else {
		memcpy(_text, text, 4);
	}

	for (int segment_idx=0; segment_idx<4; segment_idx++) {
		tm1637_set_segment_fixed(led, segment_idx, 0);
	}

	uint8_t dot_mask = 0x01;
	for (int segment_idx=3; segment_idx>=0; segment_idx--) {
		int c = text[segment_idx];
		uint8_t seg_data = ascii_symbols[c];
		// Find the lower half segment(segment=c/d/e/g)
		seg_data = seg_data & 0x5c; // 0b0101-1100
		tm1637_set_segment_fixed(led, segment_idx, seg_data);
		ets_delay_us(TM1637_AUTO_DELAY);
		seg_data = ascii_symbols[c];
		//printf("seg_data=0x%x dot_position=0x%x dot_mask=0x%x\n", seg_data, dot_position, dot_mask);
		if (dot_position & dot_mask) seg_data |= 0x80; // Set DOT segment flag
		//printf("seg_data=0x%x\n", seg_data);
		dot_mask = dot_mask << 1;
		tm1637_set_segment_fixed(led, segment_idx, seg_data);
		ets_delay_us(TM1637_AUTO_DELAY);
	}

	ets_delay_us(time*1000);
	for (int segment_idx=3; segment_idx>=0; segment_idx--) {
		int c = text[segment_idx];
		uint8_t seg_data = ascii_symbols[c];
		// Find the upper half segment(segment=a/b/f/g)
		seg_data = seg_data & 0x63; // 0b0110-0011
		tm1637_set_segment_fixed(led, segment_idx, seg_data);
		ets_delay_us(TM1637_AUTO_DELAY);
		tm1637_set_segment_fixed(led, segment_idx, 0);
		ets_delay_us(TM1637_AUTO_DELAY);
	}
}

// Fix address mode
// Display on specific addresses
// [Set data][Set address][Display data][Control display]
void tm1637_set_segment_fixed(tm1637_led_t * led, const uint8_t segment_idx, const uint8_t data)
{
	tm1637_start(led);
	tm1637_send_byte(led, TM1637_ADDR_FIXED);
	tm1637_stop(led);
	tm1637_start(led);
	tm1637_send_byte(led, segment_idx | 0xc0);
	tm1637_send_byte(led, data);
	tm1637_stop(led);
	tm1637_start(led);
	tm1637_send_byte(led, led->m_brightness | 0x88);
	tm1637_stop(led);
}

// Automatic address adding mode
// Display on consecutive addresses
// [Set data][Set address][Display data1][Display data2][Display data3][Display data4][Control display]
void tm1637_set_segment_auto(tm1637_led_t * led, const uint8_t *data, const int data_length)
{
	tm1637_start(led);
	tm1637_send_byte(led, TM1637_ADDR_AUTO);
	tm1637_stop(led);
	tm1637_start(led);
	tm1637_send_byte(led, 0xc0);
	for (int i=0;i<data_length;i++) {
		tm1637_send_byte(led, data[i]);
	}
	tm1637_stop(led);
	tm1637_start(led);
	tm1637_send_byte(led, led->m_brightness | 0x88);
	tm1637_stop(led);
}

void tm1637_set_segment_number(tm1637_led_t * led, const uint8_t segment_idx, const uint8_t num, const bool dot)
{
	uint8_t seg_data = 0x00;

	if (num < (sizeof(numerical_symbols)/sizeof(numerical_symbols[0]))) {
		seg_data = numerical_symbols[num]; // Select proper segment image
	}

	if (dot) {
		seg_data |= 0x80; // Set DOT segment flag
	}

	tm1637_set_segment_fixed(led, segment_idx, seg_data);
}

void tm1637_set_number(tm1637_led_t * led, int16_t number, bool lead_zero, const uint8_t dot_position)
{
	int16_t _number = abs(number);

	if (number >= 0) {
		uint8_t lead_number = SPACE;
		if (lead_zero) lead_number = ZERO;
		if (_number < 10) {
			tm1637_set_segment_number(led, 3, number, dot_position & 0x01);
			tm1637_set_segment_number(led, 2, lead_number, dot_position & 0x02);
			tm1637_set_segment_number(led, 1, lead_number, dot_position & 0x04);
			tm1637_set_segment_number(led, 0, lead_number, dot_position & 0x08);
		} else if (_number < 100) {
			tm1637_set_segment_number(led, 3, number % 10, dot_position & 0x01);
			tm1637_set_segment_number(led, 2, (number / 10) % 10, dot_position & 0x02);
			tm1637_set_segment_number(led, 1, lead_number, dot_position & 0x04);
			tm1637_set_segment_number(led, 0, lead_number, dot_position & 0x08);
		} else if (_number < 1000) {
			tm1637_set_segment_number(led, 3, number % 10, dot_position & 0x01);
			tm1637_set_segment_number(led, 2, (number / 10) % 10, dot_position & 0x02);
			tm1637_set_segment_number(led, 1, (number / 100) % 10, dot_position & 0x04);
			tm1637_set_segment_number(led, 0, lead_number, dot_position & 0x08);
		} else {
			tm1637_set_segment_number(led, 3, number % 10, dot_position & 0x01);
			tm1637_set_segment_number(led, 2, (number / 10) % 10, dot_position & 0x02);
			tm1637_set_segment_number(led, 1, (number / 100) % 10, dot_position & 0x04);
			tm1637_set_segment_number(led, 0, (number / 1000) % 10, dot_position & 0x08);
		}
	} else {
		if (_number < 10) {
			if (lead_zero) {
				tm1637_set_segment_number(led, 3, _number, dot_position & 0x01);
				tm1637_set_segment_number(led, 2, ZERO, dot_position & 0x02);
				tm1637_set_segment_number(led, 1, ZERO, dot_position & 0x04);
				tm1637_set_segment_number(led, 0, MINUS, dot_position & 0x08);
			} else {
				tm1637_set_segment_number(led, 3, _number, dot_position & 0x01);
				tm1637_set_segment_number(led, 2, MINUS, dot_position & 0x02);
				tm1637_set_segment_number(led, 1, SPACE, dot_position & 0x04);
				tm1637_set_segment_number(led, 0, SPACE, dot_position & 0x08);
			}
		} else if (_number < 100) {
			if (lead_zero) {
				tm1637_set_segment_number(led, 3, _number % 10, dot_position & 0x01);
				tm1637_set_segment_number(led, 2, (_number / 10) % 10, dot_position & 0x02);
				tm1637_set_segment_number(led, 1, ZERO, dot_position & 0x04);
				tm1637_set_segment_number(led, 0, MINUS, dot_position & 0x08);
			} else {
				tm1637_set_segment_number(led, 3, _number % 10, dot_position & 0x01);
				tm1637_set_segment_number(led, 2, (_number / 10) % 10, dot_position & 0x02);
				tm1637_set_segment_number(led, 1, MINUS, dot_position & 0x04);
				tm1637_set_segment_number(led, 0, SPACE, dot_position & 0x08);
			}
		} else {
			tm1637_set_segment_number(led, 3, _number % 10, dot_position & 0x01);
			tm1637_set_segment_number(led, 2, (_number / 10) % 10, dot_position & 0x02);
			tm1637_set_segment_number(led, 1, (_number / 100) % 10, dot_position & 0x04);
			tm1637_set_segment_number(led, 0, MINUS, dot_position & 0x08);
		}
	}
}
