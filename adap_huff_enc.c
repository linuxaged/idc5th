/* Adaptive Huffman encoding program */
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
int bloc=0;
int outcch=0;
Node *tree;
Node *lhead;
Node *loc[MAX+1];

void flushbuff(FILE *fout)
 { /* Flush buffer */
  fwrite(buff, sizeof(char), (bloc + 7) / 8, fout);
  memset(buff, 0, sizeof(buff));
  bloc = 0;
 }

void add_bit (char bit, FILE *fout)
 { /* Add a bit to the output file (buffered) */
  assert(bit == 0 || bit == 1);
  if (bloc == sizeof(buff)*8)
   {
    fwrite (buff, sizeof(char), 128, fout);
    memset (buff, 0, sizeof(buff));
    bloc = 0;
   }
  buff[bloc/8] |= bit << (7-(bloc % 8));
#ifdef WATCH
  if (bit == 1) printf (".");
  else printf (" ");
#endif
  bloc++;
 }

void send(Node *node, Node *child, FILE *fout)
 { /* Send the prefix code for this node */
  if (node->parent)
    send(node->parent, node, fout);
  if (child)
   {
    if (node->right == child)
      add_bit(1, fout);
    else 
     {
      assert(node->left == child);
      add_bit(0, fout);
     }
   }
 }

void transmit (int ch, FILE *fout)
 { /* Send a symbol */
  int i;
  if (loc[ch] == NULL)
   { /* Node hasn't been transmitted, send a NYT, then the symbol */
    transmit(NYT, fout);
    for (i = N-1; i >= 0; i--)
      add_bit((ch >> i) & 0x1, fout);
   }
  else
    send(loc[ch], NULL, fout);
 }

void main(int argc, char **argv)
 {
  FILE *fin;
  FILE *fout;
  int ch;
  int cch;
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

/* Add the NYT (not yeat transmitted) node into the tree/list */
  tree = lhead = loc[NYT] = (Node *)malloc(sizeof(Node));
  tree->symbol = NYT;
  tree->weight = 0;
  lhead->next = lhead->prev = NULL;
  tree->parent = tree->left = tree->right = NULL;
  loc[NYT] = tree;

  cch = 0;
  while ((ch = getc(fin)) != EOF)
   {
    cch++;
    if (cch % 1024 == 0)
     {
      printf (".");
      fflush (stdout);
     }

/* Transmit symbol */
    transmit(ch, fout);
    
/* Do update */
    add_ref(ch);

#ifdef DEBUG
    checkranks(lhead->next);
#endif
#ifdef TRACE    
    dumptree(tree);
    dumplist(lhead);
#endif
   }

/* Flush anything remaining in the buffer */
  flushbuff(fout);

/* Write the length out */
  for (ch = 31; ch >= 0; ch--)
    add_bit((cch >> ch) & 0x1, fout);

/* Flush the length */
  flushbuff(fout);

  printf ("\n");

  fclose(fin);
  fclose(fout);
 }
