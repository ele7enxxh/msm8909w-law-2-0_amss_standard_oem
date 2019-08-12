/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2011, 2002 Qualcomm Technologies, Incorporated and its licensors.  All Rights 
Reserved.  QUALCOMM Proprietary.  Export of this technology or software 
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mn/vcs/mn_put_cnm_cc_msgs.c_v   1.10   13 May 2002 15:14:16   abanh  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mn/src/mn_put_cnm_cc_msgs.c#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
3/20/01     CD     Replaced old header with QCOM header
                   Removed #ifdef ASW_SIMBA

5/2/01      CD     Fixed compilation warning C4244

5/9/01      CD     Removed unnecesary header file includes

5/24/01     CD     Corrected assignment of data_ptr in MN_put_MNCC_DISC_REQ 
                   and MN_put_MNCC_REJ_REQto avoid lint error

                   Changed declaration of index in MN_put_MNCC_DISC_REQ to avoid loss of
                   sign/precission lint errors

                   Corrected assignment of message length to avoid many lint errors.
                   Removed unnecessary temp_message (most of them)

                   Removed cause.h since it was not used

7/18/01     CD     Removed duplicate external declaration of mn_call_information

8/10/01     CD     Removed unnecessary queue.h

08/14/01    AB     Renamed connection manager references to CNM from CM.

08/16/01    AB     Renamed MMI references to CM.

3-Sep-2001  VT     In MN_put_MNCC_SETUP_REQ(), after the message is sent
                   successfully, set 
                   mn_call_information[connection_id].mo_setup_pending to TRUE

01/25/02    CD     Updated copyright
                   Replaced calls to ERR() by MSG_ERR()
                   Replaced IEI by boolean flag

03/12/02    AB     Changed the MN_put_MNCC_DISC_REQ() interface to piggy  
                   bag call deflection, CD, invoke component on to
                   the OTA DISCONNECT that is sent the network.

05/06/02    AB     Added more meaningfull failure info in 
                   MN_put_MNCC_DISC_REQ() error logging
                   function, MSG_ERROR().

09/16/02    AB     Added MN_put_MNCC_REL_COMPL_REQ() function to send
                   a OTA RELEASE_COMPLETE reponse RELEASE message received
                   from the network. 

03/19/03    AB     Stored the cm_cc_cause value on MO call clearing.

09/24/03    AB     Added CC Capability IE, that indicated supported for
                   DTMF in CALL CONFIRM message.

02/24/04    AB     The CC Capability IE, is not defined in the emergency
                   call SETUP.

11/02/04    AB     Added SS call related User-to-User Signaling feature.

05/10/05    HS     Added support for FEATURE_CCBS

11/09/05    AB     Added supports for FEATURE_REL5 and 
                   FEATURE_EMERGENCY_CATEGORY
1/19/05      NR    Adding changes for CR 86364 i.e moving CC capabilities above Codes List                   

03/14/06    NR     Reading the NV item directly from mm i.e reading mm_nv_force_mscr_r99
                   and checking for it while sending call confirm i.e in MN_put_MNCC_CALL_CONF_REQ() 

03/05/06    NR     Adding changes for WB_AMR

11/03/11    PM     Including "Supported Codecs" IE in SETUP for CSFB calls as well.
===========================================================================*/

#include "mmcp_variation.h"
#include "customer.h"
#include "rex.h"
#include "environ.h"
#include "ms.h"
#include "timers_v.h"
#include "gs_v.h"
#include "mn.h"
#include "mnglobal.h"
#include "msg.h"
#include "err.h"
#include "mn_cm_exp_v.h"
#include "mm_v.h"
#include "ds_cap_v.h"
#include "mm.h"
#include "geran_grr_api.h"
#include <stringl/stringl.h>



extern boolean is_cs_tdscdma_mode( sys_modem_as_id_e_type as_id ); 

#ifdef FEATURE_TDSCDMA
#define is_tdscdma_mode() is_cs_tdscdma_mode(SYS_MODEM_AS_ID_1)
#endif

#ifdef FEATURE_REL5
extern ue_force_rel_mode_type  mm_get_nv_force_mscr_r99(  sys_modem_as_id_e_type sub_id);
extern ue_nas_rel_compliance_type mm_nv_nas_rel_compliance;

/* NV Variable which is User Configured through UI, about his AMR preference */
extern byte                      gsm_wcdma_amr_codec_preference;

#endif

extern byte mm_get_mscr_system_information
(
  sys_modem_as_id_e_type sub_id
);

/*
 * Function name :  MN_put_MNCC_ALERT_REQ
 * -------------
 *
 * Description :  Creates the MNCC_ALERT_REQ
 * -----------     message to the MN-CC
 *
 *
 *
 *
 *
 *
 * Parameters: connection_id
 * ----------  
 *
 *
 *
 * Returns:
 * --------
 *
 */

VOID_FUNC MN_put_MNCC_ALERT_REQ(connection_id_T connection_id)
{
   MNCC_ALERT_REQ_T   message_ptr;
   gs_status_T        func_return_val;
   int                message_size;

   message_ptr.message_header.message_set = MS_MN_CC;
   message_ptr.message_header.message_id = MNCC_ALERT_REQ;
   message_size = sizeof(MNCC_ALERT_REQ_T) - sizeof(IMH_T);
#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
   {
     message_ptr.as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
   }
#endif
   PUT_IMH_LEN(message_size,&message_ptr.message_header)

       /*  SET THE CONNECTION ID  */
   message_ptr.connection_id = connection_id;

   /*  SEND MESSAGE  */
   func_return_val = MN_send_message(GS_QUEUE_MN_CM, &message_ptr, TRUE);
  
   if(func_return_val != GS_SUCCESS)
   {
      MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: MNCC_ALERT_REQ message not sent");
   }

}



/*
 * Function name :  MN_put_MNCC_CALL_CONF_REQ
 * -------------
 *
 * Description :  Creates the MNCC_CALL_CONF_REQ message which
 * -----------    confirms to CC that the call can go ahead 
 *
 *
 *
 *
 *
 *
 * Parameters: connection_id
 * ----------  cm_cc_cause
 *             negotiation flag
 *
 *
 *
 * Returns:
 * --------
 *
 */



VOID_FUNC MN_put_MNCC_CALL_CONF_REQ(boolean           negotiation,
connection_id_T   connection_id,
ie_cm_cc_cause_T *cm_cc_cause)

