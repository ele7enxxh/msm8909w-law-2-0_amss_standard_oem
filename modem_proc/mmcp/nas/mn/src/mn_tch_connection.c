/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2001 Qualcomm Technologies, Incorporated and its licensors.  All Rights 
Reserved.  QUALCOMM Proprietary.  Export of this technology or software 
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mn/vcs/mn_tch_connection.c_v   1.7   10 Jul 2002 14:10:56   cdealy  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mn/src/mn_tch_connection.c#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
3/20/01     CD     Replaced old header with QCOM header
                   Removed #ifdef ASW_SIMBA

5/10/01     CD     Removed unnecessary header file includes

6/4/01      CD     Removed cause.h and ms.h since they were not being used

7/18/01     CD     Removed duplicate external declaration of tch_user_connections_store

                   Included mn_cnmtask.h and deleted external declaration of is_gsm_mode() and 
                   is_umts_mode()

8/10/01     CD     Removed unnecessary queue.h

08/16/01    AB     Renamed MMI references to CM.

23-Jan-2002  VT    In MN_connect_TCH() and MN_disconnect_TCH(), added MSG_HIGHs 
                     when we actually switch ON/OFF the vocoder/data codec.

06/03/02     CD    Merged changes from revision 1.5.1.0
                   In MN_disconnect_TCH(), added a parameter of 
                   type mn_vocoder_ctrl_e_type. Now, if speech_channel_connected
           is TRUE in tch_user_connections_store[], it is changed to 
           FALSE only if it is not an In-call Modification from one
           speech version to another.

           In all the calls to MN_disconnect_TCH(), now passing the
               new parameter.

06/03/02     CD    In MN_connect_TCH() removed check for speech/data_channel_connected
                   set to FALSE before connecting the vocoder to account for vocoder 
                   handover cases

                   Added checking of function returns

                   In MN_disconnect_TCH() added check for incorrect Traffic Channel
                   configuration

06/27/02     CD    In MN_disconnect_TCH() changed MSG_ERROR to MSG_HIGH to report 
                   that the vocoder has not been disconnected because the 
                   speech and data channels were not connected

07/22/02     AB    In MN_disconnect_TCH(), change comments to indicated that 
                   TCH had already been disconencted from vocoder. 

10/12/02     AB    Added vocoder reconfiguration for handover supported 
                   between WCDMA and GSM.

10/9/03      AB    Relinguished controls to GSM L1 on GSM 2 GSM handover.

07/02/04     AB    Added vocoder supports for ITC_AUX_SPEECH.

08/03/04     AB    Added lower layer vocoder controls in MN_disconnect_TCH().

03/05/06     NR    Adding changes for WB_AMR

11/02/11    PM    Changes made for a feature where Voc gets released when
                  call is put on HOLD and Voc gets acquired when call is retrieved.
===========================================================================*/

#include "mmcp_variation.h"
#include "customer.h"
#include "rex.h"
#include "environ.h"
#include "timers_v.h"
#include "mn.h"
#include "mnglobal.h"
#include "msg.h"
#include "ghdi_exp.h"
#include "ghdi_exp_int.h"
#include "ds_cap_v.h"
#include "mm.h"

#include "mn_cnmtask.h"  /* for is_gsm_mode/is_umts_mode */
#include "mn_cnmtask_int.h"
#include "rrcmnif.h"
#include "nasutils.h"
#ifdef FEATURE_SEGMENT_LOADING
#include "mcfg_seg_load.h"
#include "ITDSCDMA.h"
#include "IWCDMA.h"
#include "IWCDMA_ext.h"
#endif


#define MM_AS_ID_1 1
extern boolean is_cs_umts_mode( sys_modem_as_id_e_type as_id );
extern boolean is_cs_wcdma_mode( sys_modem_as_id_e_type as_id );
extern boolean is_cs_gsm_mode( sys_modem_as_id_e_type as_id );
#ifdef FEATURE_DUAL_SIM
extern boolean is_cs_umts_mode( sys_modem_as_id_e_type as_id );
extern boolean is_cs_wcdma_mode( sys_modem_as_id_e_type as_id );
extern boolean is_cs_gsm_mode( sys_modem_as_id_e_type as_id );

#define MNCNM_is_umts_mode(x) is_cs_umts_mode(x)
#define MNCNM_is_wcdma_mode(x) is_cs_wcdma_mode(x)
#define MNCNM_is_gsm_mode(x)  is_cs_gsm_mode(x)

#else
extern boolean is_umts_mode(void);
extern boolean is_wcdma_mode(void);
extern boolean is_gsm_mode(void);

#define MNCNM_is_umts_mode(x) is_umts_mode()
#define MNCNM_is_wcdma_mode(x) is_wcdma_mode()
#define MNCNM_is_gsm_mode(x)  is_gsm_mode()

#endif

#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
boolean MN_turned_on_vocoder_sim[MAX_AS_IDS] = {FALSE, FALSE};
#define MN_turned_on_vocoder (MN_turned_on_vocoder_sim[mn_dsda_as_id])
#else
boolean MN_turned_on_vocoder = FALSE;
#endif


#ifdef FEATURE_TDSCDMA
#ifdef FEATURE_INTER_RAT_HO_OPT
#if defined FEATURE_DUAL_SIM
extern boolean tdsrrc_vocoder_control_enabled_sub(sys_modem_as_id_e_type as_id);
#else
extern boolean tdsrrc_vocoder_control_enabled(void);
#endif
#endif /* FEATURE_INTER_RAT_HO_OPT */
#endif /* FEATURE_TDSCDMA */

#ifdef FEATURE_GSM
#if defined FEATURE_DUAL_SIM
extern boolean gl1_msg_is_l1_vocoder_control_enabled_multisim(sys_modem_as_id_e_type as_id);
#else
extern boolean gl1_msg_is_l1_vocoder_control_enabled(void);
#endif
#endif

#ifdef FEATURE_NAS_VOC_REL_AFTER_CALL_HOLD
extern boolean mn_at_least_one_call_in_active_conv_state(void);
#endif

extern void  ghdi_update_mvs_mode(umts_codecs_list_T channel_mode);

/*
 * Function name :  MN_connect_TCH
 * -------------
 *
 * Description :  Connects the TCH voice/data channel provided that:-
 * -----------    a) It is not already connected and 
 *                b) the OTA connection is made
 *
 *                 It also disconnects the data/voice channel if 
 *                 connected.
 *
 *
 *
 * Parameters: information transfer capability
 * ----------  
 *
 *
 *
 * Returns:
 * --------
 *
 */


