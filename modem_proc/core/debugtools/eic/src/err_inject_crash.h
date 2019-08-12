#ifndef ERR_INJECT_CRASH_H

#define ERR_INJECT_CRASH_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                      ERR service crash simulator
                
GENERAL DESCRIPTION
  Command handler for injecting various types of crashes for testing

INITIALIZATION AND SEQUENCING REQUIREMENTS
  Diag must be initialized

Copyright (c) 2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*==========================================================================

                        EDIT HISTORY FOR MODULE
$Header: //components/rel/core.mpss/3.5.c12.3/debugtools/eic/src/err_inject_crash.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
10/28/11   mcg     File created.

===========================================================================*/



/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

/*===========================================================================

                     EXTERNAL DATA DECLARATIONS

===========================================================================*/
typedef enum
{
  ERR_INJECT_ERR_FATAL,
  ERR_INJECT_WDOG_TIMEOUT,
  ERR_INJECT_NULLPTR,
  ERR_INJECT_DIV0,
  SIZEOF_ERR_INJECT_CRASH_TYPE
} err_inject_crash_type;

/*===========================================================================

                     EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/
void err_inject_crash_init(void);

#define ERR_DIAG_PROC_ID DIAG_MODEM_PROC

#endif /* ERR_INJECT_CRASH_H */
