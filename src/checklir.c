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

#include "checklir.h"
#include "error.h"

/* check that the input program is valid LIR */
static bool
_checklir (struct Node *n, struct Node *parent, int nb_seqs)
{
  if (n == NULL)
    return true;

  switch (n->kind)
    {
    case T_BINOP:
        {
          struct Binop *b = container_of (n, struct Binop, node);
          bool left = _checklir (b->left, n, nb_seqs);
          bool right = _checklir (b->right, n, nb_seqs);
          return left && right;
        }
    case T_MEM:
        {
          struct Mem *m = container_of (n, struct Mem, node);
          return _checklir (m->exp, n, nb_seqs);
        }
    case T_CALL:
        {
          if (!parent ||
              (parent->kind != T_MOVE_MEM &&
               parent->kind != T_MOVE_TEMP && parent->kind != T_SXP))
            {
              warnl (&n->loc,
                     "calls must be wrapped in move or sxp nodes in LIR");
              return false;
            }

          struct Call *c = container_of (n, struct Call, node);
          struct Node *arg = NULL;
          bool valid = true;
          list_for_each (c->arguments, arg, list)
            {
              valid = _checklir (arg, n, nb_seqs) && valid;
            }
          return valid;
        }
    case T_ESEQ:
        {
          warnl (&n->loc, "eseq nodes are forbidden in LIR");
          return false;
        }
    case T_MOVE_MEM:
        {
          struct MoveMem *m = container_of (n, struct MoveMem, node);
          bool orig = _checklir (m->orig, n, nb_seqs);
          bool dest = _checklir (m->dest, n, nb_seqs);
          return orig && dest;
        }
    case T_MOVE_TEMP:
        {
          struct MoveTemp *m = container_of (n, struct MoveTemp, node);
          return _checklir (m->orig, n, nb_seqs);
        }
    case T_SXP:
        {
          struct Sxp *s = container_of (n, struct Sxp, node);
          return _checklir (s->exp, n, nb_seqs);
        }
    case T_CJUMP:
        {
          struct Cjump *c = container_of (n, struct Cjump, node);
          bool left = _checklir (c->left, n, nb_seqs);
          bool right = _checklir (c->right, n, nb_seqs);

          if (n->next != c->belse_target)
            {
              warnl (&n->loc,
                     "cjump must be followed by its false label in LIR");
              return false;
            }

          return left && right;
        }
    case T_SEQ:
        {
          if (nb_seqs > 0)
            {
              warnl (&n->loc, "no nested seq in LIR");
              return false;
            }
          struct Seq *s = container_of (n, struct Seq, node);
          struct Node *stm = NULL;
          bool valid = true;
          list_for_each (s->stms, stm, list)
            {
              valid = _checklir (stm, n, nb_seqs + 1) && valid;
            }
          return valid;
        }
    default:
      return true;
    }
  assert (0);
}

/* check that the input program is valid LIR */
bool
checklir (void)
{
  return _checklir (irtree, NULL, -1);
}
