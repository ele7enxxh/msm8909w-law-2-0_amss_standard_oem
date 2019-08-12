/*===========================================================================

                          P S _ S T A T _ L L C . C

GENERAL DESCRIPTION
  This program specifically deals with statistics at the 802.2 LLC layer
  of data services stack

EXTERNALIZED FUNCTIONS
  ps_stat_get_llc
    This function supplies the required llc stats.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2006-2013 Qualcomm Technologies Incorporated. 
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
08/19/09   pg      WAPI stats update.
02/27/07   scb     Added changes to fix RCVT compilation warning
08/18/06   scb     Fixed the range value check for device number.
06/27/06   scb     Initial version

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "customer.h"
#include "comdef.h"
#include "IxErrno.h"
#include "err.h"

#include "ps_stat_llc.h"
#include "ps_stat_common.h"
#include "ps_stat_logging.h"
#include "ds_Utils_DebugMsg.h"
#include "ps_lan_llci.h"
#include "ps_iface.h"

#include <stddef.h>
#include <string.h>


/*===========================================================================

                             INTERNAL DEFINITIONS

===========================================================================*/

/*===========================================================================
MACRO LLC_FILL_INSTANCE_AT_STRUCT(IDX, FIELD, SIZE)

DESCRIPTION
  This macro fills the offset and size fields of the access table structure 
  for the field passed.

DEPENDENCIES
  None

PARAMETERS
  IDX    -  The index of the structure table
  FIELD  -  The corresponding field of the statistics structure.
  SIZE   -  The size passed along to be a part of the Access Table

SIDE EFFECTS
  None
===========================================================================*/
#define LLC_FILL_INSTANCE_AT_STRUCT(IDX, FIELD, SIZE)                 \
{                                                                     \
  ps_stat_llc_i_table[(IDX)].offset = FPOS(lan_lle_cb_type, FIELD);     \
  ps_stat_llc_i_table[(IDX)].len = sizeof(SIZE);                        \
}

/*---------------------------------------------------------------------------
  LLC stats access table
---------------------------------------------------------------------------*/
static ps_stat_access_table_s_type 
  ps_stat_llc_i_table[((uint32)PS_STAT_LLC_ALL) + 1];

/*===========================================================================

                             INTERNAL FORWARD DECLARATIONS

===========================================================================*/
static errno_enum_type ps_stat_get_llc_instance_all
(
  ps_stat_llc_enum_type    stat,
  lan_lle_cb_type          *ctrl_blk_ptr,
  void                     *return_value,
  uint16                   ret_len
);

static void ps_stat_get_llc_inst_desc
(
  int32                         handle,
  void                        * log_pkt_ptr
);

static void ps_stat_get_llc_desc_log_pkt
(
  int32               handle
);

static void ps_stat_get_llc_stat_log_pkt
(
  int32               handle
);

