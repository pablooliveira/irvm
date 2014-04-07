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

#include <stdint.h>
#include "irvm.h"

/* The MEM module handles VM memory allocation and access.
   It also handles VM strings */

void init_mem (void);
void free_mem (void);

/* return the adress of a new allocated block of size SIZE */
int32_t ir_alloc (int32_t size);

/* read one 32 bit integer at ADDR */
int32_t ir_read (int32_t addr);

/* write one 32 bit integer at ADDR */
void ir_write (int32_t addr, int32_t value);

/* create a new VM string and return its address */
int32_t new_string (const char *str);

/* reads the VM string at ADDR */
char * read_string (int32_t addr);

/* returns a one character VM string with ASCII code c */
int32_t get_ascii (char c);

/* returns the empty VM string */
int32_t get_empty (void);
