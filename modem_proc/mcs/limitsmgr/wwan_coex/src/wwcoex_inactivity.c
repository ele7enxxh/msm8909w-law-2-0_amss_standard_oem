
/*!
  @file
  wwcoex_inactivity.c

  @brief
  This file implements the tech inactivity management functions of LIMTSMGR COEX module.

  @ingroup per_implementation
*/

/*=============================================================================

  Copyright (c) 2013 Qualcomm Technologies Incorporated.
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

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/limitsmgr/wwan_coex/src/wwcoex_inactivity.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
05/19/14   rj      Handle 24 Bit USTMR for BOLT using VSTMR API
05/05/13   ag      Handle USTMR rollover
03/30/13   ag      Initial Revision

=============================================================================*/

/*=============================================================================

                           INCLUDE FILES

=============================================================================*/
#include "mcs_variation.h"
#include "wwcoex_inactivity.h"
#include "lmtsmgr_task.h"

#ifdef FEATURE_BOLT_MODEM
#include "vstmr.h"
#endif

#ifdef FEATURE_MODEM_COEXISTENCE_FW
#include "msmhwioreg.h"
#include "mcs_hwio_modem.h"
#include "timer.h"
#include "time_svc.h"
#include "modem_mem.h"

#ifdef FEATURE_CDMA
#include "srch4trans.h"
#endif

#include "DalDevice.h"
#include "DDITimetick.h"

/*=============================================================================

                            TYPEDEFS

=============================================================================*/


#ifdef FEATURE_BOLT_MODEM
/* USTMR size is changed to 24 Bit in Bolt Modem */
#define WWCOEX_MAX_USTMR_TIME  (0xFFFFFF)
#else
/* USTMR size is 27 Bit */
#define WWCOEX_MAX_USTMR_TIME  (0x7FFFFFF)
#endif

/* We should not see time tick difference go more than half the max ustmr ticks */
#define WWCOEX_CHECK_OVERRUN(tick1, tick2) (((tick1) - (tick2)) >= WWCOEX_MAX_USTMR_TIME/2)  

#define WWCOEX_APPLY_OVERRUN(expr) ((expr) & WWCOEX_MAX_USTMR_TIME)

/*! Expiry time in ms for the inactivity list. At this expiry, the timer will 
  fire callback and the inactivity list needs to be populated again.
  (NUM_INACTIVITY_SLOTS-1) * (GSM frame duration) = 14 * 4.615 = 64.61ms */
#define WWCOEX_NUM_TIMER_INACTIVITY_SLOTS  10

typedef struct 
{
  /*! Inactivity list of CXM_NUM_INACTVITY_SETS */
  cxm_inactivity_slot_list_s   *slot_list;

  /*! Activity information */
  wwcoex_activity_info_type    active_info;

  /*! Time at which tech registered */
  uint64                       reg_time;
  
  /*! Timer duration */
  uint64                       timer_duration;  

  /*! Inactivity expiry timer */
  timer_type                   exp_timer;

  /*! Mutex */
  rex_crit_sect_type           crit_sect;

  /*! Send Flag: TRUE - send to srch4, FALSE - dont send */
  boolean                      send_flag;

}wwcoex_inactivity_state_type;

/*=============================================================================

                         INTERNAL VARIABLES

=============================================================================*/
/*! Stores inactivity list of each tech */
wwcoex_inactivity_state_type  wwcoex_inactivity_state[CXM_TECH_MAX];

DalDeviceHandle*  lim_handle;       /* Handle used by timetick APIs */
/*=============================================================================

                                FUNCTIONS

=============================================================================*/
#ifdef FEATURE_CDMA
static void wwcoex_translate_inactivity_list_for_srch4
(
  cxm_inactivity_slot_list_s* list_ptr,
  srch4_cxm_inactivity_slot_type* inactivity_ptr
)
{
  uint32 counter = 0;
  inactivity_ptr->is_slotted = TRUE;
  inactivity_ptr->num_slots  = CXM_NUM_INACTVITY_SETS;

  for(counter = 0; counter < CXM_NUM_INACTVITY_SETS; ++counter)
  {
    inactivity_ptr->slots[counter].info_available = TRUE;
    inactivity_ptr->slots[counter].duration = list_ptr[counter].duration;
    inactivity_ptr->slots[counter].starting_time = list_ptr[counter].start_time;
  }
}
#endif

