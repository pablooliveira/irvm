%{
#include "irparser.h"
#include "irvm.h"
#include "error.h"

/* handle locations */
int yycolumn = 1;

#define MAX_LITERAL 32768

char strbuf[MAX_LITERAL];
int strpos;

#define string_append(c) do { \
     if (strpos >= MAX_LITERAL) \
         errl((struct location *)yylloc, 1, "literal too large"); \
     strbuf[strpos++] = (c); \
     } while (0)

#define YY_USER_ACTION yylloc->first_line = yylloc->last_line = yylineno; \
    yylloc->first_column = yycolumn; yylloc->last_column = yycolumn+yyleng-1; \
    yycolumn += yyleng;

%}

%option bison-bridge
%option bison-locations
%option yylineno
%option noyywrap
%option noinput
%option nounput

%x LITERAL
%x COMMENT
%%

[ \t]+ {};
\n|\r\n {yycolumn = 0;}
-?[0-9]+ {yylval->num = strtol(yytext, NULL, 10);; return integer; }
const {return CONST;}
name {return NAME;}
temp {return TEMP;}
binop {return BINOP;}
mem {return MEM;}
call {return CALL;}
seq {return SEQ;}
eseq {return ESEQ;}
move {return MOVE;}
sxp {return SXP;}
jump {return JUMP;}
cjump {return CJUMP;}
label {return LABEL;}
call[ \t]+end { return CALLEND;}
seq[ \t]+end { return SEQEND;}

"(+)"|"add" {yylval->num = ADD; return op;}
"(-)"|"sub" {yylval->num = SUB; return op;}
"(*)"|"mul" {yylval->num = MUL; return op;}
"(/)"|"div" {yylval->num = DIV; return op;}
"(%)"|"mod" {yylval->num = MOD; return op;}
"(<=)"|"le" {yylval->num = LE; return rop;}
"(>=)"|"ge" {yylval->num = GE; return rop;}
"(<>)"|"ne" {yylval->num = NE; return rop;}
"(<)"|"lt"  {yylval->num = LT; return rop;}
"(>)"|"gt"  {yylval->num = GT; return rop;}
"(=)"|"eq"  {yylval->num = EQ; return rop;}
[$A-Za-z0-9_]+ {yylval->str = name2id(yytext); return string;}


\" {strpos = 0;  BEGIN(LITERAL); }

<LITERAL>{
    \n|\r\n  {}

    /* \" and \\ */
    "\\\"" 	         {string_append((char)0x22);}
    "\\\\"	 	     {string_append((char)0x5c);}

    /* C escape characters */
    \\[aA]           {string_append((char)0x07);}
    \\[bB]           {string_append((char)0x08);}
    \\[tT]           {string_append((char)0x09);}
    \\[nN]           {string_append((char)0x0a);}
    \\[vV]           {string_append((char)0x0b);}
    \\[fF]           {string_append((char)0x0c);}
    \\[rR]           {string_append((char)0x0d);}

    /* \OOO and \xXX ascii escapes */
    \\[0-3][0-7][0-7]  {
        /* Convert the octal characters to an integer */
        char c = (char) strtol(yytext+1, NULL, 8);
        string_append(c);
    }
    \\x([0-9a-fA-F]{2}) {
        /* Convert the hex characters to an integer */
        char c = (char) strtol(yytext+1, NULL, 16);
        string_append(c);
    }

    /* All other characters except " and \ are accepted */
    [^\\\"] {string_append(yytext[0]);}

    /* end of string */
    "\"" {
        BEGIN(INITIAL);
        string_append(0);
        yylval->str = strdup(strbuf);
        return literal;
    }

    <<EOF>> {
        warnl((struct location *)yylloc, "unterminated string");
        return 0;
    }
}


"/*"          {BEGIN(COMMENT);}
<COMMENT>"\n" {yycolumn = 1;}
<COMMENT>"*/" {BEGIN(INITIAL);}
<COMMENT>.    {}

"#"[^\n]*     {}

. { errl((struct location *)yylloc, 1, "unexpected token <%s>", yytext); };

%%
