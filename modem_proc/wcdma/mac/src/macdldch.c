/*=======================================================================================

                         D O W N L I N K   D C H   H A N D L E R


GENERAL DESCRIPTION

  This module contains functions to :
  - process downlink DCH configuration and data
  - process downlink DCH transport block set header information

Copyright (c) 2001 by Qualcomm Technologies, Inc.  All Rights Reserved.
Copyright (c) 2002-2009 by Qualcomm Technologies, Inc.  All Rights Reserved.

============================================================================================*/

/*============================================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mac/vcs/macdldch.c_v   1.5   07 Aug 2001 18:28:52   psuvarna  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/mac/src/macdldch.c#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when      who     what, where, why
--------  ---     --------------------------------------------------------------------------
10/17/14  ts      Fix to discard DL PDUs received while in the Reset Pending State, to avoid potential ciphering mismatch right after the recovery
07/26/14  rkmk    Reduce image size by using right macro depending upon number of parameters
01/17/13  as      Removed unwanted F3.
07/13/12  grk     MBMS feature cleanup at L2(MBMS_DEBUG_1, FEATURE_MODEM_MBMS)
01/02/12  grk     Feature cleanup
08/26/11  grk     Fix for KW errors
06/02/11  kc      Added support for logical channel to transport channel mapping info
06/17/10  grk     Fixed compiler warning.
06/14/10  grk     Fixed compilation errors/warnings.
07/08/09  grk     Modified cipher algo checks in accordance with new 
                  ciphering algo enum (added as part of snow3g support).
07/02/09  mrg     Coade added under feature FEATURE_TC_CONSTANT_DELAY_ENH
                  for the TS 34.121 TC 7.9.1A fix
                  with this fix we are maintaing a constant delay for the LB-2, 
                  if the delay corssed more than 10 TTIs we are dropingthe SDUs.
07/01/09  mrg     Fixed lint errors/warnings.
6/04/09   kvk     Mark the CFN to TRUE only if the PDU valdation succeeds
05/29/09  ssg     Added support for SNOW_3G algo.
05/07/09  ssg     Updated Copyright Information
04/29/09  av      Fixed compiler warnings in L2 
04/07/09  rm      Included renamed files.
02/18/09  prk     Removed the relative path in header file name for the sake
                  of offline builds.
12/23/08  mrg     MAC drops the PDU with all zero contents.
12/05/08  grk     compilation fix for 8200 (rel7).
11/31/08  mrg     During the Hdr Rd event logics, MAC should check for the TB Size and
                  Set the tb_hdr_ptr->valid flag to FALSE, in the case of TB 0.
11/21/08  mrg     Compilation error fix - when HSDPA feature is disabled 
10/09/08  kvk     Now passing ciphering algo explicity for DCH TB
09/23/08  mrg     In loopback mode2 - If TB data of 0 bits is received then
                  No Hdr to be processed and MAC return valid as TRUE so that
                  L1 will pass the CRC bits alone to MAC for loopback.
09/18/08  ksr/kvk Now passing ciphering Algo while processing the MAC header
05/19/08  kvk     Now in DCH hdr read event check the status of rlc_dl_decode_pdu_hdr() and mark the 
                  valid flag  in TB hdr ptr to TRUE/FALSE accordingly
04/29/08  stk     Modified the include path to suite Linux compilers.
01/29/08  mrg     Changed a print MSG_HIGH TO MSG_LOW in process_dl_dch_tbs_hdr()
01/21/08  stk     Modified the include path to fix compiler error in 7601 build.
04/27/07  sk      Discard DCH, DCCH blocks if the 2st 5 header bytes are all 0's
02/14/07  sk      fixed lint error
08/26/06  sk      Fixed lint indentation errors.
08/18/06  sk      Added support fro SRB5 under FEATURE_SRB5_SUPPORT
07/21/05  gsc     Fixed some more int errors.
03/09/05  sk      Fixed all lint errors.
02/15/05  gsc     Added HSDPA support under FEATURE_HSDPA feature flag.
12/30/04  sk      Fixed lint errors
10/28/04  sk      process_dl_dch_tbs_hdr (): Mark the cfn only once at which some transport block was
                  received in DL on DCH transport channel and when the channel is put in loopback mode.
09/18/04  sk      Added cfn_ciphering_table which is maintained by Uplink. DL will use this
                  table to determine if the ciphering is enabled, what algorithm to use and
                  what key index to be used.
06/04/04  sk      Print the debug messages for ciphering only every 500 ms.
06/03/04  sk      Added some debug messages.
12/12/03  sk      Fix for CR 38279 :Removed the incorrect re-settin of the ciphering_control flag
                  after setting it correctly based on the ciphering algorithm in use.
12/04/03  sk      Added debug messages to be printed every mac_ul_debug_print_interval
                  times for debugging ciphering garbling.
10/28/03  sk      In process_dl_dch_tbs_hdr() : only if the algo is MAC_CIPHER_ALGO_UE_1
                  perform deciphering.
08/25/03  sk      Added a flag "ciphering_garbled" for the purposes of debugging when there
                  is garbled voice heard. This flag can be set to TRUE at run-time to get some
                  debugging messages.
07/09/03  sk      Initialized ciphering_control flag to false by default.
04/29/03  sk      Stores the channel type (DTCH or DCCH) for DCH. This information is sent
                  by RRC during the time of configuration. Under FEATURE_TC_CONSTANT_DELAY.
01/31/03  sk      proc_dl_dch_tbs_hdr(): Populated tb_in_tm_mode field under
                  FEATURE_WCDMA_L1_TM_DECIPHER_SUPPORTED
10/18/02  sk      CHanged logical or (||) to bitwise or (|) in the calculation of count_c.
09/26/02  sk      Added a cfn_hfn_table which maintains the association of a hfn with a cfn.
                  DL just looks up atthis table to compute its count_c.
09/04/02  sk      Handled ciphering for TM mode rb's. For UM and AM copied the parameters
                  given by RLC
08/02/02  sk      Commented out parts related to ciphering.
08/07/01  ps      Fixed the leading zero message format problem
06/18/01  ps      Adjusted the MSG levels
04/04/01  ps      Initial release after merging "macdldchcfg.c" and "macdldchhdr.c"

============================================================================================*/

