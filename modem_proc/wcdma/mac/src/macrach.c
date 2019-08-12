/*===========================================================================================
                               R A C H    H A N D L E R

GENERAL DESCRIPTION

  This module provides functions to handle RACh functions

Copyright (c) 2001-2004 by Qualcomm Technologies, Inc.  All Rights Reserved.
Copyright (c) 2005-2009 by Qualcomm Technologies, Inc.  All Rights Reserved.

===============================================================================================*

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mac/vcs/macrach.c_v   1.17   10 Jul 2002 10:01:34   skrishna  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/mac/src/macrach.c#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when      who  what, where, why
--------  ---  -----------------------------------------------------------
05/10/16  as    Remove excessive F3s
05/20/15  as    Enable RxD after back-off timer expired and turn on RxD after persistency passed.
05/06/15  kc    Remember Aborted Status PDU on HS-RACH
12/15/14  as    In FACH state MAC should send CMAC_STATUS_IND only for TM CCCH UL data.
11/10/14  as    Added code change to not to acquire HS-RACH resource and also to trigger polling 
                in RWS situation.
10/08/14  kc    Decouple data indication in FACH & trigger based RACH logics
07/30/14  aa/kv Fix compiler warnings on Bolt
07/26/14  rkmk  Reduce image size by using right macro depending upon number of parameters
06/19/14  kc    Mainline FEATURE_WCDMA_HS_RACH_DELAY_CU
04/02/14  as    Fix to avoid concurency issue of MAC clearing the RACH status as part of config.
05/15/14  kc    Changes for enabling RxD for FACH Signaling when HS-RACH is configured
05/06/14  geg   Use safer versions of memcpy() and memmove()
01/06/13  kc    Data Indication to RRC during FACH state
10/29/13  ts    Added API mac_fetch_random_number() which calls secapi_get_random() having very good cryptographic properties
06/12/13  as    HSRACH - Fix to clear TM mode partial info at MAC as well during all scenarios
                while flushing RLC TM water mark and partial info.
05/09/13  kc    Fix to clear status at RLC while moving to PCH & RACH abort is received
05/08/13  scv   Fix L2 compilation errors
02/19/13  kc    Fix to pend status only if trashed pdu is RLC AM CTRL pdu
02/14/13  pr    Changed error fatal in mac_ul_rach_check_for_bfi_failure() to msg_error. 
02/07/13  as    Added debug code to crash if L2 has returned true for pending data in 
                buffer but fails during PDU build event when UE is in RACH state.
01/21/13  ksr   Initialization of hs_rach_state to RACH_IN_IDLE on config
07/16/12  mrg   MAC_I & HS-RACH Compilation Warnings fixed
09/13/12  mrg   HS-RACH Fix to start T2 & Backoff timers after collision resolution timer expires
09/04/12  kc    Fix to free DSM item pre-allocated at RLC when the very first 
                RACH procedure fails persistence and RACH abort is received thereafter.
07/25/12  as    New WM reg api() added for trigger base rach
08/13/12  ash   Correction to the ASC num value for HSRACH preamble procedure
08/13/12  mrg   HS-RACH & MAC-I Feature updated code check-in.
07/17/12  mrg   Moved enable/disable indication of Trigger based RACH from BFI to l1_query_mac_rach_data()
08/13/12  mrg   In FACH state, when L2 ack dropped due to RACH ABORT form L1 - 
                 inform to RLC to do re-tx of this control PDU
07/24/12  geg   Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
06/19/12  mrg   L2 - MAC-I and HS-RACH code changes
04/24/12  grk   Fixed compiler warnings
03/20/12  kc    Fixed concurrency issue between MAC config and RACH procedure
03/06/12  mrg   RACH staus info from L1 to MAC indicaiton with signal based approch enhanced with double buffer.
03/09/12  mrg   Persistence F3 are changed 
02/16/12   Ksr  Added support for Trigger Based Rach Procedure
01/02/12  grk   Feature cleanup
12/06/11  ksr   Fix to handle back to back RACH ABORT and RACH NO ACK to MAC from L1
                Moving to command based interface from signal based approach.
11/23/11  grk   Added mutex protection for RACH reset.
06/02/11  kc    Added support for logical channel to transport channel mapping info
12/16/10  mrg   In case of RACH abort and persistency failure MAC informs RLC to re-send status report
01/22/10  mrg   QCHAT MAC side changes
10/07/09  mrg   Fixed Klocwork warnings
06/26/08  av    l1-l2 interface cleanup 
05/07/09  ssg   Updated Copyright Information
04/29/09  av    Fixed compiler warnings in L2 
10/06/08  grk   Lint warning fixes.
09/16/08  ssg   MAC now indicates the sucessful TX of RACH msg to RRC.
04/11/07  ssg   Changed the included interface file.
09/26/07  mrg   Added support for ATF under FEATURE_CCCH_IE_ENH  
12/07/06  sk    Fixed compiler warning Removed valid declaration
05/09/06  sk/tliou Added code to remove dm dup under FEATURE_WCDMA_BUILD_PDU_DSM_OPT ASAP
05/27/05  sk    Removed compiler warnings.
05/05/05  sk    Changed the name of asc_selection to mac_asc_selection. Now this
                function is called from maculdata.c after L1 is queried to see if
                there is any available subchannel for that particular message.
03/09/05  sk    Fixed all lint errors.
02/09/05  sk    Fix for critical lint errors.
01/10/05  src   Replaced an instance of FEATURE_INTER_RAT_CELL_FACH_RE_SELECTION
                with FEATURE_CELL_FACH_MEAS_OCCASION to build without the former.
12/30/04  sk    Fixed lint errors
12/08/04  sk    Fixed compiler warnings.
11/12/04  yiz   Renamed RACH ACK status to L1_PRACH_MSG_DONE.
04/19/04  sk    Removed compiler warning for comparing an uitn with <= 0
03/30/03  sk    In persistence_is_ok() reverted the changes of randomly delaying prach based on
                persistence values.
03/13/03  sk    Fix for CR #25935. RACH is randomly delayed based on the persistence values.
12/04/02  sk    mac_reset_rach(): MAC will not register the call back function with RLC.
                PRACH would always be enabled during the lifetime of the call.
11/15/02  sk    Temporarily turn off the random delay caused in RACH transmissions for ASC_SELECTION.
11/07/02  sk    If persistence is not ok clear the num_trch = 0 to indicate that MAC is not ready
                to transmit this TTI
10/31/02  sk    mac_free_tx_dsm_items(): Clearing the number of transport channels to 0.
10/10/02  sk    Copied the CCCH parameters correctly for RLC size restrictions.
10/07/02  sk    Handled the case when MAC receives a L1_PRACH_WRONG_TTI from L1.
09/25/02  sk    proc_ul_tx_status_ind(): Ensured that MAC is able to handle RACH_ABORT ireespective
                of the current state of RACH. Copied rlc_size info whenthe cctrch type is RACH.
                This is under FEATURE_TFC_RLC_SIZE.
09/18/02  sk    Handled the case when RACH is waiting for the Right TTI to transmit after we
                get a NO_ACK from L1.
09/11/02  sk    Removed some unnecessary comments.
06/08/02  sk    Removed FEATURE_TRAFFIC_VOLUME_MEAS
05/13/02  sk    Populate the trch_id_to_idx_map_type structure for the purpose of
                traffic volume measurements. This addition is defined under the
                #ifdef FEATURE_TRAFFIC_VOLUME_MEAS
03/22/02  sh    l1_ul_trigger_tx_req(): use activation time only if
                FEATURE_L1_ACT_TIME is not defined.
03/20/02  ps    Changed the msg level of "Released DSM Item" from HIGH to LOW
03/20/02  ps    Fixed the DSM release problem
03/20/02  ps    Added F3 message when DSM items are released
03/12/02  ps    proc_rach_cctrch_cfg() : Do not process dedicated logical channel info if the
                number of dedicated logical channels is zero
02/12/02  ps    Deleted Uplink U-RNTI related code (U-RNTI is never used in UL)
11/29/01  ps    build_rach_tblks() : Added code to check number of transport channels in l1_tx_buf
08/07/01  ps    Fixed the leading zero message formatting problem
07/01/01  ps    Removed the code that sets and clears "mac_busy" flag (not used anymore)
                Removed the extra call to "asc_selection"
06/19/01  ps    Converted UL TX Status and Build Frame Indications from commands to signals
06/16/01  ps    Added code to process L1_PRACH_ABORT status and adjusted MSG levels
04/04/01  ps    Initial release after merging "macrachcfg.c" and "mactxstatus.c"

==============================================================================================*/

/*==============================================================================================
                                    INCLUDE FILES
==============================================================================================*/
#include "wcdma_variation.h"
#include  "customer.h"
#include  "rex.h"
#include  "msg.h"
#include  "rrccmd_v.h"
#include  "macrrcif_v.h"
#include  "macrlcif.h"
#include  "l1sapcommon.h"
#include  "l1task_v.h"
#include  "l2ultask.h"
#include  "macinternal.h"
#include  "macul.h"
#include  "maculdata.h"
#include  "ran.h"
#include  "secapi.h"
#include  "maculsubtask.h"
#include  "macdlhs.h"

#include "l1macif.h"

