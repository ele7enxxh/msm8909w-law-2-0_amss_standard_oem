#ifndef DSUTIL_H
#define DSUTIL_H
/*===========================================================================

                      D S U T I L . H
 
GENERAL DESCRIPTION
  Utility functions for DS module

EXTERNALIZED FUNCTIONS
  DSUTIL_ATOI()
    Converts an ASCII string to an integer

Copyright (c) 2004-2015 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE
This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/dsmgr/inc/dsutil.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
08/13/12    ss     Adding ds_init_efs_config_file() to populate values in 
                   data config file at bootup.
01/31/12    msh    Coan: Feature cleanup 
01/02/12    msh    Coan: Feature cleanup
09/05/11    ss     Changes to support RPM.
08/09/11    sha    Global Variable Clean Up.
05/05/11    ack    Moving ds_path_is_directory declaration to .h file
03/23/11    sn     Globals cleanup for free floating of tasks.
02/16/11    op     Klocwork fix and comments cleanup
01/17/11    ss     Added support for Iface linger.
09/07/10    op     Added functions to use EFS items.
07/18/10    mg     Added functions for reading EFS file for PDN throttling 
                   feature
04/16/10    ls     Merge efs functions into this file
05/16/04    vr     Initial revision.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "customer.h"
#include "fs_public.h"
#include "fs_sys_types.h"
#include "fs_fcntl.h"
#include "fs_errno.h"
#include "ds3gmgr.h"
#include "rex.h"
/*-----------------------------------------------------------------------------
   
-----------------------------------------------------------------------------*/
#define DS_EFS_READ_BUFFER_SZ 128

/*---------------------------------------------------------------------------
  Converting a value in seconds to mili seconds
---------------------------------------------------------------------------*/
#define SEC_TO_MSEC(val_sec) ((val_sec) * 1000)

/*===========================================================================
MACRO MINUTE_TO_MSEC()

DESCRIPTION
  This macro returns a msec value for the give value in minutes

PARAMETERS
  int16 - value in minutes

RETURN VALUE
  value in msec
===========================================================================*/
#define MINUTE_TO_MSEC(val_minute)                        \
  ((val_minute) * 60000)

#define DSUTIL_BYTE_TO_BITS(byte)                       ((byte) << (3))
#define DSUTIL_BITS_TO_KBITS(total_bits)                ((total_bits) >> (10))
#define DSUTIL_BYTES_TO_KBITS(byte)                     ((byte) >> (7))
/* -----------------------------------------------------------------------
** Constants
** ----------------------------------------------------------------------- */

#ifndef MIN_INT8
#define MIN_INT8 -128
#endif
#ifndef MIN_INT16
#define MIN_INT16 -32768
#endif
#ifndef MIN_INT32
#define MIN_INT32 (~0x7fffffff)   /* -2147483648 is unsigned */
#endif
#ifndef MIN_INT64
#define MIN_INT64 (~0x7fffffffffffffffLL) /* -9223372036854775808 is unsigned */
#endif

#ifndef MAX_INT8
#define MAX_INT8 127
#endif
#ifndef MAX_INT16
#define MAX_INT16 32767
#endif
#ifndef MAX_INT32
#define MAX_INT32 2147483647
#endif
#ifndef MAX_INT64
#define MAX_INT64 9223372036854775807LL
#endif

#ifndef MAX_UINT8
#define MAX_UINT8 255
#endif
#ifndef MAX_UINT16
#define MAX_UINT16 65535
#endif
#ifndef MAX_UINT32
#define MAX_UINT32 4294967295u
#endif
#ifndef MAX_UINT64
#define MAX_UINT64 18446744073709551615uLL
#endif

/*===========================================================================
                                TYPEDEFS
===========================================================================*/

/*---------------------------------------------------------------------------
  Critical section macros for Data Services
---------------------------------------------------------------------------*/
#define DS_INIT_CRIT_SECTION( rex_crit_section )                            \
  { rex_init_crit_sect( rex_crit_section ); }

#define DS_ENTER_CRIT_SECTION( rex_crit_section )                           \
  { rex_enter_crit_sect( rex_crit_section ); }

#define DS_LEAVE_CRIT_SECTION( rex_crit_section )                           \
  { rex_leave_crit_sect( rex_crit_section ); }

/*===========================================================================

MACRO          DSUTIL_ATOI

DESCRIPTION    Converts the character input to integer type

               This takes a begin and end pointer to a string and converts
               a decimal number in string format starting at begin pointer
               to the corresponding integer value. 

               It stops converting when either a non-numeric value or the
               end pointer is encountered. On returning from this macro,
               begin pointer is set to where the conversion stopped.

               For eg: If the string passed is "123 45 a 7" then on returning
               from this macro, the output integer will be 123 and begin 
               pointer will be incremented by 3. If the macro is called
               again with "45 a 7", the output integer will be 45 and begin
               pointer will be incremented by 2. If the macro is now called
               once again with "a 7", the output integer will be 0 and begin
               pointer will be left unchanged.
                        
PARAMETERS     x : Begin pointer (char *)
               y : End pointer (char *)
               z : ouput integer

DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   Begin pointer is incremented to point beyond the end of the
               first number in the string
===========================================================================*/
#define DSUTIL_ATOI(x, y, z)                  \
        z = 0;                                \
        while( x < y )                        \
        {                                     \
          if((*x) >= '0' && (*x) <= '9')      \
          {                                   \
            z = z *10 + (*x- '0');            \
            x++;                              \
          }                                   \
          else                                \
          {                                   \
            break;                            \
          }                                   \
        }

