#ifndef L1_MAC_DATA_H
#define L1_MAC_DATA_H

/*============================================================================
                            L1 DATA INTERFACE 


GENERAL DESCRIPTION
  This files contains all necessary control and data definitions for 
  interactions between L1 and MAC. It defines the format for packing uplink 
  transport channel data and control information for transmission. It also 
  defines transport block formats that aid in decoding downlink physical 
  channels. Interfaces dealing with data flow between MAC and L1 are also 
  defined.
  
EXTERNALIZED FUNCTIONS

INTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS  
  

Copyright (c) 2000 - 2012 by Qualcomm Technologies, Inc.  All Rights Reserved.

============================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/l1/vcs/l1macdata.h_v   1.14   18 Jun 2002 13:32:06   halbhavi  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/inc/l1macdata.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     -------------------------------------------------------- 
10/17/14    ts      Fix to discard DL PDUs received while in the Reset Pending State, to avoid potential ciphering mismatch right after the recovery
10/01/14    gv      Code cleanup. Removal of stale and unused code. 
07/28/14    pkg     Code Check-in for 16QAM feature.
06/23/14    pkg     Moving DC-HSUPA specific code under FEATURE_WCDMA_DC_HSUPA flag.
05/13/14    ymu     Feature name FEATURE_WCDMA_DC_HSUPA_FRAME replacement with FEATURE_WCDMA_DC_HSUPA 
03/31/14    ymu     Fix for SRB delay on primary carrier taking longer than configured value
                    and SRB delay on secondary mistakenly applied on primary
03/27/14    ymu     Fix CPC preample tx on Primary carrier when only secondary is Tx EDCH.
01/09/14    vr      Code changes for UL Compression Feature
12/12/13    yw      Secondary carrier Happy Bit fix (L1 part)
12/11/13    gp      FR:3693 EUL stats API for data services
11/22/13    ts      DCHSUPA + CPC: Fix for x430E SFN #s incremented incorrectly during MAC DTX cycle gaps
11/15/13    as      Added code change to provide SRB data presence & corresponding TTI information to
                    L1 which in turn would increase the Tx priority for W during SRB data transmission.
05/22/13    oh      Adding DC-HSUPA framework support
05/09/13    gp      Code cleanup for FEATURE_WCDMA_UL_CM_INTLV_WORKAROUND and FEATURE_WCDMA_CONCURRENT_R99_EUL_TX_ENCODE
05/08/13    scv     Fix L2 compilation errors
02/05/13    at      Mainlined UL changes for Frame Early Termination
02/05/13    at      Checked in Uplink changes to Gate FET when UL DCCH data is present
12/11/12    vr      HSRACH code cleanup
11/09/12    ms      Removing a few lines used in the Protocol Processor context
10/26/12    ash     Dime Feature clean up. Mainlined FEATURE_L1_SRB_DELAY flag
10/15/12    jd      Memory Optimization Changes
10/11/12    vr      HSRACH: L1 changes for MAC 0x4314 log packet
10/01/12    jhl     Added extern declaration of wplt_l1_phy_dl_trblk_data_ind
                    to fix compiler warnings
08/31/12    vr      HSRACH specific changes for unified WPLT build
07/24/12    geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
06/29/12    vr      HSRACH fixes
06/08/12    gv      Feature HS RACH Initial code check in
03/24/12    ms      Updated l1_prach_status_enum_type enum definition
03/21/12    ms      Added l1_prach_status_enum_type enum definition
03/09/12    at      Added interface for L1 to query L2 buffer occupancy status
02/01/12    raj     Nikel Feature cleanup.
02/01/12    gnk     Nikel Feature cleanup.
01/26/12    vs      Nikel feature cleanup.
01/26/12    zr      Mainlined FEATURE_MCAL_WCDMA, removed FEATURE_AL1
12/06/11    ksr     Fix to handle back to back RACH ABORT and RACH NO ACK to MAC from L1
                    Moving to command based interface from signal based approach.
10/21/11    sp      Nikel UL CM workaround under FEATURE_WCDMA_UL_CM_INTLV_WORKAROUND
04/28/11    sj      upmerge from offline/main
04/26/11    gnk     Removed FEATURE_WCDMA_HSUPA_IRQ_DELAY_FLOW_CONTROL feature
03/30/11    gnk     Mainlined several features
03/11/11    dp      Ported CPC DRX changes to offline Architecture
02/23/11    asm     Cleanup of unused, mainlined or irrelevant features in offline UL code
12/17/10    gv	    L1-MAC interface change to support SRB Omit and Low grant
                    scenarios in a DTX call.
12/10/10    oh      Backing out fix for Incorrect SP during Low/No Grant state, as fix is incomplete. 
12/03/10    gv      Modified the prototype of L1-MAC data Query function to include
                    whether the OTA HARQ is active and SRB OMIT info
11/02/10    gv      Provided framework for HARQ deactivation interface with
                    CPC DTX for L1 Query MAC for data present.
10/19/10    ms      Added prototype for l1_phy_dl_flush_tsn
01/10/10    gnk     Added support for DTX on PLT
09/02/10    gv      Code Checkin for CPC DTX
08/08/10    sj      Added unified PLT support
06/25/10    ms      Added prototype for l1_dl_phy_hspa_gather_and_decipher
02/05/10    ms      Taxis/Genesis code merge
11/16/09    asm     Added EUL Datapath support with A2 and Removed P2-related changes
11/09/09    rmsd    Fixed merge error.
10/26/09    ms      Indicate to MAC whether the HS blocks being delivered
                    belong to current TTI or previous TTI or both.
10/24/09    ms      A2 L1-L2 interface support
09/02/09    ms      Added support for TSN tracing.
06/26/08    av      l1-l2 interface cleanup 
06/02/09    rm      Added cipher_algo to l1_ul_tb_data_type
04/24/09    mr      Fixed Compiler Warnings
04/09/09    ms      Code cleanup.
04/01/09    ms      Added feature dependency FEATURE_DATA_PS_HDLC_PP.
03/19/09    ms      Defined L1_DEC_MAX_COMPLETE_PDU_FROM_PREV_TTI for MAC use.
12/23/08    ms      Replaced dob_handle_crit_sect with dec_tsn_q_crit_sect.
12/18/08    ms      Added extern for dob_handle_crit_sect for L2 use.
12/08/08    asm     Corrected Featurization
10/03/08    hk      Bringing in 7k Mailine fixes
06/19/08    ms      Featurized MAC-ehs code under FEATURE_HSDPA_MAC_EHS.
06/10/08    ms      Added support for MAC-ehs
10/24/07    vsr     Mainlining MBMS and Enhanced DL changes 
10/10/07    gv      Defined prototype for l1_edch_update_mac_log_packets_during_reconfig().
05/30/07    yiz     Call l1_edch_update_mac_harq_tx_info() after build_frame_ind
05/04/07    am      Add ACK status from srv RLS to L1-MAC interface
03/27/07    oh      Enhanced support to control EUL params(fixed ETCFI for debugging) via QXDM
02/05/07    yiz     Add max_allowed_etfci when requesting E-TFC from MAC
02/05/07    am      4311 logpkt changes
02/01/07    gv      Added function prototype l1_mac_eul_serv_cell_and_rls_change_ind
                    that is called by L1 to indicate MAC about the serving
					cell change, which inturn leads to triggering SI.
01/31/07    asm     Increase L1 UL restriction of Max TBs from 16 to 24
12/20/06    am      Send "SI sent alone" info to SG module
12/18/06    am      Add l1_phy_edch_get_mac_hs_ul_mac_d_si_log_info() to fix compiler
                    warning
09/28/06    mc      Added MCAL decoder support under feature 
                    FEATURE_MCAL_WCDMA
09/15/06    am      Added code for EUL IOT fixes.Changed power offset query() 
                    params to structure.
08/14/06    yiz     Mainline HSUPA code
03/02/06    yiz     Added fix of trashed UL L2 ACK
04/18/05    gs      Merged code from 6275 baseline
01/26/05    vb      Added l1_mac_suspend_resume_enum_type
11/12/04    yiz     Change RACH status enum to handle new FW interrupt at AICH ACK.
08/30/04    gs      Added interface definitions for HSDPA
09/02/04    yiz     Add UL TB_SIZE limit and l1_phy_ul_cfn_missed_ind().
08/11/04    yiz     Change constant values for FEATURE_WCDMA_PWR_TFC_ELMN per SPEC.
07/14/04    scm     Add extern to mac_update_hfn_on_rollover().
04/15/04    yiz     Rename FACH inter-RAT CR as FEATURE_CELL_FACH_MEAS_OCCASION
03/25/04    yiz     Add param ul_tx_disabled to build_frame_ind().
12/10/03    yiz     Rename L1_PRACH_WRONG_TTI to L1_PRACH_DELAY to delay RACH TX
                    for FEATURE_INTER_RAT_CELL_FACH_RE_SELECTION
05/27/03    gs      Added the definition of structure and functions required
                    for compressed mode by HLS.
02/04/03    gs      Added TM ciphering/deciphering support
10/22/02    yiz     Rename parameter tfc_pwr_state_ptr to tfc_excs_pwr_state_ptr
                    and quantify it as const for l1_phy_ul_build_frame_ind().
10/07/02    yiz     Add L1_PRACH_WRONG_TTI to l1_prach_status_enum_type for
                    MAC data at wrong TTI for RACH.
10/04/02    yiz     Add tfc_pwr_state_ptr to l1_phy_ul_build_frame_ind() to support
                    power based TFC elimination under FEATURE_WCDMA_PWR_TFC_ELMN
02/07/02    yiz     Add param SRB_omit to l1_phy_ul_build_frame_ind() under
                    FEATURE_L1_SRB_DELAY.
09/12/01    sk      Updated L1_TX_BUF_DSM_DATA macro to use byte offset.
08/21/01    gs      1. Removed the code for TB memory buffer other that DSM items
                    2. Updated the comments of structure member data_size in structure
                       l1_dl_tb_data_in_tbset_struct_type to reflect its meaning in
                       loopback mode 2.

07/17/01    gs      1. Added the TB index in the TB hdr info for the feature
                    FEATUTE_TB_IND_DETAIL_INFO.
                    2. Added num_total_tb for the above feature in tb_set hdr info.
                    
07/06/01    sk      Removed mac_busy flag.
06/18/01    gs      Changed the macro defining the number of TBset and max TB
                    in the TB Hdr ind keeping in view the number of status FIFO
                    in mDSP rather than UE capability.
                    Added strcuture member in l1_dl_tb_set_hdr_struct_type for
                    passing the failure info in case feature flag
                    FEATURE_TB_IND_DETAIL_INFO is defined.
06/12/01    sk      Added UL_PRACH_ABORT to RACH status.
05/29/01    gs      Added the tfi structure member in l1_dl_tb_set_hdr_struct_type
                    to support the loopback.
02/25/01    sk      Introduced macros to use DSM buffers.
01/22/01    sk      Re-arranged order in l1_prach_status_enum_type
                    Updated macros that accessed MAC header and data
01/17/01    sk      updated MAC Header and data to be used as byte arrays
11/17/00    sk      updates for cfn and count types in UL
09/13/00    sk/gs   file created.
                    
===========================================================================*/

