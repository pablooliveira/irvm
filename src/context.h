#include "irvm.h"

/* The CONTEXT module handles the temporary table and keeps the
   node evaluation results. */

void init_context (void);
void free_context (void);

/* push a new context. This function is used when calling a new IR function.
   It saves the return node that is later used to return to the caller.
   It also creates a new temp and node evaluation context. */
void push_context (struct Node * retnode);

/* pops a context. This function is used when returning from an IR function
   call. It returns the caller node. */
struct Node * pop_context (void);

/* set the value of temporary NAME to VALUE */
void set_temp (id name, int32_t value);

/* get the value of temporary NAME */
int32_t get_temp (id name);

/* set the evaluation of node NODE to VALUE */
void set_result (struct Node *node, int32_t value);

/* retrieves the last evaluation of node NODE */
int32_t get_result (struct Node *node);
