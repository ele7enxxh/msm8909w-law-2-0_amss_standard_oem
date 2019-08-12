#ifndef __ABT_INTERNAL_H__
#define __ABT_INTERNAL_H__
/*============================================================================

FILE:      ABT_internal.h

DESCRIPTION: Common internal function and data structure declarations 
             for AHB timeout driver Interface

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A
 
Edit History

//#CHANGE - Update when put in the depot
$Header: //components/rel/core.mpss/3.5.c12.3/buses/icb/src/common/ABT_internal.h#1 $
$DateTime: 2016/12/13 07:59:23 $
$Author: mplcsds1 $
$Change: 11985146 $ 

When        Who    What, where, why
----------  ---    ----------------------------------------------------------- 
2013/08/30  pm     Renamed ABT_Register_Interrupt() and moved it to ABT_platform.h
2013/04/16  pm     Added ABT_Register_Interrupt()
2012/05/31  av     Created

                Copyright (c) 2013 Qualcomm Technologies Incorporated.
                             All Rights Reserved.
                          QUALCOMM Proprietary/GTDR
============================================================================*/
#include "ABTimeout.h"

/*============================================================================
                          DEFINEs/MACROs
============================================================================*/

/*============================================================================
                          TYPE DEFINITION
============================================================================*/

/**
 * ABT Device Syndrome data type
 */
typedef struct
{
    char*  slave_name;                     /**< ABT slave name             */
    uint32 id;                             /**< Syndrome ID (BID, PID, MID)*/
    uint32 addr0;                          /**< Syndrome Address 0 (lower) */
    uint32 addr1;                          /**< Syndrome Address 1 (upper) */
    uint32 hready;                         /**< Syndrome HREADY for slaves */
    uint32 num_slaves;                     /**< Number of slaves on bus    */
}ABT_syndrome_info_type;


/*============================================================================
                                 FUNCTIONS
============================================================================*/

/*==========================================================================*/
/**
@brief 
      This function reads Interrupt status register and captures syndrome
      registers from corresponding ABT slaves.
 
@param[in]  void* arg context data for client.

@return    
      None.

@dependencies
      None.
 
@sideeffects 
      None. 
*/ 
/*==============================================================================*/
void* ABT_Handle_Interrupt(void* arg);

#endif /* __ABT_INTERNAL_H__ */
