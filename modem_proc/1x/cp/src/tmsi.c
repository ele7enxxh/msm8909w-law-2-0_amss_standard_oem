/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                T M S I   A D D R E S S I N G   S U P P O R T

GENERAL DESCRIPTION
  This module contains the functions to support TMSI addressing for
  PCS CDMA messages.  Determines TMSI address calculation and resets,
  and provides the ability activate and reset TMSI timers.  All
  processing is performed within the Main Control task.

  Variable names in this module are the similar to variables named in
  ANSI J-STD-008.  The "assigning_tmsi" group is read from NV at startup
  and updated when the mobile receives a new TMSI Assignment.  These
  variables constitute the mobile's TMSI address:
    cdma.assigning_tmsi.info.tmsi_code
    cdma.assigning_tmsi.zone_len
    cdma.assigning_tmsi.info.tmsi_zone

EXTERNALIZED FUNCTIONS
  tmsi_address_match
    Determine if received and stored TMSI addresses match.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

  Copyright (c) 1996 - 2013 Qualcomm Technologies, Inc. 
  All Rights Reserved Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies, Inc. and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies, Inc.


*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/tmsi.c_v   1.6   03 Jul 2002 12:52:24   phosabet  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/cp/src/tmsi.c#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/12/13   ppr     Changes to pass correct arg to mcc_nv_read()
                   in tmsi_powerdown()
01/10/13   ppr     Do NV# writes in tmsi_powerdown() only
                   if there is a change in NV value
06/29/12   ppr     FEATURE_IS2000_REL_A_CC cleanup changes
04/04/12   trc     Convert to m1x_time module
11/15/10   jtm     Initial SU API split for CP.
10/12/09   ssh     Initialized current time variable to zero as part of KW
                   error fix.
08/26/09   adw     Replaced deprecated ts.h api's with new time api's.
05/14/09   ag      CMI phase 2 changes
04/17/09   jtm     Added includes to fix compiler warnings.
06/07/07   pg      Lint clean up.
04/26/06   an      Fixed Lint Errors
02/18/05   fc      Merged the following from MSMSHARED:
02/08/05   an       Mainlined IS95B features
09/27/04   va      Corrected sci_s variable name
08/18/04   az      Rel D negative SCI support.
08/11/03   az      Added support for FEATURE_MODIFIED_SCI
06/02/03   bkm     Lint cleanup.
07/03/02   ph      Changed upm function names
06/10/02   fc      Fixed UPM TMSI page match.
05/20/02   fc      Added support for Universal Page Messsage.
02/22/02   fc      Added support for Release A common channels.
07/06/01   ph      Added support for Callback Timers
05/16/01   fc      Merged the following from common archive:
                   04/12/01   fc      Added support of events report: TMSI
                                      assigned.
05/10/99   kmp     Changed FEATURE_JSTD008 to FEATURE_TMSI and changed the
                   MSG_LOW tmsi_address_match() to a MSG_MED to avoid a
                   compiler warning.
04/08/99   dna     Removed an unused variable to fix a compiler warning.
06/10/96   jpf     Use cur_bs_ptr to find BS tmsi_zone in tmsi_page_match()
                   and in tmsi_address_match().
06/05/96   jpf     Added a couple of MSG_LOWs.
04/01/96   day     Created module.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "1x_variation.h"
#include "memory.h"
#include "comdef.h"
#include "rex.h"

#include "caii_v.h"
#include "msg.h"
#include "m1x_diag.h"
#include "m1x_time_i.h"
#include "qw.h"
#include "nv.h"
#include "mccdma_v.h"
#include "mccdma_i.h"
#include "mccreg_v.h"
#include "mccreg_i.h"
#include "tmsi.h"
#include "event.h"

/* <EJECT> */
/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/
#define TMSI_SLOT_DELAY 2560
  /* Represents a 2 slot delay in ms for full-TMSI timeout calculation */

nv_cmd_type  tmsi_nv_buf;
  /* NV task command buffer */

