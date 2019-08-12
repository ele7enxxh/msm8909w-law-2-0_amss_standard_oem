/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2001, 2002 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/cnm/vcs/cnm_mntask_debug.c_v   1.8   12 Jul 2002 17:15:28   abanh  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/cnm/src/cnm_mntask_debug.c#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
20-Jun-2001  VT    Removed unwanted declaration & definition of 
                     void debug_output( void )

8/31/01     CD     Fixed ADS110 compile warning

26-Oct-2001  VT   DTMF: In DEBUG_get_timer_name(), removed TIMER_DTMF_ON/OFF
                    & added TIMER_T336/T337
                   In DEBUG_get_state_name(), removed s==U10_DTMF.

8-Mar-2002   VT   In DEBUG_get_state_name(), corrected pstr from (s==CC_STATE_U0)?"-" 
                    to (s==CC_STATE_U0)?"CC_STATE_U0".

12-Mar-2002  VT    Removed Lint errors.
 
04/19/02    CD    Added printing message for CNM_MM_REL_TIMER in debug functions                
                  Updated Copyright

07/03/02    CD    Added cause_of_no_CLI to debug function

08/01/02    CD    Removed printing SMS_CONN_REESTABLISH state since it is not used

09/04/02    AB    In DEBUG_identify_IE(), added IEI_REDIR_SUBADDRESS, and REDIR_BCD_NO.

10/16/02    AB    Added cnm_set_mode(), to set GSM or WCDMA mode of operation
                  for unit test only.

02/12/03    AB    Removed unused header include file, queue.h per Lint.

05/12/05    HS    Added support for FEATURE_CCBS

12/22/06    RD    Removed banned APIs
 
10/10/11    HC    Removed AEEstd.h inclusion as aeestdlib is obsoleted 
                  Replaced the std_strlprintf with C equivalent function snprintf 

===========================================================================*/


#include "mmcp_variation.h"
#include "customer.h"
#include "rex.h"
#include "msg.h"

#include "environ.h"
#include <stdio.h>
#include "sys_type.h"
#include "sys_cnst_v.h"
#include "sys_stru_v.h"
#include "com_iei_v.h"

#include "mn_cnmtask.h"
#include "mn_cnmtask_int.h"
#include "cnm.h"
#include "cmlib.h"