/*===========================================================================
FUNCTION PS_STAT_GET_LLC_INSTANCE_ALL()

DESCRIPTION
  This function supplies all the LLC statistics for the instance handle
  supplied. This is needed because of the need to extract and format data
  available in the llc control block

DEPENDENCIES
  None

RETURN VALUE

  E_SUCCESS (0) for success and any other number for failure . The error
  codes are as defined in errno.h.

PARAMETERS
  stat            - IN - type of stat
  ctrl_blk_ptr    - IN - pointer that points to the LLE control block
  return_value    - IN - pointer for return value
                  - OUT- return value (statistics)
  ret_len         - length of memory allocated

SIDE EFFECTS
  None
===========================================================================*/
errno_enum_type ps_stat_get_llc_instance_all
(
  ps_stat_llc_enum_type    stat,
  lan_lle_cb_type          *ctrl_blk_ptr,
  void                     *return_value,
  uint16                   ret_len
)
{
   ps_stat_llc_i_s_type *llc_i_stats;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    validate 'stat'; only PS_STAT_LLC_ALL should be passed to this function
  -------------------------------------------------------------------------*/
  if (stat != PS_STAT_LLC_ALL || (ctrl_blk_ptr == NULL))
  {
    DATA_ERR_FATAL("ps_stat_get_llc_instance_all:Null iface_ptr or Invalid stat value");
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

  if (ret_len < sizeof(ps_stat_llc_i_s_type))
  {
    LOG_MSG_ERROR_0(" Insufficient memory allocated for ptr return_value");
    return E_NOT_ALLOWED ;
  }

  llc_i_stats = (ps_stat_llc_i_s_type *) return_value ;

  llc_i_stats->frms_tx                        = 
                    ctrl_blk_ptr->llc_i_stats.frms_tx;
  llc_i_stats->llc_mode0_tx                   = 
                    ctrl_blk_ptr->llc_i_stats.llc_mode0_tx;
  llc_i_stats->llc_mode1_tx                   =
                    ctrl_blk_ptr->llc_i_stats.llc_mode1_tx;
  llc_i_stats->frms_fail_tx                   = 
                    ctrl_blk_ptr->llc_i_stats.frms_fail_tx;
  llc_i_stats->unknown_proto_drop_tx          = 
                    ctrl_blk_ptr->llc_i_stats.unknown_proto_drop_tx;
  llc_i_stats->invalid_mode_drop_tx           = 
                    ctrl_blk_ptr->llc_i_stats.invalid_mode_drop_tx;
  llc_i_stats->frms_rx                        = 
                    ctrl_blk_ptr->llc_i_stats.frms_rx;
  llc_i_stats->llc_mode0_rx                   = 
                    ctrl_blk_ptr->llc_i_stats.llc_mode0_rx;
  llc_i_stats->llc_mode1_rx                   = 
                    ctrl_blk_ptr->llc_i_stats.llc_mode1_rx;
  llc_i_stats->llc_control_frms_rx            = 
                    ctrl_blk_ptr->llc_i_stats.llc_control_frms_rx;
  llc_i_stats->frms_drop_rx                   = 
                    ctrl_blk_ptr->llc_i_stats.frms_drop_rx;
  llc_i_stats->instance_stop_rx               = 
                    ctrl_blk_ptr->llc_i_stats.instance_stop_rx;
  llc_i_stats->instance_start_pending_rx      = 
                    ctrl_blk_ptr->llc_i_stats.instance_start_pending_rx;
  llc_i_stats->inv_dest_addr_rx               =
                    ctrl_blk_ptr->llc_i_stats.inv_dest_addr_rx;
  llc_i_stats->inv_src_addr_rx                =
                    ctrl_blk_ptr->llc_i_stats.inv_src_addr_rx;
  llc_i_stats->inv_mode_rx                    = 
                    ctrl_blk_ptr->llc_i_stats.inv_mode_rx;
  llc_i_stats->mode0_inv_ethlen_rx            = 
                    ctrl_blk_ptr->llc_i_stats.mode0_inv_ethlen_rx;
  llc_i_stats->mode1_inv_snaphdr_len_rx       = 
                    ctrl_blk_ptr->llc_i_stats.mode1_inv_snaphdr_len_rx;
  llc_i_stats->mode1_inv_snaphdr_rx           = 
                    ctrl_blk_ptr->llc_i_stats.mode1_inv_snaphdr_rx;
  llc_i_stats->mode1_inv_oui_rx               = 
                    ctrl_blk_ptr->llc_i_stats.mode1_inv_oui_rx;
  llc_i_stats->mode1_vlan_frms_rx             = 
                    ctrl_blk_ptr->llc_i_stats.mode1_vlan_frms_rx;
  llc_i_stats->mode1_inv_vlan_len_rx          = 
                    ctrl_blk_ptr->llc_i_stats.mode1_inv_vlan_len_rx;
  llc_i_stats->mode1_inv_vlan_frm_rx          = 
                    ctrl_blk_ptr->llc_i_stats. mode1_inv_vlan_frm_rx;
  llc_i_stats->mode1_inv_ethlen_rx            = 
                    ctrl_blk_ptr->llc_i_stats.mode1_inv_ethlen_rx;
  llc_i_stats->llc_802_1x_unsupported         = 
                    ctrl_blk_ptr->llc_i_stats.llc_802_1x_unsupported;
  llc_i_stats->llc_802_1x_preauth_unsupported = 
                    ctrl_blk_ptr->llc_i_stats.llc_802_1x_preauth_unsupported;
  llc_i_stats->llc_wapi_unsupported            =
                    ctrl_blk_ptr->llc_i_stats.llc_wapi_unsupported;

  ctrl_blk_ptr->llc_i_stats.queried = PS_STAT_QUERIED;

  return E_SUCCESS ;
} /* ps_stat_get_llc_instance_all */


/*===========================================================================
FUNCTION PS_STAT_GET_LLC_INST_DESC()

DESCRIPTION
  This function fills the description of one LLC instance into a log packet.

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
void ps_stat_get_llc_inst_desc
(
  int32                         handle,
  void                        * log_pkt_ptr
)
{
  lan_lle_cb_type             * llc_ptr;
  ps_stat_inst_desc_llc_type  * llc_desc_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Sanity check the handle value
  -------------------------------------------------------------------------*/
  if ( ((int32)LAN_LLE_MAX < handle) || ((int32)LAN_LLE_MIN >= handle))
  {
    LOG_MSG_ERROR_1("Invalid handle to LLC instance.  Handle: %d", handle);
    return;
  }

  /*-------------------------------------------------------------------------
    Convert the handle into the instance control block
  -------------------------------------------------------------------------*/
  llc_ptr = &lle_array[handle];

  /*-------------------------------------------------------------------------
    LLC instance has not been initialized yet
  -------------------------------------------------------------------------*/
  if (!PS_IFACE_IS_VALID(llc_ptr->start_info.iface_ptr))
  {
    LOG_MSG_ERROR_1("Invalid Iface Pointer in LLC instance Handle: %d",
                   handle);
    return;
  }

  llc_desc_ptr = (ps_stat_inst_desc_llc_type *) log_pkt_ptr;

  llc_desc_ptr->handle       = (uint8)handle;
  llc_desc_ptr->iface_handle = (uint8)
                   (llc_ptr->start_info.iface_ptr)->iface_private.iface_index;
  memset(llc_desc_ptr->reserved, 0, 1);

} /* ps_stat_get_llc_inst_desc */

