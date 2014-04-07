#include "irvm.h"

/* DECORATE module connects all tree nodes in a linked list following
   the correct execution order of the program. It also binds each jump
   and Name to its associated label. */

/* connect all the nodes in a linked list (throught the ->next pointers)
   in the order in which they should be evaluated.*/
void compute_exec_order (struct Node *n);

/* find the label named NAME */
struct Node * find_label (id name);

/* bind each Name to its label */
void bind_labels (struct Node *n);
