#ifndef RFC_PM_H
#define RFC_PM_H
/*! @todo dchow:11/04/08: Update comments */
/*!
   @file
   rfc_pm_qrf7600.h

   @brief
   This file contains the common power management interface and definitions.

   @details


*/

/*===========================================================================

Copyright (c) 2008 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$DateTime: 2016/12/13 07:59:52 $ $Author: mplcsds1 $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/rf/rfc/inc/rfc_pm.h#1 $

when       who     what, where, why
-------------------------------------------------------------------------------
09/23/10    ap     Added definitions for RF PM Voltage Levels
11/05/08   dyc     Initial version.

============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
#include "pm.h"

/*============================================================================== 
                               MACROS AND TYPEDEFS 
==============================================================================*/

#define RF_PM_2850_MV 2850
#define RF_PM_2700_MV 2700
#define RF_PM_2600_MV 2600
#define RF_PM_1300_MV 1300
#define RF_PM_1800_MV 1800
#define RF_PM_2200_MV 2200
#define RF_PM_2050_MV 2050

/*===========================================================================
                           Data Definitions
===========================================================================*/
/* ----------------------------------------------------------------------- */
/*!
   @brief
   This enum structure defines RF VREGs types.

   @detail
   The RF power grid for a particular platform may or may not support all
   the VREG groupings identified by this type.

*/
typedef enum
{
   RFC_RX_VREGS,           /*!< Rx VREG */
   RFC_RX_DIV_VREGS,       /*!< Diversity VREG */
   RFC_TX_VREGS,           /*!< Tx VREG */
   RFC_PA_VREGS,           /*!< PA VREG */
   RFC_PA_THERM_VREGS,     /*!< PA THERMISTOR VREG */
   RFC_GPS_VREGS           /*!< GPS VREG */
} rfc_vregs_id_type;

/* ----------------------------------------------------------------------- */
/*!
   @brief
   This enum data structure defines RF VREG commands.

   @details
   The enum values will be mapped to the PMIC equivalent ON/OFF commands.
   It is done here to limit the exposure of all PMIC interfaces to this
   module.

*/
typedef enum
{
   RFC_ON_CMD  = PM_ON_CMD,    /*!< Turns on VREG grouping */
   RFC_OFF_CMD = PM_OFF_CMD    /*!< Turns off VREG grouping */
} rfc_vregs_cmd_type;

/*===========================================================================
                           Function Prototypes
===========================================================================*/
/* ----------------------------------------------------------------------- */
/*!
   @brief
   RFC power management configuration function pointer.

   @details
   This function will point to function that will configure the power
   management attributes such as voltage levels, SMPS frequencies etc.

   The pointer supports configuring different power grids after transitioning
   modes.
*/
typedef void (*rfc_pm_config_vregs_type)(void);

/* ----------------------------------------------------------------------- */
/*!
   @brief
   RFC power management management function pointer.

   @details
   This function will point to function that will enable or disable the
   VREG group.

   @parameter
   vreg VREG group type to control
   cmd  ON/OFF command to enable/disable VREG
*/
typedef void (*rfc_pm_manage_vregs_type)(rfc_vregs_id_type,  \
                                         rfc_vregs_cmd_type);


/*===========================================================================
               Data Definitions (after function prototype)
===========================================================================*/
/* ----------------------------------------------------------------------- */
/*!
   @brief
   This data structure contains function ptr to the RF platform specific
   power management APIs.

   @details
   This data structure is to be referenced by the top level rfc_configuration
   pointer which will assign the appropriate PM management API based on path
   and mode.

*/
typedef struct
{
   rfc_pm_config_vregs_type rfc_pm_config_vregs;
   rfc_pm_manage_vregs_type rfc_pm_manage_vregs;
} rfc_pwr_mgnt_type;


extern rfc_pwr_mgnt_type *rfc_pwr_mgnt_table[]; 

#endif /* RFC_PM_H */



