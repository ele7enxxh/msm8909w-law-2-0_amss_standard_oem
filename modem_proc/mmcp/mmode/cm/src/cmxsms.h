#ifndef CMXSMS_H
#define CMXSMS_H
/*===========================================================================

         C A L L   M A N A G E R   S M S   H E A D E R   F I L E

DESCRIPTION
  This header file contains all the definitions necessary to interface
  with CMSMS.c

Copyright (c) 1999 - 2006 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/



/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/cm/src/cmxsms.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/17/11   cl      Add support FEATURE_MMODE_REMOVE_1X for FUSION-MDM
02/25/09   sv      Remove cmxll_v.h
03/07/07   jqi     Added support for FEATURE_SMS_OVER_IP
01/06/06   ic      Lint cleanup
11/08/05   sk      Separated CM reports and commands.
01/07/05   ic      Removed all code under and referring to FEATURE_UASMS
12/03/04   ws      Remote WMS Support.
11/18/04   dk      Added cmxsms_abort_mo_msg() function.
03/23/04   ws      Merged 6300 Branch changes.
02/06/04   ws      Initial jaguar/mainline merge.
09/18/03   vt      Added functions to update DDTM status.
01/21/03   ws      Updated copyright information for 2003
08/06/02   HQ      Added FEATURE_CDSMS.
04/09/02   sh      Added MAX_SMS_WAIT_FOR_STATUS_TIME, CMSMS_HOLD_MODE_PREF_TIME
                   and max_wait_for_status_time
05/07/02   PRK     Renamed to cmxsms.h.
01/18/02   ic      Merged MSM_CM.05.03.33 on top of tip CCS CM
                   Diffed MSM_CM.05.03.32 with MSM_CM.05.03.33 and only the changes
                   between those two were merged with CCS CM
12/13/01   ic      Merged MSM_CM.05.03.32 onto SVD CM tip.
                   CCS CM code started from CM VU MSM_CM.05.03.25 + WCDMA/GSM
                   stuff from SIMBA team merged on top of it.
                   We diff'ed MSM_CM.05.03.25 with MSM_CM.05.03.32 (CM VU in N*2165)
                   that we needed to merge with and changes between those two VUs
                   were merged onto CCS CM tip.
10/23/01   VT/IC   Baseline for CCS
08/21/01   ic      Main-lined FEATURE_NEWCM
02/28/01   HQ      Fixed compiler warnings.
02/06/01   RC      Added support for System Determination 2.0
01/29/01   HQ      Properly feature-ized UASMS, GPSONE_DBM and NEWCM.
08/23/00   vt      Added new CM interface changes.
03/16/00   VT      Added BROADCAST SMS support
08/31/99   SH      Added FEATURE_UASMS flags to featurize UASMS
07/30/99   SH      Initial release.


===========================================================================*/

#include "mmcp_variation.h"
#if defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)

#include "comdef.h"    /* Definition for basic types and macros */
#include "cm.h"        /* External interface to cm.c */
#include "cm_v.h"
#include "cmi.h"       /* Internal interface to cm.c */
#include "cmxll.h"     /* MC to CM interface*/
#include "cmsms.h"     /* Interface to SMS object */

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
#ifdef FEATURE_CDSMS

#define MAX_SMS_HOLD_ORIG_TIME           25 /* 25 seconds for MO SMS hold orig retry*/

#define MAX_SMS_WAIT_FOR_STATUS_TIME     30 /* 30 seconds to wait for MO SMS status
                                               from MC */

#define CMSMS_HOLD_MODE_PREF_TIME        20    /* seconds */
    /* At end of SMS, delay unforcing the mode preference for
    ** this long.
    **
    ** NOTE:We delay unforcing the mode preference
    **      so we are able to receive
    **      the TL ack on the current system
    */
#endif

/*===========================================================================

FUNCTION cmxsms_is_mo_sms_in_progress

DESCRIPTION
 Query the mo sms in progress status

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  Nothing.

===========================================================================*/
boolean cmxsms_is_mo_sms_in_progress ( void );


/*===========================================================================

FUNCTION cmxsms_client_cmd_forward_mc

DESCRIPTION
  Forward a client SMS commands to the protocal stack.

DEPENDENCIES
  sms object must have already been initialized with
  cmsms_init().

RETURN VALUE
  TRUE if CM needs to wait for a replay from the protocal statck before
  continuing processing this command, FALSE otherwise.

SIDE EFFECTS
  none

===========================================================================*/
extern boolean cmxsms_client_cmd_forward_mc(

    const cmsms_s_type       *sms_ptr,
        /* Pointer to a sms object */

    const cm_sms_cmd_s_type      *sms_cmd_ptr
        /* Pointer to a CM command */
);

/*===========================================================================

FUNCTION cmsms_mc_rpt_proc

DESCRIPTION
  Process MC reports (i.e. notifications of phone activity or
  replies to clients phone commands).


DEPENDENCIES
  SMS object must have already been initialized with
  cmsms_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmsms_mc_rpt_proc(

  const cm_hdr_type   *rpt_ptr
    /* Pointer to MC report */
);


/*===========================================================================

FUNCTION cmsms_sd_rpt_proc

DESCRIPTION
  Process SD reports


DEPENDENCIES
  SMS object must have already been initialized with
  cmsms_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmsms_sd_rpt_proc(

  const cm_hdr_type   *rpt_ptr
    /* Pointer to SD report */
);

#ifdef FEATURE_CDSMS
/*===========================================================================

FUNCTION cmxsms_mo_msg_retry

DESCRIPTION
  Retry mobile originated SMS message

DEPENDENCIES
  sms object must have already been initialized with
  cmsms_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmxsms_mo_msg_retry( void);


/*===========================================================================

FUNCTION cmxsms_mo_msg_start

DESCRIPTION
  Kick-start the MO SMS process.

DEPENDENCIES
  SMS object must have already been initialized with
  cmsms_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmxsms_mo_msg_start(

    cmsms_s_type  *sms_ptr
        /* pointer to a sms object */
);


/*===========================================================================

FUNCTION cmxsms_timer_proc

DESCRIPTION
  Process timer events.

DEPENDENCIES
  SMS object must have already been initialized with
  cmsms_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmxsms_timer_proc(

    cm_timer_event_type    timer_event
);
#endif /* FEATURE_CDSMS */


/*===========================================================================

FUNCTION cmxsms_mt_msg

DESCRIPTION
 Process mobile terminated sms message

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  Nothing.

===========================================================================*/
void cmxsms_mt_msg (

    uint8      format,
         /* MT SMS message format */

    uint16     len,
         /* msg length */

    const uint8      *data
          /* msg buffer pointer */
);


/*===========================================================================

FUNCTION cmxsms_abort_mo_msg

DESCRIPTION
  Abort MO SMS.

DEPENDENCIES
  sms object must have already been initialized with
  cmsms_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmxsms_abort_mo_msg( void);


#endif /* FEATURE_MMODE_CDMA_800 || FEATURE_MMODE_CDMA_1900 */
#endif /* CMXSMS_H */

