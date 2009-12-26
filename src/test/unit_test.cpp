
/*
 * Copyright (C) 2007-2009, libturglem development team.
 * This file is released under the LGPL.
 *
 */

#include "unit_adapter.h"

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <MAFSA/automaton.h>
#include <MAFSA/daciuk.hpp>

static void test1(const char *fn)
{
    MAFSA::daciuk<UNIT_LETTER_DELIM> test_daciuk;
    MAFSA_letter l [128];

    ssize_t sz;

    sz = UNIT_conv_string_to_letter_utf8("ABC", l, 128);
    test_daciuk.insert(l, sz);

    sz = UNIT_conv_string_to_letter_utf8("XYZ", l, 128);
    test_daciuk.insert(l, sz);

    sz = UNIT_conv_string_to_letter_utf8("ABCDEF", l, 128);
    test_daciuk.insert(l, sz);

    sz = UNIT_conv_string_to_letter_utf8("VAL", l, 128);
    l[sz++] = UNIT_LETTER_DELIM; l[sz++] = 7;
    test_daciuk.insert(l, sz);

    sz = UNIT_conv_string_to_letter_utf8("SEARCHITEM", l, 128);
    l[sz++] = UNIT_LETTER_DELIM;
    sz += UNIT_conv_string_to_letter_utf8("HUYORCHITEM", l + sz, 128 - sz);
    test_daciuk.insert(l, sz);

    sz = UNIT_conv_string_to_letter_utf8("VALA", l, 128);
    l[sz++] = UNIT_LETTER_DELIM; l[sz++] = 15;
    test_daciuk.insert(l, sz);

    sz = UNIT_conv_string_to_letter_utf8("DOUBLE", l, 128);
    l[sz++] = UNIT_LETTER_DELIM; l[sz++] = 1;
    l[sz++] = UNIT_LETTER_DELIM; l[sz++] = 2;
    test_daciuk.insert(l, sz);

    test_daciuk.save_to_file(fn);
}

static void test2(const char *fn)
{
    MAFSA::daciuk<UNIT_LETTER_DELIM> test_daciuk;
    MAFSA_letter l [128];

    ssize_t sz;
    test_daciuk.load_from_file(fn);

    sz = UNIT_conv_string_to_letter_utf8("QWERTY", l, 128);
    test_daciuk.insert(l, sz);

    sz = UNIT_conv_string_to_letter_utf8("ONEMOREQWERTY", l, 128);
    test_daciuk.insert(l, sz);

    sz = UNIT_conv_string_to_letter_utf8("VAL", l, 128);
    l[sz++] = UNIT_LETTER_DELIM; l[sz++] = 5;
    test_daciuk.insert(l, sz);

    sz = UNIT_conv_string_to_letter_utf8("VALA", l, 128);
    l[sz++] = UNIT_LETTER_DELIM; l[sz++] = 8;
    test_daciuk.insert(l, sz);

    sz = UNIT_conv_string_to_letter_utf8("DOUBLE", l, 128);
    l[sz++] = UNIT_LETTER_DELIM; l[sz++] = 20;
    l[sz++] = UNIT_LETTER_DELIM; l[sz++] = 21; l[sz++] = 21;
    test_daciuk.insert(l, sz);

    test_daciuk.save_to_file(fn);
}

void MAFSACALL callback_test3(void *user_data, const MAFSA_letter *s, size_t sz)
{
    (void) user_data;

    if (!sz) return;

    char str[1024];
    UNIT_conv_letter_to_string_utf8(s, sz, str, 1024);
    printf("    %s\n", str);
}

static void test3(const char *fn)
{
    MAFSA_automaton ma = MAFSA_automaton_load_from_binary_file(fn, 0);
    MAFSA_letter l_A = 0, l [128], tmp [1024], out [128];

    size_t sz, r_sz;
    int fnd1, fnd2, r_search;

    printf("test 'A'...\n");
    MAFSA_automaton_enumerate(ma, &l_A, 1, tmp, 1024, 0, callback_test3);

    printf("test all...\n");
    MAFSA_automaton_enumerate(ma, 0, 0, tmp, 1024, 0, callback_test3);

    printf("test bad tmp buffer...\n");
    MAFSA_automaton_enumerate(ma, 0, 0, 0, 0, 0, callback_test3);
    printf("OK!\n");

    printf("test search enumerate...\n");
    sz = UNIT_conv_string_to_letter_utf8("xxxvSEARCHITEMvala", l, 128);
    MAFSA_automaton_search_enumerate(ma, l, sz, tmp, 1024, 0, UNIT_LETTER_DELIM, callback_test3);

    printf("test find...\n");
    sz = UNIT_conv_string_to_letter_utf8("qwerty", l, 128);
    fnd1 = MAFSA_automaton_find(ma, l, sz);
    sz = UNIT_conv_string_to_letter_utf8("qwert", l, 128);
    fnd2 = MAFSA_automaton_find(ma, l, sz);

    if (fnd1 && !fnd2)
    {
        printf("passed!\n");
    }
    else
    {
        printf("test failed, exiting...\n");
        exit(EXIT_FAILURE);
    }

    printf("test search...\n");
    sz = UNIT_conv_string_to_letter_utf8("valaonemoreqwertyvalay", l, 128);
    r_search = MAFSA_automaton_search(ma, l, sz, out, 128, &r_sz);
    printf("SEARCH: %d\n", r_search);

    if (-1 != r_search)
    {
        char buf [128];
        UNIT_conv_letter_to_string_utf8(out, r_sz, buf, 128);
        printf("FOUND: '%s'\n", buf);
    }

    MAFSA_automaton_close(ma);
}

static void test4(const char *fn)
{
    MAFSA_automaton ma = MAFSA_automaton_load_from_binary_file(fn, 0);
    MAFSA_letter l [128], tmp [1024];

    int my_ints [10];
    size_t i, sz, int_cnt;

    printf("testing VAL...\n");
    sz = UNIT_conv_string_to_letter_utf8("valA", l, 128);

    int_cnt = MAFSA_automaton_enumerate_int(ma, l, sz, tmp, 1024, UNIT_LETTER_DELIM, my_ints, 10);
    for (i = 0; i < int_cnt; i++) printf("    value %d\n", my_ints[i]);

    MAFSA_automaton_close(ma);
}

static void test5(const char *fn)
{
    MAFSA_automaton ma = MAFSA_automaton_load_from_binary_file(fn, 0);
    MAFSA_letter l [128], tmp [1024];

    int my_ints [10];
    size_t i, sz, int_cnt;

    printf("testing DOUBLE...\n");
    sz = UNIT_conv_string_to_letter_utf8("double", l, 128);

    int_cnt = MAFSA_automaton_enumerate_int_pair(ma, l, sz, tmp, 1024, UNIT_LETTER_DELIM, my_ints, 10);
    for (i = 0; i < int_cnt; i++) printf("    value pair (%d, %d)\n", my_ints[i * 2], my_ints[i * 2 + 1]);

    MAFSA_automaton_close(ma);
}

int main()
{
    char tmpfn [1024];

    strcpy(tmpfn, "/tmp/test_MAFSA_XXXXXX");
    close(mkstemp(tmpfn));

    test1(tmpfn);
    test2(tmpfn);

    printf("test3\n");
    test3(tmpfn);

    printf("test4\n");
    test4(tmpfn);

    printf("test5\n");
    test5(tmpfn);

    unlink(tmpfn);
    return 0;
}

