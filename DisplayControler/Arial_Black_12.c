/*******************************************************************************
 * Size: 12 px
 * Bpp: 1
 * Opts: --bpp 1 --size 12 --no-compress --stride 1 --align 1 --font ariblk.ttf --symbols ° --range 32-128 --format lvgl -o Arial_Black_12.c
 ******************************************************************************/

#ifdef __has_include
    #if __has_include("lvgl.h")
        #ifndef LV_LVGL_H_INCLUDE_SIMPLE
            #define LV_LVGL_H_INCLUDE_SIMPLE
        #endif
    #endif
#endif

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
    #include "lvgl.h"
#else
    #include "lvgl/lvgl.h"
#endif



#ifndef ARIAL_BLACK_12
#define ARIAL_BLACK_12 1
#endif

#if ARIAL_BLACK_12

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t glyph_bitmap[] = {
    /* U+0020 " " */
    0x0,

    /* U+0021 "!" */
    0xff, 0xc3,

    /* U+0022 "\"" */
    0xde, 0xf6,

    /* U+0023 "#" */
    0x36, 0x4b, 0xfb, 0x66, 0xdf, 0xd2, 0x6c,

    /* U+0024 "$" */
    0x10, 0xfb, 0xff, 0x8f, 0xc7, 0xd5, 0xff, 0x7c,
    0x20,

    /* U+0025 "%" */
    0x71, 0x1b, 0x23, 0x68, 0x3b, 0x0, 0x5c, 0x1e,
    0xc2, 0xd8, 0xdb, 0x11, 0xc0,

    /* U+0026 "&" */
    0x3c, 0x33, 0x1b, 0x87, 0x8f, 0xd6, 0x7b, 0x1e,
    0xff,

    /* U+0027 "'" */
    0xfc,

    /* U+0028 "(" */
    0x2f, 0x6d, 0xb6, 0x64,

    /* U+0029 ")" */
    0x99, 0xb6, 0xdb, 0xd0,

    /* U+002A "*" */
    0x27, 0x9c, 0xa0,

    /* U+002B "+" */
    0x30, 0xcf, 0xcc, 0x30, 0xc0,

    /* U+002C "," */
    0xdc,

    /* U+002D "-" */
    0xe0,

    /* U+002E "." */
    0xc0,

    /* U+002F "/" */
    0x25, 0xa4, 0xb4,

    /* U+0030 "0" */
    0x7d, 0x9f, 0x1e, 0x3c, 0x78, 0xfb, 0xbe,

    /* U+0031 "1" */
    0x19, 0xff, 0xb1, 0x8c, 0x63,

    /* U+0032 "2" */
    0x7c, 0xcc, 0x18, 0x71, 0xcf, 0x1c, 0x7f,

    /* U+0033 "3" */
    0x79, 0x98, 0x30, 0xe0, 0x60, 0xf9, 0xbe,

    /* U+0034 "4" */
    0x6, 0xe, 0x1e, 0x36, 0x66, 0xff, 0x6, 0x6,

    /* U+0035 "5" */
    0x7e, 0xc1, 0x83, 0xe2, 0x60, 0xf9, 0xbe,

    /* U+0036 "6" */
    0x3c, 0xdf, 0x7, 0xec, 0x78, 0xd1, 0x9e,

    /* U+0037 "7" */
    0xfe, 0x18, 0x70, 0xc3, 0x86, 0xc, 0x38,

    /* U+0038 "8" */
    0x7d, 0x8f, 0x1b, 0xec, 0x78, 0xf1, 0xbe,

    /* U+0039 "9" */
    0x79, 0x8b, 0x1e, 0x37, 0xe0, 0xfb, 0x3c,

    /* U+003A ":" */
    0xc0, 0x30,

    /* U+003B ";" */
    0xc0, 0x37,

    /* U+003C "<" */
    0xe, 0x7b, 0xc7, 0x83, 0xe1, 0xc0,

    /* U+003D "=" */
    0xfc, 0x0, 0x3f,

    /* U+003E ">" */
    0xc1, 0xf0, 0x79, 0xff, 0x98, 0x0,

    /* U+003F "?" */
    0x79, 0xd8, 0x30, 0xe3, 0x86, 0x0, 0x18,

    /* U+0040 "@" */
    0x1e, 0x30, 0x96, 0xb6, 0xda, 0x4d, 0x26, 0x94,
    0xb4, 0x63, 0x9f, 0x0,

    /* U+0041 "A" */
    0x1c, 0xf, 0xf, 0x86, 0xe7, 0x73, 0xf9, 0x8f,
    0xc7,

    /* U+0042 "B" */
    0xfc, 0xc6, 0xc6, 0xfe, 0xc3, 0xc3, 0xc3, 0xfe,

    /* U+0043 "C" */
    0x3c, 0x66, 0xc0, 0xc0, 0xc0, 0xc6, 0x66, 0x3c,

    /* U+0044 "D" */
    0xfc, 0xc6, 0xc3, 0xc3, 0xc3, 0xc3, 0xc6, 0xfc,

    /* U+0045 "E" */
    0xff, 0x83, 0x7, 0xfc, 0x18, 0x30, 0x7f,

    /* U+0046 "F" */
    0xff, 0x83, 0x7, 0xec, 0x18, 0x30, 0x60,

    /* U+0047 "G" */
    0x3f, 0x31, 0xf0, 0x18, 0xc, 0xfe, 0x1d, 0x8e,
    0x7e,

    /* U+0048 "H" */
    0xc3, 0xc3, 0xc3, 0xff, 0xc3, 0xc3, 0xc3, 0xc3,

    /* U+0049 "I" */
    0xff, 0xff,

    /* U+004A "J" */
    0xc, 0x30, 0xc3, 0xc, 0x3e, 0xde,

    /* U+004B "K" */
    0xcf, 0xde, 0xfc, 0xfc, 0xfc, 0xee, 0xce, 0xc7,

    /* U+004C "L" */
    0xc1, 0x83, 0x6, 0xc, 0x18, 0x30, 0x7f,

    /* U+004D "M" */
    0xf3, 0xfc, 0xff, 0x3f, 0xff, 0xde, 0xf7, 0xbd,
    0xef, 0x33,

    /* U+004E "N" */
    0xc3, 0xe3, 0xf3, 0xfb, 0xdf, 0xcf, 0xc7, 0xc3,

    /* U+004F "O" */
    0x3e, 0x31, 0xb0, 0x78, 0x3c, 0x1e, 0xd, 0x8c,
    0x7c,

    /* U+0050 "P" */
    0xfd, 0x8f, 0x1e, 0x3f, 0xd8, 0x30, 0x60,

    /* U+0051 "Q" */
    0x3e, 0x31, 0xb0, 0x78, 0x3c, 0x1e, 0x6d, 0xbc,
    0x7f, 0x0, 0x80,

    /* U+0052 "R" */
    0xfc, 0xc6, 0xc6, 0xfc, 0xdc, 0xce, 0xce, 0xc7,

    /* U+0053 "S" */
    0x7d, 0xcf, 0xc7, 0xe7, 0xe0, 0xf9, 0xbe,

    /* U+0054 "T" */
    0xff, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,

    /* U+0055 "U" */
    0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0x7e,

    /* U+0056 "V" */
    0xe3, 0xb1, 0xdc, 0xce, 0xe3, 0x71, 0xf0, 0x78,
    0x38,

    /* U+0057 "W" */
    0xe6, 0x7e, 0xf7, 0x6f, 0x66, 0xf6, 0x7f, 0xe7,
    0x9e, 0x39, 0xc3, 0x9c,

    /* U+0058 "X" */
    0x73, 0xbb, 0x8f, 0x83, 0xc3, 0xe1, 0xf9, 0xdd,
    0xc7,

    /* U+0059 "Y" */
    0xe7, 0x66, 0x7e, 0x3c, 0x18, 0x18, 0x18, 0x18,

    /* U+005A "Z" */
    0xff, 0xf, 0x1e, 0x1c, 0x38, 0x70, 0xe0, 0xff,

    /* U+005B "[" */
    0xfc, 0xcc, 0xcc, 0xcc, 0xcf,

    /* U+005C "\\" */
    0x99, 0x24, 0xc9,

    /* U+005D "]" */
    0xf3, 0x33, 0x33, 0x33, 0x3f,

    /* U+005E "^" */
    0x30, 0xc7, 0x96, 0xcc,

    /* U+005F "_" */
    0xfc,

    /* U+0060 "`" */
    0x60,

    /* U+0061 "a" */
    0x7c, 0xcc, 0xfb, 0xb6, 0x6f, 0xc0,

    /* U+0062 "b" */
    0xc1, 0x83, 0xf6, 0x3c, 0x78, 0xf1, 0xfe,

    /* U+0063 "c" */
    0x7d, 0x9f, 0x6, 0xc, 0xef, 0x80,

    /* U+0064 "d" */
    0x6, 0xd, 0xfe, 0x3c, 0x78, 0xf1, 0xbf,

    /* U+0065 "e" */
    0x7d, 0xdf, 0xfe, 0xe, 0xef, 0x80,

    /* U+0066 "f" */
    0x73, 0x3c, 0xc6, 0x31, 0x8c,

    /* U+0067 "g" */
    0x7f, 0x3c, 0xf3, 0xcd, 0xfc, 0xfe,

    /* U+0068 "h" */
    0xc3, 0xf, 0xbf, 0xcf, 0x3c, 0xf3,

    /* U+0069 "i" */
    0xcf, 0xff,

    /* U+006A "j" */
    0x61, 0xb6, 0xdb, 0x7c,

    /* U+006B "k" */
    0xc1, 0x83, 0x77, 0xcf, 0x9f, 0x37, 0x67,

    /* U+006C "l" */
    0xff, 0xff,

    /* U+006D "m" */
    0xff, 0xbf, 0xfc, 0xcf, 0x33, 0xcc, 0xf3, 0x30,

    /* U+006E "n" */
    0xfb, 0xfc, 0xf3, 0xcf, 0x30,

    /* U+006F "o" */
    0x7d, 0x8f, 0x1e, 0x3c, 0x6f, 0x80,

    /* U+0070 "p" */
    0xfd, 0x8f, 0x1e, 0x3c, 0x7f, 0xb0, 0x60,

    /* U+0071 "q" */
    0x7f, 0x8f, 0x1e, 0x3c, 0x6f, 0xc1, 0x83,

    /* U+0072 "r" */
    0xf6, 0x31, 0x8c, 0x60,

    /* U+0073 "s" */
    0x7d, 0x93, 0xf3, 0xec, 0xcf, 0x0,

    /* U+0074 "t" */
    0x23, 0x3e, 0xc6, 0x31, 0x87,

    /* U+0075 "u" */
    0xcf, 0x3c, 0xf3, 0xfd, 0xf0,

    /* U+0076 "v" */
    0xe6, 0xdd, 0xb1, 0xe3, 0x87, 0x0,

    /* U+0077 "w" */
    0xce, 0x6d, 0xcd, 0xbf, 0x3d, 0xe3, 0xbc, 0x73,
    0x0,

    /* U+0078 "x" */
    0x66, 0x7e, 0x3c, 0x3c, 0x7e, 0xe7,

    /* U+0079 "y" */
    0xe6, 0xcd, 0xb1, 0xe3, 0xc7, 0xe, 0x78,

    /* U+007A "z" */
    0xfc, 0x73, 0x9c, 0xe3, 0xf0,

    /* U+007B "{" */
    0x76, 0x66, 0xc6, 0x66, 0x63,

    /* U+007C "|" */
    0xff, 0xff, 0xf0,

    /* U+007D "}" */
    0xe3, 0x18, 0xc3, 0xb1, 0x8c, 0x66, 0x0,

    /* U+007E "~" */
    0x73, 0x38,

    /* U+00B0 "°" */
    0xeb, 0xe0
};


