#ifndef L1_UL_CFG_H
#define L1_UL_CFG_H

/*============================================================================
              U P L I N K   C F G   F I L E

DESCRIPTION
  This files contains all necessary macros and definitions for maintainaing
  physical channel configuration database and CCTrCH configuration database.

EXTERNALIZED FUNCTIONS

INTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS  
  

Copyright (c) 2000-2014 by Qualcomm Technologies, Inc.  All Rights Reserved.

============================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/l1/vcs/l1ulcfg.h_v   1.19   14 May 2002 12:49:26   shawnm  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/inc/l1ulcfg.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     --------------------------------------------------------
10/14/13    tsk     Slot formats 5, 5a and 5b are removed as per spec 25.211 v8.
07/24/12    geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
06/08/12    gv      Feature HS RACH Initial code check in
02/01/12    gnk     Nikel Feature cleanup.
07/06/11    sb      Upmerge from offline/mainline
04/28/11    asm     Mainlined FEATURE_WCDMA_TXAGC_DB10 for offline targets
05/31/11    geg     Changes for RRC and L2 api files clean-up wcdma dependencies
04/28/11    sj      upmerge from offline/main
04/25/11    sp      Corrected TPC step value calculation for 0x410C log packet
09/02/10    gv      Code Checkin for CPC DTX
11/18/09    gv      Use db10 instead of db12 units for computation
10/29/08    vc      removed mimo specific delta values from l1_ul_hs_dpcch_beta_hs_info_struct_type.
10/22/08    vc      Added support for mimo delta cqi.
10/01/08    mc      Added support for MIMO UL.
04/17/07    ms      Harq Preamble/Postamble related changes
02/16/07    ms      Added support for Harq Preamble/Postamble.
08/14/06    yiz     Mainline HSUPA code
04/20/06    mc      Added support for HS performance optimization.
08/30/04    gs      Added interface definitions for HSDPA
02/02/04    yiz     Permanently enable FEATURE_REL_99_JUNE_2001
04/09/03    scm     Remove FEATURE_WCDMA_COMP_GF.  Feature always supported.
05/14/02    scm     Use #ifdef FEATURE_WCDMA_COMP_GF around computed gain
                    factor code added in previous rev of this file.
05/06/02    scm     Add boolean ref_tfc_id_valid field to
                    l1_ul_gain_factor_parms_struct_type.  Add
                    ref_tfc_to_real_tfc[] to l1_ul_cctrch_cfg_struct_type,
                    for computed gain factor support.
04/23/02    yiz     Fixed l1_ul_gain_factor_calc_enum_type to comply with
                    QXDM ICD about GF type flag.
01/15/02    yiz     Add macros L1_UL_PCA_VAL_LOG and 
                    L1_UL_TPC_STEP_VAL_LOG to convert PCA and TPC_Step
        values for logging.
12/12/01    yiz     Add field trch_id to l1_ul_semi_static_parms_struct_type
                    for logging.
12/07/01    src/yiz Fixed FEATURE_REL_99_JUNE_2001 ifndef closure.
12/07/01    src     Put the FEATURE_REL_99_JUNE_2001 around another deprecated
                    declaration that was missed the first time around.
12/06/01    yiz     Fixed L1_UL_TPC_STEP vals and L1_UL_PCA vals per MDSP
                    interface document.
12/06/01    src     Updated interfaces for R'99 2001-06 tranistion, enclosing
                    changes within feature FEATURE_REL_99_JUNE_2001 wherever
                    necessary. Featured out l1_ul_pcp_len_enum_type. Changed
                    the type of pcp_len in l1_ul_dpch_pwr_ctl_info_struct_type
                    to a uint8. Added a new member for SRB delay in the same
                    aggregate.
09/14/01    yiz     Fixed missing l1_ prefix in macro L1_UL_TPC_STEP_VAL.
07/19/01    sk      Added num_tfci field in cctrch config, removed 
                    ref_tfc_table from cctrch config.
06/15/01    sk      Added defines for open loop Tx power parameters.
04/30/01    sk      Removed DL AICH from UL Phychan Config structure.
                    Removed *valid* fields in different structures.
                    Reorganized reference TFC table.
04/06/01    sk      Restructured reference TFC Table definition to work
                    with Gurdeep's changes for RRC/UL interface.
02/21/2001  mdl     moved phychan enums and cphy_setup_cnf #defines to
                    l1sapcommon.h
01/15/01    sk      Modified L1_UL_PREAMBLE_P0_MIN and L1_UL_PREAMBLE_P0_MAX
                    to reflect their intent (was reversed earlier)
12/01/2000  mdl     added offset values into CPHY_SETUP_CNF packet status
                    word for UL phychan,
                    changed L1_UL_NUM_PHYCHAN_TYPE to L1_UL_NUM_PHYCHAN
10/23/00    sk      file created.

===========================================================================*/

