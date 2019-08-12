#ifndef SFP_FLASH_H
#define SFP_FLASH_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                A S W   S E C U R I T Y   S E R V I C E S
 
               S E C U R E   F L A S H   P A R T I T I O N

                          H E A D E R   F I L E


GENERAL DESCRIPTION
  These are the functions to access sfp, content will be saved to
  flash partition without encryption and hmac protection.
  Routines in this header file used internally only by Nokia 
  rom lote security.

FUNCTIONS
  sfp_flash_write()
  sfp_flash_read()
  sfp_flash_delete()

INITIALIZATION AND SEQUENCING REQUIREMENTS
sfp_open/sfp_pmm_open to get a valid fd as input parameter.


Copyright (c) 2009 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.mpss/3.5.c12.3/securemsm/sfs/environment/env/amss/inc/sfp_flash.h#1 $ 
  $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/29/09   yh      Create Module
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

/*===========================================================================

                 DEFINITIONS AND TYPE DECLARATIONS

===========================================================================*/

#include "core_variation.h"
#ifdef FEATURE_SEC_SFS
#ifdef __cplusplus
extern "C"
{
#endif

/* if BUILD_BOOT_CHAIN is definde, code will be built in oemsbl image,
   else in AMSS image */
#ifdef BUILD_BOOT_CHAIN
#include "boot_comdef.h"

#define MSG_ERROR
#define MSG_MED
#define MSG_HIGH
#define MSG_FATAL
#else
#include "comdef.h"  //boolean
#include "msg.h"
#endif

int sfp_flash_open(int file_id, 
                   int oflag, 
                   boolean encrypt);

int sfp_flash_write (int fd, 
                     const void *buf, 
                     int nbyte);

int sfp_flash_read (int fd, 
                    const void *buf, 
                    int nbyte);

int sfp_flash_delete (int fd);

boolean sfp_flash_size_check(int fd,  
                             int nbyte);


#ifdef __cplusplus
}
#endif
#endif //FEATURE_SEC_SFS
#endif //SFP_FLASH_H

