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

#include <err.h>
#include <stdlib.h>

#include "consts.h"
#include "context.h"
#include "error.h"
#include "mem.h"

/* The VM memory */
char *mem;

/* First free cell in the VM memory */
size_t first_free;

/* Address of an ascii_table to quickly return one character strings */
int32_t ascii_table;

/* init memory module */
void
init_mem (void)
{
  mem = malloc (MEM_SIZE);

  /* reserve memory for the stack */
  first_free = MAX_STACK;
  set_temp (sp, first_free - 1);
  set_temp (fp, first_free - 1);
  set_temp (rv, 0);

  /* create an ascii table */
  ascii_table = first_free;
  int i;
  for (i = 0; i < 512; i++)
    {
      mem[first_free++] = i;
      mem[first_free++] = 0;
    }
}

/* free memory module */
void
free_mem (void)
{
  free(mem);
}


/* return the adress of a new allocated block of size SIZE.
   the allocator is trivial, since memory is never reclaimed
   in this VM */
int32_t
ir_alloc (int32_t size)
{
  if (first_free + size >= MEM_SIZE)
    errx (1, "out of memory");

  int32_t pos = first_free;
  first_free += size;
  return pos;
}

/* read one 32 bit integer at ADDR */
int32_t
ir_read (int32_t addr)
{
  if (addr < 0 || addr >= MEM_SIZE)
    errl (current_loc, 1, "invalid read at addr %d\n", addr);

  return ((int32_t *) (&mem[addr]))[0];
}

/* write one 32 bit integer at ADDR */
void
ir_write (int32_t addr, int32_t value)
{
  if (addr < 0 || addr >= MEM_SIZE)
    errl (current_loc, 1, "invalid write at addr %d\n", addr);

  ((int32_t *) (&mem[addr]))[0] = value;
}

/* create a new VM string and return its address */
int32_t
new_string (const char *str)
{
  size_t len = strlen (str);
  int32_t addr = ir_alloc (len + 1);
  strcpy (&mem[addr], str);
  return addr;
}

/* reads the VM string at ADDR */
char *
read_string (int32_t addr)
{
  if (addr < 0 || addr >= MEM_SIZE)
    errl (current_loc, 1, "invalid read at addr %d\n", addr);

  char *str = &mem[addr];
  char *c;

  /* Check that the string is null-terminated before
     returning it. */
  for (c = str; c < &mem[MEM_SIZE - 1]; c++)
    if (*c == '\0') return str;

  errl (current_loc, 1, "string at %d is corrupted\n", addr);
  assert (0);
}

/* returns a one character VM string with ASCII code c */
int32_t
get_ascii (char c)
{
  return ascii_table + c * 2;
}

/* returns the empty VM string */
int32_t
get_empty (void)
{
  return ascii_table + 1;
}

