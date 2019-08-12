/*===========================================================================
                        MN    U T I L S

DESCRIPTION

   This file contains the Utility functions for MN task. 

EXTERNALIZED FUNCTIONS   
              
INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2001 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mn/vcs/mnutils.c_v   1.8   10 Jul 2002 14:11:38   cdealy  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mn/src/mnutils.c#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
9/5/01     cd      Added functions for QXDM logging

9/13/01    cd      Added function mn_log_message

9/18/01    cd      Added queue id to mn_log_message

9/26/01    cd      Added function rrcmn_verify_rab, formerly in mnlib.c.  
                   For now, set rab_id to DEFAULT_RAB_ID to avoid compile warning.

                   Added customer.h
   
10/01/01    cd     Added call -> CNM_log_rp_mti
                   Added case MS_TIMER in mn_log_message

10/31/01    CD     Fixed ADS compile error

2/19/02     CD     Changed log packet names from WCDMA_xxxx_PACKET to 
                   LOG_UMTS_xxxx_PACKET_C

22-Mar-2002  VT    In mn_log_cs_call_setup_info(), now filling 
                     calling_party_bcd_length and called_party_bcd_length,
                     in the call_info_pkt instead of calling_party_bcd_present 
                     and called_party_bcd_present (respectively), which are
                     obsolete now.

07/10/02    CD    Replaced include cmll.h by cmwll.h

08/14/02    AB    Optimize log comments by changing the following: 
                  "sent" & "received", to "->" to gain more display info.

10/12/02    AB    Added CM_MO_END_X_CALL_REQ to message log.

01/14/04    AB    Added new MN to CM primitives, CM_CS_SIG_REL_IND.

05/12/05    HS    Added support for FEATURE_CCBS

09/14/05    HS    Fixed typo in MSG_LOW()
08/14/06    PK    Updated to log CFA messages for CM->MN.

07/06/11    PM    Guarding against NULL pointer access

11/02/11    PM    Changes made for a feature where Voc gets released when
                  call is put on HOLD and Voc gets acquired when call is retrieved.
===========================================================================*/


/* ==========================================================================
** Includes and Variable Definitions
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */
#include "mmcp_variation.h"
#include "customer.h"
#include "mn.h"
#include "mm.h"
#include "mnglobal.h"
#include "naslog_v.h"
#include "memory.h"
#include "msg.h"
#include "ms.h"
#include "cfa_v.h"
/* ----------------------------------------------------------------------- 
** REMOVED compiler warning from inclusion of memheap.h 
** ----------------------------------------------------------------------- */
#ifdef FEATURE_MEM_DEBUG
#undef FEATURE_MEM_DEBUG
#endif
#include "cmwll.h"
#include "cnm.h"
#ifdef FEATURE_WCDMA
#include "rrcmnif.h"
#endif
#include "ds_cap_v.h"
#include <stringl/stringl.h>


#ifdef FEATURE_DUAL_SIM
extern boolean mm_dual_standby_mode;
#endif


#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
extern boolean MN_turned_on_vocoder_sim[MAX_AS_IDS];
#define MN_turned_on_vocoder (MN_turned_on_vocoder_sim[mn_dsda_as_id])
#else
extern boolean MN_turned_on_vocoder;
#endif



byte mn_nv_no_auto_answer_on_hold = 0;
byte mn_nv_mt_call_reject_cause = 0;
byte mn_als_itc_aux_speech = 0;

#define ITC_AUX_SPEECH_FOUR 0x04
#define ITC_AUX_SPEECH_SIX  0x06

/*===========================================================================
FUNCTION MN_LOG_CS_CALL_SETUP_INFO

DESCRIPTION
  This function sends the CS call setup info log packet to DIAG for QXDM.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean mn_log_cs_call_setup_info(uint8 connection_id, uint8 call_type, uint8 orig_direction)
{
   word pkt_size;
   LOG_UMTS_CS_CALL_SETUP_INFO_LOG_PACKET_C_type * call_info_pkt;

   call_info_pkt = NULL;
   pkt_size = 0;

#if defined(FEATURE_DUAL_SIM )
   if ( (MNCNM_IS_SINGLE_STACK) && mm_dual_standby_mode )
   {
     LOG_UMTS_DS_CS_CALL_SETUP_INFO_LOG_PACKET_C_type *ds_log_ptr;   
     ds_log_ptr = (LOG_UMTS_DS_CS_CALL_SETUP_INFO_LOG_PACKET_C_type *)
                log_alloc(LOG_UMTS_DS_CS_CALL_SETUP_INFO_LOG_PACKET_C,
                          sizeof(LOG_UMTS_DS_CS_CALL_SETUP_INFO_LOG_PACKET_C_type));

     if (ds_log_ptr == NULL)
     {
       MSG_LOW_DS_0(MN_SUB,"=MNCNM= Unable to get a LOG packet pointer");
       return FALSE;
     }

     ds_log_ptr->as_id = (uint8) mn_as_id;

     if(cc_transaction_information_p[connection_id] == NULL)
     {
       MSG_LOW_DS_1(MN_SUB,"=MNCNM= cc_transaction_information_p[%d] is NULL",connection_id);
       return FALSE;
     }
     if(mn_call_information_p[connection_id] == NULL)
     {
       MSG_LOW_DS_1(MN_SUB,"=MNCNM= mn_call_information_p[%d] is NULL",connection_id);
       return FALSE;
     }
     // Pack the Log packet data and commit to the DIAG.
     ds_log_ptr->cs_connection_info.connection_id = connection_id;

     ds_log_ptr->cs_connection_info.stream_id = 
           mn_call_information_p[connection_id]->radio_access_bearer_id;

     ds_log_ptr->cs_connection_info.cc_state = 
           cc_transaction_information_p[connection_id]->cc_state;

     ds_log_ptr->cs_connection_info.mpty_state = 
           cc_transaction_information_p[connection_id]->mpty_auxiliary_state;

     ds_log_ptr->cs_connection_info.hold_state = 
         cc_transaction_information_p[connection_id]->hold_auxiliary_state;

     ds_log_ptr->cs_connection_info.call_type = call_type;

     ds_log_ptr->cs_connection_info.origination_direction = orig_direction;

     if (ds_log_ptr->cs_connection_info.origination_direction == CN_TO_UE_DIRECTION)
     {
       if (mn_call_information_p[connection_id]->calling_party_bcd_number_present == TRUE)
       {
          ds_log_ptr->cs_connection_info.calling_party_bcd.calling_party_bcd_length = 
             mn_call_information_p[connection_id]->calling_party_bcd_number.length;

          memscpy((void *)ds_log_ptr->cs_connection_info.calling_party_bcd.calling_party_bcd,
                         sizeof(ds_log_ptr->cs_connection_info.calling_party_bcd.calling_party_bcd),
                         mn_call_information_p[connection_id]->calling_party_bcd_number.data,
                         mn_call_information_p[connection_id]->calling_party_bcd_number.length);
       }
       else
       {
           ds_log_ptr->cs_connection_info.calling_party_bcd.calling_party_bcd_length = NULL;
       }
     }
     else /* origination direction == UE_TO_CN_DIRECTION */
     {
       if (mn_call_information_p[connection_id]->called_party_bcd_number_present == TRUE)
       {
          ds_log_ptr->cs_connection_info.called_party_bcd.called_party_bcd_length = 
             mn_call_information_p[connection_id]->called_party_bcd_number.length;

          memscpy((void *)ds_log_ptr->cs_connection_info.called_party_bcd.called_party_bcd,
             sizeof(ds_log_ptr->cs_connection_info.called_party_bcd.called_party_bcd),
             mn_call_information_p[connection_id]->called_party_bcd_number.data,
             mn_call_information_p[connection_id]->called_party_bcd_number.length);
       }
       else
       {
          ds_log_ptr->cs_connection_info.called_party_bcd.called_party_bcd_length = NULL;
       }
     }

     log_commit(ds_log_ptr);

   }
   else
#endif
   {
     pkt_size = sizeof(LOG_UMTS_CS_CALL_SETUP_INFO_LOG_PACKET_C_type);
     call_info_pkt = (LOG_UMTS_CS_CALL_SETUP_INFO_LOG_PACKET_C_type *)
                      log_alloc(LOG_UMTS_CS_CALL_SETUP_INFO_LOG_PACKET_C, pkt_size);

     if (call_info_pkt == NULL)
     {
       MSG_LOW_DS_0(MN_SUB,"=MNCNM= Unable to get a LOG packet pointer");
       return FALSE;
     }
     if(cc_transaction_information_p[connection_id] == NULL)
     {
       MSG_LOW_DS_1(MN_SUB,"=MNCNM= cc_transaction_information_p[%d] is NULL",connection_id);
       return FALSE;
     }
     if(mn_call_information_p[connection_id] == NULL)
     {
       MSG_LOW_DS_1(MN_SUB,"=MNCNM= mn_call_information_p[%d] is NULL",connection_id);
       return FALSE;
     }
     // Pack the Log packet data and commit to the DIAG.
     call_info_pkt->cs_connection_info.connection_id = connection_id;

     call_info_pkt->cs_connection_info.stream_id = 
           mn_call_information_p[connection_id]->radio_access_bearer_id;

     call_info_pkt->cs_connection_info.cc_state = 
         cc_transaction_information_p[connection_id]->cc_state;

     call_info_pkt->cs_connection_info.mpty_state = 
         cc_transaction_information_p[connection_id]->mpty_auxiliary_state;

     call_info_pkt->cs_connection_info.hold_state = 
         cc_transaction_information_p[connection_id]->hold_auxiliary_state;

     call_info_pkt->cs_connection_info.call_type = call_type;

     call_info_pkt->cs_connection_info.origination_direction = orig_direction;

     if (call_info_pkt->cs_connection_info.origination_direction == CN_TO_UE_DIRECTION)
     {
       if (mn_call_information_p[connection_id]->calling_party_bcd_number_present == TRUE)
       {
         call_info_pkt->cs_connection_info.calling_party_bcd.calling_party_bcd_length = 
         mn_call_information_p[connection_id]->calling_party_bcd_number.length;

         memscpy((void *)call_info_pkt->cs_connection_info.calling_party_bcd.calling_party_bcd,
                       sizeof(call_info_pkt->cs_connection_info.calling_party_bcd.calling_party_bcd),
                       mn_call_information_p[connection_id]->calling_party_bcd_number.data,
                       mn_call_information_p[connection_id]->calling_party_bcd_number.length);
      }
      else
      {
         call_info_pkt->cs_connection_info.calling_party_bcd.calling_party_bcd_length = NULL;
      }
     }
     else /* origination direction == UE_TO_CN_DIRECTION */
     {
       if (mn_call_information_p[connection_id]->called_party_bcd_number_present == TRUE)
       {
          call_info_pkt->cs_connection_info.called_party_bcd.called_party_bcd_length = 
           mn_call_information_p[connection_id]->called_party_bcd_number.length;

          memscpy((void *)call_info_pkt->cs_connection_info.called_party_bcd.called_party_bcd,
           sizeof(call_info_pkt->cs_connection_info.called_party_bcd.called_party_bcd),
           mn_call_information_p[connection_id]->called_party_bcd_number.data,
           mn_call_information_p[connection_id]->called_party_bcd_number.length);
        }
       else
       {
        call_info_pkt->cs_connection_info.called_party_bcd.called_party_bcd_length = NULL;
       }
     }

     log_commit(call_info_pkt);
   }

  return TRUE;
}