/*===========================================================================
FUNCTION wwcoex_alloc_list

DESCRIPTION
  Allocate the inactivity list

DEPENDENCIES 
  None

RETURN VALUE  
  None

SIDE EFFECTS
  None
  
===========================================================================*/
static cxm_inactivity_slot_list_s* wwcoex_alloc_list
(
  cxm_tech_type tech_id
)
{
  cxm_inactivity_slot_list_s* list_ptr;

  /* Create inactivity buffer for GSM1 */
  list_ptr=(cxm_inactivity_slot_list_s*)modem_mem_alloc(
                    CXM_NUM_INACTVITY_SETS * sizeof(cxm_inactivity_slot_list_s),
                                          MODEM_MEM_CLIENT_MCS );
  return list_ptr;
}

/*===========================================================================
FUNCTION wwcoex_get_current_ustmr

DESCRIPTION
  Get Current USTMR value

DEPENDENCIES 
  None

RETURN VALUE  
  current ustmr value

SIDE EFFECTS
  None
  
===========================================================================*/
static uint32 wwcoex_get_current_ustmr( void )
{
  uint32 current_ustmr;

  #ifdef FEATURE_BOLT_MODEM
    current_ustmr = VSTMR_XO_READ();
  #else
    current_ustmr = HWIO_IN(UNIV_STMR_MSTMR);
  #endif

  return current_ustmr;
}

/*===========================================================================
FUNCTION wwcoex_dealloc_list

DESCRIPTION
  Deallocate the inactivity list

DEPENDENCIES 
  None

RETURN VALUE  
  None

SIDE EFFECTS
  None
  
===========================================================================*/
static void wwcoex_dealloc_list
(
  cxm_tech_type tech_id
)
{
  /* Free the list ptr */
  if (wwcoex_inactivity_state[tech_id].slot_list != NULL)
  {
    modem_mem_free((void *)wwcoex_inactivity_state[tech_id].slot_list,
                    MODEM_MEM_CLIENT_MCS );
    wwcoex_inactivity_state[tech_id].slot_list = NULL;
  }

  return ;
}

/*===========================================================================
FUNCTION wwcoex_form_inactivity_list

DESCRIPTION
  Update the inactivity list

DEPENDENCIES 
  None

RETURN VALUE  
  None

SIDE EFFECTS
  None
  
===========================================================================*/
static void wwcoex_form_inactivity_list
(
  cxm_inactivity_slot_list_s* list_ptr,
  uint32 inactivity_start,
  uint32 total_period,
  uint32 inactivity_period
)
{
  int i;
  /* Form the new inactivity list */
  for (i=0; i<CXM_NUM_INACTVITY_SETS; i++)
  {
    /* inactivity start time = activity end time for the current period */
    list_ptr[i].start_time = WWCOEX_APPLY_OVERRUN(inactivity_start + i * total_period);

    /* Duration is fixed */
    list_ptr[i].duration = inactivity_period;
  }

  
  LMTSMGR_MSG_4(HIGH, "Inactivity list: start0 %d, start9 %d, start14 %d dur %d",
                list_ptr[0].start_time, 
                list_ptr[CXM_NUM_INACTVITY_SETS-6].start_time, 
                list_ptr[CXM_NUM_INACTVITY_SETS-1].start_time, inactivity_period ); 
}

/*===========================================================================
FUNCTION wwcoex_send_inactivity_to_srch4

DESCRIPTION
  Translates activity data to inactivity and conveys to srch4 if the
  send flag is set

DEPENDENCIES 
  None

RETURN VALUE  
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void wwcoex_send_inactivity_to_srch4
(
  cxm_inactivity_slot_list_s* list_ptr,
  boolean send_flag
)
{
  #ifdef FEATURE_CDMA
  srch4_cxm_inactivity_slot_type inactivity_data;
  uint32 current_ustmr;

  if (list_ptr == NULL)
  {
    return;
  }

  if (send_flag == FALSE)
  {
    LMTSMGR_MSG_0(MED, "Cannot send data to srch4. flag is 0");
    return;
  }
  
  /* Translate inactivity list to Srch4 type */
  wwcoex_translate_inactivity_list_for_srch4(list_ptr, &inactivity_data);

  /* Send Info to Srch4*/
  srch4_update_cxm_inactivity_table(&inactivity_data);

  /* Get current timer count */
  current_ustmr = wwcoex_get_current_ustmr();

  LMTSMGR_MSG_1(HIGH,"Sent GTX Info at ustmr time %d", current_ustmr);
  #endif
}

/*===========================================================================
FUNCTION wwcoex_send_inactivity_off

DESCRIPTION
  Sends inacitivty OFF indication to srch4

DEPENDENCIES 
  None

RETURN VALUE  
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void wwcoex_send_inactivity_off ( void )
{
  #ifdef FEATURE_CDMA
  srch4_cxm_inactivity_slot_type inactivity_data;

  /* Turn off the slotted mode. */
  inactivity_data.is_slotted = FALSE;
  inactivity_data.num_slots  = 0;

  /* Send Info to Srch4*/
  srch4_update_cxm_inactivity_table(&inactivity_data);
  #endif
}

