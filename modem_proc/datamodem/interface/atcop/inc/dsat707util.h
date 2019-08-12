#ifndef DSAT707UTIL_H
#define DSAT707UTIL_H
/*===========================================================================

                        D A T A   S E R V I C E S
                A T   C O M M A N D   P R O C E S S O R

               I S - 7 0 7   C O M M A N D   T A B L E S                
                I N T E R N A L   H E A D E R   F I L E


DESCRIPTION
  This file contains the definitions of data structures, defined and
  enumerated constants, and function prototypes used by the
  data services AT command processor.

  EXTERNALIZED FUNCTIONS
  dsat707_find_cmd_name
    This function matches the command name that gets passed in with the
    command table entry and returns the corresponding index.

  dsat707_unrec_func_ptr
    This function processess unrecognized commands, and commands with
    parameter errors which will cause a connection to the base station
    if +CXT=1.
  
  Copyright (c) 1995-2004, 2012-2014 by Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/atcop/inc/dsat707util.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $
		      
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/27/14   tk      Added support for Dynamic ATCoP.
01/19/12   sk      Feature cleanup.
05/10/10   kk      Added functions to read and parse MIN and IMSI NV items.
05/16/04   vr      Added function prototypes for uint32_to_str, str_to_uint32
                   and mcast_str_to_uint32()
04/08/04   gr      Exported force_cdma_dial_str retrieved from NV
12/25/03   rsl     Changes relating to Unrec. cmds not working.
07/13/02   atp     Changed signature of dsat707_send_config() to make param
                   passed in a ptr to const byte.
04/03/02   rsl     Initial version of file.

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA_IS707
#include "dsati.h"


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
#ifdef FEATURE_DATA_IS707_ASYNC
/*===========================================================================
FUNCTION DSAT707_FIND_CMD_NAME

DESCRIPTION
  This function matches the command name that gets passed in with the
  command table entry and returns the corresponding index.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/

int dsat707_find_cmd_name (byte *s1);

/*===========================================================================

FUNCTION DSAT707_UNREC_FUNC_PTR

DESCRIPTION
  This function processess unrecognized commands, and commands with parameter
  errors which will cause a connection to the base station if +CXT=1.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/

dsat_result_enum_type dsat707_unrec_func_ptr( byte * );

/*=========================================================================

FUNCTION DSAT707I_UNREC_FUNC_PTR

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
dsat_result_enum_type dsat707i_unrec_func_ptr
(
  byte* cmd_line_ptr
);

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
  const byte *
);

/*===========================================================================

FUNCTION DSAT707I_SEND_CONFIG

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

void dsat707i_send_config
(
  const byte * cur_cmd
);
#endif /* FEATURE_DATA_IS707_ASYNC */

/*===========================================================================

FUNCTION DSAT707_IS_STATUS

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
extern boolean dsat707_is_status
(
  byte *a_ptr,
  dsat_result_enum_type result
);

/*===========================================================================
  FUNCTION DSAT707_IP_STR_TO_UINT32

  DESCRIPTION
    This function takes an IP address in string format (which includes
    "." as delimiters. e.g. 123.23.123.231) and convert the string into
    a uint32 value.

  DEPENDENCIES
    None

  RETURN VALUE
    status ( success or error)

  SIDE EFFECTS
    None
===========================================================================*/
boolean dsat707_ip_str_to_uint32
(
  char   *ip_string,
  uint32 *ip_uint32
);

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
byte* dsat707_ip_uint32_to_str
(
  uint32 ip_uint32,
  byte   *rb_ptr
);

/*===========================================================================
FUNCTION DSAT707_MCAST_IP_STR_TO_UINT32

DESCRIPTION
  This function takes an IP address in string format (which includes
  "." as delimiters. e.g. 123.23.123.231) and convert the string into
  a uint32 value. It checks to make sure that the IP address is in the
  multicast range or is 0.0.0.0

  The string may have more information than the IP address and this 
  function will stop processing once it has found or failed to find a valid
  IP address. The number of bytes processed by the function is returned in
  len_curr_ip.

DEPENDENCIES
  None

RETURN VALUE
  OK if the IP address is a valid multicast IPv4 address or 0.0.0.0,
  ERROR otherwise. The number of bytes processed is returned in 
  len_curr_ip independent of success or failure.

SIDE EFFECTS
  None
===========================================================================*/
boolean dsat707_mcast_ip_str_to_uint32
(
  char   *ip_string,
  uint32 *ip_uint32,
  uint32 *len_curr_ip
);

/*===========================================================================
FUNCTION DSAT707_STRIP_QUOTES

DESCRIPTION
  This fuction 
  - takes an input quoted string,
  - puts a copy of the string in "parsed_string" with the quotes stripped off
  - NULL-terminates the parsed_string by placing \0 at the end.
  - Validates that the output string length falls within the range specified
    in the ? 
returning  an ERROR if out 
  The function also checks for the range of the AT command and throws out an
  error if the range is exceeded.

  For examples,
    "123.12.23.21"           ---> 123.12.23.21\0
    "nobody,one@nowhere.com" ---> nobody,one@nowhere.com\0

DEPENDENCIES
  None

PARAMETERS
   raw_string - Returned by the standard ATCoP parser, ds_atcop_parse_ex().
   parsed_string - Buffer where the parsed argument shall be returned.
   table_entry  -  To find out the size of the argument defined in the table.

RETURN VALUE
  FALSE  if - null input or output string pointers are provided
            - output string length is out of range

  TRUE   if conversion goes well

SIDE EFFECTS
  None
===========================================================================*/
boolean dsat707_strip_quotes
(
  char *  in_s,                   /* input string (to be stripped)         */
  char *  parsed_s,               /* output string placed here             */
  uint16  max_parsed_len          /* size of provided output buffer        */
);

#endif /* FEATURE_DATA_IS707 */
#endif /* DSAT707UTIL_H */
