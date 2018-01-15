#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "idc.h"

/************************************************************************
*                                                                       *
* File: trvqsp_img.c                                                    *
* Function: Generates a codebook for use in the vector quantization of  *
*           images.  The functions vqimg_enc and vqimg_dec do the actual*
*           compression and decompression.                              *
* Author  : K. Sayood                                                 *
* Last mod: 6/16/95                                                     *
* Usage:  see usage()                                                   *
*                                                                       *
************************************************************************/

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
  FILE *tfp, *icb;
  int **codebook; 
  int **training_set;
  int *input, dimension, index, codebook_size, ts_size, c;
  int i, j,  iteration, tsvec, final_codebook_size, *eps;
  int **new_codebook, *count;
  int *store, max, sindex;
  int k,l, blockr, blockc, cp_row, cp_col, row_size, col_size;
  float measure, total_distortion, total_distortion_old;
  float threshold, distortion;
  char name[50];
  unsigned char **trimg, pix;
  extern int optind;
  extern char *optarg;

  final_codebook_size = -1;
  blockr = -1;
  blockc = -1;
  row_size = -1;
  col_size = -1;


/* Obtain the filename for the training image */

  if((tfp = fopen(argv[1],"rb"))==NULL)
  {
   fprintf(stderr,"File open failed for the training image %s\n",argv[1]);
   usage();
   exit(1);
  }
  optind++;
  strcpy(name,argv[1]);



