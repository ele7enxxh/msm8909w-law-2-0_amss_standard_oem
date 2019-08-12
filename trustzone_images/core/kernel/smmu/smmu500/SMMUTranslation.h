#ifndef SMMUTRANSLATION_H
#define SMMUTRANSLATION_H
/*=========================================================================

                       S M M U  D A L  L I B R A R Y

FILE:      DALSMMU.h
   System MMU DAL

GENERAL DESCRIPTION
  This file contains data structures and exported API by DAL SMMU

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS
 
Copyright (c) 2010 - 2014
Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
==========================================================================*/
/*==========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/kernel/smmu/smmu500/SMMUTranslation.h#1 $
$DateTime: 2016/06/17 14:31:11 $
$Author: pwbldsvc $
$Change: 10727476 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------
02/10/14   sc      Supported stage 2 mapping
09/03/13   sc      Updated fault_regs_dump with global space 0 registers
08/21/13   sc      Updated fault_regs_dump
06/15/11   spa     Initial Version
==========================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 *--------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

#include "SMMUTypes.h"
#include "SMMUConfig.h"
#include "HALSMMU.h"

/* ----------------------------------------------------------------------------
** Definitions
** ------------------------------------------------------------------------- */
/** Bit mask of permissions */
#define PERM_X 0x1
#define PERM_W 0x2
#define PERM_R 0x4

/** Use as parameter passing into SMMU_DumpCBFaultRegs to dump
 *  the global space 0 fault registers */
#define SMMU_DUMP_GLOBAL_FAULT 0xFFFFFFFF

/* ----------------------------------------------------------------------------
** Types
** ------------------------------------------------------------------------- */
/** Not supporting Large pages */
// for legacy address mapping
typedef enum
{
  SECTION_1MB,
  PAGE_4KB,
  SUPPORTED_SIZE_MAX,
  UNSUPPORTED_SIZE = 0x7FFFFFFF
} mappingType;

/** Format is TEX[2:0]:C:B */
// for legacy address mapping
typedef enum
{
  REGION_STRONGLY_ORDERED,
  REGION_SHARED_DEVICE,
  REGION_INNER_OUTER_WT_NWA,
  REGION_INNER_OUTER_WB_NWA,
  REGION_INNER_OUTER_NON_CACHE,
  REGION_RESERVED,
  REGION_IMP_DEFINED,
  REGION_INNER_OUTER_WB_WA,
  REGION_NON_SHARED_DEVICE,
  REGION_SUPPORTED_MAX,
  REGION_UNSUPPORTED = 0x7FFFFFFF
} regionType;

/** memory shareability type for normal memory */
typedef enum
{
  NORMAL_NON_SHAREABLE,
  NORMAL_OUTER_SHAREABLE = 2,
  NORMAL_INNER_SHAREABLE,
  MEMORY_SHAREABLE_SUPPORTED_MAX,
  MEMORY_SHAREABLE_SIZE = 0x7FFFFFFF,  // force to 32-bit enum
} memory_shareableType;

/** LPAE memory attributes */
typedef enum
{
  MEMORY_ATTR_STRONGLY_ORDERED = 0,
  MEMORY_ATTR_DEVICE,
  MEMORY_ATTR_INNER_OUTER_NON_CACHE,
  // normal, inner outer write-through, read/write allocate
  MEMORY_ATTR_INNER_OUTER_WT_RWA,
  // normal, inner outer write-through, read allocate, no write allocate
  MEMORY_ATTR_INNER_OUTER_WT_RA,
  // normal, inner outer write-back, read/write allocate
  MEMORY_ATTR_INNER_OUTER_WB_RWA,
  // normal, inner outer write-back, no read allocate, write allocate
  MEMORY_ATTR_INNER_OUTER_WB_WA,
  // normal, inner outer write-back, read allocate, no write allocate
  MEMORY_ATTR_INNER_OUTER_WB_RA,
  MEMORY_ATTR_SUPPORTED_MAX,
  MEMORY_ATTR_UNSUPPORTED = 0x7FFFFFFF, // force to 32-bit enum
} memory_attrType;

/** all mappings are non-cacheable. need this to be configurable? */
// for legacy address mapping
typedef struct map_attributesType {
  uint32      virt_addr;
  uint32      phys_addr;
  uint32      size;
  mappingType type;
  uint32      permissions;
  regionType  rngtype;
} map_attributesType;

