#ifndef TZBSP_HW_MUTEX_H
#define TZBSP_HW_MUTEX_H

/*===========================================================================
Copyright (c) 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/boot/fuse_hw_mutex.h#1 $
$DateTime: 2016/06/17 14:31:11 $
$Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/05/12   rs      First version.
============================================================================*/

/** Set reserved HW Mutex ID for Security use **/
#define TZBSP_HW_MUTEX_ID               8

/** Set Processor ID to 1 to indicate reqest from Apps **/
#define TZBSP_HW_MUTEX_PID              1

/**
 * Grab Hardware Mutex for QFPROM Driver
 *
 */
void __weak fuse_grab_mutex( void );

/**
 * Release Hardware Mutex for QFPROM Driver
 *
 */
void __weak fuse_release_mutex( void );


#endif /* TZBSP_HW_MUTEX_H */
