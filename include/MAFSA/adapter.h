
/*
 * Copyright (C) 2007-2009, libturglem development team.
 * This file is released under the LGPL.
 *
 */

#ifndef __MAFSA_ADAPTER_H__
#define __MAFSA_ADAPTER_H__

#include <sys/types.h>
#include <MAFSA/internal.h>

#ifdef __cplusplus
extern "C" {
#endif

/* should return count of converted bytes */

typedef size_t (MAFSACALL *MAFSA_conv_string_to_letter) (const char *s, MAFSA_letter *l, size_t sz_l);
typedef size_t (MAFSACALL *MAFSA_conv_letter_to_string) (const MAFSA_letter *l, size_t sz_l, char *s, size_t sz_s);

/* if you don't like null-terminated strings */

typedef size_t (MAFSACALL *MAFSA_conv_binary_to_letter) (const char *s, size_t sz_s, MAFSA_letter *l, size_t sz_l);
typedef size_t (MAFSACALL *MAFSA_conv_letter_to_binary) (const MAFSA_letter *l, size_t sz_l, char *s, size_t sz_s);

#ifdef __cplusplus
}
#endif

#endif /* __MAFSA_ADAPTER_H__ */