/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 64, .box_w = 1, .box_h = 1, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1, .adv_w = 64, .box_w = 2, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 3, .adv_w = 96, .box_w = 5, .box_h = 3, .ofs_x = 1, .ofs_y = 5},
    {.bitmap_index = 5, .adv_w = 127, .box_w = 7, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 12, .adv_w = 128, .box_w = 7, .box_h = 10, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 21, .adv_w = 192, .box_w = 11, .box_h = 9, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 34, .adv_w = 171, .box_w = 9, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 43, .adv_w = 53, .box_w = 2, .box_h = 3, .ofs_x = 1, .ofs_y = 5},
    {.bitmap_index = 44, .adv_w = 75, .box_w = 3, .box_h = 10, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 48, .adv_w = 75, .box_w = 3, .box_h = 10, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 52, .adv_w = 107, .box_w = 5, .box_h = 4, .ofs_x = 1, .ofs_y = 4},
    {.bitmap_index = 55, .adv_w = 127, .box_w = 6, .box_h = 6, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 60, .adv_w = 64, .box_w = 2, .box_h = 3, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 61, .adv_w = 64, .box_w = 3, .box_h = 1, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 62, .adv_w = 64, .box_w = 2, .box_h = 1, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 63, .adv_w = 53, .box_w = 3, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 66, .adv_w = 128, .box_w = 7, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 73, .adv_w = 128, .box_w = 5, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 78, .adv_w = 128, .box_w = 7, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 85, .adv_w = 128, .box_w = 7, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 92, .adv_w = 128, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 100, .adv_w = 128, .box_w = 7, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 107, .adv_w = 128, .box_w = 7, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 114, .adv_w = 128, .box_w = 7, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 121, .adv_w = 128, .box_w = 7, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 128, .adv_w = 128, .box_w = 7, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 135, .adv_w = 64, .box_w = 2, .box_h = 6, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 137, .adv_w = 64, .box_w = 2, .box_h = 8, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 139, .adv_w = 127, .box_w = 7, .box_h = 6, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 145, .adv_w = 127, .box_w = 6, .box_h = 4, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 148, .adv_w = 127, .box_w = 7, .box_h = 6, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 154, .adv_w = 117, .box_w = 7, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 161, .adv_w = 142, .box_w = 9, .box_h = 10, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 173, .adv_w = 149, .box_w = 9, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 182, .adv_w = 149, .box_w = 8, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 190, .adv_w = 149, .box_w = 8, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 198, .adv_w = 149, .box_w = 8, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 206, .adv_w = 139, .box_w = 7, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 213, .adv_w = 128, .box_w = 7, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 220, .adv_w = 160, .box_w = 9, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 229, .adv_w = 160, .box_w = 8, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 237, .adv_w = 75, .box_w = 2, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 239, .adv_w = 128, .box_w = 6, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 245, .adv_w = 160, .box_w = 8, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 253, .adv_w = 128, .box_w = 7, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 260, .adv_w = 181, .box_w = 10, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 270, .adv_w = 160, .box_w = 8, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 278, .adv_w = 160, .box_w = 9, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 287, .adv_w = 139, .box_w = 7, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 294, .adv_w = 160, .box_w = 9, .box_h = 9, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 305, .adv_w = 149, .box_w = 8, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 313, .adv_w = 139, .box_w = 7, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 320, .adv_w = 139, .box_w = 8, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 328, .adv_w = 160, .box_w = 8, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 336, .adv_w = 149, .box_w = 9, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 345, .adv_w = 192, .box_w = 12, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 357, .adv_w = 149, .box_w = 9, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 366, .adv_w = 149, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 374, .adv_w = 139, .box_w = 8, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 382, .adv_w = 75, .box_w = 4, .box_h = 10, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 387, .adv_w = 53, .box_w = 3, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 390, .adv_w = 75, .box_w = 4, .box_h = 10, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 395, .adv_w = 127, .box_w = 6, .box_h = 5, .ofs_x = 1, .ofs_y = 4},
    {.bitmap_index = 399, .adv_w = 96, .box_w = 6, .box_h = 1, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 400, .adv_w = 64, .box_w = 3, .box_h = 1, .ofs_x = 0, .ofs_y = 7},
    {.bitmap_index = 401, .adv_w = 128, .box_w = 7, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 407, .adv_w = 128, .box_w = 7, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 414, .adv_w = 128, .box_w = 7, .box_h = 6, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 420, .adv_w = 128, .box_w = 7, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 427, .adv_w = 128, .box_w = 7, .box_h = 6, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 433, .adv_w = 75, .box_w = 5, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 438, .adv_w = 128, .box_w = 6, .box_h = 8, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 444, .adv_w = 128, .box_w = 6, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 450, .adv_w = 64, .box_w = 2, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 452, .adv_w = 64, .box_w = 3, .box_h = 10, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 456, .adv_w = 128, .box_w = 7, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 463, .adv_w = 64, .box_w = 2, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 465, .adv_w = 192, .box_w = 10, .box_h = 6, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 473, .adv_w = 128, .box_w = 6, .box_h = 6, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 478, .adv_w = 128, .box_w = 7, .box_h = 6, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 484, .adv_w = 128, .box_w = 7, .box_h = 8, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 491, .adv_w = 128, .box_w = 7, .box_h = 8, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 498, .adv_w = 85, .box_w = 5, .box_h = 6, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 502, .adv_w = 117, .box_w = 7, .box_h = 6, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 508, .adv_w = 85, .box_w = 5, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 513, .adv_w = 128, .box_w = 6, .box_h = 6, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 518, .adv_w = 117, .box_w = 7, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 524, .adv_w = 181, .box_w = 11, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 533, .adv_w = 128, .box_w = 8, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 539, .adv_w = 117, .box_w = 7, .box_h = 8, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 546, .adv_w = 107, .box_w = 6, .box_h = 6, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 551, .adv_w = 75, .box_w = 4, .box_h = 10, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 556, .adv_w = 53, .box_w = 2, .box_h = 10, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 559, .adv_w = 75, .box_w = 5, .box_h = 10, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 566, .adv_w = 127, .box_w = 7, .box_h = 2, .ofs_x = 1, .ofs_y = 3},
    {.bitmap_index = 568, .adv_w = 77, .box_w = 4, .box_h = 3, .ofs_x = 1, .ofs_y = 5}
};

