#include <stdio.h>
#include <unistd.h>

void usage(void);

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

main(int argc, char **argv)
{
  FILE *ifp,*ofp;
  int width,height,c;
  extern int optind;
  extern char *optarg;


  if((ifp=fopen(argv[1],"r"))==NULL)
   {
     fprintf(stderr,"File open failed for input file %s\n",argv[1]);
     usage();
     exit(1);
   }
  optind++;

  if((ofp=fopen(argv[2],"w"))==NULL)
   {
    fprintf(stderr,"File open failed for output file %s\n",argv[2]);
    usage();
    exit(2);
   }
  optind++;

  width = 256;
  height = 256;

  while((c=getopt(argc,argv,"w:h:")) != EOF)
  {
   switch (c){
    case 'h':
     sscanf(optarg,"%d", &height);
     break;
    case 'w':
     sscanf(optarg,"%d", &width);
     break;
             }
   }

  fprintf(ofp,"P5\n");
  fprintf(ofp,"%d %d\n",width,height);
  fprintf(ofp,"%d\n",255);

  while((c=getc(ifp)) != EOF) 
     putc(c,ofp);
}

void usage(void)
{
  fprintf(stderr,"This program converts a raw image file containing image\n");
  fprintf(stderr,"values in eight bits per pixel to a a .pgm format.\n");
  fprintf(stderr,"To use the program type:\n");
  fprintf(stderr,"    convpgm input_file output_file [-w <width>] [-h <height>]\n");
  fprintf(stderr," The default height and width are 256\n");
  return;
}
