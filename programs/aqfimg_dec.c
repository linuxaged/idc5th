#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include "idc.h"

/**********************************************************************
*                                                                      *
*  File: aqfimg_dec.c                                                  *
*  Function:  reconstruction of an image compressed using aqfimg_enc.  *
*  Author  : K. Sayood                                                 *
*  Last mod: 7/15/95                                                   *
*  Usage:  see usage(), for details see aqfimg_dec.doc or man  page    *
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


void usage();

int main(int argc, char **argv)
{
    int row, col, row_size, col_size;
    char output_image[50], qimage[50];
    unsigned char **imgout, **block;
    int *bound, output, label, numlev, numbits, c, i, rowblocks;
    int colblocks, brow, bcol, *reco, block_count;
    int minval, maxval, range, level, stepsize, temp, end_flag, blocksize;
    FILE *ifp, *ofp;
  extern int  optind;
  extern char *optarg;




    fprintf(stderr,"\n\n\t\tForward Adaptive Quantization of Images - Decoder\n");
  ifp = stdin;
  strcpy(qimage,"standard in");
  ofp = stdout;
  strcpy(output_image,"standard out");
  numlev = -1;
  numbits = -1;
  minval = 0;
  maxval = 255;
  blocksize = 8;

  while((c=getopt(argc,argv,"i:o:h"))!=EOF)
  {
   switch (c){
   case 'i':
         strcpy(qimage,optarg);
         ifp = fopen(optarg,"rb");
         break;
   case 'o':
         strcpy(output_image,optarg);
         ofp = fopen(optarg,"wb");
         break;
   case 'h':
         usage();
         exit(1);
              }
   }


/*
    fprintf(stderr,"\n\nEnter Filename of Source Image: ");
       gets(input_image);

    fprintf(stderr,"\n\nEnter Filename of Output Image: ");
       gets(output_image);
*/

/* Find out the coding parameters */
   fread(&numlev,1,sizeof(int),ifp);
   fread(&numbits,1,sizeof(int),ifp);
   fread(&row_size,1,sizeof(int),ifp);
   fread(&col_size,1,sizeof(int),ifp);
   fread(&blocksize,1,sizeof(int),ifp);
 
/* Allocate space for the boundary and reconstruction values */
   bound = (int *) calloc((numlev+1),sizeof(int));
   reco  = (int *) calloc(numlev,sizeof(int));


/* Allocate space for output image  */

    imgout  = (unsigned char **) calloc(row_size,sizeof(char *));  
    for(row=0; row<row_size; row++)
     imgout[row] = (unsigned char *) calloc((col_size),sizeof(char));


/* Allocate space for block of the image  */

    block = (unsigned char **) calloc(blocksize,sizeof(char*));
    for(brow=0; brow<blocksize; brow++)
      block[brow] = (unsigned char *) calloc(blocksize,sizeof(char));



  
   rowblocks = row_size/blocksize;
   colblocks = col_size/blocksize;

   block_count = 0;

   for(row=0; row<row_size; row+= blocksize)
      for(col=0; col<col_size; col+= blocksize)
         {
/*         extract a block         */

           minval = readnbits(8,ifp);
           maxval = readnbits(8,ifp);


/* Construct the boundary tables   */

           range = maxval - minval + 1;
           stepsize = range/numlev;


           bound[0] = minval;
           for(i=1; i<=numlev; i++)
            {
             bound[i] = bound[i-1] + stepsize;
             reco[i-1] =  bound[i-1] + stepsize/2;
            }

/* read an integer label for each pixel and decode then write out the image */

          for(brow = 0; brow<blocksize; brow++)
           for(bcol = 0; bcol<blocksize; bcol++)
            {
             label = readnbits(numbits,ifp);
             output = decuqi(label,reco);
             imgout[row+brow][col+bcol] = output;
            }
   block_count++;
          }

          for(row=0; row<row_size; row++)
            for(col=0; col<col_size; col++)
              putc(imgout[row][col],ofp);




/***************************************************************************/




}
void usage()
{
  fprintf(stderr,"Usage: aqfimg_dec [-i input file][-o output file] [-h]\n");
  fprintf(stderr,"\t output file : File containing the reconstructed image.\n");
  fprintf(stderr,"\t input file: File which will contain the compressed representation\n");
  fprintf(stderr,"\t including any header information. The default is standard in.\n");


}
