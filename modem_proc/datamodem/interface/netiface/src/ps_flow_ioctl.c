/*===========================================================================

                      P S _ F L O W _ I O C T L. C

DESCRIPTION
  Defines API to use ps_flow specific IOCTLs.

EXTERNALIZED FUNCTIONS
  PS_FLOW_IOCTL()
    Function which supports ps_flow specific IOCTLs

  PS_FLOW_DEFAULT_LOGICAL_FLOW_IOCTL_HDLR()
    Function which performs the default handling of ioctls for logical flows

INITIALIZING AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2005-14 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/netiface/src/ps_flow_ioctl.c#1 $
  $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
09/01/07    ssh    QoS support for logical ifaces
04/25/06    msr    L4/Tasklock code review changes
02/22/06    msr    Using single critical section
02/06/06    msr    Updated for L4 tasklock/crit sections.
08/16/05    msr    Fixed PS_BRANCH_TASKFREE()
08/12/05    msr    Updated messages
05/12/05    ks     Fixed Lint errors.
05/03/05    msr    TASKLOCKing ps_flow_ioctl()
04/17/05    msr    Created the file.
===========================================================================*/

/*===========================================================================

                       INCLUDE FILES FOR THE MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#include "ps_flow_ioctl.h"
#include "ps_flow.h"
#include "ps_flowi.h"
#include "msg.h"
#include "err.h"
#include "amssassert.h"
#include "dserrno.h"
#include "ps_qos_defs.h"
#include "dss_iface_ioctl.h"
#include "ps_system_heap.h"
#include "ds_Utils_DebugMsg.h"
#include "ps_ifacei.h"
#include <stringl/stringl.h>


/*===========================================================================

                               PUBLIC FUNCTIONS

===========================================================================*/
/*===========================================================================
FUNCTION PS_FLOW_IOCTL()

DESCRIPTION
  This function performs various operations on the given flow.
  Typically, these operations are to get or set a value.

PARAMETERS
  flow_ptr   : ps_flow on which the specified operation is to be performed
  ioctl_name : the operation name
  argval_ptr : pointer to operation specific structure
  ps_errno   : error code returned in case of failure (Error values are
               those defined in dserrno.h)

                 DS_EINVAL - Returned when the specified IOCTL does not
                 belong to the common set of IOCTLs and there is no IOCTL
                 mode handler registered for the specified interface.

                 DS_EOPNOTSUPP - Returned by the lower level IOCTL mode
                 handler when specified IOCTL is not supported by the
                 interface. For instance, this would be returned by
                 interfaces that do not support a certain
                 "ps_flow specific common IOCTL" (i.e. these are common
                 IOCTLs, but the implementation is mode specific)

                 DS_EFAULT - This error code is returned if the specified
                 arguments for the IOCTL are correct but an error is
                 encountered while executing the IOCTL.

                 DS_NOMEMORY - This error code is returned if we run out of
                 mempory buffers during execution.

RETURN VALUE
   0 : on success
  -1 : on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None.
===========================================================================*/
int ps_flow_ioctl
(
  ps_flow_type         * flow_ptr,
  ps_flow_ioctl_type     ioctl_name,
  void                 * argval_ptr,
  int16                * ps_errno
)
{
  dss_iface_ioctl_primary_qos_get_granted_flow_spec_type * get_flow_ptr = NULL;
  ps_flow_ioctl_qos_get_granted_filter_spec_type         * get_filter_ptr = NULL;
  boolean                result = FALSE;
  int                    num_rx_filters = 0;
  int                    num_tx_filters = 0;
  ip_flow_type         * temp_flow_ptr = NULL;
  ps_flow_type         * assoc_flow_ptr;
  int                    ret_val;
  int                    filter_index;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO2_2("ps_flow_ioctl(): "
                  "PS FLOW IOCTL 0x%x flow 0x%p", ioctl_name, flow_ptr);

  if (ps_errno == NULL)
  {
    DATA_ERR_FATAL("ps_flow_ioctl(): NULL parameter is passed");
    return -1;
  }

  if (!PS_FLOW_IS_VALID(flow_ptr))
  {
    LOG_MSG_ERROR_2("ps_flow_ioctl(): "
                    "PS FLOW IOCTL, 0x%x failed, invalid ps_flow, 0x%p",
                    ioctl_name, flow_ptr);
    *ps_errno = DS_EBADF;
    return -1;
  }

  switch (ioctl_name)
  {
    case PS_FLOW_IOCTL_QOS_GET_STATUS:
      *(ps_flow_state_enum_type*)argval_ptr = PS_FLOW_GET_STATE(flow_ptr);
      return 0;

    case PS_FLOW_IOCTL_QOS_GET_GRANTED_FLOW_SPEC:
      /* This IOCTL should be handled at the DSS layer itself. It is
         not supported anymore from PS Flow layer. */
      ASSERT (0);
      *ps_errno = DS_EINVAL;
      return -1;

    case PS_FLOW_IOCTL_QOS_GET_GRANTED_FLOW_SPEC2:

      /*-------------------------------------------------------------------
        Return granted flow spec if ps_flow is ACTIVATED, else return 0
        since no QOS is granted by the network in any other state
      -------------------------------------------------------------------*/
      get_flow_ptr =
        (dss_iface_ioctl_primary_qos_get_granted_flow_spec_type *)argval_ptr;

      temp_flow_ptr = PS_FLOWI_GET_RX_GRANTED_FLOW2(flow_ptr);
      if (NULL != temp_flow_ptr)
      {
        (void) memscpy (&get_flow_ptr->rx_ip_flow,
                        sizeof (ip_flow_type),
                       temp_flow_ptr,
                       sizeof (ip_flow_type));
      }
      else
      {
        memset (&get_flow_ptr->rx_ip_flow, 0, sizeof(ip_flow_type));
      }

      temp_flow_ptr = PS_FLOWI_GET_TX_GRANTED_FLOW2(flow_ptr);
      if (NULL != temp_flow_ptr)
      {
        (void) memscpy (&get_flow_ptr->tx_ip_flow,
                        sizeof (ip_flow_type),
                       temp_flow_ptr,
                       sizeof (ip_flow_type));
      }
      else
      {
        memset (&get_flow_ptr->tx_ip_flow, 0, sizeof(ip_flow_type));
      }

      return 0;
    case PS_FLOW_IOCTL_QOS_GET_GRANTED_FILTER_SPEC:
      get_filter_ptr = 
        (ps_flow_ioctl_qos_get_granted_filter_spec_type *) argval_ptr;
      
      get_filter_ptr->rx_filter_spec.list_ptr = NULL;
      
      get_filter_ptr->tx_filter_spec.list_ptr = NULL;

      num_rx_filters = 
        ps_flow_get_rx_fltr_cnt( flow_ptr, 
                                 IP_V4, 
                                 FALSE);
      num_rx_filters += 
        ps_flow_get_rx_fltr_cnt( flow_ptr, 
                                 IP_V6, 
                                 FALSE);
      if(0 != num_rx_filters)
      {
        PS_SYSTEM_HEAP_MEM_ALLOC(get_filter_ptr->rx_filter_spec.list_ptr ,
                                 num_rx_filters * sizeof( ip_filter_type),
                                 ip_filter_type*);
        if(NULL == get_filter_ptr->rx_filter_spec.list_ptr)
        {
          return -1;
        }

        /* pre-condition for the following function */
        get_filter_ptr->rx_filter_spec.num_filters = 0;

        result = ps_flow_get_rx_fltr_spec
          (
            flow_ptr->flow_private.iface_ptr,
            flow_ptr,
            &(get_filter_ptr->rx_filter_spec),
            FALSE,
            num_rx_filters,
            ps_errno
          );
        if(FALSE == result)
        {
          LOG_MSG_ERROR_2("ps_flow_ioctl(): "
                          "get filter spec failed, ps_flow 0x%p, result %d",
                          flow_ptr, result);
          PS_SYSTEM_HEAP_MEM_FREE(get_filter_ptr->rx_filter_spec.list_ptr);
          return -1;
        }
      }
      num_tx_filters = 
        ps_iface_flow_get_tx_fltr_cnt_ex( flow_ptr->flow_private.iface_ptr, 
                                      flow_ptr, 
                                      FALSE);
      if(0 != num_tx_filters)
      {
        PS_SYSTEM_HEAP_MEM_ALLOC(get_filter_ptr->tx_filter_spec.list_ptr,
                                 num_tx_filters * sizeof( ip_filter_type),
                                 ip_filter_type*);
        if(NULL == get_filter_ptr->tx_filter_spec.list_ptr)
        {
          PS_SYSTEM_HEAP_MEM_FREE(get_filter_ptr->rx_filter_spec.list_ptr);
          return -1;
        }

        /* pre-condition for the following function */
        get_filter_ptr->tx_filter_spec.num_filters = 0;
        
        result = ps_iface_flow_get_tx_fltr_spec
          (
            flow_ptr->flow_private.iface_ptr,
            flow_ptr,
            &(get_filter_ptr->tx_filter_spec),
            FALSE,
            num_tx_filters,
            ps_errno
          );
        if(FALSE == result)
        {
          LOG_MSG_ERROR_2("ps_flow_ioctl(): "
                          "get filter spec failed, ps_flow 0x%p, result %d",
                          flow_ptr, result);
          PS_SYSTEM_HEAP_MEM_FREE(get_filter_ptr->rx_filter_spec.list_ptr);
          PS_SYSTEM_HEAP_MEM_FREE(get_filter_ptr->tx_filter_spec.list_ptr);
          return -1;
        }
      }

      LOG_MSG_INFO1_1("PS_FLOW_IOCTL_QOS_GET_GRANTED_FILTER_SPEC: "
                      "returning %d RX filters",
                      get_filter_ptr->rx_filter_spec.num_filters);
      for (filter_index = 0;
           filter_index < get_filter_ptr->rx_filter_spec.num_filters;
           filter_index++)
      {
        ps_iface_log_qos_spec_filter_params (
          &get_filter_ptr->rx_filter_spec.list_ptr[filter_index]);
      }

      LOG_MSG_INFO1_1("PS_FLOW_IOCTL_QOS_GET_GRANTED_FILTER_SPEC: "
                      "returning %d TX filters",
                      get_filter_ptr->tx_filter_spec.num_filters);
      for (filter_index = 0;
           filter_index < get_filter_ptr->tx_filter_spec.num_filters;
           filter_index++)
      {
        ps_iface_log_qos_spec_filter_params (
          &get_filter_ptr->tx_filter_spec.list_ptr[filter_index]);
      }
      
      return 0;
    default:
      /* No-op */
      break;

  } /* Common IOCTL switch */

  assoc_flow_ptr = flow_ptr;
  while (PS_FLOW_IS_VALID(flow_ptr))
  {
    assoc_flow_ptr = flow_ptr;
    if (assoc_flow_ptr->ps_flow_ioctl_f_ptr != NULL)
    {
      break;
    }

    flow_ptr = PS_FLOWI_GET_ASSOC_PS_FLOW(assoc_flow_ptr);
  }

  if (assoc_flow_ptr->ps_flow_ioctl_f_ptr != NULL)
  {
    LOG_MSG_INFO2_2("ps_flow_ioctl(): "
                    "PS FLOW IOCTL 0x%x calling MH callback on flow 0x%p",
                    ioctl_name, assoc_flow_ptr);
    ret_val = assoc_flow_ptr->ps_flow_ioctl_f_ptr(assoc_flow_ptr,
                                                  ioctl_name,
                                                  argval_ptr,
                                                  ps_errno);
  }
  else
  {
    LOG_MSG_INFO1_2("ps_flow_ioctl(): "
                    "PS FLOW IOCTL 0x%x is not supported on ps_flow 0x%p",
                    ioctl_name, assoc_flow_ptr);
    *ps_errno = DS_EINVAL;
    ret_val = -1;
  }

  return ret_val;

} /* ps_flow_ioctl() */



