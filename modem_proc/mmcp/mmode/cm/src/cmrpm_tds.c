/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

   CMRPM_TDS   M O D U L E

GENERAL DESCRIPTION
  This module contains the Call Manager handling the Reject message from NAS,
  which requires the SIM be reset.



INITIALIZATION AND SEQUENCING REQUIREMENTS
  cmrpm_tds_init() must be called to initialize this module before any other
  function declared in this module is being called.


Copyright (c) 2010 - 2011 by QUALCOMM Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/30/13   vm      Handle UE registering with CS/PS domain
11/30/12   hx      Initial version

===========================================================================*/



/**--------------------------------------------------------------------------
** Includes
** --------------------------------------------------------------------------
*/

#include "mmcp_variation.h"

#include "customer.h"

#ifdef FEATURE_MMODE_RPM_TDSCDMA
#include "cmrpm_tds.h"
#include "cmmmgsdi.h"
#include "cmtaski.h"
#include "cmdbg.h"     /* Interface to CM debug services */
#include "cmlog.h"     /* Interface for Diagnostic Logging */
#include "cmlog_v.h"
#include "time_svc.h"  /* Interface to clock services */
#include "cmutil.h"
#include "cmss.h"

#ifdef CM_DEBUG
#error code not present
#endif


/**--------------------------------------------------------------------------
** Forward declarations
** --------------------------------------------------------------------------
*/

/**--------------------------------------------------------------------------
** Globals 
** --------------------------------------------------------------------------
*/


/*flag for LPM request sent*/
static boolean cmrpm_tds_lpm_request_sent = FALSE;
/*flag for ONLINE reqeust sent*/
static boolean cmrpm_tds_online_request_sent = TRUE;
/*NV reset counter*/
static uint8 cmrpm_tds_efs_max_reset = 0;
/*flag for CRM_TDS triggered SIM RESET*/
static boolean cmrpm_tds_triggered_sim_insert = FALSE;
/*flag for true pwr on that init CM task*/
static boolean cmrpm_tds_power_cycle_state = TRUE;
/*6 to 24 hours cycle timer */
static uint32 cmrpm_tds_long_cycle_timer = 0;
/*reset counter for above cycle*/
static uint32 cmrpm_tds_long_cycle_counter = 0;
/**--------------------------------------------------------------------------
** Functions - internal
** --------------------------------------------------------------------------
*/



/*===========================================================================

FUNCTION cmrpm_tds_read_efs_sys_time

DESCRIPTION
 This function reads system time from EFS, that was written at last power-down 
 And updates local variable sys_time_efs with the time read from EFS.
 If time read from EFS is greater than uptime returned by API  
 cm_util_get_curr_systime(), we can consider coin cell battery as absent and
 use EFS itself for maintaining time across resets.

DEPENDENCIES
  RPM object must have already been initialized with cmrpm_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
boolean cmrpm_tds_read_efs_sys_time(void)
{
  cmrpm_tds_s_type                  *rpm_ptr        = cmrpm_tds_ptr();
  uint32                        sys_time_pwr_up ;
  int32                         size_of_timer   = sizeof(uint32);
  boolean                       status = TRUE;
 

  /* Read RPM SYS TIME from NV-EFS */
  cmnv_efs_read(CMNV_EFS_ID_RPM_SYS_TIME,
               (byte *)&sys_time_pwr_up,
                &size_of_timer);
  CM_MSG_HIGH_2("system time read at power-up is %d, curr_systime:%d",
                         sys_time_pwr_up, cm_util_get_curr_systime());

  /* If time read from EFS(time stored at last power-down) is greater than the 
  ** uptime(time since recent power-up), we can consider time is not persistent
  ** across resets and coin cell battery support is not there. 
  ** In this case, update time to local variable and set is_coin_cell_support 
  ** to FALSE
  */
  if(size_of_timer > 0 && sys_time_pwr_up > cm_util_get_curr_systime())
  {
    rpm_ptr->is_coin_cell_support = FALSE;
    rpm_ptr->sys_time_efs = (uint32)sys_time_pwr_up;
  }
  else if(size_of_timer < 0)
  {
    status = FALSE; 
  }
  CM_MSG_HIGH_2("rpm_ptr->sys_time_efs:%d coin_cell_support:%d",rpm_ptr->sys_time_efs,
   	                                                      rpm_ptr->is_coin_cell_support);
   
  return status;

}/* cmrpm_tds_read_efs_sys_time() */

