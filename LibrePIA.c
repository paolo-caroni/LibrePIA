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

/* Include dependancy*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <zlib.h>

/* Declare header and info*/
unsigned char header[37],header2[11],checksum[12];
unsigned int decompressed_size=0, compressed_size=0;
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

    /* decompressed_size set to the combination of the number 52-53-54-55 bytes*/
    decompressed_size = (decompressed_size << 8) + checksum[7];
    decompressed_size = (decompressed_size << 8) + checksum[6];
    decompressed_size = (decompressed_size << 8) + checksum[5];
    decompressed_size = (decompressed_size << 8) + checksum[4];

    /* compressed_size set to the combination of the number 56-57-58-59 bytes*/
    compressed_size = (compressed_size << 8) + checksum[11];
    compressed_size = (compressed_size << 8) + checksum[10];
    compressed_size = (compressed_size << 8) + checksum[9];
    compressed_size = (compressed_size << 8) + checksum[8];

    /* debug function*/
    #if DEBUG
    printf("expected compressed size: %d\n", compressed_size);
    printf("expected decompressed size: %d\n", decompressed_size-48-4-4-4);
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
    char buffer[compressed_size];
    /* declare data, data size is equal to decompressed size without header(48byte), Adler 32 checksum(4byte), 
    decompressed size (4byte) and compressed size(4byte), in total (60byte)*/
    char data[decompressed_size-48-4-4-4];
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

       /* write uncompressed data*/
       fwrite(data, 1, decompressed_size-60, outfile);

       /* Obtain information of output file size*/
       unsigned long output_file_size = ftell(outfile);

       /* debug*/
       #if DEBUG
       printf("readed %d compressed bytes\n", num_read);
       printf("writed %d decompressed bytes\n", output_file_size);
       #endif
    }

    /* close input and output file*/
    gzclose(infile);
    fclose(outfile);
 }

 /* funtion for read CTB uncompressed text form file*/
 int read_ctb(char *infilename)
 {
     fprintf(stderr, "Work in progress...\n");

    /* declare ctb string and matrix*/
    char description[255],aci_table_available[26],scale_factor[18],apply_factor[20],custom_lineweight_display_units[35],aci_table[258][20];
    /* declare k for 255 0r more varialbles*/
    int k=0;

    /* open decompressed PIA file, this is very stupid, there is the data string,
    but I don't know how analyze it... needs more C abilities*/
    FILE *infile = fopen(infilename, "rb");

    /* READ ctb*/
    fgets(description,sizeof(description),infile);
    fgets(aci_table_available,sizeof(aci_table_available),infile);
    fgets(scale_factor,sizeof(scale_factor),infile);
    fgets(apply_factor,sizeof(apply_factor),infile);
    fgets(custom_lineweight_display_units,sizeof(custom_lineweight_display_units),infile);

    printf("%s\n",description);
    printf("%s\n",aci_table_available);
    printf("%s\n",scale_factor);
    printf("%s\n",apply_factor);
    printf("%s\n",custom_lineweight_display_units);

    /* read aci_table as matrix*/
    for(k=0;k<258-1;k++)
    {
    fgets(aci_table[k],20,infile);
    printf("%s",aci_table[k]);
    printf("%d",k);
    }
    /* too difficult to me at the moment*/
 }

 /* proof of concept for decompress PIA file in a text form,
 can be used for all PIA file (ctb, stb, pc3, pmp)*/
 int main(int argc, char **argv)
 {
    read_header(argv[1]);
    decompress_data(argv[1],argv[2]);
    /* Verify subclass type*/
    if (header[19]=='C' && header[20]=='T' && header[21]=='B')
    {
    read_ctb(argv[2]);
    }

    else if (header[19]=='S' && header[20]=='T' && header[21]=='B')
    {
    fprintf(stderr, "Sorry, the .stb subclass type isn't yet supported\n\n");
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


