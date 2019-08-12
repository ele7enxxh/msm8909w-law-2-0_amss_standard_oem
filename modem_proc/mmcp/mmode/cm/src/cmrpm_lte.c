/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

   RPM   M O D U L E  - LTE

GENERAL DESCRIPTION
  This module contains the Call Manager RPM functionality for maintain a list of permanent EMM failure events.

  The  RPM modile  is responsible for:
  1. Maintaining the RPM related parameters 
  2. Receiving the info related to EMM failure from NAS and processing
     the info as per the RPM ploicy.


INITIALIZATION AND SEQUENCING REQUIREMENTS
  cmrpm_lte_init() must be called to initialize this module before any other
  function declared in this module is being called.


Copyright (c) 2014 - 2015 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
01/14/15   sk     Integrated RPM feature for EMM reject cause 
01/14/15   sk     Add interface for storing/retrieving the values into efs files

===========================================================================*/


/****************************************************************************

 Organization of the file:

    The file is divided into multiple sections.
    You can jump from one sections to the other by searching for / followed
    by 2 *'s. The order of objects defined is as follows:

        includes
        forward declarations
        #defines
        enums
        macros

        functions
        - Internal [Common, 1x only, GW only]
        - External [Common, 1x only, GW only]

****************************************************************************/

/**--------------------------------------------------------------------------
** Includes
** --------------------------------------------------------------------------
*/

#include "policyman.h"

#include "cmrpm_lte.h"
#include "cmmmgsdi.h"
#include "cmtaski.h"
#include "cmdbg.h"     /* Interface to CM debug services */
#include "mm.h"
#include "cm_msgr_msg.h"
#include "cmefs.h"
#include "cm_v.h"


#ifdef CM_DEBUG
#error code not present
#endif

/**--------------------------------------------------------------------------
** Forward declarations
** --------------------------------------------------------------------------
*/

#define CM_RPM_LTE_MAX_EVENT_COUNTER    5 /* Max number of EMM reject event per Tmax duration */
#define CM_RPM_LTE_MAX_LOGTIME          120 /* Timer value in min to be started following permanent EMM reject */

#define IS_VALID_TAC(X)  ( (X) != 0xFFFE && (X) != 0 )
#define INVALID_TAC      0xFFFE
#define INVALID_PLMN     0xFFFFFFFF
#define IS_EMM_GENERIC_CAUSE(X) ( ((X) == ILLEGAL_MS) || ((X) == ILLEGAL_ME) || ((X) == GPRS_SERVICES_NOT_ALLOWED) || ((X) == GPRS_SERVICES_AND_NON_GPRS_SERVICES_NOT_ALLOWED) )
#define IS_EMM_15_CAUSE(X) ( (X) == NO_SUITABLE_CELLS_IN_LA  )



/**--------------------------------------------------------------------------
** Functions - external
** --------------------------------------------------------------------------
*/  
       
/*===========================================================================

FUNCTION cmrpm_lte_ptr

DESCRIPTION
  Return a pointer to the one and only RPM object.


DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
cmrpm_lte_s_type *cmrpm_lte_ptr( void )
{
  static cmrpm_lte_s_type    cmrpm_lte_local;
  /* The one and only lte rpm object */

  return &cmrpm_lte_local;

} /* cmrpm_lte_ptr() */

/*===========================================================================

FUNCTION cmrpm_lte_is_rpm_enabled

DESCRIPTION
  Returns whether rpm is enabled, based on the rpm pointer.
  Meant for use by external functions that don't have much to do with RPM
  but need to know if it is enabled.

DEPENDENCIES
  cmrpm_lte_init should be called before this function.

RETURN VALUE
  TRUE if the rpm pointer indicates that rpm is enabled
  FALSE otherwise

SIDE EFFECTS
  none

===========================================================================*/
boolean cmrpm_lte_is_rpm_enabled( void )
{
  return (cmrpm_lte_ptr()->is_rpm_enabled);
  
} /* cmrpm_lte_is_rpm_enabled() */


/*==========================================================================

FUNCTION cmrpm_lte_read_rpm_enabled_flag

DESCRIPTION
  Function that reads RPM Enabled Flag from PM

RETURN VALUE
  boolean  flag indicating whether RPM Enabled Flag read
  from the PM successfully (TRUE) or unsuccessfully(FALSE).

DEPENDENCIES
  None
===========================================================================*/
static boolean cmrpm_lte_get_pm_rpm_enabled_flag(boolean  *enabled_flag_ptr)
{
  boolean            status = FALSE;
   
  /*
  ** Read the RPM Enabled Flag
  */
  status = policyman_get_boolean_value("cm:rpm_enabled", enabled_flag_ptr);

  CM_MSG_HIGH_1("read_rpm_enabled_flag: PM RPM Enabled Flag is %d", *enabled_flag_ptr);

  return status;

} /* cmrpm_lte_read_rpm_enabled_flag() */


/*===========================================================================

FUNCTION cmrpm_lte_read_efs_sys_time

DESCRIPTION
 This function reads system time from EFS, that was written at last power-down 
 And updates local variable sys_time_efs with the time read from EFS.
 If time read from EFS is greater than uptime returned by API  
 cm_util_get_curr_systime(), we can consider coin cell battery as absent and
 use EFS itself for maintaining time across resets.

DEPENDENCIES
  RPM object must have already been initialized with cmrpm_lte_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
boolean cmrpm_lte_read_efs_sys_time(boolean is_update)
{
  cmrpm_lte_s_type              *rpm_ptr        = cmrpm_lte_ptr();
  uint32                        sys_time_pwr_up ;
  int32                         size_of_timer   = sizeof(uint32);
  boolean                       status = TRUE;
 

  /* Read RPM SYS TIME from NV-EFS */
  cmnv_efs_read(CMNV_EFS_ID_RPM_SYS_TIME,
                (byte *)&sys_time_pwr_up,
                 &size_of_timer);

  CM_MSG_HIGH_3("read_efs: efs system time: %d, curr_systime: %d is_update: %d",
                         sys_time_pwr_up, cm_util_get_curr_systime(), is_update);
  
  /* If time read from EFS(time stored at last power-down) is greater than the 
  ** uptime(time since recent power-up), we can consider time is not persistent
  ** across resets and coin cell battery support is not there. 
  ** In this case, update time to local variable and set is_coin_cell_support 
  ** to FALSE
  */
  if((is_update == TRUE) && size_of_timer > 0 && sys_time_pwr_up > cm_util_get_curr_systime())
  {
    rpm_ptr->is_coin_cell_support = FALSE;
    rpm_ptr->sys_time_efs = (uint32)sys_time_pwr_up;
  }
  else if(size_of_timer < 0)
  {
    status = FALSE;
  }
  CM_MSG_HIGH_2("read_efs: rpm_ptr->sys_time_efs: %d, coin_cell_support: %d",
                                           rpm_ptr->sys_time_efs,
                                           rpm_ptr->is_coin_cell_support);
  return status;
}/* cmrpm_lte_read_efs_sys_time() */

