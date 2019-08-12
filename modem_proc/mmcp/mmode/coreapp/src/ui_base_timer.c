#include <stringl/stringl.h>
#include "ui_base_timer.h"
#include "msg.h"
#include "csg_app_cm_if.h"
#include "mcfg_fs.h"   /* Interface to MCFG efs wrappers */

extern ui_base_csg_config_global_s_type ui_base_csg_config_global;
/*===========================================================================
  FUNCTION ui_base_timer_init()

  DESCRIPTION
    This function attempts to Init the global csg config and init timer.
    
  PARAMETERS
    NONE

  RETURN VALUE
    TRUE if timer is initialized with valid interval, otherwise FALSE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/

void ui_base_timer_init()
{
  memset( &ui_base_csg_config_global, 0x00, sizeof(ui_base_csg_config_global_s_type) );
  ui_base_csg_config_global.timer.interval = CSG_SEARCH_DEFAULT_TIMER;
  timer_def( &ui_base_csg_config_global.timer.timer,               // timer object
             NULL,                                  // timer group (needed if timer is critical and needs to expire while device is in sleep)
             NULL,                                  // task to be called when timer expires (needed when signal parameter is provided, not needed with callback function)
             0,                                     // signal to be generated when timer expires
             ui_base_timer_cmd_cb,                  //call back function
             (timer_cb_data_type)TUI_CSG_TIMER_ID       // timer id
           );

  ui_base_read_csg_config( );
  if(ui_base_csg_config_global.is_efs_read && ui_base_csg_config_global.timer.is_valid)
  {
    TUI_MSG_MED_1("Starting Timer with interval = %d", ui_base_csg_config_global.timer.interval);
    timer_set( &ui_base_csg_config_global.timer.timer, ui_base_csg_config_global.timer.interval, 0, T_MIN);
    csg_app_cm_send_select_config_update_ind();
  }
}

/*===========================================================================
  FUNCTION ui_base_read_csg_config()

  DESCRIPTION
    This function attempts to read the EFS file to get the valid csg configuration and interval.
    
  PARAMETERS
    NONE

  RETURN VALUE
    TRUE if timer is initialized with valid interval, otherwise FALSE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/

boolean ui_base_read_csg_config( void )
{
  boolean ret = TRUE;
  mcfg_fs_status_e_type status = MCFG_FS_STATUS_OK;
  
  if( !ui_base_csg_config_global.is_efs_read ) // we haven't read the timer interval yet, read EFS file and start timer
  {
    // read efs file and mark is_efs_read as TRUE so that EFS file is not attempted to read next time
    memset(&ui_base_csg_config_global.csg_search_select_config,0x0,sizeof(ui_base_csg_config_global.csg_search_select_config));
    ui_base_csg_config_global.timer.is_valid = FALSE;
    ui_base_csg_config_global.timer.interval = 0;
    status = mcfg_fs_read (TUI_CSG_CONFIG_EFS_FILE, 
                           &ui_base_csg_config_global.csg_search_select_config, 
                           sizeof(csg_search_select_config_s_type), 
                           MCFG_FS_TYPE_EFS, 
                           MCFG_FS_SUBID_0 );

    if( status == MCFG_FS_STATUS_OK )
    {
      TUI_MSG_HIGH_6("TUI interval %d in minutes, mode=%d,list type %d,network_type=%d,band_pref %ld,select_config = %d", 
                      ui_base_csg_config_global.csg_search_select_config.search_timer, 
                      ui_base_csg_config_global.csg_search_select_config.search_mode,
                      ui_base_csg_config_global.csg_search_select_config.list_type, 
                      ui_base_csg_config_global.csg_search_select_config.network_type,
                      ui_base_csg_config_global.csg_search_select_config.umts_band_pref, 
                      ui_base_csg_config_global.csg_search_select_config.select_config);
      ui_base_csg_config_global.is_efs_read = TRUE;
      ui_base_csg_config_global.timer.is_valid = TRUE;
      if( ui_base_csg_config_global.csg_search_select_config.search_timer > 0) 
      { 
        ui_base_csg_config_global.timer.interval = (timetick_type)ui_base_csg_config_global.csg_search_select_config.search_timer;
      }
    }
    else
    {
      TUI_MSG_ERROR_1("mcfg_fs_read timer read failed status %d", status);
      ret = FALSE;
    }
	status = (int)ret;
  }

  return status;
}


/*===========================================================================
  FUNCTION ui_base_write_csg_config()

  DESCRIPTION
    This function attempts to write the EFS file and update the valid csg configuration and interval.
    
  PARAMETERS
    NONE

  RETURN VALUE
    TRUE if timer is initialized with valid interval, otherwise FALSE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/

boolean ui_base_write_csg_config( const csg_search_select_config_s_type *data_ptr,int size )
{

	mcfg_fs_status_e_type status;
    
    TUI_MSG_HIGH_1("Inside ui_base_write_csg_config size = %d", size);
	status = mcfg_fs_write(TUI_CSG_CONFIG_EFS_FILE, 
                           (void*)data_ptr, 
                           size,
                           MCFG_FS_O_RDWR|MCFG_FS_O_CREAT|MCFG_FS_O_TRUNC|MCFG_FS_O_AUTODIR, 
                           MCFG_FS_ALLPERMS, 
                           MCFG_FS_TYPE_EFS, 
                           MCFG_FS_SUBID_0);

    if ( status != MCFG_FS_STATUS_OK )
    {
      TUI_MSG_ERROR_2("mcfg fs write fail status=%d, errno %d in ui_base_write_csg_config", status, mcfg_fs_errno(MCFG_FS_TYPE_EFS));
      ui_base_csg_config_global.is_efs_read = FALSE;
      ui_base_csg_config_global.timer.is_valid = FALSE;
      ui_base_csg_config_global.timer.interval = 0;
      return FALSE;
    }
    else
    {
      TUI_MSG_HIGH_6("Original interval %d in minutes, mode=%d, list type %d,network_type=%d, band_pref=%ld,select_config = %d", 
                      ui_base_csg_config_global.csg_search_select_config.search_timer, 
                      ui_base_csg_config_global.csg_search_select_config.search_mode, 
                      ui_base_csg_config_global.csg_search_select_config.list_type, 
                      ui_base_csg_config_global.csg_search_select_config.network_type, 
                      ui_base_csg_config_global.csg_search_select_config.umts_band_pref, 
                      ui_base_csg_config_global.csg_search_select_config.select_config);
      memscpy(&ui_base_csg_config_global.csg_search_select_config,sizeof(csg_search_select_config_s_type), data_ptr,size);
      TUI_MSG_HIGH_6("Modified interval %d in minutes, mode=%d, list type %d,network_type=%d, band_pref=%ld,select_config = %d", 
                      ui_base_csg_config_global.csg_search_select_config.search_timer, 
                      ui_base_csg_config_global.csg_search_select_config.search_mode, 
                      ui_base_csg_config_global.csg_search_select_config.list_type, 
                      ui_base_csg_config_global.csg_search_select_config.network_type,
                      ui_base_csg_config_global.csg_search_select_config.umts_band_pref, 
                      ui_base_csg_config_global.csg_search_select_config.select_config);
      ui_base_csg_config_global.is_efs_read = TRUE;
      ui_base_csg_config_global.timer.is_valid = TRUE;
      if( ui_base_csg_config_global.csg_search_select_config.search_timer > 0) 
      { 
        ui_base_csg_config_global.timer.interval = (timetick_type)ui_base_csg_config_global.csg_search_select_config.search_timer;
      }
    }
    return TRUE;
  }

/*===========================================================================
  FUNCTION ui_base_timer_expiry_hdlr(boolean is_app_triggered)

  DESCRIPTION
    This function attempts to handle the timer expiry and do periodic search based on csg configuration.
    
  PARAMETERS
    NONE

  RETURN VALUE
    TRUE if timer is initialized with valid interval, otherwise FALSE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/

void ui_base_timer_expiry_hdlr(boolean is_app_triggered)
{
  //Todo Naveen
  
  csg_app_cm_trigger_search(is_app_triggered,FALSE, FALSE);
}

/*===========================================================================
  FUNCTION QMI_NAS_TIMER_CMD_CB()

  DESCRIPTION
    Handle QMI NAS TIMER command callback, queues an item to MMODE_QMI task

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void ui_base_timer_cmd_cb( timer_cb_data_type  user_data )
{  

  TUI_MSG_HIGH_1("ui_base_timer_cmd_cb()  userdata %d", (uint32)user_data);
  if ( (uint32)user_data == TUI_CSG_TIMER_ID ) 
  {

     UI_BASE_SET_SIGNAL(UI_BASE_TASK_TIMER_SIGNAL);
  }
}

/*===========================================================================
  FUNCTION ui_base_is_timer_running()

  DESCRIPTION
    This functions determines if the timer is running.

  PARAMETERS
    None

  RETURN VALUE
    TRUE if timer is active, FALSE otherwise (even if timer is paused)

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean ui_base_is_timer_running(void)
{
  return timer_is_active( &ui_base_csg_config_global.timer.timer );
}

/*===========================================================================
  FUNCTION ui_base_timer_reset()

  DESCRIPTION
    This functions resets the timer to  specified amount of time.

  PARAMETERS
    None

  RETURN VALUE
    TRUE if timer is active, FALSE otherwise (even if timer is paused)

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void ui_base_timer_reset(uint16 interval)
{
  TUI_MSG_HIGH("ui_base_timer_reset in TUI task");
  timer_set( &ui_base_csg_config_global.timer.timer, interval, 0, T_MIN);
}

void ui_base_timer_stop(void)
{
  if(ui_base_is_timer_running())
  {
    TUI_MSG_HIGH("ui_base_timer_stop in TUI task");
    timer_clr( &ui_base_csg_config_global.timer.timer, T_MIN);
  }
}

