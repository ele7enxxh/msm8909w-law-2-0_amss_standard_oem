#ifndef PDCPI_H
#define PDCPI_H
/*===========================================================================

                                 P D C P I 

                           H E A D E R   F I L E


DESCRIPTION
  This file contains data declarations and function prototypes that are
  internal to the PDCP layer.
  
INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2005 - 2014 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/pdcp/inc/pdcpi.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/22/10   sch     Defined PDCP_INVALID_RLC_LC_ID 0xFF
08/31/09   ss      SU level CMI modifications.
03/04/09   sa      AU level CMI modifications.
02/01/07   rsl     Changes relating to CPU based flow control.
08/31/06   rsl     Featurized PDCP
08/18/06   rsl     Added support for traffic class under 
                   FEATURE_CPU_BASED_FLOW_CONTROL.

===========================================================================*/


/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include "ds_3gpp_hdlr.h"
#ifdef FEATURE_SEGMENT_LOADING
#include "IWCDMA.h"
#include "ITDSCDMA.h"
#endif /* FEATURE_SEGMENT_LOADING  */
#ifdef FEATURE_DATA_WCDMA_PS
#include "pdcp.h"
#include "comptask_v.h"
#ifdef FEATURE_CPU_BASED_FLOW_CONTROL
#include "dsumtspdpreg.h"
#endif
#ifdef FEATURE_QXDM_WM_TRACING
#include "fc_wm.h"
#endif
/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  Critical Section macros
---------------------------------------------------------------------------*/
  #define PDCP_INIT_CRIT_SECTION( rex_crit_section )                           \
    { rex_init_crit_sect( rex_crit_section ); }

  #define PDCP_ENTER_CRIT_SECTION( rex_crit_section )                          \
    { rex_enter_crit_sect( rex_crit_section ); }

  #define PDCP_LEAVE_CRIT_SECTION( rex_crit_section )                          \
    { rex_leave_crit_sect( rex_crit_section ); }


  #define PDCP_BRANCH_LEAVE_CRIT_SECTION( rex_crit_section )                   \
    { rex_leave_crit_sect( rex_crit_section ); }

/* PDCP critical section */
extern rex_crit_sect_type pdcp_crit_section;

#define MAX_PDCP_INST DS_3GPP_MAX_BEARER_CONTEXT  /* based on max num of RABs */
#define MAX_PID_ENTRIES 32                        /* as in TS 25.323 V5.3.0*/
#define NUM_IPHC_ENTRIES   5                      /* as in TS 25.323 V5.3.0*/
#define NUM_PDCP_PDU_TYPE_BITS 3                  /* number of bits in pdcp pdu 
                                               based on 25.323 */
#define NUM_PDCP_PID_BITS 5                       /* number of pid bits */
#define PDCP_HDR_LEN 1                            /* there is 1 byte of pdcp header 
                                               present in each packet */
#define PDCP_SEQ_NUM_LEN 2                        /* number of seq num bytes in 
                                               pdcp pdu */

#define ROHC_DEF_MAX_CID 15                       /* default value for max_cid as in
                                               RRC spec 25.331 */
#define ROHC_REV_DECOMP_DEPTH 0                   /* default is 0, reverse decomp
                                               is not used */
#define ROHC_MRRU 0                               /* segmentation is turned off */
#define ROHC_INVALID_CID 16384                    /* as defined in 25.331 */
#define PDCP_INVALID_RLC_LC_ID 0xFF
/*---------------------------------------------------------------------------
  Following ROHC parameter values were recommended by simulation studies
  done by R&D folks.
---------------------------------------------------------------------------*/
#define ROHC_MAX_JITTER_CD 10
#define ROHC_COMP_FLOW_WIN_WIDTH 6
#define ROHC_COMP_FLOW_NUM_UPDATE_IR 4
#define ROHC_COMP_FLOW_NUM_NON_UPDATES 3
#define ROHC_COMP_FLOW_SO_IRT 512
#define ROHC_COMP_FLOW_SO_FOT 255
#define ROHC_COMP_FLOW_TIMER_COMP FALSE
#define ROHC_COMP_FLOW_PROF_HINT 0
#define ROHC_COMP_FLOW_F_MAX_TIME 40000
#define ROHC_COMP_FLOW_EXP_BACKOFF 2
#define ROHC_DECOMP_FLOW_ACK_ENAB FALSE
#define ROHC_DECOMP_FLOW_RD_DEPTH 0
#define ROHC_DECOMP_FLOW_TIMER_BASED_DECOMP FALSE
#define IPHC_FULL_HEADER IPHC_PACKET_TYPE_FULL_HEADER
#define IPHC_COMP_TCP IPHC_PACKET_TYPE_COMPRESSED_TCP
#define IPHC_COMP_TCP_NODELTA IPHC_PACKET_TYPE_COMPRESSED_TCP_NODELTA
#define IPHC_COMP_NON_TCP IPHC_PACKET_TYPE_COMPRESSED_NON_TCP
#define IPHC_CONTEXT_STATE IPHC_PACKET_TYPE_CONTEXT_STATE

