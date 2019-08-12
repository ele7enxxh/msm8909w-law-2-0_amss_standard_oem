#ifndef BOOT_EXTERN_BUS_INTERFACE_H
#define BOOT_EXTERN_BUS_INTERFACE_H
/*===========================================================================

                    BOOT EXTERN BUS DRIVER DEFINITIONS

DESCRIPTION
  Contains wrapper definition for external bus drivers

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
    
$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/boot/secboot3/src/boot_extern_bus_interface.h#1 $
$DateTime: 2015/09/01 00:30:35 $
$Author: pwbldsvc $
    
when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/11/13   jz      Added boot_ICB_Remap
10/09/12   dh      Added boot_ICB_Segments_Init
09/28/12   kedara  Added boot_SpmiCfg_Init.
07/16/12   dh      Add spmi api
06/18/12   dh      Initial Creation.

===========================================================================*/

/*==========================================================================

                               INCLUDE FILES

===========================================================================*/
#include "icbcfg.h"
#include "SpmiBus.h"
#include "SpmiCfg.h"
#include "ddr_common.h"

/*===========================================================================
                      FUNCTION DECLARATIONS
===========================================================================*/ 


/*===========================================================================

**  Function :  boot_ICB_Config_Init

** ==========================================================================
*/
/*!
*
* @brief 
      Initializes ICB config driver. It reads ICB configuration data (register 
      address and config data) from DAL Device Property file and configures 
      ICB registers.
 
* @param[in]  dev  DAL string device ID.

* @return    
      icbcfg_error_type indicating success or error.

* @dependencies
      None.
 
* @sideeffects 
      None. 
*/
#ifdef FEATURE_BOOT_EXTERN_BUS_COMPLETED
  icbcfg_error_type boot_ICB_Config_Init (const char * dev);
#else
  static inline icbcfg_error_type boot_ICB_Config_Init (const char * dev)
  {
    return ICBCFG_SUCCESS;
  }
#endif


/*===========================================================================

**  Function :  boot_ICB_Segments_Init

** ==========================================================================
*/
/*!
*
	@brief 
		Configures DDR slave segments in BIMC. It reads ICB configuration data (number
		of channels, maximum memory map, etc) from DAL Device Property file and configures 
		ICB registers.
	
	@param[in]  dev     DAL string device ID.
	@param[in]  config  DDR channel region configuration
	
	@return    
		icbcfg_error_type indicating success or error.
	
	@dependencies
		None.
	
	@sideeffects 
		None.  
*/
#ifdef FEATURE_BOOT_EXTERN_BUS_COMPLETED
  icbcfg_error_type boot_ICB_Segments_Init( const char * dev, ddr_info * info );
#else
  static inline icbcfg_error_type boot_ICB_Segments_Init( const char * dev, ddr_info * info )
  {
    return ICBCFG_SUCCESS;
  }
#endif


/*===========================================================================

**  Function :  boot_ICB_Remap

** ==========================================================================
*/
/*!
*
@brief 
      Configures the remap segments in BIMC. 

@param[in] dev  DAL string device ID.
@param[in] info The information about the DDR configuration
@param[in] remap The information about the region to remap

@return    
      icbcfg_error_type indicating success or error.

@dependencies
      None.
 
@sideeffects 
      None. 
*/ 
#ifdef FEATURE_BOOT_EXTERN_BUS_COMPLETED
icbcfg_error_type boot_ICB_Remap( const char * dev, ddr_info * info, icbcfg_remap_info_type * remap );
#else
static inline icbcfg_error_type boot_ICB_Remap( const char * dev, ddr_info * info, icbcfg_remap_info_type * remap )
{
  return ICBCFG_ERROR;
}
#endif


/*===========================================================================

**  Function :  boot_SpmiBus_Init

** ==========================================================================
*/
/*!
@brief Initialize the spmi driver 

This function initializes the Spmi Bus driver. It is mandatory to call this 
function before using any other APIs of this driver

@return  SPMI_BUS_SUCCESS on success, error code on error

@see SpmiBus_DeInit
*/

