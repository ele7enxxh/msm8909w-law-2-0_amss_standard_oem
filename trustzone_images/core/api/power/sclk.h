/*===*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                     S L E E P  /  S L O W   C L O C K

GENERAL DESCRIPTION
  This file contains routines which manage the sleep clock timing.

EXTERNALIZED FUNCTIONS (Global)
  sclk_init
  sclk_begin_estimate
  sclk_estimate_complete
  sclk_get_frequency
  sclk_begin_error_adjust
  sclk_sysclk_error_adjust
  sclk_sysclk_error_adjust_gsm
  sclk_sysclk_error_adjust_wcdma
  sclk_slow_clock_unstable
  sclk_from_chips
  sclk_to_chipx16
  sclk_to_timestamp
  sclk_propagate_estimate

EXTERNALIZED FUNCTIONS (Regional)
  None

INITIALIZATION AND SEQUENCING REQUIREMENTS
  sclk_init( ) must be called first
  sclk_begin_estimate( ) must be called before sclk_estimate_complete()
  sclk_begin_error_adjust( ) must be called before sclk_error_adjust()
  sclk conversion functions (_to_xxx, _from_xxx) require a valid estimate

Copyright (c) 2000-2009
              by Qualcomm Technologies, Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

============================================================================

                        EDIT HISTORY FOR MODULE

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/power/sclk.h#1 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when         who   what, where, why
----------   ---   -------------------------------------------------------
12/21/2009   rks   add sclk_set_system_sysclk_freq to allow sysclk freq
                   change at run-time
12/11/2009   cab   add sclk_to_timestamp_64
                   update to sclk_get_nominal_frequency to use pmic
                   add sclk_get_maximum_error
11/16/2009   cab   Added sclk_get_nominal_frequency
10/13/2009   clm   Added MDM9x00 target
08/22/2006   cab   WCDMA integration
08/08/2006   cab   7200 integration updates; set cx8_per_sclk in sclk_init
12/02/2005   cab   Estimate complete data is now system specific
09/01/2005   cab   Added SCLK_32KHZ_FAIR_ESTIMATE for 0.5 sec estimate
03/22/2005   ddh   Moved sclk_to_ms and _us functions to timetick
09/07/2004   ajn   Added uS to SCLK macro
08/24/2004   ajn   Removed sclk_enum_type.  Changed chipx16's to sys_clk's
05/26/2004   jcm   Rewrite slow clock error filter
03/04/2004   ajn   Added nominal frequency #define and MS-to-SCLK macro
10/20/2003   jcm   Added slow_clock from microsecond function
07/22/2003   ajn   Added slow-clock to TimeStamp conversion function
07/20/2003   bt    T_MSM6500 updates.
01/30/2003   ajn   Allow external slow_clock_unstable indication
01/23/2002   ajn   Added Number of Systems identified sclk_sys_enum_type
01/30/2002   ajn   sclk_estimate_complete returns tri-state enum
01/01/2002   ajn   Added parameter to control whether 1x or HDR is adjusting
                   (calibrating) the sleep clock frequency.
10/19/2001   ajn   Added "to_ms" function
09/24/2001   ajn   hdrerrno -> errno.  "to_chipx16" now accepts a uint32
07/26/2001   ajn   SRCH/SLEEP VU tip remerge - first check in
04/30/2001   ajn   HDR/1x merge
11/02/2000   ajn   Added sclk reserve (typ 2 clks) to sleepclk_from_chips
10/31/2000   ajn   Created module

==========================================================================*/


#ifndef SCLK_H
#define SCLK_H

/*==========================================================================

                     INCLUDE FILES FOR MODULE

==========================================================================*/

#include "comdef.h"
#include "IxErrno.h"
#include "qw.h"


/*==========================================================================

                     DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, typesdefs,
and other items needed by this module.

==========================================================================*/


/*--------------------------------------------------------------------------
  System for Sleep Clock
--------------------------------------------------------------------------*/

typedef enum
{
  SCLK_HDR,
  SCLK_1X,
  SCLK_GSM,
  SCLK_WCDMA,
  SCLK_OFDMA,

  /* Insertion point */

  SCLK_NUM_SYSTEMS
}
sclk_sys_enum_type;


