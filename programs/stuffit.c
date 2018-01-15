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

void stuffit(int label, int numbits, FILE *fp, int end_flag)
{
   static char save = 0;
   static filled = 0;
   int move, bytesize;
   bytesize = 8;
   
   if(numbits > 16)
   {
    fprintf(stderr,"This routine cannot handle more than 16 bits per label\n");
    exit(1);
   }

          move = bytesize-filled-numbits;
          if(move >= 0)
          {
           save = save | (label<<move);
           filled += numbits;
           if(filled == bytesize)
           {
            putc(save,fp);
            save = 0;
            filled = 0;
           }
          }
          else
          {
           save = save | (label >> -move);
           putc(save,fp);
           save = 0;
           move = bytesize+move;
           if( move >= 0)
           {
            save = save | (label<<move);
            filled = bytesize - move;
           }
           else
           {
            save = save | (label >> -move);
            putc(save,fp);
            save = 0;
            move = bytesize+move;
            save = save | (label<<move);
            filled = bytesize - move;
           }


          } 

          if(end_flag && filled != 0)
            putc(save,fp);
}