/*===========================================================================

                      INCLUDE FILES

===========================================================================*/

#include "wcdma_variation.h"
#include "comdef.h"
#include "customer.h"
#include "dsmutil.h"
#include "l1sapcommon.h"
#include "l1ulcfg.h"
#include "l1macif.h"
#include "rex.h"
#include "a2_common.h"
#include "l1def.h"

/*===========================================================================

                        FEATURE DEPENDENCIES

===========================================================================*/
/* FEATURE_HSDPA has to be enabled for FEATURE_WCDMA_DL_DATA_PATH_OPT */

#ifdef FEATURE_DATA_PS_HDLC_PP
#error code not present
#endif



/*===========================================================================

                      DOWNLINK DATA DECLARATIONS

===========================================================================*/
/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/* This is the maximum number of transport block set for which the transport
   block headers can be reported by L1 to L2 for decoding
   Note that size defined is based on the maximum amout that can be
  reported by mDSP. SW has to process that and should have that much capacity.
  This is not as per UE capability specified in standard */
#define L1_DEC_MAX_TB_SET_HDR  32

/* 32 R99 Max TB ina frame for all TTI ending at this frame (No double buffering
   Max 70 PDU per sub frame Number of sub frames to cover are 5 (in 10 ms) +
   2 (To cover interrupt latency
   Total transport block (MAC-d PDUs) will be 32 + 7*70 = 524 */
#define L1_DEC_MAX_TB          524

/* Number of word 32 data reads required for transport block header */
#define L1_DEC_TB_HDR_SIZE_W32 3
/* Number of word 32 data reads required for HS-DSCH MAC-d PDU */
#define L1_DEC_HS_MAC_D_HDR_SIZE_W32 1

#define PWR_TFC_ELMN_X        15
#define PWR_TFC_ELMN_Y        30  /* shall be multiples of 15 */
#define PWR_TFC_ELMN_Y_FRAME   2  /* Y/15 */

/* DOB Extension buffer size based on which MAC would indicate
   appropriate number of P-to-Cs to be formed. */
#define L1_DEC_DOB_EXTN_BUF_SIZE MCALWCDMA_DEC_DOB_EXTN_BUF_SIZE

/* Number of P-to-Cs that MAC could indicate L1 to concatenate in one shot. */
#define L1_DEC_MAX_COMPLETE_PDU_FROM_PREV_TTI MCALWCDMA_DEC_MAX_COMPLETE_PDU_FROM_PREV_TTI

/* Number of TSN info buffers. This value is exported for MAC to use. */
#define UE_NUM_TSN_INFO_BUF MCALWCDMA_DEC_MAX_TSN_INFO_BUF

/* During Reset Pending State, MAC will ask decoder to set the MSB for Data PDUs received for that LC */
/* Those PDUs with the MSB set will be further discarded at RLC */
/* This is implemented to get rid of the potential ciphering mismatch for the PDUs reassembled just after the recovery from RESET*/
#define L2_PDU_DISCARD_HDR_MSB_POS  31

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */
#ifdef FEATURE_WCDMA_HS_RACH

typedef enum{
 L1_MAC_HS_RACH_CCCH_TRANS_TYPE,
 L1_MAC_HS_RACH_DTCH_DCCH_TRANS_TYPE,
 L1_MAC_INVALID_TRANS_TYPE
}l1_mac_hs_rach_trans_type_enum_type;
/* Below ENUM provides the different cause for the release
** of a common EDCH resource, in HS RACH*/
typedef enum{
 /* Release Caue because of AGCH Collision resolution failure*/
 WL1_HS_RACH_EDCH_REL_AGCH_COLL_TIMER_EXPIRY,

 /* Because the Maximum time allocated for CCCH transmission
 ** expired*/
 WL1_HS_RACH_EDCH_REL_CCCH_TIMER_EXPIRY,
 
 /* Release as the NW sent an INACTIVE AGCH to the UE*/
 WL1_HS_RACH_EDCH_REL_INACTIVE_AGCH,
 
 /* UE releasing the common EDCH resource as the condition
 ** TEBS == 0 have been satisfied*/
 WL1_HS_RACH_EDCH_REL_TEBS_ZERO,

 /* UE releasing the resource on account of RL failure*/
 WL1_HS_RACH_EDCH_REL_RL_FAILURE,

 /* UE releasing the resource as Post verification failed*/
 WL1_HS_RACH_EDCH_REL_PHYCHAN_EST_FAIL,

 /* Release as RRC sent out an CPHY_IDLE_REQ*/
 WL1_HS_RACH_EDCH_REL_IDLE_REQ,

 /* Release as RRC sent a CPHY_WCDMA_SUSPEND_REQ*/
 WL1_HS_RACH_EDCH_REL_SUSPEND_REQ,

 /* Release as RRC sent a HSRACH stop */
 WL1_HS_RACH_EDCH_REL_PHYCHAN_DROP,

 /*INVALID ENUM*/
 WL1_HS_RACH_EDCH_REL_INVALID
}l1_mac_hs_rach_edch_rsrc_rel_cause_enum_type;
/* This structure stores the information that L1 gets from MAC for the HS RACH  
 ** procedure*/
typedef struct{
  /* This element stores the transmission type, i.e whether the HS RACH 
  ** is being setup for CCCH or DCCH/DTCH */
  l1_mac_hs_rach_trans_type_enum_type edch_trans_type;  
  /* Get the Additional EDCH transmission back off param for 
  ** using within layer 1.*/
  uint16 addtnl_edch_backoff;  
  uint16 maxCCCHAllocation;
  uint16 maxAGCHCollResolution;
  uint16 edchTxContBackOff;  
}l1_mac_hs_rach_config_params;

#endif /*FEATURE_WCDMA_HS_RACH*/
typedef enum
{
  L1_MAC_SUSPEND,
  L1_MAC_RESUME
} l1_mac_suspend_resume_enum_type;

typedef mcalwcdma_dec_tb_tm_cipher_state_enum_type  l1_tb_tm_cipher_state_enum_type;

typedef mcalwcdma_dec_tsn_trace_enum_type l1_dec_tsn_trace_enum_type;

