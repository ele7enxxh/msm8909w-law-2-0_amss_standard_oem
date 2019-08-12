/*===========================================================================
                       COPYRIGHT INFORMATION

Copyright (c) 2011, 2012 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

                       EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/cnm/vcs/cnm_cc_process_specific_msgs1.c_v   1.7   12 Jul 2002 17:12:22   abanh  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/cnm/src/cnm_cc_process_specific_msgs1.c#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------

08/06/01   CD      Clean-up of include header files 

08/14/01   AB      Renamed connection manager references to CNM from CM.

11-Mar-2002   VT   Removed Lint errors.

03/26/02   CD      Fixed Kovu compile warning

04/19/02   CD      In CNM_process_release_complete(), added sending a MMCNM_REL_REQ
                   when the last connection is processed

                   Added usage of CNM_send_mm_rel_cnf()
                   
                   Changed ERR macro to MSG_ERROR
                   Updated Copyright

05/07/02    CD    Changed release type enum due to name collision with RR

6/24/02     CD    In CNM_process_release_complete_message(), added check for cc_state 
                  not in idle before calling function to transtition to idle

07/01/02    CD    In CNM_process_setup_message() added passing the cause of the 
                  rejection back to CM

                  Removed redundant declaration of is_gsm_mode()
                  Removed access out-of-bounds lint errors

07/02/02    CD    Added processing of cause of no CLI in an MT SETUP message

08/14/02    AB    In CNM_process_disconnect_message(), added handling for
                  Progress IE (inband), for possible inband info.

09/04/02    AB    IN CNM_process_setup_message(), added handling for
                  Redirecting Party BCD IE and Redirectory Party Subaddress IE.

09/18/02    AB    In CNM_process_release_message(), do not sent a 
                  RELEASE COMPLETE until MN comleted processing of
                  RELEASE message to allows SS pending transaction
                  on MN_CNM queue to be processed before the channel
                  is released (via check for last connection).

09/18/02    AB    In CNM_process_setup_message(), forward the BC to 
                  higher application (CM/UI) layer, even though BC IE
                  failed syntax check.  The BC IE provides the CM/UI 
                  enough data (CS Data or Voice) to re-negotiate 
                  the BC with the network in subsequence CALL_CONFIRM 
                  message.
               
02/12/03    AB    Replaced %l with %d in ERR() macro per Lint.

04/16/03    AB    Updated the new MNCC_REJ_IND length.

05/10/05    HS    Added support for FEATURE_CCBS

01/18/06    AB    Added supports FEATURE_MULTIMEDIA_FALLBACK.

06/14/06    NR    Sending Status Messages in response to SETUP message
                  in case of SCUDIF Call

07/14/06    NR    Removing redundant variables global_flag_cpdata_arriving 
                  & global_flag_ccrelease_arriving

12/09/06    RD    Additional checks for BC with ITC = 6 
 
07/04/11    AM    Putting range check for fixing buffer overflow of cc_IE_info.
===========================================================================*/


#include "mmcp_variation.h"
#include "customer.h"
#include "rex.h"
#include "err.h"
#include "environ.h"

#include "com_iei_v.h"
#include "mn.h"
#include "mnglobal.h"
#include "cc_cc.h"
#include "ms.h"
#include "timers_v.h"
#include "ms_cc_mm_v.h"
#include "ms_timer_v.h"
#include "cause.h"
#include "cmlib.h"
#include "cnm.h"
#include "nasutils.h"
#include "geran_grr_api.h"
#include <stringl/stringl.h>


/*
** Globals
*/

/**************** Begin of   CNM_process_alerting_message *************************
 *
 *  Function name: CNM_process_alerting_message()
 *  --------------------------------------------
 *
 *  Description:
 *  ------------
 *  This function handles an alerting message received from the network. It walks
 *  through the IEs in the message checking for validity, and if all ok, sends
 *  the request on to MN, otherwise a status message is sent.
 *  This function assumes that the message has already been checked for IE
 *  content
 *
 *
 *  Parameters:
 *  -----------
 *  connection_id
 *
 *  Returns:
 *  --------
 *  none
 *
 ******************************************************************************/

VOID_FUNC   CNM_process_alerting_message( connection_id_T connection_id, boolean syntax_ok, byte cause)
{

   byte                       IEI;

   byte                       IE_count;

   MNCC_ALERT_IND_T           mncc_alerting_ind;

   /* Initialize buffer */
   memset(&mncc_alerting_ind, NULL, sizeof(mncc_alerting_ind));

   IE_count = 2;              /* start at first IE after message type */

   while ( (syntax_ok != FALSE) && ( IE_count < no_of_IEs ))
   {

  /*
   * get information of the next IE from message
   */
      IEI       = cc_IE_info[IE_count].IEI;
      if(cc_IE_info[IE_count].syntax_ok == TRUE)
      {
        switch (IEI)
        {
           case IEI_FACILITY:
           case IEI_PROGRESS_INDICATOR:
#ifdef FEATURE_UUS
           case IEI_USER_USER:
#endif
              break;

           default:                            /* this cannot happen! */
              MSG_ERROR_DS_1(MN_SUB,"=MNCNM= Unexpected IE in alerting msg: received IEI = %d\n",
                         (dword) IEI);

              syntax_ok = FALSE;
              break;
        }     /* end case */
      }

      IE_count++;       /* index next IE in the message */


   }   /* END WHILE more IEs and Syntax is OK */


   if ( syntax_ok)
   {
      mncc_alerting_ind.message_header.message_set = MS_MN_CC;
      mncc_alerting_ind.message_header.message_id  = MNCC_ALERT_IND;
      mncc_alerting_ind.connection_id = connection_id;

#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
      {
        mncc_alerting_ind.as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
      }
#endif


      PUT_IMH_LEN( (sizeof(MNCC_ALERT_IND_T) - sizeof(IMH_T)),
                   &mncc_alerting_ind.message_header);

      CNM_send_message( (IMH_T *)&mncc_alerting_ind,
                       GS_QUEUE_MN_CM );
   }
   else
   {
     CNM_send_status(connection_id,
                       PD_CC,
                       cause);
   }

}
/**************** End of   CNM_process_alerting_message ***************************/

/**************** Begin of   CNM_process_call_proceeding_message ******************
 *
 *  Function name: CNM_process_call_proceeding_message()
 *  ---------------------------------------------------
 *
 *  Description:
 *  ------------
 *  This function handles the call proceeding message from the network. It walks
 *  through the IEs in the message checking for validity, and if all ok, sends
 *  the request on to MN, otherwise a status message is sent.
 *  This function assumes that the message has already been checked for IE
 *  content
 *
 *
 *  Parameters:
 *  -----------
 *  connection_id : IN
 *
 *  Returns:
 *  --------
 *  none
 *
 ******************************************************************************/

