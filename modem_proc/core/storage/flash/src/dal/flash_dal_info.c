/*=============================================================================
 *
 * FILE:      flash_dal_info.c
 *
 * DESCRIPTION: Function declarations specific to Flash DAL Info
 *
 * PUBLIC CLASSES:  Not Applicable
 *
 * INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A
 *
 *        Copyright © 2008-2009, 2011 Qualcomm Technologies Incorporated.
 *               All Rights Reserved.
 *            QUALCOMM Proprietary/GTDR
 *===========================================================================*/

/*===================================================================
 *
 *                       EDIT HISTORY FOR FILE
 *
 *   This section contains comments describing changes made to the
 *   module. Notice that changes are listed in reverse chronological
 *   order.
 *
 *  $Header: //components/rel/core.mpss/3.5.c12.3/storage/flash/src/dal/flash_dal_info.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $
 *
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 05/20/11     bb      Added support for multiple flash devices
 * 08/28/08     dp      Initial Revision
 *==================================================================*/

/*===================================================================
 *
 *                     Include Files
 *
 ====================================================================*/

#include "DALReg.h"
#include "DALDeviceId.h"

#include "flash_dal_fwk.h"


static DALDEVICEID DALFlash_DeviceId[2] = {DALDEVICEID_FLASH_DEVICE_1, 
  DALDEVICEID_FLASH_DEVICE_2};

const DALREG_DriverInfo DALFlash_DriverInfo =
  {flash_fwk_attach, 2, DALFlash_DeviceId};