/*===========================================================================

FUNCTION cmrpm_read_efs_info

DESCRIPTION
  This function reads RPM parameter from EFS

DEPENDENCIES
  RPM object must have already been initialized with cmrpm_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static boolean cmrpm_tds_read_efs_info(cmrpm_tds_efs_info_s_type *rpm_info_ptr)
{
  boolean                  status = TRUE;
  int32                    size_of_efs_info = sizeof(cmrpm_tds_efs_info_s_type);  

  /* Read RPM INFO from NV-EFS */
  cmnv_efs_read(CMNV_EFS_ID_TDS_RPM_INFO,
                (byte *)rpm_info_ptr,
                &size_of_efs_info);

  if(size_of_efs_info <= 0)
  {
    status = FALSE;
  }  

  return status;
}

/*If record the reset is not needed, please remove the following function*/
/*===========================================================================

FUNCTION cmrpm_tds_write_efs_info

DESCRIPTION
  This function writes RPM parameter into EFS to record the reset time

DEPENDENCIES
  Phone object must have already been initialized with cmph_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static boolean cmrpm_tds_write_efs_info(const cmrpm_tds_s_type   *rpm_ptr )
{
  boolean                            status = FALSE;
  cmrpm_tds_efs_info_s_type          rpm_efs_info;
  uint32                        curr_system_time   = cm_util_get_curr_systime();
  
  CM_MSG_MED_3("tds_write_efs_info: rpm_ptr->sys_time_efs:%d, coin_cell_support:%d, curr_systime:%d",
               rpm_ptr->sys_time_efs, rpm_ptr->is_coin_cell_support, curr_system_time);
  /* If no coin cell battery, add EFS time to current uptime to get total
  ** system time since power-up
  */
  if(rpm_ptr->is_coin_cell_support == FALSE )
  {
    curr_system_time = (rpm_ptr->sys_time_efs) + curr_system_time;
  }

  if(!cmrpm_tds_read_efs_info(&rpm_efs_info))
  {
    CM_ERR_0("Can't read RPM EFS INFO");
  }
  else
  {
    /* Update writable RPM parameter 
    ** Do not update other fixed parameter such as below
    ** is_rpm_enabled, app_reset_counter, average_reset_time
    */
    rpm_efs_info.last_reset_time = curr_system_time;
    rpm_efs_info.reset_counter = rpm_ptr->mmreject_reset.reset_counter;
    
    
    cmnv_efs_write(CMNV_EFS_ID_TDS_RPM_INFO,
                  (byte *)&rpm_efs_info,
                  sizeof(cmrpm_tds_efs_info_s_type));

    status = TRUE;

  }
  return status;

}


/*===========================================================================

FUNCTION cmrpm_tds_calc_timer_offset

DESCRIPTION
  This is the function for calculating the reset timer offset.  

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static uint32 cmrpm_tds_calc_timer_offset(void)
{
  cmrpm_tds_s_type    *rpm_ptr = cmrpm_tds_ptr();
  uint32 time_offset = rpm_ptr->mmreject_reset.reset_timer_offset;

  /*CMRPM_TDS_LA_RA_AGNOSTIC*/
  if(cmrpm_tds_long_cycle_counter)
  {
    /*Not the first time for the reset in current cycle, apply long timer*/
    /*using the linear algorithm at the beginning. We may add correlation with history*/
    time_offset = rpm_ptr->mmreject_reset.long_reset_timer * (cmrpm_tds_long_cycle_counter); /* * CMRPM_SEC_PER_MIN;*/
    
  }
  else
  {
    CM_MSG_HIGH_0("first reset in current cycle:");
    time_offset = rpm_ptr->mmreject_reset.short_reset_timer;
  }

  rpm_ptr->mmreject_reset.reset_timer_offset = time_offset;
  return time_offset;

}

