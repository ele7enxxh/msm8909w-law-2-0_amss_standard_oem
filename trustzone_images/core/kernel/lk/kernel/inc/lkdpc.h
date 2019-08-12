/*
 * Copyright (c) 2008 Travis Geiselbrecht
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#ifndef __KERNEL_DPC_H
#define __KERNEL_DPC_H

#include "lklist.h"
/*#include <sys/types.h>*/

void dpc_init(void);

typedef uint32_t (*dpc_callback)(void *arg, void *arg2, uint32_t arg3);

#define DPC_FLAG_NORESCHED 0x1
#define DPC_FLAG_RESCHED 0x2

struct dpc {
  struct list_node node;	

  dpc_callback cb;
  void *arg;
  void *arg2;
  uint32_t arg3;
};

status_t dpc_queue(dpc_callback, void *arg, uint32_t flags, struct dpc *dpc, void * arg2, uint32_t arg3);

#endif

