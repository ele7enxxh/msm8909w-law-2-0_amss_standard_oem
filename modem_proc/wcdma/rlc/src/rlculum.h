#ifndef RLCULUM_H
#define RLCULUM_H
/*===========================================================================
                  U P L I N K   U N A C K N O W L E D G E D   M O D E   

               R A D I O   L I N K   C O N T R O L : H E A D E R  F I L E

DESCRIPTION
  This file contains data types and function prototypes associated with the
  Uplink Unacknowledged Mode RLC module.
  
Copyright (c) 2001 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

Copyright (c) 2003, 2006, 2008-2009 Qualcomm Technologies, Inc.  
All Rights Reserved.
===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/rlc/vcs/rlculum.h_v   1.4   22 May 2001 14:54:12   rchar  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rlc/src/rlculum.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/01/12   grk     Feature cleanup
06/26/08   av      l1-l2 interface cleanup 
05/29/09   ssg     Added support for SNOW_3G algo.
05/13/09   av      Fixed compiler error in non-datapath opt target 
05/11/09   ssg     Updated Copyright Information
04/29/09   av      Fixed compiler warnings in L2 
09/23/08   pj      Added prototype for rlc_um_form_pad_pdu_hdr()
           pj      Added prototype for rlci_ul_um_init(void);
05/26/08   pj      Added seperate SDU queues for EDCH & Non-EDCH UM channels
03/24/08   pj      Added support for data path optimizations under 
                   FEATURE_WCDMA_UL_DATA_PATH_OPT & FEATURE_WCDMA_DL_DATA_PATH_OPT
08/01/06   ttl     Add support to EUL.
10/28/03   ttl     Added rlci_re_establish_ul_um().
05/21/01   rc      Added more comments.
02/28/01   ttl     Moved prototype for rlci_ul_build_um_pdus() from rlcul.h.
01/31/01   rc      Moved functions prototypes rlci_ul_build_pdu_header(),
                   rlci_ul_build_pdu_data() and rlci_ul_insert_padding()
                   to rlctx.h since these functions are shared between 
                   Unacknowledged and acknowledged mode.
08/03/00   rc      Created file.

===========================================================================*/

/*---------------------------------------------------------------------------
** Include Files
**-------------------------------------------------------------------------*/

#include "wcdma_variation.h"
#include "rlcul.h"
#include "macrlcif.h"

/*===========================================================================
** Includes and Public Data Declarations
**=========================================================================*/
/*---------------------------------------------------------------------------
  Un acknowledge Mode definitions
---------------------------------------------------------------------------*/
#define RLC_UM_NEW_SDU_MASK           0x0002

/*---------------------------------------------------------------------------
  Length Indicator related definitions
---------------------------------------------------------------------------*/
#define RLC_UL_LI_START_SDU_7_BIT        0xF8    /* = (0x7C << 1)    */
#define RLC_UL_LI_START_SDU_15_BIT       0xFFF8  /* = (0x7FFC << 1)  */


/*==========================================================================

FUNCTION RLCI_RE_ESTABLISH_UL_UM

DESCRIPTION
  Re-establish RLC UM Channel in Uplink per RRC's request.

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.

==========================================================================*/
void rlci_re_establish_ul_um
(
  rlci_ul_ctrl_blk_type *ctl_blk_ptr,
  rlc_ul_um_channel_config_type *um_chan_info_ptr 
);

/*===========================================================================

FUNCTION RLCI_GET_UM_BUFFER_STATUS

DESCRIPTION
 This function calculates the buffer status of a Unacknowledged mode 
 channel. The number of bytes reported is the sum of:
 - Number of bytes in the uplink queue, 
 - The number of SDUs in the Uplink Queue (representative of the number of 
   Length Indicators) and
 - If the Special LI = TRUE, length of length indicator( 1 for 7bit LI and
   2 for 15-bit LI). 
  
DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void rlci_get_um_buffer_status
(
  rlci_ul_ctrl_blk_type     *ctrl_blk,   /* Pointer to Uplink Control Blk  */
  rlc_ul_lc_buf_status_type *status_ptr  /* Pointer to status bytes        */
);


