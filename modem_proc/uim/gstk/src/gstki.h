#ifndef GSTKI_H
#define GSTKI_H
/*===========================================================================


    G S T K   A N D   I N T E R N A L   M O D U L E   I N T E R F A C E S


GENERAL DESCRIPTION
  This header file is the interface definitions between GSTK and the various
  QCT internal modules



FUNCTIONS
  gstk_send_proactive_cmd
    This function allows clients to put a proactive cmd request before parsing
    onto GSTK command queue

  gstk_util_release_card_cmd_memory
    This function frees all the allocated memory for the command pointer input.

  gstk_cmd
    This function allows clients to put the gstk_cmd onto the GSTK command
    queue.

  gstk_task_get_cmd_buf
    This function get the gstk_task_cmd_type from the GSTK free queue.

  gstk_task
    GSTK Main Task

  gstk_util_dereg_client_type
    This function dereg events for certain client type.  dereg_evt indicates
    the events to be removed from the client's list

  gstk_is_cell_broadcast_supported
    Utility function to check if the cell broadcast is supposed by the mobile

  gstk_test_automation_restart
    Called by test automation script via DIAG to soft restart GSTK

INITIALIZATION AND SEQUENCING REQUIREMENTS



===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2003-2011, 2014 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/gstk/src/gstki.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/11/14   shr     Add Support for NV Refresh
05/28/14   gm      Support of CSG Rel-11 feature
05/06/14   gs      GSTK Featurization enhancements and clean up
11/12/13   sw      Removed FEATURE_GSTK_TEST_AUTOMATION code
05/17/13   hn      Support for Alpha for Refresh
04/23/13   gm      Remove unused function
12/21/11   kk      More GSDI Cleanup
12/20/11   nb      GSDI Cleanup
02/01/11   nb      Fix for compilation error
01/27/11   nb      Fix for compilation warning
12/18/10   ssr     Dynamically allocate command buffer
11/30/10   yt      Including private GSTK header
10/20/10   xz      Move gstk_queue_rrc_utran_nmr_rsp() to gstk_exp_v.h file
10/05/10   xz      Move NMR definition to public header
09/08/10   ms      Added DSDS UT Framework changes
05/17/10   xz      Add LTE NMR definitions
03/01/10   xz      DS/DS NMR Changes
02/20/10   xz      Fix compilation error
02/17/10   bd      Fixed maximum timers supported to pass GCF cases
02/15/10   nb      Moved Dual Slot Changes
11/12/09   bd      Fixed a compiler warning
09/02/09   dd      Move gstk_is_ussd_object_supported to gstk_exp.h
07/09/09   bd      Move gstk_task declaration to gstk_exp.h
06/12/09   adp     Move gstk_is_mo_sms_ctrl_required,gstk_is_sms_pp_supported,
                   gstk_is_cell_broadcast_dl_supported to gstk_exp_v.h
04/20/09   yb      Fixed compilation errors
02/17/09   nb      Fixed compilation warning
12/17/08   nb      Common Modem Interface
10/06/08   yb      Sending BIP envelope after TR while BIP is working
09/02/08   xz      Change param type of gstk_test_automation_restart to int
08/22/08   xz      Add gstk_test_automation_restart()
08/21/08   sk      Added support for client id enhanced registration
08/08/08   xz      Add support for raw envelope command and raw format
                   registration
02/06/08   sk      Support for UTRAN NMR
11/29/07   sk      Added support for differentiating SAT and USAT TP dl
02/27/07   jk      Reduce Max Timers Supported for Memory Reduction
11/08/06   tml     Added function to return the current toolkit slot
10/08/06   sk      Added support for unknown unsupported proactive commands.
07/27/06   jk      Moved definition of gstk_is_cc_required to gstk_exp for RPC
06/09/06   sk      Lint fixes
06/06/06   sk      Added support for network search mod
05/23/06   tml     GPRS Call Control support
05/03/06   tml     lint
04/04/06   sk      Added support for 1X related to location information
                   Added support for immediate digit response and variable
                   timeout duration object for get inkey cmd.
04/04/06   sk      Added support for PLI - battery status ESN, MEID, IMEISV
10/10/05   tml     Added GSTK_ACCESS_NONE
10/08/05   tml     Added get cell ID function prototype
10/07/05   tml     Added plmn and lac in rrc cell change ind type
10/04/05   sk      Defined GSTK_MIN_CELL_ID_LEN
09/20/05   tml     RRC callback registration and hanlding to allow proper
                   cell id caching and location status event when cell ID
                   changed during normal service
07/20/05   tml     Added extended cell id support
07/13/05   sk      Added support for Access Technology.
03/05/05   tml     Added tag comprehension support
01/10/05   sk      Added count to get channel status term rsp
12/08/04   sk      Changed struct gstk_get_ch_status_cnf_type
                   to support maximum GPRS channels.
11/23/04  sk/tml   Added SAT/USAT data support
09/20/04   tml     Added support for get inkey UCSII input support
08/19/04   tml     Added cell broadcast support
07/22/04   tml     Changed to use pointer for additional info, added is ussd
                   object supported for send USSD call control
06/02/04   tml     Added lang notification, timer management, send dtmf,
                   timer expiration, browser term evt dl and lang selection
                   evt dl supports
06/01/04   tml     Added sw1 and sw2 in envelope rsp type
03/04/04   tml     Compilation fix
02/06/04   wli     Featurization for CDMA only environment
09/13/03   tml     Change mode_type to mode_enum_type and
                   slot_id_type to slot_id_enum_type
09/04/03   tml     Dual slot support
05/18/03   tml     linted
04/21/03   tml     Added sms pp download support check and cdma sms boolean flag
03/19/03   tml     Added data_present field in get inkey, get input and
                   select item cnf data type
03/06/03   tml     Updated enum names, Added gstk_is_cc_required,
                   gstk_is_mo_sms_ctrl_required
02/11/03   tml     Moved gstk_util_release_card_cmd_memory to this header
                   Added gstk_util_populate_card_cmd_memory
02/07/03   tml     Initial Version


===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"
#include "gstk_exp.h"
#include "gstk_nv_p.h"
#include "cmd.h"
#ifdef FEATURE_GSM
#include "rr_usat_if.h"
#endif /* FEATURE_GSM */
#include "sys.h"
#include "mmgsdilib.h"


