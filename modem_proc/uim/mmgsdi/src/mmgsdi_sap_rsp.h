#ifndef MMGSDI_SAP_RSP_H
#define MMGSDI_SAP_RSP_H
/*===========================================================================


           M M G S D I   S A P   R S P   H E A D E R S


===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2005 - 2005, 2010-2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.  
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/mmgsdi/src/mmgsdi_sap_rsp.h#1 $$ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
05/03/16   tkl     F3 log prints cleanup
04/07/10   nb      SAP Enhancements
06/23/08   tml     Added featurization for BT in WM
09/10/07   tml     Used generic ATR and card reader functions and removed channel
                   atr function from this file.
07/26/06   sun     Lint Fix and Cleanup
07/07/06   sun     Lint Fixes
12/14/05   tml     MMGSDI Cleanup
12/05/05   sun     Added support for MMGSDI_SAP_CARD_READER_STATUS_REQ and 
                   MMGSDI_SAP_RESET_REQ
11/03/05   tml     Fixed header
10/28/05   sun     Fixed Sap Disconnect
10/20/05   sun     Added support for BT Deregisteration
08/26/05   sun     Added support for BT SAP
07/28/05   sst     Fixed compile errors when SAP feature is not defined
06/06/05   sst     Initial version

=============================================================================*/

#include "mmgsdi.h"
#include "mmgsdilib.h"

/*=============================================================================
  FUNCTION: mmgsdi_sap_rsp_sap_connect

  DESCRIPTION:
    Response to connecting the UIM to the SAP.

  DEPENDENCIES
    None

  LIMITATIONS
    NONE

  RETURN VALUE
    mmgsdi_return_enum_type

  SIDE EFFECTS
    NONE
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_sap_rsp_sap_connect(
  const mmgsdi_uim_report_rsp_type*   uim_rsp_ptr
);


/*-----------------------------------------------------------------------------
  FUNCTION: mmgsdi_sap_rsp_sap_disconnect

DESCRIPTION:
    Response to connect SAP Disconnect Request.

  DEPENDENCIES
    None

  LIMITATIONS
    NONE

  RETURN VALUE
    uim_rsp_ptr:    UIM response data.

  SIDE EFFECTS
    NONE
-----------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_sap_rsp_sap_disconnect(
  const mmgsdi_uim_report_rsp_type*   uim_rsp_ptr
);


/*=============================================================================
  FUNCTION: mmgsdi_sap_rsp_sap_send_apdu

  DESCRIPTION:
    The response to sending an APDU from the SAP to the UIM.

  DEPENDENCIES
    None

  LIMITATIONS
    NONE

  RETURN VALUE
    mmgsdi_return_enum_type

  SIDE EFFECTS
    NONE
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_sap_rsp_sap_send_apdu(
  const mmgsdi_uim_report_rsp_type*     uim_rsp_ptr,
  mmgsdi_protocol_enum_type             protocol
);


/*=============================================================================
  FUNCTION: mmgsdi_sap_rsp_sap_power_on

  DESCRIPTION:
    The response to sending a SIM power on request for the SAP to the UIM.

  DEPENDENCIES
    None

  LIMITATIONS
    NONE

  RETURN VALUE
    mmgsdi_return_enum_type

  SIDE EFFECTS
    NONE
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_sap_rsp_sap_power_on(
  const mmgsdi_uim_report_rsp_type*    uim_rsp_ptr
);


/*=============================================================================
  FUNCTION: mmgsdi_sap_rsp_sap_power_off

  DESCRIPTION:
    The response to sending a SIM power off request for the SAP to the UIM.

  DEPENDENCIES
    None

  LIMITATIONS
    NONE

  RETURN VALUE
    mmgsdi_return_enum_type

  SIDE EFFECTS
    NONE
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_sap_rsp_sap_power_off(
  const mmgsdi_uim_report_rsp_type*     uim_rsp_ptr
);


/*=============================================================================
  FUNCTION: mmgsdi_sap_rsp_sap_reset

  DESCRIPTION:
    The response to sending a SIM reset request for the SAP to the UIM.

  DEPENDENCIES
    None

  LIMITATIONS
    NONE

  RETURN VALUE
    mmgsdi_return_enum_type

  SIDE EFFECTS
    NONE
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_sap_rsp_sap_reset(
  const mmgsdi_uim_report_rsp_type*     uim_rsp_ptr
);


/*=============================================================================
  FUNCTION: mmgsdi_sap_rsp_build_sap_rsp

  DESCRIPTION:
    build and queue a sap response

  DEPENDENCIES
    None

  LIMITATIONS
    NONE

  RETURN VALUE
    mmgsdi_return_enum_type

  SIDE EFFECTS
    NONE
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_sap_rsp_build_sap_rsp (
  mmgsdi_return_enum_type     mmgsdi_status,
  int32                       index 
);


/* =============================================================================
   FUNCTION:      MMGSDI_SAP_RSP_BUILD_CNF

   DESCRIPTION:
     This function builds the SAP Confirmation and put the 
     it onto the MMGSDI task queue.
     
   DEPENDENCIES:


   LIMITATIONS:
     

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.
     MMGSDI_NOT_SUPPORTED:    When the file has an entry in the Service Table
                              but the service table indicates the card does not
                              have the support

   SIDE EFFECTS:

=============================================================================*/
mmgsdi_return_enum_type mmgsdi_sap_rsp_build_cnf(
  const mmgsdi_generic_rsp_type        *sap_rsp_ptr
);


#endif /* MMGSDI_SAP_RSP_H */
