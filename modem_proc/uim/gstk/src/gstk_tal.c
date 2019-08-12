/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                        G S T K     U T I L I T Y


GENERAL DESCRIPTION
  This is the GSTK internal utility file.  It contains function that will
  be called within GSTK module such basic util functionalities.


FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2010-2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/gstk/src/gstk_tal.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/31/16   shr     F3 reduction: Dumping Arrays
05/25/16   shr     F3 reduction: Malloc Failure, Memcpy, Null Ptr checks
07/02/15   vr      Fix potential buffer overflow while building Envelope/TR
08/27/14   vr      Move gstk_shared_data and gstk_instance_data to global
08/08/14   gm      Free memory allocated for sending command to UIM
07/11/14   shr     Add Support for NV Refresh
04/03/14   dy      Replace gstk_malloc() with macro
03/31/14   dy      Fixed gstk_byte_offset_copy and gstk_memcpy
02/21/14   dy      Fixed GSTK EFS stub fct bug and added default_apn nv
01/16/14   hn      Replace MSG_* with UIM_MSG_* macros to save memory
01/09/14   kb      Updated GSTK_NULL_CHECK_PTR macro
11/12/13   sw      Removed FEATURE_GSTK_TEST_AUTOMATION code
08/28/13   tkl     Rename FEATURE_UIM_SGLTE
10/04/13   gs      F3 Message reduction
09/06/13   gm      UIMDRV Memory De-Couple changes
08/28/13   yt      Deprecate VIRTUIM/VIRTSAT module
06/20/13   gm      Passing proactive_cmd_pending through TAL
07/25/13   gm      Sending  end_proactive_session to both local and remote
07/24/13   gm      Updating CS serving status when system mode is TDS
06/07/13   yt      Copy intermediate status words to TAL UIM report
05/27/13   vr      Use safer versions of memcpy() and memmove()
05/16/13   hh      Fix KW errors
05/03/13   av      Splitting gstk_tal_init and moving out the DS,WMS reg
04/09/13   vr      Use safer versions of memcpy() and memmove()
02/15/13   vs      Clear tal fetch delay timer when remote GSTK registers
02/14/13   vs      Added default TAL config creation support
11/14/12   vs      Updates for AcT and LS event download
10/16/12   vs      Fixed process AcT priorities to include TDSCDMA
10/04/12   hk      Convert all ERRs to MSG_ERROR to save memory
10/01/12   vs      Fixed PLI-LS and PLI-AcT command routing for SGLTE
07/20/12   vs      Added support for SGLTE command routing
07/10/12   vs      Removed references to deprecated interface CNE DSD
05/22/12   nb      Convert setting and resetting of gstk_proactive_cmd_in_progress
                   flag to MACRO
02/18/12   av      Cleaned up remaining ALWAYS OFF features
12/20/11   nb      GSDI Cleanup
11/11/11   nb      Remove AEESTD APIs
09/13/11   js      Prevent double pointer free
07/26/11   js      Change to support AcT event processing in TAL
06/22/11   sg      Fix simultaneous OTASP and BIP on a multi-modem environment
06/20/11   js      Fix issue with uim recovery on fusion
05/17/11   sg      Read access tech config from EFS and dynamic LS
05/06/11   sg      Fix crash for envelope and sms_pp_dl failure
04/22/11   sg      Store the BIP route from open channel command
04/20/11   js      TAL should send TR for invalid APDU that gets sent to
                   both modems
04/11/11   js      Fix for envelope memcpy in TAL
04/06/11   sg      Add additional config for access tech support in EFS
02/23/11   sg      Read proactive cmd config from EFS
02/22/11   js      Fix for async processing of TAL commands
02/02/11   dd      Fix env_ref_id
02/02/11   dd      Process LTE AcT correctly
02/02/11   dd      Combine Access Tech envelopes in TAL
02/02/11   dd      Send Loc Status envelope if other cache is empty
02/02/11   dd      Fix the case of lower service envelope
02/02/11   dd      Envelopes when both radios are active
02/02/11   sg      1) Fix DSD support
                   2) Fix envelope when offset is involved
                   3) Fix overwrite of callback with 0
                   4) Delay fetch of first proactive command by 15 seconds
                   5) Fix routing of BIP commands on RAT change
01/24/11   js      1) Support for SEND SMS over IMS
                   2) Fix to attemp re-registration for CNE upon initial failure
11/30/10   js      Fix crash for null pointer free
11/08/10   js      Remove client ID check in WMS event callback
11/05/10   js      Fixed merge errors
11/04/10   js      Fix for handling UI related set up events in TAL
11/03/10   js      Fix routing of PLI Location Info/Access Tech
10/28/10   js      Set gstk tal fetch delay timer before TP to avoid race
                   condition
10/27/10   js      Fix for open channel terminal rsp not getting sent
10/26/10   dd      Route PLI Date/Time/Zone to remote
08/22/10   vs      Merged TP DL fix for UICC reset refresh
08/14/10   js      Corrected client type used in gstk_tal_send_envelope_to_uim
08/14/10   js      Boundary check for gstk_tal_client_type
08/11/10   js      Fixed issue with TAL remote registration
08/06/10   js      Fixed TAL routing bugs
07/28/10   sg      TAL support for envelope commands
07/27/10   js      TAL fixes for sending TR
07/24/10   js      TAL command processing fixes
07/15/10   sg      API argument type change to pointer
06/01/10   sg      Initial version

===========================================================================*/


/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"
#include "target.h"
#include "customer.h"
#include "comdef.h"
#include <stringl/stringl.h>

#ifdef FEATURE_GSTK_TAL
#include "gstk_tal.h"
#include "gstk.h"
#include "gstk_nv.h"
#include "gstkutil.h"
#include "err.h"
#include "gstk_envelope_cmd.h"
#include "fs_lib.h"
#include "fs_public.h"
#ifdef FEATURE_UIM_TEST_FRAMEWORK
#error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */
#include "ds_sys_event.h"
#include "uim_msg.h"

static uint8 gstk_tal_terminal_profile_cache[UIM_TERMINAL_PROFILE_DATA_LENGTH];
static uint8 gstk_tal_num_tp_received = 0;
static uint8 gstk_tal_expected_num_tr = 0;
static gstk_tal_route_type gstk_tal_pro_cmd_route = GSTK_TAL_ROUTE_MAX;
boolean  tal_is_memset_done = FALSE;
boolean tal_is_fetch_ok = FALSE;
static boolean  tal_first_envelope_sent = FALSE;
rex_timer_type tal_delay_fetch_timer;
/* Data structure to store client callback pointers */
gstk_tal_reg_cb_type gstk_tal_client_cb_list[GSTK_TAL_MAX_CLIENTS];

/* Data structure to store client data while UIM async requests
   is in progress */
gstk_tal_client_req_data_type gstk_tal_client_req_table[GSTK_TAL_MAX_REQ];

gstk_tal_global_data_type gstk_tal_global = {0};

static gstk_status_enum_type gstk_tal_ds_sys_init(void);

void gstk_tal_uim_generic_report (uim_rpt_type *uim_rpt_ptr);

void gstk_tal_send_cmd_to_uim_server (uim_cmd_type *cmd_ptr);

/*===========================================================================
FUNCTION gstk_tal_get_free_client_req_tbl_index

DESCRIPTION
  The function is used to find free index in client request table

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  Positive index in the client request table on success
  -1 when no free index is available

SIDE EFFECTS
  None
===========================================================================*/
static int8 gstk_tal_get_free_client_req_tbl_index(void)
{
  int8 i     = 0;
  int8 index = -1;
  rex_enter_crit_sect(&gstk_shared_data.gstk_crit_sect);
  for(i = 0; i < GSTK_TAL_MAX_REQ; i++)
  {
    if(gstk_tal_client_req_table[i].in_use == FALSE)
    {
      UIM_MSG_HIGH_1("TAL: gstk_tal_get_free_client_req_tbl index  %d assigned",
                     i);
      gstk_tal_client_req_table[i].in_use = TRUE;
      index = i;
      break;
    }
  }
  rex_leave_crit_sect(&gstk_shared_data.gstk_crit_sect);
  return index;
} /* gstk_tal_get_free_client_req_tbl_index */

#ifdef FEATURE_UIM_DUAL_MODEM_SGLTE
/*===========================================================================
FUNCTION gstk_tal_get_preferred_sys_sglte

DESCRIPTION
  The function is used to determine the preferred system and best route
  for the SGLTE product

PARAMETERS
  Type of command
  Suggested route for "system best" route option


DEPENDENCIES
  None

RETURN VALUE
  gstk_tal_sys_type: The serving system for the particular type of command

SIDE EFFECTS
  None
===========================================================================*/
static gstk_tal_sys_type gstk_tal_get_preferred_sys_sglte(
  uint8                 cmd_type,
  gstk_tal_route_type  *sys_best_route
)
{
  gstk_tal_sys_type  pref_sys    = GSTK_TAL_SYS_NONE;

  if(sys_best_route == NULL)
  {
    UIM_MSG_ERR_0("TAL: sys_best_route is NULL.Cannot process pref sys"  );
    return pref_sys;
  }

  /* For the SGLTE product the preferred system depends on the type of command
     and the serving system as determined by the information provided by
     DS and CM events */
  switch(cmd_type)
  {
    case GSTK_CMD_STK_OPEN_CHANNEL:
      {
        /* Algorithm for Data call related command routing:
           1. Register for CM event for serving system on local modem
           2. Register for DS event for preferred data system on local modem
           3. If pref_sys is TD-LTE/TD-SCDMA/WCDMA: originate call on local modem
           4.  If pref_sys is GSM:
               a. If local modem CM says CS/PS service available:
                    Originate data call on local modem.
               b. If local modem CM says CS/PS service not available:
                    Originate data call on remote modem. */
        if(gstk_tal_global.ss_info[GSTK_TAL_LOCAL_CLIENT].ps_sys == GSTK_TAL_SYS_LTE ||
           gstk_tal_global.ss_info[GSTK_TAL_LOCAL_CLIENT].ps_sys == GSTK_TAL_SYS_WCDMA)
        {
          pref_sys        = gstk_tal_global.ss_info[GSTK_TAL_LOCAL_CLIENT].ps_sys;
          *sys_best_route = GSTK_TAL_ROUTE_TO_LOCAL;
        }
        else if(gstk_tal_global.ss_info[GSTK_TAL_LOCAL_CLIENT].ps_sys == GSTK_TAL_SYS_GSM)
        {
          pref_sys = GSTK_TAL_SYS_GSM;

          if(gstk_tal_global.ss_info[GSTK_TAL_LOCAL_CLIENT].cs_sys == GSTK_TAL_SYS_GSM)
          {
            *sys_best_route = GSTK_TAL_ROUTE_TO_LOCAL;
          }
          else
          {
            *sys_best_route = GSTK_TAL_ROUTE_TO_REMOTE;
          }
        }
        else
        {
          pref_sys        = GSTK_TAL_SYS_NONE;
          *sys_best_route = GSTK_TAL_ROUTE_TO_LOCAL;
        }
      }
      break;

    case GSTK_CMD_STK_SEND_SS:
    case GSTK_CMD_STK_USSD:
    case GSTK_CMD_STK_SET_UP_CALL:
    case GSTK_CMD_STK_SEND_DTMF:
    case GSTK_CMD_STK_SEND_SHORT_MESSAGE:
      {
          /* Algorithm for Voice call/SMS/Supplementary services cmd routing:
             1. Register for CM event for serving system on local modem
             2. If CM says CS available (GSM or WCDMA), then originate
                voice/SMS/SS call on local modem
             3. If CM says CS not available, then originate voice/SMS/SS call
                on remote modem */
        if(gstk_tal_global.ss_info[GSTK_TAL_LOCAL_CLIENT].cs_sys == GSTK_TAL_SYS_GSM ||
           gstk_tal_global.ss_info[GSTK_TAL_LOCAL_CLIENT].cs_sys == GSTK_TAL_SYS_WCDMA)
        {
          pref_sys        = gstk_tal_global.ss_info[GSTK_TAL_LOCAL_CLIENT].cs_sys;
          *sys_best_route = GSTK_TAL_ROUTE_TO_LOCAL;
        }
        else if(gstk_tal_global.ss_info[GSTK_TAL_LOCAL_CLIENT].cs_sys == GSTK_TAL_SYS_NONE)
        {
          pref_sys        = GSTK_TAL_SYS_GSM;
          *sys_best_route = GSTK_TAL_ROUTE_TO_REMOTE;
        }
      }
      break;

    case GSTK_CMD_STK_PROVIDE_LOCAL_INFO:
      {
        /* Routing for PLI-LS and PLI-AcT is determined as follows:
           1. Register for CM event for serving system on local modem
           2. Register for DS event for preferred data system on local modem
           3. If PS serving system is TD-LTE/TD-SCDMA/WCDMA route to local modem
           4. Else if PS serving system is GSM:
               a. If local modem CM says CS/PS service available:
                  Send command to local modem.
               b. If local modem CM says CS/PS service not available:
                  Send command to remote modem.
           5. Else if CS serving system is WCDMA or GSM on local modem route local
           6. Else route to remote modem */
        if(gstk_tal_global.ss_info[GSTK_TAL_LOCAL_CLIENT].ps_sys == GSTK_TAL_SYS_LTE ||
           gstk_tal_global.ss_info[GSTK_TAL_LOCAL_CLIENT].ps_sys == GSTK_TAL_SYS_WCDMA)
        {
          pref_sys        = gstk_tal_global.ss_info[GSTK_TAL_LOCAL_CLIENT].ps_sys;
          *sys_best_route = GSTK_TAL_ROUTE_TO_LOCAL;
        }
        else if(gstk_tal_global.ss_info[GSTK_TAL_LOCAL_CLIENT].ps_sys == GSTK_TAL_SYS_GSM)
        {
          pref_sys = GSTK_TAL_SYS_GSM;
          if(gstk_tal_global.ss_info[GSTK_TAL_LOCAL_CLIENT].cs_sys == GSTK_TAL_SYS_GSM)
          {
            *sys_best_route = GSTK_TAL_ROUTE_TO_LOCAL;
          }
          else
          {
            *sys_best_route = GSTK_TAL_ROUTE_TO_REMOTE;
          }
        }
        else if(gstk_tal_global.ss_info[GSTK_TAL_LOCAL_CLIENT].cs_sys == GSTK_TAL_SYS_GSM ||
                gstk_tal_global.ss_info[GSTK_TAL_LOCAL_CLIENT].cs_sys == GSTK_TAL_SYS_WCDMA)
        {
          pref_sys        = gstk_tal_global.ss_info[GSTK_TAL_LOCAL_CLIENT].cs_sys;
          *sys_best_route = GSTK_TAL_ROUTE_TO_LOCAL;
        }
        else
        {
          pref_sys        = GSTK_TAL_SYS_NONE;
          *sys_best_route  = GSTK_TAL_ROUTE_TO_REMOTE;
        }
      }
      break;

    default:
      /* For remaining commands return no pref sys and route as local */
      pref_sys        = GSTK_TAL_SYS_NONE;
      *sys_best_route = GSTK_TAL_ROUTE_TO_LOCAL;
      break;
  }

  UIM_MSG_HIGH_3("TAL: For cmd 0x%x pref_sys is 0x%x, best_route is 0x%x",
                 cmd_type, pref_sys, *sys_best_route);

  return pref_sys;
} /*gstk_tal_get_preferred_sys_sglte*/
#endif /* FEATURE_UIM_DUAL_MODEM_SGLTE */

/*===========================================================================
FUNCTION gstk_tal_get_preferred_sys

DESCRIPTION
  The function is used to determine the preferred system for a particular
  command

PARAMETERS
  Type of command
  Suggested route for "system best" route option

DEPENDENCIES
  None

RETURN VALUE
  gstk_tal_sys_type: The serving system for the particular type of command

SIDE EFFECTS
  None
===========================================================================*/
static gstk_tal_sys_type gstk_tal_get_preferred_sys(
  uint8                 cmd_type,
  gstk_tal_route_type  *sys_best_route
)
{
#ifdef FEATURE_UIM_DUAL_MODEM_SGLTE
  return gstk_tal_get_preferred_sys_sglte(cmd_type, sys_best_route);
#else
  (void)cmd_type;
  (void)sys_best_route;
  UIM_MSG_ERR_0("TAL: Preferred sys logic not defined");
  return GSTK_TAL_SYS_NONE;
#endif /* FEATURE_UIM_DUAL_MODEM_SGLTE */
} /*gstk_tal_get_preferred_sys*/

