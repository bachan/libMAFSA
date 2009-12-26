
/*
 * Copyright (C) 2007-2009, libturglem development team.
 * This file is released under the LGPL.
 *
 */

#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <MAFSA/daciuk.h>

int MAFSA_dnode_setup(MAFSA_dnode_t *dn, int ml)
{
    dn->ml = ml;
    dn->pc = 0;
    dn->fn = 0;

    dn->ch = (MAFSA_dnode_t **) calloc(dn->ml + 1, sizeof(MAFSA_dnode_t *));
    if (NULL == dn->ch) return -1;

    return 0;
}

int MAFSA_dnode_clone(MAFSA_dnode_t *dn, const MAFSA_dnode_t *sn)
{
    dn->ml = sn->ml;
    dn->pc = 0;
    dn->fn = 0;

    dn->ch = (MAFSA_dnode_t **) malloc((dn->ml + 1) * sizeof(MAFSA_dnode_t *));
    if (NULL == dn->ch) return -1;

    memcpy(dn->ch, sn->ch, (dn->ml + 1) * sizeof(MAFSA_dnode_t *));

    return 0;
}

int MAFSA_dnode_erase(MAFSA_dnode_t *dn)
{
    free(dn->ch);

    return 0;
}

int MAFSA_dnode_less(const MAFSA_dnode_t *dnl, const MAFSA_dnode_t *dnr)
{
    int i;

    if (dnl->fn != dnr->fn)
    {
        return (0 == dnl->fn);
    }

    for (i = 0; i < dnl->ml + 1; ++i)
    {
        if (dnl->ch[i] != dnr->ch[i])
        {
            return MAFSA_dnode_less(dnl->ch[i], dnr->ch[i]);
        }
    }

    return 0;
}

int MAFSA_dpath_setup(MAFSA_dpath_t *pa, MAFSA_letter lb, MAFSA_dnode_t *nd)
{
    pa->lb = lb;
    pa->nd = nd;

    return 0;
}

int MAFSA_dpath_erase(MAFSA_dpath_t *pa)
{
    return 0;
}

/* daciuk */

int MAFSA_daciuk_setup(MAFSA_daciuk_t *da, int ml)
{
    void *vroot;

    da->ml = ml;
    da->reg = NULL;

    da->pref = (MAFSA_dpath_t *) malloc(1024 * sizeof(MAFSA_dpath_t));
    if (NULL == da->pref) return -1;

    da->pool = (MAFSA_dpool_t *) malloc(sizeof(MAFSA_dpool_t));
    if (NULL == da->pool) return -1;

    MAFSA_dpool_setup(da->pool, sizeof(MAFSA_dnode_t), 1024);
    MAFSA_dpool_alloc(da->pool, &vroot);
    da->root = vroot;

    MAFSA_dnode_setup(da->root, da->ml);

    return 0;
}

int MAFSA_daciuk_erase(MAFSA_daciuk_t *da)
{
    MAFSA_daciuk_delete_branch(da, &da->root);

    free(da->pool);
    free(da->pref);

    return 0;
}

int MAFSA_daciuk_replace_state(MAFSA_daciuk_t *da, MAFSA_dnode_t *fr, MAFSA_dnode_t *v, MAFSA_letter label, int is_final, int killed)
{
    MAFSA_dnode_t **child = &(MAFSA_dnode_get_chd(fr, label));

    if (*child && v != *child && 0 == killed)
    {
        MAFSA_daciuk_delete_branch(da, child);
    }

    *child = v;

    return 0;
}

int MAFSA_daciuk_delete_branch(MAFSA_daciuk_t *da, MAFSA_dnode_t **base)
{
    if (MAFSA_dnode_add_ref(*base, 0))
    {
        MAFSA_dnode_add_ref(*base, -1);
    }

    if (0 == MAFSA_dnode_add_ref(*base, 0))
    {
        int i;

        for (i = 0; i < da->ml + 1; ++i)
        {
            MAFSA_dnode_t **child = &(MAFSA_dnode_get_chd(*base, i));

            if (*child)
            {
                MAFSA_daciuk_delete_branch(da, child);
            }
        }

        MAFSA_dnode_erase(*base);
        MAFSA_dpool_mfree(da->pool, (void **) base);

        *base = 0;
    }

    return 0;
}

