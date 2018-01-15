#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "idc.h"


#define maxit 100


/**********************************************************************
*                                                                      *
*  File: trainvq.c                                                     *
*  Function:  generates the codebook for a vector quantizer.           *
*  Author  : K. Sayood                                                 *
*  Last mod: 5/12/95                                                   *
*  Usage:  see usage().  For more details see trainvq.doc or man page. *
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
  FILE *tfp, *icb;
  int **codebook; 
  int **training_set;
  int *input, dimension, index, codebook_size, ts_size, c;
  int i, j, init_code_flag, iteration, tsvec;
  float measure, total_distortion, total_distortion_old;
  int **new_codebook, *count;
  float threshold, distortion;
  char codefile[80];
  extern int optind;
  extern char *optarg;


  init_code_flag = 0;
  codebook_size = -1;
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

/* If the initial codebook is to be read in read filename */

  init_code_flag = 0;


  while((c=getopt(argc,argv,"c:b:d:t:h")) != EOF)
  {
   switch (c){
    case 'c':
     strcpy(codefile,optarg);
     init_code_flag++;
     if((icb = fopen(codefile,"r"))==NULL)
     {
      fprintf(stderr,"Codebook file %s does not exist.  The initial\n",codefile);
      fprintf(stderr,"codebook will be selected from the training set.\n");
      init_code_flag--;
     }
     break;
    case 'b':
     sscanf(optarg,"%d", &codebook_size);
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

   fprintf(stderr,"\n\n\t\t Vector Quantizer Design Using the LBG algorithm \n\n");
   fprintf(stderr,"\t This program can be used to train a vector quantizer using the \n");
   fprintf(stderr,"\t LBG (GLA) algorithm.  The codebook is initialized by a set \n");
   fprintf(stderr,"\t provided by the user or from the training set.  If the user \n");
   fprintf(stderr,"\t wishes to use the splitting method for initialization s/he \n");
   fprintf(stderr,"\t should use the program trvqsplit.  This program reads in the \n");
   fprintf(stderr,"\t training set as ints.  The user can modify this by changing \n");
   fprintf(stderr,"\t the read statement. \n");

   if(codebook_size < 0)
   {
    fprintf(stderr,"Enter codebook size:\n");
    scanf("%d",&codebook_size);
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


   printf("Codebook size = %d, Training set = %d, dimension = %d\n",codebook_size, ts_size, dimension);
  

     
   for(i=0; i< ts_size; i++)
    for(j=0; j < dimension; j++)
     fscanf(tfp,"%d",&training_set[i][j]);
   if(init_code_flag == 1)
   {
    for(i=0; i< codebook_size; i++)
      for(j=0; j < dimension; j++)
        fscanf(icb,"%d",&codebook[i][j]);
   }
   else
   {
    for(i=0; i< codebook_size; i++)
      for(j=0; j < dimension; j++)
        codebook[i][j] = training_set[i][j];
   }




   measure = 1000.;
   threshold = 0.1;
   iteration = 0;
   fprintf(stderr,"Entering the training loop.  The change threshold is %5.2f and\n",threshold);
   fprintf(stderr,"the maximum number of iteration is %d.\n",maxit);

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
/*   printf("distortion is %f\n",total_distortion); */
     if(iteration > 0)
      measure = (total_distortion_old - total_distortion)/total_distortion_old;
     else
      measure = total_distortion;
/*     printf("Iteration number %d, measure = %f\n",iteration,measure); */
     iteration++;
    }

  printf("*****************    Final Codebook   ********************\n");
  for(i=0; i<codebook_size; i++)
  {
   for(j=0; j<dimension; j++)
    printf("  %d ",codebook[i][j]);
   printf("\n");
  }

  printf("\n*****************    Final Distortion   ********************\n");
  printf("                          %f \n",total_distortion); 
}

void usage(void)
{
  fprintf(stderr,"Usage:\n");
 fprintf(stderr,"trainvq file1 [-c codefile] [-b cb_size] [-d dimension] [-t ts_size] [-h]\n\n");
  fprintf(stderr,"\t file1     : File containing training vectors \n");

  fprintf(stderr,"\t codefile  : File containing codebook vectors \n");
  fprintf(stderr,"\t\t The elements of the vectors are read in as int.  If this is\n");
  fprintf(stderr,"\t\t not the case the user should modify the read statement.\n");
  fprintf(stderr,"\t\t If this option is not used, the initial codebook\n");
  fprintf(stderr,"\t\t is selected from the training set.\n");
  fprintf(stderr,"\t cb_size   :  The number of vectors in the codebook. \n");
  fprintf(stderr,"\t dimension :  The dimension of the vectors (total number of \n");
  fprintf(stderr,"\t\t elements in each vector).  \n");
  fprintf(stderr,"\t ts_size   : The number of vectors in the training set. \n");
}  
