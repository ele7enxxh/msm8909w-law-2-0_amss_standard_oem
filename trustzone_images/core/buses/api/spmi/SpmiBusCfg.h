/**
 * @file:  SpmiBusCfg.h
 * @brief: This module provides configuration options for a SPMI controller
 * 
 * Copyright (c) 2013-2015 by Qualcomm Technologies Incorporated. All Rights Reserved.
 * 
 * $DateTime: 2016/06/17 14:31:11 $
 * $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/buses/api/spmi/SpmiBusCfg.h#1 $
 * $Change: 10727476 $ 
 * 
 *                              Edit History
 * Date     Description
 * -------  -------------------------------------------------------------------
 * 1/19/15  Initial Version
 */
#ifndef SPMIBUSCFG_H
#define	SPMIBUSCFG_H

#include "SpmiTypes.h"

//******************************************************************************
// Constants
//******************************************************************************

#define SPMI_OPEN_OWNER 0xFF

//******************************************************************************
// Data Structures
//******************************************************************************

typedef struct
{
    uint8 slaveId;     /**< Slave id of the SPMI device */
    uint8 periphId;    /**< Peripheral id on the slave device */
    uint16 channel;    /**< The channel number assigned to this peripheral (0 indexed) */
    uint8 intOwner;    /**< The EE that will receive this peripheral's interrupt. */
    uint8 periphOwner; /**< The EE that has write access to this peripheral. */
} SpmiBusCfg_ChannelCfg;

typedef struct
{
    uint8 slaveId;  /**< Slave id of the SPMI device */
    uint16 address; /**< Address (Periph id + offset) in the device */
} SpmiBusCfg_Ppid;

typedef struct 
{
    uint8 pvcPortId;              /**< PVC Port id */
    Spmi_AccessPriority priority; /**< SPMI bus command priority */
    SpmiBusCfg_Ppid* ppids;          /**< Peripheral access table */
    uint32 numPpids;              /**< Number of addresses in the ppids array */
} SpmiBusCfg_PvcPortCfg;

typedef struct 
{
    uint8 owner;
    uint32 startAddr;
    uint32 size;
    uint8 startIdx;
    uint8 endIdx;
} SpmiBusCfg_RGConfig;

//******************************************************************************
// Public API Functions
//******************************************************************************

/**
 * @brief Initialize the driver 
 * 
 * This function initializes the Spmi Configuration driver. It is mandatory 
 * to call this function before using any other APIs of this driver
 * 
 * @param[in] initHw  True to initialize static hw configuration
 * 
 * @return  SPMI_SUCCESS on success, error code on error
 * 
 * @see SpmiBusCfg_DeInit
 */
Spmi_Result SpmiBusCfg_Init(boolean initHw);

/**
 * @brief De-initialize the driver 
 * 
 * This function de-initializes the Spmi Configuration driver.
 * 
 * @return  SPMI_SUCCESS on success, error code on error
 * 
 * @see SpmiBusCfg_Init
 */
Spmi_Result SpmiBusCfg_DeInit(void);

/**
 * @brief Gets the ppid and owner information for a SW Channel
 * 
 * This function gets the ppid and owner information of a channel
 * 
 * @param[in] busId  The bus to query
 * @param[in] channel  The SW Channel to query
 * @param[out] slaveId   Slave id of the device mapped to the channel
 * @param[out] periphId  Peripheral id of the device mapped to the channel
 * @param[out] owner     The channel's interrupt owner
 * 
 * @return  SPMI_SUCCESS on success, error code on error
 * 
 * @see SpmiBusCfg_GetPeripherialInfo
 */
Spmi_Result SpmiBusCfg_GetChannelInfo(uint8 busId, uint16 channel, uint8* slaveId, uint8* periphId, uint8* owner);

/**
 * @brief Gets the channel and owner information of a peripheral
 * 
 * This function gets the channel and owner information of a peripheral
 * 
 * @param[in] busId     The bus to query
 * @param[in] slaveId   Slave id of the device
 * @param[in] periphId  Peripheral id
 * @param[out] channel  The peripheral's SW Channel
 * @param[out] owner    The peripheral's interrupt owner
 * 
 * @return  SPMI_SUCCESS on success, error code on error
 * 
 * @see SpmiBusCfg_GetChannelInfo
 */
Spmi_Result SpmiBusCfg_GetPeripherialInfo(uint8 busId, uint8 slaveId, uint8 periphId, uint16* channel, uint8* owner);

/**
 * @brief Updates the current channel configuration mode
 * 
 * This function enables or disables the configuration of channels
 * on-the-fly.  If enabled, resources will be configured as peripherals
 * are accessed.  If disabled, a static list of peripherals needs to be 
 * configured via SpmiBusCfg_ConfigurePeripherals.
 *
 * @param[in] busId     The bus to query
 * @param[in] enabled   True, to enable dynamic configuration mode
 * 
 * @return  SPMI_SUCCESS on success, error code on error
 * @see SpmiBusCfg_ConfigurePeripherals
 */