/*===========================================================================

FUNCTION RLC_UL_BUILD_UM_PDUS

DESCRIPTION
  Builds uplink PDUs for transmission in a TTI. 
  Builds PDU header, data and padding (if any).
  Calculates the count,updates sequence number, handles incrementing of HFN 
  when the sequence number wraps.
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void rlci_ul_build_um_pdus 
(
  rlci_ul_ctrl_blk_type      *ctrl_blk,  /* Pointer to Uplink Control Blk  */
  rlc_ul_logchan_format_type *chan_info, /* PDU information for the channel*/
  rlc_ul_logchan_data_type   *data_ptr   /* PDUs built for the channel     */
);

/*===========================================================================

FUNCTION RLCI_UL_UM_INIT

DESCRIPTION
  Initialize the Up-link UM RLC.

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rlci_ul_um_init(void);

#ifdef FEATURE_WCDMA_HSUPA
/*===========================================================================

FUNCTION RLC_EUL_BUILD_UM_PDUS

DESCRIPTION
  Builds uplink PDUs for transmission in a TTI. 
  Builds PDU header, data and padding (if any).
  Calculates the count,updates sequence number, handles incrementing of HFN 
  when the sequence number wraps.

  In building PDUs for EDCH (EUL), the RLC directly write the PDU built into
  HW via a wrapper function from L1.
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void rlci_eul_build_um_pdus 
(
  rlci_ul_ctrl_blk_type      *ctrl_blk,  /* Pointer to Uplink Control Blk   */
  rlc_ul_logchan_format_type *chan_info, /* PDU information for the channel */
  boolean                     last_unit_of_tti
);
#endif //FEATURE_WCDMA_HSUPA


/*===========================================================================

FUNCTION  RLC_UL_ENH_BULD_UM_PDUS

DESCRIPTION

  Builds uplink E-DCH UM PDUs for transmission in a TTI. 

  PP.SW:
  RLC PDU payloads are provided as a set of IO vectors.  
  RLC HDR(+LIs) are stored in a header buffer as word format.
  
  7200A:
  RLC HDR(+LIs), RLC PDU payloads are all provided as set of IO vectors.

  The SDUs to be freed are stored in the free_sdu_q.
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None  .
  
SIDE EFFECTS
  None.

===========================================================================*/
void rlc_ul_enh_build_um_pdus
(
  rlci_ul_ctrl_blk_type      *ctrl_blk,   /* Pointer to Uplink Control Blk   */
  rlc_ul_logchan_format_type *chan_info,  /* PDU information for this LC   */
  l1_l2_data_type           *l1_ul_tx_buf_ptr
                                          /* Buffer to store IO vectors    */
);

/*===========================================================================

FUNCTION  RLC_UL_ENH_BULD_NON_EDCH_UM_PDUS

DESCRIPTION
  
  Builds uplink NON_E_DCH UM PDUs for transmission in a TTI. 

  The set of io vectors formed will be copied into a pre-allocated PDU
  dsm item. This dsm is passed to the lower layers.

  The SDUs to be freed are stored in the free_sdu_q.

DEPENDENCIES
  None.
  
RETURN VALUE
  uint16 - Num of PDUs built.
  
SIDE EFFECTS
  None.

===========================================================================*/
uint16 rlc_ul_enh_build_non_edch_um_pdus 
(
  rlci_ul_ctrl_blk_type      *ctrl_blk,   /* Pointer to Uplink Control Blk */
  rlc_ul_logchan_format_type *chan_info,  /* PDU information for this LC   */  
  rlc_ul_logchan_data_type   *data_ptr    /* valid if chnl_type = NON_E_DCH*/
);

/*===========================================================================

FUNCTION  RLC_UM_FREE_SDU_Q

DESCRIPTION
  
  This will free the SDU dsm chains transmitted in the previous TTI.
  This will execute in L2ul-task context.
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void rlc_um_free_sdu_q(rlc_ul_chnl_e_type ul_chnl_type);

/*===========================================================================

FUNCTION  RLC_UM_FORM_PAD_PDU_HDR

DESCRIPTION
    This will form the PDU header for the PAdding PDUs
    This also inserts the special LIs (if any) in the current PDU
    
DEPENDENCIES
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean rlc_um_form_pad_pdu
(
  rlci_ul_ctrl_blk_type   *ctrl_blk,    /* Pointer to Uplink Control Blk */
  uint16                  rem_pdu_size,
  l1_l2_data_type        *l1_ul_tx_buf_ptr,
  rlc_ul_pdu_data_type      *pdu_info   /* valid if chnl_type = NON_E_DCH */
);

