/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                 H D R   B R O A D C A S T   C O N T R O L 
       
                P R O T O C O L   C O N F I G U R A T I O N     

GENERAL DESCRIPTION
  This module contains the implementation of the HDR Broadcast Control 
  Protocol Configuration
    
EXTERNALIZED FUNCTIONS
    
INITIALIZATION AND SEQUENCING REQUIREMENTS
  The protocol must be initialized after system startup prior to activating 
  the protocol.  
  
  Copyright (c) 2004,2005,2006 by Qualcomm Technologies, Inc.  All Rights Reserved.
  Copyright (c) 2007,2008 by Qualcomm Technologies, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* EJECT */
/*===========================================================================

                           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/bc/src/hdrbccpc.c#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/05/08   mpa     Added support for BCMCS 2.0 phase 1
08/22/07   kss     Minor lint fix.
07/20/06   mpa     Mainlined REVA_L1 session config (must have on 6800)
04/07/06   hal     Changed hdrutil_new_buffer() to use hdrutil_new_msg_buffer()
04/05/06   mpa     Fixed typo in checking whether monitor period is enabled
12/13/05   pba     Modified SCMDB calls to use updated API's
12/04/05   mpa     Set prior session info on session close but do not reset 
                   the current instance.
11/15/05   mpa     Remove dependency on HDR session.
10/21/05   pba     Added Broadcast protocol to SCM
08/28/05   mpa     Added interface functions for accessing attribute values
12/07/04   hal     Changed dsm_offset_new_buffer to hdrutil_new_buffer.
11/01/04   mpa     Read/Write config from/to NV.
07/02/04   mpa     Created.

===========================================================================*/

/* EJECT */
/*===========================================================================

                          INCLUDE FILES FOR MODULE
                     
===========================================================================*/
#include "hdr_variation.h"
#include "dsm.h"
#include "comdef.h"
#include "err.h"

#include "hdrbit.h"
#include "hdrtrace.h"
#include "hdrdebug.h"
#include "hdrutil.h"

#include "hdrhmp.h"
#include "hdrscp.h"

#ifdef FEATURE_HDR_BCMCS
#include "hdrscmbccp.h"
#include "hdrscmdb.h"

#include "hdrbccpc.h"

/*===========================================================================

             DEFINITIONS  FROM  THE  HDR  AIR  INTERFACE  SPEC

===========================================================================*/


/*===========================================================================

                           TYPE DECLARATIONS

===========================================================================*/

/* Configuration parameter data. */
typedef struct
{

  hdrscmbccp_config_params_type current;

} hdrbccpc_struct_type;

STATIC hdrbccpc_struct_type hdrbccpc;


/*===========================================================================
 For HDR Release A builds BCCP configuration message handling is done through 
 SCM module and BCCPC gets a copy of current configuration from SCMDB. So for
 Rel A builds only powerup init, indication processing and attribute accessor 
 functions are required.
 ===========================================================================*/

/*=============================================================================
FUNCTION HDRBCCPC_POWERUP_INIT

DESCRIPTION
  This function should be called once upon system startup.  The protocol 
  configuration is initialized. 
  
DEPENDENCIES
  None.

PARAMETERS
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None.
=============================================================================*/
void hdrbccpc_powerup_init( void )
{
  hdrscmbccp_config_params_type *config;  /* Read Configuration from EFS */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  /* Setup Current Configuration */
  config = &hdrbccpc.current;

  hdrscmdb_get_inuse_config_and_verno( 
    HDRHAI_BROADCAST_PROTOCOL,
    config,
    NULL, 
    0 );

} /* hdrbccpc_powerup_init() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRBCCPC_PROCESS_IND

DESCRIPTION
  This function processes indications given to hdrbccpc.

DEPENDENCIES
  None

PARAMETERS
  ind_name     - Name of the indication to process
  ind_data_ptr - Any data associated with the indication
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrbccpc_process_ind
(
  hdrind_ind_name_enum_type ind_name,
  hdrind_ind_data_union_type * ind_data_ptr
)
{
  switch (ind_name)
  {

    case HDRIND_SCM_SESSION_CHANGED:
      /*-------------------------------------------------------------------------
      Retrieve the Session Configuration Parameters
      -------------------------------------------------------------------------*/
      hdrscmdb_get_inuse_config_and_verno( 
        HDRHAI_BROADCAST_PROTOCOL,
        &hdrbccpc.current,
        NULL,
        0 );

      break;   


    default:
      ERR_FATAL( "Unknown indication (%d)", ind_name, 0, 0 );
      break;
  }

} /* hdrbccpc_process_ind() */