/*============================================================================================
                                 INCLUDE FILES
============================================================================================*/
#include "wcdma_variation.h"
#include  "customer.h"
#include  "rex.h"
#include  "msg.h"
#include  "macrrcif_v.h"
#include  "l2dltask.h"
#include  "macinternal.h"
#include  "macdl.h"
#include  "l1mathutil.h"
#include  "macdldch.h"
#include  "wl1m.h"


/*===========================================================================================
                         DATA DECLARATIONS
===========================================================================================*/

/* Poking this flag to TRUE at run time will provide debug messages to see where the
ciphering went wrong */

boolean ciphering_garbled = TRUE;

/* Once the ciphering_grabled message is set to TRUE the debug print messages
would eb printed every 500 ms. */
uint8 mac_dl_debug_print_interval = 50;

uint8 prev_print_cfn = 0xff;

/* Look up table for finding the hfn associated with the cfn. This table is updated
in macul.c */
extern uint32 cfn_hfn_table[MAX_CN_DOMAIN][256];
extern mac_ciphering_table       cfn_ciphering_table[256];

/* Number of active DL DCHs */
uint8    num_active_dl_dchs;

#ifdef FEATURE_TC_CONSTANT_DELAY
  #include "maculdl.h"
lb_delay_struct_type  lb_delay_type;
extern mac_loop_back_mode_e_type loopback_test_mode;
extern boolean                   cfn_marked;
#endif

