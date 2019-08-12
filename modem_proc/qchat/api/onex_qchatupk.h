#ifndef ONEX_QCHATUPK_H
#define ONEX_QCHATUPK_H
/*===========================================================================
                    QCHAT PORTING KIT 1X
                           DEFINITIONS

DESCRIPTION
  This file contains the declarations for functions within the DATA Layer of
  the QChat universal porting kit.

Copyright (c) 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

QUALCOMM does not grant to you in this Agreement any rights in the OEM 
QChat Porting Kit including the accompanying associated documentation 
(the Software) for any purpose whatsoever, including without limitation 
any rights to copy, use or distribute the Software. Your rights to use the 
Software shall be only as set forth in any separate license between you and 
QUALCOMM which grants to you a license in the Software, and such license  
shall be under the terms and for the limited purposes set forth in that  
agreement.
===========================================================================*/
/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/qchat/api/onex_qchatupk.h#1 $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/23/12   suren   Created.

===========================================================================*/


/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "comdef.h"
#include "sys.h"
                    
/*===========================================================================
                    EXTERNAL DECLARATIONS 
===========================================================================*/

/*===========================================================================

FUNCTION QCHATUPK_MCC_GET_CURRENT_PREFERRED_SCI 

DESCRIPTION
  QChat optimization for CDMA main controller.

DEPENDENCIES
  None.

RETURN VALUE
  Returns QChat preferred SCI

SIDE EFFECTS
  None.

===========================================================================*/
int8 qchatupk_mcc_get_current_preferred_sci( void );

/*===========================================================================

FUNCTION QCHATUPK_UPDATE_LOCATION

DESCRIPTION
  QChat optimization for updating location information.

DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void qchatupk_update_location(sys_sys_mode_e_type tech_type);

/*==========================================================================
FUNCTION QCHATUPK_SCI_1X_CB

DESCRIPTION
   This functions maintains and checks if there is any new SCI value requested
   by the client while AMSS was handling the previous request.

DEPENDENCIES
   None.

RETURN VALUE
   None

SIDE EFFECTS
   None.

============================================================================*/
void qchatupk_sci_1x_cb(void);

#endif /*ONEX_QCHATUPK_H*/