nv_item_type  tmsi_nv_data;
  /* NV data buffer */

/* <EJECT> */
/*===========================================================================

FUNCTION TMSI_ADDRESS_MATCH

DESCRIPTION
  This function calculates the appropriate TMSI address based
  on the address information received and the stored TMSI information
  and determines if TMSI addresses match.  Returns boolean to
  indicate if match found; true => match found, false => no match.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

boolean tmsi_address_match
(
  caii_pc_hdr_type *hdr_ptr
    /* Pointer to received message header with TMSI address information */
)
{
  caii_ext_sysparm_type *esp_ptr;
    /* Pointer to current extended system parameters message */
  boolean addr_match = FALSE;
    /* Address match return status */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if TMSI code is valid */
  /* before calculating address. */
  if (cdma.assigning_tmsi.info.tmsi_code != INVALID_TMSI_CODE)
  {
    /* Determine type of address calculation */
    /* based on address length.              */
    /* Check if only TMSI code was sent.     */
    if (hdr_ptr->addr_len <= MAX_TMSI_CODE_LEN)
    {
      esp_ptr = (caii_ext_sysparm_type *) mcc_get_msg(CAI_EXT_SYS_PARM_MSG);

      /* TMSI address matches if:            */
      /* assigning_zone_len == tmsi_zone_len */
      /* lsb assigning_zone == tmsi_zone     */
      /* tmsi_code_addr == lsb tmsi_code     */
      /* msb tmsi_code == 0                  */
      if ((esp_ptr != NULL) &&
          (qw_cmp(esp_ptr->tmsi_zone,
                  cdma.assigning_tmsi.info.tmsi_zone ) == 0) &&
          (cdma.assigning_tmsi.zone_len == esp_ptr->tmsi_zone_len) &&
          (hdr_ptr->addr.type3.tmsi_code ==
              cdma.assigning_tmsi.info.tmsi_code))
      {
        addr_match = TRUE;
      }
    }
    /* Address length indicates full TMSI address length. */
    else
    {
      /* TMSI address matches if:                   */
      /* msb tmsi_zone == lsb tmsi_zone             */
      /* address_len - 4 == assigning_tmsi_zone_len */
      /* lsb tmsi_code_addr == tmsi_code            */
      if ((qw_cmp ( cdma.assigning_tmsi.info.tmsi_zone,
                    hdr_ptr->addr.type3.tmsi_zone ) == 0) &&
         ((hdr_ptr->addr_len - 4) == cdma.assigning_tmsi.zone_len ) &&
          (hdr_ptr->addr.type3.tmsi_code ==
              cdma.assigning_tmsi.info.tmsi_code))
      {
        addr_match = TRUE;
      }
    } /* addr_len <= max tmsi code len */
  } /* code != invalid tmsi code */

  M1X_MSG( DCP, LEGACY_MED,
    "TMSI address match: %d, TMSI %lx",
    (dword)addr_match,
    cdma.assigning_tmsi.info.tmsi_code);
  /* Return address match status */
  return (addr_match);

} /* tmsi_address_match */

