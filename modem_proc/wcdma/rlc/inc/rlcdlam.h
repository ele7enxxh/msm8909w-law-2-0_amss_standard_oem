#ifndef RLCDLAM_H
#define RLCDLAM_H
/*===========================================================================
                  D O W N L I N K   A C K N O W L E D G E D   M O D E   

               R A D I O   L I N K   C O N T R O L : H E A D E R  F I L E

DESCRIPTION
  This file contains data types and function prototypes associated with the
  downlink acknowledged Mode RLC module.
  
Copyright (c) 2001 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

Copyright (c) 2002-2003, 2008-2009 Qualcomm Technologies, Inc.  
All Rights Reserved.
===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/rlc/vcs/rlcdlam.h_v   1.5   08 May 2002 19:13:30   tliou  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rlc/inc/rlcdlam.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/30/14   aa/kv   Fix compiler warnings on Bolt
11/27/13   scv     Code changes for UL Compression feature
05/20/13   as      Added code to validate STATUS PDU before processing it.
02/01/12   grk     Feature cleanup
04/25/11   grk     Fix to skip updating count_c/hfn when old ciphering configuration
                   is used.
04/13/10   grk     changes for ZI memory reduction.
12/08/09   grk     Fix to pass NAK length as uint16 when storing 
                   NAKs(SN-LI pairs) in retransmission Q.
05/11/09   ssg     Updated Copyright Information
04/29/09   av      Fixed compiler warnings in L2 
10/17/08   grk     Merged taxis changes to mainline.
03/24/08   pj      Added support for data path optimizations under 
                   FEATURE_WCDMA_UL_DATA_PATH_OPT & FEATURE_WCDMA_DL_DATA_PATH_OPT
10/28/03   ttl     Added cipher_flag parameter to rlci_dl_am_get_count_c().
10/07/03   ttl     Added cipher_key_id as a new parameter of rlci_dl_am_get_count_c().
08/20/03   ttl     Merged branch code into tip.
08/01/03   mv      Added rlci_re_establish_dl_am() definition.
10/30/02   ttl     Added Cipher restart feature.
09/05/02   ttl     Added Ciphering feature.
07/31/02   ttl     Removed rlci_dl_am_handle_10ms_timer().
05/08/02   ttl     Changed the prototype of rlci_dl_process_am_pdus().
08/30/01   rc      Renamed rlci_dl_reassemble_data_pdus as
                   rlci_dl_am_reassemble_data_pdus.
05/21/01   rc      Added function prototypes for Processing Downlink PDUs and
                   reassembling Data PDUs.
05/02/01   ttl     Added function declarations for RESET procedure.
02/28/01   ttl     Created file.

===========================================================================*/


/*===========================================================================
** Includes and Public Data Declarations
**=========================================================================*/

/*---------------------------------------------------------------------------
** Include Files
**-------------------------------------------------------------------------*/

#include "wcdma_variation.h"
#include "rlcdl.h"
#include "rlcrrcif_v.h"
#include "macrlcif.h"

/*===========================================================================
**                      TYPE DECLARATIONS
**=========================================================================*/

/*---------------------------------------------------------------------------
**    Public Function Declaration
**-------------------------------------------------------------------------*/
/*===========================================================================

FUNCTION RLCI_DL_AM_INIT

DESCRIPTION
  Initialize the Down-link AM RLC.
  
DEPENDENCIES

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void rlci_dl_am_init(void);

/*===========================================================================

FUNCTION  RLCI_ESTABLISH_DL_AM

DESCRIPTION
  Config and establish an RLC AM in Downlink per RRC's request.
  
DEPENDENCIES

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
byte rlci_establish_dl_am
(
  rlci_dl_ctrl_blk_type *ctl_blk_ptr,
  rlc_am_config_type *am_ptr
#ifdef FEATURE_HSDPA_MAC_EHS
  , byte *result
#endif
);

/*==========================================================================

FUNCTION RLCI_RE_ESTABLISH_DL_AM

DESCRIPTION
  Re-establish RLC AM entity in Downlink per RRC's request.

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.

==========================================================================*/
void rlci_re_establish_dl_am
(
  rlci_dl_ctrl_blk_type *ctl_blk_ptr,
  rlc_am_config_type *am_ptr
);

