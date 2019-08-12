#ifndef __NOC_ERROR_H__
#define __NOC_ERROR_H__
/*============================================================================

FILE:      NOC_Error.h

DESCRIPTION: APIs and data structure declarations 
             for NOC Error Handler

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A
 
Edit History

//#CHANGE - Update when put in the depot
$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/buses/icb/src/common/NOC_error.h#1 $
$DateTime: 2016/06/17 14:31:11 $
$Author: pwbldsvc $
$Change: 10727476 $ 

When        Who    What, where, why
----------  ---    ----------------------------------------------------------- 
2013/05/10  pm     Added NOC_ERRLOG6 macro to support FlexNoc 2.8
2012/10/03  av     Created

                Copyright (c) 2012 Qualcomm Technologies Incorporated.
                             All Rights Reserved.
                          QUALCOMM Proprietary/GTDR
============================================================================*/
#include "com_dtypes.h"


/*============================================================================
                          DEFINEs/MACROs
============================================================================*/

#define BIMC_MAX_NUM_REGS 20

#define BIT_FLAG(x) ((uint32)(1 << x))

/** ERRLOG flags for log filter */
#define NOC_ERRLOG0   BIT_FLAG(0)
#define NOC_ERRLOG1   BIT_FLAG(1)
#define NOC_ERRLOG2   BIT_FLAG(2)
#define NOC_ERRLOG3   BIT_FLAG(3)
#define NOC_ERRLOG4   BIT_FLAG(4)
#define NOC_ERRLOG5   BIT_FLAG(5)
#define NOC_ERRLOG6   BIT_FLAG(6)
#define NOC_ERRFATAL  BIT_FLAG(30)

/* Macro for Slave info initialization */
#define NOCERR_INFO(name, intr_vector, intr_en, log_filter)  \
                      {                                      \
                        #name,                               \
                        (void*)NOCERR_##name##_BASE_ADDR,    \
                        intr_en,                             \
                        intr_vector,                         \
                        log_filter,                          \
                      }

/*============================================================================
                          TYPE DEFINITION
============================================================================*/
/**
 * NOC error Info data type
 */
typedef struct
{
    char* name;                     /**< NOC name */
    void* base_addr;                /**< NOC base address */
    uint8 intr_enable;              /**< NOC interrupt enable */
    uint8 intr_vector;              /**< NOC interrupt vector */
    uint32 errlog_filter;           /**< NOC ErrLog <n>-bitfield to filter ERRLog<n> */    
}NOCERR_info_type;

/**
 * NOC Error Property Data type
 */
typedef struct
{
    uint8 len;                        /**< Length of cfgdata array */
    NOCERR_info_type* NOCInfo;        /**< Pointer to cfgdata array*/
    char **BIMC_reg_names;
    uint32 *BIMC_reg_addr;
    uint8 BIMC_num_reg;
}NOCERR_propdata_type;

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
void NOC_Error_Init(void);

#endif /* __NOC_ERROR_H__ */