#ifdef FEATURE_WCDMA_HS_RACH
typedef enum{
 L1_MAC_HS_RACH_TIMER_ACTIVE,
 L1_MAC_HS_RACH_TIMER_EXPIRED,
 L1_MAC_HS_RACH_TIMER_INACTIVE
}l1_mac_hsrach_eul_timer_status_enum;
#endif

/* This strcuture defines the multisegmented TB buffer to be
   defined by MAC */
typedef struct
{
  /* Number of TB buffer segment */
  uint8 num_seg;
  
  /* Size of each TB segment */
  uint16 seg_size;
  
  /* Pointer to the array of pointer of Tb buffer segment */
  uint8 **buf;
} l1_dl_tb_buf_multi_seg;

/* Indication whether data being delivered to L2 belongs to
   current TTI or previous TTI or both. */
typedef mcalwcdma_dec_mac_hs_tti_type l1_dl_hs_tti_enum_type;

/* This structure defines the ciphering parameters for a transport block
   header information of a transport block. This is included in transport
   block info structure (l1_dl_tb_hdr_struct_type). */
typedef mcalwcdma_dec_ciphering_param_struct_type  l1_dl_ciphering_param_struct_type;

/* This structure defines the transport block header information for a
   transport block. This is included in transport block set structure
   (l1_dl_tb_set_hdr_struct_type) that accommodates all transport blocks */
typedef mcalwcdma_dec_tb_hdr_struct_type  l1_dl_tb_hdr_struct_type;

/* This structure defines the transport block header information for a
   transport block set (or a transport channel). This is the base structure
   for each transport block set to be included in transport block structure
   (l1_dl_tb_hdr_decode_struct_type) that accommodates all transport block
   sets */
typedef mcalwcdma_dec_tb_set_hdr_struct_type  l1_dl_tb_set_hdr_struct_type;

/* Access macros for featurized structure member.
   It provides access to non_hs members access based on feature definition */

#ifdef __GNUC__

 #define L1_DL_TB_SET_HDR(hdr_struct_ptr,member) \
  hdr_struct_ptr->info.non_hs.member

#else /* __GNUC__ */

 #define L1_DL_TB_SET_HDR(hdr_struct_ptr,member) \
  hdr_struct_ptr->info.non_hs.##member

#endif /* __GNUC__ */

/* This structure defines the information of all transport block headers
   information for all transport block sets for which the headers need to be
   decoded and information is required to get the data from the hardware. */
typedef mcalwcdma_dec_tb_hdr_decode_struct_type  l1_dl_tb_hdr_decode_struct_type;

/* This structure defines the data element for a transport block in a 
   transport block set. This is included in the structure
   (l1_dl_tb_set_data_struct_type) */
typedef  mcalwcdma_dec_tb_data_in_tbset_struct_type  l1_dl_tb_data_in_tbset_struct_type;

/* This structure defines the data and status element for a transport block
   set. This is included in the structure (l1_dl_tb_data_struct_type) which 
   is used for sending the data and status information for all transport
   block sets to L2. */
typedef mcalwcdma_dec_tb_set_data_struct_type  l1_dl_tb_set_data_struct_type;

/* This structure defines the transport block information that is used to get
   the transport block headers decoded and update the transport block
   information to the L2 buffers */
typedef mcalwcdma_dec_tb_data_struct_type  l1_dl_tb_data_struct_type;

#ifdef FEATURE_WCDMA_HS_RACH
typedef struct{
 boolean agch_coll_resol_timer_valid;
 l1_mac_hsrach_eul_timer_status_enum agch_coll_resol_timer_status;
 boolean ccch_timer_valid;
 l1_mac_hsrach_eul_timer_status_enum ccch_timer_status;
 boolean tebs_zero_timer_valid;
 l1_mac_hsrach_eul_timer_status_enum tebs_zero_timer_status;
}l1_mac_hs_rach_eul_timers_status_struct_type;
#endif
/* This structure defines the HS transport block information passed to
   MAC and L2 during Data Read Event handling. */
typedef mcalwcdma_dec_tsn_info_type l1_dec_hs_tsn_info_type;
#ifdef FEATURE_HSDPA_MAC_EHS
typedef mcalwcdma_dec_concatenated_pdu_tsn_type l1_dec_concatenated_pdu_tsn_type;

/* Move-in task source and destination descriptors */
/* MAC will populate the source descriptor list
 * which spans across all PDUs for the 10ms TTI */

/* Critical section mutex for TSN info Q */
extern rex_crit_sect_type dec_tsn_q_crit_sect;

/* L1 will determine the destination address and
 * populate the destination descriptor list */
#endif /* FEATURE_HSDPA_MAC_EHS */

/*===========================================================================

                      UPLINK DATA DECLARATIONS

===========================================================================*/

#ifdef FEATURE_WCDMA_HS_RACH
extern boolean hsrach_debug_enable_f3;
#endif

  /* maximum # of transport blocks in a 10 ms frame */
#define L1_UL_MAX_TRBLK        24

  /* maximum # of bits in a transport block */
#define L1_UL_MAX_TB_SIZE      5120

  /* maximum # of TrCHs in a 10 ms frame */
#define L1_UL_MAX_TRCH         L1_UL_TRCH_MAX

  /* MAC header size in bytes */
#define L1_UL_MAC_MAX_HDR_SIZE 5

/* uplink transport block format (control + data) */
typedef struct {
     /* pointer to transport block data */

  dsm_item_type *data_ptr;

    /* combination of hyper frame num and RLC seq num */
  uint32         count;

  /* Ciphering Algorithm: Kasumi or Snow3G*/
  uint32 cipher_algo;

    /* MAC header data */
  uint8          mac_hdr[L1_UL_MAC_MAX_HDR_SIZE];

    /* size of MAC header in bits (0..39)*/
  uint8          mac_hdr_size;
    /* ciphering flag for this block */
  boolean        ciphering_on;
 #ifdef FEATURE_WCDMA_L1_TM_CIPHER_SUPPORTED
  /* TM ciphering state */
  l1_tb_tm_cipher_state_enum_type tm_cipher_state;
 #endif
    /* radio bearer id */
  uint8          radio_bearer_id;
    /* ciphering key */
  uint8          ciphering_key_id;

    /* offset in transport block from where ciphering should start */
  uint8          offset;


} l1_ul_tb_data_type;

/* transport channel data */
typedef struct {
    /* transport channel data */
  l1_ul_tb_data_type trch_data[L1_UL_MAX_TRBLK];

  /* transport block size in bits (0..5120) */
  uint16             tb_size;

    /* number of transport blocks - also indicates no TrCH if zero */
  uint8              num_tb;
} l1_ul_trch_data_type;

/* Tx data (for a 10ms frame) + control information */
typedef struct {
    /* data of all transport channels in the 10ms frame */
  l1_ul_trch_data_type tx_data[L1_UL_MAX_TRCH];

    /* TFCI used for this 10ms frame of Tx data */
  uint16               TFCI;
    /* number of transport channels */
  uint8                num_trch;
    /* flag to indicate if MAC is busy writing this buffer */
  boolean              mac_busy;

    /* ASC index valid for PRACH only */
  uint8                asc_num;

  /* SRB data present or not in this TTI- TRUE means SRB data present */
  boolean              srb_data_present;
 
  /* Indicates number of TTI in which SRB data would go */
  l1_tti_enum_type     srb_data_num_tti;
} l1_ul_tx_data_type;



/* Definition for UL TFC restriction for compressed mode by HLS */

typedef struct
{
  /* number of TFCI to be evaluated */
  uint8 ntfc;
  /* TFCI passed from MAC to be evaluated for restriction by HLS */
  uint8 tfci_list[L1_UL_TFC_MAX];
  /* TFCI restricted or allowed as returned by L1 after evaluation */
  boolean restricted[L1_UL_TFC_MAX];
} l1_ul_allowed_tfci_list_type;

typedef void L1_UL_QUERY_TFCS_RESTRICT_CB_FUNC_TYPE(
  /* Pointer to array of TFCS restriction */
  l1_ul_allowed_tfci_list_type*);


/* Below structure is used to pass the non sg information 
** from MAC to L1 when Secondary UL carrier is active */
typedef struct
{
  /*Number of non SG bits to be used for tx for that TTI*/
  uint16 non_sg_payload_selected;
  /* HARQ PO to be used for Non Scheduled UL tx for that TTI*/
  uint8 non_sg_harq_po;

}l1_eul_pwr_offset_sec_ul_non_sg_struct_type;

/* Below enum type is for carr_idx corresponding to Primary or Secondary UL carrier*/
typedef enum
{
  /*Primary UL carrier idx*/
  EUL_PRI_CARR_IDX,
  /*Secondary UL carrier idx*/
  EUL_SEC_CARR_IDX,
  /*Invalid idx*/
  EUL_INVALID_CARR_IDX

}l1_eul_carr_idx_enum_type;

