/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2001,2002,2003 Qualcomm Technologies, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mm/vcs/mmforbid.c_v   1.7   01 Jul 2002 11:30:06   jault  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/src/mmforbid.c#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/03/01   jca     Rewrote RRC<->MM plmn_identity_type conversion functions.
04/23/02   jca     Moved functions to eliminate prototypes from mm.h
05/09/02   mks     Ensure that Forbidden LAI updates are not sent to RR in
                   UMTS mode
06/05/02   ks      changed PLMN_id_T to sys_plmn_id_s_type to support dual
                   mode operation.
02/07/03   jca     Insure that Forbidden LAI updates are sent to RRC in UMTS mode.
06/16/03   jca     Now consistently ERR_FATAL if attempt to allocate memory fails.
08/23/06   up      Added function mm_rrc_plmn_lai_allowed & mm_rr_plmn_lai_forbidden.
08/28/06   up      Modified function mm_rrc_plmn_lai_allowed to return
                   LAI/PLMN as not allowed when both CS+PS domains are barred.
08/28/06   sn      Added functions for handling of LAI reject list.
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "mmcp_variation.h"
#include "customer.h"
#include "rex.h"
#include "msg.h"
#include "err.h"
#include "environ.h"
#include "sys_type.h"
#include "sys_cnst_v.h"
#include "sys_stru_v.h"
#include "ms.h"
#include "timers_v.h"
#include "gs_v.h"
#include "mm_v.h"
#include "rrcmmif.h"
#include "mm_umts_v.h"
#include "mm_coord.h"
#include "reg_sim_v.h"
#include "gmm_int_v.h"
#include "mm_sim.h"
#include <stringl/stringl.h>
#if(defined(TEST_FRAMEWORK) && defined(FEATURE_LTE))
 #error code not present
#endif // TEST_FRAMEWORK


/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/


typedef struct forbidden_list_tag
{
   inter_task_lai_T        lai[MAX_NO_FORBIDDEN_LAIS];
   byte                    write_ptr;
} forbidden_list_T;

/*
typedef struct lai_reject_list_tag
{
   inter_task_lai_T        lai[MAX_NO_FORBIDDEN_LAIS];
   byte                    write_ptr;
   byte                    length;
}lai_reject_list_T;
*/

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
 lai_reject_list_T   lai_reject_list_sim[MAX_NAS_STACKS];
forbidden_list_T    forbidden_list_sim[MAX_NAS_STACKS][2];

#define lai_reject_list (lai_reject_list_sim[mm_as_id])
#define forbidden_list (forbidden_list_sim[mm_as_id])

#else
static lai_reject_list_T   lai_reject_list;

static forbidden_list_T    forbidden_list[2];

#endif /* FEATURE_DUAL_SIM ||  FEATURE_SGLTE*/

#define NATIONAL_ROAMING   0
#define REGIONAL_ROAMING   1


/*===========================================================================

FUNCTION MM_SEND_RRC_FORBIDDEN_LAI_UPDATE

DESCRIPTION
  This function updates RRC with the current list of forbidden LAIs.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mm_send_rrc_forbidden_lai_update( void )
{
  if (mm_is_multimode_sub())
  {
#ifdef FEATURE_WCDMA
    rrc_cmd_type *rrc_cmd;
#endif
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    if (mm_wcdma_supported)
    {
#ifdef FEATURE_WCDMA
        rrc_cmd = mm_get_rrc_cmd_buf( SYS_RAT_UMTS_RADIO_ACCESS );

      if (rrc_cmd == NULL)
      {
        MSG_FATAL_DS( MM_SUB, "=MM= ERROR: Unable to allocate memory", 0,0,0 );
      }

      rrc_cmd->cmd_hdr.cmd_id = RRC_FORBIDDEN_LAI_LIST_UPDATE_REQ;
     
      fill_rrc_forbidden_lai_list( &rrc_cmd->cmd.forbid_lai_list_update_req.forbid_lai_list );

      send_cmd_to_rrc( rrc_cmd, SYS_RAT_UMTS_RADIO_ACCESS );
#endif /* FEATURE_WCDMA */
    }

    if (mm_tdscdma_supported)
    {
#ifdef FEATURE_WCDMA
        rrc_cmd = mm_get_rrc_cmd_buf( SYS_RAT_TDS_RADIO_ACCESS );


      if (rrc_cmd == NULL)
      {
        MSG_FATAL_DS( MM_SUB, "=MM= ERROR: Unable to allocate memory", 0,0,0 );
      }

      rrc_cmd->cmd_hdr.cmd_id = RRC_FORBIDDEN_LAI_LIST_UPDATE_REQ;

      fill_rrc_forbidden_lai_list( &rrc_cmd->cmd.forbid_lai_list_update_req.forbid_lai_list );

      send_cmd_to_rrc( rrc_cmd, SYS_RAT_TDS_RADIO_ACCESS );
#endif /* FEATURE_WCDMA */
    }
  }
}


