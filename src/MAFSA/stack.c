
/*
 * Copyright (C) 2007-2009, libturglem development team.
 * This file is released under the LGPL.
 *
 */

#include <stdlib.h>
#include <string.h>
#include <MAFSA/stack.h>

int MAFSA_stack_setup(MAFSA_stack_t *ds, int objs, int objp)
{
    ds->objs = objs;
    ds->last = objs * objp;
    ds->curr = 0;

    ds->data = (uint8_t *) malloc(ds->last);
    if (NULL == ds->data) return -1;

    return 0;
}

int MAFSA_stack_erase(MAFSA_stack_t *ds)
{
    free(ds->data);

    return 0;
}

int MAFSA_stack_push(MAFSA_stack_t *ds, void **out)
{
    if (ds->curr >= ds->last)
    {
        uint8_t *nd = (uint8_t *) realloc(ds->data, ds->last * 2);
        if (NULL == ds->data) return -1;

        ds->data = nd;
        ds->last = ds->last * 2;
    }

    memcpy(&ds->data[ds->curr], out, ds->objs);
    ds->curr += ds->objs;

    return 0;
}

int MAFSA_stack_popp(MAFSA_stack_t *ds, void **out)
{
    if (0 == ds->curr)
    {
        return -1;
    }

    ds->curr -= ds->objs;
    memcpy(out, &ds->data[ds->curr], ds->objs);

    return 0;
}

