#ifndef TDSL1MACIF_H
#define TDSL1MACIF_H

/*============================================================================
                            L1 MAC INTERFACE 


GENERAL DESCRIPTION
  This file contains the declarations for the command types that are
  received by L1 from MAC and contains declarations for the indications
  and confirmations sent by L1 to MAC.

Copyright (c) 2000 - 2010 by Qualcomm Technologies Incorporated.  All Rights Reserved.

============================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/l1/vcs/l1macif.h_v   1.1   12 Jul 2001 11:43:34   mlevin  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/tdscdma/l1/inc/tdsl1macif.h#1 $ $DateTime: 2016/12/13 07:58:19 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     -------------------------------------------------------- 
07/16/10    ksr     A2 EUL Enhancements
07/12/10    asm     Fixed Compiler Warnings
02/26/10    rvs     Fixes for Mustang.
12/03/09    sup     Fixed compilation error
12/03/09    vc      Fixed the compilation error.
11/25/09    rmsd    Replaced __inline with INLINE.
11/16/09    asm     Added EUL Datapath support with A2 and Removed P2-related changes
08/28/09    gv      Modified the type of a variable. Part of EUL PP checkin.
06/26/08    av      l1-l2 interface cleanup 
03/10/09    asm     Moving inline function definitions to l1sapcommon.c and 
                    externing here, to pacify GCC compiler
01/30/09    mc      Added API to get free TSN from queue.
09/19/07    asm     Added interface for MAC to query limitations for ERAM buffer size,
                    max number of transport blocks and max transport block size.
08/03/05    asm     Added interface to support dynamic rate-change in multirate 
                    AMR
07/13/05    vp      Code changes to support the new MAC-CFN update mechanism.
05/05/05    asm     Added interface for MAC to query UL for sub-channel availability
05/03/05    vb      Added func prototype for tdsl1_suspend_ind and tdsl1_resume_ind
03/31/05    yiz     Add decl of tdsmac_update_ciphering_params
02/26/05    gs      Added function declaration for
                    tdsl1_mac_hs_query_mac_hs_reset_reqd
08/21/02    scm     Activation time related comment change.
08/07/02    scm     Define l1_mac_config_cnf_type for activation time support.
11/22/00    sh      Ported MAC related structures from l1rrcctrl.h

===========================================================================*/

/*===========================================================================

                      INCLUDE FILES

===========================================================================*/

#include "tdscdma_variation.h"
#include "comdef.h"
#include "tdsl1sapcommon.h"
#include "customer.h"
#include "tdsuecomdef.h"
#include "err.h"
#include "msg.h"

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/
/*--------------------------------------------------------------------------
                         UL TX CONTROL REQ COMMAND

Used by MAC in support for RACH or CPCH.
--------------------------------------------------------------------------*/
/* Uplink Physical Channel Control */
typedef enum
{
  TDSL1UL_PHYCHAN_ENABLE,
  TDSL1UL_PHYCHAN_DISABLE
} tdsl1_ul_phychan_ctrl_type;

typedef struct
{
  /* physical channel type - PRACH or PCPCH */
  tdsl1_ul_phychan_enum_type chan;
  /* specifies if physical channel is to be enabled or disabled */
  tdsl1_ul_phychan_ctrl_type action;

} tdsl1_ul_tx_ctrl_req_type;

/*--------------------------------------------------------------------------
      TDSL1_CPHY_MAC_UL_CONFIG_CNF and TDSL1_CPHY_MAC_DL_CONFIG_CNF

Used by MAC in support of activation times.  L1 signals MAC at the
activation time to process config requests from RRC.  Once MAC finishes,
it responds with this message, which includes a completion status.
Used for both uplink and downlink completions.
--------------------------------------------------------------------------*/
typedef struct
{
  /* Status returned from MAC layer.  TRUE means success. */
  boolean mac_config_status;
} tdsl1_mac_config_cnf_type;


