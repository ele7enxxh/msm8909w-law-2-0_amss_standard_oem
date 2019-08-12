#ifndef __ICBCFG_H__
#define __ICBCFG_H__
/*============================================================================

FILE:      icbcfg.h

DESCRIPTION: Function and data structure declarations for ICB CFG driver Interface

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A
 
Edit History

$Header: //components/rel/core.mpss/3.5.c12.3/buses/api/icb/icbcfg.h#1 $
$DateTime: 2016/12/13 07:59:23 $
$Author: mplcsds1 $
$Change: 11985146 $ 

When        Who    What, where, why
----------  ---    ----------------------------------------------------------- 
2014/06/17  vg     Added this file to support ICB CFG Query in MBA
2014/04/07  vg     Added icbcfg file to support  64Gb x 4POR DDR in modem
2013/10/10  sds    BIMC remap support now checks for hardware support explicitly
2013/07/23  sds    Added BIMC remap support
2012/03/27  av     Added HWIO macro support 
2012/02/07  av     Created

         Copyright (c) 2012-2014 QUALCOMM Technologies Incorporated.
                          All Rights Reserved.
                       QUALCOMM Proprietary/GTDR
============================================================================*/

#include "HALhwio.h"
#include "ddr_common.h"
#include <stdbool.h>

/*============================================================================
                          DEFINEs/MACROs
============================================================================*/

//*============================================================================*/
/**
@brief 
      ICBCFG_HWIO*() macros are used to define register configuration using HWIO
      macros.
      ICBCFG_HWIO_DW() is used for 32-bit register configuration.
      ICBCFG_HWIO_W()  is used for 16-bit register configuration.
      ICBCFG_HWIO_B()  is used for 8-bit  register configuration.
 
@param[in]  reg   Register name as specified in SWI document. 
@param[in]  field Register field name as specified in SWI document. 
@param[val] val   data value to be configured in specified register.

@return    
      icbcfg_data_type structure.

@dependencies
      Make sure required HWIO macros are defined in included HWIO header files.
 
@sideeffects 
      None. 
*/ 
/*============================================================================*/
#define ICBCFG_HWIO_DW(reg, field, val) {ICBCFG_32_BIT_REG,      \
                                         (void*)HWIO_ADDR(reg),  \
                                         HWIO_FMSK(reg, field),  \
                                         HWIO_RMSK(reg),         \
                                         (uint32)(val) << HWIO_SHFT(reg, field) \
                                        }

#define ICBCFG_HWIO_W(reg, field, val)  {ICBCFG_16_BIT_REG,      \
                                         (void*)HWIO_ADDR(reg),  \
                                         HWIO_FMSK(reg, field),  \
                                         HWIO_RMSK(reg),         \
                                         (uint32)(val) << HWIO_SHFT(reg, field) \
                                        }

#define ICBCFG_HWIO_B(reg, field, val)  {ICBCFG_8_BIT_REG,       \
                                         (void*)HWIO_ADDR(reg),  \
                                         HWIO_FMSK(reg, field),  \
                                         HWIO_RMSK(reg),         \
                                         (uint32)(val) << HWIO_SHFT(reg, field) \
                                        }


//*============================================================================*/
/**
@brief 
      ICBCFG_RAW_*() macros are used to define register configuration with
      register address and data.
      ICBCFG_RAW_DW() is used for 32-bit register configuration.
      ICBCFG_RAW_W()  is used for 16-bit register configuration.
      ICBCFG_RAW_B()  is used for 8-bit  register configuration.
 
@param[in]  addr   Register address. 
@param[val] data   data value to be written in specified address.

@return    
      icbcfg_data_type structure.

@dependencies
 
@sideeffects 
      None. 
*/ 
/*============================================================================*/
#define ICBCFG_RAW_DW(addr, data)     {ICBCFG_32_BIT_REG,       \
                                       (void*)addr,             \
                                       0xFFFFFFFF,              \
                                       0xFFFFFFFF,              \
                                       data                     \
                                      }
 
#define ICBCFG_RAW_W(addr, data)      {ICBCFG_16_BIT_REG,       \
                                       (void*)addr,             \
                                       0xFFFF,                  \
                                       0xFFFF,                  \
                                       data                     \
                                      }

#define ICBCFG_RAW_B(addr, data)      {ICBCFG_8_BIT_REG,        \
                                       (void*)addr,             \
                                       0xFF,                    \
                                       0xFF,                    \
                                       data                     \
                                      } 
/*
* Default configuration for initial config data. Default config is added 
* to device property structure as empty struct property is not allowed. 
*/
#define ICBCFG_DEFAULT()              {ICBCFG_DEFAULT,          \
                                       NULL,                    \
                                       0x0,                     \
                                       0x0,                     \
                                       0x0                      \
                                      }  

/*============================================================================
                          TYPE DEFINITION
============================================================================*/

/**
 * ICBcfg Register types
 */
