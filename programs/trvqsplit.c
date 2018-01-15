#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "idc.h"



/**********************************************************************
*                                                                      *
*  File: trvqsplit.c                                                   *
*  Function:  trains a codebook using the splitting approach of Linde, *
*             Buzo, and Gray                                           *
*  Author  : K. Sayood                                                 *
*  Last mod: 5/12/95                                                   *
*  Usage:  see usage().  For more details see trvqsplit.doc or the man *
*          page                                                        *
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
  FILE *tfp, *ofp;
  char codefile[50];
  int **codebook; 
  int **training_set;
  int *input , dimension, index, codebook_size, ts_size, c;
  int i, j,  iteration, tsvec, final_codebook_size, *eps;
  float measure, total_distortion, total_distortion_old;
  int **new_codebook, *count;
  float threshold, distortion;
  extern int optind;
  extern char *optarg;

  final_codebook_size = -1;
  dimension = -1;
  ts_size = -1;


/* Obtain the file containing the training set */

  if((tfp = fopen(argv[1],"r"))==NULL)
  {
   fprintf(stderr,"File open failed for the training set file %s\n",argv[1]);
   usage();
   exit(1);
  }
  optind++;

  ofp = stdout;
  strcpy(codefile,"standard out");
 
  while((c=getopt(argc,argv,"o:b:d:t:h")) != EOF)
  {
   switch (c){
   case 'o':
         strcpy(codefile,optarg);
         ofp = fopen(optarg,"w");
         break;
    case 'b':
     sscanf(optarg,"%d", &final_codebook_size);
     break;
    case 'd':
     sscanf(optarg,"%d", &dimension);
     break;
    case 't':
     sscanf(optarg,"%d", &ts_size);
     break;
   case 'h':
         usage();
         exit(1);
             }
   }

   if(final_codebook_size < 0)
   {
    fprintf(stderr,"Enter codebook size:\n");
    scanf("%d",&final_codebook_size);
   }

   if(dimension < 0)
   {
    fprintf(stderr,"Enter vector dimension:\n");
    scanf("%d",&dimension);
   }


   if(ts_size < 0)
   {
    fprintf(stderr,"Enter size of training set:\n");
    scanf("%d",&ts_size);
   }

   fprintf(stderr,"\n\n\t\t Vector Quantizer Design Using the LBG algorithm \n\n");

   fprintf(stderr,"\t This program can be used to train a vector quantizer using the \n");
   fprintf(stderr,"\t LBG (GLA) algorithm.  The codebook is initialized using \n");
   fprintf(stderr,"\t the splitting method. If the user wishes to use an initial\n");
   fprintf(stderr,"\t codebook for initialization s/he should use the program trainvq.\n");  
   fprintf(stderr,"\t This program reads in the training set as ints.  The user\n");
   fprintf(stderr,"\t  can modify this by changing the read statement. \n");


   printf("Codebook size = %d, Training set = %d, dimension = %d\n", final_codebook_size, ts_size, dimension);
  
/*  Allocate space for codebook, training set, input and associated work
    arrays */

    codebook = (int **) calloc(codebook_size,sizeof(int *));
    for(i=0; i< codebook_size; i++)
      codebook[i] = (int *) calloc(dimension,sizeof(int));
    training_set = (int **) calloc(ts_size,sizeof(int *));
    for(i=0; i< ts_size; i++)
      training_set[i] = (int *) calloc(dimension,sizeof(int));

    input = (int *) calloc(dimension,sizeof(int));

    new_codebook = (int **) calloc(codebook_size,sizeof(int *));
    for(i=0; i< codebook_size; i++)
      new_codebook[i] = (int *) calloc(dimension,sizeof(int));

    count = (int *) calloc(codebook_size,sizeof(int));

    eps = (int *) calloc(dimension,sizeof(int));


 
   for(i=0; i< ts_size; i++)
    for(j=0; j < dimension; j++)
     fscanf(tfp,"%d",&training_set[i][j]);

/* Set up perturbation vector */
/* For now use fixed perturbation - this can be changed to a random perturbation
   vector    */

   for(i=0; i<dimension; i++)
     eps[i] = 10.0;

/* Obtain one level VQ */

   for(j=0; j<dimension; j++)
    codebook[0][j] = 0;

   for(i=0; i<ts_size; i++)
    for(j=0; j<dimension; j++)
     codebook[0][j] += training_set[i][j];

   for(j=0; j<dimension; j++)
    codebook[0][j] = (int)((float) codebook[0][j]/(float) ts_size +0.5);

   codebook_size = 1;
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
    threshold = 0.1;
    iteration = 0;
    while(measure > threshold && iteration < maxit)
    {
     if(iteration > 0)
     {
      total_distortion_old = total_distortion;
      for(i=0;i<codebook_size;i++)
      {
       for(j=0; j<dimension; j++)
        if(count[i] > 0)
         codebook[i][j] =(int)((float) new_codebook[i][j]/(float) count[i] + .5);
      }
     }


/* do something about zero count */



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
      iteration++;
     }
 
   }
   for(i=0; i<final_codebook_size; i++)
   {
    for(j=0; j<dimension; j++)
     fprintf(ofp,"%d  ",codebook[i][j]);
    fprintf(ofp,"\n"); 
   }
   printf(" ********  Final distortion :  %f  ********\n",total_distortion);
 }
 
 void usage(void)
{
  fprintf(stderr,"Usage:\n");
 fprintf(stderr,"trvqsplit file1 [-o outfile] [-b codebook size] [-d dimension] [-t training set size] [-h]\n\n");
  fprintf(stderr,"file1  : File containing training vectors \n");

  fprintf(stderr,"outfile: File containing codebook vectors.  If a filename is\n");
  fprintf(stderr,"not specified, the values are written to standard out\n \n");
  fprintf(stderr," The elements of the vectors are read in as int.  If this is \n");
  fprintf(stderr," not desirable the user should modify the read statement.\n\n");
  fprintf(stderr,"\t codebook size    :  The number of vectors in the codebook.\n");
  fprintf(stderr,"\t dimension        :  The dimension of the vectors (total number of \n");
  fprintf(stderr,"\t\t\t\t elements in each vector).  \n");
  fprintf(stderr,"\t training set size: The number of vectors in the training set. \n");
}  
