/*! \file  pm_pbs_info.h
 *
 *  \brief  This file contains the pmic PBS info driver definitions.
 *  \details  This file contains the pm_pbs_info_init & pm_pbs_info_store_glb_ctxt
 *  API definitions.
 *
 *  &copy; Copyright 2013 QUALCOMM Technologies Incorporated, All Rights Reserved
 */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This document is created by a code generator, therefore this section will
  not contain comments describing changes made to the module.

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/systemdrivers/pmic/drivers/pbs/src/pm_pbs_info.h#1 $
$DateTime: 2015/09/01 00:30:35 $  $Author: pwbldsvc $

when         who     what, where, why
----------   ---     ----------------------------------------------------------
09/29/14     mr      Updated/Modified PBS Driver (CR-728234)
04/05/2013   kt      Created.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES

===========================================================================*/
#include "pm_err_flags.h"
#include "pm_target_information.h"

/*===========================================================================

                    MACRO AND GLOBAL VARIABLES

===========================================================================*/
/* PBS ROM/RAM Size can be 128 or 256 words. */
/* PBS OTP/ROM Start Address */
#define PM_PBS_ROM_BASE_ADDR            0x000
/* PBS RAM Start Address */
#define PM_PBS_RAM_BASE_ADDR            0x400

/* PBS Memory Version stored at the last line */
#define PM_PBS_MEMORY_VER_LINE_FROM_END   1
/* MFG Info stored at the 7th from last line (if present in PBS ROM) */
#define PM_PBS_ROM_INFO_LINE_FROM_END     7

typedef enum
{
    PM_PBS_INFO_IN_OTP,
    PM_PBS_INFO_IN_MISC,
    PM_PBS_INFO_INVALID
} pm_pbs_info_place_holder_type;

typedef struct
{
    uint16 pbs_mem_size;
    pm_pbs_info_place_holder_type pbs_info_place_holder;
    pm_register_address_type misc_base_addr;
    pm_register_address_type TRIM_NUM;
    pm_register_address_type TP_REV;
    pm_register_address_type FAB_ID;
    pm_register_address_type WAFER_ID;
    pm_register_address_type X_CO_ORDI;
    pm_register_address_type Y_CO_ORDI;
    pm_register_address_type LOT_ID;
    pm_register_address_type MFG_ID;
} pm_pbs_info_data_type;


/*===========================================================================

                Function Definitions

============================================================================*/
/**
 * @name pm_pbs_info_init
 *
 * @brief This function is called to initialize the PBS Info driver.
 *        This function internally validates the PBS Core peripheral
 *        info to determine the valid PMIC Chips and calls an internal
 *        helper function to read PBS Manufacturing IDs and foundry
 *        information such as PBS Lot ID, ROM Version, RAM Version,
 *        Fab Id, Wafer Id, X coord and Y coord and stores it in static
 *        global variable. This function is called during pm_device_init
 *        after the PBS RAM data is loaded.
 *
 * @param None.
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = PBS peripheral is not
 *          supported.
 *          else SPMI errors
 */
pm_err_flag_type pm_pbs_info_init (void);

/**
 * @name pm_pbs_info_store_glb_ctxt
 *
 * @brief This function is called to copy the PBS info to Global
 *        Context (SMEM) from static global variables where the
 *        PBS info is stored during PBS Info initilization.
 *
 * @param None.
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Error in
 *          copying to shared memory.
 */
pm_err_flag_type pm_pbs_info_store_glb_ctxt (void);