char * DEBUG_identify_IE( byte IEI)
{
   char * pstr;
   static char  temp[20];


   switch (IEI)
   {
      case IEI_LOCKING_SHIFT:
         pstr = "IEI_LOCKING_SHIFT";
         break;

      case IEI_NON_LOCKING_SHIFT:
         pstr = "IEI_NON_LOCKING_SHIFT";
         break;

      case IEI_REPEAT_INDICATOR:
         pstr = "IEI_REPEAT_INDICATOR";
         break;

      case IEI_CONGESTION_CONTROL:
         pstr = "IEI_CONGESTION_CONTROL";
         break;

      case IEI_MORE_DATA:
         pstr = "IEI_MORE_DATA";
         break;

      case IEI_CLIR_SUPPRESSION:
         pstr = "IEI_CLIR_SUPPRESSION";
         break;

      case IEI_CLIR_INVOCATION:
         pstr = "IEI_CLIR_INVOCATION";
         break;

      case IEI_REVERSE_CALL_SETUP_DIR:
         pstr = "IEI_REVERSE_CALL_SETUP_DIR";
         break;

#if defined(FEATURE_MULTIMEDIA_FALLBACK) || defined(FEATURE_REL6)
      case IEI_IMMEDIDATE_MOD_IND:
         pstr = "IEI_IMMEDIDATE_MOD_IND";
         break;

      case IEI_NW_INITIATED_SVC_UPGRADE_IND:
         pstr = "IEI_NW_INITIATED_SVC_UPGRADE_IND";
         break;
#endif /* FEATURE_MULTIMEDIA_FALLBACK */

      case IEI_BEARER_CAPABILITY:
         pstr = "IEI_BEARER_CAPABILITY";
         break;

      case IEI_CAUSE:
         pstr = "IEI_CAUSE";
         break;

      case IEI_FACILITY:
         pstr = "IEI_FACILITY";
         break;

      case IEI_PROGRESS_INDICATOR:
         pstr = "IEI_PROGRESS_INDICATOR";
         break;

      case IEI_AUXILIARY_STATES:
         pstr = "IEI_AUXILIARY_STATES";
         break;

      case IEI_KEYPAD_FACILITY:
         pstr = "IEI_KEYPAD_FACILITY";
         break;

      case IEI_SIGNAL:
         pstr = "IEI_SIGNAL";
         break;

      case IEI_CONNECTED_NUMBER:
         pstr = "IEI_CONNECTED_NUMBER";
         break;

      case IEI_CONNECTED_SUBADDRESS:
         pstr = "IEI_CONNECTED_SUBADDRESS";
         break;

      case IEI_REDIR_PARTY_BCD_NO:
         pstr = "IEI_REDIR_PARTY_BCD_NO";
         break;

      case IEI_REDIR_PARTY_SUBADDR:
         pstr = "IEI_REDIR_PARTY_SUBAD";
         break;

      case IEI_CALLING_PARTY_BCD_NO:
         pstr = "IEI_CALLING_PARTY_BCD_NO";
         break;

      case IEI_CALLING_PARTY_SUBAD:
         pstr = "IEI_CALLING_PARTY_SUBAD";
         break;

      case IEI_CALLED_PARTY_BCD_NO:
         pstr = "IEI_CALLED_PARTY_BCD_NO";
         break;

      case IEI_CALLED_PARTY_SUBAD:
         pstr = "IEI_CALLED_PARTY_SUBAD";
         break;

      case IEI_LOW_LAYER_COMPATIB:
         pstr = "IEI_LOW_LAYER_COMPATIB";
         break;

      case IEI_HIGH_LAYER_COMPATIB:
         pstr = "IEI_HIGH_LAYER_COMPATIB";
         break;

      case IEI_USER_USER:
         pstr = "IEI_USER_USER";
         break;

      case IEI_SS_VERSION_INDICATOR:
         pstr = "IEI_SS_VERSION_INDICATOR";
         break;

      case IEI_CALL_CONTROL_CAPABIL:
         pstr = "IEI_CALL_CONTROL_CAPABIL";
         break;

      case IEI_CALL_STATE:
         pstr = "IEI_CALL_STATE";
         break;

      case IEI_NOTIFICATION_IND:
         pstr = "IEI_NOTIFICATION_IND";
         break;

      case IEI_LAI:
         pstr = "IEI_LAI";
         break;

      case IEI_MOBILE_ID:
         pstr = "IEI_MOBILE_ID";
         break;

      case IEI_MS_CLASSMARK_3:
         pstr = "IEI_MS_CLASSMARK_3";
         break;

      case IEI_CAUSE_OF_NO_CLI:
          pstr = "IEI_CAUSE_OF_NO_CLI";
          break;

      case IEI_ALERTING_PATTERN:
          pstr = "IEI_ALERTING_PATTERN";
          break;

#ifdef FEATURE_CCBS
      case IEI_ALLOWED_ACTIONS:
          pstr = "IEI_ALLOWED_ACTIONS";
          break;

      case IEI_RECALL_TYPE:
          pstr = "IEI_RECALL_TYPE";
          break;

      case IEI_SETUP_CONTAINER:
          pstr = "IEI_SETUP_CONTAINER";
          break;
#endif /* FEATURE_CCBS */

#if defined(FEATURE_MULTIMEDIA_FALLBACK) || defined(FEATURE_REL6)
      case IEI_NETWORK_CC_CAPABILITY:
          pstr = "IEI_NETWORK_CC_CAPABILITY";
          break;

      case IEI_BACKUP_BEARER_CAPABILITY:
          pstr = "IEI_BACKUP_BEARER_CAPABILITY";
          break;
#endif /* FEATURE_MULTIMEDIA_FALLBACK */

      default:
#ifdef PERLUTF
         #error code not present
#else         
         (void)snprintf(temp,sizeof(temp),"Unknown IEI %#x", IEI );
#endif         
         pstr = temp;
         break;

   }

   return pstr;
}

