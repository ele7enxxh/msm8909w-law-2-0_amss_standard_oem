/*===========================================================================

                          P S _ S T A T _ A R P . C

GENERAL DESCRIPTION
  This program specifically deals with statistics at the arp layer of
  data services stack

EXTERNALIZED FUNCTIONS
  ps_stat_get_arp
    This function supplies the required arp stats.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2006-2010 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/23/10   ss      Cleaning up of Globals for thread safety in Q6 Free 
                   Floating environment.
12/23/09   cp      Lint Fixes.
09/04/08   am      Klocwork fixes.
02/27/07   scb     Added changes to fix RCVT compilation warning
06/26/06   scb     Initial version

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"
#include "comdef.h"
#include "IxErrno.h"
#include "err.h"

#include "ps_stat_arp.h"
#include "ps_stat_common.h"
#include "ps_stat_logging.h"

#include "ps_arpi.h"
#include "ds_Utils_DebugMsg.h"

#include <stddef.h>
#include <string.h>


/*===========================================================================

                             INTERNAL DEFINITIONS

===========================================================================*/

/*===========================================================================
MACRO ARP_FILL_INSTANCE_AT_STRUCT(IDX, FIELD, SIZE

DESCRIPTION
  This macro fills the offset and size fields of the access table 
  struture for the field passed.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define ARP_FILL_INSTANCE_AT_STRUCT(IDX, FIELD, SIZE)              \
{                                                                  \
  ps_stat_arp_i_table[(IDX)].offset = FPOS(arpi_lle_cb_type,FIELD);  \
  ps_stat_arp_i_table[(IDX)].len = sizeof(SIZE);                     \
}

/*---------------------------------------------------------------------------
  ARP stats access table
---------------------------------------------------------------------------*/
static ps_stat_access_table_s_type 
  ps_stat_arp_i_table[((uint32)PS_STAT_ARP_ALL) + 1];

/*===========================================================================

                             INTERNAL FORWARD DECLARATIONS

===========================================================================*/
static errno_enum_type ps_stat_get_arp_instance_all
(
  ps_stat_arp_enum_type    stat,
  arpi_lle_cb_type         *ctrl_blk_ptr,
  void                     *return_value,
  uint16                   ret_len
);

static void ps_stat_get_arp_inst_desc
(
  int32                         handle,
  void                        * log_pkt_ptr
);

static void ps_stat_get_arp_desc_log_pkt
(
  int32          handle
);

static void ps_stat_get_arp_stat_log_pkt
(
  int32          handle
);

