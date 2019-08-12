/*===============================================================================================

              U L    D C H    H A N D L E R

GENERAL DESCRIPTION

  This module provides functions to :
  - process uplink DCH configuration data.
  - build DCH Transport Blocks

Copyright (c) 2001 by Qualcomm Technologies, Inc.  All Rights Reserved.
Copyright (c) 2002-2009 by Qualcomm Technologies, Inc.  All Rights Reserved.

===============================================================================================*/
/*===============================================================================================

                        EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mac/vcs/maculdch.c_v   1.9   10 Jul 2002 10:05:54   skrishna  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/mac/src/maculdch.c#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when      who   what, where, why
--------  ---   -----------------------------------------------------------------------------
05/10/16  as    MAC F3 reduction.
07/26/14   rkmk    Reduce image size by using right macro depending upon number of parameters
05/06/14  geg   Use safer versions of memcpy() and memmove()
11/7/12   ts    Fixed KW errors
01/02/12  grk   Feature cleanup
06/02/11  kc    Added support for logical channel to transport channel mapping info
04/29/09  av    Fixed compiler warnings in L2 
09/05/07  sk    Used the rlc_ul_enh_buffer_status fn under FEATURE_RLC_ENH_BUFFER_STATUS
08/14/06  sk    Added HSUPA support
03/12/06  sk    corrected indentation
07/21/05  gsc   Fixed some more lint errors.
03/09/05  sk    Fixed all lint errors.
02/09/05  sk    Fix for critical lint errors.
12/30/04  sk    Fixed lint errors
03/16/03  sk    Kept a count of the number of active TM mode bearers mapped to DCH trach.
09/04/02  sk    proc_ul_dch_cctrch_cfg(): Populate the domain information (CS or PS)
                for the transport channel structure that is internallly maintained by MAC.
07/08/02  sk    Removed FEATURE_TRAFFIC_VOLUME_MEAS
05/13/02  sk    Populate the trch_id_to_idx_map_type structure for the purpose of
                traffic volume measurements. This addition is defined under the
                #ifdef FEATURE_TRAFFIC_VOLUME_MEAS.
                (mks) Added maclb.h for FEATURE_DUMMY_SIGNALLING. Added code to get the dch index
                for purposes of dummy signalling.
08/07/01  ps    Fixed leading zero message format problem
07/10/01  ps    num_active_ul_dchs was updated at the wong place in proc_ul_dch_cctrch_cfg()
07/01/01  ps    removed "build_dch_tblks" function and moved the code to "macul.c"
06/18/01  ps    Adjusted MSG levels
04/04/01  ps    Initial release after changing "maculdchcfg.c" to "maculdch.c"

==============================================================================================*/

/*==============================================================================================
                     INCLUDE FILES
==============================================================================================*/
#include "wcdma_variation.h"
#include  "customer.h"
#include  "rex.h"
#include  "msg.h"
#include  "macrrcif_v.h"
#include  "l2ultask.h"
#include  "macinternal.h"
#include  "macul.h"
#include  <string.h>
#ifdef FEATURE_DUMMY_SIGNALING
#error code not present
#endif
#include  "maculdch.h"

uint8   mac_tm_bearers_count = 0;

/*---------------------------------------------------------------------------
  Contains the list of logical channels for which MAC needs the buffer status
---------------------------------------------------------------------------*/
extern rlc_ul_lc_info_type              mac_ul_lc_info;

/*==============================================================================================
MACRO NAME    DCCH_DTCH_HDR_ON_UL_DCH

DESCRIPTION   This macro decides the header size and header value of DCCH/DTCH mapped to DCH

===============================================================================================*/
#define   DCCH_DTCH_HDR_ON_UL_DCH\
\
  /* Multiple DCCH/DTCH mapped to DCH - C/T field exists */ \
  if (ndlchan > 1){ \
\
    /* Header consists of 4-bit C/T field */  \
        hdrval[0] = (uint8)((uint8)(oc_dlcptr->mac_id -1) << 4);   \
\
    /* Header size = 4 bits */  \
    int_lc_ptr->mac_hdr_size = 4; \
\
    /* Save header value */   \
    WCDMA_MEMCPY(int_lc_ptr->mac_hdr_value,           \
                 sizeof(uint8) * MAC_UL_MAX_HDR_SIZE, \
                 hdrval,                              \
                 sizeof(uint8) * MAC_UL_MAX_HDR_SIZE);\
  } \
  /* Only one DCCH/DTCH is mapped to DCH */ \
    else {  \
    /* Header Size = 0 bits */  \
    int_lc_ptr->mac_hdr_size = 0; \
  }

