#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


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


void norm( float nx[], int num, float *ave, float *std)
{
  int i;
  float secmom, var;

  *ave = 0;
  secmom = 0;

  for(i=0; i<=num; i++)
  {
    *ave += nx[i];
    secmom += nx[i] * nx[i];
  }

  *ave = *ave/(float) num;
  secmom = secmom/(float) num;
  var = secmom - (*ave)*(*ave);
  *std = (float) sqrt((double) var);

  fprintf(stderr,"num %d ave %f var %f std %f\n",num,*ave,var,*std);
  for(i=0; i<=num; i++)
    nx[i] = (nx[i] - *ave)/(*std);

}
