#ifndef RLCDLLOG_H
#define RLCDLLOG_H

/*===========================================================================
         D O W N L I N K   R L C    L O G G I N G  H E A D E R   F I L E

DESCRIPTION
  This file provides the function prototypes for RLC downlink logging.
  
EXTERNALIZED FUNCTIONS
  List any functions that are externalized in this file.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  List any initialization and sequencing that is required for this module.

Copyright (c) 2001 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

Copyright (c) 2002-2006, 2008-2009 Qualcomm Technologies, Inc.  
All Rights Reserved.
===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/rlc/vcs/rlcdllog.h_v   1.5   12 Jun 2002 18:44:00   tliou  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rlc/src/rlcdllog.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when      who     what, where, why
--------  ---     ----------------------------------------------------------
06/09/15  aa      Replaced RLC UM/AM/TM config log packets with new packets
02/01/12  grk     Feature cleanup
10/26/09  pj      Added prototype for UM-LOG packet function
05/11/09  ssg     Updated Copyright Information
10/17/08  grk     Merged taxis changes to mainline.
03/24/08  pj      Added support for data path optimizations under 
                  FEATURE_WCDMA_UL_DATA_PATH_OPT & FEATURE_WCDMA_DL_DATA_PATH_OPT
11/23/06  vg      added UM DL/UL logging support under feature FEATURE_UM_PDU_LOGGING
04/05/05  aw      Removed Log_on_demand feature.
08/19/04  mv      Separated the RLC PDU logging for the user plane and 
                  signaling plane logical channels.
12/06/03  ttl     Declare rlc_dl_tx_pdu_cipher_log_pkt().
11/05/02  ttl     Added logging for ciphering config.
06/12/02  ttl     Added rlc_dl_tx_am_pdu_log_pkt() and rlc_dl_tx_am_nak_pdu_log_pkt().
04/24/02  ttl     Rewrote the statistic log code.
04/03/02  ttl     Added Log_on_demand feature.
03/15/02  sk      Added function definition rlc_dl_send_am_stat_packet.
08/24/01  rc      Moved FEATURE definition to after the include definitions.
08/20/01  rc      Created file.

===========================================================================*/


/* ==========================================================================
** Includes and Variable Definitions
** ========================================================================*/

/* --------------------------------------------------------------------------
** Include Files
** ------------------------------------------------------------------------*/
#include "wcdma_variation.h"
#include "rlclog.h"
#include "rlcrrcif_v.h"
#include "rlcdl.h"

/* ==========================================================================
** Includes and Variable Definitions
** ========================================================================*/
 extern uint32 rlci_log_mask;

#ifdef FEATURE_RLC_QXDM_LOGGING
/*===========================================================================

FUNCTION RLC_DL_SEND_TM_LOG_PACKET

DESCRIPTION
  This function prepares a log packet for configured Downlink RLC Transparent 
  Mode channels and sends the packet to log services.

DEPENDENCIES
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void rlc_dl_send_tm_log_packet
(
  rlc_dl_tm_config_type *tm_ptr  /* Ptr to Transparent Mode Config Info    */
);


/*===========================================================================

FUNCTION RLC_DL_SEND_UM_LOG_PACKET

DESCRIPTION
  This function prepares a log packet for configured Downlink RLC 
  Unacknowledged Mode channels and sends the packet to log services.    

DEPENDENCIES
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void rlc_dl_send_um_log_packet
(
  rlc_dl_um_config_type *um_ptr,  /* Ptr to Unacknowledged Mode Config Info */
  boolean *est_or_reest
);


/*===========================================================================

FUNCTION RLC_DL_SEND_AM_LOG_PACKET

DESCRIPTION
  This function prepares a log packet for configured Downlink RLC 
  Acknowledged Mode channels and sends the packet to log services.    

DEPENDENCIES
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void rlc_dl_send_am_log_packet
(
  rlc_am_config_req_type *am_ptr, /* Ptr to Acknowledged Mode Config Info   */
  boolean *est_or_reest
);


/*===========================================================================

FUNCTION RLC_DL_SEND_STATE_LOG_PACKET

DESCRIPTION
  This function prepares a log packet with state information for Downlink RLC 
  logical channels and sends the packet to log services.      

DEPENDENCIES
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void rlc_dl_send_state_log_packet
(
  rlc_dl_log_state_type *log_ptr      /* Pointer to State Information      */
);

/*===========================================================================

FUNCTION RLC_DL_SEND_AM_STAT_PACKET

DESCRIPTION
  This function prepares a log packet with statistics information for Downlink RLC 
  logical channels and sends the packet to log services.      

DEPENDENCIES
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void rlc_dl_tx_am_statistic_pkt
(
  rlci_downlink_table_type *log_ptr      /* Pointer to State Information   */
);


