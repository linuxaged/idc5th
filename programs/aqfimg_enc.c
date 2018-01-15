#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "idc.h"

/**********************************************************************
*                                                                      *
*  File: aqfimg_enc.c                                                  *
*  Function:  lossy compression of an image using forward adaptive     *
*             quantization.                                            *
*  Author (aka person to blame) : K. Sayood                            *
*  Last mod: 7/15/95                                                   *
*  Usage:  see usage(), for details see aqfimg_enc.doc or man  page    *
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
    char input_image[50], qimage[50];
    unsigned char **imgin, **block;
    int *bound, input, label, numlev, numbits, c, i, rowblocks;
    int colblocks, brow, bcol, block_count;
    int minval, maxval, range, level, stepsize, temp, end_flag, blocksize;
    FILE *ofp;
  extern int  optind;
  extern char *optarg;




    fprintf(stderr,"\n\n\t\tForward Adaptive Quantization of Images - Encoder\n");

  ofp = stdout;
  strcpy(qimage,"standard out");
  numlev = -1;
  numbits = -1;
  minval = 0;
  maxval = 255;
  blocksize = 8;
  row_size = -1;
  col_size = -1;

  while((c=getopt(argc,argv,"i:o:l:b:s:h"))!=EOF)
  {
   switch (c){
   case 'i':
         strcpy(input_image,optarg);
         break;
   case 'o':
         strcpy(qimage,optarg);
         ofp = fopen(optarg,"wb");
         break;
   case 'l':
         sscanf(optarg,"%d", &numlev);
         break;
   case 'b':
         sscanf(optarg,"%d", &numbits);
         break;
   case 's':
         sscanf(optarg,"%d",&blocksize);
         break;
   case 'x':
         sscanf(optarg,"%d",&row_size);
         break;
   case 'y':
         sscanf(optarg,"%d",&col_size);
         break;
   case 'h':
         usage();
         exit(1);
              }
   }



   if(numlev > 0 && numbits > 0)
   {
    temp = (int) pow((double) 2,(double) numbits);  
    if(temp != numlev)
    {
     fprintf(stderr,"\n You have entered values for the number of levels and\n");
     fprintf(stderr,"number of bits that are not compatible.  The number of\n");
     fprintf(stderr,"levels should be 2^(number of bits).  If you want to use\n");
     fprintf(stderr,"a number of levels which is not a power of 2 then only\n");
     fprintf(stderr,"enter a value for the number of levels.\n");
     exit(1);
    }
   }
     

    

   if(numlev < 0 && numbits < 0)
   {
    fprintf(stderr,"\n Enter number of bits per pixel: ");
    scanf("%d",&numbits);
    numlev = (int) pow((double) 2,(double) numbits);
   }

   if(numlev < 0 && numbits > 0)
    numlev = (int) pow((double) 2,(double) numbits);
  
   if(numlev > 0 && numbits < 0)
     numbits = (int) (log( (double)numlev)/log((double) 2.) + .999);
 
/* Allocate space for the boundary values */

   bound = (int *) calloc((numlev+1),sizeof(int));

/* If the dimensions of the image have not been entered find out how 
   large the image is */

    if(row_size<0 || col_size<0)
      image_size(input_image, &row_size, &col_size);   

/* Allocate space for input image  */

    imgin  = (unsigned char **) calloc(row_size,sizeof(char *));  
    for(row=0; row<row_size; row++)
     imgin[row] = (unsigned char *) calloc((col_size),sizeof(char));


/* Read the input image */

    readimage(input_image, imgin, row_size, col_size);  

/* Allocate space for block of the image  */

    block = (unsigned char **) calloc(blocksize,sizeof(char*));
    for(brow=0; brow<blocksize; brow++)
      block[brow] = (unsigned char *) calloc(blocksize,sizeof(char));


/*  Store coding parameters in the output file */

   fwrite(&numlev,1,sizeof(int),ofp);
   fwrite(&numbits,1,sizeof(int),ofp);
   fwrite(&row_size,1,sizeof(int),ofp);
   fwrite(&col_size,1,sizeof(int),ofp);
   fwrite(&blocksize,1,sizeof(int),ofp);
  
   rowblocks = row_size/blocksize;
   colblocks = col_size/blocksize;

/*  Main encoding loop  */

   block_count = 0;
   for(row=0; row<row_size; row+= blocksize)
      for(col=0; col<col_size; col+= blocksize)
         {
/*         extract a block         */
          minval = 1000;
          maxval = -1000;
          for(brow = 0; brow<blocksize; brow++)
           for(bcol = 0; bcol<blocksize; bcol++)
            {
             block[brow][bcol] = imgin[row+brow][col+bcol];
             if(block[brow][bcol] < minval)
               minval = block[brow][bcol];
             if(block[brow][bcol] > maxval)
               maxval = block[brow][bcol];
             }
/* Store the values of maxval and minval in the file */

           stuffit(minval,8,ofp,0);
           stuffit(maxval,8,ofp,0);


/* Construct the boundary tables   */

           range = maxval - minval + 1;
           stepsize = range/numlev;

           bound[0] = minval;
           for(i=1; i<=numlev; i++)
            {
             bound[i] = bound[i-1] + stepsize;
            }

/* encode each pixel into an integer label */

          end_flag = 0;
          for(brow = 0; brow<blocksize; brow++)
           for(bcol = 0; bcol<blocksize; bcol++)
            {
             input = block[brow][bcol];
             label = encuqi(input,bound,numlev);
             if(row+brow == row_size-1 && col+bcol == col_size - 1)
               end_flag = 1;
             stuffit(label,numbits,ofp,end_flag);

            }
    block_count++;
          }
}
void usage()
{
  fprintf(stderr,"Usage: aqfimg_enc [-i input file][-o output file] [-l numlev] [-b numbits] [-s blocksize]\n");
  fprintf(stderr,"\t input file : File containing the image to be quantized.\n");
  fprintf(stderr,"\t output file: File which will contain the compressed representation\n");
  fprintf(stderr,"\t including any header information. The default is standard out.\n");
  fprintf(stderr,"\t numlev  : Number of reconstruction values in the quantizer. If\n");
  fprintf(stderr,"\t this option is not used, the number of levels is computed as\n");
  fprintf(stderr,"\t 2^(number of bits).  If numlev is provided, numbits need not\n");
  fprintf(stderr,"\t be provided.\n");
  fprintf(stderr,"\t numbits  : Number of bits per pixel.\n");
  fprintf(stderr,"\t row_size: Length of a row of the image\n");
  fprintf(stderr,"\t col_size: Length of a column of the image\n");
  fprintf(stderr,"\t\t If the column or row size is not provided the program\n")
;
  fprintf(stderr,"\t\t will check to see if the iamge corresponds to any of the\
n");
  fprintf(stderr,"\t\t standard sizes it is familiar with.  To add to the list of\n");
  fprintf(stderr,"\t\t standard sizes, edit image_size.c\n\n");





}
