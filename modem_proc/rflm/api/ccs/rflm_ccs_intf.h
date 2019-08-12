#ifndef RFLM_CCS_INTF_H
#define RFLM_CCS_INTF_H

/*!
  @file
  rflm_ccs_intf.h

  @brief
  RFLM CCS processor interface.

*/

/*===========================================================================

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

===========================================================================*/
/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rflm/api/ccs/rflm_ccs_intf.h#1 $
$Date: 2016/12/13 $
when         who     what, where, why
--------   ---     -----------------------------------------------------------

11/5/14    hdz     Added resource_to_dma_chan for Non JO/DPM targets
11/25/13   Saul    CCS. New CSS Data Buffer Allocation For All Techs.
09/04/13   ra      Support CCS_RF_INTF
08/09/13   ra      Initial port to Bolt RF SW
==============================================================================*/


 
/*===========================================================================

                           INCLUDE FILES

===========================================================================*/ 
 
#include "rflm_ccs_version.h"
#include "rflm_ccs_task_defs.h"



/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! @brief RFC operating states.
    These are defined as bitmasks for the rflm_ccs_info_t.state field.
    RFLM_CCS_STATE_RUN:    RFC is out of reset. 
    RFLM_CCS_STATE_READY:  RFC has completed initialization, ready for commands. 
    RFLM_CCS_STATE_ERROR:  RFC has generated an internal error.
    RFLM_CCS_STATE_HALTED: RFC has halted and saved register state.
*/
#define RFLM_CCS_STATE_RUN        (0x1)
#define RFLM_CCS_STATE_READY      (0x2)
#define RFLM_CCS_STATE_HALTED     (0x4)
#define RFLM_CCS_STATE_ERROR      (0x80000000)

/*!
  @brief
  Items types making use of RFC/CCS data buffer

  @details
  This enumeration lists the item types that may use RFC/CCS data buffer.
*/
typedef enum
{
  /*! TQ Pair 0 */
  RFLM_CCS_DATA_BUF_ITEM_PAIR0 = 0,
  /*! TQ Pair 1 */
  RFLM_CCS_DATA_BUF_ITEM_PAIR1,
  /*! IRAT */
  RFLM_CCS_DATA_BUF_ITEM_IRAT,
  /*! Dedicated SW Port */
  RFLM_CCS_DATA_BUF_ITEM_DEDICATED
} rflm_ccs_data_buf_item_type;

/*! @brief RFC info structure.
    Contains info about RFC operating state. 
    This is placed in a dedicated INFO section, at a known address.
    It is also initialized as part of the download process, unlike INTF which is
    not downloaded.
*/
typedef struct
{
  char version[32];   /*!< Version string */
  char date[16];      /*!< Build date */
  char time[16];      /*!< Build time */

  uint32 state;       /*!< CCS state. Combination of CCS_STATE_x bitmasks. */

  uint32 error;       /*!< Error code */
  uint32 error_arg[2];/*!< Error arguments, if used. */
  uint32 error_pc;    /*!< Program counter where error was called */
  uint32 error_utime; /*!< CCS_UTIME when error was triggered */

#ifndef FEATURE_JOLOKIA_MODEM
  uint8 resource_to_dma_chan[CCS_RF_MAX_BUS_RESOURCES];
#endif

} rflm_ccs_info_t;
 
 
/*! @brief Shared memory interface.
    The start of this structure corresponds to the start of the INTF section.
    It it uninitialized at download time.
*/
typedef struct
{
  ccs_rf_intf_t rflm_ccs_rf_intf;   /*!< RF interface */

} rflm_ccs_intf_t;


#endif  // RFLM_CCS_INTF_H

