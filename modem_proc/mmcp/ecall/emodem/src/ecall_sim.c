/*===========================================================================

                            E C A L L _ S I M . C

DESCRIPTION
  eCall file that has APIs to interact with the SIM using MMGSDI APIs.

REFERENCES
  3GPP 31.102-860 for sim

EXTERNALIZED FUNCTIONS
 
INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2009-2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/ecall/emodem/src/ecall_sim.c#1 $
$DateTime: 2016/12/13 08:00:05 $
$Author: mplcsds1 $ 
===========================================================================*/

/*===========================================================================

                         I N C L U D E   F I L E S

===========================================================================*/

#include "comdef.h"
#include "customer.h"


#ifdef FEATURE_ECALL_IVS
#include "ecall_app_task.h"
#include "ecall_sim.h"
#include "ecall_efs.h"
#include "ecall_ivs_task.h"
#include "msg.h"
#include "err.h"
#include "sys.h"
#include <stringl/stringl.h>

#ifndef FEATURE_ECALL_LCU45
#include "mmgsdilib_v.h"
#else
#include "mmgsdi.h"
#endif /* FEATURE_ECALL_LCU45 */

#define ECALL_EST_FDN_MASK         0x01
#define ECALL_UST_BIT89_MASK       0x01
#define ECALL_UST_FDN_MASK         0x02
#define ECALL_UST_SDN_MASK         0x08

/* call back structure type to store the call back
   function pointer from the APP in a global struture*/
typedef struct
{
  ecall_operating_mode_cb_func_type    operating_mode_cb_func;
  void                 *operating_mode_cb_data;
} ecall_operating_mode_callback_type; 

/* Stores the Call back function pointer and user data received from APP*/
static ecall_operating_mode_callback_type ecall_operating_mode_status_cb = {NULL, NULL};   

/*===========================================================================
   FUNCTION ECALL_SIM_CFG_S_PTR

   DESCRIPTION
     Return the pointer to the ecall SIM configuration data.

   DEPENDENCIES
     none

   RETURN VALUE
     Pointer to the internal configuration data.

   SIDE EFFECTS
     None.
===========================================================================*/
ecall_sim_cfg_s_type * ecall_sim_cfg_s_ptr(
  void
)
{
  static ecall_sim_cfg_s_type ecall_sim_cfg;

  return &ecall_sim_cfg;
} /* ECALL_SIM_CFG_S_PTR */

/*===========================================================================

FUNCTION ECALL_SIM_CFG_INIT

DESCRIPTION 
   This API will initialize the eCall SIM configuration data to default values.

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
void ecall_sim_cfg_init
(
  void
)
{
  ecall_sim_cfg_s_type             *cfg_s_ptr     = ecall_sim_cfg_s_ptr();

  cfg_s_ptr->client_id = 0;

  memset(&cfg_s_ptr->ecall_cm_subs_info, 0, sizeof(ecall_sim_cm_subs_pref_info_s_type));
  memset(&cfg_s_ptr->ecall_app1_info,    0, sizeof(ecall_prov_app_info_s_type));
  memset(&cfg_s_ptr->ecall_app2_info,    0, sizeof(ecall_prov_app_info_s_type));
  memset(&cfg_s_ptr->ecall_app3_info,    0, sizeof(ecall_prov_app_info_s_type));

#ifdef FEATURE_DUAL_SIM
  cfg_s_ptr->ecall_app1_info.as_id                = SYS_MODEM_AS_ID_NONE;
  cfg_s_ptr->ecall_app1_info.session_type         = MMGSDI_GW_PROV_PRI_SESSION;
  cfg_s_ptr->ecall_app1_info.ecall_operating_mode = ECALL_MODE_UNKNOWN;

  cfg_s_ptr->ecall_app2_info.as_id                = SYS_MODEM_AS_ID_NONE;
  cfg_s_ptr->ecall_app2_info.session_type         = MMGSDI_GW_PROV_SEC_SESSION;
  cfg_s_ptr->ecall_app2_info.ecall_operating_mode = ECALL_MODE_UNKNOWN;

  cfg_s_ptr->ecall_app3_info.as_id                = SYS_MODEM_AS_ID_NONE;
  cfg_s_ptr->ecall_app3_info.session_type         = MMGSDI_GW_PROV_TER_SESSION;
  cfg_s_ptr->ecall_app3_info.ecall_operating_mode = ECALL_MODE_UNKNOWN;
#else
  /* Single SIM case */
  cfg_s_ptr->ecall_app1_info.as_id                = SYS_MODEM_AS_ID_1;
  cfg_s_ptr->ecall_app1_info.session_type         = MMGSDI_GW_PROV_PRI_SESSION;
  cfg_s_ptr->ecall_app1_info.ecall_operating_mode = ECALL_MODE_UNKNOWN;

  cfg_s_ptr->ecall_app2_info.as_id                = SYS_MODEM_AS_ID_NONE;
  cfg_s_ptr->ecall_app2_info.session_type         = MMGSDI_GW_PROV_PRI_SESSION;
  cfg_s_ptr->ecall_app2_info.ecall_operating_mode = ECALL_MODE_UNKNOWN;

  cfg_s_ptr->ecall_app3_info.as_id                = SYS_MODEM_AS_ID_NONE;
  cfg_s_ptr->ecall_app3_info.session_type         = MMGSDI_GW_PROV_PRI_SESSION;
  cfg_s_ptr->ecall_app3_info.ecall_operating_mode = ECALL_MODE_UNKNOWN;
#endif /* FEATURE_DUAL_SIM */

  /* Initialize the subs_pref session state */
  {
    uint8 idx;

    for (idx = 0; idx < MAX_AS_IDS; idx++)
    {
      cfg_s_ptr->ecall_cm_subs_info.sub[idx].session_type_1x = MMGSDI_MAX_SESSION_TYPE_ENUM;
      cfg_s_ptr->ecall_cm_subs_info.sub[idx].is_available_1x = FALSE;
      cfg_s_ptr->ecall_cm_subs_info.sub[idx].session_type_gw = MMGSDI_MAX_SESSION_TYPE_ENUM;
      cfg_s_ptr->ecall_cm_subs_info.sub[idx].is_available_gw = FALSE;
      cfg_s_ptr->ecall_cm_subs_info.sub[idx].is_operational  = FALSE;
    }
  }

} /* ECALL_SIM_CFG_INIT */

/*===========================================================================

FUNCTION ECALL_OPERATING_MODE_REG

DESCRIPTION 
   Using this API, the APP can register to get the  the status of the ECALL modem configuration
   (ECALL ONLY or ECALL NORMAL) thru Callback passed in this API.

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
void ecall_operating_mode_reg
(
  ecall_operating_mode_cb_func_type  operating_mode_cb_func,
  void *operating_mode_cb_data
)
{
  ecall_operating_mode_status_cb.operating_mode_cb_func =  operating_mode_cb_func;
  ecall_operating_mode_status_cb.operating_mode_cb_data =  operating_mode_cb_data;
}

/*============================================================================
   FUNCTION:      ECALL_PROCESS_SIM_CONFIG_UPDATE

   DESCRIPTION:
     This function provides indication to NAS about the ecall modem
     configuration - ecall_only or ecall_normal

   DEPENDENCIES:
     None

   LIMITATIONS:
     Parameter check for specific commands are not performed

   RETURN VALUE:
     None

   SIDE EFFECTS:
     None
==============================================================================*/
/*lint -esym(714,ecall_oper_mode)*/
void ecall_process_sim_config_update(ecall_operating_mode_type ecall_oper_mode)
{
  ecall_operating_mode_cb_func_type  operating_mode_cb_func = ecall_operating_mode_status_cb.operating_mode_cb_func;
  ecall_operating_mode_type *operating_mode_cb_data         = ecall_operating_mode_status_cb.operating_mode_cb_data;

  ECALL_MSG_1( ECALL_HIGH, "update operating mode %d to clients",
                            ecall_oper_mode );

  /* Inform the APP about the ecall modem configu using the callback */
  if(operating_mode_cb_func != NULL)
  {
    operating_mode_cb_func(operating_mode_cb_data, ecall_oper_mode);
  }
} /* ecall_process_sim_config_update */

/*============================================================================
   FUNCTION:      ECALL_SEND_MODEM_CONFIG_TO_NAS

   DESCRIPTION:
     This function provides indication to NAS about the ecall modem configuration - ecall_only or ecall_normal

   DEPENDENCIES:
     None

   LIMITATIONS:
     Parameter check for specific commands are not performed

   RETURN VALUE:
     None

   SIDE EFFECTS:
     None
==============================================================================*/
void ecall_send_modem_config_to_nas
(
  sys_modem_as_id_e_type as_id
)
{
  reg_cmd_type* nas_cmd_ptr = NULL;
  ecall_cmd_type *emodem_cmd_ptr = NULL;
  ecall_prov_app_info_s_type *appInfo_ptr = NULL;

  nas_cmd_ptr = reg_ecall_get_cmd_buf (ECALL_REG_MODE_IND); 

  if (nas_cmd_ptr == NULL)
  {
     ECALL_MSG_0( ECALL_ERROR, "No NAS cmd buffer" );
     return;
  }

  appInfo_ptr = ecall_sim_get_appinfo_by_as_id(as_id);

  if (NULL == appInfo_ptr)
  {
    ECALL_MSG_1( ECALL_ERROR, "ecall_send_modem_config_to_nas(): Cannot find mapped appInfo for as_id %d",
                               as_id );
    return;
  }

#ifdef FEATURE_DUAL_SIM
  nas_cmd_ptr->cmd.ecall_reg_mode_ind.as_id = as_id;
#endif /* FEATURE_DUAL_SIM */

  if (appInfo_ptr->ecall_operating_mode != ECALL_MODE_UNKNOWN )
  {
    if (appInfo_ptr->ecall_operating_mode == ECALL_MODE_NORMAL )
    {
      nas_cmd_ptr->cmd.ecall_reg_mode_ind.mode_ind = REG_ECALL_MODE_NORMAL;
    } /* ECALL_MODE_NORMAL */

    else if (appInfo_ptr->ecall_operating_mode == ECALL_MODE_ECALL_ONLY )
    {
      nas_cmd_ptr->cmd.ecall_reg_mode_ind.mode_ind = REG_ECALL_MODE_ECALL_ONLY;
    } /* ECALL_MODE_ECALL_ONLY */
          
  } /* ECALL_MODE_UNKNOWN */
  else
  {
    ECALL_MSG_1( ECALL_ERROR, "ERROR: ecall_operating_mode = ECALL_MODE_UNKNOWN, sending default REG_ECALL_MODE_NORMAL for as_id %d",
                               appInfo_ptr->as_id );
    nas_cmd_ptr->cmd.ecall_reg_mode_ind.mode_ind       = REG_ECALL_MODE_NORMAL;
  }

  if((emodem_cmd_ptr = ecall_get_cmd_buf()) == NULL)
  {
    ECALL_MSG_0( ECALL_ERROR, "No emodem_cmd_ptr buffer" );
  }
  else
  {
    emodem_cmd_ptr->cmd_id = ECALL_CMD_SIM_CONFIG_UPDATE;
    emodem_cmd_ptr->data.ecall_operating_mode = appInfo_ptr->ecall_operating_mode;                             
    ecall_put_cmd(emodem_cmd_ptr);
  }

  ECALL_MSG_2( ECALL_HIGH, "ecall_send_modem_config_to_nas with operating mode %d, as_id %d",
                            appInfo_ptr->ecall_operating_mode,
                            appInfo_ptr->as_id );
                            

  reg_put_cmd(nas_cmd_ptr);

} /* ecall_send_modem_config_to_nas */

