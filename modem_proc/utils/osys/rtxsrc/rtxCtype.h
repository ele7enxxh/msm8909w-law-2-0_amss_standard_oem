/*
 * Copyright (c) 2003-2013 Objective Systems, Inc.
 *
 * This software is furnished under a license and may be used and copied
 * only in accordance with the terms of such license and with the
 * inclusion of the above copyright notice. This software or any other
 * copies thereof may not be provided or otherwise made available to any
 * other person. No title to and ownership of the software is hereby
 * transferred.
 *
 * The information in this software is subject to change without notice
 * and should not be construed as a commitment by Objective Systems, Inc.
 *
 * PROPRIETARY NOTICE
 *
 * This software is an unpublished work subject to a confidentiality agreement
 * and is protected by copyright and trade secret law.  Unauthorized copying,
 * redistribution or other use of this work is prohibited.
 *
 * The above notice of copyright on this source code product does not indicate
 * any actual or intended publication of such source code.
 *
 *****************************************************************************/
/**
 * @file rtxCtype.h
 */
#ifndef _RTXCTYPE_H_
#define _RTXCTYPE_H_

/* Ctype substitution macros */

#define OS_ISASCII(c) ((unsigned)(c) < 0x80)

#define OS_ISUPPER(c) (c >= 'A' && c <= 'Z')
#define OS_ISLOWER(c) (c >= 'a' && c <= 'z')
#define OS_ISDIGIT(c) (c >= '0' && c <= '9')
#define OS_ISALPHA(c) (OS_ISUPPER(c) || OS_ISLOWER(c))
#define OS_ISSPACE(c) ((c >= 0x09 && c <= 0x0d) || (c == ' '))
#define OS_ISPUNCT(c) (c >= 0 && c <= 0x20)
#define OS_ISALNUM(c) (OS_ISALPHA(c) || OS_ISDIGIT(c))
#define OS_ISPRINT(c) (c >= ' ' && c <= '~')
#define OS_ISGRAPH(c) (c >= '!' && c <= '~')
#define OS_ISCNTRL(c) ((c >= 0 && c <= 0x1F) || c == 0x7F)
#define OS_ISXDIGIT(c) \
(OS_ISDIGIT(c) || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f'))

#define OS_TOLOWER(c) (OS_ISUPPER(c) ? (c) - 'A' + 'a' : (c))
#define OS_TOUPPER(c) (OS_ISLOWER(c) ? (c) - 'a' + 'A' : (c))

#endif /* _RTXCTYPE_H_ */

