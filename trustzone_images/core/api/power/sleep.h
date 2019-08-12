#ifndef SLEEP_H
#define SLEEP_H
/*===========================================================================

              S L E E P    T A S K   H E A D E R    F I L E

DESCRIPTION
  This file contains declarations for use by the Sleep Task.
  This module is used in the portable only.

      Copyright (c) 1992-2010 by Qualcomm Technologies, Inc.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/power/sleep.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/22/10   tau     Made SLEEP_RESOURCE_RBC1 a unique resource.
12/11/09   cab     Remove compile check on FEATURE_NEW_SLEEP_API, for CMI
06/18/09   taw     Temporarily add sleep_uart_clock_rgm back in until deps
                   are resolved.
06/05/09   taw     Remove all featurization and deprecated interfaces.
04/20/09   ajf     Updated with extended sleep voting functions & defines.
02/18/09   taw     Removed SLEEP_CHGT_OKTS_SIG and SLEEP_DIAG_OKTS_SIG.
01/23/09   taw     Removed the inclusion of sleep_v.h.
01/23/09   taw     Remove SDCC_DRV_OKTS_SIG, SLEEP_KEEPER_OKTS_SIG, 
                   SLEEP_MC_INIT_OKTS_SIG, SLEEP_GPSONE_OKTS_SIG, 
                   SLEEP_LSM_OKTS_SIG, SLEEP_UIM_OKTS_SIG, 
                   SLEEP_GRAPHICS_OKTS_SIG, and SLEEP_MP4_OKTS_SIG, 
                   as they are no longer used; also moved sleep_task and
                   sleep_init to this file.
12/16/08   taw     Remove stuff that's not public outside of CORE, and move it
                   to sleep.h.
12/02/08   clm     Switch back to including clkregim.h instead of clkrgm_msm.h.
11/04/08   jbh     Convert pointer in WinMob sleep_register to guarantee a
                   global address instead of a slot 0 address.
09/16/08   clm     1H08 add support for QSC6270.
08/22/08   cab     Added charger featurization
06/27/08   clm     1H08 sleep re-work.
03/31/08   taw     Added prototype for sleep_get_tcxo_shutdown_count().
03/20/08   jfl     Added support for VDD min restrictions
                   and reverted SLEEP_UIM_OKTS_SIG removal
03/13/08   jfl     Removed SLEEP_UIM_OKTS_SIG
03/13/08   jfl     Removed SLEEP_HS_OKTS_SIG
12/17/07    th     Update to WinMob sleep IOCTLs to add 
                   sleep_set_swfi_restriction                   
12/10/07   cab     Removed SLEEP_USB_OKTS_SIG.
11/27/07   cab     Removed SLEEP_HDR_OKTS_SIG.
10/04/07   taw     Added function sleep_init().
09/07/07   cab     Fixed compilation problem on single proc targets
08/15/07   ebb     Specified values for #define APPS_SLEEP and MODEM_SLEEP
07/20/07   taw     Removed the sleep_hs_usb_in_use global.
07/06/07   cab     Removed SLEEP_CAMERA_OKTS_SIG
06/12/07   ebb     Removed SLEEP_VOC_OKTS_SIG, SLEEP_SND_OKTS_SIG, 
                   SLEEP_VS_OKTS_SIG
05/22/07   cab     Added API sleep_set_swfi_restriction.
05/22/07   cab     Added sleep_hs_usb_in_use to check before going into SWFI.
05/03/07   cab     Removed SLEEP_RXC_OKTS_SIG
05/02/07   cab     Removed SLEEP_SRCH_OKTS_SIG
04/16/07   cab     Mainlined CLK_REGIME_UP_SLEEP__MCLK_HCLK
03/29/07   cab     Removed SLEEP_HDRDEC_OKTS_SIG
03/29/07    th     TCXO vote deregistration for WinMob builds
02/21/07   cab     Removed reference to 0x80000000 REX signal
11/27/06   cab     Removed SLEEP_UI_OKTS_SIG
11/09/06   ddh     Removed SLEEP_MDDI_OKTS_SIG and SLEEP_MDI_OKTS_SIG
11/09/06    th     Remove USB vote from mARM for FEATURE_USB_ON_APPS_PROC
11/01/06   cab     Removed forced definition of FEATURE_NEW_SLEEP_API
10/30/06   jhs     Removed SLEEP_KYPD_OKTS_SIG.
10/13/06   ddh     Added error if FEATURE_NEW_SLEEP_API is NOT defined. 
                   Removed SLEEP_BT_OKTS_SIG.
10/09/06   ddh     Removed use of SLEEP_BT_OKTS_SIG when
                   FEATURE_NEW_SLEEP_API is defined. Also removed DEM and APPS
                   OKTS signals.
10/04/06    th     Implementation of WinMob Sleep IOCTLs
09/05/06   taw     Moved USB vote to be available on both apps and modem.
08/16/06   cab     Removed rex_set_sigs reference from WINCE builds
08/11/06   cab     7200 integration updates 
08/03/06   cab     Remove UI/HS/KEYPAD sleep voting for thinUI builds
                   Add mclk and hclk restrictions to new sleep interface
07/28/06   jwob    Set SLEEP_LSM_OKTS_SIG to 0 if not defined
07/20/06   jwob    Stub SLEEP_(ASSERT|NEGATE)_OKTS macros when not needed
06/28/06   TMR     Added FEATURE_NO_USB_HW_ON_PROCESSOR
06/14/06   ddh     Moved modem specific functions to sleepmod.c
06/12/06   ddh     Moved SLEEP_SND_OKTS_SIG to be checked for both modem and
                   apps for dual proc builds
05/10/06   cab     First implementation of "new" voting interface functions
05/03/06   taw     Removed the DMOV vote for sleep.
04/21/06   taw     Define the battery charger vote signal if 
                   FEATURE_BATTERY_CHARGER is defined.  MSM7500 coupling
                   will be removed in the near future.
04/21/06   taw     Added votes from MDP and MDDI on the modem if the secondary
                   display is controlled by the modem during apps power 
                   collapse.  Also, added a vote for DMOV on the apps.
04/17/06   ejv     Added SLEEP_MC_INIT_OKTS_SIG for T_QSC60X0.
04/03/06   ptm     Reduced header coupling for 7500.
03/29/06   dy      Added GSM/GPRS support for multi-mode build.
03/14/06   taw     Moved the UIM vote to be on the modem side only.
02/13/06   cab     Added SDCC_DRV_OKTS_SIG for the SDCC/SDIO driver.
02/08/06   taw     Removed SLEEP_APPS_PC_OKTS_SIG, will just use 
                   SLEEP_APPS_OKTS_SIG
01/10/06   ddh     Added SLEEP_DEM_OKTS_SIG to hold off sleep from the
                   Apps side until all subsystems are up and running.
11/11/05   cab     Set SLEEP_GRAPHICS_OKTS_SIG to be 0x00000400
07/13/05   ddh     Added defining of APPS_SLEEP for none dual proc chips
06/22/05   ddh     Cleaned up chip specific sleep defines
05/19/05   ajn     Removed unused DS & DOG OKTS signals.
05/16/05   ajn     Moved signals to recreate required TASK sigs hole.
05/02/05   ajn     Added SLEEP_GRP_OKTS_SIG
04/06/05   ddh     Set SLEEP_GRAPHICS_OKTS_SIG to be 0
03/21/05   ddh     Added modem and apps support 

===========================================================================*/
/*===========================================================================

                        INCLUDE FILES

===========================================================================*/
#ifdef WINSIM
  #include "sleep_offline.h"
