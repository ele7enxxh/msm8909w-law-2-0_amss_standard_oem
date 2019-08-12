/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

             C A L L   M A N A G E R   C L I E N T   M O D U L E

GENERAL DESCRIPTION
  This module contains the Call Manager Client Interface Block

EXTERNALIZED FUNCTIONS


Copyright (c) 1991 - 2014 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/cm/src/cmclient.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/04/13   mh/jvo  Mainline the modem statistics code
08/28/13   jvo     Cleanup remaining Fusion pieces
07/19/13   jvo     Remove all MDM/MSM Fusion features
06/18/13   gm      FR 2617 - OMADM IMS and ADMIN PDN Disable Requirement
11/22/12   xs      Added PS supplemetary service support
01/06/12   gm      Data synchronization changes for freefloating platform
04/11/11   am      Cleaning stubs and runnning qtf SUTE
03/21/11   anm     CM not sending wrong call id to clients while ending the
                   call.
02/25/11   cl      GW Service Indication Propagation MSM->MDM
02/17/11   cl      Add support FEATURE_MMODE_REMOVE_1X for FUSION-MDM
02/11/11   ak      MDM to notify MSM of DO activity for acq throttling
12/23/10   sv      Integrate DSDS feature
12/10/10   mp      Merge: To buffer CM_PH_EVENT_INFO in SUSPEND state
11/24/10   xs      Fusion featurization clean up
11/02/10   am      Replacing FEATURE_MOB_MMODE_CUTE
10/01/10   ak      Merged the Fusion code from Dev branch.
08/11/10   jqi     Remove API featurization between modems and CM/CNE modules.
06/21/10   mp      Disabled all cm_mm_client_xxxx_pwr_collapse_reg () APIs to
                   return
                   CM_CLIENT_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE
05/21/10   ak      Mainlining memory optimization changes.
04/29/10   rm      Adding Modem Statistics changes for 1x/HDR
04/23/10   ak      Memory optimization updates.
03/17/10   am      Fixing the compilation Warning
10/29/09   sg      Add non-demand paging macro to function that are not to be
                   demand-paged.
10/22/09   rm      Adding Modem Statistics changes
10/14/09   sg      Do not use "tmc_heap"/gs_free() if feature
                   FEATURE_MODEM_HEAP defined in the build.
09/23/09   sv      KW Fixes
08/21/09   rn      Check NULL client ptr before call_info_list_get processing
05/26/09   mp      Updated SUPS event notification in SUSPEND state
04/02/09   sv      Updated MBMS event notification for apps power collapse
12/10/08   jd      Fix compilation warnings
11/14/08   jd      Fix for Lint Errors
09/11/08   rn      bcmcs_info_ptr checks to remove lint warning
08/28/08   aj      client_ptr NULL checks in event_ntfy APIs
07/28/08   rm      Fixing compiler warnings
07/01/08   rm      Mainlining of MBMS changes
06/26/08   ak      Fix for compiler warnings
06/11/08   sv      Updated bit_is_any_bit_on to report correct result
                   Updated power collapse buffered events notification
                   to send the events, instead of queuing a command
05/27/08   sv      Add new power collapse bitmask, to buffer any events
                   during Power Collapse State
03/31/08   ks      Add new CM client critical section
03/26/08   ks      Avoid negative value used as an index of cmclient_map_arr
                   array
06/14/07   pk/cl   Added support for power suspend
04/03/07   sk      Made changes to asynchronously free the client object and
                   and remove it from the client list.
03/29/07   ic      In cm_client_init() replaced INRANGE() with BETWEEN()
03/12/07   rp      Fixing compilation issue with nas_power_collapse_bit_arr
03/09/07   sk      RAM usage reduction.
03/07/07   jqi     Added support for FEATURE_SMS_OVER_IP
02/18/07   rp      Added support for WMS<->CM<->NAS interface.
11/07/06   sk      Memory reduction effort.
07/17/06   ic      Print ID and type of the client to whom CM sends an event.
                   For consistency, moved setting of requesting_client_id from
                   cmclient_list_call_event_ntfy() to
                   cm_client_call_event_ntfy(). Added setting of
                   requesting_client_id for BCMCS and NAS events
03/15/06   sk      Decoupled client type and client id.
                   Added support for multiple client ids per client type.
                   Client objects allocated from TMC when all static buffers
                   are used up.
03/13/06   sk      Added two new client types:
                   - CM_CLIENT_TYPE_GSDI
                   - CM_CLIENT_TYPE_BREW_APPS
03/09/06   pk      Added support for POWER COLLAPSE
02/02/06   pk      Corrected the client id for CM_CLIENT_TYPE_PBM
01/12/06   ic      For specific calls / walks Lint warnings, replaced global
                   -fsp comment with -e{error#} comments where needed
01/06/06   ka      Adding new client type CM_CLIENT_TYPE_PBM.
01/06/06   ic      Lint cleanup
12/10/05   ic      Lint cleanup
12/01/05   ic      In cmclient_get_client_ptr() do not search for client
                   structure for commands queued with CM_CLIENT_ID_ANONYMOUS
                   and not flag an error when such client id is passed into
                   the function
11/18/05   ic      Added CM_CLIENT_TYPE_OEMTELEPHONE, clients to check for
                   compile flag CM_API_CLIENT_TYPE_OEMTELEPHONE
11/07/05   ic      Added CM_CLIENT_TYPE_MFLO, clients to check for compile
                   flag CM_API_CLIENT_TYPE_MFLO
11/03/05   ic      Added CM_ASSERT() for ptr function parameters and lint
                   comments for 613 Lint error
09/14/05   pk      Removed CMCLIENT_POOL_SIZE definition
09/13/05   pk      Added CM_CLIENT_TYPE_DS_BCMCS client type
02/09/10   dk      Added RM_INTERFACE_CLIENT type.
01/07/05   ic      Removed all code under and referring to FEATURE_UASMS
12/05/04   ic      Added cmclient_get_client_ptr_by_type()
12/03/04   ws      Support for Remote WMS.
11/15/04   ic      Lint cleanup with CM_DEBUG on
08/12/04   ws      Remove cm20 support.
08/02/04   ic      CM Lint clean up - cmclient.c with Lint supression verified
                   and free of Lint issues in all CUTE configurations.
07/26/04   ic      Changed include file order to customer.h, then cm.h then
                   cmi.h
07/21/04   sj      Added support for umts to 1x handover.
07/20/04   ic      Fixed ADS 1.2 compiler warning
07/14/04   ws      Add client id in event generations.
07/12/04   ic      Removed assertions from cmclient_get_client_ptr() so I can
                   test in CUTE whether CM reports appropriate error for
                   invalid client id.
06/08/04   ank     Mainlined FEATURE_GPSONE_DBM.
06/04/04   ka      Fixed Lint errors
06/02/04   ws      Added camera as a client type.
03/18/04   sj      Reset the init_mask when queues are initialized.
03/15/04   ka      Merged changes from 6250 branch
                   Added checks to prevent double client init.
03/02/04   ic      Added BCMCS support (FEATURE_BCMCS)
02/25/04   ic      Changes to make the code pass unit test cases and at the
                   same time make calls in Saber build
02/06/04   ws      Initial jaguar/mainline merge.
11/20/03   ic      Added support for:
                   CM Commands:
                   - CM_PD_CMD_FIX_START
                   - CM_PD_CMD_FIX_END
                   CM Events:
                   - CM_PD_EVENT_FIX_START
                   - CM_PD_EVENT_FIX_END
                   Log events:
                   - EVENT_CM_BLOCK_HDR_ORIG_DURING_GPS
                   - EVENT_CM_ALLOW_HDR_ORIG_DURING_GPS
09/30/03   vt      Modified to support common cm.h across all targets.
05/15/03   ic      Removed FEATURE_MONITOR / FEATURE_JCDMA_MONITOR
04/28/03   AT      Lint cleanup.
04/11/03   prk     Removed references to FEATURE_SUPS.
02/27/03   at      Added support for CM_CLIENT_TYPE_GSTK.
02/19/03   ATM     Added DIAG client
01/29/03   AT      Added support for CAT_APP type
01/21/03   ws      Updated copyright information for 2003
11/14/02   vt      Merged changes from GW mainline.
11/13/02   ic      More changes to monitor object interface - added
                   CM_MONIT_CMD_CLIENT_ACT command
11/13/02   ic      Changes to monitor object interface.
10/30/02   vt      Fixed compiler errors.
10/22/02   ic      Fixed compile error under FEATURE_MONITOR
                   (a field changed from client to client_id)
09/12/02   kar     Fixed compile error
09/10/02   ws      Added initial cm20 backward compatibility.
08/30/02   vt      Made sms, pd, monit, & sups registration functions return
                   error status if proper fearures are not defined.
08/06/02   HQ      Added FEATURE_CDSMS.
05/05/02   vt      Fixed compile problem with new mm cm.h
04/22/02   RI      Added ATCOP as CM client.
04/17/02   prk     Removed call to cm_cmd_client_act in function
                   cm_client_act. Client now needs to call
                   cm_ph_cmd_get_ph_info to get ph info.
02/13/02   ic      Compile errors fixes related to the fact that call_id field
                   was removed from cm_inband_cmd_s_type (not needed, we use
                   call_id field in cm_inband_cmd__info_s_type instead).
                   Removed const modifier from
                   cm_inband_cmd_s_type  *inband_cmd_ptr
                   function parameter (ripple effect of removing call_id from
                   cm_inband_cmd_s_type structure)
01/18/02   ic      Merged MSM_CM.05.03.33 on top of tip CCS CM
                   Diffed MSM_CM.05.03.32 with MSM_CM.05.03.33 and only the
                   changes between those two were merged with CCS CM
12/13/01   ic      Merged MSM_CM.05.03.32 onto SVD CM tip.
                   CCS CM code started from CM VU MSM_CM.05.03.25 + WCDMA/GSM
                   stuff from SIMBA team merged on top of it.
                   We diff'ed MSM_CM.05.03.25 with MSM_CM.05.03.32
                   (CM VU in N*2165) that we needed to merge with and changes
                   between those two VUs were merged onto CCS CM tip.
10/23/01   VT/IC   Baseline for CCS
10/18/01   PRK     Added changes for WCDMA/GSM mode.
                   Added parameter in call to cm_cmd_client_act due to
                   changes in serving system group (FEATURE_NEWSS).
10/15/01   RI      Added support for Supplementary Services ( GSM/WCDMA ).
08/23/01   HD      Lint clean up.
08/21/01   ic      Main-lined FEATURE_NEWCM
04/27/01   HD      Added changes for WCDMA/GSM mode.
03/27/01   RC      Remove CM_ASSERT(FALSE) form cmclient_get_client_ptr.
03/13/01   VT      Added 3 more floating clients.
02/28/01   HQ      Fixed complier warnings.
02/26/01   sko     Added monitor call back entry initialization.
02/23/01   HQ      Fixed problem of PD event registration.
01/29/01   HQ      Properly feature-ized UASMS, GPSONE_DBM and NEWCM.
01/23/01   ych     Merged JCDMA features.
07/27/00   PJ      Added support for PD commands/events. (FEATURE_GPSONE_DBM).
08/23/00   vt      Added new CM interface changes.
08/31/99   SH      Modified codes to featurize UASMS
08/06/99   SH      Added SMS support
07/27/99   SH      Removed using sel_ptr in the q_link_type to access client
                   data structure
06/30/99   SH      Modified cm_client_init to allocate memory for client
06/21/99   SH      Added return value to cm_client_act and cm_client_deact
05/31/99   SH      Added CM 2.0 features
01/07/99   RC      Changes per ARM compiler.
11/24/98   RC      Initial release.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "mmcp_variation.h"
#include "customer.h"
#include "comdef.h"    /* Definition for basic types and macros */
#include "queue.h"     /* Interface to queue services */
#include "err.h"       /* Interface to error log services */
#include "msg.h"       /* Interface to F3 messages services */
#include "rex.h"       /* Interface to rex services */
#include "cmclient.h"  /* Interface to this file */
#include "cmdbg.h"     /* Interface to CM debug services */
#include "cmph.h"      /* Interface to the Phone object */
#include "cmsms.h"     /* Interface to SMS object */
#include "cmxdbm.h"    /* Interface to DBM object */
#include "cmsups.h"   /* Interface to SUPS object */
#include "cmtaski.h"   /* Interface to signal definitions */
#include "cm_v.h"

#ifdef CM_DEBUG
#error code not present
#endif


/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/*---------------------------------------------------------------------------
                             FREE CMCLIENT QUEUE
---------------------------------------------------------------------------*/

#define CM_CLIENT_IDS_MAX ((int) CM_CLIENT_TYPE_MAX + 40)


/*---------------------------------------------------------------------------
                             CLIENT ID TO PTR MAP LIST
---------------------------------------------------------------------------*/

typedef struct cm_client_map_s {

    cm_client_type_e_type          client_type;
        /* Client Type */

    cm_client_s_type               *client_ptr;
        /* Pointer to client object */

    cm_client_entry_status_e_type  entry_status;
        /* Gives status of cmclient_map_arr[] entries */

} cm_client_map_s_type;

/* The client id is nothing but the index of the array. It is included in
** the table to accomodate a more sophisticated scheme in the future.
*/

/* in_CS for All tasks: Array only in critical section, not client objects. */
static cm_client_map_s_type cmclient_map_arr[ CM_CLIENT_IDS_MAX ];

/* Macro to check if the power collapse notify preference of a client equals
** CM_PWR_COLLAPSE_NOTIFY_NULL. For targets without power collapse feature,
** this evaluates to FALSE (so as to not affect any || conditions ).
*/
#ifdef FEATURE_APPS_POWER_COLLAPSE
#define IS_PWR_COLLAPSE_NTFY_PREF_NULL(x) ((x)==CM_PWR_COLLAPSE_NOTIFY_NULL)
#else
#define IS_PWR_COLLAPSE_NTFY_PREF_NULL(x) (/*lint -e{506,774,831} */ FALSE)
#endif

/*===========================================================================

FUNCTION cmclient_map_init

DESCRIPTION
  Initialize the client map array (cmclient_map_arr).

  This function must be called before the cmclient_map_arr can be used.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmclient_map_init( void )
{
  unsigned int i;

  CM_MSG_LOW_1("START cmclient_map_init(), free client maps=%d",
                 ARR_SIZE(cmclient_map_arr));

  for(i = 0; i < ARR_SIZE(cmclient_map_arr); i++)
  {
    cmclient_map_arr[i].client_type = CM_CLIENT_TYPE_NONE;
    cmclient_map_arr[i].client_ptr  = NULL;
    cmclient_map_arr[i].entry_status = CM_CLIENT_ENTRY_UNUSED;
  }

}

/*===========================================================================

FUNCTION cmclient_get_client_buf

DESCRIPTION
  Allocate a client object.

  This function must be called after cmclient_free_q_init is called.

DEPENDENCIES
  none

RETURN VALUE
  Address to allocated object, or NULL if out of memory.

SIDE EFFECTS
  none

===========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif
static cm_client_s_type* cmclient_get_client_buf( void )
{
  cm_client_s_type *client_ptr = NULL;

  /* Allocate memory from modem heap */
  client_ptr =  cm_mem_malloc(sizeof(cm_client_s_type));

  return client_ptr;
}
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif

/*===========================================================================

FUNCTION cmclient_free_client_buf

DESCRIPTION
  Free a client object.

  This function must be called after cmclient_free_q_init is called.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif
void cmclient_free_client_buf( cm_client_s_type *client_ptr )
{
  CM_ASSERT( client_ptr != NULL );

  /* Deallocate the memory to heap */
  cm_mem_free(client_ptr);
}
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif

/*---------------------------------------------------------------------------
                               BIT ARRAY
---------------------------------------------------------------------------*/

/*lint -save -e702 -e734 */

