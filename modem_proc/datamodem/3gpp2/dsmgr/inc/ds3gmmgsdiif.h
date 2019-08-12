#ifndef DS3GMMGSDIIF_H
#define DS3GMMGSDIIF_H
/*===========================================================================

               3 G   D A T A   S E R V I C E S   M A N A G E R

           M M G S D I   I N T E R F A C E   F U N C T I O N S

                            H E A D E R    F I L E

DESCRIPTION
  This file contains functions for interfacing to MMGSDI, including
  Data Services client initialization and processing of MMGSDI events. This
  header file is intended to be used internal to the 3G Dsmgr module only.


  Copyright (c) 2008-2014 by Qualcomm Technologies Incorporated. All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/dsmgr/inc/ds3gmmgsdiif.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/24/13   sd      TSTS changes for DS3G.
01/02/12   msh     Coan: Feature cleanup
09/09/11   vpk     Fix for empty username during AN authentication
09/05/11   ss      Changes to support RPM.
08/20/11   ttv     GSDI to MMGSDI_SESSION api migration changes.
04/11/11   ack     Removing FEATURE_UIM_SUPPORT_HOTSWAP wrap
04/11/11   ttv     Added support for Hot Swap feature.
01/31/11   op      Cleaning up usage of UIM API
01/04/11   op      Added support for ACL enable/disable flag
11/22/10   ttv     Added support for DSDS.
01/16/10   ls      All mmgsdi API shall be called under DS context
11/25/09   ls      Add support for retrieving slot info
11/09/09   ls      Add support for retrieving IMSI info from USIM card
10/21/09   ls      Support 1x pri/sec and GW pri/sec
07/17/09   ls      Removed CDMA service table info since UIM is taking care
                   of reading CDMA service table and update table. Change
                   FEATURE_CSIM to FEATURE_MMGSDI_SESSION_LIB
04/27/09   ls      Centralized MMGSDI interface
11/26/08   sn      Removed header files that are not used.
10/23/08   yz      Fixed featurization issues
10/10/08   yz      Initial version of the file.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#include "dstask_v.h"
#include "dstaski.h"
#include "dsati.h"

#ifdef FEATURE_MMGSDI

#include "mmgsdilib.h"
#include "mmgsdilib_v.h"
#include "uim_v.h"

#include "mmgsdisessionlib.h"
#include "ds3gsubsmgr.h"
#include "ds3geventmgr.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

#if defined(FEATURE_TRIPLE_SIM)
#define MAX_DS3G_MMGSDI_SESSIONS 6
#define MAX_DS3G_MMGSDI_NON_PROV_SESSIONS 3
#else
#define MAX_DS3G_MMGSDI_SESSIONS 4
#define MAX_DS3G_MMGSDI_NON_PROV_SESSIONS 2
#endif /* FEATURE_TRIPLE_SIM */

#define DS3G_MMGSDI_INVALID_CLIENT_ID     0x00000000FFFFFFFF

#define DS3G_MMGSDI_INVALID_SESSION_ID     0x00000000FFFFFFFF

#ifdef FEATURE_CDMA

/* The following table records MIP info reading from SIM 
   and it is specific for CDMA
 */
typedef struct 
{
  uint8 mip_response[UIM_COMPUTE_IP_AUTH_RESPONSE_LENGTH]; 
                            /* mip response */
  boolean status;           /* MIP compute_ip status  */
} ds3g_mmgsdi_mip_info_type;

#endif /* FEATURE_CDMA */

/* The following table records IMSI info reading from USIM 
   and it is specific for EHRPD
 */
#define DS3G_MMGSDI_IMSI_LEN         9
#define DS3G_MMGSDI_MNC_LEN          1
#define DS3G_MMGSDI_MNC_2_DIGITS     2
#define DS3G_MMGSDI_MNC_3_DIGITS     3
#define DS3G_MMGSDI_MIN_IMSI_LEN     5  /* MCC len: 3, MNC len: 2/3 */
#define DS3G_MMGSDI_MAX_IMSI_LEN     15  

typedef struct 
{
  uint8 imsi_response[DS3G_MMGSDI_IMSI_LEN];
  uint8 mnc_response[DS3G_MMGSDI_MNC_LEN];
  nv_ehrpd_imsi_type imsi_info;
  boolean ready_status;     /* IMSI info is ready for reading */
} ds3g_mmgsdi_imsi_info_type;

typedef struct 
{
  mmgsdi_data_type impi_info;
  nv_ehrpd_imsi_type imsi_info;
  boolean ready_status;     /* IMPI info is ready for reading */
} ds3g_mmgsdi_impi_info_type;

#ifdef FEATURE_EHRPD

typedef struct 
{
  uint8 *acl_response;
  mmgsdi_len_type acl_response_len;
  boolean acl_read_from_usim;
  boolean acl_is_enabled;
} ds3g_mmgsdi_acl_info_type;

#endif /* FEATURE_EHRPD */