/*===========================================================================

FUNCTION RLC_UM_GET_SDU_FREE_Q

DESCRIPTION

  Returns the pointer to the global header free SDU queue.
  Initialize the buffer_ptr of the SDU queue to the 
  corresponding Buffer (EDCH/ Non-EDCH) based on channel type

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
rlc_um_sdu_q_type* rlc_um_get_sdu_free_q(rlc_ul_chnl_e_type chnl_type);




#ifdef FEATURE_RLC_QXDM_LOGGING
/*===========================================================================

FUNCTION  RLC_UL_STORE_UM_CIPHER_LOG

DESCRIPTION

  Stores the ciphering related information for UM log packets.
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.  

SIDE EFFECTS
  None.

===========================================================================*/
void rlc_ul_store_um_cipher_log
(
  rlc_lc_id_type          rlc_id,     /* rlc_id of the LC     */
  cipher_mode_e_type      mode,       /* Ciphering mode       */
  uint32                  key_id,     /* Ciphering key-id     */
  uint32                  cnt_c,      /* Ciphering count_c    */
  uecomdef_wcdma_cipher_algo_e_type  cipher_algo /* Ciphering algorithm  */
);

#endif

/*===========================================================================

FUNCTION  RLC_UL_UM_GET_CIPHER_PARAMS

DESCRIPTION
  
  This function computes the UM ciphering related parameters.
  
DEPENDENCIES
  None.
  
RETURN VALUE
  o/p parameters: key_index, enabled, cnt_c
  
SIDE EFFECTS
  None.

===========================================================================*/
uecomdef_wcdma_cipher_algo_e_type rlc_ul_um_get_cipher_params
(
  rlci_ul_ctrl_blk_type *ctrl_blk_ptr,  /* Pointer to ctrl_blk      */
  uint32                *key_index,     /* Pointer to key Id        */
  boolean               *enabled,       /* Pointer to enabled flag  */
  uint32                *cnt_c          /* Pointer to count_c       */
);


/*===========================================================================

FUNCTION RLC_UL_UM_FORM_PDU_PAYLOAD

DESCRIPTION

  This will form the set of IO vectors for the UM PDU payload.

  E_DCH:
  For a E_DCH channel, IO vectors are store in L1 buffer

  NON_E_DCH:
  For a NON_E_DCH channel, Io vectors are store in RLC global array

DEPENDENCIES
  None.

RETURN VALUE
  uint16 - padding length in this PDU

SIDE EFFECTS
  None.

===========================================================================*/
uint16 rlc_ul_um_form_pdu_payload
(
  rlci_ul_ctrl_blk_type *ctrl_blk,            /* Pointer to ctrl_blk          */
  uint16                pdu_size_bytes,       /* PDU size in bytes            */  
  rlc_buff_type         *hdr_list,            /* Pointer to local header list */ 
  l1_l2_data_type      *l1_ul_tx_buf_ptr,     /* Place to store IO vectors    */
  rlc_ul_chnl_e_type    ul_chnl_type,         /* Channel type, EDCH / NonEDCH */
  boolean               *build_status        /* Returns PDU building status*/
);

/*===========================================================================

FUNCTION    RLC_UL_ENH_UM_ALT_E_BIT_LI

DESCRIPTION

  This function computes if a complete SDU fits in a PDU in alternate e-bit
  interpretation for different cases like
  1. E = 0
  2. E = 1, LI = 0x7d/0x7ffd, LI = 0x7FFA
  needs to inserted in this PDU.

DEPENDENCIES
  None.

RETURN VALUE
  uint16  - No. of bytes(LI) inseretd.
            0xFFFF if doesnot meet any conditions.
  
SIDE EFFECTS
  None.

===========================================================================*/
uint16 rlc_ul_enh_um_alt_e_bit_li
(
  uint16                payload_size,        /* Size of PDU excluding header */
  rlc_buff_type         *hdr_list,     /* Length of LI                 */
  rlci_ul_ctrl_blk_type *ctrl_blk_ptr    /* Pointer to ctrl_blk          */
);


#endif
