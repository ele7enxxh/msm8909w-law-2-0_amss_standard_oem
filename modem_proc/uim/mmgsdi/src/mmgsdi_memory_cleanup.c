/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                 MMGSDI - MEMORY CLEANUP RELATED FUNCTIONS


GENERAL DESCRIPTION

  This source file contains functions for freeing up the memory allocated
  and held by MMGSDI during its normal operation in off-target environment.
  This is called only after all MMGSDI tasks have stopped

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2014-2015 QUALCOMM Techologies, Inc (QTI) and its licensors.  All Rights
Reserved.  QUALCOMM Techologies Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/mmgsdi/src/mmgsdi_memory_cleanup.c#1 $
$DateTime: 2016/12/13 08:00:05 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/23/15   vdc     Send profile switching evt when profile has switched in card
07/09/15   ar      Validate PRL at diff steps if SD task not ready
05/29/15    nr     Fixing memoory leak during card powerdown and task stop
05/18/15    nr     Fix to distiguish reads on different apps in single slot
04/17/15    nr     Phonebook record read optimization
01/19/15    kr     Fixed Compilation warnings
11/15/14    ar     Reduce usage of MMGSDI_TASK_ENTER/LEAVE_CRIT_SECT
10/16/14    am     Flush high pri queues without clien_id/session_id reference 
09/29/14    tkl    Added support for persistent cache
09/27/14    ar     Add support for multiple slots in BT-SAP
09/21/14    ar     Add check for task_state before accessing its globals
08/14/14    tl     Add support for silent PIN verification
08/06/14    am     Clear refresh retry info 
07/15/14    yt     Support for NV refresh
07/14/14    tl     Remove always ON feature FEATURE_MODEM_RCINIT
07/14/14    am     Free the pointers in response queues causing leaks
07/14/14    am     Correctly pass slot index 
06/11/14    vv     Fix for a possible memory leak
04/06/14    am     Initial version
===========================================================================*/

/*===========================================================================

                         INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"

#ifdef FEATURE_UIM_TEST_FRAMEWORK
#error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */
