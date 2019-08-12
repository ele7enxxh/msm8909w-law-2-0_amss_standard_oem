#ifndef MODEM_HW_MUTEX_H
#define MODEM_HW_MUTEX_H

/*===========================================================================
Copyright (c) 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/securemsm/modem_sec/inc/modem_hw_mutex.h#1 $
$DateTime: 2016/12/13 07:59:23 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/05/12   rs      First version.
============================================================================*/

/** Set reserved HW Mutex ID for Security use **/
#define MODEM_HW_MUTEX_ID               8

/** Set Processor ID to 2 to indicate reqest from Modem **/
#define MODEM_HW_MUTEX_PID              2

/**
 * Grab Hardware Mutex for QFPROM Driver
 *
 */
void fuse_grab_mutex( void );

/**
 * Release Hardware Mutex for QFPROM Driver
 *
 */
void fuse_release_mutex( void );


#endif /* MODEM_HW_MUTEX_H */