/* The following table records client id, session id */
typedef struct
{
  mmgsdi_session_id_type session_id;
      /* Session Id  */
  mmgsdi_session_type_enum_type session_type;
      /* Session Type  */
  mmgsdi_app_enum_type   app_type;
      /* Application type */
  mmgsdi_slot_id_enum_type  slot;
      /* Slot info */
  boolean  is_operational;
      /* Received Subscription Ready Event from MMGSDI */
  sys_modem_as_id_e_type    sub_id;
      /* Subscription ID assigned to this session type */
} ds3g_mmgsdi_session_info_type;

typedef struct 
{
  mmgsdi_client_id_type client_id;
      /* Client Id that MMGSDI assigned to DS */
  ds3g_mmgsdi_session_info_type ds3g_mmgsdi_session_info[MAX_DS3G_MMGSDI_SESSIONS];
} ds3g_mmgsdi_info_type;

/* The following events are set when receiving MMGSDI events */
typedef enum
{
  DS3G_MMGSDI_IDLE_EVENT = 0,
  /* Initilized value */
  DS3G_MMGSDI_SUBSCRIPTION_READY_EVENT = 1,
  /* Received MMGSDI_SUBSCRIPTION_READY_EVT */
  DS3G_MMGSDI_REFRESH_VOTING_EVENT  = 2,
  /* Received MMGSDI_REFRESH_EVT and stage is MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_INIT */
  /* ISIM voting feature is interested at this event */
  DS3G_MMGSDI_OK_REFRESH_EVENT = 3,
  /* Received MMGSDI_REFRESH_EVT and stage is MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_FCN */
  /* both CDMA session and UMTS session are interested at this event */
  DS3G_MMGSDI_REFRESH_START_EVENT = 4,
  /* Received MMGSDI_REFRESH_EVT and stage is MMGSDI_REFRESH_STAGE_START */
  /* Right now, only 1x is interested at this event */
  DS3G_MMGSDI_REFRESH_COMPLETE_EVENT = 5,
  /* this event is received when SIM card's refresh 
     is triggered */  
  DS3G_MMGSDI_SIM_READ_EVENT = 6,
  /* When receiving  MMGSDI_SUBSCRIPTION_READY_EVT, DS will send request to check
     CDMA service table. DS will set this event when receiving the call back of
     mmgsdi_session_read_transparent
     */
  DS3G_MMGSDI_SIM_WRITE_EVENT = 7,
  /* After reading info from CDMA service table. DS will set update
     CDMA operation table, this event is set when receiving the call back of
     mmgsdi_session_write_transparent
     */
  DS3G_MMGSDI_SRV_AVAIL_EVENT = 8,
  /* this event is received when receiving the callback of
  mmgsdi_session_is_service_available
     */
  DS3G_MMGSDI_IMSI_INFO_EVENT = 9,
  /* this event is received when receiving the callback of
  imsi info */
  DS3G_MMGSDI_MNC_INFO_EVENT = 10,
  /* this event is received when receiving the callback of
  mnc info */
  DS3G_MMGSDI_CLIENT_ID_EVENT = 11,
  /* this event is received when receiving the callback of
  mmgsdi_client_id_and_evt_reg */
  DS3G_MMGSDI_ACL_INFO_EVENT = 12,
  /* this event is received when receiving the callback of
  acl info */
  DS3G_MMGSDI_ACL_ENABLED_EVENT = 13,
  /* this event is received when receiving the callback of
  mmgsdi_session_get_app_capabilities */
  DS3G_MMGSDI_CARD_ERROR_EVENT = 14,
  /* this event is received when SIM card is removed */
  DS3G_MMGSDI_SESSION_CHANGED_EVENT = 15,
  /* this event is received when session is changed */
  DS3G_MMGSDI_ISIM_REFRESH_START_EVENT = 16,
  /* this event is received when ISIM refresh is started */
  DS3G_MMGSDI_ISIM_REFRESH_COMPLETE_EVENT = 17,
  /* this event is received when ISIM refresh is complete */
  DS3G_MMGSDI_EFACC_READ_CNF_EVENT  = 18,
  /* this event is received when callback of EFacc Read */
  DS3G_MMGSDI_ISIM_OK_REFRESH_EVENT = 19
  /* Received MMGSDI_REFRESH_EVT and stage is MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_FCN */
  /* ISIM session is interested at this event */
} ds3g_mmgsdi_event_enum_type;

typedef struct
{
  ds3g_mmgsdi_event_enum_type           event_type;
  mmgsdi_session_id_type                session_id;
  mmgsdi_app_enum_type                  app_type;
  mmgsdi_slot_id_enum_type              slot;
  mmgsdi_session_changed_evt_info_type  session_changed_info;
  union
  {
    ds_at_mmgsdi_event_type     mmgsdi_event_info;
    ds_at_mmgsdi_sim_info_type  mmgsdi_sim_info;
  }cmd_info;
  mmgsdi_refresh_mode_enum_type refresh_mode;
} ds3g_mmgsdi_event_info_type;