VOID_FUNC   CNM_process_call_proceeding_message( connection_id_T connection_id, boolean syntax_ok, byte cause)
{

   byte                       *p_IE;
   byte                       *p_write_point;
   byte                       *last_octet_write_point;
   byte                       IEI;
   boolean                    BC1;
   boolean                    BC2;
   boolean                    repeat_ind_present;
   byte                       len;
   byte                       IE_count;
   byte                       IE_length;
   MNCC_CALL_PROC_IND_T       mncc_call_proc_ind;

   /* Initialize buffer */
   memset(&mncc_call_proc_ind, NULL, sizeof(mncc_call_proc_ind));

   repeat_ind_present = FALSE;
   BC1 = FALSE;
   BC2 = FALSE;
   len = 0;
   p_IE = (byte *) 0;
   IE_count = 2;              /* start at first IE after message type */
   p_write_point = (byte *)mncc_call_proc_ind.data;
   last_octet_write_point = (byte *)&mncc_call_proc_ind.data[sizeof(mncc_call_proc_ind.data)-1];

   mncc_call_proc_ind.message_header.message_set = MS_MN_CC;
   mncc_call_proc_ind.message_header.message_id  = MNCC_CALL_PROC_IND;
   mncc_call_proc_ind.connection_id = connection_id;

#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
   {
     mncc_call_proc_ind.as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
   }
#endif


   while ( (syntax_ok != FALSE) && ( IE_count < no_of_IEs ))
   {

     /*
      * get information of the next IE from message
      */

      IEI       = cc_IE_info[IE_count].IEI;
      p_IE      = cc_IE_info[IE_count].start;
      IE_length = cc_IE_info[IE_count].length;

      if(cc_IE_info[IE_count].syntax_ok == TRUE)
      {
        switch (IEI)
         {
            case IEI_FACILITY:
            case IEI_PROGRESS_INDICATOR:
               break;

            case IEI_REPEAT_INDICATOR:

               /*
                * record that we have received a repeat indicator,
                *  but we don't yet know what its referring to
                */
              {
                repeat_ind_present = TRUE;
                (void) memscpy(p_write_point, (last_octet_write_point - p_write_point + 1) ,p_IE, IE_length);
                p_write_point = ((byte *)(p_write_point)) + IE_length;
              }

               break;

            case IEI_BEARER_CAPABILITY:
               {
                 if ( BC1 == FALSE )
                 {
                    BC1 = TRUE;
                    (void) memscpy(p_write_point, (last_octet_write_point - p_write_point + 1) ,p_IE, p_IE[1] + 2);
                    p_write_point = ((byte *)(p_write_point)) + p_IE[1] + 2;

                 }
                 else
                 {
                    if ( BC2 == FALSE )
                    {
                       BC2 = TRUE;
                       (void)memscpy(p_write_point, (last_octet_write_point - p_write_point + 1) ,p_IE, p_IE[1] + 2);
                       p_write_point = ((byte *)(p_write_point)) + p_IE[1] + 2;

                    }
                    else
                    {
                       syntax_ok = FALSE;
                    }
                 }
               }
               break;

#if defined(FEATURE_MULTIMEDIA_FALLBACK) || defined(FEATURE_REL6)
            case IEI_NETWORK_CC_CAPABILITY:

               {
                   (void) memscpy(p_write_point, (last_octet_write_point - p_write_point + 1) ,p_IE, IE_length);
                    p_write_point = ((byte *)(p_write_point)) + IE_length;

               }
               break;
#endif /* FEATURE_MULTIMEDIA_FALLBACK */

            default:                            /* this cannot happen! */
               MSG_ERROR_DS_1(MN_SUB,"=MNCNM= Unexpected IE in call proceeding msg: received IEI = %d\n",
                          (dword) IEI);

               syntax_ok = FALSE;
               break;
         }     /* end case */
      }
      

      IE_count++;       /* index next IE in the message */


   }   /* END WHILE more IEs and Syntax is OK */


   if ( ((BC2 == TRUE ) && (repeat_ind_present == FALSE)) ||
        ((BC2 == FALSE) && (repeat_ind_present == TRUE )) ||
        ((BC1 == FALSE) && (repeat_ind_present == TRUE )))
   {
      syntax_ok = FALSE;
      cause = CONDITIONAL_IE_ERROR;
   }

   if ( syntax_ok == TRUE )
   {
      /* send CALL PROCEEDING w/ possible repeat indicator to MN */

      len = (byte)( (byte *)p_write_point -
                                 (byte *)&mncc_call_proc_ind.as_id );

      PUT_IMH_LEN( len,
                   &mncc_call_proc_ind.message_header );

      CNM_send_message( (IMH_T *)&mncc_call_proc_ind,
                        GS_QUEUE_MN_CM );
   }
   else
   {
      MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Syntax error in Call Proceeding Message\n");
      CNM_send_status ( connection_id,
                      PD_CC,
                      cause );
   }

}
/**************** End of   CNM_process_call_proceeding_message ********************/


/**************** Begin of   CNM_process_connect_message **************************
 *
 *  Function name: CNM_process_connect_message()
 *  --------------------------------------------
 *
 *  Description:
 *  ------------
 *  This function handles a call proceeding message received from the network. It walks
 *  through the IEs in the message checking for validity, and if all ok, sends
 *  the request on to MN, otherwise a status message is sent.
 *  This function assumes that the message has already been checked for IE
 *  content
 *
 *
 *  Parameters:
 *  -----------
 *  connection_id : IN
 *
 *  Returns:
 *  --------
 *  none
 *
 ******************************************************************************/

