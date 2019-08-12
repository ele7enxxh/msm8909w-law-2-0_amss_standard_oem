#ifndef MCALWCDMA_DEMOD_H
#define MCALWCDMA_DEMOD_H

/*============================================================================*/
/** @file  
 * This module has definition and declaration related to MCAL UMTS demodulator
 * module APIs
 */
/*============================================================================*/

/*============================================================================
Copyright (c) 2007 - 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
============================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/inc/mcalwcdma_demod.h#1 $
$DateTime: 2016/12/13 08:00:12 $
$Author: mplcsds1 $

when        who     what, where, why
--------    ---     -------------------------------------------------------- 
09/01/14    kcm     Dime and Triton Featurization cleanup
05/21/14    sl      Changes for 0x4186 log packet version8
10/14/13    pr/sr   Channel config optimizations
08/07/13    pr      Race condition between cell reconfig due to TxD status change and MCVS updates.
07/23/13    vs      Added APIs for the antenna switch support.
07/15/13    kcm     Changes to populate carrier idx in fw/sw interface
05/30/13    pr      Set FTL mode to TRK/ACQ.
04/23/13    pr      Fixed Incorrect clearing of the TD Dirty Bit which results in 
                    wrong SW assumption of TD ongoing in FW
04/11/13    pr      Added logging changes for 0x4186 for debug purposes.
04/10/13    pr      Added Dirty bit to handle race condition between SW and FW during
                    TxD detection.
02/05/13    pv      DRX PICH log packet changes.
01/21/13    pv      API for initializing init RSSI for PCH_SLEEP.
07/24/12    geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
06/25/12    pr      Added logging for sixTapEn in 0x4186
05/03/12    pr      Changes to update DSD buffer with finger position during 
                    assignment. 
01/30/12    vs      Feature cleanup.
01/20/12    zr      Mainlined FEATURE_WCDMA_GENESIS_SW
11/15/11    hk      Added support to change combiner lock thresholds based on
                    finger combined RSSI
04/26/11    mc      Revert SW decoupling from this file.
04/25/11    mc      SW decoupling
12/02/10    hk      Cleanup the pending TD detection before disabling cell
11/02/10    sv      Defined TD detection thresholds for ACQ and non-ACQ states.
02/08/10    rgn     Syncup to mustang tip and added definition of 
                    MCALWCDMA_FTL_ACQ_FB_FACTOR
11/09/09    rgn     Added wrapper to read accumulated td energy
11/06/09    mr      Handle MIMO S_CPICH power Imbalance from RRC
10/24/09    ms      Updated TTL/FTL values for Genesis
08/12/09    rgn     Added td detect threshold
07/16/09    hk      Reorganized the code to remove INC/SRC violations
05/06/09    rgn     synchronize multiple demod cfgs and responses
05/01/09    vc      Added support for additional finger info ttlstepsize, apfAlpha.                      
04/17/09    rgn     Fixed compiler warnings
03/30/09    ks      Added FTL/TTL constants
01/13/09    vsr     Changes to check FW status multiple times during TD 
                    determination phase
12/08/08    rmak    Mainline FEATURE_QXT
12/03/08    rgn     Added support for Tx Diversity detection procedure 
11/04/08    rgn     New demod status dump designchanges
10/28/08    mmr     Added a new interface bit in the fing config struct type.
09/24/08    rgn     Initialised QXT info
09/15/08    mg      Add apis for getting freqcommon and tcxoaccum
07/17/08    mg      Support for Cx32 TTL
06/10/08    ms      Prototype for mcalwcdma_demod_update_ftl_ttl
07/18/07    yh/mmr  Initial revision.

===========================================================================*/

/* ==========================================================================
** Includes and Public Data Declarations
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */
#include "comdef.h"
#include "mcalwcdma_common.h"
#include "mcalwcdma_const.h"
#include "wl1api.h"

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
/** Bitmask to indicate to various finger utility functions that the
 *  operation is to be performed for all fingers */
#define MCALWCDMA_DEMOD_ALL_FINGERS_ID   0x80
/** Bitmask to indicate to various cell utility functions that
 *  the operation is to be performed for all cells */
#define MCALWCDMA_DEMOD_ALL_CELLS_ID     0x80