/* L1 HHO action typel */
typedef enum
{
  /* L1 is in Forward Timing Initialized HHO */
  TDSL1_HHO_FW_TI,

  /* L1 is in Forward Timing Maintained HHO */
  TDSL1_HHO_FW_TM,

  /* L1 is in Reverse Timing Initialized HHO (Going back to Old config) */
  TDSL1_HHO_REV_TI,

  /* L1 is in Reverse Timing Maintained HHO (Going back to Old config) */
  TDSL1_HHO_REV_TM
} tdsl1_hho_action_enum_type;

/*#ifdef FEATURE_WCDMA_BUILD_PDU_DSM_OPT*/
#define L2_DSM_INUSE_MASK 0x00010000
/*#endif*/

//#ifdef FEATURE_WCDMA_UL_DATA_PATH_OPT

#if 0
/* Enumerated type to determine ciphering enabled and ciphering
 * algorithm:
 * This is a 2-bit value -
 * bit 0 indicates ciphering algorithm (0 - Kasumi, 1-Snow3G)
 * bit 1 indicates ciphering enabled (1) or not (0)
 */
typedef enum
{
 TDSL1_CIPHERING_ALGO_NONE   = 0,
 TDSL1_CIPHERING_ALGO_KASUMI = 2,
 TDSL1_CIPHERING_ALGO_SNOW3G = 3
} tdsl1_ciphering_algo_enum_type;
#endif

/* Maximum number of data segment source descriptors.
**
** Size changed to 300 as a tdseulenc_result of incremental
** testing for special test case
*/
#define TDSL1_SRC_DESC_LIST_MAX_SIZE 300

/* Maximum number of cipher control words
**
** Size changed to 180 a tdseulenc_result of incremental
** testing for special test case
*/
#define TDSL1_MAX_NUM_UL_CIPHER_CTRL_WORDS 180

/* Max number of PDUs which can be prepared in one TTI between L2 and L1 */
#define TDSEULENC_MAX_NUM_PDU_BLD 200

/* Data segment type -
 * This includes a 8-bit ptr to the data segment and a
 * 32-bit field containing the 16-bit segment length and a
 * last-unit indicator. These are clubbed together because Data-mover
 * driver requires this interface
 */
typedef struct
{
  uint8 *data_seg_list_ptr;
  uint32 length;        /* Data-mover limits length to 16-bit value */
} tdsl1_data_seg_list_type;

typedef struct
{
  /* Data-buffer pointer */
  tdsl1_data_seg_list_type
              *cur_ul_buf_ptr; /* pointer to current UL data buffer */

  /* Count of number of source descriptors written by RLC.
   * This is maintained by L1 and is reset at the end of the TTI
   */
  uint32 num_src_desc;
#ifdef FEATURE_TDSCDMA_DSDA
  /* Flag to indicate whether SRB data is present or not */
  boolean srb_data_present;
#endif
} tdsl1_l2_data_type;

//#endif /* FEATURE_WCDMA_UL_DATA_PATH_OPT  */

#ifdef FEATURE_TDSCDMA_PWR_TFC_ELMN
/* Status for each TFC at the current subframe: 
 * 0: supported; 1: Excess; 2: No change(DTXed or TDMed or Out-of-sync) 
 */
typedef enum
{
  TDSL1_UL_TFC_PWR_STATE_SUPPORTED,
  TDSL1_UL_TFC_PWR_STATE_EXCESS,
  TDSL1_UL_TFC_PWR_STATE_NO_CHANGE
} tdsl1_ul_tfc_pwr_state_enum_type;
#endif


/* Status for each TFC based on rate matching calculated:
 * 0: allowed
 * 1: not allowed, because the rate matched size is too large to fit in
 *    available physical channels
 */
typedef enum
{
  TDSL1_UL_RM_STATE_ALLOWED,
  TDSL1_UL_RM_STATE_NO_ALLOWED
} tdsl1_ul_rm_state_enum_type;


/*===========================================================================
FUNCTION tdsl1_is_sub_chan_avail_for_asc

DESCRIPTION
  This function is called by MAC to check if there is any assigned subchannel
  number corresponding to the ASC selected for this TTI.

DEPENDENCIES
  None

RETURN VALUE
  Returns TRUE if there is any assigned subchannel matching the ASC else
  returns FALSE.

SIDE EFFECTS
  None
===========================================================================*/