int MAFSA_daciuk_first_state(MAFSA_daciuk_t *da, MAFSA_dpath_t *pa, unsigned int pasz)
{
    unsigned int i;

    for (i = 0; i < pasz; ++i)
    {
        if (pa[i].nd->pc > 1)
        {
            return i;
        }
    }

    return -1;
}

int MAFSA_daciuk_is_already_there(MAFSA_daciuk_t *da, MAFSA_dpath_t *pa, unsigned int pasz)
{
    return pa[pasz - 1].nd->fn;
}

int MAFSA_daciuk_common_prefix(MAFSA_daciuk_t *da, const MAFSA_letter *word, size_t sz, MAFSA_dpath_t *pa, size_t *pasz)
{
    MAFSA_dnode_t *current = da->root;
    int first_conf = -1;

    unsigned int i;
    *pasz = 0;

    for (i = 0; i < sz; ++i)
    {
        MAFSA_letter position = word[i];
        MAFSA_dpath_setup(&pa[i], position, current);
        *pasz += 1;

        current = MAFSA_dnode_get_chd(current, position);
        if (0 == current) break;

        if (first_conf < 0 && current->pc > 1)
        {
            first_conf = i + 1;
        }
    }

    if (current)
    {
        MAFSA_dpath_setup(&pa[i++], -1, current);
        *pasz += 1;
    }

    return first_conf;
}

MAFSA_dnode_t *MAFSA_daciuk_clone(MAFSA_daciuk_t *da, MAFSA_dnode_t *current)
{
    int i;

    void *vcloned;
    MAFSA_dnode_t *cloned;

    MAFSA_dpool_alloc(da->pool, &vcloned);
    cloned = vcloned;

    MAFSA_dnode_clone(cloned, current);
    cloned->fn = current->fn;

    for (i = 0; i < da->ml + 1; ++i)
    {
        MAFSA_dnode_t *child = MAFSA_dnode_get_chd(cloned, i);

        if (child)
        {
            MAFSA_dnode_add_ref(child, 1);
        }
    }

    return cloned;
}

MAFSA_dnode_t *MAFSA_daciuk_replace_or_register(MAFSA_daciuk_t *da, MAFSA_dnode_t *current)
{
    int rc = Judy1Test(da->reg, (Word_t) current, NULL);

    if (1 == rc)
    {
        MAFSA_daciuk_delete_branch(da, &current);
    }
    else
    {
        Judy1Set(&da->reg, (Word_t) current, NULL); /* XXX : need to call node_less to compare to items here */
    }

    MAFSA_dnode_add_ref(current, 1);

    return current;
}

int MAFSA_daciuk_unregister(MAFSA_daciuk_t *da, MAFSA_dnode_t *current)
{
    Judy1Unset(&da->reg, (Word_t) current, NULL);

    return 0;
}

MAFSA_dnode_t *MAFSA_daciuk_add_suffix(MAFSA_daciuk_t *da, MAFSA_dnode_t *base, const MAFSA_letter *s, size_t sz, size_t pos)
{
    if (pos < sz)
    {
        MAFSA_dnode_t **next = &(MAFSA_dnode_get_chd(base, s[pos]));

        MAFSA_dpool_alloc(da->pool, (void **) next);
        MAFSA_dnode_setup(*next, da->ml);

        *next = MAFSA_daciuk_replace_or_register(da, MAFSA_daciuk_add_suffix(da, *next, s, sz, pos + 1));
    }
    else
    {
        base->fn = 1;
    }

    return base;
}