/*===========================================================================

FUNCTION cmrpm_lte_write_efs_sys_time

DESCRIPTION
  This function writes system time at power-down in EFS

DEPENDENCIES
  RPM object must have already been initialized with cmrpm_lte_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
boolean cmrpm_lte_write_efs_sys_time(
  sys_oprt_mode_e_type  prev_oprt_mode,

  sys_oprt_mode_e_type  curr_oprt_mode
)
{
  boolean                  status = TRUE;
  
  cmrpm_lte_s_type         *rpm_ptr        = cmrpm_lte_ptr();
  uint32                    sys_time_pwr_down;
 
  CM_MSG_HIGH_3("write_efs_sys_time: oprt_mode_chg prev %d curr %d is_rpm %d",
                prev_oprt_mode, curr_oprt_mode, cmrpm_lte_is_rpm_enabled());
  CM_MSG_HIGH_3("write_efs_sys_time: sys_time_efs %d coin_cell_support %d systime %d",
  	               rpm_ptr->sys_time_efs, rpm_ptr->is_coin_cell_support, cm_util_get_curr_systime());
  
  /* Check if oprt mode switch is valid and CMRPM is enabled. If so and 
  ** no coin cell battery support, write current system time to EFS
  */
  if((TRUE == cmrpm_lte_is_rpm_enabled()) &&
       prev_oprt_mode == SYS_OPRT_MODE_ONLINE && 
      ((curr_oprt_mode == SYS_OPRT_MODE_OFFLINE)||
       (curr_oprt_mode == SYS_OPRT_MODE_OFFLINE_CDMA)||
       (curr_oprt_mode == SYS_OPRT_MODE_LPM)||
       (curr_oprt_mode == SYS_OPRT_MODE_PWROFF)||
       (curr_oprt_mode == SYS_OPRT_MODE_RESET)))
  {
    if(!cmrpm_lte_read_efs_sys_time(FALSE))
    {
      CM_ERR_0("write_efs_sys_time: Can't read RPM EFS INFO");
    }
    else if(!(rpm_ptr->is_coin_cell_support == TRUE &&
              (rpm_ptr->sys_time_efs == cm_util_get_curr_systime())))
    {
      /* Current system time = time from EFS at power-up + time since power-up
           */
      sys_time_pwr_down = (rpm_ptr->sys_time_efs) + cm_util_get_curr_systime();
      CM_MSG_HIGH_1("write_efs_sys_time: write system time: %d to EFS", sys_time_pwr_down);
	 
      cmnv_efs_write(CMNV_EFS_ID_RPM_SYS_TIME,
                     (byte *)&sys_time_pwr_down,
                      sizeof(uint32));
      status = TRUE;
     } // else if
  }

  return status;

}/* cmrpm_lte_write_efs_sys_time() */


/*===========================================================================

FUNCTION cmrpm_lte_read_efs_rpm_config

DESCRIPTION
  This function reads RPM parameter from EFS

DEPENDENCIES
  RPM object must have already been initialized with cmrpm_lte_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static boolean cmrpm_lte_read_efs_rpm_info(cmrpm_lte_efs_rpm_info_s_type *rpm_info_ptr )
{
  boolean                  status = TRUE;
  int32                    size_of_efs_info = sizeof(cmrpm_lte_efs_rpm_info_s_type);

  /* Read RPM INFO from EFS */
  cmefs_read(CMEFS_LTE_RPM_INFO_2, (byte *)rpm_info_ptr, &size_of_efs_info);

  if(size_of_efs_info <= 0)
  {
    status = FALSE;
    CM_ERR_0("read_efs_rpm_info: read CMEFS_LTE_RPM_INFO_2 failed");
  }  

  return status;
}

/*===========================================================================

FUNCTION cmrpm_lte_write_efs_rpm_config

DESCRIPTION
  This function writes RPM parameter into EFS

DEPENDENCIES
  Phone object must have already been initialized with cmph_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static boolean cmrpm_lte_write_efs_rpm_info(const cmrpm_lte_s_type   *rpm_ptr )
{
  boolean                            status = FALSE;
  cmrpm_lte_efs_rpm_info_s_type       rpm_efs_info;
  uint8                               i=0;

  if(!cmrpm_lte_read_efs_rpm_info(&rpm_efs_info))
  {
    CM_ERR_0("write_efs_rpm_info: Can't read LTE RPM EFS INFO 2");
  }
  else
  {
    /* Update writable RPM config parameter 
      ** update only fixed parameter such as below
      ** is_rpm_enabled, max_num_reset, max_reset_timer, etc.
      */
    rpm_efs_info.is_rpm_enabled      = rpm_ptr->is_rpm_enabled;
    rpm_efs_info.max_event_counter     = rpm_ptr->max_event_counter;
    rpm_efs_info.max_event_timer       = rpm_ptr->max_event_timer;
    rpm_efs_info.last_emm_reject_cause = rpm_ptr->last_emm_reject_cause;
    rpm_efs_info.event_timer           = rpm_ptr->event_timer;
    rpm_efs_info.emm_gen_counter       = rpm_ptr->emm_gen_counter;

    CM_MSG_HIGH_3("write_efs_rpm_info: is_rpm_enabled %d, max_event_counter %d, max_event_timer %d",
                  rpm_efs_info.is_rpm_enabled, rpm_efs_info.max_event_counter, rpm_efs_info.max_event_timer);
    CM_MSG_HIGH_3("write_efs_rpm_info: last_emm_reject_cause %d, event_timer %d, emm_gen_counter %d",
                  rpm_efs_info.last_emm_reject_cause, rpm_efs_info.event_timer, rpm_efs_info.emm_gen_counter);

    for(i=0; i< CM_MAX_NUM_OF_TAC; i++)
    {
      rpm_efs_info.emm_15_reject[i].counter      = rpm_ptr->emm_15_reject[i].counter;
      rpm_efs_info.emm_15_reject[i].lte_tai.tac  = rpm_ptr->emm_15_reject[i].lte_tai.tac;
      rpm_efs_info.emm_15_reject[i].lte_tai.plmn = rpm_ptr->emm_15_reject[i].lte_tai.plmn;
    }

    cmefs_write(CMEFS_LTE_RPM_INFO_2,
                  (byte *)&rpm_efs_info,
                      sizeof(cmrpm_lte_efs_rpm_info_s_type));
    status = TRUE;

  }
  return status;

}


/*===========================================================================

FUNCTION cmrpm_lte_check_sim_changed

DESCRIPTION
  Check if sim is changed. If SIM changed then reset all RPM timers and counters.
  
DEPENDENCIES
  RPM object must have already been initialized with cmrpm_lte_init().

RETURN VALUE
  boolean 

SIDE EFFECTS
  none

===========================================================================*/
static boolean cmrpm_lte_check_sim_changed( void )
{ 
  cmrpm_lte_s_type             *rpm_ptr        = cmrpm_lte_ptr();
  boolean                       is_sim_changed = FALSE;
  uint32                        curr_system_time  = cm_util_get_curr_systime();

  if (mmgsdi_rpm_has_iccid_changed(MMGSDI_SLOT_1, &is_sim_changed) 
         == MMGSDI_SUCCESS)
    {
    CM_MSG_HIGH_1("check_sim_changed: is_sim_changed: %d",
        is_sim_changed);
    
    if (is_sim_changed)
    {
    return TRUE;
    }
    
  }  
  else
  {
    CM_ERR_0("check_sim_changed: Failed to query SIM changed or not");
  }
  return FALSE;
}


/*===========================================================================

FUNCTION cmrpm_lte_check_timer_expired

DESCRIPTION
  Check if RPM timers are expired. 
  
DEPENDENCIES
  RPM object must have already been initialized with cmrpm_lte_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static boolean cmrpm_lte_check_timer_expired( void )
{
  cmrpm_lte_s_type              *rpm_ptr        = cmrpm_lte_ptr();
  uint32                     curr_system_time   = cm_util_get_curr_systime();
  uint8                      i = 0;

  CM_MSG_MED_3("check_timer: sys_time_efs: %d coin_cell_support: %d curr_systime: %d",
             rpm_ptr->sys_time_efs, rpm_ptr->is_coin_cell_support, curr_system_time);
  
  /* If no coin cell battery, add EFS time to current uptime to get total
  ** system time since power-up
  */
  if(rpm_ptr->is_coin_cell_support == FALSE )
  {
    curr_system_time = (rpm_ptr->sys_time_efs) + curr_system_time;
  }

  /* 
  ** Check EVENT_TIMER if it's already expired or not 
  */
  if((rpm_ptr->event_timer!= 0) && 
     (rpm_ptr->event_timer <= curr_system_time))
  {
    CM_MSG_HIGH_2("check_timer: Event Timer expired - curr_systime:%d, timer:%d",
      curr_system_time, rpm_ptr->event_timer);
    
    return TRUE;
    }
    
  /* Event Timer is not expired */
  return FALSE;
}/* cmrpm_lte_check_timer_expired */


