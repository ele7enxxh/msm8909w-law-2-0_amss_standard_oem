/*===========================================================================
                       COPYRIGHT INFORMATION

Copyright (c) 2001 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

                       EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/cnm/vcs/cnm_cc_mn_primitives.c_v   1.5   01 May 2002 13:22:08   cdealy  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/cnm/src/cnm_cc_mn_primitives.c#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/06/01   CD      Replaced mn header files by mn.h

08/14/01   AB      Renamed connection manager references to CNM from CM.

11-Mar-2002   VT   Removed Lint error.

04/19/02   CD      Removed lint errors
                   Added function CNM_send_mn_rel_cnf()

4/30/02    CD      In CNM_send_mn_rel_cnf(), added a check for the length 
                   parameter before executing a memcpy operation
                   
04/16/03   AB      In CNM_setup_MNCC_REJ_IND() added interface arguments to
                   supports the new establish reject type and reject value 
                   that are received from MM.

11/25/11   PM      Sending non-zero value for lower_layer_cause field when 
                   CNM send MNCC_REL_CNF to MN.
                  
===========================================================================*/


#include "mmcp_variation.h"
#include "customer.h"
#include "rex.h"
#include "err.h"
#include "environ.h"
#include "sys_cnst_v.h"
#include "sys_type.h"

#include "mn.h"
#include "cc_cc.h"
#include "ms.h"
#include "ms_cc_mm_v.h"
#include "timers_v.h"
#include "gs_v.h"
#include "ms_timer_v.h"
#include "cmlib.h"
#include "cnm.h"
#include "mnglobal.h"
#include "nasutils.h"
#include <stringl/stringl.h>


/**************** Begin of   CNM_setup_MNCC_REJ_IND *******************************
 *
 *  Function name: CNM_setup_MNCC_REJ_IND()
 *  --------------------------------------
 *
 *  Description:
 *  ------------
 *  Function to set up the MNXX_REJ_IND message. It uses the service type
 *  information to decide on the actual message Id from CC, SMS, or SS.
 *
 *  Parameters:
 *  -----------
 *  mncc_rej_ind : OUT   - pointer to area where message is formed
 *  connection_id : IN   - connection id associated with rejection
 *  cause : IN           - cause information element
 *
 *  Returns:
 *  --------
 *  send_message : boolean - TRUE if message is to be sent
 *
 ******************************************************************************/

boolean CNM_setup_MNCC_REJ_IND(MNCC_REJ_IND_T       *p_mncc_rej_ind,
                               connection_id_T      connection_id,
                               reject_type_enum_T   rej_type,
                               reject_cause_T       rej_cause,
                               byte                 *p_cause)
{

   boolean ret_code = TRUE;     /* default to OK message */

   p_mncc_rej_ind->message_header.message_set = MS_MN_CC;
   p_mncc_rej_ind->message_header.message_id = MNCC_REJ_IND;

   p_mncc_rej_ind->connection_id = connection_id;

#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
   {
      p_mncc_rej_ind->as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
   }
#endif


   p_mncc_rej_ind->cause = rej_cause;
   
   if (rej_type != INVALID_REJECT_CAUSE)   /* reject cause value */
   {
      p_mncc_rej_ind->rej_cause.present = TRUE;
      p_mncc_rej_ind->rej_cause.rej_type = rej_type;
      p_mncc_rej_ind->rej_cause.rej_value = rej_cause;

      PUT_IMH_LEN( sizeof(MNCC_REJ_IND_T) - sizeof(IMH_T),
                   &p_mncc_rej_ind->message_header );
   }
   else
   {
      p_mncc_rej_ind->rej_cause.present = FALSE;
      p_mncc_rej_ind->rej_cause.rej_type = INVALID_REJECT_CAUSE;
   }

   if(p_cause != NULL) 
   {
     /* guarding the memcpy with the length check. Defense in Depth against Buffer Overflow. Truncating if 
            there is excess data than can be accomodated by the destination buffer size . */
     if( (p_cause[1] + 2) <= GSM_CAUSE_SIZE)
     {
       (void)memscpy((VOID_DATA *) p_mncc_rej_ind->data,
                    sizeof(p_mncc_rej_ind->data),
                    (const VOID_DATA *)p_cause,
                     p_cause[1] + 2 );     /* Length field does not include */
                                           /* Length indicator or IEI       */
     }
     else
     {
       (void)memscpy((VOID_DATA *) p_mncc_rej_ind->data,
                     sizeof(p_mncc_rej_ind->data),
                    (const VOID_DATA *)p_cause,
                     GSM_CAUSE_SIZE );   /* Length field does not include */
                                         /* Length indicator or IEI*/
        p_mncc_rej_ind->data[1] = GSM_CAUSE_SIZE -2;
     }
      PUT_IMH_LEN( sizeof(MNCC_REJ_IND_T) - sizeof(IMH_T),
                   &p_mncc_rej_ind->message_header );
   }
   else
   {  
      /* zero out the cause buffer */
      memset((void *)p_mncc_rej_ind->data, NULL, sizeof(GSM_CAUSE_SIZE));
   }

   if((p_cause == NULL) && 
       (rej_type == INVALID_REJECT_CAUSE))
   {
       ret_code = FALSE;
   }

   /*
    *  Store the cause to be logged in 0x713D event to DIAG
    */
    if((ret_code == TRUE) && (mn_call_information_p[connection_id] != NULL))
    {
      mn_call_information_p[connection_id]->call_reject_cause_diag_packet.present = TRUE;
      mn_call_information_p[connection_id]->call_reject_cause_diag_packet.cause_value = CALL_CLEARING_CAUSE_REJECT;
      /*
       *  Cause reported to DIAG is 0.
       * Cases covered are MMCNM_EST_REJ, MMCNM_REL_IND, MMCC_REEST_REJ
       */
    }

   return ret_code;
}

