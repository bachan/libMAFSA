
/*
 * Copyright (C) 2007-2009, libturglem development team.
 * This file is released under the LGPL.
 *
 */

#ifndef __UNIT_ADAPTER_H__
#define __UNIT_ADAPTER_H__

#include <MAFSA/adapter.h>

/*
 *  0 : A
 * 25 : Z
 * 26 : -
 * 27 : '
 * 28 : |
 *
 */

#define UNIT_LETTER_A           0
#define UNIT_LETTER_Z          25
#define UNIT_LETTER_DIFFIS     26
#define UNIT_LETTER_APOSTROPHE 27
#define UNIT_LETTER_DELIM      28

#ifdef __cplusplus
extern "C" {
#endif

/*void MAFSACALL callback_test3(void *user_data, const MAFSA_letter *s, size_t sz);*/

size_t UNIT_conv_string_to_letter_utf8 (const char *s, MAFSA_letter *l, size_t sz_l);
size_t UNIT_conv_letter_to_string_utf8 (const MAFSA_letter *l, size_t sz_l, char *s, size_t sz_s);

#ifdef __cplusplus
}
#endif

#endif /* __UNIT_ADAPTER_H__ */