#else
  #include "comdef.h"
  #include "target.h"
  #include "task.h"
  #include "rex.h"
  #include "clkregim.h"
#endif /* WINSIM */

/*===========================================================================

                           DATA DECLARATIONS

===========================================================================*/

/*===========================================================================
                      TYPE DECLARATIONS
===========================================================================*/

typedef uint32 sleep_okts_handle;
     
/*--------------------------------------------------------------------------
                             Signal Definitions
--------------------------------------------------------------------------*/

/* 
                  TASK_START_SIG == 0x00008000
                   TASK_STOP_SIG == 0x00004000
                TASK_OFFLINE_SIG == 0x00002000
*/

/* This signal is when the RF driver wants Sleep to trigger its temp
   comp process. */
#define  SLEEP_RF_TEMP_COMP_SIG      0x00100000

/* This signal is set when the gSleepRptTimer expires.
   This timer is used to enable Sleep to kick the watchdog on time
   during a wait for some event. */
#define  SLEEP_RPT_TIMER_SIG           0x00000001

/*--------------------------------------------------------------------------
  Sleep swfi vote type 
--------------------------------------------------------------------------*/
typedef enum
{
  SLEEP_SWFI_RESTRICT_OFF = 0x0,
  SLEEP_SWFI_RESTRICT_ON
} sleep_swfi_restrict_type;

