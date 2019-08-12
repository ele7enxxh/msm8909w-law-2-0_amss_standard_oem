#ifndef CMIPAPP_H
#define CMIPAPP_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*===========================================================================

               C A L L   M A N A G E R   I P   A P P L I C A T I O N

                        H E A D E R   F I L E

DESCRIPTION

   This header describes functions that interface an application (VoIP) to
   Call Manager. Call Manager can interact with the application to place
   calls on whatever technology application may support. This way current
   Call Manager interfaces to its clients need not change and at the same
   time clients get to make use of new technology.

  Copyright (c) 2004 - 2013 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/public/cmipapp.h#1 $


when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
05/10/14   sm      Multiple SKT req FR changes 
06/25/13   sm      IMS deregister request for subs unavailable cmd  
06/05/13   qf      Added support for IMS rat_change between LTE and WLAN
04/23/13   xs      USSD detect support
04/08/13   xs      Notify NAS of hold call end after srvcc
01/07/12   xs      Added PS supplemetary service support
01/04/13   th      Add new end cause CMIPAPP_END_CAUSE_RETRY_AFTER
08/21/12   vk      Added TIR/OIR support for VOLTE 
05/05/12   xs      Support sending 486 busy here end cause to client
04/25/12   gm      Refactor IPAPP callback interface 
03/27/12   xs      Add call attribute info in connected_ind
03/27/12   xs      VoLTE Remove of user from Conf-call
03/08/12   xs      Added codec info support for VOLTE
02/13/12   xs      VT upgrade/downgrade support
01/06/12   gm      Data synchronization changes for freefloating platform
01/05/12   xs      Added support of remote party hold notification
01/04/12   xs      Add new IMS call end error codes
10/26/11   xs      L->W SRVCC support
08/25/11   xs      Add user data block support for some APIs
06/29/11   jqi     VoIP early media support 
08/16/10   aj      support for IMS deregistration during PS detach
06/18/10   xs      Add SENTINEL metacomments for maximum value sentinel in the enum 
03/16/10   jqi     EHRPD VOIP silent redial support.
01/26/10   np      Added LTE serving status info
01/15/10   aj      Added VOIP support for emergency call
12/02/09   pm      Add PARAM to resolve htorpc_cmiapp error
11/06/09   mp      Added support to remove the last party added for VOIP calls
11/06/09   pm      Added CMPIAPP_CALL_TYPE_REG
09/10/09   rm      Removing inclusion of customer.h from /api files
08/31/09   rm      Remove inclusion of cmutil.h as that is private file
03/01/09   sv      Making cmipapp_rpt_s_type private and removing cmll_v.h
02/23/09   rm      CMI Phase-2 : Removing inclusion of private header files
                   from public  
12/15/08   jd      Adding Doxygen Commenting 
11/28/08   rm      CMI defeaturisation
11/13/08   rn/cl   Corrected metacomment for below fix
10/30/08   jd      Added changes for IMS registration
10/27/08   cl      Added support for KTFT Voip Call Waiting Feature
05/23/08   aj      Adding metacomments for IMS support. 
                   Comment out cmipapp_get_ph_identity
03/29/08   rm      Adding UMTS interface support for cmipapp
12/07/07   ra      Added support for IMS VCC in HOMER files 
06/21/07   jqi     Fixed meta comemnts for cmipapp_ind_u_type. 
06/18/07   ic      Added meta comments for cmipapp_sms_cmd_mo_sms_f_type
03/07/07   jqi     Added support for FEATURE_SMS_OVER_IP
02/26/07   ka      Changes to support Voip deregistration.
01/03/07   pk      Added support for DO VOIP -> 1X Handoff
01/12/07   ka      Adding metacomments.
01/09/07   ka      Changing 0xFFFFFFFF to 0x7FFFFFFF to satisfy C++ compilers
12/25/06   ka      Including customer.h
08/16/06   ic      Fixed compiler warnings (comment inside comment)
07/31/06   ka      Adding Explicit call transfer and Multiparty support for
                   VoIP.
06/27/06   ka      Adding feature flag FEATURE_IP_CALL
05/07/06   ka      Adding callback functions for activating/deactivating 
                   answer hold
02/17/06   ka      Adding support for videoshare
12/11/05   ic      Lint cleanup
11/23/05   ka      Modification to allow hold/active/dtmf for ip calls.
10/08/05   ic      Added Header: field 
09/26/05   ka      Adding structure cmipapp_ind_mt_ring_cnf_s.
04/20/05   ka      Informing IP apps when there is a service change
04/06/05   ka      Removed mo ring back structure, adding sys_mode to call
                   orig and removed headers to find_active_app() and
                   get_cb_ptrs ()
03/14/05   ka      Cleaning hold, retrieve and other structures
08/27/04   dk/ka   Initial release.
===========================================================================*/

/** @defgroup CMAPI Call Management API
** @{
*/

/*------------------------------------------------------------------------------
**   Call Management API Includes
**------------------------------------------------------------------------------
*/


#include "cm.h"
#include "cmll.h"   /* Includes cm_hdr_type, cm_name_e_type */
#include "cmipsups.h"

#define IMS_SUPPORT_ON_SUBS

/* cmipapp call types that are already being used by clients. These were 
** introduced before ip app's could include cm.h. If cm.h is included by 
** app's then use CM_CALL_TYPE_X_MASK instead.
*/

#define CMIPAPP_CALL_TYPE_NONE       0
    /* FOR INTERNAL USE OF CM ONLY!
    ** Since it is assigned to bit mask -1 is not used
    */

#define CMIPAPP_CALL_TYPE_VOICE      CM_CALL_TYPE_VOICE_MASK
    /* Voice call
    */

#define CMIPAPP_CALL_TYPE_EMERGENCY  CM_CALL_TYPE_EMERG_MASK
    /* Emergency call
    */

#define CMIPAPP_CALL_TYPE_VT         CM_CALL_TYPE_VT_MASK
    /* VT call
    */

#define CMIPAPP_CALL_TYPE_VS         CM_CALL_TYPE_VS_MASK
    /* VS call
    */

#define CMIPAPP_CALL_TYPE_SMS        CM_CALL_TYPE_SMS_MASK
    /* SMS
    */

#define CMIPAPP_CALL_TYPE_SUPS        CM_CALL_TYPE_SUPS_MASK
    /* SUPS
    */

#define CMIPAPP_CALL_TYPE_VOICE_VT   ADD_CALL_TYPE_MASK(CMIPAPP_CALL_TYPE_VOICE,CMIPAPP_CALL_TYPE_VT)
    /* Both voice and VT
    */

#define CMIPAPP_CALL_TYPE_VOICE_VS   ADD_CALL_TYPE_MASK(CMIPAPP_CALL_TYPE_VOICE,CMIPAPP_CALL_TYPE_VS)
    /* Both voice and VS
    */ 

    /* The CMIPAPP_CALL_TYPE_REG is not associated any with CM_CALL_TYPE_XXX. 
    */
#define CMIPAPP_CALL_TYPE_REG        SYS_BM_64BIT(CM_CALL_TYPE_MAX + 1)
    /* Registration for CMIPAPP Events
    */
    
#define CMIPAPP_CALL_TYPE_MAX        SYS_BM_64BIT(CM_CALL_TYPE_MAX + 2)
    /* FOR INTERNAL USE OF CM ONLY!
    */

/**--------------------------------------------------------------------------
** Enums
** --------------------------------------------------------------------------
*/

/* List of values for call progress indication
** 1xx messages need not be reported to user
** 3xx message support in form of CMIPAPP_PROG_REDIRECT is sufficient
** 4xx messages lead to call getting ended
*/
typedef enum
{
  CMIPAPP_PROG_NONE = -1,
    /* FOR INTERNAL USE OF CM ONLY!
    */

  CMIPAPP_PROG_SESSION,
    /* Early media announcement indication.
    */

  CMIPAPP_PROG_FORWARD,
    /* User or network decided to forward the call to a diff number
    */

  CMIPAPP_PROG_REDIRECT,
    /* Indicates MO call is getting redirected
    */

  CMIPAPP_PROG_MO_RINGBACK,
    /* Indicates Remote user is being alerted
    */

  CMIPAPP_PROG_MO_CALL_WAITING,
    /* Outgoing Call is in waiting state */

  CMIPAPP_PROG_MAX
    /* FOR INTERNAL USE OF CM ONLY!
    */

} cmipapp_prog_e_type;
/*~ SENTINEL cmipapp_prog_e_type.CMIPAPP_PROG_MAX */

/* List of causes for call end from lower layers.
** error_code makes sense when cause is not NORMAL
**
**
** !!!! NEED TO START USING cm_ipapp_cause_e_type from cmutil.h and
** remove this enum declaration. !!!!
*/
typedef enum
{

  CMIPAPP_END_CAUSE_NONE = -1,
    /**< FOR INTERNAL USE OF CM ONLY!
    */
    
  CMIPAPP_END_CAUSE_NORMAL = 0,
    /**< Indicates user requested call end.
    **
    ** Cause also returned when ipapp completes
    ** operation on call hold/retrieve     
    ** successfully.
    */

  CMIPAPP_END_CAUSE_GENERAL_FAILURE = 1,
    /**< Used by CM and app to map end causes
    ** not listed in this enumeration.
    */

  CMIPAPP_END_CAUSE_TEMPORARY_FAILURE = 2,
    /**< Used by CM and app to indicate temporary failures, 
    **
    ** for example, when the AT times out due to no response from the 
    ** network, the MO call fails because RTP flow does not get QoS. 
    ** The AT shall not proceed with the RTP flow configuration for VoIP 
    ** and shall perform a silent redial for an MO call, specifically, the 
    ** silent redial shall be performed over eHRPD first. When all retries 
    ** on the current eHRPD system fail, the call falls back to 1X.
    **
    ** But if it is a emergency call, the call falls back to 1x immediately.
    */

  CMIPAPP_END_CAUSE_FALLBACK_TO_CS = 3,
    /**< Used by CM and app to indicate a permanent VOIP failures and redial
    ** over CS is required.
    **
    ** The AT considers the VoIP service unavailable for permanent RTP QoS 
    ** configuration failures. When a permanent RTP QoS failure occurs for 
    ** an MO call, the MO call fails and the application shall perform a 
    ** silent redial for the MO call. Specifically, if the AT already 
    ** succeeded with IMS registration and 
    ** DomainAvailabilityNotificationEnabled = 1, the AT shall perform the 
    ** Domain Availability Notification procedure and fall back to 1X, 
    ** such that silent redial over 1X is able to perform.
    **
    ** The VoIP application shall not attempt RTP flow configuration again 
    ** until the next time the network is re-acquired (after system loss) or 
    ** the IP address changes. If a reacquisition event occurs and the SIP 
    ** flow QoS was already released, the application shall request for SIP 
    ** flow QoS configuration. After IMS registration succeeds, the next VoIP 
    ** call triggers RTP flow QoS configuration.    
    */

  CMIPAPP_END_CAUSE_PERMANENT_FAILURE = 4,
    /**< Used by CM and app to indicate a permanent VOIP failures and silent
    ** redial shall be ended.
    **
    ** Unsuccessful session establishment with the following SIP response 
    ** codes: 402,403,412,422,429,480,486,487,600,603.    
    */

  CMIPAPP_END_CAUSE_RETRY_AFTER = 5,
    /**< Used by CM and app to treat the call in volte restricted mode as hard
    ** failure. For the call in a multimode mode, treat it as CS RAT
    */

  CMIPAPP_END_CAUSE_ANSWERED_ELSEWHERE = 200,
     /**< call already got answered elsewhere at MT end */

    /**< Call end cause due to redirectionx */
  CMIPAPP_END_CAUSE_MULTIPLE_CHOICES  = 300,
    /**< Request resolved in several choices, each with
    ** its own specific location. User can select from
    ** these to reidrect request
    */

  CMIPAPP_END_CAUSE_MOVED_PERMANENTLY = 301,
    /**< User is no longer at the requested address and
    ** client should retry at new address given
    */


  CMIPAPP_END_CAUSE_MOVED_TEMPORARILY = 302,
    /**< Requesting client should retry request at
    ** new address given. Expires header field gives
    ** the expiration time
    */


  CMIPAPP_END_CAUSE_USE_PROXY = 305,
    /**< Requested resource needs to be accessed
    ** through a proxy given by the contact field
    */


  CMIPAPP_END_CAUSE_ALTERNATE_SERVICE = 380,
    /**< Call was not successful, but alternate services
    ** are possible
    */

  CMIPAPP_END_CAUSE_ALTERNATE_EMERGENCY_CALL = 381,
    /**< 380 response from network. Call needs to be reoriginated as an 
    ** emergency call 
    */

  /**< Call end due to Request Failure 4xx */

  CMIPAPP_END_CAUSE_BAD_REQUEST  = 400,
    /**< Malformed syntax in request
    */

  CMIPAPP_END_CAUSE_UNAUTHORIZED = 401,
    /**< Request requires user
    ** authentication
    */

  CMIPAPP_END_CAUSE_PAYMENT_REQUIRED = 402,
    /**< Future use
    */

  CMIPAPP_END_CAUSE_FORBIDDEN        = 403,
    /**< Server refusing to fulfill request
    ** Request should not be repeated,
    */

  CMIPAPP_END_CAUSE_NOT_FOUND        = 404,
    /**< User does not exist at domain
    ** specified or Domain in requested
    ** URI does not match domains handled by
    ** recipient of the request
    */

  CMIPAPP_END_CAUSE_METHOD_NOT_ALLOWED = 405,
    /**< Method requested in address line
    ** not allowed for address identified
    ** by request-URI
    */

  CMIPAPP_END_CAUSE_NOT_ACCEPTABLE     = 406,
    /**< Resource identified by request can
    ** only generate resp with content
    ** not acceptable
    */

  CMIPAPP_END_CAUSE_PROXY_AUTHENTICATION_REQUIRED = 407,
    /**< Client must first authenticate with proxy
    */

  CMIPAPP_END_CAUSE_REQUEST_TIMEOUT = 408,
    /**< Server could not respond within
    ** suitable amount of time
    */

  CMIPAPP_END_CAUSE_GONE = 410,
    /**< Requested resource no longer available
    ** at server and no forwarding address
    */

  CMIPAPP_END_CAUSE_REQUEST_ENTITY_TOO_LARGE = 413,
    /**< Request entity body is larger than what
    ** server is willing to process
    */

  CMIPAPP_END_CAUSE_REQUEST_URI_TOO_LARGE    = 414,
    /**< Server refusing to service since request-URI
    ** is longer than server willing to interpret.
    */

  CMIPAPP_END_CAUSE_UNSUPPORTED_MEDIA_TYPE   = 415,
    /**< Message body of request not in format
    ** supported by server for requested method
    */

  CMIPAPP_END_CAUSE_UNSUPPORTED_URI_SCHEME   = 416,
    /**< Unsupported URI scheme
    */

  CMIPAPP_END_CAUSE_BAD_EXTENSION            = 420,
    /**< Server did not understand protocol exten
    ** specified in proxy-required or require
    ** header field.
    */

  CMIPAPP_END_CAUSE_EXTENSION_REQUIRED       = 421,
    /**< Extension to process a request is not
    ** listed in supported header field in
    ** request
    */

  CMIPAPP_END_CAUSE_INTERVAL_TOO_BRIEF       = 422,
    /**< Expiration time of resource refreshed by
    ** request is too short
    */

  CMIPAPP_END_CAUSE_TEMPORARILY_UNAVAILABLE  = 480,
    /**< End system contacted but callee is currently
    ** unavailable
    */

  CMIPAPP_END_CAUSE_CALL_OR_TRANS_DOES_NOT_EXIST = 481,
    /**< Request received by UAS does not match
    ** any existing dialog or transaction
    */

  CMIPAPP_END_CAUSE_LOOP_DETECTED              = 482,
    /**< Server detected a loop
    */

  CMIPAPP_END_CAUSE_TOO_MANY_HOPS              = 483,
    /**< Request received has Max-Forwards header
    ** field at 0
    */

  CMIPAPP_END_CAUSE_ADDRESS_INCOMPLETE         = 484,
    /**< Request had incomplete URI
    */

  CMIPAPP_END_CAUSE_AMBIGUOUS                  = 485,
    /**< Request URI was ambiguous
    */

  CMIPAPP_END_CAUSE_BUSY_HERE                  = 486,
    /**< Callee is not willing/unable to take
    ** additional calls at this end system
    */


  CMIPAPP_END_CAUSE_REQUEST_TERMINATED         = 487,
    /**< Request terminated by a BYE/Cancel
    */

  CMIPAPP_END_CAUSE_NOT_ACCEPTABLE_HERE        = 488,
    /**< Resource requested by request-URI not
    ** acceptable
    */

  CMIPAPP_END_CAUSE_REQUEST_PENDING            = 491,
    /**< Request was received by UAS that had
    ** a pending request within same dialog
    */

  CMIPAPP_END_CAUSE_UNDECIPHERABLE             = 493,
    /**< Request has an encrypted MIME body for which
    ** the recipient does not possess appropriate
    ** decryption key
    */


  /**< Call end due to Server Failure 5xx */
  CMIPAPP_END_CAUSE_SERVER_INTERNAL_ERROR      = 500,
    /**< Server hit an unexpected condition that
    ** prevented it from fulfilling the request
    */

  CMIPAPP_END_CAUSE_NOT_IMPLEMENTED            = 501,
    /**< Server does not support functionality to
    ** fulfill request
    */

  CMIPAPP_END_CAUSE_BAD_GATEWAY                = 502,
    /**< Server received invalid response from
    ** downstream gateway
    */

  CMIPAPP_END_CAUSE_SERVER_UNAVAILABLE         = 503,
    /**< Server is temporarily unable to process the
    ** request due to overloading or maintanence
    */

  CMIPAPP_END_CAUSE_SERVER_TIME_OUT            = 504,
    /**< Server did not receive timely response from
    ** external server it accessed
    */

  CMIPAPP_END_CAUSE_VERSION_NOT_SUPPORTED      = 505,
    /**< Server does not support SIP protocol version
    ** used in the request
    */

  CMIPAPP_END_CAUSE_MESSAGE_TOO_LARGE          = 513,
    /**< Server unable to process request since message
    ** length exceeded capabilities
    */

  /**< Call end due to Global Failure 6xx */
  CMIPAPP_END_CAUSE_BUSY_EVERYWHERE            = 600,
    /**< Callee's end system was contacted but callee
    ** is busy and does not wish to take call
    */

  CMIPAPP_END_CAUSE_DECLINE                    = 603,
    /**< Callee's end system was contacted but user
    ** explicitly does not wish to or cannot
    ** participate.
    */

  CMIPAPP_END_CAUSE_DOES_NOT_EXIST_ANYWHERE    = 604,
    /**< Server has information that user indicated in
    ** request-URI does not exist anywhere
    */

  CMIPAPP_END_CAUSE_SESS_DESCR_NOT_ACCEPTABLE  = 605,
    /**< User's agent contacted but some aspects of
    ** session description not acceptable
    */

  CMIPAPP_END_CAUSE_SRVCC_LTE_HO_WCDMA= 606,
    /**< obsolete
    */

  CMIPAPP_END_CAUSE_SRVCC_END_CALL = 607,
    /**< obsolote
    */

  CMIPAPP_END_CAUSE_IP_ORIGINATION_FAIL = 608,
    /**< Orignation fail for vt call 
    */

   CMIPAPP_END_CAUSE_HARD_FAILURE   = 609,
    /** <Call end reason to be send by IMS in place of fallback to CS
               CM will internally MAP 
		1. e911
		alternate redial algo : HardFailureRAN
		legacy :                FallbackToCS
	
		2. Voice calls
        	all calls : Fallback_to_CS
              for alternate silent redial for E911. */

  CMIPAPP_END_CAUSE_ALTERNATE_VOICE_CALL = 610,
    /**< 380 response from network. Call needs to be reoriginated as an 
    ** voice call 
    */

  CMIPAPP_END_CAUSE_LOOP_BACK = 611,
   /* loopback IMS call upon this reason */

  CMIPAPP_END_CAUSE_SRVCC_LTE_TO_CS = 612,
    /**< Call end reason used for VOIP client to send call end indication
    ** to CM when SRVCC handover from LTE to WCDMA/GSM for active call
    */
  
  CMIPAPP_END_CAUSE_LOW_BATTERY = 613,
  /* This end cause is used by RIL to end an ongoing VoLTE Call due to Low Battery*/

  CMIPAPP_END_CAUSE_DEAD_BATTERY = 614,
  /* This end cause is used by RIL to end an ongoing VoLTE Call due to Dead Battery*/

  CMIPAPP_END_CAUSE_HOLD_RESUME_FAILED = 615,
  /**< Server reported hold resume request failed
  **
  */

  CMIPAPP_END_CAUSE_HOLD_RESUME_CANCELED = 616,
  /**< Server reported hold resume request canceled
  **
  */

  CMIPAPP_END_CAUSE_REINVITE_COLLISION = 617,
  /**< Server reported reinvite collision
  **
  */

  CMIPAPP_END_CAUSE_DRVCC_WIFI_TO_CS = 618,
    /**< Call end reason used for VOIP client to send call end indication
    ** to CM when DRVCC handover from WiFi to WCDMA/GSM/1X for active call
    ** is completed succesfully
    */

  CMIPAPP_END_CAUSE_MAX = 0x7FFFFFFF
    /**< FOR INTERNAL USE OF CM ONLY!
    */

} cmipapp_end_cause_e_type;