VOID_FUNC   CNM_process_connect_message( connection_id_T connection_id, boolean syntax_ok, byte cause)
{
   byte                       *p_IE;
   byte                       *p_write_point;
   byte                       *last_octet_write_point;
   byte                       IEI;
   byte                       IE_count;
   byte                       IE_length;
   dword                      len;

   MNCC_SETUP_CNF_T           mncc_setup_cnf;
   memset(&mncc_setup_cnf, 0, sizeof(mncc_setup_cnf));

   p_IE = (byte *) 0;
   IE_count = 2;              /* start at first IE after message type */

   mncc_setup_cnf.message_header.message_set = MS_MN_CC;
   mncc_setup_cnf.message_header.message_id  = MNCC_SETUP_CNF;
   mncc_setup_cnf.connection_id = connection_id;

#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
   {
     mncc_setup_cnf.as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
   }
#endif

   p_write_point = (byte *)mncc_setup_cnf.data;
   last_octet_write_point = (byte *)&mncc_setup_cnf.data[sizeof(mncc_setup_cnf.data)-1];

   while ( (syntax_ok != FALSE) && ( IE_count < no_of_IEs ))
   {

      /*
       * get information of the next IE from message
       */
      IEI       = cc_IE_info[IE_count].IEI;
      p_IE      = cc_IE_info[IE_count].start;
      IE_length = cc_IE_info[IE_count].length;
      /* 
       * IE_length has been sanitized and truncated if necessary in function CNM_perform_check_on_IEs()
       */

      if(cc_IE_info[IE_count].syntax_ok == TRUE)
      {
        switch (IEI)
        {
          case IEI_FACILITY:
          case IEI_PROGRESS_INDICATOR:
#ifdef FEATURE_UUS
          case IEI_USER_USER:
#endif
             break;

          case IEI_CONNECTED_NUMBER:
          case IEI_CONNECTED_SUBADDRESS:
             {
                (void)memscpy( p_write_point, (last_octet_write_point - p_write_point +1) ,p_IE, IE_length );
                p_write_point = ((byte *)(p_write_point)) + IE_length;
             }
             break;


          default:                            /* this cannot happen! */
             MSG_ERROR_DS_1(MN_SUB,"=MNCNM= Unexpected IE in call connect msg: received IEI = %d\n",
                        (dword) IEI);

             syntax_ok = FALSE;
             break;

        }     /* end case */
      }

      IE_count++;       /* index next IE in the message */
   }   /* END WHILE more IEs and Syntax is OK */


   if(syntax_ok)
   {
      len = (dword)( (byte *)p_write_point -
                                 (byte *)&mncc_setup_cnf.as_id);

      PUT_IMH_LEN( len,
                   &mncc_setup_cnf.message_header);

      CNM_send_message( (IMH_T *)&mncc_setup_cnf,
                       GS_QUEUE_MN_CM );
   }
   else
   {
     CNM_send_status ( connection_id,
                      PD_CC,
                      cause );
     MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Syntax error in Connect Message\n");
   }
}

/**************** End of   CNM_process_connect_message ****************************/


/**************** Begin of   CNM_process_connect_ack_message ***********************
 *
 *  Function name: CNM_process_connect_ack_message()
 *  -----------------------------------------------
 *
 *  Description:
 *  ------------
 *  This function handles the connect ack message from the network.
 *  The response is to send an MNCC_SETUP_COMPL_IND message to MN
 *
 *
 *  Parameters:
 *  -----------
 *  connection_id : IN
 *
 *  Returns:
 *  --------
 *  none 
 *
 ******************************************************************************/

VOID_FUNC   CNM_process_connect_ack_message( connection_id_T connection_id ) 
{
   MNCC_SETUP_COMPL_IND_T     mncc_setup_compl_ind;

   mncc_setup_compl_ind.message_header.message_set = MS_MN_CC;
   mncc_setup_compl_ind.message_header.message_id  = MNCC_SETUP_COMPL_IND;
   mncc_setup_compl_ind.connection_id = connection_id;

#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
   {
     mncc_setup_compl_ind.as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
   }
#endif


   PUT_IMH_LEN( sizeof (MNCC_SETUP_COMPL_IND_T) - sizeof(IMH_T), 
                &mncc_setup_compl_ind.message_header);

   CNM_send_message((IMH_T *)&mncc_setup_compl_ind, 
                  GS_QUEUE_MN_CM );
}

/**************** End of   CNM_process_connect_ack_message ************************/


/**************** Begin of   CNM_process_progress_message *************************
 *
 *  Function name: CNM_process_progress_message()
 *  --------------------------------------------
 *
 *  Description:
 *  ------------
 *  This function handles the progress message from the network. It walks
 *  through the IEs in the message checking for validity, and if all ok, sends
 *  the request on to MN, otherwise a status message is sent.
 *  This function assumes that the message has already been checked for IE
 *  content
 *
 *
 *  Parameters:
 *  -----------
 *  connection_id : IN
 *
 *  Returns:
 *  --------
 *  none
 *
 ******************************************************************************/

VOID_FUNC   CNM_process_progress_message( connection_id_T connection_id, boolean syntax_ok, byte cause)
{

   byte                       *p_IE;
   ie_progress_indicator_T    *p_prog_ind;
   boolean                    progress_ind_valid;
   byte                       IEI;

   byte                       IE_count;
   MNCC_PROGRESS_IND_T        mncc_progress_ind;


   memset(&mncc_progress_ind, NULL, sizeof(mncc_progress_ind));

   p_IE = (byte *) 0;
   p_prog_ind = (ie_progress_indicator_T *)((void *)p_IE); /* Dummy initialization */
   IE_count = 2;              /* start at first IE after message type */
   progress_ind_valid = FALSE;

   while ( (syntax_ok != FALSE) && ( IE_count < no_of_IEs ))
   {

  /*
   * get information of the next IE from message
   */
      IEI       = cc_IE_info[IE_count].IEI;
      p_IE      = cc_IE_info[IE_count].start;

      if(cc_IE_info[IE_count].syntax_ok == TRUE)
      {
        switch (IEI)
        {

           case IEI_PROGRESS_INDICATOR:

              p_prog_ind = (ie_progress_indicator_T *)((void *)p_IE);
              progress_ind_valid = TRUE;
              break;

#ifdef FEATURE_UUS
           case IEI_USER_USER:
              break;
#endif

           default:                            /* this cannot happen! */
              MSG_ERROR_DS_1(MN_SUB,"=MNCNM= Unexpected IE in call proceeding msg: received IEI = %d\n",
                                    (dword) IEI);

              syntax_ok = FALSE;
              break;
        }     /* end case */
      }

      IE_count++;       /* index next IE in the message */

   }   /* END WHILE more IEs and Syntax is OK */


   if ( syntax_ok == TRUE && progress_ind_valid == TRUE )
   {
      CNM_setup_mncc_progress_ind( &mncc_progress_ind,
                                  connection_id,
                                  p_prog_ind );

      CNM_send_message((IMH_T *)&mncc_progress_ind,
                                    GS_QUEUE_MN_CM );

   }
   else
   {
      MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Syntax error in Progress Message\n");
      CNM_send_status ( connection_id,
                        PD_CC,
                        cause );
   }

}

/**************** End of   CNM_process_progress_message ***************************/