/*===========================================================================
FUNCTION PS_STAT_GET_ARP_INSTANCE_ALL()

DESCRIPTION
  This function supplies all the ARP statistics for the instance handle
  supplied. This is needed because of the need to extract and format data
  available in the arp control block

DEPENDENCIES
  None

RETURN VALUE

  E_SUCCESS (0) for success and any other number for failure . The error
  codes are as defined in errno.h.

PARAMETERS
  stat            - IN - type of stat
  ctrl_blk_ptr    - IN - pointer that points to the ARP control block
  return_value    - IN - pointer for return value
                  - OUT- return value (statistics)
  ret_len         - length of memory allocated

SIDE EFFECTS
  None
===========================================================================*/
errno_enum_type ps_stat_get_arp_instance_all
(
  ps_stat_arp_enum_type    stat,
  arpi_lle_cb_type         *ctrl_blk_ptr,
  void                     *return_value,
  uint16                   ret_len
)
{
   ps_stat_arp_i_s_type *arp_i_stats;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    validate 'stat'; only PS_STAT_ARP_ALL should be passed to this function
  -------------------------------------------------------------------------*/
  if (stat != PS_STAT_ARP_ALL || NULL == ctrl_blk_ptr)
  {
    DATA_ERR_FATAL(" ps_stat_get_arp_i_all: Invalid stat or NULl cntrl_blk _ptr passed");
    return E_NOT_ALLOWED ;
  }

  /*-------------------------------------------------------------------------
    return_value validation : should not be null
  -------------------------------------------------------------------------*/

  if (return_value == NULL)
  {
    LOG_MSG_ERROR_0("return_value should not be NULL ");
    return E_NOT_ALLOWED ;
  }

  /*-------------------------------------------------------------------------
    check the sizeof the stat requested. If the size is zero, the stat is
    invalid in the current context. return appropriate error
  -------------------------------------------------------------------------*/

  if (ret_len < sizeof(ps_stat_arp_i_s_type))
  {
    LOG_MSG_ERROR_0(" Insufficient memory allocated for ptr return_value");
    return E_NOT_ALLOWED ;
  }

  arp_i_stats = (ps_stat_arp_i_s_type *) return_value ;

  arp_i_stats->ipv4_req_tx               = 
                          ctrl_blk_ptr->arp_i_stats.ipv4_req_tx;
  arp_i_stats->ipv4_drop_new_req_tx      = 
                          ctrl_blk_ptr->arp_i_stats.ipv4_drop_new_req_tx;
  arp_i_stats->ipv4_drop_ce_flush        = 
                          ctrl_blk_ptr->arp_i_stats.ipv4_drop_ce_flush;
  arp_i_stats->pkts_tx                   = 
                          ctrl_blk_ptr->arp_i_stats.pkts_tx;
  arp_i_stats->req_bcast_tx              = 
                          ctrl_blk_ptr->arp_i_stats.req_bcast_tx;
  arp_i_stats->req_bcast_prb_tx          = 
                          ctrl_blk_ptr->arp_i_stats.req_bcast_prb_tx;
  arp_i_stats->req_ucast_prb_tx          = 
                          ctrl_blk_ptr->arp_i_stats.req_ucast_prb_tx;
  arp_i_stats->req_bcast_rexmit          = 
                          ctrl_blk_ptr->arp_i_stats.req_bcast_rexmit;
  arp_i_stats->req_bcast_prb_rexmit      = 
                          ctrl_blk_ptr->arp_i_stats.req_bcast_prb_rexmit;
  arp_i_stats->req_ucast_prb_rexmit      = 
                          ctrl_blk_ptr->arp_i_stats.req_ucast_prb_rexmit;
  arp_i_stats->arp_req_fail              = 
                          ctrl_blk_ptr->arp_i_stats.arp_req_fail;
  arp_i_stats->arp_probe_timeout         = 
                          ctrl_blk_ptr->arp_i_stats.arp_probe_timeout;
  arp_i_stats->resp_tx                   = 
                          ctrl_blk_ptr->arp_i_stats.resp_tx;
  arp_i_stats->req_bcast_fail_tx         = 
                          ctrl_blk_ptr->arp_i_stats.req_bcast_fail_tx;
  arp_i_stats->req_bcast_prb_fail_tx     = 
                          ctrl_blk_ptr->arp_i_stats.req_bcast_prb_fail_tx;
  arp_i_stats->req_ucast_prb_fail_tx     = 
                          ctrl_blk_ptr->arp_i_stats. req_ucast_prb_fail_tx;
  arp_i_stats->req_bcast_fail_rexmit     = 
                          ctrl_blk_ptr->arp_i_stats.req_bcast_fail_rexmit;
  arp_i_stats->req_bcast_prb_fail_rexmit = 
                          ctrl_blk_ptr->arp_i_stats. req_bcast_prb_fail_rexmit;
  arp_i_stats->req_ucast_prb_fail_rexmit = 
                          ctrl_blk_ptr->arp_i_stats.req_ucast_prb_fail_rexmit;
  arp_i_stats->resp_fail_tx              =
                         ctrl_blk_ptr->arp_i_stats. resp_fail_tx;
  arp_i_stats->pkts_rx                   = 
                          ctrl_blk_ptr->arp_i_stats.pkts_rx;
  arp_i_stats->req_rx                    = 
                          ctrl_blk_ptr->arp_i_stats.req_rx;
  arp_i_stats->resp_rx                   = 
                          ctrl_blk_ptr->arp_i_stats.resp_rx;
  arp_i_stats->succ_resp_pending_rx      = 
                          ctrl_blk_ptr->arp_i_stats.succ_resp_pending_rx;
  arp_i_stats->unknown_pkts_rx           = 
                          ctrl_blk_ptr->arp_i_stats.unknown_pkts_rx;
  arp_i_stats->not_for_us_rx             = 
                          ctrl_blk_ptr->arp_i_stats. not_for_us_rx;
  arp_i_stats->no_req_pending_rx         = 
                          ctrl_blk_ptr->arp_i_stats.no_req_pending_rx;
  arp_i_stats->cache_hits                = 
                          ctrl_blk_ptr->arp_i_stats.cache_hits;
  arp_i_stats->cache_misses              = 
                          ctrl_blk_ptr->arp_i_stats.cache_misses;
  arp_i_stats->arp_req_cache_hits        = 
                          ctrl_blk_ptr->arp_i_stats.arp_req_cache_hits;
  arp_i_stats->arp_req_cache_misses      = 
                          ctrl_blk_ptr->arp_i_stats.arp_req_cache_misses;
  arp_i_stats->ce_evict_timeout          = 
                          ctrl_blk_ptr->arp_i_stats. ce_evict_timeout;
  arp_i_stats->ce_evict_new_req          = 
                          ctrl_blk_ptr->arp_i_stats.ce_evict_new_req;
  arp_i_stats->ce_evict_prb_succ         = 
                          ctrl_blk_ptr->arp_i_stats.ce_evict_prb_succ;
  arp_i_stats->ce_evict_ps_ent_clear     = 
                          ctrl_blk_ptr->arp_i_stats.ce_evict_ps_ent_clear;
  arp_i_stats->ce_update_arp_req         = 
                          ctrl_blk_ptr->arp_i_stats.ce_update_arp_req;
  arp_i_stats->ce_update_check_succ      = 
                          ctrl_blk_ptr->arp_i_stats.ce_update_check_succ;
  arp_i_stats->ce_update_ps_ent_set      = 
                          ctrl_blk_ptr->arp_i_stats.ce_update_ps_ent_set;       

  ctrl_blk_ptr->arp_i_stats.queried = PS_STAT_QUERIED;

  return E_SUCCESS ;
} /* ps_stat_get_arp_instance_all */


