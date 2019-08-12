#ifndef HALXPU2_H
#define HALXPU2_H
/**

@file   HALxpu2.h

@brief  This module provides the Application Programing Interface (API) 
        to the XPU2 HAL layer for Modem.
*/
/*
===========================================================================

FILE:         HALxpu2.h

DESCRIPTION:  Header file for the HAL XPU2 interface for Modem.

===========================================================================

                             Edit History

$Header: //components/rel/core.mpss/3.5.c12.3/kernel/xpu2/hal/inc/HALxpu2.h#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
10/29/13   sc      Supported retrieving partition enable info
02/11/13   sc      Added RO access granularity to RGn_MCR
01/08/13   sc      Updated HAL_xpu2_EnableMPUPartition
12/11/12   sc      Added read-after-write support
10/30/12   sc      Added enable/disable MPU partitions APIs
07/30/12   sc      Added config support for APU/RPU
06/27/12   sc      Added config support for client r/w permissions
05/30/12   sc      Updated for Elan
03/26/12   sc      Added support to unlock a partition
12/09/11   sc      Initial version for xPU2 modem
10/26/11   sc      Initial creation for xPU2
12/20/10   sds     Add CFPB Master and SMMU SFPB default slaves
10/04/10   sds     Add GCC and TLMM xPUs
06/21/10   sds     Updates from code review
03/12/10   sds     Added device type for default slave on devices
02/03/10   sds     Integrate from 2H09 branch after 8650A target testing
01/29/10   sds     Update function comments
01/15/10   sds     Move CFPB1/2 back to match existing HAL API, add
                   CFPB0. BB needs CFPB2, so it's not a wasted slot.
01/12/10   sds     Add features for Phantom/BB XPUs
                   (Default slave ACR, additional device properties)
11/06/09   sds     Fix compile warnings
09/17/09   sds     Changed to doxygen style function commenting
08/25/09   sds     Resource group related function name typos
                   Change to resource group permission struct
08/21/09   sds     Added GetErrorReportingConfig()
02/03/09   sds     Created 

===========================================================================
Copyright (c) 2009-2013
Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================
*/

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* -----------------------------------------------------------------------
** Includes
** ----------------------------------------------------------------------- */
#include "HALxpu2DataTypes.h"
#include "HALxpu2Target.h"

/* -----------------------------------------------------------------------
** Definitions
** ----------------------------------------------------------------------- */
/** Use by HAL_xpu2_UnlockMSAResourceGroup to unlock all resource groups */
#define HAL_XPU2_ALL_MSA_RES_GROUP   0xFFFFFFFF

/** XPU2 Permissions */
#define HAL_XPU2_NO_ACCESS    0x0UL /** no access */
#define HAL_XPU2_RO_ACCESS    0x1UL /** read-only access */
#define HAL_XPU2_WO_ACCESS    0x2UL /** write-only access */
#define HAL_XPU2_FULL_ACCESS  0x3UL /** full access */

/* -----------------------------------------------------------------------
** Types
** ----------------------------------------------------------------------- */

  /** XPU2 error status enum */
typedef enum
{
  HAL_XPU2_NO_ERROR = 0,       /**< successful operation */
  HAL_XPU2_INVALID_INSTANCE,   /**< out of range xPU2 instance */
  HAL_XPU2_UNSUPPORTED_INSTANCE_FOR_TARGET,
    /**< unsupported xPU2 instance for the current target */
  HAL_XPU2_NON_MSA_OWNED_RESID,
    /**< resource id not owned by MSA */
  HAL_XPU2_INVALID_PARAM,      /**< invalid passed in parameter */
  HAL_XPU2_UNSUPPORTED_FEATURE,/**< unsupported xPU2 core feature */
  HAL_XPU2_READ_WRITE_MISMATCH /**< read-after-write values not match */
} HAL_xpu2_Status;

/** XPU2 type */
typedef enum
{
  HAL_XPU2_BIMC_S_DDR0,   /**< BIMC DDR0 */
  HAL_XPU2_BIMC_S_DDR1,   /**< BIMC DDR1 */
  HAL_XPU2_BOOT_ROM,      /**< BOOT ROM */
  HAL_XPU2_MSS,           /**< Modem APU */
  HAL_XPU2_QPIC_NAND,     /**< QPIC NAND */
  HAL_XPU2_SEC_CTRL_APU,  /**< SEC CTRL APU */
  HAL_XPU2_MPM2,          /**< MPM2 */
  HAL_XPU2_TCSR_REGS,     /**< TCSR */
  HAL_XPU2_BAM_BLSP1_DMA, /**< BAM BLSP1 */
  HAL_XPU2_SNOC_CFG,      /**< SNOC CFG */
  HAL_XPU2_Q6PCNOC_CFG,   /**< Q6 PCNOC CFG*/
  HAL_XPU2_TLMM,          /**< TLMM */
  HAL_XPU2_CLK_CTL,       /**< CLK CTL */
  HAL_XPU2_COUNT,
  HAL_XPU2_SIZE     = 0x7FFFFFFF /* force to 32-bit enum */
} HAL_xpu2_XPU2Type;

