// 
//
// Copyright (C) 2004 SIPfoundry Inc.
// Licensed by SIPfoundry under the LGPL license.
//
// Copyright (C) 2004 Pingtel Corp.
// Licensed to SIPfoundry under a Contributor Agreement.
//
// $$
//////////////////////////////////////////////////////////////////////////////

#ifndef INCLUDE_RESPARSE_TYPES_H
#define INCLUDE_RESPARSE_TYPES_H

#include <limits.h>
#include <sys/types.h>
#if defined (_WIN32) /* Use Columbia versions for win32 only --GAT */
#	include "resparse/wnt/sysdep.h"
#elif defined (_VXWORKS)
#	include <netinet/in.h>
#endif

typedef unsigned char boolean;

#ifdef __STDC__
#define MAX32U 0xFFFFFFFFU
#else
#define MAX32U 0xFFFFFFFF
#endif
#define MAX32  0x8FFFFFFF

/* 32 bit machines */
#ifndef RTP_VERSION
#if ULONG_MAX == MAX32U
typedef short int16;
typedef int   int32;
typedef unsigned long  u_int32;
typedef unsigned short u_int16;
/* 16 bit machines */
#elif ULONG_MAX == 0xFFFF
typedef int  int16;
typedef long int32;
typedef unsigned long  u_int32;
typedef unsigned int   u_int16; 
/* 64 bit machines */
#else
typedef short int16;
typedef int   int32;
typedef unsigned int   u_int32;
typedef unsigned short u_int16;
#endif
typedef char int8;
typedef unsigned char u_int8;

#endif /* RTP_VERSION */

/* Kludge if we can't get 64-bit integers. */
#ifndef NOLONGLONG
typedef long long int int64;
typedef unsigned long long u_int64;
#else
typedef long int int64;
#endif /* NOLONGLONG */

#endif /* INCLUDE_RESPARSE_TYPES_H */