/*===========================================================================
FUNCTION PS_STAT_FILL_ARP_DESC()

DESCRIPTION
  This function fills the description of one ARP instance into a log packet.

PARAMETERS
  handle          : Specifies which instance to retrieve the description from
  *log_pkt_ptr    : Pointer to the log packet that the description will be
                    copied to

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_stat_get_arp_inst_desc
(
  int32                         handle,
  void                        * log_pkt_ptr
)
{
  arpi_lle_cb_type            * arp_ptr;
  ps_stat_inst_desc_arp_type  * arp_desc_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Sanity check the handle value
  -------------------------------------------------------------------------*/
  if ( ((int32)LAN_LLE_MAX < handle) || ((int32)LAN_LLE_MIN >= handle))
  {
    LOG_MSG_ERROR_1("Invalid handle to ARP instance.  Handle: %d", handle);
    return;
  }

  /*-------------------------------------------------------------------------
    Convert the handle into the instance control block
  -------------------------------------------------------------------------*/
  arp_ptr = &arp_array[handle];

  /*-------------------------------------------------------------------------
    ARP instance has not been initialized yet
  -------------------------------------------------------------------------*/

  if (!PS_IFACE_IS_VALID(arp_ptr->iface_ptr))
  {
    LOG_MSG_ERROR_1("Invalid IFACE Pointer in ARP instance. Handle: %d", 
                    handle);
    return;
  }

  arp_desc_ptr = (ps_stat_inst_desc_arp_type *)log_pkt_ptr ;

  arp_desc_ptr->handle       = (uint8)handle;
  arp_desc_ptr->iface_handle = (uint8)arp_ptr->iface_ptr->iface_private.iface_index;
  memset(arp_desc_ptr->reserved, 0, 1);

} /* ps_stat_get_arp_inst_desc */

