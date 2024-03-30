/**
 * ESP-32 IDF library for control TM1637 LED 7-Segment display
 *
 * Author: Petro <petro@petro.ws>
 *
 * Project homepage: https://github.com/petrows/esp-32-tm1637
 * Example: https://github.com/petrows/esp-32-tm1637-example
 *
 * License: MIT (see LICENSE file included)
 *
 */

#ifndef TM1637_H
#define TM1637_H

#include <inttypes.h>
#include <stdbool.h>
#include <driver/gpio.h>

#ifdef __cplusplus
extern "C" {
#endif

struct tm;

typedef struct {
	int segment_idx[6];
	int segment_start;
	int segment_max;
	gpio_num_t m_pin_clk;
	gpio_num_t m_pin_dta;
	uint8_t m_brightness;
} tm1637_led_t;

/**
 * @brief Constructs new LED TM1637 object
 *
 * @param pin_clk GPIO pin for CLK input of LED module
 * @param pin_data GPIO pin for DIO input of LED module
 * @return
 */
tm1637_led_t * tm1637_init(gpio_num_t pin_clk, gpio_num_t pin_data);

/**
 * @brief Set brightness level. Note - will be set after next display render
 * @param led LED object
 * @param level Brightness level 0..7 value
 */
void tm1637_set_brightness(tm1637_led_t * led, uint8_t level);

/**
 * @brief Set ascii string
 * @param led LED object
 * @param text ascii string
 */
void tm1637_set_segment_ascii(tm1637_led_t * led, char *text);

/**
 * @brief Set ascii string
 * @param led LED object
 * @param text ascii string
 * @param dot_position dot position
 * @param time display time[ms]
 */
void tm1637_set_segment_ascii_with_time(tm1637_led_t * led, char * text, const uint16_t dot_position, int time);

/**
 * @brief Set one-segment with Fix addressr mode
 * @param led LED object
 * @param segment_idx Segment index (0..3)
 * @param data Raw data, bitmask is XGFEDCBA
 */
void tm1637_set_segment_fixed(tm1637_led_t * led, const int8_t segment_idx, const uint8_t data);

/**
 * @brief Set segments with Automatic address adding mode
 * @param led LED object
 * @param data Raw datas
 * @param data_length Raw data length
 */
void tm1637_set_segment_auto(tm1637_led_t * led, const uint8_t *data, const int data_length);

/**
 * @brief Set one-segment number, also controls dot of this segment
 * @param led LED object
 * @param segment_idx Segment index (0..3)
 * @param num Number to set (0x00..0x0F, 0xFF for clear)
 * @param dot Display dot of this segment
 */
void tm1637_set_segment_number(tm1637_led_t * led, const int8_t segment_idx, const uint8_t num, const bool dot);

/**
 * @brief Set full display number, in decimal encoding
 * @param led LED object
 * @param number Display number (-99999...999999)
 * @param lead_zero Leading Zero or Leading Space
 * @param dot_position dot position
 */
void tm1637_set_number(tm1637_led_t * led, int32_t number, bool lead_zero, const uint16_t dot_position);


#ifdef __cplusplus
}
#endif

#endif // TM1637_H