/** XPU2 Resource ID Type */
typedef uint32 HAL_xpu2_ResourceGroupIDType;

/** XPU2 Resource Group Addresses */
typedef struct
{
  uint32 uStartLower; /**< resource group lower 32 bits of the start address */
  uint32 uStartUpper; /**< resource group upper 32 bits of the start address */
  uint32 uEndLower;   /**< resource group lower 32 bits of the end address */
  uint32 uEndUpper;   /**< resource group upper 32 bits of the end address */
} HAL_xpu2_ResourceGroupAddrType;

/** VMID related definitions */
/* Maximum allowed VMID value HAL_XPU2_MAX_VMID is defined in HALxpu2Target.h */
#define HAL_XPU2_MASKS_PER_FIELD 32 /**< Number of mask bits per field */
#define HAL_XPU2_PERMS_PER_FIELD 16 /**< Number of access perms per field */

/** number of required VMID mask dwords */
#define HAL_XPU2_MASTER_MASK_WORDS \
        ((HAL_XPU2_MAX_VMID/HAL_XPU2_MASKS_PER_FIELD) + 1)

/** number of required VMID permission dwords */
#define HAL_XPU2_MASTER_PERM_WORDS \
        ((HAL_XPU2_MAX_VMID/HAL_XPU2_PERMS_PER_FIELD) + 1)

/** XPU2 Resource Group Permissions */
typedef struct
{
  bool32 bMultiVMID;          /**< Indicates which union member is valid */
  union
  {
    struct
    {
      uint32 auVMID[HAL_XPU2_MASTER_MASK_WORDS];     /**< VMID to mask */
      uint32 auVMIDPerm[HAL_XPU2_MASTER_PERM_WORDS]; /**< VMID permissions */
    } multi; /**< Union member for multiple-VMID configuration */
    struct
    {
      boolean bRWGlobalEn : 1;  /**< Full access global enable */
      boolean bRWVMIDValid : 1; /**< Full access VMID valid? */
      uint32 uRWVMID;           /**< VMID to be assigned full access */
      boolean bROGlobalEn : 1;  /**< Read-only access global enable */
      boolean bROVMIDValid : 1; /**< Read-only access VMID valid? */
      uint32 uROVMID;           /**< VMID to be assigned read-only access */
    } single; /**< Union member for single-VMID configuration */
  } Vmid_Type;
} HAL_xpu2_ResourceGroupPermissionType;

/** XPU2 Resource Group client access permissions */
typedef struct
{
  boolean bMSAClientROEn : 1;
    /**< enable MSA client read-only access? (for V2.4 or later) */
  boolean bNonSecureClientROEn : 1;
    /**< enable non-secure client read-only access? (for V2.1 or later) */
  boolean bSecureClientROEn : 1;
    /**< enable secure client read-only access? (for V2.1 or later) */
  boolean bNonSecureClientRWEn : 1;
    /**< enable non-secure client read/write access? */
  boolean bSecureClientRWEn : 1;
    /**< enable secure client read/write access? */
} HAL_xpu2_ResourceGroupClientPermType;

/** MPU partition enable enum */
typedef enum
{
  HAL_XPU2_MPU_PARTITION_ENABLE = 0,
    /**< Partition is valid and available for use. */
  HAL_XPU2_MPU_PARTITION_DISABLE,
    /**< Partition is disabled and content of xPU_RGn_* has no effect
     *   on access permission determination. */
  HAL_XPU2_MPU_PARTITION_DNE  = 0x7FFFFFFE,
    /**< Partition enable bit does not exist. */
  HAL_XPU2_MPU_PARTITION_SIZE = 0x7FFFFFFF /* force to 32-bit enum */
} HAL_xpu2_ResourceGroupMPUEnableType;