typedef struct
{
  
  /*  Indicates whether the structure info is for Primary or Sec UL carr  */
  l1_eul_carr_idx_enum_type            l1_eul_carr_idx;
  
  uint8                     prim_car_active_harq_bit_mask;
  uint32                    prim_car_serving_grant;
  
  uint8                     sec_car_active_harq_bit_mask;
  uint32                    sec_car_serving_grant;
  
  /*----------------------------------------------------------------
  False indicates MAC doesn't need to do anything PERHAPS happy bit
  computation.Edch build frame indication will not be called in case
  of FALSE
  ----------------------------------------------------------------*/
  boolean                   new_transmission;

  /*----------------------------------------------------------------
  The value as number of bits indicates the maximum payload that can
  carry traffic from scheduled MAC-d flows. Values in range 0-20000
  0 indicates no serving grant is available
  ----------------------------------------------------------------*/
  uint16                    srv_grant_payload_bits;


  /*----------------------------------------------------------------
  The value as E-TFCI indicates the maximum payload that can carry
  traffic from both scheduled, non-scheduled MAC-d flows. This also
  includes the Scheduling information. Values in range 0-127
  0xFF indicates no power is available to transmit any E-DCH data
  ----------------------------------------------------------------*/
  uint8                     ue_tx_pwr_max_payload_idx;


  /*----------------------------------------------------------------
  This indicates the HARQ process ID that will be used for the
  following transmission.
  ----------------------------------------------------------------*/
  uint8                     harq_id;


  /*----------------------------------------------------------------
  TRUE indicates that for the DCH a non-empty TFCI was selected
  ----------------------------------------------------------------*/
  boolean                   dpdch_present;

  /*----------------------------------------------------------------
  Maximum E-TFCI allowed by current configuration from UTRAN
  ----------------------------------------------------------------*/
  uint8                     max_allowed_etfci;

  /*----------------------------------------------------------------
  This bit mask is only for logging purposes. This bit mask indicates whether the binary
  search in the R_combo table ended up in overflow because of SG or MP. This means
  that the SG or MP can support more number of bits than the configuration can.
  #define L1_SGP_OVERFLOW_IND_BMSK 0x1
  #define L1_MP_OVERFLOW_IND_BMSK 0x2
  ----------------------------------------------------------------*/

  uint8 sg_mp_overflow_ind_bmsk; 
  
} l1_eul_serving_grant_update_info_type;

/*------------------------------------------------------------------
This structure gives the HARQ profile corresponding to the HARQ ID
of the upcoming transmission.
------------------------------------------------------------------*/
typedef struct
{
  /*----------------------------------------------------------------
  Indicates the power offset of the highest priority logical
  channel carrying data. Value in the range 0-6
  ----------------------------------------------------------------*/
  uint8     harq_power_offset;

  /*----------------------------------------------------------------
  Maximum number of retransmissions allowed in this HARQ Id
  value in the ramge 0-15
  ----------------------------------------------------------------*/
  uint8     max_number_of_retrans;

}l1_eul_harq_profile_type;


typedef struct
{
  /*----------------------------------------------------------------
  Chosen e-tfci for this tti. Value in the range 0-127. 0xFF indicates
  no TFCI was selected
  ----------------------------------------------------------------*/
  uint32                     etfci;

  /*----------------------------------------------------------------
  Scheduled number of bits corresponding to the scheduled payload
  that was transmitted in this TTI
  Case 1 if SG was 0 then Sched_num_bits would be set to 0.
  Case 2 If SG > 0 but was not enough to build
  even one MAC-d pdu from the highest priority logical channel, then
  MAC will pretend that it sent the scheduling grant number of bits
  ----------------------------------------------------------------*/
  uint32                     sched_num_bits;

  /*----------------------------------------------------------------
  In case of a e-tfci was chosen, then the harq_profile gives
  the power offset and the max number of retransmissions allowed
  using this HARQ id.
  ----------------------------------------------------------------*/
  l1_eul_harq_profile_type  harq_profile;

}l1_eul_etfci_info_type;

typedef struct
{
  /*----------------------------------------------------------------
  TRUE indicates UE was happy with the grant.
  ----------------------------------------------------------------*/
  boolean                   happy_bit_indicator;


  /*----------------------------------------------------------------
  TRUE indicates Secondary carrier was happy with the grant.
  ----------------------------------------------------------------*/
  boolean                   sec_carr_happy_bit_indicator;

  #ifdef FEATURE_WCDMA_MODEM_TPUT_STATS_API 
  /*----------------------------------------------------------------
  TRUE when UE is buffer restricted or Grant restricted with 
  happy bit as TRUE
  ----------------------------------------------------------------*/
  boolean                   low_bo_flg_ind;
  #endif /* FEATURE_WCDMA_MODEM_TPUT_STATS_API  */
  /*----------------------------------------------------------------
  Information regarding the chosen TFCI adn the corresponsding
  harq profile.
  ----------------------------------------------------------------*/
  l1_eul_etfci_info_type    eul_etfci_info;

  #if defined(FEATURE_WCDMA_PLT) || defined(FEATURE_WCDMA_PLT_MODE)
  #error code not present
#endif /*  #if defined(FEATURE_WCDMA_PLT) || defined(FEATURE_WCDMA_PLT_MODE) */

} l1_eul_edch_control_info_type;

typedef struct
{
  /* num of bits in unit */
  uint32 data_unit_len;

  /* pointer to data payload */
  uint32* data_unit_ptr;

  /* Is this unit the last unit in this TTI or not */
  boolean last_unit_of_tti;

  /* cipher enabled for this unit or not */
  boolean cipher_enabled;

  /* Below control parameters are valid only if cipher_enabled=TRUE */

  /* L1 will subtract 1 for HW */
  uint32 radio_bearer_id;

  /* cipher key index */
  uint32 cipher_key_index;

  /* offset in bits to start ciphering */
  uint32 cipher_offset;

  /* counter for ciphering */
  uint32 cipher_count_c;

} l1_eul_edch_data_unit_struct_type;

/* Maximum possible number of E-TFC's in a E-TFC table */
#define L1_UPA_UL_MAX_NUM_ETFC_PER_TAB  128

/* 2 E-TFC tables for 10 ms TTI and 2 ms TTI respectively */
#define L1_EUL_NUM_ETFC_TAB_PER_TTI  2
#ifdef FEATURE_WCDMA_16_QAM
#define L1_EUL_NUM_ETFC_TAB_FOR_2MS  4
#else
#define L1_EUL_NUM_ETFC_TAB_FOR_2MS  L1_EUL_NUM_ETFC_TAB_PER_TTI
#endif /* FEATURE_WCDMA_16_QAM */

/* struct type for one E-TFC table */
typedef struct
{
  /* Number of E-TFC in this table, up to L1_UPA_UL_MAX_NUM_ETFC_PER_TAB */
  uint32  num_etfc;

  /* E-TFC table of TB size */
  uint16  etfc_tb_size_table[L1_UPA_UL_MAX_NUM_ETFC_PER_TAB];
} l1_upa_ul_etfc_table_struct_type;

/*struct type for power offset req*/
typedef struct
{

  /*  Indicates whether the structure info is for Primary or Sec UL carr  */
  l1_eul_carr_idx_enum_type            l1_eul_carr_idx;
  /*DC UPA active Flag*/
  boolean  l1_dcupa_active;                            
  
  /*  Indicates the harq id used for the upcoming tx  */
  uint8     harq_id;

  /*----------------------------------------------------------------
  Indicates if the serving grant was 0 or not
  ----------------------------------------------------------------*/
  boolean   sg_available;

  /*----------------------------------------------------------------
  Identifies the number of HARQ processes that are active.
  MAC will need this for triggering scheduling information
  ----------------------------------------------------------------*/
  uint8     num_harq_active;

  /*----------------------------------------------------------------
  Bitmask value of the active HARQ process.
  The bit position in the active_harq_bitmask corresponds to the active HARQ process
  ----------------------------------------------------------------*/
  uint8      active_harq_bit_mask;

  /*----------------------------------------------------------------
  This identifies the state of the HARQ process used for the
  upcoming transmission.
  ----------------------------------------------------------------*/
  boolean   per_harq_active;

  /* TRUE indicates that the NACK was received from the serving RLS
     for max tx times      */
   boolean   nack_recvd_srv_rls_max_tx_times;

  /* SRB0~SRB4 data should not be transmitted during SRB delay */
  boolean   srb_omit;

  /*----------------------------------------------------------------
  False indicates it is a re transmission and TRUE indicates it is a
  new transmission.
  ----------------------------------------------------------------*/
  boolean new_tx;
  
  /* Flag to indicate DTX for the TTI because of CM, MAC DTX cycle,
  ** recfg*/
  boolean dtx_flag;
    
} l1_eul_pwr_offset_req_struct_type;

