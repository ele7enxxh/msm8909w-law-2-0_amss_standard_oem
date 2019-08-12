
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

              H D R   M E S S A G I N G   P R O T O C O L
                S T A R T   A C C E S S    A T T E M P T

GENERAL DESCRIPTION

This module encapsulates the logic involved in starting an access attempt.

EXTERNALIZED FUNCTIONS (Global)
  None
  
EXTERNALIZED FUNCTIONS (Regional)
  void hdrhmpsa_init( void )
  void hdrhmpsa_start( void )
  void hdrhmpsa_check_ovhd( void )
  void hdrhmpsa_lmac_ready( void )
  void hdrhmpsa_abort( void )


INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2000 - 2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

$PVCSPath: O:/src/asw/COMMON/vcs/hdrhmpsa.c_v   1.11   15 May 2002 14:12:18   vsali  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/src/hdrhmpsa.c#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who    what, where, why
--------   ---    ----------------------------------------------------------
04/01/14   vko    Fixed race between amac start attempt and bundling due to ROR
04/28/14   vko    Removed changes for unbundled CR and bundled ROR
03/25/14   ukl    Fixed a race between unbundled CR and bundled ROR
09/20/12   sju    QChat UPK changes in baseline AMSS
03/24/05   pba    Lint cleanup
11/29/04   dna    Avoid potentially leaving HDRSRCH in access mode when idle.
03/16/04   dna    Re-start HMPSA state machine if it is in the middle of
                  starting another access attempt when one ends.
09/10/03   mpa    Converted F3 messages to use new MSG2.0 HDR PROT SSID
05/15/02   dna    In hdrhmpsa_check_ovhd(), first check if there is a need
                  to start the access attempt before checking ovhd info.
04/16/02   dna    Avoid race where we might activate ACMAC while in sleep
04/02/02   dna    Demoted an ERR_FATAL to MSG_HIGH because the condition can 
                  occur under normal operation and has no side effect.
05/03/01   dna    Re-naming for consistency.
01/09/01   kss    Changed hdramac_activate() call to hdramac_start_attempt() 
                  due to access MAC interface changes.
08/22/00   dna    Created module.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "hdr_variation.h"
#include "err.h"
#include "hdrdebug.h"
#include "hdramac.h"
#include "hdrlmac.h"
#include "hdrovhd.h"
#include "hdrtrace.h"
#include "hdrslp.h"
#ifdef FEATURE_HDR_QCHAT
#include "hdrqchatupk_opts.h"
#include "hdrqchatupk.h"
#endif /*FEATURE_HDR_QCHAT*/
#include "hdridles.h"
#include "hdrhmp.h"

/* <EJECT> */
/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/
#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */
 

LOCAL struct
{
  enum
  {
    HDRHMPSA_START,
    HDRHMPSA_WAITING_FOR_LMAC_RESPONSE,
    HDRHMPSA_WAITING_FOR_OVERHEAD_MESSAGES,
    HDRHMPSA_WAITING_FOR_LMAC_RESPONSE_TO_RESTART
  } state;
  boolean amac_given_attempt;
} hdrhmpsa;    

