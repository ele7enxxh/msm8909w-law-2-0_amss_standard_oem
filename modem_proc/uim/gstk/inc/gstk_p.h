#ifndef GSTK_P_H
#define GSTK_P_H

/*===========================================================================


                         G S T K _ P    H E A D E R

GENERAL DESCRIPTION
  This is the GSTK internal header file to be used by UIM modules.  It
  contains data types and functions that other UIM modules can use to
  communicate with GSTK.

===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2010-2014 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved. QUALCOMM Technologies Proprietary.
Export of this technology or software is regulated by the
U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/gstk/inc/gstk_p.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/05/15   gs      Introduced SCWS status in command details ref table
10/08/14   vr      Removed FEATURE_ESTK guarding for GSTK_ESTK_CMD_SIG
10/07/14   vr      Fix SW1 SW2 TR REPORT failure for setup event list
07/11/14   shr     Add Support for NV Refresh
06/30/14   gm      change from tp_rpt_time to tp_rpt_timer
06/20/14   gm      Dynamically disable TP related to LTE
06/18/14   sw      Call/SMS control retry
04/21/14   sj      Changes to remove UIMDRV leaks due to gstk_calloc changes
04/07/14   shr     Update maximum Terminal Profile Size
04/03/14   dy      Replace gstk_malloc() with macro
03/31/14   dy      Fixed gstk_byte_offset_copy and gstk_memcpy
02/27/14   dy      Add default_apn, bip_apn_mapping_tbl, rat_priority NV item
01/10/14   kb      Added gstk_cleanup_globals function
12/06/13   bd      Changes in GSTK to support MM IDLE events for SGLTE+G
11/12/13   sw      Removed FEATURE_GSTK_TEST_AUTOMATION code
09/13/13   gm      Support for Recovery and hot-swap
05/01/13   hh      Make gstk_util_config_io() take uint32 for 4th parameter "len"
04/23/13   gm      Remove unused function
02/19/13   av      Merge mmgsi,gstk,qmiuim/cat conf files to mmgsdi.conf
02/14/13   tl      Move QMI CAT to GSTK context from QMI modem context
02/04/13   bd      Support for (e)HRPD access tech
11/29/12   bd      Clean up of redundant members from client table
11/13/12   bd      Support for Closed Subscriber Group feature
03/28/12   av      Replaced feature flag with NVITEM
03/16/12   yt      Modify reference to gstk_tcb
12/20/11   nb      GSDI CleanUp
10/18/11   av      Removed features that are always OFF
10/13/11   nb      Full recovery changes
08/23/11   nb      Async TP enhancement and moving GSTK to MMGSDI events
06/06/11   yt      Added P4 path template
05/03/11   xz      Fix CMI compile issue
04/11/11   nb      Changed TP size to 30 bytes
02/22/11   js      Renaming fusion features
01/27/11   nb      Fix for compilation warning
01/20/11   dd      Enable FEATURE_CAT_REL7
01/13/11   dd      Remove estk_util_config_io
01/07/10   bd      Removed GSTK_EXPECTED_NUM_OF_TP_DL
12/22/10   yt      Removed FEATURE_UIM_ZI_MEMORY_REDUCTION
11/24/10   yt      Initial revision

===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"
#include "nv.h"
#include "gstk_exp_v.h"
#include "modem_mem.h"

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/
#define GSTK_ESTK_CMD_SIG                     0x00400000

/* Terminal profile size */
#define GSTK_TERMINAL_PROFILE_SIZE 33 /* bytes */
#define GSTK_MAX_PROACTIVE_COMMAND 8
#define GSTK_MAX_ENVELOPE_COMMAND 9

/* client table */
/* LIMITATION: the max should be less than 32 */
#define GSTK_MAX_CLIENT 16

#ifdef FEATURE_MODEM_RCINIT
#include "rcinit.h"
#endif /* FEATURE_MODEM_RCINIT */

#ifndef FEATURE_UIM_USES_TASK_HANDLE
#define UIM_GSTK_TCB &gstk_tcb
#else
#define UIM_GSTK_TCB rcinit_lookup_rextask("gstk")
#endif /* !FEATURE_UIM_USES_TASK_HANDLE */