/*===========================================================================
FUNCTION PS_STAT_GET_LLC_DESC_LOG_PKT()

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
void ps_stat_get_llc_desc_log_pkt
(
  int32               handle
)
{
  int8                idx                = 0;
  int32               desc_size          = 0;
  uint8               inst_count         = 0;
  int32               log_pkt_avail_size = 0;
  uint8             * log_pkt_ptr        = NULL;
  uint8             * log_pkt_fill_ptr   = NULL;

  ps_stat_desc_all_inst_hdr_log_pkt_type      * desc_all_inst_pkt_ptr = NULL;
  ps_stat_desc_delta_inst_hdr_log_pkt_type    * delta_pkt_ptr         = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (PS_STAT_INST_DESC_ALL == handle)
  {
    /*-----------------------------------------------------------------------
      LAN_LLE_MIN is -1, so increment by 1 to start at LAN_LLE_RMNET1
    -----------------------------------------------------------------------*/
    for (idx = (int8)LAN_LLE_MIN + 1; idx < (int8)LAN_LLE_MAX; ++idx)
    {
      /*---------------------------------------------------------------------
        Only show if the instance has started
      ---------------------------------------------------------------------*/
      if (TRUE == lle_array[idx].started)
      {
        desc_size = sizeof(ps_stat_inst_desc_llc_type);

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

          /*-----------------------------------------------------------------
            Get the pointer to the log packet
          -----------------------------------------------------------------*/
          ps_stat_logging_get_log_pkt(log_pkt_avail_size,
                                      LOG_PS_STAT_DESC_ALL_LLC_INST_C,
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

        ps_stat_get_llc_inst_desc(idx, log_pkt_fill_ptr);

        /*-------------------------------------------------------------------
          Increment the fill pointer by the size, and decrement the count
          by the same value
        -------------------------------------------------------------------*/
        log_pkt_fill_ptr += (uint8) desc_size;
        log_pkt_avail_size -= desc_size;
        inst_count++;
      }
    } /* for all llc instances*/

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
  } /* if fill all llc instances */
  else
  {
    desc_size = sizeof(ps_stat_inst_desc_llc_type);

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
      Fill in the delta log parameters
    -----------------------------------------------------------------------*/
    delta_pkt_ptr = (ps_stat_desc_delta_inst_hdr_log_pkt_type *) log_pkt_ptr;

    delta_pkt_ptr->type    = (uint8)PS_STAT_MODULE_INST_LLC;
    delta_pkt_ptr->deleted = FALSE;
    memset(delta_pkt_ptr->reserved, 0, 2);

    log_pkt_fill_ptr =
      (log_pkt_ptr + (uint8) sizeof(ps_stat_desc_delta_inst_hdr_log_pkt_type));

    ps_stat_get_llc_inst_desc(handle, log_pkt_fill_ptr);

    /*-----------------------------------------------------------------------
      Dispatch the packet
    -----------------------------------------------------------------------*/
    ps_stat_logging_commit_log_pkt
    (
      log_pkt_ptr,
      (desc_size + sizeof(ps_stat_desc_delta_inst_hdr_log_pkt_type))
    );
  } /* else fill one llc instance */
} /* ps_stat_get_llc_desc_log_pkt */


