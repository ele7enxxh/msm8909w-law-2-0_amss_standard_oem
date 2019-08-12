#ifndef HALVMIDMT_H
#define HALVMIDMT_H
/**

@file   HALvmidmt.h

@brief  This module provides the Application Programing Interface (API) 
        to the VMIDMT HAL layer.
*/
/*
===========================================================================

                             Edit History

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/kernel/vmidmt/hal/inc/HALvmidmt.h#1 $

when         who     what, where, why
----------   ---     --------------------------------------------------------
2013/10/01   sc      Added ConfigVmidContextWithSIDList API
2013/09/10   sc      Added VMIDMT instance
2013/04/29   sc      Updated instance enums
2013/02/27   sc      Added ClearErrorExt
2012/11/29   sc      Migrated to use DevConfig
2012/08/28   sc      Added GetVmidCtxt API
2012/08/17   sc      Addressed the NSNUMSMRGO issue
2012/06/29   sc      Fixed the const type qualifier
2012/06/08   sc      Added read-after-write support
2012/05/29   sc      Updated for Elan
2012/05/23   sc      Renamed MDP5 to CAMSS for JPEG/VFE. Updated based on V3.0.1
2012/04/18   sc      Updated VMIDMT instances based on V3.0.0
2012/04/04   sc      Updated VMIDMT instances
2011/11/01   sc      Initial draft for VMIDMT
2010/06/21   sds     Code review updates
2010/03/25   sds     Added TSIF_BAM instance.
2010/02/09   sds     Added instance enums and updated type names.
2009/10/16   sds     Created 

===========================================================================
Copyright (c) 2009-2012
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
#include "HALvmidmtTarget.h"
#include "HALvmidmtDataTypes.h"

/* -----------------------------------------------------------------------
** Definitions
** ----------------------------------------------------------------------- */
/** use by HAL_vmidmt_Init for the default secureExtensions setting */
#define HAL_VMIDMT_SECURE_EXT_DEFAULT   0xFF

/** use by HAL_xpu2_ClearErrorExt to clear all the fault status bits */
#define HAL_VMIDMT_ALL_ERROR_STATUS_BITS 0x7FFFFFFE

/* -----------------------------------------------------------------------
** Types
** ----------------------------------------------------------------------- */
/** VMIDMT error status enum */
typedef enum
{
  HAL_VMIDMT_NO_ERROR,           /**< successful operation */
  HAL_VMIDMT_INVALID_INSTANCE,   /**< out of range VMIDMT instance */
  HAL_VMIDMT_UNSUPPORTED_INSTANCE_FOR_TARGET,
    /**< unsupported VMIDMT instance for the current target */
  HAL_VMIDMT_UNSUPPORTED_HANDLER,/**< unsupported handler */
  HAL_VMIDMT_INVALID_BASE_ADDR,  /**< invalid VMIDMT instance base address */
  HAL_VMIDMT_INVALID_PARAM,      /**< invalid passed in parameter */
  HAL_VMIDMT_INVALID_HW_VALUE,   /**< invalid HW generic value */
  HAL_VMIDMT_READ_WRITE_MISMATCH /**< read-after-write values not match */
} HAL_vmidmt_Status;

