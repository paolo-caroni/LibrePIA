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
#include "plot_style_table.h"

/* declare header and info common to all PIA files*/
unsigned char header[37],header2[11],checksum[12],line_buffer[4900];
unsigned int readed_decompressed_size=0, readed_compressed_size=0, readed_Adler32=0;
int k=0,PIA_uncompressed_line_number=0;
/*int PIA_ver=0, subclass_ver=0; unused*/


 /* obtain information of version,file subclass type and sizes*/
 unsigned long read_header(char *infilename)
 {
    /* open compressed PIA file but without zlib*/
    FILE *infile = fopen(infilename, "rb");

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
    while ((num_read = gzread(infile, buffer, sizeof(buffer))) > 0)
    {
       /* inflate buffer into data*/
       /* zlib struct*/
       z_stream infstream;
       infstream.zalloc = Z_NULL;
       infstream.zfree = Z_NULL;
       infstream.opaque = Z_NULL;

       /* setup "buffer" as the input and "data" as the decompressed output*/
       /* size of input*/
       infstream.avail_in = (uInt)sizeof(buffer);
       /* input char array*/
       infstream.next_in = (Bytef *)buffer;
       /* size of output*/
       infstream.avail_out = (uInt)sizeof(data);
       /* output char array*/
       infstream.next_out = (Bytef *)data;
         
       /* the real DE-compression work*/
       inflateInit(&infstream);
       inflate(&infstream, Z_NO_FLUSH);
       inflateEnd(&infstream);

       /* write uncompressed data removing last NULL byte*/
       fwrite(data, 1, readed_decompressed_size-1, outfile);

       /* Obtain information of output file size*/
       unsigned long output_file_size = ftell(outfile);

       /* debug*/
       #if DEBUG
       printf("readed %d compressed bytes\n", num_read);
       printf("writed %d decompressed bytes\n", output_file_size);
       #endif

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
       /* only for ctb get line 7 to 261 (254 values)*/
       for(k=0;k<=254;k++)
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
    for(k=0;k<=max_style;k++)
    {
       /* color/named plot style value color init (number)*/
/* known bug segmentation fault
       sscanf(line_buffer," %3[^{]",style_number[k]);
       //#if DEBUG
       if(k!=style_number[k])
       {
       printf("wrong plot style number, expected \"%d\" found \"%d\"\n", k, style_number[k]);
       }
       //#endif
*/
       /* color/named plot style value name*/
       fgets(line_buffer,sizeof(line_buffer),infile);
       sscanf(line_buffer,"  name=%12s",&name[k]);
       #if DEBUG
       printf("name=%s\n",name[k]);
       #endif
       /* color/named plot style value localized_name*/
       fgets(line_buffer,sizeof(line_buffer),infile);
       sscanf(line_buffer,"  localized_name=%12s",&localized_name[k]);
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
       /* on stb the plot_style number is variable, so it's better to verify the max_style number*/
       if (line_buffer[0]=='}')
       {
          /* this is the end of plot_style*/
          max_style=k;
       }
       else if (k==254 && line_buffer[0]!='}')
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

 /* funtion for write CTB uncompressed text form file*/
 int ctb_writer()
 {
    /* create new uncompressed .txt file*/
    FILE *writed = fopen("output.txt", "wb");

    /* Parse ctb file*/
    /* first line, description, can contain space*/
    fprintf(writed,"description=%s\n",file_description);
    /* second line, aci_table_available, always TRUE*/
    fprintf(writed,"aci_table_available=TRUE\n");
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
    /* line 6 start of aci_table*/
    fprintf(writed,"aci_table{\n");
    /* remove old value from k*/
    k=0;
    /* line 7 to 261 (254 values)*/
    for(k=0;k<=254;k++)
    {
       /* write aci_table values*/
       fprintf(writed," %d=%s\n",k,name[k]);
    }
    /* line 262 end of aci_table*/
    fprintf(writed,"}\n");
    /* line 263 start of plot_style struct*/
    fprintf(writed,"plot_style{\n");
    /* remove old value from k*/
    k=0;
    for(k=0;k<=max_style;k++)
    {
       /* color plot style value color init (number)*/
       fprintf(writed," %d{\n",k);
       /* color plot style value name*/
       fprintf(writed,"  name=%s\n",name[k]);
       /* color plot style value localized_name*/
       fprintf(writed,"  localized_name=%s\n",localized_name[k]);
       /* color plot style value description*/
       fprintf(writed,"  description=%s\n",description[k]);
       /* color plot style value color*/
       fprintf(writed,"  color=%d\n",color[k]);
       /* Verify if mode_color exist*/
       if (mode_color[k]!='\0')
       {
          /* color plot style value mode_color*/
          fprintf(writed,"  mode_color=%d\n",mode_color[k]);
       }
       /* color plot style value color_policy*/
       fprintf(writed,"  color_policy=%d\n",color_policy[k]);
       /* color plot style value physical_pen_number*/
       fprintf(writed,"  physical_pen_number=%d\n",physical_pen_number[k]);
       /* color plot style value virtual_pen_number*/
       fprintf(writed,"  virtual_pen_number=%d\n",virtual_pen_number[k]);
       /* color plot style value screen*/
       fprintf(writed,"  screen=%d\n",screen[k]);
       /* color plot style value linepattern_size*/
       fprintf(writed,"  linepattern_size=%1.1f\n",linepattern_size[k]);
       /* color plot style value linetype*/
       fprintf(writed,"  linetype=%d\n",linetype[k]);
       /* color plot style value adaptive_linetype (TRUE or FALSE)*/
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
       /* color plot style value lineweight*/
       fprintf(writed,"  lineweight=%d\n",lineweight[k]);
       /* color plot style value fill_style*/
       fprintf(writed,"  fill_style=%d\n",fill_style[k]);
       /* color plot style value end_style*/
       fprintf(writed,"  end_style=%d\n",end_style[k]);
       /* color plot style value join_style*/
       fprintf(writed,"  join_style=%d\n",join_style[k]);
       /* color plot style value end of color }*/
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

 /* proof of concept for decompress PIA file in a text form,
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
       /* write other txt for ctb*/
       ctb_writer();
    }

    else if (header[19]=='S' && header[20]=='T' && header[21]=='B')
    {
       /* parse stb*/
       plot_style_parser(argv[2]);
       fprintf(stderr, "Sorry, the .stb subclass type isn't yet debugged\n");
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
 }