#define GSTK_ACCESS_TECH_CDMA_MASK  0x0001
#define GSTK_ACCESS_TECH_HRPD_MASK  0x0002
#define GSTK_ACCESS_TECH_EHRPD_MASK 0x0004
#define GSTK_ACCESS_TECH_GSM_MASK   0x0010
#define GSTK_ACCESS_TECH_UTRAN_MASK 0x0020
#define GSTK_ACCESS_TECH_LTE_MASK   0x0040

/* Mask of some combinations */
#define GSTK_ACCESS_TECH_3GPP2_MASK      (GSTK_ACCESS_TECH_CDMA_MASK | GSTK_ACCESS_TECH_HRPD_MASK | GSTK_ACCESS_TECH_EHRPD_MASK)
#define GSTK_ACCESS_TECH_3GPP_MASK       (GSTK_ACCESS_TECH_GSM_MASK | GSTK_ACCESS_TECH_LTE_MASK | GSTK_ACCESS_TECH_GSM_MASK)
#define GSTK_ACCESS_TECH_CDMA_HRPD_MASK  (GSTK_ACCESS_TECH_CDMA_MASK | GSTK_ACCESS_TECH_HRPD_MASK)
#define GSTK_ACCESS_TECH_CDMA_EHRPD_MASK (GSTK_ACCESS_TECH_CDMA_MASK | GSTK_ACCESS_TECH_EHRPD_MASK)

/* Maximum number of GSTK RATs supported in gstk_rat_enum_type */
#define GSTK_MAX_NUM_RATS                6

/* Number of TAL command configs */
#define GSTK_TAL_MAX_CMD_CONFIG          32

#define GSTK_CMD_REF_TABLE_INDEX(x) ((x) & 0xFF)
#define GSTK_ENV_CMD_TABLE_INDEX(x) ((x) & 0xFF)
#define GSTK_UNIQUE_REF_ID_MASK(x) ((x) & 0xFFFF0000)

#ifdef  FEATURE_ESTK
#define GSTK_IDLE_S_RECOVERY(x)                             \
        ((GSTK_IDLE_S == gstk_get_state((x)))           \
         &&                                             \
         (GSTK_RECOVERY_SUB_S == gstk_get_sub_state((x))))
#endif /* FEATURE_ESTK */

#ifdef FEATURE_UIM_TEST_FRAMEWORK
#error code not present
#else /* FEATURE_UIM_TEST_FRAMEWORK */
#define GSTK_CALLOC(x) modem_mem_calloc(1,(x),MODEM_MEM_CLIENT_UIM)
#endif /* FEATURE_UIM_TEST_FRAMEWORK */

/*--------------------------------------------------------------------------
                             Type Defs
--------------------------------------------------------------------------*/
/* gstk state */
typedef enum {
    GSTK_INVALID_S,
    GSTK_START_S,
    GSTK_TERM_PROFILE_WAIT_S,
    GSTK_MMGSDI_SIG_WAIT_S,
    GSTK_TERM_PROFILE_RSP_WAIT_S,
    GSTK_IDLE_S,
    GSTK_NO_SIM_S,
    GSTK_POWER_DOWN_S
}
gstk_state_enum_type;

/* gstk sub state */
typedef enum {
    GSTK_INVALID_SUB_S,
    GSTK_RECOVERY_SUB_S,
}
gstk_sub_state_enum_type;

/* GSTK RAT types */
typedef enum
{
  GSTK_RAT_INVALID = 0x0,
  GSTK_RAT_GSM     = 0x1,
  GSTK_RAT_UMTS    = 0x2,
  GSTK_RAT_LTE     = 0x3,
  GSTK_RAT_1X      = 0x4,
  GSTK_RAT_HRPD    = 0x5,
  GSTK_RAT_EHRPD   = 0x6
}gstk_rat_enum_type;

typedef struct
{
    boolean                           is_free;
    boolean                           send_setup_event_sw1_sw2;
    gstk_client_type                  client_type;
    gstk_slot_id_enum_type            sim_slot_id;
    uint32                            user_data;
    gstk_toolkit_evt_cb_func_type     client_cb;
    gstk_client_init_cb_func_type     client_init_cmd_cb;
}
gstk_client_table_type;

