/*===========================================================================

          D A T A   S E R V I C E S   S T A C K   P R O C E S S O R

GENERAL DESCRIPTION
  Data service's SS event stack processor.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2010 - 2015 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/dsmgr/src/ds3gstkproc.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/24/13   sd      TSTS changes for DS3G.
04/25/13   var     Initial revision

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include "data_msg.h"
#include <stringl/stringl.h>

#include "ds707_pkt_mgr.h"
#include "ds3gstkproc.h"
#include "ds3gutil.h"
#include "ds3gdsdif.h"
#include "ds3gcsdhdlr.h"
#include "ds3gcmif.h"
#include "ds_dsd_ext_i.h"
#include "ds_wcdma_bearer_context.h"
#include "mc.h"
#include "cm_v.h"
#include "modem_mem.h"
#ifdef FEATURE_DATA_LTE_OFFTARGET_TEST
#include "dsut.h"
#endif /* FEATURE_DATA_LTE_OFFTARGET_TEST */

/*===========================================================================

                    PUBLIC DATA DECLARATIONS

===========================================================================*/


/*===========================================================================

                    GLOBAL VARIABLES

===========================================================================*/
/*---------------------------------------------------------------------------
   CM SS event info cache
---------------------------------------------------------------------------*/
static ds3g_ss_event_info_type
  *ds3g_ss_event_info[DS3GSUBSMGR_SUBS_ID_COUNT] = {0};

/*---------------------------------------------------------------------------
    Radio to Stack table info
---------------------------------------------------------------------------*/
static ds3g_radio_to_stack_table_type 
  *ds3g_radio_to_stack_table[DS3GSUBSMGR_SUBS_ID_COUNT] = {0};

/*---------------------------------------------------------------------------
  DSD notified by SS event
---------------------------------------------------------------------------*/
static boolean ds3g_dsd_notified_by_ss_event = FALSE;

/*---------------------------------------------------------------------------
   BSR state variable
---------------------------------------------------------------------------*/
static ds3g_stkproc_bsr_state_type ds3g_bsr_state[DS3GSUBSMGR_SUBS_ID_COUNT] 
                                                = {DS3G_STKPROC_BSR_RESET};

/*=========================================================================== 
 
                    INTERNAL FUNCTIONS

===========================================================================*/

/*===========================================================================
FUNCTION      DS3STKPROC_CREATE_SS_EVENT_INFO_INSTANCE

DESCRIPTION   This function creates SS event info instance for subs_id.

PARAMETERS    subs_id: subscription id of the instance to be created.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
ds3g_ss_event_info_type* ds3gstkproc_create_ss_event_info_instance
(
  ds3gsubsmgr_subs_id_e_type  subs_id
)
{
  ds3g_ss_event_info_type  *ss_event_info_instance = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ss_event_info_instance = (ds3g_ss_event_info_type*)
                            modem_mem_alloc(sizeof(ds3g_ss_event_info_type),
                                            MODEM_MEM_CLIENT_DATA);

  if(ss_event_info_instance != NULL)
  {
    /*-------------------------------------------------------------------------
      Initialize system mode for all radios
    -------------------------------------------------------------------------*/
    memset(ss_event_info_instance, 0 , sizeof(ds3g_ss_event_info_type));
    ss_event_info_instance->asubs_id = ds3gsubsmgr_subs_id_ds3g_to_cm(subs_id);

    ds3g_ss_event_info[subs_id] = ss_event_info_instance;
  }
  return ss_event_info_instance;
}/* ds3stkproc_create_ss_event_info_instance() */

/*===========================================================================
FUNCTION      ds3gstkproc_get_radio_to_stack_table_instance

DESCRIPTION   This utility function will return a pointer to the radio to 
              stack table for the passed sub id 

PARAMETERS    subs_id: ds3g data sub id.

DEPENDENCIES  None.

RETURN VALUE  pointer to ds3g_radio_stack_table[subs_id] 
              NULL pointer if subs_id is invalid 

SIDE EFFECTS  None.
===========================================================================*/
ds3g_radio_to_stack_table_type* ds3gstkproc_get_radio_to_stack_table_instance
(
  ds3gsubsmgr_subs_id_e_type  subs_id
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(!ds3gsubsmgr_is_ds3g_subs_index_valid(subs_id))
  {
    return NULL;
  }

  return ds3g_radio_to_stack_table[subs_id];
}/* ds3gstkproc_get_radio_to_stack_table_instance() */

/*===========================================================================
FUNCTION      DS3GSTKPROC_CREATE_RADIO_TO_STACK_TABLE_INSTANCE

DESCRIPTION   This function creates Radio to Stack table instance for subs_id.

PARAMETERS    subs_id: subscription id of the instance to be created.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
ds3g_radio_to_stack_table_type* ds3gstkproc_create_radio_to_stack_table_instance
(
  ds3gsubsmgr_subs_id_e_type  subs_id
)
{
  ds3g_radio_to_stack_table_type  *radio_to_stack_table_instance = NULL;
  ds3g_radio_index_type            indx;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  radio_to_stack_table_instance
    = (ds3g_radio_to_stack_table_type*)
        modem_mem_alloc(sizeof(ds3g_radio_to_stack_table_type),
                        MODEM_MEM_CLIENT_DATA);

  if(radio_to_stack_table_instance != NULL)
  {
    /*-------------------------------------------------------------------------
      Initialize system mode for all radios
    -------------------------------------------------------------------------*/
    memset(radio_to_stack_table_instance, 0,
           sizeof(ds3g_radio_to_stack_table_type));
  
    for(indx = DS3G_RADIO_INDEX_MIN; indx < DS3G_RADIO_INDEX_MAX; indx++)
    {
      radio_to_stack_table_instance->radio_info[indx].sys_mode
        = ds3gstkproc_convert_radio_index_to_sys_mode(indx);
      radio_to_stack_table_instance->radio_info[indx].stack_id
        = CM_NO_STACKS;
    }

    ds3g_radio_to_stack_table[subs_id] = radio_to_stack_table_instance;
  }
  return radio_to_stack_table_instance;
}/* ds3gstkproc_create_radio_to_stack_table_instance() */

/*===========================================================================
FUNCTION      DS3GSTKPROC_HANDLE_DEVICE_MODE_CHANGE

DESCRIPTION   This function handles device mode change.

PARAMETERS    device_mode: device mode from ds3gsubsmgr

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds3gstkproc_handle_device_mode_change
(
  sys_modem_device_mode_e_type  device_mode
)
{
  boolean                     create_subs_instance[DS3GSUBSMGR_SUBS_ID_COUNT];
  ds3gsubsmgr_subs_id_e_type  subs_indx = DS3GSUBSMGR_SUBS_ID_INVALID;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  memset(&create_subs_instance, 0, sizeof(boolean)*DS3GSUBSMGR_SUBS_ID_COUNT);
  switch(device_mode)
  {
    case SYS_MODEM_DEVICE_MODE_SINGLE_SIM:
      create_subs_instance[DS3GSUBSMGR_SUBS_ID_1] = TRUE;
      break;

    case SYS_MODEM_DEVICE_MODE_DUAL_SIM_DUAL_STANDBY:
    case SYS_MODEM_DEVICE_MODE_DUAL_SIM_DUAL_ACTIVE:
      create_subs_instance[DS3GSUBSMGR_SUBS_ID_1] = TRUE;
      create_subs_instance[DS3GSUBSMGR_SUBS_ID_2] = TRUE;
      break;

#if defined(FEATURE_TRIPLE_SIM)
    case SYS_MODEM_DEVICE_MODE_TRIPLE_SIM_TRIPLE_STANDBY:
      create_subs_instance[DS3GSUBSMGR_SUBS_ID_1] = TRUE;
      create_subs_instance[DS3GSUBSMGR_SUBS_ID_2] = TRUE;
      create_subs_instance[DS3GSUBSMGR_SUBS_ID_3] = TRUE;
      break;
#endif /*defined(FEATURE_TRIPLE_SIM)*/

    default:
      break;
  }

  /* create DSDIF radio info instance based on device mode */
  for(subs_indx = DS3GSUBSMGR_SUBS_ID_MIN;
      subs_indx < DS3GSUBSMGR_SUBS_ID_COUNT;
      subs_indx++)
  {
    if(create_subs_instance[subs_indx] == TRUE)
    {
      /* Create SS event info instance */
      if(ds3g_ss_event_info[subs_indx] == NULL)
      {
        if(ds3gstkproc_create_ss_event_info_instance(subs_indx) == NULL)
        {
          DATA_ERR_FATAL("Error while creating SS event info instance. return.");
        }
        else
        {
          DATA_MSG1(MSG_LEGACY_LOW,
                    "create SS event info instance success for subs_id:%d.",
                    subs_indx);
        }
      }
      else
      {
        ds3gstkproc_process_cached_stack_info(
            ds3gsubsmgr_subs_id_ds3g_to_cm(subs_indx));
      }

      /* Create radio to stack table instance */
      if(ds3g_radio_to_stack_table[subs_indx] == NULL)
      {
        if(ds3gstkproc_create_radio_to_stack_table_instance(subs_indx) == NULL)
        {
          DATA_ERR_FATAL("Error while creating radio to stack table instance");
        }
        else
        {
          DATA_MSG1(MSG_LEGACY_LOW,
                    "create radio to stack table instance success for "
                    "subs_id:%d", subs_indx);
        }
      }
    }
  }

  return;
}/* ds3gstkproc_handle_device_mode_change() */

/*===========================================================================
FUNCTION      DS3GSTKPROC_SUBSMGR_EVENT_CB

DESCRIPTION   This callback is called by DS3GSUBSMGR to notify events.

PARAMETERS    event_id: DS3GSUBSMGR event id.
              event_info_ptr: event info containing new system info 
              cb_data_ptr: not used

DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  None.
===========================================================================*/
void ds3gstkproc_subsmgr_event_cb
(
  ds3gsubsmgr_event_type        event_id,
  ds3gsubsmgr_event_info_type  *event_info_ptr,
  void                         *data_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Check if callback pointer is NULL
  -------------------------------------------------------------------------*/
  if(event_info_ptr == NULL)
  {
    DATA_MSG0(MSG_LEGACY_LOW, "NULL event info pointer");
    return;
  }

  DATA_MSG1(MSG_LEGACY_HIGH, "ds3gstkproc_subsmgr_event_cb event_id:%d",
            event_id);
  switch(event_id)
  {
    case DS3GSUBSMGR_DEVICE_MODE_EV:
      ds3gstkproc_handle_device_mode_change(event_info_ptr->device_mode.curr);
      break;

    default:
      break;
  }

  return;
}/* ds3gstkproc_subsmgr_event_cb() */

/*===========================================================================
FUNCTION      DS3GSTKPROC_NOTIFY_CSD_HDLR

DESCRIPTION   This utility function notifies CSD Hdlr.

PARAMETERS    sys_mode: system mode
              srv_status: service status
              srv_domain: service domain

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds3gstkproc_notify_csd_hdlr
(
  sys_sys_mode_e_type     sys_mode,
  sys_srv_status_e_type   srv_status,
  sys_srv_domain_e_type   srv_domain,
  sys_modem_as_id_e_type  as_id
)
{
  ds3gcsdhdlr_radio_info_type  ds3gcsdhdlr_event_info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  memset(&ds3gcsdhdlr_event_info, 0, sizeof(ds3gcsdhdlr_radio_info_type));
  ds3gcsdhdlr_event_info.sys_mode = sys_mode;
  ds3gcsdhdlr_event_info.srv_status = srv_status;
  ds3gcsdhdlr_event_info.srv_domain = srv_domain;
  ds3gcsdhdlr_event_notify(&ds3gcsdhdlr_event_info,
                           ds3gsubsmgr_subs_id_cm_to_ds3g(as_id));
  return;
}/* ds3gstkproc_notify_csd_hdlr*/

