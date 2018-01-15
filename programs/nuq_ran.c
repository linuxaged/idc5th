#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "idc.h"

/**********************************************************************
*                                                                      *
*  File: nuq_ran.c                                                     *
*  Function:  exploration of non-uniform Lloyd-Max quantization of     *
*             random inputs.                                           *
*  Author : K. Sayood                                                  *
*  Last mod: 7/15/95                                                   *
*  Usage:  see usage(), for details see nuq_ran.doc or man page        *
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

int main(int argc, char *argv[])
{
  float mean, var, secmom, qerr, msqe, snr;
  float input, output,stepsize,low,prob[256], entropy;
  int i, num, numlev,numbits,label, distribution, ent_flag;
  FILE *fp;
  float *bound, *recon;

  int c, seed;
  extern int  optind;
  extern char *optarg;

    seed = 0;
    zufalli_(seed);


  num = 0;
  distribution = -10;
  numlev = -10;
  ent_flag = 0;
  if((fp=fopen(argv[1],"r"))==NULL)
   {
    fprintf(stderr,"Open failed for quantizer input file %s\n",argv[1]);
    usage();
    exit(1);
   }
 optind++;   
  while((c=getopt(argc,argv,"n:d:eh"))!=EOF)
  {
   switch (c){
   case 'n':
         sscanf(optarg,"%d", &num);
         break;
   case 'd':
         sscanf(optarg,"%d", &distribution);
         break;
   case 'e':
         ent_flag++;
         break;
   case 'h':
    	 usage();
    	 exit(1);
              }
   }
 fprintf(stderr,"Number of iterations is %d\n",num);
  if(num == 0){
   printf("Enter # of #s to test \n");
   scanf("%d",&num);}
  if(distribution < 0){
   printf(" What is the probability model for the input\n");
   printf(" Enter 0 for uniform\n");
   printf("       1 for Gaussian\n");
   printf("       2 for Laplacian\n");
   scanf("%d",&distribution);}
  fscanf(fp,"%d",&numlev);
  fprintf(stderr," Number of levels: %d\n",numlev);


  bound = (float *) calloc((numlev+1),sizeof(float));
  recon = (float *) calloc((numlev+1),sizeof(float));



  for(i=0;i<numlev-1;i++)
   fscanf(fp,"%g",&bound[i]);

  for(i=0;i<numlev;i++)
   fscanf(fp,"%g",&recon[i]);

  
  mean = 0.0;
  secmom = 0.0;
  msqe = 0.0;
   if(ent_flag)
    for(i=0;i<numlev;i++)
      prob[i] = 0.0;


  for(i=0; i<num; i++)
  {
   input = (float) rangen(distribution);
   mean += input;
   secmom += input*input;
   label = nuquan_enc(input, bound, numlev);
   if(ent_flag)
     prob[label]++;
   output = nuquan_dec(label,  recon);
   qerr = input - output;
   msqe += qerr*qerr;
  }

   mean = mean/(float) num;
   secmom = secmom / (float) num;
   var = secmom - mean*mean;

   printf("mean of input  = %f\n",mean);
   printf("var of input = %f\n",var);
   msqe = msqe/ (float) num;
   printf("mean squared quantization error = %f\n",msqe);
   snr = 10.0 * log10(secmom/msqe);
   printf("The signal to noise ratio (dB) is = %f\n",snr);
   if(ent_flag)
   {
    for(i=0;i<numlev;i++)
      prob[i] = prob[i]/(float) num;
    entropy = ent(prob,numlev);
    printf("The output entropy is %f\n",entropy);
   }

}



void usage()
{
  fprintf(stderr,"Usage: nuq_ran quan_file [-eh][-n num][-d distribution]\n");
  fprintf(stderr,"\t quan_file:  This file contains the number of levels, boundary and \n");
  fprintf(stderr,"\t reconstruction values for the nonuniform quantizer.  The first\n");
  fprintf(stderr,"\t number is the number of reconstruction levels in the quantizer.\n");
  fprintf(stderr,"\t The next (numlev-1) numbers are the inner boundary values (note\n");
  fprintf(stderr,"\t that there are numlev+1 boundary values.  The extreme values are\n");
  fprintf(stderr,"\t assumed to be plus and minus infinity.  The next numlev values\n");
  fprintf(stderr,"\t are the reconstruction values.  Some quan_files should be available\n");
  fprintf(stderr,"\t at the location you found this program.  The name of the files\n");
  fprintf(stderr,"\t are of the form xxnn.dat, xx is ga for Gaussian, la for Laplacian\n");
  fprintf(stderr,"\t and gm for Gamma.  nn denotes the number of levels.\n\n");
  fprintf(stderr,"\t -e This option forces the calculation of the output entropy of\n");
  fprintf(stderr,"\t\t the quantizer.\n");
  fprintf(stderr,"\t -h This option generates this message\n\n");
  fprintf(stderr,"\t num         :  Number of inputs to be generated.\n");
  fprintf(stderr,"\t step_size   :  Quantization interval.\n");
  fprintf(stderr,"\t distribution:  Input probability distribution\n");
  fprintf(stderr,"\t\t A value of 0 corresponds to Uniform distribution.\n");
  fprintf(stderr,"\t\t A value of 1 corresponds to Gaussian distribution.\n");
  fprintf(stderr,"\t\t A value of 2 corresponds to Laplacian distribution.\n");
  fprintf(stderr,"\t\t A value of 3 corresponds to Gamma distribution.  This \n");
  fprintf(stderr,"\t\t particular option should be used with care, as the function \n");
  fprintf(stderr,"\t\t generating the Gamma distribution is not entirely reliable.\n");
}