{
   /* Locals */

   MNCC_CALL_CONF_REQ_T    *message_ptr;
   MNCC_CALL_CONF_REQ_T    temp_message;
   mn_call_information_T   *call_data_ptr;
   int                     data_index = 0;
   int                     index;
   int                     message_length;
   gs_status_T             func_return_val;
   byte                    *data_ptr;
   byte                    *last_octet_data_ptr;
   byte                    local_mscr = 0;
   ue_force_rel_mode_type local_nv_mscr = NV_FORCE_UE_REL99;

   if(connection_id >= MAXNO_CC_TRANSACTIONS)
   {
      MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Connection id invalid in mn_call_information_p");
   }

   else
   {
      message_ptr = &temp_message;

      message_ptr->message_header.message_set = MS_MN_CC;
      message_ptr->message_header.message_id = MNCC_CALL_CONF_REQ;

      /*  SET THE CONNECTION ID  */

      message_ptr->connection_id = connection_id;

#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
      {
        message_ptr->as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
      }
#endif



      /*  POINT AT CALL DATA  */

      call_data_ptr =  mn_call_information_p[connection_id];

      /*  POINT AT THE OUTPUT MESSAGE DATA  */

      data_index = 0;

      message_length = sizeof(connection_id_T) + sizeof(sys_modem_as_id_e_type);

      if (negotiation == TRUE)
      {
         /*  REPEAT INDICATOR  */

         if(call_data_ptr->repeat_indicator_bc == REPEAT_CIRCULAR)
         {
             if(call_data_ptr->bearer_capability_2.present == TRUE)  
             {
                 message_ptr->data[data_index] = IEI_REPEAT_INDICATOR | REPEAT_CIRCULAR;
                 data_index++;
                 message_length++;
             }
         }
         else if(call_data_ptr->repeat_indicator_bc == REPEAT_SUPPORTS_FALLBACK)
         {
            message_ptr->data[data_index] = IEI_REPEAT_INDICATOR | REPEAT_SUPPORTS_FALLBACK;

            data_index++;
            message_length++;
         }
         else if (call_data_ptr->repeat_indicator_bc == REPEAT_SERVICE_CHANGE_FALLBACK)
         {
            message_ptr->data[data_index] = IEI_REPEAT_INDICATOR | REPEAT_SERVICE_CHANGE_FALLBACK;

            data_index++;
            message_length++;
         }
         else
         {
            data_index = 0;  /* no repeat */
         }
        

         /*  CREATE A BEARER CAPABILITY IE  */
         if(call_data_ptr->bearer_capability_1.present == TRUE)
         {
            data_ptr = &message_ptr->data[data_index];
            last_octet_data_ptr = &message_ptr->data[sizeof(message_ptr->data) -1];
            index = MN_put_bearer_capability(data_ptr, 
                &call_data_ptr->bearer_capability_1,
                last_octet_data_ptr);

            data_index += index;
            message_length += index;
         }

         /*  CREATE A 2nd BEARER CAPABILITY IE  */
         if(call_data_ptr->bearer_capability_2.present == TRUE)
         {
            data_ptr = &message_ptr->data[data_index];
           last_octet_data_ptr = &message_ptr->data[sizeof(message_ptr->data) -1];

            index = MN_put_bearer_capability(data_ptr, 
                &call_data_ptr->bearer_capability_2,
                last_octet_data_ptr);

            data_index += index;
            message_length += index;
         }

      } /* negotiate */

      /*  ADD THE CAUSE IEI  */

      if(cm_cc_cause->present == TRUE)
      {
         data_ptr = &message_ptr->data[data_index];
         last_octet_data_ptr = &message_ptr->data[sizeof(message_ptr->data) -1];

         index = MN_put_cause_data(data_ptr, cm_cc_cause, last_octet_data_ptr);

         data_index += index;
         message_length += index;
      }

      /*  ADD CC CAPABILITY IEI  */
      if(data_index >= (int)((sizeof(ie_repeat_T) +    /* the repeat indicator */
                         2*sizeof(ie_bearer_capability_T) + GSM_CAUSE_SIZE)-4))
      {
          MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Index out of bound and buffer overflow");
      }
      else 
      {
          message_ptr->data[data_index++] = IEI_CALL_CONTROL_CAPABIL;
          message_ptr->data[data_index++] = 0x02;            /* length */
          message_ptr->data[data_index++] = 0x01;         /* DTMF supported */
          message_ptr->data[data_index++] = 0x00;         /* #Speech Bearer = 1 */
          message_length += 4;
      }

#if defined(FEATURE_MULTIMEDIA_FALLBACK) || defined(FEATURE_REL6)
      /* STREAM IDENTIFIER */     
      if(call_data_ptr->stream_id.present == TRUE)
      {
          if(data_index >= (int)((sizeof(ie_repeat_T) +    /* the repeat indicator */
                                  2*sizeof(ie_bearer_capability_T) + GSM_CAUSE_SIZE)-3))
          {
            MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Index out of bound and buffer overflow");
          }
          else
          {
            message_ptr->data[data_index++] = IEI_STREAM_IDENTIFIER;
            message_ptr->data[data_index++] = 0x01;       /* length */
            message_ptr->data[data_index++] = call_data_ptr->stream_id.id; 
            message_length += 3;
          }
      }
#endif /* FEATURE_MULTIMEDIA_FALLBACK */      

MSG_HIGH_DS_1(MN_SUB,"=MNCNM= =MM= als_itc_aux_speech while forming CALL CONF : %d\n ",mn_get_als_itc_aux_speech());

#ifdef FEATURE_DUAL_SIM
      if(MNCNM_IS_SINGLE_STACK)
      {
        if((mn_as_id > SYS_MODEM_AS_ID_NONE) && (mn_as_id < MAX_AS_IDS) )
        {
          local_mscr = mm_get_mscr_system_information(mn_as_id);
          local_nv_mscr = mm_get_nv_force_mscr_r99(mn_as_id);
        }
      }
#ifdef FEATURE_DUAL_ACTIVE
      else
      {
        local_mscr = mm_get_mscr_system_information((sys_modem_as_id_e_type)mn_dsda_as_id);
        local_nv_mscr = mm_get_nv_force_mscr_r99((sys_modem_as_id_e_type)mn_dsda_as_id);
      }
#endif
#else      
      {
        local_mscr = mm_get_mscr_system_information(SYS_MODEM_AS_ID_1);
        local_nv_mscr = mm_get_nv_force_mscr_r99(SYS_MODEM_AS_ID_1);
      }
#endif

#ifdef FEATURE_REL5
      if( ( (local_nv_mscr == NV_FORCE_UE_REL99) ||
            ( (local_nv_mscr == NV_FORCE_UE_DYNAMIC) && 
            (local_mscr ))) && 
            (mm_nv_nas_rel_compliance >= NV_NAS_COMPLIANCE_REL5) ) 
      {

        if (((call_data_ptr->bearer_capability_1.present == TRUE) && 
             ((call_data_ptr->bearer_capability_1.information_transfer_capability == ITC_SPEECH) ||
              (call_data_ptr->bearer_capability_1.information_transfer_capability == ITC_AUX_SPEECH_4) ||
              (call_data_ptr->bearer_capability_1.information_transfer_capability == ITC_AUX_SPEECH_6))) ||
            ((call_data_ptr->bearer_capability_2.present == TRUE) && 
             ((call_data_ptr->bearer_capability_2.information_transfer_capability == ITC_SPEECH) ||
              (call_data_ptr->bearer_capability_2.information_transfer_capability == ITC_AUX_SPEECH_4) ||
              (call_data_ptr->bearer_capability_2.information_transfer_capability == ITC_AUX_SPEECH_6))))
        {

          sys_modem_as_id_e_type local_as_id = 0;
#ifdef FEATURE_DUAL_SIM
#ifdef FEATURE_DUAL_ACTIVE
          local_as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
#else
          local_as_id = mn_as_id;
#endif
#endif

          /* SUPPORTED CODEC LIST */     
/*                                  -----  BITMAP  ------                                  */          
/* ----------------------------------------------------------*/                    
/* |  08     |   07   | 06       | 05       |  04   | 03        | 02       | 01     | */ 
/* ----------------------------------------------------------*/          
/* | TDMA | UMTS | UMTS  | HR       | FR    | GSM     | GSM    |  GSM | */          
/* |  EFR   | AMR2 | AMR    | AMR    | AMR  | EFR      |  HR      |  FR    | */          
/* -----------------------------------------------------------*/                    
/* |  16     | 15     | 14       | 13       |  12   |   11      |   10      | 09     |  */ 
/* -----------------------------------------------------------*/                    
/* | RESE  | RESE | OHR  A| OFR A  | OHR  |UMTS    |    FR     | PDC  |  */          
/* | RVED | RVED |MR-WB | MR-WB| AMR |AMR-WB|AMR-WB|  EFR  | */          
/* -----------------------------------------------------------*/                    

          /* IEI_SUPPORTED_CODECS_LIST */
          if(data_index >  (int)(sizeof(message_ptr->data)-10))    /* the repeat indicator */
          {
            MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Index out of bound and buffer overflow");
          }
          else
          {
            message_ptr->data[data_index++] = IEI_SUPPORTED_CODECS_LIST; 
            message_ptr->data[data_index++] = 0x08; /* IEI length */
            /* SysID = UMTS */ 
            message_ptr->data[data_index++] = SYSID_UMTS; 
            message_ptr->data[data_index++] = 0x02; /* bitmap length */

            /* If RAT is TDS, then don't send AMR2 in Supp Codec List */

            if(is_cs_tdscdma_mode(local_as_id)) /* Single SIM mode */
            {
              message_ptr->data[data_index++] = 0x20; /* bitmap 8...1 */
            }
            else
            {
              message_ptr->data[data_index++] = 0x60; /* bitmap 8...1 */
            }

/* Encode WCDMA AMR WB Codec based on NV Bit  */
            if( (gsm_wcdma_amr_codec_preference & 0x08) ) 
            {
              message_ptr->data[data_index++] = 0x04; /* bitmap 16...9 */
            }
            else
            {
              message_ptr->data[data_index++] = 0x00; /* bitmap 16...9 */
            }
            /* SysID = GSM */
            message_ptr->data[data_index++] = SYSID_GSM;
            message_ptr->data[data_index++] = 0x02; /* bitmap length */

/* GSM AMR codecs should be included conditionally depending on the 
    NV Item - NV_GSM_AMR_CALL_CONFIG_I */         
            
            message_ptr->data[data_index] = 0x07; /* bitmap 8...1 */
            if( (gsm_wcdma_amr_codec_preference & 0x01) )
            {
              /*GSM AMR NB is supported*/
              message_ptr->data[data_index] |= 0x08; /* bitmap 8...1 */
            }
            if( (gsm_wcdma_amr_codec_preference & 0x04) )
            {
              /*GSM AMR NB is supported*/
              message_ptr->data[data_index] |= 0x10; /* bitmap 8...1 */
            }
            data_index++;
            if( (gsm_wcdma_amr_codec_preference & 0x02) )
            {
              /*GSM AMR WB is supported*/
              message_ptr->data[data_index++] = 0x02; /* bitmap 16...9 */
            }
            else
            {
              /*GSM AMR WB is NOT supported*/
              message_ptr->data[data_index++] = 0x00; /* bitmap 16...9 */           
            }
            message_length += 10;
          }
        }
      }
#endif /* FEATURE_REL5 */

      PUT_IMH_LEN(message_length,&(message_ptr->message_header))

      /*  SEND MESSAGE  */
      func_return_val = MN_send_message(GS_QUEUE_MN_CM, message_ptr, TRUE);

      if(func_return_val != GS_SUCCESS)
      {
         MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Send message error");
 
      }
   }
}

/*
 *   Function name : MN_put_MNCC_CALL_RETRIEVE_REQ
 *   ---------------
 *
 *   Description:
 *   ------------
 *  This function generates the MNCC_CALL_RETRIEVE_REQ message to the 
 *  CM-CC.
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

VOID_FUNC MN_put_MNCC_CALL_RETRIEVE_REQ(connection_id_T connection_id)
{

   /* Locals */

   MNCC_CALL_RETRIEVE_REQ_T   message_ptr;
   gs_status_T                func_return_val;


   message_ptr.message_header.message_set = MS_MN_CC;
   message_ptr.message_header.message_id = MNCC_CALL_RETRIEVE_REQ;
   PUT_IMH_LEN((sizeof(MNCC_CALL_RETRIEVE_REQ_T) - sizeof(IMH_T)),&message_ptr.message_header)

   /*  SET THE CONNECTION ID  */
   message_ptr.connection_id = connection_id;

#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
   {
     message_ptr.as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
   }
#endif


   /*  SEND MESSAGE  */
   func_return_val = MN_send_message(GS_QUEUE_MN_CM, &message_ptr, TRUE);

   if(func_return_val != GS_SUCCESS)
   {
      MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Send message error");
   }
}


/*
 *   Function name : MN_put_MNCC_DISC_REQ
 *   ---------------
 *
 *   Description:
 *   ------------
 *  This function generates the MNCC_DISC_REQ message to the MN-CC
 *  
 *  
 *
 *  Parameters:   connection_id
 *  -----------   cm_cc_cause
 *                
 *     
 *
 *  Returns:  none
 *  --------
 *
 *
 */

