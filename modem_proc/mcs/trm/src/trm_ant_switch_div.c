
/*!
  @file
  ant_switch_div.c
 
  @brief
 
  Ant Switch Diversity apis are used for antenna arbitration and switch state
  maintenence. For non-SV(ATnT), TRM is used to just maintain switch state
  over L1 sleep cycles. For SV, TRM acts like a switch arbiter.
 
  */

/*===========================================================================

  Copyright (c) 2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR FILE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/trm/src/trm_ant_switch_div.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/20/15   sk      Workaround for G calling cancellation in callback (755230)
11/14/14   sk      Added switch diag events(748395)
11/14/14   sk      Fix is_switching_allowed logic (755230)
10/28/14   sk      fix KW issue
10/18/14   sk      support for DRDSDS ASDiv(707105)
10/10/14   ag      Added interface for tech state update to asdiv
07/15/14   sk      Consider all higher priority techs before making swithcing
		   			permission (682228)
07/15/14   sk      update preferred config based on NV default value 
06/19/14   sk      Update DSDS mode in DSDS fallback scenario(CR:681600)
06/17/14   sk      Fixed compilation error due to incorrect id access
04/08/14   sk      Fixed mob compilation error
04/08/14   sk      Correct logic to add/remove clients in active tech list
03/20/14   sk      Add SVLTE+G DSDA ASDiv support, new api to block switching
		    		block switching if there is another tech in acq or idle
02/28/14   sk      Check switch permission only if current config is different
					for idlewakeup case
02/06/14   sk      report switch initiation to passive tx tech in dual tx
01/15/14   sk      kw error fix
01/15/14   sk      DSDA ASDiv support
05/24/13   sk      Initial Rev

===========================================================================*/


/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <msg.h>
#include <err.h>
#include <event.h>

#include "modem_mcs_defs.h"

#include "fs_sys_types.h"
#include "fs_public.h"
#include "fs_errno.h"

#include "sys.h"
#include "stringl.h"

#include "trm.h"
#include "trmi.h"
#include "subs_prio.h"


/*===========================================================================

                         CONSTANTS, MACRO DEFINITIONS

===========================================================================*/


/*! BIT Manipulation macros */
/*set specific bit of a 32 bit unsigned flag using bit position*/
#define  SET_BIT_POS(flag, pos) (flag |= ((uint32)(0x1<<pos)))

/*clear specific bit of a 32 bit unsigned flag using bit position*/
#define  CLR_BIT_POS(flag, pos) ( flag &= ( ~((uint32)(0x1<<pos)) ))

/*check specific bit of a 32 bit unsigned flag using bit position*/
#define  CHECK_BIT_POS(flag, pos) ( (flag) & ((uint32)(0x1<<pos)))

#define ASDIV_MAX_CLIENT		TRM_LAST_CLIENT+1

/*!BIT position definitions of CONFIGURATION REGISTER, 
  These are set/reset at the time of initialization */


#define BIT_FEATURE_SUPPORTED			31

#define SWITCH_IN_TRANSITION	TRM_ANT_SWITCH_DIV_CONFIG_IN_TRANSITION

#define ASDIV_INVALID_DWELLING_TIME 0

/*! Other utility Macros */

/* Macros for initializing, entering and exiting critcal section */
#define ASDIV_INIT_CRIT_SECT(lock)    rex_init_crit_sect(&(lock))
#define ASDIV_ENTER_CRIT_SECT(lock)   rex_enter_crit_sect(&(lock))
#define ASDIV_EXIT_CRIT_SECT(lock)    rex_leave_crit_sect(&(lock))

/* check if antenna switch feature is supported by the HW */
#define TRM_ASDIV_IS_FEATURE_SUPPORTED     (as_div.antenna_swap_supported)

#define ASD_SWITCH_IN_TRANSITION     ( as_div.curr_setting.configuration ==\
					  SWITCH_IN_TRANSITION )

/* This Macro should be used to null check interfaces.  If a Void return
   value is desired, then pass in the RETURN_VOID macro */
#define ASD_NULL_PTR_CHK(ptr, retval)                                \
  if (ptr == NULL)                                                       \
  {                                                                      \
    MSG(MSG_SSID_DFLT, MSG_LEGACY_ERROR, "Unexpected Null Ptr"); \
    { return retval; }        \
  }

#define RETURN_VOID /* null macro */

/* efs file paths */
#define TRM_CONF_EFS_FILE         "/nv/item_files/conf/mcs_as_div.conf"
#define ANT_SWITCH_DIV_EFS_PATH   "/nv/item_files/mcs/trm/ant_div_switch_init"

/* DSDA changes */
#define TRM_ASDIV_DEFAULT_COL_LIMIT 2

#define TRM_ASDIV_SUB_STATE_SHIFT   23
#define TRM_ASDIV_TECH_MODE_SHIFT   15
#define TRM_ASDIV_MODE_TIER_SHIFT   7
#define TRM_ASDIV_SUB_PRIO_SHIFT    0
#define ASDIV_RST_COL_CNT_MASK      ( ~((uint32) ( 1<<TRM_CLIENT_MODE_IDLE_WAKEUP)|\
                                        ( 1<<TRM_CLIENT_MODE_SLEEP) ) )

#define ASDIV_IS_SINGLE_TECH_ID(tech_id)  ( (tech_id == TRM_GSM2) || (tech_id == TRM_GSM3) )

#define TRM_ASDIV_IS_TECH_VALID(tech_id) ( tech_id < ASDIV_MAX_CLIENT )
#define TRM_ASDIV_IS_SUBID_VALID(sub_id) ( sub_id < MAX_NUM_OF_SUBSCRIPTIONS && \
                                           sub_id > SYS_MODEM_AS_ID_NONE )

#define TRM_ASDIV_IS_TRAFFIC_MODE(mode) ( mode == TRM_CLIENT_MODE_RXTX || \
                                          mode == TRM_CLIENT_MODE_RXTX_LOW )

#define TRM_ASDIV_IS_IDLE_MODE(mode) ( mode == TRM_CLIENT_MODE_IDLE_WAKEUP || \
                                       mode == TRM_CLIENT_MODE_RXACQ )

#define TRM_ASDIV_OPP_CONFIG(config) (( (config) == TRM_ANT_SWITCH_DIV_CONFIG_0 )? \
                                       TRM_ANT_SWITCH_DIV_CONFIG_1 : TRM_ANT_SWITCH_DIV_CONFIG_0)
typedef enum
{
  TRM_ASDIV_MODE_TIER_NORMAL = 0x00,
  TRM_ASDIV_MODE_TIER_SLEEP_GSM1,
  TRM_ASDIV_MODE_TIER_SLEEP_GSM2,
  TRM_ASDIV_MODE_TIER_SLEEP_1X
}trm_asdiv_mode_tier;

/*----------------------------------------------------------------------------
  TRM Asdiv idle idle col info. Idle/Idle Collision information
----------------------------------------------------------------------------*/
typedef struct {

  /* idle idle collision limit */
  uint8                            idle_idle_col_limit;
  /* flag to indicate if limit has reached */
  boolean                          limit_reached;
  /* flag to indicate if collision was for idle/idle case */
  boolean                          is_idl_idl_col;
  /* client that was denied pref config due to colision */
  trm_client_enum_t                denied_cl;
  /* preferred config of denied client */
  trm_ant_switch_div_config_type   denied_config;
  /* count for successiv idle/idle collsion */
  int                              col_count;

}trm_asdiv_idl_idl_col_info;



typedef struct 
{
   /* indicate whether techs are in safe mode
       control_release: safe mode
       control request: not in safe mode */
   trm_ant_switch_set_ctrl_enum  sw_control;

   /* indicate whether crisis mode switching request from 
      low priority client can be allowed in SLEEP not safe mode */
   boolean     allow_crisis_mode_switching;

}trm_asdiv_safe_state_data;
/*----------------------------------------------------------------------------
  Ant Switch active Client list. List of active clients
----------------------------------------------------------------------------*/
typedef struct {

  /* client id */
  trm_client_enum_t          cl_id[ASDIV_MAX_CLIENT];

  /* number of active clients in the list */
  uint32                     num_clients;

  /* Bit mask for modes of all active tech */
  uint32                     active_mode_mask;

  /* active client that has highest priority */
  trm_client_enum_t          high_prio_cl;

  /* highest priority mask */
  uint32                     high_prio_mask;

}trm_asdiv_active_cl_list;

typedef struct
{
   trm_ant_switch_div_config_type switch_config;

   rfm_device_enum_type    associated_rf_dev;

}trm_asdiv_preferred_config_data;

/*----------------------------------------------------------------------------
  TRM ASDiv Event Type: Switching events type
----------------------------------------------------------------------------*/
typedef enum
{
  /* Event due to switch cancel */
  TRM_ASDIV_EVENT_SWITCHING_CANCEL,

  /* Event due to switching in idle wakeup */
  TRM_ASDIV_EVENT_SWITCHING_IDLEWAKEUP,

  /* Event due to switching in other modes */
  TRM_ASDIV_EVENT_SWITCHING_OTHERS

}trm_asdiv_event_type;

/*----------------------------------------------------------------------------
  TRM ASDiv Event Info Type: Switching events information structure
----------------------------------------------------------------------------*/
typedef struct
{
   uint8                   client_id;
   uint8                   old_switch_config;
   uint8                   new_switch_config;
   trm_asdiv_event_type    event_type;
   uint8    reserved1;
   uint8    reserved2;
   uint16   reserved3;
   uint32   reserved4;
}trm_asdiv_event_info_type;

/*----------------------------------------------------------------------------
  Ant Switch Client Structure. Client specific attributes
----------------------------------------------------------------------------*/
typedef struct {

  /* anonymous client's data that is echoed back when mode is granted */
  trm_request_tag_t       				tag;

  /* Client's id */
  trm_client_enum_t     				id;

  /* the mode in which the client it is in */
  trm_client_mode_type                	curr_mode;

  /* requested mode */
  trm_client_mode_type                	req_mode;

  /* last requested configuration by the client */
  trm_ant_switch_div_config_type      	req_config;

  /* NOT USED */
  trm_ant_switch_subcribe_switch_info 	subs_sw_info;

  /* for bw compatibility, can be removed once L1s use 
    subscribing for swithc info*/
  boolean 								ntfy_switch_change;

  /* inform the client if mode of any tech changes */
  boolean   							ntfy_mode_change;

  /* indicates if ant switch is enabled or not */
  trm_ant_switch_state_type         	ant_switch_state;

  /* set mode callback: called to grant the mode request */
  trm_ant_switch_set_mode_callback_t  	set_mode_cb;

  /* ant switch call-back..called when we want the client to
     actively switch */
  trm_ant_switch_div_callback_t      	ant_switch_cb;

  /* ant switch call-back: called when the mode chane occurs */
  trm_ant_switch_mode_notification_callback_t  mode_change_cb;

  /* DSDA ASDiv changes */

  /* preferred switch configuration of client */
  trm_asdiv_preferred_config_data      pref_config_info;
  //trm_ant_switch_div_config_type          pref_config;

  /* Switch control: held if not in safe mode
                     release in safe mode */
 // trm_ant_switch_set_ctrl_enum            sw_control;

  trm_asdiv_safe_state_data    safe_state_info;

  /* call back for set mode and config call */
  trm_asd_set_mode_and_config_cb_t        set_mode_cfg_cb;

  /* corresponding sys mode id */
  sys_sys_mode_e_type      sys_tech_id;

    /* subscription sys id */
  sys_modem_as_id_e_type  sub_id;

  /* priority of subscription */
  uint8                   sub_order_idx;

  /* priority mask for the client */
  uint32                  priority_mask;

  uint32                  switch_lock_mask;

  boolean                 switch_start_reported;
 
}ant_switch_client_struct;


