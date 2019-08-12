/*!
  @file
  lte_mac_ind.h

  @brief
  This file contains all External indications thrown by MAC layer.


*/

/*===========================================================================

  Copyright (c) 2014 QUALCOMM Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  QUALCOMM Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of QUALCOMM Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/api/lte_mac_ind.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/23/14   sb      CR772190: Extend A2 diag framework for L2
02/13/14   md      Updated EARFCN from uint16 to uint32 (lte_earfcn_t)
06/12/14   ru      CR665154: SRLTE: Fast retransmission of UL RLC packets lost 
                   due to HARQ failures in Tuneaway
09/20/11   st      Added 2 new parameters struct for 2 eMBMS messages
05/17/10   bn      Added in MAC/RRC API LTE_MAC_RELEASE_RESOURCES_IND
07/28/09   bn      Put back the changes for lte common indication
07/27/09   bn      Removed the dependencies on lte_a2_phy_dl_msg.h
07/09/09   bn      Moved the mac contention indication to lte common inc.
05/15/08   bn      Initial version
===========================================================================*/

#ifndef LTE_MAC_IND_H
#define LTE_MAC_IND_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <comdef.h>
#include <msgr.h>
#include <msgr_lte.h>
#include <msgr_attach.h>

/* Start of L2 internal including */
#include "lte_mac.h"
#include "a2_diag.h"

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! @brief MAC RACH access problem indication. MAC UL sends to RRC
*/
typedef struct 
{
  msgr_hdr_struct_type                hdr;           /*!< Message header */
  lte_mac_access_reason_e             access_reason; /*!< access reason */
  /* Message payload */
} lte_mac_random_access_problem_ind_msg_s;

/*! @brief Message for MAC RRC DL Data Indication. MAC DL sends to RRC
*/
typedef struct
{
  msgr_hdr_s                          hdr;          /*!< Message header */
  msgr_attach_s                       dsm_attach;   /*!< dsm attachment contained signal message */
  uint16                              phy_cell_id;  /*!< The cell Id of the cell on which data was received */
  lte_earfcn_t                        freq;         /*!< The frequency of the cell on which data was received */
  lte_sfn_s                           sfn;          /*!< The system and subframe number */
} lte_mac_rrc_dl_data_ind_s;

/*
* @brief Parameter struct for  LTE_MAC_A2_DL_PHY_DATA_IND. Using
* in a callback function provided to A2 driver by MAC so that
* when the callback function is invoked, it will send this indication
* to MAC DL
*/
typedef struct
{
  msgr_hdr_struct_type           hdr; /*!< Message header */
} lte_mac_a2_dl_phy_data_ind_s;

/*
* @brief Parameter struct for  LTE_MAC_RELEASE_RESOURCES_IND. MAC will send
* this indication to RRC when TA timer expired or SR MAX
*/
typedef struct
{
  msgr_hdr_struct_type           hdr; /*!< Message header */
} lte_mac_release_resources_ind_s;

/*
* @brief Parameter struct for  LTE_MAC_BCAST_DATA_IND. MAC will send
* this indication to RLC when there is some eMBMS data on DL
*/
typedef struct
{
  msgr_hdr_struct_type           hdr; /*!< Message header */
} lte_mac_bcast_data_ind_s;

/*
* @brief Parameter struct for  LTE_MAC_DUAL_DATA_IND. MAC will send
* this indication to RLC when there is both eMBMS and unicast data on DL
*/
typedef struct
{
  msgr_hdr_struct_type           hdr; /*!< Message header */
} lte_mac_dual_data_ind_s;

/*
* @brief Parameter struct for  LTE_MAC_MAX_HARQ_RETX_IND. MAC will 
* send this indication to RLC when it detects max HARQ retransmission attempt
* ML1 also sends this indication in long tuneaway. 
*/
typedef struct
{
  msgr_hdr_struct_type           hdr; /*!< Message header */
  uint8                          harq_id; /*!< harq ID */
}lte_mac_max_harq_retx_ind_s;

/*
* @brief Parameter struct for  LTE_MAC_DIAG_DATA_IND. 
* This indication will be multi-casted to RLC/PDCP/MAC when callback function
  registered with A2 diag gets invoked
*/
typedef struct
{
  msgr_hdr_struct_type           hdr; /*!< Message header */
  a2_diag_req_reqitem_s          diag_req_item; /*!< harq ID */
}lte_mac_diag_data_ind_s;

#endif /* LTE_MAC_IND_H */