VOID_FUNC MN_put_MNCC_DISC_REQ(  connection_id_T   connection_id, 
                                 ie_cm_cc_cause_T *cm_cc_cause, 
                                 ie_cm_cc_invoke_T *cm_cc_invoke)
{
    MNCC_DISC_REQ_T      message_ptr;
    gs_status_T          func_return_val;
    byte                 *data_ptr;
    byte                 *last_octet_data_ptr; /* pointer to last octet in the buffer where data_ptr points */
    byte                 index = 0;
    int                  message_length = 0;

    message_ptr.message_header.message_set = MS_MN_CC;
    message_ptr.message_header.message_id = MNCC_DISC_REQ;
    message_length = sizeof(connection_id_T) + sizeof(sys_modem_as_id_e_type);

#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
    {
      message_ptr.as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
    }
#endif

  
    /*  SET THE CONNECTION ID  */
    message_ptr.connection_id = connection_id;

    if(connection_id <  MAXNO_CC_TRANSACTIONS)
    {
       if(cm_cc_cause != NULL)
       {
           /*  SET UP THE CAUSE or INVOKE DATA  */
           data_ptr = &message_ptr.data[index];
           last_octet_data_ptr = &message_ptr.data[sizeof(message_ptr.data) -1];

           index += MN_put_cause_data(data_ptr, cm_cc_cause, last_octet_data_ptr); /* mandatory */
           if(index >= (int)sizeof(message_ptr.data))
           {
               MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Index out of bound and buffer overflow");
           }
           else if(cm_cc_invoke != NULL)
           {
               index += MN_put_invoke_cd_data(data_ptr + index, cm_cc_invoke, last_octet_data_ptr);
           }

           /* save the CM cc cause value */
           mn_call_information_p[connection_id]->cm_cc_cause = *cm_cc_cause;
           mn_call_information_p[connection_id]->cm_cc_cause.present = TRUE;
       }
       else
       {
           MSG_ERROR_DS_2(MN_SUB,"=MNCNM= Missing Mandatory CAUSE value = %d, %d", connection_id, index);
       }

#ifdef FEATURE_UUS
       /* Check for User-to-User Signalling */
       if((mn_call_information_p[connection_id]->user_user_data.present) &&
          (mn_call_information_p[connection_id]->user_user_data.type <= (byte)UUS1_NOT_REQUIRED))
       {
         cm_mo_user_data_req_T  *user_data;
         byte   ie_user_data_length;         

         user_data = MN_get_user_data(connection_id);
         if(user_data)
         {
            ie_user_data_length =  user_data->data_length + 1;  /* including protocol discriminator */
                /* To ensure that memory overflow doesn't occur */
            if((byte)(ie_user_data_length - 1) >= MIN((sizeof(user_data->data)),(sizeof(message_ptr.data) - (index + 3))))
            {
              MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Index out of bound and buffer overflow");
            }
            else
            {
              message_ptr.data[index++] = IEI_USER_USER;
              message_ptr.data[index++] = ie_user_data_length; 
              message_ptr.data[index++] = user_data->protocol_discriminator;
              memscpy(&message_ptr.data[index], sizeof(message_ptr.data)-index ,user_data->data, ie_user_data_length -1);  /* user-user data length */
              index += ie_user_data_length - 1;

            }
            /* release the buffer */
            MN_free_user_data(user_data);
         } /* user_data */

       } /* UUS1_IMPLICIT */ 
#endif  /* FEATURE_UUS */

       message_length += index;

       PUT_IMH_LEN(message_length, &message_ptr.message_header)

       /*  SEND MESSAGE  */
       func_return_val = MN_send_message(GS_QUEUE_MN_CM, &message_ptr, TRUE);

       if(func_return_val != GS_SUCCESS)
       {
           MSG_ERROR_DS_1(MN_SUB,"=MNCNM= ERROR: Send message error %d", connection_id);
       }
    } 
    else
    {
        MSG_ERROR_DS_1(MN_SUB,"=MNCNM= Invalid conn id= %d", connection_id);
    }

    /* Enabling DXT blindly to consider the case where call is
       disconected before it is connected and another call is active*/
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

#if defined(FEATURE_MULTIMEDIA_FALLBACK) || defined(FEATURE_REL6) 

/*
 *   Function name : MN_put_MNCC_MODIFY_REQ
 *   ---------------
 *
 *   Description:
 *   ------------
 *  This function generates the MNCC_MODIFY_REQ message to the 
 *  MN-CC.
 *  
 *  
 *
 *  Parameters:
 *  -----------
 *  IN  modify_data
 *
 *  Returns:
 *
 *
 */

VOID_FUNC MN_put_MNCC_MODIFY_REQ(MNCC_MODIFY_data_T *modify_data_ptr)
{

   /* Locals */

   MNCC_MODIFY_REQ_T    *message_ptr_3;
   MNCC_MODIFY_REQ_T    temp_message_3;
   byte                 *data_ptr;
   byte                 *last_octet_data_ptr;
   int                  data_index;
   int                  index;
   int                  message_size;
   gs_status_T          func_return_val;



   message_ptr_3 = &temp_message_3;

   (message_ptr_3->message_header).message_set = MS_MN_CC;
   (message_ptr_3->message_header).message_id = MNCC_MODIFY_REQ;

   message_size = sizeof(connection_id_T) + sizeof(sys_modem_as_id_e_type);

#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
   {
     message_ptr_3->as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
   }
#endif


   /*  SET THE CONNECTION ID  */

   message_ptr_3->connection_id = modify_data_ptr->connection_id;

   data_index = 0;

   /*  SET UP BEARER CAPABILITY DATA  */

   data_ptr = &message_ptr_3->data[data_index];
   last_octet_data_ptr = &message_ptr_3->data[sizeof(message_ptr_3->data) -1];

   index = MN_put_bearer_capability(data_ptr, &modify_data_ptr->cm_bearer_capability, last_octet_data_ptr);

   data_index += index;
   message_size += index;

   /*  SET THE LOW LAYER COMPATIBILTY DATA  */
   /*  THERE CAN BE ONLY 0 OR 1 LOW LAYER COMPATIBILITIES  */

   if(modify_data_ptr->repeat_indication_llc == 1)
   {

      data_ptr = &message_ptr_3->data[data_index];

      index = MN_put_llc_data(data_ptr, &modify_data_ptr->cm_low_layer_compatibility, last_octet_data_ptr);

      data_index += index;
      message_size += index;
   }

   /*  SET THE HIGH LAYER COMPATIBILITY DATA  */
   /*  THERE CAN BE ONLY 0 OR 1 HIGH LAYER COMPATIBILITIES  */

   if(modify_data_ptr->repeat_indication_hlc == 1)
   {

      data_ptr = &message_ptr_3->data[data_index];

      index = MN_put_hlc_data(data_ptr, &modify_data_ptr->cm_high_layer_compatibility, last_octet_data_ptr);

      data_index += index;
      message_size += index;
   }

   /*  SET UP THE REVERSE CALL DIRECTION DATA  */

   if(modify_data_ptr->reverse_call_direction == TRUE)
   {
      message_ptr_3->data[data_index++] = IEI_REVERSE_CALL_SETUP_DIR;
      message_size += 1;
   }

   /*  SET THE MESSAGE LENGTH  */

   PUT_IMH_LEN(message_size,&(message_ptr_3->message_header))

   /*  SEND THE MESSAGE  */

   func_return_val = MN_send_message(GS_QUEUE_MN_CM, message_ptr_3, TRUE);

   if(func_return_val != GS_SUCCESS)
   {
      MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Send message error");
   }
}
#endif  /* FEATURE_MULTIMEDIA_FALLBACK */

/*
 *   Function name : MN_put_MNCC_FACILITY_REQ
 *   ---------------
 *
 *   Description:
 *   ------------
 *  This function generates the MNCC_FACILITY_REQ message to the 
 *   MN-CC.
 *  
 *  
 *
 *  Parameters:
 *  -----------
 *  connection_id
 *  components[]
 *  component_length
 *  ss_version[]
 *  ss_version_length
 *
 *  Returns:
 *
 *
 */

VOID_FUNC MN_put_MNCC_FACILITY_REQ(connection_id_T connection_id,
byte  components[MAX_COMPONENTS_LENGTH],
int   components_length,
byte  ss_version[MAX_SS_VERSION_IND_LENGTH],
int   ss_version_length)
{

   /* Locals */

   MNCC_FACILITY_REQ_T  *message_ptr;
   MNCC_FACILITY_REQ_T  temp_message;
   int         counter = 0;
   int         index;
   int         message_length;
   gs_status_T    func_return_val;
   message_ptr = &temp_message;

   if(components_length > MAX_COMPONENTS_LENGTH)
   {
      MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Data Length exceeds maximum");
   }
   else
   {
      message_ptr->message_header.message_set = MS_MN_CC;
      message_ptr->message_header.message_id = MNCC_FACILITY_REQ;

#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
      {
        message_ptr->as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
      }
#endif


      message_length = sizeof(connection_id) + sizeof(sys_modem_as_id_e_type);

      /*  SET THE CONNECTION ID  */

      message_ptr->connection_id = connection_id;

      /*  LOAD FACILITY DATA IN TO MESSAGE  */

      message_ptr->data[counter++] = IEI_FACILITY;
      message_length++;
      message_ptr->data[counter++] = (byte)components_length;
      message_length++;
      for(index = 0; index < components_length; index++)
      {
         message_ptr->data[counter++] = components[index];
      }
      message_length += components_length;

      /*  LOAD THE SS VERSION IN TO THE MESSAGE  */
      /* Klocwork Fix */ 
      if( 1 == ss_version_length )
      {
         message_ptr->data[counter++] = IEI_SS_VERSION_INDICATOR;
         message_length++;
         message_ptr->data[counter++] = (byte)ss_version_length;
         message_length++;
         for(index = 0; index < ss_version_length; index++)
         {
            message_ptr->data[counter++] = ss_version[index];
         }
         message_length += ss_version_length;
      }

      /* Set message length */

      PUT_IMH_LEN(message_length,&(message_ptr->message_header))

          /*  SEND MESSAGE  */

      func_return_val = MN_send_message(GS_QUEUE_MN_CM, message_ptr, TRUE);

      if(func_return_val != GS_SUCCESS)
      {
         MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Send MNCC_FACILITY_REQ message error");

      }
   }
}



/*
 *   Function name : MN_put_MNCC_CALL_HOLD_REQ
 *   ---------------
 *
 *   Description:
 *   ------------
 *  This function generates the MNCC_CALL_HOLD_REQ message to the 
 *  MN-CC. Indicates that user wishes to hold a call.
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

VOID_FUNC MN_put_MNCC_CALL_HOLD_REQ(connection_id_T connection_id)
{
   MNCC_CALL_HOLD_REQ_T message_ptr;
   gs_status_T    func_return_val;

   message_ptr.message_header.message_set = MS_MN_CC;
   message_ptr.message_header.message_id = MNCC_CALL_HOLD_REQ;

   /*
   (message_ptr->message_header).message_len_lsb = 
       (byte)(sizeof(connection_id_T) & 0xff);
   (message_ptr->message_header).message_len_msb = 
       (byte)((sizeof(connection_id_T)>>8) & 0xff); */

   PUT_IMH_LEN((sizeof(MNCC_CALL_HOLD_REQ_T) - sizeof(IMH_T)), &message_ptr.message_header)

   /*  SET THE CONNECTION ID  */

   message_ptr.connection_id = connection_id;

#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
   {
     message_ptr.as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
   }
#endif


   /*  SEND MESSAGE  */

   func_return_val = MN_send_message(GS_QUEUE_MN_CM, &message_ptr, TRUE);

   if(func_return_val != GS_SUCCESS)
   {
       MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Send MNCC_CALL_HOLD_REQ message error");
   }
}

