#ifndef TDSMACRLCIF_H
#define TDSMACRLCIF_H

/*==================================================================

   MAC - RLC INTERFACE H E A D E R    F I L E

DESCRIPTION
  This file contains the interface functions and data types defined
  in MAC and used by RLC
  
  Copyright (c) 2002-2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

====================================================================*/

/*===================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mac/vcs/macrlcif.h_v   1.10   09 May 2001 18:16:08   psuvarna  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/tdscdma/mac/inc/tdsmacrlcif.h#1 $ $DateTime: 2016/12/13 07:58:19 $ $Author: mplcsds1 $


when       who     what, where, why
--------   ---     --------------------------------------------------
02/02/11  ps       Changes for TDS MAC HSUPA
01/26/11  ps       Rename some of the mutex used with tds prefix
11/23/10  cyx      Remove tdsmvsif.h to clean up l2 interface
10/24/10  guo      Initial release Upmerged WCDMA to TDS

====================================================================*/


#include "tdscdma_variation.h"
#include    "dsm.h"
#include    "tdsuecomdef.h"
#include    "tdsl1macif.h"
#include    "tdsl1macdata.h"

#include "modem_mem.h"

#include "stringl.h"

/* --------------------------------- */
/* PUBLIC APIs of tdsmacrlcif.h         */
/* --------------------------------- */
typedef struct{
   uint8  nchan;

   rlc_lc_id_type rlc_id_list[TDSUE_MAX_DL_LOGICAL_CHANNEL];

} tdsmac_hs_dl_reset_req_type;

/*+yongh add optimization feature NV for L2+*/
#define TDSL2_OPT_FEATURE_SRB_RESET 0x1
#define TDSL2_OPT_FEATURE_E4B_TWICE 0x2
#define TDSL2_OPT_FEATURE_E4B_FOURTH 0x4
#ifdef FEATURE_TDSCDMA_E4A_OPT
#define TDSL2_OPT_FEATURE_E4A_IGNORE 0x8
#endif
#ifdef FEATURE_TDSCDMA_E4B_OPT
#define TDSL2_OPT_FEATURE_E4B_IGNORE 0x10
#endif

#ifdef FEATURE_TDSCDMA_SRB_RESET
#define TDSL2_IGNORE_NUM_MASK                0x0000000F
#define TDS_DEFAULT_IGNORE_RESET_NUM  5 
#define TDS_MAX_IGNORE_RESET_INVALID_NUM  0xFF 
#define TDSL2_BAD_STATUS_PDU_IGNORE_NUM(x)   ((x) >> 28)  //use MSB four bits for ignoring number
#endif
/*-yongh add optimization feature NV for L2-*/

#ifdef FEATURE_TDSCDMA_TC_CONSTANT_DELAY
#define TDSMAC_LB_MAX_TTI_DELAY 10
#define TDSMAC_CFN_INCREMENT 10
#endif
/* ----------------------------------------------------------------------- */
/* Private Section.                                                        */
/* ----------------------------------------------------------------------- */


#ifdef FEATURE_Q6_MT
extern rex_crit_sect_type tdse_ul_isr_l2_mutex;

extern rex_crit_sect_type tdse_ul_isr_l2_datapath_mutex;
extern rex_crit_sect_type tdsr99_ul_isr_l2_datapath_mutex;
#ifdef FEATURE_TDSCDMA_FREE_FLOATING_TASK
extern rex_crit_sect_type tdsrlc_reset_table_sem;
extern rex_crit_sect_type tdsmac_cipher_param_mutex;
#endif

/* Take the UL and DL Mutex */
#define TDSL2_ACQ_UL_DL_LOCK()           REX_ISR_LOCK(&tdse_ul_isr_l2_mutex); \
                                    REX_ISR_LOCK(&tdsl1dec_tsn_q_crit_sect)

/* Free the acquired UL and DL Mutex */
#define TDSL2_ACQ_UL_DL_UNLOCK()         REX_ISR_UNLOCK(&tdse_ul_isr_l2_mutex); \
                                    REX_ISR_UNLOCK(&tdsl1dec_tsn_q_crit_sect)