#include  <string.h>
#include  "macrach.h"

#ifdef FEATURE_QCHAT
#include "rrc_qchatupk.h"
#endif

/*===============================================================================================
                           DATA DECLARATIONS
===============================================================================================*/

/* Number of RACHs */
uint8   num_rachs;

/* RACH State */
rach_state_e_type rach_state = RACH_IN_IDLE;
boolean mac_rach_ccch_data_presence = FALSE;

rach_state_e_type hs_rach_state = RACH_IN_IDLE;

/* Number of RACH Preamble Cycles */
uint8 npreamble_cycles;

/* Backoff value */
uint16 backoff_val;


#ifdef FEATURE_WCDMA_HS_RACH
 mac_ul_hs_rach_cfg_info_type  mac_hs_rach_info;
#endif

/* Ordered RACH transmit control parameters */
mac_ul_rach_txcntl_config_type    rach_cntl_info;

/* Pointer to ordered config data of RACH */
mac_ul_rach_cctrch_config_type    *rach_cfg_ptr;
mac_ul_rach_cctrch_config_type    rach_cfg_info;


/* Layer 1 Transmit Data Buffer Pointer */
l1_ul_tx_data_type    *l1_tx_buf_ptr;

/* Holds contents of L1 Transmit Data Buffer
   used by RLC API to pend status if CTRL PDUs are
   transhed during RACH_ABORT */
l1_ul_tx_data_type mac_rach_tx_buf; 

#ifdef FEATURE_WCDMA_TRIGGER_BASED_RACH
uint32   num_tti_consec_without_data = 0;
#define  MAC_RACH_CONSEC_TTI_WITHOUT_DATA   5  
#endif

/* Number of Transport Blocks transmitted on RACH */
uint16    num_tx_tbs = 0;

/* Pointer to ordered TFCS data */
extern mac_ul_tfcs_type    *oc_ul_tfcs_ptr;

/* Current TTI */
extern l1_tti_enum_type    current_tti;

extern boolean mac_ul_config_in_progress;
extern rex_crit_sect_type mac_config_mutex;

#ifdef FEATURE_WCDMA_HSUPA
extern  rlc_ul_lc_info_type              mac_hs_ul_lc_info;
#endif

/*---------------------------------------------------------------------------
  Contains the list of logical channels for which MAC needs the buffer status
---------------------------------------------------------------------------*/
extern rlc_ul_lc_info_type              mac_ul_lc_info;

#ifdef FEATURE_WCDMA_TRIGGER_BASED_RACH
extern void mac_hsrach_phychan_estblish_failure_handler(void);
#endif

extern void rlc_enh_indicate_ul_data_trash();
extern uint32 rlc_am_ctrl_pdu_built;

uint32 mac_hs_rach_debug_trace_level = 0;

boolean mac_ul_rach_abrt_rcvd = FALSE;

boolean mac_hs_rach_rxd_enable = FALSE;

/*==============================================================================================
 FUNCTION NAME  l1_ul_trigger_tx_req

 DESCRIPTION  This function triggers L1 to enable / disable PRACH
===============================================================================================*/
void  l1_ul_trigger_tx_req
  (
/* L1 TX Trigger Request Type */
  l1_ul_phychan_ctrl_type   action
  )
{
  /* L1 External Command Buffer Pointer */
  l1_ext_cmd_type   *cmd_ptr;

  /* Get External Command Buffer Pointer */
  cmd_ptr = l1_get_cmd_buf();

  /* L1 External Command Buffer Pointer is NULL */
  if (cmd_ptr == NULL)
  {
    MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH , "CPHY_UL_TX_CONTROL_REQ command buffer allocation failed");
  }
  else
  {
    /* Populate Command Header fields */
    cmd_ptr->cmd_hdr.cmd_id = CPHY_UL_TX_CONTROL_REQ;

    /* This should be removed when activation time is a regular feature */

    /* Populate ommand data fields */
    cmd_ptr->cmd.ul_tx_ctrl.chan = L1_UL_PHYCHAN_PRACH;
    cmd_ptr->cmd.ul_tx_ctrl.action = action;

    /* Put the command in L1 External Command Queue */
    l1_put_cmd (cmd_ptr);

    MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH , "CPHY_UL_TX_CONTROL_REQ sent %u",
                                                action);

  }
}

/*===============================================================================================
FUNCTION NAME mac_free_tx_dsm_items

DESCRIPTION   Frees Transmit DSM items
===============================================================================================*/
void  mac_free_tx_dsm_items
  (
/* Transmit Data Buffer Pointer */
  l1_ul_tx_data_type    *tx_buf_ptr
  )
{
  uint8 j;

  /* Transport channel TX info pointer */
  l1_ul_trch_data_type  *trch_tx_ptr;

  /* Pointer to Transport channel info */
  trch_tx_ptr = &tx_buf_ptr->tx_data[0];

  MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "Released DSM items, set Trch=0");

  tx_buf_ptr->num_trch = 0;

  /* Release the DSM item of each Transport Block */
  for (j=0; j < num_tx_tbs; j++)
  {

    l1_l2_dsm_free_packet (&trch_tx_ptr->trch_data[j].data_ptr);

    MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_LOW , "Released DSM item %d", j);
  }
}

/*===============================================================================================
FUNCTION NAME mac_reset_rach

DESCRIPTION   Resets RACH state
===============================================================================================*/
void  mac_reset_rach
  (
  void
  )
{
#ifdef FEATURE_WCDMA_HS_RACH
  if (mac_hs_rach_info.hs_rach_active == FALSE)
#endif
  {
    /* Free the DSM items */
    mac_free_tx_dsm_items (l1_tx_buf_ptr);
  }

  memset(&mac_rach_tx_buf, 0x0, sizeof(l1_ul_tx_data_type));
  MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "MAC_RACH: Clearing mac_rach_tx_buf");

  rach_state = RACH_IN_IDLE;   /* Set RACH state to IDLE */

  mac_rach_ccch_data_presence = FALSE;
#ifdef FEATURE_WCDMA_HS_RACH
  hs_rach_state = RACH_IN_IDLE;
#endif
  
  npreamble_cycles = 0;   /* Clear preamble ramping cycle count */

#ifdef FEATURE_WCDMA_TRIGGER_BASED_RACH
  num_tti_consec_without_data = 0;
#endif

#ifdef FEATURE_QCHAT
  if (qchatupk_wcdma_asc_enabled())
  {
    qchatupk_mac_reset_rach();
  }
#endif
}

/*===============================================================================================
FUNCTION NAME mac_send_tx_status_ind_to_rrc

DESCRIPTION   Sends CMAC_STATUS_IND to RRC
===============================================================================================*/
void  mac_send_tx_status_ind_to_rrc
  (
  mac_status_ind_e_type status
  )
{
  rrc_cmd_type   *rrc_cmd_ptr;

  /* Failed to allocate memory for RRC command */
  if ((rrc_cmd_ptr = rrc_get_mac_cmd_buf()) == NULL)
  {
    MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH , "NULL Ptr when sending CMAC_STATUS_IND");
  }
  /* Send RRC_CMAC_STATUS_IND to RRC */
  else
  {
    MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH , "Sending RRC_CMAC_STATUS_IND to RRC");

    /* Fill in command header */
    rrc_cmd_ptr->cmd_hdr.cmd_id = RRC_CMAC_STATUS_IND;

    /* Fill in command data */
    rrc_cmd_ptr->cmd.cmac_status_ind = status;

    /* Send command to RRC */
    rrc_put_mac_cmd(rrc_cmd_ptr);
  }
}

#ifdef FEATURE_WCDMA_HS_RACH
/*===============================================================================================
FUNCTION NAME proc_rach_tx_parameters

DESCRIPTION   Processes RACH Transmit Control Parameters
===============================================================================================*/
void  proc_hs_rach_tx_parameters(mac_hs_rach_config_s_type *hs_rach_config_ptr)
{
  
  /* The number of ASC's should be atleast 1 */
  if ((hs_rach_config_ptr->mac_ul_rach_txcntl.nASC == 0) ||
      (hs_rach_config_ptr->mac_ul_rach_txcntl.nASC > MAX_ASC) ||
      (hs_rach_config_ptr->ccch_asc > MAX_ASC))
  {
    ERR_FATAL("HS_RACH: Invalid number of ASC %d or ccch_asc %d", 
               hs_rach_config_ptr->mac_ul_rach_txcntl.nASC, hs_rach_config_ptr->ccch_asc, 0);
  }
  
  /* Copy RACH transmit control parameters to local memory */
  WCDMA_MEMCPY(&(mac_hs_rach_info.hs_rach_cntl_info),
               sizeof(mac_ul_rach_txcntl_config_type), 
               &(hs_rach_config_ptr->mac_ul_rach_txcntl), 
               sizeof(mac_ul_rach_txcntl_config_type));

  mac_hs_rach_info.hs_rach_ccch_asc = hs_rach_config_ptr->ccch_asc;
  
  memset(hs_rach_tx_status, L1_HSRACH_MAC_INVALID, sizeof(hs_rach_tx_status));
  
  /* Change the hs_rach_active to TRUE */
  mac_hs_rach_info.hs_rach_active = TRUE;
  
  #ifdef FEATURE_WCDMA_TRIGGER_BASED_RACH
  num_tti_consec_without_data = 0;
  #endif

  mac_hs_rach_rxd_enable = FALSE; /* Initialize rxd_enable to false during config */
}

