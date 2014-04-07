/* This file is part of IRVM.

Copyright (C) 2014 Pablo de Oliveira

IRVM is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Foobar is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Foobar.  If not, see <http://www.gnu.org/licenses/>. */

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