/** List of modify causes which will be sent to client
*/
typedef enum
{
  CMIPAPP_CLIENT_MODIFY_CAUSE_NONE = CM_CALL_MODIFY_CAUSE_NONE,
  /**< No end cause need to be sent to client  
  */

  CMIPAPP_CLIENT_MODIFY_CAUSE_UPGRADE_DUE_TO_LOCAL_REQ = CM_CALL_MODIFY_CAUSE_UPGRADE_DUE_TO_LOCAL_REQ,
  /**< Call Upgraded on request from the local user
  */

  CMIPAPP_CLIENT_MODIFY_CAUSE_UPGRADE_DUE_TO_REMOTE_REQ = CM_CALL_MODIFY_CAUSE_UPGRADE_DUE_TO_REMOTE_REQ,
  /**< Call upgraded on request from the remote end
  */

  CMIPAPP_CLIENT_MODIFY_CAUSE_DOWNGRADE_DUE_TO_LOCAL_REQ = CM_CALL_MODIFY_CAUSE_DOWNGRADE_DUE_TO_LOCAL_REQ,
  /**< Call downgraded on request from the local user
  */

  CMIPAPP_CLIENT_MODIFY_CAUSE_DOWNGRADE_DUE_TO_REMOTE_REQ = CM_CALL_MODIFY_CAUSE_DOWNGRADE_DUE_TO_REMOTE_REQ,
  /**< Call downgraded on request from the remote end
  */

  CMIPAPP_CLIENT_MODIFY_CAUSE_DOWNGRADE_DUE_TO_RTP_TIMEOUT = CM_CALL_MODIFY_CAUSE_DOWNGRADE_DUE_TO_RTP_TIMEOUT,
  /**< Call downgraded due to RTP/RTCP Timeout for Video stream
  */

  CMIPAPP_CLIENT_MODIFY_CAUSE_DOWNGRADE_DUE_TO_QOS = CM_CALL_MODIFY_CAUSE_DOWNGRADE_DUE_TO_QOS,
  /**< Call downgraded due to QOS for Video stream
  */

  CMIPAPP_CLIENT_MODIFY_CAUSE_DOWNGRADE_DUE_TO_PACKET_LOSS = CM_CALL_MODIFY_CAUSE_DOWNGRADE_DUE_TO_PACKET_LOSS,
  /**< Call downgraded due to PACKET LOSS for Video stream
  */

  CMIPAPP_CLIENT_MODIFY_CAUSE_DOWNGRADE_DUE_TO_LOW_THRPUT = CM_CALL_MODIFY_CAUSE_DOWNGRADE_DUE_TO_LOW_THRPUT,
  /**< Call downgraded due to LOW THRPT for Video stream
  */

  CMIPAPP_CLIENT_MODIFY_CAUSE_DOWNGRADE_DUE_TO_THERM_MITIGATION = CM_CALL_MODIFY_CAUSE_DOWNGRADE_DUE_TO_THERM_MITIGATION,
  /**< Call downgraded due to Thermal Mitigation
  */

  CMIPAPP_CLIENT_MODIFY_CAUSE_DOWNGRADE_DUE_TO_LIPSYNC = CM_CALL_MODIFY_CAUSE_DOWNGRADE_DUE_TO_LIPSYNC,
  /**< Call downgraded due to LIPSYNC
  */

  CMIPAPP_CLIENT_MODIFY_CAUSE_DOWNGRADE_DUE_TO_GENERIC_ERROR = CM_CALL_MODIFY_CAUSE_DOWNGRADE_DUE_TO_GENERIC_ERROR
  /**< Generic error for future purpose
  */

}cmipapp_client_modify_cause_e_type;

/** List of causes which will be sent to client
*/
typedef enum
{
  CMIPAPP_CLIENT_END_CAUSE_NONE = -1,
    /** No end cause need to be sent to client
    */
    
    /*0*/
  CMIPAPP_CLIENT_END_CAUSE_BAD_REQ_WAIT_INVITE = CM_CALL_END_BAD_REQ_WAIT_INVITE      ,
    /**< Received SIP 400 Bad Request, Waiting for INVITE response
    */

  CMIPAPP_CLIENT_END_CAUSE_BAD_REQ_WAIT_REINVITE = CM_CALL_END_BAD_REQ_WAIT_REINVITE      ,
    /**< Received SIP 400 Bad Request, Waiting for re-INVITE response
    */

  CMIPAPP_CLIENT_END_CAUSE_SIP_404_NOT_FOUND = CM_CALL_END_INVALID_REMOTE_URI,
    /**< Received SIP 404 Not Found, Waiting for INVITE response
    */

  CMIPAPP_CLIENT_END_CAUSE_UNSUPP_MEDIA_TYPE = CM_CALL_END_REMOTE_UNSUPP_MEDIA_TYPE,
    /**< Received SIP 415 Unsupported Media Type, Waiting for INVITE response
    */
    
   /*4*/
  CMIPAPP_CLIENT_END_CAUSE_SIP_480_TEMP_UNAVAIL = CM_CALL_END_PEER_NOT_REACHABLE,
    /**< Received SIP 480 Temporarily Unavialable, Waiting for INVITE response
    */

   CMIPAPP_CLIENT_END_CAUSE_NO_NETWORK_RESP_WAIT_INVITE = CM_CALL_END_NETWORK_NO_RESP_TIME_OUT,
    /**< No Network Response, Waiting for INVITE response
    */

   CMIPAPP_CLIENT_END_CAUSE_NO_NETWORK_RESP_WAIT_REINVITE = CM_CALL_END_NETWORK_NO_RESP_HOLD_FAIL,
    /**< No Network Response, Waiting for RE-INVITE response
    */

   CMIPAPP_CLIENT_END_CAUSE_MOVE_TO_EHRPD = CM_CALL_END_DATA_CONNECTION_LOST,
    /**<Moved to eHRPD, Waiting for RE-INVITE response
    */

    /*8*/
   CMIPAPP_CLIENT_END_CAUSE_UPGRADE_DOWNGRADE_REJ = CM_CALL_END_UPGRADE_DOWNGRADE_REJ,
    /**<Upgrade/downgrade rejected (200 OK with current call SDP, 
    ** Waiting for RE-INVITE response
    */

   CMIPAPP_CLIENT_END_CAUSE_SIP_403_FORBIDDEN = CM_CALL_END_SIP_403_FORBIDDEN,
    /**< 403 Call Forbidden. Waiting for INVITE response
    */

   CMIPAPP_CLIENT_END_CAUSE_NO_NETWORK_RESP = CM_CALL_END_NO_NETWORK_RESP,
    /**< generic timeout that didn't received response from the Server or other end
    */

   CMIPAPP_CLIENT_END_CAUSE_UPGRADE_DOWNGRADE_CANCELLED = CM_CALL_END_UPGRADE_DOWNGRADE_CANCELLED,
    /**< reported on MT side if upgrade timer has been cancelled  
         Or cannot complete the request due to some reason after 
         notifying to the user about reinvite request */

   CMIPAPP_CLIENT_END_CAUSE_SIP_486_BUSY_HERE = CM_CALL_END_SIP_486_BUSY_HERE,
    /**< 486 Busy Here: the device shall play busy tone to user then end the call */

   
   CMIPAPP_CLIENT_END_CAUSE_CALL_DEFLECTION = CM_CALL_END_CALL_DEFLECTION,
    /**< Call Ended Due to Call Deflection
    */

   CMIPAPP_CLIENT_END_CAUSE_UPGRADE_DOWNGRADE_FAILED = CM_CALL_END_UPGRADE_DOWNGRADE_FAILED,
    /**< MO side about generic internal software errors and 
         user can try again if the call still exist */

   CMIPAPP_CLIENT_END_CAUSE_INTERNAL_RTP_RTCP_TIMEOUT = CM_CALL_END_CAUSE_INTERNAL_RTP_RTCP_TIMEOUT,
   /*Call terminated due to RTP/RTCP timeout*/

   CMIPAPP_CLIENT_END_CAUSE_INTERNAL_RINGING_RINGBACK_TIMEOUT = CM_CALL_END_CAUSE_INTERNAL_RINGING_RINGBACK_TIMEOUT,
   /*Call terminated due to Ringing/ringback timeout*/

   CMIPAPP_CLIENT_END_CAUSE_INTERNAL_REG_RESTORATION = CM_CALL_END_CAUSE_INTERNAL_REG_RESTORATION,
   /*Call terminated due to reg restoration*/
   CMIPAPP_CLIENT_END_CAUSE_INTERNAL_CODEC_ERROR = CM_CALL_END_CAUSE_INTERNAL_CODEC_ERROR    ,
   /*Call terminated due to Codec error*/
   CMIPAPP_CLIENT_END_CAUSE_INTERNAL_UNSUPPORTED_SDP = CM_CALL_END_CAUSE_INTERNAL_UNSUPPORTED_SDP,
   /*Call terminated due to SDP parsing failure*/
   CMIPAPP_CLIENT_END_CAUSE_INTERNAL_RTP_FAILURE = CM_CALL_END_CAUSE_INTERNAL_RTP_FAILURE,
   /*Call terminated due to RTP Config failure*/
   CMIPAPP_CLIENT_END_CAUSE_INTERNAL_QoS_FAILURE = CM_CALL_END_CAUSE_INTERNAL_QoS_FAILURE,
   /*Call terminated due to QoS failure*/

   CMIPAPP_CLIENT_END_CAUSE_ANSWERED_ELSEWHERE = CM_CALL_END_SIP_200_ANSWERED_ELSEWHERE,
   CMIPAPP_CLIENT_END_CAUSE_MULTIPLE_CHOICES = CM_CALL_END_MULTIPLE_CHOICES,
   CMIPAPP_CLIENT_END_CAUSE_MOVED_PERMANENTLY = CM_CALL_END_MOVED_PERMANENTLY,
   CMIPAPP_CLIENT_END_CAUSE_MOVED_TEMPORARILY = CM_CALL_END_MOVED_TEMPORARILY,
   CMIPAPP_CLIENT_END_CAUSE_USE_PROXY = CM_CALL_END_USE_PROXY,
   CMIPAPP_CLIENT_END_CAUSE_ALTERNATE_SERVICE = CM_CALL_END_USE_PROXY,
   CMIPAPP_CLIENT_END_CAUSE_ALTERNATE_EMERGENCY_CALL = CM_CALL_END_ALTERNATE_EMERGENCY_CALL,
    /**< 380 response from network. Call needs to be reoriginated as emergency call */
   CMIPAPP_CLIENT_END_CAUSE_UNAUTHORIZED = CM_CALL_END_UNAUTHORIZED,
   CMIPAPP_CLIENT_END_CAUSE_PAYMENT_REQUIRED = CM_CALL_END_PAYMENT_REQUIRED,
   CMIPAPP_CLIENT_END_CAUSE_METHOD_NOT_ALLOWED = CM_CALL_END_METHOD_NOT_ALLOWED,
   CMIPAPP_CLIENT_END_CAUSE_NOT_ACCEPTABLE = CM_CALL_END_NOT_ACCEPTABLE,
   CMIPAPP_CLIENT_END_CAUSE_PROXY_AUTHENTICATION_REQUIRED = CM_CALL_END_PROXY_AUTHENTICATION_REQUIRED,
   CMIPAPP_CLIENT_END_CAUSE_GONE = CM_CALL_END_GONE,
   CMIPAPP_CLIENT_END_CAUSE_REQUEST_ENTITY_TOO_LARGE = CM_CALL_END_REQUEST_ENTITY_TOO_LARGE,
   CMIPAPP_CLIENT_END_CAUSE_REQUEST_URI_TOO_LARGE = CM_CALL_END_REQUEST_URI_TOO_LARGE,
   CMIPAPP_CLIENT_END_CAUSE_UNSUPPORTED_URI_SCHEME = CM_CALL_END_UNSUPPORTED_URI_SCHEME,
   CMIPAPP_CLIENT_END_CAUSE_BAD_EXTENSION = CM_CALL_END_BAD_EXTENSION,
   CMIPAPP_CLIENT_END_CAUSE_EXTENSION_REQUIRED = CM_CALL_END_EXTENSION_REQUIRED,
   CMIPAPP_CLIENT_END_CAUSE_INTERVAL_TOO_BRIEF = CM_CALL_END_INTERVAL_TOO_BRIEF,
   CMIPAPP_CLIENT_END_CAUSE_CALL_OR_TRANS_DOES_NOT_EXIST = CM_CALL_END_CALL_OR_TRANS_DOES_NOT_EXIST,
   CMIPAPP_CLIENT_END_CAUSE_LOOP_DETECTED = CM_CALL_END_LOOP_DETECTED,
   CMIPAPP_CLIENT_END_CAUSE_TOO_MANY_HOPS = CM_CALL_END_TOO_MANY_HOPS,
   CMIPAPP_CLIENT_END_CAUSE_ADDRESS_INCOMPLETE = CM_CALL_END_ADDRESS_INCOMPLETE,
   CMIPAPP_CLIENT_END_CAUSE_AMBIGUOUS = CM_CALL_END_AMBIGUOUS,
   CMIPAPP_CLIENT_END_CAUSE_REQUEST_TERMINATED = CM_CALL_END_REQUEST_TERMINATED,
   CMIPAPP_CLIENT_END_CAUSE_NOT_ACCEPTABLE_HERE = CM_CALL_END_NOT_ACCEPTABLE_HERE,
   CMIPAPP_CLIENT_END_CAUSE_REQUEST_PENDING = CM_CALL_END_REQUEST_PENDING,
   CMIPAPP_CLIENT_END_CAUSE_UNDECIPHERABLE= CM_CALL_END_UNDECIPHERABLE,
   CMIPAPP_CLIENT_END_CAUSE_SERVER_INTERNAL_ERROR = CM_CALL_END_CAUSE_SERVER_INTERNAL_ERROR,
   CMIPAPP_CLIENT_END_CAUSE_NOT_IMPLEMENTED = CM_CALL_END_NOT_IMPLEMENTED,
   CMIPAPP_CLIENT_END_CAUSE_BAD_GATEWAY = CM_CALL_END_BAD_GATEWAY,
   CMIPAPP_CLIENT_END_CAUSE_SERVER_UNAVAILABLE = CM_CALL_END_NO_NETWORK_RESP,
   CMIPAPP_CLIENT_END_CAUSE_SERVER_TIME_OUT = CM_CALL_END_SERVER_TIME_OUT,
   CMIPAPP_CLIENT_END_CAUSE_VERSION_NOT_SUPPORTED      = CM_CALL_END_VERSION_NOT_SUPPORTED,
    /**< Server does not support SIP protocol version
    ** used in the request
    */

  CMIPAPP_CLIENT_END_CAUSE_MESSAGE_TOO_LARGE          = CM_CALL_END_MESSAGE_TOO_LARGE,

  CMIPAPP_CLIENT_END_CAUSE_PRECONDITION_FAILURE       = CM_CALL_END_PRECONDITION_FAILURE,

    /**< Server unable to process request since message
    ** length exceeded capabilities
    */

  /**< Call end due to Global Failure 6xx */
  CMIPAPP_CLIENT_END_CAUSE_BUSY_EVERYWHERE            = CM_CALL_END_SIP_600_BUSY_EVERYWHERE,
    /**< Callee's end system was contacted but callee
    ** is busy and does not wish to take call
    */

  CMIPAPP_CLIENT_END_CAUSE_DECLINE                    = CM_CALL_END_IP_USER_REJECT,
    /**< Callee's end system was contacted but user
    ** explicitly does not wish to or cannot
    ** participate.
    */

  CMIPAPP_CLIENT_END_CAUSE_DOES_NOT_EXIST_ANYWHERE    = CM_CALL_END_DOES_NOT_EXIST_ANYWHERE,
    /**< Server has information that user indicated in
    ** request-URI does not exist anywhere
    */

  CMIPAPP_CLIENT_END_CAUSE_SESS_DESCR_NOT_ACCEPTABLE  = CM_CALL_END_SESS_DESCR_NOT_ACCEPTABLE,
    /**< User's agent contacted but some aspects of
    ** session description not acceptable
    */

  CMIPAPP_CLIENT_END_CAUSE_SRVCC_LTE_HO_WCDMA= CM_CALL_END_IP_UNSUPPORTED_USER_TERMINAL,
    /**< Call end reason used for VOIP client to send call end indication
    ** to CM when SRVCC handover from LTE to WCDMA for active call
    */

  CMIPAPP_CLIENT_END_CAUSE_SRVCC_END_CALL = CM_CALL_END_SRVCC_END_CALL,
    /**< Call end reason used for VOIP client to send call end indication
    ** to CM when SRVCC handover from LTE to WCDMA for hold calls or
    ** alerting calls
    */

  CMIPAPP_CLIENT_END_CAUSE_DRVCC_END_CALL = CM_CALL_END_DRVCC_END_CALL,
    /**< Call end reason used for VOIP client to send call end indication
    ** to CM when SRVCC handover from LTE to WCDMA for hold calls or
    ** alerting calls
    */

  CMIPAPP_CLIENT_END_CAUSE_INTERNAL_ERROR = CM_CALL_END_INTERNAL_ERROR,
    /**< Call Ended Due to Internal Error.Catch all for remaining errors, like:
    Req recvd in incorrect state, Notify subscription timer expiry Failure to send a SIP response
    */
  CMIPAPP_CLIENT_END_CAUSE_FALLBACK_TO_CS = CM_CALL_END_FALLBACK_TO_CS,
    /**< This cause is used by IMS to end the VT call with Fallback-to-CS 
    with User Consent. So here, CM would end the call and propagate this cause 
    to Apps. So that UI can trigger the call again with Voice only on CS domain
    */
    CMIPAPP_CLIENT_END_CAUSE_LOW_BATTERY = CM_CALL_END_IP_LOW_BATTERY,
  /* When the VT Call is ended with cause Low Battery, this cause is sent by IMS*/

  CMIPAPP_CLIENT_END_CAUSE_DEAD_BATTERY = CM_CALL_END_IP_DEAD_BATTERY,
  /* When the VT Call is ended with cause Dead Battery, this cause is sent by IMS*/

  CMIPAPP_CLIENT_END_CAUSE_HO_NOT_FEASIBLE = CM_CALL_END_HO_NOT_FEASIBLE,
  /* This cause is used by IMS to end ongoing VT call in case UE moves from
  LTE to 3G NW since SRVCC not possible for VT call and HO is not feasible*/
  
  CMIPAPP_CLIENT_END_CAUSE_PDN_DISCONNECTED = CM_CALL_END_PDN_DISCONNECTED,
  /* IP call ended due to PDN disconnected*/

  CMIPAPP_CLIENT_END_CAUSE_REJECTED_ELSEWHERE= CM_CALL_END_REJECTED_ELSEWHERE,
  /* One of the device(Inter-connected Endpoints) has rejected the call */

  CMIPAPP_CLIENT_END_CAUSE_CALL_PULLED = CM_CALL_END_CALL_PULLED,
   /* Ongoing call has been ended due to call being pulled to primary */

  CMIPAPP_CLIENT_END_CAUSE_CALL_PULL_OUT_OF_SYNC = CM_CALL_END_CALL_PULL_OUT_OF_SYNC,
  /* call pull in primary device is rejected due to latest IMS cache information */

}cmipapp_client_end_cause_e_type;
/*~ SENTINEL cmipapp_client_end_cause_e_type.CMIPAPP_CLIENT_END_CAUSE_MAX */

