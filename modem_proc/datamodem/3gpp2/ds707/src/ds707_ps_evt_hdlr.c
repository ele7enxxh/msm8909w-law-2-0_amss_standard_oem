/** 
  @file ds707_ps_evt_hdlr.c 
  @brief 
   Handles PS events
*/
/** Contains API's to handle PS events */

/*===========================================================================

                      D S 7 0 7 _ P S _ E V T _ H D L R
GENERAL DESCRIPTION
  This file contains the fuctions to handle ps events

EXTERNALIZED FUNCTIONS

INTERNAL FUNCTIONS 

 Copyright (c) 2012-2014 by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/src/ds707_ps_evt_hdlr.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $
  
when       who     what, where, why
--------   ---     -------------------------------------------------------
01/06/14   vm      Changes to use correct PCMT value during IRAT success and
                   failure
01/03/12   jee     Created Module
===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "datamodem_variation.h"
#include "amssassert.h"
#include "data_msg.h"
#include "ps_sys.h"
#include "ps_sys_event.h"
#include "dstask_v.h"
#include "modem_mem.h"
#include "ds707_devmgr.h"

/*===========================================================================
                LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE
===========================================================================*/

/*===========================================================================
                      FUNCTION DEFINITIONS
===========================================================================*/
#ifdef FEATURE_EHRPD


/*---------------------------------------------------------------------------
  The enum to indicate the type of LTE event received from PS
---------------------------------------------------------------------------*/
typedef enum
{
  DS707_PS_EVT_LTE_ATTACH_INIT,
  DS707_PS_EVT_LTE_ATTACH_COMPLETE,
  DS707_PS_EVT_MAX
} ds707_ps_lte_evt_type;

/*---------------------------------------------------------------------------
  Info to be passed during task switch. To extend, include the required 
  structures in an union. Event info should be passed.
---------------------------------------------------------------------------*/
typedef struct
{
  ps_sys_event_3gpp_lte_attach_complete_type  iface_info;
  ds707_ps_lte_evt_type                       event;
} ds707_ps_evt_hdlr_info;

/*===========================================================================
FUNCTION      DS707_PS_EVT_HDLR_DS_CMD_PROCESS

DESCRIPTION   Process the command in DS context

PARAMETERS    cmd_ptr   - Ptr to the command information

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_ps_evt_hdlr_ds_cmd_process 
(
  const ds_cmd_type *cmd_ptr                         /* Pointer to Command */
)
{
  ds707_ps_evt_hdlr_info  *ps_info = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if (NULL == cmd_ptr)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "cmd_ptr is NULL");
    return;
  }

  /*-----------------------------------------------------------------------
    cmd_id should be DS_CMD_707_PS_EVT_PROC
  -----------------------------------------------------------------------*/
  if (DS_CMD_707_PS_EVT_PROC != cmd_ptr->hdr.cmd_id)
  {
    DATA_IS707_MSG1(MSG_LEGACY_ERROR, 
                    "Invalid cmd %d passed to ds707_ds_cmd_process",
                    cmd_ptr->hdr.cmd_id);
    return;
  }

  /*-----------------------------------------------------------------------
    Keeping iface_info in user data for future use, currently not used
  -----------------------------------------------------------------------*/
  ps_info = (ds707_ps_evt_hdlr_info*)cmd_ptr->cmd_payload_ptr;
  if (NULL == ps_info)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, 
                    "NULL user data in DS_CMD_707_PS_EVT_PROC");
    return;
  }

  DATA_IS707_MSG1(MSG_LEGACY_HIGH, "Handle LTE event %d", ps_info->event);

  switch (ps_info->event)
  {
    case DS707_PS_EVT_LTE_ATTACH_INIT:
    {
      DATA_IS707_MSG0(MSG_LEGACY_HIGH, "LTE attach init received");

      /*-----------------------------------------------------------------------
        Send LTE attach init indication to Device manager to set PCMT value.
      -----------------------------------------------------------------------*/
      ds707_devmgr_lte_attach_init_ind();
      break;
    }

    case DS707_PS_EVT_LTE_ATTACH_COMPLETE:
    {
      /*--------------------------------------------------------------------- 
        For eHRPD to HRPD fallback feature, for LTE attach
        success, DS informs HDR to propose eHRPD next time when
        UE camps DO later.
      ---------------------------------------------------------------------*/
#ifdef FEATURE_EHRPD_HRPD_FALLBACK
      ds707_pkt_mgr_lte_attach_success_ind();
#endif /* FEATURE_EHRPD_HRPD_FALLBACK */

      break;
    }

    default:
    {
      break;
    }
  }

  return;
} /* ds707_ps_evt_hdlr_ds_cmd_process() */

