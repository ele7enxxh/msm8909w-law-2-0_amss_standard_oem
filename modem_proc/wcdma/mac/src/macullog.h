#ifndef MACULLOG_H
#define MACULLOG_H


/*===========================================================================
               U P L I N K   M A C    L O G G I N G   H E A D E R    F I L E

DESCRIPTION
  This file provides the function prototypes for MAC uplink logging.
  
EXTERNALIZED FUNCTIONS
  List any functions that are externalized in this file.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  List any initialization and sequencing that is required for this module.

Copyright (c) 2001 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

Copyright (c) 2002, 2008-2009 Qualcomm Technologies, Inc.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mac/vcs/macullog.h_v   1.1   24 Aug 2001 15:30:52   rchar  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/mac/src/macullog.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when      who     what, where, why
--------  ---     ----------------------------------------------------------
06/23/15  aa      Changes for new log packet 0x41CE
08/07/13  kc      Log packet support for UL MAC cipher parameters
10/12/12  kc      HS_RACH MAC F3's moved under debug variable and x4314 log packet issue
01/02/12  grk     Feature cleanup
05/19/10  mrg     Header guard "MACULLOG_H" typo error corrected 
05/07/09  ssg     Updated Copyright Information
04/29/09  av      Fixed compiler warnings in L2 
04/23/09  mrg     Function mac_ul_event_amr_rate() is removed.
10/27/08  ssg     Added function mac_ul_event_amr_rate()
11/15/02  sk      Added function mac_ul_send_tvm_report_packet():
08/24/01  rc      Moved FEATURE definition to after the include definitions.
08/21/01  rc      Created file.

===========================================================================*/

/* ==========================================================================
** Includes and Variable Definitions
** ========================================================================*/

/* --------------------------------------------------------------------------
** Include Files
** ------------------------------------------------------------------------*/
#include "maclog.h"
#include "macrrcif_v.h"
#include "macinternal.h"

#ifdef FEATURE_WCDMA_HS_RACH
extern boolean mac_hs_rach_f3_enable;
#endif

typedef struct {
  uint32 countc;
  uint8  cfn;
  uint8 key_idx;
}mac_ul_cipher_log_pkt_type;

/*===========================================================================

FUNCTION MAC_UL_SEND_CFG_LOG_PACKET

DESCRIPTION
   This function prepares a log packet for configured Uplink MAC Transport 
   Channels and sends the packet to log services.  

DEPENDENCIES
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void mac_ul_send_cfg_log_packet
(
  mac_ul_config_type **config_ptr   /* Address of Ptr to Uplink MAC config */
);

/*===========================================================================

FUNCTION MAC_UL_SEND_RACH_PARAM_PACKET

DESCRIPTION
   This function prepares a log packet for RACH Transmit parameters and sends
   the packet to log services.

DEPENDENCIES
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void mac_ul_send_rach_param_packet
(
  mac_ul_rach_txcntl_config_type *rach_tx_prmtrs /* Ptr to RACH TX params  */
);

/*===========================================================================

FUNCTION MAC_UL_SEND_TFCS_CFG_PACKET

DESCRIPTION
   This function prepares a log packet for Uplink MAC TFCS configuration 
   and sends the packet to log services.

DEPENDENCIES
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void mac_ul_send_tfcs_cfg_packet
(
  mac_ul_tfcs_type *tfcs_ptr,             /* Ptr to TFCS info              */
  uint8            *trch_id               /* Transport Channel IDs         */
);
/*===========================================================================

FUNCTION mac_ul_send_tvm_report_packet

DESCRIPTION
   This function prepares a log packet for Uplink MAC Traffic volume report 
   information and sends the packet to log services.

DEPENDENCIES
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void mac_ul_send_tvm_report_packet
(
  mac_ul_traffic_meas_ind_type *tvm_rpt_ptr,
  uint16    nreports_sent
);

/*===========================================================================

FUNCTION MAC_UL_SEND_CFN_LOG_PACKET

DESCRIPTION

DEPENDENCIES
    None.

RETURN VALUE
    None.

SIDE EFFECTS
    None.

===========================================================================*/
void mac_ul_send_cfn_log_packet
(
  uint8 nsamples
);

/*===========================================================================

FUNCTION MAC_UL_SEND_CIPHER_CONFIG_LOG_PACKET

DESCRIPTION
  Logs ciphering parameters for TM channels at MAC

===========================================================================*/
void mac_ul_send_cipher_config_log_packet(mac_cipher_config_type *cipher_ptr);

/*===========================================================================

FUNCTION MAC_UL_CONVERT_TVM_BUF_PAYLOAD

DESCRIPTION

DEPENDENCIES
    None.

RETURN VALUE
    None.

SIDE EFFECTS
    None.

===========================================================================*/
uint32 mac_ul_convert_tvm_buf_payload
    (
    uint32    payload
    );

#endif /* MACULLOG_H */
