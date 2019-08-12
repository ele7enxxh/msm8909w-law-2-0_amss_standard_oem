/*****************************************************************************
***
*** TITLE
***
***  GPRS RLC/MAC TIMER DEFINITION
***
***
*** DESCRIPTION
***
***  Define the timers used by both RLC and MAC
***
***
*** Copyright (c) 2001-2013 Qualcomm Technologies, Inc.
*** All Rights Reserved. QUALCOMM Proprietary.
***
*** Export of this technology or software is regulated by the U.S.
*** Government. Diversion contrary to U.S. law prohibited.
***
*****************************************************************************/


/*****************************************************************************
***
***                        EDIT HISTORY FOR MODULE
***
***  This section contains comments describing changes made to the module.
***  Notice that changes are listed in reverse chronological order.
***
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grlc/inc/grlctimer.h#1 $
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 10-04-01   tjw     Add customer.h and comdef.h to all SU API files
*** 05/01/01    hv     Created.
*** 09/18/01    hv     Added 8ms to T3198 duration for host test
*** 09/19/12    hv     Added dual data space functionality
***
*****************************************************************************/

#ifndef _RM_TIMER_H_
#define _RM_TIMER_H_

/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/

#include "geran_variation.h"
#ifndef CUSTOMER_H
  #include "customer.h"
#endif
#ifndef COMDEF_H
  #include "comdef.h"
#endif


#include "gprsdef.h"
#include "gprsdef_g.h"
#include "rex.h"


/*****************************************************************************
***
***     Public Type Definitions
***
*****************************************************************************/

/*===========================================================================
*                             Timer Signals
*
*  Definitions of action performed on timers which are signalled between
*  MAC & RLC.
*============================================================================*/

/*-------------------------------------------
*  Timers that are used by both RLC and MAC
*--------------------------------------------
*/
typedef enum
{
  /*-----------------------------------------
  ** Downlink timers owned by RLC DL module
  **-----------------------------------------
  */

  /* RLC DL: wait for the 1st valid downlink data block.
  ** action: start when an assignment is received. Stop when the 1st
  ** valid data block is received. If expired, do abnormal release
  ** with random access
  */
  RM_TMR_T3190_AWAIT_VALID_DL_DATA,


  /* RLC DL: wait for release of downlink tbf
  ** action: start after sending packet acknack (ack mode) with FAI
  ** set or after sending packet control ack in response to final
  ** data block in unack mode. If expired, return to Idle.
  */
  RM_TMR_T3192_AWAIT_RELEASE_DL_TBF,


  /* .. other downlink MAC timers here ..
  */

  /*---------------------------------------
  ** Uplink timers owned by RLC UL module
  **---------------------------------------
  */

  /* RLC: wait for ul acknack.
  ** Action: start after sending block with CV = 0 or detecting window stall
  ** condition. Stop timer on receipt of PUAacknack. When expired, decrement
  ** N3102 and perform abnormal release with random access.
  */
  RM_TMR_T3182_AWAIT_UL_ACKNACK,

  /* RLC UL: timeout waiting for MAC reply.
  ** action: starts/re-starts every time RLC sends MAC message.
  ** expired: prints error message and calls state machine.
  */
  RM_TMR_AWAIT_MAC_UL_RESP,

  /* Guard Timer for Extended Uplink TBF mode (XTimer)
  */
  RM_TMR_AWAIT_UL_ACKNACK,

//#ifdef  FEATURE_RLC_AWAIT_ASS_GUARD
  RM_TMR_AWAIT_ASS_GUARD,
//#endif

  /* RLC: Stale PDU delete timer is activated when MR_NO_PS_ACCESS_IND is
  ** received. This timer is stopped when MR_PS_ACCESS_IND is received or
  ** roved-in to GAN.
  ** On expiry: ul.b_ul_pdu_delete_after_stale_tmr_expiry flag is set to TRUE
  ** and all UL PDUs in RLC buffers are purged. Any UL PDU received will be
  ** deleted while this flag is TRUE.
  */
  RM_TMR_STALE_PDU_DELETE

} rm_timer_t;

/*-------------------
** RLC timer status
**-------------------
*/
typedef enum
{
  /*================
  ** timer status
  **================
  */

  /* created but not active
  */
  TMR_INACTIVE,

  /* started and running
  */
  TMR_ACTIVE,

  /* timer has expired, pending action(s) resulting from expiry.
  ** When done, timer should enter TMR_INACTIVE state
  */
  TMR_EXPIRED,

  /*================
  ** timer request
  **================
  */
  /* Used for MAC to inform RLC DL to start the T3190 timer.
  */
  TMR_START

} rm_timer_state_t;