/*----------------------------------------------------------------------------
  Ant Switch Diversity Structure. Overall ASdiv attributes
----------------------------------------------------------------------------*/
typedef struct
{
  /* */
   boolean                              antenna_swap_supported;

  /* Configuration Register, holds initial setting */
  uint32				config_reg;
  /* Status REgister, holds current information */
  uint32              			stat_flag;
  /* Critical section - for antenna arbitration. */
  rex_crit_sect_type              	crit_sect;
  /* asdiv initial settings from NV */
  trm_ant_switch_div_data_type    	init_setting;
  /* asdiv current switch related attribute */
  trm_ant_switch_div_data_type    	curr_setting;
  /* aggregate client mode */
  //agg_client_mode_type            	aggregate_client_mode;
  /* stores the switch config before switch is moved to transition */
  trm_ant_switch_div_config_type	prev_config;
  /* stores expected switch config while switching */
  trm_ant_switch_div_config_type	next_config;
  /* Client array depending upon the number of clients */
  ant_switch_client_struct        	client[ASDIV_MAX_CLIENT];

  /* DSDA changes */
  /* list of active clients */
  trm_asdiv_active_cl_list              active_list;
  /* idle idle collision data */
  trm_asdiv_idl_idl_col_info            idl_idl_col_data;

  uint32               lock_mask;

  /* flag to indicate if sleep mode priority should be ignore:
         valid for DSDS case only where high priority sleep tech
         shouldn't block other tecs from switching */
  boolean              ignore_high_prio_sleep;

  /* tech state info updated by limitsmgr */
  trm_asdiv_tech_state_info_type       tech_state_info;

  /* tech that is doing the switching */
  trm_client_enum_t    switching_tech;

  /* tech that is requesting for switching,
     it is same as switching tech in most case except for passive switching */
  trm_client_enum_t    requesting_tech;

  /* diag event info for switching events */
  trm_asdiv_event_info_type      event_info;
  
}ant_switch_div_struct;

/* global ant switch structure */
ant_switch_div_struct     as_div;



/*===========================================================================

                         FUNCTIONS

===========================================================================*/

/* Funtion Declarations */

void trm_asdiv_update_actv_list_tech_info(
   trm_client_enum_t    cl
   );

void trm_asdiv_update_actv_list_prio_info(void);

//boolean trm_asdiv_chk_switch_perm(trm_client_enum_t cl_id);



/*============================================================================
FUNCTION TRM_EFS_CREATE_FOLDER

DESCRIPTION
  Creates EFS Path for the conf file it is doesn't exist.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
static void trm_asdiv_create_efs_folder(void)
{
  int ret;

/*--------------------------------------------------------------------------*/
  /* Create mcs folder if it doesnt already exist */
  ret = efs_mkdir("/nv/item_files/mcs", 0x777);
  if (ret != 0 && efs_errno != EEXIST)
  {
    MSG_1(MSG_SSID_DFLT, MSG_LEGACY_FATAL, "Error %d creating mcs folder in efs", efs_errno);
    return ;
  }

  /* Create trm folder if it doesnt already exist */
  ret = efs_mkdir("/nv/item_files/mcs/trm", 0x777);
  if (ret != 0 && efs_errno != EEXIST)
  {
    MSG_1(MSG_SSID_DFLT, MSG_LEGACY_FATAL, "Error %d creating trm folder in efs", efs_errno);
    return ;
  }  
}


/*============================================================================

FUNCTION trm_asdiv_update_conf_file

DESCRIPTION
  Update the conf file with nv file location.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
void trm_asdiv_update_conf_file(void)
{

  /* File Descriptor */
  int32                 fd;
  struct 	fs_stat 	conf_file_det;
  size_t         		data_size;
  int            		file_stat_result;

  char str[] = ANT_SWITCH_DIV_EFS_PATH "\n";

/*--------------------------------------------------------------------------*/


  file_stat_result = efs_stat(TRM_CONF_EFS_FILE, &conf_file_det);
  data_size = (size_t)strlen(str);

  /* If the conf file does not exist or if the size of the existing conf file is
     not what we expect, create a new conf file */
  if (file_stat_result != 0 || data_size != conf_file_det.st_size)
  {

    trm_asdiv_create_efs_folder();

    /* Open the TRM CONF EFS file. If its not present then create it. */
    fd = efs_open(TRM_CONF_EFS_FILE, O_CREAT | O_AUTODIR | O_WRONLY | O_TRUNC, ALLPERMS);

	  if (fd < 0)
	  {
		  MSG_1(MSG_SSID_DFLT, MSG_LEGACY_FATAL, "Error opening EFS file %d", fd);
		  return;
	  }

    /* Write the conf file */
    (void)efs_write(fd, str, (size_t)strlen(str));

    efs_close(fd);
  }

}


/*============================================================================

FUNCTION trm_asdiv_is_efs_item_valid

DESCRIPTION
  Validates the data read from the tx div switch item file.
  
DEPENDENCIES
  None

RETURN VALUE
  True : Valid.
  False: Invalid.

SIDE EFFECTS
  None

============================================================================*/
boolean trm_asdiv_is_efs_item_valid
(
   const trm_ant_switch_div_data_type ant_switch_div_data
)
{

  if (ant_switch_div_data.configuration >= 0 &&
      ant_switch_div_data.configuration <= 1)
  {
    return TRUE;
  }

  return FALSE;
}


/*============================================================================

FUNCTION trm_asdiv_read_efs_data

DESCRIPTION
  Read the EFS File to get NV settings.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
void trm_asdiv_read_efs_data(void)
{

  int result = 0;

/*--------------------------------------------------------------------------*/

  /* Open the EFS file. If its not present then create it els read it. */
  result = efs_get(ANT_SWITCH_DIV_EFS_PATH, (void *)&as_div.init_setting,
                   sizeof(trm_ant_switch_div_data_type));

  /* If read passed, check if we read the expected number of bytes */
  if (result != sizeof(trm_ant_switch_div_data_type))
  {
    MSG_3(MSG_SSID_DFLT, MSG_LEGACY_FATAL,
	  "Could not read settings from EFS. result: %d, efs_errno %d, size %d", 
          result,efs_errno,sizeof(trm_ant_switch_div_data_type));
    return;
  }

  /* if efs read values are not valid, initialize to default values */
  if (!trm_asdiv_is_efs_item_valid(as_div.init_setting))
  {
	  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_FATAL,
             "Invalid settings read from EFS: configuration - %d ", 
             as_div.init_setting.configuration);
	  as_div.init_setting.dwelling_time  = 0;
	  as_div.init_setting.test_mode_ctrl = TYPICAL;
	  as_div.init_setting.configuration  = TRM_ANT_SWITCH_DIV_CONFIG_0;
	  as_div.init_setting.sv_default_configuration = TRM_ANT_SWITCH_DIV_CONFIG_0;
	  as_div.init_setting.sv_behavior    = TRM_ANT_SWITCH_SV_KEEP_IDLE_DEF;
  }

  /* transfer the init values to the current variables */
  as_div.curr_setting.configuration  = as_div.init_setting.configuration;
  as_div.curr_setting.test_mode_ctrl = as_div.init_setting.test_mode_ctrl;
  as_div.curr_setting.dwelling_time  = as_div.init_setting.dwelling_time;

}

/*============================================================================

FUNCTION ANT_SWITCH_DIV_MUTEX_INIT

DESCRIPTION
  Initializes the asdiv mutex, TRM needs to call this api first to initialize
  the asdiv mutex
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
void trm_asdiv_mutex_init()
{
  /* mutex should be initialized before L1s are active, so before
      initialization of other variables, this api is called by
      TRM to initialize the mutex */
  ASDIV_INIT_CRIT_SECT(as_div.crit_sect);

  /* reset configuration register */
  //as_div.config_reg = 0;

}

/*============================================================================

FUNCTION TRM_ASDIV_GET_SYS_MODE_TYPE

DESCRIPTION
  Gets the sys id for trm client id
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/

sys_sys_mode_e_type  trm_asdiv_get_sys_mode_type(trm_client_enum_t cl_id)
{
  switch (cl_id)
  {

  case TRM_1X:
    return SYS_SYS_MODE_CDMA;
  case TRM_HDR:
    return SYS_SYS_MODE_HDR;
  case TRM_WCDMA:
    return SYS_SYS_MODE_WCDMA;
  case TRM_GSM1:
  case TRM_GSM2:
  case TRM_GSM3:
    return SYS_SYS_MODE_GSM;
  case TRM_TDSCDMA:
    return SYS_SYS_MODE_TDS;
  case TRM_LTE:
    return SYS_SYS_MODE_LTE;
  default:
    return SYS_SYS_MODE_MAX;

  }

}
/*============================================================================

FUNCTION ANT_SWITCH_DIV_INIT

DESCRIPTION
  Initializes the asdiv variables to default, reads efs item to get the
  default values
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/

void trm_asdiv_init
(
   boolean           feature_supported,
   uint8             trm_mode,
   trm_slte_bitmask  dr_mask
)
{

  uint8   index;

  ASDIV_ENTER_CRIT_SECT(as_div.crit_sect);

  //memset(&as_div, 0, sizeof(ant_switch_div_struct));

  /* assign feature supported bit with passed argument */
  if(feature_supported) 
  {
    as_div.antenna_swap_supported = TRUE;
  }


  /* initialize the ant switch div variables
      if h/w is supported */
  if (TRM_ASDIV_IS_FEATURE_SUPPORTED)
  {
    as_div.stat_flag = 0;

    as_div.init_setting.dwelling_time  = 0;
    as_div.init_setting.test_mode_ctrl = TYPICAL;
    as_div.init_setting.configuration  = TRM_ANT_SWITCH_DIV_CONFIG_0;
    as_div.init_setting.sv_default_configuration = TRM_ANT_SWITCH_DIV_CONFIG_0;
    as_div.init_setting.sv_behavior    = TRM_ANT_SWITCH_SV_KEEP_IDLE_KEEP;

    //as_div.aggregate_client_mode         = 0;
    as_div.curr_setting.configuration    = TRM_ANT_SWITCH_DIV_CONFIG_0;

    as_div.lock_mask = 0;

    /* initialize client specific data */
    for (index = 0; index <= TRM_LAST_CLIENT; index++)
    {
      as_div.client[index].id	           = (trm_client_enum_t)index;
      as_div.client[index].curr_mode       = TRM_CLIENT_MODE_INACTIVE;
      as_div.client[index].req_mode        = TRM_CLIENT_MODE_INACTIVE;
      as_div.client[index].req_config      = TRM_ANT_SWITCH_DIV_CONFIG_MAX;

      as_div.client[index].ant_switch_state    = TRM_ANT_SWITCH_DIV_ENABLED;

      as_div.client[index].subs_sw_info		  = TRM_SUBS_NONE;
      as_div.client[index].ntfy_mode_change   = FALSE;
      as_div.client[index].ntfy_switch_change  = FALSE;

      as_div.client[index].set_mode_cb         = NULL;
      as_div.client[index].ant_switch_cb       = NULL;
      as_div.client[index].mode_change_cb      = NULL;

      as_div.client[index].sys_tech_id = trm_asdiv_get_sys_mode_type(as_div.client[index].id);

      as_div.client[index].safe_state_info.sw_control = TRM_ANT_SWITCH_CTRL_RELEASE;

      as_div.client[index].switch_lock_mask = 0;

      as_div.client[index].switch_start_reported = FALSE;


    }

    as_div.active_list.num_clients = 0;
    as_div.idl_idl_col_data.denied_cl = TRM_NO_CLIENT;
    as_div.idl_idl_col_data.idle_idle_col_limit = TRM_ASDIV_DEFAULT_COL_LIMIT;
 
    as_div.switching_tech = TRM_NO_CLIENT;
    as_div.requesting_tech = TRM_NO_CLIENT;

#ifndef TEST_FRAMEWORK

    /* read and initialize from EFS file */
    trm_asdiv_update_conf_file();
    trm_asdiv_read_efs_data();

#endif

    /* update the preferred configuration of tech with default configuration */
     for (index = 0; index <= TRM_LAST_CLIENT; index++)
     {
       as_div.client[index].pref_config_info.switch_config = as_div.init_setting.configuration;
       as_div.client[index].pref_config_info.associated_rf_dev = RFM_DEVICE_0;
     }
    
     /* check to see if high priority sleep tech should be ignored
        for switching decision, currently only for SRDSDS, SLTE, SHDR only cases */
     as_div.ignore_high_prio_sleep = FALSE;

     if ( trm_mode == 0 &&
          !(dr_mask & TRM_DR_DSDS_ENABLED) ) 
     {
        as_div.ignore_high_prio_sleep = TRUE;
     }

  }

  ASDIV_EXIT_CRIT_SECT(as_div.crit_sect);

}