/** mapping attributes for populating the translation descriptor */
typedef struct smmu_map_attributesType {
  uint64 size; // in bytes
  uint64 phys_addr;
  uint64 virt_addr;
  uint32 permissions;
  memory_shareableType shareability;
  memory_attrType mem_attr;
  // bSecure is not applicable for stage 2 translation and would be ignored
  boolean bSecure;
} smmu_map_attributesType;

/** Format used to dump CB Fault syndrome registers */
typedef struct fault_regs_dump {
  uint32      dump_size;
  uint32      fsr_addr;
  SMMU_HAL_CBn_FSR_t      fsr;
  uint32      far0_addr;
  uint32      far0;
  uint32      far1_addr;
  uint32      far1;
  //uint32      ipafar0_addr;
  //uint32      ipafar0;
  //uint32      ipafar1_addr;
  //uint32      ipafar1;
  uint32      par0_addr;
  uint32      par0;
  uint32      par1_addr;
  uint32      par1;
  uint32      fsyn0_addr;
  uint32      fsyn0;
  uint32      fsyn1_addr;
  uint32      fsyn1;
  uint32      ttbr0_0_addr;
  uint32      ttbr0_0;
  uint32      ttbr0_1_addr;
  uint32      ttbr0_1;
  uint32      ttbr1_0_addr;
  uint32      ttbr1_0;
  uint32      ttbr1_1_addr;
  uint32      ttbr1_1;
  uint32      ttbcr_addr;
  SMMU_HAL_CBn_TTBCR_t      ttbcr;
  uint32      sctlr_addr;
  SMMU_HAL_CBn_SCTLR_t      sctlr;
  uint32      actlr_addr;
  SMMU_HAL_CBn_ACTLR_t      actlr;
  uint32      prrr_addr;
  uint32      prrr;
  uint32      nmrr_addr;
  uint32      nmrr;
  uint32      cbar_addr;
  SMMU_HAL_CBARn_t          cbar;
  uint32      cbfrsynra_addr;
  uint32      cbfrsynra;
} fault_regs_dump;

/** The fault values from the global space 0 fault registers can be
 *  retrieved with SMMU_DUMP_GLOBAL_FAULT as the ctxBankNumber
 *  in SMMU_DumpCBFaultRegs(). */
typedef struct
{
  uint32      dump_size;
  // fault info from secure address space
  uint32      gfar0_addr;
  uint32      gfar0;
  uint32      gfar1_addr;
  uint32      gfar1;
  uint32      gfsr_addr;
  SMMU_HAL_GFSR_t      gfsr;
  uint32      gfsynr0_addr;
  uint32      gfsynr0;
  uint32      gfsynr1_addr;
  uint32      gfsynr1;
  uint32      gfsynr2_addr;
  uint32      gfsynr2;
  // fault info from non-secure address space
  uint32      nsgfar0_addr;
  uint32      nsgfar0;
  uint32      nsgfar1_addr;
  uint32      nsgfar1;
  uint32      nsgfsr_addr;
  SMMU_HAL_NSGFSR_t      nsgfsr;
  uint32      nsgfsynr0_addr;
  uint32      nsgfsynr0;
  uint32      nsgfsynr1_addr;
  uint32      nsgfsynr1;
  uint32      nsgfsynr2_addr;
  uint32      nsgfsynr2;
} global_fault_regs_dump;

/* ----------------------------------------------------------------------------
** Interfaces
** ------------------------------------------------------------------------- */
/**
@brief Initialize TZ SMMU driver

   This function initializes the TZ SMMU driver.

@return 0 if successful; otherwise non-zero value
*/
int SMMU_TZ_Init( void );

/**
@brief Validate initialization of the SMMU device 

   This function validates the initialization of the specified SMMU device.

@param[in] smmuDeviceID - SMMU device ID to be validated

@return SMMU_ERR_OK if successful; otherwise non-zero SMMUResult_t enum value

@sa SMMU_InitInstance()
*/
SMMUResult_t SMMU_ValidateInstanceInitialized( SMMUDeviceId_t smmuDeviceID );

