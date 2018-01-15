#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include "idc.h"
/**********************************************************************
*                                                                      *
*  File: entfile.c                                                     *
*  Function:  computes zero order entropy assuming 256 letter alphabet.*
*  Author  : K. Sayood                                                 *
*  Last mod: 7/15/95                                                   *
*  Usage:  see usage(), for details see entfile.doc or man page        *
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

void usage(void);

int main(int argc, char **argv)
{
  FILE *ifp;
  int i, c, size, hist[256];
  float prob[256], count, entropy;
  char name1[50];

  strcpy(name1,argv[1]);

  if((ifp = fopen(argv[1],"r")) == NULL)
   {
    fprintf(stderr,"Input open failed\n");
    usage();
    exit(1);
   }


/* Compute the histogram  */ 

  for(i=0; i<256; i++)
   hist[i] = 0;

  size = 0;
  while((c=getc(ifp))!=EOF)
  {
   hist[c]++;
   size++;
  }

  count = (float) (size);

/*  Compute the zero order probability model  */

  for(i=0; i< 256; i++)
      prob[i]= (float) hist[i]/ count; 
/*  Compute the entropy  */ 

  entropy = ent(prob,256);
  fprintf(stderr,"Entropy of file is %5.2f\n",entropy);

 }
 
 void usage(void)
{
  fprintf(stderr,"usage:\n \t entfile filename [-l size] [-h]\n");
  fprintf(stderr,"\t\t filename: name of file for which zero order entropy is\n");
  fprintf(stderr,"\t\t\t   to be calculated\n");
  fprintf(stderr,"\t\t size    : Size of file in bytes\n");
}  
