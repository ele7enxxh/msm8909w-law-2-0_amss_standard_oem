/*===========================================================================

                          P S _ S T A T _ P H Y S _ L I N K . C

GENERAL DESCRIPTION
  This program specifically deals with statistics at the phys_link layer of
  the data servcies stack

EXTERNALIZED FUNCTIONS
  ps_stat_get_phys_link
    This function supplies the required phys_link stats.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2005-2014 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/netiface/src/ps_stat_phys_link.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/09/14   ds      Adding FEATURE_DATA_PS_ROHC.
12/08/10   rp      RAM optimization changes.
04/11/07   scb     Fixed High LINT errors
02/27/07   scb     Added changes to fix RCVT compilation warning
08/15/05   kr      Initial version

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"
#include "comdef.h"
#include "IxErrno.h"
#include "err.h"
#include "datamodem_variation.h"

#include "ps_stat_phys_link.h"
#include "ps_stat_common.h"
#include "ps_stat_logging.h"
#include "ds_Utils_DebugMsg.h"

#include "ps_phys_link.h"
#include "ps_iface.h"

#ifdef FEATURE_DATA_PS_ROHC
#include "ps_rohc.h"
#endif /* FEATURE_DATA_PS_ROHC */

#include "ps_iphc.h"

#include <stddef.h>
#include <string.h>

/*===========================================================================
MACRO PHYS_LINK_STAT_OFFSET_SIZE(INDEX,FIELD,SIZE)

DESCRIPTION
  This macro fills the offset and size fields of the ps_stat_phys_link_i_table.
  INDEX used only for readability,not for usage
  
DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define PHYS_LINK_STAT_OFFSET_SIZE(INDEX,FIELD,SIZE)                    \
{																		\
  FPOS_STAT(ps_phys_link_type,FIELD),									\
  sizeof(SIZE)															\
}

/*---------------------------------------------------------------------------
  PHYS_LINK stats access table
-------------------------------------------------------------------------- */
const ps_stat_access_table_s_type 
	  ps_stat_phys_link_i_table[PS_STAT_PHYS_LINK_ALL + 1] = 
{
  PHYS_LINK_STAT_OFFSET_SIZE(0,phys_link_i_stats.pkts_tx,	  uint32),
  PHYS_LINK_STAT_OFFSET_SIZE(1,phys_link_i_stats.bytes_tx,	  uint64),
  PHYS_LINK_STAT_OFFSET_SIZE(2,phys_link_i_stats.pkts_rx,	  uint32),
  PHYS_LINK_STAT_OFFSET_SIZE(3,phys_link_i_stats.bytes_rx,	  uint64),
  PHYS_LINK_STAT_OFFSET_SIZE(4,phys_private.ref_cnt,		  uint32),
  PHYS_LINK_STAT_OFFSET_SIZE(5,phys_private.state,			  uint16),
  /*---------------------------------------------------------------------------
  Indices 6 and 7 are unused. Fill with zeros
  - Refer ps_stat_phys_link_enum_type
  -------------------------------------------------------------------------- */
  STAT_FILL_NULL(6),
  STAT_FILL_NULL(7)
};

/*===========================================================================

                             INTERNAL FORWARD DECLARATIONS

===========================================================================*/
static errno_enum_type ps_stat_get_phys_link_is_primary
(
  ps_stat_phys_link_enum_type  stat,
  ps_phys_link_type            *phys_link_ptr,
  void                         *return_value,
  uint16                       ret_len
);

static errno_enum_type ps_stat_get_phys_link_instance_all
(
  ps_stat_phys_link_enum_type  stat,
  ps_phys_link_type            *phys_link_ptr,
  void                         *return_value,
  uint16                       ret_len
);

static errno_enum_type ps_stat_get_phys_link_control_block
(
  int                   handle,
  ps_phys_link_type  ** ctrl_blk_ptr
);

static void ps_stat_get_phys_link_desc_log_pkt
(
  int32                                   handle
);