/*===========================================================================

FUNCTION RLCI_RELEASE_DL_AM

DESCRIPTION
  Release an RLC AM entity in Downlink per RRC's request.
  
DEPENDENCIES

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
byte rlci_release_dl_am
(
  rlci_dl_ctrl_blk_type *ctl_blk_ptr
);

/*===========================================================================

FUNCTION RLCI_MODIFY_DL_AM

DESCRIPTION
  Modify an RLC AM entity in Downlink per RRC's request.
  
DEPENDENCIES

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void rlci_modify_dl_am
(
  rlci_dl_ctrl_blk_type *ctl_blk_ptr,
  rlc_am_config_type *am_ptr
);



/*===========================================================================

FUNCTION RLCI_RESET_DL_AM

DESCRIPTION
  Reset DL AM RLC. 
  When DL AM RLC rx a RESET request from the peer side, the ul_reset_req
  has to be set to TRUE.
  If the ul_reset_req is TRUE, post a RESET request message to the 
  UL AM RLC as well.
  
DEPENDENCIES

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void rlci_reset_dl_am
(
  rlci_dl_ctrl_blk_type *ctl_blk_ptr,
  boolean ul_reset_req
);


/*===========================================================================

FUNCTION RLCI_DL_PROCESS_AM_PDUS

DESCRIPTION
  Processes Data and Control PDUs.
  If the PDU is a control PDU and has been received on the right channel, 
  process it. Otherwise delete the PDU.
  If the PDu is a data PDU and has been received on the right channel, 
  inser the PDU into the resequencing Queue and update VR(R), VR(MR) and
  VR(H). Store the sequence number and extension bit in the app_field of
  the dsm item.
  After all the PDUs have been processed, reassemble the data PDUs into
  SDUs till you reach VR(R), since only in-sequence PDUs can be 
  reassembled. If the polling bit was set in the incoming PDU, build a 
  NAK list of all the missing PDUs and enqueue them on to the 
  NAK Queue.
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
uint8 rlci_dl_process_am_pdus
(
  rlci_dl_ctrl_blk_type  *ctrl_blk,        /* Pointer to the Control Block */
  rlci_dl_chan_data_type *data_ptr         /* Pointer to the PDUs Received */
);

/*===========================================================================

FUNCTION RLCI_DL_AM_REASSEMBLE_DATA_PDUS

DESCRIPTION

  Reassembles all in-sequence PDUs(till VR(R)) and enqueues them on to the 
  downlink watermark for that channel. Updates value of VR(R) and VR(MR).
  
DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void rlci_dl_am_reassemble_data_pdus
(
  rlci_dl_ctrl_blk_type *ctrl_blk     /* Pointer to Downlink Control Block */
);

/*===========================================================================

FUNCTION RLCI_DL_AM_CIPHER_PDU

DESCRIPTION
  Check if this PDU needs to be de-ciphering.
  
DEPENDENCIES
  None.

RETURN VALUE
  Count_c for ciphering.
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean rlci_dl_am_cipher_pdu
(
  rlci_dl_ctrl_blk_type *ctl_blk_ptr,
  uint16 rx_sn
);

/*===========================================================================

FUNCTION RLCI_DL_AM_GET_COUNT_C

DESCRIPTION
  Obtain the Count_c of ciphering for this SN.
  
  If there is a wrap around, the real distance between the rx_sn and
  the VR_H will be greater than 2048.
  
  If rx_sn is greater than VR_H, there is a forward wrap around.
  If rx_sn is less than VR_H, there is a backward wrap around. 
  
DEPENDENCIES
  None.

RETURN VALUE
  Count_c for ciphering.
  
SIDE EFFECTS
  None.

===========================================================================*/
uint32 rlci_dl_am_get_count_c
(
  rlci_dl_ctrl_blk_type *ctl_blk_ptr,
  uint16 rx_sn,
  uint32 *cipher_key_id,
  boolean *cipher_flag,
  boolean *old_config
);
/*===========================================================================

FUNCTION rlc_enh_free_reseq_q

DESCRIPTION
   Free the Resequencing Queue PDUs from start_sn to end_sn
   Free the partial Sdu ptr info and other temporary SDU info

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rlc_enh_free_reseq_q(rlci_dl_ctrl_blk_type *ctrl_blk, uint16 end_sn, 
                          uint16 start_sn);

/*===========================================================================

FUNCTION rlci_enh_dl_am_insert_reseq

DESCRIPTION
  Insert PDU into resequencing queue. Insertion is based on the sequence
  number. If it is duplicate, simply discard the rcvd PDU

DEPENDENCIES
  None.

RETURN VALUE
  boolean : TRUE - If Inserted
            FALSE - If duplicate PDU.

SIDE EFFECTS
  None.

===========================================================================*/
boolean rlci_enh_dl_am_insert_reseq(rlci_dl_ctrl_blk_type *ctrl_blk_ptr,
                                    complete_pdu_type *complete_pdu_ptr,
                                    uint16 rcvd_sn);