/*===========================================================================
   FUNCTION:      ECALL_SIM_PROCESS_UPDATE_CM_SUBS_INFO

   DESCRIPTION:
     This function will process the updated CM SUBS info and find out is there
     any pending ecall_operating_mode needs to be reported to NAS.

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     None

   SIDE EFFECTS:
     None
===========================================================================*/
void ecall_sim_process_update_cm_subs_info
(
  void
)
{
  ecall_sim_cfg_s_type       *lcfg_s_ptr     = ecall_sim_cfg_s_ptr();
  uint8                       num_instances  = 0;
  ecall_cmd_type             *emodem_cmd_ptr = NULL;
  ecall_prov_app_info_s_type *appInfo_ptr    = NULL;

  /* Check if any appInfo is pending to send operating_mode to NAS */
  if ( TRUE == lcfg_s_ptr->ecall_app1_info.is_pending_send_operating_mode_to_nas )
  {
    ECALL_MSG_0( ECALL_HIGH, "ecall_app1_info is pending sending operating_mode to NAS");
    num_instances++;
    appInfo_ptr = &lcfg_s_ptr->ecall_app1_info;
  }

  if ( TRUE == lcfg_s_ptr->ecall_app2_info.is_pending_send_operating_mode_to_nas )
  {
    ECALL_MSG_0( ECALL_HIGH, "ecall_app2_info is pending sending operating_mode to NAS");
    num_instances++;
    appInfo_ptr = &lcfg_s_ptr->ecall_app2_info;
  }

  if ( TRUE == lcfg_s_ptr->ecall_app3_info.is_pending_send_operating_mode_to_nas )
  {
    ECALL_MSG_0( ECALL_HIGH, "ecall_app3_info is pending sending operating_mode to NAS");
    num_instances++;
    appInfo_ptr = &lcfg_s_ptr->ecall_app3_info;
  }

  if (num_instances == 0)
  {
    ECALL_MSG_0( ECALL_HIGH, "No ecall_app_info is pending to send operating_mode to NAS");
    return;
  }
  else if (num_instances > 1)
  {
    ECALL_MSG_0( ECALL_ERROR, "More than 1 ecall_app_info are pending to send operating_mode to NAS");
    return;
  }

  ECALL_MSG_5( ECALL_HIGH, "process_update_cm_subs_info() appInfo: as_id %d, operating_mode %d, session_id 0x%x, session_type %d, session_activated %d",
                            appInfo_ptr->as_id,
                            appInfo_ptr->ecall_operating_mode,
                            appInfo_ptr->session_id,
                            appInfo_ptr->session_type,
                            appInfo_ptr->session_activated );

  ECALL_MSG_3( ECALL_HIGH, "process_update_cm_subs_info() appInfo: is_available %d, is_sim_read_success %d, is_pending_send_operating_mode_to_nas %d",
                            appInfo_ptr->is_available,
                            appInfo_ptr->is_sim_read_success,
                            appInfo_ptr->is_pending_send_operating_mode_to_nas );
                           
  if ((emodem_cmd_ptr = ecall_get_cmd_buf()) == NULL)
  {
    ECALL_MSG_0( ECALL_ERROR, "No emodem_cmd_ptr buffer in update_cm_subs_info" );
    return;
  }
  emodem_cmd_ptr->cmd_id                    = ECALL_CMD_SET_AND_SEND_OPERATING_MODE;
  emodem_cmd_ptr->data.as_id                = appInfo_ptr->as_id;
  ecall_put_cmd(emodem_cmd_ptr);
} /* ECALL_SIM_PROCESS_UPDATE_CM_SUBS_INFO */

/*===========================================================================
   FUNCTION:      ECALL_PROCESS_SIM_READ_SUCCESS

   DESCRIPTION:
     This function will update ecall_operating_mode by the inputs:
	   Service bit 89, FDN service bit 2 and SDN service bit 4

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     None

   SIDE EFFECTS:
     None
===========================================================================*/
void ecall_process_sim_read_success
(
  const uint8 sbit89,
  const uint8 sbit2,
  const uint8 sbit4,
  ecall_prov_app_info_s_type *appInfo_ptr
)
{
  ecall_cmd_type              *emodem_cmd_ptr = NULL;

  if (NULL == appInfo_ptr)
  {
    ECALL_MSG_0( ECALL_ERROR, "sim_read_success(): Null appInfo_ptr" );
    return;
  }

  ECALL_MSG_4( ECALL_HIGH, "UST: sbit89 %d, sbit2 %d, sbit4 %d, session_id 0x%x",
                            sbit89,
                            sbit2,
                            sbit4,
                            appInfo_ptr->session_id );

  appInfo_ptr->sbit89              = sbit89;
  appInfo_ptr->sbit2               = sbit2;
  appInfo_ptr->sbit4               = sbit4;
  appInfo_ptr->is_sim_read_success = TRUE;

#ifdef FEATURE_DUAL_SIM
  if ( FALSE == appInfo_ptr->is_available )
  {
    /* For Dual SIM case with as_id not yet mapped.                         *
     * Set the is_pending_send_operating_mode_to_nas flag to TRUE and send  *
     * info to NAS later when as_id is mapped.                              */
    ECALL_MSG_0( ECALL_HIGH, "as_id not yet mapped, will send operating_mode to NAS once as_id is mapped");
    appInfo_ptr->is_pending_send_operating_mode_to_nas = TRUE;
  }
  else
  {
   /* For Dual SIM case with as_id already mapped.     *
    * Queue cmd to IVS and send operating mode to NAS. */
    if ((emodem_cmd_ptr = ecall_get_cmd_buf()) == NULL)
    {
      ECALL_MSG_0( ECALL_ERROR, "No emodem_cmd_ptr buffer in ecall_process_sim_read_success");
      return;
    }
    emodem_cmd_ptr->cmd_id     = ECALL_CMD_SET_AND_SEND_OPERATING_MODE;
    emodem_cmd_ptr->data.as_id = appInfo_ptr->as_id;
    ecall_put_cmd(emodem_cmd_ptr);
  }
#else /* Single SIM case */
  if ((emodem_cmd_ptr = ecall_get_cmd_buf()) == NULL)
  {
    ECALL_MSG_0( ECALL_ERROR, "No emodem_cmd_ptr buffer in ecall_process_sim_read_success");
    return;
  }
  emodem_cmd_ptr->cmd_id     = ECALL_CMD_SET_AND_SEND_OPERATING_MODE;
  emodem_cmd_ptr->data.as_id = SYS_MODEM_AS_ID_1;
  ecall_put_cmd(emodem_cmd_ptr);
#endif /* FEATURE_DUAL_SIM */

} /* ecall_process_sim_read_success */

/*===========================================================================
   FUNCTION:      ECALL_PROCESS_SIM_READ_FAILED

   DESCRIPTION:
     This function will update the is_sim_read_success variable and queue
     a command to IVS to process with operating mode.

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     None

   SIDE EFFECTS:
     None
===========================================================================*/
void ecall_process_sim_read_failed
(
  ecall_prov_app_info_s_type *appInfo_ptr
)
{
  ecall_cmd_type              *emodem_cmd_ptr = NULL;
  
  if (NULL == appInfo_ptr)
  {
    ECALL_MSG_0( ECALL_ERROR, "sim_read_failed(): Null appInfo_ptr" );
    return;
  }

  ECALL_MSG_1( ECALL_HIGH, "Sim read failed: session_id 0x%x",
                            appInfo_ptr->session_id );

  appInfo_ptr->is_sim_read_success = FALSE;

#ifdef FEATURE_DUAL_SIM
  if ( FALSE == appInfo_ptr->is_available )
  {
    /* For Dual SIM case with as_id not yet mapped.                         *
     * Set the is_pending_send_operating_mode_to_nas flag to TRUE and send  *
     * info to NAS later when as_id is mapped.                              */
    ECALL_MSG_0( ECALL_HIGH, "as_id not yet mapped, will send operating_mode to NAS once as_id is mapped");
    appInfo_ptr->is_pending_send_operating_mode_to_nas = TRUE;
  }
  else
  {
   /* For Dual SIM case with as_id already mapped.     *
    * Queue cmd to IVS and send operating mode to NAS. */
    if ((emodem_cmd_ptr = ecall_get_cmd_buf()) == NULL)
    {
      ECALL_MSG_0( ECALL_ERROR, "No emodem_cmd_ptr buffer in ecall_process_sim_read_success");
      return;
    }
    emodem_cmd_ptr->cmd_id     = ECALL_CMD_SET_AND_SEND_OPERATING_MODE;
    emodem_cmd_ptr->data.as_id = appInfo_ptr->as_id;
    ecall_put_cmd(emodem_cmd_ptr);
  }
#else /* Single SIM case */
  if ((emodem_cmd_ptr = ecall_get_cmd_buf()) == NULL)
  {
    ECALL_MSG_0( ECALL_ERROR, "No emodem_cmd_ptr buffer in ecall_process_sim_read_success");
    return;
  }
  emodem_cmd_ptr->cmd_id     = ECALL_CMD_SET_AND_SEND_OPERATING_MODE;
  emodem_cmd_ptr->data.as_id = SYS_MODEM_AS_ID_1;
  ecall_put_cmd(emodem_cmd_ptr);
#endif /* FEATURE_DUAL_SIM */

} /* ecall_process_sim_read_failed */

