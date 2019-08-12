#ifndef DECHS_H
#define DECHS_H

/*==========================================================================
             WCDMA L1 HSDPA configuration header file

DESCRIPTION
  This file contains definition, declaration required for HSDPA configuration
  and maintainance code.

  Copyright (c) 2004 - 2012 by Qualcomm Technologies, Inc.
  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/src/dechs.h#1 $
$DateTime: 2016/12/13 08:00:12 $
$Author: mplcsds1 $

when       who    what, where, why
--------   ---    ---------------------------------------------------------- 
10/08/14   jhl    Mainlined FEATURE_WCDMA_MDSP_INTERFACE_REV_6280 
07/21/14   hdk    Moving hs logging function prototype to hslog.h
06/27/14   hdk    Moving Logging functions to hslog.c    
06/16/14   sm     API to support wait for agc tune done when RxD transitions on
03/21/14   hdk    Forcing Pess CQI to flush last log packet when HSDPA is stopped
03/17/14   hdk    Forcefully resetting Pess CQI logging state machine when HS channel is stopped.
03/11/14   hdk    Removing HS logging state machine for HS decode, A&E and SCCH stats log pkts
02/03/14   vs     0x4222 redesign to account for the new circular buffer logging.
11/18/13   rsr    Added QICE 3C support.
11/12/13   hdk    0x4222 HS Deocode log pkt support for 3C.
07/24/12   geg    Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
05/9/12    vs     Added the support for the new log packet(0x423f).
01/28/12   vs     Nikel feature cleanup.
07/05/11   vsr    Mainlining Genesis CME features
03/14/11   ms     Mainlined FEATURE_WCDMA_MDSP_INTERFACE_REV_6280
10/06/10   ms     Klockwork fixes for LLR & MIMO Demod logging
10/05/10   mc     Enhanced QXDM overrides for misAsyncBuffer for Genesis
                  Use CLTD2 bitmask independent of CLTD1 bitmask
08/27/10   vsr    Changes for DC HS stats collection
05/14/10   rvs    Fix compile error in function declaration.
05/14/10   rvs    Add prototypes for function to set MIMO CQI margin for
                  each receiver for either PCPICH or SCPICH mode.
02/05/10   ms     Taxis/Genesis code merge
12/29/09   ms     Support to log secondary carrier HS stats in 0x4222.
11/20/09   ms     Updated UE category field in log pkt to support Cat24
11/13/09   ms     Added HS Decode Status Ver6 log packet support for DC.
09/09/09   rvs    Add callback to CME to notify of SCCH decode valid and
                  attempted statistics.
07/15/09   rvs    Added dec_hs_get_hs_pdsch_div_mode().
07/10/09   vc     Added support for 0x420F log packet and increased A&E
                  sample bitwidth for 0x421D.
05/20/09   vc     Added function prototype definitions for dec_hs_mimo_chan_submit_log_buffer
                  and void dec_hs_mimo_demod_submit_log_buffer  
05/08/09   sup       Changing the includes for the renaming of header files  dec.h, enci.h, l1m.h, srch.h, srchi.h, 
                    srchlog.h to wdec.h, wenci.h, wl1m.h, wsrch.h, wsrchi.h, wsrchlog.h
04/24/09   mr     Fixed Compiler Warnings
03/16/09   rc     Added a macro to update the RxD status in dec_hs_rl_status_mask
                  used in HS CQI BLER alignment.
03/12/09   rc     Added support for HS CQI BLER alignment.
03/03/09   ms     Removed unused non-MCAL code
12/02/08   ms     Removed feature dependency for FEATURE_PESSIMISTIC_CQI.
11/24/08   scm    Use RUMI_BRING_UP to fix CQI compilation problem.
10/28/08   mc     Added support for AUTO ACK.
09/24/08   vc     Added support for 0x4224 MIMO demod log packet.
09/12/08   vc     Added HARQ stat update for dual stream in 0x4221 log packet.
08/15/08   vc     Added mimo specific log support to 0x4222 log packet.
06/10/08   ms     Support for HS SCCH Debug log packet
01/25/08   mc     Added support for MCAL
05/14/07   mmr    Added code to get fing params update for Triage.
08/02/06   mc     HS decode status log packet is logged based on variable
                  exported from mDSP or EQ feature being defined.
05/18/06   mc     Added a function to query the HS stat next log action.
04/20/06   mc     Added support to log a subframe as MISSED if there has 
                  been a EQ->Rake switch.
04/12/06   mc     Added support for EQ log packets
04/11/06   bd     Mainlined the file from .../l1_msm6275/dechs.h#8
03/13/06   mc     Fixed lint errors
11/23/05   mc     Added support for HS amplitude and energy estimation 
                  log packet. 
08/25/05   mc     Added function definitions and variables for logging
                  pessimistic CQI samples.
07/20/05   mc     Added external variables for manual CQI update and also
                  prototype for the diag command callback function.
03/17/05   mc     Removed lint warnings and errors.
01/12/05   gs     Added macro for number of MAC-hs header log buffers
                  Externalized functions dec_hs_init and functions related
                  to MAC-hs header log support
12/27/04   gs     Added include file dec.h
                  Externalized vaiables to maintain overall BLER
10/11/04   gs     Created this module

===========================================================================*/

