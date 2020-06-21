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

/* declare ctb and stb int, float, string, matrix, etc*/

/* STB can have max_style over this value, maybe near to infinite... need improvement*/
#define MAX_STYLE 510

/* initial value of number of stile (on CTB=255, on STB can be infinite)*/
int total_style_number;
/* boolean, define TRUE and FALSE
_Bool aci_table_available;
_Bool apply_factor;
_Bool adaptive_linetype[MAX_STYLE];
const int TRUE = 1;
const int FALSE = 0;
*/

/* description of the file*/
char file_description[1027];
/* aci_table_available better using boolean? or char?*/
int aci_table_available;
/* scale_factor*/
float scale_factor;
/* apply_factor better using boolean? or char?*/
int apply_factor;
/* custom_lineweight_display_units*/
int custom_lineweight_display_units;
/* aci_table (only for ctb)*/
char aci_table[255][20];
/* plot_style*/
char name[MAX_STYLE][261];
char localized_name[MAX_STYLE][261];
char description[MAX_STYLE][4869];
int color[MAX_STYLE];
/* mode_color is optional*/
int mode_color[MAX_STYLE];
int color_policy[MAX_STYLE];
int physical_pen_number[MAX_STYLE];
int virtual_pen_number[MAX_STYLE];
int screen[MAX_STYLE];
float linepattern_size[MAX_STYLE];
int linetype[MAX_STYLE];
/* adaptive_linetype better using boolean? or char?*/
int adaptive_linetype[MAX_STYLE];
int lineweight[MAX_STYLE];
int fill_style[MAX_STYLE];
int end_style[MAX_STYLE];
int join_style[MAX_STYLE];
/* custom_lineweight_table*/
float custom_lineweight_table[27];

/* declare ACI colors*/
unsigned int aci_color[256];