/*===========================================================================

FUNCTION MM_SEND_RR_FORBIDDEN_LAI_UPDATE

DESCRIPTION
  This function updates rr with current list of forbidden LAIs

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mm_send_rr_forbidden_lai_update(void)
{
   byte                          no_of_entries;
   byte                          count;
   rr_forbidden_lai_update_req_T rr_forbidden_lai_update_req;

#if (defined(FEATURE_DUAL_SIM) && !defined(FEATURE_TWO_3GPP_SIM)) || defined(FEATURE_SGLTE)
   if(mm_is_gsm_allowed_on_sub())
#endif
   {
   no_of_entries = 0;
   memset(&rr_forbidden_lai_update_req,0x00,sizeof(rr_forbidden_lai_update_req_T));

   rr_forbidden_lai_update_req.message_header.message_set = MS_MM_RR;
   rr_forbidden_lai_update_req.message_header.message_id  = (int) RR_FORBIDDEN_LAI_UPDATE_REQ;


   for (count = 0; count < forbidden_list[NATIONAL_ROAMING].write_ptr; count++ )
   {
      (void)memscpy( &rr_forbidden_lai_update_req.forbidden_lai_list.location_area_identification[no_of_entries], LAI_SIZE,
              &forbidden_list[NATIONAL_ROAMING].lai[count], LAI_SIZE );

      no_of_entries++;
   }

#ifdef FEATURE_ENHANCED_NW_SELECTION
   for(count=0; count<mm_foreground_search_lai_timer_list.length; count++)
   {
     if((mm_foreground_search_lai_timer_list.lai_timer_list[count].is_ps_registered == FALSE) ||
        (mm_serving_plmn.info.active_rat == SYS_RAT_LTE_RADIO_ACCESS))
     {

       (void)memscpy( &rr_forbidden_lai_update_req.forbidden_lai_list.location_area_identification[no_of_entries],LAI_SIZE,
                               mm_foreground_search_lai_timer_list.lai_timer_list[count].lai, LAI_SIZE );

       no_of_entries++;
     }
   }
#endif
   rr_forbidden_lai_update_req.forbidden_lai_list.no_of_entries = no_of_entries;

#ifdef FEATURE_DUAL_SIM
   PUT_IMH_LEN( sizeof(rr_forbidden_lai_update_req_T) - sizeof(IMH_T),
                &rr_forbidden_lai_update_req.message_header );
#else
   PUT_IMH_LEN( (char *)&rr_forbidden_lai_update_req.forbidden_lai_list.
                              location_area_identification[no_of_entries] -
                (char *)&rr_forbidden_lai_update_req.forbidden_lai_list,
                &rr_forbidden_lai_update_req.message_header );
#endif

   MSG_HIGH_DS_0(MM_SUB, "=MM= MM sent RR_FORBIDDEN_LAI_UPDATE_REQ");
   mm_send_message( (IMH_T *)&rr_forbidden_lai_update_req, GS_QUEUE_RR );
   }
}


/*===========================================================================

FUNCTION MM_REMOVE_FORBIDDEN_ROAMING

DESCRIPTION
  This function removes the specified LAI from the forbidden list specified

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

static void mm_remove_forbidden_roaming( uint32 area, inter_task_lai_T *p_lai )
{
   byte  count;

   for (count = 0; count < forbidden_list[area].write_ptr; count++ )
   {
      if (memcmp( &forbidden_list[area].lai[count],
                  p_lai,
                  LAI_SIZE ) == 0 )    /* if match found */
      {

         /*
          * remove entry, and shuffle up
          */
         for ( ; (count < forbidden_list[area].write_ptr-1) && (count < MAX_NO_FORBIDDEN_LAIS-1); count++ )
         {
            (void)memscpy( &forbidden_list[area].lai[count],LAI_SIZE,
                    &forbidden_list[area].lai[count+1], LAI_SIZE );
         }

         forbidden_list[area].write_ptr = (byte)(forbidden_list[area].write_ptr - 1) ;

         if (area == NATIONAL_ROAMING)
         {
#ifdef FEATURE_WCDMA
               mm_send_rrc_forbidden_lai_update();
#endif

#ifdef FEATURE_GSM
               mm_send_rr_forbidden_lai_update();
#endif
         }
         break;
      }
   }
}


/*===========================================================================

FUNCTION MM_REMOVE_FORBIDDEN_NATIONAL_ROAMING

DESCRIPTION
  This function removes the specified LAI from the forbidden list for
  national roaming if present


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

VOID_FUNC mm_remove_forbidden_national_roaming( inter_task_lai_T * p_lai )
{

   mm_remove_forbidden_roaming( NATIONAL_ROAMING, p_lai );

}


/*===========================================================================

FUNCTION MM_REMOVE_FORBIDDEN_REGIONAL_ROAMING

DESCRIPTION
  This function removes the specified LAI from the forbidden list for
  regional roaming if present

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mm_remove_forbidden_regional_roaming( inter_task_lai_T * p_lai )
{

   mm_remove_forbidden_roaming( REGIONAL_ROAMING, p_lai );
}


/*===========================================================================

FUNCTION MM_CHECK_FORBIDDEN_ROAMING

DESCRIPTION
  This function checks if the specified LAI is in the forbidden list specified

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

boolean  mm_check_forbidden_roaming( byte area, inter_task_lai_T * p_lai )
{
   byte     count;
   boolean  return_value = FALSE;

   for (count = 0; count < forbidden_list[area].write_ptr; count++ )
   {
      if (memcmp( &forbidden_list[area].lai[count],
                  p_lai,
                  LAI_SIZE ) == 0 )    /* if match found */
      {
         return_value = TRUE;
         break;
      }
   }

   return return_value;

}

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
static boolean  mm_per_subs_check_forbidden_roaming
(
  byte area, 
  inter_task_lai_T * p_lai,
  sys_modem_as_id_e_type as_id
)
{
   byte     count;
   boolean  return_value = FALSE;

  if (IS_NOT_VALID_STACK_ID(as_id))
  {
    MSG_ERROR_DS(MM_SUB,"mm_per_subs_check_forbidden_roaming - Invalid STACK ID: %d", as_id+1,0,0);
    return FALSE;
  }

   for (count = 0; count < forbidden_list_sim[as_id][area].write_ptr; count++ )
   {
      if (memcmp( &forbidden_list_sim[as_id][area].lai[count],
                  p_lai,
                  LAI_SIZE ) == 0 )    /* if match found */
      {
         return_value = TRUE;
         break;
      }
   }

   return return_value;

}

boolean mmsim_per_subs_sim_valid
(
  sys_srv_domain_e_type domain,
  sys_modem_as_id_e_type as_id
)
{
  boolean status = FALSE;

  /* ---------------------------------------------------
  ** Return status based on the domain and the SIM state
  ** --------------------------------------------------- */
  switch (domain)
  {
    case SYS_SRV_DOMAIN_CS_ONLY:
      if ((mm_serving_plmn_sim[as_id].info.sim_state == SYS_SIM_STATE_AVAILABLE ) ||
          (mm_serving_plmn_sim[as_id].info.sim_state == SYS_SIM_STATE_PS_INVALID))
      {
        status = TRUE;
      }
      break;

    case SYS_SRV_DOMAIN_PS_ONLY:
      if ((mm_serving_plmn_sim[as_id].info.sim_state == SYS_SIM_STATE_AVAILABLE ) ||
          (mm_serving_plmn_sim[as_id].info.sim_state == SYS_SIM_STATE_CS_INVALID))
      {
        status = TRUE;
      }
      break;

    case SYS_SRV_DOMAIN_CS_PS:
      if ((mm_serving_plmn_sim[as_id].info.sim_state == SYS_SIM_STATE_AVAILABLE ) ||
          (mm_serving_plmn_sim[as_id].info.sim_state == SYS_SIM_STATE_CS_INVALID) ||
          (mm_serving_plmn_sim[as_id].info.sim_state == SYS_SIM_STATE_PS_INVALID))
      {
        status = TRUE;
      }
      break;

    default:
      MSG_FATAL_DS( MM_LOCAL_SUB, "=MM= Unknown SIM validation type", 0,0,0 );
  }

  return status;
}

#endif

