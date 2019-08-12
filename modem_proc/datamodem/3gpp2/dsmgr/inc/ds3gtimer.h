#ifndef DS3GTIMER_H
#define DS3GTIMER_H
/*===========================================================================

               3 G   D A T A   S E R V I C E S   M A N A G E R

                        T I M E R   F U N C T I O N S

                            H E A D E R    F I L E

DESCRIPTION
  This file contains wrapper functions for REX timer services. These functions
  may be used by software components within the Data Services Task.

  In order to add a new timer, the timer id must be defined below, the timer
  must be initialized in ds3gi_timer_init(), and logic to handle the timer
  expiration must be added to ds3gi_process_timer_expired_cmd().


  Copyright (c) 2001 - 2012 by Qualcomm Technologies Incorporated. All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds3gtimer.h_v   1.0   08 Aug 2002 11:19:50   akhare  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/dsmgr/inc/ds3gtimer.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/08/12   ssb     Dun Call Entitlement support using PS_SYS Interface
01/02/12   msh     Coan: Feature cleanup
10/17/11   ssh     eHRPD Always On support
06/06/11   ash     Added support for PPP Partial Context
22/10/09   ca      Fixed Lint Errors.
03/09/09   sa      AU level CMI modifications.
01/10/07   sq      Added DS3G_TIMER_RETRY_DELAY_ORIG
07/26/06   ar      Add ds3gi_is_timer_running().
04/05/05   gr      Integrated EPZID functionality into main line
03/01/05   kvd     Added timers for RDUD module.
11/30/04   kvd     Added timers for CAM module.
03/22/04   gr      Added Abort Dormant PPP timer
10/01/03   ak      Added PZID delay timer.
09/02/03   sy      Added pzid hysteresis timer.
07/13/02   atp     Added 707 async timers.
03/05/02   smp     Added definition for DCD wink timer.
07/10/01   smp     Initial version.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "customer.h"

#include "dstask_v.h"
#include "rex.h"
#include "mcs_timer.h"
#include "ds3gsubsmgr.h"


/*-------------------------------------------------------------------------
  Global variable for DS3G Critical sections
-------------------------------------------------------------------------*/
extern rex_crit_sect_type ds3g_timer_crit_sect;
/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
typedef enum
{
  DS3G_TIMER_TYPE_INVALID     =   -1,
  DS3G_TIMER_TYPE_REX_TIMER   =    0,
  DS3G_TIMER_TYPE_MCS_TIMER   =    1,
  DS3G_TIMER_TYPE_MAX_TYPE
}ds3g_timer_type_e;