/*==============================================================================================
FUNCTION NAME proc_ul_dch_cctrch_cfg

DESCRIPTION   This function processes UL DCH CCTrCh Configuration Data
==============================================================================================*/
void  proc_ul_dch_cctrch_cfg
(
  /* Pointer to ordered config data of UL DCH type CCTrCh */
  mac_ul_dch_cctrch_config_type *ded_cctrch_ptr
)
{
    uint8 i, j;
  uint8 ndchs, ndlchan;
  uint8 num_active_ul_dchs;
  uint8   hdrval[MAC_UL_MAX_HDR_SIZE];
    trch_id_to_idx_map_type   *id_to_idx_ptr;

  /* Pointer to UL DCH ordered config data */
  mac_ul_dch_trch_config_type   *oc_dch_ptr;

  /* Pointer to DCCH/DTCH ordered config data */
  mac_ul_ded_logch_config_type  *oc_dlcptr;

  /* Pointer to internal UL DCH configuration data */
  ul_trch_info_type       *int_tc_ptr;

  /* Pointer to internal UL Logical Channel Information */
  ul_lc_info_type         *int_lc_ptr;

  maci_ul_trch_priority_info_type *priority_info_ptr;

  uint8     rlc_count = 0;
  /* Initialize */
  mac_ul_lc_info.nchan = 0;
  num_active_ul_dchs = 0;

  mac_tm_bearers_count = 0;

  /* CCTrCh ordered config pointer is NULL */
  if (ded_cctrch_ptr == NULL)
  {
    MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH , "CCTrCh Config pointer is NULL");
  }
  else if (ded_cctrch_ptr->ndchs > UE_MAX_TRCH)
  {
    MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH , "invalid number of DCHs per CCTrCh",
                                                ded_cctrch_ptr->ndchs);
  }
  else
  {
      /* Number of DCHs mapped to this CCTrCh */
    ndchs = ded_cctrch_ptr->ndchs;

        /* Process ordered config data of each dedicated transport channel */
    for (i=0; i<ndchs; i++)
    {
      /* Pointer to ordered config data of Dedicated Transport Channel */
      oc_dch_ptr = &ded_cctrch_ptr->dch_info[i];

      /* DCH ordered config data pointer is NULL */
      if (oc_dch_ptr == NULL) { /*lint !e774*/
        MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH , "DCH%d pointer is NULL",
                                                    i);
      }
      /* Invalid Transport Channel ID */
      else if (UL_TRCH_ID_IS_INVALID(oc_dch_ptr->trch_id)) {
        MSG_2(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH , "DCH%d with invalid TrCh ID %d",
                                                    i, oc_dch_ptr->trch_id);
      }
      /* Invalid number of logical channels */
      else if (UL_DCCH_DTCH_NUM_IS_INVALID(oc_dch_ptr->ndlchan)) {
        MSG_2(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH , "DCH%d with invalid number of DCCH/DTCHs %d",
                                                    i, oc_dch_ptr->ndlchan);
      }
      /* Valid Transport channel parameters */
      else {
#ifdef FEATURE_DUMMY_SIGNALING
        #error code not present
#endif
        /* Pointernal to internal DCH config info */
        int_tc_ptr = GET_UL_TRCH_INFO_PTR(num_active_ul_dchs);

        id_to_idx_ptr = GET_UL_TRCH_ID_TO_IDX_PTR(oc_dch_ptr->trch_id);
        id_to_idx_ptr->valid = TRUE;
        id_to_idx_ptr->idx = num_active_ul_dchs;

        MSG_3(MSG_SSID_WCDMA_MAC, MSG_LEGACY_LOW , "DCH%d Configuration: TrCh ID %d, # of DCCH/DTCHs = %d",
                                                   i,
                                                   oc_dch_ptr->trch_id,
                                                   oc_dch_ptr->ndlchan);

        /* Save Transport channel ID */
        int_tc_ptr->trch_id = oc_dch_ptr->trch_id;

        /* Indicates if the domain is cs or ps. Used when the ciphering is enabled*/
        int_tc_ptr->cs_or_ps_domain = (uint8)oc_dch_ptr->cs_or_ps_domain;
        /* Increment the active number of DCHs */
        num_active_ul_dchs++;

        /* # of DCCH/DTCHs mapped to this transport channel*/
        ndlchan = oc_dch_ptr->ndlchan;

        /* Process ordered config data of each dedicated logical channel */
        for (j=0; j <ndlchan; j++){

          /* Pointer to ordered config data of DCCH/DTCH */
          oc_dlcptr = &oc_dch_ptr->dlchan_info[j];

          /* DCCH/DTCh Ordered config pointer is NULL */
          if (oc_dlcptr == NULL) { /*lint !e774*/
            MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH , "DCCH/DTCH%d config pointer is NULL");
          }
          /* Invalid RLC ID */
          else if (UL_RLC_ID_IS_INVALID(oc_dlcptr->rlc_id)) {
            MSG_2(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH , "DCCH/DTCH%d with invalid RLC ID %d",
                                                        j, oc_dlcptr->rlc_id);
          }
          /* Invalid MAC ID */
          else if ((ndlchan >1) && (UL_MAC_ID_IS_INVALID(oc_dlcptr->mac_id))) {
            MSG_2(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH , "DCCH/DTCH%d with invalid MAC ID %d",
                                                        j, oc_dlcptr->mac_id);
          }
          /* Invalid RB ID */
          else if (UL_RB_ID_IS_INVALID(oc_dlcptr->rb_id)) {
            MSG_2(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH , "DCCH/DTCH%d with invalid RB ID %d",
                                                        j, oc_dlcptr->rb_id);
          }
          /* Invalid MAC Priority */
          else if (UL_LC_PRIORITY_IS_INVALID(oc_dlcptr->priority)) {
            MSG_2(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH , "DCCH/DTCH%d with invalid priority %d",
                                                        j,
                                                        oc_dlcptr->priority);
          }
          /* Invalid RLC Mode */
          else if (UL_RLC_MODE_IS_INVALID(oc_dlcptr->rlc_mode)){
            MSG_2(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH , "DCCH/DTCH%d with invalid RLC Mode %d",
                                                        j,
                                                        oc_dlcptr->rlc_mode);
          }
          /* All Parameters are valid */
          else {
#ifdef FEATURE_DUMMY_SIGNALING
            #error code not present
#endif
            /* Internal logical channel config info pointer */
            int_lc_ptr = GET_UL_LC_INFO_PTR(oc_dlcptr->rlc_id);

            /* Clear header value */
            memset (hdrval, 0, MAC_UL_MAX_HDR_SIZE);

            /* Process Dedicated Logical Channel Config Info */
            DCCH_DTCH_HDR_ON_UL_DCH

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

            int_lc_ptr->rlc_size_info.all_tfi_valid = TRUE;

            /* Save Transport channel index */
            int_lc_ptr->trch_idx = i;

            // Get pointer to the transport channel trch_idx with priority level priority
            priority_info_ptr = GET_UL_TRCH_PRIORITY_INFO_PTR(int_lc_ptr->trch_idx, oc_dlcptr->priority);

            // Populate the logical channel id which has this priority.
            priority_info_ptr->lc_id[priority_info_ptr->num_lc] = oc_dlcptr->rlc_id;

            priority_info_ptr->num_lc++;

            /* Save the rlc size restrictions information */
            WCDMA_MEMCPY(&int_lc_ptr->rlc_size_info,
                         sizeof(mac_ul_rlc_size_info_type),
                         &oc_dlcptr->rlc_size_info,
                         sizeof(mac_ul_rlc_size_info_type));

            /* RLC Header Length */
            if (int_lc_ptr->rlc_mode == UE_MODE_TRANSPARENT)
            {
              int_lc_ptr->rlc_hdr_length = 0;
              mac_tm_bearers_count++;
            }
            else if (int_lc_ptr->rlc_mode == UE_MODE_UNACKNOWLEDGED)
              int_lc_ptr->rlc_hdr_length = 8;
            else
              int_lc_ptr->rlc_hdr_length = 16;

            /* Save the Logical channel ID in transport channel information */
            /* Update logical channel count of transport channel info */
            int_tc_ptr->rlc_id[int_tc_ptr->nlchan] = oc_dlcptr->rlc_id;
            int_tc_ptr->nlchan++;
            
            if(rlc_count<UE_MAX_UL_LOGICAL_CHANNEL)
            {
              mac_ul_lc_info.rlc_id[rlc_count++] = oc_dlcptr->rlc_id;
            }
            else
            {
              MSG_2(MSG_SSID_WCDMA_MAC, MSG_LEGACY_ERROR, "Index %d to get the LC id for which MAC needs the buffer status exceeds maximum",
                                                          rlc_count,
                                                          (int)UE_MAX_UL_LOGICAL_CHANNEL);
            }
            
            mac_ul_lc_info.nchan = rlc_count;

            MSG_3(MSG_SSID_WCDMA_MAC, MSG_LEGACY_LOW , "RLC%d:HdrSize= %d Hdr[0]= %x",
                  oc_dlcptr->rlc_id,
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
    }
}

/*===============================================================================================
FUNCTION NAME mac_ul_check_dch_trch

DESCRIPTION   This function checks if the input rlc id and rb id pair map to a UL-DCH transport channel
===============================================================================================*/

log_wcdma_trch_type mac_ul_check_dch_trch(rlc_lc_id_type rlc_id, rb_id_type rb_id)
{
  log_wcdma_trch_type return_trch_val = WCDMA_TRCH_INVALID;
  ul_lc_info_type *int_lc_ptr;

  /* Get the logical channel ptr corresponding to the rlc_id  */
  int_lc_ptr = GET_UL_LC_INFO_PTR(rlc_id);

  if(int_lc_ptr == NULL)
  {
    MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_ERROR, "mac_ul_check_dch_trch: GET_UL_LC_INFO_PTR returns NULL");
  }
  /* Check if given RB ID mactch logical channel's RB ID */
  else if(int_lc_ptr->rb_id == rb_id)
  {
    return_trch_val = WCDMA_TRCH_DCH;
  }
  else
  { 
    MSG_2(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "mac_ul_check_dch_trch: rrc_rb_id (%d) does not match l2_rb_id",
                                               rb_id, int_lc_ptr->rb_id);
  }
  return return_trch_val;
}