/*===========================================================================

FUNCTION MM_ADD_FORBIDDEN_ROAMING

DESCRIPTION
  This function adds the specified LAI to the forbidden list specified

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mm_add_forbidden_roaming( byte area, inter_task_lai_T * p_lai )
{
   byte count;
   /*
    * only add if not already present
    */

   if ( mm_check_forbidden_roaming( area, p_lai ) == FALSE )
   {

      /*
       * if list is full, overwrite the oldest entry
       */

      if ( forbidden_list[area].write_ptr == MAX_NO_FORBIDDEN_LAIS )
      {

         /*
          * list is full, so loose the last entry
          */
         for(count = 0; (count < forbidden_list[area].write_ptr-1) && (count < MAX_NO_FORBIDDEN_LAIS-1); count++)
         {
            (void)memscpy( &forbidden_list[area].lai[count], LAI_SIZE,
                          &forbidden_list[area].lai[count+1], LAI_SIZE );
         }
         (void)memscpy( &forbidden_list[area].lai[forbidden_list[area].write_ptr-1], LAI_SIZE,
                       p_lai, LAI_SIZE );

      }
      else
      {
         (void)memscpy( &forbidden_list[area].lai[forbidden_list[area].write_ptr], LAI_SIZE,
                       p_lai, LAI_SIZE );
         forbidden_list[area].write_ptr = (byte)(forbidden_list[area].write_ptr + 1) ;
      }

      if (area == NATIONAL_ROAMING)
      {
#ifdef FEATURE_WCDMA
            mm_send_rrc_forbidden_lai_update();
#endif
          
#ifdef FEATURE_GSM
            mm_send_rr_forbidden_lai_update();
#endif
      }
   }
}