/*===========================================================================

FUNCTION cmrpm_lte_init

DESCRIPTION
  Initializes the RPM object.

  This function must be called before the before RPM object
  is being used, in any way, place, or form.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
boolean cmrpm_lte_init( void )
{
  boolean                  is_lte_rpm = FALSE;
  boolean                  is_pm_rpm_enabled = FALSE;
  boolean                  is_gcf_enabled = FALSE;
  
  boolean                  efs_read_status = FALSE;
  boolean                  efs_time_read_status = FALSE;
  uint8                    i=0;
  cmrpm_lte_s_type         *rpm_ptr       = cmrpm_lte_ptr();

  cmrpm_lte_efs_rpm_info_s_type read_rpm_efs_info;

  /* Initialize all RPM parameters */
  memset(rpm_ptr, 0 , sizeof(cmrpm_lte_s_type));
  memset(&read_rpm_efs_info, 0 , sizeof(cmrpm_lte_efs_rpm_info_s_type));
  rpm_ptr->is_coin_cell_support = TRUE;

  /* Read RPM items and system time from EFS at init*/
  efs_read_status = cmrpm_lte_read_efs_rpm_info(&read_rpm_efs_info);
  efs_time_read_status = cmrpm_lte_read_efs_sys_time(TRUE);
  cmrpm_lte_get_pm_rpm_enabled_flag(&is_pm_rpm_enabled);
  is_gcf_enabled = *(cmph_get_gprs_anite_gcf_ptr());

  if( (TRUE == efs_read_status) &&
      (TRUE == efs_time_read_status) )
  {
     CM_MSG_HIGH_3("cmrpm_lte_init: is_rpm_enabled %d, is_pm_rpm_enabled %d, is_gcf_enabled %d",
                    read_rpm_efs_info.is_rpm_enabled, is_pm_rpm_enabled, is_gcf_enabled);

  /* RPM functionality is only enabled when GCF flag is OFF/FALSE */
      /* RPM functionality is only enabled when RPM SIM exist */
    if( (FALSE == is_gcf_enabled) && \
        (TRUE  == is_pm_rpm_enabled) && \
        (TRUE  == read_rpm_efs_info.is_rpm_enabled) )
  {
      CM_MSG_MED_0("cmrpm_lte_init: RPM Initializing...");
      is_lte_rpm = TRUE;
      
      rpm_ptr->is_rpm_enabled = read_rpm_efs_info.is_rpm_enabled;
      rpm_ptr->max_event_counter= read_rpm_efs_info.max_event_counter;
      rpm_ptr->max_event_timer= read_rpm_efs_info.max_event_timer;
      rpm_ptr->event_timer = read_rpm_efs_info.event_timer;
      rpm_ptr->last_emm_reject_cause = read_rpm_efs_info.last_emm_reject_cause;
      rpm_ptr->emm_gen_counter = read_rpm_efs_info.emm_gen_counter;

      for (i = 0; i < CM_MAX_NUM_OF_TAC; i++)
        {
        rpm_ptr->emm_15_reject[i].counter      = read_rpm_efs_info.emm_15_reject[i].counter;
        rpm_ptr->emm_15_reject[i].lte_tai.tac  = read_rpm_efs_info.emm_15_reject[i].lte_tai.tac;
        rpm_ptr->emm_15_reject[i].lte_tai.plmn = read_rpm_efs_info.emm_15_reject[i].lte_tai.plmn;
        }
      CM_MSG_HIGH_3("is_rpm %d, max_event_counter %d, max_event_timer %d",
                    rpm_ptr->is_rpm_enabled, rpm_ptr->max_event_counter, rpm_ptr->max_event_timer);
      CM_MSG_HIGH_3("event_timer %d, last_emm_reject_cause %d, emm_gen_counter %d",
                    rpm_ptr->event_timer, rpm_ptr->last_emm_reject_cause, rpm_ptr->emm_gen_counter);
        
      /* Check if sim is changed then reset all RPM timers and counters */
      if( TRUE == cmrpm_lte_check_sim_changed() )
        {
        /* Reset everything and notify NAS to clear the FTAI List */
        cmrpm_lte_reset_efs_info();

        cmrpm_lte_send_update_ftai_list(SYS_RPM_FTAI_LIST_CLEAR);
          }
          
          }
        }
      /* efs read failed, use default RPM config value */  
  else if (FALSE == efs_read_status)
      {
    CM_ERR_0("cmrpm_lte_init: Can't read RPM rpm_efs_info, write default CMNV_EFS_ID_LTE_RPM_INFO");
    rpm_ptr->max_event_counter= CM_RPM_LTE_MAX_EVENT_COUNTER;
    rpm_ptr->max_event_timer = CM_RPM_LTE_MAX_LOGTIME;

        /* first time read, reset  and write default value to efs file */
        memset(&read_rpm_efs_info, 0 , sizeof(cmrpm_lte_efs_rpm_info_s_type));

    read_rpm_efs_info.max_event_counter   = rpm_ptr->max_event_counter;
    read_rpm_efs_info.max_event_timer = rpm_ptr->max_event_timer;

    for(i=0; i< CM_MAX_NUM_OF_TAC; i++)
        {
      read_rpm_efs_info.emm_15_reject[i].lte_tai.tac = INVALID_TAC;
      sys_plmn_undefine_plmn_id(&(read_rpm_efs_info.emm_15_reject[i].lte_tai.plmn));
        }

    cmefs_write(CMEFS_LTE_RPM_INFO_2,
                      (byte *)&read_rpm_efs_info,
                           sizeof(cmrpm_lte_efs_rpm_info_s_type));
     } 
  else
  {
    /* efs_read_status == TRUE && efs_time_read_status == FALSE */
    CM_ERR_0("cmrpm_lte_init: Can't read RPM sys_time");
        }

  return is_lte_rpm;
} /* cmrpm_lte_init */


