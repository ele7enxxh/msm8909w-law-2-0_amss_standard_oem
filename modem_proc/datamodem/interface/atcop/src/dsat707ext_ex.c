/*===========================================================================

                        D A T A   S E R V I C E S
                A T   C O M M A N D   P R O C E S S O R

DESCRIPTION
  This module executes AT commands. It executes IS-707 specific extended
  AT commands.

EXTERNALIZED FUNCTIONS
  None

Copyright (c) 1995 - 2014 by Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/atcop/src/dsat707ext_ex.c#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
06/27/14   pg     Initial revision (created file for Dynamic ATCoP).

===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA_IS707
#include "cm.h"
#include "db.h"
#include "dsati.h"
#include "dsatme.h"
#include "dsat707ext.h"
#include "dsat707extctab.h"
#include "dsat707util.h"
#include "dsatparm.h"
#include "mobile.h"
#include "msg.h"
#include "snm.h"
#include "ps_iface.h"
#include "ps_tcp_async.h"
#include "ps_in.h"
#include "ds3gmgrint.h"
#include "dsatvoice.h"
#include "dsatcmif.h"
#include "ds_1x_profile.h"
#include "ds707_pkt_mgr.h"

#if defined(T_WINNT)
#error code not present
#endif /* WINNT */

#include <stringl/stringl.h>
#include "ds707_epzid.h"

#ifdef FEATURE_DS_MOBILE_IP
#include "dsat707mipctab.h"
#endif /* FEATURE_DS_MOBILE_IP */

#ifdef FEATURE_DSAT_CDMA_PIN
#include "dsatme.h"
#endif /* FEATURE_DSAT_CDMA_PIN */

#ifdef FEATURE_DSAT_EXTENDED_CMD
#include "cmutil.h"
#endif /*FEATURE_DSAT_EXTENDED_CMD*/

/*===========================================================================
                        TYPEDEFS AND VARIABLES
===========================================================================*/

#ifdef FEATURE_DSAT_CDMA_PIN

/* Bit array for PIN status */
uint32 dsat707_pin_required = (uint32)DSAT_CPIN_NONE_REQUIRED;

/* Bit array for PIN blocked status: only the bits indicating a PIN apply to 
   this mask, not "change pending" or "none required" bits */
uint32 dsat707_pin_blocked = 0;

/* PIN error tracking (assume no SIM at initialization) */
dsat_cme_error_e_type dsat707_pin_error = DSAT_CME_SIM_BUSY;

#endif /* FEATURE_DSAT_CDMA_PIN */

/*===========================================================================
                   FUNCTION DEFINITIONS
===========================================================================*/

#ifdef FEATURE_DSAT_CDMA_PIN

/*===========================================================================

FUNCTION DSAT707_STATUS_EVENT_HANDLER_EX

DESCRIPTION
  This function is called after the MMGSDI fulfilled the request to
  get the pin status.  The MMGSDI reply is in cmd_status.

 
DEPENDENCIES
  None

RETURN VALUE
  DSAT_ASYNC_CMD -- if the request is sent to mmgsdi to verify pin
                  
  DSAT_CMD_ERR_RSP -- if a ME error happens. A CME error is returned
                   to TE.
  DSAT_ERROR  -- Any other ERROR

  DSAT_ASYNC_EVENT -- if no action needs to be taken..
    
SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsat707_status_event_handler_ex
(
   ds_at_cmd_status_type *cmd_info
)
{
  dsat_result_enum_type result = DSAT_ASYNC_EVENT;

  dsat707_pin_error = DSAT_CME_NO_ERROR;
  dsat707_pin_required = (uint32)DSAT_CPIN_NONE_REQUIRED;
  dsat707_pin_blocked = (uint32)DSAT_CPIN_NONE_REQUIRED;

  /* from here till down are all +CPIN/^CPIN related handling */

  DS_AT_MSG2_HIGH("MMGSDI command PIN1 (%d), PIN2(%d)",
             cmd_info->pin1.status,cmd_info->pin2.status); 
  if( cmd_info->cmd_id == (uint16)DSAT_CDMA_CPIN_MMGSDI_ALL_PIN_STATUS)
  {
    switch (cmd_info->pin1.status)
    {
      case MMGSDI_PIN_STATUS_NOT_INITIALIZED:
        dsat707_pin_error = DSAT_CME_SIM_FAILURE;
        break;
      case MMGSDI_PIN_ENABLED_NOT_VERIFIED:
        dsat707_pin_required |= (uint32)DSAT_CPIN_SIM_PIN_REQUIRED;
        dsat707_pin_blocked &= ~(uint32)DSAT_CPIN_SIM_PIN_REQUIRED;
        break;
      case MMGSDI_PIN_BLOCKED:
        dsat707_pin_required |= (uint32)DSAT_CPIN_SIM_PIN_REQUIRED;
        dsat707_pin_blocked |= (uint32)DSAT_CPIN_SIM_PIN_REQUIRED;
        break;
      case MMGSDI_PIN_PERM_BLOCKED:
        dsat707_pin_blocked |= (uint32)DSAT_CPIN_SIM_PIN_REQUIRED;
        dsat707_pin_required |= (uint32)DSAT_CPIN_SIM_PIN_REQUIRED;
        dsat707_pin_error = DSAT_CME_SIM_FAILURE;
        break;
      case MMGSDI_PIN_DISABLED:
      case MMGSDI_PIN_UNBLOCKED:
      case MMGSDI_PIN_ENABLED_VERIFIED:
        dsat707_pin_blocked &= ~(uint32)DSAT_CPIN_SIM_PIN_REQUIRED;
        dsat707_pin_required &= ~(uint32)DSAT_CPIN_SIM_PIN_REQUIRED;
        dsat707_pin_error = DSAT_CME_NO_ERROR;
        break;
      default:
        break;
    }
    switch (cmd_info->pin2.status)
    {
      case MMGSDI_PIN_STATUS_NOT_INITIALIZED:
        dsat707_pin_error = DSAT_CME_SIM_FAILURE;
        break;
      case MMGSDI_PIN_ENABLED_NOT_VERIFIED:
        dsat707_pin_required |= (uint32)DSAT_CPIN_SIM_PIN2_REQUIRED;
        dsat707_pin_blocked &= ~(uint32)DSAT_CPIN_SIM_PIN2_REQUIRED;
        break;
      case MMGSDI_PIN_BLOCKED:
        dsat707_pin_blocked |= (uint32)DSAT_CPIN_SIM_PIN2_REQUIRED;
        dsat707_pin_required |= (uint32)DSAT_CPIN_SIM_PIN2_REQUIRED;
        break;
      case MMGSDI_PIN_PERM_BLOCKED:
        dsat707_pin_blocked |= (uint32)DSAT_CPIN_SIM_PIN2_REQUIRED;
        dsat707_pin_required |= (uint32)DSAT_CPIN_SIM_PIN2_REQUIRED;
        break;
      case MMGSDI_PIN_DISABLED:
      case MMGSDI_PIN_UNBLOCKED:
      case MMGSDI_PIN_ENABLED_VERIFIED:
        dsat707_pin_blocked &= ~(uint32)DSAT_CPIN_SIM_PIN2_REQUIRED;
        dsat707_pin_required &= ~(uint32)DSAT_CPIN_SIM_PIN2_REQUIRED;
        dsat707_pin_error = DSAT_CME_NO_ERROR;
        break;
      default:
        break;
    }
  }

  return result;
}/* dsat707_status_event_handler_ex */
#endif

#endif /* FEATURE_DATA_IS707 */
