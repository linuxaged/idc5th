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

extern Node *tree;
extern Node *lhead;
extern Node *loc[MAX+1];
static Node **freelist=NULL;

static Node **get_ppnode(void)
 {
  Node **tppnode;
  if (!freelist)
    return (Node **)malloc(sizeof(Node *));
  else
   {
    tppnode = freelist;
    freelist = (Node **)*tppnode;
    return tppnode;
   }
 }

static void free_ppnode(Node **ppnode)
 {
  *ppnode = (Node *)freelist;
  freelist = ppnode;
 }

void checkranks(Node *node)
 {
  Node *lnode;
  for (lnode = node; lnode; lnode = lnode->next) 
   {
    assert(!lnode->next || lnode->next->weight >= lnode->weight);
    assert(lnode->head && *(lnode->head) && (*lnode->head)->weight == lnode->weight);
    if (lnode->next)
      if (lnode->weight == lnode->next->weight)
        assert(lnode->next->head == lnode->head);
      else
        assert(lnode->next->head != lnode->head);
   }
 }

void dumptree(Node *node)
 { /* Debugging routine...dump the tree in prefix notation */
  if (!node) return;
  if (node->symbol == NYT)
    printf ("@");
  else if (node->symbol == INTERNAL_NODE)
    printf (".");
  else if (node->symbol == '\n')
    printf ("%%");
  else printf ("%c", node->symbol);
  if (node->left)
    assert(node->left->parent == node);
  if (node->right)
    assert(node->right->parent == node);
  dumptree(node->left);
  dumptree(node->right);
  if (node == tree)
    printf ("\n");
 }

void dumplist(Node *node)
 { /* Debugging routine...dump the link list */
  while (node)
   {
    if (!node) return;
    if (node->symbol == NYT)
      printf ("@");
    else if (node->symbol == INTERNAL_NODE)
      printf (".");
    else if (node->symbol == '\n')
      printf ("%%");
    else printf ("%c", node->symbol);
    printf ("(%i)", node->weight);
    node = node->next;
   }
  printf ("\n");
 }

void usage(char **argv)
 {
  fprintf (stderr, "Usage: %s inputfile outputfile\n", argv[0]);
 }

void swap (Node *node1, Node *node2)
 { /* Swap the location of these two nodes in the tree */
  Node *par1, *par2;

  assert (node1->symbol != NYT && node2->symbol != NYT);

  par1 = node1->parent;
  par2 = node2->parent;

  if (par1)
   {
    if (par1->left == node1)
      par1->left = node2;
    else
     {
      assert(par1->right == node1);
      par1->right = node2;
     }
   }
  else tree = node2;

  if (par2)
   {
    if (par2->left == node2)
      par2->left = node1;
    else
     {
      assert(par2->right == node2);
      par2->right = node1;
     }
   }
  else
    tree = node1;
  
  node1->parent = par2;
  node2->parent = par1;
 }

void swaplist(Node *node1, Node *node2)
 { /* Swap these two nodes in the linked list (update ranks) */
  Node *par1;

  par1 = node1->next;
  node1->next = node2->next;
  node2->next = par1;

  par1 = node1->prev;
  node1->prev = node2->prev;
  node2->prev = par1;

  if (node1->next == node1)
    node1->next = node2;
  if (node2->next == node2)
    node2->next = node1;

  if (node1->next)
    node1->next->prev = node1;
  if (node2->next)
    node2->next->prev = node2;
  if (node1->prev)
    node1->prev->next = node1;
  if (node2->prev)
    node2->prev->next = node2;
  assert(node1->next != node1);
  assert(node2->next != node2);
 }

void increment(Node *node)
 { /* Do the increments */
  Node *lnode;

  if (!node) return;

  if (node->next != NULL && node->next->weight == node->weight)
   {
    lnode = *node->head;
    assert(lnode != node);
    assert(lnode->parent != node);
    assert(lnode->weight == node->weight);
    if (lnode != node->parent)
      swap(lnode, node);
    else
      assert(node->next == lnode);
    swaplist(lnode, node);
   }
  if (node->prev && node->prev->weight == node->weight)
    *node->head = node->prev;
  else
   {
    *node->head = NULL;
    free_ppnode(node->head);
   }
  node->weight++;
  if (node->next && node->next->weight == node->weight)
    node->head = node->next->head;
  else
   { 
    node->head = get_ppnode();
    *node->head = node;
   }
  if (node->parent)
   {
    increment(node->parent);
    if (node->prev == node->parent)
     {
      swaplist(node, node->parent);
      if (*node->head == node)
        *node->head = node->parent;
     }
   }
 }

 void add_ref(int ch)
  {
   Node *tnode, *tnode2;
   if (loc[ch] == NULL)
    { /* if this is the first transmission of this node */
     tnode = (Node *)malloc(sizeof(Node));
     tnode2 = (Node *)malloc(sizeof(Node));

     tnode2->symbol = INTERNAL_NODE;
     tnode2->weight = 1;
     tnode2->next = lhead->next;
     if (lhead->next)
      {
       lhead->next->prev = tnode2;
       if (lhead->next->weight == 1)
         tnode2->head = lhead->next->head;
       else
        {
         tnode2->head = get_ppnode();
         *tnode2->head = tnode2;
        }
      }
     else
      {
       tnode2->head = get_ppnode();
       *tnode2->head = tnode2;
      }
     lhead->next = tnode2;
     tnode2->prev = lhead;
 
     tnode->symbol = ch;
     tnode->weight = 1;
     tnode->next = lhead->next;
     if (lhead->next)
      {
       lhead->next->prev = tnode;
       if (lhead->next->weight == 1)
         tnode->head = lhead->next->head;
       else
        {
         assert(0); /* this should never happen */
         tnode->head = get_ppnode();
         *tnode->head = tnode2;
        }
      }
     else
      {
       assert(0); /* this should never happen */
       tnode->head = get_ppnode();
       *tnode->head = tnode;
      }
     lhead->next = tnode;
     tnode->prev = lhead;
     tnode->left = tnode->right = NULL;
 
     if (lhead->parent)
      {
       if (lhead->parent->left == lhead) /* lhead is guaranteed to by the NYT */
         lhead->parent->left = tnode2;
       else
        {
         assert(lhead->parent->right == lhead);
         lhead->parent->right = tnode2;
        }
      }
     else
       tree = tnode2; 
 
     tnode2->right = tnode;
     tnode2->left = lhead;
 
     tnode2->parent = lhead->parent;
     lhead->parent = tnode->parent = tnode2;
     
     loc[ch] = tnode;
 
     increment(tnode2->parent);
    }
   else
     increment(loc[ch]);
 }