/*===========================================================================
FUNCTION      DS3GSTKPROC_SET_STACK_ID_OF_RADIO

DESCRIPTION   Sets stack id of passed sys_mode in the radio to stack table

PARAMETERS    sys_mode: system mode
              stack_id: CM stack id
 
DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds3gstkproc_set_stack_id_of_radio
(
  sys_modem_as_id_e_type   asubs_id,
  sys_sys_mode_e_type      sys_mode,
  uint8                    stack_id
)
{
  ds3g_radio_index_type            index = DS3G_RADIO_INDEX_MAX;
  ds3g_radio_to_stack_table_type  *radio_to_stack_table_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(stack_id >= CM_NO_STACKS)
  {
    DATA_MSG1(MSG_LEGACY_LOW, "Invalid stack_id: %d", stack_id);
    return;
  }

  if(!ds3gsubsmgr_is_subs_id_valid(asubs_id))
  {
    return;
  }

  radio_to_stack_table_ptr = ds3gstkproc_get_radio_to_stack_table_instance(
                                ds3gsubsmgr_subs_id_cm_to_ds3g(asubs_id));

  if(radio_to_stack_table_ptr != NULL)
  {
    /* get index from sys mode */
    index = ds3gstkproc_convert_sys_mode_to_radio_index(sys_mode);

    if(index != DS3G_RADIO_INDEX_MAX)
    {
      radio_to_stack_table_ptr->radio_info[index].stack_id = stack_id;
    }
  }
  return;
}/* ds3gstkproc_set_stack_id_of_radio() */

/*===========================================================================
FUNCTION      DS3GSTKPROC_CHECK_STACK_AND_NOTIFY_LOST

DESCRIPTION   This utility function checks and notifies radio lost event to 
              DSD and CSD Hdlr if cached sys mode was in the passed stack.
              Else it ignores.

PARAMETERS    sys_mode: system mode
              stack_id: CM stack id

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds3gstkproc_check_stack_and_notify_lost
(
  sys_modem_as_id_e_type   asubs_id,
  sys_sys_mode_e_type      sys_mode,
  uint8                    stack_id
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if((sys_mode < SYS_SYS_MODE_NO_SRV) || (sys_mode >= SYS_SYS_MODE_MAX))
  {
    DATA_MSG1(MSG_LEGACY_LOW, "Invalid sys_mode: %d", sys_mode);
    return;
  }

  if(stack_id >= CM_NO_STACKS)
  {
    DATA_MSG1(MSG_LEGACY_LOW, "Invalid stack_id: %d", stack_id);
    return;
  }

  /*-------------------------------------------------------------------
    Notify radio lost to DSD if sys mode is present on the same
    stack. This is to avoid sending radio lost event to DSD in case
    radio moved from one stack to another.
  -------------------------------------------------------------------*/
  if(stack_id == ds3gstkproc_get_stack_id_from_radio(asubs_id, sys_mode))
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW,"Report radio lost to DSD");
    ds3gdsdif_notify_dsd_lost_event(sys_mode, asubs_id);

    DATA_3GMGR_MSG0(MSG_LEGACY_LOW,"Report radio lost to CSD HDLR");
    ds3gstkproc_notify_csd_hdlr(sys_mode,
                                SYS_SRV_STATUS_NO_SRV,
                                SYS_SRV_DOMAIN_NO_SRV,
                                asubs_id);
  }
  return;
}/* ds3gstkproc_check_stack_and_notify_lost() */

/*===========================================================================
FUNCTION      DS3GSTKPROC_GET_RAT_MASK_FROM_CELL_SRV_IND

DESCRIPTION   This utility function returns RAT mask from the Cell service ind.

PARAMETERS    sys_mode: system mode
              cell_srv_ind_ptr: pointer to cell service indication

DEPENDENCIES  None.

RETURN VALUE  RAT mask.

SIDE EFFECTS  None.
===========================================================================*/
uint32 ds3gstkproc_get_rat_mask_from_cell_srv_ind
(
  sys_sys_mode_e_type         sys_mode,
  cm_cell_srv_ind_s_type     *cell_srv_ind_ptr
)
{
  uint32 rat_mask = DS_SYS_RAT_3GPP_NULL_BEARER;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(cell_srv_ind_ptr == NULL)
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "NULL pointer passed");
    return rat_mask;
  }

  switch(sys_mode)
  {
    case SYS_SYS_MODE_GSM:
      if(cell_srv_ind_ptr->egprs_supp == SYS_EGPRS_SUPPORT_AVAIL)
      {
        rat_mask = PS_SYS_RAT_3GPP_EDGE;
      }
      else
      {
        rat_mask = PS_SYS_RAT_3GPP_GPRS;
      }
      break;
#if defined(FEATURE_WCDMA) || defined(FEATURE_TDSCDMA)
    case SYS_SYS_MODE_WCDMA:
    case SYS_SYS_MODE_TDS:
      rat_mask 
        = ds_wcdma_bearer_cntxt_get_ps_rat_mask(
           sys_mode,
           ds_wcdma_bearer_cntxt_get_hs_status_from_cell_srv_ind(
              cell_srv_ind_ptr));
      break;
#endif /*(FEATURE_WCDMA) || (FEATURE_TDSCDMA)*/
    default:
      break;
  }

  DATA_3GMGR_MSG2(MSG_LEGACY_LOW,
                  "ds3gstkproc_get_rat_mask_from_cell_srv_ind: "
                  "sys_mode:%d rat_mask:%d",
                  sys_mode,rat_mask);

  return rat_mask;
}/* ds3gstkproc_get_rat_mask_from_cell_srv_ind() */

/*===========================================================================
FUNCTION      DS3GSTKPROC_PROCESS_CDMA_INFO

DESCRIPTION   This utility function handles CDMA radio.

PARAMETERS    force_dsd_proc: radio newly acquired. 
              stack_info_ptr: stack info pointer
              cached_stack_info_ptr: cached stack info pointer

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds3gstkproc_process_cdma_info
(
  boolean                      force_dsd_proc,
  sys_modem_as_id_e_type       asubs_id,
  ds3g_cm_ss_stack_info_type  *stack_info_ptr,
  ds3g_cm_ss_stack_info_type  *cached_stack_info_ptr,
  uint8                        stack_id,
  uint64                       changed_fields_subs
)
{
  uint32                       rat_mask = 0;
  uint32                       so_mask  = 0;
  boolean                      notify_radio_acq = FALSE;
  boolean                      csfb_call_active_flag = FALSE;
  boolean                      pref_radio_diff = FALSE;
  ds3g_ss_event_info_type     *ss_event_info_ptr = NULL;
#ifdef FEATURE_DATA_IS707
  boolean                      ret_val = FALSE;
#endif /* FEATURE_DATA_IS707 */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if((stack_info_ptr == NULL) || (cached_stack_info_ptr == NULL))
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "NULL ptr passed");
    return;
  }

  DATA_3GMGR_MSG4(MSG_LEGACY_MED,
                  "ds3gstkproc_process_cdma_info: force_dsd_proc:%d "
                  "asubs_id:%d srv_status:%d srv_domain:%d",
                  force_dsd_proc,
                  asubs_id,
                  stack_info_ptr->srv_status,
                  stack_info_ptr->srv_domain);

  /*-------------------------------------------------------------------
     Notify Ds3G CSD handler if CS service domain supported
      - for 1x always use effective_srv_domain
  -------------------------------------------------------------------*/
  if(ds3gstkproc_is_srv_domain_cs_supported(
                         stack_info_ptr->effective_srv_domain) == TRUE)
  {
    ds3gstkproc_notify_csd_hdlr(stack_info_ptr->sys_mode,
                                stack_info_ptr->srv_status,
                                stack_info_ptr->effective_srv_domain,
                                asubs_id);
  }
  else
  {
    ds3gstkproc_notify_csd_hdlr(stack_info_ptr->sys_mode,
                                SYS_SRV_STATUS_NO_SRV,
                                SYS_SRV_DOMAIN_NO_SRV,
                                asubs_id);
  }

  /*-------------------------------------------------------------------
    Check if PS domain is supported. For 1x, new field
    effective_srv_domain needs to be used instead of srv_domain.
    This is to support multi-sim scenarios where 1x is present on
    non-DDS subscription. In this case, effective_srv_domain will be
    reported as CS_ONLY. For all Single sim scenarios, effective_srv_domain
    will always be reported as CS_PS.
  -------------------------------------------------------------------*/
  if(ds3gstkproc_is_srv_domain_ps_supported(
                         stack_info_ptr->effective_srv_domain) == FALSE)
  {
    rat_mask = DS_SYS_RAT_3GPP_NULL_BEARER;
    so_mask = 0;
  }
  else
  {
#ifdef FEATURE_DATA_IS707
    /* Get 1x information from 3GPP2 MH */
    ret_val = ds707_pkt_mgr_get_cdma_rat_info(&rat_mask, &so_mask);
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW, "Get CDMA info success: %d", ret_val);
#endif /* FEATURE_DATA_IS707 */
  }

  /* Get SS event info pointer */
  ss_event_info_ptr = ds3gstkproc_get_ss_event_info_instance(
                        ds3gsubsmgr_subs_id_cm_to_ds3g(asubs_id));
  if(ss_event_info_ptr == NULL)
  {
    return;
  }
  csfb_call_active_flag = ss_event_info_ptr->is_csfb_call_active;

  /*-------------------------------------------------------------------------
    Check if radio acquired is different from current preferred radio
  -------------------------------------------------------------------------*/
  if(ds3gdsdif_get_preferred_sys(ds3gsubsmgr_subs_id_cm_to_ds3g(asubs_id))
       != SYS_SYS_MODE_CDMA)
  {
    pref_radio_diff = TRUE;
  }

  if(force_dsd_proc || pref_radio_diff) /* change of radio in stack */
  {
#ifdef FEATURE_LTE_TO_1X
    DATA_3GMGR_MSG1(MSG_LEGACY_MED,"is_csfb_call_active: %d",
                    csfb_call_active_flag);

    if (csfb_call_active_flag == TRUE)
    {
      /*-------------------------------------------------------------
      This is for voice call. Report that 1x for Data is lost. 
      
      !!! Note: the assumption is that while operating in CSFB mode, 
         simultaneous voice and data is supported in 1x channel.
         e.g. SVD feature does not work.
       
         Not reporting here also may work but there is no harm in
         loading 1X DSD with loss information.
      -------------------------------------------------------------*/
      DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "Report radio lost to DSD");

      /* report radio lost to DSD */
      ds3gdsdif_notify_dsd_lost_event(SYS_SYS_MODE_CDMA, asubs_id);
    }
    else