/*===========================================================================


FUNCTION bit_arr_set_bits

DESCRIPTION
  Takes a pointer to a bit array (bit_arr_ptr), a starting bit position
  (start_bit) and an ending bit position (end_bit) and set to ON or OFF
  all the bits between the start_bit and the end_bit, including the start
  and the end bit positions.

  Note that bits are ordered from left to right; that is, bit 0 is
  the most left bit of the bit array.

DEPENDENCIES
  Specified bit array must have already been initialized with
  bit_arr_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
#ifdef FEATURE_CM_APPS_POWER_COLLAPSE
#error code not present
#endif // FEATURE_APPS_POWER_COLLAPSE

/*===========================================================================


FUNCTION bit_arr_set_bits

DESCRIPTION
  Takes a pointer to a bit array (bit_arr_ptr), a starting bit position
  (start_bit) and an ending bit position (end_bit) and set to ON or OFF
  all the bits between the start_bit and the end_bit, including the start
  and the end bit positions.

  Note that bits are ordered from left to right; that is, bit 0 is
  the most left bit of the bit array.

DEPENDENCIES
  Specified bit array must have already been initialized with
  bit_arr_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif
static void bit_arr_set_bits(

    bit_arr_s_type  *bit_arr_ptr,
        /* pointer to a bit array item */

    boolean         is_set_bits_on,
        /* if TRUE set bits to ON, else set bits to OFF */

    int             start_bit,
        /* start from this bit, including */

    int             end_bit
        /* end at this bit, including */
)
/*lint -esym(818, bit_arr_ptr)
** Pointer parameter 'bit_arr_ptr' (line 357) could be declared as pointing to const
** ... yeah right
*/
{
  int i;

  /* Two mask tables to isolate any contiguous stream of bits
  ** with in a byte.
  **
  ** E.g. to isolate bits 2-5 do start_mask_tbl[2] & end_mask_tbl[5]
  ** which is equal to (0011:1111 & 1111:1100) which is equal to 0011:1100.
  */
  static const byte start_mask_tbl[] =
                          { 0xFF, 0x7F, 0x3F, 0x1F, 0x0F, 0x07, 0x03, 0x01 };

  static const byte end_mask_tbl[] =
                          { 0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xFE, 0xFF };

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Mask to mask 3 LSB's - i.e. 3 most right bits
  */
  #define LSB3_MASK  (0x07)

  /* Get the start and end byte bases.
  */
  const byte start_base = start_bit >> 3;
  const byte end_base   = end_bit   >> 3;

  /* Get the start and end bits offsets.
  */
  const byte start_offset = start_bit & LSB3_MASK;
  const byte end_offset   = end_bit   & LSB3_MASK;

  /* Get the start and end masks.
  */
  const byte start_mask = start_mask_tbl[start_offset];
  const byte end_mask   = end_mask_tbl  [end_offset];

  byte *arr_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check some assertions.
  */
  CM_ASSERT( bit_arr_ptr != NULL );
  CM_ASSERT( bit_arr_ptr->bit_buf != NULL );
  CM_ASSERT( 0         <= start_bit  );
  CM_ASSERT( start_bit <= end_bit);
  CM_ASSERT( end_bit   < bit_arr_ptr->bit_size );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the start and bits are within allowable range.
  */
  if( ! (0         <= start_bit             &&
         start_bit <= end_bit               &&
         end_bit   <  bit_arr_ptr->bit_size) )
  {
    CM_ERR_FATAL_3("Bits out of range, s_bit=%d, e_bit=%d, size=%d",
               start_bit, end_bit, bit_arr_ptr->bit_size );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Point a byte pointer at the actual bit buffer, so we do not
  ** need to make any assumption of the bit buffer type.
  */
  arr_ptr = (byte*) bit_arr_ptr->bit_buf;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we need to set bits to ON, do the following:
  */
  if( is_set_bits_on )
  {
    /* If stream of bits to set spans over more than one byte,
    ** (e.g. bits 6-12), do the following:
    */
    if( start_base < end_base )
    {
      arr_ptr[start_base] |= start_mask;
      for( i=start_base+1; i<end_base; i++ ) arr_ptr[i] = 0xFF;
      arr_ptr[end_base]   |= end_mask;
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Else, stream of bits to set contained with in a single byte
    ** (e.g. 2-6) - do the following:
    */
    else
    {
      arr_ptr[start_base] |= ( start_mask & end_mask );
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Else (we need to set bits to OFF), do the following:
  */
  else
  {
    /* If stream of bits to set spans over more than one byte,
    ** (e.g. bits 6-12), do the following:
    */
    if( start_base < end_base )
    {
      arr_ptr[start_base] &= ~start_mask;
      for( i=start_base+1; i<end_base; i++ ) arr_ptr[i] = 0x00;
      arr_ptr[end_base]   &= ~end_mask;
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Else, stream of bits to set contained with in a single byte
    ** (e.g. 2-6) - do the following:
    */
    else
    {
      arr_ptr[start_base] &= ~( start_mask & end_mask );
    }
  }
}
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif
/*lint +esym(818, bit_arr_ptr) */

/*===========================================================================

FUNCTION bit_arr_is_bit_on

DESCRIPTION
  Takes a pointer to a bit array (bit_arr_ptr) and a bit position
  (bit) and check whether the bit in this position is ON.

  Note that bits are ordered from left to right; that is, bit 0 is
  the most left bit of the bit array.

DEPENDENCIES
  Specified bit array must have already been initialized with
  bit_arr_init().

RETURN VALUE
  TRUE if the bit in the specified position is ON, else return FALSE.

SIDE EFFECTS
  none

===========================================================================*/
static boolean bit_arr_is_bit_on(

    const bit_arr_s_type    *bit_arr_ptr,
        /* pointer to a bit array item */

    int                     bit
        /* indicating which bit to check */
)
{

  /* Mask table to isolate any specific bit with in a byte.
  **
  ** E.g. to isolate bit 2 do mask_tbl[2] which is equal to 0010:0000
  */
  static const byte bit_mask_tbl[] =
                          { 0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01 };

  /* Get the byte base.
  */
  const byte base = bit >> 3;

  /* Get the bit offset
  */
  const byte offset = bit & LSB3_MASK;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( bit_arr_ptr != NULL );
  CM_ASSERT( bit_arr_ptr->bit_buf != NULL );
  CM_ASSERT( bit >= 0 && bit < bit_arr_ptr->bit_size );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the specified bit is within allowable range.
  */
  if( bit < 0 || bit >= bit_arr_ptr->bit_size )
  {
    sys_err_fatal_invalid_value_exception(bit);
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  else
  {
    /* Point a byte pointer at the actual bit buffer, so we do not
    ** need to make any assumption of the bit buffer type.
    */
    byte *arr_ptr = (byte*) bit_arr_ptr->bit_buf;

    /* Return TRUE if specified bit is ON.
    */
    if( arr_ptr[ base ] & bit_mask_tbl[ offset ] )
    {
      return TRUE;
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, the bit is OFF -- return FALSE.
  */
  return FALSE;

}
#ifdef FEATURE_APPS_POWER_COLLAPSE
/*===========================================================================

FUNCTION bit_arr_is_any_bit_on

DESCRIPTION
  Takes a pointer to a bit array (bit_arr_ptr) and
  check whether any bit is ON.

  Note that bits are ordered from left to right; that is, bit 0 is
  the most left bit of the bit array.

DEPENDENCIES
  Specified bit array must have already been initialized with
  bit_arr_init().

RETURN VALUE
  TRUE if the bit in the specified position is ON, else return FALSE.

SIDE EFFECTS
  none

===========================================================================*/
/*lint -esym(528, bit_arr_is_any_bit_on) bit_arr_is_any_bit_on() not
** referenced. It's not called in some configurations
** as it is currently only used under Power collapse feature
*/
static boolean bit_arr_is_any_bit_on(

    const bit_arr_s_type    *bit_arr_ptr,
        /* pointer to a bit array item */

    int             start_bit,
        /* start from this bit, including */

    int             end_bit
        /* end at this bit, including */
)
{
  int     i;
  boolean is_any_bit_on = FALSE;

  /* Two mask tables to isolate any contiguous stream of bits
  ** with in a byte.
  **
  ** E.g. to isolate bits 2-5 do start_mask_tbl[2] & end_mask_tbl[5]
  ** which is equal to (0011:1111 & 1111:1100) which is equal to 0011:1100.
  */
  static const byte start_mask_tbl[] =
                          { 0xFF, 0x7F, 0x3F, 0x1F, 0x0F, 0x07, 0x03, 0x01 };

  static const byte end_mask_tbl[] =
                          { 0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xFE, 0xFF };

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Mask to mask 3 LSB's - i.e. 3 most right bits
  */
  #ifndef LSB3_MASK
  #define LSB3_MASK  (0x07)
  #endif /* LSB3_MASK */

  /* Get the start and end byte bases.
  */
  const byte start_base = start_bit >> 3;
  const byte end_base   = end_bit   >> 3;

  /* Get the start and end bits offsets.
  */
  const byte start_offset = start_bit & LSB3_MASK;
  const byte end_offset   = end_bit   & LSB3_MASK;

  /* Get the start and end masks.
  */
  const byte start_mask = start_mask_tbl[start_offset];
  const byte end_mask   = end_mask_tbl  [end_offset];

  byte *arr_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check some assertions.
  */
  CM_ASSERT( bit_arr_ptr != NULL );
  CM_ASSERT( bit_arr_ptr->bit_buf != NULL );
  CM_ASSERT( 0         <= start_bit  );
  CM_ASSERT( start_bit <= end_bit);
  CM_ASSERT( end_bit   < bit_arr_ptr->bit_size );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the start and bits are within allowable range.
  */
  if( ! (0         <= start_bit             &&
         start_bit <= end_bit               &&
         end_bit   <  bit_arr_ptr->bit_size) )
  {
    CM_ERR_FATAL_3("Bits out of range, s_bit=%d, e_bit=%d, size=%d",
               start_bit, end_bit, bit_arr_ptr->bit_size );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Point a byte pointer at the actual bit buffer, so we do not
  ** need to make any assumption of the bit buffer type.
  */
  arr_ptr = (byte*) bit_arr_ptr->bit_buf;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If stream of bits to set spans over more than one byte,
  ** (e.g. bits 6-12), do the following:
  */
  if( start_base < end_base )
  {
    if(arr_ptr[start_base] & start_mask)
    {
      is_any_bit_on = TRUE;
    }

    for( i=start_base+1; i<end_base; i++ )
    {
      if(arr_ptr[i])
      {
       is_any_bit_on = TRUE;
      }
    }

    if(arr_ptr[end_base] & end_mask)
    {
      is_any_bit_on = TRUE;
    }
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Else, stream of bits to set contained with in a single byte
  ** (e.g. 2-6) - do the following:
  */
  else
  {
    if(arr_ptr[start_base] & ( start_mask & end_mask ))
    {
      is_any_bit_on = TRUE;
    }
  }

  return is_any_bit_on;
}
/*lint -restore */
#endif /* FEATURE_APPS_POWER_COLLAPSE */

/*===========================================================================

FUNCTION bit_arr_init

DESCRIPTION
  Initializing a bit array object.

  This function must be called on a bit array object before
  it is being used, in any way, place, or form.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif
static void bit_arr_init(

    bit_arr_s_type  *bit_arr_ptr,
        /* pointer to a bit array to be initialized */

    byte            *bit_buf_ptr,
        /* pointer to a bit buffer */

    int             bit_size
        /* bit size of bit buffer */
)
{

  CM_ASSERT( bit_arr_ptr != NULL );
  CM_ASSERT( bit_buf_ptr != NULL );
  CM_ASSERT( bit_size    > 0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Set bit_buf to point at supplied bit buffer,
  ** and set bit_size to supplied buffer size.
  */
  bit_arr_ptr->bit_buf  = bit_buf_ptr;
  bit_arr_ptr->bit_size = bit_size;

  /* Set all bits off.
  */
  bit_arr_set_bits( bit_arr_ptr, /* for this bit array */
                    FALSE,       /* set bits to OFF */
                    0,           /* start from bit 0 */
                    bit_size-1); /* end with the last bit */
}
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif



#ifdef CM_DEBUG
#error code not present
#endif /* CM_DEBUG */



/*---------------------------------------------------------------------------
                                  CM CLIENT
---------------------------------------------------------------------------*/

/*lint -save -e574 -e737 */

/*===========================================================================

FUNCTION cm_client_init

DESCRIPTION
  Initializing a client object.

  This function can be called more than once with the same client type. Each
  call will result in an ADDITIONAL client id being allocated for the client
  type. It's upto the caller to keep track of the several client ids that
  they own and use them appropriately.

  Note that in order to get notified of CM events a client still needs
  to do the following:
  1. Register its callback functions and specific events with each functional group
  2. Activate its client object with cm_client_act()

DEPENDENCIES
  none

RETURN VALUE
  request staus

SIDE EFFECTS
  none

===========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif
cm_client_status_e_type cm_client_init(
    cm_client_type_e_type           client_type,
            /* Client type */
    cm_client_id_type               *client_id_ptr
            /* Client ID pointer*/
)
{
  int client_id = CM_CLIENT_ID_ANONYMOUS;
  cm_client_s_type  *client_ptr = NULL;

  /*----------------------------------------------------------------------*/

  CM_ASSERT(INRANGE(client_type, CM_CLIENT_TYPE_NONE, CM_CLIENT_TYPE_MAX));
  CM_ASSERT( client_type != CM_CLIENT_TYPE_END_LEGACY );

  if ( client_id_ptr == NULL )
  {
      return CM_CLIENT_ERR_CLIENT_ID_PTR;
  }

  /* The same client type can now be used to register several times. Each
  ** registration will result in an additional client id being allocated for
  ** the client type.
  */

  /* If the client type is one of the legacy ones, try to get the client id
  ** from those that are reserved for the legacy client types.
  */
  rex_enter_crit_sect(cmclient_crit_sect_ptr());
  /* Serialize data access cmclient_map_arr[] only */

  if(BETWEEN(client_type, CM_CLIENT_TYPE_NONE, CM_CLIENT_TYPE_END_LEGACY) &&
     cmclient_map_arr[client_type].entry_status == CM_CLIENT_ENTRY_UNUSED)
  {
    client_id = (int) client_type;
    cmclient_map_arr[client_id].entry_status = CM_CLIENT_ENTRY_INACTIVE;
  }

  /* If the previous attempt at finding a client id fails, search for client
  ** ids from those that are not reserved for the legacy client types.
  */
  if(client_id == CM_CLIENT_ID_ANONYMOUS)
  {
    int i;
    for (i = (int) CM_CLIENT_TYPE_END_LEGACY;
        i < ARR_SIZE(cmclient_map_arr);
        i++)
    {
        /* If a free slot is found in the client map array.
        */
        if ( cmclient_map_arr[i].entry_status == CM_CLIENT_ENTRY_UNUSED)
        {
            client_id = i;
            cmclient_map_arr[client_id].entry_status = CM_CLIENT_ENTRY_INACTIVE;
            break;
        }
    }
  }

  rex_leave_crit_sect(cmclient_crit_sect_ptr());

  if(client_id == CM_CLIENT_ID_ANONYMOUS)
  {
      return  CM_CLIENT_ERR_CLIENT_INIT_FAIL;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  client_ptr = cmclient_get_client_buf();
  
  /* Return the call id to clients */
  *client_id_ptr = client_id;

  /* Store the client id in client object for ease accessing client id */
  client_ptr->client_id = client_id;
  
  /* Mark this object as initialized.
  */
  client_ptr->init_mask = CM_INIT_MASK;
 
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 
  /* Initialize call bit array.
  */
  bit_arr_init( &client_ptr->call_bit_arr,       /* call bit array struct */
                client_ptr->call_bit_buf,                   /* bit buffer */
                (8 * ARR_SIZE(client_ptr->call_bit_buf)) ); /* bit size   */
 
  /* Initialize phone bit array.
  */
  bit_arr_init( &client_ptr->ph_bit_arr,        /* phone bit array struct */
                client_ptr->ph_bit_buf,                     /* bit buffer */
                (8 * ARR_SIZE(client_ptr->ph_bit_buf)) );   /* bit size   */
 
  /* Initialize serving system bit array.
  */
  bit_arr_init( &client_ptr->ss_bit_arr, /*serving system bit array struct*/
                client_ptr->ss_bit_buf,                     /* bit buffer */
                (8 * ARR_SIZE(client_ptr->ss_bit_buf)) );   /* bit size   */
 
  /* Initialize inband bit array.
  */
  bit_arr_init( &client_ptr->inband_bit_arr, /*inband bit array struct*/
                client_ptr->inband_bit_buf,                     /* bit buffer */
                (8 * ARR_SIZE(client_ptr->inband_bit_buf)) );   /* bit size   */
 
 /* Initialize SMS bit array.
  */
  bit_arr_init( &client_ptr->sms_bit_arr, /*sms bit array struct*/
                client_ptr->sms_bit_buf,                     /* bit buffer */
                (8 * ARR_SIZE(client_ptr->sms_bit_buf)) );   /* bit size   */
 
  #ifdef FEATURE_GPSONE
  /* Initialize the PD bit array
   */
  bit_arr_init( &client_ptr->pd_bit_arr, /* PD bit array struct */
                client_ptr->pd_bit_buf,                      /* bit buffer */
                (8 * ARR_SIZE(client_ptr->pd_bit_buf)) );    /* bit size   */
 
  #endif /* FEATURE_GPSONE */
 
  /* Initialize the DBM event bit array
   */
  bit_arr_init( &client_ptr->dbm_event_bit_arr, /* DBM event bit array struct */
                client_ptr->dbm_event_bit_buf,               /* bit buffer */
                (8 * ARR_SIZE(client_ptr->dbm_event_bit_buf)) ); /* bit size   */
 
  /* Initialize the DBM class bit array
   */
  bit_arr_init( &client_ptr->dbm_class_bit_arr, /* DBM class bit array struct */
                client_ptr->dbm_class_bit_buf,               /* bit buffer */
                (8 * ARR_SIZE(client_ptr->dbm_class_bit_buf)) ); /* bit size   */
 
 
  #ifdef CM_GW_SUPPORTED
 
  /* Initialize the Supplementary Service bit array */
 
  bit_arr_init( &client_ptr->sups_bit_arr,    /* Suppl Serv bit array struct */
                  client_ptr->sups_bit_buf,                      /* bit buffer */
                  (8 * ARR_SIZE(client_ptr->sups_bit_buf)) ); /* bit size */
 
  #endif /* FEATURE_GSM || FEATURE_WCDMA */
 
  #if ((defined FEATURE_BCMCS) || (defined FEATURE_MBMS))
  /* Initialize BCMCS bit array.
  */
  bit_arr_init( &client_ptr->bcmcs_bit_arr,         /* BCMCS bit array struct */
                client_ptr->bcmcs_bit_buf,                     /* bit buffer */
                (8 * ARR_SIZE(client_ptr->bcmcs_bit_buf)) );   /* bit size   */
 
  #endif /* FEATURE_BCMCS || MBMS */
 
  #ifdef CM_GW_SUPPORTED
  /* Initialize the Supplementary Service bit array */
 
  bit_arr_init( &client_ptr->sups_bit_arr,    /* Suppl Serv bit array struct */
                  client_ptr->sups_bit_buf,                      /* bit buffer */
                  (8 * ARR_SIZE(client_ptr->sups_bit_buf)) ); /* bit size */
 
  /* Initialize the MBMS bit array */

  #ifdef FEATURE_WMS_CM_NAS_INTERFACES
  #error code not present
#endif

  #if (defined (FEATURE_MBMS))
 #error code not present
#endif

  #endif   /* FEATURE_GSM || FEATURE_WCDMA */

  
  /* Initialize AC bit array.
  */
  bit_arr_init( &client_ptr->ac_bit_arr,        /* AC bit array struct */
                client_ptr->ac_bit_buf,                     /* bit buffer */
                (8 * ARR_SIZE(client_ptr->ac_bit_buf)) );   /* bit size   */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 
  #ifdef FEATURE_APPS_POWER_COLLAPSE
  /* Initialize power collapse call bit array.
  */
  bit_arr_init( &client_ptr->call_power_collapse_bit_arr,       /* call bit array struct */
                client_ptr->call_power_collapse_bit_buf,                   /* bit buffer */
                (8 * ARR_SIZE(client_ptr->call_power_collapse_bit_buf)) ); /* bit size   */
 
  /* Initialize power collapse call event buf bit array.
  */
  bit_arr_init( &client_ptr->call_power_collapse_buf_bit_arr,    /* call bit array struct */
                client_ptr->call_power_collapse_buf_bit_buf,                   /* bit buffer */
                (8 * ARR_SIZE(client_ptr->call_power_collapse_buf_bit_buf)) ); /* bit size   */
 
  client_ptr->call_power_collpase_notify_pref = CM_PWR_COLLAPSE_NOTIFY_NULL;
 
  /* Initialize power collapse phone bit array.
  */
  bit_arr_init( &client_ptr->ph_power_collapse_bit_arr,       /* ph bit array struct */
                client_ptr->ph_power_collapse_bit_buf,                   /* bit buffer */
                (8 * ARR_SIZE(client_ptr->ph_power_collapse_bit_buf)) ); /* bit size   */
 
  /* Initialize power collapse ph event buf bit array.
  */
  bit_arr_init( &client_ptr->ph_power_collapse_buf_bit_arr,    /* ph bit array struct */
                client_ptr->ph_power_collapse_buf_bit_buf,                   /* bit buffer */
                (8 * ARR_SIZE(client_ptr->ph_power_collapse_buf_bit_buf)) ); /* bit size   */
 
  client_ptr->ph_power_collpase_notify_pref = CM_PWR_COLLAPSE_NOTIFY_NULL;
 
  /* Initialize power collapse Serving system bit array.
  */
  bit_arr_init( &client_ptr->ss_power_collapse_bit_arr,       /* ss bit array struct */
                client_ptr->ss_power_collapse_bit_buf,                   /* bit buffer */
                (8 * ARR_SIZE(client_ptr->ss_power_collapse_bit_buf)) ); /* bit size   */
 
  /* Initialize power collapse ss event buf bit array.
  */
  bit_arr_init( &client_ptr->ss_power_collapse_buf_bit_arr,    /* ss bit array struct */
                client_ptr->ss_power_collapse_buf_bit_buf,                   /* bit buffer */
                (8 * ARR_SIZE(client_ptr->ss_power_collapse_buf_bit_buf)) ); /* bit size   */
 
  client_ptr->ss_power_collpase_notify_pref = CM_PWR_COLLAPSE_NOTIFY_NULL;
 
  /* Initialize power collapse inband bit array.
  */
  bit_arr_init( &client_ptr->inband_power_collapse_bit_arr,  /* inband bit array struct */
                client_ptr->inband_power_collapse_bit_buf,   /* bit buffer */
                (8 * ARR_SIZE(client_ptr->inband_power_collapse_bit_buf)) ); /* bit size*/
 
  /* Initialize power collapse inband event buf bit array.
  */
  bit_arr_init( &client_ptr->inband_power_collapse_buf_bit_arr,    /* inband bit array struct */
                client_ptr->inband_power_collapse_buf_bit_buf,                   /* bit buffer */
                (8 * ARR_SIZE(client_ptr->inband_power_collapse_buf_bit_buf)) ); /* bit size   */
 
  client_ptr->inband_power_collpase_notify_pref = CM_PWR_COLLAPSE_NOTIFY_NULL;
 
  /* Initialize power collapse sms bit array.
  */
  bit_arr_init( &client_ptr->sms_power_collapse_bit_arr,       /* sms bit array struct */
                client_ptr->sms_power_collapse_bit_buf,                   /* bit buffer */
                (8 * ARR_SIZE(client_ptr->sms_power_collapse_bit_buf)) ); /* bit size   */
 
  /* Initialize power collapse sms event buf bit array.
  */
  bit_arr_init( &client_ptr->sms_power_collapse_buf_bit_arr,    /* sms bit array struct */
                client_ptr->sms_power_collapse_buf_bit_buf,                   /* bit buffer */
                (8 * ARR_SIZE(client_ptr->sms_power_collapse_buf_bit_buf)) ); /* bit size   */
 
  client_ptr->sms_power_collpase_notify_pref = CM_PWR_COLLAPSE_NOTIFY_NULL;
 
  #ifdef FEATURE_GPSONE
  /* Initialize power collapse PD bit array.
  */
  bit_arr_init( &client_ptr->pd_power_collapse_bit_arr,       /* pd bit array struct */
                client_ptr->pd_power_collapse_bit_buf,                   /* bit buffer */
                (8 * ARR_SIZE(client_ptr->pd_power_collapse_bit_buf)) ); /* bit size   */
 
  /* Initialize power collapse pd event buf bit array.
  */
  bit_arr_init( &client_ptr->pd_power_collapse_buf_bit_arr,    /* pd bit array struct */
                client_ptr->pd_power_collapse_buf_bit_buf,                   /* bit buffer */
                (8 * ARR_SIZE(client_ptr->pd_power_collapse_buf_bit_buf)) ); /* bit size   */
 
  client_ptr->pd_power_collpase_notify_pref = CM_PWR_COLLAPSE_NOTIFY_NULL;
 
  #endif /* FEATURE_GPSONE */
 
  /* Initialize power collapse dbm bit array.
  */
  bit_arr_init( &client_ptr->dbm_power_collapse_bit_arr,       /* dbm bit array struct */
                client_ptr->dbm_power_collapse_bit_buf,                   /* bit buffer */
                (8 * ARR_SIZE(client_ptr->dbm_power_collapse_bit_buf)) ); /* bit size   */
 
  /* Initialize power collapse dbm event buf bit array.
  */
  bit_arr_init( &client_ptr->dbm_power_collapse_buf_bit_arr,    /* dbm bit array struct */
                client_ptr->dbm_power_collapse_buf_bit_buf,                   /* bit buffer */
                (8 * ARR_SIZE(client_ptr->dbm_power_collapse_buf_bit_buf)) ); /* bit size   */
 
  client_ptr->dbm_power_collpase_notify_pref = CM_PWR_COLLAPSE_NOTIFY_NULL;
 
     
  #ifdef CM_GW_SUPPORTED
 
  /* Initialize power collapse Supplementary services bit array.
  */
  bit_arr_init( &client_ptr->sups_power_collapse_bit_arr,       /* sups bit array struct */
                client_ptr->sups_power_collapse_bit_buf,                   /* bit buffer */
                (8 * ARR_SIZE(client_ptr->sups_power_collapse_bit_buf)) ); /* bit size   */
 
  /* Initialize power collapse sups event buf bit array.
  */
  bit_arr_init( &client_ptr->sups_power_collapse_buf_bit_arr,    /* sups bit array struct */
                client_ptr->sups_power_collapse_buf_bit_buf,                   /* bit buffer */
                (8 * ARR_SIZE(client_ptr->sups_power_collapse_buf_bit_buf)) ); /* bit size   */
 
  client_ptr->sups_power_collpase_notify_pref = CM_PWR_COLLAPSE_NOTIFY_NULL;
 
  #endif /* FEATURE_GSM || FEATURE_WCDMA */
 
  #if ((defined FEATURE_BCMCS) || (defined FEATURE_MBMS))
  /* Initialize power collapse bcmcs bit array.
  */
  bit_arr_init( &client_ptr->bcmcs_power_collapse_bit_arr, /* bcmcs bit array struct */
                client_ptr->bcmcs_power_collapse_bit_buf,             /* bit buffer */
                (8 * ARR_SIZE(client_ptr->bcmcs_power_collapse_bit_buf)) ); /* bit size   */
 
  /* Initialize power collapse bcmcs event buf bit array.
  */
  bit_arr_init( &client_ptr->bcmcs_power_collapse_buf_bit_arr,    /* bcmcs bit array struct */
                client_ptr->bcmcs_power_collapse_buf_bit_buf,                   /* bit buffer */
                (8 * ARR_SIZE(client_ptr->bcmcs_power_collapse_buf_bit_buf)) ); /* bit size   */
 
  client_ptr->bcmcs_power_collpase_notify_pref = CM_PWR_COLLAPSE_NOTIFY_NULL;
 
  #endif /* FEATURE_BCMCS || MBMS */
   
  #ifdef CM_GW_SUPPORTED
  #ifdef FEATURE_WMS_CM_NAS_INTERFACES
  #error code not present
#endif
  #endif   /* FEATURE_GSM || FEATURE_WCDMA */
  #endif /* FEATURE_APPS_POWER_COLLAPSE */
 
  /* Initialize modem stats bit array.
  */
  bit_arr_init( &client_ptr->stats_bit_arr,   /* stats bit array struct */
                client_ptr->stats_bit_buf,   /* bit buffer */
                (8 * ARR_SIZE(client_ptr->stats_bit_buf)) ); /* bit size   */
 
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 
  /* Initially set all callback function pointers
  ** to point to NULL
  */
  client_ptr->call_event_func             = NULL;
  client_ptr->ph_event_func               = NULL;
  client_ptr->subs_event_func             = NULL;
  client_ptr->ss_event_func               = NULL;
  client_ptr->inband_event_func           = NULL;
  client_ptr->sms_event_func              = NULL;
  client_ptr->call_info_list_func         = NULL;
 
  client_ptr->stats_event_func            = NULL;
 
  #ifdef CM_GW_SUPPORTED
  client_ptr->sups_event_func             = NULL;
  #endif
 
  #if (defined FEATURE_BCMCS || defined FEATURE_MBMS)
  client_ptr->bcmcs_event_func            = NULL;
  #endif
 
  client_ptr->call_cmd_err_func           = NULL;
  client_ptr->ph_cmd_err_func             = NULL;
  client_ptr->ss_cmd_err_func             = NULL;
  client_ptr->call_info_list_cmd_err_func = NULL;
  client_ptr->inband_cmd_err_func         = NULL;
  #if (defined FEATURE_BCMCS || defined FEATURE_MBMS)
  client_ptr->bcmcs_cmd_err_func          = NULL;
  #endif
  #ifdef FEATURE_CDSMS
  client_ptr->sms_cmd_err_func            = NULL;
  #endif
  #ifdef FEATURE_GPSONE
  client_ptr->pd_cmd_err_func             = NULL;
  client_ptr->pd_event_func               = NULL;
  #endif /* FEATURE_GPSONE */
 
  client_ptr->dbm_cmd_err_func            = NULL;
  client_ptr->dbm_event_func              = NULL;
 
  #ifdef CM_GW_SUPPORTED
  client_ptr->sups_cmd_err_func           = NULL;
  #endif
 
  client_ptr->stats_cmd_err_func          = NULL;
 
  /* Initialize the power collapse variables
  */
  client_ptr->client_processor            = CM_PROCESSOR_MODEM;

  cmclient_map_arr[client_id].client_type= client_type;
  cmclient_map_arr[client_id].client_ptr = client_ptr;
  
  return CM_CLIENT_OK;
}
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif

/*===========================================================================

FUNCTION cm_client_release

DESCRIPTION
  Release a client object.

  This function should be called if a client doesn't need CM anymore.
  This call will release the client object. That object can be reused for
  another client. If the client needs to use CM again, it should call
  cm_client_init function to establish connection.

DEPENDENCIES
  none

RETURN VALUE
  request staus

SIDE EFFECTS
  none

===========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif
cm_client_status_e_type cm_client_release(

    cm_client_id_type               client_id
        /* Client ID pointer*/
)
{
  cm_client_cmd_s_type  *client_cmd_ptr = NULL;

  /*---------------------------------------------------------------------*/

  if(!BETWEEN(client_id, CM_CLIENT_ID_ANONYMOUS, CM_CLIENT_IDS_MAX))
  {
    CM_MSG_HIGH_1("cm_client_release():Client id %d out of range",
                                                            client_id);
    return CM_CLIENT_OK;
  }

  client_cmd_ptr = cm_cmd_alloc_client_init();

  rex_enter_crit_sect(cmclient_crit_sect_ptr());
  /* Serialize data access cmclient_map_arr[] only */
  if(cmclient_map_arr[client_id].entry_status != CM_CLIENT_ENTRY_UNUSED)
  {
    client_cmd_ptr->client_ptr = cmclient_get_client_ptr(client_id);
    cmclient_map_arr[client_id].entry_status = CM_CLIENT_ENTRY_UNUSED;
    cmclient_map_arr[client_id].client_type = CM_CLIENT_TYPE_NONE;
    cmclient_map_arr[client_id].client_ptr = NULL;
  }
  rex_leave_crit_sect(cmclient_crit_sect_ptr());

  if(client_cmd_ptr->client_ptr == NULL)
  {
    return CM_CLIENT_ERR_CLIENT_ID;
  }

  client_cmd_ptr->client_id = CM_CLIENT_ID_ANONYMOUS;
  client_cmd_ptr->cmd = CM_CLIENT_CMD_RELEASE;
  cm_cmd_queue( (cm_cmd_type*)client_cmd_ptr );

  return CM_CLIENT_OK;
}
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif



/*===========================================================================

FUNCTION cmclient_get_client_ptr

DESCRIPTION

  Given client id, return a pointer to client object

DEPENDENCIES
  None

RETURN VALUE
  A pointer to client object or NULL if not found

SIDE EFFECTS
  None

===========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif
cm_client_s_type* cmclient_get_client_ptr(

    cm_client_id_type client_id
    /* Client Id */
)
{
  cm_client_s_type* client_ptr = NULL;

  if(!BETWEEN(client_id, CM_CLIENT_ID_ANONYMOUS, CM_CLIENT_IDS_MAX))
  {
    return client_ptr;
  }
  
  rex_enter_crit_sect(cmclient_crit_sect_ptr());
  /* Serialize data access cmclient_map_arr[] only */

  if(cmclient_map_arr[client_id].entry_status != CM_CLIENT_ENTRY_UNUSED)
  {
    client_ptr = cmclient_map_arr[client_id].client_ptr;
  }

  rex_leave_crit_sect(cmclient_crit_sect_ptr());


  return client_ptr;

}
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif

/*===========================================================================

FUNCTION cmclient_get_client_ptr_by_type

DESCRIPTION

  Given client type, return a pointer to client object

DEPENDENCIES
  None

RETURN VALUE
  A pointer to client object or NULL if not found

SIDE EFFECTS
  None

===========================================================================*/
cm_client_s_type* cmclient_get_client_ptr_by_type(

    cm_client_type_e_type client_type
    /* Client type */
)
{
  int i;
  cm_client_s_type* client_ptr = NULL;

  /*---------------------------------------------------------------------*/
  rex_enter_crit_sect(cmclient_crit_sect_ptr());
  /* Serialize data access cmclient_map_arr[] only */

  for ( i = 0; i < ARR_SIZE( cmclient_map_arr ); i++ )
  {
      if ( client_type == cmclient_map_arr[i].client_type)
      {
          client_ptr = cmclient_map_arr[i].client_ptr;
          break;
      }
  }
  rex_leave_crit_sect(cmclient_crit_sect_ptr());

  return client_ptr;
}


/*===========================================================================

FUNCTION cmclient_get_client_id

DESCRIPTION
  Returns client id that corresponds to a client object

DEPENDENCIES
  None

RETURN VALUE
  client id

SIDE EFFECTS
  None

===========================================================================*/
cm_client_id_type cmclient_get_client_id(

    const cm_client_s_type *client_ptr
    /* Client object pointer */
)
{
  cm_client_id_type client_id = CM_CLIENT_ID_ANONYMOUS;

  if(client_ptr != NULL)
  {
    client_id = client_ptr->client_id;
  }

  return client_id;
}



/*===========================================================================

FUNCTION cmclient_get_client_type

DESCRIPTION
  Returns client id that corresponds to a client object

DEPENDENCIES
  None

RETURN VALUE
  client id

SIDE EFFECTS
  None

===========================================================================*/
cm_client_type_e_type cmclient_get_client_type(

    cm_client_id_type client_id
    /* Client object pointer */
)
{
  cm_client_type_e_type client_type = CM_CLIENT_TYPE_NONE;
  
  if(BETWEEN(client_id, CM_CLIENT_ID_ANONYMOUS, CM_CLIENT_IDS_MAX ))
  {
    client_type = cmclient_map_arr[client_id].client_type;
  }
  else if(client_id == CM_CLIENT_ID_ANONYMOUS)
  {
    client_type = CM_CLIENT_TYPE_NONE;
  }
  else
  {
    client_type = CM_CLIENT_TYPE_MAX;
  }

  return client_type;
}


/*===========================================================================

FUNCTION cm_client_act

DESCRIPTION
  Activate the client object, so that the client will be notified
  of call manager events (for which it is registered).

DEPENDENCIES
  Client must have already been initialized with cm_client_init().

RETURN VALUE
  request status

SIDE EFFECTS
  Add the client object to the client list.

===========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif
cm_client_status_e_type cm_client_act(

    cm_client_id_type          client_id
    /* Client ID */
)
{
  cm_client_cmd_s_type   *client_cmd_ptr;


  if(!BETWEEN(client_id, CM_CLIENT_ID_ANONYMOUS, CM_CLIENT_IDS_MAX ))
  {
    CM_MSG_HIGH_1("cm_client_act: Invalid client id %d", client_id);
    return CM_CLIENT_ERR_CLIENT_ID;
  }

  cmclient_map_arr[client_id].entry_status = CM_CLIENT_ENTRY_ACTIVE;

  client_cmd_ptr  = cm_cmd_alloc_client_init();
  
  client_cmd_ptr->client_id = client_id;
  client_cmd_ptr->cmd = CM_CLIENT_CMD_ACT;
  client_cmd_ptr->client_ptr = cmclient_get_client_ptr(client_id);

  cm_cmd_queue( (cm_cmd_type*)client_cmd_ptr );
  return CM_CLIENT_OK;
}
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif



/*===========================================================================

FUNCTION cm_client_deact

DESCRIPTION
  Deactivate a client object, so that the client will NOT be notified
  of call manager events (for which it is registered).

DEPENDENCIES
  Client must have already been initialized with cm_client_init().

RETURN VALUE
  request status

SIDE EFFECTS
  Remove the client object from the client list.

===========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif
cm_client_status_e_type cm_client_deact(

    cm_client_id_type          client_id
    /* Client ID */
)
{
  if(!BETWEEN(client_id, CM_CLIENT_ID_ANONYMOUS, CM_CLIENT_IDS_MAX ))
  {
    CM_MSG_HIGH_1("cm_client_deact: Invalid client id %d", client_id);
    return CM_CLIENT_ERR_CLIENT_ID;
  }

  cmclient_map_arr[client_id].entry_status = CM_CLIENT_ENTRY_INACTIVE;

  return CM_CLIENT_OK;
}
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif

/*===========================================================================

FUNCTION cm_client_call_event_ntfy

DESCRIPTION
  Notify a client of a specified call event iff the client is registered
  for this event.

DEPENDENCIES
  Client must have already been initialized with cm_client_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cm_client_call_event_ntfy(

    cm_client_s_type           *client_ptr,
        /* pointer to a client struct */

    cm_call_event_e_type       call_event,
        /* notify client of this call event */

    cm_mm_call_info_s_type     *call_info_ptr
        /* pointer to call state information struct */
)
/*lint -esym(818, client_ptr)
** Pointer parameter 'client_ptr' could be declared as pointing to const
** ... yeah right
*/
{
  if (client_ptr == NULL)
  {
    CM_MSG_HIGH_0("client_ptr NULL");
    return;
  }
  CM_ASSERT( BETWEEN(call_event, CM_CALL_EVENT_NONE, CM_CALL_EVENT_MAX) );
  CM_ASSERT( call_info_ptr != NULL );

  /* Make sure this object was properly initialized.
  */
  CM_ASSERT( client_ptr->init_mask == CM_INIT_MASK );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the apps processor is in Power collapse state, or if this is modem client
  ** check if the client
  ** is registered for this event, if the client is registered for this event
  ** call the call back function
  */

  if( cmph_dem_get_apps_pwr_status()  == CM_APPS_PWR_STATUS_PWR_RESTORE ||
      client_ptr->client_processor    == CM_PROCESSOR_MODEM             ||
      ( cmph_dem_get_apps_pwr_status()  != CM_APPS_PWR_STATUS_PWR_SUSPEND &&
      IS_PWR_COLLAPSE_NTFY_PREF_NULL(client_ptr->call_power_collpase_notify_pref) ) )
  {
    if( client_ptr->call_event_func != NULL &&
          bit_arr_is_bit_on(&client_ptr->call_bit_arr, (int)call_event) )
    {
      call_info_ptr->requesting_client_id = cmclient_get_client_id( client_ptr );
      client_ptr->call_event_func( call_event, call_info_ptr );
    }

    return;
  }
  #ifdef FEATURE_APPS_POWER_COLLAPSE

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch( cmph_dem_get_apps_pwr_status() )
  {
    case CM_APPS_PWR_STATUS_PWR_COLLAPSE:

    if( client_ptr->power_collapse_call_event_func != NULL &&
        bit_arr_is_bit_on(&client_ptr->call_power_collapse_bit_arr, (int)call_event) )
    {
      call_info_ptr->requesting_client_id = cmclient_get_client_id( client_ptr );
      client_ptr->power_collapse_call_event_func( call_event, call_info_ptr );
    }
    /* Check if the client is registered for this call event during power restore
    ** but is being suppressed during power collapse. Such events are buffered
    */
    else if( client_ptr->client_processor != CM_PROCESSOR_MODEM &&
             bit_arr_is_bit_on(&client_ptr->call_bit_arr, (int)call_event) &&
             !bit_arr_is_bit_on(&client_ptr->call_power_collapse_bit_arr, (int)call_event) )
    {
      /* Set the bit-mask to indicate that the call events are ignored.
      */
      bit_arr_set_bits( &client_ptr->call_power_collapse_buf_bit_arr,
                        TRUE,
                        (int)call_event,
                        (int)call_event);
    }
    return;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case CM_APPS_PWR_STATUS_PWR_SUSPEND:

      if(( client_ptr->client_processor    != CM_PROCESSOR_MODEM ) &&
         ( cmcall_dem_is_apps_wakeup_from_suspend_event( call_event ) ) &&
         ( client_ptr->call_event_func != NULL ) &&
         ( bit_arr_is_bit_on(&client_ptr->call_bit_arr, (int)call_event) ) )
      {
        call_info_ptr->requesting_client_id = cmclient_get_client_id( client_ptr );
        CM_MSG_HIGH_3("call evt %d ct %d cid id %d",
                    call_event,
                    cmclient_get_client_type(call_info_ptr->requesting_client_id),
                    call_info_ptr->requesting_client_id);
        client_ptr->call_event_func( call_event, call_info_ptr );
      }
      /* Check if the client is registered for this call event during power restore
      ** but is being suppressed during power collapse. Such events are buffered
      */
      else if( client_ptr->client_processor != CM_PROCESSOR_MODEM &&
               bit_arr_is_bit_on(&client_ptr->call_bit_arr, (int)call_event) &&
               !bit_arr_is_bit_on(&client_ptr->call_power_collapse_bit_arr, (int)call_event) )
      {
        /* Set the bit-mask to indicate that the call events are ignored.
        */
        bit_arr_set_bits( &client_ptr->call_power_collapse_buf_bit_arr,
                          TRUE,
                          (int)call_event,
                          (int)call_event);
      }
      return;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case CM_APPS_PWR_STATUS_PWR_RESTORE:
    case CM_APPS_PWR_STATUS_NONE:
    case CM_APPS_PWR_STATUS_MAX:
    default:
      CM_ERR_1( " Invalid power collapse state %d",
                                         cmph_dem_get_apps_pwr_status());
      return;
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  } /* switch */
  #endif
}/* cm_client_call_event_ntfy */
/*lint +esym(818, client_ptr) */


/*===========================================================================

FUNCTION cm_client_ph_event_ntfy

DESCRIPTION
  Notify a client of a specified phone event iff the client is registered
  for this event.

DEPENDENCIES
  Client must have already been initialized with cm_client_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cm_client_ph_event_ntfy(

    cm_client_s_type           *client_ptr,
        /* pointer to a client struct */

    cm_ph_event_e_type         ph_event,
        /* notify client of this phone event */

    cm_ph_info_s_type          *ph_info_ptr
        /* pointer to phone state information struct */
)
/*lint -esym(818, client_ptr)
** Pointer parameter 'client_ptr' could be declared as pointing to const
** ... yeah right
*/
{
  if (client_ptr == NULL)
  {
    CM_MSG_HIGH_0("client_ptr NULL");
    return;
  }
  CM_ASSERT( BETWEEN(ph_event, CM_PH_EVENT_NONE, CM_PH_EVENT_MAX) );
  CM_ASSERT( ph_info_ptr != NULL );

  /* Make sure this object was properly initialized.
  */
  CM_ASSERT( client_ptr->init_mask == CM_INIT_MASK );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the apps processor is in Power collapse state, or if this is modem client
  ** check if the client
  ** is registered for this event, if the client is registered for this event
  ** call the call back function
  */
  
  if( cmph_dem_get_apps_pwr_status() == CM_APPS_PWR_STATUS_PWR_RESTORE ||
      client_ptr->client_processor    == CM_PROCESSOR_MODEM             ||
      ( cmph_dem_get_apps_pwr_status()  != CM_APPS_PWR_STATUS_PWR_SUSPEND &&
      IS_PWR_COLLAPSE_NTFY_PREF_NULL(client_ptr->ph_power_collpase_notify_pref) ) )
  {
    if( client_ptr->ph_event_func != NULL &&
          bit_arr_is_bit_on(&client_ptr->ph_bit_arr, (int)ph_event) )
    {
      ph_info_ptr->requesting_client_id = cmclient_get_client_id( client_ptr );
      client_ptr->ph_event_func( ph_event, ph_info_ptr );
    }
    return;
  }
  #ifdef FEATURE_APPS_POWER_COLLAPSE
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch( cmph_dem_get_apps_pwr_status() )
  {
   case CM_APPS_PWR_STATUS_PWR_COLLAPSE:
      if( client_ptr->power_collapse_ph_event_func != NULL &&
        bit_arr_is_bit_on(&client_ptr->ph_power_collapse_bit_arr, (int)ph_event) )
      {
        ph_info_ptr->requesting_client_id = cmclient_get_client_id( client_ptr );
        CM_MSG_LOW_3("pc ph evt %d ct %d cid id %d",
                  ph_event,
                  cmclient_get_client_type(ph_info_ptr->requesting_client_id),
                  ph_info_ptr->requesting_client_id);
        client_ptr->power_collapse_ph_event_func( ph_event, ph_info_ptr );
      }
      /* Check if the client is registered for this ph event during power restore
      ** but is being suppressed during power collapse. Such events are buffered
      */
      else if(client_ptr->client_processor != CM_PROCESSOR_MODEM &&
              bit_arr_is_bit_on(&client_ptr->ph_bit_arr, (int)ph_event) &&
              !bit_arr_is_bit_on(&client_ptr->ph_power_collapse_bit_arr, (int)ph_event) )
      {
        /* Set the bit-mask to indicate that the ph events are buffered.
        */
        bit_arr_set_bits( &client_ptr->ph_power_collapse_buf_bit_arr,
                          TRUE,
                          (int)ph_event,
                          (int)ph_event);
      }
      else
      {
        CM_MSG_LOW_3(" Dropping pc ph evt %d ct %d cid id %d",
                   ph_event,
                   cmclient_get_client_type(ph_info_ptr->requesting_client_id),
                   ph_info_ptr->requesting_client_id);
      }
      return;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case CM_APPS_PWR_STATUS_PWR_SUSPEND:

      /* Check if the client is registered for this ph event during power restore
      ** but is being suppressed during power collapse. Such events are buffered
      */
      if(client_ptr->client_processor != CM_PROCESSOR_MODEM &&
         bit_arr_is_bit_on(&client_ptr->ph_bit_arr, (int)ph_event))
      {
        /* Set the bit-mask to indicate that the ph events are buffered.
        */
        bit_arr_set_bits( &client_ptr->ph_power_collapse_buf_bit_arr,
                          TRUE,
                          (int)ph_event,
                          (int)ph_event);
      }
      else
      {
        CM_MSG_LOW_3(" Dropping ph evt %d ct %d cid id %d",
                   ph_event,
                   cmclient_get_client_type(ph_info_ptr->requesting_client_id),
                   ph_info_ptr->requesting_client_id);
      }
      return;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case CM_APPS_PWR_STATUS_PWR_RESTORE:
    case CM_APPS_PWR_STATUS_NONE:
    case CM_APPS_PWR_STATUS_MAX:
    default:
      CM_ERR_1( " Invalid power collapse state %d",
                                         cmph_dem_get_apps_pwr_status());
      /*lint -save -e527 Unreachable
      **            When CM_DEBUG is defined, CM_ERR evaluates to exit(1)
      */

      return;
      /*lint -restore*/
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  } /* switch */
  #endif
} /* cm_client_ph_event_ntfy */
/*lint +esym(818, client_ptr) */

#if ((defined FEATURE_BCMCS) || (defined FEATURE_MBMS))
/*===========================================================================

FUNCTION cm_client_bcmcs_event_ntfy

DESCRIPTION
  Notify a client of a specified BCMCS event iff the client is registered
  for this event.

DEPENDENCIES
  Client must have already been initialized with cm_client_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cm_client_bcmcs_event_ntfy(

    cm_client_s_type           *client_ptr,
        /* pointer to a client struct */

    cm_bcmcs_event_e_type      bcmcs_event,
        /* notify client of this BCMCS event */

    cm_bcmcs_info_s_type       *bcmcs_info_ptr
        /* pointer to BCMCS information struct */
)
/*lint -esym(715,client_ptr) */
/*lint -esym(715,bcmcs_event) */
/*lint -esym(715,bcmcs_info_ptr) */
/*lint -esym(818, bcmcs_info_ptr) */
/*lint -esym(818, client_ptr) */
{
  if (client_ptr == NULL)
  {
    CM_MSG_HIGH_0("client_ptr NULL");
    return;
  }
  CM_ASSERT( BETWEEN(bcmcs_event, CM_BCMCS_EVENT_NONE, CM_BCMCS_EVENT_MAX) );

  if (bcmcs_info_ptr == NULL )
  {
    CM_MSG_HIGH_0("bcmcs_info_ptr NULL");
    return;
  }

  /* Make sure this object was properly initialized.
  */
  CM_ASSERT( client_ptr->init_mask == CM_INIT_MASK );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* If the apps processor is in Power collapse state, or if this is modem client
  ** check if the client
  ** is registered for this event, if the client is registered for this event
  ** call the call back function
  */

  if( cmph_dem_get_apps_pwr_status() == CM_APPS_PWR_STATUS_PWR_RESTORE ||
      client_ptr->client_processor    == CM_PROCESSOR_MODEM             ||
      ( cmph_dem_get_apps_pwr_status()  != CM_APPS_PWR_STATUS_PWR_SUSPEND &&
      IS_PWR_COLLAPSE_NTFY_PREF_NULL(client_ptr->bcmcs_power_collpase_notify_pref) ) )
  {
    if( client_ptr->bcmcs_event_func != NULL &&
          bit_arr_is_bit_on(&client_ptr->bcmcs_bit_arr, (int)bcmcs_event) )
    {
      bcmcs_info_ptr->requesting_client_id = cmclient_get_client_id( client_ptr );
      CM_MSG_MED_3("bcmcs evt %d ct %d cid id %d",
                  bcmcs_event,
                  cmclient_get_client_type(bcmcs_info_ptr->requesting_client_id),
                  bcmcs_info_ptr->requesting_client_id);
      client_ptr->bcmcs_event_func( bcmcs_event, bcmcs_info_ptr );
    }
    return;
  }
  #ifdef FEATURE_APPS_POWER_COLLAPSE

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch( cmph_dem_get_apps_pwr_status() )
  {
    case CM_APPS_PWR_STATUS_PWR_COLLAPSE:

      if( client_ptr->power_collapse_bcmcs_event_func != NULL &&
        bit_arr_is_bit_on(&client_ptr->bcmcs_power_collapse_bit_arr, (int)bcmcs_event) )
      {
        bcmcs_info_ptr->requesting_client_id = cmclient_get_client_id( client_ptr );
        CM_MSG_MED_3("pc bcmcs evt %d ct %d cid id %d",
                  bcmcs_event,
                  cmclient_get_client_type(bcmcs_info_ptr->requesting_client_id),
                  bcmcs_info_ptr->requesting_client_id);
        client_ptr->power_collapse_bcmcs_event_func( bcmcs_event, bcmcs_info_ptr );
      }
      /* Check if the client is registered for this bcmcs event during power restore
      ** but is being suppressed during power collapse. Such events are buffered
      */
      else if( client_ptr->client_processor != CM_PROCESSOR_MODEM &&
               bit_arr_is_bit_on(&client_ptr->bcmcs_bit_arr, (int)bcmcs_event) &&
               !bit_arr_is_bit_on(&client_ptr->bcmcs_power_collapse_bit_arr, (int)bcmcs_event) )
      {
        /* Set the bit-mask to indicate that the bcmcs events are ignored.
        */
        bit_arr_set_bits( &client_ptr->bcmcs_power_collapse_buf_bit_arr,
                          TRUE,
                          (int)bcmcs_event,
                          (int)bcmcs_event);
      }
      return;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case CM_APPS_PWR_STATUS_PWR_SUSPEND:

      /* Check if the client is registered for this bcmcs event during power restore
      ** but is being suppressed during power collapse. Such events are buffered
      */
      if( client_ptr->client_processor != CM_PROCESSOR_MODEM &&
          bit_arr_is_bit_on(&client_ptr->bcmcs_bit_arr, (int)bcmcs_event) &&
          !bit_arr_is_bit_on(&client_ptr->bcmcs_power_collapse_bit_arr, (int)bcmcs_event) )
      {
        /* Set the bit-mask to indicate that the bcmcs events are ignored.
        */
        bit_arr_set_bits( &client_ptr->bcmcs_power_collapse_buf_bit_arr,
                          TRUE,
                          (int)bcmcs_event,
                          (int)bcmcs_event);
      }
      else
      {
        CM_MSG_HIGH_3("Dropping bcmcs evt %d ct %d cid id %d",
                    bcmcs_event,
                    cmclient_get_client_type(bcmcs_info_ptr->requesting_client_id),
                    bcmcs_info_ptr->requesting_client_id);
      }
      return;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case CM_APPS_PWR_STATUS_PWR_RESTORE:
    case CM_APPS_PWR_STATUS_NONE:
    case CM_APPS_PWR_STATUS_MAX:
    default:
      CM_ERR_1( " Invalid power collapse state %d",
                                         cmph_dem_get_apps_pwr_status());
      /*lint -save -e527 Unreachable
      **            When CM_DEBUG is defined, CM_ERR evaluates to exit(1)
      */

      return;
    /*lint -restore*/
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  } /* switch */
  #endif /* FEATURE_APPS_POWER_COLLAPSE */

} /* cm_client_bcmcs_event_ntfy */
/*lint +esym(715,client_ptr) */
/*lint +esym(818, client_ptr) */
/*lint +esym(715,bcmcs_event) */
/*lint +esym(715,bcmcs_info_ptr) */
/*lint +esym(818, bcmcs_info_ptr) */
#endif /* FEATURE_BCMCS || MBMS */


/*===========================================================================

FUNCTION cm_client_ss_event_ntfy_new

DESCRIPTION
  Notify a client of a specified serving system event if the client is
  registered for this event.

DEPENDENCIES
  Client must have already been initialized with cm_client_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cm_client_ss_event_ntfy_new(

    cm_client_s_type           *client_ptr,
        /* pointer to a client struct */

    cm_ss_event_e_type         ss_event,
        /* notify client of this serving system event */

    cm_mm_msim_ss_info_s_type       *ss_info_ptr
        /* pointer to serving system information struct */

)
/*lint -esym(818, client_ptr)
** Pointer parameter 'client_ptr' could be declared as pointing to const
** ... yeah right
*/
{
  if (client_ptr == NULL)
  {
    CM_MSG_HIGH_0("client_ptr NULL");
    return;
  }
  CM_ASSERT( BETWEEN(ss_event, CM_SS_EVENT_NONE, CM_SS_EVENT_MAX) );
  CM_ASSERT( ss_info_ptr != NULL );

  /* Make sure this object was properly initialized.
  */
  CM_ASSERT( client_ptr->init_mask == CM_INIT_MASK );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* If the apps processor is in Power collapse state, or if this is modem client
  ** check if the client
  ** is registered for this event, if the client is registered for this event
  ** call the call back function
  */

  if( cmph_dem_get_apps_pwr_status() == CM_APPS_PWR_STATUS_PWR_RESTORE ||
      client_ptr->client_processor    == CM_PROCESSOR_MODEM             ||
      ( cmph_dem_get_apps_pwr_status()  != CM_APPS_PWR_STATUS_PWR_SUSPEND &&
      IS_PWR_COLLAPSE_NTFY_PREF_NULL(client_ptr->ss_power_collpase_notify_pref) ) )
  {
    if( client_ptr->msim_ss_event_func != NULL &&
          bit_arr_is_bit_on(&client_ptr->ss_bit_arr, (int)ss_event) &&
          (client_ptr->asubs_mask & BM(ss_info_ptr->asubs_id))
           )
    {
      ss_info_ptr->requesting_client_id = cmclient_get_client_id( client_ptr );
      CM_MSG_LOW_3("ss evt %d ct %d cid id %d",
                  ss_event,
                  cmclient_get_client_type(ss_info_ptr->requesting_client_id),
                  ss_info_ptr->requesting_client_id);
      
      client_ptr->msim_ss_event_func( ss_event, ss_info_ptr );
    }
  }
  
  
  return;

} /* cm_client_ss_event_ntfy */


/*===========================================================================

FUNCTION cm_client_ss_event_ntfy

DESCRIPTION
  Notify a client of a specified serving system event if the client is
  registered for this event.

DEPENDENCIES
  Client must have already been initialized with cm_client_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cm_client_ss_event_ntfy(

    cm_client_s_type           *client_ptr,
        /* pointer to a client struct */

    cm_ss_event_e_type         ss_event,
        /* notify client of this serving system event */

    cm_mm_ss_info_s_type       *ss_info_ptr
        /* pointer to serving system information struct */
)
/*lint -esym(818, client_ptr)
** Pointer parameter 'client_ptr' could be declared as pointing to const
** ... yeah right
*/
{
  if (client_ptr == NULL)
  {
    CM_MSG_HIGH_0("client_ptr NULL");
    return;
  }
  CM_ASSERT( BETWEEN(ss_event, CM_SS_EVENT_NONE, CM_SS_EVENT_MAX) );
  CM_ASSERT( ss_info_ptr != NULL );

  /* Make sure this object was properly initialized.
  */
  CM_ASSERT( client_ptr->init_mask == CM_INIT_MASK );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* If the apps processor is in Power collapse state, or if this is modem client
  ** check if the client
  ** is registered for this event, if the client is registered for this event
  ** call the call back function
  */

  if( cmph_dem_get_apps_pwr_status() == CM_APPS_PWR_STATUS_PWR_RESTORE ||
      client_ptr->client_processor    == CM_PROCESSOR_MODEM             ||
      ( cmph_dem_get_apps_pwr_status()  != CM_APPS_PWR_STATUS_PWR_SUSPEND &&
      IS_PWR_COLLAPSE_NTFY_PREF_NULL(client_ptr->ss_power_collpase_notify_pref) ) )
  {
    if( client_ptr->ss_event_func != NULL &&
          bit_arr_is_bit_on(&client_ptr->ss_bit_arr, (int)ss_event) )
    {
      ss_info_ptr->requesting_client_id = cmclient_get_client_id( client_ptr );
      CM_MSG_LOW_3("ss evt %d ct %d cid id %d",
                  ss_event,
                  cmclient_get_client_type(ss_info_ptr->requesting_client_id),
                  ss_info_ptr->requesting_client_id);
      client_ptr->ss_event_func( ss_event, ss_info_ptr );
    }
    return;
  }

  #ifdef FEATURE_APPS_POWER_COLLAPSE
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch( cmph_dem_get_apps_pwr_status() )
  {
    case CM_APPS_PWR_STATUS_PWR_COLLAPSE:

      if( client_ptr->power_collapse_ss_event_func != NULL &&
        bit_arr_is_bit_on(&client_ptr->ss_power_collapse_bit_arr, (int)ss_event) )
      {
        ss_info_ptr->requesting_client_id = cmclient_get_client_id( client_ptr );
        CM_MSG_LOW_3("pc ss evt %d ct %d cid id %d",
                  ss_event,
                  cmclient_get_client_type(ss_info_ptr->requesting_client_id),
                  ss_info_ptr->requesting_client_id);
        client_ptr->power_collapse_ss_event_func( ss_event, ss_info_ptr );
      }
      /* Check if the client is registered for this ss event during power restore
      ** but is being suppressed during power collapse. Such events are buffered
      */
      else if( client_ptr->client_processor != CM_PROCESSOR_MODEM &&
               bit_arr_is_bit_on(&client_ptr->ss_bit_arr, (int)ss_event) &&
               !bit_arr_is_bit_on(&client_ptr->ss_power_collapse_bit_arr, (int)ss_event) )
      {
        /* Set the bit-mask to indicate that the ss events are ignored.
        */
        bit_arr_set_bits( &client_ptr->ss_power_collapse_buf_bit_arr,
                          TRUE,
                          (int)ss_event,
                          (int)ss_event);
      }
      else
      {
        CM_MSG_LOW_3("Dropping pc ss evt %d ct %d cid id %d",
                   ss_event,
                   cmclient_get_client_type(ss_info_ptr->requesting_client_id),
                   ss_info_ptr->requesting_client_id);
      }
      return;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case CM_APPS_PWR_STATUS_PWR_SUSPEND:

      /* Check if the client is registered for this ss event during power restore
      ** but is being suppressed during power collapse. Such events are buffered
      */
      if( client_ptr->client_processor != CM_PROCESSOR_MODEM &&
          bit_arr_is_bit_on(&client_ptr->ss_bit_arr, (int)ss_event))
      {
        /* Set the bit-mask to indicate that the ss events are buffered.
        */
        bit_arr_set_bits( &client_ptr->ss_power_collapse_buf_bit_arr,
                          TRUE,
                          (int)ss_event,
                          (int)ss_event);
      }
      else
      {
        CM_MSG_LOW_3("Dropping ss evt %d ct %d cid id %d",
                   ss_event,
                   cmclient_get_client_type(ss_info_ptr->requesting_client_id),
                   ss_info_ptr->requesting_client_id);
      }
      return;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case CM_APPS_PWR_STATUS_PWR_RESTORE:
    case CM_APPS_PWR_STATUS_NONE:
    case CM_APPS_PWR_STATUS_MAX:
    default:
      CM_ERR_1( " Invalid power collapse state %d",
                                         cmph_dem_get_apps_pwr_status());
      /*lint -save -e527 Unreachable
      **            When CM_DEBUG is defined, CM_ERR evaluates to exit(1)
      */

      return;
    /*lint -restore*/
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  } /* switch */
  #endif
} /* cm_client_ss_event_ntfy */
/*lint +esym(818, client_ptr) */

/*===========================================================================

FUNCTION cm_client_inband_event_ntfy

DESCRIPTION
  Notify a client of a specified inband event iff the client is registered
  for this event.

DEPENDENCIES
  Client must have already been initialized with cm_client_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cm_client_inband_event_ntfy(

    cm_client_s_type           *client_ptr,
        /* pointer to a client struct */

    cm_inband_event_e_type      inband_event,
        /* notify client of this inband event */

    cm_inband_info_s_type      *inband_info_ptr
        /* pointer to inband information struct */
)
/*lint -esym(818, client_ptr)
** Pointer parameter 'client_ptr' could be declared as pointing to const
** ... yeah right
*/
{
  if (client_ptr == NULL)
  {
    CM_MSG_HIGH_0("client_ptr NULL");
    return;
  }
  CM_ASSERT( BETWEEN(inband_event, CM_INBAND_EVENT_NONE, CM_INBAND_EVENT_MAX) );
  CM_ASSERT( inband_info_ptr != NULL );

  /* Make sure this object was properly initialized.
  */
  CM_ASSERT( client_ptr->init_mask == CM_INIT_MASK );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the apps processor is in Power collapse state, or if this is modem client
  ** check if the client
  ** is registered for this event, if the client is registered for this event
  ** call the call back function
  */

  if( cmph_dem_get_apps_pwr_status() == CM_APPS_PWR_STATUS_PWR_RESTORE ||
      client_ptr->client_processor    == CM_PROCESSOR_MODEM             ||
      ( cmph_dem_get_apps_pwr_status()  != CM_APPS_PWR_STATUS_PWR_SUSPEND &&
      IS_PWR_COLLAPSE_NTFY_PREF_NULL(client_ptr->inband_power_collpase_notify_pref) ) )
  {
    if( client_ptr->inband_event_func != NULL &&
          bit_arr_is_bit_on(&client_ptr->inband_bit_arr, (int)inband_event) )
    {
      inband_info_ptr->requesting_client_id = cmclient_get_client_id( client_ptr );
      CM_MSG_MED_3("ib evt %d ct %d cid id %d",
                  inband_event,
                  cmclient_get_client_type(inband_info_ptr->requesting_client_id),
                  inband_info_ptr->requesting_client_id);
      client_ptr->inband_event_func( inband_event, inband_info_ptr );
    }
    return;
  }
  #ifdef FEATURE_APPS_POWER_COLLAPSE
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch( cmph_dem_get_apps_pwr_status() )
  {
    case CM_APPS_PWR_STATUS_PWR_COLLAPSE:

    if( client_ptr->power_collapse_inband_event_func != NULL &&
        bit_arr_is_bit_on(&client_ptr->inband_power_collapse_bit_arr, (int)inband_event) )
    {
      inband_info_ptr->requesting_client_id = cmclient_get_client_id( client_ptr );
      CM_MSG_MED_3("pc ib evt %d ct %d cid id %d",
                  inband_event,
                  cmclient_get_client_type(inband_info_ptr->requesting_client_id),
                  inband_info_ptr->requesting_client_id);
      client_ptr->power_collapse_inband_event_func( inband_event, inband_info_ptr );
    }
    /* Check if the client is registered for this inband event during power restore
    ** but is being suppressed during power collapse. Such events are buffered
    */
    else if( client_ptr->client_processor != CM_PROCESSOR_MODEM &&
             bit_arr_is_bit_on(&client_ptr->inband_bit_arr, (int)inband_event) &&
             !bit_arr_is_bit_on(&client_ptr->inband_power_collapse_bit_arr, (int)inband_event) )
    {
      /* Set the bit-mask to indicate that the inband events are ignored.
      */
      bit_arr_set_bits( &client_ptr->inband_power_collapse_buf_bit_arr,
                        TRUE,
                        (int)inband_event,
                        (int)inband_event);
    }
    return;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case CM_APPS_PWR_STATUS_PWR_SUSPEND:

      /* Check if the client is registered for this inband event during power restore
      ** but is being suppressed during power collapse. Such events are buffered
      */
      if( client_ptr->client_processor != CM_PROCESSOR_MODEM &&
          bit_arr_is_bit_on(&client_ptr->inband_bit_arr, (int)inband_event) &&
          !bit_arr_is_bit_on(&client_ptr->inband_power_collapse_bit_arr, (int)inband_event) )
      {
        /* Set the bit-mask to indicate that the inband events are ignored.
        */
        bit_arr_set_bits( &client_ptr->inband_power_collapse_buf_bit_arr,
                          TRUE,
                          (int)inband_event,
                          (int)inband_event);
      }
      else
      {
        CM_MSG_HIGH_3("dropping ib evt %d ct %d cid id %d",
                   inband_event,
                   cmclient_get_client_type(inband_info_ptr->requesting_client_id),
                   inband_info_ptr->requesting_client_id);
      }
      return;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case CM_APPS_PWR_STATUS_PWR_RESTORE:
    case CM_APPS_PWR_STATUS_NONE:
    case CM_APPS_PWR_STATUS_MAX:
    default:
      CM_ERR_1( " Invalid power collapse state %d",
                                         cmph_dem_get_apps_pwr_status());
      /*lint -save -e527 Unreachable
      **            When CM_DEBUG is defined, CM_ERR evaluates to exit(1)
      */

      return;
      /*lint -restore*/

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  } /* swtich */
  #endif
} /* cm_client_inband_event_ntfy */
/*lint +esym(818, client_ptr) */