VOID_FUNC MN_connect_TCH(byte  information_transfer_capability)
{
   sys_radio_access_tech_e_type current_rat;
   sys_modem_as_id_e_type       local_as_id = 0;
#ifdef FEATURE_SEGMENT_LOADING
   interface_t* ptr_W = get_wcdma_interface();
   interface_t* ptr_T = get_tdscdma_interface(); 
#endif

#ifdef FEATURE_DUAL_SIM
   if(MNCNM_IS_SINGLE_STACK)
   {
     local_as_id = mn_as_id;
   }
#ifdef FEATURE_DUAL_ACTIVE
   else
   {
     local_as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
   }
#endif
#endif
  if(local_as_id > SYS_MODEM_AS_ID_NONE && local_as_id < MAX_AS_IDS)
  {
#ifdef FEATURE_GSM
     
  
      /*********************************/
      /*   OTA channel is connected    */
      /*********************************/
  
      /* For both GSM & UMTS, Use DEFAULT_RAB_ID below. Will change when we 
       * implement the Multicall Feature (Not the Multicall SS)  */
  
#if defined FEATURE_DUAL_SIM
      if(gl1_msg_is_l1_vocoder_control_enabled_multisim(local_as_id))	  
#else
      if(gl1_msg_is_l1_vocoder_control_enabled())	  
#endif
     {
         MSG_HIGH_DS_0(MN_SUB,"=MNCNM= GSM L1 is controlling the vocoder");
     }
     else 
#endif /* FEATURE_GSM */
  
#ifdef FEATURE_WCDMA
#ifdef FEATURE_INTER_RAT_HO_OPT
  
#ifdef FEATURE_TDSCDMA
#ifdef FEATURE_SEGMENT_LOADING
#if defined FEATURE_DUAL_SIM
     if (((ptr_W != NULL)&&IWCDMA_rrc_vocoder_control_enabled_sub(ptr_W,local_as_id)) || 
         ((ptr_T!=NULL)&&ITDSCDMA_tdsrrc_vocoder_control_enabled_sub(ptr_T,local_as_id)))
#else
     if (((ptr_W != NULL)&&IWCDMA_rrc_vocoder_control_enabled(ptr_W)) || 
         ((ptr_T!=NULL)&&ITDSCDMA_tdsrrc_vocoder_control_enabled(ptr_T)))
#endif /* FEATURE_DUAL_SIM */

#else /*else FEATURE_SEGMENT_LOADING */
  
#if defined FEATURE_DUAL_SIM
     if (rrc_vocoder_control_enabled_sub(local_as_id) || tdsrrc_vocoder_control_enabled_sub(local_as_id))
#else
     if (rrc_vocoder_control_enabled() || tdsrrc_vocoder_control_enabled())
#endif /* FEATURE_DUAL_SIM */  
#endif /* FEATURE_SEGMENT_LOADING */

#else /* else FEATURE_TDSCDMA */

#ifdef FEATURE_SEGMENT_LOADING
#if defined FEATURE_DUAL_SIM
     if ((ptr_W != NULL)&&IWCDMA_rrc_vocoder_control_enabled_sub(ptr_W,local_as_id))
#else
     if ((ptr_W != NULL)&&IWCDMA_rrc_vocoder_control_enabled(ptr_W))
#endif /* FEATURE_DUAL_SIM */

#else /*else FEATURE_SEGMENT_LOADING */
  
#if defined FEATURE_DUAL_SIM
     if(rrc_vocoder_control_enabled_sub(local_as_id))
#else
     if(rrc_vocoder_control_enabled())
#endif /* FEATURE_DUAL_SIM */  
#endif /* FEATURE_SEGMENT_LOADING */
#endif /* FEATURE_TDSCDMA*/
     {
         MSG_HIGH_DS_0(MN_SUB,"=MNCNM= RRC is controlling the vocoder. Updating latest umts_codec in ghdi_mvs_mode");
#ifdef FEATURE_VOC_AMR_WB
         ghdi_update_mvs_mode(tch_user_connections_store[DEFAULT_RAB_ID].umts_codec);
#else
         ghdi_update_mvs_mode((umts_codecs_list_T)0);
#endif
     }
  
     else
#endif /* FEATURE_INTER_RAT_HO_OPT */
#endif /* FEATURE_WCDMA */
  
  
     if ((tch_user_connections_store[DEFAULT_RAB_ID].OTA_channel_connected == TRUE) && 
         (MN_turned_on_vocoder == FALSE) 
#ifdef FEATURE_NAS_VOC_REL_AFTER_CALL_HOLD       
         &&
         (mn_at_least_one_call_in_active_conv_state() == TRUE)
#endif
        )
     {
  
#ifndef FEATURE_NAS_VOC_REL_AFTER_CALL_HOLD
            MN_turned_on_vocoder = TRUE;  /* flag "ON" to prevent turnning more then 1 */
            
            MSG_HIGH_DS_1(MN_SUB,"=MNCNM= MN turn on the vocoder flag=%d", MN_turned_on_vocoder);
#endif /* #ifndef FEATURE_NAS_VOC_REL_AFTER_CALL_HOLD */
  
        /********************/
        /*  Speech setup    */
        /********************/
  
        MSG_HIGH_DS_1(MN_SUB,"=MNCNM= =MN= als_itc_aux_speech : %d\n ",mn_get_als_itc_aux_speech());
    
        if(
#ifdef FEATURE_ALS
           (information_transfer_capability == ITC_AUX_SPEECH_4) ||
           (information_transfer_capability == ITC_AUX_SPEECH_6) ||
#endif 
           (information_transfer_capability == ITC_SPEECH))
        {
           /*
            * If the data channel is connected,  disconnect it
            */
           if (tch_user_connections_store[DEFAULT_RAB_ID].data_channel_connected == TRUE)
           {
              MN_disconnect_TCH(MN_VOCODER_OFF);
           }
  
           /* now connect the speech channel */
  
           /*************/
           /* GSM MODE  */
           /*************/
           if (  MNCNM_is_gsm_mode(local_as_id) )
           {
              /*********************/
              /*    SUBCHANNEL 0   */
              /*********************/
              if (tch_user_connections_store[DEFAULT_RAB_ID].subchannel == 0)
              {
                 /****************/
                 /* FULL RATE    */
                 /****************/
                 if (tch_user_connections_store[DEFAULT_RAB_ID].tch_rate == TCH_FULL_RATE)
                 {
  
                    if (ghdi_control_subs_tch_gating(GHDI_TURN_SPEECH_TCH_F_ON,
                                         tch_user_connections_store[DEFAULT_RAB_ID].channel_mode,
                                         mn_asid_2_vsid_map[local_as_id],
                                         local_as_id
                                               ) ==  
                                         GHDI_FAILURE)
                    {
                        MSG_HIGH_DS_0(MN_SUB,"=MNCNM= Unable to switch ON FR vocoder");
                    }
                 }
                 /****************/
                 /* HALF RATE    */
                 /****************/
                 else
                 {
                    if (ghdi_control_subs_tch_gating(GHDI_TURN_SPEECH_TCH_H_0_ON,
                                        tch_user_connections_store[DEFAULT_RAB_ID].channel_mode,
                                         mn_asid_2_vsid_map[local_as_id],
                                         local_as_id
                                                ) ==
                                        GHDI_FAILURE)
                    {
                        MSG_HIGH_DS_0(MN_SUB,"=MNCNM= Unable to switch ON HR subchannel 0 vocoder");
                    }
  
                 }
              }
              /*********************/
              /*  SUBCHANNEL  !=0  */
              /*********************/
              else
              {
                 /****************/
                 /* HALF RATE    */
                 /****************/
                 if (ghdi_control_subs_tch_gating(GHDI_TURN_SPEECH_TCH_H_1_ON,
                                     tch_user_connections_store[DEFAULT_RAB_ID].channel_mode,
                                         mn_asid_2_vsid_map[local_as_id],
                                         local_as_id
                                            ) == 
                                     GHDI_FAILURE)
                 {
                     MSG_HIGH_DS_0(MN_SUB,"=MNCNM= Unable to switch ON HR subchannel 1 vocoder");
                 }
  
              }
  
           } /* if is_cs_gsm_mode...*/
           /***************/
           /* WCDMA MODE  */
           /***************/
           else if (MNCNM_is_umts_mode(local_as_id))
           {
              if (MNCNM_is_wcdma_mode(local_as_id))
              {
                current_rat = SYS_RAT_UMTS_RADIO_ACCESS;
              }
              else
              {
                current_rat = SYS_RAT_TDS_RADIO_ACCESS;
              }
  
#ifdef FEATURE_VOC_AMR_WB 
#ifdef FEATURE_CS_VOICE_OVER_HSPA
              if (ghdi_control_umts_voice(GHDI_TURN_AMR_ON,
                    tch_user_connections_store[DEFAULT_RAB_ID].umts_codec,
                    tch_user_connections_store[DEFAULT_RAB_ID].cs_voice_type,
                    current_rat) == GHDI_FAILURE)
              {
                    MSG_HIGH_DS_0(MN_SUB,"=MNCNM= Unable to switch ON AMR Speech vocoder");
              }                
#else
              if (ghdi_control_umts_voice(GHDI_TURN_AMR_ON,
                    tch_user_connections_store[DEFAULT_RAB_ID].umts_codec,
                    current_rat) == GHDI_FAILURE)
              {
                    MSG_HIGH_DS_0(MN_SUB,"=MNCNM= Unable to switch ON AMR Speech vocoder");
              }
#endif            
#else
#ifdef FEATURE_CS_VOICE_OVER_HSPA
              if (ghdi_control_umts_voice(GHDI_TURN_AMR_ON,
                    tch_user_connections_store[DEFAULT_RAB_ID].cs_voice_type,
                    current_rat) == GHDI_FAILURE)
              {
                    MSG_HIGH_DS_0(MN_SUB,"=MNCNM= Unable to switch ON AMR Speech vocoder");
              }
#else
              if (ghdi_control_umts_voice(GHDI_TURN_AMR_ON,
                    current_rat) == GHDI_FAILURE)
              {
                    MSG_HIGH_DS_0(MN_SUB,"=MNCNM= Unable to switch ON AMR Speech vocoder");
              }
#endif            
#endif  /* end of FEATURE_VOC_AMR_WB */
           }
           else
           {
              MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: UE not in WCDMA nor GSM mode");
           }
  
           tch_user_connections_store[DEFAULT_RAB_ID].speech_channel_connected = TRUE;
  
           
        }  /* ! = ITC_SPEECH */
  
        /********************/
        /*  DATA setup      */
        /********************/
        else
        {
           /*
            * If the speech channel is connected,  disconnect it
            */
           if (tch_user_connections_store[DEFAULT_RAB_ID].speech_channel_connected == TRUE)
           {
              MN_disconnect_TCH(MN_VOCODER_OFF);
           }
  
           /* now connect the data channel */
  
           /*************/
           /* GSM MODE  */
           /*************/
           if ( MNCNM_is_gsm_mode(local_as_id) )
           {
              /*********************/
              /*    SUBCHANNEL 0   */
              /*********************/
              if (tch_user_connections_store[DEFAULT_RAB_ID].subchannel == 0)
              {
                 /****************/
                 /* FULL RATE    */
                 /****************/
                 if (tch_user_connections_store[DEFAULT_RAB_ID].tch_rate == TCH_FULL_RATE)
  
                 {
                    if (ghdi_control_subs_tch_gating(GHDI_TURN_DATA_TCH_F_ON,
                                        tch_user_connections_store[DEFAULT_RAB_ID].channel_mode,
                                         mn_asid_2_vsid_map[local_as_id],
                                         local_as_id
                                               ) == 
                                        GHDI_FAILURE)
                    {
                        MSG_HIGH_DS_0(MN_SUB,"=MNCNM= Unable to switch ON FR voc for Data call");
  
                    }
                 }
                 /****************/
                 /* HALF RATE    */
                 /****************/
                 else
                 {
                    if (ghdi_control_subs_tch_gating(GHDI_TURN_DATA_TCH_H_0_ON,
                                        tch_user_connections_store[DEFAULT_RAB_ID].channel_mode,
                                         mn_asid_2_vsid_map[local_as_id],
                                         local_as_id
                                               ) == 
                                        GHDI_FAILURE)
                    {
                        MSG_HIGH_DS_0(MN_SUB,"=MNCNM= Unable to switch ON HR 0 voc for Data call");
                    }
                 }
              }
              /*********************/
              /*  SUBCHANNEL  !=0  */
              /*********************/
              else
              {
                 /****************/
                 /* HALF RATE    */
                 /****************/
                 if (ghdi_control_subs_tch_gating(GHDI_TURN_DATA_TCH_H_1_ON,
                                     tch_user_connections_store[DEFAULT_RAB_ID].channel_mode,
                                         mn_asid_2_vsid_map[local_as_id],
                                         local_as_id
                                            ) == 
                                     GHDI_FAILURE)
                 {
                    MSG_HIGH_DS_0(MN_SUB,"=MNCNM= Unable to switch ON HR 1 voc for Data call");
                 }
              }
           } /* if is_cs_gsm_mode...*/
  
           /***************/
           /* WCDMA MODE  */
           /***************/
           else if (MNCNM_is_umts_mode(local_as_id))
           {       /* Do nothing... RRC will take care of this 
                                                   i.e. CS DATA ON */
               MSG_MED_DS_0(MN_SUB,"=MNCNM= Turn CS DATA Transfer ON");
           }
           else
           {
               MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Invalid Operation Mode !(WCDMA nor GSM)");
           }
  
           tch_user_connections_store[DEFAULT_RAB_ID].data_channel_connected = TRUE;
  
        }  /* end else data setup...*/
  
#ifdef FEATURE_NAS_VOC_REL_AFTER_CALL_HOLD
        MN_turned_on_vocoder = TRUE;  /* flag "ON" to prevent turnning more then 1 */
        
        MSG_HIGH_DS_1(MN_SUB,"=MNCNM= MN turn on the vocoder flag=%d", MN_turned_on_vocoder);
#endif /* #ifdef FEATURE_NAS_VOC_REL_AFTER_CALL_HOLD */
  
     } /* end if OTA channel is connected */
     else if (tch_user_connections_store[DEFAULT_RAB_ID].OTA_channel_connected == FALSE)
     {
         MN_turned_on_vocoder = FALSE;  /* reset on no TCH */
  
         MSG_HIGH_DS_1(MN_SUB,"=MNCNM= MN reset the vocoder flag = %d", MN_turned_on_vocoder);
     }
     else
     {
         if(tch_user_connections_store[DEFAULT_RAB_ID].OTA_channel_connected)
         {
             MSG_HIGH_DS_1(MN_SUB,"=MNCNM= Vocoder had already been acquired = %d ?", MN_turned_on_vocoder);
         }
         else
         {
             MSG_HIGH_DS_1(MN_SUB,"=MNCNM= TCH had not been assigned = %d",
                                          tch_user_connections_store[DEFAULT_RAB_ID].OTA_channel_connected );
         }
     }
  } 
  else
  {
    MSG_HIGH_DS_1(MN_SUB,"=MNCNM= invalid as_id =%d", local_as_id);
  }
}





