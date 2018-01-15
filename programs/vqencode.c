
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

int vqencode(input,codebook,codebook_size,dimension,distortion)
  int *input, **codebook;
  int codebook_size, dimension;
  float *distortion;
{
  float dist;
  int i, j, index;

  *distortion = 10000*dimension;
  index = 0;

  for (i = 0; i< codebook_size; i++)
  {
   dist = 0;
   for(j=0; j<dimension; j++)
    dist += (input[j] - codebook[i][j])*(input[j]-codebook[i][j]);
   if(dist < *distortion)
   {
    *distortion = dist;
    index = i;
   }
  }

   return index;
}
    
