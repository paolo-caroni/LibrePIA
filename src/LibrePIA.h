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

/* declare header and info common to all PIA files*/

/* header*/
/* directly readed*/
unsigned char header[37],header2[11],checksum[12];
/* obtained by program*/
unsigned int decompressed_size, compressed_size, Adler32;
unsigned int input_file_size;
/* unused header variable*/
/*int PIA_ver=0, subclass_ver=0;*/

/* other info*/
/* number of line on uncompressed txt file*/
int PIA_uncompressed_line_number;
/* NULL byte end it's needed or not*/
int PIA_NULL_byte_needed;

/* buffer and utility*/
int k;
int position;
unsigned char line_buffer[4900];

/* declare function*/
int read_header(char *);
int decompress_data(char *, char *);
int plot_style_parser(char *);
int plot_style_writer(char *);
int write_PIA(char *, char *);
int new_ctb();
int new_stb();
int add_plot_style_stb();
int remove_plot_style_stb(int position);
int int2typergb(int integer_color_value);
int rgb2int(int R, int G, int B);
int change_color(int integer_color_value, int position);
int export_plot_style2csv(char *);
int export_plot_style2tsv(char *);
int plot_model_parameter_parser(char *);



