#ifndef CMRPM_TDS_H
#define CMRPM_TDS_H
/*===========================================================================

C A L L   M A N A G E R   RADIO POLICY MANAGER  H E A D E R   F I L E

DESCRIPTION
  This header file contains all the definitions necessary to interface
  with CMRPM.C

Copyright (c) 2010 - 2011 by QUALCOMM Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/30/12   hx      Initial version for TDS support

===========================================================================*/

/**--------------------------------------------------------------------------
** Includes
** --------------------------------------------------------------------------
*/

#include "cmrpm.h"     /*The original */




/**--------------------------------------------------------------------------
** Datatypes
** --------------------------------------------------------------------------
*/


typedef enum
{
    CMRPM_SIM_ILLEGAL_SUBSCRIPTION,
    CMRPM_SIM_SUBSCRIPTION_READY,
    CMRPM_SIM_CARD_INSERTED_EVT,
    CMRPM_TDS_SIM_EVENT_MAX    
}cmrpm_tds_sim_event_type;


typedef PACKED struct PACKED_POST{
  boolean is_rpm_enabled;      /*NV controls if SIM reset is supported or not*/
  uint8 short_reset_timer;      /*Value of short timer after Illegal SIM for the first reset*/
  uint8 long_reset_timer;       /*Value of the long timer after Illegal SIM, while the LA/RA not changes*/
  uint8 max_num_reset;         /*Limit of the SIM RESET before pwr cycle*/
  uint8 reset_counter;         /*This is optional to count the reset number since last pwr cycle*/
  uint32 tdsrpm_opt_list;     /*optional list for SIM reset*/
  uint32 last_reset_time;    /*This is optional to record the latest reset timestamp*/
} cmrpm_tds_efs_info_s_type;


typedef struct cmrpm_tds_mmreject_reset_s_type {
  uint32       short_reset_timer;
  /* The short timer before RPM resets sim following permanent Illegal SIM event */
  
  uint32      long_reset_timer;
  /* The long timer before RPM resets sim following permanent Illegal SIM event,
     if the RA/LA has no change*/
 
  uint32    timer;
  /*the timer for SIM RESET*/

  uint8      max_reset;
  /* Limit of the SIM RESET before pwr cycle*/

  uint8      reset_counter; 
  /* Stores number of reset since last pwr cycle*/
  
  uint32     last_reset_time_stamp; 
  /* Time stamp for last reset */

  uint32    reset_timer_offset;
  /*reset timer offset for correlation of history if needed*/
  
}cmrpm_tds_mmreject_reset_s_type;

typedef struct cmrpm_tds_s_type {
  boolean                          is_rpm_enabled;
  /* Indicate whether RPM functionality should be enabled or not */
  
  cmrpm_tds_mmreject_reset_s_type   mmreject_reset;
  
  
  uint32     sys_time_efs; 
  /* Current system time value read from EFS at power-up*/

  boolean   is_coin_cell_support;
  /* Indicate whether UE have coin cell battery support */
  
}cmrpm_tds_s_type;


/**--------------------------------------------------------------------------
** Functions
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
cmrpm_tds_s_type *cmrpm_tds_ptr( void );


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
boolean cmrpm_tds_init( void );

/*===========================================================================

FUNCTION cmrpm_tds_check_reset_allowed

DESCRIPTION
  Check whether reset is allowed or not when RPM is on

DEPENDENCIES
  RPM object must have already been initialized with cmrpm_tds_init().

RETURN VALUE
  CM_PH_CMD_ERR_NONE if no parameter related errors,
  otherwise specific cm_call_cmd_err_e_type.

SIDE EFFECTS
  None
===========================================================================*/
cm_ph_cmd_err_e_type  cmrpm_tds_check_reset_allowed(
  cm_ph_cmd_s_type           *ph_cmd_ptr
);

/*===========================================================================

FUNCTION cmrpm_tds_timer_proc

DESCRIPTION
  Process timer for RPM.

DEPENDENCIES
  RPM object must have already been initialized with cmrpm_tds_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmrpm_tds_timer_proc(
  cm_timer_event_type    timer_event   /* Indicate specific timer event */
);




/*===========================================================================

FUNCTION cmrpm_tds_rpt_proc

DESCRIPTION
  Process LL reports related to RPM

DEPENDENCIES
  RPM object must have already been initialized with cmrpm_tds_init().

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmrpm_tds_rpt_proc(
  const cm_hdr_type   *rpt_ptr
    /* Pointer to a LL report */
);


/*===========================================================================

FUNCTION cmrpm_tds_process_call_end

DESCRIPTION
  This function process CALL_END when RPM Is running
  If there is mm reject reset timer(T1) is running, we need to reset the modem

DEPENDENCIES
  RPM object must have already been initialized with cmrpm_tds_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmrpm_tds_process_call_end(void);

/*===========================================================================

FUNCTION cmrpm_tds_sim_evt_proc

DESCRIPTION

  Process SIM EVENT related to RPM


DEPENDENCIES
  RPM object must have already been initialized with cmrpm_tds_init().

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmrpm_tds_sim_evt_proc(cmrpm_tds_sim_event_type evt);

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
extern void cmrpm_tds_sd_rpt_proc(
  const cm_hdr_type   *rpt_ptr
    /* Pointer to SD report */
);

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
extern boolean cmrpm_tds_read_efs_sys_time(void);

#endif /* CM_RPM_H */

