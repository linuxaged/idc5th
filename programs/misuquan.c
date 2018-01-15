#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "idc.h"

/**********************************************************************
*                                                                      *
*  File: misuquan.c                                                    *
*  Function:  exploration of the effect of mismatch on uniform         *
*             quantization.                                            *
*  Author : K. Sayood                                                  *
*  Last mod: 7/15/95                                                   *
*  Usage:  see usage(), for details see misnuq.doc or man  page        *
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
  float mean, var, secmom, qerr, msqe, snr, scale;
  float input, output,stepsize,low,prob[256], entropy;
  int i, num, numlev,numbits,label, distribution, ent_flag;
  float min_std, max_std, std_step;
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
  min_std = .3;
  max_std = 3.;
  std_step = .1;

  while((c=getopt(argc,argv,"l:q:n:d:b:m:s:eh"))!=EOF)
  {
   switch (c){
   case 'l':
         sscanf(optarg,"%d",&numlev);
         break;
   case 'q':
         sscanf(optarg,"%f",&stepsize);
         break;
   case 'n':
         sscanf(optarg,"%d", &num);
         break;
   case 'd':
         sscanf(optarg,"%d", &distribution);
         break;
   case 'b':
         sscanf(optarg,"%f", &min_std);
         break;
   case 'm':
         sscanf(optarg,"%f", &max_std);
         break;
   case 's':
         sscanf(optarg,"%f", &std_step);
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

  for(scale=min_std; scale<max_std; scale+=std_step)
  {
   mean = 0.0;
   secmom = 0.0;
   msqe = 0.0;
    if(ent_flag)
     for(i=0;i<numlev;i++)
       prob[i] = 0.0;


   for(i=0; i<num; i++)
   {
    input = scale * (float) rangen(distribution);
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
}

void usage()
{
  fprintf(stderr,"Usage: misuquan [-eh][-l numlev][-q step_size][-n num][-d distribution][-b min_std][-m max_std][-s std_step]\n");
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
  fprintf(stderr,"\t\t A value of 3 corresponds to Gamma distribution.  This \n");
  fprintf(stderr,"\t\t particular option should be used with care, as the function \n");
  fprintf(stderr,"\t\t generating the Gamma distribution is not entirely reliable.\n\n");
  fprintf(stderr,"\t min_std     :  The smallest standard deviation for the input.\n");
  fprintf(stderr,"\t max_std     :  The largest standard deviation for the input.\n");
  fprintf(stderr,"\t std_step    :  Increment of standard deviation.\n");
  fprintf(stderr,"\t This program can be used to explore variance mismatch between  \n");
  fprintf(stderr,"\t source and assumed distribution and pdf mismatch.  To explore \n");
  fprintf(stderr,"\t variance mismatch the input is multiplied by a variable - scale- \n");
  fprintf(stderr,"\t which is varied within a for loop.  The upper and lower \n");
  fprintf(stderr,"\t limits of scale are set to min_std and max_std, with a \n");
  fprintf(stderr,"\t stepsize of std_step. \n");
}