#include "wcdma_variation.h"
#include "comdef.h"
#include "customer.h"


/* ==========================================================================
** Includes and Public Data Declarations
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */

#include "wdec.h"

#include "l1rrcif.h"
#include "hscfg.h"

/* ---------------------------- */
/* Check for necessary features */
/* ---------------------------- */

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/* Number of HS SCCH debug log buffer */
#define HS_SCCH_DBG_LOG_NUM_BUF 2
  
/* HS decode status fields */
/* ----------------------- */

/* Number of HS decode status buffer */
#define HS_DECODE_STATUS_NUM_BUF 2
/* number of W16 in decode status entry for sub-frame in mDSP */
#define HS_DECODE_STATUS_NUM_W16_PER_ENTRY 3

/* Various fields in mDSP decode status buffer for a sub-frame */

#define HS_DECODE_STATUS_SCCH_DEMOD_ATTEMPTED_BMSK 0x8000
#define HS_DECODE_STATUS_SCCH_DEMOD_ATTEMPTED_REG  0
#define HS_DECODE_STATUS_SCCH_DEMOD_ATTEMPTED_SHFT 15

#define HS_DECODE_STATUS_SCCH_DEMOD_VALID_BMSK 0x4000
#define HS_DECODE_STATUS_SCCH_DEMOD_VALID_REG  0
#define HS_DECODE_STATUS_SCCH_DEMOD_VALID_SHFT 14

#define HS_DECODE_STATUS_SCCH_DATA_BMSK 0x3FFF
#define HS_DECODE_STATUS_SCCH_DATA_REG  0
#define HS_DECODE_STATUS_SCCH_DATA_SHFT 0

#define HS_DECODE_STATUS_HARQ_ID_BMSK 0x0038
#define HS_DECODE_STATUS_HARQ_ID_REG  0
#define HS_DECODE_STATUS_HARQ_ID_SHFT 3

#define HS_DECODE_STATUS_WINNING_SCCH_CH_IDX_BMSK 0xC000
#define HS_DECODE_STATUS_WINNING_SCCH_CH_IDX_REG  1
#define HS_DECODE_STATUS_WINNING_SCCH_CH_IDX_SHFT 14

#define HS_DECODE_STATUS_DSCH_TB_SZ_BMSK 0x3FFF
#define HS_DECODE_STATUS_DSCH_TB_SZ_REG  1
#define HS_DECODE_STATUS_DSCH_TB_SZ_SHFT 0

#define HS_DECODE_STATUS_UNQUAN_CQI_BMSK 0xFFF0
#define HS_DECODE_STATUS_UNQUAN_CQI_REG  2
#define HS_DECODE_STATUS_UNQUAN_CQI_SHFT 4

