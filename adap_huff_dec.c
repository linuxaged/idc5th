/* Adaptive Huffman decoding program */
/* Hal Burch */
/* Sept 16, 1996 */

/* This is based on the Adaptive Huffman algorithm described in Sayood's Data
 * Compression book.  The ranks are not actually stored, but implicitly defined
 * by the location of a node within a doubly-linked list */


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

#define MAX 256 /* Maximum symbol */
#define N 8 /* number of bits */

#include <stdio.h>
#include <assert.h>
#include <malloc.h>
#include <string.h>
#include "adhc.h"

char buff[128];
int bloc=128*8;
int outcch=0;
Node *tree;
Node *lhead, *ltail;
Node *loc[MAX+1];

int get_bit (FILE *fin)
 { /* Receive one bit from the input file (buffered) */
  int t;
  if (bloc == 128*8)
   {
    t = fread(buff, sizeof(char), 128, fin);
    assert (t >= 0);
    bloc = 0;
   }
  t = (buff[bloc/8] >> (7-(bloc % 8))) & 0x1;
#ifdef WATCH
  if (bit == 1) printf (".");
  else printf (" ");
#endif
  bloc++;
  return t;
 }

int receive (Node *node, int *ch, FILE *fin)
 { /* Get a symbol */
  while (node && node->symbol == INTERNAL_NODE)
   {
    if (get_bit(fin))
      node = node->right;
    else
      node = node->left;
   }
  if (!node)
   {
    fprintf (stderr, "Illegal tree!\n");
    exit(5);
   }
  return (*ch = node->symbol);
 }

void main(int argc, char **argv)
 {
  FILE *fin;
  FILE *fout;
  int ch;
  int cch;
  char tch;
  char tw[4];
  if (argc != 3)
   {
    usage(argv);
    exit(3);
   }

  if ((fin = fopen(argv[1], "r")) == NULL)
   {
    fprintf (stderr, "Cannot open input file.\n");
    usage(argv);
    exit(3);
   }

  if ((fout = fopen(argv[2], "w")) == NULL)
   {
    fprintf (stderr, "Cannot open output file.\n");
    usage(argv);
    exit(3);
   }

/* Initialize the tree & list with the NYT node */
  tree = lhead = ltail = loc[NYT] = (Node *)malloc(sizeof(Node));
  tree->symbol = NYT;
  tree->weight = 0;
  lhead->next = lhead->prev = NULL;
  tree->parent = tree->left = tree->right = NULL;

/* Read in the number of symbols */
  if (fseek(fin, -4L, 2) == -1)
   {
    fprintf (stderr, "Seek failed.\n");
    exit(4);
   }
  fread(tw, sizeof(tw[0]), sizeof(tw), fin);
  cch = 0;
  for (ch = 7; ch >= 0; ch--)
    cch = cch*2 + ((tw[0] >> ch) & 0x1);
  for (ch = 7; ch >= 0; ch--)
    cch = cch*2 + ((tw[1] >> ch) & 0x1);
  for (ch = 7; ch >= 0; ch--)
    cch = cch*2 + ((tw[2] >> ch) & 0x1);
  for (ch = 7; ch >= 0; ch--)
    cch = cch*2 + ((tw[3] >> ch) & 0x1);
  
  fseek(fin, 0, 0);

  while (cch--)
   {
    if (cch % 1024 == 0)
     {
      printf (".");
      fflush (stdout);
     }

/* Get a character */
    receive(tree, &ch, fin);
    if (ch == NYT)
     { /* We got a NYT, get the symbol associated with it */
      int i;
      ch = 0;
      for (i = 0; i < N; i++)
        ch = 2*ch + get_bit(fin);
     }
    
/* Write symbol */
    tch = ch; 
    fwrite (&tch, sizeof(char), 1, fout);

/* Increment node */
    add_ref(ch);

#ifdef TRACE    
    dumptree(tree);
    dumplist(lhead);
#endif
   }

  printf ("\n");

  fclose(fin);
  fclose(fout);
 }