typedef struct
{
  mmgsdi_session_type_enum_type session_type;
  uint8                         subscription_ready_status;
} ds3g_mmgsdi_subscription_ready_bit_mask_type;

typedef struct
{
  boolean                       is_operational;
  mmgsdi_session_id_type        session_id;
  mmgsdi_session_type_enum_type session_type;
  int32                         channel_id;
  mmgsdi_slot_id_enum_type      slot_id;
  mmgsdi_aid_type               app_info;
} ds3g_mmgsdi_non_prov_isim_session_info_type;

/* callback for subscription ready event*/
typedef void (ds3g_mmgsdi_subscription_ready_cb)(boolean IsReady);

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION      DS3G_MMGSDI_INIT

DESCRIPTION   This Function registers DS manager as a client of MMGSDI

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3g_mmgsdi_init
(
  void
);
#ifdef FEATURE_CDMA
/*==============================================================================
FUNCTION  DS3G_MMGSDI_1X_REGISTER_FOR_REFRESH

DESCRIPTION
  Will Register 1X CDMA for File Change Notifications for EF's.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/
void ds3g_mmgsdi_1x_register_for_refresh
(
  mmgsdi_session_type_enum_type session_type
);
#endif /* FEATURE_CDMA */

/*===========================================================================

FUNCTION  DS3G_MMGSDI_CLIENT_ID_EVENT_PROCESS

DESCRIPTION
  This function is the handler function for DS3G_MMGSDI_CLIENT_ID_EVENTS

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds3g_mmgsdi_client_id_event_process
(
  ds_cmd_type         * cmd_ptr              /* DS Command pointer         */
);
/*===========================================================================

FUNCTION DS3G_MMGSDI_SUBSCRIPTION_READY_EVENT_PROCESS

DESCRIPTION
This function processes event when receiving MMGSDI_SUBSCRIPTION_READY_EVENT
and it should be called by ds3g_mmgsdi_process_cmd()

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void  ds3g_mmgsdi_subscription_ready_event_process
(
  ds_cmd_type  *cmd_ptr
);
/*===========================================================================

FUNCTION  DS3G_MMGSDI_SIM_WRITE_EVENT_PROCESS

DESCRIPTION
  This function is the handler function for the MMGSDI SIM WRITE events

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds3g_mmgsdi_sim_write_event_process
(
  ds_cmd_type         * cmd_ptr              
);
/*===========================================================================

FUNCTION DS3G_MMGSDI_REFRESH_VOTING_EVENT_PROCESS

DESCRIPTION
This function processes event when receiving MMGSDI_REFRESH_EVT with
MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_INIT from MMGSDI interface. 
This function should be called by ds3g_mmgsdi_process_cmd().

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds3g_mmgsdi_refresh_voting_event_process
(
  ds_cmd_type  *cmd_ptr
);

/*===========================================================================

FUNCTION DS3G_MMGSDI_OK_REFRESH_EVENT_PROCESS

DESCRIPTION
This function processes event when receiving MMGSDI_REFRESH_EVT with 
MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_FCN from MMGSDI interface. 
This function should be called by ds3g_mmgsdi_process_cmd().

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds3g_mmgsdi_ok_refresh_event_process
(
  ds_cmd_type  *cmd_ptr
);

/*===========================================================================

FUNCTION DS3G_MMGSDI_REFRESH_FCN_EVENT_PROCESS

DESCRIPTION
This function processes event when receiving file change notification from 
MMGSDI interface. This function should be called by ds3g_mmgsdi_process_cmd()

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds3g_mmgsdi_refresh_fcn_event_process
(
  ds_cmd_type  *cmd_ptr
);

/*===========================================================================

FUNCTION DS3G_MMGSDI_ISIM_OK_REFRESH_EVENT_PROCESS

DESCRIPTION
This function processes event when receiving MMGSDI_REFRESH_EVT with
MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_FCN from MMGSDI interface.
This function should be called by ds3g_mmgsdi_process_cmd().

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds3g_mmgsdi_isim_ok_refresh_event_process
(
  ds_cmd_type  *cmd_ptr
);

/*===========================================================================

FUNCTION DS3G_MMGSDI_ISIM_REFRESH_START_PROCESSING

DESCRIPTION
  This function processes event when receiving MMGSDI_REFRESH_EVT with 
  MMGSDI_REFRESH_STAGE_START from MMGSDI interface. 
  This function should be called by ds3g_mmgsdi_process_cmd().

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds3g_mmgsdi_isim_refresh_start_processing
(
  ds_cmd_type  *cmd_ptr
);

/*===========================================================================

FUNCTION DS3G_MMGSDI_ISIM_REFRESH_COMPLETE_PROCESSING

DESCRIPTION
  This function processes event when receiving MMGSDI_REFRESH_EVT with 
  MMGSDI_REFRESH_STAGE_END_SUCCESS from MMGSDI interface. 
  This function should be called by ds3g_mmgsdi_process_cmd().

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds3g_mmgsdi_isim_refresh_complete_processing
(
  ds_cmd_type  *cmd_ptr
);

/*===========================================================================

FUNCTION  DS3G_MMGSDI_SIM_READ_EVENT_PROCESS

DESCRIPTION
  This function is the handler function for the MMGSDI SIM events

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds3g_mmgsdi_sim_read_event_process
(
  ds_cmd_type         * cmd_ptr              /* DS Command pointer         */
);