/*===========================================================================
FUNCTION GSTK_TAL_REMOVE_UI_SETUP_EVENTS_FOR_REMOTE

DESCRIPTION
  The function is used remove UI related set up events from set up
  event list proactive command before sending them to remote modem

PARAMETERS
  tal_cmd_ptr: Reference to TAL command type for set up event list proactive cmd
  event_list_tag_offset: Offset for Event List Tag in the APDU

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void gstk_tal_remove_ui_setup_events_for_remote(
  gstk_tal_cmd_details_type     *tal_cmd_ptr,
  uint8                          event_list_tag_offset
)
{
  uint8    offset                       = 0;
  uint8    *cmd_ptr                     = NULL;
  uint32   cmd_len                      = 0;
  uint8    num_of_events                = 0;
  uint8    num_of_event_offset          = 0;
  uint8    *new_event_list              = NULL;
  uint8    delete_cnt                   = 0;
  uint8    i                            = 0;
  uint8    length_of_TLV_length_field   = 0;


  if((tal_cmd_ptr == NULL) ||
     (tal_cmd_ptr->cmd_details.tal_pro_cmd.data_ptr == NULL))
  {
    UIM_MSG_ERR_2("TAL: Bad Input param Null - tal_cmd_ptr %p data_ptr %p",
                  tal_cmd_ptr,
                  tal_cmd_ptr? tal_cmd_ptr->cmd_details.tal_pro_cmd.data_ptr:NULL);
    return;
  }

  cmd_ptr = tal_cmd_ptr->cmd_details.tal_pro_cmd.data_ptr;
  cmd_len = tal_cmd_ptr->cmd_details.tal_pro_cmd.data_length;

  if(cmd_len < event_list_tag_offset)
  {
    UIM_MSG_ERR_0("TAL: Incorrect set up event list APDU");
    return;
  }
  offset = event_list_tag_offset;

  if(!gstk_valid_tag(cmd_ptr[event_list_tag_offset],
                       (tags_enum_type)GSTK_EVENT_LIST_TAG))
  {
    UIM_MSG_ERR_1("TAL: Invalid set up event list tag = 0x%x",
                  cmd_ptr[event_list_tag_offset]);
    return;
  }
  /* Next field length of events list */
  offset++;

  if(cmd_len <= offset)
  {
    UIM_MSG_ERR_2("TAL: Missing set up event list length field cmd_len = 0x%x, offset = 0x%x",
                  cmd_len, offset);
    return;
  }

  length_of_TLV_length_field = int32touint8(gstk_find_length_of_length_value(
                                            &cmd_ptr[1]));

  if(length_of_TLV_length_field != 1 &&
     length_of_TLV_length_field != 2)
  {
    /* Invalid BER TLV len , ignore the command */
    UIM_MSG_HIGH_1("TAL: ERROR:  Length of TLV Invalid: 0x%x ",
                   length_of_TLV_length_field);
    return;
  }

  num_of_event_offset = offset;
  num_of_events       = cmd_ptr[offset];

  new_event_list = (uint8*)GSTK_CALLOC(num_of_events);
  if(!new_event_list)
  {
    return;
  }

  /* Next field  set up events list */
  while((num_of_events) && (offset < cmd_len -1))
  {
    offset++;
    switch((gstk_evt_list_code_enum_type)cmd_ptr[offset])
    {
      case GSTK_IDLE_SCRN_AVAILABLE:
      case GSTK_USER_ACTIVITY:
      case GSTK_LANGUAGE_SELECTION:
      case GSTK_BROWSER_TERMINATION:
        UIM_MSG_HIGH_1("TAL: Setup Event 0x%x Stripped", cmd_ptr[offset]);
        delete_cnt++;
        break;
      default:
        UIM_MSG_HIGH_1("TAL: Setup event 0x%x added, 0x%x", cmd_ptr[offset]);
        new_event_list[i] = cmd_ptr[offset];
        i++;
        break;
    }
    num_of_events--;
  }
  if(delete_cnt)
  {
    if(delete_cnt > cmd_len)
    {
      UIM_MSG_ERR_1("TAL: Invalid number of events to be deleted 0x%x",
                    delete_cnt);
      gstk_free(new_event_list);
      new_event_list = NULL;
      return;
    }

    tal_cmd_ptr->cmd_details.tal_pro_cmd.data_length = cmd_len - delete_cnt;

    /* Copy data from new events list */
    (void)memscpy((void*)&cmd_ptr[num_of_event_offset+1],
           (cmd_ptr[num_of_event_offset] - delete_cnt),
           (void*)new_event_list,
           (cmd_ptr[num_of_event_offset] - delete_cnt));

    /* set correct number of events */
    cmd_ptr[num_of_event_offset] =
      cmd_ptr[num_of_event_offset] - delete_cnt;

    /* set correct value for length of proactive command */
    if(length_of_TLV_length_field == 1)
    {
      cmd_ptr[GSTK_PROACTIVE_CMD_LEN_OFFSET] -= delete_cnt;
    }
    else if(length_of_TLV_length_field == 2)
    {
      cmd_ptr[GSTK_PROACTIVE_CMD_LEN_OFFSET+1] -= delete_cnt;
    }
    /* set unused bytes to 0xFF */
    memset(&cmd_ptr[cmd_len - delete_cnt], 0xFF, delete_cnt);

  }
  gstk_free(new_event_list);
  new_event_list = NULL;
} /* gstk_tal_remove_ui_setup_events_for_remote */

/*===========================================================================
FUNCTION gstk_tal_get_default_route

DESCRIPTION
  The function is used to find out the default route for proactive commands
  when multiple default conditions are specified

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  gstk_tal_route_type

SIDE EFFECTS
  None

SEE ALSO
  gstk_tal_find_route
===========================================================================*/
static gstk_tal_route_type gstk_tal_get_default_route(void)
{
  UIM_MSG_HIGH_0("TAL: gstk_tal_get_default_route");
  /* When multiple rules become available for default route this function
     can be updated - for now route to local */
  return GSTK_TAL_ROUTE_TO_LOCAL;
}

/*===========================================================================
FUNCTION gstk_tal_find_route

DESCRIPTION
  The function is used to find out if the command has to be routed to
  local or remote or both.

PARAMETERS
  type_of_command: [Input] Type of Proactive Command
  qualifier:       [Input] Command qualifier
DEPENDENCIES
  None

RETURN VALUE
  gstk_tal_route_type

SIDE EFFECTS
  None

SEE ALSO
  gstk_tal_get_preferred_sys
===========================================================================*/
static gstk_tal_route_type gstk_tal_find_route(uint8 type_of_command, uint8 qualifier)
{
  gstk_tal_route_type      route              = GSTK_TAL_ROUTE_INVALID;
  uint8                    add_config_size    = 0;
  gstk_tal_cmd_config_type *add_config_ptr    = NULL;
  uint8                    i                  = 0;
  boolean                  loop_flag          = TRUE;
  gstk_tal_sys_type        preferred_sys      = 0;

  UIM_MSG_HIGH_2("TAL: gstk_tal_find_route for 0x%x and qualifier 0x%x",
                 type_of_command, qualifier);

  if(type_of_command >= GSTK_TAL_MAX_STK_CMD_TYPE)
  {
    UIM_MSG_ERR_1("TAL: Invalid type of command 0x%x", type_of_command);
    route = gstk_tal_get_default_route();
    return route;
  }

  add_config_size = gstk_tal_global.pro_cmd_route[type_of_command].cmd_config_size;
  add_config_ptr = gstk_tal_global.pro_cmd_route[type_of_command].cmd_add_config;

  if(add_config_size == 0)
  {
    route = gstk_tal_global.pro_cmd_route[type_of_command].cmd_route;
  }

  if(add_config_size > 0)
  {
    UIM_MSG_HIGH_1("TAL: Multiple conditions exist 0x%x", add_config_size);
    for(i = 0; (i < add_config_size) && (loop_flag == TRUE); i++)
    {
      if(add_config_ptr == NULL)
      {
        UIM_MSG_ERR_1("TAL: Ptr invalid for index %d", i);
        route = gstk_tal_get_default_route();
        break;
      }
      UIM_MSG_HIGH_3("TAL: Route based on condition 0x%x opt_val 0x%x route 0x%x",
                     add_config_ptr[i].cmd_cond,
                     add_config_ptr[i].cmd_opt_val,
                     add_config_ptr[i].cmd_route);

      switch(add_config_ptr[i].cmd_cond)
      {
      case GSTK_TAL_PRO_CMD_COND_ALWAYS:
        route = (gstk_tal_route_type)add_config_ptr[i].cmd_route;
        loop_flag = FALSE;
        break;
      case GSTK_TAL_PRO_CMD_COND_QUALIFIER:
        if(add_config_ptr[i].cmd_opt_val ==  qualifier)
        {
          UIM_MSG_HIGH_1("TAL: Found match for cond_qualifier as 0x%x",
                         add_config_ptr[i].cmd_opt_val);
          route = (gstk_tal_route_type)add_config_ptr[i].cmd_route;
          loop_flag = FALSE;
        }
        break;
      case GSTK_TAL_PRO_CMD_COND_IMS_REG:
        if(gstk_tal_global.ims_subs_avail)
        {
          route = (gstk_tal_route_type)add_config_ptr[i].cmd_route;
          loop_flag = FALSE;
        }
        break;
      case GSTK_TAL_PRO_CMD_COND_IMS_REG_WITH_SMS:
        if(gstk_tal_global.ims_subs_avail) // Add other condition for SMS avail
        {
          route = (gstk_tal_route_type)add_config_ptr[i].cmd_route;
          loop_flag = FALSE;
        }
        break;
      default:
        UIM_MSG_ERR_0("TAL: Unhandled condition");
        loop_flag = FALSE;
        route = gstk_tal_get_default_route();
        break;
      }
    }
  }

  if(route == GSTK_TAL_ROUTE_SYS_BEST)
  {
    gstk_tal_route_type sys_best_route = GSTK_TAL_ROUTE_INVALID;

    preferred_sys = gstk_tal_get_preferred_sys(type_of_command, &sys_best_route);

    if(type_of_command != GSTK_TAL_CMD_STK_ACCESS_TECH_ROUTE)
    {
#ifdef FEATURE_UIM_DUAL_MODEM_SGLTE
      /* For SGLTE the route is fixed for LTE, TDSCDMA, WCDMA as local and for
         GSM as which ever modem has GSM service hence the route is set to
         whatever route is provided by the gstk_tal_get_preferred_sys
         function rather than looking up the route based on access technnology
         in the tal config file */
      route = sys_best_route;
#else
      route = gstk_tal_find_route(GSTK_TAL_CMD_STK_ACCESS_TECH_ROUTE, preferred_sys);
#endif /* FEATURE_UIM_DUAL_MODEM_SGLTE */
    }
    else
    {
      UIM_MSG_ERR_0("TAL: Invalid config for access tech route");
      route = gstk_tal_get_default_route();
    }
    UIM_MSG_HIGH_1("TAL: Route for sys best access tech is %d", route);
  }

  if(type_of_command == GSTK_CMD_STK_OPEN_CHANNEL)
  {
    if(gstk_tal_global.otasp_in_progress)
    {
      UIM_MSG_HIGH_0("OTASP in progress route to remote");
      route = GSTK_TAL_ROUTE_TO_REMOTE;
    }
    gstk_tal_global.bip_route = route;
  }

  if(route == GSTK_TAL_ROUTE_BIP_ROUTE)
  {
    route = gstk_tal_global.bip_route;
  }

  if(route == GSTK_TAL_ROUTE_INVALID)
  {
    UIM_MSG_HIGH_0("TAL: Need to calculate default route based on multiple rules");
    route = gstk_tal_get_default_route();
  }

  UIM_MSG_HIGH_1("TAL: gstk_tal_find_route returns 0x%x", route);
  return route;
} /*gstk_tal_find_route*/

/*===========================================================================
FUNCTION gstk_tal_send_pending_ls

DESCRIPTION
  Send a pending Location Status Envelope waiting to be sent after
  access technology change envelope is sent.

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
void gstk_tal_send_pending_ls(void)
{
  uint8                                   i = 0;
  uim_cmd_type                            *uim_cmd_ptr = NULL;
  boolean                                 resend_flag = FALSE;
  gstk_status_enum_type                   gstk_status = GSTK_ERROR;
  int8                                    index = -1;

  UIM_MSG_HIGH_0("TAL: gstk_tal_send_pending_ls");

  for(i=0; i<GSTK_TAL_MAX_ACCESS_TECH_TYPE; i++)
  {
    if(gstk_tal_global.loc_status_table[i].resend)
    {
      UIM_MSG_HIGH_1("TAL: Resend flag on for access tech 0x%x",
                     gstk_tal_global.loc_status_table[i].access_tech);
      if(gstk_tal_global.loc_status_table[i].data_ptr != NULL)
      {
        UIM_MSG_HIGH_1("TAL: Resend valid values at index %d", i);
        resend_flag = TRUE;
      }
      break;
    }
  }

  if(resend_flag != TRUE || i == GSTK_TAL_MAX_ACCESS_TECH_TYPE)
  {
    UIM_MSG_ERR_0("TAL: No Matching values to resend");
    return;
  }

  uim_cmd_ptr = (uim_cmd_type*)GSTK_CALLOC(sizeof(uim_cmd_type));

  if(uim_cmd_ptr == NULL)
  {
    return;
  }

  gstk_status = gstk_util_populate_uim_buf_hdr_info(UIM_ENVELOPE_F,
                                                    &uim_cmd_ptr);
  if ((gstk_status != GSTK_SUCCESS) || (uim_cmd_ptr == NULL))
  {
    UIM_MSG_ERR_0("TAL: gstk_util_populate_uim_buf_hdr_info failed");
    if(uim_cmd_ptr)
    {
      gstk_free(uim_cmd_ptr);
      uim_cmd_ptr = NULL;
    }
    return;
  }

  if((index = gstk_tal_get_free_client_req_tbl_index()) < 0)
  {
    UIM_MSG_ERR_0("TAL: No free index avail in req table");
    if(uim_cmd_ptr)
    {
      gstk_free(uim_cmd_ptr);
      uim_cmd_ptr = NULL;
    }
    return;
  }

  /* Populate client request table */
  gstk_tal_client_req_table[index].client_user_data = 0;
  gstk_tal_client_req_table[index].tal_user_data    = (uint32)(uim_cmd_ptr);
  gstk_tal_client_req_table[index].client_type      = GSTK_TAL_LOCAL_CLIENT;
  gstk_tal_client_req_table[index].ls_access_tech_status = GSTK_TAL_LS_ACCESS_TECH_LS_SENT_TO_UIM;

  /* Populate UIM command pointer */
  uim_cmd_ptr->hdr.user_data      = int32touint32(index);
  uim_cmd_ptr->hdr.rpt_function   = gstk_tal_uim_generic_report;
  uim_cmd_ptr->envelope.offset    = gstk_tal_global.loc_status_table[i].data_ptr[0];
  uim_cmd_ptr->envelope.num_bytes =
    uint16touint8(gstk_tal_global.loc_status_table[i].num_bytes
                  - uim_cmd_ptr->envelope.offset -  1);

  (void)memscpy((void *)uim_cmd_ptr->envelope.data,
  	sizeof(uim_cmd_ptr->envelope.data),
        (void *)(gstk_tal_global.loc_status_table[i].data_ptr + 1),
        gstk_tal_global.loc_status_table[i].num_bytes - 1 + uim_cmd_ptr->envelope.offset);

  gstk_tal_send_cmd_to_uim_server(uim_cmd_ptr);
  gstk_tal_global.loc_status_table[i].resend = FALSE;
} /* gstk_tal_send_pending_ls */

