
float jayquan(int numlev, int label, int mult[], float stepsize, float delmin, float delmax)
{

/***********************************************************************
*    This routine updates the quantizer stepsize using the Jayant      *
*    algorithm for quantization with one word memory.                  *
*                                                                      *
*      numlev :  Number of quantizer output points.                    *
*      label  :  integer label for the quantization region             *
*                label = 0,1,..,numlev-1                               *
*      mult[] : Contains the multiplier values for the Jayant algorithm*
*      stepsize: Size of the inner quantization regions.               *
*      delmin  : Minmimum value that the quantizer is allowed to take. *
*      delmax  : Maximum value that the quantizer is allowed to take.  *
*                                                                      *
*     file name: jayquan.c                                             *
*     last mod : 6/14/95                                               *
*     author   : K. Sayood                                             *
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

   if(label < numlev/2)
     label = numlev/2-1-label;
    else
     label = label - numlev/2;
   stepsize = mult[label]*stepsize;
   if(stepsize > delmax){
     stepsize=delmax;
                        }

   if(stepsize < delmin){
     stepsize = delmin;
                        }
   return stepsize;
}
