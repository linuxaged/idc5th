#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "idc.h"

/**********************************************************************
*                                                                      *
*  File: progt_dec.c                                                   *
*  Function:  decodes files created by progt_enc.                      *
*  Author  : K. Sayood                                                 *
*  Last mod: 7/15/95                                                   *
*  Usage:  see usage(), for details see progt_dec.doc or  man page     *
***********************************************************************/

/*******************************************************************************
*NOTICE:                                                                       *
*This code is believed by the author to be bug free.  You are free to use and  *
*modify this code with the understanding that any use, either direct or        *
*derivative, must contain acknowledgement of its author and source.  The author*
*makes no warranty of any kind, expressed or implied, of merchantability or    *
*fitness for a particular purpose.  The author shall not be held liable for any*
*incidental or consequential damages in connection with or arising out of the  *
*furnishing, performance, or use of this software.  This software is not       *
*authorized for use in life support devices or systems.                        *
********************************************************************************/


void usage(void);


int main(int argc, char **argv)
{
    int row, col, row_size, col_size, temp, mode;
    int nrow_size, ncol_size, block_size, M, Mr, Mc, i, j, ave;
    char outimage[50], cmpimage[50];
    unsigned char **image_out, **cmp_image;
    int c;
    FILE *ifp, *ofp;
  extern int  optind;
  extern char *optarg;

  ifp = stdin;
  ofp = stdout;
  mode = 1;
  while((c=getopt(argc,argv,"i:o:h"))!=EOF)
  {
   switch (c){
   case 'i':
         strcpy(cmpimage,optarg);
         ifp = fopen(optarg,"rb");
         break;
   case 'o':
         strcpy(outimage,optarg);
         ofp = fopen(optarg,"wb");
         break;
   case 'h':
         usage();
         exit(1);
              }
   }



    fprintf(stderr,"\n\n\t\tProgressive transmission decoder\n");

   fread(&mode,1,sizeof(int),ifp);
   fread(&block_size,1,sizeof(int),ifp);
   fread(&col_size,1,sizeof(int),ifp);
   fread(&row_size,1,sizeof(int),ifp);


    fprintf(stderr,"\n Image size: %d X %d\n",col_size,row_size);

/* Check to see if the blocksize divides the row and column sizes */

   M = row_size/block_size;
   if(M*block_size < row_size)
    nrow_size = M*block_size + block_size;
   else
    nrow_size = row_size;


   M = col_size/block_size;
   if(M*block_size < col_size)
    ncol_size = M*block_size + block_size;
   else
    ncol_size = col_size;


/* Assign space for the compressed and reconstructed images */

    image_out  = (unsigned char **) calloc(nrow_size,sizeof(char *));

    for(row=0; row<nrow_size; row++)
    {
     image_out[row]  = (unsigned char *) calloc(ncol_size,sizeof(char));
    }

    Mr = nrow_size/block_size;
    cmp_image  = (unsigned char **) calloc(Mr,sizeof(char *));

    Mc = ncol_size/block_size;
    for(row=0; row<Mr; row++)
    {
     cmp_image[row]  = (unsigned char *) calloc(Mc,sizeof(char));
    }

/* Read the compressed file */

    for(row=0; row<Mr; row++)
     for(col=0; col<Mc; col++)
       cmp_image[row][col] = getc(ifp);


          switch(mode) {
           case 1:
/*  Simple minded sampling method of progressive transmission */

     for(row=0; row<nrow_size; row+=block_size)
      for(col=0; col<ncol_size; col+=block_size)
       for(i=0; i<block_size; i++)
        for(j=0; j<block_size; j++)
         image_out[row+i][col+j] = cmp_image[row/block_size][col/block_size];
            break;


           case 2:
/* Simple minded averaging method */

   for(row=0; row<nrow_size; row+=block_size)
      for(col=0; col<ncol_size; col+=block_size)
       for(i=0; i<block_size; i++)
        for(j=0; j<block_size; j++)
         image_out[row+i][col+j] = cmp_image[row/block_size][col/block_size];
            break;

            default:
             fprintf(stderr,"No mode was specified");
             exit(1);
                    }


/* Store reconstructed image  */


    for(row=0; row<row_size; row++)
     for(col=0; col<col_size; col++)
       putc(image_out[row][col],ofp);




   }

void usage(void)
{
  fprintf(stderr,"Usage: progt_dec [-i infile] [-o outfile] [-h]\n");
  fprintf(stderr,"\n\t If the input and output file names are not provided\n");
  fprintf(stderr,"\t the program assumes defaults of standard in and standard out\n");
}