/*===========================================================================
FUNCTION gstk_tal_uim_generic_report

DESCRIPTION

  UIM will call this function when it receives response from the Card. The
  function serves as a wrapper and calls the specific gstk API on local
  and remote based on the type of response.

PARAMETERS
  uim_rpt_ptr: [Input] Pointer to the UIM response report

DEPENDENCIES
  None

RETURN VALUE
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
void gstk_tal_uim_generic_report (uim_rpt_type *uim_rpt_ptr)
{
  uim_cmd_type               *uim_cmd_ptr = NULL;
  int32                       index       = 0;
  uint32                      user_data   = 0;
  gstk_tal_cmd_details_type   tal_cmd;
  uint8                       i           = 0;

  if(uim_rpt_ptr == NULL)
  {
    UIM_MSG_ERR_0("TAL: NULL input pointer");
    return;
  }

  memset((void *)&tal_cmd, 0x00, sizeof(gstk_tal_cmd_details_type));

  UIM_MSG_HIGH_3("TAL: GSTK recv uim_generic_report, sw1 0x%x, sw2 0x%x status 0x%x",
                 uim_rpt_ptr->sw1, uim_rpt_ptr->sw2, uim_rpt_ptr->rpt_status);

  index = uim_rpt_ptr->user_data;
  /* retrieve Info from client req table */
  if((index >= 0 && index < GSTK_TAL_MAX_REQ) &&
     gstk_tal_client_req_table[index].in_use)
  {
    uim_cmd_ptr = (uim_cmd_type*)gstk_tal_client_req_table[index].tal_user_data;
    if(uim_cmd_ptr)
    {
      gstk_free(uim_cmd_ptr);
      uim_cmd_ptr = NULL;
    }
    user_data   = gstk_tal_client_req_table[index].client_user_data;
  }

  if(uim_rpt_ptr->rpt_type == UIM_TERMINAL_PROFILE_R ||
     uim_rpt_ptr->rpt_type == UIM_TERMINAL_RESPONSE_R ||
     uim_rpt_ptr->rpt_type == UIM_ENVELOPE_R)
  {
      tal_cmd.cmd_details.tal_uim_rpt.sw1           = (uint32)uim_rpt_ptr->sw1;
      tal_cmd.cmd_details.tal_uim_rpt.sw2           = (uint32)uim_rpt_ptr->sw2;
        tal_cmd.cmd_details.tal_uim_rpt.user_data   = user_data;
      tal_cmd.cmd_details.tal_uim_rpt.rpt_status    =
        (gstk_tal_rpt_status_enum_type)uim_rpt_ptr->rpt_status;
  }

  switch(uim_rpt_ptr->rpt_type)
  {
    case UIM_TERMINAL_PROFILE_R:
      (void)rex_set_timer(&tal_delay_fetch_timer, GSTK_TAL_DELAY_FETCH_DURATION);
      tal_cmd.cmd_group = GSTK_TERMINAL_PROFILE_RSP;
      tal_first_envelope_sent = FALSE;
      for(i=0; i < GSTK_TAL_MAX_CLIENTS; i++)
      {
        memset(&gstk_tal_global.loc_status_cache[i], 0x00, sizeof(gstk_tal_loc_status_env_type));
        gstk_tal_global.rat[i] = GSTK_ACCESS_NONE;
      }
      gstk_tal_global.last_access_tech_sent.access_tech  = GSTK_TAL_ACCESS_TECH_NONE;
      gstk_tal_global.last_access_tech_sent.client       = GSTK_TAL_MAX_CLIENTS;
      break;
    case UIM_TERMINAL_RESPONSE_R:
      tal_cmd.cmd_group = GSTK_TERMINAL_RSP;
      tal_cmd.cmd_details.tal_uim_rpt.proactive_cmd_pending =
        uim_rpt_ptr->rpt.terminal_response.proactive_cmd_pending;

      if (tal_cmd.cmd_details.tal_uim_rpt.proactive_cmd_pending == FALSE)
      {
        gstk_tal_pro_cmd_route = GSTK_TAL_ROUTE_TO_BOTH;
      }

      /* Flush error terminal response cache */
      if(gstk_tal_global.error_tr_cache.data_ptr)
      {
        gstk_free(gstk_tal_global.error_tr_cache.data_ptr);
        gstk_tal_global.error_tr_cache.data_ptr = NULL;
      }
      memset(&gstk_tal_global.error_tr_cache, 0x00,
             sizeof(gstk_tal_uim_cmd_type));
      break;
    case UIM_ENVELOPE_R:
      tal_cmd.cmd_group = GSTK_ENVELOPE_RSP;
      tal_cmd.cmd_details.tal_uim_rpt.get_rsp_sw1 = (uint8)uim_rpt_ptr->rpt.envelope.get_resp_sw1;
      tal_cmd.cmd_details.tal_uim_rpt.get_rsp_sw2 = (uint8)uim_rpt_ptr->rpt.envelope.get_resp_sw2;
      tal_cmd.cmd_details.tal_uim_rpt.data_length = uim_rpt_ptr->rpt.envelope.data_length;
      if(tal_cmd.cmd_details.tal_uim_rpt.data_length > 0)
      {
        tal_cmd.cmd_details.tal_uim_rpt.data_ptr =
          (uint8 *)GSTK_CALLOC(uim_rpt_ptr->rpt.envelope.data_length);
        GSTK_RETURN_IF_NULL_PTR(tal_cmd.cmd_details.tal_uim_rpt.data_ptr);
        if(gstk_memcpy((void *)tal_cmd.cmd_details.tal_uim_rpt.data_ptr,
                       (void *)uim_rpt_ptr->rpt.envelope.data,
                       uim_rpt_ptr->rpt.envelope.data_length,
                       uim_rpt_ptr->rpt.envelope.data_length,
                       sizeof(uim_rpt_ptr->rpt.envelope.data)) <
           (size_t)uim_rpt_ptr->rpt.envelope.data_length)
        {
          gstk_free(tal_cmd.cmd_details.tal_uim_rpt.data_ptr);
          return GSTK_MEMORY_ERROR;
        }
      }
      break;
    default:
      UIM_MSG_ERR_0("TAL: Wrong Report response from UIM");
      break;
  }

  if(uim_rpt_ptr->rpt_type == UIM_ENVELOPE_R)
  {
    if(index  < 0 && index >= GSTK_TAL_MAX_REQ)
    {
      UIM_MSG_ERR_1("TAL: Invalid TAL req_table index %d", index);
      if((tal_cmd.cmd_details.tal_uim_rpt.data_length > 0) &&
         (tal_cmd.cmd_details.tal_uim_rpt.data_ptr))
      {
        gstk_free((void *)tal_cmd.cmd_details.tal_uim_rpt.data_ptr);
        tal_cmd.cmd_details.tal_uim_rpt.data_ptr = NULL;
      }
      return;
    }
    if(gstk_tal_client_req_table[index].in_use == FALSE)
    {
      UIM_MSG_ERR_1("TAL: Client req table index 0x%x not in service", index);
      return;
    }
    if((gstk_tal_client_req_table[index].client_type != GSTK_TAL_LOCAL_CLIENT) &&
       (gstk_tal_client_req_table[index].client_type != GSTK_TAL_REMOTE_CLIENT))
    {
      UIM_MSG_ERR_1("TAL: gstk_tal_uim_generic_report incorrect client_type 0x%x",
                    gstk_tal_client_req_table[index].client_type);
      if((tal_cmd.cmd_details.tal_uim_rpt.data_length > 0) &&
         (tal_cmd.cmd_details.tal_uim_rpt.data_ptr))
      {
        gstk_free((void *)tal_cmd.cmd_details.tal_uim_rpt.data_ptr);
        tal_cmd.cmd_details.tal_uim_rpt.data_ptr = NULL;
      }
      /* Enter Critical Section */
      rex_enter_crit_sect(&gstk_shared_data.gstk_crit_sect);
      memset(&gstk_tal_client_req_table[index],0x00, sizeof(gstk_tal_client_req_data_type));
      rex_leave_crit_sect(&gstk_shared_data.gstk_crit_sect);
      UIM_MSG_HIGH_1("TAL: gstk_tal_client_req_table index = %d set free",
                     index);
      return;
    }
    UIM_MSG_HIGH_1("TAL: Envelope Response to 0x%x",
                   gstk_tal_client_req_table[index].client_type);
    /* Do not send envelope response if LS is triggered by TAL */
    /* Trigger LS envelope if applicable on envelope rsp for access tech */
    if(gstk_tal_client_req_table[index].ls_access_tech_status != GSTK_TAL_LS_ACCESS_TECH_LS_SENT_TO_UIM)
    {
      if(gstk_tal_client_req_table[index].ls_access_tech_status == GSTK_TAL_LS_ACCESS_TECH_SEND_IN_PROGRESS)
      {
        UIM_MSG_HIGH_0("TAL: GSTK_TAL_LS_ACCESS_TECH_LS_SENT_TO_UIM");
        gstk_tal_send_pending_ls();
      }
      if(gstk_tal_client_cb_list[gstk_tal_client_req_table[index].client_type] != NULL)
      {
        (void)gstk_tal_client_cb_list[gstk_tal_client_req_table[index].client_type](&tal_cmd);
      }
      else
      {
        UIM_MSG_ERR_1("TAL: NULL Callback for Envelope Response Client Type 0x%x",
                      gstk_tal_client_req_table[index].client_type);
      }
    }
    else
    {
      UIM_MSG_HIGH_0("TAL: GSTK_TAL_LS_ACESSS_TECH_LS_SENT_TO_UIM");
      /* Clear resend flag for location status */
      for(i=0; i<GSTK_TAL_MAX_ACCESS_TECH_TYPE; i++)
      {
        if(gstk_tal_global.loc_status_table[i].resend)
        {
          UIM_MSG_HIGH_1("TAL: Resend flag on for access tech 0x%x",
                         gstk_tal_global.loc_status_table[i].access_tech);
          gstk_tal_global.loc_status_table[i].resend = FALSE;
          break;
        }
      }
    }

    if((tal_cmd.cmd_details.tal_uim_rpt.data_length > 0) &&
       (tal_cmd.cmd_details.tal_uim_rpt.data_ptr))
    {
      gstk_free((void *)tal_cmd.cmd_details.tal_uim_rpt.data_ptr);
      tal_cmd.cmd_details.tal_uim_rpt.data_ptr = NULL;
    }
    /* Enter Critical Section */
    rex_enter_crit_sect(&gstk_shared_data.gstk_crit_sect);
    memset(&gstk_tal_client_req_table[index],0x00, sizeof(gstk_tal_client_req_data_type));
    rex_leave_crit_sect(&gstk_shared_data.gstk_crit_sect);
    UIM_MSG_HIGH_1("TAL: gstk_tal_client_req_table index = %d set free",
                   index);
    return;
  }

  /* Inform Local and Remote GSTKs in their callbacks */
  if(gstk_tal_pro_cmd_route == GSTK_TAL_ROUTE_TO_BOTH
     || uim_rpt_ptr->rpt_type == UIM_TERMINAL_PROFILE_R)
  {
    if(gstk_tal_client_cb_list[GSTK_TAL_REMOTE_CLIENT] != NULL)
    {
      UIM_MSG_HIGH_0("TAL: Remote Client CB not null");
      (void)gstk_tal_client_cb_list[GSTK_TAL_REMOTE_CLIENT](&tal_cmd);
    }
    else
    {
      UIM_MSG_ERR_0("TAL: Remote GSTK client not registered");
    }
    if(gstk_tal_client_cb_list[GSTK_TAL_LOCAL_CLIENT] != NULL)
    {
      UIM_MSG_HIGH_0("TAL: Local Client CB not null");
      (void)gstk_tal_client_cb_list[GSTK_TAL_LOCAL_CLIENT](&tal_cmd);
    }
    else
    {
      UIM_MSG_ERR_0("TAL: Local GSTK client not registered");
    }
  }
  else if(gstk_tal_pro_cmd_route == GSTK_TAL_ROUTE_TO_LOCAL)
  {
    if(gstk_tal_client_cb_list[GSTK_TAL_LOCAL_CLIENT] != NULL)
    {
      (void)gstk_tal_client_cb_list[GSTK_TAL_LOCAL_CLIENT](&tal_cmd);
    }
    else
    {
      UIM_MSG_ERR_0("TAL: Local GSTK client not registered");
    }
  }
  else if(gstk_tal_pro_cmd_route == GSTK_TAL_ROUTE_TO_REMOTE)
  {
    if(gstk_tal_client_cb_list[GSTK_TAL_REMOTE_CLIENT] != NULL)
    {
      (void)gstk_tal_client_cb_list[GSTK_TAL_REMOTE_CLIENT](&tal_cmd);
    }
    else
    {
      UIM_MSG_ERR_0("TAL: Remote GSTK client not registered");
    }
  }
  else
  {
    UIM_MSG_ERR_0("TAL: Fail to route ");
  }
  if(index >= 0 && index < GSTK_TAL_MAX_REQ)
  {
    /* Enter Critical Section */
    rex_enter_crit_sect(&gstk_shared_data.gstk_crit_sect);
    memset(&gstk_tal_client_req_table[index],0x00, sizeof(gstk_tal_client_req_data_type));
    rex_leave_crit_sect(&gstk_shared_data.gstk_crit_sect);
    UIM_MSG_HIGH_1("TAL: gstk_tal_client_req_table index = %d set free",
                   index);
    /* Leave Critical Section */
  }

} /*gstk_tal_uim_generic_report*/


/*===========================================================================
FUNCTION gstk_tal_send_cmd_to_uim_server

DESCRIPTION
  This function sends the uim cmd to the uim server

PARAMETERS
  cmd_ptr: [Input] Pointer to uim_cmd_type to be sent to uim server

DEPENDENCIES
  None

RETURN VALUE
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
void gstk_tal_send_cmd_to_uim_server (
  uim_cmd_type *cmd_ptr
)
{
  UIM_MSG_HIGH_0("TAL: Sending Command to UIM Server");
  if(cmd_ptr == NULL)
  {
    UIM_MSG_ERR_0("TAL: cmd_ptr ERR:NULL");
    return;
  }
  uim_return_type  uim_state = UIM_SUCCESS;
  uim_rpt_type     uim_rpt;/
  memset((void *)&uim_rpt, 0, sizeof(uim_rpt_type));

  GSTK_RETURN_IF_NULL_PTR(gstk_curr_inst_ptr);

  (void)q_link(cmd_ptr, &cmd_ptr->hdr.cmd_hdr.link);

  /* Entering gstk critical section */
  rex_enter_crit_sect(&gstk_shared_data.gstk_crit_sect);
  if(cmd_ptr->hdr.command == UIM_TERMINAL_RESPONSE_F)
  {
    gstk_curr_inst_ptr->gstk_proactive_cmd_in_progress = FALSE;
  }
  rex_leave_crit_sect(&gstk_shared_data.gstk_crit_sect);
  /* Leaving gstk critical section */

  uim_state = uim_cmd(cmd_ptr);
  if(uim_state != UIM_SUCCESS)
  {
     uim_rpt.user_data   = cmd_ptr->hdr.user_data;
     uim_rpt.sw1         = SW1_PROBLEM;
     uim_rpt.sw2         = SW2_NORMAL_END;
     uim_rpt.rpt_status  = UIM_FAIL;
     uim_rpt.slot        = cmd_ptr->hdr.slot;
     uim_rpt.rpt_type    = cmd_ptr->hdr.command;
     switch(cmd_ptr->hdr.command)
     {
     case UIM_TERMINAL_RESPONSE_F:
       uim_rpt.rpt_type    = UIM_TERMINAL_RESPONSE_R;
       break;
     case UIM_TERMINAL_PROFILE_F:
       uim_rpt.rpt_type    = UIM_TERMINAL_PROFILE_R;
       break;
     case UIM_ENVELOPE_F:
       uim_rpt.rpt.envelope.get_resp_sw1 = SW1_PROBLEM;
       uim_rpt.rpt.envelope.get_resp_sw2 = SW2_NORMAL_END;
       uim_rpt.rpt_type    = UIM_ENVELOPE_R;
       break;
     default:
       UIM_MSG_ERR_0("Unsupported command by GSTK");
       break;
     }
     gstk_tal_uim_generic_report(uim_rpt);
   }
   gstk_send_cmd_to_uim_free_q(cmd_ptr);
   cmd_ptr = NULL;
} /* gstk_tal_send_cmd_to_uim_server */

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
gstk_status_enum_type gstk_tal_send_tp_cmd_to_uim(void)
{
  uim_cmd_type                   *uim_cmd_ptr      = NULL;
  gstk_status_enum_type           gstk_status      = GSTK_SUCCESS;
  int32                           index            = -1;

  /* dump TP contents*/
  gstk_util_dump_byte_array("gstk_tal_terminal_profile_cache",
                            gstk_tal_terminal_profile_cache,
                            GSTK_TERMINAL_PROFILE_SIZE);

  /* Send Message to UIM */
  /* get a buffer for the request */
  uim_cmd_ptr = (uim_cmd_type*)GSTK_CALLOC(sizeof(uim_cmd_type));

  if (uim_cmd_ptr != NULL)
  {
    gstk_status = gstk_util_populate_uim_buf_hdr_info(
                    UIM_TERMINAL_PROFILE_F, &uim_cmd_ptr);
    if (gstk_status == GSTK_SUCCESS)
    {
      if((index = gstk_tal_get_free_client_req_tbl_index()) < 0)
      {
        UIM_MSG_ERR_0("TAL: No free index avail in req table");
        gstk_free(uim_cmd_ptr);
        return GSTK_ERROR;
      }

      /* Populate client request table */
      gstk_tal_client_req_table[index].client_user_data = 0;
      gstk_tal_client_req_table[index].tal_user_data    = (uint32)(uim_cmd_ptr);

      /* Populate UIM CMD PTR */
      uim_cmd_ptr->hdr.user_data               = int32touint32(index);
      uim_cmd_ptr->terminal_profile.num_bytes  = UIM_TERMINAL_PROFILE_DATA_LENGTH;

      (void)memscpy(uim_cmd_ptr->terminal_profile.data,
      	     sizeof(uim_cmd_ptr->terminal_profile.data),
             gstk_tal_terminal_profile_cache,
             UIM_TERMINAL_PROFILE_DATA_LENGTH);
      uim_cmd_ptr->hdr.rpt_function           = gstk_tal_uim_generic_report;
      gstk_tal_send_cmd_to_uim_server(uim_cmd_ptr);
    }
    else
    {
      UIM_MSG_ERR_0("TAL: Never here");
      gstk_util_notify_tp_dl_fail();
      gstk_free(uim_cmd_ptr);
      return GSTK_ERROR;
    }
  }
  else
  {
    gstk_util_notify_tp_dl_fail();
    return GSTK_MEMORY_ERROR;
  }
  return gstk_status;
} /* gstk_tal_send_tp_cmd_to_uim */

