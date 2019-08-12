/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                           P S _ U T I L S . C

GENERAL DESCRIPTION
  Collection of utility functions being used by various modules in PS.
  Most of these functions assume that the caller is in PS task context.

Copyright (c) 1995-2011 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_utils.c_v   1.0   08 Aug 2002 11:19:58   akhare  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/utils/src/ps_utils_cs.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
01/10/11    ss     Cleaning up of Globals for thread safety in Q6 Free 
                   Floating environment.
11/21/08    pp     Lint fixes.
09/07/07    scb    Added REX signal ext code under FEATURE_REX_SIGS_EXT
11/02/06    mct    Added 64 bit random number generator function.
02/22/06    msr    Using single critical section
10/31/04   msr/ks  Added memdump().
04/30/04    mct    Fixed some lint errors.
08/05/02    usb    Moved lcsum() from this file to ps_iputil.c
08/05/02    usb    Fixed get/set nv functions, moved them out of FEATURE_MIP
07/31/02    usb    Renamed the file from psmisc.c
06/14/02    usb    Removed byte manipulation functions.  Use dsbyte.h now.
04/17/02    rc     Wrapped code in !FEATURE_DATA_WCDMA_PS
12/21/01    dwp    Wrap get16 in !FEATURE_DATA_PS as it is defined else where
                   in MSM5200 archive.
11/12/01    dwp    Add "|| FEATURE_DATA_PS" to whole module.
05/24/00    hcg    Added TARGET_OS_SOLARIS to compile with Solaris DSPE.
04/21/00    mvl    Fixed a #define so compiles properly under COMET
01/09/99    jjw    Changed to generic Browser interface
10/27/98    ldg    For T_ARM included C version of TCP checksum routine.
06/16/98    na     Converted the routine that calculates the TCP checksum
                   into 186 assembly.
06/25/97    jgr    Added ifdef FEATURE_DS over whole file
07/22/95    jjw    Created Initial Version
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"       /* Customer Specific Features */

#include "ds_Utils_DebugMsg.h"
#include "err.h"
#include "amssassert.h"

#include "ps_utils.h"

#include "ds_Utils_StdErr.h"
#include "ds_Utils_IEnv.h"
#include "ds_Utils_env.h"

#include "ds_Utils_ISysTimer.h"
#include "ds_Utils_CSysTimer.h"

#include "nv.h"

#include "_debug.h"

#if (TG == T_PC)
#else
#include "qw.h"
#endif

/*===========================================================================

                      LOCAL DECLARATIONS FOR MODULE

===========================================================================*/
/*---------------------------------------------------------------------------
  Command item to NV.
---------------------------------------------------------------------------*/
static nv_cmd_type  ps_nv_cmd_buf;

/* list of NV item/value pairs */

typedef struct ps_nv_item_list_type {
  boolean bSet;
  nv_items_enum_type item_code;
  nv_item_type item_value;
} ps_nv_item_list_type;

static ps_nv_item_list_type ps_nv_item_list[] = {
  {FALSE, NV_TCP_GRACEFUL_DORMANT_CLOSE_I, 0},
  {FALSE, NV_TCP_GRACEFUL_DORMANT_CLOSE_I, 0},
  {FALSE, NV_TCP_KEEPALIVE_IDLE_TIME_I, 0},
  {FALSE, NV_IPV6_ENABLED_I, 0},
  {FALSE, NV_IPV6_FAILOVER_CONFIG_I, 0},
  {FALSE, NV_IPV6_PRIVACY_EXTENSIONS_ENABLED_I, 0},
  {FALSE, NV_IPV6_PRIVATE_ADDRESS_CONFIG_I, 0},
  {FALSE, NV_ICMP6_ND_CONFIG_I, 0},
  {FALSE, 0,0}
  };


/*===========================================================================

                      DEFINITIONS FOR MODULE

===========================================================================*/

