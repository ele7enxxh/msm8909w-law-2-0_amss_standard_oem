/*============================================================================================

                                U P L I N K   D A T A   H A N D L E R


GENERAL DESCRIPTION

  This module contains the functions needed to process the UL MAC data

Copyright (c) 2001 by Qualcomm Technologies, Inc.  All Rights Reserved.
Copyright (c) 2002-2009 by Qualcomm Technologies, Inc.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*===*===*===*===*===*===*===*/

/*==============================================================================================


                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mac/vcs/maculdata.c_v   1.29   10 Jul 2002 10:05:20   skrishna  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/mac/src/maculdata.c#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when      who     what, whesre, why

--------  ---     -----------------------------------------------------------------------------
12/15/14  as      In FACH state MAC should send CMAC_STATUS_IND only for TM CCCH UL data.
11/10/14  as      Added code change to not to acquire HS-RACH resource and also to trigger polling 
                  in RWS situation.
07/26/14   rkmk    Reduce image size by using right macro depending upon number of parameters
05/19/14  kv      Fixed KW Errors
05/06/14  geg     Use safer versions of memcpy() and memmove()
04/04/14  scv     Code changes for UL Compression Feature
01/09/14  scv     Code changes for UL Compression Feature
12/09/13  as      Code change to EDRX full sleep if there pending partials at MAC. 
11/08/13  as      Added code change not to disable RACH if there is any pending data in DL 
                  that needs to be received which in turn would not trigger EDRX full sleep.
11/20/13  kv      Moved mask for MSG_4to9 from MSG_SSID_DIAG to MSG_SSID_WCDMA_MAC
11/15/13  as      Added code change to provide SRB data presence & corresponding TTI information to
                  L1 which in turn would increase the Tx priority for W during SRB data transmission.
05/15/13  as      Fix to use correct RB-ID to pend status at RLC during RACH ABORT
02/19/13  kc      Fix to pend status only if trashed pdu is RLC AM CTRL pdu
02/14/13  pr      Backed out previous fix. 
02/14/13  scv     Fix for MAC to consider the RESET PENDING State while reporting data on RACH to UL
02/07/13  as      Added debug code to crash if L2 has returned true for pending data in 
                  buffer but fails during PDU build event when UE is in RACH state.
02/05/13  at      L2-L1 interface added to provide SRB data info to Gate FET when UL DCCH data is present
07/06/12  as      Fix to avoid concurency issue of L2 accessing WM during BO in ISR context and 
                  WM deregisteration happening in L2 conetxt.
07/17/12  mrg     Moved enable/disable indication of Trigger based RACH from BFI to l1_query_mac_rach_data()
07/24/12  geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
06/19/12  mrg     HS-RACH & MAC-I Feature code integrated
03/16/12  mrg     Added enable/disable indication to RxD whenver SRBs has data present/exhausted in RACH state
                  in the function call l1_query_mac_rach_data()
01/10/12  grk     Profiling support for RLC PDU building and MAC TFCI selection.
03/09/12  mrg   Added an API to provide buffer status is empty or not when L1 queries MAC
01/02/12  grk     Feature cleanup
01/23/12  kc      Changes for free floating tasks
01/09/12  sk      MAC changes to support RxD in FACH
12/21/11  mrg     R99 profiling support added in MAC
10/22/11   sk     NikeL UL CM workaround under feature FEATURE_WCDMA_UL_CM_INTLV_WORKAROUND
08/26/11  grk     Fix for KW errors
06/28/10  mrg     MAC-R99 ZI memory reduction changes
06/11/10  mrg     Lint errors fixed
10/07/09  mrg     Fixed Klocwork warnings
07/08/09  grk     Modified cipher algo checks in accordance with new 
                  ciphering algo enum (added as part of snow3g support).
06/22/09  mrg     ctrl_data_bytes includes rlc_hdr_lenght also, while computing
                  npdus take care of this.in both build_ul_tblk() and  in 
                  compute_rlc_frame_format_based_on_priorities() funcitons
05/29/09  ssg     Added support for SNOW_3G algo.
05/11/09  mrg     ctrl_data_bytes includes rlc_hdr_lenght also, while computing
                  npdus take care of this.
05/07/09  ssg     Updated Copyright Information
04/29/09  av      Fixed compiler warnings in L2 
01/23/08  ssg     Reverting “rw/zi” optimizations. 
12/16/08  mrg     “rw” and “zi” optimization changes. 
10/06/08  grk     Lint warning fixes.
09/02/08  cnp     Replaced intlock/free with wcdma_intlock/free.
          cnp     Replaced tasklock/free with wcdma_tasklock/free.
          cnp     Replaced intlock/free_sav with wcdma_intlock/free_sav.
09/05/07  sk      Used the rlc_ul_enh_buffer_status fn under FEATURE_RLC_ENH_BUFFER_STATUS
02/23/07  sk      Used the UE_MAX_UL_RLC_PDUS and L1_UL_MAX_TB_SIZE to do the checks awhile
                  the data is being sent to L1. UE_MAX_UL_RLC_PDUS is changed from 16 to 24
08/14/06  sk      Added HSUPA support
03/02/06  sk      In attach_mac_paramaters() - Populate structre which stores information
                  about the channels on which signalling data was transmitted under feature
                  FEATURE_WCDMA_LAST_UL_SRB_DATA_REC
05/06/05  sk      In function build_ul_tblks(), call l1 function l1_is_sub_chan_avail_for_asc()
                  and mac_asc_selection for only RACH state.
05/05/05  sk      In function build_ul_tblks(), call l1 function l1_is_sub_chan_avail_for_asc()
                  to query L1 if for that particular ASC, there is any available subchannel or not.
                  Based on the status rlc function would be called to build the pdus.
03/09/05  sk      Fixed all lint errors.
12/22/04  sk      Fill the ciphering parameters only if the cfg_type is not SMC_INITIATED_NON_HO_CIPHER
09/10/04  sk      Added function l1_phy_ul_cfn_missed_ind() which will be called up in L1 inteerupt
                  context on detecting that it missed CFN updates due to some higher layer doing
                  WCDMA_INTLOCK for more than 10ms.This function will initialize the state variables
                  related to TFCS selection, in particular to the transport format index which
                  is reset for the transport channels, so that one getting the build frame indication
                  subsequently, UE UL would be able to recover.
08/18/04  sk      Changed the restirction on num_tb and tb_size  as 16 and 5120 respectively.
04/14/04  sk      Initialized last_tm_idx to 0.
03/21/04  sk      Included "err.h" to remove compiler errors.
03/11/04  sk      Added a check to see if the Number of tb's or the tb size is out of bounds.
                  If so err_fatal. Err_fatal is added to see why the tb_size or the num_tb is becoming
                  so large.
02/26/04  sk      Fix for CR40299:
                  In attach_mac_parameters(): FEATURE_WCDMA_L1_TM_CIPHER_SUPPORTED explicitly
                  mark the last TM pdu asL1_TB_TM_CIPHER_STOP when there are more than 1 TM pdu.
10/28/03  sk      In attach_mac_parameters() : If ciphering is enabled, check
                  if the algorithm to be used is UE_1, if yes, then perform ciphering.
09/04/03  sk      In attach_mac_parameters() : Added MSG_ERROR to identify if there is a mismatch
                  in number of PDUS that MAC asks RLC to build and the actualy number specified
                  in the TFCS table.
08/06/03  sk      Set the ciphering flag to FALSE in the case of 1x0 transport format.
01/31/03  sk    Populated tb_cipher_state under FEATURE_WCDMA_L1_TM_CIPHER_SUPPORTED
12/08/02  sk    Ciphering done for DCH &RACH
10/23/02  sk    Populating ciphering_key_id for each pdu as the key_id can be different for each
                id.
10/18/02  sk    Added UE_MODE_ACKNOWLEDGED_DATA in switch statement for attach_mac_parameters().
09/26/02  sk    Check to see if the logical channel is enabled before you start ciphering.
09/04/02  sk    attach_mac_parameters() : Added code for ciphering. Ciphering in MAC
                is done for only Transparent mode logical channels. Pass on the
                ciphering information as is passed by RLC for UM and AM mode to L1.
07/08/02  sk    build_ul_tblks: Call proc_block_dtch_tx_on_rach() to appropraitely block DTCH
                transmissions on RACH.
06/06/02  sk    (mks) build_ul_tblks: Added function build_ul_dummy_dch_tblk() builds a dummy
                UL TB under FEATURE_DUMMY_SIGNALLING
02/12/02  ps    Added SRB Delay related code with FEATURE_L1_SRB_DELAY feature definition
11/03/01  ps    Modified "attach_mac_parameters" and "build_ul_tblks" to fix DTX problem
10/18/01  ps    attach_mac_parameters() : Added code to account for the DTX case where TB Size =0 and
                number of Transport Blocks is non-zero
10/05/01  ps    deleted the following functions (never used) :
                - log_ul_mac_pdus()
                - log_ul_rlc_pdus()
                - log_ul_rlc_frame_fmt()
                build_ul_tblks () : Removed the calls to the above functions
09/18/01  ps    attach_mac_parameters() : Added code to fix multiple TB problem
                build_ul_tblks() : Added code to set the prev_tx_flag
08/07/01  ps    Fixed the leading zero message formatting problem
07/01/01  ps    Removed "bld_frame_ind_ptr->tx_buf_ptr->num_trch = 0" statement from "build_ul_tblks()"
06/18/01  ps    Adjusted the MSG levels

==============================================================================================*/


