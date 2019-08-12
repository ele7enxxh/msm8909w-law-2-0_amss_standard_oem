#ifndef GMMINFO_H
#define GMMINFO_H
/*===========================================================================

       G M M   I N F O R M A T I O N   H A N D L E R   H E A D E R   F I L E

DESCRIPTION
  This file contains types, function prototypes and data declarations  
  required for GMM Information procedure

Copyright (c) 2001,2002 Qualcomm Technologies, Incorporated and its licensors.  All Rights 
Reserved.  QUALCOMM Proprietary.  Export of this technology or software 
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.
 
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/src/gmminfo.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $ 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/01/02   mks     Created file
===========================================================================*/
/* <EJECT> */

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "mmtask.h"
#include "mmtask_int.h"
#include "gmm_v.h"

#define  GMM_INFORMATION_MSG_MIN_LENGTH   0x02      /* Defined in TS24.008 */

/*===========================================================================
                F U N C T I O N    D E C L A R A T I O N S
===========================================================================*/

/**------------------------------------------------
 ** This function processes GMM INFORMATION message
 **------------------------------------------------ */ 
void gmm_handle_information ( mm_cmd_type * );

#endif /* #ifndef GMMINFO_H */
