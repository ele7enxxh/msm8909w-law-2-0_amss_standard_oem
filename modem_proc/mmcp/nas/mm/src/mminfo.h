#ifndef MMINFO_H
#define MMINFO_H
/*===========================================================================

        M M   I N F O R M A T I O N   H A N D L E R   H E A D E R   F I L E

DESCRIPTION
  This file contains types, function prototypes and data declarations  
  required for MM Information procedure

Copyright (c) 2001,2002 Qualcomm Technologies, Incorporated and its licensors.  All Rights 
Reserved.  QUALCOMM Proprietary.  Export of this technology or software 
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.
 
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/src/mminfo.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $ 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/01/02   mks     Created file
===========================================================================*/
/* <EJECT> */

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "mm_v.h"
#include "mmtask.h"
#include "mmtask_int.h"
#include "mm_mm_v.h"
#include "mm_iei.h"

/*===========================================================================
                     M A C R O  D E F I N I T I O N S
===========================================================================*/
#define MM_MAX_NETWORK_NAME_LEN           247
#define MM_LSA_ID_SIZE                    3
#define MM_INFORMATION_MSG_MIN_LENGTH     2

/*===========================================================================
                   T Y P E  D E C L A R A T I O N S
===========================================================================*/


/*===========================================================================
                F U N C T I O N    D E C L A R A T I O N S
===========================================================================*/

/**-----------------------------------------------
 ** This function processes MM INFORMATION message
 **----------------------------------------------- */ 
void mm_handle_information ( mm_cmd_type * );

#endif /* #ifndef MMINFO_H */
