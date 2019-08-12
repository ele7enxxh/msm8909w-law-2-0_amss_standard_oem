/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2001, 2002 Qualcomm Technologies, Incorporated and its licensors.  All Rights 
Reserved.  QUALCOMM Proprietary.  Export of this technology or software 
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mn/vcs/mn_process_cnm_cc_msgs.c_v   1.24   10 Jul 2002 14:08:26   cdealy  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mn/src/mn_process_cnm_cc_msgs.c#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
3/20/01     CD     Replaced old header with QCOM header
                   Removed #ifdef ASW_SIMBA

3/26/2001   VT     Changed MN_handle_MNCC_SYNC_IND to handle/store the 
                   radio_access_bearer_id received in MNCC_SYNC_IND in 
                   case of UMTS. Will use  DEFAULT_RAB_ID till we implement
                   the Multicall feature (not Multicall SS)

3/26/2001   VT     In MN_process_MNCC_SETUP_CNF(), introduced call to 
                   MN_put_mmi_call_connected_ind() instead of 
                   MN_put_mmi_call_progress_info_ind(). Removed call to
                   MN_put_mmi_mt_data_connected_ind() if 
                   information_transfer_capability != 0.

5/3/01      CD     Corrected compile warnings C4244- conversion from int to byte

5/3/01      CD     Changed MN_handle_MNCC_ERR_IND() function definition
                   Initalized local variable 'cause' in MN_handle_MNCC_MODIFY_IND
                   to UNASSIGNED_CAUSE as defined in cause.h

5/3/01      CD     Initialized connection_id to 0xff in MN_handle_MNCC_REEST_IND
                   to avoid compile warning C4701. 

                   Changed MN_handle_MNCC_REEST_IND to avoid compile warning C4100

                   Fixed compile warnings C4244 - conversion from int to byte
                   in MN_handle_MNCC_PROGRESS_IND

5/9/01      CD     Removed unnecessary file header includes

5/22/01     CD     Deleted null_ptr from MN_handle_MNCC_MODIFY_CNF since it was not 
                   accessed

                   Removed #include for ms.h since it was not used

                   Deleted ptr from MN_handle_MNCC_REJ_IND since it was not used

                   Corrected initialization of null_connected_subaddress in 
                   MN_process_MNCC_CALL_HOLD_CNF

                   Corrected manipulation of message_position in 
                   MN_process_MNCC_CALL_RETRIEVE_REJ to avoid lint error

                   Deleted return_ptr from MN_process_MNCC_FACILITY_IND (case 
                   processUnstructuredSS_Data) since it was not accessed

                   Corrected calculation of invoke_id to avoid out-of bounds error

7/18/01     CD     Removed duplicate external declaration of Multicall_Status to mn_cc.h
                   Removed duplicate external declaration of mn_call_information
                   Removed duplicate external declaration of ss_invoke_data
                   Removed duplicate external declaration of tch_user_connections_store

                   Included mn_cnmtask.h and deleted external declaration of is_gsm_mode() and 
                   is_umts_mode()

8/10/01     CD    Removed unnecessary queue.h

08/14/01    AB    Renamed connection manager references to CNM from CM.

08/16/01    AB    Renamed MMI references to CM.

08/23/01    TS    Records Correct RAB_ID in MN_information database.

09/05/01    CD    Added QXDM logging to MN_process_MNCC_SETUP_CNF, MN_handle_MNCC_REL_CNF
                  and MN_handle_MNCC_REL_IND

11/02/01    AB    Added the CNM_clear_dtmf_queue() to clear out DTMF digit
                  in call transition to HOLD_ACK and DISCONNECT/RELEASE_COMPLETE.
                     
11/05/01    AB    Sent CM_MANAGE_CALLS_CNF to CM on HOLD_ACK if there is
                  MO_PENDING calls. This SS notification is needed for
                      CM to updated all pending call states.

11/09/01    AB    Sent CM_END_CALLS_IND to CM regardless of MT or MO calls in 
                  MN_handle_MNCC_REL_IND().
                    
11/09/01    AB    Fixed the check for HELD calls for 1_SEND_REL and
                  corrected the RETRIEVE held call in 
                      MN_handle_MNCC_REL_IND().

11/20/01    AB    In MN_handle_MNCC_REL_IND(), Remove duplicated send of 
                      CM_END_CALLS_IND for a NORMAL_CALL_REQ.

12/19/01    AB    Moved MN_process_MNCC_FACILITY_IND to mn_ss_cc_processing.c

01/14/02    AB    Don't Disconnect TCH if there are still pending call 
                  activities in MN_handle_MNCC_DISC_IND().

01/14/02    AB    Won't Wait for for HOLD_ACK to retrieve pending held call in
                  MN_process_MNCC_CALL_HOLD_CNF().

01/23/02    AB    In MN_handle_MNCC_DISC_IND(), added a MSG_HIGH if  
                  CM_PROGRESS_IND is present.

01/25/02    CD    Replaced IEI by boolean flag
                  Updated Copyright
                  Replaced calls to ERR() by MSG_ERROR()

01/29/02    AB    In MN_handle_MNCC_REL_IND(), updated the Multicall_STatus[] table
                  and related call connection state for CM_1_X_SEND_REL event, 
                  where the remaining connection ID in the MPTY calls may no 
                  longer qualify to be in MPTY state (only 1 call left).  
                  In addition, released the voice path, disconenct_TCH, if it 
                  was the last connection.

03/27/02    AB    Turn off locally generated ring back tone when received
                  inband signaling from the network via PROGRESS indicator 
                  IE either directly from PROGRESS message or piggy back in 
                  SETUP, CALL PROCEEDING, or DISCONNECT message.

05/26/02    AB    Do not increment the notification description in 
                  MN_handle_MNCC_NOTIFY_IND().

06/01/02    AB    Retrieves a HELD calls, if any, in MN_handle_MNCC_REL_IND(),
                  that is a results from the CM_1_X_SEND command initiated
                  by the user.
                     
06/03/02    CD    Merged in changes from revicion 1.18.1.0.

                  In MN_handle_MNCC_SYNC_IND(), if old tch_rate 
                  is equal to new tch_rate, added support for In-call 
          Modification from one supported speech version to 
          another.

          In all the calls to MN_disconnect_TCH(), now passing the
                     new parameter. 

06/07/02    CD    In MN_process_MNCC_SETUP_CNF() changed the order of function
                  calls so that the vocoder is connected first and then the 
                  Connect indication is sent to CM

                  In MN_handle_MNCC_CALL_PROC_IND() added check for 
                  OTA_channel_connected set to TRUE before sending a 
                  CM_CALL_PROGRESS_INFO_IND

                  In MN_handle_MNCC_SYNC_IND() 
                  Added call to send a CM_CALL_PROGRESS_INFO_IND
                  when inband information has been previously received 

                  Added comments

                  Changed code to remove unnecessary duplicate block

6/23/02      AB   In MN_handle_MNCC_DISC_IND(), remove redudant call
                  to MN_read_multicall_status() that referesh the
                  different call session.

                  In MN_handle_MNCC_REL_IND(), remove the special checking
                  of disconnection of 2 parties MP MPTY as this 
                  this functionality had now been moved to 
                  MN_read_multicall_status(). 
                    
                  In MN_process_MNCC_SETUP_CNF(), reset the mo_cal_pending 
                  flag so that MN_read_multicall_status() can produce
                  a more deterministic result, which could affect the
                  subsequence MPTY, CALL HOLD and CALL WAITING call session.
 
6/25/02      AB   In MN_handle_SYNC_IND(), print MSG_HIGH instead of ERROR
                  when recieved an invalid connection ID.

6/25/02      CD   Added void type cast to ignore return value of 
                  MN_get_MNCC_REL_IND

07/01/02     CD   Fixed lots of 'unusual pointer cast' lint errors.
                  
07/23/02     AB   In MN_handle_MNCC_SYNC_IND(), that is operation 
                  in GSM mode, do not enable the vocoder when the
                  assigned TCH has a non-speech channel mode 
                  configuration.

08/20/02     AB   In MN_process_MNCC_CALL_HOLD_CNF(), do not clear out
                  DTMF queue on HOLD_ACK.
                   
                  In MN_handle_MNCC_REL_IND() & MN_handle_MNCC_REL_CNF(), 
                  don't accept the call WAITING automatcially in 1_X_REL 
                  because it is not requierd by 1_X_SEND MMI specification 
                  and customer request for this exclusion.

09/04/02     AB   IN MN_handle_MNCC_DISC_IND(), for inband, do not connect 
                  TCH to vocoder if TCH already been connected.

09/16/02     AB   In MN_handle_MNCC_REL_IND(), deferred the RETRIEVE of
                  available call held session until RELEASE COMPLETE message
                  had been sent to meet the GSM Conformance requirement
                  section, 31.4.4.1.2.4.

                  In MN_handle_MNCC_REJ_IND(), clear out the connection ID
                  info stored in mn_call_information[], to allow another
                  MO call attempt of the same connection ID.

10/12/02     AB   Added Handover support between WCDMA & GSM in 
                  MN_handle_MNCC_SYNC_IND().

11/12/02     AB   In MN_handle_MNCC_REL_IND(), sent RELEASE COMPLETE before
                  processing other SS call related function such as 
                  accepting wating call etc.   Also clear call information,
                  mn_call_information[], for an MT call release, so that 
                  call session, Multicall_Status, can be properly updated. 
    
12/06/02    AB    Fixed merged error in MN_handle_DISC_IND().

12/17/02    AB    In MN_handle_MNCC_SYNC_IND(), enable very early assignment
                  to goes through, to fix TC26.8.1.3.5.4.

01/07/02    AB    In MN_handle_MNCC_SYNC_IND(), added supported for new SYNC 
                  reason received from MM including:
                      WCDMA_RAB_RELEASE, 
                      WCDMA_RAB_ETABLISHED, 
                      GSM_GSM_HANDOVER, 
                      GSM_RES_ASSIGNED 
                  which are needed to supports PS+CS services, GSM IDLE handover, 
                  and improves GSM to GSM Handover vocoder reconfiguration.

02/27/03    AB    In MN_handle_MNCC_SYNC_IND(), added supported for the vocoder
                  reconfiguration during the GSM to GSM Handover with new GSM 
                  Half Rate and GSM AMR speech mode.

04/16/03    AB    In MN_handle_MNCC_REJ_IND(), added processing of 
                  of the new ie_cm_cc_reject_T IE that received from
                  MM to supports silent redial feature.

05/22/03    AB    In MN_handle_MNCC_REJ_IND(), added checking of CC CC_STATE_U0
                  state before sending MO call confirmation response 
                  to CM/UI.

06/02/03    AB    Added AOC and CS handover notification registration 
                  function for DS and CM/UI.

01/05/04    AB    Removed any vocoder control on GSM HANDOVER in 
                  MN_configure_TCH().

                  Added FEATURE_CS_DATA_HO_NOTIFY to featurize the
                  CS DATA Handover Notification function.

5/04/02     AB    Fixed CR42462, channel modified in conversation state
                  cause audio mute on Agilent8960.

11/02/04    AB    Added SS Call related User-to-User Signaling feature.

03/22/05    AB    Added incoming call handling on active call clearing, 
                  MN_handle_MNCC_REL_IND(), that is part of the dual 
                  service selection feature.

04/28/05    HS    Added support for FEATURE_CCBS

06/14/05    HS    Variable initialization

08/18/05    HS    Make CM_1_SEND_REL actions only when all active calls are
                  released

08/31/05    HS    Ignore in-band info availability when call is not a speech call

09/02/05    HS    MN_is_speech_call() now assumes calls w/o BCIE's as speech call

11/03/05    HS    Prior progress indicator info cleared before processing DISCONNECT
                  message

07/18/06    NR    Stopping HOLD timer on receipt of disconnect message

09/20/06    NR    Disconnecting the AV call if wrong BCIE is received in MODIFY_REJECT/COMPLETE

10/04/06    NR    Copying RAB id during RAB reconfiguration for SCUDIF feature

11/10/06    NR    passing proper value of ITC while acquiring vocoder incase of SCUDIF modification

10/01/07    NR    Disconnecting the vocoder when dall call is cleared or there is waiting data call

11/01/07    NR    Enabling the vocoder incase of idle G-G handover

02/13/06    NR    Answering the incoming call for abnormal cases in case of 2_SEND request

03/05/06    NR    Adding changes for WB_AMR

03/21/07    NR    For tracking SS pending requests

04/24/07    NR    Rejecting the progress message for non voice calls

06/18/07    RD    Changes to include BCIE2 in case of repeat indicator 1

05/25/11    AM    Putting error range check for Multicall_Status_index

07/06/11    PM    Guarding against NULL pointer access

08/19/11    PM    Putting up a check to avoid crash.

11/02/11    PM    Changes made for a feature where Voc gets released when
                  call is put on HOLD and Voc gets acquired when call is retrieved.

11/28/11    PM    While processing RELEASE_COMPLETE in MN, MN sends END_CALL_IND to CM
                  just before freeing MN call objects to resolve some crash issues which
                  happens because of race between MN and CM to free same memory.
===========================================================================*/

#include "mmcp_variation.h"
#include "customer.h"
#include "rex.h"
#include "environ.h"
#include <string.h>
#include "cause.h"
#include "timers_v.h"
#include "mn.h"
#include "mnglobal.h"
#include "msg.h"
#include "err.h"
#include "mn_cnmtask.h"
#include "mn_cnmtask_int.h"
#include "naslog_v.h"
#include "cmlib.h"
#include "cnm.h"
#include "mm.h"
#include "com_iei_v.h"
#include "ds_cap_v.h" /* ITC_SPEECH */
#include "mm.h"
#include "geran_grr_api.h"
#include <stringl/stringl.h>


extern boolean is_cs_umts_mode( sys_modem_as_id_e_type as_id );
extern boolean is_cs_gsm_mode( sys_modem_as_id_e_type as_id );
#ifdef FEATURE_DUAL_SIM
#ifndef FEATURE_DUAL_ACTIVE
#define is_umts_mode() is_cs_umts_mode(mn_as_id)
#define is_gsm_mode()  is_cs_gsm_mode(mn_as_id)
#else
#define is_umts_mode() is_cs_umts_mode(mn_dsda_as_id)
#define is_gsm_mode()  is_cs_gsm_mode(mn_dsda_as_id)
#endif
#else
#define is_umts_mode()  is_cs_umts_mode(SYS_MODEM_AS_ID_1)
#define is_gsm_mode()   is_cs_gsm_mode(SYS_MODEM_AS_ID_1)
#endif

extern boolean   CNM_has_active_cc_connection(void);
extern cc_dtmf_state_e_type CNM_get_dtmf_state(connection_id_T connection_id);
VOID_FUNC    MN_handle_ALT_proc_upon_call_release(connection_id_T  connection_id
#if defined(FEATURE_DUAL_SIM )
, sys_modem_as_id_e_type as_id
#endif
                                                  );

extern boolean ghdi_is_acquisition_in_progress(void);

extern void ghdi_reset_mvs_callback_flag(void);

extern void MN_update_code_type( byte information_transfer_capability);

#ifdef FEATURE_ENHANCED_AMR
extern void    ghdi_reset_sampling_rate(void);
#endif

#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
extern boolean MN_turned_on_vocoder_sim[MAX_AS_IDS];
#define MN_turned_on_vocoder (MN_turned_on_vocoder_sim[mn_dsda_as_id])
#else
extern boolean MN_turned_on_vocoder;
#endif

#ifdef FEATURE_NAS_ECALL
extern VOID_FUNC MN_put_cm_handover_complete_ind(connection_id_T connection_id,sync_enum_T handover_cause);
#endif

/*
 * Function name :  MN_handle_MNCC_ALERT_IND
 * -------------
 *
 * Description :  Remote user has been alerted for a MO call setup
 * -----------     
 *
 *
 *
 *
 *
 *
 * Parameters: message_header
 * ----------  
 *
 *
 *
 * Returns:
 * --------
 *
 */

VOID_FUNC MN_handle_MNCC_ALERT_IND(IMH_T *message)
{
   MNCC_ALERT_IND_T      *message_ptr;

   message_ptr = (MNCC_ALERT_IND_T *)((void *)message);

   if(message_ptr->connection_id >= MAXNO_CC_TRANSACTIONS)
   {
      MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Connection id invalid for mn_call_information_p");
   }
   else
   {
      if(MN_check_for_inband_message(message_ptr->connection_id) == FALSE )  /* no inband signal */
      {
         MN_put_cm_call_progress_info_ind(message_ptr->connection_id, REMOTE_ALERT); /* local generated ring back */
      }
      MN_put_cm_call_alert_ind(message_ptr->connection_id);
   }
}


/*
 * Function name :  MN_handle_MNCC_CALL_PROC_IND
 * -------------
 *
 * Description :  CALL PROCEEDING event handling.  Assigns call data to 
 * -----------    relevant structures and confirms setup of call to CM. 
 *
 *
 *
 * Parameters: message_header
 * ----------  
 *
 *
 *
 * Returns: none
 * --------
 *
 */
VOID_FUNC MN_handle_MNCC_CALL_PROC_IND(IMH_T *message)
{

  /* Locals */

  MNCC_CALL_PROC_IND_T    *message_ptr;
  mn_call_information_T   *data_ptr;
  MNCC_CALL_PROC_data_T   *proc_data;
  MNCC_CALL_PROC_data_T   MNCC_CALL_PROC_data;
  connection_id_T         connection_id;
  ie_cm_cc_cause_T        cause = { 0};
#if defined(FEATURE_MULTIMEDIA_FALLBACK) || defined(FEATURE_REL6)
  cm_repeat_indicator_T       repeat_indicator, *repeat_indicator_ptr;
  cm_network_cc_capability_T  *network_cc_capability_ptr;
  cm_low_layer_compatibility_T   low_layer_compatibility;
  cm_high_layer_compatibility_T  high_layer_compatibility;   
#endif

  message_ptr = (MNCC_CALL_PROC_IND_T *)((void *)message);

  connection_id = message_ptr->connection_id;
  proc_data =&MNCC_CALL_PROC_data;

  if (connection_id >= MAXNO_CC_TRANSACTIONS)
  {
    MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Connection id invalid for mn_call_information_p");
  }
  else
  {
    /*  GET THE OPTIONAL DATA FROM THE INPUT MESSAGE  */
    if (mn_call_information_p[connection_id] != NULL)
    {

      MN_get_MNCC_CALL_PROC_IND(message_ptr, proc_data);

      /*  THE NETWORK MAY HAVE MODIFIED THE BEARER CAPABILITIES AS PART OF
          THE NEGOTIATION PROCEDURE  */
      /*  INDEX THE CALL DATA  */

      data_ptr =  mn_call_information_p[connection_id];

#if defined(FEATURE_MULTIMEDIA_FALLBACK) || defined(FEATURE_REL6)
      if ((data_ptr->bearer_capability_1.present == TRUE) &&
          (proc_data->bearer_capability_1.present == TRUE) &&
          (data_ptr->bearer_capability_1.information_transfer_capability !=
           proc_data->bearer_capability_1.information_transfer_capability) &&
          ((data_ptr->repeat_indicator_bc == REPEAT_SERVICE_CHANGE_FALLBACK) ||
           (data_ptr->repeat_indicator_bc == REPEAT_SUPPORTS_FALLBACK)))
      {
        /* swapping the LLC and HLC as BCIE has been swapped in case of SCUDIF*/
        low_layer_compatibility = data_ptr->low_layer_compatibility_1;
        data_ptr->low_layer_compatibility_1 = data_ptr->low_layer_compatibility_2;
        data_ptr->low_layer_compatibility_2 = low_layer_compatibility;

        high_layer_compatibility = data_ptr->high_layer_compatibility_1;
        data_ptr->high_layer_compatibility_1 = data_ptr->high_layer_compatibility_2;
        data_ptr->high_layer_compatibility_2 = high_layer_compatibility;           
      }
#endif         
      if (proc_data->repeat_indication_bc > 0)
      {
        data_ptr->bearer_capability_1 = proc_data->bearer_capability_1;
      }
      else
      {
        data_ptr->repeat_indicator_bc = proc_data->repeat_indication_bc;
      }

      if (proc_data->repeat_indication_bc > 1)
      {
        data_ptr->bearer_capability_1 = proc_data->bearer_capability_1;

        data_ptr->bearer_capability_2 = proc_data->bearer_capability_2;
      }
      else
      {
        if (proc_data->bearer_capability_2.present)
        {
          data_ptr->bearer_capability_2.present = proc_data->bearer_capability_2.present;
          data_ptr->bearer_capability_2 = proc_data->bearer_capability_2;
        }
        else
        {
          data_ptr->bearer_capability_2.present = FALSE;
        }
      }

#if defined(FEATURE_MULTIMEDIA_FALLBACK) || defined(FEATURE_REL6)
      /* Repeat Indicator */
      if (proc_data->repeat_indication_bc > 1)
      {
        repeat_indicator.present = TRUE;
        repeat_indicator.repeat_indication = proc_data->repeat_indication_bc;

        repeat_indicator_ptr = &repeat_indicator;
      }
      else
      {
        repeat_indicator_ptr = NULL;
      }

      /* Network Capability */
      if (proc_data->network_cc_capability.present == TRUE)
      {
        network_cc_capability_ptr  = &proc_data->network_cc_capability;
      }
      else
      {
        network_cc_capability_ptr  = NULL;
      }
#endif /* FEATURE_MULTIMEDIA_FALLBACK */

      /*  CM ASSIGNS CALL NUMBER TO CONNECTION ID  */

      MN_put_cm_mo_call_conf(
                            connection_id,                   /* connection_id */
                            TRUE,                            /* cnm_call_success */
                            data_ptr->bearer_capability_1,   /* bearer_capability_1 */
                            data_ptr->bearer_capability_2,   /* bearer_capability_2 */
                            cause,
#if defined(FEATURE_DUAL_SIM )
                            mn_call_information_p[connection_id]->as_id,
#endif
#if defined(FEATURE_MULTIMEDIA_FALLBACK) || defined(FEATURE_REL6)
                            NULL,
                            repeat_indicator_ptr,
                            network_cc_capability_ptr
#else
                            NULL
#endif
                            );                          /* a dummy cause */

      /* handle PROGRESS indicator received from SETUP because the inband progress 
       * indication received from CALL PROCEEDING or PROGRESS message will generate 
       * MNCC_PROGRESS_IND to MN, MN will map the DEFAULT_RABID = 0 connection and 
       * turn on Vocoder.  */

      if ((MN_check_for_inband_message(message_ptr->connection_id) == TRUE)  &&  /* in band = network alert */
        (tch_user_connections_store[DEFAULT_RAB_ID].OTA_channel_connected == TRUE))
      {
        MN_put_cm_call_progress_info_ind(message_ptr->connection_id, USER_RESUMED);  /* turn off local ring back tone */
      }
    /* else wait for ALERT to determine when to play local generated ring back tone */
    }
    else
    {
      MSG_HIGH_DS_1(MN_SUB,"=MNCNM= cannot process MNCC_CALL_PROC_IND as mn_call_information_p[%d] is NULL",
               connection_id);
    }
  }
}

/*
 * Function name :  MN_is_speech_call
 * -------------
 *
 * Description :  find out if a call is a speech call
 * -----------     
 *
 * Parameters: call_id - connection id
 * ----------  
 *
 * Returns: TRUE if it is speech call, FALSE if not
 * --------
 *
 * Remarks: Assumes call_id is for valid call
 * --------
 */
static boolean MN_is_speech_call (byte call_id)
{
  mn_call_information_T *mn_info;
  byte                   bc1_itc, bc2_itc;
  boolean                ret_val=FALSE;

  if(mn_call_information_p[call_id] == NULL)
  {
    MSG_HIGH_DS_1(MN_SUB,"=MNCNM= mn_call_information_p[%d] is NULL",call_id);
    return FALSE;
  }
  
  mn_info =  mn_call_information_p[call_id];

  bc1_itc = mn_info->bearer_capability_1.information_transfer_capability;
  bc2_itc = mn_info->bearer_capability_2.information_transfer_capability;

  /* if there is no BCIE1, assume speech
   * no need to check BCIE2 since BCIE2 can't exist w/o BCIE1 */
  if ( !mn_info->bearer_capability_1.present )
  {
    return TRUE;
  }

  if (mn_info->active_bearer_capability == 0)
  {
    if ( bc1_itc == ITC_SPEECH
#ifdef FEATURE_ALS
         || bc1_itc == ITC_AUX_SPEECH
#endif
       )
    {
      ret_val = TRUE;
    }
  }
  else
  {
    if ( bc2_itc == ITC_SPEECH
#ifdef FEATURE_ALS
         || bc2_itc == ITC_AUX_SPEECH
#endif
       )
    {
      ret_val = TRUE;
    }
  }

  return ret_val;
}

/*
 * Function name :  MN_check_if_progress_indicator_is_8
 * -------------
 *
 * Description :   Checks if the progress indicator received by the NW is #8 or not.
 * -----------     
 *
 *
 *
 *
 * Parameters: message_header, connection Id.
 * ----------  
 *
 *
 *
 * Returns: boolean
 * --------
 *
 */
boolean MN_check_if_progress_indicator_is_8(connection_id_T connection_id)
{
    
/* Locals */
    
   mn_call_information_T   *mn_info_ptr;
   byte                    progress_description;
    
   mn_info_ptr = mn_call_information_p[connection_id];
    
   if(mn_info_ptr != NULL)
   {
     progress_description = mn_info_ptr->progress_indicator.progress_description;
    
     /* Progress description value are extracted from TS24.008, 5.4.4.1.2.1 */
     if (( mn_info_ptr->progress_indicator_present == TRUE) &&
          (progress_description == 8))
          
     {
        return (TRUE);
     }
     else
     {
        return (FALSE);
     }
   }
   else
   {
     MSG_HIGH_DS_1(MN_SUB,"=MNCNM= Cannot check if progress indicator is 8,as mn_call_information_p[%d] is NULL",
                              connection_id);
     return (FALSE);
   }
}


/*
 * Function name :  MN_handle_MNCC_DISC_IND
 * -------------
 *
 * Description :  Process network originated call cleardown
 * -----------     
 *
 *
 *
 *
 *
 *
 * Parameters: message_header
 * ----------  
 *
 *
 *
 * Returns:
 * --------
 *
 */
