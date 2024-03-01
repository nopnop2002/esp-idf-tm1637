/**
 * ESP-32 IDF library for control TM1637 LED 7-Segment display
 *
 * Author: Petro <petro@petro.ws>
 *
 * Project homepage: https://github.com/petrows/esp-32-tm1637
 * Example: https://github.com/petrows/esp-32-tm1637-example
 *
 */

#include "tm1637.h"

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <esp32/rom/ets_sys.h>

#define TM1637_ADDR_AUTO  0x40
#define TM1637_ADDR_FIXED 0x44
#define TM1637_AUTO_DELAY 300000

#define MINUS_SIGN_IDX  16

/*
Segment position
    a
   ---
 f| g |b
   ---
 e|   |c
   ---
    d
*/

static const int8_t numerical_symbols[] = {
                // XGFEDCBA
        0x3f, // 0b00111111,    // 0
        0x06, // 0b00000110,    // 1
        0x5b, // 0b01011011,    // 2
        0x4f, // 0b01001111,    // 3
        0x66, // 0b01100110,    // 4
        0x6d, // 0b01101101,    // 5
        0x7d, // 0b01111101,    // 6
        0x07, // 0b00000111,    // 7
        0x7f, // 0b01111111,    // 8
        0x6f, // 0b01101111,    // 9
        0x40, // 0b01000000,    // minus sign
        0x00, // 0b00000000     // space
};

static const int8_t ascii_symbols[] = {
        //NUL   SOH   STX   ETX   EOT   ENQ   ACK   BEL   BS    HT    LF    VT
        0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
        //FF    CR    SO    SI    DLE   DC1   DC2   DC3   DC4   NAK   SYN   ETB
        0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
        //CAN   EM    SUB   ESC   FS    GS    RS    US    space !     "     #
        0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0x22, 0,
        //$     %     &     '     (     )     *     +     ,     -     .     /
        0,    0,    0,    0x01, 0,    0,    0,    0,    0x08, 0x40, 0x08, 0x52,
        //0     1     2     3     4     5     6     7     8     9     :     ;
        0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F, 0,    0,
        //<     =     >     ?     @     A     B     C     D     E     F     G
        0,    0x48, 0,    0,    0,    0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71, 0x3D,
        //H     I     J     K     L     M     N     O     P     Q     R     S
        0x76, 0x30, 0x1E, 0x75, 0x38, 0x55, 0x54, 0x5C, 0x73, 0x67, 0x50, 0x6D,
        //T     U     V     W     X     Y     Z     [     \     ]     ^     _
        0x78, 0x3E, 0x1C, 0x1D, 0x64, 0x6E, 0x5B, 0,    0x64, 0,    0,    0x08,
        //`     a     b     c     d     e     f     g     h     i     j     k
        0,    0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71, 0x3D, 0x76, 0x30, 0x1E, 0x75,
        //l     m     n     o     p     q     r     s     t     u     v     w
        0x38, 0x55, 0x54, 0x5C, 0x73, 0x67, 0x50, 0x6D, 0x78, 0x3E, 0x1C, 0x1D,
        //x     y     z     {     |     }     ~     DEL
        0x64, 0x6E, 0x5B, 0,    0,    0,    0,    0
};

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
    ets_delay_us(3);
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