/*==============================================================================================
FUNCTION NAME   process_dl_dcch_dtch_dch_config

DESCRIPTION   This function processes config info of DCCH/DTCH that is maaped to DL DCH
==============================================================================================*/
void  process_dl_dcch_dtch_dch_config
(
/* Ordered DCH config data pointer */
mac_dl_dch_trch_config_type   *oc_dch_ptr,

/* Internal Dl DCH Config data pointer */
dl_dch_config_info_type     *int_dch_ptr
)
{
  boolean  valid;
  uint8    i;
  uint8    tbl_index = 0;

  /* Pointer to ordered DCCH/DTCH config data */
  mac_dl_ded_logch_config_type  *oc_dlc_ptr;

  /* Pointer to internal DCCH/DTCH config data */
  dlc_dch_config_info_type      *int_dlc_ptr;

  /* Number of DCCH/DTCHs */
  int_dch_ptr->num_dlc = oc_dch_ptr->ndlchan;

  /* Set Dedicated Logical Channel Enable flag to FALSE and mark all dedicated logical
     channels as disabled */
  for (i=0; i < MAX_DED_LOGCHAN_PER_TRCH; i++)
    int_dch_ptr->dlcinfo[i].enable = FALSE;

  /* Process each DCCH/DTCH config information */
  for (i=0 ; i < oc_dch_ptr->ndlchan; i++)
  {

    /* Initially mark the parameter validity flag to FALSE */
    valid = FALSE;

    /* Pointer to the ordered DCCH/DTCH config data */
    oc_dlc_ptr = &oc_dch_ptr->dlchan_info[i];

    /* Invalid RLC ID */
    if (DL_RLC_ID_IS_INVALID(oc_dlc_ptr->rlc_id))
    {
      MSG_2(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH , "DCCH/DTCH%d with invalid RLC ID %d",
                                                  i, oc_dlc_ptr->rlc_id);
    }
    /* RLC Mode is invalid */
    else if (DL_RLC_MODE_IS_INVALID(oc_dlc_ptr->rlc_mode))
    {
      MSG_2(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH , "DCCH/DTCH%d with invalid RLC Mode %d",
                                                  i, oc_dlc_ptr->rlc_mode);
    }
    /* RB ID is invalid */
    else if (DL_RB_ID_IS_INVALID(oc_dlc_ptr->rb_id))
    {
      MSG_2(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH , "DCCH/DTCH%d with invalid RB ID",
                                                  i, oc_dlc_ptr->rb_id);
    }
    /* Only one DCCH/DTCH is mapped to FACH */
    else if (int_dch_ptr->num_dlc == 1)
    {
      tbl_index = 0;
      valid = TRUE;
    }
    /* Multiple DCCH/DTCHs are mapped to DL DCH */
    else if (int_dch_ptr->num_dlc > 1)
    {
      /* MAC ID is invalid */
      if (DL_MAC_ID_IS_INVALID(oc_dlc_ptr->mac_id))
      {
        MSG_2(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH , "DCCH/DTCH%d with invalid MAC ID %d",
                                                    i, oc_dlc_ptr->mac_id);
      }
      else
      {
        tbl_index = (uint8)(oc_dlc_ptr->mac_id - MAC_MIN_MAC_ID);
        valid = TRUE;
      }
    }

    /* Config data is valid */
    if (valid == TRUE)
    {

      /* Pointer to internal DCCH/DTCH config data */
      int_dlc_ptr = &int_dch_ptr->dlcinfo[tbl_index];

      /* Save DCCH/DTCH info in FACH CONFIG table */
      int_dlc_ptr->enable = TRUE;
      int_dlc_ptr->rlc_id = oc_dlc_ptr->rlc_id;
      int_dlc_ptr->rb_id  = oc_dlc_ptr->rb_id;
      int_dlc_ptr->rlc_mode = oc_dlc_ptr->rlc_mode;

#ifdef FEATURE_TC_CONSTANT_DELAY
      int_dlc_ptr->chan_type = oc_dlc_ptr->chan_type;
#endif
      MSG_3(MSG_SSID_WCDMA_MAC, MSG_LEGACY_LOW , "DCCH/DTCH%d: RLC ID =%d, Mode =%d",
                                                 i,
                                                 oc_dlc_ptr->rlc_id,
                                                 oc_dlc_ptr->rlc_mode);
    }
  }
}

