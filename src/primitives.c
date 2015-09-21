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

#include <stdio.h>
#include <stdlib.h>
#include <ccan/list/list.h>

#include "context.h"
#include "error.h"
#include "mem.h"
#include "primitives.h"

/* A list of all the known primitives,
   it is filled once init_primitives is called */
struct list_head primitives;

/* A primitive entry, binds each PRIMITIVE to one ID */
struct primitive_entry
{
  id name;
  primitive prim;
  struct list_node list;
};

/* print (string) : print a string to stdout */
static int32_t
tig_print (void)
{
  int32_t i = get_temp (argid[0]);
  char *str = read_string (i);

  printf ("%s", str);
  return 0;
}

/* print_err (string) : print a string to stderr */
static int32_t
tig_print_err (void)
{
  int32_t i = get_temp (argid[0]);
  char *str = read_string (i);
  fprintf (stderr, "%s", str);
  return 0;
}

/* print_int (int) : print an integer to stderr */
static int32_t
tig_print_int (void)
{
  int32_t i = get_temp (argid[0]);
  printf ("%d", i);
  return 0;
}

/* exit (int) : exit the program with exit code status. */
static int32_t
tig_exit (void)
{
  char status = get_temp (argid[0]);
  exit (status);
}

/* strcmp (string, string) : compare strings a and b.
   Return -1 if a < b, 0 if equal, and 1 otherwise. */
static int32_t
tig_strcmp (void)
{
  int32_t s1 = get_temp (argid[0]);
  int32_t s2 = get_temp (argid[1]);
  return strcmp (read_string (s1), read_string (s2));
}

/* streq (string, string) : return 1 if strings a and b
   are equal, 0 otherwise. */
static int32_t
tig_streq (void)
{
  return tig_strcmp () == 0;
}

/* size (string) : return the size in characters of the string */
static int32_t
tig_size (void)
{
  int32_t s1 = get_temp (argid[0]);
  return strlen (read_string (s1));
}

/* concat (string, string) : concats strings a and b and returns a newly
   allocated string */
static int32_t
tig_concat (void)
{
  char *s1 = read_string (get_temp (argid[0]));
  char *s2 = read_string (get_temp (argid[1]));
  int32_t size = strlen (s1) + strlen (s2) + 1;
  char c[size];
  strncpy (c, s1, size);
  strncat (c, s2, size);
  return new_string (c);
}

/* substring (string: string, start: integer, length: integer)
   return the substring string[start:start+length] */
static int32_t
tig_substring (void)
{
  int32_t addr = get_temp (argid[0]);
  int32_t start = get_temp (argid[1]);
  int32_t length = get_temp (argid[2]);

  char *s = read_string (addr + start);
  size_t len = strlen (s);
  if (length > len || length < 0) {
    fprintf(stderr, "substring: arguments out of bounds\n");
    exit(1);
  }
  char new[length + 1];
  strncpy (new, s, length);
  new[length] = 0;
  return new_string (new);
}

/* getchar () : reads ands returns as a string one character from stdin.
   Returns "" on EOF. */
static int32_t
tig_getchar (void)
{
  char c = getchar ();
  if (c == EOF)
    return get_empty ();
  else
    return get_ascii (c);
}


/* malloc (int) : return the address of a freshly allocated
   block of memory of the given size. */
static int32_t
tig_malloc (void)
{
  int32_t size = get_temp (argid[0]);
  return ir_alloc (size);
}

/* not (int) : boolean not */
static int32_t
tig_not (void)
{
  bool boolean = get_temp (argid[0]);
  return !boolean;
}

/* init_array (size : integer, init : integer) :
   allocate an array of size SIZE and fill it with value INIT */
static int32_t
tig_init_array (void)
{
  int32_t size = get_temp (argid[0]);
  int32_t init = get_temp (argid[1]);
  int32_t addr = ir_alloc (size * 4);
  int i;
  for (i = 0; i < size; i++)
    ir_write (addr + i * 4, init);
  return addr;
}

/* chr (integer) : return the one character string matching the
  passed ASCII code */
static int32_t
tig_chr (void)
{
  int32_t c = get_temp (argid[0]);
  if (c < 0 || c > 255) {
    fprintf(stderr, "chr: character out of range\n");
    exit(1);
  }
  return get_ascii ((char)c);
}

/* ord (string) : returns the ASCII code of the first string character */
static int32_t
tig_ord (void)
{
  char *str = read_string (get_temp (argid[0]));
  if (str[0] == '\0')
    return -1;
  else
    return str[0];
}

/* flush () : flushes stdout and stderr */
static int32_t
tig_flush (void)
{
  fflush (stdout);
  fflush (stderr);
  return 0;
}

/* register one primitive in the PRIMITIVES table */
static void
register_primitive (char *name, primitive prim)
{
  struct primitive_entry *p = malloc (sizeof *p);
  p->name = name2id (name);
  p->prim = prim;
  list_add (&primitives, &p->list);
}

/* If NAME is a primitive, return it,
   otherwise return NULL */
primitive
is_primitive (id name)
{
  struct primitive_entry *p = NULL;

  list_for_each (&primitives, p, list)
  {
    if (p->name == name)
      return p->prim;
  }
  return NULL;
}

/* init primitives table */
void
init_primitives (void)
{

  list_head_init (&primitives);
  register_primitive ("chr", tig_chr);
  register_primitive ("concat", tig_concat);
  register_primitive ("exit", tig_exit);
  register_primitive ("flush", tig_flush);
  register_primitive ("getchar", tig_getchar);
  register_primitive ("init_array", tig_init_array);
  register_primitive ("malloc", tig_malloc);
  register_primitive ("not", tig_not);
  register_primitive ("_not", tig_not);
  register_primitive ("ord", tig_ord);
  register_primitive ("print_int", tig_print_int);
  register_primitive ("printint", tig_print_int);
  register_primitive ("print", tig_print);
  register_primitive ("print_err", tig_print_err);
  register_primitive ("size", tig_size);
  register_primitive ("streq", tig_streq);
  register_primitive ("strcmp", tig_strcmp);
  register_primitive ("substring", tig_substring);
}

/* destroy the primitive table */
void
free_primitives (void)
{
  struct primitive_entry *p = NULL;
  while ((p = list_pop (&primitives, struct primitive_entry, list)))
    {
      free (p);
    }
}