/*===========================================================================

FUNCTION MM_ADD_FORBIDDEN_NATIONAL_ROAMING

DESCRIPTION
  This function adds the specified LAI to the forbidden list for national

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mm_add_forbidden_national_roaming( inter_task_lai_T * p_lai )
{
   mm_add_forbidden_roaming( NATIONAL_ROAMING, p_lai );
}


/*===========================================================================

FUNCTION MM_ADD_FORBIDDEN_REGIONAL_ROAMING

DESCRIPTION
  This function adds the specified LAI to the forbidden list for regional

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

VOID_FUNC mm_add_forbidden_regional_roaming( inter_task_lai_T * p_lai )
{
   mm_add_forbidden_roaming( REGIONAL_ROAMING, p_lai );
}


/*===========================================================================

FUNCTION MM_CHECK_FORBIDDEN_NATIONAL_ROAMING

DESCRIPTION
  This function checks if the specified LAI is in the forbidden list for
  national roaming

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

boolean  mm_check_forbidden_national_roaming( inter_task_lai_T * p_lai )
{
   return mm_check_forbidden_roaming( NATIONAL_ROAMING, p_lai );
}

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
boolean  mm_per_subs_check_forbidden_national_roaming
(
  inter_task_lai_T * p_lai, 
  sys_modem_as_id_e_type as_id 
)
{
   return mm_per_subs_check_forbidden_roaming( NATIONAL_ROAMING, p_lai, as_id );
}
#endif
/*===========================================================================

FUNCTION mm_check_forbidden_regional_roaming_nas_lai

DESCRIPTION
  This function checks if the specified LAI is in the forbidden list for
  regional roaming, internal to NAS.

DEPENDENCIES
  None

RETURN VALUE
boolean 
 
SIDE EFFECTS
  None

===========================================================================*/
boolean mm_check_forbidden_regional_roaming_nas_lai(void)
{
  inter_task_lai_T            p_lai;

  memscpy(&p_lai.PLMN_id,PLMN_SIZE,&mm_serving_plmn.info.plmn,PLMN_SIZE);
  memscpy(&p_lai.location_area_code,LAC_SIZE,&mm_serving_plmn.lac,LAC_SIZE);
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
  return mm_per_subs_check_forbidden_roaming( REGIONAL_ROAMING, &p_lai, (sys_modem_as_id_e_type)mm_as_id );
#else
  return mm_check_forbidden_roaming( REGIONAL_ROAMING, &p_lai );
#endif
}
/*===========================================================================

FUNCTION MM_CHECK_FORBIDDEN_REGIONAL_ROAMING

DESCRIPTION
  This function checks if the specified LAI is in the forbidden list for
  regional roaming


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

boolean  mm_check_forbidden_regional_roaming( inter_task_lai_T * p_lai )
{
   return mm_check_forbidden_roaming( REGIONAL_ROAMING, p_lai );
}

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
boolean  mm_per_subs_check_forbidden_regional_roaming
(
  inter_task_lai_T * p_lai, 
  sys_modem_as_id_e_type as_id 
)
{
   return mm_per_subs_check_forbidden_roaming( REGIONAL_ROAMING, p_lai, as_id );
}
#endif

/*===========================================================================

FUNCTION MM_CLEAR_FORBIDDEN_LAIS

DESCRIPTION
  This function clears all the forbidden LAIs

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mm_clear_forbidden_LAIs(void)
{
   byte     area;
   boolean  rr_update_required = FALSE;

   for (area = 0; area < 2; area++ )
   {
      if ((forbidden_list[area].write_ptr != 0)
         &&
         ( area == NATIONAL_ROAMING ))
      {
         rr_update_required = TRUE;
      }

      forbidden_list[area].write_ptr = 0;
   }

   if (rr_update_required)
   {
#ifdef FEATURE_WCDMA
        mm_send_rrc_forbidden_lai_update();
#endif
                 
#ifdef FEATURE_GSM
         mm_send_rr_forbidden_lai_update();
#endif
   }
}


/*===========================================================================

FUNCTION FILL_RRC_FORBIDDEN_LAI_LIST

DESCRIPTION
  This function fills internal forbidden LAI list to the rrc list

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void fill_rrc_forbidden_lai_list(rrc_lai_list_type *list_p)
{
   byte      no_of_entries = 0;
   byte      count;
   sys_plmn_id_s_type plmn_id;

   for (count = 0; count < forbidden_list[NATIONAL_ROAMING].write_ptr; count++)
   {
      plmn_id.identity[0] = forbidden_list[NATIONAL_ROAMING].lai[count].PLMN_id.identity[0];
      plmn_id.identity[1] = forbidden_list[NATIONAL_ROAMING].lai[count].PLMN_id.identity[1];
      plmn_id.identity[2] = forbidden_list[NATIONAL_ROAMING].lai[count].PLMN_id.identity[2];
      list_p->lai[no_of_entries].plmn =
         mmumts_convert_nas_plmn_id_to_rrc_plmn_id( plmn_id );

      list_p->lai[no_of_entries].lac[0] =
         forbidden_list[NATIONAL_ROAMING].lai[count].location_area_code[0];
      list_p->lai[no_of_entries].lac[1] =
         forbidden_list[NATIONAL_ROAMING].lai[count].location_area_code[1];

      no_of_entries++;
   }

 #ifdef FEATURE_ENHANCED_NW_SELECTION
   for(count=0; count<mm_foreground_search_lai_timer_list.length; count++)
   {
     if((mm_foreground_search_lai_timer_list.lai_timer_list[count].is_ps_registered == FALSE) ||
        (mm_serving_plmn.info.active_rat == SYS_RAT_LTE_RADIO_ACCESS))
     {
       plmn_id.identity[0] = mm_foreground_search_lai_timer_list.lai_timer_list[count].lai[0];
       plmn_id.identity[1] = mm_foreground_search_lai_timer_list.lai_timer_list[count].lai[1];
       plmn_id.identity[2] = mm_foreground_search_lai_timer_list.lai_timer_list[count].lai[2];
       list_p->lai[no_of_entries].plmn =
          mmumts_convert_nas_plmn_id_to_rrc_plmn_id( plmn_id );

       list_p->lai[no_of_entries].lac[0] =
         mm_foreground_search_lai_timer_list.lai_timer_list[count].lai[3];
       list_p->lai[no_of_entries].lac[1] =
          mm_foreground_search_lai_timer_list.lai_timer_list[count].lai[4];

       no_of_entries++;
     }
   }
#endif
   list_p->num_lais = no_of_entries;
}

/*===========================================================================

FUNCTION MM_RRC_PLMN_LAI_ALLOWED

DESCRIPTION
  This function checks if the specified PLMN/LAI is alowed for CS or PS registration.
  This function checks if PLMN capability is matching with UE capability and
  the service domain is allowed (not barred) for registration or not.
  Moreover, in AUTOMATIC mode registrayion is allowed if LAI/PLMN is not in forbidden list.

DEPENDENCIES
  None

RETURN VALUE
  TRUE : if either CS or PS registration is possible/allowed in this LAI/PLMN.
  FALSE: Otherwise.


SIDE EFFECTS
  None

===========================================================================*/
boolean  mm_rrc_plmn_lai_allowed(rrc_service_info_type service_info)
{
#ifdef FEATURE_DUAL_SIM 
  MSG_FATAL_DS( MM_SUB, "=MM= DUAL SIM: Wrong API called mm_rrc_plmn_lai_allowed()", 0,0,0 );
  return FALSE;
#elif defined(FEATURE_SGLTE)
  return mm_rrc_per_subs_plmn_lai_allowed(service_info, mm_ps_stack_id);
#else

  boolean is_forbidden = FALSE;
  inter_task_lai_T lai_info;
  /* 0x00 - CS+PS not allowed, 0x01 CS Allowed, 0x02 PS Allowed, 0x03 CS+PS allowed */
  byte domain_allowed = 0x00; 
  uint32 i;

  /*
  ** Extract LAI from serving info.
  */
  lai_info.PLMN_id = mmumts_convert_rrc_plmn_id_to_nas_plmn_id(service_info.selected_plmn.plmn_id);
  lai_info.location_area_code[0] = service_info.gsm_map_common_info.value[0];
  lai_info.location_area_code[1] = service_info.gsm_map_common_info.value[1];

  /*
  ** Check for UE and PLMN capability
  ** and Cell access if registration is allowed or not.
  */

  for (i=0; i < service_info.num_cn_domains; i++)
  {
    if (service_info.cn_domain_info[i].cn_domain_id == RRC_PS_DOMAIN_CN_ID)
    {
      if ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_PS_ONLY) ||
          (mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS))
      {
        if (gmm_check_if_ps_cell_access_allowed(mm_get_ps_cell_access_from_rrc_service_info(&service_info)))
        {
          domain_allowed |= 0x02;
        }
      }
    }
    else if (service_info.cn_domain_info[i].cn_domain_id == RRC_CS_DOMAIN_CN_ID)
    {
      if ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_ONLY) ||
          (mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS))
      {
        if (mm_check_if_cs_cell_access_allowed(mm_get_cs_cell_access_from_rrc_service_info(&service_info)))
        {
          domain_allowed |= 0x01;
        }
      }
    }
  }

  /*
  ** If SIM is invalid for CS service, CS registration is not performed
  */
  if (!mmsim_sim_valid(SYS_SRV_DOMAIN_CS_ONLY))
  {
    domain_allowed |= 0xFE; /* Toggle CS allowed domain (~0x01) */
  }

  /*
  ** If SIM is invalid for PS service, PS registration is not performed
  */
  if (!mmsim_sim_valid(SYS_SRV_DOMAIN_PS_ONLY))
  {
    domain_allowed |= 0xFD; /* Toggle PS allowed domain (~0x02) */
  }
  
  /*
  ** LAI/PLMN is considered forbidden if present in forbidden PLMN list
  ** or in forbidden LAI list in AUTOMATIC mode.
  */
  if (domain_allowed)
  {
    switch (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode)
    {
      case SYS_NETWORK_SELECTION_MODE_AUTOMATIC:
      case SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION:
        if ((mm_check_forbidden_national_roaming(&lai_info)) ||
            (mm_check_forbidden_regional_roaming(&lai_info)) ||
            (reg_sim_plmn_forbidden(lai_info.PLMN_id)) ||
            ((domain_allowed == 0x02) && /* Only PS domain is allowed */
             (reg_sim_plmn_gprs_forbidden(lai_info.PLMN_id))) 
           )
        {
          is_forbidden = TRUE;
        }
        break;

      case SYS_NETWORK_SELECTION_MODE_MANUAL:
        if ((reg_sim_plmn_forbidden(lai_info.PLMN_id)) ||
            ((domain_allowed == 0x02) && /* Only PS domain is allowed */
             (reg_sim_plmn_gprs_forbidden(lai_info.PLMN_id))))
        {
          is_forbidden = TRUE;
        }
        break;

      case SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION:
        if (!(PLMN_MATCH(mmcoord_pending_reg_message.cmd.mmr_reg_req.plmn.identity, 
                         lai_info.PLMN_id.identity)))
        {
          is_forbidden = TRUE;
        }
        break;

      case SYS_NETWORK_SELECTION_MODE_LIMITED:
      default:
        is_forbidden = TRUE;
        break;
    }
  }
  else
  {
    /* CS/PS domain is not allowed, so the LAI/PLMN is not allowed for registration */
    is_forbidden = TRUE;
  }

  MSG_HIGH_DS_1(MM_SUB, "=MM= mm_rrc_plmn_lai_allowed returns %d",(!is_forbidden));

  return (boolean)(!is_forbidden);