/* <EJECT> */
/*===========================================================================

FUNCTION TMSI_PAGE_MATCH

DESCRIPTION
  This function determines the type of page match operation and
  checks for TMSI page match based on page class and page subclass
  information provided in the received General Page Message.
  Returns boolean to indicate if match found; true => match found,
  false => no match.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

mccdma_page_match_result_type tmsi_page_match
(
  caii_gen_page_rec_type *gen_ptr,
    /* Pointer to received general page message */
  mcc_page_info_type *page_info
    /* Structure to return infomation stored in the page */
)
{
  caii_ext_sysparm_type *esp_ptr;
    /* Pointer to current extended system parameters message */
  mccdma_page_match_result_type page_match = NO_MATCH;
    /* Page match return status */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if TMSI code is valid    */
  /* before calculating page match. */
  if (cdma.assigning_tmsi.info.tmsi_code != INVALID_TMSI_CODE)
  {
    esp_ptr = (caii_ext_sysparm_type *) mcc_get_msg(CAI_EXT_SYS_PARM_MSG);

    /* Perform TMSI page match based on page subclass */
    switch ( gen_ptr->page_subclass )
    {
      case 0:
        /* Store current TMSI code for page monitoring check */
        page_info->tmsi_code = gen_ptr->rec.format8.tmsi_code_addr;

        /* TMSI page matches if:               */
        /* assigning_zone_len == tmsi_zone_len */
        /* lsb assigning_zone == tmsi_zone     */
        /* tmsi_code_addr == tmsi_code         */
        if ((esp_ptr != NULL) &&
            (cdma.assigning_tmsi.zone_len == esp_ptr->tmsi_zone_len) &&
            (qw_cmp ( cdma.assigning_tmsi.info.tmsi_zone,
                      esp_ptr->tmsi_zone ) == 0) &&
            (gen_ptr->rec.format8.tmsi_code_addr ==
                cdma.assigning_tmsi.info.tmsi_code))
        {
          page_match = PAGE_MATCH;
          page_info->msg_seq = gen_ptr->rec.format8.msg_seq;
          page_info->special_service = gen_ptr->rec.format8.special_service;
          page_info->service_option = gen_ptr->rec.format8.service_option;
        }
        break;

      case 1:
        /* Store current TMSI code for page monitoring check */
        page_info->tmsi_code = gen_ptr->rec.format9.tmsi_code_addr;

        /* TMSI page matches if:               */
        /* assigning_zone_len == tmsi_zone_len */
        /* lsb assigning_zone == tmsi_zone     */
        /* tmsi_code_addr == tmsi_code         */
        if ((esp_ptr != NULL) &&
            (cdma.assigning_tmsi.zone_len == esp_ptr->tmsi_zone_len) &&
            (qw_cmp ( cdma.assigning_tmsi.info.tmsi_zone,
                      esp_ptr->tmsi_zone ) == 0) &&
            (gen_ptr->rec.format9.tmsi_code_addr ==
                cdma.assigning_tmsi.info.tmsi_code))
        {
          page_match = PAGE_MATCH;
          page_info->msg_seq = gen_ptr->rec.format9.msg_seq;
          page_info->special_service = gen_ptr->rec.format9.special_service;
          page_info->service_option = gen_ptr->rec.format9.service_option;
        }
        break;

      case 2:
        /* Store current TMSI code for page monitoring check */
        page_info->tmsi_code = gen_ptr->rec.format10.tmsi_code_addr;

        /* TMSI page matches if:               */
        /* assigning_zone_len == tmsi_zone_len */
        /* lsb assigning_zone == tmsi_zone     */
        /* tmsi_code_addr == tmsi_code         */
        if ((esp_ptr != NULL) &&
            (cdma.assigning_tmsi.zone_len == esp_ptr->tmsi_zone_len) &&
            (qw_cmp ( cdma.assigning_tmsi.info.tmsi_zone,
                      esp_ptr->tmsi_zone ) == 0) &&
            (gen_ptr->rec.format10.tmsi_code_addr ==
                cdma.assigning_tmsi.info.tmsi_code))
        {
          page_match = PAGE_MATCH;
          page_info->msg_seq = gen_ptr->rec.format10.msg_seq;
          page_info->special_service = gen_ptr->rec.format10.special_service;
          page_info->service_option = gen_ptr->rec.format10.service_option;
        }
        break;

      case 3:
        /* Store current TMSI code for page monitoring check */
        page_info->tmsi_code = gen_ptr->rec.format11.info.tmsi_code;

        /* TMSI page matches if:                   */
        /* assigning_zone_len == tmsi_zone_len rxd */
        /* lsb assigning_zone == tmsi_zone rxd     */
        /* tmsi_code_addr == tmsi_code             */
        if ((cdma.assigning_tmsi.zone_len ==
                gen_ptr->rec.format11.tmsi_zone_len) &&
            (qw_cmp ( cdma.assigning_tmsi.info.tmsi_zone,
                      gen_ptr->rec.format11.info.tmsi_zone ) == 0) &&
            (gen_ptr->rec.format11.info.tmsi_code ==
                cdma.assigning_tmsi.info.tmsi_code))
        {
          page_match = PAGE_MATCH;
          page_info->msg_seq = gen_ptr->rec.format11.msg_seq;
          page_info->special_service = gen_ptr->rec.format11.special_service;
          page_info->service_option = gen_ptr->rec.format11.service_option;
        }
        break;

      default:
        break;
    } /* switch on page subclass */
  } /* code != invalid tmsi code */

  /* Return match status */
  return page_match;

} /* tmsi_page_match */