VOID_FUNC MN_handle_MNCC_DISC_IND(IMH_T *message_header)
{
   MNCC_DISC_IND_T *message_ptr;
   boolean active_bc_speech;
   ie_cm_cc_cause_T cm_cc_cause = { 0 };
   ie_cm_cc_cause_T cm_null_cause = { 0 };
   cm_progress_indicator_T cm_prog_ind = { 0 };
#ifdef FEATURE_CCBS
   ie_cm_allowed_actions_T   allowed_actions = {0,0};
#endif /* FEATURE_CCBS */
   mn_call_information_T *data_ptr;
   boolean result;
   byte Multicall_Status_index;
   
   /*  Derferencing the MESSAGE to get cause value */
   message_ptr = (MNCC_DISC_IND_T *)((void *)message_header);

   /* parse the CAUSE and PROGRESS indication IE */
#ifdef FEATURE_CCBS
   MN_get_MNCC_DISC_IND(message_ptr, &cm_cc_cause, &cm_prog_ind, &allowed_actions);
#else
   MN_get_MNCC_DISC_IND(message_ptr, &cm_cc_cause, &cm_prog_ind);
#endif /* FEATURE_CCBS */

   /* Reset the call session */
   MN_MULTICALL_LOG(MN_read_multicall_status());
  
   MN_stop_cc_timer(message_ptr->connection_id);

  if(mn_call_information_p[message_ptr->connection_id] != NULL)
  {
     /*  FIND THE ACTIVE BEARER CAPABILITY  */
     data_ptr =  mn_call_information_p[message_ptr->connection_id];
  
     active_bc_speech = MN_is_speech_call(message_ptr->connection_id);
  
     /* store the cause of the DISC_IND */
     data_ptr->cm_cc_cause = cm_cc_cause;
  
     /*  STORE THE PROGRESS INDICATOR  */
     if(cm_prog_ind.present == TRUE)
     {
        data_ptr->progress_indicator_present = TRUE;
        data_ptr->progress_indicator.progress_description = cm_prog_ind.progress_description;
  
        MSG_HIGH_DS_0(MN_SUB,"=MNCNM= Progress Indicator received in MNCC_DISC_IND from CNM");
     }
     else
     {
        data_ptr->progress_indicator_present = FALSE;
     }
  
  #ifdef FEATURE_CCBS
     /* store CCBS allowed info */
     if (allowed_actions.present && allowed_actions.ccbs_possible)
        data_ptr->ccbs_allowed = TRUE;
     else
        data_ptr->ccbs_allowed = FALSE;
  #endif /* FEATURE_CCBS */
  
     /* For both GSM & UMTS, Use DEFAULT_RAB_ID below. Will change when we 
      * implement the Multicall Feature (Not the Multicall SS)  */
     if((MN_check_if_progress_indicator_is_8(message_ptr->connection_id) == TRUE) &&
        (tch_user_connections_store[DEFAULT_RAB_ID].OTA_channel_connected == TRUE) &&
        (active_bc_speech == TRUE))
     {
        MN_connect_TCH(0);
  
        /* Tell CM so as to STOP any locally generated tones */
        MN_put_cm_call_progress_info_ind(message_ptr->connection_id, USER_RESUMED);

        /* When Disconnect contain progress indicator, send Disconnect with cause and inband tone as TRUE */
        MN_put_cm_mt_disc_ind(message_ptr->connection_id, &cm_cc_cause, TRUE);

          
        #ifdef FEATURE_CCBS
        if (allowed_actions.present)
        {
           MN_put_cm_ccbs_info_ind (message_ptr->connection_id, allowed_actions.ccbs_possible);
        }
        #endif /* FEATURE_CCBS */
     }
     else
     {  
        #ifdef FEATURE_CCBS
        if (allowed_actions.present)
        {
           MN_put_cm_ccbs_info_ind (message_ptr->connection_id, allowed_actions.ccbs_possible);
        }
  
        if (!(allowed_actions.present && allowed_actions.ccbs_possible))
        {
        #endif /* FEATURE_CCBS */
  
           /* have the network release the call */
           #ifdef FEATURE_CCBS
           MN_put_MNCC_REL_REQ(message_ptr->connection_id, &cm_cc_cause, &cm_null_cause, 1, NULL);   /* number of causes */
           #else
           MN_put_MNCC_REL_REQ(message_ptr->connection_id, &cm_cc_cause, &cm_null_cause, 1);   /* number of causes */
           #endif /* FEATURE_CCBS */
  
           /* tell cm to START generate the tones */
           MN_put_cm_mt_disc_ind(message_ptr->connection_id, &cm_cc_cause, FALSE);
        #ifdef FEATURE_CCBS
        }
        #endif /* FEATURE_CCBS */
  
        result = MN_call_over(message_ptr->connection_id); /* this call session is over */
  
        if(result == TRUE)
        {
           /*  IF NO MORE CALLS ACTIVE  */
  
           Multicall_Status_index = MN_whereami(message_ptr->connection_id);
  
           if(Multicall_Status_index >= MULTICALL_STATUS_SIZE)
           {
             MSG_ERROR_DS_1(MN_SUB,"=MNCNM= Invalid connection %d ", message_ptr->connection_id);
           }
           else
           {
                 /*  GET THE OTHER CALL that may still utilize the TCH */
                 Multicall_Status_index = (byte)((Multicall_Status_index + 1) % 2);
  
  
                 if((Multicall_Status[Multicall_Status_index].call_activity == CALL_INACTIVE) &&
                     (Multicall_Status[MN_CALL_WAITING_SESSION].call_activity == CALL_WAITING))
                 {
                    if(mn_call_information_p[message_ptr->connection_id]->active_bearer_capability == 0)
                    {
                       if(((mn_call_information_p[message_ptr->connection_id]->bearer_capability_1.information_transfer_capability != ITC_SPEECH) &&
                           (mn_call_information_p[message_ptr->connection_id]->bearer_capability_1.information_transfer_capability != ITC_AUX_SPEECH)) ||
                           ((mn_call_information_p[Multicall_Status[MN_CALL_WAITING_SESSION].connection_id[0]]->bearer_capability_1.information_transfer_capability != ITC_SPEECH) &&
                           (mn_call_information_p[Multicall_Status[MN_CALL_WAITING_SESSION].connection_id[0]]->bearer_capability_1.information_transfer_capability != ITC_AUX_SPEECH)))
                       {
#ifdef FEATURE_DUAL_SIM                       
                          if(MNCNM_IS_SINGLE_STACK)
                          {
                            MN_disconnect_TCH_with_AS_ID(MN_VOCODER_OFF,mn_call_information_p[message_ptr->connection_id]->as_id);
                          }
                          else
#endif
                          {
                            MN_disconnect_TCH(MN_VOCODER_OFF);
                          }

                          MSG_HIGH_DS_0(MN_SUB,"=MNCNM= Disconnecting vocoder due to waiting call or call which is disconnected being data call");
                       }
                    }
                    else
                    {
                       if(((mn_call_information_p[message_ptr->connection_id]->bearer_capability_2.information_transfer_capability != ITC_SPEECH) &&
                           (mn_call_information_p[message_ptr->connection_id]->bearer_capability_2.information_transfer_capability != ITC_AUX_SPEECH)) ||
                           ((mn_call_information_p[Multicall_Status[MN_CALL_WAITING_SESSION].connection_id[0]]->bearer_capability_1.information_transfer_capability != ITC_SPEECH) &&
                           (mn_call_information_p[Multicall_Status[MN_CALL_WAITING_SESSION].connection_id[0]]->bearer_capability_1.information_transfer_capability != ITC_AUX_SPEECH)))
                       {
#ifdef FEATURE_DUAL_SIM                       
                          if(MNCNM_IS_SINGLE_STACK)
                          {
                            MN_disconnect_TCH_with_AS_ID(MN_VOCODER_OFF,mn_call_information_p[message_ptr->connection_id]->as_id);
                          }
                          else
#endif
                          {
                            MN_disconnect_TCH(MN_VOCODER_OFF);                      
                          }
                          MSG_HIGH_DS_0(MN_SUB,"=MNCNM= Disconnecting vocoder due to waiting call or call which is disconnected being data call");
                       }                     
                    }                 
                 }
                 else
                 {
                     MSG_HIGH_DS_2(MN_SUB,"=MNCNM= AMR Voc 'ON' for other call session  %d ", Multicall_Status_index, 
                                                                 message_ptr->connection_id);
                 }   
           }

        }
        else
        {
          MSG_HIGH_DS_1(MN_SUB,"=MNCNM= AMR Voc 'ON 'for other MPTY %d ", message_ptr->connection_id);
        }
     }
  }
  else
  {
    MSG_HIGH_DS_1(MN_SUB,"=MNCNM= Cannot process MNCC_DISC_IND as mn_call_information_p[%d] is NULL", 
                         message_ptr->connection_id);
  }
}


/*
 * Function name : MN_handle_MNCC_ERR_IND 
 * -------------
 *
 * Description :  MM believes connection to be lost
 * -----------     
 *
 *
 *
 *
 *
 *
 * Parameters: message_header
 * ----------  
 *
 *
 *
 * Returns:
 * --------
 *
 */
VOID_FUNC MN_handle_MNCC_ERR_IND(
#if defined(FEATURE_DUAL_SIM )
    IMH_T *message
#endif
  )
{
   /*  MM BELIEVES THE CONNECTION TO BE LOST  */
#ifdef FEATURE_DUAL_SIM     
   if(MNCNM_IS_SINGLE_STACK)
   {
     MN_disconnect_TCH_with_AS_ID(MN_VOCODER_OFF,((MNCC_ERR_IND_T*)((void *)message))->as_id);
   }
   else
#endif
   {
     MN_disconnect_TCH(MN_VOCODER_OFF);
   }

   /* Inform cm about the lack of connection */

   MN_put_cm_connection_state_ind(
#if defined(FEATURE_DUAL_SIM )
        ((MNCC_ERR_IND_T*)((void *)message))->as_id,
#endif
         FALSE);
}

#if defined(FEATURE_MULTIMEDIA_FALLBACK) || defined(FEATURE_REL6)

/*
 * Function name :  MN_handle_MNCC_MODIFY_CNF
 * -------------
 *
 * Description :  Process modify message
 * -----------     
 *
 *
 *
 *
 *
 *
 * Parameters: message_header
 * ----------  
 *
 *
 *
 * Returns:
 * --------
 *
 */
VOID_FUNC MN_handle_MNCC_MODIFY_CNF(IMH_T *message)
{
   MNCC_MODIFY_CNF_T          *message_ptr;
   MNCC_MODIFY_data_T         modify_data;
   mn_call_information_T      *call_data_ptr;
   cm_modify_complete_data_T  cm_modify_data;
   boolean                    accept;

   message_ptr = (MNCC_MODIFY_CNF_T *)((void *)message);

   if((message_ptr != NULL) && 
       (message_ptr->connection_id < MAXNO_CC_TRANSACTIONS))
   {
     if(mn_call_information_p[message_ptr->connection_id] != NULL)
     {
       accept = FALSE;

       call_data_ptr =  mn_call_information_p[message_ptr->connection_id];

       /* Initialized cm_modify_data */

       memset(&cm_modify_data,'\0',sizeof(cm_modify_complete_data_T));

       if(message_ptr->data[0] == IEI_BEARER_CAPABILITY)  /* != CAUSE == ACCEPTED */
       {

           /* adjusted for the initialized BCIE */
           call_data_ptr->active_bearer_capability = ((call_data_ptr->active_bearer_capability == 0) ? TRUE : FALSE);

           MN_get_MNCC_MODIFY_CNF(message_ptr, &modify_data);

           /* restore for initialized BCIE */
           call_data_ptr->active_bearer_capability = ((call_data_ptr->active_bearer_capability == 0) ? TRUE : FALSE);;
       }
       else
       {
           /* cause = REJECT, does not have BCIE */
           MN_get_MNCC_MODIFY_CNF(message_ptr, &modify_data);
       }

       if(modify_data.cause_present == FALSE)
       {
          /*  CHECK THE BEARER CAPABILITY  */

          if(call_data_ptr->active_bearer_capability == 0)
          {
             if(MN_compare_bearer_capability(
                 &(call_data_ptr->bearer_capability_2), 
                 &(modify_data.cm_bearer_capability), MT) == TRUE )
             {
                call_data_ptr->active_bearer_capability = 1;
                accept = TRUE;
             }
          }
          else
          {
             if(MN_compare_bearer_capability(
                 &(call_data_ptr->bearer_capability_1),       
                 &(modify_data.cm_bearer_capability), MT) == TRUE )
             {
                call_data_ptr->active_bearer_capability = 0;
                accept = TRUE;
             }
          }

          if((accept == TRUE) && 
             (modify_data.immediate_modify_ind == TRUE)) 
          {
              MSG_ERROR_DS_1(MN_SUB,"=MNCNM= In-Call Modify Accepted, TRAFFIC CHANNEL re-used = %d", modify_data.immediate_modify_ind);
          }
       }
       else
       /* Network rejected modify */
       {
          if(call_data_ptr->active_bearer_capability == 0)
          {
             if(MN_compare_bearer_capability(
                 &(call_data_ptr->bearer_capability_1), 
                 &(modify_data.cm_bearer_capability), MT) == TRUE )
             {
                accept = TRUE;
             }
          }
          else
          {
             if(MN_compare_bearer_capability(
                 &(call_data_ptr->bearer_capability_2),       
                 &(modify_data.cm_bearer_capability), MT) == TRUE )
             {
                accept = TRUE;
             }
          }

       }

       /* Set up the new data for the cm */

       if (accept == TRUE)
       {
          cm_modify_data.connection_id = modify_data.connection_id;
          cm_modify_data.cm_cc_cause = modify_data.cm_cc_cause;
          cm_modify_data.cm_bearer_capability = modify_data.cm_bearer_capability;
          cm_modify_data.cm_low_layer_compatibility = 
              modify_data.cm_low_layer_compatibility;
          cm_modify_data.cm_high_layer_compatibility = 
              modify_data.cm_high_layer_compatibility;

          if(modify_data.cm_low_layer_compatibility.present == TRUE)
          {
             cm_modify_data.low_layer_compatibility_present = TRUE;
          }
          else
          {
             cm_modify_data.low_layer_compatibility_present = FALSE;
          }

          if(modify_data.cm_high_layer_compatibility.present == TRUE)
          {
             cm_modify_data.high_layer_compatibility_present = TRUE;
          }
          else
          {
             cm_modify_data.high_layer_compatibility_present = FALSE;
          }

          cm_modify_data.reverse_call_direction = 
              modify_data.reverse_call_direction;

          if(modify_data.cause_present == FALSE)

          {
             cm_modify_data.complete = TRUE;
          }
          else
          {
             cm_modify_data.complete = FALSE;
          }
       }
       else /* Send the old data */
       {
          cm_modify_data.connection_id = modify_data.connection_id;
          cm_modify_data.cm_cc_cause.present = TRUE;
          cm_modify_data.cm_cc_cause.coding_standard  = STD_CODESTANDARD;
          cm_modify_data.cm_cc_cause.location = STD_LOCATION;
          cm_modify_data.cm_cc_cause.recommendation_present = FALSE;
          cm_modify_data.cm_cc_cause.cause_value = 
              BEARER_CAPABILITY_NOT_PRESENTLY_AVAILABLE;
          cm_modify_data.cm_cc_cause.diagnostic_length = 0;

          if(call_data_ptr->active_bearer_capability == 0)
          {
             cm_modify_data.cm_bearer_capability = 
                 call_data_ptr->bearer_capability_1;
          }
          else
          {
             cm_modify_data.cm_bearer_capability = 
                 call_data_ptr->bearer_capability_2;
          }

          cm_modify_data.low_layer_compatibility_present = FALSE;

          cm_modify_data.high_layer_compatibility_present = FALSE;

          cm_modify_data.reverse_call_direction = FALSE;

          cm_modify_data.complete = FALSE;
       }

#if defined(FEATURE_DUAL_SIM )
       if(MNCNM_IS_SINGLE_STACK)
       {
         cm_modify_data.as_id = mn_call_information_p[cm_modify_data.connection_id]->as_id;
       }
#endif
       /* Upon T324 timer expiry send a reject to CM but do not reply to CNM*/
       if(RECOVERY_ON_TIMER_EXPIRY == modify_data.cm_cc_cause.cause_value)
       {
          cm_modify_data.cm_cc_cause = modify_data.cm_cc_cause;
          MN_put_cm_mo_modify_complete_conf(&cm_modify_data);
       }
       else
       {
          MN_put_cm_mo_modify_complete_conf(&cm_modify_data);
  

          MN_put_MNCC_MODIFY_ACCEPT(modify_data.connection_id,accept);  /* update CC state */
       
          if(accept == FALSE)
          {
             ie_cm_cc_cause_T  cm_cc_cause;       
             MSG_HIGH_DS_1(MN_SUB,"=MNCNM= Aborting the call wtih TI = %d due to invalide BC in modify_complete/modify_reject", message_ptr->connection_id);
             cm_cc_cause.location = STD_LOCATION;
             cm_cc_cause.coding_standard = STD_CODESTANDARD;
             cm_cc_cause.recommendation_present = FALSE;
             cm_cc_cause.cause_value = NORMAL_CALL_CLEARING; 
             cm_cc_cause.diagnostic_length = 0;                
          
             MN_stop_cc_timer(message_ptr->connection_id);
             
             MN_put_MNCC_DISC_REQ(message_ptr->connection_id, &(cm_cc_cause), NULL); 
             mn_call_information_p[message_ptr->connection_id]->mo_call_clear_pending = TRUE;         
          }  
       }
     } 
     else
     {
       MSG_HIGH_DS_1(MN_SUB,"=MNCNM= Cannot process MNCC_MODIFY_CNF as mn_call_information_p[%d] is NULL", 
                          message_ptr->connection_id);
     }
    }
    else
    {
       MSG_ERROR_DS_1(MN_SUB,"=MNCNM= Invalid msg = %d, connection_id = %d", message_ptr);
    }
}


/*
 * Function name :  MN_handle_MNCC_MODIFY_IND
 * -------------
 *
 * Description :  Handle modify indication message from MMI
 * -----------     
 *
 *
 *
 *
 *
 *
 * Parameters: message_header
 * ----------  
 *
 *
 *
 * Returns:
 * --------
 *
 */
VOID_FUNC MN_handle_MNCC_MODIFY_IND(IMH_T *message)
{

   MNCC_MODIFY_IND_T *message_ptr;
   connection_id_T connection_id;
   MNCC_MODIFY_data_T modify_data;
   boolean reject;
   byte cause;
   byte active_capability;
   mn_call_information_T *data_ptr;

   /* Initialise data */
   memset(&modify_data,'\0',sizeof(MNCC_MODIFY_data_T));
   message_ptr = (MNCC_MODIFY_IND_T *)((void *)message);

   MN_get_MNCC_MODIFY_IND(message_ptr, &modify_data);

   connection_id = message_ptr->connection_id;

   cause = UNASSIGNED_CAUSE;

   if(connection_id >= MAXNO_CC_TRANSACTIONS)
   {
      MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Connection id invalid for mn_call_information_p");
   }
   else if(mn_call_information_p[connection_id] == NULL)
   {
      MSG_HIGH_DS_1(MN_SUB,"=MNCNM= cannot process MNCC_MODIFY_CNF as mn_call_information_p[%d] is NULL",
                    connection_id);
   }
   else
   {
      reject = FALSE;

      data_ptr =  mn_call_information_p[connection_id];

      active_capability = data_ptr->active_bearer_capability;

      /*  
       *  ONLY ONE BEARER CAPABILITY HAS BEEN NEGOTIATED, SO MODIFICATIONS 
       *  ALLOWED, BC MUST EQUAL THE STATUS QUO  
       */

      if(data_ptr->repeat_indicator_bc == 1)
      {
         if(MN_compare_bearer_capability(&(modify_data.cm_bearer_capability),
          &(data_ptr->bearer_capability_1), MT) == FALSE)
         {
            reject = TRUE;
            cause = BEARER_CAPABILITY_NOT_AUTHORISED;
         }
      }
      else
      {
         /*  
          * TWO BEARER CAPABILITIES, BUT REPEAT INDICATION = SEQUENTIAL, 
          * AND ONE MODIFY
          * HAS ALREADY BEEN DONE  
          */

         if(data_ptr->repeat_indicator_bc == REPEAT_SEQUENTIAL)
         {
            if(data_ptr->no_of_changes > 0)
            {
               reject = TRUE;
               cause = BEARER_CAPABILITY_NOT_AUTHORISED;
            }
         }
         else
         {
            /*  
             * TWO BEARER CAPABILITIES, AND THE REQUESTED ONE IS NOT THE 
             * SAME AS IN THE MODIFY MESSAGE  
             */

            if( (data_ptr->repeat_indicator_bc == REPEAT_SEQUENTIAL) ||
                (data_ptr->repeat_indicator_bc == 0x02) ||
                (data_ptr->repeat_indicator_bc == 0x04) )
            {
               if(active_capability == 0)
               {
                  if(MN_compare_bearer_capability(&(modify_data.cm_bearer_capability),
                   &(data_ptr->bearer_capability_2),MT) == FALSE)
                  {
                     reject = TRUE;
                  }
               }

               if(active_capability == 1)
               {
                  if(MN_compare_bearer_capability(&(modify_data.cm_bearer_capability),
                   &(data_ptr->bearer_capability_1), MT) == FALSE)
                  {
                     reject = TRUE;
                  }
               }

               if(reject == TRUE)
               {
                  cause = BEARER_CAPABILITY_NOT_PRESENTLY_AVAILABLE;
               }
            }
         }
      }

      modify_data.connection_id = connection_id;


      if(((reject == TRUE) &&
          (active_capability == 0)) ||
          ((reject == FALSE ) &&
          ( active_capability == 1)))


      {
         modify_data.cm_bearer_capability = data_ptr->bearer_capability_1;
         if( (data_ptr->repeat_indication_llc == 0) ||
             (data_ptr->low_layer_compatibility_1.present == FALSE) )
         {
            modify_data.repeat_indication_llc = 0;
         }

         else
         {
            modify_data.repeat_indication_llc = 1;
            modify_data.cm_low_layer_compatibility = 
                data_ptr->low_layer_compatibility_1;
         }

         if( (data_ptr->repeat_indication_hlc == 0) ||
             (data_ptr->high_layer_compatibility_1.present == FALSE) )
         {
            modify_data.repeat_indication_hlc = 0;
         }

         else
         {
            modify_data.repeat_indication_hlc = 1;
            modify_data.cm_high_layer_compatibility = 
                data_ptr->high_layer_compatibility_1;
         }
      }
      else
      {
         modify_data.cm_bearer_capability = data_ptr->bearer_capability_2;
         if( (data_ptr->repeat_indication_llc < 2) ||
             (data_ptr->low_layer_compatibility_2.present == FALSE) )
         {
            modify_data.repeat_indication_llc = 0;
         }

         else
         {
            modify_data.repeat_indication_llc = 1;
            modify_data.cm_low_layer_compatibility = 
                data_ptr->low_layer_compatibility_2;
         }

         if( (data_ptr->repeat_indication_hlc < 2) ||
             (data_ptr->high_layer_compatibility_2.present == FALSE) )
         {
            modify_data.repeat_indication_hlc = 0;
         }
         else
         {
            modify_data.repeat_indication_hlc = 1;
            modify_data.cm_high_layer_compatibility = 
                data_ptr->high_layer_compatibility_2;
         }
      }

      if(reject == TRUE)
      {
         modify_data.cause_present = TRUE;
         modify_data.cm_cc_cause.coding_standard = STD_CODESTANDARD;
         modify_data.cm_cc_cause.location = STD_LOCATION;
         modify_data.cm_cc_cause.cause_value = cause;
         modify_data.cm_cc_cause.recommendation_present = FALSE;
         modify_data.cm_cc_cause.diagnostic_length = 0;
         modify_data.reverse_call_direction = FALSE;

         /* inform CNM that the change has been rejected by MN */
         MN_put_MNCC_MODIFY_RES(connection_id, &modify_data);
      }
      else
      {
         /* Inform CM that a change ind from the network has been received */
         data_ptr->reverse_call_direction = modify_data.reverse_call_direction;

         data_ptr->immediate_modify_ind = modify_data.immediate_modify_ind;

         MN_put_cm_mt_modify_ind(connection_id, &modify_data);
      }
   }
}
#endif /* FEATURE_MULTIMEDIA_FALLBACK */




/*
 * Function name :  MN_handle_MNCC_NOTIFY_IND
 * -------------
 *
 * Description :Pass the notification indicator received during an active
 * -----------  call session (traffic) to CM/UI to configure the EU 
 *              speech path according to given notification description.  
 *
 *              Some possible action includes:
 *                0x0 = User Suspend: Disconnect the vocoder speech path 
 *                                 from earpiece. 
 *                0x1 = User Resume: Set the vocoder speech path to the 
 *                                 earpiece.
 *                0x2 = Bearer Change: Same action as 0.
 * 
 * Parameters: 
 * ----------- 
 *   message_pointer - (MNCC_NOTIFY_IND_T *) parsed NOTIFY message pointer.
 *
 * Returns:
 * --------
 *
 */
VOID_DATA MN_handle_MNCC_NOTIFY_IND(IMH_T *message)
{
   MNCC_NOTIFY_IND_T *message_ptr;
   char   notification;

   message_ptr = (MNCC_NOTIFY_IND_T *)((void *)message);

   notification = message_ptr->notification_indicator.notification_description;

   /* only use lower 4 bits */
   notification &= 0x0f;

   MN_put_cm_call_progress_info_ind(message_ptr->connection_id,
                                    notification);
}


/*
 * Function name :  MN_handle_MNCC_PROGRESS_IND
 * -------------
 *
 * Description : Handles progress information sent back 
 * -----------   from CM  
 *
 *
 *
 *
 *
 *
 * Parameters: message_header
 * ----------  
 *
 *
 *
 * Returns:
 * --------
 *
 */