#endif

}
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
boolean  mm_rrc_per_subs_plmn_lai_allowed
(
  rrc_service_info_type service_info,
  sys_modem_as_id_e_type sub_id
)
{

  boolean is_forbidden = FALSE;
  inter_task_lai_T lai_info;
  /* 0x00 - CS+PS not allowed, 0x01 CS Allowed, 0x02 PS Allowed, 0x03 CS+PS allowed */
  byte domain_allowed = 0x00; 
  uint32 i;
  mm_as_id_e_type as_id;

  if (IS_NOT_VALID_SUB_ID(sub_id))
  {
    MSG_ERROR_DS(MM_SUB,"mm_rrc_per_subs_plmn_lai_allowed - Invalid AS ID: %d", sub_id+1,0,0);
    return FALSE;
  }

#if defined(FEATURE_SGLTE)
  if(MM_SUB_IS_SGLTE_SUB(sub_id))
  {
    as_id = mm_ps_stack_id;
  }
  else
#endif
  {
    as_id = (mm_as_id_e_type)sub_id;
  }

  /*
  ** Extract LAI from serving info.
  */
  lai_info.PLMN_id = mmumts_convert_rrc_plmn_id_to_nas_plmn_id(service_info.selected_plmn.plmn_id);
  lai_info.location_area_code[0] = service_info.gsm_map_common_info.value[0];
  lai_info.location_area_code[1] = service_info.gsm_map_common_info.value[1];

  /*
  ** Check for UE and PLMN capability
  ** and Cell access if registration is allowed or not.
  */

  for (i=0; i < service_info.num_cn_domains; i++)
  {
    if (service_info.cn_domain_info[i].cn_domain_id == RRC_PS_DOMAIN_CN_ID)
    {
      if ((mm_serving_plmn_sim[as_id].ms_op_mode == SYS_SRV_DOMAIN_PS_ONLY) ||
          (mm_serving_plmn_sim[as_id].ms_op_mode == SYS_SRV_DOMAIN_CS_PS))
      {
        if (gmm_check_if_ps_cell_access_allowed(mm_get_ps_cell_access_from_rrc_service_info(&service_info)))
        {
          domain_allowed |= 0x02;
        }
      }
    }
    else if (service_info.cn_domain_info[i].cn_domain_id == RRC_CS_DOMAIN_CN_ID)
    {
      if ((mm_serving_plmn_sim[as_id].ms_op_mode == SYS_SRV_DOMAIN_CS_ONLY) ||
          (mm_serving_plmn_sim[as_id].ms_op_mode == SYS_SRV_DOMAIN_CS_PS))
      {
        if (mm_check_if_cs_cell_access_allowed(mm_get_cs_cell_access_from_rrc_service_info(&service_info)))
        {
          domain_allowed |= 0x01;
        }
      }
    }
  }

  /*
  ** If SIM is invalid for CS service, CS registration is not performed
  */
  if (!mmsim_per_subs_sim_valid(SYS_SRV_DOMAIN_CS_ONLY,(sys_modem_as_id_e_type)as_id))
  {
    domain_allowed |= 0xFE; /* Toggle CS allowed domain (~0x01) */
  }

  /*
  ** If SIM is invalid for PS service, PS registration is not performed
  */
  if (!mmsim_per_subs_sim_valid(SYS_SRV_DOMAIN_PS_ONLY,(sys_modem_as_id_e_type)as_id))
  {
    domain_allowed |= 0xFD; /* Toggle PS allowed domain (~0x02) */
  }
  
  /*
  ** LAI/PLMN is considered forbidden if present in forbidden PLMN list
  ** or in forbidden LAI list in AUTOMATIC mode.
  */
  if (domain_allowed)
  {
    switch (mmcoord_pending_reg_message_sim[as_id].cmd.mmr_reg_req.network_selection_mode)
    {
      case SYS_NETWORK_SELECTION_MODE_AUTOMATIC:
      case SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION:
        if ((mm_per_subs_check_forbidden_national_roaming(&lai_info,(sys_modem_as_id_e_type)as_id)) ||
            (mm_per_subs_check_forbidden_regional_roaming(&lai_info,(sys_modem_as_id_e_type)as_id)) ||
#ifdef FEATURE_DUAL_SIM
              (reg_sim_per_subs_plmn_forbidden(lai_info.PLMN_id,sub_id)) ||
#else
              (reg_sim_plmn_forbidden(lai_info.PLMN_id)) ||
#endif
            ((domain_allowed == 0x02) && /* Only PS domain is allowed */
#ifdef FEATURE_DUAL_SIM
             (reg_sim_plmn_gprs_forbidden(lai_info.PLMN_id,sub_id))
#else
             (reg_sim_plmn_gprs_forbidden(lai_info.PLMN_id))
#endif
            ))
        {
          is_forbidden = TRUE;
        }
        break;

      case SYS_NETWORK_SELECTION_MODE_MANUAL:
        if (
#ifdef FEATURE_DUAL_SIM
            (reg_sim_per_subs_plmn_forbidden(lai_info.PLMN_id,sub_id)) ||
#else
            (reg_sim_plmn_forbidden(lai_info.PLMN_id)) ||
#endif
        
            ((domain_allowed == 0x02) && /* Only PS domain is allowed */
#ifdef FEATURE_DUAL_SIM
             (reg_sim_plmn_gprs_forbidden(lai_info.PLMN_id,sub_id))
#else
             (reg_sim_plmn_gprs_forbidden(lai_info.PLMN_id))
#endif
           ))
        {
          is_forbidden = TRUE;
        }
        break;

      case SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION:
        if (!(PLMN_MATCH(mmcoord_pending_reg_message_sim[as_id].cmd.mmr_reg_req.plmn.identity, 
                         lai_info.PLMN_id.identity)))
        {
          is_forbidden = TRUE;
        }
        break;

      case SYS_NETWORK_SELECTION_MODE_LIMITED:
      default:
        is_forbidden = TRUE;
        break;
    }
  }
  else
  {
    /* CS/PS domain is not allowed, so the LAI/PLMN is not allowed for registration */
    is_forbidden = TRUE;
  }
  
  return (boolean)(!is_forbidden);

}
#endif

/*===========================================================================

FUNCTION MM_RRC_REGISTRATION_IN_PROGRESS

DESCRIPTION
  This function returns registration status
  
DEPENDENCIES
  None

RETURN VALUE
  TRUE : registration in progress
  FALSE: registration is not in progress


SIDE EFFECTS
  This function shall be called only iff LAI is allowed according to mm_rrc_plmn_lai_allowed

===========================================================================*/