boolean tdsl1_is_sub_chan_avail_for_asc(uint8 asc_idx);


/*===========================================================================
FUNCTION tdsl1_mac_hs_query_mac_hs_reset_reqd

DESCRIPTION
  This function is in MAC-hs module. It resturns if there there is MAC-hs
  reconfiguration request with MAC-hs request

DEPENDENCIES
  None

RETURN VALUE
  Boolean indicating if MAC-hs reset is required

SIDE EFFECTS
  None
===========================================================================*/

extern boolean tdsl1_mac_hs_query_mac_hs_reset_reqd(void);

#if defined (FEATURE_TDSCDMA_HSDPA_MAC_EHS)

/*============================================================================*/
/**
 * This function returns one free TSN entry from Free Queue.
 */
/*============================================================================*/
extern tdsl1_dec_tsn_info_type *mcaltds_dec_get_tsn_buffer(void);  
#endif


#if 0
#if  defined(FEATURE_WCDMA_HFN_UPDATE)
/*===========================================================================
FUNCTION MAC_UPDATE_CIPHERING_PARAMS

DESCRIPTION
  This function passes CFN+2 in UL timeline ISR to MAC for ciphering to update
  HFN. In resume case, it is called after L1M is ready to notify MAC to resume.
  In normal case, it is called after PhyChan Establishment.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  MAC updates HFN
===========================================================================*/
void tdsmac_update_ciphering_params(uint8 cfn);

#endif /* FEATURE_WCDMA_HFN_UPDATE */


#endif
/*==============================================================================================
 FUNCTION NAME  void tdsl1_suspend_ind

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
extern void tdsl1_suspend_ind
(
  uint8     cfn,
  //#ifndef FEATURE_WCDMA_HFN_UPDATE
  tdsl1_hho_action_enum_type   hho_action
  //#else
  //uint8     num_roll_overs,
  //boolean   revert_back_to_old_config
  //#endif
);

/*==============================================================================================
 FUNCTION NAME  void tdsl1_resume_ind

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
extern void tdsl1_resume_ind
(
  uint8     cfn,
  uint8     num_roll_overs,
  //#ifndef FEATURE_WCDMA_HFN_UPDATE
  tdsl1_hho_action_enum_type   hho_action
  //#else
  //boolean   revert_back_to_old_config
  //#endif
);

#ifdef FEATURE_TDSCDMA_PWR_TFC_ELMN
/*===========================================================================
FUNCTION        tdsl1ul_get_tfc_excess_pwr_states

DESCRIPTION     This function returns the table of tfc excess power state values

DEPENDENCIES    The table enc_tfc_excess_pwr_state should be filled with new
                values after reconfig.
                The CCTrCh should have been reconfigured

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
tdsl1_ul_tfc_pwr_state_enum_type  const  *tdsl1ul_get_tfc_excess_pwr_states( void );

#endif /* FEATURE_TDSCDMA_PWR_TFC_ELMN */

/*===========================================================================
FUNCTION        tdsl1ul_get_rate_matching_states

DESCRIPTION     This function returns the table of tfc rate matching state

DEPENDENCIES    The table tdsl1_ul_rm_state is updated by RM unit when UL
                calls tdsrm_ul_dpch_rm_process to do DPCH rate matching.

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
tdsl1_ul_rm_state_enum_type  const  *tdsl1ul_get_rate_matching_states( void );


/*===========================================================================

FUNCTION TDSSTMR_GET_CURRENT_TIME

DESCRIPTION
  This function gets the current time in units of 32 chips of the
  Rx time counter with regard to the frame boundary.

DEPENDENCIES
  None

RETURN VALUE
  current time in units of 32 chips.

SIDE EFFECTS
  None

===========================================================================*/
uint32 tdsstmr_get_current_time(void);