#endif /* FEATURE_LTE_TO_1X */
    {
      DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "Report radio acq to DSD");

      /* report radio acq to DSD  */
      ds3gdsdif_notify_dsd_acq_lost_event(DS_DSD_CLIENT_RADIO_ACQUIRED_EV,
                                          asubs_id,
                                          SYS_SYS_MODE_CDMA,
                                          rat_mask,
                                          so_mask,
                                          stack_info_ptr->srv_status,
                                          stack_info_ptr->effective_srv_domain);
    }
  }
  else /* no change of radio in stack*/
  {
#ifdef FEATURE_LTE_TO_1X
    /*------------------------------------------------------------------- 
      Check if CSFB call active mask has been set -> CSFB value change
    -------------------------------------------------------------------*/
    if ((changed_fields_subs & CM_SS_EVT_CSFB_CALL_ACTIVE_MASK) != 0)
    {
      DATA_3GMGR_MSG1(MSG_LEGACY_MED,
                      "CSFB_CALL_ACTIVE_MASK set. is_csfb_call_active: %d",
                      csfb_call_active_flag);

      if(csfb_call_active_flag == FALSE)
      {
        DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "Report radio acq to DSD");

        /* report radio acq to DSD */
        ds3gdsdif_notify_dsd_acq_lost_event(DS_DSD_CLIENT_RADIO_ACQUIRED_EV,
                                            asubs_id,
                                            SYS_SYS_MODE_CDMA,
                                            rat_mask,
                                            so_mask,
                                            stack_info_ptr->srv_status,
                                            stack_info_ptr->effective_srv_domain);
      }
      else
      {
        DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "Report radio lost to DSD");

        /* report radio lost to DSD */
        ds3gdsdif_notify_dsd_lost_event(SYS_SYS_MODE_CDMA, asubs_id);
      }
    }
#endif /* FEATURE_LTE_TO_1X */

    /*------------------------------------------------------------------- 
      Check if prev mask has been set -> notify to DSD if csfb inactive
    -------------------------------------------------------------------*/
    if((stack_info_ptr->changed_fields & CM_SS_EVT_BS_P_REV_MASK) != 0)
    {
      if(csfb_call_active_flag == FALSE)
      {
        DATA_3GMGR_MSG0(MSG_LEGACY_MED,
                        "CSFB call inactive. Notifying P_Rev change to DSD");

        /* report radio acq to DSD */
        ds3gdsdif_notify_dsd_acq_lost_event(DS_DSD_CLIENT_RADIO_ACQUIRED_EV,
                                            asubs_id,
                                            SYS_SYS_MODE_CDMA,
                                            rat_mask,
                                            so_mask,
                                            stack_info_ptr->srv_status,
                                            stack_info_ptr->effective_srv_domain);
      }
      else
      {
        DATA_3GMGR_MSG0(MSG_LEGACY_MED,
                        "CSFB call active. Not notifying P_Rev change to DSD");
      }
    } /* End if prev changed*/

    /* check if srv_status has changed */
    if(stack_info_ptr->srv_status != cached_stack_info_ptr->srv_status)
    {
      notify_radio_acq = TRUE;
    }

    /* check if srv_domain has changed */
    if(stack_info_ptr->effective_srv_domain !=
       cached_stack_info_ptr->effective_srv_domain)
    {
      notify_radio_acq = TRUE;
    }

    if(notify_radio_acq)
    {
      ds3gdsdif_notify_dsd_acq_lost_event(DS_DSD_CLIENT_RADIO_ACQUIRED_EV,
                                          asubs_id,
                                          SYS_SYS_MODE_CDMA,
                                          rat_mask,
                                          so_mask,
                                          stack_info_ptr->srv_status,
                                          stack_info_ptr->effective_srv_domain);
    }
  } /* End no change of radio in stack*/
  return;
}/* ds3gstkproc_process_cdma_info() */

/*===========================================================================
FUNCTION      DS3GSTKPROC_PROCESS_HDR_INFO

DESCRIPTION   This utility function handles HDR radio.

PARAMETERS    force_dsd_proc: radio newly acquired
              stack_info_ptr: stack info pointer
              cached_stack_info_ptr: cached stack info pointer

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds3gstkproc_process_hdr_info
(
  boolean                      force_dsd_proc,
  sys_modem_as_id_e_type       asubs_id,
  ds3g_cm_ss_stack_info_type  *stack_info_ptr,
  ds3g_cm_ss_stack_info_type  *cached_stack_info_ptr,
  uint8                        stack_id
)
{
  boolean                      notify_radio_acq = FALSE;
  uint32                       rat_mask = 0;
  uint32                       so_mask = 0;
#ifdef FEATURE_DATA_IS707
  boolean                      ret_val = FALSE;
#endif /* FEATURE_DATA_IS707 */
  boolean                      pref_radio_diff = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if((stack_info_ptr == NULL) || (cached_stack_info_ptr == NULL))
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "NULL ptr passed");
    return;
  }

  DATA_3GMGR_MSG4(MSG_LEGACY_MED,
                  "ds3gstkproc_process_hdr_info: force_dsd_proc:%d "
                  "asubs_id:%d srv_status:%d srv_domain:%d",
                  force_dsd_proc,
                  asubs_id,
                  stack_info_ptr->srv_status,
                  stack_info_ptr->srv_domain);

  /*-------------------------------------------------------------------
     Notify Ds3G CSD handler if CS service domain supported
  -------------------------------------------------------------------*/
  if(ds3gstkproc_is_srv_domain_cs_supported(stack_info_ptr->srv_domain)
       == TRUE)
  {
    ds3gstkproc_notify_csd_hdlr(stack_info_ptr->sys_mode,
                                stack_info_ptr->srv_status,
                                stack_info_ptr->srv_domain,
                                asubs_id);
  }
  else
  {
    ds3gstkproc_notify_csd_hdlr(stack_info_ptr->sys_mode,
                                SYS_SRV_STATUS_NO_SRV,
                                SYS_SRV_DOMAIN_NO_SRV,
                                asubs_id);
  }

#ifdef FEATURE_DATA_IS707
  /* Get HDR session information from 3GPP2 MH */
  ret_val = ds707_pkt_mgr_get_hdr_sess_info(&rat_mask, &so_mask);
  DATA_3GMGR_MSG1(MSG_LEGACY_LOW, "Get HDR sess info success: %d", ret_val);
#endif /* FEATURE_DATA_IS707 */

  /*-------------------------------------------------------------------------
    Check if radio acquired is different from current preferred radio
  -------------------------------------------------------------------------*/
  if(ds3gdsdif_get_preferred_sys(ds3gsubsmgr_subs_id_cm_to_ds3g(asubs_id))
       != SYS_SYS_MODE_HDR)
  {
    pref_radio_diff = TRUE;
  }

  if(force_dsd_proc || pref_radio_diff)
  {
    /* report radio acq to DSD */
    ds3gdsdif_notify_dsd_acq_lost_event(DS_DSD_CLIENT_RADIO_ACQUIRED_EV,
                                        asubs_id,
                                        SYS_SYS_MODE_HDR,
                                        rat_mask,
                                        so_mask,
                                        stack_info_ptr->srv_status,
                                        stack_info_ptr->srv_domain);
  }
  else
  {
    /* check if srv_status has changed */
    if(stack_info_ptr->srv_status != cached_stack_info_ptr->srv_status)
    {
      notify_radio_acq = TRUE;
    }

    /* check if srv_domain has changed */
    if(stack_info_ptr->srv_domain != cached_stack_info_ptr->srv_domain)
    {
      notify_radio_acq = TRUE;
    }

    if(notify_radio_acq)
    {
      ds3gdsdif_notify_dsd_acq_lost_event(DS_DSD_CLIENT_RADIO_ACQUIRED_EV,
                                          asubs_id,
                                          SYS_SYS_MODE_HDR,
                                          rat_mask,
                                          so_mask,
                                          stack_info_ptr->srv_status,
                                          stack_info_ptr->srv_domain);
    }
  }
  return;
}/* ds3gstkproc_process_hdr_info() */