#define MCALWCDMA_FTL_FACTOR             0x10F
#define MCALWCDMA_TTL_FACTOR             0x2666
#define MCALWCDMA_FTL_ACQ_FB_FACTOR      0x2190

/** td detection threshold written to fw */
#define MCALWCDMA_DEMOD_TDDET_THRESH     50000000

/** td detection threshold written to fw during ACQ state*/
#define MCALWCDMA_DEMOD_TDDET_ACQ_THRESH     30000000
/** td detection threshold written to fw during non-ACQ states */
#define MCALWCDMA_DEMOD_TDDET_NON_ACQ_THRESH 12000000


/* MCAL DEMOD critical section */
extern rex_crit_sect_type mcalwcdma_demod_crit_sect;

#define MCALWCDMA_DEMOD_INTLOCK()  REX_ISR_LOCK(&mcalwcdma_demod_crit_sect)
#define MCALWCDMA_DEMOD_INTFREE()  REX_ISR_UNLOCK(&mcalwcdma_demod_crit_sect)


/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

/* Call back function type declaration */
/* ----------------------------------- */

/* Enum type declaration */
/* --------------------- */
typedef enum
{
  /* signifies cell cfg state request*/
  MCALWCDMA_DEMOD_CELL_RESOURCE,
  /* signifies fing cfg state request*/
  MCALWCDMA_DEMOD_FING_RESOURCE,
  /* maximum number of demod resources*/
  MCALWCDMA_DEMOD_MAX_NUM_DEMOD_RESOURCE
}mcalwcdma_demod_resource_type;

/** Config state for finger or cell */
typedef enum {
  /** No configuration pending */
  CFG_INACTIVE,
  /** Configuration is in mDSP sync queue */
  CFG_PENDING,
  /** Configuration in progress in mDSP */
  CFG_IN_PROG
} mcalwcdma_demod_cfg_state_enum_type;

/* Struct type declaration */
/* ----------------------- */
/** Structure for finger config parameters */
typedef struct {
  /** Word 0 */
  /** Finger PN position in Cx8 units. Range 0..307200-1 */
  uint32 pnPosCx8;
  /** Cell index to which finger belongs. Range 0..MaxCells-1 */
  uint8 cellIdx;
  /** Force lock status */
  uint8 ForceLockInit;
  /** Antenna on which finger should operate. Range 0 or 1 */
  uint8 ssId;
  /** Phase reference. 0 is primary 1 is sec */
  uint8 phaseRef;
  /** Quick freq and time tracking enables. Range 0 or 1 */
  boolean qxtEnabled;
  /** Finger freq tracking is started from ACQ mode. Range 0 or 1 */
  boolean ftInit;
  /** Finger RSSI init. Range 0 or 1 */
  boolean rssiInit;
  /** Finger is initialized. Range 0 or 1. Set this to True
   *  whenever there is a change in the above parameters */
  boolean init;
  /** Indicate reference finger enabled or not. Range 0 or 1 */
  boolean enabled;

  /** Word 1 */
  /** Initial RSSI */
  uint16 initRssi;
  /** Init freq error */
  uint16 initFreqError;

  /** Word 2 */
  /** Finger Pair Id */
  uint8 fingerPairId;
  /** JTT enabled. Range 0 or 1 */
  boolean fingerJTTEnabled;
  /** JFT enabled. Range 0 or 1 */
  boolean fingerJFTEnabled;
  /** MMSE enabled. Range 0 or 1 */
  boolean fingerMMSEEnabled;
  /** Pairing mode Padding */
  uint8 pairAssignMode;
  /** RxD Init Range 0 or 1 Set this to True
   *  whenever there is a change in the rxd parameters above
   *  parameters*/
  boolean rxdInit;

  /*FTL mode for finger(ACQ/TRK)*/
  boolean ftlTrkInd;
} mcalwcdma_demod_fing_config_struct_type;

/** Structure for the information of each finger config. This
 *  will be the used internally by MCAL DEMOD to keep a shadow
 *  of pending information to be sent to FW */
typedef struct {
  /** Finger has updates and is waiting to send cmd to FW */
  boolean update;
  /** Flag to indicate that this finger on hold for a disable */
  boolean disable_on_hold;
  /** Finger configuration state. It indicates if command has been
   *  sent to mdsp_sync or already being processed by FW or it is
   *  inactive. */
  mcalwcdma_demod_cfg_state_enum_type finger_cfg_cmd_state;
  /** Finger info */
  mcalwcdma_demod_fing_config_struct_type fing_cfg;
} mcalwcdma_demod_fing_info_struct_type;