void proc_hs_rach_cfg(mac_ul_config_type *cfgptr)
{
  if (cfgptr->hs_rach_action != MAC_HS_RACH_NOOP)
  {
    if ((cfgptr->hs_rach_action == MAC_HS_RACH_START) || 
        (cfgptr->hs_rach_action == MAC_HS_RACH_CONFIG))
    {
      if (cfgptr->hs_rach_config_ptr != NULL)
      {
        /* Process RACH Transmit Control Parameters */
        proc_hs_rach_tx_parameters (cfgptr->hs_rach_config_ptr);
      }
      else
      {
        ERR_FATAL("HS_RACH: NULL HS-RACH Cfg Ptr", 0, 0, 0);
      }
    }
    else
    {
      mac_hs_rach_info.hs_rach_active = FALSE;
    }
    
    hs_rach_state = RACH_IN_IDLE;
  }
  
  MSG_3(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "HS_RACH: Config action %d, hs_rach_active %d, hs_rach_state %d",
                                             cfgptr->hs_rach_action,
                                             mac_hs_rach_info.hs_rach_active,
                                             hs_rach_state);
}
#endif

/*===============================================================================================
FUNCTION NAME proc_rach_tx_parameters

DESCRIPTION   Processes RACH Transmit Control Parameters
===============================================================================================*/
void  proc_rach_tx_parameters
  (
  mac_ul_rach_txcntl_config_type    *rach_tx_prmtrs
  )
{
  /* Copy RACH transmit control parameters to local memory */
  WCDMA_MEMCPY(&rach_cntl_info,
               sizeof(mac_ul_rach_txcntl_config_type), 
               rach_tx_prmtrs, 
               sizeof(mac_ul_rach_txcntl_config_type));

  /* The number of ASC's should be atleast 1 */
  if (rach_cntl_info.nASC == 0)
  {
    MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_ERROR, "Number of ASC's is 0");
  }

  mac_ul_rach_abrt_rcvd = FALSE;
}



/*===============================================================================================
FUNCTION NAME pick_mlp

DESCRIPTION   Selects the maximum MAC logical channel priority.

RETURNS     1 - 8 Depending on the selected MLP
===============================================================================================*/
uint8 pick_mlp(rlc_ul_frame_format_type *rlc_chan_info_ptr)
{
  uint8 mlp = MAC_UL_MAX_MLP_ID+1; // MAC_UL_MAX_MLP_ID = 8
  uint8           i;
  ul_lc_info_type *int_lc_ptr;
  rlc_ul_logchan_format_type    *lc_chan_ptr;

  /* Eliminate TFCs based on logical channel buffer occupancy */
  for (i=0 ; i < rlc_chan_info_ptr->nchan; i++)
  {
    lc_chan_ptr = &rlc_chan_info_ptr->chan_fmt[i];
    /* Get the logical channel ptr corresponding to the rlc_id */
    int_lc_ptr = GET_UL_LC_INFO_PTR((rlc_lc_id_type)(lc_chan_ptr->rlc_id));

    /* The number of pdus is greater than 0 */
    if (lc_chan_ptr->npdus > 0)
    {
      if (mlp > int_lc_ptr->asc_or_mlp)
      {
        mlp = int_lc_ptr->asc_or_mlp;
      }
    }

  }
  MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "Picked MLP %d", mlp);
  return mlp;
}


/*===============================================================================================
FUNCTION NAME asc_selection

DESCRIPTION   Selects an Access Service Class

RETURNS     0 - 7 Depending on the selected ASC
===============================================================================================*/
uint8 mac_asc_selection (rlc_ul_frame_format_type *chan_fmt)
{
  uint8 asc=0;

  uint8 mlp;

  /* If the ccch is enabled and if the number of dedicated logical
  channels mapped to ccch is 0 */
  if ((rach_cfg_ptr->ccch_enable) && (rach_cfg_ptr->ndlchan==0))
  {
    /* Since the value of asc is used as an index into rach_cntl_info.asc array
    to get the persistence values, care should be taken not to access the invalid
    entries in the array. hence the following check. rach_cntl_info.nASC gives the
    number of ASC's signalled to UE */
    if (rach_cfg_ptr->ccch_asc >= rach_cntl_info.nASC)
    {
      asc = rach_cntl_info.nASC-1;
    }
    else /* its safe to use the ccch_asc specified */
    {
      asc = rach_cfg_ptr->ccch_asc;
    }

    MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "Picked ASC %d", asc);
  }
  else
  {
    asc = rach_cntl_info.nASC;

    /* Pick the mlp with the highest priority */
    mlp = pick_mlp(chan_fmt);

    /* Among the ASC and MLP, pick the one with the highest priority */
    if (asc > mlp)
    {
      MSG_2(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "Picked ASC: %d MLP %d",
                                                 asc, mlp);
      asc = mlp;
    }
    else
    {
      /* Since the value of asc is used as an index into rach_cntl_info.asc array
      to get the persistence values, care should be taken not to access the invalid
      entries in the array. hence the following check. rach_cntl_info.nASC gives the
      number of ASC's signalled to UE */
      MSG_2(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "Picked ASC: %d MLP %d",
                                                 asc, mlp);
      asc = rach_cntl_info.nASC-1;
    }
  }

#ifdef FEATURE_QCHAT
  if (qchatupk_wcdma_asc_enabled())
  {
    qchatupk_mac_asc_selection(&asc);
  }
#endif /* FEATURE_QCHAT */

  return(asc);
}

/*===============================================================================================
FUNCTION NAME persistence_is_ok

DESCRIPTION   Checks to see if the persistence is OK or not

RETURNS     TRUE  if the persistence is OK FALSE otherwise

===============================================================================================*/
boolean   persistence_is_ok ()
{
  boolean rv = FALSE;




  uint32  random_num;

  // Below API returns a random number
  random_num = mac_fetch_random_number();

  /* Normalize this random number to a value between 0 & 65535 */
  random_num = ran_dist(random_num, (uint16)0, (uint16)65535);

  /* If the random value is less than the persistence associated with the
  acess service class that was selected, then persistence test has passed */
  if (random_num <= rach_cntl_info.asc[l1_tx_buf_ptr->asc_num].pval)
  {
    rv = TRUE;
    MSG_2(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "MAC_RACH: Passed Persistence. random_num %d, Persistence %d",
                                               random_num,
                                               rach_cntl_info.asc[l1_tx_buf_ptr->asc_num].pval);
  }
  /* Else the persistence test has failed and wait for the next TTI to
  generate a new random number ... */
  else
  {
    MSG_2(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "MAC_RACH: Failed Persistence. rnum %d Pi %d",
                                               random_num,
                                               rach_cntl_info.asc[l1_tx_buf_ptr->asc_num].pval);
  }


  return(rv);
}

/*===============================================================================================

MACRO     DCCH_DTCH_HDR_WITH_CRNTI_ON_RACH

DESCRIPTION   This macro decides MAC header value and size when DCCH/DTCH is mapped to
        RACH, and C-RNTI is used

          If one DCCH/DTCH is mapped to RACH, MAC header size is 20-bits long and it
        consists of :
        - TCTF     : 2-bits long and its value = 01
        - UE ID Type : 2-bits long and its value = 01
        - UE ID    : 16-bits long and its value is the C-RNTI

        If more than one DCCH/DTCH is mapped to RACH, MAC header size is 24-bits
        long and it consists of :
        - TCTF     : 2-bits long and its value = 01
        - UE ID Type : 2-bits long and its value = 01
        - UE ID    : 16-bits long and its value is C-RNTI
        - C/T    : 4-bits long & value is one minus MAC Logical channel identity

==============================================================================================*/
#define   DCCH_DTCH_HDR_WITH_CRNTI_ON_RACH\
\
    /* Get CRNTI value */ \
    crnti = GET_UL_MAC_CRNTI; \
\
    /* 1st header byte consists of TCTF, UE-ID type & 4 MSBits of UE-ID */  \
    hdrval[0] = (uint8)(0x50 + (crnti >> 12));  \
\
    /* 2nd header byte contains the next 8 MSBits of UE-ID */ \
    hdrval[1] = (uint8)(crnti >> 4);  \
\
    /* 4 MSBits of 3rd header byte contains 4 LSBits of UE ID */  \
    hdrval[2] = (uint8)((crnti << 4) & 0xf0); \
\
    /* Multiple DCCH/DTCH mapped to RACH - Add C/T field */ \
    if (ndlchan > 1) {  \
\
       hdrval[2] |= (oc_dlcptr->mac_id - 1);  \
\
       int_lc_ptr->mac_hdr_size = 24; \
    } \
\
    /* One DCCH/DTCH mapped to RACH - There is no C/T field */  \
    else {  \
\
       /* Header size = 20 bits */  \
       int_lc_ptr->mac_hdr_size = 20; \
    } \
    /* Save header value */   \
    WCDMA_MEMCPY(int_lc_ptr->mac_hdr_value,           \
                 sizeof(uint8) * MAC_UL_MAX_HDR_SIZE, \
                 hdrval,                              \
                 MAC_UL_MAX_HDR_SIZE);                \


