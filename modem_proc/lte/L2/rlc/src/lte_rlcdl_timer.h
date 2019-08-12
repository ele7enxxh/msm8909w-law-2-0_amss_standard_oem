/*!
  @file
  lte_rlcdl_timer.h

  @brief
  defintions of timer related functions for RLCDL.

  @author
  axiao

*/

/*==============================================================================

  Copyright (c) 2009 Qualcomm Technologies Incorporated. All Rights Reserved

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

==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/L2/rlc/src/lte_rlcdl_timer.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
01/31/16   cx      CR923654: Fast nak optimization
04/15/15   ru      CR816655: RLC mitigation to prevent RLF due to FC Shutdown
10/01/14   ru      CR732514: Change mcs timer APIs to core timer APIs
01/31/14   mg      CR606601: Extend '0xB097 LTE RLC UL Statistics' log packet to
                   include number of UM RLC PDUs missed 
01/23/14   mg      CR599494: RLC behavior to alleviate the low DSM count problem.
10/02/09   ax      fix CR212348: make status prohibit timer and mask reentrant 
03/12/09   ax      rename timer_ptr to rlcdl_timer_ptr to avoid shadowing global
                   #define in timer.h
01/06/09   ax      incorporate code review comments
09/26/08   ax      initial version
==============================================================================*/

#ifndef LTE_RLCDL_TIMER_H
#define LTE_RLCDL_TIMER_H

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

#include <comdef.h>
#include <lte_assert.h>
#include "lte_rlcdli.h"
/*lint -e613*/
/*lint -e528*/


/*==============================================================================

                   EXTERNAL DEFINITIONS AND TYPES

==============================================================================*/
/*! @brief RLCDL timer periodicity in ms
*/
#define LTE_RLCDL_TIMER_PERIODICITY                                 5

/*==============================================================================

                    EXTERNAL FUNCTION PROTOTYPES

==============================================================================*/
extern void lte_rlcdl_timer_init( void );

extern void lte_rlcdl_timer_terminate( void );

extern void lte_rlcdl_timer_start( void );

extern void lte_rlcdl_timer_stop( void );

extern void lte_rlcdl_timer_expiry_um_proc
(
  lte_rlcdli_ctrl_blk_ptr       ctrl_blk_ptr
);

extern void lte_rlcdl_timer_expiry_am_proc
(
  lte_rlcdli_ctrl_blk_ptr       ctrl_blk_ptr
);
/*===========================================================================

                         INLINE FUNCTIONS

===========================================================================*/

/*==============================================================================

  FUNCTION:  lte_rlcdl_timer_reordering_expired

==============================================================================*/
/*!
    @brief
    Is T_reordering timer expired.

    @return
    TRUE if the timer has expired. FALSE otherwise.
*/
/*============================================================================*/
static inline boolean lte_rlcdl_timer_reordering_expired
(
  lte_rlcdli_timer_s*         rlcdl_timer_ptr,   /*!< ptr to the timer struct */
  uint16                   timer_val
)
{
  boolean      expired = FALSE;
  /*--------------------------------------------------------------------------*/

  /*--------------------------------------------------------------------------*/
  if(rlcdl_timer_ptr->reordering_counter_ptr != NULL)
  {
    rlcdl_timer_ptr->reordering_counter += LTE_RLCDL_TIMER_PERIODICITY;
    if(rlcdl_timer_ptr->reordering_counter >= timer_val)
    {
      expired = TRUE;
    }
  }
  return expired;
} /* lte_rlcdl_timer_reordering_expired() */

/*==============================================================================

  FUNCTION:  lte_rlcdl_timer_reordering_running

==============================================================================*/
/*!
    @brief
    Is T_reordering timer running.

    @return
    TRUE if the timer is running. FALSE otherwise.
*/
/*============================================================================*/
static inline boolean lte_rlcdl_timer_reordering_running
(
  const lte_rlcdli_timer_s*     rlcdl_timer_ptr /*!< ptr to the timer struct */
)
{
  /*--------------------------------------------------------------------------*/

  /*--------------------------------------------------------------------------*/
  return (rlcdl_timer_ptr->reordering_counter_ptr != NULL);
} /* lte_rlcdl_timer_reordering_running() */