/*==============================================================================================
                     INCLUDE FILES
==============================================================================================*/
#include "wcdma_variation.h"
#include  "customer.h"
#include  "rex.h"
#include  "msg.h"
#include  "dsm.h"
#include  "macrrcif_v.h"
#include  "macrlcif.h"
#include  "l1sapcommon.h"
#include  "l1macdata.h"
#include  "macinternal.h"
#include  "l2ultask.h"
#include  "macultfcs.h"
#include  "macul.h"
#include  "err.h"
#include "l1macif.h"
#include "macul.h"
#include  <string.h>
#ifdef FEATURE_DUMMY_SIGNALING
  #error code not present
#endif
#include "mactraffic.h"

#include "l1mathutil.h"
#include "maculdata.h"
#include "macrlcif.h"


/* Uplink RLC buffer status buffer */
rlc_ul_buf_status_type    ul_buf_status;

/* UL RLC Frame Format information */
rlc_ul_frame_format_type  ul_rlc_frame_fmt;

#ifndef FEATURE_WCDMA_MAC_UL_ZI_REDUCTION
/* UL RLC PDU Set */
rlc_ul_frame_data_type    rlc_ul_data_info;
#else
rlc_ul_frame_data_type    *rlc_ul_data_info_ptr;
#endif /* FEATURE_WCDMA_MAC_UL_ZI_REDUCTION */

rlc_ul_frame_data_type    *rlc_ul_data_ptr;

boolean mac_rach_rxd_enable = FALSE;

/* Current TTI */
extern  l1_tti_enum_type    current_tti;

/* RACH State */
extern rach_state_e_type rach_state;

/*---------------------------------------------------------------------------
  Contains the list of logical channels for which MAC needs the buffer status
---------------------------------------------------------------------------*/
rlc_ul_lc_info_type              mac_ul_lc_info;

/* Profiling samples variables to capture the snapshot of the times stamps*/
l2_rlc_profile_data_struct_type rlc_tfc_profile_dump;

static int32 frame_qty, cx8_qty;
l2_profile_data_struct_type l2_profile_dump[MAC_NUM_PROFILE_SAMPLES];
uint32 l2_profile_idx = 0;
boolean l2_print_profile_data = FALSE;

#ifdef FEATURE_WCDMA_TRIGGER_BASED_RACH
extern uint32   num_tti_consec_without_data;
#endif