/*==============================================================================================
FUNCTION NAME   get_rach_cfg_ptr

DESCRIPTION     This function returns rach_cfg_ptr
==============================================================================================*/
mac_ul_rach_cctrch_config_type *get_rach_cfg_ptr(void)
{
   return (rach_cfg_ptr);
}

/*==============================================================================================
 FUNCTION NAME    proc_rach_cctrch_cfg

 DESCRIPTION    This function processes ordered config data of RACH
==============================================================================================*/
void  proc_rach_cctrch_cfg
  (
/* Pointer to ordered config data of RACH */
  mac_ul_rach_cctrch_config_type    *oc_rachptr
  )
{
  uint8 i, ndlchan, hdrval[MAC_UL_MAX_HDR_SIZE];
  mac_ul_ded_logch_config_type  *oc_dlcptr;
  ul_lc_info_type         *int_lc_ptr;
  ul_trch_info_type       *int_tc_ptr;
  //boolean                   valid; /*lint -e550*/
  uint16                    crnti;
  trch_id_to_idx_map_type   *id_to_idx_ptr;

  maci_ul_trch_priority_info_type  *priority_info_ptr;

  uint8     rlc_count=0;
  /* Initialize */
  mac_ul_lc_info.nchan = 0;

  /* Get pointer to internal Transport Channel information */
  /* Right now, it is assumed that there is only one RACH */
  int_tc_ptr = GET_UL_TRCH_INFO_PTR(0);
  id_to_idx_ptr = GET_UL_TRCH_ID_TO_IDX_PTR(oc_rachptr->trch_id);
  /* Populate the trch_id_to_idx_table */
  id_to_idx_ptr->idx = 0;
  id_to_idx_ptr->valid = TRUE;

  MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_LOW , "RACH Configuration Parameters:");

  WCDMA_MEMCPY(&rach_cfg_info,           \
               sizeof(mac_ul_rach_cctrch_config_type), \
               oc_rachptr,                              \
               sizeof(mac_ul_rach_cctrch_config_type));                \

  /* Save RACH Ordered Config Data pointer */
  rach_cfg_ptr = &rach_cfg_info;

  /* Transport channel ID */
  int_tc_ptr->trch_id = oc_rachptr->trch_id;

  /* CCCH is mapped to RACH */
  if (oc_rachptr->ccch_enable)
  {

    /* CCCH ID is invalid */
    if (UL_RLC_ID_IS_INVALID(oc_rachptr->ccch_id))
    {
      MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH , "CCCH ID is invalid %d",
                                                  oc_rachptr->ccch_id);
    }
    /* Access Service Class is invalid */
    else if (UL_ASC_IS_INVALID(oc_rachptr->ccch_asc))
    {
      MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH , "ASC is invalid %d",
                                                  oc_rachptr->ccch_asc);
    }
    /* CCCH parameters are valid */
    else
    {
      /* RLC info pointer */
      int_lc_ptr = GET_UL_LC_INFO_PTR(oc_rachptr->ccch_id);

      /* Set Enable flag to TRUE */
      int_lc_ptr->enable = TRUE;

      /* Logical channel type is CCCH */
      int_lc_ptr->lc_type = UE_LOGCHAN_CCCH;

      /* Set RLC mode to TM_MODE */
      int_lc_ptr->rlc_mode = UE_MODE_TRANSPARENT;

      /* Save the rlc size restriction information */
      int_lc_ptr->rlc_size_info.all_tfi_valid = FALSE;
      int_lc_ptr->rlc_size_info.ntfi_per_lc = 1;
    /* if ATF present , ATF that CCCH should use assigned to int_lc_ptr */
    if (rach_cfg_ptr->atf_present)
    {
      int_lc_ptr->rlc_size_info.lc_tfi_list[0] = rach_cfg_ptr->atf_ccch_tf_index;
    }
    else
    {
      int_lc_ptr->rlc_size_info.lc_tfi_list[0] = 0;
    }

    /* Access Service Class */
    int_lc_ptr->asc_or_mlp = oc_rachptr->ccch_asc;

    /* MAC Header Size = 2 bits */
    int_lc_ptr->mac_hdr_size = 2;

    /* Save the Logical channel ID in transport channel information */
    int_tc_ptr->rlc_id[int_tc_ptr->nlchan] = oc_rachptr->ccch_id;

    /* Update logical channel count of transport channel info */
    int_tc_ptr->nlchan++;

      if (rlc_count < UE_MAX_UL_LOGICAL_CHANNEL)
      {
        mac_ul_lc_info.rlc_id[rlc_count++] = oc_rachptr->ccch_id;
      }
      mac_ul_lc_info.nchan = rlc_count;

    MSG_3(MSG_SSID_WCDMA_MAC, MSG_LEGACY_LOW , "CCCH: ASC= %d RLC Mode= %d RLC ID= %d",
                                               int_lc_ptr->asc_or_mlp,
                                               int_lc_ptr->rlc_mode,
                                               oc_rachptr->ccch_id);

      // Get the priority information of the transport channel 0 with priority
      // as indicated by the ccch_asc field.
      if (oc_rachptr->ccch_asc == 0)
      {
        priority_info_ptr = &int_ul_trch_priority_info[0][0];
      }
      else
      {
        priority_info_ptr = GET_UL_TRCH_PRIORITY_INFO_PTR(0,int_lc_ptr->asc_or_mlp);
      }
      // Populate the logical channel id which has this priority.
      priority_info_ptr->lc_id[priority_info_ptr->num_lc] = oc_rachptr->ccch_id;
      priority_info_ptr->num_lc++;

      MSG_2(MSG_SSID_WCDMA_MAC, MSG_LEGACY_LOW, "Pri %d, NChan %d",
                                                 oc_rachptr->ccch_asc,
                                                 priority_info_ptr->num_lc);

    }
  }

  /* Number of DCCH/DTCHs */
  ndlchan = oc_rachptr->ndlchan;

  if (ndlchan >0 )
  {

    /* Valid number of DCCH/DTCHs mapped to RACH */
    if (ndlchan > MAX_DED_LOGCHAN_PER_TRCH)
    {
      MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH , "invalid number of DCCH/DTCHs %d",
                                                  ndlchan);
    }
    /* Invalid C-RNTI */
    else if (UL_MAC_CRNTI_IS_VALID == FALSE)
    {
      MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH , "UE has no valid CRNTI");
    }
    /* Process config info of each DCCH/DTCH */
    else
    {
      for (i=0; i < ndlchan; i++)
      {

        /* Initially mark the parameter validity flag to FALSE */
        //valid = FALSE;

        /* Pointer to ordered config parameters of DCCH/DTCH */
        oc_dlcptr = &oc_rachptr->dlchan_info[i];

        /* Invalid RLC ID */
        if (UL_RLC_ID_IS_INVALID(oc_dlcptr->rlc_id))
        {
          MSG_2(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH , "DCCH/DTCH%d with invalid RLC ID %d",
                                                      i, oc_dlcptr->rlc_id);
        }
        /* Invalid RB ID */
        else if (UL_RB_ID_IS_INVALID(oc_dlcptr->rb_id))
        {
          MSG_2(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH , "DCCH/DTCH%d with invalid RB ID %d",
                                                      i, oc_dlcptr->rb_id);
        }
        /* Invalid MAC Priority */
        else if (UL_LC_PRIORITY_IS_INVALID(oc_dlcptr->priority))
        {
          MSG_2(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH , "DCCH/DTCH%d with invalid priority %d",
                                                      i, oc_dlcptr->priority);
        }
        /* Invalid RLC Mode */
        else if (UL_RLC_MODE_IS_INVALID(oc_dlcptr->rlc_mode))
        {
          MSG_2(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH , "DCCH/DTCH%d with invalid RLC Mode %d",
                                                      i, oc_dlcptr->rlc_mode);
        }
        /* Invalid MAC ID */
        else if ((ndlchan > 1) && UL_MAC_ID_IS_INVALID(oc_dlcptr->mac_id))
        {
          MSG_2(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH , "DCCH/DTCH%d with invalid MAC ID %d",
                                                      i, oc_dlcptr->mac_id);
        }
        /* Otherwise, parameters are valid */
        else
        {
          /* Uplink logical channel information table entry pointer */
          int_lc_ptr = GET_UL_LC_INFO_PTR(oc_dlcptr->rlc_id);

          /* Clear header value */
          memset (hdrval, 0,MAC_UL_MAX_HDR_SIZE);

          /* MAC Header Size and Header Value */
          DCCH_DTCH_HDR_WITH_CRNTI_ON_RACH

          /* Set Enable flag to TRUE */
          int_lc_ptr->enable = TRUE;

          /* Logical channel type */
          int_lc_ptr->lc_type = oc_dlcptr->chan_type;

          /* Save RLC mode */
          int_lc_ptr->rlc_mode = oc_dlcptr->rlc_mode;

          /* Save RB ID */
          int_lc_ptr->rb_id = oc_dlcptr->rb_id;

          /* Save priority */
          int_lc_ptr->asc_or_mlp = oc_dlcptr->priority;

          if (rlc_count < UE_MAX_UL_LOGICAL_CHANNEL)
          {
            mac_ul_lc_info.rlc_id[rlc_count++] = oc_dlcptr->rlc_id;
          }
          mac_ul_lc_info.nchan = rlc_count;


          /* Save the rlc size restriction information */
          WCDMA_MEMCPY(&int_lc_ptr->rlc_size_info,
                       sizeof(mac_ul_rlc_size_info_type),
                       &oc_dlcptr->rlc_size_info,
                       sizeof(mac_ul_rlc_size_info_type));

          /* RLC Header Length */
          if (int_lc_ptr->rlc_mode == UE_MODE_TRANSPARENT)
            int_lc_ptr->rlc_hdr_length = 0;
          else if (int_lc_ptr->rlc_mode == UE_MODE_UNACKNOWLEDGED)
            int_lc_ptr->rlc_hdr_length = 8;
          else
            int_lc_ptr->rlc_hdr_length = 16;

          priority_info_ptr = GET_UL_TRCH_PRIORITY_INFO_PTR(0, oc_dlcptr->priority);

          // Populate the logical channel id which has this priority.
          priority_info_ptr->lc_id[priority_info_ptr->num_lc] = oc_dlcptr->rlc_id;
          priority_info_ptr->num_lc++;
          MSG_2(MSG_SSID_WCDMA_MAC, MSG_LEGACY_LOW, "Pri %d, NChan %d",
                                                     oc_rachptr->ccch_asc,
                                                     priority_info_ptr->num_lc);


          /* Save the Logical channel ID in transport channel information */
          /* Update logical channel count of transport channel info */
          int_tc_ptr->rlc_id[int_tc_ptr->nlchan] = oc_dlcptr->rlc_id;
          int_tc_ptr->nlchan++;

          MSG_3(MSG_SSID_WCDMA_MAC, MSG_LEGACY_LOW , "DCCH/DTCH%d:HdrSize= %d Hdr[0]= %x",
                                                     i,
                                                     int_lc_ptr->mac_hdr_size,
                                                     int_lc_ptr->mac_hdr_value[0]);
          MSG_3(MSG_SSID_WCDMA_MAC, MSG_LEGACY_LOW , ":Hdr[1]= %x Hdr[2]= %x Hdr[3]= %x",
                                                     int_lc_ptr->mac_hdr_value[1],
                                                     int_lc_ptr->mac_hdr_value[2],
            int_lc_ptr->mac_hdr_value[3]);
          MSG_3(MSG_SSID_WCDMA_MAC, MSG_LEGACY_LOW , ":Hdr[4]= %x RB ID= %d RLC Mode= %d",
                                                     int_lc_ptr->mac_hdr_value[4],
                                                     int_lc_ptr->rb_id,
                                                     int_lc_ptr->rlc_mode);
        }
      }
    }
  }

