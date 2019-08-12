/*****************************************************************************
***
*** TITLE
***
***  GPRS MAC TO RLC INTERFACE
***
***
*** DESCRIPTION
***
***  Definition of data structures and functions provided by MAC for RLC
***  to send messages to MAC.
***
***
*** Copyright (c) 2001-2013 Qualcomm Technologies, Inc.
*** All Rights Reserved. QUALCOMM Proprietary.
***
*** Export of this technology or software is regulated by the U.S.
*** Government. Diversion contrary to U.S. law prohibited.
***
*****************************************************************************/


/*****************************************************************************
***
***                        EDIT HISTORY FOR MODULE
***
***  This section contains comments describing changes made to the module.
***  Notice that changes are listed in reverse chronological order.
***
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gmac/inc/gmacrlc.h#1 $
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 10-04-01   tjw     Add customer.h and comdef.h to all SU API files
*** 03/29/01    hv
***
*****************************************************************************/

#ifndef INC_GMACRLC_H
#define INC_GMACRLC_H

/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/

#include "geran_variation.h"
#ifndef CUSTOMER_H
  #include "customer.h"
#endif
#ifndef COMDEF_H
  #include "comdef.h"
#endif


#include "gprsdef.h"
#include "gprsdef_g.h"
#include "grlcmaccmn.h"
#include "grlci.h"
#include "gllcrlcif.h"
#include "grlcutil.h"
#include "gprs_mem.h"

#include "csn.h"


/*****************************************************************************
***
***     Public Type Definitions
***
*****************************************************************************/

/*-----------------------------------------------------------------
** Definitions of all signals sent from RLC to MAC for both uplink
** and downlink operations. The RM_ prefix denotes the direction
** of the message.
**-----------------------------------------------------------------
*/
typedef enum
{

  /*----------- UPLINK ---------*/

  /* request MAC to send TBF establishment request */
  RM_UL_EST_TBF_REQ = 0x00,

  /* request MAC to send reallocation request */
  RM_UL_REALLOC_REQ = 0x01,

  /* inform MAC of uplink release */
  RM_UL_RELEASE_IND = 0x02,

  /* inform MAC of couter N3104 expiry */
  RM_UL_N3104_EXPIRY_IND = 0x03,

  /* confirm to MAC that RLC UL has completed reset */
  RM_UL_RESET_CNF = 0x04,

  /*----------- DOWNLINK --------*/

  /* request MAC to send downlink ack/nack message */
  RM_DL_PKT_ACKNACK_REQ = 0x40,

  /* request MAC to send DL packet control acknowledgement msg (in un-ack mode)
  */
  RM_DL_PKT_CTRL_ACK_REQ = 0x41,

  /* RLC indicates to MAC reception of the 1st data block of a DL assignment */
  RM_DL_FIRST_BLOCK_IND = 0x42,

  /* RLC indicates to MAC of downlink release */
  RM_DL_RELEASE_IND = 0x43,

  /* confirm to MAC that RLC DL has completed reset */
  RM_DL_RESET_CNF = 0x44,

  RM_PS_ACCESS_CNF = 0x60,

} rm_sig_t;


/*-------------------------------------------------------------------------
** Structure for RLC to request MAC to send a TBF establishment request to
** the peer entity.
**-------------------------------------------------------------------------
*/
typedef struct
{
  /* signal id: RM_UL_EST_TBF_REQ
  */
  rm_sig_t              sig;


  test_mode_T           test_mode;

  /* Test Mode B DL timeslot offset */
  uint8                 ts_offset;

  /* loop back blocks on the uplink using either GMSK or 8-PSK modulation */
  boolean               srb_mode;

  /* TLLI attached to LLC PDU causing this request
  */
  uint32                tlli;

#ifdef  FEATURE_GSM_DTM
  /* Always contains the length of the single current LLC PDU in octets regardless of the
  ** whether open/close-ended request is being made. This differs to rlc_octets_cnt in
  ** the channel_req_desc_t below. This is required so that MAC/GMM can check the PDU
  ** size criteria for a DTM request
  */
  uint16                pdu_octets;
#endif /* FEATURE_GSM_DTM */

  /* channel request description
  */
  channel_req_desc_t       chan_req_desc;

  /* alloction type or (medium) access mode: fixed, dynamic
  ** and extended dynamic. Determined by the n/w
  */

  /* access type (one phase, two phase or short access)
  ** RLC states what it needs depending on the rlc mode and
  ** the number of data blocks (according to coding scheme)
  ** as follows:
  **
  ** 1. un-ack data: two phase
  ** 2. ack data <= 8 blocks: short access
  ** 3. ack data >  8 blocks: one OR two phase
  */
  uint8                 access_phase;

  /* If access_phase_type is short access, RLC specifies a value
  ** from 1 to 8 blocks. MAC should encode it by subtracting
  ** 1 one this value before passing it onto the network.
  */
  uint8                 num_of_blocks;

  /* tbf type. If the MAC mode returned by the n/w is Fixed Alloc
  ** then RLC uses tbf_type to specify its preferred value.
  ** If tbf_type is open-ended then MAC will make RLC_OCTET_COUNT
  ** field in PRR 0. If tbf_type is close-ended then MAC will keep
  ** non-zero value specified in RLC_OCTET_COUNT.
  **
  ** If the n/w returns Dynamic Allocation mode, then MAC should
  ** ignore tbf_type.
  */
  uint8                 tbf_type;

  /* llc data request cause. This value is supplied by LLC only
  ** in ack mode. This contains the information about the llc pdu
  ** as to the data type is Layer 3 data, GMM or LLC signalling,
  ** GMM page response or GMM cell update.
  **
  ** MAC will map this info to its access type to request
  ** resources from the nw.
  */
  grr_data_req_cause_t  data_req_cause;


  /*PFC Feature*/
  boolean               pfi_present;
  uint8                 pfi;

  gas_id_t              gas_id;

} rm_ul_est_tbf_req_t;

