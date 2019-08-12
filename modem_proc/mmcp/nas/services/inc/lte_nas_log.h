
#ifndef LTE_NAS_LOG_H
#define LTE_NAS_LOG_H
#include "mmcp_variation.h"
#include<customer.h>
#ifdef FEATURE_LTE
/*===============================================================================================
                             NASLOG.H

DESCRIPTION
This file contains or includes files that contain NAS log packet structure definitions, 
prototypes for NAS logging functions, any constant definitions that are needed, and any external
variable declarations needed for NAS logging.

Copyright (c) 2001 by Qualcomm Technologies, Inc.  All Rights Reserved.
================================================================================================*/


/*================================================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/services/inc/lte_nas_log.h#1 $
$Author: mplcsds1 $
$DateTime: 2016/12/13 08:00:05 $

when        who     what, where, why
--------    ---     ------------------------------------------------------------------------------
10/05/09    hnam    Added support for forbidden tracking are list & GUTI in EMM STATE log packet
24/06/09    hnam    Changed the extern definition param name for 
                    send_emm_state_log_info & send_emm_usimcard_mode_log_info
23/06/09    hnam    Added initial revision  
================================================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "log.h"
#include "naslog_v.h"
#include "sys.h"

#ifndef FEATURE_LTE
#include "log_codes_umts.h" /* This file needs to be moved to "diag\inc" instead of "diag\src"*/
#endif

//#define OLD_NAS_LOGGING_DESIGN 1
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/*------------------------------------------------------------
                     OTA LOG PACKETS
------------------------------------------------------------*/
extern void send_nas_ota_msg_log_packet
(
  word log_code,
  word nas_emm_msg_size,
  byte *nas_ota_msg_ptr
);

/*------------------------------------------------------------
                     ESM LOG PACKETS
------------------------------------------------------------*/

extern void send_esm_bearer_ctxt_state_log_info
(
  lte_nas_esm_bearer_context_state_T  *esm_bearer_ctxt_state_log
);

extern void send_esm_bearer_ctxt_log_info
(
  lte_nas_esm_bearer_context_info_T  *esm_bearer_ctxt_info_log
);

extern void send_esm_proc_state_log_info
(
  lte_nas_esm_procedure_state_T  *esm_proc_state_info_log
);

/*------------------------------------------------------------
                     EMM LOG PACKETS
------------------------------------------------------------*/

extern void send_emm_state_log_info
(
  byte                emm_state,
  byte                emm_substate, /*Contradicts -> emm_substate is of type 'word" in emm_database.h*/
  sys_plmn_id_s_type    *plmn,
  boolean                guti_valid,
  lte_nas_emm_guti_type  *guti
);

extern void send_emm_usimcard_mode_log_info
(
  lte_nas_emm_usim_card_mode_type  *emm_usim_card_mode_info
);

extern void send_emm_current_ctxt_sec_log_info
(
  lte_nas_emm_current_sec_context_type  *emm_current_sec_ctxt_log
);

extern void send_emm_ps_domain_int_cipher_log_info
(
  lte_nas_emm_ps_domain_int_cipher_keys  *emm_ps_domain_int_cipher_log
);

void send_emm_native_sec_ctxt_log_info
(
  lte_nas_emm_native_sec_context_type  *emm_native_sec_ctxt_log
);

extern void send_emm_usim_serv_table_log_info
(
  lte_nas_emm_usim_serv_table_type  *emm_usim_serv_table_log
);

extern void log_forbidden_tracking_area_info
(
  lte_nas_tai_lst2_type *forbidden_for_service_list,
  lte_nas_tai_lst2_type *rrc_forbidden_list
);

/*------------------------------------------------------------
                     OLD DESIGN (DEPRECATED)
------------------------------------------------------------*/
#if defined (OLD_NAS_LOGGING_DESIGN)
void send_nas_emm_log_packet
(
  word log_code    
);

void send_nas_esm_log_packet
(
  word log_code    
);
#endif
#endif

#endif /*FEATURE_LTE*/