/*===========================================================================
FUNCTION      DS_UTIL_CTOI

DESCRIPTION   The function converts char to int

DEPENDENCIES  None

RETURN VALUE  Return a integer value

SIDE EFFECTS  None.
===========================================================================*/
__inline int ds_util_ctoi
(
  char c
);

/*===========================================================================
FUNCTION      DS_UTIL_HEXATOI

DESCRIPTION   The function converts a hexadecimal string to decimal equivalent

DEPENDENCIES  None

RETURN VALUE  Return decimal equivalent

SIDE EFFECTS  None.
===========================================================================*/
int ds_util_hexatoi
(
  const char *x
);

/*===========================================================================
FUNCTION DS_PATH_IS_DIRECTORY

DESCRIPTION
  To check if the EFS directory exists

DEPENDENCIES
  None

PARAMETERS
  dirname - Directory path

RETURN VALUE
   0         - success
   efs_errno - EFS error
   -1        - Other error

SIDE EFFECTS
  None
===========================================================================*/
int32 ds_path_is_directory
(
  const char *dirname
);

/*===========================================================================
FUNCTION DS_GET_TRAFFIC_CHANNEL_TEARDOWN_IN_LINGERING_VAL

DESCRIPTION
  Returns the current value of ds_traffic_channel_teardown_in_lingering.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  boolean - TRUE or FALSE indicating whether to tear down traffic channel or 
            not during lingering.

SIDE EFFECTS
  None
===========================================================================*/
boolean ds_get_traffic_channel_teardown_in_lingering_val( void );

/*===========================================================================
FUNCTION DS_SET_TRAFFIC_CHANNEL_TEARDOWN_IN_LINGERING_VAL

DESCRIPTION
  Updates the current value of ds_traffic_channel_teardown_in_lingering.

DEPENDENCIES
  None

PARAMETERS
  boolean - TRUE or FALSE indicating whether to tear down traffic channel or 
            not during lingering.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds_set_traffic_channel_teardown_in_lingering_val
(
  boolean current_traffic_channel_teardown_in_lingering
);

/*===========================================================================
FUNCTION DS_MATH_CEIL

DESCRIPTION
  To return ceil value.

DEPENDENCIES
  None

PARAMETERS
  dividend - the dividend.
  divisor  - the divisor.

RETURN VALUE
  int32 - quotient.

SIDE EFFECTS
  None
===========================================================================*/
int32 ds_math_ceil
(
  /* dividend */
  int32 dividend,
  /* divisor */
  int32 divisor
);

/*===========================================================================
FUNCTION DS_MATH_FLOOR

DESCRIPTION
  To return floor value.

DEPENDENCIES
  None

PARAMETERS
  dividend - the dividend.
  divisor  - the divisor.

RETURN VALUE
  int32 - quotient.

SIDE EFFECTS
  None
===========================================================================*/
int32 ds_math_floor
(
  /* dividend */
  int32 dividend,
  /* divisor */
  int32 divisor
);

/*==============================================================================
FUNCTION DS_UTIL_CALCULATE_MOVING_AVG

DESCRIPTION 
  This function calculates the exponential moving average of a given item
 
  CURRENT MATHEMATICAL MODEL USED TO CALCULATE MOVING AVG OF AN ITEM
  ========================================================================
  
   - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    ==========================================================================
   ||                            - -                    - -                  ||
   ||                     W *   |A  |          (1-W) * |A  |                 ||
   ||ACTUAL THROUGHPUT =        | 1 |      +           | 0 |                 ||
   ||                            - -                    - -                  || 
    =========================================================================                             
    where
     A
      1    --->  Current Item value
   
     A
      0    ----> Average of values recorded from the previous observations
 
      W    -----> Weighting coefficient given to the previous average
   - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
DEPENDENCIES
    None

RETURN VALUE
  uint64 - updated moving average throughput

SIDE EFFECTS
  None
===========================================================================*/
uint64 ds_util_calculate_moving_avg
(
  uint64  prev_avg,
  uint64  new_item,
  int     weighting_coefficient
);

/*==============================================================================
FUNCTION DS_UTIL_CALCULATE_THROUGHPUT_MOVING_AVG

DESCRIPTION 
  This function calculates the moving average of the throughput values 
  given the sampling interval.
  This can be used to calculate both DL/UL throughput for the bearers/MAC ID's
 
PARAMETERS 
   uint32               prev_avg_throughput,
   uint64               prev_total_rx_bytes,
   uint64               current_total_rx_bytes,
   uint64               sampling_interval
 
DEPENDENCIES
    None
RETURN VALUE
  uint32 - updated moving average throughput

SIDE EFFECTS
  None
===========================================================================*/
uint32 ds_util_calculate_throughput_moving_avg
(
   uint32               prev_avg_throughput,
   uint64               prev_total_rx_bytes,
   uint64               current_total_rx_bytes,
   uint64               sampling_interval,
   uint8                moving_avg_coeff
);

/*===========================================================================
FUNCTION      ds_util_get_throttle_token_id

DESCRIPTION   This function returns the token id associated with each pdn
              throttle config parameter. 

PARAMETERS    char *from - start of the token (param name)
              char *to   - end of the token (param name)
              *token_id  - Pointer in which Token ID is returned

DEPENDENCIES  None

RETURN VALUE  0  - success
              -1 - failure

SIDE EFFECTS  None
===========================================================================*/
int16 ds_util_get_throttle_token_id
(
  char *from,
  char *to,
  uint8 *token_id
);
#endif /* DSUTIL_H */
