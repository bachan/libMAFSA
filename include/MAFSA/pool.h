
/*
 * Copyright (C) 2007-2009, libturglem development team.
 * This file is released under the LGPL.
 *
 */

#ifndef __MAFSA_POOL_H__
#define __MAFSA_POOL_H__

#include <MAFSA/stack.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct MAFSA_dpage MAFSA_dpage_t;
typedef struct MAFSA_dpool MAFSA_dpool_t;

struct MAFSA_dpage
{
    uint8_t *data;        /* data */
    uint8_t *free;        /* free node */

    int objs;             /* object size */
    int objp;             /* object per page count */

    MAFSA_dpage_t *next;  /* next */
};

int MAFSA_dpage_setup(MAFSA_dpage_t *pp, int objs, int objp);  /* create page [pp] of [objp] objects with [objs] size */
int MAFSA_dpage_erase(MAFSA_dpage_t *pp);                      /* remove page [pp] */
int MAFSA_dpage_alloc(MAFSA_dpage_t *pp, void **out);          /* assign last unused address to *[out] if not full */

#define MAFSA_dpage_attch(pp, ap) ((pp)->next  = (ap))                                  /* attach [ap] next to [pp] */
#define MAFSA_dpage_filld(pp)     ((pp)->free >= (pp)->data + (pp)->objs * (pp)->objp)  /* is [pp] filled completely */

struct MAFSA_dpool
{
    MAFSA_stack_t  fnds;  /* freed nodes */
    MAFSA_dpage_t *root;  /* current page */

    uint16_t cpag;        /* count pages */
    uint32_t cobj;        /* count objects */
};

int MAFSA_dpool_setup(MAFSA_dpool_t *dp, int objs, int objp);
int MAFSA_dpool_erase(MAFSA_dpool_t *dp);
int MAFSA_dpool_alloc(MAFSA_dpool_t *dp, void **out);
int MAFSA_dpool_mfree(MAFSA_dpool_t *dp, void **out);

#ifdef __cplusplus
}
#endif

#endif  /* __MAFSA_POOL_H__ */