/*===========================================================================

FUNCTION cm_client_sms_event_ntfy

DESCRIPTION
  Notify a client of a specified sms event iff the client is registered
  for this event.

DEPENDENCIES
  Client must have already been initialized with cm_client_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cm_client_sms_event_ntfy(

    cm_client_s_type           *client_ptr,
        /* pointer to a client struct */

    cm_sms_event_e_type        sms_event,
        /* notify client of this sms event */

    cm_sms_info_s_type         *sms_info_ptr
        /* pointer to sms information struct */
)
/*lint -esym(818, client_ptr)
** Pointer parameter 'client_ptr' could be declared as pointing to const
** ... yeah right
*/
{
  if (client_ptr == NULL)
  {
    CM_MSG_HIGH_0("client_ptr NULL");
    return;
  }
  CM_ASSERT( BETWEEN(sms_event, CM_SMS_EVENT_NONE, CM_SMS_EVENT_MAX) );
  CM_ASSERT( sms_info_ptr != NULL );

  /* Make sure this object was properly initialized.
  */
  CM_ASSERT( client_ptr->init_mask == CM_INIT_MASK );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the apps processor is in Power collapse state, or if this is modem client
  ** check if the client
  ** is registered for this event, if the client is registered for this event
  ** call the call back function
  */

  if( cmph_dem_get_apps_pwr_status() == CM_APPS_PWR_STATUS_PWR_RESTORE ||
      client_ptr->client_processor    == CM_PROCESSOR_MODEM             ||
      ( cmph_dem_get_apps_pwr_status()  != CM_APPS_PWR_STATUS_PWR_SUSPEND &&
      IS_PWR_COLLAPSE_NTFY_PREF_NULL(client_ptr->sms_power_collpase_notify_pref) ) )
  {
    if( client_ptr->sms_event_func != NULL &&
          bit_arr_is_bit_on(&client_ptr->sms_bit_arr, (int)sms_event) )
    {
      sms_info_ptr->requesting_client_id = cmclient_get_client_id( client_ptr );
      CM_MSG_MED_3("sms evt %d ct %d cid id %d",
                  sms_event,
                  cmclient_get_client_type(sms_info_ptr->requesting_client_id),
                  sms_info_ptr->requesting_client_id);
      client_ptr->sms_event_func( sms_event, sms_info_ptr );
    }
    return;
  }
  #ifdef FEATURE_APPS_POWER_COLLAPSE
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch( cmph_dem_get_apps_pwr_status() )
  {
    case CM_APPS_PWR_STATUS_PWR_COLLAPSE:

    if( client_ptr->power_collapse_sms_event_func != NULL &&
        bit_arr_is_bit_on(&client_ptr->sms_power_collapse_bit_arr, (int)sms_event) )
    {
      sms_info_ptr->requesting_client_id = cmclient_get_client_id( client_ptr );
      CM_MSG_MED_3("pc sms evt %d ct %d cid id %d",
                  sms_event,
                  cmclient_get_client_type(sms_info_ptr->requesting_client_id),
                  sms_info_ptr->requesting_client_id);
      client_ptr->power_collapse_sms_event_func( sms_event, sms_info_ptr );
    }
    /* Check if the client is registered for this sms event during power restore
    ** but is being suppressed during power collapse. Such events are buffered
    */
    else if( client_ptr->client_processor != CM_PROCESSOR_MODEM &&
             bit_arr_is_bit_on(&client_ptr->sms_bit_arr, (int)sms_event) &&
             !bit_arr_is_bit_on(&client_ptr->sms_power_collapse_bit_arr, (int)sms_event) )
    {
      /* Set the bit-mask to indicate that the sms events are ignored.
      */
      bit_arr_set_bits( &client_ptr->sms_power_collapse_buf_bit_arr,
                        TRUE,
                        (int)sms_event,
                        (int)sms_event);
    }
    return;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case CM_APPS_PWR_STATUS_PWR_SUSPEND:

      if(( client_ptr->client_processor    != CM_PROCESSOR_MODEM ) &&
         ( cmsms_dem_is_apps_wakeup_from_suspend_event( sms_event ) ) &&
         ( client_ptr->sms_event_func != NULL ) &&
         ( bit_arr_is_bit_on(&client_ptr->sms_bit_arr, (int)sms_event) ) )
      {
        sms_info_ptr->requesting_client_id = cmclient_get_client_id( client_ptr );
        CM_MSG_MED_3("pc sms evt %d ct %d cid id %d",
                    sms_event,
                    cmclient_get_client_type(sms_info_ptr->requesting_client_id),
                    sms_info_ptr->requesting_client_id);
        client_ptr->sms_event_func( sms_event, sms_info_ptr );
      }
      /* Check if the client is registered for this sms event during power restore
      ** but is being suppressed during power collapse. Such events are buffered
      */
      else if( client_ptr->client_processor != CM_PROCESSOR_MODEM &&
               bit_arr_is_bit_on(&client_ptr->sms_bit_arr, (int)sms_event) &&
               !bit_arr_is_bit_on(&client_ptr->sms_power_collapse_bit_arr, (int)sms_event) )
      {
        /* Set the bit-mask to indicate that the sms events are ignored.
        */
        bit_arr_set_bits( &client_ptr->sms_power_collapse_buf_bit_arr,
                          TRUE,
                          (int)sms_event,
                          (int)sms_event);
      }
      return;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case CM_APPS_PWR_STATUS_PWR_RESTORE:
    case CM_APPS_PWR_STATUS_NONE:
    case CM_APPS_PWR_STATUS_MAX:
    default:
      CM_ERR_1( " Invalid power collapse state %d",
                                         cmph_dem_get_apps_pwr_status());
      /*lint -save -e527 Unreachable
      **            When CM_DEBUG is defined, CM_ERR evaluates to exit(1)
      */

      return;
      /*lint -restore*/

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  } /* switch */
  #endif

} /* cm_client_sms_event_ntfy */
/*lint +esym(818, client_ptr) */

#ifdef FEATURE_GPSONE

/*===========================================================================

FUNCTION cm_client_pd_event_ntfy

DESCRIPTION
  Notify a client of a specified pd event iff the client is registered
  for this event.

DEPENDENCIES
  Client must have already been initialized with cm_client_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cm_client_pd_event_ntfy(

    cm_client_s_type           *client_ptr,
        /* pointer to a client struct */

    cm_pd_event_e_type         pd_event,
        /* notify client of this pd event */

    cm_pd_info_s_type         *pd_info_ptr
        /* pointer to pd information struct */
)
/*lint -esym(818, client_ptr)
** Pointer parameter 'client_ptr' could be declared as pointing to const
** ... yeah right
*/
{
  if (client_ptr == NULL)
  {
    CM_MSG_HIGH_0("client_ptr NULL");
    return;
  }
  CM_ASSERT( BETWEEN(pd_event, CM_PD_EVENT_NONE, CM_PD_EVENT_MAX) );
  CM_ASSERT( pd_info_ptr != NULL );

  /* Make sure this object was properly initialized.
  */
  CM_ASSERT( client_ptr->init_mask == CM_INIT_MASK );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the apps processor is in Power collapse state, or if this is modem client
  ** check if the client
  ** is registered for this event, if the client is registered for this event
  ** call the call back function
  */

  if( cmph_dem_get_apps_pwr_status() == CM_APPS_PWR_STATUS_PWR_RESTORE ||
      client_ptr->client_processor    == CM_PROCESSOR_MODEM             ||
      IS_PWR_COLLAPSE_NTFY_PREF_NULL(client_ptr->pd_power_collpase_notify_pref) )
  {
    if( client_ptr->pd_event_func != NULL &&
          bit_arr_is_bit_on(&client_ptr->pd_bit_arr, (int)pd_event) )
    {
      pd_info_ptr->requesting_client_id = cmclient_get_client_id( client_ptr );
      CM_MSG_MED_3("pd evt %d ct %d cid id %d",
                  pd_event,
                  cmclient_get_client_type(pd_info_ptr->requesting_client_id),
                  pd_info_ptr->requesting_client_id);
      client_ptr->pd_event_func( pd_event, pd_info_ptr );
    }
    return;
  }
  #ifdef FEATURE_APPS_POWER_COLLAPSE
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch( cmph_dem_get_apps_pwr_status() )
  {
    case CM_APPS_PWR_STATUS_PWR_COLLAPSE:

    if( client_ptr->power_collapse_pd_event_func != NULL &&
        bit_arr_is_bit_on(&client_ptr->pd_power_collapse_bit_arr, (int)pd_event) )
    {
      pd_info_ptr->requesting_client_id = cmclient_get_client_id( client_ptr );
      CM_MSG_LOW_3("pd evt %d ct %d cid id %d",
                  pd_event,
                  cmclient_get_client_type(pd_info_ptr->requesting_client_id),
                  pd_info_ptr->requesting_client_id);
      client_ptr->power_collapse_pd_event_func( pd_event, pd_info_ptr );
    }
    /* Check if the client is registered for this pd event during power restore
    ** but is being suppressed during power collapse. Such events are buffered
    */
    else if( client_ptr->client_processor != CM_PROCESSOR_MODEM &&
             bit_arr_is_bit_on(&client_ptr->pd_bit_arr, (int)pd_event) &&
             !bit_arr_is_bit_on(&client_ptr->pd_power_collapse_bit_arr, (int)pd_event) )
    {
      /* Set the bit-mask to indicate that the pd events are ignored.
      */
      bit_arr_set_bits( &client_ptr->pd_power_collapse_buf_bit_arr,
                        TRUE,
                        (int)pd_event,
                        (int)pd_event);
    }
    return;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case CM_APPS_PWR_STATUS_PWR_SUSPEND:

      /* Check if the client is registered for this pd event during power restore
      ** but is being suppressed during power collapse. Such events are buffered
      */
      if( client_ptr->client_processor != CM_PROCESSOR_MODEM &&
          bit_arr_is_bit_on(&client_ptr->pd_bit_arr, (int)pd_event) &&
          !bit_arr_is_bit_on(&client_ptr->pd_power_collapse_bit_arr, (int)pd_event) )
      {
        /* Set the bit-mask to indicate that the pd events are ignored.
        */
        bit_arr_set_bits( &client_ptr->pd_power_collapse_buf_bit_arr,
                          TRUE,
                          (int)pd_event,
                          (int)pd_event);
      }
      else
      {
        CM_MSG_LOW_3("Dropping pd evt %d ct %d cid id %d",
                   pd_event,
                   cmclient_get_client_type(pd_info_ptr->requesting_client_id),
                   pd_info_ptr->requesting_client_id);
      }
      return;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case CM_APPS_PWR_STATUS_PWR_RESTORE:
    case CM_APPS_PWR_STATUS_NONE:
    case CM_APPS_PWR_STATUS_MAX:
    default:
      CM_ERR_1( " Invalid power collapse state %d",
                                         cmph_dem_get_apps_pwr_status());
      /*lint -save -e527 Unreachable
      **            When CM_DEBUG is defined, CM_ERR evaluates to exit(1)
      */

      return;
      /*lint -restore*/

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  } /* switch */
  #endif
} /* cm_client_pd_event_ntfy */
/*lint +esym(818, client_ptr) */
#endif