/*===========================================================================
FUNCTION PS_FLOW_DEFAULT_LOGICAL_FLOW_IOCTL_HDLR()

DESCRIPTION
  This function performs the default handling of ioctls for logical flows.

PARAMETERS
  flow_ptr   : ps_flow on which the specified operation is to be performed
  ioctl_name : the operation name
  argval_ptr : pointer to operation specific structure
  ps_errno   : error code returned in case of failure (Error values are
               those defined in dserrno.h)

                 DS_EINVAL - Returned when the specified IOCTL does not
                 belong to the common set of IOCTLs and there is no IOCTL
                 mode handler registered for the specified interface.

                 DS_EOPNOTSUPP - Returned by the lower level IOCTL mode
                 handler when specified IOCTL is not supported by the
                 interface. For instance, this would be returned by
                 interfaces that do not support a certain
                 "ps_flow specific common IOCTL" (i.e. these are common
                 IOCTLs, but the implementation is mode specific)

                 DS_EFAULT - This error code is returned if the specified
                 arguments for the IOCTL are correct but an error is
                 encountered while executing the IOCTL.

                 DS_NOMEMORY - This error code is returned if we run out of
                 mempory buffers during execution.

RETURN VALUE
   0 : on success
  -1 : on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None.
===========================================================================*/
int ps_flow_default_logical_flow_ioctl_hdlr
(
  ps_flow_type         * flow_ptr,
  ps_flow_ioctl_type     ioctl_name,
  void                 * argval_ptr,
  int16                * ps_errno
)
{
#ifdef FEATURE_DATA_PS_QOS
  ps_flow_type                       ** assoc_flow_ptr_arr;
  ps_flow_type                        * assoc_flow_ptr;
  ps_flow_ioctl_qos_suspend_ex_type   * qos_ioctl_arg_suspend_ex_ptr;
  ps_flow_ioctl_qos_resume_ex_type    * qos_ioctl_arg_resume_ex_ptr;
  ps_flow_ioctl_qos_release_ex_type   * qos_ioctl_arg_release_ex_ptr;
  ps_flow_ioctl_qos_suspend_ex_type     assoc_qos_ioctl_arg_suspend_ex;
  ps_flow_ioctl_qos_resume_ex_type      assoc_qos_ioctl_arg_resume_ex;
  ps_flow_ioctl_qos_release_ex_type     assoc_qos_ioctl_arg_release_ex;
  int                                   retval = 0;
  unsigned long                         num_bytes_to_alloc;
  uint8                                 idx;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (ps_errno == NULL)
  {
    DATA_ERR_FATAL("ps_flow_default_logical_flow_ioctl_hdlr(): NULL errno");
    return -1;
  }

  if (!PS_FLOW_IS_VALID(flow_ptr))
  {
    LOG_MSG_ERROR_2("ps_flow_default_logical_flow_ioctl_hdlr(): "
                    "PS FLOW IOCTL, 0x%x failed, invalid ps_flow, 0x%p",
                    ioctl_name, flow_ptr);
    *ps_errno = DS_EBADF;
    return -1;
  }

  assoc_flow_ptr = PS_FLOW_GET_ASSOC_PS_FLOW(flow_ptr);
  if (!PS_FLOW_IS_VALID(assoc_flow_ptr))
  {
    LOG_MSG_ERROR_0("ps_flow_default_logical_flow_ioctl_hdlr(): "
                    "Invalid associated flow ptr");
    *ps_errno = DS_EINVAL;
    return -1;
  }

  switch( ioctl_name )
  {
    case PS_FLOW_IOCTL_QOS_SUSPEND:
      retval = ps_flow_ioctl(assoc_flow_ptr, ioctl_name, argval_ptr, ps_errno);
      if (retval == 0 ||
         (retval == -1 && *ps_errno == DS_EWOULDBLOCK))
      {
        (void) ps_flow_suspend_cmd(flow_ptr, ps_errno, NULL);
      }
      break;

    case PS_FLOW_IOCTL_QOS_RESUME:
      retval = ps_flow_ioctl(assoc_flow_ptr, ioctl_name, argval_ptr, ps_errno);
      if (retval == 0 ||
         (retval == -1 && *ps_errno == DS_EWOULDBLOCK))
      {
        (void) ps_flow_resume_cmd(flow_ptr, ps_errno, NULL);
      }
      break;

    case PS_FLOW_IOCTL_QOS_RELEASE:
      (void) ps_flow_go_null_cmd(flow_ptr, ps_errno, NULL);
      retval = ps_flow_ioctl(assoc_flow_ptr, ioctl_name, argval_ptr, ps_errno);
      break;

    case PS_FLOW_IOCTL_QOS_SUSPEND_EX:
      qos_ioctl_arg_suspend_ex_ptr =
        (ps_flow_ioctl_qos_suspend_ex_type *) argval_ptr;

      num_bytes_to_alloc =
        sizeof(ps_flow_type *) * qos_ioctl_arg_suspend_ex_ptr->num_flows;
      PS_SYSTEM_HEAP_MEM_ALLOC(assoc_flow_ptr_arr, num_bytes_to_alloc,
                               ps_flow_type**);
      if (assoc_flow_ptr_arr == NULL)
      {
        *ps_errno = DS_ENOMEM;
        break;
      }

      for (idx = 0; idx < qos_ioctl_arg_suspend_ex_ptr->num_flows; idx++)
      {
        assoc_flow_ptr_arr[idx] =
          PS_FLOWI_GET_ASSOC_PS_FLOW(
            qos_ioctl_arg_suspend_ex_ptr->flows_ptr[idx]);
      }

      assoc_qos_ioctl_arg_suspend_ex.num_flows =
        qos_ioctl_arg_suspend_ex_ptr->num_flows;
      assoc_qos_ioctl_arg_suspend_ex.flows_ptr = assoc_flow_ptr_arr;

      retval = ps_flow_ioctl(assoc_flow_ptr,
                             ioctl_name,
                             &assoc_qos_ioctl_arg_suspend_ex,
                             ps_errno);
      if (retval == 0 ||
         (retval == -1 && *ps_errno == DS_EWOULDBLOCK))
      {

        for (idx = 0; idx < qos_ioctl_arg_suspend_ex_ptr->num_flows; idx++)
        {
          (void) ps_flow_suspend_cmd(qos_ioctl_arg_suspend_ex_ptr->flows_ptr[idx],
                                     ps_errno,
                                     NULL);
        }
      }

      PS_SYSTEM_HEAP_MEM_FREE(assoc_flow_ptr_arr);
      break;

    case PS_FLOW_IOCTL_QOS_RESUME_EX:
      qos_ioctl_arg_resume_ex_ptr =
        (ps_flow_ioctl_qos_resume_ex_type *) argval_ptr;

      num_bytes_to_alloc =
        sizeof(ps_flow_type *) * qos_ioctl_arg_resume_ex_ptr->num_flows;

      PS_SYSTEM_HEAP_MEM_ALLOC(assoc_flow_ptr_arr, num_bytes_to_alloc, 
                               ps_flow_type**);
      if (assoc_flow_ptr_arr == NULL)
      {
        *ps_errno = DS_ENOMEM;
        break;
      }

      for (idx = 0; idx < qos_ioctl_arg_resume_ex_ptr->num_flows; idx++)
      {
        assoc_flow_ptr_arr[idx] =
          PS_FLOWI_GET_ASSOC_PS_FLOW(
            qos_ioctl_arg_resume_ex_ptr->flows_ptr[idx]);
      }

      assoc_qos_ioctl_arg_resume_ex.num_flows =
        qos_ioctl_arg_resume_ex_ptr->num_flows;
      assoc_qos_ioctl_arg_resume_ex.flows_ptr = assoc_flow_ptr_arr;

      retval = ps_flow_ioctl(assoc_flow_ptr,
                             ioctl_name,
                             &assoc_qos_ioctl_arg_resume_ex,
                             ps_errno);

      if (retval == 0 ||
         (retval == -1 && *ps_errno == DS_EWOULDBLOCK))
      {

        for (idx = 0; idx < qos_ioctl_arg_resume_ex_ptr->num_flows; idx++)
        {
          (void) ps_flow_resume_cmd(qos_ioctl_arg_resume_ex_ptr->flows_ptr[idx],
                                    ps_errno,
                                    NULL);
        }
      }

      PS_SYSTEM_HEAP_MEM_FREE(assoc_flow_ptr_arr);
      break;

    case PS_FLOW_IOCTL_QOS_RELEASE_EX:
      qos_ioctl_arg_release_ex_ptr =
        (ps_flow_ioctl_qos_release_ex_type *) argval_ptr;

      num_bytes_to_alloc =
        sizeof(ps_flow_type *) * qos_ioctl_arg_release_ex_ptr->num_flows;
      PS_SYSTEM_HEAP_MEM_ALLOC(assoc_flow_ptr_arr, num_bytes_to_alloc, 
                               ps_flow_type**);

      if (assoc_flow_ptr_arr == NULL)
      {
        *ps_errno = DS_ENOMEM;
        break;
      }

      for (idx = 0; idx < qos_ioctl_arg_release_ex_ptr->num_flows; idx++)
      {
        (void) ps_flow_go_null_cmd(qos_ioctl_arg_release_ex_ptr->flows_ptr[idx],
                                   ps_errno,
                                   NULL);

        assoc_flow_ptr_arr[idx] =
          PS_FLOWI_GET_ASSOC_PS_FLOW(
            qos_ioctl_arg_release_ex_ptr->flows_ptr[idx]);
      }

      assoc_qos_ioctl_arg_release_ex.num_flows =
        qos_ioctl_arg_release_ex_ptr->num_flows;
      assoc_qos_ioctl_arg_release_ex.flows_ptr = assoc_flow_ptr_arr;

      retval = ps_flow_ioctl(assoc_flow_ptr,
                             ioctl_name,
                             &assoc_qos_ioctl_arg_release_ex,
                             ps_errno);

      PS_SYSTEM_HEAP_MEM_FREE(assoc_flow_ptr_arr);
      break;

    case PS_FLOW_IOCTL_GET_TX_QUEUE_LEVEL:
    case PS_FLOW_IOCTL_PRIMARY_QOS_MODIFY:
    case PS_FLOW_IOCTL_PRIMARY_QOS_GET_GRANTED_FLOW_SPEC:
    case PS_FLOW_IOCTL_HDR_GET_RMAC3_INFO:
    case PS_FLOW_IOCTL_707_GET_TX_STATUS:
    case PS_FLOW_IOCTL_707_GET_INACTIVITY_TIMER:
    case PS_FLOW_IOCTL_707_SET_INACTIVITY_TIMER:
    case PS_FLOW_IOCTL_GET_MODIFY_RESULT:
    case PS_FLOW_IOCTL_GET_FLOW_UPDATED_INFO_CODE:
    case PS_FLOW_IOCTL_GET_PRIMARY_QOS_MODIFY_RESULT:
      retval = ps_flow_ioctl(assoc_flow_ptr, ioctl_name, argval_ptr, ps_errno);
      break;

    default:
      LOG_MSG_ERROR_1("ps_flow_default_logical_flow_ioctl_hdlr(): "
                      "Unsupported ioctl 0x%x", ioctl_name );
      *ps_errno = DS_EOPNOTSUPP;
      retval = -1;
      break;
  } /* switch( ioctl_name ) */

  return retval;
#else
  return -1;
#endif /* FEATURE_DATA_PS_QOS */

} /* ps_flow_default_logical_flow_ioctl_hdlr() */