/*===============================================================================================
 FUNCTION NAME  : mac_profile_dump_start

 DESCRIPTION    : This will note down the time stamp of the fucntion was started

===============================================================================================*/

static void mac_profile_dump_start(void)
{
  wl1_get_profile_point(&frame_qty, &cx8_qty);
  l2_profile_dump[l2_profile_idx].frame_qty_start = frame_qty;
  l2_profile_dump[l2_profile_idx].cx8_qty_start = cx8_qty;
}


/*===============================================================================================
 FUNCTION NAME  : mac_profile_dump_end

 DESCRIPTION    : This will note down the time stamp of the fucntion was ended.

===============================================================================================*/

static void mac_profile_dump_end(void)
{
  uint8 loop_index =0;
  wl1_get_profile_point(&frame_qty, &cx8_qty);
  l2_profile_dump[l2_profile_idx].frame_qty_end = frame_qty;
  l2_profile_dump[l2_profile_idx].cx8_qty_end = cx8_qty;
  l2_profile_idx++;
	
  if(l2_profile_idx >= MAC_NUM_PROFILE_SAMPLES)
  {
    l2_profile_idx = 0;

    if (l2_print_profile_data)
    {
  	 for(loop_index = 0; loop_index > (MAC_NUM_PROFILE_SAMPLES/4); loop_index++)
  	 {
  	   MSG_8(MSG_SSID_DFLT, MSG_LEGACY_HIGH,
  		   "mac_profile_dump: (%d, %d), (%d, %d), (%d, %d), (%d, %d),",
  		   l2_profile_dump[loop_index].cx8_qty_start, l2_profile_dump[loop_index].cx8_qty_end,
  		   l2_profile_dump[loop_index+1].cx8_qty_start, l2_profile_dump[loop_index+1].cx8_qty_end,
   		   l2_profile_dump[loop_index+2].cx8_qty_start, l2_profile_dump[loop_index+2].cx8_qty_end,
   		   l2_profile_dump[loop_index+3].cx8_qty_start, l2_profile_dump[loop_index+4].cx8_qty_end
  		 );
  	 }
    }
	 
  }
}

/*===============================================================================================
 FUNCTION NAME  : mac_rach_srb_rxd_manage

 DESCRIPTION    :  * If there is any data on RACH in UL, Call the RxD FACH function 
                   * which enables the RxD (if not yet enabled) and manages FMO 
===============================================================================================*/
void mac_rach_srb_rxd_manage(boolean data_present_on_srb)
{
  
  if (GET_UL_CCTRCH_TYPE == RACH)
  {
    if (data_present_on_srb == FALSE)
    {
      if (mac_rach_rxd_enable == TRUE)
      {
        mac_rach_rxd_enable = FALSE;
        MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_ERROR, "MAC_RACH: Call RxD FACH Disable as RACH-SRB data NOT available");
        wl1_rxd_fach_signalling_data_available(FALSE);
      }
    }
    else
    {
      /* If there is any data on RACH in UL, Call the RxD FACH function 
       * which enables the RxD (if not yet enabled) and manages FMO */
      if (mac_rach_rxd_enable == FALSE)
      {
        mac_rach_rxd_enable = TRUE;
        MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_ERROR, "MAC_RACH: Call RxD FACH Enable as RACH-SRB data available");
        wl1_rxd_fach_signalling_data_available(TRUE);
      }
    }
  }
}

/*===============================================================================================
 FUNCTION NAME  : rlc_buffers_are_not_empty

 DESCRIPTION  : This function checks whether RLC Logical Channel Buffers are empty or not.
          Returns TRUE if the buffers are not empty. FALSE otherwise
===============================================================================================*/
boolean   rlc_buffers_are_not_empty
(
  rlc_ul_lc_info_type *mac_ul_lc_info_ptr,
  boolean            *rlc_wait_for_ack   
)
{
  uint16  i;
  boolean rv;
  rlc_lc_id_type              rlc_id;
  rlc_ul_lc_buf_status_type *lc_status_ptr;
  ul_lc_info_type            *int_lc_ptr = NULL;

  /* function code starts here */
  rv = FALSE;
  
  L2_ACQ_UL_LW_LOCK(UE_MAX_UL_LOGICAL_CHANNEL);
  
  rlc_ul_enh_buffer_status(mac_ul_lc_info_ptr, &ul_buf_status, NON_BFI_NON_TVM_QUERY, rlc_wait_for_ack);
  
  L2_ENH_ACQ_UL_LW_UNLOCK(UE_MAX_UL_LOGICAL_CHANNEL);
  
  /* Go through active logical channel */
  for (i = 0; i < mac_ul_lc_info_ptr->nchan; i++)
  {
    rlc_id = mac_ul_lc_info_ptr->rlc_id[i];
    
    /* Get pointer to Logical Channel Buffer Status */
    lc_status_ptr = &ul_buf_status.chan_info[rlc_id];
    
    lc_status_ptr->nbytes = lc_status_ptr->ctrl_data_bytes + 
                            lc_status_ptr->new_data_bytes + lc_status_ptr->retx_data_bytes;

    int_lc_ptr = GET_UL_LC_INFO_PTR(rlc_id);
    
    if ((int_lc_ptr != NULL) && (int_lc_ptr->rlc_mode >= UE_MODE_ACKNOWLEDGED) && 
        (int_lc_ptr->rlc_mode <= UE_MODE_ACKNOWLEDGED_CONTROL) &&
        (lc_status_ptr->effect_win_size == 0))
    {
      lc_status_ptr->nbytes -= lc_status_ptr->new_data_bytes;
    }
    if ((lc_status_ptr->nbytes > 0) || (lc_status_ptr->new_data_bytes > 0))
    {
      MSG_7(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH,"RLC %d: nBytes %d, ctrl_data %d, new_data %d, retx_data %d, un_comp %d win_size %d",
            rlc_id, lc_status_ptr->nbytes, lc_status_ptr->ctrl_data_bytes,lc_status_ptr->new_data_bytes,
            lc_status_ptr->retx_data_bytes, lc_status_ptr->new_data_uncomp_bytes, lc_status_ptr->effect_win_size);
    }
    
    /* Buffer Status Type */
    switch (lc_status_ptr->type)
    {
      /* FIXED PDUs */
      case FIXED_PDUS:
        if ((lc_status_ptr->npdus > 0) && (lc_status_ptr->pdu_size > 0))
        {
          MSG_3(MSG_SSID_WCDMA_MAC, MSG_LEGACY_LOW , "RLC%d: pduSize=%d, nPDUs=%d",
                                                     rlc_id,
                                                     lc_status_ptr->pdu_size,
                                                     lc_status_ptr->npdus);
          rv = TRUE;
        }
        break;

      /* Flexibled PDUs */
      case FLEX_PDUS:
        if ((lc_status_ptr->nbytes > 0) || ((lc_status_ptr->new_data_uncomp_bytes > 0) && 
            (lc_status_ptr->effect_win_size > 0)))
        {
          MSG_2(MSG_SSID_WCDMA_MAC, MSG_LEGACY_LOW , "RLC%d: nBytes=%d",
                                                     rlc_id,
                                                     lc_status_ptr->nbytes);
          rv = TRUE;
        }
        break;

      default:
        break;
    }

    if(rv == TRUE)
    {
      break;
    }
  }

  return(rv);
}