/*===========================================================================
FUNCTION      DS3GSTKPROC_PROCESS_GSM_INFO

DESCRIPTION   This utility function handles GSM radio.

PARAMETERS    force_dsd_proc: radio newly acquired
              stack_info_ptr: stack info pointer
              cached_stack_info_ptr: cached stack info pointer

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds3gstkproc_process_gsm_info
(
  boolean                      force_dsd_proc,
  sys_modem_as_id_e_type       asubs_id,
  ds3g_cm_ss_stack_info_type  *stack_info_ptr,
  ds3g_cm_ss_stack_info_type  *cached_stack_info_ptr,
  boolean                      ps_data_suspend,
  boolean                      cached_ps_data_suspend,
  uint8                        stack_id
)
{
  uint32                          rat_mask;
  uint32                          cached_rat_mask;
  boolean                         notify_radio_acq = FALSE;
  ds3gdsdif_voice_call_state_type  voice_call_state;
  ds3gdsdif_voice_call_state_type  emerg_call_state;
  boolean                         pref_radio_diff = FALSE;
  ds3gsubsmgr_subs_id_e_type       other_subs_id;
  sys_srv_status_e_type            srv_status;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if((stack_info_ptr == NULL) || (cached_stack_info_ptr == NULL))
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "NULL ptr passed");
    return;
  }

  DATA_3GMGR_MSG6(MSG_LEGACY_MED,
                  "ds3gstkproc_process_gsm_info: force_dsd_proc:%d "
                  "asubs_id:%d srv_status:%d srv_domain:%d "
                  "ps_data_suspend:%d cached_ps_data_suspend:%d",
                  force_dsd_proc,
                  asubs_id,
                  stack_info_ptr->srv_status,
                  stack_info_ptr->srv_domain,
                  ps_data_suspend,
                  cached_ps_data_suspend);

  /*-------------------------------------------------------------------
     Check if srv domain camped can be ignored.
  -------------------------------------------------------------------*/
  if(ds3gstkproc_ignore_srv_domain_camped(stack_info_ptr, cached_stack_info_ptr)
     == TRUE)
  {
    return;
  }

  srv_status = stack_info_ptr->srv_status;

  /*-------------------------------------------------------------------
     Notify DS3G CSD handler if CS service domain supported
  -------------------------------------------------------------------*/
  if(ds3gstkproc_is_srv_domain_cs_supported(stack_info_ptr->srv_domain)
       == TRUE)
  {
    ds3gstkproc_notify_csd_hdlr(stack_info_ptr->sys_mode,
                                srv_status,
                                stack_info_ptr->srv_domain,
                                asubs_id);
  }
  else
  {
    ds3gstkproc_notify_csd_hdlr(stack_info_ptr->sys_mode,
                                SYS_SRV_STATUS_NO_SRV,
                                SYS_SRV_DOMAIN_NO_SRV,
                                asubs_id);
  }

  rat_mask 
    = ds3gstkproc_get_rat_mask_from_cell_srv_ind(
       SYS_SYS_MODE_GSM, &(stack_info_ptr->cell_srv_ind));

  cached_rat_mask 
    = ds3gstkproc_get_rat_mask_from_cell_srv_ind(
       SYS_SYS_MODE_GSM, &(cached_stack_info_ptr->cell_srv_ind));

  /*-------------------------------------------------------------------
     If srv_domain is not supported,
       - If srv_capability is not PS capabale
          -> notify radio lost to DSD
       - Else,
          -> rat_mask = NULL bearer
  -------------------------------------------------------------------*/
  if(ds3gstkproc_is_srv_domain_ps_supported(stack_info_ptr->srv_domain)
       == FALSE)
  {
    if((stack_id == DS3G_SS_STACK_ID_1) &&
       ((stack_info_ptr->srv_domain == SYS_SRV_DOMAIN_CS_ONLY)||
        (stack_info_ptr->srv_domain == SYS_SRV_DOMAIN_CAMPED)))
    {
      rat_mask = DS_SYS_RAT_3GPP_NULL_BEARER;
    }
    else
    {
      DATA_3GMGR_MSG1(MSG_LEGACY_LOW, "Report radio lost to DSD. sys_mode:%d",
                      SYS_SYS_MODE_GSM);
      ds3gdsdif_notify_dsd_lost_event(SYS_SYS_MODE_GSM, asubs_id);
      return;
    }
  }

  voice_call_state
    = ds3gdsdif_get_voice_call_state(ds3gsubsmgr_subs_id_cm_to_ds3g(asubs_id));
  emerg_call_state
    = ds3gdsdif_get_emerg_call_state(ds3gsubsmgr_subs_id_cm_to_ds3g(asubs_id));

  /*-------------------------------------------------------------------------
    This is to handle DTM change when voice call is connected
    If voice/emergency call is ON and DTM not supported,
      - Then, change rat_mask to NULL bearer
  -------------------------------------------------------------------------*/
  if( ((voice_call_state.is_call_active == TRUE)  ||
       (emerg_call_state.is_call_active == TRUE))  &&
       (stack_info_ptr->cell_srv_ind.dtm_supp == SYS_DTM_SUPPORT_NOT_AVAIL))
  {
    DATA_3GMGR_MSG5(MSG_LEGACY_MED,
                    "mode:%d voice_call:%d emerg_call:%d DTM:%d "
                    "ps_data_suspend:%d Sending NULL bearer to DSD",
                    SYS_SYS_MODE_GSM,
                    voice_call_state.is_call_active,
                    emerg_call_state.is_call_active,
                    stack_info_ptr->cell_srv_ind.dtm_supp,
                    ps_data_suspend);

    rat_mask = DS_SYS_RAT_3GPP_NULL_BEARER;
  }

  /*-------------------------------------------------------------------------
    Handle only if suppress_gsm_on_srvcc_csfb NV set
  -------------------------------------------------------------------------*/
  if( ds3g_get_suppress_gsm_on_srvcc_csfb(asubs_id) == TRUE )
  {
    /*-----------------------------------------------------------------------
    Set RAT mask to NULL bearer once SRVCC is ended until RAU/TAU procedure
    or fast return to LTE is complete i.e. PS Suspend flag is set
    -----------------------------------------------------------------------*/
    if((voice_call_state.is_call_active == FALSE) &&
       ((voice_call_state.info_p != NULL) &&
        (voice_call_state.info_p->radio == SYS_SYS_MODE_GSM) &&
        (voice_call_state.info_p->voice_ho_type == SYS_VOICE_HO_SRVCC_L_2_G)) &&
     (stack_info_ptr->cell_srv_ind.dtm_supp == SYS_DTM_SUPPORT_NOT_AVAIL))
  {
    DATA_3GMGR_MSG5(MSG_LEGACY_MED,
                    "mode:%d voice_call:%d DTM:%d voice_ho_type:%d "
                    "ps_data_suspend:%d Sending NULL bearer to DSD",
                    SYS_SYS_MODE_GSM,
                      voice_call_state.is_call_active,
                    stack_info_ptr->cell_srv_ind.dtm_supp,
                      voice_call_state.info_p->voice_ho_type,
                    ps_data_suspend);

    if(ps_data_suspend == TRUE)
    {
      rat_mask = DS_SYS_RAT_3GPP_NULL_BEARER;
    }
    else
    {
      /* Reset voice HO type */
        DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "Resetting voice_ho_type");
        voice_call_state.info_p->voice_ho_type = SYS_VOICE_HO_NONE;
    }
  }

    if((emerg_call_state.is_call_active == FALSE) &&
       ((emerg_call_state.info_p != NULL) &&
        (emerg_call_state.info_p->radio == SYS_SYS_MODE_GSM) &&
        (emerg_call_state.info_p->voice_ho_type == SYS_VOICE_HO_SRVCC_L_2_G)) &&
     (stack_info_ptr->cell_srv_ind.dtm_supp == SYS_DTM_SUPPORT_NOT_AVAIL))
  {
    DATA_3GMGR_MSG5(MSG_LEGACY_MED,
                    "mode:%d emerg_call:%d DTM:%d emerg_ho_type:%d "
                    "ps_data_suspend:%d Sending NULL bearer to DSD",
                    SYS_SYS_MODE_GSM,
                      emerg_call_state.is_call_active,
                    stack_info_ptr->cell_srv_ind.dtm_supp,
                      emerg_call_state.info_p->voice_ho_type,
                    ps_data_suspend);

    if(ps_data_suspend == TRUE)
    {
      rat_mask = DS_SYS_RAT_3GPP_NULL_BEARER;
    }
    else
    {
      /* Reset voice HO type */
        DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "Resetting voice_ho_type");
        emerg_call_state.info_p->voice_ho_type = SYS_VOICE_HO_NONE;
    }
  }
  }

  if( (ps_data_suspend == TRUE) && (ds3gsubsmgr_get_device_mode() == 
      SYS_MODEM_DEVICE_MODE_DUAL_SIM_DUAL_STANDBY) )
  {
    other_subs_id = DS3GDSDIF_GET_OTHER_SUBS_INDX(asubs_id);
    voice_call_state = ds3gdsdif_get_voice_call_state(other_subs_id);
    if (voice_call_state.is_call_active == TRUE)
    {
      srv_status = SYS_SRV_STATUS_LIMITED;  
    }
  }

  /*-------------------------------------------------------------------------
    If service status is not full service, set RAT mask to NULL bearer
  -------------------------------------------------------------------------*/
  if(srv_status != SYS_SRV_STATUS_SRV)
  {
    rat_mask = DS_SYS_RAT_3GPP_NULL_BEARER;
  }

  /*-------------------------------------------------------------------------
    Check if radio acquired is different from current preferred radio
  -------------------------------------------------------------------------*/
  if(ds3gdsdif_get_preferred_sys(ds3gsubsmgr_subs_id_cm_to_ds3g(asubs_id))
       != SYS_SYS_MODE_GSM)
  {
    pref_radio_diff = TRUE;
  }

  if(force_dsd_proc || pref_radio_diff)
  {
    /* report radio acq to DSD */
    ds3gdsdif_notify_dsd_acq_lost_event(DS_DSD_CLIENT_RADIO_ACQUIRED_EV,
                                        asubs_id,
                                        SYS_SYS_MODE_GSM,
                                        rat_mask,
                                        0,
                                        srv_status,
                                        stack_info_ptr->srv_domain);
  }
  else if(((voice_call_state.info_p != NULL) &&
           (voice_call_state.info_p->state 
              == DS3GDSDIF_VOICE_CALL_INACTIVE_PENDING))  ||
          ((emerg_call_state.info_p != NULL) &&
           (emerg_call_state.info_p->state 
              == DS3GDSDIF_VOICE_CALL_INACTIVE_PENDING)))
  {
    /* report radio acq to DSD */
    ds3gdsdif_notify_dsd_acq_lost_event(DS_DSD_CLIENT_RADIO_ACQUIRED_EV,
                                        asubs_id,
                                        SYS_SYS_MODE_GSM,
                                        rat_mask,
                                        0,
                                        srv_status,
                                        stack_info_ptr->srv_domain);
  }
  else
  {
    /* check if srv_status has changed */
    if(srv_status != cached_stack_info_ptr->srv_status)
    {
      notify_radio_acq = TRUE;
    }

    /* check if srv_domain has changed */
    if(stack_info_ptr->srv_domain != cached_stack_info_ptr->srv_domain)
    {
      notify_radio_acq = TRUE;
    }

    /* check if rat_mask has changed */
    if(rat_mask != cached_rat_mask)
    {
      notify_radio_acq = TRUE;
    }

    /* check if DTM has changed */
    if(stack_info_ptr->cell_srv_ind.dtm_supp !=
       cached_stack_info_ptr->cell_srv_ind.dtm_supp)
    {
      notify_radio_acq = TRUE;
    }

    /* check if ps_data_suspend flag has changed */
    if(ps_data_suspend != cached_ps_data_suspend)
    {
      notify_radio_acq = TRUE;
    }


    if(notify_radio_acq)
    {
      ds3gdsdif_notify_dsd_acq_lost_event(DS_DSD_CLIENT_RADIO_ACQUIRED_EV,
                                          asubs_id,
                                          SYS_SYS_MODE_GSM,
                                          rat_mask,
                                          0,
                                          srv_status,
                                          stack_info_ptr->srv_domain);
    }
  }
  return;
}/* ds3gstkproc_process_gsm_info() */

