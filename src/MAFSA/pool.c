
/*
 * Copyright (C) 2007-2009, libturglem development team.
 * This file is released under the LGPL.
 *
 */

#include <stdlib.h>
#include <MAFSA/pool.h>

int MAFSA_dpage_setup(MAFSA_dpage_t *pp, int objs, int objp)
{
    pp->objs = objs;
    pp->objp = objp;

    pp->data = (uint8_t *) malloc(pp->objs * pp->objp);
    if (NULL == pp->data) return -1;

    pp->free = pp->data;
    pp->next = NULL;

    return 0;
}

int MAFSA_dpage_erase(MAFSA_dpage_t *pp)
{
    free(pp->data);

    if (NULL != pp->next)
    {
        return MAFSA_dpage_erase(pp->next);
    }

    return 0;
}

int MAFSA_dpage_alloc(MAFSA_dpage_t *pp, void **out)
{
    if (MAFSA_dpage_filld(pp))
    {
        return -1;
    }

    *out = pp->free;
    pp->free += pp->objs;

    return 0;
}

/* XXX : sizeof(void *) means that we setup stack of pointers to data */
/* XXX : 4096 means that stack will allocate 4096 objects */

int MAFSA_dpool_setup(MAFSA_dpool_t *dp, int objs, int objp)
{
    dp->cpag = 1;
    dp->cobj = 0;

    MAFSA_stack_setup(&dp->fnds, sizeof(void *), 4096);

    dp->root = (MAFSA_dpage_t *) malloc(sizeof(MAFSA_dpage_t));
    if (NULL == dp->root) return -1;

    MAFSA_dpage_setup(dp->root, objs, objp);

    return 0;
}

int MAFSA_dpool_erase(MAFSA_dpool_t *dp)
{
    MAFSA_dpage_erase(dp->root);
    free(dp->root);

    MAFSA_stack_erase(&dp->fnds);

    return 0;
}

int MAFSA_dpool_alloc(MAFSA_dpool_t *dp, void **out)
{
    if (0 != MAFSA_stack_size(&dp->fnds))
    {
        MAFSA_stack_popp(&dp->fnds, out);
    }
    else
    {
        if (0 != MAFSA_dpage_alloc(dp->root, out))
        {
            MAFSA_dpage_t *np = dp->root;

            dp->root = (MAFSA_dpage_t *) malloc(sizeof(MAFSA_dpage_t));
            if (NULL == dp->root) return -1;

            MAFSA_dpage_setup(dp->root, np->objs, np->objp);
            MAFSA_dpage_attch(dp->root, np);

            dp->cpag++;

            MAFSA_dpage_alloc(dp->root, out);
        }
    }

    dp->cobj++;

    return 0;
}

int MAFSA_dpool_mfree(MAFSA_dpool_t *dp, void **out)
{
    MAFSA_stack_push(&dp->fnds, out);
    dp->cobj--;

    return 0;
}

