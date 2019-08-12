/*! \file pm_uicc_app.c
*  \n
*  \brief UICC Application APIs implementation.
*  \n  
*  \n &copy; Copyright 2011-2013 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/* =======================================================================
                            Edit History
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/pmic/application/uicc/src/pm_uicc_app.c#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
10/22/13   rk      GPIO1 used for BUA in 8916.
08/26/13   rk      removing GPIO configuration for BUA as this is no more used in 8916.
07/12/12   umr     Merge to src. Remove PBS Done int.
07/12/12   umr     Add support for BUA. 
04/10/12   hs      Changed the type for internalResourceIndex from int to uint8.
02/24/12   dy      Add API to Get and Set Hotswap Polarity
07/14/11   dy      Add UICC2 and change GPIO config
06/16/11   hs      Added me->uiccCommonDS to hold NumofMods.
05/23/11   vk      Initial version
========================================================================== */
/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_uicc_app.h"
#include "CoreVerify.h"

/*===========================================================================

                        STATIC VARIABLES 

===========================================================================*/

/* Static global variable to store the UICC data */
static pm_uicc_app_data_type pm_uicc_data;

/*===========================================================================

                        FUNCTION DEFINITIONS

===========================================================================*/
void pm_uicc_app_init()
{
    if (pm_uicc_data.periph_exists == FALSE)
    {
        pm_uicc_data.periph_exists = TRUE;
		
		/* UICC Specific Info - Obtaining Data through dal config */
        pm_uicc_data.uicc_specific = (pm_uicc_specific_info_type*)pm_target_information_get_specific_info(PM_PROP_UICC_SPECIFIC);
		
        CORE_VERIFY_PTR(pm_uicc_data.uicc_specific);
     }
}

pm_uicc_app_data_type* pm_uicc_app_get_data()
{
	return &pm_uicc_data;
}