/*===========================================================================

FUNCTION rlci_enh_dl_am_process_ctrl_pdus

DESCRIPTION
   Process the Ctrl PDU according to PDU Type after validating the contents
   PDU will be discarded after CTRL PDU processing. 
   Dont discard in intermediate functions.

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
void rlci_enh_dl_am_process_ctrl_pdus(rlci_dl_ctrl_blk_type *ctrl_blk,    
                                      mcalwcdma_dec_iovec_type *payload_ptr);

extern dword b_unpackd(
   byte src[], 
   word pos, 
   word len
);

/*===========================================================================

FUNCTION rlc_enh_dl_am_validate_ack_sufi_pdu

DESCRIPTION
API to validate the recieved ACK SUFI control PDU and indicate whether PDU is 
valid or not along with need to initiate RLC RESET procedure or not.

DEPENDENCIES
  None.

RETURN VALUE
On succeusful validation returns number of bits decoded else returns 0.

SIDE EFFECTS
  None.

===========================================================================*/
uint32 rlc_enh_dl_am_validate_ack_sufi_pdu
(
  rlci_dl_ctrl_blk_type *ctrl_blk_ptr,
  uint32         bit_offset,
  uint32         pdu_end_bit_pos,
  uint8         *status_pdu_ptr,
  boolean       *reset_req,
  boolean       *status_pdu_valid
);

/*===========================================================================

FUNCTION rlc_enh_dl_am_validate_list_sufi_pdu

DESCRIPTION
API to validate the recieved LIST SUFI control PDU and indicate whether PDU is 
valid or not along with need to initiate RLC RESET procedure or not.

DEPENDENCIES
  None.

RETURN VALUE
On succeusful validation returns number of bits decoded else returns 0.

SIDE EFFECTS
  None.

===========================================================================*/
uint32 rlc_enh_dl_am_validate_list_sufi_pdu
(
  rlci_dl_ctrl_blk_type *ctrl_blk_ptr,
  uint32		 bit_offset,
  uint32		 pdu_end_bit_pos,
  uint8		    *status_pdu_ptr,
  boolean           *reset_req,
  boolean           *status_pdu_valid
);

/*===========================================================================

FUNCTION rlc_enh_dl_am_validate_rlist_sufi_pdu

DESCRIPTION
API to validate the recieved Relative LIST SUFI control PDU and indicate whether
PDU is valid or not along with need to initiate RLC RESET procedure or not.

DEPENDENCIES
  None.

RETURN VALUE
On succeusful validation returns number of bits decoded else returns 0.

SIDE EFFECTS
  None.

===========================================================================*/
uint32 rlc_enh_dl_am_validate_rlist_sufi_pdu
(
  rlci_dl_ctrl_blk_type *ctrl_blk_ptr,
  uint32		 bit_offset,
  uint32		 pdu_end_bit_pos,
  uint8 		*status_pdu_ptr,
  boolean		*reset_req,
  boolean               *status_pdu_valid
);

/*===========================================================================

FUNCTION rlc_enh_dl_am_validate_bitmap_sufi_pdu

DESCRIPTION
API to validate the recieved BITMAP SUFI control PDU and indicate whether PDU 
is valid or not along with need to initiate RLC RESET procedure or not.

DEPENDENCIES
  None.

RETURN VALUE
On succeusful validation returns number of bits decoded else returns 0.

SIDE EFFECTS
  None.

===========================================================================*/
uint32 rlc_enh_dl_am_validate_bitmap_sufi_pdu
(
  rlci_dl_ctrl_blk_type *ctrl_blk_ptr,
  uint32		 bit_offset,
  uint32		 pdu_end_bit_pos,
  uint8 		*status_pdu_ptr,
  boolean		*reset_req,
  boolean       *status_pdu_valid
);

/*===========================================================================

FUNCTION rlc_enh_dl_am_validate_rx_status_pdu

DESCRIPTION
API to validate the recieved STATUS control PDU.

DEPENDENCIES
  None.

RETURN VALUE
On succeusful validation returns TRUE else returns FALSE.

SIDE EFFECTS
  None.

===========================================================================*/
boolean rlc_enh_dl_am_validate_rx_status_pdu
(
  rlci_dl_ctrl_blk_type *ctrl_blk_ptr,    
  mcalwcdma_dec_iovec_type *payload_ptr,
  boolean               *reset_req
);
/*===========================================================================

FUNCTION rlci_enh_dl_am_unpack_status_pdu

DESCRIPTION
    Status PDU Handling

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rlci_enh_dl_am_unpack_status_pdu
(
  rlci_dl_ctrl_blk_type    *ctrl_blk,     /* Pointer to Downlink Control Block*/
  mcalwcdma_dec_iovec_type *payload_ptr   /* Downlink PDU                     */
);

/*===========================================================================

FUNCTION rlci_enh_release_dl_am

DESCRIPTION
  To release all the PDUs which are present in this logical channel and 
  return the resequence queue to free pool

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
void rlci_enh_release_dl_am(rlci_dl_ctrl_blk_type *ctrl_blk_ptr,
                            rlc_lc_id_type rlc_id);

/*===========================================================================

FUNCTION rlci_enh_dl_am_process_mrw_sufi

DESCRIPTION
   Process MRW SUFI

DEPENDENCIES
  None.

RETURN VALUE
  bit_offset - Offset(in terms of bits) in the PDU where the next SUFI
               starts.
             - Zero, if any operation fails

SIDE EFFECTS
  None.

===========================================================================*/

uint16 rlci_enh_dl_am_process_mrw_sufi(rlci_dl_ctrl_blk_type *ctrl_blk, 
                                       mcalwcdma_dec_iovec_type *payload_ptr, 
                                       uint16                bit_offset);

/*===========================================================================

FUNCTION rlci_delete_patial_sdu_ptr

DESCRIPTION  
   - Delete the Partial SDU Pointer Contents
   - Delete the Partial PDU contents extracted

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/

void rlci_delete_patial_sdu_ptr(rlci_dl_ctrl_blk_type *ctrl_blk_ptr);

/*===========================================================================

FUNCTION rlci_enh_dl_process_am_pdu

DESCRIPTION
   Process AM PDU.
    - Validate the contents and Process the AM PDU

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rlci_enh_dl_process_am_pdu(rlci_dl_ctrl_blk_type  *ctrl_blk_ptr,
                                complete_pdu_type *complete_pdu_ptr,
                                boolean first_pdu_on_this_lch);

/*===========================================================================

FUNCTION rlci_enh_establish_dl_am

DESCRIPTION
  Allocate one reseq_q entry from the available pool for this RLC Id

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - If allocation is success
  FAIL - No Reseq Q is available for allocation
  
SIDE EFFECTS
  None.

===========================================================================*/

boolean rlci_enh_establish_dl_am(rlci_dl_ctrl_blk_type *ctl_blk_ptr,
                                 rlc_lc_id_type rlc_id);

/*===========================================================================

FUNCTION rlci_enh_dl_am_reassemble_data_pdus

DESCRIPTION
  Reassemble Data PDUs from the start_seq_num till vr_r and deliver the
  complete SDUs

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
void rlci_enh_dl_am_reassemble_data_pdus
(
  rlci_dl_ctrl_blk_type *ctrl_blk,     /* Pointer to Downlink Control Block */
  uint16 start_seq_num
);