/*---------------------------------------------------------------------------
  packet types in pid mapping table, can range from 0-37.  PID table on
  UE and network has to match for pid table translation to work.  pid values
  go from 0-31 which is the index to the pid table array.  The pkt_type
  here can be > 31 since it's only an internal translation value.
---------------------------------------------------------------------------*/
typedef enum
{
  NO_HEADER_COMPRESSION = 0,
  CID_0 = IPHC_CONTEXT_STATE + 1,
  CID_1,
  CID_2,
  CID_3,
  CID_4,
  CID_5,
  CID_6,
  CID_7,
  CID_8,
  CID_9,
  CID_10,
  CID_11,
  CID_12,
  CID_13,
  CID_14,
  CID_15,
  CID_16,
  CID_17,
  CID_18,
  CID_19,
  CID_20,
  CID_21,
  CID_22,
  CID_23,
  CID_24,
  CID_25,
  CID_26,
  CID_27,
  CID_28,
  CID_29,
  CID_30,
  ROHC_PKT_FORMAT,
  INVALID_PKT_TYPE
} pkt_e_type;

/*---------------------------------------------------------------------------
  pid mapping table entry type, each entry has an optimization method and
  packet type.
  This table is as specified in 25.323 section 5.1.1
  The index in this table is the pid values, ranges from 0-31.
  pkt_type ranges from 0-37, used for pid value translation.
---------------------------------------------------------------------------*/
typedef struct
{
  pkt_e_type        pkt_type;   /* packet type */
}pid_map_table_entry_type;

/*---------------------------------------------------------------------------
  States of internal PDCP state machine
---------------------------------------------------------------------------*/
typedef enum
{
  PDCP_DOWN = 0,
  PDCP_TRANSPARENT = 1,
  PDCP_UP = 2
}pdcp_state_e_type;

/*---------------------------------------------------------------------------
  rlc state information stored as part of pdcp control block
---------------------------------------------------------------------------*/
typedef struct
{
  uecomdef_logch_mode_e_type lc_mode;         /* RLC channel mode AM/TM/UM */
  rlc_lc_id_type          rlc_dl_id;    /* RLC downlink logical channel id */
  rlc_lc_id_type          rlc_ul_id;    /* RLC uplink logical channel id   */
  q_type                  rlc_tx_q;                  /* RLC transmit queue */
  dsm_watermark_type      rlc_tx_wm_item;        /* RLC transmit watermark */
  q_type                  rlc_rx_q;                   /* RLC receive queue */
  dsm_watermark_type      rlc_rx_wm_item;         /* RLC receive watermark */
  boolean                 sent_close_to_rlc;    /* flag indicating if a Close
                                                window ind was sent to RLC */
} pdcp_rlc_state_type;

/*---------------------------------------------------------------------------
  Possible PDCP PDU types, data pdu and sequence number pdu (used during
  lossless SN sync procedure.
---------------------------------------------------------------------------*/
typedef enum
{
  PDCP_DATA_PDU = 0x00,
  PDCP_SEQ_NUM_PDU = 0x01,
  PDCP_MAX_PDU
}pdcp_pdu_e_type;

