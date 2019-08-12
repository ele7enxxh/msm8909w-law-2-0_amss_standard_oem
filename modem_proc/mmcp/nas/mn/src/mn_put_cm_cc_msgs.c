/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2001, 2002 Qualcomm Technologies, Incorporated and its licensors.  All Rights 
Reserved.  QUALCOMM Proprietary.  Export of this technology or software 
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mn/src/mn_put_cm_cc_msgs.c#1 $   

when       who     what, where, why
--------   ---     ----------------------------------------------------------
3/20/01     CD     Replaced old header by QCOM header
           Removed #ifdef ASW_SIMBA

3/26/2001   VT     Added MN_put_mmi_call_connected_ind()

4/25/01     TS     Added missing ; to line 116.  Arm compile found it.

5/9/01      CD     Removed unnecesary header file includes

5/24/01     CD     Corrected calling sequence for PUT_IMH_LEN to avoid lint errors.
                   Removed temporary structure message_buffer

                   Enclosed declaration of call_data in MN_put_mmi_call_connected_ind
                   within #ifdef FEATURE_MULTICALL to avoid lint errors

                   Fixed indentation in MN_put_mmi_connected_number_ind

                   Deleted 'ptr' in MN_put_mmi_mo_call_conf since it was not accessed

                   Removed cause.h since it was not being used

                   Deleted extern active_calls_list since it was not being used

7/13/01     CD     Replaced all instances of GS_QUEUE_MMI by GS_QUEUE_CM
                   Replaced all instances of MS_MN_MMI by MS_MN_CM

7/18/01     CD     Removed duplicate external declaration of Multicall_Status to mn_cc.h
                   Removed duplicate external declaration of mn_call_information

8/10/01     CD     Removed unnecessary queue.h

08/14/01    AB     Renamed connection manager references to CNM from CM.

08/16/01    AB     Renamed MMI references to CM.
08/22/01    TS     Added include cmll.h
                   Removed obsolete function that sent unused message to Call Manager.
08/23/01    TS     Send rab_id from database to call manager.

10/02/01    AB     If MO request failed, return the failed connection id request instead
                   of unknown id.  Also added mn_ut_debug flag to control message 
                   flow to CM when unit testing.

01/08/02    AB     Added the optional Signal element in cm_mt_call_ind_T
                   if the SIGNAL IE is present in SETUP indication message.

01/18/02    AB     Initializes the message buffer (automatic data taken from task 
                     stack) went sending message to CM to produce a deterministic 
                     output for UTF.
                     
01/22/01    AB     Moved SS call related report function, MN_put_cm_manage_calls_conf(),
                     to mn_put_cm_ss_msgs.c.
                     
01/25/02    CD     Replaced IEI  by boolean flag
                   Replaced calls to ERR() by MSG_ERROR()
                   Updated Copyright
                   Removed mn_cm.h as it is not needed, since mn.h already
                   includes it

06/12/02    AB     Removed the mn_ut_debug global flag used to stop sending 
                   message to CM in the TREX unit test enviroment.
                   
07/02/02    CD     Added processing of cause_of_no_CLI to MN_put_cm_mt_setup_ind()

07/10/02    CD     Replaced include cmll.h by cmwll.h

08/08/02    AB     In MN_put_cm_mt_setup_ind(), added the SIGNAL IE info
                   to CM_MT_SETUP_IND for possible remote alert pattern 
                   that may be play by the UE in the OTA SETUP indication.

09/02/02    AB     In MN_put_cm_mt_setup_ind(), added Redirecting party
                   BCD number and Subaddresses if available.

03/19/03    AB     In MN_put_cm_mt_call_end_ind(), retrieve the cause value, 
                   which previously stored, when cause value is not included 
                   in the release or release_complete message.
 
04/16/03    AB     Added processing of new ie_cm_cc_reject_T IE to support 
                   silent redial feature.
 
                   Deferred the cm_mt_call_end_ind_T(), until the 
                   RR connection had been released.

6/24/03     AB     In MN_put_mt_end_call_ind(), sent current call end
                   to CM in additional to any call that was deferred.

7/02/03     AB     In MN_put_mt_setup_ind(), allocate buffer instead
                   of large automatic data which can corrupt task stack. 

11/02/04    AB     Added SS Call related User-to-User Signaling feature.

12/15/04    AB     Added supports for AT+CMMS=1, multiple MO SMS via ATCOP.

05/10/05    HS     Added support for FEATURE_CCBS

07/22/05    HS     Added support for silent redial in CCBS

10/04/06    NR     Passing RAB id in call progress indication

11/02/06    NR     Fixed lint errors

02/06/07    RD     Added fixes for kloc errors

06/18/07    RD     BCIE 2 is included even when repeat indicator is 1

08/13/07    PP     Removed kloc feature flag 
 
10/10/11   gps     Message length adjusted irresprctive of Feature Dual Sim 
===========================================================================*/

/* Includes */

#include "mmcp_variation.h"
#include "customer.h"
#include "rex.h"
#include <string.h>
#include "environ.h"
#include "ms.h"
#include "timers_v.h"
#include "mn.h"
#include "mnglobal.h"
#include "msg.h"
#include "err.h"
#include "nasutils.h"

/* ----------------------------------------------------------------------- 
** REMOVED compiler warning from inclusion of memheap.h 
** ----------------------------------------------------------------------- */
#ifdef FEATURE_MEM_DEBUG
#undef FEATURE_MEM_DEBUG
#endif
#include "cmwll.h"
#include "cnm.h"

#ifdef FEATURE_MODEM_HEAP
#include "modem_mem.h"
#endif
#include <stringl/stringl.h>

/*
 * Function name :  MN_put_cm_call_progress_info_ind
 * -------------
 *
 * Description :  Creates the cm_call_progress_info_ind
 * -----------     message to the mmi
 *
 *
 *
 *
 *
 *
 * Parameters: connection_id
 * ----------  call_progress
 *
 *
 *
 * Returns:
 * --------
 *
 */

VOID_FUNC  MN_put_cm_call_progress_info_ind(
connection_id_T   connection_id,
byte              notification_indicator)

{


   /* locals */

   cm_call_progress_info_ind_T   message;
   gs_status_T  GSstatus;
   int      message_length;

   /* Set up the header */
   memset(&message, 0x00, sizeof(cm_call_progress_info_ind_T)); 
   message.message_header.message_set = MS_MN_CM;
   message.message_header.message_id = (word)CM_CALL_PROGRESS_INFO_IND;
   message_length =  sizeof(cm_call_progress_info_ind_T) - sizeof(IMH_T);
   PUT_IMH_LEN(message_length,&message.message_header)

   /* Set the connection id */
   message.connection_id = connection_id;
#ifdef FEATURE_DUAL_SIM
   if(MNCNM_IS_SINGLE_STACK)
   {
     if(mn_call_information_p[connection_id] != NULL)
     {
      message.as_id = mn_call_information_p[connection_id]->as_id;
     }
     else if(mn_as_id != SYS_MODEM_AS_ID_NONE)
     {
      message.as_id = mn_as_id;
     }
     else
     {
      MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: call obj and mn_as_id are null!");
      return;
     }
   }
#ifdef FEATURE_DUAL_ACTIVE
   else
   {
     message.as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
   }
#endif   
#endif

#ifdef FEATURE_MULTIMEDIA_FALLBACK
   #error code not present
#endif

   /* Set the progress indicator */
   message.call_progress = notification_indicator;

   /* Send the message */
   GSstatus = MN_send_message(GS_QUEUE_CM,&message,TRUE);

   /* log any failures */
   if(GSstatus != GS_SUCCESS)
   {
      MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: cm_call_progress_data_ind message failed");
   }
}

#ifdef FEATURE_NAS_ECALL
/*
 * Function name : MN_put_cm_handover_complete_ind
 * -------------
 *
 * Description : Crates the cm_handover_complete_ind
 * -----------     message to the CM
 *
 *
 *
 *
 *
 *
 * Parameters: connection_id
 * ----------  handover_cause
 *
 *
 *
 * Returns:
 * --------
 *
 */

VOID_FUNC  MN_put_cm_handover_complete_ind(
connection_id_T   connection_id,
sync_enum_T       handover_cause
)
{
  /* locals */

   cm_handover_complete_ind_T message;
   gs_status_T GSstatus;
   int message_length;

   /* Set up the header */
   memset(&message, 0x00, sizeof(cm_handover_complete_ind_T)); 
   message.message_header.message_set = MS_MN_CM;
   message.message_header.message_id = (word)CM_HANDOVER_COMPLETE_IND;
   message_length =  sizeof(cm_handover_complete_ind_T) - sizeof(IMH_T);
   PUT_IMH_LEN(message_length,&message.message_header)

   /* Set the connection id */
   message.connection_id = connection_id;

   /* Set the handover cause*/
   message.handover_cause = handover_cause;

#ifdef FEATURE_DUAL_SIM
   if(MNCNM_IS_SINGLE_STACK)
   {
     if(mn_call_information_p[connection_id] != NULL)
     {
      message.as_id = mn_call_information_p[connection_id]->as_id;
     }
     else if(mn_as_id != SYS_MODEM_AS_ID_NONE)
     {
      message.as_id = mn_as_id;
     }
     else
     {
      MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: call obj and mn_as_id are null!");
      return;
     }
   }
#ifdef FEATURE_DUAL_ACTIVE
   else
   {
     message.as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
   }
#endif   
#endif

   /* Send the message */
   GSstatus = MN_send_message(GS_QUEUE_CM,&message,TRUE);

   /* log any failures */
   if(GSstatus != GS_SUCCESS)
   {
     MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: cm_handover_complete_ind_T message failed");
   }

}

#endif /*FEATURE_NAS_ECALL*/