static void ps_stat_get_phys_link_stat_log_pkt
(
  int32                                   handle
);
/*===========================================================================
FUNCTION PS_STAT_GET_PHYS_LINK_IS_PRIMARY()

DESCRIPTION
  This function supplies the phys_link is_primary statistic

DEPENDENCIES
  None

RETURN VALUE

  E_SUCCESS (0) for success and any other number for failure . The error
  codes are as defined in errno.h.

PARAMETERS
  stat            - IN - type of stat
  phys_link_ptr  -  IN - pointer that points to the PHYS_LINK control block
  return_value    - IN - pointer for return value
                  - OUT- return value (statistics)
  ret_len         - length of memory allocated

SIDE EFFECTS
  None
===========================================================================*/
errno_enum_type ps_stat_get_phys_link_is_primary
(
  ps_stat_phys_link_enum_type  stat,
  ps_phys_link_type            *phys_link_ptr,
  void                         *return_value,
  uint16                       ret_len
)
{
  uint32 *is_primary;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    validate 'stat'; only PS_STAT_PHYS_LINK_IS_PRIMARY should be passed to
    this function
  -------------------------------------------------------------------------*/
  if (stat != PS_STAT_PHYS_LINK_IS_PRIMARY)
  {
    DATA_ERR_FATAL("ps_stat_get_phys_link_is_primary(): Invalid stat value passed to ps_stat_get_phys_link_i_all");
    return E_NOT_ALLOWED ;
  }

  /*-------------------------------------------------------------------------
    validate phys_link_ptr : should not be NULL
  -------------------------------------------------------------------------*/
  if (phys_link_ptr == NULL)
  {
    LOG_MSG_ERROR_0("ps_stat_get_phys_link_is_primary: "
                    "Null value of phys_link_ptr passed");
    ASSERT(0);
    return E_NOT_ALLOWED ;
  }

  /*-------------------------------------------------------------------------
    return_value validation : should not be null
  -------------------------------------------------------------------------*/

  if (return_value == NULL)
  {
    LOG_MSG_ERROR_0("ps_stat_get_phys_link_is_primary(): "
                    "return_value should not be NULL ");
    return E_NOT_ALLOWED ;
  }

  /*-------------------------------------------------------------------------
    check the sizeof the stat requested. return appropriate error
  -------------------------------------------------------------------------*/

  if (ret_len < sizeof(uint32))
  {
    LOG_MSG_ERROR_0("ps_stat_get_phys_link_is_primary(): "
                    " Insufficient memory allocated for ptr return_value");
    return E_NOT_ALLOWED ;
  }

  is_primary = (uint32 *) return_value ;

  *is_primary =
  PS_PHYS_LINKI_GET_CAPABILITY(phys_link_ptr, PS_PHYS_LINK_CAPABILITY_PRIMARY)
  ;
  return E_SUCCESS ;
} /* ps_stat_get_phys_link_is_primary */