/*============================================================================

FUNCTION TRM_ASDIV_IS_ANTENNA_SWITCH_SUPPORTED

DESCRIPTION
  Indicate if RF HW supports antenna switching or not
  
DEPENDENCIES
  None

RETURN VALUE
  True  : if HW supports antenna switching. 
  False : otherwise. 

SIDE EFFECTS
  None

============================================================================*/
boolean trm_asdiv_is_antenna_switch_supported(void)
{
  if (as_div.antenna_swap_supported)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

/*============================================================================

FUNCTION TRM_ANT_SWITCH_DIV_GET_CONFIG

DESCRIPTION
  Get the current tx diversity switch configuration.
  
DEPENDENCIES
  None

RETURN VALUE
  Current tx diversity switch configuration:
    config_0 or config_1 or in_transition

SIDE EFFECTS
  None

============================================================================*/
trm_ant_switch_div_config_type trm_ant_switch_div_get_config
(
  trm_ant_switch_get_input_type input_type
)
{

  trm_ant_switch_div_config_type return_val;

  ASDIV_ENTER_CRIT_SECT(as_div.crit_sect);

  /* return invalid config if feature is not supported */
  if (!TRM_ASDIV_IS_FEATURE_SUPPORTED)
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
         "TRM_ASDiv: Feature not supported" );
    return_val = TRM_ANT_SWITCH_DIV_CONFIG_MAX;
  }
  else if (input_type == TRM_ANT_SWITCH_CONFIG_DEFAULT)
  {
    /* Default config should be provided always */
    return_val = as_div.init_setting.configuration;
  }
  else
  {
    return_val = as_div.curr_setting.configuration;
  }

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
         "TRM_ADiv: get config; req:%d cfg_ret:%d",
         input_type, return_val);

  ASDIV_EXIT_CRIT_SECT(as_div.crit_sect);
  return return_val;

} /* trm_get_ant_switch_div_config */


/*============================================================================

FUNCTION trm_asdiv_get_tx_client

DESCRIPTION
  Returns the first client which is under transmit.
  
DEPENDENCIES
  None

RETURN VALUE
  Client which is under RxTx; if there is one.

SIDE EFFECTS
  None

============================================================================*/
trm_client_enum_t trm_asdiv_get_tx_client
(
  trm_client_enum_t requesting_client
)
{
  trm_client_enum_t tx_client = TRM_NO_CLIENT;
  trm_client_enum_t other_client;
  uint8 loopcnt = 0;
/*--------------------------------------------------------------------------*/

  for (loopcnt = 0; loopcnt < as_div.active_list.num_clients; ++loopcnt)
  {
    other_client = as_div.active_list.cl_id[loopcnt];
    /* ignore the current client */
    if ( requesting_client == other_client )
    {
      continue;
    }
    /* Client mode is rxtx and hence is the client switching */
    if ( TRM_ASDIV_IS_TECH_VALID(other_client) &&
         TRM_ASDIV_IS_TRAFFIC_MODE(as_div.client[other_client].curr_mode) )
    {
      tx_client = other_client;
      break;
    }
  }
  return tx_client;
}

/*============================================================================

FUNCTION TRM_ANT_SWITCH_DIV_GET_TEST_MODE_CTRL

DESCRIPTION
  Get the current tx diversity test mode control.
  
DEPENDENCIES
  None

RETURN VALUE
  Current tx diversity test mode control

SIDE EFFECTS
  None

============================================================================*/
trm_ant_switch_div_test_mode_ctrl_type trm_ant_switch_div_get_test_mode_ctrl()
{
  return as_div.init_setting.test_mode_ctrl;
} /* trm_ant_switch_div_get_test_mode_ctrl */



/*============================================================================

FUNCTION TRM_ANT_SWITCH_DIV_GET_DWELLING_TIME

DESCRIPTION
  Get the current tx diversity dwelling time.
  
DEPENDENCIES
  None

RETURN VALUE
  Current tx diversity test mode control

SIDE EFFECTS
  None

============================================================================*/
trm_ant_switch_div_dwelling_time_type trm_get_ant_switch_div_dwelling_time()
{
  if (!TRM_ASDIV_IS_FEATURE_SUPPORTED)
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
         "TRM_ASDiv: Feature not supported");
    return ASDIV_INVALID_DWELLING_TIME;
  }
  else
  {
    return as_div.init_setting.dwelling_time;
  }
} /* trm_get_ant_switch_div_dwelling_time */



/*============================================================================

FUNCTION trm_asdiv_reset_client_info

DESCRIPTION
  Reset the switching info of the client.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
void trm_asdiv_reset_client_info(trm_client_enum_t client)
{
  /* Reset all the ant switch attributes of the client */
  as_div.client[client].set_mode_cb   	= NULL;
  as_div.client[client].ant_switch_cb     = NULL;
  as_div.client[client].ntfy_switch_change  = FALSE;
  as_div.client[client].subs_sw_info	= TRM_SUBS_NONE;
  //as_div.client[client].req_config    = TRM_ANT_SWITCH_DIV_CONFIG_MAX;
  as_div.client[client].curr_mode     = TRM_CLIENT_MODE_INACTIVE;
  as_div.client[client].req_mode  	= TRM_CLIENT_MODE_INACTIVE;

}


/*============================================================================

FUNCTION TRM_VALIDATE_SET_MODE_INPUT

DESCRIPTION
  Validate the input provided for set mode.
  
DEPENDENCIES
  None

RETURN VALUE
  True -  Valid.
  False - Invalid.

SIDE EFFECTS
  None

============================================================================*/
boolean 	trm_validate_set_mode_input
(
   trm_set_mode_input_type       set_mode_input
   )
{
  boolean 						  ret_val = TRUE;
  trm_ant_switch_set_mode_type      set_mode_info;
  ant_switch_client_struct	 *req_client = NULL;

  if ( TRM_ASDIV_IS_TECH_VALID(set_mode_input.client)) 
  {
     req_client		= &as_div.client[set_mode_input.client];
  }

  /* check for NULL pointer */
  ASD_NULL_PTR_CHK(req_client, FALSE);

  /* check for invalid case, call the client's cb with app. reason
      if invalid parameters */
  if (set_mode_input.client > TRM_LAST_CLIENT ||
      set_mode_input.set_mode_cb == NULL ||
      set_mode_input.mode >= TRM_CLIENT_MODE_MAX
      )
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "Invalid input provided for set mode. Client: %d, Mode: %d",
           set_mode_input.client, set_mode_input.mode);

    set_mode_info.set_mode_grant = TRM_SET_MODE_DENIED_INVALID_PARAMETERS;
    set_mode_info.tag = set_mode_input.tag;

    if (req_client->set_mode_cb != NULL)
    {
      req_client->set_mode_cb(set_mode_info);
    }
    else if (set_mode_input.set_mode_cb != NULL)
    {
      set_mode_input.set_mode_cb(set_mode_info);
    }
    else
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "ASD Set_mode: Invalid parameters with no cb, client: %d",
             set_mode_input.client);
    }
    ret_val = FALSE;
  }

  return ret_val;
}

/*============================================================================

FUNCTION TRM_ANT_SWITCH_INFORM_MODE_CHANGE

DESCRIPTION
  Informs the registered clients about mode changes of other clients.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
void trm_ant_switch_inform_mode_change
(
  trm_client_enum_t switching_client,
  trm_client_mode_type mode_switched
)
{
  uint32 loopcnt = 0;
  trm_ant_switch_mode_notification_info_type set_mode_notify;
  trm_client_enum_t cl_id;
/*--------------------------------------------------------------------------*/
  set_mode_notify.client = switching_client;
  set_mode_notify.mode   = mode_switched;

  for (loopcnt = 0; loopcnt < as_div.active_list.num_clients; ++loopcnt)
  {
    cl_id = as_div.active_list.cl_id[loopcnt];
 
    if (TRM_ASDIV_IS_TECH_VALID(cl_id) &&
        as_div.client[cl_id].ntfy_mode_change &&
        (as_div.client[cl_id].mode_change_cb != NULL) &&
        switching_client != cl_id)
    {
      as_div.client[cl_id].mode_change_cb(set_mode_notify);
    }
  }
}

/*============================================================================

FUNCTION trm_asdiv_grant_mode

DESCRIPTION
  This api simply calls the requested client's setmode callback api to
  grant the set mode request
  
DEPENDENCIES
  None

RETURN VALUE
  none: 

SIDE EFFECTS
  None

============================================================================*/

void trm_asdiv_grant_mode(ant_switch_client_struct *req_client)
{
  trm_ant_switch_set_mode_type      set_mode_info;

  ASD_NULL_PTR_CHK(req_client, RETURN_VOID);

  /* populate the structure with required info */
  set_mode_info.set_mode_grant = TRM_SET_MODE_GRANTED;
  set_mode_info.config	= as_div.init_setting.configuration;
  set_mode_info.tag		= req_client->tag;

  /* call the registered call back if it is not NULL */
  if (req_client->set_mode_cb != NULL)
  {
    req_client->set_mode_cb(set_mode_info);

  }
  else
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
           "TRM_ASDiv; mode GRANT no call back, cl:%d Mode:%d",
           req_client->id, req_client->req_mode );
  }

  /* if the requested mode was sleep, then reset the attributes
     of the client */
  if (req_client->req_mode == TRM_CLIENT_MODE_INACTIVE)
  {
    trm_asdiv_reset_client_info(req_client->id);
  }

  /* change the client's current mode to requested mode */
  req_client->curr_mode	= req_client->req_mode;

  /* Mode granted */
  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED,
         "TRM_ASDiv: mode granted- Client:%d Mode:%d ",
         req_client->id, req_client->curr_mode );


}