/** VMIDMT type */
typedef enum
{
  HAL_VMIDMT_CRYPTO0_AXI = 0,      /**< CRYPTO AXI - 0 */
  HAL_VMIDMT_CRYPTO1_AXI,          /**< CRYPTO AXI - 1 */
  HAL_VMIDMT_CRYPTO0_BAM,          /**< CRYPTO BAM - 0 */
  HAL_VMIDMT_CRYPTO1_BAM,          /**< CRYPTO BAM - 1 */
  HAL_VMIDMT_DEHR,                 /**< DEHR */
  HAL_VMIDMT_LPASS_DM,             /**< LPASS DM */
  HAL_VMIDMT_LPASS_LPAIF,          /**< LPASS LPAIF */
  HAL_VMIDMT_LPASS_MIDI,           /**< LPASS MIDI */
  HAL_VMIDMT_LPASS_Q6AHB,          /**< LPASS Q6AHB */
  HAL_VMIDMT_LPASS_Q6AXI,          /**< LPASS Q6AXI */
  HAL_VMIDMT_LPASS_RESAMPLER,      /**< LPASS Resampler */
  HAL_VMIDMT_LPASS_SLIMBUS,        /**< LPASS Slimbus */
  HAL_VMIDMT_CAMERA_SS,            /**< CAMERA Subsystem */
  HAL_VMIDMT_CAMSS_VBIF_JPEG,      /**< CAMERA VBIF JPEG */
  HAL_VMIDMT_CAMSS_VBIF_VFE,       /**< CAMERA VBIF VFE */
  HAL_VMIDMT_MMSS_DDR,             /**< MMSS DDR */
  HAL_VMIDMT_OCMEM,                /**< OCMEM */
  HAL_VMIDMT_OXILI,                /**< OXILI */
  HAL_VMIDMT_VENUS_CPUSS,          /**< Venus CPU SS */
  HAL_VMIDMT_VENUS_VBIF,           /**< Venus VBIF */
  HAL_VMIDMT_MSS_A2BAM,            /**< MSS A2BAM */
  HAL_VMIDMT_MSS_NAV_CE,           /**< MSS NAV CE */
  HAL_VMIDMT_MSS_Q6,               /**< MSS Q6 */
  HAL_VMIDMT_BAM_DMA,              /**< BAM DMA */
  HAL_VMIDMT_BAM_BLSP1_DMA,        /**< BAM BLSP DMA - 1 */
  HAL_VMIDMT_BAM_BLSP2_DMA,        /**< BAM BLSP DMA - 2 */
  HAL_VMIDMT_BAM_SDCC1,            /**< BAM SDCC1 */
  HAL_VMIDMT_BAM_SDCC2,            /**< BAM SDCC2 */
  HAL_VMIDMT_BAM_SDCC3,            /**< BAM SDCC3 */
  HAL_VMIDMT_BAM_SDCC4,            /**< BAM SDCC4 */
  HAL_VMIDMT_TSIF,                 /**< TSIF */
  HAL_VMIDMT_USB1_HS,              /**< BAM USB OTG */
  HAL_VMIDMT_USB2_HSIC,            /**< USB OTG */
  HAL_VMIDMT_QDSS_VMIDDAP,         /**< QDSS VMIDDAP */
  HAL_VMIDMT_QDSS_VMIDETR,         /**< QDSS VMIDETR */
  HAL_VMIDMT_RPM_MSGRAM,           /**< RPM MSGRAM */
  HAL_VMIDMT_SPDM_WRAPPER,         /**< SPDM TOP */
  HAL_VMIDMT_USB30,                /**< USB30 */
  HAL_VMIDMT_PRONTO,               /**< PRONTO */
  HAL_VMIDMT_QPIC_BAM,             /**< QPIC BAM */
  HAL_VMIDMT_IPA,                  /**< IPA */
  HAL_VMIDMT_APCS,                 /**< APCS */
  HAL_VMIDMT_CRYPTO2_AXI,          /**< CRYPTO AXI - 2 */
  HAL_VMIDMT_CRYPTO2_BAM,          /**< CRYPTO BAM - 2 */
  HAL_VMIDMT_EMAC,
  HAL_VMIDMT_LPASS_HDMI,
  HAL_VMIDMT_LPASS_SPDIF,
  HAL_VMIDMT_VENUS_VBIF2,
  HAL_VMIDMT_MMSS_VPU_MAPLE,
  HAL_VMIDMT_USB_HS_SEC,
  HAL_VMIDMT_SATA,
  HAL_VMIDMT_PCIE20,
  HAL_VMIDMT_PCIE0,
  HAL_VMIDMT_PCIE1,
  HAL_VMIDMT_USB3_HSIC,
  HAL_VMIDMT_UFS,
  HAL_VMIDMT_LPASS_SB1,
  HAL_VMIDMT_QPIC,
  HAL_VMIDMT_COUNT,        
  HAL_VMIDMT_SIZE = 0x7FFFFFFF     /* force to 32-bit enum */
} HAL_vmidmt_InstanceType; 

/** VMID related definitions */
/* Maximum allowed VMID value HAL_VMIDMT_MAX_VMID is defined in SConscript */
#define HAL_VMIDMT_MASKS_PER_FIELD 32 /**< Number of mask bits per field */
#define HAL_VMIDMT_PERMS_PER_FIELD 32 /**< Number of access perms per field */

/** number of required VMID mask dwords */
#define HAL_VMIDMT_MASTER_MASK_WORDS \
        ((HAL_VMIDMT_MAX_VMID/HAL_VMIDMT_MASKS_PER_FIELD) + 1)

/** number of required VMID permission dwords */
#define HAL_VMIDMT_MASTER_PERM_WORDS \
        ((HAL_VMIDMT_MAX_VMID/HAL_VMIDMT_PERMS_PER_FIELD) + 1)

/** Access configuration */
typedef struct
{
  uint32 auVMID[HAL_VMIDMT_MASTER_MASK_WORDS];      /**< VMID to mask */
  uint32 auVMIDPerm[HAL_VMIDMT_MASTER_PERM_WORDS];  /**< VMID permissions */
} HAL_vmidmt_AccessConfigType;