#ifdef FEATURE_CCBS
/*
 * Function name :  MN_put_cm_ccbs_info_ind
 * -------------
 *
 * Description :  Creates the CM_CCBS_INFO_IND and send
 * -----------     message to CM
 *
 *
 *
 *
 *
 *
 * Parameters: connection_id
 * ----------  ccbs_allowed
 *
 *
 *
 * Returns:
 * --------
 *
 */

VOID_FUNC  MN_put_cm_ccbs_info_ind (connection_id_T connection_id, boolean ccbs_allowed)
{
   cm_ccbs_info_ind_T  message;
   gs_status_T         GSstatus;
   int                 message_length;

   memset (&message, 0x00, sizeof(cm_ccbs_info_ind_T)); /* Set up the header */

   message.message_header.message_set = MS_MN_CM;
   message.message_header.message_id  = (word)CM_CCBS_INFO_IND;
   message_length                     = sizeof(connection_id_T) + sizeof(boolean);

   PUT_IMH_LEN (message_length,&message.message_header)

   message.connection_id = connection_id;  /* Set the connection id */
   message.ccbs_allowed  = ccbs_allowed;   /* Set the ccbs_allowed */
#ifdef FEATURE_DUAL_SIM 
   if(MNCNM_IS_SINGLE_STACK)
   {
     if (mn_call_information_p[connection_id] != NULL)
     {
       message.as_id = mn_call_information_p[connection_id]->as_id;
     }
     else
     {
       message.as_id = mn_as_id;
     }
   }
#ifdef FEATURE_DUAL_ACTIVE
   else
   {
      message.as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
   }
#endif
#endif
   GSstatus = MN_send_message(GS_QUEUE_CM,&message,TRUE); /* Send the message */

   if(GSstatus != GS_SUCCESS) /* log any failures */
   {
      MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: cm_ccbs_info_ind message failed");
   }
}

/*
 * Function name :  MN_fill_bc_llc_hlc_data
 * -------------
 *
 * Description :  Copy BC/LLC/HLC information
 * -----------
 *
 * Parameters: copy or convert as below
 * ----------
 *    ri_from   -> ri_to   : repeat indicator
 *    ie_1_from -> ie_1_to : BC1, LLC1 or HLC1
 *    ie_2_from -> ie_2_to : BC2, LLC2 or HLC2
 *    size                 : size of BC1, LLC1 or HLC1
 *
 * Returns:  None
 * --------
 *
 */
static void MN_fill_bc_llc_hlc_data (
    byte ri_from,                 /*repeat indicator*/
    cm_repeat_indicator_T* ri_to, /*repeat indicator*/
    void* ie_1_from,              /*BC1,LLC1 or HLC1*/
    void* ie_1_to,                /*BC1,LLC1 or HLC1*/
    void* ie_2_from,              /*BC2,LLC2 or HLC2*/
    void* ie_2_to,                /*BC2,LLC2 or HLC2*/
    int size                      /*size of BC1,LLC1 or HLC1*/
    )
{
  ri_to->present = FALSE; /* assume there is no BCIE */

  if (ri_from != 0) /* one or more BCIE */
  {
    memscpy(ie_1_to, (size_t)size ,ie_1_from, (size_t)size);

    if (ri_from != 1) /* more than one BCIE */
    {
      ri_to->present = TRUE;
      ri_to->repeat_indication = 2;

      memscpy(ie_2_to, (size_t)size ,ie_2_from, (size_t)size);
    }
  }
}

/*
 * Function name :  MN_fill_call_info
 * -------------
 *
 * Description :  Copy call information from mn_call_information_p[] to argument
 * -----------
 *
 * Parameters: id: connection id
 * ----------  info_ptr: container to copy call data to
 *
 * Returns:  None
 * --------
 *
 */

static void MN_fill_call_info( byte id, cm_mo_normal_call_req_T *info_ptr )
{
   mn_call_information_T  *mn_info_ptr;

   if(mn_call_information_p[id] != NULL)
   {
     mn_info_ptr = mn_call_information_p[id];
  
     /* ************************************************** */
     info_ptr->connection_id = id;
  
     /* ************************************************** */
     MN_fill_bc_llc_hlc_data (
         mn_info_ptr->repeat_indicator_bc,
         &info_ptr->bc_repeat_indicator,
         (void*)&mn_info_ptr->bearer_capability_1,
         (void*)&info_ptr->bearer_capability_1,
         (void*)&mn_info_ptr->bearer_capability_2,
         (void*)&info_ptr->bearer_capability_2,
         sizeof(cm_bearer_capability_T));
  
     /* ************************************************** */
     MN_fill_bc_llc_hlc_data (
         mn_info_ptr->repeat_indication_llc,
         &info_ptr->llc_repeat_indicator,
         (void*)&mn_info_ptr->low_layer_compatibility_1,
         (void*)&info_ptr->low_layer_compatibility_1,
         (void*)&mn_info_ptr->low_layer_compatibility_2,
         (void*)&info_ptr->low_layer_compatibility_2,
         sizeof(cm_low_layer_compatibility_T));
  
     /* ************************************************** */
     MN_fill_bc_llc_hlc_data (
         mn_info_ptr->repeat_indication_hlc,
         &info_ptr->hlc_repeat_indicator,
         (void*)&mn_info_ptr->high_layer_compatibility_1,
         (void*)&info_ptr->high_layer_compatibility_1,
         (void*)&mn_info_ptr->high_layer_compatibility_2,
         (void*)&info_ptr->high_layer_compatibility_2,
         sizeof(cm_high_layer_compatibility_T));
  
     /* ************************************************** */
     if (mn_info_ptr->called_party_bcd_number_present)
     {
       /*memcpy ( &info_ptr->cm_called_party_bcd_number,
                &mn_info_ptr->called_party_bcd_number,
                sizeof(cm_called_party_bcd_no_T));*/
       info_ptr->cm_called_party_bcd_number = mn_info_ptr->called_party_bcd_number;
       info_ptr->cm_called_party_bcd_number.present = TRUE;
     }
  
     /* ************************************************** */
     if (mn_info_ptr->called_party_subaddress_present)
     {
       /*memcpy ( &info_ptr->cm_called_party_subaddress,
                &mn_info_ptr->called_party_subaddress,
                sizeof(cm_called_party_subaddress_T));*/
       info_ptr->cm_called_party_subaddress = mn_info_ptr->called_party_subaddress;
       info_ptr->cm_called_party_subaddress.present = TRUE;
     }
  
     /* ************************************************** */
     if (mn_info_ptr->calling_party_subaddress_present)
     {
       /*memcpy ( &info_ptr->cm_calling_party_subaddress,
                &mn_info_ptr->calling_party_subaddress,
                sizeof(cm_calling_party_subaddress_T));*/
       info_ptr->cm_calling_party_subaddress = mn_info_ptr->calling_party_subaddress;
       info_ptr->cm_calling_party_subaddress.present = TRUE;
     }
  
     /* ************************************************** */
     if (mn_info_ptr->cm_forward_cug_info.present)
     {
       /*memcpy ( &info_ptr->forward_cug_info,
                &mn_info_ptr->cm_forward_cug_info,
                sizeof(cm_forward_cug_info_T) );*/
       info_ptr->forward_cug_info = mn_info_ptr->cm_forward_cug_info;
     }
  
     /* ************************************************** */
     if (mn_info_ptr->clir_invocation == TRUE)
     {
       info_ptr->cm_clir = CM_CLIR_INVOCATION;
     }
     else if (mn_info_ptr->clir_suppression == TRUE)
     {
       info_ptr->cm_clir = CM_CLIR_SUPPRESSION;
     }
     else
     {
       info_ptr->cm_clir = CM_CLIR_DEFAULT;
     }
   } 
   else
   {
     MSG_HIGH_DS_1(MN_SUB,"=MNCNM= mn_call_information_p[%d] is NULL",id);
   }
}

/*
 * Function name :  MN_put_cm_recall_ind
 * -------------
 *
 * Description :  Creates the CM_RECALL_IND
 * -----------     message to the CM
 *
 *
 *
 *
 *
 *
 * Parameters: connection_id
 * ----------  recall_type
 *
 *
 *
 * Returns:
 * --------
 *
 */

VOID_FUNC  MN_put_cm_recall_ind (connection_id_T connection_id, byte type)
{
   cm_recall_ind_T     message;
   gs_status_T         GSstatus;
   int                 message_length;

   memset (&message, 0x00, sizeof(cm_recall_ind_T));
   
   /* Set up the header */
   message.message_header.message_set = MS_MN_CM;
   message.message_header.message_id  = (word)CM_RECALL_IND;
   message_length                     = sizeof(cm_recall_ind_T) - sizeof(IMH_T);

   PUT_IMH_LEN (message_length,&message.message_header)

   message.connection_id = connection_id;  /* Set the connection id */
   message.recall_type   = type;           /* Set the recall_type */
#ifdef FEATURE_DUAL_SIM 

   if(MNCNM_IS_SINGLE_STACK)
   {
     if (mn_call_information_p[connection_id] != NULL)
     {
       message.as_id = mn_call_information_p[connection_id]->as_id;
     }
     else
     {
       message.as_id = mn_as_id;
     }
   }
#ifdef FEATURE_DUAL_ACTIVE
   else
   {
     message.as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
   }
#endif
#endif
   MN_fill_call_info( connection_id, &message.call_info );

   GSstatus = MN_send_message(GS_QUEUE_CM,&message,TRUE); /* Send the message */

   if (GSstatus == GS_SUCCESS) /* log any failures */
   {
      MSG_HIGH_DS_1(MN_SUB,"=MNCNM= [ccbs]cm_recall_ind with type %d",type);
   }
   else
   {
      MSG_ERROR_DS_1(MN_SUB,"=MNCNM= [ccbs]cm_recall_ind msg failed:%d",GSstatus);
   }
}
#endif /* FEATURE_CCBS */