/* <EJECT>^L */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"
#include "comdef.h"
#include "queue.h"
#include "l1sapcommon.h"

/*===========================================================================

                     DATA TYPE DEFINITIONS

===========================================================================*/
/* Parameters for Open Loop Tx power setting */
/* See 25.331 10.3.6.61, 2000-12, V3.5.0 for details */
#define L1_UL_CPICH_TX_PWR_MAX   (50)  //in dBm
#define L1_UL_CPICH_TX_PWR_MIN   (-10)  //in dBm

#define L1_UL_INTERFERENCE_MAX   (-70) //dBm
#define L1_UL_INTERFERENCE_MIN   (-110) //dBm

#define L1_UL_CONSTANT_VAL_MAX   (-10)
#define L1_UL_CONSTANT_VAL_MIN   (-35)
/* End parameters for Open Loop Tx power setting */

#define L1_UL_PHYCHAN_DPCH_VAL   1
#define L1_UL_PHYCHAN_PRACH_VAL  2
#define L1_UL_PHYCHAN_PCPCH_VAL  3
/* table of physical channel enumeration values */
extern uint8 l1_ul_phychan_val_tab[L1_UL_NUM_PHYCHAN];

/* return uplink physical channel value given its enumeration */
#define L1_UL_PHYCHAN_VAL(chan_enum)  l1_ul_phychan_val_tab[chan_enum]

#ifdef FEATURE_WCDMA_HS_RACH
  /*Invalid value for powerOffsetPp-e valid range is INTEGER   (-5 .  . 10 )*/
#define L1_HS_RACH_INVALID_POWER_OFFSET_PP_E  11
#endif /*FEATURE_WCDMA_HS_RACH*/ 

#define L1_UL_CODING_NONE_VAL             0
#define L1_UL_CODING_CONV_ONE_HALF_VAL    1
#define L1_UL_CODING_CONV_ONE_THIRD_VAL   2
#define L1_UL_CODING_TURBO_VAL            3

/* table of coding type values */
extern uint8 l1_ul_coding_val_tab[L1_UL_NUM_CODING_TYPE];

/* return uplink coding type value given coding enumeration */
#define L1_UL_CODING_VAL(code_typ_enume)  l1_ul_coding_val_tab[code_type_enum]



#define L1_UL_CRC_0_BITS_VAL   0
#define L1_UL_CRC_8_BITS_VAL   8
#define L1_UL_CRC_12_BITS_VAL  12
#define L1_UL_CRC_16_BITS_VAL  16
#define L1_UL_CRC_24_BITS_VAL  24

/* table of uplink crc enumeration values */
extern uint8 l1_ul_crc_val_tab[L1_UL_NUM_CRC];

/* return crc value given its enumeration */
#define L1_UL_CRC_VAL(crc_enum) l1_ul_crc_val_tab[crc_enum]

/* Static RM min and max defines */
#define L1_UL_STATIC_RM_MIN             1
#define L1_UL_STATIC_RM_MAX             256


/* GF range */
#define L1_UL_GF_MIN                    0
#define L1_UL_GF_MAX                    15
/* Min PRACH power offset in dB */
#define L1_UL_PRACH_MIN_PWR_OFFSET      (-5)
/* Max PRACH power offset in dB */
#define L1_UL_PRACH_MAX_PWR_OFFSET      (10)

#define L1_UL_RF_EQ_FILLER_0_VAL    0
#define L1_UL_RF_EQ_FILLER_1_VAL    2
/* table of uplink filler enumeration values */
extern uint8 l1_ul_filler_val_tab[L1_UL_NUM_FILLER_TYPE];

/* return filler value given its enumeration */
#define L1_UL_FILLER_VAL(filler_enum) l1_ul_filler_val_tab[filler_enum]



/* Access Service Classes partition PRACH resources (i.e. signature and access
 * sub-channels) in order to provide different priorities for RACH usage. More
 * than one or all ASCs may be assigned to the same resources. MAC specifies an
 * ASC number for use during PRACH Tx.
 * See ASC Setting - 25.331 V3.5.0 2000-12 10.3.6.6.
 */
