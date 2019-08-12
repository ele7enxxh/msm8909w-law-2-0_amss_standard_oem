/*===========================================================================

                 P S _ I F A C E _ R X _ Q O S _ F L T R . C

DESCRIPTION
  This file provides functions to manipulate QOS filters which are used by
  network to filter traffic in forward direction.

EXTERNALIZED FUNCTIONS
  PS_IFACEI_RX_QOS_FLTR_ADD()
    Adds pointers to Rx filters to the iface. Rx filters are actually
    stored in ps_flow

  PS_IFACEI_RX_QOS_FLTR_MODIFY()
    Adds pointers to Rx filters to the iface. Rx filters are actually
    stored in ps_flow. This function is called when existing Rx fltrs are
    modified in QOS_MODIFY

  PS_IFACEI_RX_QOS_FLTR_DELETE()
    Deletes pointers to Rx filters from the iface. Rx filters are actually
    stored in ps_flow

Copyright (c) 2005-2008 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/netiface/src/ps_iface_rx_qos_fltr.c#1 $
  $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
04/25/06    msr    L4/Tasklock code review changes
02/06/06    msr    Updated for L4 tasklock/crit sections.
08/16/05    msr    Fixed PS_BRANCH_TASKFREE()
08/15/05    mct    Fixed naming causing some issues w/C++ compilers.
08/03/05    msr    Added ps_ifacei_rx_qos_fltr_modify().
05/12/05    ks     fixed lint errors
05/03/05    msr    Using int16 instead of errno_enum_type.
04/17/05    msr    File created.
===========================================================================*/


/*===========================================================================

                       INCLUDE FILES FOR THE MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA_PS_QOS

#include "ps_iface_ipfltr.h"
#include "ps_mem.h"
#include "ps_iface.h"
#include "ps_flow.h"
#include "ps_flowi.h"
#include "ps_utils.h"
#include "ps_crit_sect.h"
#include "ps_iface_rx_qos_fltr.h"
#include "ds_Utils_DebugMsg.h"



/*===========================================================================

                             LOCAL DEFINITIONS

===========================================================================*/
#define MAX_FLTR_PRCD  255