/**************** End of   CNM_setup_MNCC_REJ_IND ******************************/

#ifdef FEATURE_UUS
/**************** Begin of   CNM_setup_mncc_user_user_data_ind ********************
 *
 *  Function name: CNM_setup_mncc_user_user_data_ind()
 *  -------------------------------------------------
 *
 *  Description:
 *  ------------
 *  Function to set up the MNCC_USER_USER_DATA_IND message.
 *
 *  Parameters:
 *  -----------
 *  mncc_user_user_data_ind : OUT   - pointer to area where message is formed
 *  connection_id : IN   - connection id associated with this transaction
 *  user_user_data : IN           - pointer to user_user information element
 *
 *  Returns:
 *  --------
 *  none
 *
 ******************************************************************************/
VOID_FUNC CNM_setup_mncc_user_user_data_ind( MNCC_USER_USER_DATA_IND_T *
                                                p_mncc_user_user_data_ind,
                                            connection_id_T  connection_id,
                                            ie_user_user_T   *p_ie_user_user )
{

   /*
    * set up header portion
    */

   p_mncc_user_user_data_ind->message_header.message_set = MS_MN_CC;
   p_mncc_user_user_data_ind->message_header.message_id  = MNCC_USER_USER_DATA_IND;
   p_mncc_user_user_data_ind->connection_id = connection_id;

#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
   {
      p_mncc_user_user_data_ind->as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
   }
#endif


   /*
    * copy in user_user_data information element
    */

   (void)memscpy(&p_mncc_user_user_data_ind->data[0],
                sizeof(p_mncc_user_user_data_ind->data),
                p_ie_user_user,
                p_ie_user_user->Length  + 2);

   /*
    * make sure IEI is set up
    */

   p_mncc_user_user_data_ind->data[0] = IEI_USER_USER;

   /*
    * fill in message length field
    */

   PUT_IMH_LEN( sizeof(MNCC_USER_USER_DATA_IND_T) - sizeof(IMH_T),
                &p_mncc_user_user_data_ind->message_header );

}
/**************** Begin of   CNM_setup_mncc_user_user_data_ind ********************/

/**************** Begin of   CNM_setup_mncc_congestion_ind ********************
 *
 *  Function name: CNM_setup_mncc_user_user_data_ind()
 *  -------------------------------------------------
 *
 *  Description:
 *  ------------
 *  Function to set up the MNCC_CONGESTION_IND message.
 *
 *  Parameters:
 *  -----------
 *  mncc_congestion_ind : OUT   - pointer to area where message is formed
 *  connection_id : IN   - connection id associated with this transaction
 *  congestion_data : IN           - pointer to user_user information element
 *
 *  Returns:
 *  --------
 *  none
 *
 ******************************************************************************/
VOID_FUNC CNM_setup_mncc_congestion_ind( MNCC_CONGESTION_IND_T *
                                            p_mncc_congestion_ind,
                                            connection_id_T  connection_id,
                                            ie_congestion_control_T   *p_ie_congestion )
{

   /*
    * set up header portion
    */

   p_mncc_congestion_ind->message_header.message_set = MS_MN_CC;
   p_mncc_congestion_ind->message_header.message_id  = MNCC_CONGESTION_IND;
   p_mncc_congestion_ind->connection_id = connection_id;

#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
   {
      p_mncc_congestion_ind->as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
   }
#endif


   /*
    * Get congestion level
    */

   p_mncc_congestion_ind->data[0] = p_ie_congestion->IeiLevel & 
                                              CONGESTION_RECEIVER_NOT_READY;

   /*
    * fill in message length field
    */

   PUT_IMH_LEN( sizeof(MNCC_CONGESTION_IND_T) - sizeof(IMH_T),
                &p_mncc_congestion_ind->message_header );

}
/**************** End of   CNM_setup_mncc_congestion_ind ********************/
#endif


/**************** Begin of   CNM_setup_mncc_progress_ind **************************
 *
 *  Function name: CNM_setup_mncc_progress_ind()
 *  -------------------------------------------
 *
 *  Description:
 *  ------------
 *  Function to set up the MNCC_PROGRESS_IND message.
 *
 *  Parameters:
 *  -----------
 *  mncc_progress_ind : OUT   - pointer to area where message is formed
 *  connection_id : IN   - connection id associated with this transaction
 *  progress_data : IN           - pointer to progress information element
 *
 *  Returns:
 *  --------
 *  none
 *
 ******************************************************************************/