/*
 * Function name :  MN_put_cm_call_connected_ind
 * -------------
 *
 * Description :  Creates the cm_call_connected_ind
 * -----------     message to the mmi
 *
 *
 *
 *
 *
 * Parameters: connection_id
 * ----------  
 *
 *
 * Returns:
 * --------
 *
 */

VOID_FUNC  MN_put_cm_call_connected_ind(connection_id_T   connection_id)

{
   /* locals */

   cm_call_connected_ind_T   message;
   gs_status_T  GSstatus;
   int      message_length;
   mn_call_information_T      *call_data;

   call_data = mn_call_information_p[connection_id];


   /* Set up the header */
   memset(&message, 0x00, sizeof(cm_call_connected_ind_T)); 
   message.message_header.message_set = MS_MN_CM;
   message.message_header.message_id = (word)CM_CALL_CONNECTED_IND;
   message_length = sizeof(cm_call_connected_ind_T) - sizeof(IMH_T);
   PUT_IMH_LEN(message_length,&message.message_header)

   /* Set the connection id */
   message.connection_id = connection_id;

   /* Pass the correct rab id to call manager */
   if(call_data != NULL)
   {
     message.radio_access_bearer_id = call_data->radio_access_bearer_id;
#ifdef FEATURE_DUAL_SIM 
     if(MNCNM_IS_SINGLE_STACK)
     {
       message.as_id = call_data->as_id;
     }
#ifdef FEATURE_DUAL_ACTIVE
     else
     {
       message.as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
     }
#endif

#endif
   }  
   else
   {
     MSG_HIGH_DS_0(MN_SUB,"=MNCNM= cannot fill radio_access_bearer_id as call_data is NULL");
   }
   /* Send the message */
   GSstatus = MN_send_message(GS_QUEUE_CM,&message,TRUE);

   /* log any failures */
   if(GSstatus != GS_SUCCESS)
   {
      MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: cm_call_connected_ind message failed");  
   }

}  /* End of MN_put_cm_call_connected_ind() */



/*
 * Function name :  MN_put_cm_connected_number_ind
 * -------------
 *
 * Description :  Sends message to the cm containing the connected
 *                number for the connection_id
 *
 *
 *
 *
 * Parameters:  connection_id
 * ----------
 *
 *
 *
 * Returns:
 * --------
 *
 */

VOID_FUNC MN_put_cm_connected_number_ind(connection_id_T  connection_id)

{
   /* locals */

   cm_connected_number_ind_T   message;
   mn_call_information_T        *call_data;
   int                          i,message_length;
   gs_status_T                  GSstatus;

   if(connection_id >= MAXNO_CC_TRANSACTIONS)
   {

      MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Invalid connection id for mn_call_information_p");
      
   }
   else if(mn_call_information_p[connection_id] == NULL)
   {
      MSG_HIGH_DS_1(MN_SUB,"=MNCNM= mn_call_information_p[%d] is NULL",connection_id);
   }
   else
   {
      call_data = mn_call_information_p[connection_id];

      /* Set up the header */
      memset(&message, 0x00, sizeof(cm_connected_number_ind_T)); 
      message.message_header.message_set = MS_MN_CM;
      message.message_header.message_id = (word)CM_CONNECTED_NUMBER_IND;
      message_length = sizeof(cm_connected_number_ind_T) - sizeof(IMH_T);
      PUT_IMH_LEN(message_length,&message.message_header)

      /* Set the connection id */
      message.connection_id = connection_id;
#ifdef FEATURE_DUAL_SIM 
      if(MNCNM_IS_SINGLE_STACK)
      {
        message.as_id = call_data->as_id;
      }
#ifdef FEATURE_DUAL_ACTIVE 
      else
      {
        message.as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
      }
#endif
#endif
      /* Transfer the connected number */
      if (call_data->connected_number_present == TRUE)
      {
          message.cm_connected_number.present = TRUE;
          message.cm_connected_number.length = 
          call_data->connected_number.length;

          for (i=0;i<call_data->connected_number.length;i++)
          {
             message.cm_connected_number.data[i] =   
             call_data->connected_number.data[i];
          }
      }
      else
      {
         message.cm_connected_number.present = FALSE;
      }

      /* Transfer the connected subaddress */
      if (call_data->connected_subaddress_present == TRUE)
      {
         /* Build the message */
         message.cm_connected_subaddress.present = TRUE;
         message.cm_connected_subaddress.length =        
         call_data->connected_subaddress.length;

         for (i=0;i<call_data->connected_subaddress.length;i++)
         {
            message.cm_connected_subaddress.data[i] = 
            call_data->connected_subaddress.data[i];
         }
      }
      else
      {
         message.cm_connected_subaddress.present = FALSE;
      }

      /* Send the message */

      GSstatus = MN_send_message(GS_QUEUE_CM,&message,TRUE);

      /* log any failures */
      if(GSstatus != GS_SUCCESS)
      {
         MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Failed to send mm_connected_number_ind message");
      }

   }

}


/*
 *   Function name : MN_put_cm_mt_setup_ind
 *   ---------------
 *
 *   Description:
 *   ------------
 *  This function generates the cm_mt_setup_ind message to the 
 *  mmi.
 *  
 *  
 *
 *  Parameters:
 *  -----------
 *  IN connection_id
 *
 *  Returns:
 *
 *
 */


