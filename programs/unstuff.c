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

void unstuff(int numbits, FILE *fp, int *buffer, int *count)
{
  int c, bitsleft, readbits, input, bytesize;


  bytesize = 8;
  *count = 0;
  readbits = numbits;
  while((c=getc(fp))!=EOF)
  {
   bitsleft = 8;
   if(readbits != numbits)
   {
    input = input |((int) c >>(bytesize-numbits+readbits));
    bitsleft -= numbits-readbits;
    readbits = numbits;
    buffer[*count] = input;
    *count +=1;
   }
   while(bitsleft >= numbits)
   {
     input = (int) c & (0xff>>(bytesize-bitsleft));
     bitsleft -=numbits;
     input = input>>bitsleft;
    buffer[*count] = input;
    *count +=1;
    }
   
    if(bitsleft >0)
    {
     input = (int) c & (0xff>>(bytesize-bitsleft));
     input = input<<(numbits-bitsleft);
     readbits = bitsleft;
    }
   }
   
}