/*===========================================================================

                             INTERNAL FUNCTIONS

===========================================================================*/
/*===========================================================================
FUNCTION PS_IFACEI_RX_QOS_FLTR_STORE_PTRS()

DESCRIPTION
  This function adds a set of Rx QOS filters to the specified iface. The
  filter set is tied to a filter id, which uniquely idenifies a set of
  filters for the given iface for the given ps_flow.

PARAMETERS
  iface_ptr          : Iface to add filters to
  qos_info_ptr       : address where Rx fltrs are stored in a ps_flow. Could
                       be qos_info_ptr or qos_modify_info_ptr
  fltr_prcd          : precedence to be assigned to fltrs
  fltr_priority      : priority class of filters
  fltr_compare_f_ptr : fltr comparator
  ps_errno           : error code passed back when storage fails.

RETURN VALUE
  A unique ID which is assigned to set of stored filters : on sucess
  PS_IFACE_RX_QOS_FLTR_INVALID_HANDLE                    : on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static ps_iface_rx_qos_fltr_handle_type ps_ifacei_rx_qos_fltr_store_ptrs
(
  ps_iface_type                 * iface_ptr,
  qos_info_type                 * qos_info_ptr,
  uint8                           fltr_prcd,
  fltr_priority_enum_type         fltr_priority,
  ipfltr_comparison_f_ptr_type    fltr_compare_f_ptr,
  int16                         * ps_errno
)
{
  ps_iface_rx_qos_fltr_node_type     * new_filter_node_ptr;
  ps_iface_rx_qos_fltr_node_type     * tmp_rx_fltr_buf;
  ps_flow_rx_fltr_buf_type           * fltr_buf_ptr;
  ps_iface_rx_qos_fltr_handle_type     fltr_handle;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  fltr_handle = PS_IFACE_IPFLTR_INVALID_HANDLE;

  fltr_buf_ptr = list_peek_front(&(qos_info_ptr->rx.fltr_list));
  while (fltr_buf_ptr != NULL)
  {
    /*-----------------------------------------------------------------------
      Get a buffer for rx filter from ps_mem. Store filters in the
      decreasing order of precedence.
    -----------------------------------------------------------------------*/
    new_filter_node_ptr =
      (ps_iface_rx_qos_fltr_node_type *)
        ps_mem_get_buf(PS_MEM_IFACE_RX_QOS_FLTR_NODE_TYPE);
    if (new_filter_node_ptr == NULL)
    {
      LOG_MSG_ERROR_2("ps_ifacei_rx_qos_fltr_store_ptrs(): "
                      "Couldn't create a buffer for Rx filter on iface 0x%x:%d",
                      iface_ptr->name, iface_ptr->instance);
      ps_ifacei_rx_qos_fltr_delete(iface_ptr, fltr_handle);

      *ps_errno = DS_ENOMEM;
      return PS_IFACE_IPFLTR_INVALID_HANDLE;
    }

    if (fltr_handle == PS_IFACE_IPFLTR_INVALID_HANDLE)
    {
      fltr_handle = (uint32) new_filter_node_ptr;
    }

    new_filter_node_ptr->fltr_buf_ptr             = fltr_buf_ptr;
    new_filter_node_ptr->fltr_handle              = fltr_handle;
    new_filter_node_ptr->fltr_buf_ptr->precedence = fltr_prcd++;

    /*-------------------------------------------------------------------------
       Add the new filters to the flow sorted by precedence
    -------------------------------------------------------------------------*/
    tmp_rx_fltr_buf =
      list_peek_front(&(iface_ptr->iface_private.rx_qos_fltr_node_list));

    while (tmp_rx_fltr_buf != NULL &&
           new_filter_node_ptr->fltr_buf_ptr->precedence >=
             tmp_rx_fltr_buf->fltr_buf_ptr->precedence)
    {
      tmp_rx_fltr_buf =
        list_peek_next(&(iface_ptr->iface_private.rx_qos_fltr_node_list),
                       &(tmp_rx_fltr_buf->link));
    }

    if (NULL == tmp_rx_fltr_buf)
    {
      list_push_back(&(iface_ptr->iface_private.rx_qos_fltr_node_list),
                     &(new_filter_node_ptr->link));
    }
    else
    {
      list_push_before(&(iface_ptr->iface_private.rx_qos_fltr_node_list),
                       &(new_filter_node_ptr->link), &(tmp_rx_fltr_buf->link));
    }

    fltr_buf_ptr = list_peek_next(&(qos_info_ptr->rx.fltr_list),
                                  &(fltr_buf_ptr->link));
  } /* while there is a fltr in the list */

  return fltr_handle;
} /* ps_ifacei_rx_qos_fltr_store_ptrs() */



