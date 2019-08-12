#ifndef DCVS_NV_H
#define DCVS_NV_H
/*============================================================================
  @file dcvs_nv.h

  This header provides a variety of preprocessor definitions, type, data, and
  function declarations -- all intended for INTERNAL DCVS USE ONLY.
  
  If you are not a part of the DCVS module, you should not be using this file.

  This file defines the common, and os specific NV implementation in addition 
  to NV Parameter definitions and types.

  Copyright (c) 2010-2011 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
============================================================================*/
/* $Header: //components/rel/core.mpss/3.5.c12.3/power/dcvs/inc/dcvs_nv.h#1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "DALStdDef.h"
#include "BuildFeatures.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

// Target specific DCVS NV Parameters can apply to either on specific
// core, or to all cores.  
// These values are used in the coreId parameter of the
// DCVS_NV_PARAM_LIST structure defined below.
// 
#define DCVS_NV_CORE_ID_ALL   -1
#define DCVS_NV_CORE_ID_0      0
#define DCVS_NV_CORE_ID_1      1
// Add here to the core id list as necessary.  The ID must match the
// value for any real core.

#ifdef FEATURE_DCVS_LIMITED_EFS
/* When full EFS support is not available, limit strings to
 * 5 chars (+1 for '\0'). This is necessary since efs_get() expects the
 * exact file length to be provided to it. */
#define DCVS_NV_STRING_LEN_LIMIT (5+1)

#else
/* Based on EFS limit after which the stored item can blowup into a "regular"
   EFS file. */
#define DCVS_NV_STRING_LEN_LIMIT (106)

#endif

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

// NV Parameters can only be
// one of these types.
// Used in parameter definition.
typedef enum
{
  DCVS_NV_PARAM_NULL,
  DCVS_NV_PARAM_UINT32,
  DCVS_NV_PARAM_STRING,
} DCVS_NV_PARAM_TYPE;

// NV Parameter Values are in this union,
// and correspond to the types above.
// Used to set the default value in parameter
// definition.
typedef union
{
  void * pValue;
  uint32 uValue;
  const char * sValue;
} DCVS_NV_PARAM_VALUE;

// NV Parameter Entries are the combination of a name,
// a type, and a defaultValue.
// Used to define each parameter in a list of
// parameters.
typedef struct
{
  const char * name;
  const DCVS_NV_PARAM_TYPE type;
  const DCVS_NV_PARAM_VALUE defaultValue;
} DCVS_NV_PARAM_ENTRY;

// This macro is used to end an array of DCVS_NV_PARAM_ENTRY.
// 
#define DCVS_NV_PARAM_LIST_END  {NULL, DCVS_NV_PARAM_NULL, {0}}

// NV Parameter Lists are used to define target
// specific NV Parameter values.
// 
typedef struct
{
  const char * name;                         // The subpath name these parameters will 
                                             // be put under.
  const int coreId;                          // The core these parameters will apply 
                                             // to.
  const DCVS_NV_PARAM_ENTRY * paramList;     // Pointer to a NULL terminated array of 
                                             // DCVS_NV_PARAM_ENTRY.
} DCVS_NV_PARAM_LIST;

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/**
 @brief DcvsNvInitialize - Initialize NV items on the target.

 Initialize all DCVS NV items on the target.  If they don't
 exist, create them.

 @return None
 
 */
void DcvsNvInitialize(void);

// ToDo: Do we need a function to initialize the defaults for an
// externally provided list of default parameters?
// DcvsNvInitialize handles Algorithms, Target Specific Defaults,
// Startup, and Monitor.
// At least the monitor could benefit from designating its own 
// parameter list.  Startup probably could also.
// 

/**
 @brief DcvsNvParamStringRead - Read one string parameter.

 Read a string NV parameter for the given core and algorithm.
 This function can only fail if the entry passed is invalid.  It
 will read from NV in this order:
   CoreN
   CoreAll
   Default
   entry defaultValue parameter
 
 @param coreId : The core we're reading the parameter for
 @param alg : The name of the algorithm whose parameter we are
            reading.
 @param entry      : The NV Parameter to read
 @param outStr : The output string.
 @param outStrLen : The length of the output string.
 @return None
 
 */
