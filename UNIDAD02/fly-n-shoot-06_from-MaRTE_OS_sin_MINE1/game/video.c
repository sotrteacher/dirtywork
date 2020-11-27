//#define LMC_8000      //No imprime el fondo
//#define LMC_9000      //No imprime las letras peque\~nas
//#define LMC_10000     //No llama a blit()
//#define LMC_11000     //No llam a vga_drawscansegment()
/*****************************************************************************
*                     INSTITUTO POLITÉCNICO NACIONAL
* UNIDAD PROFESIONAL INTERDISCIPLINARIA EN INGENIERÍA Y TECNOLOGÍAS AVANZADAS
*
*                   SISTEMAS OPERATIVOS EN TIEMPO REAL.
*                             Periodo 20/1
*
* Port de juego Fly'n'Shoot (Quantum Leaps) para plataforma MaRTE OS
*
* Autores de port:
* José Fausto Romero Lujambio
* Pastor Alan Rodríguez Echeverría
*
******************************************************************************
* Product:  VGA screen output
* Last Updated for Version: 4.1.01
* Date of the Last Update:  Nov 02, 2009
*
*                    Q u a n t u m     L e a P s
*                    ---------------------------
*                    innovating embedded systems
*
* Copyright (C) 2002-2009 Quantum Leaps, LLC. All rights reserved.
*
* This software may be distributed and modified under the terms of the GNU
* General Public License version 2 (GPL) as published by the Free Software
* Foundation and appearing in the file GPL.TXT included in the packaging of
* this file. Please note that GPL Section 2[b] requires that all works based
* on this software must also be made publicly available under the terms of
* the GPL ("Copyleft").
*
* Alternatively, this software may be distributed and modified under the
* terms of Quantum Leaps commercial licenses, which expressly supersede
* the GPL and are specifically designed for licensees interested in
* retaining the proprietary status of their code.
*
* Contact information:
* Quantum Leaps Web site:  http://www.quantum-leaps.com
* e-mail:                  info@quantum-leaps.com
*****************************************************************************/

#include "qp_port.h"
#include "video.h"

#include <stdlib.h>
#include <stdio.h>

#define VIDEO_WIDTH  80
#define VIDEO_HEIGHT 25
#define VIDEO_BLINK_TICKS 15

typedef struct {
    uint8_t color;
    char letter;
} term_char_t;

/* variables locales */
static term_char_t terminal_buffer[VIDEO_HEIGHT][VIDEO_WIDTH];