/*===========================================================================

FUNCTION TMSI_RESET_FULL_TIMER

DESCRIPTION
  This function resets the full TMSI timer.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

void tmsi_reset_full_timer
(
  void
)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  M1X_MSG( DCP, LEGACY_LOW,
    "TMSI Full timer reset, was %ld: %ld, tmsi: 0x%lx",
    (dword)cdma.tmsi_full_timer_set,
    (dword)rex_get_timer(&mcc_full_tmsi_timer),
    cdma.assigning_tmsi.info.tmsi_code);

  /* Reset full TMSI timer indicator */
  cdma.tmsi_full_timer_set = FALSE;

  /* Reset full TMSI timer and timer signal */
  (void) mcc_clr_callback_timer (&mcc_full_tmsi_timer, MCC_FULL_TMSI_TIMER_ID);

} /* tmsi_reset_full_timer */

/* <EJECT> */
/*===========================================================================

FUNCTION TMSI_CODE_AND_TIMER_RESET

DESCRIPTION
  This function resets the TMSI code and all TMSI timers.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

void tmsi_code_and_timer_reset
(
  void
)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  M1X_MSG( DCP, LEGACY_LOW,
    "TMSI reset: %lx",
    cdma.assigning_tmsi.info.tmsi_code);

  /* Set TMSI code to invalid value */
  cdma.assigning_tmsi.info.tmsi_code = INVALID_TMSI_CODE;

  /* Events update: TMSI assigned */
  event_report_payload(EVENT_TMSI_ASSIGNED, sizeof(event_tmsi_assigned_type),
    &cdma.assigning_tmsi.info.tmsi_code);

  /* Reset full TMSI timer indicator and    */
  /* reset full TMSI timer and timer signal */
  tmsi_reset_full_timer ();

  /* Reset TMSI expiration timer and timer signal */
  (void) mcc_clr_callback_timer (&mcc_tmsi_exp_timer, MCC_TMSI_EXP_TIMER_ID);

} /* tmsi_code_and_timer_reset */

/* <EJECT> */
/*===========================================================================

FUNCTION TMSI_SET_FULL_TIMER

DESCRIPTION
  This function calculates full TMSI timeout value and sets the
  full TMSI timer.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

void tmsi_set_full_timer
(
  void
)
{
  int8 sci_s = 0;
  int16 neg_sci_s = 0;
  word tmsi_timeout;
    /* TMSI timeout value */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If valid TMSI code and full TMSI timer has not been set, */
  /* then set full TMSI timer.                                */
  if ((cdma.assigning_tmsi.info.tmsi_code != INVALID_TMSI_CODE) &&
      ( !cdma.tmsi_full_timer_set ))
  {
    /* Calculate full TMSI timeout value based on slot */
    /* cycle index.  See J-STD-008 - 2.6.3.1.6         */
    sci_s = mccidl_get_slot_cycle_index_s();

    /* Calculate full-TMSI timeout value */
    /* Timeout = T69m + 2.56 * pow(2,i) seconds
                      where i is equal to SLOT_CYCLE_INDEXs.
     */
    neg_sci_s = -1 * sci_s;
    tmsi_timeout = CAI_T69M + ( TMSI_SLOT_DELAY *
            ((sci_s > 0) ? (1 << sci_s): (1/(1 << neg_sci_s)))
                              ); /*lint !e734 */

    /* Set full-TMSI timer */
    (void) mcc_set_callback_timer( &mcc_full_tmsi_timer, tmsi_timeout, MCC_FULL_TMSI_TIMER_ID );

    /* Set full TMSI timer indicator */
    cdma.tmsi_full_timer_set = TRUE;
    M1X_MSG( DCP, LEGACY_LOW,
      "Full timer set %ldms (%ld)",
      (dword)tmsi_timeout,
      (dword) sci_s);
  }

} /* tmsi_set_full_timer */