/*===========================================================================
FUNCTION PS_STAT_GET_LLC_STAT_LOG_PKT()

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
void ps_stat_get_llc_stat_log_pkt
(
  int32               handle
)
{
  int8                                  idx                  = 0;
  uint8                                * log_pkt_ptr          = NULL;
  lan_lle_cb_type                      * llc_ptr              = NULL;
  ps_stat_inst_llc_log_pkt_type        * inst_llc_log_pkt_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (PS_STAT_INST_DESC_ALL == handle)
  {
    /*-----------------------------------------------------------------------
      LAN_LLE_MIN is -1, so increment by 1 to start at LAN_LLE_RMNET1
    -----------------------------------------------------------------------*/
    for (idx = (int8)LAN_LLE_MIN + 1; idx < (int8)LAN_LLE_MAX; ++idx)
    {
      /*---------------------------------------------------------------------
        Only show if the instance has started
      ---------------------------------------------------------------------*/
      if (TRUE == lle_array[idx].started)
      {
        llc_ptr = &lle_array[idx];

        ps_stat_logging_get_log_pkt(sizeof(ps_stat_inst_llc_log_pkt_type),
                                    LOG_PS_STAT_LLC_INST_C,
                                    &log_pkt_ptr);

        if (NULL == log_pkt_ptr)
        {
          LOG_MSG_ERROR_0("Log packet allocated for LLC failed");
          return;
        }

        inst_llc_log_pkt_ptr = (ps_stat_inst_llc_log_pkt_type *) log_pkt_ptr;

        inst_llc_log_pkt_ptr->handle = idx;

        (void) ps_stat_get_llc(PS_STAT_LLC_ALL,
                                (void *) llc_ptr,
                                &(inst_llc_log_pkt_ptr->inst_llc_stats),
                                sizeof(ps_stat_llc_i_s_type));

        ps_stat_logging_commit_log_pkt
        (
          log_pkt_ptr,
          sizeof(ps_stat_inst_llc_log_pkt_type)
        );
      }
    } /* for all llc instances*/
  } /* if fill all llc instances */
  else
  {
    /*---------------------------------------------------------------------
      Only show if the instance has started
    ---------------------------------------------------------------------*/
    if (TRUE == lle_array[handle].started)
    {
      llc_ptr = &lle_array[handle];

      ps_stat_logging_get_log_pkt(sizeof(ps_stat_inst_llc_log_pkt_type),
                                  LOG_PS_STAT_LLC_INST_C,
                                  &log_pkt_ptr);

    
      if (NULL == log_pkt_ptr)
      {
        LOG_MSG_ERROR_0("Log packet allocated for LLC failed");
        return;
      }

      inst_llc_log_pkt_ptr = (ps_stat_inst_llc_log_pkt_type *) log_pkt_ptr;

      inst_llc_log_pkt_ptr->handle  = handle;

      (void) ps_stat_get_llc(PS_STAT_LLC_ALL,
                              (void *) llc_ptr,
                              &(inst_llc_log_pkt_ptr->inst_llc_stats),
                              sizeof(ps_stat_llc_i_s_type));

      ps_stat_logging_commit_log_pkt
      (
        log_pkt_ptr,
        sizeof(ps_stat_inst_llc_log_pkt_type)
      );
    }
  } /* else fill one llc instance */
} /* ps_stat_get_llc_stat_log_pkt */