/* 8.7 */
typedef enum
{
  /* values based on standard */
   GSTK_NO_DEVICE        = 0x00,
   GSTK_KEYPAD_DEVICE    = 0x01,
   GSTK_DISPLAY_DEVICE   = 0x02,
   GSTK_EARPIECE_DEVICE  = 0x03,
   GSTK_CHANNEL_1_DEVICE = 0x21,
   GSTK_CHANNEL_2_DEVICE = 0x22,
   GSTK_CHANNEL_3_DEVICE = 0x23,
   GSTK_CHANNEL_4_DEVICE = 0x24,
   GSTK_CHANNEL_5_DEVICE = 0x25,
   GSTK_CHANNEL_6_DEVICE = 0x26,
   GSTK_CHANNEL_7_DEVICE = 0x27,
   GSTK_UICC_SIM_DEVICE  = 0x81,
   GSTK_ME_DEVICE        = 0x82,
   GSTK_NETWORK_DEVICE   = 0x83,
   GSTK_MAX_DEVICE       = 0xFF
}
device_identity_e;

typedef device_identity_e gstk_device_identity_type;

/* 8.6 */
typedef struct
{
    uint8             tag;                                 /* GSTK_COMMAND_DETAILS_TAG */
    uint8             length;                              /* GSTK_COMMAND_DETAILS_LEN */
    uint8             command_number;
    uint8             type_of_command;                     /* Type of Command     */
    uint8             qualifier; /*Q: need to return what was in the command before? */
}
gstk_command_details_tlv_type;

/* Store the Open Channel information */
typedef struct
{
   boolean is_scws_status;
}gstk_proc_cmd_open_channel;

/* Store the Proactive command information that can be used while sending the TR */
typedef union
{
   gstk_proc_cmd_open_channel open_channel;
}gstk_proc_cmd_info_type;

typedef struct
{
    gstk_slot_id_enum_type                  sim_slot_id;
    boolean                                 free_slot;
    gstk_command_details_tlv_type           command_details;
    gstk_device_identity_type               destination_dev_id;
    gstk_cmd_enum_type                      expected_cmd_rsp_type;
      /* only _CNF in the gstk_cmd_enum_type is valid in the xpected_cmd_rsp_type
      ** field.  This provides an additional checking mechanism between the
      ** proactive command and terminal response
      ** e.g., command_details.type_of_command = CMD_STK_DISPLAY_TEXT
      ** the expected_cmd_rsp_type = GSTK_DISPLAY_TEXT_CNF */
    rex_timer_type                          tp_rpt_timer;
    boolean                                 partial_comprehension;
    uint32                                  pending_responses;
    uint32                                  unique_ref_id;
    gstk_proc_cmd_info_type                 proc_cmd_info;
    gstk_generic_data_type                  raw_cmd;
    /* Fill this information in case of GSTK_WAKEUP_TYPE clients */
    gstk_cmd_from_card_type*                cmd_ptr;
}
gstk_cmd_details_ref_table_type;


/* Flag to indicate if current proactive session involves UI */
extern boolean gstk_is_curr_session_ui_related;

extern gstk_cmd_details_ref_table_type command_details_ref_table[GSTK_MAX_PROACTIVE_COMMAND];
extern gstk_cmd_details_ref_table_type  *command_details_ref_table_ptr;

/*===========================================================================

                        FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================
FUNCTION gstk_gsdi_continue_task

DESCRIPTION
  This function is used to set the global flag 'gstk_continue_task'to the
  input argument which in turn synchronizes the communication between GSTK
  and GSDI.

PARAMETER
  Input: val: Boolean value to set the flag to.

DEPENDENCIES
  None

RETURN VALUE
  None

COMMENTS
  When this flag is set to False GSTK will assumme that GSDI is processing a
  command on behalf of him and will process the new command when this one
  finishes.

SIDE EFFECTS
  GSTK will not process any new command instantenously

SEE ALSO
  None
===========================================================================*/
extern void gstk_gsdi_continue_task(
  boolean val
);