/* 10 ms TTI E-TFC tables [Table_Index] */
extern const l1_upa_ul_etfc_table_struct_type  l1_upa_10ms_ul_etfc_tabs[L1_EUL_NUM_ETFC_TAB_PER_TTI];

/* 2 ms TTI E-TFC tables [Table_Index] */
extern const l1_upa_ul_etfc_table_struct_type  l1_upa_2ms_ul_etfc_tabs[L1_EUL_NUM_ETFC_TAB_FOR_2MS];


typedef PACKED struct PACKED_POST {
     /*NUM_POW_LIMITED:Number of TTIs transmission is limited by power*/
 uint8 num_tx_limited_by_pwr;

 /*NUM_SG_LIMITED:Number of TTIs transmission is limited by serving grant*/
 uint8 num_tx_limited_by_sg;

 /*NUM_BUF_LIMITED:Number of TTIs transmission is limited by available buffer
   status*/
 uint8 num_buf_limited;

 /*NUM_BUF_EMPTY:Number of TTIs both sched + non-sched buffers were empty*/
 uint8 num_buf_empty;
 
 /* NUM_S_BUF_EMPTY:Number of TTIs total scheduled buffer was empty*/
 uint8 num_sched_buf_empty;

 /*NUM_NS_BUF_EMPTY:Number of TTIs total non-scheduled buffer was empty*/
 uint8 num_non_sched_buf_empty;

 /*SUM_S_BUF:SUM of schedule buffer bytes status for newtx  divided by 64 is sent
   to diag and Mean scheduled buffer status in bytes (value of 0xFFFF means 
   more than 65535 bytes) is displayed
 */
 uint16 sum_sched_buf_bytes;

 /*SUM_NS_BUF:SUM of non schedule buffer bytes status for newtx  divided by 64 is 
   sent to diag and Mean non-scheduled buffer status in bytes (total over all NS flows)
   is displayed*/
 uint16 sum_non_sched_buf_bytes;

 /* NUM_SI:Number of SIs sent */
 uint8  num_si_sent;
}eul_log_l1_mac_stats_mac_info_struct_type;

/*----------------------------------------------------------------------------
This structure stores the values needed for logging the 0x4311 Log packet.
The accumulated log packet is logged at L1. MAC provides all the following
information to L1.
----------------------------------------------------------------------------*/

typedef struct
{
/*MAC_RESETS    1   Number of MAC-e resets*/
uint8    num_mac_e_resets;

/*Sum of the number of transmitted scheduled bits per TTI, divided by 64, counting only new transmissions*/
uint16    sum_sched_bits_txed;

/*Sum of the number of transmitted non-scheduled bits per TTI, divided by 64, counting only new transmissions*/
uint16    sum_non_sched_bits_txed;

/*Sum of the number of bits that could have been transmitted per TTI based on available buffer status (scheduled flows only) divided by 64,
counting only new transmissions NOTE While summing up over all TTIs: For all TTIs for which the buffer status is greater than the max TB size,
use the max TB size instead of the actual buffer status in the summation.*/
uint16   sum_buf_bits;

/*mac continuous data as in the 4311 logpkt*/
eul_log_l1_mac_stats_mac_info_struct_type continuous_mac_info;

}l1_eul_mac_stats_struct_type;

/* Bit mask to indicate to MAC when the number of bits supported by SG is greater than
** that supported by MAX_CFG*/
#define L1_SGP_OVERFLOW_IND_BMSK 0x1
/* Bit mask to indicate to MAC when the number of bits supported by MP (Power) is greater than
** that supported by MAX_CFG*/
#define L1_MP_OVERFLOW_IND_BMSK 0x2

/* Macros to access important l1_ux_data_type elements */
/* Ciphering flag */
#define L1_TX_BUF_CIPH_FLAG(trch_ptr, tb_id) \
        ((trch_ptr)->trch_data[tb_id].ciphering_on)
/* MAC header size */
#define L1_TX_BUF_MAC_HDR_SIZE(trch_ptr, tb_id) \
        ((trch_ptr)->trch_data[tb_id].mac_hdr_size)
/* MAC header data pointer */
#define L1_TX_BUF_MAC_HDR_DATA(trch_ptr, tb_id, offset) \
        (*(uint32 *)&((trch_ptr)->trch_data[tb_id].mac_hdr[offset << 2]))

/* MAC DSM data pointer */
#define L1_TX_BUF_DSM_DATA_PTR(trch_ptr, tb_id) \
        ((trch_ptr->trch_data[tb_id].data_ptr))
/* MAC DSM data */
#define L1_TX_BUF_DSM_DATA(dsm_item_ptr, offset) \
        (*(uint32 *)&(dsm_item_ptr->data_ptr[offset]))

#ifdef FEATURE_WCDMA_CPC_DTX


/*  enum to indicate the different cases in for the return value
** for L1 mac data query function*/
typedef enum {
  /* mac has no data in its buffers*/
  L1_MAC_DATA_QUERY_NO_DATA,
  /* mac has non-scheduled data*/
  L1_MAC_DATA_QUERY_NON_SCHED_DATA,
  /* mac has SI data*/
  L1_MAC_DATA_QUERY_SI_DATA,
  /* MAC has scheduled data*/
  L1_MAC_DATA_QUERY_SCHED_DATA,
  /* MAC has scheduled data but low grant timer is active*/
  L1_MAC_DATA_QUERY_SCHED_DATA_LG_TIMER_ACTIVE,
  /* MAC has  scheduled data & low grant timer is active & timer expiry*/
  L1_MAC_DATA_QUERY_SCHED_DATA_LG_TIMER_EXPIRY
}l1_mac_data_query_enum_type;


#endif /* FEATURE_WCDMA_CPC_DTX */

/* MDSP return values corresponding these enums. So, the order is important */
typedef enum
{
  L1_PRACH_NO_ACK,
  L1_PRACH_MSG_DONE, /* RACH MSG TX done */
  L1_PRACH_NAK,
  L1_PRACH_AICH_ACK, /* RACH MSG yet to TX */
  L1_PRACH_ABORT,
  L1_PRACH_DELAY,
  L1_PRACH_MAC_INVALID
} l1_prach_status_enum_type;

#ifdef FEATURE_WCDMA_HS_RACH
typedef enum
{
  L1_HSRACH_NO_ACK,
  L1_HSRACH_NACK,
  L1_HSRACH_ACK,
  L1_HSRACH_ABORT,
  L1_HSRACH_MAC_INVALID
}l1_hsrach_status_enum_type;
#endif /*FEATURE_WCDMA_HS_RACH*/
/*===========================================================================

                      DOWNLINK FUNCTIONS

===========================================================================*/

/*===========================================================================
FUNCTION l1_phy_dl_trblk_header_ind

DESCRIPTION
  This function exists in the L2 and is used by L1 to update the TrBlk header
  information to MAC/RLC and get the TrBlk data offset and ciphering parameters
  (if the TrBlks are ciphered).

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  The members of the structure pointed in the function argument, get the
  update to the values after L2 decodes the header information.
===========================================================================*/

extern void l1_phy_dl_trblk_header_ind
(
  l1_dl_tb_hdr_decode_struct_type *tb_hdr_decode_info 
    /* TrBlk header information to get it decoded */
);

#if defined(FEATURE_WCDMA_PLT) || defined(FEATURE_WCDMA_PLT_MODE)
#error code not present
#endif  /* defined(FEATURE_WCDMA_PLT) || defined(FEATURE_WCDMA_PLT_MODE) */

/*===========================================================================
FUNCTION l1_phy_dl_trblk_data_ind

DESCRIPTION
  This function exists in the L2 and is used by L1 to update the TrBlk data
  information to MAC/RLC.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void l1_phy_dl_trblk_data_ind
(
  l1_dl_tb_data_struct_type *tb_data_info /* TrBlk data information */
);

/*============================================================================================
 FUNCTION NAME  l1_phy_dl_flush_tsn

 DESCRIPTION  This function will flush the TSNs based on L1 request

 CONTEXT    Called by PHY layer in interrupt context
=============================================================================================*/
extern void l1_phy_dl_flush_tsn(void);

#ifdef FEATURE_WCDMA_DL_MODEM_ACC
/*===========================================================================
FUNCTION l1_phy_dl_trblk_hs_data_ind

DESCRIPTION
  This function exists in the L2 and is used by L1 to update the HSDPA 
  TrBlk data information to MAC/RLC.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void l1_phy_dl_trblk_hs_data_ind(l1_dl_hs_tti_enum_type tti_type);

#if defined(FEATURE_WCDMA_PLT) || defined(FEATURE_WCDMA_PLT_MODE)
#error code not present
#endif /* defined(FEATURE_WCDMA_PLT) || defined(FEATURE_WCDMA_PLT_MODE) */

