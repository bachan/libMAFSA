
/*
 * Copyright (C) 2007-2009, libturglem development team.
 * This file is released under the LGPL.
 *
 */

#ifndef __MAFSA_AUTOMATON_H__
#define __MAFSA_AUTOMATON_H__

#include <sys/types.h>
#include <MAFSA/internal.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Load and attach errors */

#define MAFSA_ERROR_NOMEM            1
#define MAFSA_ERROR_CANT_OPEN_FILE   2
#define MAFSA_ERROR_CORRUPTED_FILE   3
#define MAFSA_ERROR_CORRUPTED_NODES  4
#define MAFSA_ERROR_CORRUPTED_LINKS  5

struct MAFSA_automaton_struct
{
    uint32_t *ptr_nodes;
    uint32_t *ptr_links;
    int shared;
};

typedef struct MAFSA_automaton_struct *MAFSA_automaton;

/* 
 * Opens binary file or memory region, loads it to automaton.
 * Remember to free by calling MAFSA_automaton_close() which does nothing if automaton was attached.
 *
 */

extern MAFSA_automaton MAFSA_automaton_load_from_binary_file(const char *fname, int *error);
extern MAFSA_automaton MAFSA_automaton_load_from_binary_memo(const void *bmemo, int *error);
extern MAFSA_automaton MAFSA_automaton_attach(const void *ptr_nodes, const void *ptr_links, int *error);
extern void MAFSA_automaton_close(MAFSA_automaton ma);

/* 
 * Checks, whether string [l] is in automaton.
 *
 */

extern ssize_t MAFSA_automaton_find   (MAFSA_automaton ma, const MAFSA_letter *l, size_t sz_l);
extern ssize_t MAFSA_automaton_search (MAFSA_automaton ma, const MAFSA_letter *l, size_t sz_l, MAFSA_letter *out, size_t sz_out, size_t *sz_out_result);

/* 
 * Enumerates all strings, which start with l.
 * If l is NULL or sz is 0, enumerates all strings.
 *
 */

typedef void (MAFSACALL *MAFSA_automaton_string_handler) (void *user_data,
    const MAFSA_letter *l, size_t sz_l);

extern void MAFSA_automaton_enumerate(MAFSA_automaton ma, const MAFSA_letter *l, size_t sz_l, MAFSA_letter *tmp,
    size_t sz_tmp, void *user_data, MAFSA_automaton_string_handler fetcher);

/* 
 * Searches for any substring+delim in automaton in l.
 * Enumerates all variants.
 *
 */

extern void MAFSA_automaton_search_enumerate(MAFSA_automaton ma, const MAFSA_letter *l, size_t sz_l, MAFSA_letter *tmp,
    size_t sz_tmp, void *user_data, MAFSA_letter delim, MAFSA_automaton_string_handler fetcher);

/*
 * Enumerates all ints or int pairs for a given word.
 *
 */

extern size_t MAFSA_automaton_enumerate_int(MAFSA_automaton ma, const MAFSA_letter *l, size_t sz_l,
    MAFSA_letter *tmp, size_t sz_tmp, MAFSA_letter delim, int *out, size_t sz_out);

extern size_t MAFSA_automaton_enumerate_int_pair(MAFSA_automaton ma, const MAFSA_letter *l, size_t sz_l,
    MAFSA_letter *tmp, size_t sz_tmp, MAFSA_letter delim, int *out, size_t sz_out);

/* 
 * Enumerates fuzzy 
 *
 */

typedef unsigned (MAFSACALL *MAFSA_automaton_fuzzy_callback) (void *user_data,
    const MAFSA_letter* s, size_t ls, const MAFSA_letter* t, size_t lt);

extern void MAFSA_automaton_fuzzy_enumerate(MAFSA_automaton ma, const MAFSA_letter* l, size_t sz_l, MAFSA_letter* tmp,
    size_t sz_tmp, void *user_data, MAFSA_automaton_fuzzy_callback sdist, unsigned max_dist, MAFSA_automaton_fuzzy_callback fetcher);

/* 
 * Converts MAFSA ints to real ints.
 *
 */

extern int MAFSA_automaton_str_to_int      (const MAFSA_letter *l, size_t sz_l, MAFSA_letter delim, int *out);
extern int MAFSA_automaton_val_to_int      (const MAFSA_letter *l, size_t sz_l, MAFSA_letter delim, int *out);

extern int MAFSA_automaton_str_to_int_pair (const MAFSA_letter *l, size_t sz_l, MAFSA_letter delim, int *out);
extern int MAFSA_automaton_val_to_int_pair (const MAFSA_letter *l, size_t sz_l, MAFSA_letter delim, int *out);

#ifdef __cplusplus
}
#endif

#endif /* __MAFSA_AUTOMATON_H__ */