VOID_DATA MN_handle_MNCC_PROGRESS_IND(IMH_T *message)
{
   /* Locals */
   MNCC_PROGRESS_IND_T *message_ptr;
   mn_call_information_T *data_ptr;
   connection_id_T connection_id;

   message_ptr = (MNCC_PROGRESS_IND_T *)((void *)message);

   if(message_ptr->connection_id >= MAXNO_CC_TRANSACTIONS)
   {
      MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Connection id invalid for mn_call_information_p");
   }
   else if(mn_call_information_p[message_ptr->connection_id] == NULL)
   {
      MSG_HIGH_DS_1(MN_SUB,"=MNCNM= Cannot process MNCC_PROGRESS_IND as mn_call_information_p[%d] is NULL",
                   message_ptr->connection_id);
   }
   else
   {
      if((message_ptr->progress_indicator.CodstdLocation & 0x60) == 
          (STANDARD_GSM))
      {
         /*  LOAD THE CODE  */
         boolean  is_speech_call;
         

         connection_id = message_ptr->connection_id;
         data_ptr =  mn_call_information_p[connection_id];

         is_speech_call = MN_is_speech_call (connection_id);
         

         if ( is_speech_call )
         {
           data_ptr->progress_indicator_present = TRUE;
  
           /* Corrected compilation warnings C4244 - conversion from int to byte */
           data_ptr->progress_indicator.coding_standard  =       
               (byte)((message_ptr->progress_indicator.CodstdLocation >>5)&0x03);
           data_ptr->progress_indicator.location = 
               (byte)(message_ptr->progress_indicator.CodstdLocation & 0x0f);
           data_ptr->progress_indicator.progress_description =    
               (byte)(message_ptr->progress_indicator.ProgressDescription & 0x7f);
           
           /* For normal mo calls, inband info can come in MO transction ids but For srvcc calls inband info can come with MT transactions. */
           if (connection_id < MAXNO_CC_TRANSACTIONS)
           {
              /* For both GSM & UMTS, Use DEFAULT_RAB_ID below. */
              /* Will change when we implement the Multicall    */
              /* Feature (Not the Multicall SS)                 */
  
              if ((tch_user_connections_store[DEFAULT_RAB_ID].OTA_channel_connected == TRUE) &&
                  (MN_check_for_inband_message(connection_id) == TRUE) )
              {
   
                 MN_connect_TCH( data_ptr->bearer_capability_1.information_transfer_capability);
  
                 /* Stop locally generated ring back tone */
                 MN_put_cm_call_progress_info_ind(message_ptr->connection_id, USER_RESUMED);  /* in band = network alert */
              }
              
           }
         }
         else /* case when inband info avail rcvd in non-speech call */
         {
           /* NW sent inband info for non-speech call -> ignore : TS34.123-2 10.1.2.4.5B */
           MSG_HIGH_DS_0(MN_SUB,"=MNCNM= progress indicator dropped"); 
         }
      }
   }
}


/*
 * Function name :  MN_handle_MNCC_REEST_IND
 * -------------
 *
 * Description :  This temporarily interrupted call has been reestablished
 * -----------     
 *
 *
 *
 *
 *
 *
 * Parameters: message_header
 * ----------  
 *
 *
 *
 * Returns:
 * --------
 *
 */
VOID_DATA MN_handle_MNCC_REEST_IND(void)
{
   boolean call_active;
   connection_id_T connection_id;
   mn_call_information_T  *call_data_ptr;

   MN_MULTICALL_LOG(MN_read_multicall_status());

   if(Multicall_Status[0].call_activity == CALL_ACTIVE)
   {
      call_active = TRUE;
      connection_id = (Multicall_Status[0].connection_id[0]);
   }

   else if(Multicall_Status[1].call_activity == CALL_ACTIVE)
   {
      call_active = TRUE;
      connection_id = (Multicall_Status[1].connection_id[0]);
   }
   else
   {
      /*  NO ACTIVE CALLS - DO NOTHING  */

      call_active = FALSE;
      connection_id = 0xff; /* to avoid compile warning C4701 */
   }
      
   if(call_active == TRUE)
   {

      if(connection_id >= MAXNO_CC_TRANSACTIONS)
      {
         MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Connection id invalid for mn_call_information_p");
      }
      else 
      {
          /*  TURN ON APPROPRIATE TYPE OF TCH  */

         call_data_ptr =  mn_call_information_p[connection_id];

         if (call_data_ptr->active_bearer_capability == 0)
         {
            MN_connect_TCH(
                call_data_ptr->bearer_capability_1.
                information_transfer_capability);
         }
         else
         {
            MN_connect_TCH(
                call_data_ptr->bearer_capability_2.
                information_transfer_capability);
         }
         
          /* Inform cm that the connection is resumed */

         MN_put_cm_connection_state_ind(
#if defined(FEATURE_DUAL_SIM )
           call_data_ptr->as_id,
#endif
           TRUE);
      }
   }
}


/*
 * Function name :  MN_handle_MNCC_REJ_IND
 * -------------
 *
 * Description :  Indication that call is to be rejected
 * -----------     
 *
 *
 *
 *
 *
 *
 * Parameters: message_header
 * ----------  
 *
 *
 *
 * Returns:
 * --------
 *
 */
VOID_FUNC MN_handle_MNCC_REJ_IND(IMH_T *message)
{
   MNCC_REJ_IND_T *message_ptr;
   ie_cm_cc_cause_T cm_cc_cause;
   ie_cm_cc_reject_T cm_cc_reject;
   cm_bearer_capability_T  null_bearer_capability;
   boolean ss_request_pending = FALSE;
   ie_cm_ss_error_T   dummy_ss_error =  { 0        };
#if defined(FEATURE_DUAL_SIM )
   sys_modem_as_id_e_type    as_id = SYS_MODEM_AS_ID_1;
#endif
   boolean                      log_packet_sent = FALSE;
   uint8                        release_cause;

   memset(&cm_cc_reject, NULL, sizeof(ie_cm_cc_reject_T));

   message_ptr = (MNCC_REJ_IND_T *)((void *)message);
#if defined(FEATURE_DUAL_SIM )
   if((MNCNM_IS_SINGLE_STACK) && (mn_call_information_p[message_ptr->connection_id] != NULL))
   {
     as_id = mn_call_information_p[message_ptr->connection_id]->as_id;
   }
#endif
   /* Zero null bearer capability data to send to CM */
   memset((VOID_DATA *)(&null_bearer_capability), NULL, sizeof(cm_bearer_capability_T));

   /* Retrieve cause value */
   MN_get_MNCC_REJ_IND(message_ptr, &cm_cc_cause);

   /* Reject cause value */
   cm_cc_reject = message_ptr->rej_cause;

   if((mn_call_information_p[message_ptr->connection_id] != NULL) &&
      (mn_call_information_p[message_ptr->connection_id]->ss_request_pending != CM_NONE_PENDING))
   {
      ss_request_pending = TRUE;
   }      
  
   /* Clear out call information for this connection id */
   MN_clear_call_data(message_ptr->connection_id);

   /* Clear out any remaining DTMF digit */
   CNM_clear_dtmf_queue(message_ptr->connection_id);

   /* update the ALT proc status */
   MN_handle_ALT_proc_upon_call_release(message_ptr->connection_id
#if defined(FEATURE_DUAL_SIM )
                               ,as_id
#endif    
         );

   /* Refresh call session if no alternate procedure is going on */
   if(mn_alternate_proc_status.is_alternate_procedure != TRUE)
   {
      MN_MULTICALL_LOG(MN_read_multicall_status());
   }

   /* Disconnect TCH from Vocoder */
   if ((Multicall_Status[0].call_activity == CALL_INACTIVE)&&
       (Multicall_Status[1].call_activity == CALL_INACTIVE)&&
       (Multicall_Status[2].call_activity == CALL_INACTIVE))
   {
      /*  NO MORE CALLS - DISCONNECT THE TRAFFIC CHANNEL Based on the As-id */
#ifdef FEATURE_DUAL_SIM
      if(MNCNM_IS_SINGLE_STACK)
      {
        if(is_cs_gsm_mode(as_id))
        {
           MN_disconnect_TCH_with_AS_ID(MN_VOCODER_GSM_OFF, as_id);
        }
        else
        {
           MN_disconnect_TCH_with_AS_ID(MN_VOCODER_WCDMA_OFF, as_id);
        }
      }
      else
#endif
      {
        MN_disconnect_TCH(MN_VOCODER_SWITCH);
      }

      /* For both GSM & UMTS, Use DEFAULT_RAB_ID below. Will change when we 
       * implement the Multicall Feature (Not the Multicall SS)  */
      tch_user_connections_store[DEFAULT_RAB_ID].OTA_channel_connected = FALSE;
   }
 
   if((cc_transaction_information_p[message_ptr->connection_id] != NULL) && 
       (cc_transaction_information_p[message_ptr->connection_id]->cc_state != CC_STATE_U0) && 
      (cc_transaction_information_p[message_ptr->connection_id]->cc_state < CC_STATE_U3))
   {
       /*  Returns CAUSE DATA (CM_CC_CAUSE) */
       MN_put_cm_mo_call_conf(message_ptr->connection_id,
                          FALSE, 
                          null_bearer_capability,
                          null_bearer_capability,
                          cm_cc_cause,
#if defined(FEATURE_DUAL_SIM )
                          as_id, 
#endif 
#ifndef  FEATURE_MULTIMEDIA_FALLBACK
                           &cm_cc_reject
#else
                           #error code not present
#endif /* FEATURE_MULTIMEDIA_FALLBACK */
                          );
   }

   /*
    *  Logging 0x713D log packet to Diag
    */
    
   if((mn_call_information_p[message_ptr->connection_id] != NULL) &&
    (mn_call_information_p[message_ptr->connection_id]->call_reject_cause_diag_packet.present == TRUE))
   {
     release_cause = mn_call_information_p[message_ptr->connection_id]->call_reject_cause_diag_packet.cause_value;
   }
   else
   {
     release_cause = CALL_CLEARING_CAUSE_REJECT;
   }
   
   log_packet_sent = mn_log_cs_call_release_info(message_ptr->connection_id, release_cause);

   if (log_packet_sent == FALSE)
   {
       MSG_LOW_DS_0(MN_SUB,"=MNCNM= Cannot send log packet to DIAG");
   }

#ifdef FEATURE_NAS_CALL_END_CAUSE
   cm_cc_cause.present = TRUE;
   cm_cc_cause.cause_value = release_cause;
#endif /* FEATURE_NAS_CALL_END_CAUSE */

   /* End the call on reject */
   MN_put_cm_mt_end_call_ind(
                             message_ptr->connection_id, 
                             cm_cc_cause, 
#if defined(FEATURE_DUAL_SIM )
                             as_id, 
#endif    
                             &cm_cc_reject
                             );

#ifdef FEATURE_ENHANCED_AMR   
   /* Reset eAMR sampling rate at call establishment if no other ACTIVE call exists */

   if((Multicall_Status[0].call_activity == CALL_INACTIVE) &&
      (Multicall_Status[1].call_activity == CALL_INACTIVE))
   {
     ghdi_reset_sampling_rate();
   }
#endif

   /* Refresh the call session and store the RAB status if no Alternate procedure is going on */
   if(mn_alternate_proc_status.is_alternate_procedure != TRUE)
   {
#ifdef FEATURE_WCDMA
      mn_store_rab_status();
#endif
   } 

   /* send end_call_ind if any MO call is pending when RLF happens :  Case where 2nd MO Call triggered the HOLD REQ for the 1st Active Call
           And RLF happens */
   if(Multicall_Status[0].call_activity == MO_CALL_PENDING)
   {
     connection_id_T  temp_connection_id; /* temp_connection_id is the one for which if any mo call is pending */

     temp_connection_id = Multicall_Status[0].connection_id[0]; /* get the connection id */
     
     if((temp_connection_id < MAXNO_CC_TRANSACTIONS) &&
        (mn_call_information_p[temp_connection_id] != NULL))
     {
       MSG_HIGH_DS_1(MN_SUB,"=MNCNM= clear the pending MO call = %d" ,temp_connection_id);

       MN_put_cm_mt_end_call_ind(temp_connection_id, 
                                 cm_cc_cause, 
#if defined(FEATURE_DUAL_SIM )
                                 as_id, 
#endif    
                                 &cm_cc_reject
                               );
       MN_clear_call_data(temp_connection_id); /* clear call objects */
     }
   }

   if(ss_request_pending == TRUE)
   {
     /* for sending response to ATCOP requests */
         
     dummy_ss_error.present = TRUE;
     dummy_ss_error.error_code_tag = MN_SS_ERROR_TAG;
     dummy_ss_error.error_code = MN_NETWORK_NOT_RESPONDING;

     MN_put_cm_manage_calls_conf(FALSE, 
#if defined(FEATURE_DUAL_SIM )
                             as_id, 
#endif    
                             dummy_ss_error
                             );  /* Successful release */  
   }     
   /* Enabling DXT blindly to consider the case where call is rejected by MM*/
#ifdef FEATURE_GSM
#ifdef FEATURE_DUAL_SIM
   if(MNCNM_IS_SINGLE_STACK)
   {
     geran_grr_dtx_override_off_per_subs(mn_as_id);
   }
#ifdef FEATURE_DUAL_ACTIVE
   else
   {
     geran_grr_dtx_override_off_per_subs((sys_modem_as_id_e_type)mn_dsda_as_id);
   }
#endif
#else
   geran_grr_dtx_override_off();
#endif
#endif
}


/*
 * Function name :  MN_handle_MNCC_REL_CNF
 * -------------
 *
 * Description :  Process network response to a MO MNCC_REL_REQ which
 * -----------     was itself a response to a MNCC_DISC_IND
 *
 *
 *
 *
 *
 *
 * Parameters: message_header
 * ----------  
 *
 *
 *
 * Returns:
 * --------
 *
 */
VOID_FUNC MN_handle_MNCC_REL_CNF(IMH_T *message)
{
   MNCC_REL_CNF_T               *message_ptr;
   mn_call_information_T        *data_ptr;
#ifdef FEATURE_NAS_VOC_REL_AFTER_CALL_HOLD  
   mn_call_information_T        *cw_data_ptr;
#endif
   ie_cm_cc_cause_T             cm_cc_cause;
   uint8                        release_cause;   
   connection_id_T              connection_id, cleared_connection_id; /*cleared_connection_id is the id whose call object gets cleared*/
   ie_cm_ss_error_T             cm_ss_error =  { 0 };
   boolean                      log_packet_sent = FALSE;
   boolean                      waitingcall_tobe_answered = FALSE;
   byte                         mt_accept_cause;
   boolean                      retrieve_call = FALSE, retrieve_session = 0;
   boolean                      hold_manage_calls_conf_pending = FALSE;
   cm_connected_subaddress_T    null_subaddress = { 0   };
   byte                         Multicall_Status_index;
#if defined(FEATURE_DUAL_SIM )
   sys_modem_as_id_e_type       as_id = SYS_MODEM_AS_ID_NONE;
#endif
   /*  PROCESS NETWORK RESPONSE TO A MO MNCC_REL_REQ WHICH WAS ITSELF
                  A RESPONSE TO A MT_MNCC_DISC_IND  */

   memset(&cm_cc_cause, NULL, sizeof(ie_cm_cc_cause_T));

   message_ptr = (MNCC_REL_CNF_T *)((void *)message);
#if defined(FEATURE_DUAL_SIM )
   if(MNCNM_IS_SINGLE_STACK)
   {
     if (mn_call_information_p[message_ptr->connection_id] != NULL)
     {
       as_id = mn_call_information_p[message_ptr->connection_id]->as_id;
     }
     else if(mn_as_id != SYS_MODEM_AS_ID_NONE)
     {
       as_id = mn_as_id;
     }
     else
     {
       MSG_ERROR_DS_1(MN_SUB,"=MNCNM= Rxed MNCC_REL_CNF on invalid mn_as_id = %d",mn_as_id);
     }
   }
#endif
   if(message_ptr->cause)
   {
       MN_get_MNCC_REL_CNF(message_ptr, &cm_cc_cause);
   }

   connection_id = message_ptr->connection_id;
   if(mn_call_information_p[connection_id] != NULL)
   {

     data_ptr =  mn_call_information_p[connection_id];
  
     MN_stop_cc_timer(connection_id);

     /*
      * Filling the cause to be sent to DIAG in the log packet.
      */

     if((data_ptr->mo_release_cause_diag_packet.present == TRUE) &&
         (data_ptr->mo_release_cause_diag_packet.cause_value == RECOVERY_ON_TIMER_EXPIRY))
     {
       /*
        * First fill the second cause in mn_call_information_p, i.e.
        * if RELEASE was sent by UE with second cause,
        * use that cause and ignore REL_COMP's cause.
        */
        
       release_cause = data_ptr->mo_release_cause_diag_packet.cause_value;
     }
     else if(data_ptr->cm_cc_cause.present == TRUE)
     {
       /*
        * Else fill the cause in DISCONNECT message, i.e.
        * if DISCONNECT is the first message used by NW/UE for call clearing,
        * use DISCONNECT's cause and ignore REL_COMP's cause.
        */
        
       release_cause = data_ptr->cm_cc_cause.cause_value;
     }
     else if(data_ptr->mo_release_cause_diag_packet.present == TRUE)
     {
       /*
        * Else fill the first cause in mn_call_information_p, i.e.
        * if RELEASE was sent by UE with only one cause (or second cause was not 102),
        * use that cause and ignore REL_COMP's cause.
        */
        
       release_cause = data_ptr->mo_release_cause_diag_packet.cause_value;
     }
     else if(cm_cc_cause.present == TRUE)
     {
       /*
        * If DISCONNECT was not sent either by NW or UE, use the cause in RELEASE_COMPLETE
        * if present.
        */
       release_cause = cm_cc_cause.cause_value;
     }
     else
     {
       MSG_HIGH_DS_0(MN_SUB,"=MNCNM=  Cause is not present in either RELEASE_COMPLETE or DISCONNECT, Filling default cause");
       release_cause = DEFAULT_CALL_CLEARING_CAUSE_REL_COMP;
     }
     

     if(cm_cc_cause.present != TRUE )
     {
        /* no cause in the input message, so use the stored cause, if any */
        cm_cc_cause = data_ptr->cm_cc_cause;
     }
  
     /* Send log packet to DIAG */
     log_packet_sent = mn_log_cs_call_release_info(connection_id, release_cause);
  
     if (log_packet_sent == FALSE)
     {
         MSG_LOW_DS_0(MN_SUB,"=MNCNM= Cannot send log packet to DIAG");
     }
  
     /* CC State Transistion */
     if(!MN_manage_CC_auxiliary_state(message_ptr->connection_id, CCE_CALL_ENDED))
     {
       MSG_HIGH_DS_2(MN_SUB,"=MNCNM= Update AUX state failed", message_ptr->connection_id, CCE_CALL_ENDED);
     }
  
     if(data_ptr->hold_request == CM_2_SEND_HOLD)
     {
         /* Locate call session */
         Multicall_Status_index = MN_whichMultiCallIndex(connection_id);
         
         /* waiting call to be answered if call is not multiparty
           or this is the last call in the muliparty group*/
         if(Multicall_Status_index >= MULTICALL_STATUS_SIZE)
         {
           MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Index Out of bounds and buffer overflow");
         }
         else if((FALSE == Multicall_Status[Multicall_Status_index].call_is_mpty)||
           ((TRUE == Multicall_Status[Multicall_Status_index].call_is_mpty) && 
                 (1 == Multicall_Status[Multicall_Status_index].id_index)))
         {   
           waitingcall_tobe_answered = TRUE;
           mt_accept_cause = CM_2_SEND_REQ;
         }
     }
     /* If MT call is answered as a result of hold request or 1 send*/
     if(CM_2_SEND_REQ == data_ptr->mt_call_accept_cause 
#ifdef FEATURE_NAS_1SEND_ANDROID
                  || CM_1_SEND_REQ == data_ptr->mt_call_accept_cause
#endif
         )
     { 
        hold_manage_calls_conf_pending = TRUE;
     }

#ifdef FEATURE_NAS_CALL_END_CAUSE
   cm_cc_cause.present = TRUE;
   cm_cc_cause.cause_value = release_cause;
#endif /* FEATURE_NAS_CALL_END_CAUSE */

  
     if(TRUE == hold_manage_calls_conf_pending)
     {
        /* Reply to CM for the hold request */
        MN_put_cm_manage_calls_conf(TRUE, 
#if defined(FEATURE_DUAL_SIM )
                               as_id, 
#endif    
                               cm_ss_error
                              ); 
     }

     MN_handle_ALT_proc_upon_call_release(connection_id
#if defined(FEATURE_DUAL_SIM )
                               ,as_id
#endif    
         );
  
     /* refresh call session */
     MN_MULTICALL_LOG(MN_read_multicall_status());
  
     /* Disconnect TCH from Vocoder */
     if ((Multicall_Status[0].call_activity == CALL_INACTIVE)&&
         (Multicall_Status[1].call_activity == CALL_INACTIVE)&&
         (Multicall_Status[2].call_activity == CALL_INACTIVE))
  
     {

        /*  NO MORE CALLS - DISCONNECT THE TRAFFIC CHANNEL  */
#ifdef FEATURE_DUAL_SIM     
        if(MNCNM_IS_SINGLE_STACK)
        {
          MN_disconnect_TCH_with_AS_ID(MN_VOCODER_SWITCH,as_id);
        }
        else
#endif
        {
          MN_disconnect_TCH(MN_VOCODER_SWITCH);
        }

  
        /* For both GSM & UMTS, Use DEFAULT_RAB_ID below. Will change when we 
         * implement the Multicall Feature */
        tch_user_connections_store[DEFAULT_RAB_ID].OTA_channel_connected = FALSE;
     }
  

     /*
      * Check if any waiting or held call is to be retrieved in response to the user press of CM_1_SEND button.
      * Note that WAITING call has higher priority
      */


     /*
      * First Check that WAITING call is not being retrieved due to user press of CM_2_SEND button.
      * Then check call was disconnected because 1_Send was pressed.
      */

     if((waitingcall_tobe_answered == FALSE) && (data_ptr->mo_release_indicator == CM_1_SEND_REL))
     {

       /*
       * Call to MN_MULTICALL_LOG(MN_read_multicall_status()) is important before going forward in this block.
       * Here, this call is not made in this block, because that call has been made earlier in this function, unconditionally.
       */
       if ( MN_num_of_calls_in_group(CALL_ACTIVE) == 0 )
       {

         if (Multicall_Status[MN_HOLD_ACTIVE_SESSION].call_activity == CALL_INACTIVE)
         {
           /* 
           * There is the possibility of getting a Waiting call or 
           * retrieving a Held call according to the rules of GSM 02.30 
           * section 4.5.5.1 
           *
           * Note: The Waiting call has priority.
           */
           if (((Multicall_Status[MN_ACTIVE_HOLD_SESSION].call_activity == CALL_HELD) ||
           (Multicall_Status[MN_ACTIVE_HOLD_SESSION].call_activity == CALL_INACTIVE)) &&
           (Multicall_Status[MN_CALL_WAITING_SESSION].call_activity == CALL_WAITING))
           {
   
             waitingcall_tobe_answered = TRUE;
             mt_accept_cause = CM_1_SEND_REQ;
  
           }
           else if ( (Multicall_Status[MN_ACTIVE_HOLD_SESSION].call_activity == CALL_HELD) && 
                     (Multicall_Status[MN_CALL_WAITING_SESSION].call_activity == CALL_INACTIVE) )
           {
             /*  SEE THAT THE OTHER CALL IS A HELD CALL  */
             retrieve_call =  TRUE;
             retrieve_session = MN_ACTIVE_HOLD_SESSION;
           }
           else
           {
             retrieve_call =  FALSE;
           }
         } 
         else  /* MN_ACTIVE_HOLD_SESSION */
         {
           /*  SEE THAT THE OTHER CALL IS A HELD CALL  */
           if (((Multicall_Status[MN_HOLD_ACTIVE_SESSION].call_activity == CALL_HELD) ||
                (Multicall_Status[MN_HOLD_ACTIVE_SESSION].call_activity == CALL_INACTIVE)) &&
                (Multicall_Status[MN_CALL_WAITING_SESSION].call_activity == CALL_WAITING))
           {
             waitingcall_tobe_answered = TRUE;
             mt_accept_cause = CM_1_SEND_REQ;
           }
           else if ((Multicall_Status[MN_HOLD_ACTIVE_SESSION].call_activity == CALL_HELD) &&
                    (Multicall_Status[MN_ACTIVE_HOLD_SESSION].call_activity == CALL_INACTIVE) &&
                    (Multicall_Status[MN_CALL_WAITING_SESSION].call_activity == CALL_INACTIVE))
           {
             retrieve_call = TRUE;
             retrieve_session = MN_HOLD_ACTIVE_SESSION;
           }
           else
           {
             retrieve_call = FALSE;
           }
         }
       }
     }

     /* Inform the CM */
     MN_put_cm_mt_end_call_ind(connection_id, 
                             cm_cc_cause, 
#if defined(FEATURE_DUAL_SIM )
                             as_id, 
#endif  
                             NULL);

#ifdef FEATURE_ENHANCED_AMR     
   /* Reset eAMR sampling rate at call establishment if no other ACTIVE call exists */

   if((Multicall_Status[0].call_activity == CALL_INACTIVE) &&
      (Multicall_Status[1].call_activity == CALL_INACTIVE))
   {
     ghdi_reset_sampling_rate();
   }
#endif

#ifdef FEATURE_WCDMA
     mn_store_rab_status();
#endif

     /* Reset all information relating the this state */
     MN_clear_call_data(connection_id);
     cleared_connection_id = connection_id;

     /* only for abnormal case handling of 2_SEND where DISCONNECT may be
             received in response to HOLD_REQ */  
     if((Multicall_Status[MN_CALL_WAITING_SESSION].call_activity == CALL_WAITING) && (waitingcall_tobe_answered == TRUE))
     {
        if(Multicall_Status[MN_CALL_WAITING_SESSION].connection_id[0] < MAXNO_CC_TRANSACTIONS 
           && mn_call_information_p[Multicall_Status[MN_CALL_WAITING_SESSION].connection_id[0]] != NULL)
        {

          mn_call_information_p[Multicall_Status[MN_CALL_WAITING_SESSION].connection_id[0]]->
                                                                             mt_call_accept_cause = mt_accept_cause;
  
          /* CONNECT */
          MN_put_MNCC_SETUP_RES(Multicall_Status[MN_CALL_WAITING_SESSION].connection_id[0],
                                                &null_subaddress);  
          MSG_HIGH_DS_0(MN_SUB,"=MNCNM= Answering the incoming call due to DISCONNECT received in response to 2_SEND");  
#ifdef FEATURE_NAS_VOC_REL_AFTER_CALL_HOLD  
          cw_data_ptr =  mn_call_information_p[Multicall_Status[MN_CALL_WAITING_SESSION].connection_id[0]];

          /*  IF 1st BC IS FOR SPEECH, CONNECT SPEECH NOW  */
          if(
#ifdef FEATURE_ALS
             (cw_data_ptr->bearer_capability_1.information_transfer_capability == ITC_AUX_SPEECH) ||
#endif
             (cw_data_ptr->bearer_capability_1.information_transfer_capability == ITC_SPEECH)
            )
          {
             MN_connect_TCH(cw_data_ptr->bearer_capability_1.information_transfer_capability);
          }
#endif
        }
        else
        {
           MSG_ERROR_DS_1(MN_SUB,"=MNCNM=  Multicall status array reflects wrong information for waiting call with connection id = %d ",Multicall_Status[MN_CALL_WAITING_SESSION].connection_id[0]);
        }
     }
     else if (Multicall_Status[0].call_activity == MO_CALL_PENDING)
     {
      /* resume the mo call setup:-> Case where 2nd MO Call triggered the HOLD REQ for the 1st Active Call
           And the 1st Active Call was released from the NW side, */
        connection_id = Multicall_Status[0].connection_id[0];
        if((connection_id != cleared_connection_id) && 
           (connection_id < MAXNO_CC_TRANSACTIONS) &&
           (mn_call_information_p[connection_id] != NULL))
        {
          /* reset the mo call pending flag */
          mn_call_information_p[connection_id]->mo_setup_pending = FALSE;
        
          /* Override the use of UL DTX between call setup and connect/connect_ack */
#ifdef FEATURE_GSM
#ifdef FEATURE_DUAL_SIM
          if(MNCNM_IS_SINGLE_STACK)
          {
            geran_grr_dtx_override_on_per_subs(mn_as_id);
          }
#ifdef FEATURE_DUAL_ACTIVE
          else
          {
             geran_grr_dtx_override_on_per_subs((sys_modem_as_id_e_type)mn_dsda_as_id);
          }
#endif
#else
          geran_grr_dtx_override_on();
#endif
#endif
          /* normal MO calls */
          MN_put_MNCC_SETUP_REQ(connection_id);
        }
     }

     /* Retrieve call session of 1 SEND REL */
     if(retrieve_call == TRUE) 
     {
         MN_retrieve_call(retrieve_session);
     }
     
   }     
   else
   {
     MSG_HIGH_DS_1(MN_SUB,"=MNCNM= cannot process MNCC_REL_CNF as mn_call_information_p[%d] is NULL", connection_id);    
   }
}