/*===========================================================================
FUNCTION PS_STAT_GET_ARP_DESC_LOG_PKT()

DESCRIPTION
  This function fills the description of the specified instance handle into
  a log packet.  If the handle matches the handle for all descriptions, it
  copies all of the active instance descriptions into the log packet.  If the
  handle does not specify all instances but specifies a valid active instance,
  the description associated with that instance is copied into the log packet.
  If the handle is invalid, nothing is done.

PARAMETERS
  handle          : Specifies which instance to retrieve the description from

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_stat_get_arp_desc_log_pkt
(
  int32          handle
)
{
  int32          idx                = 0;
  int32          desc_size          = 0;
  uint8          inst_count         = 0;
  int32          log_pkt_avail_size = 0;
  uint8        * log_pkt_ptr        = NULL;
  uint8        * log_pkt_fill_ptr   = NULL;


  ps_stat_desc_all_inst_hdr_log_pkt_type     * desc_all_inst_pkt_ptr = NULL;
  ps_stat_desc_delta_inst_hdr_log_pkt_type   * delta_pkt_ptr         = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (PS_STAT_INST_DESC_ALL == handle)
  {
    /*-----------------------------------------------------------------------
      LAN_LLE_MIN is -1, so increment by 1 to start at LAN_LLE_RMNET1
    -----------------------------------------------------------------------*/
    for (idx = ((int32)LAN_LLE_MIN + 1); idx < (int32)LAN_LLE_MAX; ++idx)
    {
      /*---------------------------------------------------------------------
        Only show if the instance has started.  The value of arp_instance is
        only set in the entry once the instance has been started
      ---------------------------------------------------------------------*/
      if (idx == (int32)(arp_array[idx].arp_instance))
      {
        desc_size = sizeof(ps_stat_inst_desc_arp_type);

        /*-------------------------------------------------------------------
          Insufficient amount of space to fit this description in the log
          packet, so commit the existing log
        -------------------------------------------------------------------*/
        if (log_pkt_avail_size < desc_size)
        {
          if (NULL != log_pkt_ptr)
          {
            desc_all_inst_pkt_ptr = 
              (ps_stat_desc_all_inst_hdr_log_pkt_type *) log_pkt_ptr;

            desc_all_inst_pkt_ptr->count = inst_count;
            memset(desc_all_inst_pkt_ptr->reserved, 0, 3);

            ps_stat_logging_commit_log_pkt
            (
              log_pkt_ptr,
              (PS_STAT_MAX_LOG_PKT_SIZE - log_pkt_avail_size)
            );
          }

          inst_count         = 0;
          log_pkt_avail_size = PS_STAT_MAX_LOG_PKT_SIZE;
          log_pkt_ptr        = NULL;
          log_pkt_fill_ptr   = NULL;

          /*------------------------------------------------------------------
            Get the pointer to the log packet
          ------------------------------------------------------------------*/
          ps_stat_logging_get_log_pkt(log_pkt_avail_size,
                                      LOG_PS_STAT_DESC_ALL_ARP_INST_C,
                                      &log_pkt_ptr);

          /*-----------------------------------------------------------------
            If we fail to get a log packet, just return
          -----------------------------------------------------------------*/
          if (NULL == log_pkt_ptr)
          {
            LOG_MSG_ERROR_0("Unable to allocate log packet");
            return;
          }

          /*-----------------------------------------------------------------
            Leave space for the describe all instance header
          -----------------------------------------------------------------*/
          log_pkt_fill_ptr =
            (log_pkt_ptr + 
              (uint8) sizeof(ps_stat_desc_all_inst_hdr_log_pkt_type));

          log_pkt_avail_size -= sizeof(ps_stat_desc_all_inst_hdr_log_pkt_type);
        }

        ps_stat_get_arp_inst_desc(idx, log_pkt_fill_ptr);

        /*-------------------------------------------------------------------
          Increment the fill pointer by the size, and decrement the count
          by the same value
        -------------------------------------------------------------------*/
        log_pkt_fill_ptr += (uint8) desc_size;
        log_pkt_avail_size -= desc_size;
        inst_count++;
      }
    } /* for all arp instances*/

    /*-----------------------------------------------------------------------
      Dispatch the last packet
    -----------------------------------------------------------------------*/
    if (NULL != log_pkt_ptr)
    {
      desc_all_inst_pkt_ptr = 
        (ps_stat_desc_all_inst_hdr_log_pkt_type *) log_pkt_ptr;

      desc_all_inst_pkt_ptr->count = inst_count;
      memset(desc_all_inst_pkt_ptr->reserved, 0, 3);

      ps_stat_logging_commit_log_pkt
      (
        log_pkt_ptr,
        (PS_STAT_MAX_LOG_PKT_SIZE - log_pkt_avail_size)
      );
    }
  } /* if fill all arp instances */
  else
  {
    desc_size = sizeof(ps_stat_inst_desc_arp_type);

    /*-----------------------------------------------------------------------
      We need to allocate enough memory for the description and the delta
      header
    -----------------------------------------------------------------------*/
    log_pkt_avail_size = 
              (desc_size + sizeof(ps_stat_desc_delta_inst_hdr_log_pkt_type));

    ps_stat_logging_get_log_pkt(log_pkt_avail_size,
                                     LOG_PS_STAT_DESC_DELTA_INST_C,
                                     &log_pkt_ptr);

    /*-----------------------------------------------------------------------
      If we fail to get a log packet, just return
    -----------------------------------------------------------------------*/
    if (NULL == log_pkt_ptr)
    {
      LOG_MSG_ERROR_0("Unable to allocate log packet");
      return;
    }

    /*-----------------------------------------------------------------------
      Fill in the delta-log-packet specific fields
    -----------------------------------------------------------------------*/
    delta_pkt_ptr = (ps_stat_desc_delta_inst_hdr_log_pkt_type *) log_pkt_ptr;

    delta_pkt_ptr->type    = (uint8)PS_STAT_MODULE_INST_ARP;
    delta_pkt_ptr->deleted = FALSE;
    memset(delta_pkt_ptr->reserved, 0, 2);

    log_pkt_fill_ptr =
      (log_pkt_ptr + (uint8) sizeof(ps_stat_desc_delta_inst_hdr_log_pkt_type));

    ps_stat_get_arp_inst_desc(handle, log_pkt_fill_ptr);

    /*-----------------------------------------------------------------------
      Dispatch the last packet
    -----------------------------------------------------------------------*/
    ps_stat_logging_commit_log_pkt
    (
      log_pkt_ptr,
      (desc_size + sizeof(ps_stat_desc_delta_inst_hdr_log_pkt_type))
    );

  } /* else fill one arp instance */
} /* ps_stat_get_arp_desc_log_pkt */

