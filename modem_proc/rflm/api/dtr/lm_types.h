#ifndef LM_TYPES_H
#define LM_TYPES_H

/*!
   @file
   lm_types.h

   @brief
   This file implements the LM types. These are platform independent.
*/

/*==============================================================================

  Copyright (c) 2010 - 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
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

==============================================================================*/


/*==============================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rflm/api/dtr/lm_types.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
10/21/13   sar     Added LM_COMMON.
08/05/13   dej     Added LM_FBRX
02/25/11   aro     Added LM_HDR and LM_TDSCDMA
02/03/11   aro     Made Doxygen Compatible
01/13/11   aro     [1] Removed unnecessary include 
                   [2] Documentation Update 
01/12/11   bmg     Added lm_handle_type
12/14/10   sr      Initial version.

==============================================================================*/

#include "comdef.h"

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure definition for Link Manager Header
*/
typedef struct
{
  uint32 buffer_id;
  /*!< Buffer Identification */

  boolean free_flag;
  /*!< Flag indicating, if the buffer is free */

  uint32 chain;
  /*!< Link Manager Chain on which the operation is done */

  uint32 tech;
  /*!< Link Manager Technology */

} lm_buffer_header_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Enumeration of Link Managers Modes supported 
*/
typedef enum 
{
  LM_UMTS, /*!< UMTS Link Manager Mode */
  LM_1X, /*!< 1x Link Manager Mode */
  LM_HDR, /*!< 1x Link Manager Mode */
  LM_GSM, /*!< GSM Link Manager Mode */
  LM_LTE, /*!< LTE Link Manager Mode */
  LM_TDSCDMA, /*!< 1x Link Manager Mode */
  LM_DAC_CAL, /*!< DAC Calibration Link Manager Mode */
  LM_UMTS_XO_CAL,
  LM_1X_XO_CAL,
  LM_LTE_XO_CAL,
  LM_DEBUG,
  LM_FBRX,
  LM_COMMON,
  LM_TECH_MAX /*!< Maximum number of LM Modes supported */
} lm_tech_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Enumeration of types of Link Managers. 
*/
typedef enum 
{
  RXLM, /*!< Rx Link Manager */
  TXLM, /*!< Tx Link Manager */
  LM_MAX /*!< Maximum number of LM Managers types supported */
} lm_type;


/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Enumeration of Success and Error Codes to be used by Link Manager
  Interface
*/
typedef enum
{
  LM_SUCCESS = 1, /*!< Successful execution of LM Interface */
  LM_INVALID_PARAMETER = -99, /*!< Invalid Parameter Error Code */
  LM_BUFFERS_UNAVAILABLE,  /*!< Unavailable Buffer Error Code */
  LM_INVALID_BUFFER_INDEX,  /*!< Invalid Buffer Index Error Code */
  LM_UNINITIALIZED_BUFFER,  /*!< Uninitialized Buffer Error Code */
  LM_OBJ_NOT_CREATED,  /*!< Invalid Object Error Code */
  LM_INCORRECT_BUF_SIZE,  /*!< Incorrect Buffer Size Error Code */
  LM_BUFFER_NOT_ALLOCATED,  /*!< Not allocated Buffer Error Code */

} lm_status_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Opaque handle type for link manager buffers.

  @details
  All link manager calls should only refer to buffers by their
  handle, and ignore the implementation details in the link
  manager.
*/
typedef uint32 lm_handle_type;

#endif /* LM_TYPES_H */