/*..........................................................................*/
void init_terminal_buffer(void) {
    clear_terminal_buffer(0, 0, VIDEO_WIDTH, VIDEO_HEIGHT);
}
/*..........................................................................*/
void clear_terminal_buffer(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2) {
    int i, j;
    for (i = x1; i < x2; ++i) {
        for (j = y1; j < y2; ++j) {
            terminal_buffer[j][i].color = 0;
            terminal_buffer[j][i].letter = ' ';
        }
    }
}
/*..........................................................................*/
int get_x_to_pixel(uint8_t x) {
    return (float)screenWidth/VIDEO_WIDTH*x;
}
/*..........................................................................*/
int get_y_to_pixel(uint8_t y) {
    return (float)screenHeight/VIDEO_HEIGHT*y;
}
/*..........................................................................*/
int get_pixel_to_x(int x) {
    return (float)VIDEO_WIDTH/screenWidth*x;
}
/*..........................................................................*/
int get_pixel_to_y(int y) {
    return (float)VIDEO_HEIGHT/screenHeight*y;
}
/*..........................................................................*/
void Video_render(void) {
    static int blink_timer = 0;
    static bool blink_state = true;
    char str[2] = {0,0};
    int i, j, x, y;

    for (i = 0; i < VIDEO_WIDTH; ++i) {
        for (j = 0; j < VIDEO_HEIGHT; ++j) {
            uint8_t color = terminal_buffer[j][i].color;
            bool blinking = (color & 0x80) && 1;

#ifndef LMC_8000
            // pintar fondo
            for (x = get_x_to_pixel(i); x < get_x_to_pixel(i+1); ++x) {
                for (y = get_y_to_pixel(j); y < get_y_to_pixel(j+1); ++y) {
                    backBuffer[screenWidth*(y)+(x)] = (color & 0x70) >> 4;
                }
            }
#endif /*LMC_8000*/

#ifndef LMC_9000
            // dibujar caracter considerando parpadeo
            if (terminal_buffer[j][i].letter != ' ' && (blink_state || !blinking)) {
                snprintf(str, sizeof(str), "%c", terminal_buffer[j][i].letter);
                bmpfont_print(str, get_x_to_pixel(i), get_y_to_pixel(j), backBuffer, color);
            }
#endif /*LMC_9000*/
        }
    }

    // generador de frecuencia para blinker
    if ((++blink_timer % VIDEO_BLINK_TICKS) == 0) {
        blink_state ^= true;
    }

#ifndef LMC_10000
    blit(); // swap vga buffer
#endif /*LMC_10000*/
}/*end Video_render()*/
/*..........................................................................*/
void Video_clearScreen(uint8_t bgColor) {
    Video_clearRect(0,  0, VIDEO_WIDTH, VIDEO_HEIGHT, bgColor);
}
/*..........................................................................*/
void Video_clearRect(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2,
                     uint8_t bgColor)
{
    int i, j;

    clear_terminal_buffer(x1,y1,x2,y2);
    for (i = x1; i < x2; ++i) {
        for (j = y1; j < y2; ++j) {
            terminal_buffer[j][i].color = bgColor;
        }
    }
}
/*..........................................................................*/
void Video_printStrAt(uint8_t x, uint8_t y, uint8_t color,
                      char const *str)
{
    while (*str != '\0') {
        uint8_t current_color = terminal_buffer[y][x].color;
        terminal_buffer[y][x].letter = *str;
        terminal_buffer[y][x].color |= (current_color & 0xF0) | (color & 0x0F);
        ++str;
        ++x;
    }
}
/*..........................................................................*/
void Video_printNumAt(uint8_t x, uint8_t y, uint8_t color, uint32_t num) {
    char str[8]; str[7] = '\0';
    snprintf(str, sizeof(str), "%4d", num);
    Video_printStrAt(x, y, color, str);
}
/*..........................................................................*/
void Video_drawBitmapAt(uint8_t x, uint8_t y,
                        uint8_t const *bitmap, uint8_t width, uint8_t height)
{
    int i, j;
    uint8_t w = width;
    uint8_t h = height;

    /* perform the clipping */
    if (x > VIDEO_WIDTH) {
        x = VIDEO_WIDTH;
    }
    if (y > VIDEO_HEIGHT) {
        y = VIDEO_HEIGHT;
    }
    if (w > VIDEO_WIDTH - x) {
        w = VIDEO_WIDTH - x;
    }
    if (h > VIDEO_HEIGHT - y) {
        h = VIDEO_HEIGHT - y;
    }

    for (i = x; i < x+w; ++i) {
        for (j = y; j < y+h; ++j) {
            static uint8_t const pixel[2] = {
                VIDEO_BGND_BLACK,
                VIDEO_BGND_LIGHT_GRAY
            }; // { 0x20, 0xDB };
            uint8_t byte = bitmap[(i-x) + ((j-y) >> 3)*width];
            terminal_buffer[j][i].color = pixel[(byte >> (j & 0x7)) & 1];
        }
    }
}
/*..........................................................................*/
void Video_drawStringAt(uint8_t x, uint8_t y, char const *str) {
    static uint8_t const font5x7[95][5] = {
        { 0x00, 0x00, 0x00, 0x00, 0x00 },                            /* ' ' */
        { 0x00, 0x00, 0x4F, 0x00, 0x00 },                              /* ! */
        { 0x00, 0x07, 0x00, 0x07, 0x00 },                              /* " */
        { 0x14, 0x7F, 0x14, 0x7F, 0x14 },                              /* # */
        { 0x24, 0x2A, 0x7F, 0x2A, 0x12 },                              /* $ */
        { 0x23, 0x13, 0x08, 0x64, 0x62 },                              /* % */
        { 0x36, 0x49, 0x55, 0x22, 0x50 },                              /* & */
        { 0x00, 0x05, 0x03, 0x00, 0x00 },                              /* ' */
        { 0x00, 0x1C, 0x22, 0x41, 0x00 },                              /* ( */
        { 0x00, 0x41, 0x22, 0x1C, 0x00 },                              /* ) */
        { 0x14, 0x08, 0x3E, 0x08, 0x14 },                              /* * */
        { 0x08, 0x08, 0x3E, 0x08, 0x08 },                              /* + */
        { 0x00, 0x50, 0x30, 0x00, 0x00 },                              /* , */
        { 0x08, 0x08, 0x08, 0x08, 0x08 },                              /* - */
        { 0x00, 0x60, 0x60, 0x00, 0x00 },                              /* . */
        { 0x20, 0x10, 0x08, 0x04, 0x02 },                              /* / */
        { 0x3E, 0x51, 0x49, 0x45, 0x3E },                              /* 0 */
        { 0x00, 0x42, 0x7F, 0x40, 0x00 },                              /* 1 */
        { 0x42, 0x61, 0x51, 0x49, 0x46 },                              /* 2 */
        { 0x21, 0x41, 0x45, 0x4B, 0x31 },                              /* 3 */
        { 0x18, 0x14, 0x12, 0x7F, 0x10 },                              /* 4 */
        { 0x27, 0x45, 0x45, 0x45, 0x39 },                              /* 5 */
        { 0x3C, 0x4A, 0x49, 0x49, 0x30 },                              /* 6 */
        { 0x01, 0x71, 0x09, 0x05, 0x03 },                              /* 7 */
        { 0x36, 0x49, 0x49, 0x49, 0x36 },                              /* 8 */
        { 0x06, 0x49, 0x49, 0x29, 0x1E },                              /* 9 */
        { 0x00, 0x36, 0x36, 0x00, 0x00 },                              /* : */
        { 0x00, 0x56, 0x36, 0x00, 0x00 },                              /* ; */
        { 0x08, 0x14, 0x22, 0x41, 0x00 },                              /* < */
        { 0x14, 0x14, 0x14, 0x14, 0x14 },                              /* = */
        { 0x00, 0x41, 0x22, 0x14, 0x08 },                              /* > */
        { 0x02, 0x01, 0x51, 0x09, 0x06 },                              /* ? */
        { 0x32, 0x49, 0x79, 0x41, 0x3E },                              /* @ */
        { 0x7E, 0x11, 0x11, 0x11, 0x7E },                              /* A */
        { 0x7F, 0x49, 0x49, 0x49, 0x36 },                              /* B */
        { 0x3E, 0x41, 0x41, 0x41, 0x22 },                              /* C */
        { 0x7F, 0x41, 0x41, 0x22, 0x1C },                              /* D */
        { 0x7F, 0x49, 0x49, 0x49, 0x41 },                              /* E */
        { 0x7F, 0x09, 0x09, 0x09, 0x01 },                              /* F */
        { 0x3E, 0x41, 0x49, 0x49, 0x7A },                              /* G */
        { 0x7F, 0x08, 0x08, 0x08, 0x7F },                              /* H */
        { 0x00, 0x41, 0x7F, 0x41, 0x00 },                              /* I */
        { 0x20, 0x40, 0x41, 0x3F, 0x01 },                              /* J */
        { 0x7F, 0x08, 0x14, 0x22, 0x41 },                              /* K */
        { 0x7F, 0x40, 0x40, 0x40, 0x40 },                              /* L */
        { 0x7F, 0x02, 0x0C, 0x02, 0x7F },                              /* M */
        { 0x7F, 0x04, 0x08, 0x10, 0x7F },                              /* N */
        { 0x3E, 0x41, 0x41, 0x41, 0x3E },                              /* O */
        { 0x7F, 0x09, 0x09, 0x09, 0x06 },                              /* P */
        { 0x3E, 0x41, 0x51, 0x21, 0x5E },                              /* Q */
        { 0x7F, 0x09, 0x19, 0x29, 0x46 },                              /* R */
        { 0x46, 0x49, 0x49, 0x49, 0x31 },                              /* S */
        { 0x01, 0x01, 0x7F, 0x01, 0x01 },                              /* T */
        { 0x3F, 0x40, 0x40, 0x40, 0x3F },                              /* U */
        { 0x1F, 0x20, 0x40, 0x20, 0x1F },                              /* V */
        { 0x3F, 0x40, 0x38, 0x40, 0x3F },                              /* W */
        { 0x63, 0x14, 0x08, 0x14, 0x63 },                              /* X */
        { 0x07, 0x08, 0x70, 0x08, 0x07 },                              /* Y */
        { 0x61, 0x51, 0x49, 0x45, 0x43 },                              /* Z */
        { 0x00, 0x7F, 0x41, 0x41, 0x00 },                              /* [ */
        { 0x02, 0x04, 0x08, 0x10, 0x20 },                              /* \ */
        { 0x00, 0x41, 0x41, 0x7F, 0x00 },                              /* ] */
        { 0x04, 0x02, 0x01, 0x02, 0x04 },                              /* ^ */
        { 0x40, 0x40, 0x40, 0x40, 0x40 },                              /* _ */
        { 0x00, 0x01, 0x02, 0x04, 0x00 },                              /* ` */
        { 0x20, 0x54, 0x54, 0x54, 0x78 },                              /* a */
        { 0x7F, 0x48, 0x44, 0x44, 0x38 },                              /* b */
        { 0x38, 0x44, 0x44, 0x44, 0x20 },                              /* c */
        { 0x38, 0x44, 0x44, 0x48, 0x7F },                              /* d */
        { 0x38, 0x54, 0x54, 0x54, 0x18 },                              /* e */
        { 0x08, 0x7E, 0x09, 0x01, 0x02 },                              /* f */
        { 0x0C, 0x52, 0x52, 0x52, 0x3E },                              /* g */
        { 0x7F, 0x08, 0x04, 0x04, 0x78 },                              /* h */
        { 0x00, 0x44, 0x7D, 0x40, 0x00 },                              /* i */
        { 0x20, 0x40, 0x44, 0x3D, 0x00 },                              /* j */
        { 0x7F, 0x10, 0x28, 0x44, 0x00 },                              /* k */
        { 0x00, 0x41, 0x7F, 0x40, 0x00 },                              /* l */
        { 0x7C, 0x04, 0x18, 0x04, 0x78 },                              /* m */
        { 0x7C, 0x08, 0x04, 0x04, 0x78 },                              /* n */
        { 0x38, 0x44, 0x44, 0x44, 0x38 },                              /* o */
        { 0x7C, 0x14, 0x14, 0x14, 0x08 },                              /* p */
        { 0x08, 0x14, 0x14, 0x18, 0x7C },                              /* q */
        { 0x7C, 0x08, 0x04, 0x04, 0x08 },                              /* r */
        { 0x48, 0x54, 0x54, 0x54, 0x20 },                              /* s */
        { 0x04, 0x3F, 0x44, 0x40, 0x20 },                              /* t */
        { 0x3C, 0x40, 0x40, 0x20, 0x7C },                              /* u */
        { 0x1C, 0x20, 0x40, 0x20, 0x1C },                              /* v */
        { 0x3C, 0x40, 0x30, 0x40, 0x3C },                              /* w */
        { 0x44, 0x28, 0x10, 0x28, 0x44 },                              /* x */
        { 0x0C, 0x50, 0x50, 0x50, 0x3C },                              /* y */
        { 0x44, 0x64, 0x54, 0x4C, 0x44 },                              /* z */
        { 0x00, 0x08, 0x36, 0x41, 0x00 },                              /* { */
        { 0x00, 0x00, 0x7F, 0x00, 0x00 },                              /* | */
        { 0x00, 0x41, 0x36, 0x08, 0x00 },                              /* } */
        { 0x02, 0x01, 0x02, 0x04, 0x02 },                              /* ~ */
    };

    while (*str != '\0') {
        Video_drawBitmapAt(x, y, font5x7[*str - ' '], 5, 8);
        ++str;
        x += 6;
    }
}
/*..........................................................................*/
void blit() {
#ifndef LMC_11000
    //send the buffer to the video memory (double buffering/blitting)
    vga_drawscansegment(backBuffer,0,0,screenWidth*screenHeight);
#endif /*LMC_11000*/
}
/*..........................................................................*/
void initPalette(void) {
    //fill the 256 colors of the VGA palette
    vga_setpalette(0, 0, 0, 0); // black
    vga_setpalette(1, 0, 0, 63); // blue
    vga_setpalette(2, 0, 63, 0); // green
    vga_setpalette(3, 0, 63, 63); // cyan
    vga_setpalette(4, 63, 0, 0); // red
    vga_setpalette(5, 32, 0, 63); // purple
    vga_setpalette(6, 32, 16, 0); // brown
    vga_setpalette(7, 48, 48, 48); // light gray
    vga_setpalette(8, 16, 16, 16); // dark gray
    vga_setpalette(9, 32, 32, 63); // light blue
    vga_setpalette(10, 32, 63, 32); // light green
    vga_setpalette(11, 32, 63, 63); // light cyan
    vga_setpalette(12, 63, 32, 32); // light red
    vga_setpalette(13, 32, 16, 63); // light purple
    vga_setpalette(14, 63, 63, 0); // yellow
    vga_setpalette(15, 63, 63, 63); // white

    vga_setpalette(16, 63, 45, 45);
    vga_setpalette(17, 61, 42, 42);
    vga_setpalette(18, 60, 40, 40);
    vga_setpalette(19, 58, 37, 37);
    vga_setpalette(20, 57, 35, 35);
    vga_setpalette(21, 55, 33, 33);
    vga_setpalette(22, 54, 30, 30);
    vga_setpalette(23, 52, 28, 28);
    vga_setpalette(24, 50, 26, 26);
    vga_setpalette(25, 49, 24, 24);
    vga_setpalette(26, 47, 22, 22);
    vga_setpalette(27, 46, 21, 21);
    vga_setpalette(28, 44, 19, 19);
    vga_setpalette(29, 43, 17, 17);
    vga_setpalette(30, 41, 15, 15);
    vga_setpalette(31, 40, 14, 14);
    vga_setpalette(32, 38, 12, 12);
    vga_setpalette(33, 37, 11, 11);
    vga_setpalette(34, 35, 10, 10);
    vga_setpalette(35, 34, 8, 8);
    vga_setpalette(36, 32, 7, 7);
    vga_setpalette(37, 31, 6, 6);
    vga_setpalette(38, 29, 5, 5);
    vga_setpalette(39, 28, 4, 4);
    vga_setpalette(40, 26, 3, 3);
    vga_setpalette(41, 25, 2, 2);
    vga_setpalette(42, 23, 1, 1);
    vga_setpalette(43, 22, 1, 1);
    vga_setpalette(44, 20, 1, 1);
    vga_setpalette(45, 19, 0, 0);
    vga_setpalette(46, 17, 0, 0);
    vga_setpalette(47, 16, 0, 0);
    vga_setpalette(48, 63, 58, 55);
    vga_setpalette(49, 63, 56, 52);
    vga_setpalette(50, 63, 54, 49);
    vga_setpalette(51, 63, 52, 46);
    vga_setpalette(52, 63, 51, 44);
    vga_setpalette(53, 63, 49, 41);
    vga_setpalette(54, 63, 47, 38);
    vga_setpalette(55, 63, 46, 36);
    vga_setpalette(56, 63, 44, 32);
    vga_setpalette(57, 61, 42, 30);
    vga_setpalette(58, 59, 40, 28);
    vga_setpalette(59, 57, 38, 26);
    vga_setpalette(60, 55, 36, 24);
    vga_setpalette(61, 53, 34, 22);
    vga_setpalette(62, 51, 32, 20);
    vga_setpalette(63, 50, 31, 19);
    vga_setpalette(64, 47, 30, 18);
    vga_setpalette(65, 44, 28, 17);
    vga_setpalette(66, 42, 27, 16);
    vga_setpalette(67, 40, 26, 15);
    vga_setpalette(68, 38, 24, 14);
    vga_setpalette(69, 35, 23, 13);
    vga_setpalette(70, 33, 21, 12);
    vga_setpalette(71, 31, 20, 11);
    vga_setpalette(72, 29, 19, 10);
    vga_setpalette(73, 26, 17, 9);
    vga_setpalette(74, 23, 16, 8);
    vga_setpalette(75, 20, 15, 7);
    vga_setpalette(76, 18, 13, 6);
    vga_setpalette(77, 15, 11, 5);
    vga_setpalette(78, 12, 10, 4);
    vga_setpalette(79, 10, 8, 3);
    vga_setpalette(80, 59, 59, 59);
    vga_setpalette(81, 57, 57, 57);
    vga_setpalette(82, 55, 55, 55);
    vga_setpalette(83, 54, 54, 54);
    vga_setpalette(84, 52, 52, 52);
    vga_setpalette(85, 50, 50, 50);
    vga_setpalette(86, 49, 49, 49);
    vga_setpalette(87, 47, 47, 47);
    vga_setpalette(88, 45, 45, 45);
    vga_setpalette(89, 44, 44, 44);
    vga_setpalette(90, 42, 42, 42);
    vga_setpalette(91, 41, 41, 41);
    vga_setpalette(92, 39, 39, 39);
    vga_setpalette(93, 37, 37, 37);
    vga_setpalette(94, 36, 36, 36);
    vga_setpalette(95, 34, 34, 34);
    vga_setpalette(96, 32, 32, 32);
    vga_setpalette(97, 31, 31, 31);
    vga_setpalette(98, 29, 29, 29);
    vga_setpalette(99, 27, 27, 27);
    vga_setpalette(100, 26, 26, 26);
    vga_setpalette(101, 24, 24, 24);
    vga_setpalette(102, 22, 22, 22);
    vga_setpalette(103, 21, 21, 21);
    vga_setpalette(104, 19, 19, 19);
    vga_setpalette(105, 17, 17, 17);
    vga_setpalette(106, 16, 16, 16);
    vga_setpalette(107, 14, 14, 14);
    vga_setpalette(108, 13, 13, 13);
    vga_setpalette(109, 11, 11, 11);
    vga_setpalette(110, 9, 9, 9);
    vga_setpalette(111, 8, 8, 8);
    vga_setpalette(112, 29, 63, 27);
    vga_setpalette(113, 27, 59, 25);
    vga_setpalette(114, 25, 55, 23);
    vga_setpalette(115, 23, 51, 21);
    vga_setpalette(116, 22, 47, 19);
    vga_setpalette(117, 20, 43, 17);
    vga_setpalette(118, 18, 39, 15);
    vga_setpalette(119, 16, 36, 13);
    vga_setpalette(120, 15, 32, 11);
    vga_setpalette(121, 13, 28, 10);
    vga_setpalette(122, 11, 24, 8);
    vga_setpalette(123, 9, 20, 6);
    vga_setpalette(124, 7, 16, 5);
    vga_setpalette(125, 5, 12, 3);
    vga_setpalette(126, 4, 8, 2);
    vga_setpalette(127, 2, 5, 1);
    vga_setpalette(128, 47, 41, 35);
    vga_setpalette(129, 45, 39, 33);
    vga_setpalette(130, 43, 37, 31);
    vga_setpalette(131, 41, 35, 29);
    vga_setpalette(132, 39, 33, 27);
    vga_setpalette(133, 38, 31, 26);
    vga_setpalette(134, 36, 30, 24);
    vga_setpalette(135, 34, 28, 22);
    vga_setpalette(136, 32, 26, 21);
    vga_setpalette(137, 30, 24, 19);
    vga_setpalette(138, 29, 23, 18);
    vga_setpalette(139, 27, 21, 16);
    vga_setpalette(140, 25, 20, 15);
    vga_setpalette(141, 23, 18, 13);
    vga_setpalette(142, 21, 16, 12);
    vga_setpalette(143, 20, 15, 11);
    vga_setpalette(144, 39, 32, 24);
    vga_setpalette(145, 35, 29, 20);
    vga_setpalette(146, 32, 26, 18);
    vga_setpalette(147, 29, 23, 15);
    vga_setpalette(148, 25, 20, 12);
    vga_setpalette(149, 22, 17, 10);
    vga_setpalette(150, 19, 14, 8);
    vga_setpalette(151, 16, 12, 6);
    vga_setpalette(152, 30, 31, 24);
    vga_setpalette(153, 27, 28, 21);
    vga_setpalette(154, 25, 26, 19);
    vga_setpalette(155, 22, 24, 17);
    vga_setpalette(156, 20, 21, 14);
    vga_setpalette(157, 17, 19, 12);
    vga_setpalette(158, 15, 17, 10);
    vga_setpalette(159, 13, 15, 9);
    vga_setpalette(160, 63, 63, 28);
    vga_setpalette(161, 58, 54, 21);
    vga_setpalette(162, 53, 46, 16);
    vga_setpalette(163, 48, 38, 11);
    vga_setpalette(164, 43, 30, 7);
    vga_setpalette(165, 38, 22, 4);
    vga_setpalette(166, 33, 16, 1);
    vga_setpalette(167, 28, 10, 0);
    vga_setpalette(168, 63, 63, 63);
    vga_setpalette(169, 63, 54, 54);
    vga_setpalette(170, 63, 46, 46);
    vga_setpalette(171, 63, 38, 38);
    vga_setpalette(172, 63, 30, 30);
    vga_setpalette(173, 63, 23, 23);
    vga_setpalette(174, 63, 15, 15);
    vga_setpalette(175, 63, 7, 7);
    vga_setpalette(176, 63, 0, 0);
    vga_setpalette(177, 59, 0, 0);
    vga_setpalette(178, 56, 0, 0);
    vga_setpalette(179, 53, 0, 0);
    vga_setpalette(180, 50, 0, 0);
    vga_setpalette(181, 47, 0, 0);
    vga_setpalette(182, 44, 0, 0);
    vga_setpalette(183, 41, 0, 0);
    vga_setpalette(184, 38, 0, 0);
    vga_setpalette(185, 34, 0, 0);
    vga_setpalette(186, 31, 0, 0);
    vga_setpalette(187, 28, 0, 0);
    vga_setpalette(188, 25, 0, 0);
    vga_setpalette(189, 22, 0, 0);
    vga_setpalette(190, 19, 0, 0);
    vga_setpalette(191, 16, 0, 0);
    vga_setpalette(192, 57, 57, 63);
    vga_setpalette(193, 49, 49, 63);
    vga_setpalette(194, 42, 42, 63);
    vga_setpalette(195, 35, 35, 63);
    vga_setpalette(196, 28, 28, 63);
    vga_setpalette(197, 20, 20, 63);
    vga_setpalette(198, 13, 13, 63);
    vga_setpalette(199, 6, 6, 63);
    vga_setpalette(200, 0, 0, 63);
    vga_setpalette(201, 0, 0, 56);
    vga_setpalette(202, 0, 0, 50);
    vga_setpalette(203, 0, 0, 44);
    vga_setpalette(204, 0, 0, 38);
    vga_setpalette(205, 0, 0, 32);
    vga_setpalette(206, 0, 0, 26);
    vga_setpalette(207, 0, 0, 20);
    vga_setpalette(208, 63, 63, 63);
    vga_setpalette(209, 63, 58, 54);
    vga_setpalette(210, 63, 53, 46);
    vga_setpalette(211, 63, 49, 38);
    vga_setpalette(212, 63, 44, 30);
    vga_setpalette(213, 63, 40, 22);
    vga_setpalette(214, 63, 35, 14);
    vga_setpalette(215, 63, 31, 6);
    vga_setpalette(216, 60, 28, 5);
    vga_setpalette(217, 58, 27, 3);
    vga_setpalette(218, 55, 25, 3);
    vga_setpalette(219, 53, 23, 2);
    vga_setpalette(220, 50, 21, 1);
    vga_setpalette(221, 48, 19, 0);
    vga_setpalette(222, 45, 17, 0);
    vga_setpalette(223, 43, 16, 0);
    vga_setpalette(224, 63, 63, 63);
    vga_setpalette(225, 63, 63, 53);
    vga_setpalette(226, 63, 63, 44);
    vga_setpalette(227, 63, 63, 35);
    vga_setpalette(228, 63, 63, 26);
    vga_setpalette(229, 63, 63, 17);
    vga_setpalette(230, 63, 63, 8);
    vga_setpalette(231, 63, 63, 0);
    vga_setpalette(232, 41, 15, 0);
    vga_setpalette(233, 39, 13, 0);
    vga_setpalette(234, 36, 11, 0);
    vga_setpalette(235, 33, 8, 0);
    vga_setpalette(236, 19, 14, 9);
    vga_setpalette(237, 16, 11, 6);
    vga_setpalette(238, 13, 8, 4);
    vga_setpalette(239, 11, 6, 2);
    vga_setpalette(240, 0, 0, 20);
    vga_setpalette(241, 0, 0, 17);
    vga_setpalette(242, 0, 0, 14);
    vga_setpalette(243, 0, 0, 11);
    vga_setpalette(244, 0, 0, 8);
    vga_setpalette(245, 0, 0, 5);
    vga_setpalette(246, 0, 0, 2);
    vga_setpalette(247, 0, 0, 0);
    vga_setpalette(248, 63, 39, 16);
    vga_setpalette(249, 63, 57, 18);
    vga_setpalette(250, 63, 30, 63);
    vga_setpalette(251, 63, 0, 63);
    vga_setpalette(252, 51, 0, 51);
    vga_setpalette(253, 39, 0, 38);
    vga_setpalette(254, 27, 0, 26);
    vga_setpalette(255, 41, 26, 26);
}
/*..........................................................................*/