/** List of reasons given by user for rejecting an incoming call
*/
typedef enum
{

  CMIPAPP_INCOM_REJ_CAUSE_NONE = -1,
    /**< FOR INTERNAL USE OF CM ONLY!
    */

  CMIPAPP_INCOM_REJ_CAUSE_UNAVAILABLE,
    /**< Indicates that there is no reject cause sent 
      from clients or there is no handling at CM.
    */

  CMIPAPP_INCOM_REJ_CAUSE_BUSY,
    /**< Indicates user requested call end
    */

  CMIPAPP_INCOM_REJ_CAUSE_ANOTHER_CALL,
    /**< Indicates user was in another call
    */

  CMIPAPP_INCOM_REJ_CAUSE_ONLY_VOICE,
    /**< Accept voice and reject video
    */

  CMIPAPP_INCOM_REJ_CAUSE_ONLY_VIDEO,
    /**< Accept video and reject voice
    */

  CMIPAPP_INCOM_REJ_CAUSE_1X_COLLISION,
    /**< Reject MT due to existing 1x
    */

  CMIPAPP_INCOM_REJ_CAUSE_LOW_BATTERY,
  /**< Call Reject by user due to low battery
    */

  CMIPAPP_INCOM_REJ_CAUSE_BLACKLISTED_CALL_ID,
  /**< Call Reject by User due to call id in blacklist
	 */

    
  CMIPAPP_INCOM_REJ_CAUSE_USER_REJECT,
    /**< Indicates user requested call end
    */

  CMIPAPP_INCOM_REJ_CAUSE_MAX
    /**< FOR INTERNAL USE OF CM ONLY!
    */

} cmipapp_incom_rej_cause_e_type;
/*~ SENTINEL cmipapp_incom_rej_cause_e_type.CMIPAPP_INCOM_REJ_CAUSE_MAX */


/** Bit Masks of system modes app can support.
** Combination of system modes can be created
** by using the macro ADD_SYSMODE
*/
typedef enum
{

  CMIPAPP_SYS_MODE_NONE        = SYS_SYS_MODE_MASK_NONE,
                                /**< Acquire no system */

  CMIPAPP_SYS_MODE_NO_SRV      = SYS_SYS_MODE_MASK_NO_SRV,
                                /**< No service */

  CMIPAPP_SYS_MODE_CDMA        = SYS_SYS_MODE_MASK_CDMA,
                                /**< CDMA mode systems only */

  CMIPAPP_SYS_MODE_HDR         = SYS_SYS_MODE_MASK_HDR,
                                /**< Acquire HDR mode systems only */

  CMIPAPP_SYS_MODE_GSM         = SYS_SYS_MODE_MASK_GSM,
                                /**< Acquire GSM systems only */

  CMIPAPP_SYS_MODE_WCDMA       = SYS_SYS_MODE_MASK_WCDMA,
                                /**< Acquire WCDMA systems only */

  CMIPAPP_SYS_MODE_WLAN        = SYS_SYS_MODE_MASK_WLAN,
                                /**< Acquire WLAN systems only */
  
  CMIPAPP_SYS_MODE_LTE         = SYS_SYS_MODE_MASK_LTE,
                  /**< Acquire LTE systems only */

  CMIPAPP_SYS_MODE_MAX         = SYS_SYS_MODE_MASK_MAX

} cmipapp_sys_mode_e_type;
/*~ SENTINEL cmipapp_sys_mode_e_type.CMIPAPP_SYS_MODE_MAX */

/** ECMP (Emergency call mode preference) values possible
*/
typedef enum
{

  CMIPAPP_ECMP_NONE        = -1,
  /**<  FOR INTERNAL USE OF CM ONLY! */

  CMIPAPP_ECMP_3GPP,
  /**<  3GPP */
  
  CMIPAPP_ECMP_WLAN,
  /**<  WLAN */

  CMIPAPP_ECMP_MAX

} cmipapp_ecmp_e_type;
/*~ SENTINEL cmipapp_ecmp_e_type.CMIPAPP_ECMP_MAX */


/** From and To technology in hand over operation
*/
typedef enum
{
  CMIPAPP_HO_TYPE_NONE = -1,
    /**< FOR INTERNAL USE OF CM ONLY!
    */

  CMIPAPP_HO_TYPE_DO_1X,
    /**< HDR to CDMA handoff
    */

  CMIPAPP_HO_TYPE_UMTS_1X,
    /**< UMTS to CDMA handoff
    */

  CMIPAPP_HO_TYPE_CS_WLAN,
    /**< CS to WLAN handoff
    */

  CMIPAPP_HO_TYPE_MAX
    /**< FOR INTERNAL USE OF CM ONLY!
    */

} cmipapp_ho_type_e_type;
/*~ SENTINEL cmipapp_ho_type_e_type.CMIPAPP_HO_TYPE_MAX */


/* App abort cause enumeration type
*/
typedef enum
{
  CMIPAPP_ABORT_CAUSE_NONE,
    /* FOR INTERNAL USE OF CM ONLY!
    */

  CMIPAPP_ABORT_CAUSE_CS_FALLBACK,
    /* IPAPP is abort due to cs fallback.
    */

  CMIPAPP_ABORT_CAUSE_MAX
  /* FOR INTERNAL USE OF CM ONLY!
  */
} cmipapp_abort_cause_e_type;
/*~ SENTINEL  cmipapp_abort_cause_e_type.CMIPAPP_ABORT_CAUSE_MAX*/

/* SRVCC notification
*/
typedef enum
{
  CMIPAPP_SRVCC_HO_CANCEL_NOTIFY = 0,
   /**< Indicate SRVCC is cancelled by network
   */

  CMIPAPP_SRVCC_HO_COMPLETE_NOTIFY,
   /**< Indicate SRVCC completed
   */

  CMIPAPP_SRVCC_HO_FAIL_NOTIFY,
   /**< Indicate SRVCC failed
   */

	CMIPAPP_SRVCC_HO_START_NOTIFY,
   /**< Indicate SRVCC Started
   */

  CMIPAPP_SRVCC_CALL_CONTEXT_HO_FAIL_NOTIFY,
   /**< Indicate SRVCC Failed due to
        invalid call context
	    Upon Receiving this, IMS is
		expected to end calls Normally
   */

  CMIPAPP_SRVCC_HO_MAX
  /* FOR INTERNAL USE OF CM ONLY!
  */
  
}cmipapp_srvcc_notification_e_type;
/*~ SENTINEL  cmipapp_srvcc_notification_e_type.CMIPAPP_SRVCC_HO_MAX*/

/* DRVCC notification
*/
typedef enum
{

  CMIPAPP_DRVCC_HO_COMPLETE_NOTIFY = 0,
   /**< Indicate DRVCC completed
   */

  CMIPAPP_DRVCC_HO_FAIL_NOTIFY,
   /**< Indicate DRVCC failed
   */
  CMIPAPP_DRVCC_HO_MAX
  /* FOR INTERNAL USE OF CM ONLY!
  */
  
}cmipapp_drvcc_notification_e_type;
/*~ SENTINEL  cmipapp_drvcc_notification_e_type.CMIPAPP_dRVCC_HO_MAX*/


/* Secure Call capability enum */
typedef enum {

  CMIPAPP_SECURE_CALL_CAPABILITY_NONE = -1,
  /**< FOR INTERNAL USE OF CM ONLY! */

  CMIPAPP_SECURE_CALL_CAPABILITY_NOT_AVAILABLE = CM_PH_SECURE_CALL_CAPABILITY_NOT_AVAILABLE,
  /**< Secure Call capability is NOT available (TF Card not ready).*/

  CMIPAPP_SECURE_CALL_CAPABILITY_AVAILABLE = CM_PH_SECURE_CALL_CAPABILITY_AVAILABLE,
  /**< Secure Call capability is available (TF Card is Ready).*/

  CMIPAPP_SECURE_CALL_CAPABILITY_MAX
  /**< FOR INTERNAL USE OF CM ONLY! */

}cmipapp_secure_call_capability_e_type;  

/* Secure Call context enum */
typedef enum
{
  CMIPAPP_SECURE_CONTEXT_NONE = -1,
  /**< FOR INTERNAL USE OF CM ONLY!
  */

  CMIPAPP_SECURE_CONTEXT_NOT_ESTABLISTED = CM_CALL_SECURE_CONTEXT_NOT_ESTABLISTED,
  /**< Secure Context has NOT been established.
  */

  CMIPAPP_SECURE_CONTEXT_ESTABLISTED = CM_CALL_SECURE_CONTEXT_ESTABLISTED,
  /**< Secure Context HAS been established.
  */

  CMIPAPP_SECURE_CONTEXT_MAX
  /**< FOR INTERNAL USE OF CM ONLY!
  */

} cmipapp_secure_context_e_type;

/* Each app talks to CM with a pre registered id */
typedef uint8 cmipapp_id_type;

/* call type mask used by cmipapp. Maintained for backward compatibility.
** cm_call_type_mask_type got added later.
*/
typedef cm_call_type_mask_type cmipapp_call_type_e_type;

typedef uint32 cmipapp_trans_id_type;
/* Type to define trans id used
** between CM and ipapps
*/

/* Utility to create combination of system modes app can work with */
#define ADD_SYSMODE (X,Y)   (cmipapp_sys_mode_e_type) (((int)X) | ((int)Y))

/**--------------------------------------------------------------------------
** Datatypes
** --------------------------------------------------------------------------
*/

/* This will be passed while calling IMS Callback for ending a Call*/
typedef struct cmipapp_end_params_s
{
  cmipapp_end_cause_e_type   end_cause;
  /* The end cause passed from RIL while ending a Call*/

}cmipapp_end_params_s_type;


/* Message is sent to indicate called party is being alerted
** for mobile originated call.
** Generic message to report provisional 1xx and redirection 3xx
** messages.
*/
typedef struct cmipapp_ind_mo_progr_info_s
{

  cm_call_id_type                id;
    /* call id of MO call
    */

  cmipapp_prog_e_type           progr;
    /* Indicates call progress
    */

  cm_num_s_type                  num;
    /* Gives remote number that is being
    ** alerted
    */
  cm_ip_call_cap_attrib_info_s_type   peer_call_cap_attrib;
    /* Peer Call attrib info for multimedia call */

  boolean                        is_secure_call;
    /* Indicates if call is Secure
    */

  boolean                       is_mo_prog_ind_for_180_ringing;
  /* indicates if the MO_PROGRESS_IND is sent to inform receipt 
     of 180 ringing by IMS */

} cmipapp_ind_mo_progr_info_s_type;

/* Message is sent to indicate set up of an incoming call
** INVITE message consists of only the calling party identity
** in SIP format "sip:4455666@sip.internet.com"
*/
typedef struct cmipapp_ind_mt_invite_s
{

  cm_call_id_type           id;
    /* call id of incoming call
    */

  cmipapp_id_type           app_id;
    /* App that is placing the call
    */

  cm_num_s_type             num;
    /* Phone number of calling party
    */

  cm_num_s_type             child_num;
   /* Local Child Number [SIP/TEL URI]
    */

  cm_num_s_type			    redirecting_number;
   /* Phone number of redirecting party
   */

  cmipapp_call_type_e_type  call_type;
    /* Call type of incoming call
    */

  cmipapp_sys_mode_e_type   sys_mode;
    /* Sytem mode of incoming call
    */

  cm_ip_call_attrib_info_s_type  call_attrib_info;
    /* Audio/Video call attribute info for multimedia call
    */

  cm_ip_caller_info_s_type *caller_info;
	/* Caller ID info, ref counted object
    */

  cm_ip_call_cap_attrib_info_s_type   peer_call_cap_attrib;
    /* Peer Call attrib info for multimedia call
    */
} cmipapp_ind_mt_invite_s_type;

/* UI responds to MT invite with setup response.
** Set up response sends 180 Ringing to remote user.
** Remote user sends an ack for 180 ringing.
**
** UI is informed of this ack with mt_ring_cnf.
*/
typedef struct cmipapp_ind_mt_ring_cnf_s
{

  cm_call_id_type           id;
    /* call id of incoming call
    */

  cmipapp_sys_mode_e_type   sys_mode;
    /* Sytem mode of incoming call
    */
  cmipapp_call_type_e_type  call_type;
    /* Call type of incoming call
    */

  boolean                        is_secure_call;
    /* Indicates call progress
    */

} cmipapp_ind_mt_ring_cnf_s_type;

/* Message is sent for both MO and MT after call gets connected
*/
typedef struct cmipapp_ind_connected_s
{

  cm_call_id_type          id;
    /* call id of connected call
    */

  cm_num_s_type            num;
    /* Remote number call is connected to
    */

  cmipapp_sys_mode_e_type  sys_mode;
    /* Sytem mode of incoming call
    */

  cmipapp_call_type_e_type        call_type;
    /* call type
    */

  cm_ip_call_attrib_info_s_type   call_attrib;
    /* Call attrib info for multimedia call
    */

  cm_ip_call_cap_attrib_info_s_type   local_call_cap_attrib;
    /* Local Call attrib info for multimedia call
    */
  cm_ip_call_cap_attrib_info_s_type   peer_call_cap_attrib;
    /* Peer Call attrib info for multimedia call
    */

} cmipapp_ind_connected_s_type;

/* Message sent to indicate call end
** error_code is valid when end_cause is not NORMAL
*/
typedef struct cmipapp_ind_call_end_s
{

  cm_call_id_type              id;
    /* call id of ended call
    */

  cmipapp_end_cause_e_type    end_cause;
    /* reason for call getting ended. Used to notify
    ** client of call end reason
    */

  cmipapp_client_end_cause_e_type client_end_cause;
    /* end reason will be populated to client
    */

  cm_ipapp_warn_info_s_type   warn_info;
    /* Warning info for ending the call
    */

  char                        reason[CM_IPAPP_TEXT_SIZE];
    /* Null terminated reason string 
    ** RFC 3326
    ** Does not mention any specific reason string lengths
    */

  cm_emerg_srv_categ_type     emerg_srv_categ;
	/*
	** Valid in case IMS returns error code with CMIPAPP_CLIENT_END_CAUSE_ALTERNATE_EMERGENCY_CALL
	** and we have to originate EMERG call over CS
	*/

  cm_ip_call_end_reason_s_type  *end_reason;

} cmipapp_ind_call_end_s_type;

/* Message indicating call being put on hold
*/
typedef struct cmipapp_ind_hold_s
{

  cm_call_id_type              id;
    /* call id of ended call
    */

  cm_ipapp_cause_e_type        cause;
    /* sip cause reflecting result of hold operation
    */

   boolean                     is_call_held_by_remote;
    /* TRUE means call is put to held by remote party
    */

   cm_call_attrib_e_type       held_call_audio_attrib;
    /* This field tells if Audio RX /TX is available for 
     * Held call. Valid for REMOTE_PARTY_HOLD Procedure. Default from IMS is NONE
    */

} cmipapp_ind_hold_s_type;

/* Message indicating call being put on local hold
*/
typedef struct cmipapp_ind_local_hold_s
{
  cm_call_id_type              id;
    /* call id of local hold call
    */

  cm_ipapp_cause_e_type        cause;
    /* sip cause reflecting result of hold operation
    */

} cmipapp_ind_local_hold_s_type;

/* Message indicating answer reject without processing
*/
typedef struct cmipapp_ind_answer_reject_s
{

  cm_call_id_type              id;
    /* call id of INCOM call rejected from IMS without processing
    */

  cm_ipapp_cause_e_type        cause;
    /* answer reject cause */

} cmipapp_ind_answer_reject_s_type;

/* Message indicating call being deflected
*/
typedef struct cmipapp_ind_call_deflection_s
{

  cm_call_id_type              id;
    /* call id
    */

  cm_ipapp_cause_e_type        cause;
    /* sip cause reflecting result of call deflection operation
    */

} cmipapp_ind_call_deflection_s;


/* Message indicating call being retrieved
*/
typedef struct cmipapp_ind_retrieve_s
{

  cm_call_id_type              id;
    /* call id of ended call
    */

  cm_ipapp_cause_e_type        cause;
    /* sip cause reflecting result of retrieve operation
    */

  boolean                     is_call_retrieve_by_remote;
    /* TRUE means call is retrieved by remote party
    */

} cmipapp_ind_retrieve_s_type;

/* Message indicating local held call being retrieved
*/
typedef struct cmipapp_ind_local_retrieve_s
{
  cm_call_id_type              id;
    /* call id of local retrieve call
    */

  cm_ipapp_cause_e_type        cause;
    /* sip cause reflecting result of retrieve operation
    */

} cmipapp_ind_local_retrieve_s_type;

/*------------------------------------------------------------------------*/

/* Message indicating videoshare availability
*/
typedef struct cmipapp_ind_vs_status
{

  cm_call_id_type              id;
    /* call id of voice call on which
    ** videoshare is possible
    */

  boolean                      is_vs_poss;
    /* TRUE   - videoshare is possible on voice call
    ** FALSE  - videoshare is not possible on voice call
    */

} cmipapp_ind_vs_status_s_type;

/*------------------------------------------------------------------------*/

/* Message indicating success/failure on the call related
** sups command. Call dependant supp service works on more
** than one call. Conf is not sent with respect to a particular
** call id.
*/
typedef struct cmipapp_ind_sups_conf
{  

  cm_call_id_type              id;
   /* Added newly to identify the call which added a
	   new participant */ 

  cm_call_sups_type_e_type     sups_type;
    /* Gives the supp service req that 
    ** is generating this conf.
    */

  boolean                      call_success;
    /* TRUE   - Sups request on call id is successfull.
    ** FALSE  - Sups request on call id failed.
    */

  uint8                        number_of_participants_add_removed;
    /* Number of participants added/removed successfully. sups type will be used to
    ** differentiate it is add or remove. 
    ** sups_type = CM_CALL_SUPS_TYPE_MULTIPARTY_CALL: add
    ** sups_type = CM_CALL_SUPS_TYPE_RELEASE_CALL_X_FROM_CONFERENCE: Remove
    */

  cm_usr_uri_list_s_type      *participants;
    /* List of participants added/removed successfully
    **
    */

  cm_ipapp_cause_e_type        failure_cause;
    /* failure cause reflecting result of add participant sups cmd*/

} cmipapp_ind_sups_conf_s_type;

/*------------------------------------------------------------------------*/

/* Message sent to indicate that current call has been transferred
** to another party
*/
typedef struct cmipapp_ind_call_transfer
{

  cm_call_id_type              id;
    /* call that has been transferred to a different party.
    */  

} cmipapp_ind_call_transfer_s_type;

/* Message sent to update remote number that current call 
** is connected to.
*/
typedef struct cmipapp_ind_connected_num
{

  cm_call_id_type              id;
    /* call that needs to update number of remote party.
    */  

  cm_num_s_type                num;
    /* Remote number call is connected to
    */

  boolean                      is_ect_triggered;
   /* Connected Number indication is triggered by ECT procedure
    */
  
} cmipapp_ind_connected_num_s_type;

/* Message sent to indicate the MO SMS status
*/
typedef struct cmipapp_ind_mo_sms_status
{
  cm_sms_mo_status_e_type      status;
    /* The MO SMS status 
    */
} cmipapp_ind_mo_sms_status_s_type;

/*------------------------------------------------------------------------*/

/* Message sent for the contents of Incoming SMS.
*/
typedef cm_mt_sms_msg_type cmipapp_ind_mt_sms_s_type;

/* Indication sent for the waiting VOIP call */
typedef struct cmipapp_ind_wait_voip_call
{
  /* Call id */
  cm_call_id_type                 call_id;

  /* Calling number
  */
  cm_num_s_type                   num;

} cmipapp_ind_wait_voip_call_type;

/* Confirmation sent for call type modify */
typedef struct cmipapp_ind_call_modify_cnf_s
{
   /* Call id */
  cm_call_id_type                 call_id;

  boolean                         modify_error_status;
   /* TRUE - call modify fail
   ** FALSE - call modify success
   */

  cmipapp_call_type_e_type        call_type;
    /* call type
    */

  cm_ip_call_attrib_info_s_type   call_attrib;
    /* Call attrib info for multimedia call
    */

  cmipapp_client_modify_cause_e_type modify_cause_code;  
  /**< Modify cause code, only sent in Success case
  */

  cmipapp_client_end_cause_e_type  modify_error_code;
    /* mofify error code
    */

  uint32                           retry_timer_val;
    /*retry timer indicates in case of upgrade scenario collision or reject request 
    ** so that user can try after such time
    */
  
}cmipapp_ind_call_modify_cnf_s_type;

/* call type modify infication */
typedef struct cmipapp_ind_call_modify_ind_s
{
   /* Call id */
  cm_call_id_type                 call_id;

  cmipapp_call_type_e_type        req_call_type;
    /* Requested call type to modify the call into
    */

  cm_ip_call_attrib_info_s_type   call_attrib;
  
}cmipapp_ind_call_modify_ind_s_type;



typedef struct cmipapp_ind_audio_codec_s
{
   /* Call id */
  cm_call_id_type               call_id;

   /* Speech codec type*/
  sys_speech_codec_e_type       speech_codec_type;

  /* Speech sample rate*/
  uint32                        speech_enc_samp_freq; /* 8000: narrow-band or 16000: wide-band, 0 - unknown: ignore */

}cmipapp_ind_audio_codec_s_type;


typedef struct cmipapp_ind_dtmf_conf_s
{
  /* Call id */
  cm_call_id_type               call_id;

  /* dtmf conf status 
  * TRUE - success
  * FALSE: fail
  */
  boolean                       dtmf_success;
}cmipapp_ind_dtmf_conf_s_type;