/*===========================================================================
FUNCTION wwcoex_inactivity_timer_cb

DESCRIPTION
  This is the timer expiry callback for any tech's inactivity timer expiry
  Called in Timer IST context.

DEPENDENCIES 
  None

RETURN VALUE  
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void wwcoex_inactivity_timer_cb
(
  timer_cb_data_type            data
)
{
  cxm_tech_type  tech_id;
  cxm_inactivity_slot_list_s* list_ptr = NULL;
  uint32 current_ustmr, adv_multiple;
  uint32 list_start_time;
  

  tech_id = (cxm_tech_type)data;
  if (tech_id <= CXM_TECH_DFLT_INVLD || tech_id >= CXM_TECH_MAX)
  {
    LMTSMGR_MSG_1(ERROR,"Unexpected tech id %d received in timer cb", data);
    return;
  }

  /* Acquire mutex */
  rex_enter_crit_sect(&wwcoex_inactivity_state[tech_id].crit_sect);

  if (wwcoex_inactivity_state[tech_id].reg_time > 0)
  {
    /* Get the list ptr */
    list_ptr = wwcoex_inactivity_state[tech_id].slot_list; 

    /* Get current timer count */
    current_ustmr = wwcoex_get_current_ustmr();
    list_start_time = wwcoex_inactivity_state[tech_id].slot_list[WWCOEX_NUM_TIMER_INACTIVITY_SLOTS-1].start_time;

    /* Current time is to the right of start time... */
    if ( (current_ustmr > list_start_time) ||
         WWCOEX_CHECK_OVERRUN(list_start_time, current_ustmr) )
    { 
      /* Start time for the first entry in the list will depend on the timer count */
      adv_multiple = WWCOEX_APPLY_OVERRUN(current_ustmr - list_start_time) /
                         (wwcoex_inactivity_state[tech_id].active_info.period);


      /* Form the inactivity list*/
      wwcoex_form_inactivity_list( list_ptr, 
	                             WWCOEX_APPLY_OVERRUN(list_start_time + 
						  adv_multiple * wwcoex_inactivity_state[tech_id].active_info.period), 
	                             wwcoex_inactivity_state[tech_id].active_info.period,
	                             wwcoex_inactivity_state[tech_id].slot_list[WWCOEX_NUM_TIMER_INACTIVITY_SLOTS-1].duration );
    }
    else
    {
      /* Start time for the first entry in the list will depend on the timer count */
      adv_multiple = WWCOEX_APPLY_OVERRUN(list_start_time - current_ustmr) /
                         (wwcoex_inactivity_state[tech_id].active_info.period);


      /* Form the inactivity list*/
      wwcoex_form_inactivity_list( list_ptr, 
	                             WWCOEX_APPLY_OVERRUN(list_start_time - 
						  adv_multiple * wwcoex_inactivity_state[tech_id].active_info.period), 
	                             wwcoex_inactivity_state[tech_id].active_info.period,
	                             wwcoex_inactivity_state[tech_id].slot_list[WWCOEX_NUM_TIMER_INACTIVITY_SLOTS-1].duration );
    }

	
    wwcoex_send_inactivity_to_srch4(list_ptr, 
                                    wwcoex_inactivity_state[tech_id].send_flag);

    LMTSMGR_MSG_2(MED,"GTX Info at ustmr time %d adv_multiple %d", current_ustmr, adv_multiple);

    /* Reset the timer... */
    timer_set(&wwcoex_inactivity_state[tech_id].exp_timer,
	            wwcoex_inactivity_state[tech_id].timer_duration, 0, T_MSEC);
  }
  else
  {
    LMTSMGR_MSG_1(ERROR, "Timer cb for unregistered tech %d", tech_id);
  }

  /* Release mutex */
  rex_leave_crit_sect(&wwcoex_inactivity_state[tech_id].crit_sect);

  return;
}

