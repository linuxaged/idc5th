#include <stdio.h>

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

float predictor(int i,int order,float pred[],float xf[], float *prediction)
{
  int k;

  if(i == 0)
   *prediction = 0.0;
  else if(i < order)
   {
    *prediction = 0;
    for(k=0; k<i; k++)
      *prediction += pred[k]*xf[i-1-k];
   }
  else
   {
    *prediction = 0;
    for(k=0; k < order; k++)
      *prediction += pred[k]*xf[i-1-k];
   }
  return (*prediction);
}