/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/
#define GSTK_MAX_TIMER_SUPPORTED            8
#define GSTK_MILISECONDS_IN_AN_HOUR       3600000
#define GSTK_MILISECONDS_IN_A_MINUTE      60000
#define GSTK_MILISECONDS_IN_A_SECOND      1000
#define GSTK_MILISECONDS_IN_ONE_TENTH_SEC 100

/*--------------------------------------------------------------------------
                             Type Defs
--------------------------------------------------------------------------*/
/* Various local information that the provide local information cmd can
** request from the SIM/USIM
** Based on the parsing result from the proactive command tlv, GSTK will call
** the corresponding function in the various layers, i.e., the various layers
** involved do not need to know this enum type
*/
typedef enum
{
   /* values based on Standard */
   GSTK_LOCATION_INFO                  = 0x00,
   GSTK_IMEI                           = 0x01,
   GSTK_NMR                            = 0x02,
   GSTK_DATE_TIME_ZONE                 = 0x03,
   GSTK_LANGUAGE_SETTING               = 0x04,
   GSTK_TIMING_ADVANCE                 = 0x05,
   GSTK_ACCESS_TECHNOLOGY              = 0x06,
   GSTK_ESN                            = 0x07,
   GSTK_IMEISV                         = 0x08,
   GSTK_NETWORK_SEARCH_MODE            = 0x09,
   GSTK_BATTERY_STATUS                 = 0x0A,
   GSTK_MEID                           = 0x0B,
   GSTK_CSG_CELL                       = 0x11,
   GSTK_NO_LOCAL_INFO                  = 0xFF
}
gstk_local_info_enum_type;

/*gstki.h includes APIs between DMSS layers for the following (U)SAT related cmds:
** Polling Interval -> UIM
** Refresh          -> GSDK
** Location Info    -> MM, RR, RRC
** Location Status  -> CM
** Run AT Cmd       -> DS  (? since there is alpha tag )
*/

/* various refresh type, as specified in the command detail */
/* GSDI will based on this information to refresh its cache
and acknowledge other layers????????? */
typedef enum
{
   INITIALIZATION_FULL_FCN        = 0x00,
   FCN                            = 0x01,
   INITIALIZATION_FCN             = 0x02,
   INITIALIZATION                 = 0x03,
   USIM_RESET                     = 0x04,
   USIM_APPLICATION_RESET         = 0x05,
   A_3G_SESSION_RESET             = 0x06
}
gstk_refresh_enum_type;

/*===========================================================================
FUNCTION gstk_util_release_card_cmd_memory

DESCRIPTION
  This function frees all the allocated memory for the command pointer input.

PARAMETERS
  message_p: [Input] Pointer to message to which memory location is required
                     to be freed

DEPENDENCIES
  None

RETURN VALUE
  None

COMMENTS
  None

SIDE EFFECTS
  The member fields in the gstk_cmd_from_card_type pointer that client passed
  in will be set to null upon the return of this function

SEE ALSO
  None
===========================================================================*/
extern void gstk_util_release_card_cmd_memory(
    gstk_cmd_from_card_type * message_p );

/*===========================================================================
FUNCTION gstk_is_gprs_cc_required

DESCRIPTION
  Utility function to check if call control for PDP Context Activation
  is required for any GPRS Data calls

PARAMETER
  Slot ID

DEPENDENCIES
  None

RETURN VALUE
  boolean

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern boolean gstk_is_gprs_cc_required(gstk_slot_id_enum_type slot);

/*===========================================================================
FUNCTION gstk_is_cell_broadcast_dl_supported

DESCRIPTION
  Utility function to check if Cell Broadcast Download is allowed/supported

PARAMETER
  slot [Input]: Indicates which slot the query is intended for

DEPENDENCIES
  None

RETURN VALUE
  boolean

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern boolean gstk_is_cell_broadcast_dl_supported(gstk_slot_id_enum_type slot);

/*===========================================================================
FUNCTION gstk_cmd

DESCRIPTION

  This function allows clients to put the gstk_cmd onto the GSTK command
  queue.

PARAMETER
  gstk_cmd [Input]:  Pointer of the task_cmd_type

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
extern void gstk_cmd(gstk_task_cmd_type *gstk_cmd);

/*===========================================================================
FUNCTION gstk_task_get_cmd_buf

DESCRIPTION
  This function get the gstk_task_cmd_type from the GSTK free queue.
  Calling client SHOULD NOT change information on the gstk_task_cmd_type's
  header.

PARAMETER
  None

DEPENDENCIES
  None

RETURN VALUE
  gstk_task_cmd_type pointer

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern gstk_task_cmd_type *gstk_task_get_cmd_buf ( void  );

/*===========================================================================
FUNCTION gstk_task_free_cmd_buf

DESCRIPTION
  This function frees the GSTK command buffer memory

PARAMETER
  cmd_ptr The pointer to GSTK command buffer

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
extern void gstk_task_free_cmd_buf ( gstk_task_cmd_type * cmd_ptr );

#endif /* GSTKI_H */