/*=============================================================================

FUNCTION HDRBCCPC_GET_CURR_DYN_CB_REG_PERIOD

DESCRIPTION
  This accessor function returns current configured value for the 
  DynamicBroadcastRegistrationPeriod parameter.

DEPENDENCIES
  None
  
PARAMETERS
  None

RETURN VALUE
  dyn_bc_reg_period - Current DynamicBroadcastRegistration Period

SIDE EFFECTS
  None

=============================================================================*/
uint8 hdrbccpc_get_curr_dyn_bc_reg_period(void)
{
  
  return (hdrbccpc.current.dyn_bc_reg_period);

} /* hdrbccpc_get_curr_dyn_bc_reg_period() */

/* <EJECT> */
/*=============================================================================

FUNCTION HDRBCCPC_GET_CURR_PAGING_REG_PERIOD

DESCRIPTION
  This accessor function returns current configured value for the 
  PagingRegistrationPeriod parameter.

DEPENDENCIES
  None
  
PARAMETERS
  None

RETURN VALUE
  paging_reg_period - Current PagingRegistration Period

SIDE EFFECTS
  None

=============================================================================*/
uint8 hdrbccpc_get_curr_paging_reg_period(void)
{
  
  return (hdrbccpc.current.paging_reg_period);

} /* hdrbccpc_get_curr_paging_reg_period() */

/* <EJECT> */
/*=============================================================================

FUNCTION HDRBCCPC_GET_CURR_MONITOR_PERIOD

DESCRIPTION
  This accessor function returns current configured value for the 
  MonitorPeriod parameter.

DEPENDENCIES
  None
  
PARAMETERS
  None

RETURN VALUE
  monitor_period - Current Monitor Period

SIDE EFFECTS
  None

=============================================================================*/
uint8 hdrbccpc_get_curr_monitor_period(void)
{
  
  return (hdrbccpc.current.monitor_period);

} /* hdrbccpc_get_curr_monitor_period() */

/* <EJECT> */
/*=============================================================================

FUNCTION HDRBCCPC_GET_CURR_NUM_MONITOR_PERIOD

DESCRIPTION
  This accessor function returns current configured value for the 
  NumMonitorPeriod parameter.

DEPENDENCIES
  None
  
PARAMETERS
  None

RETURN VALUE
  num_monitor_period - Current Number of Monitor Period

SIDE EFFECTS
  None

=============================================================================*/
uint8 hdrbccpc_get_curr_num_monitor_period(void)
{
  
  return (hdrbccpc.current.num_monitor_period);

} /* hdrbccpc_get_curr_num_monitor_period() */

/* <EJECT> */
/*=============================================================================

FUNCTION HDRBCCPC_GET_CURR_BROADCAST_OVERHEAD_PERIOD

DESCRIPTION
  This accessor function returns current configured value for the 
  BroadcastOverhead parameter.

DEPENDENCIES
  None
  
PARAMETERS
  None

RETURN VALUE
  bcast_overhead_period - Current BroadcastOverhead Period

SIDE EFFECTS
  None

=============================================================================*/
uint8 hdrbccpc_get_curr_bc_ovhd_period(void)
{
  
  return (hdrbccpc.current.bcast_overhead_period);

} /* hdrbccpc_get_curr_bc_ovhd_period() */