#if defined(FEATURE_MULTIMEDIA_FALLBACK) || defined(FEATURE_REL6)
/*
 * Function name :  MN_put_MNCC_MODIFY_RES
 * -------------
 *
 * Description :  Creates the above named message
 * -----------    message and sends to the GS_QUEUE_MN_CC
 *                queue. Responds to modify request sent
 *                by CC
 *
 *
 * Uses :       
 * ----
 *
 *
 * Parameters:  connection_id
 * ----------   MNCC_MODIFY_data
 *
 *
 *
 * Returns:
 * --------
 *
 */

VOID_FUNC MN_put_MNCC_MODIFY_RES(connection_id_T      connection_id, 
MNCC_MODIFY_data_T   *modify_data_ptr)
{
   MNCC_MODIFY_RES_T       temp_message;
   MNCC_MODIFY_RES_T       *message_ptr;
   int                     data_index;
   int                     index;
   int                     message_length;
   byte                    *data_ptr;
   byte                    *last_octet_data_ptr;
   gs_status_T       func_return_val;

   message_ptr = &temp_message;

   message_ptr->message_header.message_set = MS_MN_CC;
   message_ptr->message_header.message_id = MNCC_MODIFY_RES;

   message_length = sizeof(connection_id) + sizeof(sys_modem_as_id_e_type);

#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
   {
     message_ptr.as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
   }
#endif


   /*  SET THE CONNECTION ID  */

   message_ptr->connection_id = connection_id;

   data_index = 0;

   /*  SET UP BEARER CAPABILITY DATA  */

   data_ptr = &message_ptr->data[data_index];
   last_octet_data_ptr = &message_ptr->data[sizeof(message_ptr->data) -1];

   index = MN_put_bearer_capability(data_ptr, 
       &modify_data_ptr->cm_bearer_capability,
       last_octet_data_ptr);

   data_index += index;
   message_length += index;

   /*  SET UP THE CAUSE DATA ONLY FOR A `REJECT' MESSAGE  */

   if(modify_data_ptr->cause_present == TRUE)
   {
      data_ptr = &message_ptr->data[data_index];
      last_octet_data_ptr = &message_ptr->data[sizeof(message_ptr->data) -1];

      index = MN_put_cause_data(data_ptr, &modify_data_ptr->cm_cc_cause, last_octet_data_ptr);

      data_index += index;
      message_length += index;
   }


   /*  SET UP THE LOW LAYER COMPATIBILITY DATA  */
   /*  THERE CAN BE ONLY 0 OR 1 LOW LAYER COMPATIBILITIES  */

   if(modify_data_ptr->repeat_indication_llc == 1)
   {
      data_ptr = &message_ptr->data[data_index];
      index = MN_put_llc_data(data_ptr, 
          &modify_data_ptr->cm_low_layer_compatibility,
          last_octet_data_ptr);

      data_index += index;
      message_length += index;
   }

   /*  SET UP THE HIGH LAYER COMPATIBILITY DATA  */
   /*  THERE CAN BE ONLY 0 OR 1 HIGH LAYER COMPATIBILITIES  */

   if(modify_data_ptr->repeat_indication_hlc == 1)
   {
      data_ptr = &message_ptr->data[data_index];
      index = MN_put_hlc_data(data_ptr, 
          &modify_data_ptr->cm_high_layer_compatibility, last_octet_data_ptr);

      data_index += index;
      message_length += index;
   }

   /*  
                * SET UP REVERSE CALL DIRECTION DATA - ONLY IF THE MESSAGE DOES NOT 
                * SIGNify REJECTION  
                */

   if(modify_data_ptr->cause_present != TRUE)
   {
      if(modify_data_ptr->reverse_call_direction == TRUE)
      {
         message_ptr->data[data_index++] = IEI_REVERSE_CALL_SETUP_DIR;
         message_length++;

      }
   }

   /*  SET THE MESSAGE LENGTH  */

   PUT_IMH_LEN(message_length,&(message_ptr->message_header))


   /*  SEND THE MESSAGE  */

   func_return_val = MN_send_message(GS_QUEUE_MN_CM, message_ptr, TRUE);
   if(func_return_val != GS_SUCCESS)
   {
      MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Send MNCC_MODIFY_RES message error");

   }

}
#endif /* FEATURE_MULTIMEDIA_FALLBACK */



/*
 * Function name :  MN_put_MNCC_REJ_REQ
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
 * Parameters:  connection_id
 * ----------   cm_cc_cause
 *
 *
 *
 * Returns:
 * --------
 *
 */
VOID_FUNC MN_put_MNCC_REJ_REQ(connection_id_T   connection_id,
ie_cm_cc_cause_T *cm_cc_cause)
{

   /* Local */

   MNCC_REJ_REQ_T       message_ptr;
   int                  index;
   int                  message_length;
   gs_status_T          func_return_val;


   message_ptr.message_header.message_set = MS_MN_CC;
   message_ptr.message_header.message_id = MNCC_REJ_REQ;
   message_length = sizeof(connection_id) + sizeof(sys_modem_as_id_e_type);

#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
   {
     message_ptr.as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
   }
#endif


   /*  SET THE CONNECTION ID  */

   message_ptr.connection_id = connection_id;

   /*  SET UP THE CAUSE DATA  */

   index = MN_put_cause_data(message_ptr.cause, cm_cc_cause, (message_ptr.cause + sizeof(message_ptr.cause) - 1));

   message_length += index;

   /*  SET MESSAGE LENGTH  */

   PUT_IMH_LEN(message_length,&message_ptr.message_header)

       /*  SEND MESSAGE  */

   func_return_val = MN_send_message(GS_QUEUE_MN_CM, &message_ptr, TRUE);

   if(func_return_val != GS_SUCCESS)
   {
      MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Send MNCC_REJ_REQ message error");
  
   }
}

/*
 * Function name :  MN_put_MNCC_MODIFY_ACCEPT
 * -------------
 *
 * Description :   Send message to CC indicating that the modification 
 * -----------     has been accepted 
 *
 *
 *
 *
 *
 *
 * Parameters:    connection_id
 * ----------     accept
 *
 *
 *
 * Returns:  none
 * --------
 *
 */

VOID_FUNC MN_put_MNCC_MODIFY_ACCEPT(connection_id_T connection_id,
boolean         accept)

{

   /* Locals */


   MNCC_MODIFY_ACCEPT_T  message;
   int                   message_length;
   gs_status_T           GSstatus;


   /* Set up the message header */

   message.message_header.message_set = MS_MN_CC;
   message.message_header.message_id = MNCC_MODIFY_ACCEPT;
   message_length = sizeof(MNCC_MODIFY_ACCEPT_T) - sizeof(IMH_T);
   PUT_IMH_LEN(message_length,&message.message_header)

       /* Set up the connection id */

   message.connection_id = connection_id;

#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
   {
     message.as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
   }
#endif


   /* Set up the accept element */

   message.accept = accept;


   /* Send the message */

   GSstatus = MN_send_message(GS_QUEUE_MN_CM,&message,TRUE);

   /* log any failures */
   if(GSstatus != GS_SUCCESS)

   {

      MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Failed to send MNCC_MODIFY_ACCEPT message");
   }
}

/*
 *   Function name : MN_put_MNCC_REL_REQ
 *   ---------------
 *
 *   Description:
 *   ------------
 *  This function generates the MNCC_REL_REQ message to the 
 *  MN-CC . Requests the release of the call.
 *  
 *  
 *
 *  Parameters:
 *  -----------
 *  IN connection_id
 *    *cm_cc_cause_1
 *    *cm_cc_cause_2
 *    no_of_causes
 *
 *  Returns:
 *
 *
 */

VOID_FUNC MN_put_MNCC_REL_REQ(connection_id_T    connection_id,
ie_cm_cc_cause_T *cm_cc_cause_1,
ie_cm_cc_cause_T *cm_cc_cause_2,
int                no_of_causes
#ifdef FEATURE_CCBS
,ie_facility_T *p_facility
#endif /* FEATURE_CCBS */
)
{

   /* Locals */

   MNCC_REL_REQ_T    message_ptr;
   int               index = 0, message_length = 0;
   byte              *data_ptr;
   byte              *last_octet_data_ptr;
   gs_status_T       func_return_val;


   message_ptr.message_header.message_set = MS_MN_CC;
   message_ptr.message_header.message_id = MNCC_REL_REQ;
   message_length = sizeof(connection_id_T) + sizeof(sys_modem_as_id_e_type);

#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
   {
     message_ptr.as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
   }
#endif


   /*  SET THE CONNECTION ID  */
   message_ptr.connection_id = connection_id;
   
   /*  SET UP THE CAUSE DATA  */
   if( (no_of_causes == 1) || (no_of_causes == 2) )
   {
      /* Deference the data portion */
      data_ptr = &message_ptr.data[index];
      last_octet_data_ptr = &message_ptr.data[sizeof(message_ptr.data) -1];

      index = MN_put_cause_data(data_ptr, cm_cc_cause_1, last_octet_data_ptr);

      /*
       * If cm_cc_cause is not present in mn_call_information_p
       * global variable, it means that RELEASE is first call clearing message
       * So, storing cause value now in the global variable.
       */
       
      if(mn_call_information_p[connection_id] != NULL)
      {
        /* save the CM cc cause value, if its already not present */
        mn_call_information_p[connection_id]->mo_release_cause_diag_packet.cause_value = cm_cc_cause_1->cause_value;
        mn_call_information_p[connection_id]->mo_release_cause_diag_packet.present = TRUE;
      }

   }

   /*  IF THERE ARE TWO CAUSES  */
   /*  SET UP THE SECOND CAUSE DATA  */

   if(no_of_causes == 2)
   {
      data_ptr = &message_ptr.data[0];
      index = MN_put_cause_data(data_ptr, cm_cc_cause_2, last_octet_data_ptr);

      if(mn_call_information_p[connection_id] != NULL)
      {
        /* save the CM cc cause value, if its already not present */
        mn_call_information_p[connection_id]->mo_release_cause_diag_packet.cause_value = cm_cc_cause_2->cause_value;
        mn_call_information_p[connection_id]->mo_release_cause_diag_packet.present = TRUE;
      }

   }
#ifdef FEATURE_CCBS
   if (p_facility != NULL)
   {
      byte length = p_facility->Length;
         /* To ensure that memory overflow doesn't occur */
      if(length > MIN((sizeof(p_facility->data)),(sizeof(message_ptr.data) - (index + 2))))
      {
         MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Index out of bound and buffer overflow");
      }
      else
      {
         /* generate facility IE */
         message_ptr.data[index++] = IEI_FACILITY;
         message_ptr.data[index++] = length;
         memscpy ( &message_ptr.data[index], sizeof(message_ptr.data)-index ,p_facility->data, length);
         index += length;
      }

      /* generate SS ver ind IE */
      if(index > (int)(sizeof(message_ptr.data) - 3))
      {
         MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Index out of bound and buffer overflow");
      }
      else
      {
        message_ptr.data[index++] = IEI_SS_VERSION_INDICATOR;
        message_ptr.data[index++] = 0x01;
        message_ptr.data[index++] = 0x01; /* SS version 3 */
      }
   }
#endif /* FEATURE_CCBS */

#ifdef FEATURE_UUS
   /* Check for User-to-User Signalling */
   if((mn_call_information_p[connection_id]->user_user_data.present) &&
      (mn_call_information_p[connection_id]->user_user_data.type <= (byte)UUS1_NOT_REQUIRED))
   {
     cm_mo_user_data_req_T  *user_data;
     byte   ie_user_data_length;         

     user_data = MN_get_user_data(connection_id);

     if(user_data)
     {
        ie_user_data_length =  user_data->data_length + 1;  /* including protocol discriminator */
           /* To ensure that memory overflow doesn't occur */
        if((byte)(ie_user_data_length - 1) > MIN((sizeof(user_data->data)),(sizeof(message_ptr.data) - (index + 3))))
        {
          MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Index out of bound and buffer overflow");
        }
        else
        {
          message_ptr.data[index++] = IEI_USER_USER;
          message_ptr.data[index++] = ie_user_data_length; 
          message_ptr.data[index++] = user_data->protocol_discriminator;
          memscpy(&message_ptr.data[index], sizeof(message_ptr.data)-index ,user_data->data, ie_user_data_length -1);  /* user-user data length */
          index += ie_user_data_length -1;

        }
        /* release the buffer */
        MN_free_user_data(user_data); 

     } /* user_data */
     
   } /* UUS1_IMPLICIT */ 
#endif

   message_length += index;

   /*  SET MESSAGE_LENGTH  */
   PUT_IMH_LEN(message_length,&message_ptr.message_header)

   /*  SEND THE MESSAGE  */
   func_return_val = MN_send_message(GS_QUEUE_MN_CM, &message_ptr, TRUE);

   if(func_return_val != GS_SUCCESS)
   {

      MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Send MNCC_REL_REQ message error");

   }
}


