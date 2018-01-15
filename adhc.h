#define NYT MAX
/* NYT = Not Yet Transmitted */
#define INTERNAL_NODE MAX+1

typedef struct nodetype
 {
  int weight;
  int symbol;
  struct nodetype *left, *right, *parent; /* tree structure */ 
  struct nodetype *next, *prev; /* doubly-linked list */
  struct nodetype **head; /* highest ranked node in block */
 } Node;

void dumptree(Node *node);
void dumplist(Node *node);
void usage(char **argv);
void swap (Node *node1, Node *node2);
void swaplist(Node *node1, Node *node2);
void increment(Node *node);
void checkranks(Node *node);
void add_ref(int ch);
