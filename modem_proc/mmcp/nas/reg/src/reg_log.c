/*==============================================================================
                             COPYRIGHT INFORMATION

Copyright (c) 2002 Qualcomm Technologies, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/reg/vcs/reg_log.c_v   1.0   10 May 2002 14:45:42   kabraham  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/reg/src/reg_log.c#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
02/07/02   kwa     Initial version.
04/28/04   kwa     Added reg_log_umts_nas_eplmn_list.
05/05/04   kwa     Fixed an array overrun in reg_log_umts_nas_eplmn_list.
12/20/04   up      Added reg_log_umts_nas_fplmn_list_update,
                   reg_log_umts_nas_fplmn_list,
                   reg_log_umts_nas_hplmn_search_timer_start,
                   reg_log_umts_nas_hplmn_search_timer_expire,
                   reg_log_umts_nas_hplmn_search_start and
                   reg_log_umts_nas_hplmn_search_end
                   to log updation of FPLMN list, FPLMN list, hplmn timer
                   start/expiry and pplmn search start/end.
02/03/06   hs      Fixed errors introduced due to using RVDS
==============================================================================*/

/*==============================================================================

                            INCLUDE FILES FOR MODULE

==============================================================================*/

#include "mmcp_variation.h"
#include "reg_log.h"
#include "naslog_v.h"
#include "msg.h"
#include "event_defs.h"
#if defined (FEATURE_DUAL_SIM) ||defined (FEATURE_SGLTE)
extern boolean mm_dual_standby_mode;
extern boolean mm_sglte_mode;
#endif
/*==============================================================================

                                TYPE DEFINITIONS

==============================================================================*/

/*==============================================================================

                          STATIC FUNCTION DECLARATIONS

==============================================================================*/

/*==============================================================================

                               DATA DECLARATIONS

==============================================================================*/

/*==============================================================================

                          STATIC FUNCTION DEFINITIONS

==============================================================================*/

/*==============================================================================

                              FUNCTION DEFINITIONS

==============================================================================*/

/*==============================================================================

FUNCTION NAME

  reg_log_reg_packet

==============================================================================*/

void reg_log_reg_packet
(
  reg_state_e_type                   reg_state,
  sys_network_selection_mode_e_type  network_selection_mode
)
{
  LOG_UMTS_NAS_REG_LOG_PACKET_C_type* log_p;
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE 
  LOG_UMTS_DS_NAS_REG_LOG_PACKET_C_type *ds_log_ptr;

  if ( mm_dual_standby_mode  || mm_sglte_mode)
  {
    ds_log_ptr = (LOG_UMTS_DS_NAS_REG_LOG_PACKET_C_type *)
                   log_alloc(LOG_UMTS_DS_NAS_REG_LOG_PACKET_C, 
                             sizeof(LOG_UMTS_DS_NAS_REG_LOG_PACKET_C_type));

    if (ds_log_ptr)
    {
      ds_log_ptr->as_id = (uint8) reg_as_id;
      ds_log_ptr->reg_state           = (uint8)reg_state;
      ds_log_ptr->plmn_selection_mode = (uint8)network_selection_mode;
      ds_log_ptr->ue_operation_mode   = 0;

      log_commit(ds_log_ptr);
    }
  }
  else
#endif
  {
    log_p = (LOG_UMTS_NAS_REG_LOG_PACKET_C_type*)log_alloc(LOG_UMTS_NAS_REG_LOG_PACKET_C,
                    sizeof(LOG_UMTS_NAS_REG_LOG_PACKET_C_type));
    if (log_p)
    {
      log_p->reg_state           = (uint8)reg_state;
      log_p->plmn_selection_mode = (uint8)network_selection_mode;
      log_p->ue_operation_mode   = 0;

      log_commit(log_p);
    }
  }
}


/*==============================================================================

FUNCTION NAME

  reg_log_umts_nas_eplmn_list

==============================================================================*/