/*--------------------------------------------------------------------------
  32kHz Slow Clock frequency estimate
     Rough - for millisecond timing accuracy
     Fair  - for (barely good enough) chip level timing accuracy
     Good  - for chip level timing accuracy
--------------------------------------------------------------------------*/

typedef enum
{
  SCLK_32KHZ_NO_ESTIMATE,
  SCLK_32KHZ_ROUGH_ESTIMATE      = 8,        /* FEE calculation time in ms */
  SCLK_32KHZ_FAIR_ESTIMATE       = 500,      /* FEE calculation time in ms */
  SCLK_32KHZ_GOOD_ESTIMATE_OFDMA = 900,      /* FEE calculation time in ms 
                                                Estimate request cannot exceed
                                                900ms for OFDMA to avoid
                                                FEE_SLEEP_XTAL_FREQ_ERR counter
                                                rollover. */
  SCLK_32KHZ_GOOD_ESTIMATE       = 1000      /* FEE calculation time in ms */
}
sclk_32kHz_estimate_enum_type;

#define SCLK_DEFAULT_SLOW_CLOCK_NOM_VALUE         32768
#define SCLK_EXT_SLOW_CLOCK_NOM_VALUE             32768
#define SCLK_TCXO_DIV_SLOW_CLOCK_NOM_VALUE        (19200000/586)

#define SCLK_DEFAULT_SLOW_CLOCK_ERR_VALUE         200
#define SCLK_EXT_SLOW_CLOCK_ERR_VALUE             200
#define SCLK_TCXO_DIV_SLOW_CLOCK_ERR_VALUE        20

/* <EJECT> */
/*==========================================================================

                    EXTERN VARIABLES and FUNCTIONS

This section contains variables that are accessible outside this module
==========================================================================*/


/*==========================================================================

FUNCTION SCLK_INIT

DESCRIPTION
  This function initializes the Sleep/Slow Clock module.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Frequency data is cleared.

==========================================================================*/

void sclk_init( void );


/* <EJECT> */
/*==========================================================================

FUNCTION SCLK_BEGIN_ESTIMATE

DESCRIPTION
  This function initializes causes a frequency estimate of the slow clock
  to be started.  The best estimate available for SCLK_OFDMA fee_system is
  SCLK_32KHZ_GOOD_ESTIMATE_OFDMA.

DEPENDENCIES
  sclk_init( )

RETURN VALUE
  E_SUCCESS     - Estimation process is beginning
  E_IN_PROGRESS - An estimation is currently underway, can't start a new
                  estimate right now.
  E_NOT_ALLOWED - FEATURE_SLEEP_SLOW_CLOCK is not defined or invalid 
                  estimate value for SCLK_OFDMA fee_system.

SIDE EFFECTS
  "complete_cb( )" will be called from interrupt context.  The callback
  function should cause the interested task to call the "estimate available"
  function to complete the estimation process.

==========================================================================*/

typedef
void (*sclk_32kHz_est_complete_cb_type)(void);

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

errno_enum_type sclk_begin_estimate
(
  sclk_sys_enum_type                fee_system,
    /* Collect FEE using 1x or HDR system FEE circuitry */

  sclk_32kHz_estimate_enum_type     estimate,
    /* Specifies the required accuracy of the estimate */

  sclk_32kHz_est_complete_cb_type   complete_cb
    /* Estimate complete callback function */
);


/* <EJECT> */
/*==========================================================================

FUNCTION SCLK_ESTIMATE_COMPLETE

DESCRIPTION
  Attempts to complete the frequency estimate, if it has not already been
  completed.  The best estimate available for SCLK_OFDMA fee_system is
  SCLK_32KHZ_GOOD_ESTIMATE_OFDMA.

DEPENDENCIES
  sleepclk_init( ).

RETURN VALUE
  E_SUCCESS     - an estimate is available
  E_IN_PROGRESS - the given estimate is still underway.
  E_FAILURE     - the estimate is neither available or in progress.

SIDE EFFECTS
  If the frequency has never been estimated, this begins an estimate.
  If the frequency estimate is completed, the FEE ISR is uninstalled.

==========================================================================*/