/*===========================================================================
FUNCTION  l1_dl_phy_hspa_gather_and_decipher

DESCRIPTION
  This function is invoked by Mac-hs in its task context to perform
  P-to-C operation. MAC provides a list of P-to-Cs and the IOVEC information
  which are passed to A2 driver by L1.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void l1_dl_phy_hspa_gather_and_decipher
(
  /* P-to-C information for each partial-to-complete PDU */
  mcalwcdma_dec_rlc_pdu_concatenate_info_struct_type *mac_p_to_c_ptr,
  /* IOVEC information for each partial PDU */
  a2_iovec_t a2_dec_src_iovec_list[]
);

#endif /* FEATURE_WCDMA_DL_MODEM_ACC */

/*===========================================================================
FUNCTION l1_phy_sfn_decode_ind

DESCRIPTION
  This function exists in the L3 (RRC) and is used to get the SFN decoded
  for the neighbor BCH TrBlk.

DEPENDENCIES
  None

RETURN VALUE
  Decoded SFN for the BCH TrBlk.

SIDE EFFECTS
  None
===========================================================================*/

extern word l1_phy_sfn_decode_ind
(
  byte *bch_trblk   /* TrBlk data information */
);

/*===========================================================================

                      UPLINK FUNCTIONS

===========================================================================*/

#ifdef FEATURE_UMTS_CIPHERING_ENABLE
/*==============================================================================================
 FUNCTION NAME  void mac_update_hfn_on_rollover

 DESCRIPTION : This function is called 2 times by layer 1. Once at the time when Layer 1 is
 suspended and there is a W->G handover. This function is called again when Layer 1 is 
 resumed (G->W this may happen if the reconfiguration fails or...).  
 
 cfn : indicates the current value of the CFN
 num_roll_overs : The number of roll-overs that occureed during the time L1 was suspended
 to the time the Layer 1 resumed .
 
 If the reconfiguration took 
   a) < 2.5 s ==> the variable num_roll_overs = 0
   b) 2.5s <= reconfig_time < 5.12s ===> num_roll_overs = 1
      ....
MAC based on the number of roll overs that ocurred would increment
its hfn count.

===============================================================================================*/
void mac_update_hfn_on_rollover
(
  uint8 cfn, 
  uint8 num_roll_overs,
  l1_mac_suspend_resume_enum_type l1_suspend_state
);
#endif

/*===========================================================================
FUNCTION l1_phy_ul_build_frame_ind

DESCRIPTION
  This function is defined by L2 and builds Tx data for the specified TTI and
  action time. The data is filled by L2 in a transmnit buffer owned by L1.

DEPENDENCIES
  None

RETURN VALUE
  TRUE  Build Frame indication accepted by MAC
  FALSE Build Frame indication rejected by MAC

SIDE EFFECTS
  None
===========================================================================*/
extern boolean l1_phy_ul_build_frame_ind
(
    /* current tti */
  l1_tti_enum_type     tti,        
    /* next next frame's CFN */
  uint8                cfn,        

#ifdef FEATURE_UMTS_CIPHERING_ENABLE
  boolean        ul_tx_disabled,     
#endif

  /* omit SRB data for specified delay frames */
  boolean              SRB_omit,

  /* array of TFCI states for power based TFC elimination
   * array size = L1_UL_TFC_MAX
   * TRUE means Excess_Power state (maybe Blocked state for MAC)
   * FALSE means SUPPORTED state
   */
  const boolean  *tfc_excs_pwr_state_ptr,


  /* TFCS restriction check call back function pointer */
  L1_UL_QUERY_TFCS_RESTRICT_CB_FUNC_TYPE *cb_func,
  
    /* pointer to Tx data buffer */
  l1_ul_tx_data_type  *tx_buf_ptr  
);

#if defined(FEATURE_WCDMA_PLT) || defined(FEATURE_WCDMA_PLT_MODE)
#error code not present
#endif /*defined(FEATURE_WCDMA_PLT) || defined(FEATURE_WCDMA_PLT_MODE) */

/*===========================================================================
FUNCTION l1_phy_ul_trch_status_ind

DESCRIPTION
  This function is defined by L2 and processes the transmit status of a
  PRACH transmission.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void l1_phy_ul_trch_status_ind
(
    /* PRACH transmit status as informed by mDSP */
  l1_prach_status_enum_type   status  
);

/*===========================================================================
FUNCTION  L1_PHY_UL_CFN_MISSED_IND

DESCRIPTION
  This function notifies MAC that CFN update interrupt was missed due to
  long period of WCDMA_INTLOCK().

DEPENDENCIES
  DCH state when ENC detects missed CFN update interrupt.

RETURN VALUE
  None

SIDE EFFECTS
  MAC should sync-up or reset.
===========================================================================*/
extern void l1_phy_ul_cfn_missed_ind
(
  uint8 curr_cfn  
);

/*==========================================================================
FUNCTION: MAC_SET_ETFCI_THROUGH_QXDM

DESCRIPTION:
  This function gives the flexibility to set the value of ETFCI to a
  fixed value through QXDM

DEPENDENCIES:
  Validation for val is done in L1.

RETURN VALUE:
  None

SIDE EFFECTS:
  Normal functioning of ETFCI selection will not happen, instead 
  the fixed value set through QXDM will be used every TTI even 
  if no data exists in the buffer. Padding and Truncation 
  will happen as normal as part of frame building.
==========================================================================*/

void mac_hs_ul_set_etfci_through_QXDM
(
  uint8 val
);

/*===========================================================================
FUNCTION  L1_EDCH_QUERY_MAC_PWR_OFFSET_INFO

DESCRIPTION
This function will return the power offset corresponding to the highest
priority logical channel carrying data mapped onto non-scheduled MAC-d flows
incase when SG_available is FALSE and the PO of  overall highest priority
logical channel carrying data if SG_Available is TRUE

When the CPC DTX feature is defined, an extra variable is passed that would
indicate to MAC that the upcoming sub frame has an OTA transmission scheduled.
Then MAC code flows through their implementation of BO and HARQ PO calculation


DEPENDENCIES

Serving grant function should have been complete before calling
this function

RETURN VALUE

  Power OFfset.

  Boolean to indicate if SI was sent alone

SIDE EFFECTS

  The returned poweroffset will be used for the mapping of T/P to payload.

===========================================================================*/
extern uint8 l1_edch_query_mac_pwr_offset_info( 
  l1_eul_pwr_offset_req_struct_type* mac_pwr_offset_info
#ifdef FEATURE_WCDMA_CPC_DTX
  , boolean is_eul_data_tx_next_subfn
#endif /*FEATURE_WCDMA_CPC_DTX*/
  #ifdef FEATURE_WCDMA_CPC_DRX
  , boolean                           *is_tebs_non_zero
  #endif /*FEATURE_WCDMA_CPC_DRX*/
  , wl1_ul_carr_id_enum_type carr_idx
);

#if defined(FEATURE_WCDMA_PLT) || defined(FEATURE_WCDMA_PLT_MODE)
#error code not present
#endif /* defined(FEATURE_WCDMA_PLT) || defined(FEATURE_WCDMA_PLT_MODE) */

#if defined(FEATURE_WCDMA_CPC_DTX) 
/*===========================================================================
FUNCTION  L1_EDCH_MAC_QUERY_DATA_PRESENT

DESCRIPTION
This function returns a boolean if the data is present to be Txed.
The input parameter to this function is the HARQ ID. 
MAC Returns a value TRUE if:
1. There is non-zero scheduled data.
2. There is only Non Scheduled data and the HARQ ID passed
    will allow the Tx of the Non Scheduled data.

Otherwise this returns FALSE.

L1 will call this function only when CPC DTX feature is enabled
and CPC is configured and in Cycle 1 or 2. If for some reason
L1 cannot do any transmission, because of MAC DTX cycle, then
this API WILL NOT be Called.

DEPENDENCIES
This function is called only when the CPC DTX feature is defined.
The functions to compute and get a sub FN value which could translate
to the OTA Tx should have been done by now.

RETURN VALUE

Boolean to indicate whether MAC has data to Tx or not.

Boolean to indicate if SI was sent alone

SIDE EFFECTS

Indicates to L1 whether MAC has data to Tx.

===========================================================================*/
#ifdef FEATURE_WCDMA_PLT
 #error code not present
#else