/*===========================================================================

FUNCTION cmrpm_lte_set_rpm_parameters_req

DESCRIPTION
  Updates value of  CM RPM parameters and updates the EFS as well.
  If there is any RPM timer running currently it will incorporate new value.

DEPENDENCIES

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmrpm_lte_set_rpm_parameters_req( cm_mm_lte_rpm_parameters_s_type set_cm_rpm_parameters_req)
{
  
  cmrpm_lte_s_type *lte_rpm_ptr = cmrpm_lte_ptr();
  cmrpm_lte_s_type  old_rpm_info;

  cm_mm_set_rpm_parameters_rsp_s_type *cm_rpm_set_params_rsp;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    
  cm_rpm_set_params_rsp = (cm_mm_set_rpm_parameters_rsp_s_type *)cm_mem_malloc(
	sizeof(cm_mm_set_rpm_parameters_rsp_s_type));

  if(cm_rpm_set_params_rsp == NULL)
  { 
    CM_ERR_0("set_rpm: cm_rpm_set_params_rsp heap exhausted, cm_mem_malloc() returned NULL");
  }

  if(!lte_rpm_ptr->is_rpm_enabled)
  {
    cm_rpm_set_params_rsp->rpm_rsp_ret_val = CM_RPM_FAILED_RPM_DISABLED;
    
    (void) cm_msgr_send( MM_CM_SET_RPM_PARAMETERS_RSP, MSGR_MM_CM,
        (msgr_hdr_s *)cm_rpm_set_params_rsp, sizeof(cm_mm_set_rpm_parameters_rsp_s_type) );
    
    CM_MSG_HIGH_1("set_rpm: send SET RPM_PARAMETERS_RSP with rsp = %d", \
                     cm_rpm_set_params_rsp->rpm_rsp_ret_val);

    cm_mem_free(cm_rpm_set_params_rsp);

    return;
  }

  /* if new_rpm_config is  set to > 16 and > 120, QMI  itself would
         reject the command so CM does not expect this input at all */
  if(set_cm_rpm_parameters_req.max_event_counter > 16)
  {
    CM_ERR_1("set_rpm: rpm_parameters out of range max_event_counter: %d, set to default value 5",
                   set_cm_rpm_parameters_req.max_event_counter);
    set_cm_rpm_parameters_req.max_event_counter = 5;
  }
  else if(set_cm_rpm_parameters_req.max_event_timer > 360 || \
     set_cm_rpm_parameters_req.max_event_timer == 0)
  {
    CM_ERR_1("set_rpm: rpm_parameters out of range max_event_timer: %d, set to default value 120",
                   set_cm_rpm_parameters_req.max_event_timer);
    set_cm_rpm_parameters_req.max_event_timer   = 120;
  }
  
  /* compute new event timer if  timer is running */
  if(lte_rpm_ptr->event_timer != 0)
  {
    lte_rpm_ptr->event_timer += (set_cm_rpm_parameters_req.max_event_timer - \
                                     lte_rpm_ptr->max_event_timer)* CMRPM_SEC_PER_MIN;
  }
  
  /* fill the structure to be written in EFS from provide new config */
  {
    old_rpm_info.max_event_counter = lte_rpm_ptr->max_event_counter;
    old_rpm_info.max_event_timer = lte_rpm_ptr->max_event_timer;
    
    lte_rpm_ptr->max_event_counter = set_cm_rpm_parameters_req.max_event_counter;
    lte_rpm_ptr->max_event_timer = set_cm_rpm_parameters_req.max_event_timer;
  }

  if (!cmrpm_lte_write_efs_rpm_info(lte_rpm_ptr))
  {
    CM_ERR_0( "set_rpm: efs write for set_rpm_parameters fail");
    cm_rpm_set_params_rsp->rpm_rsp_ret_val = CM_RPM_FAILED_RPM_DISABLED;
    
    /* write failed, restore old rpm config */
    lte_rpm_ptr->max_event_counter = old_rpm_info.max_event_counter;
    lte_rpm_ptr->max_event_timer = old_rpm_info.max_event_timer;
  }
  else
  {
    cm_rpm_set_params_rsp->rpm_rsp_ret_val = CM_RPM_SUCCESS_RPM_ENABLED;
      
    CM_MSG_HIGH_2("set_rpm: set_rpm_parameters updated with max event counter = %d, max event timer = %d", \
                  lte_rpm_ptr->max_event_counter, lte_rpm_ptr->max_event_timer);
  }

  (void) cm_msgr_send( MM_CM_SET_RPM_PARAMETERS_RSP, MSGR_MM_CM,
	                   (msgr_hdr_s *)cm_rpm_set_params_rsp, sizeof(cm_mm_set_rpm_parameters_rsp_s_type) );

  CM_MSG_HIGH_1("set_rpm: send SET RPM_PARAMETERS_RSP with rsp = %d", \
                   cm_rpm_set_params_rsp->rpm_rsp_ret_val);

  cm_mem_free(cm_rpm_set_params_rsp);

  return;
  
}/* cmrpm_lte_set_rpm_parameters_req */

/*===========================================================================

FUNCTION cmrpm_lte_send_get_rpm_parameters_rsp

DESCRIPTION
  Sends the current value of the CM RPM parameters  to 
  CM clients via MSGR interface MM_CM_GET_RPM_PARAMETERS_RSP.

DEPENDENCIES

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmrpm_lte_send_get_rpm_parameters_rsp( void )
{
  cm_mm_get_rpm_parameters_rsp_s_type *cm_rpm_get_params_rsp;
  
  
  cm_rpm_get_params_rsp = (cm_mm_get_rpm_parameters_rsp_s_type *)cm_mem_malloc(
	sizeof(cm_mm_get_rpm_parameters_rsp_s_type));

  if(cm_rpm_get_params_rsp == NULL)
  { 
    CM_ERR_0("get_rpm: cm_rpm_get_params_rsp heap exhausted, cm_mem_malloc() returned NULL");
  }

  if(cmrpm_lte_ptr()->is_rpm_enabled)
  {
    cm_rpm_get_params_rsp->rpm_rsp_ret_val = CM_RPM_SUCCESS_RPM_ENABLED;
    cm_rpm_get_params_rsp->lte_rpm_params.max_event_counter = cmrpm_lte_ptr()->max_event_counter;
    cm_rpm_get_params_rsp->lte_rpm_params.max_event_timer   = cmrpm_lte_ptr()->max_event_timer;
  }
  else
  {
    cm_rpm_get_params_rsp->rpm_rsp_ret_val = CM_RPM_FAILED_RPM_DISABLED;
  }
  (void) cm_msgr_send( MM_CM_GET_RPM_PARAMETERS_RSP, MSGR_MM_CM,
	                   (msgr_hdr_s *)cm_rpm_get_params_rsp, sizeof(cm_mm_get_rpm_parameters_rsp_s_type) );

  CM_MSG_HIGH_3("get_rpm: send GET RPM_PARAMETERS_RSP with rsp = %d, max_event_counter = %d, max_event_timer =%d", \
                cm_rpm_get_params_rsp->rpm_rsp_ret_val,
                cm_rpm_get_params_rsp->lte_rpm_params.max_event_counter,
                cm_rpm_get_params_rsp->lte_rpm_params.max_event_timer);

  cm_mem_free(cm_rpm_get_params_rsp);

  return;
  
} /* cmrpm_lte_send_get_rpm_parameters_rsp */


/*===========================================================================

FUNCTION cmrpm_lte_set_rpm_config_req

DESCRIPTION
  Updates value of  CM RPM config and updates the EFS as well.
  If there is any RPM timer running currently it will be stopped on 
  disabling rpm config.

DEPENDENCIES

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmrpm_lte_set_rpm_config_req( boolean set_rpm_config_req)
{
  
  cmrpm_lte_s_type  *rpm_ptr = cmrpm_lte_ptr();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(rpm_ptr->is_rpm_enabled != set_rpm_config_req)
  {
    rpm_ptr->is_rpm_enabled = set_rpm_config_req;

    /* 
    ** Check EVENT_TIMER if it's already running, reset it 
    */
    if((cmrpm_lte_check_timer_expired() == FALSE) && 
       (set_rpm_config_req == FALSE))
    {
      /* Reset everything and notify NAS to clear the FTAI List */
      CM_MSG_HIGH_0("set_rpm_config: new rpm conf set to FALSE, reset EFS items");
      cmrpm_lte_reset_efs_info();

      cmrpm_lte_send_update_ftai_list(SYS_RPM_FTAI_LIST_CLEAR);
    } 

    /* Write the new is_rpm_enabled setting to EFS */
    if(cmrpm_lte_write_efs_rpm_info(rpm_ptr) == FALSE)
    {
      CM_ERR_0("set_rpm_config: fail to write is_rpm_enabled ");
    }    

  }
  else
  {
    CM_MSG_HIGH_2("set_rpm_config: no change in rpm config old: %d, new config: %d",
                  rpm_ptr->is_rpm_enabled, set_rpm_config_req);
  }

  return;
  
}/* cmrpm_lte_set_rpm_config_req */

