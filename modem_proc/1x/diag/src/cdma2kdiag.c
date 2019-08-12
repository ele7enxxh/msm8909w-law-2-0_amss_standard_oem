/*==========================================================================

                   D I A G N O S T I C     T A S K
                  P A C K E T   P R O C E S S I N G
                           1 X Integration

GENERAL DESCRIPTION
  This file contains routines that are called to process the arrival of
  packets on the sio link.  There is one procedure for each packet id
  and each procedure has the same job:

    1. Check packet for correctness.

    2. Perform processing required by the packet.

    3. Format a response packet.


  Copyright (c) 2001 - 2013 Qualcomm Technologies, Inc. 
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
===========================================================================*/


/*===========================================================================

                            Edit History

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/diag/src/cdma2kdiag.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/29/13   ab      Remove DIAG_TA_PARM_F and DIAG_TEST_F commands
12/19/12   bph     Replace memcpy w/ struct assignment or memscpy
09/14/12   jtm     Feature cleanup.
04/04/12   trc     Convert to m1x_time module
03/26/12   srk     Removed cdma2kdiag_tmob().
02/24/12   srk     Passed correct parameters to DAL sync enter and leave calls
02/08/12   srk     Removed FEATURE_HWTC, FEATURE_DIAG_RPC and IMAGE_APPS_PROC
01/04/12   srk     Feature Cleanup.
12/20/11   srk     Feature clean up
12/13/11   srk     Feature clean up: Mainlined FEATURE_FTM_RF
12/08/11   jtm     Gracefully handle DALSYS_SyncCreate failure.
11/30/11   srk     Feature clean up: Mainlined and Internalized Features
08/10/11   adw     Temporarily featurize out srch state check in tagraph.
06/28/11   jtm     Added srchi_i.h include.
05/20/11   mca     Only run cdma2kdiag_tagraph() when 1x is active
05/16/11   vks     Remove the below temporary change.
04/27/11   vks     Skip diag software switches init temporarily.
04/22/11   jtm     CMI Phase 4 - Final SU API changes.
01/31/11   jtm     Featurized _i.h includes for backwards compatibility.
01/20/11   jtm     Add ftm_cmd.h include.
11/15/10   jtm     Initial SU API split for CP.
11/05/10   jtm     Added more _i.h includes for Modem 1x SU API support.
11/01/10   jtm     Modem 1x SU API changes.
10/21/10   mca     Replace rex_crit_sections with DAL crit sections
08/16/10   mca     Remove crit sections from cdma2kdiag_get_switch
07/08/10   sst     Update to resolve pack issue with walsh diag command
03/30/10   vks     Replaced FEATURE_QDSP6 with FEATURE_MODEM_1X_SUPPORTS_Q6.
03/23/10   jtm     Replaced macro qw_equ_misaligned with equivalent memcpy.
03/15/10   vks     Remove Genesis revisit feature as DAL support is now
                   available on MDM9K.
02/04/10   jtm     Removed FEATURE_PLT.
01/29/10   jtm     Removed include for snd.h and dependencies.
                   Removed feature TMOBILE and FEATURE_ACP.
01/20/10   mca     Updated PACKED macro
01/10/10   vks     Use FEATURE_QDSP6 instead of T_QDSP6.
12/29/09   mca     Updated to use ONEX_CVT_Q2N()
10/30/09   mca     Prevent crash on native 64 bit architectures
10/12/09   vks     Corrected header file name DALDeviceId.h
09/22/09   jtm     Corrected compiler warnings.
09/16/09   adw     Klocwork cleanup.
09/09/09   adw     Removed unnecessary diagtarget.h include.
08/26/09   adw     Replaced deprecated ts.h api's with new time api's.
08/24/09   mca     Fixed Q6 link errors
08/13/09   mca     Changed cdma2kdiag_init() to avoid link error with C++
08/03/09   adw     Added cdma2kdiag dipswitch interface to support the new
                   switches DAL which replaces deprecated BIO interface.
08/02/09   jtm     Lint fixes.
07/22/09   adw     Mainlined FEATURE_DIAG_COUPLED_ARCHITECTURE.
06/11/09   jtm     Replaced FEATURE_1X_Q6_CRIT_SECT with T_QDSP6.
03/17/09   jtm     Replace INTLOCK with QDSP6 equivalent when
                   FEATURE_1X_Q6_CRIT_SECT is enabled.
03/04/09   adw     Added mccdma_v.h include.
08/26/08   mca     Removed calls to gpsonediag_init(), hdrlog_init()
08/21/08   ag      Renamed cdma2kdiag_inc_event_cnt() to diag_inc_event_cnt()
                   Renamed cdma2kdiag_get_event_cnt() to diag_get_event_cnt()
08/07/08   mca     Removed calls to fsdiag_init(), hsdiag_init()
                   Removed calls to nvdiag_init(), vocdiag_init()
                   Renamed diag_inc_event_cnt() to cdma2kdiag_inc_event_cnt()
                   Renamed diag_get_event_cnt() to cdma2kdiag_get_event_cnt()
12/15/06   as      Fixed critical lint errors.
06/20/06   as      Added featurization around cdma2kdiag_status_snapshot.
10/31/05   as      Fixed lint errors.
07/15/05   as      Cleanup cdma2kdiag_tmob featurization
05/18/04   as      Added 10 sec timeout in AKEY to prevent attacker to query
                   the A-key more rapidly.
01/08/04   as      Moved prl specific functions to prldiag.c/h
12/03/03   yll     Changed the mob_cai_rev to reflect the NV setting.
10/17/03   ph      For Rel A builds, redirect to new MC subsystem command and
                   added support for the new subsystem command.
09/12/03   yll     Added featurization for ACP include file.
08/19/03   gr      Deleted the #ifndef FEATURE_EFS2 wrapping the call to
                   fsdiag_init. The old diag interface will be present even
                   if FEATURE_EFS2 is defined.
08/16/03   as      Removed FEATURE_SPECIAL_MDR
07/28/03   wd      Un-Remove oboslete FTM_RF_TEST_CMDS, some targets still
                   rely on these commands.
07/15/03   jqi     Added Extended PRL support
01/24/03   wd      Remove obsolete FTM_RF_TEST_CMDS.
01/08/03   ma      featurization for GSM PLT
11/20/01   as      Created file.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "1x_variation.h"
#ifdef __cplusplus
  extern "C" {
#endif

/* Common */
#include "comdef.h"
#include "customer.h"
#include "modem_1x_defs.h"
#include "modem_1x_utils.h"