/* Take the DL  Mutex */
#define TDSL2_ACQ_DL_LOCK()            REX_ISR_LOCK(&tdsl1dec_tsn_q_crit_sect)
/* Free the acquired DL Mutex */
#define TDSL2_ACQ_DL_UNLOCK()          REX_ISR_UNLOCK(&tdsl1dec_tsn_q_crit_sect)

/* Take the UL  Mutex */
#define TDSL2_ACQ_UL_LOCK()            REX_ISR_LOCK(&tdse_ul_isr_l2_mutex)
/* Free the acquired UL Mutex */
#define TDSL2_ACQ_UL_UNLOCK()          REX_ISR_UNLOCK(&tdse_ul_isr_l2_mutex)

#ifdef FEATURE_TDSCDMA_FREE_FLOATING_TASK
  #define TDSRLC_ACQ_RESET_TABLE_LOCK()          REX_ISR_LOCK(&tdsrlc_reset_table_sem)
  #define TDSRLC_ACQ_RESET_TABLE_UNLOCK()        REX_ISR_UNLOCK(&tdsrlc_reset_table_sem)

  /* Take Mutex that protects ciphering related info */
  #define TDSL2_ACQ_CIPHER_PARAM_LOCK()    REX_ISR_LOCK(&tdsmac_cipher_param_mutex)
  /* Free  Mutex that protects ciphering related info */
  #define TDSL2_ACQ_CIPHER_PARAM_UNLOCK()  REX_ISR_UNLOCK(&tdsmac_cipher_param_mutex)

#else
  #define TDSRLC_ACQ_RESET_TABLE_LOCK()
  #define TDSRLC_ACQ_RESET_TABLE_UNLOCK()
  #define TDSL2_ACQ_CIPHER_PARAM_LOCK()    {}
  #define TDSL2_ACQ_CIPHER_PARAM_UNLOCK()  {}
#endif
#else

#define TDSL2_ACQ_UL_DL_LOCK()  {}
#define TDSL2_ACQ_UL_DL_UNLOCK()   {}

#define TDSL2_ACQ_DL_LOCK()   {}         
#define TDSL2_ACQ_DL_UNLOCK()  {}        

#define TDSL2_ACQ_UL_LOCK()           {}
#define TDSL2_ACQ_UL_UNLOCK()       {}

#define TDSL2_ACQ_UL_LW_LOCK()      {}
#define TDSL2_ACQ_UL_LW_UNLOCK()    {}
#define TDSRLC_ACQ_RESET_TABLE_LOCK()
#define TDSRLC_ACQ_RESET_TABLE_UNLOCK()

#define TDSL2_ACQ_CIPHER_PARAM_LOCK()   {}
#define TDSL2_ACQ_CIPHER_PARAM_UNLOCK()  {}
#endif

#ifdef FEATURE_Q6_MT

#define TDSL2_MUTEX_LOCK(x)    REX_ISR_LOCK(x)
#define TDSL2_MUTEX_UNLOCK(x)  REX_ISR_UNLOCK(x)

#else /* FEATURE_Q6_MT */

#define TDSL2_MUTEX_LOCK(x)    rex_enter_crit_sect(x)
#define TDSL2_MUTEX_UNLOCK(x)  rex_leave_crit_sect(x)

#endif /* FEATURE_Q6_MT */
/*===================================================================

                       ENUMERATED TYPE DATA DEFINITIONS

====================================================================*/
#define TDSMAX_MAC_E_HDR_SIZE_BYTES  16

#define TDSUM_PDU_HDR_SIZE_BITS 8

#define TDSAM_PDU_HDR_SIZE_BITS 16


/*------------------------------------------------------------------*/
/* RLC Buffer occupancy information specification                   */
/*------------------------------------------------------------------*/

typedef enum {

    /* Buffer Occupancy specified as "PDU size & Number of PDUs" */
    TDSFIXED_PDUS = 1,

    /* Buffer Occupancy specified as "Number of bytes" */
    TDSFLEX_PDUS

}tdsrlc_ul_buf_status_info_e_type;


/*------------------------------------------------------------------*/
/* Enum for UL channel type R99(DCH) or EUL(EDCH)                   */
/*------------------------------------------------------------------*/

