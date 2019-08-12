#ifndef WMSDC_H
#define WMSDC_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
           W I R E L E S S    M E S S A G I N G   S E R V I C E S
           wmsdc.h -- Dedicated Channel Group definitions

  The WMS module which implements the User API for SMS. This source file
  defines the data types for the Dedicated Channel group.

  -------------

    Copyright (c) 2001-2008,2010-2012 QUALCOMM Technologies Incorporated.
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/wms/src/WMSE/wmsdc.h#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

#include "mmcp_variation.h"

#if defined(FEATURE_CDSMS) || defined(FEATURE_GWSMS)

/*===========================================================================
========================  INCLUDE FILES =====================================
===========================================================================*/

#include "wms.h"
#include "wmsi.h"
#include "cm.h"

/*===========================================================================

                         DATA TYPE DECLARATIONS

===========================================================================*/

#define WMS_DUMMY_CALL_ID  0xFF

/* DC data
*/
typedef struct wms_dc_struct
{
#ifdef FEATURE_CDSMS
  uint32                idle_timeout;
    /* in milli-seconds; 0 means auto-disconnect is disbaled */

  boolean               wms_initiated_call;
    /* Flag to keep track if the call was started by WMS.  If set,
     * and auto-dis is disabled, wms is responsible for tearing
     * down call also once rec queue is empty
     */
  boolean               call_active;
    /* Flag to keep track if call is active or not */

  rex_timer_type        call_out_timer;
  rex_timer_type        idle_timer;
  boolean               auto_disc_active;
  wms_dc_so_e_type      default_so_from_nv;
  boolean               is_access_channel_enabled;
  boolean               is_traffic_channel_enabled;
  boolean               is_voice_data_call_conn;
  uint8                 dc_call_id;

#endif /* FEATURE_CDSMS */

#ifdef FEATURE_GWSMS
  uint32                dummy;
#endif /* FEATURE_GWSMS */

} wms_dc_s_type;



/*===========================================================================

                       FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================
FUNCTION wms_dc_s_ptr

DESCRIPTION
  Return the pointer to the DC data.

DEPENDENCIES
  none

RETURN VALUE
  Pointer to the internal DC data.

SIDE EFFECTS
  None.
===========================================================================*/
wms_dc_s_type* wms_dc_s_ptr
(
  void
);



/*===========================================================================
FUNCTION wms_dc_process_cmd

DESCRIPTION
  Dispatch the DC command processing.

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wms_dc_process_cmd(
  wms_cmd_type                       *cmd_ptr
);



/*===========================================================================
FUNCTION wms_dc_init

DESCRIPTION
  Do DC-related initialization.

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  DC data initialized.
===========================================================================*/
void wms_dc_init(
  void
);


#ifdef FEATURE_CDSMS

void wms_dc_call_out_timer_proc(
  void
);

void wms_dc_idle_timer_proc(
  void
);

#endif /* FEATURE_CDSMS */

#endif /* FEATURE_CDSMS || FEATURE_GWSMS */

#endif /* WMSDC_H */