/* Cp */
#include "mccdma.h"
#include "mccdma_v.h"
#include "mccdma_i.h"
#include "snm.h"
#include "parm.h"

/* Diag */
#include "cdma2kdiag.h"
#include "cdma2kdiagi.h"
#include "diag.h"
#include "diagi.h"
#include "diagnv.h"
#include "diagpkt.h"
#include "diagbuf.h"

/* Mux */
#include "rxc.h"
#include "txc.h"

/* Srch */
#include "srch.h"
#include "srch_diag.h"
#include "srch_diag_i.h"

/* Other */
#include "DalDevice.h"            /* DAL_OPEN_SHARED */
#include "DALDeviceId.h"
#include "DDISoftwareSwitches.h"  /* switches */
#include "DALSys.h"               /* DAL_DeviceAttach() */
#include "DALStdErr.h"            /* DAL_SUCCESS */
#include "mobile.h"
#include "msg.h"
#include "m1x_diag.h"
#include "qw.h"
#include "rex.h"
#include "ftm_cmd.h"
#include "stringl.h" /* for memscpy */

#ifndef FEATURE_GSM_PLT
/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

/*--------------------------------------------------------------------------
  DB item data buffer for db reads
--------------------------------------------------------------------------*/
LOCAL db_items_value_type diag_db;

/*--------------------------------------------------------------------------
  Local variable to keep track of significant events.  It is incremented by
  other tasks calling the diag_inc_event_cnt function, and is returned as
  part of the phone_state response packet.
--------------------------------------------------------------------------*/
LOCAL word diag_event_count = 0;

/*-------------------------------------------------------------------------
      Macros
-------------------------------------------------------------------------*/

/* Critical sections */
#define CDMA2KDIAG_ENTER_CRIT_SECT()    REX_DISABLE_PREMPTION()
#define CDMA2KDIAG_LEAVE_CRIT_SECT()    REX_ENABLE_PREMPTION()

/*-------------------------------------------------------------------------
      Defines
-------------------------------------------------------------------------*/
#define CDMA2KDIAG_SW_CLEARED     0

/*-------------------------------------------------------------------------
      Typedefs
-------------------------------------------------------------------------*/

/* Switch DAL Initialization State */
typedef enum
{
  /* Switch DAL init has not been called */
  CDMA2KDIAG_SWITCHES_NOT_INITIALIZED,

  /* Switch DAL initialization is in progress */
  CDMA2KDIAG_SWITCHES_INITIALIZING,

  /* Switch DAL init has completed */
  CDMA2KDIAG_SWITCHES_INITIALIZED
}
cdma2kdiag_switch_init_status_type;


/* CDMA2K Diag Module data */
typedef struct
{
  /* Switch DAL initialization state */
  cdma2kdiag_switch_init_status_type   switch_init_status;

  /* Handle to the switches dal */
  DalDeviceHandle                     *switch_dal_handle;

  /* Critical section to protect shared switch data */
  DALSYSSyncHandle                     crit_sect;
}
cdma2kdiag_type;


/*-------------------------------------------------------------------------
      Static variables
-------------------------------------------------------------------------*/
/* Declare and initialize module static data structure */
static cdma2kdiag_type cdma2kdiag =
{
  CDMA2KDIAG_SWITCHES_NOT_INITIALIZED,      /* switch_init_status */
  NULL,                                     /* switch_dal_handle  */
};



/*===========================================================================

                    FUNCTION DECLARATIONS FOR MODULE

===========================================================================*/

/*===========================================================================

FUNCTION CDMA2KDIAG_TAGRAPH

DESCRIPTION
  This returns the latest vocoder rates and powers for the strip chart
  on the camera view temporal analyzer

DEPENDENCIES

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.
===========================================================================*/
PACK( void* ) cdma2kdiag_tagraph (
  PACK( void* ) req_ptr,       /* pointer to request packet  */
  word pkt_len                 /* length of request packet   */
)
{
  parm_fer_type   fer_data;         /* data for FER parameters */
  diag_tagraph_rsp_type   *rsp_ptr;

  const int rsp_len = sizeof(diag_tagraph_rsp_type);


  rsp_ptr = (diag_tagraph_rsp_type *)diagpkt_alloc (DIAG_TAGRAPH_F, rsp_len);

  /* Check for a valid pointer */
  if ( rsp_ptr == NULL )
  {
    M1X_MSG_SSID( DIAG, LEGACY_ERROR,
      "Falied to allocate rsp_ptr");
    return NULL;
  }

  /*------------------------------------------------------------------------
    Get the rssi data from the searcher and the vocoder rate data from
    the vocoder.
  -------------------------------------------------------------------------*/
  rsp_ptr->rx_rssi  = srch_get_rx_agc();
  rsp_ptr->adj_rssi = srch_get_tx_adj();

  /*------------------------------------------------------------------------
    txc_rpt_voc_rate and rxc_rpt_voc_rate return an enumerated type which
    only contains a few entries, so the packet uses a byte for them.
  -------------------------------------------------------------------------*/
  rsp_ptr->tx_voc   = (byte) txc_rpt_voc_rate();
  rsp_ptr->rx_voc   = (byte) rxc_rpt_voc_rate();

  parm_get_fer ( &fer_data );

  rsp_ptr->bad_frames   = fer_data.bad_frames;
  rsp_ptr->total_frames = fer_data.total_frames;

  return (rsp_ptr);

} /* cdma2kdiag_tagraph */


