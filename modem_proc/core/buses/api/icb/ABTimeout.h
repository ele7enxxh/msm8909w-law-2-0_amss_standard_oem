#ifndef __ABTIMEOUT_H__
#define __ABTIMEOUT_H__
/*============================================================================

FILE:      ABTtimeout.h

DESCRIPTION: Function and data structure declarations for AHB timeout driver Interface

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A
 
Edit History

//#CHANGE - Update when put in the depot
$Header: //components/rel/core.mpss/3.5.c12.3/buses/api/icb/ABTimeout.h#1 $
$DateTime: 2016/12/13 07:59:23 $
$Author: mplcsds1 $
$Change: 11985146 $ 

When        Who    What, where, why
----------  ---    ----------------------------------------------------------- 
2014/03/14  ddk    Added support for additional clock voting.
2014/03/10  ddk    Added support for hwio mapping.
2013/11/14  tb     Added support for multiple enable/status registers
2013/04/16  pm     Added interrupt priority
2012/10/04  av     Support for disabling ABT
2012/05/31  av     Created

                Copyright (c) 2013-2014 Qualcomm Technologies Incorporated.
                             All Rights Reserved.
                          QUALCOMM Proprietary/GTDR
============================================================================*/
#include "com_dtypes.h"

/*============================================================================
                          DEFINEs/MACROs
============================================================================*/
/* Macro for Slave info initialization */
#define ABT_SLAVE_INFO(name, clk_name_array, num_of_clk, slave_en, intr_en, timeout_val) \
                      {                                      \
                        #name,                               \
                        ABT_##name##_BMSK,                   \
                        (void*)ABT_##name##_BASE_ADDR,       \
                        clk_name_array,                      \
                        num_of_clk,                          \
                        slave_en,                            \
                        intr_en,                             \
                        timeout_val,                         \
                      }

/*============================================================================
                          TYPE DEFINITION
============================================================================*/

/**
 * ABT configuration Data type
 */
typedef struct
{
    char*  name;                     /**< ABT Slave name */
    uint8  slave_id;                 /**< ABT Slave internal id */
    void*  base_addr;                /**< ABT Slave base address */
    char** clk_name_array;           /**< ABT Slave clk name array */
    uint32 num_of_clk;               /**< Number of clocks to be voted */    
    uint8  slave_enable;             /**< Slave enable */
    uint8  intr_enable;              /**< Slave Interrupt enable */
    uint8  timeout_val;              /**< Slave timeout value */    
}ABT_slave_info_type;

/**
 * ABT Platform HWIO type
 */
typedef struct
{
    char*  name;                /**< Name of region */
    uint8* base_addr;           /**< Base  address */    
}ABT_platform_hwio_info_type;

/**
 * ABT Platform Data type
 */
typedef struct
{
    char*  name;                    /**< Platform name */
    void** intr_map_addr;           /**< ABT Interrupt map reg address */
    void** intr_status_addr;        /**< ABT Interrupt pending status 
                                         regisger address*/
    void*  globa_en_addr;           /**< ABT Slave Global EN reg address */
    uint32 intr_vector;             /**< Summary Interrupt vector */
    uint32 intr_priority;           /**< Interrupt priority */
    uint32 num_status_reg;          /**< Number of ABT Interrupt enable/status registers */
    ABT_platform_hwio_info_type* hwio_map_info;          /**< HWIO map info */    
    uint32  num_hwio_regions;          /**< Number of HWIO region mappings */    
}ABT_platform_info_type;


/**
 * ABT Device Property Data type
 */
typedef struct
{
    uint8 len;                        /**< Length of property data array */
    ABT_slave_info_type* slave_info;  /**< Pointer to cfgdata array*/
    ABT_platform_info_type *platform_info; /**< Pointer to platform info */
}ABT_propdata_type;


/**
 * ICBCFG Error types.
 */ 
typedef enum
{
  ABT_SUCCESS = 0,      				/**< Success */
  ABT_ERROR_INVALID_PARAM,              /**< Invalid Parameters */
  ABT_ERROR_INIT_FAILURE,               /**< Initialization Failure */
  /* Add new errors here */         

  ABT_ERROR_SIZE = 0x7FFFFFFF,          /* force 32-bit enum */
  ABT_ERROR = -1
}ABT_error_type;

/*============================================================================
                                 FUNCTIONS
============================================================================*/

//*============================================================================*/
/**
@brief 
      Initializes AHB Timeout driver. It configures interrupt and timeout values
      for AHB timeout slaves
 
@param[in]  None.

@return    
      None.

@dependencies
      None.
 
@sideeffects 
      None. 
*/ 
/*============================================================================*/
void ABT_Init(void);


#endif /* __ABTIMEOUT_H__ */
