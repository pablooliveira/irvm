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

#include <assert.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>

#include <ccan/opt/opt.h>

#include "config.h"

#include "context.h"
#include "decorate.h"
#include "error.h"
#include "execute.h"
#include "irvm.h"
#include "mem.h"
#include "primitives.h"
#include "destroy.h"

/* the input file name */
char *input_fn;

/* the root of the IR tree */
struct Node *irtree;

/* parse function and parse input file */
int yyparse (void);
extern FILE *yyin;

/* is trace mode on ? */
bool trace;

/* initialize the program command-line option table */
static struct opt_table opts[] = {
    OPT_WITHOUT_ARG ("--trace|-t", opt_set_bool, &trace,
                     "trace execution"),
    OPT_WITHOUT_ARG ("--help|-h", opt_usage_and_exit,
                     "[OPTIONS] INPUT-FILE", "print this help message"),
    OPT_WITHOUT_ARG ("--version|-V", opt_version_and_exit,
                     "irvm " VERSION "\n"
                     "Copyright (C) 2014 Pablo de Oliveira\n"
                     "This is free software; see the source for copying conditions. There is NO\n"
                     "warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n",
                     "print version and exit"),
    OPT_ENDTABLE
};

/* IRVM entry function */
void
irvm (void)
{
  /* init IRVM modules */
  init_id ();
  init_context ();
  init_primitives ();
  init_mem ();

  /* register the destroy handler */
  atexit (destroy);

  /* parse the input */
  int status = yyparse ();
  fclose (yyin);
  if (status != 0)
    errx (1, "Parsing failed");

  /* decorate the tree */
  compute_exec_order (irtree);
  bind_labels (irtree);

  /* find MAIN IR label */
  struct Node *main_label = find_label (mainid);

  if (main_label == NULL)
    errx (1, "label main not found");

  set_temp (argid[0], 0);

  /* execute the IR program */
  execute (main_label, trace);
}

/* Main: parse arguments and call IRVM entry function */
int
main (int argc, char ** argv)
{
  opt_register_table (opts, NULL);
  if (!opt_parse (&argc, argv, opt_log_stderr))
    exit (1);

  if (argc != 2)
    opt_usage_and_exit ("[OPTIONS] INPUT-FILE");
  input_fn = argv[1];
  opt_free_table ();

  if (!(yyin = fopen (input_fn, "r")))
    err (1, "could not open %s", input_fn);

  irvm ();

  return 0;
}
