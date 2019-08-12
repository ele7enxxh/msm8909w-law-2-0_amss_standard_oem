/*============================================================================

@file
sns_debug_dsps_str.c

@brief
Contains main implementation of receiving and processing
debug strings on DSPS.

  Copyright (c) 2010-2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

============================================================================*/

/*===========================================================================

      EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/debug/src/sns_debug_dsps_str.c#1 $

when        who    what, where, why
(mm/dd/yyyy)
--------    ----  ----------------------------------------------------------
08/13/2015  gju   Remove use of smr_utils and PCSIM
07/01/2015   BD   Enabled batchbuff module for printing
07/20/2014   VY   Disabled logging in uImage
01/29/2013   vy   Updated for uImage support.
01/29/2013  gju   Include different file for SMR utlity functions.
01/18/2013   ag   Use the tail end of file name(with path) to get useful information
11/21/2012   ps   Added sns_debug_is_module_disabled() function
10/04/2012   dc   Update priority field. Make request, responses and async indications,
                  and log and debug indications as high priority.
08/08/2012   ag   Fix size param of csi_send_ind
07/30/2012   ag   Porting from SMR to QMI
01/26/2012   ag   Filename parameter of sns_debug_printf_dsps_string_id will
                  no longer contain entire path
11/22/2011   br   Use of SNS_OS_MEMXXX instead of OI_MemXXX
11/18/2011   sc   Fix for compilation warnings
11/14/2011   jhh  Updated sns_smr_msg_alloc and sns_smr_msg_free to meet new API
05/23/2011   sj   Disable all print msg masks by default
                  (this is helpful during 8960 bringup)
03/04/2011   ad   Added SCM debug messages
02/28/2011   br   disable the debug_print feature with SNS_DSPS_PROFILE_ON definition
02/08/2011   ag   Add AKM8975 Magnetometer support (original checkin by osnat)
01/28/2011   br   changed constant names for deploying QMI tool ver#2
01/15/2011   sc   Fix on PCSIM to reflect last change of debug msg interface
11/15/2010   ag   fix memory leak introduced in case of DAL tests
11/09/2010   ad   added support for debug filtering
10/28/2010   ad   added device driver modules to string database
10/26/2010   ag   sns_smr_send() called only when DAL_UCOS_TEST is not defined
10/11/2010   sj   Added support for SMGR Debug Strings (for PCSIM)
09/30/2010   sj   More error handling and minor fixes
09/29/2010   sj   Support for DSPS style Debug Strings on PCSIM
09/27/2010   sj   Fixes for capturing the appropriate
                  filename of caller
09/21/2010   sj   Created
===========================================================================*/

/*=====================================================================
                 INCLUDE FILES
=======================================================================*/
#include "sns_common.h"
#include "sns_debug_str.h"
#include "sns_debug_api.h"
#include "sns_memmgr.h"
#include "oi_string.h" //For OI_StrLen
#include "sns_memmgr.h"
#include "sns_debug_interface_v01.h"
#include "qmi_csi.h"
#include "qmi_csi_common.h"
#include "qmi_csi_target_ext.h"

/* debug mask to filter debug messages on DSPS */
sns_debug_mask_t sns_diag_dsps_debug_mask = 0xffffffffffffffffULL;

/*===========================================================================
                  FUNCTIONS
============================================================================*/

/*===========================================================================

  FUNCTION:   sns_diag_dsps_set_debug_mask

===========================================================================*/
/*!
  @brief
  Sets the debug mask on the DSPS processor

  @param[i]
  msg_ptr: pointer to message containing debug mask that indicates which
           debug messages are enabled/disabled.

  @return
  none
*/
/*=========================================================================*/
void sns_diag_dsps_set_debug_mask(sns_diag_set_debug_mask_req_msg_v01* msg_ptr)
{
  if (msg_ptr != NULL)
  {
    sns_diag_dsps_debug_mask = msg_ptr->debug_mask.mask;
  }
}

/*===========================================================================

  FUNCTION:   sns_debug_is_module_disabled

===========================================================================*/
/*===========================================================================*/
/*!
  @brief
  Returns 1 if the the module's debug strings need to be filtered.

  @param[i] module_id        : Debug module id

  @return
  uint8_t : 1 - Module's messages should be filtered
            0 - Module's messages should NOT be filtered
*/
/*=========================================================================*/
uint8_t sns_debug_is_module_disabled(sns_debug_module_id_e module_id)
{
  uint64_t   temp_i=1;
  uint8_t    ret_value=0;

  /* Check if the messages from the module are enabled
   * based on the debug string filter mask
   */
  if ( SNS_DBG_MOD_DSPS_SAM == module_id )
  {
    ret_value = 0;
  }
  else if ((sns_diag_dsps_debug_mask & (temp_i << module_id)) != 0)
  {
    ret_value = 1;
  }

  return ret_value;
}
