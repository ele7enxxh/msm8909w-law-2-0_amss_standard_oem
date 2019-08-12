/**
 * Common function for all TlApi Header files.
 * @file
 * Copyright (c) 2013 TRUSTONIC LIMITED
 * All rights reserved
 * 
 * The present software is the confidential and proprietary information of
 * TRUSTONIC LIMITED. You shall not disclose the present software and shall
 * use it only in accordance with the terms of the license agreement you
 * entered into with TRUSTONIC LIMITED. This software may be subject to
 * export or import laws in certain countries.
 */
#ifndef __TLAPICOMMON_H__
#define __TLAPICOMMON_H__

//=============================================================================
// Compiler settings
//=============================================================================

// _TLAPI_EXTERN_C tells the C++ compiler, that this function is a plain C
// function. It has no effect when using a plain C compiler. Usually, this is
// defined to 'extern "C"' in most compilers. define it to nothing if you
// compiler does no support it.
#if !defined(_TLAPI_EXTERN_C)
    #error "_TLAPI_EXTERN_C is not defined."
#endif


// _TLAPI_NORETURN tells the compiler, that this function will not return. This
// allows further optimization of the code. For GCC and ARMCC this defines to
// '__attribute__((noreturn))'. Define it to nothing if your compiler does not
// support it.
#if !defined(_TLAPI_NORETURN)
    #error "_TLAPI_NORETURN is not defined."
#endif


//=============================================================================
// TlApi constants
//=============================================================================

#define TLAPI_INFINITE_TIMEOUT  ((uint32_t)(-1))    /**< Wait infinite for a response of the MC. */
#define TLAPI_NO_TIMEOUT        0                   /**< Do not wait for a response of the MC. */


//=============================================================================
// TlApi working buffer
//=============================================================================

#define MCLIB_WB_SIZE 16
extern uint32_t _mcLibWorkArea[MCLIB_WB_SIZE];

#endif // __TLAPICOMMON_H__


