#ifndef __BIMC_H__
#define __BIMC_H__
/*============================================================================

FILE:      BIMC.h

DESCRIPTION: APIs and data structure declarations 
             for NOC Error Handler

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A
 
Edit History

//#CHANGE - Update when put in the depot
$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/buses/icb/src/common/BIMC_error.h#1 $
$DateTime: 2016/06/17 14:31:11 $
$Author: pwbldsvc $
$Change: 10727476 $ 

When        Who    What, where, why
----------  ---    ----------------------------------------------------------- 
2014/06/06  tb     Created

                Copyright (c) 2014,2015 Qualcomm Technologies Incorporated.
                             All Rights Reserved.
                          QUALCOMM Proprietary/GTDR
============================================================================*/
#include "com_dtypes.h"


/*============================================================================
                          DEFINEs/MACROs
============================================================================*/

#define REG_NOT_AVAIL 0xFFFF

/*============================================================================
                          TYPE DEFINITION
============================================================================*/
/**
 * BIMC Info data type
 */
typedef struct
{
    char* name;                      /**< BIMC name */
    uint32 base_addr;                /**< BIMC base address */
    uint8 turn_on_intr;              /**< BIMC turn on interrupt */
    uint8 err_fatal;                 /**< BIMC error fatal */
    uint16 intr_status;              /**< BIMC interrupt status register offset */
    uint16 intr_clear;               /**< BIMC interrupt clear register offset */
    uint16 intr_enable;              /**< BIMC interrupt enable register offset */ 
}BIMC_BRIC_info_type;

/**
 * BIMC Info data type
 */
typedef struct
{
    char* name;                       /**< BIMC name */
    uint32 base_addr;                 /**< BIMC base address */
    uint8 turn_on_intr;               /**< BIMC turn on interrupt */
    uint8 err_fatal;                  /**< BIMC error fatal */
    uint16 intr_status;               /**< BIMC interrupt status register offset */
    uint16 intr_clear;                /**< BIMC interrupt clear register offset */
    uint16 intr_enable;               /**< BIMC interrupt enable register offset */
    uint16 intr_status0;              /**< BIMC interrupt status0 register offset */
    uint16 intr_status1;              /**< BIMC interrupt status1 register offset */
    uint16 intr_status2;              /**< BIMC interrupt status2 register offset */
}BIMC_DEFAULT_info_type;

/**
 * BIMC Info data type
 */
typedef struct
{
    char* name;                       /**< BIMC name */
    uint32 base_addr;                 /**< BIMC base address */
    uint8 turn_on_intr;               /**< BIMC turn on interrupt */
    uint8 err_fatal;                  /**< BIMC error fatal */
    uint16 intr_status;               /**< BIMC interrupt status register offset */
    uint16 intr_clear;                /**< BIMC interrupt clear register offset */
    uint16 intr_enable;               /**< BIMC interrupt enable register offset */
    uint16 intr_addr;                 /**< BIMC interrupt addr register offset */
    uint16 intr_addr1;                /**< BIMC interrupt addr1 register offset */
    uint16 intr_packet0;              /**< BIMC interrupt packet0 register offset */
    uint16 intr_packet1;              /**< BIMC interrupt packet1 register offset */
    uint16 intr_packet2;              /**< BIMC interrupt packet2 register offset */
}BIMC_DDR_SCMO_info_type;

/**
 * NOC Error Property Data type
 */
typedef struct
{
    uint32 vector;                               /**< Vector number for BIMC interrupt */
    uint8 BRIC_len;                              /**< Length of BRIC info array */
    const BIMC_BRIC_info_type* BRIC_info;              /**< Pointer to BRIC info array */
    uint8 DEFAULT_len;                           /**< Length of DEFAULT info array */
    const BIMC_DEFAULT_info_type* DEFAULT_info;        /**< Pointer to DEFAULT info array */
    uint8 DDR_SCMO_len;                          /**< Length of DDR SCMO array */
    const BIMC_DDR_SCMO_info_type* DDR_SCMO_info;      /**< Pointer to DDR SCMO array */
}BIMC_propdata_type;

/*============================================================================
                                 FUNCTIONS
============================================================================*/

//*============================================================================*/
/**
@brief 
      Performs initialization for NOC error handler.
      It enables interrupts required to handle NOC errors.
 
@param[in]  None.

@return    
      None.

@dependencies
      None.
 
@sideeffects 
      None. 
*/ 
/*============================================================================*/
void BIMC_Error_Init( void );

#endif /* __BIMC_H__ */