/* <EJECT> */
/*=============================================================================

FUNCTION HDRBCCPC_REGISTRATION_ENABLED

DESCRIPTION
  This function returns whether the AT should perform periodic registration.
  (Either dynamic or for paging).
  
DEPENDENCIES
  Should only be called when broadcast is available.

PARAMETERS
  None
  
RETURN VALUE
  TRUE  - Perform registration for dynamic broadcast and/or paging
  FALSE - Periodic registration is disabled

SIDE EFFECTS
  None

=============================================================================*/
boolean hdrbccpc_registration_enabled( void )
{

  boolean registration_enabled = FALSE;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*---------------------------------------------------------------------------
  From C.S0054 ver1.0 section 2.4.4.3.3.1
  
  The access terminal shall maintain an indicator of BCMCS Flow Registration
  enable status, called RegistrationEnabled.
  
  If PagingRegistrationPeriod is zero, and DynamicBroadcastRegistrationPeriod
  is zero, the access terminal shall set RegistrationEnabled to NO.  
  Otherwise, the access terminal shall set RegistrationEnabled to YES.    
  ---------------------------------------------------------------------------*/
  registration_enabled = (hdrbccpc.current.dyn_bc_reg_period != 0) ||
                         (hdrbccpc.current.paging_reg_period != 0);

  if (registration_enabled)
  {
    HDR_MSG_PROT(MSG_LEGACY_HIGH,"Broadcast Registration is Enabled");
  }
  else
  {
    HDR_MSG_PROT(MSG_LEGACY_HIGH,"Broadcast Registration is Disabled");
  }

  return registration_enabled;

} /* hdrbccpc_registration_enabled() */

/* <EJECT> */
/*=============================================================================

FUNCTION HDRBCCPC_DYN_BC_REG_ENABLED

DESCRIPTION
  This query function determines whether Timer based BCMCS Flow registration 
  for dynamic assignment of BCMCS Flow to physical channels is enabled
  
DEPENDENCIES
  None
  
PARAMETERS
  None

RETURN VALUE
  TRUE  - Dynamic Broadcast Registration is enabled
  FALSE - Dynamic Broadcast Registration is disabled
  
SIDE EFFECTS
  None

=============================================================================*/
boolean hdrbccpc_dyn_bc_reg_enabled(void)
{

  return (hdrbccpc.current.dyn_bc_reg_period != 0);

} /* hdrbccpc_dyn_bc_reg_enabled() */

/* <EJECT> */
/*=============================================================================

FUNCTION HDRBCCPC_PAGING_REG_ENABLED

DESCRIPTION
  This query function determines whether Timer based BCMCS Flow registration 
  for paging is enabled
  
DEPENDENCIES
  None
  
PARAMETERS
  None

RETURN VALUE
  TRUE  - Paging Registration is enabled
  FALSE - Paging Registration is disabled
  
SIDE EFFECTS
  None

=============================================================================*/
boolean hdrbccpc_paging_reg_enabled(void)
{

  return (hdrbccpc.current.paging_reg_period != 0);

} /* hdrbccpc_paging_reg_enabled() */

/* <EJECT> */
/*=============================================================================

FUNCTION HDRBCCPC_MONITOR_PERIOD_ENABLED

DESCRIPTION
  This query function determines whether the Monitor Timer is enabled 

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  TRUE  - Monitor Period is enabled
  FALSE - Monitor Period is disabled

SIDE EFFECTS
  None
=============================================================================*/
boolean hdrbccpc_monitor_period_enabled(void)
{

  return (hdrbccpc.current.monitor_period != 0);

} /* hdrbccpc_monitor_period_enabled() */

/* <EJECT> */
/*=============================================================================

FUNCTION HDRBCCPC_MONITOR_TIMER_ENABLED

DESCRIPTION
  This query function determines whether the Monitor Timer is enabled 

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  TRUE  - Monitor Timer is enabled
  FALSE - Monitor Timer is disabled

SIDE EFFECTS
  None
=============================================================================*/
boolean hdrbccpc_monitor_timer_enabled(void)
{

  return ( (hdrbccpc.current.monitor_period != 0) &&
           (hdrbccpc.current.num_monitor_period != 0) );

} /* hdrbccpc_monitor_timer_enabled() */

#endif /* FEATURE_HDR_BCMCS */