/* Indication for MT DTMF for start/stop */
typedef struct cmipapp_ind_dtmf_ind_s
{
  /**< Call id */
  cm_call_id_type     call_id;

  /**< Digit count. */
  uint8               cnt;

  /**< DTMF digit buffer. */
  uint8               digits[CM_INBAND_MAX_DIALED_DIGITS_JCDMA];

  /**< Volume */
  uint16              volume;       

}cmipapp_ind_dtmf_ind_s_type;

/* Indication to upper layer to refresh the list with the
** content present in participant
*/
typedef struct cmipapp_ind_conf_s
{

  cm_call_id_type     id;
    /* call id of call
    */
  cm_conference_info_s_type  *conf;

} cmipapp_ind_conf_s_type;


/* Indicate the upper layer that the paticipant to join in a conf call
*/
typedef struct cmipapp_ind_join_conf
{
  cm_call_id_type     id;
    /* call id of call
    */
  cm_usr_uri_list_s_type  *participant_uri;
  
}cmipapp_ind_join_conf_s_type;

typedef struct cmipapp_ind_call_rat_change_s
{
  cm_call_id_type     id;
  cmipapp_sys_mode_e_type  sys_mode;
}cmipapp_ind_call_rat_change_s_type;

typedef struct cmipapp_ind_ims_audio_rel_cnf_s
{
  boolean success;
}cmipapp_ind_ims_audio_rel_cnf_s_type;

/* Type for CM_IP_APP_SUBS_CNF
*/
typedef struct {
  uint8                          subs_id_bitmask;        /* Based on SYS_MODEM_AS_ID_MASK_* */
}cmipapp_subs_cnf_s_type;

/* Update Indication from IMS to provide Caller Name*/
typedef struct cmipapp_ind_call_update_s
{
  cm_call_id_type     id;
  cm_ip_caller_name_s_type    * caller_name;
}cmipapp_ind_call_update_s_type;

typedef struct cmipapp_call_switch_codec_cnf_s
{
  cm_call_id_type     call_id;
  boolean  status;
  /* whether switch of codec was succesfull or not */
}cmipapp_call_switch_codec_cnf_s_type;

/* Call Forward Indication from IMS to provide History-info from 181*/
typedef struct cmipapp_ind_call_forward_s
{
  cm_call_id_type     id;
  cm_ip_forward_info_s_type    * forward_info;

}cmipapp_ind_call_forward_s_type;

/* Call Barring Indication from IMS */
typedef struct cmipapp_ind_call_barring_s
{
  cm_call_id_type     id;

}cmipapp_ind_call_barring_s_type;


/*------------------------------------------------------------------------*/

/* Common indication data
*/
typedef struct cmipapp_ind_common_s {

  sys_modem_as_id_e_type   asubs_id;
    /**< Indicates the asubs_id */

} cmipapp_ind_common_s_type;

/* Common command data
*/
typedef struct cmipapp_cmd_common_s {

  sys_modem_as_id_e_type   asubs_id;
    /**< Indicates the asubs_id */

} cmipapp_cmd_common_s_type;

/* Structure for lower layer report
*/
typedef struct cmipapp_ind_s {

  cmipapp_ind_common_s_type   hdr;
    /**< Common indication data */

  /* Union of all lower layer report types.
  */
  union {
  
      /* Message is sent to indicate set up of an incoming call
      */
      cmipapp_ind_mt_invite_s_type               mt_invite;
  
      /* Message is sent after remote user acks 180 ringing.
      ** local user can be alerted (ring) at this point.
      */
      cmipapp_ind_mt_ring_cnf_s_type             mt_ring_cnf;
  
      /* Message indicates progress of mobile originated call
      */
      cmipapp_ind_mo_progr_info_s_type           mo_progr_info;
  
      /* Message is sent after call gets connected
      */
      cmipapp_ind_connected_s_type               connected;
  
      /* Message sent to indicate call end
      */
      cmipapp_ind_call_end_s_type                call_end;
  
  	/* Message sent to indicate ANSWER reject from IMS without processing
      */
      cmipapp_ind_answer_reject_s_type           answer_reject;
  
      /* Message sent to indicate call being put on hold
      */
      cmipapp_ind_hold_s_type                    hold;
  
      /* Message sent to indicate call being put on local hold
      */
      cmipapp_ind_local_hold_s_type                    local_hold;
  
      /* Message sent to indicate local held call being made active
      */
      cmipapp_ind_local_retrieve_s_type                local_retrieve;
  
      /* Message sent to indicate call being deflected
      */
      cmipapp_ind_call_deflection_s              call_deflection;
  
      /* Message sent to indicate call being made active
      */
      cmipapp_ind_retrieve_s_type                retrieve;
  
      /* Message sent to indicate videoshare availability for
      ** a voice call
      */
      cmipapp_ind_vs_status_s_type                 vs_status;
  
      /* Message sent to indicate success/failure of sups request       
      */
      cmipapp_ind_sups_conf_s_type                sups_conf ;
  
      /* Message sent to indicate that current call has been transferred
      ** to another party
      */
      cmipapp_ind_call_transfer_s_type            call_transfer;     
      
      /* Message sent to update remote number current call 
      ** is connected to.
      */
      cmipapp_ind_connected_num_s_type            connected_num;     
  
      /* Message sent to indicate the SMS status
      */
      cmipapp_ind_mo_sms_status_s_type            mo_sms_status;
  
      /* Message sent for Incoming SMS status
      */
      cmipapp_ind_mt_sms_s_type                   mt_sms;
  
      /* Call Waiting Report */
      cmipapp_ind_wait_voip_call_type             call_waiting;
  
      /* Call modify confirmation */
      cmipapp_ind_call_modify_cnf_s_type          call_modify_cnf;
  
      /* Call modify indication */
      cmipapp_ind_call_modify_ind_s_type          call_modify_ind;
  
      /* Audio codec infomation sent from IMS */
      cmipapp_ind_audio_codec_s_type              audio_codec;
  
      /* start DTMF status info */
      cmipapp_ind_dtmf_conf_s_type               dtmf_start_conf;
  
      /* stop DTMF cnf status */
      cmipapp_ind_dtmf_conf_s_type               dtmf_stop_conf;
      
      /* Join conf call */
      cmipapp_ind_join_conf_s_type               join_conf_call;
  
      /* conference info */
      cmipapp_ind_conf_s_type                    conf_info;
  
      /* Activate SS confirmation */
      cmipsups_activate_ss_conf_type             activate_ss_conf;
  
      /* Deactivate SS confirmation */
      cmipsups_deactivate_ss_conf_type           deactivate_ss_conf;
  
      /* Erase SS confirmation */
      cmipsups_erase_ss_conf_type                erase_ss_conf;
  
      /* Register SS confirmation */
      cmipsups_register_ss_conf_type             register_ss_conf;

      /* Interrogate SS confirmation */
      cmipsups_interrogate_ss_conf_type          interrogate_ss_conf;

      /* CM_UNSTRUCTURED_SS_IND */
      cmipsups_unstructured_ss_ind_s_type             unstructuresd_ss_ind;

      /* CM_UNSTRUCTURED_SS_NOTIFY_IND  */
      cmipsups_unstructured_ss_notify_ind_s_type      unstructured_ss_notify_ind;
  
      /* CM_RELEASE_UNSTRUCTURED_SS_IND  */
      cmipsups_release_unstructured_ss_ind_s_type     rel_unstructured_ss_ind;

      /* CM_PROCESS_UNSTRUCTURED_SS_CONF  */
      cmipsups_process_unstructured_ss_conf_s_type    process_unstructured_ss_conf;

      /* Call RAT change indication */
      cmipapp_ind_call_rat_change_s_type         call_rat_change;

      /* audio rel cnf during SRVCC */
      cmipapp_ind_ims_audio_rel_cnf_s_type              audio_rel_cnf;

      /* subs change ACKed by IMS APP*/
      cmipapp_subs_cnf_s_type					  subs_chng_cnf;

      /* Caller Name for Call UPDATE Indication */
      cmipapp_ind_call_update_s_type         call_update;

      /* Forward Info coming in SIP response 181 */
      cmipapp_ind_call_forward_s_type        call_forward;

      /*Call Barring Indication send by IMS*/
      cmipapp_ind_call_barring_s_type call_barring;

      /* call codec switch confirmation by IMS APP*/
      cmipapp_call_switch_codec_cnf_s_type        call_switch_codec_cnf;

    /* start DTMF status info for MT DTMF*/
      cmipapp_ind_dtmf_ind_s_type               start_dtmf_ind;

    /* stop DTMF cnf status for MT DTMF */
      cmipapp_ind_dtmf_ind_s_type               stop_dtmf_ind;


  } ind;

} cmipapp_ind_s_type;

/*~ CASE CM_IP_CALL_IND_MT_INVITE      cmipapp_ind_u_type.mt_invite     */
/*~ CASE CM_IP_CALL_IND_MT_RING_CNF    cmipapp_ind_u_type.mt_ring_cnf   */
/*~ CASE CM_IP_CALL_IND_MO_PROGR_INFO  cmipapp_ind_u_type.mo_progr_info */
/*~ CASE CM_IP_CALL_IND_CONNECTED      cmipapp_ind_u_type.connected     */
/*~ CASE CM_IP_CALL_IND_CALL_END       cmipapp_ind_u_type.call_end      */
/*~ CASE CM_IP_CALL_IND_CALL_HOLD      cmipapp_ind_u_type.hold          */
/*~ CASE CM_IP_CALL_IND_CALL_RETRIEVE  cmipapp_ind_u_type.retrieve      */
/*~ CASE CM_IP_CALL_IND_VS_STATUS      cmipapp_ind_u_type.vs_status     */
/*~ CASE CM_IP_CALL_IND_SUPS_CONF      cmipapp_ind_u_type.sups_conf     */
/*~ CASE CM_IP_CALL_IND_TRANSFER       cmipapp_ind_u_type.call_transfer */
/*~ CASE CM_IP_CALL_IND_CONNECTED_NUM  cmipapp_ind_u_type.connected_num */
/*~ CASE CM_IP_SMS_IND_MO_SMS_STATUS   cmipapp_ind_u_type.mo_sms_status */
/*~ CASE CM_IP_SMS_IND_MT_SMS          cmipapp_ind_u_type.mt_sms        */
/*~ CASE CM_IP_CALL_IND_CALL_WAIT      cmipapp_ind_u_type.call_waiting  */

/* Used to inform apps of current service availability in CM
*/
typedef struct {

  sys_srv_status_e_type  evdo_srv_status;
  sys_srv_status_e_type  wlan_srv_status;
  sys_srv_status_e_type  umts_srv_status;
  sys_srv_status_e_type  lte_srv_status;

} cmipapp_srv_status_s_type;


/* Parameters for switch codec command for particular call id
*/
typedef struct {

  cm_call_id_type                call_id;
    /* call id of call for which codec is to be switched
    */

  sys_speech_codec_e_type              switch_to_codec;
    /* codec to which call has to be switched */

} cmipapp_call_switch_codec_s_type;

typedef struct {
  boolean onex_orm_status;
  /* ORM status of 1xcp, true (orm sent)
    False (ORm was not sent), 
    IMS will do sip_reinvite only if ORM was sent by 1XCP*/
} cmipapp_drvcc_ho_fail_notify_s_type;

typedef union cmipapp_drvcc_status_params_u {

  cmipapp_drvcc_ho_fail_notify_s_type drvcc_ho_fail;
    /**< CMIPAPP_DRVCC_HO_FAIL_NOTIFY parameters. */ 

} cmipapp_drvcc_status_params_u_type;


/**--------------------------------------------------------------------------
** Datatypes - Call back Functions
** --------------------------------------------------------------------------
*/

/*  Call back function called by CM to send
**  call origination request.
**  TRUE is returned on successfull operation, FALSE otherwise
*/
typedef boolean   (cmipapp_cmd_orig_f_type) (

  const cm_num_s_type      called_num,
    /* Number dialed - phone that is being called
    */

  const cm_call_id_type    call_id,
    /* call ID of the call
    */

  cmipapp_call_type_e_type  call_type,
    /* call type for the call
    */

  cmipapp_sys_mode_e_type  sys_mode,
    /* Sys mode call needs to be originated on.
    ** Possible use when multiple systems WLAN, EVDO
    ** are available
    */

  uint32                   data_block,
    /* Token created and used by registered apps.
    ** App gives CM this value whenever cmipapp_rpt_ind () is called.
    ** CM passes the most recent value given by app while calling
    ** app registered call back functions
    */

  cm_ip_call_attrib_info_s_type  *call_attrib_info,
    /* Indicate audio/video attributes 
    */

  const cm_npi_type              pi
  /* Presentation indicator 
  */
);
/*~ CALLBACK cmipapp_cmd_orig_f_type
    ONERROR return FALSE */
/*~ PARAM call_attrib_info POINTER */

/*  Call back function called by CM to answer
**  an incoming call.
**  TRUE is returned on successfull operation, FALSE otherwise
*/
typedef boolean (cmipapp_cmd_answer_f_type) (

  const cm_call_id_type            call_id,
    /* call id of incoming call
    */

  boolean                          accept,
    /* Flag indicating whether the call is accepted or not
    */

  cmipapp_incom_rej_cause_e_type   answer_cause,
    /* Valid when accept is set to FALSE
    */

  uint32                           data_block,
    /* Token created and used by registered apps.
    ** App gives CM this value whenever cmipapp_rpt_ind () is called.
    ** CM passes the most recent value given by app while calling
    ** app registered call back functions
    */
  cmipapp_call_type_e_type         answer_type,
    /* Indicate the call type when call is accepted by client
    */

  cm_ip_call_attrib_info_s_type   call_attrib,
    /**< call attrib info for answering multimedia call  */

  const cm_npi_type                    pi,
  /* Presentation indicator 
  */

  cm_ip_call_sip_reject_code_s_type    sip_reject_code
  /* SIP code sent for rejecting incoming call   */

);

/*~ CALLBACK cmipapp_cmd_answer_f_type
    ONERROR return FALSE */

/*  Call back function called by CM to indicate
**  decision of CM client to accept or reject call.
**  TRUE is returned on successfull operation, FALSE otherwise
*/
typedef boolean (cmipapp_cmd_invite_res_f_type) (

  cm_call_id_type                  call_id,
    /* call id of the call
    */

  boolean                          accept,
    /* Flag indicating whether the call is accepted or not
    */

  cmipapp_incom_rej_cause_e_type   answer_cause,
    /* Valid when accept is set to FALSE
    */

  uint32                           data_block
    /* Token created and used by registered apps.
    ** App gives CM this value whenever cmipapp_rpt_ind () is called.
    ** CM passes the most recent value given by app while calling
    ** app registered call back functions
    */
);

/*~ CALLBACK cmipapp_cmd_invite_res_f_type
    ONERROR return FALSE */

/* Call back function called by CM to request
** ending a call
**  TRUE is returned on successfull operation, FALSE otherwise
*/
typedef boolean (cmipapp_cmd_end_f_type) (

  const cm_call_id_type call_id,
    /* call id of the call
    */

  uint32          data_block
    /* Token created and used by registered apps.
    ** App gives CM this value whenever cmipapp_rpt_ind () is called.
    ** CM passes the most recent value given by app while calling
    ** app registered call back functions
    */
);

/*~ CALLBACK cmipapp_cmd_end_f_type
    ONERROR return FALSE */

/* Call back function called by CM to request
** ending a call
**  TRUE is returned on successfull operation, FALSE otherwise
*/
typedef boolean (cmipapp_cmd_end_ext_sub_f_type) (

  const cm_call_id_type call_id,
    /* call id of the call
    */

  uint32          data_block,
    /* Token created and used by registered apps.
    ** App gives CM this value whenever cmipapp_rpt_ind () is called.
    ** CM passes the most recent value given by app while calling
    ** app registered call back functions
    */
  cmipapp_end_params_s_type end_params,
  /*End params sent to IMS while ending a call. 
  Struct is used here to make this CB extensible for future*/

  cmipapp_cmd_common_s_type      cmd_common
    /**< Common command data */
);

/*  Call back function called by CM to start dtmf.
**  TRUE is returned on successfull operation, FALSE otherwise
*/
typedef boolean (cmipapp_inband_cmd_start_cont_dtmf_f_type) (

  const cm_call_id_type call_id,
    /* call id of the call
    */

  uint8           digit,
    /* DTMF digit
    */

  uint32          data_block
    /* Token created and used by registered apps.
    ** App gives CM this value whenever cmipapp_rpt_ind () is called.
    ** CM passes the most recent value given by app while calling
    ** app registered call back functions
    */
);

/*~ CALLBACK cmipapp_inband_cmd_start_cont_dtmf_f_type
    ONERROR return FALSE */

/*  Call back function called by CM to stop dtmf.
**  TRUE is returned on successfull operation, FALSE otherwise
*/
typedef boolean (cmipapp_inband_cmd_stop_cont_dtmf_f_type) (

  const cm_call_id_type call_id,
    /* call id of the call
    */

  uint16          duration,
    /* DTMF duration
    */

  uint32          data_block
    /* Token created and used by registered apps.
    ** App gives CM this value whenever cmipapp_rpt_ind () is called.
    ** CM passes the most recent value given by app while calling
    ** app registered call back functions
    */
);

/*~ CALLBACK cmipapp_inband_cmd_stop_cont_dtmf_f_type
    ONERROR return FALSE */

/* Call back function for CM to put call on hold
**  TRUE is returned on successfull operation, FALSE otherwise
*/
typedef boolean (cmipapp_cmd_hold_f_type) (

  const cm_call_id_type call_id,
    /* call id
    */

  uint32          data_block
    /* Token created and used by registered apps.
    ** App gives CM this value whenever cmipapp_rpt_ind () is called.
    ** CM passes the most recent value given by app while calling
    ** app registered call back functions
    */

);

/*~ CALLBACK cmipapp_cmd_hold_f_type
    ONERROR return FALSE */

/* Call back function for CM to put call on local hold
**  TRUE is returned on successful operation, FALSE otherwise
*/
typedef boolean (cmipapp_cmd_local_hold_sub_f_type) (

  const cm_call_id_type call_id,
    /* call id
    */

  uint32          data_block,
    /* Token created and used by registered apps.
    ** App gives CM this value whenever cmipapp_rpt_ind () is called.
    ** CM passes the most recent value given by app while calling
    ** app registered call back functions
    */

  cmipapp_cmd_common_s_type      cmd_common
    /**< Common command data */
);

/*~ CALLBACK cmipapp_cmd_local_hold_f_type
    ONERROR return FALSE */

/* Call back function for CM to retrieve local held call
**  TRUE is returned on successful operation, FALSE otherwise
*/
typedef boolean (cmipapp_cmd_local_retrieve_sub_f_type) (

  const cm_call_id_type call_id,
    /* call id
    */

  uint32          data_block,
    /* Token created and used by registered apps.
    ** App gives CM this value whenever cmipapp_rpt_ind () is called.
    ** CM passes the most recent value given by app while calling
    ** app registered call back functions
    */

  cmipapp_cmd_common_s_type      cmd_common
    /**< Common command data */
);

/*~ CALLBACK cmipapp_cmd_local_retrieve_f_type
    ONERROR return FALSE */

/* Call back function for CM to put call active
**  TRUE is returned on successfull operation, FALSE otherwise
*/
typedef boolean (cmipapp_cmd_active_f_type) (

  const cm_call_id_type call_id,
    /* call id
    */

  uint32          data_block
    /* Token created and used by registered apps.
    ** App gives CM this value whenever cmipapp_rpt_ind () is called.
    ** CM passes the most recent value given by app while calling
    ** app registered call back functions
    */
);

/*~ CALLBACK cmipapp_cmd_active_f_type
    ONERROR return FALSE */

/* Call back function for CM to modify call type
**  TRUE is returned on successfull operation, FALSE otherwise
*/
typedef boolean (cmipapp_cmd_modify_f_type) (

  const cm_call_id_type           call_id,
    /* call id
    */

  cmipapp_call_type_e_type        cap,
    /* Call type to modify the call into
    */

  uint32                          data_block,
    /* Token created and used by registered apps.
    ** App gives CM this value whenever cmipapp_rpt_ind () is called.
    ** CM passes the most recent value given by app while calling
    ** app registered call back functions
    */
  cm_ip_call_attrib_info_s_type   call_attrib
    /* Audio/Video call attribute
    */
);

/*~ CALLBACK cmipapp_cmd_modify_f_type
    ONERROR return FALSE */