VOID_FUNC CNM_setup_mncc_progress_ind(
                     MNCC_PROGRESS_IND_T     *p_mncc_progress_ind,
                     connection_id_T         connection_id,
                     ie_progress_indicator_T *p_ie_progress )
{


   /*
    * set up header portion
    */

   p_mncc_progress_ind->message_header.message_set = MS_MN_CC;
   p_mncc_progress_ind->message_header.message_id  = MNCC_PROGRESS_IND;
   p_mncc_progress_ind->connection_id = connection_id;

#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
  {
     p_mncc_progress_ind->as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
  }
#endif


   /*
    * copy in progress information element
    */
  /**************************************************************************** 
     *From section 10.5.4.21  of 24.008,the MAX length of the progress indicator contents is 2, 
     *as the MAX length of the whole IE is 4 octets. From section 11.4.2 of 24.007 it is clear that network 
     *can send more than value 2 in "length of the progress indicator contents". If NW sends a value >2 in 
     *the length of the progress indicator contents, then MS picks only the first 2 octets and ignores the rest.
     ****************************************************************************/     
   if(p_ie_progress->Length + 2 <= sizeof(ie_progress_indicator_T))
   {
      (void)memscpy(&p_mncc_progress_ind->progress_indicator,
                sizeof(p_mncc_progress_ind->progress_indicator),
                p_ie_progress,
                p_ie_progress->Length + 2 );
   }
   else
   {
      ERR("truncating as length %d exceeded buffer of size %d octets ",p_ie_progress->Length + 2,sizeof(ie_progress_indicator_T),0);
      (void)memscpy(&p_mncc_progress_ind->progress_indicator,
                sizeof(p_mncc_progress_ind->progress_indicator),
                p_ie_progress,
                sizeof(ie_progress_indicator_T) );
      /* copying the correct value to the structure which we send to the higher layers */
      p_mncc_progress_ind->progress_indicator.Length = 2;
   }

   /*
    * make sure IEI is correct
    */

   p_mncc_progress_ind->progress_indicator.Iei = IEI_PROGRESS_INDICATOR;

   /*
    * fill in message length field
    */

   PUT_IMH_LEN( sizeof(MNCC_PROGRESS_IND_T) - sizeof(IMH_T),
                &p_mncc_progress_ind->message_header );

   /*
    * store the progress if GSM coding
    */

   if (p_ie_progress->Length >= 2 )
   {

      /*
       * check for GSM coding
       */

      if ((p_ie_progress->CodstdLocation & 0x60) == 0x60 )
      {

         /*
          * extract the progress value
          */

         cc_progress_information[connection_id] =
                    (p_ie_progress->ProgressDescription & 0x7f);

      }
   }
}
/**************** End of   CNM_setup_mncc_progress_ind **************************/

/**************** Begin of   CNM_send_mn_rel_cnf **************************
 *
 *  Function name: CNM_send_mn_rel_cnf()
 *  -----------------------------------
 *
 *  Description:
 *  ------------
 *  Function to sends a  MNCC_REL_CNF message.
 *
 *  Parameters:
 *  -----------
 *  mncc_rel_cnf : OUT   - pointer to area where message is formed
 *  connection_id : IN   - connection id associated with this transaction
 *  cause : IN           - release cause
 *  data_length : IN     - length of the data to be included in the message
 *  data : IN            - data to be included in the message
 *
 *  Returns:
 *  --------
 *  none
 *
 ******************************************************************************/

VOID_FUNC CNM_send_mn_rel_cnf (connection_id_T connection_id,
                               lower_layer_cause_T cause,
                               int data_length,
                               byte *data)
{
    MNCC_REL_CNF_T mncc_rel_cnf;
    
    mncc_rel_cnf.message_header.message_set = MS_MN_CC;
    mncc_rel_cnf.message_header.message_id  = MNCC_REL_CNF;
    mncc_rel_cnf.connection_id = connection_id;

#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
    {
       mncc_rel_cnf.as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
    }
#endif


    mncc_rel_cnf.cause = cause;    

    if ((data_length > 0) && (data_length <= GSM_CAUSE_SIZE))
    {
       memscpy(mncc_rel_cnf.data, sizeof(mncc_rel_cnf.data) ,data, (unsigned int)data_length);

       if(mncc_rel_cnf.cause == 0)
       {
          mncc_rel_cnf.cause = data[3]&0x7f;
       }
       
    }
    else
    {
       MSG_HIGH_DS_0(MN_SUB,"=MNCNM= Number of bytes to copy is out of bounds in memscpy");
    }

    PUT_IMH_LEN(sizeof(MNCC_REL_CNF_T) - sizeof(IMH_T),
                &mncc_rel_cnf.message_header);

    CNM_send_message((IMH_T *)&mncc_rel_cnf,GS_QUEUE_MN_CM );

}

/**************** Begin of   CNM_send_mn_rel_cnf **************************/

