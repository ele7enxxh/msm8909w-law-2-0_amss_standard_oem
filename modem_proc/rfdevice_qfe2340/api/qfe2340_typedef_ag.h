#ifndef QFE2340_TYPEDEF_AG_H
#define QFE2340_TYPEDEF_AG_H
/*
  WARNING: This file is auto-generated.

  Generated at:    Tue May 27 15:44:56 2014
  Generated using: rffe_autogen.pl
  Generated from:  Rev_2.20 of the QFE2340 SBI spreadsheet
*/

/*!
  @file
  qfe2340_typedef_ag.h

  @brief
  QFE2340 Data Type Definition autogen header 

  @details
  This file is auto-generated and it contains the QFE2340 Data Type Definition RFFE data type 
  to support the interaction with the QUALCOMM QFE2340 chip

  @addtogroup QFE2340 Data Type Definition
  @{
*/

/*=============================================================================

Copyright (c) 2013,2014 by QUALCOMM Technologies, Inc.  All Rights Reserved.

Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. 
  Government. Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  QUALCOMM Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of QUALCOMM Technologies Incorporated.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_qfe2340/api/qfe2340_typedef_ag.h#1 $
  
=============================================================================*/
  
/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include "comdef.h"
#include "qfe2340_typedef.h"

typedef enum
{
  QFE2330_DEVICE = 1,
  QFE2340_DEVICE = 2,
}qfe2340_modules_type;

typedef enum
{
  QFE2340_HB1_IN = 1,
  QFE2340_HB2_IN = 2,
  QFE2340_HB1_TX = 3,
  QFE2340_HB2_TRX = 4,
  QFE2340_HB3_TRX = 5,
  QFE2340_HB4_TRX = 6,
  QFE2340_INVALID_PORT = 0xFF,
}qfe2340_portnum_type;

typedef enum
{
  QFE2340_LTEFDD_PA_BAND7_HB1_IN,
  QFE2340_LTEFDD_PA_BAND7_HB2_IN,
  QFE2340_LTEFDD_PA_BAND30_HB1_IN,
  QFE2340_LTEFDD_PA_BAND30_HB2_IN,
  QFE2340_LTEFDD_PA_BAND_NUM,
  QFE2340_LTEFDD_PA_BAND_INVALID,
}qfe2340_ltefdd_pa_port_data_type;

typedef enum
{
  QFE2340_LTETDD_PA_BAND38_HB1_IN,
  QFE2340_LTETDD_PA_BAND38_HB2_IN,
  QFE2340_LTETDD_PA_BAND40_HB1_IN,
  QFE2340_LTETDD_PA_BAND40_HB2_IN,
  QFE2340_LTETDD_PA_BAND40_B_HB1_IN,
  QFE2340_LTETDD_PA_BAND40_B_HB2_IN,
  QFE2340_LTETDD_PA_BAND41_HB1_IN,
  QFE2340_LTETDD_PA_BAND41_HB2_IN,
  QFE2340_LTETDD_PA_BAND41_B_HB1_IN,
  QFE2340_LTETDD_PA_BAND41_B_HB2_IN,
  QFE2340_LTETDD_PA_BAND41_C_HB1_IN,
  QFE2340_LTETDD_PA_BAND41_C_HB2_IN,
  QFE2340_LTETDD_PA_BAND_NUM,
  QFE2340_LTETDD_PA_BAND_INVALID,
}qfe2340_ltetdd_pa_port_data_type;

typedef enum
{
  QFE2340_TDSCDMA_PA_BAND40_HB1_IN,
  QFE2340_TDSCDMA_PA_BAND40_HB2_IN,
  QFE2340_TDSCDMA_PA_BAND_NUM,
  QFE2340_TDSCDMA_PA_BAND_INVALID,
}qfe2340_tdscdma_pa_port_data_type;

typedef enum
{
  QFE2340_LTEFDD_ASM_BAND7_HB1_TX,
  QFE2340_LTEFDD_ASM_BAND30_HB1_TX,
  QFE2340_LTEFDD_ASM_BAND30_HB4_TRX,
  QFE2340_LTEFDD_ASM_BAND_NUM,
  QFE2340_LTEFDD_ASM_BAND_INVALID,
}qfe2340_ltefdd_asm_port_data_type;

typedef enum
{
  QFE2340_LTETDD_ASM_BAND38_HB3_TRX,
  QFE2340_LTETDD_ASM_BAND38_HB4_TRX,
  QFE2340_LTETDD_ASM_BAND40_HB2_TRX,
  QFE2340_LTETDD_ASM_BAND40_B_HB2_TRX,
  QFE2340_LTETDD_ASM_BAND41_HB3_TRX,
  QFE2340_LTETDD_ASM_BAND41_B_HB3_TRX,
  QFE2340_LTETDD_ASM_BAND41_C_HB3_TRX,
  QFE2340_LTETDD_ASM_BAND41_HB4_TRX,
  QFE2340_LTETDD_ASM_BAND41_B_HB4_TRX,
  QFE2340_LTETDD_ASM_BAND41_C_HB4_TRX,
  QFE2340_LTETDD_ASM_BAND_NUM,
  QFE2340_LTETDD_ASM_BAND_INVALID,
}qfe2340_ltetdd_asm_port_data_type;

typedef enum
{
  QFE2340_TDSCDMA_ASM_BAND40_HB2_TRX,
  QFE2340_TDSCDMA_ASM_BAND_NUM,
  QFE2340_TDSCDMA_ASM_BAND_INVALID,
}qfe2340_tdscdma_asm_port_data_type;

#endif
