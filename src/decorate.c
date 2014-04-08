/* This file is part of IRVM.

Copyright (C) 2014 Pablo de Oliveira

IRVM is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

IRVM is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with IRVM.  If not, see <http://www.gnu.org/licenses/>. */

#include <assert.h>

#include "decorate.h"
#include "error.h"
#include "primitives.h"

/* connect all the nodes in a linked list (throught the ->next pointers)
   in the order in which they should be evaluated.*/
static struct Node *
_compute_exec_order (struct Node *n, struct Node *prev)
{
  switch (n->kind)
    {
	case T_BINOP:
	  {
	    struct Binop *b = container_of (n, struct Binop, node);
	    struct Node *left = _compute_exec_order (b->left, prev);
	    struct Node *right = _compute_exec_order (b->right, left);
	    right->next = n;
	    return n;
	  }
	case T_MEM:
	  {
	    struct Mem *m = container_of (n, struct Mem, node);
	    struct Node *exp = _compute_exec_order (m->exp, prev);
	    exp->next = n;
	    return n;
	  }
	case T_CALL:
	  {
	    struct Call *c = container_of (n, struct Call, node);
	    struct Node *arg = NULL;
	    struct Node *last = prev;
	    list_for_each (c->arguments, arg, list)
	    {
	      last = _compute_exec_order (arg, last);
	    }
	    last->next = n;
	    return n;
	  }
	case T_ESEQ:
	  {
	    struct Eseq *e = container_of (n, struct Eseq, node);
	    struct Node *stm = _compute_exec_order (e->stm, prev);
	    struct Node *exp = _compute_exec_order (e->exp, stm);
	    exp->next = n;
	    return n;
	  }
	case T_MOVE_MEM:
	  {
	    struct MoveMem *m = container_of (n, struct MoveMem, node);
	    struct Node *orig = _compute_exec_order (m->orig, prev);
	    struct Node *dest = _compute_exec_order (m->dest, orig);
	    dest->next = n;
	    return n;
	  }
	case T_MOVE_TEMP:
	  {
	    struct MoveTemp *m = container_of (n, struct MoveTemp, node);
	    struct Node *orig = _compute_exec_order (m->orig, prev);
	    orig->next = n;
	    return n;
	  }
	case T_SXP:
	  {
	    struct Sxp *s = container_of (n, struct Sxp, node);
	    prev->next = n;
	    return _compute_exec_order (s->exp, n);
	  }
	case T_CJUMP:
	  {
	    struct Cjump *c = container_of (n, struct Cjump, node);
	    struct Node *left = _compute_exec_order (c->left, prev);
	    struct Node *right = _compute_exec_order (c->right, left);
	    right->next = n;
	    return n;
	  }
	case T_SEQ:
	  {
	    struct Seq *s = container_of (n, struct Seq, node);
	    struct Node *stm = NULL;
	    struct Node *last = n;
	    prev->next = n;
	    list_for_each (s->stms, stm, list)
	    {
	      last = _compute_exec_order (stm, last);
	    };
	    return last;
	  }
	default:
	  prev->next = n;
	  return n;
    }
  assert (0);
}

/* connect all the nodes in a linked list (throught the ->next pointers)
   in the order in which they should be evaluated.*/
void
compute_exec_order (struct Node *n)
{
  struct Node sentinel;
  _compute_exec_order (n, &sentinel);
}

/* find the label named NAME */
static struct Node *
_find_label (id name, struct Node *n)
{
  if (n->kind == T_LABEL)
    {
      struct Label *l = container_of (n, struct Label, node);
      if (l->name == name)
	    return n;
    }

  if (n->next != NULL)
    return _find_label (name, n->next);
  else
    return NULL;
}

/* find the label named NAME */
struct Node *
find_label (id name)
{
  return _find_label (name, irtree);
}

/* bind each Name to its label */
void
bind_labels (struct Node *n)
{
  switch (n->kind)
    {
	case T_NAME:
	  {
	    struct Name *a = container_of (n, struct Name, node);
	    struct Node *target = find_label (a->name);
	    if (!target)
	      errl (&n->loc, 1, "missing label (%s)", a->name);
	    struct Label *l = container_of (target, struct Label, node);
	    if (l->lit == 0)
	      errl (&n->loc, 1,
		    "target label (%s) has no string literal", a->name);
	    a->lit = l->lit;
	  }
	  break;
	case T_CALL:
	  {
	    struct Call *c = container_of (n, struct Call, node);

	    primitive p = is_primitive (c->name);
	    if (p)
	      {
		c->primitive = p;
	      }
	    else
	      {
		c->target = find_label (c->name);
		if (c->target == NULL)
		  errl (&n->loc, 1, "call missing label (%s)", c->name);
	      }
	  }
	  break;
	case T_JUMP:
	  {
	    struct Jump *j = container_of (n, struct Jump, node);
	    j->target = find_label (j->name);
	    if (j->target == NULL)
	      errl (&n->loc, 1, "jump missing label (%s)", j->name);
	  }
	  break;
	case T_CJUMP:
	  {
	    struct Cjump *c = container_of (n, struct Cjump, node);
	    c->bthen_target = find_label (c->bthen);
	    if (c->bthen_target == NULL)
	      errl (&n->loc, 1, "cjump missing label (%s)", c->bthen);

	    c->belse_target = find_label (c->belse);
	    if (c->belse_target == NULL)
	      errl (&n->loc, 1, "cjump missing label (%s)", c->belse);
	  }
	  break;
    default:
      break;
    }
  if (n->next != NULL)
    bind_labels (n->next);
}
