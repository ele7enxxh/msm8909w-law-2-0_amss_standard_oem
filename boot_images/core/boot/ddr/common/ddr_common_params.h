#ifndef __DDR_COMMON_PARAMS_H__
#define __DDR_COMMON_PARAMS_H__

/*=============================================================================

                                DDR COMMON PARAMETERS
                                Header File
GENERAL DESCRIPTION
This file defines the DDR parameters header structure and API. It allows user to pass
their own DDR parameters to DDR driver.


Copyright 2013 by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/

/*==============================================================================
                                EDIT HISTORY

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/boot/ddr/common/ddr_common_params.h#1 $
$DateTime: 2015/09/01 00:30:35 $
$Author: pwbldsvc $
================================================================================
when       who     what, where, why
--------   ---     -------------------------------------------------------------
12/18/14   xl      Add ddr_set_params_enhance
09/05/13   sr      New functions added to supoort DDR training in SBL1
08/27/13   sr      Initial version.
==============================================================================*/
/*==========================================================================

                               INCLUDE FILES
===========================================================================*/
#include "ddr_common.h"
#include "comdef.h"

/* ============================================================================
**  Function : ddr_set_params
** ============================================================================
*/
/**
*   @brief
*   This function sets the DDR driver's device table to an external device table.
*   It will also update the interleaving mode information according to the parameter
*   header that's passed in.
*   User should append ddr_device_params_header_v1 header in front of their own DDR device 
*   table and pass  a pointer points to the beginning of this header via the this API to 
*   force DDR driver to utilize the user defined parameters.
*   The user defined ddr device table must contain a minimal number of entries to 
*   match the system's ddr devices.
*   For example if a system has two ddr devices attached then the external table must 
*   define at least two entries to match those two devices.
*   However, more entries can be added to the end of table to support other ddr devices
*   with different parameters.
*
*   @param[in]  ddr_params_ptr   Pointer to the external ddr parameters. It should
*                                point to the user defined ddr device table with ddr
*                                parameters header appended in front. This pointer must
*                                points to a 4 bytes aligned address.
*                         
*
*   @return
*   TRUE if DDR device table is updated, FALSE if an error happens and this operation
*   is aborted
*
*   @dependencies
*   Must be called prior to ddr_initialize_device. 
*   This API will take no effect after ddr_initialize_device
*
*   @sa
*   None
*
*   @sa
*   None
*/
boolean ddr_set_params(void *ddr_params_ptr, uint32 ddr_params_size);


/* ============================================================================
**  Function : ddr_set_params_enhance,enhance set
** ============================================================================
*/
/**
*   @brief
*   This function sets the DDR DC timing parameters stored in CDB2 and CDB3.enhance set.
*   For CDB2,this function will directly set the registers of DDR driver .
*   For CDB3,this function will put the data in IMEM for RPM setting.
*   @param[in]  ddr_params_ptr   pointer to the base address of CDB2 or CDB3 in memory
*                
*
*   @return
*   TRUE if the parameters was successfully set.
*
*   @dependencies
*  None
*
*   @sideeffects
*   None
*
*   @sa
*   None
*/
boolean ddr_set_params_enhance(void *ddr_params_ptr, uint32 ddr_params_size);


/* ============================================================================
**  Function : ddr_get_params
** ============================================================================
*/
/**
*   @brief
*   Returns DDR parameters.
*
*   @param[in]  interface_name    Interface to get DDR parameters for
*
*   @return
*   DDR parameters
*
*   @dependencies
*   None
*
*   @sa
*   None
*
*   @sa
*   None
*/
union ddr_device_params *ddr_get_params
(
  SDRAM_INTERFACE interface_name
);

/* =============================================================================
**  Function : ddr_params_detection
** =============================================================================
*/
/**
*   @brief
*   Detect correct DDR device parameters based on manufacture and dimension
*   information.
*
*   @param[in]  interface_name  Interface to detect for
*
*   @retval  TRUE   Parameters detected
*   @retval  FALSE  Parameters not dectected
*
*   @dependencies
*   None
*
*   @sideeffects
*   None
*
*   @sa
*   None
*/
boolean ddr_params_detection(SDRAM_INTERFACE interface_name);

/* ============================================================================
**  Function : ddr_param_interleaved
** ============================================================================
*/
/**
*   @brief
*   Return the interleave setting based on ddr parameters
*
*   @return
*   return a bitmask of interleave status
*
*   @dependencies
*   Must be called after ddr_set_params. Calling it before ddr_set_params will
*   return the default setting which is TRUE.
*
*   @sa
*   None
*
*   @sa
*   None
*/
uint32 ddr_param_interleaved(void);

