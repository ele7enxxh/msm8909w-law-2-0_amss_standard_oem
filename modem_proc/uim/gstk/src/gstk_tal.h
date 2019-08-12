#ifndef GSTK_TAL_H
#define GSTK_TAL_H
/*===========================================================================


                  G S T K  T A L  M O D U L E     H E A D E R


GENERAL DESCRIPTION

  This header file is the gstk tal header and function prototypes

EXTERNALIZED FUNCTIONS



INITIALIZATION AND SEQUENCING REQUIREMENTS


===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2010-2014 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/gstk/src/gstk_tal.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/11/14   shr     Add Support for NV Refresh
01/16/14   hn      Replace MSG_* with UIM_MSG_* macros to save memory
05/16/13   hh      Fix KW errors
05/03/13   av      Splitting gstk_tal_init and moving out the DS,WMS reg
11/14/12   vs      Updates for AcT and LS event download
07/20/12   vs      Updated TAL logic for SGLTE
07/10/12   vs      Removed AEEStd header inclusion
07/10/12   vs      Removed references to deprecated interface CNE DSD
07/26/11   js      Change to support AcT event processing in TAL
06/18/11   sg      Fix simultaneous OTASP and BIP on a multi-modem environment
05/17/11   sg      Fix dynamic LS envelope
05/03/11   sg      Increase value of GSTK_TAL_MAX_STK_CMD_TYPE
04/20/11   js      TAL should send TR for invalid APDU that gets sent to
                   both modems
04/06/11   sg      Add additional config for access tech support in EFS
02/23/11   sg      Read proactive command config from EFS
02/22/11   js      Fix for async processing of TAL commands
02/15/11   dd      Fix bip_route build break
02/02/11   dd      Combine Access Tech envelopes in TAL
02/02/11   dd      Envelopes when both radios are active
02/02/11   sg      1) Fix DSD support
                   2) Fix envelope when offset is involved
                   3) Delay first fetch by 15 seconds
                   4) Fix routing of BIP commands on RAT change
01/24/11   js      Support for SEND SMS over IMS
11/30/10   yt      Including private UIMDRV header
11/08/10   js      Remove client ID check in WMS event callback
08/12/10   sg      TAL fix to accept TP only from Local
08/06/10   js      TAL fixes to make master modem boot in stand-alone mode
07/29/10   sg      TAL fixes for envelope commands
07/27/10   js      TAL fixes for sending TR
07/24/10   js      TAL command processing fixes
07/15/10   sg      Change gstk_tal_reg_type to gstk_tal_reg_cb_type
06/01/10   sg      Initial version
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"
#include "target.h"
#include "comdef.h"
#include "customer.h"
#ifdef FEATURE_GSTK_TAL
#include "gstk_exp.h"
#include "gstk.h"
#include "gstk_exp_v.h"
#include "uim_p.h"
#include "wms.h"
#include "cm.h"
#include "uim_msg.h"

#define GSTK_TAL_MAX_TP                   1
#define GSTK_TAL_TR_RESULT_BYTE           12
#define GSTK_TAL_MAX_REQ                  8
#define GSTK_TAL_DELAY_FETCH_DURATION     15000 /* Delay fetch by 15 seconds */
#define GSTK_TAL_DEFAULT_CMD_TYPE         0
/* Based on standard GSTK_CMD_STK_END_OF_PROACTIVE_SES = 0x81 is the highest value for
   STK command type - Type of Command Section 8.6 and 9.4 3GPP 31.111 v 3.3*/
#define GSTK_TAL_MAX_STK_CMD_TYPE         (GSTK_CMD_STK_END_OF_PROACTIVE_SES + 1)
/* Highest significant byte will be Condition for proactive cmd type */
#define GSTK_TAL_PROACTIVE_CMD_TYPE_MASK        0xFF000000
/* Second significant byte from left will be Condition for proactive cmd routing */
#define GSTK_TAL_PROACTIVE_CMD_CONDITION_MASK   0x00FF0000
/* Third significant byte from left will be Optional Value for proactive cmd routing */
#define GSTK_TAL_PROACTIVE_CMD_OPT_VAL_MASK     0x0000FF00
/* Least significant byte from right will be Route for proactive cmd routing */
#define GSTK_TAL_PROACTIVE_CMD_ROUTE_MASK       0x000000FF
/* To specify the route based on sys best use the properietary CMD_TAG */
/* 0x36 as command type is currently unused by the spec - we use this value for now */
#define GSTK_TAL_CMD_STK_ACCESS_TECH_ROUTE      0x36
/* Currently there are max of 5 access technologies */
#define GSTK_TAL_MAX_ACCESS_TECH_TYPE           5 /* 1x, HDR, GSM, WCDMA and LTE */
/* 12th byte in Location Status envelope has the service availability*/
#define GSTK_TAL_LS_POS_OF_SRV_BYTE             12
/* 12th bye in Acess Technology envelope has the Acces Technology Type */
#define GSTK_TAL_ACCESS_TECH_POS_OF_AT_BYTE     12

