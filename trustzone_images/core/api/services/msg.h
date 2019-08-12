#ifndef MSG_H
#define MSG_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
         
                EXTENDED DIAGNOSTIC MESSAGE SERVICE HEADER FILE

GENERAL DESCRIPTION

  All the declarations and definitions necessary to support the reporting 
  of messages for errors and debugging.  This includes support for the 
  extended capabilities as well as the legacy messaging scheme.

Copyright (c) 2002 - 2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR FILE

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/services/msg.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/02/10   mad     Removed uldr feature.
01/20/10   sg      Moved inclusion of msg_qsr.h to msg_diag_service.h
11/18/09   ps      Added qsr feature support.
09/30/09   as      Removed inclusion of customer.h 
08/05/09   JV      Removed the CUST_H featurization around the inclusion of 
                   customer.h.
07/17/09   mad     Included customer.h conditionally.
03/16/09   mad     Removed inclusion of customer.h
10/23/08   mad     Separated out OS-specific redirections of MSG macros into
                   different header files. Now, diag message service macros 
                   are defined in msg_diag_service.h.
01/02/08   mad     Enabled real diag F3 messages for FEATURE_WINCE, 
                   instead of retail messages.
09/01/04   eav     Code reorganization
07/08/04   eav     Moved F3 trace saving typedefs to diagdebug.h
06/02/04   eav     Added "debug_mask".  Determines whether or not to save
                   the F3 trace to the RAM buffer.
04/21/03   as      Added MSG_SPRINTF macros for 4,5,6,7 &8 arguments.
03/12/04   eav     Added msg_copy_to_efs_check prototype to be called from
                   tmc.c. 
02/20/04   eav     Added savetime and savevar flags to msg_const_type struct. 
09/23/03   as      defined (TARGET_UNIX) to support the DSPE - a Linux based 
                   phone emulator used by data team in testing and debugging.
07/24/02   lad     Updated to reflect requirements changes and final 
                   implementation of extended message service.
02/06/02   igt     Created module.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/


#if (defined FEATURE_WINCE_OAL || defined FEATURE_WINCE_BOOTLOADER)
#include  "msg_wince.h"
#else
#include "msg_diag_service.h"
#endif

#endif              /* MSG_H */
