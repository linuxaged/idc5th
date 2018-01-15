#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
//#include <malloc.h>
#include "idc.h"

void usage(void);

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

int main(int argc, char *argv[])
{
  float ave, std, reconstruction, output,resrec, scale;
  int numlev,numbits,label, name_length;
  char infile[100], outfile[100], qfile[100], pfile[100];
  float *bound, *recon, *pred, *xf, prediction;
  int i, j, k, num, order, count, size, *buffer;
  int c;
  extern int  optind;
  extern char *optarg;
  FILE *ifp, *ofp,*qfp, *pfp;
  short *aufile;

  order = -1; 
  numlev = -1;
  ofp = stdout;
  strcpy(outfile,"standard out");
  qfp = NULL;
  pfp = NULL;
  while((c=getopt(argc,argv,"i:o:h"))!=EOF)
  {
   switch (c){
   case 'i':
         strcpy(infile,optarg);
         if((ifp=fopen(infile,"rb")) == NULL)
         {
          fprintf(stderr,"Unable to open file %s for read\n",infile);
          exit(1);
         }
         break;
   case 'o':
         strcpy(outfile,optarg);
         if((ofp=fopen(outfile,"wb")) == NULL)
         {
          fprintf(stderr,"Unable to open file %s for write\n",outfile);
          exit(1);
         }
         break;
   case 'h':
         usage();
         exit(4);
         break;
              }
   }

/*                read quantizer file name */


   fread(&name_length,1,sizeof(int),ifp);
   fread(qfile,name_length+1,sizeof(char),ifp);
   fprintf(stderr,"Quantizer file is %s\n",qfile);


     if((qfp=fopen(qfile,"r")) == NULL)
      {
       fprintf(stderr,"Unable to open file %s for reading quantizer parameters\n",qfile);
       exit(4);
      }



  fscanf(qfp,"%d",&numlev);
  fprintf(stderr," Number of levels: %d\n",numlev);

  bound = (float *) calloc((numlev+1),sizeof(float));
  recon = (float *) calloc((numlev+1),sizeof(float));



  for(i=0;i<numlev-1;i++)
   fscanf(qfp,"%g",&bound[i]);

  for(i=0;i<numlev;i++)
   fscanf(qfp,"%g",&recon[i]);





/*                read predictor file name 			*/


   fread(&name_length,1,sizeof(int),ifp);
   fread(pfile,name_length+1,sizeof(char),ifp);

   fprintf(stderr,"Predictor file is %s\n",pfile);

   if(!strcmp(pfile,"nofile"))
    { 
     order = 1;
     pred[0] = 0.866;
     fprintf(stderr,"No predictor file specified.  A first order predictor\n");
     fprintf(stderr,"with predictor coefficient 0.86 will be used\n");
    }
   else
     if((pfp=fopen(pfile,"r")) == NULL)
     {
      fprintf(stderr,"Unable to open file %s for reading predictor parameters\n",pfile);
      exit(3);
     }
     else
     {
     fscanf(pfp,"%d",&order);
     pred = (float *) calloc(order,sizeof(float));
     for(i=0; i<order; i++)
       fscanf(pfp,"%g",&pred[i]);
     }


/* get filesize for the audio file */

   fread(&num,1,sizeof(int),ifp);

/* assuming it takes two bytes for each audio or speech sample */

  fprintf(stderr,"Number of data points is %d\n",num);
  fread(&ave,1,sizeof(float),ifp);
  fread(&std,1,sizeof(float),ifp);
  fread(&scale,1,sizeof(float),ifp);

    buffer = (int *) calloc(num+8,sizeof(int));

/* Read the coded speech or audio file */

    numbits = (int) (log((double) numlev)/log((double) 2.) + 0.99999);
    fprintf(stderr,"bits/label = %d\n",numbits);
    unstuff(numbits,ifp,buffer,&count);
    if(count != num)
    {
     fprintf(stderr,"Mismatch in amount of data\n");
     fprintf(stderr,"num = %d, Number of data points read = %d\n",num,count);
    }

/* get memory for the audio or speech file   */

  aufile = (short *) calloc(num,sizeof(short));

  xf = (float *) calloc(num,sizeof(float));



  fprintf(stderr,"Average value = %f, Standard deviation = %f, scale = %f\n",ave,std,scale);


 for(i=0; i< num; i++)
 {
  predictor(i,order,pred,xf,&prediction);
  label = buffer[i];
  resrec = nuquan_dec(label,recon);
  resrec = resrec/scale;
  reconstruction = prediction + resrec;
  xf[i] = reconstruction;
 }

/*   Denormalize  */

   for(i=0; i<num; i++)
     xf[i] = xf[i]*std+ave;

/* write out to file  */


   for(i=0; i<=num; i++)
   {
     if(xf[i] > 32767)
       xf[i] = 32767;
     if(xf[i] < -32768)
       xf[i] = -32768;
     aufile[i] = (short) xf[i];
   }

   if(fwrite(aufile,sizeof(short),num,ofp)!=num)
     fprintf(stderr,"what the ...."); 
}

void usage(void)
{

  fprintf(stderr,"Usage:\n dpcm_dec [-i infile][-o outfile][-e][-h]\n");
  fprintf(stderr,"\t infile : file containing the compressed speech or audio\n");
  fprintf(stderr,"\t outfile: file containing the reconstructed output\n");
 fprintf(stderr,"\t -h  : results in the display of this message\n");
}