void reg_log_umts_nas_eplmn_list
(
        sys_plmn_id_s_type    rplmn,
  const sys_plmn_list_s_type* eplmn_list_p
)
{
  uint8 i;

  LOG_UMTS_NAS_EPLMN_LIST_LOG_PACKET_C_type* log_p;
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE 
  LOG_UMTS_DS_NAS_EPLMN_LIST_LOG_PACKET_C_type *ds_log_ptr;

  if ( mm_dual_standby_mode || mm_sglte_mode)
  {
    ds_log_ptr = (LOG_UMTS_DS_NAS_EPLMN_LIST_LOG_PACKET_C_type *)
                   log_alloc(LOG_UMTS_DS_NAS_EPLMN_LIST_LOG_PACKET_C, 
                             sizeof(LOG_UMTS_DS_NAS_EPLMN_LIST_LOG_PACKET_C_type));

    if (ds_log_ptr)
    {

      ds_log_ptr->as_id = (uint8) reg_as_id;
      /*
      ** Copy the RPLMN into the log packet.
      */
      ds_log_ptr->rplmn.plmn_id[0] = (uint8)rplmn.identity[0];
      ds_log_ptr->rplmn.plmn_id[1] = (uint8)rplmn.identity[1];
      ds_log_ptr->rplmn.plmn_id[2] = (uint8)rplmn.identity[2];

      /*
      ** Copy the equivalent PLMN list into the static variable
      ** to be written to NVRAM.
      */
      ds_log_ptr->num_equivalent_plmns = (uint8)MAX_NUMBER_EQUIVALENT_PLMNS;

      if ((uint8)SYS_PLMN_LIST_MAX_LENGTH < ds_log_ptr->num_equivalent_plmns)
      {
        ds_log_ptr->num_equivalent_plmns = (uint8)SYS_PLMN_LIST_MAX_LENGTH;
      }

      if (eplmn_list_p->length < ds_log_ptr->num_equivalent_plmns)
      {
        ds_log_ptr->num_equivalent_plmns = (uint8)eplmn_list_p->length;
      }

      for (i = 0; i < ds_log_ptr->num_equivalent_plmns; i++)
      {
        ds_log_ptr->equivalent_plmn[i].plmn_id[0] = eplmn_list_p->plmn[i].identity[0];
        ds_log_ptr->equivalent_plmn[i].plmn_id[1] = eplmn_list_p->plmn[i].identity[1];
        ds_log_ptr->equivalent_plmn[i].plmn_id[2] = eplmn_list_p->plmn[i].identity[2];
      }

      /*
      ** Undefine the unused PLMN ids contained within the equivalent
      ** PLMN list to make it easier to verify that the log item has
      ** been properly updated.
      */
      for (i = ds_log_ptr->num_equivalent_plmns; i < MAX_NUMBER_EQUIVALENT_PLMNS; i++)
      {
        ds_log_ptr->equivalent_plmn[i].plmn_id[0] = 0xFF;
        ds_log_ptr->equivalent_plmn[i].plmn_id[1] = 0xFF;
        ds_log_ptr->equivalent_plmn[i].plmn_id[2] = 0xFF;
      }

      log_commit(ds_log_ptr);
    }
  }
  else
#endif
  {
    log_p = (LOG_UMTS_NAS_EPLMN_LIST_LOG_PACKET_C_type*)log_alloc(LOG_UMTS_NAS_EPLMN_LIST_LOG_PACKET_C,
                    sizeof(LOG_UMTS_NAS_EPLMN_LIST_LOG_PACKET_C_type));

    if (log_p)
    {
      /*
      ** Copy the RPLMN into the log packet.
      */
      log_p->rplmn.plmn_id[0] = (uint8)rplmn.identity[0];
      log_p->rplmn.plmn_id[1] = (uint8)rplmn.identity[1];
      log_p->rplmn.plmn_id[2] = (uint8)rplmn.identity[2];

      /*
      ** Copy the equivalent PLMN list into the static variable
      ** to be written to NVRAM.
      */
      log_p->num_equivalent_plmns = (uint8)MAX_NUMBER_EQUIVALENT_PLMNS;

      if ((uint8)SYS_PLMN_LIST_MAX_LENGTH < log_p->num_equivalent_plmns)
      {
        log_p->num_equivalent_plmns = (uint8)SYS_PLMN_LIST_MAX_LENGTH;
      }

      if (eplmn_list_p->length < log_p->num_equivalent_plmns)
      {
        log_p->num_equivalent_plmns = (uint8)eplmn_list_p->length;
      }

      for (i = 0; i < log_p->num_equivalent_plmns; i++)
      {
        log_p->equivalent_plmn[i].plmn_id[0] = eplmn_list_p->plmn[i].identity[0];
        log_p->equivalent_plmn[i].plmn_id[1] = eplmn_list_p->plmn[i].identity[1];
        log_p->equivalent_plmn[i].plmn_id[2] = eplmn_list_p->plmn[i].identity[2];
      }

      /*
      ** Undefine the unused PLMN ids contained within the equivalent
      ** PLMN list to make it easier to verify that the log item has
      ** been properly updated.
      */
      for (i = log_p->num_equivalent_plmns; i < MAX_NUMBER_EQUIVALENT_PLMNS; i++)
      {
        log_p->equivalent_plmn[i].plmn_id[0] = 0xFF;
        log_p->equivalent_plmn[i].plmn_id[1] = 0xFF;
        log_p->equivalent_plmn[i].plmn_id[2] = 0xFF;
      }

      log_commit(log_p);

    }
  }
}

/*==============================================================================

FUNCTION NAME

  reg_log_umts_nas_fplmn_list_update

==============================================================================*/

