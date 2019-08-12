#ifndef MMGSDI_1X_UPP_H
#define MMGSDI_1X_UPP_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


            M M G S D I   1 X   U P P   H E A D E R S


GENERAL DESCRIPTION

  This header file contains the processing of all MMGSDI file mapping related
  functions

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2014 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/mmgsdi/src/mmgsdi_1x_upp.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/19/14   hh      Initial version
===========================================================================*/

/*=============================================================================

                         INCLUDE FILES FOR MODULE

=============================================================================*/
#include "mmgsdi.h"

/*=============================================================================

                       FUNCTION PROTOTYPES

=============================================================================*/

#ifdef FEATURE_MMGSDI_3GPP2
/*================================================================
FUNCTION  MMGSDI_1X_READ_MDN_AND_VALIDATE_UPP_FILES

DESCRIPTION:
  This function is called to queue a read request for EF-MDN using
  1x PRI session.

  INPUT PARAMETERS:
    session_id :             Id for a 1x primary session
    otasp_commit_rsp_ptr:    Pointer to OTASP commit response from
                             UIM. It is passed as client_data and
                             will be valid only when this function
                             is called during processing of OTASP
                             Commit response.


DEPENDENCIES:
  None

LIMITATIONS:
  None

RETURN VALUE:
  mmgsdi_return_enum_type

SIDE EFFECTS:
  None
===============================================================*/
mmgsdi_return_enum_type mmgsdi_1x_read_mdn_and_validate_upp_files
(
  mmgsdi_session_id_type       session_id,
  mmgsdi_uim_report_rsp_type * otasp_commit_rsp_ptr
);
#endif /* FEATURE_MMGSDI_3GPP2 */

#endif /* MMGSDI_1X_UPP_H */
