#ifndef HDRBCCPC_H
#define HDRBCCPC_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                H D R   B R O A D C A S T  C O N T R O L 
       
                             P R O T O C O L
       
            C O N F I G U R A T I O N   H E A D E R   F I L E       

GENERAL DESCRIPTION
  This file contains the configuration interface of the HDR Broadcast Control 
  Protocol.
  
EXTERNALIZED FUNCTIONS
  hdrbccpc_powerup_init
    Powerup init of protocol configuration.
    
  hdrbccpc_process_ind
    Process indications given to BCCPC.
  
  hdrbccpc_process_msg
    Process messages given to BCCPC.


INITIALIZATION AND SEQUENCING REQUIREMENTS
  hdrbccpc_powerup_init must be called at startup.  
  
  Copyright (c) 2004, 2005, 2006 by Qualcomm Technologies, Inc.  All Rights Reserved.
  Copyright (c) 2007, 2008 by Qualcomm Technologies, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* EJECT */
/*===========================================================================

                           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/bc/src/hdrbccpc.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/05/08   mpa     Added support for BCMCS 2.0 phase 1
07/20/06   mpa     Mainlined REVA_L1 session config (must have on 6800)
10/21/05   pba     Added Broadcast protocol to SCM
08/28/05   mpa     Added interface functions for accessing attribute values
11/01/04   mpa     num_mon_period attribute should be a byte type.
07/02/04   mpa     Created.

===========================================================================*/

/* EJECT */
/*===========================================================================

                         INCLUDE FILES FOR MODULE
                     
===========================================================================*/
#include "comdef.h"
#include "dsm.h"
#include "hdrhai.h"

/*===========================================================================

                      DATA DECLARATIONS

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
extern void hdrbccpc_powerup_init( void );


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
extern void hdrbccpc_process_ind
(
  hdrind_ind_name_enum_type ind_name,
  hdrind_ind_data_union_type * ind_data_ptr
);

/* EJECT */
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
extern uint8 hdrbccpc_get_curr_dyn_bc_reg_period(void);

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
extern uint8 hdrbccpc_get_curr_paging_reg_period(void);

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
extern uint8 hdrbccpc_get_curr_monitor_period(void);

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
extern uint8 hdrbccpc_get_curr_num_monitor_period(void);

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
extern uint8 hdrbccpc_get_curr_bc_ovhd_period(void);

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
extern boolean hdrbccpc_registration_enabled( void );

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
extern boolean hdrbccpc_dyn_bc_reg_enabled(void);

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
extern boolean hdrbccpc_paging_reg_enabled(void);

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
extern boolean hdrbccpc_monitor_period_enabled(void);

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
extern boolean hdrbccpc_monitor_timer_enabled(void);

#endif /* HDRBCCPC_H */