/*===========================================================================
FUNCTION wwcoex_apply_correction

DESCRIPTION
  Checks if the current time is to the right of the end time and if any
  ovverrun correction needs to be applied

DEPENDENCIES 
  None

RETURN VALUE  
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void wwcoex_apply_correction
(
  wwcoex_activity_info_type *active_info
)
{
  uint32 current_ustmr, adv_time_multiple;

  /* Check if active_info is in the past...*/
  current_ustmr = wwcoex_get_current_ustmr();

  /* Is current ustmr to the right of start time... */
  if ( (current_ustmr > active_info->start_time)  ||
       WWCOEX_CHECK_OVERRUN(active_info->start_time, current_ustmr) )
  {
    /* Current time is to the right of start time... */
     
    if ( (current_ustmr > active_info->end_time) ||
         WWCOEX_CHECK_OVERRUN(active_info->end_time, current_ustmr) )
    { 
      /* Current time is to the right of end time... */

      adv_time_multiple = (WWCOEX_APPLY_OVERRUN(current_ustmr - active_info->end_time))/active_info->period;
      active_info->start_time = WWCOEX_APPLY_OVERRUN(active_info->start_time + ((adv_time_multiple+1) * active_info->period));
      active_info->end_time = WWCOEX_APPLY_OVERRUN( active_info->end_time + ((adv_time_multiple+1) * active_info->period));
     
      LMTSMGR_MSG_4(ERROR,"Reset the active time info to st %d, end %d, period %d, multiple %d",
                                   active_info->start_time, active_info->end_time,
                                   active_info->period, adv_time_multiple);  
    }
    else 
    {
      /* Current time is to the left of end time... no need to apply any correction */
    }
  }
  else
  {
     /* Current time is to the left of start time... so not need to apply any correction */
  }
}

#endif /* FEATURE_MODEM_COEXISTENCE_FW */

/*===========================================================================
FUNCTION wwcoex_init_inactivity_st

DESCRIPTION
  This API will init the wwcoex conflict tables

DEPENDENCIES 
  None

RETURN VALUE  
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void wwcoex_init_inactivity_st(void)
{
#ifdef FEATURE_MODEM_COEXISTENCE_FW
  uint32 i;
  uint32 result;

  /* Init all tables to null */
  for (i=0;i<CXM_TECH_MAX; i++)
  {
    wwcoex_inactivity_state[i].slot_list = NULL;

    rex_init_crit_sect(&wwcoex_inactivity_state[i].crit_sect);

    wwcoex_inactivity_state[i].reg_time = 0;

    /* Callback function for timer expiry */
    timer_def( &wwcoex_inactivity_state[i].exp_timer, NULL, rex_self(), 0,
                 (timer_t1_cb_type)( wwcoex_inactivity_timer_cb ),
                 (timer_cb_data_type)i );
  }

    /* Get handle for SystemTimer */
  result = DalTimetick_Attach( "SystemTimer", &lim_handle );
  if( ( result != DAL_SUCCESS ) || ( lim_handle == NULL ) )
  {
    ERR_FATAL( "Cannot attach to SystemTimer: result=%d, lim_handle=0x%X",
               result, lim_handle, 0 );
  }
  else
  {
    MSG(MSG_SSID_DFLT, MSG_LEGACY_HIGH, "Limits Manager Timetick handle initialized");
  }

#endif /* FEATURE_MODEM_COEXISTENCE_FW */
}

/*===========================================================================
FUNCTION wwcoex_register_activity

DESCRIPTION
  This API will register tech's activity

DEPENDENCIES 
  None

RETURN VALUE  
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void wwcoex_register_activity
(
  cxm_tech_type               tech_id,
  wwcoex_activity_info_type   active_info
)
{
#ifdef FEATURE_MODEM_COEXISTENCE_FW

   cxm_inactivity_slot_list_s* list_ptr = NULL;
   uint32 duration_ustmr;


   if (tech_id <= CXM_TECH_DFLT_INVLD || tech_id >= CXM_TECH_MAX)
   {
     return;
   }

   /* Acquire mutex */
   rex_enter_crit_sect(&wwcoex_inactivity_state[tech_id].crit_sect);

   /* If tech has already registered, just print a debug msg... */
   if (wwcoex_inactivity_state[tech_id].reg_time > 0)
   {
     LMTSMGR_MSG_1(MED, "Received new inactivity time for registered tech %d",
                   tech_id);     
   }
  
   if (wwcoex_inactivity_state[tech_id].slot_list == NULL)
   {
     /* Allocate a list and store it */
     list_ptr = wwcoex_alloc_list(tech_id);
     wwcoex_inactivity_state[tech_id].slot_list = list_ptr;
   }
   else
   {
     list_ptr = wwcoex_inactivity_state[tech_id].slot_list; 
   }

   if (list_ptr != NULL)
   {
     wwcoex_apply_correction(&active_info);

     /* Store the activity information */
     wwcoex_inactivity_state[tech_id].active_info = active_info;

     /* Form the inactivity list*/
     wwcoex_form_inactivity_list( list_ptr, 
	                          active_info.end_time, 
	                          active_info.period,
	                          WWCOEX_APPLY_OVERRUN(active_info.start_time +
                                                       active_info.period - 
                                                       active_info.end_time) );

     wwcoex_send_inactivity_to_srch4(list_ptr, wwcoex_inactivity_state[tech_id].send_flag);

     /* get current ustmr */
     wwcoex_inactivity_state[tech_id].reg_time = wwcoex_get_current_ustmr();
     /* Setup the timer for 10th inactivity */
     duration_ustmr = WWCOEX_APPLY_OVERRUN( WWCOEX_NUM_TIMER_INACTIVITY_SLOTS * 
                                                                            wwcoex_inactivity_state[tech_id].active_info.period); 
	 
     ( void ) DalTimetick_CvtFromTimetick64( lim_handle, duration_ustmr,
                                         	 T_MSEC, &wwcoex_inactivity_state[tech_id].timer_duration );

     /* Reset the timer... */
     timer_set(&wwcoex_inactivity_state[tech_id].exp_timer,
	            wwcoex_inactivity_state[tech_id].timer_duration, 0, T_MSEC);

   }
   else
   {
     LMTSMGR_MSG_1(ERROR, "Could not allocate inactivity list for tech %d", 
                   tech_id);
   }
   
   /* Release mutex */
   rex_leave_crit_sect(&wwcoex_inactivity_state[tech_id].crit_sect);

