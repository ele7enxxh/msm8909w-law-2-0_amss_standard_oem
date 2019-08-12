#ifndef PM_QC_PMIC_H
#define PM_QC_PMIC_H
/*===========================================================================


                  P M    H E A D E R    F I L E

DESCRIPTION
  This file contains prototype definitions to support interaction
  with the QUALCOMM Power Management ICs.

Copyright (c) 2003 - 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/systemdrivers/pmic/pm_qc_pmic.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/16/12   hs      Removed obsolete files.
02/10/11   hw      Uncomment pmapp_pmic_events.h header
02/08/11   hw      Merging changes from the PMIC Distributed Driver Arch branch
06/28/03   wra     Created as final PMIC library main include file. Named
                   so that it will not have naming collision with other
                   HLOS header files. Most of the contents in this file
                   were formerly in pm.h
===========================================================================*/

#include "comdef.h"

/*===========================================================================

                        TYPE DEFINITIONS

===========================================================================*/


/*===========================================================================

                        ERROR FLAGS DEFINITIONS

===========================================================================*/



/*===========================================================================

                        PMIC INCLUDE FILES

===========================================================================*/
#include "pm_lib_api.h"         /* interface files for all library APIs */

#include "pm_diag.h"            /* Diagnotic messages  */
#include "pmapp_rtc.h"          /* RTC apps API        */
#include "pmapp_npa.h"          /* Contains the power node architecture support */
#include "vbatt.h"              /* VBATT ADC */

/*===========================================================================

                        GENERIC FUNCTION PROTOTYPES

===========================================================================*/
/*===========================================================================

FUNCTION pm_init                                EXTERNAL FUNCTION

DESCRIPTION
    This function initializes the PM60X0 for operation.

    It does the following:
    1)  It initializes the PMIC software driver memory
    2)  It initializes the LDOs voltages.
    3)  It enables and starts the 32Khz external sleep crystal

INPUT PARAMETERS
  None.

RETURN VALUE
  None.

DEPENDENCIES
  rflib_init() must have been called.

SIDE EFFECTS
  Interrupts are disable while writing and reading to/from sbi.

===========================================================================*/
extern void pm_init(void);

/*===========================================================================

FUNCTION pm_init_delayed                            EXTERNAL FUNCTION

DESCRIPTION
    This function initializes the PM60X0 ISR services. It disables all PMIC
  IRQs and it registers the PMIC ISR with the GPIO software driver.

INPUT PARAMETERS
  None

RETURN VALUE
  None

DEPENDENCIES
  pm_init(), tramp_init() and gpio_int_init() must have been called.

SIDE EFFECTS
  None

===========================================================================*/
extern void pm_init_delayed(void);

/*! \brief This function is used by clients to determine if the 
    PMIC pm_init() function has been called.
 *  \details This function is used by outside clients to determine
 *      if the PMIC SW driver has been initialized.
 *  \return void.
 *  \note This function can be called any time.
 *
 *  <b>Dependencies</b>
 *  \li None
 */ 
boolean is_pm_init_done ( void ) ;

/*! \brief PMIC SW initialization function for the boot build.
 *  \details This function is by boot build to initialize the PMIC
    software. This function was added so that we did not need
    the BOOT_BUILD featurization flag.
 *  \param vptr - This is a void pointer that can be utilized and defined 
 *                  more information is needed by the boot build PMIC SW
 *  \param numComms - This is the number of devices that are going to 
 *                     be initialized during the boot sequence.
 *  \return unsigned int - number of errors that occured during initialization.
 *  \note This function should only be called during boot initialization.
 *
 *  <b>Dependencies</b>
 *  \li Boot build
 */ 
unsigned int boot_pm_init ( void* vptr  , unsigned char numComms ) ;

void pm_rpm_init( void );
/*===========================================================================

                        PUBLIC CONSTANT DEFINITIONS

===========================================================================*/

#endif /* PM_QC_PMIC_H */