/** Error reporting options */
typedef enum
{
  HAL_VMIDMT_ERROR_O_SMCFCFG_EN = 0x1,
    /**< raise a stream match conflict fault configuration error
     * on transactions that match multiple entries in the stream
     * mapping table */
  HAL_VMIDMT_ERROR_O_USFCFG_EN  = 0x2,
    /**< raise a unidentified stream fault error on transactions
     * that match no entries in the stream mapping table */
  HAL_VMIDMT_ERROR_O_GCFGFIE    = 0x4,
    /**< generate interrupt on global configuration fault */
  HAL_VMIDMT_ERROR_O_GCFGFRE    = 0x8,
    /**< report global configuration fault to offending master */
  HAL_VMIDMT_ERROR_O_GFIE       = 0x10,
    /**< generate interrupt on global client fault */
  HAL_VMIDMT_ERROR_OPTIONS_SIZE = 0x7FFFFFFF /* force to 32-bit enum*/
} HAL_vmidmt_ErrorOptionType;

/** Store error reporting options ORed in this */
typedef uint32 HAL_vmidmt_ErrorOptionConfigType;

/** VMIDMT device parameters */
typedef struct
{
  uint16 uEntryCount;      /**< Total number of table entries */
  uint16 uNumVmid;         /**< number of VMID supported by the core */
  uint8 bStreamMapSupport; /** Is stream mapping mode supported? */
  uint8 uNumSSDIndexBits;
    /**< number of secure status determination index bits can be used to index
     * into the secure status determination table */
  uint8 uNumStreamIDBits; 
    /**< number of stream ID bits that are available to configure the SMT */
} HAL_vmidmt_DeviceParamsType;

/** VMIDMT Error flags */
typedef enum
{
  HAL_VMIDMT_ERROR_F_CLMULTI     = 0x1,
    /**< multiple error condition.  2nd error is client error.
     * 1st error can be either client or configuration error */
  HAL_VMIDMT_ERROR_F_CFGMULTI    = 0x2,
    /**< multiple error condition.  2nd error is configuration error.
     * 1st error can be either client or configuration error */
  HAL_VMIDMT_ERROR_F_PF          = 0x4,
    /**< fault due to permission fault */
  HAL_VMIDMT_ERROR_F_CAF         = 0x8,
    /**< configuration access fault.  Fault due to access to disallowed
     * configuration address.  E.g. access to reserved location */
  HAL_VMIDMT_ERROR_F_SMCF        = 0x10,
    /**< stream match conflict fault.  Fault due to multiple match in
     * stream match registers */
  HAL_VMIDMT_ERROR_F_USF         = 0x20,
    /**< unidentified stream fault.  Fault due to no match in
     * stream match registers */
  HAL_VMIDMT_ERROR_FLAGS_SIZE    = 0x7FFFFFFF /* force to 32-bit enum*/
} HAL_vmidmt_ErrorFlagsType;

/** Store error flags ORed in this */
typedef uint32 HAL_vmidmt_ErrorFlagsConfigType;

/** Bus related error flags */
typedef enum
{
  HAL_VMIDMT_BUS_F_ERROR_NSATTR  = 0x1, /**< non-secure attribute */
  HAL_VMIDMT_BUS_F_ERROR_NSSTATE = 0x2, /**< non-secure state */
  HAL_VMIDMT_BUS_F_ERROR_WNR     = 0x4, /**< write not read */
  HAL_VMIDMT_BUS_FLAGS_SIZE     = 0x7FFFFFFF /* force to 32-bit enum*/
} HAL_vmidmt_BusErrorFlagsType;

/** Store bus flags ORed in this */
typedef uint32 HAL_vmidmt_BusErrorFlagsMaskType;

/** Full VMIDMT error reporting structure */
typedef struct
{
  HAL_vmidmt_ErrorFlagsConfigType uErrorFlags; /**< Error Flags */
  HAL_vmidmt_BusErrorFlagsMaskType uBusFlags;  /**< Bus specific flags */
  uint32 uPhysicalAddressLower32; 
    /**< The lower 32 bits of the physical address of the errant request */
  uint32 uPhysicalAddressUpper32; 
    /**< The upper 32 bits of the physical address of the errant request */
  uint32 uSSDIndex;       /**< The SSD index of transaction causing fault */
  uint32 uSID;            /**< The stream ID of transaction causing fault */
  uint32 uMasterId;       /**< The master ID of the offending master */
  uint32 uAVMID;          /**< The virtual master ID of the offending master */
  uint32 uATID;           /**< The ATID field of the errant request */
  uint32 uABID;           /**< The ABID field of the errant request */
  uint32 uAPID;           /**< The APID field of the errant request */
} HAL_vmidmt_ErrorType;