/*===========================================================================
FUNCTION      DS707_PS_EVT_HDLR_LTE_ATTACH_COMP_EV_HDLR

DESCRIPTION   Handle PS_SYS_LTE_ATTACH_COMPLETE_EV from PS task. Post
              DS_CMD_707_PS_EVT_PROC to DS task. It is used to switch PS 
              event context from PS task to DS task.

DEPENDENCIES  None.

RETURN VALUE

SIDE EFFECTS  None.
===========================================================================*/
void ds707_ps_evt_hdlr_lte_attach_comp_ev_hdlr
(
  ps_iface_type         *v4_iface_ptr,
  ps_iface_type         *v6_iface_ptr,
  ds707_ps_lte_evt_type  lte_event
)
{
  ds_cmd_type            *cmd_ptr = NULL;
  ds707_ps_evt_hdlr_info *ds707_ps_evt_hdlr_info_p = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  cmd_ptr = ds_allocate_cmd_buf(sizeof(ds707_ps_evt_hdlr_info));
  if( (NULL == cmd_ptr) || (NULL == cmd_ptr->cmd_payload_ptr) )
  {
    ASSERT(0);
    return;
  }
  ds707_ps_evt_hdlr_info_p = (ds707_ps_evt_hdlr_info* ) cmd_ptr->cmd_payload_ptr;
  
  cmd_ptr->hdr.cmd_id = DS_CMD_707_PS_EVT_PROC;
  ds707_ps_evt_hdlr_info_p->event = lte_event;
  ds707_ps_evt_hdlr_info_p->iface_info.v4_iface_ptr = v4_iface_ptr;
  ds707_ps_evt_hdlr_info_p->iface_info.v6_iface_ptr = v6_iface_ptr;
  
  ds_put_cmd(cmd_ptr);

  return;
} /* ds707_ps_evt_hdlr_lte_attach_comp_ev_hdlr() */

/*===========================================================================
FUNCTION      DS707_PS_EVT_HDLR_CB

DESCRIPTION   Callback function for ps sys event

DEPENDENCIES  None.

RETURN VALUE

SIDE EFFECTS  None.
===========================================================================*/
void ds707_ps_evt_hdlr_cb
(
  ps_sys_tech_enum_type     tech_type,
  ps_sys_event_enum_type    event_name,
  void                     *event_info_ptr,
  void                     *user_data_ptr
)
{
  ps_sys_event_3gpp_lte_attach_complete_type *lte_attached_info = NULL;
  ds_cmd_type                                *cmd_ptr = NULL;
  ds707_ps_evt_hdlr_info                     *ds707_ps_evt_hdlr_info_p = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  DATA_IS707_MSG2(MSG_LEGACY_HIGH, 
              "ds707_sys_ps_sys_event_cb() received PS event %d, tech_type %d",
              event_name, tech_type);

  switch (event_name)
  {
    case PS_SYS_EVENT_3GPP_LTE_ATTACH_INIT:
    {
      DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                      "PS sys event for LTE attach init received");

      cmd_ptr = ds_allocate_cmd_buf(sizeof(ds707_ps_evt_hdlr_info));
      if( (NULL == cmd_ptr) || (NULL == cmd_ptr->cmd_payload_ptr) )
      {
        ASSERT(0);
        return;
      }
      ds707_ps_evt_hdlr_info_p = (ds707_ps_evt_hdlr_info* ) cmd_ptr->cmd_payload_ptr;
  
      cmd_ptr->hdr.cmd_id = DS_CMD_707_PS_EVT_PROC;
      ds707_ps_evt_hdlr_info_p->event = DS707_PS_EVT_LTE_ATTACH_INIT;
      ds707_ps_evt_hdlr_info_p->iface_info.v4_iface_ptr = NULL;;
      ds707_ps_evt_hdlr_info_p->iface_info.v6_iface_ptr = NULL;
  
      ds_put_cmd(cmd_ptr);

      break;
    }

    case PS_SYS_EVENT_3GPP_LTE_ATTACH_COMPLETE:
    {
      lte_attached_info = (ps_sys_event_3gpp_lte_attach_complete_type*)
                                                              (event_info_ptr); 
      if (NULL == lte_attached_info)
      {
        DATA_IS707_MSG0(MSG_LEGACY_ERROR, "NULL event info passed");
        return;
      }

      ds707_ps_evt_hdlr_lte_attach_comp_ev_hdlr(lte_attached_info->v4_iface_ptr,
                                              lte_attached_info->v6_iface_ptr,
                                              DS707_PS_EVT_LTE_ATTACH_COMPLETE);
      break;
    }

    default:
    {
      break;
    }
  }

  return;
}/*ds707_ps_evt_hdlr_cb*/

/*===========================================================================
FUNCTION      DS707_PS_EVT_HDLR_REG_EVENT

DESCRIPTION   Register ps sys event

DEPENDENCIES  None.

RETURN VALUE

SIDE EFFECTS  None.
===========================================================================*/
void ds707_ps_evt_hdlr_reg_event
(
  ps_sys_event_enum_type     event_name,
  ps_sys_tech_enum_type      tech_type,
  void                      *user_data_ptr
)
{
  int16                                      ps_errno;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /* Register for PS SYS event*/
  if(0 != ps_sys_event_reg(tech_type, 
                           event_name, 
                           ds707_ps_evt_hdlr_cb,
                           user_data_ptr, 
                           &ps_errno))
  {
    DATA_IS707_MSG1(MSG_LEGACY_HIGH, 
                    "Could not reg PS SYS event %d", event_name);
    ASSERT(0);
  }
  
  return;
} /* ds707_ps_evt_hdlr_reg_event */

/*===========================================================================
FUNCTION      DS707_PS_EVT_HDLR_INIT

DESCRIPTION   Called at initialization.     

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_ps_evt_hdlr_init
(
  void
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ds707_ps_evt_hdlr_reg_event(PS_SYS_EVENT_3GPP_LTE_ATTACH_INIT,
                              PS_SYS_TECH_3GPP, 
                              NULL);

  ds707_ps_evt_hdlr_reg_event(PS_SYS_EVENT_3GPP_LTE_ATTACH_COMPLETE,
                              PS_SYS_TECH_3GPP, 
                              NULL);
  return;
} /* ds707_ps_evt_hdlr_init */

#endif /* FEATURE_EHRPD */
