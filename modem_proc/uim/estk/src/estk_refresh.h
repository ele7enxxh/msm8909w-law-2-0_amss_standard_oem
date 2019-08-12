#ifndef ESTK_REFRESH_H
#define ESTK_REFRESH_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                      ESTK  SHORT  MESSAGE


GENERAL DESCRIPTION

  This header file contains ESTK functions that handle Refresh.

EXTERNALIZED FUNCTIONS
  
INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2013 QUALCOMM Technologies, Inc (QTI) and its licensors.  
All Rights Reserved.  QUALCOMM Technologies Proprietary. 
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/estk/src/estk_refresh.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/29/13   hn      Initial version

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "uim_variation.h"
#include "customer.h"

#ifdef FEATURE_ESTK
#include "estk_priv.h"


/*===========================================================================
FUNCTION: estk_process_refresh_cnf

DESCRIPTION:
  GSTK calls this ESTK function to process the REFRESH CNF.

PARAMETERS:
  cmd_ptr : [Input] ESTK command pointer
 
DEPENDENCIES:
  None

RETURN VALUE:
  estk_status_enum_type
    ESTK_SUCCESS: - When REFRESH is successfully handled
    ESTK_ERROR:   - When there is failure in handling REFRESH

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_process_refresh_cnf(
  estk_cmd_type *cmd_ptr);


/*===========================================================================
FUNCTION: estk_process_refresh_req

DESCRIPTION:
  GSTK calls this ESTK function to process the REFRESH request.

PARAMETERS:
  gstk_req_ptr : [Input] gstk REFRESH cmd pointer
 
DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type
    ESTK_SUCCESS
    ESTK_ERROR

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_process_refresh_req(
  gstk_cmd_from_card_type *gstk_req_ptr);

#endif /* FEATURE_ESTK */
#endif /* ESTK_REFRESH_H */