/**************** Begin of   CNM_in_modify_pending ****************************
 *
 *  Function name: CNM_in_modify_pending()
 *  -----------------------------------------
 *
 *  Description:
 *  ------------
 *  Check for any pending CC In-Call Modification pending transactions.
 *
 *
 *  Parameters:
 *  -----------
 *  None.
 *
 *  Returns:
 *  --------
 *  TRUE or FALSE
 *
 ******************************************************************************/
#if defined(FEATURE_MULTIMEDIA_FALLBACK) || defined(FEATURE_REL6)
boolean CNM_in_modify_pending()
{
    byte    connection_id;
    boolean modify_pending;
  
    for(connection_id=0, modify_pending=FALSE; connection_id < MAXNO_CC_TRANSACTIONS; connection_id++)
    {
        if( (connection_id != UNKNOWN_CONNECTION_ID) &&
            (cc_transaction_information_p[connection_id] != NULL) &&
            ((cc_transaction_information_p[connection_id]->cc_state == CC_STATE_U26) ||
             (cc_transaction_information_p[connection_id]->cc_state == CC_STATE_U27)) )
        {
            modify_pending = TRUE;
            break;
        }
    }

    return modify_pending;
}
#endif /* FEATURE_MULTIMEDIA_FALLBACK */

/**************** End of   CNM_in_modify_pending ***************************/

/**************** Begin of   CNM_process_setup_message ****************************
 *
 *  Function name: CNM_process_setup_message()
 *  -----------------------------------------
 *
 *  Description:
 *  ------------
 *  This function handles a setup message received from the network. It walks
 *  through the IEs in the message checking for validity, and if all ok, sends
 *  the request on to MN, otherwise a release_complete is sent.
 *  This function assumes that the message has already been checked for IE
 *  content
 *
 *
 *  Parameters:
 *  -----------
 *  message : IN
 *  connection_id : IN
 *
 *  Returns:
 *  --------
 *  none
 *
 ******************************************************************************/