/* ============================================================================
**  Function : ddr_params_set_shared_memory
** ============================================================================
*/
/**
*   @brief
*   Copies the detected ddr parameter to shared memory region
*
*   @return
*   None
*
*   @dependencies
*   None
*
*   @sa
*   None
*
*   @sa
*   None
*/
void ddr_params_set_shared_memory( void );

/* =============================================================================
**  Function : ddr_params_get_partition
** =============================================================================
*/
/**
*   @brief
*   Get DDR parameter partition: partition base is returned, and partition size
*   is updated through pointer.
*
*   @param[out]  size  DDR parameter partition size
*
*   @return
*   DDR parameter partition base
*
*   @dependencies
*   None
*
*   @sideeffects
*   None
*
*   @sa
*   None
*/
void *ddr_params_get_training_data(uint32 *size);

/* =============================================================================
**  Function : ddr_params_set_partition
** =============================================================================
*/
/**
*   @brief
*   Set DDR parameter partition.
*
*   @param[in]  base  DDR parameter partition base
*   @param[in]  size  DDR parameter partition size
*
*   @retval  TRUE   Partition set successfully
*   @retval  FALSE  Partition set unsuccessfully
*
*   @dependencies
*   None
*
*   @sideeffects
*   None
*
*   @sa
*   None
*/
boolean ddr_params_set_training_data(void *base, uint32 size);

/* =============================================================================
**  Function : ddr_params_is_training_required
** =============================================================================
*/
/**
*   @brief
*   Indicate whether DDR parameter training is required or not. Training is
*   required if and only if DDR itself (e.g. PCDDR3) requires parameter training
*   and DDR parameter partition is invalid.
*
*   @param  None
*
*   @retval  TRUE   Training required
*   @retval  FALSE  Training not required
*
*   @dependencies
*   None
*
*   @sideeffects
*   None
*
*   @sa
*   None
*/
boolean ddr_params_is_training_required( void );

/* =============================================================================
**  Function : ddr_get_trained_params
** =============================================================================
*/
/**
*   @brief
*   Get DDR trained parameters.
*
*   @param[in]  interface_name  Interface to get for
*
*   @return
*   DDR trained parameters
*
*   @dependencies
*   None
*
*   @sideeffects
*   None
*
*   @sa
*   None
*/
union ddr_trained_params *ddr_get_trained_params(SDRAM_INTERFACE interface_name);

/* =============================================================================
**  Function : ddr_get_training_cookie
** =============================================================================
*/
/**
*   @brief
*   Get DDR training magic cookie.
*
*   @param
*   None
*
*   @return
*   DDR training cookie, 0 if no valid training partition is found
*
*   @dependencies
*   None
*
*   @sideeffects
*   None
*
*   @sa
*   None
*/
uint32 ddr_get_training_cookie(void);

/* =============================================================================
**  Function : ddr_set_training_cookie
** =============================================================================
*/
/**
*   @brief
*   Set DDR training magic cookie.
*
*   @param
*   None
*
*   @return
*   boolean - operation successful or fail
*
*   @dependencies
*   None
*
*   @sideeffects
*   None
*
*   @sa
*   None
*/
boolean ddr_set_training_cookie(uint32 cookie);

/* =============================================================================
**  Function : ddr_params_is_training_on_rpm_required
** =============================================================================
*/
/**
*   @brief
*   Indicate whether DDR parameter training is required or not in RPM. Training is
*   required if and only if DDR itself (e.g. PCDDR3) requires parameter training
*   and DDR parameter partition is invalid. 
*
*   @param  None
*
*   @retval  TRUE   Training required
*   @retval  FALSE  Training not required
*
*   @dependencies
*   None
*
*   @sideeffects
*   None
*
*   @sa
*   None
*/
boolean ddr_params_is_training_on_rpm_required( void );
 /* ddr_params_is_training_on_rpm_required */

/* =============================================================================
**  Function : ddr_do_phy_training
** =============================================================================
*/
/**
*   @brief
*   function called to do the DDR PHY training 
*
*   @param  None
*
*   @retval  NONE
*
*   @dependencies
*   None
*
*   @sideeffects
*   None
*
*   @sa
*   None
*/
void ddr_do_phy_training( void );
#endif /* __DDR_PARAMS_H__ */
