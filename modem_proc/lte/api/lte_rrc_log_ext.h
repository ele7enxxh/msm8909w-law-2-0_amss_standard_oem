/*!
  @file lte_rrc_log_ext.h

  @brief
  This header file contains the variou defines, enums, and other data
  structure types which are necessary for logging binary packets via DIAG.
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

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/api/lte_rrc_log_ext.h#1 $

when       who       what, where, why
--------   ---       ---------------------------------------------------------- 
03/10/13   ap   Initial version
===========================================================================*/

#ifndef LTE_RRC_LOG_EXT_H
#define LTE_RRC_LOG_EXT_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

#ifdef _WIN32
#pragma pack(push,1) // Save previous, and turn on 1 byte alignment
#endif

/*! @brief Serving cell info log structure
*/
typedef PACK(struct)
{
  uint8 log_packet_ver;  /*!< Log packet version */
  uint16 phy_cell_id;  /*!< Physical cell ID */
  uint32 dl_freq;  /*!< DL Frequency of the cell */
  uint32 ul_freq;  /*!< UL Frequency (from SIB2) */
  uint8 dl_bw;  /*!< Downlink bandwidth (from MIB) */
  uint8 ul_bw;  /*!< Uplink bandwidth */
  uint32 cell_id;  /*!< Cell identity received in SIB1 */
  uint16 tracking_area_code;  /*!< Tracking area code of the cell */
  uint32 freq_band_ind;  /*!< Frequency band indicator */
  uint16 sel_plmn_mcc;  /*!< MCC of selected PLMN */
  uint8 num_mnc_digits;  /*!< Number of digits in MNC (2 or 3) */
  uint16 sel_plmn_mnc;  /*!< MNC of selected PLMN */
  uint8 allowed_access;  /*!< Allowed access on the cell - Full/Limited */
} lte_rrc_log_serv_cell_info_s;

#ifdef _WIN32
#pragma pack(pop) // Revert alignment to what it was previously
#endif

#endif /* LTE_RRC_LOG_EXT_H */