/*
 * Function name :  MN_handle_MNCC_REL_IND
 * -------------
 *
 * Description :  Processes the network response to a MO_DISC_REQ
 * -----------     message - this can be generated by 1-send, etc
 *
 *
 *
 * Parameters: message_header
 * ----------  
 *
 *
 *
 * Returns:
 * --------
 *
 */

VOID_FUNC MN_handle_MNCC_REL_IND(IMH_T *message)
{
   MNCC_REL_IND_T                      *message_ptr;
   mn_call_information_T               *data_ptr;
#ifdef FEATURE_NAS_VOC_REL_AFTER_CALL_HOLD  
   mn_call_information_T               *cw_data_ptr;
#endif
   connection_id_T                     connection_id = 0;
   uint8                               release_cause;   
   ie_cm_cc_cause_T                    cm_cc_cause_1 =  { 0        };
   ie_cm_cc_cause_T                    cm_cc_cause_2 =  { 0        };
   ie_cm_ss_error_T                    dummy_ss_error =  { 0        };
   cm_connected_subaddress_T           null_subaddress = { 0        };
   boolean                             log_packet_sent = FALSE;
   byte                                retrieve_call = FALSE, retrieve_session = 0;
#if defined(FEATURE_DUAL_SIM )
   sys_modem_as_id_e_type              as_id = SYS_MODEM_AS_ID_1;
#endif
   /* Dereferencing the message */
   message_ptr = (MNCC_REL_IND_T *)((void *)message);

   connection_id = message_ptr->connection_id;
#if defined(FEATURE_DUAL_SIM )
   if(MNCNM_IS_SINGLE_STACK)
   {
     if (mn_call_information_p[connection_id] != NULL)
     {
       as_id = mn_call_information_p[connection_id]->as_id;
     }
     else if(mn_as_id != SYS_MODEM_AS_ID_NONE)
     {
       as_id = mn_as_id;
     }
   }
#endif
   if(message_ptr->connection_id <  MAXNO_CC_TRANSACTIONS)
   {
     if(mn_call_information_p[connection_id] != NULL)
     {
        /*
         * PROCESSES THE NETWORK RESPONSE TO A MO CALL RELEASE or a 
         * NETWORK RELEASE  
         */
        data_ptr =  mn_call_information_p[connection_id];

        MN_stop_cc_timer(connection_id);
  
        /* ignore return value of MN_get_MNCC_REL_IND  for now */
        /* since only 1 cause value is passed to CM */
        (void)MN_get_MNCC_REL_IND(message_ptr, &cm_cc_cause_1, &cm_cc_cause_2);
  
        /*
         * Following if-else block decides on the cause which needs to be sent to DIAG
         * in 0x713D log packet.
         */
        if((data_ptr->mo_release_cause_diag_packet.present == TRUE) && (data_ptr->mo_release_cause_diag_packet.cause_value == NORMAL_UNSPECIFIED)) 
        {
          /*
           *   Fill the second cause sent by NW in RELEASE message, if it is present and value is 102.
           */

          release_cause = data_ptr->mo_release_cause_diag_packet.cause_value;
        }
        else if((cm_cc_cause_2.present == TRUE) && (cm_cc_cause_2.cause_value == RECOVERY_ON_TIMER_EXPIRY))
        {
          /*
           *   Fill the second cause sent by NW in RELEASE message, if it is present and value is 102.
           */

          release_cause = cm_cc_cause_2.cause_value;
        }
        else if(data_ptr->cm_cc_cause.present == TRUE)
        {
          /*
           *   If RELEASE message from NW does not has a second cause, Fill the cause which was sent
           *   by UE in DISCONNECT message. This cause was stored in mn_call_information_p
           *   global variable.
           */
          release_cause = data_ptr->cm_cc_cause.cause_value;
        }
        else if(cm_cc_cause_1.present == TRUE)
        {
          /*
           *   If RELEASE is the first call clearing message received by UE and it contains only one cause, 
           *   send it in 0x713D log packet.
           */
          release_cause = cm_cc_cause_1.cause_value;
        }
        else
        {
          MSG_HIGH_DS_0(MN_SUB,"=MNCNM=  Cause is not present in either RELEASE or DISCONNECT, Filling Default cause");
          release_cause = DEFAULT_CALL_CLEARING_CAUSE_VALUE;
        }

        /* Send log packet to DIAG */
        log_packet_sent = mn_log_cs_call_release_info(connection_id, release_cause);
  
        if (log_packet_sent == FALSE)
        {
             MSG_LOW_DS_0(MN_SUB,"=MNCNM= Cannot send log packet to DIAG");
        }
  
        /* Inhibit the RELEASE_COMPLETE if POWER OFF or OFFLINE */
        if(mn_call_information_p[connection_id]->offline != TRUE)
        {
            /* Send RELEASE COMPLETE */
            MN_put_MNCC_REL_COMPL_REQ(connection_id, PD_CC, NULL, NULL);
        }
  
#ifdef FEATURE_NAS_CALL_END_CAUSE
        cm_cc_cause_1.present = TRUE;
        cm_cc_cause_1.cause_value = release_cause;
#endif /* FEATURE_NAS_CALL_END_CAUSE */


        /* Tell CM if the associated connection ID had been released by the network */
        MN_put_cm_mt_end_call_ind(connection_id, cm_cc_cause_1, 
#if defined(FEATURE_DUAL_SIM )
                               as_id,
#endif
                                   NULL);

#ifdef FEATURE_ENHANCED_AMR
        /* Reset eAMR sampling rate at call establishment if no other ACTIVE call exists */

        if((Multicall_Status[0].call_activity == CALL_INACTIVE) &&
           (Multicall_Status[1].call_activity == CALL_INACTIVE))
        {
          ghdi_reset_sampling_rate();
        }
#endif

        /* Clear out any remaining DTMF digit */
        MN_handle_ALT_proc_upon_call_release(connection_id
#if defined(FEATURE_DUAL_SIM )
                               ,as_id
#endif
                                            );

#ifdef FEATURE_WCDMA
		mn_store_rab_status();
#endif

        /* Refresh Multicall Session */
        MN_MULTICALL_LOG(MN_read_multicall_status());
  
        /* Check if this is a MT call release */
        if (data_ptr->mo_call_clear_pending == FALSE)
        {
           /* If MT call is answered as a result of hold request or 1 send */  
           if(CM_2_SEND_REQ == data_ptr->mt_call_accept_cause 
  #ifdef FEATURE_NAS_1SEND_ANDROID
                        || CM_1_SEND_REQ == data_ptr->mt_call_accept_cause
  #endif
              )
           {                
               MN_put_cm_manage_calls_conf(TRUE, 
#if defined(FEATURE_DUAL_SIM )
                                         as_id, 
#endif    
                                         dummy_ss_error
                                         ); 

           }
           /* only for abnormal case handling of 2_SEND where DISCONNECT may be
              received in response to HOLD_REQ */  
           if((Multicall_Status[MN_CALL_WAITING_SESSION].call_activity == CALL_WAITING) && 
               (mn_call_information_p[connection_id]->hold_request == CM_2_SEND_HOLD))
           {
               mn_call_information_p[Multicall_Status[MN_CALL_WAITING_SESSION].connection_id[0]]-> 
                                                                            mt_call_accept_cause = CM_2_SEND_REQ;
               /* CONNECT */
               MN_put_MNCC_SETUP_RES(Multicall_Status[MN_CALL_WAITING_SESSION].connection_id[0],
                                                 &null_subaddress);
               MSG_HIGH_DS_0(MN_SUB,"=MNCNM= Answering the incoming call due to 2_SEND request and timer expiry of T308");
#ifdef FEATURE_NAS_VOC_REL_AFTER_CALL_HOLD  
               cw_data_ptr =  mn_call_information_p[Multicall_Status[MN_CALL_WAITING_SESSION].connection_id[0]];
              
               /*  IF 1st BC IS FOR SPEECH, CONNECT SPEECH NOW  */
               if(
#ifdef FEATURE_ALS
                  (cw_data_ptr->bearer_capability_1.information_transfer_capability == ITC_AUX_SPEECH) ||
#endif
                  (cw_data_ptr->bearer_capability_1.information_transfer_capability == ITC_SPEECH)
                 )
               {
                  MN_connect_TCH(cw_data_ptr->bearer_capability_1.information_transfer_capability);
               }
#endif
           }
           /* Reset all information relating the this state */
           MN_clear_call_data(connection_id);
  
           /* Refresh the Multicall Session per above state changes */
           MN_MULTICALL_LOG(MN_read_multicall_status());
  
           /* If this is the last connection, remove the vocoder connection */
           if ((Multicall_Status[0].call_activity == CALL_INACTIVE) &&
               (Multicall_Status[1].call_activity == CALL_INACTIVE) &&
               (Multicall_Status[2].call_activity == CALL_INACTIVE))
           {
#ifdef FEATURE_DUAL_SIM           
              if(MNCNM_IS_SINGLE_STACK)
              {
                MN_disconnect_TCH_with_AS_ID(MN_VOCODER_SWITCH,as_id);
              }
              else
#endif
              {
                MN_disconnect_TCH(MN_VOCODER_SWITCH);
              }

              /* For both GSM & UMTS, Use DEFAULT_RAB_ID below. Will change when we 
               * implement the Multicall Feature (Not the Multicall SS)  */
              tch_user_connections_store[DEFAULT_RAB_ID].
                      OTA_channel_connected = FALSE;  
           }
        }
        else
        {
           /*
            * Check to see what message initiated the MO call release
            */
           /* to check whether waiting call or the call being disconnected is not a voice call */
           /* and disconnect the vocoder  in that case */
           if ((Multicall_Status[0].call_activity == CALL_INACTIVE) &&
                  (Multicall_Status[1].call_activity == CALL_INACTIVE) &&
                  (Multicall_Status[2].call_activity == CALL_WAITING))
           {
              if(mn_call_information_p[connection_id]->active_bearer_capability == 0)
              {
                 if(((mn_call_information_p[connection_id]->bearer_capability_1.information_transfer_capability != ITC_SPEECH) &&
                     (mn_call_information_p[connection_id]->bearer_capability_1.information_transfer_capability != ITC_AUX_SPEECH)) ||
                     ((mn_call_information_p[Multicall_Status[MN_CALL_WAITING_SESSION].connection_id[0]]->bearer_capability_1.information_transfer_capability != ITC_SPEECH) &&
                     (mn_call_information_p[Multicall_Status[MN_CALL_WAITING_SESSION].connection_id[0]]->bearer_capability_1.information_transfer_capability != ITC_AUX_SPEECH)))
                 {
#ifdef FEATURE_DUAL_SIM     
                    if(MNCNM_IS_SINGLE_STACK)
                    {
                      MN_disconnect_TCH_with_AS_ID(MN_VOCODER_SWITCH,as_id);
                    }
                    else
#endif
                    {
                      MN_disconnect_TCH(MN_VOCODER_SWITCH);
                    }

                    MSG_HIGH_DS_0(MN_SUB,"=MNCNM= Disconnecting vocoder due to waiting or call which is disconnected being data call");
                 }
              }
              else
              {
                 if(((mn_call_information_p[connection_id]->bearer_capability_2.information_transfer_capability != ITC_SPEECH) &&
                     (mn_call_information_p[connection_id]->bearer_capability_2.information_transfer_capability != ITC_AUX_SPEECH)) ||
                     ((mn_call_information_p[Multicall_Status[MN_CALL_WAITING_SESSION].connection_id[0]]->bearer_capability_1.information_transfer_capability != ITC_SPEECH) &&
                     (mn_call_information_p[Multicall_Status[MN_CALL_WAITING_SESSION].connection_id[0]]->bearer_capability_1.information_transfer_capability != ITC_AUX_SPEECH)))
                 {
#ifdef FEATURE_DUAL_SIM     
                    if(MNCNM_IS_SINGLE_STACK)
                    {
                      MN_disconnect_TCH_with_AS_ID(MN_VOCODER_SWITCH,as_id);
                    }
                    else
#endif
                    {
                      MN_disconnect_TCH(MN_VOCODER_SWITCH);
                    }

                    MSG_HIGH_DS_0(MN_SUB,"=MNCNM= Disconnecting vocoder due to waiting or call which is disconnected being data call");
                 }                     
              }
           }       
           
           switch (data_ptr->mo_release_indicator)
           {
              case DEFAULT_REL:   /* Initiated by User, cm_mo_end_call_req */
  
                 /* Send log packet to DIAG */
                 log_packet_sent = mn_log_cs_call_release_info(connection_id, release_cause);
  
                 if (log_packet_sent == FALSE)
                 {
                     MSG_LOW_DS_0(MN_SUB,"=MNCNM= Cannot send log packet to DIAG");
                 }
                 /* If MT call is answered as a result of hold request or 1 send*/
                 if(CM_2_SEND_REQ == data_ptr->mt_call_accept_cause 
  #ifdef FEATURE_NAS_1SEND_ANDROID
                       || CM_1_SEND_REQ == data_ptr->mt_call_accept_cause 
  #endif
                    )
                 {                
                     MN_put_cm_manage_calls_conf(TRUE, 
#if defined(FEATURE_DUAL_SIM )
                                                 as_id, 
#endif    
                                                             dummy_ss_error); 
                 }  
                 /* Update the connection auxiliary state */
                 if(!MN_manage_CC_auxiliary_state(connection_id, CCE_CALL_ENDED))
                 {
                      MSG_HIGH_DS_2(MN_SUB,"=MNCNM= Unable to update AUX state",connection_id, CCE_CALL_ENDED);
                 }               
                 /* Reset all information relating the this state */
                 MN_clear_call_data(connection_id);
  
                 /* Refresh the Multicall Session per above state changes */
                 MN_MULTICALL_LOG(MN_read_multicall_status());
  
                 /* 
                  * If this is the last active call and there is a 
                  * waiting call, complete that call 
                  */
                 if ((Multicall_Status[0].call_activity == CALL_INACTIVE) &&
                     (Multicall_Status[1].call_activity == CALL_INACTIVE))
                 {
  
                    if (Multicall_Status[2].call_activity == CALL_WAITING)
                    {
                        mn_call_information_T  *cw_data_ptr;
                        ie_cm_cc_cause_T       cw_cc_cause;
                        connection_id_T        cw_connection_id;
  
                        /* dual service call confirmation & alerting */
                        cw_connection_id = Multicall_Status[2].connection_id[0];
  
                        cw_data_ptr =  mn_call_information_p[cw_connection_id];
  
                        cw_cc_cause.present = FALSE;
  
                        if( ((cw_data_ptr->dual_services == DUAL_SERVICE_IN_VOICE) ||
                             (cw_data_ptr->dual_services == DUAL_SERVICE_IN_DATA)) &&
                             (cc_transaction_information_p[cw_connection_id] != NULL) &&
                            (cc_transaction_information_p[cw_connection_id]->cc_state == CC_STATE_U6) )
                              
                        {
                            cw_cc_cause.present = FALSE;
  
                            MN_put_MNCC_CALL_CONF_REQ(FALSE,
                                                        cw_connection_id,
                                                        &cw_cc_cause);
     
                            MN_put_cm_mt_call_ind(
#if defined(FEATURE_DUAL_SIM )
                                                  as_id, 
#endif    
                                                  cw_connection_id);
    
                            MN_put_MNCC_ALERT_REQ(cw_connection_id);
                        }
                        else if( ((cw_data_ptr->dual_services == DUAL_SERVICE_IN_VOICE) ||
                                  (cw_data_ptr->dual_services == DUAL_SERVICE_IN_DATA)) &&
                                  (cc_transaction_information_p[cw_connection_id] != NULL) &&
                                 (cc_transaction_information_p[cw_connection_id]->cc_state == CC_STATE_U7) )
                        {
                            mn_call_information_p[cw_connection_id]->mt_call_accept_cause = CM_MT_CALL_RES;
  
                            MN_put_MNCC_SETUP_RES(cw_connection_id, &null_subaddress);
#ifdef FEATURE_NAS_VOC_REL_AFTER_CALL_HOLD  
                            /*  IF 1st BC IS FOR SPEECH, CONNECT SPEECH NOW */
                            if(
#ifdef FEATURE_ALS
                               (cw_data_ptr->bearer_capability_1.information_transfer_capability == ITC_AUX_SPEECH) ||
#endif
                               (cw_data_ptr->bearer_capability_1.information_transfer_capability == ITC_SPEECH)
                              )
                            {
                              MN_connect_TCH(cw_data_ptr->bearer_capability_1.information_transfer_capability);
                            }
#endif
                        }
                        else 
                        {
                            if(cc_transaction_information_p[cw_connection_id] != NULL)
                            {
                              MSG_ERROR_DS_3(MN_SUB,"=MNCNM= Invalid DUAL SERVICE=%d conn=%d, state=%d", cw_data_ptr->dual_services,
                                                                                   cw_connection_id, 
                                                                                   cc_transaction_information_p[cw_connection_id]->cc_state);
                            }
                            else
                            {
                              MSG_ERROR_DS_3(MN_SUB,"=MNCNM= Invalid DUAL SERVICE=%d conn=%d, state=%d", cw_data_ptr->dual_services,
                                                                                   cw_connection_id, 
                                                                                   CC_STATE_U0);
                            }
                        }
                    }
                    else
                    {
#ifdef FEATURE_DUAL_SIM                         
                        if(MNCNM_IS_SINGLE_STACK)
                        {
                          MN_disconnect_TCH_with_AS_ID(MN_VOCODER_SWITCH,as_id);
                        }
                        else
#endif
                        {
                          MN_disconnect_TCH(MN_VOCODER_SWITCH);
                        }

  
                        /* For both GSM & UMTS, Use DEFAULT_RAB_ID below. Will change 
                         * when we implement the Multicall Feature (Not the Multicall SS)*/
                         tch_user_connections_store[DEFAULT_RAB_ID].
                             OTA_channel_connected = FALSE;
  
                    }
  
                 } /* CALL WATING */
   
                 break;
  
              case CM_1_X_SEND_REL:
                {
  
                 /* Update the connection state */
                 if(!MN_manage_CC_auxiliary_state(connection_id, CCE_CALL_ENDED))
                 {
                   MSG_HIGH_DS_2(MN_SUB,"=MNCNM= Unable to update AUX state",connection_id, CCE_CALL_ENDED);
                 }
  
                 /* Refresh Call Session per above state changes */
                 MN_MULTICALL_LOG(MN_read_multicall_status());
  
                 /* 
                  * If this is the last active call and there is a 
                  * waiting call, complete that call 
                  */
                 if ((Multicall_Status[0].call_activity == CALL_INACTIVE) &&
                        (Multicall_Status[1].call_activity == CALL_INACTIVE))
                 {
                   if (Multicall_Status[2].call_activity == CALL_WAITING)
  
                   {
                      mn_call_information_p[Multicall_Status[2].
                          connection_id[0]]->
                          mt_call_accept_cause = CM_MO_END_CALL_REQ;
  
                      /* only for abnormal case handling where DISCONNECT may be
                              received in response to HOLD_REQ */  
                      if(mn_call_information_p[connection_id]->hold_request == CM_2_SEND_HOLD) 
                      {
                         MN_put_MNCC_SETUP_RES(Multicall_Status[MN_CALL_WAITING_SESSION].connection_id[0],
                                                &null_subaddress);      
                         mn_call_information_p[Multicall_Status[2].connection_id[0]]->mt_call_accept_cause = CM_2_SEND_REQ;
#ifdef FEATURE_NAS_VOC_REL_AFTER_CALL_HOLD  
/* if HELD Call was released, then Answering the Waiting call should trigger  vocoder acquisition */
                         cw_data_ptr =  mn_call_information_p[Multicall_Status[2].connection_id[0]];

                         /* IF 1st BC IS FOR SPEECH, CONNECT SPEECH NOW  */
                         if(
#ifdef FEATURE_ALS
                            (cw_data_ptr->bearer_capability_1.information_transfer_capability == ITC_AUX_SPEECH) ||
#endif
                            (cw_data_ptr->bearer_capability_1.information_transfer_capability == ITC_SPEECH)
                           )
                         {
                           MN_connect_TCH(cw_data_ptr->bearer_capability_1.information_transfer_capability);
                         }
#endif
                      }
                      MSG_HIGH_DS_0(MN_SUB,"=MNCNM= Answering the incoming call due to 2_SEND request");
                   }
                   else
                   {
#ifdef FEATURE_DUAL_SIM                        
                      if(MNCNM_IS_SINGLE_STACK)
                      {
                        MN_disconnect_TCH_with_AS_ID(MN_VOCODER_SWITCH,as_id);
                      }
                      else
#endif
                      {
                        MN_disconnect_TCH(MN_VOCODER_SWITCH);
                      }

  
                     /* For both GSM & UMTS, Use DEFAULT_RAB_ID below. Will change 
                      * when we implement the Multicall Feature (Not the Multicall SS)*/
                      tch_user_connections_store[DEFAULT_RAB_ID].
                          OTA_channel_connected = FALSE;
                   }
                 }
  
                 /* Reset all information relating the this state */
                 MN_clear_call_data(connection_id);
  
                 /* Update CM */
                 MN_put_cm_manage_calls_conf(TRUE, 
#if defined(FEATURE_DUAL_SIM )
                                           as_id, 
#endif    
                                           dummy_ss_error
                                           );

  
                 break;
               }
  
              case CM_1_SEND_REL:
               {
                 /* Update the connection state */
                 if(!MN_manage_CC_auxiliary_state(connection_id, CCE_CALL_ENDED))
                 {
                   MSG_HIGH_DS_2(MN_SUB,"=MNCNM= Updated CC AUX state failed", connection_id, CCE_CALL_ENDED);
                 }
  
                 /* wait until all active calls are cleared before doing anything */
                 if ( MN_num_of_calls_in_group(CALL_ACTIVE) == 0 )
                 {
                   /* See if there is room for an active call */
                   if (Multicall_Status[MN_HOLD_ACTIVE_SESSION].call_activity == CALL_INACTIVE)
                   {
                     /* 
                      * There is the possibility of getting a Waiting call or 
                      * retrieving a Held call according to the rules of GSM 02.30 
                      * section 4.5.5.1 
                      *
                      * Note: The Waiting call has priority.
                      */
                     if (((Multicall_Status[MN_ACTIVE_HOLD_SESSION].call_activity == CALL_HELD) ||
                          (Multicall_Status[MN_ACTIVE_HOLD_SESSION].call_activity == CALL_INACTIVE)) &&
                          (Multicall_Status[MN_CALL_WAITING_SESSION].call_activity == CALL_WAITING))
                     {
   
                         mn_call_information_p[Multicall_Status[MN_CALL_WAITING_SESSION].connection_id[0]]-> 
                                                                           mt_call_accept_cause = CM_1_SEND_REQ;
  
                         /* CONNECT */
                         MN_put_MNCC_SETUP_RES(Multicall_Status[MN_CALL_WAITING_SESSION].connection_id[0],
                                                &null_subaddress);
#ifdef FEATURE_NAS_VOC_REL_AFTER_CALL_HOLD  
                         cw_data_ptr =  mn_call_information_p[Multicall_Status[MN_CALL_WAITING_SESSION].connection_id[0]];
                     
                         /* IF 1st BC IS FOR SPEECH, CONNECT SPEECH NOW  */
                         if(
#ifdef FEATURE_ALS
                            (cw_data_ptr->bearer_capability_1.information_transfer_capability == ITC_AUX_SPEECH) ||
#endif
                            (cw_data_ptr->bearer_capability_1.information_transfer_capability == ITC_SPEECH)
                           )
                         {
                           MN_connect_TCH(cw_data_ptr->bearer_capability_1.information_transfer_capability);
                         }
#endif  
                     }
                     else if ( (Multicall_Status[MN_ACTIVE_HOLD_SESSION].call_activity == CALL_HELD) && 
                               (Multicall_Status[MN_CALL_WAITING_SESSION].call_activity == CALL_INACTIVE) )
                     {
                         /*  SEE THAT THE OTHER CALL IS A HELD CALL  */
                         retrieve_call =  TRUE;
                         retrieve_session = MN_ACTIVE_HOLD_SESSION;
                     }
                     else
                     {
                        retrieve_call =  FALSE;
                     }
                   } 
                   else  /* MN_ACTIVE_HOLD_SESSION */
                   {
                     /*  SEE THAT THE OTHER CALL IS A HELD CALL  */
                     if (((Multicall_Status[MN_HOLD_ACTIVE_SESSION].call_activity == CALL_HELD) ||
                          (Multicall_Status[MN_HOLD_ACTIVE_SESSION].call_activity == CALL_INACTIVE)) &&
                          (Multicall_Status[MN_CALL_WAITING_SESSION].call_activity == CALL_WAITING))
                     {
                         mn_call_information_p[Multicall_Status[MN_CALL_WAITING_SESSION].connection_id[0]]->
                                                                    mt_call_accept_cause = CM_1_SEND_REQ;
  
                         MN_put_MNCC_SETUP_RES(Multicall_Status[MN_CALL_WAITING_SESSION].connection_id[0],
                                                &null_subaddress);
#ifdef FEATURE_NAS_VOC_REL_AFTER_CALL_HOLD  
                         cw_data_ptr =  mn_call_information_p[Multicall_Status[MN_CALL_WAITING_SESSION].connection_id[0]];
                     
                         /* IF 1st BC IS FOR SPEECH, CONNECT SPEECH NOW  */
                         if(
#ifdef FEATURE_ALS
                            (cw_data_ptr->bearer_capability_1.information_transfer_capability == ITC_AUX_SPEECH) ||
#endif
                            (cw_data_ptr->bearer_capability_1.information_transfer_capability == ITC_SPEECH)
                           )
                         {
                           MN_connect_TCH(cw_data_ptr->bearer_capability_1.information_transfer_capability);
                         } 
#endif
                     }
                     else if ((Multicall_Status[MN_HOLD_ACTIVE_SESSION].call_activity == CALL_HELD) &&
                              (Multicall_Status[MN_ACTIVE_HOLD_SESSION].call_activity == CALL_INACTIVE) &&
                              (Multicall_Status[MN_CALL_WAITING_SESSION].call_activity == CALL_INACTIVE))
                     {
                        retrieve_call = TRUE;
                        retrieve_session = MN_HOLD_ACTIVE_SESSION;
                     }
                     else
                     {
                        retrieve_call = FALSE;
                     }
                   }
                 }
  
                 /*  Otherwise there may be no more connections */
                 if ((Multicall_Status[MN_ACTIVE_HOLD_SESSION].call_activity == CALL_INACTIVE) &&
                     (Multicall_Status[MN_HOLD_ACTIVE_SESSION].call_activity == CALL_INACTIVE) &&
                     (Multicall_Status[MN_CALL_WAITING_SESSION].call_activity == CALL_INACTIVE))
                 {
#ifdef FEATURE_DUAL_SIM                      
                      if(MNCNM_IS_SINGLE_STACK)
                      {
                        MN_disconnect_TCH_with_AS_ID(MN_VOCODER_SWITCH,as_id);
                      }
                      else
#endif
                      {
                        MN_disconnect_TCH(MN_VOCODER_SWITCH);
                      }


                      MN_put_cm_abort_cc_conf(
#if defined(FEATURE_DUAL_SIM )
                         as_id
#endif
                                           );
                      MN_clear_call_data(connection_id);
  
                      /* For both GSM & UMTS, Use DEFAULT_RAB_ID below. Will change when
                       * we implement the Multicall Feature (Not the Multicall SS)  */
                      tch_user_connections_store[DEFAULT_RAB_ID].
                                        OTA_channel_connected = FALSE;
  #ifdef FEATURE_NAS_1SEND_ANDROID
                      MN_put_cm_manage_calls_conf(TRUE, 
#if defined(FEATURE_DUAL_SIM )
                                                as_id, 
#endif    
                                                dummy_ss_error
                                                );
                      break;
  #endif
                 }
  
                 /* Reset all information relating the this state */
                 MN_clear_call_data(connection_id);
  #ifndef FEATURE_NAS_1SEND_ANDROID
                                  MN_put_cm_manage_calls_conf(TRUE, 
#if defined(FEATURE_DUAL_SIM )
                                           as_id, 
#endif    
                                           dummy_ss_error
                                           ); /* Successful release */

  #endif
                 break;
               }
  
              case CM_ABORT_REL:
                {
                    MN_clear_call_data(connection_id);
  
                    /* 
                     * If this is the last call to be cleared, send an 
                     * abort confirm message to cm 
                     */
                    MN_MULTICALL_LOG(MN_read_multicall_status());
  
                    if ((Multicall_Status[0].call_activity == CALL_INACTIVE) &&
                        (Multicall_Status[1].call_activity == CALL_INACTIVE) &&
                        (Multicall_Status[2].call_activity == CALL_INACTIVE))
                    {
#ifdef FEATURE_DUAL_SIM     
                       if(MNCNM_IS_SINGLE_STACK)
                       {
                         MN_disconnect_TCH_with_AS_ID(MN_VOCODER_SWITCH,as_id);
                       }
                       else
#endif
                       {
                         MN_disconnect_TCH(MN_VOCODER_SWITCH);
                       }

  
                       MN_put_cm_abort_cc_conf(
#if defined(FEATURE_DUAL_SIM )
                                 as_id
#endif
                                            );
  
                       /* For both GSM & UMTS, Use DEFAULT_RAB_ID below. Will change when
                        * we implement the Multicall Feature (Not the Multicall SS)  */
                       tch_user_connections_store[DEFAULT_RAB_ID].OTA_channel_connected = FALSE;
                    }
  
                    break;
                }
  
  #ifdef FEATURE_CCBS
              case CM_RECALL_REL:
                {
                   boolean  send_setup_ok=FALSE;
  
                   /* Update the connection state */
                   if(!MN_manage_CC_auxiliary_state(connection_id, CCE_CALL_ENDED))
                   {
                      MSG_HIGH_DS_2(MN_SUB,"=MNCNM= Updated CC AUX state failed", connection_id, CCE_CALL_ENDED);
                   }
  
                   /* See if there is room for an active call */
                   if (Multicall_Status[MN_HOLD_ACTIVE_SESSION].call_activity == CALL_INACTIVE)
                   {
                      /* 
                       * There is the possibility of getting a Waiting call or 
                       * retrieving a Held call according to the rules of GSM 02.30 
                       * section 4.5.5.1 
                       *
                       * Note: The Waiting call has priority. */
                      if ( Multicall_Status[MN_ACTIVE_HOLD_SESSION].call_activity == CALL_HELD ||
                           Multicall_Status[MN_ACTIVE_HOLD_SESSION].call_activity == CALL_INACTIVE )
                      {
                         send_setup_ok = TRUE;
                      }
                   } 
                   else  /* MN_ACTIVE_HOLD_SESSION */
                   {
                      /*  SEE THAT THE OTHER CALL IS A HELD CALL  */
                      if ( Multicall_Status[MN_HOLD_ACTIVE_SESSION].call_activity == CALL_HELD ||
                           Multicall_Status[MN_HOLD_ACTIVE_SESSION].call_activity == CALL_INACTIVE )
                      {
                         send_setup_ok = TRUE;
                      }
                      else if ( Multicall_Status[MN_HOLD_ACTIVE_SESSION].call_activity == CALL_HELD &&
                                Multicall_Status[MN_ACTIVE_HOLD_SESSION].call_activity == CALL_INACTIVE)
                      {
                         send_setup_ok = TRUE;
                      }
                   }
  
                   /* Reset all information relating the this state */
                   MN_clear_call_data(connection_id);
  
                   MN_put_cm_manage_calls_conf(TRUE, 
#if defined(FEATURE_DUAL_SIM )
                                             as_id, 
#endif    
                                             dummy_ss_error
                                             );
  
                   if (send_setup_ok)
                   {
                      cc_transaction_information_T  *cc_info;
                      byte                           cid;
                      cm_mo_normal_call_req_T        setup;
                      gs_status_T                    GSstatus;
  
                      /* look for only MO calls */
                      for (cid=0; cid<UNKNOWN_CONNECTION_ID; cid++)
                      {
                         cc_info = cc_transaction_information_p[cid];
                         
                         if ((cc_info != NULL) && (cc_info->cc_state == CC_STATE_U0_6))
                            break;
                      }
  
                      if (cid != UNKNOWN_CONNECTION_ID)
                      {
                         /* **************************************************** *
                          * create a fake setup message and send it to myself    *
                          * **************************************************** */
                         memset (&setup, 0x00, sizeof(cm_mo_normal_call_req_T));
  
                         /* Set up the message header */
                         setup.message_header.message_set = MS_MN_CM;
                         setup.message_header.message_id  = CM_MO_NORMAL_CALL_REQ;
                         PUT_IMH_LEN (sizeof(cm_mo_normal_call_req_T)-sizeof(IMH_T), &setup.message_header);
  
                         MN_fill_setup_from_mn (&setup, cid);
  
                         /* Send the message */
                         GSstatus = MN_send_message(GS_QUEUE_MN_CM, &setup, TRUE);
  
                         /* log any failures */
                         if(GSstatus != GS_SUCCESS)
                         {
                            MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Recall response failed");
                         }
                      }
                      else
                      {
                         MSG_ERROR_DS_0(MN_SUB,"=MNCNM= [ccbs]cannot find call in CC_STATE_U6");
                         /* do nothing */
                      }
                   }
                }
                break;
  #endif /* FEATURE_CCBS */
  
              default:
                {
                  MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Invalid mo release call");
   
                 break;
                }
  
           } /* release_indicator */
           if(mn_call_information_p[connection_id] != NULL)
           {
               mn_call_information_p[connection_id]->mo_release_indicator = DEFAULT_REL;
           }
           
        }
  
        /* Retrieve call session of 1 SEND REL */
        if(retrieve_call == TRUE) 
        {
            MN_retrieve_call(retrieve_session);
        }
     }
     else
     {
       MSG_HIGH_DS_1(MN_SUB,"=MNCNM= cannot process MNCC_REL_IND as mn_call_information_p[%d] is NULL",connection_id);
     }
     
   }
   else
   {
      MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Connection id invalid for mn_call_information_p");
   }
}

