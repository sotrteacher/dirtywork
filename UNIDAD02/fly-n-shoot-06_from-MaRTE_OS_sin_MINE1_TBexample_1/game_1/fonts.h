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
--  file: fonts.h                                                          --
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

#pragma once
#ifndef _FONTS_H_
#define _FONTS_H_

void bmpfont_print(const char *theString, int x, int y,unsigned char *backBuffer, uint8_t color);

#endif