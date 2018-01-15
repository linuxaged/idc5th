#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
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
  float mean, var, secmom, ave, std;
  float residual, sig, qerr, msqe, snr, sper;
  float reconstruction, output,resrec,prob[256], entropy;
  int numlev,numbits,label, ent_flag, end_flag, name_length;
  char infile[100], outfile[100], qfile[100], pfile[100];
  float *bound, *recon, *pred, *xf, prediction, scale;
  int i, j, k, num, order, flag, size;
  int c;
  extern int  optind;
  extern char *optarg;
  FILE *ofp,*qfp, *pfp;
  short *aufile;

  order = -1; 
  numlev = -1;
  ent_flag = 0;
  ofp = stdout;
  scale = 1.0;
  strcpy(outfile,"standard out");
  qfp = NULL;
  pfp = NULL;

  while((c=getopt(argc,argv,"i:o:q:p:s:eh"))!=EOF)
  {
   switch (c){
   case 'i':
         strcpy(infile,optarg);
         break;
   case 'o':
         strcpy(outfile,optarg);
         if((ofp=fopen(outfile,"wb")) == NULL)
         {
          fprintf(stderr,"Unable to open file %s for write\n",outfile);
          exit(1);
         }
         break;
   case 'q':
         strcpy(qfile,optarg);
         if((qfp=fopen(qfile,"r")) == NULL)
         {
          fprintf(stderr,"Unable to open file %s for reading quantizer parameters\n",qfile);
          exit(2);
         }
         break;
   case 'p':
         strcpy(pfile,optarg);
         if((pfp=fopen(pfile,"r")) == NULL)
         {
          fprintf(stderr,"Unable to open file %s for reading predictor parameters\n",qfile);
          exit(3);
         }
         break;
   case 's':
         sscanf(optarg,"%f",&scale);
         break;
   case 'e':
         ent_flag++;
         break;
   case 'h':
         usage();
         exit(4);
         break;
              }
   }
   if(qfp == NULL)
   {
     fprintf(stderr,"You need to enter the filename containing the quantizer\n");
     fprintf(stderr,"parameters for this program to work.  Please do so now.\n");
     scanf("%s",&qfile);
     if((qfp=fopen(qfile,"r")) == NULL)
      {
       fprintf(stderr,"Unable to open file %s for reading quantizer parameters\n",qfile);
       exit(4);
      }
    }

/*                write quantizer file name to output file              */


   name_length = strlen(qfile);
   fwrite(&name_length,1,sizeof(int),ofp);
   fwrite(qfile,name_length+1,sizeof(char),ofp);




  fscanf(qfp,"%d",&numlev);
  fprintf(stderr," Number of levels: %d\n",numlev);

  bound = (float *) calloc((numlev+1),sizeof(float));
  recon = (float *) calloc((numlev+1),sizeof(float));



  for(i=0;i<numlev-1;i++)
   fscanf(qfp,"%g",&bound[i]);

  for(i=0;i<numlev;i++)
   fscanf(qfp,"%g",&recon[i]);



    if(pfp == NULL)
    {
     fprintf(stderr,"\nWe are assuming a single tap predictor with prediction\n");
     fprintf(stderr,"coefficient 0.86.  If you want to use a different predictor\n");
     fprintf(stderr,"run this program with the -p option\n\n");
     order = 1;
     pred = (float *) calloc(1,sizeof(float));
     pred[0] = 0.86;
     strcpy(pfile,"nofile");
    }
    else
    {
     fscanf(pfp,"%d",&order);
     pred = (float *) calloc(order,sizeof(float));
     for(i=0; i<order; i++)
       fscanf(pfp,"%g",&pred[i]);
    }


/*                write predictor file name to output file              */

   name_length = strlen(pfile);

   fwrite(&name_length,1,sizeof(int),ofp);
   fwrite(pfile,name_length+1,sizeof(char),ofp);


/* get filesize for the audio file */

   get_file_size(infile,&size);

/* assuming it takes two bytes for each audio or speech sample */

  num = size/2;
  fprintf(stderr,"Number of data points is %d\n",num);
  fwrite(&num,1,sizeof(int),ofp);

/* get memory for the audio or speech file   */

  aufile = (short *) calloc(num,sizeof(short));

/*   Get the audio data */

  readau(infile,aufile);


/* Convert the data into float */

  xf = (float *) calloc(num,sizeof(float));
  for(i=0; i< num; i++)
    xf[i] = (float) aufile[i];

/* Normalize  */

  norm(xf,num,&ave,&std);

  fwrite(&ave,1,sizeof(float),ofp);
  fwrite(&std,1,sizeof(float),ofp);
  fwrite(&scale,1,sizeof(float),ofp);

 
   numbits = (int) (log((double) numlev)/log((double) 2.) + 0.99999);


  mean = 0.0;
  secmom = 0.0;
  sig = 0;
  msqe = 0.0;
   if(ent_flag)
    for(i=0;i<numlev;i++)
      prob[i] = 0.0;


 ave = 0;
 secmom = 0;
 end_flag = 0; 
 for(i=0; i< num; i++)
 {
  if(i==(num-1))
   end_flag = 1;
  predictor(i,order,pred,xf,&prediction);
  sig += xf[i]*xf[i];
  residual = xf[i] - prediction;
  ave += residual;
  secmom += residual * residual;
  residual = scale*residual;
  label = nuquan_enc(residual, bound, numlev);
  if(ent_flag)
      prob[label]++;
  stuffit(label,numbits,ofp,end_flag);
  resrec = nuquan_dec(label,  recon);
  resrec = resrec/scale;
  reconstruction = prediction + resrec;
  msqe += (xf[i]-reconstruction)*(xf[i]-reconstruction);
  xf[i] = reconstruction;
 }
 ave = ave /(float) (num - order);
 secmom = secmom / (float) (num - order);
 var = secmom - ave*ave;
 printf("mean and variance of residuals %f   %f\n",ave,var);
 sig = sig/ (float) (num - order);
 msqe = msqe/ (float) (num - order);
 snr = 10.0 * log10(sig/msqe);
 sper = 10.0 * log10(sig/secmom);

 printf("Signal to noise ratio %f   Signal to prediction error ratio %f\n",snr,sper);

if(ent_flag)
   {
    for(i=0;i<numlev;i++)
      prob[i] = prob[i]/(float) num;
    entropy = ent(prob,numlev);
    printf("The output entropy is %f\n",entropy);
   }

}

void usage(void)
{

  fprintf(stderr,"Usage:\n dpcm_enc [-i infile][-o outfile][-q qfile][-p pfile][-s scale][-e][-h]\n");
  fprintf(stderr,"\t infile : file containing the audio or speech input\n");
  fprintf(stderr,"\t outfile: file containing the compressed output\n");
  fprintf(stderr,"\t qfile  : file containing the quantizer parameters\n");
  fprintf(stderr,"\t pfile  : file containing the predictor coefficients.  If\n");
  fprintf(stderr,"\t\t this option is not used the program uses a one tap predictor\n");
 fprintf(stderr,"\t\t with predictor coefficient 0.86\n");
 fprintf(stderr,"\t scale   : scales the input to the quantizer.\n");
 fprintf(stderr,"\t -e  : This option results in the computation of the output entropy\n");
 fprintf(stderr,"\t\t of the dpcm encoder.\n");
 fprintf(stderr,"\t -h  : results in the display of this message\n");
}