/*
 *   Function name : MN_put_MNCC_REL_COMPL_REQ()
 *   ---------------
 *
 *   Description:
 *   ------------
 *   This function generates the MNCC_REL_COMPL_REQ message to send
 *   an OTA RELEASE_COMPLETE in respond to RELEASE message received
 *   from the network.
 *  
 *
 *  Parameters:
 *  -----------
 *   connection_id
 *   causes
 *   *ie_facility_ptr
 * 
 *  Returns:
 *   none.
 *
 */
VOID_FUNC MN_put_MNCC_REL_COMPL_REQ(connection_id_T    connection_id,
                                    byte pd,
                                    byte cause,
                                    ie_facility_T *p_facility)
{

   gs_status_T             func_return_val;
   MNCC_REL_COMPL_REQ_T    message;


   message.message_header.message_set = MS_MN_CC;
   message.message_header.message_id = MNCC_REL_COMPL_REQ;
   PUT_IMH_LEN(sizeof(MNCC_REL_COMPL_REQ_T) - sizeof(IMH_T), &message.message_header)

   /*  SET THE CONNECTION ID  */
   message.connection_id = connection_id;

   message.pd = pd;
   message.cause = cause;
   message.p_facility = p_facility;
#if defined(FEATURE_DUAL_SIM )
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

   /*  SEND MESSAGE  */
   func_return_val = MN_send_message(GS_QUEUE_MN_CM, &message, TRUE);

   if(func_return_val != GS_SUCCESS)
   {
      MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Send MNCC_REL_COMPL_REQ message error");
   }
}



/*
 *   Function name : MN_put_MNCC_SETUP_COMPL_REQ
 *   ---------------
 *
 *   Description:
 *   ------------
 *  This function generates the MNCC_SETUP_COMPL_REQ message to the 
 *  MN-CC. Indicates that setup of call is being requested. 
 *  
 *  
 *
 *  Parameters:
 *  -----------
 *  IN connection_id
 *    
 *    
 *
 *  Returns:
 *
 *
 */

VOID_FUNC MN_put_MNCC_SETUP_COMPL_REQ(connection_id_T connection_id)
{

   /* Locals */

   gs_status_T             func_return_val;
   MNCC_SETUP_COMPL_REQ_T  message_ptr_2;


   message_ptr_2.message_header.message_set = MS_MN_CC;
   message_ptr_2.message_header.message_id = MNCC_SETUP_COMPL_REQ;
   PUT_IMH_LEN((sizeof(MNCC_SETUP_COMPL_REQ_T) - sizeof(IMH_T)),&message_ptr_2.message_header);

       /*  SET THE CONNECTION ID  */
   message_ptr_2.connection_id = connection_id;

#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
   {
     message_ptr_2.as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
   }
#endif


   /*  SEND MESSAGE  */
   func_return_val = MN_send_message(GS_QUEUE_MN_CM, &message_ptr_2, TRUE);

   if(func_return_val != GS_SUCCESS)
   {
      MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Send MNCC_SETUP_COMPL_REQ message error");
   }
}


/*
 *   Function name : MN_put_MNCC_SETUP_COMPL_RES
 *   ---------------
 *
 *   Description:
 *   ------------
 *  This function generates the MNCC_SETUP_COMPL_RES message to the 
 *  MN_CC. Response to the setup complete request. 
 *  
 *  
 *
 *  Parameters:
 *  -----------
 *  IN connection_id
 *   
 *
 *  Returns:
 *
 *
 */
VOID_FUNC MN_put_MNCC_SETUP_COMPL_RES(connection_id_T connection_id)
{

   /* locals */

   gs_status_T             func_return_val;
   MNCC_SETUP_COMPL_RES_T  message_ptr_3;


   message_ptr_3.message_header.message_set = MS_MN_CC;
   message_ptr_3.message_header.message_id = MNCC_SETUP_COMPL_RES;
   PUT_IMH_LEN((sizeof(MNCC_SETUP_COMPL_RES_T) - sizeof(IMH_T)),&message_ptr_3.message_header)
       /*  SET THE CONNECTION ID  */
   message_ptr_3.connection_id = connection_id;

#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
   {
     message_ptr_3.as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
   }
#endif


   /*  SEND MESSAGE  */
   func_return_val = MN_send_message(GS_QUEUE_MN_CM, &message_ptr_3, TRUE);

   if(func_return_val != GS_SUCCESS)
   {
      MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Send MNCC_SETUP_COMPL_RES message error");
   }
}


/*
 *   Function name : MN_put_MNCC_SETUP_RES
 *   ---------------
 *
 *   Description:
 *   ------------
 *  This function generates the MNCC_SETUP_RES message to the 
 *   CM-CC.
 *  
 *  
 *
 *  Parameters:
 *  -----------
 *  IN connection_id
 *     connected_subaddress_ptr
 *    
 *
 *  Returns:
 *
 *
 */
VOID_FUNC MN_put_MNCC_SETUP_RES(
connection_id_T            connection_id, 
cm_connected_subaddress_T *connected_subadd_ptr)
{

   /* locals */
   gs_status_T       func_return_val;
   MNCC_SETUP_RES_T  message_ptr_4;
   int               counter = 0;
   int               message_length;
   int               ie_length;
   int               i;


   message_ptr_4.message_header.message_set = MS_MN_CC;
   message_ptr_4.message_header.message_id = MNCC_SETUP_RES;

   /*  SET THE CONNECTION ID  */

   message_ptr_4.connection_id = connection_id;
   message_length = sizeof(connection_id) + sizeof(sys_modem_as_id_e_type);

#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
   {
     message_ptr_4.as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
   }
#endif

   /*  SET THE SUBADDRESS  */
   if(connected_subadd_ptr->present == TRUE )
   {
      message_ptr_4.data[counter++] = IEI_CONNECTED_SUBADDRESS;
      message_length++;

      if(connected_subadd_ptr->length > sizeof(connected_subadd_ptr->data))
      {
         connected_subadd_ptr->length = sizeof(connected_subadd_ptr->data);
         MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: connected_subadd_ptr->length overflow , truncating \n ");
         
      }
      ie_length = connected_subadd_ptr->length;
      message_ptr_4.data[counter++] = (byte)ie_length;
      message_length++;

      for(i=0; i< ie_length; i++, message_length++,counter++)
      {
         message_ptr_4.data[counter] = connected_subadd_ptr->data[i];
      }
   }

   PUT_IMH_LEN(message_length,&message_ptr_4.message_header)


       /*  SEND THE MESSAGE  */
   func_return_val = MN_send_message(GS_QUEUE_MN_CM, &message_ptr_4, TRUE);

   if(func_return_val != GS_SUCCESS)
   {
      MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Send MN_put_MNCC_SETUP_RES message error");
   }

}

