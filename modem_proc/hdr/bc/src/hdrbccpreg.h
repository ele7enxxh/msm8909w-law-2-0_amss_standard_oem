#ifndef HDRBCCPREG_H
#define HDRBCCPREG_H

/*===========================================================================

         B R O A D C A S T   R E G I S T R A T I O N   M O D U L E    

DESCRIPTION
  This contains the declarations for regional types and variables used
  in the Broadcast Registration Module
  
Copyright (c) 2005-2008 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$PVCSPath: O:/src/asw/COMMON/vcs/hdrbccpfs.h_v   1.7   08 May 2001 18:59:52   dandrus  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/bc/src/hdrbccpreg.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/23/09   wsh     CMI: featurized CMI under FEATURE_CMI
02/24/09   wsh     CMI: updated included headers due to header split
09/15/08   wsh     Addition reg. trigger based on softcombinability
09/05/08   mpa     Added support for BCMCS 2.0 phase 2
08/07/08   mpa     Fixed forced registration success report.
05/05/08   mpa     Added support for BCMCS 2.0 phase 1
08/28/05   mpa     Created Module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "hdr_variation.h"
#include "comdef.h"
#include "hdrbccp.h"

#include "hdraddr.h"

#include "hdrbit.h"
#include "hdrbccpi.h"
#include "hdrerrno.h"

/* <EJECT> */ 
/*===========================================================================

             DEFINITIONS  FROM  THE  HDR  AIR  INTERFACE  SPEC

===========================================================================*/
                                 
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/
/*=============================================================================
FUNCTION HDRBCCPREG_POWERUP_INIT

DESCRIPTION
  This function initializes the Broadcast protocol Suite on power up.
  It mainly defines some timers.
  
DEPENDENCIES
  None.

PARAMETERS
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.
=============================================================================*/
extern void hdrbccpreg_powerup_init( void );

/*=============================================================================

FUNCTION HDRBCCPREG_CONFIGURE_REGISTRATION

DESCRIPTION
  This function updates registration related configuration parameters.

DEPENDENCIES
  None.

PARAMETERS
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
  Dynamic and/or paging registration may be started or stopped depending on the 
  value of the parameters configured during session negotiation.

=============================================================================*/
extern void hdrbccpreg_configure_registration( void );

/*=============================================================================
FUNCTION HDRBCCPREG_PROCESS_REG_TIMER_EXP

DESCRIPTION
  
DEPENDENCIES
  None.

PARAMETERS
  timer_id - Which timer expired.  If this isn't the timer for the current 
             subnet, we just restart the timer..
             
RETURN VALUEi
  None.

SIDE EFFECTS
  None.
=============================================================================*/
extern void hdrbccpreg_process_reg_timer_exp 
( 
  uint32 timer_id
);

/*=============================================================================
FUNCTION HDRBCCPREG_GET_CURRENT_SUBNET

DESCRIPTION
  
DEPENDENCIES
  None.

PARAMETERS
  subnet_ptr- Pointer to allocated memory where the subnet will get copied.

RETURN VALUE
  E_SUCCESS - if operation is successful
  E_NO_DATA - otherwise 
  
PARAMETERS
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.
=============================================================================*/
extern hdrerrno_enum_type hdrbccpreg_get_current_subnet
(
  hdraddr_type subnet_ptr
);
                                 
/*=============================================================================

FUNCTION HDRBCCPREG_SUBNET_CHANGED

DESCRIPTION
  This function updates the subnet info and determines whether subnet 
  registration needs to occur.  
  
DEPENDENCIES
  None.

PARAMETERS
  None.
  
RETURN VALUE
  TRUE  - Subnet has changed
  FALSE - We are still on the same subnet or in an invalid state

SIDE EFFECTS
  None

=============================================================================*/
extern boolean hdrbccpreg_subnet_changed( void );

/*=============================================================================
FUNCTION HDRBCCPREG_FLOW_IS_PURGEABLE

DESCRIPTION
   This function determines if a flow can be removed from the undesired flow 
   list. A flow is considered useful to keep around, if it was last registered  
   or monitored "recently".
     
DEPENDENCIES
  None.

PARAMETERS
  flow_ptr - Flow entry pointer

RETURN VALUE
  TRUE  - this flow has outlived its useful period
  FALSE - otherwise

SIDE EFFECTS
  None.
=============================================================================*/
extern boolean hdrbccpreg_flow_is_purgeable
(                                         
  hdrbccp_flow_buf_type *flow_ptr
);

/*=============================================================================
FUNCTION HDRBCCPREG_ASYNC_FLOW_REGISTRATION

DESCRIPTION
   Flow registrations that are not required but allowed by the std.
   
DEPENDENCIES
  None.

PARAMETERS
  TRUE - Registration after a subnet change
  FALSE - Async Registration ()
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.
=============================================================================*/
extern void hdrbccpreg_async_flow_registration 
(
  boolean subnet_changed
);

#ifdef FEATURE_HDR_BCMCS_2p0
#error code not present
#endif /* FEATURE_HDR_BCMCS_2p0 */

#ifdef FEATURE_HDR_BCMCS_2p0
#error code not present
#endif /* FEATURE_HDR_BCMCS_2p0 */

#endif /* HDRBCCPREG_H */