boolean mm_rrc_registration_in_progress()
{

#ifdef FEATURE_SGLTE
  return mm_per_subs_rrc_registration_in_progress((sys_modem_as_id_e_type)mm_ps_stack_id);
#else
  boolean reg_in_progress = FALSE;
  
  if(mm_reg_waiting_for_reg_cnf)
  {
    reg_in_progress = TRUE;
  }

  MSG_HIGH_DS_1(MM_SUB ,"=MM= mm_rrc_registration_in_progress(only applicable to WRLF) returns %d",reg_in_progress);

  return reg_in_progress;
#endif
}

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
boolean mm_per_subs_rrc_registration_in_progress(sys_modem_as_id_e_type sub_id)
{
  boolean reg_in_progress = FALSE;
  mm_as_id_e_type stack_id_nas = (mm_as_id_e_type)sub_id;
  
  if (IS_NOT_VALID_SUB_ID(sub_id))
  {
    MSG_ERROR_DS(MM_SUB,"mm_per_subs_rrc_registration_in_progress - Invalid AS ID: %d", sub_id+1,0,0);
    return FALSE;
  }
#if defined(FEATURE_SGLTE)
  if(MM_SUB_IS_SGLTE_SUB((mm_as_id_e_type)sub_id))  
  {
    stack_id_nas = mm_ps_stack_id;
  }
#endif

  if(mm_reg_waiting_for_reg_cnf_sim[stack_id_nas])
  {
    reg_in_progress = TRUE;
  }

  MSG_HIGH_DS_1( sub_id, "=MM= mm_per_subs_rrc_registration_in_progress(only applicable to WRLF) returns %d",reg_in_progress);

  return reg_in_progress;
}
#endif

#ifdef FEATURE_DEEP_SLEEP_REL6_OUT_OF_SERVICE_ENHANCEMENT
/*===========================================================================

FUNCTION MM_RR_PLMN_LAI_FORBIDDEN

DESCRIPTION
  This function checks if the specified PLMN/LAI is alowed for CS or PS registration.
  In AUTOMATIC mode registrayion is allowed if LAI/PLMN is not in forbidden list.
  Assumption: This function is used by RR only. RR takes care to camp on a Cell which is
  capable of domains asked by user. Also RR shall take care to camp on a Cell whose access is allowed.

  This function receives the subscription ID and NAS stack ID and used it to check the NAS variables.

DEPENDENCIES
  None

RETURN VALUE
  FALSE: if either CS or PS registration is possible/allowed in this LAI/PLMN.
  TRUE : Otherwise.


SIDE EFFECTS
  None

===========================================================================*/
boolean  mm_rr_plmn_lai_forbidden_per_nas_stacks( 
  inter_task_lai_T lai_info,
  sys_modem_as_id_e_type           sub_id,
  sys_modem_as_id_e_type           stack_id_nas
)
{
#if !defined(FEATURE_DUAL_SIM) && !defined (FEATURE_SGLTE)
  (void)stack_id_nas;
  (void)sub_id;
  return mm_rr_plmn_lai_forbidden(lai_info);
#else

  boolean is_forbidden = FALSE;
  if (IS_NOT_VALID_STACK_ID(stack_id_nas) ||
      IS_NOT_VALID_SUB_ID(sub_id))
  {
    MSG_ERROR_DS(MM_SUB,"mm_rr_plmn_lai_forbidden_per_subs - Invalid STACK ID: %d or SUB ID: %d", 
                       stack_id_nas+1,sub_id+1,0);
    return FALSE;
  }

  /*
  ** If SIM is invalid for CS service, CS registration is not performed
  */
  if ((!mmsim_per_subs_sim_valid(SYS_SRV_DOMAIN_CS_ONLY,stack_id_nas)) &&
      ((mm_serving_plmn_sim[stack_id_nas].ms_op_mode == SYS_SRV_DOMAIN_CS_ONLY) ||
       (((mm_serving_plmn_sim[stack_id_nas].ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
#ifdef FEATURE_DUAL_SIM
          (reg_sim_plmn_gprs_forbidden(lai_info.PLMN_id,sub_id))
#else
          (reg_sim_plmn_gprs_forbidden(lai_info.PLMN_id))
#endif
         ))))
  {
    return TRUE;
  }

  /*
  ** If SIM is invalid for PS service, PS registration is not performed
  */
  if ((!mmsim_per_subs_sim_valid(SYS_SRV_DOMAIN_PS_ONLY,stack_id_nas)) &&
      (mm_serving_plmn_sim[stack_id_nas].ms_op_mode == SYS_SRV_DOMAIN_PS_ONLY))
  {
    return TRUE;
  }

  /*
  ** LAI/PLMN is considered forbidden if present in forbidden PLMN list
  ** or in forbidden LAI list in AUTOMATIC mode.
  */
  switch (mmcoord_pending_reg_message_sim[stack_id_nas].cmd.mmr_reg_req.network_selection_mode)
  {
    case SYS_NETWORK_SELECTION_MODE_AUTOMATIC:
    case SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION:
      if ((mm_per_subs_check_forbidden_national_roaming(&lai_info,stack_id_nas)) ||
          (mm_per_subs_check_forbidden_regional_roaming(&lai_info,stack_id_nas)) 
#ifdef FEATURE_DUAL_SIM
          ||(reg_sim_per_subs_plmn_forbidden(lai_info.PLMN_id,sub_id)) ||
#else
          ||(reg_sim_plmn_forbidden(lai_info.PLMN_id)) ||
#endif
          ((mm_serving_plmn_sim[stack_id_nas].ms_op_mode == SYS_SRV_DOMAIN_PS_ONLY) &&
#ifdef FEATURE_DUAL_SIM
             (reg_sim_plmn_gprs_forbidden(lai_info.PLMN_id,sub_id))
#else
             (reg_sim_plmn_gprs_forbidden(lai_info.PLMN_id))
#endif
          ))
      {
        is_forbidden = TRUE;
      }
      break;

    case SYS_NETWORK_SELECTION_MODE_MANUAL:
      if (
#ifdef FEATURE_DUAL_SIM
          (reg_sim_per_subs_plmn_forbidden(lai_info.PLMN_id,sub_id)) ||
#else
          (reg_sim_plmn_forbidden(lai_info.PLMN_id)) ||
#endif

          ((mm_serving_plmn_sim[stack_id_nas].ms_op_mode == SYS_SRV_DOMAIN_PS_ONLY) &&
#ifdef FEATURE_DUAL_SIM
             (reg_sim_plmn_gprs_forbidden(lai_info.PLMN_id,sub_id))
#else
             (reg_sim_plmn_gprs_forbidden(lai_info.PLMN_id))
#endif
          ))
      {
        is_forbidden = TRUE;
      }
      break;

    case SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION:
      if (!(PLMN_MATCH(mmcoord_pending_reg_message_sim[stack_id_nas].cmd.mmr_reg_req.plmn.identity, 
                       lai_info.PLMN_id.identity)))
      {
        is_forbidden = TRUE;
      }
      break;

    case SYS_NETWORK_SELECTION_MODE_LIMITED:
    default:
      is_forbidden = TRUE;
      break;
  }
  
  return is_forbidden;
#endif
}


/*===========================================================================

FUNCTION MM_RR_PLMN_LAI_FORBIDDEN

DESCRIPTION
  This function checks if the specified PLMN/LAI is alowed for CS or PS registration.
  In AUTOMATIC mode registrayion is allowed if LAI/PLMN is not in forbidden list.
  Assumption: This function is used by RR only. RR takes care to camp on a Cell which is
  capable of domains asked by user. Also RR shall take care to camp on a Cell whose access is allowed.

DEPENDENCIES
  None

RETURN VALUE
  FALSE: if either CS or PS registration is possible/allowed in this LAI/PLMN.
  TRUE : Otherwise.


SIDE EFFECTS
  None

===========================================================================*/
boolean  mm_rr_plmn_lai_forbidden_per_subs( 
  inter_task_lai_T lai_info,
  sys_modem_as_id_e_type           sub_id
)
{
  sys_modem_as_id_e_type stack_id_nas = sub_id;
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
  if (IS_NOT_VALID_SUB_ID(sub_id))
  {
    MSG_ERROR_DS(MM_SUB,"mm_rr_plmn_lai_forbidden_per_subs - Invalid SUB ID: %d", sub_id+1,0,0);
    return FALSE;
  }
#if defined(FEATURE_SGLTE)
  if(MM_SUB_IS_SGLTE_SUB(sub_id))
  {
    stack_id_nas = (sys_modem_as_id_e_type)mm_cs_stack_id;
  }
#endif
#endif
  return mm_rr_plmn_lai_forbidden_per_nas_stacks(lai_info,sub_id,stack_id_nas);
}

boolean  mm_rr_plmn_lai_forbidden( inter_task_lai_T lai_info)
{
#ifdef FEATURE_DUAL_SIM

  MSG_FATAL_DS( MM_SUB, "=MM= DUAL SIM: Wrong API called mm_rr_plmn_lai_forbidden()", 0,0,0 );

  return FALSE;
#elif defined(FEATURE_SGLTE)
  return mm_rr_plmn_lai_forbidden_per_subs(lai_info, mm_cs_stack_id);
#else

  boolean is_forbidden = FALSE;

  /*
  ** If SIM is invalid for CS service, CS registration is not performed
  */
  if ((!mmsim_sim_valid(SYS_SRV_DOMAIN_CS_ONLY)) &&
      ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_ONLY) ||
       (((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
        (reg_sim_plmn_gprs_forbidden(lai_info.PLMN_id))))))
  {
    return TRUE;
  }

  /*
  ** If SIM is invalid for PS service, PS registration is not performed
  */
  if ((!mmsim_sim_valid(SYS_SRV_DOMAIN_PS_ONLY)) &&
      (mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_PS_ONLY))
  {
    return TRUE;
  }

  /*
  ** LAI/PLMN is considered forbidden if present in forbidden PLMN list
  ** or in forbidden LAI list in AUTOMATIC mode.
  */
  switch (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode)
  {
    case SYS_NETWORK_SELECTION_MODE_AUTOMATIC:
    case SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION:
      if ((mm_check_forbidden_national_roaming(&lai_info)) ||
          (mm_check_forbidden_regional_roaming(&lai_info)) ||
          (reg_sim_plmn_forbidden(lai_info.PLMN_id)) ||
          ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_PS_ONLY) &&
           (reg_sim_plmn_gprs_forbidden(lai_info.PLMN_id))))
      {
        is_forbidden = TRUE;
      }
      break;

    case SYS_NETWORK_SELECTION_MODE_MANUAL:
      if ((reg_sim_plmn_forbidden(lai_info.PLMN_id)) ||
          ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_PS_ONLY) &&
           (reg_sim_plmn_gprs_forbidden(lai_info.PLMN_id))))
      {
        is_forbidden = TRUE;
      }
      break;

    case SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION:
      if (!(PLMN_MATCH(mmcoord_pending_reg_message.cmd.mmr_reg_req.plmn.identity, 
                       lai_info.PLMN_id.identity)))
      {
        is_forbidden = TRUE;
      }
      break;

    case SYS_NETWORK_SELECTION_MODE_LIMITED:
    default:
      is_forbidden = TRUE;
      break;
  }
  
  return is_forbidden;