#ifdef FEATURE_WCDMA_TRIGGER_BASED_RACH
  num_tti_consec_without_data = 0;
#endif
  
}

#ifdef  FEATURE_WCDMA_TRIGGER_BASED_RACH
/*==============================================================================================
 FUNCTION NAME  mac_ul_trigger_tx_req
 DESCRIPTION  This function is called by RLC whenever there is data queued in RLC buffers
        RLC notifies MAC to trigger L1 for PRACH enable
===============================================================================================*/
void  mac_rach_ul_trigger_tx_req(l1_ul_phychan_ctrl_type ul_req_type)
{
  MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "TRIG_RACH: Trigger L1 to enable/disable %d BFI",
                                             ul_req_type);
  l1_ul_trigger_tx_req (ul_req_type);
}

void  mac_check_for_bfi_disable_trigger()
{
  num_tti_consec_without_data++;
  
  if (num_tti_consec_without_data >= MAC_RACH_CONSEC_TTI_WITHOUT_DATA)
  {
    MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "TRIG_RACH: %d consecutive TTI didnt have any data - Requesting L1 to stop BFI",
                                               num_tti_consec_without_data);
                
    num_tti_consec_without_data = 0;
    
    if (l1_mac_rach_disable_trigger_needed() == TRUE)
    {
      rlc_ul_register_trigger_based_rach_api_active_rbs();
      mac_rach_ul_trigger_tx_req(L1_UL_PHYCHAN_DISABLE);
    }
    
  }
}

#endif
/*==============================================================================================
 FUNCTION NAME  mac_ul_trigger_tx_req

 DESCRIPTION  This function is called by RLC whenever there is data queued in RLC buffers
        RLC notifies MAC to trigger L1 for PRACH enable

===============================================================================================*/
void  mac_ul_trigger_tx_req
  (
#ifdef FEATURE_DSM_WM_CB
      dsm_watermark_type* wm_ptr,
      void*               func_data
#else
  void
#endif
  )
{
  #ifdef  FEATURE_WCDMA_TRIGGER_BASED_RACH
  if ((l1_mac_rach_enable_trigger_needed() == TRUE))
  {
    mac_rach_ul_trigger_tx_req(L1_UL_PHYCHAN_ENABLE);
  }

  if(mac_ul_config_in_progress != TRUE)
  {
    rlc_ul_call_data_ind_in_fach_cb();
  }  
  #else

  MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "Enabling PRACH");
  /* Trigger L1 to enable PRACH */
  l1_ul_trigger_tx_req (L1_UL_PHYCHAN_ENABLE);
  #endif
}

/*===============================================================================================
 FUNCTION NAME  proc_tx_status

 DESCRIPTION  This function processes transmit status received from PHY layer

===============================================================================================*/
void  proc_ul_tx_status_ind
(
  void
)
{

  uint8                       loop_index;
  
  REX_ISR_LOCK(&r99_ul_isr_l2_datapath_mutex);
  
  for(loop_index = 0; loop_index < 2; loop_index++)
  {
    if (rach_tx_status[loop_index] != L1_PRACH_MAC_INVALID)
    {

      if (rach_state != RACH_IN_IDLE)
      {
        /* RACH is waiting for Status from Layer 1 */
        switch (rach_tx_status[loop_index])
        {
        
          /* Received Tx Status is ACK */
          case L1_PRACH_MSG_DONE:
          /* RACH is not waiting for status */
          if (rach_state != RACH_WAITING_FOR_STATUS)
          {
            MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH , "rcvd Tx status %x in wrong rach state %x",
                                                        rach_tx_status[loop_index]);
          }
          else
          {
             MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH , "rcvd L1_PRACH_ACK from L1");
             
            /* Send sucess indication to RRC */
             if (mac_rach_ccch_data_presence)
             {
               mac_send_tx_status_ind_to_rrc(RACH_TX_SUCCESS);
             }
        
             /* Reset RACH */
             mac_reset_rach();
          }
          break;

#ifdef FEATURE_CELL_FACH_MEAS_OCCASION
          case L1_PRACH_DELAY:
#else
          case L1_PRACH_WRONG_TTI:
#endif
          /* RACH is not waiting for status */
          if (rach_state != RACH_WAITING_FOR_STATUS)
          {
              MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH , "rcvd Tx status %x in wrong rach state %x",
                                                          rach_tx_status[loop_index]);
          }
          else
          {
#ifdef FEATURE_INTER_RAT_CELL_FACH_RE_SELECTION
            MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH , "rcvd L1_PRACH_DELAY from L1");
#else
            MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH , "rcvd L1_PRACH_WRONG_TTI from L1");
#endif
            /* Set RACH state to RCVD_NO_ACK */
            rach_state = RACH_RCVD_NOACK;
          }
          break;

          /* Received TX Status is NO ACK */
          case L1_PRACH_NO_ACK:

         /* RACH is not waiting for status */
          if (rach_state != RACH_WAITING_FOR_STATUS)
          {
            MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH , "rcvd Tx status %x in wrong rach state %x",
                                                        rach_tx_status[loop_index]);
          }
          else
          {
            MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH , "rcvd L1_PRACH_NO_ACK from L1");
            
            /* Preamble Ramping cycles exceeded maximum count */
            if (npreamble_cycles >= rach_cntl_info.mmax)
            {
            
               MSG_2(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH , "Reached max no. of Preamble cycles(%d)",
                                                           npreamble_cycles,
                                                           rach_cntl_info.mmax);

             /* Send CMAC_STATUS_IND to RRC */
             if (mac_rach_ccch_data_presence)
             {
              mac_send_tx_status_ind_to_rrc (MAX_RACH_CYCLES_REACHED);
             }
             /* Reset MAC */
              mac_reset_rach();
            }
            else
            {
              /* Set RACH state to RCVD_NO_ACK */
              rach_state = RACH_RCVD_NOACK;

#ifdef FEATURE_QCHAT
              if (qchatupk_wcdma_asc_enabled())
              {
                qchatupk_mac_rach_noack(npreamble_cycles);
              }
#endif
            }
          }
          break;

          /* Received TX Status is NACK */
          case L1_PRACH_NAK:
          /* RACH is not waiting for status */
          if (rach_state != RACH_WAITING_FOR_STATUS)
          {
             MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH , "rcvd Tx status %x in wrong rach state %x",
                                                         rach_tx_status[loop_index]);
          }
          else
          {

            MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH , "rcvd L1_PRACH_NAK from L1");

            /* Preamble Ramping cycles exceeded maximum count */
            if (npreamble_cycles >= rach_cntl_info.mmax)
            {

              MSG_2(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH , "Reached max no. of Preamble cycles(%d)",
                                                          npreamble_cycles,
                                                          rach_cntl_info.mmax);

             /* Send CMAC_STATUS_IND to RRC */
             if (mac_rach_ccch_data_presence)
             {
              mac_send_tx_status_ind_to_rrc (MAX_RACH_CYCLES_REACHED);
             }
             /* Reset MAC */
              mac_reset_rach();
            }
            else
            {

              /* Set RACH state to Backoff */
              rach_state = RACH_IN_BACKOFF;

              /* set backoff counter value */

              backoff_val =
                 ran_dist(mac_fetch_random_number(), (uint16)rach_cntl_info.nbo1min, (uint16)(rach_cntl_info.nbo1max+1));

              MSG_3(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "Backoff_Val %d, lo_val %d, hi_val %d",
                                                         backoff_val,
                                                         rach_cntl_info.nbo1min,
                                                         rach_cntl_info.nbo1max);

#ifdef FEATURE_QCHAT
              if (qchatupk_wcdma_asc_enabled())
              {
                qchatupk_mac_rach_nack(npreamble_cycles);
              }
#endif
            }
          }
          break;

        /* Received Abort Request from L1 */
        case L1_PRACH_ABORT:
        if(num_tx_tbs > 0)
        {
          MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "Req RLC to re-send status report for ntbs %d",
                                                     num_tx_tbs);
          rlc_enh_indicate_ul_data_trash();
          mac_ul_rach_abrt_rcvd = TRUE;
        }
        {

          MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH , "rcvd L1_PRACH_ABORT from L1");

          /* Send CMAC_STATUS_IND to RRC */
          if (mac_rach_ccch_data_presence)
          {
            mac_send_tx_status_ind_to_rrc (RACH_TX_ABORTED);
          }
          /* Reset MAC */
           mac_reset_rach();
         }
         break;

         /* Invalid TX Status */
         default:
            MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH , "rcvd invalid Tx status (%d)",
                                                        rach_tx_status[loop_index]);
          break;
        }
      }
      else
      {
        MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "RACH is in IDLE State - Not expected response %d",
                                                   rach_tx_status[loop_index]);
      }
      
      rach_tx_status[loop_index] = L1_PRACH_MAC_INVALID;
     
    }
  }
  
  REX_ISR_UNLOCK(&r99_ul_isr_l2_datapath_mutex);
}

