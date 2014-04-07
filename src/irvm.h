#ifndef IRVM_IRVM_H
#define IRVM_IRVM_H

#include <stdint.h>
#include <ccan/list/list.h>
#include "id.h"

/* a location in the user's input program */
struct location
{
  int first_line;
  int first_column;

  int last_line;
  int last_column;
};

/* current location in the tree execution:
   used when reporting errors */
extern struct location *current_loc;

/* one IRVM runtime primitive handler */
typedef int32_t (*primitive) (void);

/* IR nodes */
enum NODE
{
  T_CONST,
  T_TEMP,
  T_BINOP,
  T_MEM,
  T_CALL,
  T_ESEQ,
  T_MOVE_MEM,
  T_MOVE_TEMP,
  T_SXP,
  T_JUMP,
  T_CJUMP,
  T_SEQ,
  T_LABEL,
  T_NAME
};

/* IR arithmetic operators */
enum OPER
{
  ADD,
  SUB,
  MUL,
  DIV,
  MOD
};


/* IR relative operators */
enum RELOP
{
  LT,
  LE,
  GT,
  GE,
  NE,
  EQ
};

/* The generic IR Node structure */
struct Node
{
  /* the kind of node */
  enum NODE kind;

  /* list element so it can be embedded in lists */
  struct list_node list;

  /* next node that should be executed after this one */
  struct Node *next;

  /* node location in the input program */
  struct location loc;
};

/* The root of the parsed IR tree */
extern struct Node *irtree;

/* Each kind of node has a specific structure
   that captures its peculiarities.

   Therefore each IR node has:

      * a specific structure that depends on its kind,
        for example struct Const for a CONST node.

      * a generic Node structure

   Usually when moving through the IR tree, we will use the outside generic
   Node structure.

   It is trivial to get the generic Node structure from the Specific structure
   through the node pointer.

   To get the Specific structure from the Generic structure is a bit harder.
   We use the ccan/container_of macros, that retrieve the outer specific
   structure by computing the memory offset of the node field. For example
   the following code gets the specific structure of a node:

      struct Const * get(struct Node * n)
      {
        assert(n.kind == T_CONST);
        return container_of(n, struct Const, node);
      }
 */

struct Const
{
  struct Node node;
  int32_t value;
};

struct Temp
{
  struct Node node;
  id name;
};

struct Binop
{
  struct Node node;
  enum OPER opr;
  struct Node *left;
  struct Node *right;
};

struct Mem
{
  struct Node node;
  struct Node *exp;
};

struct Call
{
  struct Node node;
  id name;
  struct Node *target;
  primitive primitive;
  struct list_head *arguments;
};

struct Eseq
{
  struct Node node;
  struct Node *stm;
  struct Node *exp;
};

struct MoveTemp
{
  struct Node node;
  id dest;
  struct Node *orig;
};

struct MoveMem
{
  struct Node node;
  struct Node *dest;
  struct Node *orig;
};

struct Sxp
{
  struct Node node;
  struct Node *exp;
};

struct Jump
{
  struct Node node;
  id name;
  struct Node *target;
};

struct Cjump
{
  struct Node node;
  enum RELOP ropr;
  struct Node *left;
  struct Node *right;
  id bthen;
  id belse;
  struct Node *bthen_target;
  struct Node *belse_target;
};

struct Seq
{
  struct Node node;
  struct list_head *stms;
};

struct Label
{
  struct Node node;
  id name;
  int32_t lit;
};

struct Name
{
  struct Node node;
  id name;
  int32_t lit;
};

#endif /* IRVM_IRVM_H */
