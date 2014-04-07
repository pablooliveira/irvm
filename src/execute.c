#include <assert.h>

#include "context.h"
#include "execute.h"
#include "error.h"
#include "mem.h"

#define TRACE(...)  do {if (trace) warnl(current_loc, __VA_ARGS__);} while(0)

struct location *current_loc;

const char *op2str[] = { "(+)", "(-)", "(*)", "(/)", "(%)" };
const char *rop2str[] = { "(<)", "(<=)", "(>)", "(>=)", "(<>)", "(=)" };

/* applies relative operator ROPR to LEFT and RIGHT operands */
static bool
do_cmp (enum RELOP ropr, int32_t left, int32_t right)
{
  switch (ropr)
    {
    case GT:
      return left > right;
    case LT:
      return left < right;
    case GE:
      return left >= right;
    case LE:
      return left <= right;
    case EQ:
      return left == right;
    case NE:
      return left != right;
    default:
      assert (0 && "illegal operator");
    }
}

/* applies arithmetic operator OPR to LEFT and RIGHT operands */
static int32_t
do_binop (enum OPER opr, int32_t left, int32_t right)
{
  switch (opr)
    {
    case ADD:
      return left + right;
    case SUB:
      return left - right;
    case MUL:
      return left * right;
    case DIV:
      return left / right;
    case MOD:
      return left % right;
    default:
      assert (0 && "illegal operator");
    }
}

/* runs the IR tree NODE, if TRACE is true each
   executed node is printed */
void
execute (struct Node *n, bool trace)
{

NEXT:
  if (n == NULL)
    errl (current_loc, 1, "program ended, but no label end was found");

  /* update current_loc, so error messages are localized */
  current_loc = &n->loc;

  switch (n->kind)
    {
    /* Statements */
    case T_MOVE_MEM:
        {
          struct MoveMem *m = container_of (n, struct MoveMem, node);
          int32_t value = get_result (m->orig);

          TRACE ("move mem [%d] %d", get_result (m->dest), value);
          ir_write (get_result (m->dest), value);
          n = n->next;
          goto NEXT;
        }
    case T_MOVE_TEMP:
        {
          struct MoveTemp *m = container_of (n, struct MoveTemp, node);
          int32_t value = get_result (m->orig);
          TRACE ("move temp %s %d", m->dest, value);
          set_temp (m->dest, value);
          n = n->next;
          goto NEXT;
        }
    case T_JUMP:
        {
          struct Jump *j = container_of (n, struct Jump, node);
          TRACE ("jump %s", j->name);
          n = j->target;
          goto NEXT;
        }
    case T_CJUMP:
        {
          struct Cjump *c = container_of (n, struct Cjump, node);
          int32_t left = get_result (c->left);
          int32_t right = get_result (c->right);

          if (do_cmp (c->ropr, left, right))
            {
              n = c->bthen_target;
              TRACE ("cjump %d %d %s [%s] %s", left, right,
                     rop2str[c->ropr], c->bthen, c->belse);
            }
          else
            {
              n = c->belse_target;
              TRACE ("cjump %d %d %s %s [%s]", left, right,
                     rop2str[c->ropr], c->bthen, c->belse);
            }

          goto NEXT;
        }

    /* Expressions */
    case T_CONST:
        {
          struct Const *c = container_of (n, struct Const, node);
          set_result (n, c->value);
          TRACE ("const %d", c->value);
          n = n->next;
          goto NEXT;
        }
    case T_TEMP:
        {
          struct Temp *t = container_of (n, struct Temp, node);
          set_result (n, get_temp (t->name));
          TRACE ("temp %s = %d", t->name, get_result (n));
          n = n->next;
          goto NEXT;
        }
    case T_BINOP:
        {
          struct Binop *b = container_of (n, struct Binop, node);

          int32_t left = get_result (b->left);
          int32_t right = get_result (b->right);

          set_result (n, do_binop (b->opr, left, right));
          TRACE ("binop %s %d %d = %d", op2str[b->opr], left,
                 right, get_result (n));
          n = n->next;
          goto NEXT;
        }
    case T_ESEQ:
        {
          struct Eseq *e = container_of (n, struct Eseq, node);

          set_result (n, get_result (e->exp));
          TRACE ("eseq = %d", get_result (n));
          n = n->next;
          goto NEXT;
        }
    case T_MEM:
        {
          struct Mem *m = container_of (n, struct Mem, node);

          set_result (n, ir_read (get_result (m->exp)));
          TRACE ("mem [%d] = %d", get_result (m->exp), get_result (n));
          n = n->next;
          goto NEXT;
        }
    case T_CALL:
        {
          struct Call *c = container_of (n, struct Call, node);
          struct Node *arg = NULL;

          TRACE ("call %s", c->name);

          /* first, read all arguments into an array */
          int count = 0;
          int32_t args[MAX_ARGS];
          list_for_each (c->arguments, arg, list)
            {
              if (count >= MAX_ARGS)
                errl (current_loc, 1, "too many arguments");
              args[count] = get_result (arg);
              count++;
            }

          /* then, push a new context */
          push_context (n);
          int i;
          for (i = 0; i < count; i++)
            {
              /* and set i0..in temporaries */
              set_temp (argid[i], args[i]);
              TRACE ("    %s = %d", argid[i], args[i]);
            }

          if (c->primitive)
            {
              /* a primitive is directly executed */
              int32_t result = c->primitive ();
              TRACE ("call end %s = %d", c->name, result);
              pop_context ();
              set_result (n, result);
              n = n->next;
            }
          else
            {
              /* a user defined function requires a jump */
              n = c->target;
            }
          goto NEXT;
        }
    case T_LABEL:
        {
          struct Label *l = container_of (n, struct Label, node);

          /* label end is special: it marks the end of a function */
          if (l->name == end)
            {
              /* result is in the rv temporary */
              int32_t result = get_temp (rv);

              /* pop the context */
              struct Node *caller = pop_context ();

              /* NULL is used as a sentinel in the context stack,
                 it marks the end of the program */
              if (caller == NULL)
                {
                  TRACE ("end of program");
                  return;
                }
              current_loc = &caller->loc;

              struct Call *c = container_of (caller, struct Call, node);
              TRACE ("call end %s = %d", c->name, result);

              /* set result of function into the caller node */
              set_result (caller, result);

              /* return to the caller */
              n = caller->next;
            }
          else
            {
              /* other labels can be safely skipped */
              TRACE ("label %s", l->name);
              n = n->next;
            }
          goto NEXT;
        }
    case T_NAME:
        {
          struct Name *a = container_of (n, struct Name, node);
          int32_t result = a->lit;
          set_result (n, result);
          TRACE ("name %s = (%d) %s\n", a->name, result,
                 read_string (result));
          n = n->next;
          goto NEXT;
        }
    default:
      n = n->next;
      goto NEXT;
    }
  assert (0);
}