#ifdef FEATURE_NAS_MN_CB_SERVICES 

/*
 * Function name :  cm_init_conn_ids_crit_sect
 * -------------
 *
 * Description : 
 * -----------     
 *
 *  Initialize the semaphore that is used by the 
 *  the CM or UI task to modify MN global data inorder
 *  to allocate or deallocate the connection ID.
 *
 * Parameters: None
 * ----------  
 *
 * Returns: Reserved connection_id, if available. Else, 
 * --------   
 *   UNKNOWN_CONNECTION_ID.
 *
 */

void MN_process_ds_sync_notify(MNCC_SYNC_IND_T *message_ptr)
{

    if(message_ptr != NULL)
    {
       /* DS */
       if((mn_cb_services[MN_CLIENT_DS].cb_function.sync_notify != NULL) && 
          (mn_cb_services[MN_CLIENT_DS].client == MN_CLIENT_DS) &&
          (mn_cb_services[MN_CLIENT_DS].type == MN_SYNC_NOTIFICATION))
       {
            /* send the HO notification */
            (*mn_cb_services[MN_CLIENT_DS].cb_function.sync_notify)(message_ptr->reason, message_ptr->radio_access_bearer_id);
       }

       /* ATCOP */
       if((mn_cb_services[MN_CLIENT_ATCOP].cb_function.sync_notify != NULL) && 
          (mn_cb_services[MN_CLIENT_ATCOP].client == MN_CLIENT_ATCOP) &&
          (mn_cb_services[MN_CLIENT_ATCOP].type == MN_SYNC_NOTIFICATION))
       {
            /* send the HO notification */
            (*mn_cb_services[MN_CLIENT_ATCOP].cb_function.sync_notify)(message_ptr->reason, message_ptr->radio_access_bearer_id);
       }
    }
    else  
    {
        MSG_ERROR_DS_1(MN_SUB,"=MNCNM= Invalid msg = %d", message_ptr);
    }

}



/*
 * Function name :  MN_check_ho_cs_data_notify()
 * -------------
 *
 * Description :  
 * -----------     
 *   Function to check if the CS DATA HO notification is needed send to DS.
 *
 * Parameters: message_header
 * ----------  
 *
 * Returns:
 * --------
 *
 */
VOID_FUNC MN_check_ho_cs_data_notify(MNCC_SYNC_IND_T *message_ptr)
{
    mn_call_information_T *data_ptr = NULL;

    connection_id_T connection_id = message_ptr->connection_id;
    
    if(connection_id < MAXNO_CC_TRANSACTIONS)
    {
      if(mn_call_information_p[connection_id] != NULL)
      {
         data_ptr =  mn_call_information_p[connection_id];
        
         /* CS Data Handover Notification */
         if(
#ifdef FEATURE_ALS
            (data_ptr->bearer_capability_1.information_transfer_capability != ITC_AUX_SPEECH) && 
#endif
            (data_ptr->bearer_capability_1.information_transfer_capability != ITC_SPEECH) /* CS Data Services */
           )
         {
             /* Channel modified or handover */
             if ((message_ptr->reason == WCDMA_GSM_HANDOVER) ||
                 (message_ptr->reason == WCDMA_RAB_RECONFIGURED) ||
                 (message_ptr->reason == GSM_WCDMA_HANDOVER) ||
                 (message_ptr->reason == GSM_GSM_HANDOVER) ||
                 (message_ptr->reason == GSM_CHANNEL_MODE_MODIFIED))
             {
                 MN_process_ds_sync_notify(message_ptr);
             }
         }  /* CS Data Handover Notification Services */
      }
      else
      {
        MSG_HIGH_DS_1(MN_SUB,"=MNCNM= MN_check_ho_cs_data_notify:mn_call_information_p[%d] is NULL",connection_id);
      }

    }
    else
    {
        ERR("Invalid connection ID = %d, data_ptr=%d", connection_id, data_ptr, 0);
    }
}

#endif /* FEATURE_NAS_MN_CB_SERVICES */  


/*
 * Function name :  MN_configure_TCH()
 * -------------
 *
 * Description :  
 * -----------     
 *   Function to configure the TCH for MVS according to the tch_user_connecton_store[] data and
 *   sync_reason that is received from MM.
 *
 * Parameters: message_header
 * ----------  
 *
 * Returns:
 * --------
 *
 */
VOID_FUNC  MN_configure_TCH(MNCC_SYNC_IND_T * message_ptr)
{
    mn_call_information_T  *data_ptr;
    connection_id_T        connection_id;
    boolean                channel_mode_changed = TRUE;
    boolean                voc_aquisition_going_on = FALSE;
    boolean                is_G_RAT = FALSE;
    boolean                is_W_RAT = FALSE;
    

    connection_id = message_ptr->connection_id;

    if(connection_id >= MAXNO_CC_TRANSACTIONS)
    {
      ERR("Invalid connection id = %d", connection_id, 0, 0);
    }
    else if(mn_call_information_p[connection_id] == NULL)
    {
      MSG_HIGH_DS_1(MN_SUB,"=MNCNM= cannot configure TCH as mn_call_information_p[%d] is NULL",
                  connection_id);
    }
    else
    {
      data_ptr =  mn_call_information_p[connection_id];

#ifdef FEATURE_DUAL_SIM
      if(MNCNM_IS_SINGLE_STACK)
      {
        is_G_RAT = is_cs_gsm_mode(mn_call_information_p[connection_id]->as_id);
        is_W_RAT = is_cs_umts_mode(mn_call_information_p[connection_id]->as_id);
      }
#ifdef FEATURE_DUAL_ACTIVE
      else
      {
        is_G_RAT = is_cs_gsm_mode((sys_modem_as_id_e_type)mn_dsda_as_id);
        is_W_RAT = is_cs_umts_mode((sys_modem_as_id_e_type)mn_dsda_as_id);
      }
#endif
#else   
      {
        is_G_RAT = is_gsm_mode();
        is_W_RAT = is_umts_mode();
      }
#endif

      if(is_G_RAT == TRUE)
      {
         /* This is GSM Context, use DEFAULT_RAB_ID in this block */
         data_ptr->radio_access_bearer_id = DEFAULT_RAB_ID;

         /*********************************/
         /* OTA channel already connected */
         /*********************************/
         if (tch_user_connections_store[DEFAULT_RAB_ID].OTA_channel_connected == TRUE)
         {
            /*********************************************************/
            /* Channel configurate changed and/or GSM 2 GSM handover */
            /*********************************************************/
            if(((message_ptr->reason == GSM_GSM_HANDOVER) ||
               (message_ptr->reason == GSM_CHANNEL_MODE_MODIFIED)) && CNM_has_active_cc_connection())
            {
                voc_aquisition_going_on = ghdi_is_acquisition_in_progress();
                if(voc_aquisition_going_on != TRUE)
                {
                  channel_mode_changed = FALSE;  
                  MSG_HIGH_DS_0(MN_SUB,"=MNCNM= GSM->GSM HO or GSM Channel Modified:  IGNORED by NAS");               
                }
                else
                {
                  MSG_HIGH_DS_0(MN_SUB,"=MNCNM= GSM->GSM HO or GSM Channel Modified: Updated by NAS");
                }
                /* Store new channel configuration */  
                tch_user_connections_store[DEFAULT_RAB_ID].subchannel = message_ptr->sub_channel;
                tch_user_connections_store[DEFAULT_RAB_ID].tch_rate = message_ptr->tch_rate;
                tch_user_connections_store[DEFAULT_RAB_ID].channel_mode = message_ptr->channel_mode;

            }
            else if ((tch_user_connections_store[DEFAULT_RAB_ID].subchannel != message_ptr->sub_channel) ||
                     (tch_user_connections_store[DEFAULT_RAB_ID].tch_rate != message_ptr->tch_rate) ||
                     (tch_user_connections_store[DEFAULT_RAB_ID].channel_mode !=  message_ptr->channel_mode))
            {
                /*****************************/
                /* Same traffic channel rate */
                /*****************************/
                if ((tch_user_connections_store[DEFAULT_RAB_ID].tch_rate == message_ptr->tch_rate) && /* no blanking needed */
                    (tch_user_connections_store[DEFAULT_RAB_ID].subchannel == message_ptr->sub_channel))
                {
                    /*****************************/
                    /* Different speech versions */
                    /*****************************/
                    if(tch_user_connections_store[DEFAULT_RAB_ID].channel_mode != message_ptr->channel_mode)
                    {

                        MN_disconnect_TCH(MN_VOCODER_SWITCH_TO_WCDMA); /* to different mode */ 

                        switch (message_ptr->channel_mode)
                        {
                            case SPEECH_V1:
                            case SPEECH_V2:    /* EFR */
                            case SPEECH_V3:   /* GSM AMR */
                            case SPEECH_V5:   /* GSM AMR WB*/                                
                                {
                                    MSG_HIGH_DS_1(MN_SUB,"=MNCNM= Switching to new GSM Vocoder mode=%d", message_ptr->channel_mode);
                                }
                                break;

                            case SIGNALLING_ONLY:
                                if (message_ptr->channel_mode == SIGNALLING_ONLY)
                                {
                                    channel_mode_changed = FALSE;  /* to prevent from connecting TCH to Vocoder */
                                    MSG_HIGH_DS_1(MN_SUB,"=MNCNM= Switching channel mode to signalling only=%d",message_ptr->channel_mode);
                                }
                                break;

                            case DATA_14400:
                            case DATA_9600:
                            case DATA_4800:
                            case DATA_2400:
                                {
                                    channel_mode_changed = FALSE;  /* to prevent from connecting TCH to Vocoder */
                                    MSG_HIGH_DS_1(MN_SUB,"=MNCNM= Switching channel mode to DATA=%d",message_ptr->channel_mode);
                                }
                                break;
                    
                            default:
                                {
                                    channel_mode_changed = FALSE;  /* to prevent from connecting TCH to Vocoder */
                                    MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Unsupported Incall-modification");
                                }
                                break;
                                
                        }  /* End of switch... */

                    } /* channel_mode */
             
                }  /* Different traffic channel rates */      
                else  
                {
                    MN_disconnect_TCH(MN_VOCODER_OFF); 

                    if((message_ptr->channel_mode == SPEECH_V1) ||
                       (message_ptr->channel_mode == SPEECH_V2) ||
                       (message_ptr->channel_mode == SPEECH_V3) ||
                       (message_ptr->channel_mode == SPEECH_V5))
                    {
                        MSG_HIGH_DS_1(MN_SUB,"=MNCNM= Switching GSM Vocoder to new rate=%d", message_ptr->tch_rate);
                    }
                    else
                    {
                        channel_mode_changed = FALSE; 
                    }
                }

                /* Store new channel configuration */  
                tch_user_connections_store[DEFAULT_RAB_ID].OTA_channel_connected = TRUE;
                tch_user_connections_store[DEFAULT_RAB_ID].subchannel = message_ptr->sub_channel;
                tch_user_connections_store[DEFAULT_RAB_ID].tch_rate = message_ptr->tch_rate;
                tch_user_connections_store[DEFAULT_RAB_ID].channel_mode = message_ptr->channel_mode;

            } /* GSM TCH config changed */
            else
            {
                channel_mode_changed = FALSE; 
                MSG_HIGH_DS_0(MN_SUB,"=MNCNM= No change in GSM TCH configuration");
            }

         }
         /*********************************************************/
         /* OTA channel not connected, Do channel assignment here */
         /*********************************************************/
         else
         {
            /* Save vocoder config data 'til OTA L3 CONNECT message or => CC_STATE_U8 */
            tch_user_connections_store[DEFAULT_RAB_ID].OTA_channel_connected = TRUE;
            tch_user_connections_store[DEFAULT_RAB_ID].tch_rate = message_ptr->tch_rate;
            tch_user_connections_store[DEFAULT_RAB_ID].subchannel = message_ptr->sub_channel;
            tch_user_connections_store[DEFAULT_RAB_ID].channel_mode = message_ptr->channel_mode;
         }  

      } /* is_gsm_mode...*/
      else if (is_W_RAT == TRUE)
      {

         /* Record rab id for later use */
         data_ptr->radio_access_bearer_id = message_ptr->radio_access_bearer_id;

         tch_user_connections_store[DEFAULT_RAB_ID].OTA_channel_connected = TRUE;
#ifdef FEATURE_VOC_AMR_WB 
         tch_user_connections_store[DEFAULT_RAB_ID].umts_codec = message_ptr->umts_codec;
#endif  /*end of FEATURE_VOC_AMR_WB */
#ifdef FEATURE_CS_VOICE_OVER_HSPA
         tch_user_connections_store[DEFAULT_RAB_ID].cs_voice_type = message_ptr->cs_voice_type;
#endif
      }  /* else if is_cs_umts_mode...*/
      else
      {
          MSG_ERROR_DS_1(MN_SUB,"=MNCNM= Invalid/Unsupported UMTS mode", connection_id);
      }

      /****************************************************** 
       * Common (UMTS & GSM) Vocoder Configuration Handling *
       ******************************************************/

      if(connection_id > UNKNOWN_CONNECTION_ID)   /* MT transaction ID */
      {
          /*  PROCESS MT CALLS  */
          if(data_ptr->signal_present == FALSE)
          {
             /*  
             * ALERT THE CM THAT THERE IS AN INCOMING CALL  -
             * ONLY in state CC_STATE_U9; Handle Early Assignment   
             */
             if(cc_transaction_information_p[connection_id] != NULL)
             {
               if(cc_transaction_information_p[connection_id]->cc_state == CC_STATE_U9)
               {
                   MN_put_cm_mt_call_ind(
#if defined(FEATURE_DUAL_SIM )
                                        mn_call_information_p[connection_id]->as_id, 
#endif     
                                        connection_id);
                   MN_put_MNCC_ALERT_REQ(connection_id);
               }
               else if (cc_transaction_information_p[connection_id]->cc_state == CC_STATE_U8)
               {

                  if( is_G_RAT == TRUE )
                  {
                      if((message_ptr->channel_mode == SPEECH_V1) ||
                         (message_ptr->channel_mode == SPEECH_V2) ||
                         (message_ptr->channel_mode == SPEECH_V3) ||                    
                         (message_ptr->channel_mode == SPEECH_V5))
                      {
                         /*  
                         * if a MNCC_SETUP_RES message has been sent, the CC state 
                         * will be CC_STATE_U8 or higher, so connect immediately 
                         */
                         if ((cc_transaction_information_p[connection_id] != NULL) &&
                             (cc_transaction_information_p[connection_id]->cc_state >= CC_STATE_U8) &&
                             (message_ptr->reason == GSM_RES_ASSIGNED))
                         {
                            MN_connect_TCH(data_ptr->bearer_capability_1.information_transfer_capability);
                         }
                      }
                      else  /* DATA */
                      {
                         MN_disconnect_TCH(MN_VOCODER_OFF);

                         MSG_ERROR_DS_1(MN_SUB,"=MNCNM= Invalid GSM channel mode", message_ptr->channel_mode);
                      }
                  }
                  else /* UMTS */
                  {
                     if ((cc_transaction_information_p[connection_id] != NULL) &&
                         (cc_transaction_information_p[connection_id]->cc_state >=CC_STATE_U8) &&
                         (message_ptr->reason == WCDMA_RAB_ESTABLISHED))
                     {
                        MN_connect_TCH(data_ptr->bearer_capability_1.information_transfer_capability);
                     }
                  } /* is_gsm_mode */

               }
             }

          }
          else /* no signal_present */
          {
             if( is_G_RAT == TRUE )
             {
                 if((message_ptr->channel_mode == SPEECH_V1) ||
                    (message_ptr->channel_mode == SPEECH_V2) ||
                    (message_ptr->channel_mode == SPEECH_V3) ||                    
                    (message_ptr->channel_mode == SPEECH_V5))
                 {
                     /*  
                     * if a MNCC_SETUP_RES message has been sent, the CC state 
                     * will be CC_STATE_U8 or higher, so connect immediately 
                     */
                     if ((cc_transaction_information_p[connection_id] != NULL) &&
                         (cc_transaction_information_p[connection_id]->cc_state >= CC_STATE_U8) &&
                         (message_ptr->reason == GSM_RES_ASSIGNED))
                     {
                        MN_connect_TCH(data_ptr->bearer_capability_1.information_transfer_capability);
                     }
                 }
                 else  /* DATA */
                 {
                     MN_disconnect_TCH(MN_VOCODER_OFF);

                     MSG_ERROR_DS_1(MN_SUB,"=MNCNM= Invalid GSM channel mode", message_ptr->channel_mode);
                 }
             }
             else /* UMTS */
             {
                 if ((cc_transaction_information_p[connection_id] != NULL) &&
                     (cc_transaction_information_p[connection_id]->cc_state >=CC_STATE_U8) &&
                     (message_ptr->reason == WCDMA_RAB_ESTABLISHED))
                 {
                     MN_connect_TCH(data_ptr->bearer_capability_1.information_transfer_capability);
                 }
             } /* is_gsm_mode */

          } /* Signal IE */

          /* IDLE handover check */
          if((message_ptr->reason == WCDMA_GSM_HANDOVER) ||
             (message_ptr->reason == WCDMA_RAB_RECONFIGURED) ||
             (message_ptr->reason == GSM_WCDMA_HANDOVER) ||
             (message_ptr->reason == GSM_CHANNEL_MODE_MODIFIED) ||
             (message_ptr->reason == GSM_GSM_HANDOVER))
          {
               if(channel_mode_changed && ((cc_transaction_information_p[connection_id] != NULL) && 
                                           (cc_transaction_information_p[connection_id]->cc_state == CC_STATE_U8 ||
                                            cc_transaction_information_p[connection_id]->cc_state >= CC_STATE_U10)))
               {
#if defined(FEATURE_MULTIMEDIA_FALLBACK) || defined(FEATURE_REL6)
                   boolean active_bearer_capability;

                   /* Update the active BCIE */
                   if( (cc_transaction_information_p[connection_id] != NULL) &&
                       ((cc_transaction_information_p[connection_id]->cc_state == CC_STATE_U26) ||
                        (cc_transaction_information_p[connection_id]->cc_state == CC_STATE_U27)) )
                   {
                       active_bearer_capability = ((data_ptr->active_bearer_capability) ? FALSE : TRUE); 
                   }
                   else
                   {
                       active_bearer_capability = data_ptr->active_bearer_capability; 
                   }
                   if(active_bearer_capability)
                   {
                     if(voc_aquisition_going_on != TRUE)
                     {
                       MN_connect_TCH(data_ptr->bearer_capability_2.information_transfer_capability);
                       MSG_HIGH_DS_1(MN_SUB,"=MNCNM= enabling vocoder with BCIE2 due to incall modification with ITC = %d", data_ptr->bearer_capability_2.information_transfer_capability);
                     }
                     else
                     {
                       MN_update_code_type(data_ptr->bearer_capability_2.information_transfer_capability);
                     }
                   }
                   else
#endif /* FEATURE_MULTIMEDIA_FALLBACK */
                   {
                     if(voc_aquisition_going_on != TRUE)
                     {                   
                       MN_connect_TCH(data_ptr->bearer_capability_1.information_transfer_capability); 
                       MSG_HIGH_DS_1(MN_SUB,"=MNCNM= enabling vocoder with BCIE1 and ITC = %d", data_ptr->bearer_capability_1.information_transfer_capability);                       
                     }
                     else
                     {
                       MN_update_code_type(data_ptr->bearer_capability_1.information_transfer_capability);
                     }
                   }
               }
                   
               /* Only for SRVCC calls*/
               else if((cc_transaction_information_p[connection_id] != NULL) &&
                       ((cc_transaction_information_p[connection_id]->cc_state >= CC_STATE_U3) ||
                        (cc_transaction_information_p[connection_id]->cc_state >= CC_STATE_U4))&&
                       (MN_check_for_inband_message(connection_id) == TRUE))
               {
                  MSG_HIGH_DS_2(MN_SUB,"=MNCNM= Received  SRVCC call with connection_id = %d in state =%d",
                                                                       connection_id, cc_transaction_information_p[connection_id]->cc_state); 
                  MN_connect_TCH(data_ptr->bearer_capability_1.information_transfer_capability);
                  
               }

               MSG_HIGH_DS_1(MN_SUB,"=MNCNM= Handover completed, MVS enabled", connection_id);
          }

      } /* MT Calls Handling */
      else if ( connection_id < UNKNOWN_CONNECTION_ID )  /* MO Transaction */
      {
          /*  PROCESS MT CALL */
          if (((message_ptr->reason == WCDMA_GSM_HANDOVER) ||
              (message_ptr->reason == WCDMA_RAB_RECONFIGURED) ||
              (message_ptr->reason == GSM_WCDMA_HANDOVER) ||
              (message_ptr->reason == GSM_CHANNEL_MODE_MODIFIED)) &&
              (cc_transaction_information_p[connection_id] != NULL) &&
              (cc_transaction_information_p[connection_id]->cc_state >= CC_STATE_U3) &&
              (MN_check_for_inband_message(connection_id) == TRUE)
              )
          {
               if(channel_mode_changed)
               {
#if defined(FEATURE_MULTIMEDIA_FALLBACK) || defined(FEATURE_REL6)

                   boolean active_bearer_capability;

                   /* Update the active BCIE */
                   if((cc_transaction_information_p[connection_id] != NULL) &&
                      ((cc_transaction_information_p[connection_id]->cc_state == CC_STATE_U26) ||
                       (cc_transaction_information_p[connection_id]->cc_state == CC_STATE_U27)) )
                   {
                       active_bearer_capability = ((data_ptr->active_bearer_capability) ? FALSE : TRUE); 
                   }
                   else
                   {
                       active_bearer_capability = data_ptr->active_bearer_capability; 
                   }

                   if(active_bearer_capability)
                   {
                       MN_connect_TCH(data_ptr->bearer_capability_2.information_transfer_capability);
                   }
                   else
#endif /* FEATURE_MULTIMEDIA_FALLBACK */
                   {
                       MN_connect_TCH(data_ptr->bearer_capability_1.information_transfer_capability); 
                   }
               }

               MSG_HIGH_DS_1(MN_SUB,"=MNCNM= Handover completed, MVS enabled", connection_id);
          }
          /* Connect user if a progress indicator(in-band-messages) has 
          *  been received ans send an indication to CM.  
          */
          else if (((message_ptr->reason == WCDMA_RAB_ESTABLISHED) || 
                    (message_ptr->reason == GSM_RES_ASSIGNED)) &&  /* TCH assignment */
                    (cc_transaction_information_p[connection_id] != NULL) &&
                    (cc_transaction_information_p[connection_id]->cc_state >= CC_STATE_U3) &&
                    (MN_check_for_inband_message(connection_id) == TRUE))
          {
               MN_connect_TCH(data_ptr->bearer_capability_1.information_transfer_capability);

               MN_put_cm_call_progress_info_ind(connection_id, USER_RESUMED);  
          }
          else
          {
               MSG_HIGH_DS_1(MN_SUB,"=MNCNM= Sync ind = %d, vocoder IDLE",message_ptr->reason);
          }
      }
      else
      {
           MSG_HIGH_DS_1(MN_SUB,"=MNCNM= Early TCH assignment or idle Handover", connection_id);
      } 

      /* Check for call Re-Establishment */
      if((connection_id <  MAXNO_CC_TRANSACTIONS) && 
         (connection_id != UNKNOWN_CONNECTION_ID))
      {
          if((cc_transaction_information_p[connection_id] != NULL) &&
             (cc_transaction_information_p[connection_id]->cc_state == CC_STATE_U10) &&
             ((message_ptr->reason == WCDMA_RAB_ESTABLISHED) || 
              (message_ptr->reason == GSM_RES_ASSIGNED)))
          {
              MN_connect_TCH(data_ptr->bearer_capability_1.information_transfer_capability);
          } 
      }
      
#ifdef FEATURE_NAS_MN_CB_SERVICES
      MN_check_ho_cs_data_notify(message_ptr);
#endif /* FEATURE_NAS_MN_CB_SERVICES */
      /* RAB Change notification */
      if((message_ptr->reason == WCDMA_RAB_RECONFIGURED) ||
         (message_ptr->reason == GSM_CHANNEL_MODE_MODIFIED))
      {
#ifdef FEATURE_MULTIMEDIA_FALLBACK
          #error code not present
#endif
          MN_put_cm_call_progress_info_ind(message_ptr->connection_id, BEARER_CHANGED);
      }

    } /* connection id */

}




