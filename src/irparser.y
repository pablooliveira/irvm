%expect 0
%define api.pure full
%locations
%error-verbose
%{
#define YYLTYPE struct location
#include <assert.h>
#include <stdlib.h>
#include "irvm.h"
#include "error.h"
#include "mem.h"
void yyerror();
int yylex();
%}

%token BINOP CALL CJUMP CONST ESEQ SXP
%token JUMP LABEL MEM MOVE NAME SEQ
%token TEMP SEQEND CALLEND LABELEND

%union {
 int num;
 char * str;
 struct Node* node;
 struct list_head* list;
}

%token <str> string
%token <str> literal
%token <num> integer
%token <num> op
%token <num> rop

%type <node> Exp Stm;
%type <str> Name;
%type <list> Exps Stms Program;

%start Program
%%

Program:
    Stms
        {
            struct Seq *s = calloc(1,sizeof *s);
            s->node.kind = T_SEQ;
            s->stms = $1;
            irtree = &s->node;
        };

Exp :
      CONST integer
        {
            struct Const *c = calloc(1,sizeof *c);
            c->node.loc = @1;
            c->node.kind = T_CONST;
            c->value = $2;
            $$ = &c->node;
        }
    | TEMP string
        {
            struct Temp *t = calloc(1, sizeof *t);
            t->node.loc = @1;
            t->node.kind = T_TEMP;
            t->name = $2;
            $$ = &t->node;
        }
    | BINOP op Exp Exp
        {
            struct Binop *b = calloc(1,sizeof *b);
            b->node.loc = @1;
            b->node.kind = T_BINOP;
            b->opr = $2;
            b->left = $3;
            b->right = $4;
            $$ = &b->node;
        }
    | MEM Exp
        {
            struct Mem *m = calloc(1,sizeof *m);
            m->node.loc = @1;
            m->node.kind = T_MEM;
            m->exp = $2;
            $$ = &m->node;
        }
    | CALL Name Exps CALLEND
        {
            struct Call *c = calloc(1,sizeof *c);
            c->node.loc = @1;
            c->node.kind = T_CALL;
            c->name = $2;
            c->arguments = $3;
            $$ = &c->node;
        }
    | ESEQ Stm Exp
        {
            struct Eseq *e = calloc(1,sizeof *e);
            e->node.loc = @1;
            e->node.kind = T_ESEQ;
            e->stm = $2;
            e->exp = $3;
            $$ = &e->node;
        }
    | Name
        {
            struct Name *n = calloc(1,sizeof *n);
            n->node.loc = @1;
            n->node.kind = T_NAME;
            n->name = $1;
            $$ = &n->node;
        }
    ;

Name : NAME string
        {
            $$ = $2;
        };

Stm :
      MOVE MEM Exp Exp
        {
            struct MoveMem *m = calloc(1,sizeof *m);
            m->node.loc = @1;
            m->node.kind = T_MOVE_MEM;
            m->dest = $3;
            m->orig = $4;
            $$ = &m->node;
        }
    |
      MOVE TEMP string Exp
        {
            struct MoveTemp *m = calloc(1,sizeof *m);
            m->node.loc = @1;
            m->node.kind = T_MOVE_TEMP;
            m->dest = $3;
            m->orig = $4;
            $$ = &m->node;
        }
    | SXP Exp
        {
            struct Sxp *s = calloc(1,sizeof *s);
            s->node.loc = @1;
            s->node.kind = T_SXP;
            s->exp = $2;
            $$ = &s->node;
        }
    | JUMP Name
        {
            struct Jump *j = calloc(1,sizeof *j);
            j->node.loc = @1;
            j->node.kind = T_JUMP;
            j->name = $2;
            $$ = &j->node;
        }
    | CJUMP rop Exp Exp Name Name
        {
            struct Cjump *c = calloc(1,sizeof *c);
            c->node.loc = @1;
            c->node.kind = T_CJUMP;
            c->ropr = $2;
            c->left = $3;
            c->right = $4;
            c->bthen = $5;
            c->belse = $6;
            $$ = &c->node;
        }
    | SEQ Stms SEQEND
        {
            struct Seq *s = calloc(1,sizeof *s);
            s->node.loc = @1;
            s->node.kind = T_SEQ;
            s->stms = $2;
            $$ = &s->node;
        }
    | LABEL string
        {
            struct Label *l = calloc(1,sizeof *l);
            l->node.loc = @1;
            l->node.kind = T_LABEL;
            l->name = $2;
            l->lit = 0;
            $$ = &l->node;
        }
    | LABEL string literal
        {
            struct Label *l = calloc(1,sizeof *l);
            l->node.loc = @1;
            l->node.kind = T_LABEL;
            l->name = $2;
            current_loc = &@1;
            l->lit = new_string($3);
            free($3);
            $$ = &l->node;
        }
    ;

Exps :
        {
            struct list_head * h = malloc(sizeof *h);
            list_head_init(h);
            $$ = h;
        }
    | Exp Exps
        {
            list_add($2, &$1->list);
            $$ = $2;
        }
    ;

Stms :
        {
            struct list_head * h = malloc(sizeof *h);
            list_head_init(h);
            $$ = h;
        }
    | Stm Stms
        {
            list_add($2, &$1->list);
            $$ = $2;
        }
    ;

%%
void
yyerror(YYLTYPE* loc, const char * err)
{
    assert(err);
    warnl(loc, "%s", err);
}