/*==========================================================================
FUNCTION: L1_QUERY_MAC_RACH_DATA

DESCRIPTION:
This function will be called by L1 to check if L2 has data to send. It will
internally invoke rlc_buffers_are_not_empty() to check RLC Buffer
Occupancy. 

DEPENDENCIES:
None

RETURN VALUE:
TRUE if the buffers are not empty and MAC RACH is not in IDLE state. FALSE otherwise.

SIDE EFFECTS:
==========================================================================*/
extern boolean wl1_ue_in_plt_mode; //atmod
boolean l1_query_mac_rach_data (void)
{
  boolean data_present_on_srb;
  boolean buffer_staus_not_empty_flag = TRUE;
  boolean rlc_wait_for_ack = FALSE;

  #if defined (FEATURE_WCDMA_PLT) || defined (FEATURE_WCDMA_PLT_MODE) //atmod
  #error code not present
#endif

  /* If RACH is in IDLE state, calculate the BO.
   *   Return TRUE, if there is a data to be sent out
   *   If data belongs to SRB, Enable/disable the RxD if needed
   * If RACH is NOT in IDLE state,
   *   There is some data pending with MAC. 
   *   Always return TRUE and dont modify the RxD state
   */
  if(rach_state == RACH_IN_IDLE)
  {
    data_present_on_srb = FALSE;
    buffer_staus_not_empty_flag = rlc_buffers_are_not_empty(&(mac_ul_lc_info), &rlc_wait_for_ack);
    
    if (mac_ul_lc_info.nchan > 0 )
    {
      mac_compute_pdus_from_rlc_status(mac_ul_lc_info.nchan, mac_ul_lc_info.rlc_id, &ul_buf_status, &data_present_on_srb);
    }
    
    mac_rach_srb_rxd_manage(data_present_on_srb);
#ifdef FEATURE_WCDMA_HS_FACH_DRX
    if (TRUE == wl1_edrx_get_edrx_configured_status())
    {
      if (TRUE != buffer_staus_not_empty_flag)
      {
        if (TRUE != rlc_wait_for_ack)
        {
          buffer_staus_not_empty_flag = mac_rlc_check_pending_dl_data();
        }
        else
        {
          MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "MAC_EDRX_FS: L2 UL is waiting for ACK");
          buffer_staus_not_empty_flag = TRUE;
        }
      }
    }
#endif
    #ifdef FEATURE_WCDMA_TRIGGER_BASED_RACH
    if(buffer_staus_not_empty_flag == TRUE)
    {
      num_tti_consec_without_data = 0;
    }
    else
    {
      mac_check_for_bfi_disable_trigger();
    }
    #endif /* #ifdef FEATURE_WCDMA_TRIGGER_BASED_RACH */
  }
  else if(rach_state == RACH_WAITING_FOR_STATUS)
  {
    buffer_staus_not_empty_flag = FALSE;
  }
  
  return buffer_staus_not_empty_flag;
}

/*===============================================================================================
 FUNCTION NAME  : mac_ignore_ul_srbs

 DESCRIPTION  : This function clears the buffer status of UL Signal Radio Bearers
===============================================================================================*/
void    mac_ignore_ul_srbs
(
/* Uplink Buffer Status Pointer */
rlc_ul_buf_status_type        *rlc_ul_buf_status
)
{
  uint16  i;

  /* Pointer to UL Logical channel info */
  ul_lc_info_type           *lc_info_ptr;

  /* Pointer to Logical Channel Buffer Status */
  rlc_ul_lc_buf_status_type *lc_status_ptr;

  /* Radio bearer identity */
  rb_id_type                    rb_id;

  /* Go through each logical channel */
  for (i=0; i <UE_MAX_UL_LOGICAL_CHANNEL; i++)
  {

    /* Logical channel information pointer */
    lc_info_ptr = GET_UL_LC_INFO_PTR (i);

    /* Radio Bearer ID */
    rb_id = lc_info_ptr->rb_id;

    /* Logical channel is an SRB */
    if (rb_id <= 4)
    {

      /* Pointer to Logical Channel Buffer Status */
      lc_status_ptr = &rlc_ul_buf_status->chan_info[i];

      /* Buffer Status Type */
      switch (lc_status_ptr->type)
      {

      /* FIXED PDUs */
      case FIXED_PDUS:
        lc_status_ptr->npdus = 0;
        lc_status_ptr->pdu_size = 0;
        break;

        /* Flexibled PDUs */
      case FLEX_PDUS:
        lc_status_ptr->nbytes = 0;
        break;

      default:
        break;
      }
    }
  }
}