/** Structure for cell config parameters */
typedef struct {
  /** Word 0 */
  /** Reference finger index. Range 0..31 */
  uint8 refFingerPri;
  /** Reference finger Valid or not. Range 0 or 1 */
  boolean refFingerPriValid;
  /** Reference finger time tracking enabled. Range 0 or 1 */
  boolean refFingerPriTTEnabled;
  /** CPICH STTD enabled. Range 0 or 1 (1 bits) */
  boolean cpichSTTDEnabled;
  /** Following are valid if secCPICHenabled is TRUE */
  /** Reference finger index. Range 0..31 */
  uint8 refFingerSec;
  /** Reference finger Valid or not. Range 0 or 1 */
  boolean refFingerSecValid;
  /** Reference finger time tracking enabled. Range 0 or 1 */
  boolean refFingerSecTTEnabled;
  /** Sec CPICH OVSF code. Range 0..255 */
  boolean secCPICHovsfCode;
  /** Sec CPICH Enabled, use secScrCodeIdx. Range 0 or 1 */
  boolean secCPICHenabled;
  /*! Bit 25 Sec CPICH secondary scrambling code select */
  boolean secCPICHSecScrCodeSel;
  /*! Bit 26 Sec CPICH secondary mode - 0=non-MIMO mode, 1=MIMO mode */
  boolean secCPICHMode;
  /** Enable flag for STTD determination algorithm,
   *  valid at initial cell configuration */
  boolean tdDetEn;
  /** Cell enabled */
  boolean enabled;

  /** Word 1 */
  /** CPICH primary scrambling code. Range 0..511 */
  uint16 priScrCode;
  /** sec scrambling code. Range 0..15 (4 bits) */
  uint8 secScrCodeIdx;
  /** EQ index for cell, if eqValid. Range 0..2 (2 bits) */
  uint8 eqIdx;
  /** EQ index valid. Range 0 or 1 */
  boolean eqValid;
  /** SCH in TSTD mode. Range 0 or 1 */
  boolean schTSTDEnabled;
  /** Invert SCH (if P-CCPCH is STTD) */
  boolean schInvert;
  /*! SCPICH Power Imbalance scale in Q14 */
  uint16 mimoScpichImbScale;
  /** Carrier Idx*/
  uint8 carrierIdx;

} mcalwcdma_demod_cell_config_struct_type;

/** Structure for the information of each cell config. This will
 *  be the used internally by MCAL DEMOD to keep a shadow of
 *  pending information to be sent to FW */
typedef struct {
  /** Cell has updates and is waiting to send cmd to FW */
  boolean update;
  /** Flag to indicate that this cell and all the fingers are waiting to be disabled */
  boolean disable_on_hold;
  /** Bitmask of which fingers it is holding a disable */
  uint32 finger_holding_disable_bmsk;
  /** Cell configuration state. It indicates if command has been
   *  sent to mdsp_sync or already being processed by FW or it is
   *  inactive. */
  mcalwcdma_demod_cfg_state_enum_type cell_cfg_cmd_state;
  /** Cell info */
  mcalwcdma_demod_cell_config_struct_type cell_cfg;
  boolean tdDetDirtyBit;
} mcalwcdma_demod_cell_info_struct_type;

/** WCDMA FW Demod status dump per-finger information
 *  structure */
typedef struct {
  /** finger PN position in Cx32 units */
  uint32 pnPosCx32;

  /** finger RSSI for TxD antenna 0 and 1 */
  uint32 rssi0;
  uint32 rssi1;

  /** finger frequency error estimate */
  int16 freqError;

  /** finger lock status */
  boolean  lockStatus;

  /* ttl stepSize info (0 – cx8, 1 –Cx32 )*/
  uint8 ttlStepSize;

  uint8 vrtCellBpgOffset;

  /* IIR filter tap coefficient for time-critical pilot filter */
  uint16 apfAlpha;

  uint8 cellRscType;
  /*TCPF^2 in Q16 format*/
  /* the non-diversity RSSI */
  uint16 tcpfSq0;

  /* the diversity RSSI */
  uint16 tcpfSq1;

} mcalwcdma_demod_status_dump_fing_info_struct_type;

