#include <math.h>
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


float ent(float p[], int num_symbols)
{

   int i;
   float entropy;

   entropy = 0.0;
   for(i=0; i< num_symbols; i++)
      if(p[i] > 0.0)
         entropy -= p[i] * (float) log( (double) p[i]);

   entropy = entropy/ (float) log ((double) 2.0);

   fprintf(stderr,"entropy in subroutine is %f\n",entropy);
   return entropy;
}
