/*!
  @file
  lte_l2_timer.h

  @brief
  This file contains defines  and functions interfaces for timer.


*/

/*===========================================================================

  Copyright (c) 2008 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/L2/common/inc/lte_l2_timer.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/01/14   ru      CR732514: Change mcs timer APIs to core timer APIs
05/15/08   bn      Initial version
===========================================================================*/

#ifndef LTE_L2_TIMER_H
#define LTE_L2_TIMER_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include <comdef.h>
#include <msgr.h>
#include <msgr_lte.h>
#include <timer.h>

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/


/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! @brief L2 common timer structure
*/
typedef struct
{
  timer_type timer;                   /*!< Timer  */
  msgr_tech_module_type  moduleid;    /*!< TECH,MODULE Performing Operation */
  msgr_umid_type umid;                /*!< UMID of message to be posted from callback */
} lte_l2_timer_s;


/*===========================================================================

  FUNCTION:  lte_l2_create_timer

===========================================================================*/
/*!
    @brief
    This function is called to create a timer.

    @detail
    Creates a timer by initializing the "timer" object
    passed in by the user. After the timer expires,
    a message with (moduleid, umid) is posted to the message router.
    The calling thread should register for that message.

    @return

    @note

    @see
    lte_l2_stop_timer(), lte_l2_start_timer()

*/
/*=========================================================================*/
extern void lte_l2_create_timer
( 
  lte_l2_timer_s* timer,             /*!< pointer to timer object */
  msgr_tech_module_type  moduleid,   /*!< TECH,MODULE Performing Operation */
  msgr_umid_type umid                /*!< UMID of message to be posted from callback */
);

/*===========================================================================

  FUNCTION:  lte_l2_start_timer_oneshot

===========================================================================*/
/*!
    @brief
    This function is called to start a timer.

    @detail
    Starts a timer for the specified delay in milliseconds.

    @return

    @note
    

    @see
    lte_l2_create_timer(), lte_l2_stop_timer()

*/
/*=========================================================================*/
extern void lte_l2_start_timer_oneshot
( 
  lte_l2_timer_s* timer,             /*!< pointer to timer object */
  uint32 delay                           /*!< duration of timer in milliseconds */
);

/*===========================================================================

  FUNCTION:  lte_l2_start_timer_periodic

===========================================================================*/
/*!
    @brief
    This function is called to start a timer.

    @detail
    Starts a timer for the specified delay in milliseconds.
    Automatically restarts the timer.
 
    @return

    @note

    @see
    lte_l2_create_timer(), lte_l2_stop_timer()

*/
/*=========================================================================*/
extern void lte_l2_start_timer_periodic
( 
  lte_l2_timer_s* timer,             /*!< pointer to timer object */
  uint32 delay                       /*!< duration of timer in milliseconds */
);

/*===========================================================================

  FUNCTION:  lte_l2_stop_timer

===========================================================================*/
/*!
    @brief
    This function is called to stop a timer.

    @detail
    Stops a timer

    @return

    @note
    
    @see
    lte_l2_create_timer(), lte_l2_start_timer()

*/
/*=========================================================================*/
extern void lte_l2_stop_timer
( 
  lte_l2_timer_s* timer              /*!< pointer to timer object */
);

/*===========================================================================

  FUNCTION:  lte_l2_delete_timer

===========================================================================*/
/*!
    @brief
    This function is called to delete a timer.

    @detail
    Deletes a timer

    @return

    @note
    

    @see
    lte_l2_create_timer(), lte_l2_start_timer()

*/
/*=========================================================================*/
extern void lte_l2_delete_timer
( 
  lte_l2_timer_s* timer              /*!< pointer to timer object */
);

/*===========================================================================

  FUNCTION:  lte_l2_timer_is_running

===========================================================================*/
/*!
    @brief
    This function is called to determine whether a timer is already running.

    @detail
    Returns TRUE if the timer is already running

    @return
    boolean
    
    @note
    

    @see related_function()
    lte_l2_create_timer(), lte_l2_start_timer(), lte_l2_stop_timer()

*/
/*=========================================================================*/
extern boolean lte_l2_timer_is_running
( 
  lte_l2_timer_s* timer              /*!< pointer to timer object */
);

#endif /* LTE_L2_TIMER_H */