/*===========================================================================

FUNCTION rlci_get_li_e_value

DESCRIPTION
   Extracts the LI+E Value from the offset mentioned

DEPENDENCIES
  None.

RETURN VALUE
   Extracts the LI+E Value from the offset mentioned
   
SIDE EFFECTS
  None.

===========================================================================*/

uint16 rlci_get_li_e_value(complete_pdu_type *complete_pdu_ptr, uint16 offset,
                           uint16 li_len);

#if ((defined (FEATURE_HSDPA_MAC_EHS)) && (defined (FEATURE_WCDMA_REL7)))
/*===========================================================================

FUNCTION rlci_enh_dl_am_process_poll_sufi

DESCRIPTION
  Reads the POLL_SUFI and updates the vr_h value to (poll_sn + 1)
  POLL SUFI is valid only when FLEXIBLE PDU Size is configured at RLC level
  in MAC-ehs

DEPENDENCIES
  None.

RETURN VALUE
  bit_offset - Offset(in terms of bits) in the PDU where the next SUFI
               starts.
               
SIDE EFFECTS
  None.

===========================================================================*/
uint16 rlci_enh_dl_am_process_poll_sufi(rlci_dl_ctrl_blk_type *ctrl_blk, 
                                      dsm_item_type         **item_ptr, 
                                      uint16                bit_offset);
#endif /* (defined (FEATURE_HSDPA_MAC_EHS)) && (defined (FEATURE_WCDMA_REL7)) */


/*===========================================================================

FUNCTION rlci_enh_calc_dl_li_length

DESCRIPTION
  Calculate the DL LI Length based on RRC Configuration
   typedef enum {
     LI_7_BITS = 0,                         => 7 bits L1                    
     LI_15_BITS,                            =>  15 bits LI                    
     LI_SAME_AS_BEFORE,                     =>  Use the previous one 
     LI_UNSPECIFIED                         =>  Calculate as in Rel 99        
   } rlc_li_e_type;
   
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
uint8 rlci_enh_calc_dl_li_length(rlc_li_e_type am_li_type);

/*===========================================================================

FUNCTION rlci_enh_am_discard_flag_handling

DESCRIPTION
  Decrement the value of nlen. If nlen is zero, Clear the discard_flag
  Discard the Partial SDU and extracted DSM contents

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rlci_enh_am_discard_flag_handling(rlci_dl_ctrl_blk_type *ctrl_blk);

/*===========================================================================

FUNCTION RLCI_ENH_DL_AM_PROCESS_MRW_ACK_SUFI

DESCRIPTION
  Processes the MRW ACK SUFI and updates the appropriate parameters.
  MRW_ACK SUFI acknwoledges the reception of a MRW SUFI.
  It consists of a Type ID = MRW_ACK, a N field set equal to 0 or SN_MRWlen,
  and a SN_ACK field indicating the updated value of VR(R) after reception of
  MRW SUFI. With the aid of this information in combination with the
  N field, it cna be determined if the MRW_ACK corresponds to a previously
  transmitted MRW SUFI.
  This function get the N and SN_ACK values and calls a function on the
  uplink that checks if the received ACK is for the previous MRW.

DEPENDENCIES
  None.

RETURN VALUE
  bit_offset - Offset(in terms of bits) in the PDU where the next SUFI
               starts.
             - Zero, if any operation fails

SIDE EFFECTS
  None.

===========================================================================*/
uint16 rlci_enh_dl_am_process_mrw_ack_sufi(rlci_dl_ctrl_blk_type *ctrl_blk, 
                                           dsm_item_type         **item_ptr, 
                                           uint16                bit_offset);

