#include <stdio.h>


int readau(char filename[], short aufile[])
{

  int num;
  FILE *fp;
  short *p;

  num = 0;
  if((fp = fopen(filename,"r")) == NULL)
    fprintf(stderr,"open failed, bombed, died\n");
  else
    fprintf(stderr,"File %s opened for read\n",filename);

  p = aufile;
  while(fread(p++,2,1,fp)==1)
    num++;
  fprintf(stderr,"Number of data points is %d\n",num);

  return(num);
}