/*===========================================================================
FUNCTION PS_STAT_GET_PHYS_LINK_INSTANCE_ALL()

DESCRIPTION
  This function supplies all the PHYS_LINK statistics for the instance handle
  supplied. This is needed because of the need to extract and format data
  available in the phys_link control block

DEPENDENCIES
  None

RETURN VALUE

  E_SUCCESS (0) for success and any other number for failure . The error
  codes are as defined in errno.h.

PARAMETERS
  stat            - IN - type of stat
  phys_link_ptr   - IN - pointer that points to the PHYS_LINK control block
  return_value    - IN - pointer for return value
                  - OUT- return value (statistics)
  ret_len         - length of memory allocated

SIDE EFFECTS
  None
===========================================================================*/
errno_enum_type ps_stat_get_phys_link_instance_all
(
  ps_stat_phys_link_enum_type  stat,
  ps_phys_link_type            *phys_link_ptr,
  void                         *return_value,
  uint16                       ret_len
)
{
   ps_stat_phys_link_i_s_type *phys_link_i_stats;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    validate 'stat'; only PS_STAT_PHYS_LINK_ALL
    should be passed to this function
  -------------------------------------------------------------------------*/
  if (stat != PS_STAT_PHYS_LINK_ALL)
  {
    DATA_ERR_FATAL("ps_stat_get_phys_link_instance_all(): Invalid stat value passed to ps_stat_get_phys_link_i_all");
    return E_NOT_ALLOWED ;
  }

  /*-------------------------------------------------------------------------
    validate phys_link_ptr : should not be NULL
  -------------------------------------------------------------------------*/
  if (phys_link_ptr == NULL)
  {
    LOG_MSG_ERROR_0("ps_stat_get_phys_link_instance_all: "
                    "Null value of phys_link_ptr passed");
    ASSERT(0);
    return E_NOT_ALLOWED ;
  }

  /*-------------------------------------------------------------------------
    return_value validation : should not be null
  -------------------------------------------------------------------------*/

  if (return_value == NULL)
  {
    LOG_MSG_ERROR_0("ps_stat_get_phys_link_instance_all(): "
                    "return_value should not be NULL ");
    return E_NOT_ALLOWED ;
  }

  /*-------------------------------------------------------------------------
    check the sizeof the stat requested. If the size is zero, the stat is
    invalid in the current context. return appropriate error
  -------------------------------------------------------------------------*/

  if (ret_len < sizeof(ps_stat_phys_link_i_s_type))
  {
    LOG_MSG_ERROR_0("ps_stat_get_phys_link_instance_all(): "
                    " Insufficient memory allocated for ptr return_value");
    return E_NOT_ALLOWED ;
  }

  phys_link_i_stats = (ps_stat_phys_link_i_s_type *) return_value ;

  phys_link_i_stats->pkts_tx   = phys_link_ptr->phys_link_i_stats.pkts_tx ;
  phys_link_i_stats->bytes_tx  = phys_link_ptr->phys_link_i_stats.bytes_tx;
  phys_link_i_stats->pkts_rx   = phys_link_ptr->phys_link_i_stats.pkts_rx ;
  phys_link_i_stats->bytes_rx  = phys_link_ptr->phys_link_i_stats.bytes_rx;
  phys_link_i_stats->ref_cnt   = phys_link_ptr->phys_private.ref_cnt;
  phys_link_i_stats->state     = phys_link_ptr->phys_private.state ;
  phys_link_i_stats->is_primary =
  PS_PHYS_LINKI_GET_CAPABILITY(phys_link_ptr,PS_PHYS_LINK_CAPABILITY_PRIMARY);

  phys_link_ptr->phys_link_i_stats.queried = PS_STAT_QUERIED;

  return E_SUCCESS ;
} /* ps_stat_get_phys_link_instance_all */


/*===========================================================================
FUNCTION PS_STAT_GET_PHYS_LINK_CONTRL_BLOCK()

DESCRIPTION
  This function converts a QXDM handle into a physlink control block pointer.
  It is necessary to convert a QXDM handle into a control block pointer here
  because the common stats module does not have access to the phys link fctns.

DEPENDENCIES
  None

RETURN VALUE
  E_SUCCESS (0) for success and any other number for failure . The error
  codes are as defined in errno.h.

PARAMETERS
  handle          - IN - index of the ctrl blk of interest
  ctrl_blk_ptr    - OUT- ctrl blk ptr.  void** so it can be used directly
                         in the associated get functions

SIDE EFFECTS
  None
===========================================================================*/
errno_enum_type ps_stat_get_phys_link_control_block
(
  int                   handle,
  ps_phys_link_type  ** ctrl_blk_ptr
)
{
  ps_iface_type     * iface_ptr;
  uint32              iface_handle;
  uint32              phys_link_handle;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Convert the input handle into the correct iface and phys_link handles
    Phys_link handle is the lower 8 bits
    Iface handle is the next 8 bits
  -------------------------------------------------------------------------*/
  phys_link_handle = (handle & 0xFF);
  iface_handle = (handle >> 8);

  /*-------------------------------------------------------------------------
    Sanity check the iface handle value
  -------------------------------------------------------------------------*/
  if (MAX_SYSTEM_IFACES <= iface_handle)
  {
    LOG_MSG_ERROR_1("ps_stat_get_phys_link_control_block(): "
                    "Invalid handle to iface instance.  Handle: %d", handle);
    return E_INVALID_ARG;
  }

  /*-------------------------------------------------------------------------
    Convert the handle into the instance control block
  -------------------------------------------------------------------------*/
  iface_ptr = global_iface_ptr_array[iface_handle];

  /*-------------------------------------------------------------------------
    Iface instance has not been initialized yet
  -------------------------------------------------------------------------*/
  if (NULL == iface_ptr)
  {
    LOG_MSG_ERROR_1("ps_stat_get_phys_link_control_block(): "
                    "Attempting to reset statistics on an invalid iface instance. "
                    "Handle: %d", handle);
    return E_INVALID_ARG;
  }

  /*-------------------------------------------------------------------------
    Sanity check the phys_link handle value
  -------------------------------------------------------------------------*/
  if ( iface_ptr->iface_private.phys_link.instances < phys_link_handle)
  {
    LOG_MSG_ERROR_1("ps_stat_get_phys_link_control_block(): "
                    "Invalid handle to phys_link instance.  Handle: %d",
                    handle);
    return E_INVALID_ARG;
  }

  *ctrl_blk_ptr = (iface_ptr->iface_private.phys_link.array + phys_link_handle);

  return E_SUCCESS;
} /* ps_stat_get_phys_link_control_block */