#ifdef FEATURE_EHRPD
/*===========================================================================
FUNCTION  DS3G_MMGSDI_ACL_INFO_EVENT_PROCESS

DESCRIPTION
  This function is the handler function for DS3G_MMGSDI_ACL_INFO_EVENTS

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds3g_mmgsdi_acl_info_event_process
(
  ds_cmd_type         * cmd_ptr              /* DS Command pointer         */
);

/*===========================================================================
FUNCTION  DS3G_MMGSDI_ACL_ENABLED_EVENT_PROCESS

DESCRIPTION
  This function is the handler function for DS3G_MMGSDI_ACL_ENABLED_EVENT

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds3g_mmgsdi_acl_enabled_event_process
(
  ds_cmd_type         * cmd_ptr              /* DS Command pointer         */
);

#endif /* FEATURE_EHRPD */

/*===========================================================================

FUNCTION  DS3G_MMGSDI_CARD_REMOVED_EVENT_PROCESS

DESCRIPTION
  This function is the handler function for the MMGSDI CARD ERROR event

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void
ds3g_mmgsdi_card_error_event_process
(
  ds_cmd_type         * cmd_ptr              /* DS Command pointer         */
);

/*===========================================================================

FUNCTION  DS3G_MMGSDI_SRV_ABAIL_EVENT_PROCESS

DESCRIPTION
  This function is the handler function for the MMGSDI SRV AVAIL events

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds3g_mmgsdi_srv_avail_event_process
(
  ds_cmd_type         * cmd_ptr              /* DS Command pointer         */
);
/*===========================================================================

FUNCTION DS3G_MMGSDI_PROCESS_CMD

DESCRIPTION
  A command processing function is called based on the type of command received.

  This function should be called by the top-level task dispatcher whenever
  any MMGSDI command is received.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds3g_mmgsdi_process_cmd
(
  ds_cmd_type* cmd_ptr
);

/*==============================================================================
FUNCTION  DS3G_MMGSDI_READ_TRANSPARENT

DESCRIPTION
  This functions provides read access to any transparent files using MMGSDI
  interface

DEPENDENCIES
  None.

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None

==============================================================================*/
mmgsdi_return_enum_type ds3g_mmgsdi_read_transparent
(
  mmgsdi_access_type            access,
  mmgsdi_offset_type            offset,
  mmgsdi_len_type               req_len,
  mmgsdi_callback_type          response_cb_ptr,
  mmgsdi_session_type_enum_type session_type
);
/*==============================================================================
FUNCTION  DS3G_MMGSDI_WRITE_TRANSPARENT

DESCRIPTION
  This functions provides write access to any transparent files using MMGSDI 
  interface

DEPENDENCIES
  None.

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None

==============================================================================*/
mmgsdi_return_enum_type ds3g_mmgsdi_write_transparent
(
  mmgsdi_access_type             file_access,
  mmgsdi_write_data_type         write_data,
  mmgsdi_callback_type           response_cb_ptr,
  mmgsdi_session_type_enum_type  session_type
);

/*==============================================================================
FUNCTION  DS3G_MMGSDI_SERVICE_AVAILABLE

DESCRIPTION
  This functions provides service availble functionality using MMGSDI 
  interface

DEPENDENCIES
  None.

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None

==============================================================================*/
mmgsdi_return_enum_type ds3g_mmgsdi_service_available
(
  mmgsdi_service_enum_type       srvc_type,
  mmgsdi_callback_type           response_cb_ptr,
  mmgsdi_session_type_enum_type  session_type
);

/*==============================================================================
FUNCTION  DS3G_MMGSDI_SESSION_GET_APP_CAPABILITIES

DESCRIPTION
  This functions requests app capabilities using MMGSDI interface

DEPENDENCIES
  None.

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None

==============================================================================*/
mmgsdi_return_enum_type ds3g_mmgsdi_session_get_app_capabilities
(
  mmgsdi_session_type_enum_type  session_type,
  mmgsdi_callback_type           response_cb_ptr,
  mmgsdi_client_data_type        client_ref
);