/*===========================================================================

FUNCTION cmrpm_lte_update_sim_state

DESCRIPTION
  This function will check whether LTE RPM is enabled to determine which
  SIM state to use based on emm_gen_counter and last_emm_reject_cause.

DEPENDENCIES

RETURN VALUE
  The updated SIM state for SD to use.

SIDE EFFECTS
  none

===========================================================================*/
sys_sim_state_e_type cmrpm_lte_update_sim_state( void )
{
  sys_sim_state_e_type  lte_rpm_sim_state = SYS_SIM_STATE_NONE;
#ifdef FEATURE_CM_LTE
  cmrpm_lte_s_type     *rpm_ptr = cmrpm_lte_ptr();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Change the SIM state only when LTE RPM is enabled, and event timer has not expired.
   */
  if( (TRUE == rpm_ptr->is_rpm_enabled) &&
      (FALSE == cmrpm_lte_check_timer_expired()) )
    {
      switch(rpm_ptr->last_emm_reject_cause)
      {
        case ILLEGAL_MS:
        case ILLEGAL_ME:
        case GPRS_SERVICES_AND_NON_GPRS_SERVICES_NOT_ALLOWED:
          /* If last EMM reject cause is 3/6/8 and emm_gen_counter exceeds maximum value,
           * change the SIM state to CS_PS_INVALID (limited).
           */
          if(rpm_ptr->emm_gen_counter >= rpm_ptr->max_event_counter)
          { 
            CM_MSG_HIGH_1("update_sim_state: emm_gen_counter %d exceed max counter, change SIM state to CS_PS_INVALID",
              rpm_ptr->emm_gen_counter);
            lte_rpm_sim_state = SYS_SIM_STATE_CS_PS_INVALID;
          }
          break;

        case GPRS_SERVICES_NOT_ALLOWED:
          /* If last EMM reject cause is 7 and emm_gen_counter exceeds maximum value,
           * change the SIM state to PS_INVALID (CS_ONLY).
           */
          if(rpm_ptr->emm_gen_counter >= rpm_ptr->max_event_counter)
          { 
            CM_MSG_HIGH_1("update_sim_state: emm_gen_counter %d exceed max counter, change SIM state to PS_INVALID",
              rpm_ptr->emm_gen_counter);
            lte_rpm_sim_state = SYS_SIM_STATE_PS_INVALID;
          }
          break;

        default:
          break;
      }
  }
#endif /* FEATURE_CM_LTE */

  return lte_rpm_sim_state;
  
}/* cmrpm_lte_update_sim_state */

/*===========================================================================

FUNCTION cmrpm_lte_get_srv_domain_pref

DESCRIPTION
  This function will check whether LTE RPM is enabled to determine which
  srv_domain_pref to use based on emm_gen_counter and last_emm_reject_cause.

DEPENDENCIES

RETURN VALUE
  The updated srv_domain_pref for SD to use.

SIDE EFFECTS
  none

===========================================================================*/
sd_ss_srv_domain_pref_e_type cmrpm_lte_get_srv_domain_pref( void )
{
  sd_ss_srv_domain_pref_e_type  lte_rpm_srv_domain_pref = SD_SS_SRV_DOMAIN_PREF_NONE;
#ifdef FEATURE_CM_LTE
  cmrpm_lte_s_type             *rpm_ptr = cmrpm_lte_ptr();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Change the srv_domain_pref only when LTE RPM is enabled, and event timer has not expired.
   */
  if( (TRUE == rpm_ptr->is_rpm_enabled) &&
      (FALSE == cmrpm_lte_check_timer_expired()) )
  {
    switch(rpm_ptr->last_emm_reject_cause)
    {
      case GPRS_SERVICES_NOT_ALLOWED:
        /* If last EMM reject cause is 7 and emm_gen_counter exceeds maximum value,
         * change the srv_domain_pref to CS_ONLY.
         */
        if(rpm_ptr->emm_gen_counter >= rpm_ptr->max_event_counter)
        { 
          CM_MSG_HIGH_1("update_sim_state: emm_gen_counter %d exceed max counter, change SIM state to PS_INVALID",
            rpm_ptr->emm_gen_counter);
          lte_rpm_srv_domain_pref = SD_SS_SRV_DOMAIN_PREF_CS_ONLY;
        }
        break;

      default:
        break;
    }
  }
#endif /* FEATURE_CM_LTE */

  return lte_rpm_srv_domain_pref;
  
}/* cmrpm_lte_get_srv_domain_pref */



