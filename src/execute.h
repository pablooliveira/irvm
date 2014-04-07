#include "irvm.h"

/* EXECUTE module is in charge of running the IR tree */

/* runs the IR tree NODE, if TRACE is true each
   executed node is printed */
void execute (struct Node * node, bool trace);
