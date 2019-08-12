/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
               D S U T I L . C
           
GENERAL DESCRIPTION
  Utility functions for DS module

Copyright (c) 2004-2014 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/dsmgr/src/dsutil.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

===========================================================================
when       who        what, where, why
--------   ---       ------------------------------------------------------- 
06/03/13   sd        ATCoP decoupling changes
08/13/12   ss        EFS write optimizations.
09/05/11   ss        Changes to support RPM.
08/09/11   sha       Global variable clean up.
07/12/11   dvk       Fixed Klocwork errors
06/06/11   ash       Added support for PPP Partial Context
05/05/11   ack       Moving ds_path_is_directory declaration to .h file
03/23/11   sn        Globals cleanup for free floating of tasks.
02/16/11   op        Klocwork fix and comments cleanup
01/17/11   ss        Added support for Iface linger.
09/07/10   op        Added functions to use EFS items.
07/18/10   mg        Added functions for reading EFS file for PDN throttling 
                     feature for eHRPD
04/16/10   ls        Created this file. Merge efs functions from 
                     dshdr_efsif.c to dsutil.c

===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "customer.h"
#include "mcfg_fs.h"

#include "dsutil.h"
#include "msg.h"
#include "data_msg.h"

#include <stringl/stringl.h>
#include "data_err_suppr.h"
#include "ds_3gpp_hdlr.h"
#include "ds_3gpp_throttle_sm.h"
#include "ds_3gpp_pdn_throttle_sm.h"
#include "ds_3gpp_bearer_context.h"
#include "ds_3gppi_utils.h"
#include "ds707_pkt_mgr.h"
#include "ds_3gppi_utils.h"
#include "ds_3gpp_nv_manager.h"
#include "ds_wlan_util.h"
#include "ds3gcfgmgr.h"
#include "ds3gmgr.h"
#include "ds_3gpp_srv_req_throt_config.h"
#include "ds_3gpp_pdn_throt_config.h"
/*= = = = = = = =  = = = = = = = = = =  = = =  = = = = = = = = = = = = = = =
                        MACRO DEFINITIONS
= = = = = = = = = = = = = = =  = = = = =  = = = = = = = = = = = = = = = = =*/

/*===========================================================================
            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE
===========================================================================*/

/*---------------------------------------------------------------------------
  Specifies the no. & size of PDN throttle config params
  10 failure timer(for global throttling)
---------------------------------------------------------------------------*/
#define DS_UTIL_MAX_NUM_THROTTLE_PARAMS 10 
#define DS_UTIL_MAX_THROTTLE_PARAM_NAME_SIZE 20


#define DS_SIGN(x)              (((x) < 0) ? -1 : 1)
/*---------------------------------------------------------------------------
  Weighting coefficient to calculate the exponential moving average of
  actual throughput
  100%   --> Ignore the history of moving averages calculated previously,
             and record the current value as the average
 
  0%     --> Discard the current value of the item and report only
                          the previous moving average 
  CHOOSE WEIGHTING COEFFICIENT BETWEEN 0 and 100% to moving average
  ---------------------------------------------------------------------------*/
#define DSUTIL_MOVING_AVG_COEFF          40

/*---------------------------------------------------------------------------
  An array of strings to map the throttle name string (mentioned in EFS 
  file) to a token ID. This token ID will be used to modify the correct entry
  in the internal data structure. The token ID is the index of the string 
  entry in the array.
---------------------------------------------------------------------------*/
static char ds_util_throt_param_name
      [DS_UTIL_MAX_NUM_THROTTLE_PARAMS][DS_UTIL_MAX_THROTTLE_PARAM_NAME_SIZE] = 
                                                {"FAILURE_TIMER_1",
                                                 "FAILURE_TIMER_2",
                                                 "FAILURE_TIMER_3",
                                                 "FAILURE_TIMER_4",
                                                 "FAILURE_TIMER_5",
                                                 "FAILURE_TIMER_6",
                                                 "FAILURE_TIMER_7",
                                                 "FAILURE_TIMER_8",
                                                 "FAILURE_TIMER_9",
                                                 "FAILURE_TIMER_10",
                                                };