/*===========================================================================

FUNCTION cmrpm_lte_process_app_reset

DESCRIPTION
  Process oprt mode change 

DEPENDENCIES
  RPM object must have already been initialized with cmrpm_lte_init().

RETURN VALUE
  TRUE if reset counter is incremented

SIDE EFFECTS
  None
===========================================================================*/
  boolean  cmrpm_lte_process_app_reset(
  cm_ph_cmd_s_type           *ph_cmd_ptr
)
{
  cm_ph_cmd_info_s_type     *cmd_info_ptr = NULL;
  cmrpm_lte_s_type          *rpm_ptr      = cmrpm_lte_ptr();
  boolean                    is_emm_15_counter_reach_max = FALSE;
  uint8                      i=0;

  boolean                    is_lte_rpm = FALSE;
  boolean                    is_pm_rpm_enabled = FALSE;
  boolean                    is_gcf_enabled = FALSE;
  
  boolean                    efs_read_status = FALSE;
  boolean                    efs_time_read_status = FALSE;
  cmrpm_lte_efs_rpm_info_s_type read_rpm_efs_info;

  static const struct {
    
          sys_oprt_mode_e_type    from_oprt_mode;
              /* from operating mode */
    
          sys_oprt_mode_e_type    to_oprt_mode;
              /* to operating mode */
    
      } cmrpm_oprt_mode_tbl[] = {
    
            /* from oprt mode */         /* to oprt mode */
            {SYS_OPRT_MODE_PWROFF,        SYS_OPRT_MODE_ONLINE},
            {SYS_OPRT_MODE_LPM,           SYS_OPRT_MODE_ONLINE},
            {SYS_OPRT_MODE_FTM,           SYS_OPRT_MODE_ONLINE}
           
                };
  
  /* Initialize all RPM parameters */
  memset(rpm_ptr, 0 , sizeof(cmrpm_lte_s_type));
  memset(&read_rpm_efs_info, 0 , sizeof(cmrpm_lte_efs_rpm_info_s_type));
  rpm_ptr->is_coin_cell_support = TRUE;

  /* Read RPM items and system time from EFS at init*/
  efs_read_status = cmrpm_lte_read_efs_rpm_info(&read_rpm_efs_info);
  efs_time_read_status = cmrpm_lte_read_efs_sys_time(TRUE);
  cmrpm_lte_get_pm_rpm_enabled_flag(&is_pm_rpm_enabled);
  is_gcf_enabled = *(cmph_get_gprs_anite_gcf_ptr());

  if( (TRUE == efs_read_status) &&
      (TRUE == efs_time_read_status) )
  {
     CM_MSG_HIGH_3("process_app_reset: is_rpm_enabled %d, is_pm_rpm_enabled %d, is_gcf_enabled %d",
                    read_rpm_efs_info.is_rpm_enabled, is_pm_rpm_enabled, is_gcf_enabled);

    /* RPM functionality is only enabled when GCF flag is OFF/FALSE */
        /* RPM functionality is only enabled when RPM SIM exist */
    if( (FALSE == is_gcf_enabled) && \
        (TRUE  == is_pm_rpm_enabled) && \
        (TRUE  == read_rpm_efs_info.is_rpm_enabled) )
    {
      CM_MSG_MED_0("process_app_reset: RPM Initializing...");
      is_lte_rpm = TRUE;

      rpm_ptr->is_rpm_enabled = read_rpm_efs_info.is_rpm_enabled;
      rpm_ptr->max_event_counter= read_rpm_efs_info.max_event_counter;
      rpm_ptr->max_event_timer= read_rpm_efs_info.max_event_timer;
      rpm_ptr->event_timer = read_rpm_efs_info.event_timer;
      rpm_ptr->last_emm_reject_cause = read_rpm_efs_info.last_emm_reject_cause;
      rpm_ptr->emm_gen_counter = read_rpm_efs_info.emm_gen_counter;

      for (i = 0; i < CM_MAX_NUM_OF_TAC; i++)
      {
        rpm_ptr->emm_15_reject[i].counter      = read_rpm_efs_info.emm_15_reject[i].counter;
        rpm_ptr->emm_15_reject[i].lte_tai.tac  = read_rpm_efs_info.emm_15_reject[i].lte_tai.tac;
        rpm_ptr->emm_15_reject[i].lte_tai.plmn = read_rpm_efs_info.emm_15_reject[i].lte_tai.plmn;
      }
      CM_MSG_HIGH_3("is_rpm %d, max_event_counter %d, max_event_timer %d",
                    rpm_ptr->is_rpm_enabled, rpm_ptr->max_event_counter, rpm_ptr->max_event_timer);
      CM_MSG_HIGH_3("event_timer %d, last_emm_reject_cause %d, emm_gen_counter %d",
                    rpm_ptr->event_timer, rpm_ptr->last_emm_reject_cause, rpm_ptr->emm_gen_counter);
      
    }
  }
  /* efs read failed, use default RPM config value */
  else if (FALSE == efs_read_status)
  {
    CM_ERR_0("process_app_reset: Can't read RPM rpm_efs_info, write default CMNV_EFS_ID_LTE_RPM_INFO");
    rpm_ptr->max_event_counter= CM_RPM_LTE_MAX_EVENT_COUNTER;
    rpm_ptr->max_event_timer = CM_RPM_LTE_MAX_LOGTIME;

    /* first time read, reset and write default value to efs file */
    memset(&read_rpm_efs_info, 0 , sizeof(cmrpm_lte_efs_rpm_info_s_type));

    read_rpm_efs_info.max_event_counter   = rpm_ptr->max_event_counter;
    read_rpm_efs_info.max_event_timer = rpm_ptr->max_event_timer;

    for(i=0; i< CM_MAX_NUM_OF_TAC; i++)
    {
      read_rpm_efs_info.emm_15_reject[i].lte_tai.tac = INVALID_TAC;
      sys_plmn_undefine_plmn_id(&(read_rpm_efs_info.emm_15_reject[i].lte_tai.plmn));
    }

    cmefs_write(CMEFS_LTE_RPM_INFO_2,
                  (byte *)&read_rpm_efs_info,
                  sizeof(cmrpm_lte_efs_rpm_info_s_type));
  }
  else
  {
    /* efs_read_status == TRUE && efs_time_read_status == FALSE */
    CM_ERR_0("cmrpm_lte_init: Can't read RPM sys_time");
  }
  

  /*  App inititated resets from oprt mode OFFLINE/LPM/OFFLINE_CDMA to ONLINE 
   ** are counted for rpm resets
   */
  cmd_info_ptr = CMD_INFO_PTR( ph_cmd_ptr );

  if(rpm_ptr->is_rpm_enabled &&
      (ph_cmd_ptr->client_id != CM_CLIENT_ID_ANONYMOUS) )
    {
      boolean valid_oprt_change = FALSE;
      for( i=0; i < ARR_SIZE(cmrpm_oprt_mode_tbl); i++ )
      {
        if( cmrpm_oprt_mode_tbl[i].from_oprt_mode == cmph_ptr()->oprt_mode &&
            cmrpm_oprt_mode_tbl[i].to_oprt_mode   == cmd_info_ptr->oprt_mode )
        {
          valid_oprt_change =  TRUE;
        }
      }
      
    if( TRUE  == valid_oprt_change )
    {

      /* Check whether Event Timer has expired or not.
      ** If not expired, we need to check if any EMM Reject 15 TAC counter has reached
      ** maximum, and notify NAS to read the FTAI List.
      ** If expired, we need to reset every EFS item and notify NAS to clear the FTAI List
              */
      if(FALSE == cmrpm_lte_check_timer_expired())
        {
        for(i = 0; i < CM_MAX_NUM_OF_TAC; i++)
          {
          if(rpm_ptr->emm_15_reject[i].counter >= rpm_ptr->max_event_counter)
            {
            CM_MSG_HIGH_2("process_app_reset: emm_15_reject[%d].counter = %d exceed max counter, send IND to NAS", 
              i, rpm_ptr->emm_15_reject[i].counter);
              is_emm_15_counter_reach_max = TRUE;
            }
          }
            
        if(is_emm_15_counter_reach_max)
        {
          /* Send IND to NAS to read the FTAI List */
          cmrpm_lte_send_update_ftai_list(SYS_RPM_FTAI_LIST_READ);
        }
          
        }
      else
      {
        /* If event timer has expired, reset everything and notify NAS 
        ** to clear the FTAI List
        */
        cmrpm_lte_reset_efs_info();
        
        cmrpm_lte_send_update_ftai_list(SYS_RPM_FTAI_LIST_CLEAR);
      }
    }
  
  }
  
  return TRUE;

}/* cmrpm_lte_process_app_reset */

/*===========================================================================

FUNCTION cmrpm_lte_app_reset_rejected_proc

DESCRIPTION
  Processes PH cmd from CM when app reset was rejected 

DEPENDENCIES
  RPM object must have already been initialized with cmrpm_lte_init().

RETURN VALUE
None

SIDE EFFECTS
  None
===========================================================================*/

void cmrpm_lte_app_reset_rejected_proc( void )
{
  return;
   }

/*===========================================================================

FUNCTION cmrpm_lte_check_reset_allowed

DESCRIPTION
  Check whether reset is allowed or not when RPM is on

DEPENDENCIES
  RPM object must have already been initialized with cmrpm_lte_init().

RETURN VALUE
TRUE if reset is allowed

SIDE EFFECTS
  None
===========================================================================*/
boolean  cmrpm_lte_check_reset_allowed(
  void
)
 {
  
  return TRUE;
} /* cmrpm_lte_check_reset_allowed */


/*===========================================================================

FUNCTION cmrpm_lte_timer_proc

DESCRIPTION
  Process timer for RPM.

DEPENDENCIES
  RPM object must have already been initialized with cmrpm_lte_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmrpm_lte_timer_proc(
  cm_timer_event_type    timer_event   /* Indicate specific timer event */
)
{
  cmrpm_lte_s_type             *rpm_ptr        = cmrpm_lte_ptr();
  uint32                        curr_sys_time  = cm_util_get_curr_systime();
    
  if(TRUE == cmrpm_lte_check_timer_expired())
    {
    /* Reset all EFS info items */
    cmrpm_lte_reset_efs_info();
      
    /* Notify NAS to clear the FTAI List */
    cmrpm_lte_send_update_ftai_list(SYS_RPM_FTAI_LIST_CLEAR);
  }  
  
} /* cmrpm_lte_timer_proc */