/*===========================================================================
   FUNCTION:      ECALL_SIM_PROCESS_SET_AND_SEND_OPERATING_MODE

   DESCRIPTION:
     This function set and send ecall_operating_mode by the parameters
     in the appInfo, and send it to NAS.

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     None

   SIDE EFFECTS:
     None
===========================================================================*/
void ecall_sim_process_set_and_send_operating_mode
(
  sys_modem_as_id_e_type  input_as_id
)
{
  ecall_prov_app_info_s_type  *appInfo_ptr =  NULL;
  ecall_operating_mode_type   set_operating_mode = ECALL_MODE_UNKNOWN;
  mmgsdi_slot_id_enum_type    ecall_slot_id = (mmgsdi_slot_id_enum_type)0;

  ecall_slot_id = ecall_app_task_get_ecall_usim_slot_id();

  if ((mmgsdi_slot_id_enum_type)0 == ecall_slot_id)
  {
    ECALL_MSG_1( ECALL_ERROR, "Set and send operating mode: Unmapped ecall_slot_id %d",
                               ecall_slot_id );
    return;
  }

  appInfo_ptr = ecall_sim_get_appinfo_by_slot_id(ecall_slot_id);

  if (NULL == appInfo_ptr)
  {
    ECALL_MSG_0( ECALL_ERROR, "Set and send operating mode: NULL appInfo_ptr" );
    return;
  }

  if (input_as_id != appInfo_ptr->as_id)
  {
    ECALL_MSG_2( ECALL_ERROR, "appInfo_ptr->as_id %d != input_as_id %d in process_set_and_send_operating_mode",
                               appInfo_ptr->as_id,
                               input_as_id );
    return;
  }

  if (TRUE == ecall_app_task_get_is_era_glonass_enabled())
  {
    ECALL_MSG_0( ECALL_HIGH, "ERA-GLONASS is enabled, set operating_mode to ECALL_ONLY" );
    set_operating_mode = ECALL_MODE_ECALL_ONLY;
    goto Done;
  }

  ECALL_MSG_4( ECALL_HIGH, "Set and send operating mode: ecall efs slot_id %d, is_available %d, as_id %d, is_pending_send_operating_mode_to_nas %d",
                            ecall_slot_id,
                            appInfo_ptr->is_available,
                            appInfo_ptr->as_id,
                            appInfo_ptr->is_pending_send_operating_mode_to_nas );

  ECALL_MSG_5( ECALL_HIGH, "Set and send operating mode: appInfo app_type %d, slot_id %d, session_type %d, session_id 0x%x, session_activated %d",
                            appInfo_ptr->app_type,
                            appInfo_ptr->slot_id,
                            appInfo_ptr->session_type,
                            appInfo_ptr->session_id,
                            appInfo_ptr->session_activated );
  
  ECALL_MSG_5( ECALL_HIGH, "Set and send operating mode: is_sim_read_success %d, sbit2 %d, sbit4 %d, sbit89 %d, is_est_fdn_enabled %d",
                            appInfo_ptr->is_sim_read_success,
                            appInfo_ptr->sbit2,
                            appInfo_ptr->sbit4,
                            appInfo_ptr->sbit89,
                            appInfo_ptr->is_est_fdn_enabled );
                            
#ifdef FEATURE_DUAL_SIM
  if ( FALSE == appInfo_ptr->is_available )
  {
    ECALL_MSG_0( ECALL_HIGH, "Set and send operating mode: as_id not mapped yet");
    return;
  }
#endif /* FEATURE_DUAL_SIM */

  if ( FALSE == appInfo_ptr->session_activated )
  {
    ECALL_MSG_0( ECALL_HIGH, "Set and send operating mode: session not activated");
    goto Done;
  }

  if ( MMGSDI_APP_USIM != appInfo_ptr->app_type )
  {
    ECALL_MSG_1( ECALL_HIGH, "Set and send operating mode: app_type not USIM (%d), set operating_mode to NORMAL",
                              appInfo_ptr->app_type );
    set_operating_mode = ECALL_MODE_NORMAL;
    goto Done;
  }

  if ( FALSE == appInfo_ptr->is_sim_read_success )
  {
    ECALL_MSG_0( ECALL_HIGH, "Set and send operating mode: sim read NOT successful, set operating_mode to NORMAL");
    set_operating_mode = ECALL_MODE_NORMAL;
    goto Done;
  }
  
  if (appInfo_ptr->sbit89 == 1)
  {
     /* Check if Service Bit 89, Service Bit 2 and EST FDN are available for eCall Only mode */
    if((appInfo_ptr->sbit2 == 1) && (appInfo_ptr->is_est_fdn_enabled == 1))
    {
      set_operating_mode = ECALL_MODE_ECALL_ONLY;
      ECALL_MSG_0( ECALL_HIGH, "USIM configuration with ECALL_ONLY mode" );
    }
    else if (appInfo_ptr->sbit4 == 1)
    {
      /* Check if Service Bit 89 and Service Bit 4 are available for eCall Normal mode */
      set_operating_mode = ECALL_MODE_NORMAL;
      ECALL_MSG_0( ECALL_HIGH, "USIM configuration with ECALL_NORMAL mode" );
    }
    else /* sbit89==1, sbit2==0, sbit4==0. Incorrect USIM configuration */
    {
      ECALL_MSG_0( ECALL_ERROR, "Incorrect eCall USIM configuration (sbit89==1,sbit2==0,sbit4==0" );
    }
  }

Done:
  ECALL_MSG_2( ECALL_HIGH, "Set ecall_operating_mode to %d for as_id %d, and send to NAS",
                            set_operating_mode,
                            appInfo_ptr->as_id );
  appInfo_ptr->ecall_operating_mode = set_operating_mode;
  appInfo_ptr->is_pending_send_operating_mode_to_nas = FALSE;
  ecall_send_modem_config_to_nas(appInfo_ptr->as_id);
} /* ECALL_SIM_PROCESS_SET_AND_SEND_OPERATING_MODE */

/*===========================================================================
   FUNCTION:      ECALL_MMGSDI_READ_TRANSPARENT_UST_CB

   DESCRIPTION:
     This callback is called from mmgsdi_read_transparent.
	 The service bit 89, FDN service bit 2 and SDN service bit 4 is read here.

   DEPENDENCIES:
     None

   LIMITATIONS:
     Parameter check for specific commands are not performed

   RETURN VALUE:
     None

   SIDE EFFECTS:
     None
===========================================================================*/
/* ARGSUSED */
void ecall_mmgsdi_read_transparent_ust_cb
(
  mmgsdi_return_enum_type status, 
  mmgsdi_cnf_enum_type    cnf, 
  mmgsdi_cnf_type        *cnf_ptr
)
{
  ecall_cmd_type *emodem_cmd_ptr = NULL;
  uint8 sbit89 = 0;
  

  if (NULL == cnf_ptr)
  {
    ECALL_MSG_0( ECALL_ERROR, "Bad cnf_ptr from ecall_mmgsdi_read_transparent_est_cb");
    return;
  }

  if ( cnf_ptr->read_cnf.response_header.mmgsdi_status == MMGSDI_SUCCESS )
  {
     sbit89 = cnf_ptr->read_cnf.read_data.data_ptr[11] & ECALL_UST_BIT89_MASK;

     /* Check for value of byte 12 & Service bit 89 is 1 */
    if ((cnf_ptr->read_cnf.read_data.data_len == 12) && (sbit89 == 1))
    {
      emodem_cmd_ptr = ecall_get_cmd_buf();

      if (emodem_cmd_ptr == NULL)
      {
        ECALL_MSG_0( ECALL_ERROR, "No emodem_cmd_ptr buffer" );
        return;
      }
      /*======================================================
         | Byte 0   |  Byte 1  | …           |Byte 11  |
         -----------------------------------------------
     Bit |  8.7….1  | 16……… 9  | …           |96………89  |  
        ======================================================*/ 
      emodem_cmd_ptr->cmd_id = ECALL_CMD_SIM_READ_SUCCESS;

      /* Client 'user_data' is returned in: cnf_ptr->response_header.client_data */
      emodem_cmd_ptr->data.sim_data.appInfo_ptr = (ecall_prov_app_info_s_type*) cnf_ptr->response_header.client_data;
      emodem_cmd_ptr->data.sim_data.bit89_value = sbit89;
      emodem_cmd_ptr->data.sim_data.bit2_value  = (cnf_ptr->read_cnf.read_data.data_ptr[0] & ECALL_UST_FDN_MASK) >> 1;
      emodem_cmd_ptr->data.sim_data.bit4_value  = (cnf_ptr->read_cnf.read_data.data_ptr[0] & ECALL_UST_SDN_MASK) >> 3;
      ecall_put_cmd(emodem_cmd_ptr);
    }
    else
    {
      ECALL_MSG_2( ECALL_ERROR, "USIM card does not have service bit 89 (data bytes < 12), data_len = %d or bit 89 diabled = %d ",
                                 cnf_ptr->read_cnf.read_data.data_len,
                                 sbit89 );
      emodem_cmd_ptr = ecall_get_cmd_buf();
      if (emodem_cmd_ptr == NULL)
      {
        ECALL_MSG_0( ECALL_ERROR, "No emodem_cmd_ptr buffer" );
        return;
      }
      /* Client 'user_data' is returned in: cnf_ptr->response_header.client_data */
      emodem_cmd_ptr->data.sim_data.appInfo_ptr = (ecall_prov_app_info_s_type*) cnf_ptr->response_header.client_data;
      emodem_cmd_ptr->cmd_id                    = ECALL_CMD_SIM_READ_FAILED;
      ecall_put_cmd(emodem_cmd_ptr);
    }

  } /* MMGSDI_SUCCESS */
  else
  {
    ECALL_MSG_2( ECALL_ERROR, "ust_cb can not handle opearation client_data = %d, mmgsdi_status = %d", 
                               cnf_ptr->read_cnf.response_header.client_data,
                               cnf_ptr->read_cnf.response_header.mmgsdi_status );
    emodem_cmd_ptr = ecall_get_cmd_buf();
    if (emodem_cmd_ptr == NULL)
    {
      ECALL_MSG_0( ECALL_ERROR, "No emodem_cmd_ptr buffer" );
      return;
    }
    /* Client 'user_data' is returned in: cnf_ptr->response_header.client_data */
    emodem_cmd_ptr->data.sim_data.appInfo_ptr = (ecall_prov_app_info_s_type*) cnf_ptr->response_header.client_data;
    emodem_cmd_ptr->cmd_id                    = ECALL_CMD_SIM_READ_FAILED;
    ecall_put_cmd(emodem_cmd_ptr);
  }

} /* ecall_mmgsdi_read_transparent_ust_cb */