/*===========================================================================
FUNCTION CDMA2KDIAG_SER_RESET

DESCRIPTION
  Processes a request to reset the SER statistics.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.
===========================================================================*/
PACK( void* ) cdma2kdiag_ser_reset (
  PACK( void* ) req_ptr,     /* pointer to request packet  */
  word pkt_len               /* length of request packet   */
)
{
  diag_ser_reset_rsp_type *rsp_ptr;
  const int rsp_len = sizeof(diag_ser_reset_rsp_type);

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  rxc_ser_reset();

  rsp_ptr = (diag_ser_reset_rsp_type *)diagpkt_alloc(DIAG_SER_RESET_F, rsp_len);

  /* Check for a valid pointer */
  if ( rsp_ptr == NULL )
  {
    M1X_MSG_SSID( DIAG, LEGACY_ERROR,
      "Falied to allocate rsp_ptr");
  }

  return (rsp_ptr);

} /* cdma2kdiag_ser_reset */



/*===========================================================================
FUNCTION CDMA2KDIAG_SER_REPORT

DESCRIPTION
  Processes a request for SER statistics.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.
===========================================================================*/
PACK( void* ) cdma2kdiag_ser_report (
  PACK( void* ) req_ptr,       /* pointer to request packet  */
  word pkt_len                 /* length of request packet   */
)
{
  rxc_ser_type *ser_ptr;
  diag_ser_report_rsp_type *rsp_ptr;
  const int rsp_len = sizeof (diag_ser_report_rsp_type);

  rsp_ptr = (diag_ser_report_rsp_type *)diagpkt_alloc(DIAG_SER_REPORT_F, rsp_len);

  /* Check for a valid pointer */
  if ( rsp_ptr == NULL )
  {
    M1X_MSG_SSID( DIAG, LEGACY_ERROR,
      "Falied to allocate rsp_ptr");
    return NULL;
  }

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Cast the response buffer to the rxc_ser_type.  This works since the
  ** diag_ser_report_rsp_type, starting at the frame_cnt field, is the
  ** same as the rxc_ser_type.  Tell lint this is not a problem.
  */
  ser_ptr = (rxc_ser_type *)&rsp_ptr->frame_cnt; //lint !e740

  /* call service routine to report the ser results */
  rxc_ser_report( ser_ptr );

  return (rsp_ptr);

} /* cdma2kdiag_ser_report */


/*===========================================================================

FUNCTION DIAG_INC_EVENT_CNT

DESCRIPTION
  This function increments a count of events that took place in the phone
  which might imply that the phone had changed enough that other kinds of
  status might want to be requested from diag by external devices.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void  diag_inc_event_cnt (void)
{
  diag_event_count++;
}



/*===========================================================================

FUNCTION DIAG_GET_EVENT_CNT

DESCRIPTION
  This function returns the count of events maintained by this module.

DEPENDENCIES
  None.

RETURN VALUE
  diag_event_count.

SIDE EFFECTS
  None.

===========================================================================*/
word  diag_get_event_cnt (void)
{
  return diag_event_count;
}




/*===========================================================================
FUNCTION CDMA2KDIAG_STATE

DESCRIPTION
  Processes a request for the DMSS to return the current operating state
  of the phone.  The states mostly correspond to IS95/A states, and are
  set by the appropriate master control into the db, where the value is
  retrieved by Diag.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.

===========================================================================*/
PACK( void* ) cdma2kdiag_state (
  PACK( void* ) req_ptr,       /* pointer to request packet  */
  word pkt_len                 /* length of request packet   */
)
{
  diag_state_rsp_type *rsp_ptr;
  const int rsp_len = sizeof(diag_state_rsp_type);

  rsp_ptr = (diag_state_rsp_type *)diagpkt_alloc(DIAG_STATE_F, rsp_len);

  /* Check for a valid pointer */
  if ( rsp_ptr == NULL )
  {
    M1X_MSG_SSID( DIAG, LEGACY_ERROR,
      "Falied to allocate rsp_ptr");
    return NULL;
  }

/*------------------------------------------------------------------------*/

  /* For Rel A, DIAG clients are redirected to use the subsystem command */
  /* (subsys 30, cmd_code 2) implemented in mclog.c                      */
  /* This redirection is indicated to clients through                    */
  /* DIAG_D_USE_SUBSYS_CMD_S. Upon receiving this code, clients must     */
  /* query the phone with the DIAG subsystem dispatch command using      */
  /* (subsys 30 and cmd_code 2). The format of this subsystem command    */
  /* is to be included in a DIAG Subsystem ICD due out soon.             */
  /* You may also refer to mclog.h for the req/rsp structure.            */

  #ifdef FEATURE_IS2000_REL_A
  rsp_ptr->phone_state = DIAG_D_USE_SUBSYS_CMD_S;
  #else
  /* Get the current phone state from MC */
  rsp_ptr->phone_state = mc_state();
  #endif

  rsp_ptr->event_count = diag_event_count;

  M1X_MSG_SSID( DIAG, LEGACY_LOW,
    "Retrieved current state: %d, %d",
    rsp_ptr->phone_state,
    rsp_ptr->event_count);

  return (rsp_ptr);

} /* cdma2kdiag_state */



