#ifndef L1MACIF_H
#define L1MACIF_H

/*============================================================================
                            L1 MAC INTERFACE 


GENERAL DESCRIPTION
  This file contains the declarations for the command types that are
  received by L1 from MAC and contains declarations for the indications
  and confirmations sent by L1 to MAC.

Copyright (c) 2000 - 2014 by Qualcomm Technologies, Inc.  All Rights Reserved.

============================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/l1/vcs/l1macif.h_v   1.1   12 Jul 2001 11:43:34   mlevin  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/api/l1macif.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     -------------------------------------------------------- 
10/21/14    ts      SW enhancement to avoid reporting of event 6D due to persistent higher ETFCIs selected on both the carriers
07/01/14    hk      Header file inclusion cleanup
12/09/13    as      Added code change not to disable RACH if there is any pending data in DL
                    that needs to be received which in turn would not trigger EDRX full sleep.
11/15/13    as      Added code change to provide SRB data presence & corresponding TTI information to
                    L1 which in turn would increase the Tx priority for W during SRB data transmission.
02/12/13    raj     Adding Mutex protection to avoid the concurrencies and 
                    calling an L2 API to crash if PDU building fails.
08/13/12    ash     Populating ASC num for L1 in HSRACH code flow
07/24/12    geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
06/08/12    gv      Feature HS RACH Initial code check in
03/16/12    ms      Added support for FEATURE_WCDMA_HS_FACH
02/17/12    geg     Defeaturized FEATURE_WCDMA_FREE_FLOATING_TASK and FEATURE_WCDMA_TRIGGER_BASED_RACH
02/16/12    sp      Added support for Trigger Based Rach Procedure
01/31/12    as      Added changes to avoid compilation error due to free floating changes.
01/20/12    bs      free floating changes.
12/21/11    hk      Added a function to fetch the time for profiling
05/31/11    geg     Changes for RRC and L2 api files clean-up wcdma dependencies
03/30/11    gnk     Mainlined several features
02/23/11    asm     Cleanup of unused, mainlined or irrelevant features in offline UL code
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
05/03/05    vb      Added func prototype for l1_suspend_ind and l1_resume_ind
03/31/05    yiz     Add decl of mac_update_ciphering_params
02/26/05    gs      Added function declaration for
                    l1_mac_hs_query_mac_hs_reset_reqd
08/21/02    scm     Activation time related comment change.
08/07/02    scm     Define l1_mac_config_cnf_type for activation time support.
11/22/00    sh      Ported MAC related structures from l1rrcctrl.h

===========================================================================*/

/*===========================================================================

                      INCLUDE FILES

===========================================================================*/

#include "comdef.h"
#include "customer.h"
#include "uecomdef.h"
#include "err.h"
#include "msg.h"
#include "l1rrcif.h"
#include "l1const.h"
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
  L1_UL_PHYCHAN_ENABLE,
  L1_UL_PHYCHAN_DISABLE
} l1_ul_phychan_ctrl_type;

typedef struct
{
  /* physical channel type - PRACH or PCPCH */
  l1_ul_phychan_enum_type chan;
  /* specifies if physical channel is to be enabled or disabled */
  l1_ul_phychan_ctrl_type action;

} l1_ul_tx_ctrl_req_type;

/*--------------------------------------------------------------------------
      CPHY_MAC_UL_CONFIG_CNF and CPHY_MAC_DL_CONFIG_CNF

Used by MAC in support of activation times.  L1 signals MAC at the
activation time to process config requests from RRC.  Once MAC finishes,
it responds with this message, which includes a completion status.
Used for both uplink and downlink completions.
--------------------------------------------------------------------------*/
typedef struct
{
  /* Status returned from MAC layer.  TRUE means success. */
  boolean mac_config_status;
} l1_mac_config_cnf_type;

/* L1 HHO action typel */
typedef enum
{
  /* L1 is in Forward Timing Initialized HHO */
  L1_HHO_FW_TI,

  /* L1 is in Forward Timing Maintained HHO */
  L1_HHO_FW_TM,

  /* L1 is in Reverse Timing Initialized HHO (Going back to Old config) */
  L1_HHO_REV_TI,

  /* L1 is in Reverse Timing Maintained HHO (Going back to Old config) */
  L1_HHO_REV_TM
} l1_hho_action_enum_type;

