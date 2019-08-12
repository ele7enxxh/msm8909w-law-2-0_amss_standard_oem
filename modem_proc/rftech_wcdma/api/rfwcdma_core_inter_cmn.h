#ifndef RFWCDMA_CORE_INTER_CMN_H
#define RFWCDMA_CORE_INTER_CMN_H

/*!
  @file
  rfwcdma_core_inter_cmn.h

  @brief
  This file contains declarations and definitions to support WtoW operation
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

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: 
  $DateTime: 
  $Author: 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/29/10    ka     Update w2w buffer size. 
11/02/09   ckl     Initial Revision.

===========================================================================*/

/*===========================================================================

                              INCLUDE FILES

===========================================================================*/

/* Max number of SBI transactions allowed */
#define RFWCDMA_CORE_INTER_CMN_MAX_NUM_WTOW_SBI_SCRIPTS  128  
/* Max number of RXF registers allowed */
#define RFWCDMA_CORE_INTER_CMN_MAX_NUM_WTOW_RXF_SCRIPTS  11
/* Number of RXF registers used during CM */
#define RFWCDMA_CORE_INTER_CMN_WTOW_RXF_SCRIPTS          9

#endif  /* RFWCDMA_CORE_INTER_CMN_H */