/*===========================================================================
FUNCTION CDMA2KDIAG_PILOT_SETS

DESCRIPTION
  Processes a request for the DMSS to return all of the information regarding
  pilot sets at this time.  It returns the number pilots in each of active,
  candidate, and neighbor sets.  It also gives the remaining set pilot
  increment.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.

===========================================================================*/
PACK( void* ) cdma2kdiag_pilot_sets (
  PACK( void* ) req_ptr,       /* pointer to request packet  */
  word pkt_len                 /* length of request packet   */
)
{
  srch_pilot_set_struct_type pilot_sets;  /* To hold data from srch */

  int i;                                  /* Index for copying structs */
  diag_pilot_sets_rsp_type *rsp_ptr;
  const int rsp_len = sizeof(diag_pilot_sets_rsp_type);

/*------------------------------------------------------------------------*/

  /* Call srch function to fill in response here */
  srch_get_pilot_set ( &pilot_sets );

  /* fill response packet */
  rsp_ptr = (diag_pilot_sets_rsp_type *)diagpkt_alloc(DIAG_PILOT_SETS_F, rsp_len);

  /* Check for a valid pointer */
  if ( rsp_ptr == NULL )
  {
    M1X_MSG_SSID( DIAG, LEGACY_ERROR,
      "Falied to allocate rsp_ptr");
    return NULL;
  }

  rsp_ptr->pilot_inc  = pilot_sets.pilot_inc;
  rsp_ptr->active_cnt = pilot_sets.active_cnt;
  rsp_ptr->cand_cnt   = pilot_sets.cand_cnt;
  rsp_ptr->neigh_cnt  = pilot_sets.neigh_cnt;

  for (i = 0;
       i < (pilot_sets.active_cnt + pilot_sets.cand_cnt + pilot_sets.neigh_cnt);
       i++) {
    rsp_ptr->pilot_sets[i].pilot_pn =
                                      pilot_sets.pilot_sets[i].pilot_pn;
    rsp_ptr->pilot_sets[i].pilot_strength =
                                      pilot_sets.pilot_sets[i].pilot_strength;
  }

  return (rsp_ptr);

} /* cdma2kdiag_pilot_sets */


/*===========================================================================
FUNCTION CDMA2KDIAG_AKEY_VERIFY

DESCRIPTION


  This procedure verifies an entry of the AKEY.  This function reads the
  AKey from NV, and compares it to what was sent in the packet.
  If NAM value is something other than 0xff, the AKEY for given NAM is changed.
  If NAM value is 0xff, AKEY for the current nam is changed.

  If they are the same, a value of TRUE is returned to the external device.
  If they are NOT the same, a value of FALSE is returned to the external
  device, and a rex_timed_wait is called, to make DIAG pause
  for 10 seconds, to deter hackers who are trying all possible AKEYs.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  May cause DIAG to timeout for 10 seconds.

===========================================================================*/
PACK( void* ) cdma2kdiag_akey_verify (
  PACK( void* ) req_pkt_ptr,
  word pkt_len
)
{
  int i;                            /* loop counter */

  qword challenge_A_key;            /* A_key converted from request packet. */
  qword existing_A_key;             /* A_key from NV request packet. */

  diag_akey_verify_req_type *req_ptr =
    (diag_akey_verify_req_type *) req_pkt_ptr;
  diag_akey_verify_rsp_type *rsp_ptr;
  const int rsp_len = sizeof(diag_akey_verify_rsp_type);

 /*------------------------------------------------------------------------*/

  /* Check for a valid pointer */
  if ( req_ptr == NULL )
  {
    M1X_MSG_SSID( DIAG, LEGACY_ERROR,
      "req_ptr is NULL.");
    return NULL;
  }

  rsp_ptr = (diag_akey_verify_rsp_type *)diagpkt_alloc(DIAG_AKEY_VERIFY_F, rsp_len);

  /* Check for a valid pointer */
  if ( rsp_ptr == NULL )
  {
    M1X_MSG_SSID( DIAG, LEGACY_ERROR,
      "Falied to allocate rsp_ptr");
    return NULL;
  }

  /* Assume failure. */
  rsp_ptr->akey_ok = FALSE;

  /* Send the nv read request, and report the status of the
  ** nv operation back to external device.
  */

  qw_set( challenge_A_key, 0L, 0L );
  qw_set( existing_A_key, 0L, 0L );

  /* -------------------------------------------------------------
  ** Convert A_key to 64-bit representation.  The last digits in
  ** the array are a checksum, which is not used in this function.
  ** ------------------------------------------------------------- */

#define  DIAG_CAVE_DATA_DIGITS       20

  for (i = 0; i < DIAG_CAVE_DATA_DIGITS; i++)
  {
    qw_mul( challenge_A_key, challenge_A_key, 10L );
    qw_inc( challenge_A_key, (dword)(req_ptr->akey[i] - '0'));
  }

#define DIAG_CURR_NAM 0xFF

  do
  {
    if (req_ptr->nam == DIAG_CURR_NAM)
    {
      if (diag_nv_read (NV_CURR_NAM_I, &nitem) == NV_DONE_S)
      {
        nitem.a_key.nam = nitem.curr_nam;
      }
      else
      {
        M1X_MSG_SSID( DIAG, LEGACY_HIGH,
          "Unable to read current NAM from NV");

        /* We cannot find out which AKey to read, so we must fail; */
        break;
      } /* nv_read CURR_NAM */
    }
    else
    {
      nitem.a_key.nam = req_ptr->nam;
    }

    if (diag_nv_read (NV_A_KEY_I, &nitem) != NV_DONE_S)
    {
      M1X_MSG_SSID( DIAG, LEGACY_HIGH,
        "Failed to read A-Key");
      /* Since we couldn't check it, assume it was wrong (but don't make diag
      ** timeout)
      */
      break;
    }
    else
    {
      /* Start out assuming it is ok */

      memscpy((void *) existing_A_key,
              sizeof(qword),
              (void *) nitem.a_key.key,
              sizeof(qword));

      if (!qw_cmp(challenge_A_key, existing_A_key))
      {
        rsp_ptr->akey_ok = TRUE;
      }
      else
      {
        /* If failed match, make DIAG timeout 10 seconds (just like SPC) */
        rsp_ptr->akey_ok = FALSE;
        diagpkt_commit(rsp_ptr);
        diag_timeout();
        diagbuf_flush();
      }
    } /* nv read AKEY */
  } while (0);

  return (rsp_ptr);

} /* cdma2kdiag_akey_verify */



