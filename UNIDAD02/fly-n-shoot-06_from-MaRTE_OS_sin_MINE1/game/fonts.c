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
-----------------------------------------------------------------------------
--                               MaRTEnoid                                 --
-----------------------------------------------------------------------------
--                                                                         --
--  MaRTEnoid is an example game for MaRTE OS.                             --
--  This code is distributed for educational purposes only.                --
--                                                                         --
--  author:   Alvaro Garcia Cuesta                                         --
--  email:    alvaro@binarynonsense.com                                    --
--  website:  www.binarynonsense.com                                       --
--                                                                         --
--  file: fonts.c                                                          --
--                                                                         --
--  this file contains the functions used to print strings                 --
--  using a bitmap font                                                    --
-----------------------------------------------------------------------------
--                               License                                  --
-----------------------------------------------------------------------------
--                                                                         --
-- MaRTEnoid is free software; you can redistribute it and/or modify it    --
-- under the terms of the GNU General Public License version 2 as          --
-- published by the Free Software Foundation.                              --
-- See COPYING file for more info about the license                        --
--                                                                         --
-----------------------------------------------------------------------------
--                        last update: 02 Jun 09                           --
---------------------------------------------------------------------------*/

#include <string.h>
#include "vga.h"
#include "font.h"
#include "fonts.h"

int *theFont=font_img;
int screenWidth = 320;

// Modified 64 by 64 font map
int fontWidth=4;
int fontHeight=8;
int fontFileWidth=64;


void bmpfont_print(const char *theString, int x, int y,unsigned char *backBuffer, uint8_t color){
    uint8_t bg_color = (color & 0x70) >> 4, fg_color = color & 0x0F;
    int length;
    length = strlen(theString);

    int charPosition,charValue,fontPositionX,fontPositionY,fcolor;
    int xf,yf,initX,initY;
    for(charPosition=0;charPosition<length;charPosition++){
        charValue=theString[charPosition];
        if(charValue<128){
            fontPositionY=charValue/16;
            fontPositionX=charValue-(fontPositionY*16);
            initX=fontPositionX*fontWidth;
            initY=fontPositionY*fontHeight;
            for(xf=0;xf<fontWidth;xf++){
                for(yf=0;yf<fontHeight;yf++){
                    fcolor=theFont[fontFileWidth*(yf+initY)+(xf+initX)];
                    if(fcolor!=0){
                        backBuffer[screenWidth*(y+yf)+(x+xf+charPosition*fontWidth)]=fg_color;//foreground
                    }
                    else{
                        backBuffer[screenWidth*(y+yf)+(x+xf+charPosition*fontWidth)]=bg_color;//background
                    }
                }
            }

        }

    }
}
