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

#include "stddef.h"
#include "sys/types.h"

/* 
 * Check if dynamic loading is supported on this target and if so error out
 * if the stub version of the header is being included.
 */
#ifdef FEATURE_DYNAMIC_LOADING_SENSORS
  #error "Incorrect version of this header being is being included! Modify your header include path to include this header from platform/rtld/inc"
#endif //FEATURE_DYNAMIC_LOADING_SENSORS

/* 
 * Values for dlopen `mode'
 */
#define RTLD_LAZY 1
#define RTLD_NOW 2
#define RTLD_GLOBAL 0x100       /* Allow global searches in object */
#define RTLD_LOCAL 0x200
#define RTLD_NOUIMG 0x10000 /* Do not allow loading into micro-image */

/*
 * Special handle arguments for dlsym()
 */
#define _RTLD_NEXT -1 /* Search subsequent objects */
#define _RTLD_DEFAULT -2 /* Use default search algorithm */
#define _RTLD_SELF -3 /* Search the caller itself */

#define RTLD_NEXT ((void*)_RTLD_NEXT) /* Search subsequent objects */
#define RTLD_DEFAULT ((void*)_RTLD_DEFAULT) /* Use default search algorithm */
#define RTLD_SELF ((void*)_RTLD_SELF) /* Search the caller itself */

/* 
 * symbol info returned by dladdr
 */
typedef struct _dl_info {
  const char* dli_fname; /* File defining the symbol */
  void* dli_fbase; /* Base address */
  const char* dli_sname; /* Symbol name */
  const void* dli_saddr; /* Symbol address */
} Dl_info;

/*
 * type of requests supported by dlinfo()
 */
#define RTLD_DI_UIMG 1 /* is loaded in micro-image, p returns int, 0=FALSE */
#define RTLD_DI_UIMG_IN_TREE 2 /* any micro-image loaded in tree, p returns int, 0=FALSE */

/*
 * User interface to the run-time linker
 */
/*
void* dlopen(const char*, int);
int dlclose(void*);
void* dlsym(void* __restrict, const char* __restrict);
int dladdr(const void* __restrict, Dl_info* __restrict);
char* dlerror(void);
*/
int dlinfo(void* __restrict, int request, void*	__restrict p);

typedef struct _dl_vtbl {
  int size;
  void (*msg)(int level, const char* file, int line, const char* fmt, ...); // optional
  int (*verify_start)(const char*, const unsigned char*, int, const unsigned char*, int, void**);
  int (*verify_checkseg)(void*, int, const unsigned char*, int); // optional
  int (*verify_stop)(void*); // optional
  int (*open)(const char *, int, ...);
  int (*close)(int);
  int (*fstat)(int filedes, void *buf);
  size_t (*read)(int ctx, void*, size_t); // defaults
  off_t (*lseek)(int ctx, off_t, int); // defaults
  void* (*malloc)(size_t); // defaults
  void (*free)(void*); // defaults
  void* (*mmap)(void*, size_t, int, int, int, off_t, int ctx); // defaults
  int (*munmap)(void* addr, size_t length, int ctx); // defaults
  int (*mprotect)(void*, size_t, int, int ctx); // defaults
  int (*uimg_x_malloc)(size_t, void**, void**); // optional
  void (*uimg_x_free)(void* addr); // optional
  int (*uimg_x_mprotect)(void* addr, size_t length, int, int ctx); // optional
  int (*uimg_w_malloc)(size_t, void**, void**); // optional
  void (*uimg_w_free)(void* addr); // optional
  int (*uimg_w_mprotect)(void* addr, size_t length, int, int ctx); // optional
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
