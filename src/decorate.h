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

#include "irvm.h"

/* DECORATE module connects all tree nodes in a linked list following
   the correct execution order of the program. It also binds each jump
   and Name to its associated label. */

/* connect all the nodes in a linked list (throught the ->next pointers)
   in the order in which they should be evaluated.*/
void compute_exec_order (struct Node *n);

/* find the label named NAME */
struct Node * find_label (id name);

/* bind each Name to its label */
void bind_labels (struct Node *n);