/* maximum number of access sub-channels */
#define L1_UL_ACCESS_SUB_CHAN_MAX       12
/* maximum number of preamble signatures */
#define L1_UL_PREAMBLE_SIGNATURE_MAX    16

/* range of access service classes */
#define L1_UL_ASC_MIN                   1


#define L1_UL_PRACH_SF_MIN  L1_SF_32
#define L1_UL_DPCH_SF_MIN   L1_SF_4

/* RACH message lengths */
typedef enum {
  L1_UL_RACH_MSG_LEN_10_MS,
  L1_UL_RACH_MSG_LEN_20_MS,
  L1_UL_NUM_RACH_MSG_LEN
} l1_ul_rach_msg_len_enum_type;
    
#define L1_UL_RACH_MSG_LEN_10_MS_VAL  0
#define L1_UL_RACH_MSG_LEN_20_MS_VAL  1

/* table of RACH message length values */
extern uint8 l1_ul_rach_msg_len_val_tab[L1_UL_NUM_RACH_MSG_LEN];

/* return RACH message length value given its enumeration */
#define L1_UL_RACH_MSG_LEN_VAL(rach_msg_len_enum)  \
  l1_ul_rach_msg_len_val_tab[rach_msg_len_enum]

/* PRACH Scrambling Code range */
#define L1_UL_PRACH_SC_NUM_MIN          0
#define L1_UL_PRACH_SC_NUM_MAX          15
/* UL puncturing in steps of 0.04 between 0.4 and 1, or
 * (40..100) in steps of 4 - to avoid fixed point calculations.
 */
#define L1_UL_PUNCT_MIN                 40
#define L1_UL_PUNCT_MAX                 100
/* range of preamble power offsets in dB */
#define L1_UL_PREAMBLE_P0_MIN           1 
#define L1_UL_PREAMBLE_P0_MAX           8
/* maximum number of preamble transmissions */
#define L1_UL_PREAMBLE_TX_MIN           1 
#define L1_UL_PREAMBLE_TX_MAX           64  

#define L1_UL_FBI_LEN_0_VAL    0
#define L1_UL_FBI_LEN_1_VAL    1

/* table of FBI bit sizes */
extern uint8 l1_ul_fbi_len_val[L1_UL_NUM_FBI_BITS];

/* return AICH TT value given its enumeration */
#define L1_UL_FBI_LEN_VAL(fbi_len_enum)  l1_ul_fbi_len_val_tab[fbi_len_enum]



// 1 and 2 dB respectively
#define L1_UL_TPC_STEP_ONE_VAL   10
#define L1_UL_TPC_STEP_TWO_VAL   20

/* table of TPC step sizes */
extern uint8 l1_ul_tpc_step_val_tab[L1_UL_NUM_TPC_STEP_SIZE];

/* return TPC step value given its enumeration */
#define L1_UL_TPC_STEP_VAL(tpc_step_enum)  l1_ul_tpc_step_val_tab[tpc_step_enum]

/* return TPC step value for logging given its enumeration */
#define L1_UL_TPC_STEP_VAL_LOG(tpc_step_enum)  \
  (L1_UL_TPC_STEP_VAL(tpc_step_enum) / L1_UL_TPC_STEP_ONE_VAL)



#define L1_UL_PCA_ONE_VAL   1
#define L1_UL_PCA_TWO_VAL   2

/* table of PCA values */
extern uint8 l1_ul_pca_val_tab[L1_UL_NUM_PCA];

/* return PCA value given its enumeration */
#define L1_UL_PCA_VAL(pca_enum)  pca_enum

/* return PCA value for logging given its enumeration */
#define L1_UL_PCA_VAL_LOG(pca_enum)  l1_ul_pca_val_tab[pca_enum]

/* Range of DPCCH power offsets */
#define L1_UL_DPCCH_PWR_OFF_MIN   (-164)
#define L1_UL_DPCCH_PWR_OFF_MAX   (-6) 
/* Number of DPDCHs */
/*Default value is 1. Number of DPDCH is 1 in HANDOVER TO UTRAN COMMAND. 
  Value 0 may only be used if target configuration has no uplink DPDCH.in Rel6
The change is done as part of the EUL implementation*/
#define L1_UL_DPDCH_MIN            0
#define L1_UL_DPDCH_MAX            6



/* Min Tx Power in dBm */
#define L1_UL_MAX_TX_PWR_MIN          (-50)
/* Max Tx Power in dBm */
#define L1_UL_MAX_TX_PWR_MAX            33


/* --------------------------------------------- */
/* HSDPA UL physical channel related information */
/* --------------------------------------------- */


#endif