/*
 *   Function name : MN_put_MNCC_SETUP_REQ
 *   ---------------
 *
 *   Description:
 *   ------------
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
VOID_FUNC MN_put_MNCC_SETUP_REQ(connection_id_T connection_id)
{
   MNCC_SETUP_REQ_T        *message_ptr;
   MNCC_SETUP_REQ_T        temp_message;
   mn_call_information_T   *call_data_ptr;
   int                     data_index;
   int                     ie_length;
   int                     index;
   int                     message_length;
   gs_status_T             message_send_status;
   byte                    *data_ptr; /* pointer inside message_ptr->data array*/
   byte                    *last_octet_data_ptr; /* pointer to last octet of the data buffer*/
   byte                    *length_ptr1;
   byte                    *length_ptr2;
   byte                    *length_ptr3;
   byte                    invoke_id;
   /* byte                    ss_op;  */ /* un-useful register - AB */
   unsigned long int       cug_index;
   byte                    length;
   byte                 local_mscr = 0;


   if(connection_id <= MAX_MO_CONNECTION_ID)
   {
      message_ptr = &temp_message;

      call_data_ptr =  mn_call_information_p[connection_id];

      message_ptr->message_header.message_set = MS_MN_CC;
      message_ptr->message_header.message_id = MNCC_SETUP_REQ;
      message_length = sizeof(connection_id_T) + sizeof(sys_modem_as_id_e_type);


#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
      {
        message_ptr->as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
      }
#endif


      /*  SET CONNECTION ID  */

      message_ptr->connection_id = connection_id;


      /*  LOAD CNM SERVICE TYPE DATA  */

      message_ptr->CNM_service_type = call_data_ptr->CNM_service_type;

      message_length ++;
      data_index = 0;

      /*  ONE BEARER CAPABILITIES  */

      if(call_data_ptr->repeat_indicator_bc == 1)
      {
         data_ptr = &message_ptr->data[data_index];
         last_octet_data_ptr = &message_ptr->data[sizeof(message_ptr->data) -1];
         index = MN_put_bearer_capability(data_ptr, 
             &call_data_ptr->bearer_capability_1,
             last_octet_data_ptr);

         data_index += index;
         message_length += index;
      }

      /*  TWO BEARER CAPABILITIES  */

      else if((call_data_ptr->repeat_indicator_bc == REPEAT_SUPPORTS_FALLBACK) ||
          (call_data_ptr->repeat_indicator_bc == REPEAT_SERVICE_CHANGE_FALLBACK))
      {
        
         message_ptr->data[data_index++] = IEI_REPEAT_INDICATOR | call_data_ptr->repeat_indicator_bc;
   
         message_length++;

         /* BCIE 1 */
         data_ptr = &message_ptr->data[data_index];
         last_octet_data_ptr = &message_ptr->data[sizeof(message_ptr->data) -1];
         index = MN_put_bearer_capability(data_ptr, &call_data_ptr->bearer_capability_1, last_octet_data_ptr);
         data_index += index; 
         message_length += index;

         /* BCIE 2 */
         data_ptr = &message_ptr->data[data_index];
         last_octet_data_ptr = &message_ptr->data[sizeof(message_ptr->data) -1];
         index = MN_put_bearer_capability(data_ptr, &call_data_ptr->bearer_capability_2, last_octet_data_ptr);
         data_index += index;
         message_length += index;
      }

      /*  FACILITY */
      if (call_data_ptr->cm_forward_cug_info.present == TRUE)
      {
         /* Place the data in to the message */

         message_ptr->data[data_index++] = IEI_FACILITY;  /* Facility IEI */
         length_ptr1 = &(message_ptr->data[data_index++]); /* Length of facility contents */
         index = 0;

         /* Put the component data in to the message; facility information */

         message_ptr->data[data_index++] = INVOKE; /* Component type tag */
         index++;
         length_ptr2 = &(message_ptr->data[data_index++]); /* Component length */
         index++;
         message_ptr->data[data_index++] = INVOKE_ID_TAG;   /* Invoke id tag */
         index++;
         message_ptr->data[data_index++] = 0x01;   /* Invoke id length */
         index++;

         /* Assign an invoke id */

         invoke_id = MN_request_invoke_id(forwardCUG_Info);

         message_ptr->data[data_index++] = invoke_id;

         /* Release the invoke_id, as there is never a reply from network */

         (void)MN_retrieve_ss_operation(invoke_id,
                                  TRUE); /* reset invoke_id */

         index++;
         message_ptr->data[data_index++] = OP_CODE_TAG;   /* op-code tag */
         index++;
         message_ptr->data[data_index++] = 0x01;   /* op-code length */
         index++;
         message_ptr->data[data_index++] = forwardCUG_Info;  /* op-code */
         index++;
         message_ptr->data[data_index++] = 0x30;   /* Sequence tag */
         index++;
         length_ptr3 = &(message_ptr->data[data_index++]);
         index++;

         /* Get optional data elements */

         if(call_data_ptr->cm_forward_cug_info.cm_cug_index.present == TRUE)
         {

            /* Copy cugIndex info if it exists */

            message_ptr->data[data_index++] = 0x80;   /* cug index id */
            index++;

            /*
             * get value of cug index ( 0..32767 )
             */

            cug_index = 
               call_data_ptr->cm_forward_cug_info.cm_cug_index.msb * 256 + 
               call_data_ptr->cm_forward_cug_info.cm_cug_index.lsb;

            /*
             * encode it as an asn.1 INTEGER
             */

            length = EncodeAsnInt( &message_ptr->data[data_index], cug_index );

            data_index = data_index + length + 1;
            index = index + length + 1;
         }

         if(call_data_ptr->cm_forward_cug_info.cm_suppress_pref_cug ==
             TRUE)
         {
            message_ptr->data[data_index++] = 0x81;   /* suppressCug id */
            index++;
            message_ptr->data[data_index++] = 0x00;
            index++;
         }

         if(call_data_ptr->cm_forward_cug_info.cm_suppress_outgoing_access ==
             TRUE)
         {
            message_ptr->data[data_index++] = 0x82;   /* suppressOA id */
            index++;
            message_ptr->data[data_index++] = 0x00;
            index++;
         }

         *length_ptr1 = (byte)index;
         *length_ptr2 = (byte)(index - 2);
         *length_ptr3 = (byte)(index - 10);

         message_length += (index + 2);    /* index + facility IE & length */
      }


#ifdef FEATURE_UUS
      /* Explicitly activate UUS */

      else if(call_data_ptr->user_user_data.type > (byte)UUS1_IMPLICIT)   /* Explicitly Activated */
      {
         byte uUS, uUSRequired, new_invoke_id = MN_request_invoke_id(userUserService);

         /* Start the timer */
         MN_start_ss_timer(connection_id, new_invoke_id, 40000); /* T303 = 30 + SS = 10 sec */

         /* Place the data in to the message */
         message_ptr->data[data_index++] = IEI_FACILITY;  /* Facility IEI */

         message_ptr->data[data_index++] = 16; /* Length of facility contents */

         message_ptr->data[data_index++] = INVOKE; /* Component type tag */

         message_ptr->data[data_index++] = 14; /* Component length */

         message_ptr->data[data_index++] = INVOKE_ID_TAG;   /* Invoke id tag */

         message_ptr->data[data_index++] = 0x01;   /* Invoke id length */

         message_ptr->data[data_index++] = new_invoke_id;

         message_ptr->data[data_index++] = OP_CODE_TAG;   /* op-code tag */

         message_ptr->data[data_index++] = 0x01;   /* op-code length */

         message_ptr->data[data_index++] = userUserService;  /* op-code */

         message_ptr->data[data_index++] = 0x30;   /* userUserService Sequence tag */

         message_ptr->data[data_index++] = 0x06; /* userUserService Sequence length */

         message_ptr->data[data_index++] = 0x80; /* uUS-Service Enumerated tag */

         message_ptr->data[data_index++] = 0x01; /* uUS-Service Enumerated length */

         if(call_data_ptr->user_user_data.type == (byte)UUS1_REQUIRED)
         {
             uUS = 1; 
             uUSRequired = TRUE;
         }
         else if(call_data_ptr->user_user_data.type == (byte)UUS1_NOT_REQUIRED)
         {
             uUS = 1; 
             uUSRequired = FALSE;
         }
         else if(call_data_ptr->user_user_data.type == (byte)UUS2_REQUIRED)
         {
             uUS = 2; 
             uUSRequired = TRUE;
         }
         else if(call_data_ptr->user_user_data.type == (byte)UUS2_NOT_REQUIRED)
         {
             uUS = 2; 
             uUSRequired = FALSE;
         }
         else if(call_data_ptr->user_user_data.type == (byte)UUS3_REQUIRED)
         {
             uUS = 3; 
             uUSRequired = TRUE;
         }
         else if (call_data_ptr->user_user_data.type == (byte)UUS3_NOT_REQUIRED)
         {
             uUS = 3; 
             uUSRequired = FALSE;
         }
         else
         {
             uUS = 1;
               uUSRequired = FALSE;

               MSG_ERROR_DS_2(MN_SUB,"=MNCNM= Invalid type=%d, conn id=%d", call_data_ptr->user_user_data.type, connection_id);
         }

         message_ptr->data[data_index++] = uUS; /* uUS-Service */
       
         message_ptr->data[data_index++] = 0x81; /* uUS-Required tag */

         message_ptr->data[data_index++] = 0x01; /* uUS-Required tag */

         message_ptr->data[data_index++] = uUSRequired; /* uUS-Required value */

         message_length += 18;

      }
#endif

      /*  SET UP CALLING PARTY SUBADDRESS DATA  */

      if(call_data_ptr->calling_party_subaddress_present == TRUE)
      {
         message_ptr->data[data_index++] = IEI_CALLING_PARTY_SUBAD;
         message_length++;
         if(call_data_ptr->calling_party_subaddress.length > sizeof(call_data_ptr->calling_party_subaddress.data))
         {
              call_data_ptr->calling_party_subaddress.length = sizeof(call_data_ptr->calling_party_subaddress.data);
              MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Error:call_data_ptr->calling_party_subaddress.length overflow , truncating \n ");
         }
            
         ie_length = (int)call_data_ptr->calling_party_subaddress.length;
         message_ptr->data[data_index++] = (byte)ie_length;
         message_length++;

         for(index = 0; index < ie_length;data_index++,index++)
         {
            message_ptr->data[data_index] = 
                call_data_ptr->calling_party_subaddress.data[index];
         }
         message_length += ie_length;
      }

      /*  SET UP THE CALLED PARTY BCD NUMBER DATA  */

      if ((call_data_ptr->CNM_service_type == MO_CALL_ESTABLISHMENT_HALF_SPEECH)||
          (call_data_ptr->CNM_service_type == MO_CALL_ESTABLISHMENT_FULL_SPEECH)||
          (call_data_ptr->CNM_service_type == MO_CALL_ESTABLISHMENT_DATA_4800)||
          (call_data_ptr->CNM_service_type == MO_CALL_ESTABLISHMENT_DATA_9600))
      {
         message_ptr->data[data_index++] = IEI_CALLED_PARTY_BCD_NO;
         message_length++;
         if(call_data_ptr->called_party_bcd_number.length > sizeof(call_data_ptr->called_party_bcd_number.data))
         {
              call_data_ptr->called_party_bcd_number.length = sizeof(call_data_ptr->called_party_bcd_number.data);
              MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Error: call_data_ptr->called_party_bcd_number.length overflow , truncating \n ");
         }
         ie_length = (int)call_data_ptr->called_party_bcd_number.length;
         message_ptr->data[data_index++] = (byte)ie_length;
         message_length++;

         for(index=0;index < ie_length; index++ ,data_index++)
         {
            message_ptr->data[data_index] =
                call_data_ptr->called_party_bcd_number.data[index];
         }
         message_length += ie_length;
      }

      /*  SET UP THE CALLED PARTY SUBADDRESS DATA  */

      if(call_data_ptr->called_party_subaddress_present == TRUE)
      {
         message_ptr->data[data_index++] = IEI_CALLED_PARTY_SUBAD;
         message_length++;
          if(call_data_ptr->called_party_subaddress.length > sizeof(call_data_ptr->called_party_subaddress.data))
         {
              call_data_ptr->called_party_subaddress.length = sizeof(call_data_ptr->called_party_subaddress.data);
              MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Error: call_data_ptr->called_party_bcd_number.length overflow , truncating \n ");
         }
         ie_length = (int)call_data_ptr->called_party_subaddress.length;
         message_ptr->data[data_index++] = (byte)ie_length;
         message_length++;
         for(index=0; index < ie_length;index++,data_index ++)
         {
            message_ptr->data[data_index] = 
                call_data_ptr->called_party_subaddress.data[index];
         }
         message_length += ie_length;
      }

      /*  SET UP LOW LAYER COMPATIBILITY DATA  */
      /*  FOR ONE LOW LAYER COMPATIBILITY      */

      if(call_data_ptr->repeat_indication_llc == 1)
      {
         data_ptr = &message_ptr->data[data_index];
         last_octet_data_ptr = &message_ptr->data[sizeof(message_ptr->data) -1];
         index = MN_put_llc_data(data_ptr, 
             &call_data_ptr->low_layer_compatibility_1,
             last_octet_data_ptr);

         data_index += index;
         message_length += index;
      }

      /*  FOR TWO LOW LAYER COMPATIBILITIES  */

      else if ((call_data_ptr->repeat_indication_llc == REPEAT_SUPPORTS_FALLBACK) ||
          (call_data_ptr->repeat_indication_llc == REPEAT_SERVICE_CHANGE_FALLBACK))
      {
         if(call_data_ptr->repeat_indication_llc == REPEAT_SUPPORTS_FALLBACK)
         {
            message_ptr->data[data_index++] = IEI_REPEAT_INDICATOR | REPEAT_SUPPORTS_FALLBACK;
         }

         if(call_data_ptr->repeat_indication_llc == REPEAT_SERVICE_CHANGE_FALLBACK)
         {
            message_ptr->data[data_index++] = IEI_REPEAT_INDICATOR | REPEAT_SERVICE_CHANGE_FALLBACK;
         }

         message_length++;

         data_ptr = &message_ptr->data[data_index];
         last_octet_data_ptr = &message_ptr->data[sizeof(message_ptr->data) -1];

         index = MN_put_llc_data(data_ptr, 
             &call_data_ptr->low_layer_compatibility_1,
             last_octet_data_ptr);

         data_index += index;
         message_length += index;

         data_ptr = &message_ptr->data[data_index];
         last_octet_data_ptr = &message_ptr->data[sizeof(message_ptr->data) -1];
         index = MN_put_llc_data(data_ptr, 
             &call_data_ptr->low_layer_compatibility_2,
             last_octet_data_ptr);

         data_index += index;
         message_length += index;
      }

      /*  SET UP HIGH LAYER COMPATIBILITY DATA  */
      /*  FOR ONE HIGH LAYER COMPATIBILITY      */

      if(call_data_ptr->repeat_indication_hlc == 1)
      {
         data_ptr = &message_ptr->data[data_index];
         last_octet_data_ptr = &message_ptr->data[sizeof(message_ptr->data) -1];
         index = MN_put_hlc_data(data_ptr, 
             &call_data_ptr->high_layer_compatibility_1,
             last_octet_data_ptr);

         data_index += index;
         message_length += index;
      }

      /*  FOR TWO HIGH LAYER COMPATIBILITIES  */

      else if ((call_data_ptr->repeat_indication_hlc == REPEAT_SERVICE_CHANGE_FALLBACK) ||
          (call_data_ptr->repeat_indication_hlc == REPEAT_SUPPORTS_FALLBACK))
      {
         if(call_data_ptr->repeat_indication_hlc == REPEAT_SUPPORTS_FALLBACK)
         {
            message_ptr->data[data_index++] = IEI_REPEAT_INDICATOR | REPEAT_SUPPORTS_FALLBACK;
         }

         if(call_data_ptr->repeat_indication_hlc == REPEAT_SERVICE_CHANGE_FALLBACK)
         {
            message_ptr->data[data_index++] = IEI_REPEAT_INDICATOR | REPEAT_SERVICE_CHANGE_FALLBACK;
         }

         message_length++;

         data_ptr = &message_ptr->data[data_index];
         last_octet_data_ptr = &message_ptr->data[sizeof(message_ptr->data) -1];

         index = MN_put_hlc_data(data_ptr, 
             &call_data_ptr->high_layer_compatibility_1,
             last_octet_data_ptr);

         data_index += index;
         message_length += index;

         data_ptr = &message_ptr->data[data_index];
         last_octet_data_ptr = &message_ptr->data[sizeof(message_ptr->data) -1];
         index = MN_put_hlc_data(data_ptr, 
             &call_data_ptr->high_layer_compatibility_2,
             last_octet_data_ptr);

         data_index += index;
         message_length += index;
      }

#ifdef FEATURE_UUS
      /*  SET UP THE USER USER DATA  */

      if( (call_data_ptr->user_user_data.present) &&
          (call_data_ptr->user_user_data.type <= (byte)UUS1_IMPLICIT) )
      {
         cm_mo_user_data_req_T  *user_data;
         byte   ie_user_data_length;         

         user_data = MN_get_user_data(connection_id);

         if(user_data)
         {
            ie_user_data_length =  user_data->data_length + 1;  /* including protocol discriminator */        
                /* To ensure that memory overflow doesn't occur */
            if((byte)(ie_user_data_length - 1) > MIN((sizeof(user_data->data)),(sizeof(message_ptr->data) - (data_index + 3))))
            {
               MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Index out of bound and buffer overflow");
            }
            else
            {
               message_ptr->data[data_index++] = IEI_USER_USER;
    
               message_length++;
    
               message_ptr->data[data_index++] = ie_user_data_length; 
    
               message_length++;
    
               message_ptr->data[data_index++] = user_data->protocol_discriminator;
    
               message_length++;
    
               memscpy(&message_ptr->data[data_index], sizeof(message_ptr->data)-data_index ,user_data->data, ie_user_data_length -1);  /* user-user data length */
    
               data_index += ie_user_data_length -1;
    
               message_length +=   ie_user_data_length -1;
            }

            /* release the buffer */
            MN_free_user_data(user_data);  

         } /* user_data */

      } /* user_user_data.present */
#endif

      /*  SET UP CLIR REQUIREMENT  */

      if(call_data_ptr->clir_suppression == TRUE)
      {
         message_ptr->data[data_index++] = IEI_CLIR_SUPPRESSION;
         message_length++;
      }

      else if(call_data_ptr->clir_invocation == TRUE)
      {
         message_ptr->data[data_index++] = IEI_CLIR_INVOCATION;
         message_length++;
      }

      /* SET UP CC CAPABLITY IE */

      if(call_data_ptr->CNM_service_type != EMERGENCY_CALL_ESTABLISHMENT)
      {
          /* add cc capabilities */
          message_ptr->data[data_index++] = IEI_CALL_CONTROL_CAPABIL;
          message_ptr->data[data_index++] = 0x02;       /* length */

#if defined(FEATURE_MULTIMEDIA_FALLBACK) || defined(FEATURE_REL6)
          message_ptr->data[data_index++] = 0x05;       /* DTMF supported, enhanced network-initiated in-call modification supported*/
#else
          message_ptr->data[data_index++] = 0x01;      
#endif

          message_ptr->data[data_index++] = 0x00;       /* #Speech Bearer supported */

          message_length += 4;
      }

#if defined(FEATURE_MULTIMEDIA_FALLBACK) || defined(FEATURE_REL6)
      /* STREAM IDENTIFIER */     

      if(call_data_ptr->stream_id.present == TRUE)
      {
          message_ptr->data[data_index++] = IEI_STREAM_IDENTIFIER;
          message_ptr->data[data_index++] = 0x01;       /* length */
          message_ptr->data[data_index++] = call_data_ptr->stream_id.id;
          
          message_length += 3;
      }
      
#endif /* FEATURE_MULTIMEDIA_FALLBACK */      

MSG_HIGH_DS_1(MN_SUB,"=MNCNM= =MM= als_itc_aux_speech while forming SETUP : %d\n ",mn_get_als_itc_aux_speech());


#ifdef FEATURE_DUAL_SIM
      if(MNCNM_IS_SINGLE_STACK)
      {
        if((mn_as_id > SYS_MODEM_AS_ID_NONE) && (mn_as_id < MAX_AS_IDS) )
        {
          local_mscr = mm_get_mscr_system_information(mn_as_id);
        }
      }
#ifdef FEATURE_DUAL_ACTIVE
      else
      {
        local_mscr = mm_get_mscr_system_information((sys_modem_as_id_e_type)mn_dsda_as_id);
      }
#endif
#else
      {
        local_mscr = mm_get_mscr_system_information(SYS_MODEM_AS_ID_1);
      }
#endif


#ifdef FEATURE_REL5
      if( ( (mm_nv_force_mscr_r99 == NV_FORCE_UE_REL99) ||
            ( (mm_nv_force_mscr_r99 == NV_FORCE_UE_DYNAMIC) && 
             (local_mscr ))) &&             
          (mm_nv_nas_rel_compliance >= NV_NAS_COMPLIANCE_REL5) ) 
      {

        if (((call_data_ptr->bearer_capability_1.present == TRUE) && 
             ((call_data_ptr->bearer_capability_1.information_transfer_capability == ITC_SPEECH) ||
              (call_data_ptr->bearer_capability_1.information_transfer_capability == ITC_AUX_SPEECH_4) ||
              (call_data_ptr->bearer_capability_1.information_transfer_capability == ITC_AUX_SPEECH_6))) ||
            ((call_data_ptr->bearer_capability_2.present == TRUE) && 
             ((call_data_ptr->bearer_capability_2.information_transfer_capability == ITC_SPEECH) ||
              (call_data_ptr->bearer_capability_2.information_transfer_capability == ITC_AUX_SPEECH_4) ||
              (call_data_ptr->bearer_capability_2.information_transfer_capability == ITC_AUX_SPEECH_6))))
        {

          sys_modem_as_id_e_type local_as_id = 0;
#ifdef FEATURE_DUAL_SIM
#ifdef FEATURE_DUAL_ACTIVE
          local_as_id = (sys_modem_as_id_e_type)(sys_modem_as_id_e_type)mn_dsda_as_id;
#else
          local_as_id = mn_as_id;
#endif
#endif
             /* SUPPORTED CODEC LIST */     
/*                                  -----  BITMAP  ------                                  */          
/* ----------------------------------------------------------*/                    
/* |  08     |   07   | 06       | 05       |  04   | 03        | 02       | 01     | */ 
/* ----------------------------------------------------------*/          
/* | TDMA | UMTS | UMTS  | HR       | FR    | GSM     | GSM    |  GSM | */          
/* |  EFR   | AMR2 | AMR    | AMR    | AMR  | EFR      |  HR      |  FR    | */          
/* -----------------------------------------------------------*/                    
/* |  16     | 15     | 14       | 13       |  12   |   11      |   10      | 09     |  */ 
/* -----------------------------------------------------------*/                    
/* | RESE  | RESE | OHR  A| OFR A  | OHR  |UMTS    |    FR     | PDC  |  */          
/* | RVED | RVED |MR-WB | MR-WB| AMR |AMR-WB|AMR-WB|  EFR  | */          
/* -----------------------------------------------------------*/                    

          /* IEI_SUPPORTED_CODECS_LIST */
          message_ptr->data[data_index++] = IEI_SUPPORTED_CODECS_LIST; 
          message_ptr->data[data_index++] = 0x08; /* IEI length */
          /* SysID = UMTS */ 
          message_ptr->data[data_index++] = SYSID_UMTS; 
          message_ptr->data[data_index++] = 0x02; /* bitmap length */

          /* If RAT is TDS, then don't send AMR2 in Supp Codec List */

          if(is_cs_tdscdma_mode(local_as_id)) /* Single SIM mode */
          {
            message_ptr->data[data_index++] = 0x20; /* bitmap 8...1 */
          }
          else
          {
            message_ptr->data[data_index++] = 0x60; /* bitmap 8...1 */
          }

#ifdef FEATURE_VOC_AMR_WB
          if( (gsm_wcdma_amr_codec_preference & 0x08) )
          {
            message_ptr->data[data_index++] = 0x04; /* bitmap 16...9 */
          }
          else
          {
            message_ptr->data[data_index++] = 0x00; /* bitmap 16...9 */
          }
#else
          message_ptr->data[data_index++] = 0x00; /* bitmap 16...9 */
#endif  /*end of FEATURE_VOC_AMR_WB */
          /* SysID = GSM */
          message_ptr->data[data_index++] = SYSID_GSM;
          message_ptr->data[data_index++] = 0x02; /* bitmap length */

/* GSM AMR codecs should be included conditionally depending on the 
    NV Item - NV_GSM_AMR_CALL_CONFIG_I */          
          
          message_ptr->data[data_index] = 0x07; /* bitmap 8...1 */
          if( (gsm_wcdma_amr_codec_preference & 0x01) )
          {
            /*GSM AMR NB is supported*/
            message_ptr->data[data_index] |= 0x08; /* bitmap 8...1 */
          }
          if( (gsm_wcdma_amr_codec_preference & 0x04) )
          {
            /*GSM AMR NB is supported*/
            message_ptr->data[data_index] |= 0x10; /* bitmap 8...1 */
          }
          data_index++;
#ifdef FEATURE_GSM_AMR_WB            
          if( (gsm_wcdma_amr_codec_preference & 0x02) )
          {
            /*GSM AMR WB is supported*/
            message_ptr->data[data_index++] = 0x02; /* bitmap 16...9 */
          }
          else
          {
            /*GSM AMR WB is NOT supported*/
            message_ptr->data[data_index++] = 0x00; /* bitmap 16...9 */           
          }
#else
          /*GSM AMR WB is NOT supported*/
          message_ptr->data[data_index++] = 0x00; /* bitmap 16...9 */
#endif
          message_length += 10;
        }
      }
#endif /* FEATURE_REL5 */

      /* EMERGENCY CATEGORY */
      if ((call_data_ptr->emergency_category.present == TRUE)
#ifdef FEATURE_TDSCDMA
          && ((!is_tdscdma_mode()) || (is_tdscdma_mode()))
#endif /* FEATURE_TDSCDMA */
         )
      {
          /* service category */
          message_ptr->data[data_index++] = IEI_SERVICE_CATEGORY;
          message_ptr->data[data_index++] = 0x01;       /* length */
          message_ptr->data[data_index++] = call_data_ptr->emergency_category.emergency_category  & EMERGENCY_CATEGORY_MASK;  /* 0x7F */;

          message_length += 3;
      }


      /*  SET MESSAGE LENGTH  */

      PUT_IMH_LEN(message_length,&(message_ptr->message_header))

      /*  SEND MESSAGE  */

      message_send_status = MN_send_message(GS_QUEUE_MN_CM, message_ptr, TRUE);

      /*  CHECK THAT MESSAGE SENT OK  */

      if(message_send_status != GS_SUCCESS)
      {
         MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Send MNCC_SETUP_REQ message error");
        
      }
      else   /* Successfully sent MNCC_SETUP_REQ, set mo_setup_pending to TRUE  */
      {
         call_data_ptr->mo_setup_pending = TRUE;
#ifdef FEATURE_WCDMA
         mn_store_rab_status();
#endif
      }
   }
   else
   {
      MSG_ERROR_DS_1(MN_SUB,"=MNCNM= Invalid MO connection_id = %d",connection_id);
   }

}