/** VMIDMT secure status determination type */
typedef uint32 HAL_vmidmt_SecureStatusDetType;

/** VMIDMT non-secure allocate configuration options.
 * Only valid for secure access. */
typedef enum
{
  HAL_VMIDMT_NSCFG_XTRAN = 0, /**< use xPROTNS as presented with transaction */
  HAL_VMIDMT_NSCFG_SECURE  = 2,    /**< secure setting */
  HAL_VMIDMT_NSCFG_NONSECURE  = 3, /**< non-secure setting */
  HAL_VMIDMT_NSCFG_DEFAULT = 4,    /**< use default setting */
} HAL_vmidmt_NonSecureAllocConfigType;

/** VMIDMT write allocate configuration options */
typedef enum
{
  HAL_VMIDMT_WACFG_XTRAN = 0,
    /**< use allocation attributes as presented with transaction */
  HAL_VMIDMT_WACFG_ALLOC  = 2,    /**< allocate setting */
  HAL_VMIDMT_WACFG_NONALLOC  = 3, /**< non-allocate setting */
  HAL_VMIDMT_WACFG_DEFAULT = 4,   /**< use default setting */
} HAL_vmidmt_WriteAllocConfigType;

/** VMIDMT read allocate configuration options */
typedef enum
{
  HAL_VMIDMT_RACFG_XTRAN = 0,
    /**< use allocation attributes as presented with transaction */
  HAL_VMIDMT_RACFG_ALLOC  = 2,    /**< allocate setting */
  HAL_VMIDMT_RACFG_NONALLOC  = 3, /**< non-allocate setting */
  HAL_VMIDMT_RACFG_DEFAULT = 4,   /**< use default setting */
} HAL_vmidmt_ReadAllocConfigType;

/** VMIDMT shared configuration options */
typedef enum
{
  HAL_VMIDMT_SHCFG_XTRAN = 0,
    /**< use shareable attributes as presented with transaction */
  HAL_VMIDMT_SHCFG_OUTER_SHARE = 1,  /**< outer-shareable */
  HAL_VMIDMT_SHCFG_INNER_SHARE  = 2, /**< inner-shareable */
  HAL_VMIDMT_SHCFG_NON_SHARE  = 3,   /**< non-shareable */
  HAL_VMIDMT_SHCFG_DEFAULT = 4,      /**< use default setting */
} HAL_vmidmt_SharedConfigType;

/** VMIDMT memory type configuration options */
typedef enum
{
  HAL_VMIDMT_MTCFG_XTRAN = 0,
    /**< use memory attributes as presented with transaction */
  HAL_VMIDMT_MTCFG_MEMATTR = 1, /**< use MemAttr field for memory attributes */
  HAL_VMIDMT_MTCFG_DEFAULT = 2, /**< use default setting */
} HAL_vmidmt_MemTypeConfigType;

/** VMIDMT transient control configuration options */
typedef enum
{
  HAL_VMIDMT_TRANSIENTCFG_XTRAN = 0,
    /**< use transient attributes as presented with transaction */
  HAL_VMIDMT_TRANSIENTCFG_NON_TRANSIENT = 2, /**< non-transient */
  HAL_VMIDMT_TRANSIENTCFG_TRANSIENT = 3,     /**< transient */
  HAL_VMIDMT_TRANSIENTCFG_DEFAULT = 4,       /**< use default setting */
} HAL_vmidmt_TransientConfigType;

/** VMIDMT bus attributes structure */
typedef struct
{
  HAL_vmidmt_NonSecureAllocConfigType eNSCFG;
    /**< non-secure allocate configuration */
  HAL_vmidmt_WriteAllocConfigType eWACFG; /**< write allocate configuration */
  HAL_vmidmt_ReadAllocConfigType eRACFG;  /**< read allocate configuration */
  HAL_vmidmt_SharedConfigType eSHCFG;     /**< shared configuration */
  HAL_vmidmt_MemTypeConfigType eMTCFG;    /**< memory type configuration */
  uint8 uMemAttr; /**< memory attributes.  Support only 3 bits. */
  HAL_vmidmt_TransientConfigType eTransientCfg;
    /**< transient control configuration */
} HAL_vmidmt_BusAttribType;

/** VMIDMT redirect cacheable-non-shareable configuration options */
typedef enum
{
  HAL_VMIDMT_RCNSH_DISABLE = 0,
    /**< do not enable redirect for cacheable-non-shareable transactions */
  HAL_VMIDMT_RCNSH_ENABLE = 1,
    /**< enable redirect for cacheable-non-shareable transactions */
  HAL_VMIDMT_RCNSH_DEFAULT = 2, /**< use default setting */
} HAL_vmidmt_RedirCacheNonShareableConfigType;