/*===========================================================================
FUNCTION gstk_tal_send_tr_to_uim

DESCRIPTION
  This function is used by client to send TR to TAL. TAL will consolidate
  the TR from local and remote(if applicable) and send it to UIM

PARAMETERS
  tr: [Input] Terminal Response contents

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type
  GSTK_SUCCESS            for succesful operation
  GSTK_MEMORY_ERROR       for malloc failures
  GSTK_ERROR              for generic failures

COMMENTS
  This function should only be called after gstk_client_init has been called

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
static gstk_status_enum_type gstk_tal_send_tr_to_uim(gstk_tal_uim_cmd_type *uim_tr_ptr)
{
  uim_cmd_type                        *uim_cmd_ptr = NULL;
  int32                                index       = -1;

  GSTK_RETURN_ERROR_IF_NULL_PTR(uim_tr_ptr, GSTK_ERROR);
  GSTK_RETURN_ERROR_IF_NULL_PTR(uim_tr_ptr->data_ptr, GSTK_ERROR);

  UIM_MSG_HIGH_1("TAL: gstk_tal_send_tr_to_uim, Command Route 0x%x",
                 gstk_tal_pro_cmd_route);

  switch(gstk_tal_pro_cmd_route)
  {
    case GSTK_TAL_ROUTE_TO_REMOTE:
    case GSTK_TAL_ROUTE_TO_LOCAL:
      if(gstk_tal_expected_num_tr > 0)
      {
        gstk_tal_expected_num_tr--;
      }
       /* clear signal and call uim_cmd */
      break;

    case GSTK_TAL_ROUTE_TO_BOTH:
      /* 0 is Success, 7 is partial success */
      if(uim_tr_ptr->data_ptr[GSTK_TAL_TR_RESULT_BYTE - 1]
         == GSTK_COMMAND_PERFORMED_SUCCESSFULLY
         ||
         uim_tr_ptr->data_ptr[GSTK_TAL_TR_RESULT_BYTE - 1]
          == GSTK_COMMAND_PERFORMED_WITH_MODIFICATION)
      {
        if(gstk_tal_expected_num_tr > 0)
        {
          gstk_tal_expected_num_tr--;
        }
      }
      else
      {
        UIM_MSG_HIGH_0("TAL: One TR from GSTK shows failure");
        /* cache error TR */
        gstk_tal_global.error_tr_cache.cmd_type   = uim_tr_ptr->cmd_type;
        gstk_tal_global.error_tr_cache.slot       = uim_tr_ptr->slot;
        gstk_tal_global.error_tr_cache.user_data  = uim_tr_ptr->user_data;
        gstk_tal_global.error_tr_cache.num_bytes  = uim_tr_ptr->num_bytes;
        if(gstk_tal_global.error_tr_cache.data_ptr)
        {
          gstk_free(gstk_tal_global.error_tr_cache.data_ptr);
          gstk_tal_global.error_tr_cache.data_ptr = NULL;
        }
        gstk_tal_global.error_tr_cache.data_ptr =
          GSTK_CALLOC(uim_tr_ptr->num_bytes);

        GSTK_RETURN_ERROR_IF_NULL_PTR(gstk_tal_global.error_tr_cache.data_ptr, GSTK_ERROR);

        (void)memscpy((void*)gstk_tal_global.error_tr_cache.data_ptr,
               uim_tr_ptr->num_bytes
               (void*)uim_tr_ptr->data_ptr,
               uim_tr_ptr->num_bytes);

        if(gstk_tal_expected_num_tr > 0)
        {
          gstk_tal_expected_num_tr--;
        }
      }
      break;

    case GSTK_TAL_ROUTE_DROP:
    case GSTK_TAL_ROUTE_MAX:
    default:
      if(gstk_tal_expected_num_tr > 0)
      {
        gstk_tal_expected_num_tr--;
      }
      break;
  }

  if(gstk_tal_expected_num_tr == 0)
  {
    UIM_MSG_HIGH_0("TAL: Sending TR to UIM");

    /*  Memory allocated on the heap will be free'ed in TAL's uim cb */
    uim_cmd_ptr = (uim_cmd_type*)GSTK_CALLOC(sizeof(uim_cmd_type));
    if (uim_cmd_ptr == NULL)
    {
      return GSTK_MEMORY_ERROR;
    }
    if(gstk_util_populate_uim_buf_hdr_info(UIM_TERMINAL_RESPONSE_F,
                                           &uim_cmd_ptr)
       != GSTK_SUCCESS)
    {
      UIM_MSG_ERR_0("TAL: Populate uim buf header info failed");
      gstk_free(uim_cmd_ptr);
      return GSTK_ERROR;
    }

    if((index = gstk_tal_get_free_client_req_tbl_index()) < 0)
    {
      UIM_MSG_ERR_0("TAL: No free index avail in req table");
      gstk_free(uim_cmd_ptr);
      return GSTK_ERROR;
    }

    /* Populate client request table */
    gstk_tal_client_req_table[index].client_user_data = uim_tr_ptr->user_data;
    gstk_tal_client_req_table[index].tal_user_data    = (uint32)(uim_cmd_ptr);

    /* Populate uim cmd ptr */
    uim_cmd_ptr->hdr.rpt_function            = gstk_tal_uim_generic_report;
    uim_cmd_ptr->hdr.user_data               = int32touint32(index);

    /* If we have error TR received during this proactive session
       then use the error TR to be sent to the card.

       Error TR cache will be flushed when we receive response from
       UIM driver for TR
    */
    if(gstk_tal_global.error_tr_cache.num_bytes > 0)
    {
      uim_cmd_ptr->terminal_response.num_bytes =
        uint16touint8(gstk_tal_global.error_tr_cache.num_bytes);
      memset(uim_cmd_ptr->terminal_response.data,
             0x00, sizeof(uim_cmd_ptr->terminal_response.data));
      (void)memscpy(uim_cmd_ptr->terminal_response.data,
               sizeof(uim_cmd_ptr->terminal_response.data),
             gstk_tal_global.error_tr_cache.data_ptr,
             gstk_tal_global.error_tr_cache.num_bytes);
    }
    else
    {
      uim_cmd_ptr->terminal_response.num_bytes =
        uint16touint8(uim_tr_ptr->num_bytes);
      memset(uim_cmd_ptr->terminal_response.data,
             0x00, sizeof(uim_cmd_ptr->terminal_response.data));
      (void)memscpy(uim_cmd_ptr->terminal_response.data,
      	     sizeof(uim_cmd_ptr->terminal_response.data),
             uim_tr_ptr->data_ptr, uim_tr_ptr->num_bytes);
    }
    gstk_tal_send_cmd_to_uim_server(uim_cmd_ptr);

  } /* if (gstk_tal_expected_num_tr == 0)*/
  return GSTK_SUCCESS;
} /* gstk_tal_send_tr_to_uim */

/*===========================================================================
FUNCTION gstk_tal_notify_otasp_status

DESCRIPTION
  This function is used by client to send OTASP status to TAL. This OTASP
  status is further used while routing subsequent BIP commands

PARAMETERS
  client_type: [Input] Local or Remote client
  cmd_ptr: [Input] Ptr to envelope

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
static gstk_status_enum_type gstk_tal_notify_otasp_status(
  uint32                  otasp_in_progress)
{
  UIM_MSG_HIGH_1("TAL: otasp_in_progress is %d", otasp_in_progress);
  if(otasp_in_progress)
  {
    gstk_tal_global.otasp_in_progress = TRUE;
  }
  else
  {
    gstk_tal_global.otasp_in_progress = FALSE;
  }
  return GSTK_SUCCESS;
} /*gstk_tal_notify_otasp_status*/

/*===========================================================================
FUNCTION gstk_tal_send_tp_to_uim

DESCRIPTION
  This function is used by client to send TP to TAL. TAL will consolidate
  the TP from local and remote and send it to UIM

PARAMETERS
  tp_ptr: [Input] Terminal Profile contents

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
static gstk_status_enum_type gstk_tal_send_tp_to_uim(uint8 *tp_ptr,
                                                     uint16 num_bytes)
{
  uint8                           i  = 0;
  UIM_MSG_HIGH_0("TAL: gstk_tal_send_tp_to_uim");

  if(tp_ptr == NULL)
  {
    UIM_MSG_ERR_0("TAL:Null pointer as call back");
    return GSTK_NULL_INPUT_PARAM;
  }

  if(num_bytes > GSTK_TERMINAL_PROFILE_SIZE )
  {
    UIM_MSG_ERR_1("TAL:Invalid TP data size 0x%x", num_bytes);
    return GSTK_BAD_PARAM;
  }

  /* Update the TAL's copy of TP cache */
  for(i=0; (i<GSTK_TERMINAL_PROFILE_SIZE) && (i<num_bytes); i++)
  {
    gstk_tal_terminal_profile_cache[i] =
      gstk_tal_terminal_profile_cache[i] | tp_ptr[i];
  }

  /* Update the number of Terminal Profiles recieved so far */
  gstk_tal_num_tp_received++;

  /* If both Local and Remote have sent TP, send to UIM */
  if(gstk_tal_num_tp_received == GSTK_TAL_MAX_TP)
  {
    UIM_MSG_HIGH_0("TAL: Send TP to UIM");
    /* Reset num of TPs TAL received to zero in case UICC resets
       and GSTK has to do TP again */
    gstk_tal_num_tp_received = 0;
    return gstk_tal_send_tp_cmd_to_uim();
  }

  return GSTK_SUCCESS;
} /* gstk_tal_send_tp_to_uim */

/*===========================================================================
FUNCTION gstk_tal_process_loc_status

DESCRIPTION
  This function checks if the current envelope command is for location status.
  If so it will try to handle the scenario for both radios being active and
  prevents multiple envelopes sent to UICC which can potentially confuse the
  UICC.

PARAMETERS
  client_type: [Input] Local or Remote client
  uim_env_ptr: [Input] Ptr to envelope

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
gstk_status_enum_type gstk_tal_process_loc_status(
  gstk_tal_client_type             client_type,
  gstk_tal_uim_cmd_type            *uim_env_ptr,
  gstk_tal_generic_data_type       *new_env_ptr)
{
  uint8                               i = 0;
  boolean                             copy_flag = FALSE;
  uint8                               recent_ls_index = GSTK_TAL_MAX_ACCESS_TECH_TYPE;
  uint8                               curr_ls_index = GSTK_TAL_MAX_ACCESS_TECH_TYPE;
  gstk_location_status_enum_type      recent_ls = GSTK_NO_SERVICE;
  gstk_location_status_enum_type      current_ls = GSTK_NO_SERVICE;
  uint8                               send_ls_index = GSTK_TAL_MAX_ACCESS_TECH_TYPE;
  boolean                             success_flag = FALSE;
  boolean                             return_flag = TRUE;

  UIM_MSG_HIGH_1("TAL: gstk_tal_process_loc_status for client 0x%x",
                 client_type);

  if(uim_env_ptr == NULL || new_env_ptr == NULL)
  {
    UIM_MSG_ERR_0("TAL: Input pointers NULL");
    return GSTK_ERROR;
  }

  if(client_type >= (gstk_tal_client_type)GSTK_TAL_MAX_CLIENTS)
  {
    UIM_MSG_ERR_1("TAL: Client_type is out of bounds 0x%x", client_type);
    return GSTK_ERROR;
  }

  /* Copy the current location status to cache */
  /* Try to find a matching entry in the table to update */
  /* First narrow down by access tech and if fail then on access tech */
  /* If access tech is NONE then it means current LS has to be No/Limited
     - hence update all the access tech in that route with these values */
  /* Also use this run to pick up index of the last sent LS */

  for(i=0; i<GSTK_TAL_MAX_ACCESS_TECH_TYPE; i++)
  {
    UIM_MSG_HIGH_3("TAL: Checking Match for access tech %d for client type %d resend_flag %d",
                   gstk_tal_global.loc_status_table[i].access_tech,
                   gstk_tal_global.loc_status_table[i].client_type,
                   gstk_tal_global.loc_status_table[i].most_recent);
    UIM_MSG_HIGH_3("TAL: Number of bytes is %d and resend_flag %d and data_ptr 0x%x",
                   gstk_tal_global.loc_status_table[i].num_bytes,
                   gstk_tal_global.loc_status_table[i].resend,
                   gstk_tal_global.loc_status_table[i].data_ptr);

    /* dump table contents*/
    if(gstk_tal_global.loc_status_table[i].data_ptr)
    {
      UIM_MSG_HIGH_1("TAL: Location Status Table for Index %d", i);
      gstk_util_dump_byte_array("LS_Table",
                                gstk_tal_global.loc_status_table[i].data_ptr,
                                gstk_tal_global.loc_status_table[i].num_bytes);
    }

    if(gstk_tal_global.loc_status_table[i].most_recent == TRUE)
    {
      recent_ls_index = i;
      recent_ls = (gstk_location_status_enum_type)gstk_tal_global.loc_status_table[recent_ls_index].data_ptr[GSTK_TAL_LS_POS_OF_SRV_BYTE];
      UIM_MSG_HIGH_2("TAL: recent_ls_index %d and recent_ls %d",
                     recent_ls_index, recent_ls);
    }
    if(client_type == gstk_tal_global.loc_status_table[i].client_type)
    {
      UIM_MSG_HIGH_1("TAL: Client Type match at index %d", i);

      if(gstk_tal_global.current_active_rat[client_type] == gstk_tal_global.loc_status_table[i].access_tech)
      {
        UIM_MSG_HIGH_2("TAL: Match access tech %d for LS at index %d",
                       gstk_tal_global.loc_status_table[i].access_tech, i);
        copy_flag = TRUE;
      }
      else if(gstk_tal_global.current_active_rat[client_type] == GSTK_TAL_ACCESS_TECH_NONE)
      {
        UIM_MSG_HIGH_0("TAL: Access Tech None so far");
        copy_flag = TRUE;
      }

      if(copy_flag == TRUE)
      {
        if(curr_ls_index == GSTK_TAL_MAX_ACCESS_TECH_TYPE)
        {
          curr_ls_index = i;
          UIM_MSG_HIGH_1("TAL: Setting curr_ls_index %d", i);
        }
        /* Copy to the table in this index */
        if(gstk_tal_global.loc_status_table[i].data_ptr != NULL)
        {
          gstk_free(gstk_tal_global.loc_status_table[i].data_ptr);
          gstk_tal_global.loc_status_table[i].data_ptr = NULL;
        }
        gstk_tal_global.loc_status_table[i].num_bytes = uim_env_ptr->num_bytes;
        gstk_tal_global.loc_status_table[i].data_ptr =
          (uint8 *)GSTK_CALLOC(uim_env_ptr->num_bytes * sizeof(uint8));
        GSTK_TAL_CHECK_IF_PTR_NULL(gstk_tal_global.loc_status_table[i].data_ptr, return_flag);
        if(gstk_memcpy(gstk_tal_global.loc_status_table[i].data_ptr,
                       uim_env_ptr->data_ptr,
                       gstk_tal_global.loc_status_table[i].num_bytes,
                       gstk_tal_global.loc_status_table[i].num_bytes,
                       gstk_tal_global.loc_status_table[i].num_bytes) <
          (size_t)gstk_tal_global.loc_status_table[i].num_bytes)
        {
          gstk_free(gstk_tal_global.loc_status_table[i].data_ptr);
          gstk_tal_global.loc_status_table[i].data_ptr = NULL;
          return GSTK_MEMORY_ERROR;
        }
        copy_flag = FALSE;
      }
    }
  }

  /* If first location status - send - anyways */
  if(recent_ls_index == GSTK_TAL_MAX_ACCESS_TECH_TYPE)
  {
    UIM_MSG_HIGH_0("TAL: First LS - send this anyways");
    send_ls_index = curr_ls_index;
    success_flag = TRUE;
    goto GSTK_TAL_PROCESS_LOC_STATUS_RETURN;
  }

  if(curr_ls_index == GSTK_TAL_MAX_ACCESS_TECH_TYPE)
  {
    UIM_MSG_ERR_0("TAL Matching LS index failed - curr_ls_index invalid");
    return GSTK_ERROR;
  }
  /* Normal Service(0) < Limited Service(1) < No Service(2) */
  /* Compare current LS against most recent LS */
  current_ls = (gstk_location_status_enum_type)uim_env_ptr->data_ptr[GSTK_TAL_LS_POS_OF_SRV_BYTE];
  UIM_MSG_HIGH_2("TAL: Recent LS: 0x%x current LS: 0x%x",
                 recent_ls, current_ls);

  /* If currrent LS == prev LS - compare priority */
  if(current_ls == recent_ls)
  {
    /* Go through the LS table - find if any other RAT with higher priority has same LS */
    for(i=0; i<curr_ls_index; i++)
    {
      if(gstk_tal_global.loc_status_table[i].data_ptr == NULL)
      {
        UIM_MSG_HIGH_1("TAL: Data pointer NULL for index %d", i);
        continue;
      }
      if(gstk_tal_global.loc_status_table[i].data_ptr[GSTK_TAL_LS_POS_OF_SRV_BYTE] == current_ls)
      {
        UIM_MSG_HIGH_2("TAL: Higher priority Access Tech with same LS 0x%x with recent flag %d",
                       gstk_tal_global.loc_status_table[i].access_tech,
                       gstk_tal_global.loc_status_table[i].most_recent);
        /* If most recent LS was for same access tech just send this LS down - No Access Tech */
        if(gstk_tal_global.loc_status_table[i].most_recent)
        {
          UIM_MSG_HIGH_0("TAL: Duplicate of the same location status");
          return GSTK_ERROR; /* Dont send duplicate again */
        }
        /* If LS is in progress i.e send access tech and then LS - return error */
        /* On getting access tech envelope response this LS will be sent again */
        if(gstk_tal_global.loc_status_table[i].resend)
        {
          UIM_MSG_HIGH_0("TAL: LS resend in progress just return");
          return GSTK_ERROR;
        }
        /* Send Access Tech Change and later send LS on envelope response for access tech */
        success_flag = FALSE;
        send_ls_index = i;
        goto GSTK_TAL_PROCESS_LOC_STATUS_RETURN;
      }
    }
    if(i == curr_ls_index)
    {
      UIM_MSG_HIGH_0("TAL: Did not find high priority tech with same LS Send Current LS");
      success_flag = TRUE;
      send_ls_index = curr_ls_index;
      goto GSTK_TAL_PROCESS_LOC_STATUS_RETURN;
    }
  }
  else if(current_ls < recent_ls)
  {
    UIM_MSG_HIGH_1("TAL: Current LS better than Previous LS, Resend = 0x%x",
                   gstk_tal_global.loc_status_table[curr_ls_index].resend);
    /* If LS is in progress i.e send access tech and then LS - return busy */
    if(gstk_tal_global.loc_status_table[curr_ls_index].resend)
    {
      return GSTK_ERROR;
    }
    /* If most recent LS was for same access tech just send this LS down - No Access Tech */
    if((gstk_tal_global.loc_status_table[curr_ls_index].most_recent) ||
       (gstk_tal_global.loc_status_table[curr_ls_index].access_tech ==
        gstk_tal_global.last_access_tech_sent.access_tech))
    {
      UIM_MSG_HIGH_0("TAL: Most recent access tech same");
      send_ls_index = curr_ls_index;
      success_flag = TRUE;
      goto GSTK_TAL_PROCESS_LOC_STATUS_RETURN;
    }
    success_flag = FALSE;
    send_ls_index = curr_ls_index;
    goto GSTK_TAL_PROCESS_LOC_STATUS_RETURN;
  }
  else
  {
    UIM_MSG_HIGH_0("TAL: Current LS worse than Previous LS");
    /* Go through the LS table - find if any other RAT with any other priority has better LS */
    for(i=0; i<GSTK_TAL_MAX_ACCESS_TECH_TYPE; i++)
    {
      if(gstk_tal_global.loc_status_table[i].data_ptr == NULL)
      {
        UIM_MSG_HIGH_1("TAL: Data pointer NULL for index %d", i);
        continue;
      }
      if(gstk_tal_global.loc_status_table[i].data_ptr[GSTK_TAL_LS_POS_OF_SRV_BYTE] < current_ls)
      {
        GSTK_TAL_CHECK_IF_PTR_NULL(gstk_tal_global.loc_status_table[i].data_ptr,
                                   return_flag);
        UIM_MSG_HIGH_3("TAL: Access Tech 0x%x with better LS 0x%x with recent flag %d",
                       gstk_tal_global.loc_status_table[i].access_tech,
                       gstk_tal_global.loc_status_table[i].data_ptr[GSTK_TAL_LS_POS_OF_SRV_BYTE],
                       gstk_tal_global.loc_status_table[i].most_recent);
        /* If most recent LS was for same access tech just send this LS down - No Access Tech */
        if(gstk_tal_global.loc_status_table[i].most_recent)
        {
          return GSTK_ERROR;
        }
        /* If LS is in progress i.e send access tech and then LS - return error */
        if(gstk_tal_global.loc_status_table[i].resend)
        {
          UIM_MSG_HIGH_0("TAL: LS resend in progress");
          return GSTK_ERROR;
        }
        success_flag = FALSE;
        send_ls_index = i;
        goto GSTK_TAL_PROCESS_LOC_STATUS_RETURN;
      }
    }
    if(i == GSTK_TAL_MAX_ACCESS_TECH_TYPE)
    {
      UIM_MSG_HIGH_0("TAL: No other RAT with better LS");
      if(gstk_tal_global.loc_status_table[curr_ls_index].most_recent)
      {
        success_flag = TRUE;
      }
      else
      {
        success_flag = FALSE;
      }
      send_ls_index = curr_ls_index;
      goto GSTK_TAL_PROCESS_LOC_STATUS_RETURN;
    }
  }

