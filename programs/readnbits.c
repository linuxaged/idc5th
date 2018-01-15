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

int readnbits(int nbits, FILE *fp)
{
  int value, bitstoread,c;
  static int bytesize = 8;
  static int bitsleft = 0;
  static int buffer   = 0;

  value = 0;
  bitstoread = nbits;
  if(bitsleft >= nbits)
  {
   bitsleft -= nbits;
   value = (int) 0xff & (buffer >> bitsleft);
   buffer = (int) buffer & (0xff >> (bytesize - bitsleft));
   return value;
  }
  else if(bitsleft > 0)
  {
   bitstoread = nbits - bitsleft;
   value = (int) 0xff & (buffer << bitstoread);
   if((c=getc(fp)) == EOF)
   {
    fprintf(stderr,"End of File encountered in routine readnbits\n");
    exit(1);
   }
   buffer = 0xff & c;
   bitsleft = bytesize - bitstoread;
   value = (int) value | (buffer>>bitsleft);
   buffer = (int) buffer & (0xff>>bitstoread);
   return value;
  }
  else
  {
   if((c=getc(fp)) == EOF)
   {
    fprintf(stderr,"End of File encountered in routine readnbits\n");
    exit(1);
   }
   buffer = 0xff & c;
   bitsleft = bytesize - bitstoread;
   value = (int) (buffer>>bitsleft);
   buffer = (int) buffer & (0xff>>bitstoread);
   return value;
  }
}
   
   
   

   
