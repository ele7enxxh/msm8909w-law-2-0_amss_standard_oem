#ifndef HDRMRLPSPD_H
#define HDRMRLPSPD_H

/*===========================================================================
            H D R M R L P   S T A L E  P A C K E T   D R O P P I N G

GENERAL DESCRIPTION
  
 
EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
   

 Copyright (c) 2006-2009 by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/hdrdata/inc/hdrmrlpspd.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $
  
when       who        what, where, why
--------   ---        -------------------------------------------------------
09/04/09   ss         CMI SU level modifications.
12/07/06   spn        Initial Revision.


===========================================================================*/


/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "datamodem_variation.h"
#include "customer.h"

#ifdef FEATURE_HDR_QOS
#include "dsrlp_v.h"
#include "hdrmrlp_api.h"

/*---------------------------------------------------------------------------
TYPEDEF HDRMRLPSPD_TIME_STAMP_CB_TYPE

DESCRIPTION
  Type of callback function registered with RESV module on reservation
  sent.
---------------------------------------------------------------------------*/

typedef void ( *hdrmrlpspd_time_stamp_cb_type ) (
                                            dsrlp_rlp_identity_type rlp );      

                             

/*===========================================================================
                       EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/
/*=========================================================================== 
 
FUNCTION       HDRMRLPSPD_REGISTER_TIME_STAMP_CB
 
DESCRIPTION    Module interested can register for callbacks to start/stop
               time stamping the data. Presently only one module can register
               This registration is done during powerup, and is one time. 
               Hence there is no function to deregister from these callbacks
   
DEPENDENCIES   None.
 
RETURN VALUE   None.
 
SIDE EFFECTS   None.
===========================================================================*/ 
void hdrmrlpspd_register_time_stamp_cb
(
  hdrmrlpspd_time_stamp_cb_type start_time_stamp_cb,
  hdrmrlpspd_time_stamp_cb_type stop_time_stamp_cb
); 

/*=========================================================================== 
FUNCTION       HDRMRLPSPD_TRANSMIT_ABORT_TIMER_CHANGED_NOTIF
 
DESCRIPTION    Called whenever the SPD Module needs to be indicated if the 
               value of the transmit abort timer has been changed. 
               This function is called only when the RLP is activated and 
               initialized
                
DEPENDENCIES   NONE 
 
RETURN VALUE   NONE 
 
SIDE EFFECTS   Starts the timer in SPD if applicable
===========================================================================*/ 
void hdrmrlpspd_transmit_abort_timer_changed_notif ( byte rscb_index, dword time_val ) ;


/*=========================================================================== 
FUNCTION       HDRMRLPSPD_CLEANUP_FOR_RSCB
 
DESCRIPTION    Called whenever a RLP is deactivated. 
                
DEPENDENCIES   NONE 
 
RETURN VALUE   NONE 
 
SIDE EFFECTS   The timer in SPD is stopped if applicable.
===========================================================================*/ 
void hdrmrlpspd_cleanup_for_rscb ( byte rscb_index ) ;

#endif /* FEATURE_HDR_QOS */

#endif /* HDRMRLPSPD_H */