//#ifdef FEATURE_WCDMA_UL_DATA_PATH_OPT
/*=========================================================
FUNCTION      L1_L2_BUILD_CIPHER_ENGINE_CTRL_INFO

DESCRIPTION   This function is called by RLC to create the
              ciphering control words from the parameters

DEPENDENCIES  Pointer to buffer and word to write should be
              initialized

RETURN VALUE  VOID

SIDE EFFECTS  Ciphering Control word will be written to
              tdseulenc_ciphering_ctrl_buf
============================================================*/
extern void tdsl1_l2_build_cipher_engine_ctrl_info
(
  uint32 data_unit_len,                       /* bits 0~14 data_unit_len */
  uint32 radio_bearer_id,                     /* bits 15~19 radio_bearer_id */
  uecomdef_umts_cipher_algo_e_type cipher_algo,/* bit 20 algo Snow3G/Kasumi */
  uint32 cipher_key_index,                    /* bits 21~23 cipher_key_index */
  uint32 cipher_offset,                       /* bits 24~29 ciphering_offset */
  boolean last_unit_of_tti,                   /* bit 31 indicates last_unit  */
  boolean cipher_enabled,                     /* bit 30 indicates ciph enabled */
  uint32 cipher_count_c                       /* CountC present if ciph enabled,else -1) */
);

//#ifdef FEATURE_WCDMA_UL_MODEM_ACC
/*=========================================================
FUNCTION      L1_L2_BUILD_COPY_ENGINE_CTRL_WORD

DESCRIPTION   This function is called by RLC to indicate to L1 to 
              build the copy engine control word for the current
              PDU. Offset length tells L1 how many data bytes to
              ignore in the source data-section. 
              Offset is always a positive value.

DEPENDENCIES  Validity of ctrl words not verified

RETURN VALUE  VOID

SIDE EFFECTS  Copy Engine control word is built in tdsedpch_l1_p2_ul_ctrl_wrd
============================================================*/
extern __inline void tdsl1_l2_build_copy_engine_ctrl_info
(
  uint32 hdr_len_bits,
  uint32 data_len_bits,  
  uint32 offset_bits
);

/*=========================================================
FUNCTION      L1_L2_WRITE_DATA_SECTION

DESCRIPTION   This function is called by RLC after it's done
              ready to be written to EUL ERAM. 

DEPENDENCIES  None

RETURN VALUE  VOID

SIDE EFFECTS  PDU bytes are word-alligned before ERAM copy
============================================================*/
void tdsl1_l2_write_data_section
(
 uint32 data_section_length_bytes  // FIXME: Please add to Intf doc - need the total length
);

/*=========================================================
FUNCTION      L1_L2_WRITE_HDR_SECTION

DESCRIPTION   This function is called by RLC to write header info to
              header section in ERAM. RLC should call this after it's 
              done filling the header info data structure.
              L1 will use Modem data mover to do this copy.

DEPENDENCIES  RLC should have defined and prepared the header info
              block.

RETURN VALUE  VOID

SIDE EFFECTS  All Ctrl information is written to HW CTRL RAM 
============================================================*/
void tdsl1_l2_write_hdr_section
(
  uint32      hdr_section_len_bytes,     /* header section in bytes */
  uint8      *l1_rlc_pdu_hdr_sect_ptr   /* header info ptr*/
);

//#endif /* FEATURE_WCDMA_UL_MODEM_ACC */
//#endif /* FEATURE_WCDMA_UL_DATA_PATH_OPT */

/*#ifdef FEATURE_WCDMA_BUILD_PDU_DSM_OPT*/
/*===========================================================================

FUNCTION L1_L2_DSM_FREE_PACKET

DESCRIPTION
  Frees DSM item is not used either by L1 or RLC.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void tdsl1_l2_dsm_free_packet
(
  dsm_item_type **item_ptr
);
/*#endif*/

#ifdef FEATURE_TDSCDMA_E4A_OPT
/*=========================================================================
FUNCTION tdsuldpchctrl_get_e4a_barred

DESCRIPTION
  This function returns if e4a is barred or not from L1 evaluation

DEPENDENCIES
  None.

RETURN VALUE
  TRUE: e4a is barred. FALSE: e4a is not barred.

SIDE EFFECTS
  None.
=========================================================================*/
boolean tdsuldpchctrl_get_e4a_barred(void);
#endif

#endif /* TDSL1MACIF_H */