#ifdef FEATURE_BOOT_EXTERN_BUS_COMPLETED
  SpmiBus_ResultType boot_SpmiBus_Init(void);
#else
  static inline SpmiBus_ResultType boot_SpmiBus_Init()
  {
    return SPMI_BUS_SUCCESS;
  }
#endif

/**
@brief Reads from a SPMI slave device 

This function reads data from SPMI Slave device. The register address 
is in long(16 bit) format

@param[in] uSlaveId  Slave Id of the device

@param[in] Priority with which the command needs to be sent on the SPMI bus

@param[in] uRegisterAddress  Register Address on the SPMI 
slave device from which read is to be initiated. 16 LSB bit of this
parameter are used as Register address

@param[in] pucData  Pointer to data array. Data read from the 
SPMI bus will be filled in this array

@param[in] uDataLen  Number of bytes to read

@param[out] puTotalBytesRead Pointer to an uint32 which is 
used to return the total number of bytes read from SPMI
device

@return  SPMI_BUS_SUCCESS on success, error code on error

@see SpmiBus_WriteLong
*/
#ifdef FEATURE_BOOT_EXTERN_BUS_COMPLETED
SpmiBus_ResultType  boot_SpmiBus_ReadLong(uint32 uSlaveId,
                                     SpmiBus_AccessPriorityType eAccessPriority,
                                     uint32 uRegisterAddress, 
                                     uint8 *pucData,
                                     uint32 uDataLen, 
                                     uint32 *puTotalBytesRead);
#else
static inline SpmiBus_ResultType  boot_SpmiBus_ReadLong(uint32 uSlaveId,
                                     SpmiBus_AccessPriorityType eAccessPriority,
                                     uint32 uRegisterAddress, 
                                     uint8 *pucData,
                                     uint32 uDataLen, 
                                     uint32 *puTotalBytesRead)
{
    return SPMI_BUS_SUCCESS;
}
#endif

/**
@brief Writes to a SPMI slave device 

This function writes data to SPMI Slave device. The register address 
is in long(16 bit) format

@param[in] uSlaveId  Slave Id of the device

@param[in] Priority with which the command needs to be sent on the SPMI bus

@param[in] uRegisterAddress  Register Address on the SPMI 
slave device to which write is to be initiated. 16 LSB bit of this
parameter are used as Register address

@param[in] pucData  Pointer to data array containing data to 
be written on the bus.

@param[in] uDataLen  Number of bytes to write 

@return  SPMI_BUS_SUCCESS on success, error code on error

@see SpmiBus_ReadLong
*/
#ifdef FEATURE_BOOT_EXTERN_BUS_COMPLETED
SpmiBus_ResultType  boot_SpmiBus_WriteLong(uint32 uSlaveId,
                                      SpmiBus_AccessPriorityType eAccessPriority,
                                      uint32 uRegisterAddress,
                                      uint8 *pucData, 
                                      uint32 uDataLen);
#else
static inline SpmiBus_ResultType  boot_SpmiBus_WriteLong(uint32 uSlaveId,
                                      SpmiBus_AccessPriorityType eAccessPriority,
                                      uint32 uRegisterAddress,
                                      uint8 *pucData, 
                                      uint32 uDataLen)
{
    return SPMI_BUS_SUCCESS;
}
#endif


/**
@brief Initialize the driver 

This function initializes the Spmi Configuration driver. It is mandatory 
to call this function before using any other APIs of this driver

@param[in] bUpdateFirmware  TRUE if Firmware update is needed
                            FALSE otherwise

@return  SPMI_CFG_SUCCESS on success, error code on error

@see SpmiCfg_DeInit
*/
#ifdef FEATURE_BOOT_EXTERN_BUS_COMPLETED
SpmiCfg_ResultType boot_SpmiCfg_Init(uint32 bUpdateFirmware);
#else
static inline SpmiCfg_ResultType boot_SpmiCfg_Init(uint32 bUpdateFirmware)
{
  return SPMI_CFG_SUCCESS;
}
#endif

#endif /* BOOT_EXTERN_BUS_INTERFACE_H */
