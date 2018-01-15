#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>


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

int main(int argc, char **argv)
{
  FILE *fp1, *fp2;
  int c, row, col, row_size, col_size, peak_signal_value;
  float error, total_squared_error, total_absolute_error, total_signal_power;
  float mse, mae, snr, psnr;
  char name1[50], name2[50];
  unsigned char **img1, **img2;
  extern int optind;
  extern char *optarg;

  optind++;
  strcpy(name1,argv[1]);


  optind++;
  strcpy(name2,argv[2]);
  peak_signal_value = 255;
  row_size = -1;
  col_size = -1;

  while((c=getopt(argc,argv,"p:x:y:h"))!=EOF)
  {
   switch (c){
   case 'p':
         sscanf(optarg,"%d",peak_signal_value);
         break;
   case 'x':
         sscanf(optarg,"%d",col_size);
         break;
   case 'y':
         sscanf(optarg,"%d",row_size);
         break;
   case 'h':
         usage();
         exit(1);
              }
   }


  if(row_size < 0 || col_size < 0)
    image_size(name1, &row_size, &col_size);
  printf("%s %d %d\n",name1,row_size,col_size);

/* Assign space for image 1 */

  img1  = (unsigned char **) calloc(row_size,sizeof(char *));
  for(row=0; row< row_size; row++)
    img1[row] = (unsigned char *) calloc(col_size,sizeof(char));
 
  readimage(name1, img1, row_size, col_size);

/* Assign space for image 2 */

  img2  = (unsigned char **) calloc(row_size,sizeof(char *));
  for(row=0; row< row_size; row++)
    img2[row] = (unsigned char *) calloc(col_size,sizeof(char));
 
  readimage(name2, img2, row_size, col_size);
  printf("%s %d %d\n",name2,row_size,col_size);

/*  Initialize distortion metrics */

  total_squared_error = 0.0;
  total_absolute_error = 0.0;
  total_signal_power = 0.0;

  for(row=0; row< row_size; row++)
    for(col=0; col< col_size; col++)
     {
      error = (float) (img1[row][col] - img2[row][col]);
      total_squared_error += (float) (error * error);
      total_absolute_error += (float) fabsf(error);
      total_signal_power += (float) (img1[row][col]*img1[row][col]);
     }

  mse = total_squared_error / (float) (row_size*col_size);
   fprintf(stderr,"mean squared error = %5.2f\n",mse);
  mae = total_absolute_error / (float) (row_size*col_size);
   fprintf(stderr,"mean absolute error = %5.2f\n",mae);
  snr = total_signal_power / total_squared_error;
  snr = 10. * (float) log10((double) snr);
   fprintf(stderr,"Signal-to-Noise Ratio = %5.2f\n",snr);
  psnr=(float) (peak_signal_value*peak_signal_value)/mse;
  psnr = 10. * (float) log10((double) psnr);
   fprintf(stderr,"Peak Signal-to-Noise Ratio = %5.2f\n",psnr);
 }
 
 void usage(void)
{
  fprintf(stderr,"The usage will appear here");
}  