/*===========================================================================
FUNCTION PS_STAT_GET_PHYS_LINK_DESC_LOG_PKT()

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
void ps_stat_get_phys_link_desc_log_pkt
(
  int32                                   handle
)
{
  ps_iface_type                         * iface_ptr;
  ps_phys_link_type                     * phys_link_ptr        = NULL;
  ps_stat_inst_desc_phys_link_type      * phys_link_desc_ptr;
  uint32                                  iface_handle;
  uint32                                  phys_link_handle;
  uint8                                   idx_iface;
  uint8                                   idx_phys_link;
  errno_enum_type                         ret;

  int32                         desc_size          = 0;
  uint8                         inst_count         = 0;
  int32                         log_pkt_avail_size = 0;
  uint8                       * log_pkt_ptr        = NULL;
  uint8                       * log_pkt_fill_ptr   = NULL;

  ps_stat_desc_all_inst_hdr_log_pkt_type     * desc_all_inst_pkt_ptr = NULL;
  ps_stat_desc_delta_inst_hdr_log_pkt_type   * delta_pkt_ptr         = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (PS_STAT_INST_DESC_ALL == handle)
  {
    for (idx_iface = 0; idx_iface < MAX_SYSTEM_IFACES; ++idx_iface)
    {
      /*---------------------------------------------------------------------
        Convert the handle into the instance control block
      ---------------------------------------------------------------------*/
      iface_ptr = global_iface_ptr_array[idx_iface];

      /*---------------------------------------------------------------------
        Interface has not been initialized yet, so skip
      ---------------------------------------------------------------------*/
      if ( (NULL == iface_ptr) ||
           (NULL == iface_ptr->iface_private.this_iface) )
      {
        continue;
      }

      for (idx_phys_link = 0;
           idx_phys_link < iface_ptr->iface_private.phys_link.instances;
           ++idx_phys_link)
      {
        phys_link_ptr =
          iface_ptr->iface_private.phys_link.array + idx_phys_link;
        iface_handle  = (iface_ptr->iface_private.iface_index) << 8;
        phys_link_handle = phys_link_ptr->phys_private.instance;

        desc_size = sizeof(ps_stat_inst_desc_phys_link_type);

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
          ps_stat_logging_get_log_pkt
          (
            log_pkt_avail_size,
            LOG_PS_STAT_DESC_ALL_PHYS_LINK_INST_C,
            &log_pkt_ptr
          );

          /*-----------------------------------------------------------------
            If we fail to get a log packet, just return
          -----------------------------------------------------------------*/
          if (NULL == log_pkt_ptr)
          {
            LOG_MSG_ERROR_0("ps_stat_get_phys_link_desc_log_pkt(): "
                            "Unable to allocate log packet");
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

        phys_link_desc_ptr =
          (ps_stat_inst_desc_phys_link_type *) log_pkt_fill_ptr;

        phys_link_desc_ptr->handle        = (uint16)(iface_handle | phys_link_handle);
        phys_link_desc_ptr->hdr_comp_type = phys_link_ptr->hdr_comp_protocol;

        switch (phys_link_ptr->hdr_comp_protocol)
        {
#ifdef FEATURE_DATA_PS_ROHC
          case PS_PHYS_LINK_HDR_COMP_PROTOCOL_ROHC:
            phys_link_desc_ptr->hdr_comp_handle =
              rohc_get_ctrl_blk_index(phys_link_ptr->hdr_comp_handle);
            break;
#endif /* FEATURE_DATA_PS_ROHC */

          case PS_PHYS_LINK_HDR_COMP_PROTOCOL_IPHC:
            phys_link_desc_ptr->hdr_comp_handle =
              iphc_get_ctrl_blk_index(phys_link_ptr->hdr_comp_handle);
            break;

          default:
            phys_link_desc_ptr->hdr_comp_handle = 0xFF;
            break;
        }

        /*-------------------------------------------------------------------
          Increment the fill pointer by the size, and decrement the count
          by the same value
        -------------------------------------------------------------------*/
        log_pkt_fill_ptr += (uint8) desc_size;
        log_pkt_avail_size -= desc_size;
        inst_count++;
      } /* for each instance in the iface */
    } /* for all interfaces on the system */

    /*-------------------------------------------------------------------------
      Dispatch the last packet
    -------------------------------------------------------------------------*/
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
  } /* if all instances */
  else
  {
    ret = ps_stat_get_phys_link_control_block(handle, &phys_link_ptr);

    if (E_SUCCESS != ret)
    {
      LOG_MSG_ERROR_1("ps_stat_get_phys_link_desc_log_pkt(): "
                      "Unable to get phys link ctl blk for get stats.  Handle: %d",
                      handle);
      return;
    }

    desc_size = sizeof(ps_stat_inst_desc_phys_link_type);

    /*-----------------------------------------------------------------------
      We need to allocate enough memory for the description and the standard
      header
    -----------------------------------------------------------------------*/
    log_pkt_avail_size = (desc_size +
                          sizeof(ps_stat_desc_delta_inst_hdr_log_pkt_type));

    ps_stat_logging_get_log_pkt(log_pkt_avail_size,
                                LOG_PS_STAT_DESC_DELTA_INST_C,
                                &log_pkt_ptr);

    /*-----------------------------------------------------------------------
      If we fail to get a log packet, just return
    -----------------------------------------------------------------------*/
    if (NULL == log_pkt_ptr)
    {
      LOG_MSG_ERROR_0("ps_stat_get_phys_link_desc_log_pkt(): "
                      "Unable to allocate log packet");
      return;
    }

    /*-----------------------------------------------------------------------
      Fill in the delta log parameters
    -----------------------------------------------------------------------*/
    delta_pkt_ptr = (ps_stat_desc_delta_inst_hdr_log_pkt_type *) log_pkt_ptr;

    delta_pkt_ptr->type    = PS_STAT_MODULE_INST_PHYS_LINK;
    delta_pkt_ptr->deleted = FALSE;
    memset(delta_pkt_ptr->reserved, 0, 2);

    phys_link_desc_ptr =
     (ps_stat_inst_desc_phys_link_type *)
     (log_pkt_ptr + (uint8) sizeof(ps_stat_desc_delta_inst_hdr_log_pkt_type));

    phys_link_desc_ptr->handle          = (uint16)handle;
    phys_link_desc_ptr->hdr_comp_type   = phys_link_ptr->hdr_comp_protocol;

    switch (phys_link_ptr->hdr_comp_protocol)
    {
#ifdef FEATURE_DATA_PS_ROHC
      case PS_PHYS_LINK_HDR_COMP_PROTOCOL_ROHC:
        phys_link_desc_ptr->hdr_comp_handle =
          rohc_get_ctrl_blk_index(phys_link_ptr->hdr_comp_handle);
        break;
#endif /* FEATURE_DATA_PS_ROHC */

      case PS_PHYS_LINK_HDR_COMP_PROTOCOL_IPHC:
        phys_link_desc_ptr->hdr_comp_handle =
          iphc_get_ctrl_blk_index(phys_link_ptr->hdr_comp_handle);
        break;

      default:
        phys_link_desc_ptr->hdr_comp_handle = 0xFF;
        break;
    }

    /*-----------------------------------------------------------------------
      Dispatch the last packet
    -----------------------------------------------------------------------*/
    ps_stat_logging_commit_log_pkt
    (
      log_pkt_ptr,
      (desc_size + sizeof(ps_stat_desc_delta_inst_hdr_log_pkt_type))
    );
  } /* else fill one phys_link instance */
} /* ps_stat_get_phys_link_desc_log_pkt */