/*===========================================================================
FUNCTION MN_LOG_CS_CALL_RELEASE_INFO

DESCRIPTION
  This function sends the CS call releasep info log packet to DIAG for QXDM.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean mn_log_cs_call_release_info(uint8 connection_id, uint8 release_cause)
{
   word pkt_size;
   LOG_UMTS_CS_CALL_RELEASE_INFO_LOG_PACKET_C_type * call_info_pkt;
   call_info_pkt = NULL;
   pkt_size = 0;

#if defined(FEATURE_DUAL_SIM )
     if((MNCNM_IS_SINGLE_STACK) && mm_dual_standby_mode)
     {
       LOG_UMTS_DS_CS_CALL_RELEASE_INFO_LOG_PACKET_C_type *ds_log_ptr;     
       ds_log_ptr = (LOG_UMTS_DS_CS_CALL_RELEASE_INFO_LOG_PACKET_C_type *)
                log_alloc(LOG_UMTS_DS_CS_CALL_RELEASE_INFO_LOG_PACKET_C,
                          sizeof(LOG_UMTS_DS_CS_CALL_RELEASE_INFO_LOG_PACKET_C_type));

       if (ds_log_ptr == NULL)
       {
         MSG_LOW_DS_0(MN_SUB,"=MNCNM= Unable to get a LOG packet pointer");
         return FALSE;
       }


       // Pack the Log packet data and commit to the DIAG.
       ds_log_ptr->as_id = (uint8) mn_as_id;
       ds_log_ptr->connection_id = connection_id;
       ds_log_ptr->release_cause = release_cause;

       log_commit(ds_log_ptr);
     }
     else
#endif
     {
     pkt_size = sizeof(LOG_UMTS_CS_CALL_RELEASE_INFO_LOG_PACKET_C_type);
     call_info_pkt = (LOG_UMTS_CS_CALL_RELEASE_INFO_LOG_PACKET_C_type *)
                    log_alloc(LOG_UMTS_CS_CALL_RELEASE_INFO_LOG_PACKET_C, pkt_size);

     if (call_info_pkt == NULL)
     {
       MSG_LOW_DS_0(MN_SUB,"=MNCNM= Unable to get a LOG packet pointer");
       return FALSE;
     }
    
     // Pack the Log packet data and commit to the DIAG.
     call_info_pkt->connection_id = connection_id;
     call_info_pkt->release_cause = release_cause;

     log_commit(call_info_pkt);
     }
   MSG_LOW_DS_0(MN_SUB,"=MNCNM= Logging LOG_UMTS_CS_CALL_RELEASE_INFO_LOG_PACKET_C");

  return TRUE;
}

/*===========================================================================
FUNCTION MN_LOG_CS_CALL_CHANGE_INFO

DESCRIPTION
  This function sends the CS call change info log packet to DIAG for QXDM.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean mn_log_cs_call_change_info(uint8 connection_id)
{
   word pkt_size;
   LOG_UMTS_CS_CALL_CHANGE_INFO_LOG_PACKET_C_type * call_info_pkt;
   call_info_pkt = NULL;
   pkt_size = 0;

#if defined(FEATURE_DUAL_SIM )
     if((MNCNM_IS_SINGLE_STACK) && mm_dual_standby_mode)
     {
       LOG_UMTS_DS_CS_CALL_CHANGE_INFO_LOG_PACKET_C_type *ds_log_ptr;     
       ds_log_ptr = (LOG_UMTS_DS_CS_CALL_CHANGE_INFO_LOG_PACKET_C_type *)
                log_alloc(LOG_UMTS_DS_CS_CALL_CHANGE_INFO_LOG_PACKET_C,
                          sizeof(LOG_UMTS_DS_CS_CALL_CHANGE_INFO_LOG_PACKET_C_type));

       if (ds_log_ptr == NULL)
       {
         MSG_LOW_DS_0(MN_SUB,"=MNCNM= Unable to get a LOG packet pointer");
         return FALSE;
       }

       // Pack the Log packet data and commit to the DIAG.
       ds_log_ptr->as_id = (uint8) mn_as_id;
       ds_log_ptr->connection_id = connection_id;

       if(cc_transaction_information_p[connection_id] != NULL)
       {
         ds_log_ptr->mpty_state = 
           cc_transaction_information_p[connection_id]->mpty_auxiliary_state;

         ds_log_ptr->hold_state = 
           cc_transaction_information_p[connection_id]->hold_auxiliary_state;
       }
       else
       {
         MSG_LOW_DS_0(MN_SUB,"=MNCNM= cc_transaction_information_p is NULL");
       }
       log_commit(ds_log_ptr);
     }
     else
#endif
     {
     pkt_size = sizeof(LOG_UMTS_CS_CALL_CHANGE_INFO_LOG_PACKET_C_type);
     call_info_pkt = (LOG_UMTS_CS_CALL_CHANGE_INFO_LOG_PACKET_C_type *)
                    log_alloc(LOG_UMTS_CS_CALL_CHANGE_INFO_LOG_PACKET_C, pkt_size);

     if (call_info_pkt == NULL)
     {
       MSG_LOW_DS_0(MN_SUB,"=MNCNM= Unable to get a LOG packet pointer");
       return FALSE;
     }
    
     // Pack the Log packet data and commit to the DIAG.
     call_info_pkt->connection_id = connection_id;

     if(cc_transaction_information_p[connection_id] != NULL)
     {
       call_info_pkt->mpty_state = 
           cc_transaction_information_p[connection_id]->mpty_auxiliary_state;

       call_info_pkt->hold_state = 
           cc_transaction_information_p[connection_id]->hold_auxiliary_state;
     }
     else
     {
       MSG_LOW_DS_0(MN_SUB,"=MNCNM= cc_transaction_information_p is NULL");
     }
     log_commit(call_info_pkt);
    }
   MSG_LOW_DS_0(MN_SUB,"=MNCNM= Logging LOG_UMTS_CS_CALL_CHANGE_INFO_LOG_PACKET_C");

  return TRUE;
}

/*===========================================================================
FUNCTION MN_LOG_CS_CONNECTION_BC_INFO

DESCRIPTION
  This function sends the CS connection bearer capability info log packet to DIAG for QXDM.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean mn_log_cs_connection_bc_info(uint8 connection_id, cm_bearer_capability_T bearer_capability)
{
  word pkt_size;
  LOG_UMTS_NAS_CS_CONNECTION_BC_LOG_PACKET_C_type * bc_pkt;
  bc_pkt = NULL;
  pkt_size = 0;

#if defined(FEATURE_DUAL_SIM )
  if((MNCNM_IS_SINGLE_STACK) && mm_dual_standby_mode)
  {
   LOG_UMTS_DS_NAS_CS_CONNECTION_BC_LOG_PACKET_C_type *ds_log_ptr;  
   ds_log_ptr = (LOG_UMTS_DS_NAS_CS_CONNECTION_BC_LOG_PACKET_C_type *)
              log_alloc(LOG_UMTS_DS_NAS_CS_CONNECTION_BC_LOG_PACKET_C,
                        sizeof(LOG_UMTS_DS_NAS_CS_CONNECTION_BC_LOG_PACKET_C_type));

    if (ds_log_ptr == NULL)
    {
      MSG_LOW_DS_0(MN_SUB,"=MNCNM= Unable to get a LOG packet pointer");
      return FALSE;
    }

    ds_log_ptr->as_id = (uint8) mn_as_id;

    ds_log_ptr->connection_id = connection_id;
    ds_log_ptr->radio_channel_requirement = bearer_capability.radio_channel_requirement;
    ds_log_ptr->information_transfer_capability = bearer_capability.information_transfer_capability;
    ds_log_ptr->structure = bearer_capability.structure;
    ds_log_ptr->duplex_mode = bearer_capability.duplex_mode;
    ds_log_ptr->rate_adaption = bearer_capability.rate_adaption;
    ds_log_ptr->signalling_access_protocol = bearer_capability.signalling_access_protocol;
    ds_log_ptr->sync_async = bearer_capability.sync_async;
    ds_log_ptr->number_of_data_bits = bearer_capability.number_of_data_bits;
    ds_log_ptr->number_of_stop_bits = bearer_capability.number_of_stop_bits;
    ds_log_ptr->user_rate = bearer_capability.user_rate;
    ds_log_ptr->parity = bearer_capability.parity;
    ds_log_ptr->modem_type = bearer_capability.modem_type;
    ds_log_ptr->connection_element = bearer_capability.connection_element;
    ds_log_ptr->user_information_layer_2_protocol = 
            bearer_capability.user_information_layer_2_protocol;
    ds_log_ptr->coding_standard = bearer_capability.coding_standard;
    ds_log_ptr->transfer_mode = bearer_capability.transfer_mode;
    ds_log_ptr->configuration = bearer_capability.configuration;
    ds_log_ptr->NIRR = bearer_capability.NIRR;
    ds_log_ptr->establishment = bearer_capability.establishment;
    ds_log_ptr->user_information_layer_1_protocol = 
            bearer_capability.user_information_layer_1_protocol;
    ds_log_ptr->negotiation = bearer_capability.negotiation;
    ds_log_ptr->intermediate_rate = bearer_capability.intermediate_rate;
    ds_log_ptr->NIC_on_Tx = bearer_capability.NIC_on_Tx;
    ds_log_ptr->NIC_on_Rx = bearer_capability.NIC_on_Rx;
    ds_log_ptr->other_modem_type = bearer_capability.other_modem_type;
    ds_log_ptr->fixed_network_user_rate = bearer_capability.fixed_network_user_rate;
    ds_log_ptr->acceptable_channel_codings = bearer_capability.acceptable_channel_codings;
    ds_log_ptr->wanted_air_interface_user_rate = bearer_capability.wanted_air_interface_user_rate;
    ds_log_ptr->mt_fallback_rate = bearer_capability.mt_fall_back_rate;

    log_commit(ds_log_ptr);

  }
  else
#endif
    {
    pkt_size = sizeof(LOG_UMTS_NAS_CS_CONNECTION_BC_LOG_PACKET_C_type);
    bc_pkt = (LOG_UMTS_NAS_CS_CONNECTION_BC_LOG_PACKET_C_type *)
                    log_alloc(LOG_UMTS_NAS_CS_CONNECTION_BC_LOG_PACKET_C, pkt_size);

    if (bc_pkt == NULL)
    {
      MSG_LOW_DS_0(MN_SUB,"=MNCNM= Unable to get a LOG packet pointer");
      return FALSE;
    }

    bc_pkt->connection_id = connection_id;
    bc_pkt->radio_channel_requirement = bearer_capability.radio_channel_requirement;
    bc_pkt->information_transfer_capability = bearer_capability.information_transfer_capability;
    bc_pkt->structure = bearer_capability.structure;
    bc_pkt->duplex_mode = bearer_capability.duplex_mode;
    bc_pkt->rate_adaption = bearer_capability.rate_adaption;
    bc_pkt->signalling_access_protocol = bearer_capability.signalling_access_protocol;
    bc_pkt->sync_async = bearer_capability.sync_async;
    bc_pkt->number_of_data_bits = bearer_capability.number_of_data_bits;
    bc_pkt->number_of_stop_bits = bearer_capability.number_of_stop_bits;
    bc_pkt->user_rate = bearer_capability.user_rate;
    bc_pkt->parity = bearer_capability.parity;
    bc_pkt->modem_type = bearer_capability.modem_type;
    bc_pkt->connection_element = bearer_capability.connection_element;
    bc_pkt->user_information_layer_2_protocol = 
          bearer_capability.user_information_layer_2_protocol;
    bc_pkt->coding_standard = bearer_capability.coding_standard;
    bc_pkt->transfer_mode = bearer_capability.transfer_mode;
    bc_pkt->configuration = bearer_capability.configuration;
    bc_pkt->NIRR = bearer_capability.NIRR;
    bc_pkt->establishment = bearer_capability.establishment;
    bc_pkt->user_information_layer_1_protocol = 
          bearer_capability.user_information_layer_1_protocol;
    bc_pkt->negotiation = bearer_capability.negotiation;
    bc_pkt->intermediate_rate = bearer_capability.intermediate_rate;
    bc_pkt->NIC_on_Tx = bearer_capability.NIC_on_Tx;
    bc_pkt->NIC_on_Rx = bearer_capability.NIC_on_Rx;
    bc_pkt->other_modem_type = bearer_capability.other_modem_type;
    bc_pkt->fixed_network_user_rate = bearer_capability.fixed_network_user_rate;
    bc_pkt->acceptable_channel_codings = bearer_capability.acceptable_channel_codings;
    bc_pkt->wanted_air_interface_user_rate = bearer_capability.wanted_air_interface_user_rate;
    bc_pkt->mt_fallback_rate = bearer_capability.mt_fall_back_rate;
  //  bc_pkt->speech_version_indication = bearer_capability.speech_version_indication;

    log_commit(bc_pkt);
    }
  MSG_LOW_DS_0(MN_SUB,"=MNCNM= Logging LOG_UMTS_NAS_CS_CONNECTION_BC_LOG_PACKET_C");

  return TRUE;
}

/*******************************************************************************
 *
 *  Function name: mn_log_message
 *  ------------------------------
 *  Description:  This function logs the receipt or sending of intertask
 *  ------------  messages for the MN subtask.
 *                It bases the subtask on the message_set, and the message direction
 *                on the message_id.
 *  
 *                Intertask messages are logged as MSG_HIGH when received and as 
 *                MSG_HIGH when sent.
 *
 *  Parameters:
 *  -----------
 *  IN:  message_p      pointer to the message
 *
 *  OUT: (none)
 *
 *  Returns:  void
 *  --------
 *
 ******************************************************************************/