/*================================================================================================
FUNCTION NAME rach_tx_control

DESCRIPTION   This function controls RACH transmission and is called once every TTI
================================================================================================*/
boolean   rach_tx_control
  (
  void
  )
{
  /* Initially set OK to transmit flag to FALSE */
  boolean   rach_tx_is_ok = FALSE;

  /* RACH Tx Control is performed based on RACH state */
  switch (rach_state)
  {

  /* RACH in BACOFF state */
  case RACH_IN_BACKOFF:

    /* If backoff timer has expired, check the persistence */
    if (backoff_val == 0)
    {
      MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "Backoff Tmr expired");

      /* If persistence check has failed, set RACH state to RACH_PERSISTENCE_FAILED
         and UE is not allowed to transmit on RACH */
      if (persistence_is_ok() == TRUE)
      { /* Check if this is the right TTI to transmit */
        if (current_tti >= oc_ul_tfcs_ptr->tfs_info[0].tti_info)
        {
          rach_tx_is_ok = TRUE;
          MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "Found right TTI to transmit : cur_tti %d",
                                                     current_tti);
        }
        else
        {
          rach_state = RACH_WAITING_FOR_RIGHT_TTI;
          MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "RACH_WAITING_FOR_RIGHT_TTI to transmit : cur_tti %d",
                                                     current_tti);
        }
      }
      else
        rach_state = RACH_PERSISTENCE_FAILED;
    }
    else
    {   /*Update backoff timer value */
      MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "Backoff Val:%d",
                                                 backoff_val);
      backoff_val--;
    }
    break;

    /* RACH in RCVD_NO_ACK state or PERSISTENCE FAILED state */
  case RACH_WAITING_FOR_RIGHT_TTI:
  case RACH_RCVD_NOACK:
  case RACH_PERSISTENCE_FAILED:
    /* IF the persistence test has passed */
    if (persistence_is_ok () == TRUE)
    {
      /* Check if this is the right TTI to transmit */
      if (current_tti >= oc_ul_tfcs_ptr->tfs_info[0].tti_info)
      {
        rach_tx_is_ok = TRUE;
        MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "Found right TTI to transmit : cur_tti %d",
                                                   current_tti);
      }
      else
      {
        rach_state = RACH_WAITING_FOR_RIGHT_TTI;
        MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "RACH_WAITING_FOR_RIGHT_TTI to transmit : cur_tti %d",
                                                   current_tti);
      }
    }
    else
      rach_state = RACH_PERSISTENCE_FAILED;
    break;

    /* RACH in Waiting for Status mode */
  case RACH_WAITING_FOR_STATUS:
    break;

  default:
    MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH , "invalid RACH state",
                                                rach_state);
    break;
  }

  return(rach_tx_is_ok);
}

/*===============================================================================================
FUNCTION NAME build_rach_tblks

DESCRIPTION   This function builds RACH Transport Blocks
===============================================================================================*/
void  build_rach_tblks
  (
/* Pointer to Build Frame Indication Parameters */
  mac_ul_build_frame_ind_type   *bld_frame_ind_ptr

  )
{

  uint8           selected_tfi;

  if(mac_ul_config_in_progress == TRUE)
  {
    MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_ERROR, "MAC_ERR: UL MAC config in progress - cannot build rach tblks");
    return;
  }
  L2_MUTEX_LOCK(&mac_config_mutex);

  /* Layer 1 Transmit Data Buffer Pointer */
  l1_tx_buf_ptr = bld_frame_ind_ptr->tx_buf_ptr;

  /* RACH in idle state and hence build new Transport blocks */
  if (rach_state == RACH_IN_IDLE)
  {

    /* Build UL Transport Blocks */
    (void)build_ul_tblks (bld_frame_ind_ptr);

    if (l1_tx_buf_ptr->num_trch == 1)
    {

      /* Selected ASC */
      //l1_tx_buf_ptr->asc_num = mac_asc_selection();

      /* Selected TFCI */
      l1_tx_buf_ptr->TFCI = GET_SELECTED_TFCI;

      /* Save Transport Block Info */
      num_tx_tbs = l1_tx_buf_ptr->tx_data[0].num_tb;

      /* Persistence check has failed, set RACH state to RACH_PERSISTENCE_FAILED and */
      /* UE is not allowed to transmit on RACH */
      if (persistence_is_ok() == FALSE)
      {
        rach_state = RACH_PERSISTENCE_FAILED;
        l1_tx_buf_ptr->num_trch = 0;
      }
      /* Persistence check has passed. It is OK to transmit on RACH */
      else
      {

        /* Set preamble ramping cycle count to one */
        npreamble_cycles = 1;

        /* Set RACH state to RACH_WAITING_FOR_STATUS */
        rach_state = RACH_WAITING_FOR_STATUS;

        MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_LOW , "UL Frame Queued on RACH");
      }
    }
  }
  /* OK to transmit in this TTI */
  else if (rach_tx_control())
  {

    /* Set RACH state to RACH_WAITING_FOR_STATUS */
    rach_state = RACH_WAITING_FOR_STATUS;

    /* Increment number of preamble ramping cycle count */
    npreamble_cycles++;

    MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "npreamble_cycles %d",
                                               npreamble_cycles);

    /* Set Active Transport Channel Count to 1 */
    l1_tx_buf_ptr->num_trch = 1;

    /* Selected Transport Format Combination Pointer */
    selected_tfi = oc_ul_tfcs_ptr->tfc_info[GET_SELECTED_TFCI].tfi[0];

    /* IF it is OK to transmit in this TTI then assign the transport blocks
    and the transport block information */
    l1_tx_buf_ptr->tx_data[0].num_tb  = (uint8)oc_ul_tfcs_ptr->tfs_info[0].tf_info[selected_tfi].ntblks;
    l1_tx_buf_ptr->tx_data[0].tb_size = oc_ul_tfcs_ptr->tfs_info[0].tf_info[selected_tfi].tb_size ;

    MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_LOW , "UL Frame Requeued on RACH");
  }

  /* Save Transport Block Info */
  if (l1_tx_buf_ptr->num_trch ==1)
  {
    num_tx_tbs = l1_tx_buf_ptr->tx_data[0].num_tb;

    /* Few contents of L1 Transmit Buffer are reset by encoder at the end of UL ISR.
       Copying them as they are used while pend status checking during RACH ABORT */
    WCDMA_MEMCPY(&mac_rach_tx_buf, 
                 sizeof(l1_ul_tx_data_type),
                 l1_tx_buf_ptr, 
                 sizeof(l1_ul_tx_data_type));

    MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "MAC_RACH: updating mac_rach_tx_buf--num_tx_tbs %d",
                                               num_tx_tbs);
  }

  if(rach_state == RACH_WAITING_FOR_STATUS)
  {   
    mac_ul_rach_abrt_rcvd = FALSE;
  }
  L2_MUTEX_UNLOCK(&mac_config_mutex);
}

