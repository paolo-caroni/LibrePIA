/*
 * LibrePIA is an attempt to create a LIBrary to Read and Edit PIA plot files (.pc3, .pmp, .stb, .ctb).
 *
 * Copyright (C) 2020 Paolo Caroni <kenren89@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with this program.
 * If not, see <http://www.gnu.org/licenses/>.
 */

/* WARNING, the LibrePIA project at the moment is only a proof of concept*/

/* include dependancy*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <zlib.h>
/* include header files*/
#include "LibrePIA.h"
#include "plot_style_table.h"

 /* proof of concept for modify PIA file in a text form,
 can be used for all PIA file (ctb, stb, pc3, pmp)*/
 int main(int argc, char **argv)
 {
    read_header(argv[1]);
    decompress_data(argv[1],argv[2]);
    /* verify subclass type*/
    if (header[19]=='C' && header[20]=='T' && header[21]=='B')
    {
       /* parse ctb*/
       plot_style_parser(argv[2]);
       /* here can be put a code for modify the values
       */
       /* rewrite txt for ctb*/
       plot_style_writer(argv[2]);
    }

    else if (header[19]=='S' && header[20]=='T' && header[21]=='B')
    {
       /* parse stb*/
       plot_style_parser(argv[2]);
       fprintf(stderr, "Sorry, actually can be readed, writed or edited max %d styles, your file have %d styles.\n\n", MAX_STYLE, total_style_number);
       /* here can be put a code for modify the values
       */
       /* rewrite txt for stb*/
       plot_style_writer(argv[2]);
    }

    else if (header[19]=='P' && header[20]=='C' && header[21]=='3')
    {
       fprintf(stderr, "Sorry, the .pc3 subclass type isn't yet supported.\nMaybe .pc3 would be never supported since is correlated to .hdi file.\n\n");
    }

    else if (header[19]=='P' && header[20]=='M' && header[21]=='P')
    {
       fprintf(stderr, "Sorry, the .pmp subclass type isn't yet supported.\nThe support of .pmp. isn't a priority.\n\n");
    }

    /* if in the header there is something wrong*/
    else
    {
       fprintf(stderr, "WARNING:This isn't a known subclass type\n\n");
    }

    /* compress data to other*/
    write_PIA(argv[2], argv[3]);
 }


