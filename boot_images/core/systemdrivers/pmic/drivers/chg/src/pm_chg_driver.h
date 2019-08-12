#ifndef PM_CHG_DRIVER_H
#define PM_CHG_DRIVER_H

/*! \file
*  \n
*  \brief  pm_chg_driver.h 
*  \details  This file contains functions prototypes and variable/type/constant
*  declarations for supporting chg peripheral 
*  
*  \n &copy; Copyright 2014 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/* =======================================================================
Edit History
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/03/14   pxm     Created
========================================================================== */

/*===========================================================================
						INCLUDE FILES
===========================================================================*/
#include "pm_comm.h"

typedef struct
{
    pm_comm_info_type  *comm_ptr;
    pm_chg_type chg_sub_type;
} pm_chg_data_type;

#endif /* PM_CHG_DRIVER_H */

