
/*
 * Copyright (C) 2007-2009, libturglem development team.
 * This file is released under the LGPL.
 *
 */

#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <MAFSA/automaton.h>
#include <MAFSA/daciuk.h>

#define MAX_LETTER 255

const u_char *urls [] =
{
    (const u_char *) "http://yandex.ru/yandsearch",         /* ... */
    (const u_char *) "http://images.yandex.ru/yandsearch",  /* ... */
    (const u_char *) "http://yandex.ua/yandsearch",         /* ... */
    (const u_char *) "http://yandex.ru:8081/yandsearch",    /* ... */
    (const u_char *) "http://hghltd.yandex.net/yandbtm",    /* 30 Jan 2009 */
    (const u_char *) "http://google.",                      /* 30 Jan 2009 */
    (const u_char *) "http://www.rambler.ru/srch",          /* 30 Jan 2009 */
    (const u_char *) "http://rambler.ru/srch",              /* 03 Feb 2009 */
    (const u_char *) "http://search.live.com/",             /* ... */
    (const u_char *) "http://search.icq.com/",              /* ... */
    (const u_char *) "http://go.mail.ru/search",            /* ... */
    (const u_char *) "http://foto.mail.ru/",                /* ... */
    (const u_char *) "http://my.mail.ru/",                  /* ... */
    (const u_char *) "odnoklassniki.ru/",                   /* ... */
    (const u_char *) "odnoklassniki.ua/",                   /* ... */
    (const u_char *) "ifolder.ru/",                         /* ... */
    (const u_char *) "http://depositfiles.com/ru",          /* ... */
    (const u_char *) "http://tempfile.ru/file/",            /* ... */
    (const u_char *) "http://ads.sup.com/",                 /* 16 Feb 2009 */
    (const u_char *) "ad.ad24.ru/bb.cgi",                   /* ... */
    (const u_char *) "multitran.ru/c/m.exe",                /* ... */
    (const u_char *) "http://file//localhost",              /* ... */
    (const u_char *) "http://file///",                      /* ... */

    NULL
};

int main(int argc, char *argv[])
{
    MAFSA_daciuk_t da;
    MAFSA_automaton ma;

    int i;
    int rc;

    uint32_t ex = 0, ok = 0;
    uint32_t cs = 0, ss = 0;

    size_t size;
    void *memo;

    MAFSA_daciuk_setup(&da, MAX_LETTER);

    for (i = 0; urls[i]; ++i)
    {
        size_t sz;
        MAFSA_letter word [1024];

        struct timeval tvc1;
        struct timeval tvc2;

        sz = strlen((const char *) urls[i]);
        memcpy(word, urls[i], sz);

        gettimeofday(&tvc1, NULL);
        MAFSA_daciuk_insert(&da, word, sz);
        gettimeofday(&tvc2, NULL);

        cs += (tvc2.tv_sec - tvc1.tv_sec) * 1000000 + (tvc2.tv_usec - tvc1.tv_usec);
    }

    MAFSA_daciuk_save_memo(&da, &memo, &size);
    ma = MAFSA_automaton_load_from_binary_memo(memo, NULL);

    for (;;)
    {
        MAFSA_letter outbuf [1024];
        char urlbuf [4096], *pos;
        size_t outsz = 0, urlsz = 0;

        struct timeval tvs1;
        struct timeval tvs2;

        if (NULL == fgets(urlbuf, 4096, stdin))
        {
            break;
        }

        if (NULL != (pos = strchr(urlbuf, '\n'))) *pos = 0;
        if (NULL != (pos = strchr(urlbuf, '\r'))) *pos = 0;

        urlsz = strlen(urlbuf);

        gettimeofday(&tvs1, NULL);
        rc = MAFSA_automaton_search(ma, (const MAFSA_letter *) urlbuf, urlsz, outbuf, 1024, &outsz);
        gettimeofday(&tvs2, NULL);

        ss += (tvs2.tv_sec - tvs1.tv_sec) * 1000000 + (tvs2.tv_usec - tvs1.tv_usec);

        if (-1 == rc)
        {
            ++ok;
        }
        else
        {
            char dbg [4096];

            ++ex;
            pos = dbg;

            snprintf(pos, rc + 1, "%s", urlbuf);
            pos += rc;

            snprintf(pos, sizeof("\033[1;31m"), "\033[1;31m");
            pos += sizeof("\033[1;31m") - 1;

            snprintf(pos, outsz + 1, "%s", outbuf);
            pos += outsz;

            snprintf(pos, sizeof("\033[0m"), "\033[0m");
            pos += sizeof("\033[0m") - 1;

            snprintf(pos, urlsz - outsz - rc + 1, "%s", urlbuf + rc + outsz);
            printf("Filter (%03d .. %03u): %s\n", rc, (unsigned int) (rc + outsz), dbg);
        }
    }

    printf("\n");
    printf("Result: ex = %d, ok = %d\n", ex, ok);
    printf("TimeComple: %6u.%06u seconds\n", cs / 1000000, cs % 1000000);
    printf("TimeSearch: %6u.%06u seconds\n", ss / 1000000, ss % 1000000);

    MAFSA_automaton_close(ma);
    MAFSA_daciuk_erase(&da);

    return 0;
}

