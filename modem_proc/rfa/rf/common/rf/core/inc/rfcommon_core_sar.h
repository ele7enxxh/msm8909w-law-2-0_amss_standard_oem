#ifndef RFCOMMON_CORE_SAR_H
#define RFCOMMON_CORE_SAR_H

/*!
   @file
   rfcommon_core_sar.h

   @brief
      This file contains prototype declarations and definitions necessary to
      be used within RF core module.

   @details

*/

/*===========================================================================
Copyright (c) 2011 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$DateTime: 2016/12/13 07:59:52 $ 
$Author: mplcsds1 $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/rf/core/inc/rfcommon_core_sar.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
08/12/11    ap      Fix warnings
08/11/11    ap     Modify Get SAR State API to remove parameter 
03/20/11   lcl     Initial version
============================================================================*/ 
#include "rfm_common_sar.h"
#include "rfm_mode_types.h"

/*===========================================================================

                           FUNCTION PROTOTYPES

===========================================================================*/

/*--------------------------------------------------------------------------*/
extern void rfcommon_core_sar_set_state(rfm_common_sar_state_type sar_state);

/*--------------------------------------------------------------------------*/
extern rfm_common_sar_state_type rfcommon_core_sar_get_state(void);

/*--------------------------------------------------------------------------*/
extern void rfcommon_core_sar_set_overrride(rfm_common_sar_override_type override);

#endif /* RFCOMMON_CORE_SAR_H */