/*===========================================================================

FUNCTION cmrpm_tds_gsdi_lpm_cb

DESCRIPTION
  This is callback function for processing the mmgsdi_card_pdown/pup.  

  This function should be called after the gsdi card status change.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cmrpm_tds_gsdi_lpm_cb(
    mmgsdi_return_enum_type       status,
    mmgsdi_cnf_enum_type          cnf,
    const mmgsdi_cnf_type         *cnf_ptr
)
{

  CM_ASSERT(cnf_ptr != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (status == MMGSDI_SUCCESS)
  {
    switch(cnf)
    {
      case MMGSDI_CARD_PUP_CNF:
        /*call the MODE ONLINE*/
        CM_MSG_HIGH_0("MMGSDI succesfull power-up SIM RESET finished, send SYS_OPRT_MODE_ONLINE");
          (void) cm_ph_cmd_oprt_mode(NULL, NULL, CM_CLIENT_ID_ANONYMOUS,
                                   SYS_OPRT_MODE_ONLINE);
        cmrpm_tds_online_request_sent = TRUE; /*Set it to true to avoid resend in failure case*/
        break;

      case MMGSDI_CARD_PDOWN_CNF:
        CM_MSG_HIGH_0(" TDS-RPM : MMGSDI succesfull power-down");
        break;
      case MMGSDI_REFRESH_CNF:
        switch (cnf_ptr->refresh_cnf.orig_refresh_req)
        {
          case MMGSDI_REFRESH_ORIG_REG_REQ:
            CM_MSG_HIGH_2("MMGSDI suceed reg CM refsh evnt.Status %d, Confirmation %d",
                   status, cnf);
            break;

          case MMGSDI_REFRESH_ORIG_OK_TO_REFRESH_REQ:
            CM_MSG_HIGH_2("Ok to refsh Ack succeed %d %d", cnf, status);
            break;

          default:
            CM_ERR_2("MMGSDI ret unexpected cnf.orig_refresh_req %d, Confirmation %d",
                   cnf_ptr->refresh_cnf.orig_refresh_req, cnf);
            break;
        }
        break;

      default:
        CM_ERR_2("MMGSDI ret unexpected cnf. Status %d, Confirmation %d",
               status, cnf);
        break;
    }
  }
  else if(status == MMGSDI_REFRESH_SUCCESS)
  {
    switch(cnf)
    {
      case MMGSDI_CARD_PUP_CNF:
        CM_MSG_HIGH_0("MMGSDI power-up REFRESH CNF");
        
        break;
      case MMGSDI_CARD_PDOWN_CNF:
        CM_MSG_HIGH_0("MMGSDI power-down REFRESH CNF");
        break;

      default:
        CM_ERR_2("MMGSDI ret unexpected cnf. Status %d, Confirmation %d",
               status, cnf);
        break;
    }

  }
  else /* status != MMGSDI_SUCCESS */
  {
    switch(cnf)
    {
      case MMGSDI_CARD_PUP_CNF:
        CM_ERR_0("MMGSDI power-up fail");
        if( cmrpm_tds_online_request_sent == FALSE)
        {
          /*If and only if it is the first callback failure from SIM, which is a command queue error*/
          CM_ERR_1("TDS-RPM: SIM FAILED PUP, CNF=%d, send SYS_OPRT_MODE_ONLINE anyway",cnf);
          (void) cm_ph_cmd_oprt_mode(NULL, NULL, CM_CLIENT_ID_ANONYMOUS,
                                   SYS_OPRT_MODE_ONLINE);
          cmrpm_tds_online_request_sent = TRUE;
        }
        
        break;
      case MMGSDI_CARD_PDOWN_CNF:
        CM_ERR_0("MMGSDI power-down fail, continue power up");
        break;
      case MMGSDI_REFRESH_CNF:
        switch (cnf_ptr->refresh_cnf.orig_refresh_req)
        {
          case MMGSDI_REFRESH_ORIG_REG_REQ:
            CM_MSG_HIGH_2("MMGSDI failed reg CM refsh evnt.Status %d, Confirmation %d",
                   status, cnf);
            break;
    
          case MMGSDI_REFRESH_ORIG_OK_TO_REFRESH_REQ:
            CM_MSG_HIGH_2("Ok to refresh Ack fail %d %d", cnf, status);
            break;
    
          default:
            CM_ERR_2("MMGSDI ret unexpected cnf. Status %d, Confirmation %d",
                   status, cnf);
            break;
        }
        break;

      default:
        CM_ERR_2("MMGSDI ret unexpected cnf. Status %d, Confirmation %d",
               status, cnf);
        break;
    }
  }  /* status != MMGSDI_SUCCESS */
}  /* cmrpm_tds_gsdi_lpm_cb() */