/*
 * Function name :  MN_put_MNCC_START_DTMF_REQ
 * -------------
 *
 * Description :  Creates the above named message
 * -----------    message and sends to the GS_QUEUE_MN
 *                queue.
 *
 * Uses :       
 * ----
 *
 * Parameters:  connection_id,
 * ----------   keypad_information
 *
 * Returns:     None
 * --------
 */

VOID_FUNC MN_put_MNCC_START_DTMF_REQ(connection_id_T connection_id, 
byte keypad_information)
{

   /* Locals */

   gs_status_T func_return_val;
   int         message_length;

   MNCC_START_DTMF_REQ_T   message_ptr;

   message_ptr.message_header.message_set = MS_MN_CC;

   message_ptr.message_header.message_id = MNCC_START_DTMF_REQ;

#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
   {
     message_ptr.as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
   }
#endif

   message_length = sizeof(MNCC_START_DTMF_REQ_T) - sizeof(IMH_T);

   PUT_IMH_LEN(message_length,&message_ptr.message_header)

   /*  SET THE CONNECTION ID  */

   message_ptr.connection_id = connection_id;

   /*  SET THE DTMF DATA  */

   message_ptr.keypad_facility.Iei = IEI_KEYPAD_FACILITY;
   message_ptr.keypad_facility.IA5Char = keypad_information;

   /*  SEND THE MESSAGE  */

   func_return_val = MN_send_message(GS_QUEUE_MN_CM, &message_ptr, TRUE);

   if(func_return_val != GS_SUCCESS)
   {
      MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Send MNCC_START_DTMF_REQ message error");
   }
}