/*===========================================================================

FUNCTION cm_client_dbm_event_ntfy

DESCRIPTION
  Notify a client of a specified dbm event iff the client is registered
  for this event.

DEPENDENCIES
  Client must have already been initialized with cm_client_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cm_client_dbm_event_ntfy(

    cm_client_s_type           *client_ptr,
        /* pointer to a client struct */

    cm_dbm_event_e_type         dbm_event,
        /* notify client of this dbm event */

    cm_dbm_info_s_type         *dbm_info_ptr
        /* pointer to dbm information struct */
)
/*lint -esym(818, client_ptr)
** Pointer parameter 'client_ptr' could be declared as pointing to const
** ... yeah right
*/
{
  int dbm_class = (int)CM_DBM_CLASS_NONE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (client_ptr == NULL)
  {
    CM_MSG_HIGH_0("client_ptr NULL");
    return;
  }
  CM_ASSERT( BETWEEN(dbm_event, CM_DBM_EVENT_NONE, CM_DBM_EVENT_MAX) );
  CM_ASSERT( dbm_info_ptr != NULL );
  CM_ASSERT( client_ptr->init_mask == CM_INIT_MASK );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

   #if ((defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)) && \
        !defined(FEATURE_MMODE_REMOVE_1X))
  switch (dbm_info_ptr->burst_type)
  {

    case CAI_POSITION_DET:
      dbm_class = (int)CM_DBM_CLASS_PD;
      break;

    case CAI_SHORT_DATA_BURST:
      dbm_class = (int)CM_DBM_CLASS_SDB;
      break;

    default:
      CM_ERR_1("Unexpected burst type %d", dbm_info_ptr->burst_type);
      return;
  }

  /* If the apps processor is in Power collapse state, or if this is modem client
  ** check if the client
  ** is registered for this event, if the client is registered for this event
  ** call the call back function
  */

  if( cmph_dem_get_apps_pwr_status() == CM_APPS_PWR_STATUS_PWR_RESTORE ||
      client_ptr->client_processor    == CM_PROCESSOR_MODEM             ||
      ( cmph_dem_get_apps_pwr_status()  != CM_APPS_PWR_STATUS_PWR_SUSPEND &&
      IS_PWR_COLLAPSE_NTFY_PREF_NULL(client_ptr->dbm_power_collpase_notify_pref) ) )
  {
    if( client_ptr->dbm_event_func != NULL &&
        bit_arr_is_bit_on(&client_ptr->dbm_event_bit_arr, (int)dbm_event) &&
        bit_arr_is_bit_on(&client_ptr->dbm_class_bit_arr, (int)dbm_class))
    {
      dbm_info_ptr->requesting_client_id = cmclient_get_client_id( client_ptr );
      CM_MSG_LOW_3("dbm evt %d ct %d cid id %d",
                  dbm_event,
                  cmclient_get_client_type(dbm_info_ptr->requesting_client_id),
                  dbm_info_ptr->requesting_client_id);
      client_ptr->dbm_event_func( dbm_event, dbm_info_ptr );
    }
    return;
  }
  #ifdef FEATURE_APPS_POWER_COLLAPSE
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch( cmph_dem_get_apps_pwr_status() )
  {
    case CM_APPS_PWR_STATUS_PWR_COLLAPSE:

    if( client_ptr->power_collapse_dbm_event_func != NULL &&
        bit_arr_is_bit_on(&client_ptr->dbm_power_collapse_bit_arr, (int)dbm_event) &&
        bit_arr_is_bit_on(&client_ptr->dbm_class_bit_arr, (int)dbm_class))
    {
      dbm_info_ptr->requesting_client_id = cmclient_get_client_id( client_ptr );
      CM_MSG_LOW_3("pc dbm evt %d ct %d cid id %d",
                  dbm_event,
                  cmclient_get_client_type(dbm_info_ptr->requesting_client_id),
                  dbm_info_ptr->requesting_client_id);
      client_ptr->power_collapse_dbm_event_func( dbm_event, dbm_info_ptr );
    }
    /* Check if the client is registered for this dbm event during power restore
    ** but is being suppressed during power collapse. Such events are buffered
    */
    else if( client_ptr->client_processor != CM_PROCESSOR_MODEM &&
             bit_arr_is_bit_on(&client_ptr->dbm_event_bit_arr, (int)dbm_event) &&
             !bit_arr_is_bit_on(&client_ptr->dbm_power_collapse_bit_arr, (int)dbm_event) )
    {
      /* Set the bit-mask to indicate that the dbm events are ignored.
      */
      bit_arr_set_bits( &client_ptr->dbm_power_collapse_buf_bit_arr,
                        TRUE,
                        (int)dbm_event,
                        (int)dbm_event);
    }
    return;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case CM_APPS_PWR_STATUS_PWR_SUSPEND:

      if(( client_ptr->client_processor    != CM_PROCESSOR_MODEM ) &&
         ( cmxdbm_dem_is_apps_wakeup_from_suspend_event( dbm_event ) ) &&
         ( client_ptr->dbm_event_func != NULL ) &&
           bit_arr_is_bit_on(&client_ptr->dbm_event_bit_arr, (int)dbm_event) &&
           bit_arr_is_bit_on(&client_ptr->dbm_class_bit_arr, (int)dbm_class))

      {
        dbm_info_ptr->requesting_client_id = cmclient_get_client_id( client_ptr );
        CM_MSG_LOW_3("pc dbm evt %d ct %d cid id %d",
                    dbm_event,
                    cmclient_get_client_type(dbm_info_ptr->requesting_client_id),
                    dbm_info_ptr->requesting_client_id);
        client_ptr->dbm_event_func( dbm_event, dbm_info_ptr );
      }
      /* Check if the client is registered for this dbm event during power restore
      ** but is being suppressed during power collapse. Such events are buffered
      */
      else if( client_ptr->client_processor != CM_PROCESSOR_MODEM &&
               bit_arr_is_bit_on(&client_ptr->dbm_event_bit_arr, (int)dbm_event) &&
               !bit_arr_is_bit_on(&client_ptr->dbm_power_collapse_bit_arr, (int)dbm_event) )
      {
        /* Set the bit-mask to indicate that the dbm events are ignored.
        */
        bit_arr_set_bits( &client_ptr->dbm_power_collapse_buf_bit_arr,
                          TRUE,
                          (int)dbm_event,
                          (int)dbm_event);
      }
      return;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case CM_APPS_PWR_STATUS_PWR_RESTORE:
    case CM_APPS_PWR_STATUS_NONE:
    case CM_APPS_PWR_STATUS_MAX:
    default:
      CM_ERR_1( "Invalid power collapse state %d",
                                         cmph_dem_get_apps_pwr_status());
      /*lint -save -e527 Unreachable
      **            When CM_DEBUG is defined, CM_ERR evaluates to exit(1)
      */

      return;
      /*lint -restore*/

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  } /* switch */
  #endif

  #else
  SYS_ARG_NOT_USED(dbm_class);
  SYS_ARG_NOT_USED(dbm_info_ptr);
  SYS_ARG_NOT_CONST(dbm_info_ptr);
  SYS_ARG_NOT_USED(dbm_event);
  CM_ERR_0("FSN2 : 1X is not supported on this target ");
  #endif /*FEATURE_MMODE_REMOVE_1X*/

}/* cm_client_dbm_event_ntfy */
/*lint +esym(818, client_ptr) */



#if defined (CM_GW_SUPPORTED) ||defined(FEATURE_IP_CALL)

/*===========================================================================

FUNCTION cm_client_sups_event_ntfy

DESCRIPTION
  Notify a client of a specified SUPS event iff the client is registered
  for this event.

DEPENDENCIES
  Client must have already been initialized with cm_client_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cm_client_sups_event_ntfy(

    cm_client_s_type           *client_ptr,
        /* pointer to a client struct */

    cm_sups_event_e_type       sups_event,
        /* notify client of this sups event */

    cm_sups_info_s_type        *sups_info_ptr
        /* pointer to sups information struct */
)
/*lint -esym(818, client_ptr)
** Pointer parameter 'client_ptr' could be declared as pointing to const
** ... yeah right
*/
{
  if (client_ptr == NULL)
  {
    CM_MSG_HIGH_0("client_ptr NULL");
    return;
  }
  /* Verify all the parameters before notifying of the event */
  CM_ASSERT ( BETWEEN( sups_event, CM_SUPS_EVENT_NONE, CM_SUPS_EVENT_MAX) );
  CM_ASSERT( sups_info_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Verify that the client is already initialized and is registered for
  ** the event */

  CM_ASSERT( client_ptr->init_mask == CM_INIT_MASK );

  /* If the apps processor is in Power collapse state, or if this is modem client
  ** check if the client
  ** is registered for this event, if the client is registered for this event
  ** call the call back function
  */

  if( cmph_dem_get_apps_pwr_status() == CM_APPS_PWR_STATUS_PWR_RESTORE ||
      client_ptr->client_processor    == CM_PROCESSOR_MODEM             ||
      ( cmph_dem_get_apps_pwr_status()  != CM_APPS_PWR_STATUS_PWR_SUSPEND &&
      IS_PWR_COLLAPSE_NTFY_PREF_NULL(client_ptr->sups_power_collpase_notify_pref) ) )
  {
    if( client_ptr->sups_event_func != NULL &&
          bit_arr_is_bit_on(&client_ptr->sups_bit_arr, (int)sups_event) )
    {
      sups_info_ptr->requesting_client_id = cmclient_get_client_id( client_ptr );
      CM_MSG_MED_3("sups evt %d ct %d cid id %d",
                  sups_event,
                  cmclient_get_client_type(sups_info_ptr->requesting_client_id),
                  sups_info_ptr->requesting_client_id);
      client_ptr->sups_event_func( sups_event, sups_info_ptr );
    }
    return;
  }
  #ifdef FEATURE_APPS_POWER_COLLAPSE
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch( cmph_dem_get_apps_pwr_status() )
  {
    case CM_APPS_PWR_STATUS_PWR_COLLAPSE:

    if( client_ptr->power_collapse_sups_event_func != NULL &&
        bit_arr_is_bit_on(&client_ptr->sups_power_collapse_bit_arr, (int)sups_event) )
    {
      sups_info_ptr->requesting_client_id = cmclient_get_client_id( client_ptr );
      CM_MSG_MED_3("pc sups evt %d ct %d cid id %d",
                  sups_event,
                  cmclient_get_client_type(sups_info_ptr->requesting_client_id),
                  sups_info_ptr->requesting_client_id);
      client_ptr->power_collapse_sups_event_func( sups_event, sups_info_ptr );
    }
    /* Check if the client is registered for this sups event during power restore
    ** but is being suppressed during power collapse. Such events are buffered
    */
    else if( client_ptr->client_processor != CM_PROCESSOR_MODEM &&
             bit_arr_is_bit_on(&client_ptr->sups_bit_arr, (int)sups_event) &&
             !bit_arr_is_bit_on(&client_ptr->sups_power_collapse_bit_arr, (int)sups_event) )
    {
      /* Set the bit-mask to indicate that the sups events are ignored.
      */
      bit_arr_set_bits( &client_ptr->sups_power_collapse_buf_bit_arr,
                        TRUE,
                        (int)sups_event,
                        (int)sups_event);
    }
    return;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case CM_APPS_PWR_STATUS_PWR_SUSPEND:

      /* Check if the client is registered for this sups event during power restore
      ** but is being suppressed during power collapse. Such events are buffered
      */
      if(( client_ptr->client_processor    != CM_PROCESSOR_MODEM ) &&
         ( cmsups_dem_is_apps_wakeup_from_suspend_event( sups_event ) ) &&
         ( client_ptr->sups_event_func != NULL ) &&
         ( bit_arr_is_bit_on(&client_ptr->sups_bit_arr, (int)sups_event) ) )
      {
        sups_info_ptr->requesting_client_id = cmclient_get_client_id( client_ptr );
        CM_MSG_MED_3("pc sups evt %d ct %d cid id %d",
                   sups_event,
                   cmclient_get_client_type(sups_info_ptr->requesting_client_id),
                   sups_info_ptr->requesting_client_id);
        client_ptr->sups_event_func( sups_event, sups_info_ptr );
      }
      else if( client_ptr->client_processor != CM_PROCESSOR_MODEM &&
               bit_arr_is_bit_on(&client_ptr->sups_bit_arr, (int)sups_event) &&
               !bit_arr_is_bit_on(&client_ptr->sups_power_collapse_bit_arr, (int)sups_event) )
      {
        /* Set the bit-mask to indicate that the sups events are ignored.
        */
        bit_arr_set_bits( &client_ptr->sups_power_collapse_buf_bit_arr,
                          TRUE,
                          (int)sups_event,
                          (int)sups_event);
      }
      else
      {
        CM_MSG_HIGH_3("Dropping sups evt %d ct %d cid id %d",
                    sups_event,
                    cmclient_get_client_type(sups_info_ptr->requesting_client_id),
                    sups_info_ptr->requesting_client_id);
      }
      return;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case CM_APPS_PWR_STATUS_PWR_RESTORE:
    case CM_APPS_PWR_STATUS_NONE:
    case CM_APPS_PWR_STATUS_MAX:
    default:
      CM_ERR_1( "Invalid power collapse state %d",
                                         cmph_dem_get_apps_pwr_status());
      /*lint -save -e527 Unreachable
      **            When CM_DEBUG is defined, CM_ERR evaluates to exit(1)
      */

      return;
      /*lint -restore*/

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  } /* switch */
  #endif
} /* cm_client_sups_event_ntfy */
/*lint +esym(818, client_ptr) */
#endif


/*===========================================================================

FUNCTION cm_client_ac_event_ntfy

DESCRIPTION
  Notify a client of a specified access control event iff the client is
  registered for this event.

DEPENDENCIES
  Client must have already been initialized with cm_client_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cm_client_ac_event_ntfy(

    cm_client_s_type *client_ptr,
        /* pointer to a client struct */

    cm_ac_event_e_type event,
        /* notify client of this AC event */

    cm_ac_info_s_type *info_ptr
        /* pointer to AC information struct */
)
{
  if( cmph_dem_get_apps_pwr_status() == CM_APPS_PWR_STATUS_PWR_RESTORE ||
      client_ptr->client_processor    == CM_PROCESSOR_MODEM             ||
      ( cmph_dem_get_apps_pwr_status()  != CM_APPS_PWR_STATUS_PWR_SUSPEND &&
      IS_PWR_COLLAPSE_NTFY_PREF_NULL(client_ptr->sups_power_collpase_notify_pref) ) )
  {
    if( client_ptr->ac_event_func != NULL &&
          bit_arr_is_bit_on(&client_ptr->ac_bit_arr, (int)event) )
    {
      info_ptr->ac_client_id = client_ptr->client_id;
      client_ptr->ac_event_func( event, info_ptr );
    }
    return;
  }
}

#ifdef CM_GW_SUPPORTED
#ifdef FEATURE_WMS_CM_NAS_INTERFACES
#error code not present
#endif /* WMS_CM_NAS_INTERFACES  */
#endif /* FEATURE_GSM || FEATURE_WCDMA */



/*===========================================================================

FUNCTION cm_client_call_cmd_err_ntfy

DESCRIPTION
  Notify a client of a specified call command error code iff the client is
  registered to be notified of OTHER clients call command errors and the
  client is NOT the one originally sending this call command.

  Note that the client originally sending this command is notified of
  the command error via its command supplied callback function.

DEPENDENCIES
  Client must have already been initialized with cm_client_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cm_client_call_cmd_err_ntfy(

    const cm_client_s_type      *client_ptr,
        /* pointer to a client struct */

    const cm_call_cmd_s_type    *call_cmd_ptr,
        /* pointer to call command struct */

    cm_call_cmd_err_e_type      call_cmd_err
        /* call command error code */
)
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( client_ptr    != NULL );
  CM_ASSERT( call_cmd_ptr  != NULL );
  CM_ASSERT( BETWEEN(call_cmd_err, CM_CALL_CMD_ERR_NONE, CM_CALL_CMD_ERR_MAX) );

  /* Make sure this object was properly initialized.
  */
  CM_ASSERT( client_ptr->init_mask == CM_INIT_MASK );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( client_ptr != call_cmd_ptr->client_ptr )
  {
    if( client_ptr->call_cmd_err_func != NULL )
    {
      /* If call command is end command then proper call id won't be available
         with call_cmd_ptr->info.
      */
      if(CM_CALL_CMD_END != call_cmd_ptr->cmd)
      {
        client_ptr->call_cmd_err_func( call_cmd_ptr->info.call_id,
                                       call_cmd_ptr->client_id,
                                       call_cmd_ptr->cmd,
                                       call_cmd_err
                                      );
      }
      else
      {
        client_ptr->call_cmd_err_func( call_cmd_ptr->info.end_params[0].call_id,
                                       call_cmd_ptr->client_id,
                                       call_cmd_ptr->cmd,
                                       CM_CALL_CMD_ERR_CALL_STATE_S
                                      );
      }
    }
  }
}


/*===========================================================================

FUNCTION cm_client_ph_cmd_err_ntfy

DESCRIPTION
  Notify a client of a specified phone command error code iff the client is
  registered to be notified of OTHER clients phone command errors and the
  client is NOT the one originally sending this phone command.

  Note that the client originally sending this command is notified of
  the command error via its command supplied callback function.

DEPENDENCIES
  Client must have already been initialized with cm_client_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cm_client_ph_cmd_err_ntfy(

    const cm_client_s_type    *client_ptr,
        /* pointer to a client struct */

    const cm_ph_cmd_s_type    *ph_cmd_ptr,
        /* pointer to phone command struct */

    cm_ph_cmd_err_e_type      ph_cmd_err
        /* phone command error code */
)
{
  CM_ASSERT( client_ptr    != NULL );
  CM_ASSERT( ph_cmd_ptr  != NULL );
  CM_ASSERT( BETWEEN((int)ph_cmd_err, (int)CM_PH_CMD_ERR_NONE, (int)CM_PH_CMD_ERR_MAX) );

  /* Make sure this object was properly initialized.
  */
  CM_ASSERT( client_ptr->init_mask == CM_INIT_MASK );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( client_ptr != ph_cmd_ptr->client_ptr )
  {
    if( client_ptr->ph_cmd_err_func != NULL )
    {
      client_ptr->ph_cmd_err_func( ph_cmd_ptr->client_id,
                                   ph_cmd_ptr->cmd,
                                   ph_cmd_err );
    }
  }
}

/*===========================================================================

FUNCTION cm_client_stats_cmd_err_ntfy

DESCRIPTION
  Notify a client of a specified phone command error code iff the client is
  registered to be notified of OTHER clients phone command errors and the
  client is NOT the one originally sending this phone command.

  Note that the client originally sending this command is notified of
  the command error via its command supplied callback function.

DEPENDENCIES
  Client must have already been initialized with cm_client_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cm_client_stats_cmd_err_ntfy(

    const cm_client_s_type    *client_ptr,
        /* pointer to a client struct */

    const cm_stats_cmd_s_type    *stats_cmd_ptr,
        /* pointer to phone command struct */

    cm_stats_cmd_err_e_type      stats_cmd_err
        /* phone command error code */
)
/*lint -esym(715, client_ptr)*/
/*lint -esym(715, stats_cmd_ptr)*/
/*lint -esym(715, stats_cmd_err)*/
/*lint -esym(528, cm_client_stats_cmd_err_ntfy) not referenced */
{

  CM_ASSERT( client_ptr    != NULL );
  CM_ASSERT( stats_cmd_ptr  != NULL );
  CM_ASSERT( BETWEEN((int)stats_cmd_err,
             (int)CM_STATS_CMD_ERR_NONE, (int)CM_STATS_CMD_ERR_MAX) );

  /* Make sure this object was properly initialized.
  */
  CM_ASSERT( client_ptr->init_mask == CM_INIT_MASK );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( client_ptr != stats_cmd_ptr->client_ptr )
  {
    if( client_ptr->stats_cmd_err_func != NULL )
    {
      client_ptr->stats_cmd_err_func( stats_cmd_ptr->client_id,
                                      stats_cmd_ptr->cmd,
                                      stats_cmd_err );
    }
  }
}
/*lint +esym(715, client_ptr)*/
/*lint +esym(715, stats_cmd_ptr)*/
/*lint +esym(715, stats_cmd_err)*/


  #if ((defined FEATURE_BCMCS) || (defined FEATURE_MBMS))
/*===========================================================================

FUNCTION cm_client_bcmcs_cmd_err_ntfy

DESCRIPTION
  Notify a client of a specified phone command error code iff the client is
  registered to be notified of OTHER clients phone command errors and the
  client is NOT the one originally sending this phone command.

  Note that the client originally sending this command is notified of
  the command error via its command supplied callback function.

DEPENDENCIES
  Client must have already been initialized with cm_client_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cm_client_bcmcs_cmd_err_ntfy(

    const cm_client_s_type    *client_ptr,
        /* pointer to a client struct */

    const cm_bcmcs_cmd_s_type    *bcmcs_cmd_ptr,
        /* pointer to phone command struct */

    cm_bcmcs_cmd_err_e_type      bcmcs_cmd_err
        /* phone command error code */
)
{
  CM_ASSERT( client_ptr    != NULL );
  CM_ASSERT( bcmcs_cmd_ptr  != NULL );
  CM_ASSERT( BETWEEN((int)bcmcs_cmd_err, (int)CM_BCMCS_CMD_ERR_NONE, (int)CM_BCMCS_CMD_ERR_MAX) );

  /* Make sure this object was properly initialized.
  */
  CM_ASSERT( client_ptr->init_mask == CM_INIT_MASK );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( client_ptr != bcmcs_cmd_ptr->client_ptr )
  {
    if( client_ptr->bcmcs_cmd_err_func != NULL )
    {
      client_ptr->bcmcs_cmd_err_func( bcmcs_cmd_ptr->client_id,
                                   bcmcs_cmd_ptr->cmd,
                                   bcmcs_cmd_err );
    }
  }
}
#endif /* FEATURE_BCMCS  || MBMS */

#if defined (FEATURE_MBMS)
#error code not present
#endif

/*===========================================================================

FUNCTION cm_client_ss_cmd_err_ntfy

DESCRIPTION
  Notify a client of a specified serving system command error code iff the client is
  registered to be notified of OTHER clients serving system command errors and the
  client is NOT the one originally sending this serving system command.

  Note that the client originally sending this command is notified of
  the command error via its command supplied callback function.

DEPENDENCIES
  Client must have already been initialized with cm_client_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cm_client_ss_cmd_err_ntfy(

    const cm_client_s_type      *client_ptr,
        /* pointer to a client struct */

    const cm_ss_cmd_s_type      *ss_cmd_ptr,
        /* pointer to ss command struct */

    cm_ss_cmd_err_e_type        ss_cmd_err
        /* call command error code */
)
{
  CM_ASSERT( client_ptr    != NULL );
  CM_ASSERT( ss_cmd_ptr  != NULL );
  CM_ASSERT( BETWEEN(ss_cmd_err, CM_SS_CMD_ERR_NONE, CM_SS_CMD_ERR_MAX) );

  /* Make sure this object was properly initialized.
  */
  CM_ASSERT( client_ptr->init_mask == CM_INIT_MASK );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( client_ptr != ss_cmd_ptr->client_ptr )
  {
    if( client_ptr->ss_cmd_err_func != NULL )
    {
      client_ptr->ss_cmd_err_func( ss_cmd_ptr->client_id,
                     ss_cmd_ptr->cmd,
                     ss_cmd_err );
    }
  }
}



/*===========================================================================

FUNCTION cm_client_inband_cmd_err_ntfy

DESCRIPTION
  Notify a client of a specified inband command error code iff the client is
  registered to be notified of OTHER clients inband command errors and the
  client is NOT the one originally sending this inband command.

  Note that the client originally sending this command is notified of
  the command error via its command supplied callback function.

DEPENDENCIES
  Client must have already been initialized with cm_client_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cm_client_inband_cmd_err_ntfy(

    const cm_client_s_type      *client_ptr,
        /* pointer to a client struct */

    const cm_inband_cmd_s_type  *inband_cmd_ptr,
        /* pointer to inband command struct */

    cm_inband_cmd_err_e_type    inband_cmd_err
        /* phone command error code */
)
{
  const cm_inband_cmd_info_s_type *cmd_info_ptr  = NULL;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( client_ptr    != NULL );
  CM_ASSERT( inband_cmd_ptr  != NULL );
  CM_ASSERT( BETWEEN((int)inband_cmd_err,
                     (int)CM_INBAND_CMD_ERR_NONE,
                     (int)CM_INBAND_CMD_ERR_MAX) );
  CM_ASSERT( client_ptr->init_mask == CM_INIT_MASK );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmd_info_ptr  = CMD_INFO_PTR( inband_cmd_ptr );


  if( client_ptr != inband_cmd_ptr->client_ptr )
  {
    if( client_ptr->inband_cmd_err_func != NULL )
    {
      client_ptr->inband_cmd_err_func( cmd_info_ptr->call_id,
                                       inband_cmd_ptr->client_id,
                                       inband_cmd_ptr->cmd,
                                       inband_cmd_err
                                       );
    }
  }
}


/*===========================================================================

FUNCTION cm_client_sms_cmd_err_ntfy

DESCRIPTION
  Notify a client of a specified SMS command error code iff the client is
  registered to be notified of OTHER clients SMS command errors and the
  client is NOT the one originally sending this SMS command.

  Note that the client originally sending this command is notified of
  the command error via its command supplied callback function.

DEPENDENCIES
  Client must have already been initialized with cm_client_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cm_client_sms_cmd_err_ntfy(

    const cm_client_s_type      *client_ptr,
        /* pointer to a client struct */

    const cm_sms_cmd_s_type     *sms_cmd_ptr,
        /* pointer to sms command struct */

    cm_sms_cmd_err_e_type       sms_cmd_err
        /* phone command error code */
)
{
  CM_ASSERT( client_ptr    != NULL );
  CM_ASSERT( sms_cmd_ptr  != NULL );
  CM_ASSERT( BETWEEN((int)sms_cmd_err, (int)CM_SMS_CMD_ERR_NONE, (int)CM_SMS_CMD_ERR_MAX) );

  /* Make sure this object was properly initialized.
  */
  CM_ASSERT( client_ptr->init_mask == CM_INIT_MASK );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( client_ptr != sms_cmd_ptr->client_ptr )
  {
    if( client_ptr->sms_cmd_err_func != NULL )
    {
      client_ptr->sms_cmd_err_func( sms_cmd_ptr->client_id,
                                       sms_cmd_ptr->cmd,
                                       sms_cmd_err );
    }
  }
}


#ifdef FEATURE_GPSONE
/*===========================================================================

FUNCTION cm_client_pd_cmd_err_ntfy

DESCRIPTION
  Notify a client of a specified PD command error code iff the client is
  registered to be notified of OTHER clients PD command errors and the
  client is NOT the one originally sending this PD command.

  Note that the client originally sending this command is notified of
  the command error via its command supplied callback function.

DEPENDENCIES
  Client must have already been initialized with cm_client_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/

static void cm_client_pd_cmd_err_ntfy(

    const cm_client_s_type      *client_ptr,
        /* pointer to a client struct */

    const cm_pd_cmd_s_type     *pd_cmd_ptr,
        /* pointer to sms command struct */

    cm_pd_cmd_err_e_type       pd_cmd_err
        /* phone command error code */
)
{
  CM_ASSERT( client_ptr    != NULL );
  CM_ASSERT( pd_cmd_ptr  != NULL );
  CM_ASSERT( BETWEEN((int)pd_cmd_err, (int)CM_PD_CMD_ERR_NONE, (int)CM_PD_CMD_ERR_MAX) );

  /* Make sure this object was properly initialized.
  */
  CM_ASSERT( client_ptr->init_mask == CM_INIT_MASK );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( client_ptr != pd_cmd_ptr->client_ptr )
  {
    if( client_ptr->pd_cmd_err_func != NULL )
    {
      client_ptr->pd_cmd_err_func( pd_cmd_ptr->client_id,
                                   pd_cmd_ptr->cmd,
                                   pd_cmd_err );
    }
  }
}
#endif /* FEATURE_GPSONE */


/*===========================================================================

FUNCTION cm_client_dbm_cmd_err_ntfy

DESCRIPTION
  Notify a client of a specified DBM command error code iff the client is
  registered to be notified of OTHER clients DBM command errors and the
  client is NOT the one originally sending this DBM command.

  Note that the client originally sending this command is notified of
  the command error via its command supplied callback function.

DEPENDENCIES
  Client must have already been initialized with cm_client_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/

static void cm_client_dbm_cmd_err_ntfy(

    const cm_client_s_type      *client_ptr,
        /* pointer to a client struct */

    const cm_dbm_cmd_s_type     *dbm_cmd_ptr,
        /* pointer to sms command struct */

    cm_dbm_cmd_err_e_type       dbm_cmd_err
        /* phone command error code */
)
{
  CM_ASSERT( client_ptr   != NULL );
  CM_ASSERT( dbm_cmd_ptr  != NULL );
  CM_ASSERT( BETWEEN((int)dbm_cmd_err, (int)CM_DBM_CMD_ERR_NONE, (int)CM_DBM_CMD_ERR_MAX));

  /* Make sure this object was properly initialized.
  */
  CM_ASSERT( client_ptr->init_mask == CM_INIT_MASK );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( client_ptr != dbm_cmd_ptr->client_ptr )
  {
    if( client_ptr->dbm_cmd_err_func != NULL )
    {
      client_ptr->dbm_cmd_err_func( dbm_cmd_ptr->client_id,
                                    dbm_cmd_ptr->cmd,
                                    dbm_cmd_err );
    }
  }
}

#ifdef CM_GW_SUPPORTED


/*===========================================================================

FUNCTION cm_client_sups_cmd_err_ntfy

DESCRIPTION
  Notify a client of a specified phone command error code iff the client is
  registered to be notified of OTHER clients phone command errors and the
  client is NOT the one originally sending this sups command.

  Note that the client originally sending this command is notified of
  the command error via its command supplied callback function.

DEPENDENCIES
  Client must have already been initialized with cm_client_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cm_client_sups_cmd_err_ntfy(

    const cm_client_s_type    *client_ptr,
        /* pointer to a client struct */

    const cm_sups_cmd_s_type    *sups_cmd_ptr,
        /* pointer to phone command struct */

    cm_sups_cmd_err_e_type      sups_cmd_err
        /* phone command error code */
)
{
  CM_ASSERT( client_ptr    != NULL );
  CM_ASSERT( sups_cmd_ptr  != NULL );
  CM_ASSERT( BETWEEN((int)sups_cmd_err, (int)CM_SUPS_CMD_ERR_NONE, (int)CM_SUPS_CMD_ERR_MAX) );

  /* Make sure this object was properly initialized.
  */
  CM_ASSERT( client_ptr->init_mask == CM_INIT_MASK );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( client_ptr != sups_cmd_ptr->client_ptr )
  {
    if( client_ptr->sups_cmd_err_func != NULL )
    {
      client_ptr->sups_cmd_err_func( sups_cmd_ptr->client_id,
                       sups_cmd_ptr->cmd,
                       sups_cmd_err );
    }
  }
}

#endif

