#ifndef MCALWCDMA_CONST_H
#define MCALWCDMA_CONST_H

/*============================================================================*/
/** @file  
 * This module has common constant related to MCAL WCDMA. Example of this is
 * MAX CFN/SFN count, number of HS subframe in 10 ms radio frame, PN circle
 * chip count is various units etc.
 */
/*============================================================================*/

/*============================================================================
Copyright (c) 2006 - 2012 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
============================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/inc/mcalwcdma_const.h#1 $
$DateTime: 2016/12/13 08:00:12 $
$Author: mplcsds1 $

when        who     what, where, why
--------    ---     -------------------------------------------------------- 
11/13/14    kr      Corrt JO featurization
11/10/14    sl      Code changes for new f/w log packet 0x41C4 V1
10/17/13    bs      Preliminary 3C Support
07/10/13    kcm     Reverted the change to reduce the mac num of cells on primary carrier by 1
07/10/13    kcm     Added new macro to get number of cells supported on sec carrier
07/05/13    sad     Reduced the max number of cell in primary carrier by 1
01/09/13    pr      Reverted Change of Cell Idx start for C1 from Idx8 back to Idx7.
11/20/12    pr      Change Cell Idx start for C1 from Idx8.
11/19/12    pr      Changes for max num of fingers to match FW.
10/22/12    pr      Changed max num of fingers to 2.
06/12/12    abs     Hardcoded max num of fingers to be 1.
02/12/12    hk      Nikel feature cleanup
02/12/12    hk      Nikel feature cleanup
01/19/12    zr      Mainlined FEATURE_MCAL_WCDMA and FEATURE_WCDMA_GENESIS_SW
09/13/11    kcm     Reverting the change of increasing the MAX_NUM_CELLS by 1
                    for Nikel.
09/02/11    kcm     Increased the MCALWCDMA_MAX_NUM_CELLS by 1 for Nikel to 
                    workaround a hole in the FW implementation of cell_idx
04/23/10    ksr     KW Fixes
04/21/10    ksr     New define to return Max primary Carrier Cells
02/26/10    rvs     Fixes for Mustang.
02/27/10    hk      Added new macros for number of fingers on primary and 
                    secondary carriers
11/06/09    hk      Replaced the cell/finger definitions with FW constants
10/24/09    ms      Changed value of MCALWCDMA_MAX_NUM_FINGERS
07/16/09    hk      Reorganized the code to remove INC/SRC violations
11/11/08    ks      Max number of finger related changes
10/20/08    ks      increased #of fingers to 6
09/29/08    ks      reduced #of fingers to 4
09/26/08    vc      reduced #of Fingers to 2 if FEATURE_WCDMA_MIMO is enabled.
9/26/08     vb      reverted previous change
09/26/08    vc      reduced #of Fingers to 2 if FEATURE_WCDMA_MIMO is enabled.
09/20/08    vb      reduced #of fingers to 4
09/18/08    saik    Imported constants from msmdsp.h file under feature FEATURE_WFW_SW_INTERFACE 
                    to support compilation on Mustang, in absence of msmdsp.h file
08/27/08    ks      increased number of fingers to 12
01/23/08    mc      Introduced in mainline from MBMS dev branch.
03/03/07    mc      Sync up for MBMS phase2A integration
12/12/06    mc      Synced up for MBMS demo - 3042 baseline
11/29/06    mc      Initial version for MBMS demo integration.
===========================================================================*/

/* ==========================================================================
** Includes and Public Data Declarations
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */

//These constants are being added in order to be able to compile Mustang build
// M8200ASKCAAAFD99102 without the msmdps.h file.

#include "wcdma_variation.h"
#include "comdef.h"
#include "wfw_demod_intf.h"

/*
 * rssiLockStatus flags
 */
#define BIT_LOCK_DETECT_STATE  (0)
#define BIT_TT_LOCK            (1)
#define BIT_COMB_LOCK          (2)
#define BIT_POWER_LOCK         (3)

/*
 * rssiLockControl flags
 */
#define BIT_TT_ENABLE          (1)
#define BIT_COMB_LOCK_ENABLE   (2)
#define BIT_POWER_LOCK_ENABLE  (3)
#define BIT_SET_LOCK           (4)
#define BIT_RESET_LOCK         (5)


 
/*=========================================================================*/
/* Miscellanous Other Useful Definitions                                   */
/*=========================================================================*/
#define MDSP_WCDMA_DESKEW_DEPTH_IN_CHIPS     0
#define MDSP_WCDMA_DESKEW_DEPTH_IN_256_CHIP_UINTS \
          (MDSP_WCDMA_DESKEW_DEPTH_IN_CHIPS / 256)