void reg_log_umts_nas_fplmn_list_update
(
  sys_plmn_id_s_type  plmn,
  boolean             is_added
#ifdef FEATURE_DUAL_SIM
  ,sys_modem_as_id_e_type as_id
#endif
)
{
  LOG_UMTS_NAS_FPLMN_LIST_UPDATE_LOG_PACKET_C_type* log_p;
#ifdef FEATURE_DUAL_SIM
  LOG_UMTS_DS_NAS_FPLMN_LIST_UPDATE_LOG_PACKET_C_type *ds_log_ptr;
#endif

  boolean plmn_id_is_undefined;
  boolean mnc_includes_pcs_digit;
  uint32  mcc;
  uint32  mnc;

  sys_plmn_get_mcc_mnc
  (
    plmn,
    &plmn_id_is_undefined,
    &mnc_includes_pcs_digit,
    &mcc,
    &mnc
  );

  if (is_added)
  {
    MSG_HIGH_DS_2(REG_SUB, "=REG= PLMN(%d-%d) is added to FPLMN List", mcc, mnc);
  }
  else
  {
    MSG_HIGH_DS_2(REG_SUB, "=REG= PLMN(%d-%d) is removed from FPLMN List", mcc, mnc);
  }
#ifdef FEATURE_DUAL_SIM
  if ( mm_dual_standby_mode )
  {
    ds_log_ptr = (LOG_UMTS_DS_NAS_FPLMN_LIST_UPDATE_LOG_PACKET_C_type *)
                   log_alloc(LOG_UMTS_DS_NAS_FPLMN_LIST_UPDATE_LOG_PACKET_C, 
                             sizeof(LOG_UMTS_DS_NAS_FPLMN_LIST_UPDATE_LOG_PACKET_C_type));

    if (ds_log_ptr)
    {
      ds_log_ptr->as_id = (uint8) as_id;
      /*
      ** Copy the FPLMN into the log packet.
      */
      ds_log_ptr->fplmn.plmn_id[0] = (uint8)plmn.identity[0];
      ds_log_ptr->fplmn.plmn_id[1] = (uint8)plmn.identity[1];
      ds_log_ptr->fplmn.plmn_id[2] = (uint8)plmn.identity[2];
      ds_log_ptr->is_added = is_added;

      log_commit(ds_log_ptr);
    }
  }
  else
#endif
  {
    log_p = (LOG_UMTS_NAS_FPLMN_LIST_UPDATE_LOG_PACKET_C_type*)log_alloc(LOG_UMTS_NAS_FPLMN_LIST_UPDATE_LOG_PACKET_C,
                      sizeof(LOG_UMTS_NAS_FPLMN_LIST_UPDATE_LOG_PACKET_C_type));

    if (log_p)
    {
      /*
      ** Copy the FPLMN into the log packet.
      */
      log_p->fplmn.plmn_id[0] = (uint8)plmn.identity[0];
      log_p->fplmn.plmn_id[1] = (uint8)plmn.identity[1];
      log_p->fplmn.plmn_id[2] = (uint8)plmn.identity[2];
      log_p->is_added = is_added;

      log_commit(log_p);
    }
  }
}

/*==============================================================================

FUNCTION NAME

  reg_log_umts_nas_fplmn_list

==============================================================================*/

