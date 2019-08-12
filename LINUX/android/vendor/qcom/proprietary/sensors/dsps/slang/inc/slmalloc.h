/* ===================================================================
** Copyright (c) 2015 Qualcomm Technologies, Inc.
** All Rights Reserved.
** Confidential and Proprietary - Qualcomm Technologies, Inc.
**
** FILE: slmalloc.h
** DESC: sensor language - slang - memory manager
**                                 basis for TODO garbage collector
** Revision History
** ================================================================ */
#ifndef SLMALLOC_H
#define SLMALLOC_H

void *slmalloc( size_t size, char *loc);    // loc == caller_id
void slfree( void *p_memory, char *loc);
void slmem_list( void);
void *LatchLiteral( const char *p_from, int len);

#endif // SLMALLOC_H