/* <EJECT> */
/*===========================================================================

FUNCTION TMSI_SET_EXP_TIMER

DESCRIPTION
  This function determines if a TMSI expiration timer needs to be set
  to check if TMSI expiration time has expired.  If timer is required,
  then it will calculate TMSI expiration timeout value by determining
  the delta value between system time and TMSI expiration timer and sets
  the TMSI expiration timer to the delta value or maximum timeout value
  that can be set in mcc_set_callback_timer (word size), whichever is less.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

void tmsi_set_exp_timer
(
  void
)
{

  qword exp_sys_time;
    /* TMSI expiration time in system time format */
  qword curr_time;
    /* Current time */
  qword delta_time;
    /* TMSI expiration timeout value */
  qword max_timeout_value;
    /* Maximum timeout value that can be set in mcc_set_callback_timer (word) */

  /* initialize the current time to zero */
  qw_set(curr_time, 0, 0);

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If valid TMSI code then set expiration timer. */
  if (cdma.assigning_tmsi.info.tmsi_code != INVALID_TMSI_CODE)
  {
    /* Clear TMSI expiration timer and timer signal */
    (void) mcc_clr_callback_timer (&mcc_tmsi_exp_timer, MCC_TMSI_EXP_TIMER_ID);

    /* Create quadword maximum timeout value */
    qw_set ( max_timeout_value, 0L, 65535UL );

    /* Calculate system time in 80 ms units */
    /* Get current time */
    m1x_time_get_20ms_frame_time( curr_time );

    /* Get time into 80 ms units */
    (void) qw_div( curr_time, curr_time, 4U );

    /* Create 64-bit expiration time */
    qw_set ( exp_sys_time, 0L, cdma.assigning_tmsi.exp_time );

    /* Convert expiration time to system time format */
    /* by multiplying expiration time by 2*E12.      */
    qw_mul ( exp_sys_time, exp_sys_time, (dword) 4096UL );

    /* Calculate TMSI expiration timeout value in 80ms units */
    qw_sub( delta_time, exp_sys_time, curr_time );

    /* Convert TMSI expiration timeout value into ms units */
    qw_mul( delta_time, delta_time, (dword) 80UL );

    /* Set TMSI expiration timer. */
    /* If TMSI expiration timeout value is greater than or equal */
    /* to the maximum timeout value that can be set, then set    */
    /* maximum timeout value.                                    */
    if (( (int4) qw_hi ( delta_time ) < 0 ) ||
        ( qw_cmp ( delta_time, max_timeout_value ) >= 0 ))
    {
      /* Set TMSI expiration timer with max timeout value */
      (void) mcc_set_callback_timer( &mcc_tmsi_exp_timer, 65535U, MCC_TMSI_EXP_TIMER_ID );
    }
    /* If TMSI expiration timeout value is less than maximum */
    /* timeout value then set TMSI expiration timeout value. */
    else
    {
      /* If timeout value is <= 10 ms then consider */
      /* expiration timer expired.                  */
      if (( (int4) qw_hi ( delta_time ) < 0 ) ||
          ( qw_lo ( delta_time ) <= 10 ))
      {
        /* Reset TMSI code and full TMSI timer */
        tmsi_code_and_timer_reset ();
      }
      else
      {
        /* Set TMSI expiration timer with TMSI expiration timeout value */
        (void) mcc_set_callback_timer( &mcc_tmsi_exp_timer,
                              (word) (qw_lo ( delta_time )), MCC_TMSI_EXP_TIMER_ID);
      }
    }
  }

} /* tmsi_set_exp_timer */