VOID_FUNC mn_log_message(gs_queue_id_T queue_id, IMH_T *message_p)
{
#ifdef FEATURE_DUAL_SIM
   sys_modem_as_id_e_type local_as_id = SYS_MODEM_AS_ID_NONE;
#endif

   /*
   ** MESSAGE SET ?
   */ 

   switch( message_p->message_set)
   {

      case MS_MN_CM:

         /*
         ** MESSAGE ID ?
         */

         /*
         **
         ** If incoming message, message_id is enumerated based on message group
         ** see mn_cm.h
         **
         ** else if outgoing message, message_id enumeration is defined in cmwll.h
         **
         ** Note:  To avoid compile errors due to message_id values, it is necessary 
         ** to break a switch statement for message_id into two statements
         **
         **/

         /* 
         ** First go through incoming messages 
         */
#ifdef FEATURE_DUAL_SIM
         if (queue_id == GS_QUEUE_MN_CM)
         {
            word message_length;
            /*    Get message length */
            GET_IMH_LEN(message_length, message_p); 
            /*    Add in header size */
            message_length +=  sizeof(IMH_T);
            /* Log using CFA packet */                
            cfa_log_packet( GS_QUEUE_MN_CM, message_length, (byte*)message_p );

            switch( message_p->message_id )
            {

               case CM_MO_NORMAL_CALL_REQ:
                  local_as_id = ((cm_mo_normal_call_req_T *)message_p)->as_id;
                  break;

               case CM_MO_EMERGENCY_CALL_REQ:
                  local_as_id = ((cm_mo_emergency_call_req_T *)message_p)->as_id;
                  break;

               case CM_START_DTMF_REQ:
                  local_as_id = ((cm_start_dtmf_req_T *)message_p)->as_id;
                  break;

               case CM_STOP_DTMF_REQ:
                  local_as_id = ((cm_stop_dtmf_req_T *)message_p)->as_id;
                  break;

               case CM_0_SEND_REQ:
                  local_as_id = ((cm_n_send_req_T *)message_p)->as_id;
                  break;

               case CM_1_SEND_REQ:
                  local_as_id = ((cm_n_send_req_T *)message_p)->as_id;
                  break;

               case CM_1_X_SEND_REQ:
                  local_as_id = ((cm_n_x_send_req_T *)message_p)->as_id;
                  break;

               case CM_2_SEND_REQ:
                  local_as_id = ((cm_n_send_req_T *)message_p)->as_id;
                  break;

               case CM_2_X_SEND_REQ:
                  local_as_id = ((cm_n_x_send_req_T *)message_p)->as_id;
                  break;

               case CM_3_SEND_REQ:
                  local_as_id = ((cm_n_send_req_T *)message_p)->as_id;
                  break;

               case CM_4_SEND_REQ:
                  local_as_id = ((cm_n_send_req_T *)message_p)->as_id;
                  break;

               case CM_MO_END_CALL_REQ:
                  local_as_id = ((cm_mo_end_call_req_T *)message_p)->as_id;
                  break;

               case CM_MO_END_X_CALL_REQ:
                  local_as_id = ((cm_n_x_send_req_T *)message_p)->as_id;
                  break;

               case CM_ASID_2_VSID_MAPPING_IND:
                  break;

#ifdef FEATURE_UUS
               case CM_MO_USER_DATA_REQ:
                  local_as_id = ((cm_mo_user_data_req_T *)message_p)->as_id;
                  break;
#endif

               case CM_MO_MODIFY_REQ:
                  local_as_id = ((cm_mo_modify_req_T *)message_p)->as_id;
                  break;

               case CM_ABORT_CC_REQ:
                  local_as_id = ((cm_abort_cc_req_T *)message_p)->as_id;
                  break;

               case CM_RELEASE_SS_REQ:
                  local_as_id = ((cm_release_ss_req_T *)message_p)->as_id;
                  break;

               case CM_ACTIVATE_SS_REQ:
                  local_as_id = ((cm_activate_ss_req_T *)message_p)->as_id;
                  break;

               case CM_DEACTIVATE_SS_REQ:
                  local_as_id = ((cm_deactivate_ss_req_T *)message_p)->as_id;
                  break;

               case CM_ERASE_SS_REQ:
                  local_as_id = ((cm_erase_ss_req_T *)message_p)->as_id;
                  break;

               case CM_INTERROGATE_SS_REQ:
                  local_as_id = ((cm_interrogate_ss_req_T *)message_p)->as_id;
                  break;

               case CM_REGISTER_PASSWORD_REQ:
                  local_as_id = ((cm_register_password_req_T *)message_p)->as_id;
                  break;

               case CM_REGISTER_SS_REQ:
                  local_as_id = ((cm_register_ss_req_T *)message_p)->as_id;
                  break;

               case CM_PROCESS_UNSTRUCTURED_SS_REQ:
                  local_as_id = ((cm_process_unstructured_ss_req_T *)message_p)->as_id;
                  break;

               case CM_ABORT_SS_REQ:
                  local_as_id = ((cm_abort_ss_req_T *)message_p)->as_id;
                  break;

               case CM_PROCESS_UNSTRUCTURED_SS_DATA:
                  local_as_id = ((cm_process_unstructured_ss_data_T *)message_p)->as_id;
                  break;

#if defined ( FEATURE_CM_MMGPS ) || defined ( FEATURE_CM_LCS )
               case CM_LCS_MOLR_REQ:
                  local_as_id = ((cm_lcs_molr_req_T *)message_p)->as_id;
                  break;

               case CM_LCS_LOCATION_NOTIFICATION_RES:
                  local_as_id = ((cm_lcs_location_notification_res_T *)message_p)->as_id;
                  break;

               case CM_LCS_MOLR_COMPLETED:
                  break;
#endif

               case CM_MT_CALL_RES:
                  local_as_id = ((cm_mt_call_res_T *)message_p)->as_id;
                  break;

               case CM_MT_SETUP_RES:
                  local_as_id = ((cm_mt_setup_res_T *)message_p)->as_id;
                  break;

               case CM_MT_MODIFY_RES:
                  local_as_id = ((cm_mt_modify_res_T *)message_p)->as_id;
                  break;

               case CM_GET_PASSWORD_RES:
                  local_as_id = ((cm_get_password_res_T *)message_p)->as_id;
                  break;

               case CM_UNSTRUCTURED_SS_NOTIFY_RES:
                  local_as_id = ((cm_unstructured_ss_notify_res_T *)message_p)->as_id;
                  break;

               case CM_UNSTRUCTURED_SS_RES:
                  local_as_id = ((cm_unstructured_ss_res_T *)message_p)->as_id;
                  break;

               case CM_MT_CALL_SS_RES:
                  local_as_id = ((cm_mt_call_ss_res_T *)message_p)->as_id;
                  
                  break;

               default:
                  break;

             }/* end switch(message_p->message_id) */
             MSG_HIGH_DS_4(MN_SUB,"=MNCNM= MS:0x%X   MSG_ID:0x%X   QUEUE_ID : %d  AS_ID_%d",message_p->message_set,message_p->message_id,queue_id,local_as_id+1);
         }
         else if (queue_id == GS_QUEUE_CM)
         {
         
         /*
         * If it is an outgoing message 
         */
            switch(message_p->message_id)
            {
               case CM_MO_CALL_CONF:
                 local_as_id = ((cm_mo_call_conf_T *)message_p)->as_id;
                  break;

               case CM_START_DTMF_CONF:
                 local_as_id = ((cm_start_dtmf_conf_T *)message_p)->as_id;
                  break;

               case CM_STOP_DTMF_CONF:
                  local_as_id = ((cm_stop_dtmf_conf_T *)message_p)->as_id;
                  break;

               case CM_MANAGE_CALLS_CONF:
                  local_as_id = ((cm_manage_calls_conf_T *)message_p)->as_id;
                  break;

               case CM_MODIFY_COMPLETE_CONF:
                  local_as_id = ((cm_modify_complete_conf_T *)message_p)->as_id;
                  break;

               case CM_ABORT_CC_CONF:
                  local_as_id = ((cm_abort_cc_conf_T *)message_p)->as_id;
                  break;

               case CM_ACTIVATE_SS_CONF:
                  local_as_id = ((cm_activate_ss_conf_T *)message_p)->as_id;
                  break;

               case CM_DEACTIVATE_SS_CONF:
                  local_as_id = ((cm_deactivate_ss_conf_T *)message_p)->as_id;
                  break;

               case CM_ERASE_SS_CONF:
                  local_as_id = ((cm_erase_ss_conf_T *)message_p)->as_id;
                  break;

               case CM_INTERROGATE_SS_CONF:
                  local_as_id = ((cm_interrogate_ss_conf_T *)message_p)->as_id;
                  break;

               case CM_REGISTER_PASSWORD_CONF:
                  local_as_id = ((cm_register_password_conf_T *)message_p)->as_id;
                  break;

               case CM_REGISTER_SS_CONF:
                 local_as_id = ((cm_register_ss_conf_T *)message_p)->as_id;
                  break;

               case CM_PROCESS_UNSTRUCTURED_SS_CONF:
                  local_as_id = ((cm_process_unstructured_ss_conf_T *)message_p)->as_id;
                  break;

               case CM_PROCESS_UNSTRUCTURED_SS_DATA_CONF:
                  local_as_id = ((cm_process_unstructured_ss_data_T *)message_p)->as_id;
                  break;

               case CM_MT_CALL_IND:
                  local_as_id = ((cm_mt_call_ind_T *)message_p)->as_id;
                  break;

               case CM_CALL_PROGRESS_INFO_IND:
                 local_as_id = ((cm_call_progress_info_ind_T *)message_p)->as_id;
                  break;

               case CM_MO_CALL_ALERT_IND:
                  local_as_id = ((cm_mo_call_alert_ind_T *)message_p)->as_id;
                  break;
                      
               case CM_CALL_CONNECTED_IND:
                  local_as_id = ((cm_call_connected_ind_T *)message_p)->as_id;
                  break;

               case CM_CONNECTED_NUMBER_IND:
                 local_as_id = ((cm_connected_number_ind_T *)message_p)->as_id;
                  break;

               case CM_MT_END_CALL_IND:
                  local_as_id = ((cm_mt_end_call_ind_T *)message_p)->as_id;
                  break;

#ifdef FEATURE_UUS
               case CM_MT_USER_DATA_IND:
                  local_as_id = ((cm_mt_user_data_ind_T *)message_p)->as_id ;
                  break;

               case CM_USER_CONGESTION_IND:
                  local_as_id = ((cm_mt_user_data_ind_T *)message_p)->as_id;
                  break;
#endif

               case CM_MT_SETUP_IND:
                 local_as_id = ((cm_mt_setup_ind_T *)message_p)->as_id;
                  break;

               case CM_MT_MODIFY_IND:
                  local_as_id = ((cm_mt_modify_ind_T *)message_p)->as_id;
                  break;

               case CM_MT_DISC_IND:
                  local_as_id = ((cm_mt_disc_ind_T *)message_p)->as_id;
                  break;

               case CM_MT_CALL_SS_IND:
                 local_as_id = ((cm_mt_call_ss_ind_T *)message_p)->as_id;
                  break;

               case CM_FORWARD_CHECK_SS_INDICATION_IND:
                 local_as_id = ((cm_forward_check_ss_ind_T *)message_p)->as_id;
                  break;

               case CM_UNSTRUCTURED_SS_NOTIFY_IND:
                  local_as_id = ((cm_unstructured_ss_notify_ind_T *)message_p)->as_id;
                  break;

               case CM_UNSTRUCTURED_SS_IND:
                 local_as_id = ((cm_unstructured_ss_ind_T *)message_p)->as_id;
                  break;

#if defined ( FEATURE_CM_MMGPS ) || defined ( FEATURE_CM_LCS )
               case CM_LCS_LOCATION_NOTIFICATION_IND:
                 local_as_id = ((cm_lcs_location_notification_ind_T *)message_p)->as_id;
                  break;

               case CM_LCS_MOLR_CONF:
                  local_as_id = ((cm_lcs_molr_conf_T *)message_p)->as_id;
                  break;
#endif
               case CM_RELEASE_UNSTRUCTURED_SS_IND:
                  local_as_id = ((cm_release_unstructured_ss_ind_T *)message_p)->as_id;
                  break;

               case CM_CONNECTION_STATE_IND:
                 local_as_id = ((cm_connection_state_ind_T *)message_p)->as_id;
                  break;

               case CM_GET_PASSWORD_IND:
                 local_as_id = ((cm_get_password_ind_T *)message_p)->as_id;
                  break;

               case CM_CS_SIG_REL_IND:
                  local_as_id = ((cm_cs_sig_rel_ind_T *)message_p)->as_id;
                  break;

#ifdef FEATURE_CCBS
               case CM_ERASE_CC_ENTRY_CONF:
                  local_as_id = ((cm_erase_cc_entry_conf_T *)message_p)->as_id;
                  break;

               case CM_CCBS_INFO_IND:
                  local_as_id = ((cm_ccbs_info_ind_T *)message_p)->as_id;
                  break;

               case CM_5_SEND_CONF:
                  local_as_id = ((cm_5_send_conf_T *)message_p)->as_id;
                  break;

               case CM_RECALL_IND:
                  local_as_id = ((cm_recall_ind_T *)message_p)->as_id;
                  break;
#endif /* FEATURE_CCBS */

               case CM_GET_SRVCC_CONTEXT_REQ:
                  local_as_id = ((cm_get_srvcc_context_req_T *)message_p)->as_id;
                  break;

               case CM_NAS_SRVCC_HANDOVER_COMPLETE_IND:
                 local_as_id = ((cm_srvcc_handover_complete_ind_T *)message_p)->as_id;
                  break;

               default:
                  MSG_HIGH_DS_1(MN_SUB,"=MNCNM= MN received/sent unknown message id =%02X to CM",
                      message_p->message_id);
                  break;
            }/* end switch (message_id) */
            MSG_HIGH_DS_4(MN_SUB,"=MNCNM=  MS:0x%X   MSG_ID:0x%X   QUEUE_ID: %d   AS_ID_%d",message_p->message_set,message_p->message_id,queue_id,local_as_id+1);
         }/* endif queue_id */
#else  /* FEATURE_DUAL_SIM */
         if (queue_id == GS_QUEUE_MN_CM)
         {
            word message_length;
            /*    Get message length */
            GET_IMH_LEN(message_length, message_p); 
            /*    Add in header size */
            message_length +=  sizeof(IMH_T);
            /* Log using CFA packet */                
            cfa_log_packet( GS_QUEUE_MN_CM, message_length, (byte*)message_p );
           
            MSG_HIGH_DS_3(MN_SUB,"=MNCNM= =MN= MS:0x%X   MSG_ID:0x%X   QUEUE_ID: %d ",message_p->message_set,message_p->message_id,queue_id );

            switch( message_p->message_id )
            {

               case CM_MO_NORMAL_CALL_REQ:
                  break;

               case CM_MO_EMERGENCY_CALL_REQ:
                  break;

               case CM_START_DTMF_REQ:
                  break;

               case CM_STOP_DTMF_REQ:
                  break;

               case CM_0_SEND_REQ:
                  break;

               case CM_1_SEND_REQ:
                  break;

               case CM_1_X_SEND_REQ:
                  break;

               case CM_2_SEND_REQ:
                  break;

               case CM_TRANSFER_SRVCC_CALL_CONTEXT_RES:
                  break;

               case CM_2_X_SEND_REQ:
                  break;

               case CM_3_SEND_REQ:
                  break;

               case CM_4_SEND_REQ:
                  break;

               case CM_MO_END_CALL_REQ:
                  break;

               case CM_MO_END_X_CALL_REQ:
                  break;

               case CM_ASID_2_VSID_MAPPING_IND:
                  break;

               case CM_MO_USER_DATA_REQ:
                  break;

               case CM_MO_MODIFY_REQ:
                  break;

               case CM_ABORT_CC_REQ:
                  break;

               case CM_RELEASE_SS_REQ:
                  break;

               case CM_ACTIVATE_SS_REQ:
                  break;

               case CM_DEACTIVATE_SS_REQ:
                  break;

               case CM_ERASE_SS_REQ:
                  break;

               case CM_INTERROGATE_SS_REQ:
                  break;

               case CM_REGISTER_PASSWORD_REQ:
                  break;

               case CM_REGISTER_SS_REQ:
                  break;

               case CM_PROCESS_UNSTRUCTURED_SS_REQ:
                  break;

               case CM_ABORT_SS_REQ:
                  break;

               case CM_PROCESS_UNSTRUCTURED_SS_DATA:
                  break;

#if defined ( FEATURE_CM_MMGPS ) || defined ( FEATURE_CM_LCS )
               case CM_LCS_MOLR_REQ:
                  break;

               case CM_LCS_LOCATION_NOTIFICATION_RES:
                  break;

               case CM_LCS_MOLR_COMPLETED:
                  break;
#endif

               case CM_MT_CALL_RES:
                  break;

               case CM_MT_SETUP_RES:
                  break;

               case CM_MT_MODIFY_RES:
                  break;

               case CM_GET_PASSWORD_RES:
                  break;

               case CM_UNSTRUCTURED_SS_NOTIFY_RES:
                  break;

               case CM_UNSTRUCTURED_SS_RES:
                  break;

               case CM_MT_CALL_SS_RES:
                  break;

               default:
                  break;

             }/* end switch(message_p->message_id) */
         }
         else if (queue_id == GS_QUEUE_CM)
         {
         
         /*
         * If it is an outgoing message 
         */
            MSG_HIGH_DS_3(MN_SUB,"=MNCNM= =MN= MS:0x%X   MSG_ID:0x%X  QUEUE_ID: %d",message_p->message_set,message_p->message_id,queue_id); 
            switch(message_p->message_id)
            {
               case CM_MO_CALL_CONF:
                  break;

               case CM_START_DTMF_CONF:
                  break;

               case CM_STOP_DTMF_CONF:
                  break;

               case CM_MANAGE_CALLS_CONF:
                  break;

               case CM_MODIFY_COMPLETE_CONF:
                  break;

               case CM_ABORT_CC_CONF:
                  break;

               case CM_ACTIVATE_SS_CONF:
                  break;

               case CM_DEACTIVATE_SS_CONF:
                  break;

               case CM_ERASE_SS_CONF:
                  break;

               case CM_INTERROGATE_SS_CONF:
                  break;

               case CM_REGISTER_PASSWORD_CONF:
                  break;

               case CM_REGISTER_SS_CONF:
                  break;

               case CM_PROCESS_UNSTRUCTURED_SS_CONF:
                  break;

               case CM_PROCESS_UNSTRUCTURED_SS_DATA_CONF:
                  break;

               case CM_MT_CALL_IND:
                  break;

               case CM_CALL_PROGRESS_INFO_IND:
                  break;

               case CM_MO_CALL_ALERT_IND:
                  break;

               case CM_CALL_CONNECTED_IND:
                  break;

               case CM_CONNECTED_NUMBER_IND:
                  break;

               case CM_MT_END_CALL_IND:
                  break;

#ifdef FEATURE_UUS
               case CM_MT_USER_DATA_IND:
                  break;

               case CM_USER_CONGESTION_IND:
                  break;
#endif

               case CM_MT_SETUP_IND:
                  break;

               case CM_MT_MODIFY_IND:
                  break;

               case CM_MT_DISC_IND:
                  break;

               case CM_MT_CALL_SS_IND:
                  break;

               case CM_FORWARD_CHECK_SS_INDICATION_IND:
                  break;

               case CM_UNSTRUCTURED_SS_NOTIFY_IND:
                  break;

               case CM_UNSTRUCTURED_SS_IND:
                  break;

#if defined ( FEATURE_CM_MMGPS ) || defined ( FEATURE_CM_LCS )
               case CM_LCS_LOCATION_NOTIFICATION_IND:
                  break;

               case CM_LCS_MOLR_CONF:
                  break;
#endif
               case CM_RELEASE_UNSTRUCTURED_SS_IND:
                  break;

               case CM_CONNECTION_STATE_IND:
                  break;

               case CM_GET_PASSWORD_IND:
                  break;

               case CM_CS_SIG_REL_IND:
                  break;

#ifdef FEATURE_CCBS
               case CM_ERASE_CC_ENTRY_CONF:
                  break;

               case CM_CCBS_INFO_IND:
                  break;

               case CM_5_SEND_CONF:
                  break;

               case CM_RECALL_IND:
                  break;
#endif /* FEATURE_CCBS */

               case CM_NAS_SRVCC_HANDOVER_COMPLETE_IND:
                  break;

               case CM_GET_SRVCC_CONTEXT_REQ:
                  break;

               default:
                  MSG_HIGH_DS_1(MN_SUB,"=MNCNM= MN received/sent unknown message id =%02X to CM",
                      message_p->message_id);
                  break;
            }/* end switch (message_id) */
         }/* endif queue_id */
#endif

         else
         {
             MSG_HIGH_DS_1(MN_SUB,"=MNCNM= MN <- a MS_MN_CM message with unknown queue_id %d",
                       queue_id);
         }
         break;

      case MS_MN_UASMS:

         /*
         ** MESSAGE ID ?
         */
#ifdef FEATURE_DUAL_SIM
         switch(message_p->message_id)
         {
            case UASMS_SM_DATA_REQ:
               local_as_id = ((SM_RL_DATA_REQ_T *)message_p)->as_id;
               break;

            case UASMS_SM_REPORT_REQ:
               local_as_id = ((SM_RL_REPORT_REQ_T *)message_p)->as_id;
               break;

            case UASMS_MEMORY_AVAIL_NOTIFICATION_REQ:
               local_as_id = ((SM_RL_MEMORY_AVAILABLE_REQ_T *)message_p)->as_id;
               break;

            case UASMS_ABORT_SM_REQ:
               local_as_id = ((SM_RL_ABORT_SM_REQ_T *)message_p)->as_id;
               break;

            case UASMS_SM_LINK_CONTROL_REQ:
               local_as_id = ((SM_RL_LINK_CONTROL_REQ_T *)message_p)->as_id;
               break;

            case UASMS_SM_DATA_IND:
               local_as_id = ((SM_RL_DATA_IND_T *)message_p)->as_id;
               break;

            case UASMS_SM_REPORT_IND:
              local_as_id = ((SM_RL_REPORT_IND_T *)message_p)->as_id;
               break;

            case UASMS_SM_LINK_CONTROL_IND:
               local_as_id = ((SM_RL_LINK_CONTROL_REPORT_T *)message_p)->as_id ;
               break;

            case UASMS_SM_UPDATE_TIMERF:
               local_as_id = ((SM_RL_UPDATE_TIMERF_T *)message_p)->as_id;
               break;

            default:
               MSG_HIGH_DS_1(MN_SUB,"=MNCNM= MN received/sent unknown message id %02X from/to UASMS",
                   message_p->message_id);
            break;
         }
         MSG_HIGH_DS_4(MN_SUB,"=MNCNM= =MN= MS:0x%X   MSG_ID:0x%X   QUEUE_ID:%d  AS_ID_%d",message_p->message_set,message_p->message_id,queue_id,local_as_id+1);
#else
         MSG_HIGH_DS_3(MN_SUB,"=MNCNM= =MN= MS:0x%X   MSG_ID:0x%X   QUEUE_ID:%d ",message_p->message_set,message_p->message_id,queue_id );
         switch(message_p->message_id)
         {
            case UASMS_SM_DATA_REQ:
               break;

            case UASMS_SM_REPORT_REQ:
               break;

            case UASMS_MEMORY_AVAIL_NOTIFICATION_REQ:
               break;

            case UASMS_ABORT_SM_REQ:
               break;

            case UASMS_SM_LINK_CONTROL_REQ:
               break;

            case UASMS_SM_UPDATE_TIMERF:
               break;

            case UASMS_SM_DATA_IND:
               break;

            case UASMS_SM_REPORT_IND:
               break;

            case UASMS_SM_LINK_CONTROL_IND:
               break;

            default:
               MSG_HIGH_DS_1(MN_SUB,"=MNCNM= MN received/sent unknown message id %02X from/to UASMS",
                   message_p->message_id);
            break;
         }
#endif
         break;

      case MS_MN_CC:
#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
         /*
          ** MESSAGE ID ?
          */
         switch( message_p->message_id )
         {
            case MNCC_ALERT_REQ:
               local_as_id = ((MNCC_ALERT_REQ_T *)message_p)->as_id;
               break;

            case MNCC_CALL_CONF_REQ:
              local_as_id = ((MNCC_CALL_CONF_REQ_T *)message_p)->as_id;
               break;

            case MNCC_CALL_HOLD_REQ:
               local_as_id = ((MNCC_CALL_HOLD_REQ_T *)message_p)->as_id;
               break;

            case MNCC_CALL_RETRIEVE_REQ:
              local_as_id = ((MNCC_CALL_RETRIEVE_REQ_T *)message_p)->as_id;
               break;

            case MNCC_DISC_REQ:
               local_as_id = ((MNCC_DISC_REQ_T *)message_p)->as_id;
               break;

            case MNCC_FACILITY_REQ:
               local_as_id = ((MNCC_FACILITY_REQ_T *)message_p)->as_id;
               break;

            case MNCC_REJ_REQ:
               local_as_id = ((MNCC_REJ_REQ_T *)message_p)->as_id;
               break;

            case MNCC_REL_REQ:
               local_as_id = ((MNCC_REL_REQ_T *)message_p)->as_id;
               break;

            case MNCC_REL_COMPL_REQ:
               local_as_id = ((MNCC_REL_COMPL_REQ_T *)message_p)->as_id;
               break;

            case MNCC_MODIFY_REJ:
               local_as_id = ((MNCC_MODIFY_REQ_T *)message_p)->as_id;
               break;

            case MNCC_MODIFY_REQ:
               local_as_id = ((MNCC_MODIFY_REQ_T *)message_p)->as_id;
               break;

            case MNCC_MODIFY_RES:
               local_as_id = ((MNCC_MODIFY_RES_T *)message_p)->as_id;
               break;

            case MNCC_NOTIFY_REQ:
               break;

            case MNCC_SETUP_COMPL_REQ:
               local_as_id = ((MNCC_SETUP_COMPL_REQ_T *)message_p)->as_id;
               break;

            case MNCC_SETUP_COMPL_RES:
               local_as_id = ((MNCC_SETUP_COMPL_RES_T *)message_p)->as_id;
               break;

            case MNCC_SETUP_REQ:
               local_as_id = ((MNCC_SETUP_REQ_T *)message_p)->as_id;
               break;

            case MNCC_SETUP_RES:
               local_as_id = ((MNCC_SETUP_RES_T *)message_p)->as_id;
               break;

            case MNCC_START_DTMF_REQ:
               local_as_id = ((MNCC_START_DTMF_REQ_T *)message_p)->as_id;
               break;

            case MNCC_STOP_DTMF_REQ:
               local_as_id = ((MNCC_STOP_DTMF_REQ_T *)message_p)->as_id;
               break;

		   case MNCC_SRVCC_HANDOVER_FAIL_REQ:
               local_as_id = ((MNCC_SRVCC_HANDOVER_FAIL_REQ_T *)message_p)->as_id;
               break;

#ifdef FEATURE_UUS
            case MNCC_USER_USER_DATA_REQ:
               local_as_id = ((MNCC_USER_USER_DATA_REQ_T *)message_p)->as_id;
               break;
#endif

            case MNCC_MODIFY_ACCEPT:
               local_as_id = ((MNCC_MODIFY_ACCEPT_T *)message_p)->as_id;
               break;

            case MNCC_ALERT_IND:
               local_as_id = ((MNCC_ALERT_IND_T *)message_p)->as_id;
               break;

            case MNCC_CALL_HOLD_CNF:
               local_as_id = ((MNCC_CALL_HOLD_CNF_T *)message_p)->as_id;
               break;

            case MNCC_CALL_HOLD_REJ:
               local_as_id = ((MNCC_CALL_HOLD_REJ_T *)message_p)->as_id;
               break;

            case MNCC_CALL_PROC_IND:
               local_as_id = ((MNCC_CALL_PROC_IND_T *)message_p)->as_id;
               break;

            case MNCC_CALL_RETRIEVE_CNF:
               local_as_id = ((MNCC_CALL_RETRIEVE_CNF_T *)message_p)->as_id;
               break;

            case MNCC_CALL_RETRIEVE_REJ:
               local_as_id = ((MNCC_CALL_RETRIEVE_REJ_T *)message_p)->as_id;
               break;

            case MNCC_DISC_IND:
               local_as_id = ((MNCC_DISC_IND_T *)message_p)->as_id;
               break;

            case MNCC_ERR_IND:
               local_as_id = ((MNCC_ERR_IND_T *)message_p)->as_id;
               break;

            case MNCC_FACILITY_IND:
               local_as_id = ((MNCC_FACILITY_IND_T *)message_p)->as_id;
               break;

            case MNCC_MODIFY_CNF:
               local_as_id = ((MNCC_MODIFY_CNF_T *)message_p)->as_id;
               break;

            case MNCC_MODIFY_IND:
               local_as_id = ((MNCC_MODIFY_IND_T *)message_p)->as_id;
               break;

            case MNCC_NOTIFY_IND:
               local_as_id = ((MNCC_NOTIFY_IND_T *)message_p)->as_id;
               break;

            case MNCC_PROGRESS_IND:
               local_as_id = ((MNCC_PROGRESS_IND_T *)message_p)->as_id;
               break;

            case MNCC_REEST_IND:
               local_as_id = ((MNCC_REEST_IND_T *)message_p)->as_id;
               break;

            case MNCC_REJ_IND:
               local_as_id = ((MNCC_REJ_IND_T *)message_p)->as_id;
               break;

            case MNCC_REL_CNF:
               local_as_id = ((MNCC_REL_CNF_T *)message_p)->as_id;
               break;

            case MNCC_REL_IND:
               local_as_id = ((MNCC_REL_IND_T *)message_p)->as_id;
               break;

            case MNCC_SETUP_CNF:
               local_as_id = ((MNCC_SETUP_CNF_T *)message_p)->as_id;
               break;

            case MNCC_SETUP_COMPL_IND:
               local_as_id = ((MNCC_SETUP_COMPL_IND_T *)message_p)->as_id;
               break;

            case MNCC_SETUP_IND:
               local_as_id = ((MNCC_SETUP_IND_T *)message_p)->as_id;
               break;

            case MNCC_START_DTMF_CNF:
               local_as_id = ((MNCC_START_DTMF_CNF_T *)message_p)->as_id;
               break;

            case MNCC_STOP_DTMF_CNF:
               local_as_id = ((MNCC_STOP_DTMF_CNF_T *)message_p)->as_id;
               break;

            case MNCC_SYNC_IND:
               local_as_id = ((MNCC_SYNC_IND_T *)message_p)->as_id;
               break;

            case MNCC_RAB_REL_IND:
               local_as_id = ((MNCC_RAB_REL_IND_T *)message_p)->as_id;
               break;

            case MNCC_NOTIFY_DEFERRED_IND:
               local_as_id = ((MNCC_NOTIFY_DEFERRED_IND_T *)message_p)->as_id;
               break;

#ifdef FEATURE_UUS
            case MNCC_USER_DATA_DEFERRED_IND:
               local_as_id = ((MNCC_USER_DATA_DEFERRED_IND_T *)message_p)->as_id;
               break;

            case MNCC_USER_USER_DATA_IND:
               local_as_id = ((MNCC_USER_USER_DATA_IND_T *)message_p)->as_id;
               break;

            case MNCC_CONGESTION_IND:
               local_as_id = ((MNCC_CONGESTION_IND_T *)message_p)->as_id;
               break;
#endif

#ifdef FEATURE_CCBS
            case MNCC_RECALL_IND:
               local_as_id = ((MNCC_RECALL_IND_T *)message_p)->as_id;
               break;

            case MNCC_CC_EST_IND:
               local_as_id = ((MNCC_CC_EST_IND_T *)message_p)->as_id;
               break;
#endif /* FEATURE_CCBS */
               
            case MNCC_CS_SIG_CONN_REL_IND:
               local_as_id = ((MNCC_CS_SIG_CONN_REL_IND_T *)message_p)->as_id;
               break;

            case MNCC_SRVCC_HANDOVER_COMPLETE_IND:
               local_as_id = ((MNCC_SRVCC_HANDOVER_COMPLETE_IND_T *)message_p)->as_id;
               break;		    

            default:
               MSG_HIGH_DS_1(MN_SUB,"=MNCNM= MN received/sent unknown msg id %02X from/from CNM",
                   message_p->message_id);
            break;
         }
         MSG_HIGH_DS_4(MN_SUB,"=MNCNM= =MN= MS:0x%X   MSG_ID:0x%X  QUEUE_ID:%d  AS_ID_%d",message_p->message_set,message_p->message_id,queue_id,local_as_id+1);
         break;
#else
         MSG_HIGH_DS_3(MN_SUB,"=MNCNM= =MN= MS:0x%X   MSG_ID:0x%X  QUEUE_ID:%d ",message_p->message_set,message_p->message_id,queue_id );
         switch( message_p->message_id )
         {
            case MNCC_ALERT_REQ:
               break;

            case MNCC_CALL_CONF_REQ:
               break;

            case MNCC_CALL_HOLD_REQ:
               break;

            case MNCC_CALL_RETRIEVE_REQ:
               break;

            case MNCC_DISC_REQ:
               break;

            case MNCC_FACILITY_REQ:
               break;

            case MNCC_REJ_REQ:
               break;

            case MNCC_REL_REQ:
               break;

            case MNCC_REL_COMPL_REQ:
               break;

            case MNCC_MODIFY_REJ:
               break;

            case MNCC_MODIFY_REQ:
               break;

            case MNCC_MODIFY_RES:
               break;

            case MNCC_NOTIFY_REQ:
               break;

            case MNCC_SETUP_COMPL_REQ:
               break;

            case MNCC_SETUP_COMPL_RES:
               break;

            case MNCC_SETUP_REQ:
               break;

            case MNCC_SETUP_RES:
               break;

            case MNCC_START_DTMF_REQ:
               break;

            case MNCC_STOP_DTMF_REQ:
               break;

#ifdef FEATURE_UUS
            case MNCC_USER_USER_DATA_REQ:
               break;
#endif

            case MNCC_MODIFY_ACCEPT:
               break;

            case MNCC_SRVCC_HANDOVER_FAIL_REQ:
               break;

            case MNCC_ALERT_IND:
               break;

            case MNCC_CALL_HOLD_CNF:
               break;

            case MNCC_CALL_HOLD_REJ:
               break;

            case MNCC_CALL_PROC_IND:
               break;

            case MNCC_CALL_RETRIEVE_CNF:
               break;

            case MNCC_CALL_RETRIEVE_REJ:
               break;

            case MNCC_DISC_IND:
               break;

            case MNCC_ERR_IND:
               break;

            case MNCC_FACILITY_IND:
               break;

            case MNCC_MODIFY_CNF:
               break;

            case MNCC_MODIFY_IND:
               break;

            case MNCC_NOTIFY_IND:
               break;

            case MNCC_PROGRESS_IND:
               break;

            case MNCC_REEST_IND:
               break;

            case MNCC_REJ_IND:
               break;

            case MNCC_REL_CNF:
               break;

            case MNCC_REL_IND:
               break;

            case MNCC_SETUP_CNF:
               break;

            case MNCC_SETUP_COMPL_IND:
               break;

            case MNCC_SETUP_IND:
               break;

            case MNCC_START_DTMF_CNF:
               break;

            case MNCC_STOP_DTMF_CNF:
               break;

            case MNCC_SYNC_IND:
               break;

            case MNCC_RAB_REL_IND:
               break;

            case MNCC_NOTIFY_DEFERRED_IND:
               break;

#ifdef FEATURE_UUS
            case MNCC_USER_DATA_DEFERRED_IND:
               break;

            case MNCC_USER_USER_DATA_IND:
               break;

            case MNCC_CONGESTION_IND:
               break;
#endif

#ifdef FEATURE_CCBS
            case MNCC_RECALL_IND:
               break;

            case MNCC_CC_EST_IND:
               break;
#endif /* FEATURE_CCBS */
               
            case MNCC_CS_SIG_CONN_REL_IND:
               break;

            case MNCC_SRVCC_HANDOVER_COMPLETE_IND:
               break;

            default:
               MSG_HIGH_DS_1(MN_SUB,"=MNCNM= MN received/sent unknown msg id %02X from/from CNM",
                   message_p->message_id);
            break;
         }
         break;
#endif
      case MS_MN_SS:

         /*
          ** MESSAGE ID ?

          */
         MSG_HIGH_DS_3(MN_SUB,"=MNCNM= =MN= MS:0x%X   MSG_ID:0x%X  QUEUE_ID:%d",message_p->message_set,message_p->message_id,queue_id);
         switch( message_p->message_id )
         {
            case MNSS_BEGIN_REQ:
               break;

            case MNSS_END_REQ:
               break;

            case MNSS_FACILITY_REQ:
               break;

            case MNSS_BEGIN_IND:
               break;

            case MNSS_END_IND:
               break;

            case MNSS_FACILITY_IND:
               break;

            case MNSS_REJ_IND:
               break;

            default:
               MSG_HIGH_DS_1(MN_SUB,"=MNCNM= MN sent/sent unknown message id %02X from/from CNM",
                   message_p->message_id);
            break;
         }
         break;

      case MS_MN_SMS:

         /*
          ** MESSAGE ID ?
          */
              
        MSG_HIGH_DS_3(MN_SUB,"=MNCNM= =MN= MS:0x%X   MSG_ID:0x%X  QUEUE_ID:%d",message_p->message_set,message_p->message_id,queue_id);

         switch( message_p->message_id)
         {
            case MNSMS_EST_REQ:
               break;

            case MNSMS_DATA_REQ:
               break;

            case MNSMS_ABORT_REQ:
               break;

            case MNSMS_REL_REQ:
               break;

            case MNSMS_EST_IND:
               break;

            case MNSMS_DATA_IND:
               {
                   MNSMS_DATA_IND_T   mnsms_data_ind;

                   /* Log the RP message type */
                   memscpy(&mnsms_data_ind, sizeof(mnsms_data_ind) ,message_p, sizeof(MNSMS_DATA_IND_T));
                   CNM_log_rp_mti(mnsms_data_ind.rpdu.data[0]);
                  break;
               }

            case MNSMS_ERROR_IND:
               break;

            case MNSMS_CPACK_IND:
               break;

            default:
               MSG_HIGH_DS_0(MN_SUB,"=MNCNM= MN received/sent unknown message id from/from CNM");
            break;
         }
         break;

         case MS_TIMER:
            MSG_HIGH_DS_0(MN_SUB,"=MNCNM= MN timer expiration");
            break;

      case MS_MN_TEST:
         MSG_HIGH_DS_1(MN_SUB,"=MNCNM= MN received/sent MS_MN_TEST, msg_id = %02X",message_p->message_id);
         break;

      default:
          MSG_HIGH_DS_2(MN_SUB,"=MNCNM= MN received/sent unknown message_set %02X, message_id %02X",
                 message_p->message_set,
                 message_p->message_id);

         break;
   }/* end switch (message_p->message_set) */
}