/*===========================================================================

FUNCTION cmrpm_lte_emm_tai_list_ind_proc

DESCRIPTION
  Update the TAI info based on NAS_EMM_TAI_LIST_IND.

DEPENDENCIES

None

RETURNS
  None

SIDE EFFECTS

None

===========================================================================*/
void cmrpm_lte_emm_tai_list_ind_proc (emm_tai_list_ind_type  *emm_tai_list_ind)
{
  uint8 i;

  lte_nas_emm_tai_lst_info_type    *tai_lst_ptr = NULL;

  lte_nas_tai_lst1_type cmrpm_tai;
  
  cmrpm_lte_s_type      *rpm_ptr = cmrpm_lte_ptr();
  
  tai_lst_ptr = &(emm_tai_list_ind->tai_lst_info);

  CM_ASSERT( tai_lst_ptr != NULL );

  if( (TRUE  == rpm_ptr->is_rpm_enabled) &&
      (FALSE == cmrpm_lte_check_timer_expired()) )
  {
    /* Search for valid tai in cmrpm tai list */
    for (i=0; i < CM_MAX_NUM_OF_TAC; i++)
    {
      if(IS_VALID_TAC(rpm_ptr->emm_15_reject[i].lte_tai.tac))
      {
        cmrpm_tai.plmn = rpm_ptr->emm_15_reject[i].lte_tai.plmn;
        cmrpm_tai.tac  = rpm_ptr->emm_15_reject[i].lte_tai.tac;

        CM_MSG_HIGH_2("tai_list_ind: valid cmrpm tac[%d] = %d",
                      i, cmrpm_tai.tac);
        
        CM_MSG_HIGH_3("tai_list_ind: plmn %d %d %d",
                       cmrpm_tai.plmn.identity[0], 
                       cmrpm_tai.plmn.identity[1], 
                       cmrpm_tai.plmn.identity[2]);

        /* Check with NAS on whether the valid tai in cmrpm matches 
         * with the tai reported by NAS and remove it */
        if(emm_search_tai_list(&cmrpm_tai, tai_lst_ptr))
        {
          CM_MSG_HIGH_2("tai_list_ind: tac matches with NAS, emm_15_reject[%d].lte_tai.tac = %d",
               i, rpm_ptr->emm_15_reject[i].lte_tai.tac);

          rpm_ptr->emm_15_reject[i].counter = 0;
          rpm_ptr->emm_15_reject[i].lte_tai.tac = INVALID_TAC;
          sys_plmn_undefine_plmn_id(&(rpm_ptr->emm_15_reject[i].lte_tai.plmn));

        }
        else
        {
          CM_MSG_HIGH_2("tai_list_ind: tac NOT found, tac[%d] = %d",\
               i, rpm_ptr->emm_15_reject[i].lte_tai.tac);
        }
      }
    } /* for () */

  } /* if(rpm_enabled)*/


} /* cmrpm_lte_emm_tai_list_ind_proc */

/*===========================================================================

  FUNCTION cmrpm_lte_sd_rpt_proc

  DESCRIPTION
    Process reports from System Determination.

  DEPENDENCIES
    RPM object must have already been initialized with cmrpm_lte_init().

  RETURN VALUE
    none

  SIDE EFFECTS
    none

===========================================================================*/
void cmrpm_lte_sd_rpt_proc(

  const cm_hdr_type   *rpt_ptr
    /* Pointer to SD report */
)
{
#ifdef FEATURE_CM_LTE
  const cm_sd_rpt_u_type         *sd_rpt_ptr = (cm_sd_rpt_u_type *) rpt_ptr;
    /* Pointer to SD reports */

  cmrpm_lte_s_type                *rpm_ptr   = cmrpm_lte_ptr();
    /* Point at rpm  object */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  CM_ASSERT( rpm_ptr != NULL );
  CM_ASSERT( sd_rpt_ptr != NULL );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* This is serving system change notifiction
  ** If UE is registered in both CS and PS:
  ** 1. Reset all LTE RPM timer and counters 
  ** 2. Send CLEAR IND to NAS to clear the FTAI list
  */
  switch( sd_rpt_ptr->hdr.cmd )
  {
    /* Service Indicators information is changed.
    */
    case CM_SRV_IND_INFO_F:
        if((sd_rpt_ptr->srv_ind_info.si_info.srv_status == SYS_SRV_STATUS_SRV) && 
           (sd_rpt_ptr->srv_ind_info.si_info.srv_domain == SYS_SRV_DOMAIN_CS_PS))
        {
        /* Reset all LTE RPM timer and counters */
        cmrpm_lte_reset_efs_info();
          
        /* Send CLEAR IND to NAS to clear the FTAI list */
        cmrpm_lte_send_update_ftai_list(SYS_RPM_FTAI_LIST_CLEAR);          
      }
      break;
         
    default:
      break;
  } /* switch( sd_rpt_ptr->hdr.cmd ) */

#endif
  return;
} /* cmrpm_lte_sd_rpt_proc() */

