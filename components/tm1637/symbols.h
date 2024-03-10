/**
 * ESP-32 IDF library for control TM1637 LED 7-Segment display
 *
 * Author: Petro <petro@petro.ws>
 *
 * Project homepage: https://github.com/petrows/esp-32-tm1637
 * Example: https://github.com/petrows/esp-32-tm1637-example
 *
 */

#define MINUS_SIGN_IDX 10

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

#define ZERO  0
#define MINUS 10
#define SPACE 11

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