/*===========================================================================

FUNCTION cmrpm_tds_reset_sim

DESCRIPTION
  Reset the SIM card with power down and power up.

  This function must be called after UE goes to LPM mode.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/

static void cmrpm_tds_reset_sim (void)
{
      CM_MSG_HIGH_0("TDS-RPM : RPM UE is in LPM. LPM done with no error, reset USIM now");
      /*reuse the CM's client_id*/
      if(mmgsdi_card_pdown(cm_mmgsdi_get_client_id(),
                           MMGSDI_SLOT_1,
                           cmrpm_tds_gsdi_lpm_cb,
                           MMGSDI_CARD_POWER_DOWN_NOTIFY_GSDI,
                           0 ) != MMGSDI_SUCCESS
        )
      {
        CM_ERR_0("Unable to power-down card");
      }
      cmrpm_tds_triggered_sim_insert = TRUE;
      cmrpm_tds_online_request_sent = FALSE; /*Only set when power down SIM, where the UE is in LPM already*/
      /* Send card power-up cmd to MMGSDI.
      */
      if(mmgsdi_card_pup(cm_mmgsdi_get_client_id(),
                         MMGSDI_SLOT_1,
                         cmrpm_tds_gsdi_lpm_cb,
                         MMGSDI_CARD_POWER_UP_INITIAL_PUP,
                         0 ) != MMGSDI_SUCCESS
        )
      {
        CM_ERR_0("Unable to power-up card");
      }
  
}




/**--------------------------------------------------------------------------
** Functions - external
** --------------------------------------------------------------------------
*/  
       
/*===========================================================================

FUNCTION cmrpm_tds_ptr

DESCRIPTION
  Return a pointer to the one and only RPM object.


DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
cmrpm_tds_s_type *cmrpm_tds_ptr( void )
{
  static cmrpm_tds_s_type    cmrpm_local;       /* The one and only rpm object */

  return &cmrpm_local;

} /* *cmrpm_tds_ptr() */



