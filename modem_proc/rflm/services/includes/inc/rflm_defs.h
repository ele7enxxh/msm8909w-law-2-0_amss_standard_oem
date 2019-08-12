/*!
  @file
  rflm_defs.h

  @brief
  Definitions and macros for RFLM.

  @detail
  This file must contain only #defines so it can be included from assembly code.
  This file should not be included standalone in C code - use rflm.h instead,
  which includes this file.

*/

/*===========================================================================

  Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rflm/services/includes/inc/rflm_defs.h#1 $

when         who     what, where, why
----------   ---     --------------------------------------------------------
2014-08-04   jc      Avoid BITMASK redefinition warning
2013-08-23   jc      Changes to allow windows QTF compilation
2013-08-06   jyw     Removed string.h and stringl.h to the rflm.h
2013-07-26   hdz     Added string.h and stringl.h
2013-07-18   JC      Port from modem_fw_defs.h

===========================================================================*/

/* This file should only be included as part of modem_fw.h. Enforce the 
   order (it's important that this be before the #define MODEM_FW_DEFS_H).
   When including from assembly code, this file is standalone. */
#ifndef __ASSEMBLER__
#include "rflm.h"
#endif

// To avoid duplicate type definitions (comdef.h in fw standalone build
// includes modem_fw_defs.h)
#ifndef MODEM_FW_DEFS_H
#define MODEM_FW_DEFS_H

#ifndef __ASSEMBLER__
#include "comdef.h"
#endif

/*! @brief Defines so Lint recognizes keywords.
*/
/*lint -cgnu 
       -rw_asgn(__inline,inline)
       -rw_asgn(__inline__,inline)
       -rw_asgn( __volatile, volatile )
       -rw_asgn( __volatile__, volatile )
       +rw(_to_semi) 
       -dasm=_to_semi 
       -d__asm__=_to_semi 
*/


/*===========================================================================

                   CONSTANTS

===========================================================================*/

/*! @brief Define some aliases for the Hexagon version (v1=1, v2=2, etc.).
    If undefined, not Hexagon. 
*/
#ifndef __HEXAGON_ARCH__
#ifndef __QDSP6_ARCH__

#ifdef TEST_FRAMEWORK
  #error code not present
#else
  #error "FW header files only compile on Hexagon"
#endif  
#else
  #define __HEXAGON_ARCH__ __QDSP6_ARCH__
#endif
#endif

#define HEXAGON_ARCH      __HEXAGON_ARCH__   
#define HEXAGON_VER       __HEXAGON_ARCH__

/*! @brief Number of HW threads (specifically, the number available to FW).
*/
#if ((HEXAGON_ARCH >= 4) && (HEXAGON_ARCH <= 5))
#define FW_NUM_HW_THREADS   3
#elif (HEXAGON_ARCH == 55)
#define FW_NUM_HW_THREADS 4
#else

#ifdef TEST_FRAMEWORK
#error code not present
#else
#error "Unsupported Hexagon version"
#endif

#endif

/*! @brief Size of a data cache line, in bytes.
*/
#define CACHE_LINE_SZ       32
 

/*! @brief Boolean flags.
*/
#define TRUE                1
#define FALSE               0


/*! @brief NULL definition if nothing has defined it yet.
*/
#ifndef NULL
  #define NULL              0
#endif


/*===========================================================================

                   UTILITY MACROS

  Expressions evaluated at compile time.

===========================================================================*/


/*! @brief Compile-time assertion. 
    The compiler will generate a "negative array index" error if the assertion
    fails. 
    Only use this for constants known at compile time. 
    e.g. 
    COMPILE_ASSERT(sizeof(mystruct) == 12);
*/
#define COMPILE_ASSERT(expr) \
  /*lint -save -e18 -e19 -e506 */ \
  extern char _SYM_CONCAT(_SYM_CONCAT(N,__COUNTER__),_SYM_CONCAT(_THIS_ERROR_MEANS_A_COMPILE_ASSERT_FAILURE_AT_LINE_,__LINE__))[ (expr) ? 1 : -1 ] __attribute__((unused)); \
  /*lint -restore */
#define _SYM_CONCAT(a,b) _SYM_CONCAT_2(a,b)
#define _SYM_CONCAT_2(a,b) a##b
 