/*This value is MDSP_WCDMA_DESKEW_DEPTH_IN_CHIPS*8 */
#define MDSP_WCDMA_DESKEW_DEPTH_IN_CX8 (MDSP_WCDMA_DESKEW_DEPTH_IN_CHIPS * 8)

#define MDSP_WCDMA_FFE_BUFFER_LENGTH_IN_CX8 0


/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/** Maximum CFN count */
#define MCALWCDMA_MAX_CFN_COUNT 256

/** Total number of fingers supported by hardware */
#define MCALWCDMA_MAX_NUM_FINGERS  WFW_DEMOD_NUM_MAX_FINGERS

/** Max number of cells supported  */
#define MCALWCDMA_MAX_NUM_CELLS   WFW_DEMOD_NUM_MAX_CELLS

/** Max number of cells supported on Primary Carrier */
#define MCALWCDMA_MAX_NUM_CELLS_PRI_CARRIER   WFW_DEMOD_NUM_MAX_CELLS_PRI_CARRIER

#ifdef FEATURE_WCDMA_DC_HSDPA
/** Max number of cells supported on Primary Carrier */
#define MCALWCDMA_MAX_NUM_CELLS_SEC_CARRIER   WFW_DEMOD_NUM_MAX_CELLS_SEC_CARRIER
#endif

#ifdef FEATURE_WCDMA_3C_HSDPA
#define MCALWCDMA_MAX_NUM_CELLS_TER_CARRIER   WFW_DEMOD_NUM_MAX_CELLS_TER_CARRIER
#endif

/** Total number of combiners for status dump  */
#define MCALWCDMA_MAX_NUM_COMBINER             4

#define MCALWCDMA_PICH_PI_BITS_PER_FRAME 288

#define MCALWCDMA_PICH_PI_PER_FRAME_18 18
#define MCALWCDMA_PICH_PI_PER_FRAME_36 36
#define MCALWCDMA_PICH_PI_PER_FRAME_72 72
#define MCALWCDMA_PICH_PI_PER_FRAME_144 144

#define MCALWCDMA_INVALID_NV_MASK                      0xFFFFFFFF

#define MCALWCDMA_INVALID_PSC_SSC                      0xFFFF

#define MCALWCDMA_INVALID_CFN                          0xFFFF

#define MCALWCDMA_INVALID_ECIO                         0xFFFF

#define MCALWCDMA_INVALID_START_WIN                    0xFFFFFFFF


#define DL_MAX_NUM_CELLS MCALWCDMA_MAX_NUM_CELLS
#define MSMHW_MAX_NUM_FINGERS MCALWCDMA_MAX_NUM_FINGERS
#define DL_MAX_NUM_FINGERS MSMHW_MAX_NUM_FINGERS

#ifdef FEATURE_WCDMA_JOLOKIA_MODEM
 #define WCDMA_NUM_TIMELINE_CHECKS WFW_NUM_TIMELINE_CHECKS
#endif /*FEATURE_WCDMA_JOLOKIA_MODEM */

#define DL_MAX_NUM_FINGERS_PRI_CARRIER WFW_DEMOD_NUM_MAX_FINGERS_PRI_CARRIER

#ifdef FEATURE_WCDMA_DC_HSDPA
#define DL_MAX_NUM_FINGERS_SEC_CARRIER WFW_DEMOD_NUM_MAX_FINGERS_SEC_CARRIER
#ifdef FEATURE_WCDMA_3C_HSDPA
#define DL_MAX_NUM_FINGERS_3RD_CARRIER (WFW_DEMOD_NUM_MAX_FINGERS - (WFW_DEMOD_NUM_MAX_FINGERS_PRI_CARRIER + WFW_DEMOD_NUM_MAX_FINGERS_SEC_CARRIER)) //TO DO WFW_DEMOD_NUM_MAX_FINGERS_TER_CARRIER
#endif /*FEATURE_WCDMA_3C_HSDPA*/
#endif /*FEATURE_WCDMA_DC_HSDPA*/

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

/* Call back function type declaration */
/* ----------------------------------- */

/* Enum type declaration */
/* --------------------- */

/* Structure type declaration */
/* -------------------------- */

/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */

/* =======================================================================
**                        Function Declarations
** ======================================================================= */
#endif /* MCALWCDMA_CONST_H */
