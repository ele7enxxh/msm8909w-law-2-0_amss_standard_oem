/*!
  @file
  rfca_to_ftm_adapter.h
 
  @brief
  Defines DIAG messages for RFCA communication.

*/


/*==============================================================================

  Copyright (c) 2011-2012 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/rfca/inc/rfca_to_ftm_adapter.h#1 $
   
when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
08/09/13   aki     Cleanup
06/10/13   aki     Mainline RFCA
04/23/12   aki     Added RFCA_SyncEfs
01/25/12   aki     Documentation update
10/28/11   aki     Initial revision

==============================================================================*/

#ifndef _RFCA_TO_FTM_ADAPTER_H_
#define _RFCA_TO_FTM_ADAPTER_H_

#include "rfa_variation.h"
#include "comdef.h"

#include "rfca_custom_types.h"

#ifdef __cplusplus
  extern "C" {
#endif

/*----------------------------------------------------------------------------*/
/*!
 @brief Enables FTM log messages

 @return 0 success
 */
int RFCA_FtmLogsEnable();

/*----------------------------------------------------------------------------*/
/*!
 @brief Deinitialises FTM seqeuncer

 */
void RFCA_FtmSequencerDeinit();

/*----------------------------------------------------------------------------*/
/*!
 @brief Initialises FTM seqeuncer

 @return 0 success
 */
int RFCA_FtmSequencerInit();

/*----------------------------------------------------------------------------*/
/*!
 @brief Executes FTM seqeuncer 

 @return 0 success
 */
int RFCA_FtmSequencerExecute();

/*----------------------------------------------------------------------------*/
/*!
 @brief Sends sync message to ftm task

 @param [in] req_data           Request data to send 
 @param [in] req_data_size      Size of the request data
 @param [out] resp_data         Buffer where response is copied to
 @param [in,out] resp_data_size Size of buffer when going in, size of response when coming out

 @return 0 success
 */
int RFCA_SendToFtmTaskSync(uint8 *req_data, uint16 req_data_size, uint8 *resp_data, uint16 *resp_data_size);
#ifdef __cplusplus
  }
#endif

#endif //_RFCA_TO_FTM_ADAPTER_H_
