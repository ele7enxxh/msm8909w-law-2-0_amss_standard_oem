/*===========================================================================

         G P R S   S U S P E N S I O N   A N D   R E S U M P T I O N

DESCRIPTION
  This file contains the data types, function definitions required for
  suspension/resumption of GPRS services.


Copyright (c) 1991 - 2003 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/src/gmmsuspend.c#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/05/02   mks     Created file
12/09/02   mks     Added gmm_saved_substate to save the current GMM substate
                   prior to GPRS suspension.
02/10/03   cd      Fixed Lint errors
03/19/03   mks     Skipping actions to be taken on entry to the interrupted
                   GMM substate from SUSPENDED sub state (upon GPRS resumption)
04/17/03   jca     Do not perform RAU on resumption failure if GPRS Detach pending.
04/29/03   jca     Added support for change of network mode of operation.
04/29/03   ks      Sending the RR_SERVICE_CNF to get GRR/RLC out of suspension
                   When Pending Detach Request overrides a RAU.
06/05/03   vdr     Removed sending RR_SERVICE_CNF mentioned 04/29/03 by ks
07/08/03   jca     Now setting RAU updating type to GMM_RA_UPDATING on GPRS
                   resumption failure even if T3312 expires while GMM is in
                   SUSPENDED substate (CR 30723).
07/16/03   jca     Added support for new RR_GMM_GPRS_RESUMPTION_IND primitive.
07/17/03   jca     GMM-LLC interface module now sending the RR_GMM_GPRS_SERVICE_CNF.
07/30/03   mks     Transition out of GMM suspended substate during GSM to WCDMA
                   handover procedure.
09/22/03   jca     Now setting ra_update_type to COMBINED_RA_LA on GPRS resumption
                   failure if NETWORK_OP_MODE_1 and ms_op_mode = CS+PS as per
                   24.008 R99 spec CR# 695.
01/15/04   mks     Now initiating RAU procedure upon GPRS resumption failure when
                   GMM is stranded in GMM_SERVICE_REQUEST_INITIATED.SUSPENDED state
                   due to collision of RAB re-establishment and W2G Inter-RAT
                   procedures.
03/17/04   jca     Added GMM_REGISTERED_INITIATED state to list of GMM states
                   allowed before checking for gmm_gprs_detach_pending.
06/18/04   jca     Added variable gmm_pending_ra_update_type.
06/01/05   jca     Ensure that the GPRS Detach procedure takes precedence over the
                   GPRS Attach procedure when a RR_GMM_GPRS_RESUMPTION_IND is received
                   and both the gmm_gprs_attach_pending and gmm_gprs_detach_pending
                   flags are set.
12/14/05   abe     Supported changes for FEATURE_GSM_DTM
03/16/05   abe     After GPRS resumption don't trigger RAU if one is already in progress 
03/26/13   c_gm    ported CR-458986
===========================================================================*/

/*===========================================================================
                    I N C L U D E D    H E A D E R     F I L E S
============================================================================*/
#include "mmcp_variation.h"
#include "customer.h"

#ifdef FEATURE_GSM_GPRS

#include "gmmsuspend.h"
#include "err.h"
#include "gmmllcm.h"
#include "mm_coord.h"

/*===========================================================================
                      PUBLIC DATA DECLARATIONS
===========================================================================*/

/**---------------------------------------------------------------------------
 ** This variable is used to save the GMM substate prior to GPRS suspension
 **--------------------------------------------------------------------------*/
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
LOCAL gmm_substate_type      gmm_saved_substate_sim[MAX_NAS_STACKS];
#define gmm_saved_substate gmm_saved_substate_sim[mm_as_id]
#else
LOCAL gmm_substate_type      gmm_saved_substate;
#endif /* FEATURE_DUAL_SIM || FEATURE_SGLTE*/

extern void gmm_handle_gprs_detach_low_level_failure( void );

/*===========================================================================
                          FUNCTION DEFINITIONS
===========================================================================*/

