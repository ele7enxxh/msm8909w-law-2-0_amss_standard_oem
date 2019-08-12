/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                          Ecall MSD File

GENERAL DESCRIPTION 
This file contains the API to originate an ECALL thru QXDM, 
using a dedicated command code for ECALL

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2008 by Qualcomm Technologies Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/ecall/hlap/src/ecall_diag.c#1 $ 
  $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/20/09   PA      Initial version. 
10/02/09   PA      Overloaded the Command code CM_ECALL_CALL_ORIGINATION from CM subsystem
                   will change it to ECALL command code once we get that from QXDM (Diag team) 

===========================================================================*/



/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_ECALL_APP

#include "ecall_diag.h"
#include "diagcmd.h"
#include "ecall_efs.h"

/*==========================================================================

  FUNCTION ECALL_SESSION_START_STATUS_CB

  DESCRIPTION
    Call back function sent to ecall_session_start, indicated whether ECALL session was successful or failure 

  DEPENDENCIES
    None.

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
/*lint -e818 -esym(715,user_data) Pointer parameter could be declared as pointing to const */
void ecall_session_start_status_cb
(
  ecall_session_status_type  session_status
)
{
  switch (session_status)
  {
    case ECALL_PASS:
      ECALL_MSG_0( ECALL_HIGH, "Diag Rx ECALL_PASS" );
      break; 
    
    case ECALL_FAILURE:
      ECALL_MSG_0( ECALL_HIGH, "Diag Rx ECALL_FAILURE" );
      break; 
    
    default:
      ECALL_MSG_1( ECALL_MED, "Diag Rx Unkown status = %d",
                               session_status );
      break;
  } /* switch*/

} /* ecall_session_start_status_cb */

/*===========================================================================
Function: ecall_DIAG_CALL_ORIGINATION
Description:

       This function processes the ecall specific call orig request packet
       from QXDM and sends the response back.

============================================================================*/

/*lint -esym(765, ecall_diag_call_origination)
** Can't be static, to be called by DIAG via COMMAND DISPATCH TABLE
*/
PACKED void *ecall_diag_call_origination (PACKED void* req_pkt, uint16 pkt_len)
{

  DIAG_SUBSYS_CM_CM_ECALL_CALL_ORIGINATION_req_type *req_ptr =
    (DIAG_SUBSYS_CM_CM_ECALL_CALL_ORIGINATION_req_type *)req_pkt;
  DIAG_SUBSYS_CM_CM_ECALL_CALL_ORIGINATION_rsp_type *rsp_ptr;

  const int rsp_len = sizeof(DIAG_SUBSYS_CM_CM_ECALL_CALL_ORIGINATION_rsp_type);

  /* New params */
  ecall_type_of_call    call_type = ECALL_EMERGENCY;
  ecall_activation_type activation_type;

  /*lint -save -e641*/
  /* Allocate the memory for this */
  rsp_ptr = (DIAG_SUBSYS_CM_CM_ECALL_CALL_ORIGINATION_rsp_type *)diagpkt_subsys_alloc (DIAG_SUBSYS_CM, CM_ECALL_CALL_ORIGINATION, rsp_len);
  /*lint -restore */

  /* Ensure response packet allocation */
  if (!rsp_ptr)
  {
    ECALL_MSG_1( ECALL_HIGH, "ecall diag call orig memory alloc failed for response packet %d \n",
                              pkt_len );
    return NULL;
  }

 /* Sanity check */
  if (!req_ptr)
  {
    rsp_ptr->status = FALSE;
  }
  else
  {
    switch(req_ptr->test_call)
    {
      case 0:
      call_type = ECALL_TEST ;
      break;
  
      case 1:
      call_type = ECALL_EMERGENCY ;
      break;
  
      case 2:
      call_type = ECALL_RECONFIG;
      break;
  
      default:
      ECALL_MSG_0( ECALL_MED, "Unknown req_ptr->test_call" );
      break;
    }
    activation_type = (req_ptr->type_of_call == 0) ? ECALL_MANUAL_INITIATED : ECALL_AUTOMATIC_INITIATED ;
   
    ECALL_MSG_0( ECALL_HIGH, "ECALL diag calling ECALL thru ecall_diag_call_origination....  \n" );
    rsp_ptr->status = ecall_session_start(call_type, activation_type, ecall_session_start_status_cb);

  } /* if (!req_ptr) */

  return rsp_ptr ;

} /* ECALL_DIAG_CALL_ORIGINATION() */
/*lint +esym(765, ecall_diag_call_origination)*/

/*===========================================================================
Function: ECALL_DIAG_CALL_END
Description:

       This function processes the call manager call end request
       packet from QXDM and sends the response back.

============================================================================*/
/*lint -esym(765, cmlog_cm_call_end) -esym(715,pkt_len)
** Can't be static, to be called by DIAG via COMMAND DISPATCH TABLE
*/
PACKED void *ecall_diag_call_end (PACKED void* req_pkt, uint16 pkt_len)
{
  DIAG_SUBSYS_CM_CM_ECALL_CALL_END_req_type  *req_ptr =
    (DIAG_SUBSYS_CM_CM_ECALL_CALL_END_req_type*) req_pkt;
  DIAG_SUBSYS_CM_CM_ECALL_CALL_END_rsp_type  *rsp_ptr;

  /*lint -save -e641*/
  /* Allocate the memory for this */
  rsp_ptr = (DIAG_SUBSYS_CM_CM_ECALL_CALL_END_rsp_type *) diagpkt_subsys_alloc(DIAG_SUBSYS_CM, CM_ECALL_CALL_END,
    sizeof(DIAG_SUBSYS_CM_CM_ECALL_CALL_END_rsp_type) );
  /*lint -restore */

  /* Ensure response packet allocation */
  if (!rsp_ptr)
  {
    return NULL;
  }

  /* Sanity check request packet */
  /* Checking for packet size to conform to min length required*/
  if ( !req_ptr )
  {
    rsp_ptr->status = FALSE;
  }
  else
  {
    ECALL_MSG_0( ECALL_HIGH, "ecall ecall_session_stop called" );
    rsp_ptr->status = ecall_session_stop();
  }

  return rsp_ptr ;

} /* ECALL_DIAG_CALL_END() */
/*lint +esym(765, cmlog_cm_call_end) +esym(715,pkt_len)*/

#else

void ecall_diag_dummy_func(void);

#endif //End Featurisation