GSTK_TAL_PROCESS_LOC_STATUS_RETURN:
  if(recent_ls_index < GSTK_TAL_MAX_ACCESS_TECH_TYPE)
  {
    gstk_tal_global.loc_status_table[recent_ls_index].most_recent = FALSE;
  }
  else
  {
    UIM_MSG_HIGH_0("TAL: recent_ls_index equals GSTK_TAL_MAX_ACCESS_TECH_TYPE");
  }
  if(send_ls_index >= GSTK_TAL_MAX_ACCESS_TECH_TYPE)
  {
    UIM_MSG_ERR_0("TAL: Send LS Index invalid");
    return GSTK_ERROR;
  }
  gstk_tal_global.loc_status_table[send_ls_index].most_recent = TRUE;
  if(success_flag)
  {
    new_env_ptr->data_len = gstk_tal_global.loc_status_table[send_ls_index].num_bytes;
    new_env_ptr->data_ptr =
      (uint8 *)GSTK_CALLOC(new_env_ptr->data_len * sizeof(uint8));
    GSTK_TAL_CHECK_IF_PTR_NULL(new_env_ptr->data_ptr, return_flag);
    (void)gstk_memcpy(new_env_ptr->data_ptr,
                   gstk_tal_global.loc_status_table[send_ls_index].data_ptr,
                   new_env_ptr->data_len,
                   new_env_ptr->data_len,
                   new_env_ptr->data_len);
    return GSTK_SUCCESS;
  }
  else
  {
    UIM_MSG_HIGH_2("TAL: Building envelope for access tech 0x%x for client 0x%x",
                   gstk_tal_global.loc_status_table[send_ls_index].access_tech,
                   gstk_tal_global.loc_status_table[send_ls_index].client_type);
    /* Send Access Tech change event */
    (void)gstk_build_access_technology_envelope((gstk_access_technology_type)gstk_tal_global.loc_status_table[send_ls_index].access_tech);
    /* On getting envelope response for access tech change send locations status */
    gstk_tal_global.loc_status_table[send_ls_index].resend = TRUE;
    /* Store the current access tech envelope for check */
    gstk_tal_global.ls_triggered_access_tech.access_tech =
      gstk_tal_global.loc_status_table[send_ls_index].access_tech;
    gstk_tal_global.ls_triggered_access_tech.client      =
      gstk_tal_global.loc_status_table[send_ls_index].client_type;
    /* For the current LS send GSTK an Envelope response */
    return GSTK_ERROR; /* So that the envelope response goes from TAL itself */
  }
} /* gstk_tal_process_loc_status */

/*===========================================================================
FUNCTION gstk_tal_process_access_tech

DESCRIPTION
  This function checks the current Access Tech envelope, and determines
  which one should be sent

PARAMETERS
  client_type: [Input] Local or Remote client
  uim_env_ptr: [Input] Ptr to envelope

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
gstk_status_enum_type gstk_tal_process_access_tech(
  gstk_tal_client_type             client_type,
  gstk_tal_uim_cmd_type            *uim_env_ptr)
{

  gstk_access_technology_type  current_access_tech = GSTK_ACCESS_NONE;
  gstk_access_technology_type  prev_access_tech    = GSTK_ACCESS_NONE;
  uint8                        i                   = 0;
  uint8                        curr_act_index      = GSTK_TAL_MAX_ACCESS_TECH_TYPE;
  uint8                        prev_act_index      = GSTK_TAL_MAX_ACCESS_TECH_TYPE;
  gstk_tal_access_tech_type    temp_act            = GSTK_TAL_ACCESS_TECH_NONE;

  GSTK_RETURN_ERROR_IF_NULL_PTR(uim_env_ptr, GSTK_ERROR);
  if(!uim_env_ptr->data_ptr)
  {
    UIM_MSG_ERR_0("TAL: Null Access Tech Env Data");
    return GSTK_ERROR;
  }

  if((client_type < 0) || (client_type >= GSTK_TAL_MAX_CLIENTS))
  {
    UIM_MSG_ERR_1("TAL: Invalid client_type: 0x%x", client_type);
    return GSTK_ERROR;
  }

  current_access_tech = (gstk_access_technology_type)uim_env_ptr->data_ptr[GSTK_TAL_ACCESS_TECH_POS_OF_AT_BYTE];
  prev_access_tech    = (gstk_access_technology_type)gstk_tal_global.last_access_tech_sent.access_tech;

  UIM_MSG_HIGH_3("TAL: gstk_tal_process_access_tech client 0x%x, Curr AcT 0x%x, Prev AcT 0x%x",
                 client_type, current_access_tech, prev_access_tech);

  if(prev_access_tech == GSTK_ACCESS_NONE)
  {
    gstk_tal_global.last_access_tech_sent.access_tech = current_access_tech;
    gstk_tal_global.last_access_tech_sent.client = client_type;
    return GSTK_SUCCESS;
  }
  if(prev_access_tech == current_access_tech)
  {
    UIM_MSG_HIGH_0("TAL: Access Tech envelopes are identical, do not send");
    return GSTK_ERROR;
  }
  else
  {
    /* If previous and current access tech don't match then compare the priority of
       previous access tech vs current one. Send access tech only if priority of
       current access tech is higher than previous one else ignore it */

    for(i = 0; i < GSTK_TAL_MAX_ACCESS_TECH_TYPE; i++)
    {
      /* convert from gstk_tal_access_tech_type to gstk_access_tech_type */
      temp_act = gstk_tal_global.loc_status_table[i].access_tech;
      if(temp_act == GSTK_TAL_ACCESS_TECH_HRPD)
      {
        temp_act = GSTK_TAL_ACCESS_TECH_CDMA;
      }
      if((temp_act == (gstk_tal_access_tech_type)current_access_tech) &&
          (gstk_tal_global.loc_status_table[i].client_type == client_type))
      {
        UIM_MSG_HIGH_2("TAL: Match found for curr_act 0x%x at index %d",
                       current_access_tech, i);
        curr_act_index = i;
      }
      else if((temp_act == (gstk_tal_access_tech_type)prev_access_tech) &&
              (gstk_tal_global.loc_status_table[i].client_type ==
                 gstk_tal_global.last_access_tech_sent.client))
      {
        UIM_MSG_HIGH_2("TAL: Match found for prev_act 0x%x at index %d",
                       prev_access_tech, i);
        prev_act_index = i;
      }
    }
    /* Compare priorities by index only if they have valid values.
       Lower the index, higher the priority */
    if((curr_act_index != GSTK_TAL_MAX_ACCESS_TECH_TYPE) &&
       (prev_act_index != GSTK_TAL_MAX_ACCESS_TECH_TYPE) &&
       (prev_act_index < curr_act_index) &&
       (gstk_tal_global.loc_status_table[prev_act_index].most_recent) &&
       (gstk_tal_global.loc_status_table[prev_act_index].data_ptr))
    {
      /* Check if the prev access tech is still in Normal Service */
      if(gstk_tal_global.loc_status_table[prev_act_index].data_ptr[GSTK_TAL_LS_POS_OF_SRV_BYTE]
         == GSTK_NORMAL_SERVICE )
      {
        UIM_MSG_ERR_2("TAL: Can't Send Env, Prev AcT 0x%x with higher priority 0x%x is in Normal Service",
                      prev_access_tech, prev_act_index);
        return GSTK_ERROR;
      }
    }

    switch(gstk_tal_global.gstk_tal_location_status[client_type])
    {
      case GSTK_NORMAL_SERVICE:
        gstk_tal_global.last_access_tech_sent.access_tech = current_access_tech;
        gstk_tal_global.last_access_tech_sent.client = client_type;
        return GSTK_SUCCESS;
      case GSTK_LIMITED_SERVICE:
        if((current_access_tech != GSTK_TAL_ACCESS_TECH_CDMA) &&
           (current_access_tech != GSTK_TAL_ACCESS_TECH_HRPD))
        {
          gstk_tal_global.last_access_tech_sent.access_tech = current_access_tech;
          gstk_tal_global.last_access_tech_sent.client = client_type;
          return GSTK_SUCCESS;
        }
        else
        {
          UIM_MSG_ERR_0("Limited Service is not supported for CDMA/HDR");
          return GSTK_ERROR;
        }
      case GSTK_NO_SERVICE:
      default:
        UIM_MSG_HIGH_2("TAL: Do not send the Envelope for access Tech 0x%x  Location status is 0x%x",
                       current_access_tech,
                       gstk_tal_global.gstk_tal_location_status[client_type]);
        return GSTK_ERROR;
    }
  }
} /* gstk_tal_process_access_tech */