/*---------------------
 *  CHARACTER MAPPING
 *--------------------*/



/*Collect the unicode lists and glyph_id offsets*/
static const lv_font_fmt_txt_cmap_t cmaps[] =
{
    {
        .range_start = 32, .range_length = 95, .glyph_id_start = 1,
        .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
    },
    {
        .range_start = 176, .range_length = 1, .glyph_id_start = 96,
        .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
    }
};

/*-----------------
 *    KERNING
 *----------------*/


/*Pair left and right glyphs for kerning*/
static const uint8_t kern_pair_glyph_ids[] =
{
    34, 36,
    34, 40,
    34, 48,
    34, 50,
    34, 53,
    34, 54,
    34, 55,
    34, 56,
    34, 58,
    34, 81,
    34, 86,
    34, 87,
    35, 13,
    35, 15,
    35, 34,
    35, 54,
    36, 13,
    36, 15,
    37, 13,
    37, 15,
    37, 34,
    37, 55,
    37, 56,
    37, 58,
    39, 13,
    39, 15,
    39, 34,
    39, 66,
    39, 70,
    39, 77,
    39, 80,
    39, 83,
    40, 13,
    40, 15,
    43, 13,
    43, 15,
    43, 34,
    43, 66,
    43, 70,
    43, 80,
    43, 86,
    43, 90,
    44, 36,
    44, 40,
    44, 48,
    44, 70,
    44, 80,
    44, 86,
    44, 87,
    44, 90,
    45, 36,
    45, 40,
    45, 48,
    45, 53,
    45, 54,
    45, 55,
    45, 56,
    45, 58,
    45, 88,
    45, 90,
    47, 34,
    48, 13,
    48, 15,
    48, 34,
    48, 53,
    48, 55,
    48, 56,
    48, 57,
    48, 58,
    49, 13,
    49, 15,
    49, 34,
    49, 66,
    49, 70,
    49, 80,
    50, 13,
    50, 15,
    50, 34,
    50, 53,
    50, 55,
    50, 58,
    51, 36,
    51, 40,
    51, 48,
    51, 50,
    51, 53,
    51, 54,
    51, 55,
    51, 58,
    51, 70,
    51, 80,
    51, 86,
    51, 88,
    53, 13,
    53, 14,
    53, 15,
    53, 27,
    53, 28,
    53, 34,
    53, 36,
    53, 40,
    53, 48,
    53, 50,
    53, 66,
    53, 68,
    53, 70,
    53, 73,
    53, 77,
    53, 78,
    53, 80,
    53, 83,
    53, 84,
    53, 86,
    53, 88,
    53, 90,
    53, 91,
    54, 13,
    54, 15,
    54, 34,
    55, 13,
    55, 14,
    55, 15,
    55, 27,
    55, 28,
    55, 34,
    55, 36,
    55, 40,
    55, 48,
    55, 50,
    55, 66,
    55, 70,
    55, 74,
    55, 80,
    55, 83,
    55, 86,
    56, 13,
    56, 14,
    56, 15,
    56, 36,
    56, 40,
    56, 48,
    56, 66,
    56, 69,
    56, 70,
    56, 73,
    56, 74,
    56, 80,
    56, 86,
    57, 36,
    57, 40,
    57, 48,
    58, 13,
    58, 14,
    58, 15,
    58, 27,
    58, 28,
    58, 34,
    58, 36,
    58, 40,
    58, 48,
    58, 52,
    58, 66,
    58, 69,
    58, 70,
    58, 80,
    58, 81,
    58, 82,
    58, 86,
    58, 87,
    66, 67,
    66, 72,
    66, 81,
    66, 85,
    66, 87,
    67, 67,
    67, 87,
    68, 73,
    68, 76,
    68, 77,
    68, 90,
    69, 69,
    69, 88,
    70, 67,
    70, 72,
    70, 87,
    70, 89,
    71, 1,
    71, 2,
    71, 13,
    71, 15,
    71, 32,
    71, 66,
    71, 70,
    71, 71,
    71, 76,
    71, 80,
    72, 72,
    72, 77,
    72, 83,
    72, 90,
    76, 70,
    76, 80,
    76, 90,
    77, 90,
    79, 87,
    80, 13,
    80, 15,
    80, 87,
    80, 88,
    80, 89,
    80, 90,
    80, 91,
    81, 13,
    81, 15,
    81, 88,
    81, 90,
    83, 13,
    83, 15,
    83, 27,
    83, 28,
    83, 66,
    83, 68,
    83, 69,
    83, 70,
    83, 72,
    83, 75,
    83, 76,
    83, 77,
    83, 78,
    83, 79,
    83, 80,
    83, 81,
    83, 82,
    83, 83,
    83, 84,
    83, 85,
    83, 86,
    83, 87,
    83, 90,
    84, 88,
    87, 13,
    87, 15,
    87, 68,
    87, 69,
    87, 70,
    87, 80,
    87, 82,
    88, 13,
    88, 15,
    88, 66,
    88, 68,
    88, 69,
    88, 70,
    88, 73,
    88, 80,
    88, 82,
    89, 68,
    89, 69,
    89, 70,
    89, 80,
    90, 13,
    90, 15,
    90, 66,
    90, 68,
    90, 69,
    90, 70,
    90, 80,
    91, 68,
    91, 69,
    91, 70,
    91, 80
};