/*===========================================================================
   FUNCTION:      ECALL_MMGSDI_READ_TRANSPARENT_EST_CB

   DESCRIPTION:
     This callback is called from mmgsdi_read_transparent.
   The service bit 89, FDN service bit 2 and SDN service bit 4 is read here.

   DEPENDENCIES:
     None

   LIMITATIONS:
     Parameter check for specific commands are not performed

   RETURN VALUE:
     None

   SIDE EFFECTS:
     None
===========================================================================*/
/* ARGSUSED */
void ecall_mmgsdi_read_transparent_est_cb
(
  mmgsdi_return_enum_type status, 
  mmgsdi_cnf_enum_type    cnf, 
  mmgsdi_cnf_type        *cnf_ptr
)
{
  uint8 est_fdn = 0;
  ecall_cmd_type *emodem_cmd_ptr = NULL;

  if (NULL == cnf_ptr)
  {
    ECALL_MSG_0( ECALL_ERROR, "Bad cnf_ptr from ecall_mmgsdi_read_transparent_est_cb");
    return;
  }

  if ( cnf_ptr->read_cnf.response_header.mmgsdi_status == MMGSDI_SUCCESS)
  {
    if (cnf_ptr->read_cnf.read_data.data_ptr != NULL)
    {
       est_fdn = cnf_ptr->read_cnf.read_data.data_ptr[0] & ECALL_EST_FDN_MASK;
       ECALL_MSG_1( ECALL_HIGH, "ecall_mmgsdi_read_transparent_est_cb, EST FDN Enabled = %d",
                                 est_fdn );
    }
    else
    {
      ECALL_MSG_0( ECALL_HIGH, "ecall_mmgsdi_read_transparent_est_cb, Setting EST FDN Enabled to 0 (data_ptr is NULL)" );
      est_fdn = 0;
    }  
  }
  else
  {
    ECALL_MSG_1( ECALL_HIGH, "ecall_mmgsdi_read_transparent_est_cb, Setting FDN Enabled to 0, mmgsdi_status = %d",
                              cnf_ptr->read_cnf.response_header.mmgsdi_status );
    est_fdn = 0;
  }

  emodem_cmd_ptr = ecall_get_cmd_buf();
  if (emodem_cmd_ptr == NULL)
  {
    ECALL_MSG_0( ECALL_ERROR, "No emodem_cmd_ptr buffer" );
    return;
  }

  /* Client 'user_data' is returned in: cnf_ptr->response_header.client_data */
  emodem_cmd_ptr->data.sim_data.appInfo_ptr     = (ecall_prov_app_info_s_type*) cnf_ptr->response_header.client_data;
  emodem_cmd_ptr->data.sim_data.est_fdn_enabled = est_fdn;
  emodem_cmd_ptr->cmd_id                        = ECALL_CMD_SET_EST_FDN_ENABLED;
  ecall_put_cmd(emodem_cmd_ptr);

} /* ecall_mmgsdi_read_transparent_est_cb */

/*===========================================================================

FUNCTION ECALL_SIM_MMGSDI_REFRESH_RESPONSE_CB

DESCRIPTION
    This function is the callback function of eCall register for MMGSDI
    REFRESH.

DEPENDENCIES
    None

INPUT
    mmgsdi_cnf_type* cnf_data_ptr

RETURN VALUE
    None

OUTPUT
   
===========================================================================*/
void ecall_sim_mmgsdi_refresh_response_cb
(
  mmgsdi_return_enum_type  status,
  mmgsdi_cnf_enum_type     cnf,
  const mmgsdi_cnf_type   *cnf_ptr
)
{
   ECALL_MSG_3( ECALL_HIGH, "refresh response cb: client_data 0x%x, cnf %d, status %d",
                             cnf_ptr->refresh_cnf.response_header.client_data,
                             cnf,
                             status );
} /* ECALL_SIM_MMGSDI_REFRESH_RESPONSE_CB() */

/*===========================================================================

FUNCTION ECALL_SIM_MMGSDI_REGISTER_FOR_REFRESH

DESCRIPTION
    This function is for eCall to register for MMGSDI REFRESH

DEPENDENCIES
    None

INPUT
    mmgsdi_cnf_type* cnf_data_ptr

RETURN VALUE
    None

OUTPUT
   
===========================================================================*/
void ecall_sim_mmgsdi_register_for_refresh
(
  mmgsdi_session_id_type session_id
)
{
  mmgsdi_refresh_file_list_type refresh_files;
  mmgsdi_return_enum_type       status;
  mmgsdi_file_enum_type         file_list[] =
  {
    MMGSDI_USIM_UST,            /* USIM Service Table. */
    MMGSDI_USIM_EST             /* Enabled Services Table. */
  };
  refresh_files.num_files     = sizeof(file_list)/sizeof(file_list[0]);
  refresh_files.file_list_ptr = file_list;
  refresh_files.file_path_ptr = NULL;
  refresh_files.file_path_len = 0;

  status = mmgsdi_session_register_for_refresh( session_id,
                                                refresh_files,
                                                TRUE,
                                                ecall_sim_mmgsdi_refresh_response_cb,
                                                NULL );
  if (MMGSDI_SUCCESS != status)
  {
    ECALL_MSG_1( ECALL_ERROR, "mmgsdi_session_register_for_refresh failed: error %d",
                               status );
  }
} /* ECALL_SIM_MMGSDI_REGISTER_FOR_REFRESH */

/*===========================================================================

FUNCTION ECALL_SIM_MMGSDI_SESSION_OPEN_CB

DESCRIPTION
    This function is the callback function to retrieve session id if
    sesion open is successfull

DEPENDENCIES
    None

INPUT
    mmgsdi_cnf_type* cnf_data_ptr

RETURN VALUE
    None

OUTPUT
   
===========================================================================*/
void ecall_sim_mmgsdi_session_open_cb
(
  mmgsdi_return_enum_type   status, 
  mmgsdi_cnf_enum_type      cnf,
  const mmgsdi_cnf_type    *cnf_ptr
)
{
  ecall_cmd_type *emodem_cmd_ptr = NULL;

  if (cnf_ptr == NULL)
  {
     ECALL_MSG_0( ECALL_ERROR, "MMGSDI session open resp cnf_ptr as NULL" );
     return;
  }

  if ((MMGSDI_SUCCESS != status) || (MMGSDI_SESSION_OPEN_CNF !=  cnf))
  {
    ECALL_MSG_2( ECALL_ERROR, "MMGSDI fail session open. Status %d, Confirmation %d",
                               status,
                               cnf );
  }
  else
  {
    emodem_cmd_ptr = ecall_get_cmd_buf();
    if (emodem_cmd_ptr == NULL)
    {
      ECALL_MSG_0( ECALL_ERROR, "No emodem_cmd_ptr buffer" );
      return;
    }
    ECALL_MSG_2( ECALL_HIGH, "MMGSDI session opened: session_type %d, session_id 0x%x",
                              cnf_ptr->session_open_cnf.response_header.client_data,
                              cnf_ptr->session_open_cnf.response_header.session_id );
    ECALL_MSG_2( ECALL_HIGH, "MMGSDI session opened: slot_id %d, app_type %d",
                              cnf_ptr->session_open_cnf.slot_id,
                              cnf_ptr->session_open_cnf.app_info.app_type );

    emodem_cmd_ptr->data.mmgsdi_session_open_data.session_type = (mmgsdi_session_type_enum_type)cnf_ptr->session_open_cnf.response_header.client_data;
    emodem_cmd_ptr->data.mmgsdi_session_open_data.session_id   = cnf_ptr->session_open_cnf.session_id;
    emodem_cmd_ptr->data.mmgsdi_session_open_data.slot_id      = cnf_ptr->session_open_cnf.slot_id;
    emodem_cmd_ptr->data.mmgsdi_session_open_data.app_type     = cnf_ptr->session_open_cnf.app_info.app_type;
    emodem_cmd_ptr->cmd_id = ECALL_CMD_SIM_SESSION_OPEN_SUCCESS;
    ecall_put_cmd(emodem_cmd_ptr);
  }
} /* ECALL_SIM_MMGSDI_SESSION_OPEN_CB */

/*==============================================================================
FUNCTION NAME  ECALL_SIM_MMGSDI_EVENT_CB
 
DESCRIPTION
    This function is the event handling function for different events from
    MMGSDI CLIENT API's.

DEPENDENCIES
    None

INPUT
    mmgsdi_event_data_type *event_ptr

RETURN VALUE
    None

OUTPUT
    None

==============================================================================*/
LOCAL void ecall_sim_mmgsdi_event_cb
(
  const mmgsdi_event_data_type *event_ptr
)
{
  ecall_cmd_type *emodem_cmd_ptr = NULL;
 
  if(event_ptr == NULL)
  {
    ECALL_MSG_0( ECALL_ERROR, "ecall_mmgsdi_client_evt_reg_cb:  event_ptr is NULL" );
    return;
  }

  emodem_cmd_ptr = ecall_get_cmd_buf();

  if (emodem_cmd_ptr == NULL)
  {
     ECALL_MSG_0( ECALL_ERROR, "No emodem_cmd_ptr buffer" );
     return;
  }

  emodem_cmd_ptr->cmd_id = ECALL_CMD_SIM_CLIENT_SESSION_EVENTS;
  ecall_memscpy ( &emodem_cmd_ptr->data.sim_data.event, sizeof(emodem_cmd_ptr->data.sim_data.event),
                  event_ptr, sizeof(mmgsdi_event_data_type));
  
  ecall_put_cmd(emodem_cmd_ptr);

} /* ECALL_SIM_MMGSDI_EVENT_CB */

