#ifndef CMSSIDM_H
#define CMSSIDM_H
/*===========================================================================

                   I D M    H E A D E R    F I L E

DESCRIPTION
  This file contains all the definitions necessary for the Call Manager
  to interface with the Idle digital mode( IDM ) module.

Copyright (c) 2003 - 2006 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR FILE

   This section contains comments describing changes made to this file.
   Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/cm/src/cmssidm.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
06/06/07   pk/cl   Added support for LN status notification from HDR
01/06/06   ic      Lint cleanup  
12/09/05   ic      Changes to cmssidm_estimate_base_station_idm() 
                   declaration so cmcall.c lints clean
10/20/05   sk      Added prototypes of functions used to access and update
                   base station IDM.
02/06/04   ws      Initial jaguar/mainline merge.
11/21/03   ic      Fixed FEATURE_HDR_HANDOFF featurization bug. 
11/19/03   ic      Featurized SS IDM object's functionality under
                   FEATURE_HDR_HANDOFF.
11/08/03   sj      Initial release.

===========================================================================*/
#include "mmcp_variation.h"
#include "target.h"
#include "customer.h"
#include "sd.h"
#include "sd_v.h"
#include "cmll.h"
#include "cmll_v.h"

#ifdef FEATURE_HDR_HANDOFF


/*===========================================================================
=============================================================================
=============================================================================
============================== CM -> IDM procs ==============================
=============================================================================
=============================================================================
===========================================================================*/


/*===========================================================================

FUNCTION cmssidm_proc_rpt_srv_ind_ss_main

DESCRIPTION
  This function should be called to process the CM_SRV_IND_INFO_F report
  from SD20 intended for SS-Main's instance.

DEPENDENCIES
  cmssidm_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern  void                           cmssidm_proc_rpt_srv_ind_ss_main
(

        const sd_si_info_s_type        *si_info_ptr
           /* Pointer to buffer containing new service info information.
           */
);



/*===========================================================================

FUNCTION cmssidm_proc_rpt_srv_ind_ss_main

DESCRIPTION
  This function should be called to process the CM_SRV_IND_INFO_F report
  from SD20 intended for SS-HDR's instance.

DEPENDENCIES
  cmssidm_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern  void                           cmssidm_proc_rpt_srv_ind_ss_hdr
(

        const sd_si_info_s_type        *si_info_ptr,
           /* Pointer to buffer containing new service info information.
           */
        boolean                        is_hdr_no_srv_timer_expire           
);

/*===========================================================================

FUNCTION cmssidm_proc_rpt_hdr_ln_status

DESCRIPTION
  This function should be called to process the 
  CM_HDR_LOCATION_NOTIFICATION_STATUS_F report

DEPENDENCIES
  cmssidm_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern  void                           cmssidm_proc_rpt_hdr_ln_status
(
        sys_hdr_location_notification_status_e_type hdr_ln_status
           /* The LN status
           */

);

/*===========================================================================

FUNCTION cmssidm_proc_hdr_reselect

DESCRIPTION
  This function should be called when the HDRMC reports Hdr reselect or not
  good for traffic.
  Implemented only for backwards compatability.

DEPENDENCIES
  cmssidm_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern  void                           cmssidm_proc_hdr_reselect
(
        void
);




/*===========================================================================

FUNCTION cmssidm_proc_call_connected

DESCRIPTION
  This function should be called to process the call connected event from
  CMCALL.

DEPENDENCIES
  cmssidm_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern  void                           cmssidm_proc_call_connected
(
        const cmcall_s_type           *call_ptr
          /* Pointer to the call object.
          */
);


/*===========================================================================

FUNCTION cmssidm_proc_call_ended

DESCRIPTION
  This function should be called to process the call end event from
  CMCALL.

DEPENDENCIES
  cmssidm_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern  void                           cmssidm_proc_call_ended
(
        void
);


/*===========================================================================

FUNCTION cmssidm_estimate_base_station_idm

DESCRIPTION
  This function should be called when the a data call ends. This function
  does a best effort estimation of the Base Station's IDM. There are some
  cases where the estimation might be wrong, but those cases are limited by
  the lack of service.

DEPENDENCIES
  cmssidm_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern  void                           cmssidm_estimate_base_station_idm
(
        const cmcall_s_type  *call_ptr
);


/*===========================================================================

FUNCTION cmssidm_proc_timer

DESCRIPTION
  This function should be called to process the CM's 1 second timer tick.

DEPENDENCIES
  cmssidm_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern  void                           cmssidm_proc_timer
(

        void
);



/*===========================================================================

FUNCTION cmssidm_get_curr_idm

DESCRIPTION
  This function should be called to get the curr_idm.

DEPENDENCIES
  None.

RETURN VALUE
  sys_sys_mode_e_type - current Idle digital mode.

SIDE EFFECTS
  None.
===========================================================================*/
extern  sys_sys_mode_e_type            cmssidm_get_curr_idm
(
        void

);


/*===========================================================================

FUNCTION cmssidm_get_prev_idm

DESCRIPTION
  This function should be called to get the previous IDM.

DEPENDENCIES
  None.

RETURN VALUE
  sys_sys_mode_e_type - Previous Idle digital mode.

SIDE EFFECTS
  None.
===========================================================================*/
extern  sys_sys_mode_e_type            cmssidm_get_prev_idm
(
        void

);


/*===========================================================================

FUNCTION cmssidm_get_base_station_idm

DESCRIPTION
  This function returns CM's best estimate of the base station's IDM.

DEPENDENCIES
  cmssidm_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern  sys_sys_mode_e_type            cmssidm_get_base_station_idm
(
        void
);


/*===========================================================================

FUNCTION cmssidm_init

DESCRIPTION
  This function should be called to initialize the IDM module.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern  void                           cmssidm_init
(
        void

);
#endif // FEATURE_HDR_HANDOFF
#endif /* CMSSIDM_H */