/*===============================================================================================
FUNCTION NAME mac_ul_check_rach_trch

DESCRIPTION   This function checks if the input rlc id and rb id pair map to a RACH transport channel
===============================================================================================*/

log_wcdma_trch_type mac_ul_check_rach_trch(rlc_lc_id_type rlc_id, rb_id_type rb_id)
{
  log_wcdma_trch_type return_trch_val = WCDMA_TRCH_INVALID;
  ul_lc_info_type *int_lc_ptr;

  /* Get the logical channel ptr corresponding to the rlc_id  */
  int_lc_ptr = GET_UL_LC_INFO_PTR(rlc_id);

  if(int_lc_ptr == NULL)
  {
    MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_ERROR, "mac_ul_check_rach_trch: GET_UL_LC_INFO_PTR returns NULL");
  }
  /* Check if given RB ID mactch logical channel's RB ID */
  else if(int_lc_ptr->rb_id == rb_id)
  {
    return_trch_val = WCDMA_TRCH_RACH;
  }
  else
  { 
    MSG_2(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "mac_ul_check_rach_trch: rrc_rb_id (%d) does not match l2_rb_id",
                                               rb_id, int_lc_ptr->rb_id);
  }

  return return_trch_val;
}

/*=========================================================================
FUNCTION MAC_UL_RACH_CHECK_FOR_BFI_FAILURE

DESCRIPTION  
  This routine is called by L1 when L2 has data in buffer but fails to build PDU during 
  build frame indication event.

DEPENDENCIES

RETURN VALUE
None

SIDE EFFECTS
  None
=========================================================================*/
void mac_ul_rach_check_for_bfi_failure(void)
{

  if ((rach_state == RACH_IN_IDLE) &&
      (FALSE == block_transmission) &&
      (FALSE == mac_ul_config_in_progress))
  {
    MSG_3(MSG_SSID_WCDMA_MAC, MSG_LEGACY_ERROR, "MAC_ERR: PDU building failed due to RACH_STATE %d or block_transmission %d or mac_config %d",
                                                rach_state,
                                                block_transmission,
                                                mac_ul_config_in_progress);

  }
  else
  {
    MSG_3(MSG_SSID_WCDMA_MAC, MSG_LEGACY_ERROR, "MAC_ERR:NA PDU building failed due to RACH_STATE %d or block_transmission %d or mac_config %d",
                                                rach_state,
                                                block_transmission,
                                                mac_ul_config_in_progress);

  }
}

/*===============================================================================================
FUNCTION NAME mac_fetch_random_number

DESCRIPTION Based on featurization, it calls ran_next() or secapi_get_random()
            secapi_get_random() has better cryptographic properties

RETURNS     uint32 Random Number value
===============================================================================================*/
uint32 mac_fetch_random_number(void)
{
  uint32 rand_num = 0;
  secerrno_enum_type  errorVal;
  
  /* secapi_get_random() function, declared in secapi.h, fetches random data from its internal 
     random buffer which is in turn filled from PRNG HW */
  errorVal = secapi_get_random(SECAPI_SECURE_RANDOM, (uint8*)&rand_num, sizeof(rand_num));
  
  if(errorVal != E_SUCCESS)
  {
     ERR_FATAL("RACH: Random Number was not generated properly, errorVal %d",(int)errorVal, 0, 0);
  } 
  return rand_num;
}

#ifdef FEATURE_WCDMA_HS_RACH

/*===============================================================================================
 FUNCTION NAME  : mac_hsrach_srb_rxd_manage

 DESCRIPTION    :  * If there is any data on HSRACH in UL, Call the RxD FACH function 
                   * which enables the RxD (if not yet enabled) and manages FMO 
===============================================================================================*/
void mac_hsrach_srb_rxd_manage(boolean data_present_on_srb)
{
  if (mac_hs_rach_info.hs_rach_active == TRUE)
  {
    if (data_present_on_srb == FALSE)
    {
      if (mac_hs_rach_rxd_enable == TRUE)
      {
        mac_hs_rach_rxd_enable = FALSE;
        MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_ERROR, "MAC_HS_RACH: Call RxD FACH Disable as SRB data NOT available");
        wl1_rxd_fach_signalling_data_available(FALSE);
      }
    }
    else
    {
      if (mac_hs_rach_rxd_enable == FALSE)
      {
        mac_hs_rach_rxd_enable = TRUE;
        MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_ERROR, "MAC_HS_RACH: Call RxD FACH Enable as SRB data available");
        wl1_rxd_fach_signalling_data_available(TRUE);
      }
    }
  }
}

boolean   mac_hs_rach_persistence_is_ok (uint8  selected_asc)
{
  boolean rv = FALSE;
  uint32  random_num;
  
  random_num = mac_fetch_random_number();
  
  /* Normalize this random number to a value between 0 & 65535 */
  random_num = ran_dist(random_num, (uint16)0, (uint16)65535);
  
  /* If the random value is less than the persistence associated with the
  acess service class that was selected, then persistence test has passed */
  if ((selected_asc < MAX_ASC) && (random_num <= mac_hs_rach_info.hs_rach_cntl_info.asc[selected_asc].pval))
  {
    rv = TRUE;
  }
  
  MSG_3(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "HS_RACH: PASS(1)/FAIL(0) PERSISTENCY %d - random_num %d, Pi %d",
                                             rv,
                                             random_num,
                                             mac_hs_rach_info.hs_rach_cntl_info.asc[selected_asc].pval);
  
  return(rv);
}

uint8 mac_hs_rach_asc_selection(void)
{
  uint8       selected_asc = 0xFF, mlp;

  selected_asc = mac_hs_rach_info.hs_rach_cntl_info.nASC;
  
  if (mac_hs_rach_info.mac_hsrach_logch_tx_type == L1_MAC_HS_RACH_CCCH_TRANS_TYPE)
  {
    /* Select the highest ASC, if the hs_rach_ccch_asc is greater than configured */
    if (mac_hs_rach_info.hs_rach_ccch_asc >= mac_hs_rach_info.hs_rach_cntl_info.nASC)
    {
      selected_asc = mac_hs_rach_info.hs_rach_cntl_info.nASC - 1;
    }
    else
    {
      selected_asc = mac_hs_rach_info.hs_rach_ccch_asc;
    }
  }  
  else if(mac_hs_rach_info.mac_hsrach_logch_tx_type == L1_MAC_HS_RACH_DTCH_DCCH_TRANS_TYPE)
  {
    mlp = mac_hsrach_pick_mlp();
    
    /* Select the highest ASC, if the hs_rach_ccch_asc is greater than configured */
    if (selected_asc > mlp)
    {
      selected_asc = mlp;
    }
    else
    {
      selected_asc = mac_hs_rach_info.hs_rach_cntl_info.nASC - 1;
    }
  }
  
  MSG_3(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "HS_RACH: configured ccch_asc %d, num_asc %d - Selected ASC %d",
                                             mac_hs_rach_info.hs_rach_ccch_asc,
                                             mac_hs_rach_info.hs_rach_cntl_info.nASC,
                                             selected_asc);
  
  return(selected_asc);
}

boolean l1_phy_hs_rach_check_rach_trigger(uint32 * asc_num)
{
  uint8          selected_asc;
  boolean        buffer_staus_not_empty_flag, persistency_result, data_present_on_srb;
  
  persistency_result = FALSE;
  data_present_on_srb = mac_hs_rach_info.data_present_on_srb;

  if (mac_hs_rach_info.hs_rach_active == TRUE) 
  {
    /* If there is a resource REL IND, Complete RACH procedure is cleared and RACH state moved to RACH_IN_IDLE */
    if ((mac_hs_rach_info.mac_hsrach_edch_rsrc_rel_cause_valid == TRUE) &&
        ((mac_hs_rach_info.mac_hsrach_edch_rsrc_rel_cause == WL1_HS_RACH_EDCH_REL_PHYCHAN_EST_FAIL) ||
         (mac_hs_rach_info.mac_hsrach_edch_rsrc_rel_cause == WL1_HS_RACH_EDCH_REL_RL_FAILURE) ||
         (mac_hs_rach_info.mac_hsrach_edch_rsrc_rel_cause == WL1_HS_RACH_EDCH_REL_AGCH_COLL_TIMER_EXPIRY)))
    {      
      mac_hsrach_phychan_estblish_failure_handler();
      MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "HS_RACH: RACH is released on L1 Resource Rel Ind %d",
                                                 mac_hs_rach_info.mac_hsrach_edch_rsrc_rel_cause);
    } 
    else if (hs_rach_state == RACH_IN_IDLE)
    {
      data_present_on_srb = FALSE;
      buffer_staus_not_empty_flag = mac_hsrach_buffer_status(&data_present_on_srb);
      if (buffer_staus_not_empty_flag == TRUE)
      {
        /* If buffers are non empty - 
         *  - Step 1: ASC selection 
         *  - Step 2: Persistency Check 
         */
        selected_asc = mac_hs_rach_asc_selection();
        *asc_num = selected_asc;
        
        if(selected_asc < MAX_ASC)
        {
          persistency_result = mac_hs_rach_persistence_is_ok(selected_asc);
        }
        else
        {
          MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_ERROR, "Number of access service classes reached maximum number of access service classes supported by the UE");
        }
        
        if (persistency_result == TRUE)
        {
          mac_hsrach_srb_rxd_manage(data_present_on_srb);
          npreamble_cycles = 1;
          hs_rach_state = RACH_WAITING_FOR_STATUS;
        } 
        
        #ifdef FEATURE_WCDMA_TRIGGER_BASED_RACH
        num_tti_consec_without_data = 0;
        #endif /*  FEATURE_WCDMA_TRIGGER_BASED_RACH  */
      }
      #ifdef FEATURE_WCDMA_TRIGGER_BASED_RACH
      else
      {  
        mac_check_for_bfi_disable_trigger();
      }
      #endif /* #ifdef FEATURE_WCDMA_TRIGGER_BASED_RACH */          
    }
    else
    {
      if (hs_rach_tx_control() == TRUE)
      {
        hs_rach_state = RACH_WAITING_FOR_STATUS;
	mac_hsrach_srb_rxd_manage(data_present_on_srb);
        persistency_result = TRUE;
        npreamble_cycles++;
      }
      else
      {
        MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "MAC_HS_RACH: not ready to tx hs_rach_state:%d",
                                                   hs_rach_state);
      }
    }
    mac_hs_rach_info.data_present_on_srb = data_present_on_srb;
  }
  else
  {
    MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_ERROR, "HS_RACH: MAC HS RACH is NOT ACTIVE");
  }
  
  return (persistency_result);
}