#ifdef FEATURE_ECALL_HAS_ERA_GLONASS
/*==============================================================================
    FUNCTION NAME  ECALL_SIM_MMGSDI_PROFILE_DATA_EVENT_PROC
     
    DESCRIPTION
        This API will process the MMGSDI_PROFILE_DATA event.
    
    DEPENDENCIES
        FEATURE_ECALL_HAS_ERA_GLONASS
    
    INPUT
        mmgsdi_event_data_type *event_ptr
    
    RETURN VALUE
        None
    
    OUTPUT
        None
==============================================================================*/
void ecall_sim_mmgsdi_profile_data_event_proc
(
  const mmgsdi_event_data_type event
)
{
  ecall_app_task_cmd_type *cmd_ptr = NULL;  
  
  switch (event.data.profile_data.profile.profile_type)
  {
    case MMGSDI_PROFILE_TYPE_REGULAR:
      ECALL_MSG_0( ECALL_HIGH, "Received MMGSDI_PROFILE_TYPE_REGULAR" );
      /* Queue up a cmd to eCall App task to update this profile event */
      cmd_ptr = ecall_app_task_get_cmd_buf();
      if (NULL == cmd_ptr)
      {
        ECALL_MSG_0( ECALL_ERROR, "ecall_app_task_get_cmd_buf ran out of command buffers in ecall_sim_mmgsdi_profile_data_event_proc()" );
        return;
      }
      cmd_ptr->data.ecall_profile = ECALL_PROFILE_TYPE_REGULAR;
      cmd_ptr->cmd_id             = ECALL_APP_CMD_UPDATE_ECALL_PROFILE;
      ecall_app_task_put_cmd(cmd_ptr);
      break;

    case MMGSDI_PROFILE_TYPE_EMERGENCY:
      ECALL_MSG_0( ECALL_HIGH, "Received MMGSDI_PROFILE_TYPE_EMERGENCY" );
      /* Queue up a cmd to eCall App task to update this profile event */
      cmd_ptr = ecall_app_task_get_cmd_buf();
      if (NULL == cmd_ptr)
      {
        ECALL_MSG_0( ECALL_ERROR, "ecall_app_task_get_cmd_buf ran out of command buffers in ecall_sim_mmgsdi_profile_data_event_proc()" );
        return;
      }
      cmd_ptr->data.ecall_profile = ECALL_PROFILE_TYPE_EMERGENCY;
      cmd_ptr->cmd_id             = ECALL_APP_CMD_UPDATE_ECALL_PROFILE;
      ecall_app_task_put_cmd(cmd_ptr);
      break;

    default:
      ECALL_MSG_1( ECALL_ERROR, "Unsupported MMGSDI profile %d",
                                 event.data.profile_data.profile.profile_type );
      break;
  }

}
#endif /* FEATURE_ECALL_HAS_ERA_GLONASS */


/*==============================================================================
    FUNCTION NAME  ECALL_SIM_GET_APPINFO_BY_SESSION_TYPE
     
    DESCRIPTION
        This API will return the eCall Provisioning App pointer by
        the input session_type.
    
    DEPENDENCIES
        None
    
    INPUT
        mmgsdi_session_type_enum_type
    
    RETURN VALUE
        ecall_prov_app_info_s_type
    
    OUTPUT
        None
==============================================================================*/
ecall_prov_app_info_s_type* ecall_sim_get_appinfo_by_session_type
(
  mmgsdi_session_type_enum_type session_type
)
{
  ecall_sim_cfg_s_type        *lcfg_s_ptr = ecall_sim_cfg_s_ptr();
  ecall_prov_app_info_s_type  *info_ptr   = NULL;

  switch (session_type)
  {
    case MMGSDI_GW_PROV_PRI_SESSION:
      info_ptr = &lcfg_s_ptr->ecall_app1_info;
      break;

  #ifdef FEATURE_DUAL_SIM
    case MMGSDI_GW_PROV_SEC_SESSION:
      info_ptr = &lcfg_s_ptr->ecall_app2_info;
      break;
  
    case MMGSDI_GW_PROV_TER_SESSION:
      info_ptr = &lcfg_s_ptr->ecall_app3_info;
      break;
  #endif /* FEATURE_DUAL_SIM */

    default:
      ECALL_MSG_1( ECALL_HIGH, "Unmapped mmgsdi session_type: %d", session_type);
  } /* switch */

  return info_ptr;
} /* ecall_sim_get_appinfo_by_session_type() */

/*==============================================================================
    FUNCTION NAME  ECALL_SIM_GET_APPINFO_BY_AS_ID
     
    DESCRIPTION
        This API will return the eCall Provisioning App pointer by
        the input as_id.
    
    DEPENDENCIES
        None
    
    INPUT
        sys_modem_as_id_e_type
    
    RETURN VALUE
        ecall_prov_app_info_s_type
    
    OUTPUT
        None
==============================================================================*/
ecall_prov_app_info_s_type* ecall_sim_get_appinfo_by_as_id
(
  sys_modem_as_id_e_type  as_id
)
{
  ecall_sim_cfg_s_type        *lcfg_s_ptr = ecall_sim_cfg_s_ptr();
  ecall_prov_app_info_s_type  *info_ptr   = NULL;

  if (as_id == lcfg_s_ptr->ecall_app1_info.as_id)
  {
    info_ptr = &lcfg_s_ptr->ecall_app1_info;
  }
  else if (as_id == lcfg_s_ptr->ecall_app2_info.as_id)
  {
    info_ptr = &lcfg_s_ptr->ecall_app2_info;
  }
  else if (as_id == lcfg_s_ptr->ecall_app3_info.as_id)
  {
    info_ptr = &lcfg_s_ptr->ecall_app3_info;
  }

  if (NULL == info_ptr)
  {
    ECALL_MSG_1( ECALL_ERROR, "ecall_sim_get_appinfo_by_as_id(): Unmapped as_id %d",
                               as_id );
  }

  return info_ptr;
} /* ecall_sim_get_appinfo_by_as_id() */


/*==============================================================================
    FUNCTION NAME  ECALL_SIM_GET_APPINFO_BY_SESSION_ID
     
    DESCRIPTION
        This API will return the mapped ecall_mmgsdi_gw_prov_type
    
    DEPENDENCIES
        None
    
    INPUT
        mmgsdi_session_type_enum_type
    
    RETURN VALUE
        ecall_mmgsdi_gw_prov_enum_type
    
    OUTPUT
        None
==============================================================================*/
ecall_prov_app_info_s_type* ecall_sim_get_appinfo_by_session_id
(
  mmgsdi_session_id_type  session_id
)
{
  ecall_sim_cfg_s_type        *lcfg_s_ptr = ecall_sim_cfg_s_ptr();
  ecall_prov_app_info_s_type  *info_ptr   = NULL;

  if (session_id == lcfg_s_ptr->ecall_app1_info.session_id)
  {
    info_ptr = &lcfg_s_ptr->ecall_app1_info;
  }
  else if (session_id == lcfg_s_ptr->ecall_app2_info.session_id)
  {
    info_ptr = &lcfg_s_ptr->ecall_app2_info;
  }
  else if (session_id == lcfg_s_ptr->ecall_app3_info.session_id)
  {
    info_ptr = &lcfg_s_ptr->ecall_app3_info;
  }

  if (NULL == info_ptr)
  {
    ECALL_MSG_1( ECALL_ERROR, "ecall_sim_get_appinfo_by_session_id(): Unmapped session_id 0x%x",
                               session_id );
  }

  return info_ptr;
} /* ecall_sim_get_appinfo_by_session_id() */

/*==============================================================================
    FUNCTION NAME  ECALL_SIM_GET_APPINFO_BY_SLOT_ID
     
    DESCRIPTION
        This API will return the eCall Provisioning App pointer by
        the input slot_id.
    
    DEPENDENCIES
        None
    
    INPUT
        mmgsdi_slot_id_enum_type
    
    RETURN VALUE
        ecall_prov_app_info_s_type
    
    OUTPUT
        None
==============================================================================*/
ecall_prov_app_info_s_type* ecall_sim_get_appinfo_by_slot_id
(
  mmgsdi_slot_id_enum_type slot_id
)
{
  ecall_sim_cfg_s_type        *lcfg_s_ptr = ecall_sim_cfg_s_ptr();
  ecall_prov_app_info_s_type  *info_ptr   = NULL;

  if (slot_id == lcfg_s_ptr->ecall_app1_info.slot_id)
  {
    info_ptr = &lcfg_s_ptr->ecall_app1_info;
  }
  else if (slot_id == lcfg_s_ptr->ecall_app2_info.slot_id)
  {
    info_ptr = &lcfg_s_ptr->ecall_app2_info;
  }
  else if (slot_id == lcfg_s_ptr->ecall_app3_info.slot_id)
  {
    info_ptr = &lcfg_s_ptr->ecall_app3_info;
  }

  if (NULL == info_ptr)
  {
    ECALL_MSG_1( ECALL_ERROR, "ecall_sim_get_appinfo_by_slot_id(): Unmapped slot_id %d",
                               slot_id );
  }

  return info_ptr;
} /* ecall_sim_get_appinfo_by_slot_id() */