/*===============================================================================================
 FUNCTION NAME  attach_mac_parameters

 DESCRIPTION  Attaches MAC parameters to each UL RLC PDU
================================================================================================*/
void  attach_mac_parameters
(
/* RLC Frame Data Pointer */
rlc_ul_frame_data_type  *rlc_frame_data_ptr,

/* Shared Transmit Data Buffer Pointer */
l1_ul_tx_data_type    *txbufptr
)
{
  uint8                     i, j;
  rlc_ul_logchan_data_type  *lc_data_ptr;
  l1_ul_trch_data_type      *l1_trch_data_ptr;
  l1_ul_tb_data_type        *tbptr;
  ul_lc_info_type           *lc_info_ptr;
  boolean                   trch_select_flag[UE_MAX_TRCH];
  mac_ul_tfc_type           *oc_tfc_ptr;
  mac_ul_tf_type            *oc_tf_ptr;
  ul_trch_info_type         *int_tc_ptr;

  uint8                     trch_idx;

  uint8                     index;

  uint16                    n_tm_pdus=0;
  uint16                    last_tm_idx=0;

  /* Selected Transport Format Combination Pointer */
  oc_tfc_ptr = &oc_ul_tfcs_ptr->tfc_info[GET_SELECTED_TFCI];
  txbufptr->srb_data_num_tti = 0;
  txbufptr->srb_data_present = FALSE;

  /* Clear Transport Channel Select Flags */
  for (i=0; i <UE_MAX_TRCH; i++)
    trch_select_flag[i] = FALSE;

  /* Number of uplink RLC logical channels */
  for (i=0; i < rlc_frame_data_ptr->nchan; i++)
  {

    /* Pointer to logical channel PDU information */
    lc_data_ptr = &rlc_frame_data_ptr->chan_info[i];

    n_tm_pdus = 0;
    /* Valid RLC ID */
    if (!UL_RLC_ID_IS_INVALID(lc_data_ptr->rlc_id))
    {

      /* Logical channel information pointer */
      lc_info_ptr = GET_UL_LC_INFO_PTR(lc_data_ptr->rlc_id);

      /* get pointer to internal transport channel info */
      int_tc_ptr = GET_UL_TRCH_INFO_PTR(lc_info_ptr->trch_idx);

      /* Pointer to Transport Channel info */
      l1_trch_data_ptr = &txbufptr->tx_data[lc_info_ptr->trch_idx];

      /* TB Size */
      l1_trch_data_ptr->tb_size = (uint16)(lc_info_ptr->mac_hdr_size + lc_data_ptr->pdu_size);

      for (j=0 ;j <lc_data_ptr->npdus; j++)
      {

        /* TB data pointer */
        tbptr = &l1_trch_data_ptr->trch_data[l1_trch_data_ptr->num_tb+j];

        /* Populate fields of transport block info */
        tbptr->data_ptr = (void *)lc_data_ptr->pdu_info[j].pdu_ptr;
        WCDMA_MEMCPY(tbptr->mac_hdr, 
                     sizeof(uint8) * L1_UL_MAC_MAX_HDR_SIZE,
                     lc_info_ptr->mac_hdr_value,
                     sizeof(uint8) * MAC_UL_MAX_HDR_SIZE);

        tbptr->mac_hdr_size = lc_info_ptr->mac_hdr_size;

        if (lc_info_ptr->enable)
        {
          /* In 1x0 case, ciphering is not done */
          if (l1_trch_data_ptr->tb_size == 0)
          {
            tbptr->ciphering_on = FALSE;
          }
          else if (GET_UL_CCTRCH_TYPE != INVALID_UL_CCTRCH )
          {
            tbptr->ciphering_on = FALSE;

            /*---------------------------------------------------------------------------
            Fill in the ciphering parameters if the ciphering is enabled. For TM mode
            radio bearers ciphering is performed in MAC. For UM, AM mode RB's ciphering
            is done in RLC.
            ---------------------------------------------------------------------------*/
            switch (lc_info_ptr->rlc_mode)
            {
            case UE_MODE_TRANSPARENT:
              /* Get the index */
              index = int_tc_ptr->cs_or_ps_domain;

              /*--------------------------------------------------------------------------
              Copy the ciphering parameters related to TM mode. If ciphering is enabled, check
              if the algorithm to be used is UE_1 or UE_2, if yes, then perform ciphering.
              ---------------------------------------------------------------------------*/
              L2_ACQ_CIPHER_PARAM_LOCK();
              if (cur_ul_cipher_info[index].cipher_enable &&
              (cur_ul_cipher_info[i].cipher_cfg_type != SMC_INITIATED_NON_HO_CIPHER_CFG) &&
              ((cur_ul_cipher_info[index].cipher_algo == UE_WCDMA_CIPHER_ALGO_UEA1)
#ifdef FEATURE_WCDMA_REL7
              || (cur_ul_cipher_info[index].cipher_algo == UE_WCDMA_CIPHER_ALGO_UEA2)
#endif
              ))
              {
                if (lc_info_ptr->rb_id != 0)
                {
                  tbptr->ciphering_on = TRUE;
                  tbptr->cipher_algo = (uint32)cur_ul_cipher_info[index].cipher_algo;
                  tbptr->ciphering_key_id = cur_ul_cipher_info[index].key_idx;
                  tbptr->count = count_c[index];
                  tbptr->offset = 0;
                }
              }
              if ((0 == lc_info_ptr->rb_id) && (RACH == GET_UL_CCTRCH_TYPE ))
              {
                
                MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "MAC_RACH: CCCH data present in this RACH TX");
                mac_rach_ccch_data_presence = TRUE;
              }
              tbptr->radio_bearer_id = (uint8)lc_info_ptr->rb_id;
              L2_ACQ_CIPHER_PARAM_UNLOCK();

              /* TM ciphering state */
              tbptr->tm_cipher_state = ((n_tm_pdus==0)?L1_TB_TM_CIPHER_START:L1_TB_TM_CIPHER_CONT);
              n_tm_pdus++;
              last_tm_idx=j;

              break;
            case UE_MODE_ACKNOWLEDGED:
            case UE_MODE_ACKNOWLEDGED_DATA:
            case UE_MODE_UNACKNOWLEDGED:
              if (lc_data_ptr->cipher_flag)
              {
                if (lc_data_ptr->pdu_info[j].pdu_cipher_enable)
                {
                  tbptr->offset = lc_data_ptr->cipher_offset;
                  tbptr->ciphering_key_id = (uint8)lc_data_ptr->pdu_info[j].cipher_key_id;
                  tbptr->cipher_algo = lc_data_ptr->pdu_info[j].cipher_algo;
                  tbptr->count = lc_data_ptr->pdu_info[j].count_c;
                  tbptr->ciphering_on=TRUE;
                }

              }
              tbptr->radio_bearer_id = (uint8)lc_data_ptr->rb_id;
              /* TM ciphering state */
              tbptr->tm_cipher_state = L1_TB_TM_CIPHER_NOT_APPLICABLE;

              break;
            default:
              break;
            } /* switch rlc mode */
          } /* Trch Type is RACH or DCH */

        }// logical channel is enabled

        /* Set transport channel select flag */
        trch_select_flag[lc_info_ptr->trch_idx] = TRUE;
      }

      /* Get the last TB data pointer and update its cipherign state information */
      tbptr = &l1_trch_data_ptr->trch_data[l1_trch_data_ptr->num_tb+last_tm_idx];

      if (n_tm_pdus == 1)
      {
        tbptr->tm_cipher_state = L1_TB_TM_CIPHER_SINGLE;
      }
      else if (n_tm_pdus > 1)
      {
        tbptr->tm_cipher_state = L1_TB_TM_CIPHER_STOP;

      }

      /* Update Transport Channel TB count value */
      l1_trch_data_ptr->num_tb = (uint8)(l1_trch_data_ptr->num_tb + lc_data_ptr->npdus);

      trch_idx = lc_info_ptr->trch_idx;

      /* Transport Format Pointer */
      oc_tf_ptr=
      &oc_ul_tfcs_ptr->tfs_info[trch_idx].tf_info[oc_tfc_ptr->tfi[trch_idx]];
      if ((UE_LOGCHAN_CCCH == lc_info_ptr->lc_type) ||
          (UE_LOGCHAN_DCCH == lc_info_ptr->lc_type))
      {
        txbufptr->srb_data_present = TRUE;
        txbufptr->srb_data_num_tti = oc_ul_tfcs_ptr->tfs_info[trch_idx].tti_info;
      }

      if (l1_trch_data_ptr->num_tb != oc_tf_ptr->ntblks)
      {
        MSG_3(MSG_SSID_WCDMA_MAC, MSG_LEGACY_ERROR, "Mismatch in num_tb %d for LC %d, TFCI %d",
                                                    l1_trch_data_ptr->num_tb,
                                                    lc_data_ptr->rlc_id,
                                                    GET_SELECTED_TFCI);
      }
    } /* Valid RLC ID */

    /* Invalid RLC ID */
    else
    {
      MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH , "Invalid RLC ID in UL RLC Frame Data");
    }
  }
  /* Determine the number of active transport channels */
  /* Clear the TB count and TB Size values of inactive channels */
  for (i=0; i <L1_UL_MAX_TRCH; i++)
  {
    /* Transport Format Pointer */
    oc_tf_ptr= &oc_ul_tfcs_ptr->tfs_info[i].tf_info[oc_tfc_ptr->tfi[i]];

    /* Transport channel has data */
    if (trch_select_flag[i] == TRUE)
    {
      /* Update active transport channel count */
      txbufptr->num_trch++;

#ifdef  FEATURE_AL1
      #error code not present
#endif

      int_tc_ptr = GET_UL_TRCH_INFO_PTR(i);
      l1_trch_data_ptr = &txbufptr->tx_data[i];

      if ((l1_trch_data_ptr->num_tb > UE_MAX_UL_RLC_PDUS) ||
          (l1_trch_data_ptr->tb_size > L1_UL_MAX_TB_SIZE))
      {
        txbufptr->num_trch = 0;
        ERR_FATAL("Mismatch in num_tb %d tb_size %d, TFCI %d",
        l1_trch_data_ptr->num_tb,l1_trch_data_ptr->tb_size,GET_SELECTED_TFCI);
      }
    }
    /* Check if the TF has non-zero number of TBs and TB size of 0 */
    else if ((oc_tf_ptr->ntblks >0) && (oc_tf_ptr->tb_size ==0) &&
    (current_tti >= oc_ul_tfcs_ptr->tfs_info[i].tti_info))
    {
      txbufptr->num_trch++;
      txbufptr->tx_data[i].num_tb =(uint8) oc_tf_ptr->ntblks;
      txbufptr->tx_data[i].tb_size = 0;

      /* Set DSM pointers to NULL */
      for (j=0; j < oc_tf_ptr->ntblks; j++)
      {
        /* Set the ciphering flag to FALSE for 1x0 tf's */
        txbufptr->tx_data[i].trch_data[j].ciphering_on = FALSE;
        txbufptr->tx_data[i].trch_data[j].data_ptr = NULL;
        txbufptr->tx_data[i].trch_data[j].mac_hdr_size = 0;
      }
    }
    /* Transport channel has no data */
    else
    {
      txbufptr->tx_data[i].num_tb = 0;
      txbufptr->tx_data[i].tb_size = 0;
    }
  }
}