/*===========================================================================

FUNCTION cmrpm_tds_init

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
boolean cmrpm_tds_init( void )
{

  cmrpm_tds_s_type                  *rpm_ptr        = cmrpm_tds_ptr();
  /*To resue NV CMNV_EFS_ID_RPM_INFO forTDS*/
  cmrpm_tds_efs_info_s_type         rpm_efs_info;

   cmrpm_tds_online_request_sent = TRUE; /*Init is as TRUE since the UE is ONLINE mode at the beginning*/

  /* Initialize all RPM parameters in ph_ptr */
  cmrpm_tds_efs_max_reset = 1; /*NV counters in case NV is not read*/
  rpm_ptr->is_rpm_enabled = FALSE;  /*For TDS use, not read from SIM*/
  rpm_ptr->mmreject_reset.max_reset = 1; /*For TDS use, not read from SIM*/
  rpm_ptr->mmreject_reset.short_reset_timer = 0; /*Init timer to 0*/
  rpm_ptr->mmreject_reset.long_reset_timer = 0; /*Init timer to 0 */
  rpm_ptr->is_coin_cell_support = TRUE;
  rpm_ptr->sys_time_efs = 0;

  /* Read EFS configured system time */
  cmrpm_tds_read_efs_sys_time();

  /*Read the RPM NV for TDS case*/
  if(cmrpm_tds_read_efs_info(&rpm_efs_info))
  {
    if(rpm_efs_info.is_rpm_enabled == TRUE && *(cmph_get_gprs_anite_gcf_ptr()) == FALSE)
    {
      rpm_ptr->is_rpm_enabled = TRUE;
      rpm_ptr->mmreject_reset.short_reset_timer = rpm_efs_info.short_reset_timer;
      rpm_ptr->mmreject_reset.long_reset_timer = rpm_efs_info.long_reset_timer;
      cmrpm_tds_efs_max_reset = (rpm_efs_info.max_num_reset)>0?rpm_efs_info.max_num_reset:1;

    }
    else
    {
      rpm_ptr->is_rpm_enabled = FALSE;

    }
  }
  else
  { /*NV is not readable*/
    CM_MSG_HIGH_0("RPM TDS: CMNV_EFS_ID_RPM_INFO does not exist");
    rpm_ptr->is_rpm_enabled = FALSE;
  }

  /*rpm_ptr->mmreject_reset.counter = 0;*/ /*Cannot be init, need it keep the counter*/
  if(cmrpm_tds_power_cycle_state == TRUE)
  {
    uint32  curr_system_time   = cm_util_get_curr_systime();
    CM_MSG_MED_3("tds_init: rpm_ptr->sys_time_efs:%d, coin_cell_support:%d, curr_systime:%d",
                 rpm_ptr->sys_time_efs, rpm_ptr->is_coin_cell_support, curr_system_time);
    /* If no coin cell battery, add EFS time to current uptime to get total
    ** system time since power-up
    */
    if(rpm_ptr->is_coin_cell_support == FALSE )
    {
      curr_system_time = (rpm_ptr->sys_time_efs) + curr_system_time;
    }
    cmrpm_tds_power_cycle_state = FALSE;
    rpm_ptr->mmreject_reset.timer = 0;               /*reset the timer*/
    rpm_ptr->mmreject_reset.reset_counter = 0;       /*reset the counter only for power cycle*/
    rpm_ptr->mmreject_reset.reset_timer_offset = 0;  /*reset the offset only for power cycle and 6~24 hour timer */
    CM_MSG_HIGH_2("RPM TDS: reset cmrpm_tds_power_cycle_state = %d, reset_counter = %d",
                  cmrpm_tds_power_cycle_state, rpm_ptr->mmreject_reset.reset_counter);

    /*set the 6~24 hours cycle timer*/
    
    if(rpm_ptr->is_rpm_enabled)
    {
      cmrpm_tds_long_cycle_counter = 0; /*reset the long cycle timer*/
      cmrpm_tds_long_cycle_timer = curr_system_time + 6 * CMRPM_SEC_PER_HOUR;  /* CMRPM_SEC_PER_HOUR*/
      CM_MSG_HIGH_2("RPM TDS: cycle timer started: current time: %d, 6to24_hour_timer: %d", curr_system_time, cmrpm_tds_long_cycle_timer ); 
    }
    else
    {
      cmrpm_tds_long_cycle_timer = 0;
      cmrpm_tds_long_cycle_counter = 0; /*reset the long cycle timer*/
    }
    
  }
  else
  {
    CM_MSG_HIGH_1("RPM TDS: DO NOT reset mmreject_reset.counter = %d", rpm_ptr->mmreject_reset.reset_counter);
  }

  CM_MSG_HIGH_4("RPM params loaded: rpm_ptr->is_rpm_enabled %d | mmreject_reset: short timer %d, long timer %d, NV reset counter %d",
                rpm_ptr->is_rpm_enabled,
    rpm_ptr->mmreject_reset.short_reset_timer,
    rpm_ptr->mmreject_reset.long_reset_timer,
    cmrpm_tds_efs_max_reset);
 
  return rpm_ptr->is_rpm_enabled;
} /* cmrpm_tds_init */