/*============================================================================

FUNCTION TRM_ANT_SWITCH_DIV_SET_MODE

DESCRIPTION
  L1s update the mode which they are entering: SLEEP, RX or TX. Depending on
  the mode, set mode logic could initiate call-back to other techs.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
void trm_ant_switch_div_set_mode
(
  trm_set_mode_input_type 	set_mode_input
)
{

  ant_switch_client_struct          *req_client = NULL;
  trm_ant_switch_set_mode_type	    set_mode_info;

  /* get the lock */
  ASDIV_ENTER_CRIT_SECT(as_div.crit_sect);

  /* deny request if feature is not supported */
  if (!TRM_ASDIV_IS_FEATURE_SUPPORTED)
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           " ASD set mode: Feature unsupported!! CL;%d Mode:%d ",
           set_mode_input.client,set_mode_input.mode );

    set_mode_info.set_mode_grant = TRM_SET_MODE_DENIED_FEATURE_UNSUPPORTED;
    set_mode_info.config = TRM_ANT_SWITCH_DIV_CONFIG_MAX;
    set_mode_info.tag = set_mode_input.tag;

    if (set_mode_input.set_mode_cb != NULL)
    {
      set_mode_input.set_mode_cb(set_mode_info);
    }
    ASDIV_EXIT_CRIT_SECT(as_div.crit_sect);
    return;
  }

  /* validate the input values, deny if invalid request is made */
  if ( !trm_validate_set_mode_input(set_mode_input) )
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           " ASD set mode: Invalid params!! CL;%d Mode:%d ",
           set_mode_input.client, set_mode_input.mode );
    ASDIV_EXIT_CRIT_SECT(as_div.crit_sect);
    return;
  }

  /* initialization */
  if ( TRM_ASDIV_IS_TECH_VALID(set_mode_input.client)) 
  {
     req_client		= &as_div.client[set_mode_input.client];
  }
  else
  {
     ERR_FATAL("TRM_ASDiv: set mode, Invalid client %d ", 
               set_mode_input.client, 0, 0);
  }

    /* request is valid, so process further */
  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
         " ASD set mode- Client:%d Mode:%d ",
         set_mode_input.client, set_mode_input.mode );

  req_client->req_mode = set_mode_input.mode;
  req_client->ant_switch_cb = set_mode_input.ant_switch_cb;
  req_client->ntfy_switch_change = set_mode_input.inform_switch_change;
  req_client->set_mode_cb = set_mode_input.set_mode_cb;
  req_client->set_mode_cfg_cb = NULL;
  req_client->tag = set_mode_input.tag;

  /* update tech info */
  trm_asdiv_update_actv_list_tech_info(req_client->id);

  /* update active list clients' priority */
  trm_asdiv_update_actv_list_prio_info();

  /* if mode request is for tx and switch is in transition keep the
     request in pending till switch is complete */
  if ( TRM_ASDIV_IS_TRAFFIC_MODE(req_client->req_mode) &&
       req_client->req_mode != req_client->curr_mode &&
       ASD_SWITCH_IN_TRANSITION)
  {
    req_client->req_config = as_div.next_config;
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH, 
           "TRM_ASDIV: req pending, switching in progress by:%d req_tech:%d",
           as_div.switching_tech, as_div.requesting_tech );
  }
  else
  {
    trm_asdiv_grant_mode(req_client);
  }

  ASDIV_EXIT_CRIT_SECT(as_div.crit_sect);

  return;

} /* trm_ant_switch_div_set_mode */



/*============================================================================

FUNCTION TRM_ANT_SWITCH_DIV_SWITCH_COMPLETE

DESCRIPTION
  L1s update trm about switch complete once they have done the switch following
  set_config or set_mode.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
void trm_ant_switch_div_switch_complete
(
  trm_switch_complete_input_type switch_input
)
{

  uint8 			       loopcnt, client_idx;
  trm_ant_switch_cb_info_type	       cb_info;

/*--------------------------------------------------------------------------*/


  ASDIV_ENTER_CRIT_SECT(as_div.crit_sect);

  if (switch_input.config == TRM_ANT_SWITCH_DIV_CONFIG_MAX ||
      !TRM_ASDIV_IS_FEATURE_SUPPORTED)
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "TRM_ASDiv: sw complete unexp Cl:%d config:%d",
           switch_input.client, switch_input.config );
    ASDIV_EXIT_CRIT_SECT(as_div.crit_sect);
    return;
  }

  MSG_3( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
         "TRM_ASDiv: Sw complete: exp cfg:%d Cl:%d cfg:%d",
         as_div.next_config, switch_input.client, switch_input.config);

  /* get the default config */
  if (switch_input.config == TRM_ANT_SWITCH_DIV_CONFIG_DEFAULT)
  {
    switch_input.config = as_div.init_setting.configuration;
  }

  /* if the switch changed is the last requested switching */
  if (ASD_SWITCH_IN_TRANSITION &&
      as_div.switching_tech == switch_input.client &&
      switch_input.config == as_div.next_config)
  {
    /* populate the call back info */
    cb_info.action = TRM_ANT_SWITCH_SET_CONFIG_DONE;
    cb_info.config = as_div.next_config;
    cb_info.feature_override = TRM_ANT_SWITCH_FEATURE_OVERRIDE_UNSET;

    for (loopcnt = 0; loopcnt < as_div.active_list.num_clients; ++loopcnt)
    {
      client_idx = (uint8) as_div.active_list.cl_id[loopcnt];

      /* inform subscribed clients about switch completion */
      if ( (as_div.client[client_idx].ntfy_switch_change == TRUE &&
            as_div.client[client_idx].id != switch_input.client) ||
           as_div.client[client_idx].switch_start_reported == TRUE )
      {

        cb_info.client = as_div.client[client_idx].id;

        if (as_div.client[client_idx].ant_switch_cb != NULL)
        {
          as_div.client[client_idx].ant_switch_cb(cb_info);
        }
        else
        {
          MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
                 "TRM_ASDiv: sw complete NULL cb; cl:%d ",
                 client_idx );
        }

        as_div.client[client_idx].switch_start_reported = FALSE;

      }

      /* Grant pending mode request */
      if (as_div.client[client_idx].req_config == switch_input.config)
      {
        as_div.client[client_idx].req_config = TRM_ANT_SWITCH_DIV_CONFIG_MAX;

        if (as_div.client[client_idx].req_mode != as_div.client[client_idx].curr_mode)
        {

          if ( client_idx != ((uint8)switch_input.client) )
          {
            trm_asdiv_grant_mode(&as_div.client[client_idx]);
          }

          trm_ant_switch_inform_mode_change(as_div.client[client_idx].id,
                                            as_div.client[client_idx].req_mode);

        }
      }
    }

    /* send the switch event */
    as_div.event_info.client_id = (uint8) switch_input.client;
    as_div.event_info.old_switch_config = (uint8) as_div.prev_config;
    as_div.event_info.new_switch_config = (uint8) as_div.next_config;
    as_div.event_info.event_type = TRM_ASDIV_EVENT_SWITCHING_OTHERS;

    event_report_payload((event_id_enum_type) EVENT_MCS_TRM_ASDIV,
                         sizeof(trm_asdiv_event_info_type),
                         &as_div.event_info );


    /* update the current switch configuration */
    as_div.curr_setting.configuration	= as_div.next_config;

    /* update switching and requesting techs */
    as_div.switching_tech = TRM_NO_CLIENT;
    as_div.requesting_tech = TRM_NO_CLIENT;

    /* change previous and next config to max after switch complet */
    as_div.prev_config = TRM_ANT_SWITCH_DIV_CONFIG_MAX;
    as_div.next_config = TRM_ANT_SWITCH_DIV_CONFIG_MAX;

  }

  ASDIV_EXIT_CRIT_SECT(as_div.crit_sect);

} /* trm_ant_switch_div_switch_complete */


/*============================================================================

FUNCTION TRM_ANT_SWITCH_DIV_SWITCH_CANCEL

DESCRIPTION
  L1s update trm about switch incomplete if for any reason the switching
  couldnt go through.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/

void trm_ant_switch_div_switch_cancel(trm_switch_complete_input_type req_data)
{
  uint8 cnt;
  trm_ant_switch_cb_info_type		cb_info;

  ASDIV_ENTER_CRIT_SECT(as_div.crit_sect);

  if (req_data.config == TRM_ANT_SWITCH_DIV_CONFIG_MAX ||
      !TRM_ASDIV_IS_FEATURE_SUPPORTED)
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "ASD switch cancel unexpected Client:%d config:%d",
           req_data.client, req_data.config );
    ASDIV_EXIT_CRIT_SECT(as_div.crit_sect);
    return;
  }

  /* check if the caller is the legitimate client in switch control */
  if (ASD_SWITCH_IN_TRANSITION &&
       as_div.switching_tech == req_data.client &&
      as_div.next_config == req_data.config)
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
           "ASD Switch Req Cancelled, Cl:%d, cf:%d",
           req_data.client, req_data.config );

    /* populate the call back info */
    cb_info.action = TRM_ANT_SWITCH_SET_CONFIG_DONE;
    cb_info.config = as_div.prev_config;
    cb_info.feature_override = TRM_ANT_SWITCH_FEATURE_OVERRIDE_UNSET;

    as_div.curr_setting.configuration = as_div.prev_config;
    as_div.client[req_data.client].req_config = TRM_ANT_SWITCH_DIV_CONFIG_MAX;

    /* retrieve waiting client's mode */
    for (cnt = 0; cnt <= TRM_LAST_CLIENT; cnt++)
    {
      if (as_div.client[cnt].req_config == req_data.config)
      {
        as_div.client[cnt].req_config = TRM_ANT_SWITCH_DIV_CONFIG_MAX;
      }
      if (as_div.client[cnt].curr_mode != as_div.client[cnt].req_mode)
      {

        /* mode granting if switching cancelled enabled, so grant */
        trm_asdiv_grant_mode(&as_div.client[cnt]);
      }

      /* inform switch initiation reported techs about switch process is done */
      if(as_div.client[cnt].switch_start_reported == TRUE)
      {
        as_div.client[cnt].switch_start_reported = FALSE;
        cb_info.client = as_div.client[cnt].id;

        if (as_div.client[cnt].ant_switch_cb != NULL)
        {
          as_div.client[cnt].ant_switch_cb(cb_info);
        }

     }

    }

    /* send the switch event */
    as_div.event_info.client_id = (uint8) req_data.client;
    as_div.event_info.old_switch_config = (uint8) as_div.prev_config;
    as_div.event_info.new_switch_config = (uint8) as_div.prev_config;
    as_div.event_info.event_type = TRM_ASDIV_EVENT_SWITCHING_CANCEL;

    event_report_payload((event_id_enum_type) EVENT_MCS_TRM_ASDIV,
                         sizeof(trm_asdiv_event_info_type),
                         &as_div.event_info );

    /*update the switching and requesting techs */
    as_div.switching_tech = TRM_NO_CLIENT;
    as_div.requesting_tech = TRM_NO_CLIENT;

    /* change previous and next config to max after switch complet */
    as_div.prev_config = TRM_ANT_SWITCH_DIV_CONFIG_MAX;
    as_div.next_config = TRM_ANT_SWITCH_DIV_CONFIG_MAX;

  }
  else
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "TRM_ASDiv: Invalid Switch cancel request, cl:%d cf: %d",
           req_data.client, req_data.config );
  }

  ASDIV_EXIT_CRIT_SECT(as_div.crit_sect);

} /* trm_ant_switch_div_switch_cancel*/


/*============================================================================

FUNCTION TRM_ANT_SWITCH_SET_CONTROL

DESCRIPTION
  Updates the ant switch diversity switch control, each tech can hold or
  release the switch control form its side. 
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
void trm_ant_switch_set_control(trm_ant_switch_set_ctrl_type 	ctrl_req)
{

  ASDIV_ENTER_CRIT_SECT(as_div.crit_sect);

  do
  {
    if (!TRM_ASDIV_IS_FEATURE_SUPPORTED)
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "TRM_ASDiv Feature not supported" );
      break;
    }

    if (ctrl_req.req_type >= TRM_ANT_SWITCH_CTRL_INVALID ||
           ctrl_req.client > TRM_LAST_CLIENT)
    {
      MSG_3( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "TRM_ASDiv; invalid values cl:%d req:%d allow_crisis_mode_switch:%d",
             ctrl_req.client, ctrl_req.req_type, ctrl_req.allow_crisis_mode_switching );
      break;
    }

    as_div.client[ctrl_req.client].safe_state_info.sw_control = \
       ctrl_req.req_type;
    as_div.client[ctrl_req.client].safe_state_info.allow_crisis_mode_switching = \
       ctrl_req.allow_crisis_mode_switching;

    MSG_3( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
           "TRM_ASDiv; control set by cl:%d to %d allow_cris_switch_flag:%d",
           ctrl_req.client, ctrl_req.req_type, ctrl_req.allow_crisis_mode_switching );


  }while(0);

  
  ASDIV_EXIT_CRIT_SECT(as_div.crit_sect);

} /* trm_ant_switch_set_control */