void reg_log_umts_nas_fplmn_list
(
  const reg_sim_plmn_list_s_type* fplmn_list_p
#ifdef FEATURE_DUAL_SIM
  ,sys_modem_as_id_e_type as_id
#endif
)
{
  uint8 i;

  LOG_UMTS_NAS_FPLMN_LIST_LOG_PACKET_C_type* log_p;
#ifdef FEATURE_DUAL_SIM
  LOG_UMTS_DS_NAS_FPLMN_LIST_LOG_PACKET_C_type *ds_log_ptr;

  if ( mm_dual_standby_mode )
  {
    ds_log_ptr = (LOG_UMTS_DS_NAS_FPLMN_LIST_LOG_PACKET_C_type *)
                   log_alloc(LOG_UMTS_DS_NAS_FPLMN_LIST_LOG_PACKET_C, 
                             sizeof(LOG_UMTS_DS_NAS_FPLMN_LIST_LOG_PACKET_C_type));

    if (ds_log_ptr)
    {

      ds_log_ptr->as_id = (uint8) as_id;
      /*
      ** Copy the forbidden PLMN list into the log packet
      */
      ds_log_ptr->num_fplmns = (uint8)MAX_NUMBER_FORBIDDEN_PLMNS;

      if ((uint8)REG_SIM_PLMN_LIST_LENGTH < ds_log_ptr->num_fplmns)
      {
        ds_log_ptr->num_fplmns = (uint8)REG_SIM_PLMN_LIST_LENGTH;
      }

      if (fplmn_list_p->length < ds_log_ptr->num_fplmns)
      {
        ds_log_ptr->num_fplmns = (uint8)fplmn_list_p->length;
      }

      for (i = 0; i < ds_log_ptr->num_fplmns; i++)
      {
        ds_log_ptr->fplmns[i].plmn_id[0] = fplmn_list_p->plmn[i].identity[0];
        ds_log_ptr->fplmns[i].plmn_id[1] = fplmn_list_p->plmn[i].identity[1];
        ds_log_ptr->fplmns[i].plmn_id[2] = fplmn_list_p->plmn[i].identity[2];
      }

      /*
      ** Undefine the unused PLMN ids contained within the forbidden
      ** PLMN list to make it easier to verify that the log item has
      ** been properly updated.
      */
      for (i = ds_log_ptr->num_fplmns; i < MAX_NUMBER_FORBIDDEN_PLMNS; i++)
      {
        ds_log_ptr->fplmns[i].plmn_id[0] = 0xFF;
        ds_log_ptr->fplmns[i].plmn_id[1] = 0xFF;
        ds_log_ptr->fplmns[i].plmn_id[2] = 0xFF;
      }

      log_commit(ds_log_ptr);
    }

  }
  else
#endif
  {
    log_p = (LOG_UMTS_NAS_FPLMN_LIST_LOG_PACKET_C_type*)log_alloc(LOG_UMTS_NAS_FPLMN_LIST_LOG_PACKET_C,
                      sizeof(LOG_UMTS_NAS_FPLMN_LIST_LOG_PACKET_C_type));

    if (log_p)
    {
      /*
      ** Copy the forbidden PLMN list into the log packet
      */
      log_p->num_fplmns = (uint8)MAX_NUMBER_FORBIDDEN_PLMNS;

      if ((uint8)REG_SIM_PLMN_LIST_LENGTH < log_p->num_fplmns)
      {
        log_p->num_fplmns = (uint8)REG_SIM_PLMN_LIST_LENGTH;
      }

      if (fplmn_list_p->length < log_p->num_fplmns)
      {
        log_p->num_fplmns = (uint8)fplmn_list_p->length;
      }

      for (i = 0; i < log_p->num_fplmns; i++)
      {
        log_p->fplmns[i].plmn_id[0] = fplmn_list_p->plmn[i].identity[0];
        log_p->fplmns[i].plmn_id[1] = fplmn_list_p->plmn[i].identity[1];
        log_p->fplmns[i].plmn_id[2] = fplmn_list_p->plmn[i].identity[2];
      }

      /*
      ** Undefine the unused PLMN ids contained within the forbidden
      ** PLMN list to make it easier to verify that the log item has
      ** been properly updated.
      */
      for (i = log_p->num_fplmns; i < MAX_NUMBER_FORBIDDEN_PLMNS; i++)
      {
        log_p->fplmns[i].plmn_id[0] = 0xFF;
        log_p->fplmns[i].plmn_id[1] = 0xFF;
        log_p->fplmns[i].plmn_id[2] = 0xFF;
      }

      log_commit(log_p);
    }
  }
}

/*==============================================================================

FUNCTION NAME

  reg_log_umts_nas_hplmn_search_timer_start

==============================================================================*/

void reg_log_umts_nas_hplmn_search_timer_start
(
  dword   hplmn_timer
)
{
  LOG_UMTS_NAS_HPLMN_SEARCH_TIMER_START_LOG_PACKET_C_type* log_p;
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE 
  LOG_UMTS_DS_NAS_HPLMN_SEARCH_TIMER_START_LOG_PACKET_C_type *ds_log_ptr;

  if ( mm_dual_standby_mode || mm_sglte_mode)
  {
    ds_log_ptr = (LOG_UMTS_DS_NAS_HPLMN_SEARCH_TIMER_START_LOG_PACKET_C_type *)
                   log_alloc(LOG_UMTS_DS_NAS_HPLMN_SEARCH_TIMER_START_LOG_PACKET_C, 
                             sizeof(LOG_UMTS_DS_NAS_HPLMN_SEARCH_TIMER_START_LOG_PACKET_C_type));

    if (ds_log_ptr)
    {
      ds_log_ptr->as_id = (uint8) reg_as_id;
      ds_log_ptr->hplmn_search_timer = (uint16) (hplmn_timer&0xFFFF);

      log_commit(ds_log_ptr);
    }
  }
  else
#endif
  {
    log_p = (LOG_UMTS_NAS_HPLMN_SEARCH_TIMER_START_LOG_PACKET_C_type*)log_alloc(LOG_UMTS_NAS_HPLMN_SEARCH_TIMER_START_LOG_PACKET_C,
                    sizeof(LOG_UMTS_NAS_HPLMN_SEARCH_TIMER_START_LOG_PACKET_C_type));

    if (log_p)
    {
      log_p->hplmn_search_timer = (uint16) (hplmn_timer&0xFFFF);

      log_commit(log_p);
    }
  }
}

/*==============================================================================

FUNCTION NAME

  reg_log_umts_nas_hplmn_search_timer_expire

==============================================================================*/

