#ifndef LTE_QDR_H
#define LTE_QDR_H

/*!
  @file lte_qdr.h

  GENERAL DESCRIPTION
  Utility functions related to QDR memory: Memory mapping,
  dynamic memory allocation from QDR memory.

  @detail
  
  INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
*/

/*===========================================================================

  Copyright (c) 2009 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/api/lte_qdr.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/12/09   yg     Initial Release

===========================================================================*/

/*===========================================================================

               INCLUDE FILES

===========================================================================*/
#include <comdef.h>

/*===========================================================================

           DEFINITIONS AND TYPES

===========================================================================*/ 

/*! @brief Define physical start address of QDR memory
*/
#define LTE_QDR_PHY_START_ADDR 0x12000000

/*! @brief Define size of QDR memory. 8MBytes
*/
#define LTE_QDR_SIZE (8*1024*1024)

/*! @brief Define size of QDR memory simulated on SMI
*/
#define LTE_QDR_ON_SMI_SIZE 0x8000

/*! @brief Define physical start address of QDR Clock Register memory
*/
#define LTE_QDR_CLK_REG_PHY_START_ADDR 0x84002848

/*! @brief Define size of QDR Clock Register memory
*/
#define LTE_QDR_CLK_REG_SIZE 0x10

/*! @brief Global structure for QDR support
*/
typedef struct
{
  boolean           map_result;   /*!< TRUE if QDR memory mapping was success */


  uint8*            mem_alloc_current_addr; /*!< Used for dynamic memory allocation
                                                 from QDR memory. Start Addr from which
                                                 memory is available */
} lte_qdr_s;



/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

/*===========================================================================

  FUNCTION:  lte_qdr_init

===========================================================================*/
/*!
  @brief
  Maps QDR memory

  @detail

  @return
  EOK if successful. error code if erred.
*/
/*=========================================================================*/

void lte_qdr_init(void);

/*==============================================================================

  FUNCTION:  lte_qdr_mem_alloc

==============================================================================*/
/*!
  @brief
  Allocate memory from the QDR memory region.

*/
/*============================================================================*/
void* lte_qdr_mem_alloc ( uint32 bytes );

#endif /* LTE_QDR_H */