/*===========================================================================

FUNCTION cmrpm_tds_timer_proc

DESCRIPTION
  Process timer for RPM.

DEPENDENCIES
  RPM object must have already been initialized with cmrpm_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmrpm_tds_timer_proc(
  cm_timer_event_type    timer_event   /* Indicate specific timer event */
)
{
  cmrpm_tds_s_type                  *rpm_ptr        = cmrpm_tds_ptr();
  uint32                        curr_sys_time  = cm_util_get_curr_systime();

  /* If no coin cell battery, add EFS time to current uptime to get total
  ** system time since power-up
  */
  if(rpm_ptr->is_coin_cell_support == FALSE )
  {
    curr_sys_time = (rpm_ptr->sys_time_efs) + curr_sys_time;
  }

  SYS_ARG_NOT_USED( timer_event );

 
  /* Check if MM/GMM reject reset timer is expired 
  ** This check should be at the last since we should process all other timer 
  ** prior to reset the modem.
  */

  if((rpm_ptr->mmreject_reset.timer !=0) && 
     (curr_sys_time >= rpm_ptr->mmreject_reset.timer))
  {
    CM_MSG_HIGH_2("MMReject Reset Timer expired - curr_systime:%d, timer:%d",
      curr_sys_time, rpm_ptr->mmreject_reset.timer);
        
    /* Check if there is any on-going call, if then, we need to delay 
    ** the reset until the ME is back to IDLE 
    */    
    if (cmcall_overall_call_state() == CM_OVERALL_CALL_STATE_IDLE)
    {
      /* stop the timer */
      rpm_ptr->mmreject_reset.timer = 0;
      

      /* reset modem */
      CM_MSG_HIGH_0(" TDS-RPM: sending LPM and check the LPM mode in cmrpm_tds_timer_proc later");

      /*set the callback function*/
      (void) cm_ph_cmd_oprt_mode(NULL, NULL, CM_CLIENT_ID_ANONYMOUS,
                                 SYS_OPRT_MODE_LPM);
      
       cmrpm_tds_lpm_request_sent = TRUE;
      
    }
    else
    {
      CM_MSG_HIGH_0("MM/GMM Reject reset pending - call in progress");
    }
    
  }


  /*check the UE is in LPM mode*/
  if(cmrpm_tds_lpm_request_sent)
  {
    if(cm_ph_get_oprt_mode() == SYS_OPRT_MODE_LPM) 
    {
      cmrpm_tds_lpm_request_sent = FALSE;
      CM_MSG_HIGH_0("TDS RPM: UE is in LPM mode, RESET SIM now");
      cmrpm_tds_reset_sim();
      
    }
    else
    {
      CM_MSG_HIGH_1("TDS RPM: phone mode is: %d", cm_ph_get_oprt_mode());
    }
  }


  /*process long cycle timer timeout*/

  if((cmrpm_tds_long_cycle_timer != 0)
      && (curr_sys_time >= cmrpm_tds_long_cycle_timer))
  {
    /*start next cycle*/
    cmrpm_tds_long_cycle_counter = 0; /*reset the long cycle timer*/
    cmrpm_tds_long_cycle_timer = curr_sys_time + 6 * CMRPM_SEC_PER_HOUR;  /* * CMRPM_SEC_PER_HOUR*/
    CM_MSG_HIGH_2("RPM TDS: long cycle timer started after timeout: current time: %d, 6to24_hour_timer: %d", curr_sys_time, cmrpm_tds_long_cycle_timer );

    rpm_ptr->mmreject_reset.reset_timer_offset = 0;  /*reset the offset only for power cycle and 6~24 hour timer */

  }
    
}