#if defined(FEATURE_WCDMA) || defined(FEATURE_TDSCDMA)

/*===========================================================================

FUNCTION rrcmn_verify_rab

DESCRIPTION
  This function validates the received RAB and identifies the call type 
  for which the RAB belongs to.

DEPENDENCIES

PARAMETERS
  radio_access_bearer_T : IN - RAB Id received from OTA messsage 

RETURN VALUE
  rrcmn_rab_status_T : If the RAB is valid, it returns either 
  RRCMN_CS_DATA_CALL or RRCMN_CS_VOICE_CALL. Otherwise it returns 
  RRCMN_INVALID_RAB_ID.


SIDE EFFECTS
  None
===========================================================================*/
#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
rrcmn_rab_status_T mn_rab_status_sim[MAX_AS_IDS] = {RRCMN_INVALID_RAB_ID,RRCMN_INVALID_RAB_ID};
#define mn_rab_status (mn_rab_status_sim[mn_dsda_as_id])
#else
rrcmn_rab_status_T mn_rab_status = RRCMN_INVALID_RAB_ID;
#endif

rrcmn_rab_status_T rrcmn_verify_rab ( radio_access_bearer_T  rab_id )
{
  rrcmn_rab_status_T rrcmn_rab_status = mn_rab_status;

  (void) rab_id;

  MSG_HIGH_DS_1(MN_SUB,"=MNCNM= rrcmn_verify_rab return : %d",rrcmn_rab_status);
  return rrcmn_rab_status;
}