/** Structure for Demod Status Dump information */
typedef struct {
  /** indicates if status dump was successful and results are
   *  valid */
  boolean results_under_process;
  /** the reference count in Cx8 units */
  uint32 ref_cnt_cx8;
  /** the TX system time in Cx8 units */
  uint32 tx_sys_time_cx8;
  /** the TX frame */
  uint32 tx_frame;
  /** cell div info */
  uint8 cell_divInfo[MCALWCDMA_MAX_NUM_CELLS];
  /** the combiner counters in Cx1 units */
  uint16 comb_cnt_cx1[MCALWCDMA_MAX_NUM_COMBINER];
  /** finger information latched during status dump */
  mcalwcdma_demod_status_dump_fing_info_struct_type
    fing_info[MCALWCDMA_MAX_NUM_FINGERS];
} mcalwcdma_demod_status_dump_info_struct_type;

/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */

/* =======================================================================
**                        Function Declarations
** ======================================================================= */
/*============================================================================*/
/**
 * This function initializes all MCAL DEMOD related items.
 */
/*============================================================================*/
extern void mcalwcdma_demod_init(void);

/*============================================================================*/
/**
 * This function is top level function to do complete finger assignment. It
 * takes as parameters the finger index to assign and the finger info. This 
 * function blindly copies all the finger configuration received into MCAL
 * DEMOD shadow. It assumes that if there was a prior configuration saved
 * for that finger, the configuration will be overwritten by this new 
 * command.
 *
 * @see mcalwcdma_demod_fing_info_struct_type
 */
/*============================================================================*/
extern void mcalwcdma_demod_fing_assign(
  /** Finger index */
  uint8 fing,
  /** Finger config assign parameters */
  mcalwcdma_demod_fing_config_struct_type *fing_config);


/*============================================================================*/
/**
 * This function is top level function to do complete finger assignment. It
 * takes as parameters the finger index to assign and the finger info. This 
 * function blindly copies all the finger configuration received into MCAL
 * DEMOD shadow. It assumes that if there was a prior configuration saved
 * for that finger, the configuration will be overwritten by this new 
 * command.
 *
 * @see MCALWCDMA_DEMOD_ALL_FINGERS_ID
 */
/*============================================================================*/
extern void mcalwcdma_demod_disable_finger(
  /** Finger index to be disabled   */
  uint8 fing);

/*============================================================================*/
/**
 * This function is top level function to do complete cell config. It
 * takes as parameters the cell index to config and the cell info. This 
 * function blindly copies all the cell configuration received into MCAL
 * DEMOD shadow. It assumes that if there was a prior configuration saved
 * for that cell, the configuration will be overwritten by this new 
 * command.
 *
 * @see mcalwcdma_demod_cell_info_struct_type
 */
/*============================================================================*/
extern void mcalwcdma_demod_cell_config(
  /** Cell index */
  uint8 cell,
  /** Cell config parameters */
  mcalwcdma_demod_cell_config_struct_type *cell_config);

/*===========================================================================
FUNCTION mcalwcdma_demod_set_td_dirty_bit

DESCRIPTION
  This function sets the tdDetDirtyBit and populates it into the cell shadow
  The dirty bit is an indication along with the tdDetEn for FW to perform TxD detection

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
extern void mcalwcdma_demod_set_td_dirty_bit(uint8 cell, boolean tdDetDirtyBit);

/*============================================================================*/
/**
 * This function is top level function to disable a cell. It receives the
 * cell index to be disabled and clears all the information in the MCAL
 * DEMOD shadow. If cell parameter is set to MCALWCDMA_DEMOD_ALL_CELLS_ID,
 * all the cells are disabled. The fingers for this cell will also be disabled.
 *
 * @see MCALWCDMA_DEMOD_ALL_CELLS_ID
 */
/*============================================================================*/
extern void mcalwcdma_demod_disable_cell(
  /** Finger index to be disabled   */
  uint8 cell);

/*============================================================================*/
/**
 * This function commits the finger and cell configuration changes 
 * to MDSP and also asks MDSP to process the combiner and physical 
 * channel that comes from the parameters. It will call MCAL internal 
 * functions to check and transfer the possible configurations to MDSP 
 * and ships a command to MDSP to process the finger and cell configurations.
 *
 */