typedef enum {

    TDSCHANNEL_DCH = 0,

    TDSCHANNEL_EDCH,
    
    TDSCHANNEL_MAX

}tdsrlc_ul_channel_type;

/*---------------------------------------------------------------------------*/
/* Enum for UL channel type which is same as  log_tdscdma_trch_type */
/*---------------------------------------------------------------------------*/
typedef enum
{
  TDSCDMA_MAC_TRCH_INVALID,
  TDSCDMA_MAC_TRCH_BCH, 
  TDSCDMA_MAC_TRCH_PCH,
  TDSCDMA_MAC_TRCH_FACH,
  TDSCDMA_MAC_TRCH_DCH, 
  TDSCDMA_MAC_TRCH_HSDSCH, 
  TDSCDMA_MAC_TRCH_RACH, 
  TDSCDMA_MAC_TRCH_EDCH
}tdscdma_trch_type;

/*===================================================================

                       STRUCTURE TYPE DATA DEFINITIONS

====================================================================*/

/*------------------------------------------------------------------*/
/* Buffer occupancy information needs to be reported only for the
channels that MAC is requesting for  */
/*------------------------------------------------------------------*/
typedef struct
{
  /* Number of channels for which MAC need the buffer status */
  uint8                     nchan;

  /* LC ids for which MAC needs the buffer status */
  rlc_lc_id_type            rlc_id[TDSUE_MAX_UL_LOGICAL_CHANNEL];

}tdsrlc_ul_lc_info_type;


/*------------------------------------------------------------------*/
/* Loopback delay information of a single UL RLC logical channel  */
/*------------------------------------------------------------------*/

/*
possible TTI info values are :
{
  TDSL1_TTI_10MS,-->1
  TDSL1_TTI_20MS,-->2
  TDSL1_TTI_40MS,-->4
  TDSL1_TTI_80MS,-->8
  TDSL1_NUM_TTI  -->
} 
*/

#ifdef FEATURE_TDSCDMA_TC_CONSTANT_DELAY
typedef struct
{
  boolean lb_configured_flag;
  uint16  loop_back_constant_delay;
  uint32  ul_cfn;
  uint16  lb_trch_tti_info;
}tdsrlc_ul_lb_lc_info_type;
#endif /* FEATURE_TDSCDMA_TC_CONSTANT_DELAY */


/*------------------------------------------------------------------*/
/* Buffer occupancy information of a single UL RLC logical channel  */
/*------------------------------------------------------------------*/

typedef struct
{
    /* Buffer occupancy  (AM/UM-FLEX_PDUS, TM-FIXED_PDU)            */
    tdsrlc_ul_buf_status_info_e_type   type;

    /* No. of PDUs. Only applicable to TM(FIXED_PDUs)               */
    uint16  npdus;

    /* PDU Size in bits - Valid for TM & AM                         */
    uint16  pdu_size;

    /* This parameter is kept for backward compatibility. Used only */
    /* by MAC during TFCI selection algorithm. RLC will not fill    */
    /* this parameter.                                              */
    uint32  nbytes;

    /* No. of bytes in UL- WM + No. of LIs. Doesn't include RLC-hdr.*/
    /* This appilcable for both AM & UM.                            */
    /* For UM channel, if alt_e_bit = TRUE, doesn't include LI's    */
    uint32   new_data_bytes;

    /* The following parameters are applicable to only AM           */
    /* No. of bytes of retx PDUs. It includes LI's and payload.     */
    /* Does not include RLC-hdr.                                    */
    uint32   retx_data_bytes;

    /* Total number of bytes of control/status SUFI.                */
    /* It is not rounded to the pdu sizes (i.e. if the control PDU  */
    /* has 4 bytes of data, then ctrl_data_bytes = 4 rather than    */
    /* (pdu_size - 2))                                              */

    uint32   ctrl_data_bytes;

    /* Available RLC PDU window size for the LC                     */
    uint16   effect_win_size;

    /* Flag to block the PDU building as the RLC is in the internal */
    /* states transition. This is applicable to both AM & UM        */
    boolean suspend_pdu_build_f;


    /* TRUE - if alternate E-bit interpretation is configured and 
              no. of SDUs in UL-WM is one. 
       FALSE - Otherwise */
    boolean alt_e_bit_opt;              

}tdsrlc_ul_lc_buf_status_type;

