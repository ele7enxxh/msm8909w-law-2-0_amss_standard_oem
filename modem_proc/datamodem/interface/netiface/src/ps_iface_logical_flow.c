/*===========================================================================
  FILE: ps_iface_logical_flow.c

  OVERVIEW: TODO

  DEPENDENCIES: None

  Copyright (c) 2010 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/netiface/src/ps_iface_logical_flow.c#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2010-05-11 msr Created file

===========================================================================*/

/*===========================================================================

                                INCLUDE FILES

===========================================================================*/
#include "customer.h"
#include "comdef.h"

#include "ps_iface_logical_flow.h"

#include "dserrno.h"
#include "ds_Utils_DebugMsg.h"
#include "ps_tx_meta_info.h"
#include "ps_crit_sect.h"
#include "ps_iface_utils.h"
#include "ps_metai_info.h"
#include "ps_flowi.h"
/*===========================================================================

                             EXTERNAL FUNCTIONS

===========================================================================*/
int ps_iface_create_logical_flows
(
  ps_iface_type                    * iface_ptr,
  ps_iface_ioctl_type                ioctl_name,
  const ps_flow_create_param_type  * qos_req_param,
  void                             * ioctl_argval_ptr,
  ps_flow_type                    ** out_flow_ptr_ptr,
  int16                            * ps_errno
)
{
  int  (*ps_flow_cmd_to_call)(ps_flow_type *, int16 *, void *);

  ps_flow_type                        * assoc_flow_ptr = NULL;
  ps_flow_type                        * default_flow_ptr = NULL;
  ps_iface_type                       * assoc_iface_ptr;
  ps_iface_ioctl_qos_request_ex_type  * qos_request_ex_info_ptr = NULL;
  ps_iface_ioctl_qos_request_type     * qos_request_info_ptr = NULL;
  ps_tx_meta_info_type                * default_flow_meta_info_ptr = NULL;
  ps_tx_meta_info_type                * logical_flow_meta_info_ptr = NULL;
  int32                                 ret_val;
  int16                                 tmp_errno;
  int8                                  cnt = 0;
  uint8                                 num_qos_flows;

  ps_iface_ioctl_qos_request_ex_opcode_enum_type  qos_opcode;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (ps_errno == NULL)
  {
    DATA_ERR_FATAL("ps_iface_create_logical_flows(): NULL errno");
    return -1;
  }

  if (ioctl_argval_ptr == NULL || out_flow_ptr_ptr == NULL)
  {
    DATA_ERR_FATAL("ps_iface_create_logical_flows(): NULL params");
    *ps_errno = DS_EFAULT;
    return -1;
  }

  /*-------------------------------------------------------------------------
    Validate QoS spec
  -------------------------------------------------------------------------*/
  switch (ioctl_name)
  {
    case PS_IFACE_IOCTL_QOS_REQUEST:
    {
      qos_request_info_ptr =
        (ps_iface_ioctl_qos_request_type *) (ioctl_argval_ptr);

      if (qos_request_info_ptr->qos_ptr == NULL)
      {
        DATA_ERR_FATAL("ps_iface_create_logical_flows(): NULL QoS spec");
        *ps_errno = DS_EFAULT;
        return -1;
      }

      num_qos_flows = 1;
      qos_opcode    = PS_IFACE_IOCTL_QOS_REQUEST_OP;
      break;
    }

    case PS_IFACE_IOCTL_QOS_REQUEST_EX:
    {
      qos_request_ex_info_ptr =
        (ps_iface_ioctl_qos_request_ex_type *) (ioctl_argval_ptr);

      if (qos_request_ex_info_ptr->qos_specs_ptr == NULL)
      {
        DATA_ERR_FATAL("ps_iface_create_logical_flows(): NULL QoS spec");
        *ps_errno = DS_EFAULT;
        return -1;
      }

      if (qos_request_ex_info_ptr->num_qos_specs >
            MAX_QOS_OPERATIONS_PER_IOCTL)
      {
        LOG_MSG_ERROR_2("ps_iface_create_logical_flows(): "
                        "Too many requests per IOCTL %d, MAX=%d",
                        qos_request_ex_info_ptr->num_qos_specs,
                        MAX_QOS_OPERATIONS_PER_IOCTL);
        ASSERT(0);
        *ps_errno = DS_EFAULT;
        return -1;
      }

      num_qos_flows = qos_request_ex_info_ptr->num_qos_specs;
      qos_opcode    = qos_request_ex_info_ptr->opcode;
      break;
    }

    default:
    {
      LOG_MSG_ERROR_1("ps_iface_create_logical_flows(): "
                      "Invalid ioctl 0x%x", ioctl_name);
      ASSERT(0);
      *ps_errno = DS_EFAULT;
      return -1;
    }
  } /* switch (ioctl_name) */

  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  if (!PS_IFACE_IS_VALID(iface_ptr))
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_1("ps_iface_create_logical_flows(): "
                    "Invalid iface 0x%p", iface_ptr);
    ASSERT(0);
    *ps_errno = DS_EFAULT;
    return -1;
  }

  if (!PS_IFACEI_IS_LOGICAL(iface_ptr))
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_2("ps_iface_create_logical_flows(): "
                    "Iface 0x%x:%d not logical",
                    iface_ptr->name, iface_ptr->instance);
    ASSERT(0);
    *ps_errno = DS_EFAULT;
    return -1;
  }

  assoc_iface_ptr = PS_IFACEI_GET_ASSOC_IFACE(iface_ptr);
  if (!PS_IFACE_IS_VALID(assoc_iface_ptr))
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_1("ps_iface_create_logical_flows(): "
                    "Invalid assoc iface 0x%p", assoc_iface_ptr);
    ASSERT(0);
    *ps_errno = DS_EFAULT;
    return -1;
  }

  default_flow_ptr = PS_IFACEI_GET_DEFAULT_FLOW(iface_ptr);
  default_flow_meta_info_ptr =
    PS_FLOW_GET_META_INFO_FROM_FLOW(default_flow_ptr);

  if (qos_opcode == PS_IFACE_IOCTL_QOS_REQUEST_OP)
  {
    ps_flow_cmd_to_call = ps_flow_activate_cmd;
  }
  else
  {
    ps_flow_cmd_to_call = ps_flow_configure_cmd;
  }

  /*-------------------------------------------------------------------------
    Create the requested number of logical flows (L_flow) and invoke
    either ps_flow_activate_cmd() or ps_flow_configure_cmd() on each
  -------------------------------------------------------------------------*/
  for (cnt = 0; cnt < num_qos_flows; cnt++)
  {
    ret_val = ps_iface_create_flow(iface_ptr,
                                   NULL,
                                   &qos_req_param[cnt],
                                   &out_flow_ptr_ptr[cnt],
                                   ps_errno);
    if (ret_val != 0)
    {
      LOG_MSG_ERROR_1 ("ps_iface_create_logical_flows(): "
                       "Couldn't create ps_flow, err %d", *ps_errno );
      break;
    }

    ret_val = ps_flow_cmd_to_call(out_flow_ptr_ptr[cnt], ps_errno, NULL);
    if (0 != ret_val && DS_EWOULDBLOCK != *ps_errno)
    {
      LOG_MSG_ERROR_2 ("ps_iface_create_logical_flows(): "
                       "Couldn't activate/configure ps_flow 0x%x, err %d",
                       out_flow_ptr_ptr[cnt], *ps_errno );

      ps_flow_go_null_ind(out_flow_ptr_ptr[cnt], PS_EIC_NOT_SPECIFIED );
      (void) ps_iface_delete_flow(iface_ptr, out_flow_ptr_ptr[cnt], &tmp_errno);
      break;
    }

    /*-----------------------------------------------------------------------
      Allocate meta-info for flow and copy default flow's meta-info into it
    -----------------------------------------------------------------------*/
    if (default_flow_meta_info_ptr != NULL)
    {
      PS_TX_META_INFO_AND_RT_META_INFO_GET(logical_flow_meta_info_ptr);

      if (logical_flow_meta_info_ptr == NULL ||
          PS_TX_META_GET_RT_META_INFO_PTR(logical_flow_meta_info_ptr) == NULL)
      {
        LOG_MSG_ERROR_0("ps_iface_create_logical_flows(): "
                        "Couldn't alloc metainfo");

        ps_flow_go_null_ind(out_flow_ptr_ptr[cnt], PS_EIC_NOT_SPECIFIED );
        (void) ps_iface_delete_flow(iface_ptr,
                                    out_flow_ptr_ptr[cnt],
                                    &tmp_errno);

        break;
      }

      PS_TX_META_INFO_COPY(default_flow_meta_info_ptr,
                           logical_flow_meta_info_ptr);
    }

    out_flow_ptr_ptr[cnt]->flow_private.logical_flow.mi_ptr =
      logical_flow_meta_info_ptr;
  } /* for (num_qos_flows) */

  /*-------------------------------------------------------------------------
    Clean up all the flows if num_qos_flows couldn't be created
  -------------------------------------------------------------------------*/
  if (cnt != num_qos_flows)
  {
    for (--cnt; cnt >= 0; cnt--)
    {
      ps_flow_go_null_ind(out_flow_ptr_ptr[cnt], PS_EIC_NOT_SPECIFIED );
      (void) ps_iface_delete_flow(iface_ptr, out_flow_ptr_ptr[cnt], &tmp_errno);
    }

    /* ps_errno will be filled by the call which failed */
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    return -1;
  } /* for all flows not created and activated/configured */

  /*-------------------------------------------------------------------------
    Invoke corresponding ioctl on underlying iface
  -------------------------------------------------------------------------*/
  ret_val =
    ps_iface_ioctl(assoc_iface_ptr, ioctl_name, ioctl_argval_ptr, ps_errno );
  if (ret_val == DSS_ERROR && *ps_errno != DS_EWOULDBLOCK)
  {
    LOG_MSG_ERROR_2("ps_iface_create_logical_flows(): "
                    "IOCTL failed, type:%d errno: %d", ioctl_name, *ps_errno);
    for (cnt = 0; cnt < num_qos_flows; cnt++)
    {
      ps_flow_go_null_ind(out_flow_ptr_ptr[cnt], PS_EIC_NOT_SPECIFIED);
      (void) ps_iface_delete_flow(iface_ptr, out_flow_ptr_ptr[cnt], &tmp_errno);
    }

    /* ps_errno will be filled by the ps_iface_ioctl() */
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    return -1;
  }

  /*-------------------------------------------------------------------------
    Associate the logical flows to the newly created flows
  -------------------------------------------------------------------------*/
  if (ioctl_name == PS_IFACE_IOCTL_QOS_REQUEST)
  {
    assoc_flow_ptr = qos_request_info_ptr->flow_ptr;
    ret_val = ps_flow_set_assoc_flow(*out_flow_ptr_ptr,
                                     assoc_flow_ptr,
                                     NULL,
                                     ps_errno);
    if (ret_val == DSS_ERROR)
    {
      LOG_MSG_ERROR_1("ps_iface_create_logical_flows(): "
                      "ps_flow_set_assoc_flow failed, errno: %d", *ps_errno);
      ps_flow_go_null_ind(*out_flow_ptr_ptr, PS_EIC_NOT_SPECIFIED);
      (void) ps_iface_delete_flow(iface_ptr, *out_flow_ptr_ptr, &tmp_errno);
      PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
      return -1;
    }

    qos_request_info_ptr->flow_ptr = *out_flow_ptr_ptr;
  }
  else
  {
    for (cnt = 0; cnt < num_qos_flows; cnt++)
    {
      /*---------------------------------------------------------------------
        Set the associated flow ptr and register for events
      ---------------------------------------------------------------------*/
      assoc_flow_ptr = qos_request_ex_info_ptr->flows_ptr[cnt];
      ret_val = ps_flow_set_assoc_flow(out_flow_ptr_ptr[cnt],
                                       assoc_flow_ptr,
                                       NULL,
                                       ps_errno );

      if (ret_val == DSS_ERROR)
      {
        LOG_MSG_ERROR_1("ps_iface_create_logical_flows(): "
                        "ps_flow_set_assoc_flow failed, errno: %d", *ps_errno);
        for (cnt = 0; cnt < num_qos_flows; cnt++)
        {
          ps_flow_go_null_ind(out_flow_ptr_ptr[cnt], PS_EIC_NOT_SPECIFIED );
          (void) ps_iface_delete_flow(iface_ptr, out_flow_ptr_ptr[cnt], &tmp_errno);
        }

        PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
        return -1;
      }

      qos_request_ex_info_ptr->flows_ptr[cnt] = out_flow_ptr_ptr[cnt];
    }
  }

  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
  return 0;
} /* ps_iface_create_logical_flows() */


