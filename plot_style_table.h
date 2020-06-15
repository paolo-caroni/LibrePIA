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
int color_number[255];
char name[255][261];
char localized_name[255][261];
char description[255][4869];
int color[255];
/* mode_color is optional*/
int mode_color[255];
int color_policy[255];
int physical_pen_number[255];
int virtual_pen_number[255];
int screen[255];
float linepattern_size[255];
int linetype[255];
/* adaptive_linetype better using boolean? or char?*/
int adaptive_linetype[255];
int lineweight[255];
int fill_style[255];
int end_style[255];
int join_style[255];
/* custom_lineweight_table*/
float custom_lineweight_table[27];

/* other useful entities*/
int min_style=0,max_style=254,style_number;
/* boolean, define TRUE and FALSE
_Bool aci_table_available;
_Bool apply_factor;
_Bool adaptive_linetype[255];
const int TRUE = 1;
const int FALSE = 0;
*/