/*------------------------------------------------------------------*/
/* Buffer occupancy information of all uplink RLC logical channels  */
/*------------------------------------------------------------------*/

typedef struct

{
    /* Buffer occupancy specification (FLEX_PDUS, FIXED_PDU) */
    tdsrlc_ul_lc_buf_status_type chan_info[TDSUE_MAX_UL_LOGICAL_CHANNEL];

}tdsrlc_ul_buf_status_type;


/*------------------------------------------------------------------*/
/* Format of a single uplink RLC logical channel                    */
/*------------------------------------------------------------------*/

typedef struct {

    /* Contains TSN for this LC.This has to be written to HW as is.
    This value if given to RLC from MAC*/
    uint32          mac_es_hdr;
    uint8           mac_es_hdr_size;  /* Indicates the TSN size in bits.    */

    /* RLC logical channel ID */
    rlc_lc_id_type  rlc_id;

    /* Number of PDUs */
    uint16          npdus;

    /* PDU Size in bits */
    uint16          pdu_size;

#ifdef FEATURE_TDSCDMA_TC_CONSTANT_DELAY
    tdsrlc_ul_lb_lc_info_type   loop_back_lc_info;
#endif
    
}tdsrlc_ul_logchan_format_type;

/*------------------------------------------------------------------*/
/* Uplink frame format of all RLC logical channels                  */
/*------------------------------------------------------------------*/

typedef struct {

  uint8  mac_e_hdr[TDSMAX_MAC_E_HDR_SIZE_BYTES];
                              /* Contains (DDIi, Ni) sets for i = 1..8 LCs */
                              /* including optional DDI0. This is given in */
                              /* the word format written to the ERAM bank       */

  uint16  mac_e_hdr_size;     /* Size of mac_e header in bits. RLC is expected
                              to compute the number of words that has to be written
                              from mac_e_hdr array to eram bank             */

  uint16  si_size;            /* 0 – Indicates SI field is not present     */
                              /* X – Indicates size of SI field in bits    */

  uint32  si;                 /* Contains the optional SI information. RLC */
                              /* is expected to write this as it is to ERAM*/

  uint32  padding_size;       /* 0 – no padding in E-DCH transport block   */
                              /* X – Indicates padding is present.         */
                              /* Note:                                     */
                              /* 1. If padding is present, the last cipher */
                              /* control word is prepared for padding along*/
                              /* with indication for last_uint_indicator.  */
                              /* 2. If padding is not present the last     */
                              /* cipher control word is prepared only for  */
                              /* indicating last_uint_indicator.           */

  tdsl1_l2_data_type *l1_ul_tx_buf;
                              /* Pointer to current UL buffer to store the */
                              /* IO vectors for the payloads.              */


    /* Number of logical channels */
    uint8   nchan;

    /* Format of each logical channel */
    tdsrlc_ul_logchan_format_type  chan_fmt[TDSUE_MAX_UL_LOGICAL_CHANNEL];

}tdsrlc_ul_frame_format_type;


/*------------------------------------------------------------------*/
/* Information of a single uplink RLC PDU data                      */
/*------------------------------------------------------------------*/

typedef struct {

    /* Pointer to RLC PDU data */
    dsm_item_type   *pdu_ptr;

    /* TRUE indicates if this PDU needs to be ciphered. FALSE in the case
    of control pdu which need not be ciphered */
    boolean         pdu_cipher_enable;

    /* 32-bit COUNT-C value used by ciphering engine */
    /* Valid only when "cipher_flag = TRUE" */
    uint32          count_c;

    /* Ciphering key ID and is used by ciphering engine */
    /* Valid only  when "cipher_flag = TRUE" */
    uint32          cipher_key_id;

    /* The ciphering algorithm can be either UE_0,  UE_1 or UE_2*/
    uecomdef_umts_cipher_algo_e_type cipher_algo;

}tdsrlc_ul_pdu_data_type;

/*------------------------------------------------------------------*/
/* PDU data of a single uplink logical channel                      */
/*------------------------------------------------------------------*/