/** XPU2 Resource Group config info */
typedef struct
{
  HAL_xpu2_ResourceGroupIDType uResId; /**< resource group ID */
  HAL_xpu2_ResourceGroupAddrType resAddr;
    /**< resource group address range for MPU device type */
  HAL_xpu2_ResourceGroupPermissionType resPerm;
    /**< resource group vmid permissions */
  HAL_xpu2_ResourceGroupClientPermType clientPerm;
    /**< resource group client access permissions */
  HAL_xpu2_ResourceGroupMPUEnableType eMPURGEnableInfo;
    /**< MPU partition enabled info (for V2.1 or later) */
} HAL_xpu2_ResourceGroupConfigType;

/** Error reporting options */
typedef enum
{
  HAL_XPU2_ERROR_O_DECODE_EN     = (1 << 0),
    /**< Record decode errors */
  HAL_XPU2_ERROR_O_INTERRUPT_EN  = (1 << 1),
    /**< Generate interrupt on errors */
  HAL_XPU2_ERROR_O_CLIENT_RPT_EN = (1 << 2),
    /**< Report client errors to master */
  HAL_XPU2_ERROR_O_CFG_RPT_EN    = (1 << 3),
    /**< Report config errors to master */
  HAL_XPU2_ERROR_O_CLIENT_INT_EN = (1 << 4),
    /**< Generate client port interrupt on errors (for V2.1 or later) */
  HAL_XPU2_ERROR_O_CFG_INT_EN    = (1 << 5),
    /**< Generate config port interrupt on errors (for V2.1 or later) */
  HAL_XPU2_ERROR_O_SIZE   = 0x7FFFFFFF /* force to 32-bit enum*/
} HAL_xpu2_ErrorOptionsType;

/** Store error options ORed in this */
typedef uint32 HAL_xpu2_ErrorOptionsConfigType;

/** XPU2 Error flags */
typedef enum
{
  HAL_XPU2_ERROR_F_CONFIG_PORT = (1 << 0),  /**< Configuration port error */
  HAL_XPU2_ERROR_F_CLIENT_PORT = (1 << 1),  /**< Client port error */
  HAL_XPU2_ERROR_F_MULTIPLE    = (1 << 2),  /**< Multiple errors */
  HAL_XPU2_ERROR_F_MULTI_CONFIG_PORT = (1 << 3),
    /**< For revision 2.1 or later: Multiple configuration port errors */
  HAL_XPU2_ERROR_F_MULTI_CLIENT_PORT = (1 << 4),
    /**< For revision 2.1 or later: Multiple client port errors */
  HAL_XPU2_ERROR_FLAGS_SIZE    = 0x7FFFFFFF /* force to 32-bit enum*/
} HAL_xpu2_ErrorFlagsType;

/** Store error flags ORed in this */
typedef uint32 HAL_xpu2_ErrorFlagsConfigType;

/** Bus related error flags */
typedef enum
{
  HAL_XPU2_BUS_F_ERROR_AC     = (1 << 0),
    /**< Bus reports access control error */
  HAL_XPU2_BUS_F_ERROR_DCD    = (1 << 1),  /**< Bus reports decode error */
  HAL_XPU2_BUS_F_ASHARED      = (1 << 2),  /**< ASHARED Field */
  HAL_XPU2_BUS_F_AINNERSHARED = (1 << 3),  /**< AINNERSHARED Field */
  HAL_XPU2_BUS_F_APRIV        = (1 << 4),  /**< APRIV Field */
  HAL_XPU2_BUS_F_APROTNS      = (1 << 5),  /**< APROTNS Field */
  HAL_XPU2_BUS_F_AINST        = (1 << 6),  /**< AINST Field */
  HAL_XPU2_BUS_F_AWRITE       = (1 << 7),  /**< AWRITE Field */
  HAL_XPU2_BUS_F_AOOO         = (1 << 8),  /**< AOOO Field */
  HAL_XPU2_BUS_F_AFULL        = (1 << 9),  /**< AFULL Field */
  HAL_XPU2_BUS_F_ABURST       = (1 << 10), /**< ABURST Field */
  HAL_XPU2_BUS_F_BURSTLEN     = (1 << 11), /**< Client burst length error */
  HAL_XPU2_BUS_F_ARDALLOCATE  = (1 << 12), /**< ARDALLOCATE field */
  HAL_XPU2_BUS_F_AEXCLUSIVE   = (1 << 13), /**< AEXCLUSIVE field */
  HAL_XPU2_BUS_F_ARDBEADNDXEN = (1 << 14), /**< ARDBEADNDXEN field */
  HAL_XPU2_BUS_F_AMSSSELFAUTH = (1 << 15), /**< AMSSSELFAUTH field */
  HAL_XPU2_BUS_F_SAVERESTORE_IN_PROG = (1 << 16),
    /**< save/restore in progress during error */
  HAL_XPU2_BUS_F_MSA_RG_MATCH = (1 << 17), /**< MSA resource match error */
  HAL_XPU2_BUS_F_SECURE_RG_MATCH = (1 << 18),
    /**< Secure resource match error */
  HAL_XPU2_BUS_F_NONSECURE_RG_MATCH = (1 << 19),
    /**< Non-Secure resource match error */
  HAL_XPU2_BUS_FLAGS_SIZE     = 0x7FFFFFFF /* force to 32-bit enum*/
} HAL_xpu2_BusErrorFlagsType;

