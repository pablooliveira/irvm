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

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include "error.h"
#include "irvm.h"

/* report an error at location LOC and exits with status EXIT_CODE */
void
errl (struct location *loc, int exit_code, const char *format, ...)
{
  va_list arg;
  fprintf (stderr, "%d.%d-%d.%d: error: ",
	   loc->first_line,
	   loc->first_column, loc->last_line, loc->last_column);

  va_start (arg, format);
  vfprintf (stderr, format, arg);
  va_end (arg);
  fprintf (stderr, "\n");
  exit (exit_code);
}

/* report a warning at location LOC */
void
warnl (struct location *loc, const char *format, ...)
{
  va_list arg;
  fprintf (stderr, "%d.%d-%d.%d: ",
	   loc->first_line,
	   loc->first_column, loc->last_line, loc->last_column);

  va_start (arg, format);
  vfprintf (stderr, format, arg);
  va_end (arg);
  fprintf (stderr, "\n");
}