#define L2_DSM_INUSE_MASK 0x00010000

/* Enumerated type to determine ciphering enabled and ciphering
 * algorithm:
 * This is a 2-bit value -
 * bit 0 indicates ciphering algorithm (0 - Kasumi, 1-Snow3G)
 * bit 1 indicates ciphering enabled (1) or not (0)
 */
typedef enum
{
 L1_CIPHERING_ALGO_NONE   = 0,
 L1_CIPHERING_ALGO_KASUMI = 2,
 L1_CIPHERING_ALGO_SNOW3G = 3
} l1_ciphering_algo_enum_type;

/* Maximum number of data segment source descriptors.
**
** Size changed to 300 as a result of incremental
** testing for special test case
*/
#define L1_SRC_DESC_LIST_MAX_SIZE 300

/* Maximum number of cipher control words
**
** Size changed to 180 a result of incremental
** testing for special test case
*/
#define L1_MAX_NUM_UL_CIPHER_CTRL_WORDS 180

/* Max number of PDUs which can be prepared in one TTI between L2 and L1 */
#define EULENC_MAX_NUM_PDU_BLD 400

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
} l1_data_seg_list_type;

typedef struct
{
  /* Data-buffer pointer */
  l1_data_seg_list_type
              *cur_ul_buf_ptr; /* pointer to current UL data buffer */

  /* Count of number of source descriptors written by RLC.
   * This is maintained by L1 and is reset at the end of the TTI
   */
  uint32 num_src_desc;

  /* Flag to indicate whether SRB data is present or not */
  boolean srb_data_present;
} l1_l2_data_type;

typedef struct
{
  boolean is_evt_6d_actvd;
  boolean elapsed_hlf_ttt;
  boolean is_evt_6d_expired;
  uint8   mac_eul_6d_etfci_x;
  uint8   mac_eul_6d_etfci_y;
  uint8   evt_6d_num_conseq_ttis;
  uint8   evt_6d_skip_count;
/*  uint8   etfci_idx_evt_6d[MAC_HS_UL_MAX_NUM_CARRIER_ALLOWED]; */
}mac_eul_evt_6d_common_info_type;

extern boolean l1_phy_hs_rach_check_rach_trigger(uint32 * asc_num);

/*=========================================================================
FUNCTION l1_mac_rach_enable_trigger_needed

DESCRIPTION
  This routine is called by MAC to check with WL1 whether to send 
  ext command to enable Build frame indication

DEPENDENCIES

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  None
=========================================================================*/
boolean l1_mac_rach_enable_trigger_needed(void);

/*=========================================================================
FUNCTION l1_mac_rach_disable_trigger_needed

DESCRIPTION  
  This routine is called by MAC to check with WL1 whether to send 
  ext command to disable build frame indication

DEPENDENCIES

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  None
=========================================================================*/
boolean l1_mac_rach_disable_trigger_needed(void);
/*=========================================================================
FUNCTION MAC_UL_RACH_CHECK_FOR_BFI_FAILURE

DESCRIPTION  
  This routine is called by L1 when L2 has data in buffer but fails to build PDU during 
  build frame indication event.

DEPENDENCIES

RETURN VALUE
None

SIDE EFFECTS
  None
=========================================================================*/
void mac_ul_rach_check_for_bfi_failure(void);

/*===========================================================================
FUNCTION l1_is_sub_chan_avail_for_asc

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

boolean l1_is_sub_chan_avail_for_asc(uint8 asc_idx);

/*===========================================================================
FUNCTION l1_mac_hs_query_mac_hs_reset_reqd

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

extern boolean l1_mac_hs_query_mac_hs_reset_reqd(void);

/*==============================================================================================
 FUNCTION NAME  hs_perform_mac_ehs_reset

 DESCRIPTION : This function is called by MAC whenever Treset timer
               expires. L1 will indicate FW to perform Soft
               Buffer flush and will cleanup the DOB FIFO Q.

DEPENDENCIES    None

RETURN VALUE    None

===============================================================================================*/
extern void hs_perform_mac_ehs_reset(void);

