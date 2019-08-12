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
#ifndef __ARCH_OPS_H
#define __ARCH_OPS_H

#ifndef ASSEMBLY

#include "lktypes.h"
#include "lkcompiler.h"

#if defined(__cplusplus)
extern "C" {
#endif

void arch_enable_ints(void);
void arch_disable_ints(void);

int atomic_swap(volatile int *ptr, int val);
int atomic_add(volatile int *ptr, int val);
int atomic_and(volatile int *ptr, int val);
int atomic_or(volatile int *ptr, int val);

#endif // !ASSEMBLY

void arch_idle(void);



#if defined(__cplusplus)
}
#endif

#endif // !ASSEMBLY