/* <EJECT> */
/*===========================================================================

FUNCTION TMSI_EXPIRED

DESCRIPTION
  This function determines if TMSI expiration timer has expired.
  Compared against system time.  Returns boolean value indicating
  if time has or has not expired.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

boolean tmsi_expired
(
  void
)
{

  boolean expired;
    /* Indicates if TMSI has expired */
  qword exp_sys_time;
    /* TMSI expiration time in system time format */
  qword curr_time;
    /* Current time */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize return status */
  expired = FALSE;

  /* initialize the current time to zero */
  qw_set(curr_time, 0, 0);

  /* If valid TMSI code then check TMSI expiration time. */
  if (cdma.assigning_tmsi.info.tmsi_code != INVALID_TMSI_CODE)
  {
    /* Calculate system time in 80 ms units */
    /* Get current time */
    m1x_time_get_20ms_frame_time( curr_time );

    /* Get time into 80 ms units */
    (void) qw_div( curr_time, curr_time, 4U );

    /* Create 64-bit expiration time */
    qw_set ( exp_sys_time, 0L, cdma.assigning_tmsi.exp_time );

    /* Convert expiration time to system time format */
    qw_mul ( exp_sys_time, exp_sys_time, (dword) 4096UL );

    /* Compare current system time against TMSI expiration time */
    if ( qw_cmp ( curr_time, exp_sys_time ) > 0 )
    {
      /* Clear TMSI expiration timer and timer signal */
      (void) mcc_clr_callback_timer (&mcc_tmsi_exp_timer, MCC_TMSI_EXP_TIMER_ID);

      /* TMSI has expired */
      expired = TRUE;
    }
  }

  return (expired);

} /* tmsi_expired */
/* <EJECT>  */
/*===========================================================================

FUNCTION TMSI_DETERMINE_CODE_SIZE

DESCRIPTION
  Determine the size (in bytes) of the specified TMSI code.

DEPENDENCIES
  None

RETURN VALUE
  Returns the size of the specified TMSI code, in bytes.

SIDE EFFECTS
  None

===========================================================================*/
byte tmsi_determine_code_size(dword tmsi_code)
{
   byte code_len;

  /* Determine the byte size of TMSI code */
  if (( tmsi_code >> 16 ) == 0 )
  {
    /* 16-bit TMSI code */
    code_len = 2;
  }
  else if (( tmsi_code >> 24 ) == 0 )
  {
    /* 24-bit TMSI code */
    code_len = 3;
  }
  else
  {
    /* 32-bit TMSI code */
    code_len = 4;
  }

  return code_len;
} /* tmsi_determine_code_size */
/* <EJECT>  */
/*===========================================================================

FUNCTION TMSI_INFO_UPDATE

DESCRIPTION
  This function updates the current TMSI information and sets
  TMSI expiration time based on data received in the TMSI
  assignment message.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void tmsi_info_update
(
  caii_tmsi_data_type  *tmsi_data
    /* Received TMSI assignment message data */
)
{
  /* Update current mobile information with new TMSI info */
  cdma.assigning_tmsi.zone_len = tmsi_data->zone_len;
  cdma.assigning_tmsi.exp_time = tmsi_data->exp_time;
  qw_equ (cdma.assigning_tmsi.info.tmsi_zone, tmsi_data->info.tmsi_zone);
  cdma.assigning_tmsi.info.tmsi_code = tmsi_data->info.tmsi_code;

  /* Events update: TMSI assigned */
  event_report_payload(EVENT_TMSI_ASSIGNED, sizeof(event_tmsi_assigned_type),
    &cdma.assigning_tmsi.info.tmsi_code);

  cdma.tmsi_code_len =
               tmsi_determine_code_size(cdma.assigning_tmsi.info.tmsi_code);

  /* Reset full TMSI timer indicator and    */
  /* reset full TMSI timer and timer signal */
  tmsi_reset_full_timer ();

  /* Determine timeout value based on delta between */
  /* TMSI expiration time and current system time.  */
  tmsi_set_exp_timer ();

} /* tmsi_info_update */