/*===========================================================================

                             EXTERNAL FUNCTIONS

===========================================================================*/
/*===========================================================================
FUNCTION PS_IFACEI_RX_QOS_FLTR_ADD()

DESCRIPTION
  This function adds a set of Rx QOS filters to the specified iface. The
  filter set is tied to a filter id, which uniquely idenifies a set of
  filters for the given iface for the given ps_flow.

PARAMETERS
  iface_ptr          : Iface to add filters to
  qos_info_ptr       : address where Rx fltrs are stored in a ps_flow. Could
                       be qos_info_ptr or qos_modify_info_ptr
  fltr_priority      : priority class of filters
  fltr_compare_f_ptr : fltr comparator
  ps_errno           : error code passed back when storage fails.

RETURN VALUE
  A unique ID which is assigned to set of added filters : on sucess
  PS_IFACE_RX_QOS_FLTR_INVALID_HANDLE                   : on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
ps_iface_rx_qos_fltr_handle_type ps_ifacei_rx_qos_fltr_add
(
  ps_iface_type                 * iface_ptr,
  qos_info_type                 * qos_info_ptr,
  fltr_priority_enum_type         fltr_priority,
  ipfltr_comparison_f_ptr_type    fltr_compare_f_ptr,
  int16                         * ps_errno
)
{
  ps_iface_rx_qos_fltr_node_type     * filter_node_ptr;
  ps_iface_rx_qos_fltr_handle_type     fltr_handle;
  uint8                                fltr_prcd;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (ps_errno == NULL)
  {
    DATA_ERR_FATAL("ps_ifacei_rx_qos_fltr_add(): NULL parameter is passed");
    return PS_IFACE_IPFLTR_INVALID_HANDLE;
  }

  if (qos_info_ptr == NULL)
  {
    DATA_ERR_FATAL("ps_ifacei_rx_qos_fltr_add(): qos_info_ptr is NULL");
    *ps_errno = DS_EFAULT;
    return PS_IFACE_IPFLTR_INVALID_HANDLE;
  }

  if (fltr_priority != PS_IFACE_IPFLTR_PRIORITY_FCFS)
  {
    LOG_MSG_ERROR_1("ps_ifacei_rx_qos_fltr_add(): "
                    "Fltr priority style, %d, is not supported",
                    fltr_priority);
    ASSERT(0);
    *ps_errno = DS_EFAULT;
    return PS_IFACE_IPFLTR_INVALID_HANDLE;
  }

  if (!PS_IFACE_IS_VALID(iface_ptr))
  {
    LOG_MSG_ERROR_1("ps_ifacei_rx_qos_fltr_add(): "
                    "iface 0x%p is invalid", iface_ptr);
    ASSERT(0);
    *ps_errno = DS_EFAULT;
    return PS_IFACE_IPFLTR_INVALID_HANDLE;
  }

  LOG_MSG_INFO2_2("ps_ifacei_rx_qos_fltr_add(): "
                  "Adding pointers to Rx fltrs to iface 0x%x:%d",
                  iface_ptr->name, iface_ptr->instance);

  /*-------------------------------------------------------------------------
    Find a block in precedence space which can be used to assign precedence
    for newly added filters
  -------------------------------------------------------------------------*/
  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  filter_node_ptr =
    list_peek_back(&(iface_ptr->iface_private.rx_qos_fltr_node_list));
  if (filter_node_ptr == NULL)
  {
    fltr_prcd = 0;
  }
  else
  {
    fltr_prcd = filter_node_ptr->fltr_buf_ptr->precedence;

    if ((fltr_prcd + MAX_FLTR_PER_REQ) < MAX_FLTR_PRCD) /* Blk found at end */
    {
      /*---------------------------------------------------------------------
        Get next multiple of MAX_FLTR_PER_REQ
      ---------------------------------------------------------------------*/
      fltr_prcd += MAX_FLTR_PER_REQ - (fltr_prcd % MAX_FLTR_PER_REQ);
    }
    else /* Wrap around happened */
    {
      /*---------------------------------------------------------------------
        Get first block in precedence space which is available. Since FCFS
        with blocks are used to assign precedence, a partially full block is
        not considered as a hole
      ---------------------------------------------------------------------*/
      fltr_prcd = 0;
      filter_node_ptr =
        list_peek_front(&(iface_ptr->iface_private.rx_qos_fltr_node_list));
      while (filter_node_ptr != NULL)
      {
        if (filter_node_ptr->fltr_buf_ptr->precedence > fltr_prcd)
        {
          break;
        }
        else
        {
          /*-----------------------------------------------------------------
            Skip this block since precedence is already used
          -----------------------------------------------------------------*/
          if (filter_node_ptr->fltr_buf_ptr->precedence == fltr_prcd)
          {
            fltr_prcd += MAX_FLTR_PER_REQ;
          }

          filter_node_ptr =
            list_peek_next(&(iface_ptr->iface_private.rx_qos_fltr_node_list),
                           &(filter_node_ptr->link));
        }
      }

      if (filter_node_ptr == NULL)
      {
        PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
        LOG_MSG_ERROR_2("ps_ifacei_rx_qos_fltr_add(): "
                        "Couldn't find a hole in precedence space iface 0x%x:%d",
                        iface_ptr->name, iface_ptr->instance);
        ASSERT(0);
        *ps_errno = DS_ENOMEM;
        return PS_IFACE_IPFLTR_INVALID_HANDLE;
      }
    }
  }

  LOG_MSG_INFO2_1("ps_ifacei_rx_qos_fltr_add(): "
                  "Assigning fltr precedence from %d", fltr_prcd);

  fltr_handle = ps_ifacei_rx_qos_fltr_store_ptrs(iface_ptr,
                                                 qos_info_ptr,
                                                 fltr_prcd,
                                                 fltr_priority,
                                                 fltr_compare_f_ptr,
                                                 ps_errno);
  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
  return fltr_handle;

} /* ps_ifacei_rx_qos_fltr_add() */