/*===========================================================================
FUNCTION gstk_tal_send_envelope_to_uim

DESCRIPTION
  This function is used by client to send envelope to TAL. TAL will consolidate
  the envelopes from local and remote(if applicable) and send it to UIM

PARAMETERS
  client_type: [Input] Local or Remote client
  uim_env_ptr: [Input] Ptr to envelope

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
static gstk_status_enum_type gstk_tal_send_envelope_to_uim(
  gstk_tal_client_type             client_type,
  gstk_tal_uim_cmd_type            *uim_env_ptr)
{
  uim_cmd_type                   *uim_cmd_ptr      = NULL;
  gstk_status_enum_type          gstk_status       = GSTK_SUCCESS;
  int32                          index             = -1;
  uint16                         offset            = 1;
  gstk_tal_cmd_details_type      tal_cmd;
  gstk_tal_generic_data_type     replaced_env;
  boolean                        is_env_access_tech = FALSE;

  UIM_MSG_HIGH_1("TAL: gstk_tal_send_envelope_to_uim for client 0x%x",
                 client_type);

  if (uim_env_ptr == NULL || uim_env_ptr->data_ptr == NULL)
  {
    UIM_MSG_ERR_2("TAL: Null Input pointer uim_env_ptr %p, data_ptr %p",
                  uim_env_ptr, uim_env_ptr? (uim_env_ptr->data_ptr):NULL);
    return GSTK_NULL_INPUT_PARAM;
  }

  if((client_type != GSTK_TAL_LOCAL_CLIENT &&
      client_type != GSTK_TAL_REMOTE_CLIENT)    ||
      uim_env_ptr->num_bytes > UIM_MAX_ENVELOPE_BYTES)
  {
    UIM_MSG_ERR_2("TAL:  BAD input params client_type 0x%x data_size 0x%x",
                  client_type, uim_env_ptr->num_bytes);
    return GSTK_BAD_PARAM;
  }
  memset(&tal_cmd, 0x00, sizeof(tal_cmd));
  memset(&replaced_env, 0x00, sizeof(replaced_env));

  /* Check if event download envelope */
  /* NOTE: For Velcro solutions this might have to be revisited as
     spec allows multiple event download envelopes to be combined
     to one command, but gstk does not support this now */
  if(uim_env_ptr->data_ptr[offset] == (uint8)GSTK_EVENT_DOWNLOAD_TAG
     && uim_env_ptr->data_ptr[offset + 2] == (uint8)GSTK_EVENT_LIST_TAG
     && uim_env_ptr->data_ptr[offset + 4] == (uint8)GSTK_LOCATION_STATUS
     && uim_env_ptr->data_ptr[offset + 9] == (uint8)GSTK_LOCATION_STATUS_TAG)
  {
    gstk_status = gstk_tal_process_loc_status(client_type, uim_env_ptr, &replaced_env);
    if(gstk_status != GSTK_SUCCESS)
    {
        tal_cmd.cmd_group = GSTK_ENVELOPE_RSP;
        tal_cmd.cmd_details.tal_uim_rpt.rpt_status = GSTK_TAL_UIM_PASS;
        tal_cmd.cmd_details.tal_uim_rpt.sw1 = (uint32)SW1_NORMAL_END;
        tal_cmd.cmd_details.tal_uim_rpt.sw2 = (uint32)SW2_NORMAL_END;
        tal_cmd.cmd_details.tal_uim_rpt.user_data = uim_env_ptr->user_data;
        UIM_MSG_HIGH_1("TAL: tal_cmd.cmd_details.tal_uim_rpt.user_data is 0x%x",
                       tal_cmd.cmd_details.tal_uim_rpt.user_data);
        GSTK_RETURN_ERROR_IF_NULL_PTR(gstk_tal_client_cb_list[client_type], GSTK_ERROR);
        gstk_tal_client_cb_list[client_type](&tal_cmd);
        return GSTK_SUCCESS;
    }
  }

  if(uim_env_ptr->data_ptr[offset] == (uint8)GSTK_EVENT_DOWNLOAD_TAG
     && uim_env_ptr->data_ptr[offset + 2] == (uint8)GSTK_EVENT_LIST_TAG
     && uim_env_ptr->data_ptr[offset + 4] == (uint8)GSTK_ACCESS_TECH
     && uim_env_ptr->data_ptr[offset + 9] == (uint8)GSTK_ACCESS_TECHNOLOGY_TAG)
  {
    gstk_tal_client_type  client_type_for_AcT = client_type;

    if(gstk_tal_global.ls_triggered_access_tech.access_tech ==
         (gstk_tal_access_tech_type) uim_env_ptr->data_ptr[GSTK_TAL_LS_POS_OF_SRV_BYTE])
    {
      if(gstk_tal_global.ls_triggered_access_tech.client != client_type)
      {
        UIM_MSG_HIGH_2("LS triggered AcT. Switching client type from 0x%x to 0x%x",
                       client_type,
                       gstk_tal_global.ls_triggered_access_tech.client);
        client_type_for_AcT = gstk_tal_global.ls_triggered_access_tech.client;
      }
      is_env_access_tech = TRUE;
      gstk_tal_global.ls_triggered_access_tech.client =
        GSTK_TAL_MAX_CLIENTS;
      gstk_tal_global.ls_triggered_access_tech.access_tech =
        GSTK_TAL_ACCESS_TECH_NONE;
    }

    if (((client_type_for_AcT < GSTK_TAL_MAX_CLIENTS)
         && ((gstk_status = gstk_tal_process_access_tech(client_type_for_AcT, uim_env_ptr))
             != GSTK_SUCCESS))
        || (client_type_for_AcT >= GSTK_TAL_MAX_CLIENTS))
    {
      tal_cmd.cmd_group = GSTK_ENVELOPE_RSP;
      tal_cmd.cmd_details.tal_uim_rpt.rpt_status = GSTK_TAL_UIM_PASS;
      tal_cmd.cmd_details.tal_uim_rpt.sw1 = (uint32)SW1_NORMAL_END;
      tal_cmd.cmd_details.tal_uim_rpt.sw2 = (uint32)SW2_NORMAL_END;
      tal_cmd.cmd_details.tal_uim_rpt.user_data = uim_env_ptr->user_data;
      UIM_MSG_HIGH_1("TAL: tal_cmd.cmd_details.tal_uim_rpt.user_data is 0x%x",
                     tal_cmd.cmd_details.tal_uim_rpt.user_data);
      GSTK_RETURN_ERROR_IF_NULL_PTR(gstk_tal_client_cb_list[client_type], GSTK_ERROR);
      gstk_tal_client_cb_list[client_type](&tal_cmd);
      return GSTK_SUCCESS;
    }
  }

  uim_cmd_ptr = (uim_cmd_type*)GSTK_CALLOC(sizeof(uim_cmd_type));

  if (uim_cmd_ptr == NULL )
  {
    return GSTK_MEMORY_ERROR;
  }

  gstk_status = gstk_util_populate_uim_buf_hdr_info(UIM_ENVELOPE_F,
                                                    &uim_cmd_ptr);

  if (gstk_status != GSTK_SUCCESS)
  {
    UIM_MSG_ERR_0("TAL: gstk_util_populate_uim_buf_hdr_info failed");
    gstk_free((void *)uim_cmd_ptr);
    return  gstk_status;
  }
  if((index = gstk_tal_get_free_client_req_tbl_index()) < 0)
  {
    UIM_MSG_ERR_0("TAL: No free index avail in req table");
    gstk_free(uim_cmd_ptr);
    return GSTK_ERROR;
  }

  /* Populate client request table */
  gstk_tal_client_req_table[index].client_user_data = uim_env_ptr->user_data;
  gstk_tal_client_req_table[index].tal_user_data    = (uint32)(uim_cmd_ptr);
  gstk_tal_client_req_table[index].client_type      = client_type;

  if(is_env_access_tech)
  {
    UIM_MSG_HIGH_0("TAL: GSTK_TAL_LS_ACCESS_TECH_SEND_IN_PROGRESS");
    gstk_tal_client_req_table[index].ls_access_tech_status = GSTK_TAL_LS_ACCESS_TECH_SEND_IN_PROGRESS;
  }
  else
  {
    UIM_MSG_HIGH_0("TAL: GSTK_TAL_LS_ACCESS_TECH_INACTIVE");
    gstk_tal_client_req_table[index].ls_access_tech_status = GSTK_TAL_LS_ACCESS_TECH_INACTIVE;
  }

  /* Populate UIM command pointer */
  uim_cmd_ptr->hdr.user_data      = int32touint32(index);
  uim_cmd_ptr->hdr.rpt_function   = gstk_tal_uim_generic_report;
  if (replaced_env.data_ptr && (replaced_env.data_len > 0))
  {
    uim_cmd_ptr->envelope.offset    = replaced_env.data_ptr[0];
    uim_cmd_ptr->envelope.num_bytes = uint16touint8(replaced_env.data_len - uim_cmd_ptr->envelope.offset - 1);
    (void)memscpy((void *)uim_cmd_ptr->envelope.data,
         sizeof(uim_cmd_ptr->envelope.data),
           (void *)(replaced_env.data_ptr + 1),
           replaced_env.data_len - 1 + uim_cmd_ptr->envelope.offset);
  }
  else
  {
    uim_cmd_ptr->envelope.offset    = uim_env_ptr->data_ptr[0];
    uim_cmd_ptr->envelope.num_bytes = uint16touint8(uim_env_ptr->num_bytes - uim_cmd_ptr->envelope.offset - 1);
    (void)memscpy((void *)uim_cmd_ptr->envelope.data,
          sizeof(uim_cmd_ptr->envelope.data),
          (void *)(uim_env_ptr->data_ptr + 1),
          (uim_env_ptr->num_bytes -1) + uim_cmd_ptr->envelope.offset);
  }
  gstk_tal_send_cmd_to_uim_server(uim_cmd_ptr);
  if(replaced_env.data_ptr != NULL)
  {
    gstk_free(replaced_env.data_ptr);
    replaced_env.data_ptr = NULL;
  }
  return gstk_status;
} /*gstk_tal_send_envelope_to_uim*/

/*===========================================================================
FUNCTION gstk_tal_send_proactive_cmd

DESCRIPTION
  This function peeks into the proactive command and sends it to local
  or remote or both depending on type of command

PARAMETERS
  slot: [Input] Slot type
  data_length: [Input] Length of the data
  data_ptr: [Input] Pointer to the data buffer that contains the Proactive cmd

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
gstk_status_enum_type gstk_tal_send_proactive_cmd(
  uim_slot_type           slot,
  uint32                  data_length,
  const uint8             *cmd_data_ptr)
{
  uint8      type_of_command             = 0x00;
  uint8      length_of_TLV_length_field  = 0x01;
  uint8     *data_ptr                    = NULL;
  boolean    error_flag                  = FALSE;
  uint8      qualifier                   = 0x00;
  uint8      offset                      = 0;
  gstk_tal_cmd_details_type tal_cmd;

  UIM_MSG_HIGH_0("TAL: gstk_tal_send_proactive_cmd");
  memset(&tal_cmd, 0x00, sizeof(gstk_tal_cmd_details_type));

  do
  {
    if(!cmd_data_ptr)
    {
      UIM_MSG_ERR_0("TAL: Null input param cmd_data_ptrTR");
      return GSTK_BAD_PARAM;
    }
    data_ptr = GSTK_CALLOC(data_length);
    if(!data_ptr)
    {
      error_flag = TRUE;
      break;
    }
    tal_cmd.cmd_group = GSTK_PROACTIVE_CMD;
    tal_cmd.cmd_details.tal_pro_cmd.data_length = data_length;
    memset(data_ptr,0x00, data_length);
    (void)memscpy(data_ptr,data_length,cmd_data_ptr,data_length);
    tal_cmd.cmd_details.tal_pro_cmd.data_ptr =  data_ptr;
    tal_cmd.cmd_details.tal_pro_cmd.slot     = (gstk_slot_id_enum_type)slot;

    /* Check if we have a valid proactive SIM command tag (0xD0) */
    if(!gstk_valid_tag(cmd_data_ptr[GSTK_PROACTIVE_CMD_TAG_INDEX],
                       (tags_enum_type)GSTK_PROACTIVE_SIM_COMMAND_TAG))
    {
      /* Not a proactive command tag, ignore the command */
      UIM_MSG_HIGH_1("TAL: Not a valid proactive command 0x%x",
                     cmd_data_ptr[GSTK_PROACTIVE_CMD_TAG_INDEX]);
      error_flag = TRUE;
      break;
    }
    /*
    ** BER-TLV LENGTH
    ** This length field can either be 0x00-0x7F or
    ** 0x81 if the 2nd byte is used
    */

    length_of_TLV_length_field = int32touint8(gstk_find_length_of_length_value(
                                              &cmd_data_ptr[1]));

    UIM_MSG_HIGH_1("TAL: Length of TLV: %x", length_of_TLV_length_field);

    if(length_of_TLV_length_field != 1 &&
       length_of_TLV_length_field != 2)
    {
      /* Invalid BER TLV len , ignore the command */
      UIM_MSG_HIGH_1("TAL: ERROR:  Length of TLV Invalid: 0x%x ",
                     length_of_TLV_length_field);
      error_flag = TRUE;
      break;
    }
    /* 4th location after total length field is the type of command
    ** 0   Proactive SIM Command Tag
    ** 1-2 total length
    ** +1  Command Details tag
    ** +1  Command details len = 3
    ** +1  Command number
    ** +1  Command Type
    ** +1  Command Qualifier
    */
    if((length_of_TLV_length_field + 5) > data_length)
    {
      error_flag = TRUE;
      break;
    }
    type_of_command = cmd_data_ptr[length_of_TLV_length_field + 4];
    qualifier       = cmd_data_ptr[length_of_TLV_length_field + 5];
    offset          = length_of_TLV_length_field + 5;
  } while(0);

  /* In case we found error in parsing command then force the command to local
     and let local GSTK send error terminal response */
  if(error_flag)
  {
    gstk_tal_pro_cmd_route = GSTK_TAL_ROUTE_TO_LOCAL;
  }
  else
  {
    gstk_tal_pro_cmd_route = gstk_tal_find_route(type_of_command, qualifier);
  }

  UIM_MSG_HIGH_1("TAL: Sending command route", gstk_tal_pro_cmd_route);
  switch(gstk_tal_pro_cmd_route)
  {
  case GSTK_TAL_ROUTE_TO_LOCAL:
    gstk_tal_expected_num_tr = 1;
    if(gstk_tal_client_cb_list[GSTK_TAL_LOCAL_CLIENT] != NULL)
    {
      (void)gstk_tal_client_cb_list[GSTK_TAL_LOCAL_CLIENT](&tal_cmd);
    }
    if(tal_cmd.cmd_details.tal_pro_cmd.data_ptr)
    {
      gstk_free(tal_cmd.cmd_details.tal_pro_cmd.data_ptr);
      tal_cmd.cmd_details.tal_pro_cmd.data_ptr = NULL;
    }
    return GSTK_SUCCESS;
  case GSTK_TAL_ROUTE_TO_REMOTE:
    gstk_tal_expected_num_tr = 1;
    if(gstk_tal_client_cb_list[GSTK_TAL_REMOTE_CLIENT] != NULL)
    {
      (void)gstk_tal_client_cb_list[GSTK_TAL_REMOTE_CLIENT](&tal_cmd);
    }
    else
    {
      UIM_MSG_ERR_0("TAL: Remote GSTK client not registered");
    }
    if(tal_cmd.cmd_details.tal_pro_cmd.data_ptr)
    {
      gstk_free(tal_cmd.cmd_details.tal_pro_cmd.data_ptr);
      tal_cmd.cmd_details.tal_pro_cmd.data_ptr = NULL;
    }
    return GSTK_SUCCESS;
  case GSTK_TAL_ROUTE_TO_BOTH:
    gstk_tal_expected_num_tr = 2;
    if(gstk_tal_client_cb_list[GSTK_TAL_LOCAL_CLIENT] != NULL)
    {
      UIM_MSG_HIGH_0("TAL: Sending command to local");
      (void)gstk_tal_client_cb_list[GSTK_TAL_LOCAL_CLIENT](&tal_cmd);
    }
    else
    {
      UIM_MSG_ERR_0("TAL: Local GSTK client not registered");
    }
    if(gstk_tal_client_cb_list[GSTK_TAL_REMOTE_CLIENT] != NULL)
    {
      UIM_MSG_HIGH_0("TAL: Sending command to remote");
      if(type_of_command == GSTK_CMD_STK_SET_UP_EVENT_LIST)
      {
        /* Set up event list comamnd goes to both modems. If set up event list
           has any UI related events then both GSTK will try to send this command
           to Android. Ideally Android should expect a proactive command only from
           one modem and as per design all UI related command are going from the
           local modem. Hence we strip off UI related events from set up event list
           command before sending it to remote modem */
        /*
        Qualifer = 1 Byte
        Device Identities : 4 bytes (82 02 81 82)
        Device ID Tag 82
        Length 02
        Source Device ID = 81
        Destination Device ID = 82
        Set up Event List Tag = 99 or 19
        */
      /* Check if we have a valid Device Identity tag (0x82 or 0x02) */
        offset++; /* points to device ID tag */
        if(gstk_valid_tag(cmd_data_ptr[offset],
                           (tags_enum_type)GSTK_DEVICE_IDENTITY_TAG))
        {
          offset++; /* points to length */
          offset += cmd_data_ptr[offset]; /* length */
          offset++; /* points to set up event list tag */
          gstk_tal_remove_ui_setup_events_for_remote(&tal_cmd, offset );
        }
        else
        {
          /* Not a proactive command tag, ignore the command */
          UIM_MSG_HIGH_2("TAL: Invalid tag 0x%x expect device id tag at offset 0x%x",
                         cmd_data_ptr[offset], offset);
        }
      }
      (void)gstk_tal_client_cb_list[GSTK_TAL_REMOTE_CLIENT](&tal_cmd);
    }
    else
    {
      UIM_MSG_ERR_0("TAL: Remote GSTK client not registered");
    }
    if(tal_cmd.cmd_details.tal_pro_cmd.data_ptr)
    {
      gstk_free(tal_cmd.cmd_details.tal_pro_cmd.data_ptr);
      tal_cmd.cmd_details.tal_pro_cmd.data_ptr = NULL;
    }
    return GSTK_SUCCESS;
  case GSTK_TAL_ROUTE_DROP:
  case GSTK_TAL_ROUTE_MAX:
  default:
    gstk_tal_expected_num_tr = 1;
    if(gstk_tal_client_cb_list[GSTK_TAL_LOCAL_CLIENT] != NULL)
    {
      (void)gstk_tal_client_cb_list[GSTK_TAL_LOCAL_CLIENT](&tal_cmd);
    }
    if(tal_cmd.cmd_details.tal_pro_cmd.data_ptr)
    {
      gstk_free(tal_cmd.cmd_details.tal_pro_cmd.data_ptr);
      tal_cmd.cmd_details.tal_pro_cmd.data_ptr = NULL;
    }
    return GSTK_SUCCESS;
  }
} /*gstk_tal_send_proactive_cmd*/


/*===========================================================================
  FUNCTION GSTK_TAL_WMS_MSG_EVT_CB()

  DESCRIPTION
    Callback function called by WMS when WMS has an event to return

  PARAMETERS
    msg_event      : event
    msg_event_info : event info struct
    shared         : if this event is shared by all WMS clients

  RETURN VALUE
    None

  DEPENDENCIES
    Expects a sucessful registration with WMS using wms_client_reg_msg_cb()

  SIDE EFFECTS
    None
===========================================================================*/
void gstk_tal_wms_msg_evt_cb
(
  wms_msg_event_e_type       event,
  wms_msg_event_info_s_type *info_ptr,
  boolean                   *shared
)
{
  UIM_MSG_HIGH_1("TAL: gstk_tal_wms_msg_evt_cb received event (%d)", event);
  /* check for NULL event info pointer */
  if (NULL == info_ptr)
  {
    UIM_MSG_ERR_0("TAL: Null info_ptr");
    return;
  }
  /* check for valid wms message event */
  if (WMS_MSG_EVENT_MAX <= event)
  {
    UIM_MSG_ERR_1("TAL: Invalid event (%d)", event);
    return;
  }
  switch(event)
  {
    case WMS_MSG_EVENT_TRANSPORT_REG:
      UIM_MSG_HIGH_2("TAL: WMS TRANSPT REG EVT is_reg = 0x%x, transpt_type=0x%x",
                     info_ptr->transport_reg_info.is_registered,
                     info_ptr->transport_reg_info.transport_type);
      if((info_ptr->transport_reg_info.is_registered) &&
         (info_ptr->transport_reg_info.transport_type ==
          WMS_MSG_TRANSPORT_TYPE_SMS_OVER_IMS))
      {
        gstk_tal_global.ims_subs_avail = TRUE;
      }
      else
      {
        gstk_tal_global.ims_subs_avail = FALSE;
      }
      break;
    default:
      break;
  }
  UIM_MSG_HIGH_1("TAL: IMS Subscription Avail : %d ",
                 gstk_tal_global.ims_subs_avail);
} /* gstk_tal_wms_msg_evt_cb */