/*===========================================================================
FUNCTION      DS3GSTKPROC_PROCESS_WCDMA_TDS_INFO

DESCRIPTION   This utility function handles WCDMA and TDS radio.

PARAMETERS    force_dsd_proc: radio newly acquired
              stack_info_ptr: stack info pointer
              cached_stack_info_ptr: cached stack info pointer

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds3gstkproc_process_wcdma_tds_info
(
  boolean                      force_dsd_proc,
  sys_modem_as_id_e_type       asubs_id,
  ds3g_cm_ss_stack_info_type  *stack_info_ptr,
  ds3g_cm_ss_stack_info_type  *cached_stack_info_ptr,
  boolean                      ps_data_suspend,
  boolean                      cached_ps_data_suspend,
  uint8                        stack_id
)
{
  uint32                       rat_mask;
  uint32                       cached_rat_mask;
  boolean                      notify_radio_acq = FALSE;
  boolean                      pref_radio_diff = FALSE;
  ds3gdsdif_voice_call_state_type  voice_call_state;
  ds3gsubsmgr_subs_id_e_type       other_subs_id;
  sys_srv_status_e_type            srv_status;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if((stack_info_ptr == NULL) || (cached_stack_info_ptr == NULL))
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "NULL ptr passed");
    return;
  }

  DATA_3GMGR_MSG7(MSG_LEGACY_MED,
                  "ds3gstkproc_process_wcdma_tds_info: force_dsd_proc:%d "
                  "asubs_id:%d sys_mode:%d srv_status:%d srv_domain:%d "
                  "ps_data_suspend:%d cached_ps_data_suspend:%d",
                  force_dsd_proc,
                  asubs_id,
                  stack_info_ptr->sys_mode,
                  stack_info_ptr->srv_status,
                  stack_info_ptr->srv_domain,
                  ps_data_suspend,
                  cached_ps_data_suspend);

  /*-------------------------------------------------------------------
     Check if srv domain camped can be ignored.
  -------------------------------------------------------------------*/
  if(ds3gstkproc_ignore_srv_domain_camped(stack_info_ptr, cached_stack_info_ptr)
     == TRUE)
  {
    return;
  }

  srv_status = stack_info_ptr->srv_status;

  /*-------------------------------------------------------------------
     Notify Ds3G CSD handler if CS service domain supported
  -------------------------------------------------------------------*/
  if(ds3gstkproc_is_srv_domain_cs_supported(stack_info_ptr->srv_domain) 
       == TRUE)
  {
    ds3gstkproc_notify_csd_hdlr(stack_info_ptr->sys_mode,
                                srv_status,
                                stack_info_ptr->srv_domain,
                                asubs_id);
  }
  else
  {
    ds3gstkproc_notify_csd_hdlr(stack_info_ptr->sys_mode,
                                SYS_SRV_STATUS_NO_SRV,
                                SYS_SRV_DOMAIN_NO_SRV,
                                asubs_id);
  }

  rat_mask 
    = ds3gstkproc_get_rat_mask_from_cell_srv_ind(stack_info_ptr->sys_mode,
                                               &(stack_info_ptr->cell_srv_ind));

  cached_rat_mask 
    = ds3gstkproc_get_rat_mask_from_cell_srv_ind(
       stack_info_ptr->sys_mode,
       &(cached_stack_info_ptr->cell_srv_ind));

  /*-------------------------------------------------------------------
     If srv_domain is not supported,
       - If srv_capability is not PS capabale
          -> notify radio lost to DSD
       - Else,
          -> rat_mask = NULL bearer
  -------------------------------------------------------------------*/
  if(ds3gstkproc_is_srv_domain_ps_supported(stack_info_ptr->srv_domain)
       == FALSE)
  {
    if((stack_id == DS3G_SS_STACK_ID_1) && 
       (stack_info_ptr->srv_domain == SYS_SRV_DOMAIN_CS_ONLY))
    {
      rat_mask = DS_SYS_RAT_3GPP_NULL_BEARER;
    }
    else
    {
      DATA_3GMGR_MSG1(MSG_LEGACY_LOW,"Report radio lost to DSD. sys_mode:%d",
                      SYS_SYS_MODE_WCDMA);
      ds3gdsdif_notify_dsd_lost_event(stack_info_ptr->sys_mode, asubs_id);
      return;
    }
  }

  if( (ps_data_suspend == TRUE) && (ds3gsubsmgr_get_device_mode() == 
      SYS_MODEM_DEVICE_MODE_DUAL_SIM_DUAL_STANDBY) )
  {
    other_subs_id = DS3GDSDIF_GET_OTHER_SUBS_INDX(asubs_id);
    voice_call_state = ds3gdsdif_get_voice_call_state(other_subs_id);
    if (voice_call_state.is_call_active == TRUE)
    {
      srv_status = SYS_SRV_STATUS_LIMITED;  
    }
  }

  /*-------------------------------------------------------------------------
    If service status is not full service, set RAT mask to NULL bearer
  -------------------------------------------------------------------------*/
  if(srv_status != SYS_SRV_STATUS_SRV)
  {
    rat_mask = DS_SYS_RAT_3GPP_NULL_BEARER;
  }

  /*-------------------------------------------------------------------------
    Check if radio acquired is different from current preferred radio
  -------------------------------------------------------------------------*/
  if(ds3gdsdif_get_preferred_sys(ds3gsubsmgr_subs_id_cm_to_ds3g(asubs_id))
       != stack_info_ptr->sys_mode)
  {
    pref_radio_diff = TRUE;
  }

  if(force_dsd_proc || pref_radio_diff)
  {
    /* report radio acq to DSD */
    ds3gdsdif_notify_dsd_acq_lost_event(DS_DSD_CLIENT_RADIO_ACQUIRED_EV,
                                        asubs_id,
                                        stack_info_ptr->sys_mode,
                                        rat_mask,
                                        0,
                                        srv_status,
                                        stack_info_ptr->srv_domain);
  }
  else
  {
    /* check if srv_status has changed */
    if(srv_status != cached_stack_info_ptr->srv_status)
    {
      notify_radio_acq = TRUE;
    }

    /* check if srv_domain has changed */
    if(stack_info_ptr->srv_domain != cached_stack_info_ptr->srv_domain)
    {
      notify_radio_acq = TRUE;
    }

    /* check if rat_mask has changed */
    if(rat_mask != cached_rat_mask)
    {
      notify_radio_acq = TRUE;
    }

    /* check if ps_data_suspend flag has changed */
    if(ps_data_suspend != cached_ps_data_suspend)
    {
      notify_radio_acq = TRUE;
    }

    if(notify_radio_acq)
    {
      ds3gdsdif_notify_dsd_acq_lost_event(DS_DSD_CLIENT_RADIO_ACQUIRED_EV,
                                          asubs_id,
                                          stack_info_ptr->sys_mode,
                                          rat_mask,
                                          0,
                                          srv_status,
                                          stack_info_ptr->srv_domain);
    }
  }
  return;
}/* ds3gstkproc_process_wcdma_tds_info() */

/*===========================================================================
FUNCTION      DS3GSTKPROC_PROCESS_LTE_INFO

DESCRIPTION   This utility function handles LTE radio.

PARAMETERS    force_dsd_proc: radio newly acquired
              stack_info_ptr: stack info pointer
              cached_stack_info_ptr: cached stack info pointer

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds3gstkproc_process_lte_info
(
  boolean                      force_dsd_proc,
  sys_modem_as_id_e_type       asubs_id,
  ds3g_cm_ss_stack_info_type  *stack_info_ptr,
  ds3g_cm_ss_stack_info_type  *cached_stack_info_ptr,
  boolean                      ps_data_suspend,
  boolean                      cached_ps_data_suspend,
  uint8                        stack_id
)
{
  boolean                      notify_radio_acq = FALSE;
  boolean                      pref_radio_diff = FALSE;
  ds3gdsdif_voice_call_state_type  voice_call_state;
  ds3gsubsmgr_subs_id_e_type       other_subs_id;
  sys_srv_status_e_type            srv_status;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if((stack_info_ptr == NULL) || (cached_stack_info_ptr == NULL))
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "NULL ptr passed");
    return;
  }

  DATA_3GMGR_MSG6(MSG_LEGACY_MED,
                  "ds3gstkproc_process_lte_info: force_dsd_proc:%d "
                  "asubs_id:%d srv_status:%d srv_domain:%d "
                  "ps_data_suspend:%d cached_ps_data_suspend:%d",
                  force_dsd_proc,
                  asubs_id,
                  stack_info_ptr->srv_status,
                  stack_info_ptr->srv_domain,
                  ps_data_suspend,
                  cached_ps_data_suspend);

  /*-------------------------------------------------------------------
     Check if srv domain camped can be ignored.
  -------------------------------------------------------------------*/
  if(ds3gstkproc_ignore_srv_domain_camped(stack_info_ptr, cached_stack_info_ptr)
     == TRUE)
  {
    return;
  }

  srv_status = stack_info_ptr->srv_status;

  /*-------------------------------------------------------------------
     Notify Ds3G CSD handler if CS service domain supported
  -------------------------------------------------------------------*/
  if(ds3gstkproc_is_srv_domain_cs_supported(stack_info_ptr->srv_domain)
       == TRUE)
  {
    ds3gstkproc_notify_csd_hdlr(stack_info_ptr->sys_mode,
                                srv_status,
                                stack_info_ptr->srv_domain,
                                asubs_id);
  }
  else
  {
    ds3gstkproc_notify_csd_hdlr(stack_info_ptr->sys_mode,
                                SYS_SRV_STATUS_NO_SRV,
                                SYS_SRV_DOMAIN_NO_SRV,
                                asubs_id);
  }

  if(ds3gstkproc_is_srv_domain_ps_supported(stack_info_ptr->srv_domain)
       == FALSE)
  {
    if( srv_status != SYS_SRV_STATUS_LIMITED )
    {
      /* If srv_domain is not supported and LTE attach is not in progress,
         report radio lost to DSD */
      if(ds3gdsdif_is_lte_attach_in_progress(asubs_id) == FALSE)
      {
        DATA_3GMGR_MSG1(MSG_LEGACY_LOW, "Report radio lost to DSD. sys_mode:%d",
                        SYS_SYS_MODE_LTE);
        ds3gdsdif_notify_dsd_lost_event(SYS_SYS_MODE_LTE, asubs_id);
      }
      else
      {
        DATA_3GMGR_MSG0(MSG_LEGACY_MED,"LTE attach in progress. Ignore");
      }
      return;
    }
    else
    {
      /* This is to handle simless emergency call scenarios. In these
         scenarios LTE will be limited service and srv_domain will be NO_SRVC */
      DATA_3GMGR_MSG1(MSG_LEGACY_MED, "LTE Limited service srv_domain:%d",
                      stack_info_ptr->srv_domain);
    }
  }

  if( (ps_data_suspend == TRUE) && (ds3gsubsmgr_get_device_mode() == 
      SYS_MODEM_DEVICE_MODE_DUAL_SIM_DUAL_STANDBY) )
  {
    other_subs_id = DS3GDSDIF_GET_OTHER_SUBS_INDX(asubs_id);
    voice_call_state = ds3gdsdif_get_voice_call_state(other_subs_id);
    if (voice_call_state.is_call_active == TRUE)
    {
      srv_status = SYS_SRV_STATUS_LIMITED;  
    }
  }

  /*-------------------------------------------------------------------------
    Check if radio acquired is different from current preferred radio
  -------------------------------------------------------------------------*/
  if(ds3gdsdif_get_preferred_sys(ds3gsubsmgr_subs_id_cm_to_ds3g(asubs_id))
       != SYS_SYS_MODE_LTE)
  {
    pref_radio_diff = TRUE;
  }

  if(force_dsd_proc || pref_radio_diff)
  {
    /* report radio acq to DSD */
    ds3gdsdif_notify_dsd_acq_lost_event(DS_DSD_CLIENT_RADIO_ACQUIRED_EV,
                                        asubs_id,
                                        SYS_SYS_MODE_LTE,
                                        DS_SYS_RAT_3GPP_NULL_BEARER,
                                        0,
                                        srv_status,
                                        stack_info_ptr->srv_domain);
  }
  else
  {
    /* check if srv_status has changed */
    if(srv_status != cached_stack_info_ptr->srv_status)
    {
      notify_radio_acq = TRUE;
    }

    /* check if srv_domain has changed */
    if(stack_info_ptr->srv_domain != cached_stack_info_ptr->srv_domain)
    {
      notify_radio_acq = TRUE;
    }

    /* check if ps_data_suspend flag has changed */
    if(ps_data_suspend != cached_ps_data_suspend)
    {
      notify_radio_acq = TRUE;
    }

    if(notify_radio_acq)
    {
      ds3gdsdif_notify_dsd_acq_lost_event(DS_DSD_CLIENT_RADIO_ACQUIRED_EV,
                                          asubs_id,
                                          SYS_SYS_MODE_LTE,
                                          DS_SYS_RAT_3GPP_NULL_BEARER,
                                          0,
                                          srv_status,
                                          stack_info_ptr->srv_domain);
    }
  }
  return;
}/* ds3gstkproc_process_lte_info() */

/*===========================================================================
FUNCTION      DS3GSTKPROC_PROCESS_CELL_SRV_IND

DESCRIPTION   This function parse cell service indication of CM's SS event.

PARAMETERS    asubs_id: subs id
              stack_info_ptr: stack info ptr

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds3gstkproc_process_cell_srv_ind
(
  sys_modem_as_id_e_type       asubs_id,
  ds3g_cm_ss_stack_info_type  *stack_info_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(stack_info_ptr == NULL)
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "NULL ptr passed");
    return;
  }

  DATA_3GMGR_MSG1(MSG_LEGACY_MED,
                  "ds3gstkproc_process_cell_srv_ind asubs_id:%d",
                  asubs_id);


  /*---------------------------------------------------------------------
      Report the cell info through seperate command as a payload. 
  ---------------------------------------------------------------------*/
  if(((stack_info_ptr->sys_mode == SYS_SYS_MODE_GSM) ||
      (stack_info_ptr->sys_mode == SYS_SYS_MODE_TDS) ||
      (stack_info_ptr->sys_mode == SYS_SYS_MODE_WCDMA)) &&
     (stack_info_ptr->srv_status == SYS_SRV_STATUS_SRV))
  {
    ds3gdsdif_notify_dsd_gwt_param_chg_event(
       stack_info_ptr->sys_mode, asubs_id, &(stack_info_ptr->cell_srv_ind));
  }


  return;
}/* ds3gstkproc_process_cell_srv_ind() */

