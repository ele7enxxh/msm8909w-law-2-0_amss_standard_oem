#ifndef RLCDLUM_H
#define RLCDLUM_H
/*===========================================================================
            D O W N L I N K   U N A C K N O W L E D G E D   M O D E 

           R A D I O   L I N K   C O N T R O L : H E A D E R  F I L E

DESCRIPTION
  This file contains data types and function prototypes associated with the
  Downlink Unacknowledged Mode RLC module.
  
Copyright (c) 2001 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

Copyright (c) 2002-2003, 2005-2006, 2008-2009 Qualcomm Technologies, Inc.  
All Rights Reserved.
===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/rlc/vcs/rlcdlum.h_v   1.7   08 May 2002 19:13:42   tliou  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rlc/src/rlcdlum.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/01/12   grk     Feature cleanup
22/04/10   pj      Modified cipher function to accept DL-cipher info ptr
09/09/09   grk     Fix to populate app_ptr for DUP DSM items.
05/11/09   ssg     Updated Copyright Information
04/29/09   av      Fixed compiler warnings in L2 
03/03/08   ssg     Added support to alternate e-bit interpretations under the
                   featue flag FEATURE_RLC_ALTERNATE_E_BIT.
06/12/08   ssg     Added function prototype for releasing DL UM channel.
03/24/08   pj      Added support for data path optimizations under 
                   FEATURE_WCDMA_UL_DATA_PATH_OPT & FEATURE_WCDMA_DL_DATA_PATH_OPT
11/30/06   ssg     Added function prototypes rlci_establish_dl_um(),
                   rlci_modify_dl_um().
03/09/05   mv      Added the support for BMC under FEATURE_UMTS_BMC.
03/02/05   mv      Moved the function definition of
                   rlci_dl_um_check_pdu_validity() from rlcdlum.c
10/28/03   ttl     Added rlci_re_establish_dl_um().
10/30/02   ttl     Added Cipher restart feature.
09/05/02   ttl     Added Ciphering feature.
05/08/02   ttl     Changed the prototype of rlci_dl_reassemble_um_pdus().
06/28/01   rc      Removed prototype for rlc_dl_um_calculate_count().
05/21/01   rc      Added more comments.
02/13/01   rc      rlci_dl_um_adjust_pdu_pointer() does not take discard 
                   flag as parameter.
01/31/01   rc      Added prototype for rlci_dl_um_adjust_pdu_pointer().
01/08/00   rc      Added description to the prototype.
11/22/00   rc      Created file.

===========================================================================*/


/*===========================================================================
** Includes and Public Data Declarations
**=========================================================================*/

/*---------------------------------------------------------------------------
** Include Files
**-------------------------------------------------------------------------*/

#include "rlcdl.h"
#include "macrlcif.h"


/*==========================================================================

FUNCTION RLCI_ESTABLISH_DL_UM

DESCRIPTION
  Establish RLC UM Channel in Downlink per RRC's request.

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.

==========================================================================*/
void rlci_establish_dl_um
(
  rlci_dl_ctrl_blk_type *ctrl_blk_ptr,
  rlc_dl_um_channel_config_type *chan_info_ptr,
  rlc_lc_action_type *act_ptr
);

/*==========================================================================

FUNCTION RLCI_RE_ESTABLISH_DL_UM

DESCRIPTION
  Re-establish RLC UM Channel in Downlink per RRC's request.

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.

==========================================================================*/
void rlci_re_establish_dl_um
(
  rlci_dl_ctrl_blk_type *ctrl_blk_ptr,
  rlc_dl_um_channel_config_type *chan_info_ptr  
);

/*===========================================================================

FUNCTION RLCI_MODIFY_DL_UM

DESCRIPTION
  Modify an RLC UM entity in Downlink per RRC's request.

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rlci_modify_dl_um
(
  rlci_dl_ctrl_blk_type *ctl_blk_ptr,
  rlc_dl_um_channel_config_type *chan_info_ptr,
  rlc_lc_action_type *act_ptr  
);

/*===========================================================================

FUNCTION RLCI_RELEASE_DL_UM

DESCRIPTION
  Releases RLC UM Channel in Downlink per RRC's request.

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rlci_release_dl_um
(
  rlci_dl_ctrl_blk_type *ctrl_blk_ptr
);

/*===========================================================================

FUNCTION RLCI_DL_REASSEMBLE_UM_PDUS

DESCRIPTION
  Reassembles PDUs that arrive from MAC in the TTI. If the PDUs arrive
  out of order, the PDUs for the SDU(s) are discrded. If a PDU arrives 
  in-sequence, and has a length indicator, extract the first LI and its
  associated data(if any), chain it to the pdu packet chain, and enqueue
  it on to the downlink watermark for the channel. In addition, if there 
  are more length indicators in the PDU, extract them and enqueue the 
  SDUs on to the watermark. If the PDU arrives in sequence, and does not 
  have a length indicator, just chain the PDU to the packet chain.
  If the PDU arrives out of sequence, and the PDU does not have a length
  indicator, just chain the PDU to the packet and set discard flag to TRUE.
  IPDU arrives out of sequence and the PDU has a length indicator, extract
  the first LI and its associated data and discard it and discard the PDUs 
  in the packet chain. If there are more length indicators in the PDU, 
  extract them, and enqueue them on to the watermark.
 
DEPENDENCIES
  None.
  
RETURN VALUE
  Number of SDUs enqueued into WM.
  
SIDE EFFECTS
  None.

===========================================================================*/
uint8 rlci_dl_reassemble_um_pdus
(
  rlci_dl_ctrl_blk_type  *ctrl_blk,   /* Pointer to Downlink Control Block */
  rlci_dl_chan_data_type *data_ptr    /* Downlink PDU informationn         */
);