/*===========================================================================
FUNCTION   gstk_uim_fetch_allow

DESCRIPTION
  Library function called by UIM to check if UIM should do fetch or not
  depending on the following two condtions:
  1. whether the next command in gstk queue BIP Envelope or not
  2. whether there is a BIP Envelope that has been sent to UIM from gstk
     for which gstk has not received a response yet.

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  boolean

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern boolean gstk_uim_fetch_allow(gstk_slot_id_enum_type slot);

/*===========================================================================

FUNCTION gstk_get_state

DESCRIPTION
  This function returns the current gstk state

PARAMETERS
  None

DEPENDENCIES
  None

COMMENTS
  None

RETURN VALUE
  gstk_state_enum_type
    GSTK_START_S                   - gstk start state
    GSTK_TERM_PROFILE_WAIT_S       - waiting for client profile dl
    GSTK_MMGSDI_SIG_WAIT_S         - waiting for GSDI signal to do TP dl
    GSTK_TERM_PROFILE_RSP_WAIT_S   - waiting for tp dl response from UIM
    GSTK_IDLE_S                    - Idle state after successful tp dl
    GSTK_NO_SIM_S                  - Bad SIM state
    GSTK_POWER_DOWN_S              - Power down state

SIDE EFFECTS
  None
===========================================================================*/
gstk_state_enum_type gstk_get_state(gstk_slot_id_enum_type slot_id);

/*===========================================================================

FUNCTION gstk_get_sub_state

DESCRIPTION
  This function returns the current gstk sub state

PARAMETERS
  None

DEPENDENCIES
  None

COMMENTS
  None

RETURN VALUE
  gstk_sub_state_enum_type
    GSTK_INVALID_SUB_S
    GSTK_RECOVERY_SUB_S

SIDE EFFECTS
  None
===========================================================================*/
gstk_sub_state_enum_type gstk_get_sub_state(gstk_slot_id_enum_type slot_id);

#ifdef FEATURE_UIM_TEST_FRAMEWORK
#error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */

/*===========================================================================

FUNCTION gstk_memcpy

DESCRIPTION
  gstk_memcpy first checks if the size requested does not exceed the source
  size and that the size of the data to be copied does not exceed the
  max data size the destination buffer can accomodate before preceeding with
  the memcpy

PARAMETERS
  dest_ptr:   Pointer to copy data to.
  src_ptr: Pointer to copy data from.
  copy_size:     size of data to copy
  dest_max_size:      size of destination buffer
  src_max_size: size of source buffer, if 0 then no check on source size done

DEPENDENCIES
  None.

RETURN VALUE
  the number of bytes copied

SIDE EFFECTS
  data is copied to the destination buffer.  the copy may be truncated if the
  size of data to be copied exceeds the size of the source buffer or if the
  size of data to be copied exceeds the size of the destination buffer.
===========================================================================*/
size_t  gstk_memcpy(
  void *dest_ptr,
  const void *src_ptr,
  size_t copy_size,
  size_t dest_max_size,
  size_t src_max_size);

/*===========================================================================
FUNCTION GSTK_TASK_PUT_CMD_BUF

DESCRIPTION
  This function puts the item at the end of the queue and sets the GSTK cmd
  queue signal

DEPENDENCIES
  None

RETURN VALUE
  Does not return.
===========================================================================*/
extern void gstk_task_put_cmd_buf (
  gstk_task_cmd_type *cmd_ptr
);

/*===========================================================================

FUNCTION GSTK_FREE

DESCRIPTION
  The gstk_free frees the pointer from the regular tmc_heap or the tmc_heap_small
  based on the pointer address.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  the pointer is freed.
===========================================================================*/
void gstk_free(void * ptr);