/*  Call back function called by CM to inform client app
**  of modify response.
**  TRUE is returned on successfull operation, FALSE otherwise
*/
typedef boolean (cmipapp_cmd_modify_rsp_sub_f_type) (

  const cm_call_id_type           call_id,
    /* call id
    */

  boolean                         call_modif_rsp,
    /* Call type to modify the call into
    */

  uint32                          data_block,
    /* Token created and used by registered apps.
    ** App gives CM this value whenever cmipapp_rpt_ind () is called.
    ** CM passes the most recent value given by app while calling
    ** app registered call back functions
    */
  cmipapp_call_type_e_type        call_type_rsp,
    /* responded call type
    */
    
  cm_ip_call_attrib_info_s_type   call_attrib,
    /* Audio/Video call attribute
    */

  cmipapp_cmd_common_s_type      cmd_common
    /**< Common command data */
);

/*~ CALLBACK cmipapp_cmd_modify_rsp_f_type
    ONERROR return FALSE */

/*  Call back function called by CM to inform client app
**  of change in serving system parameters.
**  TRUE is returned on successfull operation, FALSE otherwise
*/

typedef boolean (cmipapp_cmd_srv_sys_info_sub_f_type) (


  cmipapp_srv_status_s_type       *srv_status_info,
    /* Serving system information
    */

  cmipapp_cmd_common_s_type      cmd_common
    /**< Common command data */
);


/*~ CALLBACK cmipapp_cmd_srv_sys_info_f_type
    ONERROR return FALSE */
/*~ PARAM srv_status_info POINTER */

/*  Function called into ip app for activating answer hold.
*/
typedef boolean (cmipapp_cmd_act_answer_hold_sub_f_type) (


  const cm_call_id_type           call_id,
    /* call id
    */  

  uint32                          data_block,
    /* Token created and used by registered apps.
    ** App gives CM this value whenever cmipapp_rpt_ind () is called.
    ** CM passes the most recent value given by app while calling
    ** app registered call back functions
    */

  cmipapp_cmd_common_s_type      cmd_common
    /**< Common command data */
);

/*~ CALLBACK cmipapp_cmd_act_answer_hold_f_type
    ONERROR return FALSE */

/*  Function called into ip app for deactivating answer hold.
*/
typedef boolean (cmipapp_cmd_deact_answer_hold_sub_f_type) (


  const cm_call_id_type           call_id,
    /* call id
    */  

  uint32                          data_block,
    /* Token created and used by registered apps.
    ** App gives CM this value whenever cmipapp_rpt_ind () is called.
    ** CM passes the most recent value given by app while calling
    ** app registered call back functions
    */

  cmipapp_cmd_common_s_type      cmd_common
    /**< Common command data */
);
/*~ CALLBACK cmipapp_cmd_deact_answer_hold_f_type
    ONERROR return FALSE */

/*  Function called into ip app for deactivating answer hold.
*/
typedef boolean (cmipapp_cmd_answer_call_waiting_sub_f_type) (


  const cm_call_id_type           call_id,
    /* call id
    */  

  uint32                          data_block,
    /* Token created and used by registered apps.
    ** App gives CM this value whenever cmipapp_rpt_ind () is called.
    ** CM passes the most recent value given by app while calling
    ** app registered call back functions
    */

  cmipapp_cmd_common_s_type      cmd_common
    /**< Common command data */
);
/*~ CALLBACK cmipapp_cmd_answer_call_waiting_f_type
    ONERROR return FALSE */

/*  Call back function called by CM to indicate flash event.
**  TRUE is returned on successfull operation, FALSE otherwise
*/
typedef boolean (cmipapp_cmd_simple_flash_sub_f_type) (

  const cm_call_id_type call_id,
    /* call id of the call
    */

  const cm_mm_call_info_s_type     *call_info_ptr,
    /* call info pointer
    */

  uint32          data_block,
    /* Token created and used by registered apps.
    ** App gives CM this value whenever cmipapp_rpt_ind () is called.
    ** CM passes the most recent value given by app while calling
    ** app registered call back functions
    */

  cmipapp_cmd_common_s_type      cmd_common
    /**< Common command data */
);
/*~ CALLBACK cmipapp_cmd_simple_flash_f_type
    ONERROR return FALSE */
/*~ PARAM call_info_ptr POINTER */

/*  ECT supplementary service enables the served mobile subscriber 
**  (subscriber A) who has two calls , each of which can be an incoming or 
**  outgoing call, to connect the other parties in the two calls and release 
**  the served mobile subscriber's own connection
**
**  Example - call1 is active, call2 is on hold, Sending
**  ect connects remote parties of call1 and call2. Phone initiating
**  ect will finally receive call end on both the calls
**  TRUE is returned on successfull operation, FALSE otherwise.
**
*/

typedef boolean (cmipapp_cmd_ect_sub_f_type) (

  uint32 data_block,
   /* User token passed to CM at registration. CM will
   ** return it back in callback and other notification 
   ** function
   */

  cmipapp_cmd_common_s_type      cmd_common
    /**< Common command data */
);

/*~ CALLBACK cmipapp_cmd_ect_f_type
    ONERROR return FALSE */

/*  Call back function into IP app to put calls in MPTY.
**  Example - call1 is active, call2 is on hold, Sending
**  MPTY puts call1 and call2 in Conference. One call id is retained 
**  and the other is cleared.
**  TRUE is returned on successfull operation, FALSE otherwise.
**
*/

typedef boolean (cmipapp_cmd_mpty_sub_f_type) (

  uint32 data_block,
   /* User token passed to CM at registration. CM will
   ** return it back in callback and other notification 
   ** function
   */

  cmipapp_cmd_common_s_type      cmd_common
    /**< Common command data */
);

/*~ CALLBACK cmipapp_cmd_mpty_f_type
    ONERROR return FALSE */


/*  Call back function called by CM to inform client app
**  about being handed over from technology to another
*/
typedef boolean (cmipapp_cmd_notify_ho_sub_f_type) (

 cmipapp_ho_type_e_type  ho_type,
    /* Indicates from and to handover technology
    */

  cm_call_id_type         call_id,
    /* Call for which handover is being
    ** notified
    */

  cm_call_id_type         new_call_id,
    /* New call id assigned to the call 
    ** after handoff
    */
  
  uint32                  data_block,
    /* Token created and used by registered apps.
    ** App gives CM this value whenever cmipapp_rpt_ind () is called.
    ** CM passes the most recent value given by app while calling
    ** app registered call back functions
    */

  cmipapp_cmd_common_s_type      cmd_common
    /**< Common command data */
);

/* Function called into ipapp 
** to report current operating 
** mode
*/
typedef boolean (cmipapp_inform_oprt_mode_f_type) (

  sys_oprt_mode_e_type    oprt_mode,
    /* Current operating mode
    */  

  cmipapp_trans_id_type   trans_id,
    /* Transaction id used between
    ** CM and App
    */
    
  boolean                 wait_for_cleanup,
    /*Indicates whether CM will wait for response from
    ** IMS for further processing
    */
    
  uint32                  data_block
    /* User token passed to CM at registration. CM will
    ** return it back in callback and other notification 
    ** function
    */
);
/*~ CALLBACK cmipapp_inform_oprt_mode_f_type
    ONERROR return FALSE */


/* Function called into ipapp 
** to report service domain pref 
*/
typedef boolean (cmipapp_inform_srv_domain_sub_f_type) (

  cm_srv_domain_pref_e_type    srv_domain,
    /* Current service domain
    */  

  cmipapp_trans_id_type   trans_id,
    /* Transaction id used between
    ** CM and App
    */
    
  uint32                  data_block,
    /* User token passed to CM at registration. CM will
    ** return it back in callback and other notification 
    ** function
    */

  cmipapp_cmd_common_s_type      cmd_common
    /**< Common command data */
);
/*~ CALLBACK cmipapp_inform_srv_domain_f_type
    ONERROR return FALSE */


/*  Call back function into IP app to send MO SMS over SIP
**  TRUE is returned on successfull operation, FALSE otherwise.
*/
typedef boolean (cmipapp_sms_cmd_mo_sms_sub_f_type) (

  cm_sms_msg_type                 *data,
   /* The pointer to MO SMS message.
   */

  sys_sys_mode_e_type             sys_mode,
  /* The sys mode over which the mo SMS is sent.
  */

  cmipapp_cmd_common_s_type      cmd_common
    /**< Common command data */
);

/*~ CALLBACK cmipapp_sms_cmd_mo_sms_f_type
    ONERROR return FALSE */
/*~ PARAM data POINTER */

/* Call back function called by CM to request
** abort a call
**  TRUE is returned on successfull operation, FALSE otherwise
** The difference between the abort and end is that
** IPAPP will not send the call end indication in the abort case.
*/
typedef boolean (cmipapp_cmd_abort_sub_f_type) (

  const cm_call_id_type                call_id,
    /* call id of the call
    */

  const cmipapp_abort_cause_e_type     abort_cause,
    /* The abort cause when the IPCALL is aborted.
    */

  uint32                               data_block,
    /* Token created and used by registered apps.
    ** App gives CM this value whenever cmipapp_rpt_ind () is called.
    ** CM passes the most recent value given by app while calling
    ** app registered call back functions
    */

  cmipapp_cmd_common_s_type      cmd_common
    /**< Common command data */
);


/*~ CALLBACK cmipapp_cmd_abort_f_type
    ONERROR return FALSE */


/* Function called into ipapp 
** to report srvcc status 
*/
typedef boolean (cmipapp_send_srvcc_sub_f_type) (

  cmipapp_srvcc_notification_e_type srvcc_status,
    /* SRVCC status
    */  
    
  uint32                  data_block,
    /* User token passed to CM at registration. CM will
    ** return it back in callback and other notification 
    ** function
    */

  cmipapp_cmd_common_s_type      cmd_common
    /**< Common command data */
);
/*~ CALLBACK cmipapp_send_srvcc_f_type
    ONERROR return FALSE */

/* Function called into ipapp 
** to remove last party from conference 
*/
typedef boolean (cmipapp_send_rm_last_party_sub_f_type) (
   cm_num_s_type           call_num,
    /* Call number need to be removed from conference
    */
   
   uint32                  data_block,
    /* User token passed to CM at registration. CM will
    ** return it back in callback and other notification 
    ** function
    */

  cmipapp_cmd_common_s_type      cmd_common
    /**< Common command data */
);
/*~ CALLBACK cmipapp_send_rm_last_party_f_type
    ONERROR return FALSE */

/* Function called into ipapp 
**  to add the party 
*/
typedef boolean (cmipapp_send_add_new_party_sub_f_type) (
	cm_call_id_type           call_id,
    /* Call which needs to add the party 
    */

	cm_num_s_type           party_num,
    /* Call number need to be added from conference
    */
   
   uint32                  data_block,
    /* User token passed to CM at registration. CM will
    ** return it back in callback and other notification 
    ** function
    */

  cmipapp_cmd_common_s_type      cmd_common
    /**< Common command data */
);
/*~ CALLBACK cmipapp_send_add_new_party_f_type
    ONERROR return FALSE */

/* Function called into ipapp 
** to send ps supplementary service command 
*/
typedef boolean (cmipapp_send_sups_cmd_sub_f_type) (

   cmipsups_cmd_s_type    *sups_cmd,
    /* supplementary service cmd to enable/disable supplement service
    */
   uint32                  data_block,
    /* User token passed to CM at registration. CM will
    ** return it back in callback and other notification 
    ** function
    */

  cmipapp_cmd_common_s_type      cmd_common
    /**< Common command data */
);
/*~ CALLBACK cmipapp_send_sups_cmd_f_type
    ONERROR return FALSE */

/* Function called into ipapp 
** to send subscription unavailable  command 
*/
typedef boolean (cmipapp_send_subsc_chngd_f_type) (

  cm_ph_cmd_e_type        subsc_cmd,
    /* subcription unavailable cmd to deregister ims
    */

	cmipapp_trans_id_type    trans_id,
    /* trans id */

  uint32                  data_block,
    /* User token passed to CM at registration. CM will
    ** return it back in callback and other notification 
    ** function
    */

  uint8                   subs_id_bitmask,
    /* Bitmask of the subs_id impacted. Based on SYS_MODEM_AS_ID_MASK_*
    */

  uint8                   reason_mask[]
  /* Each index in reason mask corresponds to a sub
     and will be set based on cm_sub_clnup_reason_e_type ,
	 length based on MAX_AS_IDS */

);
/*~ CALLBACK cmipapp_send_subsc_chngd_f_type
    ONERROR return FALSE */

/* Function called into ipapp 
** to send call deflection command
*/
typedef boolean (cmipapp_call_deflection_sub_f_type) (

   const cm_call_id_type                call_id,
    /* call id of call that has tko be deflected
    */

   cm_num_s_type                       deflect_dest_num,
    /* desination number where call is to be deflected */

    uint32                  data_block,
    /* User token passed to CM at registration. CM will
    ** return it back in callback and other notification 
    ** function
    */

  cmipapp_cmd_common_s_type      cmd_common
    /**< Common command data */
);
/*~ CALLBACK cmipapp_call_deflection_f_type
    ONERROR return FALSE */


/* Function called into ipapp 
** to send call codec switch command
*/
typedef boolean (cmipapp_call_switch_codec_sub_f_type) (

   cmipapp_call_switch_codec_s_type switch_codec_params,
     /* parameters for codec switch command any future params addition
             is to extend this interface */

    uint32                  data_block,
    /* User token passed to CM at registration. CM will
    ** return it back in callback and other notification 
    ** function
    */

  cmipapp_cmd_common_s_type      cmd_common
    /**< Common command data */
);
/*~ CALLBACK cmipapp_call_switch_codec_f_type
    ONERROR return FALSE */

/*  Call back function called by CM to send
**  call origination request.
**  TRUE is returned on successfull operation, FALSE otherwise
*/
typedef boolean   (cmipapp_cmd_orig_sub_f_type) (

  const cm_num_s_type      called_num,
    /* Number dialed - phone that is being called
    */

  const cm_call_id_type    call_id,
    /* call ID of the call
    */

  cmipapp_call_type_e_type  call_type,
    /* call type for the call
    */

  cmipapp_sys_mode_e_type  sys_mode,
    /* Sys mode call needs to be originated on.
    ** Possible use when multiple systems WLAN, EVDO
    ** are available
    */

  uint32                   data_block,
    /* Token created and used by registered apps.
    ** App gives CM this value whenever cmipapp_rpt_ind () is called.
    ** CM passes the most recent value given by app while calling
    ** app registered call back functions
    */

  cm_ip_call_attrib_info_s_type  *call_attrib_info,
    /* Indicate audio/video attributes 
    */

  const cm_npi_type              pi,
  /* Presentation indicator 
  */

  cmipapp_cmd_common_s_type      cmd_common
    /**< Common command data */
);

/*  Call back function called by CM to answer
**  an incoming call.
**  TRUE is returned on successfull operation, FALSE otherwise
*/
typedef boolean (cmipapp_cmd_answer_sub_f_type) (

  const cm_call_id_type            call_id,
    /* call id of incoming call
    */

  boolean                          accept,
    /* Flag indicating whether the call is accepted or not
    */

  cmipapp_incom_rej_cause_e_type   answer_cause,
    /* Valid when accept is set to FALSE
    */

  uint32                           data_block,
    /* Token created and used by registered apps.
    ** App gives CM this value whenever cmipapp_rpt_ind () is called.
    ** CM passes the most recent value given by app while calling
    ** app registered call back functions
    */
  cmipapp_call_type_e_type         answer_type,
    /* Indicate the call type when call is accepted by client
    */

  cm_ip_call_attrib_info_s_type   call_attrib,
    /**< call attrib info for answering multimedia call  */

  const cm_npi_type                    pi,
  /* Presentation indicator 
  */

  cm_ip_call_sip_reject_code_s_type    sip_reject_code,
  /* SIP code sent for rejecting incoming call   */

  cmipapp_cmd_common_s_type      cmd_common
    /**< Common command data */
);

/*  Call back function called by CM to indicate
**  decision of CM client to accept or reject call.
**  TRUE is returned on successfull operation, FALSE otherwise
*/
typedef boolean (cmipapp_cmd_invite_res_sub_f_type) (

  cm_call_id_type                  call_id,
    /* call id of the call
    */

  boolean                          accept,
    /* Flag indicating whether the call is accepted or not
    */

  cmipapp_incom_rej_cause_e_type   answer_cause,
    /* Valid when accept is set to FALSE
    */

  uint32                           data_block,
    /* Token created and used by registered apps.
    ** App gives CM this value whenever cmipapp_rpt_ind () is called.
    ** CM passes the most recent value given by app while calling
    ** app registered call back functions
    */

  cmipapp_cmd_common_s_type      cmd_common
    /**< Common command data */
);

/* Call back function called by CM to request
** ending a call
**  TRUE is returned on successfull operation, FALSE otherwise
*/
typedef boolean (cmipapp_cmd_end_sub_f_type) (

  const cm_call_id_type call_id,
    /* call id of the call
    */

  uint32          data_block,
    /* Token created and used by registered apps.
    ** App gives CM this value whenever cmipapp_rpt_ind () is called.
    ** CM passes the most recent value given by app while calling
    ** app registered call back functions
    */

  cmipapp_cmd_common_s_type      cmd_common
    /**< Common command data */
);

/*  Call back function called by CM to start dtmf.
**  TRUE is returned on successfull operation, FALSE otherwise
*/
typedef boolean (cmipapp_inband_cmd_start_cont_dtmf_sub_f_type) (

  const cm_call_id_type call_id,
    /* call id of the call
    */

  uint8           digit,
    /* DTMF digit
    */

  uint32          data_block,
    /* Token created and used by registered apps.
    ** App gives CM this value whenever cmipapp_rpt_ind () is called.
    ** CM passes the most recent value given by app while calling
    ** app registered call back functions
    */

  cmipapp_cmd_common_s_type      cmd_common
    /**< Common command data */
);

/*  Call back function called by CM to stop dtmf.
**  TRUE is returned on successfull operation, FALSE otherwise
*/
typedef boolean (cmipapp_inband_cmd_stop_cont_dtmf_sub_f_type) (

  const cm_call_id_type call_id,
    /* call id of the call
    */

  uint16          duration,
    /* DTMF duration
    */

  uint32          data_block,
    /* Token created and used by registered apps.
    ** App gives CM this value whenever cmipapp_rpt_ind () is called.
    ** CM passes the most recent value given by app while calling
    ** app registered call back functions
    */

  cmipapp_cmd_common_s_type      cmd_common
    /**< Common command data */
);

/* Call back function for CM to put call on hold
**  TRUE is returned on successfull operation, FALSE otherwise
*/
typedef boolean (cmipapp_cmd_hold_sub_f_type) (

  const cm_call_id_type call_id,
    /* call id
    */

  uint32          data_block,
    /* Token created and used by registered apps.
    ** App gives CM this value whenever cmipapp_rpt_ind () is called.
    ** CM passes the most recent value given by app while calling
    ** app registered call back functions
    */

  cmipapp_cmd_common_s_type      cmd_common
    /**< Common command data */
);

/* Call back function for CM to put call active
**  TRUE is returned on successfull operation, FALSE otherwise
*/
typedef boolean (cmipapp_cmd_active_sub_f_type) (

  const cm_call_id_type call_id,
    /* call id
    */

  uint32          data_block,
    /* Token created and used by registered apps.
    ** App gives CM this value whenever cmipapp_rpt_ind () is called.
    ** CM passes the most recent value given by app while calling
    ** app registered call back functions
    */

  cmipapp_cmd_common_s_type      cmd_common
    /**< Common command data */
);

/* Call back function for CM to modify call type
**  TRUE is returned on successfull operation, FALSE otherwise
*/
typedef boolean (cmipapp_cmd_modify_sub_f_type) (

  const cm_call_id_type           call_id,
    /* call id
    */

  cmipapp_call_type_e_type        cap,
    /* Call type to modify the call into
    */

  uint32                          data_block,
    /* Token created and used by registered apps.
    ** App gives CM this value whenever cmipapp_rpt_ind () is called.
    ** CM passes the most recent value given by app while calling
    ** app registered call back functions
    */
  cm_ip_call_attrib_info_s_type   call_attrib,
    /* Audio/Video call attribute
    */

  cmipapp_cmd_common_s_type      cmd_common
    /**< Common command data */
);



/* IP ECT Type - Blind, Assured or Consultative
*/
typedef enum
{
  CMIPAPP_EXPLICIT_CALL_TRANSFER_TYPE_NONE = 0,  
  /**< FOR INTERNAL USE OF CM ONLY!
  */

  CMIPAPP_EXPLICIT_CALL_TRANSFER_TYPE_BLIND,
  /**< Blind ECT
  */

  CMIPAPP_EXPLICIT_CALL_TRANSFER_TYPE_ASSURED,
  /**< Assured ECT 
  */

  CMIPAPP_EXPLICIT_CALL_TRANSFER_TYPE_CONSULTATIVE,
  /**< Consultative ECT 
  */

  CMIPAPP_EXPLICIT_CALL_TRANSFER_TYPE_MAX
  /* FOR INTERNAL USE OF CM ONLY!
  */

}cmipapp_ip_expicit_call_transfer_e_type;
/*~ SENTINEL  cmipapp_ip_expicit_call_transfer_e_type .CMIPAPP_EXPLICIT_CALL_TRANSFER_TYPE_MAX*/


