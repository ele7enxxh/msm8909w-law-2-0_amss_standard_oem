#ifndef _DS_QMI_NAS_H
#define _DS_QMI_NAS_H
/*===========================================================================

                         D S _ Q M I _ N A S . H

DESCRIPTION

 The Data Services Qualcomm Wireless Data Services MSM Interface header file.

EXTERNALIZED FUNCTIONS

  qmi_nas_init()
    Register the Wireless Data Service with QMUX for all applicable QMI links.
     
  qmi_nas_gen_serving_system_ind_cb()  
    Call back function called when the servign system changes.
    
  qmi_nas_rssi_change_cb()                             
    Callback to be called when signal strength changes.  

  
Copyright (c) 2004-2009 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/qmi/src/qmi_nas.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
08/10/11    sm     Moving qmi_nas.h to modem/api/mmode directory
===========================================================================*/
#include "modem_mem.h"
#include "mmgsdilib_common.h"
#include "sys.h"

#define QMI_NAS_MEM_ALLOC(siz)  modem_mem_alloc(siz, MODEM_MEM_CLIENT_QMI_MMODE)
#define QMI_NAS_MEM_FREE(ptr)  modem_mem_free(ptr, MODEM_MEM_CLIENT_QMI_MMODE)

/*===========================================================================
  FUNCTION QMI_NAS_PROCESS_MMGSDI_OPERATION_COMPLETE()

  DESCRIPTION
    The demultiplexing routine within NAS subsystem to send out pending 
    responses based on MMGSDI confirmations.
    
  PARAMETERS
    cnf_type : Type of MMGSDI confirmation
    cnf_data : Response data

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_nas_process_mmgsdi_operation_complete
(
  mmgsdi_cnf_enum_type    cnf_type,
  mmgsdi_cnf_type        *cnf_data
);
/*=============================================================================
FUNCTION: QMI_NAS_GET_HOME_MCC_MNC

DESCRIPTION: Reads the MCC and MNC from SIM.

PARAMETERS:
  None

RETURN VALUE:
  None

DEPENDENCIES:
  None

SIDE EFFECTS:
  None
=============================================================================*/
void qmi_nas_get_home_mcc_mnc 
(
  void
);

/*===========================================================================
  FUNCTION QMI_NASI_GENERATE_GET_CSP_PLMN_MODE_BIT_RESP()

  DESCRIPTION
    Send response for QMI_NAS_GET_CSP_PLMN_MODE_BIT 

  PARAMETERS
    user_data   : user data
    ph_cmd_err  : error type

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/

void qmi_nasi_generate_get_csp_plmn_mode_bit_resp( void );

#endif /* _DS_QMI_NAS_H */