typedef enum 
{
    ICBCFG_INVALID_TYPE = 0,    /**< Invalid type */
    ICBCFG_DEFAULT,             /**< Default Config  */
    ICBCFG_32_BIT_REG,          /**< 32-bit register */
    ICBCFG_16_BIT_REG,          /**< 16-bit register */
    ICBCFG_8_BIT_REG,           /**< 8-bit  register */
    /* Add new types here */

    /* Force 32-bit */
    icbcfg_config_type_Placeholder = 0x7FFFFFFF
}icbcfg_config_type;


/**
 * ICBcfg configuration data type
 */
typedef struct 
{
    icbcfg_config_type cfg_type;     /**< Config type */
    void* addr;                      /**< Register address */
    unsigned int bmask;              /**< Bit mask for field */   
    unsigned int rmask;              /**< Register mask */
    unsigned int data;               /**< Configuration data */
}icbcfg_data_type;


/**
 * ICBcfg device property type
 */
typedef struct 
{
    unsigned int len;                /**< Length of the config  data array */
    icbcfg_data_type * cfg_data;     /**< Pointer to config data array */
}icbcfg_prop_type;


/**
 * ICBCFG Error types.
 */ 
typedef enum
{
  ICBCFG_SUCCESS             = 0,          /**< Success */
  ICBCFG_ERROR_INVALID_PARAM,              /**< Invalid Parameters */
  ICBCFG_ERROR_INIT_FAILURE,               /**< Initialization Failure */
  ICBCFG_ERROR_EXHAUSTED_SEGMENTS,         /**< Out of slave segments. */
  ICBCFG_ERROR_UNSUPPORTED,                /**< This operation unsupported on this target. */
  /* Add new errors here */         

  ICBCFG_ERROR_SIZE = 0x7FFFFFFF, /* force 32-bit enum */
  ICBCFG_ERROR = -1
}icbcfg_error_type;

/**
 * ICBCFG remap types
*/
typedef enum
{
  ICBCFG_REMAP_INTERLEAVE_DEFAULT = 0,  /**< Remap all memory in the region. */
  ICBCFG_REMAP_INTERLEAVE_ODD,          /**< Remap ODD 1k pages in the region. */
  ICBCFG_REMAP_INTERLEAVE_EVEN,         /**< Remap EVEN 1k pages in the region. */
  ICBCFG_REMAP_INTERLEAVE_COUNT
} icbcfg_interleave_type;

typedef struct
{
  uint64                 src_addr;     /**< The source address of the region to remap (address at the memory controller) */
  uint64                 size;         /**< The size of the region to remap */
  uint64                 dest_addr;    /**< The destination address of the region to remap (desired address in system memory map) */
  icbcfg_interleave_type interleaved;  /**< Type of interleaving on the memory region */
  bool                   deinterleave; /**< Deinterleave as well as remap? */
} icbcfg_remap_info_type;

/**
 * ICBCFG memory region types
*/
typedef struct
{
  uint64 base_addr;
  uint64 size;
  bool   interleaved;
} region_type;

/* DDR channel definitions */
#define MAX_REGIONS 4
typedef struct
{
  region_type regions[MAX_REGIONS];
} regions_type;

/* DDR slave region configuration */
#define MAX_CHANNELS 2
typedef struct
{
  regions_type channels[MAX_CHANNELS];
} icb_mem_map_type;

/*============================================================================
                                 FUNCTIONS
============================================================================*/

//*============================================================================*/
/**
@brief 
      Initializes ICB config driver. It reads ICB configuration data (register 
      address and config data) from DAL Device Property file and configures 
      ICB registers.
 
@param[in]  dev  DAL string device ID.

@return    
      icbcfg_error_type indicating success or error.

@dependencies
      None.
 
@sideeffects 
      None. 
*/ 
/*============================================================================*/
icbcfg_error_type ICB_Config_Init( const char * dev );

//*============================================================================*/
/**
@brief 
      Configures DDR slave segments in BIMC. It reads ICB configuration data (number
      of channels, maximum memory map, etc) from DAL Device Property file and configures 
      ICB registers.
 
@param[in]  dev   DAL string device ID.
@param[in]  info  DDR channel region configuration

@return    
      icbcfg_error_type indicating success or error.

@dependencies
      None.
 
@sideeffects 
      None. 
*/ 
/*============================================================================*/
icbcfg_error_type ICB_Segments_Init( const char * dev, ddr_info * info );

//*============================================================================*/
/**
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
/*============================================================================*/
icbcfg_error_type ICB_Remap( const char * dev, ddr_info * info, icbcfg_remap_info_type * remap );

//*============================================================================*/
/**
@brief 
      Returns the current memory map.

@param[in]    dev  DAL string device ID.
@param[inout] info The information about the DDR configuration

@return    
      icbcfg_error_type indicating success or error.

@dependencies
      None.
 
@sideeffects 
      None. 
*/ 
/*============================================================================*/
icbcfg_error_type ICB_Get_Memmap( const char * dev, icb_mem_map_type * info );

#endif /* ICBCFG_H */