/*
 * Function name :  MN_handle_MNCC_SYNC_IND
 * -------------
 *
 * Description :  Function to deal with the sync indication from CM
 * -----------     
 *
 *
 *
 * Parameters: message_header
 * ----------  
 *
 *
 *
 * Returns:
 * --------
 *
 */

VOID_FUNC MN_handle_MNCC_SYNC_IND(IMH_T *message)
{

   /* Locals */
   MNCC_SYNC_IND_T *message_ptr;
   connection_id_T   connection_id, index;

   /* Reset mn_waiting_for_tch_in_srvcc as 
    * MN has now received SYNC_IND. This var
    * was set when NAS received context transfer 
    * response and no SYNC_IND was received by 
    * that time */
   mn_waiting_for_tch_in_srvcc = FALSE;
   message_ptr = (MNCC_SYNC_IND_T *)((void *)message);

   connection_id = message_ptr->connection_id;

   /* CHECK IF CONNECTION ID IS VALID */
   if(connection_id >= MAXNO_CC_TRANSACTIONS)
   {
      MSG_ERROR_DS_1(MN_SUB,"=MNCNM= Invalid connection ID", connection_id);
   }
   else  /* acceptable CC Transaction  */
   {
      /***********************************************/
      /* Re-configure Vocoder for Inter-RAT Handoff  */
      /***********************************************/
      switch(message_ptr->reason)
      {
          case WCDMA_RAB_ESTABLISHED:
          {
              MSG_HIGH_DS_1(MN_SUB,"=MNCNM= WCDMA TCH channel assigned", connection_id);

              MN_configure_TCH(message_ptr);
              break;
          } 
          case WCDMA_GSM_HANDOVER:
          {
              MSG_HIGH_DS_1(MN_SUB,"=MNCNM= WCDMA 2 GSM Handover", connection_id);

#ifdef FEATURE_NAS_ECALL
              MN_put_cm_handover_complete_ind(connection_id,message_ptr->reason);
#endif
              MN_disconnect_TCH(MN_VOCODER_SWITCH_TO_GSM);
              MN_configure_TCH(message_ptr);
              break;
          }
          case WCDMA_RAB_RECONFIGURED:
          {
              MSG_HIGH_DS_1(MN_SUB,"=MNCNM= RAB re-Configuration", connection_id);

              MN_disconnect_TCH(MN_VOCODER_OFF);
              MN_configure_TCH(message_ptr);

#if defined(FEATURE_MULTIMEDIA_FALLBACK) || defined(FEATURE_REL6)
              if(mn_call_information_p[connection_id] != NULL)
              {
                mn_call_information_p[connection_id]->modifiy_in_progress = FALSE;
                mn_store_rab_modify_status();
                MSG_HIGH_DS_0(MN_SUB,"=MNCNM= modify_in_progress set to False ");
              }         
                    else
                    {
                      MSG_HIGH_DS_0(MN_SUB,"=MNCNM= cannot set modify_in_progress as mn_call_information_p[%d] is NULL",
                          connection_id, 0, 0);
                    }
#endif /* FEATURE_MULTIMEDIA_FALLBACK */
              break;
          }
          case WCDMA_RAB_RELEASED:
          {
              MSG_HIGH_DS_0(MN_SUB,"=MNCNM= WCDMA RAB (TCH) released");

              if(tch_user_connections_store[DEFAULT_RAB_ID].OTA_channel_connected == TRUE)
              {
#ifdef FEATURE_DUAL_SIM                   
                  if(MNCNM_IS_SINGLE_STACK)
                  {
                    if(mn_call_information_p[connection_id] != NULL)
                    {
                      MN_disconnect_TCH_with_AS_ID(MN_VOCODER_OFF,mn_call_information_p[connection_id]->as_id);
                    }
                    else
                    {
                      MSG_HIGH_DS_0(MN_SUB,"=MNCNM= MN Call object is NULL, Cannot Release Vocoder\n");                
                    }  
                  }
                  else
#endif
                  {
                    MN_disconnect_TCH(MN_VOCODER_OFF);
                  }
              }

              /* Inform CM that active + connected transaction should be release */
              for(index=0; index < MAXNO_CC_TRANSACTIONS; index++)
              {
                  if (index==UNKNOWN_CONNECTION_ID)
                     continue;

                  if((cc_transaction_information_p[index]!= NULL) &&
                     (cc_transaction_information_p[index]->cc_state == CC_STATE_U10))
                  {
                      MN_put_cm_call_progress_info_ind(index, BEARER_RELEASED);
                  }
              }

              break;
          }
          case GSM_WCDMA_HANDOVER:
          {
              MSG_HIGH_DS_1(MN_SUB,"=MNCNM= GSM 2 WCDMA Handover", connection_id);

#ifdef FEATURE_NAS_ECALL
              MN_put_cm_handover_complete_ind(connection_id,message_ptr->reason);
#endif

              MN_disconnect_TCH(MN_VOCODER_SWITCH_TO_WCDMA);
              MN_configure_TCH(message_ptr);
              break;
          }
          case GSM_GSM_HANDOVER:
          {
              MSG_HIGH_DS_1(MN_SUB,"=MNCNM= GSM 2 GSM Handover", connection_id);

              MN_configure_TCH(message_ptr);  /* save configuration as call setup may not have started */
              break; 
          }
          case GSM_CHANNEL_MODE_MODIFIED:
          {
              MSG_HIGH_DS_1(MN_SUB,"=MNCNM= GSM TCH channel had been modified", connection_id);

              MN_configure_TCH(message_ptr); /* save configuration as call setup may not have started */
              break;
          }
          case GSM_RES_ASSIGNED:
          {
              MSG_HIGH_DS_1(MN_SUB,"=MNCNM= GSM TCH channel assigned", connection_id);

              MN_configure_TCH(message_ptr);
              break;
          } 
          default: 
          {
              MSG_ERROR_DS_2(MN_SUB,"=MNCNM= Unknown SYNC reason conn = %d, reason = %d", connection_id, message_ptr->reason);
              break;
          }
      } /* switch message_ptr->reason */
   }
}



/*
 *   Function name : MN_process_MNCC_CALL_HOLD_CNF
 *   ---------------
 *
 *   Description:
 *   ------------
 * Process the network response to a call related SS request. This will mean
 * stopping any CALL SS timer for the particular transaction, update all auxiliary states
 * for those transactions affected by a request, e.g. for a build mpty request, if
 * a successful build is indicated, then change the mpty auxiliary states of all
 * calls with cc state CC_STATE_U10 to 'Call in Mpty' by sending the c_call_event to MN_manage_CC_auxiiary_state
 *
 *
 *  Parameters:
 *  -----------
 *  IN message
 *
 *  Returns:
 *
 *
 */

VOID_FUNC   MN_process_MNCC_CALL_HOLD_CNF(IMH_T   *message)
{

   MNCC_CALL_HOLD_CNF_T     *pointer;
   boolean                   s_respond_to_event;
   int                       Multicall_Status_index,index;
   connection_id_T           connection_id;
   ie_cm_ss_error_T          cm_ss_error = { 0        };
   cm_connected_subaddress_T null_connected_subaddress;
   byte no_auto_answer_on_hold = mn_get_no_auto_answer_on_hold();
   
 
   /* initialize buffer */
   cm_ss_error.present = FALSE;

   /* initialize null_connected_subaddress */
   memset(&null_connected_subaddress,0,sizeof(cm_connected_subaddress_T));

   /* align pointers */
   pointer = (MNCC_CALL_HOLD_CNF_T *)((void *)message);

   if (mn_alternate_proc_status.is_alternate_procedure == TRUE)
   {
      if (mn_alternate_proc_status.status == MN_SS_ALT_HOLD_ACK_RETRIEVE_ACK_AWAITED)
      {
        mn_alternate_proc_status.status = MN_SS_ALT_RETRIEVE_ACK_AWAITED;
      }
      else if (mn_alternate_proc_status.status == MN_SS_ALT_HOLD_ACK_AWAITED)
      {
        mn_alternate_proc_status.status = MN_SS_ALT_NONE_AWAITED;
        mn_alternate_proc_status.is_alternate_procedure = FALSE;

        /* Clear out any remaining DTMF requests and send the DTMF CNF to CM  */
        MN_flush_queued_dtmf_requests(pointer->connection_id);
        /* Update CC auxiliary state for this connection */
        s_respond_to_event = MN_manage_CC_auxiliary_state(pointer->connection_id, CCE_CALL_HOLD_CONF);/* c_call_event value */

        if (s_respond_to_event == TRUE)
        {
          /* clear the ss pending requests */   
          mn_call_information_p[pointer->connection_id]->ss_request_pending= CM_NONE_PENDING;
          mn_call_information_p[pointer->connection_id]->hold_request = FALSE;

          /* Stop the timer */
          MN_stop_cc_timer(pointer->connection_id);
   
          /* Find out which call the MNCC_Call_Hold_CNF was about */
          MN_MULTICALL_LOG(MN_read_multicall_status()); /* Refresh the Multicall_Status array */
          Multicall_Status_index = (int)MN_whereami(pointer->connection_id);

          /*the other has been already retrieved, update the auxiliary states */
          /*Point at the other call */
          Multicall_Status_index = (Multicall_Status_index + 1) % 2;
          
          if(Multicall_Status[Multicall_Status_index].call_is_mpty == TRUE)
          {
            /*  COMPLETE THE RETRIEVE OPERATION FOR THE MPTY CALL  */
            for(index = 0; index < Multicall_Status[Multicall_Status_index].id_index; index++)
            {
              (void)MN_manage_CC_auxiliary_state(Multicall_Status[Multicall_Status_index].
                   connection_id[index], CCE_RETRIEVE_MPTY_CONF);
              
              
            }
          }
          else
          {
            connection_id = Multicall_Status[Multicall_Status_index].connection_id[0];
            s_respond_to_event = MN_manage_CC_auxiliary_state (connection_id,
                                                             CCE_CALL_RETRIEVE_CONF);/*c_call_event */
            if (s_respond_to_event == TRUE)
            {
              /* clear the ss pending requests */   
              mn_call_information_p[connection_id]->ss_request_pending= CM_NONE_PENDING;
              MN_stop_cc_timer(connection_id);
            }
            else
            {
              MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: MNCC_CALL_RETRIEVE_CNF message received in wrong state");
            }
          }

          cm_ss_error.present = FALSE;
          MN_put_cm_manage_calls_conf(TRUE,     /* call_ss_success */
#if defined(FEATURE_DUAL_SIM )
                                      mn_call_information_p[pointer->connection_id]->as_id, 
#endif    

                                      cm_ss_error);
          
        }
        else
        {
          MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: MNCC_CALL_HOLD_CNF message received in wrong state");
        }

      }
      else if (mn_alternate_proc_status.status == MN_SS_ALT_NACKED)
      {
        s_respond_to_event = MN_manage_CC_auxiliary_state(pointer->connection_id,
                               CCE_CALL_HOLD_REJ);  /*  c_call_event */
        if (s_respond_to_event == TRUE)
        {
            /* clear the ss pending requests */   
            mn_call_information_p[pointer->connection_id]->ss_request_pending= CM_NONE_PENDING;
            MN_stop_cc_timer(pointer->connection_id);

            mn_alternate_proc_status.is_alternate_procedure = FALSE;
            mn_alternate_proc_status.status = MN_SS_ALT_NONE_AWAITED;

#ifdef FEATURE_CCBS
            memset (&cm_ss_error, 0x00, sizeof(ie_cm_ss_error_T));
#endif /* FEATURE_CCBS */

            cm_ss_error.present = TRUE;
            cm_ss_error.error_code_tag = MN_SS_ERROR_TAG;
            cm_ss_error.error_code = MN_CALL_HOLD_REJ;

            MN_MULTICALL_LOG(MN_read_multicall_status());  
            MN_put_cm_manage_calls_conf(FALSE,    /*  call_ss_success */
#if defined(FEATURE_DUAL_SIM )
                                        mn_call_information_p[pointer->connection_id]->as_id, 
#endif    

                                        cm_ss_error);
        }
        else
        {
            MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: MNCC_CALL_HOLD_CNF message received in wrong state");
        }
      }
      MSG_HIGH_DS_1(MN_SUB,"=MNCNM= ALT Proc is on ,status = %d",mn_alternate_proc_status.status);

   } /* alternate procedure */
   else
   {
     /* Stop the timer */
     MN_stop_cc_timer(pointer->connection_id);
     /* Clear out any remaining DTMF requests and send the DTMF CNF to CM  */
     MN_flush_queued_dtmf_requests(pointer->connection_id);

     /* Update CC auxiliary state for this connection */
     s_respond_to_event = MN_manage_CC_auxiliary_state(pointer->connection_id, CCE_CALL_HOLD_CONF);   /* c_call_event value */

     if (s_respond_to_event == TRUE)
     {
      /* clear the ss pending requests */   
      mn_call_information_p[pointer->connection_id]->ss_request_pending= CM_NONE_PENDING;
       
      /* Find out which call the MNCC_Call_Hold_CNF was about */

      MN_MULTICALL_LOG(MN_read_multicall_status()); /* Refresh the Multicall_Status array */

      Multicall_Status_index = (int)MN_whereami(pointer->connection_id);

      /*check if the other call is held */

      /*Point at the other call */
      Multicall_Status_index = (Multicall_Status_index + 1) % 2;

      if (Multicall_Status[Multicall_Status_index].call_activity != CALL_ACTIVE)
      {
         if (Multicall_Status[Multicall_Status_index].call_activity == MO_CALL_PENDING)
         { /* resume the mo call setup */

            connection_id = 
                Multicall_Status[Multicall_Status_index].connection_id[0];

            /* reset the mo call pending flag */
            mn_call_information_p[connection_id]->mo_setup_pending = FALSE;

 
            /* Notify CM of SS call-related service before start the MO call */
                  MN_put_cm_manage_calls_conf(TRUE, 
#if defined(FEATURE_DUAL_SIM )
                                        mn_call_information_p[pointer->connection_id]->as_id, 
#endif    
                                        cm_ss_error);

            /* Override the use of UL DTX between call setup and connect/connect_ack */
#ifdef FEATURE_GSM
#ifdef FEATURE_DUAL_SIM
            if(MNCNM_IS_SINGLE_STACK)
            {
               geran_grr_dtx_override_on_per_subs(mn_as_id);
            }
#ifdef FEATURE_DUAL_ACTIVE
            else
            {
              geran_grr_dtx_override_on_per_subs((sys_modem_as_id_e_type)mn_dsda_as_id);
            }
#endif
#else
            geran_grr_dtx_override_on();
#endif
#endif        
            /* normal MO calls */
            MN_put_MNCC_SETUP_REQ(connection_id);
         }
         else
         {
            connection_id = Multicall_Status[2].connection_id[0];
            /* Get the waiting call */
            if ((no_auto_answer_on_hold == FALSE) && ( Multicall_Status[2].call_activity == CALL_WAITING && 
                 (cc_transaction_information_p[connection_id] != NULL) &&
                 CC_STATE_U7 == cc_transaction_information_p[connection_id]->cc_state ))
            {
                if(mn_call_information_p[pointer->connection_id]->hold_request == CM_2_SEND_HOLD )
                {
                  /* resume the MT call setup */
                  mn_call_information_p[connection_id]->
                                     mt_call_accept_cause = CM_2_SEND_REQ;

                  MN_put_MNCC_SETUP_RES(connection_id,
                   &(null_connected_subaddress));
                }
                else if(mn_call_information_p[connection_id]->hold_request == CM_2_X_SEND_HOLD )
                {
                  mn_call_information_p[connection_id]->
                                     mt_call_accept_cause = CM_2_X_SEND_REQ;

                  MN_put_MNCC_SETUP_RES(connection_id,
                   &(null_connected_subaddress));
                }
                else
                {
                  cm_ss_error.error_code_tag = 0;
                  cm_ss_error.error_code = 0;

                  MN_put_cm_manage_calls_conf ( TRUE, 
#if defined(FEATURE_DUAL_SIM )
                                             mn_call_information_p[pointer->connection_id]->as_id, 
#endif    
                                             cm_ss_error
                                             );

                }
            }
            else
            {
#ifdef FEATURE_NAS_VOC_REL_AFTER_CALL_HOLD 
            
               /* Disconnect TCH from Vocoder */
               MN_disconnect_TCH(MN_VOCODER_SWITCH); /* Releasing vocoder when no active call is there to support simultaneous CS/PS call */

#endif /* FEATURE_NAS_VOC_REL_AFTER_CALL_HOLD */       

               /* There was no other call */
               cm_ss_error.error_code_tag = 0;
               cm_ss_error.error_code = 0;

               /* Send success back to CM */
               MN_put_cm_manage_calls_conf ( TRUE, 
#if defined(FEATURE_DUAL_SIM )
                                             mn_call_information_p[pointer->connection_id]->as_id, 
#endif    
                                             cm_ss_error
                                             );


            } /* CALL_WAITING */

         } /* MO_CALL_PENDING */

       } /* CALL_ACTIVE */

       mn_call_information_p[pointer->connection_id]->hold_request = FALSE;

     }
     else
     {
       MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: MNCC_CALL_HOLD_CNF message received in wrong state");
     }
   }
}


/*
 *   Function name : MN_process_MNCC_CALL_HOLD_REJ
 *   ---------------
 *
 *   Description:
 *   ------------
 * Process the network response to a call hold. This will mean
 * stopping any CALL SS timer for the particular transaction, update all auxiliary states
 * for those transactions affected by the hold request, 
 * 
 * 
 *
 *
 *  Parameters:
 *  -----------
 *  IN message
 *
 *  Returns:
 *
 *
 */

