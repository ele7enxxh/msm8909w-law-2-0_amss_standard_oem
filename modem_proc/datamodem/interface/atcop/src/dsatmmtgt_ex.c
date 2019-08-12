/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        D A T A   S E R V I C E S
                A T   C O M M A N D   P R O C E S S O R

      M U L T I M O D E   T A R G E T   C O M M A N D   T A B L E S

GENERAL DESCRIPTION
  This module contains the top level command tables for the multimode
  target.  It also contains any target specific command tables required
  to support target specific support of AT modem commands.

EXTERNALIZED FUNCTIONS
  None
    
INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

  Copyright (c) 2005 - 2015 by Qualcomm Technologies, Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/atcop/src/dsatmmtgt_ex.c#1 $


when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/24/14   tk      ATCoP changes for dynamic memory optimizations.
06/27/14   tk/sc   Initial revision (created file for Dynamic ATCoP).

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"


#include "dsati.h"
#include "dsatclienti.h"
#include "dsatctab.h"
#include "dsatdl.h"
#ifdef FEATURE_DATA_IS707
#include "dsat707extctab.h"
#include "dsat707faxctab.h"
#include "dsat707vend.h"
#ifdef FEATURE_HDR
#include "dsat707hdrctab.h"
#endif
#ifdef FEATURE_DS_MOBILE_IP
#include "dsat707mipctab.h"
#endif
#ifdef FEATURE_DS_PSTATS
#include "dsat707pstatsctab.h"
#endif
#include "dsat707vendctab.h"
#endif /* FEATURE_DATA_IS707 */
#ifdef FEATURE_DSAT_ETSI_MODE
#include "dsatetsictab.h"
#include "dsatetsicmif.h"
#include "dsatetsime.h"
#include "dsatetsicall.h"
#endif /* FEATURE_DSAT_ETSI_MODE */
#if defined(FEATURE_ETSI_SMS) || defined(FEATURE_CDMA_SMS)
#include "dsatsms.h"
#endif /* defined(FEATURE_ETSI_SMS) || defined(FEATURE_CDMA_SMS) */


#ifdef FEATURE_CDMA_SMS
#include "dsat707smsctab.h"
#endif /* FEATURE_ETSI_SMS */

#ifdef FEATURE_DATA_GCSD_FAX
#include "dsatgsmfax.h"
#endif  /* FEATURE_DATA_GCSD_FAX */

#include "dsatvoice.h"
#include "dsatcmif.h"
#include "dsatme.h"
#include "dsatvend.h"
#include "dsatact.h"
/*===========================================================================

                    REGIONAL DEFINITIONS AND DECLARATIONS

===========================================================================*/



/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains local definitions for constants, macros, types,
  variables and other items needed by this module.

===========================================================================*/

/*---------------------------------------------------------------------------
                           Defaults and limits
---------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------
                       Common Command Tables 
--------------------------------------------------------------------------*/

LOCAL dsati_at_cmd_table_entry_type basic_table_entries[ ] =
{
  { dsat_basic_table,        &dsat_basic_table_size },
  { dsat_basic_action_table, &dsat_basic_action_table_size },  
  { NULL, 0 }
};

LOCAL dsati_at_cmd_table_entry_type sreg_table_entries[ ] =
{
  { dsat_sreg_table, &dsat_sreg_table_size },
  { NULL, 0 }
};

/*--------------------------------------------------------------------------
                       Target Specific Command Tables 
--------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------
                      Top Level Command Tables - GSM
---------------------------------------------------------------------------*/

LOCAL dsati_at_cmd_table_entry_type extended_table_entries[ ] =
{
  { dsat_ext_table,      &dsat_ext_table_size },
#ifdef FEATURE_DSAT_ETSI_MODE
  { dsatetsi_ext_table,  &dsatetsi_ext_table_size },  
  { dsatetsi_ext_action_table, &dsatetsi_ext_action_table_size },
#endif /* FEATURE_DSAT_ETSI_MODE */
#ifdef FEATURE_DATA_IS707
    { dsat707_ext_para_table, &dsat707_ext_para_table_size },
    { dsat707_ext_action_table, &dsat707_ext_action_table_size },
#ifdef FEATURE_DATA_IS707_ASYNC
    { dsat707_fax_table, &dsat707_fax_table_size },
#endif /* FEATURE_DATA_IS707_ASYNC */
#endif /* FEATURE_DATA_IS707 */

  { NULL, 0 }
};