/*============================================================================

FUNCTION TRM_GET_CLIENT_ANT_SWITCH_MODE

DESCRIPTION
  Returns the mode of the requested client.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
trm_client_mode_type trm_get_client_ant_switch_mode(trm_client_enum_t clientid)
{
  trm_client_mode_type ret_val = TRM_CLIENT_MODE_MAX;

  ASDIV_ENTER_CRIT_SECT(as_div.crit_sect);

  if (!TRM_ASDIV_IS_FEATURE_SUPPORTED)
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
         "TRM_ASDiv: Feature not supported" );
  }
  else if (clientid <= TRM_LAST_CLIENT)
  {
    ret_val = as_div.client[clientid].curr_mode;
    if (as_div.client[clientid].curr_mode != as_div.client[clientid].req_mode)
    {
      ret_val = as_div.client[clientid].req_mode;
    }
  }

  ASDIV_EXIT_CRIT_SECT(as_div.crit_sect);

  return ret_val;
}


/*============================================================================

FUNCTION TRM_SET_ANT_SWITCH_DIV_STATE

DESCRIPTION
  Updates the ant switch diversity state(enable or disable) for a given client.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
void trm_set_ant_switch_div_state(trm_set_asd_state_input_type input)
{

  ASDIV_ENTER_CRIT_SECT(as_div.crit_sect);

  if (!TRM_ASDIV_IS_FEATURE_SUPPORTED)
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
         "TRM_ASDiv: Feature not supported" );
  }
  else if (input.client <= TRM_LAST_CLIENT)
  {
    as_div.client[input.client].ant_switch_state = input.state;
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
           "Client %d ant switch div state changed to %d",
           input.client, input.state );
  }
  ASDIV_EXIT_CRIT_SECT(as_div.crit_sect);
}


/*============================================================================

FUNCTION TRM_SET_ANT_SWITCH_DIV_STATE

DESCRIPTION
  Updates the ant switch diversity state(enable or disable) for a given client.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
void trm_set_ant_switch_mode_change_notify
(
  trm_ant_switch_mode_notification_input_type input
)
{

  ASDIV_ENTER_CRIT_SECT(as_div.crit_sect);

  if (!TRM_ASDIV_IS_FEATURE_SUPPORTED)
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
         "TRM_ASDiv: Feature not supported" );
  }
  else if (input.client <= TRM_LAST_CLIENT)
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
           "Mode change notify request, Client: %d, notify: %d",
           input.client, input.should_notify );
    as_div.client[input.client].ntfy_mode_change = input.should_notify;
    as_div.client[input.client].mode_change_cb   = input.mode_change_cb;
  }
  else
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "Invalid client info for mode change notify: %d", 
           input.client );
  }

  ASDIV_EXIT_CRIT_SECT(as_div.crit_sect);
}



/*============================================================================

FUNCTION TRM_ANT_SWITCH_IS_SWITCHING_ALLOWED

DESCRIPTION
  Returns whether switching is allowed or not.
  
DEPENDENCIES
  None

RETURN VALUE
  boolean

SIDE EFFECTS
  None

============================================================================*/
boolean trm_ant_switch_is_switching_allowed(void)
{
  boolean retval = TRUE;

  ASDIV_ENTER_CRIT_SECT(as_div.crit_sect);

  if (!TRM_ASDIV_IS_FEATURE_SUPPORTED)
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
         "TRM_ASDiv: Feature not supported" );
    retval = FALSE;
  }

  ASDIV_EXIT_CRIT_SECT(as_div.crit_sect);

  return retval;

}


/* DSDA ASDiv changes */

/*============================================================================

FUNCTION TRM_ASDIV_REPORT_SWITCH_INITIATION

DESCRIPTION
   Updates the switch initiation to the passive tech. In dual tx scenario of
   techs X+Y, if X requests for switching that can be granted, Y is notified
   about the starting of the switching process.
 
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
void trm_asdiv_report_switch_initiation
(
   trm_client_enum_t                switching_client,
   trm_ant_switch_div_config_type   requested_config
)
{
  uint8                           i, client_id;
  uint8                           tx_tech_count = 0;
  trm_ant_switch_cb_info_type     call_back_info;

  /* check for dual tx scenario by getting tech count in tx */
  for(i=0; i<as_div.active_list.num_clients; i++)
  {
    client_id = (uint8) as_div.active_list.cl_id[i];
    if( TRM_ASDIV_IS_TECH_VALID(client_id) &&
        ( as_div.client[client_id].curr_mode == TRM_CLIENT_MODE_RXTX ||
          as_div.client[client_id].curr_mode == TRM_CLIENT_MODE_RXTX_LOW ) )
    {
      tx_tech_count++;
    }

  }

  /* if tech count is greater than 1, report switch initiation */
  if( tx_tech_count > 1 )
  {
    call_back_info.action = TRM_ANT_SWITCH_SET_CONFIG_STARTED;
    
    call_back_info.config = requested_config;
    call_back_info.feature_override = TRM_ANT_SWITCH_FEATURE_OVERRIDE_UNSET;

    for(i=0; i<as_div.active_list.num_clients; i++)
    {
      /* skip reporting if this is the requesting client */
      if( as_div.active_list.cl_id[i] == switching_client )
      {
        continue;
      }

      call_back_info.client = as_div.active_list.cl_id[i];
      client_id = (uint8) as_div.active_list.cl_id[i];

      if( TRM_ASDIV_IS_TECH_VALID(client_id) &&
          TRM_ASDIV_IS_TRAFFIC_MODE(as_div.client[client_id].curr_mode) &&
          as_div.client[client_id].ant_switch_cb != NULL )
      {
        as_div.client[client_id].ant_switch_cb( call_back_info );

        as_div.client[client_id].switch_start_reported = TRUE;

        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
               "TRM_ASDIV: rep_sw_initiate; cl:%d, reported switch initiation",
               client_id );
        
      }
    }
  }
}


/*============================================================================

FUNCTION TRM_ASDIV_UPDATE_PREF_CONFIG

DESCRIPTION
   Updates the preferred switch configuration of the client, it is triggered
   when a tech requests for switching
 
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
void trm_asdiv_update_pref_config
(
  trm_client_enum_t                cl,
  trm_ant_switch_div_config_type   config
)
{

  trm_get_info_input_type  get_info_input;
  trm_get_info_return_type get_info_output;

  trm_ant_switch_div_config_type    new_pref_cfg;
  rfm_device_enum_type              new_rf_dev;

  boolean   is_new_dev;
  
  //new_pref_cfg = config;

  /* get device info from TRM data */
  get_info_input.client_id = cl;
  get_info_input.info_type = TRM_CLIENT_INFO;
  get_info_input.input_data.client_info.requested_client_id = cl;
  trm_get_info(&get_info_input, &get_info_output);

  /* add sanity check for the same client id */

  new_rf_dev = get_info_output.info.client_info.dev;

  /* make sure that dev is not invalid/max */
  if ( new_rf_dev == RFM_INVALID_DEVICE ||
       new_rf_dev == RFM_MAX_DEVICES ) 
  {
    new_rf_dev = as_div.client[cl].pref_config_info.associated_rf_dev;
  }

  /* check if device has changed */
  is_new_dev = (new_rf_dev == as_div.client[cl].pref_config_info.associated_rf_dev)?
     FALSE:TRUE;


  /* figure out the preferred switch configuration */
  switch(config)
  {
   case TRM_ANT_SWITCH_DIV_CONFIG_0:
   case TRM_ANT_SWITCH_DIV_CONFIG_1:
     new_pref_cfg = config;
     break;

   case TRM_ANT_SWITCH_DIV_CONFIG_CURRENT:
     if(as_div.curr_setting.configuration != SWITCH_IN_TRANSITION)
     {
       new_pref_cfg = as_div.curr_setting.configuration;
     }
     else
     {
       new_pref_cfg = as_div.client[cl].pref_config_info.switch_config;
     }
     break;

   case TRM_ANT_SWITCH_DIV_CONFIG_DEFAULT:
     new_pref_cfg = as_div.init_setting.configuration;
     break;

   case TRM_ANT_SWITCH_DIV_CONFIG_PREFERRED:
     if (is_new_dev) 
     {
       new_pref_cfg = TRM_ASDIV_OPP_CONFIG(as_div.client[cl].pref_config_info.switch_config);
     }
     else
     {
       new_pref_cfg = as_div.client[cl].pref_config_info.switch_config;
     }
     break;

   default:
     MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED,
            "TRM_ASDIV: pref_cfg_upd; Invalid sw config cl:%d, cfg:%d",
            cl, config );
     new_pref_cfg = as_div.client[cl].pref_config_info.switch_config;
     break;

  }

  /* update the pref config if it has changed */
  if ( is_new_dev ||
       as_div.client[cl].pref_config_info.switch_config != new_pref_cfg)
  {
    MSG_5( MSG_SSID_DFLT, MSG_LEGACY_MED,
           "TRM_ASDIV: pref_cfg_upd; client:%d prev_cfg:%d, prev_dev: %d "
           "new_cfg:%d, new_dev",
           cl, as_div.client[cl].pref_config_info.switch_config, 
           as_div.client[cl].pref_config_info.associated_rf_dev,
           new_pref_cfg, new_rf_dev );

    as_div.client[cl].pref_config_info.switch_config = new_pref_cfg;
    as_div.client[cl].pref_config_info.associated_rf_dev = new_rf_dev;
  }

}

/*============================================================================

FUNCTION TRM_ASDIV_UPDATE_SWITCH_LOCK_MASK

DESCRIPTION
   Update the switch lock mask with the request made by the clients.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
void trm_asdiv_update_switch_lock_mask(void)
{
  uint8 i;
  uint8 client_id;

  /* start with clearing the mask*/
  as_div.lock_mask = 0;

  /* check for each active client */
  for (i = 0; i < as_div.active_list.num_clients; i++)
  {
    client_id = (uint8) as_div.active_list.cl_id[i];
    as_div.lock_mask |= as_div.client[client_id].switch_lock_mask;
  }

}

/*============================================================================

FUNCTION TRM_ASDIV_UPDATE_IDLE_COL_DATA

DESCRIPTION
   Updates the collision count if a client is denied its preferred chain
   due to idle_wakup + idle_wakeup collision
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
void trm_asdiv_update_idle_col_data(
   trm_client_enum_t       cl
   )
{


  MSG_3( MSG_SSID_DFLT, MSG_LEGACY_MED,
         "TRM_ASDIV: col_upd; BEFORE cl:%d cnt:%d cfg:%d",
         as_div.idl_idl_col_data.denied_cl,
         as_div.idl_idl_col_data.col_count,
         as_div.idl_idl_col_data.denied_config);

  /* Before updating the colision count, make sure
        a) the client id is same, otherwise reset
        b) the preferred config is same, otherwise reset */
  if (as_div.idl_idl_col_data.denied_cl != cl ||
      as_div.idl_idl_col_data.denied_config != as_div.client[cl].pref_config_info.switch_config)
  {
    as_div.idl_idl_col_data.col_count = 0;
  }

  /* update the collision data */
  as_div.idl_idl_col_data.denied_cl = cl;
  as_div.idl_idl_col_data.denied_config = as_div.client[cl].pref_config_info.switch_config;
  as_div.idl_idl_col_data.col_count++;

  /* if the limit has been reached, set limit_reach flag */
  if (as_div.idl_idl_col_data.col_count >= as_div.idl_idl_col_data.idle_idle_col_limit)
  {
    as_div.idl_idl_col_data.limit_reached = TRUE;
    MSG_3( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
           "TRM_ASDIV: col_upd; col_limit reached cl:%d cnt:%d cfg:%d",
           cl, as_div.idl_idl_col_data.col_count,
           as_div.idl_idl_col_data.denied_config);
  }

  MSG_3( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
         "TRM_ASDIV: col_upd; AFTER cl:%d cnt:%d cfg:%d",
         cl, as_div.idl_idl_col_data.col_count,
         as_div.idl_idl_col_data.denied_config);
}