/*===========================================================================
FUNCTION PS_STAT_GET_PHYS_LINK_STAT_LOG_PKT()

DESCRIPTION
  This function fills the statistics of the specified instance handle into
  a log packet.  If the handle matches the handle for all statistics, it
  copies all of the active instance statistics into the log packet.  If the
  handle does not specify all instances but specifies a valid active instance,
  the statistics associated with that instance is copied into the log packet.
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
void ps_stat_get_phys_link_stat_log_pkt
(
  int32                                   handle
)
{
  ps_iface_type                         * iface_ptr;
  ps_phys_link_type                     * phys_link_ptr        = NULL;
  uint32                                  iface_handle;
  uint32                                  phys_link_handle;
  uint8                                   idx_iface;
  uint8                                   idx_phys_link;

  errno_enum_type                         ret;
  uint8                                 * log_pkt_ptr;
  ps_stat_inst_phys_link_log_pkt_type   * inst_phys_link_log_pkt_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (PS_STAT_INST_DESC_ALL == handle)
  {
    for (idx_iface = 0; idx_iface < MAX_SYSTEM_IFACES; ++idx_iface)
    {
      /*---------------------------------------------------------------------
        Convert the handle into the instance control block
      ---------------------------------------------------------------------*/
      iface_ptr = global_iface_ptr_array[idx_iface];

      /*---------------------------------------------------------------------
        Interface has not been initialized yet, so skip
      ---------------------------------------------------------------------*/
      if ( (NULL == iface_ptr) ||
           (NULL == iface_ptr->iface_private.this_iface) )
      {
        continue;
      }

      for (idx_phys_link = 0;
           idx_phys_link < iface_ptr->iface_private.phys_link.instances;
           ++idx_phys_link)
      {
        phys_link_ptr =
          (iface_ptr->iface_private.phys_link.array + idx_phys_link);

        if (NULL == phys_link_ptr)
        {
          LOG_MSG_ERROR_0("ps_stat_get_phys_link_stat_log_pkt(): "
                          "Null phys link ptr");
          return;
        }

        iface_handle  = (iface_ptr->iface_private.iface_index) << 8;
        phys_link_handle = phys_link_ptr->phys_private.instance;

        ps_stat_logging_get_log_pkt(sizeof(ps_stat_inst_phys_link_log_pkt_type),
                                    LOG_PS_STAT_PHYS_LINK_INST_C,
                                    &log_pkt_ptr);

        if (NULL == log_pkt_ptr)
        {
          LOG_MSG_ERROR_0("ps_stat_get_phys_link_stat_log_pkt(): "
                          "Log pkt alloc for phys link get stats failed");
          return;
        }

        inst_phys_link_log_pkt_ptr =
          (ps_stat_inst_phys_link_log_pkt_type *) log_pkt_ptr;

        inst_phys_link_log_pkt_ptr->handle = (iface_handle | phys_link_handle);

        (void) ps_stat_get_phys_link
          (
            PS_STAT_PHYS_LINK_ALL,
            (void *) phys_link_ptr,
            &(inst_phys_link_log_pkt_ptr->inst_phys_link_stats),
            sizeof(ps_stat_phys_link_i_s_type)
          );

        ps_stat_logging_commit_log_pkt
        (
          log_pkt_ptr,
          sizeof(ps_stat_inst_phys_link_log_pkt_type)
        );
      } /* for each instance in the iface */
    } /* for all interfaces on the system */
  } /* if all instances */
  else
  {
    ret = ps_stat_get_phys_link_control_block(handle, &phys_link_ptr);

    if (E_SUCCESS != ret)
    {
      LOG_MSG_ERROR_1("ps_stat_get_phys_link_stat_log_pkt(): "
                      "Unable to get phys link ctl blk for get stats %d",
                      handle);
      return;
    }

    ps_stat_logging_get_log_pkt(sizeof(ps_stat_inst_phys_link_log_pkt_type),
                                LOG_PS_STAT_PHYS_LINK_INST_C,
                                &log_pkt_ptr);

    if (NULL == log_pkt_ptr)
    {
      LOG_MSG_ERROR_0("ps_stat_get_phys_link_stat_log_pkt(): "
                      "Log pkt alloc for phys link get stats failed");
      return;
    }

    inst_phys_link_log_pkt_ptr =
      (ps_stat_inst_phys_link_log_pkt_type *) log_pkt_ptr;

    inst_phys_link_log_pkt_ptr->handle = handle;

    (void) ps_stat_get_phys_link
      (
        PS_STAT_PHYS_LINK_ALL,
        (void *) phys_link_ptr,
        &(inst_phys_link_log_pkt_ptr->inst_phys_link_stats),
        sizeof(ps_stat_phys_link_i_s_type)
      );

    ps_stat_logging_commit_log_pkt
    (
      log_pkt_ptr,
      sizeof(ps_stat_inst_phys_link_log_pkt_type)
    );

  } /* else fill one phys_link instance */
} /* ps_stat_get_phys_link_stat_log_pkt */