#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION 
#error code not present
#endif
void reg_log_umts_nas_hplmn_search_timer_expire
(
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE 
  sys_modem_as_id_e_type as_id
#else
  void
#endif
)
{
  LOG_UMTS_NAS_HPLMN_SEARCH_TIMER_EXPIRE_LOG_PACKET_C_type* log_p;
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE 
  LOG_UMTS_DS_NAS_HPLMN_SEARCH_TIMER_EXPIRE_LOG_PACKET_C_type *ds_log_ptr;

  if ( mm_dual_standby_mode || mm_sglte_mode)
  {
    ds_log_ptr = (LOG_UMTS_DS_NAS_HPLMN_SEARCH_TIMER_EXPIRE_LOG_PACKET_C_type *)
                   log_alloc(LOG_UMTS_DS_NAS_HPLMN_SEARCH_TIMER_EXPIRE_LOG_PACKET_C, 
                             sizeof(LOG_UMTS_DS_NAS_HPLMN_SEARCH_TIMER_EXPIRE_LOG_PACKET_C_type));

    if (ds_log_ptr)
    {
      ds_log_ptr->as_id = (uint8) as_id;
      ds_log_ptr->dummy = 0;

      log_commit(ds_log_ptr);
    }
  }
  else
#endif
  {
    log_p = (LOG_UMTS_NAS_HPLMN_SEARCH_TIMER_EXPIRE_LOG_PACKET_C_type*)log_alloc(LOG_UMTS_NAS_HPLMN_SEARCH_TIMER_EXPIRE_LOG_PACKET_C,
                    sizeof(LOG_UMTS_NAS_HPLMN_SEARCH_TIMER_EXPIRE_LOG_PACKET_C_type));

    if (log_p)
    {
      log_p->dummy = 0;

      log_commit(log_p);
    }
  }

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE 
  if ( mm_dual_standby_mode  || mm_sglte_mode)
  {
    uint8 event_packet = (uint8) as_id;
    event_report_payload(EVENT_DS_HPLMN_TIMER_EXPIRED, 1, (void *)&event_packet);
  }
  else
#endif
  {
    event_report(EVENT_HPLMN_TIMER_EXPIRED);
  }
}
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION 
#error code not present
#endif 

#if defined(FEATURE_GTOW_BACKGROUND_PLMN_SEARCH) || defined(FEATURE_RAT_PRIORITY_LIST)
/*==============================================================================

FUNCTION NAME

  reg_log_umts_nas_hplmn_search_start

==============================================================================*/