#ifdef FEATURE_DATA_LTE
/*===========================================================================
FUNCTION      DS3GSTKPROC_PROCESS_LTE_DIV_DUPLEX_IND

DESCRIPTION   This function process division duplex indication of CM's SS event.
 
PARAMETERS    ss_event_info_ptr
 
DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds3gstkproc_process_lte_div_duplex_ind
(
  ds3g_ss_event_info_type  *ss_event_info_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(ss_event_info_ptr == NULL)
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "NULL ptr passed");
    return;
  }
  DATA_3GMGR_MSG0(MSG_LEGACY_MED, "ds3gstkproc_process_lte_div_duplex_ind");

  /*-----------------------------------------------------------------------
    Process div duplex ind change. Send this information to DSD 
  -----------------------------------------------------------------------*/
  ds3gdsdif_notify_dsd_lte_div_duplex_chg_event(ss_event_info_ptr->asubs_id,
                                                ss_event_info_ptr->div_duplex);
  return;
} /*ds3gstkproc_process_lte_div_duplex_ind() */
#endif /*FEATURE_DATA_LTE*/

/*===========================================================================
FUNCTION      DS3GSTKPROC_PROCESS_STACK_INFO

DESCRIPTION   This function parses the CM SS event stacks.

PARAMETERS    ss_event_info_ptr: SS event info pointer.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds3gstkproc_process_stack_info
(
  ds3g_ss_event_info_type  *ss_event_info_ptr
)
{
  uint8                        stk_idx;
  ds3g_ss_event_info_type     *cached_ss_event_info_ptr = NULL;
  ds3g_cm_ss_stack_info_type  *cached_stack_info_ptr = NULL;
  ds3g_cm_ss_stack_info_type  *new_stack_info_ptr = NULL;
  boolean                      force_dsd_proc =  FALSE;
  boolean                      new_radio_status_valid = FALSE;
  ds3gsubsmgr_subs_id_e_type   ds3g_subs_id = DS3GSUBSMGR_SUBS_ID_INVALID;
  sys_srv_status_e_type        hdr_intl_srv_status = SYS_SRV_STATUS_NO_SRV;
  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(ss_event_info_ptr == NULL)
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "Null ptr passed");
    return;
  }

  DATA_3GMGR_MSG0(MSG_LEGACY_MED, "ds3gstkproc_process_stack_info");

  ds3g_subs_id = ds3gsubsmgr_subs_id_cm_to_ds3g(ss_event_info_ptr->asubs_id);
  hdr_intl_srv_status = cm_get_hdr_intl_srv_status();
  
  cached_ss_event_info_ptr = ds3gstkproc_get_ss_event_info_instance(
                               ds3gsubsmgr_subs_id_cm_to_ds3g(
                                  ss_event_info_ptr->asubs_id));

  if(cached_ss_event_info_ptr == NULL)
  {
    return;
  }

  /*  to handle the possible case of number stacks change from 2--> 1 during DDS Switch */
  if ((cached_ss_event_info_ptr->number_of_stacks > ss_event_info_ptr->number_of_stacks) &&
      (cached_ss_event_info_ptr->stack_info[DS3G_SS_STACK_ID_2].is_operational))
  {
      DATA_3GMGR_MSG0(MSG_LEGACY_MED, "reporting secondary stack loss to DSD");
      /* report Radio loss on secondary stack */
     ds3gstkproc_check_stack_and_notify_lost(
      cached_ss_event_info_ptr->asubs_id,
      cached_ss_event_info_ptr->stack_info[DS3G_SS_STACK_ID_2].sys_mode,
      DS3G_SS_STACK_ID_2);   
  }

  /*-------------------------------------------------------------------------
    Process SS event stacks
  -------------------------------------------------------------------------*/
  for(stk_idx = 0; stk_idx < ss_event_info_ptr->number_of_stacks; stk_idx++)
  {
    if(stk_idx > DS3G_SS_STACK_MAX)
    {
      break;
    }

    /* Get new and cached stack info pointers */
    new_stack_info_ptr = &(ss_event_info_ptr->stack_info[stk_idx]);
    ds3gstkproc_get_stack_info_from_stack_id(
                  ds3gsubsmgr_subs_id_cm_to_ds3g(ss_event_info_ptr->asubs_id),
                  stk_idx, &cached_stack_info_ptr);
    
    if(cached_stack_info_ptr == NULL)
    {
      DATA_3GMGR_MSG0(MSG_LEGACY_LOW,"Null stack_info_ptr. return");
      return;
    }

    /* If stack not operational, send radio lost on cached radio */
    if(ss_event_info_ptr->stack_info[stk_idx].is_operational == FALSE)
    {
      DATA_3GMGR_MSG1(MSG_LEGACY_LOW, "stack_info[%d] not operational",
                      stk_idx);
      ds3gstkproc_check_stack_and_notify_lost(ss_event_info_ptr->asubs_id,
                                              cached_stack_info_ptr->sys_mode,
                                              stk_idx);
      continue;
    }

    /*-----------------------------------------------------------------------
      Stack operational - process stack contents
    -----------------------------------------------------------------------*/
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW, "stack_info[%d] operational", stk_idx);

    /*-----------------------------------------------------------------------
      Check if the radio has been changed on the stacks. If yes, report radio
      lost event on previous radio on the same stack.
    -----------------------------------------------------------------------*/

    /* This flag determines if DSD needs to process the stack info */
    force_dsd_proc =  FALSE;

    if(new_stack_info_ptr->sys_mode != cached_stack_info_ptr->sys_mode)
    {
      force_dsd_proc = TRUE;
      DATA_3GMGR_MSG1(MSG_LEGACY_LOW, "Report radio lost to DSD. sys_mode:%d",
                      cached_stack_info_ptr->sys_mode);
      ds3gstkproc_check_stack_and_notify_lost(
         ss_event_info_ptr->asubs_id,
         cached_stack_info_ptr->sys_mode,
         stk_idx);
    }

    /*-----------------------------------------------------------------------
      Check if the is_operational flag has been changed to TRUE. If yes,
      report radio acquired.
    -----------------------------------------------------------------------*/
    if(new_stack_info_ptr->is_operational != 
       cached_stack_info_ptr->is_operational)
    {
      force_dsd_proc = TRUE;
    }

    /*-----------------------------------------------------------------------
      Call individual radio processing based on sys_mode and srv_status
    -----------------------------------------------------------------------*/
    new_radio_status_valid = FALSE;

    switch(new_stack_info_ptr->sys_mode)
    {
      case SYS_SYS_MODE_CDMA:
        if(new_stack_info_ptr->srv_status == SYS_SRV_STATUS_SRV)
        {
          new_radio_status_valid = TRUE;
          ds3gstkproc_process_cdma_info(force_dsd_proc,
                                        ss_event_info_ptr->asubs_id,
                                        new_stack_info_ptr,
                                        cached_stack_info_ptr,
                                        stk_idx,
                                        ss_event_info_ptr->changed_fields_subs);
        }
        break;

      case SYS_SYS_MODE_HDR:
        if(new_stack_info_ptr->srv_status == SYS_SRV_STATUS_SRV)
        {
          new_radio_status_valid = TRUE;

#ifdef FEATURE_DATA_LTE

          if ((ds3gdsdif_get_preferred_sys(ds3g_subs_id) == SYS_SYS_MODE_LTE)&&
              (hdr_intl_srv_status != SYS_SRV_STATUS_SRV) )
          {
            DATA_3GMGR_MSG0(MSG_LEGACY_ERROR,"Ignore HDR Stack Info");
          }
          else
#endif /* FEATURE_DATA_LTE */
          {
          ds3gstkproc_process_hdr_info(force_dsd_proc,
                                       ss_event_info_ptr->asubs_id,
                                       new_stack_info_ptr,
                                       cached_stack_info_ptr,
                                       stk_idx);
          }
        }
        break;

      case SYS_SYS_MODE_GSM:
        if((new_stack_info_ptr->srv_status == SYS_SRV_STATUS_LIMITED) ||
           (new_stack_info_ptr->srv_status == SYS_SRV_STATUS_SRV))
        {
          new_radio_status_valid = TRUE;
          ds3gstkproc_process_gsm_info(force_dsd_proc,
                                       ss_event_info_ptr->asubs_id,
                                       new_stack_info_ptr,
                                       cached_stack_info_ptr,
                                       ss_event_info_ptr->ps_data_suspend,
                                       cached_ss_event_info_ptr->ps_data_suspend,
                                       stk_idx);
        }
        break;

      case SYS_SYS_MODE_WCDMA:
      case SYS_SYS_MODE_TDS:
        if((new_stack_info_ptr->srv_status == SYS_SRV_STATUS_LIMITED) ||
           (new_stack_info_ptr->srv_status == SYS_SRV_STATUS_SRV))
        {
          new_radio_status_valid = TRUE;
          ds3gstkproc_process_wcdma_tds_info(force_dsd_proc,
                                             ss_event_info_ptr->asubs_id,
                                             new_stack_info_ptr,
                                             cached_stack_info_ptr,
                                             ss_event_info_ptr->ps_data_suspend,
                                             cached_ss_event_info_ptr->ps_data_suspend,
                                             stk_idx);
        }
        break;

      case SYS_SYS_MODE_LTE:
        if((new_stack_info_ptr->srv_status == SYS_SRV_STATUS_LIMITED) ||
           (new_stack_info_ptr->srv_status == SYS_SRV_STATUS_SRV))
        {
          new_radio_status_valid = TRUE;
          ds3gstkproc_process_lte_info(force_dsd_proc,
                                       ss_event_info_ptr->asubs_id,
                                       new_stack_info_ptr,
                                       cached_stack_info_ptr,
                                       ss_event_info_ptr->ps_data_suspend,
                                       cached_ss_event_info_ptr->ps_data_suspend,
                                       stk_idx);
        }
        break;

      default:
        ds3gstkproc_check_stack_and_notify_lost(
           cached_ss_event_info_ptr->asubs_id,
           cached_stack_info_ptr->sys_mode,
           stk_idx);
        break;
    }

    if(new_radio_status_valid == FALSE)
    {
      ds3gstkproc_check_stack_and_notify_lost(ss_event_info_ptr->asubs_id,
                                              new_stack_info_ptr->sys_mode,
                                              stk_idx);
    }

    /*-----------------------------------------------------------------------
      Process Cell service indication
    -----------------------------------------------------------------------*/
    ds3gstkproc_process_cell_srv_ind(
       ss_event_info_ptr->asubs_id, new_stack_info_ptr);

    /*-----------------------------------------------------------------------
      Process LTE Div duplex only when LTE is in FULL Service
    -----------------------------------------------------------------------*/
#ifdef FEATURE_DATA_LTE
    if((ss_event_info_ptr->stack_info[stk_idx].sys_mode == SYS_SYS_MODE_LTE)&&
       (ss_event_info_ptr->stack_info[stk_idx].srv_status == SYS_SRV_STATUS_SRV))
    {
      ds3gstkproc_process_lte_div_duplex_ind(ss_event_info_ptr);
    }
#endif /*FEATURE_DATA_LTE*/

    ds3gstkproc_set_stack_id_of_radio(ss_event_info_ptr->asubs_id,
                                      new_stack_info_ptr->sys_mode,
                                      stk_idx);
  }

  return;
}/* ds3gstkproc_process_stack_info() */

