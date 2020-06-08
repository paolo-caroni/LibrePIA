


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

/* declare ctb int, float, string, matrix, etc*/
char line_buffer[1039],description[1027],/*aci_table_available[6],apply_factor[5],*/aci_table[256][20];
int aci_table_available,apply_factor,custom_lineweight_display_units,min_color=0,max_color=254;
float scale_factor;
/* boolean, define TRUE and FALSE
_Bool aci_table_available;
const int TRUE = 1;
const int FALSE = 0;
*/
int color_number[255];
char name[255][12];
char localized_name[255][12];
char color_description[255][1025];
int color[255];
int mode_color[255];
int color_policy[255];
int physical_pen_number[255];
int virtual_pen_number[255];
int screen[255];
float linepattern_size[255];
int linetype[255];
/* better using boolean? or char?*/
int adaptive_linetype[255];
int lineweight[255];
int fill_style[255];
int end_style[255];
int join_style[255];

float custom_lineweight_table[27];