
/*
 * Copyright (C) 2007-2009, libturglem development team.
 * This file is released under the LGPL.
 *
 */

#include <stdio.h>
#include "unit_adapter.h"

size_t UNIT_conv_string_to_letter_utf8(const char *s, MAFSA_letter *l, size_t sz_l)
{
    const u_char *i = (const u_char *) s;
    size_t pos = 0;
    while (*i && pos < sz_l)
    {
        if (i[0] == '\'')
        {
            l[pos] = UNIT_LETTER_APOSTROPHE;
        }
        else if (i[0] == '-')
        {
            l[pos] = UNIT_LETTER_DIFFIS;
        }
        else
        {
            if ((i[0] >= 'A') && (i[0] <= 'Z'))
            {
                l[pos] = (MAFSA_letter) (i[0] - 'A');
            }
            else if ((i[0] >= 'a') && (i[0] <= 'z'))
            {
                l[pos] = (MAFSA_letter) (i[0] - 'a');
            }
            else
            {
                return pos;
            }
        }
        ++i;
        ++pos;
    }
    return pos;
}

size_t UNIT_conv_letter_to_string_utf8(const MAFSA_letter *l, size_t sz_l, char *s, size_t sz_s)
{
    size_t pos = 0;
    while (pos < sz_l && pos < sz_s - 1)
    {
        switch (l[pos])
        {
            case UNIT_LETTER_DIFFIS:     s[pos] = '-';                   break;
            case UNIT_LETTER_APOSTROPHE: s[pos] = '\'';                  break;
            case UNIT_LETTER_DELIM:      s[pos] = '|';                   break;
            default:                        s[pos] = (char) ('A' + l[pos]); break;
        }
        ++pos;
    }
    s[pos] = 0;
    return pos;
}