/**
@brief Map address range

   The purpose of this function is to map the address range using the
   short descriptor translation format.  Stage 1 translation uses
   short descriptor.

   This is the legacy function for short descriptor translation format.
   This function will be deprecated once the SMMU_Unified_MapAddressRange
   function handles the short descriptor translation format.

@param[in] smmuDeviceID - SMMU device ID to have the address mapped
@param[in] ctxBankNumber - context bank number to have the address mapped
@param[in] L1PageTableBase - pointer to the level 1 page table base address
                             This base address is ignored for stage 2 mapping.
@param[in] L2PageTableBase - pointer to the level 2 page table base address
                             This base address is ignored for stage 2 mapping.
@param[in] attr - pointer to the structure of the mapping attributes info
@param[in] smmu_on - indicates SMMU power and clock have been turned on

@return SMMU_ERR_OK if successful; otherwise non-zero SMMUResult_t enum value
*/
SMMUResult_t SMMU_MapAddressRange
  ( SMMUDeviceId_t smmuDeviceID,
    uint32 ctxBankNumber,
    uint32 *L1PageTableBase,
    uint32 *L2PageTableBase,
    map_attributesType *attr,
    boolean smmu_on );

/**
@brief Map address range

   This purpose of this function is to map the address range for stage 2
   LPAE translation.

   This function also serves as a unified function of handling mapping of
   the address range for short descriptor translation format, stage 1 and
   stage 2 LPAE translation format in the future.  The legacy
   SMMU_MapAddressRange function will be deprecated.

@param[in] smmuDeviceID - SMMU device ID to have the address mapped
@param[in] ctxBankNumber - context bank number to have the address mapped
@param[in] attr - pointer to the structure of the mapping attributes info

@return SMMU_ERR_OK if successful; otherwise non-zero SMMUResult_t enum value
*/
SMMUResult_t SMMU_Unified_MapAddressRange
  ( SMMUDeviceId_t smmuDeviceID,
    uint32 ctxBankNumber,
    const smmu_map_attributesType *attr );

/**
@brief Unmap address range

   The purpose of this function is to unmap the address range using the
   short descriptor translation format.  Stage 1 translation uses short
   descriptor.

   This is the legacy function for short descriptor translation format.
   This function will be deprecated once the SMMU_Unified_UnmapAddressRange
   function handles the short descriptor translation format.

@param[in] smmuDeviceID - SMMU device ID to have the address unmapped
@param[in] ctxBankNumber - context bank number to have the address unmapped
@param[in] L1PageTableBase - pointer to the level 1 page table base address
                             This base address is ignored for stage 2 unmapping.
@param[in] virt_addr - virtual address to be unmapped
@param[in] size - size to be unmapped
@param[in] smmu_on - indicates SMMU power and clock have been turned on

@return SMMU_ERR_OK if successful; otherwise non-zero SMMUResult_t enum value
*/
SMMUResult_t SMMU_UnmapAddressRange
  ( SMMUDeviceId_t smmuDeviceID,
    uint32 ctxBankNumber,
    uint32 *L1PageTableBase,
    uint32 virt_addr,
    uint32 size,
    boolean smmu_on );

/**
@brief Unmap address range

   The purpose of this function is to unmap the address range for stage 2
   LPAE translation.

   This function also serves as a unified function of handling unmapping of
   the address range for short descriptor translation format, stage 1 and
   stage 2 LPAE translation format in the future.  The legacy
   SMMU_UnmapAddressRange function will be deprecated.

@param[in] smmuDeviceID - SMMU device ID to have the address unmapped
@param[in] ctxBankNumber - context bank number to have the address unmapped
@param[in] virt_addr - virtual address to be unmapped
@param[in] size - size to be unmapped

@return SMMU_ERR_OK if successful; otherwise non-zero SMMUResult_t enum value
*/
SMMUResult_t SMMU_Unified_UnmapAddressRange
  ( SMMUDeviceId_t smmuDeviceID,
    uint32 ctxBankNumber,
    uint64 virt_addr,
    uint64 size );

/**
@brief Configure context bank translation

   The purpose of this function is to configure the context bank translation
   info that uses the short descriptor translation format.  Stage 1 translation
   uses short descriptor.

   This is the legacy function for short descriptor translation format.
   This function will be deprecated once the
   SMMU_Unified_InstanceConfigureCBTranslation function handles the context
   bank translation configuration for short descriptor translation format.

@param[in] smmuDeviceID - SMMU device ID to be configured
@param[in] L1PageTableBase - pointer to the level 1 page table base address
                             This base address is ignored for stage 2 config.
@param[in] ctxBankNumber - context bank number to be configured
@param[in] enable - whether MMU is enabled

@return SMMU_ERR_OK if successful; otherwise non-zero SMMUResult_t enum value
*/
SMMUResult_t SMMU_InstanceConfigureCBTranslation
  ( SMMUDeviceId_t smmuDeviceID,
    uint32 *L1PageTableBase,
    uint32 ctxBankNumber,
    boolean enable );

