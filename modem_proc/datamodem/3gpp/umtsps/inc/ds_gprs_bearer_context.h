


/*!
  @file
  ds_gprs_bearer_context.h

  @brief
  Lower layer/Bearer Context interface

  @detail
  OPTIONAL detailed description of this C header file.
  - DELETE this section if unused.

*/

/*===========================================================================

  Copyright (c) 2010-2011 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/umtsps/inc/ds_gprs_bearer_context.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/02/12   msh     Coan: Feature cleanup
02/23/11   ttv     Added support for watermark tracing.
01/31/11   ss      Added support for reporting bearer tech type in WCDMA 
                   and GPRS.
12/01/10   ss      Support for SIM REFRESH VOTING for WCDMA and GPRS.

===========================================================================*/

#ifndef DSGPRSBEARERCONTEXT_H
#define DSGPRSBEARERCONTEXT_H

#include "datamodem_variation.h"
#ifdef FEATURE_GSM_GPRS

#include "ds_umts_bearer_context.h"
#include "sys.h"


/*===========================================================================

                           INCLUDE FILES

===========================================================================*/


/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/
/* Structure for GSNDCP Registration/Resumption Confirmation Events */
typedef struct
{
  void               *bearer_context_p;    /* instance of the call */
  boolean            cnf_status;           /* confirmation status  */
} ds_gprs_gsndcp_reg_cnf_ev_type;


/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/
/*===========================================================================
FUNCTION DS_GPRS_BEARER_CNTXT_INIT

DESCRIPTION
  This function is used to initialize the GPRS bearer context.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  
SIDE EFFECTS
  None.

===========================================================================*/
void ds_gprs_bearer_cntxt_init( void );

/*===========================================================================

FUNCTION DSUMTSPS_PROCESS_GSNDCP_EV_CMD

DESCRIPTION

  This function processes the GSNDCP REGISTRATION/RESUMPTION COMPLETE 
  commands received by the DS task. The following are handled:

  DSUMTSPS_GSNDCP_REG_CNF_CMD: Command to indicate registration proceeding
                               call setup with GSNDCP is complete

  DSUMTSPS_GSNDCP_RES_CNF_CMD: Command to indicate registration proceeding 
                               resumption with GSNDCP is complete

PARAMETERS
  cmd_ptr   - Ptr to the command information

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds_gprs_bearer_cntxt_process_gsndcp_ev_cmd
(
  const ds_cmd_type *cmd_ptr                         /* Pointer to Command */
);

/*===========================================================================
FUNCTION DS_GPRS_BEARER_CONTEXT_REFRESH_VOTING_HDLR

DESCRIPTION   
  This function checks the state of all GPRS PS calls and decides whether an
  ISIM refresh will be allowed to proceed.

PARAMETERS   
   subs_id         - subscription id
  
DEPENDENCIES  
  None.

RETURN VALUE  
  TRUE  - if SIM Refresh can proceed from mode handler perspective.
  FALSE - if SIM Refresh cannot proceed from mode handler perspective.

SIDE EFFECTS  
  None.
===========================================================================*/
boolean ds_gprs_bearer_cntxt_refresh_voting_hdlr
(
  sys_modem_as_id_e_type      subs_id
);

/*===========================================================================
FUNCTION DS_GPRS_GET_COST_OF_MODEM_POWER

DESCRIPTION
  This returns the cost of modem power on the current subscription ID
  on GPRS RAT
 
PARAMETERS
  *argval_ptr       - Pointer to fill in the cost of modem Power
  *ps_errno         - PS Error code in case of error
 
DEPENDENCIES
  None.

RETURN VALUE
  0   - If the modem power has been determined successfully from Lower
        layers
  -1  - otherwise 

SIDE EFFECTS
  None.

===========================================================================*/
int16 ds_gprs_get_cost_of_modem_power
(
  void                        *argval_ptr,
  int16                       *ps_errno
);

/*===========================================================================
FUNCTION DS_GPRS_BEARER_CNTXT_GET_PS_RAT_MASK_FOR_EGPRS_SUPPORT

DESCRIPTION
  This function returns the RAT MASK corresponding to the passed EGPRS 
  support.

PARAMETERS
  egprs_support - Indicates whether EGPRS is supported or not

DEPENDENCIES
  None.

RETURN VALUE
  RAT mask 

SIDE EFFECTS
  None.
===========================================================================*/
uint32 ds_gprs_bearer_cntxt_get_ps_rat_mask_for_egprs_support
(
  sys_egprs_support_e_type  egprs_support
);

#ifdef FEATURE_QXDM_WM_TRACING
/*===========================================================================
FUNCTION DS_GPRS_BEARER_CNTXT_REPORT_WM_STATS

DESCRIPTION
  This function stores the current watermark status.

PARAMETERS
  bearer_instance - bearer context instance 
  call_id         - call id
  wm_stat_ptr     - where the wm values are stored

DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void ds_gprs_bearer_cntxt_report_wm_stats
(
  uint32          bearer_instance,
  uint32          call_id,
  fc_wm_stat_type *wm_stat_ptr
);
#endif /* FEATURE_QXDM_WM_TRACING */

#endif /* FEATURE_GSM_GPRS */
#endif /* DSGPRSBEARERCONTEXT_H */
