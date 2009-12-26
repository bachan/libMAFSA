
/*
 * Copyright (C) 2007-2009, libturglem development team.
 * This file is released under the LGPL.
 *
 */

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <MAFSA/automaton.h>

MAFSA_letter link_get_label(const uint32_t data)
{
    return (MAFSA_letter) (data >> 24);
}

MAFSA_letter link_set_label(const uint32_t data, const MAFSA_letter v)
{
    return (MAFSA_letter) ((0x00FFFFFF & data) | (v << 24));
}

uint32_t link_get_link(const uint32_t data)
{
    return data & 0x00FFFFFF;
}

uint32_t link_set_link(const uint32_t data, const uint32_t l)
{
    return (0xFF000000 & data) | l;
}

int link_is_terminating(const uint32_t data)
{
    return data == 0xFF000000;
}

uint32_t link_set_terminating()
{
    return 0xFF000000;
}

int node_is_final(const uint32_t data)
{
    return (data & 0x80000000);
}

uint32_t node_get_children_start(const uint32_t data)
{
    return data & 0x7FFFFFFF;
}

uint32_t node_set_children_start(const uint32_t data, uint32_t v)
{
    return (data & 0x80000000) | v;
}

uint32_t node_set_final(const uint32_t data, int v)
{
    return v ? (data | 0x80000000) : (data & 0x7FFFFFFF);
}

uint32_t MAFSA_delta(const uint32_t * links, uint32_t state, MAFSA_letter label)
{
    uint32_t links_begin = node_get_children_start(state);
    uint32_t t = links[links_begin];

    while (!link_is_terminating(t))
    {
        if (link_get_label(t) == label)
        {
            return link_get_link(t);
        }

        t = links[++links_begin];
    }

    return 0;
}

extern MAFSA_automaton MAFSA_automaton_load_from_binary_file(const char *fname, int *error)
{
    uint32_t nodes_count;
    uint32_t links_count;
    FILE *fp;

    MAFSA_automaton ret_data;
    if (error) *error = 0;

    if (NULL == (fp = fopen(fname, "r")))
    {
        if (error) *error = MAFSA_ERROR_CANT_OPEN_FILE;
        return 0;
    }

    ret_data = (MAFSA_automaton) malloc(sizeof(struct MAFSA_automaton_struct));

    if (0 == ret_data)
    {
        if (error) *error = MAFSA_ERROR_NOMEM;
        return 0;
    }

    if ((1 != fread(&nodes_count, sizeof(uint32_t), 1, fp)) || (1 != fread(&links_count, sizeof(uint32_t), 1, fp)))
    {
        if (error) *error = MAFSA_ERROR_CORRUPTED_FILE;
        fclose(fp);
        free(ret_data);
        return 0;
    }

    ret_data->ptr_nodes = (uint32_t *) malloc(nodes_count * sizeof(uint32_t));

    if (NULL == ret_data->ptr_nodes)
    {
        if (error) *error = MAFSA_ERROR_NOMEM;
        fclose(fp);
        free(ret_data);
        return 0;
    }

    if (nodes_count != fread(ret_data->ptr_nodes, sizeof(uint32_t), nodes_count, fp))
    {
        if (error) *error = MAFSA_ERROR_CORRUPTED_NODES;
        fclose(fp);
        free(ret_data->ptr_nodes);
        free(ret_data);
        return 0;
    }

    ret_data->ptr_links = (uint32_t *) malloc(links_count * sizeof(uint32_t));

    if (0 == ret_data->ptr_links)
    {
        if (error) *error = MAFSA_ERROR_NOMEM;
        fclose(fp);
        free(ret_data->ptr_nodes);
        free(ret_data);
        return 0;
    }

    if (links_count != fread(ret_data->ptr_links, sizeof(uint32_t), links_count, fp))
    {
        if (error) *error = MAFSA_ERROR_CORRUPTED_LINKS;
        fclose(fp);
        free(ret_data->ptr_nodes);
        free(ret_data->ptr_links);
        free(ret_data);
        return 0;
    }

    fclose(fp);

    ret_data->shared = 0;
    return ret_data;
}