/*============================================================================

FUNCTION TRM_ASDIV_CHK_MODE_COMB_RESTRICTION

DESCRIPTION
  Called internally to see if active mode combination restricts switching
  restricting combined modes:
      idle_wakeup+idle_wakeup
      idle_wakeup+traffic
      acq + traffic
      sleep + non-traffic
  
DEPENDENCIES
  None

RETURN VALUE
  TRUE: if current combined mode is not restricting
  FALSE: if current combined mode is restricting

SIDE EFFECTS
  None

============================================================================*/
boolean trm_asdiv_chk_mode_comb_restriction(
   trm_client_enum_t    this_cl
   )
{
  uint8               i;
  boolean              reiterate;
  boolean              mode_comb_alowd = TRUE;
  trm_client_enum_t    other_cl;


  /* check for invalid tech in the list */
  if ( !TRM_ASDIV_IS_TECH_VALID(this_cl) ) 
  {
     MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
            "TRM_ASDIV: mode_comb_chk, Invalid client id: %d", this_cl );
     return FALSE;
  }

  /* check with the client in active list */
  for (i = 0; i < as_div.active_list.num_clients; i++)
  {
    other_cl = as_div.active_list.cl_id[i];

    if (other_cl == this_cl)
    {
      continue;
    }

    do{
     /* check for invalid tech in the list */
      reiterate = FALSE;

    if ( !TRM_ASDIV_IS_TECH_VALID(other_cl) ) 
    {
       MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
              "TRM_ASDIV: mode_comb_chk, Invalid client id: %d", other_cl );
       mode_comb_alowd = FALSE;
       break;
    }

      if( TRM_ASDIV_IS_IDLE_MODE(as_div.client[other_cl].req_mode) )
      {
        MSG_3( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
               "TRM_ASDIV: sw_perm-DENIED for client:%d as client:%d is in idle mode: %d",
               this_cl, other_cl, as_div.client[other_cl].req_mode );
        mode_comb_alowd = FALSE;

        /* check if it is idle/idle collision to set collision count */
      if (as_div.client[this_cl].req_mode == TRM_CLIENT_MODE_IDLE_WAKEUP &&
          as_div.client[other_cl].req_mode == TRM_CLIENT_MODE_IDLE_WAKEUP)
      {
        /* restricting mode idle_wakeup + idle_wakeup */
        as_div.idl_idl_col_data.is_idl_idl_col = TRUE;
      }

        break;

      }

      if( (TRM_ASDIV_IS_IDLE_MODE(as_div.client[this_cl].req_mode)||
           as_div.client[this_cl].req_mode == TRM_CLIENT_MODE_RX_ONLY ) &&
          TRM_ASDIV_IS_TRAFFIC_MODE(as_div.client[other_cl].req_mode) )
      {
        MSG_3( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
               "TRM_ASDIV: sw_perm-DENIED for client:%d as client:%d is in traffic mode: %d",
               this_cl, other_cl, as_div.client[other_cl].req_mode );
        mode_comb_alowd = FALSE;
        break;

      }
      reiterate = TRUE;
    }while(0);

    if( reiterate == FALSE ) 
    {
      break;
    }
  }

  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED,
         "TRM_ASDIV: mode_comb_alowd, %d ", mode_comb_alowd );

  return mode_comb_alowd;

}

/*============================================================================

FUNCTION TRM_ASDIV_CHK_SWITCH_PERM

DESCRIPTION
  Called internally to see if switching can be done by the passed client
  
DEPENDENCIES
  None

RETURN VALUE
  TRUE: if switching can be done
  FALSE: if switching cannot be done

SIDE EFFECTS
  None

============================================================================*/
boolean trm_asdiv_chk_switch_perm
(
   trm_client_enum_t    cl_id,
   boolean              is_crisis_mode
)
{
  int i;
  uint8 client;
  boolean  perm_granted = FALSE;
  boolean  mode_comb_alowd;

  do
  {
  
    if ( as_div.curr_setting.configuration == SWITCH_IN_TRANSITION )
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
             "TRM_ASDIV: sw_perm-DENIED, sw in TRANSITION sw_tech:%d req_tech:%d",
             as_div.switching_tech,
             as_div.requesting_tech );

      break;
    }

    if( as_div.lock_mask != 0 )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
             "TRM_ASDIV: sw_perm-DENIED, sw locked mask: 0x%x",
             as_div.lock_mask );
      for(i=0; i<as_div.active_list.num_clients; i++)
      {
        client = (uint8) as_div.active_list.cl_id[i];
        if(as_div.client[client].switch_lock_mask != 0)
        {
          MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
                 "TRM_ASDIV: Sw locking tech:%d mask: 0x%x",
                 client, as_div.client[client].switch_lock_mask );
        }
      }

      break;
    }

    /* check if current active mode combination restrict switching */
    mode_comb_alowd = trm_asdiv_chk_mode_comb_restriction(cl_id);

    if (mode_comb_alowd == TRUE)
    {
      /* mode combination allows switching,
         if requesting cl is high priority client, grant
         else check if the high priority client is in safe situation */
      if ( cl_id == as_div.active_list.high_prio_cl ||
           as_div.ignore_high_prio_sleep == TRUE )
      {
        /* it is high priority client, so grant */
        perm_granted = TRUE;
      }
      else
      {
        /* neither standalone nor highest priority client */
        /* start with grant */
        perm_granted = TRUE;

        /* check in the active list it it has high priority 
           client holding the control */
        for(i=0; i<as_div.active_list.num_clients; i++)
        {
          /* no need to check with itself */
          if(cl_id == as_div.active_list.cl_id[i])
          {
            continue;
          }

          client = (uint8) as_div.active_list.cl_id[i];
          
          if( as_div.client[client].priority_mask > as_div.client[cl_id].priority_mask &&
              as_div.client[client].safe_state_info.sw_control != TRM_ANT_SWITCH_CTRL_RELEASE )
          {
             /* for crisis mode switch request from Low priority client, switching can
                be allowed if high priority client is in SLEEP and has indicated to 
                allow crisis mode siwtching in unsafe mode */
             if ( !(is_crisis_mode &&
                    as_div.client[client].curr_mode == TRM_CLIENT_MODE_SLEEP &&
                    as_div.client[client].safe_state_info.allow_crisis_mode_switching) ) 
             {
             
                MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
                       "TRM_ASDIV: SW perm denied: High priority cl:%d ctrl held:%d",
                       client, as_div.client[client].safe_state_info.sw_control );
                MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
                       "TRM_ASDIV: Priority Mask High:0x%x Low: 0x%x",
                       as_div.client[client].priority_mask, 
                       as_div.client[cl_id].priority_mask );

                perm_granted = FALSE;
                break;
             }
          }
        }/* for loop */
      }
    }
  }while(0);

  return perm_granted;

}

/*============================================================================

FUNCTION TRM_ASDIV_MAP_TRM_SP_ID

DESCRIPTION
  Maps TRM client id to subscription priority client id

DEPENDENCIES
  None

RETURN VALUE
  Subscription priority client id

SIDE EFFECTS
  None

============================================================================*/
sp_client_enum_t trm_asdiv_map_trm_sp_id(
   trm_client_enum_t  trm_id
)
{

  switch(trm_id)
  {
    
    case TRM_GSM2:
      return SP_GSM2;

    case TRM_GSM3:
      return SP_GSM3;

  default:
    return SP_MAX_CLIENT;

  }

}


/*============================================================================

FUNCTION TRM_ASDIV_GET_MODE_TIER

DESCRIPTION
  This api returns the mode tier for different techs
  1x SLEEP tier>G sub1 SLEEP tier>G sub2 SLEEP tier >other tech/modes  
  
  
DEPENDENCIES
  None

RETURN VALUE
  mode tier

SIDE EFFECTS
  None

============================================================================*/
trm_asdiv_mode_tier trm_asdiv_get_mode_tier
(
   trm_client_enum_t client_id
)
{
   /* check for valid client */

   if (as_div.client[client_id].req_mode != TRM_CLIENT_MODE_SLEEP) 
   {
     return TRM_ASDIV_MODE_TIER_NORMAL;
   }

   switch (client_id) 
   {
      case TRM_1X:
        return TRM_ASDIV_MODE_TIER_SLEEP_1X;

      case TRM_GSM1:
        return TRM_ASDIV_MODE_TIER_SLEEP_GSM1;

      case TRM_GSM2:
        return TRM_ASDIV_MODE_TIER_SLEEP_GSM2;

      default:
        return TRM_ASDIV_MODE_TIER_NORMAL;
   }
}


/*============================================================================

FUNCTION TRM_ASDIV_UPDATE_ACTV_LIST_PRIO_INFO

DESCRIPTION
  This api updates the priority mask of the active clients. Also finds
  out the high priority client in the list.
    prioirty mask = 32 bit
      31-24 -> tech macrop priority   
      23-16 -> tech registered mode
      8-15  -> reserved for tech id
      7-0   -> priority index  
  
  
DEPENDENCIES
  None

RETURN VALUE
  NONE

SIDE EFFECTS
  Idle\idle collision count could get reset if the combined mode of active
  clients is other than idle & sleep combination

============================================================================*/