/*==============================================================================
    FUNCTION NAME  ECALL_SIM_MMGSDI_CARD_ERROR_EVENT_PROC
     
    DESCRIPTION
        This API will process the MMGSDI CARD_ERROR events.
    
    DEPENDENCIES
        None
    
    INPUT
        mmgsdi_event_data_type *event_ptr
    
    RETURN VALUE
        None
    
    OUTPUT
        None
==============================================================================*/
void ecall_sim_mmgsdi_card_error_event_proc
(
  const mmgsdi_slot_id_enum_type slot_id
)
{
  ecall_prov_app_info_s_type *appInfo_ptr = NULL;
  ecall_cmd_type *emodem_cmd_ptr = NULL;

  ECALL_MSG_1( ECALL_HIGH, "ecall mmgsdi MMGSDI_CARD_ERROR_EVT on slot %d",
                            slot_id );

  appInfo_ptr = ecall_sim_get_appinfo_by_slot_id(slot_id);

  if (NULL == appInfo_ptr)
  {
    ECALL_MSG_1( ECALL_ERROR, "card_error_event_proc(): Unmapped slot_id %d",
                               slot_id );
    return;
  }

  /* Update the appInfo operating_mode, but do not inform NAS */
  appInfo_ptr->ecall_operating_mode = ECALL_MODE_UNKNOWN;

  ECALL_MSG_1( ECALL_HIGH, "card_error_event_proc(): Move ecall_operating_mode to UNKNOWN for as_id %d",
                            appInfo_ptr->as_id );

  /* Post a command to ecall_ivs_task to update ECALL_MODE_UNKNOWN.         
     NAS will get CARD_ERROR_EVENT handled from CM,
     so NAS would not drop N/W Connection release */
  emodem_cmd_ptr = ecall_get_cmd_buf();
  if(emodem_cmd_ptr != NULL)
  {
    emodem_cmd_ptr->cmd_id = ECALL_CMD_SIM_CONFIG_UPDATE;
    emodem_cmd_ptr->data.ecall_operating_mode = ECALL_MODE_UNKNOWN;
    ecall_put_cmd(emodem_cmd_ptr);
  }
  else
  {
    ECALL_MSG_0( ECALL_ERROR, "ecall_get_cmd_buf() ran out of cmd buffer in ecall_process_sim_events" );
  }
} /* ECALL_SIM_MMGSDI_CARD_ERROR_EVENT_PROC */

/*==============================================================================
    FUNCTION NAME  ECALL_SIM_MMGSDI_SESSION_CHANGED_EVENT_PROC
     
    DESCRIPTION
        This API will process the MMGSDI REFRESH events.
    
    DEPENDENCIES
        None
    
    INPUT
        mmgsdi_event_data_type *event_ptr
    
    RETURN VALUE
        None
    
    OUTPUT
        None
==============================================================================*/
void ecall_sim_mmgsdi_session_changed_event_proc
(
  const mmgsdi_event_data_type event
)
{
  ecall_prov_app_info_s_type *appInfo_ptr = NULL;

  ECALL_MSG_2( ECALL_HIGH, "Received MMGSDI_SESSION_CHANGED_EVT session_activated = %d, session_id 0x%x",
                            event.data.session_changed.activated,
                            event.session_id );
                         
  appInfo_ptr = ecall_sim_get_appinfo_by_session_id(event.session_id);

  if (NULL == appInfo_ptr)
  {
    ECALL_MSG_1( ECALL_ERROR, "session_changed_event_proc(): Unmapped session_id 0x%x",
                               event.session_id );
    return;
  }

  ECALL_MSG_4( ECALL_HIGH, "SESSION_CHANGED_EVT: appInfo_ptr as_id %d, session_id 0x%x: session_activated %d->%d",
                            appInfo_ptr->as_id,
                            event.session_id,
                            appInfo_ptr->session_activated,
                            event.data.session_changed.activated );
  
  appInfo_ptr->session_activated = event.data.session_changed.activated;

} /* ECALL_SIM_MMGSDI_SESSION_CHANGED_EVENT_PROC */

/*==============================================================================
    FUNCTION NAME  ECALL_SIM_MMGSDI_REFRESH_EVENT_PROC
     
    DESCRIPTION
        This API will process the MMGSDI REFRESH events.
    
    DEPENDENCIES
        None
    
    INPUT
        mmgsdi_event_data_type *event_ptr
    
    RETURN VALUE
        None
    
    OUTPUT
        None
==============================================================================*/
void ecall_sim_mmgsdi_refresh_event_proc
(
  const mmgsdi_event_data_type event
)
{
  boolean ok_to_refresh = FALSE;
  mmgsdi_return_enum_type ret;
  ecall_app_state_type current_app_state = ECALL_APP_ECALL_INACTIVE;
  ecall_prov_app_info_s_type *appInfo_ptr = NULL;
  current_app_state = ecall_app_task_get_state();

  appInfo_ptr = ecall_sim_get_appinfo_by_session_id(event.session_id);

  if ( appInfo_ptr != NULL )
  {
    ECALL_MSG_2( ECALL_HIGH, "eCall related Refresh Event with session_id 0x%x, session_type %d",
                              appInfo_ptr->session_id,
                              appInfo_ptr->session_type );
  }
  else
  {
    ECALL_MSG_2( ECALL_HIGH, "Not a eCall Refresh Event %d, session_id 0x%x",
                              event.data.refresh.stage,
                              event.session_id );
    return;
  }

  switch (event.data.refresh.stage)
  {
    case MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_INIT:
    case MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_FCN:

      /* MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_[INIT|FCN] is received when files
         registered for change notification are required to be refreshed. At this
         stage, eCall must vote based on whether it is idle or not. */
      if ( (current_app_state == ECALL_APP_ECALL_INACTIVE) || 
           (current_app_state == ECALL_APP_IDLE_ALLOW_MT_ECALL) )
      {
        /* Only say yes to MMGSDI REFRESH if eCall is in inactive or idle_allow_mt_ecall state */
        ok_to_refresh = TRUE;
      }
      else
      {
        ok_to_refresh = FALSE;
      }

      ECALL_MSG_1( ECALL_HIGH,  "eCall votes %d to refresh",
                                 ok_to_refresh );

      ret = mmgsdi_session_ok_to_refresh( appInfo_ptr->session_id,
                                          ok_to_refresh,
                                          ecall_sim_mmgsdi_refresh_response_cb,
                                          NULL );
      if (MMGSDI_SUCCESS != ret)
      {
        ECALL_MSG_1( ECALL_ERROR, "mmgsdi_session_ok_to_refresh WAIT_FOR_OK failed: error=%d",
                                   ret );
      }

      break;

    case MMGSDI_REFRESH_STAGE_START:

      // For INIT refreshes, no files are read when MMGSDI_REFRESH_STAGE_START
      // arrives. When the INIT is complete, a MMGSDI_SUBSCRIPTION_READY event
      // or MMGSDI_REFRESH_STAGE_END_[SUCCESS|FAILED] event will arrive and
      // processing will resume then.

      ECALL_MSG_0( ECALL_HIGH, "REFRESH_STAGE_START" );

      if ( (MMGSDI_REFRESH_NAA_FCN == event.data.refresh.mode) )
      {
        // For FCN refresh, it is expected to read files when MMGSDI_REFRESH_STAGE_START
        // arrives. Once all files are read, a REFRESH_COMPLETE response must be sent.
        // eCall cares about EST and UST changed in REFRESH FCN, but they are updated
        // INIT, not FCN. eCall does not need to read any file at REFRESH FCN.
        ECALL_MSG_0( ECALL_ERROR, "eCall should not receives REFRESH FCN" );

        ret = mmgsdi_session_refresh_complete( appInfo_ptr->session_id,
                                               TRUE,
                                               ecall_sim_mmgsdi_refresh_response_cb,
                                               NULL );
        if (MMGSDI_SUCCESS != ret)
        {
          ECALL_MSG_1( ECALL_ERROR, "mmgsdi_session_refresh_complete failed: error=%d",
                                     ret );
        }
      }
      break;

    case MMGSDI_REFRESH_STAGE_END_FAILED:
      // The MMGSDI_REFRESH_STAGE_END_FAILED event arrives after eCall has read
      // its files and has responded with REFRESH_COMPLETE or after an INIT
      // refresh has completed.
      ECALL_MSG_0( ECALL_HIGH, "REFRESH_STAGE_END_FAILED" );
      break;

    case MMGSDI_REFRESH_STAGE_END_SUCCESS:
      // The MMGSDI_REFRESH_STAGE_END_SUCCESS event arrives after eCall has read
      // its files and has responded with REFRESH_COMPLETE or after an INIT
      // refresh has completed.
      ECALL_MSG_0( ECALL_HIGH, "REFRESH_STAGE_END_SUCCESS" );
      break;

    default:
      ECALL_MSG_1( ECALL_ERROR, "Unhandled refresh stage: %d",
                                 event.data.refresh.stage );
      break;
  }

} /* ECALL_SIM_MMGSDI_REFRESH_EVENT_PROC */

/*==============================================================================
    FUNCTION NAME  ECALL_SIM_MMGSDI_READ_FILE
     
    DESCRIPTION
        This API will call MMGSDI API to read eCall related files.
    
    DEPENDENCIES
        None
    
    INPUT
        None
    
    RETURN VALUE
        None
    
    OUTPUT
        None
==============================================================================*/
void ecall_sim_mmgsdi_read_file
(
  ecall_prov_app_info_s_type *appInfo_ptr
)
{
  mmgsdi_access_type access_type_ust;
  mmgsdi_access_type access_type_est;

  ECALL_MSG_0( ECALL_HIGH, "In ecall_sim_mmgsdi_read_file()" );

  access_type_ust.access_method   = MMGSDI_EF_ENUM_ACCESS;
  access_type_ust.file.file_enum  = MMGSDI_USIM_UST;

  access_type_est.access_method   = MMGSDI_EF_ENUM_ACCESS;
  access_type_est.file.file_enum  = MMGSDI_USIM_EST;

  if( MMGSDI_SUCCESS != mmgsdi_session_read_transparent(appInfo_ptr->session_id,
                                                        access_type_est, 
                                                        0, 
                                                        0, /* Need to read 12 bytes */ 
                                                        (mmgsdi_callback_type) ecall_mmgsdi_read_transparent_est_cb,
                                                        (mmgsdi_client_data_type)appInfo_ptr) )
  {
    ECALL_MSG_0( ECALL_ERROR, "Fail mmgsdi_read_transparent MMGSDI_USIM_EST" );
  }

  if( MMGSDI_SUCCESS != mmgsdi_session_read_transparent(appInfo_ptr->session_id,
                                                        access_type_ust, 
                                                        0, 
                                                        12, /* Need to read 12 bytes */ 
                                                        (mmgsdi_callback_type)ecall_mmgsdi_read_transparent_ust_cb,
                                                        (mmgsdi_client_data_type)appInfo_ptr) )
  { 
    ECALL_MSG_0( ECALL_ERROR, "Fail mmgsdi_read_transparent " );
  }
  
} /* ECALL_SIM_MMGSDI_READ_FILE */