/*! @brief Lint-only assertion.
    This macro creates an assertion only for the lint pass, and is never 
    included in compiled code. It is useful for providing information
    that lint can't infer on its own.
    e.g.
    LINT_ASSERT(x < 10);
    my_array[x] = 0;  // Generates an "out of range array access" otherwise
*/
#ifdef _lint
#define LINT_ASSERT(expr) if (!(expr)) { _lint_assert_fail(); } 
/*lint -function(exit,_lint_assert_fail) */
extern void _lint_assert_fail(void);
#else
#define LINT_ASSERT(expr)
#endif


/*! @brief Create a false access to a variable to suppress "unused" lint warnings.
    This is similar to the UNUSED macro, but does not need to be added to the
    declaration.
    e.g.
    void foo(int x) {
      NOTUSED(x);
    }
*/
#ifndef NOTUSED
#define NOTUSED(x)          if (x) { }
#endif

/*! @brief Compiler hint about the value of expression. expr should be 
    the full expression, and hint a true/false value. For example:
    e.g. 
    if (EXPECT(ptr == NULL, FALSE))
      fatal_error();
*/
#ifdef _lint
#define EXPECT(expr, hint) ( expr )
#else
#define EXPECT(expr, hint)  __builtin_expect( (expr), (hint) )
#endif


/*! @brief Get the offset of "field" within structure/union "type".
    e.g.
    typedef struct {
      int32 a;
      int32 b;
    } my_struct;
    OFFSETOF(my_struct, b)  returns 4.
*/
#ifndef _lint
#define OFFSETOF(type, field) __builtin_offsetof(type, field)
#else
/*lint -emacro((413,545),OFFSETOF) */
#define OFFSETOF(type, field) \
  ( (unsigned int) &( ((type *) 0)->field ) ) 
#endif

#ifndef FPOS
#define FPOS(type, field)   OFFSETOF(type, field)
#endif
/*! @brief Return the number of elements in an array. 
    Use only with a true array, not with a pointer, or the result will not
    be correct.
    e.g.
    int x[14];
    y = ARR_SIZE(x);  // y = 14.
*/
#ifndef ARR_SIZE
#define ARR_SIZE(a)         ( sizeof(a) / sizeof(a[0]) )
#endif

/*! @brief Translate an address to a different address space (for example,
    from virtual to physical space).
    "addr" is relative to "old_base", in the same address space.
    The offset from "old_base" to "addr" is added to "new_base", effectively
    translating into the new address space. 
*/
#define ADDR_TRANSLATE(addr, old_base, new_base) ( ((uint32) (addr)) - ((uint32) (old_base)) + ((uint32) (new_base)) )


/*! @brief Return the maximum/minimum of two arguments. 
    This macro should only be used when the arguments are "untyped", e.g. 
    when used from another macro, or when comparing pointers.
    In other cases, use the maxw(), minw() etc. macros instead - they are more efficient. 
*/
#define MAX(x, y)           ( ((x) > (y)) ? (x) : (y) )
#define MIN(x, y)           ( ((x) < (y)) ? (x) : (y) )

 
/*! @brief Bit at position "n", n < 32 . */
#define BIT(n)   (1U << (uint32)(n))

/*! @brief Bit at position "n", n < 64 .*/
#define BITL(n)  (1ULL << (uint32)(n))

/*! @brief Bitmask containing "n" bits, 0 <= n <= 32 */
#ifndef BITMASK
#define BITMASK(n) ( /*lint -save -e506 */ \
                     (n) >= 32 ? MAX_UINT32 : BIT((n) & 0x1F) - 1 \
                     /*lint -restore */ )
#endif                     

/*! @brief Bitmask containing "n" bits, 0 <= n <= 64 */
#define BITMASKL(n) ( /*lint -save -e506 */ \
                      (n) >= 64 ? MAX_UINT64 : BITL((n) & 0x3F) - 1 \
                      /*lint -restore */ )


/*! @brief Quantize "n" to the next-lowest multiple of "q" .
    Result is <= n, (result % q) == 0. 
    Defined for positive inputs only.
    Use for constants only, not efficient for runtime (unless q is power of 2).
*/
#define QUANTIZE_DOWN(n,q) ( ( (n) / (q) ) * (q) )


