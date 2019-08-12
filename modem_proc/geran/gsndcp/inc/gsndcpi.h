/*****************************************************************************
***
*** TITLE  gsndcpi.h
***
***  GPRS SNDCP LAYER INTERNAL GERAN HEADER FILE.
***
***
*** DESCRIPTION
***
***  Accessibility function prototypes for requesting message resource from
***  SNDCP, and registration definitions and prototypes for both task and
***  PDP interfacing.
***
***
***  FEATURE SUPPORT
***  ---------------
***
***   FEATURE_GSM_GPRS              - Supports GPRS including GPRS SNDCP.
***
***   FEATURE_GSM_GPRS_SNDCP_DCOMP  - Supports GPRS SNDCP Data Compression
***
***   FEATURE_GSM_GPRS_SNDCP_PCOMP  - Supports GPRS SNDCP Protocol Control
***                                   Information (PCI) Compression
***
***
*** Copyright (c) 2001-2013 Qualcomm Technologies, Inc.
*** All Rights Reserved. QUALCOMM Proprietary.
***
*** Export of this technology or software is regulated by the U.S.
*** Government. Diversion contrary to U.S. law prohibited.
***
*****************************************************************************/


/*****************************************************************************
***
***                        EDIT HISTORY FOR MODULE
***
***  This section contains comments describing changes made to the module.
***  Notice that changes are listed in reverse chronological order.
***
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gsndcp/inc/gsndcpi.h#1 $
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 11-07-08   tjw     New file created from API file
*** 11/14/02   rc      Added multimode support.
*** mm/dd/yy   xyz     changes
***
*****************************************************************************/

#ifndef INC_GSNDCPI_H
#define INC_GSNDCPI_H


/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/

#include "geran_variation.h"
#ifndef CUSTOMER_H
  #include "customer.h"
#endif

#ifndef COMDEF_H
  #include "comdef.h"
#endif

#include "gsmsnif.h"
#include "dsm.h"
#include "rex.h"
#ifdef FEATURE_DATA_MM
#include "comptask.h"
#include "comptask_api.h"
#endif /* FEATURE_DATA_MM */


/*****************************************************************************
***
***     Public Defines & Constants
***
*****************************************************************************/


/*****************************************************************************
***
***     Public Data References
***
*****************************************************************************/


/*****************************************************************************
***
***     Public Macros
***
*****************************************************************************/


/*****************************************************************************
***
***     Public Functions
***
*****************************************************************************/


/*===========================================================================
===
===  FUNCTION      GSNDCP_PROCESS_CMD_QUEUE()
===
===  DESCRIPTION
===
===    To be called by the hosting task on activation of the
===    host_cmd_queue_sig that was registered with SNDCP on initialization.
===
===  DEPENDENCIES
===
===    SNDCP must be initialised. The host task is expected to clear the
===    associated REX signal before calling this handler.
===
===  RETURN VALUE
===
===    None.
===
===  SIDE EFFECTS
===
===    None.
===
===========================================================================*/

extern boolean gsndcp_process_cmd_queue
(
#ifdef FEATURE_DATA_MM
  comp_sig_enum_type sig,
  void             *pkt_instance
#else
 void
#endif /* FEATURE_DATA_MM */
);

/*===========================================================================
===
===  FUNCTION      GSNDCP_PROCESS_LL_DL_QUEUE()
===
===  DESCRIPTION
===
===    To be called by the hosting task on activation of the
===    host_ll_dl_queue_sig that was registered with SNDCP on initialization.
===
===  DEPENDENCIES
===
===    SNDCP must be initialised. The host task is expected to clear the
===    associated REX signal before calling this handler.
===
===  RETURN VALUE
===
===    None.
===
===  SIDE EFFECTS
===
===    None.
===
===========================================================================*/

extern boolean gsndcp_process_ll_dl_queue
(
#ifdef FEATURE_DATA_MM
  comp_sig_enum_type sig,
  void             *pkt_instance
#else
  void
#endif /* FEATURE_DATA_MM */
);


/*===========================================================================
===
===  FUNCTION      GSNDCP_PDP_DL_SUSPEND()
===
===  DESCRIPTION
===
===    Suspends downlink PDU flow to the PDP.
===
===  DEPENDENCIES
===
===    SNDCP must be initialised.
===
===  RETURN VALUE
===
===    None.
===
===  SIDE EFFECTS
===
===    None.
===
===========================================================================*/

extern void gsndcp_pdp_dl_suspend( void );


/*===========================================================================
===
===  FUNCTION      GSNDCP_PDP_DL_RESUME()
===
===  DESCRIPTION
===
===    Resumes downlink PDU flow to the PDP.
===
===  DEPENDENCIES
===
===    SNDCP must be initialised.
===
===  RETURN VALUE
===
===    None.
===
===  SIDE EFFECTS
===
===    None.
===
===========================================================================*/

extern void gsndcp_pdp_dl_resume( void );


#endif /* INC_GSNDCPI_H */

/*** EOF: don't remove! ***/