void trm_asdiv_update_actv_list_prio_info()
{

  uint8    i,j;
  uint8    tech_state, state_order_index;
  uint8    mode, mode_tier;
  uint32   prio_mask;
  boolean  match_found;
  trm_client_enum_t    cl_id;


  /* start with combined mode mask as 0, it will be updated with
     active list clients' mode */
  as_div.active_list.active_mode_mask = 0;
  as_div.active_list.high_prio_mask = 0;

  for (i = 0; i < as_div.active_list.num_clients; i++)
  {
    cl_id = as_div.active_list.cl_id[i];

    /* Sanity check for invalid tech in the list */
    if ( !TRM_ASDIV_IS_TECH_VALID(cl_id) ) 
    {
       MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
              "TRM_ASDIV: lst_prio, Invalid client id: %d", cl_id );
       break;
    }

    

    match_found = FALSE;
    tech_state = SP_BASELINE_STATE;
    state_order_index = 0;

    /* go through the tech state list to figure out the tech state */
    for (j=0; j<as_div.tech_state_info.num_active_techs; j++) 
    {
       if (as_div.tech_state_info.tech_state[j].client_id == cl_id) 
       {
          tech_state = as_div.tech_state_info.tech_state[j].macro_state;
          state_order_index = j;
          match_found = TRUE;
          break;
       }
    }

    if (!match_found) 
    {
       /* do nothing for now. later need to add the logic to copy state from other tech */
        MSG_2(MSG_SSID_DFLT, MSG_LEGACY_ERROR, 
              "Active tech not in tech state list %d %d", 
              cl_id, as_div.client[cl_id].curr_mode);
		
    }

    mode = (uint8) as_div.client[cl_id].req_mode;
    mode_tier = (uint8) trm_asdiv_get_mode_tier(cl_id);

    /* calculate the priority mask */
    prio_mask = ((1 << tech_state) << TRM_ASDIV_SUB_STATE_SHIFT) |\
       ((1 << mode) << TRM_ASDIV_TECH_MODE_SHIFT) |\
       ((1 << mode_tier) << TRM_ASDIV_MODE_TIER_SHIFT) |\
       ((1 << (TRM_ASDIV_MAX_CONCURRENT_TECHS - state_order_index)) << TRM_ASDIV_SUB_PRIO_SHIFT);


    /* find out the high priority client and update */
    /* needs to check if it is needed */
    if (as_div.active_list.high_prio_mask < prio_mask)
    {
      as_div.active_list.high_prio_mask = prio_mask;
      as_div.active_list.high_prio_cl = cl_id;
    }

    /* assign caluclated priority mask */
    as_div.client[cl_id].priority_mask = prio_mask;

    /* update the aggregate active mode mask, it contains the bit mask for active mode */
    as_div.active_list.active_mode_mask |= ((uint32)(1 << as_div.client[cl_id].req_mode));

    MSG_3( MSG_SSID_DFLT, MSG_LEGACY_MED,
           "TRM_ASDIV: lst_prio, Cl: %d, tech_state: %d prio: 0x%x", 
           cl_id, tech_state, prio_mask);
  }

  /* if a tech is in mode other than sleep/idlewakeup, reset col count */
  if (as_div.active_list.active_mode_mask & ASDIV_RST_COL_CNT_MASK)
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED,
           "TRM_ASDIV: lst_prio, Col cnt rst due to Comb mode: 0x%x",
           as_div.active_list.active_mode_mask );
    as_div.idl_idl_col_data.denied_cl = TRM_NO_CLIENT;
  }
}

/*============================================================================

FUNCTION TRM_ASDIV_UPDATE_ACTIVE_LIST_TECH_INFO

DESCRIPTION
  Whenever a new tech becomes active, it is added to the active list and tech
  becoming inactive is removed
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
void trm_asdiv_update_actv_list_tech_info(
   trm_client_enum_t    cl
   )
{

  int ind, num;

  num = as_div.active_list.num_clients;

  if ( !TRM_ASDIV_IS_TECH_VALID (cl) ) 
  {
     return;
  }

  /* if requesting client is new, add to the active list */
  if ( as_div.client[cl].curr_mode == TRM_CLIENT_MODE_INACTIVE &&
       as_div.client[cl].req_mode != TRM_CLIENT_MODE_INACTIVE )
  {
    as_div.active_list.cl_id[num] = cl;
    as_div.active_list.num_clients++;

    /* need to find the sub_id, order_index, sub_prio */
    //trm_asdiv_update_actv_list_sub_info();

    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED,
           "TRM_ASDIV: upd_act_lst: Client: %d, added; cnt:%d",
           cl, as_div.active_list.num_clients );
  }
  /* if requesting client is being inactive, remove from the list */
  else if ( as_div.client[cl].req_mode == TRM_CLIENT_MODE_INACTIVE &&
            as_div.client[cl].curr_mode != TRM_CLIENT_MODE_INACTIVE)
  {
    for (ind = 0; ind < num; ind++)
    {
      if (as_div.active_list.cl_id[ind] == cl)
      {
        /* there is a match, reduce the active client number and shift */
        num = --as_div.active_list.num_clients;

        while (ind < num)
        {
          as_div.active_list.cl_id[ind] = as_div.active_list.cl_id[ind + 1];
          ind++;
        }
        break;
      }
    }

    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED,
           "TRM_ASDIV: upd_act_lst: Client: %d, removed, cnt:%d",
           cl, as_div.active_list.num_clients );
  }
  
}

/*============================================================================

FUNCTION TRM_ASDIV_VALIDATE_CLIENT_INFO

DESCRIPTION
  
  
DEPENDENCIES
  None

RETURN VALUE
  NONE

SIDE EFFECTS
 

============================================================================*/
boolean trm_asdiv_validate_client_info(
   const trm_set_mode_and_config_input_t 	*input_data
   )
{
  boolean                             info_valid = TRUE;
  trm_client_enum_t                   cl;
  trm_asd_set_mode_and_config_type    cb_info;

  cl = input_data->client;

  cb_info.config = as_div.curr_setting.configuration;
  cb_info.tag = input_data->tag;
  cb_info.set_config_grant = TRM_ANT_SWITCH_DIV_SET_CFG_DENIED;

  if (!TRM_ASDIV_IS_FEATURE_SUPPORTED)
  {
    info_valid = FALSE;
    cb_info.set_mode_grant = TRM_SET_MODE_DENIED_FEATURE_UNSUPPORTED;
    
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
         "TRM_ASDIV: Feature not supported!" );

  }
  /* Validate the client info */
  else if (cl > TRM_LAST_CLIENT ||
           input_data->set_mode_cb == NULL ||
           input_data->config >= TRM_ANT_SWITCH_DIV_CONFIG_INVALID ||
           input_data->mode >= TRM_CLIENT_MODE_MAX)
  {
    info_valid = FALSE;
    cb_info.set_mode_grant = TRM_SET_MODE_DENIED_INVALID_PARAMETERS;

    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
         "TRM_ASDIV: set_mod_cfg: Invalid client info");
  }

  if (info_valid == FALSE)
  {
    /* deny the request if validation fails */
    if (input_data->set_mode_cb != NULL)
    {
      input_data->set_mode_cb(cb_info);
    }
    else
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "TRM_ASDiv: Set_mode: Invalid param with no cb, client: %d",
             input_data->client );
    }

  }
  else
  {
    /* update the client info if validation passes */
    as_div.client[cl].req_mode = input_data->mode;
    as_div.client[cl].ant_switch_cb = input_data->ant_switch_cb;
    as_div.client[cl].set_mode_cfg_cb = input_data->set_mode_cb;
    as_div.client[cl].set_mode_cb = NULL;
    as_div.client[cl].ntfy_switch_change = input_data->inform_switch_change;
    as_div.client[cl].tag = input_data->tag;
  }

  return info_valid;

}

/*============================================================================

FUNCTION TRM_ANT_SWITCH_DIV_SET_CONFIG

DESCRIPTION
  Sets the tx diversity switch configuration to given configuration.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
trm_set_ant_switch_return_type trm_ant_switch_div_set_config
(
  trm_ant_switch_set_input_type new_setting
)
{
  trm_set_ant_switch_return_type 	return_val;

  ASDIV_ENTER_CRIT_SECT(as_div.crit_sect);

  do
  {
    if (!TRM_ASDIV_IS_FEATURE_SUPPORTED)
    {
       MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
            "TRM_ASDiv: Feature not supported" );
       return_val = TRM_ANT_SWITCH_DIV_SET_FAIL_FEAT_DISABLED;
       break;
    }

    if (new_setting.config >= TRM_ANT_SWITCH_DIV_CONFIG_MAX)
    {
       return_val  = TRM_ANT_SWITCH_DIV_SET_FAIL_INVALID_INPUT;
       break;
    }

    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
           "TRM_ASDiv: Set Config: Cl: %d, cfg: %d",
           new_setting.client, new_setting.config );

    /* convert 'default' config to init config */
    if (new_setting.config  ==  TRM_ANT_SWITCH_DIV_CONFIG_DEFAULT)
    {
       new_setting.config = as_div.init_setting.configuration;
    }

    trm_asdiv_update_pref_config(new_setting.client, new_setting.config);

    if ( !trm_asdiv_chk_switch_perm(new_setting.client, 
                                 new_setting.is_crisis_mode) ) 
    {
      return_val = TRM_ANT_SWITCH_DIV_SET_CFG_DENIED;
      break;
    }

    /* check if it is dual tx scenario and inform the passive client
         about the switch initiation*/
    trm_asdiv_report_switch_initiation( new_setting.client,
                                        new_setting.config );

    /* save the current config before switching, change current status to
     in transition and assign next_config with requested config */
    as_div.prev_config  = as_div.curr_setting.configuration;
    as_div.curr_setting.configuration = SWITCH_IN_TRANSITION;
    as_div.next_config	= new_setting.config;
    
    /* change requesting client requested config attribute */
    as_div.client[new_setting.client].req_config = new_setting.config;

    /* update switching tech and requesting tech */
    as_div.switching_tech = new_setting.client;
    as_div.requesting_tech = new_setting.client;

    return_val = TRM_ANT_SWITCH_DIV_SET_SUCCESS;

    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
           "TRM_ASDiv: Set Config Success: Cl: %d, cfg: %d",
           new_setting.client, new_setting.config );

  }while(0);
  
  ASDIV_EXIT_CRIT_SECT(as_div.crit_sect);

  return return_val;
}