/*
 * Function name :  MN_disconnect_TCH_with_AS_ID
 * -------------
 *
 * Description :  Disconnects the TCH voice/data channel
 * -----------     
 *
 *
 *
 *
 *
 *
 * Parameters:
 * ----------  
 *
 *
 *
 * Returns:
 * --------
 *
 */
#if defined(FEATURE_DUAL_SIM )

VOID_FUNC   MN_disconnect_TCH_with_AS_ID(mn_vocoder_ctrl_e_type mn_voc_ctrl,sys_modem_as_id_e_type as_id)
{
    
    if((MNCNM_IS_SINGLE_STACK) && (as_id != SYS_MODEM_AS_ID_NONE))
    {
       if(is_cs_gsm_mode(as_id))
       {
          MN_disconnect_TCH_with_RAT(MN_VOCODER_OFF,SYS_RAT_GSM_RADIO_ACCESS, as_id);  /* turn off GSM vocoder */  
       }
       else if (is_cs_wcdma_mode(as_id))
       {
          MN_disconnect_TCH_with_RAT(MN_VOCODER_OFF,SYS_RAT_UMTS_RADIO_ACCESS, as_id);   /* turn off WCDMA vocoder */
       }
       else
       {
          MN_disconnect_TCH_with_RAT(MN_VOCODER_OFF,SYS_RAT_TDS_RADIO_ACCESS, as_id);   /* turn off TDS vocoder */           
       }
    }   
    else
    {
       MSG_HIGH_DS_0(MN_SUB,"=MNCNM= AS_ID is SYS_MODEM_AS_ID_NONE, cannot Disconnect the TCH/RAB !!!!!");         
    }

}

/*
 * Function name :  MN_disconnect_TCH_with_RAT
 * -------------
 *
 * Description :  Disconnects the TCH voice/data channel
 * -----------     
 *
 *
 *
 *
 *
 *
 * Parameters:
 * ----------  
 *
 *
 *
 * Returns:
 * --------
 *
 */

