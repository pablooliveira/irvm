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

/* Maximum size of the stack */
#define MAX_STACK 16384

/* Maximum memory size */
#define MEM_SIZE (1 << 22)

/* Maximum arguments in a call */
#define MAX_ARGS 4096
#define MAX_CHAR_ARGS 6	/* define this as ceil(log10(MAX_ARGS) + 2) */