/*===========================================================================
FUNCTION PS_STAT_GET_ARP_STAT_LOG_PKT()

DESCRIPTION
  This function fills the statistics of the specified instance handle into
  a log packet.  If the handle matches the handle for all statistics, it
  copies all of the active instance statistics into the log packet.  If the
  handle does not specify all instances but specifies a valid active instance,
  the statistics associated with that instance is copied into the log packet.
  If the handle is invalid, nothing is done.

PARAMETERS
  handle          : Specifies which instance to retrieve the stats from

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_stat_get_arp_stat_log_pkt
(
  int32          handle
)
{
  int32                             i                    = 0;
  uint8                           * log_pkt_ptr          = NULL;
  arpi_lle_cb_type                * arp_ptr              = NULL;
  ps_stat_inst_arp_log_pkt_type   * inst_arp_log_pkt_ptr = NULL;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
   LOG_MSG_INFO1_1 ( "ps_stat_get_arp_stat_log_pkt: handle %d", handle);
  /*-------------------------------------------------------------------------
    Sanity check inputs
  -------------------------------------------------------------------------*/
  if ((int32)LAN_LLE_MAX <= handle || (int32)LAN_LLE_MIN >= handle)
  {
    LOG_MSG_ERROR_0("Invalid handle");
    return;
  }

  if (PS_STAT_INST_DESC_ALL == handle)
  {
    /*-----------------------------------------------------------------------
      LAN_LLE_MIN is -1, so increment by 1 to start at LAN_LLE_RMNET1
    -----------------------------------------------------------------------*/
    for (i = ((int32)LAN_LLE_MIN + 1); i < (int32)LAN_LLE_MAX; ++i)
    {
      /*---------------------------------------------------------------------
        Only show if the instance has started
      ---------------------------------------------------------------------*/
      if (i == (int32)(arp_array[i].arp_instance))
      {
        arp_ptr = &arp_array[i];

        ps_stat_logging_get_log_pkt(sizeof(ps_stat_inst_arp_log_pkt_type),
                                    LOG_PS_STAT_ARP_INST_C,
                                    &log_pkt_ptr);

        if (NULL == log_pkt_ptr)
        {
          LOG_MSG_ERROR_0("Log packet allocated for ARP get stats failed");
          return;
        }

        inst_arp_log_pkt_ptr = (ps_stat_inst_arp_log_pkt_type *) log_pkt_ptr;

        inst_arp_log_pkt_ptr->handle  = i;

        (void) ps_stat_get_arp(PS_STAT_ARP_ALL,
                                (void *) arp_ptr,
                                &(inst_arp_log_pkt_ptr->inst_arp_stats),
                                sizeof(ps_stat_arp_i_s_type));

        ps_stat_logging_commit_log_pkt
        (
          log_pkt_ptr,
          sizeof(ps_stat_inst_arp_log_pkt_type)
        );
      }
    } /* for all arp instances*/
  } /* if fill all arp instances */
  else
  {
    /*---------------------------------------------------------------------
      Only show if the instance has started
    ---------------------------------------------------------------------*/
    if (handle == (int32)(arp_array[handle].arp_instance))
    {
      arp_ptr = &arp_array[handle];

      ps_stat_logging_get_log_pkt(sizeof(ps_stat_inst_arp_log_pkt_type),
                                  LOG_PS_STAT_ARP_INST_C,
                                  &log_pkt_ptr);

      if (NULL == log_pkt_ptr)
      {
        LOG_MSG_ERROR_0("Log packet allocated for ARP get stats failed");
        return;
      }

      inst_arp_log_pkt_ptr = (ps_stat_inst_arp_log_pkt_type *) log_pkt_ptr;

      inst_arp_log_pkt_ptr->handle  = handle;

      (void) ps_stat_get_arp(PS_STAT_ARP_ALL,
                              (void *) arp_ptr,
                              &(inst_arp_log_pkt_ptr->inst_arp_stats),
                              sizeof(ps_stat_arp_i_s_type));

      ps_stat_logging_commit_log_pkt
      (
        log_pkt_ptr,
        sizeof(ps_stat_inst_arp_log_pkt_type)
      );
    }
    else
    {
      LOG_MSG_ERROR_0("Attempting to get statistics on a stopped ARP instance.");
      return;
    }
  } /* else fill one arp instance */
} /* ps_stat_get_arp_stat_log_pkt */