/*============================================================================*/
extern void mcalwcdma_demod_commit_demod_cfg(
  /** Combiner bitmask that needs to be included to the command
  sent to MDSP */
  uint16 combiner_bmsk,
  /** Physical Channel bitmask that needs to be included to the
  command sent to MDSP  */
  uint32 phychan_bmsk);


/*===========================================================================
 **
 * This function takes an input bmsk and checks to see if any resource in that 
 * bitmask is under pending with fw. If it is pending the ret_val would show that
 * it is pending
 *
 *=========================================================================*/
extern uint32 mcalwcdma_demod_query_demod_cfg_request_state(
  /** demod resource type */
  mcalwcdma_demod_resource_type demod_resource_type,
  /** resource bmsk */
  uint32 demod_resource_bmsk);

/*===========================================================================
 *
 * This function gets the freq common.
 *
 *=========================================================================*/
extern int16 mcalwcdma_demod_get_freqcommon(void);

/*===========================================================================
 *
 * This function gets the freq common.
 *
 *=========================================================================*/
extern int32 mcalwcdma_demod_get_tcxoaccum(void);

/*============================================================================*/
/**
 * This function sends a demod status dump command to the MDSP and returns a
 * structure containing the various combiner counts, TX system time, finger
 * RSSI and positions.  This is a blocking function which will take at least
 * 133 microseconds (512 chips).  The caller will pass a pointer to a status
 * dump result buffer and it will be populated with the status dump data. If
 * the status dump operation fails the return value will be FALSE. Successful
 * status dumps will return TRUE.
 */
/*============================================================================*/
extern boolean mcalwcdma_demod_status_dump(
  /** pointer to a result buffer which will be populated with status dump info */
  mcalwcdma_demod_status_dump_info_struct_type *result_buffer);

/*============================================================================*/
/**
 *  This function updates the FTL accumulator factor and the TTL accumulator
 *  factor in FW.
 */
/*============================================================================*/
extern void mcalwcdma_demod_update_ftl_ttl(
  /** FTL factor to be written into FW */
  uint32 ftl_factor, 
  /** TTL factor to be written into FW */
  uint32 ttl_factor);

/*============================================================================*/
/**
 *  This function sets the td val instead of waiting for the firmware to detect
 */
/*============================================================================*/
extern void mcalwcdma_demod_set_tddet_status(uint8 cell_id, uint8 td_val);

/*============================================================================*/
/**
 *  This function reads the td val that has been detected by firmware for a cell
 * 
 * @return TD status of given cell
 */
/*============================================================================*/
extern uint8 mcalwcdma_demod_get_tddet_status(uint8 cell_id);

/*===========================================================================*/
/**
 * This function reads the td accumulated energy for a cell 
 *
 * @return TD divaccum egy
*/
/*===========================================================================*/
extern uint32 mcalwcdma_demod_get_td_divaccum_egy(uint8 cell_idx);

/*============================================================================*/
/**
 *  This function updates the QFT integration length
 */
/*============================================================================*/
extern void mcalwcdma_demod_set_qft_integration_length(
  /** qft int length */
  uint32 qft_int_len);

/*============================================================================*/
/**
 *  This function updates the qft egy threshold
 */
/*============================================================================*/
extern void mcalwcdma_demod_set_qft_energy_threshold(
  /** qft egy threshold */
  uint32 qft_energy_thresh);

/*============================================================================*/
/**
 *  This function updates the qtt coherent integration length
 */
/*============================================================================*/
extern void mcalwcdma_demod_set_qtt_coherent_integration_length(
  /** qtt coherent integration length */
  uint32 qtt_coh_int_len);

/*============================================================================*/
/**
 *  This function updates teh non coherent integration lenght for qtt
 */
/*============================================================================*/
extern void mcalwcdma_demod_set_qtt_non_coherent_integration_length(
  /** qtt non coherent integration length */
  uint32 qtt_non_coh_int_len);

/*============================================================================*/
/**
 *  This function copies the demod status dump information from the shared memory 
    buffers of the firmware. 
 */