/*============================================================================================
FUNCTION NAME   process_dl_dch_config

DESCRIPTION   This function processes DL DCH configuration data

============================================================================================*/
void  process_dl_dch_config
(
/* Number of DCHs */
uint8  num_dchs,

/* Pointer to ordered DCH configuration data list */
mac_dl_dch_trch_config_type *oc_dch_ptr_list[]
)
{
  uint8   i;

  /* ID to Index Table Pointer */
  trch_id_to_idx_map_type     *id_to_idx_ptr;

  /* Pointer to DCH TrCh config info */
  mac_dl_dch_trch_config_type    *oc_dch_ptr;

  /* Pointer to config info in DCH Config table */
  dl_dch_config_info_type      *int_dch_ptr;

  /* Set number of active DCHs to zero */
  num_active_dl_dchs = 0;

  /* Initialize DCH ID to Index Mapping Table */
  for (i=0; i<MAX_TRCH; i++)
  {
    dl_dch_id_to_idx[i].valid = FALSE;
  }

#ifdef FEATURE_TC_CONSTANT_DELAY
  /* Initialize the first_tb boolean to TRUE so that UL MAC would use this
  flag to calculate the delay and subsequently this constant delay would
  be maintained for transmitting any block in the UL */
#ifndef FEATURE_TC_CONSTANT_DELAY_ENH
  memset(&lb_delay_type, 0x00, sizeof(lb_delay_struct_type));
  lb_delay_type.first_tb = TRUE;
#endif
#endif

  /* Invalid number of DCHs */
  if (num_dchs > UE_MAX_TRCH)
  {
    MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH , "invalid number of DCHs %d",
                                                num_dchs);
  }
  /* Process DCH config data */
  else
  {

    for (i=0; i < num_dchs; i++)
    {

      /* Pointer to DCH CCTrCh info */
      oc_dch_ptr = oc_dch_ptr_list[i];

      /* DCH ordered config data pointer is NULL */
      if (oc_dch_ptr == NULL)
      {
        MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH , "DCH%d config pointer is NULL",
                                                    i);
      }
      /* Invalid TrCh ID */
      else if (DL_TRCH_ID_IS_INVALID(oc_dch_ptr->trch_id))
      {
        MSG_2(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH , "DCH%d with invalid TrCh ID %d",
                                                    i, oc_dch_ptr->trch_id);
      }
      /* Process DCH config data */
      else
      {
        /* Pointer to DCH config table entry */
        int_dch_ptr = &dl_dch_cfg_info[num_active_dl_dchs];

        /* Get Transport ID to Index map Table */
        id_to_idx_ptr = DL_DCH_ID_TO_IDX_PTR(oc_dch_ptr->trch_id);
        id_to_idx_ptr->valid = TRUE;
        id_to_idx_ptr->idx = num_active_dl_dchs;

        /* Save CCTrCh ID in internal DCH Config Table */
        int_dch_ptr->cctrch_id = oc_dch_ptr->cctrch_id;
        int_dch_ptr->trch_id = oc_dch_ptr->trch_id;

        MSG_3(MSG_SSID_WCDMA_MAC, MSG_LEGACY_LOW , "DL DCH%d: TrChID=%d CCTrChID=%d",
                                                   i,
                                                   int_dch_ptr->trch_id,
                                                   int_dch_ptr->cctrch_id);

        /* Process DCCH/DTCH data */
        process_dl_dcch_dtch_dch_config (oc_dch_ptr, int_dch_ptr);

        /* Increment the number of active DL DCHs */
        num_active_dl_dchs++;
      }
    }
  }
}

