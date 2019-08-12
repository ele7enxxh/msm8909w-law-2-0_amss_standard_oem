#ifndef WMSBC_H
#define WMSBC_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
           W I R E L E S S    M E S S A G I N G   S E R V I C E S
           wmsbc.h -- Broadcast Group definitions

  The WMS module which implements the User API for SMS. This source file
  defines the data types for the BC group.

  -------------

    Copyright (c) 2001-2014 QUALCOMM Technologies Incorporated.
    All Rights Reserved.
    Qualcomm Confidential and Proprietary

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/wms/inc_deprecated/wmsbc.h#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

#include "mmcp_variation.h"

#if defined(FEATURE_CDSMS) || defined(FEATURE_GWSMS)

/*===========================================================================
========================  INCLUDE FILES =====================================
===========================================================================*/

#include "wms.h"
#include "wmsi.h"

#ifdef FEATURE_IS2000_REL_A_CC_BCSMS
#include "wms_modem.h"
#endif /* FEATURE_IS2000_REL_A_CC_BCSMS */

#ifdef FEATURE_SMS_PWS_OVER_LTE
#include "lte_rrc_ext_msg.h"
#include "mm.h"
#include "time_types.h"
#endif /* FEATURE_SMS_PWS_OVER_LTE */

/*===========================================================================

                         DATA TYPE DECLARATIONS

===========================================================================*/
#ifdef FEATURE_SMS_PWS_OVER_LTE

/*======================================
  Duplicate detection types and definitions
  ======================================*/

#define WMS_BC_MCC_0_JA_MASK      0x04
#define WMS_BC_MCC_1_JA_MASK      0x04
#define WMS_BC_MCC_2_JA_MASK1     0x00
#define WMS_BC_MCC_2_JA_MASK2     0x01

#define WMS_BC_SECS_IN_HOUR       (60 * 60)

// Cell location information - PLMN/ TAC/ Cell ID
typedef struct wms_cell_info_struct
{
   lte_rrc_plmn_s           selected_plmn; /* req PLMN or its equivalent or first entry in SIB1 list for limited service */
   lte_rrc_global_cell_id_t cell_identity; /* Global Cell Id */
   lte_phy_cell_id_t        phy_cell_id;  /* Physical cell id */
   uint16                   tac; /* Tracking area code */
} wms_cell_info_s_type;

// List entry contents
typedef struct {
  lte_rrc_msg_id_t     message_id;
  uint16               serial_no;
  lte_phy_cell_id_t    cell_id;
  uint16               tac;
  lte_rrc_plmn_s       plmn;
  time_t               timestamp;
} wms_duplist_entry_type;

// List body
typedef struct {
  int                     size;
  int                     start_idx;
  int                     end_idx;
  int                     entry_count;
  wms_duplist_entry_type *entries;
} wms_duplist_list_type;

typedef enum wms_duplist_ret_status
{
  WMS_DUPLIST_BAD_PARAM          = -3,
  WMS_DUPLIST_FAILED_HOLEY       = -2,
  WMS_DUPLIST_FAILED_FULL        = -1,
  WMS_DUPLIST_SUCCESS            =  0,
  WMS_DUPLIST_SUCCESS_FULL       =  1,
  WMS_DUPLIST_SUCCESS_HOLEY      =  2
} wms_duplist_ret_status_t;

typedef boolean (* wms_bc_compare_dup_func_type)(
  wms_duplist_entry_type *ent1,
  wms_duplist_entry_type *ent2
);

// Valid geo Scope values
typedef enum
{
  WMS_BC_GEO_SCOPE_HIGH_PRIORITY_CELL_WIDE = 0,
  WMS_BC_GEO_SCOPE_PLMN_WIDE,
  WMS_BC_GEO_SCOPE_TA_WIDE,
  WMS_BC_GEO_SCOPE_LOW_PRIORITY_CELL_WIDE
} wms_bc_geo_scope_e_type;


#endif /* FEATURE_SMS_PWS_OVER_LTE */