/* Kerning between the respective left and right glyphs
 * 4.4 format which needs to scaled with `kern_scale`*/
static const int8_t kern_pair_values[] =
{
    -3, -3, -3, -4, -13, -7, -11, 2,
    -16, 3, -2, -3, 5, 3, -3, -5,
    5, 3, -2, -3, -8, -7, 3, -7,
    -28, -29, -18, -5, -7, 2, -7, -7,
    3, 2, -3, -5, -5, -3, -5, -5,
    -5, -3, -7, -7, -7, -7, -7, -5,
    -3, -3, -3, -3, -3, -10, -5, -10,
    -3, -15, -3, -2, -2, -3, -5, -7,
    -5, -8, -3, -10, -13, -35, -36, -16,
    -3, -5, -5, 6, 3, 3, -3, -8,
    -10, -3, -3, -3, -4, -2, -3, -3,
    -8, -5, -5, -3, 3, -28, -13, -29,
    -7, -7, -13, -7, -7, -7, -7, -10,
    -12, -12, 5, 4, -8, -13, -8, -13,
    -8, -2, -3, -7, -5, -7, -7, -23,
    -8, -25, -5, -5, -11, -7, -7, -7,
    -7, -8, -10, 3, -10, -7, -7, -8,
    -2, -10, -3, -3, -3, -3, -5, -5,
    6, 6, -5, -3, -7, -7, -7, -31,
    -16, -33, -12, -12, -18, -12, -12, -12,
    -8, -16, -18, -18, -16, -10, -16, -12,
    -5, 2, 3, 2, -3, -3, 2, -5,
    -3, -3, -5, -3, 2, 2, -2, 5,
    -5, -5, 11, 16, -10, -12, 16, -2,
    -3, 5, 2, -3, 2, 2, 3, 3,
    -7, -7, 3, 3, -5, -2, -3, -7,
    -3, -7, -3, 3, -2, -3, -5, -3,
    -13, -15, 8, 8, 3, 3, 5, 3,
    5, 8, 6, 6, 10, 10, 3, 8,
    5, 8, 5, 10, 6, 11, 11, -5,
    -16, -18, -5, -3, -5, -5, -5, -15,
    -16, -2, -2, -2, -2, 3, -3, -3,
    -3, -3, -3, -3, -16, -18, -3, -3,
    -3, -3, -3, -2, -2, -2, -2
};