/*----------------------------------------------------------------------
** Structure for RLC to request MAC to send an uplink TBF re-allocation
** request to the peer entity.
**----------------------------------------------------------------------
*/
typedef struct
{
  /* signal id: RM_UL_REALLOC_REQ
  */
  rm_sig_t             sig;

  /* channel request description
  */
  channel_req_desc_t   chan_req_desc;

  /* indicates if this is a reallocation after countdown has
  ** completed for a previous allocation
  */
  boolean              b_cv_zero;

  /*PFC Feature*/
  boolean              pfi_present;
  uint8                pfi;

  gas_id_t             gas_id;

} rm_ul_realloc_req_t;

/*----------------------------------------------------------------------
** Structure for RLC to inform MAC of a normal or abnormal tbf release.
**----------------------------------------------------------------------
*/
typedef struct
{
  /* signal id: RM_UL_RELEASE_IND
  */
  rm_sig_t          sig;

  /* cause of ul release
  */
  rm_tbf_release_t  cause;

  gas_id_t          gas_id;

} rm_ul_release_t;

/*------------------------------------------------------
** Structure for RLC to inform MAC counter N3104 expiry
**------------------------------------------------------
*/
typedef struct
{
  /* signal id: RM_UL_N3104_EXPIRY_IND
  */
  rm_sig_t          sig;

  gas_id_t          gas_id;

} rm_ul_n3104_expiry_ind_t;

/*----------------------------------------------------------
** Structure for RLC to confirm to MAC of UL reset
**----------------------------------------------------------
*/
typedef struct
{
  /* signal id: RM_UL_RESET_CNF
  */
  rm_sig_t      sig;

  /* no cause
  */
  gas_id_t      gas_id;

} rm_ul_reset_t;

/*--------------------------------------------------------------------------
** Structure for RLC to request MAC to send Packet Downlink Acknack message
** to the peer entity.
**--------------------------------------------------------------------------
*/
typedef struct
{
  /* signal id: RM_DL_PKT_ACKNACK_REQ
  */
  rm_sig_t            sig;

  /* packet acknack description
  */
  rm_acknack_desc_t   acknack_desc;

  /* tfi. MAC matches this value as a sanity check
  */
  uint8               tfi;

  /* specific RRBP and frame number for MAC to send the acknack on
  */
  uint8               rrbp;

  /* timeslot
  */
  uint8               time_slot;

  /* frame number from L1
  */
  uint32              frame_number;

  /* EGPRS packet acknack description
  */
  egprs_ack_nack_des_t  e_acknack_desc;


#ifdef PERLUTF
  #error code not present
#endif

  /* Pointer to quality report, if present. NULL if not present
  */
  egprs_chn_qual_rpt_t   *pl1_qrep_ptr;

  /* Extra supplementary polling bits
  */
  uint8               esp;

  /* Indicates if a UL channel request is to be include in the EGPRS PDAN
  */
  boolean include_chan_req;

  /* Indicate to NW if MS has memory available for EDGE IR.
  */
  boolean ms_out_of_mem;

  gas_id_t gas_id;

} rm_dl_pkt_acknack_req_t;

/*------------------------------------------------------------------
** Structure for RLC to request MAC to send downlink Packet Control
** Acknowledgement message in un-ack mode.
**------------------------------------------------------------------
*/
typedef struct
{
  /* signal id: RM_DL_PKT_CTRL_ACK_REQ
  */
  rm_sig_t            sig;

  /* tfi. MAC matches this value as a sanity check
  */
  uint8               tfi;

  /* specific RRBP and frame number for MAC to send the acknack on
  */
  uint8               rrbp;

  /* timeslot
  */
  uint8               time_slot;

  /* frame number from L1
  */
  uint32              frame_number;

  gas_id_t            gas_id;

} rm_dl_pkt_ctrl_ack_req_t;


