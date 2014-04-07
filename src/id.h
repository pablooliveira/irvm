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

#ifndef IRVM_ID_H
#define IRVM_ID_H

#include "consts.h"

/* ID module is in charge of ID handling :
   all identifier strings are replaced by unique ids for faster id comparison
   and id hashing */

/* free the id module */
void free_id (void);

/* init the id module */
void init_id (void);

/* the id type */
typedef char *id;

/* return the unique id for string STR */
id name2id (char *str);

/* a set of reserved ids */
extern id end;
extern id rv;
extern id fp;
extern id sp;
extern id argid[MAX_ARGS];
extern id mainid;

#endif /* IRVM_ID_H */