/*===========================================================================

FUNCTION CDMA2KDIAG_STATUS_SNAPSHOT
DESCRIPTION
  This procedure configures the event reporting mechanism.

===========================================================================*/
PACK( void* ) cdma2kdiag_status_snapshot (
  PACK( void* ) req_pkt,
  word pkt_len
)
{
  diag_status_snapshot_rsp_type *rsp_ptr;
  const int rsp_len = sizeof(diag_status_snapshot_rsp_type);

  rsp_ptr = (diag_status_snapshot_rsp_type *)diagpkt_alloc(DIAG_STATUS_SNAPSHOT_F, rsp_len);

  /* Check for a valid pointer */
  if ( rsp_ptr == NULL )
  {
    M1X_MSG_SSID( DIAG, LEGACY_ERROR,
      "Falied to allocate rsp_ptr");
    return NULL;
  }

  rsp_ptr->esn = diagnv.esn;

  /* Collect the FULL IMSI */
  rsp_ptr->full_imsi.imsi_s1 = cdma.imsi_s1;
  rsp_ptr->full_imsi.imsi_s2 = cdma.imsi_s2;
  rsp_ptr->full_imsi.imsi_s = ONEX_CVT_Q2N(cdma.imsi_s);
  rsp_ptr->full_imsi.imsi_11_12 = cdma.imsi_11_12;
  rsp_ptr->full_imsi.mcc = cdma.mcc;
  rsp_ptr->full_imsi.imsi_addr_num = cdma.imsi_addr_num;

  db_get ( DB_SID, &diag_db );
  rsp_ptr->sid = diag_db.sid;

  db_get ( DB_NID, &diag_db );
  rsp_ptr->nid = diag_db.nid;

  db_get ( DB_BS_P_REV, &diag_db );
  rsp_ptr->p_rev = diag_db.bs_p_rev;

  rsp_ptr->prev_in_use = mc_get_p_rev_in_use();
  rsp_ptr->mob_p_rev = cdma.mob_cai_rev;

  rsp_ptr->band_class = mc_get_band_class();

  db_get ( DB_CHANNEL, &diag_db );
  rsp_ptr->frequency = diag_db.channel;

  db_get ( DB_RF_MODE, &diag_db );
  rsp_ptr->operation_mode = (uint8) diag_db.rf_mode;

  rsp_ptr->state = (byte) (cdma.curr_state >> 8);
  rsp_ptr->sub_state = (byte) cdma.curr_state;

  return (rsp_ptr);

} /* cdma2kdiag_status_snapshot */