/*---------------------------------------------------------------------------
  List of timer ids across Data.  No enum's as the actual timers are
  defined in the technology-specific modules, and then registered into an
  array that is kept centrally.
---------------------------------------------------------------------------*/
typedef enum
{
  DS3G_TIMER_MIN = -1,       /* minimum value - used for bounds checking   */

  /*-------------------------------------------------------------------------
     GENERIC TIMERS
  -------------------------------------------------------------------------*/
  DS3G_TIMER_RING_TE_1,        /* timer used to ring the laptop              */
  DS3G_TIMER_WINK_DCD_1,       /* timer used to wink DCD                     */

  DS3G_TIMER_RING_TE_2,        /* timer used to ring the laptop              */
  DS3G_TIMER_WINK_DCD_2,       /* timer used to wink DCD                     */

  /*-------------------------------------------------------------------------
     IS707 TIMERS
  -------------------------------------------------------------------------*/
  DS3G_TIMER_CTA,
  DS3G_TIMER_HOLDDOWN,
  DS3G_TIMER_EPZID_HYSTERESIS,
  DS3G_TIMER_EPZID_DELAY,
  DS3G_TIMER_RETRY_DELAY_ORIG,
  DS3G_TIMER_PKT_AUTO_REDIAL,
  /*-------------------------------------------------------------------------
     IS707 ASYNC TIMERS
  -------------------------------------------------------------------------*/
  DS3G_TIMER_ASYNC_PTCL_OPENING,

  /*-------------------------------------------------------------------------
     DORMANT PPP ABORT TIMER
  -------------------------------------------------------------------------*/
  DS3G_TIMER_ABORT_DORMANT_PPP,

  /*-------------------------------------------------------------------------
    Timers for CAM module-CAM timer for CAM to kick-in and flow ctrl timer
  -------------------------------------------------------------------------*/
  DS3G_TIMER_CAM,
  DS3G_TIMER_CAM_FLOW_CTRL,

  /*-------------------------------------------------------------------------
    Timers for RDUD module-RDUD timer started to revert to default CTA timeou
    after certian time period.
  -------------------------------------------------------------------------*/
  DS3G_TIMER_RDUD,

  /*-------------------------------------------------------------------------
    eHRPD partial context timers
  -------------------------------------------------------------------------*/
  DS3G_TIMER_MPIT,
  DS3G_TIMER_PCM,
  
  /*-------------------------------------------------------------------------
    eHRPD Always On Link status determination timer
  -------------------------------------------------------------------------*/
  DS3G_TIMER_LSD,

  /*-------------------------------------------------------------------------
    DUN Control Entitlement Response Waiting Timer
  -------------------------------------------------------------------------*/
  DS3G_TIMER_DUN_CTRL_ENTL,
  /*------------------------------------------------------------------------- 
    Timers to generate Throughput Information per subscription
    -------------------------------------------------------------------------*/
  DS3G_TIMER_REPORT_THROUGHPUT_INFO_IND_MIN,

  DS3G_TIMER_REPORT_THROUGHPUT_INFO_IND_SUBS_ID_1 
                                 = DS3G_TIMER_REPORT_THROUGHPUT_INFO_IND_MIN, 

  DS3G_TIMER_REPORT_THROUGHPUT_INFO_IND_SUBS_ID_2,

#if defined(FEATURE_TRIPLE_SIM)
  DS3G_TIMER_REPORT_THROUGHPUT_INFO_IND_SUBS_ID_3,
#endif

  DS3G_TIMER_REPORT_THROUGHPUT_INFO_IND_MAX,

 /*-------------------------------------------------------------------------- 
   Timers to refresh actual throughput Information per subscription
   --------------------------------------------------------------------------*/
  DS3G_TIMER_REFRESH_ACTUAL_THROUGHPUT_MIN 
                                = DS3G_TIMER_REPORT_THROUGHPUT_INFO_IND_MAX,
  DS3G_TIMER_REFRESH_ACTUAL_THROUGHPUT_SUBS_ID_1 
                                = DS3G_TIMER_REFRESH_ACTUAL_THROUGHPUT_MIN,
  DS3G_TIMER_REFRESH_ACTUAL_THROUGHPUT_SUBS_ID_2,
#if defined(FEATURE_TRIPLE_SIM)
  DS3G_TIMER_REFRESH_ACTUAL_THROUGHPUT_SUBS_ID_3,
#endif
  DS3G_TIMER_REFRESH_ACTUAL_THROUGHPUT_MAX,

 /*-------------------------------------------------------------------------- 
   Timers to downlink throughput estimation Information per subscription
   --------------------------------------------------------------------------*/
  DS3G_TIMER_DOWNLINK_THROUGHPUT_ESTIMATION_MIN,
  DS3G_TIMER_DOWNLINK_THROUGHPUT_ESTIMATION_SUBS_ID_1 
                                = DS3G_TIMER_DOWNLINK_THROUGHPUT_ESTIMATION_MIN,
  DS3G_TIMER_DOWNLINK_THROUGHPUT_ESTIMATION_SUBS_ID_2,
#if defined(FEATURE_TRIPLE_SIM)
  DS3G_TIMER_DOWNLINK_THROUGHPUT_ESTIMATION_SUBS_ID_3,
#endif
  DS3G_TIMER_DOWNLINK_THROUGHPUT_ESTIMATION_MAX,
  DS3G_TIMER_MAX             /* maximum value - used for bounds checking   */

} ds3g_timer_enum_type;

/*---------------------------------------------------------------------------
  Struct for DS_CMD_TIMER_EXPIRED
---------------------------------------------------------------------------*/
typedef struct
{
  unsigned long  timer_id;       /* Timer id of the timer that expired */
  void*          user_data;
} ds3g_timer_expired_cmd_type;

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION      DS3GI_TIMER_INIT

DESCRIPTION   This function initializes the timer services. This function must 
              be called once, at Data Services Task startup.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void  ds3gi_timer_init( void );

