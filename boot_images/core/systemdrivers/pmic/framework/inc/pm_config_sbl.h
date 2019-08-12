#ifndef __PMIC_SBL_CONFIG_H__
#define __PMIC_SBL_CONFIG_H__

/*! \file  pm_sbl_config.h
 *  
 *  \brief  Contains PMIC Set Mode driver declaration
 *  \details  This file contains the Set Mode driver API. These interfaces 
 *  should not be called directly. All entry into these interfaces should 
 *  from the application level set mode interfaces.
 *  
 *    PMIC code generation Version: 1.0.0.0
 *    PMIC code generation Locked Version: PM8026-x.x-OTPx.x-MSM8x26-07012013v1 - Approved
 *    This file contains code for Target specific settings and modes.
 *  
 *  &copy; Copyright 2013 Qualcomm Technologies, All Rights Reserved
 */

/*===========================================================================

                EDIT HISTORY FOR MODULE

  This document is created by a code generator, therefore this section will
  not contain comments describing changes made to the module over time.

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/systemdrivers/pmic/framework/inc/pm_config_sbl.h#1 $ 
$DateTime: 2015/09/01 00:30:35 $  $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
08/07/13   aab     Creation
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/

#include "pm_qc_pmic.h"
#include "com_dtypes.h"         /* Containse basic type definitions */
#include "pm_err_flags.h"     /* Containse the error definitions */
#include "pm_target_information.h"
#define PM_SBL_16BITADD_8BITDATA_REGULAR 2
#define PBS_RAM_DATA_SIZE 256
#define REV_ID_COMMON 0xFFFFFFFF
#define SET_TEST_MAX_STR_LENGTH 60 /* For settings validation test */
#define OFFSET_SEC_ACCESS 0xD0 /* For settings validation test */
#define OFFSET_INT_EN 0x15 /* For settings validation test */

/*=============================================================================

DEFINITIONS

=============================================================================*/

typedef enum
{
   PM_SBL_WRITE,
   PM_SBL_DELAY,
   PM_SBL_PBS_RAM,
   PM_SBL_NOP,
   PM_SBL_OPERATION_INVALID
}pm_sbl_reg_operation_type;

 

typedef enum  
{
   EQUAL,
   GREATER,
   GREATER_OR_EQUAL,
   LESS,
   LESS_OR_EQUAL,
   REV_ID_OPERATION_INVALID
}pm_sbl_rev_id_operation_type;



typedef struct
{
   uint8  sid;
   uint8  data;
   uint16 base_address;
   uint16 offset;
   pm_sbl_reg_operation_type reg_operation;
   pm_sbl_rev_id_operation_type rev_id_operation;
   uint32  rev_id;
}pm_sbl_seq_type;


/** 
  @struct pm_pdm_chip_info_type
  @brief Stores the PMIC's chip revision.
 */
typedef struct
{
  uint32         pdmChipRevision;          /**< chip Revision in a format that PDM configuration is generated*/
} pm_pdm_chip_info_type;

typedef struct pm_settings_test_flag_reg_info_type
{
  pm_register_address_type         flag_register;          /* flag register utilized to trigger settings validation test code functions*/
} pm_settings_test_flag_reg_info_type;


/*=============================================================================

                              FUNCTION PROTOTYPES

=============================================================================*/

/*Generic function that reads SBL Config data and PBS RAM data from DAL Config and writes it to PMIC */
pm_err_flag_type pm_sbl_config(void);
pm_err_flag_type pm_sbl_config_test(void);
pm_err_flag_type pm_pbs_ram_version_validation_test(void);

pm_err_flag_type pm_resolve_chip_revision(uint32 slave_id, boolean* valid_slave_id, uint32* pmic_revision );


#endif // __PMIC_SBL_CONFIG_H__