typedef struct {

    /* Radio Bearer ID */
    rb_id_type      rb_id;

    /* RLC  ID */
    rlc_lc_id_type  rlc_id;

    /* Number of RLC PDUs */
    uint16          npdus;

    /* RLC PDU Size in bits */
    uint16          pdu_size;

    /* If ciphering is to be done, this flag is set to TRUE */
    /* Otherwise, it is set to FALSE */
    /* For TM mode logical channels,MAC decides this value */
    /* Otherwise, it is decided by RLC */
    boolean         cipher_flag;


    /* Bit offset from the start of the RLC PDU */
    /* Valid only when "cipher_flag = TRUE" */
    uint8           cipher_offset;

    /* Information of each PDU */
    tdsrlc_ul_pdu_data_type    pdu_info[TDSUE_MAX_UL_RLC_PDUS];

}tdsrlc_ul_logchan_data_type;

/*------------------------------------------------------------------*/
/* PDU data of the current uplink frame                             */
/*------------------------------------------------------------------*/

typedef struct {

    /* Number of RLC logical channels */
    uint8   nchan;

    /* PDU data of each logical channel */
    tdsrlc_ul_logchan_data_type  chan_info[TDSUE_MAX_UL_LOGICAL_CHANNEL];

}tdsrlc_ul_frame_data_type;



/*------------------------------------------------------------------*/
/* Header information of a single downlink transport block          */
/*------------------------------------------------------------------*/

typedef struct {

    /* Downlink RLC logical channel identity */
    rlc_lc_id_type  rlc_id;

    /* Bit offset of start of RLC PDU from start of transport block
    header */
    uint8           rlc_pdu_start_offset;

    /* Pointer to start of transport block header data */
    uint8           *hdr_ptr;

}tdsrlc_dl_tb_hdr_info_type;

/*------------------------------------------------------------------*/
/* Ciphering information of a single downlink transport block       */
/*------------------------------------------------------------------*/

typedef struct {

    /* If ciphering is to be done, it is set to TRUE */
    /* Otherwise, it is set to FALSE */
    boolean     cipher_flag;

    /* Ciphering algorithm */
    uecomdef_umts_cipher_algo_e_type ciphering_algo;

    /* Bit offset of ciphering from start of transport block header */
    /* Valid only when "cipher_flag = TRUE" */
    uint8       cipher_offset;

    /* Ciphering key ID used by ciphering engine */
    /* Valid only when "cipher_flag = TRUE" */
    uint32      cipher_key_id;

    /* 32-bit COUNT-C value used by ciphering engine */
    /* Valid only when "cipher_flag = TRUE" */
    uint32      count_c;

    /* Uplink Radio bearer ID used by ciphering engine */
    rb_id_type  rb_id;

}tdsrlc_dl_tb_cipher_info_type;


/*------------------------------------------------------------------*/
/* Downlink RLC PDUs                                                */
/*------------------------------------------------------------------*/
typedef struct {

    /* RLC logical channel identity */
    rlc_lc_id_type  rlc_id;

    /* RLC PDU size in bits */
    uint16          pdusize;

    /* Pointers to downlink RLC PDUs */
    dsm_item_type   *pdu_ptr;

}tdsrlc_dl_pdu_data_type;

/*------------------------------------------------------------------*/
/* Downlink RLC PDUs of entire frame                                */
/*------------------------------------------------------------------*/
typedef struct {

    /* Number of RLC PDUs */
    uint8   npdus;

    /* RLC PDUs of each valid downlink RLC logical channel */
    tdsrlc_dl_pdu_data_type  pdu_info[TDSUE_MAX_DL_RLC_PDUS];

}tdsrlc_dl_frame_data_type;




typedef struct{
  /* Number of channels for which status report has been requested for */
  uint8  nchan;
  /* Channels ids of the logical channels for which status has to be
  reported */
  rlc_lc_id_type rlc_id_list[TDSUE_MAX_UL_LOGICAL_CHANNEL];

} tdsrlc_status_report_req_type;



#define TDSUE_OPT_MAX_DL_RLC_PDUS 32