extern l1_mac_data_query_enum_type l1_edch_mac_query_data_present
(
  /*  Indicates whether the structure info is for Primary or Sec UL carr  */
  l1_eul_carr_idx_enum_type            l1_eul_carr_idx_info,
  
  uint8 edch_tx_harq_id,
  
  /* Indicates the number of active HARQ processes. For 2ms 8 harq and for 10ms
  4 harq's will be configured */
  boolean         num_active_harq_processes,

  /*----------------------------------------------------------------
  The value as number of bits indicates the maximum payload that can
  carry traffic from scheduled MAC-d flows. Values in range 0-20000
  0 indicates no serving grant is available
  ----------------------------------------------------------------*/
  uint16          *sg_payload_bits,

  /*----------------------------------------------------------------
  The value as E-TFCI indicates the maximum payload that can carry
  traffic from both scheduled, non-scheduled MAC-d flows. This also
  includes the Scheduling information. Values in range 0-127
  0xFF indicates no power is available to transmit any E-DCH data
  ----------------------------------------------------------------*/
  uint8           *ue_tx_pwr_max_payload_idx,
  
  /* Is the OTA for which we are requesting data query active or not*/
  boolean      is_ota_harq_active,
  /* Is L1 in SRB OMIT stage*/
  boolean      srb_omit
);

extern void mac_cpc_dtx_hs_ul_maci_log_packet_update(uint32 harq_id
, boolean sec_carr_active
);
/*===========================================================================
FUNCTION    L1_EDCH_REQ_LOW_GRANT_TIMER_STATE

DESCRIPTION
L1 calls the function to get the status of LOW grant timer is active or not

DEPENDENCIES

RETURN VALUE
  TRUE or FASE (If Low Grant Timer is active MAC will return TRUE)

SIDE EFFECTS

===========================================================================*/

extern  boolean l1_edch_req_low_grant_timer_state(l1_eul_carr_idx_enum_type  l1_eul_carr_idx);


#endif /* FEATURE WCDMA_PLT*/
#endif /*FEATURE_WCDMA_CPC_DTX*/


/*===========================================================================
FUNCTION    L1_EDCH_REQ_CONTROL_INFO

DESCRIPTION
L1 calls the function with serving grant information as the input parameter.
MAC will perform the E-TFCI selection based on this information.


DEPENDENCIES

Mapping of T/P to payload should have been done prior to this function call.

RETURN VALUE
  Chosen E-TFCI and the HARQ profile information

SIDE EFFECTS

===========================================================================*/
extern l1_eul_edch_control_info_type* l1_edch_req_control_info
(
  l1_eul_serving_grant_update_info_type *sg_info_type
);

#if defined(FEATURE_WCDMA_PLT) || defined(FEATURE_WCDMA_PLT_MODE)
#error code not present
#endif /* defined(FEATURE_WCDMA_PLT) || defined(FEATURE_WCDMA_PLT_MODE) */

/*===========================================================================
FUNCTION        L1_EDCH_SERVING_CELL_CHANGE_IND

DESCRIPTION
This function is called by L1 after L1 gets reconfigured due to an Active
set update command. TRUE indicates that serving cell has changed.

DEPENDENCIES

This function will be called only if the serving cell change is detected
by L1 based on the ASET message recvd frm the nw.

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
extern void l1_edch_serving_cell_change_ind
(
  boolean   srv_cell_changed
);

/*===========================================================================
FUNCTION  L1_EUL_GET_SRV_CELL_CHANGE_IND

DESCRIPTION
  This function is called by MAC and is needed for SI triggering

  Quote from spec
    11.8.1.6.2 Report Triggering when SG <> 'Zero_Grant' and at least one 
               process is activated

      If an E-DCH serving cell change occurs and if the new E-DCH serving 
      cell was not part of the previous Serving E-DCH RLS, the transmission 
      of a Scheduling Information shall be triggered.

 In the following scenarios L1 returns TRUE

 1: Simple SHO/Serving Cell Change with Change of Serving RLS
 2: HHO: Irrespective of the RGComb Index for the previous and the new Serving 
    Cell (even if they are the same) SI should be triggered
 3: SHO/SCC when SRGCH is not configured before or after the Serving Cell 
    Change:
      This is the E// configuration <this is the case that has been discussed
      in this thread  look for Anils response>
 4: SHO/SCC where NS_RGCH is de-configured but RG Combination index is re-used
    for S_RGCH (we don't expect to see this practically, but our 
    implementation should address this):


  NOTE: (4) above is not TRUE currently because of the complications involved.
 
DEPENDENCIES

RETURN VALUE
  Boolean TRUE indicates that serving cell change occurred

SIDE EFFECTS
  none
===========================================================================*/
extern void l1_mac_eul_serv_cell_and_rls_change_ind
(
  boolean eul_serv_cell_and_rls_change 
);

/*===========================================================================
FUNCTION        L1_EDCH_WRITE_ERAM_DATA_UNIT

DESCRIPTION
  After L1 calls l1_phy_edch_build_frame_ind(), MAC and RLC will call this
  function to write TX data to EUL ERAM bank in HW.

DEPENDENCIES
  After L1 calls l1_phy_edch_build_frame_ind(), MAC and RLC will call this
  function to write TX data to EUL ERAM bank in HW.
  The first call of this function in a TTI must be from MAC for MAC-es header
  only so that L1 can save the MAC-es header len.
  The last call of this function in a TTI must set the last cipher unit bit.

RETURN VALUE
  None

SIDE EFFECTS
  E-DCH TX data will be writen to HW
===========================================================================*/
/*
 *  REV1: L1 will concatenate all units into one E-DCH TB on the fly with
 *        book keeping of unit ending bit position
 *  REV2: write directly to HW
 */
void l1_edch_write_eram_data_unit
(
  l1_eul_edch_data_unit_struct_type *edch_data_unit_ptr
);

/*===========================================================================
FUNCTION        L1_PHY_EDCH_BUILD_FRAME_IND

DESCRIPTION
  This function is called by L1 to trigger MAC/RLC to build E-DCH MAC-e PDU.

DEPENDENCIES
  This function is called by L1 to trigger MAC/RLC to build E-DCH MAC-e PDU.

RETURN VALUE
  none

SIDE EFFECTS
  E-DCH TX data will be writen to HW via L1 wrappers during this function
===========================================================================*/
void l1_phy_edch_build_frame_ind
(
  l1_eul_carr_idx_enum_type    l1_eul_carr_idx_info,
  uint32 harq_id,
  uint32 etfci,
  uint32 frame_number, // cfn for 10 ms TTI; cfn*5+sub_frame_number for 2 ms TTI
  uint32 uph, // UE power headroom value
  l1_l2_data_type *data_buf
);


/*===========================================================================
FUNCTION  L1_EUL_GET_ETFC_TAB_PTR

DESCRIPTION
  This function returns the pointer to the E-TFC table for input TTI and table
  index

DEPENDENCIES
  E-DCH TTI and E-TFC table index must have been validated.

RETURN VALUE
  pointer to the E-TFC table

SIDE EFFECTS
  none
===========================================================================*/
const l1_upa_ul_etfc_table_struct_type* l1_eul_get_etfc_tab_ptr
(
  e_tti_enum_type tti, /* E-DCH TTI */
  uint32 e_tfci_table_idx
);


/*===========================================================================
FUNCTION        L1_PHY_EDCH_GET_MAC_HS_UL_MAC_D_SI_LOG_INFO

DESCRIPTION

This function is called by L1 whenever they need to send the status log packet
to diag services.

DEPENDENCIES



RETURN VALUE
MAC returns the following which is populated in the L1 structure
Log packet 4309
mac-d present   - For each Mac-d flow (max 8) whether it was present in this TTI or not.
   For example, bit 0 (LSB) indicates whether MAC-d flow 0 is present in this transport block.
0: this Mac-d flow is absent
1: this Mac-d flow was present
Si_present  - Indicates if scheduling information was present  in this TTI

Returns SI present field.

SIDE EFFECTS

===========================================================================*/
extern uint8 l1_phy_edch_get_mac_hs_ul_mac_d_si_log_info
(
  uint8     *mux_list,
  uint8     *etfci_reason
);

/*===========================================================================
FUNCTION        L1_PHY_EDCH_GET_MAC_HS_UL_MAC_STATS_INFO

DESCRIPTION

This function is called by L1 whenever they need to send the status log packet 0x4311
to diag services.

DEPENDENCIES


RETURN VALUE


SIDE EFFECTS

===========================================================================*/

void l1_phy_edch_get_mac_hs_ul_mac_stats_info
(
    l1_eul_mac_stats_struct_type*  l1_mac_stats_accum
);

/*==========================================================================
FUNCTION: L1_EDCH_UPDATE_MAC_LOG_PACKETS_DURING_RECONFIG

DESCRIPTION:
This function will be called by L1 whenever there is a reconfig
happening. In seam less reconfig scenarios MAC does not get configured
and the build frame also doesnt come for certain frames.
For logging purposes MAC needs to know that there has been a gap in the
accumulation of log packets. Hence now whenever log code 0x4309 gets flushed
L1 will call MAC function so that the MAC log packets also gets flushed

DEPENDENCIES:
None

RETURN VALUE:
None

SIDE EFFECTS:
The log packet will be flushed to diag. There may be less than 20 samples

==========================================================================*/

