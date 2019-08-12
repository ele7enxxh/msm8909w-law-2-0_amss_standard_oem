#ifndef MMGSDI_REFRESH_RSP_H
#define MMGSDI_REFRESH_RSP_H
/*===========================================================================


           M M G S D I   R E F R E S H   R S P   H E A D E R S


===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2007-2009, 2013 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.  
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/mmgsdi/src/mmgsdi_refresh_rsp.h#1 $$ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
11/18/13   vv       Added support for parallel REFRESH
03/30/09   xz       Change mmgsdi_refresh_rsp_send_cnf_to_gstk() return type
12/01/08   ssr      Handled error condition for some MMGSDI Functions
03/17/08   sun      Allow cleanup to be done during a refresh
11/12/07   sun      Initial version

=============================================================================*/

#include "mmgsdi.h"
#include "mmgsdilib.h"

/* ============================================================================
  FUNCTION:      MMGSDI_REFRESH_MAIN_RSP

  DESCRIPTION:
    This function will process any refresh response 
     
  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    mmgsdi_return_enum_type
    MMGSDI_SUCCESS:          The function was successful is sending the response
    MMGSDI_ERROR:            Could not get client index/
                             Could not build confirmation/
                             Other error
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the function are not
                             within appropriate ranges.
  
  SIDE EFFECTS:
    None
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_refresh_main_rsp(
  const mmgsdi_uim_report_rsp_type      * uim_rsp_ptr
);

/* ============================================================================
  FUNCTION:      MMGSDI_REFRESH_RSP_SEND_CNF_TO_GSTK

  DESCRIPTION:
    This function will send confirmation to GSTK  

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    mmgsdi_return_enum_type
    MMGSDI_SUCCESS:          Succeed in send confirmation to GSTK
    MMGSDI_ERROR:            Fail to send confirmation to GSTK
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the function are not
                             within appropriate ranges.

    
  SIDE EFFECTS:
    None
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_refresh_rsp_send_cnf_to_gstk(
  mmgsdi_request_header_type            *request_hdr_ptr);


/* ============================================================================
  FUNCTION:      MMGSDI_REFRESH_RSP_CLEAN_UP

  DESCRIPTION:
    This function will clean up the refresh linked list and the global 
    refresh pointer

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    None

  SIDE EFFECTS:
    None
=============================================================================*/
void mmgsdi_refresh_rsp_clean_up(uint8 slot_index);


#endif /* MMGSDI_REFRESH_RSP_H */