/*=============================================================================================================
FUNCTION    process_dl_dch_tbs_hdr

DESCRIPTION   This function processes the header information of DCH transport block set

==============================================================================================================*/
void  process_dl_dch_tbs_hdr
(
/* transport Block ID */
uint8 tb_id,

/* Pointer to internal DCH config info */
dl_dch_config_info_type   *int_dch_ptr,

/* Pointer to the header info of Transport Block Set */
l1_dl_tb_set_hdr_struct_type  *tbs_hdr_ptr
)
{
  boolean   valid, status;

  uint8 i, ct_field, num_tb, dlcidx = 0;

  /* Bit offset of RLC PDU */
  uint8 rlc_pdu_offset = 0;

  /* Pointer to DL TB info */
  dl_tb_info_type       *dl_tb_info_ptr;

  /* Pointer to internal DCCH/DTCH config info */
  dlc_dch_config_info_type  *dlcptr = NULL;

  /* Pointer to TB header information */
  l1_dl_tb_hdr_struct_type    *tb_hdr_ptr;

  /* RLC Header Info */
  rlc_dl_tb_hdr_info_type   rlc_hdr_info;


  /* Current CFN for a transport block */
  uint8 current_tbs_cfn, index;

  uint32                    cipher_seg[3];

  /* RLC Ciphering Info */
  rlc_dl_tb_cipher_info_type  rlc_cipher_info;

  /* Pointer to TB Ciphering Parameters */
  l1_dl_ciphering_param_struct_type *tb_cipher_ptr;

  /* COUNT_C value to be given to Layer 1 */
  uint32  countc;
  uint32 temp_tb_header = 0;

  memset(&rlc_hdr_info, 0, sizeof(rlc_dl_tb_hdr_info_type));

  if (tbs_hdr_ptr == NULL)
  {
      MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_ERROR, "TBS_HDR_PTR NULL\n");
      return;
  }

  /* Get the current CFN number for the transport block set */
  current_tbs_cfn = tbs_hdr_ptr->info.non_hs.cfn;

  /* Number of transport blocks */
  num_tb = tbs_hdr_ptr->num_tb;

  /* Process each transport block of the set */
  for (i=0 ; i < num_tb; i++)
  {
    /* Initially set valid flag to FALSE */
    valid = FALSE;

    /* Get pointer to transport block information */
    tb_hdr_ptr = &tbs_hdr_ptr->tb_hdr_info[i];

    /* TB Header Pointer is NULL */
    if (tb_hdr_ptr == NULL) /*lint !e774*/
    {
      MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH , "DCH TB%d header pointer is NULL",
                                                  i);
    }
    /* Only one DCCH/DTCH is mapped to DCH */
    else
    {
      
#ifdef FEATURE_TC_CONSTANT_DELAY
      
      if ((loopback_test_mode == MAC_LOOPBACK_MODE_2) &&
          (int_dch_ptr->dlcinfo[0].rlc_mode == UE_MODE_TRANSPARENT) 
           && (tbs_hdr_ptr->info.non_hs.tb_num_bits == 0))
      {
        /* Case where - TB data of 0 bits is received. No Hdr to be processed.
         * Return valid as TRUE - so that, L1 will pass the CRC bits alone to 
         * MAC for loopback 
         */
        tb_hdr_ptr->ciphering_param.ciphering_control = FALSE;
        
        /* Save RLC ID in Transport Block info table */
        dl_tb_info_ptr = GET_DL_TB_INFO_PTR(tb_id + i);
        
        /* Set other header parameters of TB */
        tb_hdr_ptr->tb_id = (uint8)(tb_id + i); /*lint !e613*/
        tb_hdr_ptr->valid = TRUE; /*lint !e613*/
        tb_hdr_ptr->num_bit_offset_trblk = rlc_pdu_offset; /*lint !e613*/
        
        dl_tb_info_ptr->valid = TRUE;
        dl_tb_info_ptr->rlc_id = int_dch_ptr->dlcinfo[0].rlc_id;
        
        dlcidx = 0;
        /* Pointer to internal DCCH/DTCH config info */
        dlcptr = &int_dch_ptr->dlcinfo[dlcidx];
        
        /* DCCH/DTCH is enabled */
        if (dlcptr->enable == TRUE)
        {
          /* Constant Loopback Delay has to be maintained for DCH type transport channel
          and when the loopback test mode is either of type 1 or 2 */
          /*---------------------------------------------------------------------------------------
           register the downlink CFN when the transport block was received in the
           downlink. This block would be transmitted in the uplink after applying
           the constant delay. If this cfn is already registered in the array then do not
           re-enter it. If cfn_marked is set to TRUE then this cfn exists in the database already
           ---------------------------------------------------------------------------------------*/
          if ((cfn_marked == FALSE) && (lb_delay_type.cur_idx < MAX_NUM_CFN))
          {
            lb_delay_type.dl_cfn[lb_delay_type.cur_idx] = current_tbs_cfn;
            lb_delay_type.cur_idx++;
            lb_delay_type.cur_idx = lb_delay_type.cur_idx % MAX_NUM_CFN;
            MSG_3(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "cur_dl_cfn %d ntb %d rlc_id %d",
                                                       current_tbs_cfn,
                                                       num_tb,
                                                       rlc_hdr_info.rlc_id);
            cfn_marked = TRUE;
          }
        }
        
        continue;
      }
#endif /* FEATURE_TC_CONSTANT_DELAY */
      
      if (int_dch_ptr->num_dlc == 1)
      {
        dlcidx = 0;
        rlc_pdu_offset = 0;
        valid = TRUE;
      }
      /* Multiple DCCH/DTCHs mapped to DCH */
      else if (int_dch_ptr->num_dlc > 1)
      {
        /* C/T field */
        ct_field = (uint8)(tb_hdr_ptr->tb_header[0] >> 4);

        /* Invalid C/T field */
        if (DL_CT_FLD_IS_INVALID(int_dch_ptr->num_dlc,ct_field))
        {
          MSG_2(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH , "TB%d with invalid C/T %x",
                                                      tb_id, ct_field);

        }
        /* Valid C/T field */
        else
        {
          dlcidx = ct_field;
          rlc_pdu_offset = 4;
          valid = TRUE;

          /* MAC drops the PDU with all zero contents */
          if (!ct_field && !tb_hdr_ptr->tb_header[0] && !tb_hdr_ptr->tb_header[1] && !tb_hdr_ptr->tb_header[2]
              && !tb_hdr_ptr->tb_header[3] && !tb_hdr_ptr->tb_header[4] && !tb_hdr_ptr->tb_header[5])
          {
            /* Pointer to internal DCCH/DTCH config info */
            dlcptr = &int_dch_ptr->dlcinfo[dlcidx];

            if ((dlcptr->chan_type==UE_LOGCHAN_DCCH) &&(dlcptr->rlc_mode == UE_MODE_UNACKNOWLEDGED))
            {
              valid = FALSE;
              MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "DCH Block headers are 0, discarding %d tb",
                                                         i);
            }
          }
        }
      }

      /* Initialize the ciphering _control flag to FLASE */
      tb_hdr_ptr->ciphering_param.ciphering_control = FALSE;

    } /* TB HDR not null */

    /* TB Parameters are valid */
    if (valid == TRUE)
    {
      /* Pointer to internal DCCH/DTCH config info */
      dlcptr = &int_dch_ptr->dlcinfo[dlcidx];

      /* DCCH/DTCH is enabled */
      if (dlcptr->enable == TRUE)
      {
        /* Get RLC Header info */
        rlc_hdr_info.rlc_id = dlcptr->rlc_id;
        rlc_hdr_info.rlc_pdu_start_offset = rlc_pdu_offset;
        rlc_hdr_info.hdr_ptr = (uint8 *)&tb_hdr_ptr->tb_header[0];

        rlc_cipher_info.cipher_flag = FALSE;
        status = FALSE;

        if (tbs_hdr_ptr->info.non_hs.tb_num_bits > 0)
        {
          status = rlc_dl_decode_pdu_hdr (&rlc_hdr_info, &rlc_cipher_info);
        }
        
        /*Decode the PDU header*/
        if (status == TRUE)
        {
          /* DCCH/DTCH is in Transparent Mode */
          tb_cipher_ptr = &tb_hdr_ptr->ciphering_param;/*lint !e613*/

          // Set ciphering control to FALSE.
          tb_cipher_ptr->ciphering_control = FALSE;

          MSG_2(MSG_SSID_WCDMA_MAC, MSG_LEGACY_LOW, "TrchId->%d;TrchType->%d;",
                                                    int_dch_ptr->trch_id,
                                                    dlcptr->chan_type);

          if (dlcptr->chan_type != UE_LOGCHAN_TM_DCCH_DL)
          {
            if (dlcptr->rlc_mode == UE_MODE_TRANSPARENT)
            {
              /* Get the domain this channel belongs to */
              index = int_dch_ptr->cs_or_ps_domain;

              tbs_hdr_ptr->tb_in_tm_mode = TRUE;
              /* If ciphering is enabled */
              if ((cfn_ciphering_table[current_tbs_cfn].cipher_enable) &&
                  ((cfn_ciphering_table[current_tbs_cfn].cipher_algo == UE_WCDMA_CIPHER_ALGO_UEA1) 
#ifdef FEATURE_WCDMA_REL7
                  ||(cfn_ciphering_table[current_tbs_cfn].cipher_algo == UE_WCDMA_CIPHER_ALGO_UEA2)
#endif
              ))
              {

                tb_cipher_ptr->ciphering_control = TRUE;

                /* Copy the ciphering related parameters to pass to Layer 1 */
                countc =  ((cfn_hfn_table[index][current_tbs_cfn] << 8) | current_tbs_cfn);

                /* Setting this flag to TRUE at run time will provide debug messages to
                figure out what is going wrong with ciphering */
                if ((i==0) && (ciphering_garbled &&
                               (current_tbs_cfn % mac_dl_debug_print_interval == 0)) &&
                               (current_tbs_cfn != prev_print_cfn))
                {
                  MSG_3(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "DL count_c 0x%x dl_cfn 0x%x key 0x%x",
                                                             countc,
                                                             current_tbs_cfn,
                                                             cfn_ciphering_table[current_tbs_cfn].key_idx);

                  temp_tb_header = 0;
                  temp_tb_header = ((0x000000FF & tb_hdr_ptr->tb_header[0])<<24)|((0x000000FF & tb_hdr_ptr->tb_header[1])<<16)|\
                              ((0x000000FF & tb_hdr_ptr->tb_header[2])<<8)|(0x000000FF & tb_hdr_ptr->tb_header[3]);
                  cipher_seg[0] = BIG_TO_LITTLE_ENDIAN_32(temp_tb_header);

                  temp_tb_header = 0;
                  temp_tb_header = ((0x000000FF & tb_hdr_ptr->tb_header[4])<<24)|((0x000000FF & tb_hdr_ptr->tb_header[5])<<16)|\
                              ((0x000000FF & tb_hdr_ptr->tb_header[6])<<8)|(0x000000FF & tb_hdr_ptr->tb_header[7]);
                  cipher_seg[1] = BIG_TO_LITTLE_ENDIAN_32(temp_tb_header);

                  temp_tb_header = 0;
                  temp_tb_header = ((0x000000FF & tb_hdr_ptr->tb_header[8])<<24)|((0x000000FF & tb_hdr_ptr->tb_header[9])<<16)|\
                              ((0x000000FF & tb_hdr_ptr->tb_header[10])<<8)|(0x000000FF & tb_hdr_ptr->tb_header[11]);
                  cipher_seg[2] = BIG_TO_LITTLE_ENDIAN_32(temp_tb_header);

                  MSG_3(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "PDU %x %x %x",
                                                             cipher_seg[0],
                                                             cipher_seg[1],
                                                             cipher_seg[2]);

                }

                tb_cipher_ptr->ciphering_algo = cfn_ciphering_table[current_tbs_cfn].cipher_algo;
                tb_cipher_ptr->ciphering_key_id  = cfn_ciphering_table[current_tbs_cfn].key_idx;
                tb_cipher_ptr->ciphering_count   = countc;

                tb_cipher_ptr->ciphering_bit_offset = rlc_pdu_offset;
                tb_cipher_ptr->ciphering_radio_bearerid = (uint8)dlcptr->rb_id;
                prev_print_cfn = current_tbs_cfn;

              }
            }
            /*DCCH/DTCH in non-Transparent Mode */
            else
            {
              tbs_hdr_ptr->tb_in_tm_mode = FALSE;
              /* Copy the cipherign parameters given by RRC */
              if (rlc_cipher_info.cipher_flag)
              {
                tb_cipher_ptr->ciphering_control = TRUE;

                tb_cipher_ptr->ciphering_algo = rlc_cipher_info.ciphering_algo;
				
                tb_cipher_ptr->ciphering_key_id = (uint8)rlc_cipher_info.cipher_key_id;
                tb_cipher_ptr->ciphering_count = rlc_cipher_info.count_c;
                tb_cipher_ptr->ciphering_radio_bearerid = (uint8)rlc_cipher_info.rb_id;
                tb_cipher_ptr->ciphering_bit_offset = rlc_cipher_info.cipher_offset;
              }
            }
          } //UE_LOGCHAN_TM_DCCH_DL
        }
        /* Save RLC ID in Transport Block info table */
        dl_tb_info_ptr = GET_DL_TB_INFO_PTR(tb_id + i);
        /*If the PDU decoding succeds then populate the TB strcuture*/
        if (status == TRUE)
        {
        
#ifdef FEATURE_TC_CONSTANT_DELAY
          /* Constant Loopback Delay has to be maintained for DCH type transport channel
          and when the loopback test mode is either of type 1 or 2 */
          if ((dlcptr->chan_type==UE_LOGCHAN_DTCH) &&
             (((dlcptr->rlc_mode == UE_MODE_TRANSPARENT) &&(loopback_test_mode != MAC_LOOPBACK_NA))
             || (((dlcptr->rlc_mode != UE_MODE_TRANSPARENT))&&(loopback_test_mode == MAC_LOOPBACK_MODE_2))))
          {
            /*---------------------------------------------------------------------------------------
            register the downlink CFN when the transport block was received in the
            downlink. This block would be transmitted in the uplink after applying
            the constant delay. If this cfn is already registered in the array then do not
            re-enter it. If cfn_marked is set to TRUE then this cfn exists in the database already
            ---------------------------------------------------------------------------------------*/
            if (cfn_marked == FALSE)
            {
              lb_delay_type.dl_cfn[lb_delay_type.cur_idx] = current_tbs_cfn;
              lb_delay_type.cur_idx++;
              lb_delay_type.cur_idx = lb_delay_type.cur_idx % MAX_NUM_CFN;
              MSG_3(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "cur_dl_cfn %d ntb %d rlc_id %d",
                                                         current_tbs_cfn,
                                                         num_tb,
                                                         rlc_hdr_info.rlc_id);
              cfn_marked = TRUE;
            }
          }/*lint !e613*/
#endif /* fetaure_tc_constant_delay */
          
          /* Set other header parameters of TB */
          tb_hdr_ptr->tb_id = (uint8)(tb_id + i); /*lint !e613*/
          tb_hdr_ptr->valid = TRUE; /*lint !e613*/
          tb_hdr_ptr->num_bit_offset_trblk = rlc_pdu_offset; /*lint !e613*/
          tb_hdr_ptr->discard_l2_pdu = rlc_hdr_info.discard_l2_pdu;
          dl_tb_info_ptr->valid = TRUE;
          dl_tb_info_ptr->rlc_id = dlcptr->rlc_id;
        }
        else
        {
           /*If the PDU hdr decoding fails mark the TB hdr as INVALID*/
           tb_hdr_ptr->valid = FALSE;
           dl_tb_info_ptr->valid = FALSE;
        }
      } /* If DCCH/DTCH is enabled */
    } /* IF the TB parameters are valid */
  } /* For all the transport blocks */
}/* process_dl_dch_tbs_hdr */


/*===============================================================================================
FUNCTION NAME mac_dl_check_dch_trch

DESCRIPTION   This function checks if given rlc-id and rb-id pair map to a DL-DCH transport channel
===============================================================================================*/
log_wcdma_trch_type mac_dl_check_dch_trch(rlc_lc_id_type rlc_id, rb_id_type rb_id)
{
  log_wcdma_trch_type return_trch_val = WCDMA_TRCH_INVALID;
  int i, j;

  for(i = 0; i < num_active_dl_dchs; i++)
  {
    for(j = 0; j < dl_dch_cfg_info[i].num_dlc; j++)
    {
      if((dl_dch_cfg_info[i].dlcinfo[j].enable == TRUE) &&
         (dl_dch_cfg_info[i].dlcinfo[j].rlc_id == rlc_id) &&
         (dl_dch_cfg_info[i].dlcinfo[j].rb_id == rb_id))
      {
        return_trch_val = WCDMA_TRCH_DCH;
        break;
      }
    }
  }

  return return_trch_val;
}



