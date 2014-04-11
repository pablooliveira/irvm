IRVM 0.0.2
==========

IRVM is a virtual machine designed to execute simple register based high level
intermediate code.  It interprets the intermediate representations defined by
Andrew Appel in his "Modern Compiler Implementation".  For more information
about the Andrew Appel's Modern Compiler Implementation book series please
visit [Andrew Appel's home
page](https://www.cs.princeton.edu/~appel/modern/c/).

IRVM features are:

 * accepts HIR tree-like source language (two way conditional jumps,
      arbitrarily nested subroutines calls)

 * accepts LIR threaded source language (one way conditional jumps, 
      no nested subroutines calls)

 * a runtime library comparable to SPIM's

IRVM is inspired by the HAVM Tree virtual machine developped by Robert Anisko
at EPITA. For more information about HAVM, please visit its [project
page](http://www.lrde.epita.fr/wiki/Havm).

HAVM is a robust Tree Virtual Machine, but it does not support Tree programs
that change the control flow by jumping outside of an expression evaluation
(see [HAVM documentation section 3 Known
Problems](https://www.lrde.epita.fr/~akim/ccmp/doc/havm.html#Known-Problems)).
For example, HAVM is unable to interpret the Tree IR generated from the
following Tiger source program.

    print_int (if 0 | 0 then 0 else 1)

In contrast, IRVM fully supports these programs. In IRVM, `Jump` and
`CJump` breaking out of expressions nodes are properly handled.

##Installation

For full instructions please read the INSTALL procedure.
In most of the cases, typing the following commands should suffice,

    ./autogen.sh
    ./configure
    make && make install

For those commands to work, you should ensure that the GNU autotools are
installed on your system.

##Usage

IRVM is invoked with the following parameters

    irvm OPTIONS FILE

where FILE is the input IR Tree file and OPTIONS may be chosen among the following:

 * `-h / --help` : Displays a help message and exits

 * `-V / --version`: Displays the version number and exits

 * `-t / --trace`: Produces detailed execution traces on stderr. This option is very useful when debugging a Tree program.

 * `-l / --low`: Checks that the input is a valid LIR program before execution.

For more information please refer to the documentation in `doc/`.

##License

IRVM is released under the GNU General Public License Version 3 or later.  The
Licence terms are available in the COPYING file.