/*===========================================================================

FUNCTION GMM_HANDLE_GPRS_SUSPENSION

DESCRIPTION
  This  function  handles the RR_GMM_GPRS_SUSPENSION notification from GRR.
  (When an MS which is attached for GPRS services enters the dedicated mode
  and  when the MS limitations make it unable to handle both dedicated mode
  and packet transfer mode simultaneously, MS performs GPRS suspension).

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  - In GSM, if MS in operation mode B is in state GMM-REGISTERED.SUSPENDED
    when  the  timer  expires  the periodic routing area updating procedure
    is delayed until the state is left.
  - No signalling information is sent when GMM enters SUSPENDED substate.

===========================================================================*/
void gmm_handle_gprs_suspension ( void )
{
#if defined(FEATURE_LTE)
  emm_ctrl_data_type *emm_ctrl_data_ptr = NULL;

  emm_ctrl_data_ptr = emm_db_get_ctrl_data();

  ASSERT(emm_ctrl_data_ptr != NULL);
#endif
  if ( gmm_substate == GMM_SUSPENDED )
  {
    MSG_ERROR_DS_1( MM_SUB, "=MM= GMM is already in substate %d", GMM_SUSPENDED);
  }
  else if(gmm_substate == GMM_NO_CELL_AVAILABLE)
  {
     MSG_HIGH_DS_0( MM_SUB, "=MM= Suspension received in No Cell Available substate");
     gmm_is_suspended = TRUE;
#if defined(FEATURE_LTE)
     emm_ctrl_data_ptr->ps_resumption_status = EMM_PS_RESUMPTION_PENDING;
#endif
  }
  else
  {

#ifdef FEATURE_GPRS_PS_HANDOVER  
    gmm_psho_status = GMM_PSHO_NONE;
#endif

    MSG_HIGH_DS_0( MM_SUB, "=MM= MM received RR_GMM_GPRS_SUSPENSION" );

    /* -----------------------------------------------------------------
    **  Remember the current GMM substate - GMM will get back to this
    **  substate after GPRS resumption.
    ** -----------------------------------------------------------------*/
    gmm_saved_substate = gmm_substate;

    gmm_is_suspended = TRUE;
#if defined(FEATURE_LTE)
    emm_ctrl_data_ptr->ps_resumption_status = EMM_PS_RESUMPTION_PENDING;
#endif
    /* ------------------------------------------------------------------
    **  Transition to the GPRS  suspended state - If  T3312  expires in
    **  SUSPENDED substate Periodic RUA will be delayed till GMM leaves
    **  the SUSPENDED substate
    ** ------------------------------------------------------------------*/
    gmm_set_substate ( GMM_SUSPENDED );

  }
}