/*Collect the kern pair's data in one place*/
static const lv_font_fmt_txt_kern_pair_t kern_pairs =
{
    .glyph_ids = kern_pair_glyph_ids,
    .values = kern_pair_values,
    .pair_cnt = 271,
    .glyph_ids_size = 0
};

/*--------------------
 *  ALL CUSTOM DATA
 *--------------------*/

#if LVGL_VERSION_MAJOR == 8
/*Store all the custom data of the font*/
static  lv_font_fmt_txt_glyph_cache_t cache;
#endif

#if LVGL_VERSION_MAJOR >= 8
static const lv_font_fmt_txt_dsc_t font_dsc = {
#else
static lv_font_fmt_txt_dsc_t font_dsc = {
#endif
    .glyph_bitmap = glyph_bitmap,
    .glyph_dsc = glyph_dsc,
    .cmaps = cmaps,
    .kern_dsc = &kern_pairs,
    .kern_scale = 16,
    .cmap_num = 2,
    .bpp = 1,
    .kern_classes = 0,
    .bitmap_format = 0,
#if LVGL_VERSION_MAJOR == 8
    .cache = &cache
#endif

};



/*-----------------
 *  PUBLIC FONT
 *----------------*/

/*Initialize a public general font descriptor*/
#if LVGL_VERSION_MAJOR >= 8
const lv_font_t Arial_Black_12 = {
#else
lv_font_t Arial_Black_12 = {
#endif
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt,    /*Function pointer to get glyph's data*/
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,    /*Function pointer to get glyph's bitmap*/
    .line_height = 11,          /*The maximum line height required by the font*/
    .base_line = 2,             /*Baseline measured from the bottom of the line*/
#if !(LVGL_VERSION_MAJOR == 6 && LVGL_VERSION_MINOR == 0)
    .subpx = LV_FONT_SUBPX_NONE,
#endif
#if LV_VERSION_CHECK(7, 4, 0) || LVGL_VERSION_MAJOR >= 8
    .underline_position = -1,
    .underline_thickness = 1,
#endif
    .static_bitmap = 0,
    .dsc = &font_dsc,          /*The custom font data. Will be accessed by `get_glyph_bitmap/dsc` */
#if LV_VERSION_CHECK(8, 2, 0) || LVGL_VERSION_MAJOR >= 9
    .fallback = NULL,
#endif
    .user_data = NULL,
};



#endif /*#if ARIAL_BLACK_12*/