#endif /* FEATURE_MODEM_COEXISTENCE_FW */

   return ;
}

/*===========================================================================
FUNCTION wwcoex_deregister_activity

DESCRIPTION
  This API will deregister tech's activity

DEPENDENCIES 
  None

RETURN VALUE  
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void wwcoex_deregister_activity
(
  cxm_tech_type               tech_id
)
{
#ifdef FEATURE_MODEM_COEXISTENCE_FW

  if (tech_id <= CXM_TECH_DFLT_INVLD || tech_id >= CXM_TECH_MAX)
  {
    return;
  }

  /* Acquire mutex */
  rex_enter_crit_sect(&wwcoex_inactivity_state[tech_id].crit_sect);

  if ( wwcoex_inactivity_state[tech_id].reg_time > 0 )
  {
    /* Deallocate list */
    wwcoex_dealloc_list(tech_id);
    
    /* Clr the timer */
    (void)timer_clr(&wwcoex_inactivity_state[tech_id].exp_timer, T_NONE);

    wwcoex_inactivity_state[tech_id].reg_time = 0;

    wwcoex_send_inactivity_off();
  }
  else
  {
    LMTSMGR_MSG_1(MED, "Tech %d has not registered", tech_id);
  }

  /* Release mutex */
  rex_leave_crit_sect(&wwcoex_inactivity_state[tech_id].crit_sect);

#endif /* FEATURE_MODEM_COEXISTENCE_FW */

  return ;
}

/*===========================================================================
FUNCTION wwcoex_set_send_flag

DESCRIPTION
  This API will set or clear the send flag for a tech

DEPENDENCIES 
  None

RETURN VALUE  
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void wwcoex_set_send_flag
(
  cxm_tech_type         tech_id,
  boolean               send_flag 
)
{

  if (tech_id <= CXM_TECH_DFLT_INVLD || tech_id >= CXM_TECH_MAX)
  {
    return;
  }

#ifdef FEATURE_MODEM_COEXISTENCE_FW
  /* Acquire mutex */
  rex_enter_crit_sect(&wwcoex_inactivity_state[tech_id].crit_sect);

  /* Has the flag changed ? */
  if (wwcoex_inactivity_state[tech_id].send_flag != send_flag)
  {
    /* set the send flag */
    wwcoex_inactivity_state[tech_id].send_flag = send_flag;

    if ((wwcoex_inactivity_state[tech_id].reg_time > 0 ) &&
         (wwcoex_inactivity_state[tech_id].slot_list != NULL))
    {
      if (send_flag == TRUE)
      {
        LMTSMGR_MSG_1(HIGH, "Inactivity info sending for tech %d enabled...", 
                      tech_id);
        wwcoex_send_inactivity_to_srch4(wwcoex_inactivity_state[tech_id].slot_list,
                                        send_flag);
      }
      else
      {
        LMTSMGR_MSG_1(HIGH, "Inactivity info sending for tech %d disabled...", 
                      tech_id);
        wwcoex_send_inactivity_off();
      }
    }
  }

  /* Release mutex */
  rex_leave_crit_sect(&wwcoex_inactivity_state[tech_id].crit_sect);

#endif /* FEATURE_MODEM_COEXISTENCE_FW */
}