/*===========================================================================

                             EXTERNAL FUNCTIONS

===========================================================================*/

/*===========================================================================
FUNCTION PS_STAT_RESET_ARP()

DESCRIPTION
  This function resets the instance statistics associated with the specified
  instance.

DEPENDENCIES
  None

RETURN VALUE
  E_SUCCESS (0) for success and any other number for failure . The error
  codes are as defined in errno.h.

PARAMETERS
  handle  : Handle to the instance

SIDE EFFECTS
  None
===========================================================================*/
void ps_stat_reset_arp
(
  int32              handle
)
{
  arpi_lle_cb_type  *arp_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (handle >= (int32)LAN_LLE_MAX || handle <= (int32)LAN_LLE_MIN )
  {
    LOG_MSG_ERROR_1("Invalid handle to ARP instance when resetting.  Handle: %d",
                    handle);
    return;
  }

  arp_ptr = &arp_array[handle];

  if (PS_STAT_QUERIED == arp_ptr->arp_i_stats.queried)
  {
    ps_stat_get_arp_stat_log_pkt(handle);
  }

  memset(&(arp_ptr->arp_i_stats), 0, sizeof(arp_ptr->arp_i_stats));

  return;
} /* ps_stat_reset_arp */


/*===========================================================================
FUNCTION PS_STAT_INIT_ARP()

DESCRIPTION

DEPENDENCIES
  None

PARAMETERS

RETURN VALUE

SIDE EFFECTS
  None
===========================================================================*/
void ps_stat_init_arp
(
  void
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  ARP_FILL_INSTANCE_AT_STRUCT(0, arp_i_stats.ipv4_req_tx, uint32);
  ARP_FILL_INSTANCE_AT_STRUCT(1, arp_i_stats.ipv4_drop_new_req_tx, 
                              uint32);
  ARP_FILL_INSTANCE_AT_STRUCT(2, arp_i_stats.ipv4_drop_ce_flush, 
                              uint32);
  ARP_FILL_INSTANCE_AT_STRUCT(3, arp_i_stats.pkts_tx, uint32);
  ARP_FILL_INSTANCE_AT_STRUCT(4, arp_i_stats.req_bcast_tx, uint32);
  ARP_FILL_INSTANCE_AT_STRUCT(5, arp_i_stats.req_bcast_prb_tx, uint32);
  ARP_FILL_INSTANCE_AT_STRUCT(6, arp_i_stats.req_ucast_prb_tx, uint32);
  ARP_FILL_INSTANCE_AT_STRUCT(7, arp_i_stats.req_bcast_rexmit, uint32);
  ARP_FILL_INSTANCE_AT_STRUCT(8, arp_i_stats.req_bcast_prb_rexmit, 
                              uint32);
  ARP_FILL_INSTANCE_AT_STRUCT(9, arp_i_stats.req_ucast_prb_rexmit, 
                              uint32);
  ARP_FILL_INSTANCE_AT_STRUCT(10, arp_i_stats.arp_req_fail, uint32);
  ARP_FILL_INSTANCE_AT_STRUCT(11, arp_i_stats.arp_probe_timeout, 
                              uint32);
  ARP_FILL_INSTANCE_AT_STRUCT(12, arp_i_stats.resp_tx, uint32);
  ARP_FILL_INSTANCE_AT_STRUCT(13, arp_i_stats.req_bcast_fail_tx, 
                              uint32);
  ARP_FILL_INSTANCE_AT_STRUCT(14, arp_i_stats.req_bcast_prb_fail_tx, 
                              uint32);
  ARP_FILL_INSTANCE_AT_STRUCT(15, arp_i_stats.req_ucast_prb_fail_tx, 
                              uint32);
  ARP_FILL_INSTANCE_AT_STRUCT(16, arp_i_stats.req_bcast_fail_rexmit,
                              uint32);
  ARP_FILL_INSTANCE_AT_STRUCT(17, arp_i_stats.req_bcast_prb_fail_rexmit,
                             uint32);
  ARP_FILL_INSTANCE_AT_STRUCT(18, arp_i_stats.req_ucast_prb_fail_rexmit,
                              uint32);
  ARP_FILL_INSTANCE_AT_STRUCT(19, arp_i_stats.resp_fail_tx, uint32);
  ARP_FILL_INSTANCE_AT_STRUCT(20, arp_i_stats.pkts_rx, uint32);
  ARP_FILL_INSTANCE_AT_STRUCT(21, arp_i_stats.req_rx, uint32);
  ARP_FILL_INSTANCE_AT_STRUCT(22, arp_i_stats.resp_rx, uint32);
  ARP_FILL_INSTANCE_AT_STRUCT(23, arp_i_stats.succ_resp_pending_rx, 
                              uint32);
  ARP_FILL_INSTANCE_AT_STRUCT(24, arp_i_stats.unknown_pkts_rx, uint32);
  ARP_FILL_INSTANCE_AT_STRUCT(25, arp_i_stats.not_for_us_rx, uint32);
  ARP_FILL_INSTANCE_AT_STRUCT(26, arp_i_stats.no_req_pending_rx, 
                              uint32);
  ARP_FILL_INSTANCE_AT_STRUCT(27, arp_i_stats.cache_hits, uint32);
  ARP_FILL_INSTANCE_AT_STRUCT(28, arp_i_stats.cache_misses, uint32);
  ARP_FILL_INSTANCE_AT_STRUCT(29, arp_i_stats.arp_req_cache_hits, 
                              uint32);
  ARP_FILL_INSTANCE_AT_STRUCT(30, arp_i_stats.arp_req_cache_misses, 
                              uint32);
  ARP_FILL_INSTANCE_AT_STRUCT(31, arp_i_stats.ce_evict_timeout, uint32);
  ARP_FILL_INSTANCE_AT_STRUCT(32, arp_i_stats.ce_evict_new_req, uint32);
  ARP_FILL_INSTANCE_AT_STRUCT(33, arp_i_stats.ce_evict_prb_succ, 
                              uint32);
  ARP_FILL_INSTANCE_AT_STRUCT(34, arp_i_stats.ce_evict_ps_ent_clear, 
                              uint32);
  ARP_FILL_INSTANCE_AT_STRUCT(35, arp_i_stats.ce_update_arp_req, 
                              uint32);
  ARP_FILL_INSTANCE_AT_STRUCT(36, arp_i_stats.ce_update_check_succ, 
                              uint32);
  ARP_FILL_INSTANCE_AT_STRUCT(37, arp_i_stats.ce_update_ps_ent_set, 
                              uint32);
  /*-------------------------------------------------------------------------
    for ARP_ALL offset starts from ipv4_req_tx and includes the entire
    structure.
  -------------------------------------------------------------------------*/
  ARP_FILL_INSTANCE_AT_STRUCT(38, arp_i_stats.ipv4_req_tx, 
                              ps_stat_arp_i_s_type);

  ps_stat_logging_inst_reg_fill_desc_cback(PS_STAT_MODULE_INST_ARP,
                                           ps_stat_get_arp_desc_log_pkt);

  ps_stat_logging_inst_reg_get_stat_cback(PS_STAT_MODULE_INST_ARP,
                                          ps_stat_get_arp_stat_log_pkt);

  ps_stat_logging_inst_reg_reset_stat_cback(PS_STAT_MODULE_INST_ARP,
                                            ps_stat_reset_arp);

} /* ps_stat_init_arp */


