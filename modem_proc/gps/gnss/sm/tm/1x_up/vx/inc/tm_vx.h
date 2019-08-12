#ifndef TM_VX_H
#define TM_VX_H

#include "gps_variation.h"
#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================

                  TM Vx Header File

DESCRIPTION
  This header file contains definitions of data structure necessary fo
  Vx message packing/unpacking.

  Copyright (c) 2006 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/1x_up/vx/inc/tm_vx.h#1 $  
  
when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/17/14   skm    NV Async Read changes  
07/02/08   ank     Added LBS support for CDG OMH R-UIM Spec
10/31/06   cl      Initial Release
===========================================================================*/

/*=============================================================================

                                INCLUDE FILES

=============================================================================*/
#include "tm_common.h"
#include "pdapi.h"
#include "tm_prtl_iface.h"


/*===========================================================================
FUNCTION tm_vx_prtl_init

DESCRIPTION
  This function is called by TM-Core to initialize Vx module
  
DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
extern void tm_vx_prtl_init(void);

/*===========================================================================
FUNCTION tm_vx_pd_info_available

DESCRIPTION
  This function is called by is801 driver module to provide pos info.
  
DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
extern void tm_vx_pd_info_available
(  
  pdsm_pd_info_s_type  *pd_info_ptr,
  qword                time,
  pdsm_pd_end_e_type   end_status  
);


/*===========================================================================

FUNCTION tm_vx_fallback_to_standalone 

DESCRIPTION
  This function tries to continue with an SA session when called in the event
  of failures in the Vx phase. It does so by calling into the IS801
  driver module with the newly introduced TM_IS801_DRV_REQ_START_NODWLD mode.
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
boolean tm_vx_fallback_to_standalone(void);

/*===========================================================================
FUNCTION tm_vx_reinit

DESCRIPTION
  This function is called to reset Vx module to a clean state

DEPENDENCIES 
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
 None

===========================================================================*/
extern void tm_vx_reinit
(
  boolean                     stop_tm_core, 
  tm_sess_stop_reason_e_type  tm_core_stop_reason
);


#ifdef FEATURE_UIM_SUPPORT_LBS
/*===========================================================================
FUNCTION: tm_vx_ruim_read

DESCRIPTION:

PARAMETERS:

===========================================================================*/
extern void tm_vx_ruim_read(void);

/*===========================================================================
FUNCTION: tm_vx_ruim_read_complete

DESCRIPTION:

PARAMETERS:

===========================================================================*/
extern void tm_vx_ruim_read_complete(const prtl_event_u_type  *event_payload);
#endif /* FEATURE_UIM_SUPPORT_LBS */

#ifdef __cplusplus
}
#endif

#endif /* TM_VX_H */