VOID_FUNC MN_put_cm_mt_setup_ind(connection_id_T  connection_id)
{

   /* Locals */

   cm_mt_setup_ind_T      *message;
   gs_status_T            status;
   mn_call_information_T  *call_data;
   int                    message_size;

#ifndef FEATURE_MODEM_HEAP
   message = (cm_mt_setup_ind_T *)gs_alloc(sizeof(cm_mt_setup_ind_T)); 
#else
   message = (cm_mt_setup_ind_T *)modem_mem_calloc(1, sizeof(cm_mt_setup_ind_T), MODEM_MEM_CLIENT_NAS);
#endif

   /* If there are no connection id's left */
   if((message != NULL) && (connection_id < MAXNO_CC_TRANSACTIONS))
   {
     if(mn_call_information_p[connection_id] != NULL)
     {
        /* Set up the message header */
        memset(message, 0x00, sizeof(cm_mt_setup_ind_T)); /* init auto data */
        message->message_header.message_set = MS_MN_CM;
        message->message_header.message_id = (word)CM_MT_SETUP_IND;
  
        message_size = sizeof(cm_mt_setup_ind_T) - sizeof(IMH_T);
        message->message_header.message_len_msb = (unsigned char)(message_size / 0x100);
        message->message_header.message_len_lsb = (unsigned char)(message_size % 0x100);
  
        /* Set up the connection id */
        message->connection_id = connection_id;
  
        call_data =  mn_call_information_p[connection_id];
#ifdef FEATURE_DUAL_SIM 
        if(MNCNM_IS_SINGLE_STACK)
        {
          message->as_id = call_data->as_id;
        } 
#ifdef FEATURE_DUAL_ACTIVE 
        else
        {
          message->as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
        }
#endif
#endif
        /* If there is one bearer capability */
        if (call_data->repeat_indicator_bc == REPEAT_CIRCULAR)  /* 5.3.4.1, mode 1 alternate mode 1 use for In-call Mod */
        {
           message->bc_repeat_indicator.present = FALSE;
           message->bearer_capability_1 = call_data->bearer_capability_1;
           message->bearer_capability_1.present = TRUE;
           message->bearer_capability_2.present = FALSE;
           
           if(call_data->bearer_capability_2.present)
           {
              message->bc_repeat_indicator.present = TRUE;
              message->bc_repeat_indicator.repeat_indication = call_data->repeat_indicator_bc;
              message->bearer_capability_2.present = TRUE;
              message->bearer_capability_2 = call_data->bearer_capability_2;
           }
        }
        /* If there are two bearer capabilities */
        else if ((call_data->repeat_indicator_bc == REPEAT_SUPPORTS_FALLBACK) ||
            (call_data->repeat_indicator_bc == REPEAT_SERVICE_CHANGE_FALLBACK))
        {
           /* TS 24.008, Section 10.5.4.22 */
           message->bc_repeat_indicator.present = TRUE;
           message->bc_repeat_indicator.repeat_indication = call_data->repeat_indicator_bc;
  
           message->bearer_capability_1 = call_data->bearer_capability_1;
           message->bearer_capability_1.present = TRUE;
           message->bearer_capability_2 = call_data->bearer_capability_2;
           message->bearer_capability_2.present = TRUE;
        }
        else
        {
           message->bc_repeat_indicator.present = FALSE;
           message->bearer_capability_1.present = FALSE;
           message->bearer_capability_2.present = FALSE;
        }
  
        /* Set up the low layer compatibility data */
  
        /* If there is one low layer compatibility */
        if (call_data->repeat_indication_llc == REPEAT_CIRCULAR)
        {
           message->llc_repeat_indicator.present = FALSE;
           message->low_layer_compatibility_1 = call_data->low_layer_compatibility_1;
           message->low_layer_compatibility_1.present = TRUE; /* first byte */
           message->low_layer_compatibility_2.present = FALSE;
        }
  
        /* If there are two low layer compatibilities */
        else if ((call_data->repeat_indication_llc == REPEAT_SUPPORTS_FALLBACK) ||
            (call_data->repeat_indication_llc == REPEAT_SERVICE_CHANGE_FALLBACK ))
        {
           
           message->llc_repeat_indicator.repeat_indication = call_data->repeat_indication_llc;
           message->llc_repeat_indicator.present = TRUE;
  
           message->low_layer_compatibility_1 = call_data->low_layer_compatibility_1;
           message->low_layer_compatibility_1.present = TRUE; /* first byte */
           message->low_layer_compatibility_2 = call_data->low_layer_compatibility_2;
           message->low_layer_compatibility_2.present = TRUE;
        }
        else
        {
           message->llc_repeat_indicator.present = FALSE;
           message->low_layer_compatibility_1.present = FALSE;
           message->low_layer_compatibility_2.present = FALSE;
        }
  
        /* Set up the high level compatibility data */
  
        /* If there is one high layer compatibility */
        if (call_data->repeat_indication_hlc == REPEAT_CIRCULAR)
        {
           message->hlc_repeat_indicator.present = FALSE;
           message->high_layer_compatibility_1 = 
               call_data->high_layer_compatibility_1;
           message->high_layer_compatibility_1.present = TRUE;  /* first byte */
           message->high_layer_compatibility_2.present = FALSE;
        }
  
        /* If there are two high layer compatibilities */
        else if ((call_data->repeat_indication_hlc == REPEAT_SUPPORTS_FALLBACK)||
            (call_data->repeat_indication_hlc == REPEAT_SERVICE_CHANGE_FALLBACK))
        {
  
           message->hlc_repeat_indicator.repeat_indication = call_data->repeat_indication_hlc;
           message->hlc_repeat_indicator.present = TRUE;
  
           message->high_layer_compatibility_1 = 
               call_data->high_layer_compatibility_1;
           message->high_layer_compatibility_1.present = TRUE; /* first byte */
           message->high_layer_compatibility_2 = 
               call_data->high_layer_compatibility_2;
           message->high_layer_compatibility_2.present = TRUE;   /* first byte */
  
        }
        else
            /* If there are no high layer compatibilities */
        {
           message->hlc_repeat_indicator.present = FALSE;
           message->high_layer_compatibility_1.present = FALSE;
           message->high_layer_compatibility_2.present = FALSE;
        }
  
  
        /* Set up the called party BCD number data */
  
        /* If not present */
        if (call_data->called_party_bcd_number_present == FALSE)
        {
           message->cm_called_party_bcd_number.present = FALSE;
        }
        else
        {
           message->cm_called_party_bcd_number =  
               call_data->called_party_bcd_number;
           message->cm_called_party_bcd_number.present = TRUE;
        }
  
        /* Set up the called party subaddress data */
  
        if (call_data->called_party_subaddress_present == FALSE)
        {
           message->cm_called_party_subaddress.present = FALSE;
        }
        else
        {
           message->cm_called_party_subaddress = 
               call_data->called_party_subaddress;
           message->cm_called_party_subaddress.present = TRUE;
        }
  
        /* Set up the calling party BCD number data */
  
        if (call_data->calling_party_bcd_number_present == FALSE)
        {
           message->cm_calling_party_bcd_number.present = FALSE;
        }
        else
        {
           message->cm_calling_party_bcd_number = 
               call_data->calling_party_bcd_number;
           message->cm_calling_party_bcd_number.present = TRUE;
        }
  
        /* Set up the calling party subaddress data */
  
        if (call_data->calling_party_subaddress_present == FALSE)
        {
           message->cm_calling_party_subaddress.present = FALSE;
        }
        else
        {
           message->cm_calling_party_subaddress = call_data->calling_party_subaddress;
           message->cm_calling_party_subaddress.present = TRUE;
        }
  
        /* Set up the Redirecting party BCD number data */
  
        if (call_data->redirect_party_bcd_number_present == FALSE)
        {
           message->cm_redirect_party_bcd_number.present = FALSE;
        }
        else
        {
           message->cm_redirect_party_bcd_number = call_data->redirect_party_bcd_number;
           message->cm_redirect_party_bcd_number.present = TRUE;
        }
  
        /* Set up the Redirecting party subaddress data */
  
        if (call_data->redirect_party_subaddress_present == FALSE)
        {
           message->cm_redirect_party_subaddress.present = FALSE;
        }
        else
        {
           message->cm_redirect_party_subaddress =  call_data->redirect_party_subaddress;
           message->cm_redirect_party_subaddress.present = TRUE;
        }
  
        /* Set up the cause of no CLI */
        if (call_data->cause_of_no_CLI.present == FALSE)
        {
            message->cm_cause_of_no_CLI.present = FALSE;
        }
        else
        {
            message->cm_cause_of_no_CLI.cause_value = 
                call_data->cause_of_no_CLI.cause_value;
            message->cm_cause_of_no_CLI.present = TRUE;
        }

       /* Set up the value of alerting pattern */
       if (call_data->alerting_pattern.present == FALSE)
        {
            message->cm_alerting_pattern.present = FALSE;
        }
        else
        {
            message->cm_alerting_pattern.alerting_pattern_value = 
                call_data->alerting_pattern.alerting_pattern_value;
            message->cm_alerting_pattern.present = TRUE;
        }

        /* Setup the signal IE info */
        if(call_data->signal_present == TRUE) 
        {
            message->cm_signal.present = TRUE;
         message->cm_signal.signal_value = call_data->SignalValue; 
        }
        else
        {
            message->cm_signal.present = FALSE;
        }
  
  #if defined(FEATURE_MULTIMEDIA_FALLBACK) || defined(FEATURE_REL6)
  
        if(call_data->backup_bearer_capability.present == TRUE)
        {
           message->backup_bearer_capability = call_data->backup_bearer_capability;
        }
        else
        {
           message->backup_bearer_capability.present = FALSE;
        }
  
        if(call_data->network_cc_capability.present == TRUE)
        {
           message->cm_network_capability = call_data->network_cc_capability;
        }
        else
        {
           message->cm_network_capability.present = FALSE;
        }       
  #endif  /* FEATURE_MULTIMEDIA_FALLBACK */
  
  
        /* Send message */
        status = MN_send_message(GS_QUEUE_CM, message, TRUE);
        if ( status != GS_SUCCESS)
        {
           MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Send cm_mt_setup_ind message failure");
         
        }
#ifdef FEATURE_WCDMA
        mn_store_rab_status();
#endif
#ifndef FEATURE_MODEM_HEAP
        gs_free( message );
#else
        modem_mem_free(message, MODEM_MEM_CLIENT_NAS);
#endif
     }
     else
     {
       MSG_HIGH_DS_1(MN_SUB,"=MNCNM= mn_put_cm_mt_setup_ind: mn_call_information_p[%d] is NULL",connection_id);
     }
   }
   else
   {
      MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: The connection id is invalid for mn_call_information_p"); 
   }

}

#ifdef  FEATURE_MULTIMEDIA_FALLBACK
#error code not present
#endif /* FEATURE_MULTIMEDIA_FALLBACK */

/*
 *   Function name : MN_put_cm_mo_call_conf
 *   ---------------
 *
 *   Description:
 *   ------------
 *  This function generates the cm_mo_call_conf message to the 
 *  mmi. Confirms to the cm that a MO call can go ahead.
 *  
 *  
 *
 *  Parameters:
 *  -----------
 *  IN connection_id
 *     cnm_service_success
 *     bearer_capability_1
 *     bearer_capability_2
 *     cm_cc_cause
 *
 *  Returns:
 *
 *
 */


VOID_FUNC   MN_put_cm_mo_call_conf(
connection_id_T            connection_id,
boolean                    cnm_service_success,
cm_bearer_capability_T     bearer_capability_1,
cm_bearer_capability_T     bearer_capability_2,
ie_cm_cc_cause_T           cm_cc_cause,
#if defined(FEATURE_DUAL_SIM )
sys_modem_as_id_e_type     as_id,
#endif
#ifndef  FEATURE_MULTIMEDIA_FALLBACK
ie_cm_cc_reject_T          *cm_cc_reject
#else
#error code not present
#endif /*  FEATURE_MULTIMEDIA_FALLBACK */  
)

