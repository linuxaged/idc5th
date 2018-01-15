#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
//#include <malloc.h>          /* for halloc */


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

int get_file_size(char file[], int *len)
{
/*
Written by James R. Nau, 6-Oct-89
Modified K. Sayood 13 July, 1995

Attempt to find the size of a file pointed to by *file using a call to stat.

         *file: pointer to filename containing image

Return values:
        0: ok
       -1: Couldn't use stat function
*/

   struct stat status;
   int res;

   res = stat (file, &status);
   if (res != 0)
   {
      fprintf(stderr,"Couldn't use stat function\n");
      return (-1);
   }
   *len = (int) status.st_size;


   return (0);
}
