#include <stdio.h>
/*

	sub.c

	This is a file of subroutines

	getcode, diff, value, images, and files

*/

#include "idc.h"

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

/* This subroutine gets code and code lengths from a file
   code is put in an array named code and the code lengths
   are put in an array named length */

void getcode(FILE *fp, int num, unsigned int *code, char *length)
{
        fread(code,sizeof(unsigned int),num,fp); /* gets code from a file */
        fread(length,sizeof(char),num,fp); /* gets code lengths from file */
        fclose(fp);
}

/* This subroutine calculates the differences between pixals, 
   the differences are put in an array named dimage so they can
   be used later and do not have to be recalculated */	

void diff(int *diffs, unsigned char *image,int rows,int cols,int num, unsigned char *dimage)
{
	int i,j,d; /* i,j are counters, d is the difference between pixals */
	unsigned char p; /* p is an unsigned char used to write the 
			    differences into an array */

/* find diffs */

	d=0-*(image); /* the first character's difference is subtracted 
			 from zero */

        for(i=0;i<rows;i++){
                for(j=0;j<cols;j++){
                if(i!=0 && j==0)

	/* the first column's difference is found by subtracting the
	   pixal value from the pixal value above it */

		d=*(image+(i-1)*cols)-*(image+i*cols);
		if(j!=0)
	
	/* all other column's differences are calculated by subtracting
	   the pixal value from the pixal value before it */

			d=*(image+i*cols+j-1)-*(image+i*cols+j);
		if (d<0)
		   d+=256; /* if the difference is negative 256 is added to
			      it so there are only 256 different difference
			      values */
                ++diffs[d]; /* sums up how many times each differece 
			       occurs */
		p=d; /* p equals the difference */
		*(dimage+i*cols+j)=p; /* the differences are written to
				         an array */
		}
	}
}

/* This program counts the number of times a character is used in a file
   the results are placed in an array named values */

void value(int *values, unsigned char *image, int size,int num)
{

	int i,c;

        for(i=0;i<size;i++){
                c=*(image+i);
                ++values[c];
		}
}

/* This subroutine encodes an image */

void images(int rows,int cols,int *code,char *length,unsigned char *file,unsigned char *dimage)
{
	int i,j,k,l,count; /* counters */
	int d,num;
	unsigned int w,c;
	unsigned char word;

	l=0;
	word=0;
	count=0;
	num=256;

/* encode image */

        for(i=0;i<rows;i++){
                for(j=0;j<cols;j++){
                        d=*(dimage+i*cols+j); /* d=difference between pixals */
                        if(length[d]==1)
                                w=length[d];
                                else
                                w=(1<<(length[d]-1));
                        c=code[d];
                for(k=0;k<length[d];k++){
                        if((c & w) == w) /* checks bit value of codes */
                                ++word;
                        c<<=1;
                        ++l;
                        if(l==8){
                                *(file+count)=word; /* encoded image */
                                ++count; /* counts lenghth of encoded image */
                                word=0;
                                l=0;
			}
                        else
                                word<<=1;
		}
		}
	}

/* left shift word so highest bit is the next code word */

        if(l!=0){
                word=word<<(7-l);
                *(file+count)=word;
                ++count;
	}

/* end to image equals NULL */
                word=NULL;

        *(file+count)=word;
        ++count;
/* store count at end of length array */

        length[num]=count;

}

/* This subroutine encodes a file */

int files(int size,int *code,char *length,unsigned char *file)
{
        int i,k,l,count; /* counters */
        int v, num;
        unsigned int w,c;
        unsigned char word, *newfile;

	fprintf(stderr,"Inside files\n");
        l=0;
        word=0;
        count=0;
	num=256;

	newfile=(unsigned char*)malloc(2*size*sizeof(unsigned char));
        if (newfile==NULL){
                printf("Unable to allocate memory for file.\n");
                exit(1);
	}

/* encode file */

        for(i=0;i<size;i++){
                        v=*(file+i); /* character  */
/*			fprintf(stderr,"v=%d\n",v);
			fprintf(stderr,"length[v]=%d\n",length[v]);
			fprintf(stderr,"code[v]=%d\n",code[v]);
*/
			if(length[v]==1)
                                w=length[v];
                                else
                                w=(1<<(length[v]-1));
                        c=code[v];
                for(k=0;k<length[v];k++){
                        if((c & w) == w) /* checks bit value of codes */
                                ++word;
                        c<<=1;
                        ++l;
                        if(l==8){
                                *(newfile+count)=word; /* encoded file */
                                ++count; /* counts lenghth of encoded file */
                                word=0;
                                l=0;
			}
                        else
                                word<<=1;
		}
	}
	
	fprintf(stderr,"Coded\n");
/* left shift word so highest bit is the next code word */

        if(l!=0){
                word=word<<(7-l);
                *(newfile+count)=word;
                ++count;
	}
	fprintf(stderr,"After shift\n");

/* put encoded file array into old file array */

	for(i=0;i<count;i++)
		*(file+i)=*(newfile+i);

	fprintf(stderr,"Before return\n");
	return count;

}