#define GSTK_TAL_CHECK_IF_PTR_NULL(ptr, return_flag)                           \
    if(ptr == NULL)                                                               \
    {                                                                             \
      UIM_MSG_ERR_0("GSTK :NULL pointer access");                                 \
      if(return_flag)                                                             \
      {                                                                           \
        return GSTK_MEMORY_ERROR;                                                 \
      }                                                                           \
    }

typedef enum
{
  GSTK_TAL_ROUTE_INVALID,
  GSTK_TAL_ROUTE_TO_LOCAL,
  GSTK_TAL_ROUTE_TO_REMOTE,
  GSTK_TAL_ROUTE_TO_BOTH,
  GSTK_TAL_ROUTE_SYS_BEST,
  GSTK_TAL_ROUTE_BIP_ROUTE,
  GSTK_TAL_ROUTE_DROP,
  GSTK_TAL_ROUTE_MAX = 0xF
}gstk_tal_route_type;

typedef enum
{
  GSTK_TAL_LS_ACCESS_TECH_INACTIVE         = 0x00,
  GSTK_TAL_LS_ACCESS_TECH_SEND_IN_PROGRESS = 0x01,
  GSTK_TAL_LS_ACCESS_TECH_LS_SENT_TO_UIM   = 0x02,
  GSTK_TAL_LS_ACCESS_TECH_MAX              = 0xFF
}gstk_tal_ls_access_tech_type;

typedef struct
{
  boolean                       in_use;
  uint32                        client_user_data;
  uint32                        tal_user_data;
  gstk_tal_client_type          client_type;
  gstk_tal_ls_access_tech_type  ls_access_tech_status;
  uint8                         resend_ls_index;
}gstk_tal_client_req_data_type;

typedef enum
{
  GSTK_TAL_PRO_CMD_COND_ALWAYS           = 0x00,
  GSTK_TAL_PRO_CMD_COND_QUALIFIER        = 0x01,
  GSTK_TAL_PRO_CMD_COND_IMS_REG          = 0x02,
  GSTK_TAL_PRO_CMD_COND_IMS_REG_WITH_SMS = 0x03,
  GSTK_TAL_PRO_CMD_COND_MAX              = 0xFF
}gstk_tal_pro_cmd_cond_type;

typedef enum
{
  GSTK_TAL_ACCESS_TECH_GSM               = 0x00,
  GSTK_TAL_ACCESS_TECH_UTRAN             = 0X03,
  GSTK_TAL_ACCESS_TECH_CDMA              = 0X06,
  GSTK_TAL_ACCESS_TECH_HRPD              = 0x07,
  GSTK_TAL_ACCESS_TECH_LTE               = 0x08,
  GSTK_TAL_ACCESS_TECH_NONE              = 0x7FFFFFFF
}gstk_tal_access_tech_type;

/** Serving systems. */
typedef enum
{
  GSTK_TAL_SYS_NONE=-1,   /**< Internal use.  */
  GSTK_TAL_SYS_1X,        /**< Serving system is 1X on the master modem */
  GSTK_TAL_SYS_HDR,       /**< Serving system is HDR on the master modem */
  GSTK_TAL_SYS_GSM,       /**< Serving system is GSM on the master modem */
  GSTK_TAL_SYS_WCDMA,     /**< Serving system is WCDMA on the master modem */
  GSTK_TAL_SYS_LTE,       /**< Serving system is LTE on the master modem */
  GSTK_TAL_SYS_MAX=0x7FFFFFFF  /**< Internal use. */
} gstk_tal_sys_type;

typedef struct
{
  gstk_tal_access_tech_type         access_tech;
  boolean                           most_recent;
  boolean                           resend;
  gstk_tal_client_type              client_type;
  uint16                            num_bytes;
  uint8                             *data_ptr;
}gstk_tal_loc_status_env_type;

typedef struct
{
  uint16                             data_len;
  uint8                             *data_ptr;
}gstk_tal_generic_data_type;

typedef struct
{
  uint32                       cmd_type;
  uint32                       cmd_cond;
  uint32                       cmd_opt_val;
  uint32                       cmd_route;
}gstk_tal_cmd_config_type;

typedef struct
{
  gstk_tal_route_type              cmd_route;
  uint8                            cmd_config_size;
  gstk_tal_cmd_config_type         *cmd_add_config;
}gstk_tal_pro_cmd_config_type;

typedef struct
{
  gstk_tal_sys_type               ps_sys;
  gstk_tal_sys_type               cs_sys;
}gstk_tal_serving_sys_info_type;