/*==========================================================================

FUNCTION RLCI_DL_UM_CHECK_PDU_VALIDITY

DESCRIPTION

  Checks for the validity of the PDU by checking the value of LIs
  as they appear in a PDU. If any LI is invalid or points to beyond the
  end of PDU, the discard flag is set to true which causes this PDU and any
  SDU whose parts were contained in this PDU to be discarded.
  
DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the PDU has valid LIs and FALSE otherwise

SIDE EFFECTS
  None.

===========================================================================*/

boolean rlci_dl_um_check_pdu_validity
(
  dsm_item_type         *item_ptr,      
  uint16                li_len,
  uint16                hdr_len,
  uint8                 *num_li,
  boolean              alt_e_bit
);

/*===========================================================================

FUNCTION RLCI_DL_UM_CIPHER_PDU

DESCRIPTION
  Check if this receiving PDU needs to be de-cipher.
  
  There is no retx PDU in UM. So, when cipher_mode changes from
  CIPHER_MOD to CIPHER_ON, there is no need to remember the old 
  cipher config.
  
DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the PDU needs to be de-ciphering. FALSE, otherwise 
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean rlci_dl_um_cipher_pdu
(
  rlci_dl_ctrl_blk_type *ctl_blk_ptr,
  rlc_dl_tb_cipher_info_type *cipher_ptr,
  uint16 rx_sn
);

/*===========================================================================

FUNCTION RLCI_DL_UM_GET_COUNT_C

DESCRIPTION
  Obtain the Count_c of ciphering for this SN.
  
  If there is a wrap around, the real distance between the rx_sn and
  the VR_UR will be greater than .
  
  If rx_sn is greater than VR_UR, there is a forward wrap around.
  If rx_sn is less than VR_UR, there is a backward wrap around. 
  
DEPENDENCIES
  None.

RETURN VALUE
  Count_c for ciphering.
  
SIDE EFFECTS
  None.

===========================================================================*/
uint32 rlci_dl_um_get_count_c
(
  rlci_dl_ctrl_blk_type *ctl_blk_ptr,
  uint16 rx_sn
);



/*===========================================================================

FUNCTION rlci_enh_update_partial_sdu_ptr

DESCRIPTION
  Update the Partial SDU Contents

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rlci_enh_update_um_partial_sdu_ptr(rlci_dl_ctrl_blk_type *ctrl_blk_ptr,
                                     rlci_dl_temp_pdu_info_type *dl_pdu_info_ptr,
                                     dsm_item_type *last_pdu_item_ptr);

/*===========================================================================

FUNCTION rlci_enh_dl_process_um_pdus

DESCRIPTION
    - Process UM data 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rlci_enh_dl_process_um_pdus(rlci_dl_ctrl_blk_type *ctrl_blk_ptr,
                                 complete_pdu_type *complete_pdu_ptr);

/*===========================================================================

FUNCTION rlci_enh_discard_flag_handling

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
boolean rlci_enh_discard_flag_handling(rlci_dl_ctrl_blk_type *ctrl_blk);

/*===========================================================================

FUNCTION rlci_enh_get_temp_pdu_info_ptr

DESCRIPTION
   Get the pointer to global variable

DEPENDENCIES
  None.

RETURN VALUE
  Returns the pointer to global variable

SIDE EFFECTS
  None.

===========================================================================*/
rlci_dl_temp_pdu_info_type *rlci_enh_get_temp_pdu_info_ptr(void);

/*===========================================================================

FUNCTION rlc_enh_establish_dl_um

DESCRIPTION
  Clear the contents of dl_um_pdu_info.
  Partial SDU related pointers are set to NULL.
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rlc_enh_establish_dl_um(rlci_dl_ctrl_blk_type *ctrl_blk_ptr);

/*===========================================================================

FUNCTION rlci_enh_um_discard_flag_handling

DESCRIPTION
  Discard the partial SDU contents and temp PDU Info contents
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rlci_enh_um_discard_flag_handling(rlci_dl_ctrl_blk_type *ctrl_blk);

/*===========================================================================

FUNCTION rlci_enh_dl_um_reassemble_data_pdus

DESCRIPTION
    - Complete the reassembly functionality for this PDU

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rlci_enh_dl_um_reassemble_data_pdus(rlci_dl_ctrl_blk_type *ctrl_blk,
                                         rlc_pdu_info_type     *walk_item_ptr);


#endif
