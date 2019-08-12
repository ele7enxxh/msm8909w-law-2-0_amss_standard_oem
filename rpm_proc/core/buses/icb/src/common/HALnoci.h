#ifndef HALNOCI_H
#define HALNOCI_H
/**

@file   HALnoci.h

@brief  Internal header file for HAL NOC.
        
===========================================================================

                             Edit History

$Header: //components/rel/rpm.bf/2.1.1/core/buses/icb/src/common/HALnoci.h#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
08/15/11   sds     Created 

===========================================================================
                  Copyright (c) 2011 Qualcomm Technologies Incorporated.
                           All Rights Reserved.
                        QUALCOMM Proprietary/GTDR
===========================================================================
*/

/* -----------------------------------------------------------------------
** Includes
** ----------------------------------------------------------------------- */
#include "HALnoc.h"

/* -----------------------------------------------------------------------
** Definitions
** ----------------------------------------------------------------------- */
#define NOC_VERIFY_PTR(ptr)                 if(NULL == ptr) return
#define NOC_VERIFY_MASTER_IDX(info,idx)     if(idx >= info->uNumMasters) return
#define NOC_VERIFY_QOS_MASTER_IDX(info,idx) if(idx >= info->uNumQosMasters) return
#define NOC_VERIFY_SLAVE_IDX(info,idx)      if(idx >= info->uNumSlaves) return

#define CEILING_DIV(dividend,divisor)       (((dividend)+(divisor)-1)/(divisor))

#ifndef MIN
#define MIN(a,b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef MAX
#define MAX(a,b) (((a) > (b)) ? (a) : (b))
#endif

/* -----------------------------------------------------------------------
** Types
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Function declarations
** ----------------------------------------------------------------------- */
void noc_Init           ( HAL_noc_InfoType * );
void noc_Reset          ( HAL_noc_InfoType * );
void noc_Save           ( HAL_noc_InfoType * );
void noc_Restore        ( HAL_noc_InfoType * );

/* -----------------------------------------------------------------------
**  Configuration Interfaces
** ----------------------------------------------------------------------- */
void noc_SetQosMode     ( HAL_noc_InfoType *, uint32_t, HAL_noc_QosModeType );
void noc_SetQosPriority ( HAL_noc_InfoType *, uint32_t, HAL_noc_QosModeType, HAL_noc_QosPriorityType * );
void noc_SetQosBandwidth( HAL_noc_InfoType *, uint32_t, HAL_noc_QosBandwidthType * );

/* -----------------------------------------------------------------------
**  Data Transfer Interfaces
** ----------------------------------------------------------------------- */
void noc_GetQosMode     ( HAL_noc_InfoType *, uint32_t, HAL_noc_QosModeType * );
void noc_GetQosPriority ( HAL_noc_InfoType *, uint32_t, HAL_noc_QosPriorityType * );
void noc_GetQosBandwidth( HAL_noc_InfoType *, uint32_t, HAL_noc_QosBandwidthType * );

#endif /* #ifndef HALNOCI_H */