/*===========================================================================
FUNCTION      DS3GSTKPROC_CHANGE_BSR_STATE

DESCRIPTION   This function changes BSR state.

PARAMETERS    bsr_in_progress flag from SS event.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds3gstkproc_change_bsr_state
(
  ds3gsubsmgr_subs_id_e_type  subs_id,
  boolean                     bsr_in_progress
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  DATA_3GMGR_MSG2(MSG_LEGACY_MED,
                  "ds3gstkproc_change_bsr_state:%d for subs_id:%d",
                  bsr_in_progress, subs_id);

  /*---------------------------------------------------------------------------
    If BSR flag is being set (i.e. FALSE -> TRUE) or reset (i.e. TRUE -> FALSE)
     move the state variable accordingly.
  ---------------------------------------------------------------------------*/
  if(bsr_in_progress == TRUE)
  {
    switch(ds3g_bsr_state[subs_id])
    {
      case DS3G_STKPROC_BSR_RESET:
        ds3gstkproc_set_bsr_state(subs_id, DS3G_STKPROC_BSR_SET);
        break;

      case DS3G_STKPROC_BSR_RESET_PENDING:
        ds3gstkproc_set_bsr_state(subs_id, DS3G_STKPROC_BSR_SET);
        break;

      case DS3G_STKPROC_BSR_SET:
        ds3gstkproc_set_bsr_state(subs_id, DS3G_STKPROC_BSR_SET);
        break;

      default:
        DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                        "BSR state variable in invalid state:%d",
                        ds3g_bsr_state[subs_id]);
        break;
    }
  }
  else
  {
    switch(ds3g_bsr_state[subs_id])
    {
      case DS3G_STKPROC_BSR_RESET:
        ds3gstkproc_set_bsr_state(subs_id, DS3G_STKPROC_BSR_RESET);
        break;

      case DS3G_STKPROC_BSR_RESET_PENDING:
        ds3gstkproc_set_bsr_state(subs_id, DS3G_STKPROC_BSR_RESET_PENDING);
        break;

      case DS3G_STKPROC_BSR_SET:
        ds3gstkproc_set_bsr_state(subs_id, DS3G_STKPROC_BSR_RESET_PENDING);
        break;

      default:
        DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                        "BSR state variable in invalid state:%d",
                        ds3g_bsr_state[subs_id]);
        break;
    }
  }/* BSR flag is un-set */
 
  return;
}/* ds3gstkproc_change_bsr_state() */

/*===========================================================================

                    EXTERNAL FUNCTIONS

===========================================================================*/

/*===========================================================================
FUNCTION      DS3GSTKPROC_INIT

DESCRIPTION   This function initializes ds3g stack processor.

PARAMETERS    None.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds3gstkproc_init(void)
{
  ds3gsubsmgr_subs_id_e_type  subs_indx;
  ds3gsubsmgr_client_type     client_id;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  DATA_MSG0(MSG_LEGACY_LOW, "ds3gstkproc_init called");

  ds3g_dsd_notified_by_ss_event = FALSE;

  /*-------------------------------------------------------------------------
   Initialize SS event info and radio to stack table
  -------------------------------------------------------------------------*/
  for(subs_indx = DS3GSUBSMGR_SUBS_ID_MIN;
      subs_indx < DS3GSUBSMGR_SUBS_ID_COUNT;
      subs_indx++)
  {
    ds3g_ss_event_info[subs_indx] = NULL;
    ds3g_radio_to_stack_table[subs_indx] = NULL;
    ds3g_bsr_state[subs_indx] = DS3G_STKPROC_BSR_RESET;
  }

  /*-------------------------------------------------------------------------
    Register for subsmgr events
  -------------------------------------------------------------------------*/
  ds3gsubsmgr_event_reg(DS3GSUBSMGR_DEVICE_MODE_EV,
                        ds3gstkproc_subsmgr_event_cb,
                        NULL,
                        &client_id);

  return;
}/* ds3gstkproc_init() */

#ifdef TEST_FRAMEWORK
#error code not present
#endif

/*===========================================================================
FUNCTION      DS3GSTKPROC_IGNORE_SRV_DOMAIN_CAMPED

DESCRIPTION   This function checks whether camped indication can be ignored.

PARAMETERS    srv_domain: service domain

DEPENDENCIES  None.

RETURN VALUE  TRUE:  If srv domain camped can be ignored
              FALSE: If srv domain camped can not be ignored

SIDE EFFECTS  None.
===========================================================================*/
boolean ds3gstkproc_ignore_srv_domain_camped
(
  ds3g_cm_ss_stack_info_type  *stack_info_ptr,
  ds3g_cm_ss_stack_info_type  *cached_stack_info_ptr
)
{
  boolean ret_val = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ((stack_info_ptr->sys_mode == cached_stack_info_ptr->sys_mode) &&
      (stack_info_ptr->srv_domain == SYS_SRV_DOMAIN_CAMPED) &&
     ((cached_stack_info_ptr->srv_domain == SYS_SRV_DOMAIN_CS_PS) ||
      (cached_stack_info_ptr->srv_domain == SYS_SRV_DOMAIN_PS_ONLY) ||
      (cached_stack_info_ptr->srv_domain == SYS_SRV_DOMAIN_CS_ONLY)))
  {
    ret_val = TRUE;

    DATA_MSG1(MSG_LEGACY_MED,
             "ds3gstkproc_ignore_srv_domain_camped: %d ", ret_val);
  }

  return ret_val;
}/* ds3gstkproc_ignore_srv_domain_camped() */

/*===========================================================================
FUNCTION      DS3GSTKPROC_IS_SRV_DOMAIN_PS_SUPPORTED

DESCRIPTION   This utility function checks if srv_domain is PS supported.

PARAMETERS    srv_domain: service domain

DEPENDENCIES  None.

RETURN VALUE  TRUE:  If the srv domain is PS supported
              FALSE: If the srv domain is not PS supported

SIDE EFFECTS  None.
===========================================================================*/
boolean ds3gstkproc_is_srv_domain_ps_supported
(
  sys_srv_domain_e_type srv_domain
)
{
  boolean ret_val = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_MSG1(MSG_LEGACY_LOW,
            "ds3gstkproc_is_srv_domain_ps_supported srv_domain:%d",
            srv_domain);

  switch(srv_domain)
  {
    case SYS_SRV_DOMAIN_PS_ONLY:
    case SYS_SRV_DOMAIN_CS_PS:
      ret_val = TRUE;
      break;

    default:
      break;
  }
  return ret_val;
}/* ds3gstkproc_is_srv_domain_ps_supported() */

/*===========================================================================
FUNCTION      DS3GSTKPROC_IS_SRV_DOMAIN_CS_SUPPORTED

DESCRIPTION   This utility function checks if srv_domain is PS supported.

PARAMETERS    srv_domain: service domain

DEPENDENCIES  None.

RETURN VALUE  TRUE:  If the srv domain is CS supported
              FALSE: If the srv domain is not CS supported

SIDE EFFECTS  None.
===========================================================================*/
boolean ds3gstkproc_is_srv_domain_cs_supported
(
  sys_srv_domain_e_type srv_domain
)
{
  boolean ret_val = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_MSG1(MSG_LEGACY_LOW,
            "ds3gstkproc_is_srv_domain_cs_supported srv_domain:%d",srv_domain);

  switch(srv_domain)
  {
    case SYS_SRV_DOMAIN_CS_ONLY:
    case SYS_SRV_DOMAIN_CS_PS:
      ret_val = TRUE;
      break;

    default:
      break;
  }
  return ret_val;
}/* ds3gstkproc_is_srv_domain_cs_supported() */

/*===========================================================================
FUNCTION      DS3GSTKPROC_CONVERT_SYS_MODE_TO_RADIO_INDEX

DESCRIPTION   This is a utility to translate sys mode to DS3G radio index.

PARAMETERS    sys_mode: sys_mode
 
DEPENDENCIES  None.

RETURN VALUE  ds3g radio index.

SIDE EFFECTS  None.
===========================================================================*/
ds3g_radio_index_type ds3gstkproc_convert_sys_mode_to_radio_index
(
  sys_sys_mode_e_type sys_mode
)
{
  ds3g_radio_index_type index = DS3G_RADIO_INDEX_MAX;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  switch(sys_mode)
  {
    case SYS_SYS_MODE_LTE:
      index = DS3G_RADIO_INDEX_LTE;
      break;

    case SYS_SYS_MODE_HDR:
      index = DS3G_RADIO_INDEX_HDR;
      break;

    case SYS_SYS_MODE_CDMA:
      index = DS3G_RADIO_INDEX_1X;
      break;

    case SYS_SYS_MODE_GSM:
      index = DS3G_RADIO_INDEX_GSM;
      break;

    case SYS_SYS_MODE_TDS:
      index = DS3G_RADIO_INDEX_TDS;
      break;

    case SYS_SYS_MODE_WCDMA:
      index = DS3G_RADIO_INDEX_WCDMA;
      break;

    default:
      index = DS3G_RADIO_INDEX_MAX;
      break;
  }
  return index;
}/* ds3gstkproc_convert_sys_mode_to_radio_index()*/

/*===========================================================================
FUNCTION      DS3GSTKPROC_CONVERT_RADIO_INDEX_TO_SYS_MODE

DESCRIPTION   This is a utility to translate DS3G radio index to sys mode.

PARAMETERS    index: ds3g radio index

DEPENDENCIES  None.

RETURN VALUE  sys_mode

SIDE EFFECTS  None.
===========================================================================*/
sys_sys_mode_e_type ds3gstkproc_convert_radio_index_to_sys_mode
(
  ds3g_radio_index_type index
)
{
  sys_sys_mode_e_type translated_sys = SYS_SYS_MODE_NONE;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  switch(index)
  {
    case DS3G_RADIO_INDEX_LTE:
      translated_sys = SYS_SYS_MODE_LTE;
      break;

    case DS3G_RADIO_INDEX_HDR:
      translated_sys = SYS_SYS_MODE_HDR;
      break;

    case DS3G_RADIO_INDEX_1X:
      translated_sys = SYS_SYS_MODE_CDMA;
      break;

    case DS3G_RADIO_INDEX_GSM:
      translated_sys = SYS_SYS_MODE_GSM;
      break;

    case DS3G_RADIO_INDEX_TDS:
      translated_sys = SYS_SYS_MODE_TDS;
      break;

    case DS3G_RADIO_INDEX_WCDMA:
      translated_sys = SYS_SYS_MODE_WCDMA;
      break;

    default:
      break;
  }
  return translated_sys;
}/* ds3gstkproc_convert_radio_index_to_sys_mode()*/

