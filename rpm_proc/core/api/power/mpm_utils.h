#ifndef MPM_UTILS_H
#define MPM_UTILS_H

/**
@file mpm_utils.h
 
@brief Contains public MPM utility interfaces.

*/

/*-----------------------------------------------------------------------------
             Copyright © 2011 Qualcomm Technologies Incorporated.
                 All Rights Reserved.
                 QUALCOMM Confidential and Proprietary.
-----------------------------------------------------------------------------*/

/*=============================================================================
                             Edit History
$Header: //components/rel/rpm.bf/2.1.1/core/api/power/mpm_utils.h#1 $

=============================================================================*/


/*=============================================================================
                       FUNCTION PROTOTYPES
=============================================================================*/

/**
 * 
 * Read the MPM TIMETICK counter, and return the current count. NOTE:
 * the counter tracks time in slow clock counts, from 0 to 0xFFFFFFFF 
 * (~1.5 days)
 * 
 * @return uint32 Slow clock timetick counter value
 * 
 * @dependencies Must be called from an INTLOCK'd context.
 */
uint32 mpm_get_timetick( void );

#endif /* !MPM_UTILS_H */