/*============================================================================*/
extern void mcalwcdma_demod_copy_dsd_info(
  /** pointer to a result buffer which will be populated with status dump info */
  mcalwcdma_demod_status_dump_info_struct_type *result_buffer);


/*============================================================================*/
/**
 *  This function updates the td detection threshold to fw. 
 */
/*============================================================================*/
extern void mcalwcdma_demod_set_td_det_thresh(uint32 td_det_thresh);

/*===========================================================================
FUNCTION        MCALWCDMA_DEMOD_UPDATE_COMBINER_THRESHOLD

FILE            mcalwcdma_demod.c

DESCRIPTION      This function checks and updates combiner lock threshold to
                 (-29dB if new_threshold is set to TRUE;)
                 (-27dB if new_threshold is set to FALSE)

DEPENDENCIES    None 

RETURN VALUE    None

SIDE EFFECTS    combiner lock threshold changes in FW.
===========================================================================*/
extern void mcalwcdma_demod_update_combiner_threshold(boolean set_new_threshold);

/*===========================================================================
FUNCTION        MCALWCDMA_DEMOD_UPDATE_FING_POS_DSD_BUFF_DUR_ASSIGN

FILE            mcalwcdma_demod.c

DESCRIPTION       This function updates the DSD Buffer with the finger position during assignment
DEPENDENCIES    None 

RETURN VALUE    None

SIDE EFFECTS    
===========================================================================*/
extern void mcalwcdma_demod_update_fing_pos_dsd_buf_dur_assign(uint8 fing, uint32 pnposCx8);
/*===========================================================================
FUNCTION        MCALWCDMA_DEMOD_REVERT_FING_CFG

FILE            mcalwcdma_demod.c

DESCRIPTION      This function reverts back the prepared fing cfg. Only used in 
                 very specific conditions 
                 

DEPENDENCIES    None 

RETURN VALUE    None

SIDE EFFECTS    combiner lock threshold changes in FW.
===========================================================================*/
extern void mcalwcdma_demod_revert_fing_cfg(uint8 fing);
/*===========================================================================
FUNCTION        MCALWCDMA_DEMOD_FAST_CELL_FINGER_TEARDOWN

DESCRIPTION     This function sends the cell disable to FW.               

DEPENDENCIES    None 

RETURN VALUE    None

SIDE EFFECTS    Cell/fing decfg.
===========================================================================*/
extern void mcalwcdma_demod_fast_cell_finger_teardown(uint8 cell_db_idx);

/*===========================================================================
FUNCTION        mcalwcdma_demod_get_td_val_in_buf

FILE            mcalwcdma_demod.c

DESCRIPTION     This function returns TRUE if TD status is set to 1 in the FW shadow buf.
                 

DEPENDENCIES    None 

RETURN VALUE    Boolean

SIDE EFFECTS    None
===========================================================================*/

extern boolean mcalwcdma_demod_get_td_val_in_buf(uint8 cell_index);

/*===========================================================================
FUNCTION        MCALWCDMA_DEMOD_SET_INIT_RSSI_PCH_SLEEP

DESCRIPTION     This function sets the init rssi of all the fingers to the 
                minimum value of computed init rssi's for a given finger cfg. 

DEPENDENCIES    None 

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
extern void mcalwcdma_demod_set_init_rssi_pch_sleep(void);

#ifdef FEATURE_WCDMA_ANT_SWITCH_SUPPORT
/*===========================================================================
FUNCTION        MCALWCDMA_DEMOD_FING_ANT_SWITCH

FILE            mcalwcdma_demod.c

DESCRIPTION     This function switches the fing ssid in fw sw intf
DEPENDENCIES    None 

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
extern void mcalwcdma_demod_fing_ant_switch(uint8 fing);

/*===========================================================================
FUNCTION        MCALWCDMA_DEMOD_BLOCK_DEMOD_CFG_CMDS

FILE            mcalwcdma_demod.c

DESCRIPTION     This function would set the value of the global mcalwcdma_demod_block_demod_cfg_cmds 
                which would be used to block demod cfg cmds sent to MDSP. 
 
DEPENDENCIES    None 

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
extern void mcalwcdma_demod_block_demod_cfg_cmds(boolean enable);

#endif /* FEATURE_WCDMA_ANT_SWITCH_SUPPORT */

#endif /* MCALWCDMA_DEMOD_H */