/*===========================================================================

                             EXTERNAL FUNCTIONS

===========================================================================*/

/*===========================================================================
FUNCTION PS_STAT_RESET_LLC()

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
void ps_stat_reset_llc
(
  int32              handle
)
{
  lan_lle_cb_type  * llc_cb_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (handle <= (int32)LAN_LLE_MIN || handle >= (int32)LAN_LLE_MAX)
  {
    LOG_MSG_ERROR_0("Invalid device number passed");
    return;
  }

  llc_cb_ptr = &lle_array[handle];

  if (PS_STAT_QUERIED == llc_cb_ptr->llc_i_stats.queried)
  {
    ps_stat_get_llc_stat_log_pkt(handle);
  }

  memset(&(llc_cb_ptr->llc_i_stats), 0, sizeof(llc_cb_ptr->llc_i_stats));

  return;
} /* ps_stat_reset_llc */


/*===========================================================================
FUNCTION PS_STAT_INIT_LLC()

DESCRIPTION

DEPENDENCIES
  None

PARAMETERS

RETURN VALUE

SIDE EFFECTS
  None
===========================================================================*/
void ps_stat_init_llc
(
  void
)
{
 /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  LLC_FILL_INSTANCE_AT_STRUCT(0, llc_i_stats.frms_tx, uint32);
  LLC_FILL_INSTANCE_AT_STRUCT(1, llc_i_stats.llc_mode0_tx, uint32);
  LLC_FILL_INSTANCE_AT_STRUCT(2, llc_i_stats.llc_mode1_tx, uint32);
  LLC_FILL_INSTANCE_AT_STRUCT(3, llc_i_stats.frms_fail_tx, uint32);
  LLC_FILL_INSTANCE_AT_STRUCT(4, llc_i_stats.unknown_proto_drop_tx, 
                              uint32);
  LLC_FILL_INSTANCE_AT_STRUCT(5, llc_i_stats.invalid_mode_drop_tx, 
                              uint32);
  LLC_FILL_INSTANCE_AT_STRUCT(6, llc_i_stats.frms_rx, uint32);
  LLC_FILL_INSTANCE_AT_STRUCT(7, llc_i_stats.llc_mode0_rx, uint32);
  LLC_FILL_INSTANCE_AT_STRUCT(8, llc_i_stats.llc_mode1_rx, uint32);
  LLC_FILL_INSTANCE_AT_STRUCT(9, llc_i_stats.llc_control_frms_rx, 
                              uint32);
  LLC_FILL_INSTANCE_AT_STRUCT(10, llc_i_stats.frms_drop_rx, uint32);
  LLC_FILL_INSTANCE_AT_STRUCT(11, llc_i_stats.instance_stop_rx, uint32);
  LLC_FILL_INSTANCE_AT_STRUCT(12, llc_i_stats.instance_start_pending_rx
                              , uint32);
  LLC_FILL_INSTANCE_AT_STRUCT(13, llc_i_stats.inv_dest_addr_rx, uint32);
  LLC_FILL_INSTANCE_AT_STRUCT(14, llc_i_stats.inv_src_addr_rx, uint32);
  LLC_FILL_INSTANCE_AT_STRUCT(15, llc_i_stats.inv_mode_rx, uint32);
  LLC_FILL_INSTANCE_AT_STRUCT(16, llc_i_stats.mode0_inv_ethlen_rx, 
                              uint32);
  LLC_FILL_INSTANCE_AT_STRUCT(17, llc_i_stats.mode1_inv_snaphdr_len_rx,
                              uint32);
  LLC_FILL_INSTANCE_AT_STRUCT(18, llc_i_stats.mode1_inv_snaphdr_rx, 
                              uint32);
  LLC_FILL_INSTANCE_AT_STRUCT(19, llc_i_stats.mode1_inv_oui_rx, uint32);
  LLC_FILL_INSTANCE_AT_STRUCT(20, llc_i_stats.mode1_vlan_frms_rx, 
                              uint32);
  LLC_FILL_INSTANCE_AT_STRUCT(21, llc_i_stats.mode1_inv_vlan_len_rx, 
                              uint32);
  LLC_FILL_INSTANCE_AT_STRUCT(22, llc_i_stats.mode1_inv_vlan_frm_rx, 
                              uint32);
  LLC_FILL_INSTANCE_AT_STRUCT(23, llc_i_stats.mode1_inv_ethlen_rx, 
                              uint32);
  LLC_FILL_INSTANCE_AT_STRUCT(24, llc_i_stats.llc_802_1x_unsupported, 
                              uint32);
  LLC_FILL_INSTANCE_AT_STRUCT(25, 
                              llc_i_stats.llc_802_1x_preauth_unsupported, 
                              uint32);
  LLC_FILL_INSTANCE_AT_STRUCT(26, 
                              llc_i_stats.llc_wapi_unsupported, 
                              uint32);
  /*-------------------------------------------------------------------------
    for LLC_ALL offset starts from frms_tx and includes the entire
    stucture.
  -------------------------------------------------------------------------*/
  LLC_FILL_INSTANCE_AT_STRUCT(27, llc_i_stats.frms_tx, 
                              ps_stat_llc_i_s_type);

  ps_stat_logging_inst_reg_fill_desc_cback(PS_STAT_MODULE_INST_LLC,
                                           ps_stat_get_llc_desc_log_pkt);

  ps_stat_logging_inst_reg_get_stat_cback(PS_STAT_MODULE_INST_LLC,
                                          ps_stat_get_llc_stat_log_pkt);

  ps_stat_logging_inst_reg_reset_stat_cback(PS_STAT_MODULE_INST_LLC,
                                            ps_stat_reset_llc);

} /* ps_stat_init_llc */


