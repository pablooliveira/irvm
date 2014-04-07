#include "irvm.h"

/* PRIMITIVES implements the IRVM runtime */

void init_primitives (void);
void free_primitives (void);

/* If NAME is a primitive, return it,
   otherwise return NULL */
primitive is_primitive (id name);
