/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

   TM SIM UTILS module to interface with MMGSDI.


GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS


  Copyright (c) 2000 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/inc/tm_sim_utils.h#1 $
  $DateTime: 2016/12/13 07:59:45 $
  $Author: mplcsds1 $

  when       who     what, where, why
  --------   ---     ----------------------------------------------------------
  12/27/13  mjohn  Remove "FEATURE_DUAL_SIM" 
===========================================================================*/

#ifndef TM_SIM_UTILS_H
#define TM_SIM_UTILS_H

#include "comdef.h"    /* Definition for basic types and macros */
#include "gps_common.h"

#if (defined(FEATURE_CGPS_UMTS_UP_SUPL) || defined(FEATURE_CGPS_XTRA_T)) 

#ifdef FEATURE_MMGSDI_SESSION_LIB
#include "mmgsdisessionlib.h"
#include "mmgsdisessionlib_v.h"
#else /* FEATURE_MMGSDI_SESSION_LIB */
#include "uim.h"  /* For accessing the IMSI*/
#include "gsdi_exp.h" /* For accessing the IMSI*/
#endif /* FEATURE_MMGSDI_SESSION_LIB */

#include "sys.h"

#ifdef FEATURE_MMGSDI_SESSION_LIB
/*===========================================================================
FUNCTION tm_sim_mmgsdi_init

DESCRIPTION
  Register for MMGSDI client ID

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void tm_sim_mmgsdi_init(void);

/*===========================================================================
FUNCTION tm_sim_mmgsdi_client_reg_cb

DESCRIPTION
  Callback function from MMGSDI to provide client ID.

DEPENDENCIES
  mmgsdi_client_id_reg() should be called before.

SIDE EFFECTS
  None
===========================================================================*/
void tm_sim_mmgsdi_client_reg_cb(
  mmgsdi_return_enum_type status,
  mmgsdi_cnf_enum_type    cnf,
  const mmgsdi_cnf_type   *cnf_ptr);


/*===========================================================================
FUNCTION tm_sim_mmgsdi_session_reg_cb

DESCRIPTION
  This function handles the MMGSDI SESSION OPEN command callback.
  It will assign the global Session ID value which will be used in mmgsdi
  function calls

DEPENDENCIES
   Valid client ID should have been obtained before calling this.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void tm_sim_mmgsdi_session_reg_cb(
  mmgsdi_return_enum_type status,
  mmgsdi_cnf_enum_type    cnf,
  const mmgsdi_cnf_type   *cnf_ptr);

/*===========================================================================
FUNCTION tm_sim_mmgsdi_evt_cb

DESCRIPTION
  Internal Event Callback function registered during mmgsdi_init so that
  MMGSDI can receive event notification

DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECTS
  State variables may be updated as needed
===========================================================================*/
void tm_sim_mmgsdi_evt_cb (
  const mmgsdi_event_data_type *event_ptr
);

#endif

#endif /* FEATURE_CGPS_UMTS_UP_SUPL || FEATURE_CGPS_XTRA_T */

/*===========================================================================
FUNCTION tm_sim_retrieve_imsi

DESCRIPTION
  Retrieve the IMSI value from the the SIM and store a copy 
  in the Ram (z_umts_nv_item_ram_copy).

DEPENDENCIES
   Valid client ID should have been obtained before calling this.

PARAMETERS: 
  p_IMSI - The retrieved IMSI to return to the caller (output)

RETURN VALUE
  True/False

SIDE EFFECTS
  None
===========================================================================*/
boolean tm_sim_retrieve_imsi( cgps_IMSIType * const pz_IMSI );


#endif 