/*===========================================================================
FUNCTION      DS3GSTKPROC_GET_STACK_ID_FROM_RADIO

DESCRIPTION   returns the stack in which sys_mode was present 
 
PARAMETERS    sys_mode: system mode

DEPENDENCIES  None.

RETURN VALUE  stack id.
              
SIDE EFFECTS  None.
===========================================================================*/
uint8 ds3gstkproc_get_stack_id_from_radio
(
  sys_modem_as_id_e_type  asubs_id,
  sys_sys_mode_e_type     sys_mode
)
{
  uint8                            stack_id = CM_NO_STACKS;
  ds3g_radio_index_type            indx;
  ds3g_radio_to_stack_table_type  *radio_to_stack_table_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if((sys_mode < SYS_SYS_MODE_NO_SRV) || (sys_mode >= SYS_SYS_MODE_MAX))
  {
    DATA_MSG1(MSG_LEGACY_LOW, "Invalid sys_mode: %d", sys_mode);
    return stack_id;
  }

  if(!ds3gsubsmgr_is_subs_id_valid(asubs_id))
  {
    return stack_id;
  }

  radio_to_stack_table_ptr = ds3gstkproc_get_radio_to_stack_table_instance(
                                ds3gsubsmgr_subs_id_cm_to_ds3g(asubs_id));

  if(radio_to_stack_table_ptr != NULL)
  {
    for(indx = DS3G_RADIO_INDEX_MIN; indx < DS3G_RADIO_INDEX_MAX; indx++)
    {
      if(sys_mode == radio_to_stack_table_ptr->radio_info[indx].sys_mode)
      {
        stack_id = radio_to_stack_table_ptr->radio_info[indx].stack_id;
      }
    }
  }

  DATA_MSG2(MSG_LEGACY_MED,
            "ds3gstkproc_get_stack_id_from_radio:sys_mode:%d, stack:%d",
            sys_mode, stack_id);
  return stack_id;
}/* ds3gstkproc_get_stack_id_from_radio() */

/*===========================================================================
FUNCTION      DS3GSTKPROC_IS_DSD_NOTIFIED_BY_SS_EVENT

DESCRIPTION   This function returns the flag DSD has been notified by current 
              SS event.
  
PARAMETERS    None.

DEPENDENCIES  None.

RETURN VALUE  True:  DSD notified by ss_event.
              False: DSD not notified by ss_event.

SIDE EFFECTS  None.
===========================================================================*/
boolean ds3gstkproc_is_dsd_notified_by_ss_event(void)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return ds3g_dsd_notified_by_ss_event;
}/* ds3gstkproc_is_dsd_notified_by_ss_event() */

/*===========================================================================
FUNCTION      DS3GSTKPROC_SET_DSD_NOTIFIED_SS_EVENT

DESCRIPTION   This function sets the flag DSD notified by current SS event.
  
PARAMETERS    DSD notified by ss_event.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds3gstkproc_set_dsd_notified_by_ss_event
(
  boolean dsd_notified_by_ss_event
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ds3g_dsd_notified_by_ss_event = dsd_notified_by_ss_event;
  return;
}/* ds3gstkproc_set_dsd_notified_by_ss_event() */

/*===========================================================================
FUNCTION      DS3GSTKPROC_GET_BSR_STATE

DESCRIPTION   This function returns the BSR state flag.

PARAMETERS    None.

DEPENDENCIES  None.

RETURN VALUE  bsr state.

SIDE EFFECTS  None.
===========================================================================*/
ds3g_stkproc_bsr_state_type ds3gstkproc_get_bsr_state
(
  ds3gsubsmgr_subs_id_e_type  subs_id
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if( !ds3gsubsmgr_is_ds3g_subs_index_valid(subs_id) )
  {
    return DS3G_STKPROC_BSR_RESET;
  }

  DATA_3GMGR_MSG2(MSG_LEGACY_LOW,
                  "ds3gstkproc_get_bsr_state:%d for subs_id:%d",
                  ds3g_bsr_state[subs_id], subs_id);

  return ds3g_bsr_state[subs_id];
}/* ds3gstkproc_get_bsr_state() */

/*===========================================================================
FUNCTION      DS3GSTKPROC_SET_BSR_STATE

DESCRIPTION   This function sets the BSR state flag.

PARAMETERS    None.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds3gstkproc_set_bsr_state
(
  ds3gsubsmgr_subs_id_e_type   subs_id,
  ds3g_stkproc_bsr_state_type  bsr_state
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if((subs_id >= DS3GSUBSMGR_SUBS_ID_MIN) ||
     (subs_id < DS3GSUBSMGR_SUBS_ID_COUNT))
  {
    ds3g_bsr_state[subs_id] = bsr_state;
    DATA_3GMGR_MSG3(MSG_LEGACY_MED,
                    "BSR state transition: %d -> %d for subs_id:%d",
                    ds3g_bsr_state[subs_id], bsr_state, subs_id);
  }
  return;
}/* ds3gstkproc_set_bsr_state() */

/*===========================================================================
FUNCTION      DS3GSTKPROC_GET_SS_EVENT_INFO_INSTANCE

DESCRIPTION   This utility function will return a pointer to the ss event info
              for the passed sub id 

PARAMETERS    subs_id: ds3g data sub id.

DEPENDENCIES  None.

RETURN VALUE  pointer to ds3g_ss_event_info[subs_id] 
              NULL pointer if subs_id is invalid 

SIDE EFFECTS  None.
===========================================================================*/
ds3g_ss_event_info_type* ds3gstkproc_get_ss_event_info_instance
(
  ds3gsubsmgr_subs_id_e_type  subs_id
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( !ds3gsubsmgr_is_ds3g_subs_index_valid(subs_id) )
  {
    return NULL;
  }

  return ds3g_ss_event_info[subs_id];
}/* ds3gstkproc_get_ss_event_info_instance() */

/*===========================================================================
FUNCTION      DS3GSTKPROC_GET_STACK_INFO_FROM_STACK_ID

DESCRIPTION   This utility function returns pointer of cached stack_info for 
              the specified stack_id.

PARAMETERS    subs_id:  subs_id
              stack_id: CM stack id 
              stack_info_ptr: cached stack info pointer
 
DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds3gstkproc_get_stack_info_from_stack_id
(
  ds3gsubsmgr_subs_id_e_type    subs_id,
  uint8                         stack_id,
  ds3g_cm_ss_stack_info_type  **stack_info_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(stack_info_ptr == NULL)
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "NULL ptr passed");
    return;
  }

  if(stack_id > DS3G_SS_STACK_MAX)
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                    "stack_id:%d > maximum number of stacks", stack_id);
    return;
  }

  *stack_info_ptr = NULL;
  DATA_3GMGR_MSG2(MSG_LEGACY_MED,
                  "ds3gstkproc_get_stack_info_from_stack_id subs_id:%d "
                  "stack:%d", subs_id, stack_id);

  if(ds3g_ss_event_info[subs_id] != NULL)
  {
    *stack_info_ptr = &(ds3g_ss_event_info[subs_id]->stack_info[stack_id]);
  }
  return;
} /* ds3gstkproc_get_stack_info_from_stack_id() */

/*===========================================================================
FUNCTION      DS3GSTKPROC_PROCESS_CACHED_STACK_INFO

DESCRIPTION   This function parses the cached SS event stacks.

PARAMETERS    None.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds3gstkproc_process_cached_stack_info
(
  sys_modem_as_id_e_type  asubs_id
)
{
  ds3g_ss_event_info_type  *ss_event_info_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ss_event_info_ptr = ds3gstkproc_get_ss_event_info_instance(
                        ds3gsubsmgr_subs_id_cm_to_ds3g(asubs_id));

  if(ss_event_info_ptr != NULL)
  {
    ds3gstkproc_process_stack_info(ss_event_info_ptr);
  }
  return;
}/* ds3gstkproc_process_cached_stack_info() */

/*===========================================================================
FUNCTION      DS3GSTKPROC_PROCESS_CM_SS_SRV_CHG_CMD

DESCRIPTION   This function process the SS event information.

PARAMETERS    ss_event_info_p: SS event info ptr.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void  ds3gstkproc_process_cm_ss_srv_chg_cmd
(
  ds3g_ss_event_info_type  *ss_event_info_ptr
)
{
  uint8                       stack_idx;
  ds3g_ss_event_info_type    *cached_ss_event_info_ptr = NULL;
  ds3gsubsmgr_subs_id_e_type  ds3g_subs_id;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(ss_event_info_ptr == NULL)
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "NULL ptr passed");
    return;
  }

  ds3g_subs_id = ds3gsubsmgr_subs_id_cm_to_ds3g(ss_event_info_ptr->asubs_id);

  if(!ds3gsubsmgr_is_ds3g_subs_index_valid(ds3g_subs_id))
  {
    return;
  }

  DATA_3GMGR_MSG3(MSG_LEGACY_HIGH,
                  "ds3gstkproc_process_cm_ss_srv_chg_cmd: asubs_id:%d "
                  "num_stacks:%d changed_fields_subs:0x%x",
                  ss_event_info_ptr->asubs_id,
                  ss_event_info_ptr->number_of_stacks,
                  ss_event_info_ptr->changed_fields_subs);

  for(stack_idx=0; stack_idx<ss_event_info_ptr->number_of_stacks; stack_idx++)
  {
    if(stack_idx > DS3G_SS_STACK_MAX)
    {
      break;
    }
    DATA_3GMGR_MSG2(MSG_LEGACY_HIGH,
                  "ds3gstkproc_process_cm_ss_srv_chg_cmd: stack[%d]: "
                  "changed_fields:%d changed_fields2:%d",
                  ss_event_info_ptr->stack_info[stack_idx].changed_fields,
                  ss_event_info_ptr->stack_info[stack_idx].changed_fields2);
  }

  /*-------------------------------------------------------------------------
    If BSR mask is set, update the BSR flag
  -------------------------------------------------------------------------*/
  if((ss_event_info_ptr->changed_fields_subs & CM_SS_EVT_BSR_PROGRESS_MASK) != 0)
  {
    ds3gstkproc_change_bsr_state(ds3g_subs_id,
                                 ss_event_info_ptr->bsr_in_progress);
  }

  /*-------------------------------------------------------------------------
    Reset DSD notified by SS event before processing SS event
  -------------------------------------------------------------------------*/
  ds3gstkproc_set_dsd_notified_by_ss_event(FALSE);

  /*-------------------------------------------------------------------------
    Process SS event stack info
  -------------------------------------------------------------------------*/
  ds3gstkproc_process_stack_info(ss_event_info_ptr);

  /*-------------------------------------------------------------------------
    Cache SS event info
  -------------------------------------------------------------------------*/
  cached_ss_event_info_ptr
    = ds3gstkproc_get_ss_event_info_instance(ds3g_subs_id);

  if(cached_ss_event_info_ptr)
  {
    memscpy(cached_ss_event_info_ptr,
            sizeof(ds3g_ss_event_info_type),
            ss_event_info_ptr,
            sizeof(ds3g_ss_event_info_type));
  }

  if(ds3gstkproc_is_dsd_notified_by_ss_event() == FALSE)
  {
    /*-------------------------------------------------------------------------
      Handle BSR flag if DSD not notified by SS event.
      Else, it will be handled during SS event processing complete hdlr.
    -------------------------------------------------------------------------*/
    /* Check if any notification is pending */
    if(DS3G_STKPROC_BSR_RESET_PENDING ==
       ds3gstkproc_get_bsr_state(ds3g_subs_id))
    {
      ds3gstkproc_set_bsr_state(ds3g_subs_id, DS3G_STKPROC_BSR_RESET);

      /* notify mode handlers */
      ds3g_dispatch_bsr_reset_handler(
         ds3gsubsmgr_subs_id_ds3g_to_cm(ds3g_subs_id));
    }

    /*-------------------------------------------------------------------------
      Handle PS Suspend flag if DSD not notified by SS event.
      Else, it will be handled during SS event processing  complete hdlr.
      Note: SS event info already cached.
    -------------------------------------------------------------------------*/
    ds3gdsdif_process_ps_suspend(ds3g_subs_id);
  }

  return;
}/* ds3gstkproc_process_cm_ss_srv_chg_cmd() */