/*===========================================================================

FUNCTION RLC_DL_TX_AM_SIG_PLANE_PDU_LOG_PKT

DESCRIPTION
  This function prepares a log packet with PDU size, data information for 
  Downlink RLC signalling plane logical channels and sends the packet to 
  log services.      

DEPENDENCIES
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void rlc_dl_tx_am_sig_plane_pdu_log_pkt
(
  rlci_downlink_table_type *dl_tbl_ptr,  /* Pointer to  pdu  Information */
  uint16                    num_am_ch
  
);

/*===========================================================================

FUNCTION RLC_DL_TX_AM_USR_PLANE_PDU_LOG_PKT

DESCRIPTION
  This function prepares a log packet with PDU size, data information for 
  Downlink RLC user plane logical channels and sends the packet to 
  log services.      

DEPENDENCIES
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void rlc_dl_tx_am_usr_plane_pdu_log_pkt
(
  rlci_downlink_table_type *dl_tbl_ptr,  /* Pointer to  pdu  Information */
  uint16                    num_am_ch
  
);

/*===========================================================================

FUNCTION RLC_DL_TX_AM_NAK_PDU_LOG_PKT

DESCRIPTION
  This function prepares a log packet with NAK control PDU information for 
  Downlink RLC logical channels and sends the packet to log services.      

DEPENDENCIES
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void rlc_dl_tx_am_nak_pdu_log_pkt
(
  rlc_lc_id_type rlc_id,
  uint16         pdu_size,
  dsm_item_type  *pdu_ptr
  , uint16          offset

);

/*===========================================================================

FUNCTION  RLC_DL_TX_CIPHER_LOG_PACKET

DESCRIPTION
  Send DL Ciphering config LOG packet to DIAG.

DEPENDENCIES
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void rlc_dl_tx_cipher_log_packet
(
  rlc_dl_cipher_config_type *cipher_ptr  /* Ptr to Downlink Cipher Config.   */
);

/*===========================================================================

FUNCTION  RLC_DL_TX_CIPHER_PDU_LOG_PKT

DESCRIPTION
  Send ciphering PDU parameters log to DIAG.

DEPENDENCIES
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void rlc_dl_tx_pdu_cipher_log_pkt
(
  rlc_dl_pdu_cipher_info_type *rlci_dl_pdu_cipher_log
);


/*===========================================================================

FUNCTION RLC_DL_TX_UM_USR_PLANE_PDU_LOG_PKT

DESCRIPTION
  This function prepares a log packet with PDU size, data information for
  Downlink RLC UM logical channels and sends the packet to
  log services.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

#ifdef FEATURE_UM_PDU_LOGGING
void rlc_dl_tx_um_pdu_log_pkt
(
  rlci_downlink_table_type  *dl_tbl_ptr, /* Pointer to pdu log information */
  uint16                     num_um_ch
);
#endif
#endif /* FEATURE_RLC_QXDM_LOGGING */

#if ((defined (FEATURE_HSDPA_MAC_EHS)) && (defined (FEATURE_WCDMA_REL7)))
/*===========================================================================

FUNCTION RLC_DL_R7_FLEX_AM_PDU_LOG_PKT

DESCRIPTION
  This function prepares a log packet with pdu information for 
  Downlink RLC logical channels and sends the packet to log services.      

DEPENDENCIES
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void rlc_dl_enh_r7_am_pdu_log_pkt(rlci_downlink_table_type  *dl_tbl_ptr);

#endif

/*===========================================================================

FUNCTION RLC_DL_ENH_TX_AM_PDU_LOG_PKT

DESCRIPTION
  This function prepares a log packet with pdu information for 
  Downlink RLC logical channels and sends the packet to log services.      

DEPENDENCIES
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void rlc_dl_enh_tx_am_pdu_log_pkt(rlci_dl_ctrl_blk_type *ctrl_blk_ptr, uint8 lc_id);

#ifdef FEATURE_UM_PDU_LOGGING
/*===========================================================================

FUNCTION RLC_DL_ENH_TX_UM_USR_PLANE_PDU_LOG_PKT

DESCRIPTION
  This function prepares a log packet with pdu information for
  Downlink UM logical channels and sends the packet to
  log services.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rlc_dl_enh_tx_um_pdu_log_pkt(rlci_dl_ctrl_blk_type *ctrl_blk_ptr);
#endif

/*===========================================================================

FUNCTION rlci_enh_dl_log_info_handler

DESCRIPTION
This function checks the status of AM and UM log packets and appropriately 
sets the log_mask bits. 
Following bit configuration is used: 
RLCI_UM_USR_PLANE_LOG_MASK : 0x01
RLCI_AM_USR_PLANE_LOG_MASK : 0x02
RLCI_AM_SIG_PLANE_LOG_MASK : 0x04

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void rlci_enh_dl_log_info_handler(void);
#endif /* RLCDLLOG_H */