/*==============================================================================
FUNCTION  DS3G_MMGSDI_REGISTER_FOR_REFRESH

DESCRIPTION
  This function registers file change notification using MMGSDI interface

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None

==============================================================================*/
mmgsdi_return_enum_type ds3g_mmgsdi_register_for_refresh
(
  mmgsdi_refresh_file_list_type refresh_files,
  mmgsdi_session_type_enum_type session_type
);
/*==============================================================================
FUNCTION  DS3G_MMGSDI_REFRESH_COMPLETE

DESCRIPTION
    This functions inform MMGSDI when it has finished all the files that were 
  refreshed

DEPENDENCIES
  None.

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None

==============================================================================*/
mmgsdi_return_enum_type ds3g_mmgsdi_refresh_complete
(
  boolean  pass_fail,
  mmgsdi_session_type_enum_type session_type
);
/*==============================================================================
FUNCTION  DS3G_MMGSDI_OK_TO_REFRESH

DESCRIPTION
  This functions sends OK to refresh to MMGSDI interface

DEPENDENCIES
  None.

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None

==============================================================================*/
mmgsdi_return_enum_type ds3g_mmgsdi_ok_to_refresh
(
  boolean ok_to_refresh,
  mmgsdi_session_type_enum_type session_type
);

#ifdef FEATURE_CDMA

/*==============================================================================

FUNCTION DS3G_MMGSDI_SESSION_COMPUTE_IP_AUTH

DESCRIPTION
  This function sends compute IP authentication to MMGSDI interface. 
  This function is called by dsmip_auth

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type ds3g_mmgsdi_session_compute_ip_auth
(
  mmgsdi_compute_ip_data_type        compute_ip_data
);
/*===========================================================================
FUNCTION DS3G_MMGSDI_MIP_DATA_RETRIEVE()

DESCRIPTION
  Retrieves the MN-AAA/RRQ/MN-HA Response from the mmgsdi response saved into
  mip table

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds3g_mmgsdi_mip_data_retrieve
(
  void*    data_ptr
);
#endif /* FEATURE_CDMA */
/*==============================================================================
FUNCTION  DS3G_MMGSDI_VALIDATE_CLIENT_ID

DESCRIPTION
  Validate provided client id with client id getting from MMGSDI interface. 

DEPENDENCIES
  None.

RETURN VALUE
  TRUE: Valid client id
  FALSE: Invalid client id

SIDE EFFECTS
  None

==============================================================================*/
boolean ds3g_mmgsdi_validate_client_id
(
  mmgsdi_client_id_type client_id
);

/*==============================================================================
FUNCTION  DS3G_MMGSDI_GET_SESSION_ID

DESCRIPTION
  Get session id from ds3g_mmgsdi_info based on session_type

DEPENDENCIES
  None.

RETURN VALUE
  TRUE: Successfully getting session id from ds3g_mmgsdi_info
  FALSE: Failed to get session id from ds3g_mmgsdi_info

SIDE EFFECTS
  None

==============================================================================*/
boolean ds3g_mmgsdi_get_session_id
(
  mmgsdi_session_type_enum_type session_type,
  mmgsdi_session_id_type* session_id_ptr
);
/*==============================================================================
FUNCTION  DS3G_MMGSDI_GET_APP_TYPE

DESCRIPTION
  Get session app type from ds3g_mmgsdi_info based on session_type

DEPENDENCIES
  None.

RETURN VALUE
  TRUE: Successfully getting app type from ds3g_mmgsdi_info
  FALSE: Failed to get app type from ds3g_mmgsdi_info

SIDE EFFECTS
  None

==============================================================================*/
boolean ds3g_mmgsdi_get_app_type
(
  mmgsdi_session_type_enum_type session_type,
  mmgsdi_app_enum_type* app_type_ptr
);
/*==============================================================================
FUNCTION  DS3G_MMGSDI_GET_SLOT

DESCRIPTION
  Get slot info from ds3g_mmgsdi_info based on session_type

DEPENDENCIES
  None.

RETURN VALUE
  TRUE: Successfully getting slot info from ds3g_mmgsdi_info
  FALSE: Failed to get slot info from ds3g_mmgsdi_info

SIDE EFFECTS
  None

==============================================================================*/
boolean ds3g_mmgsdi_get_slot
(
  mmgsdi_session_type_enum_type session_type,
  mmgsdi_slot_id_enum_type* slot_ptr
);

/*===========================================================================
FUNCTION DS3G_MMGSDI_GET_SUB_ID_FROM_SLOT

DESCRIPTION
  This function returns the subs id based on slot.

PARAMETERS
  slot : mmgsdi session slot.

RETURN VALUE
  sub_id : sub_id corresponding to the given session_type

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
sys_modem_as_id_e_type ds3g_mmgsdi_get_sub_id_from_slot
(
  mmgsdi_slot_id_enum_type slot
);

/*==============================================================================
FUNCTION  DS3G_MMGSDI_GET_SESSION_TYPE

DESCRIPTION
  Get CDMA/UMTS session type based on session id

DEPENDENCIES
  None.

RETURN VALUE
  TRUE: Getting session type successfully
  FALSE: Failed to get session type

SIDE EFFECTS
  None

==============================================================================*/
boolean ds3g_mmgsdi_get_session_type
(
  mmgsdi_session_id_type session_id,
  mmgsdi_session_type_enum_type* session_type_ptr
);

/*===========================================================================

FUNCTION  DS3G_MMGSDI_IMSI_INFO_EVENT_PROCESS

DESCRIPTION
  This function is the handler function for DS3G_MMGSDI_IMSI_INFO_EVENTS

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds3g_mmgsdi_imsi_info_event_process
(
  ds_cmd_type         * cmd_ptr              /* DS Command pointer         */
);

