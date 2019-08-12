/*!
  @file
  modem_fw_ipc.h

  @brief
  Modem firmware inter-process communication

  @detail
  Provides definitions of FWS IPC indices for SW-FW communication.
  
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

#ifndef MODEM_FW_IPC_H
#define MODEM_FW_IPC_H


/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

#define FWS_IPC_FW_SW__DIAG_FATAL                   0
#define FWS_IPC_FW_SW__GERAN_SLEEP_CNF              1
#define FWS_IPC_FW_SW__WCDMA_EUL_IND                2
#define FWS_IPC_FW_SW__WCDMA_IND                    3
#define FWS_IPC_FW_SW__CCS_RF_CMD_DONE              4
#define FWS_IPC_FW_SW__RF_EVENT_DONE                5
#define FWS_IPC_FW_SW__TDSCDMA_IND                  6

#define FWS_IPC_SW_FW__GRFC_CMD                     10
#define FWS_IPC_SW_FW__CCS_RF_CMD                   11
#define FWS_IPC_SW_FW__GERAN_CMD_GFW_SLEEP          12
#define FWS_IPC_SW_FW__GERAN_CMD_GFW_SLEEP_QUERY    13
#define FWS_IPC_SW_FW__GERAN_CMD_GFW_WAKE           14
#define FWS_IPC_SW_FW__WCDMA_CMD                    15

/* FWS_IPC_NUM_ID cannot be higher than 16 due to MCS
   limitation.  If more are needed, check with MCS
   team first before incrementing this count */
#define FWS_IPC_NUM_ID                              16

#endif /* MODEM_FW_IPC_H */