int MAFSA_daciuk_insert(MAFSA_daciuk_t *da, const MAFSA_letter *current_string, size_t c_str_sz)
{
    MAFSA_dpath_t prefix [1024];
    size_t prefix_size = 0;

    MAFSA_dnode_t *current_state;
    MAFSA_dnode_t *last_state;

    int initial_conf, first_conf;
    int last_pos, modified;

    if (NULL == current_string || 0 == c_str_sz)
    {
        return 0;
    }

    initial_conf = MAFSA_daciuk_common_prefix(da, current_string, c_str_sz, prefix, &prefix_size);

    if (prefix_size == c_str_sz + 1 && MAFSA_daciuk_is_already_there(da, prefix, prefix_size))
    {
        return 1;
    }

    last_pos = prefix_size - 1;

    if (initial_conf > 0)
    {
        last_state = MAFSA_daciuk_clone(da, prefix[last_pos].nd);

        if (initial_conf > 1)
        {
            MAFSA_daciuk_unregister(da, prefix[initial_conf - 1].nd);
        }

        last_state = MAFSA_daciuk_add_suffix(da, last_state, current_string, c_str_sz, prefix_size - 1);

        first_conf = MAFSA_daciuk_first_state(da, prefix, prefix_size);
        if (first_conf < 0) first_conf = initial_conf;

        for (last_pos--; last_pos >= first_conf; last_pos--)
        {
            last_state = MAFSA_daciuk_replace_or_register(da, last_state);

            current_state = MAFSA_daciuk_clone(da, prefix[last_pos].nd);
            MAFSA_daciuk_replace_state(da, current_state, last_state, prefix[last_pos].lb, 1, 0);

            last_state = current_state;
        }

        last_state = MAFSA_daciuk_replace_or_register(da, last_state);
        current_state = prefix[last_pos].nd;

        if (last_pos)
        {
            MAFSA_dnode_add_ref(current_state, - MAFSA_dnode_add_ref(current_state, 0));
        }

        MAFSA_daciuk_replace_state(da, current_state, last_state, prefix[last_pos].lb, 1, 0);
        last_state = current_state;
    }
    else
    {
        current_state = prefix[last_pos].nd;
        last_state = prefix[last_pos].nd;

        if (prefix_size < c_str_sz + 1)
        {
            if (last_pos)
            {
                int newpc;
                MAFSA_daciuk_unregister(da, current_state);
                newpc = MAFSA_dnode_add_ref(current_state, 0);
                MAFSA_dnode_add_ref(current_state, -newpc);
            }

            last_state = MAFSA_daciuk_add_suffix(da, current_state, current_string, c_str_sz, prefix_size - 1);
        }
        else
        {
            current_state = prefix[last_pos].nd;

            if (last_pos)
            {
                MAFSA_daciuk_unregister(da, current_state);
                MAFSA_dnode_add_ref(current_state, - MAFSA_dnode_add_ref(current_state, 0));
            }

            current_state->fn = 1;
        }
    }

    modified = 1;

    while (--last_pos > 0 && modified)
    {
        MAFSA_dnode_t *n;

        last_state = current_state;
        current_state = prefix[last_pos].nd;

        n = MAFSA_daciuk_replace_or_register(da, last_state);
        modified = (n != last_state);

        if (modified)
        {
            last_state = n;
            MAFSA_daciuk_unregister(da, current_state);
            MAFSA_dnode_add_ref(current_state, - MAFSA_dnode_add_ref(current_state, 0));
        }

        MAFSA_daciuk_replace_state(da, current_state, last_state, prefix[last_pos].lb, 1, modified);
    }

    if (modified && 0 == last_pos)
    {
        MAFSA_dnode_t *n;

        last_state = current_state;
        current_state = prefix[last_pos].nd;

        n = MAFSA_daciuk_replace_or_register(da, last_state);
        modified = (n != last_state);

        MAFSA_daciuk_replace_state(da, current_state, n, prefix[last_pos].lb, 1, modified);
    }

    return 1;
}

typedef uint32_t MAFSA_node_t;
typedef uint32_t MAFSA_link_t;

#define MAFSA_node_get_children_start(n)    ((n) & 0x7fffffff)
#define MAFSA_node_set_children_start(n, v) ((n) = (((n) & 0x80000000) | (v)))