/*===========================================================================

FUNCTION PS_GET_NV_ITEM

DESCRIPTION
  This function retrieves the specified item from NV.

DEPENDENCIES
  The NV task has been started and is running.
  This functions is Non-reentrant.

RETURN VALUE
  Status returned from the NV read request.  An LOG_MSG_ERROR is logged if status is
  other than:
    NV_DONE_S       - request done
    NV_NOTACTIVE_S  - item was not active

SIDE EFFECTS
  While this function is running all other PS task activities are
  suspended except for watchdog kicking, and until the NV item is
  read in.

===========================================================================*/

nv_stat_enum_type ps_get_nv_item
(
  nv_items_enum_type  item_code,       /* Item to get                      */
  nv_item_type        *data_ptr        /* Pointer where to put the item    */
)
{
  int i;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for (i = 0; 0 != ps_nv_item_list[i].item_code; i++) {
    if (item_code == ps_nv_item_list[i].item_code &&
        TRUE == ps_nv_item_list[i].bSet) {
      *data_ptr = ps_nv_item_list[i].item_value;
      return NV_DONE_S;
    }
  }

  return NV_FAIL_S;
} /* ps_get_nv_item() */


/*===========================================================================
FUNCTION PS_PUT_NV_ITEM

DESCRIPTION
  Write an item to NV memory.  Wait until write is completed.

RETURN VALUE
  Status returned from the NV read request.  An LOG_MSG_ERROR is logged if status is
  other than:
    NV_DONE_S       - request done

DEPENDENCIES
  This function can only be called from PS task.  Also it is not
  reentrant. Shouldn't be a problem, as it doesn't exit till we're done, and
  it's only called from the PS task.

RETURN VALUE
  Status returned from the NV read request.  An LOG_MSG_ERROR is logged if status is
  other than:
    NV_DONE_S       - request done
    NV_NOTACTIVE_S  - item was not active

SIDE EFFECTS
  While this function is running all other PS task activities are
  suspended except for watchdog kicking, and until the NV item is
  wrote down.

===========================================================================*/
nv_stat_enum_type ps_put_nv_item(
  nv_items_enum_type item_code,                              /* which item */
  nv_item_type *data_ptr                       /* pointer to data for item */
)
{
  int i;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for (i = 0; 0 != ps_nv_item_list[i].item_code; i++) {
    if (item_code == ps_nv_item_list[i].item_code) {
      ps_nv_item_list[i].item_value = *data_ptr;
      ps_nv_item_list[i].bSet = TRUE;
      return NV_DONE_S;
    }
  }

  return NV_FAIL_S;

} /* ps_put_nv_item() */


/*===========================================================================
FUNCTION PS_UTILS_GENERATE_RAND_64BIT_NUM()

DESCRIPTION
  This function generates a random 64 bit number.

PARAMETERS
  *random_num - Pointer to the 64 bit number to be returned by this function.

RETURN VALUE
  None

DEPENDENCIES
  As a pre-condition, this function assumes that ran_seed() has been called
  at least once. Currently, ran_seed() is called by the main control task
  as part of initialization.

SIDE EFFECTS
  None
===========================================================================*/
void ps_utils_generate_rand_64bit_num
(
  uint64 *rand_num                 /* Pointer to the 64bit num be returned */
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  ps_utils_generate_rand_num(rand_num, sizeof(uint64));

} /* ps_utils_generate_rand_64bit_num */


/*===========================================================================

FUNCTION msclock

DESCRIPTION
  This function will return the time in milliseconds since ....

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
dword msclock( void)
{
  IEnv* piEnv;
  ISysTimer* pist = 0;
  uint32 dwTime = 0;
  int nErr;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  nErr = env_GetCurrent(&piEnv);
  ASSERT(AEE_SUCCESS == nErr);
  nErr = IEnv_CreateInstance(piEnv, AEECLSID_CSysTimer, (void**)&pist);
  ASSERT(AEE_SUCCESS == nErr);
  ISysTimer_GetTimeRef(pist, &dwTime);
  ASSERT(AEE_SUCCESS == nErr);
  IQI_RELEASEIF(pist);

  return dwTime;

} /* msclock() */

