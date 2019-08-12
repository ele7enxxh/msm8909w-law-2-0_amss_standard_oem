#ifndef DTR_H
#define DTR_H

/*!
   @file
   dtr.h

   @brief
   

*/

/*===========================================================================

Copyright (c) 2010 - 2013 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$DateTime: 2016/12/13 07:59:54 $ $Author: mplcsds1 $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rflm/dtr/inc/rflm_dtr.h#1 $

when       who     what, where, why
------------------------------------------------------------------------------- 
11/18/13   cvd     Initial version.

============================================================================*/ 

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/

#include "rflm.h"
#include "rflm_diag.h"



#ifdef __cplusplus
extern "C" {
#endif

#define RFLM_DTR_MAX_READER_THREADS 3

#define DTR_DIAG_TRACE_MSG_LOW(fmt, ... ) \
  {RFLM_DIAG_MSG_LOW( fmt, ## __VA_ARGS__); RFLM_DIAG_TRACE( LOW, RFLM_DIAG_MSG_SSID_NONE, fmt, ## __VA_ARGS__);}

#define DTR_DIAG_TRACE_MSG_MED(fmt, ... ) \
  {RFLM_DIAG_MSG_MED( fmt, ## __VA_ARGS__); RFLM_DIAG_TRACE( MED, RFLM_DIAG_MSG_SSID_NONE, fmt, ## __VA_ARGS__);}

#define DTR_DIAG_TRACE_MSG_HIGH(fmt, ... ) \
  {RFLM_DIAG_MSG_HIGH( fmt, ## __VA_ARGS__); RFLM_DIAG_TRACE( HIGH, RFLM_DIAG_MSG_SSID_NONE, fmt, ## __VA_ARGS__);}

#define DTR_DIAG_TRACE_MSG_ERROR(fmt, ... ) \
  {RFLM_DIAG_MSG_ERROR( fmt, ## __VA_ARGS__); RFLM_DIAG_TRACE( ERROR, RFLM_DIAG_MSG_SSID_NONE, fmt, ## __VA_ARGS__);}


typedef enum
{
  DTR_VERBOSE_LEVEL_0,
  DTR_VERBOSE_LEVEL_1,
  DTR_VERBOSE_LEVEL_2,
  DTR_VERBOSE_LEVEL_3
} dtr_verbose_level;


/*===========================================================================
                           FUNCTION DEFINITIONS
===========================================================================*/
void dtr_print_memory_chunk(uint8* addr, uint32 size);

#ifdef __cplusplus
}
#endif

#endif /* DTR_H */