#define MAFSA_node_get_final(n)    ((n) & 0x80000000)
#define MAFSA_node_set_final(n, v) ((n) = ((v) ? ((n) | 0x80000000) : ((n) & 0x7fffffff)))

#define MAFSA_link_get_link(l)     ((l) & 0x00ffffff)
#define MAFSA_link_set_link(l, v)  ((l) = ((0xff000000 & (l)) | (v)))

#define MAFSA_link_get_label(l)    ((l) >> 24)
#define MAFSA_link_set_label(l, v) ((l) = ((0x00ffffff & (l)) | ((v) << 24)))

#define MAFSA_link_get_terminating(l) (0xff == MAFSA_link_get_label(l))
#define MAFSA_link_set_terminating(l) { MAFSA_link_set_label(l, 0xff); MAFSA_link_set_link(l, 0); }

int MAFSA_daciuk_save_memo(MAFSA_daciuk_t *da, void **memo, size_t *size)
{
    size_t num_states;

    int links_number_index;
    int current_index;
    int links_begin;
    int child_id;

    uint32_t index_size;
    uint32_t k;

    uint32_t *index;
    uint32_t *kval;

    Word_t pindex;
    Pvoid_t node2index;

    MAFSA_node_t node = 0;
    MAFSA_link_t link = 0;

    num_states = Judy1Count(da->reg, 0, -1, NULL) + 1;
    node2index = NULL;

    index = malloc((num_states * (da->ml + 1) + num_states + 2) * sizeof(uint32_t));
    index_size = 0;

    index[index_size++] = num_states;

    links_number_index = index_size;
    index[index_size++] = 0;

    current_index = index_size;
    links_begin = index_size + num_states;

    while (index_size < links_begin)
    {
        index[index_size++] = 0;
    }

    k = 0;

    kval = (uint32_t *) JudyLIns(&node2index, (Word_t) da->root, NULL);
    *kval = k++;

    pindex = 0;

    for (;;)
    {
        int rc = Judy1Next(da->reg, &pindex, NULL);
        if (0 == rc) break;

        if (1 == rc)
        {
            kval = (uint32_t *) JudyLIns(&node2index, (Word_t) pindex, NULL);
            *kval = k++;
        }
    }

    MAFSA_node_set_children_start(node, index_size - links_begin);
    MAFSA_node_set_final(node, 0);
    index[current_index++] = node;

    for (child_id = 0; child_id < da->ml + 1; ++child_id)
    {
        MAFSA_dnode_t *child = MAFSA_dnode_get_chd(da->root, child_id);

        if (child)
        {
            kval = (uint32_t *) JudyLGet(node2index, (Word_t) child, NULL);

            MAFSA_link_set_label(link, child_id);
            MAFSA_link_set_link(link, *kval);
            index[index_size++] = link;
        }
    }

    MAFSA_link_set_terminating(link);
    index[index_size++] = link;

    pindex = 0;

    for (;;)
    {
        int rc = Judy1Next(da->reg, &pindex, NULL);
        if (0 == rc) break;

        if (1 == rc)
        {
            MAFSA_dnode_t *tnode = (MAFSA_dnode_t *) pindex;

            MAFSA_node_set_children_start(node, index_size - links_begin);
            MAFSA_node_set_final(node, tnode->fn);
            index[current_index++] = node;

            for (child_id = 0; child_id < da->ml + 1; ++child_id)
            {
                MAFSA_dnode_t *child = MAFSA_dnode_get_chd(tnode, child_id);

                if (child)
                {
                    kval = (uint32_t *) JudyLGet(node2index, (Word_t) child, NULL);

                    MAFSA_link_set_label(link, child_id);
                    MAFSA_link_set_link(link, *kval);
                    index[index_size++] = link;
                }
            }

            MAFSA_link_set_terminating(link);
            index[index_size++] = link;
        }
    }

    index[links_number_index] = index_size - links_begin;

    *size = index_size * sizeof(uint32_t);
    *memo = index;

    return 0;
}