/*===========================================================================
FUNCTION gstk_util_find_client_with_registered_event

DESCRIPTION
  This function goes through the gstk_client_table and the class event
  registration table to see which client has registered for the event

PARAMETERS
  client_id_ptr:[Input] Pointer to client id
  cmd_type:     [Input] type of command that GSTK is going to search for in the
                        registration table
  format_type:  [Input] Format type (RAW/GSTK)
  cli_func_type:[Input] Client functionality type (MODEM/UI/ALL)

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type
    GSTK_NO_CLIENT_REG_FOUND - no client registered for the command.
    GSTK_SUCCESS             - registered client for the specific cmd found
    GSTK_ERROR               - error in finding a registered client
    GSTK_NULL_INPUT_PARAM    - null pointer provided as input
    GSTK_BAD_PARAM           - Bad parameter passed in
    GSTK_INVALID_CLIENT_TYPE - Invalid client id found in class evt reg table

COMMENTS
  There should only be 1 client registers for each command event for a
  specific functionality (MODEM/UI/ALL) at any given time

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern gstk_status_enum_type gstk_util_find_client_with_registered_event(
    gstk_client_id_type                        *client_id_ptr,
    gstk_cmd_enum_type                          cmd_type,
    gstk_client_pro_cmd_data_format_enum_type   format_type,
    gstk_tk_functionality_enum_type             cli_func_type);


/*===========================================================================
FUNCTION gstk_util_call_client_evt_cb

DESCRIPTION
  Based on the type of the client, this function packs the right format of the
  arguments and invokes the event callback function registerd by the client.

PARAMETERS
  client_id:  [Input] The ID of the client whose event call back function will
                      be invoked.s
  message_ptr:[Input] The pointer to the message to be passed to the client

DEPENDENCIES
  None

RETURN VALUE
  Void
COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern void gstk_util_call_client_evt_cb(
  gstk_client_id_type      client_id,
  gstk_cmd_from_card_type *message_ptr
);

#ifdef FEATURE_GSTK_TAL
/*===========================================================================
FUNCTION gstk_tal_send_proactive_cmd

DESCRIPTION
  This function peeks into the proactive command and sends it to local
  or remote or both depending on type of command

PARAMETERS
  slot: [Input] Slot type
  data_length: [Input] Length of the data
  data: [Input] Pointer to the data buffer that contains the Proactive cmd

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
extern gstk_status_enum_type gstk_tal_send_proactive_cmd(
  uim_slot_type          slot,
  uint32                 data_length,
  const uint8            *data);
#endif /* FEATURE_GSTK_TAL */

/*============================================================================
  FUNCTION:      GSTK_GSDI_SET_FETCH_STATUS

  DESCRIPTION:
    API used by GSDI to set status for fetch allowed or not

  PARAMETERS:
    slot_id: [Input]  Slot
    status:  [Input]  FETCH Status
                      TRUE - FETCH Allowed
                      FALSE - FETCH Not Allowed

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    None

  SIDE EFFECTS:
    None

  SEE ALSO:
    None
========================================================================== */
void gstk_gsdi_set_fetch_status(
  gstk_slot_id_enum_type       slot_id,
  boolean                      status
);

/*===========================================================================
FUNCTION gstk_slot_send_terminal_profile_dl_command

DESCRIPTION
  This function is called by MMGSDI to indicate GSTK to perform TO download

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

===========================================================================*/
gstk_status_enum_type gstk_slot_send_terminal_profile_dl_command (
  gstk_slot_id_enum_type      slot,
  boolean                     ok_to_dl,
  uint32                      user_data,
  gstk_evt_cb_funct_type      gstk_tp_dl_cb
);

/*===========================================================================
FUNCTION gstk_send_recovery_cmd

DESCRIPTION
  This function is called by UIM to send the Recovery command to GSTK

DEPENDENCIES
  None

RETURN VALUE
  Void
===========================================================================*/
gstk_status_enum_type gstk_send_recovery_cmd(
  uim_slot_type                   slot,
  uimdrv_recovery_stage_enum_type stage
);

/*================================================================
FUNCTION  GSTK_SET_QMI_CAT_SIG

DESCRIPTION:
  Sets the GSTK_QMI_CAT_SIG to begin processing of QMI CAT.

DEPENDENCIES:
  gstk_task must be finished initializing.

RETURN VALUE:
  None
===============================================================*/
void gstk_set_qmi_cat_sig(
  void
);

/*===========================================================================
FUNCTION gstk_cleanup_globals

DESCRIPTION

  This function
  - Sends pending envelope response
  - Deletes global variables
  - Deletes GSTK queue
  - Deletes GSTK timers


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
void gstk_cleanup_globals ( void );

/*===========================================================================
FUNCTION gstk_util_find_modem_capability

DESCRIPTION
  This function find out the modem capability.

PARAMETERS
  capability [input]: RAT type for which capability is needed.

DEPENDENCIES
  None

RETURN VALUE
  boolean

===========================================================================*/
boolean gstk_util_find_modem_capability(gstk_modem_cap_enum_type capability);
#endif /* GSTK_P_H */