/*===========================================================================

FUNCTION GMM_HANDLE_GPRS_RESUMPTION
  This function handles the GPRS resumption information when UE comes out of
  dedicated mode that earlier caused GPRS suspension.

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  -  If T3312 expired when MS is in GMM-REGISTERED.SUSEPNDED state, periodic
     RAU is initiated when GRR notifies either GPRS resumption success/failure.
  -  If GRR notifies GPRS resumption failure by the network in GMM-REGISTERED.
     SUSPENDED state, Routing Area Update is initiated to resume GPRS services.

===========================================================================*/
void gmm_handle_gprs_resumption ( mm_cmd_type *message )
{
  boolean gmm_resumption_ind    = FALSE;
  gmm_state_type prev_gmm_state = gmm_state;
  sm_cmd_type                *sm_cmd;
#if defined(FEATURE_LTE)
  emm_ctrl_data_type *emm_ctrl_data_ptr = NULL;

  emm_ctrl_data_ptr = emm_db_get_ctrl_data();

  ASSERT(emm_ctrl_data_ptr != NULL);
#endif

#ifdef FEATURE_NAS_FREE_FLOAT
  gmmllc_reset_service_state();
#endif


  if ((gmm_substate == GMM_PLMN_SEARCH) || (mm_waiting_for_stop_mode_cnf))
  {
    gmm_is_suspended = FALSE;
    if (message->cmd.hdr.message_id == (int) RR_GMM_GPRS_RESUMPTION_IND)
    {
      MSG_HIGH_DS_1( MM_SUB, "=MM= MM received RR_GMM_GPRS_RESUMPTION_IND while in PLMN Search/ After stopping GRAT", mm_waiting_for_stop_mode_cnf );
      gmm_resumption_ind = message->cmd.rr_gmm_gprs_resumption_ind.gprs_resumption_ind;
    }
    else
    {
      MSG_FATAL_DS( MM_SUB, "=MM= Unexpected message id %d", message->cmd.hdr.message_id, 0,0 );
    }

    if (gmm_resumption_ind)
    {
      MSG_HIGH_DS_0( MM_SUB, "=MM= GPRS resumption successful");
#if defined(FEATURE_LTE)
      emm_ctrl_data_ptr->ps_resumption_status = EMM_PS_RESUMPTION_NONE;
#endif
    }
    else
    {
      MSG_HIGH_DS_0( MM_SUB, "=MM= GPRS resumption necessary");
    }
    if ((gmm_state == GMM_REGISTERED) ||
        (gmm_state == GMM_ROUTING_AREA_UPDATING_INITIATED) ||
        (gmm_state == GMM_SERVICE_REQUEST_INITIATED))
    {
      if (!gmm_resumption_ind)
      {
        gmm_ra_update_pending = TRUE;
      }
    }
    else
    {
      MSG_HIGH_DS_0( MM_SUB, "=MM= UE is not registered yet");
    }
  }
  else if ((gmm_substate == GMM_SUSPENDED) ||
           (gmm_is_suspended == TRUE))
  {
    gmm_is_suspended = FALSE;    

    /* ----------------------------------------------------------------
    ** MS leaves the SUSPENDED state and resumes with the interrupted
    ** substate. Since GMM was already in that substate prior to GPRS
    ** suspension, the actions to be taken on entry in to the new sub
    ** state are not performed.
    ** ----------------------------------------------------------------*/
    MSG_HIGH_DS_2( MM_SUB, "=MM= GMM substate change from %d to %d", gmm_substate, gmm_saved_substate );

    if (gmm_substate == GMM_SUSPENDED)
    {
      gmm_substate = gmm_saved_substate;
    }

    /* -------------------------------------------------------------
    **  Extract the GPRS_RESUMPTION_IND flag - it indicates whether
    **  network was able to successfully resume GPRS services for UE
    ** -------------------------------------------------------------*/
    if (message->cmd.hdr.message_id == (int) RR_GMM_GPRS_RESUMPTION_IND)
    {
      MSG_HIGH_DS_0( MM_SUB, "=MM= MM received RR_GMM_GPRS_RESUMPTION_IND");
      gmm_resumption_ind = message->cmd.rr_gmm_gprs_resumption_ind.gprs_resumption_ind;
    }
    else
    {
      MSG_FATAL_DS( MM_SUB, "=MM= Unexpected message id %d", message->cmd.hdr.message_id, 0,0 );
    }

    if (gmm_resumption_ind)
    {
      MSG_HIGH_DS_0( MM_SUB, "=MM= GPRS resumption successful");
#if defined(FEATURE_LTE)
      emm_ctrl_data_ptr->ps_resumption_status = EMM_PS_RESUMPTION_NONE;
#endif
    }
    else
    {
      MSG_HIGH_DS_0( MM_SUB, "=MM= GPRS resumption necessary");
    }
#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)
    if (gmm_gprs_detach_pending)
    {
      if (mmcoord_pending_reg_message.cmd.hdr.message_set != 0)
      {
        gmm_initiate_gprs_detach( &mmcoord_pending_reg_message );
      }
      else
      {
        MSG_ERROR_DS_0( MM_SUB, "=MM= Unable to proceed with GPRS Detach");
      }
    }
    else 
#endif /*#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)*/      
    if (gmm_gprs_attach_pending)
    {
      gmm_initiate_gprs_attach();
    }
	else if(gmm_power_down_active && gmm_state == GMM_DEREGISTERED_INITIATED && gmm_resumption_ind == FALSE)
    {
	  gmm_ra_update_pending=FALSE;
      gmm_set_state(GMM_DEREGISTERED);
      sm_cmd = sm_get_cmd_buf();

      if (sm_cmd == NULL)
      {
        MSG_FATAL_DS( MM_SUB, "=MM= ERROR: Unable to allocate memory", 0,0,0 );
      }

      sm_cmd->header.message_set = MS_GMM_SM;
      sm_cmd->header.cmd_id      = GMMSM_GPRS_RELEASE_IND;
      sm_cmd->cmd.gmm_gprs_rel_ind.cause = NO_GPRS_CONTEXT;
#ifdef FEATURE_DUAL_SIM
      sm_cmd->cmd.gmm_gprs_rel_ind.as_id = (sys_modem_as_id_e_type)mm_sub_id;
#endif

      MSG_HIGH_DS_0( MM_SUB, "=MM= Sending GMMSM_GPRS_RELEASE_IND");

      sm_put_cmd( sm_cmd ); 
    }
    else if ((gmm_state == GMM_REGISTERED) ||
             (gmm_state == GMM_REGISTERED_INITIATED) ||
             (gmm_state == GMM_ROUTING_AREA_UPDATING_INITIATED) ||
             (gmm_state == GMM_SERVICE_REQUEST_INITIATED) ||
             (gmm_state == GMM_DEREGISTERED_INITIATED))
    {
      if (gmm_ra_update_pending)
      {
             
        /***********************************************
        If RAU is already initiated,no need to Initiate 
        RAU after GPRS Resumption Indication.any how 
        after timer T3330 expires, RAU will be initiated 
        again.
        ***********************************************/

        if((gmm_state == GMM_ROUTING_AREA_UPDATING_INITIATED) &&
           (mm_timer_status[TIMER_T3330] == TIMER_ACTIVE))
        {
           MSG_HIGH_DS_0(MM_SUB, "=MM= Routing Area Update procedure is already inproggress");
        }

        /**************************************************
        If TIMER_T3311 or TIMER_T3302 is running, it means 
        RAU was initiated and it was not successful.RAU
        is pended,may be not because of GMM is suspened but
        because of MM is not MM_IDLE.In that case when 
        resumption indication comes no need to initiate 
        RAU.Any how after timer TIMER_T3311 expires, 
        RAU will be initiated.
        ***************************************************/

        else if(((mm_timer_status[TIMER_T3311] == TIMER_ACTIVE) ||
                 (mm_timer_status[TIMER_T3302] == TIMER_ACTIVE)) &&
                !((gmm_rai_changed( gmm_previous_registration_serving_rai.location_area_code,
                          gmm_previous_registration_serving_rai.routing_area_code,
                          mm_serving_plmn.lac, mm_serving_plmn.rac )) ||
                  (gmm_plmn_changed(gmm_previous_registration_serving_rai.plmn_id,
                            mm_serving_plmn.info.plmn)))
               )
        {
           MSG_HIGH_DS_0(MM_SUB, "=MM=  TIMER_T3311 or  TIMER_T3302 is active");
        }

        /************************************************************
        When MM state is not MM_IDLE and gmm_update_status is GMM_GU1_UPDATED,
        NORMAL RAU might have initiated,got sussessful.Combined RAU is pended 
        becuase of MM being not IDLE but not because of GMM is suspended.No need to
        intiaite RAU on GPRS Resumption.
        ************************************************************/

        else if(mm_state != MM_IDLE && 
                ((gmm_update_status == GMM_GU1_UPDATED) &&
                 (!gmm_rai_changed(gmm_stored_rai.location_area_code, gmm_stored_rai.routing_area_code, 
                        mm_serving_plmn.lac, mm_serving_plmn.rac)) &&
                 (!gmm_plmn_changed(gmm_stored_rai.plmn_id, mm_serving_plmn.info.plmn))
                )
#if defined(FEATURE_LTE)
                /* We need to trigger RAU if tin is guti as this is because of interrat
                from LTE to GSM DTM cell */
                &&(emm_ctrl_data_ptr->TIN != NAS_MM_TIN_GUTI)
#endif
               )
        {
           MSG_HIGH_DS_1(MM_SUB, "=MM=  MM state is %d  No need to intiaite RAU", mm_state);
        }
        else if(gmm_state != GMM_REGISTERED_INITIATED) 
        {
           gmm_ra_update_type = gmm_pending_ra_update_type;
           gmm_initiate_routing_area_update();
        }
      }
      else if ((gmm_periodic_rau_pending) &&
               (gmm_state != GMM_ROUTING_AREA_UPDATING_INITIATED) &&
               (gmm_state != GMM_REGISTERED_INITIATED)&&
               (gmm_substate == GMM_NORMAL_SERVICE)&&
               (gmm_resumption_ind))
      {
        /* ----------------------------------------------------------------------
        ** If timer T3312 expired when  MS was in state GMM-REGISTERED.SUSPENDED,
        ** periodic routing area procedure is performed as soon as MS leaves
        ** the SUSPENDED substate.
        ** ---------------------------------------------------------------------*/
        gmm_ra_update_type = GMM_PERIODIC_UPDATING;
        gmm_initiate_routing_area_update();
      }
      /* --------------------------------------------------------------
      **  If the MS is in state GMM-REGISTERED.SUSPENDED state and the
      **  RR sublayer indicates GPRS resumption failure (when dedicated
      **  mode is left) it shall perform Routing Area updating procedure
      **  to resume the GPRS services.
      ** -------------------------------------------------------------*/
      else if ((gmm_resumption_ind == FALSE) &&
               (gmm_state != GMM_ROUTING_AREA_UPDATING_INITIATED))
      {
        if (((mm_timer_status[TIMER_T3311] == TIMER_ACTIVE) ||
               (mm_timer_status[TIMER_T3302] == TIMER_ACTIVE)) &&
             !((gmm_rai_changed( gmm_previous_registration_serving_rai.location_area_code,
                          gmm_previous_registration_serving_rai.routing_area_code,
                          mm_serving_plmn.lac, mm_serving_plmn.rac )) ||
                (gmm_plmn_changed(gmm_previous_registration_serving_rai.plmn_id,
                            mm_serving_plmn.info.plmn))))
        {
           MSG_HIGH_DS_0(MM_SUB, "=MM=  Dont do RAU sinceTIMER_T3311 or  TIMER_T3302 is active ");
        }
/*24.008: 4.7.5.1.5 j)	Timer T3346 is running
	The MS shall not start the routing area updating procedure unless the MS is accessing the network with access class 11 – 15*/
#ifdef FEATURE_NAS_REL10
        else if((mm_timer_status[TIMER_T3346] == TIMER_ACTIVE)
#ifdef FEATURE_NAS_REL11
                  &&(mm_is_rrc_access_class_11_to_15() == FALSE)
#endif
		       )
        {
          MSG_HIGH_DS_0( MM_SUB, "=MM= Ignore RAU since T3346 congestion timer is running" );
        }
#endif
#ifndef FEATURE_CLASS_B_OPERATION_IN_NMO3 
        else if((mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_3) && (gmm_state == GMM_DEREGISTERED_INITIATED))
        {
          MSG_HIGH_DS_0(MM_SUB,"=MM= Local detach after getting resume failure in NMO3 with GMM DEREG state");
          gmm_handle_gprs_detach_low_level_failure();
          mmsend_nas_reg_event_packet(PROC_NAS_MO_DETACH_EVENT,PROC_END,PROC_END_LOWER_LAYER_FAILURE,MT_DETACH_TYPE_NONE,MT_DETACH_CAUSE_NONE);
        }
#endif
        else
        {
#ifdef FEATURE_DUAL_SIM
                sys_modem_as_id_e_type		mm_other_as_id;
#endif
                boolean   ps_dreg_started = FALSE;
#ifdef FEATURE_DUAL_SIM
                for(mm_other_as_id = SYS_MODEM_AS_ID_1 ; mm_other_as_id < MAX_NAS_STACKS ; mm_other_as_id++)
                {
                  if((mmcoord_reg_req_pending_sim[mm_other_as_id] == TRUE) &&
                         (gmm_force_local_detach_sim[mm_as_id] == TRUE))
                  {           
                    MSG_HIGH_DS_1(MM_SUB,"=MM= Local detach on SUB %d",mm_as_id+1);
                    gmm_handle_gprs_detach_low_level_failure();
                    mmsend_nas_reg_event_packet(PROC_NAS_MO_DETACH_EVENT,PROC_END,PROC_END_LOWER_LAYER_FAILURE,MT_DETACH_TYPE_NONE,MT_DETACH_CAUSE_NONE);
                    ps_dreg_started = TRUE;
                    if(mm_serv_change_pending)
                    {
                      mm_change_service_domain();
                    }
                  }
                }
#endif 
		

           /*If RAU is initiated successfully then the flag 
           gmm_ra_update_pending will be set to FALSE inside 
           the function gmm_initiate_routing_area_update*/

           if (!ps_dreg_started)
           {
              if ((mm_serv_change_pending)
#ifdef FEATURE_LTE			      
                    ||( mm_ps_detach_state == MM_PS_DETACH_STATE_WAITING_TO_SEND_CNF)
#endif		    
                  )
              {
                 gmm_detach_after_rau_pending = TRUE;
                 mmsend_nas_reg_event_packet(PROC_NAS_MO_DETACH_EVENT,PROC_END,PROC_END_LOWER_LAYER_FAILURE,MT_DETACH_TYPE_NONE,MT_DETACH_CAUSE_NONE);
              }

              if ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
                  (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1))
              {
                gmm_ra_update_type = GMM_COMBINED_RA_LA_UPDATING;
              }
              else
              {
                gmm_ra_update_type = GMM_RA_UPDATING;
              }

              gmm_ra_update_pending = TRUE;
              gmm_pending_ra_update_type = gmm_ra_update_type;
              gmm_initiate_routing_area_update();

           }
        }
      }
      else if (mm_reg_waiting_for_reg_cnf && mm_ready_to_send_reg_cnf)
      {   /*   no need to inform CS+PS if Detach is ongoing due to op mode change  */
         if(!(gmm_detach_after_rau_pending) &&
            !(gmm_state==GMM_DEREGISTERED_INITIATED && 
              mm_serving_plmn.ms_op_mode ==SYS_SRV_DOMAIN_CS_ONLY &&
              mmcoord_pending_reg_message.cmd.mmr_reg_req.req_service_domain == SYS_SRV_DOMAIN_CS_ONLY))  
         {
           MSG_HIGH_DS_0( MM_SUB, "=MM= Sending MMR_REG_CNF");
           mmcoord_mmr_reg_cnf.service_state = mm_serving_plmn.info;
           mmcoord_mmr_reg_cnf.cause = MMR_CAUSE_NOT_SPECIFIED;
           mm_send_mmr_reg_cnf();
           mm_reg_waiting_for_reg_cnf = FALSE;
           mm_ready_to_send_reg_cnf   = FALSE;      
         }
      }
      else if ((gmm_state == GMM_REGISTERED) &&
               (gmm_last_signalling_sent_rat == mm_serving_plmn.info.active_rat))
      {
        gmm_rau_is_required = FALSE;
        MSG_HIGH_DS_1(MM_SUB,"=MM= Reset flag to handle unnecessary RAU = %d",gmm_rau_is_required);
      }
    }
    else
    {
      MSG_HIGH_DS_0( MM_SUB, "=MM= UE is not registered yet");
    }
    if(prev_gmm_state != gmm_state)
    {
      gmm_registration_initiated = TRUE;
    }
  }
  else 
  {
    MSG_HIGH_DS_0( MM_SUB, "=MM= GPRS resumption Not handled in this sub state");
  }
}

/*===========================================================================

FUNCTION GMM_HANDLE_GPRS_RESUMPTION_DURING_G2W_HO
  This function handles the GPRS resumption to be performed during G2W
  handover

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void gmm_handle_gprs_resumption_during_g2w_ho ( void )
{
  if ( gmm_substate == GMM_SUSPENDED )
  {
    if((gmm_state == GMM_DEREGISTERED) && (gmm_saved_substate == GMM_NORMAL_SERVICE))
    {
      gmm_set_substate(GMM_ATTACH_NEEDED);
    }
    else
    {
      MSG_HIGH_DS_2( MM_SUB, "=MM= GMM substate change from %d to %d", gmm_substate, gmm_saved_substate );
      gmm_substate = gmm_saved_substate;
    }
  }
}
/*=============================================================================
FUNCTION GMM_GET_SAVED_SUBSTATE
  This function gives the gmm saved substate
  
DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  gmm_substate_type

SIDE EFFECTS
  None 
=============================================================================*/
gmm_substate_type gmm_get_saved_substate(void)
{
  return gmm_saved_substate;
}

#endif /* #ifdef FEATURE_GSM_GPRS */
