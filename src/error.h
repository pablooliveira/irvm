#include "irvm.h"

/* ERROR module is in charge of error reporting */

/* report an error at location LOC and exits with status EXIT_CODE */
void errl (struct location *loc, int exit_code, const char *format, ...);

/* report a warning at location LOC */
void warnl (struct location *loc, const char *format, ...);