/*===========================================================================

FUNCTION cmrpm_tds_sim_evt_proc

DESCRIPTION

  Process SIM EVENT related to RPM


DEPENDENCIES
  RPM object must have already been initialized with cmrpm_init().

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmrpm_tds_sim_evt_proc(cmrpm_tds_sim_event_type evt)
{
  cmrpm_tds_s_type                  *rpm_ptr        = cmrpm_tds_ptr();
  /* Point at RPM object */
  cmss_s_type *ss_ptr = cmss_ptr();
  uint32                        curr_system_time   = cm_util_get_curr_systime();

  CM_MSG_MED_4("tds_sim_evt:rpm_ptr->sys_time_efs:%d coin_cell_support:%d curr_systime:%d is_rpm_enabled = %d",
               rpm_ptr->sys_time_efs, rpm_ptr->is_coin_cell_support,
               curr_system_time, rpm_ptr->is_rpm_enabled);
  /* If no coin cell battery, add EFS time to current uptime to get total
  ** system time since power-up
  */
  if(rpm_ptr->is_coin_cell_support == FALSE )
  {
    curr_system_time = (rpm_ptr->sys_time_efs) + curr_system_time;
  }

  CM_ASSERT(rpm_ptr != NULL);
  CM_ASSERT (ss_ptr != NULL);

  if(rpm_ptr->is_rpm_enabled)
  {
    switch(evt)
    {
       case CMRPM_SIM_ILLEGAL_SUBSCRIPTION:
          CM_MSG_HIGH_0("SIM ILLEGAL_SUBSCRIPTION Received");
          if((ss_ptr->info.sys_mode != SYS_SYS_MODE_TDS) && (ss_ptr->info.sys_mode != SYS_SYS_MODE_GSM))
          {
            CM_MSG_HIGH_1("SIM event is not for TDSCDMA or GSM: current RAT %d",
                 ss_ptr->info.sys_mode);
            return; 
          }

          if((rpm_ptr->mmreject_reset.timer == 0)&&(rpm_ptr->mmreject_reset.reset_counter < cmrpm_tds_efs_max_reset))
          {

            rpm_ptr->mmreject_reset.timer = curr_system_time + cmrpm_tds_calc_timer_offset();
            /*check the relation between long cycle timer and reset timer*/
            if((rpm_ptr->mmreject_reset.timer > cmrpm_tds_long_cycle_timer)&&(cmrpm_tds_long_cycle_timer != 0))
            {
               /*make the reset timer equals the long cycle timer*/
               rpm_ptr->mmreject_reset.timer = cmrpm_tds_long_cycle_timer;
            }

            rpm_ptr->mmreject_reset.reset_counter++;
            cmrpm_tds_long_cycle_counter++;

            CM_MSG_HIGH_5("Reset timer running %d, curr_systime %d, reset since pwr on %d, Long cycle counter %d, timer %d",
                          rpm_ptr->mmreject_reset.timer, curr_system_time,
                          rpm_ptr->mmreject_reset.reset_counter,
                          cmrpm_tds_long_cycle_counter,
                          cmrpm_tds_long_cycle_timer);
          }
          else
          {
            CM_MSG_HIGH_3("NOT Automatically RESET SIM: Reset Counter: %d, NV reset counter%d, T1: %d", 
                 rpm_ptr->mmreject_reset.reset_counter, cmrpm_tds_efs_max_reset, rpm_ptr->mmreject_reset.timer);
          }
          break;
       case CMRPM_SIM_SUBSCRIPTION_READY:
         CM_MSG_HIGH_0("SIM CMRPM_SIM_SUBSCRIPTION_READY received: write EFS with timing info and reset number");
         /* MODE ONLINE called already*/
         /*If record the reset is not needed, please remove the following call*/
         cmrpm_tds_write_efs_info(rpm_ptr);

         break;
         case CMRPM_SIM_CARD_INSERTED_EVT:
         if(cmrpm_tds_triggered_sim_insert)
         {
           CM_MSG_HIGH_1("SIM CMRPM_SIM_CARD_INSERTED_EVT Received, NOT RESET counters: %d", rpm_ptr->mmreject_reset.reset_counter);
           cmrpm_tds_triggered_sim_insert = FALSE;
         }
         else
         {
           rpm_ptr->mmreject_reset.reset_counter = 0;
           CM_MSG_HIGH_1("None RPM TDS triggered SIM_CARD_INSERTED, reset mmreject_reset.counter: %d", rpm_ptr->mmreject_reset.reset_counter);
         }

         break;

       default:
         CM_MSG_HIGH_1("Sim event NOT processed: event = %d", evt);
         break;
    }
  }
}

/*===========================================================================

FUNCTION cmrpm_tds_process_call_end

DESCRIPTION
  This function process CALL_END when RPM Is running
  If there is mm reject reset timer(T1) is running, we need to reset the modem

DEPENDENCIES
  RPM object must have already been initialized with cmrpm_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmrpm_tds_process_call_end(void)
{
  cmrpm_tds_s_type                  *rpm_ptr        = cmrpm_tds_ptr();
  uint32                        curr_sys_time  = cm_util_get_curr_systime();

  CM_MSG_MED_3("tds_proc_call_end: rpm_ptr->sys_time_efs:%d, coin_cell_support:%d, curr_systime:%d",
               rpm_ptr->sys_time_efs, rpm_ptr->is_coin_cell_support, curr_sys_time);
  /* If no coin cell battery, add EFS time to current uptime to get total
  ** system time since power-up
  */
  if(rpm_ptr->is_coin_cell_support == FALSE )
  {
    curr_sys_time = (rpm_ptr->sys_time_efs) + curr_sys_time;
  }

  if(rpm_ptr->is_rpm_enabled)
  {    
    
    /* Check if MM/GMM reject reset timer(T1) is expired */ 
    if((rpm_ptr->mmreject_reset.timer != 0 )&&
      (curr_sys_time >= rpm_ptr->mmreject_reset.timer))
    {
      CM_MSG_HIGH_0("TDS-RPM: Call ended and reset timer expired, reset the phone, sending LPM, check LPM mode in cmrpm_tds_timer_proc later");
      
      /* stop the timer */
      rpm_ptr->mmreject_reset.timer = 0; /*Can save time is the reset timer is large*/

      /* reset modem */
      /*set the callback function*/
      (void) cm_ph_cmd_oprt_mode(NULL, NULL, CM_CLIENT_ID_ANONYMOUS,
                                 SYS_OPRT_MODE_LPM);
      
       cmrpm_tds_lpm_request_sent = TRUE;
  
      
    }
  }  
}