/*===========================================================================

FUNCTION RLCI_DL_AM_PROCESS_RLIST_SUFI

DESCRIPTION
  Processes the LIST SUFI and updates the appropriate parameters.
  The RLIST SUFI consists of a type ID RLIST, a list length field, the
  first erroneous sequence number(FSN) and a list of length number of
  codewords.
  If FSN is the first erroneous PDU reported in the status report, stores
  that value since it will be used to update the VT(A) when an ACK PDU
  is received.
  Unpacks each codeword and processes it as follows:
  - If the LSB of the codeword is 1, store the number as the distance
    between the lst erroneous PDU and the next.
  - If the LSB of the codeword is 0, store the codeword and continue in the
    next codeword
  - If the codeword = 0001, the next codewords represent the number of
    subsequent erroneous PUs(not counting the already indicated error
    position. After the number of errors in a burst is terminated with XXX1,
    the next codeword will again by default be the LSB of the distance to
    the next error.
  Gets the number of bytes for all erroneous PDUs that need to be
  retransmitted and then updates the total number of retransmission
  bytes.

DEPENDENCIES
  None.

RETURN VALUE
  bit_offset - Offset(in terms of bits) in the PDU where the next SUFI
               starts.
             - Zero, if any operation fails

SIDE EFFECTS
  None.

===========================================================================*/
uint16 rlci_enh_dl_am_process_rlist_sufi
(
  rlci_dl_ctrl_blk_type *ctrl_blk,        /* Downlink Control BLock        */
  dsm_item_type         **item_ptr,       /* Downlink PDU                  */
  uint16                bit_offset        /* Offset into the PDU           */
);

/*===========================================================================

FUNCTION RLCI_ENH_DL_AM_PROCESS_BITMAP_SUFI

DESCRIPTION
  Processes the BITMAP SUFI and updates the appropriate parameters.
  Bitmap SUFI consists of a Type ID BITMAP, a bitmap length field, a first
  sequence number and a bitmap.
  This function gets the size of the bitmap, the sequence
  number of the first bit in the bitmap and the bitmaps in the SUFI.
  Finds the erroneous PDUs in the bitmap and calculate the retransmission
  bytes of the PDU and update the total retransmission size. The uplink uses
  this information to report buffer status to MAC.
  Call function to build retransmission information.

DEPENDENCIES
  None.

RETURN VALUE
  bit_offset - Offset(in terms of bits) in the PDU where the next SUFI
               starts.
             - Zero, if any operation fails

SIDE EFFECTS
  None.

===========================================================================*/
uint16 rlci_enh_dl_am_process_bitmap_sufi(rlci_dl_ctrl_blk_type *ctrl_blk, 
                                          dsm_item_type         **item_ptr, 
                                          uint16                bit_offset);

/*===========================================================================

FUNCTION RLCI_ENH_DL_AM_PROCESS_LIST_SUFI

DESCRIPTION
  Processes the LIST SUFI and updates the appropriate parameters.
  The LIST SUFI contains a Type ID LIST, a list length field and a list of
  length number of pairs of SNi not received correctly and number of
  consecutive PDUs not correctly received following PDU with Sequence number
  SNi.If the erroneous PDU is the first erroneous PDU in the status PDU, the
  sequence number is stored. The stored SN is used to update VT(A) on the
  uplink, if the value is less than the LSN in the ACK or MRW_ACK SUFI.
  Calls function to build retrnamission information.

DEPENDENCIES
  None.

RETURN VALUE
  bit_offset - Offset(in terms of bits) in the PDU where the next SUFI
               starts.

SIDE EFFECTS
  None.

===========================================================================*/
uint16 rlci_enh_dl_am_process_list_sufi(rlci_dl_ctrl_blk_type *ctrl_blk, 
                                        dsm_item_type         **item_ptr, 
                                        uint16                bit_offset);

/*===========================================================================

FUNCTION RLCI_ENH_DL_AM_PROCESS_WINDOW_SUFI

DESCRIPTION
  Process the WIN(Window) SUFI and update the appropriate parameters.
  The Window Size SUFI consists of a Type ID WINDOW and a window size
  number. The reciever is allowed to change the Tx window size of the peer
  but the minimum and maximum allowed value is given by the RRC
  configuration.
  If WSN = 0, the SUFI is discarded. If the value of
  WSN > Configured_Tx_Window_Size, VT(WS) shall be set equal to the
  Configured_Tx_Window_Size. Otherwise,the value of VT(WS) on the uplink is
  updated with the WSN.

DEPENDENCIES

RETURN VALUE
  bit_offset - Offset(in terms of bits) in the PDU where the next SUFI
               starts.
             - Zero, if any operation fails

SIDE EFFECTS
  None.

===========================================================================*/
uint16 rlci_enh_dl_am_process_window_sufi(rlci_dl_ctrl_blk_type *ctrl_blk, 
                                          dsm_item_type         **item_ptr, 
                                          uint16                bit_offset);