/*===========================================================================

FUNCTION CDMA2KDIAG_SSD_VERIFY

DESCRIPTION


  This procedure verifies an entry of the SSD.  This function reads the
  SSDs from NV, and compares it to what was sent in the packet.
  If NAM value is something other than 0xff, the SSDs for given NAM are
  checked.  If NAM value is 0xff, SSDs for the current nam are checked.

  If they are the same, a value of TRUE is returned to the external device.
  If they are NOT the same, a value of FALSE is returned to the external
  device, and a rex_timed_wait is called, to make DIAG pause
  for 10 seconds, to deter hackers who are trying all possible SSDs.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  May cause DIAG to timeout for 10 seconds.

===========================================================================*/
PACK( void* ) cdma2kdiag_ssd_verify (
  PACK( void* ) req_pkt_ptr,
  word pkt_len
)
{
  DIAG_SSD_VERIFY_F_req_type *req_ptr =
    (DIAG_SSD_VERIFY_F_req_type *) req_pkt_ptr;
  DIAG_SSD_VERIFY_F_rsp_type *rsp_ptr;
  const int rsp_len = sizeof( DIAG_SSD_VERIFY_F_rsp_type );

 /*------------------------------------------------------------------------*/

  /* Check for a valid pointer */
  if ( req_pkt_ptr == NULL )
  {
    M1X_MSG_SSID( DIAG, LEGACY_ERROR,
      "req_pkt_ptr is NULL.");
    return NULL;
  }

  rsp_ptr = (DIAG_SSD_VERIFY_F_rsp_type *)diagpkt_alloc (DIAG_SSD_VERIFY_F, rsp_len);

  /* Check for a valid pointer */
  if ( rsp_ptr == NULL )
  {
    M1X_MSG_SSID( DIAG, LEGACY_ERROR,
      "Falied to allocate rsp_ptr");
    return NULL;
  }

  /* Assume failure. */
  rsp_ptr->ssd_ok = FALSE;

  /* Send the nv read request, and report the status of the
  ** nv operation back to external device.
  */

#define DIAG_SSD_CURR_NAM 0xFF

  do {
    if (req_ptr->nam == DIAG_SSD_CURR_NAM) {
      if (diag_nv_read(NV_CURR_NAM_I, &nitem) == NV_DONE_S) {
         if (req_ptr->ssd_select == DIAG_SSD_A) {
           nitem.ssd_a.nam = nitem.curr_nam;
         }
         else if (req_ptr->ssd_select == DIAG_SSD_B) {
           nitem.ssd_b.nam = nitem.curr_nam;
         }
         else {
           M1X_MSG_SSID( DIAG, LEGACY_ERROR,
             " Bad SSD Select entry ");
           return 0;
         }
      }
      else {
        M1X_MSG_SSID( DIAG, LEGACY_HIGH,
          "Unable to read current NAM from NV");

        /* We cannot find out which AKey to read, so we must fail; */
        break;
      } /* nv_read CURR_NAM */
    }
    else {
      if (req_ptr->ssd_select == DIAG_SSD_A) {
        nitem.ssd_a.nam = req_ptr->nam;
      }
      else if (req_ptr->ssd_select == DIAG_SSD_B) {
        nitem.ssd_b.nam = req_ptr->nam;
      }
      else {
        M1X_MSG_SSID( DIAG, LEGACY_ERROR,
          " Bad SSD Select entry ");
        return 0;
      }
    }

    if (req_ptr->ssd_select == DIAG_SSD_A) {
      /* Read SSD_A from NV and check against challenge */
      if ( diag_nv_read ( NV_SSD_A_I, &nitem ) != NV_DONE_S ) {
        M1X_MSG_SSID( DIAG, LEGACY_HIGH,
          "Failed to read SSD_A");
        /* Since we couldn't check it, assume it was wrong (but don't make diag
        ** timeout)
        */
        break;
      }
      else {
        if( qw_cmp( req_ptr->ssd_a, nitem.ssd_a.ssd ) == 0 ) {
          rsp_ptr->ssd_ok = TRUE;
        }
        else {
          /* If failed match, make DIAG timeout 10 seconds (just like SPC) */
          rsp_ptr->ssd_ok = FALSE;
          break;
        }
      } /* nv read SSD_A */
    }
    else if( req_ptr->ssd_select == DIAG_SSD_B) {
      /* Read SSD_B from NV and check against challenge */
      if ( diag_nv_read ( NV_SSD_B_I, &nitem ) != NV_DONE_S ) {
        M1X_MSG_SSID( DIAG, LEGACY_HIGH,
          "Failed to read SSD_B");
        /* Since we couldn't check it, assume it was wrong (but don't make diag
        ** timeout)
        */
        break;
      }
      else {
        if (qw_cmp( req_ptr->ssd_b, nitem.ssd_b.ssd ) == 0)
        {
          rsp_ptr->ssd_ok = TRUE;
        }
        else
        {
          rsp_ptr->ssd_ok = FALSE;
          break;
        }
      } /* nv read SSD_B */
    }
    else {
      M1X_MSG_SSID( DIAG, LEGACY_ERROR,
        " Bad SSD Select entry ");
      return 0;
    }

  } while (0);

  if (rsp_ptr->ssd_ok == FALSE)
  {
    diagpkt_commit (rsp_ptr);
    diag_timeout ();
    rsp_ptr = NULL;
  }

  return (rsp_ptr);

} /* cdma2kdiag_ssd_verify */


/*===========================================================================

FUNCTION       CDMA2KDIAG_SWITCH_INIT

DESCRIPTION    This function initializes the global cdma2kdiag switches
               DAL handle and must be called before any other switch
               function.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   Initializes the 1x handle into the switches DAL.

===========================================================================*/
void cdma2kdiag_switch_init( void )
{
  /* Return value from DAL interface */
  DALResult status = DAL_ERROR;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  M1X_MSG_SSID( DIAG, LEGACY_FATAL,
    "Bringup Revisit: skipping cdma2kdiag_switch_init");

  /* Must protect the creation of a critical section */
  CDMA2KDIAG_ENTER_CRIT_SECT();

  /* If the critical section has not been created ... */
  if ( cdma2kdiag.switch_init_status == CDMA2KDIAG_SWITCHES_NOT_INITIALIZED )
  {
    /* ... create one ... */
    status = DALSYS_SyncCreate( DALSYS_SYNC_ATTR_RESOURCE_INTERRUPT,
                                &cdma2kdiag.crit_sect,
                                NULL );

    /* Check create status ... */
    if ( status != DAL_SUCCESS )
    {
      /* Create failed */
      M1X_MSG_SSID( DIAG, LEGACY_ERROR,
        "Unable to create DAL Sync device, status %d",
        status );
    }
    else
    {
    /* ... and remember that one has been created */
    cdma2kdiag.switch_init_status = CDMA2KDIAG_SWITCHES_INITIALIZING;
    }
  }

  /* Now make use of the critical section instead of intlocking */
  CDMA2KDIAG_LEAVE_CRIT_SECT();

  if ( status == DAL_SUCCESS )
  {
    /* Ensure that only one task can initialize the switch DAL */
    DALSYS_SyncEnter( cdma2kdiag.crit_sect );

    /* If the switches have not been initialized ... */
    if ( cdma2kdiag.switch_init_status != CDMA2KDIAG_SWITCHES_INITIALIZED )
    {
      /* Attach to the switches dal */
      status = DAL_DeviceAttach( DALDEVICEID_SOFTWARESWITCHES,
                                 &cdma2kdiag.switch_dal_handle );

      /* Check initialization status ... */
      if ( status != DAL_SUCCESS )
      {
        /* Initialization failed */
        M1X_MSG_SSID( DIAG, LEGACY_ERROR,
          "Unable to attach to switches DAL, status %d",
          status );
      }
      else
      {
        /* Open the switches DAL */
        status = DalDevice_Open( cdma2kdiag.switch_dal_handle,
                                 DAL_OPEN_SHARED );

        /* Check open status ... */
        if ( status != DAL_SUCCESS ||
             cdma2kdiag.switch_dal_handle == NULL )
        {
          /* Open failed */
          M1X_MSG_SSID( DIAG, LEGACY_ERROR,
            "Unable to open switches DAL, status %d",
            status );
        }
        else
        {
          /* Both the switch DAL attach and open operations were successful and
             the switch handle is now initialized and ready for use. Set the
             status so that no other task may re-initialize the handle. */
          cdma2kdiag.switch_init_status = CDMA2KDIAG_SWITCHES_INITIALIZED;
        }
      }
    }

    /* Ensure only one task can initialize switches */
    DALSYS_SyncLeave( cdma2kdiag.crit_sect );
  }

} /* cdma2kdiag_switch_init */


