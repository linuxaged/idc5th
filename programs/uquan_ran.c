#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "idc.h"

/**********************************************************************
*                                                                      *
*  File:  uquan_ran.c                                                  *
*  Function:  explore uniform quantization of random inputs.           *
*  Author  : K. Sayood                                                 *
*  Last mod: 5/12/95                                                   *
*  Usage:  see usage(), for details see uquan_ran.doc or the man page. *
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

int main(int argc, char *argv[])
{
  float mean, var, secmom, qerr, msqe, snr;
  float input, output,stepsize,low,prob[256], entropy;
  int i, num, numlev,numbits,label, distribution, ent_flag;
  FILE *fp;
  int c, seed;
  extern int  optind;
  extern char *optarg;


    seed = 0;
    zufalli_(seed);


  num = -1;
  numlev = -1;
  stepsize = -1.0;
  distribution = -10;
  ent_flag = 0;
  while((c=getopt(argc,argv,"l:s:n:d:eh"))!=EOF)
  {
   switch (c){
   case 'l':
         sscanf(optarg,"%d",&numlev);
         break;
   case 's':
         sscanf(optarg,"%f",&stepsize);
         break;
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
  if(numlev < 0){
   printf("Enter number of levels\n");
   scanf("%d",&numlev);}

  if(stepsize < 0.){
   printf("Enter stepsize\n");
   scanf("%f",&stepsize);}

  if(num < 0){
   printf("Enter # of #s to test \n");
   scanf("%d",&num);}
  if(distribution < 0){
   printf(" What is the probability model for the input\n");
   printf(" Enter 0 for uniform\n");
   printf("       1 for Gaussian\n");
   printf("       2 for Laplacian\n");
   scanf("%d",&distribution);}


   numbits = (int) (log((double) numlev)/log((double) 2.) + 0.99999);


/* Are the number of levels even or odd
*/

  if((numlev/2)*2 == numlev)
      low = (float) -(numlev/2 - 1)* stepsize;
  else
      low = (float) -((numlev/2)* stepsize -stepsize/2);

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
  
   label = uquan_enc(input, stepsize, numlev, low);
   if(ent_flag)
     prob[label]++;
   output = uquan_dec(label,  stepsize, low);
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
  fprintf(stderr,"Usage: uquan_ran [-eh][-l numlev][-s step_size][-n num][-d distribution]\n");
  fprintf(stderr,"\t -e This option forces the calculation of the output entropy of\n");
  fprintf(stderr,"\t\t the quantizer.\n");
  fprintf(stderr,"\t -h This option generates this message\n");
  fprintf(stderr,"\n\t numlev      :  Number of levels in the quantizer.\n");
  fprintf(stderr,"\t num         :  Number of inputs to be generated.\n");
  fprintf(stderr,"\t step_size   :  Quantization interval.\n");
  fprintf(stderr,"\t distribution:  Input probability distribution\n");
  fprintf(stderr,"\t\t A value of 0 corresponds to Uniform distribution.\n");
  fprintf(stderr,"\t\t A value of 1 corresponds to Gaussian distribution.\n");
  fprintf(stderr,"\t\t A value of 2 corresponds to Laplacian distribution.\n");
  fprintf(stderr,"\t\t A value of 3 corresponds to Gamma distribution.  This particular \n");
  fprintf(stderr,"\t\t option should be used with care, as the function generating the \n");
  fprintf(stderr,"\t\t Gamma distribution is not entirely reliable.\n");
}
