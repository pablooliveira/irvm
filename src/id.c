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

#include <string.h>
#include <stdio.h>
#include <ccan/htable/htable.h>
#include <ccan/hash/hash.h>
#include <ccan/compiler/compiler.h>

#include "id.h"
#include "destroy.h"

/* a set of reserved ids */
id end;
id rv;
id fp;
id sp;
id argid[MAX_ARGS];
id mainid;

/* the id htable */
struct htable ht_id;

/* rehash function for strings */
static size_t
rehash (const void *e, void *unused UNUSED)
{
  return hash_string ((char *) e);
}

/* string equality */
static bool
strequ (const void *e, void *string)
{
  return strcmp ((char *) e, string) == 0;
}

/* init id module */
void
init_id (void)
{
  htable_init (&ht_id, rehash, NULL);
  end = name2id ("end");
  rv = name2id ("rv");
  sp = name2id ("sp");
  fp = name2id ("fp");
  mainid = name2id ("main");
  int i;
  for (i = 0; i < MAX_ARGS; i++)
    {
      char temp[MAX_ARGS];
      snprintf (temp, MAX_CHAR_ARGS, "i%d", i);
      argid[i] = name2id (temp);
    }
}

/* free id module */
void
free_id (void)
{
  free_htable (&ht_id);
}

/* return the unique id for string STR */
id
name2id (char *name)
{
  /* did we already allocate an id for STR? */
  id n = htable_get (&ht_id, hash_string (name), strequ, name);

  /* No: create a new unique copy of the string,
     and add it to the unique id hash table. */
  if (!n)
    {
      n = strdup (name);
      htable_add (&ht_id, hash_string (n), n);
    }

  /* Yes: return the previous unique id copy */
  return n;
}