/*==============================================================================================
 FUNCTION NAME  void l1_suspend_ind

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
extern void l1_suspend_ind
(
  uint8     cfn,
  l1_hho_action_enum_type   hho_action
);

/*==============================================================================================
 FUNCTION NAME  void l1_resume_ind

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
extern void l1_resume_ind
(
  uint8     cfn,
  uint8     num_roll_overs,
  l1_hho_action_enum_type   hho_action
);

/*===========================================================================
FUNCTION        L1_UL_GET_TFC_EXCESS_PWR_STATES

DESCRIPTION     This function returns the table of tfc excess power state values

DEPENDENCIES    The table enc_tfc_excess_pwr_state should be filled with new
                values after reconfig.
                The CCTrCh should have been reconfigured

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
boolean const  *l1_ul_get_tfc_excess_pwr_states( void );


/*=========================================================
FUNCTION      L1_L2_BUILD_CIPHER_ENGINE_CTRL_INFO

DESCRIPTION   This function is called by RLC to create the
              ciphering control words from the parameters

DEPENDENCIES  Pointer to buffer and word to write should be
              initialized

RETURN VALUE  VOID

SIDE EFFECTS  Ciphering Control word will be written to
              ciphering_ctrl_buf
============================================================*/
extern void l1_l2_build_cipher_engine_ctrl_info
(
  uint32 data_unit_len,                       /* bits 0~14 data_unit_len */
  uint32 radio_bearer_id,                     /* bits 15~19 radio_bearer_id */
  uecomdef_wcdma_cipher_algo_e_type cipher_algo,/* bit 20 algo Snow3G/Kasumi */
  uint32 cipher_key_index,                    /* bits 21~23 cipher_key_index */
  uint32 cipher_offset,                       /* bits 24~29 ciphering_offset */
  boolean last_unit_of_tti,                   /* bit 31 indicates last_unit  */
  boolean cipher_enabled,                     /* bit 30 indicates ciph enabled */
  uint32 cipher_count_c                       /* CountC present if ciph enabled,else -1) */
);

/*=========================================================
FUNCTION      L1_L2_BUILD_COPY_ENGINE_CTRL_WORD

DESCRIPTION   This function is called by RLC to indicate to L1 to 
              build the copy engine control word for the current
              PDU. Offset length tells L1 how many data bytes to
              ignore in the source data-section. 
              Offset is always a positive value.

DEPENDENCIES  Validity of ctrl words not verified

RETURN VALUE  VOID

SIDE EFFECTS  Copy Engine control word is built in l1_p2_ul_ctrl_wrd
============================================================*/
extern void l1_l2_build_copy_engine_ctrl_info
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
void l1_l2_write_data_section
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
void l1_l2_write_hdr_section
(
  uint32      hdr_section_len_bytes,     /* header section in bytes */
  uint8      *l1_rlc_pdu_hdr_sect_ptr   /* header info ptr*/
);

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
void l1_l2_dsm_free_packet
(
  dsm_item_type **item_ptr
);

/*===========================================================================
FUNCTION        WL1_GET_PROFILE_POINT

FILE NAME       wl1i.c

DESCRIPTION     This function takes the current time stamp of the Tx
                system time and returns the frame quantity and chipx8
                to the passed variables

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
extern void wl1_get_profile_point(int32* frame_qty, int32* cx8_qty);


 
 extern void l1_l2_update_cipher_key_stream_offset_in_bytes(uint16 ciph_start_offset_in_bytes, 
                                                     uint16 ciph_key_stream_offset_in_bytes);
 
 
 extern void l1_l2_get_cipher_key_stream_offset_in_bytes(boolean *ciph_enabled, 
                                                  uint16 *ciph_start_offset_in_bytes, 
                                                  uint16 *ciph_key_stream_offset_in_bytes);


extern void l1_l2_build_copy_engine_ctrl_info_with_index
(
  uint32 hdr_len_bits,
  uint32 data_len_bits,  
  uint32 offset_bits,
  uint32 ctrl_wrd_idx
);

/*===========================================================================

FUNCTION        wl1_edrx_get_edrx_configured_status

DESCRIPTION     This function can be used to find out the E-DRX configured status

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None

===========================================================================*/
extern boolean wl1_edrx_get_edrx_configured_status(void);

/*===========================================================================
FUNCTION     L1_SRCH_6D_TRG_CHECK

DESCRIPTION
  This function updates the flags that will be used by MAC layer to prevent MTPL cycles.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void l1internalmeas_srch_6d_trg_check(mac_eul_evt_6d_common_info_type *mac_eul_etfci_evt_6d_common_info);

#endif /* L1MACIF_H */