typedef struct
{
  q_link_type link; /**< Link to manage this item on the queue */
  uint8 num_complete_pdus;
  tds_complete_pdu_type TDS_COMPLETE_PDU[TDSUE_OPT_MAX_DL_RLC_PDUS];
} tdsrlc_enh_data_ind_type;

#define TDSRLC_MAX_TTI_DATA_DL 10

/* This will indicate if the get rlc buffer call is from L1 ISR */
extern boolean  tds_rlc_buffer_call_from_ISR;


/*===================================================================

                            FUNCTION DEFINITIONS

====================================================================*/

/*-------------------------------------------------------------------
FUNCTION tdsrlc_ul_enh_buffer_status

DESCRIPTION
  This function exists in RLC. MAC will call always this function
  for the BO, irrespective of UE capability and LC type i.e R99
  LC, E-DCH mapped LC.

--------------------------------------------------------------------*/
extern void tdsrlc_ul_enh_buffer_status
(
  tdsrlc_ul_lc_info_type    *lc_info_ptr,  /* Report buffer status for */
                                        /* only following channels  */
  tdsrlc_ul_buf_status_type *status_ptr    /* Pointer to UL Buf.Status */
);

/*-------------------------------------------------------------------
FUNCTION tdsrlc_ul_build_pdus

DESCRIPTION
  This function exists in RLC. MAC uses it to get RLC PDUs for a
  given frame

  RLC builds RLC PDUs as per the MAC-specified format (*fmtptr)
  and returns the frame data information

--------------------------------------------------------------------*/
extern    void  tdsrlc_ul_build_pdus
(
    /* Pointer to frame format data */
    tdsrlc_ul_frame_format_type  *fmtptr,

    /* Pointer to uplink frame data */
    tdsrlc_ul_frame_data_type      *ul_data_ptr
);


/*===========================================================================

FUNCTION RLC_UL_ENH_BUILD_PDUS

DESCRIPTION
  This function is executed in RLC. This is called to build pdus of
  logical channels that are mapped E-DCH transport channel.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void  tdsrlc_ul_enh_build_pdus
(
  tdsrlc_ul_frame_format_type *ul_fmt_ptr /* Pointer to frame format data */
);

#ifdef FEATURE_TDSCDMA_RLC_QXDM_LOGGING
/*===========================================================================

FUNCTION tdsrlc_ul_statistic_rlc_buffer_status

DESCRIPTION
  This function is executed in RLC. This is called to get rlc buffer status.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void  tdsrlc_ul_statistic_rlc_buffer_status
(
  rlc_lc_id_type rlc_id, /* rlc id */
  uint32  nbytes         /* buffer bytes */
);
#endif

#define tdsrlc_discard_dsm_item(item_ptr) \
             tdsrlci_discard_dsm_item(item_ptr, __FILENAME__, __LINE__)
/*===========================================================================

FUNCTION tdsrlci_discard_dsm_item

DESCRIPTION
    Discard the DSM Item as follows
    If the app_field is greater than zero, decrement it.
    If the resulting app_field is zero, 
       - DISCARD the DSM item by calling dsm_free_packet
    Otherwise
       - Nothing

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void tdsrlci_discard_dsm_item
(
 dsm_item_type **dsm_ptr,
 const char * file,
 uint32 line
);

/*-------------------------------------------------------------------
FUNCTION tdsrlc_dl_decode_pdu_hdr

DESCRIPTION
  This function exists in RLC. MAC uses it to get RLC ciphering
  parameters of a downlink transport block

  Depending on RLC header of the transport block, RLC returns ciphering
  information.

  This function is called in interrupt context of Physical Layer.

--------------------------------------------------------------------*/
extern  boolean  tdsrlc_dl_decode_pdu_hdr
(
    /* Pointer to transport block header information */
    tdsrlc_dl_tb_hdr_info_type     *hdrptr,

    /* Pointer to the ciphering info */
    tdsrlc_dl_tb_cipher_info_type  *cipherptr
);

/*-------------------------------------------------------------------
FUNCTION rlc_dl_pdu_data

DESCRIPTION
  This function exists in RLC and is used by MAC to indicate downlink
  PDUs

--------------------------------------------------------------------*/
extern  void    tdsrlc_dl_pdu_data_ind
(
    /* Pointer to DL Transport Block Data */
    tdsrlc_dl_frame_data_type  *dataptr
);