/* Call back function for CM to initiate explicit call transfer
**  TRUE is returned on successful operation, FALSE otherwise
*/
typedef boolean (cmipapp_cmd_ip_explicit_call_transfer_f_type) (

  const cm_call_id_type transferee_call_id,
  /* call id of call between Transferor (UE B) and Transferee (UE A)
  */

  cmipapp_ip_expicit_call_transfer_e_type ect_type,
  /* ECT Type
  */

  cm_num_s_type      transfer_target_call_num,
  /* Calling Number of Transfer target (UE C)
  */

  cm_call_id_type    transfer_target_call_id,
  /* Call ID of Transfer Target (UE C)
  */

  uint32          data_block,
  /* Token created and used by registered apps.
  ** App gives CM this value whenever cmipapp_rpt_ind () is called.
  ** CM passes the most recent value given by app while calling
  ** app registered call back functions
  */

  cmipapp_cmd_common_s_type      cmd_common
  /**< Common command data */
  );



/* Function called into ipapp 
** to report drvcc status 
*/
typedef boolean (cmipapp_send_drvcc_sub_f_type) (

  cmipapp_drvcc_notification_e_type drvcc_status,
    /* DRVCC status
    */ 

  cmipapp_drvcc_status_params_u_type drvcc_status_params,
    
  uint32                  data_block,
    /* User token passed to CM at registration. CM will
    ** return it back in callback and other notification 
    ** function
    */
  cmipapp_cmd_common_s_type      cmd_common
    /**< Common command data */
);
/*~ CALLBACK cmipapp_send_drvcc_sub_f_type
    ONERROR return FALSE */



/* Call back function called by CM to indicate
**  secure call capability information (TF Card Status).
**  TRUE is returned on successfull operation, FALSE otherwise
*/
typedef boolean (cmipapp_cmd_inform_secure_call_capability_f_type) (

  cmipapp_secure_call_capability_e_type  secure_call_capability,
    /**< Secure Call capability enum  */

  uint32          data_block,
    /* Token created and used by registered apps.
    ** App gives CM this value whenever cmipapp_rpt_ind () is called.
    ** CM passes the most recent value given by app while calling
    ** app registered call back functions
    */

  cmipapp_cmd_common_s_type      cmd_common
    /**< Common command data */
);

/* Call back function called by CM to indicate
**  secure context information for a call
**  TRUE is returned on successfull operation, FALSE otherwise
*/
typedef boolean (cmipapp_cmd_inform_secure_context_f_type) (

  const cm_call_id_type call_id,
    /**< call id of the call  */

  cmipapp_secure_context_e_type  secure_context_info,
    /**< Secure Context information enum */

  uint32          data_block,
    /* Token created and used by registered apps.
    ** App gives CM this value whenever cmipapp_rpt_ind () is called.
    ** CM passes the most recent value given by app while calling
    ** app registered call back functions */

  cmipapp_cmd_common_s_type      cmd_common
    /**< Common command data */
);

/*-------------------------------------------------------------------------*/

/* Structure defining function pointers that
** are registered by Apps. CM calls these
** functions for call related services
*/
typedef struct {

  cmipapp_cmd_orig_f_type      *cmipapp_cmd_orig_func_ptr;
    /* callback to send call origination request
    */

  cmipapp_cmd_answer_f_type    *cmipapp_cmd_answer_func_ptr;
    /* callback for user to answer incoming call
    */

  cmipapp_cmd_invite_res_f_type       *cmipapp_cmd_invite_res_func_ptr;
    /* callback for client to answer incoming call
    */

  cmipapp_cmd_end_f_type       *cmipapp_cmd_end_func_ptr;
    /* callback to request ending a call
    */

  cmipapp_inband_cmd_start_cont_dtmf_f_type
    /* callback to start DTMF
    */
                               *cmipapp_inband_cmd_start_cont_dtmf_func_ptr;

  cmipapp_inband_cmd_stop_cont_dtmf_f_type
    /* callback to stop DTMF
    */
                               *cmipapp_inband_cmd_stop_cont_dtmf_func_ptr;

  cmipapp_cmd_hold_f_type      *cmipapp_cmd_hold_func_ptr;
    /* callback to put call on hold
    */

  cmipapp_cmd_local_hold_sub_f_type      *cmipapp_cmd_local_hold_sub_func_ptr;
    /* callback to put call on local hold
    */

  cmipapp_cmd_local_retrieve_sub_f_type  *cmipapp_cmd_local_retrieve_sub_func_ptr;
    /* callback to retrieve local held call
    */

  cmipapp_cmd_active_f_type    *cmipapp_cmd_active_func_ptr;
    /* callback to make call active
    */

  cmipapp_cmd_modify_f_type    *cmipapp_cmd_modify_func_ptr;
    /* callback for CM to modify call type
    */

  cmipapp_cmd_modify_rsp_sub_f_type  *cmipapp_cmd_modify_rsp_sub_func_ptr;
    /* callback for CM to send modify response
    */

  cmipapp_cmd_srv_sys_info_sub_f_type *cmipapp_cmd_serv_sys_info_sub_ptr;
    /* callback to inform change in serving system parameters.
    */  

  cmipapp_cmd_act_answer_hold_sub_f_type 
                               *cmipapp_cmd_act_answer_hold_sub_func_ptr;
    /*  Function called into ip app for activating answer hold.
    */

  cmipapp_cmd_deact_answer_hold_sub_f_type 
                               *cmipapp_cmd_deact_answer_hold_sub_func_ptr;
    /*  Function called into ip app for de-activating answer hold.
    */

  cmipapp_cmd_answer_call_waiting_sub_f_type
                                *cmipapp_cmd_answer_call_waiting_sub_func_ptr; 
    /*  Function called into ip app for answer the call waiting;
    */

  cmipapp_cmd_simple_flash_sub_f_type
                                *cmipapp_cmd_simple_flash_sub_func_ptr; 
    /*  Function called into ip app for answer the call waiting or
     *  to remove last party added to the conference call
     */

  cmipapp_cmd_ect_sub_f_type       *cmipapp_cmd_ect_sub_func_ptr;
    /* Call back function called by CM of mobile initiating
    ** ECT
    */

  cmipapp_cmd_mpty_sub_f_type      *cmipapp_cmd_mpty_sub_func_ptr;
    /* Callback to put all the current calls in multiparty. */      

  cmipapp_cmd_notify_ho_sub_f_type *cmipapp_cmd_notify_ho_sub_func_ptr;
    /* Callback to inform ipapp of a handover from one technology
    ** to another
    */

  cmipapp_inform_oprt_mode_f_type *cmipapp_inform_oprt_mode;
    /* Callback to inform ipapp of operating mode */      

  cmipapp_sms_cmd_mo_sms_sub_f_type 
                               *cmipapp_sms_cmd_mo_sms_sub_func_ptr;
    /* Call back to send MO SMS over SIP */

  cmipapp_cmd_abort_sub_f_type       *cmipapp_cmd_abort_sub_func_ptr;
    /* callback to abort the voice call over IP.
    */

  cmipapp_inform_srv_domain_sub_f_type *cmipapp_inform_srv_domain_sub;
    /* Callback to inform ipapp of service domain preference (eg:ps detach) */ 

  cmipapp_send_srvcc_sub_f_type    *cmipapp_srvcc_status_sub_func_ptr;
    /* Callback to inform ipapp of SRVCC status */ 

  cmipapp_send_rm_last_party_sub_f_type   *cmipcall_rm_last_party_from_conf_sub;
    /* callback to remove last party from conference call */

  cmipapp_send_sups_cmd_sub_f_type        *cmipapp_sups_cmd_sub;
    /* callback to send sups command */

  cmipapp_send_sups_cmd_sub_f_type        *cmipapp_ussd_cmd_sub;
    /* callback to send ussd command */

	cmipapp_send_subsc_chngd_f_type        *cmipapp_subsc_chgnd;
    /* callback to send subcription changed command */

  cmipapp_send_add_new_party_sub_f_type   *cmipcall_add_party_to_conf_sub;
    /* callback to send new party addition command */

  cmipapp_call_deflection_sub_f_type   *cmipcall_call_deflection_cmd_sub_func_ptr;
    /* callback to send new party addition command */

  cmipapp_call_switch_codec_sub_f_type   *cmipcall_call_switch_codec_cmd_sub_func_ptr;
    /* callback to send switch codec command for call */

  cmipapp_cmd_orig_sub_f_type      *cmipapp_cmd_orig_sub_func_ptr;
    /* callback to send call origination request
    */

  cmipapp_cmd_answer_sub_f_type    *cmipapp_cmd_answer_sub_func_ptr;
    /* callback for user to answer incoming call
    */

  cmipapp_cmd_invite_res_sub_f_type       *cmipapp_cmd_invite_res_sub_func_ptr;
    /* callback for client to answer incoming call
    */

  cmipapp_cmd_end_sub_f_type       *cmipapp_cmd_end_sub_func_ptr;
    /* callback to request ending a call
    */

  cmipapp_cmd_end_ext_sub_f_type    *cmipapp_cmd_end_ext_sub_func_ptr;
      /*callback to request ending a call with params*/

  cmipapp_inband_cmd_start_cont_dtmf_sub_f_type
    /* callback to start DTMF
    */
                               *cmipapp_inband_cmd_start_cont_dtmf_sub_func_ptr;

  cmipapp_inband_cmd_stop_cont_dtmf_sub_f_type
    /* callback to stop DTMF
    */
                               *cmipapp_inband_cmd_stop_cont_dtmf_sub_func_ptr;

  cmipapp_cmd_hold_sub_f_type      *cmipapp_cmd_hold_sub_func_ptr;
    /* callback to put call on hold
    */

  cmipapp_cmd_active_sub_f_type    *cmipapp_cmd_active_sub_func_ptr;
    /* callback to make call active
    */

  cmipapp_cmd_modify_sub_f_type    *cmipapp_cmd_modify_sub_func_ptr;
    /* callback for CM to modify call type
    */
  cmipapp_send_drvcc_sub_f_type    *cmipapp_drvcc_status_sub_func_ptr;
    /* Callback to inform ipapp of DRVCC status */ 

  cmipapp_cmd_ip_explicit_call_transfer_f_type    *cmipapp_cmd_ip_ect_sub_func_ptr;
    /* callback for CM to send IP ECT request
    */
  
  cmipapp_cmd_inform_secure_call_capability_f_type  *cmipapp_cmd_inform_secure_call_capability_func_ptr;
    /* Callback to inform ipapp about Secure Call Capability 
    */ 

  cmipapp_cmd_inform_secure_context_f_type          *cmipapp_cmd_inform_secure_context_func_ptr;
    /* Callback to inform ipapp about Secure call context per call_id 
    */ 

} cmipapp_cb_s_type;

/*------------------------------------------------------------------------------
**   IPAPP Management API Enums
**------------------------------------------------------------------------------
*/


/** Return status for app registration
*/
typedef enum
{
  CMIPAPP_STATUS_NONE        = -1,
    /**< FOR INTERNAL USE OF CM ONLY!
    */

  CMIPAPP_STATUS_OK,
    /**< request was completed successfully
    */

  CMIPAPP_STATUS_CB_REG_FAILED,
    /**< Call to register app call backs failed
    */

  CMIPAPP_STATUS_ID_DEALLOC_FAILED,
    /**< Call to deallocate app id failed
    */

  CMIPAPP_STATUS_GET_PH_IDENTITY_FAILED,
    /**< call to get phone identity failed
    */

  CMIPAPP_STATUS_CALL_TYPE_REG_FAILED,
    /**< Request to register app call type failed
    */

  CMIPAPP_STATUS_INFORM_FAILED,
    /**< call to inform app status failed
    */

  CMIPAPP_STATUS_APPID_PTR_ERR,
    /**< call to get app id failed due to null
    ** parameter
    */

  CMIPAPP_STATUS_APPID_EXHAUST,
    /**< No more free app ids available
    */

  CMIPAPP_STATUS_DUP_APP_EXISTS,
    /**< Duplicate app exists
    */

  CMIPAPP_STATUS_GET_CB_PTRS_FAILED,
    /**< Action to get Apps function ptr Failed
    */

  CMIPAPP_STATUS_INVALID_APP_ID,
    /**< App id passed is not within array bound for IP data
    */
  
  CMIPAPP_STATUS_MAX
    /**< FOR INTERNAL USE OF CM ONLY!
    */

} cmipapp_status_e_type;
/*~ SENTINEL cmipapp_status_e_type.CMIPAPP_STATUS_MAX */


/** App active status
*/
typedef enum
{
  CMIPAPP_RUN_STATUS_NONE = -1,
    /**< FOR INTERNAL USE OF CM ONLY!
    */


  CMIPAPP_RUN_STATUS_INACTIVE,
    /**< App is inactive. App can be
    ** regist/deregist but cannot make
    ** a call.
    */

  CMIPAPP_RUN_STATUS_ACTIVE,
    /**< App is active. App is registered
    ** and can make a call.
    */  

  CMIPAPP_RUN_STATUS_MAX
    /**< FOR INTERNAL USE OF CM ONLY!
    */

} cmipapp_run_status_e_type;
/*~ SENTINEL cmipapp_run_status_e_type.CMIPAPP_RUN_STATUS_MAX */

/**--------------------------------------------------------------------------
** Datatypes -   Others
** --------------------------------------------------------------------------
*/

typedef struct {

  cm_ipapp_cause_e_type           ipapp_cause;
    /* CM_IPAPP_CAUSE_NORMAL - When registration is successfull.
    ** Actual cause code when registration fails.
    */

  cm_ipapp_warn_info_s_type       warn_info;
    /* Warning information given by ipapp
    */

} cmipapp_cap_reg_reason_s_type; 

/**--------------------------------------------------------------------------
** Functions - Called by Apps
** ------------------------------------------------------------------------*/

/*===========================================================================

FUNCTION cmipapp_cb_init

DESCRIPTION
  Used by application to initialize cmipapp_cb_s_type structure with
  NULL values for function pointers


DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void cmipapp_cb_init (

    cmipapp_cb_s_type                *app_cb_ptr
    /* Structure pointing to app call back functions
    */

);


/*~ FUNCTION cmipapp_cb_init */
/*~ PARAM OUT app_cb_ptr POINTER */

/*===========================================================================

FUNCTION cmipapp_reg

DESCRIPTION
  Used by application to register callback functions with CM

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE
  CMIPAPP_STATUS_OK - Success, Other enumerations - Specific failure reasons          

SIDE EFFECTS
  None

===========================================================================*/

extern cmipapp_status_e_type cmipapp_reg (

    const cmipapp_cb_s_type               *app_cb_ptr,
    /* Structure pointing to app call back functions
    */

    cmipapp_id_type                        app_id,
    /* App id assigned by CM
    */

    uint32                                 data_block
    /* User token passed to CM at registration. CM will
    ** return it back in callback and other notification 
    ** function
    */
);


/*~ FUNCTION cmipapp_reg */
/*~ PARAM app_cb_ptr POINTER */

/*===========================================================================

FUNCTION cmipapp_rpt_ind

DESCRIPTION
  Allows Voip Apps to queue up messages to the CM. Grab a buffer from the
  cmipapp_rpt_free_q, fill it in, and call cmipapp_rpt_ind with it.
  All this will do is queue it and set a signal for the CM task.  When the
  command is done, the buffer will be queued as directed in the command
  header, and a task you specify may be signaled.


DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void cmipapp_rpt_ind (

  cm_name_type        ip_rpt_name,
    /* Name of report being queued to CM
    */

  cmipapp_ind_s_type     ip_ind,
    /* Union of all rpt types getting queued to CM
    */

  uint32           data_block
    /* Token created and used by CM.
    ** data_block is passed by CM, when calling app registered
    ** callbacks. Client needs to pass the most recent value
    ** given by CM, all the time when cmipapp_rpt_ind () is called.
    */
);


/*~ FUNCTION cmipapp_rpt_ind */
/*~ PARAM ip_ind DISC ip_rpt_name */

/*===========================================================================

FUNCTION cmipapp_cap_reg

DESCRIPTION
  Allows clients to register call handling capabilities

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE
  CMIPAPP_STATUS_OK - Success, Other enumerations - Specific failure reasons          

SIDE EFFECTS
  None
===========================================================================*/

extern cmipapp_status_e_type cmipapp_cap_reg (

  cmipapp_id_type             app_id,
    /* App ID
    */

  cmipapp_call_type_e_type    call_type,
    /* Bit Masks of call type it supports
    */

  cmipapp_sys_mode_e_type     sys_mode
    /* System modes app can support for calls
    */
);


/*~ FUNCTION cmipapp_cap_reg */

/*===========================================================================

FUNCTION cmipapp_cap_update

DESCRIPTION
Updates the capabilties array.
If there is a duplicate of the same call type already in the 
capabilities array, the capability in the array is replaced 
with this new one

DEPENDENCIES
cmipapp_init

RETURN VALUE

CMIPAPP_STATUS_OK            - Capabilities successfully updated

SIDE EFFECTS

Modifies global cmipapp_arr[].
===========================================================================*/
cmipapp_status_e_type cmipapp_cap_update (

  cmipapp_call_type_e_type     call_type,
  /* Bit Mask of Call Types
  */

  cmipapp_sys_mode_e_type      sys_mode,
  /* System modes app can support for calls
  */

  cmipapp_id_type             app_id
  /* App ID pointer
  */
);


/*===========================================================================

FUNCTION cmipapp_ecmp_reg

DESCRIPTION
Allows app to inform the ECMP (Emergency call mode preference) value to CM

DEPENDENCIES
cmipapp_init

RETURN VALUE

CMIPAPP_STATUS_OK                   - Successfully registered
CMIPAPP_STATUS_CALL_TYPE_REG_FAILED - Error in registering ecmp value

SIDE EFFECTS
None
===========================================================================*/

extern cmipapp_status_e_type cmipapp_ecmp_reg (

                                       cmipapp_id_type                 app_id,
                                       /* App id
                                       */

                                       cmipapp_ecmp_e_type             ecmp
                                       /* ECMP Value (from 200 ok response)
                                       */
);

/*~ FUNCTION cmipapp_ecmp_reg */

/*===========================================================================

FUNCTION cmipapp_cap_reg_with_reason

DESCRIPTION
  Allows clients to register call handling capabilities

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE
  CMIPAPP_STATUS_OK - Success, Other enumerations - Specific failure reasons          

SIDE EFFECTS
  None
===========================================================================*/

extern cmipapp_status_e_type cmipapp_cap_reg_with_reason (

  cmipapp_id_type                 app_id,
    /* App ID
    */

  cmipapp_call_type_e_type        call_type,
    /* Bit Masks of call type it supports
    */

  cmipapp_sys_mode_e_type         sys_mode,
    /* System modes app can support for calls
    */

  cmipapp_cap_reg_reason_s_type   reason
    /* reason carries success or details of failure  
    ** with respect to app registeration with network.
    */  
);

/*~ FUNCTION cmipapp_cap_reg_with_reason */

/*===========================================================================

FUNCTION cmipapp_inform_status

DESCRIPTION
  Client informs CM if it is active or inactive

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE
  CMIPAPP_STATUS_OK - Success, Other enumerations - Specific failure reasons          

SIDE EFFECTS
  None
===========================================================================*/

extern cmipapp_status_e_type cmipapp_inform_status (

  cmipapp_id_type                app_id,
   /* App ID
   */

  cmipapp_run_status_e_type      app_run_status
   /* CMIPAPP_STATUS_ACTIVE  - App is active
   ** CMIPAPP_STATUS_ACTIVE  - App is inactive
   */
);


/*~ FUNCTION cmipapp_inform_status */

/*===========================================================================

FUNCTION cmipapp_inform_status_with_callid

DESCRIPTION
  Client informs CM if it is active or inactive

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE
  CMIPAPP_STATUS_OK - Success, Other enumerations - Specific failure reasons          

SIDE EFFECTS
  None
===========================================================================*/

extern cmipapp_status_e_type cmipapp_inform_status_with_callid (

  cmipapp_id_type                app_id,
   /* App ID
   */

  cmipapp_run_status_e_type      app_run_status,
    /* CMIPAPP_STATUS_ACTIVE  - App is active
    ** CMIPAPP_STATUS_ACTIVE  - App is inactive
    */

  cm_call_id_type                call_id 
    /* voice_call_id on which videoshare call can be made
    ** is supplied here.
    */
);

/*~ FUNCTION cmipapp_inform_status_with_callid */

