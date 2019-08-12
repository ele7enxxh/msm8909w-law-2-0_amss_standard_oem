#ifndef MCS_TIMER_H
#define MCS_TIMER_H

/*!
  @mcs_timer.h

  @brief Declares the mcs_timer functions
*/

/*===========================================================================

  Copyright (c) 2013 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/api/mcs_timer.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/10/10   ck      Removing featurizations to prepare for CMI Phase 4 changes
07/15/08   adm     Initial Revision           
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <comdef.h>
#include <timer.h>

#ifdef __cplusplus
/* Avoid name mangling for any C++ customers */
extern "C" {
#endif

/*===========================================================================

                           TYPE DECLARATIONS

===========================================================================*/

typedef struct mcs_timer_struct
{
  timer_type timer_type_inst;

}mcs_timer_type;

typedef struct mcs_timer_struct       *mcs_timer_ptr_type;

typedef struct mcs_timer_group_struct
{
  timer_group_type timer_grp_inst;

}mcs_timer_group_type;

typedef struct mcs_timer_group_struct *mcs_timer_group_ptr;

typedef unsigned long    mcs_timer_cb_data_type;

typedef void (*mcs_timer_t1_cb_type)
(
  /* Arbitrary data given to timer_def( ) to pass to this callback function */
  mcs_timer_cb_data_type              data
);


typedef void (*mcs_timer_t2_cb_type)
(
  /* Time elapsed from timer setting (start) */
  uint32                          time_ms,

  /* Arbitrary data given to timer_def2( ) to pass to this callback function */
  mcs_timer_cb_data_type          data
);

typedef timer_unit_type mcs_timer_unit_type;

/*===========================================================================

                           FUNCTION DECLARATIONS

===========================================================================*/


/*===========================================================================

  FUNCTION:  mcs_timer_def

===========================================================================*/
/*!
    @brief
    Defines and initializes a timer.

    @return
    None
*/
/*=========================================================================*/
void mcs_timer_def
(
  /* Timer to set */
  mcs_timer_ptr_type                  timer,

  /* Group timer will belong to (NULL=the "NULL" timer group) */
  mcs_timer_group_ptr                 group,

  /* APC (tcb!=NULL) or callback (tcb==NULL) to queue/call at timer expiry */
  mcs_timer_t1_cb_type                func,

  /* Arbitrary data for APC or callback */
  mcs_timer_cb_data_type              data
);

/*===========================================================================

  FUNCTION:  mcs_timer_def2

===========================================================================*/
/*!
    @brief
    Defines a 'clock callback' style timer.

    @detail
    When the timer expires:
    func(time_ms, data) is called from interrupt context,

    @return
      None
*/
/*=========================================================================*/
void mcs_timer_def2
(
  /* Timer to set */
  mcs_timer_ptr_type                  timer,

  /* Group timer will belong to (NULL=the "NULL" timer group) */
  mcs_timer_group_ptr                 group
);



/*===========================================================================

  FUNCTION:  mcs_timer_reg

===========================================================================*/
/*!
    @brief
    Defines a 'clock callback' style timer.

    @return
    None
*/
/*=========================================================================*/

void mcs_timer_reg
(
  /* Timer to set */
  mcs_timer_ptr_type                  timer,

  /* Function to call at timer expiry */
  mcs_timer_t2_cb_type                func,

  /* Arbitrary data for func(time_ms, data) */
  mcs_timer_cb_data_type              data,

  /* Time (in ms) until first timer expiry */
  timetick_type                   time_ms,

  /* Period (in ms) between repeated expiries (0 = not periodic) */
  timetick_type                   reload_ms
);


/*===========================================================================

  FUNCTION:  mcs_timer_set

===========================================================================*/
/*!
    @brief
    Sets an inactive timer to expire after a given period of time, or changes
    an active timer to expire after a given period of time.

    Optionally, specifies the timer to repeatly expire with a given period.


    @return
      None
*/
/*=========================================================================*/
void mcs_timer_set
(
  /* Timer to set */
  mcs_timer_ptr_type                  timer,

  /* Time (in units below) until first timer expiry */
  timetick_type                       time_value,

  /* Period (in units) between repeated expiries (0 = not periodic) */
  timetick_type                       reload,

  /* Unit to measure "ticks" in. */
  mcs_timer_unit_type                 unit
);


/*===========================================================================

  FUNCTION:  mcs_timer_get

===========================================================================*/
/*!
    @brief
    Get number of ticks before timer expires

    @return
    Number of ticks before timer expires
*/
/*=========================================================================*/

timetick_type mcs_timer_get
(
  /* Timer to get */
  mcs_timer_ptr_type                  timer,

  /* Unit to measure "ticks before timer expires" in. */
  mcs_timer_unit_type                 unit
);


/*===========================================================================

  FUNCTION:  mcs_timer_is_active

===========================================================================*/
/*!
    @brief
    To determine if a given timer is active
    
    @return
    Number of ticks before timer expires
*/
/*=========================================================================*/
extern boolean mcs_timer_is_active (mcs_timer_ptr_type timer);


/*===========================================================================

  FUNCTION:  mcs_timer_clr

===========================================================================*/
/*!
    @brief
    Stops an active timer
    
    @return
    Number of ticks remaining before expiry.
*/
/*=========================================================================*/

timetick_type mcs_timer_clr
(
  /* Timer to stop */
  mcs_timer_ptr_type                  timer,

  /* Unit to measure "ticks before expiry" in.  Use T_SCLK if not required */
  mcs_timer_unit_type                 unit
);


/*===========================================================================

  FUNCTION:  mcs_timer_pause

===========================================================================*/
/*!
    @brief
    Pauses an active timer.
    
    @return
    None
*/
/*=========================================================================*/

void mcs_timer_pause
(
  /* Timer to pause */
  mcs_timer_ptr_type                  timer
);


/*===========================================================================

  FUNCTION:  mcs_timer_resume

===========================================================================*/
/*!
    @brief
    Resumes a previously paused timer.
    
    @return
    None
*/
/*=========================================================================*/

void mcs_timer_resume
(
  /* Timer to resume */
  mcs_timer_ptr_type                  timer
);

/*===========================================================================

  FUNCTION:  mcs_timer_group_enable

===========================================================================*/
/*!
    @brief
    Enables a timer group.  Timers in the timer group which expired during the
    disabled period will have their expiry processing executed.

    @return
    None
*/
/*=========================================================================*/

void mcs_timer_group_enable
(
  /* Timer group to be enabled */
  mcs_timer_group_ptr                 timer_group
);


/*===========================================================================

  FUNCTION:  mcs_timer_group_enable

===========================================================================*/
/*!
    @brief
    Disables a timer group.  Timers in the timer group will continue to count
    down, but they will not expire.

    @return
    None
*/
/*=========================================================================*/

void mcs_timer_group_disable
(
  /* Timer group to be disabled */
  mcs_timer_group_ptr                 timer_group
);



/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* mcs_timers API specifically for Off-target (QTF) tests                  */
/* CAUTION: These APIs must only be used in an off-target build, if used   */
/*           otherwise; user will see linker errors                        */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*!
    @name
    mcs_timer_expire_now

    @brief
    Sets an active timer to expire right now.

    @return
    None
*/
extern void mcs_timer_expire_now
(
  mcs_timer_ptr_type                  timer
);
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* End of Off-target (QTF) specific mcs_timers API                         */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* MCS_TIMER_H */


