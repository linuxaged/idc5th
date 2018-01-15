#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "idc.h"



/**********************************************************************
*                                                                      *
*  File: vqimg_enc.c                                                   *
*  Function:  encodes an image using a vector quantizer (brute force)  *
*  Author  : K. Sayood                                                 *
*  Last mod: 5/12/95                                                   *
*  Usage:  see usage(). For more details see vqimg_enc.doc or the man  *
*         page                                                         *
*                                                                      *
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


#define maxit 100

void usage(void);

int main(int argc, char **argv)
{
  FILE *ifp, *ofp, *cb;
  int **codebook; 
  int *input, dimension, index, codebook_size ;
  int c, i, j,  iteration, numvecs, numbits, *count, end_flag, name_length;
  int k,l, blockr, blockc, cp_row, cp_col, row_size, col_size;
  float measure, total_distortion, distortion;
  char input_image[50], outfile[50];
  unsigned char **inimg; 
  char *cbfile;
  extern int optind;
  extern char *optarg;

  blockr = -1;
  blockc = -1;


/* Obtain the filename for the input image */
  ifp = stdin;
  ofp = stdout;
  cb = NULL;
  row_size = -1;
  col_size = -1;
  strcpy(input_image,"standard in");
  strcpy(outfile,"standard out");

  while((c=getopt(argc,argv,"i:o:c:x:y:h"))!=EOF)
  {
   switch (c){
   case 'i':
         strcpy(input_image,optarg);
         ifp = fopen(optarg,"rb");
         break;
   case 'o':
         strcpy(outfile,optarg);
         ofp = fopen(optarg,"wb");
         break;
   case 'c':
         name_length = strlen(optarg);
         cbfile = (char *) malloc((name_length+1)*sizeof(char));
         strcpy(cbfile,optarg);
         if((cb = fopen(optarg,"rb")) == NULL)
           fprintf(stderr,"Codebook file %s not found\n",cbfile);
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

  if(ifp == stdin )
   fprintf(stderr,"Input will be read from standard in\n");


  if(ofp == stdout)
   fprintf(stderr,"Output will be written to standard out\n");

/* Make sure you have the filename for the codebook */

  if(cb ==NULL)
  {
   fprintf(stderr,"You *have* to enter a file name for the codebook\n");
   fprintf(stderr,"A codebook can be created by using trvqsp_img.  Remember\n");
   fprintf(stderr,"this codebook will be needed to reconstruct the image.\n\n");
   usage();
   exit(1);
  }

/* Write the name of the codebook file to the output file   */

  fwrite(&name_length,1,sizeof(int),ofp);
  fwrite(cbfile,name_length+1,sizeof(char),ofp);

  fprintf(stderr,"Codebook file is %s\n",cbfile);

/*  If the image dimensions have not been provided get image size   */

  if(row_size < 0 || col_size < 0)
   image_size(input_image, &row_size, &col_size);
  printf("Image: %s, Number of rows: %d, Number of columns: %d\n",input_image,row_size,col_size);

/*  Write image parameters to the output file */

   fwrite(&row_size,1,sizeof(int),ofp);
   fwrite(&col_size,1,sizeof(int),ofp);

/* Assign space for input image */

  inimg  = (unsigned char **) calloc(row_size,sizeof(char *));
  for(i=0; i< row_size; i++)
    inimg[i] = (unsigned char *) calloc(col_size,sizeof(char));

/*  Read the image to be compressed */

  readimage(input_image, inimg, row_size, col_size);


/* Read coding parameters from the codebook file */

   fread(&blockr,1,sizeof(int),cb);
   fread(&blockc,1,sizeof(int),cb);
   fread(&codebook_size,1,sizeof(int),cb);
   numbits = (int) (log((double) codebook_size)/log((double) 2.) + 0.999);


   dimension = blockr*blockc;
   numvecs = (row_size*col_size)/dimension;


   printf("Codebook size = %d, dimensions = %d %d\n", codebook_size,  blockr, blockc);
   fprintf(stderr,"Number of bits per vector = %d\n",numbits);
 
/*  Allocate space for the codebook  */

   codebook = (int **) calloc(codebook_size,sizeof(int *));
   for(i=0; i< codebook_size; i++)
     codebook[i] = (int *) calloc(dimension,sizeof(int));
 
/* Read in the codebook */

  for(i=0; i< codebook_size; i++)
    for(j=0; j< dimension; j++)
    {
     c=getc(cb);
     codebook[i][j] = (int) 0xff & c;
    }



/* Allocate space for input and work arrays */

   input = (int *) calloc(dimension,sizeof(int));
   count = (int *) calloc(codebook_size,sizeof(int));
 
 
  distortion = 0.0;
  total_distortion = 0.0;
  cp_row = 0;
  cp_col = 0;
  for(i=0; i< numvecs; i++)
  {
   l = 0;
   for(j=0; j < blockr; j++)
    for(k=0; k < blockc; k++)
   {
    input[l]=inimg[cp_row+j][cp_col+k];
    l++;
    }
    index = vqencode(input,codebook,codebook_size,dimension,&distortion);

/* Store index  */

    end_flag = 0;
    if(i == (numvecs-1))
      end_flag = 1;
    stuffit(index,numbits,ofp,end_flag);

    total_distortion += distortion;
    count[index]++;

    cp_col += blockc;
    if(cp_col+blockc-1 > col_size)
    {
     cp_col = 0;
     cp_row += blockr;
    }
   }
 
    
      total_distortion = total_distortion/(float) numvecs;
    printf("distortion is %f\n",total_distortion);
   

 }
 
 void usage(void)
{
  fprintf(stderr,"Usage:\n");
  fprintf(stderr,"vqimg_enc [-i imagein][-o cmpfile][-c codebook][-x row_size][-y col_size][-h]\n");
  fprintf(stderr,"\t imagein : file containing the image to be encoded.  If no\n");
  fprintf(stderr,"\t\t name is provided input is read from standard in.\n");
  fprintf(stderr,"\t cmpfile : File to contain the compressed representation.  If\n");
  fprintf(stderr,"\t\t If no name is provided the output is written to standard out.\n");
  fprintf(stderr,"\t codebook: File containing the VQ codebook.\n");
  fprintf(stderr,"\t row_size: Number of pixels in one row of the image\n");
  fprintf(stderr,"\t col_size: Number of rows in the image\n");
}  