/* Cell Broadcast service table data structure
 *
 * Defines the internal structure used to hold the runtime cell broadcast
 * configuration.  There are two groups of entries within the structure, one
 * is for CDMA, the other for GW.  The table contents are initially read from
 * NV and SIM card entries, depending on the technology.  After initialization
 * entries can be modified via client interaction, card refresh, etc.
 *
 * Under most conditions, WMS clients will interact with WMS via the public
 * WMS API. Some use cases require that fast, non-blocking searches of the
 * table data be performed, so a set of internal, non-public API's exists
 * (defined in wms_modem.h) for use by internal modem clients.
 * The three API methods are:
 *    wms_get_1x_bc_config()
 *    wms_bc_addr_match()
 *    wms_bc_gw_cb_filter()
 *
 * These API methods directly access internal table data, bypassing the WMS
 * command processing queue. Because they can be called asynchronously from
 * normal WMS processing, the table elements they reference must be maintained
 * and accessed in a manner such that they are stable/consistent while being
 * accessed through the internal methods.
 *
 * Protected table entries:
 *    bc_pref             (CDMA, single variable)
 *    bc_enable           (CDMA, single variable)
 *    bc_table_size       (CDMA, relates to bc_table)
 *    bc_table
 *    gw_cb_pref          (GW, single variable)
 *    gw_cb_enable        (GW, single variable)
 *    gw_cb_table_size    (GW, relates to gw_cb_table)
 *    gw_cb_table
 *
 * A critical section (see definition of wms_bc_crit_sect later in the code)
 * is defined to protect the data accesses.
 */
typedef struct wms_bc_struct
{
  uint32                dummy;

#ifdef FEATURE_SMS_PWS_OVER_LTE
  wms_duplist_list_type          *primary_lte_duplist;    // List for primary notification duplicate detection
  wms_duplist_list_type          *secondary_lte_duplist;  // List for secondary notification duplicate detection
  wms_duplist_list_type          *cmas_lte_duplist;  // List for secondary notification duplicate detection
#endif /* FEATURE_SMS_PWS_OVER_LTE */
} wms_bc_s_type;

/*===========================================================================

                       FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================
FUNCTION wms_bc_s_ptr                                              (INTERNAL)

DESCRIPTION
  Return the pointer to the static BC data.

DEPENDENCIES
  None

RETURN VALUE
  Pointer to the internal BC data.

SIDE EFFECTS
  None.
===========================================================================*/
wms_bc_s_type * wms_bc_s_ptr(
  void
);

/*===========================================================================
FUNCTION wms_bc_enable_check

DESCRIPTION
  Return the TRUE or FALSE based on whether broadcast is enabled/disabled.

DEPENDENCIES
  none

RETURN VALUE
  Pointer to the internal BC data.

SIDE EFFECTS
  None.
===========================================================================*/
boolean wms_bc_enable_check(
  wms_prov_app_info_s_type    *appInfo_ptr
);


/*===========================================================================
FUNCTION wms_bc_init

DESCRIPTION
  Do BC-related initialization.

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  BC data initialized.
===========================================================================*/
void wms_bc_init(
  void
);

void wms_bc_cleanup(
  void
);

/*===========================================================================
FUNCTION wms_bc_pre_init

DESCRIPTION
  Initialize the BC items which should only be done once at startup.
  Functions wms_bc_init() and wms_bc_gw_cb_init() may be called more
  than once as the bc service is disabled and enabled.

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  BC data initialized.
===========================================================================*/
void wms_bc_pre_init(
  void
);

#ifdef FEATURE_BROADCAST_SMS_MULTIMODE
/*===========================================================================
FUNCTION wms_bc_mm_process_cmd

DESCRIPTION
  Dispatch the BC MM command processing.

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wms_bc_mm_process_cmd(
  wms_cmd_type                       *cmd_ptr
);
#endif /* FEATURE_BROADCAST_SMS_MULTIMODE */

#ifdef FEATURE_GWSMS_BROADCAST
/*===========================================================================
FUNCTION wms_bc_gw_cb_init

DESCRIPTION
  Do BC-related initialization.

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  BC data initialized.
===========================================================================*/
void wms_bc_gw_cb_init(
  wms_prov_app_info_s_type           *appInfo_ptr,
  boolean                             do_sim_initialization
);