#define HS_DECODE_STATUS_NEW_TX_BMSK 0x0002
#define HS_DECODE_STATUS_NEW_TX_REG  2
#define HS_DECODE_STATUS_NEW_TX_SHFT 1

#define HS_DECODE_STATUS_DSCH_STATUS_BMSK 0x0001
#define HS_DECODE_STATUS_DSCH_STATUS_REG  2
#define HS_DECODE_STATUS_DSCH_STATUS_SHFT 0

#define HS_DECODE_STATUS_DSCH_STATUS_NACK 0
#define HS_DECODE_STATUS_DSCH_STATUS_ACK  1
#define HS_DECODE_STATUS_DSCH_STATUS_AUTO_ACK  2


/* DPCH_STTD bit field mask and shift in control register */
#define DEC_HS_DPCH_STTD_BMSK      0x01
#define DEC_HS_DPCH_STTD_SHFT      0

/* DPCH_CLTD bit field mask and shift in control register */
#define DEC_HS_DPCH_CLTD1_BMSK      0x02
#define DEC_HS_DPCH_CLTD1_SHFT      1

#define DEC_HS_DPCH_CLTD2_BMSK      0x04
#define DEC_HS_DPCH_CLTD2_SHFT      2

/* CPICH Diveristy mask and shift in the control register */
#define DEC_HS_DIV_PILOT_BMSK      0x08
#define DEC_HS_DIV_PILOT_SHFT      3

/* RxD transition bit field mask and shift in control register */
#define DEC_HS_RXD_TRANS_BMSK      0x10
#define DEC_HS_RXD_TRANS_SHFT      4