/** VMIDMT redirect cacheable-inner-shareable configuration options */
typedef enum
{
  HAL_VMIDMT_RCISH_DISABLE = 0,
    /**< do not enable redirect for cacheable-inner-shareable transactions */
  HAL_VMIDMT_RCISH_ENABLE = 1,
    /**< enable redirect for cacheable-inner-shareable transactions */
  HAL_VMIDMT_RCISH_DEFAULT = 2, /**< use default setting */
} HAL_vmidmt_RedirCacheInnerShareableConfigType;

/** VMIDMT redirect cacheable-Outer-shareable configuration options */
typedef enum
{
  HAL_VMIDMT_RCOSH_DISABLE = 0,
    /**< do not enable redirect for cacheable-Outer-shareable transactions */
  HAL_VMIDMT_RCOSH_ENABLE = 1,
    /**< enable redirect for cacheable-Outer-shareable transactions */
  HAL_VMIDMT_RCOSH_DEFAULT = 2, /**< use default setting */
} HAL_vmidmt_RedirCacheOuterShareableConfigType;

/** VMIDMT request priority configuration options */
typedef enum
{
  HAL_VMIDMT_REQPRICFG_XTRAN = 0,
    /**< use REQPRIORITY attributes as presented with transactions */
  HAL_VMIDMT_REQPRICFG_ACR_REQPRI = 1,
    /**< use VMIDMT_(S)ACR.REQPRIORITY override value instead of
     * value presented with transactions */
  HAL_VMIDMT_REQPRICFG_DEFAULT = 2, /**< use default setting */
} HAL_vmidmt_ReqPriorityConfigType;

/** VMIDMT request priority options */
typedef enum
{
  HAL_VMIDMT_REQPRI_NORMAL = 0,  /**< normal relative priority */
  HAL_VMIDMT_REQPRI_HIGH = 1,    /**< high relative priority */
  HAL_VMIDMT_REQPRI_HIGHER = 2,  /**< higher relative priority */
  HAL_VMIDMT_REQPRI_HIGHEST = 3, /**< highest relative priority */
  HAL_VMIDMT_REQPRI_DEFAULT = 4, /**< use default setting */
} HAL_vmidmt_ReqPriorityType;

/** VMIDMT auxiliary configuration structure */
typedef struct
{
  HAL_vmidmt_RedirCacheNonShareableConfigType eRCNSH;
    /**< redirect cacheable-non-shareable */
  HAL_vmidmt_RedirCacheInnerShareableConfigType eRCISH;
    /**< redirect cacheable-inner-shareable */
  HAL_vmidmt_RedirCacheOuterShareableConfigType eRCOSH;
    /**< redirect cacheable-outer-shareable */
  HAL_vmidmt_ReqPriorityConfigType eReqPriorityCfg;
    /**< request priority configuration */
  HAL_vmidmt_ReqPriorityType eReqPriority;
    /**< request priority */
} HAL_vmidmt_AuxConfigType;

/** Default VMIDMT configurations which includes the configurations for the
 *  bypass bus attributes, bypass VMID, bypass auxiliary configuration,
 *  and permission access control for the built-in RPU. */
typedef struct
{
   HAL_vmidmt_BusAttribType     *pBypassBusAttrib;
     /**< bypass bus attributes */
   HAL_vmidmt_AuxConfigType     *pBypassAuxConfig;
     /**< bypass auxiliary configuration */
   HAL_vmidmt_AccessConfigType  *pAccessControl;
     /**< permission access control for built-in RPU */
   bool32                        bVmidPrivateNamespaceEnable;
     /**< VMID private namespace enable */
   uint8                         bypassVmid;
     /**< bypass VMID */
} HAL_vmidmt_DefaultVmidConfigType;

/** Default secure VMIDMT configurations which include all the VMID
 *  configuration fields and the security extensions */
typedef struct
{
   HAL_vmidmt_DefaultVmidConfigType     *pDefaultSecureConfig;
     /**< default VMID configuration setting */
   bool32                                bGlbAddrSpaceRestrictedAccEnable;
     /**< global address space restricted access enable */
   uint8                                 secureExtensions;
     /**< non-secure number of stream mapping register groups override
      *   HAL_VMIDMT_SECURE_EXT_DEFAULT can be used for default setting */
} HAL_vmidmt_DefaultSecureVmidConfigType;

/** VMIDMT context configuration structure which includes the configurations
 *  for stream mapping table and group to context table */
typedef struct
{
  HAL_vmidmt_BusAttribType        *pBusAttrib; /**< config S2VRn */
  HAL_vmidmt_AuxConfigType        *pAuxConfig; /**< config AS2VRn */
  uint8                            uVmid;      /**< config VMID in S2VRn */
} HAL_vmidmt_ContextConfigType;


