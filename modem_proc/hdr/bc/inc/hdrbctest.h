#ifndef HDRBCTEST_H
#define HDRBCTEST_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                H D R   B R O A D C A S T   L O G I C A L 
                
                        T E S T   M O D E   A P P
       
                  E X T E R N A L   H E A D E R   F I L E       

GENERAL DESCRIPTION
  This file contains the external interface of the HDR Broadcast 
  Test Mode app functionality.
  
EXTERNALIZED FUNCTIONS
  hdrbctest_init
    Initialize the test mode application.
    
INITIALIZATION AND SEQUENCING REQUIREMENTS
  The application must be initialized after system startup.
  
  Copyright (c) 2003 by Qualcomm Technologies, Inc.  All Rights Reserved.                                                            
  Copyright (c) 2004 by Qualcomm Technologies, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* EJECT */
/*===========================================================================

                           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/bc/inc/hdrbctest.h#1 $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/25/04   kss     Ported from QC Corp.

===========================================================================*/

/* EJECT */
/*===========================================================================

                         INCLUDE FILES FOR MODULE
                     
===========================================================================*/
#include "comdef.h"

/*===========================================================================

FUNCTION HDRBCTEST_ENABLE

DESCRIPTION
  The function enables a test mode flow over the HDR BCMCS air interface.
  Test mode packets must be transmitted on the requested flow.
  
DEPENDENCIES
  None.

PARAMETERS
  flow_id - The desired flow that carries test mode packets.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrbctest_enable( 
  uint32 flow_id 
);

/*===========================================================================

FUNCTION HDRBCTEST_DISABLE

DESCRIPTION
  The function disables interest in a test mode flow.
  
DEPENDENCIES
  None.

PARAMETERS
  flow_id - The flow to disable.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrbctest_disable( 
  uint32 flow_id 
);


/*===========================================================================

FUNCTION HDRBCTEST_INIT

DESCRIPTION
  This function initializes the test mode app prior to use.  
  
DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrbctest_init( void );


/*===========================================================================

FUNCTION HDRBCTEST_RESET_METRICS

DESCRIPTION
  This function clears metrics for the specified flow. 
    
DEPENDENCIES
  None.

PARAMETERS
  flow_id - The flow whose metrics we are going to clear.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrbctest_reset_metrics
( 
  uint32 flow_id
);


#endif
