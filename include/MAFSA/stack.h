
/*
 * Copyright (C) 2007-2009, libturglem development team.
 * This file is released under the LGPL.
 *
 */

#ifndef __MAFSA_STACK_H__
#define __MAFSA_STACK_H__

#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct MAFSA_stack MAFSA_stack_t;

struct MAFSA_stack
{
    uint8_t *data;  /* stack data  */

    int objs;       /* object size */
    int last;       /* reserved    */
    int curr;       /* current     */
};

int MAFSA_stack_setup(MAFSA_stack_t *ds, int objs, int objp);  /* create stack [ds] of [objp] elements [objs] size each */
int MAFSA_stack_erase(MAFSA_stack_t *ds);                      /* remove stack [ds] */

int MAFSA_stack_push(MAFSA_stack_t *ds, void **out);           /* push element pointed by [data] to stack [ds] */
int MAFSA_stack_popp(MAFSA_stack_t *ds, void **out);           /* popp element from stack [ds] and copy it to [data] */

#define MAFSA_stack_size(ds) ((ds)->curr)                      /* returns size of [ds] in BYTES (!!), not in elements */

#ifdef __cplusplus
}
#endif

#endif  /* __MAFSA_STACK_H__ */
