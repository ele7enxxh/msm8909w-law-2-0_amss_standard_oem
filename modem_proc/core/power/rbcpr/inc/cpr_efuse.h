#ifndef CPR_EFUSE_H
#define CPR_EFUSE_H
/*============================================================================
@file cpr_efuse.h

Data types and interfaces needed to read efused configuration data.

Copyright © 2013-2015 QUALCOMM Technologies, Incorporated.
All Rights Reserved.
QUALCOMM Confidential and Proprietary.

$Header: //components/rel/core.mpss/3.5.c12.3/power/rbcpr/inc/cpr_efuse.h#1 $
=============================================================================*/
#include "DALSysTypes.h"
#include "cpr_internal.h"

// Read contents at non-NULL register addr, apply mask and right-shift
#define CPR_EFUSE_IN(field) \
  ( (field.addr) ? \
    ( ( (*((uint32*)field.addr)) & field.mask ) >> field.shift ) : \
    0 )
  

/**
 * Describes a eFuse bitfield
 */
typedef struct
{
  uint32 addr;  // Register address of the bitfield
  uint32 mask;  // Bitfield mask
  uint32 shift; // Number of bits to shift the results right by
} CprEfuseBitFieldType;


/**
 * Configuration for a voltage corner
 */
typedef struct
{
  //for which mode is this fuse data for
  uint32 modeEnum;
  // Unused - Ring oscillator for this corner
  CprEfuseBitFieldType roscSel;
  
  // Initial offset to apply to the ceiling voltage (derived from
  // characterization) Lower bits
  CprEfuseBitFieldType targetVsteps0;

  // Initial offset to apply to the ceiling voltage (derived from
  // characterization) Upper bits
  CprEfuseBitFieldType targetVsteps1;

  // Number of bits to shift targetVsteps1 left before ORing targetVsteps0
  // to get the target steps
  uint32               shiftLeftBy;

  // Unused - Step quotient for the corner
  CprEfuseBitFieldType stepQuot;
  // Unused - SW quotient to use at Vmin
  CprEfuseBitFieldType quotVmin;
} CprEfuseVModeCfgType;


/**
 * Efuse database type
 */
typedef struct
{
  // Unused
  CprEfuseBitFieldType quotInd;

  // Disable CPR if set
  CprEfuseBitFieldType disCpr;

  // Voltage mode/corner specific settings
  CprEfuseVModeCfgType fuseVmodeCfgPtrs[CPR_MAX_VMODE_COUNT];
} CprEfuseDatabaseType;


/**
 * Primary and 2ndary locations of CPR efuse settings.
 */
typedef struct
{
  // Selects whether to use the primary or 2ndary efuse location
  CprEfuseBitFieldType redundancySel;

  // Number of fuse entries
  uint32 numFuseEntries;

  // Primary database
  CprEfuseDatabaseType primaryDb;

  // Secondary database
  CprEfuseDatabaseType redundantDb;
} CprEfuseMasterDatabaseType;


/**
 * <!-- CprEfuseGetDataFromFuses -->
 *
 * @brief Reads CPR configuration data blown into efuses.
 * 
 * @param fuseDb : Pointer to the database of efuse registers and fields from
 * which CPR data can be read.
 * @param vmodeCfg : Pointer to an array of structs in which the efused data
 * will be stored. The array size must match CPR_MAX_VMODE_COUNT.
 * 
 * @return TRUE if efuses have indicated that CPR should be disabled; 
 * FALSE otherwise.
 */
boolean CprEfuseGetData( CprEfuseMasterDatabaseType* fuseDb,
                         CprVmodeBoundsAndOffsetCfgType** vmodeBoundsAndOffsetCfg);

#endif // CPR_EFUSE_H