void tm1637_set_segment_ascii_with_time(tm1637_led_t * led, char * text, int time)
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

	for (int segment_idx=3; segment_idx>=0; segment_idx--) {
		int c = text[segment_idx];
		uint8_t seg_data = ascii_symbols[c];
		// Find the lower half segment(segment=c/d/e/g)
		seg_data = seg_data & 0x5c; // 0b0101-1100
   		tm1637_set_segment_fixed(led, segment_idx, seg_data);
    	ets_delay_us(TM1637_AUTO_DELAY);
		seg_data = ascii_symbols[c];
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

void tm1637_set_number(tm1637_led_t * led, uint16_t number)
{
    tm1637_set_number_lead_dot(led, number, false, 0x00);
}

void tm1637_set_number_lead(tm1637_led_t * led, uint16_t number, const bool lead_zero)
{
    tm1637_set_number_lead_dot(led, number, lead_zero, 0x00);
}

void tm1637_set_number_lead_dot(tm1637_led_t * led, uint16_t number, bool lead_zero, const uint8_t dot_mask)
{
    uint8_t lead_number = lead_zero ? 0xFF : numerical_symbols[0];

    if (number < 10) {
        tm1637_set_segment_number(led, 3, number, dot_mask & 0x01);
        tm1637_set_segment_number(led, 2, lead_number, dot_mask & 0x02);
        tm1637_set_segment_number(led, 1, lead_number, dot_mask & 0x04);
        tm1637_set_segment_number(led, 0, lead_number, dot_mask & 0x08);
    } else if (number < 100) {
        tm1637_set_segment_number(led, 3, number % 10, dot_mask & 0x01);
        tm1637_set_segment_number(led, 2, (number / 10) % 10, dot_mask & 0x02);
        tm1637_set_segment_number(led, 1, lead_number, dot_mask & 0x04);
        tm1637_set_segment_number(led, 0, lead_number, dot_mask & 0x08);
    } else if (number < 1000) {
        tm1637_set_segment_number(led, 3, number % 10, dot_mask & 0x01);
        tm1637_set_segment_number(led, 2, (number / 10) % 10, dot_mask & 0x02);
        tm1637_set_segment_number(led, 1, (number / 100) % 10, dot_mask & 0x04);
        tm1637_set_segment_number(led, 0, lead_number, dot_mask & 0x08);
    } else {
        tm1637_set_segment_number(led, 3, number % 10, dot_mask & 0x01);
        tm1637_set_segment_number(led, 2, (number / 10) % 10, dot_mask & 0x02);
        tm1637_set_segment_number(led, 1, (number / 100) % 10, dot_mask & 0x04);
        tm1637_set_segment_number(led, 0, (number / 1000) % 10, dot_mask & 0x08);
    }
}

void tm1637_set_float(tm1637_led_t * led, float n) {
    if( n < 0 ) {
        tm1637_set_segment_number(led, 0, MINUS_SIGN_IDX, 0);
        float absn = nearestf(fabs(n),1);
        int int_part = (int)absn;
        float fx_part = absn - int_part;
        if( absn < 10 ) {
            fx_part *= 100;
            tm1637_set_segment_number(led, 1, (int)(absn + 0.5), 1 );
            tm1637_set_segment_number(led, 2, ((int)fx_part/10) % 10, 0 );
            tm1637_set_segment_number(led, 3, ((int)fx_part) % 10, 0 );
        }
        else if( n < 100 ) {
            fx_part *= 100;
            uint8_t f = ((int)fx_part % 10);
            
            tm1637_set_segment_number(led, 1, (int_part/10) % 10, 0 );
            tm1637_set_segment_number(led, 2, int_part % 10, 1 );
            tm1637_set_segment_number(led, 3, ((int)fx_part/10) % 10 + ((f > 4)?1:0), 0 );
        }
        else if( n < 1000 ) {
            tm1637_set_segment_number(led, 1, (int_part/100) % 10, 0 );
            tm1637_set_segment_number(led, 2, (int_part/10) % 10, 0 );
            tm1637_set_segment_number(led, 3, (int_part % 10) + ((fx_part >= 0.5 )?1:0), 0 );
        }
    }
    else {
        //  positive number
        int int_part = (int)n;
        float fx_part = n - int_part;
        if( n < 10 ) {
            n = nearestf(n,1);
            int_part = (int)n;
            fx_part = 10000 * (n - int_part);
            
            tm1637_set_segment_number(led, 0, int_part, 1);
            tm1637_set_segment_number(led, 1, ((int)fx_part/1000) % 10, 0 );
            tm1637_set_segment_number(led, 2, ((int)fx_part/100) % 10, 0 );
            tm1637_set_segment_number(led, 3, ((int)fx_part/10) % 10, 0 );
        }
        else if( n < 100 ) {
            n = nearestf(n,2);
            int_part = (int)n;
            fx_part = 1000 * (n - int_part);
            
            tm1637_set_segment_number(led, 0, (int_part/10) % 10, 0);
            tm1637_set_segment_number(led, 1, int_part % 10, 1 );
            tm1637_set_segment_number(led, 2, ((int)fx_part/100) % 10, 0 );
            tm1637_set_segment_number(led, 3, ((int)fx_part/10) % 10,0);
        }
        else if( n < 1000 ) {
            n = nearestf(n,2);
            int_part = (int)n;
            fx_part = 100 * (n - int_part);
            
            tm1637_set_segment_number(led, 0, (int_part/100) % 10, 0);
            tm1637_set_segment_number(led, 1, (int_part/10) % 10, 0 );
            tm1637_set_segment_number(led, 2, int_part % 10, 1 );
            tm1637_set_segment_number(led, 3, ((int)fx_part/10) % 10, 0 );
        }
    }
}
