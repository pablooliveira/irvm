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
