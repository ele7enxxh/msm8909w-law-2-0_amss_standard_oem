/** @addtogroup DRAPI_COM
 * @{
 *
 * @file DrApiCommon.h
 * @brief  Secure Driver API Common Definitions
 *
 * Copyright (c) 2013 TRUSTONIC LIMITED
 * All rights reserved
 * 
 * The present software is the confidential and proprietary information of
 * TRUSTONIC LIMITED. You shall not disclose the present software and shall
 * use it only in accordance with the terms of the license agreement you
 * entered into with TRUSTONIC LIMITED. This software may be subject to
 * export or import laws in certain countries.
 */

#ifndef __DRAPICOMMON_H__
#define __DRAPICOMMON_H__

#include "DrApi/DrApiError.h"

#ifdef __cplusplus
extern "C" {
#endif

//------------------------------------------------------------------------------
/** Definitions */

#define SHIFT_4KB               (12U) /**<  SIZE_4KB is 1 << SHIFT_4KB aka. 2^SHIFT_4KB. */
#define SIZE_4KB                (1 << SHIFT_4KB) /**< Size of 1 KiB. */
typedef uint8_t                 page4KB_t[SIZE_4KB]; /**< 4 KiB page. */
typedef page4KB_t               *page4KB_ptr; /**< pointer to 4 KiB page. */

#define PTR2VAL(p)   			((uintptr_t)(p))
#define VAL2PTR(v)   			((addr_t)(v))
#define FUNC_PTR(func)			VAL2PTR( PTR2VAL( func ) )

//Stack types
typedef uint32_t        stackEntry_t;
typedef stackEntry_t    *stackEntry_ptr;
typedef stackEntry_ptr  stackTop_ptr;

//==============================================================================
//Common integer types
typedef unsigned int	u32_t;
typedef unsigned short	u16_t;
typedef unsigned char	u08_t;
typedef u32_t			word_t;

//Common result type
typedef word_t drApiResult_t;

//MTK types
typedef word_t  taskid_t,   *taskid_ptr;     /**< task id data type. */
typedef word_t  threadno_t, *threadno_ptr;   /**< thread no. data type. */
typedef word_t  threadid_t, *threadid_ptr;   /**< thread id data type. */


// interrupt mode flags. The design of the bits is that most common setting
// RISING, EDGE, PERIODIC maps to the value the value 0. Not all mode
// combinations may be available for each interrupt.
#define INTR_MODE_MASK_TRIGGER          	(1U<<0)
#define INTR_MODE_TRIGGER_LEVEL           	INTR_MODE_MASK_TRIGGER
#define INTR_MODE_TRIGGER_EDGE            	0
#define INTR_MODE_MASK_CONDITION        	(1U<<1)
#define INTR_MODE_CONDITION_FALLING       	INTR_MODE_MASK_CONDITION
#define INTR_MODE_CONDITION_LOW           	INTR_MODE_MASK_CONDITION
#define INTR_MODE_CONDITION_RISING        	0
#define INTR_MODE_CONDITION_HIGH          	0
#define INTR_MODE_MASK_OCCURANCE        	(1U<<2)
#define INTR_MODE_OCCURANCE_ONESHOT       	INTR_MODE_MASK_OCCURANCE
#define INTR_MODE_OCCURANCE_PERIODIC      	0

// convenience constants
#define INTR_MODE_RAISING_EDGE              (INTR_MODE_TRIGGER_EDGE | INTR_MODE_CONDITION_RISING)
#define INTR_MODE_FALLING_EDGE              (INTR_MODE_TRIGGER_EDGE | INTR_MODE_CONDITION_FALLING)
#define INTR_MODE_LOW_LEVEL                 (INTR_MODE_TRIGGER_LEVEL | INTR_MODE_CONDITION_LOW)
#define INTR_MODE_HIGH_LEVEL                (INTR_MODE_TRIGGER_LEVEL | INTR_MODE_CONDITION_HIGH)

//Interrupt types
typedef word_t  intrNo_t, *intrNo_ptr;      /**< interrupt number. */
typedef word_t  intrMode_t, *intrMode_ptr;  /**< interrupt mode. */

#ifdef __cplusplus
}
#endif

#endif //__DRAPICOMMON_H__

/** @} */