Spmi_Result SpmiBusCfg_SetDynamicChannelMode(uint8 busId, boolean enabled);

/**
 * @brief Maps SW channels to a peripherals and sets interrupt ownership on bus 0
 * @deprecated Use SpmiBusCfg_ConfigurePeripherals instead
 * 
 * This function will map logical SW Channels to the peripherals and set ownership.  
 * The mapping will exist for both the owner and the observer SW channels.
 * 
 * @Note Each owner's channels should be consecutive so they can be grouped together
 *       during SMMU/XPU configuration.
 * 
 * @param[in] entries  An array containing the mapping information.
 * @param[in] numEntries  Number of entries in the array
 * 
 * @return  SPMI_SUCCESS on success, error code on error
 * 
 * @see SpmiBusCfg_ConfigurePeripherals
 */
Spmi_Result SpmiBusCfg_ConfigureChannels(SpmiBusCfg_ChannelCfg* entries, uint32 numEntries);

/**
 * @brief Maps SW channels to peripherals and sets interrupt ownership.
 * 
 * This function will map peripherals to SW Channels and set related ownership information.  
 * It will override any existing configuration.
 * 
 * @param[in] busId       The bus to configure
 * @param[in] entries     An array containing the mapping information.
 * @param[in] numEntries  Number of entries in the array
 * @param[out] rgCfg      An array of Resource Groups that will be populated.
 *                        Can be NULL if this info isn't wanted.
 * @param[in/out] rgCount [in]  Pointer to number of Resource Groups available
 *                        [out] Pointer to number of Resource Groups used
 *                        Can be NULL if rgCfg is NULL.
 * 
 * @return  SPMI_SUCCESS on success, error code on error
 * @see SpmiBusCfg_SetDynamicChannelMode
 */
Spmi_Result SpmiBusCfg_ConfigurePeripherals(uint8 busId, SpmiBusCfg_ChannelCfg* entries, uint32 numEntries, 
                                            SpmiBusCfg_RGConfig* rgCfg, uint32* rgCount);

/**
 * @brief Calculates resource group configuration
 * 
 * This function will perform identically to SpmiBusCfg_ConfigurePeripherals, given the
 * same input, but without modifying any HW configuration.
 * 
 * @param[in] entries     An array containing the mapping information.
 * @param[in] numEntries  Number of entries in the array
 * @param[out] rgCfg      An array of Resource Groups that will be populated.
 *                        Can be NULL if this info isn't wanted.
 * @param[in/out] rgCount [in]  Pointer to number of Resource Groups available
 *                        [out] Pointer to number of Resource Groups used
 *                        Can be NULL if rgCfg is NULL.
 * 
 * @return  SPMI_SUCCESS on success, error code on error
 * @see SpmiBusCfg_ConfigurePeripherals
 */
Spmi_Result SpmiBusCfg_CalculateRGConfig(uint8 busId, SpmiBusCfg_ChannelCfg* entries, uint32 numEntries, 
                                         SpmiBusCfg_RGConfig* rgCfg, uint32* rgCount);

/**
 * @brief Configures the PVC ports of the PMIC Arbiter 
 * 
 * This function configures the PMIC Arbiter for various PVC 
 * ports. It also enables the listed PVC ports. 
 * 
 * @param[in] busId    The bus to configure
 * @param[in] portCfg  The port configuration information
 * 
 * @return  SPMI_SUCCESS on success, error code on failure
 */
Spmi_Result SpmiBusCfg_ConfigurePvcPort(uint8 busId, const SpmiBusCfg_PvcPortCfg* portCfg);

/**
 * @brief Configures the ports of the PMIC Arbiter
 * 
 * The PMIC Arbiter has 1 SW port, n number of PVC ports, and possibly other
 * ports for things such as debug.  Each of these ports contend with each other
 * for access to the SPMI bus.  This function will set the priority of these 
 * ports relative to each other.
 * 
 * @param[in] busId  The bus to configure
 * @param[in] ports  Array containing the priority info.  The priority 
 *                   is used as the array index and the highest priority is 0.
 *                   e.g. ports[0] = 2 (port 2 has the highest priority)
 *                        ports[1] = 5 (port 5 has the next highest priority)
 * @param[in] numPorts  Number of ports in the array
 * 
 * @return  SPMI_SUCCESS on success, error code on failure
 */
Spmi_Result SpmiBusCfg_SetPortPriorities(uint8 busId, uint8* ports, uint32 numPorts);

/**
 * @brief Enables or disables all PVC ports
 * 
 * This function will enable or disable the HW PVC ports.  Called after
 * the ports have been configured.
 * 
 * @param[in] busId   The bus to configure
 * @param[in] enable  TRUE to enable, FALSE to disable
 * 
 * @return  SPMI_SUCCESS on success, error code on failure
 * 
 * @see SpmiBusCfg_ConfigurePvcPort
 */
Spmi_Result SpmiBusCfg_SetPVCPortsEnabled(uint8 busId, boolean enable);

#endif
