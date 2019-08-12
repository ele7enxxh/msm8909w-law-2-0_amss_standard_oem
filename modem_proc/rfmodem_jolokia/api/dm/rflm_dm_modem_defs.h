/*!
  @file
  rflm_dm_modem_defs.h

  @brief
  This header file contains the RFLM DM(data manager) target-specific definitions

*/

/*===========================================================================

  Copyright (c) 2013 - 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfmodem_jolokia/api/dm/rflm_dm_modem_defs.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/21/15   bp     Increase RFLM_TECH_BUF_SIZE_UINT32 from 15360 to 17408 for JO1.1.1+WTR2955  
09/04/14   aw     Created the initial version
===========================================================================*/

#ifndef RFLM_DM_MODEM_DEFS_H
#define RFLM_DM_MODEM_DEFS_H


/* Modem specific DM workarounds */
#include "rflm_dm_modem_workaround.h"

/*===========================================================================
						   CONSTANTS
===========================================================================*/
/* Handle counts*/
#define RFLM_HANDLE_COUNTS 			            18 

/* Max number of scripts for a handle */
#define RFLM_MAX_NUM_HANDLE_SCRIPT_BUF          50

/* Max caller history number for stack trace */
#define RFLM_DM_MAX_CALLER_HISTORY              8 

/* The size in uint32 for tech buffer */
#ifndef FEATURE_TX_SHARING
#define RFLM_TECH_BUF_SIZE_UINT32               (17408)
#else
/* single TX-DSDA - 70K*/
#define RFLM_TECH_BUF_SIZE_UINT32               (17920)
#endif

/* The size in uint32 of the tech data buffer */
// #define TECH_DATA_SIZE_UINT32                   256    // Moved to rflm_dm_modem_defs.h and use sizeof to match rflm_wcdma_dm_buf_type

/* The size in uint32 of the DTR RX and TX buffer */
#define RFLM_DM_DTR_RX_BUF_UINT32_SIZE          0x200
#define RFLM_DM_DTR_TX_BUF_UINT32_SIZE          0x200

/* The size in uint32 of the script buffer */
/* paddding is enabled*/
#define SCRIPT_BUF_PADDING

#ifndef SCRIPT_BUF_PADDING
// --- no padding ---
#define RFLM_DM_SBUF_2HB_UINT32_SIZE            50
#define RFLM_DM_SBUF_5HB_UINT32_SIZE            128
#define RFLM_DM_SBUF_1KB_UINT32_SIZE            256
#define RFLM_DM_SBUF_2KB_UINT32_SIZE            512
#define RFLM_DM_SBUF_3KB_UINT32_SIZE            768
#else
// --- padding ---
#define RFLM_DM_SBUF_2HB_UINT32_SIZE            50
#define RFLM_DM_SBUF_5HB_UINT32_SIZE            144    // DM Reduction: 16 words padding 
#define RFLM_DM_SBUF_1KB_UINT32_SIZE            306    // DM Reduction: revert back padding as a workaround
#define RFLM_DM_SBUF_2KB_UINT32_SIZE            528    // DM Reduction: 16 words padding
#define RFLM_DM_SBUF_3KB_UINT32_SIZE            768      
#endif

/* The number of items in the script buff*/
#define RFLM_SCRIPT_BUF_2HB_COUNTS              30
#define RFLM_SCRIPT_BUF_5HB_COUNTS              110    // DM Reduction: 150 -> 110
#define RFLM_SCRIPT_BUF_1KB_COUNTS              40     // Dm Reduction: 50 -> 40
#define RFLM_SCRIPT_BUF_2KB_COUNTS              20
#define RFLM_SCRIPT_BUF_3KB_COUNTS              2      // DM Reduction: 3 -> 2

/* DTR buffer counts*/
#define RFLM_DM_DTR_RX_BUF_COUNTS               14 
#define RFLM_DM_DTR_TX_BUF_COUNTS               4

/* Tech buffer counts*/
#ifndef RFLM_DM_WORKAROUND_TECH_BUF_DSDA_LTE_TDS_1X_G
  #define RFLM_TECH_BUF_POOL_SIZE               3
#else
  #define RFLM_TECH_BUF_POOL_SIZE               4
#endif 

/* Max event dependency number */
#define RFLM_DM_MAX_DEPENDENCY_NUM_PER_EVENT    1 

/* Request size ranges */
typedef enum
{
  BUFF_SIZE_IN_BYTE_2H = 200,
  BUFF_SIZE_IN_BYTE_5H = 512,
  BUFF_SIZE_IN_BYTE_1K = 1024,
  BUFF_SIZE_IN_BYTE_2K = 2048,
  BUFF_SIZE_IN_BYTE_3K = 3072
} rflm_dm_buff_size_e;


#endif /* RFLM_DM_MODEM_DEFS_H */