rrcmn_rab_status_T rrcmn_verify_rab_subs( radio_access_bearer_T  rab_id, sys_modem_as_id_e_type as_id )
{
#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
  rrcmn_rab_status_T rrcmn_rab_status;

  (void) rab_id;

  if(MNCNM_IS_SINGLE_STACK)
  {
    rrcmn_rab_status = mn_rab_status_sim[0];
  }
  else
  {
    if( (as_id > SYS_MODEM_AS_ID_NONE) && (as_id < MAX_AS_IDS) )
    {
      rrcmn_rab_status = mn_rab_status_sim[as_id];
    }
    else
    {
      MSG_ERROR_DS_1(MN_SUB,"=MNCNM= Recd invalid as_id = %d", as_id);
      rrcmn_rab_status = RRCMN_INVALID_RAB_ID;
    }
  }

  MSG_HIGH_DS_1(MN_SUB,"=MNCNM= rrcmn_verify_rab return : %d",rrcmn_rab_status);
  return rrcmn_rab_status;
#else
  MSG_HIGH_DS_0(MN_SUB,"=MNCNM= Error: Incorrect usage of DSDA API"); 
  return rrcmn_verify_rab(rab_id);
#endif
}

void mn_store_rab_status ( void )
{
    rrcmn_rab_status_T        return_value = RRCMN_INVALID_RAB_ID;
    connection_id_T           cur_connection_id = 0;
    uint8                     index;

    if (MN_read_multicall_status() != TRUE)
    {
     /*  SEE IF ACTIVE CALL INVOLVES DATA OR FAX  */
        for(index=0; index<3; index++)
        {
           cur_connection_id = Multicall_Status[index].connection_id[0];

           if((Multicall_Status[index].call_activity > CALL_INACTIVE) &&
               (mn_call_information_p[cur_connection_id] != NULL) &&
               (cc_transaction_information_p[cur_connection_id] != NULL))
       {

#if defined(FEATURE_MULTIMEDIA_FALLBACK) || defined(FEATURE_REL6)
               mn_call_information_T     *call_data_ptr;
               byte                      itc;

               call_data_ptr = mn_call_information_p[cur_connection_id];

               if(call_data_ptr->repeat_indicator_bc > 1) 
               {
                   boolean active_bearer_capability;

                   if( (cc_transaction_information_p[cur_connection_id]->cc_state == CC_STATE_U26) ||
                       (cc_transaction_information_p[cur_connection_id]->cc_state == CC_STATE_U27) )
                   {
                       active_bearer_capability = ((call_data_ptr->active_bearer_capability) ? FALSE : TRUE);
                   }
                   else
                   {
                       active_bearer_capability = call_data_ptr->active_bearer_capability;
                   }

                   if(active_bearer_capability)
                   {
                       itc = call_data_ptr->bearer_capability_2.information_transfer_capability;
                   }
                   else
                   {
                       itc = call_data_ptr->bearer_capability_1.information_transfer_capability;
                   }
               }
               else
               {
                   itc = call_data_ptr->bearer_capability_1.information_transfer_capability;
               }

           if((itc != ITC_AUX_SPEECH) && (itc != ITC_SPEECH))
           {
              return_value =RRCMN_CS_DATA_CALL;
              break;
           }
           else
           {
              return_value = RRCMN_CS_VOICE_CALL;
              break;
            }  /*   if mn_call_information[cur_connection_id]... */
#else
           cur_connection_id = Multicall_Status[index].connection_id[0];

          if( (mn_call_information_p[cur_connection_id]->repeat_indicator_bc > 1) ||
                   ( (mn_call_information_p[cur_connection_id]->bearer_capability_1.information_transfer_capability != ITC_AUX_SPEECH) && 
                     (mn_call_information_p[cur_connection_id]->bearer_capability_1.information_transfer_capability != ITC_SPEECH) )
                 )
           {
              return_value = RRCMN_CS_DATA_CALL;
              break;
           }
           else
           {
              return_value = RRCMN_CS_VOICE_CALL;
              break;
           }  /*   if mn_call_information[cur_connection_id]... */

#endif /* defined(FEATURE_MULTIMEDIA_FALLBACK) || defined(FEATURE_REL6) */

       }  /*   if Multicall_Status[index]... */
    
    }  /* for */

    }  /* if MN_read_multicall_status... */

    mn_rab_status  = return_value;

}  /* End of function rrcmn_verify_rab()  */