extern void l1_edch_update_mac_log_packets_during_reconfig(void);

/*===========================================================================
FUNCTION      L1_GET_CUR_EUL_SUBFN
 
DESCRIPTION   This function returns the value of the OTA EUL sub fn number
 
DEPENDENCIES  None
 
RETURN VALUE  eul_fig current subfn value if eul is active and TTI is 2msec
              else returns invalid subfn
 
SIDE EFFECTS  None
===========================================================================*/
extern uint16 l1_get_cur_eul_subfn(void);

/*==========================================================================
FUNCTION: L1_EDCH_RESET_MAC_TSN

DESCRIPTION:
L1 Indicate to the mac through a function call at the activation time even
before processing the cphy_setup req (when mac-es/e reset indicator present)
so that mac will be ready with new TSNs . But this has disadvantage that
the cphy_setup is not processed at L1 and mac will be resetting the TSNs
and will have old cfg till L1 signals the mac to process the new cfg. And
this also involves design change.


The various scenarios and how the UE will handle it are listed below.
In all the following scenarios the MAC_E_RESET_INDICATOR is set to TRUE

++ No config change in MAC or L1
Reset TSN's and build frame can go on as usual


++ MAC config change, L1 has NO change
Reset the TSN's (option 1)
MAC will determine if config has changed
MAC will DTX until it received the signal from L1 to apply new config
L1 will continue build frame indication


++ MAC no config change, L1 has config change
Reset TSN's (option 1)
L1 will not send Build Frame indication


++ MAC config changes, L1 config also changes
 Reset TSN
L1 will not call Build Frame indication

DEPENDENCIES:
  None

RETURN VALUE:
  TRUE indicates that mac config has changed and FALSE indicates
  that no MAC config has really changed.
  based on this L1 can evaluate if it needs to suspend the BFI
  during this time frame.

SIDE EFFECTS:


==========================================================================*/
extern boolean l1_edch_reset_mac_tsn(void);

/*===========================================================================
FUNCTION wl1_rxd_fach_signalling_data_available
         
DESCRIPTION
  API called by MAC to inform RxD that signalling data is ready for Tx

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void wl1_rxd_fach_signalling_data_available(boolean status);

/*==========================================================================
FUNCTION: L1_QUERY_MAC_RACH_DATA

DESCRIPTION:
This function will be called by L1 to check if L2 has data to send. It will
internally invoke rlc_buffers_are_not_empty() to check RLC Buffer
Occupancy. 

DEPENDENCIES:
None

RETURN VALUE:
TRUE if the buffers are not empty and MAC is not in IDLE state. FALSE otherwise.

SIDE EFFECTS:
==========================================================================*/
extern boolean l1_query_mac_rach_data (void);

extern boolean wplt_l1_query_mac_rach_data(void);

#ifdef FEATURE_WCDMA_HS_RACH

extern void l1_mac_hsrach_get_mac_config_params(l1_mac_hs_rach_config_params *config_params);

/*===========================================================================
FUNCTION	   L1_MAC_HSRACH_EAGCH_DECODE_IND

DESCRIPTION   This function is called from EUL ISR on successful decode
                     of AGCH and passes TRUE as a parameter. 

DEPENDENCIES  None

RETURN VALUE   None

SIDE EFFECTS  None
===========================================================================*/
extern void l1_mac_hsrach_eagch_decode_ind(boolean agch_decode_status);

/*===========================================================================
FUNCTION	   L1_MAC_HSRACH_UPD_VALID_TIMER_STATUS

DESCRIPTION   This function is called by the EUL ISR and the
             intention is to get the status of the HS RACH EUL related
             timers from MAC. The EUL ISR will flow as per these timers.
             
DEPENDENCIES  None

RETURN VALUE   None

SIDE EFFECTS  None
===========================================================================*/
extern void l1_mac_hsrach_upd_valid_timer_status(l1_mac_hs_rach_eul_timers_status_struct_type *timer_status);


/*===========================================================================
FUNCTION	   L1_MAC_HSRACH_EDCH_RESRC_REL_IND

DESCRIPTION   This function is called when the EDCH resource is 
             released. The cause is also sent to MAC. MAC should use the
             cause to find out if the release is because of RL Failure or
             phychan establishment failure and trigger the Timers.
             
DEPENDENCIES  None

RETURN VALUE   None

SIDE EFFECTS  None
===========================================================================*/
extern void l1_mac_hsrach_edch_resrc_rel_ind(l1_mac_hs_rach_edch_rsrc_rel_cause_enum_type rel_cause);

/*===========================================================================
FUNCTION	   L1_MAC_HSRACH_CELL_RESEL_IND

DESCRIPTION   This function is called when there is a cell reselection.
             The mac needs to do some specific operation based on this
             especially if the timers regarding the RL Failure or phychan est failure
             is running.
             
DEPENDENCIES  None

RETURN VALUE   None

SIDE EFFECTS  None
===========================================================================*/
extern void l1_mac_hsrach_cell_resel_ind(void);

/*===========================================================================
FUNCTION	   L1_MAC_HSRACH_FIRST_EDCH_TX_IND

DESCRIPTION   This function is called when the EUL Tx begins. This is called
             correctly in that OTA frame when the EUL Tx would have been OTA
             
DEPENDENCIES  None

RETURN VALUE   None

SIDE EFFECTS  None
===========================================================================*/
extern void l1_mac_hsrach_first_edch_tx_ind(void);

/*===========================================================================
FUNCTION      L1_GET_HS_RACH_CFN_SFN_INFO

DESCRIPTION  Returns the SFN or CFN info based on TTI provided by MAC
                    ONLY FOR HS RACH
                                
DEPENDENCIES  None

RETURN VALUE  CFN/SFN value- uint16 

SIDE EFFECTS  None
===========================================================================*/
extern uint16 l1_get_hs_rach_cfn_sfn_info(e_tti_enum_type e_tti);

/*===========================================================================
FUNCTION      L1_MAC_HSRACH_PREAMBLE_PROC_STATUS

DESCRIPTION   API for L1 to indicate to MAC the HSRACH status
                                
DEPENDENCIES  None

RETURN VALUE  None 

SIDE EFFECTS  None
===========================================================================*/
extern void l1_mac_hsrach_preamble_proc_status(l1_hsrach_status_enum_type  status);

/*===========================================================================
FUNCTION      L1_MAC_QUERY_TEBS_ZERO

DESCRIPTION   API to query MAC if TEBS is 0
                                
DEPENDENCIES  None

RETURN VALUE  TRUE - TEBS is 0
              FALSE - TEBS is non-0

SIDE EFFECTS  None
===========================================================================*/
extern boolean l1_mac_query_tebs_zero(void);

/*===========================================================================
FUNCTION      L1_MAC_HSRACH_GET_MAC_CONFIG_PARAMS

DESCRIPTION   API to get MAC configured params when HSRACH status is ACK.
              This API populates, for L1 usage, EDCH TRANS type and various timer values 
              maintained by MAC. 
                                
DEPENDENCIES  None

RETURN VALUE  None 

SIDE EFFECTS  None
===========================================================================*/
extern void l1_mac_hsrach_get_mac_config_params(l1_mac_hs_rach_config_params *config_params); 

#if defined(FEATURE_WCDMA_PLT) || defined(FEATURE_WCDMA_PLT_MODE)
#error code not present
#endif /*defined(FEATURE_WCDMA_PLT) || defined(FEATURE_WCDMA_PLT_MODE)*/

#endif /*FEATURE_WCDMA_HS_RACH*/

#ifdef FEATURE_WCDMA_DC_HSUPA 

/*===========================================================================
FUNCTION     l1_edch_query_mac_non_sg_info

DESCRIPTION  This function is called when Secondary UL is active. L1 
             queries MAC for the Non scheduled data to be tx for that TTI
             and also the corresponding HARQ PO.
             
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/

extern  l1_eul_pwr_offset_sec_ul_non_sg_struct_type l1_edch_query_mac_non_sg_info(uint8  harq_id);

/*===========================================================================
FUNCTION       EULENC_IS_DCHSUPA_SEC_CARR_ACTIVE

DESCRIPTION    This is an API to check if secondary carrier in DCHSUPA is active.
  
DEPENDENCIES   None
 
RETURN VALUE   Returns TRUE or FALSE depending on carrier state.

SIDE EFFECTS   None
===========================================================================*/
extern boolean eulenc_is_dchsupa_sec_carr_active(void);

#endif /*FEATURE_WCDMA_DC_HSUPA */

#endif /* L1_MAC_DATA_H */