/*===========================================================================

FUNCTION RLCI_ENH_DL_AM_PROCESS_ACK_SUFI

DESCRIPTION
  Process the ACK(acknowledgement) SUFI and update the appropriate
  parameters. The ACK SUFI consists of a Type ID ACK and the last sequence
  number.
  If LSN <= the value of the first error indicated in the status PDU, VT(A)
  will be updated according to the LSN, otherwise VT(A) will be updated
  according to the first error indicated in the STATUS PDU.

DEPENDENCIES

RETURN VALUE
  bit_offset - Offset(in terms of bits) in the PDU where the next SUFI
               starts.
             - Zero, if any operation fails

SIDE EFFECTS
  None.

===========================================================================*/
void rlci_enh_dl_am_process_ack_sufi(rlci_dl_ctrl_blk_type *ctrl_blk, 
                                     dsm_item_type         **item_ptr,
                                     uint16                bit_offset);

/*===========================================================================

FUNCTION rlci_enh_am_update_partial_sdu_ptr

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rlci_enh_am_update_partial_sdu_ptr(rlc_dl_am_reseq_q_type *reseq_q_ptr,
                                        rlci_dl_temp_pdu_info_type *dl_pdu_info_ptr,
                                        dsm_item_type *last_pdu_item_ptr,
                                        uint16 last_pdu_item_size);


/*===========================================================================

FUNCTION RLCI_DL_UPDATE_CIPHER_STATES

DESCRIPTION
  Update Ciphering states when RLC perform RESET or RE-ESTABLISHMENT.

DEPENDENCIES
  None.

RETURN VALUE
  Count_c for ciphering.

SIDE EFFECTS
  None.

===========================================================================*/
void rlci_dl_update_cipher_states
(
  rlci_dl_ctrl_blk_type *ctl_blk_ptr

);

/*===========================================================================

FUNCTION RLCI_RESET_DL_AM_CTL_BLK

DESCRIPTION
  Reset the DL AM RLC control block.

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rlci_reset_dl_am_ctl_blk
(
  rlci_dl_ctrl_blk_type *ctl_blk_ptr,
  boolean IsRelease
);

/*===========================================================================

FUNCTION RLCI_POST_UL_RESET_MSG

DESCRIPTION
  Post RESET messages to the UL AM RLC. The RESET messages could be
  RLCI_UL_RESET_REQ or RLCI_DL_RESET_DONE.

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rlci_post_ul_reset_msg
(
  rlc_lc_id_type ul_data_id,                /* RLC data logic channel ID  */
  wcdma_l2_ul_cmd_enum_type msg_id          /* DL Message ID  */
);

/*===========================================================================

FUNCTION RLCI_DL_AM_FIND_FIRST_ERR_BIT

DESCRIPTION
  Look for first bit with a value of 0 and return the bit position.
  First SN = Sequence Number of first bit in the bitmap.
  0 - Indicates that the PDU with (SN = (First SN + bit_position) was
      not correctly received.
  1 - Indicates that the PDU with (SN = (First SN + bit_position) was
      not correctly received.

DEPENDENCIES
  None.

RETURN VALUE
  Bit Position of the first error (0) in the bitmap.

SIDE EFFECTS
  None.

===========================================================================*/
uint8 rlci_dl_am_find_first_err_bit
(
  uint8 bitmap                          /* Bitmap                          */
);

/*===========================================================================

FUNCTION RLCI_DL_AM_UPDATE_BIT_OFFSET

DESCRIPTION
  Pullup bytes from the dsm item that have already been processed.
  If the bit_offset after that is greater than 0, leave that
  byte in the dsm item.

DEPENDENCIES
  None

RETURN VALUE
  bit_offset - Offset(in terms of bits) in the PDU where the next SUFI
               starts.

SIDE EFFECTS
  None.

===========================================================================*/
uint16 rlci_dl_am_update_bit_offset
(
  dsm_item_type **item_ptr,          /* Control PDU                         */
  uint16        bit_offset           /* Current bit_offset                  */
);

