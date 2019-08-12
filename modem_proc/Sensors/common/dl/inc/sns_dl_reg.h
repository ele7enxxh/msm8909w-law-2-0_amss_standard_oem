/*==============================================================================
  @file sns_dl_reg.h

  @brief
  Header file containing the registry message formats pertaining to SNS
  Dynamic Loaded Configuration for Drivers and Algorithms.

  Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
==============================================================================*/
#ifndef SNS_DL_REG_H
#define SNS_DL_REG_H

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

/**< Definition for packed structures */
#if defined __GNUC__
  #define PACK(x)   x __attribute__((__packed__))
#elif defined _WIN32
  #define PACK(x)   x
#endif

/**< Macro to pertaining to string concatenation */
#define ASSERT_CONCAT_(a, b)  a##b
#define ASSERT_CONCAT(a, b)   ASSERT_CONCAT_(a, b)
#define CHECK_SIZE(a, size)   (((a) == (size)))
/**< Macro to fail validation checks during preprocessing */
#define STATIC_ASSERT(e, msg) enum {ASSERT_CONCAT(msg, __LINE__) = 1/(!!(e))}
/**< Retrieve offset of member in a structure */
#define GET_OFFSETOF(type, member)  (((char*)(&((type*)1)->member))-((char*)1))

#ifndef SNS_DYN_CONFIG_REV
  #define SNS_DYN_CONFIG_REV          1 /**< Version of registry message */
#endif

#define SNS_DRIVER_UUID_SZ_V01        16  /**< Size of SNS Driver UUID */
#define SNS_DSO_FILEPATH_SZ_V01       128 /**< DSO filepath */
/**< Load symbol for drivers and algos */
#define SNS_DSO_LOAD_SYM_NAME_SZ_V01  64
#define SNS_REG_CFG_SZ_V01            256 /**< Total size of the registry entry */

/**< All registry entries pertaining to dynamic loading will start off with 
     the elements defined in this macro
*/
#define SMS_REG_DYN_CFG_BASE          uint32_t  rev

#ifdef _WIN32
#pragma pack(push,1)
#endif

/**
  Dynamic Loading Registry Format Base structure. All registry formats will
  start with these members.
*/
typedef PACK(struct)
{
  SMS_REG_DYN_CFG_BASE;   /**< Revision number denoting how registry is packed*/
} sns_reg_dyn_cfg_base_t;

/**
  Dynamic Loading Registry Format version #1. It will be expected that all
  numeric data types will be will be packed in little endian format.
*/
typedef PACK(struct)
{
  /**<  Revision number denoting how registry is packed. This field must always 
        be first member in this structure. */
  SMS_REG_DYN_CFG_BASE;
  /**<  UUID information associated with the driver.
        For SNS algos this field is not used */
  uint8_t   uuid[SNS_DRIVER_UUID_SZ_V01];
  /**<  Path to the SNS driver or algorithm */
  char      full_file_path[SNS_DSO_FILEPATH_SZ_V01];
  /**<  Symbol to be used by the SMGR or SAM FWK to initialize the module */
  char      entry_symbol[SNS_DSO_LOAD_SYM_NAME_SZ_V01];
  /**<  Load attrib associated with a SNS algos. For drivers this is ignored */
  uint32_t  load_attrib;
  /**<  Unused */
  char      reserved[40];
} sns_reg_dyn_cfg_v01_t;

#ifdef _WIN32
#pragma pack(pop)
#endif

/* validation check to ensure size should be the same on all platforms */
STATIC_ASSERT(CHECK_SIZE(sizeof(sns_reg_dyn_cfg_v01_t), 
              (SNS_REG_CFG_SZ_V01)), static_size_check_failed_line_num_);

/** 
  @note: These sizes should be changed to a MAX of all the V0* 
  sizes when newer registry formats are added.
*/

/**< Size of the SNS Driver UUID */
#define SNS_DRIVER_UUID_SZ        SNS_DRIVER_UUID_SZ_V01
/**< Size of the SNS Driver, Algo shared obj file path and name */
#define SNS_DSO_FILEPATH_SZ       SNS_DSO_FILEPATH_SZ_V01
/**< Size of the SNS Driver, Algo load symbol name size */
#define SNS_DSO_LOAD_SYM_NAME_SZ  SNS_DSO_LOAD_SYM_NAME_SZ_V01
/**< Macro to get an offset of a member within a structure */
#define SNS_DSO_UUID_OFFSET       GET_OFFSETOF(sns_reg_dyn_cfg_v01_t, uuid)

typedef struct
{
  /**< UUID information associated with the driver */
  uint8_t   uuid[SNS_DRIVER_UUID_SZ];
  /**< Path and file name of the SNS driver or algorithm */
  char      full_file_path[SNS_DSO_FILEPATH_SZ];
  /**< Symbol to be used by the SMGR or SAM FWK to load and init the module */
  char      entry_symbol[SNS_DSO_LOAD_SYM_NAME_SZ];
  /**< Load attrib associated with a SNS algos. For drivers this is ignored */
  uint32_t  load_attrib;
} sns_reg_dyn_cfg_t;

#ifdef __cplusplus
}
#endif

#endif  //SNS_DL_REG_H