/*---------------------------------------------------------------------------
  main PDCP control block
  pdcp_entry_valid - whether this entry is valid
  ul_pid_map_table - pid table on the UL
  dl_pid_map_table - pid table on the DL
  pdcp_state - pdcp state
  next_rx_sn - SN rxed from network
  rohc_hdl - rohc handle returned by ROHC comp eng library
  iphc_hdl - iphc handle returned by IPHC comp eng library
  pdcp_info - pdcp_info for this pdcp inst
  rlc_state - rlc_info for this pdcp inst
  pdcp_to_rlc_flow_mask - flow mask controlled by RLC
  rlc_to_pdcp_sig - signal set by data on RLC_PDCP WM
  ps_to_pdcp_wm - tx wm between PS and PDCP
  pdcp_to_ps_wm - rx WM between PDCP and PS
  pdcp_to_ps_post_rx_func_ptr - function to invoke when data gets enqueued on
  the pdcp to ps WM 
  pdcp_to_ps_rx_func_para - parameter used with pdcp_to_ps_rx_func_ptr
  pdcp_to_ps_flow_mask - flow mask controlled by DS mode handler
  ps_to_pdcp_sig - pdcp's tx_sig set by DS
---------------------------------------------------------------------------*/
typedef struct
{
  boolean                    pdcp_entry_valid;
  pid_map_table_entry_type   ul_pid_map_table[MAX_PID_ENTRIES];
  uint8                      ul_next_free_pid;
  uint8                      ul_hash_table[INVALID_PKT_TYPE];
  pid_map_table_entry_type   dl_pid_map_table[MAX_PID_ENTRIES];
  uint8                      dl_next_free_pid;
  pdcp_state_e_type          pdcp_state;                       
  uint16                     next_rx_sn;
  rohc_handle_type           rohc_hdl;
  void                       *iphc_hdl;
  pdcp_info_type             pdcp_info;
  rohc_cid_inc_info_e_type   rohc_ul_cid_inc_info;
  rohc_cid_inc_info_e_type   rohc_dl_cid_inc_info;
  pdcp_rlc_state_type        rlc_state;
  ds3g_flow_e_type           pdcp_to_rlc_flow_mask;
  comp_sig_enum_type         rlc_to_pdcp_sig;      
  dsm_watermark_type         *ps_to_pdcp_wm;
  dsm_watermark_type         *pdcp_to_ps_wm;
  void                       (*pdcp_to_ps_post_rx_func_ptr) (byte, byte, void *);
  void                       *pdcp_to_ps_rx_func_para;
  ds3g_flow_e_type           pdcp_to_ps_flow_mask;    
  comp_sig_enum_type         ps_to_pdcp_sig;          
#ifdef FEATURE_CPU_BASED_FLOW_CONTROL
  ds_umts_qos_tclass_enum_type traf_class;          
#endif /* FEATURE_CPU_BASED_FLOW_CONTROL */
  boolean                     is_mcast_call;
  uint32                     dl_data_rate;
  boolean                    pdcp_wmk_hi;
  sys_modem_as_id_e_type     subs_id;  
}pdcp_ctl_type;


/*---------------------------------------------------------------------------
  Global PDCP control block array.
---------------------------------------------------------------------------*/
extern pdcp_ctl_type pdcp_ctl_blk[MAX_PDCP_INST];

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================
FUNCTION      PDCP_POWERUP_INIT

DESCRIPTION   This function should be called at UE powerup from ps task init.
              Initializes all variables associated with pdcp control block.


DEPENDENCIES
    None

RETURN VALUE
    None

SIDE EFFECTS
    None
===========================================================================*/
void pdcp_powerup_init
(
  void
);
/*===========================================================================
FUNCTION PDCP_VALIDATE_PDCP_INST

DESCRIPTION This function checks to make sure given pdcp inst has a valid
            pdcp ctl blk entry.
  
DEPENDENCIES
  None

RETURN VALUE
  true - if valid entry is found
  false - if valid entry is not found

SIDE EFFECTS
  None
===========================================================================*/

boolean pdcp_validate_pdcp_inst
(
  uint32 pdcp_inst
);
/*===========================================================================
FUNCTION PDCP_TX_DATA_HDLR

DESCRIPTION
  This is the function that handles data on the transmit path, compresses
  data on PDCP WM and enqueues compressed data onto the RLC WM.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: There is more data to be transmitted.
  FALSE: There is no mode data.
  
SIDE EFFECTS
  PDCP task gets a chance to run again if there is more data to be trasmitted.
===========================================================================*/

boolean pdcp_tx_data_hdlr
( 
  uint8 pdcp_inst
);
/*===========================================================================
FUNCTION PDCP_RX_DATA_HDLR

DESCRIPTION
  This function forwards pkts received on Um interface to ps_iface for 
  pdp-ip call.
  
PARAMETERS   
  pdcp_inst - the instance that has data to receive.
      
DEPENDENCIES 
  None.
  
RETURN VALUE 
  TRUE: There is more data on the pdcp rx WM.
  FALSE: There is no mode data on the WM.
     
SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean pdcp_rx_data_hdlr
(
  uint8 pdcp_inst
);

#ifdef FEATURE_QXDM_WM_TRACING
/*===========================================================================
FUNCTION  PDCP_REPORT_WM_STATS_CB

DESCRIPTION
  Called by FC to gather watermark statistics

PARAMETERS
  call_instance - Index into call table for the call
  wm_stat_ptr - Pointer to structure where fc stores watermark info
DEPENDENCIES
  Called in FC TASK context.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void pdcp_report_wm_stats_cb
(
  uint32 pdcp_inst,
  fc_wm_stat_type *wm_stat_ptr
);

#endif /* FEATURE_QXDM_WM_TRACING */

#endif /* FEATURE_DATA_WCDMA_PS */



#endif /* PDCPI_H */
