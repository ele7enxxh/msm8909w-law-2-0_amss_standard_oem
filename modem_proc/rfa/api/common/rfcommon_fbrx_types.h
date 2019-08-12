#ifndef RFCOMMON_FBRX_TYPES_H
#define RFCOMMON_FBRX_TYPES_H
/*!
  @file
  rfcommon_fbrx_types.h

  @brief
  This module contains prototypes and definitions used by common FBRx.
*/

/*==============================================================================

  Copyright (c) 2013 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

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

==============================================================================*/

/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/api/common/rfcommon_fbrx_types.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
11/18/14   aro     Renamed SW based ILPC variable to better suit the feature
08/15/14   aro/gh  Add support for FBRX ILPC
08/08/14   aa      Moved gps_state enum type to RFLM 
07/31/14   aa      Added api to set the gps state and update it to the fbrx dm
06/17/14   aa      Updated fbrx control enum type 
03/21/14   aa      Update the fbrx error type 
02/27/14   aa      Use rfcom_band_type_u instead
02/03/14   aa      Move enum tx_cfg_type to rflm_fbrx_api.h
02/03/14   aa      Initial version

==============================================================================*/

#include "rfcom.h"

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/
/*! Enum used to hold FBRx error codes */
typedef enum
{
  RFCOMMON_FBRX_ERROR = 0,
  RFCOMMON_FBRX_SUCCESS = 1,
} rfcommon_fbrx_error_type;

typedef enum
{
  RFCOMMON_FBRX_1_CARRIER = 1,
  RFCOMMON_FBRX_2_CARRIER = 2,
  RFCOMMON_FBRX_3_CARRIER = 3,
} rfcommon_fbrx_num_carriers_type;

typedef enum
{
  RFCOMMON_FBRX_DISABLE_NO_UPDATES = 0,         /* Disable FBRx, Do not apply any FBRX updates */
  RFCOMMON_FBRX_ENABLE_NO_UPDATES = 1,          /* Enable FBRx, Do not apply any FBRX updates */
  RFCOMMON_FBRX_ENABLE_LIN_UPDATES = 3,         /* Enable FBRx and apply FBRX updates to TxLin table */ 
  RFCOMMON_FBRX_ENABLE_MTPL = 7,                /* Enable FBRx and apply FBRX updates using MTPL method */ 
  RFCOMMON_FBRX_ENABLE_IMM_DTR_ALWAYS = 9,      /* Enable FBRx and apply IMM DTR Updates every TXAGC */ 
  RFCOMMON_FBRX_ENABLE_IMM_DTR_OPT = 11,        /* Enable FBRx and apply IMM DTR Updates with TxLin Table updates */ 
  RFCOMMON_FBRX_ENABLE_SW_BASED_ILPC = 13,      /* Enable SW based of ILPC correction */ 
  RFCOMMON_FBRX_INVALID                         /* Invalid : Disable FBRx, Disable updates */
}rfcommon_fbrx_control_enum_type;

#ifdef __cplusplus
}
#endif

#endif /* RFCOMMON_FBRX_TYPES_H */