VOID_FUNC   MN_disconnect_TCH_with_RAT(mn_vocoder_ctrl_e_type mn_voc_ctrl,sys_radio_access_tech_e_type current_rat, sys_modem_as_id_e_type as_id)
{

#ifdef FEATURE_SEGMENT_LOADING
    interface_t* ptr_W = NULL;
    interface_t* ptr_T = NULL;

    ptr_W = get_wcdma_interface();
    ptr_T = get_tdscdma_interface();
#endif
#ifdef FEATURE_GSM
   
  if(as_id > SYS_MODEM_AS_ID_NONE && as_id < MAX_AS_IDS)
  {
#if defined FEATURE_DUAL_SIM
     if(gl1_msg_is_l1_vocoder_control_enabled_multisim(as_id) 
#else
     if(gl1_msg_is_l1_vocoder_control_enabled() 
#endif
#ifdef FEATURE_NAS_VOC_REL_AFTER_CALL_HOLD 
      && (mn_voc_ctrl != MN_VOCODER_SWITCH)
#endif /*#ifdef FEATURE_NAS_VOC_REL_AFTER_CALL_HOLD */
      )
     {
         MSG_HIGH_DS_0(MN_SUB,"=MNCNM= GSM L1 is controlling the vocoder");
     }
     else 
#endif /* FEATURE_GSM */

#ifdef FEATURE_WCDMA
#ifdef FEATURE_INTER_RAT_HO_OPT
#ifndef FEATURE_TDSCDMA
#ifdef FEATURE_SEGMENT_LOADING
#if defined FEATURE_DUAL_SIM
     if((ptr_W != NULL) && IWCDMA_rrc_vocoder_control_enabled_sub(ptr_W,as_id) 
#else
     if((ptr_W != NULL) && IWCDMA_rrc_vocoder_control_enabled(ptr_W) 
#endif /* FEATURE_DUAL_SIM */

#else /* else FEATURE_SEGMENT_LOADING */

#if defined FEATURE_DUAL_SIM
     if(rrc_vocoder_control_enabled_sub(as_id)
#else
     if(rrc_vocoder_control_enabled() 
#endif /* FEATURE_DUAL_SIM */
#endif /* FEATURE_SEGMENT_LOADING */

#ifdef FEATURE_NAS_VOC_REL_AFTER_CALL_HOLD 
        && (mn_voc_ctrl != MN_VOCODER_SWITCH)
#endif /*#ifdef FEATURE_NAS_VOC_REL_AFTER_CALL_HOLD */
       )
     {
         MSG_HIGH_DS_0(MN_SUB,"=MNCNM= RRC is controlling the vocoder");
     }

#else /* else FEATURE_TDSCDMA */

#ifdef FEATURE_SEGMENT_LOADING
#if defined FEATURE_DUAL_SIM
     if(((ptr_W != NULL)&&IWCDMA_rrc_vocoder_control_enabled_sub(ptr_W,as_id)) ||
        ((ptr_T != NULL)&&ITDSCDMA_tdsrrc_vocoder_control_enabled_sub(ptr_T,as_id))
#else
     if(((ptr_W != NULL) && IWCDMA_rrc_vocoder_control_enabled(ptr_W)) || 
        ((ptr_T != NULL)&&ITDSCDMA_tdsrrc_vocoder_control_enabled(ptr_T))
#endif /* FEATURE_DUAL_SIM */

#else /* else FEATURE_SEGMENT_LOADING */

#if defined FEATURE_DUAL_SIM
     if(rrc_vocoder_control_enabled_sub(as_id) || tdsrrc_vocoder_control_enabled_sub(as_id) 
#else
     if(rrc_vocoder_control_enabled() || tdsrrc_vocoder_control_enabled() 
#endif /* FEATURE_DUAL_SIM */
#endif /* FEATURE_SEGMENT_LOADING */

#ifdef FEATURE_NAS_VOC_REL_AFTER_CALL_HOLD 
      && (mn_voc_ctrl != MN_VOCODER_SWITCH)
#endif /*#ifdef FEATURE_NAS_VOC_REL_AFTER_CALL_HOLD */
      )
     {
         MSG_HIGH_DS_0(MN_SUB,"=MNCNM= RRC is controlling the vocoder");
     }
#endif /* FEATURE_TDSCDMA */
     else
#endif /* FEATURE_INTER_RAT_HO_OPT */
#endif /* FEATURE_WCDMA */

     /*****************************/
     /* Disconnect Speech Channel */
     /*****************************/
     if(tch_user_connections_store[DEFAULT_RAB_ID].speech_channel_connected == TRUE)
     {

        /*************/
        /* GSM MODE  */
        /*************/
        if (((current_rat == SYS_RAT_GSM_RADIO_ACCESS) || (mn_voc_ctrl == MN_VOCODER_SWITCH_TO_WCDMA)
#if defined(FEATURE_DUAL_SIM )
         || ((MNCNM_IS_SINGLE_STACK == FALSE) || (mn_voc_ctrl == MN_VOCODER_GSM_OFF))
#endif
            ) &&
            (mn_voc_ctrl != MN_VOCODER_SWITCH_TO_GSM))
        {
           /*********************/
           /*    SUBCHANNEL 0   */
           /*********************/
           if(tch_user_connections_store[DEFAULT_RAB_ID].subchannel ==  0)
           {
             /****************/
             /* FULL RATE    */
             /****************/
              if(tch_user_connections_store[DEFAULT_RAB_ID].tch_rate == TCH_FULL_RATE)
              {
                 if (ghdi_control_subs_tch_gating(GHDI_TURN_SPEECH_TCH_F_OFF,
                                       tch_user_connections_store[DEFAULT_RAB_ID].channel_mode,
                                       mn_asid_2_vsid_map[as_id],
                                       as_id
                                            ) ==
                                       GHDI_FAILURE)
                 {
                     MSG_HIGH_DS_0(MN_SUB,"=MNCNM= Unable to switch OFF FR Speech Vocoder");
                 }
              }
             /****************/
             /* HALF RATE    */
             /****************/
              else
              {
                 if (ghdi_control_subs_tch_gating(GHDI_TURN_SPEECH_TCH_H_0_OFF,
                                       tch_user_connections_store[DEFAULT_RAB_ID].channel_mode,
                                       mn_asid_2_vsid_map[as_id],
                                       as_id
                                            )==
                                      GHDI_FAILURE)
                 {
                     MSG_HIGH_DS_0(MN_SUB,"=MNCNM= Unable to switch OFF HR 0 Speech Vocoder");
                 }
              }
           }

           /*********************/
           /*  SUBCHANNEL  !=0  */
           /*********************/
           else
           {
             /****************/
             /* HALF RATE    */
             /****************/

              if(ghdi_control_subs_tch_gating(GHDI_TURN_SPEECH_TCH_H_1_OFF,
                                    tch_user_connections_store[DEFAULT_RAB_ID].channel_mode,
                                    mn_asid_2_vsid_map[as_id],
                                    as_id
                                      )==
                                   GHDI_FAILURE)
              {
                  MSG_HIGH_DS_0(MN_SUB,"=MNCNM= Unable to switch OFF HR 1 Speech Vocoder");
              }
           }

        }  /* end if is_cs_gsm_mode...*/

        /***************/
        /* WCDMA MODE  */
        /***************/
        else if (((current_rat == SYS_RAT_UMTS_RADIO_ACCESS) || (current_rat == SYS_RAT_TDS_RADIO_ACCESS) || (mn_voc_ctrl == MN_VOCODER_SWITCH_TO_GSM)
#if defined(FEATURE_DUAL_SIM ) 
              || ((MNCNM_IS_SINGLE_STACK == FALSE) || (mn_voc_ctrl == MN_VOCODER_WCDMA_OFF))
#endif
                 ) &&
                 (mn_voc_ctrl != MN_VOCODER_SWITCH_TO_WCDMA))
        {

#ifdef FEATURE_VOC_AMR_WB
#ifdef FEATURE_CS_VOICE_OVER_HSPA
            if (ghdi_control_umts_voice(GHDI_TURN_AMR_OFF,
                  tch_user_connections_store[DEFAULT_RAB_ID].umts_codec,
                  tch_user_connections_store[DEFAULT_RAB_ID].cs_voice_type,
                  current_rat)== GHDI_FAILURE)
            {
                MSG_HIGH_DS_0(MN_SUB,"=MNCNM= Unbale to switch OFF AMR Speech Vocoder");
            }
#else
            if (ghdi_control_umts_voice(GHDI_TURN_AMR_OFF,
                  tch_user_connections_store[DEFAULT_RAB_ID].umts_codec,
                  current_rat)== GHDI_FAILURE)
            {
                MSG_HIGH_DS_0(MN_SUB,"=MNCNM= Unbale to switch OFF AMR Speech Vocoder");
            }
#endif          
#else
#ifdef FEATURE_CS_VOICE_OVER_HSPA
            if (ghdi_control_umts_voice(GHDI_TURN_AMR_OFF,
                  tch_user_connections_store[DEFAULT_RAB_ID].cs_voice_type,
                  current_rat)== GHDI_FAILURE)
            {
                MSG_HIGH_DS_0(MN_SUB,"=MNCNM= Unbale to switch OFF AMR Speech Vocoder");
            }
#else
            if (ghdi_control_umts_voice(GHDI_TURN_AMR_OFF,
                  current_rat)== GHDI_FAILURE)
            {
                MSG_HIGH_DS_0(MN_SUB,"=MNCNM= Unbale to switch OFF AMR Speech Vocoder");
            }
#endif          
#endif /* end of FEATURE_VOC_AMR_WB */
        }
        else
        {
            MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Unabled to disable vocoder, unknown config");
        }

#ifdef FEATURE_NAS_VOC_REL_AFTER_CALL_HOLD
        /* Initialize the GSM vocoder configuration to DEFAULT value */
        if (mn_voc_ctrl != MN_VOCODER_SWITCH)
        {
          tch_user_connections_store[DEFAULT_RAB_ID].OTA_channel_connected = FALSE;
          tch_user_connections_store[DEFAULT_RAB_ID].speech_channel_connected = FALSE;
          tch_user_connections_store[DEFAULT_RAB_ID].data_channel_connected = FALSE;
          tch_user_connections_store[DEFAULT_RAB_ID].tch_rate = TCH_FULL_RATE;
          tch_user_connections_store[DEFAULT_RAB_ID].subchannel = 0;  /* determine FULL or HALF RATE */
          tch_user_connections_store[DEFAULT_RAB_ID].channel_mode = SPEECH_V1;  /* determine FULL or HALF RATE */
        }
   
        MN_turned_on_vocoder = FALSE;  /* Reset attempt */
        MSG_HIGH_DS_1(MN_SUB,"=MNCNM= MN reset the vocoder flag speech =%d", MN_turned_on_vocoder);
#endif /* #ifdef FEATURE_NAS_VOC_REL_AFTER_CALL_HOLD */  

     }  /* end disconnect Speech channel  */

     /*****************************/
     /* Disconnect Data Channel   */
     /*****************************/

     else if(tch_user_connections_store[DEFAULT_RAB_ID].data_channel_connected == TRUE)
     {
        /*************/
        /* GSM MODE  */
        /*************/
        if ((current_rat == SYS_RAT_GSM_RADIO_ACCESS) || (mn_voc_ctrl == MN_VOCODER_SWITCH_TO_WCDMA))
        {
           /*********************/
           /*    SUBCHANNEL 0   */
           /*********************/
           if(tch_user_connections_store[DEFAULT_RAB_ID].subchannel ==  0)
           {
             /****************/
             /* FULL RATE    */
             /****************/
              if(tch_user_connections_store[DEFAULT_RAB_ID].tch_rate == TCH_FULL_RATE)
              {
                 if (ghdi_control_subs_tch_gating(GHDI_TURN_DATA_TCH_F_OFF,
                                       tch_user_connections_store[DEFAULT_RAB_ID].channel_mode,
                                       mn_asid_2_vsid_map[as_id],
                                       as_id
                                            ) ==
                                      GHDI_FAILURE)
                 {
                     MSG_HIGH_DS_0(MN_SUB,"=MNCNM= Unable to switch OFF FR Data Vocoder");
                 }
              }

             /****************/
             /* HALF RATE    */
             /****************/
              else
              {
                 if (ghdi_control_subs_tch_gating(GHDI_TURN_DATA_TCH_H_0_OFF,
                                       tch_user_connections_store[DEFAULT_RAB_ID].channel_mode,
                                       mn_asid_2_vsid_map[as_id],
                                       as_id
                                            )==
                                      GHDI_FAILURE)
                 {
                     MSG_HIGH_DS_0(MN_SUB,"=MNCNM= Unable to switch OFF HR 0 Data Vocoder");
                 }

              }
           }  /* end if subchannel 0*/

           /************************/
           /*    SUBCHANNEL  !=0   */
           /************************/
           else
           {

             /****************/
             /* HALF RATE    */
             /****************/
              if (ghdi_control_subs_tch_gating(GHDI_TURN_DATA_TCH_H_1_OFF,
                                    tch_user_connections_store[DEFAULT_RAB_ID].channel_mode,
                                    mn_asid_2_vsid_map[as_id],
                                    as_id
                                       ) == 
                                    GHDI_FAILURE)
              {
                  MSG_HIGH_DS_0(MN_SUB,"=MNCNM= Unable to switch OFF HR 1 Data Vocoder");
              }
           }

        }  /* end if gsm_mode  */

        /***************/
        /* WCDMA MODE  */
        /***************/
        else if ((current_rat == SYS_RAT_UMTS_RADIO_ACCESS) || (current_rat == SYS_RAT_TDS_RADIO_ACCESS))
        {       /* Do nothing... RRC will take care of this 
                                                 i.e. CS DATA OFF */
            MSG_MED_DS_0(MN_SUB,"=MNCNM= CS Data call stopped");
        }
        else
        {
            MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Invalid Operation Mode !(WDMA nor GSM)");
        }

#ifdef FEATURE_NAS_VOC_REL_AFTER_CALL_HOLD
        /* Initialize the GSM vocoder configuration to DEFAULT value */
        if (mn_voc_ctrl != MN_VOCODER_SWITCH)
        {
          tch_user_connections_store[DEFAULT_RAB_ID].OTA_channel_connected = FALSE;
          tch_user_connections_store[DEFAULT_RAB_ID].speech_channel_connected = FALSE;
          tch_user_connections_store[DEFAULT_RAB_ID].data_channel_connected = FALSE;
          tch_user_connections_store[DEFAULT_RAB_ID].tch_rate = TCH_FULL_RATE;
          tch_user_connections_store[DEFAULT_RAB_ID].subchannel = 0;  /* determine FULL or HALF RATE */
          tch_user_connections_store[DEFAULT_RAB_ID].channel_mode = SPEECH_V1;  /* determine FULL or HALF RATE */
       }

       MN_turned_on_vocoder = FALSE;  /* Reset attempt */
       MSG_HIGH_DS_1(MN_SUB,"=MNCNM= MN reset the vocoder flag data =%d", MN_turned_on_vocoder);
#endif /* #ifdef FEATURE_NAS_VOC_REL_AFTER_CALL_HOLD */

     }  /* end if else data channel connected */
     else
     {
        MSG_HIGH_DS_0(MN_SUB,"=MNCNM= TCH had been disconnected from Vocoder");

#ifdef FEATURE_NAS_VOC_REL_AFTER_CALL_HOLD
     if (mn_voc_ctrl == MN_VOCODER_OFF)
     {
        tch_user_connections_store[DEFAULT_RAB_ID].OTA_channel_connected = FALSE;
        tch_user_connections_store[DEFAULT_RAB_ID].speech_channel_connected = FALSE;
        tch_user_connections_store[DEFAULT_RAB_ID].data_channel_connected = FALSE;
        tch_user_connections_store[DEFAULT_RAB_ID].tch_rate = TCH_FULL_RATE;
        tch_user_connections_store[DEFAULT_RAB_ID].subchannel = 0;  /* determine FULL or HALF RATE */
        tch_user_connections_store[DEFAULT_RAB_ID].channel_mode = SPEECH_V1;  /* determine FULL or HALF RATE */
     }
#endif   

     }  /* end if disconnect data channel */

#ifndef FEATURE_NAS_VOC_REL_AFTER_CALL_HOLD
        /* Initialize the GSM vocoder configuration to DEFAULT value */
        if (mn_voc_ctrl != MN_VOCODER_SWITCH)
        {
          tch_user_connections_store[DEFAULT_RAB_ID].OTA_channel_connected = FALSE;
          tch_user_connections_store[DEFAULT_RAB_ID].speech_channel_connected = FALSE;
          tch_user_connections_store[DEFAULT_RAB_ID].data_channel_connected = FALSE;
          tch_user_connections_store[DEFAULT_RAB_ID].tch_rate = TCH_FULL_RATE;
          tch_user_connections_store[DEFAULT_RAB_ID].subchannel = 0;  /* determine FULL or HALF RATE */
          tch_user_connections_store[DEFAULT_RAB_ID].channel_mode = SPEECH_V1;  /* determine FULL or HALF RATE */
       }

       MN_turned_on_vocoder = FALSE;  /* Reset attempt */

       MSG_HIGH_DS_1(MN_SUB,"=MNCNM= MN reset the vocoder flag data =%d", MN_turned_on_vocoder);
#endif /* #ifndef FEATURE_NAS_VOC_REL_AFTER_CALL_HOLD */

  }
  else
  {
     MSG_HIGH_DS_1(MN_SUB,"=MNCNM= invalid as_id =%d", as_id);
  }
}	  

#endif /* #ifdef FEATURE_DUAL_SIM  */


/*
 * Function name :  MN_disconnect_TCH
 * -------------
 *
 * Description :  Disconnects the TCH voice/data channel
 * -----------     
 *
 *
 *
 *
 *
 *
 * Parameters:
 * ----------  
 *
 *
 *
 * Returns:
 * --------
 *
 */

VOID_FUNC   MN_disconnect_TCH(mn_vocoder_ctrl_e_type mn_voc_ctrl)
{
   sys_radio_access_tech_e_type current_rat;
   sys_modem_as_id_e_type       local_as_id = 0;
#ifdef FEATURE_SEGMENT_LOADING
   interface_t* ptr_W = NULL;
   interface_t* ptr_T = NULL;
#endif

#ifdef FEATURE_DUAL_SIM
   if(MNCNM_IS_SINGLE_STACK)
   {
     local_as_id = mn_as_id;
   }
#ifdef FEATURE_DUAL_ACTIVE
   else
   {
     local_as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
   }
#endif
#endif
#ifdef FEATURE_SEGMENT_LOADING   
   ptr_W = get_wcdma_interface();
   ptr_T = get_tdscdma_interface();
#endif

  if(local_as_id > SYS_MODEM_AS_ID_NONE && local_as_id < MAX_AS_IDS)
  {
#ifdef FEATURE_GSM
     
  
#if defined FEATURE_DUAL_SIM
     if(gl1_msg_is_l1_vocoder_control_enabled_multisim(local_as_id)
#else
     if(gl1_msg_is_l1_vocoder_control_enabled()
#endif
  
#ifdef FEATURE_NAS_VOC_REL_AFTER_CALL_HOLD 
        && (mn_voc_ctrl != MN_VOCODER_SWITCH)
#endif /*#ifdef FEATURE_NAS_VOC_REL_AFTER_CALL_HOLD */
       )
     {
         MSG_HIGH_DS_0(MN_SUB,"=MNCNM= GSM L1 is controlling the vocoder");
     }
     else 
#endif /* FEATURE_GSM */
  
#ifdef FEATURE_WCDMA
#ifdef FEATURE_INTER_RAT_HO_OPT
#ifndef FEATURE_TDSCDMA
#ifdef FEATURE_SEGMENT_LOADING
#if defined FEATURE_DUAL_SIM
      if((ptr_W != NULL) && IWCDMA_rrc_vocoder_control_enabled_sub(ptr_W,local_as_id)
#else
      if((ptr_W != NULL) && IWCDMA_rrc_vocoder_control_enabled(ptr_W)
#endif /* FEATURE_DUAL_SIM */

#else /* else FEATURE_SEGMENT_LOADING */

#if defined FEATURE_DUAL_SIM
     if(rrc_vocoder_control_enabled_sub(local_as_id)
#else
     if(rrc_vocoder_control_enabled()
#endif /* FEATURE_DUAL_SIM */
#endif /* FEATURE_SEGMENT_LOADING */

#ifdef FEATURE_NAS_VOC_REL_AFTER_CALL_HOLD 
        && (mn_voc_ctrl != MN_VOCODER_SWITCH)
#endif /*#ifdef FEATURE_NAS_VOC_REL_AFTER_CALL_HOLD */
       )
     {
         MSG_HIGH_DS_0(MN_SUB,"=MNCNM= RRC is controlling the vocoder");
     }

#else /* else FEATURE_TDSCDMA */

#ifdef FEATURE_SEGMENT_LOADING
#if defined FEATURE_DUAL_SIM
      if (((ptr_W != NULL) && IWCDMA_rrc_vocoder_control_enabled_sub(ptr_W,local_as_id)) || 
          ((ptr_T!=NULL) && ITDSCDMA_tdsrrc_vocoder_control_enabled_sub(ptr_T,local_as_id))
#else
      if (((ptr_W != NULL) && IWCDMA_rrc_vocoder_control_enabled(ptr_W)) ||
          ((ptr_T!=NULL) && ITDSCDMA_tdsrrc_vocoder_control_enabled(ptr_T))
#endif /* FEATURE_DUAL_SIM */

#else /* else FEATURE_SEGMENT_LOADING */

#if defined FEATURE_DUAL_SIM
     if ( (rrc_vocoder_control_enabled_sub(local_as_id) || tdsrrc_vocoder_control_enabled_sub(local_as_id)) 
#else
     if ( (rrc_vocoder_control_enabled() || tdsrrc_vocoder_control_enabled()) 
#endif /* FEATURE_DUAL_SIM */
#endif /* FEATURE_SEGMENT_LOADING */

#ifdef FEATURE_NAS_VOC_REL_AFTER_CALL_HOLD 
          && (mn_voc_ctrl != MN_VOCODER_SWITCH)
#endif /*#ifdef FEATURE_NAS_VOC_REL_AFTER_CALL_HOLD */
        )
     {
         MSG_HIGH_DS_0(MN_SUB,"=MNCNM= RRC is controlling the vocoder");
     }
#endif /* FEATURE_TDSCDMA */
     else
#endif /* FEATURE_INTER_RAT_HO_OPT */
#endif /* FEATURE_WCDMA */
  
     /*****************************/
     /* Disconnect Speech Channel */
     /*****************************/
     if(tch_user_connections_store[DEFAULT_RAB_ID].speech_channel_connected == TRUE)
     {
  
        /*************/
        /* GSM MODE  */
        /*************/
        if ((MNCNM_is_gsm_mode(local_as_id) || (mn_voc_ctrl == MN_VOCODER_SWITCH_TO_WCDMA)
#if defined(FEATURE_DUAL_SIM ) 
         || ((MNCNM_IS_SINGLE_STACK == FALSE) || (mn_voc_ctrl == MN_VOCODER_GSM_OFF))
#endif
            ) &&
            (mn_voc_ctrl != MN_VOCODER_SWITCH_TO_GSM))
        {
           /*********************/
           /*    SUBCHANNEL 0   */
           /*********************/
           if(tch_user_connections_store[DEFAULT_RAB_ID].subchannel ==  0)
           {
             /****************/
             /* FULL RATE    */
             /****************/
              if(tch_user_connections_store[DEFAULT_RAB_ID].tch_rate == TCH_FULL_RATE)
              {
                 if (ghdi_control_subs_tch_gating(GHDI_TURN_SPEECH_TCH_F_OFF,
                                       tch_user_connections_store[DEFAULT_RAB_ID].channel_mode,
                                         mn_asid_2_vsid_map[local_as_id],
                                         local_as_id
                                            ) ==
                                      GHDI_FAILURE)
                 {
                     MSG_HIGH_DS_0(MN_SUB,"=MNCNM= Unable to switch OFF FR Speech Vocoder");
                 }
              }
             /****************/
             /* HALF RATE    */
             /****************/
              else
              {
                 if (ghdi_control_subs_tch_gating(GHDI_TURN_SPEECH_TCH_H_0_OFF,
                                       tch_user_connections_store[DEFAULT_RAB_ID].channel_mode,
                                       mn_asid_2_vsid_map[local_as_id],
                                       local_as_id
                                            )==
                                      GHDI_FAILURE)
                 {
                     MSG_HIGH_DS_0(MN_SUB,"=MNCNM= Unable to switch OFF HR 0 Speech Vocoder");
                 }
              }
           }
  
           /*********************/
           /*  SUBCHANNEL  !=0  */
           /*********************/
           else
           {
             /****************/
             /* HALF RATE    */
             /****************/
  
              if(ghdi_control_subs_tch_gating(GHDI_TURN_SPEECH_TCH_H_1_OFF,
                                    tch_user_connections_store[DEFAULT_RAB_ID].channel_mode,
                                         mn_asid_2_vsid_map[local_as_id],
                                         local_as_id
                                        )==
                                   GHDI_FAILURE)
              {
                  MSG_HIGH_DS_0(MN_SUB,"=MNCNM= Unable to switch OFF HR 1 Speech Vocoder");
              }
           }
  
        }  /* end if is_cs_gsm_mode...*/
  
        /***************/
        /* WCDMA MODE  */
        /***************/
        else if ((MNCNM_is_umts_mode(local_as_id) || (mn_voc_ctrl == MN_VOCODER_SWITCH_TO_GSM)
#if defined(FEATURE_DUAL_SIM ) 
              || ((MNCNM_IS_SINGLE_STACK == FALSE) || (mn_voc_ctrl == MN_VOCODER_WCDMA_OFF))
#endif
                 ) &&
                 (mn_voc_ctrl != MN_VOCODER_SWITCH_TO_WCDMA))
        {
          if (MNCNM_is_wcdma_mode(local_as_id))
          {
            current_rat = SYS_RAT_UMTS_RADIO_ACCESS;
          }
          else
          {
            current_rat = SYS_RAT_TDS_RADIO_ACCESS;
          }
  
#ifdef FEATURE_VOC_AMR_WB
#ifdef FEATURE_CS_VOICE_OVER_HSPA
            if (ghdi_control_umts_voice(GHDI_TURN_AMR_OFF,
                  tch_user_connections_store[DEFAULT_RAB_ID].umts_codec,
                  tch_user_connections_store[DEFAULT_RAB_ID].cs_voice_type,
                  current_rat)== GHDI_FAILURE)
            {
                MSG_HIGH_DS_0(MN_SUB,"=MNCNM= Unbale to switch OFF AMR Speech Vocoder");
            }
#else
            if (ghdi_control_umts_voice(GHDI_TURN_AMR_OFF,
                  tch_user_connections_store[DEFAULT_RAB_ID].umts_codec,
                  current_rat)== GHDI_FAILURE)
            {
                MSG_HIGH_DS_0(MN_SUB,"=MNCNM= Unbale to switch OFF AMR Speech Vocoder");
            }
#endif          
#else
#ifdef FEATURE_CS_VOICE_OVER_HSPA
            if (ghdi_control_umts_voice(GHDI_TURN_AMR_OFF,
                  tch_user_connections_store[DEFAULT_RAB_ID].cs_voice_type,
                  current_rat)== GHDI_FAILURE)
            {
                MSG_HIGH_DS_0(MN_SUB,"=MNCNM= Unbale to switch OFF AMR Speech Vocoder");
            }
#else
            if (ghdi_control_umts_voice(GHDI_TURN_AMR_OFF,
                  current_rat)== GHDI_FAILURE)
            {
                MSG_HIGH_DS_0(MN_SUB,"=MNCNM= Unbale to switch OFF AMR Speech Vocoder");
            }
#endif          
#endif /* end of FEATURE_VOC_AMR_WB */
        }
        else
        {
          MSG_ERROR("unknown config : Force releasing the vocoder", 0, 0, 0);

#ifdef FEATURE_VOC_AMR_WB
#ifdef FEATURE_CS_VOICE_OVER_HSPA
          if (ghdi_control_umts_voice(GHDI_TURN_AMR_OFF,
                tch_user_connections_store[DEFAULT_RAB_ID].umts_codec,
                tch_user_connections_store[DEFAULT_RAB_ID].cs_voice_type,
                SYS_RAT_TDS_RADIO_ACCESS)== GHDI_FAILURE)
          {
              MSG_HIGH("Unbale to switch OFF AMR Speech Vocoder",0,0,0);
          }
#else
          if (ghdi_control_umts_voice(GHDI_TURN_AMR_OFF,
                tch_user_connections_store[DEFAULT_RAB_ID].umts_codec,
                SYS_RAT_TDS_RADIO_ACCESS)== GHDI_FAILURE)
          {
              MSG_HIGH("Unbale to switch OFF AMR Speech Vocoder",0,0,0);
          }
#endif          
#else
#ifdef FEATURE_CS_VOICE_OVER_HSPA
          if (ghdi_control_umts_voice(GHDI_TURN_AMR_OFF,
                tch_user_connections_store[DEFAULT_RAB_ID].cs_voice_type,
                SYS_RAT_TDS_RADIO_ACCESS)== GHDI_FAILURE)
          {
              MSG_HIGH("Unbale to switch OFF AMR Speech Vocoder",0,0,0);
          }
#else
          if (ghdi_control_umts_voice(GHDI_TURN_AMR_OFF,
                SYS_RAT_TDS_RADIO_ACCESS)== GHDI_FAILURE)
          {
              MSG_HIGH("Unbale to switch OFF AMR Speech Vocoder",0,0,0);
          }
#endif          
#endif /* end of FEATURE_VOC_AMR_WB */

        }
  
#ifdef FEATURE_NAS_VOC_REL_AFTER_CALL_HOLD   
        /* Initialize the GSM vocoder configuration to DEFAULT value */
        if (mn_voc_ctrl != MN_VOCODER_SWITCH)
        {
          tch_user_connections_store[DEFAULT_RAB_ID].OTA_channel_connected = FALSE;
          tch_user_connections_store[DEFAULT_RAB_ID].speech_channel_connected = FALSE;
          tch_user_connections_store[DEFAULT_RAB_ID].data_channel_connected = FALSE;
          tch_user_connections_store[DEFAULT_RAB_ID].tch_rate = TCH_FULL_RATE;
          tch_user_connections_store[DEFAULT_RAB_ID].subchannel = 0;  /* determine FULL or HALF RATE */
          tch_user_connections_store[DEFAULT_RAB_ID].channel_mode = SPEECH_V1;  /* determine FULL or HALF RATE */
        }
     
        MN_turned_on_vocoder = FALSE;  /* Reset attempt */
        MSG_HIGH_DS_1(MN_SUB,"=MNCNM= MN reset the vocoder flag speech =%d", MN_turned_on_vocoder);
#endif /* FEATURE_NAS_VOC_REL_AFTER_CALL_HOLD */
     }  /* end disconnect Speech channel  */
  
     /*****************************/
     /* Disconnect Data Channel   */
     /*****************************/
  
     else if(tch_user_connections_store[DEFAULT_RAB_ID].data_channel_connected == TRUE)
     {
        /*************/
        /* GSM MODE  */
        /*************/
        if ( MNCNM_is_gsm_mode(local_as_id) || (mn_voc_ctrl == MN_VOCODER_SWITCH_TO_WCDMA))
        {
           /*********************/
           /*    SUBCHANNEL 0   */
           /*********************/
           if(tch_user_connections_store[DEFAULT_RAB_ID].subchannel ==  0)
           {
             /****************/
             /* FULL RATE    */
             /****************/
              if(tch_user_connections_store[DEFAULT_RAB_ID].tch_rate == TCH_FULL_RATE)
              {
                 if (ghdi_control_subs_tch_gating(GHDI_TURN_DATA_TCH_F_OFF,
                                       tch_user_connections_store[DEFAULT_RAB_ID].channel_mode,
                                       mn_asid_2_vsid_map[local_as_id],
                                       local_as_id
                                            ) ==
                                      GHDI_FAILURE)
                 {
                     MSG_HIGH_DS_0(MN_SUB,"=MNCNM= Unable to switch OFF FR Data Vocoder");
                 }
              }
  
             /****************/
             /* HALF RATE    */
             /****************/
              else
              {
                 if (ghdi_control_subs_tch_gating(GHDI_TURN_DATA_TCH_H_0_OFF,
                                       tch_user_connections_store[DEFAULT_RAB_ID].channel_mode,
                                         mn_asid_2_vsid_map[local_as_id],
                                         local_as_id
                                            )==
                                      GHDI_FAILURE)
                 {
                     MSG_HIGH_DS_0(MN_SUB,"=MNCNM= Unable to switch OFF HR 0 Data Vocoder");
                 }
  
              }
           }  /* end if subchannel 0*/
  
           /************************/
           /*    SUBCHANNEL  !=0   */
           /************************/
           else
           {
  
             /****************/
             /* HALF RATE    */
             /****************/
              if (ghdi_control_subs_tch_gating(GHDI_TURN_DATA_TCH_H_1_OFF,
                                    tch_user_connections_store[DEFAULT_RAB_ID].channel_mode,
                                    mn_asid_2_vsid_map[local_as_id],
                                    local_as_id
                                         ) == 
                                   GHDI_FAILURE)
              {
                  MSG_HIGH_DS_0(MN_SUB,"=MNCNM= Unable to switch OFF HR 1 Data Vocoder");
              }
           }
  
        }  /* end if gsm_mode  */
  
        /***************/
        /* WCDMA MODE  */
        /***************/
        else if (MNCNM_is_umts_mode(local_as_id))
        {       /* Do nothing... RRC will take care of this 
                                                   i.e. CS DATA OFF */
            MSG_MED_DS_0(MN_SUB,"=MNCNM= CS Data call stopped");
        }
        else
        {
            MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Invalid Operation Mode !(WDMA nor GSM)");
        }
  
#ifdef FEATURE_NAS_VOC_REL_AFTER_CALL_HOLD   
        /* Initialize the GSM vocoder configuration to DEFAULT value */
        if (mn_voc_ctrl != MN_VOCODER_SWITCH)
        {
          tch_user_connections_store[DEFAULT_RAB_ID].OTA_channel_connected = FALSE;
          tch_user_connections_store[DEFAULT_RAB_ID].speech_channel_connected = FALSE;
          tch_user_connections_store[DEFAULT_RAB_ID].data_channel_connected = FALSE;
          tch_user_connections_store[DEFAULT_RAB_ID].tch_rate = TCH_FULL_RATE;
          tch_user_connections_store[DEFAULT_RAB_ID].subchannel = 0;  /* determine FULL or HALF RATE */
          tch_user_connections_store[DEFAULT_RAB_ID].channel_mode = SPEECH_V1;  /* determine FULL or HALF RATE */
       }
  
       MN_turned_on_vocoder = FALSE;  /* Reset attempt */
       MSG_HIGH_DS_1(MN_SUB,"=MNCNM= MN reset the vocoder flag data =%d", MN_turned_on_vocoder);
#endif /* FEATURE_NAS_VOC_REL_AFTER_CALL_HOLD */
  
     }  /* end if else data channel connected */
     else
     {
        MSG_HIGH_DS_0(MN_SUB,"=MNCNM= TCH had been disconnected from Vocoder");
#ifdef FEATURE_NAS_VOC_REL_AFTER_CALL_HOLD 
        if (mn_voc_ctrl == MN_VOCODER_OFF)
        {
          tch_user_connections_store[DEFAULT_RAB_ID].OTA_channel_connected = FALSE;
          tch_user_connections_store[DEFAULT_RAB_ID].speech_channel_connected = FALSE;
          tch_user_connections_store[DEFAULT_RAB_ID].data_channel_connected = FALSE;
          tch_user_connections_store[DEFAULT_RAB_ID].tch_rate = TCH_FULL_RATE;
          tch_user_connections_store[DEFAULT_RAB_ID].subchannel = 0;  /* determine FULL or HALF RATE */
          tch_user_connections_store[DEFAULT_RAB_ID].channel_mode = SPEECH_V1;  /* determine FULL or HALF RATE */
       }      
#endif /* FEATURE_NAS_VOC_REL_AFTER_CALL_HOLD */
     }  /* end if disconnect data channel */
  
#ifndef FEATURE_NAS_VOC_REL_AFTER_CALL_HOLD
        /* Initialize the GSM vocoder configuration to DEFAULT value */
        if (mn_voc_ctrl != MN_VOCODER_SWITCH)
        {
          tch_user_connections_store[DEFAULT_RAB_ID].OTA_channel_connected = FALSE;
          tch_user_connections_store[DEFAULT_RAB_ID].speech_channel_connected = FALSE;
          tch_user_connections_store[DEFAULT_RAB_ID].data_channel_connected = FALSE;
          tch_user_connections_store[DEFAULT_RAB_ID].tch_rate = TCH_FULL_RATE;
          tch_user_connections_store[DEFAULT_RAB_ID].subchannel = 0;  /* determine FULL or HALF RATE */
          tch_user_connections_store[DEFAULT_RAB_ID].channel_mode = SPEECH_V1;  /* determine FULL or HALF RATE */
       }
  
       MN_turned_on_vocoder = FALSE;  /* Reset attempt */
  
       MSG_HIGH_DS_1(MN_SUB,"=MNCNM= MN reset the vocoder flag data =%d", MN_turned_on_vocoder);
#endif /* #ifndef FEATURE_NAS_VOC_REL_AFTER_CALL_HOLD */
  
  }
  else
  {
    MSG_HIGH_DS_1(MN_SUB,"=MNCNM= invalid as_id =%d", local_as_id);
  }
}


/*
 * Function name :  MN_update_code_type
 * -------------
 *
 * Description :  updates the codec type, if it got changed after event A and before event B; event A being TCH assignment with a codec
 * ----------- event B being, actual acquisition of the vocoder by MVS
 *
 *
 *
 * Parameters:
 * ----------  
 *
 *
 *
 * Returns:
 * --------
 *
 */
VOID_FUNC MN_update_code_type(byte  information_transfer_capability)
{
   sys_radio_access_tech_e_type current_rat;
   sys_modem_as_id_e_type       local_as_id = 0;
#ifdef FEATURE_SEGMENT_LOADING
   interface_t* ptr_W = NULL;
   interface_t* ptr_T = NULL;
#endif
#ifdef FEATURE_DUAL_SIM
   if(MNCNM_IS_SINGLE_STACK)
   {
     local_as_id = mn_as_id;
   }
#ifdef FEATURE_DUAL_ACTIVE
   else
   {
     local_as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
   }
#endif
#endif
#ifdef FEATURE_SEGMENT_LOADING  
   ptr_W = get_wcdma_interface();   
   ptr_T = get_tdscdma_interface(); 
#endif

  if(local_as_id > SYS_MODEM_AS_ID_NONE && local_as_id < MAX_AS_IDS)
  {
#ifdef FEATURE_GSM
     
  
      /*********************************/
      /*   OTA channel is connected    */
      /*********************************/
  
      /* For both GSM & UMTS, Use DEFAULT_RAB_ID below. Will change when we 
       * implement the Multicall Feature (Not the Multicall SS)  */
  
#if defined FEATURE_DUAL_SIM
      if(gl1_msg_is_l1_vocoder_control_enabled_multisim(local_as_id))	  
#else
      if(gl1_msg_is_l1_vocoder_control_enabled())	  
#endif
     {
         MSG_HIGH_DS_0(MN_SUB,"=MNCNM= GSM L1 is controlling the vocoder");
     }
     else 
#endif /* FEATURE_GSM */
  
#ifdef FEATURE_WCDMA
#ifdef FEATURE_INTER_RAT_HO_OPT
#ifdef FEATURE_TDSCDMA
#ifdef FEATURE_SEGMENT_LOADING
#if defined FEATURE_DUAL_SIM
      if (((ptr_W != NULL)&&IWCDMA_rrc_vocoder_control_enabled_sub(ptr_W,local_as_id)) || 
          ((ptr_T!=NULL)&&ITDSCDMA_tdsrrc_vocoder_control_enabled_sub(ptr_T,local_as_id)))		 	
#else
      if (((ptr_W != NULL)&&IWCDMA_rrc_vocoder_control_enabled(ptr_W)) || 
          ((ptr_T!=NULL)&&ITDSCDMA_tdsrrc_vocoder_control_enabled(ptr_T)))
#endif /* FEATURE_DUAL_SIM */

#else /* else FEATURE_SEGMENT_LOADING */

#if defined FEATURE_DUAL_SIM
     if (rrc_vocoder_control_enabled_sub(local_as_id) || tdsrrc_vocoder_control_enabled_sub(local_as_id))	
#else
     if (rrc_vocoder_control_enabled() || tdsrrc_vocoder_control_enabled())
#endif /* FEATURE_DUAL_SIM */
#endif /* FEATURE_SEGMENT_LOADING */

#else /* else FEATURE_TDSCDMA */

#ifdef FEATURE_SEGMENT_LOADING
#if defined FEATURE_DUAL_SIM
     if((ptr_W != NULL)&&IWCDMA_rrc_vocoder_control_enabled_sub(ptr_W,local_as_id))
#else
     if((ptr_W != NULL)&&IWCDMA_rrc_vocoder_control_enabled(ptr_W))
#endif /* FEATURE_DUAL_SIM */

#else /* else FEATURE_SEGMENT_LOADING */

#if defined FEATURE_DUAL_SIM
      if(rrc_vocoder_control_enabled_sub(local_as_id))
#else
     if(rrc_vocoder_control_enabled())
#endif /* FEATURE_DUAL_SIM */
#endif /* FEATURE_SEGMENT_LOADING */
#endif /* FEATURE_TDSCDMA */      
     {
         MSG_HIGH_DS_0(MN_SUB,"=MNCNM= RRC is controlling the vocoder. Updating latest umts_codec in ghdi_mvs_mode");
#ifdef FEATURE_VOC_AMR_WB
         ghdi_update_mvs_mode(tch_user_connections_store[DEFAULT_RAB_ID].umts_codec);
#else
         ghdi_update_mvs_mode((umts_codecs_list_T)0);
#endif
     }  /* FEATURE_VOC_AMR_WB */ 
  
     else
#endif /* FEATURE_INTER_RAT_HO_OPT */
#endif /* FEATURE_WCDMA */
  
     if ((tch_user_connections_store[DEFAULT_RAB_ID].OTA_channel_connected == TRUE) && 
         (MN_turned_on_vocoder == TRUE)) 
     {
        /********************/
        /*  Speech setup    */
        /********************/
        if(
#ifdef FEATURE_ALS
           (information_transfer_capability == ITC_AUX_SPEECH) ||
#endif 
           (information_transfer_capability == ITC_SPEECH))
        {
           /* now update the speech channel configuration */
  
           /*************/
           /* GSM MODE  */
           /*************/
           if (  MNCNM_is_gsm_mode(local_as_id) )
           {
              /*********************/
              /*    SUBCHANNEL 0   */
              /*********************/
              if (tch_user_connections_store[DEFAULT_RAB_ID].subchannel == 0)
              {
                 /****************/
                 /* FULL RATE    */
                 /****************/
                 if (tch_user_connections_store[DEFAULT_RAB_ID].tch_rate == TCH_FULL_RATE)
                 {
  
                    if (ghdi_control_subs_tch_gating(GHDI_TURN_SPEECH_TCH_F_ON,
                                         tch_user_connections_store[DEFAULT_RAB_ID].channel_mode,
                                         mn_asid_2_vsid_map[local_as_id],
                                         local_as_id
                                               ) ==  
                                         GHDI_FAILURE)
                    {
                        MSG_HIGH_DS_0(MN_SUB,"=MNCNM= Unable to Configure FR vocoder to ON");
                    }
                 }
                 /****************/
                 /* HALF RATE    */
                 /****************/
                 else
                 {
                    if (ghdi_control_subs_tch_gating(GHDI_TURN_SPEECH_TCH_H_0_ON,
                                        tch_user_connections_store[DEFAULT_RAB_ID].channel_mode,
                                         mn_asid_2_vsid_map[local_as_id],
                                         local_as_id
                                               ) ==
                                        GHDI_FAILURE)
                    {
                        MSG_HIGH_DS_0(MN_SUB,"=MNCNM= Unable to Configure HR vocoder to ON");
                    }
  
                 }
              }
              /*********************/
              /*  SUBCHANNEL  !=0  */
              /*********************/
              else
              {
                 /****************/
                 /* HALF RATE    */
                 /****************/
                 if (ghdi_control_subs_tch_gating(GHDI_TURN_SPEECH_TCH_H_1_ON,
                                     tch_user_connections_store[DEFAULT_RAB_ID].channel_mode,
                                         mn_asid_2_vsid_map[local_as_id],
                                         local_as_id
                                            ) == 
                                     GHDI_FAILURE)
                 {
                     MSG_HIGH_DS_0(MN_SUB,"=MNCNM= Unable to Configure to ON HR subchannel 1 vocoder");
                 }
  
              }
  
           } /* if is_cs_gsm_mode...*/
           /***************/
           /* WCDMA MODE  */
           /***************/
           else if (MNCNM_is_umts_mode(local_as_id))
           {
              if (MNCNM_is_wcdma_mode(local_as_id))
              {
                 current_rat = SYS_RAT_UMTS_RADIO_ACCESS;
              }
              else
              {
                 current_rat = SYS_RAT_TDS_RADIO_ACCESS;
              }
  
#ifdef FEATURE_VOC_AMR_WB 
              if (ghdi_control_umts_voice(GHDI_TURN_AMR_ON,
                    tch_user_connections_store[DEFAULT_RAB_ID].umts_codec,
                    current_rat) == GHDI_FAILURE)
              {
                    MSG_HIGH_DS_0(MN_SUB,"=MNCNM= Unable to switch ON AMR Speech vocoder");
              }
#else
              if (ghdi_control_umts_voice(GHDI_TURN_AMR_ON,
                    current_rat) == GHDI_FAILURE)
              {
                    MSG_HIGH_DS_0(MN_SUB,"=MNCNM= Unable to switch ON AMR Speech vocoder");
              }
#endif  /* end of FEATURE_VOC_AMR_WB */
           }
           else
           {
              MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: UE not in WCDMA nor GSM mode");
           }
           
           tch_user_connections_store[DEFAULT_RAB_ID].speech_channel_connected = TRUE;
           
        }  /* ! = ITC_SPEECH */
  
        /********************/
        /*  DATA setup      */
        /********************/
        else
        {
           /* now update the data channel configuration */
  
           /*************/
           /* GSM MODE  */
           /*************/
           if (  MNCNM_is_gsm_mode(local_as_id) )
           {
              /*********************/
              /*    SUBCHANNEL 0   */
              /*********************/
              if (tch_user_connections_store[DEFAULT_RAB_ID].subchannel == 0)
              {
                 /****************/
                 /* FULL RATE    */
                 /****************/
                 if (tch_user_connections_store[DEFAULT_RAB_ID].tch_rate == TCH_FULL_RATE)
  
                 {
                    if (ghdi_control_subs_tch_gating(GHDI_TURN_DATA_TCH_F_ON,
                                        tch_user_connections_store[DEFAULT_RAB_ID].channel_mode,
                                        mn_asid_2_vsid_map[local_as_id],
                                        local_as_id
                                               ) == 
                                        GHDI_FAILURE)
                    {
                        MSG_HIGH_DS_0(MN_SUB,"=MNCNM= Unable to configure to ON FR voc for Data call");
  
                    }
                 }
                 /****************/
                 /* HALF RATE    */
                 /****************/
                 else
                 {
                    if (ghdi_control_subs_tch_gating(GHDI_TURN_DATA_TCH_H_0_ON,
                                        tch_user_connections_store[DEFAULT_RAB_ID].channel_mode,
                                        mn_asid_2_vsid_map[local_as_id],
                                        local_as_id
                                               ) == 
                                        GHDI_FAILURE)
                    {
                        MSG_HIGH_DS_0(MN_SUB,"=MNCNM= Unable to configure to ON HR 0 voc for Data call");
                    }
                 }
              }
              /*********************/
              /*  SUBCHANNEL  !=0  */
              /*********************/
              else
              {
                 /****************/
                 /* HALF RATE    */
                 /****************/
                 if (ghdi_control_subs_tch_gating(GHDI_TURN_DATA_TCH_H_1_ON,
                                     tch_user_connections_store[DEFAULT_RAB_ID].channel_mode,
                                         mn_asid_2_vsid_map[local_as_id],
                                         local_as_id
                                            ) == 
                                     GHDI_FAILURE)
                 {
                    MSG_HIGH_DS_0(MN_SUB,"=MNCNM= Unable to configure to ON HR 1 voc for Data call");
                 }
              }
           } /* if is_cs_gsm_mode...*/
  
           /***************/
           /* WCDMA MODE  */
           /***************/
           else if (MNCNM_is_umts_mode(local_as_id))
           {       /* Do nothing... RRC will take care of this 
                                                   i.e. CS DATA ON */
               MSG_MED_DS_0(MN_SUB,"=MNCNM= Turn CS DATA Transfer ON");
           }
           else
           {
               MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Invalid Operation Mode !(WCDMA nor GSM)");
           }
  
           tch_user_connections_store[DEFAULT_RAB_ID].data_channel_connected = TRUE;
  
        }  /* end else data setup...*/
  
     } /* end if OTA channel is connected */
     else
     {
        MSG_HIGH_DS_1(MN_SUB,"=MNCNM= Either VOC has not been acquired = %d ? or OTA Channel not connected", MN_turned_on_vocoder);
     }
  } 
  else
  {
    MSG_HIGH_DS_1(MN_SUB,"=MNCNM= invalid as_id =%d", local_as_id);
  }
}