/*===============================================================================================
 FUNCTION NAME  mac_compute_pdus_from_rlc_status

 DESCRIPTION  Compute the nbytes and npdus needed per LC based on RLC BO calculations
================================================================================================*/
void mac_compute_pdus_from_rlc_status(uint8  nlchan, rlc_lc_id_type   *rlc_id_ptr, 
rlc_ul_buf_status_type    *ul_buf_status_ptr,
                                      boolean *data_present_on_srb_ptr)
{
  uint32                      i, divisor, npdus, total_pdus;
  rlc_lc_id_type              rlc_id;
  ul_lc_info_type            *int_lc_ptr;
  rlc_ul_lc_buf_status_type  *buf_status_ptr;
  
  npdus = 0;
  total_pdus = 0;
  
  /* Make a copy of the buffer status so that this can be updated
  after ETFCI selection and can be used for SI reporting */
  for (i=0;i< nlchan;i++)
  {
    if (i >= UE_MAX_UL_LOGICAL_CHANNEL)
    {
      MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_ERROR, "MAC_ERROR: number of logical channels - %d is greater than max logical channels allowed",
                                                  nlchan);
      break;
    }
    rlc_id = rlc_id_ptr[i];
    int_lc_ptr = GET_UL_LC_INFO_PTR(rlc_id);
    buf_status_ptr = &(ul_buf_status_ptr->chan_info[rlc_id]);
  
    if (buf_status_ptr->type == FIXED_PDUS)
    {
      if ((int_lc_ptr->lc_type == UE_LOGCHAN_CCCH) && (buf_status_ptr->npdus > 0))
      {
        MSG_3(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "MAC_HS_RACH: TM lc_id %d, npdus %d, pdu_size %d",
                                                   rlc_id,
                                                   buf_status_ptr->npdus,
                                                   buf_status_ptr->pdu_size);
        *data_present_on_srb_ptr = TRUE;
      }
     continue;
    }
  
    if (buf_status_ptr->suspend_pdu_build_f)
    {
      buf_status_ptr->nbytes = 0;
    }
    else
    {
      if (int_lc_ptr->rlc_mode == UE_MODE_UNACKNOWLEDGED)
      {
        // effect_win_size, retx_bytes, ctrl_bytes matter only for am
        buf_status_ptr->nbytes = buf_status_ptr->new_data_bytes;
        
        // Get the divisor value
        divisor = (int_lc_ptr->pdu_size - int_lc_ptr->rlc_hdr_length);
        total_pdus = math_ceil(buf_status_ptr->new_data_bytes, divisor);
      }
      else
      {
         // First set nbytes to retx bytes
         buf_status_ptr->nbytes = buf_status_ptr->retx_data_bytes;
  
         // Get the divisor value
         divisor = (int_lc_ptr->pdu_size - int_lc_ptr->rlc_hdr_length);
  
         total_pdus = math_ceil(buf_status_ptr->retx_data_bytes, divisor);
         
         if (buf_status_ptr->ctrl_data_bytes && divisor)
         {
           /* Round off the ctrl bytes to number of PDUS.
            * ctrl_data_bytes includes rlc_hdr_lenght also, so 
            * npdus = ceil(ctrl_data_bytes,pdu_size).
            */
           npdus = math_ceil((buf_status_ptr->ctrl_data_bytes << 3), int_lc_ptr->pdu_size);
  
           // Add to the number of bytes the value of ctrl bytes
           buf_status_ptr->nbytes += ((npdus * divisor) >> 3);
           
           total_pdus += npdus;
         }
  
         if (buf_status_ptr->new_data_bytes && divisor)
         {
           // Get the number of PDUs that can be formed with
           npdus = math_ceil((buf_status_ptr->new_data_bytes << 3),divisor);
  
           npdus = MIN(npdus, buf_status_ptr->effect_win_size);
  
           // Add to the number of bytes the value of new bytes
           buf_status_ptr->nbytes += ((npdus * divisor) >> 3);
         }
      }
      
      if (buf_status_ptr->nbytes)
      {
        if (int_lc_ptr->lc_type == UE_LOGCHAN_DCCH)
        {
          *data_present_on_srb_ptr = TRUE;
        }

        MSG_9(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH,
        "DBG_BUF_STATUS: Rlc_id %d, new_data %d bytes, retx_data %d bytes,ctrl_data %d bytes, pdu_size %d, hdr_len %d, win_size %d, nbytes %d, npdus %d",
        rlc_id, buf_status_ptr->new_data_bytes, buf_status_ptr->retx_data_bytes,
        buf_status_ptr->ctrl_data_bytes, int_lc_ptr->pdu_size, int_lc_ptr->rlc_hdr_length,
        buf_status_ptr->effect_win_size, buf_status_ptr->nbytes, npdus);
      }
    }
  }

}

