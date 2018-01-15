#include <stdio.h>
/*

        imsub.c

        This is a file of subroutines

        create_code, create_list, huff, getim, storeim, and sort

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

/* This subroutine writes code for a binary tree 
   the code is placed in an array named code
   the length of the code is placed in an array named length 

   This subroutine checks to see if the node it is given when it is called
   is at the end of the tree.  If it is the code and length for this node is 
   written to arrays. If it is not at the end of the tree the code for the
   next node in the tree is written.  This subroutine travels left first
   assigning zeros for each step left and then travels right assigning
   ones for each step right. */ 

void create_code(NODE *root, int lgth, unsigned int *code, char *length)
{
	if (root -> left != NULL){ 

	  /* writes code for next node */
		root->left->code = root -> code;
		root -> left -> code=root->left->code << 01;

	  /* keep track of length of code */
		++lgth;	

	  /* call subroutine again but with next node left as root node */
		create_code(root->left,lgth,code,length);
		}
	if (root -> right != NULL){

          /* writes code for next node */
		root -> right -> code = root -> code;
		root -> right -> code = root -> right -> code << 01;

	  /* add one for step right */
		root -> right -> code += 1;

          /* call subroutine again but with next node right as root node */
		create_code(root->right,lgth,code,length);
		}
	if (root -> left == NULL && root -> right == NULL){

	  /* write code and length to arrays, 
	     code and length are written in the same order
	     in the code and length arrays as the probability
	     they go with before the probabilities were sorted */

		code[root->l] = root -> code; 
		length[root->l] = lgth;
	}
}

/* This subroutine creates a binary tree */

BTREE create_list (float prob[],int loc[],int num)
{
	NODE *head,*tail,*f,*b,*e,*par; /* pointers to nodes */
	int i,j,k,count,x; /* counters */

	count=num-1; /* counter used when assigning parents */

	if (num>0){
		head=(NODE *) malloc(sizeof(NODE)); /* head is the start of the list */
		head -> pro = prob[0];
		head -> l = loc[0];
		head -> left = NULL;
		head -> right = NULL;

		tail=head; /* tail is the end of the list */

/* create a list with a node for each probability 
   and conect them together using b as a pointer */
        for (j=1 ;j<num;j++){ 
		tail->forward=(NODE *) malloc(sizeof(NODE));
		b=tail;
		tail=tail->forward;
		tail->back=b;
		tail -> pro = prob[j];
		tail -> l = loc[j];
		tail -> left = NULL;
		tail -> right = NULL;

	} 

/* create the tree by creating parents
   after each parent is created its probability is equal to the sum
   of its children
   then the parent is inserted into the list so it is still sorted
   by probabilities
*/
	for (i=1;i<num;i++){
		tail -> parent =(NODE *) malloc(sizeof(NODE));
		par=tail->parent; /* par is a pointer of parent */
		par -> right = tail;
		par -> left = tail -> back;
		b=tail;
		tail = tail->back;
		tail->parent=par;
		b -> back = NULL; /* unhook original list */
		tail -> forward = NULL; /* unhook original list */
		par -> pro = par->right->pro + par->left->pro;
if(count!=1){ 
		b=tail;
		tail=tail->back;
		b->back=NULL; /* unhook original list */
		tail->forward=NULL; /* unhook original list */
}

/* insert parent into list */
		b=head;
		x=0; /* marker to see if parent has been placed in list */
		for(j=1;j<count;j++){
			if(b!=NULL){
				if((b->pro <= par->pro) && x==0){
 
				if(b==head){

			/* probability is larger than largest probability
			   in the list so it becomes the head */
					par->forward=b;
					b->back=par;
					head=par; 
					x=1; 
				}
				if(x!=1){

			/* f is a pointer to a node 
			   the probability of f is larger than parent's
			   the probability of b is smaller then parent's
			   parent is placed between f and b */
					f=b->back;
					f->forward=par;
					par->back=f;
					par->forward=b;
					b->back=par;
					x=1;
				}
				}
		if(x==0 && count!=0)
			b=b->forward;
}
		}

	/* if the probability of parent is the smallest on the list 
	   x will equal zero */
		if(x==0 && tail->pro>par->pro){
			tail->forward=par;
			par->back=tail;
			tail=par;
		}

		count--;
	}
		} 

/* tree is created */
		head=par; /* the last parent created becomes the start of 
			     the tree */
		head->code=0; /* the code for the first node is set to
				 zero */
	return (BTREE) head;

}

/* This subroutine creates huffman code */

void huff(float prob[],int loc[],int num, unsigned int *code, char *length)
{
	NODE *tree; /* pointer to a node */
	int lgth;

	lgth=0;
	tree=(NODE *)create_list(prob,loc,num); /* create tree */
	create_code(tree,lgth,code,length); /* get code from tree */
}

/* This subroutine gets an image */

void getim(char *fname, unsigned char *image, int size)
{
	FILE *fp;

	if ((fp=fopen(fname,"rb"))==NULL){
		fprintf(stderr,"Image file cannot be opened for input.\n");
		return;
	}
	printf ("Input file opened successsful.\n");
	fread(image,sizeof(unsigned char),size,fp);
	fclose(fp);
}

/* This subroutine stores an image to a file */

void storeim(char *fname, unsigned char *image, int size)
{
	FILE *fp;

	if ((fp=fopen(fname,"wb"))==NULL){
		fprintf(stderr,"File cannot be opened for output.\n");
		return;
		}
		fwrite(image,sizeof(unsigned char),size,fp);
}

/* This subroutine sorts an array */

void sort(float *value, int *loc,int num)
{
	int i,j,k; /* counters */
	int maxloc; /* location of largest probability */
	float *svalue; /* arrray to hold sorted values */
	float max; /* largest probability in the array */

/* get memory for an array of sorted probabilities and set it to zero */

	svalue=(float *)malloc(num*sizeof(float));
	for(i=0;i<num;i++)
		svalue[i]=0.0;

	i=num-1;
	for(j=0;j<num;j++){
		if(value[j]==0.0){
                        svalue[i]=value[j];
                        loc[i]=j;
			--i;
		}
	}
	k=0; 
	for(i=0;i<num;i++){
		max=0.0;
/* find largest probability */
		for(j=0;j<num;j++){
			if(value[j]>max){
				max=value[j];
				maxloc=j;
			}
		}
		if(max!=0.0){
			svalue[k]=value[maxloc];
			loc[k]=maxloc;
/* largest probability is set to zero */
			value[maxloc]=0.0;
			k++;
		}
		}
/* sorted probabilities are written to original array */
	for(i=0;i<num;i++)
		value[i]=svalue[i];
}
