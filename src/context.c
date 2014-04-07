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

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <ccan/htable/htable.h>
#include <ccan/hash/hash.h>
#include <ccan/list/list.h>

#include "context.h"
#include "error.h"
#include "irvm.h"
#include "destroy.h"

/* A temp structure */
struct temp
{
  /* Its key: may be and Id in the case of a temporary
     of a struct Node * in the case of a node eval */
  void *key;

  /* The value */
  uint32_t value;
};

/* A context structure */
struct context
{
  struct htable temps;
  struct htable results;
  struct Node * retnode;
  struct list_node list;
};

/* The stack of contexts */
struct list_head context_list;

void
init_context (void)
{
  list_head_init (&context_list);
  push_context (NULL);
}

void
free_context (void)
{
  while (!list_empty (&context_list))
    pop_context ();
}

/* rehash function for pointers */
static size_t
rehash (const void *e, void *unused __attribute__ ((unused)))
{
  return hash_pointer (((struct temp *) e)->key, 0);
}

/* equality between pointers */
static bool
ptreq (const void *e, void *id)
{
  return ((struct temp *) e)->key == id;
}

/* generic htable set function for temps */
static void
set (struct htable *ht, void *key, int32_t value)
{
  struct temp *t = htable_get (ht, hash_pointer (key, 0), ptreq, key);

  if (t == NULL)
    {
      t = malloc (sizeof *t);
      t->key = key;
      htable_add (ht, hash_pointer (key, 0), t);
    }

  t->value = value;
}

/* generic htable get function for temps */
static int32_t
get (struct htable *ht, void *key, char *errmsg)
{
  struct temp *t = htable_get (ht, hash_pointer (key, 0), ptreq, key);
  if (!t)
    errl (current_loc, 1, errmsg);
  return t->value;
}

/* deep copy ORIG htable to DEST htable */
static void
copy_htable (struct htable * dest, struct htable * orig)
{
  struct htable_iter i;
  struct temp *t = NULL;
  for (t = htable_first (orig, &i); t; t = htable_next (orig, &i))
    set (dest, t->key, t->value);
}

/* return the current context */
static struct context *
current_context ()
{
  return list_top (&context_list, struct context, list);
}

/* push a new context. This function is used when calling a new IR function.
   It saves the return node that is later used to return to the caller.
   It also creates a new temp and node evaluation context. */
void
push_context (struct Node * retnode)
{
  struct context *old = current_context ();
  struct context *new = malloc (sizeof *new);
  new->retnode = retnode;
  htable_init (&new->temps, rehash, NULL);
  htable_init (&new->results, rehash, NULL);
  if (old != NULL)
    copy_htable (&new->temps, &old->temps);
  list_add (&context_list, &new->list);
 }

/* pops a context. This function is used when returning from an IR function
   call. It returns the caller node. */
struct Node *
pop_context ()
{
  struct context *old = list_pop (&context_list, struct context, list);
  struct Node * retnode = old->retnode;
  free_htable (&old->temps);
  free_htable (&old->results);
  free (old);
  return retnode;
}

/* set the value of temporary NAME to VALUE */
void
set_temp (id name, int32_t value)
{
  set (&current_context ()->temps, name, value);
}

/* get the value of temporary NAME */
int32_t
get_temp (id name)
{
  return get (&current_context ()->temps, name, "temp accessed but never set");
}

/* set the evaluation of node NODE to VALUE */
void
set_result (struct Node *node, int32_t value)
{
  set (&current_context ()->results, node, value);
}

/* retrieves the last evaluation of node NODE */
int32_t
get_result (struct Node *node)
{

  return get (&current_context ()->results, node, "evaluation error");
}