/*-------------------------------------------------------------------
FUNCTION RLC_PROCESS_STATUS_REPORT_REQ

DESCRIPTION
  This function is called in the context of the build frame
  indication . This function will trigger RLC to re-send
  the control pdu's fo rthe signalling channel for which the
  data was trashed at L1 in the last TTI because the activation
  time was too close.

  rlc_status_report_req will be populated/updated every TTI with
  information about which channels were chosen to build data from.

DEPENDENCIES
  L1 will have to detect that the some uplink data was trashed.
  This indication is then propogated to MAC in build frame indication

RETURN VALUE
  None

SIDE EFFECTS

  RLC will send status report for SRB logical channels.

--------------------------------------------------------------------*/
void tdsrlc_process_status_report_req
(
  tdsrlc_status_report_req_type *rlc_status_report_req
);


#if defined(FEATURE_TDSCDMA_RLC_QXDM_LOGGING)
/*===========================================================================

FUNCTION TDSRLC_DL_SET_CIPHER_LOGGING

DESCRIPTION
This function sets signal to log the cipher PDU packets in task context.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void tdsrlc_dl_set_cipher_logging
(
  void
);
#endif


/*===========================================================================

FUNCTION tdsrlc_enh_get_free_data_q_entry

DESCRIPTION
    - Returns entry from rlc_enh_free_data_q.
      If Quue is empty, NULL will be returned

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

tdsrlc_enh_data_ind_type *tdsrlc_enh_get_free_data_q_entry(void);

/*===========================================================================

FUNCTION tdsrlc_enh_add_to_free_data_q_entry

DESCRIPTION
    - Adds the rlc_enh_data entry into Free Data Queue rlc_enh_free_data_q

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void tdsrlc_enh_add_to_free_data_q_entry(tdsrlc_enh_data_ind_type *rlc_enh_data_ind_ptr);

/*===========================================================================

FUNCTION tdsrlc_enh_dl_pdu_data_ind

DESCRIPTION
    - Process TM data, if present and configured to process in interrupt context
    - If some data present,
        - Enqueue the RLC entry into rlc_enh_data_queue
        - Post a signal to RLC DL Task to process in DL Task context
      else
        - Return the entry to free queue

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void tdsrlc_enh_dl_pdu_data_ind(tdsrlc_enh_data_ind_type *rlc_enh_data_ptr);


/*===========================================================================

FUNCTION tdsrlc_enh_calc_rlc_hdr_length

DESCRIPTION
    - Return the RLC header length, which is
        2 for AM Data PDU, 0 for AM Ctrl PDU, 1 for UM and 0 for TM

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

uint8 tdsrlc_enh_calc_rlc_hdr_length(rlc_lc_id_type rlc_id, uint8 first_byte,
  boolean *is_tm_mode);


/*===========================================================================
FUNCTION: tdsmac_hs_enqueue_free_tsn

DESCRIPTION:
  Enqueue the tsn_info_buf to Free queue

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
===========================================================================*/
void tdsmac_hs_enqueue_free_tsn(tdsl1_dec_hs_tsn_info_type *tsn_info_buf_ptr);

/*===========================================================================

FUNCTION tdsrlc_enh_dl_pdu_data_ind_hs

DESCRIPTION
    - Enqueue the tsn entry into rlc_tsn_info_q
    - Post a signal to RLC DL Task to process in DL Task context

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void tdsrlc_enh_dl_pdu_data_ind_hs(tdsl1_dec_hs_tsn_info_type *tsn_info_buf_ptr);
/*===========================================================================

FUNCTION tdsrlc_enh_dl_tsn_flush_hs

DESCRIPTION
    - Flush the TSN from TSN q
    - 
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  There might be slight data loss dueing HS STOP

===========================================================================*/
 void tdsrlc_enh_dl_tsn_flush_hs(void);
#if defined (FEATURE_TDSCDMA_HSDPA_MAC_EHS)
/*===========================================================================
FUNCTION: tdsmac_ehs_query_hs_type

DESCRIPTION:
  returns HS Call type - HS or EHS

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
===========================================================================*/
mac_hs_e_type tdsmac_ehs_query_hs_type(void);

