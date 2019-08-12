/*!
  @file
  modem_fw_memmap.h

  @brief
  Modem firmware memory map.

  @detail
  Provides shared memory definitions for interfacing to FW.

*/

/*===========================================================================

  Copyright (c) 2012 Qualcomm Technologies Incorporated. All Rights Reserved

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

#ifndef MODEM_FW_MEMMAP_H
#define MODEM_FW_MEMMAP_H

#include "fws.h"

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! @brief Shared memory interface IDs.
*/
#define FW_SMEM_INTF_FWS                   0
#define FW_SMEM_INTF_C2KSRCH               1
#define FW_SMEM_INTF_CDMA1X                2
#define FW_SMEM_INTF_MCDO                  3
#define FW_SMEM_INTF_WCDMA                 4
#define FW_SMEM_INTF_GERAN                 5
#define FW_SMEM_INTF_LTE                   6
#define FW_SMEM_INTF_RFFW_LTE              7
#define FW_SMEM_INTF_TDSCDMA               8
#define FW_SMEM_INTF_RXLM                  9
#define FW_SMEM_INTF_TXLM                  10
#define FW_SMEM_INTF_IRAT                  11
#define FW_SMEM_INTF_COMMON_RF             12
#define FW_SMEM_INTF_RF_CAL                13
#define FW_SMEM_INTF_COEX                  14

#define FW_SMEM_INTF_TOTAL                 16


/*! @brief Legacy macros for getting shared memory address from SW.
*/
#define FW_SMEM_FWS_ADDR                   ((uint32) fws_smem_get_interface(FW_SMEM_INTF_FWS))
#define FW_SMEM_SRCH_ADDR                  ((uint32) fws_smem_get_interface(FW_SMEM_INTF_C2KSRCH))
#define FW_SMEM_CDMA_ADDR                  ((uint32) fws_smem_get_interface(FW_SMEM_INTF_CDMA1X))
#define FW_SMEM_MCDO_ADDR                  ((uint32) fws_smem_get_interface(FW_SMEM_INTF_MCDO))
#define FW_SMEM_WCDMA_ADDR                 ((uint32) fws_smem_get_interface(FW_SMEM_INTF_WCDMA))
#define FW_SMEM_GERAN_ADDR                 ((uint32) fws_smem_get_interface(FW_SMEM_INTF_GERAN))
#define FW_SMEM_LTE_ADDR                   ((uint32) fws_smem_get_interface(FW_SMEM_INTF_LTE))
#define FW_SMEM_RFFW_LTE_ADDR              ((uint32) fws_smem_get_interface(FW_SMEM_INTF_RFFW_LTE))
#define FW_SMEM_TDSCDMA_ADDR               ((uint32) fws_smem_get_interface(FW_SMEM_INTF_TDSCDMA))
#define FW_SMEM_RXLM_BUF_ADDR              ((uint32) fws_smem_get_interface(FW_SMEM_INTF_RXLM))
#define FW_SMEM_TXLM_BUF_ADDR              ((uint32) fws_smem_get_interface(FW_SMEM_INTF_TXLM))
#define FW_SMEM_IRAT_SCRIPT_ADDR           ((uint32) fws_smem_get_interface(FW_SMEM_INTF_IRAT))
#define FW_SMEM_COMMON_FW_RF_ADDR          ((uint32) fws_smem_get_interface(FW_SMEM_INTF_COMMON_RF))
#define FW_SMEM_RF_CAL_ADDR                ((uint32) fws_smem_get_interface(FW_SMEM_INTF_RF_CAL))
#define FW_SMEM_COEX_ADDR                  ((uint32) fws_smem_get_interface(FW_SMEM_INTF_COEX))


#endif /* MODEM_FW_MEMMAP_H */
