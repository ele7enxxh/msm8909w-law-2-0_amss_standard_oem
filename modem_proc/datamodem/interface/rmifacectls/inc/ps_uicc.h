#ifndef _PS_UICC_H
#define _PS_UICC_H
/*===========================================================================

                              P S _ U I C C . H
                   
DESCRIPTION
  The header file for UICC external/common declarations.

EXTERNALIZED FUNCTIONS
  
  ps_uicc_powerup_init()
    This function performs power-up initialization for the UICC sub-system

  ps_uicc_sio_cleanup()
    This function is used to close the SIO stream

Copyright (c) 2008-2009 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/rmifacectls/inc/ps_uicc.h#1 $
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $
===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#ifdef FEATURE_DATA_PS_UICC

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                             EXTERNAL FUNCTIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*===========================================================================
FUNCTION  PS_UICC_POWERUP_INIT()

DESCRIPTION
  This function performs power-up initialization for the UICC sub-system

PARAMETERS
  void 

RETURN VALUE
  void

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_uicc_powerup_init
(
  void
);

/*===========================================================================
FUNCTION  PS_UICC_SIO_CLEANUP()

DESCRIPTION
  This function is used to cleanup the SIO watermarks

PARAMETERS
  void

RETURN VALUE
  void

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_uicc_sio_cleanup
(
  void
);

#endif /* FEATURE_DATA_PS_UICC */
#endif /* _PS_UICC_H */