errno_enum_type sclk_estimate_complete
(
  sclk_sys_enum_type                fee_system,
    /* Collect FEE using 1x or HDR system FEE circuitry */

  sclk_32kHz_estimate_enum_type     estimate
    /* Accuracy of the slow clock frequency estimate */
);


/*==========================================================================

FUNCTION SCLK_ABORT_ESTIMATE

DESCRIPTION
  Abort any estimate in progress.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Uninstalls FEE ISR's.

==========================================================================*/

void sclk_abort_estimate( void );


/*==========================================================================

FUNCTION SCLK_SLOW_CLOCK_UNSTABLE

DESCRIPTION
  This function is used to indicate if the slow clock frequency is
  unstable, due to a disturbance such as PMIC initialization.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Prevents sclk_estimate_complete( ) from returning E_SUCCESS while
  slow clock unstable is TRUE.

==========================================================================*/

void sclk_slow_clock_unstable
(
  boolean                         unstable
    /* Slow clock stability status */
);


/* <EJECT> */
/*===========================================================================

FUNCTION SCLK_GET_MAXIMUM_ERROR

DESCRIPTION
  Returns the maximum devation of the slow clock frequency from nominal
  value

DEPENDENCIES

RETURN VALUE
  The error magnitude in ppm

SIDE EFFECTS
  None

===========================================================================*/

uint32 sclk_get_maximum_error( void );


/*===========================================================================

FUNCTION SCLK_GET_NOMINAL_FREQUENCY

DESCRIPTION
  Returns the sleep clock nominal frequency.

DEPENDENCIES

RETURN VALUE
  The nominal sleep clock frequency, in Hz

SIDE EFFECTS
  None

===========================================================================*/

uint32 sclk_get_nominal_frequency( void );

/*===========================================================================

FUNCTION SCLK_GET_FREQUENCY

DESCRIPTION
  Returns the sleep clock frequency.

DEPENDENCIES
  sclk_estimate_complete( ) must return true.

RETURN VALUE
  The sleep clock frequency, in Hz

SIDE EFFECTS
  None

===========================================================================*/

uint32 sclk_get_frequency( void );


/* <EJECT> */
/*===========================================================================

FUNCTION SCLK_GET_mFREQUENCY

DESCRIPTION
  Returns the sleep clock frequency in mHz.

DEPENDENCIES
  sclk_estimate_complete( ) must return true.

RETURN VALUE
  The sleep clock frequency, in mHz

SIDE EFFECTS
  None

===========================================================================*/
uint32 sclk_get_mfrequency(void);

/* <EJECT> */
/*===========================================================================

FUNCTION SCLK_SET_SYSTEM_SYSCLK_FREQ

DESCRIPTION
  Sets the system frequency of system to freq

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  Error estimate on system frequency is invalidated

===========================================================================*/
void sclk_set_system_sysclk_freq
(
  sclk_sys_enum_type             system,
    /* System to be initialized */
  
  uint32                          sysclk_freq
    /* System clock frequency */
);

/* <EJECT> */
/*===========================================================================

FUNCTION SCLK_BEGIN_ERROR_ADJUST

DESCRIPTION
  Records the start of a long duration calibration.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void sclk_begin_error_adjust
(
  sclk_sys_enum_type              sclk_system
    /* System which is beginning the long duration measurement */
);


/*===========================================================================

FUNCTION SCLK_SYSCLK_ERROR_ADJUST

DESCRIPTION
  Alters the sleep clock frequency based on an sys_clk error term (e.g. the
  reacquisition slew).

DEPENDENCIES
  A valid slow clock frequency estimate.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void sclk_sysclk_error_adjust
(
  sclk_sys_enum_type              sclk_system,
    /* System which has completed the long duration measurement */

  uint32                          sleep_sclks,
    /* The duration the error measurement was computed over in slow clocks */
  
  uint32                          sleep_sysclk,
    /* The duration the error measurement was computed over in sys_clk's */

  int16                           sysclk_error
    /* The amount of error which accumulated over above interval */
);