/*===========================================================================
FUNCTION PS_IFACEI_RX_QOS_FLTR_MODIFY()

DESCRIPTION
  This function adds a set of Rx QOS filters to the specified iface. The
  filter set is tied to a filter id, which uniquely idenifies a set of
  filters for the given iface for the given ps_flow.

PARAMETERS
  iface_ptr          : Iface to add filters to
  qos_info_ptr       : address where Rx fltrs are stored in a ps_flow. Has
                       to be qos_modify_info_ptr
  rx_fltr_handle     : handle to existing Rx fltrs specified in current QOS
  fltr_priority      : priority class of filters
  fltr_compare_f_ptr : fltr comparator
  ps_errno           : error code passed back when storage fails.

RETURN VALUE
  A unique ID which is assigned to set of added filters : on sucess
  PS_IFACE_RX_QOS_FLTR_INVALID_HANDLE                   : on failure

DEPENDENCIES
  Must be called only if existing Rx fltrs are modified in QOS_MODIFY
  iface critical section must have been entered

SIDE EFFECTS
  None
===========================================================================*/
ps_iface_rx_qos_fltr_handle_type ps_ifacei_rx_qos_fltr_modify
(
  ps_iface_type                     * iface_ptr,
  qos_info_type                     * qos_info_ptr,
  ps_iface_rx_qos_fltr_handle_type    rx_fltr_handle,
  fltr_priority_enum_type             fltr_priority,
  ipfltr_comparison_f_ptr_type        fltr_compare_f_ptr,
  int16                             * ps_errno
)
{
  ps_iface_rx_qos_fltr_node_type     * filter_node_ptr;
  ps_iface_rx_qos_fltr_handle_type     fltr_handle;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (ps_errno == NULL)
  {
    DATA_ERR_FATAL("ps_ifacei_rx_qos_fltr_modify(): NULL parameter is passed");
    return PS_IFACE_IPFLTR_INVALID_HANDLE;
  }

  if (qos_info_ptr == NULL)
  {
    DATA_ERR_FATAL("ps_ifacei_rx_qos_fltr_modify(): qos_info_ptr is NULL");
    *ps_errno = DS_EFAULT;
    return PS_IFACE_IPFLTR_INVALID_HANDLE;
  }

  if (fltr_priority != PS_IFACE_IPFLTR_PRIORITY_FCFS)
  {
    LOG_MSG_ERROR_1("ps_ifacei_rx_qos_fltr_modify(): "
                    "Fltr priority style, %d, is not supported",
                    fltr_priority);
    ASSERT(0);
    *ps_errno = DS_EFAULT;
    return PS_IFACE_IPFLTR_INVALID_HANDLE;
  }

  if (!PS_IFACE_IS_VALID(iface_ptr))
  {
    LOG_MSG_ERROR_1("ps_ifacei_rx_qos_fltr_modify(): "
                    "iface 0x%p is invalid", iface_ptr);
    ASSERT(0);
    *ps_errno = DS_EFAULT;
    return PS_IFACE_IPFLTR_INVALID_HANDLE;
  }

  LOG_MSG_INFO2_2("ps_ifacei_rx_qos_fltr_modify(): "
                  "Modifying pointers to Rx fltrs on iface 0x%x:%d",
                  iface_ptr->name, iface_ptr->instance);

  /*-------------------------------------------------------------------------
    Find a block in precedence space which can be used to assign precedence
    for newly added filters
  -------------------------------------------------------------------------*/
  filter_node_ptr =
    list_peek_front(&(iface_ptr->iface_private.rx_qos_fltr_node_list));
  while (filter_node_ptr != NULL &&
         filter_node_ptr->fltr_handle != rx_fltr_handle)
  {
    filter_node_ptr =
      list_peek_next(&(iface_ptr->iface_private.rx_qos_fltr_node_list),
                     &(filter_node_ptr->link));
  }

  if (filter_node_ptr == NULL)
  {
    LOG_MSG_ERROR_3("ps_ifacei_rx_qos_fltr_modify(): "
                    "Couldn't find filters to modify on iface 0x%x:%d. Passed "
                    "fltr_handle is 0x%x",
                    iface_ptr->name, iface_ptr->instance, rx_fltr_handle);
    ASSERT(0);
    *ps_errno = DS_EFAULT;
    return PS_IFACE_IPFLTR_INVALID_HANDLE;
  }

  fltr_handle = ps_ifacei_rx_qos_fltr_store_ptrs
                (
                  iface_ptr,
                  qos_info_ptr,
                  filter_node_ptr->fltr_buf_ptr->precedence,
                  fltr_priority,
                  fltr_compare_f_ptr,
                  ps_errno
                );
  return fltr_handle;

} /* ps_ifacei_rx_qos_fltr_modify() */