/*===========================================================================
FUNCTION      DS3GI_TIMER_REGISTER

DESCRIPTION   Clients register their timer info with timer services. 
              The passed in ID must come from the ds3g_timer_enum_type.Once
              registered, the timers cannot be de-registered (though they can
              be stopped/started).
 
              Registers the timer with REX (or) MCS.  Timer is not started
              during registration

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void  ds3gi_timer_register
(
  void                  *timer_ptr,
  void                  *timer_cb_f_ptr,
  ds3g_timer_enum_type   timer_id,
  ds3g_timer_type_e      timer_type
);

/*===========================================================================
FUNCTION      DS3GI_START_TIMER

DESCRIPTION   This function starts the specified timer for the specified 
               duration.

DEPENDENCIES  A valid registered timer id must be specified, otherwise the 
              timer is not started.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void  ds3gi_start_timer
(
  ds3g_timer_enum_type  timer_id,                   /* Timer to be started */
  dword                 duration         /* Timer duration in milliseconds */
);

/*===========================================================================
FUNCTION      DS3GI_STOP_TIMER

DESCRIPTION   This function stops the specified timer.

DEPENDENCIES  A valid registered timer id must be specified, otherwise no action is taken.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void  ds3gi_stop_timer
(
  ds3g_timer_enum_type  timer_id                    /* Timer to be stopped */
);

/*===========================================================================
FUNCTION      DS3GI_GET_ACTUAL_THROUGHPUT_REFRESH_INTERVAL

DESCRIPTION   For the given subscription ID, this function returns the time 
              interval(in msec), the actual throughput rate gets refreshed
              for every bearer/RLP depending on the current sys mode and
              the number of active calls 

DEPENDENCIES  The throughput Info event timer should be running to refresh 
              actual throughput for all bearers/RLP's  

RETURN VALUE  uint32 - Refresh Interval in (msec)
 
              0       - If the timer is not running (or) if the subscription
                        ID is invalid
 
SIDE EFFECTS  None
===========================================================================*/
uint32 ds3gi_get_actual_throughput_refresh_interval
(
  ds3gsubsmgr_subs_id_e_type ds3g_subs_id
);

/*===========================================================================
FUNCTION      DS3GI_GET_ESTIMATED_DL_THROUGHPUT_INTERVAL

DESCRIPTION   For the given subscription ID, this function returns the 
              multiplier for t_acumulate that is used to calculate t_report

DEPENDENCIES  

RETURN VALUE  uint32 - T_accumulate multiplier 
 

 
SIDE EFFECTS  None
===========================================================================*/
uint32 ds3gi_get_estimated_dl_throughput_interval
(
  ds3gsubsmgr_subs_id_e_type ds3g_subs_id
);

/*===========================================================================
FUNCTION      DS3GI_IS_TIMER_RUNNING

DESCRIPTION   This function reports status of specified timer.

DEPENDENCIES  A valid timer id must be specified, otherwise no action is taken.

RETURN VALUE  TRUE if timer is active, FALSE otherwise

SIDE EFFECTS  None
===========================================================================*/
boolean ds3gi_is_timer_running
(
  ds3g_timer_enum_type  timer_id                      /* Timer of interest */
);

/*===========================================================================
FUNCTION      DS3GI_TIMER_PROCESS_INIT_COMPLETE_CMD

DESCRIPTION   This function registers for PS sys events and timer objects

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3gi_timer_process_init_complete_cmd( void );

/*===========================================================================
FUNCTION      DS3GI_PROCESS_TIMER_EXPIRED_CMD

DESCRIPTION   This function processes the DS_TIMER_EXPIRED_CMD. It determines 
              which timer has expired, and calls the appropriate function to
              handle the timer expiry.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void  ds3gi_process_timer_expired_cmd
(
  ds_cmd_type  *cmd_ptr                              /* Pointer to command */
);

/*===========================================================================
FUNCTION      DS3GI_CONFIGURE_TIMER_PERIODICITY

DESCRIPTION   This utility can be used to set (or) unset the periodicity of 
              the timer. (one time (or) recurring timer runs)

DEPENDENCIES  Currently used for MCS timer types only

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3gi_configure_timer_periodicity
( 
  ds3g_timer_enum_type  timer_id,
  boolean               is_periodic_tmr
);

/*===========================================================================
FUNCTION      DS3GI_IS_TIMER_PERIODIC

DESCRIPTION   This function returns if the input timer is periodic or not
 
DEPENDENCIES  None

RETURN VALUE  TRUE  - If the timer is periodic 
              FALSE - IF the timer is not periodic

SIDE EFFECTS  None
===========================================================================*/
boolean ds3gi_is_timer_periodic
(
  ds3g_timer_enum_type      timer_id
);
#endif /* DS3GTIMER_H */