/*===========================================================================
FUNCTION PS_STAT_GET_LLC()

DESCRIPTION
  This function supplies the required 802.2 LLC layer statistics. The
  first argument passed to the function is an enum that denotes the
  metric of interest.The second argument will be a pointer to the
  address of the llc control block. The third argument points to the
  memory location where the results will be copied.The result (statistic
  ) will be copied to this memory address. The fourth argument passes
  the length of memory allocated for the return value and should be
  greater than or equal to the memory size of the statistic requested.

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
errno_enum_type ps_stat_get_llc
(
  ps_stat_llc_enum_type     stat,
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
    LOG_MSG_ERROR_0(" Instance pointer should NOT be NULL for 802.2 LAN LLC"
                    " stats");
    return E_INVALID_ARG ;
  }

  if (stat == PS_STAT_LLC_ALL)
  {
    return ps_stat_get_llc_instance_all(stat,
                                        instance_ptr,
                                        return_value,
                                        ret_len
                                       ) ;
  }
  else
  {
    return ps_stat_get_common((uint32)stat,
                              ps_stat_llc_i_table,
                              (uint16)PS_STAT_LLC_ALL,
                              instance_ptr,
                              return_value,
                              ret_len
                             ) ;
  }
} /* ps_stat_get_llc */

/*===========================================================================
FUNCTION PS_STAT_GET_LLC_CONTROL_BLOCK()

DESCRIPTION
  This function supplies the address of the LLC control block
  corresponding to the LLC device passed.

DEPENDENCIES
  None

RETURN VALUE
  E_SUCCESS (0) for success and any other number for failure . The error
  codes are as defined in errno.h.

PARAMETERS
  device        - IN  - LLC device
  llccb_address - OUT - address of the llc control block corresponding
  to the device. This is a void ** so that a void* value can be
  returned back that can be used to call ps_stat_get_llc

SIDE EFFECTS
  None
===========================================================================*/
errno_enum_type ps_stat_get_llc_control_block
(
  int     device ,
  void    **llccb_address
)
{
  if (device <= (int32)LAN_LLE_MIN || device >= (int32)LAN_LLE_MAX)
  {
    LOG_MSG_ERROR_0(" Invalid device number passed");
    return E_INVALID_ARG ;
  }

  if (llccb_address == NULL)
  {
    return E_INVALID_ARG;
  }

  /*-------------------------------------------------------------------------
    Copy the control block address
  -------------------------------------------------------------------------*/
  *llccb_address = (void *) (&lle_array[device]) ;
  return E_SUCCESS ;
}

