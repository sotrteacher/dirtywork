/********************************************************************
Copyright 2010-2015 K.C. Wang, <kwang@eecs.wsu.edu>
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
********************************************************************/
#include "ucode.c"

int nbuf, requests, hits, ratio;

main(argc, argv) int argc; char *argv[];
{
    printf("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");
    printf("        This is KC's hit-ratio in action\n");
    printf("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");

    nbuf = khits(&requests, &hits);
    ratio = (hits)/( (requests+99)/100 );
    printf("buffer#=%d   ", nbuf);
    printf("requests=%d  ", requests);
    printf("hits=%d      ", hits); 
    printf("ratio=%d\n", ratio);
    exit(0);   

}

             
