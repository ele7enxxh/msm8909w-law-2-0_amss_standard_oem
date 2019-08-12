#ifndef _DLFCN_H_
#define _DLFCN_H_
/*  $NetBSD: dlfcn.h,v 1.17.2.1 2005/03/21 21:25:37 jwise Exp $ */

/*-
 * Copyright (c) 1998 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Paul Kranenburg.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *        This product includes software developed by the NetBSD
 *        Foundation, Inc. and its contributors.
 * 4. Neither the name of The NetBSD Foundation nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifdef __cplusplus
extern "C" {
#endif

/* Values for dlopen `mode' */
#define RTLD_LAZY 1
#define RTLD_NOW 2
#define RTLD_GLOBAL 0x100       /* Allow global searches in object */
#define RTLD_LOCAL 0x200

/*
 * Special handle arguments for dlsym()
 */
#define _RTLD_NEXT -1 /* Search subsequent objects */
#define _RTLD_DEFAULT -2 /* Use default search algorithm */
#define _RTLD_SELF -3 /* Search the caller itself */

#define RTLD_NEXT ((void*)_RTLD_NEXT) /* Search subsequent objects */
#define RTLD_DEFAULT ((void*)_RTLD_DEFAULT) /* Use default search algorithm */
#define RTLD_SELF ((void*)_RTLD_SELF) /* Search the caller itself */

typedef struct _dl_info {
  const char* dli_fname; /* File defining the symbol */
  void* dli_fbase; /* Base address */
  const char* dli_sname; /* Symbol name */
  const void* dli_saddr; /* Symbol address */
} Dl_info;

/*
 * User interface to the run-time linker
 */
void* dlopen(const char*, int);
int dlclose(void*);
void* dlsym(void* __restrict, const char* __restrict);
int dladdr(const void* __restrict, Dl_info* __restrict);
char* dlerror(void);

typedef struct _dl_vtbl {
  int size;
  void (*msg)(int level, const char* file, int line, const char* fmt, ...);
  int (*verify_start)(const char*, const unsigned char*, int, const unsigned char*, int, void**);
  int (*verify_checkseg)(void*, int, const unsigned char*, int);
  int (*verify_stop)(void*);
} DL_vtbl;

int dlinit(int, char**);
int dlinitex(int, char**, DL_vtbl*);
int dlinitex_pie(int, char**, DL_vtbl*, void* base);
// TODO
//void dldeinit(void);

#ifdef __cplusplus
}
#endif

#endif /* !defined(_DLFCN_H_) */
