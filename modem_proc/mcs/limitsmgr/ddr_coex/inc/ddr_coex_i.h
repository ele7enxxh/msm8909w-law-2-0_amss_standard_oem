#ifndef __DDR_COEX_I_H__
#define __DDR_COEX_I_H__

/*!
  @file
  	ddr_coex_i.h

  @brief
	Definitions of functions which allow DDR functions to interface with other components.
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/limitsmgr/ddr_coex/inc/ddr_coex_i.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/27/14   rj     Initial implementation

===========================================================================*/

/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/


#include "comdef.h"
#include "trm.h"
#include "lmtsmgr_i.h"
#include "ddr_coex_npa.h"

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

  FUNCTION:  ddr_coex_init

=============================================================================*/
/*!
    @brief
    Initialize DDR_Coex related functionalities.
 
    @return
    None
*/
/*===========================================================================*/
void ddr_coex_init (void);

/*=============================================================================

  FUNCTION:  ddr_coex_trm_tech_state_update_hdlr

=============================================================================*/
/*!
    @brief
    DDR coex function to handle TRM tech_state indication.
 
    @return
    None
*/
/*===========================================================================*/
void ddr_coex_trm_tech_state_update_hdlr
(
  trm_state_info_type trm_state_info
);

/*=============================================================================

  FUNCTION:  ddr_coex_process_ddr_input

=============================================================================*/
/*!
    @brief
    Processes any change in the DDR coex input variables
 
    @return
    None
*/
/*===========================================================================*/
void ddr_coex_process_ddr_input(void);

/*=============================================================================

  FUNCTION:  ddr_coex_update_tech_info

=============================================================================*/
/*!
    @brief
    Updates the tech's channel and Band only if it has changed. Also sets the
    band/channel changed flag.
 
    @return
    None
*/
/*===========================================================================*/
void ddr_coex_update_tech_info
(
  cxm_tech_type tech_id
);

#endif /* __DDR_COEX_I_H__ */