VOID_FUNC   CNM_process_setup_message( connection_id_T connection_id, boolean syntax_ok, byte cause)
{

   byte                       *p_IE;
   boolean                    BC_repeat;
   boolean                    unknown_repeat_indicator_found;
   boolean                    BC1;
   boolean                    BC2;
   boolean                    LLC_1;
   boolean                    LLC_2;
   boolean                    LLC_repeat;
   boolean                    HLC_1;
   boolean                    HLC_2;
   boolean                    HLC_repeat;
   byte                       IEI;
   byte                       *p_write_point;
   byte                       *p_end_write_point;
   byte                       repeat_data;
   byte                       IE_count;
   byte                       IE_length;
   dword                      len;

   MNCC_SETUP_IND_T           mncc_setup_ind;
   MNCC_REL_IND_T             mncc_rel_ind;
   ie_bearer_capability_T     bc_capability[2]; /* BC 1+2 */

   memset(&bc_capability[0], NULL, (sizeof(ie_bearer_capability_T)*2));
   memset(&mncc_rel_ind, NULL, sizeof(mncc_rel_ind));
   memset(&mncc_setup_ind, NULL, sizeof(mncc_setup_ind));
   
   p_IE = (byte *) 0;
   IE_count = 2;              /* start at first IE after message type */

   BC_repeat = FALSE;
   LLC_repeat = FALSE;
   HLC_repeat = FALSE;
   unknown_repeat_indicator_found = FALSE;
   BC1 = FALSE;
   BC2 = FALSE;
   LLC_1 = FALSE;
   LLC_2 = FALSE;
   HLC_1 = FALSE;
   HLC_2 = FALSE;
   IEI = 0;
   mncc_setup_ind.message_header.message_set = MS_MN_CC;
   mncc_setup_ind.message_header.message_id  = MNCC_SETUP_IND;
   mncc_setup_ind.connection_id = connection_id;

   
#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
   {
     mncc_setup_ind.as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
   }
#endif


   /*
    * set up write pointer ready to append the IEs
    */

   p_write_point = mncc_setup_ind.data;
   p_end_write_point = p_write_point + sizeof(mncc_setup_ind.data) - 1;

   if ( no_of_IEs > MAXNO_IES_IN_MESSAGE )
   {
     ERR("no_of_IEs exceeds MAXNO_IES_IN_MESSAGE", 0, 0, 0);
     no_of_IEs = MAXNO_IES_IN_MESSAGE;
   }

   while (( syntax_ok != FALSE ) && ( IE_count < no_of_IEs ))
   {

      /*
       * get information of the next IE from global store
       */

      IEI       = cc_IE_info[IE_count].IEI;
      p_IE      = cc_IE_info[IE_count].start;
      IE_length = cc_IE_info[IE_count].length;
      /* 
       * IE_length has been sanitized and truncated if necessary in function CNM_perform_check_on_IEs()
       */

      if(cc_IE_info[IE_count].syntax_ok == TRUE)
      {
        switch (IEI)
        {
           case IEI_REPEAT_INDICATOR:

              /*
               * record that we have received a repeat indicator,
               *  but we don't yet know what its referring to
               */
              {
                unknown_repeat_indicator_found = TRUE;
                repeat_data = ((ie_repeat_T *)p_IE)->repeat_data;
              }                                /* store for later use */
              break;

           case IEI_FACILITY:
           case IEI_PROGRESS_INDICATOR:
#ifdef FEATURE_UUS
           case IEI_USER_USER:
#endif
              break;

           case IEI_BEARER_CAPABILITY:
              {
                if ( unknown_repeat_indicator_found != FALSE )
                {

                   /*
                    * it was a BC repeat
                    */

                   BC_repeat = TRUE;
                   unknown_repeat_indicator_found = FALSE;

                   (void) CNM_append_IE_to_message( &repeat_data,
                                          (byte **)&p_write_point,
                                          sizeof( byte ),
                                          p_end_write_point);
                }

                if ( BC1 == FALSE )
                {
                   BC1 = TRUE;

                   memscpy(&bc_capability[0], sizeof(bc_capability[0]) ,p_IE, p_IE[1] + 2);
                }
                else
                {
                   if ( BC2 == FALSE )
                   {
                      BC2 = TRUE;

                      memscpy(&bc_capability[1], sizeof(bc_capability[1]) ,p_IE, p_IE[1] + 2);
                   }
                   else
                   {
                      syntax_ok = FALSE;
                   }
                }
                (void) CNM_append_IE_to_message(p_IE,
                                         (byte **)&p_write_point,
                                         (byte)(p_IE[1] + 2),
                                         p_end_write_point);
              }
              break;

#if defined(FEATURE_MULTIMEDIA_FALLBACK) || defined(FEATURE_REL6)
           case IEI_NETWORK_CC_CAPABILITY:
              {
                (void) CNM_append_IE_to_message( p_IE,
                                         (byte **)&p_write_point,
                                         sizeof( ie_network_CC_CAP_T ),
                                         p_end_write_point);
              }
              break;

           case IEI_BACKUP_BEARER_CAPABILITY:
              {
                (void) CNM_append_IE_to_message( p_IE,
                                         (byte **)&p_write_point,
                                         p_IE[1] + 2,
                                         p_end_write_point);
              }
              break;
#endif /* FEATURE_MULTIMEDIA_FALLBACK */

           case IEI_SIGNAL:
              {
                (void) CNM_append_IE_to_message( p_IE,
                                         (byte **)&p_write_point,
                                         sizeof( ie_signal_T),
                                         p_end_write_point);
                                                       /* fixed length IE */
              }
              break;

           case IEI_CALLING_PARTY_BCD_NO:
           case IEI_CALLING_PARTY_SUBAD:
           case IEI_CALLED_PARTY_BCD_NO:
           case IEI_CALLED_PARTY_SUBAD:
           case IEI_REDIR_PARTY_BCD_NO:
           case IEI_REDIR_PARTY_SUBADDR:
              {
                (void) CNM_append_IE_to_message( p_IE,
                                         (byte **)&p_write_point,
                                         IE_length,
                                         p_end_write_point);
              }
              break;

           case IEI_LOW_LAYER_COMPATIB:
              {
                if ( unknown_repeat_indicator_found != FALSE )
                {

                   /*
                    * it was an LLC repeat
                    */

                   LLC_repeat = TRUE;
                   unknown_repeat_indicator_found = FALSE;
                   (void) CNM_append_IE_to_message( &repeat_data,
                                            (byte **)&p_write_point,
                                            sizeof( byte),
                                            p_end_write_point);
                }

                if ( LLC_1 == FALSE )
                {
                  LLC_1 = TRUE;
                }
                else
                {
                   if (  ( LLC_2 == FALSE ) &&
                         ( LLC_repeat != FALSE ) )
                   {
                      LLC_2 = TRUE;
                   }
                   else
                   {
                      syntax_ok = FALSE;  /* too many LLCs or no repeat ind */
                   }
                }

                (void) CNM_append_IE_to_message( p_IE,
                                         (byte **)&p_write_point,
                                         (byte)(p_IE[1] + 2),
                                         p_end_write_point);
              }
              break;

         case IEI_HIGH_LAYER_COMPATIB:
              {
                if ( unknown_repeat_indicator_found != FALSE )
                {

                /*
                 * it was a HLC repeat
                 */

                   HLC_repeat = TRUE;
                   unknown_repeat_indicator_found = FALSE;
                }

                if ( HLC_1 == FALSE )
                {
                   HLC_1 = TRUE;
                }
                else
                {
                   if (  ( HLC_2 == FALSE ) &&
                         ( HLC_repeat == TRUE ) )
                   {
                      HLC_2 = TRUE;
                   }
                   else
                   {
                      syntax_ok = FALSE;  /* too many HLCs or no repeat ind */
                   }
                }
                (void) CNM_append_IE_to_message( p_IE,
                                         (byte **)&p_write_point,
                                         IE_length,
                                         p_end_write_point);
              }
              break;

           case IEI_CAUSE_OF_NO_CLI:
              {
                (void) CNM_append_IE_to_message( p_IE,
                                         (byte **)&p_write_point,
                                         sizeof( ie_cause_of_no_CLI_T ),
                                         p_end_write_point);
              }

              break;

           case IEI_ALERTING_PATTERN:
              {
                (void) CNM_append_IE_to_message( p_IE,
                                         (byte **)&p_write_point,
                                         sizeof( ie_alerting_pattern_T ),
                                         p_end_write_point);
              }

              break;

           default:                            /* this cannot happen! */
              MSG_ERROR_DS_1(MN_SUB,"=MNCNM= Unexpected IE in setup msg: received IEI = %d\n",
                                   (dword) IEI);

              syntax_ok = FALSE;
              break;
        }
      }

      IE_count++;       /* index next IE in the message */

   }   /* END WHILE more IEs and Syntax is OK */

   if ( syntax_ok )
   {

      if ( ( ( BC_repeat != FALSE ) && ( BC2 == FALSE ) ) ||
           ( ( BC_repeat == FALSE ) && ( BC2 != FALSE ) ) )
      {
         MSG_ERROR_DS_3(MN_SUB,"=MNCNM= BC-bad combination of BC info: %d, %d, %d\n", connection_id, BC_repeat, BC2);
         syntax_ok = FALSE;
         cause = CONDITIONAL_IE_ERROR;

      }
      else if ( ( ( LLC_repeat != FALSE ) && ( LLC_2 == FALSE ) ) ||
                ( ( LLC_repeat == FALSE ) && ( LLC_2 != FALSE ) ) )
      {
         MSG_ERROR_DS_3(MN_SUB,"=MNCNM= BC-bad combination of LLC info : %d, %d, %d\n", connection_id, LLC_repeat, LLC_2);
         syntax_ok = FALSE;
         cause = CONDITIONAL_IE_ERROR;

      }
      else if ( ( ( HLC_repeat != FALSE ) && ( HLC_2 == FALSE ) ) ||
                ( ( HLC_repeat == FALSE ) && ( HLC_2 != FALSE ) ) )
      {
         MSG_ERROR_DS_3(MN_SUB,"=MNCNM= BC-bad combination of HLC info : %d, %d, %d\n", connection_id, HLC_repeat, HLC_2);
         syntax_ok = FALSE;
         cause = CONDITIONAL_IE_ERROR;
      }
      else
      {
         unknown_repeat_indicator_found = FALSE;  /* valid repeat indicator */
      } 
   }

#if defined(FEATURE_MULTIMEDIA_FALLBACK) || defined(FEATURE_REL6)
   if( (BC_repeat == TRUE) && (BC1 ==TRUE) && (BC2 ==TRUE) &&
       (memcmp(&bc_capability[0], &bc_capability[1], 
               (size_t)(MIN(bc_capability[0].Length, bc_capability[1].Length)+2)) == 0) )
   {  
       CNM_send_status(connection_id, PD_CC, CONDITIONAL_IE_ERROR); /* TS 24.008, 5.5.3.2.2 */
   }
   else if(CNM_in_modify_pending())
   {
       syntax_ok = FALSE;
   }
#endif /* FEATURE_MULTIMEDIA_FALLBACK */

   if ( syntax_ok )
   {
      len = (dword)( (byte *)p_write_point -
                                 (byte *)&mncc_setup_ind.as_id);

      PUT_IMH_LEN( len,
                   &mncc_setup_ind.message_header);

      CNM_send_message( (IMH_T *)&mncc_setup_ind,
                        GS_QUEUE_MN_CM );
   }
   else
   {
      MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Syntax error in Setup Message\n");

      if (cause != CONDITIONAL_IE_ERROR)
      {
#if defined(FEATURE_MULTIMEDIA_FALLBACK) || defined(FEATURE_REL6)
          if(CNM_in_modify_pending())
          {
              cause = USER_BUSY;
          }
          else
#endif /* FEATURE_MULTIMEDIA_FALLBACK */
          {
              cause = INVALID_MANDATORY_INFORMATION;
          }
      }
      
      for (IE_count = 2; IE_count < no_of_IEs ;IE_count ++)
      {
        /* check for the foremost IE for which syntax is FALSE */
        if(cc_IE_info[IE_count].syntax_ok == FALSE) 
        {
          /* check to see whether the error is due to SCUDIF Repeat Indicator */
          if((cc_IE_info[IE_count].IEI == IEI_REPEAT_INDICATOR) && (((*cc_IE_info[IE_count].start & 0x0f)==0x04) || ((*cc_IE_info[IE_count].start & 0x0f)==0x02)))
          {
            CNM_set_state_to_U0(connection_id);
            CNM_send_status(connection_id, PD_CC, cause);

            /* To clear call objects */
            CNM_send_mn_rel_cnf(connection_id, cause,0,NULL);     

            /* check for last connection */
            (void)CNM_check_for_last_connection(CNM_NORMAL_RELEASE
#if defined(FEATURE_DUAL_SIM )
                                        , mn_as_id
#endif
                                          );		
          }
          else
          {
            CNM_send_release_complete ( connection_id,
                                   PD_CC,
                                   cause,
                                   (ie_facility_T *) 0);
 
 
 #if defined(FEATURE_MULTIMEDIA_FALLBACK) || defined(FEATURE_REL6)
           if(connection_id > UNKNOWN_CONNECTION_ID)
           {        
              CNM_set_state_to_U0(connection_id);  /* MT Call */
           }
           else
 #endif
           {
           /*
            * set this connection call state back to CC_STATE_U0
            */
 
               CNM_set_state_to_U0(connection_id);
 
               mncc_rel_ind.message_header.message_set = MS_MN_CC;
               mncc_rel_ind.message_header.message_id  = MNCC_REL_IND;
               mncc_rel_ind.connection_id = connection_id;
 #if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
               {
                  mncc_rel_ind.as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
               }
 #endif
 
 
               mncc_rel_ind.cause = 0;  /* NULL local cause */
 
               p_write_point = &mncc_rel_ind.data[0];
 
               len = sizeof(MNCC_REL_IND_T) - sizeof(IMH_T); 
 
               PUT_IMH_LEN( len, &mncc_rel_ind.message_header);
 
               CNM_send_message((IMH_T *)&mncc_rel_ind, GS_QUEUE_MN_CM ); 		    
            }
          }
          break; 
        }
      }  
   }
}
/**************** End of   CNM_process_setup_message ******************************/