/*===========================================================================
FUNCTION wms_bc_gw_cb_pre_init

DESCRIPTION
  Do BC-related Pre-initialization.

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  BC data pre-initialized.
===========================================================================*/
void wms_bc_gw_cb_pre_init(
  void
);

/*===========================================================================
FUNCTION wms_bc_gw_cb_enable_to_nas

DESCRIPTION
  Send the GW CB Enable to the NAS Layer to Enable Cell Broadcast

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None
===========================================================================*/
void wms_bc_gw_cb_enable_to_nas(
  wms_prov_app_info_s_type           *appInfo_ptr
);

#endif /* FEATURE_GWSMS_BROADCAST */

#ifdef FEATURE_CDSMS_BROADCAST

void wms_bc_online_proc(
  wms_prov_app_info_s_type *appInfo_ptr
);

void wms_bc_enable_failure_proc(
  wms_cmd_type                       *cmd_ptr
);

#endif /* FEATURE_CDSMS_BROADCAST */


#ifdef FEATURE_CDSMS_BROADCAST
/*===========================================================================
FUNCTION wms_bc_scpt_data

DESCRIPTION
  Send an internal command to WMS task to process the Broadcast SCPT data

DEPENDENCIES
  None

RETURNS
  wms_status_e_type

SIDE EFFECTS
  None
===========================================================================*/
wms_status_e_type wms_bc_scpt_data(
   const wms_tl_message_type         *tl_ptr,
   wms_msg_transport_type_e_type      transport_type
);
#endif /* FEATURE_CDSMS_BROADCAST */

#ifdef FEATURE_SMS_PWS_OVER_LTE
/*======================================
  Duplicate detection types and definitions
  ======================================*/
// List identifiers
typedef enum wms_duplist_ident
{
  WMS_LTE_DUPLIST_PRIMARY,
  WMS_LTE_DUPLIST_SECONDARY,
  WMS_LTE_DUPLIST_CMAS
} wms_duplist_ident_t;

/*===========================================================================
FUNCTION wms_bc_lte_message_is_duplicate()

DESCRIPTION
  Checks if a received ETWS message is duplicate.

DEPENDENCIES
  None

PARAMETERS
  wms_duplist_ident_t      list_id      [IN]: Specifies primary or secondary list
  void                    *msg_ptr      [IN]: A pointer to the message to be searched for.
                                              The opaque pointer will reconstructed based on list_id
  time_t                   search_time  [IN]: Timestamp of the search

RETURNS
  boolean: TRUE; message is duplicate
           FALSE; message is not duplicate

SIDE EFFECTS
  When a message is not a duplicate, the duplicate detection history will
  be updated with the the message information contained in 'entry'.

  Any duplicate detection history entry found with an expiration timestamp
  less than the value in 'search_time' is purged during the search.

===========================================================================*/
boolean wms_bc_lte_message_is_duplicate(
  wms_duplist_ident_t                 list_id,
  void                               *msg_ptr,
  time_t                              search_time,
  wms_cell_info_s_type                cell_info
);

/*===========================================================================
FUNCTION wms_bc_clear_duplists()

DESCRIPTION
  Expires all entries in all duplists (ETWS primary, ETWS secondary, CMAS)

DEPENDENCIES
  None

PARAMETERS
  None

RETURNS
  Nothing

SIDE EFFECTS
  All duplicate detection history entries will be expired/purged.

===========================================================================*/
void wms_bc_clear_duplists(
  void
);

/*===========================================================================
FUNCTION
  boolean wms_bc_plmns_are_equal()

DESCRIPTION
  Compares two plmns to see if they are the same

DEPENDENCIES
  None

PARAMETERS
  lte_rrc_plmn_s     *plmn1     [IN]:
  lte_rrc_plmn_s     *plmn2     [IN]:

RETURNS
  boolean: TRUE; plmns are equal
           FALSE; plmns are not equal

SIDE EFFECTS
  None
===========================================================================*/
boolean wms_bc_plmns_are_equal(
  lte_rrc_plmn_s *plmn1,
  lte_rrc_plmn_s *plmn2
);

#endif /* FEATURE_SMS_PWS_OVER_LTE */

#endif /* FEATURE_CDSMS || FEATURE_GWSMS */

#endif /* WMSBC_H */
