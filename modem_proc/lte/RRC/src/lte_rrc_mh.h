/*!
  @file
  lte_rrc_mh.h

  @brief
  MH module's exposed APIs.
*/

/*===========================================================================

  Copyright (c) 2008 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/RRC/src/lte_rrc_mh.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
01/13/11   mm      Moved LTE_RRC_MH_MAX_BOOK_KEEPING_MSG from mhi.h
06/09/10   aramani Added extern declaration for 
                   lte_rrc_mh_get_last_encoded_message_rcvd 
04/06/10   amit    Initial Version
===========================================================================*/

#ifndef LTE_RRC_MH_H
#define LTE_RRC_MH_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <comdef.h>
#include "lte_rrc_int_msg.h"
#include "lte_rrc_osys_asn1util.h"

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! @brief Maximum number of messages that book keeping is done for
*/
#define LTE_RRC_MH_MAX_BOOK_KEEPING_MSG 32

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

/* Allocates dynamic memory for dd_ptr of private data and initializes private 
   data */
extern void lte_rrc_mh_dd_init(void);

/* Deallocates dynamic memory part of private data */
extern void lte_rrc_mh_dd_deinit(void);

/* Accesses the last encoded message recieved by the MH on the downlink*/
extern void lte_rrc_mh_get_last_encoded_message_rcvd(lte_rrc_pdu_buf_s *);

extern boolean lte_rrc_mh_dcch_msg_is_valid_in_current_sec_context
(
  uint32    ota_type, /*!< DL_DCCH or UL_DCCH type */
  boolean   sec_enabled, /*!< current sec context */
  boolean   dl_ota, /*!< Boolean to indicate if its DL DCCH OTA Message, FALSE indicates UL OTA */
  lte_rrc_ul_msg_type_e msg_type  /*!< Message Type*/
);

#endif /* LTE_RRC_MH_H */