/*===========================================================================

FUNCTION HDRHMPSA_INIT

DESCRIPTION
  This function performs initialization required for HMPSA every time HDR
  mode is entered.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrhmpsa_init( void )
{

  HDR_MSG_PROT (MSG_LEGACY_LOW,  "HMPSA to START state");
  hdrhmpsa.state = HDRHMPSA_START;
  hdrhmpsa.amac_given_attempt = FALSE;
} /* hdrhmpsa_init() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRHMPSA_START

DESCRIPTION
  This function commands the LMAC into access mode, if necessary, and 
  changes the state of the access attempt setup to indicate we are waiting
  for LMAC to enter access mode.
  

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrhmpsa_start( void )
{
  #ifdef FEATURE_HDR_QCHAT
    hdrqchatupk_hmpsa_start_procedure();
  #endif
  if (hdrhmpsa.state == HDRHMPSA_START)
  {
    HDRTRACE_HMP_WAITING_TO_ACTIVATE_ACMAC();
    hdrlmac_access_mode(HDRHAI_HDR_MESSAGING_PROTOCOL);
    hdrhmpsa.state = HDRHMPSA_WAITING_FOR_LMAC_RESPONSE;
    HDR_MSG_PROT (MSG_LEGACY_LOW,  "HMPSA to WAITING_FOR_LMAC_RESPONSE state");
  }
  else
  {
    HDR_MSG_PROT_1 (MSG_LEGACY_HIGH, "Another AC msg for tx before start %d", 
                    hdrhmpsa.state);
  }
} /* hdrhmpsa_start() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRHMPSA_RESTART

DESCRIPTION
  If HMPSA is in the middle of starting an access attempt, this command 
  re-starts the state machine from the beginning.  This is needed if
  HDRSRCH returns to idle mode after HMPSA commands it to access mode but
  before valid overhead info is received.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrhmpsa_restart( void )
{
  if (hdrhmpsa.state == HDRHMPSA_WAITING_FOR_OVERHEAD_MESSAGES)
  {
    hdrhmpsa.state = HDRHMPSA_START;
    if (!hdrslp_tx_queue_is_empty( HDRHAI_HDR_MESSAGING_PROTOCOL ))
    {
      /* If there are message queued, try to start another access.
       * Otherwise we will try when the message is re-queued.
       */
      hdrhmpsa_start();
      HDR_MSG_PROT (MSG_LEGACY_HIGH, "HMPSA RE-START");
    }
    else
    {
      HDR_MSG_PROT (MSG_LEGACY_MED, "HMPSA no access attempt needed");
    }
  }
  else if (hdrhmpsa.state == HDRHMPSA_WAITING_FOR_LMAC_RESPONSE)
  {
    hdrhmpsa.state = HDRHMPSA_WAITING_FOR_LMAC_RESPONSE_TO_RESTART;
  }
} /* hdrhmpsa_restart() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRHMPSA_CHECK_OVHD

DESCRIPTION
  This function activates the ACMAC if necessary and possible.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrhmpsa_check_ovhd( void )
{
  if ( hdrhmpsa.state == HDRHMPSA_WAITING_FOR_OVERHEAD_MESSAGES )
  {
    if (hdrslp_tx_queue_is_empty( HDRHAI_HDR_MESSAGING_PROTOCOL ))
    {
      /* It is possible that ACMAC already sent out all the messages and 
       * we already went back to sleep, in which case there is no need
       * to start this access attempt after all.
       */
      hdrhmpsa.state = HDRHMPSA_START;
      HDR_MSG_PROT (MSG_LEGACY_MED,  "HMPSA no access attempt needed");
    }
    else if ( hdrovhd_info_is_updated( HDRHAI_HDR_MESSAGING_PROTOCOL ) && 
              hdrovhd_ap_is_updated( HDRHAI_HDR_MESSAGING_PROTOCOL ) )
    {
		/* We are good to go.  Activate the ACMAC */

		  /* Priority dependency - If ACMAC is in a lower priority task or if 
		   * OVHD is in a different task than HMP, there is potential for
		   * ACMAC to discover the overhead information is not current when
		   * it is activated.
		   */
		   if((hdridles_get_csetup_substate() == HDRIDLES_CSETUP_CREQ_SLP) &&
              (hdrslp_is_waiting_for_bundling() == TRUE))
          {	  
             HDR_MSG_PROT (MSG_LEGACY_HIGH,  "HMPSA: A CR already queued and bundling is set. RESET bundling now.");
             hdrhmp_stop_waiting_for_bundling_msg();
          }
          HDR_MSG_PROT (MSG_LEGACY_HIGH,  "HMPSA: amac given attempt is set.");
	      hdrhmpsa.amac_given_attempt = TRUE;
	  
		  hdramac_start_attempt( HDRHAI_HDR_MESSAGING_PROTOCOL );
		  hdrhmpsa.state = HDRHMPSA_START;
		  HDR_MSG_PROT (MSG_LEGACY_MED,  "HMPSA started access attempt");	
    }
    else
    {
      HDR_MSG_PROT (MSG_LEGACY_HIGH,  "Still waiting to activate ACMAC");
    }
  }
} /* hdrhmpsa_check_ovhd() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRHMPSA_LMAC_READY

DESCRIPTION
  This function changes the state to indicate we are now ready for 
  overhead messages.
  

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrhmpsa_lmac_ready( void )
{
  if (hdrhmpsa.state == HDRHMPSA_WAITING_FOR_LMAC_RESPONSE)
  {
    /* Once LMAC is in access mode we can activate the ACMAC */
    hdrhmpsa.state = HDRHMPSA_WAITING_FOR_OVERHEAD_MESSAGES;
    HDR_MSG_PROT (MSG_LEGACY_LOW, 
                  "HMPSA to WAITING_FOR_OVERHEAD_MESSAGES state");
    hdrhmpsa_check_ovhd();
  }
  else if (hdrhmpsa.state == HDRHMPSA_WAITING_FOR_LMAC_RESPONSE_TO_RESTART)
  {
    /* We were commanded to re-start while waiting for LMAC response,
     * so to avoid the need for a seqence number in the LMAC responses
     * we simply wait for the response, then re-start.
     */
    hdrhmpsa.state = HDRHMPSA_START;
    if (!hdrslp_tx_queue_is_empty( HDRHAI_HDR_MESSAGING_PROTOCOL ))
    {
      /* If there are message queued, try to start another access.
       * Otherwise we will try when the message is re-queued.
       */
      hdrhmpsa_start();
      HDR_MSG_PROT (MSG_LEGACY_HIGH, "HMPSA Delayed RE-START");
    }
    else
    {
      HDR_MSG_PROT (MSG_LEGACY_MED, "HMPSA Delayed no access attempt needed");
    }
  }
  else 
  {
    /* In hybrid mode we can wait a long time for the LMAC response.
     * This is the race condition where we got a supervision timeout 
     * and returned to acquisition and at the same time LMAC finally 
     * responed that it entered access mode.
     */
    HDR_MSG_PROT (MSG_LEGACY_HIGH, "LMAC entered access mode unexpectedly");
  }
} /* hdrhmpsa_lmac_ready() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRHMPSA_ABORT

DESCRIPTION
  This function aborts any attempt that may be in progress to set up an
  access attempt.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrhmpsa_abort( void )
{
  if ( hdrhmpsa.state != HDRHMPSA_START )
  {
    hdrhmpsa.state = HDRHMPSA_START;
	hdrhmpsa.amac_given_attempt = FALSE;
    HDR_MSG_PROT (MSG_LEGACY_LOW,  "HMPSA to START state");
    HDR_MSG_PROT (MSG_LEGACY_HIGH,  "Aborting access attempt setup");
  }
} /* hdrhmpsa_abort() */

/*===========================================================================

FUNCTION HDRHMPSA_AMAC_GIVEN_ATTEMPT

DESCRIPTION
  This function returns the current value of amac given attempt flag

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  current value of amac given attempt flag

SIDE EFFECTS
  None

===========================================================================*/
boolean hdrhmpsa_amac_given_attempt( void )
{
  	return hdrhmpsa.amac_given_attempt;
} /* hdrhmpsa_amac_given_attempt() */

/*===========================================================================

FUNCTION HDRHMPSA_RESET_AMAC_GIVEN_ATTEMPT

DESCRIPTION
  This function resets the amac given attempt flag to FALSE.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrhmpsa_reset_amac_given_attempt( void )
{
  	hdrhmpsa.amac_given_attempt = FALSE;  
	HDR_MSG_PROT (MSG_LEGACY_HIGH,  "Resetting amac given attempt");
} /* hdrhmpsa_reset_amac_given_attempt() */