/*==============================================================================================
 FUNCTION NAME  build_ul_tblks

 DESCRIPTION  This function builds UL Transport Blocks
===============================================================================================*/
boolean   build_ul_tblks
(
/* Pointer to Build Frame Indication Parameters */
mac_ul_build_frame_ind_type   *bld_frame_ind_ptr
)
{
  uint16  i;
  uint8   asc=0;

  /* Pointer to internal TrCh information */
  ul_trch_info_type   *int_tc_ptr;

  /* Ponter to ordered TFC config data */
  mac_ul_tfc_type     *oc_tfc_ptr;

  boolean data_present_on_srb = FALSE;

  /* Clear uplink buffer status info */
  memset (&ul_buf_status, 0, sizeof (rlc_ul_buf_status_type));


#ifndef FEATURE_WCDMA_MAC_UL_ZI_REDUCTION
  rlc_ul_data_ptr = &rlc_ul_data_info;
#else
  rlc_ul_data_ptr = rlc_ul_data_info_ptr;
#endif /* FEATURE_WCDMA_MAC_UL_ZI_REDUCTION */


  /* Clear RLC Data Count */
  rlc_ul_data_ptr->nchan = 0;

  /*------------------------------------------------------------------------
  Get the logical channel buffer status from RLC
  mac_hs_ul_lc_info contains the list of logical channels for which buffer
  status is requested for. mac_hs_ul_lc_info structure is populated at
  configuration time by MAC.
  mac_hs_ul_ls_buf_status structure is populated by RLC. It contains the
  buffer status for the UM/AM logical channels in bytes.
  ------------------------------------------------------------------------*/
  if (mac_ul_lc_info.nchan > 0 )
  {
    rlc_ul_enh_buffer_status(&mac_ul_lc_info,&ul_buf_status, BFI_QUERY, NULL);

    mac_compute_pdus_from_rlc_status(mac_ul_lc_info.nchan, mac_ul_lc_info.rlc_id, &ul_buf_status, &data_present_on_srb);

   } // nchan > 0

  if (GET_UL_CCTRCH_TYPE == RACH)
  {
    mac_rach_srb_rxd_manage(data_present_on_srb);
  }
  bld_frame_ind_ptr->tx_buf_ptr->srb_data_present = data_present_on_srb;

  if (block_transmission && (GET_UL_CCTRCH_TYPE==RACH))
    proc_block_dtch_tx_on_rach(&ul_buf_status);

  /* Do not send SRB info during this TTI */
  if (bld_frame_ind_ptr->srb_omit)
  {
    mac_ignore_ul_srbs (&ul_buf_status);
  }
  rlc_profile_dump_start(&rlc_tfc_profile_dump);
  /* Select suitable TFC */
  select_tfc (bld_frame_ind_ptr->tti, &ul_buf_status, &ul_rlc_frame_fmt);
  rlc_tfc_profile_dump.profile_array[rlc_tfc_profile_dump.dump_idx].add_on3 = 
                            GET_SELECTED_TFCI;
  rlc_profile_dump_end(&rlc_tfc_profile_dump);

  /* Number of RLC Logical Channels selected is greater than zero */
  if (ul_rlc_frame_fmt.nchan >0)
  {
    if (GET_UL_CCTRCH_TYPE == RACH)
    {
      /* Selected ASC */
      asc = mac_asc_selection(&ul_rlc_frame_fmt);

      /* Query L1 to check if the ASC has any assigned sub channel number ?
      If the ASC chosen doesnt have any assigned subchannel number then
      the l1_ul_tx_buf->num_trch will be set to 0 so that we dont ask RLC
      to build the pdu. */

      if ( (asc < L1_UL_ASC_MAX) && (l1_is_sub_chan_avail_for_asc(asc)))
      {
        /* Clear the memory */
        memset(rlc_ul_data_ptr, 0x00, sizeof(rlc_ul_frame_data_type));

        /* Get uplink RLC PDUs */
        rlc_ul_build_pdus (&ul_rlc_frame_fmt, rlc_ul_data_ptr);
      }
      else
      {
        MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "There is no assigned subchan for this ASC %d",
                                                   asc);
        bld_frame_ind_ptr->tx_buf_ptr->num_trch = 0;
      }
    }
    else
    {
      /* Clear the memory */
      memset(rlc_ul_data_ptr, 0x00, sizeof(rlc_ul_frame_data_type));

      mac_profile_dump_start();
			
      /* Get uplink RLC PDUs */
      rlc_ul_build_pdus (&ul_rlc_frame_fmt, rlc_ul_data_ptr);

      mac_profile_dump_end();			
    }
  }

  /* Attach MAC header and ciphering parameters */
  attach_mac_parameters ( rlc_ul_data_ptr, bld_frame_ind_ptr->tx_buf_ptr);

