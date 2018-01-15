#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
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
   int size, num, i;
   float error, sqrerror, abserror, sigpower, mse, mae, sig, snr;
   short *aufile1, *aufile2;

/* get filesize for the first audio file */

   get_file_size(argv[1],&size);

  num = size/2;
  fprintf(stderr,"Number of data points is %d\n",num);

/* get memory for the audio or speech file   */

  aufile1 = (short *) calloc(num+8,sizeof(short));
  aufile2 = (short *) calloc(num+8,sizeof(short));

/*   Get the audio data */

  readau(argv[1],aufile1);
  readau(argv[2],aufile2);


/* Compute performance metrics */

  sqrerror = 0.0;
  abserror = 0.0;
  sigpower = 0.0;

  for(i=0; i<num; i++)
  {
   sig   = (float) aufile1[i];
   error = (float) (aufile1[i]-aufile2[i]); 
   sqrerror += error*error;
   abserror += myabs(error);
   sigpower += sig*sig;
  }

  mse = sqrerror/ (float) num;
  mae = abserror/ (float) num;
  printf("mean squared error = %5.2f, mean absolute error = %5.2f\n",mse,mae);
  sig = sigpower/ (float) num;
  snr = 10.0 * (float) (log((double) (sig/mse))/log((double) 10.));

 printf("Signal to noise ratio %f\n",snr);
}

void usage(void)
{

  fprintf(stderr,"Usage:\n distau file1 file2\n");
  fprintf(stderr,"\t file1 : file containing the audio or speech input\n");
  fprintf(stderr,"\t file2 : file containing the reconstructed output\n");
  fprintf(stderr,"\t -h  : results in the display of this message\n");
}
