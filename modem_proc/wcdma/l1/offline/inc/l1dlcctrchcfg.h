#ifndef L1_DL_CCTRCH_CFG_H
#define L1_DL_CCTRCH_CFG_H

/*============================================================================
                     L 1 _ D L _ C C T R C H _ C F G _ H
DESCRIPTION
  
EXTERNALIZED FUNCTIONS

INTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS  

  Copyright (c) 2007 - 2008, 2011 - 2012 by Qualcomm Technologies, Inc.  All Rights Reserved.

============================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/l1/vcs/l1dlcctrchcfg.h_v   1.11   01 May 2002 09:23:14   yshi  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/inc/l1dlcctrchcfg.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     -------------------------------------------------------- 
07/24/12    geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
06/07/12    vs      MBMS feature cleanup.
03/16/12    ms      Added support for FEATURE_WCDMA_HS_FACH
01/30/12    vs      Feature cleanup.
01/19/12    zr      Mainlined FEATURE_MCAL_WCDMA 
05/31/11    geg     Changes for RRC and L2 api files clean-up wcdma dependencies
03/05/11    ms      Moved L1_DL_HS_DSCH_HMS43200 to the end of the list
12/05/08    yh      Use uecomdef.h for WPLT compilation.
10/30/08    ms      Removed enum mac_hs_e_type since it is defined in RRC
10/03/08    hk      Bringing in 7k Mailine fixes
09/24/08    mc      Added Mimo i/f params
09/16/08    ms      Changed L1_DL_MAX_LOGICAL_CHANNEL from 15 to 16.
                    Added L1_DL_NON_DTX_MODE_LOGICAL_CHANNEL with value 15.
07/30/08    mc      Added support for MIMO configuration.
06/19/08    ms      Featurized RRC-L1 interface under FEATURE_WCDMA_REL7
                    FEATURE_HSDPA_MAC_EHS & FEATURE_WCDMA_64QAM.
06/19/08    ms      Featurized enums under FEATURE_WCDMA_PLT.
06/10/08    ms      Added support for 64QAM & octet-aligned TFRI table.
10/24/07    vsr     Mainlining MBMS and Enhanced DL changes 
09/28/06    mc      Added MCAL decoder support under feature 
                    FEATURE_MCAL_WCDMA
03/22/06    gs      Added non_tm_rb in transport channel info as additional
                    information for loopback mode.
08/21/05    gs      Added macro L1_DEC_MAX_TB_PER_TF
03/22/05    scm     Set L1_CCTRCH_ID_COUNT to 6 to match RRC.
03/17/05    mc      Removed lint warnings and errors related to FEATURE_HSDPA.
08/30/04    gs      Added interface definitions for HSDPA
03/04/03    gs      Added include file msm.h for including MSM HW target
                    related features to have effect just by including this
                    file in source file.
05/01/02   yus      Changed FEATURE_MSM5200c to MSMHW_DEC.
02/28/02    gs      1. Merged the code changes for MSM5200C. It has different
                       number of CCTrCh table available.

08/21/01    gs      1. Added the structure member in loopback_mode in
                       l1_dl_trch_info_struct_type.

08/14/01    gs      1. Added the structure member in bler_quality_val in
                       l1_dl_trch_info_struct_type.

04/23/01    gs      1. Removed the feature macro from this file

04/19/01    gs      1. Defined the macro for the features
                       FEATURE_RRC_L1_INF_USE_CTFC
                       and FEATURE_USE_ONLY_ALWED_TFI_FIX_POS_RM.
                    2. Defined macro L1_MAX_TF_PER_TRCH (=32)
                    3. Added the structure member allowed of boolean type to
                       structure l1_dl_tf_info_struct_type.
                    4. Changed the array size of number of TF in a TrCh
                       as defined in 25.331 in chapter type constraint value.
                    5. Added the conditional structure member in ctfc entry
                       info structure for TFI array pointer.
                    6. Changed the type of member last_ctfc_array_index to
                       int16 in structure l1_dl_ctfc_info_struct_type. The
                       negative value indicate no valid CTFC entry.

03/08/01    gs      Moved the DL TrCh reference type BF to l1sapcommon.h

12/08/00    gs      1. Changed the TF pointer to the starting element in the
                       TF array to array of pointers to the TF structure.
                    2. Changed the TrCh pointer to the starting element in
                       the TF array to array of pointers to the TrCh structure.
                    3. Changed the CTFC entry pointer to the starting element
                       in the CTFC entry array to array of pointers to the
                       CTFC entry structure.
                    4. Added the TrCh enable BF. This is applicable to the
                       PCH_FACH type only.

11/08/00    gs      1. Removed the ciphering key req related macros, structure,
                       enum to l1sapcommon.h
                    2. Corrected the macros to get the TTI and CRC enum from
                       their values.

11/01/00    sh      Removed underscores from file name
===========================================================================*/

/* ==========================================================================
** Includes and Public Data Declarations
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */

#include "wcdma_variation.h"
#include "comdef.h"
#include "msm.h"
#include "l1sapcommon.h"
#include "uecomdef.h"

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/* Maximum CCTrCh Id counts that can be used by RRC or L1 manager */
#define L1_CCTRCH_ID_COUNT 6

/* macro to convert the CRC value to CRC enum type */
#define L1_CRC_VAL_TO_ENUM(crc_val) \
  ((l1_crc_length_enum_type) ((crc_val == 0) ? L1_CRC_LENGTH_0 : \
   ((crc_val == 8) || (crc_val == 12) || (crc_val == 16)) ? \
   ((l1_crc_length_enum_type) ((crc_val / 4) - 1)) : L1_CRC_LENGTH_24))

/* macro to convert the TTI value to TTI enum type */
#define L1_TTI_VAL_TO_ENUM(tti_val) \
  ((l1_tti_enum_type) ((tti_val == 10) ? L1_TTI_10MS : \
    (tti_val == 20) ? L1_TTI_20MS : \
    (tti_val == 40) ? L1_TTI_40MS : L1_TTI_80MS))

#ifdef FEATURE_WCDMA_HS_FACH
/* Bit mask to indicate Common H-RNTI Valid */
#define L1_COMMON_H_RNTI_VALID 0x01

/* Bit mask to indicate BCCH H-RNTI Valid */
#define L1_BCCH_H_RNTI_VALID 0x02

/* Bit mask to indicate dedicated H-RNTI Valid */
#define L1_DEDICATED_H_RNTI_VALID 0x04

#define CPHY_HS_CFG_HRNTI_BMSK_IS_HRNTI_INCL(mask, cfg) \
  (((mask) & L1_##cfg##_H_RNTI_VALID) ? TRUE : FALSE)
#endif /* FEATURE_WCDMA_HS_FACH */

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

/* ------------------------------------------------- */
/* HSDPA DL TrCh channel HS DSCH related information */
/* ------------------------------------------------- */


/* maximum MAC-d PDU size */
#define L1_DL_MACD_MAX_PDU_SZ 5000


#define L1_DL_NON_DTX_MODE_LOGICAL_CHANNEL 15


#endif /* L1_DL_CCTRCH_CFG_H */