/*===========================================================================
FUNCTION:   gstk_tal_delay_fetch

DESCRIPTION:
  This TAL function sets a timer to delay fetch for the first time on Fusion

PARAMETERS:
  None

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
void gstk_tal_delay_fetch(unsigned long dummy)
{
  UIM_MSG_HIGH_0("TAL: tal_delay_fetch_timer expiry");
  tal_is_fetch_ok = TRUE;
  (void)dummy;
} /*gstk_tal_delay_fetch*/

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
)
{
#ifdef FEATURE_UIM_DUAL_MODEM_SGLTE
  switch(srv_domain)
  {
    case SYS_SRV_DOMAIN_CS_ONLY:
    case SYS_SRV_DOMAIN_CS_PS:
      if(sys_mode == SYS_SYS_MODE_GSM)
      {
        gstk_tal_global.ss_info[GSTK_TAL_LOCAL_CLIENT].cs_sys =
          GSTK_TAL_SYS_GSM;
      }
      else if((sys_mode == SYS_SYS_MODE_WCDMA) || (sys_mode == SYS_SYS_MODE_TDS))
      {
        gstk_tal_global.ss_info[GSTK_TAL_LOCAL_CLIENT].cs_sys =
          GSTK_TAL_SYS_WCDMA;
      }
      else
      {
        gstk_tal_global.ss_info[GSTK_TAL_LOCAL_CLIENT].cs_sys =
          GSTK_TAL_SYS_NONE;
      }
      break;

    case SYS_SRV_DOMAIN_NO_SRV:
    case SYS_SRV_DOMAIN_PS_ONLY:
    case SYS_SRV_DOMAIN_CAMPED:
    default:
      gstk_tal_global.ss_info[GSTK_TAL_LOCAL_CLIENT].cs_sys =
        GSTK_TAL_SYS_NONE;
      break;
  }

  UIM_MSG_HIGH_3("TAL: Received CM SS evt,mode 0x%x, domain 0x%x, TAL CS sys set to 0x%x",
                 sys_mode,
                 srv_domain,
                 gstk_tal_global.ss_info[GSTK_TAL_LOCAL_CLIENT].cs_sys);
#else
  (void)sys_mode;
  (void)srv_domain;
#endif /* FEATURE_UIM_DUAL_MODEM_SGLTE */
} /* gstk_tal_update_cs_serving_sys_info */

/*===========================================================================
FUNCTION gstk_tal_ds_sys_evt_cb

DESCRIPTION
  Callback registered with DS which will be invoked when DS serving system
  changes

PARAMETERS
  Technology (3GPP or 3GPP2) for which the event is being sent
  Event name
  Event information
  User data being returned to client

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
static void  gstk_tal_ds_sys_evt_cb
(
  ds_sys_tech_enum_type                          tech_type,
  ds_sys_event_enum_type                         event_name,
  void                                         * event_info_ptr,
  void                                         * user_data_ptr
)
{
  ds_sys_system_status_type    *ds_event_info_ptr = NULL;

  (void)user_data_ptr;

  if(event_info_ptr == NULL)
  {
    UIM_MSG_ERR_0("TAL: Event info ptr is NULL");
    return;
  }

  ds_event_info_ptr = (ds_sys_system_status_type *)event_info_ptr;

  UIM_MSG_HIGH_1("TAL: Received DS SS evt for network 0x%x",
                 ds_event_info_ptr->preferred_network);

  if(ds_event_info_ptr->preferred_network == DS_SYS_NETWORK_3GPP)
  {
    switch(ds_event_info_ptr->network_info[DS_SYS_NETWORK_3GPP].rat_mask)
    {
      case DS_SYS_RAT_3GPP_GPRS:
      case DS_SYS_RAT_3GPP_EDGE:
        gstk_tal_global.ss_info[GSTK_TAL_LOCAL_CLIENT].ps_sys =
          GSTK_TAL_SYS_GSM;
        break;
      case DS_SYS_RAT_3GPP_WCDMA:
      case DS_SYS_RAT_3GPP_HSDPA:
      case DS_SYS_RAT_3GPP_HSUPA:
      case DS_SYS_RAT_3GPP_HSDPAPLUS:
      case DS_SYS_RAT_3GPP_DC_HSDPAPLUS:
      case DS_SYS_RAT_3GPP_64_QAM:
      case DS_SYS_RAT_3GPP_TDSCDMA:
        gstk_tal_global.ss_info[GSTK_TAL_LOCAL_CLIENT].ps_sys =
          GSTK_TAL_SYS_WCDMA;
        break;
      case DS_SYS_RAT_3GPP_LTE:
        gstk_tal_global.ss_info[GSTK_TAL_LOCAL_CLIENT].ps_sys =
          GSTK_TAL_SYS_LTE;
        break;
      case DS_SYS_RAT_3GPP_NULL_BEARER:
        gstk_tal_global.ss_info[GSTK_TAL_LOCAL_CLIENT].ps_sys =
          GSTK_TAL_SYS_NONE;
      default:
        break;
    }

    UIM_MSG_HIGH_2("TAL: Received event for 0x%x, setting TAL ps sys to 0x%x",
                   ds_event_info_ptr->network_info[DS_SYS_NETWORK_3GPP].rat_mask,
                   gstk_tal_global.ss_info[GSTK_TAL_LOCAL_CLIENT].ps_sys);
  }
} /* gstk_tal_ds_sys_evt_cb */

/*===========================================================================
FUNCTION gstk_tal_ds_sys_init

DESCRIPTION
  Function for having GSTK TAL register with DS SYS.

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
static gstk_status_enum_type gstk_tal_ds_sys_init(void)
{
  int16  dss_ret_val = 0;
  int16  dss_errno   = 0;

  UIM_MSG_HIGH_0("TAL: gstk_tal_ds_sys_init");

  dss_ret_val =  ds_sys_event_reg(DS_SYS_TECH_ALL,
                                  DS_SYS_EVENT_SYSTEM_STATUS_CHANGE,
                                  &gstk_tal_ds_sys_evt_cb,
                                  NULL,
                                  &dss_errno);
  if(0 > dss_ret_val)
  {
    UIM_MSG_ERR_2("TAL: DS SYS event reg failed, ret_val=0x%x, err 0x%x",
                  dss_ret_val, dss_errno);
    return GSTK_ERROR;
  }
  else
  {
    UIM_MSG_HIGH_0("TAL: DS SYS event reg successful");
    return GSTK_SUCCESS;
  }
} /* gstk_tal_ds_sys_init */

/*===========================================================================
FUNCTION gstk_tal_notify_remote_client_of_recovery

DESCRIPTION
  Function handles recovery indication from drivers to ensure remote
  modem gets the notification to end any active proacitve session

PARAMETERS
  Slot id

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
)
{
  gstk_tal_cmd_details_type tal_cmd;
  uint8                     recovery_cmd[11];

  memset(&tal_cmd, 0x00, sizeof(gstk_tal_cmd_details_type));
  memset(&recovery_cmd, 0x00, sizeof(recovery_cmd));

  /* Construct a fake proactive command with end session
     notification to let the remote modem know that it needs
     to end any pending proactive commands */
  recovery_cmd[0] = (uint8)GSTK_PROACTIVE_SIM_COMMAND_TAG;
  recovery_cmd[1] = 9;
  recovery_cmd[2] = (uint8)GSTK_COMMAND_DETAILS_TAG;
  recovery_cmd[3] = (uint8)GSTK_COMMAND_DETAILS_LEN;
  recovery_cmd[4] = 0;
  recovery_cmd[5] = (uint8)GSTK_CMD_STK_END_OF_PROACTIVE_SES;
  recovery_cmd[6] = (uint8)GSTK_GENERATED_END_SES;
  recovery_cmd[7] = (uint8)GSTK_DEVICE_IDENTITY_TAG;
  recovery_cmd[8] = (uint8)GSTK_DEVICE_IDENTITY_LEN;
  recovery_cmd[9] = (uint8)GSTK_UICC_SIM_DEVICE;
  recovery_cmd[10]= (uint8)GSTK_ME_DEVICE;

  tal_cmd.cmd_group = GSTK_PROACTIVE_CMD;
  tal_cmd.cmd_details.tal_pro_cmd.data_length = sizeof(recovery_cmd);
  tal_cmd.cmd_details.tal_pro_cmd.slot = slot_id;
  tal_cmd.cmd_details.tal_pro_cmd.data_ptr =  recovery_cmd;

  if(gstk_tal_client_cb_list[GSTK_TAL_REMOTE_CLIENT] != NULL)
  {
    (void)gstk_tal_client_cb_list[GSTK_TAL_REMOTE_CLIENT](&tal_cmd);
  }
  else
  {
    UIM_MSG_HIGH_0("TAL: Remote GSTK client not yet registered");
  }
} /* gstk_tal_notify_remote_client_of_recovery */

/*===========================================================================
FUNCTION gstk_tal_process_access_tech_priorities

DESCRIPTION
  Processes and stores the access technologies priorities based on EFS
  configuration. The order of the rules in tal_conf specifies the priority
  of access technology.

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
static void gstk_tal_process_access_tech_priorities(void)
{
  uint8                                 config_size     = 0;
  gstk_tal_cmd_config_type              *config_ptr     = NULL;
  boolean                               use_default     = FALSE;
  uint8                                 i               = 0;
  gstk_tal_sys_type                     access_tech_val = 0;

  UIM_MSG_HIGH_0("TAL: gstk_tal_process_access_tech_priorities");

  /* Read the access technologies from the pro_cmd_route global table */
  config_size = gstk_tal_global.pro_cmd_route[GSTK_TAL_CMD_STK_ACCESS_TECH_ROUTE].cmd_config_size;

  UIM_MSG_HIGH_1("TAL: Config size is %d", config_size);

  if(config_size == 0 || config_size > GSTK_TAL_MAX_ACCESS_TECH_TYPE)
  {
    UIM_MSG_HIGH_1("TAL: Use default as config_size is bad %d", config_size);
    use_default = TRUE;
    goto GSTK_TAL_PROCESS_ACCESS_TECH_PRIORITIES_RETURN;
  }

  for(i=0; i<config_size; i++)
  {
    config_ptr = gstk_tal_global.pro_cmd_route[GSTK_TAL_CMD_STK_ACCESS_TECH_ROUTE].cmd_add_config;
    if(config_ptr == NULL)
    {
      UIM_MSG_ERR_0("TAL: Null pointer in TAL config is unexpected");
      use_default = TRUE;
      goto GSTK_TAL_PROCESS_ACCESS_TECH_PRIORITIES_RETURN;
    }
    access_tech_val = (gstk_tal_sys_type)config_ptr[i].cmd_opt_val;

    UIM_MSG_HIGH_1("TAL: Access Tech Val in config is %d", access_tech_val);

    if(access_tech_val < GSTK_TAL_SYS_1X || access_tech_val > GSTK_TAL_SYS_LTE)
    {
      UIM_MSG_ERR_0("TAL: Invalid access tech value in TAL config");
      use_default = TRUE;
      goto GSTK_TAL_PROCESS_ACCESS_TECH_PRIORITIES_RETURN;
    }

    /* Convert access tech values GSTK TAL to GSTK format */
    switch(access_tech_val)
    {
    case GSTK_TAL_SYS_1X:
      gstk_tal_global.loc_status_table[i].access_tech = GSTK_TAL_ACCESS_TECH_CDMA;
      break;
    case GSTK_TAL_SYS_HDR:
      gstk_tal_global.loc_status_table[i].access_tech = GSTK_TAL_ACCESS_TECH_HRPD;
      break;
    case GSTK_TAL_SYS_GSM:
      gstk_tal_global.loc_status_table[i].access_tech = GSTK_TAL_ACCESS_TECH_GSM;
      break;
    case GSTK_TAL_SYS_WCDMA:
      gstk_tal_global.loc_status_table[i].access_tech = GSTK_TAL_ACCESS_TECH_UTRAN;
      break;
    case GSTK_TAL_SYS_LTE:
      gstk_tal_global.loc_status_table[i].access_tech = GSTK_TAL_ACCESS_TECH_LTE;
      break;
    default:
      UIM_MSG_ERR_0("TAL: Invalid access tech config");
      use_default = TRUE;
      goto GSTK_TAL_PROCESS_ACCESS_TECH_PRIORITIES_RETURN;
    }

    /* Convert the routes from route to client format */
    if(config_ptr[i].cmd_route == GSTK_TAL_ROUTE_TO_LOCAL)
    {
      gstk_tal_global.loc_status_table[i].client_type = GSTK_TAL_LOCAL_CLIENT;
    }
    else if(config_ptr[i].cmd_route == GSTK_TAL_ROUTE_TO_REMOTE)
    {
      gstk_tal_global.loc_status_table[i].client_type = GSTK_TAL_REMOTE_CLIENT;
    }
    UIM_MSG_HIGH_3("TAL: Priority for access tech %d with route %d is %d",
                   access_tech_val, config_ptr[i].cmd_route, i);
  }

GSTK_TAL_PROCESS_ACCESS_TECH_PRIORITIES_RETURN:
  if(use_default == TRUE)
  {
    gstk_tal_global.loc_status_table[0].access_tech = GSTK_TAL_ACCESS_TECH_LTE;
    gstk_tal_global.loc_status_table[0].client_type = GSTK_TAL_LOCAL_CLIENT;
    gstk_tal_global.loc_status_table[1].access_tech = GSTK_TAL_ACCESS_TECH_UTRAN;
    gstk_tal_global.loc_status_table[1].client_type = GSTK_TAL_LOCAL_CLIENT;
    gstk_tal_global.loc_status_table[2].access_tech = GSTK_TAL_ACCESS_TECH_GSM;
    gstk_tal_global.loc_status_table[2].client_type = GSTK_TAL_LOCAL_CLIENT;
#ifdef FEATURE_UIM_DUAL_MODEM_SGLTE
    gstk_tal_global.loc_status_table[3].access_tech = GSTK_TAL_ACCESS_TECH_GSM;
    gstk_tal_global.loc_status_table[3].client_type = GSTK_TAL_REMOTE_CLIENT;
#else
    gstk_tal_global.loc_status_table[3].access_tech = GSTK_TAL_ACCESS_TECH_HRPD;
    gstk_tal_global.loc_status_table[3].client_type = GSTK_TAL_LOCAL_CLIENT;
    gstk_tal_global.loc_status_table[4].access_tech = GSTK_TAL_ACCESS_TECH_CDMA;
    gstk_tal_global.loc_status_table[4].client_type = GSTK_TAL_LOCAL_CLIENT;
#endif /* FEATURE_UIM_DUAL_MODEM_SGLTE */
  }
} /* gstk_tal_process_access_tech_priorities */

