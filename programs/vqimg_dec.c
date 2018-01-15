#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "idc.h"



/**********************************************************************
*                                                                      *
*  File:  vqimg_dec.c                                                  *
*  Function:  decodes an image encoded using vqimg_enc.                *
*  Author  : K. Sayood                                                 *
*  Last mod: 5/12/95                                                   *
*  Usage:  see usage(). For more details see vqimg_dec.doc or the man  *
*          page.                                                       *
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


void usage(void);

int main(int argc, char **argv)
{
  FILE *ifp, *ofp, *cb;
  int **codebook; 
  int dimension, index, codebook_size, row;
  int c, i, j,  numvecs, numbits, *count, *buffer, name_length;
  int k,l, blockr, blockc, cp_row, cp_col, row_size, col_size, check_count;
  float measure, total_distortion, distortion;
  char output_image[50], cmpfile[50];
  unsigned char **outimg;
  char *cbfile;
  extern int optind;
  extern char *optarg;

  blockr = -1;
  blockc = -1;


/* Obtain the filename for the input image */
  ifp = stdin;
  ofp = stdout;
  cb = NULL;
  strcpy(cmpfile,"standard in");
  strcpy(output_image,"standard out");

  while((c=getopt(argc,argv,"i:o:h"))!=EOF)
  {
   switch (c){
   case 'i':
         strcpy(cmpfile,optarg);
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

  if(ifp == stdin )
   fprintf(stderr,"Input will be read from standard in\n");


  if(ofp == stdout)
   fprintf(stderr,"Output will be written to standard out\n");

/* Read filename for the codebook */

  fread(&name_length,1,sizeof(int),ifp);
  cbfile = (char *) malloc((name_length+1)*sizeof(char));
  fread(cbfile,name_length+1,sizeof(char),ifp);
  cb = fopen(cbfile,"rb");
  
  fprintf(stderr,"Codebook file is %s\n",cbfile);

  if(cb ==NULL)
  {
   fprintf(stderr,"No file available for codebook\n");
   exit(1);
  }



/*  Get image size   */

  fread(&row_size,1,sizeof(int),ifp);
  fread(&col_size,1,sizeof(int),ifp);
  
printf("Image: %s, Number of rows: %d, Number of columns: %d\n",output_image,row_size,col_size);

/* Assign space for output image */

  outimg  = (unsigned char **) calloc(row_size,sizeof(char *));
  for(i=0; i< row_size; i++)
    outimg[i] = (unsigned char *) calloc(col_size,sizeof(char));


/* Read coding parameters from the codebook file */

   fread(&blockr,1,sizeof(int),cb);
   fread(&blockc,1,sizeof(int),cb);
   fread(&codebook_size,1,sizeof(int),cb);
   numbits = (int) (log((double) codebook_size)/log((double) 2.) + 0.999);


   dimension = blockr*blockc;
   numvecs = (row_size*col_size)/dimension;


   printf("Codebook size = %d, dimensions = %d %d\n", codebook_size,  blockr, blockc);
 

/*  Allocate space for the codebook  */

   codebook = (int **) calloc(codebook_size,sizeof(int *));
   for(i=0; i< codebook_size; i++)
     codebook[i] = (int *) calloc(dimension,sizeof(int));
 
/* Read in the codebook */

  for(i=0; i< codebook_size; i++)
    for(j=0; j< dimension; j++)
     codebook[i][j] = getc(cb);


/* Allocate space for input buffer and work arrays */

   buffer = (int *) calloc(numvecs+8,sizeof(int));
   count = (int *) calloc(codebook_size,sizeof(int));
 
 
/*  Read the compressed file */

    unstuff(numbits,ifp,buffer,&check_count);

    printf(" Number of vectors read is %d\n",check_count);

/*  Reconstruct the image   */
  
  cp_row = 0;
  cp_col = 0;
  for(i=0; i< numvecs; i++)
  {

/*  get index */
   index = buffer[i];
   l=0;
   for(j=0; j < blockr; j++)
    for(k=0; k < blockc; k++)
   {
    outimg[cp_row+j][cp_col+k] = 0xff & codebook[index][l];
    l++;
    }

    count[index]++;

    cp_col += blockc;
    if(cp_col+blockc-1 > col_size)
    {
     cp_col = 0;
     cp_row += blockr;
    }
   }
 
   
/*  Write out image  */

   for(row=0; row<row_size; row++)
     fwrite(outimg[row],col_size,sizeof(char),ofp); 

 }
 
 void usage(void)
{
  fprintf(stderr,"Usage:\n");
  fprintf(stderr,"vqimg_dec [-i infile] [-o outfile]\n\n");
  fprintf(stderr,"\t infile  : File to be decompressed.  If no input filename\n");
  fprintf(stderr,"\t\t is provided input is read from standard in.\n");
  fprintf(stderr,"\t outfile : File containing the reconstructed image.  If no\n");
  fprintf(stderr,"\t\t filename is provided output is written to standard out.\n");

}  