VOID_FUNC   MN_process_MNCC_CALL_HOLD_REJ(IMH_T   *message)

{

   /* Locals */

   MNCC_CALL_HOLD_REJ_T     *pointer;
   boolean                   s_respond_to_event;
   ie_cm_ss_error_T            cm_ss_error = { 0        };
   ie_cm_cc_cause_T       cm_cc_cause;
   cm_bearer_capability_T bearer_capability;
   int                       Multicall_Status_index,index;
   connection_id_T              connection_id;


   /* align pointers */

   pointer = (MNCC_CALL_HOLD_REJ_T *)((void *)message);

   if (mn_alternate_proc_status.is_alternate_procedure == TRUE)
   {
     s_respond_to_event = MN_manage_CC_auxiliary_state(pointer->connection_id,
                          CCE_CALL_HOLD_REJ);      /*  c_call_event */
     if (s_respond_to_event == TRUE)
     {
       /* clear the ss pending requests */   
       mn_call_information_p[pointer->connection_id]->ss_request_pending= CM_NONE_PENDING;
       mn_call_information_p[pointer->connection_id]->hold_request = FALSE;
      
       MN_stop_cc_timer(pointer->connection_id);
      
       if (mn_alternate_proc_status.status == MN_SS_ALT_HOLD_ACK_RETRIEVE_ACK_AWAITED)
       {
         mn_alternate_proc_status.status = MN_SS_ALT_NACKED;
       }
       else if (mn_alternate_proc_status.status == MN_SS_ALT_NACKED || 
                mn_alternate_proc_status.status == MN_SS_ALT_HOLD_ACK_AWAITED)
       {
         if (mn_alternate_proc_status.status == MN_SS_ALT_HOLD_ACK_AWAITED)
         {
           Multicall_Status_index = (int)MN_whereami(pointer->connection_id);

           /*the other has been already retrieved, update the auxiliary states */
           /*Point at the other call */
           Multicall_Status_index = (Multicall_Status_index + 1) % 2;

           if(Multicall_Status[Multicall_Status_index].call_is_mpty == TRUE)
           {
              /*  REVERSE THE HOLD CALL OPERATION FOR THE MPTY CALL  */
              for(index = 0; index < Multicall_Status[Multicall_Status_index].id_index; index++)
              {
                  (void)MN_manage_CC_auxiliary_state(Multicall_Status[Multicall_Status_index].
                                     connection_id[index], CCE_RETRIEVE_MPTY_REJ);
              }
           }
           else
           {
              connection_id = Multicall_Status[Multicall_Status_index].connection_id[0];
              s_respond_to_event = MN_manage_CC_auxiliary_state (connection_id,
                                                             CCE_CALL_RETRIEVE_REJ);/*c_call_event */
              if (s_respond_to_event == TRUE)
              {
                 MN_stop_cc_timer(connection_id);
              }
              else
              {
                 MSG_ERROR_DS_1(MN_SUB,"=MNCNM= ERROR:ALT Proc on,CC Aux state of %d call in wrong state",connection_id);
              }
           }
         }

         mn_alternate_proc_status.is_alternate_procedure = FALSE;
         mn_alternate_proc_status.status = MN_SS_ALT_NONE_AWAITED;

#ifdef FEATURE_CCBS
         memset (&cm_ss_error, 0x00, sizeof(ie_cm_ss_error_T));
#endif /* FEATURE_CCBS */

         cm_ss_error.present = TRUE;
         cm_ss_error.error_code_tag = MN_SS_ERROR_TAG;
         cm_ss_error.error_code = MN_CALL_HOLD_REJ;

         MN_MULTICALL_LOG(MN_read_multicall_status());

        MN_put_cm_manage_calls_conf(FALSE,    /*  call_ss_success */
#if defined(FEATURE_DUAL_SIM )
                           mn_call_information_p[pointer->connection_id]->as_id, 
#endif    
                                     cm_ss_error);
       }
     }
     else
     {
       MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Single call in wrong state");
     }
   }
   else
   {
     s_respond_to_event = MN_manage_CC_auxiliary_state(pointer->connection_id,
         CCE_CALL_HOLD_REJ);      /*  c_call_event */
     if   (s_respond_to_event == TRUE)
     {

      /* clear the ss pending requests */   
      mn_call_information_p[pointer->connection_id]->ss_request_pending= CM_NONE_PENDING;
      mn_call_information_p[pointer->connection_id]->hold_request = FALSE;
      
#ifdef FEATURE_CCBS
      memset (&cm_ss_error, 0x00, sizeof(ie_cm_ss_error_T));
#endif /* FEATURE_CCBS */

      cm_ss_error.present = TRUE;
      cm_ss_error.error_code_tag = MN_SS_ERROR_TAG;
      cm_ss_error.error_code = MN_CALL_HOLD_REJ;

      MN_put_cm_manage_calls_conf(FALSE,    /*  call_ss_success */
#if defined(FEATURE_DUAL_SIM )
           mn_call_information_p[pointer->connection_id]->as_id, 
#endif    
          cm_ss_error);

      MN_stop_cc_timer(pointer->connection_id);


     }
     else
     {
      MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Single call in wrong state");
     }

     /* Find out which call the MNCC_Call_Hold_REJ was about */
     /* Refresh the Multicall_Status array */

     MN_MULTICALL_LOG(MN_read_multicall_status());

     Multicall_Status_index = (int)MN_whereami(pointer->connection_id);

      /*check if the other call is a MO call pending */

      /*Point at the other call */

     Multicall_Status_index = (Multicall_Status_index + 1) % 2;

     if (Multicall_Status[Multicall_Status_index].call_activity == 
             MO_CALL_PENDING)
         /* reject the mo call setup */
     {
      connection_id = 
                Multicall_Status[Multicall_Status_index].connection_id[0];

      /* reset the mo call pending flag */

      mn_call_information_p[connection_id]->mo_setup_pending = FALSE;

      /* Set up null data structures for the message */

      memset(&(cm_cc_cause),'\0',sizeof(ie_cm_cc_cause_T));
      memset(&(bearer_capability),'\0',sizeof(cm_bearer_capability_T));

      MN_put_cm_mo_call_conf(connection_id,
                                 FALSE,      /* cnm_service_success */
                                 bearer_capability,
                                 bearer_capability,
                                 cm_cc_cause, 
#if defined(FEATURE_DUAL_SIM )
                                 mn_call_information_p[connection_id]->as_id,
#endif                                                               
#if defined(FEATURE_MULTIMEDIA_FALLBACK) || defined(FEATURE_REL6)
                                 NULL,
                                 NULL,
                                 NULL
#else
                                 NULL
#endif
                            );
      MN_clear_call_data(connection_id);
     }

   }
}



/*
 *   Function name : MN_process_MNCC_CALL_RETRIEVE_CNF
 *   ---------------
 *
 *   Description:
 *   ------------
 * Process the network response to a call retrieve. This will mean
 * stopping any CALL SS timer for the particular transaction, update all auxiliary states
 * for those transactions affected by the retrieve request, 
 * 
 * 
 *
 *
 *  Parameters:
 *  -----------
 *  IN message
 *
 *  Returns:
 *
 *
 */

VOID_FUNC   MN_process_MNCC_CALL_RETRIEVE_CNF(IMH_T   *message)

{

   /* Locals */

   MNCC_CALL_RETRIEVE_CNF_T     *pointer;
   boolean                      s_respond_to_event;
   ie_cm_ss_error_T             cm_ss_error = { 0        };
   connection_id_T              connection_id;
   int                          Multicall_Status_index;
   int                          index;

   /* align pointers */

   pointer = (MNCC_CALL_RETRIEVE_CNF_T *)((void *)message);


   if (mn_alternate_proc_status.is_alternate_procedure == TRUE)
   {
      if (mn_alternate_proc_status.status == MN_SS_ALT_HOLD_ACK_RETRIEVE_ACK_AWAITED)
      {
        mn_alternate_proc_status.status = MN_SS_ALT_HOLD_ACK_AWAITED;
      }
      else if (mn_alternate_proc_status.status == MN_SS_ALT_RETRIEVE_ACK_AWAITED)
      {
        mn_alternate_proc_status.status = MN_SS_ALT_NONE_AWAITED;
        mn_alternate_proc_status.is_alternate_procedure = FALSE;

        s_respond_to_event = MN_manage_CC_auxiliary_state (pointer->connection_id,
                              CCE_CALL_RETRIEVE_CONF);  /*c_call_event */

        if (s_respond_to_event == TRUE)
        {
          /* clear the ss pending requests */   
          mn_call_information_p[pointer->connection_id]->ss_request_pending= CM_NONE_PENDING;
          MN_stop_cc_timer(pointer->connection_id);

          /* Now the other ACK (HOLD ACK) is already received, update the Auxiliary state accordingly */
          MN_MULTICALL_LOG(MN_read_multicall_status()); /* Refresh the Multicall_Status array */
          Multicall_Status_index = (int)MN_whereami(pointer->connection_id);

          /*Point at the other call */
          Multicall_Status_index = (Multicall_Status_index + 1) % 2;

          if(Multicall_Status[Multicall_Status_index].call_is_mpty == TRUE)
          {
            for(index = 0; index < Multicall_Status[Multicall_Status_index].id_index; index++)
            {
              (void)MN_manage_CC_auxiliary_state(Multicall_Status[Multicall_Status_index].connection_id[index], CCE_HOLD_MPTY_CONF);

              /*  FLUSHING THE DTMF REQs OF EACH PARTY OF THE MPTY CALL  */         
              MN_flush_queued_dtmf_requests(Multicall_Status[Multicall_Status_index].connection_id[index]);
            }
          }
          else
          {
              connection_id = Multicall_Status[Multicall_Status_index].connection_id[0];
              /* Clear out any remaining DTMF requests and send the DTMF CNF to CM  */
              MN_flush_queued_dtmf_requests(connection_id);
              /* Update CC auxiliary state for this connection */
              s_respond_to_event = MN_manage_CC_auxiliary_state(connection_id,
                                                            CCE_CALL_HOLD_CONF);/* c_call_event value */
              if (s_respond_to_event == TRUE)
              {
                 MN_stop_cc_timer(connection_id);
                          /* clear the ss pending requests */   
                 mn_call_information_p[connection_id]->ss_request_pending= CM_NONE_PENDING;
                 mn_call_information_p[connection_id]->hold_request = FALSE;
              }
          }
          
          cm_ss_error.present = FALSE;
          MN_put_cm_manage_calls_conf(TRUE,     /* call_ss_success */
#if defined(FEATURE_DUAL_SIM )
                      mn_call_information_p[pointer->connection_id]->as_id, 
#endif    
                                        cm_ss_error);
          
        }
        else
        {
          MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Call retrieve call in wrong state");
        }
      }
      else if (mn_alternate_proc_status.status == MN_SS_ALT_NACKED)
      {
          s_respond_to_event = MN_manage_CC_auxiliary_state(pointer->connection_id,
                               CCE_CALL_RETRIEVE_REJ);  /*  c_call_event */
          if (s_respond_to_event == TRUE)
          {
            /* clear the ss pending requests */   
            mn_call_information_p[pointer->connection_id]->ss_request_pending= CM_NONE_PENDING;
            MN_stop_cc_timer(pointer->connection_id);

            mn_alternate_proc_status.is_alternate_procedure = FALSE;
            mn_alternate_proc_status.status = MN_SS_ALT_NONE_AWAITED;

            cm_ss_error.present = TRUE;
            cm_ss_error.error_code_tag = MN_SS_ERROR_TAG;
            cm_ss_error.error_code = MN_CALL_RETRIEVE_REJ;

            MN_MULTICALL_LOG(MN_read_multicall_status()); 
            MN_put_cm_manage_calls_conf(FALSE,    /*  call_ss_success */
#if defined(FEATURE_DUAL_SIM )
           mn_call_information_p[pointer->connection_id]->as_id, 
#endif    
                                      cm_ss_error);
          }
          else
          {
            MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Retrieved call in wrong state, ALT proc is on");
          }
      }
      MSG_HIGH_DS_1(MN_SUB,"=MNCNM= ALT Proc is on ,status = %d",mn_alternate_proc_status.status);
   }
   else
   {

     s_respond_to_event = MN_manage_CC_auxiliary_state (pointer->connection_id,
                          CCE_CALL_RETRIEVE_CONF);  /*c_call_event */

     if (s_respond_to_event == TRUE)
     {

#ifdef FEATURE_NAS_VOC_REL_AFTER_CALL_HOLD 

       /* For both GSM & UMTS, Use DEFAULT_RAB_ID below. */
       /* Will change when we implement the Multicall    */
       /* Feature (Not the Multicall SS)                 */

       MN_MULTICALL_LOG(MN_read_multicall_status()); /* update the call_activity of Multicall_Status */

       if (tch_user_connections_store[DEFAULT_RAB_ID].OTA_channel_connected == TRUE)
       {
          MSG_HIGH_DS_1(MN_SUB,"=MNCNM= =MN= BC.ITC  : %d\n ",mn_call_information_p[pointer->connection_id]->bearer_capability_1.information_transfer_capability );
  
          /* Connect the appropriate traffic channel */
  
          MN_connect_TCH(mn_call_information_p[pointer->connection_id]->bearer_capability_1.
                 information_transfer_capability); /* Re-acquisition of the vocoder is required. Vocoder was released by NAS at the time of HOLD CNF */
       }

#endif /* FEATURE_NAS_VOC_REL_AFTER_CALL_HOLD */

       /* clear the ss pending requests */   
       mn_call_information_p[pointer->connection_id]->ss_request_pending= CM_NONE_PENDING;

       cm_ss_error.present = FALSE;
       MN_put_cm_manage_calls_conf(TRUE,     /* call_ss_success */
#if defined(FEATURE_DUAL_SIM )
           mn_call_information_p[pointer->connection_id]->as_id, 
#endif    
                                   cm_ss_error);

       MN_stop_cc_timer(pointer->connection_id);
     }
     else
     {
        MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Call retrieve call in wrong state");
     }
   }

}



/*
 *   Function name : MN_process_MNCC_CALL_RETRIEVE_REJ
 *   ---------------
 *
 *   Description:
 *   ------------
 * Process the network response to a call retrieve. This will mean
 * stopping any CALL SS timer for the particular transaction, update all auxiliary states
 * for those transactions affected by the retrieve request, 
 * 
 * 
 *
 *
 *  Parameters:
 *  -----------
 *  IN message
 *
 *  Returns:
 *
 *
 */

VOID_FUNC   MN_process_MNCC_CALL_RETRIEVE_REJ(IMH_T   *message)

{

   /* Locals */

   MNCC_CALL_RETRIEVE_REJ_T     *pointer;
   boolean                   s_respond_to_event;
   ie_cm_ss_error_T            cm_ss_error={0};
   connection_id_T              connection_id;
   int                          Multicall_Status_index,index;

   /* align pointers */

   pointer = (MNCC_CALL_RETRIEVE_REJ_T *)((void *)message);

   if (mn_alternate_proc_status.is_alternate_procedure == TRUE)
   {
      s_respond_to_event = MN_manage_CC_auxiliary_state(pointer->connection_id,
                           CCE_CALL_RETRIEVE_REJ);  /*  c_call_event */
      if (s_respond_to_event == TRUE)
      {
        /* clear the ss pending requests */   
        mn_call_information_p[pointer->connection_id]->ss_request_pending= CM_NONE_PENDING;
        MN_stop_cc_timer(pointer->connection_id);

        if (mn_alternate_proc_status.status == MN_SS_ALT_HOLD_ACK_RETRIEVE_ACK_AWAITED)
        {
          mn_alternate_proc_status.status = MN_SS_ALT_NACKED;
        }
        else if (mn_alternate_proc_status.status == MN_SS_ALT_NACKED || 
                 mn_alternate_proc_status.status == MN_SS_ALT_RETRIEVE_ACK_AWAITED)
        {
          if (mn_alternate_proc_status.status == MN_SS_ALT_RETRIEVE_ACK_AWAITED)
          {
            Multicall_Status_index = (int)MN_whereami(pointer->connection_id);

            /*the ACK for other call has been already Received, update the auxiliary states */
            /*Point at the other call */
            Multicall_Status_index = (Multicall_Status_index + 1) % 2;

            if(Multicall_Status[Multicall_Status_index].call_is_mpty == TRUE)
            {
               /*  REVERSE THE HOLD CALL OPERATION FOR THE MPTY CALL  */
               for(index = 0; index < Multicall_Status[Multicall_Status_index].id_index; index++)
               {
                   (void)MN_manage_CC_auxiliary_state(
                                         Multicall_Status[Multicall_Status_index].connection_id[index], 
                                            CCE_HOLD_MPTY_REJ);
               }
            }
            else
            {
              connection_id = Multicall_Status[Multicall_Status_index].connection_id[0];
              s_respond_to_event = MN_manage_CC_auxiliary_state (connection_id,
                                                             CCE_CALL_HOLD_REJ);/*c_call_event */
              if (s_respond_to_event == TRUE)
              {
                MN_stop_cc_timer(connection_id);   
              }
              else
              {
                MSG_ERROR_DS_1(MN_SUB,"=MNCNM= ERROR:ALT Proc on,CC Aux state of %d call in wrong state",connection_id);
              }
            }
          }

          mn_alternate_proc_status.is_alternate_procedure = FALSE;
          mn_alternate_proc_status.status = MN_SS_ALT_NONE_AWAITED;

          cm_ss_error.present = TRUE;
          cm_ss_error.error_code_tag = MN_SS_ERROR_TAG;
          cm_ss_error.error_code = MN_CALL_RETRIEVE_REJ;

          MN_MULTICALL_LOG(MN_read_multicall_status());

          MN_put_cm_manage_calls_conf(FALSE,    /*  call_ss_success */
#if defined(FEATURE_DUAL_SIM )
                mn_call_information_p[pointer->connection_id]->as_id, 
#endif    
                                      cm_ss_error);
        }
      }
   }
   else
   {
      s_respond_to_event = MN_manage_CC_auxiliary_state(pointer->connection_id,
                           CCE_CALL_RETRIEVE_REJ);  /*  c_call_event */
      if (s_respond_to_event == TRUE)
      {
        /* clear the ss pending requests */   
        mn_call_information_p[pointer->connection_id]->ss_request_pending= CM_NONE_PENDING;

        cm_ss_error.present = TRUE;
        cm_ss_error.error_code_tag = MN_SS_ERROR_TAG;
        cm_ss_error.error_code = MN_CALL_RETRIEVE_REJ;

        MN_put_cm_manage_calls_conf(FALSE,    /*  call_ss_success */
#if defined(FEATURE_DUAL_SIM )
           mn_call_information_p[pointer->connection_id]->as_id, 
#endif    
                                    cm_ss_error);
        MN_stop_cc_timer(pointer->connection_id);
      }
      else
      {
        MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Retrieved call in wrong state");

      }
   }

}





/*
 *   Function name : MN_process_MNCC_SETUP_CNF
 *   ---------------
 *
 *   Description:
 *   ------------
 *  This function processes the MNCC_SETUP_CNF message 
 *   which comes from CC to MN indicating a successfull MO
 *   call setup.
 *   
 *  
 *
 *  Parameters:
 *  -----------
 *  IN message
 *
 *  Returns:
 *
 *
 */


VOID_FUNC    MN_process_MNCC_SETUP_CNF(IMH_T    *message)

{
   /* Locals */

   MNCC_SETUP_CNF_T         *pointer;
   connection_id_T          connection_id;
   mn_call_information_T    *call_data;
   boolean                  log_packet_sent = FALSE;
   byte                     call_type = VOICE_CALL;
   cm_bearer_capability_T   bearer_capability;

   /* Align pointers */

   pointer = (MNCC_SETUP_CNF_T *)((void *)message);

   connection_id = MN_get_MNCC_SETUP_CNF(pointer);

   if(connection_id >= MAXNO_CC_TRANSACTIONS)
   {
      MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Invalid connection id");

   }
   else if(mn_call_information_p[connection_id] == NULL)
   {
      MSG_HIGH_DS_1(MN_SUB,"=MNCNM= cannot process MNCC_SETUP_CNF as mn_call_information_p[%d] is NULL",
                        connection_id);
   }
   else
   {
      /* Disable the UL DTX override */
#ifdef FEATURE_GSM
#ifdef FEATURE_DUAL_SIM
      if(MNCNM_IS_SINGLE_STACK)
      {
        geran_grr_dtx_override_off_per_subs(mn_as_id);
      }
#ifdef FEATURE_DUAL_ACTIVE
      else
      {
        geran_grr_dtx_override_off_per_subs((sys_modem_as_id_e_type)mn_dsda_as_id);
      }
#endif
#else
      geran_grr_dtx_override_off();
#endif
#endif
      /* index the call data */

      call_data =  mn_call_information_p[connection_id];


      /* If the MNCC_SETUP_CNF message contains connected number information,
                       send it to the cm in the cm_connected_number_ind message */

      if ((call_data->connected_subaddress_present == TRUE ) ||
          (call_data->connected_number_present == TRUE ))
      {
         MN_put_cm_connected_number_ind(connection_id);
      }

      /* 
      * Find call type 
      */

     if(call_data->active_bearer_capability == 0)
     {
         memscpy(&bearer_capability, 
             sizeof(cm_bearer_capability_T),
             &(call_data->bearer_capability_1), 
             sizeof(cm_bearer_capability_T));
     }
     else
     {
         memscpy(&bearer_capability, sizeof(cm_bearer_capability_T) ,&(call_data->bearer_capability_2), 
             sizeof(cm_bearer_capability_T));
     }

     /* If BC transfer capability is NOT speech */
     if((bearer_capability.information_transfer_capability != ITC_SPEECH) &&
        (bearer_capability.information_transfer_capability != ITC_AUX_SPEECH))
     {
         call_type = DATA_CALL;

         /* Send log packet to DIAG */

         log_packet_sent = mn_log_cs_connection_bc_info(connection_id, bearer_capability);

         if (log_packet_sent == FALSE)
         {
             MSG_LOW_DS_0(MN_SUB,"=MNCNM= Cannot send CS connection BC info log packet to DIAG");
         }
     }


     /* Send log packet to DIAG */

     log_packet_sent = mn_log_cs_call_setup_info(connection_id, call_type, UE_TO_CN_DIRECTION);

     if (log_packet_sent == FALSE)
     {
         MSG_LOW_DS_0(MN_SUB,"=MNCNM= Cannot send CS call setup log packet to DIAG");
     }

     /* If the speech channel has not yet been connected for the ringing tone/
                   call announcements, do it now */

     if ((MN_check_for_inband_message(connection_id) == FALSE)||
         ((call_data->bearer_capability_1.information_transfer_capability != ITC_SPEECH) &&
          (call_data->bearer_capability_1.information_transfer_capability != ITC_AUX_SPEECH)))
     {
        MSG_HIGH_DS_1(MN_SUB,"=MNCNM= =MN= BC.ITC  : %d\n ",call_data->bearer_capability_1.information_transfer_capability );

        /* Connect the appropriate traffic channel */

        MN_connect_TCH(call_data->bearer_capability_1.
               information_transfer_capability);

     }

     /* Reset the MO SETUP pending flag = MO call SETUP completed */

     call_data->mo_setup_pending = FALSE;

     /* Tell cm that the connection has been made */

     MN_put_cm_call_connected_ind(connection_id);


     /* Reply to the network */

     MN_put_MNCC_SETUP_COMPL_REQ(connection_id);
   }

}

/*
 *   Function name : MN_process_MNCC_START_DTMF_CNF
 *   ---------------
 *
 *   Description:
 *   ------------
 *  This function processes the MNCC_START_DTMF_CNF message 
 *   which comes from CC to MN indicating a successfull DTMF
 *   tone setup.
 *   
 *  
 *
 *  Parameters:
 *  -----------
 *  IN message
 *
 *  Returns:
 *
 *
 */


VOID_FUNC    MN_process_MNCC_START_DTMF_CNF(IMH_T    *message)

{

   MNCC_START_DTMF_CNF_T    *pointer;
#ifdef FEATURE_DUAL_SIM
     ie_cm_cc_reject_T      cm_cc_reject;
     sys_modem_as_id_e_type as_id = SYS_MODEM_AS_ID_1;
#endif
   pointer =(MNCC_START_DTMF_CNF_T *)((void *)message);

#ifdef FEATURE_DUAL_SIM

   if(MNCNM_IS_SINGLE_STACK)
   {

     if (mn_call_information_p[pointer->connection_id] != NULL)
     {
       as_id = mn_call_information_p[pointer->connection_id]->as_id;
     }
     else if(mn_as_id != SYS_MODEM_AS_ID_NONE)
     {
       as_id = mn_as_id;
     }
  
     cm_cc_reject.present = FALSE;
    }
     MN_put_cm_start_dtmf_conf(pointer->connection_id, 
                               cm_cc_reject, 
                               as_id,
                               pointer->dtmf_success
                               );  
#else 

      MN_put_cm_start_dtmf_conf(pointer->connection_id,
       pointer->dtmf_success);
#endif

}

/*
 *   Function name : MN_process_MNCC_STOP_DTMF_CNF
 *   ---------------
 *
 *   Description:
 *   ------------
 *  This function processes the MNCC_STOP_DTMF_CNF message 
 *   which comes from CC to MN indicating a successfull DTMF
 *   tone cancellation.
 *   
 *  
 *
 *  Parameters:
 *  -----------
 *  IN message
 *
 *  Returns:
 *
 *
 */


VOID_FUNC    MN_process_MNCC_STOP_DTMF_CNF(IMH_T    *message)

{

   MNCC_STOP_DTMF_CNF_T    *pointer;
#ifdef FEATURE_DUAL_SIM
   ie_cm_cc_reject_T        cm_cc_reject;
   sys_modem_as_id_e_type as_id = SYS_MODEM_AS_ID_1;
#endif
   pointer =(MNCC_STOP_DTMF_CNF_T *)((void *)message);


#if defined(FEATURE_DUAL_SIM )

   if(MNCNM_IS_SINGLE_STACK)
   {
     if (mn_call_information_p[pointer->connection_id] != NULL)
     {
       as_id = mn_call_information_p[pointer->connection_id]->as_id;
     }
     else if(mn_as_id != SYS_MODEM_AS_ID_NONE)
     {
       as_id = mn_as_id;
     }
  
     cm_cc_reject.present = FALSE;
   }
     MN_put_cm_stop_dtmf_conf(pointer->connection_id, 
                               cm_cc_reject, 
                               as_id
                               );  

#else
     MN_put_cm_stop_dtmf_conf(pointer->connection_id);
#endif
   


}

