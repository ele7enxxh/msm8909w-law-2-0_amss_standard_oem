/*!
  @file
  hw_decob_buf.h

  @brief
  enums of LTE buffers in decoder output buffer

  @detail
  description of functions in this file
 
*/

/*===========================================================================

  Copyright (c) 2007 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/fw/fw_lte/api/hw_decob_buf.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
===========================================================================*/

#ifndef HW_DECOB_BUF_H
#define HW_DECOB_BUF_H


/*===========================================================================

      Constants

===========================================================================*/

/*! @brief Enum of LTE buffers in decoder output buffer.  This is used to
           in HW memory access function for on-target testing and in 
           scheduling decoder tasks for PBCH, PDCCH, and PDSCH.

    Note: do not change this without changing offset lookup table in hw_decob.c
*/
typedef enum
{
  LTE_LL1_HW_DECOB_PBCH_DATA,
  LTE_LL1_HW_DECOB_PBCH_STATUS,
  LTE_LL1_HW_DECOB_PDCCH_DATA,
  LTE_LL1_HW_DECOB_PDCCH_STATUS,
  LTE_LL1_HW_DECOB_PDSCH_0_DATA,
  LTE_LL1_HW_DECOB_PDSCH_0_STATUS,
  LTE_LL1_HW_DECOB_PDSCH_1_DATA,
  LTE_LL1_HW_DECOB_PDSCH_1_STATUS,
  LTE_LL1_HW_DECOB_PDSCH_SIB_DATA,
  LTE_LL1_HW_DECOB_PDSCH_SIB_STATUS,
  LTE_LL1_HW_DECOB_PDSCH_MULTICAST_DATA,
  LTE_LL1_HW_DECOB_PDSCH_MULTICAST_STATUS  
} lte_LL1_hw_decob_buf_e;





/*===========================================================================

      Macros

===========================================================================*/


/*===========================================================================

      Typedefs

===========================================================================*/


/*===========================================================================

      Functions

===========================================================================*/


#endif /* HW_DECOB_BUF_H */