void l1_mac_hsrach_preamble_proc_status(l1_hsrach_status_enum_type status)
{
   uint8                       loop_index;
   mac_hs_ul_logical_ch_mac_d_flow_info_type *hs_lc_macd_flow_map = NULL;

   L2_MUTEX_LOCK(&e_ul_isr_l2_datapath_mutex);

   hs_rach_tx_status[0] = status;
   hs_lc_macd_flow_map = get_mac_hs_logch_macd_flow_mapping_ptr();
   for(loop_index = 0; loop_index < 2; loop_index++)
   {
     if (hs_rach_tx_status[loop_index] != L1_HSRACH_MAC_INVALID)
     {
       if ((hs_rach_state != RACH_IN_IDLE) && (hs_rach_state == RACH_WAITING_FOR_STATUS))
       {
         if(mac_hs_rach_rxd_enable == TRUE)
         {
           /* Disable RxD if *any* valid HSRACH status is received. 
            * For NAK/NOACK/ABORT, UL would start new HSRACH procedure later
            * where we would enable RxD if there is data pending Tx on SRBs */         
           mac_hsrach_srb_rxd_manage(FALSE);
         }
         /* RACH is waiting for Status from Layer 1 */
         switch (hs_rach_tx_status[loop_index])
         {
           case L1_HSRACH_ACK:
           {
              MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH , "rcvd L1_HSRACH_ACK from L1");
              
              /* Send sucess indication to RRC */
              if ((L1_MAC_HS_RACH_CCCH_TRANS_TYPE == mac_hs_rach_info.mac_hsrach_logch_tx_type) &&
                   (MAC_I_CHECK_SDU_SEG_NOT_PRESENT(hs_lc_macd_flow_map,mac_hs_rach_info.mac_hsrach_ccch_rlc_id))) 
              {
                mac_send_tx_status_ind_to_rrc(RACH_TX_SUCCESS);
              }

              mac_reset_rach();
           }
           break;
 
           case L1_HSRACH_NO_ACK:
           {
             MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH , "rcvd L1_HSRACH_NO_ACK from L1");
 
             /* Preamble Ramping cycles exceeded maximum count */
             if (npreamble_cycles >= mac_hs_rach_info.hs_rach_cntl_info.mmax)
             {
               MSG_2(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH , "Reached max no. of Preamble cycles(%d)",
                                                           npreamble_cycles,
                                                           mac_hs_rach_info.hs_rach_cntl_info.mmax);
 
              /* Send CMAC_STATUS_IND to RRC */
               if (L1_MAC_HS_RACH_CCCH_TRANS_TYPE == mac_hs_rach_info.mac_hsrach_logch_tx_type)
               {
                 mac_send_tx_status_ind_to_rrc (MAX_RACH_CYCLES_REACHED);
               }
               mac_reset_rach();
               mac_hs_ul_clear_rlc_id_partial_info(mac_hs_rach_info.mac_hsrach_ccch_rlc_id);
               rlci_enh_ul_hsrach_flush_tm_pdus(mac_hs_rach_info.mac_hsrach_ccch_rlc_id);//Flush the RLC CCCH DSM item]
             }
             else
             {
               /* Set RACH state to RCVD_NO_ACK */
               hs_rach_state = RACH_RCVD_NOACK;
             }
           }
           break;
 
           case L1_HSRACH_NACK:
           {
 
             MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH , "rcvd L1_HSRACH_NAK from L1");
 
             /* Preamble Ramping cycles exceeded maximum count */
             if (npreamble_cycles >= mac_hs_rach_info.hs_rach_cntl_info.mmax)
             {
 
               MSG_2(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH , "Reached max no. of Preamble cycles(%d)",
                                                           npreamble_cycles,
                                                           mac_hs_rach_info.hs_rach_cntl_info.mmax);
 
              /* Send CMAC_STATUS_IND to RRC */
              if (L1_MAC_HS_RACH_CCCH_TRANS_TYPE == mac_hs_rach_info.mac_hsrach_logch_tx_type)
              {
                mac_send_tx_status_ind_to_rrc (MAX_RACH_CYCLES_REACHED);
              }
              /* Reset MAC */
               mac_reset_rach();
               mac_hs_ul_clear_rlc_id_partial_info(mac_hs_rach_info.mac_hsrach_ccch_rlc_id);
               rlci_enh_ul_hsrach_flush_tm_pdus(mac_hs_rach_info.mac_hsrach_ccch_rlc_id);//Flush the RLC CCCH DSM item
             }
             else
             {
 
               /* Set RACH state to Backoff */
               hs_rach_state = RACH_IN_BACKOFF;
 
               /* set backoff counter value */
 
               backoff_val =
                  ran_dist(mac_fetch_random_number(), (uint16)mac_hs_rach_info.hs_rach_cntl_info.nbo1min, (uint16)(mac_hs_rach_info.hs_rach_cntl_info.nbo1max + 1));
 
               MSG_3(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "Backoff_Val %d, lo_val %d, hi_val %d",
                                                          backoff_val,
                                                          mac_hs_rach_info.hs_rach_cntl_info.nbo1min,
                         mac_hs_rach_info.hs_rach_cntl_info.nbo1max);
 
             }
           }
           break;
 
         case L1_HSRACH_ABORT:
          {
            MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH , "rcvd L1_HSRACH_ABORT from L1");
            rlc_enh_indicate_ul_data_trash();            
            mac_reset_rach();
          }
          break;
 
          default:
             ERR_FATAL ("rcvd invalid Tx status (%d)", hs_rach_tx_status[loop_index], 0, 0);
           break;
         }
       }
       else
       {
         MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_ERROR, "MAC_HS_RACH: RACH is either in IDLE or not in WAITING for response State - Not expected response %d",
                                                     hs_rach_tx_status[loop_index]);
       }
       
       hs_rach_tx_status[loop_index] = L1_HSRACH_MAC_INVALID;
      
     }
   }

   L2_MUTEX_UNLOCK(&e_ul_isr_l2_datapath_mutex);
}

/*================================================================================================
FUNCTION NAME rach_tx_control

DESCRIPTION   This function controls RACH transmission and is called once every TTI
================================================================================================*/
boolean   hs_rach_tx_control(void)
{
  boolean   rach_tx_is_ok = FALSE;
  uint8     selected_asc = 0;

  
  switch (hs_rach_state)
  {
    case RACH_IN_BACKOFF:
      if (backoff_val == 0)
      {
        MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "HS_RACH: Backoff Tmr expired");
        selected_asc = mac_hs_rach_asc_selection();
        if ((selected_asc < MAX_ASC) && (mac_hs_rach_persistence_is_ok(selected_asc)) == TRUE)
        { 
          rach_tx_is_ok = TRUE;          
        }
        else
        {
          hs_rach_state = RACH_PERSISTENCE_FAILED;
        }
      }
      else
      {   
        MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "HS_RACH: Backoff Val %d > 0",
                                                   backoff_val);
        backoff_val--;
      }
      break;
    
    case RACH_RCVD_NOACK:
    case RACH_PERSISTENCE_FAILED:
      selected_asc = mac_hs_rach_asc_selection();
      if ((selected_asc < MAX_ASC) && (mac_hs_rach_persistence_is_ok(selected_asc)) == TRUE)
      {
        rach_tx_is_ok = TRUE;
      }
      else
      {
        hs_rach_state = RACH_PERSISTENCE_FAILED;
      }
      break;
    
    case RACH_WAITING_FOR_STATUS:
      break;
    
    default:
      MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH , "HS_RACH: Invalid RACH state %d",
                                                  hs_rach_state);
      break;
  }

  return(rach_tx_is_ok);
}
#endif
