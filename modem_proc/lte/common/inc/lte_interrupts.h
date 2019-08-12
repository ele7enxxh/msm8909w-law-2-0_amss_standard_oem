/*!
  @file
  lte_interrupts.h

  @brief
  LTE interrupt numbers and priorities

  @detail
  This file specifies the priority and number of all interrupts used by
  LTE lower layer SW.
 
*/

/*===========================================================================

  Copyright (c) 2007 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/common/inc/lte_interrupts.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
===========================================================================*/

#ifndef LTE_INTERRUPTS_H
#define LTE_INTERRUPTS_H


/*===========================================================================

      Constants

===========================================================================*/


/*===========================================================================

      Macros

===========================================================================*/


/*===========================================================================

      Typedefs

===========================================================================*/

/*! @brief LTE interrupt priorities

    This is a list of priorities for all LTE lower layer SW interrupts.
    Priorites should range between QTHREAD_MIN_PRIORITY (2)
    and QTHREAD_MAX_PRIORITY (250)
    
    @see lte_thread_prio_enum
    
    @todo add interrupts
*/
typedef enum
{
  LTE_INT_PRIO_SEARCH_DONE = 103,   ///< search done interrupt priority, 
                                    ///< need to update
                                    
  LTE_INT_PRIO_RTC         = 220,   ///< RTC interrupt priority
  LTE_INT_PRIO_DECODER     = 210,   ///< Decoder interrupt priority
  LTE_INT_PRIO_PILOT_XFER  = 215,   ///< pilot transfer interrupt priority
  LTE_INT_PRIO_DM          = 210,   ///< DM interrupt priority  
                                     
  LTE_INT_PRIO_BDEC        = 200,   ///< BDEC interrupt priority
  LTE_INT_PRIO_CHAN_EST    = 218,   ///< Channel Estimation interrupt priority
  LTE_INT_PRIO_DEMOD       = 215,   ///< DEMOD interrupt priority 
  LTE_INT_PRIO_VDEC        = 215,   ///< VDEC interrupt priority 
  LTE_INT_PRIO_TDEC        = 215,   ///< TDEC interrupt priority 
  LTE_INT_PRIO_TXHWCTL     = 215,
  LTE_INT_PRIO_FFT         = 200,    ///< FFT interrupt priority 
  LTE_INT_PRIO_AXI_EMU     = 230
} lte_interrupt_prio_enum;


/*! @brief LTE interrupt numbers
    This is a list all LTE lower layer SW interrupts numbers.    

    *NOTE* don't use 20-21 on Raptor2 - it maps to an AMR9 FIQ that is fired
*/
typedef enum
{
#include "lte_variation.h"
  #if defined(T_RUMI_9X00) 
  #if defined (FEATURE_LTE_PLT_T32)
  LTE_INT_NUM_T32         = 22,
  #endif /* FEATURE_LTE_PLT_T32 */
  #endif /* T_RUMI_9X00 */

  LTE_INT_NUM_TXHWCTL     = 16,     ///< TX HW CTL interrupt #: need to change

  LTE_INT_NUM_RTC_0_7     = 30+26*32,    ///< RTC interrupt #

  LTE_INT_NUM_8             = 8,
  LTE_INT_NUM_SW2FWQ6_1     = 8 + 28*32,  

  LTE_INT_NUM_TASK_QUEUE_0  = 12,
  LTE_INT_NUM_TASK_QUEUE_1  = 19,
  LTE_INT_NUM_TASK_QUEUE_2  = 20,
  LTE_INT_NUM_TASK_QUEUE_3  = 21,
  
  LTE_INT_NUM_TASK_QUEUE_4  = 3+24*32,
  LTE_INT_NUM_TASK_QUEUE_5  = 4+24*32,
  LTE_INT_NUM_TASK_QUEUE_6  = 5+24*32,
  LTE_INT_NUM_TASK_QUEUE_7  = 6+24*32,
  LTE_INT_NUM_TASK_QUEUE_8  = 7+24*32,
  LTE_INT_NUM_TASK_QUEUE_9  = 8+24*32,

  LTE_INT_NUM_SEARCH_DONE = LTE_INT_NUM_TASK_QUEUE_1  ///< search done interrupt

} lte_interrupt_num_enum;


#define LTE_INT_NUM_TASK_QUEUE( TQ_NUM ) LTE_INT_NUM_TASK_QUEUE_( TQ_NUM )
#define LTE_INT_NUM_TASK_QUEUE_( TQ_NUM ) LTE_INT_NUM_TASK_QUEUE_##TQ_NUM


/*===========================================================================

      Functions

===========================================================================*/



#endif /* LTE_INTERRUPTS_H */
