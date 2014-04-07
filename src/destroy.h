#include <ccan/htable/htable.h>
#include "irvm.h"

/* DESTROY module is in charge of freeing the memory */

/* free all memory at the end of the program */
void destroy (void);

/* free an htable */
void free_htable (struct htable *ht);

/* free a IR Node tree */
void free_irtree (struct Node *n);