/*===========================================================================

FUNCTION  DS3G_MMGSDI_MNC_INFO_EVENT_PROCESS

DESCRIPTION
  This function is the handler function for DS3G_MMGSDI_MNC_INFO_EVENTS

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds3g_mmgsdi_mnc_info_event_process
(
  ds_cmd_type         * cmd_ptr              /* DS Command pointer         */
);

/*===========================================================================
FUNCTION DS3G_MMGSDI_GET_IMSI_DATA()

DESCRIPTION
  Passed cached IMSI data to client 

DEPENDENCIES
  None

RETURN VALUE
  FALSE: Failed to read IMSI data
  TRUE: Read IMSI data successfully

SIDE EFFECTS
  None
===========================================================================*/
boolean ds3g_mmgsdi_get_imsi_data
(
  nv_ehrpd_imsi_type* imsi_data_ptr
);

/*===========================================================================
FUNCTION DS3G_MMGSDI_GET_IMPI_DATA()

DESCRIPTION
  Passed cached IMPI data to client 

DEPENDENCIES
  None

RETURN VALUE
  FALSE: Failed to read IMPI data
  TRUE: Read IMPI data successfully

SIDE EFFECTS
  None
===========================================================================*/
boolean ds3g_mmgsdi_get_impi_data
(
  mmgsdi_data_type       *impi_data_ptr,
  nv_ehrpd_imsi_type     *imsi_data_ptr
);

/*===========================================================================
FUNCTION DS3G_MMGSDI_GET_IMSI_DATA_PER_SUB

DESCRIPTION
  Passed cached IMSI data to client 

DEPENDENCIES
  None

RETURN VALUE
  FALSE: Failed to read IMSI data
  TRUE: Read IMSI data successfully

SIDE EFFECTS
  None
===========================================================================*/

