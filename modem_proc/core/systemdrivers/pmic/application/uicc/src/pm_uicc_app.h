#ifndef __PM_UICC_APP_H__
#define __PM_UICC_APP_H__

/*! \file pm_uicc_app.h
*  \n
*  \brief UICC Application type definitions and function prototypes.
*  \n
*  \n &copy; Copyright 2011-2013 Qualcomm Technologies Incorporated, All Rights Reserved
*/

/* =======================================================================
                                Edit History
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/pmic/application/uicc/src/pm_uicc_app.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/10/12   hs      Changed the type for internalResourceIndex from int to uint8.
02/24/12   dy      Add API to set UICC Hotswap Polarity
06/16/11   hs      Added uiccCommonDS to hold NumofMods.
05/23/11   vk      Initial version.
========================================================================== */

/*===========================================================================

                     INCLUDE FILES

===========================================================================*/
#include "pm_err_flags.h"
#include "pm_target_information.h"
#include "pm_bua.h"


/*===========================================================================

                     TYPE DEFINITIONS

===========================================================================*/

typedef struct
{
    uint8 uicc_gpio;
    uint8 pbs_client;
    uint8 uicc1_ldo;
    uint8 uicc2_ldo;
    uint8 uicc3_ldo;
    uint8 uicc4_ldo;
} pm_uicc_specific_info_type;


typedef struct
{
    boolean      periph_exists;
    pm_uicc_specific_info_type* uicc_specific;
}pm_uicc_app_data_type;

/*===========================================================================

                        FUNCTION PROTOTYPES

===========================================================================*/
void pm_uicc_app_init(void);

pm_uicc_app_data_type* pm_uicc_app_get_data(void);

pm_err_flag_type pm_uicc_cntrl_enable(pm_bua_alarm_type alarm, boolean enable);

#endif /* __PM_UICC_APP_H__ */