/*----------------------------------------------------------------------
** Structure for RLC to inform MAC of reception of the first data block
** after a downlink assignment. MAC can enter DL packet transfer mode
** after receiving this signal
**----------------------------------------------------------------------
*/
typedef struct
{
  /* signal id: RM_DL_FIRST_BLOCK_IND
  */
  rm_sig_t      sig;

  gas_id_t      gas_id;

} rm_dl_first_block_ind_t;

/*----------------------------------------------------------
** Structure for RLC to inform MAC of TBF downlink release
**----------------------------------------------------------
*/
typedef struct
{
  /* signal id: RM_DL_RELEASE_IND
  */
  rm_sig_t          sig;

  /* cause for dl release
  */
  rm_tbf_release_t  cause;

  gas_id_t          gas_id;

} rm_dl_release_t;

/*----------------------------------------------------------
** Structure for RLC to confirm to MAC of DL reset
**----------------------------------------------------------
*/
typedef struct
{
  /* signal id: RM_DL_RESET_CNF
  */
  rm_sig_t      sig;

  /* no cause
  */
  gas_id_t      gas_id;

} rm_dl_reset_t;

/*-----------------------------------------------------------------
** Structure for RLC to confirm to MAC that PS access is available.
**-----------------------------------------------------------------
*/
typedef struct
{
  /* signal id: RM_PS_ACCESS_CNF
  */
  rm_sig_t      sig;

  /* no cause
  */
  gas_id_t      gas_id;

} rm_ps_access_cnf_t;

/*-------------------------------
** RLC to MAC message definition
**-------------------------------
*/
typedef struct
{
  q_link_type     link;

  /* say message is from RLC
  */
  uint8           task_id;

  /*---------------------------------------
  ** Union of all messages from RLC to MAC
  **---------------------------------------
  */
  union
  {
    rm_sig_t                        sig;

    /* uplink operation
    */
    rm_ul_est_tbf_req_t             ul_est_tbf_req;
    rm_ul_realloc_req_t             ul_realloc_req;
    rm_ul_release_t                 ul_release_ind;
    rm_ul_n3104_expiry_ind_t        ul_n3104_expiry_ind;
    rm_ul_reset_t                   ul_reset_cnf;

    /* downlink operation
    */
    rm_dl_pkt_acknack_req_t         dl_pkt_acknack_req;
    rm_dl_pkt_ctrl_ack_req_t        dl_pkt_ctrl_ack_req;
    rm_dl_first_block_ind_t         dl_first_block_ind;
    rm_dl_release_t                 dl_release_ind;
    rm_dl_reset_t                   dl_reset_cnf;

    rm_ps_access_cnf_t              ps_access_cnf;

  } msg;

} rlc_mac_msg_t;


/*****************************************************************************
***
***     Public Defines & Constants
***
*****************************************************************************/



/*****************************************************************************
***
***     Public Data References
***
*****************************************************************************/



/*****************************************************************************
***
***     Public Macros
***
*****************************************************************************/



/*****************************************************************************
***
***     Public Functions
***
*****************************************************************************/


/*===========================================================================
===
===  FUNCTION      MAC_GET_CMD_BUF_RLC()
===
===  DESCRIPTION
===
===    Returns a pointer to a buffer for RLC to build a message destined
===    for MAC
===
===
===  DEPENDENCIES
===
===    none
===
===  RETURN VALUE
===
===    pointer to an RLC-to-MAC message buffer
===
===  SIDE EFFECTS
===
===    none
===
===========================================================================*/
rlc_mac_msg_t *mac_get_cmd_buf_rlc (void);


/*===========================================================================
===
===  FUNCTION      MAC_PUT_CMD_RLC()
===
===  DESCRIPTION
===
===    Send the given RLC message to MAC
===
===
===  DEPENDENCIES
===
===    none
===
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===    none
===
===========================================================================*/
void mac_put_cmd_rlc (gas_id_t gas_id, rlc_mac_msg_t *msg_ptr);

/*===========================================================================
===
===  FUNCTION      gmac_access_func_get_num_dl_timeslots()
===
===  DESCRIPTION
===
===    MAC access function for RLC.
===    Returns the number of DL timeslots currently assigned in MAC.
===    Note that this access function contains a loop.
===
===
===  DEPENDENCIES
===
===    none
===
===  RETURN VALUE
===
===    uint8 - Number of DL timeslots (range 0..8)
===
===  SIDE EFFECTS
===
===    none
===
===========================================================================*/
uint8 gmac_access_func_get_num_dl_timeslots (gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION      gmac_store_trace_send_msg_buf()
===
===  DESCRIPTION
===
===    This function stores the send signal message by GMAC in gmac_trace_msg_buf.
===    This function will also increment gmac_trace_msg_buf_index
===
===  DEPENDENCIES
===
===  gmac_trace_msg_buf, gmac_trace_msg_buf_index
===
===  RETURN VALUE
===
===    None.
===
===  SIDE EFFECTS
===
===    None.
===========================================================================*/
extern void gmac_store_trace_send_msg_buf(gas_id_t gas_id,
                                          uint8 message_set, void* message_ptr);


#endif /* INC_GMACRLC_H */

/*** EOF: don't remove! ***/