/*===========================================================================
FUNCTION PS_STAT_GET_ARP()

DESCRIPTION
  This function supplies the required arp layer statistics. The first
  argument passed to the function is an enum that denotes the metric of
  interest.The second argument will be  a pointer to the address of the
  arp control block. The third argument points to the memory location
  where the results will be copied.The result (statistic) will be copied
  to this memory address. The fourth argument passes the length of
  memory allocated for the return value and should be greater than or
  equal to the memory size of the statistic requested.

DEPENDENCIES
  None

RETURN VALUE
  E_SUCCESS (0) for success and any other number for failure . The error
  codes are as defined in errno.h.

PARAMETERS
  stat            - IN - type of stat
  instance_ptr    - IN - Handle to instance
  return_value    - IN - pointer for return value
                  - OUT- return value (statistics)
  ret_len         - length of memory allocated

SIDE EFFECTS
  None
===========================================================================*/
errno_enum_type ps_stat_get_arp
(
  ps_stat_arp_enum_type     stat,
  void                      *instance_ptr,
  void                      *return_value,
  uint16                    ret_len
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Check Instance pointer; Instance pointer should not be null
  -------------------------------------------------------------------------*/

  if (instance_ptr == NULL)
  {
    LOG_MSG_ERROR_0(" Instance pointer should NOT be NULL for ARP stats");
    return E_INVALID_ARG ;
  }

  if (stat == PS_STAT_ARP_ALL)
  {
    return ps_stat_get_arp_instance_all(stat,
                                        instance_ptr,
                                        return_value,
                                        ret_len
                                       ) ;
  }
  else
  {
    return ps_stat_get_common((uint32)stat,
                              ps_stat_arp_i_table,
                              (uint16)PS_STAT_ARP_ALL,
                              instance_ptr,
                              return_value,
                              ret_len
                             ) ;
  }

} /* ps_stat_get_arp */

/*===========================================================================
FUNCTION PS_STAT_GET_ARP_CONTROL_BLOCK()

DESCRIPTION
  This function supplies the address of the ARP control block corresponding
  the ARP device passed.

DEPENDENCIES
  None

RETURN VALUE
  E_SUCCESS (0) for success and any other number for failure . The error
  codes are as defined in errno.h.

PARAMETERS
  device        - IN  - ARP device
  arpcb_address - OUT - address of the arp control block corresponding
                        to the device.
  This is a void ** so that a void* value can be returned back that
  can be used to call ps_stat_get_arp

SIDE EFFECTS
  None
===========================================================================*/
errno_enum_type ps_stat_get_arp_control_block
(
  int     device ,
  void    **arpcb_address
)
{
  if (device >= (int32)LAN_LLE_MAX || device <= (int32)LAN_LLE_MIN )
  {
    LOG_MSG_ERROR_0(" Invalid device number passed");
    return E_INVALID_ARG ;
  }

  if (arpcb_address == NULL)
  {
    return E_INVALID_ARG;
  }

  /*-------------------------------------------------------------------------
    Copy the control block address
  -------------------------------------------------------------------------*/
  *arpcb_address = (void *) (&arp_array[device]) ;
  return E_SUCCESS ;
}

