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