#ifdef FEATURE_DUMMY_SIGNALING
  #error code not present
#endif

  /* Selected TFC pointer */
  oc_tfc_ptr = &oc_ul_tfcs_ptr->tfc_info[GET_SELECTED_TFCI];

  /* Set the transport channel tx flags */
  for (i=0; i <UE_MAX_TRCH; i++)
  {
    int_tc_ptr = GET_UL_TRCH_INFO_PTR(i);
    if (int_tc_ptr->prev_tx_flag == FALSE)
    {
      int_tc_ptr->prev_tx_flag =
      (oc_ul_tfcs_ptr->tfs_info[i].tf_info[oc_tfc_ptr->tfi[i]].ntblks >0) ? TRUE:FALSE;
      int_tc_ptr->prev_tfi = oc_tfc_ptr->tfi[i];
    }
  }

  /* Set TFCI value */
  bld_frame_ind_ptr->tx_buf_ptr->TFCI = GET_SELECTED_TFCI;
  bld_frame_ind_ptr->tx_buf_ptr->asc_num = asc;

  MSG_3(MSG_SSID_WCDMA_MAC, MSG_LEGACY_LOW , "TFCI(%d) PDU Count (%d/%d)",
  bld_frame_ind_ptr->tx_buf_ptr->TFCI,
                                             rlc_ul_data_ptr->nchan,
                                             ul_rlc_frame_fmt.nchan);

  MSG_2(MSG_SSID_WCDMA_MAC, MSG_LEGACY_LOW, "TFCI %d, ASC %d",
                                            bld_frame_ind_ptr->tx_buf_ptr->TFCI,
                                            bld_frame_ind_ptr->tx_buf_ptr->asc_num);
  return(TRUE);
}

/*===========================================================================
FUNCTION  L1_PHY_UL_CFN_MISSED_IND

DESCRIPTION
  This function notifies MAC that CFN update interrupt was missed due to
  long period of WCDMA_INTLOCK().

  This function will be called up in L1 inteerupt
  context on detecting that it missed CFN updates due to some higher layer doing
  WCDMA_INTLOCK for more than 10ms.This function will initialize the state variables
  related to TFCS selection, in particular to the transport format index which
  is reset for the transport channels, so that one getting the build frame indication
  subsequently, UE UL would be able to recover.

DEPENDENCIES
  DCH state when ENC detects missed CFN update interrupt.

RETURN VALUE
  None

SIDE EFFECTS
  MAC should sync-up or reset.
===========================================================================*/
void l1_phy_ul_cfn_missed_ind
(
  uint8 curr_cfn
)
{
  /* Pointer to internal TrCh information */
  ul_trch_info_type   *int_tc_ptr;
  uint8 i;

  MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_ERROR, "MAC UL sync-up on missed CFN %d",
                                              curr_cfn);

  /* Initialize internal transport channel information */
  for (i=0; i <UE_MAX_TRCH; i++)
  {
    /* Pointer to Transport Channel internal info */
    int_tc_ptr = GET_UL_TRCH_INFO_PTR(i);

    int_tc_ptr->prev_tx_flag = FALSE;
    int_tc_ptr->prev_tfi = 0;
  }
}