/** Store bus flags ORed in this */
typedef uint32 HAL_xpu2_BusErrorFlagsMaskType;

/** Full xPU2 error reporting structure */
typedef struct
{
  HAL_xpu2_ErrorFlagsConfigType uErrorFlags; /**< Error Flags */
  HAL_xpu2_BusErrorFlagsMaskType uBusFlags;  /**< Bus specific flags */
  uint32 uPhysicalAddressLower32;
    /**< The lower 32 bits of the physical address of the errant request */
  uint32 uPhysicalAddressUpper32;
    /**< The upper 32 bits of the physical address of the errant request */
  uint32 uMasterId;       /**< The master ID of the offending master */
  uint32 uAVMID;          /**< The virtual master ID of the offending master */
  uint32 uATID;           /**< The ATID field of the errant request */
  uint32 uABID;           /**< The ABID field of the errant request */
  uint32 uAPID;           /**< The APID field of the errant request */
  uint32 uALen;           /**< The ALEN field of the errant request */
  uint32 uASize;          /**< The ASIZE field of the errant request */
  uint32 uAMemType;       /**< The AMEMTYPE field of the errant request */
  uint32 uAPReqPriority;  /**< The APREQPRIORITY field of the errant request */
} HAL_xpu2_ErrorType;

/* -----------------------------------------------------------------------
** Configuration Interface
** ----------------------------------------------------------------------- */
/** @name Configuration Interface */
/** @{ */

/**
@brief Configure MSA resource group with the specified partition ID.

   This function configures the start and end addresses, client access
   permissions of the MSA partition, partition read/write access control, and
   partition enable info with the specified partition ID.  The partition enable
   info is supported by MPU on revision 2.1 and onward.

@param[in] eXpu2 - The XPU2 to set up a resource group in.
@param[in] pMSAResConfigInfo - A pointer to the config info structure to use
                               for the MSA resource group to be configured.

@return The error status of the xPU2.

@sa HAL_xpu2_EnableMPUPartition()
*/
HAL_xpu2_Status HAL_xpu2_ConfigMSAResourceGroup
  ( HAL_xpu2_XPU2Type                       eXpu2,
    const HAL_xpu2_ResourceGroupConfigType *pMSAResConfigInfo );

/**
@brief Enable MPU partition

   This function enables the MPU partition with the specified resource group
   to be used in client permission determination.  This functionality is
   supported in revision 2.1 or later.  In case the revision is prior to 2.1,
   \c pResAddr is used to enable the partition start/end addresses.

@param[in] eXpu2 - The XPU2 to enable.
@param[in] uResId - The resource group to set up.
@param[in] pResAddr - A pointer to the resource group addresses info to be used
                      if revision is before 2.1.

@return The error status of the xPU2.
*/
HAL_xpu2_Status HAL_xpu2_EnableMPUPartition
  ( HAL_xpu2_XPU2Type eXpu2,
    HAL_xpu2_ResourceGroupIDType uResId,
    const HAL_xpu2_ResourceGroupAddrType *pResAddr );

/**
@brief Disable MPU partition

   This function disables the MPU partition with the specified resource group
   and the content of xPU_RGn_* has no effect on access permission
   determination.  This functionality is supported in revision 2.1 or later.
   In case the revision is prior to 2.1, \c pResAddr is used to disable the
   partition start/end addresses.

@param[in] eXpu2 - The XPU2 to disable.
@param[in] uResId - The resource group to set up.
@param[in] pResAddr - A pointer to the resource group addresses info to be used
                      if revision is before 2.1.

@return The error status of the xPU2.
*/
HAL_xpu2_Status HAL_xpu2_DisableMPUPartition
  ( HAL_xpu2_XPU2Type eXpu2,
    HAL_xpu2_ResourceGroupIDType uResId,
    const HAL_xpu2_ResourceGroupAddrType *pResAddr );