/**************** Begin of   CNM_process_disconnect_message ***********************
 *
 *  Function name: CNM_process_disconnect_message()
 *  --------------------------------------------
 *
 *  Description:
 *  ------------
 *  This function handles a disconnect message received from the network.
 *  It walks through the IEs in the message checking for validity, and if
 *  all ok, sends the request on to MN, otherwise a status message is sent.
 *  This function assumes that the message has already been checked for IE
 *  content
 *
 *
 *  Parameters:
 *  -----------
 *  message : IN
 *  connection_id : IN
 *
 *  Returns:
 *  --------
 *  none
 *
 ******************************************************************************/

VOID_FUNC   CNM_process_disconnect_message( connection_id_T connection_id, boolean syntax_ok, byte cause)
{

   byte                       *p_IE;
   byte                       *p_write_point;
   byte                       *last_octet_write_point;
   byte                       IEI;
   byte                       IE_count;
   byte                       IE_length;
   dword                      len;
   byte                       no_of_bytes;
   byte                       ProgressDescription = 0; 
   MNCC_DISC_IND_T            *p_mncc_disc_ind;
   release_msg_T              *p_release;

   p_mncc_disc_ind = (MNCC_DISC_IND_T*) CNM_alloc( sizeof(MNCC_DISC_IND_T) );
   p_release = (release_msg_T*) CNM_alloc( sizeof(release_msg_T) );


   if((p_mncc_disc_ind != NULL) &&
      (p_release != NULL))
   {
       memset(p_mncc_disc_ind , NULL, sizeof(MNCC_DISC_IND_T));
       memset(p_release , NULL, sizeof(release_msg_T));
 
       p_IE = (byte *) 0;
       IE_count = 2;              /* start at first IE after message type */
       p_mncc_disc_ind->message_header.message_set = MS_MN_CC;
       p_mncc_disc_ind->message_header.message_id  = MNCC_DISC_IND;
       p_mncc_disc_ind->connection_id = connection_id;

#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
       {
          p_mncc_disc_ind->as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
       }
#endif


       p_write_point = (byte *)p_mncc_disc_ind->data;
       last_octet_write_point = &p_mncc_disc_ind->data[sizeof(p_mncc_disc_ind->data)-1];


       while ( (syntax_ok != FALSE) && ( IE_count < no_of_IEs ))
       {

      /*
       * get information of the next IE from message
       */
          IEI       = cc_IE_info[IE_count].IEI;
          p_IE      = cc_IE_info[IE_count].start;
          IE_length = cc_IE_info[IE_count].length;
          /* 
           * IE_length has been sanitized and truncated if necessary in function CNM_perform_check_on_IEs()
           */
             if(cc_IE_info[IE_count].syntax_ok == TRUE)
             {    
               switch (IEI)
               {
                  case IEI_CAUSE:
                     {
                       (void)memscpy( p_write_point, (last_octet_write_point - p_write_point + 1) ,p_IE, IE_length );
                       /*
                        * add missing cause IEI to mncc message
                        */
                       *p_write_point = IEI_CAUSE;
                       p_write_point = ((byte *)(p_write_point)) + IE_length;
                     }
                     break;
    
                  case IEI_FACILITY:
#ifdef FEATURE_UUS    
                  case IEI_USER_USER:
#endif
                     break;    
    
                  case IEI_PROGRESS_INDICATOR:
                     {
                       (void)memscpy(p_write_point, (last_octet_write_point - p_write_point + 1) ,p_IE, IE_length );
                       p_write_point = ((byte *)(p_write_point)) + IE_length;

                       ProgressDescription = 
                         ((ie_progress_indicator_T *)((void *)p_IE))->ProgressDescription & 0x7f;
                     }
                     break;

#ifdef FEATURE_CCBS
                  case IEI_ALLOWED_ACTIONS:
                     {
                       (void)memscpy( p_write_point, (last_octet_write_point - p_write_point + 1) ,p_IE, IE_length );
                       /*
                        * add missing cause IEI to mncc message
                        */
                       *p_write_point = IEI_ALLOWED_ACTIONS;
                       p_write_point = ((byte*)(p_write_point)) + IE_length;
                     }
                     break;
#endif /* FEATURE_CCBS */

                  default:                            /* this cannot happen! */
                     MSG_ERROR_DS_1(MN_SUB,"=MNCNM= Unexpected IE in disconnect msg: received IEI = %d\n",
                     (dword) IEI);
    
                     syntax_ok = FALSE;
                     break;
               }     /* end case */
             }
    
          IE_count++;       /* index next IE in the message */
    
    
       }   /* END WHILE more IEs and Syntax is OK */
    
    
       if( syntax_ok )
       {
    
          len = (dword)( (byte *)p_write_point -
                                     (byte *)&p_mncc_disc_ind->as_id);
          
          PUT_IMH_LEN( len,
                       &p_mncc_disc_ind->message_header);

          if(ProgressDescription != 8 || 
              tch_user_connections_store[DEFAULT_RAB_ID].OTA_channel_connected != TRUE)
          {
            CNM_clear_dtmf_queue(connection_id);    
          }
          CNM_send_message( (IMH_T *)p_mncc_disc_ind,
                           GS_QUEUE_MN_CM );
       }
       else
       {
          /*
           * Set up a pointer to the end of the message so fare and
           * add a cause IE
           */
    
          p_write_point = &p_release->data[0];
    
          no_of_bytes = CNM_write_cause_ie( p_write_point, cause );

          MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Syntax error in Disconnect Message\n");

          /*
           * Store cm_cc_cause in mn_call_information_p
           * This will be used when NW sends RELEASE_COMPLETE message
           */
          if(mn_call_information_p[connection_id] != NULL)
          {
            mn_call_information_p[connection_id]->mo_release_cause_diag_packet.present = TRUE;
            mn_call_information_p[connection_id]->mo_release_cause_diag_packet.cause_value = (p_write_point[no_of_bytes - 1] & 0x7f);
          }
    
          p_release->msg_head.type = RELEASE;
    
    
          p_write_point = ((byte *)(p_write_point)) + no_of_bytes;
    
          len = (dword)( (byte *)p_write_point -
                               (byte *)&p_release->msg_head.ti_pd);
    
          PUT_IMH_LEN( len,
                       &p_release->msg_head.IMH);
    
          CNM_send_primitive_to_mm ( connection_id, PD_CC, 
                                     &p_release->msg_head.IMH);
       }


       CNM_free(p_mncc_disc_ind);
       CNM_free(p_release);


       /* Enabling DXT blindly to consider the case where call is
          disconected by NW before it is connected and another call is active*/
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
    else
    {
       MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Can't allocate buffer\n");
    }
}

/**************** End of   CNM_process_disconnect_message *************************/


/**************** Begin of   CNM_process_release_message ***************************
 *
 *  Function name: CNM_process_release_message()
 *  -------------------------------------------
 *
 *  Description:
 *  ------------
 *  This function handles a release message received from the network. It walks
 *  through the IEs in the message checking for validity, and if all ok, sends
 *  the request on to MN, otherwise a status message is sent.
 *  This function assumes that the message has already been checked for IE
 *  content
 *
 *
 *  Parameters:
 *  -----------
 *  message : IN
 *  connection_id : IN
 *
 *  Returns:
 *  --------
 *  none
 *
 ******************************************************************************/

VOID_FUNC   CNM_process_release_message( connection_id_T connection_id, boolean syntax_ok, byte cause)
{

   byte                       *p_IE;
   byte                       *p_write_point;
   byte                       *last_octet_write_point;

   byte                       IEI;

   byte                       IE_count;
   byte                       IE_length;

   byte                       first_cause[GSM_CAUSE_SIZE];

   MNCC_REL_IND_T             *p_mncc_rel_ind;


   /*
   ** Clear global flag, see l2/m60600.c for description.
   */


   p_mncc_rel_ind = (MNCC_REL_IND_T*) CNM_alloc( sizeof( MNCC_REL_IND_T ));

   if(p_mncc_rel_ind != NULL)
   {
       memset(p_mncc_rel_ind, NULL, sizeof(MNCC_REL_IND_T));

       p_IE = (byte *) 0;
       IE_count = 2;              /* start at first IE after message type */
    
       first_cause[0] = 0;
    
       p_mncc_rel_ind->message_header.message_set = MS_MN_CC;
       p_mncc_rel_ind->message_header.message_id  = MNCC_REL_IND;
       p_mncc_rel_ind->connection_id = connection_id;

#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
       {
         p_mncc_rel_ind->as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
       }
#endif

    
       p_mncc_rel_ind->cause = 0;
    
       p_write_point = (byte *)p_mncc_rel_ind->data;
       last_octet_write_point = &p_mncc_rel_ind->data[sizeof(p_mncc_rel_ind->data)-1];
    
       while ( (syntax_ok != FALSE) && ( IE_count < no_of_IEs ))
       {
    
         /*
          * get information of the next IE from message
          */
    
         IEI       = cc_IE_info[IE_count].IEI;
         p_IE      = cc_IE_info[IE_count].start;
         IE_length = cc_IE_info[IE_count].length;
         /* 
          * IE_length has been sanitized and truncated if necessary in function CNM_perform_check_on_IEs()
          */
         if(cc_IE_info[IE_count].syntax_ok == TRUE)
         {
           switch (IEI)
           {
              case IEI_FACILITY:
#ifdef FEATURE_UUS    
              case IEI_USER_USER:
#endif
                 break;

              case IEI_CAUSE:
                 {
                   (void)memscpy( p_write_point, (last_octet_write_point - p_write_point + 1) ,p_IE, IE_length );
    
                   p_write_point = ((byte *)(p_write_point)) + IE_length;
   
                   if ( first_cause[0] == 0 )
                   {
                      (void)memscpy( first_cause, sizeof(first_cause) ,p_IE, IE_length );
                   }
                 }
                 break;
    
              default:                            /* this cannot happen! */
                 MSG_ERROR_DS_1(MN_SUB,"=MNCNM= Unexpected IE in release msg: received IEI = %d\n",
                                      (dword) IEI);
    
                 syntax_ok = FALSE;
                 break;
           }     /* end case */
         }

        
          IE_count++;       /* index next IE in the message */
    
       }   /* END WHILE more IEs and Syntax is OK */

       if ( syntax_ok != FALSE )
       {
           /* 
           * check to see if a MMCNM_REL_REQ has already been sent (race condition)
           *.In that case it is not necessary to inform MN of the release
           * because it has already been done
           */
    
           {
               PUT_IMH_LEN( (int)( (byte *)p_write_point -
                                   (byte *)&p_mncc_rel_ind->as_id),
                                    &p_mncc_rel_ind->message_header);
    
               CNM_send_message( (IMH_T *)p_mncc_rel_ind,
                                GS_QUEUE_MN_CM );
           }
       }
       else
       {
         CNM_send_release_complete ( connection_id,
                                    PD_CC,
                                    cause,
                                    (ie_facility_T *) 0);
       }

       CNM_free( p_mncc_rel_ind );
    }
    else
    {
       MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Can't alloc buffer");
    }
}

/**************** End of   CNM_process_release_message ****************************/


/**************** Begin of   CNM_process_release_complete_message ******************
 *
 *  Function name: CNM_process_release_complete_message()
 *  ----------------------------------------------------
 *
 *  Description:
 *  ------------
 *  This function handles a release complete message received from the
 *  network. It walks through the IEs in the message checking for validity,
 *  and if all ok, sends the request on to MN, otherwise a status message
 *  is sent.
 *  This function assumes that the message has already been checked for IE
 *  content
 *
 *
 *  Parameters:
 *  -----------
 *  message : IN
 *  connection_id : IN
 *
 *  Returns:
 *  --------
 *  none
 *
 ******************************************************************************/

VOID_FUNC   CNM_process_release_complete_message( connection_id_T connection_id, boolean syntax_ok, byte error_cause)
{
   byte                       *p_IE;

   byte                       IEI;

   byte                       IE_count;
   byte                       IE_length;

   dword                      len;

   byte                       no_of_bytes;
   byte                       cause[GSM_CAUSE_SIZE];
   byte                       *last_octet_cause = cause + sizeof(cause) - 1;
   byte                       release_cause[GSM_CAUSE_SIZE];
   MNCC_REJ_IND_T             mncc_rej_ind;

   memset(&mncc_rej_ind, NULL, sizeof(mncc_rej_ind));

   p_IE = (byte *) 0;
   IE_count = 2;              /* start at first IE after message type */

   cause[1] = 0;

   while ( (syntax_ok != FALSE) && ( IE_count < no_of_IEs ))
   {

      /*
       * get information of the next IE from message
       */

      IEI       = cc_IE_info[IE_count].IEI;
      p_IE      = cc_IE_info[IE_count].start;
      IE_length = cc_IE_info[IE_count].length;
      /* 
       * IE_length has been sanitized and truncated if necessary in function CNM_perform_check_on_IEs()
       */
      if(cc_IE_info[IE_count].syntax_ok == TRUE)
      {
        switch (IEI)
        {
           case IEI_FACILITY:
#ifdef FEATURE_UUS
           case IEI_USER_USER:
#endif
              break;

           case IEI_CAUSE:
              {
                (void)memscpy(cause, sizeof(cause) ,p_IE, IE_length);
              }
              break;

           default:                            /* this cannot happen! */
              MSG_ERROR_DS_1(MN_SUB,"=MNCNM= Unexpected IE in release complete msg: received IEI = %d\n",
                                                                  (dword) IEI);

              syntax_ok = FALSE;
              break;
        }     /* end case */
      }


      IE_count++;       /* index next IE in the message */


   }   /* END WHILE more IEs and Syntax is OK */


   if ( syntax_ok )
   {

      if ( last_state == CC_STATE_U1 )
      {
         mncc_rej_ind.message_header.message_set = MS_MN_CC;
         mncc_rej_ind.message_header.message_id  = MNCC_REJ_IND;
         mncc_rej_ind.connection_id = connection_id;

#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
         {
           mncc_rej_ind.as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
         }
#endif


         mncc_rej_ind.cause = 0;    /* NULL local cause value */

         if (cause[1] != 0)
         {
            IE_length = (unsigned char) (cause[1] + 2);
            (void) memscpy( &mncc_rej_ind.data[0], sizeof(mncc_rej_ind.data) , cause, IE_length );
         }
         else
         {
            IE_length = 0;
         }

         len = sizeof(MNCC_REJ_IND_T) -  sizeof(IMH_T);

         PUT_IMH_LEN( len,
                      &mncc_rej_ind.message_header);

         CNM_send_message( (IMH_T *)&mncc_rej_ind,
                       GS_QUEUE_MN_CM );
         /*
          * Fill the cause value to be sent in 0x713D log event
          */
         if(mn_call_information_p[connection_id] != NULL)
         {
           (void)MN_get_cause_data(cause, &(mn_call_information_p[connection_id]->call_reject_cause_diag_packet), last_octet_cause);
         }
      }
      else
      {
         if (cause[1] != 0)
         {
            IE_length = (unsigned char) (cause[1] + 2);
         }
         else
         {
            IE_length = 0;
         }

#ifdef FEATURE_CCBS
         if (cause[1] != 0) /* we have some cause */
         {
            CNM_send_mn_rel_cnf(connection_id, (lower_layer_cause_T)(cause[3]&0x7f), IE_length, cause);
         }
         else
         {
            CNM_send_mn_rel_cnf(connection_id, 0, IE_length, cause);
         }
#else
         CNM_send_mn_rel_cnf(connection_id, 0, IE_length, cause);
#endif /* FEATURE_CCBS */
      }

      if (cc_transaction_information_p[connection_id] != NULL 
          && cc_transaction_information_p[connection_id]->cc_state != CC_STATE_U0)
      {
         CNM_set_state_to_U0( connection_id );
      }
      (void)CNM_check_for_last_connection(CNM_NORMAL_RELEASE
#if defined(FEATURE_DUAL_SIM )
                                        , mn_as_id
#endif
                                         );
   }
   else
   {
     MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Syntax error in Release Complete Message\n");

     CNM_send_release_complete ( connection_id,
                                PD_CC,
                                error_cause,
                                (ie_facility_T *) 0);
     no_of_bytes = CNM_write_cause_ie( release_cause,
                                                              error_cause );

     CNM_send_mn_rel_cnf(connection_id, 0, no_of_bytes, release_cause);

   }
}

/**************** End of   CNM_process_release_complete_message ********************/