LOCAL dsati_at_cmd_table_entry_type vendor_table_entries[ ] =
{
 { dsat_vendor_table,  &dsat_vendor_table_size },
#ifdef FEATURE_DATA_IS707
 { dsat707_vend_para_table, &dsat707_vend_para_table_size },
 { dsat707_vend_action_table, &dsat707_vend_action_table_size },
#ifdef FEATURE_HDR_AT_TEST_ONLY
 { dsat707_hdr_table,  &dsat707_hdr_table_size },
#endif /* FEATURE_HDR_AT_TEST_ONLY */
#ifdef FEATURE_DS_MOBILE_IP
 { dsat707_mip_table,  &dsat707_mip_table_size },
#endif /* FEATURE_DS_MOBILE_IP */
#ifdef FEATURE_DS_PSTATS
 { dsat707_pstats_table, &dsat707_pstats_table_size },
#endif /* FEATURE_DS_PSTATS */
#ifdef FEATURE_CDMA_SMS
 { dsat707_sms_table, &dsat707_sms_table_size },
#endif /* FEATURE_CDMA_SMS */
#endif /* FEATURE_DATA_IS707 */
    { NULL, 0 }
};
/*---------------------------------------------------------------------------
                      Top Level Command Tables - 1X
---------------------------------------------------------------------------*/

const dsati_at_cmd_table_type at_cmd_table =
  { basic_table_entries,
    sreg_table_entries, 
    extended_table_entries,
    vendor_table_entries
  };


/*---------------------------------------------------------------------------
                      Asynchronous Event Handler Table
---------------------------------------------------------------------------*/

#ifdef FEATURE_DSAT_DYNAMIC_LOADING
const dsati_async_event_table_entry_type async_event_table_entries_ex[] =
{
  { DS_CMD_ATCOP_STATUS_CMD,         dsatme_cmd_status_handler_ex},

  { DS_CMD_ATCOP_CM_CALL_CMD,        dsatcmif_cm_call_cmd_handler_ex },
  { DS_CMD_ATCOP_CM_CALL_INFO_CMD,   dsatcmif_cm_call_event_handler_ex },
  { DS_CMD_ATCOP_CM_PH_CMD,          dsatcmif_cm_ph_cmd_handler_ex },
  { DS_CMD_ATCOP_CM_PH_INFO_CMD,     dsatcmif_cm_ph_event_handler_ex },

  { DS_CMD_ATCOP_TIMER_EXPIRED_CMD,  dsatutil_dispatch_timer_handler },

#ifdef FEATURE_MMGSDI
  { DS_CMD_ATCOP_MMGSDI_INFO_CMD,    dsatme_mmgsdi_event_handler },
  { DS_CMD_ATCOP_MMGSDI_SIM_INFO,    dsatme_mmgsdi_sim_event_handler_ex },
#endif /* FEATURE_MMGSDI */

#if defined(FEATURE_ETSI_PBM) || defined(FEATURE_DSAT_CDMA_PBM)
  { DS_CMD_ATCOP_PBM_CB_CMD,         dsatme_pbm_cb_cmd_handler },
#endif /* defined(FEATURE_ETSI_PBM) || defined(FEATURE_DSAT_CDMA_PBM) */

  { DS_CMD_ATCOP_TO_SIO_LOW_CMD,     dsati_to_sio_low_cmd_handler },

#ifdef FEATURE_DSAT_ETSI_MODE
  { DS_CMD_ATCOP_CM_SUPS_CMD,        dsatetsicmif_cm_sups_cmd_handler_ex },
  { DS_CMD_ATCOP_CM_SUPS_INFO_CMD,   dsatetsicmif_cm_sups_event_handler_ex },
#endif /* FEATURE_DSAT_ETSI_MODE */

  { DS_CMD_ATCOP_FWD_CLIENT_REG,    dsatclient_register_fwd_client_handler },
  { DS_CMD_ATCOP_FWD_CLIENT_DEREG,  dsatclient_deregister_fwd_client_handler }
  { DS_CMD_ATCOP_FWD_AT_CMD_REG,    dsatclient_register_fwd_at_cmd_handler },
  { DS_CMD_ATCOP_FWD_AT_CMD_DEREG,  dsatclient_deregister_fwd_at_cmd_handler },
  { DS_CMD_ATCOP_RESET_REQ_CMD,      dsatclient_reset_cmd_request_handler},
  { DS_CMD_ATCOP_SHUTDOWN_REQ_CMD,   dsatclient_shutdown_cmd_request_handler},

#ifdef FEATURE_MMGSDI
  { DS_CMD_ATCOP_MMGSDI_INIT_INFO,   dsat_mmgsdi_init_handler },
#endif /* FEATURE_MMGSDI */

#ifdef FEATURE_DATA_MUX
  { DS_CMD_ATCOP_RDM_NOTIFY_CMD,     dsatact_rdm_notify_handler},
#endif /* FEATURE_DATA_MUX */

  /* need this here to appease the compiler when none of the
     above features is defined,
     DS_AT_ASYNC_EVENT_END should never be used for a real 
     event, ERR will be thrown if it is used */
  { DS_CMD_ATCOP_ASYNC_EVENT_END,    NULL }
};

const dsati_async_event_table_type async_event_table_ex =
{
  async_event_table_entries_ex,
  ARR_SIZE(async_event_table_entries_ex)
};
#endif /* FEATURE_DSAT_DYNAMIC_LOADING */