extern MAFSA_automaton MAFSA_automaton_load_from_binary_memo(const void *bmemo, int *error)
{
    uint32_t nodes_count;
    uint32_t links_count;

    const uint8_t *mem_p = (const uint8_t *) bmemo;
    MAFSA_automaton ret_data;

    if (error) *error = 0;

    ret_data = (MAFSA_automaton) malloc(sizeof(struct MAFSA_automaton_struct));

    if (0 == ret_data)
    {
        if (error) *error = MAFSA_ERROR_NOMEM;
        return 0;
    }

    memcpy(&nodes_count, mem_p, sizeof(uint32_t));
    mem_p += sizeof(uint32_t);

    memcpy(&links_count, mem_p, sizeof(uint32_t));
    mem_p += sizeof(uint32_t);

    ret_data->ptr_nodes = (uint32_t *) malloc(nodes_count * sizeof(uint32_t));

    if (0 == ret_data->ptr_nodes)
    {
        if (error) *error = MAFSA_ERROR_NOMEM;
        free(ret_data);
        return 0;
    }

    memcpy(ret_data->ptr_nodes, mem_p, sizeof(uint32_t) * nodes_count);
    mem_p += sizeof(uint32_t) * nodes_count;

    ret_data->ptr_links = (uint32_t*)malloc(links_count * sizeof(uint32_t));

    if (0 == ret_data->ptr_links)
    {
        if (error) *error = MAFSA_ERROR_NOMEM;
        free(ret_data->ptr_nodes);
        free(ret_data);
        return 0;
    }

    memcpy(ret_data->ptr_links, mem_p, sizeof(uint32_t) * links_count);
    mem_p += sizeof(uint32_t) * links_count;

    ret_data->shared = 0;
    return ret_data;
}

extern MAFSA_automaton MAFSA_automaton_attach(const void *ptr_nodes, const void *ptr_links, int *error)
{
    MAFSA_automaton ret_data = (MAFSA_automaton)malloc(sizeof(struct MAFSA_automaton_struct));
    if (error) *error = 0;

    if (0 == ret_data)
    {
        if (error) *error = MAFSA_ERROR_NOMEM;
        return 0;
    }

    ret_data->ptr_nodes = (uint32_t *) ptr_nodes;
    ret_data->ptr_links = (uint32_t *) ptr_links;
    ret_data->shared = 1;

    return ret_data;
}

extern void MAFSA_automaton_close(MAFSA_automaton ma)
{
    if (0 == ma)
    {
        return;
    }

    if (0 == ma->shared)
    {
        free(ma->ptr_nodes);
        free(ma->ptr_links);

#if 0
        if (ma->ptr_nodes)
        {
            free(ma->ptr_nodes);
        }

        if (ma->ptr_links)
        {
            free(ma->ptr_links);
        }
#endif
    }

    free(ma);
}