/**
@brief Unlock the resource group(s) that is/are owned by MSA.

   This function unlocks the resource group(s) that is/are owned by MSA by
   disabling the MSA ownership.

@param[in] eXpu2 - The XPU2 to perform the unlock.
@param[in] uResId - The resource group to be unlocked.
                    Use HAL_XPU2_ALL_MSA_RES_GROUP to unlock all resource
                    groups.

@return The error status of the xPU2.
*/
HAL_xpu2_Status HAL_xpu2_UnlockMSAResourceGroup
  ( HAL_xpu2_XPU2Type             eXpu2,
    HAL_xpu2_ResourceGroupIDType  uResId );

/**
@brief Configure xPU2 error options.

   This includes whether or not:
   @li Client errors are reported to the requesting master and target slave
   @li Config errors are reported to the requesting master and target slave
   @li Interrupts are generated when errors occur
   @li Decode errors to the configuration port generate xPU2 errors

@param[in] eXpu2 - The XPU2 to configure device options on
@param[in] errOpt - The error options configuration

@return The error status of the xPU2.

@sa HAL_xpu2_GetErrorReportingConfig()
*/
HAL_xpu2_Status HAL_xpu2_ConfigErrorReporting
  ( HAL_xpu2_XPU2Type               eXpu2,
    HAL_xpu2_ErrorOptionsConfigType errOpt );
/** @} */

/* -----------------------------------------------------------------------
** Data Interface
** ----------------------------------------------------------------------- */
/** @name Data Interface */
/** @{ */

/**
@brief Retrieve MSA resource group configuration info.

   This function retrieves a list of MSA resource group configuration info
   such as partition IDs, start and end addresses, client access
   permissions, partition read/write access control, and partition control
   info.  The actual number of resource groups is also updated in
   \c puNumResGroups.  If \c pMSAResConfigInfoList is NULL, the actual number
   of resource groups is returned in \c puNumResGroups.  In case of error,
   \c puNumResGroups would be zero'd out.

@param[in] eXpu2 - The XPU2 to get the resource group config info
@param[out] pMSAResConfigInfo - A pointer to pointer of the list of config info
                                structure to store the partition IDs, start and
                                end addresses, client access permissions of
                                the MSA resource group, partition
                                read/write access control, and partition enable
                                info
@param[in/out] puNumResGroup - A pointer to the number of resource groups that
                               are owned by MSA

@return The error status of the xPU2.
*/
HAL_xpu2_Status HAL_xpu2_GetMSAResourceGroupConfigInfo
  ( HAL_xpu2_XPU2Type                  eXpu2,
    HAL_xpu2_ResourceGroupConfigType **pMSAResConfigInfoList,
    uint32                            *puNumResGroups );

/**
@brief Get the current xPU2 error options.

   This includes whether or not:
   @li Client errors are reported back to the requesting master
   @li Config errors are reported back to the requesting master
   @li Interrupts are generated when errors occur
   @li Decode errors to the configuration port generate xPU2 errors

@param[in]  eXpu2 - The xPU2 to get device options from
@param[out] pDevConfig - A pointer to the location to store the device options
                         configuration

@return The error status of the xPU2.

@sa HAL_xpu2_ConfigErrorReporting()
*/
HAL_xpu2_Status HAL_xpu2_GetErrorReportingConfig
  ( HAL_xpu2_XPU2Type                eXpu2,
    HAL_xpu2_ErrorOptionsConfigType *pErrConfig );

/**
@brief Check for an xPU2 error condition.

   This function checks for the xPU2 error condition.

@param[in] eXpu2 - The XPU2 to obtain error information from

@return The error status of the xPU2.

@retval TRUE  The xPU2 has recorded an error.
@retval FALSE The xPU2 has no recorded errors.
*/
bool32 HAL_xpu2_IsError          ( HAL_xpu2_XPU2Type    eXpu2 );

/**
@brief Get the pending xPU2 error.

   This includes: the error type (access control or decode) and the relevant
   bus parameters for the errant request.

@param[in]  eXpu2 - The XPU2 to obtain error information from
@param[out] pError - A pointer to the structure to use to return the xPU2 error

@return The error status of the xPU2.
*/
HAL_xpu2_Status HAL_xpu2_GetError( HAL_xpu2_XPU2Type    eXpu2,
                                   HAL_xpu2_ErrorType  *pError );

/**
@brief Clear the xPU2 device error status.

@param[in] eXpu2 - The XPU2 to clear errors on

@return The error status of the xPU2.
*/
HAL_xpu2_Status HAL_xpu2_ClearError( HAL_xpu2_XPU2Type    eXpu2 );

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* #ifndef HALXPU2_H */