#if defined(FEATURE_MULTIMEDIA_FALLBACK) || defined(FEATURE_REL6)
/*===========================================================================

FUNCTION rrcmn_is_rab_modified

DESCRIPTION
  This function will indicate whether Rab type has changed or not
  

DEPENDENCIES

PARAMETERS
  none

RETURN VALUE
  boolean 
  TRUE(1)  - RAb type has changed 
  FALSE(0) - Rab type has not changed


SIDE EFFECTS
  None
===========================================================================*/
#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
boolean mn_rab_modified_sim[MAX_AS_IDS] = {FALSE,FALSE};
#define mn_rab_modified (mn_rab_modified_sim[mn_dsda_as_id])
#else
boolean mn_rab_modified = FALSE;
#endif

boolean rrcmn_is_rab_modified()
{
  boolean rrcmn_rab_modified = mn_rab_modified;

  MSG_HIGH_DS_1(MN_SUB,"=MNCNM= rrcmn_is_rab_modified return : %d",rrcmn_rab_modified);
  return rrcmn_rab_modified;
}

boolean rrcmn_is_rab_modified_subs(sys_modem_as_id_e_type as_id)
{
#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
  boolean rrcmn_rab_modified = 0;
  
  if(MNCNM_IS_SINGLE_STACK)
  {
     rrcmn_rab_modified = mn_rab_modified_sim[0];
  }
  else
  {
     rrcmn_rab_modified = mn_rab_modified_sim[as_id]; 
  }

  MSG_HIGH_DS_1(MN_SUB,"=MNCNM= rrcmn_is_rab_modified_subs return : %d",rrcmn_rab_modified);
  return rrcmn_rab_modified;
#else
  MSG_HIGH_DS_0(MN_SUB,"=MNCNM= Error: Incorrect usage of DSDA API"); 
  return rrcmn_is_rab_modified();
#endif
}

