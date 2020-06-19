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
unsigned int readed_decompressed_size=0, readed_compressed_size=0, readed_Adler32=0;
unsigned int writed_decompressed_size=0, writed_compressed_size=0, writed_Adler32=0;
unsigned int input_file_size=0;
/* unused header variable*/
/*int PIA_ver=0, subclass_ver=0;*/

/* other info*/
/* number of line on uncompressed txt file*/
int PIA_uncompressed_line_number=0;

/* buffer and utility*/
int k=0;
int position;
unsigned char line_buffer[4900];