#endif


}
#endif

/*===========================================================================

FUNCTION MM_CHECK_LAI_IN_REJECT_LIST

DESCRIPTION
  This function checks if LAI is in reject list.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean mm_check_lai_in_reject_list(inter_task_lai_T mm_lai)
{
  int i;
#ifdef FEATURE_DUAL_SIM
  MSG_FATAL_DS( MM_SUB, "=MM= DUAL SIM: Wrong API called mm_check_lai_in_reject_list()", 0,0,0 );
  return FALSE;
#endif
#ifdef FEATURE_SGLTE
  return mm_per_subs_check_lai_in_reject_list( mm_lai, (sys_modem_as_id_e_type)mm_ps_stack_id);
#endif

  for(i=0;i<lai_reject_list.length;i++)
  {
    if(memcmp(&lai_reject_list.lai[i],
              &mm_lai,
              LAI_SIZE ) == 0 )    /* if match found */
    {
      MSG_MED_DS(MM_SUB, "=MM= LAI found in Reject List. PLMN [%X %X %X]",
               mm_lai.PLMN_id.identity[0], mm_lai.PLMN_id.identity[1],
               mm_lai.PLMN_id.identity[2]);
      MSG_MED_DS(MM_SUB, "=MM= LAC [%X %X]", mm_lai.location_area_code[0],
               mm_lai.location_area_code[1],0);
      return TRUE;
    }
  }

  return FALSE;
}

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
boolean mm_per_subs_check_lai_in_reject_list
(
  inter_task_lai_T mm_lai,
  sys_modem_as_id_e_type as_id
)
{
  int i;
  if(IS_NOT_VALID_STACK_ID(as_id))
  {
    MSG_ERROR_DS(MM_LOCAL_SUB, "=MM= Invalid AS ID:%d", as_id+1,0,0);
    return FALSE;
  }

#if defined(FEATURE_SGLTE)
  if(MM_SUB_IS_SGLTE_SUB(as_id))
  {
    as_id = (sys_modem_as_id_e_type)mm_ps_stack_id;
  }
#endif

  for(i=0;i<lai_reject_list_sim[as_id].length;i++)
  {
    if(memcmp(&lai_reject_list_sim[as_id].lai[i],
              &mm_lai,
              LAI_SIZE ) == 0 )    /* if match found */
    {
      MSG_MED_DS(MM_LOCAL_SUB, "=MM= LAI found in Reject List. PLMN [%X %X %X]",
               mm_lai.PLMN_id.identity[0], mm_lai.PLMN_id.identity[1],
               mm_lai.PLMN_id.identity[2]);
      MSG_MED_DS(MM_LOCAL_SUB, "=MM= LAC [%X %X]", mm_lai.location_area_code[0],
               mm_lai.location_area_code[1],0);
      return TRUE;
    }
  }

  return FALSE;
}