/*===========================================================================

FUNCTION cm_client_ac_cmd_err_ntfy

DESCRIPTION
  Notify a client of a specified AC command error code.
===========================================================================*/
static void cm_client_ac_cmd_err_ntfy(

    const cm_client_s_type    *client_ptr,
        /* pointer to a client struct */

    const cm_ac_cmd_s_type  *cmd_ptr,
        /* pointer to AC command struct */

    cm_ac_cmd_err_e_type    cmd_err
        /* AC command error code */
)
{
  CM_ASSERT( client_ptr    != NULL );
  CM_ASSERT( cmd_ptr  != NULL );
  CM_ASSERT( BETWEEN((int)cmd_err, (int)CM_AC_CMD_ERR_NONE, (int)CM_AC_CMD_ERR_MAX) );

  /* Make sure this object was properly initialized.
  */
  CM_ASSERT( client_ptr->init_mask == CM_INIT_MASK );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( client_ptr != cmd_ptr->client_ptr )
  {
    if( client_ptr->ac_cmd_err_func != NULL )
    {
      client_ptr->ac_cmd_err_func( cmd_ptr->client_id, cmd_ptr->cmd,
                                   cmd_err );
    }
  }
}

#ifdef CM_DEBUG
#error code not present
#endif /* CM_DEBUG */

/*===========================================================================

FUNCTION cmclient_call_info_list_ntfy

DESCRIPTION
  Notify a client of call info list iff the client is registered
  for this.

DEPENDENCIES
  Client must have already been initialized with cm_client_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmclient_call_info_list_ntfy (

    const cm_client_s_type       *client_ptr,

    const cm_call_state_info_list_s_type    *call_info_list_ptr
        /* pointer to call state information struct */
)
{

  CM_ASSERT( call_info_list_ptr != NULL );

  /* If callback function exists and bit for receiving call info list
  *  is present, then call the callback.
  */
  if( client_ptr != NULL &&
      client_ptr->init_mask == CM_INIT_MASK &&
      client_ptr->call_info_list_func != NULL)
  {
    client_ptr->call_info_list_func (call_info_list_ptr);
  }

}


/*===========================================================================

FUNCTION cmclient_list_call_event_ntfy

DESCRIPTION
  Notify all the clients in the client list of a specified call event.

DEPENDENCIES
  cmclient_map_init()

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmclient_list_call_event_ntfy(

    cm_call_event_e_type           call_event,
        /* notify clients of this call event */

    cm_mm_call_info_s_type   *call_info_ptr
        /* pointer to call state information struct */
)
{
  int client_id;
  cm_client_s_type *client_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( BETWEEN(call_event, CM_CALL_EVENT_NONE, CM_CALL_EVENT_MAX) );
  CM_ASSERT( call_info_ptr != NULL );
  /*
  ** 571: Suspicious cast
  ** Cast call_event as word so we can use generic function to print it
  */
  /*lint -save -e571 */
  CMDBG_PRINT_CM_EVENT_FUNC( CM_CMD_TYPE_CALL,
    (word) call_event, (const void *) call_info_ptr );
  /*lint -restore */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for ( client_id = 0; client_id < ARR_SIZE( cmclient_map_arr ); client_id++ )
  {
    client_ptr = cmclient_get_client_ptr(client_id);
    if(client_ptr != NULL)
    {
      cm_client_call_event_ntfy( client_ptr, call_event, call_info_ptr );
    }
  }
}


/*===========================================================================

FUNCTION cmclient_list_subs_event_ntfy

DESCRIPTION
  Notify all the clients in the client list of a specified phone event.

DEPENDENCIES
  cmclient_map_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmclient_list_subs_event_ntfy(

    const cm_subs_info_s_type    *subs_info_ptr
        /* pointer to phone state information struct */
)
/*lint -esym(550,client_type) */
{
  int client_id;
  cm_client_type_e_type client_type;
  cm_client_s_type      *client_ptr;
  uint64  client_list = 0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( subs_info_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Traverse client list and notify the
  ** individual client objects of the event.
  */
  for ( client_id = 0; client_id < ARR_SIZE( cmclient_map_arr ); client_id++ )
  {
    rex_enter_crit_sect(cmclient_crit_sect_ptr());
    /* Serialize data access cmclient_map_arr[] only */
    client_ptr = cmclient_map_arr[client_id].client_ptr;
    client_type = cmclient_map_arr[client_id].client_type;
    rex_leave_crit_sect(cmclient_crit_sect_ptr());

    if(client_ptr != NULL)
    {
      if(client_ptr->subs_event_func)
      {
        client_list |= SYS_BM_64BIT(client_type);
        client_ptr->subs_event_func( CM_PH_EVENT_SUBSCRIPTION_PREF_INFO,
                                     subs_info_ptr);
      }
    }
    client_type = CM_CLIENT_TYPE_NONE;
  }

  CM_MSG_LOW_2("subs evt %d ct 0x%x", CM_PH_EVENT_SUBSCRIPTION_PREF_INFO, client_list);

}
/*lint +esym(550,client_type) */

/*===========================================================================

FUNCTION cmclient_list_ph_event_ntfy

DESCRIPTION
  Notify all the clients in the client list of a specified phone event.

DEPENDENCIES
  Initialized with cmclient_map_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmclient_list_ph_event_ntfy(

    cm_ph_event_e_type         ph_event,
        /* notify clients of this phone event */

    cm_ph_info_s_type    *ph_info_ptr
        /* pointer to phone state information struct */
)
{
  int client_id;
  cm_client_s_type *client_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( BETWEEN(ph_event, CM_PH_EVENT_NONE, CM_PH_EVENT_MAX) );
  CM_ASSERT( ph_info_ptr != NULL );
  /*
  ** 571: Suspicious cast
  ** Cast ph_event as word so we can use generic function to print it
  */
  /*lint -save -e571 */
  CMDBG_PRINT_CM_EVENT_FUNC( CM_CMD_TYPE_PH,
    (word) ph_event, (const void *) ph_info_ptr );
  /*lint -restore */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_HIGH_3("ph evt %d ct %d cid id %d",
                ph_event,
                cmclient_get_client_type(ph_info_ptr->requesting_client_id),
                ph_info_ptr->requesting_client_id);

  /* Traverse client list and notify the
  ** individual client objects of the event.
  */
  for ( client_id = 0; client_id < ARR_SIZE( cmclient_map_arr ); client_id++ )
  {
    client_ptr = cmclient_get_client_ptr(client_id);
    if(client_ptr != NULL)
    {
      cm_client_ph_event_ntfy( client_ptr, ph_event, ph_info_ptr );
    }
  }
}

#if ((defined FEATURE_BCMCS) || (defined FEATURE_MBMS))
/*===========================================================================

FUNCTION cmclient_list_bcmcs_event_ntfy

DESCRIPTION
  Notify all the clients in the client list of a specified BCMCS event.

DEPENDENCIES
  Initialized with cmclient_map_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmclient_list_bcmcs_event_ntfy(

    cm_bcmcs_event_e_type         bcmcs_event,
        /* notify clients of this BCMCS event */

    cm_bcmcs_info_s_type         *bcmcs_info_ptr
        /* pointer to BCMCS information struct */
)
{
  int client_id;
  cm_client_s_type *client_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( BETWEEN(bcmcs_event, CM_BCMCS_EVENT_NONE, CM_BCMCS_EVENT_MAX) );

  if (bcmcs_info_ptr == NULL )
  {
    CM_MSG_HIGH_0("bcmcs_info_ptr NULL");
    return;
  }
  /*lint -save -e571 */
  CMDBG_PRINT_CM_EVENT_FUNC( CM_CMD_TYPE_BCMCS,
    (word) bcmcs_event, (const void *) bcmcs_info_ptr );
  /*lint -restore */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Traverse client list and notify the
  ** individual client objects of the event.
  */
  for(client_id = 0; client_id < ARR_SIZE(cmclient_map_arr); client_id++)
  {
    client_ptr = cmclient_get_client_ptr(client_id);
    bcmcs_info_ptr->requesting_client_id = client_id;
    if(client_ptr != NULL)
    {
      cm_client_bcmcs_event_ntfy( client_ptr, bcmcs_event, bcmcs_info_ptr );
    }
  }
}
#endif /* FEATURE_BCMCS || MBMS */


/*===========================================================================

FUNCTION cmclient_list_ss_event_ntfy_new

DESCRIPTION
  Notify all the clients in the client list of a specified serving system
  event.

DEPENDENCIES
  Initialized with cmclient_map_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmclient_list_ss_event_ntfy_new(

    cm_ss_event_e_type            ss_event,
        /* notify clients of this serving system event */

    cm_mm_msim_ss_info_s_type    *ss_info_ptr
        /* pointer to serving system state information struct */
)
{
  int client_id;
  cm_client_s_type *client_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( BETWEEN(ss_event, CM_SS_EVENT_NONE, CM_SS_EVENT_MAX) );
  CM_ASSERT( ss_info_ptr != NULL );
  /*
  ** 571: Suspicious cast
  ** Cast ss_event as word so we can use generic function to print it
  */
  /*lint -save -e571 */
  CMDBG_PRINT_CM_EVENT_FUNC( CM_CMD_TYPE_SS,
    (word) ss_event, (const void *) ss_info_ptr );
  /*lint -restore */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Traverse client list and notify the
  ** individual client objects of the event.
  */
  for(client_id = 0; client_id < ARR_SIZE(cmclient_map_arr); client_id++)
  {
    client_ptr = cmclient_get_client_ptr(client_id);
    ss_info_ptr->requesting_client_id = client_id;
    if(client_ptr != NULL)
    {
      cm_client_ss_event_ntfy_new( client_ptr, ss_event, ss_info_ptr );
    }
  }
}


/*===========================================================================

FUNCTION cmclient_list_ss_event_ntfy

DESCRIPTION
  Notify all the clients in the client list of a specified serving system
  event.

DEPENDENCIES
  Initialized with cmclient_map_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmclient_list_ss_event_ntfy(

    cm_ss_event_e_type            ss_event,
        /* notify clients of this serving system event */

    cm_mm_ss_info_s_type    *ss_info_ptr
        /* pointer to serving system state information struct */
)
{
  int client_id;
  cm_client_s_type *client_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( BETWEEN(ss_event, CM_SS_EVENT_NONE, CM_SS_EVENT_MAX) );
  CM_ASSERT( ss_info_ptr != NULL );
  /*
  ** 571: Suspicious cast
  ** Cast ss_event as word so we can use generic function to print it
  */
  /*lint -save -e571 */
  CMDBG_PRINT_CM_EVENT_FUNC( CM_CMD_TYPE_SS,
    (word) ss_event, (const void *) ss_info_ptr );
  /*lint -restore */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Traverse client list and notify the
  ** individual client objects of the event.
  */
  for(client_id = 0; client_id < ARR_SIZE(cmclient_map_arr); client_id++)
  {
    client_ptr = cmclient_get_client_ptr(client_id);
    ss_info_ptr->requesting_client_id = client_id;
    if(client_ptr != NULL)
    {
      cm_client_ss_event_ntfy( client_ptr, ss_event, ss_info_ptr );
    }
  }
}

/*===========================================================================

FUNCTION cmclient_list_inband_event_ntfy

DESCRIPTION
  Notify all the clients in the client list of a specified inband event.

DEPENDENCIES
  Initialized with cmclient_map_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmclient_list_inband_event_ntfy(

    cm_inband_event_e_type         inband_event,
        /* notify clients of this inband event */

    cm_inband_info_s_type    *inband_info_ptr
        /* pointer to inband information struct */
)
{
  int client_id;
  cm_client_s_type *client_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( BETWEEN(inband_event, CM_INBAND_EVENT_NONE, CM_INBAND_EVENT_MAX) );
  CM_ASSERT( inband_info_ptr != NULL );
  /*
  ** 571: Suspicious cast
  ** Cast inband_event as word so we can use generic function to print it
  */
  /*lint -save -e571 */
  CMDBG_PRINT_CM_EVENT_FUNC( CM_CMD_TYPE_INBAND,
    (word) inband_event, (const void *) inband_info_ptr );
  /*lint -restore */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Traverse client list and notify the
  ** individual client objects of the event.
  */
  for(client_id = 0; client_id < ARR_SIZE(cmclient_map_arr); client_id++)
  {
    client_ptr = cmclient_get_client_ptr(client_id);
    if(client_ptr != NULL)
    {
      cm_client_inband_event_ntfy( client_ptr, inband_event, inband_info_ptr );
    }
  }
}


/*===========================================================================

FUNCTION cmclient_list_sms_event_ntfy

DESCRIPTION
  Notify all the clients in the client list of a specified sms event.

DEPENDENCIES
  Initialized with cmclient_map_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmclient_list_sms_event_ntfy(

    cm_sms_event_e_type         sms_event,
        /* notify clients of this sms event */

    cm_sms_info_s_type    *sms_info_ptr
        /* pointer to sms information struct */
)
{
  int client_id;
  cm_client_s_type *client_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( BETWEEN(sms_event, CM_SMS_EVENT_NONE, CM_SMS_EVENT_MAX) );
  CM_ASSERT( sms_info_ptr != NULL );
  /*
  ** 571: Suspicious cast
  ** Cast sms_event as word so we can use generic function to print it
  */
  /*lint -save -e571 */
  CMDBG_PRINT_CM_EVENT_FUNC( CM_CMD_TYPE_SMS,
    (word) sms_event, (const void *) sms_info_ptr );
  /*lint -restore */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Traverse client list and notify the
  ** individual client objects of the event.
  */
  for(client_id = 0; client_id < ARR_SIZE(cmclient_map_arr); client_id++)
  {
    client_ptr = cmclient_get_client_ptr(client_id);
    if(client_ptr != NULL)
    {
      cm_client_sms_event_ntfy( client_ptr, sms_event, sms_info_ptr );
    }
  }
}


#ifdef FEATURE_GPSONE

/*===========================================================================

FUNCTION cmclient_list_pd_event_ntfy

DESCRIPTION
  Notify all the clients in the client list of a specified pd event.

DEPENDENCIES
  Initialized with cmclient_map_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmclient_list_pd_event_ntfy(
    cm_pd_event_e_type         pd_event,
        /* notify clients of this pd event */

    cm_pd_info_s_type    *pd_info_ptr
        /* pointer to pd information struct */
)
{
  int client_id;
  cm_client_s_type *client_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( BETWEEN(pd_event, CM_PD_EVENT_NONE, CM_PD_EVENT_MAX) );
  CM_ASSERT( pd_info_ptr != NULL );
  /*
  ** 571: Suspicious cast
  ** Cast pd_event as word so we can use generic function to print it
  */
  /*lint -save -e571 */
  CMDBG_PRINT_CM_EVENT_FUNC( CM_CMD_TYPE_PD,
    (word) pd_event, (const void *) pd_info_ptr );
  /*lint -restore */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Traverse client list and notify the
  ** individual client objects of the event.
  */
  for(client_id = 0; client_id < ARR_SIZE(cmclient_map_arr); client_id++)
  {
    client_ptr = cmclient_get_client_ptr(client_id);
    if(client_ptr != NULL)
    {
      cm_client_pd_event_ntfy(client_ptr, pd_event, pd_info_ptr);
    }
  }
}
#endif

/*===========================================================================

FUNCTION cmclient_list_dbm_event_ntfy

DESCRIPTION
  Notify all the clients in the client list of a specified dbm event.

DEPENDENCIES
  Initialized with cmclient_map_init()..

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmclient_list_dbm_event_ntfy(
    cm_dbm_event_e_type         dbm_event,
        /* notify clients of this dbm event */

    cm_dbm_info_s_type    *dbm_info_ptr
        /* pointer to dbm information struct */
)
{
  int client_id;
  cm_client_s_type *client_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( BETWEEN(dbm_event, CM_DBM_EVENT_NONE, CM_DBM_EVENT_MAX) );
  CM_ASSERT( dbm_info_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Traverse client list and notify the
  ** individual client objects of the event.
  */
  for(client_id = 0; client_id < ARR_SIZE(cmclient_map_arr); client_id++)
  {
    client_ptr = cmclient_get_client_ptr(client_id);
    if(client_ptr != NULL)
    {
      cm_client_dbm_event_ntfy(client_ptr, dbm_event, dbm_info_ptr);
    }
  }
}

/*===========================================================================

FUNCTION cmclient_list_ac_event_ntfy

DESCRIPTION
  Notify all the clients in the client list of a specified access control
  events
  
DEPENDENCIES
  Initialized with cmclient_map_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmclient_list_ac_event_ntfy(

    cm_ac_event_e_type event,
        /* notify clients of this access control event */

    cm_ac_info_s_type *info_ptr
        /* pointer to access control state information struct */
)
{
  int client_id;
  cm_client_s_type *client_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for(client_id = 0; client_id < ARR_SIZE(cmclient_map_arr); client_id++)
  {
    client_ptr = cmclient_get_client_ptr(client_id);
    if(client_ptr != NULL)
    {
      cm_client_ac_event_ntfy( client_ptr, event, info_ptr );
    }
  }
}

#if defined (CM_GW_SUPPORTED) || defined (FEATURE_IP_CALL)

/*===========================================================================

FUNCTION cmclient_list_sups_event_ntfy

DESCRIPTION
  Notify all the clients in the client list of a specified SUPS event.

DEPENDENCIES
  Initialized with cmclient_map_init()..

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmclient_list_sups_event_ntfy (

             cm_sups_event_e_type    sups_event,
                  /* notify clients of this sups event */

             cm_sups_info_s_type     *sups_info_ptr
                  /* pointer to sups information struct */
)
{
  int client_id;
  cm_client_s_type  *client_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  CM_ASSERT( BETWEEN(sups_event, CM_SUPS_EVENT_NONE, CM_SUPS_EVENT_MAX) );

  CM_ASSERT( sups_info_ptr != NULL );
  /*
  ** 571: Suspicious cast
  ** Cast sups_event as word so we can use generic function to print it
  */
  /*lint -e571 */
  CMDBG_PRINT_CM_EVENT_FUNC( CM_CMD_TYPE_SUPS,
    (word) sups_event, (const void *) sups_info_ptr );
  /*lint -e571 */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
   /* Traverse client list and notify the
      individual client objects of the event.
   */

  for(client_id = 0; client_id < ARR_SIZE(cmclient_map_arr); client_id++)
  {
    client_ptr = cmclient_get_client_ptr(client_id);
    if(client_ptr != NULL)
    {
      cm_client_sups_event_ntfy( client_ptr,
                                 sups_event,
                                 sups_info_ptr );
    }
  }

}

#if (defined FEATURE_MBMS)
#error code not present
#endif /* #if defined (FEATURE_MBMS) */

/*===========================================================================

FUNCTION cmclient_list_mbms_event_ntfy

DESCRIPTION
  Notify all the clients in the client list of a specified NAS event.

DEPENDENCIES
  Initialized with cmclient_map_init()..

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmclient_list_mbms_event_ntfy(

    cm_mbms_event_e_type         mbms_event,
        /* notify clients of this NAS event */

    cm_mbms_info_s_type         *mbms_info_ptr
        /* pointer to NAS information struct */
)
/*lint -esym(715,mbms_event) */
/*lint -esym(715,mbms_info_ptr) */
/*lint -esym(818, mbms_info_ptr) */
{
  #if (defined FEATURE_MBMS)
  #error code not present
#else
  CM_MSG_HIGH_0("FEATURE_MBMS not defined");
  #endif /* #if defined (FEATURE_MBMS) */

}
/*lint +esym(715,mbms_event) */
/*lint +esym(715,mbms_info_ptr) */
/*lint +esym(818,mbms_info_ptr) */
#endif

#ifdef CM_GW_SUPPORTED
#ifdef FEATURE_WMS_CM_NAS_INTERFACES
#error code not present
#endif /* FEATURE_WMS_CM_NAS_INTERFACES */
#endif /* FEATURE_GSM || FEATURE_WCDMA */

#ifdef FEATURE_APPS_POWER_COLLAPSE
/*===========================================================================

FUNCTION cmclient_list_power_collapse_buffered_events_ntfy

DESCRIPTION
  Notify the clients of their suspended events that are buffered during
  power collapse.

  Internal Info get commands are queued based on the type of events buffered
  during power collapse.

DEPENDENCIES
  Initialized with cmclient_map_init()..

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmclient_list_power_collapse_buffered_events_ntfy(void)
{
  cm_client_s_type              *client_ptr;

  static cm_ss_cmd_s_type           *ss_cmd_ptr;

  cm_ph_info_s_type          *ph_info_ptr = NULL;
      /* pointer for phone info */
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Traverse client list and notify the
  ** individual client objects of the event.
  */
  for( client_ptr = cmclient_list_first();
       client_ptr != NULL;
       client_ptr = cmclient_list_next(client_ptr) )
  {
    if(client_ptr->client_processor == CM_PROCESSOR_MODEM)
    {
      continue;
    }

    /* If any of the Ph events are buffered during power collapse
    ** then send the ph state by queuing ph_info_get command.
    */
    if(bit_arr_is_any_bit_on(&client_ptr->ph_power_collapse_buf_bit_arr, 0,
                             client_ptr->ph_power_collapse_buf_bit_arr.bit_size - 1))
    {
      if(ph_info_ptr == NULL)
      {
        /* get a buffer for phone info */
        ph_info_ptr = cmph_info_alloc();

        /* Copy phone information into
        ** allocated phone info buffer.
        */
        (void) cmph_info_get( ph_info_ptr, CM_SS_MAIN );
      }

      /* notify only the requesting client
         Note: Client ID is set to the beginning address
               of the client object during the client initialization
      */
      cm_client_ph_event_ntfy(client_ptr,
                              CM_PH_EVENT_INFO,
                              ph_info_ptr );

      if ( bit_arr_is_bit_on(&client_ptr->ph_power_collapse_buf_bit_arr,
           (int)CM_PH_EVENT_OPRT_MODE ))
      {
        cm_client_ph_event_ntfy( client_ptr,
                                 CM_PH_EVENT_OPRT_MODE,
                                 ph_info_ptr );
      }

      if (cmph_is_msim())
      {
        /* Copy phone information into
        ** allocated phone info buffer.
        */
        (void) cmph_info_get( ph_info_ptr, CM_SS_HYBR_2 );
  
        cm_client_ph_event_ntfy(client_ptr,
                                CM_PH_EVENT_INFO,
                                ph_info_ptr );
      

      #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
      /* Copy phone information into
      ** allocated phone info buffer.
      */
      (void) cmph_info_get( ph_info_ptr, CM_SS_HYBR_3 );

      cm_client_ph_event_ntfy(client_ptr,
                              CM_PH_EVENT_INFO,
                              ph_info_ptr );
      #endif
      }

      /* Deallocate the memory after sending the ph event. */
      cm_mem_free(ph_info_ptr);

      /* Clear power collapse ph event buf bit array.
      */
      bit_arr_init( &client_ptr->ph_power_collapse_buf_bit_arr,
                     client_ptr->ph_power_collapse_buf_bit_buf,
                  (8 * ARR_SIZE(client_ptr->ph_power_collapse_buf_bit_buf)) );
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* If any of the SS events are buffered during power collapse
    ** then send the SS state  by queuing ss_info_get command.
    */
    if(bit_arr_is_any_bit_on(&client_ptr->ss_power_collapse_buf_bit_arr, 0,
                             client_ptr->ss_power_collapse_buf_bit_arr.bit_size - 1))
    {
      if( ss_cmd_ptr == NULL )
      {
        ss_cmd_ptr = cm_cmd_alloc_ss_init();

      }
      ss_cmd_ptr->cmd_type = CM_CMD_TYPE_SS;
      /* Copy command header fields.
      */
      ss_cmd_ptr->cmd_cb_func     = NULL;
      ss_cmd_ptr->data_block_ptr  = NULL;
      ss_cmd_ptr->client_id       = cmclient_get_client_id(client_ptr);
      ss_cmd_ptr->client_ptr      = client_ptr;
      ss_cmd_ptr->cmd             = CM_SS_CMD_INFO_GET;

      cmss_send_ss_info(ss_cmd_ptr, CM_SS_EVENT_INFO);

      /* Clear power collapse Serving system bit array.
      */
      bit_arr_init( &client_ptr->ss_power_collapse_buf_bit_arr,
                    client_ptr->ss_power_collapse_buf_bit_buf,
                    (8 * ARR_SIZE(client_ptr->ss_power_collapse_buf_bit_buf)) );
    }
  }
}
#endif /* FEATURE_APPS_POWER_COLLAPSE */


/*===========================================================================

FUNCTION cmclient_list_call_cmd_err_ntfy

DESCRIPTION
  Notify all the clients in the client list of a specified call command
  error.

DEPENDENCIES
  Initialized with cmclient_map_init()..

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmclient_list_call_cmd_err_ntfy(

    cm_call_cmd_err_e_type      call_cmd_err,
        /* call command error code */

    const cm_call_cmd_s_type    *call_cmd_ptr
        /* pointer to call command struct */

)
{
  int client_id;
  cm_client_s_type *client_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( call_cmd_ptr != NULL );
  CM_ASSERT( BETWEEN(call_cmd_err, CM_CALL_CMD_ERR_NONE, CM_CALL_CMD_ERR_MAX) );
  /*
  ** 571: Suspicious cast
  ** Cast call_cmd_err as word so we can use generic function to print it
  */
  /*lint -save -e571 */
  CMDBG_PRINT_CM_CMDERR_FUNC( CM_CMD_TYPE_CALL, (word) call_cmd_err,
    (const void *) call_cmd_ptr );
  /*lint -restore */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Traverse client list and notify the
  ** individual client object of the error.
  */
  for(client_id = 0; client_id < ARR_SIZE(cmclient_map_arr); client_id++)
  {
    client_ptr = cmclient_get_client_ptr(client_id);
    if(client_ptr != NULL)
    {
      cm_client_call_cmd_err_ntfy( client_ptr,
                                   call_cmd_ptr,
                                   call_cmd_err );
    }
  }
}




/*===========================================================================

FUNCTION cmclient_list_ph_cmd_err_ntfy

DESCRIPTION
  Notify all the clients in the client list of a specified phone command
  error.

DEPENDENCIES
  Initialized with cmclient_map_init()..

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmclient_list_ph_cmd_err_ntfy(

    cm_ph_cmd_err_e_type      ph_cmd_err,
        /* phone command error code */

    const cm_ph_cmd_s_type    *ph_cmd_ptr
        /* pointer to ph command struct */

)
{
  int client_id;
  cm_client_s_type *client_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( ph_cmd_ptr != NULL );
  CM_ASSERT( BETWEEN(ph_cmd_err, CM_PH_CMD_ERR_NONE, CM_PH_CMD_ERR_MAX) );
  /*
  ** 571: Suspicious cast
  ** Cast ph_cmd_err as word so we can use generic function to print it
  */
  /*lint -save -e571 */
  CMDBG_PRINT_CM_CMDERR_FUNC( CM_CMD_TYPE_PH, (word) ph_cmd_err,
    (const void *) ph_cmd_ptr );
  /*lint -restore */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Traverse client list and notify the
  ** individual client object of the error.
  */
  for(client_id = 0; client_id < ARR_SIZE(cmclient_map_arr); client_id++)
  {
    client_ptr = cmclient_get_client_ptr(client_id);
    if(client_ptr != NULL)
    {
      cm_client_ph_cmd_err_ntfy( client_ptr,
                                 ph_cmd_ptr,
                                 ph_cmd_err );
    }
  }
}

/*===========================================================================

FUNCTION cmclient_list_stats_cmd_err_ntfy

DESCRIPTION
  Notify all the clients in the client list of a specified stats command
  error.

DEPENDENCIES
  Initialized with cmclient_map_init()..

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmclient_list_stats_cmd_err_ntfy(

    cm_stats_cmd_err_e_type      stats_cmd_err,
        /* phone command error code */

    const cm_stats_cmd_s_type    *stats_cmd_ptr
        /* pointer to ph command struct */

)
/*lint -esym(715, stats_cmd_err)*/
/*lint -esym(715, stats_cmd_ptr)*/
{
  int client_id;
  cm_client_s_type *client_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( stats_cmd_ptr != NULL );
  CM_ASSERT( BETWEEN(stats_cmd_err, CM_STATS_CMD_ERR_NONE, CM_STATS_CMD_ERR_MAX) );
  /*
  ** 571: Suspicious cast
  ** Cast ph_cmd_err as word so we can use generic function to print it
  */
  /*lint -save -e571 */
  CMDBG_PRINT_CM_CMDERR_FUNC( CM_CMD_TYPE_STATS, (word) stats_cmd_err,
    (const void *) stats_cmd_ptr );
  /*lint -restore */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Traverse client list and notify the
  ** individual client object of the error.
  */
  for(client_id = 0; client_id < ARR_SIZE(cmclient_map_arr); client_id++)
  {
    client_ptr = cmclient_get_client_ptr(client_id);
    if(client_ptr != NULL)
    {
      cm_client_stats_cmd_err_ntfy( client_ptr,
                                    stats_cmd_ptr,
                                    stats_cmd_err );
    }
  }
}
/*lint +esym(715, stats_cmd_err)*/
/*lint +esym(715, stats_cmd_ptr)*/


  #if ((defined FEATURE_BCMCS) || (defined FEATURE_MBMS))
/*===========================================================================

FUNCTION cmclient_list_bcmcs_cmd_err_ntfy

DESCRIPTION
  Notify all the clients in the client list of a specified BCMCS command
  error.

DEPENDENCIES
  Initialized with cmclient_map_init()..

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmclient_list_bcmcs_cmd_err_ntfy(

    cm_bcmcs_cmd_err_e_type      bcmcs_cmd_err,
        /* phone command error code */

    const cm_bcmcs_cmd_s_type    *bcmcs_cmd_ptr
        /* pointer to BCMCS command struct */

)
{
  int client_id;
  cm_client_s_type *client_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( bcmcs_cmd_ptr != NULL );
  CM_ASSERT( BETWEEN(bcmcs_cmd_err, CM_BCMCS_CMD_ERR_NONE, CM_BCMCS_CMD_ERR_MAX) );
  /*lint -save -e571 */
  CMDBG_PRINT_CM_CMDERR_FUNC( CM_CMD_TYPE_BCMCS, (word) bcmcs_cmd_err,
    (const void *) bcmcs_cmd_ptr );
  /*lint -restore */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Traverse client list and notify the
  ** individual client object of the error.
  */
  for(client_id = 0; client_id < ARR_SIZE(cmclient_map_arr); client_id++)
  {
    client_ptr = cmclient_get_client_ptr(client_id);
    if(client_ptr != NULL)
    {
      cm_client_bcmcs_cmd_err_ntfy( client_ptr,
                                 bcmcs_cmd_ptr,
                                 bcmcs_cmd_err );
    }
  }
}
#endif /* FEATURE_BCMCS || MBMS */
/*===========================================================================

FUNCTION cmclient_list_mbms_cmd_err_ntfy

DESCRIPTION
  Notify all the clients in the client list of a specified MBMS command
  error.

DEPENDENCIES
  Initialized with cmclient_map_init()..

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmclient_list_mbms_cmd_err_ntfy(

    cm_mbms_cmd_err_e_type        mbms_cmd_err,
        /* phone command error code */

    const cm_mbms_cmd_s_type    *mbms_cmd_ptr
        /* pointer to MBMS command struct */

)
/*lint -esym(715,mbms_cmd_err) */
/*lint -esym(715,mbms_cmd_ptr) */
/*lint -esym(714,cmclient_list_mbms_cmd_err_ntfy) */
/*lint -esym(765,cmclient_list_mbms_cmd_err_ntfy)
** Can't be static as it would result in compiler warnings
*/
{
  #if (defined FEATURE_MBMS)
  #error code not present
#else
  CM_MSG_HIGH_0("FEATURE_MBMS not defined");
  #endif /* defined (FEATURE_MBMS) */
}
/*lint +esym(715,mbms_cmd_err) */
/*lint +esym(715,mbms_cmd_ptr) */


