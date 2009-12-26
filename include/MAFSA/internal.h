
/*
 * Copyright (C) 2007-2009, libturglem development team.
 * This file is released under the LGPL.
 *
 */

#ifndef __MAFSA_INTERNAL_H__
#define __MAFSA_INTERNAL_H__

#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_MSC_EXTENSIONS) && !defined(__BEOS__) && !defined(__CYGWIN__)
#define MAFSA_USE_MSC_EXTENSIONS 1
#endif

#ifndef MAFSACALL
#if defined(MAFSA_USE_MSC_EXTENSIONS)
#define MAFSACALL __cdecl
#elif defined(__GNUC__) && defined(__i386)
#define MAFSACALL __attribute__((cdecl))
#else
#define MAFSACALL
#endif
#endif

typedef uint8_t MAFSA_letter;

#ifdef __cplusplus
}
#endif

#endif /* __MAFSA_INTERNAL_H__ */