/*===========================================================================

FUNCTION cmipapp_id_allocate

DESCRIPTION
  Returns a free index from cmipapp_arr[]

DEPENDENCIES
  cmipapp_init

RETURN VALUE

  CMIPAPP_STATUS_APPID_PTR_ERR - Supplied app id ptr is NULL

  CMIPAPP_STATUS_APPID_EXHAUST - No free app id available

  CMIPAPP_STATUS_OK            - App id successfully allocated

SIDE EFFECTS

  Modifies global cmipapp_arr[].
===========================================================================*/
extern cmipapp_status_e_type cmipapp_id_allocate (

    cmipapp_call_type_e_type     call_type,
      /* App type
      */

    cmipapp_sys_mode_e_type      sys_mode,
      /* System modes app can support for calls
      */

    cmipapp_id_type             *app_id_ptr
      /* App ID pointer
      */
);


/*~ FUNCTION cmipapp_id_allocate */
/*~ PARAM INOUT app_id_ptr POINTER */

/*===========================================================================

FUNCTION cmipapp_id_deallocate

DESCRIPTION
  Application calls to deallocate app id

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE

  CMIPAPP_STATUS_OK - Success, Other enumerations - Specific failure reasons          

SIDE EFFECTS
  None

===========================================================================*/
extern cmipapp_status_e_type cmipapp_id_deallocate (

  cmipapp_id_type                         app_id
    /* App id assigned by CM
    */
);

/*~ FUNCTION cmipapp_id_deallocate */


/****************************************************************************
** RPC tool has limitations in remoting function pointers declared in a 
** structure. Defining individual apis for each of the function pointers
** in cmipapp_cb_s_type to overcome the limitation.
****************************************************************************/


/*===========================================================================

FUNCTION cmipapp_reg_orig_func

DESCRIPTION
  Registers function for the app to originate a call.

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE

  CMIPAPP_STATUS_OK - Success, Other enumerations - Specific failure reasons          

SIDE EFFECTS
  None

===========================================================================*/
extern cmipapp_status_e_type cmipapp_reg_orig_func (

  cmipapp_id_type                         app_id,
    /* App id assigned by CM
    */

  cmipapp_cmd_orig_f_type                 *cmipapp_cmd_orig_func_ptr
    /* Ptr to call orig function
    */
);


/*~ FUNCTION cmipapp_reg_orig_func */

/*===========================================================================

FUNCTION cmipapp_reg_answer_func

DESCRIPTION
  Registers function for the app to answer a MT call.

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE

  CMIPAPP_STATUS_OK - Success, Other enumerations - Specific failure reasons          

SIDE EFFECTS
  None

===========================================================================*/
extern cmipapp_status_e_type cmipapp_reg_answer_func (

  cmipapp_id_type                         app_id,
    /* App id assigned by CM
    */

  cmipapp_cmd_answer_f_type              *cmipapp_cmd_answer_func_ptr
    /* Ptr to MT call answer function
    */
);

/*~ FUNCTION cmipapp_reg_answer_func */


/*===========================================================================

FUNCTION cmipapp_reg_answer_call_waiting_func

DESCRIPTION
  Registers function for the app to answer an call waiting MT call.

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE

  CMIPAPP_STATUS_OK         - Deregistration Successful

SIDE EFFECTS
  None

===========================================================================*/
cmipapp_status_e_type cmipapp_reg_answer_call_waiting_func (

  cmipapp_id_type                         app_id,
    /* App id assigned by CM
    */

  cmipapp_cmd_answer_call_waiting_sub_f_type *answer_call_waiting_func_ptr
    /* Ptr to MT call answer function
    */
);

/*~ FUNCTION cmipapp_reg_answer_call_waiting_func */

/*===========================================================================

FUNCTION cmipapp_reg_simple_flash_func

DESCRIPTION
  Registers function for the app to answer an call waiting MT call or to
  remove the last party added to the conference call

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE

  CMIPAPP_STATUS_OK         - Registration Successful

SIDE EFFECTS
  None

===========================================================================*/
cmipapp_status_e_type cmipapp_reg_simple_flash_func (

  cmipapp_id_type                         app_id,
    /* App id assigned by CM
    */

  cmipapp_cmd_simple_flash_sub_f_type *simple_flash_func_ptr
    /* Ptr to the simple flash request function
    */
);

/*~ FUNCTION cmipapp_reg_simple_flash_func */

/*===========================================================================

FUNCTION cmipapp_reg_invite_res_func

DESCRIPTION
  Registers function for the app to respond to a invite message.

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE

  CMIPAPP_STATUS_OK - Success, Other enumerations - Specific failure reasons          

SIDE EFFECTS
  None

===========================================================================*/
extern cmipapp_status_e_type cmipapp_reg_invite_res_func (

  cmipapp_id_type                         app_id,
    /* App id assigned by CM
    */

  cmipapp_cmd_invite_res_f_type          *cmipapp_cmd_invite_res_func_ptr
    /* Ptr to invite response function
    */
);


/*~ FUNCTION cmipapp_reg_invite_res_func */

/*===========================================================================

FUNCTION cmipapp_reg_end_func

DESCRIPTION
  Registers function for the app to end a call.

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE

  CMIPAPP_STATUS_OK - Success, Other enumerations - Specific failure reasons          

SIDE EFFECTS
  None

===========================================================================*/
extern cmipapp_status_e_type cmipapp_reg_end_func (

  cmipapp_id_type                         app_id,
    /* App id assigned by CM
    */

  cmipapp_cmd_end_f_type                 *cmipapp_cmd_end_func_ptr
    /* Ptr to call end function
    */
);


/*~ FUNCTION cmipapp_reg_end_func */

/*===========================================================================

FUNCTION cmipapp_reg_end_ext_func

DESCRIPTION
  Registers function for the app to end a call.

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE

  CMIPAPP_STATUS_OK - Success, Other enumerations - Specific failure reasons          

SIDE EFFECTS
  None

===========================================================================*/
extern cmipapp_status_e_type cmipapp_reg_end_ext_func (

  cmipapp_id_type                         app_id,
    /* App id assigned by CM
    */

  cmipapp_cmd_end_ext_sub_f_type                 *cmipapp_cmd_end_ext_sub_func_ptr
    /* Ptr to call end function
    */
);


/*===========================================================================

FUNCTION cmipapp_reg_inband_start_cont_dtmf_func

DESCRIPTION
  Registers function for the app to start continuous inband dtmf.

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE

  CMIPAPP_STATUS_OK - Success, Other enumerations - Specific failure reasons          

SIDE EFFECTS
  None

===========================================================================*/
extern cmipapp_status_e_type cmipapp_reg_inband_start_cont_dtmf_func (

  cmipapp_id_type                         app_id,
    /* App id assigned by CM
    */

  cmipapp_inband_cmd_start_cont_dtmf_f_type

                              *cmipapp_inband_cmd_start_cont_dtmf_func_ptr
    /* Ptr to start inband dtmf function
    */
);


/*~ FUNCTION cmipapp_reg_inband_start_cont_dtmf_func */

/*===========================================================================

FUNCTION cmipapp_reg_inband_stop_cont_dtmf_func

DESCRIPTION
  Registers function for the app to stop inband continuous dtmf.

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE

  CMIPAPP_STATUS_OK - Success, Other enumerations - Specific failure reasons          

SIDE EFFECTS
  None

===========================================================================*/
extern cmipapp_status_e_type cmipapp_reg_inband_stop_cont_dtmf_func (

  cmipapp_id_type                         app_id,
    /* App id assigned by CM
    */

  cmipapp_inband_cmd_stop_cont_dtmf_f_type

                              *cmipapp_inband_cmd_stop_cont_dtmf_func_ptr
    /* Ptr to stop inband dtmf function
    */
);


/*~ FUNCTION cmipapp_reg_inband_stop_cont_dtmf_func */

/*===========================================================================

FUNCTION cmipapp_reg_hold_func

DESCRIPTION
  Registers function for the app to hold a call

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE

  CMIPAPP_STATUS_OK - Success, Other enumerations - Specific failure reasons          

SIDE EFFECTS
  None

===========================================================================*/
extern cmipapp_status_e_type cmipapp_reg_hold_func (

  cmipapp_id_type                         app_id,
    /* App id assigned by CM
    */

  cmipapp_cmd_hold_f_type                *cmipapp_cmd_hold_func_ptr
    /* Ptr to call hold function 
    */
);


/*~ FUNCTION cmipapp_reg_hold_func */

/*===========================================================================

FUNCTION cmipapp_reg_active_func

DESCRIPTION
  Registers function for the app to make the call active.

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE

  CMIPAPP_STATUS_OK - Success, Other enumerations - Specific failure reasons          

SIDE EFFECTS
  None

===========================================================================*/
extern cmipapp_status_e_type cmipapp_reg_active_func (

  cmipapp_id_type                         app_id,
    /* App id assigned by CM
    */

  cmipapp_cmd_active_f_type              *cmipapp_cmd_active_func_ptr
    /* Ptr to call active function
    */
);


/*~ FUNCTION cmipapp_reg_active_func */

/*===========================================================================

FUNCTION cmipapp_reg_modify_func

DESCRIPTION
  Registers function for the app to modify the call type (ex:- voice to VT).

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE

  CMIPAPP_STATUS_OK - Success, Other enumerations - Specific failure reasons          

SIDE EFFECTS
  None

===========================================================================*/
extern cmipapp_status_e_type cmipapp_reg_modify_func (

  cmipapp_id_type                         app_id,
    /* App id assigned by CM
    */

  cmipapp_cmd_modify_f_type              *cmipapp_cmd_modify_func_ptr
    /* Ptr to call modify function
    */
);


/*~ FUNCTION cmipapp_reg_modify_func */

/*===========================================================================

FUNCTION cmipapp_reg_act_answer_hold_func

DESCRIPTION
  Registers function for the app to activate answer hold.

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE

  CMIPAPP_STATUS_OK - Success, Other enumerations - Specific failure reasons          

SIDE EFFECTS
  None

===========================================================================*/
extern cmipapp_status_e_type cmipapp_reg_act_answer_hold_func (

  cmipapp_id_type                      app_id,
    /* App id assigned by CM
    */

  cmipapp_cmd_act_answer_hold_sub_f_type 
                                      *cmipapp_cmd_act_answer_hold_func_ptr
    /* Ptr to activate answer hold function
    */
);
/*~ FUNCTION cmipapp_reg_act_answer_hold_func */

/*===========================================================================

FUNCTION cmipapp_reg_deact_answer_hold_func

DESCRIPTION
  Registers function for the app to de-activate answer hold.

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE

  CMIPAPP_STATUS_OK - Success, Other enumerations - Specific failure reasons          

SIDE EFFECTS
  None

===========================================================================*/
extern cmipapp_status_e_type cmipapp_reg_deact_answer_hold_func (

  cmipapp_id_type                      app_id,
    /* App id assigned by CM
    */

  cmipapp_cmd_deact_answer_hold_sub_f_type 
                                      *cmipapp_cmd_deact_answer_hold_func_ptr
    /* Ptr to de-activate answer hold function
    */
);


/*~ FUNCTION cmipapp_reg_deact_answer_hold_func */

/*===========================================================================

FUNCTION cmipapp_reg_serv_sys_info_func

DESCRIPTION
  Registers function for the app to be informed of current service.

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE

  CMIPAPP_STATUS_OK - Success, Other enumerations - Specific failure reasons          

SIDE EFFECTS
  None

===========================================================================*/
extern cmipapp_status_e_type cmipapp_reg_serv_sys_info_func (

  cmipapp_id_type                      app_id,
    /* App id assigned by CM
    */

  cmipapp_cmd_srv_sys_info_sub_f_type 
                                      *cmipapp_cmd_srv_sys_info_func_ptr
    /* Ptr to function that will inform app of service
    */
);


/*~ FUNCTION cmipapp_reg_serv_sys_info_func */

/*===========================================================================

FUNCTION cmipapp_reg_ect_func

DESCRIPTION
  Registers function for the app to provide Call transfer funcitonality.

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE

  CMIPAPP_STATUS_OK - Success, Other enumerations - Specific failure reasons          

SIDE EFFECTS
  None

===========================================================================*/
extern cmipapp_status_e_type cmipapp_reg_ect_func (

  cmipapp_id_type                      app_id,
    /* App id assigned by CM
    */

  cmipapp_cmd_ect_sub_f_type 
                                      *cmipapp_cmd_ect_func_ptr
    /* Ptr to call transfer function.
    */
);


/*~ FUNCTION cmipapp_reg_ect_func */


/*===========================================================================

FUNCTION cmipapp_reg_ip_explicit_call_transfer_func

DESCRIPTION
  Registers callback function of the app to provide
  IP Explicit Call transfer funcitonality.

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE
  CMIPAPP_STATUS_OK         - Deregistration Successful

SIDE EFFECTS
  None

===========================================================================*/
cmipapp_status_e_type cmipapp_reg_ip_explicit_call_transfer_func (

  cmipapp_id_type                                  app_id,
  /* App id assigned by CM */

  cmipapp_cmd_ip_explicit_call_transfer_f_type    *cmipapp_cmd_ip_ect_sub_func_ptr
  /* Ptr to call ip explicit call transfer function   */
);

/*~ FUNCTION cmipapp_reg_ip_explicit_call_transfer_func */


/*===========================================================================

FUNCTION cmipapp_reg_mpty_func

DESCRIPTION
  Registers function for the app to provide Multiparty functionality.

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE

  CMIPAPP_STATUS_OK - Success, Other enumerations - Specific failure reasons          

SIDE EFFECTS
  None

===========================================================================*/
extern cmipapp_status_e_type cmipapp_reg_mpty_func (

  cmipapp_id_type                      app_id,
    /* App id assigned by CM
    */

  cmipapp_cmd_mpty_sub_f_type 
                                      *cmipapp_cmd_mpty_func_ptr
    /* Ptr to de-activate answer hold function
    */
);


/*~ FUNCTION cmipapp_reg_mpty_func */

/*===========================================================================

FUNCTION cmipapp_reg_mo_sms_func

DESCRIPTION
  Registers function for the app to be informed of MO SMS message.

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE

  CMIPAPP_STATUS_OK         - Deregistration Successful

SIDE EFFECTS
  None

===========================================================================*/
extern cmipapp_status_e_type cmipapp_reg_mo_sms_func (

  cmipapp_id_type                      app_id,
    /* App id assigned by CM
    */

  cmipapp_sms_cmd_mo_sms_sub_f_type        *cmipapp_sms_cmd_mo_sms_func_ptr
    /* Ptr to send MO SMS message function
    */
);

/*~ FUNCTION cmipapp_reg_mo_sms_func */

/*===========================================================================

FUNCTION cmipapp_reg_abort_func

DESCRIPTION
  Registers function for the app to abort a call.

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE

  CMIPAPP_STATUS_OK         - Deregistration Successful

SIDE EFFECTS
  None

===========================================================================*/
extern cmipapp_status_e_type cmipapp_reg_abort_func (

  cmipapp_id_type                         app_id,
    /* App id assigned by CM
    */

  cmipapp_cmd_abort_sub_f_type                 *cmipapp_cmd_abort_func_ptr
    /* Ptr to call end function
    */
);
/*~ FUNCTION cmipapp_reg_abort_func */


/*===========================================================================

FUNCTION cmipapp_reg_call_deflection_func

DESCRIPTION
  Registers function for the app to handle communication deflection.

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE

  CMIPAPP_STATUS_OK         - Deregistration Successful

SIDE EFFECTS
  None

===========================================================================*/
extern cmipapp_status_e_type cmipapp_reg_call_deflection_func (
  
  cmipapp_id_type                         app_id,
      /* App id assigned by CM
      */
  
  cmipapp_call_deflection_sub_f_type         *cmipcall_call_deflection_cmd_func_ptr
      /* Ptr to call end function
      */
);
  /*~ FUNCTION cmipapp_reg_call_deflection_func */


/*===========================================================================

FUNCTION cmipapp_reg_inform_secure_call_capability_func

DESCRIPTION
  API which IMS needs to invoke to register a call_back using 
  which CM can inform TF Card Status (Secure Call Capability Status) info to IMS.

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE
  CMIPAPP_STATUS_OK         - Deregistration Successful

SIDE EFFECTS
  None

===========================================================================*/
extern cmipapp_status_e_type cmipapp_reg_inform_secure_call_capability_func (
  cmipapp_id_type                         app_id,
    /* App id assigned by CM    
    */

  cmipapp_cmd_inform_secure_call_capability_f_type                 *cmipapp_cmd_inform_secure_call_capability_func_ptr
    /* Ptr to secure call capability function 
    */
);
/*~ FUNCTION cmipapp_reg_inform_secure_call_capability_func */


/*===========================================================================

FUNCTION cmipapp_reg_inform_secure_context_func

DESCRIPTION
  API which IMS needs to invoke to register a call_back using which
  CM can inform secure context establishment info to IMS.

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE
  CMIPAPP_STATUS_OK         - Deregistration Successful

SIDE EFFECTS
  None

===========================================================================*/
extern cmipapp_status_e_type cmipapp_reg_inform_secure_context_func (

  cmipapp_id_type                         app_id,
    /* App id assigned by CM  
    */

  cmipapp_cmd_inform_secure_context_f_type                 *cmipapp_cmd_inform_secure_context_func_ptr
    /* Ptr to inform secure context function 
    */
);
/*~ FUNCTION cmipapp_reg_inform_secure_context_func */



/*===========================================================================

FUNCTION cmipapp_call_id_allocate

DESCRIPTION
  Returns a new CM call id

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE
  CM_CALL_ID_INVALID - invalid call id
  valid call id

SIDE EFFECTS
  None
===========================================================================*/
extern cm_call_id_type cmipapp_call_id_allocate (

  void
);


/*~ FUNCTION cmipapp_call_id_allocate */

/*===========================================================================

FUNCTION cmipapp_call_id_deallocate

DESCRIPTION
  Deallocates a CM call id

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void cmipapp_call_id_deallocate (

   cm_call_id_type call_id
     /* Call id */
);


/*~ FUNCTION cmipapp_call_id_deallocate */


/*===========================================================================

FUNCTION cmipapp_reg_subs_cmd_func

DESCRIPTION
  Registers function for the app to be informed of Phone's subscription change .  

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE
  CMIPAPP_STATUS_OK             - Registration successfull.
  CMIPAPP_STATUS_CB_REG_FAILED  - Registration failed.

SIDE EFFECTS
  None
===========================================================================*/
cmipapp_status_e_type cmipapp_reg_subs_cmd_func (

  cmipapp_id_type                      app_id,
    /* App id assigned by CM
    */

  cmipapp_send_subsc_chngd_f_type
                                      *cmipapp_sub_cmd_func_ptr
    /* Ptr to record subs cmd  
    */
);

/*===========================================================================

FUNCTION cmipapp_reg_oprt_mode_func

DESCRIPTION
  Registers function for the app that gets called 
  to be informed about Phone's operating mode.

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE
  CMIPAPP_STATUS_OK - Success, Other enumerations - Specific failure reasons          

SIDE EFFECTS
  None
===========================================================================*/
extern cmipapp_status_e_type cmipapp_reg_oprt_mode_func (

  cmipapp_id_type                      app_id,
    /* App id assigned by CM
    */

  cmipapp_inform_oprt_mode_f_type 
                                      *cmipapp_inform_oprt_mode_func_ptr
    /* Ptr to record oprt mode function
    */
);


/*~ FUNCTION cmipapp_reg_oprt_mode_func */

/*===========================================================================

FUNCTION cmipapp_oprt_mode_cnf

DESCRIPTION
  Api called by app to confirm processing
  of operating mode request.

DEPENDENCIES
  cmipapp_init

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void cmipapp_oprt_mode_cnf (

  cmipapp_id_type                      app_id,
    /* App id assigned by CM
    */

  cmipapp_trans_id_type                trans_id
    /* Transaction id
    */
);

/*~ FUNCTION cmipapp_oprt_mode_cnf */

/*===========================================================================

FUNCTION cmipapp_reg_srv_domain_func

DESCRIPTION
  Registers function for the app that gets called 
  to be informed about srv_domain pref set by clients.

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE
  CMIPAPP_STATUS_OK - Success, Other enumerations - Specific failure reasons          

SIDE EFFECTS
  None
===========================================================================*/
extern cmipapp_status_e_type cmipapp_reg_srv_domain_func (

  cmipapp_id_type                      app_id,
    /* App id assigned by CM
    */

  cmipapp_inform_srv_domain_sub_f_type 
                                      *cmipapp_inform_srv_domain_func_ptr
    /* Ptr to record srv_domain function
    */
);


/*~ FUNCTION cmipapp_reg_srv_domain_func */