/* Macro to extract the bit field value from the HS RL status mask */
#define DEC_HS_GET_RL_STATUS_BF_PARAM(buffer, type) \
  ((buffer & DEC_HS_##type##_BMSK) >> DEC_HS_##type##_SHFT)


/* Macro to set the bit foeld value from the HS RL status mask */
#define DEC_HS_SET_RL_STATUS_BF_PARAM(type, value) \
  dec_hs_rl_status_mask = ((dec_hs_rl_status_mask & ~(DEC_HS_##type##_BMSK)) | ((value << DEC_HS_##type##_SHFT) & DEC_HS_##type##_BMSK))


#define DEC_HS_SET_RXD_STATUS_IN_HS_RL_MASK()   DEC_HS_SET_RL_STATUS_BF_PARAM(RXD_TRANS, rxd_is_active())

/* This macro gets the value form mDSP decode status buffer (W16 array)
   for the given field name */
#define HS_DECODE_STATUS_FIELD_VAL(buffer, field) \
  (((buffer)[HS_DECODE_STATUS_##field##_REG] & HS_DECODE_STATUS_##field##_BMSK) >> \
   HS_DECODE_STATUS_##field##_SHFT)
   
/* For HS decode status TB data bit logging, sub frame for frame N-2
   are logged at sub frame N, so there is 3 frame buffer, it will have
   3x5= 15 sub frames. Keep data buffered for 16 sub frames indexed with
   subframe mod 16 */
#define HS_DECODE_DSCH_TB_DATA_BIT_BUF_LEN 16
  
/* Number of log packet buffer for MAC hs header logging */
#define DEC_HS_NUM_MAC_HS_HDR_LOG_PKT_BUF 2

/* Number of HS LLR log buffer */
#define HS_AMP_ENG_LOG_NUM_BUF 2
#define HS_MIMO_LOG_NUM_BUF 2
#define HS_PESS_CQI_LOG_NUM_BUF 2
/* Number of HS SCCH log buffer */
#define HS_SCCH_LOG_NUM_BUF 2

#define DEC_HS_INCR_LLR_LOG_BUF_OFFSET() \
  ((hs_llr_sample_buf_offset < \
          ((HS_NUM_MAX_HS_AMP_ENG_SAMPLES * HS_AMP_ENG_INFO_NUM_W8_PER_ENTRY)-1)) ? \
          hs_llr_sample_buf_offset++ : 0)

#ifdef FEATURE_WCDMA_MIMO
#define DEC_HS_INCR_MIMO_DEMOD_BUF_OFFSET() \
  ((hs_mimo_demod_sample_buf_offset < \
          (HS_MIMO_DEMOD_LOG_MAX_BUF_SIZE-1)) ? \
          hs_mimo_demod_sample_buf_offset++ : 0)
#endif
/*Macro for invalid Stop/Final GSFN for logging*/
#define INVALID_STOP_GSFN (0xFFFF)
/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

/* Call back function type declaration */
/* ----------------------------------- */

/* Enum type declaration */
/* --------------------- */

/* This enum defines the various actions for pessimistic CQI logging */
typedef enum
{
  DEC_HS_LOG_PESS_CQI,
  DEC_HS_CLEAR_PESS_CQI,
  DEC_HS_FLUSH_PESS_CQI
}dec_hs_pess_cqi_action_enum_type;

typedef enum
{
  DEMFRONT_RAKE = 1,
  DEMFRONT_QICE,
  DEMFRONT_INVALID = 0xFF
}dec_hs_demfront_enum_type;
/* Structure type declaration */
/* -------------------------- */



/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */

/* HS-DSCH MAC-d PDU deciphering related variables */

/* Deciphering start address of current MAC-d PDU */
extern uint16 dec_hs_tb_deciph_start_addr;
/* Deciphering start bit position at start address */
extern uint16 dec_hs_deciph_start_bit_pos;
/* Length with in MAC-d PDU size to decipher. This length is one less
   than actual length */
extern uint16 dec_hs_tb_deciph_len;
/* Offset of first bit to be deciphered of current MAC-d PDU from start of
   HS-DSCH block */
extern uint16 dec_hs_blk_deciph_first_bit_offset;

/* HS-DSCH MAC-d SDU data transfer related variables */

/* DOB start address to read data from */
extern uint16 dec_hs_tb_bg_xfer_start_addr;
/* Starting offset to read first bit at DOB start address */
extern uint8  dec_hs_bg_xfer_start_offset;
/* MAC-d SDU length */
extern uint16 dec_hs_tb_bg_xfer_len;

/* status mask for HS RL TxD status */
extern uint8 dec_hs_rl_status_mask;
/* Critical section mutex for hs_rl_status_mask */
extern rex_crit_sect_type hs_rl_status_mask_crit_sect;

/* This maintains running count of new transmissions received and
   that are in error */
extern uint32 dec_hs_num_new_tx_rece;
extern uint32 dec_hs_new_tx_in_err;

/* =======================================================================
**                        Function Declarations
** ======================================================================= */


/*===========================================================================
FUNCTION dec_hs_init

DESCRIPTION
  This function initialize various variables in this module. It is intended
  to be called at L1 stack startup along with other module init.
  
DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  Variables get inited to their initial values
===========================================================================*/

extern void dec_hs_init(void);

#ifdef FEATURE_WCDMA_MIMO
/*===========================================================================
FUNCTION dec_hs_set_mimo_cqi_margin_rake_pcpich

DESCRIPTION
  Update the CQI Margin values in FW for Rake during P-CPICH MIMO.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void dec_hs_set_mimo_cqi_margin_rake_pcpich(uint16 mimo_cqi_margin_ss,
                                                   uint16 mimo_cqi_margin_ds);

/*===========================================================================
FUNCTION dec_hs_set_mimo_cqi_margin_eq_pcpich

DESCRIPTION
  Update the CQI Margin values in FW for EQ during P-CPICH MIMO.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void dec_hs_set_mimo_cqi_margin_eq_pcpich(uint16 mimo_cqi_margin_ss,
                                                 uint16 mimo_cqi_margin_ds);

/*===========================================================================
FUNCTION dec_hs_set_mimo_cqi_margin_qice_pcpich

DESCRIPTION
  Update the CQI Margin values in FW for QICE during P-CPICH MIMO.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void dec_hs_set_mimo_cqi_margin_qice_pcpich(uint16 mimo_cqi_margin_ss,
                                                   uint16 mimo_cqi_margin_ds);

/*===========================================================================
FUNCTION dec_hs_set_mimo_cqi_margin_rake_scpich

DESCRIPTION
  Update the CQI Margin values in FW for Rake during S-CPICH MIMO.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void dec_hs_set_mimo_cqi_margin_rake_scpich(uint16 mimo_cqi_margin_ss,
                                                   uint16 mimo_cqi_margin_ds);

/*===========================================================================
FUNCTION dec_hs_set_mimo_cqi_margin_eq_scpich

DESCRIPTION
  Update the CQI Margin values in FW for EQ during S-CPICH MIMO.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void dec_hs_set_mimo_cqi_margin_eq_scpich(uint16 mimo_cqi_margin_ss,
                                                 uint16 mimo_cqi_margin_ds);

/*===========================================================================
FUNCTION dec_hs_set_mimo_cqi_margin_qice_scpich

DESCRIPTION
  Update the CQI Margin values in FW for QICE during S-CPICH MIMO.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void dec_hs_set_mimo_cqi_margin_qice_scpich(uint16 mimo_cqi_margin_ss,
                                                   uint16 mimo_cqi_margin_ds);

/*===========================================================================
FUNCTION dec_hs_set_mimo_cqi_margin_rake_ds_sic

DESCRIPTION
  Update the CQI Margin_DS_SIC for Rake during MIMO call
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void dec_hs_set_mimo_cqi_margin_rake_ds_sic(uint32 mimo_param_val);

/*===========================================================================
FUNCTION dec_hs_set_mimo_cqi_margin_eq_ds_sic

DESCRIPTION
  Update the CQI Margin_DS_SIC for EQ during MIMO call
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void dec_hs_set_mimo_cqi_margin_eq_ds_sic(uint32 mimo_param_val);

/*===========================================================================
FUNCTION dec_hs_set_mimo_cqi_margin_qice_ds_sic

DESCRIPTION
  Update the CQI Margin_DS_SIC for QICE during MIMO call
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void dec_hs_set_mimo_cqi_margin_qice_ds_sic(uint32 mimo_param_val);
#endif

/*===========================================================================
FUNCTION dec_hs_rxd_transition_callback

DESCRIPTION
  This is the callback function registered with RxD module. 
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void dec_hs_rxd_transition_callback(
  /* State of the callback, TRUE if RxD transition has started */
  boolean transition);

/*===========================================================================
FUNCTION dec_hs_srch_td_update_callback

DESCRIPTION
  This is the callback function registered with SRCH module. 
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void dec_hs_srch_td_update_callback(void);

/*===========================================================================
FUNCTION dec_hs_register_callbacks_for_cqi_bler_alignment

DESCRIPTION
  This function registers callbacks  
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void dec_hs_register_callbacks_for_cqi_bler_alignment(void);


/*===========================================================================
FUNCTION dec_hs_deregister_callbacks_for_cqi_bler_alignment

DESCRIPTION
  This function deregisters callbacks    
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void dec_hs_deregister_callbacks_for_cqi_bler_alignment(void);


typedef void DEC_HS_CME_CME_CB_FUNC_TYPE(uint8, boolean *, boolean *);

/*===========================================================================
FUNCTION dec_hs_get_hs_pdsch_div_mode

DESCRIPTION
  Returns the diversity mode of the HS-PDSCH.
  
DEPENDENCIES
  None

RETURN VALUE
  0 - None, 1 - STTD, 2 - CLTD

SIDE EFFECTS
  None
===========================================================================*/
extern l1_tx_div_mode_enum_type dec_hs_get_hs_pdsch_div_mode(void);

/*===========================================================================
FUNCTION dec_hs_demfront_mode

DESCRIPTION
  Returns the Demfront Mode and Rake by default if QICE is not enabled
  
DEPENDENCIES
  HS module

RETURN VALUE
  dec_hs_demfront_enum_type 

SIDE EFFECTS
  None
===========================================================================*/

dec_hs_demfront_enum_type dec_hs_demfront_mode(void);


#endif /* DECHS_H */ 
