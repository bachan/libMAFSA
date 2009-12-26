
/*
 * Copyright (C) 2007-2009, libturglem development team.
 * This file is released under the LGPL.
 *
 */

#ifndef __MAFSA_DACIUK_H__
#define __MAFSA_DACIUK_H__

#include <Judy.h>
#include <MAFSA/pool.h>
#include <MAFSA/internal.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct MAFSA_dnode MAFSA_dnode_t;
typedef struct MAFSA_dpath MAFSA_dpath_t;

struct MAFSA_dnode
{
    int ml;              /* max_letter   */
    int pc;              /* parent_count */
    int fn;              /* is_final     */

    MAFSA_dnode_t **ch;  /* children     */
};

int MAFSA_dnode_setup(MAFSA_dnode_t *dn, int ml);
int MAFSA_dnode_clone(MAFSA_dnode_t *dn, const MAFSA_dnode_t *sn);
int MAFSA_dnode_erase(MAFSA_dnode_t *dn);

int MAFSA_dnode_less(const MAFSA_dnode_t *dnl, const MAFSA_dnode_t *dnr);

#define MAFSA_dnode_add_ref(dn, cnt) ((dn)->pc += (cnt))
#define MAFSA_dnode_get_chd(dn, pos) ((dn)->ch[(pos)])

struct MAFSA_dpath
{
    MAFSA_letter lb;
    MAFSA_dnode_t *nd;
};

int MAFSA_dpath_setup(MAFSA_dpath_t *pa, MAFSA_letter lb, MAFSA_dnode_t *nd);
int MAFSA_dpath_erase(MAFSA_dpath_t *pa);

/* daciuk */

typedef struct MAFSA_daciuk MAFSA_daciuk_t;

struct MAFSA_daciuk
{
    int ml;  /* max_letter */

    MAFSA_dnode_t *root;  /* tree root */
    MAFSA_dpool_t *pool;  /* nodes pool */
    MAFSA_dpath_t *pref;  /* prefix */

    Pvoid_t reg;
};

int MAFSA_daciuk_setup(MAFSA_daciuk_t *da, int ml);
int MAFSA_daciuk_erase(MAFSA_daciuk_t *da);

int MAFSA_daciuk_save_memo(MAFSA_daciuk_t *da, void **memo, size_t *size);
int MAFSA_daciuk_load_memo(MAFSA_daciuk_t *da, void  *memo, size_t  size);

int MAFSA_daciuk_save_file(MAFSA_daciuk_t *da, const char *fn);
int MAFSA_daciuk_load_file(MAFSA_daciuk_t *da, const char *fn);

int MAFSA_daciuk_replace_state(MAFSA_daciuk_t *da, MAFSA_dnode_t *fr, MAFSA_dnode_t *v, MAFSA_letter label, int is_final, int killed);
int MAFSA_daciuk_delete_branch(MAFSA_daciuk_t *da, MAFSA_dnode_t **base);
int MAFSA_daciuk_first_state(MAFSA_daciuk_t *da, MAFSA_dpath_t *pa, unsigned int pasz);
int MAFSA_daciuk_is_already_there(MAFSA_daciuk_t *da, MAFSA_dpath_t *pa, unsigned int pasz);
int MAFSA_daciuk_common_prefix(MAFSA_daciuk_t *da, const MAFSA_letter *word, size_t sz, MAFSA_dpath_t *pa, size_t *pasz);

MAFSA_dnode_t *MAFSA_daciuk_clone(MAFSA_daciuk_t *da, MAFSA_dnode_t *current);
MAFSA_dnode_t *MAFSA_daciuk_replace_or_register(MAFSA_daciuk_t *da, MAFSA_dnode_t *current);
MAFSA_dnode_t *MAFSA_daciuk_add_suffix(MAFSA_daciuk_t *da, MAFSA_dnode_t *base, const MAFSA_letter *s, size_t sz, size_t pos);
int MAFSA_daciuk_unregister(MAFSA_daciuk_t *da, MAFSA_dnode_t *current);
int MAFSA_daciuk_insert(MAFSA_daciuk_t *da, const MAFSA_letter *current_string, size_t c_str_sz);

#ifdef __cplusplus
}
#endif

#endif  /* __MAFSA_DACIUK_H__ */