/*===========================================================================

FUNCTION cmrpm_lte_rpt_proc

DESCRIPTION

  Process LL reports related to RPM


DEPENDENCIES
  RPM object must have already been initialized with cmrpm_lte_init().

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmrpm_lte_rpt_proc(

  const cm_hdr_type   *rpt_ptr
    /* Pointer to a LL report */
)
{
  cm_rpt_type             *cm_rpt_ptr = (cm_rpt_type *) rpt_ptr;
     /* Pointer to a LL report */
     
  cmrpm_lte_s_type        *rpm_ptr        = cmrpm_lte_ptr();     
     /* Point at RPM object */
     
  uint32           curr_system_time   = cm_util_get_curr_systime();
  uint8            i = 0;
  boolean          is_tac_already_present = FALSE;
    

  /* If no coin cell battery, add EFS time to current uptime to get total
  ** system time since power-up
  */
  if(rpm_ptr->is_coin_cell_support == FALSE )
  {
    curr_system_time = (rpm_ptr->sys_time_efs) + curr_system_time;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT(cm_rpt_ptr != NULL);
  CM_ASSERT(rpm_ptr != NULL);


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Process the report
  */
  switch( cm_rpt_ptr->hdr.cmd )
  {
    case CM_REG_REJECT_IND:
      if(rpm_ptr->is_rpm_enabled)
      {
        cm_reg_reject_ind_s_type *reg_reject_ind_ptr = 
          &cm_rpt_ptr->cmd.reg_reject_ind;
        
        CM_MSG_HIGH_3("reg_rej_ind: sys_time_efs: %d, coin_cell: %d, curr_system_time: %d",
            rpm_ptr->sys_time_efs, rpm_ptr->is_coin_cell_support, curr_system_time);
        CM_MSG_HIGH_3("reg_rej_ind: reject_domain: %d, reject_cause: %d, tac: %d",
            reg_reject_ind_ptr->reject_domain, reg_reject_ind_ptr->reject_cause, reg_reject_ind_ptr->tac);
          
        if( (reg_reject_ind_ptr->reject_domain == SYS_SRV_DOMAIN_CS_ONLY || 
          reg_reject_ind_ptr->reject_domain == SYS_SRV_DOMAIN_PS_ONLY || 
               reg_reject_ind_ptr->reject_domain == SYS_SRV_DOMAIN_CS_PS) &&
             (IS_EMM_GENERIC_CAUSE(reg_reject_ind_ptr->reject_cause)||
              IS_EMM_15_CAUSE(reg_reject_ind_ptr->reject_cause)) )
          {

          /* Start RPM event timer upon any one of the 3,6,7,8 or 15 EMM reject cause */
          if(rpm_ptr->event_timer == 0)
            {
            rpm_ptr->event_timer = CMRPM_SEC_PER_MIN * rpm_ptr->max_event_timer \
                                       + curr_system_time;

            CM_MSG_HIGH_1("reg_rej_ind: started event timer: %d",
              rpm_ptr->event_timer);            
            }
            else
            {
              CM_MSG_HIGH_2("reg_rej_ind: timer is already running: %d, curr_systime: %d",
              rpm_ptr->event_timer, curr_system_time);
            }

          /* Store the last EMM reject cause */
          rpm_ptr->last_emm_reject_cause = reg_reject_ind_ptr->reject_cause;
            

          switch (reg_reject_ind_ptr->reject_cause)
          {
            case ILLEGAL_MS:
            case ILLEGAL_ME:
            case GPRS_SERVICES_NOT_ALLOWED:
            case GPRS_SERVICES_AND_NON_GPRS_SERVICES_NOT_ALLOWED:
              /* For EMM reject cause 3, 6, 7 and 8, increment the emm_gen_counter */
              if( rpm_ptr->emm_gen_counter < rpm_ptr->max_event_counter )
              {
                rpm_ptr->emm_gen_counter++;
                CM_MSG_HIGH_2("reg_rej_ind: emm_gen_counter incremented to: %d, max_event_counter: %d",
                  rpm_ptr->emm_gen_counter, rpm_ptr->max_event_counter);
              }
              break;

            case NO_SUITABLE_CELLS_IN_LA:
            CM_ASSERT(IS_VALID_TAC(reg_reject_ind_ptr->tac));

              /* There can be EMM Reject from different TA from what was received earlier. 
               * Run a loop and check whether new tac is different from previous one.
               */

            /* Check for duplicates before adding the tac */
              for ( i = 0;i < CM_MAX_NUM_OF_TAC; i++)
            {
                if ( (IS_VALID_TAC(rpm_ptr->emm_15_reject[i].lte_tai.tac)) && \
                     (rpm_ptr->emm_15_reject[i].lte_tai.tac == reg_reject_ind_ptr->tac) && \
                     sys_plmn_match(rpm_ptr->emm_15_reject[i].lte_tai.plmn, reg_reject_ind_ptr->plmn))
              {
              /* tac is already in the list, no need to add again */
                is_tac_already_present = TRUE;
                  CM_MSG_HIGH_3("reg_rej_ind: TAC already present: tac = %d, emm_15_reject[%d].counter = %d",
                  reg_reject_ind_ptr->tac, i, rpm_ptr->emm_15_reject[i].counter);

                  if( rpm_ptr->emm_15_reject[i].counter < rpm_ptr->max_event_counter )
                  {
                    /* For EMM reject cause 15, increment the per-TAC emm_15_counter */
                    rpm_ptr->emm_15_reject[i].counter++;
                    CM_MSG_HIGH_3("reg_rej_ind: emm_15_reject[%d].counter incremented to: %d, max_event_counter: %d",
                      i, rpm_ptr->emm_15_reject[i].counter, rpm_ptr->max_event_counter);
                  }
              break;
              }
            } /* for () */

              /* If tac is not in the list, add to the list */
              if( is_tac_already_present == FALSE )
            {
              /* Check for available slot for adding the tac */
                for ( i = 0; i < CM_MAX_NUM_OF_TAC; i++)
                {
                  if(!IS_VALID_TAC(rpm_ptr->emm_15_reject[i].lte_tai.tac))
                  {
                    rpm_ptr->emm_15_reject[i].counter++;
                    rpm_ptr->emm_15_reject[i].lte_tai.tac = reg_reject_ind_ptr->tac;
                    rpm_ptr->emm_15_reject[i].lte_tai.plmn = reg_reject_ind_ptr->plmn;

                    CM_MSG_HIGH_3("reg_rej_ind: Added tac[%d] = %d, Counter = %d",
                                  i, rpm_ptr->emm_15_reject[i].lte_tai.tac,
                                  rpm_ptr->emm_15_reject[i].counter);
                    CM_MSG_HIGH_3("reg_rej_ind: Added plmn %d %d %d",
                                  rpm_ptr->emm_15_reject[i].lte_tai.plmn.identity[0],
                                  rpm_ptr->emm_15_reject[i].lte_tai.plmn.identity[1],
                                  rpm_ptr->emm_15_reject[i].lte_tai.plmn.identity[2]);
                  break;                  
                }
                else
                {
                    CM_MSG_HIGH_2("reg_rej_ind: existing tac[%d] = %d",
                      i, rpm_ptr->emm_15_reject[i].lte_tai.tac);
                  }
                }
            }
            else
            {
                CM_MSG_HIGH_3("reg_rej_ind: TAC counter emm_15_reject[%d].counter = %d not added, max_event_counter %d", 
                            i, rpm_ptr->emm_15_reject[i].counter, rpm_ptr->max_event_counter);
            }
              break;

            default:
              break;
            }

          if(cmrpm_lte_write_efs_rpm_info(rpm_ptr) == FALSE)
        {
            CM_ERR_0("reg_rej_ind: Can't write LTE RPM info into EFS");
          }
          
        }
      }
      break; 

    default:
      break;
  } // CM_REG_REJECT_IND
} /* cmrpm_lte_rpt_proc */

/*===========================================================================

FUNCTION cmrpm_lte_reset_efs_info

DESCRIPTION
  Reset the EFS info items, and write to EFS.
  
DEPENDENCIES
  RPM object must have already been initialized with cmrpm_lte_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmrpm_lte_reset_efs_info( void )
{
  cmrpm_lte_s_type        *rpm_ptr        = cmrpm_lte_ptr();
  uint8                    i              = 0;

  CM_MSG_HIGH_0("Reset_efs_info: Reset all LTE RPM EFS items");
  
  /* Reset EVENT_TIMER and last EMM reject cause to 0 */
  rpm_ptr->event_timer = 0;
  rpm_ptr->last_emm_reject_cause = 0;

  /* clear the TAC list and EMM reject counters */
  rpm_ptr->emm_gen_counter = 0;
  for(i=0; i< CM_MAX_NUM_OF_TAC; i++)
  {
    rpm_ptr->emm_15_reject[i].counter = 0;
    rpm_ptr->emm_15_reject[i].lte_tai.tac = INVALID_TAC;
    sys_plmn_undefine_plmn_id(&(rpm_ptr->emm_15_reject[i].lte_tai.plmn));
  }
    
  if(cmrpm_lte_write_efs_rpm_info(rpm_ptr) == FALSE)
  {
    CM_ERR_0("Reset EFS Info: fail to write RPM info ");
  }    
}

/*===========================================================================

FUNCTION  cmrpm_lte_send_update_ftai_list

DESCRIPTION
 The function sends the update FTAI List request to MM


DEPENDENCIES
 none

RETURN VALUE

SIDE EFFECTS
  none

===========================================================================*/
void cmrpm_lte_send_update_ftai_list(sys_rpm_ftai_list_action_e_type action)
{
#ifdef FEATURE_CM_LTE
  mm_cmd_type                   *mm_ptr = NULL;
  /* MM command pointer */

  mm_ptr = mm_cm_get_cmd_buf(MMCM_RPM_UPDATE_FTAI_LIST);

  if (mm_ptr == NULL)
  {
    CM_ERR_0( "MMCM_RPM_UPDATE_FTAI_LIST: out of MM cmd buffer" );
    return ;
  }

  mm_ptr->cmd.cm_mm_update_ftai_list.ftai_list_action = action;
  mm_ptr->cmd.cm_mm_update_ftai_list.as_id            = cmph_get_sub_with_lte_cap();

  mm_put_cmd( mm_ptr );

  CM_MSG_HIGH_1( "Send LTE RPM update_ftai_list action %d to MM", action );

#endif /* FEATURE_CM_LTE */
} /* cmph_send_trigger_ecall_inactivity */