void reg_log_umts_nas_hplmn_search_start
(
  const sys_plmn_rat_list_s_type* pplmn_list_p
)
{
  uint8 i;

  LOG_UMTS_NAS_HPLMN_SEARCH_START_LOG_PACKET_C_type* log_p;
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE 
  LOG_UMTS_DS_NAS_HPLMN_SEARCH_START_LOG_PACKET_C_type *ds_log_ptr;

  if ( mm_dual_standby_mode || mm_sglte_mode)
  {
    ds_log_ptr = (LOG_UMTS_DS_NAS_HPLMN_SEARCH_START_LOG_PACKET_C_type *)
                   log_alloc(LOG_UMTS_DS_NAS_HPLMN_SEARCH_START_LOG_PACKET_C, 
                             sizeof(LOG_UMTS_DS_NAS_HPLMN_SEARCH_START_LOG_PACKET_C_type));

    if (ds_log_ptr)
    {
      ds_log_ptr->as_id = (uint8) reg_as_id;
      /*
      ** Copy the higher priority PLMN list into the log packet
      */
      ds_log_ptr->num_pplmns = (uint8)MAX_NUMBER_PRIORITY_PLMNS;

      if ((uint8)SYS_PLMN_LIST_MAX_LENGTH < ds_log_ptr->num_pplmns)
      {
        ds_log_ptr->num_pplmns = (uint8)SYS_PLMN_LIST_MAX_LENGTH;
      }

      if (pplmn_list_p->length < ds_log_ptr->num_pplmns)
      {
        ds_log_ptr->num_pplmns = (uint8)pplmn_list_p->length;
      }

      for (i = 0; i < ds_log_ptr->num_pplmns; i++)
      {
        ds_log_ptr->pplmns[i].plmn.plmn_id[0] = pplmn_list_p->info[i].plmn.identity[0];
        ds_log_ptr->pplmns[i].plmn.plmn_id[1] = pplmn_list_p->info[i].plmn.identity[1];
        ds_log_ptr->pplmns[i].plmn.plmn_id[2] = pplmn_list_p->info[i].plmn.identity[2];
        ds_log_ptr->pplmns[i].rat = (uint8)pplmn_list_p->info[i].rat;
      }

      /*
      ** Undefine the unused PLMN ids contained within the priority
      ** PLMN list to make it easier to verify that the log item has
      ** been properly updated.
      */
      for (i = ds_log_ptr->num_pplmns; i < MAX_NUMBER_PRIORITY_PLMNS; i++)
      {
        ds_log_ptr->pplmns[i].plmn.plmn_id[0] = 0xFF;
        ds_log_ptr->pplmns[i].plmn.plmn_id[1] = 0xFF;
        ds_log_ptr->pplmns[i].plmn.plmn_id[2] = 0xFF;
        ds_log_ptr->pplmns[i].rat = (uint8)SYS_RAT_NONE;
      }

      log_commit(ds_log_ptr);
    }
  }
  else
#endif
  {
    log_p = (LOG_UMTS_NAS_HPLMN_SEARCH_START_LOG_PACKET_C_type*)log_alloc(LOG_UMTS_NAS_HPLMN_SEARCH_START_LOG_PACKET_C,
                    sizeof(LOG_UMTS_NAS_HPLMN_SEARCH_START_LOG_PACKET_C_type));

    if (log_p)
    {
      /*
      ** Copy the higher priority PLMN list into the log packet
      */
      log_p->num_pplmns = (uint8)MAX_NUMBER_PRIORITY_PLMNS;

      if ((uint8)SYS_PLMN_LIST_MAX_LENGTH < log_p->num_pplmns)
      {
        log_p->num_pplmns = (uint8)SYS_PLMN_LIST_MAX_LENGTH;
      }

      if (pplmn_list_p->length < log_p->num_pplmns)
      {
        log_p->num_pplmns = (uint8)pplmn_list_p->length;
      }

      for (i = 0; i < log_p->num_pplmns; i++)
      {
        log_p->pplmns[i].plmn.plmn_id[0] = pplmn_list_p->info[i].plmn.identity[0];
        log_p->pplmns[i].plmn.plmn_id[1] = pplmn_list_p->info[i].plmn.identity[1];
        log_p->pplmns[i].plmn.plmn_id[2] = pplmn_list_p->info[i].plmn.identity[2];
        log_p->pplmns[i].rat = (uint8)pplmn_list_p->info[i].rat;
      }

      /*
      ** Undefine the unused PLMN ids contained within the priority
      ** PLMN list to make it easier to verify that the log item has
      ** been properly updated.
      */
      for (i = log_p->num_pplmns; i < MAX_NUMBER_PRIORITY_PLMNS; i++)
      {
        log_p->pplmns[i].plmn.plmn_id[0] = 0xFF;
        log_p->pplmns[i].plmn.plmn_id[1] = 0xFF;
        log_p->pplmns[i].plmn.plmn_id[2] = 0xFF;
        log_p->pplmns[i].rat = (uint8)SYS_RAT_NONE;
      }

      log_commit(log_p);
    }
  }
}

/*==============================================================================

FUNCTION NAME

  reg_log_umts_nas_hplmn_search_end

==============================================================================*/