/*-----------------------------------------------------------------
** return the parameter to the timer callback function as a uint32
** given the timer id, the timer number if timer id is the t3198
** array
**-----------------------------------------------------------------
*/
#define  PACK_TIMER_PARAM(id,num)                       \
            (uint32)((uint32)(((num) & 0xff) << 8)  |   \
             (uint32)((id) & 0xff)                      \
            )

/*--------------------------------------------------------------
** unpack the parameter of the callback function into separate
** timer id and timer number, if timer is is the t3198 array.
**--------------------------------------------------------------
*/
#define  UNPACK_TIMER_PARAM(p,id,num)         \
            (id)  = (rm_timer_t)((p) & 0xff); \
            (num) = (uint8)((p) >> 8) & 0xff;

/*-----------------------------------------------------------------
** structure to hold message sent by the timer callback function
** to the rlc ul or dl task.
**-----------------------------------------------------------------
*/
typedef struct
{
  q_link_type   link;

  /* timer id
  */
  rm_timer_t    sig;

} rm_timer_msg_t;

/*------------------------------
** general RLC timer structure
**------------------------------
*/
typedef struct
{
  rm_timer_t      id;

  /* statically allocated timer
  */
  rex_timer_type  rex_timer;

  /* for timer array where expiry action does not result in a
  ** signal being sent, the state is changed to RM_TMR_EXPIRED
  */
  rm_timer_state_t  state;

} grlc_timer_t;

/*****************************************************************************
***
***     Public Defines & Constants
***
*****************************************************************************/

/* NOTE - GCF Test Case 42.3.1.2.3 induces a simultaneous restart of T3182
** and T3180 (No USF) in MAC. In our implementation, MAC is a higher task
** priority and will process T3180 expiry before RLC will process T3182
** expiry. The desire is to process T3182 in this instance so shorten the
** specified T3182 period by two frame ticks to ensure this order of precedence.
*/

/* CR 2531 in TS 51.010-1 negates the need to shorten the
** specified T3182 period by two frame ticks (see commends above)
*/

//#define RM_TMR_T3182_DUR        ((5*1000)-10) /* 5 secs (4990ms), 13.1 */
#define RM_TMR_T3182_DUR        (5*1000)     /* 5 secs */
#define RM_TMR_MAC_UL_RESP_DUR  (6*1000)    /* ! */

/* Timer to wait for packet uplink acknack. This is set to 6s to cater for both single DS and
** multiple DS mode
*/
#define RM_TMR_AWAIT_UL_ACKNACK_DUR (6*1000)

//#ifdef  FEATURE_RLC_AWAIT_ASS_GUARD
#define RM_TMR_AWAIT_ASS_GUARD_DUR  (TBF_EST_REQ_GUARD_TMR)
//#endif /* FEATURE_RLC_AWAIT_ASS_GUARD */

#define RM_TMR_STALE_PDU_DELETE_DUR (130*1000) /* 130 secs */

/*****************************************************************************
***
***     Public Data References
***
*****************************************************************************/




/*****************************************************************************
***
***     Public Macros
***
*****************************************************************************/

/*===========================================================================
===
===  MACRO         xxx
===
===  DESCRIPTION
===
===    xxx.
===
===  DEPENDENCIES
===
===    xxx.
===
===  RETURN VALUE
===
===    xxx.
===
===  SIDE EFFECTS
===
===    xxx.
===
===========================================================================*/

/***************************************************************************************
** For DUAL DATA SPACE implementation, the timer_id is extended to encode the gas_id 
** in the 2nd byte. The following 3 macros allow the gas_id to be encoded and extracted 
** from the timer parameter, and also allow the timer ID to be extracted from the timer 
** parameter, ie. to mask out the gas_id
****************************************************************************************
*/

/* Extract the gas_id from the 2nd byte of the REX timer parameter
*/
#define TIMER_GET_DATA_SPACE_INDEX(timer_param)   (((timer_param) >> 8) & 0xff)

/* Place the gas_id in the 2nd byte and the timer ID in the 1st byte
*/
#define TIMER_SET_PARAM(gas_id, timer_param)      \
          ((0xffff) & ((((gas_id) & 0x0ff) << 8) | ((timer_param) & 0x0ff)))

/* Extract the timer_id (in the 1st byte) ignoring the gas_id in the 2nd byte
*/
#define TIMER_GET_ID(timer_param) (rm_timer_t)((timer_param) & 0xff)

/*****************************************************************************
***
***     Public Functions
***
*****************************************************************************/

/*===========================================================================
===
===  FUNCTION      xxx
===
===  DESCRIPTION
===
===    xxx.
===
===  DEPENDENCIES
===
===    xxx.
===
===  RETURN VALUE
===
===    xxx.
===
===  SIDE EFFECTS
===
===    xxx.
===
===========================================================================*/


#endif /* _RM_TIMER_H_ */

/*** EOF: don't remove! ***/