/*===========================================================================

FUNCTION RLCI_DL_UPDATE_NAK_LIST_Q

DESCRIPTION
  Remove the passed-in SN from the DL NAK list Queue.
  The NAK items is in the ascend order in DL NAK list Queue. And no duplication.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rlci_dl_update_nak_list_q
(
  rlci_dl_ctrl_blk_type *ctl_blk_ptr,    /* Pointer to Downlink Control Block */
  uint16  sn
);

/*==========================================================================

FUNCTION RLCI_DL_MRW_UPDATE_NAK_QUEUE

DESCRIPTION
        This function removes all the pdus with sequence numbers less than
        passed in value of sequence number (seq) from the DL NAK Queue.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.


==========================================================================*/

void rlci_dl_mrw_update_nak_queue
(
  rlci_dl_ctrl_blk_type *ctl_blk_ptr,
  uint16 seq
);

/*===========================================================================

FUNCTION RLCI_DL_UPDATE_RX_STATE

DESCRIPTION
  Updates the RX state(VR_R , VR_H, VR_MR).

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rlci_dl_update_rx_state
(
  rlci_dl_ctrl_blk_type *ctrl_blk,    /* Pointer to Downlink Control Block */
  uint16                seq_num       /* Sequence number to update the DL  */
                                      /* state variables.                  */
);

/*===========================================================================

FUNCTION RLC_ENQ_LIST_SUFI

DESCRIPTION
  Gets a NAK buffer from the UL Free NAK Q, stores the information about the
  NAK LIST in the buffer and enqueues the buffer on to the retransmission
  queue.Calculates the number of bytes of an erroneous PDU and updates the
  total number of retransmission bytes. This information is used by the
  uplink to provide buffer status to MAC.


DEPENDENCIES
  None.

RETURN VALUE
  reurns TRUE if Successful

SIDE EFFECTS
  None.

===========================================================================*/
boolean rlc_enq_list_sufi
(
  rlc_lc_id_type  lc_id,
  uint16          list_sn,
  int16           list_l
);

/*===========================================================================

FUNCTION RLCI_DL_AM_ENQ_NAK_LIST

DESCRIPTION
  Gets a NAK buffer from the UL Free NAK Q, stores the information about the
  NAK LIST in the buffer and enqueues the buffer on to the retransmission
  queue.Calculates the number of bytes of an erroneous PDU and updates the
  total number of retransmission bytes. This information is used by the
  uplink to provide buffer status to MAC.


DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
boolean rlci_dl_am_enq_nak_list
(
  rlci_dl_ctrl_blk_type *ctrl_blk, /* Pointer to the Downlink Control Block*/
  uint16                sn,        /* First erroneous PDU Sequence Number  */
  uint16                missing_len/* Number of consecutive erroneous PDUs */
);

/*===========================================================================

FUNCTION RLCI_DL_AM_UPDATE_NAK_BITMAP

DESCRIPTION
  Gets a NAK buffer from the Free Q, stores the information about the
  NAK BITMAP in the buffer and enqueues the buffer on the retransmission
  queue. Calculates the number of bytes of an erroneous PDU and updates the
  total number of retransmission bytes. This information is used by the
  uplink to give the buffer status to MAC.


DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
boolean rlci_dl_am_update_nak_bitmap
(
  rlci_dl_ctrl_blk_type *ctrl_blk,/* Pointer to the Downlink Control Bloack*/
  uint16                sn,       /* First erroneous sequence number       */
  uint8                 bitmap    /* Bitmap containing erroneous Sequence  */
                                  /* numbers as well as those received     */
                                  /* correctly                             */
);

#ifdef FEATURE_WCDMA_UL_COMPR
/*===========================================================================

FUNCTION RLC_DL_PTP_FUNC_RESPONSE_PER_RB

DESCRIPTION
  None.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rlc_dl_ptp_func_response_per_rb(void);
#endif
	
#endif