/*---------------------------------------------------------------------------
  Global variable to hold the value of 
  NV_TRAFFIC_CHANNEL_TEARDOWN_IN_LINGERING
---------------------------------------------------------------------------*/
static boolean ds_traffic_channel_teardown_in_lingering = FALSE;


/*===========================================================================
        Local functions.
===========================================================================*/
int16 ds_util_get_throttle_token_id
(
  char   *from,
  char   *to,
  uint8  *token_id
);


/*===========================================================================

                      EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/


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
  int     coefficient
)
{
  uint64  new_avg = 0;
  uint16  factor = 0;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (coefficient < 0)
  {
    factor = 0;
  }
  else
  {
    factor = coefficient;
  }
  new_avg =  (((factor * new_item)+((100 - factor) * prev_avg)) / 100);
  return new_avg;
}

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
  uint32  prev_avg_throughput,
  uint64  prev_total_rx_bytes,
  uint64  current_total_rx_bytes,
  uint64  sampling_interval,
  uint8   moving_avg_coeff
)
{
  uint64 current_rate =  0; 
  uint64 num_rx_bytes =  0;
  uint64 num_rx_kbits =  0;
  uint32 new_avg =  0;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /*------------------------------------------------------------------------- 
    Calculate the total bytes received/transferred in the current sampling
    interval
    -------------------------------------------------------------------------*/
  if(current_total_rx_bytes >= prev_total_rx_bytes)
  {
    num_rx_bytes  = (current_total_rx_bytes - prev_total_rx_bytes);
  }
  else
  {
    /*---------------------------------------------------------------------- 
      (uint64) Integer overflow. This may happen if the bearer is active
      for a sufficiently longer time
      ----------------------------------------------------------------------*/
    num_rx_bytes =(MAX_UINT64 - prev_total_rx_bytes)+(current_total_rx_bytes);
  }
  /*------------------------------------------------------------------------- 
    From total bytes received, calculate total number of kilo bits rx'ed
    1 Byte = 8 bits
    1 kbit = 1024 bits
    and convert the result in kbits
    -------------------------------------------------------------------------*/
  num_rx_kbits = DSUTIL_BYTES_TO_KBITS(num_rx_bytes); 

  /*------------------------------------------------------------------------- 
    Calculate the throughput rate for the current sampling interval
    -------------------------------------------------------------------------*/
  if(sampling_interval > 0)
  {
    /*-----------------------------------------------------------------------
      From the total bits rx'ed and the current sampling interval,
      calculate the equivalent throughput per sec
     
      RATE PER SECOND (kbps)  =    (Total bits received * 1000 ( in ms))
                                   -------------------------------------
                                    Sampling Interval ( in ms)
      -----------------------------------------------------------------------*/
    current_rate  = ((num_rx_kbits * 1000) / (sampling_interval));


  }
  else
  {
    current_rate = 0;
  }
  /*-------------------------------------------------------------------------- 
    Compute the running average of the actual throughput
    --------------------------------------------------------------------------*/
  new_avg = (uint32)ds_util_calculate_moving_avg(prev_avg_throughput,
                                               current_rate,
                                               moving_avg_coeff);
  return new_avg;
}
/*===========================================================================
FUNCTION      DS_UTIL_CTOI

DESCRIPTION   This function will convert character to interger.

DEPENDENCIES  None.

RETURN VALUE  Integer value
             
SIDE EFFECTS  None.
===========================================================================*/
__inline int ds_util_ctoi( char c )
{
  int  n = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( c >= 'A' && c <= 'F' ) 
  {
    n =  ( c - 'A' ) + 10;
  }
  else if ( c >= 'a' && c <= 'f' ) 
  {
    n =  ( c - 'a' ) + 10;
  }
  else if( c >= '0' && c <= '9' )
  {
    n = ( c - '0' ) + 0;
  }

  return n;
} /* ds_util_ctoi() */