/*! @brief Quantize "n" to the next-highest multiple of "q".
    Result is >= n, (result % q) == 0.
    Defined for positive inputs only. 
    Use for constants only, not efficient for runtime (unless q is power of 2).
*/
#define QUANTIZE_UP(n,q) ( QUANTIZE_DOWN( ( (n) + (q) - 1 ), (q) ) )


/*! @brief Return 1 if x is a power of 2, 0 otherwise.
    Efficient to be used at runtime.
*/
#define IS_POW2(x)       ( ( (x) & ( (x) - 1 ) ) == 0 )


/*! @brief Return the highest power-of-2 <= x. 
    x must be unsigned 32-bits.
    Use for constants only, not efficient for runtime.
    Algorithn (from "Hacker's Delight", Henry S. Warren Jr.):
    uint32 FLOOR_POW2(uint32 x) {
       x = x | (x >> 1);
       x = x | (x >> 2);
       x = x | (x >> 4);
       x = x | (x >> 8);
       x = x | (x >> 16);
       return x - (x >> 1);
    }
*/
#define FLOOR_POW2(x) ( /*lint -save -e572 */ \
                        FLOOR_POW2_1_(x) - (FLOOR_POW2_1_(x)>>1) \
                        /*lint -restore */ )
#define FLOOR_POW2_1_(x) (FLOOR_POW2_2_(x)|(FLOOR_POW2_2_(x)>>16))
#define FLOOR_POW2_2_(x) (FLOOR_POW2_3_(x)|(FLOOR_POW2_3_(x)>>8))
#define FLOOR_POW2_3_(x) (FLOOR_POW2_4_(x)|(FLOOR_POW2_4_(x)>>4))
#define FLOOR_POW2_4_(x) (FLOOR_POW2_5_(x)|(FLOOR_POW2_5_(x)>>2))
#define FLOOR_POW2_5_(x) ((x)|((x)>>1))


/*! @brief Return the lowest power-of-2 >= x.
    x must be unsigned 32-bits.
    Use for constants only, not efficient for runtime.
    Algorithn (from "Hacker's Delight", Henry S. Warren Jr.):
    uint32 CEIL_POW2(uint32 x) {
       x = x - 1;
       x = x | (x >> 1);
       x = x | (x >> 2);
       x = x | (x >> 4);
       x = x | (x >> 8);
       x = x | (x >> 16);
       return x + 1;
    }
*/
#define CEIL_POW2(x) ( /*lint -save -e572 */ \
                       CEIL_POW2_1_(x) + 1  \
                       /*lint -restore */ )
#define CEIL_POW2_1_(x) (CEIL_POW2_2_(x)|(CEIL_POW2_2_(x)>>16))
#define CEIL_POW2_2_(x) (CEIL_POW2_3_(x)|(CEIL_POW2_3_(x)>>8))
#define CEIL_POW2_3_(x) (CEIL_POW2_4_(x)|(CEIL_POW2_4_(x)>>4))
#define CEIL_POW2_4_(x) (CEIL_POW2_5_(x)|(CEIL_POW2_5_(x)>>2))
#define CEIL_POW2_5_(x) (CEIL_POW2_6_(x)|(CEIL_POW2_6_(x)>>1))
#define CEIL_POW2_6_(x) ((x)-1)


/*! @brief Population count, i.e. number of bits set in a word.
    x must be unsigned 32-bits.
    Result is from 0 (for input=0) to 32 (for input=0xFFFFFFFF).
    Use for constants only, not efficient at runtime.
    Algorithn (from "Hacker's Delight", Henry S. Warren Jr.):
    int POPCOUNT(uint32 x) {
       x = (x & 0x55555555) + ((x >> 1) & 0x55555555);
       x = (x & 0x33333333) + ((x >> 2) & 0x33333333);
       x = (x & 0x0F0F0F0F) + ((x >> 4) & 0x0F0F0F0F);
       x = (x & 0x00FF00FF) + ((x >> 8) & 0x00FF00FF);
       x = (x & 0x0000FFFF) + ((x >> 16) & 0x0000FFFF);
     }
*/
#define POPCOUNT(x) ( /*lint -save -e572 */ \
                      (POPCOUNT_1_(x)&0x0000FFFF)+\
                      ((POPCOUNT_1_(x)>>16)&0x0000FFFF)\
                      /*lint -restore */ )