/*===========================================================================

                             EXTERNAL FUNCTIONS

===========================================================================*/

/*===========================================================================
FUNCTION PS_STAT_RESET_PHYS_LINK()

DESCRIPTION
  This function resets the instance statistics associated with the specified
  instance.

DEPENDENCIES
  None

RETURN VALUE
  E_SUCCESS (0) for success and any other number for failure . The error
  codes are as defined in errno.h.

PARAMETERS
  handle : Handle to the instance

SIDE EFFECTS
  None
===========================================================================*/
void ps_stat_reset_phys_link
(
  int32                 handle
)
{
  ps_phys_link_type   * phys_link_cb_ptr;
  errno_enum_type       ret;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ret = ps_stat_get_phys_link_control_block(handle, &phys_link_cb_ptr);

  if (ret != E_SUCCESS)
  {
    LOG_MSG_ERROR_1("ps_stat_reset_phys_link(): "
                    "Unable to get phys_link control block for reset.  Handle: %d",
                    handle);
    return;
  }

  if (PS_STAT_QUERIED == phys_link_cb_ptr->phys_link_i_stats.queried)
  {
    ps_stat_get_phys_link_stat_log_pkt(handle);
  }

  memset(&(phys_link_cb_ptr->phys_link_i_stats), 0,
         sizeof(phys_link_cb_ptr->phys_link_i_stats));

  return;
} /* ps_stat_reset_phys_link */