{

   /* Locals */

   cm_mo_call_conf_T     message;
   gs_status_T           status;
   int                   message_size;

   /* Set up the message header */
   memset(&message, NULL, sizeof(cm_mo_call_conf_T)); 
   message.message_header.message_set = MS_MN_CM;
   message.message_header.message_id = (word)CM_MO_CALL_CONF;
   message_size = sizeof(cm_mo_call_conf_T) - sizeof(IMH_T);
   PUT_IMH_LEN(message_size,&message.message_header)

   /* Set up the connection id */
   message.connection_id = connection_id;

   /* set up the success / failure indication */
   message.cnm_service_success = cnm_service_success;

   /* set up reject cause value */
   if(cm_cc_reject != NULL)
   {
      message.cm_cc_reject.present = TRUE;

      message.cm_cc_reject = *cm_cc_reject;
   }
   else
   {
      message.cm_cc_reject.present = FALSE;
   }
#ifdef FEATURE_DUAL_SIM 
   if(MNCNM_IS_SINGLE_STACK)
   {
     message.as_id = as_id;
   }
#ifdef FEATURE_DUAL_ACTIVE 
   else
   {
     message.as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
   }
#endif
#endif
   /* set up the data for success */
   if (cnm_service_success == TRUE)
   {
      message.cm_mo_call_details.present = TRUE;

#if defined(FEATURE_MULTIMEDIA_FALLBACK) || defined(FEATURE_REL6)
      /* Repeat indicator */
      if(repeat_indicator_bc != NULL)
      {
          message.cm_mo_call_details.bc_repeat_indicator.present = TRUE;
          message.cm_mo_call_details.bc_repeat_indicator.repeat_indication = repeat_indicator_bc->repeat_indication;
      }
      else
      {
          message.cm_mo_call_details.bc_repeat_indicator.present = FALSE;
          message.cm_mo_call_details.bc_repeat_indicator.repeat_indication = FALSE;
      }

      /* BCIE */
      if(bearer_capability_1.present == TRUE)
      {
          message.cm_mo_call_details.bearer_capability_1 = bearer_capability_1;
      }
      else
      {
          message.cm_mo_call_details.bearer_capability_1.present = FALSE;
      }

      if(bearer_capability_2.present == TRUE)
      {
          message.cm_mo_call_details.bearer_capability_2 = bearer_capability_2;
      }
      else
      {
          message.cm_mo_call_details.bearer_capability_2.present = FALSE;
      }

      /* Network Capability */
      if(network_cc_capability != NULL)
      {
          message.network_cc_capability.present = TRUE;
          message.network_cc_capability.length = network_cc_capability->capability;
          message.network_cc_capability.capability = network_cc_capability->capability;
      }
      else
      {
          message.network_cc_capability.present = FALSE;
          message.network_cc_capability.length = FALSE;
          message.network_cc_capability.capability = FALSE;
      }
#else
      message.cm_mo_call_details.bearer_capability_1 = bearer_capability_1;
      message.cm_mo_call_details.bearer_capability_2 = bearer_capability_2;
#endif /* FEATURE_MULTIMEDIA_FALLBACK */

      /* zero the cm_cc_cause data */
      memset(&message.cm_cc_cause, NULL, sizeof(ie_cm_cc_cause_T));

   }
   else  /* set the failure data */
   {
      message.cm_mo_call_details.present = TRUE;

      message.cm_cc_cause.present = TRUE;

      message.cm_cc_cause = cm_cc_cause;

#if defined(FEATURE_MULTIMEDIA_FALLBACK) || defined(FEATURE_REL6)
      message.cm_mo_call_details.bc_repeat_indicator.present = FALSE;
      message.cm_mo_call_details.bc_repeat_indicator.repeat_indication = FALSE;

      message.network_cc_capability.present = FALSE;
      message.network_cc_capability.capability = FALSE;

      memset(&message.cm_mo_call_details.bearer_capability_1, NULL, sizeof(cm_bearer_capability_T));
      memset(&message.cm_mo_call_details.bearer_capability_2, NULL, sizeof(cm_bearer_capability_T));
#endif
   }

   /* send the message */
   status = MN_send_message(GS_QUEUE_CM, &message, TRUE);

   if (status != GS_SUCCESS)
   {
      MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Message queueing error"); 
   }
}

#if defined(FEATURE_MULTIMEDIA_FALLBACK) || defined(FEATURE_REL6)
/*
 * Function name :  MN_put_cm_mo_modify_complete_conf
 * -------------
 *
 * Description : Indicates to the CM that the modification to  
 * -----------   call status is complete
 *
 *
 *
 *
 *
 *
 * Parameters :  cm_modify_complete_data
 * ----------
 *
 *
 *
 * Returns:      none
 * --------
 *
 */

VOID_FUNC MN_put_cm_mo_modify_complete_conf(
cm_modify_complete_data_T   *cm_modify_complete_data)
{
   cm_modify_complete_conf_T      message;
   gs_status_T                    GSstatus;


   /* clear the buffer */
   memset(&message, NULL, sizeof(cm_modify_complete_conf_T));

   /* Set up the message header */
   message.message_header.message_set = MS_MN_CM;
   message.message_header.message_id = (word)CM_MODIFY_COMPLETE_CONF;
   PUT_IMH_LEN(sizeof(cm_modify_complete_conf_T) - sizeof(IMH_T), &message.message_header)

   /* Set up the connection id */
   message.connection_id = cm_modify_complete_data->connection_id;

   /* Set up the complete data element */
   if ( cm_modify_complete_data->complete == TRUE)
   {
      message.complete =  TRUE;

      message.cm_cc_cause.present = FALSE;
   }
   else
   {
      message.complete  = FALSE;

      message.cm_cc_cause = cm_modify_complete_data->cm_cc_cause;

      message.cm_cc_cause.present = TRUE;
   }


   /* Set up the bearer capability */
   message.cm_bearer_capability = 
       cm_modify_complete_data->cm_bearer_capability;
   message.cm_bearer_capability.present = TRUE;


   /* Set up the low layer compatibility layer data */
   if (cm_modify_complete_data->low_layer_compatibility_present == FALSE )
   {
      message.cm_low_layer_compatibility.present = FALSE;
   }
   else
   {
      message.cm_low_layer_compatibility = 
          cm_modify_complete_data->cm_low_layer_compatibility;
      message.cm_low_layer_compatibility.present = TRUE;

      /* Set up the high layer compatibility layer data */
   }

   if (cm_modify_complete_data->high_layer_compatibility_present == FALSE )
   {
      message.cm_high_layer_compatibility.present = FALSE;
   }
   else
   {
      message.cm_high_layer_compatibility = 
          cm_modify_complete_data->cm_high_layer_compatibility;
      message.cm_high_layer_compatibility.present = TRUE;
   }

   /* Set up the reverse call direction data element */
   message.reverse_call_setup_direction = 
       cm_modify_complete_data->reverse_call_direction;
#ifdef FEATURE_DUAL_SIM 
   if(MNCNM_IS_SINGLE_STACK)
   {
     message.as_id = cm_modify_complete_data->as_id;
   }
#ifdef FEATURE_DUAL_ACTIVE 
   else
   {
     message.as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
   }
#endif
#endif
   /* Send the message */
   GSstatus = MN_send_message(GS_QUEUE_CM, &message, TRUE);

   /* log any failures */
   if(GSstatus != GS_SUCCESS)
   {
      MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Failed to send cm_modify_complete_ind message"); 
     
   }
}
#endif /* FEATURE_MULTIMEDIA_FALLBACK */

/*
 * Function name :  MN_put_cm_mt_call_ind
 * -------------
 *
 * Description :  Creates the above named message
 * -----------    message and sends to the GS_QUEUE_CM
 *                queue.
 *
 *
 * Uses :        
 * ----
 *
 *
 * Parameters:  connection_id
 * ----------
 *
 *
 *
 * Returns:
 * --------
 *
 */

VOID_FUNC MN_put_cm_mt_call_ind(
#if defined(FEATURE_DUAL_SIM )
  sys_modem_as_id_e_type     as_id,
#endif
  connection_id_T  connection_id
  )
{

   /* Locals */
   cm_mt_call_ind_T             message;
   mn_call_information_T        *call_data_ptr;
   int                          message_length;
   gs_status_T                  GSstatus;


   if(connection_id >= MAXNO_CC_TRANSACTIONS)
   {
      MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Connection id invalid for mn_call_information_p");
   }
   else if(mn_call_information_p[connection_id] == NULL)
   {
      MSG_HIGH_DS_1(MN_SUB,"=MNCNM= mn_put_cm_mt_call_ind: mn_call_information_p[%d] is NULL",
              connection_id);
   }
   else
   {
      /* Set up the message header */
      memset(&message, 0x00, sizeof(cm_mt_call_ind_T)); 
      message.message_header.message_set = MS_MN_CM;
      message.message_header.message_id = (word)CM_MT_CALL_IND;
      message_length = sizeof(cm_mt_call_ind_T) - sizeof(IMH_T);
      PUT_IMH_LEN(message_length,&message.message_header)

      /* Set up the connection id */
      message.connection_id = connection_id;

      /* optional network Signal IE */
      call_data_ptr =  mn_call_information_p[connection_id];
#ifdef FEATURE_DUAL_SIM 
      if(MNCNM_IS_SINGLE_STACK)
      {
        message.as_id = as_id;
      }
#ifdef FEATURE_DUAL_ACTIVE 
      else
      {
         message.as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
      }
#endif

#endif
      if(call_data_ptr->signal_present == TRUE) 
      {
          message.signal.present = TRUE;
          message.signal.signal_value = call_data_ptr->SignalValue; 
      }
      else
      {
          message.signal.present = FALSE;
      }

      /* Send the message */
      GSstatus = MN_send_message(GS_QUEUE_CM,&message,TRUE);

      /* log any failures */
      if(GSstatus != GS_SUCCESS)
      {
         MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Failed to send cm_mt_call_ind message"); 
      }
   }
}

/*
 * Function name : MN_put_cm_abort_cc_conf 
 * -------------
 *
 * Description : Sends confirmation to the CM that all calls  
 * -----------   are cleared and CC can be aborted 
 *                
 *
 *
 * Uses :        
 * ----
 *
 *
 * Parameters: none  
 * ----------
 *
 *
 *
 * Returns:  none
 * --------
 *
 */

VOID_FUNC  MN_put_cm_abort_cc_conf( 
#if defined(FEATURE_DUAL_SIM )
   sys_modem_as_id_e_type              as_id
#endif
                                  )
{

   /* Locals */

   cm_abort_cc_conf_T            message;
   gs_status_T                    GSstatus;

   /* Set up the message header */
   memset(&message, 0x00, sizeof(cm_abort_cc_conf_T)); 
   message.message_header.message_set = MS_MN_CM;
   message.message_header.message_id = (word)CM_ABORT_CC_CONF;
#ifdef FEATURE_DUAL_SIM
   if(MNCNM_IS_SINGLE_STACK)
   {
     message.as_id = as_id;
   }
#ifdef FEATURE_DUAL_ACTIVE 
   else
   {
     message.as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
   }
#endif
#endif
   PUT_IMH_LEN(sizeof(cm_abort_cc_conf_T) - sizeof(IMH_T),&message.message_header);


   /* Send the message */
   GSstatus = MN_send_message(GS_QUEUE_CM,&message,TRUE);

   /* log any failures */
   if(GSstatus != GS_SUCCESS)
   {
      MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Failed to send cm_abort_cc_conf message");
   }
}