/*===========================================================================

FUNCTION cmss_list_ss_cmd_err_ntfy

DESCRIPTION
  Notify all the clients in the client list of a specified serving system command
  error.

DEPENDENCIES
  Initialized with cmclient_map_init()..

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmclient_list_ss_cmd_err_ntfy(

    cm_ss_cmd_err_e_type      ss_cmd_err,
        /* call command error code */

    const cm_ss_cmd_s_type    *ss_cmd_ptr
        /* pointer to ss command struct */

)
{
  int client_id;
  cm_client_s_type *client_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( ss_cmd_ptr != NULL );
  CM_ASSERT( BETWEEN(ss_cmd_err, CM_SS_CMD_ERR_NONE, CM_SS_CMD_ERR_MAX) );
  /*
  ** 571: Suspicious cast
  ** Cast ss_cmd_err as word so we can use generic function to print it
  */
  /*lint -save -e571 */
  CMDBG_PRINT_CM_CMDERR_FUNC( CM_CMD_TYPE_SS, (word) ss_cmd_err,
    (const void *) ss_cmd_ptr );
  /*lint -restore */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Traverse client list and notify the
  ** individual client object of the error.
  */
  for(client_id = 0; client_id < ARR_SIZE(cmclient_map_arr); client_id++)
  {
    client_ptr = cmclient_get_client_ptr(client_id);
    if(client_ptr != NULL)
    {
      cm_client_ss_cmd_err_ntfy( client_ptr,
                                 ss_cmd_ptr,
                                 ss_cmd_err );
    }
  }
}


/*===========================================================================

FUNCTION cmclient_list_inband_cmd_err_ntfy

DESCRIPTION
  Notify all the clients in the client list of a specified inband command
  error.

DEPENDENCIES
  Initialized with cmclient_map_init()..

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmclient_list_inband_cmd_err_ntfy(

    cm_inband_cmd_err_e_type      inband_cmd_err,
        /* phone command error code */

    const cm_inband_cmd_s_type    *inband_cmd_ptr
        /* pointer to inband command struct */

)
{
  int client_id;
  cm_client_s_type *client_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( inband_cmd_ptr != NULL );
  CM_ASSERT( BETWEEN(inband_cmd_err, CM_INBAND_CMD_ERR_NONE, CM_INBAND_CMD_ERR_MAX) );
  /*
  ** 571: Suspicious cast
  ** Cast inband_cmd_err as word so we can use generic function to print it
  */
  /*lint -save -e571 */
  CMDBG_PRINT_CM_CMDERR_FUNC( CM_CMD_TYPE_INBAND, (word) inband_cmd_err,
    (const void *) inband_cmd_ptr );
  /*lint -restore */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Traverse client list and notify the
  ** individual client object of the error.
  */
  for(client_id = 0; client_id < ARR_SIZE(cmclient_map_arr); client_id++)
  {
    client_ptr = cmclient_get_client_ptr(client_id);
    if(client_ptr != NULL)
    {
      cm_client_inband_cmd_err_ntfy( client_ptr,
                                     inband_cmd_ptr,
                                     inband_cmd_err );
    }
  }
}


/*===========================================================================

FUNCTION cmclient_list_sms_cmd_err_ntfy

DESCRIPTION
  Notify all the clients in the client list of a specified SMS command
  error.

DEPENDENCIES
  Initialized with cmclient_map_init()..

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmclient_list_sms_cmd_err_ntfy(

    cm_sms_cmd_err_e_type      sms_cmd_err,
        /* phone command error code */

    const cm_sms_cmd_s_type    *sms_cmd_ptr
        /* pointer to sms command struct */

)
{
  int client_id;
  cm_client_s_type *client_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( sms_cmd_ptr != NULL );
  CM_ASSERT( BETWEEN(sms_cmd_err, CM_SMS_CMD_ERR_NONE, CM_SMS_CMD_ERR_MAX) );
  /*
  ** 571: Suspicious cast
  ** Cast sms_cmd_err as word so we can use generic function to print it
  */
  /*lint -save -e571 */
  CMDBG_PRINT_CM_CMDERR_FUNC( CM_CMD_TYPE_SMS, (word) sms_cmd_err,
    (const void *) sms_cmd_ptr );
  /*lint -restore */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Traverse client list and notify the
  ** individual client object of the error.
  */
  for(client_id = 0; client_id < ARR_SIZE(cmclient_map_arr); client_id++)
  {
    client_ptr = cmclient_get_client_ptr(client_id);
    if(client_ptr != NULL)
    {
      cm_client_sms_cmd_err_ntfy( client_ptr,
                                  sms_cmd_ptr,
                                  sms_cmd_err );
    }
  }
}

#ifdef FEATURE_GPSONE
/*===========================================================================

FUNCTION cmclient_list_pd_cmd_err_ntfy

DESCRIPTION
  Notify all the clients in the client list of a specified PD command
  error.

DEPENDENCIES
  Initialized with cmclient_map_init()..

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/

void cmclient_list_pd_cmd_err_ntfy(
    cm_pd_cmd_err_e_type      pd_cmd_err,
        /* phone command error code */

    const cm_pd_cmd_s_type    *pd_cmd_ptr
        /* pointer to sms command struct */
)
{
  int client_id;
  cm_client_s_type *client_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( pd_cmd_ptr != NULL );
  CM_ASSERT( BETWEEN(pd_cmd_err, CM_PD_CMD_ERR_NONE, CM_PD_CMD_ERR_MAX) );
  /*
  ** 571: Suspicious cast
  ** Cast pd_cmd_err as word so we can use generic function to print it
  */
  /*lint -save -e571 */
  CMDBG_PRINT_CM_CMDERR_FUNC( CM_CMD_TYPE_PD, (word) pd_cmd_err,
    (const void *) pd_cmd_ptr );
  /*lint -restore */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Traverse client list and notify the
  ** individual client object of the error.
  */
  for(client_id = 0; client_id < ARR_SIZE(cmclient_map_arr); client_id++)
  {
    client_ptr = cmclient_get_client_ptr(client_id);
    if(client_ptr != NULL)
    {
      cm_client_pd_cmd_err_ntfy( client_ptr,
                                 pd_cmd_ptr,
                                 pd_cmd_err);
    }
  }
}

#endif



/*===========================================================================

FUNCTION cmclient_list_dbm_cmd_err_ntfy

DESCRIPTION
  Notify all the clients in the client list of a specified DBM command
  error.

DEPENDENCIES
  Initialized with cmclient_map_init()..

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/

void cmclient_list_dbm_cmd_err_ntfy(
    cm_dbm_cmd_err_e_type      dbm_cmd_err,
        /* phone command error code */

    const cm_dbm_cmd_s_type    *dbm_cmd_ptr
        /* pointer to sms command struct */
)
{
  int client_id;
  cm_client_s_type *client_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( dbm_cmd_ptr != NULL );
  CM_ASSERT( BETWEEN(dbm_cmd_err, CM_DBM_CMD_ERR_NONE, CM_DBM_CMD_ERR_MAX) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Traverse client list and notify the
  ** individual client object of the error.
  */
  for(client_id = 0; client_id < ARR_SIZE(cmclient_map_arr); client_id++)
  {
    client_ptr = cmclient_get_client_ptr(client_id);
    if(client_ptr != NULL)
    {
      cm_client_dbm_cmd_err_ntfy( client_ptr,
                                  dbm_cmd_ptr,
                                  dbm_cmd_err);
    }
  }
}

#ifdef CM_GW_SUPPORTED


/*===========================================================================

FUNCTION cmclient_list_sups_cmd_err_ntfy

DESCRIPTION
  Notify all the clients in the client list of a specified sups command
  error.

DEPENDENCIES
  Initialized with cmclient_map_init()..

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmclient_list_sups_cmd_err_ntfy(

    cm_sups_cmd_err_e_type      sups_cmd_err,
        /* sups command error code */

    const cm_sups_cmd_s_type    *sups_cmd_ptr
        /* pointer to sups command struct */

)
{
  int client_id;
  cm_client_s_type *client_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( sups_cmd_ptr != NULL );
  CM_ASSERT( BETWEEN(sups_cmd_err, CM_SUPS_CMD_ERR_NONE, CM_SUPS_CMD_ERR_MAX) );
  /*
  ** 571: Suspicious cast
  ** Cast sups_cmd_err as word so we can use generic function to print it
  */
  /*lint -save -e571 */
  CMDBG_PRINT_CM_CMDERR_FUNC( CM_CMD_TYPE_SUPS, (word) sups_cmd_err,
    (const void *) sups_cmd_ptr );
  /*lint -restore */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Traverse client list and notify the
  ** individual client object of the error.
  */
  for( client_id = 0; client_id < ARR_SIZE(cmclient_map_arr); client_id++ )
  {
    client_ptr = cmclient_get_client_ptr(client_id);
    if(client_ptr != NULL)
    {
      cm_client_sups_cmd_err_ntfy( client_ptr,
                                   sups_cmd_ptr,
                                   sups_cmd_err );
    }
  }
}

#endif

/*===========================================================================

FUNCTION cmclient_list_ac_cmd_err_ntfy

DESCRIPTION
  Notify all the clients in the client list of a specified AC command
  error.

===========================================================================*/
void cmclient_list_ac_cmd_err_ntfy(

    cm_ac_cmd_err_e_type      cmd_err,
        /* AC command error code */

    const cm_ac_cmd_s_type    *cmd_ptr
        /* pointer to AC command struct */

)
{
  int client_id;
  cm_client_s_type *client_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( cmd_ptr != NULL );
  CM_ASSERT( BETWEEN(cmd_err, CM_AC_CMD_ERR_NONE, CM_AC_CMD_ERR_MAX) );

  /*
  ** 571: Suspicious cast
  ** Cast cmd_err as word so we can use generic function to print it
  */
  /*lint -save -e571 */
  CMDBG_PRINT_CM_CMDERR_FUNC( CM_CMD_TYPE_AC, (word) cmd_err,
                                                    (const void *) cmd_ptr );
  /*lint -restore */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for( client_id = 0; client_id < ARR_SIZE(cmclient_map_arr); client_id++ )
  {
    client_ptr = cmclient_get_client_ptr(client_id);
    if(client_ptr != NULL)
    {
      cm_client_ac_cmd_err_ntfy( client_ptr, cmd_ptr, cmd_err );
    }
  }
}

/*===========================================================================

FUNCTION cm_client_validate_reg

DESCRIPTION
  Validate all parameters received by client registration APIs.

DEPENDENCIES
  Initialized with cmclient_map_init().

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
static cm_client_status_e_type cm_client_validate_reg(

  cm_client_event_reg_e_type event_reg_type,
  int from_event,
  int to_event,
  int event_low_limit,
  int event_high_limit
)
{
  if (!BETWEEN(from_event, event_low_limit, event_high_limit))
  {
      return CM_CLIENT_ERR_FROM_EVENT;
  }

  if (!BETWEEN(to_event, event_low_limit, event_high_limit))
  {
    return CM_CLIENT_ERR_TO_EVENT;
  }

  if (!BETWEEN(event_reg_type, CM_CLIENT_EVENT_NONE, CM_CLIENT_EVENT_MAX))
  {
   return CM_CLIENT_ERR_TO_EVENT;
  }

  if ( from_event > to_event )
  {
     return CM_CLIENT_ERR_FROM_EVENT;
  }

  return CM_CLIENT_OK;
}

/*===========================================================================

FUNCTION cm_client_validate_asubs_mask

DESCRIPTION
  Validate asubs_mask received by client registration APIs.

DEPENDENCIES
  Initialized with cmclient_map_init().

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
static cm_client_status_e_type cm_client_validate_asubs_mask(
  uint8 asubs_mask
)
{
  if(asubs_mask > SYS_MODEM_AS_ID_ALL_ACTIVE_MASK)
  {
      return CM_CLIENT_ERR_ASUBS_MASK;
  }

  return CM_CLIENT_OK;
}

/*===========================================================================

FUNCTION cm_client_send_cmd

DESCRIPTION
  Populates data received from CM-clients in cmd. Queues client command to CM.

DEPENDENCIES
  Initialized with cmclient_map_init().

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
static cm_client_status_e_type cm_client_send_cmd(

  cm_client_id_type client_id,
  cm_client_event_reg_e_type event_reg_type,
  int from_event,
  int to_event,
  int event_low_limit,
  int event_high_limit,
  void *event_func,
  void *cmd_err_func,
  cm_client_cmd_e_type cmd
)
{
  cm_client_status_e_type return_status;
  cm_client_s_type *client_ptr;
  cm_client_cmd_s_type  *client_cmd_ptr;
  cm_client_gen_reg_data_s_type *reg_data = NULL;

  return_status = cm_client_validate_reg(event_reg_type,
                                      from_event, to_event,
                                      event_low_limit, event_high_limit);
  if(return_status != CM_CLIENT_OK)
  {
    return return_status;
  }

  client_ptr = cmclient_get_client_ptr(client_id);
  if(client_ptr == NULL)
  {
    return CM_CLIENT_ERR_CLIENT_ID;
  }
  
  client_cmd_ptr  = cm_cmd_alloc_client_init();

  client_cmd_ptr->client_id = client_id;
  client_cmd_ptr->client_ptr = client_ptr;
  client_cmd_ptr->cmd = cmd;

  reg_data = &client_cmd_ptr->info;

  reg_data->event_reg_type = event_reg_type;
  reg_data->from_event = from_event;
  reg_data->to_event = to_event;
  reg_data->event_func = event_func;
  reg_data->cmd_err_func = cmd_err_func;

  cm_cmd_queue( (cm_cmd_type*)client_cmd_ptr  );
   
  return CM_CLIENT_OK;
}

/*===========================================================================

FUNCTION cm_client_send_cmd_msim

DESCRIPTION
  Populates data received from CM-clients in cmd. Queues client command to CM.

DEPENDENCIES
  Initialized with cmclient_map_init().

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
static cm_client_status_e_type cm_client_send_cmd_msim(

  cm_client_id_type client_id,
  cm_client_event_reg_e_type event_reg_type,
  int from_event,
  int to_event,
  int event_low_limit,
  int event_high_limit,
  void *event_func,
  void *cmd_err_func,
  cm_client_cmd_e_type cmd,
  uint8 asubs_mask
)
{
  cm_client_status_e_type return_status;
  cm_client_s_type *client_ptr;
  cm_client_cmd_s_type  *client_cmd_ptr;
  cm_client_gen_reg_data_s_type *reg_data = NULL;

  return_status = cm_client_validate_reg(event_reg_type,
                                      from_event, to_event,
                                      event_low_limit, event_high_limit);
  if(return_status != CM_CLIENT_OK)
  {
    return return_status;
  }

  client_ptr = cmclient_get_client_ptr(client_id);
  if(client_ptr == NULL)
  {
    return CM_CLIENT_ERR_CLIENT_ID;
  }

  return_status = cm_client_validate_asubs_mask(asubs_mask);

  if(return_status != CM_CLIENT_OK)
  {
    return return_status;
  }
  
  client_cmd_ptr  = cm_cmd_alloc_client_init();

  client_cmd_ptr->client_id = client_id;
  client_cmd_ptr->client_ptr = client_ptr;
  client_cmd_ptr->cmd = cmd;

  reg_data = &client_cmd_ptr->info;

  reg_data->event_reg_type = event_reg_type;
  reg_data->from_event = from_event;
  reg_data->to_event = to_event;
  reg_data->event_func = event_func;
  reg_data->cmd_err_func = cmd_err_func;
  reg_data->asubs_mask = asubs_mask;

  cm_cmd_queue( (cm_cmd_type*)client_cmd_ptr  );
   
  return CM_CLIENT_OK;
}

/*===========================================================================

FUNCTION cm_mm_client_call_reg

DESCRIPTION
  This function allows a client to register the following call-related
  callbacks and events with CM:

   1. Callback function to receive registered call events
   2. Register and de-register call events
   3. Callback function to receive other clients' call-command errors.

   If a client registers a callback function to receive call event
   notification, it also needs to register one or more call events it is
   interested in receiving. This API has a cumulative effect for event
   registration. For example, in order to register for two different ranges
   of call events, a client needs to call on this API twice- once for each
   range of call events.

  If a client registers a callback function to receive other clients'
  call-command errors, it will receive notifications on all possible
  call-command errors caused by other clients. A client cannot register to
  receive only a subset of call-command errors. The NULL function pointer can
  also be used for the callback function if a client is not interested in
  receiving that callback. For example, if a client is not interested in
  receiving other clients' call-command errors, it can set
  call_cmd_err_func = NULL. However, if the call_event_func is set to NULL,
  all registered call events are also de-registered and the event_reg_type,
  from_call_event, and to_call_event parameters are ignored.

DEPENDENCIES
  Client must have already been initialized with cm_client_init().

RETURN VALUE
  request status

SIDE EFFECTS
  none

===========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif
cm_client_status_e_type cm_mm_client_call_reg(

    cm_client_id_type           client_id,
        /* client id */

    cm_mm_call_event_f_type     *call_event_func,
        /* Pointer to a callback function to notify the client of call
        ** events */

    cm_client_event_reg_e_type  event_reg_type,

    cm_call_event_e_type        from_call_event,
        /* register from this event (including) */

    cm_call_event_e_type        to_call_event,
          /* to this event (including) */

    cm_mm_call_cmd_err_f_type   *call_cmd_err_func
      /* pointer to a callback function to notify the client of call
      ** command errors of OTHER clients */
)
{
  return cm_client_send_cmd(client_id, event_reg_type,
                            (int)from_call_event, (int)to_call_event,
                            (int)CM_CALL_EVENT_NONE, (int)CM_CALL_EVENT_MAX,
                            call_event_func, call_cmd_err_func,
                            CM_CLIENT_CMD_CALL_REG);
}
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif

/*===========================================================================

FUNCTION cm_mm_client_call_pwr_collapse_reg

DESCRIPTION
  This function allows a client to register/de-register the events it should
  be notified during power collapse. If the client wants to be notified of
  all the events that it is registered for in the normal (power restored
  state), it should use CM_PWR_COLLAPSE_NOTIFY_NORMAL as the
  pwr_collapse_type. If the client wants CM to decide on "important events"
  to wake up the processor from the power collapse state, it should use
  CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_DEFAULT. If the client wants to override
  CM's default, it should use CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_CUSTOM.

  The from_call_event and to_call_event, and event_reg_type are used only
  if the pwr_collapse_type is CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_CUSTOM. If
  event_reg_type is CM_CLIENT_EVENT_REG, the event will be notified while in
  power collapse. If the event_reg_type is CM_CLIENT_EVENT_DEREG, the event
  will not be reported while in power collapse.

  This API has a cumulative effect for event registration.
  For example, in order to regieter/de-register for two different ranges
  of call events, a client needs to call on this API twice- once for each
  range of call events. In case of conflicting pwr_collapse_type, the last
  one will be honored


DEPENDENCIES
  Client must have already been initialized with cm_client_init().


RETURN VALUE
  request status

SIDE EFFECTS
  none

===========================================================================*/
cm_client_status_e_type cm_mm_client_call_pwr_collapse_reg(

    cm_client_id_type             client,
        /* client id */

    cm_mm_call_event_f_type      *call_event_func,
        /* Pointer to a callback function to notify the client of call
        ** events */

    cm_processor_e_type           proc_id,
        /* The processor on which the client is executing */

    cm_pwr_collapse_notify_e_type pwr_collapse_type,
        /* The power collapse type, None, Normal, default, custom */

    cm_client_event_reg_e_type    event_reg_type,
        /* Registration type */

    cm_call_event_e_type          from_call_event,
        /* register from this event (including) */

    cm_call_event_e_type          to_call_event
        /* to this event (including) */
)
/*lint -esym(715,client) */
/*lint -esym(715,call_event_func) */
/*lint -esym(715,proc_id) */
/*lint -esym(715,pwr_collapse_type) */
/*lint -esym(715,event_reg_type) */
/*lint -esym(715,from_call_event) */
/*lint -esym(715,to_call_event) */
{
  #ifdef FEATURE_CM_APPS_POWER_COLLAPSE
#error code not present
#else
  return CM_CLIENT_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE;
  #endif

} /* cm_mm_client_call_pwr_collapse_reg */
/*lint +esym(715,client) */
/*lint +esym(715,call_event_func) */
/*lint +esym(715,proc_id) */
/*lint +esym(715,pwr_collapse_type) */
/*lint +esym(715,event_reg_type) */
/*lint +esym(715,from_call_event) */
/*lint +esym(715,to_call_event) */


/*===========================================================================

@FUNCTION cm_client_subs_reg

@DESCRIPTION
  This function allows a client to register the following subscription-related
  callbacks and events with CM:

   1. Callback function to receive registered subscription events
   2. Register and de-register subscription events
   3. Callback function to receive other clients' phone-command errors.

@DEPENDENCIES
  Client must have already been initialized with cm_client_init().

@RETURNS
  request status

@SIDE_EFFECT
  none

@x
===========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif
cm_client_status_e_type cm_client_subs_reg(

  cm_client_id_type               client_id,
    /**< Requesting client */

  cm_subs_event_f_type            *subs_event_func
    /**< Pointer to a callback function to notify the client of phone
    ** events */

)
{

  cm_client_cmd_s_type  *client_cmd_ptr;
  cm_client_gen_reg_data_s_type *reg_data = NULL;
  cm_client_s_type *client_ptr = NULL;

  client_ptr = cmclient_get_client_ptr(client_id);

  if(client_ptr == NULL)
  {
      return CM_CLIENT_ERR_CLIENT_ID;
  }

  client_cmd_ptr  = cm_cmd_alloc_client_init();

  client_cmd_ptr->client_id = client_id;
  client_cmd_ptr->client_ptr = client_ptr;
  client_cmd_ptr->cmd = CM_CLIENT_CMD_SUBS_REG;

  reg_data = &client_cmd_ptr->info;

  reg_data->event_func = subs_event_func;

  cm_cmd_queue( (cm_cmd_type*)client_cmd_ptr  );
   
  return CM_CLIENT_OK;

}

#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif

/*===========================================================================

FUNCTION cm_client_ph_reg

DESCRIPTION
  This function allows a client to register the following phone-related
  callbacks and events with CM:

   1. Callback function to receive registered phone events
   2. Register and de-register phone events
   3. Callback function to receive other clients' phone-command errors.

   If a client registers a callback function to receive phone event
   notification, it also needs to register one or more phone events it is
   interested in receiving. This API has a cumulative effect for event
   registration. For example, in order to register for two different ranges
   of call events, a client needs to call on this API twice- once for each
   range of call events.

  If a client registers a callback function to receive other clients'
  phone-command errors, it will receive notifications on all possible
  phone-command errors caused by other clients. A client cannot register to
  receive only a subset of phone-command errors. The NULL function pointer can
  also be used for the callback function if a client is not interested in
  receiving that callback. For example, if a client is not interested in
  receiving other clients' phone-command errors, it can set
  ph_cmd_err_func = NULL. However, if the ph_event_func is set to NULL,
  all registered phone events are also de-registered and the event_reg_type,
  from_ph_event, and to_ph_event parameters are ignored.

DEPENDENCIES
  Client must have already been initialized with cm_client_init().

RETURN VALUE
  request status

SIDE EFFECTS
  none

===========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif
cm_client_status_e_type cm_client_ph_reg(

    cm_client_id_type           client_id,
        /* client id */

    cm_ph_event_f_type          *ph_event_func,
        /* Pointer to a callback function to notify the client of phone
        ** events */

    cm_client_event_reg_e_type  event_reg_type,

    cm_ph_event_e_type          from_ph_event,
        /* register from this event (including) */

    cm_ph_event_e_type          to_ph_event,
          /* to this event (including) */

    cm_ph_cmd_err_f_type        *ph_cmd_err_func
      /* pointer to a callback function to notify the client of phone
      ** command errors of OTHER clients */
)
{
  return cm_client_send_cmd(client_id, event_reg_type, (int)from_ph_event,
                           (int)to_ph_event, (int)CM_PH_EVENT_NONE,
                           (int)CM_PH_EVENT_MAX, ph_event_func,
                           ph_cmd_err_func, CM_CLIENT_CMD_PH_REG);
}

#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif


/*===========================================================================

FUNCTION cm_mm_client_ph_pwr_collapse_reg

DESCRIPTION
  This function allows a client to register/de-register the events it should
  be notified during power collapse. If the client wants to be notified of
  all the events that it is registered for in the normal (power restored
  state), it should use CM_PWR_COLLAPSE_NOTIFY_NORMAL as the
  pwr_collapse_type. If the client wants CM to decide on "important events"
  to wake up the processor from the power collapse state, it should use
  CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_DEFAULT. If the client wants to override
  CM's default, it should use CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_CUSTOM.

  The from_ph_event and to_ph_event, and event_reg_type are used only
  if the pwr_collapse_type is CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_CUSTOM. If
  event_reg_type is CM_CLIENT_EVENT_REG, the event will be notified while in
  power collapse. If the event_reg_type is CM_CLIENT_EVENT_DEREG, the event
  will not be reported while in power collapse.

  This API has a cumulative effect for event registration.
  For example, in order to regieter/de-register for two different ranges
  of ph events, a client needs to call on this API twice- once for each
  range of ph events. In case of conflicting pwr_collapse_type, the last
  one will be honored


DEPENDENCIES
  Client must have already been initialized with cm_client_init().


RETURN VALUE
  request status

SIDE EFFECTS
  none

===========================================================================*/
cm_client_status_e_type cm_mm_client_ph_pwr_collapse_reg(

    cm_client_id_type             client,
        /* client id */

    cm_ph_event_f_type          *ph_event_func,
        /* Pointer to a callback function to notify the client of call
        ** events */

    cm_processor_e_type           proc_id,
        /* The processor on which the client is executing */

    cm_pwr_collapse_notify_e_type pwr_collapse_type,
        /* The power collapse type, None, Normal, default, custom */

    cm_client_event_reg_e_type    event_reg_type,
        /* Registration type */

    cm_ph_event_e_type            from_ph_event,
        /* register from this event (including) */

    cm_ph_event_e_type            to_ph_event
        /* to this event (including) */
)
/*lint -esym(715,client) */
/*lint -esym(715,ph_event_func) */
/*lint -esym(715,proc_id) */
/*lint -esym(715,pwr_collapse_type) */
/*lint -esym(715,event_reg_type) */
/*lint -esym(715,from_ph_event) */
/*lint -esym(715,to_ph_event) */
{
  #ifdef FEATURE_CM_APPS_POWER_COLLAPSE
#error code not present
#else
  return CM_CLIENT_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE;
  #endif

} /* cm_mm_client_ph_pwr_collapse_reg */
/*lint +esym(715,client) */
/*lint +esym(715,ph_event_func) */
/*lint +esym(715,proc_id) */
/*lint +esym(715,pwr_collapse_type) */
/*lint +esym(715,event_reg_type) */
/*lint +esym(715,from_ph_event) */
/*lint +esym(715,to_ph_event) */

/*===========================================================================

FUNCTION cm_mm_client_ss_reg_msim

DESCRIPTION
  This function allows a client to register the following serving system-related
  callbacks and events with CM:

   1. Callback function to receive registered serving system events
   2. Register and de-register serving system events
   3. Callback function to receive other clients' serving system-command errors.

   If a client registers a callback function to receive serving system event
   notification, it also needs to register one or more serving system events it is
   interested in receiving. This API has a cumulative effect for event
   registration. For example, in order to register for two different ranges
   of serving system events, a client needs to call on this API twice- once for each
   range of serving system events.

  If a client registers a callback function to receive other clients'
  serving system-command errors, it will receive notifications on all possible
  serving system-command errors caused by other clients. A client cannot register to
  receive only a subset of serving system-command errors. The NULL function pointer can
  also be used for the callback function if a client is not interested in
  receiving that callback. For example, if a client is not interested in
  receiving other clients' serving system-command errors, it can set
  ss_cmd_err_func = NULL. However, if the ss_event_func is set to NULL,
  all registered serving system events are also de-registered and the event_reg_type,
  from_ss_event, and to_ss_event parameters are ignored.

DEPENDENCIES
  Client must have already been initialized with cm_client_init().

RETURN VALUE
  request status

SIDE EFFECTS
  none

===========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif
cm_client_status_e_type cm_mm_client_ss_reg_msim(

    cm_client_id_type           client_id,
        /* client id */

    cm_mm_msim_ss_event_f_type  *ss_event_func,
        /* Pointer to a callback function to notify the client of serving system
           ** events */

    cm_client_event_reg_e_type  event_reg_type,

    cm_ss_event_e_type          from_ss_event,
        /* register from this event (including) */

    cm_ss_event_e_type          to_ss_event,
          /* to this event (including) */

    cm_ss_cmd_err_f_type        *ss_cmd_err_func,
      /* pointer to a callback function to notify the client of serving system
      ** command errors of OTHER clients */

    uint8                       asubs_mask
      /* The bit mask of asubs_id(s) that the client register the events for */

)
{
  return cm_client_send_cmd_msim(client_id, event_reg_type, (int)from_ss_event,
                           (int)to_ss_event, (int)CM_SS_EVENT_NONE,
                           (int)CM_SS_EVENT_MAX, ss_event_func,
                           ss_cmd_err_func, CM_CLIENT_CMD_SS_REG_MSIM, asubs_mask);
}

#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif


/*===========================================================================

FUNCTION cm_mm_client_ss_pwr_collapse_reg

DESCRIPTION
  This function allows a client to register/de-register the events it should
  be notified during power collapse. If the client wants to be notified of
  all the events that it is registered for in the normal (power restored
  state), it should use CM_PWR_COLLAPSE_NOTIFY_NORMAL as the
  pwr_collapse_type. If the client wants CM to decide on "important events"
  to wake up the processor from the power collapse state, it should use
  CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_DEFAULT. If the client wants to override
  CM's default, it should use CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_CUSTOM.

  The from_ss_event and to_ss_event, and event_reg_type are used only
  if the pwr_collapse_type is CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_CUSTOM. If
  event_reg_type is CM_CLIENT_EVENT_REG, the event will be notified while in
  power collapse. If the event_reg_type is CM_CLIENT_EVENT_DEREG, the event
  will not be reported while in power collapse.

  This API has a cumulative effect for event registration.
  For example, in order to regieter/de-register for two different ranges
  of ss events, a client needs to call on this API twice- once for each
  range of ph events. In case of conflicting pwr_collapse_type, the last
  one will be honored


DEPENDENCIES
  Client must have already been initialized with cm_client_init().


RETURN VALUE
  request status

SIDE EFFECTS
  none

===========================================================================*/
cm_client_status_e_type cm_mm_client_ss_pwr_collapse_reg(

    cm_client_id_type             client,
        /* client id */

    cm_mm_ss_event_f_type        *ss_event_func,
        /* Pointer to a callback function to notify the client of call
           ** events */

    cm_processor_e_type           proc_id,
        /* The processor on which the client is executing */

    cm_pwr_collapse_notify_e_type pwr_collapse_type,
        /* The power collapse type, None, Normal, default, custom */

    cm_client_event_reg_e_type  event_reg_type,
        /* Registration type */

    cm_ss_event_e_type          from_ss_event,
        /* register from this event (including) */

    cm_ss_event_e_type            to_ss_event
          /* to this event (including) */
)
/*lint -esym(715,client) */
/*lint -esym(715,ss_event_func) */
/*lint -esym(715,proc_id) */
/*lint -esym(715,pwr_collapse_type) */
/*lint -esym(715,event_reg_type) */
/*lint -esym(715,from_ss_event) */
/*lint -esym(715,to_ss_event) */
{
  #ifdef FEATURE_CM_APPS_POWER_COLLAPSE
#error code not present
#else
  return CM_CLIENT_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE;
#endif

} /* cm_mm_client_ss_pwr_collapse_reg */
/*lint +esym(715,client) */
/*lint +esym(715,ss_event_func) */
/*lint +esym(715,proc_id) */
/*lint +esym(715,pwr_collapse_type) */
/*lint +esym(715,event_reg_type) */
/*lint +esym(715,from_ss_event) */
/*lint +esym(715,to_ss_event) */

