#ifndef _HSU_DEV_APP_H_
#define _HSU_DEV_APP_H_



/*==============================================================================

High-Speed USB Device Stack Application
H E A D E R   F I L E
=============================================================================*/
/**
@file hsu_dev_app.h

Defines APIs for interfacing a USB stack with a device stack application.

There are no initialization or sequencing requirements.
*/

/*=============================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/wiredconnectivity/hsu_dev_app.h#1 $
$DateTime: 2016/06/17 14:31:11 $ $Author: pwbldsvc $

when     who  what, where, why
-------- ---  ---------------------------------------------------------
06/25/10 lo   (Tech Pubs) Edited/added Doxygen comments and markup.
09/05/07 dl   Initial version

=============================================================================*/
/*=============================================================================
Copyright (c) 2007-2010 by Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
=============================================================================*/

/*=============================================================================

INCLUDE FILES FOR MODULE

=============================================================================*/

/** @ingroup hsusb_qc_api
    @{
*/

/*=============================================================================
Forward Declarations
=============================================================================*/

/*=============================================================================
Constants
=============================================================================*/

/*=============================================================================
Macros
=============================================================================*/

/*=============================================================================
Typedefs
=============================================================================*/

/** Pointer to the application context.
*/
typedef void* appctx_t;

/** Application callbacks.
*/
typedef struct {

  void (*notify_device_configured)(appctx_t appctx, jbool_t configured);
  /**< Invoked when the device has been configured. */

} app_callbacks_t;

/*==============================================================================
PUBLIC FUNCTION DECLARATIONS FOR MODULE
==============================================================================*/

/** @} */ /* end_group hsusb_qc_api */

#endif /* _HSU_DEV_APP_H_ */