/*===========================================================================
FUNCTION PS_IFACEI_RX_QOS_FLTR_DELETE()

DESCRIPTION
  This function deletes all the existing Rx QOS filters identified by the
  specified filter handle from the specified iface.

PARAMETERS
  iface_ptr      : Iface to delete filters from
  rx_fltr_handle : handle identifying filters to be deleted

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_ifacei_rx_qos_fltr_delete
(
  ps_iface_type                     * iface_ptr,
  ps_iface_rx_qos_fltr_handle_type    rx_fltr_handle
)
{
  ps_iface_rx_qos_fltr_node_type  * curr_fltr_node_ptr;
  ps_iface_rx_qos_fltr_node_type  * next_fltr_node_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO2_2("ps_ifacei_rx_qos_fltr_delete(): "
                  "Deleting pointers to Rx fltrs on iface 0x%x:%d",
                  iface_ptr->name, iface_ptr->instance);

  /*-------------------------------------------------------------------------
    Free Rx filters from iface and add them to ps_mem pool
  -------------------------------------------------------------------------*/
  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  curr_fltr_node_ptr =
    list_peek_front(&(iface_ptr->iface_private.rx_qos_fltr_node_list));
  while (curr_fltr_node_ptr != NULL)
  {
    next_fltr_node_ptr =
      list_peek_next(&(iface_ptr->iface_private.rx_qos_fltr_node_list),
                     &(curr_fltr_node_ptr->link));
    if (curr_fltr_node_ptr->fltr_handle == rx_fltr_handle)
    {
      list_pop_item(&(iface_ptr->iface_private.rx_qos_fltr_node_list),
                    &(curr_fltr_node_ptr->link));
      PS_MEM_FREE(curr_fltr_node_ptr);
    }

    curr_fltr_node_ptr = next_fltr_node_ptr;
  } /* while there is a fltr in the list */

  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

} /* ps_ifacei_rx_qos_fltr_delete() */

#endif /* FEATURE_DATA_PS_QOS */