/*
 * Function name :  MN_put_MNCC_STOP_DTMF_REQ
 * -------------
 *
 * Description :  Creates the above named message
 * -----------    message and sends to the GS_QUEUE_MN
 *                queue.
 *
 * Uses :       
 * ----
 *
 * Parameters:  connection_id
 * ----------   
 *
 * Returns:
 * --------
 */
VOID_FUNC MN_put_MNCC_STOP_DTMF_REQ(connection_id_T connection_id)
{

   /* Locals */
   gs_status_T func_return_val;

   MNCC_STOP_DTMF_REQ_T message_ptr_2;


   message_ptr_2.message_header.message_set = MS_MN_CC;

   message_ptr_2.message_header.message_id = MNCC_STOP_DTMF_REQ;

   PUT_IMH_LEN((sizeof(MNCC_STOP_DTMF_REQ_T) - sizeof(IMH_T)),&message_ptr_2.message_header)

   /*  SET THE CONNECTION ID  */

   message_ptr_2.connection_id = connection_id;

#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
   {
     message_ptr_2.as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
   }
#endif


   /*  SEND MESSAGE  */

   func_return_val = MN_send_message(GS_QUEUE_MN_CM, &message_ptr_2, TRUE);


   if(func_return_val != GS_SUCCESS)
   {
      MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Send MNCC_STOP_DTMF_REQ message error");
   
   }
}


#ifdef FEATURE_UUS

/*
 * Function name :  MN_put_MNCC_USER_USER_DATA_REQ
 * -------------
 *
 * Description :  Creates the above named message
 * -----------    message and sends to the GS_QUEUE_MN
 *                queue.
 *
 * Uses :       
 * ----
 *
 * Parameters:  connection_id,
 * ----------   keypad_information
 *
 * Returns:
 * --------
 *
 */

VOID_FUNC MN_put_MNCC_USER_USER_DATA_REQ( connection_id_T connection_id,
                                          ie_user_user_T  *ie_user_user_ptr,
                                          boolean         more_data)
{
   gs_status_T                func_return_val;
   MNCC_USER_USER_DATA_REQ_T  message_ptr_3;
   int                        ie_length;
   int                        index;
   int                        data_index;
   int                        message_length;
   int                        user_data_length;

   data_index = 0;

   if(ie_user_user_ptr->Length > MAX_USER_USER_DATA_LENGTH)
   {
      MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: User to user data exceeds maximum");
   }
   else
   {

      message_ptr_3.message_header.message_set = MS_MN_CC;
      message_ptr_3.message_header.message_id = MNCC_USER_USER_DATA_REQ;
      message_length = sizeof(connection_id_T) + sizeof(sys_modem_as_id_e_type);

#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
      {
        message_ptr_3.as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
      }
#endif

      /*  SET THE CONNECTION ID  */

      message_ptr_3.connection_id = connection_id;

      /*  SET UP THE USER USER DATA  */

      message_ptr_3.data[data_index++] = IEI_USER_USER;

      message_length++;

      ie_length = (int)ie_user_user_ptr->Length;

      message_ptr_3.data[data_index++] = (byte)ie_length;

      message_length++;

      message_ptr_3.data[data_index++] = 
          ie_user_user_ptr->UuProtocolDescriminator;

      message_length++;

      user_data_length = ie_length - 1;

      for(index=0; index < user_data_length;index++, data_index++)
      {

         message_ptr_3.data[data_index] = 
             ie_user_user_ptr->UserUserInformation[index];

      }

      message_length += user_data_length;

      if(more_data == TRUE)
      {
         message_ptr_3.data[data_index++] = IEI_MORE_DATA;

         message_length += (int)sizeof(byte);
      }

      /* Set the message_length */

      PUT_IMH_LEN(message_length,&message_ptr_3.message_header)

      /*  SEND THE MESSAGE  */

      func_return_val = MN_send_message(GS_QUEUE_MN_CM, 
                                        &message_ptr_3, 
                                        TRUE);

      if(func_return_val != GS_SUCCESS)
      {
         MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Send MNCC_USER_USER_DATA_REQ message error");
      }
   }
}

#endif

