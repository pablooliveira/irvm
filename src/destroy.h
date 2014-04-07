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

#include <ccan/htable/htable.h>
#include "irvm.h"

/* DESTROY module is in charge of freeing the memory */

/* free all memory at the end of the program */
void destroy (void);

/* free an htable */
void free_htable (struct htable *ht);

/* free a IR Node tree */
void free_irtree (struct Node *n);