typedef struct
{
  gstk_tal_access_tech_type       access_tech;
  gstk_tal_client_type            client;
}gstk_tal_ls_triggred_AcT_type;

typedef struct
{
  gstk_tal_access_tech_type       access_tech;
  gstk_tal_client_type            client;
}gstk_tal_last_sent_AcT_type;

typedef struct
{
  wms_client_id_type              gstk_tal_wms_cid;
  boolean                         ims_subs_avail;
  gstk_tal_route_type             bip_route;
  gstk_tal_loc_status_env_type    loc_status_cache[GSTK_TAL_MAX_CLIENTS];
  gstk_access_technology_type     rat[GSTK_TAL_MAX_CLIENTS];
  gstk_tal_pro_cmd_config_type    pro_cmd_route[GSTK_TAL_MAX_STK_CMD_TYPE];
  gstk_tal_loc_status_env_type    loc_status_table[GSTK_TAL_MAX_ACCESS_TECH_TYPE];
  gstk_tal_access_tech_type       last_loc_status_sent;
  gstk_tal_last_sent_AcT_type     last_access_tech_sent;
  gstk_tal_uim_cmd_type           error_tr_cache;
  gstk_tal_access_tech_type       current_active_rat[GSTK_TAL_MAX_CLIENTS];
  gstk_tal_ls_triggred_AcT_type   ls_triggered_access_tech;
  gstk_location_status_enum_type  gstk_tal_location_status[GSTK_TAL_MAX_CLIENTS];
  boolean                         otasp_in_progress;
  gstk_tal_serving_sys_info_type  ss_info[GSTK_TAL_MAX_CLIENTS];
}gstk_tal_global_data_type;

extern gstk_tal_reg_cb_type          gstk_tal_client_cb_list[GSTK_TAL_MAX_CLIENTS];
extern gstk_tal_client_req_data_type gstk_tal_client_req_table[GSTK_TAL_MAX_REQ];
extern gstk_tal_global_data_type     gstk_tal_global;
extern boolean                       tal_is_memset_done;
extern boolean                       tal_is_fetch_ok;
extern rex_timer_type                tal_delay_fetch_timer;
extern gstk_tal_global_data_type     gstk_tal_global;


/*===========================================================================
FUNCTION gstk_tal_init

DESCRIPTION
  Function for TAL initialization. Registers with DS for event
  notifications.

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_tal_init(void);

/*===========================================================================
FUNCTION gstk_tal_external_init

DESCRIPTION
  Function for TAL registration with external services - DS, WMS etc.

PARAMETERS
  None

DEPENDENCIES
  Those external services (DS, WMS tasks) must be ready before calling
  this function

RETURN VALUE
  gstk_status_enum_type

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_tal_external_init(
  void
);

/*===========================================================================
FUNCTION gstk_tal_send_tp_cmd_to_uim

DESCRIPTION
  This function is used by TAL to send TP to UIM

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  This function should only be called after gstk_client_init has been called

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern gstk_status_enum_type gstk_tal_send_tp_cmd_to_uim(void);

/*===========================================================================
FUNCTION gstk_tal_command_main

DESCRIPTION
  Function for TAL command processing

PARAMETERS
  tal_cmd_ptr -> contains TAL command data to be sent to UIM driver
  client_type -> contains the client Local or Remote that sent the cmd
  gstk_tal_cb -> GSTK TAL Callback

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type
  GSTK_BAD_PARAM:              Invalid/Erroneous Input Parameters
  GSTK_SUCCESS:                Successful Processing of Command
  GSTK_ERROR:                  Generic Error in processing the command

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_tal_command_main(
  gstk_tal_client_type             client_type,
  gstk_tal_uim_cmd_type           *tal_cmd_ptr,
  gstk_tal_reg_cb_type             gstk_tal_cb
);

/*===========================================================================
FUNCTION gstk_tal_update_cs_serving_sys_info

DESCRIPTION
  Function to update circuit switch network serving system info

PARAMETERS
  sys_sys_mode_e_type   (access technology)
  sys_srv_domain_e_type (CS or PS domain info)

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
void gstk_tal_update_cs_serving_sys_info(
  sys_sys_mode_e_type    sys_mode,
  sys_srv_domain_e_type  srv_domain
);


/*===========================================================================
FUNCTION gstk_tal_notify_remote_client_of_recovery

DESCRIPTION
  Function handles recovery indication from drivers to ensure remote
  modem gets the notification to end any active proacitve session

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
void gstk_tal_notify_remote_client_of_recovery(
  gstk_slot_id_enum_type  slot_id
);

#endif /* FEATURE_GSTK_TAL */
#endif /* GSTK_TAL_H */

