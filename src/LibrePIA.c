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


 /* obtain information of version,file subclass type and sizes*/
 int read_header(char *infilename)
 {
    /* open compressed PIA file but without zlib*/
    FILE *infile=fopen(infilename, "rb");

    /* READ header and other info*/
    fread(header, 1, sizeof(header), infile);
    fread(header2, 1, sizeof(header2), infile);
    fread(checksum, 1, sizeof(checksum), infile);

    /* verify if it's a PIA file*/
    if (header[0]=='P' && header[1]=='I' && header[2]=='A' && header[3]=='F' && header[4]=='I' && header[5]=='L' && header[6]=='E')
    {
    }
    /* if in the header there is something wrong*/
    else
    {
    fprintf(stderr, "WARNING:This isn't a PIA file\n\n");
    }

    /* verify PIA file version*/
    /* PIA version*/
    if (header[15]=='2' && header[16]=='.' && header[17]=='0')
    {
    /*PIA_ver=2; unused the actual aim is 2.0 only*/
    }
    /* PIA version isn't 2.0*/
    else
    {
    fprintf(stderr, "WARNING:unsupported PIA file version\n\n");
    }

    /* verify subclass version*/
    /* subclass version*/
    if (header[25]=='1')
    {
    /*subclass_ver=1; unused the actual aim is 1 only*/
    }
    /* subclass_ver isn't 1*/
    else
    {
    fprintf(stderr, "WARNING:unsupported sub-class version\n\n");
    }

    /* readed_Adler32 set to the combination of the number 49-50-51-52 bytes*/
    readed_Adler32 = (readed_Adler32 << 8) + checksum[3];
    readed_Adler32 = (readed_Adler32 << 8) + checksum[2];
    readed_Adler32 = (readed_Adler32 << 8) + checksum[1];
    readed_Adler32 = (readed_Adler32 << 8) + checksum[0];

    /* readed_decompressed_size set to the combination of the number 53-54-55-56 bytes*/
    readed_decompressed_size = (readed_decompressed_size << 8) + checksum[7];
    readed_decompressed_size = (readed_decompressed_size << 8) + checksum[6];
    readed_decompressed_size = (readed_decompressed_size << 8) + checksum[5];
    readed_decompressed_size = (readed_decompressed_size << 8) + checksum[4];

    /* readed_compressed_size set to the combination of the number 57-58-59-60 bytes*/
    readed_compressed_size = (readed_compressed_size << 8) + checksum[11];
    readed_compressed_size = (readed_compressed_size << 8) + checksum[10];
    readed_compressed_size = (readed_compressed_size << 8) + checksum[9];
    readed_compressed_size = (readed_compressed_size << 8) + checksum[8];

    /* debug function*/
    #if DEBUG
    printf("expected compressed size: %d\n", readed_compressed_size);
    printf("expected decompressed size: %d\n", readed_decompressed_size);
    #endif

    /* close input and output file*/
    fclose(infile);
 }


 /* funtion for unzip PIA file*/
 int decompress_data(char *infilename, char *outfilename)
 {
    /* open compressed PIA file*/
    gzFile infile = gzopen(infilename, "rb");
    /* create uncompressed file*/
    FILE *outfile = fopen(outfilename, "wb");
    /* verify if files exist*/
    if (!infile || !outfile) return -1;

    /* declare buffer, buffer size is the compressed size*/
    char buffer[readed_compressed_size];
    /* declare data, data size is equal to decompressed size without header(48byte), Adler 32 checksum(4byte), 
    decompressed size (4byte) and compressed size(4byte), in total (60byte)*/
    char data[readed_decompressed_size];
    /* number of byte readed*/
    int num_read = 0;

    /* first 48 bytes = header (not compressed, composed by "PIAFILEVERSION_2.0,???VER1,compress/r/npmzlibcodec)
    next 4 bytes Adler32 checksum
    next 4 bytes (unsigned int) decompressed stream size
    next 4 bytes (unsigned int) compressed stream size*/
    gzseek (infile, 48+4+4+4, SEEK_CUR);

    /* read input compressed data file*/
    while ((num_read = gzread(infile, buffer, readed_compressed_size)) > 0)
    {
       /* inflate buffer into data*/
       /* zlib struct*/
       z_stream infstream;
       infstream.zalloc = Z_NULL;
       infstream.zfree = Z_NULL;
       infstream.opaque = Z_NULL;

       /* setup "buffer" as the input and "data" as the decompressed output*/
       /* size of input*/
       infstream.avail_in = readed_compressed_size;
       /* input char array*/
       infstream.next_in = (Bytef *)buffer;
       /* size of output*/
       infstream.avail_out = readed_decompressed_size;
       /* output char array*/
       infstream.next_out = (Bytef *)data;
         
       /* the real DE-compression work*/
       inflateInit(&infstream);
       inflate(&infstream, Z_NO_FLUSH);
       inflateEnd(&infstream);

       if(data[readed_compressed_size]!='\n')
       {
          /* write uncompressed data removing last NULL byte*/
          fwrite(data, 1, readed_decompressed_size-1, outfile);
       }
       else
       {
          /* write uncompressed data*/
          fwrite(data, 1, readed_decompressed_size, outfile);
       }

       /* debug*/
       #if DEBUG
       printf("readed %d compressed bytes\n", num_read);
       printf("writed %d decompressed bytes\n", ftell(outfile));
       #endif

       /* remove old value from k*/
       k=0;
       /* count number of line in uncompressed stream*/
       for(k=0;data[k];k++)
       {
       if(data[k]=='\n'){PIA_uncompressed_line_number++;}
       }
       #if DEBUG
       printf("uncompressed data splitted in %d lines\n", PIA_uncompressed_line_number);
       #endif
    }

    /* close input and output file*/
    gzclose(infile);
    fclose(outfile);
 }

 /* funtion for read CTB or STB uncompressed text form file*/
 int plot_style_parser(char *infilename)
 {
    /* open decompressed PIA file, this is very stupid, there is the data string,
    but I don't know how analyze it... needs more C abilities*/
    FILE *infile = fopen(infilename, "rb");

    /* Parse ctb file*/
    /* first line, description, can contain space*/
    fgets(line_buffer,sizeof(line_buffer),infile);
    sscanf(line_buffer,"description=%[^\n]",&file_description);
    /* second line, aci_table_available, always TRUE*/
    fgets(line_buffer,sizeof(line_buffer),infile);
    sscanf(line_buffer,"aci_table_available=%c",&aci_table_available);
    /* third line, scale_factor*/
    fgets(line_buffer,sizeof(line_buffer),infile);
    sscanf(line_buffer,"scale_factor=%f",&scale_factor);
    /* fourth line, apply factor*/
    fgets(line_buffer,sizeof(line_buffer),infile);
    sscanf(line_buffer,"apply_factor=%c",&apply_factor);
    /* fifth line, custom_lineweight_display_units*/
    fgets(line_buffer,sizeof(line_buffer),infile);
    sscanf(line_buffer,"custom_lineweight_display_units=%d",&custom_lineweight_display_units);


    #if DEBUG
    printf("description=%s\n",file_description);
    if (aci_table_available==84)
    {
    printf("aci_table_available=TRUE\n");
    }
    else if (aci_table_available==70)
    {
    printf("aci_table_available=FALSE\n");
    }
    else
    {
    printf("aci_table_available=%d\n",aci_table_available);
    }
    printf("scale_factor=%1.1f\n",scale_factor);
    if (apply_factor==84)
    {
    printf("apply_factor=TRUE\n");
    }
    else if (apply_factor==70)
    {
    printf("apply_factor=FALSE\n");
    }
    else
    {
    printf("apply_factor=%d\n",apply_factor);
    }
    printf("custom_lineweight_display_units=%d\n",custom_lineweight_display_units);
    #endif

    /* if aci_table_available is set to TRUE (ctb) then read aci_table */
    if (aci_table_available==84)
    {
       /* only for ctb line 6 to 262 read aci_table as matrix*/
       fgets(line_buffer,sizeof(line_buffer),infile);
       if(line_buffer[0]=='a' && line_buffer[1]=='c' && line_buffer[2]=='i'&& line_buffer[3]=='_' && line_buffer[4]=='t' && line_buffer[5]=='a')
       {
       }
       else
       {
       #if DEBUG
       printf("aci_table on wrong line\n");
       printf("%s\n",line_buffer);
       #endif
       }

        /* remove old value from k*/
        k=0;
       /* only for ctb get line 7 to 261 (255 values)*/
       for(k=0;k<255;k++)
       {
       /* stupid code, try new approach*/
       fgets(aci_table[k],20,infile);
       #if DEBUG
       printf("%s",aci_table[k]);
       #endif
       }
       /* only for ctb line 262 end of aci_table*/
       fgets(line_buffer,sizeof(line_buffer),infile);
       if(line_buffer[0]=='}')
       {
       }
       else
       {
       fprintf(stderr,"end of aci_table on wrong line\n");
       fprintf(stderr,"expected } obtained %c\n",line_buffer[0]);
       }
	}

    /* start of plot_style sctruct (line 263 on ctb, line 6 on stb)*/
    fgets(line_buffer,sizeof(line_buffer),infile);
    if(line_buffer[0]=='p' && line_buffer[1]=='l' && line_buffer[2]=='o'&& line_buffer[3]=='t' && line_buffer[4]=='_' && line_buffer[5]=='s')
    {
    }
    else
    {
    #if DEBUG
    printf("plot_style on wrong line\n");
    printf("%s\n",line_buffer);
    #endif
    }

    /* first line in plot_style the next lines are readen on for loop*/
    fgets(line_buffer,sizeof(line_buffer),infile);

    /* remove old value from k*/
    k=0;
    /* remove old value from total_style_number*/
    total_style_number=1;
    for(k=0;k<total_style_number;k++)
    {
       /* color/named plot style value color init (number)*/
       /* unused, is always equal to k*/

       /* color/named plot style value name*/
       fgets(line_buffer,sizeof(line_buffer),infile);
       sscanf(line_buffer,"  name=%[^\n]",&name[k]);
       #if DEBUG
       printf("name=%s\n",name[k]);
       #endif
       /* color/named plot style value localized_name*/
       fgets(line_buffer,sizeof(line_buffer),infile);
       sscanf(line_buffer,"  localized_name=%[^\n]",&localized_name[k]);
       #if DEBUG
       printf("localized_name=%s\n",localized_name[k]);
       #endif
       /* color/named plot style value description*/
       fgets(line_buffer,sizeof(line_buffer),infile);
       sscanf(line_buffer,"  description=%[^\n]",&description[k]);
       #if DEBUG
       printf("description=%s\n",description[k]);
       #endif
       /* color/named plot style value color*/
       fgets(line_buffer,sizeof(line_buffer),infile);
       sscanf(line_buffer,"  color=%d",&color[k]);
       #if DEBUG
       printf("color=%d\n",color[k]);
       #endif
       /* get the string that can contain mode_color or color_policy*/
       fgets(line_buffer,sizeof(line_buffer),infile);
       /* Verify if mode_color is present*/
       if (line_buffer[2]=='m' && line_buffer[3]=='o' && line_buffer[4]=='d' && line_buffer[5]=='e' && line_buffer[6]=='_' && line_buffer[7]=='c')
       {
          /* color/named plot style value mode_color*/
          sscanf(line_buffer,"  mode_color=%d",&mode_color[k]);
          #if DEBUG
          printf("mode_color=%d\n",mode_color[k]);
          #endif
          /* get the string that contain color_policy*/
          fgets(line_buffer,sizeof(line_buffer),infile);
       }
       /* color/named plot style value color_policy*/
       sscanf(line_buffer,"  color_policy=%d",&color_policy[k]);
       #if DEBUG
       printf("color_policy=%d\n",color_policy[k]);
       #endif
       /* color/named plot style value physical_pen_number*/
       fgets(line_buffer,sizeof(line_buffer),infile);
       sscanf(line_buffer,"  physical_pen_number=%d",&physical_pen_number[k]);
       #if DEBUG
       printf("physical_pen_number=%d\n",physical_pen_number[k]);
       #endif
       /* color/named plot style value virtual_pen_number*/
       fgets(line_buffer,sizeof(line_buffer),infile);
       sscanf(line_buffer," virtual_pen_number=%d",&virtual_pen_number[k]);
       #if DEBUG
       printf("virtual_pen_number=%d\n",virtual_pen_number[k]);
       #endif
       /* color/named plot style value screen*/
       fgets(line_buffer,sizeof(line_buffer),infile);
       sscanf(line_buffer," screen=%d",&screen[k]);
       #if DEBUG
       printf("screen=%d\n",screen[k]);
       #endif
       /* color/named plot style value linepattern_size*/
       fgets(line_buffer,sizeof(line_buffer),infile);
       sscanf(line_buffer," linepattern_size=%f",&linepattern_size[k]);
       #if DEBUG
       printf("linepattern_size=%1.1f\n",linepattern_size[k]);
       #endif
       /* color/named plot style value linetype*/
       fgets(line_buffer,sizeof(line_buffer),infile);
       sscanf(line_buffer," linetype=%d",&linetype[k]);
       #if DEBUG
       printf("linetype=%d\n",linetype[k]);
       #endif
       /* color/named plot style value adaptive_linetype (TRUE or FALSE)*/
       fgets(line_buffer,sizeof(line_buffer),infile);
       sscanf(line_buffer," adaptive_linetype=%c",&adaptive_linetype[k]);
       #if DEBUG
       if (adaptive_linetype[k]==84)
       {
       printf("adaptive_linetype=TRUE\n");
       }
       else if (adaptive_linetype[k]==70)
       {
       printf("adaptive_linetype=FALSE\n");
       }
       else
       {
       printf("adaptive_linetype=%d\n",adaptive_linetype[k]);
       }
       #endif
       /* color/named plot style value lineweight*/
       fgets(line_buffer,sizeof(line_buffer),infile);
       sscanf(line_buffer," lineweight=%d",&lineweight[k]);
       #if DEBUG
       printf("lineweight=%d\n",lineweight[k]);
       #endif
       /* color/named plot style value fill_style*/
       fgets(line_buffer,sizeof(line_buffer),infile);
       sscanf(line_buffer," fill_style=%d",&fill_style[k]);
       #if DEBUG
       printf("fill_style=%d\n",fill_style[k]);
       #endif
       /* color/named plot style value end_style*/
       fgets(line_buffer,sizeof(line_buffer),infile);
       sscanf(line_buffer," end_style=%d",&end_style[k]);
       #if DEBUG
       printf("end_style=%d\n",end_style[k]);
       #endif
       /* color/named plot style value join_style*/
       fgets(line_buffer,sizeof(line_buffer),infile);
       sscanf(line_buffer," join_style=%d",&join_style[k]);
       #if DEBUG
       printf("join_style=%d\n",join_style[k]);
       #endif
       /* color/named plot style value end of color }*/
       fgets(line_buffer,sizeof(line_buffer),infile);
       if (line_buffer[1]=='}')
       {
       }
       else
       {
          fprintf(stderr, "ERROR, on plot style number %d\n", k);
          fprintf(stderr, "expected \"}\" obtained \"%c\"\n", line_buffer[1]);
       }
       /* read the end of plot_style or the next color/named plot style value color init (number)*/
       fgets(line_buffer,sizeof(line_buffer),infile);
       /* on stb the plot_style number is variable, so it's better to verify the MAX_STYLE number*/
       if (line_buffer[0]=='}')
       {
          /* this is the end of plot_style*/
          total_style_number=k+1;
       }
       /* this is valid only for STB*/
       else if (line_buffer[0]!='}' && header[19]=='S' && header[20]=='T' && header[21]=='B' && total_style_number<MAX_STYLE)
       {
          /* there is another plot style*/
          total_style_number++;
       }
       /* this is valid only for CTB*/
       else if (line_buffer[0]!='}' && header[19]=='C' && header[20]=='T' && header[21]=='B' && total_style_number<255)
       {
          /* there is another plot style*/
          total_style_number++;
       }
       /* verify correct end for CTB*/
       else if (header[19]=='C' && header[20]=='T' && header[21]=='B' && k==254 && line_buffer[0]!='}')
       {
          fprintf(stderr, "end of plot_style, expected \"}\" obtained \"%c\"\n", line_buffer[0]);
       }
    }

    /* read custom_lineweight_table*/
    fgets(line_buffer,sizeof(line_buffer),infile);
    if (line_buffer[0]=='c' && line_buffer[1]=='u' && line_buffer[2]=='s' && line_buffer[3]=='t' && line_buffer[4]=='o' && line_buffer[5]=='m')
    {
    }
    else
    {
       fprintf(stderr, "ERROR!\nexpected: \"custom_lineweight_table{\"\nobtained: \"%s\"", line_buffer);
    }
    /* read values from custom_lineweight_table*/
    fgets(line_buffer,sizeof(line_buffer),infile);
    sscanf(line_buffer," 0=%f",&custom_lineweight_table[0]);
    fgets(line_buffer,sizeof(line_buffer),infile);
    sscanf(line_buffer," 1=%f",&custom_lineweight_table[1]);
    fgets(line_buffer,sizeof(line_buffer),infile);
    sscanf(line_buffer," 2=%f",&custom_lineweight_table[2]);
    fgets(line_buffer,sizeof(line_buffer),infile);
    sscanf(line_buffer," 3=%f",&custom_lineweight_table[3]);
    fgets(line_buffer,sizeof(line_buffer),infile);
    sscanf(line_buffer," 4=%f",&custom_lineweight_table[4]);
    fgets(line_buffer,sizeof(line_buffer),infile);
    sscanf(line_buffer," 5=%f",&custom_lineweight_table[5]);
    fgets(line_buffer,sizeof(line_buffer),infile);
    sscanf(line_buffer," 6=%f",&custom_lineweight_table[6]);
    fgets(line_buffer,sizeof(line_buffer),infile);
    sscanf(line_buffer," 7=%f",&custom_lineweight_table[7]);
    fgets(line_buffer,sizeof(line_buffer),infile);
    sscanf(line_buffer," 8=%f",&custom_lineweight_table[8]);
    fgets(line_buffer,sizeof(line_buffer),infile);
    sscanf(line_buffer," 9=%f",&custom_lineweight_table[9]);
    fgets(line_buffer,sizeof(line_buffer),infile);
    sscanf(line_buffer," 10=%f",&custom_lineweight_table[10]);
    fgets(line_buffer,sizeof(line_buffer),infile);
    sscanf(line_buffer," 11=%f",&custom_lineweight_table[11]);
    fgets(line_buffer,sizeof(line_buffer),infile);
    sscanf(line_buffer," 12=%f",&custom_lineweight_table[12]);
    fgets(line_buffer,sizeof(line_buffer),infile);
    sscanf(line_buffer," 13=%f",&custom_lineweight_table[13]);
    fgets(line_buffer,sizeof(line_buffer),infile);
    sscanf(line_buffer," 14=%f",&custom_lineweight_table[14]);
    fgets(line_buffer,sizeof(line_buffer),infile);
    sscanf(line_buffer," 15=%f",&custom_lineweight_table[15]);
    fgets(line_buffer,sizeof(line_buffer),infile);
    sscanf(line_buffer," 16=%f",&custom_lineweight_table[16]);
    fgets(line_buffer,sizeof(line_buffer),infile);
    sscanf(line_buffer," 17=%f",&custom_lineweight_table[17]);
    fgets(line_buffer,sizeof(line_buffer),infile);
    sscanf(line_buffer," 18=%f",&custom_lineweight_table[18]);
    fgets(line_buffer,sizeof(line_buffer),infile);
    sscanf(line_buffer," 19=%f",&custom_lineweight_table[19]);
    fgets(line_buffer,sizeof(line_buffer),infile);
    sscanf(line_buffer," 20=%f",&custom_lineweight_table[20]);
    fgets(line_buffer,sizeof(line_buffer),infile);
    sscanf(line_buffer," 21=%f",&custom_lineweight_table[21]);
    fgets(line_buffer,sizeof(line_buffer),infile);
    sscanf(line_buffer," 22=%f",&custom_lineweight_table[22]);
    fgets(line_buffer,sizeof(line_buffer),infile);
    sscanf(line_buffer," 23=%f",&custom_lineweight_table[23]);
    fgets(line_buffer,sizeof(line_buffer),infile);
    sscanf(line_buffer," 24=%f",&custom_lineweight_table[24]);
    fgets(line_buffer,sizeof(line_buffer),infile);
    sscanf(line_buffer," 25=%f",&custom_lineweight_table[25]);
    fgets(line_buffer,sizeof(line_buffer),infile);
    sscanf(line_buffer," 26=%f",&custom_lineweight_table[26]);
    /* read the end of custom_lineweight_table*/
    fgets(line_buffer,sizeof(line_buffer),infile);
    if (line_buffer[0]=='}')
    {
    }
    else
    {
       fprintf(stderr, "end of custom_lineweight_table ERROR, expected \"}\" obtained \"%c\"\n", line_buffer[1]);
    }
    /* close input file*/
    fclose(infile);
 }

 /* funtion for write CTB or STB uncompressed text*/
 int plot_style_writer(char *outfilename)
 {
    /* open outfilename file*/
    FILE *writed = fopen(outfilename, "wb");

    /* Parse ctb or stb file*/
    /* first line, description, can contain space*/
    fprintf(writed,"description=%s\n",file_description);
    /* second line, aci_table_available, always TRUE for ctb*/
    if (aci_table_available==84)
    {
       fprintf(writed,"aci_table_available=TRUE\n");
    }
    /* always FALSE for stb*/
    else if (aci_table_available==70)
    {
       fprintf(writed,"aci_table_available=FALSE\n");
    }
    /* third line, scale_factor*/
    fprintf(writed,"scale_factor=%1.1f\n",scale_factor);
    /* fourth line, apply factor*/
    if (apply_factor==84)
    {
       fprintf(writed,"apply_factor=TRUE\n");
    }
    else if (apply_factor==70)
    {
       fprintf(writed,"apply_factor=FALSE\n");
    }
    /* if impossible value set to default*/
    else
    {
       fprintf(writed,"apply_factor=TRUE\n");
    }
    /* fifth line, custom_lineweight_display_units*/
    fprintf(writed,"custom_lineweight_display_units=%d\n",custom_lineweight_display_units);
    /* if aci_table_available is TRUE (ctb)*/
    if (aci_table_available==84)
    {
       /* line 6 start of aci_table*/
       fprintf(writed,"aci_table{\n");
       /* remove old value from k*/
       k=0;
       /* line 7 to 261 (255 values)*/
       for(k=0;k<255;k++)
       {
          /* write aci_table values*/
          fprintf(writed," %d=%s\n",k,name[k]);
       }
       /* line 262 end of aci_table*/
       fprintf(writed,"}\n");
    }
    /* line 263 start of plot_style struct*/
    fprintf(writed,"plot_style{\n");
    /* remove old value from k*/
    k=0;
    for(k=0;k<total_style_number;k++)
    {
       /* color/named plot style value color init (number)*/
       fprintf(writed," %d{\n",k);
       /* color/named plot style value name*/
       fprintf(writed,"  name=%s\n",name[k]);
       /* color/named plot style value localized_name*/
       fprintf(writed,"  localized_name=%s\n",localized_name[k]);
       /* color/named plot style value description*/
       fprintf(writed,"  description=%s\n",description[k]);
       /* color/named plot style value color*/
       fprintf(writed,"  color=%d\n",color[k]);
       /* Verify if mode_color exist*/
       if (mode_color[k]!='\0')
       {
          /* color/named plot style value mode_color*/
          fprintf(writed,"  mode_color=%d\n",mode_color[k]);
       }
       /* color/named plot style value color_policy*/
       fprintf(writed,"  color_policy=%d\n",color_policy[k]);
       /* color/named plot style value physical_pen_number*/
       fprintf(writed,"  physical_pen_number=%d\n",physical_pen_number[k]);
       /* color/named plot style value virtual_pen_number*/
       fprintf(writed,"  virtual_pen_number=%d\n",virtual_pen_number[k]);
       /* color/named plot style value screen*/
       fprintf(writed,"  screen=%d\n",screen[k]);
       /* color/named plot style value linepattern_size*/
       fprintf(writed,"  linepattern_size=%1.1f\n",linepattern_size[k]);
       /* color/named plot style value linetype*/
       fprintf(writed,"  linetype=%d\n",linetype[k]);
       /* color/named plot style value adaptive_linetype (TRUE or FALSE)*/
       if (adaptive_linetype[k]==84)
       {
       fprintf(writed,"  adaptive_linetype=TRUE\n");
       }
       else if (adaptive_linetype[k]==70)
       {
       fprintf(writed,"  adaptive_linetype=FALSE\n");
       }
       /* if impossible value set to default*/
       else
       {
       fprintf(writed,"  adaptive_linetype=TRUE\n");
       }
       /* color/named plot style value lineweight*/
       fprintf(writed,"  lineweight=%d\n",lineweight[k]);
       /* color/named plot style value fill_style*/
       fprintf(writed,"  fill_style=%d\n",fill_style[k]);
       /* color/named plot style value end_style*/
       fprintf(writed,"  end_style=%d\n",end_style[k]);
       /* color/named plot style value join_style*/
       fprintf(writed,"  join_style=%d\n",join_style[k]);
       /* color/named plot style value end of color }*/
       fprintf(writed," }\n");
    }
    /* write the end of plot_style*/
    fprintf(writed,"}\n");
    /* write custom_lineweight_table*/
    fprintf(writed,"custom_lineweight_table{\n");
    /* write values from custom_lineweight_table*/
    k=0;
    for(k=0;k<=26;k++)
    {
       /* custom_lineweight_table have a curious significant digits mode not totaly compatible with printf %g*/
       if(custom_lineweight_table[k]==0.0 || custom_lineweight_table[k]==1.0 || custom_lineweight_table[k]==2.0 || custom_lineweight_table[k]==3.0)
       {
          fprintf(writed," %d=%1.1f\n",k,custom_lineweight_table[k]);
       }
       else
       {
          fprintf(writed," %d=%g\n",k,custom_lineweight_table[k]);
       }
    }
    /* write the end of custom_lineweight_table*/
    fprintf(writed,"}\n");
    /* close output file*/
    fclose(writed);
 }

 /* funtion for zip PIA file*/
 int write_PIA(char *infilename, char *outfilename)
 {
    /* open compressed PIA file*/
    FILE *infile = fopen(infilename, "rb");
    /* create uncompressed file*/
    FILE *outfile = fopen(outfilename, "wb");
    /* verify if files exist*/
    if (!infile || !outfile) return -1;

    /* move to the end of input file*/
    fseek(infile, 0, SEEK_END);
    /* obtain information of input file size (uncompressed text form)*/
    input_file_size = ftell(infile);
    /* move to the start of input file*/
    fseek(infile, 0, SEEK_SET);

    /* declare buffer, buffer size is the compressed size, actally I don't know how predict this size...
    so I simply assumed that is similar to the readed_compressed_size. Need improvement*/
    char buffer[readed_compressed_size];
    /* declare data, data size is equal to decompressed size (the size of the input txt file)*/
    char data[input_file_size];
    /* number of byte readed*/
    int num_read = 0;

    /* read input compressed data file*/
    while ((num_read = fread(data, sizeof(char), input_file_size, infile)) > 0)
    {
       /* deflate buffer into data*/
       /* zlib struct*/
       z_stream defstream;
       defstream.zalloc = Z_NULL;
       defstream.zfree = Z_NULL;
       defstream.opaque = Z_NULL;

       /* setup "buffer" as the compressed output and "data" as the decompressed input*/
       /* size of input plus terminator*/
       defstream.avail_in = input_file_size+1;
       /* input char array*/
       defstream.next_in = (Bytef *)data;
       /* size of output*/
       defstream.avail_out = (uInt)sizeof(buffer);
       /* output char array*/
       defstream.next_out = (Bytef *)buffer;

       /* the real compression work, this seems to be different from original deflate compression, need more investigations*/
       deflateInit(&defstream, Z_DEFAULT_COMPRESSION);
       deflate(&defstream, Z_FINISH);
       deflateEnd(&defstream);

       /* obtain information of decompressed size*/
       writed_decompressed_size = defstream.total_in;

       /* obtain information of compressed size*/
       writed_compressed_size = defstream.total_out;

       /* calculate Adler32*/
       writed_Adler32 = adler32(0, data, input_file_size);
	   
       /* first 48 bytes = header (not compressed, composed by "PIAFILEVERSION_2.0,???VER1,compress/r/npmzlibcodec)
       next 4 bytes Adler32 checksum
       next 4 bytes (unsigned int) decompressed stream size
       next 4 bytes (unsigned int) compressed stream size*/
       /* write PIA header (bytes number 0 to 48)*/
       fprintf(outfile,"PIAFILEVERSION_2.0,%c%c%cVER1,compress\r\npmzlibcodec", header[19], header[20], header[21]);

       /* divide adler32 in 4 bytes*/
       checksum[3] = (writed_Adler32 >> 24) & 0xFF;
       checksum[2] = (writed_Adler32 >> 16) & 0xFF;
       checksum[1] = (writed_Adler32 >> 8) & 0xFF;
       checksum[0] = writed_Adler32 & 0xFF;

       /* write adler 32 (bytes number 49-50-51-52)*/
       fprintf(outfile,"%c%c%c%c", checksum[0], checksum[1], checksum[2], checksum[3]);

       /* divide uncompressed size in 4 bytes*/
       checksum[7] = (writed_decompressed_size >> 24) & 0xFF;
       checksum[6] = (writed_decompressed_size >> 16) & 0xFF;
       checksum[5] = (writed_decompressed_size >> 8) & 0xFF;
       checksum[4] = writed_decompressed_size & 0xFF;

       /* write uncompressed size (bytes number 53-54-55-56)*/
       fprintf(outfile,"%c%c%c%c", checksum[4], checksum[5], checksum[6], checksum[7]);

       /* divide compressed size in 4 bytes*/
       checksum[11] = (writed_compressed_size >> 24) & 0xFF;
       checksum[10] = (writed_compressed_size >> 16) & 0xFF;
       checksum[9] = (writed_compressed_size >> 8) & 0xFF;
       checksum[8] = writed_compressed_size & 0xFF;

       /* write compressed size (bytes number 57-58-59-60)*/
       fprintf(outfile,"%c%c%c%c", checksum[8], checksum[9], checksum[10], checksum[11]);
	   
       /* write compressed data*/
       fwrite(buffer, sizeof(char), writed_compressed_size, outfile);

       /* debug*/
       #if DEBUG
       printf("readed %d decompressed bytes, expected %d bytes\n", writed_decompressed_size, input_file_size);
       printf("writed %d compressed bytes\n", writed_compressed_size);
       #endif
    }

    /* close input and output file*/
    fclose(outfile);
    fclose(infile);
 }

 /* funtion for create a new default CTB*/
 int new_ctb()
 {
    /* declare ctb file*/
    header[19]='C';
    header[20]='T';
    header[21]='B';
    readed_compressed_size=4046;
    /* set total_style_number to 255 (in CTB are always 255)*/
    total_style_number=255;
    /* first line, description, can contain space*/
    sprintf( file_description, "\"");
    /* second line, aci_table_available, always TRUE for ctb*/
    aci_table_available=84;
    /* third line, scale_factor*/
    scale_factor=1.0;
    /* fourth line, apply factor*/
    apply_factor=70;
    /* fifth line, custom_lineweight_display_units*/
    custom_lineweight_display_units=0;
    /* line 263 start of plot_style struct*/
    /* remove old value from k*/
    k=0;
    /* loop for declare all colours*/
    for(k=0; k<total_style_number; k++)
    {
       /* color plot style value name*/
       sprintf( name[k], "\"Color_%d", k+1);
       /* color plot style value localized_name*/
       sprintf(localized_name[k], "\"Color_%d", k+1);
       /* color plot style value description*/
       sprintf( description[k], "\"");
       /* color plot style value color*/
       color[k]=-1;
       /* color plot style mode_color for default dosn't exist*/
       /* color plot style value color_policy*/
       color_policy[k]=1;
       /* color plot style value physical_pen_number*/
       physical_pen_number[k]=0;
       /* color plot style value virtual_pen_number*/
       virtual_pen_number[k]=0;
       /* color plot style value screen*/
       screen[k]=100;
       /* color plot style value linepattern_size*/
       linepattern_size[k]=0.5;
       /* color plot style value linetype*/
       linetype[k]=31;
       /* color plot style value adaptive_linetype (TRUE or FALSE)*/
       adaptive_linetype[k]=84;
       /* color plot style value lineweight*/
       lineweight[k]=0;
       /* color plot style value fill_style*/
       fill_style[k]=73;
       /* color plot style value end_style*/
       end_style[k]=4;
       /* color plot style value join_style*/
       join_style[k]=5;
    }
    /* values for custom_lineweight_table*/
    custom_lineweight_table[0]=0.0;
    custom_lineweight_table[1]=0.05;
    custom_lineweight_table[2]=0.09;
    custom_lineweight_table[3]=0.1;
    custom_lineweight_table[4]=0.13;
    custom_lineweight_table[5]=0.15;
    custom_lineweight_table[6]=0.18;
    custom_lineweight_table[7]=0.2;
    custom_lineweight_table[8]=0.25;
    custom_lineweight_table[9]=0.3;
    custom_lineweight_table[10]=0.35;
    custom_lineweight_table[11]=0.4;
    custom_lineweight_table[12]=0.45;
    custom_lineweight_table[13]=0.5;
    custom_lineweight_table[14]=0.53;
    custom_lineweight_table[15]=0.6;
    custom_lineweight_table[16]=0.65;
    custom_lineweight_table[17]=0.7;
    custom_lineweight_table[18]=0.8;
    custom_lineweight_table[19]=0.9;
    custom_lineweight_table[20]=1.0;
    custom_lineweight_table[21]=1.06;
    custom_lineweight_table[22]=1.2;
    custom_lineweight_table[23]=1.4;
    custom_lineweight_table[24]=1.58;
    custom_lineweight_table[25]=2.0;
    custom_lineweight_table[26]=2.11;
 }

 /* funtion for write a new default STB uncompressed text*/
 int new_stb()
 {
    /* declare stb file*/
    header[19]='S';
    header[20]='T';
    header[21]='B';
    readed_compressed_size=382;
    /* declare number of plot styles*/
    total_style_number=2;
    /* first line, description, can contain space*/
    sprintf( file_description, "\"");
    /* second line, aci_table_available, always FALSE for stb*/
    aci_table_available=70;
    /* third line, scale_factor*/
    scale_factor=1.0;
    /* fourth line, apply factor*/
    apply_factor=70;
    /* fifth line, custom_lineweight_display_units*/
    custom_lineweight_display_units=0;
    /* named plot style value name*/
    sprintf( name[0], "\"Normal");
    /* named plot style value localized_name*/
    sprintf( localized_name[0], "\"Normal");
    /* named plot style value description*/
    sprintf( description[0], "\"");
    /* named plot style value color*/
    color[0]=-1;
    /* named plot style mode_color for default dosn't exist*/
    /* named plot style value color_policy*/
    color_policy[0]=1;
    /* named plot style value physical_pen_number*/
    physical_pen_number[0]=0;
    /* named plot style value virtual_pen_number*/
    virtual_pen_number[0]=0;
    /* named plot style value screen*/
    screen[0]=100;
    /* named plot style value linepattern_size*/
    linepattern_size[0]=0.5;
    /* named plot style value linetype*/
    linetype[0]=31;
    /* named plot style value adaptive_linetype (set to TRUE)*/
    adaptive_linetype[0]=84;
    /* named plot style value lineweight*/
    lineweight[0]=0;
    /* named plot style value fill_style*/
    fill_style[0]=73;
    /* named plot style value end_style*/
    end_style[0]=4;
    /* named plot style value join_style*/
    join_style[0]=5;
    /* named plot style value name*/
    sprintf( name[1], "\"Style_1");
    /* named plot style value localized_name*/
    sprintf( localized_name[1], "\"Style 1");
    /* named plot style value description*/
    sprintf( description[1], "\"");
    /* named plot style value color*/
    color[1]=-1;
    /* named plot style mode_color for default dosn't exist*/
    /* named plot style value color_policy*/
    color_policy[1]=1;
    /* named plot style value physical_pen_number*/
    physical_pen_number[1]=0;
    /* named plot style value virtual_pen_number*/
    virtual_pen_number[1]=0;
    /* named plot style value screen*/
    screen[1]=100;
    /* named plot style value linepattern_size*/
    linepattern_size[1]=0.5;
    /* named plot style value linetype*/
    linetype[1]=31;
    /* named plot style value adaptive_linetype (set to TRUE)*/
    adaptive_linetype[1]=84;
    /* named plot style value lineweight*/
    lineweight[1]=0;
    /* named plot style value fill_style*/
    fill_style[1]=73;
    /* named plot style value end_style*/
    end_style[1]=4;
    /* named plot style value join_style*/
    join_style[1]=5;
    /* values for custom_lineweight_table*/
    custom_lineweight_table[0]=0.0;
    custom_lineweight_table[1]=0.05;
    custom_lineweight_table[2]=0.09;
    custom_lineweight_table[3]=0.1;
    custom_lineweight_table[4]=0.13;
    custom_lineweight_table[5]=0.15;
    custom_lineweight_table[6]=0.18;
    custom_lineweight_table[7]=0.2;
    custom_lineweight_table[8]=0.25;
    custom_lineweight_table[9]=0.3;
    custom_lineweight_table[10]=0.35;
    custom_lineweight_table[11]=0.4;
    custom_lineweight_table[12]=0.45;
    custom_lineweight_table[13]=0.5;
    custom_lineweight_table[14]=0.53;
    custom_lineweight_table[15]=0.6;
    custom_lineweight_table[16]=0.65;
    custom_lineweight_table[17]=0.7;
    custom_lineweight_table[18]=0.8;
    custom_lineweight_table[19]=0.9;
    custom_lineweight_table[20]=1.0;
    custom_lineweight_table[21]=1.06;
    custom_lineweight_table[22]=1.2;
    custom_lineweight_table[23]=1.4;
    custom_lineweight_table[24]=1.58;
    custom_lineweight_table[25]=2.0;
    custom_lineweight_table[26]=2.11;
 }

 /* funtion for add a plot style on STB*/
 int add_plot_style_stb()
 {
    /* verify if can added a plot style on STB, this is needed because of a bug, need improvement*/
    if(total_style_number<MAX_STYLE)
    {
       /* set k to actual_total_style*/
       k=total_style_number;
       /* increase plot_style_number (add a plot style)*/
       total_style_number++;
       /* increase readed_compressed_size, need improvement*/
       readed_compressed_size=382+total_style_number*14;
       /* named plot style value name*/
       sprintf( name[k], "\"Style_%d", k);
       /* named plot style value localized_name*/
       sprintf( localized_name[k], "\"Style %d",k);
       /* named plot style value description*/
       sprintf( description[k], "\"");
       /* named plot style value color*/
       color[k]=-1;
       /* Verify if mode_color exist on precedent plot style*/
       if (mode_color[k-1]!='\0')
       {
          /* named plot style value mode_color exist, set to color*/
          mode_color[k]=color[k];
       }
       /* named plot style value color_policy*/
       color_policy[k]=1;
       /* named plot style value physical_pen_number*/
       physical_pen_number[k]=0;
       /* named plot style value virtual_pen_number*/
       virtual_pen_number[k]=0;
       /* named plot style value screen*/
       screen[k]=100;
       /* named plot style value linepattern_size*/
       linepattern_size[k]=0.5;
       /* named plot style value linetype*/
       linetype[k]=31;
       /* named plot style value adaptive_linetype (TRUE or FALSE)*/
       adaptive_linetype[k]=84;
       /* named plot style value lineweight*/
       lineweight[k]=0;
       /* named plot style value fill_style*/
       fill_style[k]=73;
       /* named plot style value end_style*/
       end_style[k]=4;
       /* named plot style value join_style*/
       join_style[k]=5;
    }
    /* if can't added a plot style on STB*/
    else
    {
       fprintf(stderr,"actually isn't possible to have an STB with more of %s plot styles, this is my fault, sorry...", MAX_STYLE);
    }

 }


 /* funtion for remove a plot style on STB*/
 int remove_plot_style_stb()
 {
    /* verify if is an STB*/
    if(header[19]=='S' && header[20]=='T' && header[21]=='B')
    {
       /* Input element position to delete */
       printf("Plot style position minimum \"1\", maximum \"%d\".\n", total_style_number);
       printf("Enter the plot style position number to delete : ");
       scanf("%d", &position);


       /* Invalid delete position */
       if(position <= 0 || position > total_style_number)
       {
           fprintf(stderr,"Invalid number! Please enter position between 1 to %d.\n", total_style_number);
       }
       else
       {
           /* copy next plot style value to current value*/
           for(k=position-1; k<total_style_number-1; k++)
           {
               /* named plot style value name*/
               name[k]==name[k+1];
               /* named plot style value localized_name*/
               localized_name[k]==localized_name[k+1];
               /* named plot style value description*/
               description[k]==description[k+1];
               /* named plot style value color*/
               color[k]=color[k+1];
               /* named plot style mode_color if exist*/
               if (mode_color[k]!='\0')
               {
                  mode_color[k]=mode_color[k+1];
               }
               /* named plot style value color_policy*/
               color_policy[k]=color_policy[k+1];
               /* named plot style value physical_pen_number*/
               physical_pen_number[k]=physical_pen_number[k+1];
               /* named plot style value virtual_pen_number*/
               virtual_pen_number[k]=virtual_pen_number[k+1];
               /* named plot style value screen*/
               screen[k]= screen[k+1];
               /* named plot style value linepattern_size*/
               linepattern_size[k]=linepattern_size[k+1];
               /* named plot style value linetype*/
               linetype[k]=linetype[k+1];
               /* named plot style value adaptive_linetype*/
               adaptive_linetype[k]=adaptive_linetype[k+1];
               /* named plot style value lineweight*/
               lineweight[k]=lineweight[k+1];
               /* named plot style value fill_style*/
               fill_style[k]=fill_style[k+1];
               /* named plot style value end_style*/
               end_style[k]=end_style[k+1];
               /* named plot style value join_style*/
               join_style[k]=join_style[k+1];
           }
           /* delte last array number (the value will be "lost" on next plot_style_writer function call)*/
           total_style_number--;
       }
    }
    else
    {
       fprintf(stderr,"the file isn't an STB, plot style can't be removed on CTB");
    }
 }