/*--------------------------------------------------------------------------
  Sleep vdd min vote type (for modem processor)
--------------------------------------------------------------------------*/
typedef enum
{
  VDD_MIN_ALLOWED = 0x0,
  VDD_MIN_NOT_ALLOWED
} sleep_vdd_min_restrict_type;

/* Specific resources that can be voted upon individually. */
typedef enum
{
  SLEEP_RESOURCE_MICROPROC_SLEEP = 0x01, /* Local processor's simple halt */
  SLEEP_RESOURCE_TCXO            = 0x02, /* The actual TCXO clock */
  SLEEP_RESOURCE_MEMORY          = 0x04, /* Access to memory */
  SLEEP_RESOURCE_VDD_MIN         = 0x08, /* Modem voltage minimization */
  SLEEP_RESOURCE_APPS_SLEEP      = 0x10, /* Apps SWFI w/notification */
  SLEEP_RESOURCE_APPS_PWRC       = 0x20, /* Apps power collapse */
  SLEEP_RESOURCE_RBC1            = 0x40  /* A variety of clocks currently shut
                                            down when memory is freed, but do
                                            not depend on that behavior. */
} sleep_resources_type;

/* Used to indicate if TCXO must be kept on or not for the UART */
extern boolean sleep_uart_clock_rgm;

/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION SLEEP_REGISTER

DESCRIPTION
  Required at init time for any client that wants a unique vote for sleep.
  Pointer passed in must be unique, and the initial vote for sleep is
  indicated by the second parameter.

DEPENDENCIES
  None.

RETURN VALUE
  Sleep client handle, used for subsequent calls to assert and negate.

SIDE EFFECTS
  After this function completes, a new client record in the internal sleep 
  array structure is reserved for the calling client.

===========================================================================*/

sleep_okts_handle sleep_register
( 
  const char      *pszName, 
  boolean          bOkts
);

/*===========================================================================

FUNCTION SLEEP_DEGREGISTER

DESCRIPTION
  Remove a client from the list of those currently registered

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void sleep_deregister (sleep_okts_handle handle);

/*===========================================================================

FUNCTION SLEEP_ASSERT_OKTS

DESCRIPTION
  Used by each sleep client to indicate that, as far as that client is
  concerned, it is ok to sleep (tcxo off).  Client must pass in handle
  obtained from register call at init time.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  Client's internal sleep vote is set to TRUE.

===========================================================================*/

void sleep_assert_okts
(
  sleep_okts_handle handle
);

/*===========================================================================

FUNCTION SLEEP_NEGATE_OKTS

DESCRIPTION
  Used by each sleep client to indicate that, as far as that client is
  concerned, it is not ok to sleep (tcxo on).  Client must pass in handle
  obtained from register call at init time.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  Client's internal sleep vote is set to FALSE.

===========================================================================*/

void sleep_negate_okts
(
  sleep_okts_handle handle
);

/*===========================================================================

FUNCTION SLEEP_SET_MCLK_RESTRICTION

DESCRIPTION
  Allow sleep client to place a restriction on mclk.  If a client requires
  mclk during sleep, the client can use this function to indicate that.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  Client's internal mclk_restriction is set

===========================================================================*/

void sleep_set_mclk_restriction
(
  sleep_okts_handle       handle,
  clkrgm_mclk_sleep_type  mclk_restriction
);