int MAFSA_daciuk_save_file(MAFSA_daciuk_t *da, const char *fn)
{
    int     fd;
    void   *memo;
    size_t  size;

    if (0 != MAFSA_daciuk_save_memo(da, &memo, &size))
    {
        return -1;
    }

    if (-1 == (fd = open(fn, O_WRONLY|O_TRUNC|O_CREAT, 0644)))
    {
        free(memo); return -1;
    }

    if (-1 == write(fd, memo, size))
    {
        free(memo); close(fd); return -1;
    }

    if (0 != close(fd))
    {
        free(memo); return -1;
    }

    free(memo);

    return 0;
}

int MAFSA_daciuk_load_memo(MAFSA_daciuk_t *da, void *memo, size_t size)
{
    uint32_t nodes_count = 0;
    uint32_t links_count = 0;

    uint32_t i;
    uint32_t *raw_data;

    MAFSA_dnode_t **states;
    uint8_t *mem_p;

    MAFSA_daciuk_delete_branch(da, &da->root);
    Judy1FreeArray(&da->reg, NULL);

    mem_p = (uint8_t *) memo;

    memcpy(&nodes_count, mem_p, sizeof(nodes_count));
    mem_p += sizeof(nodes_count);

    memcpy(&links_count, mem_p, sizeof(links_count));
    mem_p += sizeof(links_count);

    if (NULL == (raw_data = (uint32_t *) malloc((nodes_count + links_count) * sizeof(uint32_t))))
    {
        return -1;
    }

    memcpy(raw_data, mem_p, sizeof(raw_data[0]) * (nodes_count + links_count));
    mem_p += sizeof(raw_data[0]) * (nodes_count + links_count);

    if (NULL == (states = (MAFSA_dnode_t **) malloc(nodes_count * sizeof(MAFSA_dnode_t *))))
    {
        free(raw_data); return -1;
    }

    for (i = 0; i < nodes_count; ++i)
    {
        MAFSA_dpool_alloc(da->pool, (void **) &states[i]);
        MAFSA_dnode_setup(states[i], da->ml);
    }

    da->root = states[0];

    for (i = 0; i < nodes_count; ++i)
    {
        MAFSA_node_t a_node;
        MAFSA_link_t a_link;  /* a_child */

        uint32_t links_begin;

        a_node = raw_data[i];
        states[i]->fn = MAFSA_node_get_final(a_node);
        links_begin   = MAFSA_node_get_children_start(a_node);

        for (a_link = raw_data[links_begin + nodes_count]; 0 == MAFSA_link_get_terminating(a_link); a_link = raw_data[++links_begin + nodes_count])
        {
            uint32_t where;
            MAFSA_letter label;

            where = MAFSA_link_get_link(a_link);
            label = MAFSA_link_get_label(a_link);

            MAFSA_dnode_get_chd(states[i], label) = states[where];
            MAFSA_dnode_add_ref(states[where], 1);
        }
    }

    for (i = 1; i < nodes_count; ++i)
    {
        Judy1Set(&da->reg, (Word_t) states[i], NULL);
    }

    return 0;
}

int MAFSA_daciuk_load_file(MAFSA_daciuk_t *da, const char *fn)
{
    int          fd;
    void        *memo;
    size_t       size;
    struct stat  st;

	if (0 != stat(fn, &st))
    {
        return -1;
    }

	if (-1 == (fd = open(fn, O_RDONLY)))
    {
        return -1;
    }

    if (0 >= (size = st.st_size))
    {
        return -1;
    }

    if (MAP_FAILED == (memo = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0)))
    {
        close(fd); return -1;
    }

    if (0 != MAFSA_daciuk_load_memo(da, memo, size))
    {
        munmap(memo, size); close(fd); return -1;
    }

	if (0 != munmap(memo, size))
    {
        close(fd); return -1;
    }
    
	if (0 != close(fd))
    {
        return -1;
    }

    return 0;
}

