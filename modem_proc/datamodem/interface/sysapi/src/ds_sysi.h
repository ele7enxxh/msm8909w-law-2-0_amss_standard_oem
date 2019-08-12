/**
@file ds_sysi.h
@brief
This file defines function, variables and data structures internal to ds_sys 
System API module 
*/

#ifndef DS_SYSI_H
#define DS_SYSI_H

/*===========================================================================

                          D S  _ S Y S . H

DESCRIPTION

This file defines function, variables and data structures internal to ds_sys 
System API module 

Copyright (c) 2011 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  Copyright (c) 2011 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary.


  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/sysapi/src/ds_sysi.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
02/06/12    bvd     Created module

===========================================================================*/

#include "comdef.h"
#include "dserrno.h"
#include "ds_sys.h"
#include "ds_Utils_DebugMsg.h"


/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                           EXTERNALIZED FUNCTIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
#ifdef __cplusplus
extern "C" {
#endif

/**
  @brief  This method sets the variable indicating whether modem is in 
          reset condition 

  @param[in] is_modem_reset_in_progress     boolean value indicating modem 
                                            is in reset condition or not 

*/
void ds_sysi_set_modem_reset_in_progress
(
  boolean is_modem_reset_in_progress
);

/**
  @brief  This method is a callback registered for handling qmi sys level 
          events.

  @return  boolean value indicating whether modem is in reset condition 
           or not 
*/
boolean ds_sysi_is_modem_reset_in_progress
(
  void
);

/**
  @brief  This method initializes the ds sys qmi client 
          connection

  @return  -1 for failure, 0 for success
*/
int16 ds_sys_qmi_client_init
(
  void
);

/**
  @brief  This method releases the ds sys qmi client connection

  @return  void
*/
void ds_sys_qmi_client_release
(
  void
);

#ifdef __cplusplus
}
#endif

#endif /* DS_SYSI_H */