/*==============================================================================

  FUNCTION:  lte_rlcdl_timer_reordering_start

==============================================================================*/
/*!
    @brief
    start T_reordering timer.

    @note
    This API can be called with two possible timer_value arguments.
    timer_value(0) = restarts the reordering timer
    timer_value(cfg.t_reordering) = forces the reordering timer to expire

    @return
    None.
*/
/*============================================================================*/
static inline void lte_rlcdl_timer_reordering_start
(
  lte_rlcdli_ctrl_blk_ptr       ctrl_blk_ptr,   /*!< ptr to the control block */
  uint16                        timer_value  /*!< init timer count */
)
{
  /*--------------------------------------------------------------------------*/

  /*--------------------------------------------------------------------------*/
  ctrl_blk_ptr->timers.reordering_counter = (int16)timer_value;
  ctrl_blk_ptr->timers.reordering_counter_ptr = 
    &(ctrl_blk_ptr->timers.reordering_counter);
} /* lte_rlcdl_timer_reordering_start() */

/*==============================================================================

  FUNCTION:  lte_rlcdl_timer_reordering_stop

==============================================================================*/
/*!
    @brief
    stop T_reordering timer.

    @return
    None.
*/
/*============================================================================*/
static inline void lte_rlcdl_timer_reordering_stop
(
  lte_rlcdli_timer_s*           rlcdl_timer_ptr  /*!< ptr to the timer struct */
)
{
  /*--------------------------------------------------------------------------*/

  /*--------------------------------------------------------------------------*/
  rlcdl_timer_ptr->reordering_counter_ptr = NULL;
} /* lte_rlcdl_timer_reordering_stop() */

/*==============================================================================

  FUNCTION:  lte_rlcdl_timer_status_prohibit_start

==============================================================================*/
/*!
    @brief
    start T_status_prohibit timer and also clear the status prohibit mask.

    @note
    this function is executed under the MAC_UL task context. 
    Any change made here needs to be reviewed to ensure it is reentrant.

    @see related_function()
    lte_rlcdl_nak_build_status()

    @return
    None.
*/
/*============================================================================*/
static inline void lte_rlcdl_timer_status_prohibit_start
(
  lte_rlcdli_ctrl_blk_ptr       ctrl_blk_ptr,  /*!< ptr to control block */
  uint16                        time_value     /*!< init timer count */
)
{
  /*--------------------------------------------------------------------------*/

  /*--------------------------------------------------------------------------*/
  ctrl_blk_ptr->timers.status_prohibit_counter = time_value;
  ctrl_blk_ptr->status.status_prohibit_mask = LTE_RLCDLI_STATUS_PROHIBIT_MASK;

} /* lte_rlcdl_timer_status_prohibit_start() */

/*==============================================================================

  FUNCTION:  lte_rlcdl_timer_status_prohibit_stop

==============================================================================*/
/*!
    @brief
    stop the status prohibit timer

    @return
    None.
*/
/*============================================================================*/
static inline void lte_rlcdl_timer_status_prohibit_stop
(
  const lte_rlcdli_ctrl_blk_ptr       ctrl_blk_ptr  /*!< ptr to control block */
)
{
  /*--------------------------------------------------------------------------*/

  /*--------------------------------------------------------------------------*/

  ctrl_blk_ptr->status.status_prohibit_mask = LTE_RLCDLI_STATUS_ALLOWED_MASK;

} /* lte_rlcdl_timer_status_prohibit_stop() */

/*==============================================================================

  FUNCTION:  lte_rlcdl_timer_status_prohibit_running

==============================================================================*/
/*!
    @brief
    Is status prohibit timer running.

    @return
    TRUE if the timer is running. FALSE otherwise.
*/
/*============================================================================*/
static inline boolean lte_rlcdl_timer_status_prohibit_running
(
  const lte_rlcdli_ctrl_blk_s* ctrl_blk_ptr  /*!< ptr to control block */
)
{
  /*--------------------------------------------------------------------------*/

  /*--------------------------------------------------------------------------*/
  return (ctrl_blk_ptr->status.status_prohibit_mask == 
          LTE_RLCDLI_STATUS_PROHIBIT_MASK);
} /* lte_rlcdl_timer_status_prohibit_running() */

#endif /* LTE_RLCDL_TIMER_H */