/*==============================================================================
    FUNCTION NAME  ECALL_PROCESS_SIM_EVENTS
     
    DESCRIPTION
        processes the events from MMGSDI client and session API's.
    
    DEPENDENCIES
        None
    
    INPUT
        mmgsdi_event_data_type *event_ptr
    
    RETURN VALUE
        None
    
    OUTPUT
        None
==============================================================================*/
void ecall_process_sim_events
(
  const mmgsdi_event_data_type event
)
{
  ecall_cmd_type             *emodem_cmd_ptr = NULL;
  ecall_prov_app_info_s_type *appInfo_ptr    = NULL;

  switch (event.evt)
  {
    /*if card is removed and inserted INIT COMPL evt generated again
      after card insertion if hotswap is suported*/
    /* Client Event */
    case MMGSDI_CARD_INSERTED_EVT:
      ECALL_MSG_1( ECALL_HIGH, "INFO: MMGSDI_CARD_INSERTED_EVT slot %d",
                                event.data.card_inserted.slot );
      break;

    /* Session Event */
    case MMGSDI_SUBSCRIPTION_READY_EVT:
      /* The only session events have valid session id */
      appInfo_ptr = ecall_sim_get_appinfo_by_session_id(event.session_id);

      if (NULL == appInfo_ptr)
      {
        ECALL_MSG_1( ECALL_HIGH, "Received non-eCall MMGSDI event for non-existent session id 0x%x", event.session_id);
        return;
      }

      ECALL_MSG_5( ECALL_HIGH, "MMGSDI_SUBSCRIPTION_READY_EVT: app type %d->%d, slot 0x%x->0x%x, session_id 0x%x",
                                appInfo_ptr->app_type,
                                event.data.subscription_ready.app_info.app_data.app_type,
                                appInfo_ptr->slot_id,
                                event.data.subscription_ready.app_info.slot,
                                event.session_id );

      appInfo_ptr->app_type          = event.data.subscription_ready.app_info.app_data.app_type;
      appInfo_ptr->slot_id           = event.data.subscription_ready.app_info.slot;
      appInfo_ptr->session_activated = TRUE; // MMGSDI_SESSION_CHANGED_EVT may be missed at startup, READY implies activated

      if ( (ecall_app_task_get_ecall_usim_slot_id() == event.data.subscription_ready.app_info.slot) && 
           (MMGSDI_APP_USIM == event.data.subscription_ready.app_info.app_data.app_type) &&
           (TRUE == ecall_app_task_get_is_era_glonass_enabled()) )
      {
      #ifdef FEATURE_DUAL_SIM
        if ( FALSE == appInfo_ptr->is_available )
        {
          /* For Dual SIM case with as_id not yet mapped.                         *
           * Set the is_pending_send_operating_mode_to_nas flag to TRUE and send  *
           * info to NAS later when as_id is mapped.                              */
          ECALL_MSG_0( ECALL_HIGH, "as_id not yet mapped, will send operating_mode to NAS once as_id is mapped");
          appInfo_ptr->is_pending_send_operating_mode_to_nas = TRUE;
        }
        else
        {
         /* For Dual SIM case with as_id already mapped.     *
          * Queue cmd to IVS and send operating mode to NAS. */
          if ((emodem_cmd_ptr = ecall_get_cmd_buf()) == NULL)
          {
            ECALL_MSG_0( ECALL_ERROR, "No emodem_cmd_ptr buffer in ecall_process_sim_read_success");
            return;
          }
          emodem_cmd_ptr->cmd_id     = ECALL_CMD_SET_AND_SEND_OPERATING_MODE;
          emodem_cmd_ptr->data.as_id = appInfo_ptr->as_id;
          ecall_put_cmd(emodem_cmd_ptr);
        }
      #else /* Single SIM case */
        if ((emodem_cmd_ptr = ecall_get_cmd_buf()) == NULL)
        {
          ECALL_MSG_0( ECALL_ERROR, "No emodem_cmd_ptr buffer in ecall_process_sim_read_success");
          return;
        }
        emodem_cmd_ptr->cmd_id     = ECALL_CMD_SET_AND_SEND_OPERATING_MODE;
        emodem_cmd_ptr->data.as_id = SYS_MODEM_AS_ID_1;
        ecall_put_cmd(emodem_cmd_ptr);
      #endif /* FEATURE_DUAL_SIM */
      }
      else if ( (ecall_app_task_get_ecall_usim_slot_id() == event.data.subscription_ready.app_info.slot) && 
                (MMGSDI_APP_USIM == event.data.subscription_ready.app_info.app_data.app_type) )
      {
        /* Proceed to read MMGSDI Files for Pan-EU eCall */
        ecall_sim_mmgsdi_read_file(appInfo_ptr);
      }
      else if ( (ecall_app_task_get_ecall_usim_slot_id() == event.data.subscription_ready.app_info.slot) && 
                (MMGSDI_APP_USIM != event.data.subscription_ready.app_info.app_data.app_type) )
      {
        ECALL_MSG_1( ECALL_HIGH, "Not a eCall USIM, sim type %d. Set ecall_operation_mode to ECALL_MODE_NORMAL",
                                  event.data.subscription_ready.app_info.app_data.app_type );
        /* Inform NAS with ECALL_MODE_NORMAL if sim card type is not USIM */
      #ifdef FEATURE_DUAL_SIM
        if ( FALSE == appInfo_ptr->is_available )
        {
          /* For Dual SIM case with as_id not yet mapped.                         *
           * Set the is_pending_send_operating_mode_to_nas flag to TRUE and send  *
           * info to NAS later when as_id is mapped.                              */
          ECALL_MSG_0( ECALL_HIGH, "as_id not yet mapped, will send operating_mode to NAS once as_id is mapped");
          appInfo_ptr->is_pending_send_operating_mode_to_nas = TRUE;
        }
        else
        {
         /* For Dual SIM case with as_id already mapped.     *
          * Queue cmd to IVS and send operating mode to NAS. */
          if ((emodem_cmd_ptr = ecall_get_cmd_buf()) == NULL)
          {
            ECALL_MSG_0( ECALL_ERROR, "No emodem_cmd_ptr buffer in ecall_process_sim_read_success");
            return;
          }
          emodem_cmd_ptr->cmd_id     = ECALL_CMD_SET_AND_SEND_OPERATING_MODE;
          emodem_cmd_ptr->data.as_id = appInfo_ptr->as_id;
          ecall_put_cmd(emodem_cmd_ptr);
        }
      #else /* Single SIM case */
        if ((emodem_cmd_ptr = ecall_get_cmd_buf()) == NULL)
        {
          ECALL_MSG_0( ECALL_ERROR, "No emodem_cmd_ptr buffer in ecall_process_sim_read_success");
          return;
        }
        emodem_cmd_ptr->cmd_id     = ECALL_CMD_SET_AND_SEND_OPERATING_MODE;
        emodem_cmd_ptr->data.as_id = SYS_MODEM_AS_ID_1;
        ecall_put_cmd(emodem_cmd_ptr);
      #endif /* FEATURE_DUAL_SIM */
      }
      else
      {
        ECALL_MSG_1( ECALL_HIGH, "Not a eCall related subscription ready event. eCall USIM in slot %d",
                                  ecall_app_task_get_ecall_usim_slot_id() );
      }
      break;
      
    /* Client Event */
    case MMGSDI_CARD_ERROR_EVT:
      ecall_sim_mmgsdi_card_error_event_proc(event.data.card_error.slot);
      break;

    case MMGSDI_SESSION_CHANGED_EVT:
      ecall_sim_mmgsdi_session_changed_event_proc(event);
      break;

    case MMGSDI_REFRESH_EVT:
      ECALL_MSG_0( ECALL_HIGH, "Received MMGSDI_REFRESH_EVT" );
      ecall_sim_mmgsdi_refresh_event_proc(event);
      break;

  #ifdef FEATURE_ECALL_HAS_ERA_GLONASS
    case MMGSDI_PROFILE_DATA_EVT:
      ECALL_MSG_0( ECALL_HIGH, "Received MMGSDI_PROFILE_DATA_EVT" );
      ecall_sim_mmgsdi_profile_data_event_proc(event);
      break;
  #endif /* FEATURE_ECALL_HAS_ERA_GLONASS */

    default:
      break;
  } /*switch (event)*/

} /* ecall_process_sim_events */

/*===========================================================================
FUNCTION ECALL_PROCESS_SIM_CLIENT_ID_REG_SUCCESS

DESCRIPTION
    processes the client id registration and would open the session.

DEPENDENCIES
    None

INPUT
    mmgsdi_client_id_type gsdi_client_id

RETURN VALUE
    None

OUTPUT
   
===========================================================================*/
void ecall_process_sim_client_id_reg_success 
( 
  mmgsdi_client_id_type mmgsdi_client_id
)
{
  mmgsdi_return_enum_type ret;
  ecall_sim_cfg_s_type          *lcfg_s_ptr    = ecall_sim_cfg_s_ptr();

  lcfg_s_ptr->client_id = mmgsdi_client_id;
  
  ECALL_MSG_2( ECALL_HIGH, "ECALL GSDI Client ID U32:0x%X L32:0x%X", 
                            lcfg_s_ptr->client_id>>32,
                            lcfg_s_ptr->client_id );

  /* Session should be opened only once. Airplane mode on/off
     would trigger CARD_INSERTED_EVT */

  ret = mmgsdi_session_open(lcfg_s_ptr->client_id,
                            MMGSDI_GW_PROV_PRI_SESSION,
                            NULL, /* non_prov_app_data_ptr */
                            ecall_sim_mmgsdi_event_cb,
                            FALSE, /* set_notify */
                            ecall_sim_mmgsdi_session_open_cb,
                            (mmgsdi_client_data_type) MMGSDI_GW_PROV_PRI_SESSION);

  if( MMGSDI_SUCCESS != ret )
  {
    ECALL_MSG_1( ECALL_ERROR, "Failed to open MMGSDI GW PRI session: error %d",
                               ret );
  }

#ifdef FEATURE_DUAL_SIM
  ret = mmgsdi_session_open(lcfg_s_ptr->client_id,
                            MMGSDI_GW_PROV_SEC_SESSION,
                            NULL, /* non_prov_app_data_ptr */
                            ecall_sim_mmgsdi_event_cb,
                            FALSE, /* set_notify */
                            ecall_sim_mmgsdi_session_open_cb,
                            (mmgsdi_client_data_type) MMGSDI_GW_PROV_SEC_SESSION);

  if( MMGSDI_SUCCESS != ret )
  {
    ECALL_MSG_1( ECALL_ERROR, "Failed to open MMGSDI GW SEC session: error %d",
                               ret );
  }
#endif /* FEATURE_DUAL_SIM */

} /* ecall_process_sim_client_id_reg_success */