/*===========================================================================

FUNCTION       CDMA2KDIAG_GET_SWITCH

DESCRIPTION    This function returns the state of a dipswitch.

DEPENDENCIES   The cdma2kdiag_switch_init() routine must be called
               prior to invoking this function.

RETURN VALUE   Returns the state of the dipswitch. The Switches DAL returns
               a value of 0 when a dipswitch is set. This function translates
               the DAL API value into a boolean.

               TRUE - the dipswitch is set.

SIDE EFFECTS   None.

===========================================================================*/
boolean cdma2kdiag_get_switch
(
  /* Dip switch to check */
  cdma2kdiag_dip_sw_type dip_sw
)
{
  /* State of the dipswitch */
  uint32 dip_sw_state = CDMA2KDIAG_SW_CLEARED;

  /* Return value from DAL interface */
  DALResult status;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( cdma2kdiag.switch_init_status != CDMA2KDIAG_SWITCHES_INITIALIZED ||
       cdma2kdiag.switch_dal_handle == NULL )
  {
    M1X_MSG_SSID( DIAG, LEGACY_ERROR,
      "Invalid switches DAL handle, status: %d",
      cdma2kdiag.switch_init_status );
  }
  else
  {
    /* Querry the current dipswitch state */
    status = DalSoftwareSwitches_GetMask( cdma2kdiag.switch_dal_handle,
                                          (uint32) dip_sw,
                                          &dip_sw_state );

    /* Check for an unsuccessful read */
    if ( status != DAL_SUCCESS )
    {
      M1X_MSG_SSID( DIAG, LEGACY_ERROR,
        "Failed to get switch %d: DAL returned %d",
        dip_sw,
        status );
    }
  }

  /* Return TRUE if the dipswitch is set */
  return ( dip_sw_state != CDMA2KDIAG_SW_CLEARED );

} /* cdma2kdiag_get_switch */


/*===========================================================================

FUNCTION       CDMA2KDIAG_SET_SWITCH

DESCRIPTION    This function sets the state of a dipswitch.

DEPENDENCIES   The cdma2kdiag_switch_init() routine must be called prior
               to invoking this function.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
void cdma2kdiag_set_switch
(
  /* Dip switch to set */
  cdma2kdiag_dip_sw_type dip_sw
)
{
  /* Mask to select all dipswitches */
  uint32 all_switches = 0xFFFF;

  /* State of the dipswitch mask */
  uint32 dip_sw_mask = 0;

  /* Return value from DAL interface */
  DALResult status;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( cdma2kdiag.switch_init_status != CDMA2KDIAG_SWITCHES_INITIALIZED ||
       cdma2kdiag.switch_dal_handle == NULL )
  {
    M1X_MSG_SSID( DIAG, LEGACY_ERROR,
      "Invalid switches DAL handle, status: %d",
      cdma2kdiag.switch_init_status );
  }
  else
  {
    /* Protect the shared handle */
    DALSYS_SyncEnter( cdma2kdiag.crit_sect );

    /* Querry the current dipswitch mask state */
    status = DalSoftwareSwitches_GetMask( cdma2kdiag.switch_dal_handle,
                                          all_switches,
                                          &dip_sw_mask );

    /* Check for an unsuccessful read */
    if ( status != DAL_SUCCESS )
    {
      M1X_MSG_SSID( DIAG, LEGACY_ERROR,
        "Failed to get switch %d: DAL returned %d  mask %d",
        dip_sw,
        status,
        dip_sw_mask );
    }
    else
    {
      /* Update the dipswitch mask state to set the new switch */
      dip_sw_mask |= (uint32) dip_sw;

      /* Set the specified dipswitch using the updated mask */
      status = DalSoftwareSwitches_SetAll( cdma2kdiag.switch_dal_handle,
                                           dip_sw_mask );
    }

    /* Leave critical section */
    DALSYS_SyncLeave( cdma2kdiag.crit_sect );

    /* Check for an unsuccessful set */
    if ( status != DAL_SUCCESS )
    {
      M1X_MSG_SSID( DIAG, LEGACY_ERROR,
        "Failed to set switch %d: DAL returned %d  mask %d",
        dip_sw,
        status,
        dip_sw_mask );
    }
  }

} /* cdma2kdiag_set_switch */