/**
@brief Configure context bank translation

   The purpose of this function is to configure the context bank translation
   info.

   This function also serves as a unified function of configuring the context
   bank translation for short descriptor translation format, stage 1 and
   stage 2 LPAE translation format in the future.  The legacy
   SMMU_InstanceConfigureCBTranslation function will be deprecated.

@param[in] smmuDeviceID - SMMU device ID to be configured
@param[in] ctxBankNumber - context bank number to be configured
@param[in] enable - whether MMU is enabled

@return SMMU_ERR_OK if successful; otherwise non-zero SMMUResult_t enum value
*/
SMMUResult_t SMMU_Unified_InstanceConfigureCBTranslation
  ( SMMUDeviceId_t smmuDeviceID,
    uint32 ctxBankNumber,
    boolean enable );

/**
@brief Invalidate all TLB entries of the context bank 

   The purpose of this function is to invalidate all TLB entries of the
   context bank.

@param[in] smmuDeviceID - SMMU device ID to be invalidated
@param[in] ctxBankNumber - context bank number to be invalidated

@return SMMU_ERR_OK if successful; otherwise non-zero SMMUResult_t enum value
*/
SMMUResult_t SMMU_InvalidateCBTLBAll
  ( SMMUDeviceId_t smmuDeviceID,
    uint32 ctxBankNumber );

/**
@brief Dump the context bank fault syndrome registers

   The purpose of this function is to dump the context bank fault syndrome
   registers.

@param[in] smmuDeviceID - SMMU device ID to be dumped
@param[in] ctxBankNumber - context bank number to be dumped
                           If ctxBankNumber is SMMU_DUMP_GLOBAL_FAULT,
                           values of global space 0 fault registers will be
                           retrieved.
@param[out] buffer - pointer to buffer to get the registers info
@param[in] len - length of the buffer

@return SMMU_ERR_HAL_SUCCESS if successful;
        otherwise non-zero SMMUResult_t enum value
*/
SMMUResult_t SMMU_DumpCBFaultRegs
  ( SMMUDeviceId_t smmuDeviceID,
    uint32 ctxBankNumber,
    uint32 *buffer,
    int32 len );

/**
@brief Clear the fault status register

   The purpose of this function is to clear the fault status register.

@param[in] smmuDeviceID - SMMU device ID to have the FSR cleared
@param[in] ctxBankNumber - context bank number to have the FSR cleared
*/
void SMMU_ClearFaultRegs
  ( SMMUDeviceId_t smmuDeviceID,
    uint32 ctxBankNumber );

/**
@brief Check if the inquired physical address is mapped

   The purpose of this function is to check if the inquired physical address
   is mapped.  If it is mapped, virtual address \c virtaddr is updated with
   SMMU_ERR_OK return status.  The \c maptype to be inquired is limit to 1MB.

@param[in] smmuDeviceID - SMMU device ID to be inquired
@param[in] ctxBankNumber - context bank number to be inquired
@param[in] L1PageTableBase - pointer to the level 1 page table base address
                             This base address is ignored for stage 2 config.
@param[in] physaddr - physical address to be inquired
@param[in] maptype - mapping type to be inquired
@param[in] smmu_on - indicates SMMU power and clock have been turned on
@param[out] virtaddr - pointer to virtual address to get the mapped virtual
                       address info

@return SMMU_ERR_OK if successful; otherwise non-zero SMMUResult_t enum value
*/
SMMUResult_t SMMU_IsPhysAddrMapped
  ( SMMUDeviceId_t smmuDeviceID,
    uint32 ctxBankNumber,
    uint32 *L1PageTableBase,
    uint32 physaddr,
    mappingType maptype,
    boolean smmu_on,
    uint32 *virtaddr );

/**
@brief Change the Secure status for the said BLSP pipe. This is used for support Dynamic Dual EE mode.

   The purpose of this function is to enable/disable the setting responsbile
   for securing the pass through transactions. If it was able to set the configuration
   with that in the arguments then the API returns with SMMU_ERR_OK return status.  

@param[in] pipeIndex  - BLSP pipe Index to be operated
@param[in] secState     - indicates if we are to enable/diable Secure Status.
                           0 -- sets the transactions as non-secure
					       1 -- sets the transactions as secure

@return SMMU_ERR_OK if successful; otherwise non-zero SMMUResult_t enum value
*/
SMMUResult_t SMMU_BLSPChangeSecureStatus
(
  smmu_blsp_pipe_t pipeIndex,
  boolean secState
);

#ifdef __cplusplus
}
#endif

#endif /* SMMUTRANSLATION_H */