char * DEBUG_get_timer_name(timer_id_T timer_id)
{
   char * pstr;

   switch (timer_id)
   {

      case ALL_TIMERS:
         pstr = "ALL_TIMERS";
         break;
      case ALL_CC_TIMERS:
         pstr = "ALL_CC_TIMERS";
         break;
      case TIMER_T303:
         pstr = "TIMER_T303";
         break;
      case TIMER_T305:
         pstr = "TIMER_T305";
         break;
      case TIMER_T308:
         pstr = "TIMER_T308";
         break;
      case TIMER_T310:
         pstr = "TIMER_T310";
         break;
      case TIMER_T313:
         pstr = "TIMER_T313";
         break;
      case TIMER_T323:
         pstr = "TIMER_T323";
         break;
      case TIMER_T324:
         pstr = "TIMER_T324";
         break;
      case TIMER_T336:
         pstr = "TIMER_T336";
         break;
      case TIMER_T337:
         pstr = "TIMER_T337";
         break;
      case TIMER_TC1M_MO:
         pstr = "TIMER_TC1M_MO";
         break;
      case TIMER_TC1M_MT:
         pstr = "TIMER_TC1M_MT";
         break;
      case MN_SMS_TIMER + TLC1M:
         pstr = "TLC1M";
         break;
      case TIMER_RAB_RELEASED:
         pstr = "TIMER_RAB_RELEASED";
         break;
      case CNM_MM_REL_TIMER:
         pstr = "CNM_MM_REL_TIMER";
         break;
      default:
         pstr = "FK_TIMER";
         break;
   }
   return pstr;
}

char * DEBUG_get_sms_state_name( byte s )
{
   char * pstr =
      (s==SMS_IDLE)?" SMS_IDLE":
      (s==SMS_MM_CONN_PEND)?"SMS_MM_CONN_PEND":
      (s==SMS_WAIT_CP_ACK)?"SMS_WAIT_CP_ACK":
      (s==SMS_MM_CONN_EST)?"SMS_MM_CONN_EST":
      "Unknown SMS State" ;

   return pstr;
}

char * DEBUG_get_ss_state_name( byte s )
{
   char * pstr =
      (s== (byte)SS_IDLE) ? "SS_IDLE" : (s== (byte)SS_MM_CONN_PEND) ? "SS_MM_CONN_PEND": (s==(byte)SS_CONN_EST)?"SS_CONN_EST":
      "Unknown SS State" ;

   return pstr;
}

char * DEBUG_get_state_name( byte s )
{
   char * pstr =
      (s==CC_STATE_U0)?"CC_STATE_U0":
      (s==CC_STATE_U0_1)?"CC_STATE_U0_1":
#ifdef FEATURE_CCBS
      (s==CC_STATE_U0_2)?"CC_STATE_U0_2":
      (s==CC_STATE_U0_3)?"CC_STATE_U0_3":
      (s==CC_STATE_U0_4)?"CC_STATE_U0_4":
      (s==CC_STATE_U0_5)?"CC_STATE_U0_5":
      (s==CC_STATE_U0_6)?"CC_STATE_U0_6":
#endif /* FEATURE_CCBS */
      (s==CC_STATE_U1)?"CC_STATE_U1":
      (s==CC_STATE_U3)?"CC_STATE_U3":
      (s==CC_STATE_U4)?"CC_STATE_U4":
      (s==CC_STATE_U6)?"CC_STATE_U6":
      (s==CC_STATE_U7)?"CC_STATE_U7":
      (s==CC_STATE_U8)?"CC_STATE_U8":
      (s==CC_STATE_U9)?"CC_STATE_U9":
      (s==CC_STATE_U10)?"CC_STATE_U10":
      (s==CC_STATE_U11)?"CC_STATE_U11":
      (s==CC_STATE_U12)?"CC_STATE_U12":
      (s==CC_STATE_U19)?"CC_STATE_U19":
      (s==CC_STATE_U26)?"CC_STATE_U26":
      (s==CC_STATE_U27)?"CC_STATE_U27":
      "UNKNOWN" ;

   return pstr;
}

#ifdef FEATURE_CNM_UNIT_TEST


void cnm_set_mode(char *mode)
{
    extern boolean mode_WCDMA;
    extern boolean mode_GSM;

    if(!memcmp(mode, "GSM", 3))
    {
        mode_GSM = 1;
        mode_WCDMA = 0;
    }
    else if(!memcmp(mode, "WCDMA", 5))
    {
        mode_WCDMA = 1;
        mode_GSM = 0;
    }
    else
    {
        mode_GSM = 0;
        mode_WCDMA = 0;
    }
}


/* Helper function for UTF GetState() hook, cnmutf_get_state for CC, SS, SMS */
byte cnm_set_context(byte pd, byte connection_id)
{
    extern unsigned char cnmutf_pd;
    extern unsigned char cnmutf_connection_id;

    cnmutf_pd = pd;
        
    cnmutf_connection_id = connection_id;

    return connection_id;
}

#endif