/*===========================================================================

FUNCTION ECALL_SIM_MMGSDI_CLIENT_ID_REG_CB

DESCRIPTION
    This function is the callback function of client_id and event
    registration. used to retrieve client id.

DEPENDENCIES
    None

INPUT
    mmgsdi_cnf_type* cnf_data_ptr

RETURN VALUE
    None

OUTPUT
   
===========================================================================*/
LOCAL void ecall_sim_mmgsdi_client_id_reg_cb
(
  mmgsdi_return_enum_type   status, 
  mmgsdi_cnf_enum_type      cnf,
  const mmgsdi_cnf_type    *cnf_ptr
)
{
  ecall_cmd_type *emodem_cmd_ptr = NULL;

  if (cnf_ptr == NULL)
  {
     ECALL_MSG_0( ECALL_ERROR, "MMGSDI client_id_and_evt reg cnf_ptr as NULL" );
     return;
  }

  if (status == MMGSDI_SUCCESS)
  {
    if (cnf == MMGSDI_CLIENT_ID_AND_EVT_REG_CNF)
    {
             
      if((emodem_cmd_ptr = ecall_get_cmd_buf())!= NULL)
      {
        emodem_cmd_ptr->data.gsdi_client_id = cnf_ptr->client_id_and_evt_reg_cnf.response_header.client_id;
        emodem_cmd_ptr->cmd_id = ECALL_CMD_SIM_CLIENT_ID_REG_SUCCESS;
        ecall_put_cmd(emodem_cmd_ptr);
      }
      else
      {
        ECALL_MSG_0( ECALL_ERROR, "No emodem_cmd_ptr buffer to register for sesion" );
      }
    } 
  } /* MMGSDI_SUCCESS */
  else
  {
    ECALL_MSG_1( ECALL_ERROR, "ecall_mmgsdi_client_id_and_evt_resp_cb(): Client ID Registration Failed [%d]", 
                               status );
  }
} /* ECALL_SIM_MMGSDI_CLIENT_ID_REG_CB */

/*===========================================================================

FUNCTION ECALL_SIM_MMGSDI_INIT

DESCRIPTION
    This function calls the API to get a client ID .

DEPENDENCIES
    None

INPUT
    mmgsdi_cnf_type* cnf_data_ptr

RETURN VALUE
    None

OUTPUT
   
===========================================================================*/
void ecall_sim_mmgsdi_init
(
  void 
)
{
  mmgsdi_return_enum_type mmgsdi_status = MMGSDI_SUCCESS;
   
  /* Register the MMGSDI event callback function */
  /* This function allows client registration for new event callback
     that will contain additional information based on specific events being sent.
     The response to this request will return the client ID to the calling task.
  */
  mmgsdi_status = mmgsdi_client_id_and_evt_reg(ecall_sim_mmgsdi_event_cb, /* mmgsdi_evt_callback_type */
                                               ecall_sim_mmgsdi_client_id_reg_cb, /* mmgsdi_callback_type */
                                               (mmgsdi_client_data_type)NULL);

  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    ECALL_MSG_1( ECALL_ERROR, "MMGSDI Registration failed : STATUS %d",
                                mmgsdi_status );
  }
  else
  {
    ECALL_MSG_0( ECALL_MED, "Reg called MMGSDI to register for client ID" );
  }

} /* ecall_sim_mmgsdi_init */


/*==========================================================================

  FUNCTION ECALL_GET_ECALL_OPERATING_MODE

  DESCRIPTION
    This API will read the service bit 89 from the SIM to find out if the eCall
    modem is ECALL ONLY or ECALL NORMAL. This information is then sent to NAS,
    for the EIP procedure. And also the the ECALL APP in the form of callback,
    so APP is aware of the ecall modem config state.

  DEPENDENCIES
    None.

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
ecall_operating_mode_type ecall_get_ecall_operating_mode
( 
  void
)
{
  ecall_prov_app_info_s_type *appInfo_ptr = NULL;
  mmgsdi_slot_id_enum_type    slot_id = (mmgsdi_slot_id_enum_type)0;

  slot_id = ecall_app_task_get_ecall_usim_slot_id();

  if (slot_id == (mmgsdi_slot_id_enum_type)0)
  {
    ECALL_MSG_1( ECALL_ERROR, "get_ecall_operating_mode: Invalid slot_id %d",
                               slot_id );
    return ECALL_MODE_UNKNOWN;
  }
  
  appInfo_ptr = ecall_sim_get_appinfo_by_slot_id(slot_id);

  if (NULL == appInfo_ptr)
  {
    ECALL_MSG_1( ECALL_ERROR, "get_ecall_operating_mode: Unmapped slot_id %d",
                               slot_id );
    return ECALL_MODE_UNKNOWN;
  }

  ECALL_MSG_2( ECALL_MED, "get_ecall_operating_mode: ecall_operating_mode %d, slot_id %d",
                            appInfo_ptr->ecall_operating_mode,
                            appInfo_ptr->slot_id );

  return appInfo_ptr->ecall_operating_mode;
} /* ECALL_GET_ECALL_OPERATING_MODE */

/*==========================================================================

  FUNCTION ECALL_GET_ECALL_OPERATING_MODE_PER_SUBS

  DESCRIPTION
    This API will read the service bit 89 from the SIM to find out if the eCall
    modem is ECALL ONLY or ECALL NORMAL. This information is then sent to NAS,
    for the EIP procedure. And also the the ECALL APP in the form of callback,
    so APP is aware of the ecall modem config state.

  DEPENDENCIES
    None.

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
ecall_operating_mode_type ecall_get_ecall_operating_mode_per_subs
( 
  sys_modem_as_id_e_type as_id
)
{
  ecall_prov_app_info_s_type *appInfo_ptr = NULL;

  if ( (SYS_MODEM_AS_ID_NONE >= as_id) &&
       ((int)as_id >= MAX_AS_IDS) )
  {
    ECALL_MSG_1( ECALL_ERROR, "get_ecall_operating_mode_per_subs(): Invalid as_id %d",
                               as_id );
    return ECALL_MODE_UNKNOWN;
  }
  
  appInfo_ptr = ecall_sim_get_appinfo_by_as_id(as_id);

  if (NULL == appInfo_ptr)
  {
    ECALL_MSG_1( ECALL_ERROR, "get_ecall_operating_mode_per_subs(): Unmapped as_id %d",
                               as_id );
    return ECALL_MODE_UNKNOWN;
  }

  ECALL_MSG_3( ECALL_MED, "get_ecall_operating_mode_per_subs(): ecall_operating_mode %d, slot_id %d, as_id %d",
                            appInfo_ptr->ecall_operating_mode,
                            appInfo_ptr->slot_id,
                            appInfo_ptr->as_id );

  return appInfo_ptr->ecall_operating_mode;
} /* ECALL_GET_ECALL_OPERATING_MODE_PER_SUBS */

/*===========================================================================
   FUNCTION:      ECALL_SIM_PROCESS_SESSION_OPEN_SUCCESS

   DESCRIPTION:
     This function will set ecall_sim global ecall_session_id to the
     passed in data.

   DEPENDENCIES:
     None

   LIMITATIONS:
     Parameter check for specific commands are not performed

   RETURN VALUE:
     None

   SIDE EFFECTS:
     None
===========================================================================*/
void ecall_sim_process_session_open_success
(
  mmgsdi_session_type_enum_type   session_type,
  mmgsdi_session_id_type          session_id,
  mmgsdi_slot_id_enum_type        slot_id,
  mmgsdi_app_enum_type            app_type
)
{
  ecall_prov_app_info_s_type *appInfo_ptr;

  appInfo_ptr = ecall_sim_get_appinfo_by_session_type(session_type);

  if (NULL != appInfo_ptr)
  {
    appInfo_ptr->session_id              = session_id;
    appInfo_ptr->slot_id                 = slot_id;
    appInfo_ptr->session_type            = session_type;
    appInfo_ptr->app_type                = app_type;

    ECALL_MSG_3( ECALL_HIGH, "MMGSDI session_type %d open was success with session_id 0x%x on slot_id %d, proceed to register for refresh",
                              appInfo_ptr->session_type,
                              appInfo_ptr->session_id,
                              appInfo_ptr->slot_id );

    /* eCall register for MMGSDI REFRESH if app_type is USIM */
    ecall_sim_mmgsdi_register_for_refresh( appInfo_ptr->session_id );
  }

}/* ECALL_SIM_PROCESS_SESSION_OPEN_SUCCESS */

/*===========================================================================
   FUNCTION:      ECALL_SIM_SET_EST_FDN_ENABLED

   DESCRIPTION:
     This function will set ecall_sim global ecall_est_fdn_enabled to the
     passed in data.

   DEPENDENCIES:
     None

   LIMITATIONS:
     Parameter check for specific commands are not performed

   RETURN VALUE:
     None

   SIDE EFFECTS:
     None
===========================================================================*/
void ecall_sim_set_est_fdn_enabled
(
  const uint8                 est_fdn,
  ecall_prov_app_info_s_type *appInfo_ptr
)
{
  if (NULL == appInfo_ptr)
  {
    ECALL_MSG_0( ECALL_ERROR, "sim_set_est_fdn_enabled(): Null appInfo_ptr" );
    return;
  }

  appInfo_ptr->is_est_fdn_enabled = est_fdn;

  ECALL_MSG_2( ECALL_MED, "Set ecall_est_fdn_enabled to %d for session_id 0x%x",
                           appInfo_ptr->is_est_fdn_enabled,
                           appInfo_ptr->session_id );
} /* ECALL_SIM_SET_EST_FDN_ENABLED */

#else

void ecall_sim_dummy_func(void);

#endif /* End Featurisation */

