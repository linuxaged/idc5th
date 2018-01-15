
/*  Given the boundary values for an integer uniform quantizer, and an
input this routine returns an integer code for the quantizer level. 
*/


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

int encuqi(int input, int *b, int numlev)

{
   int level;

   for(level=1; level<numlev; level++)
      if(input<*(b+level))
        return (level-1);
   return (numlev-1);
}


/*  Given the reconstruction levels for an integer uniform quantizer, and an
integer code for the quantizer level, this routine returns the reconstruction
value. 
*/


int decuqi(int level, int *y)

{
   return *(y+level);
}