/*============================================================================

FUNCTION TRM_ASD_SET_MODE_AND_CONFIG

DESCRIPTION
  L1s update the mode which they are entering: SLEEP, RX or TX. Depending on
  the mode, set mode logic could initiate call-back to other techs.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/

void trm_asd_set_mode_and_config
(
   const trm_set_mode_and_config_input_t 	*input_data
)
{
  trm_client_enum_t                   cl, tx_cl;
  trm_asd_set_mode_and_config_type    cb_info;
  trm_ant_switch_cb_info_type         sw_cb_info;

  /* check for null pointer */
  if (input_data == NULL)
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
         "TRM_ASDIV: set_mod_cfg: NULL pointer" );
    return;
  }

  ASDIV_ENTER_CRIT_SECT(as_div.crit_sect);

  /* validate info */
  if (!trm_asdiv_validate_client_info(input_data))
  {
    ASDIV_EXIT_CRIT_SECT(as_div.crit_sect);
    return;
  }

  MSG_3( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
         "TRM_ASDIV: set mode config; Cl: %d, mode:%d config:%d",
         input_data->client, input_data->mode, input_data->config);
  /* validation successful */
  cl = input_data->client;
  cb_info.set_config_grant = TRM_ANT_SWITCH_DIV_SET_SUCCESS;
  cb_info.set_mode_grant = TRM_SET_MODE_GRANTED;
  cb_info.config = as_div.curr_setting.configuration;
  cb_info.tag = input_data->tag;

  /* update the preferred config of the client */
  trm_asdiv_update_pref_config(cl, input_data->config);

  /* update the active list */
  trm_asdiv_update_actv_list_tech_info(input_data->client);

  /* update the priority mask in the list */
  trm_asdiv_update_actv_list_prio_info();

  /* check if switching is required*/
  if( as_div.curr_setting.configuration == as_div.client[cl].pref_config_info.switch_config &&
      !as_div.idl_idl_col_data.limit_reached )
  {

     /* set_config grant is always DENIED for sleep unless passive switching
        is involved*/
    if (input_data->mode == TRM_CLIENT_MODE_SLEEP)
    {
      cb_info.set_config_grant = TRM_ANT_SWITCH_DIV_SET_CFG_DENIED;
    }

  }
  /* check if switchin is allowed */
  else if (trm_asdiv_chk_switch_perm(cl, FALSE))
  {
    /* check if mode is sleep */
    if (input_data->mode == TRM_CLIENT_MODE_IDLE_WAKEUP)
    {
      /* client can be given its preferred antenna */
      cb_info.config = as_div.client[cl].pref_config_info.switch_config;

      //populate the config with clients preferred config
      if (as_div.idl_idl_col_data.limit_reached == TRUE)
      {
        /* idle/idle colision limit had reached,
           so update the config with denied client's pref config*/
        cb_info.config = as_div.idl_idl_col_data.denied_config;

        /* reset idle collision data */
        as_div.idl_idl_col_data.limit_reached = FALSE;
        as_div.idl_idl_col_data.denied_cl = TRM_NO_CLIENT;

        MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
               "TRM_ASDIV: denied cl:%d's cfg:%d being granted due to denial limit",
               as_div.idl_idl_col_data.denied_cl, cb_info.config );

      }
      else if (as_div.idl_idl_col_data.denied_cl == cl)
      {
        /* current colision data contains this client,
           since it is granted its pref config, reset col data */
        as_div.idl_idl_col_data.denied_cl = TRM_NO_CLIENT;
      }

      /* send the switch event */
      if ( cb_info.config != as_div.curr_setting.configuration) 
      {

         as_div.event_info.client_id = (uint8) cl;
         as_div.event_info.old_switch_config = (uint8) as_div.curr_setting.configuration;
         as_div.event_info.new_switch_config = (uint8) cb_info.config;
         as_div.event_info.event_type = TRM_ASDIV_EVENT_SWITCHING_IDLEWAKEUP;

         event_report_payload((event_id_enum_type) EVENT_MCS_TRM_ASDIV,
                              sizeof(trm_asdiv_event_info_type),
                              &as_div.event_info );

      }

      /* update current switch configuration */
      as_div.curr_setting.configuration = cb_info.config;

    }
    else if (input_data->mode == TRM_CLIENT_MODE_SLEEP)
    {
      cb_info.set_config_grant = TRM_ANT_SWITCH_DIV_SET_CFG_DENIED;

      if ( as_div.client[cl].pref_config_info.switch_config != as_div.curr_setting.configuration &&
           as_div.ignore_high_prio_sleep != TRUE )
      {
        /* get tech in tx and request for switching */
        tx_cl = trm_asdiv_get_tx_client(cl);
        if ( TRM_ASDIV_IS_TECH_VALID(tx_cl) &&
            as_div.client[tx_cl].ant_switch_cb != NULL)
        {
          /* update switching and requesting tech */
          as_div.switching_tech = tx_cl;
          as_div.requesting_tech = cl;

          sw_cb_info.action = TRM_ANT_SWITCH_INITIATE_SET_CONFIG;
          sw_cb_info.config = as_div.client[cl].pref_config_info.switch_config;
          sw_cb_info.feature_override = TRM_ANT_SWITCH_FEATURE_OVERRIDE_UNSET;
          sw_cb_info.client = tx_cl;

          /* move switch to transition */
          as_div.prev_config  = as_div.curr_setting.configuration;
          as_div.curr_setting.configuration = SWITCH_IN_TRANSITION;
          as_div.next_config	= sw_cb_info.config;

          as_div.client[tx_cl].ant_switch_cb(sw_cb_info);

          cb_info.set_config_grant = TRM_ANT_SWITCH_SET_CFG_BY_TX_TECH;

          as_div.client[tx_cl].req_config = as_div.next_config;

          MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
                 "TRM_ASDIV: Passive switch req to Cl: %d, for cl:%d",
                 tx_cl, input_data->client );
        }
        else
        {
          MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
                 "TRM_ASDIV: set_mod_cfg: NULL cb cl:%d", tx_cl);
        }
      }
    }
    else
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "TRM_ASDIV: set_mod_cfg: Unexpected mode cl:%d mode:%d",
             cl, input_data->mode );
    }
  }
  else
  {
    /* Switching is not allowed, so give current config */
    cb_info.config = as_div.curr_setting.configuration;

    /* check if idle/idle collision and update collision data*/
    if (as_div.idl_idl_col_data.is_idl_idl_col == TRUE)
    {
      as_div.idl_idl_col_data.is_idl_idl_col = FALSE;
      trm_asdiv_update_idle_col_data(cl);
      /* Reset col data if the denied client's preferred
         config is being given */
      if (as_div.client[cl].pref_config_info.switch_config == cb_info.config)
      {
        as_div.idl_idl_col_data.denied_cl = TRM_NO_CLIENT;
      }
    }

    /* set_config is always SUCCESS for idle_wakeup */
    if (input_data->mode == TRM_CLIENT_MODE_SLEEP)
    {
      cb_info.set_config_grant = TRM_ANT_SWITCH_DIV_SET_CFG_DENIED;
    }
    /* change the switch config to current config*/

  }

  /* update tech's current mode */
  as_div.client[cl].curr_mode = as_div.client[cl].req_mode;

  MSG_3( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
         "TRM_ASDIV: set mode cfg; Cl: %d, pref_cfg:%d cfg_givn:%d",
         input_data->client, as_div.client[cl].pref_config_info.switch_config, cb_info.config);

  /* call Grant call back */
  as_div.client[cl].set_mode_cfg_cb(cb_info);

  ASDIV_EXIT_CRIT_SECT(as_div.crit_sect);

}


/*============================================================================

FUNCTION TRM_ASDIV_REQUEST_SWITCH_STATE_UPDATE

DESCRIPTION
  L1s can request for switch state update. Currently supported switch states:
    LOCK STATE: if successful, TRM denies following switching requests
                irrespective of the priority till it is updated back to
                UNLOCK STATE
 
    UNLOCK STATE: switching decisions are now based on priority and modes
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
trm_asdiv_switch_state_update_return_info_t trm_asdiv_request_switch_state_update
(
   trm_asdiv_switch_state_update_request_info_t* update_info
)
{
  trm_asdiv_switch_state_update_return_info_t ret_info;

  trm_client_enum_t     client_id;
  trm_asdiv_switch_state_update_reason_t  reason;

  /* check for the NULL pointer */
  if( update_info == NULL )
  {
    ret_info.update_result = TRM_ASDIV_SWITCH_STATE_UPDATE_FAILED;
    ret_info.fail_reason = TRM_ASDIV_STATE_UPDATE_FAIL_NONE;
    return ret_info;
  }
  
  ASDIV_ENTER_CRIT_SECT(as_div.crit_sect);

  do
  {

    client_id = update_info->client;
    reason = update_info->update_reason;

    ret_info.update_result = TRM_ASDIV_SWITCH_STATE_UPDATE_FAILED;
    
    /* check if ASDiv feature is supported */
    if (!TRM_ASDIV_IS_FEATURE_SUPPORTED)
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "TRM_ASDIV: Feature not supported!" );
      ret_info.fail_reason = TRM_ASDIV_STATE_UPDATE_FAIL_FEAT_NOT_SUPPORTED;
      break;

    }

    /* check for the valid info */
    if( !TRM_ASDIV_IS_TECH_VALID(update_info->client) ||
        update_info->request_type >= TRM_ASDIV_UPDATE_REQUEST_INVALID ||
        update_info->update_reason >= TRM_ASDIV_STATE_UPDATE_REASON_MAX )
    {
      MSG_3( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "TRM_ASDIV: updt sw state invalid info; client: %d, req:%d reason:%d",
             update_info->client, update_info->request_type, update_info->update_reason);
      ret_info.fail_reason = TRM_ASDIV_STATE_UPDATE_FAIL_INVALID_INFO;
      break;
    }

    /* update return info with success */
    ret_info.update_result = TRM_ASDIV_SWITCH_STATE_UPDATE_SUCCESS;
    ret_info.fail_reason = TRM_ASDIV_STATE_UPDATE_FAIL_NONE;

    switch(update_info->request_type)
    {

    case TRM_ASDIV_SWITCH_LOCK_REQUEST:

      /* if request is to lock the switch, then check if the update
         can be done when switch is in transition.
         if it can be simply update the state
         else update only if switch is not in transition */
      if( update_info->update_if_switch_in_progress == TRUE ||
          ( update_info->update_if_switch_in_progress == FALSE &&
            !ASD_SWITCH_IN_TRANSITION ))
      {
        TRM_SET_BIT_ON(as_div.client[client_id].switch_lock_mask, reason);
        trm_asdiv_update_switch_lock_mask();
      }
      else
      {
        ret_info.update_result = TRM_ASDIV_SWITCH_STATE_UPDATE_FAILED;
        ret_info.fail_reason = TRM_ASDIV_STATE_UPDATE_FAIL_SWITCH_IN_PROGRESS;
      }
      break;


    case TRM_ASDIV_SWITCH_UNLOCK_REQUEST:
      /* for unlock state update simply clear the lock state for the reason */
      TRM_SET_BIT_OFF(as_div.client[client_id].switch_lock_mask, reason);
      trm_asdiv_update_switch_lock_mask();
      break;

    default:
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
         "TRM_ASDIV: updt sw state unexpected" );
      break;

    }

  }while(0);

  MSG_3( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
         "TRM_ASDIV: update sw state; client: %d, upd_req:%d reason:%d",
         client_id, update_info->request_type, reason);
  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
         "Update_result: %d, fail_reason:%d",
         ret_info.update_result, ret_info.fail_reason);

  ASDIV_EXIT_CRIT_SECT(as_div.crit_sect);

  return ret_info;
}

/*============================================================================

FUNCTION TRM_ASDIV_UPDATE_TECH_STATE_INFO

DESCRIPTION
  This API is called by limitsmgr to update the tech state
   1) macro proirity
   2) sub id
   3) list of techs based on priority
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
void trm_asdiv_update_tech_state_info
(
  trm_asdiv_tech_state_info_type* state_info
)
{

  if ( state_info == NULL ) 
  {
     MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
          "TRM_ASDIV: tech state update NULL pointer" );
     return;
  }

  ASDIV_ENTER_CRIT_SECT(as_div.crit_sect);

  MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
       "TRM_ASDIV: tech state updated" );

  /* copy the tech state */
  memscpy(&as_div.tech_state_info, sizeof(trm_asdiv_tech_state_info_type),
          state_info, sizeof(trm_asdiv_tech_state_info_type) );

  /* update the priority information of active techs */
  trm_asdiv_update_actv_list_prio_info();

  ASDIV_EXIT_CRIT_SECT(as_div.crit_sect);

}

/*============================================================================

FUNCTION TRM_ASDIV_UPDATE_NEW_MODE

DESCRIPTION
  This API is called by TRM whenever device mode changes. 
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
void trm_asdiv_update_new_mode
( 
  uint8              trm_mode,
  trm_slte_bitmask   dr_mask 
)
{
  ASDIV_ENTER_CRIT_SECT(as_div.crit_sect);  

  /* if mode is 0 and DR mask is not enabled, set ignore flag */
  if ( trm_mode == 0 &&
       !(dr_mask & TRM_DR_DSDS_ENABLED) ) 
  {
    as_div.ignore_high_prio_sleep = TRUE;
  }
  else
  {
    as_div.ignore_high_prio_sleep = FALSE;
  }

  ASDIV_EXIT_CRIT_SECT(as_div.crit_sect);
}

/*============================================================================

FUNCTION TRM_ASDIV_SET_PREF_CONFIG_TEST

DESCRIPTION
  OFFTARGET API: is used to set the preferred config of a client
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
void trm_asdiv_set_pref_config_test
(
   trm_client_enum_t                cl, 
   trm_ant_switch_div_config_type   cfg
)
{

#ifdef TEST_FRAMEWORK
   #error code not present
#else
  MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
         "TRM_ASDIV SET_CFG_TEST: unexpected call to offtarget test api" );

#endif
}

/*============================================================================

FUNCTION TRM_ASDIV_GET_PREF_CONFIG_INFO_TEST

DESCRIPTION
  OFFTARGET API: is used to get the perferred config information  
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
void trm_asdiv_get_pref_config_info_test
(
   trm_client_enum_t                cl, 
   trm_ant_switch_div_config_type   *cfg,
   rfm_device_enum_type             *dev
)
{

#ifdef TEST_FRAMEWORK
   #error code not present
#else
  MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
         "TRM_ASDIV SET_CFG_TEST: unexpected call to offtarget test api" );

#endif
}