void DcvsNvParamStringRead(int coreId, 
                           const char * alg, 
                           const DCVS_NV_PARAM_ENTRY * entry, 
                           char * outStr, 
                           uint32 outStrLen);

/**
 @brief DcvsNvParamUint32Read - Read one uint32 parameter.

 Read a uint32 NV parameter for the given core and algorithm.
 This function can only fail if the entry passed is invalid.  It
 will read from NV in this order:
   CoreN
   CoreAll
   Default
   entry defaultValue parameter
 
 @param coreId : The core we're reading the parameter for
 @param alg : The name of the algorithm whose parameter we are
            reading.
 @param entry      : The NV Parameter to read
 @param outVal : The output uint32.
 @return None
 
 */
void DcvsNvParamUint32Read(int coreId, 
                           const char * alg, 
                           const DCVS_NV_PARAM_ENTRY * entry, 
                           uint32 * outVal);

/*----------------------------------------------------------------------------
 * Functions below this point should ONLY be used by the generic DCVS NV 
 * code.
 * -------------------------------------------------------------------------*/

/**
 @brief DcvsNvJoin - Join different elements in a path.

 This function joins different elements in a path with the os
 specific joining character.

 @param path       : The output string.
 @param pathLength : The length of the output string.
 @return None
 
 */
void DcvsNvJoin(char * path, uint32 pathLength, ...);


/**
 @brief DCVSNvDefaultParamsInit - Initialize the default
        parameter set for a subsection's list of parameters.

 This function initializes the default NV memory for a provided
 list of parameters.  Each subsection can provide a list of
 parameters with names and default values.  This function
 creates entries for those parameters under the Default section.

 @param alg  : The subsection name.
 @param list : The parameters that apply to the subsection.
 
 */
void DCVSNvDefaultParamsInit (const char * alg, 
                              const DCVS_NV_PARAM_ENTRY * list);

/**
 @brief DcvsNvStringRead - Read a string from NV.

 This function Reads a string from NV.

 @param path       : The location of the parameter to read.
 @param paramName : The name of the parameter to read.
 @param paramString : The output string.
 @param paramStringLength : The length of the output string.
 @return TRUE if the read was successful.
 
 */
DALBOOL DcvsNvStringRead(const char * path, 
                         const char * paramName, 
                         char * paramString,
                         uint32 paramStringLength);

/**
 @brief DcvsNvStringWrite - Write a string to NV.

 This function writes a string to NV.

 @param path       : The location of the parameter to write.
 @param paramName : The name of the parameter to write.
 @param paramString : The string to write.
 @return TRUE if the write was successful.
 
 */
DALBOOL DcvsNvStringWrite(const char * path, 
                          const char * paramName, 
                          const char * paramString);

/**
 @brief DcvsNvUint32Read - Read a uint32 from NV.

 This function Reads a uint32 from NV.

 @param path       : The location of the parameter to read.
 @param paramName : The name of the parameter to read.
 @param paramValue : The output parameter.
 @return TRUE if the read was successful.
 
 */
DALBOOL DcvsNvUint32Read(const char * path, 
                         const char * paramName, 
                         uint32 * paramValue);

/**
 @brief DcvsNvUint32Write - Write a uint32 to NV.

 This function writes a uint32 to NV.

 @param path       : The location of the parameter to write.
 @param paramName : The name of the parameter to write.
 @param paramValue : The value to write.
 @return TRUE if the write was successful.
 
 */
DALBOOL DcvsNvUint32Write(const char * path, 
                          const char * paramName, 
                          uint32 paramValue);


/**
 @brief dcvsTargetNvDefaults - Array of target specific default
        values.

 This extern defines an array of target specific NV parameters
 that must exist on every target.  These values are initialized
 by the DCVS NV Initialization function.
 */
extern const DCVS_NV_PARAM_LIST dcvsTargetNvDefaults[];

#endif /* DCVS_NV_H */