/*===========================================================================

FUNCTION cm_mm_client_ss_pwr_collapse_reg_msim

DESCRIPTION
  This function allows a client to register/de-register the events it should
  be notified during power collapse. If the client wants to be notified of
  all the events that it is registered for in the normal (power restored
  state), it should use CM_PWR_COLLAPSE_NOTIFY_NORMAL as the
  pwr_collapse_type. If the client wants CM to decide on "important events"
  to wake up the processor from the power collapse state, it should use
  CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_DEFAULT. If the client wants to override
  CM's default, it should use CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_CUSTOM.

  The from_ss_event and to_ss_event, and event_reg_type are used only
  if the pwr_collapse_type is CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_CUSTOM. If
  event_reg_type is CM_CLIENT_EVENT_REG, the event will be notified while in
  power collapse. If the event_reg_type is CM_CLIENT_EVENT_DEREG, the event
  will not be reported while in power collapse.

  This API has a cumulative effect for event registration.
  For example, in order to regieter/de-register for two different ranges
  of ss events, a client needs to call on this API twice- once for each
  range of ph events. In case of conflicting pwr_collapse_type, the last
  one will be honored


DEPENDENCIES
  Client must have already been initialized with cm_client_init().


RETURN VALUE
  request status

SIDE EFFECTS
  none

===========================================================================*/
cm_client_status_e_type cm_mm_client_ss_pwr_collapse_reg_msim(

    cm_client_id_type             client,
        /* client id */

    cm_mm_msim_ss_event_f_type        *msim_ss_event_func,
        /* Pointer to a callback function to notify the client of call
        ** events */

    cm_processor_e_type           proc_id,
        /* The processor on which the client is executing */

    cm_pwr_collapse_notify_e_type pwr_collapse_type,
        /* The power collapse type, None, Normal, default, custom */

    cm_client_event_reg_e_type    event_reg_type,
        /* Registration type */

    cm_ss_event_e_type            from_ss_event,
        /* register from this event (including) */

    cm_ss_event_e_type            to_ss_event
        /* to this event (including) */
)
/*lint -esym(715,client) */
/*lint -esym(715,ss_event_func) */
/*lint -esym(715,proc_id) */
/*lint -esym(715,pwr_collapse_type) */
/*lint -esym(715,event_reg_type) */
/*lint -esym(715,from_ss_event) */
/*lint -esym(715,to_ss_event) */
{
  #ifdef FEATURE_CM_APPS_POWER_COLLAPSE
#error code not present
#else
  return CM_CLIENT_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE;
  #endif

} /* cm_mm_client_ss_pwr_collapse_reg */
/*lint +esym(715,client) */
/*lint +esym(715,ss_event_func) */
/*lint +esym(715,proc_id) */
/*lint +esym(715,pwr_collapse_type) */
/*lint +esym(715,event_reg_type) */
/*lint +esym(715,from_ss_event) */
/*lint +esym(715,to_ss_event) */


/*===========================================================================

FUNCTION cm_mm_client_inband_reg

DESCRIPTION
  This function allows a client to register the following inband-related
  callbacks and events with CM:

   1. Callback function to receive registered inband events
   2. Register and de-register inband events
   3. Callback function to receive other clients' inband-command errors.

   If a client registers a callback function to receive inband event
   notification, it also needs to register one or more inband events it is
   interested in receiving. This API has a cumulative effect for event
   registration. For example, in order to register for two different ranges
   of inband events, a client needs to call on this API twice- once for each
   range of inband events.

  If a client registers a callback function to receive other clients'
  inband-command errors, it will receive notifications on all possible
  inband-command errors caused by other clients. A client cannot register to
  receive only a subset of inband-command errors. The NULL function pointer can
  also be used for the callback function if a client is not interested in
  receiving that callback. For example, if a client is not interested in
  receiving other clients' inband-command errors, it can set
  inband_cmd_err_func = NULL. However, if the inband_event_func is set to NULL,
  all registered inband events are also de-registered and the event_reg_type,
  from_inbnad_event, and to_inband_event parameters are ignored.

DEPENDENCIES
  Client must have already been initialized with cm_client_init().

RETURN VALUE
  request status

SIDE EFFECTS
  none

===========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif

cm_client_status_e_type cm_mm_client_inband_reg(

    cm_client_id_type               client_id,
        /* client id */

    cm_inband_event_f_type          *inband_event_func,
        /* Pointer to a callback function to notify the client of inband
        ** events */

    cm_client_event_reg_e_type      event_reg_type,

    cm_inband_event_e_type          from_inband_event,
        /* register from this event (including) */

    cm_inband_event_e_type          to_inband_event,
          /* to this event (including) */

    cm_mm_inband_cmd_err_f_type     *inband_cmd_err_func
      /* pointer to a callback function to notify the client of inband
      ** command errors of OTHER clients */
)
{
  return cm_client_send_cmd(client_id, event_reg_type, (int)from_inband_event,
                            (int)to_inband_event, (int)CM_INBAND_EVENT_NONE,
                            (int)CM_INBAND_EVENT_MAX, inband_event_func,
                            inband_cmd_err_func, CM_CLIENT_CMD_INBAND_REG);
}

#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif

/*===========================================================================

FUNCTION cm_mm_client_inband_pwr_collapse_reg

DESCRIPTION
  This function allows a client to register/de-register the events it should
  be notified during power collapse. If the client wants to be notified of
  all the events that it is registered for in the normal (power restored
  state), it should use CM_PWR_COLLAPSE_NOTIFY_NORMAL as the
  pwr_collapse_type. If the client wants CM to decide on "important events"
  to wake up the processor from the power collapse state, it should use
  CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_DEFAULT. If the client wants to override
  CM's default, it should use CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_CUSTOM.

  The from_inband_event and to_inband_event, and event_reg_type are used only
  if the pwr_collapse_type is CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_CUSTOM. If
  event_reg_type is CM_CLIENT_EVENT_REG, the event will be notified while in
  power collapse. If the event_reg_type is CM_CLIENT_EVENT_DEREG, the event
  will not be reported while in power collapse.

  This API has a cumulative effect for event registration.
  For example, in order to regieter/de-register for two different ranges
  of inband events, a client needs to call on this API twice- once for each
  range of ph events. In case of conflicting pwr_collapse_type, the last
  one will be honored


DEPENDENCIES
  Client must have already been initialized with cm_client_init().


RETURN VALUE
  request status

SIDE EFFECTS
  none

===========================================================================*/
cm_client_status_e_type cm_mm_client_inband_pwr_collapse_reg(

    cm_client_id_type             client,
        /* client id */

    cm_inband_event_f_type       *inband_event_func,
        /* Pointer to a callback function to notify the client of call
        ** events */

    cm_processor_e_type           proc_id,
        /* The processor on which the client is executing */

    cm_pwr_collapse_notify_e_type pwr_collapse_type,
        /* The power collapse type, None, Normal, default, custom */

    cm_client_event_reg_e_type    event_reg_type,
        /* Registration type */

    cm_inband_event_e_type        from_inband_event,
        /* register from this event (including) */

    cm_inband_event_e_type        to_inband_event
        /* to this event (including) */
)
/*lint -esym(715,client) */
/*lint -esym(715,inband_event_func) */
/*lint -esym(715,proc_id) */
/*lint -esym(715,pwr_collapse_type) */
/*lint -esym(715,event_reg_type) */
/*lint -esym(715,from_inband_event) */
/*lint -esym(715,to_inband_event) */
{
  #ifdef FEATURE_CM_APPS_POWER_COLLAPSE
#error code not present
#else
  return CM_CLIENT_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE;
  #endif

} /* cm_mm_client_inband_pwr_collapse_reg */
/*lint +esym(715,client) */
/*lint +esym(715,inband_event_func) */
/*lint +esym(715,proc_id) */
/*lint +esym(715,pwr_collapse_type) */
/*lint +esym(715,event_reg_type) */
/*lint +esym(715,from_inband_event) */
/*lint +esym(715,to_inband_event) */


/*===========================================================================

FUNCTION cm_client_sms_reg

DESCRIPTION
  This function allows a client to register the following sms-related
  callbacks and events with CM:

   1. Callback function to receive registered sms events
   2. Register and de-register sms events
   3. Callback function to receive other clients'sms-command errors.

   If a client registers a callback function to receive inband event
   notification, it also needs to register one or more inband events it is
   interested in receiving. This API has a cumulative effect for event
   registration. For example, in order to register for two different ranges
   of sms events, a client needs to call on this API twice- once for each
   range of sms events.

  If a client registers a callback function to receive other clients'
  sms-command errors, it will receive notifications on all possible
  sms-command errors caused by other clients. A client cannot register to
  receive only a subset of sms-command errors. The NULL function pointer can
  also be used for the callback function if a client is not interested in
  receiving that callback. For example, if a client is not interested in
  receiving other clients' sms-command errors, it can set
  sms_cmd_err_func = NULL. However, if the sms_event_func is set to NULL,
  all registered sms events are also de-registered and the event_reg_type,
  from_sms_event, and to_sms_event parameters are ignored.

DEPENDENCIES
  Client must have already been initialized with cm_client_init().

RETURN VALUE
  request status

SIDE EFFECTS
  none

===========================================================================*/
cm_client_status_e_type cm_client_sms_reg(

    cm_client_id_type               client_id,
        /* Requesting client */

    cm_sms_event_f_type             *sms_event_func,
        /* Pointer to a callback function to notify the client of sms
        ** events */

    cm_client_event_reg_e_type      event_reg_type,

    cm_sms_event_e_type             from_sms_event,
        /* register from this event (including) */

    cm_sms_event_e_type             to_sms_event,
          /* to this event (including) */


    cm_sms_cmd_err_f_type           *sms_cmd_err_func
      /* pointer to a callback function to notify the client of sms
      ** command errors of OTHER clients */

)
/*lint -esym(715,client_id) */
/*lint -esym(715,sms_event_func) */
/*lint -esym(715,event_reg_type) */
/*lint -esym(715,from_sms_event) */
/*lint -esym(715,to_sms_event) */
/*lint -esym(715,sms_cmd_err_func) */
{
  #ifdef FEATURE_CDSMS

  return cm_client_send_cmd(client_id, event_reg_type, (int)from_sms_event,
                           (int)to_sms_event, (int)CM_SMS_EVENT_NONE,
                           (int)CM_SMS_EVENT_MAX, sms_event_func,
                           sms_cmd_err_func, CM_CLIENT_CMD_SMS_REG);
 
  #else
  return CM_CLIENT_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE;
  #endif /* FEATURE_CDSMS */

}
/*lint +esym(715,client_id) */
/*lint +esym(715,sms_event_func) */
/*lint +esym(715,event_reg_type) */
/*lint +esym(715,from_sms_event) */
/*lint +esym(715,to_sms_event) */
/*lint +esym(715,sms_cmd_err_func) */

/*===========================================================================

FUNCTION cm_mm_client_sms_pwr_collapse_reg

DESCRIPTION
  This function allows a client to register/de-register the events it should
  be notified during power collapse. If the client wants to be notified of
  all the events that it is registered for in the normal (power restored
  state), it should use CM_PWR_COLLAPSE_NOTIFY_NORMAL as the
  pwr_collapse_type. If the client wants CM to decide on "important events"
  to wake up the processor from the power collapse state, it should use
  CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_DEFAULT. If the client wants to override
  CM's default, it should use CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_CUSTOM.

  The from_sms_event and to_sms_event, and event_reg_type are used only
  if the pwr_collapse_type is CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_CUSTOM. If
  event_reg_type is CM_CLIENT_EVENT_REG, the event will be notified while in
  power collapse. If the event_reg_type is CM_CLIENT_EVENT_DEREG, the event
  will not be reported while in power collapse.

  This API has a cumulative effect for event registration.
  For example, in order to regieter/de-register for two different ranges
  of sms events, a client needs to call on this API twice- once for each
  range of ph events. In case of conflicting pwr_collapse_type, the last
  one will be honored


DEPENDENCIES
  Client must have already been initialized with cm_client_init().


RETURN VALUE
  request status

SIDE EFFECTS
  none

===========================================================================*/
cm_client_status_e_type cm_mm_client_sms_pwr_collapse_reg(

    cm_client_id_type             client,
        /* client id */

    cm_sms_event_f_type          *sms_event_func,
        /* Pointer to a callback function to notify the client of call
        ** events */

    cm_processor_e_type           proc_id,
        /* The processor on which the client is executing */

    cm_pwr_collapse_notify_e_type pwr_collapse_type,
        /* The power collapse type, None, Normal, default, custom */

    cm_client_event_reg_e_type    event_reg_type,
        /* Registration type */

    cm_sms_event_e_type           from_sms_event,
        /* register from this event (including) */

    cm_sms_event_e_type           to_sms_event
        /* to this event (including) */
)
  /*lint -esym(715,client)*/
  /*lint -esym(715,proc_id)*/
  /*lint -esym(715,pwr_collapse_type)*/
  /*lint -esym(715,event_reg_type)*/
  /*lint -esym(715,from_sms_event)*/
  /*lint -esym(715,to_sms_event)*/
  /*lint -esym(715,sms_event_func) */

{
  #if defined(FEATURE_CDSMS) && defined(FEATURE_CM_APPS_POWER_COLLAPSE)
#error code not present
#else
  return CM_CLIENT_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE;
  #endif

} /* cm_mm_client_sms_pwr_collapse_reg */
/*lint +esym(715,client)*/
/*lint +esym(715,proc_id)*/
/*lint +esym(715,pwr_collapse_type)*/
/*lint +esym(715,event_reg_type)*/
/*lint +esym(715,from_sms_event)*/
/*lint +esym(715,to_sms_event)*/
/*lint +esym(715,sms_event_func)*/



/*===========================================================================

FUNCTION cm_mm_client_call_info_list_reg

DESCRIPTION
  This function allows a client to register the following call-related
  callbacks with CM:

   1. Callback function to receive call information list.
   2. Register and de-register function for call information list
   3. Callback function to receive other clients'  errors.


  If a client registers a callback function to receive other clients'
  call-command errors, it will receive notifications on all possible
  call-command errors caused by other clients. A client cannot register to
  receive only a subset of call-command errors. The NULL function pointer can
  also be used for the callback function if a client is not interested in
  receiving that callback. For example, if a client is not interested in
  receiving other clients' call-command errors, it can set
  call_info_list_cmd_err_func = NULL.

DEPENDENCIES
  Client must have already been initialized with cm_client_init().

RETURN VALUE
  request status

SIDE EFFECTS
  none

===========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif
cm_client_status_e_type cm_mm_client_call_info_list_reg(

    cm_client_id_type           client_id,
        /* client id */

    cm_call_info_list_f_type     *call_info_list_func,
        /* Pointer to a callback function to notify the client of call
        ** events */

    cm_client_event_reg_e_type  event_reg_type,

    cm_mm_call_cmd_err_f_type   *call_info_list_cmd_err_func
      /* pointer to a callback function to notify the client of call
      ** command errors of OTHER clients */
)
/*lint -esym(715,client_id) */
/*lint -esym(715,call_info_list_func) */
/*lint -esym(715,event_reg_type) */
/*lint -esym(715,call_info_list_cmd_err_func) */
{
  cm_client_cmd_s_type* client_cmd_ptr;
  cm_client_gen_reg_data_s_type *reg_data = NULL;
  cm_client_s_type *client_ptr = NULL;

  if (!BETWEEN(event_reg_type, CM_CLIENT_EVENT_NONE, CM_CLIENT_EVENT_MAX))
  {
   return CM_CLIENT_ERR_TO_EVENT;
  }

  client_ptr = cmclient_get_client_ptr(client_id);
  if(client_ptr == NULL)
  {
      return CM_CLIENT_ERR_CLIENT_ID;
  }

  client_cmd_ptr  = cm_cmd_alloc_client_init();

  client_cmd_ptr->client_id = client_id;
  client_cmd_ptr->client_ptr = client_ptr;
  client_cmd_ptr->cmd = CM_CLIENT_CMD_CALL_LIST_REG;

  reg_data = &client_cmd_ptr->info;

  reg_data->event_reg_type = event_reg_type;
  reg_data->event_func = call_info_list_func;
  reg_data->cmd_err_func = call_info_list_cmd_err_func;

  cm_cmd_queue( (cm_cmd_type*)client_cmd_ptr  );
   
  return CM_CLIENT_OK;
}
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif
/*lint +esym(715,client_id) */
/*lint +esym(715,call_info_list_func) */
/*lint +esym(715,event_reg_type) */
/*lint +esym(715,call_info_list_cmd_err_func) */
/*===========================================================================

FUNCTION cm_client_pd_reg

DESCRIPTION
  This function is to be used by CM clients interested in registering for
  Position Determination call manager events.

DEPENDENCIES
  cm_client_init must have been called previously.

RETURN VALUE
  results of the registration.

SIDE EFFECTS
  None

===========================================================================*/

cm_client_status_e_type cm_client_pd_reg(

    cm_client_id_type             client_id,
        /* Requesting client_id */

    cm_pd_event_f_type           *pd_event_func,
        /* Pointer to a callback function to notify the client_id of pd
        ** events */

    cm_client_event_reg_e_type    event_reg_type,
        /* Is this a registration or de-reg */

    cm_pd_event_e_type            from_pd_event,
        /* register from this event (including) */

    cm_pd_event_e_type            to_pd_event,
        /* to this event (including) */


    cm_pd_cmd_err_f_type         *pd_cmd_err_func
      /* pointer to a callback function to notify the client_id of pd
      ** command errors of OTHER clients */

)
/*lint -esym(715,client_id) */
/*lint -esym(715,pd_event_func) */
/*lint -esym(715,event_reg_type) */
/*lint -esym(715,from_pd_event) */
/*lint -esym(715,to_pd_event) */
/*lint -esym(715,pd_cmd_err_func) */
{
  #ifdef FEATURE_GPSONE
  boolean reg_flag;
  struct cm_client_s  *client_ptr;

  client_ptr = cmclient_get_client_ptr( client_id );

  /* verify client_id pointer */
  if ( (client_ptr == NULL) || (client_ptr->init_mask != CM_INIT_MASK) )
  {
     return CM_CLIENT_ERR_CLIENT_ID;
  }

  if(pd_event_func == NULL)
  {
     /* deregister all of client_id's events */

     bit_arr_init( &client_ptr->pd_bit_arr,               /* bit array struct*/
                   client_ptr->pd_bit_buf,                     /* bit buffer */
                   (8 * ARR_SIZE(client_ptr->pd_bit_buf)) );   /* bit size   */

     client_ptr->pd_event_func   = pd_event_func;
     client_ptr->pd_cmd_err_func = pd_cmd_err_func;

     return CM_CLIENT_OK;
  }


  if (!BETWEEN(from_pd_event, CM_PD_EVENT_NONE, CM_PD_EVENT_MAX))
  {
      return CM_CLIENT_ERR_FROM_EVENT;
  }

  if (!BETWEEN(to_pd_event, CM_PD_EVENT_NONE, CM_PD_EVENT_MAX))
  {
    return CM_CLIENT_ERR_TO_EVENT;
  }

  if ( from_pd_event > to_pd_event )
  {
     return CM_CLIENT_ERR_FROM_EVENT;
  }

  switch(event_reg_type)
  {
     case CM_CLIENT_EVENT_REG:
             reg_flag=TRUE;
             break;

     case CM_CLIENT_EVENT_DEREG:
             reg_flag=FALSE;
             break;

     default:
             return CM_CLIENT_ERR_EVENT_REG_TYPE;
  }

  bit_arr_set_bits( &client_ptr->pd_bit_arr,       /* for this bit array */
                    reg_flag,                      /* set bits to ON/off */
                    (int)from_pd_event,            /* start from this */
                    (int)to_pd_event);             /* end with this bit */


  /* update SMS event and other clients' command error callback functions */

  client_ptr->pd_event_func   = pd_event_func;
  client_ptr->pd_cmd_err_func = pd_cmd_err_func;

  return CM_CLIENT_OK;
  #else /* FEATURE_GPSONE */
  return CM_CLIENT_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE;
  #endif /* else of FEATURE_GPSONE */

}
/*lint +esym(715,client_id) */
/*lint +esym(715,pd_event_func) */
/*lint +esym(715,event_reg_type) */
/*lint +esym(715,from_pd_event) */
/*lint +esym(715,to_pd_event) */
/*lint +esym(715,pd_cmd_err_func) */

/*===========================================================================

FUNCTION cm_mm_client_pd_pwr_collapse_reg

DESCRIPTION
  DESCRIPTION
  This function allows a client to register/de-register the events it should
  be notified during power collapse. If the client wants to be notified of
  all the events that it is registered for in the normal (power restored
  state), it should use CM_PWR_COLLAPSE_NOTIFY_NORMAL as the
  pwr_collapse_type. If the client wants CM to decide on "important events"
  to wake up the processor from the power collapse state, it should use
  CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_DEFAULT. If the client wants to override
  CM's default, it should use CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_CUSTOM.

  The from_pd_event and to_pd_event, and event_reg_type are used only
  if the pwr_collapse_type is CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_CUSTOM. If
  event_reg_type is CM_CLIENT_EVENT_REG, the event will be notified while in
  power collapse. If the event_reg_type is CM_CLIENT_EVENT_DEREG, the event
  will not be reported while in power collapse.

  This API has a cumulative effect for event registration.
  For example, in order to regieter/de-register for two different ranges
  of pd events, a client needs to call on this API twice- once for each
  range of ph events. In case of conflicting pwr_collapse_type, the last
  one will be honored

DEPENDENCIES
  Client must have already been initialized with cm_client_init().


RETURN VALUE
  request status

SIDE EFFECTS
  none

===========================================================================*/
cm_client_status_e_type cm_mm_client_pd_pwr_collapse_reg(

    cm_client_id_type             client,
        /* client id */

    cm_pd_event_f_type           *pd_event_func,
        /* Pointer to a callback function to notify the client of call
        ** events */

    cm_processor_e_type           proc_id,
        /* The processor on which the client is executing */

    cm_pwr_collapse_notify_e_type pwr_collapse_type,
        /* The power collapse type, None, Full, default, custom */

    cm_client_event_reg_e_type    event_reg_type,
        /* Is this a registration or de-reg */

    cm_pd_event_e_type            from_pd_event,
        /* register from this event (including) */

    cm_pd_event_e_type            to_pd_event
          /* to this event (including) */
)
/*lint -esym(715,client)*/
/*lint -esym(715,proc_id)*/
/*lint -esym(715,pwr_collapse_type)*/
/*lint -esym(715,event_reg_type)*/
/*lint -esym(715,from_pd_event)*/
/*lint -esym(715,to_pd_event)*/
/*lint -esym(715,pd_event_func)*/

{
  #if defined(FEATURE_GPSONE) && defined(FEATURE_CM_APPS_POWER_COLLAPSE)
#error code not present
#else
  return CM_CLIENT_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE;
  #endif

} /* cm_mm_client_pd_pwr_collapse_dereg */
/*lint +esym(715,client)*/
/*lint +esym(715,proc_id)*/
/*lint +esym(715,pwr_collapse_type)*/
/*lint +esym(715,event_reg_type)*/
/*lint +esym(715,from_pd_event)*/
/*lint +esym(715,to_pd_event)*/
/*lint +esym(715,pd_event_func)*/


/*===========================================================================

FUNCTION cm_client_dbm_event_reg

DESCRIPTION
  This function is to be used by CM clients interested in registering for
  DBM call manager events.

DEPENDENCIES
  cm_client_init must have been called previously.

RETURN VALUE
  results of the registration.

SIDE EFFECTS
  None

===========================================================================*/

cm_client_status_e_type cm_client_dbm_event_reg(

    cm_client_id_type               client_id,
        /* Requesting client */

    cm_dbm_event_f_type             *dbm_event_func,
        /* Pointer to a callback function to notify the client of dbm
        ** events */

    cm_client_event_reg_e_type      event_reg_type,
        /* Is this a registration or de-reg */

    cm_dbm_event_e_type             from_dbm_event,
        /* register from this event (including) */

    cm_dbm_event_e_type             to_dbm_event,
        /* to this event (including) */


    cm_dbm_cmd_err_f_type           *dbm_cmd_err_func
      /* pointer to a callback function to notify the client of dbm
      ** command errors of OTHER clients */

)
/*lint -esym(715,client_id) */
/*lint -esym(715,dbm_event_func) */
/*lint -esym(715,event_reg_type) */
/*lint -esym(715,from_dbm_event) */
/*lint -esym(715,to_dbm_event) */
/*lint -esym(715,dbm_cmd_err_func) */
{

  return cm_client_send_cmd(client_id, event_reg_type, (int)from_dbm_event,
                           (int)to_dbm_event, (int)CM_DBM_EVENT_NONE,
                           (int)CM_DBM_EVENT_MAX, dbm_event_func,
                           dbm_cmd_err_func, CM_CLIENT_CMD_DBM_EVENT_REG);
 
}
/*lint +esym(715,client_id) */
/*lint +esym(715,dbm_event_func) */
/*lint +esym(715,event_reg_type) */
/*lint +esym(715,from_dbm_event) */
/*lint +esym(715,to_dbm_event) */
/*lint +esym(715,dbm_cmd_err_func) */


/*===========================================================================

FUNCTION cm_client_dbm_class_reg

DESCRIPTION
  This function is to be used by CM clients interested in registering for
  DBM classes like PD, SDB etc..

DEPENDENCIES
  cm_client_init must have been called previously.

RETURN VALUE
  results of the registration.

SIDE EFFECTS
  None

===========================================================================*/

cm_client_status_e_type cm_client_dbm_class_reg(

    cm_client_id_type               client_id,
        /* Requesting client */

    cm_client_event_reg_e_type      event_reg_type,
        /* Is this a registration or de-reg */

    cm_dbm_class_e_type             from_dbm_class,
        /* register from this class (including) */

    cm_dbm_class_e_type             to_dbm_class
        /* to this class (including) */

)
/*lint -esym(715,client_id) */
/*lint -esym(715,event_reg_type) */
/*lint -esym(715,from_dbm_class) */
/*lint -esym(715,to_dbm_class) */
{
  cm_client_status_e_type return_status;
  cm_client_cmd_s_type  *client_cmd_ptr;
  cm_client_gen_reg_data_s_type *reg_data = NULL;
  cm_client_s_type *client_ptr = NULL;

  return_status = cm_client_validate_reg(event_reg_type,
                                      (int)from_dbm_class, (int)to_dbm_class,
                                      (int)CM_DBM_CLASS_NONE,
                                      (int)CM_DBM_CLASS_MAX);
  if(return_status != CM_CLIENT_OK)
  {
    return return_status;
  }

  client_ptr = cmclient_get_client_ptr(client_id);
  if(client_ptr == NULL)
  {
    return CM_CLIENT_ERR_CLIENT_ID;
  }
  
  client_cmd_ptr = cm_cmd_alloc_client_init();

  client_cmd_ptr->client_id = client_id;
  client_cmd_ptr->client_ptr = client_ptr;
  client_cmd_ptr->cmd = CM_CLIENT_CMD_DBM_CLASS_REG;

  reg_data = &client_cmd_ptr->info;

  reg_data->event_reg_type = event_reg_type;
  reg_data->from_event = (int)from_dbm_class;
  reg_data->to_event = (int)to_dbm_class;

  cm_cmd_queue( (cm_cmd_type*)client_cmd_ptr );
   
  return return_status;

}
/*lint +esym(715,client_id) */
/*lint +esym(715,event_reg_type) */
/*lint +esym(715,from_dbm_class) */
/*lint +esym(715,to_dbm_class) */


/*===========================================================================

FUNCTION cm_mm_client_dbm_pwr_collapse_reg

DESCRIPTION
  DESCRIPTION
  This function allows a client to register/de-register the events it should
  be notified during power collapse. If the client wants to be notified of
  all the events that it is registered for in the normal (power restored
  state), it should use CM_PWR_COLLAPSE_NOTIFY_NORMAL as the
  pwr_collapse_type. If the client wants CM to decide on "important events"
  to wake up the processor from the power collapse state, it should use
  CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_DEFAULT. If the client wants to override
  CM's default, it should use CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_CUSTOM.

  The from_dbm_event and to_dbm_event, and event_reg_type are used only
  if the pwr_collapse_type is CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_CUSTOM. If
  event_reg_type is CM_CLIENT_EVENT_REG, the event will be notified while in
  power collapse. If the event_reg_type is CM_CLIENT_EVENT_DEREG, the event
  will not be reported while in power collapse.

  This API has a cumulative effect for event registration.
  For example, in order to regieter/de-register for two different ranges
  of dbm events, a client needs to call on this API twice- once for each
  range of dbm events. In case of conflicting pwr_collapse_type, the last
  one will be honored

DEPENDENCIES
  Client must have already been initialized with cm_client_init().


RETURN VALUE
  request status

SIDE EFFECTS
  none

===========================================================================*/
cm_client_status_e_type cm_mm_client_dbm_pwr_collapse_reg(

    cm_client_id_type             client,
        /* client id */

    cm_dbm_event_f_type          *dbm_event_func,
        /* Pointer to a callback function to notify the client of call
        ** events */

    cm_processor_e_type           proc_id,
        /* The processor on which the client is executing */

    cm_pwr_collapse_notify_e_type pwr_collapse_type,
        /* The power collapse type, None, Full, default, custom */

    cm_client_event_reg_e_type    event_reg_type,
        /* Is this a registration or de-reg */

    cm_dbm_event_e_type           from_dbm_event,
        /* register from this event (including) */

    cm_dbm_event_e_type           to_dbm_event
          /* to this event (including) */
)
/*lint -esym(715,client)*/
/*lint -esym(715,proc_id)*/
/*lint -esym(715,pwr_collapse_type)*/
/*lint -esym(715,event_reg_type)*/
/*lint -esym(715,from_dbm_event)*/
/*lint -esym(715,to_dbm_event)*/
/*lint -esym(715,dbm_event_func)*/

{
  #if defined(FEATURE_CM_APPS_POWER_COLLAPSE)
#error code not present
#else
  return CM_CLIENT_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE;
  #endif

} /* cm_mm_client_dbm_pwr_collapse_reg */
/*lint +esym(715,client)*/
/*lint +esym(715,proc_id)*/
/*lint +esym(715,pwr_collapse_type)*/
/*lint +esym(715,event_reg_type)*/
/*lint +esym(715,from_dbm_event)*/
/*lint +esym(715,to_dbm_event)*/
/*lint +esym(715,dbm_event_func)*/


/*===========================================================================

FUNCTION cm_client_sups_reg

DESCRIPTION
  This function is to be used by CM clients interested in registering for
  SUPS call manager events.

DEPENDENCIES
  cm_client_init must have been called previously.

RETURN VALUE
  results of the registration.

SIDE EFFECTS
  None

===========================================================================*/

cm_client_status_e_type cm_client_sups_reg(

    cm_client_id_type             client_id,
        /* Requesting client */

    cm_sups_event_f_type         *sups_event_func,
        /* Pointer to a callback function to notify the client of sups
        ** events */

    cm_client_event_reg_e_type    event_reg_type,
        /* Is this a registration or de-reg */

    cm_sups_event_e_type          from_event,
        /* register from this event (including) */

    cm_sups_event_e_type          to_event,
        /* to this event (including) */


    cm_sups_cmd_err_f_type       *sups_cmd_err_func
      /* pointer to a callback function to notify the client of pd
      ** command errors of OTHER clients */

)
/*lint -esym(715,client_id) */
/*lint -esym(715,sups_event_func) */
/*lint -esym(715,event_reg_type) */
/*lint -esym(715,from_event) */
/*lint -esym(715,to_event) */
/*lint -esym(715,sups_cmd_err_func) */
{
  #ifdef CM_GW_SUPPORTED

  return cm_client_send_cmd(client_id, event_reg_type, (int)from_event,
                           (int)to_event, (int)CM_SUPS_EVENT_NONE,
                           (int)CM_SUPS_EVENT_MAX, sups_event_func,
                           sups_cmd_err_func, CM_CLIENT_CMD_SUPS_REG);

  #else /* ( defined (FEATURE_WCDMA) || defined (FEATURE_GSM) ) */
  return CM_CLIENT_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE;
  #endif /* else of ( defined (FEATURE_WCDMA) || defined (FEATURE_GSM) ) */
}