/*===========================================================================
FUNCTION PS_STAT_INIT_PHYS_LINK()

DESCRIPTION

DEPENDENCIES
  None

PARAMETERS

RETURN VALUE

SIDE EFFECTS
  None
===========================================================================*/
void ps_stat_init_phys_link
(
  void
)
{
  ps_stat_logging_inst_reg_fill_desc_cback(PS_STAT_MODULE_INST_PHYS_LINK,
                                           ps_stat_get_phys_link_desc_log_pkt);

  ps_stat_logging_inst_reg_get_stat_cback(PS_STAT_MODULE_INST_PHYS_LINK,
                                          ps_stat_get_phys_link_stat_log_pkt);

  ps_stat_logging_inst_reg_reset_stat_cback(PS_STAT_MODULE_INST_PHYS_LINK,
                                            ps_stat_reset_phys_link);
} /* ps_stat_init_phys_link */


/*===========================================================================
FUNCTION PS_STAT_GET_PHYS_LINK()

DESCRIPTION
  This function supplies the required phys_link layer statistics. The first
  argument passed to the function is an enum that denotes the metric of
  interest.The second argument will be  a pointer to the address of the
  phys_link control block. The third argument points to the memory location
  where the results will be copied.The result (statistic) will be copied to
  this memory address. The fourth argument passes the length of memory
  allocated for the return value and should be  greater than or equal to
  the memory size of the statistic requested.

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
errno_enum_type ps_stat_get_phys_link
(
  ps_stat_phys_link_enum_type  stat,
  void                        *instance_ptr,
  void                        *return_value,
  uint16                       ret_len
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Check Instance pointer; Instance pointer should not be null
  -------------------------------------------------------------------------*/

  if (instance_ptr == NULL)
  {
    LOG_MSG_ERROR_0("ps_stat_get_phys_link(): "
                    " Instance pointer should NOT be NULL "
                    "for PHYS_LINK stats");
    return E_INVALID_ARG ;
  }

  if (stat == PS_STAT_PHYS_LINK_ALL)
  {
     return ps_stat_get_phys_link_instance_all(stat,
                                               instance_ptr,
                                               return_value,
                                                ret_len
                                              );
  }
  else if (stat == PS_STAT_PHYS_LINK_IS_PRIMARY)
  {
     return ps_stat_get_phys_link_is_primary(stat,
                                             instance_ptr,
                                             return_value,
                                             ret_len
                                            );

  }
  {
    return ps_stat_get_common(stat,
                              ps_stat_phys_link_i_table,
                              PS_STAT_PHYS_LINK_ALL,
                              instance_ptr,
                              return_value,
                              ret_len
                             );
  }

} /* ps_stat_get_phys_link */