/*===========================================================================

FUNCTION SLEEP_SET_HCLK_RESTRICTION

DESCRIPTION
  Allow sleep client to place a restriction on hclk.  If a client requires
  hclk during sleep, the client can use this function to indicate the speed
  that is required.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  Client's internal hclk_restriction is set

===========================================================================*/

void sleep_set_hclk_restriction
(
  sleep_okts_handle       handle,
  clkrgm_hclk_sleep_type  hclk_restriction
);

/*===========================================================================

FUNCTION SLEEP_SET_SWFI_RESTRICTION

DESCRIPTION
  Allow sleep client to place a restriction on swfi.  If a client requires 
  that SWFI (microprocessor sleep) not happen, then it can use this function
  to indicate that.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  Client's internal swfi_restriction is set

===========================================================================*/

void sleep_set_swfi_restriction
(
  sleep_okts_handle          handle,
  sleep_swfi_restrict_type   swfi_restriction
);

/*===========================================================================

FUNCTION SLEEP_SET_VDD_MIN_RESTRICTION

DESCRIPTION
  Allow sleep client to place a restriction on vdd minimization.
  This would only occur if tcxo shutdown (sleep) occurs, so is only relevant
  if the clients sleep vote is okts.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  Client's internal vdd_min_restriction is set

===========================================================================*/

void sleep_set_vdd_min_restriction
(
  sleep_okts_handle             handle,
  sleep_vdd_min_restrict_type   vdd_min_restriction
);

/*===========================================================================

FUNCTION SLEEP_GET_ACQUIRED_RESOURCES

DESCRIPTION
  This function can be used to get a bitmask that represents all types of sleep
  that currently have someone voting against them.

DEPENDENCIES
  None.

RETURN VALUE
  See description.

SIDE EFFECTS
  None.

===========================================================================*/

extern uint32 sleep_get_acquired_resources( void );

/*===========================================================================

FUNCTION SLEEP_ACQUIRE_RESOURCES

DESCRIPTION
  This function changes state of internal sleep values, marking the resources
  passed in the second parameter as "in use."  Resources in use will never be
  transitioned to a low power state.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Some types of sleep may cease to occur.

===========================================================================*/

void sleep_acquire_resources
(
  sleep_okts_handle handle,
  uint32 resources
);

/*===========================================================================

FUNCTION SLEEP_RELEASE_RESOURCES

DESCRIPTION
  This function changes state of internal sleep values, marking the resources
  passed in the second parameter as not "in use."  Resources not in use may
  be transitioned to a low power state.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Some previously disabled types of sleep may start to occur.

===========================================================================*/

void sleep_release_resources
(
  sleep_okts_handle handle,
  uint32 resources
);

/*===========================================================================

FUNCTION SLEEP_GET_TCXO_SHUTDOWN_COUNT

DESCRIPTION
  Returns the number of times the TCXO has been shutdown since the phone
  was turned on.

DEPENDENCIES
  None

SIDE EFFECTS
  None

===========================================================================*/

extern uint32 sleep_get_tcxo_shutdown_count( void );

/*===========================================================================

FUNCTION SLEEP_TASK

DESCRIPTION
  This procedure contains the main processing loop for the sleep task.
  It continually loops, looking for a consensus that the CPU may be
  put to sleep.  Lacking a consensus, it still halts the processor, but
  in "idle" mode -- in which any normal interrupt will restart processing
  with minimal delay.

DEPENDENCIES
  This procedure should be called by rex_def_task.

RETURN VALUE
  This procedure does not return.

SIDE EFFECTS
  The CPU may be put to sleep by this task.

===========================================================================*/

extern void sleep_task
(
  /* Parameter received from Main Control task - ignored */
  /*lint -esym(715,ignored)
     Have lint not complain about the ignored parameter 'ignored' which is
     specified to make this routine match the template for rex_def_task(). */
  dword ignored
);

/*===========================================================================

FUNCTION SLEEP_INIT

DESCRIPTION
  Initialize the sleep subsystem.

DEPENDENCIES
  timer_init() must already be called, and the timer task must be started.

RETURN VALUE
  None

SIDE EFFECTS
  A timer is set which causes sleep to be withheld until it expires.

===========================================================================*/

extern void sleep_init( void );

#endif /* SLEEP_H */