#define POPCOUNT_1_(x) ((POPCOUNT_2_(x)&0x00FF00FF)+\
                        ((POPCOUNT_2_(x)>>8)&0x00FF00FF))
#define POPCOUNT_2_(x) ((POPCOUNT_3_(x)&0x0F0F0F0F)+\
                        ((POPCOUNT_3_(x)>>4)&0x0F0F0F0F))
#define POPCOUNT_3_(x) ((POPCOUNT_4_(x)&0x33333333)+\
                        ((POPCOUNT_4_(x)>>2)&0x33333333))
#define POPCOUNT_4_(x) (((x)&0x55555555)+\
                        (((x)>>1)&0x55555555))


/*! @brief Count leading zeros in "x".
    x must be unsigned 32-bits. 
    Result is from 0 (for input=0xFFFFFFFF) to 32 (for input=0).
    Use for constants only, not efficient at runtime.
    Algorithn (from "Hacker's Delight", Henry S. Warren Jr.):
    int CLZ(uint32 x) {
       x = x | (x >> 1);
       x = x | (x >> 2);
       x = x | (x >> 4);
       x = x | (x >> 8);
       x = x | (x >> 16);
       // return POPCOUNT(~x) changed to avoid "integer overflow" warning
       return 32 - POPCOUNT(x);
    }
*/
#define CLZ(x) ( /*lint -save -e572 */ \
                 32 - POPCOUNT(CLZ_1_(x))  \
                 /*lint -restore */ )
#define CLZ_1_(x) (CLZ_2_(x)|(CLZ_2_(x)>>16))
#define CLZ_2_(x) (CLZ_3_(x)|(CLZ_3_(x)>>8))
#define CLZ_3_(x) (CLZ_4_(x)|(CLZ_4_(x)>>4))
#define CLZ_4_(x) (CLZ_5_(x)|(CLZ_5_(x)>>2))
#define CLZ_5_(x) ((x)|((x)>>1))


/*! @brief Calculate the largest n such that 2^n <= x.
    x must be unsigned 32-bits, x >= 1. 
    Use for constants only, not efficient at runtime.
*/
#define FLOOR_LOG2(x) ( 31 - CLZ(x) )


/*! @brief Calculate the smallest n such that 2^n >= x.
    x must be unsigned 32-bits, x >= 1. 
    Use for constants only, not efficient at runtime.
*/
#define CEIL_LOG2(x) ( FLOOR_LOG2( (x) - 1 ) + 1 )


/*! @brief Calculate the required "K" parameter for buffer length "len" bytes
    for Hexagon circular addressing.
    K is the minimum that satisfies (n < 2^(K+2) ). 
    n must be >= 3.
    Use for constants only, not efficient at runtime.
*/
#define CIRC_K(len) ( CEIL_LOG2( (len) + 1 ) - 2)


/*! @brief Alignment directive of a buffer of length "len" for circular 
    addressing.
    e.g.
    char myBuf[24000] CIRC_ALIGN(24000);
*/
#define CIRC_ALIGN(len) ALIGN( CEIL_POW2( (len) + 1 ) )


/*! @brief Build the high/low 16 bits or the entire M register to initialize Hexagon
    circular addressing. 
    len is the length of the buffer in bytes, must be >= 3.
    inc is the "I" post-update increment. Set to 0 if not using post-update by I.
*/
#define CIRC_M_HI(len,inc) ( /*lint -save -e572 -e648 */ \
                             ( CIRC_K(len) << (24-16) ) | \
                             ( ( (inc) & 0x7F ) << (17-16) ) | \
                             ( ( (inc) & 0x780 ) << (28-7-16) ) | \
                             ( ( (len) & 0x10000 ) >> 16 ) \
                             /*lint -restore */ )
#define CIRC_M_LO(len,inc) ( (len) & 0xFFFF )
#define CIRC_M(len,inc) ( /*lint -save -e648 */ \
                          ( CIRC_M_HI(len,inc) << 16 ) | CIRC_M_LO(len,inc) \
                          /*lint -restore */ )



/*===========================================================================

                   MACROS FOR DECLARATIONS

  These macros are tags to apply to function or variable declarations.

===========================================================================*/