/*===========================================================================
FUNCTION      DS_UTIL_HEXATOI

DESCRIPTION   Converts a hexadecimal string to decimal equivalent.

DEPENDENCIES  None.

RETURN VALUE  Return decimal equivalent
             
SIDE EFFECTS  None.
===========================================================================*/
int ds_util_hexatoi
(
  const char *x
)
{
  int nsystem = 10; /* Binary system.*/
  int z = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( *x == '0' && *(x+1) && ( *(x+1) == 'x' || *(x+1) == 'X' ) )
  {
    /*
     Hexa decimal system. Always look for 0x before we read it
     as hexa decimal number.
    */
    nsystem = 16;
    x += 2;
  }

  while( *x )
  {
    if(*x != '\r')
    {
      z = z * nsystem + ds_util_ctoi(*x);
    }
    (x)++;
  }

  return z;
} /* ds_util_hexatoi() */

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
  const char  *dirname
)
{
  mcfg_fs_status_e_type  rsp;
  struct fs_stat         stat_info;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  rsp = mcfg_fs_stat( dirname,
                      &stat_info,
                      MCFG_FS_TYPE_EFS,
                      MCFG_FS_SUBID_NONE );
  DATA_3GMGR_MSG3(MSG_LEGACY_LOW,
                  "mcfg_fs_stat rsp:%d mode:%d errno:%d",
                  rsp,stat_info.st_mode, mcfg_fs_errno(MCFG_FS_TYPE_EFS));

  if( MCFG_FS_STATUS_OK != rsp )
  {
    rsp = mcfg_fs_errno(MCFG_FS_TYPE_EFS);
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW, "mcfg_fs_stat error %d", rsp);
    if( ENOENT != rsp )
    {
      return rsp;
    }
  }
  else if( S_ISDIR (stat_info.st_mode))
  {
    return 0;
  }

  return -1;
} /* ds_path_is_directory */

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
boolean ds_get_traffic_channel_teardown_in_lingering_val( void )
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  return ds_traffic_channel_teardown_in_lingering;
} /* ds_get_traffic_channel_teardown_in_lingering_val() */

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
  boolean  current_traffic_chan_teardown_val
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  ds_traffic_channel_teardown_in_lingering = 
                              current_traffic_chan_teardown_val;
  return;
} /* ds_set_traffic_channel_teardown_in_lingering_val() */

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
  int32 dividend, /* dividend */
  int32  divisor  /* divisor */
)
{
  int32  quotient;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (divisor == 0)
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "Divide by zero");
    return 0;
  }

  /* Get exact/truncated value after integer division */
  quotient = dividend/divisor;

  /* Determine integer x such that x <= ceil(x) < x+1 */
  if ((quotient > 0) ||
      ((quotient == 0) && ((DS_SIGN(dividend) == DS_SIGN(divisor))))
     ) // leave ceil(x) truncated for -ve quotients
  {
    if ((divisor * quotient) != (dividend))
      quotient++;  //round towards +infinity
  }

  /* Note that -22/5 = -4, not -5 */
  return quotient;
} /* ds_math_ceil */

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
  int32  dividend,  /* dividend */
  int32  divisor    /* divisor */
)
{
  int32  quotient;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if (divisor == 0)
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "Divide by zero");
    return 0;
  }

  /* Get exact/truncated value after integer division */
  quotient = dividend/divisor;

  /* Determine integer x such that x-1 < floor(x) <= x */
  if ((quotient < 0) ||
      ((quotient == 0) && (DS_SIGN(dividend) != DS_SIGN(divisor))))  
     // leave floor(x) truncated for +ve quotients
  {
    if (abs(divisor * quotient) != abs(dividend))
      quotient--;  // round towards -infinity
  }

  /* Note that -22/5 = -5, not -4 */
  return quotient;
} /* ds_math_floor */

/*===========================================================================
FUNCTION      DS_UTIL_GET_THROTTLE_TOKEN_ID

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
)
{
  int16 ret_val = -1; /* return value */
  uint8 i;            /* counter for number of params */
  uint8 length;       /* length of the token (param name) */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Get the length of the string that is passed */
  if  ( from > to )  
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "Invalid Parameters");
    return ret_val;
  }

  /* Assumption that the token length will not be greater than 255 */
  length =(uint8)(to-from);

  for (i=0; i<DS_UTIL_MAX_NUM_THROTTLE_PARAMS; i++)
  {
    if (length == strlen(ds_util_throt_param_name[i]))
    {
      if (0 == strncasecmp(from,ds_util_throt_param_name[i],length))
      {
        *token_id = i;
        ret_val = 0;
        break;
      }
    }
  }
  return ret_val;
} /* ds_util_get_throttle_token_id */

