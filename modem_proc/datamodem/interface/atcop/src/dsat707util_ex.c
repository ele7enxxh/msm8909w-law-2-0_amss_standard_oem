/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        D A T A   S E R V I C E S
                A T   C O M M A N D   P R O C E S S O R

               I S - 7 0 7   C O M M A N D   T A B L E S

GENERAL DESCRIPTION
  This module contains generic function definitions used by the IS707
  AT command processor.
  
EXTERNALIZED FUNCTIONS
  dsat707_unrec_func_ptr
    This function processess unrecognized commands, and commands with
    parameter errors which will cause a connection to the base station
    if +CXT=1.
    
INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

  Copyright (c) 1995-2014 by Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/atcop/src/dsat707util_ex.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/27/14   tk      Initial revision (created file for Dynamic ATCoP).

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA_IS707
#include "amssassert.h"
#include "dsati.h"
#include "dsatdl.h"
#include "dsat707extctab.h"
#include "dsat707faxctab.h"
#include "ps_in.h"
#ifdef FEATURE_HDR
#include "dsat707hdrctab.h"
#endif /* FEATURE_HDR */
#ifdef FEATURE_DS_MOBILE_IP
#include "dsat707mipctab.h"
#endif /* FEATURE_DS_MOBILE_IP */
#ifdef FEATURE_DS_PSTATS
#include "dsat707pstatsctab.h"
#endif /* FEATURE_DS_PSTATS */
#include "dsat707vendctab.h"
#include "dsat707util.h"
#include "dsatparm.h"
#include "msg.h"
#include "dstaski.h"
#include "ds707_async_mgr.h"
#include "dsatact.h"
#include "dsat707mip.h"
#include "dsat707mipctab.h"
#include "ds_1x_profile.h"
#include "err.h"

#include <stringl/stringl.h>

#include "ds707_jcdma_m51.h"
#include "ds707_epzid.h"
#ifdef FEATURE_DS_EPZID_HYSTERESIS
#include "ds707_epzid_hyst.h"
#endif /* FEATURE_DS_EPZID_HYSTERESIS */

#include "dsat707extctab.h"
#include "dsat707faxctab.h"

#include "ds707_roaming.h"

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains local definitions for constants, macros, types,
  variables and other items needed by this module.

===========================================================================*/
#ifdef FEATURE_DATA_IS707_ASYNC
/*=========================================================================

FUNCTION DSAT707_UNREC_FUNC_PTR

DESCRIPTION
  This function processess unrecognized commands, and commands with parameter
  errors which will cause a connection to the base station if +CXT=1.

DEPENDENCIES
  None
  
RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

SIDE EFFECTS
  None
===========================================================================*/
dsat_result_enum_type dsat707_unrec_func_ptr
(
  byte* cmd_line_ptr
)
{
  DSAT_DL_CHECK_SYMBOL_ADDR(dsatdl_vtable.dsat707i_unrec_func_ptr_fp);

  return dsatdl_vtable.dsat707i_unrec_func_ptr_fp(cmd_line_ptr);
}

/*===========================================================================

FUNCTION DSAT707_SEND_CONFIG

DESCRIPTION
  This function transmits the mobile configuration to the base station.
  Configuration parameters are those which have the DOWN_LOAD attribute
  in the parse table.  The only DOWN_LOAD paramters transmitted are those
  with the ALWAYS attribute in the parse table, and those which
  have been changed from their default values.  Parameters with
  the FCLASS2P0 attribute are not transmitted unless the current value
  of the FCLASS parameter is 2.0.

  Follow the configuration with the +CFG string (always, even if empty),
  and then the current command.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

void dsat707_send_config
(
  const byte * cur_cmd
)
{
  DSAT_DL_CHECK_SYMBOL_ADDR(dsatdl_vtable.dsat707i_send_config_fp);

  dsatdl_vtable.dsat707i_send_config_fp(cur_cmd);

  return;
} /*  dsat707_send_config  */
#endif /* FEATURE_DATA_IS707_ASYNC */

/*===========================================================================

FUNCTION DSAT_IS_STATUS

DESCRIPTION
  This function returns TRUE if the argument string begins with
  the numeric character (1-8)  or the word corresponding to the
  result specified by the second arg.  Trailing characters are ignored.
  Other wise if returns FALSE.

DEPENDENCIES
  None

RETURN VALUE
  See description.

SIDE EFFECTS
  None
===========================================================================*/

boolean dsat_is_status
(
  byte *a_ptr,
  dsat_result_enum_type result
)
{
  DSAT_DL_CHECK_SYMBOL_ADDR(dsatdl_vtable.dsat707_is_status_fp);

  return dsatdl_vtable.dsat707_is_status_fp(a_ptr, result);
} /* dsat_is_status */


/*===========================================================================
  FUNCTION DSAT707_IP_UINT32_TO_STR

  DESCRIPTION
    This function takes an IP address in uint32 format and returns a string.

  DEPENDENCIES
    None

  RETURN VALUE
    rb_ptr incremented by the size of the ASCII representation of the IP
    address.

  SIDE EFFECTS
    None
===========================================================================*/
byte * dsat707_ip_uint32_to_str
(
  uint32 ip_uint32,
  byte   *rb_ptr
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  rb_ptr = dsatutil_itoa(((ip_uint32 >> 24) & 0xFF ),rb_ptr,10);
  *rb_ptr++ = '.';
  rb_ptr = dsatutil_itoa(((ip_uint32 >> 16) & 0xFF),rb_ptr,10);
  *rb_ptr++ = '.';
  rb_ptr = dsatutil_itoa(((ip_uint32 >>  8) & 0xFF),rb_ptr,10);
  *rb_ptr++ = '.';
  rb_ptr = dsatutil_itoa((ip_uint32 & 0xFF),rb_ptr,10);

  return rb_ptr;

} /* dsat707_ip_uint32_to_str () */

#endif /* FEATURE_DATA_IS707 */