/*===========================================================================

FUNCTION TMSI_POWERDOWN

DESCRIPTION
  This function saves the current TMSI information in NV before
  powering down.

DEPENDENCIES
  This procedure saves parameters in NV.  The NV task should be ready
  to receive commands when this procedure is called.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void tmsi_powerdown
(
  void
)
{
  nv_stat_enum_type    nv_read_status; /* for status from NV read */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the fields of NV command to be sent to NV */
  tmsi_nv_buf.data_ptr = &tmsi_nv_data;

  /* Read out TMSI_CODE */
  tmsi_nv_buf.cmd  = NV_READ_F;
  tmsi_nv_buf.item = NV_TMSI_CODE_I;
  if( (nv_read_status = mcc_nv_cmd( &tmsi_nv_buf ) ) == NV_DONE_S )
  {
    if( (tmsi_nv_data.tmsi_code.nam != cdma.curr_nam) ||
        (tmsi_nv_data.tmsi_code.code != cdma.assigning_tmsi.info.tmsi_code)
      )
    {
      /* There is change in NV_TMSI_CODE_I value so write this NV */
      tmsi_nv_data.tmsi_code.nam = cdma.curr_nam;
      tmsi_nv_data.tmsi_code.code = cdma.assigning_tmsi.info.tmsi_code;
      /* Write out TMSI_CODE */
      tmsi_nv_buf.cmd = NV_WRITE_F;
      (void) mcc_nv_cmd( &tmsi_nv_buf );
    }
  }
  else
  {
    /* NV_TMSI_CODE_I read failed may be because of NV_NOTACTIVE_S
     * so write NV */
    tmsi_nv_data.tmsi_code.nam = cdma.curr_nam;
    tmsi_nv_data.tmsi_code.code = cdma.assigning_tmsi.info.tmsi_code;

    M1X_MSG( DCP, LEGACY_ERROR,
      "Write NV_TMSI_CODE_I, as read failed with nv_read_status:%d",
      nv_read_status);
    tmsi_nv_buf.cmd = NV_WRITE_F;
    (void) mcc_nv_cmd( &tmsi_nv_buf );
  }

  if (cdma.assigning_tmsi.info.tmsi_code != INVALID_TMSI_CODE)
  {
    /* Read out TMSI_ZONE_LEN */
    tmsi_nv_buf.cmd  = NV_READ_F;
    tmsi_nv_buf.item = NV_ASSIGNING_TMSI_ZONE_LEN_I;
    if( (nv_read_status = mcc_nv_cmd( &tmsi_nv_buf ) ) == NV_DONE_S )
    {
      if( (tmsi_nv_data.assigning_tmsi_zone_length.nam != cdma.curr_nam) ||
          (tmsi_nv_data.assigning_tmsi_zone_length.length !=
             cdma.assigning_tmsi.zone_len)
        )
      {
        /* There is change in TMSI_ZONE_LEN value so write this NV */
        tmsi_nv_data.assigning_tmsi_zone_length.nam = cdma.curr_nam;
        tmsi_nv_data.assigning_tmsi_zone_length.length =
                 cdma.assigning_tmsi.zone_len;
        /* Write out TMSI_ZONE_LEN */
        tmsi_nv_buf.cmd = NV_WRITE_F;
        (void) mcc_nv_cmd( &tmsi_nv_buf );
      }
    }
    else
    {
      /* NV_ASSIGNING_TMSI_ZONE_LEN_I read failed may be because of NV_NOTACTIVE_S
       * so write NV */
      tmsi_nv_data.assigning_tmsi_zone_length.nam = cdma.curr_nam;
      tmsi_nv_data.assigning_tmsi_zone_length.length =
                 cdma.assigning_tmsi.zone_len;

      M1X_MSG( DCP, LEGACY_ERROR,
        "Write NV_ASSIGNING_TMSI_ZONE_LEN_I, as read failed with nv_read_status:%d",
        nv_read_status);
      tmsi_nv_buf.cmd = NV_WRITE_F;
      (void) mcc_nv_cmd( &tmsi_nv_buf );
    }

    /* Read out TMSI_ZONE */
    tmsi_nv_buf.cmd  = NV_READ_F;
    tmsi_nv_buf.item = NV_ASSIGNING_TMSI_ZONE_I;
    if( (nv_read_status = mcc_nv_cmd( &tmsi_nv_buf ) ) == NV_DONE_S )
    {
      if( (tmsi_nv_data.assigning_tmsi_zone.nam != cdma.curr_nam) ||
          (qw_cmp(tmsi_nv_data.assigning_tmsi_zone.zone,
                  cdma.assigning_tmsi.info.tmsi_zone) != 0)
        )
      {
        /* There is change in TMSI_ZONE value so write this NV */
        tmsi_nv_data.assigning_tmsi_zone.nam = cdma.curr_nam;
        qw_equ(tmsi_nv_data.assigning_tmsi_zone.zone,
               cdma.assigning_tmsi.info.tmsi_zone);
        /* Write out TMSI_ZONE */
        tmsi_nv_buf.cmd = NV_WRITE_F;
        (void) mcc_nv_cmd( &tmsi_nv_buf );
      }
    }
    else
    {
      /* NV_ASSIGNING_TMSI_ZONE_I read failed may be because of NV_NOTACTIVE_S
       * so write NV */
      tmsi_nv_data.assigning_tmsi_zone.nam = cdma.curr_nam;
      qw_equ(tmsi_nv_data.assigning_tmsi_zone.zone,
               cdma.assigning_tmsi.info.tmsi_zone);

      M1X_MSG( DCP, LEGACY_ERROR,
        "Write NV_ASSIGNING_TMSI_ZONE_I, as read failed with nv_read_status:%d",
        nv_read_status);
      tmsi_nv_buf.cmd = NV_WRITE_F;
      (void) mcc_nv_cmd( &tmsi_nv_buf );
    }

    /* Read out TMSI_EXP_TIME */
    tmsi_nv_buf.cmd  = NV_READ_F;
    tmsi_nv_buf.item = NV_TMSI_EXP_I;
    if( (nv_read_status = mcc_nv_cmd( &tmsi_nv_buf ) ) == NV_DONE_S )
    {
      if( (tmsi_nv_data.tmsi_exp_timer.nam != cdma.curr_nam) ||
          (tmsi_nv_data.tmsi_exp_timer.time != cdma.assigning_tmsi.exp_time)
        )
      {
        /* There is change in TMSI_EXP_TIME value so write this NV */
        tmsi_nv_data.tmsi_exp_timer.nam = cdma.curr_nam;
        tmsi_nv_data.tmsi_exp_timer.time = cdma.assigning_tmsi.exp_time;

        /* Write out TMSI_EXP_TIME */
        tmsi_nv_buf.cmd = NV_WRITE_F;
        (void) mcc_nv_cmd( &tmsi_nv_buf );
      }
    }
    else
    {
      /* NV_TMSI_EXP_I read failed may be because of NV_NOTACTIVE_S
       * so write NV */
      tmsi_nv_data.tmsi_exp_timer.nam = cdma.curr_nam;
      tmsi_nv_data.tmsi_exp_timer.time = cdma.assigning_tmsi.exp_time;

      M1X_MSG( DCP, LEGACY_ERROR,
        "Write NV_TMSI_EXP_I, as read failed with nv_read_status:%d",
        nv_read_status);
      tmsi_nv_buf.cmd = NV_WRITE_F;
      (void) mcc_nv_cmd( &tmsi_nv_buf );
    }
  } /* End if (cdma.assigning_tmsi.info.tmsi_code != INVALID_TMSI_CODE) */

} /* tmsi_powerdown */