/*===========================================================================
FUNCTION gstk_tal_read_proactive_cmd_config

DESCRIPTION
  Function for TAL configuration. Reads a file from the EFS that contains
  details on how to route the proactive commands

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
static void gstk_tal_read_proactive_cmd_config(void)
{
  uint32                   cmd_type       = 0;
  uint32                   cmd_condition  = 0;
  uint32                   cmd_opt_val    = 0;
  uint32                   cmd_route      = 0;
  uint8                    i              = 0;
  uint8                    j              = 0;
  uint8                    multiple_cond[GSTK_TAL_MAX_STK_CMD_TYPE];
  uint32                   cmd_mask       = 0;
  gstk_status_enum_type    gstk_status    = GSTK_SUCCESS;
  gstk_nv_items_data_type  nv_data;

#ifdef FEATURE_UIM_DUAL_MODEM_SGLTE
/* SGLTE default TAL configuration
  0x02 0x00 0x00 0x03 : MORE TIME, always to BOTH
  0x05 0x00 0x00 0x03 : SETUP EVENT LIST, always to BOTH
  0x81 0x00 0x00 0x03 : END PROACTIVE SESSION, always to BOTH
  0x40 0x00 0x00 0x04 : OPEN CHANNEL, always to SYSTEM BEST
  0x11 0x00 0x00 0x04 : SEND SS, always to SYSTEM BEST
  0x12 0x00 0x00 0x04 : SEND USSD, always to SYSTEM BEST
  0x14 0x00 0x00 0x04 : SEND DTMF, always to SYSTEM BEST
  0x41 0x00 0x00 0x05 : CLOSE CHANNEL, always to BIP route
  0x42 0x00 0x00 0x05 : RECEIVE DATA, always to BIP route
  0x43 0x00 0x00 0x05 : SEND DATA, always to BIP route
  0x44 0x00 0x00 0x05 : GET CHANNEL STATUS, always to BIP route
  0x26 0x01 0x03 0x01 : PROVIDE LOCAL INFO, Date, time and time zone, LOCAL
  0x26 0x01 0x00 0x04 : PROVIDE LOCAL INFO, Location Information, SYSTEM BEST
  0x26 0x01 0x06 0x04 : PROVIDE LOCAL INFO, Access Technology, SYSTEM BEST
  0x26 0x00 0x00 0x01 : PROVIDE LOCAL INFO, all others, LOCAL
  0x10 0x00 0x00 0x04 : SETUP CALL, always to SYSTEM BEST
  0x13 0x03 0x00 0x01 : SEND SMS, if IMS supports SMS, LOCAL
  0x13 0x00 0x00 0x04 : SEND SMS, all other cases, SYSTEM BEST
  0x36 0x01 0x04 0x01 : ACCESS TECH LTE is supported by LOCAL
  0x36 0x01 0x03 0x01 : ACCESS TECH WCDMA is supported by LOCAL
  0x36 0x01 0x02 0x01 : ACCESS TECH GSM is supported by LOCAL
  0x36 0x01 0x02 0x02 : ACCESS TECH GSM is supported by REMOTE
  0x00 0x00 0x00 0x01 : All other commands, always to LOCAL */
  uint32                   sglte_default_tal_conf[GSTK_TAL_MAX_CMD_CONFIG] =
    {
      0x02000003, 0x05000003, 0x81000003, 0x40000004,
      0x11000004, 0x12000004, 0x14000004, 0x41000005,
      0x42000005, 0x43000005, 0x44000005, 0x26010301,
      0x26010004, 0x26010604, 0x26000001, 0x10000004,
      0x13030001, 0x13000004, 0x36010401, 0x36010301,
      0x36010201, 0x36010202, 0x00000001, 0x00000000,
      0x00000000, 0x00000000, 0x00000000, 0x00000000,
      0x00000000, 0x00000000, 0x00000000, 0x00000000
    };
#endif /* FEATURE_UIM_DUAL_MODEM_SGLTE */

  UIM_MSG_HIGH_0("TAL: gstk_tal_read_proactive_cmd_config");

  memset(gstk_tal_global.pro_cmd_route, 0x00,
         sizeof(gstk_tal_global.pro_cmd_route));
  memset(multiple_cond, 0x00, sizeof(multiple_cond));

  /* Read the CAT version */
  gstk_status = gstk_nv_access_read(GSTK_NV_ME_TAL_CONF,
                                    GSTK_SLOT_ID_MAX,
                                    &nv_data);

#ifdef FEATURE_UIM_DUAL_MODEM_SGLTE
  if (gstk_status != GSTK_SUCCESS)
  {
    (void)memscpy(nv_data.tal_conf, sizeof(nv_data.tal_conf),
                  sglte_default_tal_conf, sizeof(sglte_default_tal_conf));
    UIM_MSG_HIGH_0("TAL: tal_conf absent. Using default SGLTE config");
  }
#else
  UIM_MSG_HIGH_1("TAL: cfg read TAL config returns 0x%x", gstk_status);
#endif /* FEATURE_UIM_DUAL_MODEM_SGLTE */


  for(i = 0; (i < GSTK_TAL_MAX_CMD_CONFIG/4) && (i < GSTK_TAL_MAX_CMD_CONFIG); i++)
  {
    cmd_mask        = nv_data.tal_conf[i];
    cmd_type        = (cmd_mask & GSTK_TAL_PROACTIVE_CMD_TYPE_MASK) >> 24;
    cmd_condition   = (cmd_mask & GSTK_TAL_PROACTIVE_CMD_CONDITION_MASK) >> 16;
    cmd_opt_val     = (cmd_mask & GSTK_TAL_PROACTIVE_CMD_OPT_VAL_MASK) >> 8;
    cmd_route       = cmd_mask & GSTK_TAL_PROACTIVE_CMD_ROUTE_MASK;

    if((cmd_type == 0) && (cmd_condition == 0)
       && (cmd_opt_val == 0) && (cmd_route == 0))
    {
      break;
    }

    if(cmd_type >= GSTK_TAL_MAX_STK_CMD_TYPE)
    {
      UIM_MSG_ERR_1("TAL: Invalid cmd_type %d", cmd_type);
      continue;
    }
    multiple_cond[cmd_type]++;
    UIM_MSG_HIGH_2("TAL: Cond count of cmd 0x%x is 0x%x",
                   cmd_type, multiple_cond[cmd_type]);
  }

  for(i = 0; (i < GSTK_TAL_MAX_CMD_CONFIG/4) && (i < GSTK_TAL_MAX_CMD_CONFIG); i++)
  {
    cmd_mask        = nv_data.tal_conf[i];
    cmd_type        = (cmd_mask & GSTK_TAL_PROACTIVE_CMD_TYPE_MASK) >> 24;
    cmd_condition   = (cmd_mask & GSTK_TAL_PROACTIVE_CMD_CONDITION_MASK) >> 16;
    cmd_opt_val     = (cmd_mask & GSTK_TAL_PROACTIVE_CMD_OPT_VAL_MASK) >> 8;
    cmd_route       = cmd_mask & GSTK_TAL_PROACTIVE_CMD_ROUTE_MASK;

    UIM_MSG_HIGH_1("TAL: Processing cmd type 0x%x", cmd_type);
    UIM_MSG_HIGH_3("TAL: Condition = 0x%x, Option = 0x%x, Route = 0x%x",
                   cmd_condition, cmd_opt_val, cmd_route);

    if((cmd_type == 0) && (cmd_condition == 0)
       && (cmd_opt_val == 0) && (cmd_route == 0))
    {
      return;
    }

    if(cmd_type >= GSTK_TAL_MAX_STK_CMD_TYPE)
    {
      UIM_MSG_ERR_1("TAL: Invalid cmd_type %d", cmd_type);
      continue;
    }

    if(multiple_cond[cmd_type] < 2)
    {
      UIM_MSG_HIGH_0("TAL: Only one condition for cmd");
      gstk_tal_global.pro_cmd_route[cmd_type].cmd_route = (gstk_tal_route_type)cmd_route;
      gstk_tal_global.pro_cmd_route[cmd_type].cmd_config_size = 0;
      gstk_tal_global.pro_cmd_route[cmd_type].cmd_add_config = NULL;
      continue;
    }

    gstk_tal_global.pro_cmd_route[cmd_type].cmd_config_size =
      multiple_cond[cmd_type];

    if(gstk_tal_global.pro_cmd_route[cmd_type].cmd_add_config == NULL)
    {
      gstk_tal_global.pro_cmd_route[cmd_type].cmd_add_config =
        (gstk_tal_cmd_config_type *)GSTK_CALLOC(
          sizeof(gstk_tal_cmd_config_type) * multiple_cond[cmd_type]);
      if(gstk_tal_global.pro_cmd_route[cmd_type].cmd_add_config == NULL)
      {
        return;
      }
      memset(gstk_tal_global.pro_cmd_route[cmd_type].cmd_add_config,
             0xFF, //cmd_type will be compared with 0xFFFFFFFF
             sizeof(gstk_tal_cmd_config_type) * multiple_cond[cmd_type]);
    }
    for(j = 0; j < gstk_tal_global.pro_cmd_route[cmd_type].cmd_config_size; j++)
    {
      if(gstk_tal_global.pro_cmd_route[cmd_type].cmd_add_config[j].cmd_type == 0xFFFFFFFF)
      {
        UIM_MSG_HIGH_1("TAL: Found empty add config index 0x%x", j);
        break;
      }
    }
    if(j == gstk_tal_global.pro_cmd_route[cmd_type].cmd_config_size)
    {
      UIM_MSG_ERR_0("TAL: No empty slot for config! ");
      return;
    }
    gstk_tal_global.pro_cmd_route[cmd_type].cmd_add_config[j].cmd_type = cmd_type;
    gstk_tal_global.pro_cmd_route[cmd_type].cmd_add_config[j].cmd_cond = cmd_condition;
    gstk_tal_global.pro_cmd_route[cmd_type].cmd_add_config[j].cmd_opt_val = cmd_opt_val;
    gstk_tal_global.pro_cmd_route[cmd_type].cmd_add_config[j].cmd_route = cmd_route;
  }

  /* Update default route for commands as long as there is one condition */
  /* Check if multiple conditions for default route */
  if(gstk_tal_global.pro_cmd_route[GSTK_TAL_DEFAULT_CMD_TYPE].cmd_config_size > 1)
  {
    UIM_MSG_HIGH_0("TAL: Multiple conditions for default route");
    return;
  }

  /* If only one condition update defualt route for all commands */
  for(i = 1; i<GSTK_TAL_MAX_CMD_CONFIG; i++)
  {
    if(gstk_tal_global.pro_cmd_route[i].cmd_route == GSTK_TAL_ROUTE_INVALID)
    {
      UIM_MSG_HIGH_2("TAL: Setting default route for cmd type 0x%x with route 0x%x",
                     i,
                     gstk_tal_global.pro_cmd_route[GSTK_TAL_DEFAULT_CMD_TYPE].cmd_route);
      gstk_tal_global.pro_cmd_route[i].cmd_route =
        gstk_tal_global.pro_cmd_route[GSTK_TAL_DEFAULT_CMD_TYPE].cmd_route;
    }
  }
} /* gstk_tal_read_proactive_cmd_config */

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
gstk_status_enum_type gstk_tal_init(void)
{
  uint8                                 i = 0;

  UIM_MSG_HIGH_0("TAL: gstk_init_tal()");

  /* Memset static globals */
  memset((void *)gstk_tal_terminal_profile_cache, 0x00, GSTK_TERMINAL_PROFILE_SIZE);

  /* Init timer for delaying the fetch of proactive commands */
  rex_def_timer_ex(&tal_delay_fetch_timer,
                   gstk_tal_delay_fetch,
                   0);

  gstk_tal_global.bip_route = GSTK_TAL_ROUTE_MAX;
  gstk_tal_global.last_loc_status_sent = GSTK_TAL_ACCESS_TECH_NONE;

  gstk_tal_global.last_access_tech_sent.access_tech      = GSTK_TAL_ACCESS_TECH_NONE;
  gstk_tal_global.last_access_tech_sent.client           = GSTK_TAL_MAX_CLIENTS;

  gstk_tal_global.ls_triggered_access_tech.access_tech   = GSTK_TAL_ACCESS_TECH_NONE;
  gstk_tal_global.ls_triggered_access_tech.client        = GSTK_TAL_MAX_CLIENTS;

  for(i = 0; i < GSTK_TAL_MAX_CLIENTS; i++)
  {
    gstk_tal_global.ss_info[i].cs_sys = GSTK_TAL_SYS_NONE;
    gstk_tal_global.ss_info[i].ps_sys = GSTK_TAL_SYS_NONE;
  }

  for(i = 0; i < GSTK_TAL_MAX_ACCESS_TECH_TYPE; i++)
  {
    gstk_tal_global.loc_status_table[i].most_recent = FALSE;
    gstk_tal_global.loc_status_table[i].resend = FALSE;
    gstk_tal_global.loc_status_table[i].client_type = GSTK_TAL_MAX_CLIENTS;
    gstk_tal_global.loc_status_table[i].data_ptr = NULL;
    gstk_tal_global.loc_status_table[i].access_tech = GSTK_TAL_ACCESS_TECH_NONE;
    gstk_tal_global.loc_status_table[i].num_bytes = 0;
  }
  memset(gstk_tal_global.gstk_tal_location_status, 0x00,
         sizeof(gstk_tal_global.gstk_tal_location_status));

  gstk_tal_read_proactive_cmd_config();
  gstk_tal_process_access_tech_priorities();
  UIM_MSG_HIGH_0("TAL: gstk_init_tal() - complete");
  return GSTK_SUCCESS;
} /*gstk_tal_init*/

/*===========================================================================
FUNCTION gstk_tal_external_init

DESCRIPTION
  Function for TAL registration with external services - DS, WMS stc.

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
gstk_status_enum_type gstk_tal_external_init(void)
{
  UIM_MSG_HIGH_0("TAL: gstk_init_external_tal()");

  if(GSTK_SUCCESS == gstk_tal_ds_sys_init())
  {
    UIM_MSG_HIGH_0("TAL: Registered with DS SYS");
  }
  /*-----------------------------------------------------------------------
      Register for client ID for WMS service
    -----------------------------------------------------------------------*/
  if (WMS_CLIENT_ERR_NONE == wms_client_init (WMS_CLIENT_FLOATING2,
                                              &gstk_tal_global.gstk_tal_wms_cid))
  {

   /*-----------------------------------------------------------------------
      Register for necessary events with the WMS service
    -----------------------------------------------------------------------*/
    if (WMS_CLIENT_ERR_NONE !=
        wms_client_reg_msg_cb (gstk_tal_global.gstk_tal_wms_cid,
                               gstk_tal_wms_msg_evt_cb))
    {
      UIM_MSG_ERR_0("TAL: Error in wms_client_reg_msg_cb");
    }

    if (WMS_CLIENT_ERR_NONE != wms_client_activate(gstk_tal_global.gstk_tal_wms_cid))
    {
      UIM_MSG_ERR_0("TAL: Error in wms_client_activate");
    }
  }
  else
  {
    UIM_MSG_ERR_0("TAL: Error in wms_client_init");
  }

  return GSTK_SUCCESS;
} /*gstk_tal_external_init*/

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
)
{
  gstk_status_enum_type gstk_status = GSTK_ERROR;

  if(!tal_cmd_ptr)
  {
    UIM_MSG_ERR_0("TAL: Null input param tal_cmd_ptr");
    return GSTK_BAD_PARAM;
  }

  if(client_type != GSTK_TAL_LOCAL_CLIENT &&
     client_type != GSTK_TAL_REMOTE_CLIENT)
  {
    UIM_MSG_ERR_1("TAL: Invalid client type param 0x%x", client_type);
    return GSTK_BAD_PARAM;
  }

  UIM_MSG_HIGH_1("TAL: gstk_tal_command_main - processing cmd 0x%x",
                 tal_cmd_ptr->cmd_type);

  switch (tal_cmd_ptr->cmd_type)
  {
  case GSTK_TERMINAL_PROFILE_CMD:
    gstk_status = gstk_tal_send_tp_to_uim(tal_cmd_ptr->data_ptr,
                                          tal_cmd_ptr->num_bytes);
    break;

  case GSTK_TERMINAL_RSP:
    gstk_status = gstk_tal_send_tr_to_uim(tal_cmd_ptr);
    break;

  case GSTK_ENVELOPE_CMD:
    gstk_status = gstk_tal_send_envelope_to_uim(client_type, tal_cmd_ptr);
    break;

  case GSTK_CLIENT_REG_REQ_CMD:
    if(gstk_tal_cb)
    {
      gstk_tal_client_cb_list[client_type] = gstk_tal_cb;
      gstk_status = GSTK_SUCCESS;

      if (client_type == GSTK_TAL_REMOTE_CLIENT)
      {
        (void)rex_clr_timer(&tal_delay_fetch_timer);
        tal_is_fetch_ok = TRUE;
        UIM_MSG_HIGH_0("Remote client reg. TAL fetch timer cleared, fetch ok");
      }
    }
    else
    {
      UIM_MSG_ERR_0("TAL: Null GSTK TAL CB provided during registration");
      gstk_status = GSTK_ERROR;
    }
    break;

  case GSTK_GET_CURR_ACCESS_TECH_CMD:
    UIM_MSG_HIGH_2("TAL: Update for access tech 0x%x from client 0x%x",
                   tal_cmd_ptr->user_data, client_type);
    gstk_tal_global.current_active_rat[client_type] =
      (gstk_tal_access_tech_type)tal_cmd_ptr->user_data;
    gstk_status = GSTK_SUCCESS;
    break;

  case GSTK_SETUP_OTASP_CALL_CMD:
    gstk_status = gstk_tal_notify_otasp_status(tal_cmd_ptr->user_data);
    break;

  case GSTK_TAL_SEND_LS_CMD:
    UIM_MSG_HIGH_2("TAL: Update for Location Status 0x%x from client 0x%x",
                   tal_cmd_ptr->user_data, client_type);
    gstk_tal_global.gstk_tal_location_status[client_type] =
      (gstk_location_status_enum_type)tal_cmd_ptr->user_data;
    gstk_status = GSTK_SUCCESS;
    break;

  default:
    UIM_MSG_ERR_1("TAL: command not supported in TAL 0x%x",
                  tal_cmd_ptr->cmd_type);
    gstk_status = GSTK_ERROR;
    break;
  }
  return gstk_status;
} /* gstk_tal_command_main */

#endif /* FEATURE_GSTK_TAL */
