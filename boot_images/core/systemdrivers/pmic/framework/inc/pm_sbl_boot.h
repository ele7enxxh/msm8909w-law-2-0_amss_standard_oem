#ifndef __PM_SBL_BOOT__H__
#define __PM_SBL_BOOT__H__

/*! \file pm_sbl_boot.h
*  \n
*  \brief This header file contains enums and helper function declarations needed
*  \n during PMIC device initialization and during initial PMIC SBL settings configured
*  \n through the PDM auto-generated code.
*  \n
*  \n &copy; Copyright 2010-2013 QUALCOMM Technologies Incorporated, All Rights Reserved
*/
/* =======================================================================
Edit History
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/systemdrivers/pmic/framework/inc/pm_sbl_boot.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/03/13   kt      Added Comment header for this file. 
 
========================================================================== */
/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/

#include "pm_err_flags.h"
#include "pm_version.h"

/*===========================================================================

                        TYPE DEFINITIONS 

===========================================================================*/


/*===========================================================================

                        FUNCTION PROTOTYPES 

===========================================================================*/

pm_err_flag_type pm_clk_busy_wait (unsigned int mS);

pm_err_flag_type pm_driver_post_init (void);

pm_err_flag_type pm_boot_tcxo_config (void);

pm_err_flag_type pm_clk_tcxo_config(void);

pm_err_flag_type pm_pon_init(void);

#endif // __PM_SBL_BOOT__H__
