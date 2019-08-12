/*!
  @file
  intf_dl_pdcch.h

  @brief
  This file contains lte LL downlink command id's and parameters structures

  @detail
  description of functions in this file
 
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/fw/fw_lte/api/intf_dl_pdcch_test.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/15/09   anm     Added PDSCH stat message to replace PDSCH result ind
===========================================================================*/
#ifndef INTF_DL_PDCCH_TEST_H
#define INTF_DL_PDCCH_TEST_H

#include "msgr.h"
#include "intf_common.h"

/*===========================================================================

      Constants

===========================================================================*/

/*===========================================================================

      Macros

===========================================================================*/

/*===========================================================================

      Typedefs

===========================================================================*/


/*! @brief DL_PDCCH_TEST request input structure
*/

typedef struct {
  lte_LL1_req_header_struct   req_hdr; ///< Common request parameters
  uint8 num_phich;          
  uint8 n_seq_phich;                           ///< phich sequence number
  uint8 n_group_num_phich;                     ///< phich group number
  uint8 n_seq_phich_1;
  uint8 n_group_num_phich_1;
} lte_LL1_dl_pdcch_test_req_struct;

LTE_LL1_INTF_MSG( dl_pdcch_test_req );

#endif /* INTF_DL_PDCCH_TEST_H */