/* -----------------------------------------------------------------------
** Mandatory Interface
** ----------------------------------------------------------------------- */
/** 
@brief Initialization function.

   This function initializes the VMIDMT by configuring the permission access
   control for built-in RPU, bypass bus attributes, bypass VMID, bypass
   auxiliary configurations, and secure extensions.

@param[in]  eVmidmt - The vmidmt to initalize
@param[in]  pSecureConfig - Pointer to the default secure configurations
@param[in]  pNonSecureConfig - Pointer to the default non-secure configurations
@param[out] ppszVersion - Pointer to version string.  Currently not populated.

@return The error status of the VMIDMT.
*/
HAL_vmidmt_Status HAL_vmidmt_Init
        ( HAL_vmidmt_InstanceType                       eVmidmt,
          const HAL_vmidmt_DefaultSecureVmidConfigType *pSecureConfig,
          const HAL_vmidmt_DefaultVmidConfigType       *pNonSecureConfig,
          char                                        **ppszVersion );

/**
@brief Reset the selected VMIDMT hardware to a default state. Currently stub out.

@param[in] eVmidmt - The VMIDMT hardware to reset.

@pre HAL_vmidmt_Init() must have been called.

@see HAL_vmidmt_Init()
*/
void HAL_vmidmt_Reset( HAL_vmidmt_InstanceType eVmidmt ) ;

/**
@brief Save the current VMIDMT state.  Currently stub out.

@param[in] eVmidmt - The VMIDMT state to save.

@pre HAL_vmidmt_Init() must have been called.

@see HAL_vmidmt_Init()
@see HAL_vmidmt_Restore()
*/
void HAL_vmidmt_Save( HAL_vmidmt_InstanceType eVmidmt );

/**
@brief Restore an VMIDMT to a previously saved state.  Currently stub out.

@param[in] eVmidmt -The VMIDMT to have its state restored from a previous save.

@pre HAL_vmidmt_Init() and HAL_vmidmt_Save() must have been called.

@see HAL_vmidmt_Init()
@see HAL_vmidmt_Save()
*/
void HAL_vmidmt_Restore( HAL_vmidmt_InstanceType eVmidmt );

/* -----------------------------------------------------------------------
** Configuration Interface
** ----------------------------------------------------------------------- */
/**
@brief Enable client port.

   This function enables the client port such that the client accesses are
   processed using VMIDMT SMT to generate the bus attributes and VMID.

@param[in] eVmidmt - The VMIDMT to configure
@param[in] bSecure - indicate if it is secure vs non-secure configuration

@return The error status of the VMIDMT.

@pre HAL_vmidmt_Init() must have been called.

@see HAL_vmidmt_Init()
@see HAL_vmidmt_DisableClientPort()
*/
HAL_vmidmt_Status HAL_vmidmt_EnableClientPort
        ( HAL_vmidmt_InstanceType    eVmidmt,
          bool32                     bSecure );

/**
@brief Disable client port.

   This function disables the client port such that the client accesses are
   processed using the user defined bypass values to generate the bus
   attributes and VMID.

@param[in] eVmidmt - The VMIDMT to configure
@param[in] bSecure - indicate if it is secure vs non-secure configuration

@return The error status of the VMIDMT.

@pre HAL_vmidmt_Init() must have been called.

@see HAL_vmidmt_Init()
@see HAL_vmidmt_EnableClientPort()
*/
HAL_vmidmt_Status HAL_vmidmt_DisableClientPort
        ( HAL_vmidmt_InstanceType   eVmidmt,
          bool32                    bSecure );

/**
@brief Configure the secure status determination table.

   This function configures the secure status determination table per the
   specified secure status determination array.  The maximum number of entries
   in the secure status determination table is 4.  Each bit in the array entry
   corresponds to a secure state.

@param[in] eVmidmt - The VMIDMT to configure
@param[in] pSecureStatusDetArray - pointer to the secure status determination
                                   array to be configured
@param[in] uNumElements - number of elements in the secure status determination
                          array.  Max is 4.

@return The error status of the VMIDMT.

@pre HAL_vmidmt_Init() must have been called.

@see HAL_vmidmt_Init()
*/
HAL_vmidmt_Status HAL_vmidmt_ConfigSecureStatusDetTable
        ( HAL_vmidmt_InstanceType               eVmidmt,
          const HAL_vmidmt_SecureStatusDetType *pSecureStatusDetArray,
          uint32                                uNumElements );