int ps_iface_modify_logical_flows
(
  ps_iface_type                    * iface_ptr,
  ps_flow_type                     * modify_flow_ptr,
  ps_flow_ioctl_type                 ioctl_name,
  const ps_flow_modify_param_type  * qos_modify_param,
  void                             * ioctl_argval_ptr,
  int16                            * ps_errno
)
{
  ps_flow_type   * assoc_flow_ptr = NULL;
  ps_iface_type  * assoc_iface_ptr;
  int32            ret_val;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (ps_errno == NULL)
  {
    DATA_ERR_FATAL("ps_iface_modify_logical_flows(): NULL errno");
    return -1;
  }

  if (ioctl_argval_ptr == NULL || modify_flow_ptr == NULL)
  {
    DATA_ERR_FATAL("ps_iface_modify_logical_flows(): NULL params");
    *ps_errno = DS_EFAULT;
    return -1;
  }

  /*-------------------------------------------------------------------------
    Validate QoS spec
  -------------------------------------------------------------------------*/
  if (ioctl_name != PS_FLOW_IOCTL_QOS_MODIFY)
  {
    LOG_MSG_ERROR_1("ps_iface_modify_logical_flows(): "
                    "Invalid ioctl 0x%x", ioctl_name);
    ASSERT(0);
    *ps_errno = DS_EFAULT;
    return -1;
  }

  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  if (!PS_IFACE_IS_VALID(iface_ptr))
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_1("ps_iface_modify_logical_flows(): "
                    "Invalid iface 0x%p", iface_ptr);
    ASSERT(0);
    *ps_errno = DS_EFAULT;
    return -1;
  }

  if (!PS_IFACEI_IS_LOGICAL(iface_ptr))
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_2("ps_iface_modify_logical_flows(): "
                    "Iface 0x%x:%d not logical",
                    iface_ptr->name, iface_ptr->instance);
    ASSERT(0);
    *ps_errno = DS_EFAULT;
    return -1;
  }

  assoc_iface_ptr = PS_IFACEI_GET_ASSOC_IFACE(iface_ptr);
  if (!PS_IFACE_IS_VALID(assoc_iface_ptr))
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_1("ps_iface_modify_logical_flows(): "
                    "Invalid assoc iface 0x%p", assoc_iface_ptr);
    ASSERT(0);
    *ps_errno = DS_EFAULT;
    return -1;
  }

  /*-------------------------------------------------------------------------
    Modify the logical flow (L_flow)
  -------------------------------------------------------------------------*/
  ret_val = ps_iface_modify_flow(iface_ptr,
                                 modify_flow_ptr,
                                 qos_modify_param,
                                 ps_errno);
  if (ret_val != 0)
  {
    LOG_MSG_ERROR_2 ("ps_iface_modify_logical_flows(): "
                     "Couldn't modify ps_flow 0x%x, err %d",
                     modify_flow_ptr, *ps_errno);
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    return -1;
  }

  /*-------------------------------------------------------------------------
    Invoke corresponding ioctl on underlying flow
  -------------------------------------------------------------------------*/
  assoc_flow_ptr = PS_FLOWI_GET_ASSOC_PS_FLOW(modify_flow_ptr);
  ret_val =
    ps_flow_ioctl(assoc_flow_ptr, ioctl_name, ioctl_argval_ptr, ps_errno );

  if (ret_val == DSS_ERROR && *ps_errno != DS_EWOULDBLOCK)
  {

   /*-------------------------------------------------------------------------
     Remove the modifications made to the logical flow
   -------------------------------------------------------------------------*/
    ps_iface_modify_flow_rejected(iface_ptr, 
                                  modify_flow_ptr, 
                                  PS_EIC_QOS_INTERNAL_ERROR, 
                                  ps_errno);

    /* ps_errno will be filled by the ps_iface_ioctl() */
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    return -1;
  }

  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
  return 0;
} /* ps_iface_modify_logical_flows() */

