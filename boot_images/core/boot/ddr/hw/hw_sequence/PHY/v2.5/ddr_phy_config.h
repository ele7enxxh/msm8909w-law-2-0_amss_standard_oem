#ifndef _DDR_PHY_CONFIG_H_
#define _DDR_PHY_CONFIG_H_
/*=============================================================================

                                DDR PARAMETERS
                                Header File
GENERAL DESCRIPTION
TBD

               Copyright 2013-2014 Qualcomm Technologies Incorporated
                              All Rights Reserved
                     Qualcomm Confidential and Proprietary
					 
					 
 All data and information contained in or disclosed by this document are confidential and
 proprietary information of Qualcomm Technologies Incorporated, and all rights therein are expressly
 reserved. By accepting this material, the recipient agrees that this material and the
 information contained therein are held in confidence and in trust and will not be used,
 copied, reproduced in whole or in part, nor its contents revealed in any manner to others
 without the express written permission of Qualcomm Technologies Incorporated.

 This technology was exported from the United States in accordance with the Export
 Administration Regulations. Diversion contrary to U. S. law prohibited.
=============================================================================*/

/*==============================================================================
                                EDIT HISTORY

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/boot/ddr/hw/hw_sequence/PHY/v2.5/ddr_phy_config.h#1 $
$DateTime: 2015/09/01 00:30:35 $
$Author: pwbldsvc $
================================================================================
when       who           what, where, why
--------   --------      -------------------------------------------------------
10/28/14   sk            Changes for 8952 Chipset
07/07/14   amoussav      Initial version.
==============================================================================*/

/*==============================================================================
                               INCLUDE FILES
==============================================================================*/

#include "ddr_target.h"
#include "ddrss_seq_hwiobase.h"
#include "ddrss_seq_hwioreg.h"
// for SW compilation to go through in generic way we need to include ddr_target.h in this file , so that the #define of PC32,PC64 , PCLP64 will be picked correctly from SW .
// for compilation independent of SW file , we just need to enable them here according to the chipset .

//*************************************************************************************************************************************************
//common definitions
//*************************************************************************************************************************************************
//#define ENABLE_PRINTF
#define WRLVL_TOTAL_FREQ           7

#define BYTE_PER_DQ_CNT            2
#define JCPLL_CNT                  2
#define DCC_CNT                    2
#define REN_CFG_CNT                7
#define WRLVL_CFG_CNT              7
#define MAX_WRLVL_CDC_DLY          0x7F

#define BYTE_SIZE                  8
#define WORD_SIZE                  32
#define PHY_SIZE                   16
#define WORD_PER_BUS               (BUS_SIZE  / WORD_SIZE)
#define PHY_PER_BUS                (BUS_SIZE  / PHY_SIZE)
#define BYTE_PER_BUS               (BUS_SIZE  / BYTE_SIZE)
#define BYTE_PER_WORD              (WORD_SIZE / BYTE_SIZE)
#define BYTE_PER_PHY               (PHY_SIZE  / BYTE_SIZE)

#define DESKEW_CDC_INITIAL_VALUE    7
#define DM_CSR_INITIAL              0x7777
#define PER_BIT_CSR_INITIAL         0x77777777


//*************************************************************************************************************************************************
// product spesific definitions
//l*************************************************************************************************************************************************
/* Luxor */
#if MPQ8092
#include "ddr_phy_config_8092.h"
#endif

/* Trahira */
#if FSM9900
#include "ddr_phy_config_9900.h"
#endif


/* Gandalf */
#if APQ8084
#include "ddr_phy_config_8084.h"
#endif

/* SHERE */
#if MSM8936
#include "ddr_phy_config_8936.h"
#endif
//#define DESKEW_CDC_INITIAL_VALUE 0
//#define DM_CSR_INITIAL 0x0
//#define PER_BIT_CSR_INITIAL 0x0

// ***********************************************************************
/// -------------------------------------------------------------
//  LPDDR3 CA : CH0/CH1 Recommended values from SW CAlc 
/// -------------------------------------------------------------
// ***********************************************************************
#define CA_PAD_CFG0_LP_REC_VALUE     0x200FF140
#define CA_PAD_CFG1_LP_REC_VALUE     0x200F00C0
#define CA_PAD_CFG2_LP_REC_VALUE     0x10000000
#define CA_PAD_CFG3_LP_REC_VALUE     0x000001E0
#define CA_PAD_CFG4_LP_REC_VALUE     0x000001E0
#define CA_PAD_CFG5_LP_REC_VALUE     0x0

// ***********************************************************************
/// -------------------------------------------------------------
//  LPDDR3 DQ : CH0/CH1 Recommended values from SW CAlc
/// -------------------------------------------------------------
// ***********************************************************************
#define DQ_PAD_CFG0_LP_REC_VALUE 0x270FF140
#define DQ_PAD_CFG1_LP_REC_VALUE 0x170F00C0
#define DQ_PAD_CFG2_LP_REC_VALUE 0x10010000
#define DQ_PAD_CFG3_LP_REC_VALUE 0x0

// ***********************************************************************
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  PCDDR3 CA : CH0/CH1 Recommended values from SW CAlc
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ***********************************************************************
// CH0 Recommended value for CA
#define CH0_CA_PAD_CFG0_PC_REC_VALUE 0x2000F170
#define CH0_CA_PAD_CFG1_PC_REC_VALUE 0x200000F0
#define CH0_CA_PAD_CFG2_PC_REC_VALUE 0x10000000
// #define CH0_CA_PAD_CFG3_PC_REC_VALUE 0x1103C0
// #define CH0_CA_PAD_CFG4_PC_REC_VALUE 0xFFFFFFFF
#define CH0_CA_PAD_CFG5_PC_REC_VALUE 0x0

// CH1 Recommended value for CA
#define CH1_CA_PAD_CFG0_PC_REC_VALUE CH0_CA_PAD_CFG0_PC_REC_VALUE
#define CH1_CA_PAD_CFG1_PC_REC_VALUE CH0_CA_PAD_CFG1_PC_REC_VALUE
#define CH1_CA_PAD_CFG2_PC_REC_VALUE CH0_CA_PAD_CFG2_PC_REC_VALUE
// #define CH1_CA_PAD_CFG3_PC_REC_VALUE CH0_CA_PAD_CFG3_PC_REC_VALUE
// #define CH1_CA_PAD_CFG4_PC_REC_VALUE CH0_CA_PAD_CFG4_PC_REC_VALUE
#define CH1_CA_PAD_CFG5_PC_REC_VALUE CH0_CA_PAD_CFG5_PC_REC_VALUE

// ***********************************************************************
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// PCDDR3 DQ : CH0/CH1 Recommended values from SW CAlc
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ***********************************************************************
// V1.1
#define DQ_PAD_CFG0_PC_REC_VALUE_V1P1 0x370FF150
#define DQ_PAD_CFG1_PC_REC_VALUE_V1P1 0x1F0F00D0
#define DQ_PAD_CFG2_PC_REC_VALUE_V1P1 0x10010000
#define DQ_PAD_CFG3_PC_REC_VALUE_V1P1 0x0



/*==============================================================================
                                  TYPES
==============================================================================*/
/*NA*/



/*==============================================================================
                                  FUNCTIONS
==============================================================================*/
/*NA*/
#endif //_DDR_PHY_CONFIG_H_