#endif /* ((defined (FEATURE_TDSCDMA_HSDPA_MAC_EHS))  */



/*===========================================================================
FUNCTION: MAC_UL_GET_CHANNEL_TYPE

DESCRIPTION:
  API to map UL RLC LC ID to channel type

DEPENDENCIES:
  None

RETURN VALUE:
  channel type

SIDE EFFECTS:
  None
===========================================================================*/
tdsrlc_ul_channel_type tdsmac_ul_get_channel_type(rlc_lc_id_type rlc_id);
                      
/*===========================================================================
FUNCTION: TDSL2_ACQ_UL_LW_LOCK

DESCRIPTION:
  API to acquire UL data path lock based on RLC LC id passed.

DEPENDENCIES:
  None

RETURN VALUE:
  Mutex locked R99/EUL.

SIDE EFFECTS:
  None
===========================================================================*/
rex_crit_sect_type* TDSL2_ACQ_UL_LW_LOCK(rlc_lc_id_type rlc_id);

/*===========================================================================
FUNCTION: TDSL2_ACQ_UL_LW_UNLOCK

DESCRIPTION:
  API to release UL data path lock passed.

DEPENDENCIES:
  None

RETURN VALUE:
  NONE

SIDE EFFECTS:
  None
===========================================================================*/ 
void TDSL2_ACQ_UL_LW_UNLOCK(rex_crit_sect_type* datapath_mutex);

/*===============================================================================================
FUNCTION NAME tdsl2_ul_get_trch_type

DESCRIPTION   This function returns the up-link transport channel type corresponding to given RLC & RB IDs
===============================================================================================*/

tdscdma_trch_type tdsl2_ul_get_trch_type(rlc_lc_id_type rlc_id, rb_id_type rb_id);

/*---------------------------------------------------------------------------
 *   Enum Flag used to determine the type of logging in HS path

 *   MAC_HS_DBG_LOGGING_ONLY: Extra logging in HS path; No extra check for validity of TSNs
 *   MAC_HS_DBG_VLDT_TSN: Extra logging and check for validity of TSNs in HS path
 *-------------------------------------------------------------------------*/

typedef enum
{
  TDSMAC_HS_DBG_NO_LOGGING,
  TDSMAC_HS_DBG_LOGGING_ONLY,
  TDSMAC_HS_DBG_VLDT_TSN
}tdsmac_hs_dbg_enum_type;
extern uint8 tdsmac_ul_debug_print_interval;

extern boolean tdsmac_enable_tfci_debug_dump;
extern boolean tdsmac_enable_low_medium_f3s;

extern tdsmac_hs_dbg_enum_type tdsmac_hs_logging;

#if defined (FEATURE_TDSCDMA_HSDPA_MAC_EHS)
extern uint32 tdsmac_ehs_logging;
extern uint32 tdsmac_ehs_p_to_c_logging;
#endif

typedef enum
{

  TDSMAC_DEBUG_NONE = 0,
  TDSMAC_ENABLE_TFCI_DEBUG_DUMP = 1, 
  TDSMAC_ENABLE_LOW_MEDIUM_F3S = 2,

  TDSMAC_DL_DEBUG_PRINT_INTERVAL = 3, 
  TDSMAC_UL_DEBUG_PRINT_INTERVAL = 4, 

  TDSMAC_HS_LOGGING = 5, 

#if defined (FEATURE_TDSCDMA_HSDPA_MAC_EHS)
  TDSMAC_EHS_LOGGING = 6, 
  TDSMAC_EHS_P_TO_C_LOGGING = 7
#endif

}tds_mac_debug_var_type;


#endif

/*remapping function definitions in l1_l2 to mac_rlc
The original functions are actually defined in l1
*/
#define tdsmac_rlc_build_cipher_engine_ctrl_info tdsl1_l2_build_cipher_engine_ctrl_info
#define tdsmac_rlc_build_copy_engine_ctrl_info tdsl1_l2_build_copy_engine_ctrl_info
#define tdsmac_rlc_write_hdr_section tdsl1_l2_write_hdr_section