/*
 * Function name :  MN_put_cm_mt_end_call_ind
 * -------------
 *
 * Description :  Creates the above named message
 * -----------    message and sends to the GS_QUEUE_MN
 *                queue.
 *
 *
 * Uses :        
 * ----
 *
 *
 * Parameters:  connection_id,
 * ----------   cause_value,
 *              
 *
 *
 * Returns:
 * --------
 *
 */

VOID_FUNC MN_put_cm_mt_end_call_ind(connection_id_T    connection_id,
ie_cm_cc_cause_T  cm_cc_cause,
#if defined(FEATURE_DUAL_SIM )
sys_modem_as_id_e_type     as_id,
#endif
ie_cm_cc_reject_T  *cm_cc_reject)
{
   /* Locals */
   cm_mt_end_call_ind_T   message;
   int                    message_size;

   /* Set up the message header */
   memset(&message, 0x00, sizeof(cm_mt_end_call_ind_T)); 
   message.message_header.message_set = MS_MN_CM;
   message.message_header.message_id = (word)CM_MT_END_CALL_IND;
   message_size = sizeof(cm_mt_end_call_ind_T) - sizeof(IMH_T);
   PUT_IMH_LEN(message_size,&message.message_header)

   /* Set up the connection id */
   message.connection_id = connection_id;
#ifdef FEATURE_DUAL_SIM 
   if(MNCNM_IS_SINGLE_STACK)
   {
     message.as_id = as_id;
   }
#ifdef FEATURE_DUAL_ACTIVE 
   else
   {
     message.as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
   }
#endif
#endif
   /* Set up the cause data */
   if(cm_cc_cause.present == TRUE)
   {
       message.cm_cc_cause = cm_cc_cause;
   }
   else if(mn_call_information_p[connection_id] != NULL && mn_call_information_p[connection_id]->cm_cc_cause.present == TRUE)
   {
       message.cm_cc_cause = mn_call_information_p[connection_id]->cm_cc_cause;
   }
   else
   {
       message.cm_cc_cause.present = FALSE;
   }

   /*
   * Fill mo_disconnect_cause value in struct cm_mt_end_call_ind_T
   */
   message.mo_disconnect_cause.present = FALSE; /* Default value of cause present field is 0. */

   if(mn_call_information_p[connection_id] != NULL)
   {
       if(mn_call_information_p[connection_id]->cm_cc_cause.present == TRUE)
       {
          message.mo_disconnect_cause = mn_call_information_p[connection_id]->cm_cc_cause;
       }
   }


   /* reject cause */
   if(cm_cc_reject != NULL)
   {
       message.cm_cc_reject = *cm_cc_reject;
       message.cm_cc_reject.present = TRUE;
   }

   /* Send the message */
   (void)MN_send_message(GS_QUEUE_CM, &message, TRUE);
}



/*
 * Function name :  MN_put_cm_mt_disc_ind
 * -------------
 *
 * Description :  Creates the above named message
 * -----------    message and sends to the GS_QUEUE_MN
 *                queue.
 *
 *
 * Uses :        
 * ----
 *
 *
 * Parameters:  connection_id,
 * ----------   cause_value,
 *              inband_tone_present
 *              
 *
 *
 * Returns:
 * --------
 *
 */
 /*when Disconnect contains progress indicator send Disc_ind with cause and status of inband tone */

VOID_FUNC MN_put_cm_mt_disc_ind( connection_id_T     connection_id,
ie_cm_cc_cause_T   *cm_cc_cause,
boolean            inband_tone_present)
{

   /* Locals */

   cm_mt_disc_ind_T             message;
   gs_status_T                   GSstatus;
   int                           message_size;

   /* Set up the message header */
   memset(&message, 0x00, sizeof(cm_mt_disc_ind_T)); 
   message.message_header.message_set = MS_MN_CM;
   message.message_header.message_id = (word)CM_MT_DISC_IND;
   message_size = sizeof(cm_mt_disc_ind_T) - sizeof(IMH_T);
   PUT_IMH_LEN(message_size,&message.message_header)


   /* Set up the connection id */
   message.connection_id = connection_id;

   /* Set up the cause data */
   message.cm_cc_cause = *cm_cc_cause;
   message.inband_tone_present = inband_tone_present;
#ifdef FEATURE_DUAL_SIM 
   if(MNCNM_IS_SINGLE_STACK)
   {
     message.as_id = mn_call_information_p[connection_id]->as_id;
   }
#ifdef FEATURE_DUAL_ACTIVE 
   else
   {
     message.as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
   }
#endif
#endif
   /* Send the message */
   GSstatus = MN_send_message(GS_QUEUE_CM,&message,TRUE);

   /* log any failures */
   if(GSstatus != GS_SUCCESS)
   {
      MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Failed to send cm_end_mt_disc_ind message");
   }
}

#ifdef FEATURE_UUS
/*
 * Function name :  MN_put_cm_mt_user_data_ind
 * -------------
 *
 * Description :  Creates the above named message
 * -----------    message and sends to the GS_QUEUE_CM
 *                queue.
 *
 *
 * Uses :        
 * ----
 *
 *
 * Parameters:  connection_id,user_user_data,
 * ----------   user_user_information_length,
 *              user_user_protocol_discriminator,
 *              more_data
 *
 *
 * Returns:
 * --------
 *
 */

VOID_FUNC MN_put_cm_mt_user_data_ind(
connection_id_T  connection_id,
byte           user_user_data[MAX_USER_USER_DATA_LENGTH],
byte           user_user_information_length,
byte           user_user_protocol_discriminator,
boolean        more_data,
boolean        deferred)

{
   MNCC_USER_DATA_DEFERRED_IND_T message_deferred;
   cm_mt_user_data_ind_T         message;
   int                           i;
   int                           message_size;
   gs_status_T                   GSstatus;


   /* check for excessive size */
   if (user_user_information_length > MAX_USER_USER_DATA_LENGTH)
   {
       MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: cm_mt_user_data_ind message too long");
   }
   else
   {
      /* Set up the message header */
      memset(&message, 0x00, sizeof(cm_mt_user_data_ind_T)); 
      message.message_header.message_set = MS_MN_CM;
      message.message_header.message_id = (word)CM_MT_USER_DATA_IND;
      message_size = sizeof(cm_mt_user_data_ind_T) - sizeof(IMH_T);
      PUT_IMH_LEN(message_size,&message.message_header)


      /* Set up the connection id */
      message.connection_id = connection_id;
#ifdef FEATURE_DUAL_SIM 
      if(MNCNM_IS_SINGLE_STACK)
      {
        if (mn_call_information_p[connection_id] != NULL)
        {
          message.as_id = mn_call_information_p[connection_id]->as_id;
        }
        else
        {
          message.as_id = mn_as_id;
        }
      }
#ifdef FEATURE_DUAL_ACTIVE 
      else
      {
        message.as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
      }
#endif
#endif
      message.user_user_data_length = user_user_information_length;
      message.user_user_protocol_discriminator = user_user_protocol_discriminator;

      for (i=0;i < user_user_information_length;i++)
      {
         message.user_user_data[i] = user_user_data[i];
      }

      message.more_data_indicator = more_data;

      if(!deferred) 
      {
          /* Send the message */
          GSstatus = MN_send_message(GS_QUEUE_CM,&message,TRUE);
      }
      else
      {
          /* Set up the deferred message header */
          message_deferred.message_header.message_set = MS_MN_CC;
          message_deferred.message_header.message_id = MNCC_USER_DATA_DEFERRED_IND;
#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
          {
            message_deferred.as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
          }
#endif
          message_size = sizeof(MNCC_USER_DATA_DEFERRED_IND_T) - sizeof(IMH_T);
          PUT_IMH_LEN(message_size, &message_deferred.message_header)

#ifndef FEATURE_MODEM_HEAP
          if((message_deferred.user_data_ind = gs_alloc(sizeof(cm_mt_user_data_ind_T))) != NULL)
#else
          if((message_deferred.user_data_ind = modem_mem_calloc(1, sizeof(cm_mt_user_data_ind_T), MODEM_MEM_CLIENT_NAS)) != NULL)
#endif
          {
             memscpy(message_deferred.user_data_ind, sizeof(cm_mt_user_data_ind_T) ,&message, sizeof(cm_mt_user_data_ind_T));
          }
          else
          {
             MSG_FATAL_MN_DS(MN_SUB,"=MNCNM= gs_alloc() failed in MN", 0, 0, 0);
          }

          /* Send the message */
          GSstatus = MN_send_message(GS_QUEUE_MN_CM, &message_deferred, TRUE);
      }

      /* log any failures */
      if(GSstatus != GS_SUCCESS)
      {
         MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Failed to send cm_mt_user_data_ind message"); 
      }
   }
}


/*
 * Function name :  MN_put_cm_congestion_ind
 * -------------
 *
 * Description :  Creates the above named message
 * -----------    message and sends to the GS_QUEUE_CM
 *                queue.
 *
 *
 * Uses :        
 * ----
 *
 *
 * Parameters:  connection_id,i cause, receiver_ready,
 * ----------   
 *
 * Returns:
 * --------
 *
 */