/**
@brief Configure the secure status determination table.

	  This function configures the secure status determination table per the
	  specified secure status of a secific index.  
   
   @param[in] pVmidmt - A pointer to the VMIDMT's info structure.
   @param[in] secureStatusDetIndex - Index whose SSD has to be changed
   @param[in] secState	- Secure/Non-Secure state which has to be configured for the above index


@return The error status of the VMIDMT.

@pre HAL_vmidmt_Init() must have been called.

@see HAL_vmidmt_Init()
*/
HAL_vmidmt_Status HAL_vmidmt_ConfigSecureStatusDetTableExt
        ( HAL_vmidmt_InstanceType               eVmidmt,
          const HAL_vmidmt_SecureStatusDetType  secureStatusDetIndex,
          uint32                                secState );

/**
@brief Configure the VMIDMT context

   This function configures the VMIDMT context table per the specified stream
   ID and the context configuration info.

@param[in] eVmidmt - The VMIDMT to configure
@param[in] uStreamID - stream ID to index into the context table
@param[in] pContextConfig - pointer to the context configuration

@return The error status of the VMIDMT.

@pre HAL_vmidmt_Init() must have been called.

@see HAL_vmidmt_Init()
@see HAL_vmidmt_ConfigVmidContextWithSIDList()
@see HAL_vmidmt_GetVmidContext()
*/
HAL_vmidmt_Status HAL_vmidmt_ConfigVmidContext
        ( HAL_vmidmt_InstanceType             eVmidmt,
          uint32                              uStreamID,
          const HAL_vmidmt_ContextConfigType *pContextConfig );


/**
@brief Configure the VMIDMT context with all the possible stream IDs in both 
       non-secure and secure domains. SSDT will guide us which domain to be used.
       Two consecutive stream-ids will me masked and programmed as one SID.
      
   This function configures the VMIDMT context table per the list of stream
   IDs and the context configuration info into the specified table index.

@param[in] eVmidmt - The VMIDMT to configure
@param[in] pContextConfig - pointer to the context configuration

@return The error status of the VMIDMT.

@pre HAL_vmidmt_Init() must have been called.

@see HAL_vmidmt_Init()
*/
HAL_vmidmt_Status HAL_vmidmt_StaticConfigVmidContext
        ( HAL_vmidmt_InstanceType             eVmidmt,
          const HAL_vmidmt_ContextConfigType *pContextConfig );

/**
@brief Configure the VMIDMT context with list of stream IDs

   This function configures the VMIDMT context table per the list of stream
   IDs and the context configuration info into the specified table index.

@param[in] eVmidmt - The VMIDMT to configure
@param[in] uIndex - table index to be configured
@param[in] pSIDList - pointer to the list of stream IDs to match
@param[in] uNumElements - number of elements in the list of stream IDs
@param[in] pContextConfig - pointer to the context configuration

@return The error status of the VMIDMT.

@pre HAL_vmidmt_Init() must have been called.

@see HAL_vmidmt_Init()
@see HAL_vmidmt_ConfigVmidContext()
@see HAL_vmidmt_GetVmidContext()
*/
HAL_vmidmt_Status HAL_vmidmt_ConfigVmidContextWithSIDList
        ( HAL_vmidmt_InstanceType             eVmidmt,
          uint32                              uIndex,
          const uint32                       *pSIDList,
          uint32                              uNumElements,
          const HAL_vmidmt_ContextConfigType *pContextConfig );

/**
@brief Configure VMIDMT error reporting.

   This includes whether or not:
   @li Raise a stream match conflict configuration fault (SMCFCFG)
   @li Raise a unidentified stream configuration fault (USFCFG)
   @li Interrupts are generated on global configuration fault (GCFGFIE)
   @li Report back to the requesting master on global config fault (GCFGFRE)
   @li Interrupts are generated on global client fault (GFIE)

@param[in] eVmidmt - The VMIDMT to configure error reporting on
@param[in] bSecure - indicate if it is secure vs non-secure configuration
@param[in] errConfig - The error reporting configuration

@return The error status of the VMIDMT.

@pre HAL_vmidmt_Init() must have been called.

@see HAL_vmidmt_Init()
@see HAL_vmidmt_GetErrorReportingConfig()
*/
HAL_vmidmt_Status HAL_vmidmt_ConfigErrorReporting
        ( HAL_vmidmt_InstanceType          eVmidmt,
          bool32                           bSecure,
          HAL_vmidmt_ErrorOptionConfigType errConfig );

/* -----------------------------------------------------------------------
** Data Interface
** ----------------------------------------------------------------------- */
/**
@brief Get VMIDMT hardware device parameters.

   This function reads the VMIDMT device operating parameters from the
   hardware.  This includes device type, device variant, and resource
   group information.

@param[in]  eVmidmt - The VMIDMT to get parameters for
@param[out] pDevParams - A pointer to the structure to store the device
                         parameters

@return The error status of the VMIDMT.
*/
HAL_vmidmt_Status HAL_vmidmt_GetDeviceParameters
        ( HAL_vmidmt_InstanceType      eVmidmt,
          HAL_vmidmt_DeviceParamsType *pDevParams );

