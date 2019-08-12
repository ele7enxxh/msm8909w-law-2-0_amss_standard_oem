#ifndef CMRPM_H
#define CMRPM_H
/*===========================================================================

C A L L   M A N A G E R   RADIO POLICY MANAGER  H E A D E R   F I L E

DESCRIPTION
  This header file contains all the definitions necessary to interface
  with CMRPM.C

Copyright (c) 2010 - 2011 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/04/13   jvo     Mainline FEATURE_MMODE_RPM, add cmrpm_is_rpm_enabled
11/15/11   jh      Integrated RPM feature

===========================================================================*/

/**--------------------------------------------------------------------------
** Includes
** --------------------------------------------------------------------------
*/

#include "comdef.h"    /* Definition for basic types and macros */
#include "cm.h"        /* External interface to cm.c */
#include "cm_v.h"
#include "cmi.h"       /* Internal interface to cm.c */
#include "sys.h"       /* System wide definitions */
#include "sys_v.h"


/**--------------------------------------------------------------------------
** Defines
** --------------------------------------------------------------------------
*/
#define CMRPM_SEC_PER_MIN   60    /* seconds in min */
#define CMRPM_SEC_PER_HOUR  3600  /* seconds in hour */
#define CMRPM_MIN_PER_T1    6     /* Indicates how many min increment for T1  
                                  * ex> if T1 is 10, total is 6*10=60min 
                                  */
#define CMRPM_VERSION   0x02      /* Indicates RPM Version supported */


#define CMMMGSDI_RPM_ENABLED_FLAG_SIZE 1 /* EF-RPM Enabled Flag */
#define CMMMGSDI_RPM_PARAMETERS_SIZE  32 /* EF-RPM Parameters */
#define CMMMGSDI_RPM_OMCLR_SIZE        6 /* EF-Operational Management Counters Leak Rate */
#define CMMMGSDI_RPM_OMC_SIZE         32 /* EF-Operational Management Counters */
#define CMMMGSDI_RPM_VERSION_SIZE      1 /* EF-RPM Version Implemented */
#define CMMMGSDI_RPM_AD_SIZE           4 /* EF-AD */

/**--------------------------------------------------------------------------
** Datatypes
** --------------------------------------------------------------------------
*/

/* This holds RPM parameter information stored in CM_EFS_RPM_INFO
 * Need to declare as PACKED since this structure is also stored in
 * CM-NV 
 */
typedef PACKED struct PACKED_POST{
  boolean                          is_rpm_enabled;
  /* Indicate whether RPM functionality should be enabled or not */
  
  uint8       app_max_num_reset;
  /* Max # of APP-initiated resets allowed per hour 
  ** Stored in N1 in EF-RPM parameters
  */
  
  uint8       app_reset_counter;    
  /* # of APP-initiated reset per hour */
  
  uint32      app_reset_timer;
  /* 1 hour Timer for tracking APP initiated reset. 
  ** Will store expired time in secs for reset 
  */

  uint32      app_reset_block_leak_rate_timer;  
  /* Timer for decrementing C-BR-1
  ** Expired time in secs for decrementing C-BR-1
  */

  uint8       average_reset_time;
  /* Average time before RPM resets modem following permanent MM/GMM reject */
  
  uint32      leak_rate_timer;
  /* Timer for decrementing C-R-1. 
  ** Expired time in secs for decrementing C-R-1
  */
  
}cmrpm_efs_info_s_type;



typedef struct cmrpm_mmreject_reset_s_type {
  uint8       average_reset_time;
  /* Average time before RPM resets modem following permanent MM/GMM reject */
  
  uint8       counter; 
  /* # of RPM-initiated reset due to MM/GMM reject 
  ** Stored in C-R-1 in EF-RPM Operational Management Counters 
  */
  
  uint8      leak_rate_hour;
  /* leak late for decrementing C-R-1 
  ** Stored in LR-2 in EF-RPM Operational Management Counters Leak Rate
  */
  
  uint32     timer; 
  /* Timer for tracking RPM reset. Will store expired time in secs for reset */
  
  uint32     leak_rate_timer;
  /* Timer for decrementing C-R-1. 
  ** Expired time in secs for decrementing C-R-1
  */
  
}cmrpm_mmreject_reset_s_type;


typedef struct cmrpm_app_reset_s_type {
  uint8       max_num_reset;
  /* Max # of APP-initiated resets allowed per hour 
  ** Stored in N1 in EF-RPM parameters
  */
  
  uint8       counter;    
  /* # of APP-initiated reset per hour */
  
  uint8       block_counter;    
  /* # of Blocked APP-initiated reset after counter reaches max_reset */
  
  uint8       block_leak_rate_hour;
  /* leak rate for decrementing C-BR-1
  ** Stored in LR-1 in EF-RPM Operational Management Counters Leak Rate
  */
  
  uint32     timer;
  /* 1 hour Timer for tracking APP initiated reset. 
  ** Will store expired time in secs for reset 
  */
  
  uint32     block_leak_rate_timer;  
  /* Timer for decrementing C-BR-1
  ** Expired time in secs for decrementing C-BR-1
  */
  
}cmrpm_app_reset_s_type;



typedef struct cmrpm_s_type {
  boolean                          is_rpm_enabled;
  /* Indicate whether RPM functionality should be enabled or not */
  
  cmrpm_mmreject_reset_s_type   mmreject_reset;
  /* MM/GMM reject initiated reset info */
  
  cmrpm_app_reset_s_type        app_reset;  
  /* Application initiated reset info */
  
  uint32     sys_time_efs; 
  /* Current system time value read from EFS at power-up*/

  boolean   is_coin_cell_support;
  /* Indicate whether UE have coin cell battery support */
  
}cmrpm_s_type;

