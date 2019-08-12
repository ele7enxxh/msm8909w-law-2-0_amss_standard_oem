#ifndef __SAR_I_H__
#define __SAR_I_H__

/*!
  @file
  	sar_i.h

  @brief
	Definitions of functions which allow SAR functions to interfacewith other components.
*/

/*===========================================================================

  Copyright (c) 2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

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
/*==========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/limitsmgr/sar/inc/sar_i.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/27/14   rj     Initial implementation

===========================================================================*/

/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/


#include "comdef.h"
#include "lmtsmgr_i.h"
#include "sar_efs.h"
#include "sar_dsi.h"

#include "diagpkt.h"

/*=============================================================================

                       CONSTANTS AND DEFINES

=============================================================================*/


/*=============================================================================

                             TYPEDEFS

=============================================================================*/




/*=============================================================================

                      	FUNCTION PROTOTYPES

=============================================================================*/

/*=============================================================================

  FUNCTION:  sar_core_max_pwr_backoff

=============================================================================*/
/*!
    @brief
    update limits to MAX Power Backoff for a tech
 
    @return
    None
*/
/*===========================================================================*/
void sar_core_max_pwr_backoff(int16 *sar_plimit, uint8* num_valid_sar, cxm_tech_type tech);

/*=============================================================================

  FUNCTION:  lmtsmgr_process_sar_input

=============================================================================*/
/*!
    @brief
    Processes any change in the input variables for SAR/Batt
 
    @return
    None
*/
/*===========================================================================*/
extern void lmtsmgr_process_sar_input(void);


#endif /* __SAR_I_H__ */