void reg_log_umts_nas_hplmn_search_end
(
  const sys_detailed_plmn_list_s_type* avail_plmn_list_p
)
{
  uint8 i;

  LOG_UMTS_NAS_HPLMN_SEARCH_END_LOG_PACKET_C_type* log_p;
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE 
  LOG_UMTS_DS_NAS_HPLMN_SEARCH_END_LOG_PACKET_C_type *ds_log_ptr;

  if ( mm_dual_standby_mode  || mm_sglte_mode)
  {
    ds_log_ptr = (LOG_UMTS_DS_NAS_HPLMN_SEARCH_END_LOG_PACKET_C_type *)
                   log_alloc(LOG_UMTS_DS_NAS_HPLMN_SEARCH_END_LOG_PACKET_C, 
                             sizeof(LOG_UMTS_DS_NAS_HPLMN_SEARCH_END_LOG_PACKET_C_type));

    if (ds_log_ptr)
    {
      ds_log_ptr->as_id = (uint8) reg_as_id;
      /*
      ** Copy the available PLMN list into the log packet
      */
      ds_log_ptr->num_available_plmns = (uint8)MAX_NUMBER_AVAILABLE_PLMNS;

      if ((uint8)SYS_PLMN_LIST_MAX_LENGTH < ds_log_ptr->num_available_plmns)
      {
        ds_log_ptr->num_available_plmns = (uint8)SYS_PLMN_LIST_MAX_LENGTH;
      }

      if (avail_plmn_list_p->length < ds_log_ptr->num_available_plmns)
      {
        ds_log_ptr->num_available_plmns = (uint8)avail_plmn_list_p->length;
      }

      for (i = 0; i < ds_log_ptr->num_available_plmns; i++)
      {
        /* Copy PLMN ID */
        ds_log_ptr->info[i].plmn.plmn_id[0]   = avail_plmn_list_p->info[i].plmn.identity[0];
        ds_log_ptr->info[i].plmn.plmn_id[1]   = avail_plmn_list_p->info[i].plmn.identity[1];
        ds_log_ptr->info[i].plmn.plmn_id[2]   = avail_plmn_list_p->info[i].plmn.identity[2];


        /* Copy type of PLMN - User/Operator preferred  b3-b1 */
        ds_log_ptr->info[i].plmn_details      = (uint8)((byte)avail_plmn_list_p->info[i].list_category & 0x07);

        /* Copy RAT of the PLMN b5-b4 */
        ds_log_ptr->info[i].plmn_details     |= (uint8)(((byte)avail_plmn_list_p->info[i].rat << 3) & 0x18);

        /* Copy Service capability of the PLMN b7-b6 */
        ds_log_ptr->info[i].plmn_details     |= (uint8)(((byte)avail_plmn_list_p->info[i].plmn_service_capability << 5) & 0x60);

        /* Copy Signal quality b8 */
        ds_log_ptr->info[i].plmn_details     |= (uint8)(((byte)avail_plmn_list_p->info[i].signal_quality << 7) & 0x80);

        /* Signal Strength */
        ds_log_ptr->info[i].signal_strength   = (uint8)avail_plmn_list_p->info[i].signal_strength;

      }

      /*
      ** Undefine the unused PLMN information contained within the available
      ** PLMN list to make it easier to verify that the log item has
      ** been properly updated.
      */
      for (i = ds_log_ptr->num_available_plmns; i < MAX_NUMBER_AVAILABLE_PLMNS; i++)
      {
        (void)memset((void *)&(ds_log_ptr->info[i]), 0xFF, sizeof(log_detailed_plmn_info_type));
      }
  
      log_commit(ds_log_ptr);
    }
  }
  else
#endif
  {
    log_p = (LOG_UMTS_NAS_HPLMN_SEARCH_END_LOG_PACKET_C_type*)log_alloc(LOG_UMTS_NAS_HPLMN_SEARCH_END_LOG_PACKET_C,
                    sizeof(LOG_UMTS_NAS_HPLMN_SEARCH_END_LOG_PACKET_C_type));

    if (log_p)
    {
      /*
      ** Copy the available PLMN list into the log packet
      */
      log_p->num_available_plmns = (uint8)MAX_NUMBER_AVAILABLE_PLMNS;

      if ((uint8)SYS_PLMN_LIST_MAX_LENGTH < log_p->num_available_plmns)
      {
        log_p->num_available_plmns = (uint8)SYS_PLMN_LIST_MAX_LENGTH;
      }

      if (avail_plmn_list_p->length < log_p->num_available_plmns)
      {
        log_p->num_available_plmns = (uint8)avail_plmn_list_p->length;
      }

      for (i = 0; i < log_p->num_available_plmns; i++)
      {
        /* Copy PLMN ID */
        log_p->info[i].plmn.plmn_id[0]   = avail_plmn_list_p->info[i].plmn.identity[0];
        log_p->info[i].plmn.plmn_id[1]   = avail_plmn_list_p->info[i].plmn.identity[1];
        log_p->info[i].plmn.plmn_id[2]   = avail_plmn_list_p->info[i].plmn.identity[2];


        /* Copy type of PLMN - User/Operator preferred  b3-b1 */
        log_p->info[i].plmn_details      = (uint8)((byte)avail_plmn_list_p->info[i].list_category & 0x07);

        /* Copy RAT of the PLMN b5-b4 */
        log_p->info[i].plmn_details     |= (uint8)(((byte)avail_plmn_list_p->info[i].rat << 3) & 0x18);

        /* Copy Service capability of the PLMN b7-b6 */
        log_p->info[i].plmn_details     |= (uint8)(((byte)avail_plmn_list_p->info[i].plmn_service_capability << 5) & 0x60);

        /* Copy Signal quality b8 */
        log_p->info[i].plmn_details     |= (uint8)(((byte)avail_plmn_list_p->info[i].signal_quality << 7) & 0x80);

        /* Signal Strength */
        log_p->info[i].signal_strength   = (uint8)avail_plmn_list_p->info[i].signal_strength;

      }

      /*
      ** Undefine the unused PLMN information contained within the available
      ** PLMN list to make it easier to verify that the log item has
      ** been properly updated.
      */
      for (i = log_p->num_available_plmns; i < MAX_NUMBER_AVAILABLE_PLMNS; i++)
      {
        (void)memset((void *)&(log_p->info[i]), 0xFF, sizeof(log_detailed_plmn_info_type));
      }
  
      log_commit(log_p);
    }
  }
}

#endif /*defined(FEATURE_GTOW_BACKGROUND_PLMN_SEARCH) || defined(FEATURE_RAT_PRIORITY_LIST)*/


/*==============================================================================
Function Name: REG_LOG_UMTS_NAS_PPLMN_LIST

Description: This function sends the log packet for Preferred PLMN list

Return Value: None
==============================================================================*/

