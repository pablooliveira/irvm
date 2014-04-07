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

#include <stdio.h>
#include <ccan/container_of/container_of.h>

#include "context.h"
#include "destroy.h"
#include "id.h"
#include "mem.h"
#include "primitives.h"

extern void yylex_destroy (void);

/* free all memory at the end of the program */
void
destroy (void)
{
  free_irtree (irtree);
  free_context ();
  free_primitives ();
  free_id ();
  free_mem ();
  yylex_destroy ();
}

/* free an htable */
void
free_htable (struct htable *ht)
{
  struct htable_iter i;
  void *s = NULL;
  for (s = htable_first (ht, &i); s; s = htable_next (ht, &i))
    {
      free (s);
    }
  htable_clear (ht);
}

/* free a IR Node tree */
void
free_irtree (struct Node *cur)
{
  while (cur)
    {
      /* The field node is always the first for all
       * the Ir Node structures */
      void *tmp = container_of (cur, struct Const, node);

      if (cur->kind == T_CALL)
        free (((struct Call *) tmp)->arguments);
      else if (cur->kind == T_SEQ)
        free (((struct Seq *) tmp)->stms);

      cur = cur->next;
      free (tmp);
    }
}