/*===========================================================================

FUNCTION mm_per_subs_copy_forbidden_lai_list

DESCRIPTION
  This function copies forbidden LAIs list from one to another sub.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_per_subs_copy_forbidden_lai_list
(
  byte dest,
  byte src
)
{
   memscpy((void *)&forbidden_list_sim[dest],2*sizeof (forbidden_list_T), 
                              (void*)&forbidden_list_sim[src],2*sizeof (forbidden_list_T));
}



#endif
/*===========================================================================

FUNCTION MM_ADD_LAI_IN_REJECT_LIST

DESCRIPTION
  This function adds the passed LAI in reject list.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_add_lai_in_reject_list(inter_task_lai_T* lai)
{
  int i;
  boolean found = FALSE;

  if(lai != NULL)
  {
    /*Check if already existing */
    for(i=0;i<lai_reject_list.length;i++)
    {
      if(memcmp(&lai_reject_list.lai[i],
                lai,
                LAI_SIZE ) == 0 )    /* if match found */
      {
        found = TRUE;
        MSG_MED_DS(MM_SUB, "=MM= LAI already in Reject List, not added. PLMN [%X %X %X]",
                 lai->PLMN_id.identity[0], lai->PLMN_id.identity[1], lai->PLMN_id.identity[2]);
        MSG_MED_DS(MM_SUB, "=MM= LAC [%X %X]", lai->location_area_code[0], lai->location_area_code[1],0);
        break;
      }
    }

    /*If not found then add */
    if(!found)
    {
      /*If list is full, remove oldest entry */
      memscpy(&lai_reject_list.lai[lai_reject_list.write_ptr], LAI_SIZE, lai, LAI_SIZE);
      lai_reject_list.write_ptr=(byte)((lai_reject_list.write_ptr+1)% MAX_NO_FORBIDDEN_LAIS);
      if(lai_reject_list.length < MAX_NO_FORBIDDEN_LAIS)
      {
        lai_reject_list.length++;
      }
      MSG_HIGH_DS_3(MM_SUB, "=MM= LAI added in Reject List. PLMN [%X %X %X]",
                lai->PLMN_id.identity[0], lai->PLMN_id.identity[1], lai->PLMN_id.identity[2]);
      MSG_HIGH_DS_2(MM_SUB, "=MM= LAC [%X %X]", lai->location_area_code[0], lai->location_area_code[1]);
    }
  }

  mm_send_rrc_lai_reject_list_ind(&lai_reject_list);
  mm_send_rr_lai_reject_list_ind(&lai_reject_list);

  return;
}

/*===========================================================================

FUNCTION MM_CLEAR_LAI_IN_REJECT_LIST

DESCRIPTION
  This function clears LAI reject list.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_clear_lai_reject_list( void )
{
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
  lai_reject_list_sim[mm_as_id].length=0;
  lai_reject_list_sim[mm_as_id].write_ptr=0;
#else
  lai_reject_list.length=0;
  lai_reject_list.write_ptr=0;
#endif
  MSG_HIGH_DS_0(MM_SUB, "=MM= LAI Reject list cleared");

  mm_send_rrc_lai_reject_list_ind(&lai_reject_list);
  mm_send_rr_lai_reject_list_ind(&lai_reject_list);
}

/*===========================================================================

FUNCTION MM_PLMN_IN_LAI_REJECT_LIST

DESCRIPTION
  This function clears LAI reject list.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean mm_plmn_in_lai_reject_list
(
  sys_plmn_id_s_type plmn
)
{
  int i;
  for(i=0;i<lai_reject_list.length; i++)
  {
    if(sys_plmn_match(plmn, lai_reject_list.lai[i].PLMN_id))
    {
      MSG_HIGH_DS_3(MM_SUB, "=MM= PLMN %x %x %x found in LAI reject list",plmn.identity[0], plmn.identity[1], plmn.identity[2]);
      return TRUE;
    }
  }

  return FALSE;
}


/*===========================================================================

FUNCTION MM_INIT_FORBIDDEN_LAIS

DESCRIPTION
  This function initializes forbidden LAIs list.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mm_init_forbidden_LAIs(void)
{
   byte     area;   
    
   for (area = 0; area < 2; area++ )
   {
     forbidden_list[area].write_ptr = 0;
   }   
}

/*===========================================================================

FUNCTION  mm_copy_lau_reject_list_to_forbidden_list

DESCRIPTION: This function is used by mm to reject list to 
             Manual forbidden list

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
 
 void mm_copy_lau_reject_list_to_forbidden_list(void)
 {
   byte index = 0;
   byte num_entries = lai_reject_list.length;   
   
   if ( num_entries > MAX_NO_FORBIDDEN_LAIS )
   {
     MSG_HIGH_DS(MM_SUB,"Inavalid size of reject list ",0,0,0);
     return ;
   }
   for(index=0; index < num_entries;index++)
   {
     forbidden_list[NATIONAL_ROAMING].lai[index] = lai_reject_list.lai[index];
     forbidden_list[NATIONAL_ROAMING].write_ptr = (byte)(forbidden_list[NATIONAL_ROAMING].write_ptr+1);
   }
   
   if (num_entries > 0)
   {
     if(mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info, 
                                    SYS_RAT_GSM_RADIO_ACCESS))
      {
        mm_send_rr_forbidden_lai_update();
      }
      if( mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info, 
                                     SYS_RAT_UMTS_RADIO_ACCESS)  || 
              mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info, 
                                     SYS_RAT_TDS_RADIO_ACCESS)) 
     {
       mm_send_rrc_forbidden_lai_update();
     }
   }
 }