void mn_store_rab_modify_status(void)
{
    connection_id_T           cur_connection_id = 0;
    uint8                     index;
    boolean                   return_value = FALSE;

    if (MN_read_multicall_status() != TRUE)
    {
        for(index=0; index<3; index++)
        {
           cur_connection_id = Multicall_Status[index].connection_id[0];

           if((Multicall_Status[index].call_activity > CALL_INACTIVE) &&
               (mn_call_information_p[cur_connection_id] != NULL) &&
               (cc_transaction_information_p[cur_connection_id] != NULL))
	         {
     
               if(REPEAT_SUPPORTS_FALLBACK == mn_call_information_p[cur_connection_id]->repeat_indicator_bc ||
                  REPEAT_SERVICE_CHANGE_FALLBACK == mn_call_information_p[cur_connection_id]->repeat_indicator_bc)
               {
                   if(CC_STATE_U26 == cc_transaction_information_p[cur_connection_id]->cc_state ||
                      ((CC_STATE_U10 == cc_transaction_information_p[cur_connection_id]->cc_state) &&  
                        (TRUE == mn_call_information_p[cur_connection_id]->modifiy_in_progress)))
                   {
                      return_value = TRUE;
                      break;
                   }
               }
           }
        }
    }
    MSG_HIGH_DS_1(MN_SUB,"=MNCNM= Inside mn_store_rab_modify_status  return_value = %d", return_value);
    mn_rab_modified = return_value;
}
#endif /* FEATURE_MULTIMEDIA_FALLBACK */
#endif /* FEATURE_WCDMA */

/********************************************************************************
 * Function name :  mn_has_nas_already_acquired_voc()
 * -------------
 *
 * Description :
 * -----------
 *  This Function tells the AS module (GL1 or RRC) which is invoking this function,If it is OK to take control 
 * of the MVS/ VOCODER during I-RAT HO. NAS should be the first Module that should acquire the MVS/VOCODER
 * for a MO/MT Call. Especiall for MT Call as it alone knows when the USER has answered the MT Call.
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
 ********************************************************************************/
extern rex_crit_sect_type        cm_conn_ids_crit_sect;  /* to be used for critical sections */

boolean mn_has_nas_already_acquired_voc(void)
{
  uint8                     index;
  boolean                   return_value = FALSE;

  /* Enter critical section */
  (void)gs_lock(&cm_conn_ids_crit_sect);

  /* 0x00 to 0x06 - M0 Connection IDs */  
  for( index = 0; index <= MAX_MO_CONNECTION_ID; index++)
  {
    if((cc_transaction_information_p[index] != NULL) &&
       ( cc_transaction_information_p[index]->cc_state == CC_STATE_U10
/* Take Care of MODIFY Cases */
    ))
    {

#ifdef FEATURE_NAS_VOC_REL_AFTER_CALL_HOLD 

      /* NAS does not have the vocoder acquired when a call ( single / single MPTY ) is in HELD state
       * This change was done to support simultaneous CS/PS call. */

      if(MN_turned_on_vocoder == TRUE)
#endif /* FEATURE_NAS_VOC_REL_AFTER_CALL_HOLD */
      {
        MSG_HIGH_DS_0(MN_SUB,"=MNCNM= has NAS ACQd voc first returning TRUE");    
        return_value = TRUE;
        break;
      }
    }

    if((cc_transaction_information_p[index]!= NULL) &&
       (cc_transaction_information_p[index]->cc_state >= CC_STATE_U3) &&
#ifdef FEATURE_NAS_VOC_REL_AFTER_CALL_HOLD
       (MN_turned_on_vocoder == TRUE) &&
#else
         (MN_check_for_inband_message(index) == TRUE) &&
#endif /* FEATURE_NAS_VOC_REL_AFTER_CALL_HOLD */ 
       (tch_user_connections_store[DEFAULT_RAB_ID].OTA_channel_connected == TRUE))
    {
      MSG_HIGH_DS_0(MN_SUB,"=MNCNM= NAS ACQd voc first return TRUE,Progress IE present");
      return_value = TRUE;
      break;
    }
  }

  if (!return_value)
  {
    /* 0x08 to 0x14 - MT Connection IDs */
    for( index = MT; index < MAXNO_CC_TRANSACTIONS; index++)
    {
      if((cc_transaction_information_p[index] != NULL) &&
         (cc_transaction_information_p[index]->cc_state == CC_STATE_U10
  /* Take Care of MODIFY Cases */
      ))
      {

  /* NAS does not have the vocoder acquired when a call ( single / single MPTY ) is in HELD state
  * This change was done to support simultaneous CS/PS call. */

        if(MN_turned_on_vocoder == TRUE)

        {
          MSG_HIGH_DS_0(MN_SUB,"=MNCNM= has NAS ACQd voc first returning TRUE");
          return_value = TRUE;
          break;
        }
      }

      if((cc_transaction_information_p[index]!= NULL) &&
         (cc_transaction_information_p[index]->cc_state >= CC_STATE_U3) &&
         (MN_turned_on_vocoder == TRUE) &&
         (MN_check_for_inband_message(index) == TRUE) &&
         (tch_user_connections_store[DEFAULT_RAB_ID].OTA_channel_connected == TRUE))
      {
        MSG_HIGH_DS_0(MN_SUB,"=MNCNM= NAS ACQd voc first return TRUE,Progress IE present"); 
        return_value = TRUE;
        break;
      }

    }
  }

  /* exit critical section */
  (void)gs_unlock(&cm_conn_ids_crit_sect); 

/*Case where MS is ordered to attach the user connection after it is assigned Traffic Channel by 
   the NW even when CALL SETUP is ongoing ---  PROGRESS INDICATOR IE */

/*
active_bc_speech = MN_is_speech_call(message_ptr->connection_id);   -- INDICATES if the call is a speech call

if((MN_check_for_inband_message(message_ptr->connection_id) == TRUE) &&
   (tch_user_connections_store[DEFAULT_RAB_ID].OTA_channel_connected == TRUE) &&
   active_bc_speech)

  MO SIDE Check for the below STATE CHECK 
(cc_transaction_information[connection_id].cc_state >= CC_STATE_U3) &&
(MN_check_for_inband_message(connection_id) == TRUE))
   
*/  
  MSG_HIGH_DS_1(MN_SUB,"=MNCNM= has NAS ACQd voc first returning %d",return_value);
  return return_value;
}



boolean mn_has_nas_already_acquired_voc_subs(sys_modem_as_id_e_type as_id)
{
  
#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)  

  boolean                   return_value = FALSE;
  sys_modem_as_id_e_type    local_as_id  = as_id;

  if(MNCNM_IS_SINGLE_STACK)
  {
     local_as_id = 0;
  }

  if (local_as_id > SYS_MODEM_AS_ID_NONE && local_as_id < MAX_AS_IDS)
  {  
     uint8                     index;
     /* Enter critical section */
     (void)gs_lock(&cm_conn_ids_crit_sect);
   
     /* 0x00 to 0x06 - M0 Connection IDs */  
     for( index = 0; index <= MAX_MO_CONNECTION_ID; index++)
     {
       if((cc_transaction_information_p_sim[local_as_id][index] != NULL) &&
          ( cc_transaction_information_p_sim[local_as_id][index]->cc_state == CC_STATE_U10
   /* Take Care of MODIFY Cases */
       ))
       {
   
#ifdef FEATURE_NAS_VOC_REL_AFTER_CALL_HOLD 
   
         /* NAS does not have the vocoder acquired when a call ( single / single MPTY ) is in HELD state
          * This change was done to support simultaneous CS/PS call. */
   
         if(MN_turned_on_vocoder_sim[local_as_id] == TRUE)
#endif /* FEATURE_NAS_VOC_REL_AFTER_CALL_HOLD */
         {
           MSG_HIGH_DS_0(MN_SUB,"=MNCNM= has NAS ACQd voc first returning TRUE");    
           return_value = TRUE;
           break;
         }
       }
   
       if((cc_transaction_information_p_sim[local_as_id][index]!= NULL) &&
          (cc_transaction_information_p_sim[local_as_id][index]->cc_state >= CC_STATE_U3) &&
#ifdef FEATURE_NAS_VOC_REL_AFTER_CALL_HOLD
          (MN_turned_on_vocoder_sim[local_as_id] == TRUE) &&
#else
            (MN_check_for_inband_message_subs(index, local_as_id) == TRUE) &&
#endif /* FEATURE_NAS_VOC_REL_AFTER_CALL_HOLD */ 
          (tch_user_connections_store_sim[local_as_id][DEFAULT_RAB_ID].OTA_channel_connected == TRUE))
       {
         MSG_HIGH_DS_0(MN_SUB,"=MNCNM= NAS ACQd voc first return TRUE,Progress IE present");	
         return_value = TRUE;
         break;
       }
     }
   
     if (!return_value)
     {
       /* 0x08 to 0x14 - MT Connection IDs */
       for( index = MT; index < MAXNO_CC_TRANSACTIONS; index++)
       {
         if((cc_transaction_information_p_sim[local_as_id][index] != NULL) &&
            (cc_transaction_information_p_sim[local_as_id][index]->cc_state == CC_STATE_U10
     /* Take Care of MODIFY Cases */
         ))
         {
     /* NAS does not have the vocoder acquired when a call ( single / single MPTY ) is in HELD state
     * This change was done to support simultaneous CS/PS call. */

           if(MN_turned_on_vocoder_sim[local_as_id] == TRUE)
           {
             MSG_HIGH_DS_0(MN_SUB,"=MNCNM= has NAS ACQd voc first returning TRUE");
             return_value = TRUE;
             break;
           }
         }
   
         if((cc_transaction_information_p_sim[local_as_id][index]!= NULL) &&
            (cc_transaction_information_p_sim[local_as_id][index]->cc_state >= CC_STATE_U3) &&
            (MN_turned_on_vocoder_sim[local_as_id] == TRUE) &&
            (MN_check_for_inband_message_subs(index, local_as_id) == TRUE) &&
            (tch_user_connections_store_sim[local_as_id][DEFAULT_RAB_ID].OTA_channel_connected == TRUE))
         {
           MSG_HIGH_DS_0(MN_SUB,"=MNCNM= NAS ACQd voc first return TRUE,Progress IE present"); 
           return_value = TRUE;
           break;
         }
   
       }
     }
   
     /* exit critical section */
     (void)gs_unlock(&cm_conn_ids_crit_sect); 
   
   /*Case where MS is ordered to attach the user connection after it is assigned Traffic Channel by 
      the NW even when CALL SETUP is ongoing ---  PROGRESS INDICATOR IE */
   
   /*
   active_bc_speech = MN_is_speech_call(message_ptr->connection_id);   -- INDICATES if the call is a speech call
   
   if((MN_check_for_inband_message(message_ptr->connection_id) == TRUE) &&
      (tch_user_connections_store[DEFAULT_RAB_ID].OTA_channel_connected == TRUE) &&
      active_bc_speech)
   
     MO SIDE Check for the below STATE CHECK 
   (cc_transaction_information[connection_id].cc_state >= CC_STATE_U3) &&
   (MN_check_for_inband_message(connection_id) == TRUE))
      
   */  
     MSG_HIGH_DS_1(MN_SUB,"=MNCNM= has NAS ACQd voc first returning %d",return_value);
  }
  else
  {
     MSG_ERROR_DS_2(MN_SUB,"=MNCNM= Recd invalid as_id in mn_has_nas_already_acquired_voc_subs()= %d, local_as_id = %d", as_id, local_as_id);
  }
  return return_value;
#else
  MSG_HIGH_DS_0(MN_SUB,"=MNCNM= Error: Incorrect usage of DSDA API"); 
  return mn_has_nas_already_acquired_voc(); 
#endif  
  
}





