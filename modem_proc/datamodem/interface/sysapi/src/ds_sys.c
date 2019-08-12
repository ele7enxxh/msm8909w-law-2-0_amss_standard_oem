/******************************************************************************
  @file    ds_sys.c
  @brief

  DESCRIPTION
  This file defines function, variables and data structures common to all
  DS System API module

  INITIALIZATION AND SEQUENCING REQUIREMENTS
  N/A

  ---------------------------------------------------------------------------
  Copyright (C) 2011 Qualcomm Technologies Incorporated.
  All Rights Reserved. QUALCOMM Proprietary and Confidential.
  ---------------------------------------------------------------------------
******************************************************************************/
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/sysapi/src/ds_sys.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/06/12    bvd     Created module
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "ds_sysi.h"

/*===========================================================================

                           LOCAL DATA DECLARATIONS

===========================================================================*/


/*===========================================================================

                           EXTERNAL FUNCTIONS

===========================================================================*/
void ds_sysi_set_modem_reset_in_progress
(
  boolean is_modem_reset_in_progress
)
{
  return;
} /*  ds_sysi_set_modem_reset_in_progress() */

boolean ds_sysi_is_modem_reset_in_progress
(
  void
)
{
  return FALSE;
} /* ds_sysi_is_modem_reset_in_progress() */

int16 ds_sys_init_lib
(
  void
)
{
  return 0;
} /* ds_sys_init_lib() */

/**
  @brief  This method initializes the ds sys qmi client 
          connection

  @return  -1 for failure, 0 for success
*/
int16 ds_sys_qmi_client_init
(
  void
)
{
  return 0;
}

/**
  @brief  This method releases the ds sys qmi client connection

  @return  void
*/
void ds_sys_qmi_client_release
(
  void
)
{
  return;
}