extern ssize_t MAFSA_automaton_find(MAFSA_automaton ma, const MAFSA_letter *l, size_t sz_l)
{
    uint32_t i = 0;
    uint32_t current = ma->ptr_nodes[0];
    uint32_t where = 0;

    while (i < sz_l)
    {
        MAFSA_letter label = l[i++];

        where = MAFSA_delta(ma->ptr_links, current, label);

        if (where)
        {
            current = ma->ptr_nodes[where];
        }
        else
        {
            break;
        }
    }

    if (i == sz_l && node_is_final(current))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

extern ssize_t MAFSA_automaton_search(MAFSA_automaton ma, const MAFSA_letter *l, size_t sz_l,
    MAFSA_letter *out, size_t sz_out, size_t *sz_out_result)
{
    ssize_t cur_pos;

    for (cur_pos = 0; cur_pos < sz_l; cur_pos++)
    {
        uint32_t i = cur_pos;
        uint32_t current = ma->ptr_nodes[0];
        uint32_t where = 0;

        while (i < sz_l && !node_is_final(current))
        {
            MAFSA_letter label = l[i++];

            if (0 != (where = MAFSA_delta(ma->ptr_links, current, label)))
            {
                current = ma->ptr_nodes[where];
            }
            else
            {
                break;
            }
        }

        if (node_is_final(current))
        {
            ssize_t cpy_sz;
            
            cpy_sz = i - cur_pos < sz_out ? i - cur_pos : sz_out;
            memcpy(out, l + cur_pos, cpy_sz);

            if (sz_out_result)
            {
                *sz_out_result = cpy_sz;
            }

            return cur_pos;
        }
    }

    return -1;
}

struct MAFSA_stack_struct
{
    MAFSA_letter *buffer;
    size_t reserved;
    ssize_t current;
};

static size_t stack_size(const struct MAFSA_stack_struct* st)
{
    return st->current + 1;
}

static MAFSA_letter stack_pop(struct MAFSA_stack_struct* st)
{
    if (st->current < 0) return 0;

    return st->buffer[st->current--];
}

static void stack_push(struct MAFSA_stack_struct* st, MAFSA_letter l)
{
    if (st->current + 1 < st->reserved)
    {
        st->buffer[++st->current] = l;
    }
}

static void enumerate(const MAFSA_automaton ma, const uint32_t cstate, struct MAFSA_stack_struct *out_stack,
    MAFSA_automaton_string_handler fetcher, void *user_data)
{
    uint32_t links_begin, child, where;

    if (node_is_final(cstate))
    {
        fetcher(user_data, out_stack->buffer, stack_size(out_stack));
    }

    links_begin = node_get_children_start(cstate);

    for (child = ma->ptr_links[links_begin]; !link_is_terminating(child); child = ma->ptr_links[++links_begin])
    {
        stack_push(out_stack, link_get_label(child));
        where = link_get_link(child);
        enumerate(ma, ma->ptr_nodes[where], out_stack, fetcher, user_data);
        stack_pop(out_stack);
    }
}

extern void MAFSA_automaton_enumerate(MAFSA_automaton ma, const MAFSA_letter *l, size_t sz_l, MAFSA_letter *tmp,
    size_t sz_tmp, void *user_data, MAFSA_automaton_string_handler fetcher)
{
    uint32_t i;
    uint32_t current = ma->ptr_nodes[0];
    uint32_t where = 1;

    struct MAFSA_stack_struct tmp_stack;

    tmp_stack.buffer   = tmp;
    tmp_stack.reserved = sz_tmp;
    tmp_stack.current  = -1;

    for (i = 0; i < sz_l; ++i)
    {
        MAFSA_letter label = l[i];
        stack_push(&tmp_stack, label);

        if (0 != (where = MAFSA_delta(ma->ptr_links, current, label)))
        {
            current = ma->ptr_nodes[where];
        }
        else
        {
            break;
        }
    }

    if ((i == sz_l && where) || 0 == l)
    {
        enumerate(ma, current, &tmp_stack, fetcher, user_data);
    }
}

extern void MAFSA_automaton_search_enumerate(MAFSA_automaton ma, const MAFSA_letter *l, size_t sz_l, MAFSA_letter *tmp,
    size_t sz_tmp, void *user_data, MAFSA_letter delim, MAFSA_automaton_string_handler fetcher)
{
    ssize_t cur_pos;

    for (cur_pos = 0; cur_pos < sz_l; ++cur_pos)
    {
        uint32_t i = cur_pos;
        uint32_t current = ma->ptr_nodes[0];
        uint32_t where = 0;

        struct MAFSA_stack_struct tmp_stack;

        tmp_stack.buffer   = tmp;
        tmp_stack.reserved = sz_tmp;
        tmp_stack.current  = -1;

        while (i <= sz_l)
        {
            MAFSA_letter label;

            if (0 != (where = MAFSA_delta(ma->ptr_links, current, delim)))
            {
                stack_push(&tmp_stack, delim);
                enumerate(ma, ma->ptr_nodes[where], &tmp_stack, fetcher, user_data);
                stack_pop(&tmp_stack);
            }

            label = l[i++];
            stack_push(&tmp_stack, label);

            if (0 != (where = MAFSA_delta(ma->ptr_links, current, label)))
            {
                current = ma->ptr_nodes[where];
            }
            else
            {
                break;
            }
        }
    }
}

extern int MAFSA_automaton_val_to_int(const MAFSA_letter *l,
    size_t sz_l, MAFSA_letter delim, int *out)
{
    int i;
    int count;

    for (i = sz_l - 1, count = 0; i >= 0; --i)
    {
        count *= delim;
        count += l[i];
    }

    out[0] = count;

    return 0;
}

extern int MAFSA_automaton_str_to_int(const MAFSA_letter *l,
    size_t sz_l, MAFSA_letter delim, int *out)
{
    int i;

    for (i = 0; i < sz_l; ++i)
    {
        if (delim == l[i])
        {
            break;
        }
    }

    return MAFSA_automaton_val_to_int(l + i + 1,
        sz_l - i - 1, delim, out);
}

extern int MAFSA_automaton_val_to_int_pair(const MAFSA_letter *l,
    size_t sz_l, MAFSA_letter delim, int *out)
{
    int i;
    int count;
    int valid = 0;

    for (i = sz_l - 1, count = 0; i >= 0; --i)
    {
        if (delim == l[i])
        {
            valid = 1;
            break;
        }

        count *= delim;
        count += l[i];
    }

    if (0 == valid)
    {
        return -1;
    }

    out[1] = count;

    for (--i, count = 0; i >= 0; --i)
    {
        count *= delim;
        count += l[i];
    }

    out[0] = count;

    return 0;
}

extern int MAFSA_automaton_str_to_int_pair(const MAFSA_letter *l,
    size_t sz_l, MAFSA_letter delim, int *out)
{
    int i;

    for (i = 0; i < sz_l; ++i)
    {
        if (delim == l[i])
        {
            break;
        }
    }

    return MAFSA_automaton_val_to_int_pair(l + i + 1,
        sz_l - i - 1, delim, out);
}

struct str_inner_params
{
    int *data;
    size_t data_sz;
    MAFSA_letter delim;
    int processed;
};

static void MAFSACALL MAFSA_str_to_int(void *user_data,
    const MAFSA_letter *l, size_t sz_l)
{
    struct str_inner_params *data = (struct str_inner_params *) user_data;

    if (data->processed < data->data_sz)
    {
        if (0 == MAFSA_automaton_val_to_int(l, sz_l, data->delim,
                data->data + data->processed))
        {
            data->processed++;
        }
    }
}

static void MAFSACALL MAFSA_str_to_int_pair(void *user_data,
    const MAFSA_letter *l, size_t sz_l)
{
    struct str_inner_params *data = (struct str_inner_params *) user_data;

    if (data->processed < data->data_sz / 2)
    {
        if (0 == MAFSA_automaton_val_to_int_pair(l, sz_l, data->delim,
                data->data + 2 * data->processed))
        {
            data->processed++;
        }
    }
}

static void enumerate_ints(const MAFSA_automaton ma, struct MAFSA_stack_struct *ptr_stack,
    uint32_t cstate, MAFSA_automaton_string_handler fetcher, void *user_data)
{
    uint32_t links_begin, child, where;

    if (node_is_final(cstate))
    {
        fetcher(user_data, ptr_stack->buffer, stack_size(ptr_stack));
    }

    links_begin = node_get_children_start(cstate);

    for (child = ma->ptr_links[links_begin]; !link_is_terminating(child); child = ma->ptr_links[++links_begin])
    {
        stack_push(ptr_stack, link_get_label(child));

        where = link_get_link(child);

        enumerate_ints(ma, ptr_stack, ma->ptr_nodes[where], fetcher, user_data);

        stack_pop(ptr_stack);
    }
}

static void MAFSA_automaton_enumerate_ints(MAFSA_automaton ma, const MAFSA_letter *l, size_t sz_l, MAFSA_letter *tmp,
    size_t sz_tmp, MAFSA_letter delim, void *user_data, MAFSA_automaton_string_handler fetcher)
{
    uint32_t i;
    uint32_t current = ma->ptr_nodes[0];
    uint32_t where = 1;

    struct MAFSA_stack_struct tmp_stack;

    tmp_stack.buffer   = tmp;
    tmp_stack.reserved = sz_tmp;
    tmp_stack.current  = -1;

    for (i = 0; i < sz_l; i++)
    {
        MAFSA_letter label = l[i];

        if (0 != (where = MAFSA_delta(ma->ptr_links, current, label)))
        {
            current = ma->ptr_nodes[where];
        }
        else
        {
            break;
        }
    }

    if (i == sz_l && where)
    {
        if (0 != (where = MAFSA_delta(ma->ptr_links, current, delim)))
        {
            enumerate_ints(ma, &tmp_stack, ma->ptr_nodes[where], fetcher, user_data);
        }
    }
}


extern size_t MAFSA_automaton_enumerate_int(MAFSA_automaton ma, const MAFSA_letter *l, size_t sz_l,
    MAFSA_letter *tmp, size_t sz_tmp, MAFSA_letter delim, int *out, size_t sz_out)
{
    struct str_inner_params str;

    str.delim     = delim;
    str.processed = 0;
    str.data      = out;
    str.data_sz   = sz_out;

    MAFSA_automaton_enumerate_ints(ma, l, sz_l, tmp, sz_tmp,
        delim, &str, MAFSA_str_to_int);

    return str.processed;
}

extern size_t MAFSA_automaton_enumerate_int_pair(MAFSA_automaton ma, const MAFSA_letter *l, size_t sz_l,
    MAFSA_letter *tmp, size_t sz_tmp, MAFSA_letter delim, int *out, size_t sz_out)
{
    struct str_inner_params str;

    str.delim     = delim;
    str.processed = 0;
    str.data      = out;
    str.data_sz   = sz_out;

    MAFSA_automaton_enumerate_ints(ma, l, sz_l, tmp, sz_tmp,
        delim, &str, MAFSA_str_to_int_pair);

    return str.processed;
}


