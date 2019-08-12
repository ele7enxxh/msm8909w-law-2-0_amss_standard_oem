#ifndef MCFG_LOG_H
#define MCFG_LOG_H
/*==============================================================================

                M O D E M   C O N F I G   U T I L S
 
GENERAL DESCRIPTION
  Header MCFG Debug EFS logging support

Copyright (c) 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcfg_fwk/mcfg_utils/inc/mcfg_log.h#1 $ 
$DateTime: 2016/12/13 08:00:27 $ 
$Author: mplcsds1 $ 
$Change: 11985219 $ 

when     who   what, where, why
-------- ---   ----------------------------------------------------------------
05/29/12 sbt   Initial creation

==============================================================================*/

/*==============================================================================

                PUBLIC DEFINITIONS AND DECLARATIONS FOR MODULE

==============================================================================*/
/*==============================================================================
  Constants and Macros
==============================================================================*/

/*------------------------------------------------------------------------------
  Log file
------------------------------------------------------------------------------*/
/* Log filenames. Log rotation is mcfg_debug.log -> mcfg_debug_old.log -> deleted */
#define MCFG_LOG_PATH       "/mcfg_debug.log"
#define MCFG_OLD_LOG_PATH   "/mcfg_debug_old.log"

/* How big the log can get before it gets rotated. */
#define MCFG_LOG_MAX_SIZE       (65536)
#define MCFG_MAX_LOG_LINE_LEN   (128)

/*==============================================================================
  Typedefs
==============================================================================*/

/*==============================================================================
  Variables
==============================================================================*/


/*============================================================================== 
 
                    PUBLIC FUNCTION DECLARATIONS FOR MODULE
 
==============================================================================*/
void mcfg_save_log_entry( const char *line );

#endif /* MCFG_LOG_H */

