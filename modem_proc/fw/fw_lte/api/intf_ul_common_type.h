/*!
  @file
  intf_ul_common_type.h

  @brief
  This file contains LTE LL uplink command id's and parameters structures

  @detail
  The messages that describe the interface between the ML (UL Manager and 
  Grant Manager) and the LL (lower layer) are described.
 
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/fw/fw_lte/api/intf_ul_common_type.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
===========================================================================*/
#ifndef INTF_UL_COMMON_TYPE_H 
#define INTF_UL_COMMON_TYPE_H

#define MAX_TXPLL_SCRIPT_ENTRIES    5

/*! 
 	@brief TXPLL entry structure to write to RF through SSBI
 	Max up to 5 SSBI writes per TXPLL change
*/
typedef struct 
{
   uint8 bus;
   uint8 page;
   uint8 addr;
   uint8 val;
} lte_LL1_ul_txpll_script_entry_t;
typedef struct 
{
   uint8 num_entries;
   lte_LL1_ul_txpll_script_entry_t txpll_script[MAX_TXPLL_SCRIPT_ENTRIES];
} lte_LL1_ul_txpll_script_array_t;

#endif /* INTF_UL_COMMON_TYPE_H */
