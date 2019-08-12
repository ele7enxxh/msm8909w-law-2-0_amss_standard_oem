#ifndef MCALWCDMA_EDLI_H
#define MCALWCDMA_EDLI_H

/*============================================================================*/
/** @file  
 * This module has definition and declaration related to MCAL WCDMA
 * EDL (EUL Downlink) module APIs. This header filer is only intended to
 *  be included in MCAL WCDMA EDL related modules only.
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/src/mcalwcdma_edli.h#1 $
$DateTime: 2016/12/13 08:00:12 $
$Author: mplcsds1 $

when        who     what, where, why
--------    ---     --------------------------------------------------------
06/16/14   ash    Delay calling edl_cfg_evt_handler until DL processing is done for FACH->DCH Seamless
10/14/13   pr/sr  Channel config optimizations
05/22/13   oh     Adding DC-HSUPA framework support
08/23/10   ks     Removed inclusion of mdsp header for Genesis (featurized)
08/04/09   gnk    Removing the obsolete features FEATURE_WCDMA_HSUPA_EDL_TWO_COMBINER
07/22/09   gnk    Mainlining the feature FEATURE_WCDMA_OFFLINE_DEMOD_SUPPORT
11/16/08   oh     Changing the config event offset to 40.
11/16/08   oh     Push frame bdry event to next frame if the config cmd is processed late.
10/03/08   hk     Bringing in 7k Mailine fixes
09/25/08   oh     Modifying the EDL channel config window as per DPCH
08/13/08   gv     EUL fixes for stop and other clean up.
01/25/08   vsr    Checking in Enhanced DL files onto mainline
08/08/07   gv/vp  Created initial version
===========================================================================*/

/* ==========================================================================
** Includes and Public Data Declarations
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */
#include "comdef.h"
#include "dsm.h"
#include "mcalwcdma_common.h"

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
/* The start and end of channel config window are placed at 30th and 120th ticks
 * respectively */
#define MCALWCDMA_E_DL_CFG_WINDOW_START (20)

#define MCALWCDMA_E_DL_CFG_WINDOW_END (120)

/* This value is the offset of FIQ handler + 2 ticks. This will ensure that we never switch the 
** config and also update SC indices very close to FIQ event*/
#define MCALWCDMA_E_DL_DELAY_CFG_DBSWITCH_OFFSET 86

/* Special stop sub FN value to indicate to FW that we are requesting an
** immediate stop*/
#define MCALWCDMA_E_DL_IMMEDIATE_STOP_SUB_FN_VAL 0xF800

#define MCALWCDMA_E_DL_OFFSET_FOR_CFG_EVT (60)

#define MCALWCDMA_E_DL_OFFSET_FOR_REG_UPD_EVT (0)

#define MCALWCDMA_E_DL_OFFSET_FOR_FRM_EVT_ON_DPCH (0)

#define MCALWCDMA_E_DL_CFG_PENDING_OFFSET 30

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */
  /* Macro to update the ch to DPCH frame alignment flag */
  #define MCALWCDMA_E_DL_UPDATE_CM_FRAME_ALIGNMENT_INFO(cfn,start_sub_fn,ch_cm_info)\
    ch_cm_info.ch2dpch_cfn_subfn_aligned = (((cfn & 0x1) == (start_sub_fn & 0x1)) ? TRUE : FALSE)


/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */


/* =======================================================================
**                        Function Declarations
** ======================================================================= */

/*===========================================================================

FUNCTION        MCALWCDMA_E_DL_CALC_SUB_FN_AND_PATCH

DESCRIPTION     This function computes the start sub FN and final aub FN
                and patches it to the MDSP config variable that will be transfered 
                to the shadow before the config command is sent.

DEPENDENCIES    Unknown

RETURN VALUE    None

SIDE EFFECTS    Unknown

===========================================================================*/
extern void mcalwcdma_e_dl_calc_sub_fn_and_patch(wl1_ul_carr_id_enum_type carr_idx);

/*===========================================================================

FUNCTION        MCALWCDMA_E_DL_CALC_START_FINAL_FN

DESCRIPTION     This function calculates the start and final fn for a
                a particular channel on edl start, stop or reconfig. The
                calculation is based on the action being performed on the 
                channel and passed in start_sub_fn and stop_sub_fn values.

DEPENDENCIES    Unknown

RETURN VALUE    None

SIDE EFFECTS    Unknown

===========================================================================*/

extern void mcalwcdma_e_dl_calc_start_final_sub_fn (uint16 cfn, mcalwcdma_e_dl_chan_action_enum_type action, 
                              uint16 * start_sub_fn_ptr, 
                              uint16 * final_sub_fn_ptr,
                              uint16 num_sub_frms_offset
                              );

/*===========================================================================

FUNCTION        MCALWCDMA_E_DL_SET_CONFIG_EVENT

DESCRIPTION     This function set the register update event. This 
                event fires on the 20th tick on the DPCH frame.

DEPENDENCIES    Unknown

RETURN VALUE    None

SIDE EFFECTS    Unknown

===========================================================================*/
extern void mcalwcdma_e_dl_set_config_event(void);

/*===========================================================================

FUNCTION        MCALWCDMA_E_DL_SET_REG_UPD_EVENT

DESCRIPTION     This function set the register update event. This 
                event fires on the DPCH frame boundary after the config
                command has been sent.

DEPENDENCIES    Unknown

RETURN VALUE    None

SIDE EFFECTS    Unknown

===========================================================================*/
extern void mcalwcdma_e_dl_set_reg_upd_event(void);

/*===========================================================================

FUNCTION        MCAL_E_DL_SET_FRAME_EVENT

DESCRIPTION     This function sets the EDL frame boundary event
                based on the farthest channel to DPCH offset and other
                few conditions.

DEPENDENCIES    Unknown

RETURN VALUE    None

SIDE EFFECTS    Unknown

===========================================================================*/
extern void mcalwcdma_e_dl_set_frame_event(uint16 offset);

/*===========================================================================

FUNCTION        MCAL_E_DL_REG_UPD_HANDLER

DESCRIPTION     This function updates the register contents in the
                firmware to support the 2 combiner approach. The EUL
                CELL TT register is updated in this handler. This function
                also sets the EDL frame event.

DEPENDENCIES    Unknown

RETURN VALUE    None

SIDE EFFECTS    Unknown

===========================================================================*/
extern void mcalwcdma_e_dl_reg_upd_handler(void);

/*===========================================================================

FUNCTION        MCAL_E_DL_FRM_EVT_HANDLER

DESCRIPTION     This function calls the frame event post processing
                function that switches the EDL config pages and 
                posts a local command to triggers the update of EUL DL
                structures.

DEPENDENCIES    Unknown

RETURN VALUE    None

SIDE EFFECTS    Unknown

===========================================================================*/
extern void mcalwcdma_e_dl_frm_evt_handler(void);

/*===========================================================================

FUNCTION MCALWCDMA_E_DL_CFG_EVT_HANDLER

DESCRIPTION     This is the config event handler. This function sends the
                   channel config command to mdsp.

DEPENDENCIES     None

RETURN VALUE    None

SIDE EFFECTS    None

===========================================================================*/
extern void mcalwcdma_e_dl_cfg_evt_handler(void);

#endif