/* <EJECT> */
/*===========================================================================

FUNCTION SCLK_SYSCLK_ERROR_ADJUST_GSM

DESCRIPTION
  Alters the sleep clock frequency based on an sys_clk error term (e.g. the
  reacquisition slew).

DEPENDENCIES
  A valid slow clock frequency estimate.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void sclk_sysclk_error_adjust_gsm
(
  sclk_sys_enum_type              sclk_system,
    /* System which has completed the measurement */

  uint32                          sleep_sclks,
    /* The duration the error measurement was computed over in slow clocks */
  
  uint32                          sleep_sysclk,
    /* The duration the error measurement was computed over in chips */

  int16                           sysclk_error
    /* The amount of error which accumulated over above interval */
);

/*===========================================================================

FUNCTION SCLK_SYSCLK_ERROR_ADJUST_WCDMA

DESCRIPTION
  Alters the sleep clock frequency based on an sys_clk error term (e.g. the
  reacquisition slew).

DEPENDENCIES
  A valid slow clock frequency estimate.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void sclk_sysclk_error_adjust_wcdma
(
  sclk_sys_enum_type              sclk_system,
    /* System which has completed the measurement */

  uint32                          sleep_sclks,
    /* The duration the error measurement was computed over in slow clocks */
  
  uint32                          sleep_sysclk,
    /* The duration the error measurement was computed over in chips */

  int16                           sysclk_error
    /* The amount of error which accumulated over above interval */
);

/*===========================================================================

FUNCTION SCLK_TO_SYSCLKS

DESCRIPTION
  Determine the number of sys_clk's for a given number of slow clocks

DEPENDENCIES
  A good slow clock frequency estimate.

RETURN VALUE
  sys_clks.

SIDE EFFECTS
  None

===========================================================================*/

uint32 sclk_to_sysclks
(
  sclk_sys_enum_type              sclk_system,
    /* System conversion is being done for */

  uint32                          sclks
    /* Duration to be converted into sysclks */
);


/*===========================================================================

FUNCTION SCLK_FROM_SYSCLKS

DESCRIPTION
  Determine the number of slow clocks which correspond to the given number
  of sysclks.
  
  2 slow clocks worth of sys_clk's are excluded from the conversion to
  slow clocks, and included in the remainder_sysclk value.

DEPENDENCIES
  A valid slow clock frequency estimate.

RETURN VALUE
  slow clocks

SIDE EFFECTS
  None

===========================================================================*/

uint32 sclk_from_sysclks
(
  sclk_sys_enum_type              sclk_system,
    /* System conversion is being done for */

  uint32                          duration_sysclks,
    /* Duration to be converted into slow clocks */

  uint16 *                        remainder_sysclks
    /* Residual sysclk count that could not be converted to slow clocks */
);


/* <EJECT> */
/*===========================================================================

FUNCTION SCLK_TO_TIMESTAMP

DESCRIPTION
  Given a base timestamp, and an sclk offset, determine the resulting
  timestamp.

DEPENDENCIES
  A valid slow clock frequency estimate.

RETURN VALUE
  None (Parameter 'timestamp' modified)

SIDE EFFECTS
  None

===========================================================================*/

void sclk_to_timestamp
(
  qword                           timestamp,
    /* In: Base timestamp;  Out: Timestamp incremented by sclks */

  uint32                          sclks
    /* Number of slow clocks to advance timestamp by */
);


/*===========================================================================

FUNCTION SCLK_TO_TIMESTAMP_64

DESCRIPTION
  Given a base timestamp, and an sclk offset, determine the resulting
  timestamp.  The offset is given in a 64bit word.

DEPENDENCIES
  A valid slow clock frequency estimate.

RETURN VALUE
  None (Parameter 'timestamp' modified)

SIDE EFFECTS
  None

===========================================================================*/
void sclk_to_timestamp_64
(
  qword                           timestamp,
    /* In: Base timestamp;  Out: Timestamp incremented by sclks */

  uint64                          sclks
    /* Number of slow clocks to advance timestamp by */
);


/*===========================================================================

FUNCTION SCLK_PROPAGATE_ESTIMATE

DESCRIPTION
  Convert the FEE result generated using sys_clk from_id to the equivalent
  estimate for sys_clk to_id

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void sclk_propagate_estimate
( 
  sclk_sys_enum_type from_id, 
    /* System from which the estimate is being transferred */

  sclk_sys_enum_type to_id 
    /* System to which the estimate is being transferred */
);

#endif /* SCLK_H */