VOID_FUNC MN_put_cm_congestion_ind(connection_id_T  connection_id, byte cause, boolean receiver_busy)
{
    cm_user_congestion_ind_T  message;
    gs_status_T               GSstatus;
    byte                      message_size;


    /* Set up the message header */
    memset(&message, 0x00, sizeof(cm_user_congestion_ind_T)); 

    message.message_header.message_set = MS_MN_CM;
    message.message_header.message_id = (word)CM_USER_CONGESTION_IND;
    message_size = sizeof(cm_user_congestion_ind_T) - sizeof(IMH_T);
    PUT_IMH_LEN(message_size,&message.message_header)

    message.connection_id = connection_id;
    message.receiver_busy = receiver_busy;
#ifdef FEATURE_DUAL_SIM 
    if(MNCNM_IS_SINGLE_STACK)
    {
      message.as_id = mn_call_information_p[connection_id]->as_id;
    }
#ifdef FEATURE_DUAL_ACTIVE 
    else
    {
     message.as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
    }
#endif
#endif
    if(cause)
    {
        message.cm_cc_cause.present = TRUE;
        message.cm_cc_cause.coding_standard = STD_CODESTANDARD;
        message.cm_cc_cause.location = STD_LOCATION;
        message.cm_cc_cause.cause_value = cause;
    }

    /* Send the message */
    GSstatus = MN_send_message(GS_QUEUE_CM,&message,TRUE);
 
    /* log any failures */
    if(GSstatus != GS_SUCCESS)
    {
       MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Failed to send cm_user_congestion_ind message"); 
    }
}

/*
 * Function name :  MN_process_MNCC_USER_DATA_DEFERRED_IND
 * -------------
 *
 * Description :  
 * -----------  
 *     Now pass the deferred notify SS message to CM.
 *
 * Uses :
 * ----
 *
 *
 * Parameters:  
 * ----------  
 *    IMH_T *message_ptr - Inter-Task message header.
 *
 * Returns:
 * --------
 *
 */

VOID_FUNC MN_process_MNCC_USER_DATA_DEFERRED_IND(IMH_T  *message_ptr)
{
   MNCC_USER_DATA_DEFERRED_IND_T             *message;
   cm_mt_user_data_ind_T  *user_data_ind;

   message = (MNCC_USER_DATA_DEFERRED_IND_T *)message_ptr;

   if(message != NULL)
   {
       user_data_ind = (cm_mt_user_data_ind_T *)message->user_data_ind;
       MN_put_cm_mt_user_data_ind(user_data_ind->connection_id,
                                  user_data_ind->user_user_data,
                                  user_data_ind->user_user_data_length,
                                  user_data_ind->user_user_protocol_discriminator,
                                  user_data_ind->more_data_indicator,
                                  FALSE);

   
#ifndef FEATURE_MODEM_HEAP
       gs_free(message->user_data_ind);
#else
       modem_mem_free(message->user_data_ind, MODEM_MEM_CLIENT_NAS);
#endif
   }
   else
   {
       MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Invalid deferred notify SS indicator");
   }
}


#endif


/*
 * Function name :  MN_put_cm_start_dtmf_conf
 * -------------
 *
 * Description :  Creates the above named message
 * -----------    message and sends to the GS_QUEUE_CM
 *                queue.
 *
 *
 * Uses :        
 * ----
 *
 *
 * Parameters:  connection_id,dtmf_success,
 * ----------
 *              
 *
 *
 * Returns:
 * --------
 *
 */

VOID_FUNC MN_put_cm_start_dtmf_conf(
    connection_id_T        connection_id,
#if defined(FEATURE_DUAL_SIM )
    ie_cm_cc_reject_T      cm_cc_reject,
    sys_modem_as_id_e_type as_id,
#endif
    boolean                dtmf_success
    )
{

   /* Locals */

   cm_start_dtmf_conf_T              message;
   gs_status_T                        GSstatus;
   int                                message_size;

   /* Set up the message header */
   memset(&message, 0x00, sizeof(cm_start_dtmf_conf_T)); 
   message.message_header.message_set = MS_MN_CM;
   message.message_header.message_id = (word)CM_START_DTMF_CONF;
   message_size = sizeof(cm_start_dtmf_conf_T) - sizeof(IMH_T);
   PUT_IMH_LEN(message_size,&message.message_header)

   /* Set up the success indicator */
   message.dtmf_success = dtmf_success;
   message.connection_id = connection_id;

#ifdef FEATURE_DUAL_SIM 
   if(MNCNM_IS_SINGLE_STACK)
   {
     message.cm_cc_reject = cm_cc_reject;
     message.as_id = as_id;
   }
#ifdef FEATURE_DUAL_ACTIVE 
   else
   {
     message.as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
   }
#endif
#endif
   /* Send the message */
   GSstatus = MN_send_message(GS_QUEUE_CM,&message,TRUE);

   /* log any failures */
   if(GSstatus != GS_SUCCESS)
   {
      MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Failed to send cm_start_dtmf_conf message");
   }
}

/*
 * Function name :  MN_put_cm_stop_dtmf_conf
 * -------------
 *
 * Description :  Creates the above named message
 * -----------    message and sends to the GS_QUEUE_CM
 *                queue.
 *
 *
 * Uses :        
 * ----
 *
 *
 * Parameters:  connection_id.
 * ----------
 *              
 *
 *
 * Returns:
 * --------
 *
 */

VOID_FUNC MN_put_cm_stop_dtmf_conf(
   connection_id_T        connection_id
#if defined(FEATURE_DUAL_SIM )
   , ie_cm_cc_reject_T      cm_cc_reject
   , sys_modem_as_id_e_type as_id
#endif
  )
{

   /* Locals */

   cm_stop_dtmf_conf_T              message;
   gs_status_T                       GSstatus;
   int                               message_size;

   /* Set up the message header */
   memset(&message, 0x00, sizeof(cm_stop_dtmf_conf_T)); 
   message.message_header.message_set = MS_MN_CM;
   message.message_header.message_id = (word)CM_STOP_DTMF_CONF;
   message_size = sizeof(cm_stop_dtmf_conf_T) - sizeof(IMH_T);
   PUT_IMH_LEN(message_size,&message.message_header)

   message.connection_id = connection_id;
#ifdef FEATURE_DUAL_SIM 
   if(MNCNM_IS_SINGLE_STACK)
   {
     message.cm_cc_reject = cm_cc_reject;
     message.as_id = as_id;
   }
#ifdef FEATURE_DUAL_ACTIVE 
   else
   {
     message.as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
   }
#endif
#endif
   /* Send the message */
   GSstatus = MN_send_message(GS_QUEUE_CM,&message,TRUE);

   /* log any failures */
   if(GSstatus != GS_SUCCESS)
   {
      MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Failed to send cm_stop_dtmf_conf message"); 
   }
}


/*
 * Function name :  MN_put_cm_rab_released_ind
 * -------------
 *
 * Description :  Creates the above named message
 * -----------    message and sends to the GS_QUEUE_MN
 *                queue.
 *
 * Uses :        
 * ----
 *
 * Parameters:  
 * ----------  
 *              
 * Returns:
 * --------
 *
 */

VOID_FUNC MN_put_cm_rab_released_ind(
#if defined(FEATURE_DUAL_SIM )
        IMH_T * msg
#endif
)
{
   /* Locals */
   cm_cs_sig_rel_ind_T             message;
   gs_status_T                     GSstatus;
   int                             message_size;

   /* Set up the message header */
   memset(&message, 0x00, sizeof(cm_cs_sig_rel_ind_T)); 
   message.message_header.message_set = MS_MN_CM;
   message.message_header.message_id = (word)CM_CS_SIG_REL_IND;
#ifdef FEATURE_DUAL_SIM
   if(MNCNM_IS_SINGLE_STACK)
   {
     message.as_id = ((MNCC_RAB_REL_IND_T *)((void *)msg))->as_id;
   }
#ifdef FEATURE_DUAL_ACTIVE 
   else
   {
     message.as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
   }
#endif
#endif
   message_size = sizeof(cm_cs_sig_rel_ind_T) - sizeof(IMH_T); 
   PUT_IMH_LEN(message_size, &message.message_header)

   /* Send the message */
   GSstatus = MN_send_message(GS_QUEUE_CM, &message, TRUE);

   /* log any failures */
   if(GSstatus != GS_SUCCESS)
   {
      MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Failed to send cm_end_mt_disc_ind message");
   }

   /* SMS link control processing */
   if(sms_link_control.status == SMS_LINK_CONTROL_ACTIVE)   /* Active && Timer is running */
   {
       MN_stop_sms_timer(sms_link_control.connection_id, TLC1M);

       /* Update the link status */
       sms_link_control.status = SMS_LINK_CONTROL_INACTIVE;

       /* Reset link control mode */
       sms_link_control.mode = ((sms_link_control.mode == SMS_LINK_CONTROL_ENABLED_ONE) ? 
                                   SMS_LINK_CONTROL_DISABLED : sms_link_control.mode);
         
       /* send report the UASMS */
      MN_put_uasms_link_control_report_ind(
                        sms_link_control.mode, 
#if defined(FEATURE_DUAL_SIM )
                        ((MNCC_RAB_REL_IND_T *)((void *)msg))->as_id,
#endif
                        SMS_LINK_CONTROL_CONNECTION_RELEASED);

   }

   /* AT+CMMS=1: No MM connection for transporting the deferred CP_ACK */
   sms_link_control.cp_ack_deferred = FALSE;

   /* Safety check to make sure vocoder is release when MM connection is released */
#if defined(FEATURE_DUAL_SIM )
   if(MNCNM_IS_SINGLE_STACK)
   {
     MN_disconnect_TCH_with_AS_ID(MN_VOCODER_OFF, message.as_id);
   }
   else
#endif
   {
     MN_disconnect_TCH(MN_VOCODER_OFF);
   }

   //tch_user_connections_store[DEFAULT_RAB_ID].OTA_channel_connected = FALSE;
}

/*
 *   Function name : MN_put_cm_srvcc_ho_complete_ind
 *   ---------------
 *
 *   Description:
 *   ------------
 *  This function generates the cm_srvcc_handover_complete_ind message to the 
 *  mmi.
 *  
 *  
 *
 *  Parameters:
 *  -----------
 *  IN connection_id
 *
 *  Returns:
 *
 *
 */