/***********************************************************************
 *   Function name : MN_flush_queued_dtmf_requests
 *   ---------------
 *
 *   Description:
 *   ------------
 *  This function dumps the Queued DTMF requests from CM belonging to a call,
 *  with default return status to CM, without sending the DTMF request OTA.
 *  EX: when putting the call on hold.
 *  
 *  Parameters:
 *  -----------
 *  Connection ID.
 *
 *  Returns:
 *
 ***********************************************************************/
VOID_FUNC    MN_flush_queued_dtmf_requests(connection_id_T  connection_id)
{
   dtmf_queue_entry_T          dtmf_command;
   boolean                     send_stop_dtmf = FALSE;   
#if defined(FEATURE_DUAL_SIM )
   ie_cm_cc_reject_T        cm_cc_reject;
   cm_cc_reject.present = FALSE;
#endif
   /* Clear out any remaining DTMF requests and send the DTMF CNF to CM  */
   MSG_HIGH_DS_1(MN_SUB,"=MNCNM= Clearing DTMF Q for Call %d as it is put on HOLD", connection_id);

   /* Incase we are waiting for a DTMF ACK from the n/w,and the call is now put on hold, 
      we will ourselves send the CNF to the CM and just drop the DTMF ACK received from the n/w
   */
   if(WAIT_FOR_START_DTMF_ACK == (CNM_get_dtmf_state(connection_id)))
   {
     MN_put_cm_start_dtmf_conf(connection_id, 
#if defined(FEATURE_DUAL_SIM )
                               cm_cc_reject, 
                               mn_call_information_p[connection_id]->as_id,
#endif
                               FALSE
                            );  
   }
   else if(WAIT_FOR_STOP_DTMF_ACK == (CNM_get_dtmf_state(connection_id)))
   {
     MN_put_cm_stop_dtmf_conf(connection_id 
#if defined(FEATURE_DUAL_SIM )
                             ,cm_cc_reject 
                             ,mn_call_information_p[connection_id]->as_id
#endif
                             );  
   }
   else
   {
     /* state id DTMF_IDLE so do nothing*/
   }

   /* Check the first msg in the queue. If it is a STOP_DTMF_REQ, it implies that START_DTMF_REQ 
      is being processed/has already been sent to the n/w.So send the STOP_DTMF_REQ msg
      to CNM as this also has to be sent to the n/w.
   */
   if(TRUE == CNM_pop_dtmf_queue_entry(connection_id, &dtmf_command))
   {
      if(MNCC_START_DTMF_REQ == dtmf_command.start_dtmf_queue_entry.message_header.message_id)
      {
         MN_put_cm_start_dtmf_conf(connection_id, 
#if defined(FEATURE_DUAL_SIM )
                             cm_cc_reject, 
                             mn_call_information_p[connection_id]->as_id,
#endif
                             FALSE
                             );  
      }
      else if(MNCC_STOP_DTMF_REQ == dtmf_command.start_dtmf_queue_entry.message_header.message_id)
      {
          MN_put_cm_stop_dtmf_conf(connection_id
#if defined(FEATURE_DUAL_SIM )
                             ,cm_cc_reject
                             ,mn_call_information_p[connection_id]->as_id
#endif
                             );  
         send_stop_dtmf = TRUE;    
      }
      else
      {
         ERR("Unexpected msg: received message_id = %d\n",
             (word) (dtmf_command.start_dtmf_queue_entry.
                    message_header.message_id), 0, 0);
      }
   }


   /* For all the pending DTMF requests, send a CNF to CM */
   while(TRUE == CNM_pop_dtmf_queue_entry(connection_id, &dtmf_command ))
   {
      if(MNCC_START_DTMF_REQ == dtmf_command.start_dtmf_queue_entry.message_header.message_id)
      {
         MN_put_cm_start_dtmf_conf(connection_id, 
#if defined(FEATURE_DUAL_SIM )
                             cm_cc_reject, 
                             mn_call_information_p[connection_id]->as_id,
#endif
                             FALSE
                             );  
      }
      else if(MNCC_STOP_DTMF_REQ == dtmf_command.start_dtmf_queue_entry.message_header.message_id)
      {
         MN_put_cm_stop_dtmf_conf(connection_id
#if defined(FEATURE_DUAL_SIM )
                             ,cm_cc_reject
                             ,mn_call_information_p[connection_id]->as_id
#endif
                             );  
      }
      else
      {
         ERR("Unexpected msg: received message_id = %d\n",
             (word) (dtmf_command.start_dtmf_queue_entry.
                    message_header.message_id), 0, 0);
      }
      
   }
   if(TRUE == send_stop_dtmf)
   {
       MN_put_MNCC_STOP_DTMF_REQ(connection_id);  
   }
    
   
}

/***********************************************************************
 *   Function name : MN_handle_cs_sig_conn_rel_ind
 *   ---------------
 *
 *   Description:
 *   ------------
 *  This function marks the OTA Channel as relased, as the CS Signalling Connection 
 *  has been released, also as a guard code releases the vocoder, and .
 *  
 *  Parameters:
 *  -----------
 *  Connection ID.
 *
 *  Returns:
 *
 ***********************************************************************/
VOID_FUNC    MN_handle_cs_sig_conn_rel_ind(void)
{

   MSG_HIGH_DS_0(MN_SUB,"=MNCNM= MN handle CS Sig Conn Rel Ind");
   /* SMS link control processing */
   if(sms_link_control.status == SMS_LINK_CONTROL_ACTIVE)   /* Active && Timer is running */
   {
       MN_stop_sms_timer(sms_link_control.connection_id, TLC1M);

       /* Update the link status */
       sms_link_control.status = SMS_LINK_CONTROL_INACTIVE;

       /* Reset link control mode */
       sms_link_control.mode = ((sms_link_control.mode == SMS_LINK_CONTROL_ENABLED_ONE) ? 
                                   SMS_LINK_CONTROL_DISABLED : sms_link_control.mode);
         
       /* not sending report the UASMS as object is not available to extract SUB ID*/

   }

   /* AT+CMMS=1: No MM connection for transporting the deferred CP_ACK */
   sms_link_control.cp_ack_deferred = FALSE;

   /* Safety check to make sure vocoder is release when MM connection is released and OTA Channel is marked as 
        released */
   MN_disconnect_TCH(MN_VOCODER_OFF);

   /* Reset mvs global variable - mvs_callback_pending */
   ghdi_reset_mvs_callback_flag();

   MN_is_lte_deact_in_progress = FALSE;

   MN_turned_on_vocoder = FALSE;
   mn_waiting_for_tch_in_srvcc = FALSE;
   MSG_HIGH_DS_1(MN_SUB,"=MNCNM= MN_turned_on_vocoder flag is reset to %d", MN_turned_on_vocoder);

}
    
#ifdef FEATURE_UUS

/*
 *   Function name : MN_process_MNCC_USER_USER_DATA_IND
 *   ---------------
 *
 *   Description:
 *   ------------
 * This function processes the MNCC_USER_USER_DATA_IND message
 * to generate an cm_user_data_ind message to the MMI
 * 
 *
 *  Parameters:
 *  -----------
 *  IN message
 *
 *  Returns:
 *
 *
 */

VOID_FUNC   MN_process_MNCC_USER_USER_DATA_IND(IMH_T    *message)

{

   /* Locals */


   MNCC_USER_USER_DATA_IND_T   *pointer;
   int                         input_message_length,
   data_index,
   index;
   connection_id_T             connection_id;
   byte                        user_user_data[MAX_USER_USER_DATA_LENGTH],
                               user_user_information_length,
                               user_user_protocol_discriminator;
   boolean                     more_data;

   /* Align the pointers */

   pointer = (MNCC_USER_USER_DATA_IND_T *)((void *)message);

   input_message_length = pointer->message_header.message_len_lsb + 
       (256* pointer->message_header.message_len_msb);

   data_index = 0;

   connection_id = pointer->connection_id;

 
   /* Decode the data */

   if (pointer->data[data_index] == IEI_USER_USER)

   /* There is some user data */

   {

      data_index++;

      /* corrected compile warning C4244 conversion from int to byte */
      
      user_user_information_length = (byte)(pointer->data[data_index]);
      
      /****************************************************************
      *As per 24.008, User-User IE exists in TLV format in all messages 
      *except for USER INFORMATION message(section 9.3.31),where 
      *User-user IE exists in LV format. 
      *In TLV format, MIN length of User-User IE is 3 bytes.
      *1 byte for Type field + 1 byte for length field + 1 byte 
      *for value(user protocol).length field will therefore 
      *contain MIN value as 1. 
      *In LV format, MIN length of User-User IE is 2 bytes.
      *1 byte for Length field + 1 byte for value(user protocol).
      *length field will therefore contain MIN value as 1. 
      *****************************************************************/
      /*user-user length < 1 is invalid.*/
      if(user_user_information_length == 0)
      {
        return;
      }
      /* Prevent overruns */

      if (user_user_information_length > MAX_USER_USER_DATA_LENGTH)

      {

         user_user_information_length = MAX_USER_USER_DATA_LENGTH;

      }

      data_index++;

      user_user_protocol_discriminator = pointer->data[data_index];

      for (index = 0;index < user_user_information_length -1;index++)
      {

         data_index++;

         user_user_data[index] = pointer->data[data_index];

      }

      data_index++;

      /* see if there is an optional 'more data' IEI */

      more_data = FALSE;

      if (data_index  < (input_message_length - 1))
      {
         if (pointer->data[data_index] == IEI_MORE_DATA)
         {
            more_data = TRUE;
         }
      }

      if((cc_transaction_information_p[connection_id] != NULL) &&
         (cc_transaction_information_p[connection_id]->cc_state == CC_STATE_U6))
      {
          
              /* Send message to cm */
              MN_put_cm_mt_user_data_ind(connection_id,
                                     user_user_data,
                                     (byte)(user_user_information_length -1),
                                     user_user_protocol_discriminator,
                                     more_data,
                     TRUE);
      }
      else
      {

              /* Send message to cm */
              MN_put_cm_mt_user_data_ind(connection_id,
                                     user_user_data,
                                     (byte)(user_user_information_length -1),
                                     user_user_protocol_discriminator,
                                     more_data,
                     FALSE);
      }
      

      if(mn_call_information_p[connection_id] != NULL)
      {
        /* set implicit activation */
        if(mn_call_information_p[connection_id]->user_user_data.type == (byte)UUS1_IMPLICIT)
        {
           mn_call_information_p[connection_id]->user_user_data.activated = TRUE;
        }
  
        /* Check for any pending User-User Data */
        if(mn_call_information_p[connection_id]->user_user_data.activated != FALSE)
        {
            (void)MN_put_USER_USER_DATA_REQ(connection_id); 
        }
      }
      else
      {
        MSG_HIGH_DS_1(MN_SUB,"=MNCNM= process_MNCC_USER_USER_DATA_IND: mn_call_information_p[%d] is NULL",connection_id);    
      }
   }
   else
   {
      MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: no active connection for user user data");
   }
}

/*
 *   Function name : MN_process_CONGESTION_IND
 *   ---------------
 *
 *   Description:
 *   ------------
 * This function processes the MNCC_CONGESTION_IND message
 * to generate an cm_user_congestion_ind message to the CM
 * 
 *
 *  Parameters:
 *  -----------
 *  IN message
 *
 *  Returns:
 *
 *
 */

VOID_FUNC   MN_process_MNCC_CONGESTION_IND(IMH_T    *message)
{
    MNCC_CONGESTION_IND_T   *pointer;
    boolean  receiver_busy;

    pointer = (MNCC_CONGESTION_IND_T *) message;

    if(pointer->data[0])
        receiver_busy = TRUE;    
    else
        receiver_busy = FALSE;

    MN_put_cm_congestion_ind(pointer->connection_id, FALSE, receiver_busy);
}
#endif /* FEATURE_UUS */

#ifdef FEATURE_CCBS
/*
 *   Function name : MN_process_RECALL_IND
 *   ---------------
 *
 *   Description:
 *   ------------
 * This function processes the MNCC_RECALL_IND message
 * to generate an CM_RECALL_IND message to the CM
 * 
 *
 *  Parameters:
 *  -----------
 *  IN message
 *
 *  Returns:
 *
 *
 */

VOID_FUNC MN_process_MNCC_RECALL_IND (IMH_T *message)
{
   /* send CM_RECALL_IND to CM */
   MNCC_RECALL_IND_T       *pMsg;

   pMsg = (MNCC_RECALL_IND_T*)message;

   MN_put_cm_recall_ind (pMsg->connection_id, pMsg->data[0]);
}

/*
 *   Function name : MN_process_MNCC_CC_EST_IND
 *   ---------------
 *
 *   Description:
 *   ------------
 * This function processes the MNCC_CC_EST_IND message
 * 
 *
 *  Parameters:
 *  -----------
 *  IN message
 *
 *  Returns:
 *
 *
 */

VOID_FUNC MN_process_MNCC_CC_EST_IND (IMH_T *message)
{
   MNCC_CC_EST_IND_T  *cc_est=(MNCC_CC_EST_IND_T*)message;
   MNCC_SETUP_IND_T    setup;
     /* create a fake setup message to retrieve parameters */
   byte                setup_container_len;

   memset (&setup, 0x00, sizeof(MNCC_SETUP_IND_T));

   setup_container_len = cc_est->data[0];
   setup.connection_id = cc_est->connection_id; /* call id */

   memscpy (setup.data, sizeof(setup.data) ,&cc_est->data[1], setup_container_len);

   PUT_IMH_LEN (sizeof(setup.connection_id)+setup_container_len, &setup.message_header);

   /* save setup container in mn_call_information_p[setup.connection_id] */
   (void)MN_get_MNCC_SETUP_IND (&setup);
}
#endif /* FEATURE_CCBS */

/*
 *   Function name : MN_handle_ALT_proc_upon_call_release
 *   ---------------
 *
 *   Description:
 *   ------------
 * 
 * 
 *
 *  Parameters:
 *  -----------
 *  IN message
 *
 *  Returns:
 *
 *
 */

VOID_FUNC MN_handle_ALT_proc_upon_call_release(connection_id_T  connection_id
#if defined(FEATURE_DUAL_SIM )
,sys_modem_as_id_e_type as_id
#endif
)
{
  boolean                     s_respond_to_event;
  ie_cm_ss_error_T            cm_ss_error={0};
  int                         Multicall_Status_index,index;
  byte                        invoke_id = 0;
  byte                        mpty_operation = NULL_SS_OPERATION;
  
  /* ---------- local procedure_success ------------
   * 
   * This is the flag which indicates the success or failure of alternate procedure.
   * This will be set to FALSE when previous state is NACKED.
   * This basically means that when UE receives DISCONNECT followed
   *  by HOLD_REJ or RETRIEVE_REJ, ALT proc is considered failed.
   */
  byte                        procedure_success = TRUE; 

  
  if(mn_alternate_proc_status.is_alternate_procedure == TRUE)
  {
   
    Multicall_Status_index = (int)MN_whichMultiCallIndex(connection_id);
    if(Multicall_Status_index >= MULTICALL_STATUS_SIZE)
    {
      MSG_ERROR_DS_1(MN_SUB,"=MNCNM= ERROR:ALT Proc on, Invalid call session for = %d",connection_id);
    }
    else
    {
      if (mn_alternate_proc_status.status == MN_SS_ALT_HOLD_ACK_RETRIEVE_ACK_AWAITED)
      {
/*        mn_alternate_proc_status.status = MN_SS_ALT_NACKED; */
        /* For MPTY call, update the call states for connection ids other than the one on which RELEASE is received  
             For Single call, this is not required as the call state is already reset.
        */

        if(Multicall_Status[Multicall_Status_index].call_activity == CALL_ACTIVE || 
            Multicall_Status[Multicall_Status_index].call_activity == CALL_IN_PROGRESS)
        {
          mn_alternate_proc_status.status = MN_SS_ALT_RETRIEVE_ACK_AWAITED; 
        }
        else if(Multicall_Status[Multicall_Status_index].call_activity == CALL_HELD)
        {
          mn_alternate_proc_status.status = MN_SS_ALT_HOLD_ACK_AWAITED; 
        }

        if(Multicall_Status[Multicall_Status_index].call_is_mpty == TRUE)
        {
            if(Multicall_Status[Multicall_Status_index].call_activity == CALL_ACTIVE)
            {
      
              for(index = 0; index < Multicall_Status[Multicall_Status_index].id_index; index++)
              { /*
                 (void)MN_manage_CC_auxiliary_state(
                 Multicall_Status[Multicall_Status_index].connection_id[index],CCE_HOLD_MPTY_REJ); */
               }
            }
            if(Multicall_Status[Multicall_Status_index].call_activity == CALL_HELD)
            {
      
              for(index = 0; index < Multicall_Status[Multicall_Status_index].id_index; index++)
              { /*
                 (void)MN_manage_CC_auxiliary_state(
                  Multicall_Status[Multicall_Status_index].connection_id[index], CCE_RETRIEVE_MPTY_REJ); */
              }
            }
        }
      }
      else if(mn_alternate_proc_status.status == MN_SS_ALT_NACKED || 
              mn_alternate_proc_status.status == MN_SS_ALT_RETRIEVE_ACK_AWAITED ||
              mn_alternate_proc_status.status == MN_SS_ALT_HOLD_ACK_AWAITED)
      {
        memset (&cm_ss_error, 0x00, sizeof(ie_cm_ss_error_T));
        cm_ss_error.present = TRUE;
        cm_ss_error.error_code_tag = MN_SS_ERROR_TAG;
        
        if(mn_alternate_proc_status.status == MN_SS_ALT_NACKED)
        {
          /* If CALL RELEASE is for a call on which an HOLD/RETRIEVE REJ has already been receieved
             then do nothing as the ALT proc states and call states have already been updated
          */
          if((cc_transaction_information_p[Multicall_Status[(Multicall_Status_index + 1) % 2].connection_id[0]] != NULL) &&
            /* If ALT state is MN_SS_ALT_NACKED
               then the call on the other session will alwys be in state HOLD_RETRIEVE_REQUEST or HOLD_REQUEST*/
           ((cc_transaction_information_p[Multicall_Status[(Multicall_Status_index + 1) % 2].connection_id[0]]
            ->hold_auxiliary_state == HOLD_RETRIEVE_REQUEST)
            ||
           (cc_transaction_information_p[Multicall_Status[(Multicall_Status_index + 1) % 2].connection_id[0]]
           ->hold_auxiliary_state == HOLD_REQUEST)))
          {
            return;
          }
          /* For MPTY call, update the call states for connection ids other than the one on which RELEASE is received  
             For Single call, this is not required as the call state is already reset.
          */
          if(Multicall_Status[Multicall_Status_index].call_is_mpty == TRUE)
          {
            if(Multicall_Status[Multicall_Status_index].call_activity == CALL_ACTIVE)
            {
      
              for(index = 0; index < Multicall_Status[Multicall_Status_index].id_index; index++)
              {
                 (void)MN_manage_CC_auxiliary_state(
                 Multicall_Status[Multicall_Status_index].connection_id[index],CCE_HOLD_MPTY_REJ);
              }
              mpty_operation = holdMPTY;
            }
            if(Multicall_Status[Multicall_Status_index].call_activity == CALL_HELD)
            {
      
              for(index = 0; index < Multicall_Status[Multicall_Status_index].id_index; index++)
              {
                 (void)MN_manage_CC_auxiliary_state(
                  Multicall_Status[Multicall_Status_index].connection_id[index], CCE_RETRIEVE_MPTY_REJ);
              }
              mpty_operation = retrieveMPTY;
            }
          }
          cm_ss_error.error_code = MN_NETWORK_NOT_RESPONDING;

      /*
           * Set the local variable procedure_success as FALSE.
           * Since previous state is NACKED and now a DISCONNECT is received
           * we need to indicate procedure failure to upper layer.
           */
           procedure_success = FALSE;

        }
        else if (mn_alternate_proc_status.status == MN_SS_ALT_HOLD_ACK_AWAITED)
        {
          /* If CALL RELEASE is for a call on which RETRIEVE ACK has already been receieved
            then do nothing.
          */
          if((cc_transaction_information_p[Multicall_Status[(Multicall_Status_index + 1) % 2].connection_id[0]] != NULL) && 
        (cc_transaction_information_p[Multicall_Status[(Multicall_Status_index + 1) % 2].connection_id[0]]
            ->hold_auxiliary_state != HOLD_RETRIEVE_REQUEST)
            )
          {
            return;
          }

          /* consider HOLD REJ for the call on which RELEASE has been receieved 
             state to be updated only if it is a MPTY call
             for single call the states are already reset
          */
          if(Multicall_Status[Multicall_Status_index].call_is_mpty == TRUE)
          {
            /*  REVERSE THE HOLD CALL OPERATION FOR THE MPTY CALL  */
            for(index = 0; index < Multicall_Status[Multicall_Status_index].id_index; index++)
            {/*
              (void)MN_manage_CC_auxiliary_state(Multicall_Status[Multicall_Status_index].
                               connection_id[index], CCE_HOLD_MPTY_REJ);
                               */
            }
            mpty_operation = holdMPTY;
          }

          /* consider RETRIEVE CNF for the other call, as this call has been released */
          if(Multicall_Status[(Multicall_Status_index + 1) % 2].call_is_mpty == TRUE)
          {
            /*  Consider THE REtrieve cnf for FOR THE other call MPTY CALL  */
            for(index = 0; index < Multicall_Status[(Multicall_Status_index + 1) % 2].id_index; index++)
            {
              (void)MN_manage_CC_auxiliary_state(Multicall_Status[(Multicall_Status_index + 1) % 2].
                               connection_id[index], CCE_RETRIEVE_MPTY_CONF);
            }
          }
          else
          {
            s_respond_to_event = MN_manage_CC_auxiliary_state (Multicall_Status[(Multicall_Status_index + 1) % 2].connection_id[0],
                                                             CCE_CALL_RETRIEVE_CONF);/*c_call_event */
            if (s_respond_to_event == TRUE)
            {
              MN_stop_cc_timer(Multicall_Status[(Multicall_Status_index + 1) % 2].connection_id[0]);
            }
            else
            {
              MSG_ERROR_DS_1(MN_SUB,"=MNCNM= ERROR:ALT Proc on,CC Aux state of %d call in wrong state",connection_id);
            }
          }
          /*cm_ss_error.error_code = MN_CALL_HOLD_REJ;*/
        }
        else if (mn_alternate_proc_status.status == MN_SS_ALT_RETRIEVE_ACK_AWAITED)
        {
          /* If CALL RELEASE is for a call on which HOLD ACK has already been receieved
            then do nothing.
          */
        
          if((cc_transaction_information_p[Multicall_Status[(Multicall_Status_index + 1) % 2].connection_id[0]] != NULL) && 
        (cc_transaction_information_p[Multicall_Status[(Multicall_Status_index + 1) % 2].connection_id[0]]
             ->hold_auxiliary_state != HOLD_REQUEST))
          {
            return;
          }

          /* consider RETRVE REJ for the call on which RELEASE has been receieved 
             state to be updated only if it is a MPTY call
             for single call the states are already reset
          */          
          if(Multicall_Status[Multicall_Status_index].call_is_mpty == TRUE)
          {
            /*  REVERSE THE HOLD CALL OPERATION FOR THE MPTY CALL  */
            for(index = 0; index < Multicall_Status[Multicall_Status_index].id_index; index++)
            {/*
              (void)MN_manage_CC_auxiliary_state(Multicall_Status[Multicall_Status_index].
                               connection_id[index], CCE_RETRIEVE_MPTY_REJ);
                               */
            }
            mpty_operation = retrieveMPTY;
          }
          /* consider HOLD CNF for the other call*/
          if(Multicall_Status[(Multicall_Status_index + 1) % 2].call_is_mpty == TRUE)
          {
            /*  REVERSE THE HOLD CALL OPERATION FOR THE MPTY CALL  */
            for(index = 0; index < Multicall_Status[(Multicall_Status_index + 1) % 2].id_index; index++)
            {
              (void)MN_manage_CC_auxiliary_state(
                                    Multicall_Status[(Multicall_Status_index + 1) % 2].connection_id[index], 
                                    CCE_HOLD_MPTY_CONF);
            }
          }
          else
          {
            s_respond_to_event = MN_manage_CC_auxiliary_state(
                                      Multicall_Status[(Multicall_Status_index + 1) % 2].connection_id[0],
                                      CCE_CALL_HOLD_CONF);/*c_call_event */
            if (s_respond_to_event == TRUE)
            {
              MN_stop_cc_timer(Multicall_Status[(Multicall_Status_index + 1) % 2].connection_id[0]);   
            }
            else
            {
              MSG_ERROR_DS_1(MN_SUB,"=MNCNM= ERROR:ALT Proc on,CC Aux state of %d call in wrong state",connection_id);
            }
          }
/*          cm_ss_error.error_code = MN_CALL_RETRIEVE_REJ; */
        }
       
        mn_alternate_proc_status.is_alternate_procedure = FALSE;
        mn_alternate_proc_status.status = MN_SS_ALT_NONE_AWAITED;

        /* stop Timers*/
        /* Single call */
        MN_stop_cc_timer(connection_id);

        /* MPTY Call*/
        if (mpty_operation != NULL_SS_OPERATION)
        {
          for(invoke_id = 0x00; invoke_id < MAXNO_INVOKE_IDS; invoke_id++)
          {
            if(ss_invoke_data[invoke_id].ss_stored_operation == mpty_operation)
            {
              MN_stop_ss_timer(invoke_id);
              ss_invoke_data[invoke_id].ss_stored_operation = NULL_SS_OPERATION;
              break;
            }
          }
        }
        
        MN_MULTICALL_LOG(MN_read_multicall_status());

        MN_put_cm_manage_calls_conf(procedure_success, 
#if defined(FEATURE_DUAL_SIM )
                             as_id, 
#endif    
                             cm_ss_error
                             );  /* Successful release */  
      }
    }
    MSG_HIGH_DS_1(MN_SUB,"=MNCNM= ALT Proc is on ,status = %d",mn_alternate_proc_status.status);
  }
  
   
}

