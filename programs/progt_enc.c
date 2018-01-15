#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "idc.h"

/**********************************************************************
*                                                                      *
*  File: progt_enc.c                                                   *
*  Function:  program to experiment with different approaches to       *
*             progressive transmission.                                *
*  Author  : K. Sayood                                                 *
*  Last mod: 7/15/95                                                   *
*  Usage:  see usage(), for details see progt_enc.doc or man page      *
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
    char inimage[50], cmpimage[50];
    unsigned char **image_in, **cmp_image;
    int c;
    FILE *ifp, *ofp;
  extern int  optind;
  extern char *optarg;

  ifp = stdin;
  ofp = stdout;
  mode = 1;
  row_size = -1;
  col_size = -1;
  while((c=getopt(argc,argv,"i:o:m:b:x:y:h"))!=EOF)
  {
   switch (c){
   case 'i':
         strcpy(inimage,optarg);
         ifp = fopen(optarg,"rb");
         break;
   case 'o':
         strcpy(cmpimage,optarg);
         ofp = fopen(optarg,"wb");
         break;
   case 'm':
         sscanf(optarg,"%d", &mode);
         break;
   case 'b':
         sscanf(optarg,"%d",&block_size);
         break;
   case 'x':
         sscanf(optarg,"%d", &row_size);
         break;
   case 'y':
         sscanf(optarg,"%d", &col_size);
         break;
   case 'h':
         usage();
         exit(1);
              }
   }



    fprintf(stderr,"\n\n\t\tProgressive transmission encoder\n");
    fprintf(stderr,"This program can be used to try different approaches \n");
    fprintf(stderr,"to progressive transmission.  The two modes included\n");
    fprintf(stderr,"here are a subsampling approach (mode 1) and an averaging\n");
    fprintf(stderr,"approach (mode 2).  The selected mode for this particular\n");
    fprintf(stderr,"run is mode %d.  The approximations are for a \n",mode);
    fprintf(stderr,"blocksize of %d and the approximation is stored in %s\n",block_size,cmpimage);
    fprintf(stderr,"In order to obtain further compression, this approximation should\n");
    fprintf(stderr,"be encoded using a variable rate coder.\n");


/* If the image dimensions have not been provided find the image dimensions   */

    if(row_size<0 || col_size<0)
     image_size(inimage, &row_size, &col_size);

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


/* Assign space for the input and residual images */

    image_in  = (unsigned char **) calloc(nrow_size,sizeof(char *));

    for(row=0; row<nrow_size; row++)
    {
     image_in[row]  = (unsigned char *) calloc(ncol_size,sizeof(char));
    }

    Mr = nrow_size/block_size;
    cmp_image  = (unsigned char **) calloc(Mr,sizeof(char *));

    Mc = ncol_size/block_size;
    for(row=0; row<Mr; row++)
    {
     cmp_image[row]  = (unsigned char *) calloc(Mc,sizeof(char));
    }

/* Read the image to be compressed */

    readimage(inimage, image_in, row_size, col_size);

/* If block size does not exactly divide row or column sizes pad the image */

    for(row=row_size; row<nrow_size; row++)
     for(col=0; col<ncol_size; col++)
      image_in[row][col] = 128;


    for(col=col_size; col<ncol_size; col++)
     for(row=0; row<nrow_size; row++)
      image_in[row][col] = 128;

/* Generate compressed representation using the mode selected */




          switch(mode) {
           case 1:
/*  Simple minded sampling method of progressive transmission */

     for(row=0; row<nrow_size; row+=block_size)
      for(col=0; col<ncol_size; col+=block_size)
        cmp_image[row/block_size][col/block_size] = image_in[row][col];
            break;


           case 2:
/* Simple minded averaging method */

   for(row=0; row<nrow_size; row+=block_size)
      for(col=0; col<ncol_size; col+=block_size)
      {
       ave = 0;
       for(i=0; i<block_size; i++)
        for(j=0; j<block_size; j++)
         ave += image_in[row+i][col+j];

       ave = ave / (block_size*block_size);
       cmp_image[row/block_size][col/block_size] = ave;
       }
            break;

            default:
             fprintf(stderr,"No mode was specified");
             exit(1);
                    }


/* Store mode, block size, and dimensions of the image */
     
    fwrite(&mode,1,sizeof(int),ofp);
    fwrite(&block_size,1,sizeof(int),ofp);
    fwrite(&col_size,1,sizeof(int),ofp);
    fwrite(&row_size,1,sizeof(int),ofp);

/* Store compressed image  */

    for(row=0; row<Mr; row++)
     for(col=0; col<Mc; col++)
       putc(cmp_image[row][col],ofp);


   }

void usage(void)
{
  fprintf(stderr,"Usage: progt_enc [-i infile] [-o outfile] [-m mode] [-b block size] [-h]\n");
  fprintf(stderr,"\t mode is an integer between 1 and 2: \n");
  fprintf(stderr,"\t\t Mode 1: reduction by subsampling\n");
  fprintf(stderr,"\t\t Mode 2: reduction by averaging\n");
  fprintf(stderr,"\t Block size is the size of the block to be replaced by a single pixel\n");
  fprintf(stderr,"\n\t If the input and output file names are not provided\n");
  fprintf(stderr,"\t the program assumes defaults of standard in and standard out\n");
}