VOID_FUNC MN_put_cm_srvcc_ho_complete_ind(boolean success)
{
   cm_srvcc_handover_complete_ind_T     *message;
   gs_status_T                          status;
   unsigned int                         message_size;


   /* If there are no connection id's left */
#ifndef FEATURE_MODEM_HEAP
        message = (cm_srvcc_handover_complete_ind_T *)gs_alloc(sizeof(cm_srvcc_handover_complete_ind_T)); 
#else
        message = (cm_srvcc_handover_complete_ind_T *)modem_mem_calloc(1, sizeof(cm_srvcc_handover_complete_ind_T),
                                                                             MODEM_MEM_CLIENT_NAS);
#endif

        if(message != NULL)
        { 
          /* Set up the message header */
          memset(message, 0x00, sizeof(cm_srvcc_handover_complete_ind_T)); /* init auto data */
          message->message_header.message_set = MS_MN_CM;
          message->message_header.message_id = (word)CM_NAS_SRVCC_HANDOVER_COMPLETE_IND;

          message_size = sizeof(cm_srvcc_handover_complete_ind_T) - sizeof(IMH_T);
          message->message_header.message_len_msb = (unsigned char)(message_size / 0x100);
          message->message_header.message_len_lsb = (unsigned char)(message_size % 0x100);

#ifdef FEATURE_DUAL_SIM 
          if(MNCNM_IS_SINGLE_STACK)
          {
            if(mn_as_id != SYS_MODEM_AS_ID_NONE)
            {
               message->as_id = mn_as_id;
            }
            else
            {
               MSG_HIGH_DS_0(MN_SUB,"=MNCNM= Error! mn_as_id is NULL ");
            }
          } 
#ifdef FEATURE_DUAL_ACTIVE 
          else
          {
            message->as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
          }
#endif
#endif
         message->success = success;
         MSG_HIGH_DS_1(MN_SUB,"=MNCNM= success = %d in CM_NAS_SRVCC_HANDOVER_COMPLETE_IND",success);

          /* Send message */
          status = MN_send_message(GS_QUEUE_CM, message, TRUE);
          if ( status != GS_SUCCESS)
          {
             MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Send CM_NAS_SRVCC_HANDOVER_COMPLETE_IND message failure");
          }

#ifndef FEATURE_MODEM_HEAP
          gs_free( message );
#else
          modem_mem_free(message, MODEM_MEM_CLIENT_NAS);
#endif
        }
        else
        {
           MSG_HIGH_DS_0(MN_SUB,"=MNCNM= SRVCC HO : memory allocation failied ");
        }

}

/*
 *   Function name : MN_put_MNCC_SRVCC_HANDOVER_FAIL_REQ
 *   ---------------
 *
 *   Description:
 *   ------------
 *  This function generates the MNCC_SRVCC_HANDOVER_FAIL_REQ message and sends to the GS_QUEUE_MN_CM queue 
 *  
 *  
 *  
 *
 *  Parameters:
 *  -----------
 *  
 *
 *  Returns:
 *
 *
 */

VOID_FUNC  MN_put_MNCC_SRVCC_HANDOVER_FAIL_REQ(void)
{
   MNCC_SRVCC_HANDOVER_FAIL_REQ_T *message; 
   gs_status_T                          status;
   unsigned int                         message_size;


   
#ifndef FEATURE_MODEM_HEAP
    message = (MNCC_SRVCC_HANDOVER_FAIL_REQ_T *)gs_alloc(sizeof(MNCC_SRVCC_HANDOVER_FAIL_REQ_T)); 
#else
    message = (MNCC_SRVCC_HANDOVER_FAIL_REQ_T *)modem_mem_calloc(1, sizeof(MNCC_SRVCC_HANDOVER_FAIL_REQ_T),
                                                                             MODEM_MEM_CLIENT_NAS);
#endif

    if(message != NULL)
    { 
       /* Set up the message header */
       message->message_header.message_set = MS_MN_CC;
       message->message_header.message_id = (word)MNCC_SRVCC_HANDOVER_FAIL_REQ;

       message_size = sizeof(MNCC_SRVCC_HANDOVER_FAIL_REQ_T) - sizeof(IMH_T);
       message->message_header.message_len_msb = (unsigned char)(message_size / 0x100);
       message->message_header.message_len_lsb = (unsigned char)(message_size % 0x100);

#ifdef FEATURE_DUAL_SIM 
#ifdef FEATURE_DUAL_ACTIVE
       message->as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
#endif
#endif
        /* Send message */
        status = MN_send_message(GS_QUEUE_MN_CM, message, TRUE);
        if ( status != GS_SUCCESS)
        {
           MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Send MNCC_SRVCC_HANDOVER_FAIL_REQ message failure");
        }

#ifndef FEATURE_MODEM_HEAP
        gs_free( message );
#else
        modem_mem_free(message, MODEM_MEM_CLIENT_NAS);
#endif
     }
     else
     {
        MSG_ERROR_DS_0(MN_SUB,"=MNCNM= SRVCC HO : memory allocation failied ");
     }

}


/*
 *   Function name : MN_put_cm_get_srvcc_context_req
 *   ---------------
 *
 *   Description:
 *   ------------
 *  This function generates the CM_GET_SRVCC_CONTEXT_REQ message to CM 
 * 
 *  
 *  
 *
 *  Parameters:
 *  -----------
 *  IN active_rat
 *
 *  Returns:
 *
 *
 */


VOID_FUNC  MN_put_cm_get_srvcc_context_req(sys_radio_access_tech_e_type  active_rat)
{
   cm_get_srvcc_context_req_T  *message;
   gs_status_T                  status;
   unsigned int                 message_size;

#ifndef FEATURE_MODEM_HEAP
        message = (cm_get_srvcc_context_req_T *)gs_alloc(sizeof(cm_get_srvcc_context_req_T)); 
#else
        message = (cm_get_srvcc_context_req_T *)modem_mem_calloc(1, sizeof(cm_get_srvcc_context_req_T),
                                                                             MODEM_MEM_CLIENT_NAS);        
#endif

        if(message != NULL)
        { 
          /* Set up the message header */
          memset(message, 0x00, sizeof(cm_get_srvcc_context_req_T)); /* init auto data */
          message->message_header.message_set = MS_MN_CM;
          message->message_header.message_id = (word)CM_GET_SRVCC_CONTEXT_REQ;

          message_size = sizeof(cm_get_srvcc_context_req_T) - sizeof(IMH_T);
          message->message_header.message_len_msb = (unsigned char)(message_size / 0x100);
          message->message_header.message_len_lsb = (unsigned char)(message_size % 0x100);

          message->active_rat = active_rat;
          MSG_HIGH_DS_1(MN_SUB,"=MNCNM= Sending Active RAT as %d to CM in CM_GET_SRVCC_CONTEXT_REQ",active_rat);

#ifdef FEATURE_DUAL_SIM 
          if(MNCNM_IS_SINGLE_STACK == TRUE)
          {
            if(mn_as_id != SYS_MODEM_AS_ID_NONE)
            {
              message->as_id = mn_as_id;
            }
            else
            {
              MSG_HIGH_DS_0(MN_SUB,"=MNCNM= Error! mn_as_id is NULL ");
            }
          } 
#ifdef FEATURE_DUAL_ACTIVE 
          else
          {
            message->as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
          }
#endif
#endif

          /* Send message */
          status = MN_send_message(GS_QUEUE_CM, message, TRUE);
          if ( status != GS_SUCCESS)
          {
             MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Send CM_GET_SRVCC_CONTEXT_REQ message failure");
          }

#ifndef FEATURE_MODEM_HEAP
          gs_free( message );
#else
          modem_mem_free(message, MODEM_MEM_CLIENT_NAS);
#endif
        }
        else
        {
           /* Actually this should be Fatal error and a trigger to crash */
           MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Error! Couldn't send CM_GET_SRVCC_CONTEXT_REQ : mem alloc failed ");
        }

}

/*
 * Function name :  MN_put_cm_call_alert_ind
 * -------------
 *
 * * Description : Creates the cm_mo_call_alert_ind
 * ----------- message to the mmi
 *
 * Uses :        
 * ----
 *
 * Parameters: connection_id
 * ----------  
 *              
 * Returns:
 * --------
 *
 */

VOID_FUNC MN_put_cm_call_alert_ind(connection_id_T connection_id)
{
   /* locals */
   cm_mo_call_alert_ind_T message;
   gs_status_T GSstatus;
   int message_length;


   /* Set up the header */
   memset(&message, 0x00, sizeof(cm_mo_call_alert_ind_T));
   message.message_header.message_set = MS_MN_CM;
   message.message_header.message_id = (word)CM_MO_CALL_ALERT_IND;
   message_length = sizeof(cm_mo_call_alert_ind_T) - sizeof(IMH_T);

   PUT_IMH_LEN(message_length,&message.message_header)
   /* Set the connection id */
   message.connection_id = connection_id;


#ifdef FEATURE_DUAL_SIM
 	if(MNCNM_IS_SINGLE_STACK)
 	{
 	  if (mn_call_information_p[connection_id] != NULL)
 	  {
 	    message.as_id = mn_call_information_p[connection_id]->as_id;
 	  }
 	  else
 	  {
 	     message.as_id = mn_as_id;
 	   }
 	 }
#ifdef FEATURE_DUAL_ACTIVE
    else
 	{
 	  message.as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
 	}
#endif
#endif

   /* Send the message */
   GSstatus = MN_send_message(GS_QUEUE_CM,&message,TRUE);

   /* log any failures */
   if(GSstatus != GS_SUCCESS)
   {
     MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: cm_mo_call_alert_ind message failed");
   }

}