/* CMRPM Supported mcc/mnc structure stored in NV-EFS */
typedef struct cmrpm_supported_mcc_mnc_s_type{
   uint8  mcc[MAX_MCC_DIGITS];
   uint8  mnc[MAX_MNC_DIGITS];
}cmrpm_supported_mcc_mnc_s_type;

#define CMRPM_MAX_SIM_SUPPORTED   10 /* Max RPM SIM supported in NV */

/**--------------------------------------------------------------------------
** Functions
** --------------------------------------------------------------------------
*/



/*===========================================================================

FUNCTION cmrpm_write_efs_sys_time

DESCRIPTION
   This function writes system time at power-down in EFS

DEPENDENCIES
  RPM object must have already been initialized with cmrpm_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern boolean cmrpm_write_efs_sys_time(

  sys_oprt_mode_e_type  prev_oprt_mode,

  sys_oprt_mode_e_type  curr_oprt_mode
);


/*===========================================================================

FUNCTION cmrpm_read_efs_sys_time

DESCRIPTION
 This function reads system time from EFS, that was written at last power-down 
 And updates local variable sys_time_efs with the time read from EFS.
 If time read from EFS is greater than uptime returned by API  
 cm_util_get_curr_systime(), we can consider coin cell battery as absent and
 use EFS itself for maintaining time across resets.S

DEPENDENCIES
  RPM object must have already been initialized with cmrpm_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern boolean cmrpm_read_efs_sys_time( boolean );



/*===========================================================================

FUNCTION cmrpm_is_rpm_enabled

DESCRIPTION
  Returns whether rpm is enabled, based on the rpm pointer.
  Meant for use by external functions that don't have much to do with RPM
  but need to know if it is enabled.

DEPENDENCIES
  cmrpm_init should be called before this function.

RETURN VALUE
  TRUE if the rpm pointer indicates that rpm is enabled
  FALSE otherwise

SIDE EFFECTS
  none

===========================================================================*/
boolean cmrpm_is_rpm_enabled( void );



/*===========================================================================

FUNCTION cmrpm_ptr

DESCRIPTION
  Return a pointer to the one and only RPM object.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern cmrpm_s_type *cmrpm_ptr( void );


/*===========================================================================

FUNCTION cmrpm_init

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
extern boolean cmrpm_init( void );

/*===========================================================================

FUNCTION cmrpm_app_reset_rejected_proc

DESCRIPTION
  Processes PH cmd from CM when app reset was rejected 

DEPENDENCIES
  RPM object must have already been initialized with cmrpm_init().

RETURN VALUE
None

SIDE EFFECTS
  None
===========================================================================*/

extern void cmrpm_app_reset_rejected_proc( void );

/*===========================================================================

FUNCTION cmrpm_check_reset_allowed

DESCRIPTION
  Check whether reset is allowed or not when RPM is on

DEPENDENCIES
  RPM object must have already been initialized with cmrpm_init().

RETURN VALUE
  CM_PH_CMD_ERR_NONE if no parameter related errors,
  otherwise specific cm_call_cmd_err_e_type.

SIDE EFFECTS
  None
===========================================================================*/
extern boolean  cmrpm_check_reset_allowed(
  void
);

/*===========================================================================

FUNCTION cmrpm_process_app_reset

DESCRIPTION
  Process oprt mode change

DEPENDENCIES
  RPM object must have already been initialized with cmrpm_init().

RETURN VALUE
  boolean

SIDE EFFECTS
  None
===========================================================================*/
extern boolean  cmrpm_process_app_reset(
  cm_ph_cmd_s_type           *ph_cmd_ptr
);


/*===========================================================================

FUNCTION cmrpm_timer_proc

DESCRIPTION
  Process timer for RPM.

DEPENDENCIES
  RPM object must have already been initialized with cmrpm_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cmrpm_timer_proc(
  cm_timer_event_type    timer_event   /* Indicate specific timer event */
);

/*===========================================================================

  FUNCTION cmrpm_sd_rpt_proc

  DESCRIPTION
    Process reports from System Determination.

  DEPENDENCIES
    RPM object must have already been initialized with cmrpm_init().

  RETURN VALUE
    none

  SIDE EFFECTS
    none

===========================================================================*/
extern void cmrpm_sd_rpt_proc(
  const cm_hdr_type   *rpt_ptr
    /* Pointer to SD report */
);


/*===========================================================================

FUNCTION cmrpm_rpt_proc

DESCRIPTION
  Process LL reports related to RPM

DEPENDENCIES
  RPM object must have already been initialized with cmrpm_init().

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void cmrpm_rpt_proc(
  const cm_hdr_type   *rpt_ptr
    /* Pointer to a LL report */
);


/*===========================================================================

FUNCTION cmrpm_process_call_end

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
extern void cmrpm_process_call_end(void);

/*===========================================================================

FUNCTION cmph_rpm_process_gsdi_refresh

DESCRIPTION
  This function process GSDI REFRESH_EVT when one of RPM files are updated
  This function will be called from CMMMGSDI.

DEPENDENCIES
  RPM object must have already been initialized with cmrpm_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cmrpm_process_gsdi_refresh(
  const mmgsdi_refresh_file_list_type    *refresh_file_list
);

/*===========================================================================

FUNCTION cmrpm_oprt_mode_proc

DESCRIPTION
  This function reports oprt mode change to LPM

DEPENDENCIES
  RPM object must have already been initialized with cmrpm_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/

extern void cmrpm_oprt_mode_proc(void);


#endif /* CM_RPM_H */

