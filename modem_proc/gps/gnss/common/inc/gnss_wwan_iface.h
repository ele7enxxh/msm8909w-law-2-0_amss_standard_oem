#ifndef GNSS_WWAN_IFACE_H
#define GNSS_WWAN_IFACE_H
/*=============================================================================

                             GNSS WWAN Interface Header file

GENERAL DESCRIPTION
  This file contains the interface to WWAN function prototypes relevant for the entire GPS module.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
  


Copyright (c) 2013-2014 by QUALCOMM ATHEROS, INCORPORATED. All Rights Reserved.

Copyright (c) 2015 Qualcomm Technologies, Inc. 
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

=============================================================================*/


/*=============================================================================

                           EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  
when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/09/15   mj      Add support for W+W feature
05/22/13   mj      Initial version.

=============================================================================*/

/*=============================================================================

                                INCLUDE FILES

=============================================================================*/

#include "customer.h"
#include "comdef.h"
#include "gps_types.h"
#include "gps_consts.h"
#include "gnss_consts.h"
#include "cgps_api.h"
#include "tcxomgr.h"
#include "trm.h"
#include "sys.h"
#include "queue.h"
#include "qw.h"
#include "cmd.h"
#include "timetick.h"
#include "log.h"
#include "msg.h"
#include <stddef.h>
#include "memheap.h"
#include "err.h"
#include "tm_common.h"
#include "gps_variation.h"
#include "gps_common.h"
#include "aries_os_api.h"
#include "tm_api.h"
#include "mgp_api.h"
#include "msg.h" 
#include "tdsrrclsmif.h"
#include "rrccmd.h"

#ifdef FEATURE_SEGMENT_LOADING
  #include "IWCDMA.h"
  #include "ITDSCDMA.h"
#endif /* FEATURE_SEGMENT_LOADING */  


/*=============================================================================

                     DEFINITIONS AND DECLARATIONS FOR MODULE

=============================================================================*/


/*=============================================================================

       Constants

=============================================================================*/


/*=============================================================================

       Macros

=============================================================================*/


/*=============================================================================

       Typedefs

=============================================================================*/


/*=============================================================================

       Functions

=============================================================================*/
#ifdef FEATURE_SEGMENT_LOADING
interface_t *gnss_wwan_init_wcdma_handle(void);

interface_t *gnss_wwan_init_tdscdma_handle(void);
#endif /* FEATURE_SEGMENT_LOADING */


rrc_cmd_type *gnss_wwan_iface_rrc_get_lsm_cmd_buf(void);

void gnss_wwan_iface_rrc_put_lsm_cmd(rrc_cmd_type* rrc_cmd_ptr);

boolean gnss_wwan_iface_rrcmeas_get_cell_plmn_info(rrcmeas_plmn_cell_info_struct_type* z_CellInfoWCDMAGlobal);

boolean gnss_wwan_iface_tdsrrcmeas_get_cell_plmn_info(tdsrrcmeas_plmn_cell_info_struct_type* z_CellInfoTDSCDMAGlobal);

int16 gnss_wwan_iface_wcdma_query_tx_power_dbm(void);

l1_ext_cmd_type *gnss_wwan_iface_l1_get_cmd_buf(void);

void gnss_wwan_iface_l1_put_cmd(l1_ext_cmd_type* cmd_ptr);


#ifdef FEATURE_USER_PLANE_MRL
boolean gnss_wwan_iface_l1_cgps_register_event_cb(l1_cgps_event_cb_type event_cb_fp);
#ifdef FEATURE_CGPS_DUAL_WCDMA
boolean gnss_wwan_iface_wl1_cgps_register_event_cb(l1_cgps_event_cb_type event_cb_fp, sys_modem_as_id_e_type e_as_id);
#endif

#endif /* FEATURE_USER_PLANE_MRL */


boolean gnss_wwan_iface_rrcgps_register_cgps_event_cb( rrcgps_cgps_event_cb_type event_cb_fp );

boolean gnss_wwan_iface_rrcgps_register_cgps_ue_pos_capability_cb (rrcgps_cgps_ue_pos_capability_cb_type ue_cap_cb_fp);

boolean gnss_wwan_iface_rrcscr_get_signalling_connection_status (rrc_cn_domain_identity_e_type cn_domain_id);

uint8 gnss_wwan_iface_rrc_return_wcdma_rrc_version(void);

boolean gnss_wwan_iface_tdsl1_api_send_timetag_cmd (boolean allow_sleep_timetag);


#ifdef FEATURE_CGPS_DUAL_WCDMA
/* Support for W+W */
boolean gnss_wwan_iface_rrcgps_register_cgps_ue_pos_capability_cb_sub (rrcgps_cgps_ue_pos_capability_cb_type ue_cap_cb_fp, sys_modem_as_id_e_type e_as_id);

boolean gnss_wwan_iface_rrcgps_register_cgps_event_cb_sub( rrcgps_cgps_event_cb_type event_cb_fp, sys_modem_as_id_e_type e_as_id );

void gnss_wwan_iface_rrc_put_lsm_cmd_sub(rrc_cmd_type* rrc_cmd_ptr, sys_modem_as_id_e_type e_as_id );

boolean gnss_wwan_iface_rrcmeas_get_cell_plmn_info_sub(rrcmeas_plmn_cell_info_struct_type* z_CellInfoWCDMAGlobal, sys_modem_as_id_e_type e_as_id );

boolean gnss_wwan_iface_rrcscr_get_signalling_connection_status_sub(rrc_cn_domain_identity_e_type cn_domain_id, sys_modem_as_id_e_type e_as_id);

uint8 gnss_wwan_iface_rrc_return_wcdma_rrc_version_sub(sys_modem_as_id_e_type e_as_id);

void gnss_wwan_iface_wl1_put_cmd(l1_ext_cmd_type* cmd_ptr, sys_modem_as_id_e_type e_as_id);
#endif






#endif /* GNSS_WWAN_IFACE_H */