/*===========================================================================

FUNCTION       CDMA2KDIAG_CLEAR_SWITCH

DESCRIPTION    This function clears the state of a dipswitch.

DEPENDENCIES   The cdma2kdiag_switch_init() routine must be called prior
               to invoking this function.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
void cdma2kdiag_clear_switch
(
  /* Dip switch to set */
  cdma2kdiag_dip_sw_type dip_sw
)
{
  /* Mask to select all dipswitches */
  uint32 all_switches = 0xFFFF;

  /* State of the dipswitch mask */
  uint32 dip_sw_mask = 0;

  /* Return value from DAL interface */
  DALResult status;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* REVISIT: This interface requires additional testing and validation
     before general use in 1x code. Outputing an error message on each
     invocation with this notification. Clients using the current
     implementation do so at their own risk. */
  M1X_MSG_SSID( DIAG, LEGACY_ERROR,
    "Interface requires additional validation before use" );

  if ( cdma2kdiag.switch_init_status != CDMA2KDIAG_SWITCHES_INITIALIZED ||
       cdma2kdiag.switch_dal_handle == NULL )
  {
    M1X_MSG_SSID( DIAG, LEGACY_ERROR,
      "Invalid switches DAL handle, status: %d",
      cdma2kdiag.switch_init_status );
  }
  else
  {
    /* Protect the shared handle */
    DALSYS_SyncEnter( cdma2kdiag.crit_sect );

    /* Querry the current dipswitch mask state */
    status = DalSoftwareSwitches_GetMask( cdma2kdiag.switch_dal_handle,
                                          all_switches,
                                          &dip_sw_mask );

    /* Check for an unsuccessful read */
    if ( status != DAL_SUCCESS )
    {
      M1X_MSG_SSID( DIAG, LEGACY_ERROR,
        "Failed to get switch %d: DAL returned %d  mask %d",
        dip_sw,
        status,
        dip_sw_mask );
    }
    else
    {
      /* Update the dipswitch mask state to clear the specified switch */
      dip_sw_mask &= ~( (uint32) dip_sw );

      /* Clear the specified dipswitch using the updated mask */
      status = DalSoftwareSwitches_SetAll( cdma2kdiag.switch_dal_handle,
                                           dip_sw_mask );
    }

    /* Leave critical section */
    DALSYS_SyncLeave( cdma2kdiag.crit_sect );

    /* Check for an unsuccessful set */
    if ( status != DAL_SUCCESS )
    {
      M1X_MSG_SSID( DIAG, LEGACY_ERROR,
        "Failed to clear switch %d: DAL returned %d  mask %d",
        dip_sw,
        status,
        dip_sw_mask );
    }
  }

} /* cdma2kdiag_clear_switch */



/*===========================================================================

                    DIAG DISPATCH DELCARATIONS FOR MODULE

===========================================================================*/

#define DISPATCH_DECLARE(func) \
extern PACK( void* ) func(PACK( void* ) req_pkt, word len)

DISPATCH_DECLARE (mclog_stat);
DISPATCH_DECLARE (mclog_get_is2k_status);
DISPATCH_DECLARE (mclog_markov);
DISPATCH_DECLARE (mclog_markov_reset);
DISPATCH_DECLARE (parm_diag_retrieve);
DISPATCH_DECLARE (parm_diag_legacy_get);
DISPATCH_DECLARE (parm_diag_set);
DISPATCH_DECLARE (parm_diag_sup_fer);
DISPATCH_DECLARE (tdso_clear_stats);
DISPATCH_DECLARE (rxc_clear_rlp_statistics);

static const diagpkt_user_table_entry_type cdma2kdiag_tbl[] =
{
  {DIAG_STATUS_F, DIAG_STATUS_F, mclog_stat},
  {DIAG_IS2000_STATUS_F, DIAG_IS2000_STATUS_F, mclog_get_is2k_status},
  {DIAG_MARKOV_F, DIAG_MARKOV_F, mclog_markov},
  {DIAG_MARKOV_RESET_F, DIAG_MARKOV_RESET_F, mclog_markov_reset},
  {DIAG_PARM_RETRIEVE_F, DIAG_PARM_RETRIEVE_F, parm_diag_retrieve},
  {DIAG_PARM_GET2_F, DIAG_PARM_GET2_F, parm_diag_legacy_get},
  {DIAG_PARM_SET_F, DIAG_PARM_SET_F, parm_diag_set},
  {DIAG_TDSO_STAT_RESET_F, DIAG_TDSO_STAT_RESET_F, tdso_clear_stats},
  {DIAG_RLP_STAT_RESET_F, DIAG_RLP_STAT_RESET_F, rxc_clear_rlp_statistics},
  {DIAG_TAGRAPH_F, DIAG_TAGRAPH_F, cdma2kdiag_tagraph},
  {DIAG_SER_REPORT_F, DIAG_SER_REPORT_F, cdma2kdiag_ser_report},
  {DIAG_SER_RESET_F, DIAG_SER_RESET_F, cdma2kdiag_ser_reset},
  {DIAG_STATE_F, DIAG_STATE_F, cdma2kdiag_state},
  {DIAG_PILOT_SETS_F, DIAG_PILOT_SETS_F, cdma2kdiag_pilot_sets},
  {DIAG_AKEY_VERIFY_F, DIAG_AKEY_VERIFY_F, cdma2kdiag_akey_verify},
  {DIAG_STATUS_SNAPSHOT_F, DIAG_STATUS_SNAPSHOT_F, cdma2kdiag_status_snapshot},
  {DIAG_SSD_VERIFY_F, DIAG_SSD_VERIFY_F, cdma2kdiag_ssd_verify},
};


/*===========================================================================

DESCRIPTION

  This function sets up the dispatch table for the cdma2kdiag functions.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Registers the diag dispatch table.

===========================================================================*/
void cdma2kdiag_init (void)
{
  #ifndef __cplusplus
  DIAGPKT_DISPATCH_TABLE_REGISTER (DIAGPKT_NO_SUBSYS_ID, cdma2kdiag_tbl);
  #endif /* __cplusplus */
}

#endif /* !FEATURE_GSM_PLT */

#ifdef __cplusplus
  DIAGPKT_DISPATCH_AUTOREGISTER (DIAGPKT_NO_SUBSYS_ID, cdma2kdiag_tbl);

} /* extern "C" */
#endif /* __cplusplus */