/* File to store the final codebook */

  if((icb = fopen(argv[2],"wb"))==NULL)
  {
   fprintf(stderr,"File open failed for output file %s\n", argv[2]);
   usage();
   exit(2);
  }
  optind++;

   fprintf(stderr,"\n\n\t\t Vector Quantizer Design For Image Compression\n\n");
   fprintf(stderr,"\t This program can be used to obtain a VQ codebook for image\n");
   fprintf(stderr,"\t compression using the LBG (GLA) algorithm.  The codebook is \n");
   fprintf(stderr,"\t initialized using the splitting method. \n\n");
   fprintf(stderr,"\t ******************* NOTE *********************\n");
   fprintf(stderr,"\t This program takes a long time to run.  It might be a\n");
   fprintf(stderr,"\t good idea to nice it and run it in background\n\n");

  
  while((c=getopt(argc,argv,"b:t:w:x:y:h")) != EOF)
  {
   switch (c){
    case 'b':
     sscanf(optarg,"%d", &final_codebook_size);
     break;
    case 't':
     sscanf(optarg,"%d", &blockr);
     break;
    case 'w':
     sscanf(optarg,"%d", &blockc);
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

  if(row_size < 0 || col_size < 0)
    image_size(name, &row_size, &col_size);
  printf("Image: %s, Number of rows = %d, Number of columns = %d\n",name,row_size,col_size);
  trimg  = (unsigned char **) calloc(row_size,sizeof(char *));
  for(i=0; i< row_size; i++)
    trimg[i] = (unsigned char *) calloc(col_size,sizeof(char));

  readimage(name, trimg, row_size, col_size);

   if(final_codebook_size < 0)
   {
    fprintf(stderr,"Enter codebook size:\n");
    scanf("%d",&final_codebook_size);
   }

   if(blockr < 0)
   {
    fprintf(stderr,"Enter height of block :\n");
    scanf("%d",&blockr);
   }


   if(blockc < 0)
   {
    fprintf(stderr,"Enter width of block :\n");
    scanf("%d",&blockc);
   }

   dimension = blockr*blockc;
   ts_size = (row_size * col_size)/dimension;


   printf("Codebook size = %d, Training set = %d, dimension = %d\n", final_codebook_size, ts_size, dimension);

/*  Allocate space for codebook, training set, input and associated work
    arrays */

    codebook = (int **) calloc(final_codebook_size,sizeof(int *));
    for(i=0; i< final_codebook_size; i++)
      codebook[i] = (int *) calloc(dimension,sizeof(int));
    training_set = (int **) calloc(ts_size,sizeof(int *));
    for(i=0; i< ts_size; i++)
      training_set[i] = (int *) calloc(dimension,sizeof(int));

    input = (int *) calloc(dimension,sizeof(int));

    new_codebook = (int **) calloc(final_codebook_size,sizeof(int *));
    for(i=0; i< final_codebook_size; i++)
      new_codebook[i] = (int *) calloc(dimension,sizeof(int));

    count = (int *) calloc(final_codebook_size,sizeof(int));

    eps = (int *) calloc(dimension,sizeof(int));

    store = (int *) calloc(final_codebook_size,sizeof(int));



   printf("Codebook size = %d, Training set = %d, dimensions = %d %d\n", final_codebook_size, ts_size, blockr, blockc);
   
  cp_row = 0;
  cp_col = 0;
  for(i=0; i< ts_size; i++)
  {
   l = 0;
   for(j=0; j < blockr; j++)
    for(k=0; k < blockc; k++)
   {
    training_set[i][l]= trimg[cp_row+j][cp_col+k];
    l++;
    }
    cp_col += blockc;
    if(cp_col+blockc-1 > col_size)
    {
     cp_col = 0;
     cp_row += blockr;
    }
   }
 
    
/* Set up perturbation vector */
/* For now use fixed perturbation */
   for(i=0; i<dimension; i++)
     eps[i] = 10.0;

/* Obtain one level VQ */

   codebook_size = 1;
   for(j=0; j<dimension; j++)
    codebook[0][j] = 0;

   for(i=0; i<ts_size; i++)
    for(j=0; j<dimension; j++)
     codebook[0][j] += training_set[i][j];

   for(j=0; j<dimension; j++)
    codebook[0][j] = (int)((float) codebook[0][j]/(float) ts_size +0.5);

   while(codebook_size < final_codebook_size)
   {
    codebook_size = 2*codebook_size;
    
    for(i=0; i<codebook_size/2; i++)
     for(j=0; j<dimension; j++)
     {
      new_codebook[2*i][j] = codebook[i][j] + eps[j];
      new_codebook[2*i+1][j] = codebook[i][j];
     }
    for(i=0; i<codebook_size; i++)
     for(j=0; j<dimension; j++)
      codebook[i][j] = new_codebook[i][j];

    measure = 1000.;
    threshold = 1./(float) ts_size;
    iteration = 0;
    while(measure > threshold && iteration < maxit)
    {
     if(iteration > 0)
     {
      total_distortion_old = total_distortion;
      for(i=0;i<codebook_size;i++)
       store[i] = count[i];
      
      for(i=0;i<codebook_size;i++)
      {
       for(j=0; j<dimension; j++)
        if(count[i] > 0)
         codebook[i][j] =(int)((float) new_codebook[i][j]/(float) count[i] + .5);
        else {
/* do something about zero count */
         max = -100;
         for(k=0;k<codebook_size;k++)
          if(store[k]>max) {
           max = store[k];
           sindex = k;      }
         store[sindex] = -1;
         for(k=0; k<dimension; k++)
           codebook[i][k] = codebook[sindex][k] + eps[k];
             }
      }
     }





     for(i=0;i<codebook_size;i++)
      for(j=0; j<dimension; j++)
       new_codebook[i][j] = 0;
     for(i=0;i<codebook_size;i++)
       count[i] = 0;
     total_distortion = 0;
     for(tsvec=0; tsvec<ts_size; tsvec++)
     {
      for(i=0; i<dimension; i++)
       input[i] = training_set[tsvec][i];
      
      index = vqencode(input,codebook,codebook_size,dimension,&distortion);
 
      total_distortion += distortion;
      count[index]++;
      for(j=0; j<dimension; j++)
       new_codebook[index][j] += input[j];
      }
      total_distortion = total_distortion/(float) ts_size;
      if(iteration > 0)
       measure = (total_distortion_old - total_distortion)/total_distortion_old;
      else
       measure = total_distortion;
       measure = (float) fabs((double) measure);
      iteration++;
     }
 
/*    for(i=0; i<codebook_size; i++)
    {
     for(j=0; j<dimension; j++)
      printf("  %d ",codebook[i][j]);
     printf("\n");
    }
*/
   }
   fwrite(&blockr,1,sizeof(int),icb);
   fwrite(&blockc,1,sizeof(int),icb);
   fwrite(&final_codebook_size,1,sizeof(int),icb);
   for(i=0; i<final_codebook_size; i++)
    for(j=0; j<dimension; j++)
   {
     if(codebook[i][j] > 255)
        codebook[i][j] = 255;
     if(codebook[i][j] < 0)
        codebook[i][j] = 0;
     pix = (unsigned char) codebook[i][j] & 255;
     putc(pix,icb);
   }
   printf("******** Final distortion %f ****************\n",total_distortion);

 }
 
 void usage(void)
{
  fprintf(stderr,"This program is used to obtain codebooks for the vector quantization of\n");
  fprintf(stderr,"images. The codebook is initialized using the splitting approach\n\n");
  fprintf(stderr,"Usage: \n");
  fprintf(stderr,"trvqsp_img infile outfile [-b csize][-w width][-t height][-x row_size][-y col_size][-h]\n\n");
  fprintf(stderr,"\t infile:  File containing the image to be used for training.\n");
  fprintf(stderr,"\t outfile: File to contain the codebook.\n");
  fprintf(stderr,"\t csize  : Size of the codebook to be generated.\n");
  fprintf(stderr,"\t width  : width of the block of pixels making up a vector\n");
  fprintf(stderr,"\t height : height of the block of pixels making up the vector\n");
  fprintf(stderr,"\t row_size: number of pixels in a row of the image.\n");
  fprintf(stderr,"\t col_size: number of rows in an image.\n");
}  