/*! @brief Force a variable/type to the specified alignment, in bytes.
     e.g.
     int x[10] ALIGN(128);  // x starts at 128-byte boundary
     typedef struct { 
        int x;
        int y ALIGN(16);
        ... } mystruct ALIGN(64); 
      // Whole struct is 64-byte aligned, padding inserted between x and y
      // to make y 16-byte aligned.
*/
#ifndef ALIGN
#define ALIGN(n)            __attribute__((aligned(n)))
#endif

/*! @brief Directive to cache-align a variable or struct (use like "ALIGN").
*/
#define CACHE_ALIGN         ALIGN(CACHE_LINE_SZ)


/*! @brief PACK(struct) or PACK(union): 
      Pack the following struct/union so there is no padding between elements. 
      Use this only when memory is absolutely critical (like large arrays of 
      odd-sized structures), as packed structures have a high runtime overhead.
      e.g.
      typedef PACK(struct) { uint8 x; uint32 y; } mystruct; 
      // Normally 3 padding bytes would be inserted to make y 4-byte aligned, so
      // sizeof(mystruct)==8. With PACK, sizeof(struct)==5. 
*/
#ifndef PACK
#define PACK(x)             x __attribute__((packed))
#endif

/*! @brief Attribute to declare inline functions.
    Redefine this to force always inline, which is required if functions
    are declared in header files.
*/
#ifdef TEST_FRAMEWORK
#error code not present
#else

#ifdef _lint
#define STATIC_INLINE       inline
#else
#define STATIC_INLINE       static inline __attribute__((always_inline))
#endif

#endif

#undef INLINE
#define INLINE              STATIC_INLINE
#define FW_INLINE           FW_INLINE


/*! @brief Used to declare inline class methods, where "static" has other
    connotations.
*/
#define CLASS_INLINE        inline __attribute__((always_inline))


/*! @brief Attribute to force a function to not be auto-inlined.
    e.g. NOINLINE uint32 my_func(void) { ... }
*/
#define NOINLINE            __attribute__((noinline))


/*! @brief Restrict pointer aliasing. When applied to a pointer type, tells
    the compiler that pointer references a unique location not accessible
    through other means. This is typically useful for buffer processing where
    the read/write pointers are distinct (without RESTRICT, the compiler must 
    assume that the write for element [k] must be done before the read for [k+1]).
    RESTRICT is only relevant for pointers that will be used for writes.
    e.g.
    void myfunc(short int *a, short int *b, short int * RESTRICT c) {
       for (i=0; i < 100; ++i) {
          *c++ = *a++ - *b++;
       }
    }
    Be very careful that this is applied accurately, or the compiler may 
    generate incorrect code.
*/
#define RESTRICT            __restrict


/*! @brief Mark a function as "static" in normal builds, but export
    in unit tests.
    FOR USE IN FUNCTION DECLARATIONS ONLY.
*/
#ifndef STATIC
#ifdef GLOBALIZE_STATIC
  #define STATIC
#else
  #define STATIC            static
#endif
#endif

/*! @brief Attribute to suppress compiler warnings for unused variables.
    This is used for declarations; for suppressing within the body of a function,
    see NOTUSED().
    e.g. 
    void foo(int arg UNUSED) { ... }
    int x UNUSED;
*/
#define UNUSED              __attribute__((unused))


/*! @brief Attribute to mark a function, variable, or typedef as deprecated.
    Use of the symbol will generate a compiler warning.
    Note that the tag must be applied to the external declaration in order
    to have an effect.
    e.g. extern void func(int) DEPRECATED;
         extern int x DEPRECATED;
         typedef int my_type DEPRECATED;
*/
#define DEPRECATED          __attribute__((deprecated))

/*! @brief Attribute to force a variable or function into a particular section. 
    Name must be a quoted string.
    e.g. void my_func(void) SECTION(".tcm_text");
         int my_var SECTION(".data") = 0;
    Separate SECTION_CODE and SECTION_DATA are necessary for assembly to set
    attributes correctly. 
*/
#define SECTION(name)       __attribute__((section(name)))
#ifndef __ASSEMBLER__
#define SECTION_CODE(name)  SECTION(name)
#define SECTION_DATA(name)  SECTION(name)
#else
#define SECTION_CODE(name)  .section name, "ax"
#define SECTION_DATA(name)  .section name, "aw"
#endif



#endif /* RFLM_DEFS_H */