/*===========================================================================

FUNCTION cmipapp_srv_domain_cnf

DESCRIPTION
  Api called by app to confirm processing
  of srv_domain information.

DEPENDENCIES
  cmipapp_init

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void cmipapp_srv_domain_cnf (

  cmipapp_id_type                      app_id,
    /* App id assigned by CM
    */

  cmipapp_trans_id_type                trans_id,
    /* Transaction id
    */

  cmipapp_ind_common_s_type   ind_common
    /**< Common indication data */
);

/*~ FUNCTION cmipapp_srv_domain_cnf */

/*===========================================================================

FUNCTION cmipapp_send_srvcc_notification

DESCRIPTION
  Api called by CM to notify SRVCC status to IMS client

DEPENDENCIES
  cmipapp_init

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void cmipapp_send_srvcc_notification
(
  cmipapp_srvcc_notification_e_type  srvcc_status,
  /* srvcc status  */
  
  sys_modem_as_id_e_type             as_id  
);
/*~ FUNCTION cmipapp_send_srvcc_notification */

/*===========================================================================
FUNCTION cmipapp_send_orig_cmd

DESCRIPTION
  Send call orig cmd to IP App.

RETURN VALUE
  TRUE - IP App recvd call origination.
  FALSE - call orig is not sent to IP App
===========================================================================*/
boolean cmipapp_send_orig_cmd(

  cmipapp_id_type          app_id,
  
  const cm_num_s_type      called_num,
    /* Number dialed - phone that is being called */

  const cm_call_id_type    call_id,
    /* call ID of the call */

  cmipapp_call_type_e_type call_type,
    /* call type for the call */

  cmipapp_sys_mode_e_type  sys_mode,
    /* Sys mode call needs to be originated on */

  cm_ip_call_attrib_info_s_type  *call_attrib_info,
    /* Indicate audio/video attributes */

  const cm_npi_type              pi,
    /* Presentation indicator */

  sys_modem_as_id_e_type   asubs_id
    /**< Indicates the asubs_id */

);

/*===========================================================================
FUNCTION cmipapp_send_answer_cmd

DESCRIPTION
  Send answer response for incoming call.

RETURN VALUE
  TRUE - IP App sent call response.
  FALSE - Answer response was not sent.
===========================================================================*/
boolean cmipapp_send_answer_cmd(

  cmipapp_id_type                  app_id,
    /* App handling this call */

  const cm_call_id_type            call_id,
    /* call id of incoming call */

  boolean                          accept,
    /* Flag indicating whether the call is accepted or not */

  cmipapp_incom_rej_cause_e_type   answer_cause,
    /* Cause for rejecting a call */

  cmipapp_call_type_e_type         call_type,
    /* Indicate the call type when call is accepted by client */

  cm_ip_call_attrib_info_s_type    call_attrib,
    /* Current call attributes. */

  const cm_npi_type                      pi,
    /* Presentation indicator */

  cm_ip_call_sip_reject_code_s_type      sip_reject_code,
    /* SIP code sent for rejecting incoming call */

  sys_modem_as_id_e_type   asubs_id
    /**< Indicates the asubs_id */
);

/*===========================================================================
FUNCTION cmipapp_send_invite_response

DESCRIPTION
  Send response to invite received by IP App.

RETURN VALUE
  TRUE - Was able to send response to IP App.
  FALSE - Failed to send response to IP App.
===========================================================================*/
boolean cmipapp_send_invite_response(

  cmipapp_id_type                  app_id,
    /* App handling this call */

  cm_call_id_type                  call_id,
    /* call id of the call */

  boolean                          accept,
    /* Flag indicating whether the call is accepted or not */

  cmipapp_incom_rej_cause_e_type   answer_cause,
    /* If Invite is declined, specify the reason */

  sys_modem_as_id_e_type   asubs_id
    /**< Indicates the asubs_id */
);

/*===========================================================================
FUNCTION cmipapp_send_call_end_cmd

DESCRIPTION
  Send cmd to end the call with given call id parameter.

RETURN VALUE
  TRUE - Call end was sent.
  FALSE - Call end was not sent to IP App.
===========================================================================*/
boolean cmipapp_send_call_end_cmd(

  cmipapp_id_type       app_id,
  
  const cm_call_id_type call_id,

  cm_call_end_e_type	  end_cause,

  sys_modem_as_id_e_type   asubs_id
    /**< Indicates the asubs_id */
);

/*===========================================================================
FUNCTION cmipapp_send_start_inband_dtmf_cmd

DESCRIPTION
  Send cmd to start DTMF.

RETURN VALUE
  TRUE - If start DTMF was sent to App.
  FALSE - Failed to send cmd to App.
===========================================================================*/
boolean cmipapp_send_start_inband_dtmf_cmd(

  cmipapp_id_type       app_id,

  const cm_call_id_type call_id,

  uint8                 digit,
    /* DTMF digit */

  sys_modem_as_id_e_type   asubs_id
    /**< Indicates the asubs_id */
);

/*===========================================================================
FUNCTION cmipapp_send_stop_inband_dtmf_cmd

DESCRIPTION
  Send cmd to stop DTMF.

RETURN VALUE
  TRUE - If stop DTMF was sent to App.
  FALSE - Failed to send cmd to App.
===========================================================================*/
boolean cmipapp_send_stop_inband_dtmf_cmd(

  cmipapp_id_type       app_id,

  const cm_call_id_type call_id,

  uint16                duration,
    /* DTMF duration */

  sys_modem_as_id_e_type   asubs_id
    /**< Indicates the asubs_id */
);

/*===========================================================================
FUNCTION cmipapp_send_hold_cmd

DESCRIPTION
  Sends hold cmd for a call to IP App.

RETURN VALUE
  TRUE - Sent hold cmd to IP App.
  FALSE - Failed to send to IP App.
===========================================================================*/
boolean cmipapp_send_hold_cmd(

  cmipapp_id_type       app_id,

  const cm_call_id_type call_id,

  sys_modem_as_id_e_type   asubs_id
    /**< Indicates the asubs_id */

);

/*===========================================================================
FUNCTION cmipapp_send_local_hold_cmd

DESCRIPTION
  Sends local hold cmd for a call to IP App.

RETURN VALUE
  TRUE - Sent local hold cmd to IP App.
  FALSE - Failed to send to IP App.
===========================================================================*/
boolean cmipapp_send_local_hold_cmd(

  cmipapp_id_type       app_id,

  const cm_call_id_type call_id,

  const sys_modem_as_id_e_type asubs_id

);

/*===========================================================================
FUNCTION cmipapp_send_resume_cmd

DESCRIPTION
  Sends resume cmd to previously held call.

RETURN VALUE
  TRUE - Sent resume cmd to IP App.
  FALSE - Failed to send to IP App.
===========================================================================*/
boolean cmipapp_send_resume_cmd(

  cmipapp_id_type       app_id,

  const cm_call_id_type call_id,

  sys_modem_as_id_e_type   asubs_id
    /**< Indicates the asubs_id */

);

/*===========================================================================
FUNCTION cmipapp_send_local_retrieve_cmd

DESCRIPTION
  Sends local retrieve cmd to previously local held call.

RETURN VALUE
  TRUE - Sent local retrieve cmd to IP App.
  FALSE - Failed to send to IP App.
===========================================================================*/
boolean cmipapp_send_local_retrieve_cmd(

  cmipapp_id_type       app_id,

  const cm_call_id_type call_id,

  const sys_modem_as_id_e_type asubs_id

);

/*===========================================================================
FUNCTION cmipapp_send_modify_call_cmd

DESCRIPTION
  Sends cmd to modify call type.

RETURN VALUE
  TRUE - Sent change call type cmd to IP App.
  FALSE - Failed to send to IP App.
===========================================================================*/
boolean cmipapp_send_modify_call_cmd(

  cmipapp_id_type                app_id,

  const cm_call_id_type          call_id,

  cmipapp_call_type_e_type       cap,
    /* Requested call type */

  cm_ip_call_attrib_info_s_type  call_attrib,
    /* Audio/Video call attribute */

  sys_modem_as_id_e_type   asubs_id
    /**< Indicates the asubs_id */

);


/*===========================================================================
FUNCTION cmipapp_send_modify_response

DESCRIPTION
  Send response to modify call type response.

RETURN VALUE
  TRUE - Modify response is sent.
  FALSE - Failed to send to IP App.
===========================================================================*/
boolean cmipapp_send_modify_response(

  cmipapp_id_type               app_id,

  const cm_call_id_type         call_id,

  boolean                       call_modif_rsp,
    /* Call type to modify the call into */

  cmipapp_call_type_e_type      call_type_rsp,
    /* responded call type */
    
  cm_ip_call_attrib_info_s_type call_attrib,
    /* Audio/Video call attribute */

  sys_modem_as_id_e_type   asubs_id
    /**< Indicates the asubs_id */

);

/*===========================================================================
FUNCTION cmipapp_send_activate_answer_hold_cmd

DESCRIPTION
  Sends cmd to activate answer hold.

RETURN VALUE
  TRUE - Answer hold was activate sent.
  FALSE - Failed to send to IP App.
===========================================================================*/
boolean cmipapp_send_activate_answer_hold_cmd(

  cmipapp_id_type               app_id,

  const cm_call_id_type         call_id,

  sys_modem_as_id_e_type   asubs_id
    /**< Indicates the asubs_id */

);

/*===========================================================================
FUNCTION cmipapp_send_deactivate_answer_hold_cmd

DESCRIPTION
  Sends cmd to deactivate answer hold.

RETURN VALUE
  TRUE - Answer hold was deactivate sent.
  FALSE - Failed to send to IP App.
===========================================================================*/
boolean cmipapp_send_deactivate_answer_hold_cmd(

  cmipapp_id_type               app_id,

  const cm_call_id_type         call_id,

  sys_modem_as_id_e_type   asubs_id
    /**< Indicates the asubs_id */

);

/*===========================================================================
FUNCTION cmipapp_send_answer_call_waiting_response

DESCRIPTION
  Answer call which is in waiting.

RETURN VALUE
  TRUE - Answer cmd is sent.
  FALSE - Failed to send to IP App.
===========================================================================*/
boolean cmipapp_send_answer_call_waiting_response(

  cmipapp_id_type               app_id,

  const cm_call_id_type         call_id,

  sys_modem_as_id_e_type   asubs_id
    /**< Indicates the asubs_id */

);

/*===========================================================================
FUNCTION cmipapp_send_simple_flash_cmd

DESCRIPTION
  Send flash event either to accept a pending call or remove last participant
  from a conference.

RETURN VALUE
  TRUE - flash request sent.
  FALSE - Failed to send to IP App.
===========================================================================*/
boolean cmipapp_send_simple_flash_cmd(

  cmipapp_id_type              app_id,

  const cm_call_id_type        call_id,

  const cm_mm_call_info_s_type *call_info_ptr,
    /* Call info on flash cmd is applied */

  sys_modem_as_id_e_type   asubs_id
    /**< Indicates the asubs_id */
);

/*===========================================================================
FUNCTION cmipapp_send_ect_on_calls_cmd

DESCRIPTION
  Applies ECT supplementary service on existing calls for the APP.
  ECT - A & B have a call
        A & C have a call
        Now A wants connect C and B by using ECT service.

RETURN VALUE
  TRUE - ECT request sent.
  FALSE - Failed to send to IP App.
===========================================================================*/
boolean cmipapp_send_ect_on_calls_cmd(

  cmipapp_id_type               app_id,

  sys_modem_as_id_e_type   asubs_id
    /**< Indicates the asubs_id */
);

/*===========================================================================
FUNCTION cmipapp_send_mpty_cmd

DESCRIPTION
  When a phone has 2 calls, one active, othe hold. This API merges both into a
  conference - MPTY ( MultiParTY).

RETURN VALUE
  TRUE - MPTY cmd sent.
  FALSE - Failed to send to IP App.
===========================================================================*/
boolean cmipapp_send_mpty_cmd(

  cmipapp_id_type               app_id,

  sys_modem_as_id_e_type   asubs_id
    /**< Indicates the asubs_id */
);

/*===========================================================================
FUNCTION cmipapp_send_mo_sms_cmd

DESCRIPTION
  Sends MO SMS to IP APP.

RETURN VALUE
  TRUE - SMS request sent.
  FALSE - Failed to send to IP App.
===========================================================================*/
boolean cmipapp_send_mo_sms_cmd(

  cmipapp_id_type     app_id,

  cm_sms_msg_type     *data,
    /* The pointer to MO SMS data */

  sys_sys_mode_e_type sys_mode,
    /* System mode over which SMS is sent */

  sys_modem_as_id_e_type   asubs_id
    /**< Indicates the asubs_id */
);

/*===========================================================================
FUNCTION cmipapp_send_abort_call_cmd

DESCRIPTION
  Sends abort request to IP App for existing call.

RETURN VALUE
  TRUE - Abort request sent.
  FALSE - Failed to send to IP App.
===========================================================================*/
boolean cmipapp_send_abort_call_cmd(

  cmipapp_id_type     app_id,

  const cm_call_id_type call_id,

  const cmipapp_abort_cause_e_type abort_cause,
    /* Reason for aborting a call */

  sys_modem_as_id_e_type   asubs_id
    /**< Indicates the asubs_id */
);

/*===========================================================================
FUNCTION cmipapp_send_remove_last_party_cmd

DESCRIPTION
  Sends request to remove last party from conference.

RETURN VALUE
  TRUE - Request was sent.
  FALSE - Failed to send to IP App.
===========================================================================*/
boolean cmipapp_send_remove_last_party_cmd(

  cmipapp_id_type     app_id,

  cm_num_s_type       call_num,
    /* Call number need to be removed from conference */

  sys_modem_as_id_e_type   asubs_id
    /**< Indicates the asubs_id */
);

/*===========================================================================
FUNCTION cmipapp_send_add_new_party_cmd

DESCRIPTION
  Sends request to add new party 

RETURN VALUE
  TRUE - Request was sent.
  FALSE - Failed to send to IP App.
===========================================================================*/
boolean cmipapp_send_add_new_party_cmd(

 cmipapp_id_type     app_id,

 cm_call_id_type      call_id,
   /* call that needs to add this new party */

 cm_num_s_type       party_num,
   /* party need to be added to  from normal/conference */

  sys_modem_as_id_e_type   asubs_id
    /**< Indicates the asubs_id */
);

/*===========================================================================
FUNCTION cmipapp_find_ics_support_app

DESCRIPTION
  Check if ICS is supported and return the corresponding app_id. 

RETURN VALUE
  cmipapp_id_type
===========================================================================*/

extern cmipapp_id_type cmipapp_find_ics_support_app( void );

/*===========================================================================
FUNCTION cmipapp_send_sups_cmd

DESCRIPTION
  Sends request of sups command .

RETURN VALUE
  TRUE - Request was sent.
  FALSE - Failed to send to IP App.
===========================================================================*/
boolean cmipapp_send_sups_cmd(

  cmipapp_id_type     app_id,

  cmipsups_cmd_s_type    *sups_cmd,

  sys_modem_as_id_e_type   asubs_id
    /**< Indicates the asubs_id */
);

/*===========================================================================
FUNCTION cmipapp_find_ussd_support_app

DESCRIPTION
  Check if USSD is supported over IP and return the corresponding app_id. 

RETURN VALUE
  cmipapp_id_type
===========================================================================*/

cmipapp_id_type cmipapp_find_ussd_support_app( void );


/*===========================================================================

FUNCTION cmipapp_subs_cmd_cnf

DESCRIPTION
  Api called by app to confirm processing
  of subs unavailable cmd request 

DEPENDENCIES
  cmipapp_init

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void cmipapp_subs_cmd_cnf (

  cmipapp_id_type                      app_id,
    /* App id assigned by CM
    */

  cmipapp_trans_id_type                trans_id,
    /* Transaction id
    */   

	uint8                              subs_id_bitmask
    /* Bitmask of the subs_id impacted. Based on SYS_MODEM_AS_ID_MASK_*
    */
);



/*===========================================================================
FUNCTION cmipapp_send_call_deflection_cmd

DESCRIPTION
  Sends request to deflect incoming call to deflect_num 

RETURN VALUE
  TRUE - Request was sent.
  FALSE - Failed to send to IP App.
===========================================================================*/
boolean cmipapp_send_call_deflection_cmd(

  cmipapp_id_type     app_id,

  cm_call_id_type      call_id,
    /* call that needs to be deflected to deflect_num */

  cm_num_s_type       deflect_num,
    /* destination number on which call is to be deflected */

  sys_modem_as_id_e_type   asubs_id
    /**< Indicates the asubs_id */
); 



/*===========================================================================

FUNCTION cmipapp_is_wait_on_subsc_cmd_cnf

DESCRIPTION
  Checks if ipapp would need to deregister on this request.

DEPENDENCIES
  Call object must have been initialized with cmcall_init.

RETURN VALUE
  None.

SIDE EFFECTS
  Nothing.

===========================================================================*/
boolean cmipapp_is_wait_on_subsc_cmd_cnf (

  cm_ph_cmd_e_type  subs_cmd
    /* subs cmd 
    */
);

/*===========================================================================
FUNCTION cmipapp_send_switch_speech_codec_cmd

DESCRIPTION
  Sends request to switch speech codec for call 

RETURN VALUE
  TRUE - Request was sent.
  FALSE - Failed to send to IP App.
===========================================================================*/
boolean cmipapp_send_switch_speech_codec_cmd(

  cmipapp_id_type     app_id,

  cm_call_id_type      call_id,
    /* call that needs to be deflected to deflect_num */

  sys_speech_codec_e_type       switch_to_codec,
    /* destination number on which call is to be deflected */

  sys_modem_as_id_e_type   asubs_id
    /**< Indicates the asubs_id */
);

/*===========================================================================

FUNCTION cmipapp_send_drvcc_notification

DESCRIPTION
Send DRVCC notification to IP app

DEPENDENCIES
Call object must have been initialized with cmcall_init.

RETURN VALUE
None.

SIDE EFFECTS
Nothing.

===========================================================================*/
void cmipapp_send_drvcc_notification
(
 cmipapp_drvcc_notification_e_type  drvcc_status,
 /* drvcc status
 */
 boolean orm_send_status,

 sys_modem_as_id_e_type   asubs_id
    /**< Indicates the asubs_id */
 );


/*===========================================================================
FUNCTION cmipapp_send_ip_explict_call_transfer_on_calls_cmd

DESCRIPTION
  IMS supports IP Explicit Call transfer of type - Blind, Assured and Consultative
  This function sends the ECT request to IMS

RETURN VALUE
  TRUE - IP ECT request was sent.
  FALSE - Failed to send to IP App.
===========================================================================*/
boolean cmipapp_send_ip_explict_call_transfer_on_calls_cmd
(
  cmipapp_id_type                         app_id,

  sys_modem_as_id_e_type                  asubs_id,
  /**< Indicates the asubs_id */

  cm_call_id_type                         transferee_call_id,
  /**< call id of call between Transferor (UE B) and Transferee (UE A) */

  cmipapp_ip_expicit_call_transfer_e_type ect_type,
  /**< ECT Type */

  cm_num_s_type                           transfer_target_call_num,
  /**< Calling Number of Transfer target (UE C) */

  cm_call_id_type                         transfer_target_call_id
  /**< Calling ID of Transfer target (UE C) */
);

/*~ FUNCTION cmipapp_send_ip_explict_call_transfer_on_calls_cmd */


/*===========================================================================
FUNCTION cmipapp_send_secure_call_capability

DESCRIPTION

RETURN VALUE
  TRUE - Secure Call capability info was sent.
  FALSE - Failed to send to IP App.
===========================================================================*/
boolean cmipapp_send_secure_call_capability
(
  sys_modem_as_id_e_type                  asubs_id,
  /**< Indicates the asubs_id */

  cmipapp_secure_call_capability_e_type  secure_call_capability
  /**<  Secure Call capability enum  */
);
/*~ FUNCTION cmipapp_send_secure_call_capability */

/*===========================================================================
FUNCTION cmipapp_send_secure_context_info

DESCRIPTION

RETURN VALUE
  TRUE - Secure call capability request was sent.
  FALSE - Failed to send to IP App.
===========================================================================*/
boolean cmipapp_send_secure_context_info
(
  cmipapp_id_type                         app_id,

  sys_modem_as_id_e_type                  asubs_id,
  /**< Indicates the asubs_id */

  cm_call_id_type                         call_id,
  /**<  Secure Context info enum  */

  cmipapp_secure_context_e_type           secure_context_info
  /**<  Secure Context info enum  */
);
/*~ FUNCTION cmipapp_send_secure_context_info */

/*===========================================================================

FUNCTION cmipapp_notify_ue_based_cw

DESCRIPTION
Api called by app to notify if ue based call waiting is enabled

DEPENDENCIES
cmipapp_init

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/

void cmipapp_notify_ue_based_cw (

                            cmipapp_id_type                      app_id,
                            sys_modem_as_id_e_type               asubs_id,
                            boolean                              use_ue_cw
                            );

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* CMIPAPP_H */