#ifdef FEATURE_NAS_VOC_REL_AFTER_CALL_HOLD

/*===========================================================================

FUNCTION: mn_at_least_one_call_in_active_conv_state

DESCRIPTION: this function checks the Suplementary Status of all the Speech Calls and returns 
TRUE if at least one of the Speech Call is in Active Conversational State.

DEPENDENCIES:

PARAMETERS: none

RETURN VALUE: Boolean 
    TRUE if at least one of the Speech Call is in Active Conversational State else FALSE

SIDE EFFECTS:
  None
===========================================================================*/
boolean mn_at_least_one_call_in_active_conv_state(void)
{
  byte     hold_state;
  int      index;

  for(index = 0; index < MAXNO_CC_TRANSACTIONS; index++)
  {
    if(index != UNKNOWN_CONNECTION_ID && cc_transaction_information_p[index] != NULL &&
        mn_call_information_p[index] != NULL)  /* reserved connection ID = emergency call */ 
    {
      hold_state = cc_transaction_information_p[index]->hold_auxiliary_state;

      if((hold_state == HOLD_IDLE) ||
          (hold_state == HOLD_REQUEST)) /* not call held active state */
      {
        MSG_HIGH_DS_0(MN_SUB,"=MNCNM= mn_at_least_one_call_in_active_conv_state returning TRUE\n");    
        return TRUE;
      }
    }
  }
  MSG_HIGH_DS_0(MN_SUB,"=MNCNM= mn_at_least_one_call_in_active_conv_state returning FALSE\n");    
  return FALSE;
}

#endif /* FEATURE_NAS_VOC_REL_AFTER_CALL_HOLD */

/*===========================================================================

FUNCTION: 

DESCRIPTION:

DEPENDENCIES:

PARAMETERS:

RETURN VALUE:

SIDE EFFECTS:
  None
===========================================================================*/
void   MN_q_init(MN_q_T *queue)
{
    if(queue)
    {
        queue->head = NULL;
        queue->tail = 0;
        queue->count = 0;
    }
    else
    {
        MSG_ERROR_DS_1(MN_SUB,"=MNCNM= Invalid Queue = %x", queue);
    }
}


/*===========================================================================

FUNCTION: 

DESCRIPTION:

DEPENDENCIES:

PARAMETERS:

RETURN VALUE:

SIDE EFFECTS:
  None
===========================================================================*/
void   MN_q_put(MN_q_T *queue, MN_qlink_T *link)
{
    if(link && queue)
    {
        if(queue->count)
        {
            queue->tail->next_ptr = link;  /* added new link to tail */

            link->next_ptr = NULL; /* end of queue */

            link->prev_ptr = queue->tail;  

            queue->tail = link; /* update tail */
        }
        else
        {
            queue->head = link;

            queue->tail = link;
        }

        queue->count++;
    }
    else
    {
        MSG_ERROR_DS_2(MN_SUB,"=MNCNM= Invalid qLink = %d or Queue = %d", link, queue);
    }
}

/*===========================================================================

FUNCTION: 

DESCRIPTION:

DEPENDENCIES:

PARAMETERS:

RETURN VALUE:

SIDE EFFECTS:
  None
===========================================================================*/
void   *MN_q_get(MN_q_T *queue)
{
    void *item = NULL;
 
    if(queue)
    {
        if(queue->count)
        {
            item = queue->head->item;  /* get item from head */

            queue->head = queue->head->next_ptr;  /* update head */

        queue->count--;  
        }
        else 
        {
            item = NULL;
        }
    } 
    else
    {
        MSG_ERROR_DS_1(MN_SUB,"=MNCNM= Invalid Queue = %d", queue);
    }

    return item;
}

/*===========================================================================

FUNCTION: 

DESCRIPTION:

DEPENDENCIES:

PARAMETERS:

RETURN VALUE:

SIDE EFFECTS:
  None
===========================================================================*/
MN_qlink_T *MN_q_link(void *item, MN_qlink_T *link)
{
    if(item && link)
    {
        link->next_ptr = NULL;
        link->prev_ptr = NULL;
        link->item = item;
    }
    else
    {
        MSG_ERROR_DS_2(MN_SUB,"=MNCNM= Invalid qLink = %d or item = %d", link, item);
    }

    return link;
}


void mn_set_nv_items(mm_nas_feature_to_be_configured_T* mm_nas_feature_to_be_configured_local)
{
   mn_nv_no_auto_answer_on_hold = mm_nas_feature_to_be_configured_local->No_auto_answer_on_hold;
   mn_nv_mt_call_reject_cause = mm_nas_feature_to_be_configured_local->MT_Call_reject_cause;
   mn_als_itc_aux_speech = (mm_nas_feature_to_be_configured_local->als_itc_value == ALS_ITC_VALUE_6)? ITC_AUX_SPEECH_SIX: ITC_AUX_SPEECH_FOUR;
}

byte mn_get_mt_call_reject_cause(void)
{
  return mn_nv_mt_call_reject_cause;
}

byte mn_get_no_auto_answer_on_hold(void)
{
  return mn_nv_no_auto_answer_on_hold;
}


byte mn_get_als_itc_aux_speech(void)
{
  return mn_als_itc_aux_speech;
}

/*===========================================================================

FUNCTION: MN_is_MO_SMS_in_progress

DESCRIPTION: This function checks whether MO SMS is in progress over any transport type

DEPENDENCIES: none

PARAMETERS: none

RETURN VALUE: TRUE, if MO SMS exists.
                          else FALSE

SIDE EFFECTS:
  None
===========================================================================*/


boolean MN_is_MO_SMS_in_progress(void)
{
   transaction_id_T  transaction_id;
   boolean           transactions_in_progress = FALSE;

   for( transaction_id = MO; transaction_id < UNKNOWN_CONNECTION_ID; transaction_id++ )
   {
     if(sm_rl_transaction_information[transaction_id].rl_state != SMS_IDLE)
     {
       MSG_HIGH_DS_1(MN_SUB,"=MNCNM= MO SMS Transaction in progress over transport = %d", mo_sms_transport_type);
       transactions_in_progress = TRUE;
       break;
     }
   }
   return transactions_in_progress;
}

/*===========================================================================

FUNCTION: mn_voice_call_vote_for_reselection()

DESCRIPTION: This API returns resel allowed as FALSE if there is a voice call 
             not in conversational state for which CS RAB is not yet established
	     else it returns TRUE.

DEPENDENCIES:

PARAMETERS: as_id

RETURN VALUE:

SIDE EFFECTS:
  None
===========================================================================*/


nas_vote_for_reselection_e_type mn_voice_call_vote_for_reselection(sys_modem_as_id_e_type as_id)
{

   nas_vote_for_reselection_e_type ret_val     = NAS_VOTE_FOR_RESEL_TRUE;
   byte                            index       = 0;
   byte                            proc_finish = FALSE;

#ifdef FEATURE_DUAL_SIM

   if(MNCNM_IS_SINGLE_STACK && (as_id != mn_as_id))
   {

     MSG_HIGH("mn_voice_call_vote_for_reselection votes %d, CNM single stack and as_id != mn_as_id",NAS_VOTE_FOR_RESEL_TRUE,0,0);

     /* Allow resel if CNM's active stack's as_id is different than argument as_id*/
     ret_val     = NAS_VOTE_FOR_RESEL_TRUE; 
     proc_finish = TRUE;
   }

#ifdef FEATURE_DUAL_ACTIVE
   else if((as_id <= SYS_MODEM_AS_ID_NONE) || (as_id > (SYS_MODEM_AS_ID_NONE + MAX_AS_IDS)) ) 
   {
      MSG_HIGH("mn_voice_call_vote_for_reselection votes %d, DUAL_ACTIVE enabled and as_id = %d, out of bounds",NAS_VOTE_FOR_RESEL_TRUE,as_id,0);

      /* Allow resel if CNM's active stack's as_id is different than argument as_id*/
      ret_val     = NAS_VOTE_FOR_RESEL_TRUE; 
      proc_finish = TRUE;
   }
#endif
#endif


#if defined(FEATURE_DUAL_SIM ) && defined(FEATURE_DUAL_ACTIVE)

   /* If for some TID call state is not U0, then check if OTA channel is connected or not.
    */
   for(index = 0; (index < MAXNO_CC_TRANSACTIONS)&& (proc_finish == FALSE); index++)
   {
     if(index != UNKNOWN_CONNECTION_ID && cc_transaction_information_p_sim[as_id][index] != NULL &&
         mn_call_information_p_sim[as_id][index] != NULL)  
     {
       if( (CC_STATE_U10 != cc_transaction_information_p_sim[as_id][index]->cc_state) &&
        (tch_user_connections_store_sim[as_id][DEFAULT_RAB_ID].OTA_channel_connected != TRUE))
       {
         MSG_HIGH("mn_voice_call_vote_for_reselection =%d, call state = %d\n",NAS_VOTE_FOR_RESEL_FALSE,cc_transaction_information_p_sim[as_id][index]->cc_state,0);
         ret_val = NAS_VOTE_FOR_RESEL_FALSE;
         proc_finish = TRUE;
       }
     }
   }

#else

   /* If for some TID call state is not U0, then check if OTA channel is connected or not.
    */
   for(index = 0; (index < MAXNO_CC_TRANSACTIONS)&& (proc_finish == FALSE); index++)
   {
     if(index != UNKNOWN_CONNECTION_ID && cc_transaction_information_p[index] != NULL &&
         mn_call_information_p[index] != NULL)  
     {
       if( (CC_STATE_U10 != cc_transaction_information_p[index]->cc_state) &&
           (tch_user_connections_store[DEFAULT_RAB_ID].OTA_channel_connected != TRUE))
       {
         MSG_HIGH("mn_voice_call_vote_for_reselection =%d, call state = %d\n",NAS_VOTE_FOR_RESEL_FALSE,cc_transaction_information_p[index]->cc_state,0); 
         ret_val = NAS_VOTE_FOR_RESEL_FALSE;
         proc_finish = TRUE;
       }
     }
   }

#endif


   MSG_HIGH("mn_voice_call_vote_for_reselection votes %d",ret_val,0,0);
   return ret_val;
  
}


/*===========================================================================

FUNCTION: CNM_set_buffer_transient_triggers

DESCRIPTION:  This function sets the transient buffers

DEPENDENCIES: none

PARAMETERS:   Trigger and connection_id

RETURN VALUE: None

SIDE EFFECTS:
  None
===========================================================================*/

void CNM_set_buffer_transient_triggers(srvcc_trigger_e_type trigger, connection_id_T connection_id)
{
  byte counter = 0;
  for(counter = 0; counter < MAX_TRANSIENT_TRIGGERS; counter++)
  {
    if(buffer_transient_triggers[counter].trigger == T_NULL)
    {
      buffer_transient_triggers[counter].trigger = trigger;
      buffer_transient_triggers[counter].connection_id = connection_id;
      break;
    }
  }
}


/*===========================================================================

FUNCTION: CNM_reset_buffer_transient_triggers

DESCRIPTION:  This function resets the transient buffers

DEPENDENCIES: none

PARAMETERS:   None

RETURN VALUE: None

SIDE EFFECTS:
  None
===========================================================================*/

void CNM_reset_buffer_transient_triggers(void)
{
  
  byte counter = 0;
  for(counter = 0; counter < MAX_TRANSIENT_TRIGGERS; counter++)
  {
    
    buffer_transient_triggers[counter].trigger = T_NULL;
    buffer_transient_triggers[counter].connection_id = UNKNOWN_CONNECTION_ID;
  }

}