void reg_log_umts_nas_pplmn_list
(
  const reg_sim_plmn_w_act_s_type* pplmn_list_p,
  uint16                           num_opplmn,
  uint16                           num_upplmn,
  uint16                           num_pplmn,
  sys_modem_as_id_e_type           as_id
)
{
  uint16 i = 0;
  uint16 total_num_pplmn = 0;
  LOG_UMTS_NAS_PPLMN_LIST_LOG_PACKET_C_type* log_p = NULL;

  if((num_opplmn + num_upplmn) > 0)
  {
    total_num_pplmn = num_opplmn + num_upplmn;
  }
  else
  {
    total_num_pplmn = num_pplmn;
  }  

  if(pplmn_list_p != NULL && total_num_pplmn > 0)
  {
    log_p = (LOG_UMTS_NAS_PPLMN_LIST_LOG_PACKET_C_type*)log_alloc(LOG_UMTS_NAS_PPLMN_LIST_LOG_PACKET_C,
                      sizeof(LOG_UMTS_NAS_PPLMN_LIST_LOG_PACKET_C_type));

    if(log_p)
    {
      log_p->version = (uint8)1;

      log_p->as_id = (uint8)as_id + 1;

      log_p->num_pplmns = total_num_pplmn;
      if(log_p->num_pplmns > (uint16)REG_SIM_MAX_PREFERRED_PLMN_LIST_LENGTH)
      {
        log_p->num_pplmns = (uint16)REG_SIM_MAX_PREFERRED_PLMN_LIST_LENGTH;
      }

      for (i = 0; i < log_p->num_pplmns; i++)
      {
        /* Populate PLMN ID */
        log_p->pplmn_list[i].plmn.plmn_id[0] = pplmn_list_p[i].plmn.identity[0];
        log_p->pplmn_list[i].plmn.plmn_id[1] = pplmn_list_p[i].plmn.identity[1];
        log_p->pplmn_list[i].plmn.plmn_id[2] = pplmn_list_p[i].plmn.identity[2];

        /* Populate PLMN category */
        if((num_opplmn + num_upplmn) == 0)
        {
          log_p->pplmn_list[i].category = (uint8)SYS_DETAILED_PLMN_LIST_CATEGORY_PREFERRED;
        }
        else if(i < num_upplmn)
        {
          log_p->pplmn_list[i].category = (uint8)SYS_DETAILED_PLMN_LIST_CATEGORY_USER_PREFERRED;
        }
        else
        {
          log_p->pplmn_list[i].category = (uint8)SYS_DETAILED_PLMN_LIST_CATEGORY_OPERATOR_PREFERRED;
        }

        /* Populate Access Technologies */
        log_p->pplmn_list[i].num_rats = 0;
#ifdef FEATURE_LTE
        /* LTE */
        if((pplmn_list_p[i].act[0] & 0x40) == 0x40)
        {
          log_p->pplmn_list[i].rat[log_p->pplmn_list[i].num_rats] = (uint8)SYS_RAT_LTE_RADIO_ACCESS;
	      log_p->pplmn_list[i].num_rats = log_p->pplmn_list[i].num_rats + 1;
        }
#endif
        /* UMTS */
        if((pplmn_list_p[i].act[0] & 0x80) == 0x80)
        {
          log_p->pplmn_list[i].rat[log_p->pplmn_list[i].num_rats] = (uint8)SYS_RAT_UMTS_RADIO_ACCESS;
	      log_p->pplmn_list[i].num_rats = log_p->pplmn_list[i].num_rats + 1;
        }
        /* GSM */
        if((pplmn_list_p[i].act[1] & 0x80) == 0x80)
        {
          log_p->pplmn_list[i].rat[log_p->pplmn_list[i].num_rats] = (uint8)SYS_RAT_GSM_RADIO_ACCESS;
	      log_p->pplmn_list[i].num_rats = log_p->pplmn_list[i].num_rats + 1;
        }

        /* ALL */
        if((log_p->pplmn_list[i].num_rats == 0) &&
           (pplmn_list_p[i].act[0] == 0x00) &&
           (pplmn_list_p[i].act[1] == 0x00))
        {
#ifdef FEATURE_LTE
           log_p->pplmn_list[i].num_rats = 3;
           log_p->pplmn_list[i].rat[0] = (uint8)SYS_RAT_LTE_RADIO_ACCESS;
           log_p->pplmn_list[i].rat[1] = (uint8)SYS_RAT_UMTS_RADIO_ACCESS;
           log_p->pplmn_list[i].rat[2] = (uint8)SYS_RAT_GSM_RADIO_ACCESS;
#else
           log_p->pplmn_list[i].num_rats = 2;
           log_p->pplmn_list[i].rat[0] = (uint8)SYS_RAT_UMTS_RADIO_ACCESS;
           log_p->pplmn_list[i].rat[1] = (uint8)SYS_RAT_GSM_RADIO_ACCESS;
#endif
        }
	
      } /* End for loop */

      log_commit(log_p);
    }
  }
}