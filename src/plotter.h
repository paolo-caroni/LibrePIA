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

/* declare pmp struct, int, float, string, matrix, etc*/

/* initial value of number of size, description*/
int total_size_number, total_description_number;

/* size struct*/
typedef struct {
    int caps_type;
    char name[44];
    /* localized_name is the user defined name*/
    char localized_name[44];
    /* media_description_name is the name writed in the description struct*/
    char media_description_name[110];
    int media_group;
    /* landscape_mode better using boolean? or char?*/
    int landscape_mode;
} pmp_size;

/* description struct*/
typedef struct {
    int caps_type;
    /* name is the media_description_name writed in the size struct*/
    char name[110];
    /* width*/
    float media_bounds_urx;
    /* height*/
    float media_bounds_ury;
    /* width border that can not be printed*/
    float printable_bounds_llx;
    /* height border that can not be printed*/
    float printable_bounds_lly;
    /* printable width*/
    float printable_bounds_urx;
    /* printable height*/
    float printable_bounds_ury;
    /* roral printable area*/
    float printable_area;
    /* dimensional better using boolean? or char?*/
    int dimensional;
} pmp_description;

/* other info and data*/
char user_defined_model_pathname[140];
char user_defined_model_basename[140];
char driver_pathname[70];
char driver_version[40];
char driver_tag_line[50];
int toolkit_version;
int driver_type;
char canonical_family_name[40];
/* show_custom_first better using boolean? or char?*/
int show_custom_first;
/* truetype_as_text better using boolean? or char?*/
int truetype_as_text;
char canonical_model_name[10];
char localized_family_name[40];
char localized_model_name[10];
/* file_only better using boolean? or char?*/
int file_only;
char  model_abilities[12];
char  udm_description[12];
char win_device_name[40];
char win_driver_name[40];
char short_net_name[40];
char friendly_net_name[12];
int dm_driver_version;
/* default_system_cfg better using boolean? or char?*/
int default_system_cfg;
char platform[5];
char locale[8];
char abilities[52];
char caps_state[52];
char ui_owner[24];
float size_max_x;
float size_max_y;
float calibration_x;
float calibration_y;