/*===========================================================================

FUNCTION cmrpm_tds_sd_rpt_proc

DESCRIPTION
  Process reports from System Determination.

DEPENDENCIES
  RPM object must have already been initialized with cmrpm_tds_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmrpm_tds_sd_rpt_proc(

  const cm_hdr_type   *rpt_ptr
    /* Pointer to SD report */
)
{

  /*lint -e{826} */
  const cm_sd_rpt_u_type         *sd_rpt_ptr = (cm_sd_rpt_u_type *) rpt_ptr;
    /* Pointer to SD reports */

  cmrpm_tds_s_type                  *rpm_ptr        = cmrpm_tds_ptr();
  uint32                        curr_system_time   = cm_util_get_curr_systime();

  /* If no coin cell battery, add EFS time to current uptime to get total
  ** system time since power-up
  */
  if(rpm_ptr->is_coin_cell_support == FALSE )
  {
    curr_system_time = (rpm_ptr->sys_time_efs) + curr_system_time;
  }
    /* Point at rpm  object */


 
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  CM_ASSERT( rpm_ptr != NULL );
  CM_ASSERT( sd_rpt_ptr != NULL );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* This is serving system change notifiction
  ** 1. Updating appropriate domain selection object fields.
  */
  switch( sd_rpt_ptr->hdr.cmd )
  {
    /* Service Indicators information is changed.
    */
    case CM_SRV_IND_INFO_F:
      if(rpm_ptr->is_rpm_enabled)
      { 
        /* Reset timer 1hr timer when UE is registered in both CS and PS domain */
        if((sd_rpt_ptr->srv_ind_info.si_info.srv_status == SYS_SRV_STATUS_SRV) && 
           ((sd_rpt_ptr->srv_ind_info.si_info.srv_domain == SYS_SRV_DOMAIN_CS_ONLY) ||
             (sd_rpt_ptr->srv_ind_info.si_info.srv_domain == SYS_SRV_DOMAIN_PS_ONLY) ||
             (sd_rpt_ptr->srv_ind_info.si_info.srv_domain == SYS_SRV_DOMAIN_CS_PS)))
        {
          
          /*start next cycle*/
          cmrpm_tds_long_cycle_counter = 0; /*reset the long cycle timer*/
          cmrpm_tds_long_cycle_timer = curr_system_time + 6 * CMRPM_SEC_PER_HOUR;  /* * CMRPM_SEC_PER_HOUR*/
          CM_MSG_HIGH_2("RPM TDS: long cycle timer started: current time: %d, 6to24_hour_timer: %d", curr_system_time, cmrpm_tds_long_cycle_timer);
      
          rpm_ptr->mmreject_reset.reset_timer_offset = 0;  /*reset the offset only for power cycle and 6~24 hour timer */
          
  
          CM_MSG_HIGH_0("CS/PS registered : Stop Long reset timer. Start next cycle");
        }
      }
      break;
         
          
    default:
      break;
  } /* switch( sd_rpt_ptr->hdr.cmd ) */

  return;
} /* cmsds_sd_rpt_proc() */


#else /* FEATURE_MMODE_RPM_TDSCDMA */

/*===========================================================================
FUNCTION cmrpm_dummy

DESCRIPTION

  Dummy function, defined to get rid of
  'Warning: C3040E: no external declaration in translation unit'

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmrpm_tds_dummy( void )
/*lint -esym(714,cmrpm_dummy) */
/*lint -esym(765,cmrpm_dummy)
** Can't be static as it would result in compiler warnings
*/
{
}

#endif