boolean ds3g_mmgsdi_get_imsi_data_per_subs_id
(
  nv_ehrpd_imsi_type*    imsi_data_ptr,
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================
FUNCTION DS3G_MMGSDI_GET_IMPI_DATA_PER_SUB

DESCRIPTION
  Passed cached IMPI data to client 

DEPENDENCIES
  None

RETURN VALUE
  FALSE: Failed to read IMPI data
  TRUE: Read IMPI data successfully

SIDE EFFECTS
  None
===========================================================================*/

boolean ds3g_mmgsdi_get_impi_data_per_subs_id
(
  sys_modem_as_id_e_type  subs_id,
  mmgsdi_data_type       *impi_data_ptr,
  nv_ehrpd_imsi_type     *imsi_data_ptr
);

/*===========================================================================
FUNCTION DS3G_MMGSDI_UPDATE_SUB_ID()

DESCRIPTION
  Update subscription id into mmgsdi table

DEPENDENCIES
  None

RETURN VALUE
  FALSE: Failed to update subscription id
  TRUE: Update subscription id successfully

SIDE EFFECTS
  None
===========================================================================*/
boolean ds3g_mmgsdi_update_sub_id
(
  mmgsdi_session_type_enum_type session_type,
  sys_modem_as_id_e_type        sub_id
);

/*===========================================================================
FUNCTION ds3g_mmgsdi_clear_subs_id_ready_mask

DESCRIPTION
  Function to clear subs_id_ready_mask.

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None
===========================================================================*/
void ds3g_mmgsdi_clear_subs_id_ready_mask
(
  sys_modem_as_id_e_type         subs_id
);

/*===========================================================================
FUNCTION DS3G_MMGSDI_GET_SESSION_ID_FROM_SUB_ID()

DESCRIPTION
  Get session id from mmgsdi table provided subscription id

DEPENDENCIES
  None

RETURN VALUE
  FALSE: Failed to get session id
  TRUE: Get session id successfully

SIDE EFFECTS
  None
===========================================================================*/
boolean ds3g_mmgsdi_get_session_id_from_sub_id
(
  sys_modem_as_id_e_type  sub_id,
  mmgsdi_session_id_type* session_id_ptr
);

#ifdef FEATURE_MMGSDI_SESSION_LIB
/*===========================================================================
FUNCTION DS3G_MMGSDI_SESSION_READ_RECORD()

DESCRIPTION
  This function internally calls mmgsdi_session_read_record.

PARAMETERS
  session_id      : Session ID.
  file_access     : Info about the file need to be read.
  record_number   : Number of the record need to read.
  request_length  : Number of bytes need to be read.
  response_cb_ptr : Callback called after read is complete.
  client_ref      : Client reference.

RETURN VALUE
  MMGSDI_SUCCESS - If mmgsdi_session_read_record returns success.
  MMGSDI_ERROR   - If any Error Scenario occured.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type 
ds3g_mmgsdi_session_read_record
(
  mmgsdi_session_id_type session_id,
  mmgsdi_access_type file_access,
  mmgsdi_rec_num_type record_number,
  mmgsdi_len_type request_length,
  mmgsdi_callback_type response_cb_ptr,
  mmgsdi_client_data_type client_ref
);

/*===========================================================================
FUNCTION DS3G_MMGSDI_SESSION_GET_FILE_ATTR()

DESCRIPTION
  This function internally calls mmgsdi_session_get_file_attr.

PARAMETERS
  session_id      : Session ID.
  file_name       : File name
  response_cb_ptr : Callback called after read is complete.
  client_ref      : Client reference.

RETURN VALUE
  MMGSDI_SUCCESS - If mmgsdi_session_read_record returns success.
  MMGSDI_ERROR   - If any Error Scenario occured.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type
ds3g_mmgsdi_session_get_file_attr(
  mmgsdi_session_id_type session_id,
  mmgsdi_access_type file_name,
  mmgsdi_callback_type response_cb_ptr,
  mmgsdi_client_data_type client_ref
);

/*===========================================================================
FUNCTION DS3G_MMGSDI_SESSION_JCDMA_GET_CARD_INFO()

DESCRIPTION
  This function internally calls mmgsdi_session_jcdma_get_card_info.

PARAMETERS
  session_id      : Session ID.
  response_cb_ptr : Callback called after read is complete.
  client_ref      : Client reference.

RETURN VALUE
  MMGSDI_SUCCESS - If mmgsdi_session_read_record returns success.
  MMGSDI_ERROR   - If any Error Scenario occured.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type
ds3g_mmgsdi_session_jcdma_get_card_info
(
  mmgsdi_session_id_type             session_id,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref
);

/*===========================================================================
FUNCTION DS3G_MMGSDI_GET_SESSION_ID_FROM_SESSION_TYPE()

DESCRIPTION
  This function returns the session_id based on session_type.

PARAMETERS
  session_type : mmgsdi session type.

RETURN VALUE
  session_id : session_id corresponding to the given session_type

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_session_id_type
ds3g_mmgsdi_get_session_id_from_session_type
(
  mmgsdi_session_type_enum_type session_type
);

/*===========================================================================
FUNCTION DS3G_MMGSDI_GET_SUB_ID_FROM_SESSION_TYPE()

DESCRIPTION
  This function returns the session_id based on session_type.

PARAMETERS
  session_type : mmgsdi session type.

RETURN VALUE
  sub_id : sub_id corresponding to the given session_type

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
sys_modem_as_id_e_type
ds3g_mmgsdi_get_sub_id_from_session_type
(
  mmgsdi_session_type_enum_type session_type
);

/*===========================================================================
FUNCTION DS3G_MMGSDI_GET_SESSION_TYPE_FROM_SESSION_id()

DESCRIPTION
  This function returns the session_type based on session_id.

PARAMETERS
  session_id : mmgsdi session id.

RETURN VALUE
  session_type : session_type corresponding to the given session_id.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_session_type_enum_type
ds3g_mmgsdi_get_session_type_from_session_id
(
  mmgsdi_session_id_type session_id
);

/*===========================================================================
FUNCTION DS3G_MMGSDI_GET_APP_TYPE_FROM_SESSION_id()

DESCRIPTION
  This function returns the app_type based on session_id.

PARAMETERS
  session_id : mmgsdi session id.

RETURN VALUE
  app_type : app_type corresponding to the given session_id.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_app_enum_type
ds3g_mmgsdi_get_app_type_from_session_id
(
  mmgsdi_session_id_type session_id
);

#endif /* FEATURE_MMGSDI_SESSION_LIB */

#ifdef FEATURE_HDR
/*===========================================================================
FUNCTION      DS3G_MMGSDI_REGISTER_FOR_SUBSCRIPTION_READY

DESCRIPTION 
  This function notifies the callback for when subscription is ready
 
DEPENDENCIES 
  None.

RETURN VALUE 
  None.

SIDE EFFECTS 
  None.
===========================================================================*/
void ds3g_mmgsdi_register_for_subscription_ready
(
  ds3g_mmgsdi_subscription_ready_cb *callback_ptr
);
#endif /* FEATURE_HDR */

/*===========================================================================
FUNCTION DS3G_MMGSDI_GET_SUB_ID_FROM_SESSION_ID

DESCRIPTION
  Get subs id from mmgsdi table provided session type

DEPENDENCIES
  None

RETURN VALUE
  SUBS ID

SIDE EFFECTS
  None
===========================================================================*/
sys_modem_as_id_e_type ds3g_mmgsdi_get_sub_id_from_session_id
(
  mmgsdi_session_id_type session_id
);

/*==============================================================================
FUNCTION DS3G_MMGSDI_READ_EFACC

DESCRIPTION
  Read EF-ACC information after receiving Subscription Ready/Refresh Events 
  for GW sessions.

DEPENDENCIES
  None.
  
PARAMETERS
  mmgsdi_session_type_enum_type session_type.

RETURN VALUE
  TRUE  - if the read is successful.
  FALSE - otherwise.

SIDE EFFECTS
  None.

==============================================================================*/
boolean ds3g_mmgsdi_read_efacc
(
  mmgsdi_session_type_enum_type session_type
);

/*===========================================================================
FUNCTION DS3G_MMGSDI_IS_EFACC_CLASS_SET

DESCRIPTION
  Returns true if any of the bits from b4-b8 is set in the byte 1 
  i.e 11 -15 as shown below. 

  Byte 1:
  b8 b7 b6 b5 b4 b3 b2 b1
  15 14 13 12 11 10 09 08 Number of the ACC (except for bit b3)

  if any of the bits from 11-15 are enabled the functions returns true
  Spec reference: TS 3GPP: 31.102
  Byte 2:
  b8 b7 b6 b5 b4 b3 b2 b1
  07 06 05 04 03 02 01 00 Number of the ACC

PARAMETERS
  sys_modem_as_id_e_type subs_id.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE  - if bits b4-b8 are set in the byte 1 of ACC.
  FALSE - otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds3g_mmgsdi_is_efacc_class_set
(
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================
FUNCTION DS3G_MMGSDI_EFACC_READ_RESP_HDLR_CB

DESCRIPTION
  Process EFACC data from the mmgsdi response, and posts a cmd to dstask for read 
  confirmation.This is a callback function is called from  mmgsdi task
  context.

PARAMETERS
  mmgsdi_return_enum_type   status
  mmgsdi_cnf_enum_type      cnf
  const mmgsdi_cnf_type     *cnf_ptr
  
DEPENDENCIES
  None.

RETURN VALUE  
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds3g_mmgsdi_efacc_read_resp_hdlr_cb
(
  mmgsdi_return_enum_type status,
  mmgsdi_cnf_enum_type	  cnf,
  const mmgsdi_cnf_type   *cnf_ptr
);

/*===========================================================================
FUNCTION DS3G_MMGSDI_EFACC_REGISTER_FOR_REFRESH

DESCRIPTION
  This function registers for refresh EFacc files on SIM/USIM MMGSDI interface.

PARAMETERS
  mmgsdi_session_type_enum_type session_type
  
DEPENDENCIES
  None.

RETURN VALUE  
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds3g_mmgsdi_efacc_register_for_refresh
(
  mmgsdi_session_type_enum_type session_type
);

/*===========================================================================
FUNCTION ds3g_mmgsdi_notify_event

DESCRIPTION
  Function to notify clients of ds3g mmgsdi events

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds3g_mmgsdi_notify_event
(
  ds3geventmgr_event_type              event_id,
  mmgsdi_session_type_enum_type        session_type,
  mmgsdi_session_changed_evt_info_type *session_changed_info
);

/*===========================================================================
FUNCTION      DS3GMMGSDI_NV_REFRESH_IND

DESCRIPTION   Notify DS3G module about NV refresh indication.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds3gmmgsdi_nv_refresh_ind
(
  sys_modem_as_id_e_type subs_id
);
/*===========================================================================
FUNCTION DS3G_MMGSDI_GET_LOW_PRIO_SIGNALING

DESCRIPTION
  This function returns the low priority signaling bit read from the SIM.
 
  This function returns FALSE if 
    1. If the file cannot be read
    2. If the Low priority signaling TAG cannot be found
 
   It returns TRUE otherwise
 
   If the function returns TRUE, low priority signaling bit parameter will
   be populated
 
DEPENDENCIES
  None

RETURN VALUE
  Boolean
 
SIDE EFFECTS
  None
===========================================================================*/
boolean ds3g_mmgsdi_get_low_prio_signaling
(
  sys_modem_as_id_e_type         subs_id,
  boolean                       *low_prio_sig
);

/*===========================================================================
  FUNCTION DS3G_MMGSDI_GET_EFACC_CLASS
  
  DESCRIPTION
   Returns true if any of the bits from b4-b8 is set in the byte 1 
   i.e 11 -15 as shown below. 
  
   Byte 1:
   b8 b7 b6 b5 b4 b3 b2 b1
   15 14 13 12 11 10 09 08 Number of the ACC (except for bit b3)
   
   if any of the bits from 11-15 are enabled then EFACC flag is set to true.
   Spec reference: TS 3GPP: 31.102
   Byte 2:
   b8 b7 b6 b5 b4 b3 b2 b1
   07 06 05 04 03 02 01 00 Number of the ACC
 
   In addition this returns the EFACC CLASS value
  
  PARAMETERS
    sys_modem_as_id_e_type subs_id.
    Pointer to retrieve EFACC class
  
  DEPENDENCIES
    None.
  
  RETURN VALUE
    TRUE  - if bits b4-b8 are set in the byte 1 of ACC
    FALSE - otherwise
  
  SIDE EFFECTS
    None.
============================================================================*/
boolean ds3g_mmgsdi_get_efacc_class
(
  sys_modem_as_id_e_type subs_id,
  uint32                *efacc_class
);


#endif /* FEATURE_MMGSDI */

#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */

#endif /* DS3GMMGSDIIF_H */