/**
@brief Get the VMIDMT context

   This function reads the VMIDMT context table per the specified stream ID.

@param[in]  eVmidmt - The VMIDMT to obtain the VMIDMT context table
@param[in]  uStreamID - stream ID to index into the context table
@param[out] pContextConfig - A pointer to the structure to store the VMIDMT
                             context information

@return The error status of the VMIDMT.

@pre HAL_vmidmt_Init() must have been called.

@see HAL_vmidmt_Init()
@see HAL_vmidmt_ConfigVmidContext()
*/
HAL_vmidmt_Status HAL_vmidmt_GetVmidContext
        ( HAL_vmidmt_InstanceType       eVmidmt,
          uint32                        uStreamID,
          HAL_vmidmt_ContextConfigType *pContextConfig );

/**
@brief Get the current VMIDMT error reporting options.

   This includes whether or not:
   @li Raise a stream match conflict configuration fault (SMCFCFG)
   @li Raise a unidentified stream configuration fault (USFCFG)
   @li Interrupts are generated on global configuration fault (GCFGFIE)
   @li Report back to the requesting master on global config fault (GCFGFRE)
   @li Interrupts are generated on global client fault (GFIE)

@param[in]  eVmidmt - The VMIDMT to get error reporting options from
@param[in] bSecure - indicate if it is secure vs non-secure configuration
@param[out] pErrConfig - A pointer to the location to store the error reporting
                         configuration

@pre HAL_vmidmt_Init() must have been called.

@see HAL_vmidmt_Init()
@see HAL_vmidmt_ConfigErrorReporting()
*/
void HAL_vmidmt_GetErrorReportingConfig
        ( HAL_vmidmt_InstanceType           eVmidmt,
          bool32                            bSecure,
          HAL_vmidmt_ErrorOptionConfigType *pErrConfig );

/**
@brief Check for an VMIDMT error condition.

@param[in] eVmidmt - The VMIDMT to obtain error information from
@param[in] bSecure - indicate if check on secure vs non-secure side

@return The error status of the VMIDMT.

@retval TRUE  The VMIDMT has recorded an error.
@retval FALSE The VMIDMT has no recorded errors.

@pre HAL_vmidmt_Init() must have been called.

@see HAL_vmidmt_Init()
*/
bool32 HAL_vmidmt_IsError( HAL_vmidmt_InstanceType eVmidmt,
                           bool32                  bSecure );

/**
@brief Get the pending VMIDMT error.

   This includes: the error type (access control or fault type),
   transaction index and the relevant bus parameters for the errant request.

@param[in]  eVmidmt - The VMIDMT to obtain error information from
@param[in]  bSecure - indicate obtaining error info on the secure vs
                      non-secure side
@param[out] pError - A pointer to the structure to use to return the
                     VMIDMT error

@pre HAL_vmidmt_Init() must have been called.

@see HAL_vmidmt_Init()
*/
void HAL_vmidmt_GetError( HAL_vmidmt_InstanceType   eVmidmt,
                          bool32                    bSecure,
                          HAL_vmidmt_ErrorType     *pError );

/**
@brief Clear the VMIDMT device error status.

@param[in] eVmidmt - The VMIDMT to clear errors on
@param[in] bSecure - indicate clear errors on secure vs non-secure side

@return The error status of the VMIDMT.

@pre HAL_vmidmt_Init() must have been called.

@see HAL_vmidmt_Init()
*/
HAL_vmidmt_Status HAL_vmidmt_ClearError( HAL_vmidmt_InstanceType eVmidmt,
                                         bool32                  bSecure );

/**
@brief Clear the specific VMIDMT error fault(s).

@param[in] eVmidmt - The VMIDMT to clear fault(s) on
@param[in] bSecure - indicate clear errors on secure vs non-secure side
@param[in] uErrorFlags - the ORed fault status bit(s) to be cleared or
                         HAL_VMIDMT_ALL_ERROR_STATUS_BITS to clear all
                         fault status bits

@return The error status of the VMIDMT.

@pre HAL_vmidmt_Init() must have been called.

@see HAL_vmidmt_Init()
*/
HAL_vmidmt_Status HAL_vmidmt_ClearErrorExt
        ( HAL_vmidmt_InstanceType         eVmidmt,
          bool32                          bSecure,
          HAL_vmidmt_ErrorFlagsConfigType uErrorFlags );

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* #ifndef HALVMIDMT_H */