/*lint +esym(715,client_id) */
/*lint +esym(715,sups_event_func) */
/*lint +esym(715,event_reg_type) */
/*lint +esym(715,from_event) */
/*lint +esym(715,to_event) */
/*lint +esym(715,sups_cmd_err_func) */

/*===========================================================================

FUNCTION cm_client_ac_reg

DESCRIPTION
  This function is to be used by CM clients interested in registering for
  access control events.

DEPENDENCIES
  cm_client_init must have been called previously.

RETURN VALUE
  results of the registration.

SIDE EFFECTS
  None

===========================================================================*/

cm_client_status_e_type cm_client_ac_reg(

    cm_client_id_type client_id,
        /* Requesting client */

    cm_ac_event_f_type *event_func,
        /* Pointer to a callback function to notify the client of AC
        ** events */

    cm_client_event_reg_e_type event_reg_type,
        /* Is this a registration or de-reg */

    cm_ac_event_e_type from_event,
        /* register from this event (including) */

    cm_ac_event_e_type to_event,
        /* to this event (including) */


    cm_ac_cmd_err_f_type *cmd_err_func
      /* pointer to a callback function to notify the client cmd errors */

)
{
  return cm_client_send_cmd(client_id, event_reg_type, (int)from_event,
                           (int)to_event, (int)CM_AC_EVENT_NONE,
                           (int)CM_AC_EVENT_MAX, event_func,
                           cmd_err_func, CM_CLIENT_CMD_AC_REG);
}

/*===========================================================================

FUNCTION cm_mm_client_sups_pwr_collapse_reg

DESCRIPTION
  DESCRIPTION
  This function allows a client to register/de-register the events it should
  be notified during power collapse. If the client wants to be notified of
  all the events that it is registered for in the normal (power restored
  state), it should use CM_PWR_COLLAPSE_NOTIFY_NORMAL as the
  pwr_collapse_type. If the client wants CM to decide on "important events"
  to wake up the processor from the power collapse state, it should use
  CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_DEFAULT. If the client wants to override
  CM's default, it should use CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_CUSTOM.

  The from_sups_event and to_sups_event, and event_reg_type are used only
  if the pwr_collapse_type is CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_CUSTOM. If
  event_reg_type is CM_CLIENT_EVENT_REG, the event will be notified while in
  power collapse. If the event_reg_type is CM_CLIENT_EVENT_DEREG, the event
  will not be reported while in power collapse.

  This API has a cumulative effect for event registration.
  For example, in order to regieter/de-register for two different ranges
  of sups events, a client needs to call on this API twice- once for each
  range of sups events. In case of conflicting pwr_collapse_type, the last
  one will be honored

DEPENDENCIES
  Client must have already been initialized with cm_client_init().


RETURN VALUE
  request status

SIDE EFFECTS
  none

===========================================================================*/
cm_client_status_e_type cm_mm_client_sups_pwr_collapse_reg(

    cm_client_id_type             client,
        /* client id */

    cm_sups_event_f_type         *sups_event_func,
        /* Pointer to a callback function to notify the client of call
        ** events */

    cm_processor_e_type           proc_id,
        /* The processor on which the client is executing */

    cm_pwr_collapse_notify_e_type pwr_collapse_type,
        /* The power collapse type, None, Full, default, custom */

    cm_client_event_reg_e_type    event_reg_type,
        /* Is this a registration or de-reg */

    cm_sups_event_e_type          from_sups_event,
        /* register from this event (including) */

    cm_sups_event_e_type          to_sups_event
          /* to this event (including) */
)
/*lint -esym(715,client)*/
/*lint -esym(715,proc_id)*/
/*lint -esym(715,pwr_collapse_type)*/
/*lint -esym(715,event_reg_type)*/
/*lint -esym(715,from_sups_event)*/
/*lint -esym(715,to_sups_event)*/
/*lint -esym(715,sups_event_func)*/

{
  #if (defined CM_GW_SUPPORTED && defined FEATURE_CM_APPS_POWER_COLLAPSE)
#error code not present
#else
  return CM_CLIENT_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE;
  #endif

} /* cm_mm_client_sups_pwr_collapse_reg */
/*lint +esym(715,client)*/
/*lint +esym(715,proc_id)*/
/*lint +esym(715,pwr_collapse_type)*/
/*lint +esym(715,event_reg_type)*/
/*lint +esym(715,from_sups_event)*/
/*lint +esym(715,to_sups_event)*/
/*lint +esym(715,sups_event_func)*/

/*===========================================================================

@FUNCTION cm_client_stats_reg

@DESCRIPTION
  This function allows a client to register the following Statistics-related
  callbacks and events with CM:

   1. Callback function to receive registered stats events
   2. Register and de-register stats events

   If a client registers a callback function to receive stats event
   notification, it also needs to register one or more stats events it is
   interested in receiving. This API has a cumulative effect for event
   registration.

@DEPENDENCIES
  Client must have already been initialized with cm_client_init().

@RETURNS
  request status

@SIDE_EFFECT
  none

@x
===========================================================================*/
cm_client_status_e_type cm_client_stats_reg(

    cm_client_id_type             client_id,
        /* client id */

    cm_stats_event_f_type         *stats_event_func,
        /* Pointer to a callback function to notify the client of statistics
        ** events */

    cm_client_event_reg_e_type    event_reg_type,

    cm_stats_event_e_type         from_stats_event,
        /* register from this event (including) */

    cm_stats_event_e_type         to_stats_event,
          /* to this event (including) */

     cm_stats_cmd_err_f_type      *stats_cmd_err_func
       /**< pointer to a callback function to notify the client of stats
       ** command errors of OTHER clients */
)
/*lint -esym(715,client_id)*/
/*lint -esym(715,stats_event_func)*/
/*lint -esym(715,event_reg_type)*/
/*lint -esym(715,from_stats_event)*/
/*lint -esym(715,to_stats_event)*/
/*lint -esym(715,stats_cmd_err_func)*/
/*lint -esym(765, cm_client_stats_reg)
** Can't be static, unit test uses it
*/
{
  cm_client_status_e_type result = CM_CLIENT_OK;

  result = cm_client_send_cmd(client_id, event_reg_type, (int)from_stats_event,
                            (int)to_stats_event, (int)CM_STATS_EVENT_NONE,
                            (int)CM_STATS_EVENT_MAX, stats_event_func,
                            stats_cmd_err_func, CM_CLIENT_CMD_STATS_REG);

  return result;

}
/*lint +esym(715,client_id)*/
/*lint +esym(715,stats_event_func)*/
/*lint +esym(715,event_reg_type)*/
/*lint +esym(715,from_stats_event)*/
/*lint +esym(715,to_stats_event)*/
/*lint +esym(715,stats_cmd_err_func)*/


#ifdef CM_DEBUG
#error code not present
#endif /* CM_DEBUG */


/*===========================================================================
FUNCTION cm_client_mbms_reg

DESCRIPTION


DEPENDENCIES
  Client must have already been initialized with cm_client_init().

RETURN VALUE
  request status

SIDE EFFECTS
  none

===========================================================================*/

cm_client_status_e_type cm_client_mbms_reg(

    cm_client_id_type              client_id,
        /* client id
        */

    cm_mbms_event_f_type         *mbms_event_func,
        /* Pointer to a callback function to notify the client
        ** of MBMS events
        */

    cm_client_event_reg_e_type    event_reg_type,
        /* Registering or deregistering?
        */

    cm_mbms_event_e_type          from_mbms_event,
        /* register from this event (including)
        */

    cm_mbms_event_e_type          to_mbms_event,
          /* to this event (including)
          */

    cm_mbms_cmd_err_f_type        *mbms_cmd_err_func
      /* pointer to a callback function to notify the client of MBMS
      ** command errors of OTHER clients
      */
)
/*lint -esym(715,client_id)*/
/*lint -esym(715,event_reg_type)*/
/*lint -esym(715,from_mbms_event)*/
/*lint -esym(715,to_mbms_event)*/
/*lint -esym(715,mbms_event_func)*/
/*lint -esym(715,mbms_cmd_err_func)*/
{
  #ifdef FEATURE_MBMS
#error code not present
#else
  return CM_CLIENT_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE;
  #endif /* #if defined (FEATURE_MBMS) */

}



/*lint +esym(715,client_id)*/
/*lint +esym(715,event_reg_type)*/
/*lint +esym(715,from_mbms_event)*/
/*lint +esym(715,to_mbms_event)*/
/*lint +esym(715,mbms_event_func)*/
/*lint +esym(715,mbms_cmd_err_func)*/

/*===========================================================================
FUNCTION cm_client_bcmcs_reg

DESCRIPTION


DEPENDENCIES
  Client must have already been initialized with cm_client_init().

RETURN VALUE
  request status

SIDE EFFECTS
  none

===========================================================================*/
cm_client_status_e_type cm_client_bcmcs_reg(

    cm_client_id_type           client_id,
        /* client id */

    cm_bcmcs_event_f_type          *bcmcs_event_func,
        /* Pointer to a callback function to notify the client of phone
        ** events */

    cm_client_event_reg_e_type  event_reg_type,

    cm_bcmcs_event_e_type          from_bcmcs_event,
        /* register from this event (including) */

    cm_bcmcs_event_e_type          to_bcmcs_event,
          /* to this event (including) */

    cm_bcmcs_cmd_err_f_type        *bcmcs_cmd_err_func
      /* pointer to a callback function to notify the client of phone
      ** command errors of OTHER clients */
)
/*lint -esym(715,client_id)*/
/*lint -esym(715,event_reg_type)*/
/*lint -esym(715,from_bcmcs_event)*/
/*lint -esym(715,to_bcmcs_event)*/
/*lint -esym(715,bcmcs_event_func)*/
/*lint -esym(715,bcmcs_cmd_err_func)*/
{
  #if ((defined (FEATURE_BCMCS)) || (defined (FEATURE_MBMS)))

  return cm_client_send_cmd(client_id, event_reg_type, (int)from_bcmcs_event,
                            (int)to_bcmcs_event, (int)CM_BCMCS_EVENT_NONE,
                            (int)CM_BCMCS_EVENT_MAX, bcmcs_event_func,
                            bcmcs_cmd_err_func, CM_CLIENT_CMD_BCMCS_REG);

  #else
  return CM_CLIENT_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE;
  #endif /* FEATURE_BCMCS || MBMS */
}
/*lint +esym(715,client_id)*/
/*lint +esym(715,event_reg_type)*/
/*lint +esym(715,from_bcmcs_event)*/
/*lint +esym(715,to_bcmcs_event)*/
/*lint +esym(715,bcmcs_event_func)*/
/*lint +esym(715,bcmcs_cmd_err_func)*/


/*===========================================================================

FUNCTION cm_mm_client_bcmcs_pwr_collapse_reg

DESCRIPTION
  DESCRIPTION
  This function allows a client to register/de-register the events it should
  be notified during power collapse. If the client wants to be notified of
  all the events that it is registered for in the normal (power restored
  state), it should use CM_PWR_COLLAPSE_NOTIFY_NORMAL as the
  pwr_collapse_type. If the client wants CM to decide on "important events"
  to wake up the processor from the power collapse state, it should use
  CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_DEFAULT. If the client wants to override
  CM's default, it should use CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_CUSTOM.

  The from_bcmcs_event and to_bcmcs_event, and event_reg_type are used only
  if the pwr_collapse_type is CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_CUSTOM. If
  event_reg_type is CM_CLIENT_EVENT_REG, the event will be notified while in
  power collapse. If the event_reg_type is CM_CLIENT_EVENT_DEREG, the event
  will not be reported while in power collapse.

  This API has a cumulative effect for event registration.
  For example, in order to regieter/de-register for two different ranges
  of bcmcs events, a client needs to call on this API twice- once for each
  range of bcmcs events. In case of conflicting pwr_collapse_type, the last
  one will be honored

DEPENDENCIES
  Client must have already been initialized with cm_client_init().


RETURN VALUE
  request status

SIDE EFFECTS
  none

===========================================================================*/
cm_client_status_e_type cm_mm_client_bcmcs_pwr_collapse_reg(

    cm_client_id_type             client,
        /* client id */

    cm_bcmcs_event_f_type        *bcmcs_event_func,
        /* Pointer to a callback function to notify the client of call
        ** events */

    cm_processor_e_type           proc_id,
        /* The processor on which the client is executing */

    cm_pwr_collapse_notify_e_type pwr_collapse_type,
        /* The power collapse type, None, Full, default, custom */

    cm_client_event_reg_e_type    event_reg_type,
        /* Is this a registration or de-reg */

    cm_bcmcs_event_e_type         from_bcmcs_event,
        /* register from this event (including) */

    cm_bcmcs_event_e_type         to_bcmcs_event
          /* to this event (including) */
)
/*lint -esym(715,client)*/
/*lint -esym(715,proc_id)*/
/*lint -esym(715,pwr_collapse_type)*/
/*lint -esym(715,event_reg_type)*/
/*lint -esym(715,from_bcmcs_event)*/
/*lint -esym(715,to_bcmcs_event)*/
/*lint -esym(715,bcmcs_event_func)*/

{
  #if defined(FEATURE_BCMCS) && defined(FEATURE_CM_APPS_POWER_COLLAPSE)
#error code not present
#else
  return CM_CLIENT_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE;
  #endif

} /* cm_mm_client_bcmcs_pwr_collapse_reg */
/*lint +esym(715,client)*/
/*lint +esym(715,proc_id)*/
/*lint +esym(715,pwr_collapse_type)*/
/*lint +esym(715,event_reg_type)*/
/*lint +esym(715,from_bcmcs_event)*/
/*lint +esym(715,to_bcmcs_event)*/
/*lint +esym(715,bcmcs_event_func)*/


/*===========================================================================
FUNCTION cm_client_nas_reg

DESCRIPTION

  This function allows a client to register the following NAS
  callbacks and events with CM:

   1. Callback function to receive registered NAS events
   2. Register for and de-register from receiving NAS events
   3. Callback function to receive other clients' NAS command errors.

   If a client registers a callback function to receive NAS event
   notification, it also needs to register one or more NAS events
   it is interested in receiving. This API has a cumulative effect for event
   registration. For example, in order to register for two different ranges
   of NAS events, a client needs to call on this API twice -
   once for each range of BCMCS events.

   If a client registers a callback function to receive other clients'
   NAS command errors, it will receive notifications on all possible
   NAS command errors caused by other clients. A client cannot
   register to receive only a subset of NAS command errors.
   The NULL function pointer can also be used for the callback function
   if a client is not interested in receiving that callback.
   For example, if a client is not interested in
   receiving other clients' NAS command errors, it can set
   nas_cmd_err_func = NULL. However, if the nas_event_func is set to NULL,
   all registered NAS events are also de-registered and the
   event_reg_type, from_nas_event, and to_nas_event parameters are ignored.

DEPENDENCIES
  Client must have already been initialized with cm_client_init().

RETURN VALUE
  Request status

SIDE EFFECTS
  None

===========================================================================*/
cm_client_status_e_type cm_client_nas_reg(

    cm_client_id_type              client_id,
        /* client id
        */

    cm_nas_event_f_type         *nas_event_func,
        /* Pointer to a callback function to notify the client
        ** of NAS events
        */

    cm_client_event_reg_e_type     event_reg_type,
        /* Registering or deregistering?
        */

    cm_nas_event_e_type          from_nas_event,
        /* register from this event (including)
        */

    cm_nas_event_e_type          to_nas_event,
          /* to this event (including)
          */

    cm_nas_cmd_err_f_type        *nas_cmd_err_func
      /* pointer to a callback function to notify the client of NAS
      ** command errors of OTHER clients
      */
)
/*lint -esym(715,client_id)*/
/*lint -esym(715,event_reg_type)*/
/*lint -esym(715,from_nas_event)*/
/*lint -esym(715,to_nas_event)*/
/*lint -esym(715,nas_event_func)*/
/*lint -esym(715,nas_cmd_err_func)*/
{

  #if (defined FEATURE_WMS_CM_NAS_INTERFACES && defined CM_GW_SUPPORTED)
#error code not present
#else
  return CM_CLIENT_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE;
  #endif    /* FEATURE_WMS_CM_NAS_INTERFACES && FEATURE_GSM || FEATURE_WCDMA */

}  /* cm_client_nas_reg */
/*lint +esym(715,client_id)*/
/*lint +esym(715,event_reg_type)*/
/*lint +esym(715,from_nas_event)*/
/*lint +esym(715,to_nas_event)*/
/*lint +esym(715,nas_event_func)*/
/*lint +esym(715,nas_cmd_err_func)*/


/*===========================================================================

FUNCTION cm_mm_client_nas_pwr_collapse_reg

DESCRIPTION
  DESCRIPTION
  This function allows a client to register/de-register the events it should
  be notified during power collapse. If the client wants to be notified of
  all the events that it is registered for in the normal (power restored
  state), it should use CM_PWR_COLLAPSE_NOTIFY_NORMAL as the
  pwr_collapse_type. If the client wants CM to decide on "important events"
  to wake up the processor from the power collapse state, it should use
  CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_DEFAULT. If the client wants to override
  CM's default, it should use CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_CUSTOM.

  The from_nas_event and to_nas_event, and event_reg_type are used only
  if the pwr_collapse_type is CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_CUSTOM. If
  event_reg_type is CM_CLIENT_EVENT_REG, the event will be notified while in
  power collapse. If the event_reg_type is CM_CLIENT_EVENT_DEREG, the event
  will not be reported while in power collapse.

  This API has a cumulative effect for event registration.
  For example, in order to regieter/de-register for two different ranges
  of nas events, a client needs to call on this API twice- once for each
  range of nas events. In case of conflicting pwr_collapse_type, the last
  one will be honored

DEPENDENCIES
  Client must have already been initialized with cm_client_init().


RETURN VALUE
  request status

SIDE EFFECTS
  none

===========================================================================*/
cm_client_status_e_type cm_mm_client_nas_pwr_collapse_reg(

    cm_client_id_type             client,
        /* client id */

    cm_nas_event_f_type          *nas_event_func,
        /* Pointer to a callback function to notify the client of call
        ** events */

    cm_processor_e_type           proc_id,
        /* The processor on which the client is executing */

    cm_pwr_collapse_notify_e_type pwr_collapse_type,
        /* The power collapse type, None, Full, default, custom */

    cm_client_event_reg_e_type    event_reg_type,
        /* Is this a registration or de-reg */

    cm_nas_event_e_type           from_nas_event,
        /* register from this event (including) */

    cm_nas_event_e_type           to_nas_event
          /* to this event (including) */
)
/*lint -esym(715,client)*/
/*lint -esym(715,proc_id)*/
/*lint -esym(715,pwr_collapse_type)*/
/*lint -esym(715,event_reg_type)*/
/*lint -esym(715,from_nas_event)*/
/*lint -esym(715,to_nas_event)*/
/*lint -esym(715,nas_event_func)*/

{
  #if ( defined (FEATURE_WMS_CM_NAS_INTERFACES) \
      && ( defined (FEATURE_GSM) || defined (FEATURE_WCDMA) )) \
      && defined(FEATURE_CM_APPS_POWER_COLLAPSE)
#error code not present
#else
  return CM_CLIENT_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE;
  #endif

} /* cm_mm_client_nas_pwr_collapse_reg */
/*lint +esym(715,client)*/
/*lint +esym(715,proc_id)*/
/*lint +esym(715,pwr_collapse_type)*/
/*lint +esym(715,event_reg_type)*/
/*lint +esym(715,from_nas_event)*/
/*lint +esym(715,to_nas_event)*/
/*lint +esym(715,nas_event_func)*/


/*===========================================================================

FUNCTION cmclient_get_client_id_for_client_type

DESCRIPTION
  Returns client id that corresponds to a client type

DEPENDENCIES
  None

RETURN VALUE
  client id

SIDE EFFECTS
  None

===========================================================================*/
cm_client_id_type cmclient_get_client_id_for_client_type
(

    cm_client_type_e_type client_type
      /* Client type */
)
{
    cm_client_id_type client_id;

    for ( client_id = 0; client_id < ARR_SIZE( cmclient_map_arr ); client_id++ )
    {
      if(cmclient_map_arr[client_id].client_type == client_type)
      {
        return client_id;
      }
    }
    
    /* This catches bogus client_type */
    CM_ASSERT(FALSE);
    return CM_CLIENT_ID_ANONYMOUS;
}

/*===========================================================================

FUNCTION cm_client_stats_event_ntfy

DESCRIPTION
  Notify a client of a specified Statistics event iff the client
  is registered for this event.

DEPENDENCIES
  Client must have already been initialized with cm_client_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cm_client_stats_event_ntfy (

  cm_stats_event_e_type        stats_event,
   /* pointer to a client struct */

  sys_modem_stats_info_s_type  *stats_info_ptr
  /* pointer to STATS information struct */

)
/*lint -esym(715, stats_event)*/
/*lint -esym(715, stats_info_ptr)*/
/*lint -esym(818, stats_info_ptr)*/
{
  /*lint -save -e550 Symbol 'client_id' not accessed */
  int client_id;
  cm_client_type_e_type client_type;
  cm_client_s_type          *client_ptr;


  CM_ASSERT( BETWEEN(stats_event, CM_STATS_EVENT_NONE, CM_STATS_EVENT_MAX) );
  CM_ASSERT( stats_info_ptr != NULL );

  /*lint -save -e571 */
  CMDBG_PRINT_CM_EVENT_FUNC( CM_CMD_TYPE_STATS,
    (word) stats_event, (const void *) stats_info_ptr );
  /*lint -restore */

  CM_MSG_HIGH_2("stats evt %d, stats_info_ptr asubs_id %d",
               stats_event, stats_info_ptr->asubs_id);

  /* Traverse client list and notify the
  ** individual client objects of the event.
  */
  for(client_id = 0; client_id < ARR_SIZE(cmclient_map_arr); client_id++)
  {
    rex_enter_crit_sect(cmclient_crit_sect_ptr());
    /* Serialize data access cmclient_map_arr[] only */
    client_ptr = cmclient_map_arr[client_id].client_ptr;
    client_type = cmclient_map_arr[client_id].client_type;
    rex_leave_crit_sect(cmclient_crit_sect_ptr());

    if( client_ptr != NULL && client_ptr->stats_event_func != NULL &&
          bit_arr_is_bit_on(&client_ptr->stats_bit_arr, (int)stats_event) )
    {
      client_ptr->stats_event_func(stats_event, stats_info_ptr);
    }
  }
}
/*lint +esym(715, stats_event)*/
/*lint +esym(715, stats_info_ptr)*/
/*lint +esym(818, stats_info_ptr)*/

/*=========================================================================

FUNCTION
  cmclient_crit_sect_ptr

DESCRIPTION

  Return pointer to CM client critical section.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif
rex_crit_sect_type* cmclient_crit_sect_ptr(void)
{
  static rex_crit_sect_type cmclient_crit_sect;
  /* The one and only CM critical section */

  return &cmclient_crit_sect;
}
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif

/*===========================================================================

FUNCTION
  cmclient_init_crit_sect

DESCRIPTION
  Initialize CM critical section.
  This function must be called before any other part of CM is initialized

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmclient_init_crit_sect(void)
{
  rex_init_crit_sect(cmclient_crit_sect_ptr());
}

/*===========================================================================

FUNCTION cmclient_client_cmd_proc

DESCRIPTION
  Process clients' phone commands.

DEPENDENCIES
  Client must have already been initialized with cm_client_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmclient_client_cmd_proc(

    cm_client_cmd_s_type  *client_cmd
        /* pointer to a CM command */
)
{
  cm_client_s_type *client_ptr;
  cm_client_gen_reg_data_s_type *reg_data;
  boolean reg_flag;

  CM_ASSERT( client_cmd != NULL );
  CM_ASSERT( client_cmd->cmd_type == CM_CMD_TYPE_CLIENT);

  client_ptr = client_cmd->client_ptr;
  reg_data = &client_cmd->info;
  reg_flag = reg_data->event_reg_type == CM_CLIENT_EVENT_REG ? TRUE : FALSE;
  
  switch(client_cmd->cmd)
  {
    
    case CM_CLIENT_CMD_ACT:
      {
        /* Notify client with latest phone info */
        
        cm_ph_info_s_type    *ph_info_ptr;
        ph_info_ptr = cmph_info_alloc();
        /* Copy latest pref in PH ptr into PH info to be sent to the clients */
        (void) cmph_info_get( ph_info_ptr, CM_SS_MAIN );

        CM_MSG_HIGH_3("ph evt %d ct %d cid id %d",
                      CM_PH_EVENT_INFO_AVAIL,
                      cmclient_get_client_type(ph_info_ptr->requesting_client_id),
                      ph_info_ptr->requesting_client_id);

        cm_client_ph_event_ntfy( client_ptr,
                                 CM_PH_EVENT_INFO_AVAIL,
                                 ph_info_ptr );
        cm_mem_free(ph_info_ptr);
      }
      break;
    
    case CM_CLIENT_CMD_RELEASE:
      {
        cmclient_free_client_buf(client_ptr);
      }
      break;
    
    case CM_CLIENT_CMD_CALL_REG:
      {
        bit_arr_set_bits( &client_ptr->call_bit_arr,
                          reg_flag,
                          reg_data->from_event,
                          reg_data->to_event);
        client_ptr->call_event_func = reg_data->event_func;
        client_ptr->call_cmd_err_func = reg_data->cmd_err_func;
      }
      break;
    
    case CM_CLIENT_CMD_SUBS_REG:
      {
        client_ptr->subs_event_func = reg_data->event_func;
      }
      break;

    case CM_CLIENT_CMD_PH_REG:
      {
        bit_arr_set_bits( &client_ptr->ph_bit_arr,
                          reg_flag,
                          reg_data->from_event,
                          reg_data->to_event);
        client_ptr->ph_event_func = reg_data->event_func;
        client_ptr->ph_cmd_err_func = reg_data->cmd_err_func;
      }
      break;
        
    case CM_CLIENT_CMD_SS_REG:
      {
        bit_arr_set_bits( &client_ptr->ss_bit_arr,
                          reg_flag,
                          reg_data->from_event,
                          reg_data->to_event);
        client_ptr->ss_event_func = reg_data->event_func;
        client_ptr->ss_cmd_err_func = reg_data->cmd_err_func;
      }
      break;
    
    case CM_CLIENT_CMD_SS_REG_MSIM:
      {
	    bit_arr_set_bits( &client_ptr->ss_bit_arr,
                          reg_flag,
                          reg_data->from_event,
                          reg_data->to_event);
        client_ptr->msim_ss_event_func = reg_data->event_func;
        client_ptr->ss_cmd_err_func = reg_data->cmd_err_func;
        client_ptr->asubs_mask = reg_data->asubs_mask;
      }
      break;

    case CM_CLIENT_CMD_SMS_REG:
      {
        bit_arr_set_bits( &client_ptr->sms_bit_arr,
                          reg_flag,
                          reg_data->from_event,
                          reg_data->to_event);
        client_ptr->sms_event_func = reg_data->event_func;
        client_ptr->sms_cmd_err_func = reg_data->cmd_err_func;
      }
      break;
    
    case CM_CLIENT_CMD_INBAND_REG:
      {
        bit_arr_set_bits( &client_ptr->inband_bit_arr,
                          reg_flag,
                          reg_data->from_event,
                          reg_data->to_event);
        client_ptr->inband_event_func = reg_data->event_func;
        client_ptr->inband_cmd_err_func = reg_data->cmd_err_func;
      }
      break;
    
    case CM_CLIENT_CMD_CALL_LIST_REG:
      {
        client_ptr->call_info_list_func = reg_data->event_func;
        client_ptr->call_info_list_cmd_err_func = reg_data->cmd_err_func;
      }
      break;
    
    case CM_CLIENT_CMD_DBM_EVENT_REG:
      {
        bit_arr_set_bits( &client_ptr->dbm_event_bit_arr,
                          reg_flag,
                          reg_data->from_event,
                          reg_data->to_event);
        client_ptr->dbm_event_func = reg_data->event_func;
        client_ptr->dbm_cmd_err_func = reg_data->cmd_err_func;
      }
      break;

    case CM_CLIENT_CMD_DBM_CLASS_REG:
      {
        bit_arr_set_bits( &client_ptr->dbm_class_bit_arr,
                          reg_flag,
                          reg_data->from_event,
                          reg_data->to_event);
      }
      break;

    #ifdef CM_GW_SUPPORTED
    case CM_CLIENT_CMD_SUPS_REG:
      {
        bit_arr_set_bits( &client_ptr->sups_bit_arr,
                          reg_flag,
                          reg_data->from_event,
                          reg_data->to_event);
        client_ptr->sups_event_func = reg_data->event_func;
        client_ptr->sups_cmd_err_func = reg_data->cmd_err_func;
      }
      break;

    #ifdef FEATURE_MBMS
    #error code not present
#endif /* FEATURE_MBMS */
    #endif /* CM_GW_SUPPORTED */
    
    case CM_CLIENT_CMD_STATS_REG:
      {
        bit_arr_set_bits( &client_ptr->stats_bit_arr,
                          reg_flag,
                          reg_data->from_event,
                          reg_data->to_event);
        client_ptr->stats_event_func = reg_data->event_func;
        client_ptr->stats_cmd_err_func = reg_data->cmd_err_func;
      }
      break;

    #if ((defined (FEATURE_BCMCS)) || (defined (FEATURE_MBMS)))
    case CM_CLIENT_CMD_BCMCS_REG:
      {
        bit_arr_set_bits( &client_ptr->bcmcs_bit_arr,
                          reg_flag,
                          reg_data->from_event,
                          reg_data->to_event);
        client_ptr->bcmcs_event_func = reg_data->event_func;
        client_ptr->bcmcs_cmd_err_func = reg_data->cmd_err_func;
      }
      break;
    #endif

    #if (defined FEATURE_WMS_CM_NAS_INTERFACES && defined CM_GW_SUPPORTED)
    #error code not present
#endif

    case CM_CLIENT_CMD_AC_REG:
    {
      bit_arr_set_bits( &client_ptr->ac_bit_arr,
                        reg_flag,
                        reg_data->from_event,
                        reg_data->to_event);
      client_ptr->ac_event_func = reg_data->event_func;
      client_ptr->ac_cmd_err_func = reg_data->cmd_err_func;
    }
   
    default:
      CM_MSG_HIGH_1("Client cmd %d is unknown", client_cmd->cmd);
      
  }
  
} /* cmclient_client_cmd_proc() */

/*===========================================================================

FUNCTION cm_cmd_client_init

DESCRIPTION
  Initializes cmd pointer for Client commands

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/

static void cm_cmd_client_init( cm_client_cmd_s_type *client_cmd_ptr )
{

  client_cmd_ptr->cmd_type = CM_CMD_TYPE_CLIENT;

}

  
/*===========================================================================

FUNCTION cm_cmd_alloc_client_init

DESCRIPTION
  Allocates memory for client command to be queued to CM task.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/

cm_client_cmd_s_type *cm_cmd_alloc_client_init( void )
{
  cm_client_cmd_s_type          *client_cmd_ptr       = 
      (cm_client_cmd_s_type*)cm_cmd_alloc(sizeof(cm_client_cmd_s_type));

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If command allocation is successful, initialize
  ** the client command fields.
  */
  cm_cmd_client_init(client_cmd_ptr);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return client_cmd_ptr;

}

