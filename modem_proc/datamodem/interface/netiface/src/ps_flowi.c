/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                             P S _ F L O W I . C

DESCRIPTION
  File defining internal functions used by PS to create and manipulate a
  flow.

EXTERNAL FUNCTIONS
  PS_FLOWI_CREATE()
    Creates a flow

  PS_FLOWI_DELETE()
    Deletes a flow

  PS_FLOWI_BIND_PHYS_LINK()
    Binds a phys link to a flow

  PS_FLOWI_UNBIND_PHYS_LINK()
    Unbinds a phys link from a flow

  PS_FLOWI_REBIND_PHYS_LINK()
    Binds a phys link to a flow different from its current binding

  PS_FLOWI_VALIDATE_QOS_SPEC()
    Validates a given QOS spec

  PS_FLOWI_STORE_QOS_SPEC()
    Stores a QOS spec in a ps_flow

  PS_FLOWI_DELETE_QOS_SPEC()
    Delete QOS spec from a ps_flow

  PS_FLOWI_VALIDATE_QOS_MODIFY_SPEC()
    Validates a given QOS spec, specified as part of QOS_MODIFY operation

  PS_FLOWI_STORE_QOS_MODIFY_SPEC()
    Stores a QOS spec in a ps_flow, specified as part of QOS_MODIFY operation

  PS_FLOWI_DELETE_QOS_MODIFY_SPEC()
    Delete QOS spec from a ps_flow, specified as part of QOS_MODIFY operation

  PS_FLOWI_VALIDATE_PRIMARY_QOS_MODIFY_SPEC()
    Validates primary QoS spec

  PS_FLOWI_DELETE_AUX_FLOW_SPEC()
    Deletes auxiliary flow specs from a qos_info_ptr

  PS_FLOWI_DELETE_RX_FLTR_SPEC()
    Deletes Rx fltr spec from a qos_info_ptrps_flowi_get_rx_fltr_handle_ex

  PS_FLOWI_GET_QOS_INFO_PTR()
    Given a flow_ptr returns qos_info_ptr where all QoS related info is
    stored pertaining to active QoS or pending QOS_MODIFY

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c)2005-2014 by Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/netiface/src/ps_flowi.c#1 $
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $
when        who    what, where, why
--------    ---    ----------------------------------------------------------
02/05/10    ss     Destroy the queue using q_destroy () when it is no longer
                   needed to free up queue mutex.
09/01/07    ssh    QoS support for logical ifaces
6/28/07    msr/ssh QoS supportfor M.IP CCoA iface (currently featurized)
03/16/07    scb    Fixed Klocwork High errors
02/07/07    scb    Fixed Klocworks high errors
11/16/06    msr    Added DSS_IFACE_IOCTL_QOS_INFO_CODE_UPDATED_EV
08/16/06    msr    Added extra check for NOMINAL_SDU_SIZE
08/14/06    msr    Extended WLAN QoS support
07/31/06    msr    Added support for PRIMARY_QOS_MODIFY
04/25/06    msr    L4/Tasklock code review changes
04/13/06    msr    Making sure that filters are not modified in a direction
                   when QoS is deleted in that direction
02/22/06    msr    Using single critical section
02/15/06    msr    Removed qos_applied flag
02/14/06    msr    Added support for QoS over WLAN
02/06/06    msr    Updated for L4 tasklock/crit sections.
01/09/06    msr    Conforming to new QOS_MODIFY API
12/05/05    msr    Conforming to changes made to support fltr validation in
                   filtering library
10/17/05    msr    Handling the case where app specifies list_ptr as NULL and
                   num_fltrs as non-zero. Similarly for auxiliary flows also
08/29/05    msr    Moved ps_flowi_get_aux_flow_spec_handle() and
                   ps_flowi_get_aux_flow_spec_by_handle() to ps_flow.c.
                   Removed support for ICMP in QoS filters
08/16/05    msr    Fixed PS_BRANCH_TASKFREE().
                   Added ps_flowi_validate_aux_flow_spec()
08/15/05    msr    Added ps_flowi_get_aux_flow_spec_handle() and
                   ps_flowi_get_aux_flow_spec_by_handle
08/10/05    msr    Added support for QOS_MODIFY
06/10/05    mct    Added support for profile ids and multiple flow specs.
05/12/05    ks     Fixed Lint errors.
05/03/05    ks     Corrected return value in ps_flowi_unbind_phys_link().
05/03/05    msr    Checking if flow is bound to a phys link in
                   ps_flowi_unbind_phys_link()
05/03/05    msr    Updating errmask correctly in all cases when flow or fltr
                   validation fails
04/18/05    msr    Memsetting ps_flow properly in ps_flowi_delete().
04/17/05    msr    Created the file.

===========================================================================*/
/*===========================================================================

                                INCLUDE FILES

===========================================================================*/
#include "datamodem_variation.h"
#include "customer.h"
#include "comdef.h"

#include "amssassert.h"
#include "msg.h"

#include "ps_flow.h"
#include "ps_flowi.h"
#include "ps_phys_link.h"
#include "ps_iface_defs.h"
#include "ps_iface_ipfltr.h"
#include "ps_mem.h"
#include "ps_flowi_event.h"
#include "ds_flow_control.h"
#include "ds_Utils_DebugMsg.h"



/*===========================================================================

                             INTERNAL FUNCTIONS

===========================================================================*/
#ifdef FEATURE_DATA_PS_QOS
/*===========================================================================
FUNCTION PS_FLOWI_VALIDATE_QOS_FLOW_PARAM()

DESCRIPTION
  this function is used to validate the parameters of a QOS flow

PARAMETERS
  ip_flow_ptr : ptr to the requested QOS flow
  flow_type   : enum stating Rx or Tx flow type

RETURN VALUE
  TRUE  : if parameters are valid
  FALSE : otherwise

DEPENDENCIES
  None

SIDE EFFECTS
  err_mask is updated with appropriate error codes for values and enums
  which failed validation. Because variables are only validated if the enums
  are set in the field masks the only error masks that will be updated will
  be those corresponding to the values set within the field masks.

  If field mask is IPFLOW_MASK_NONE, errmask is set to IPFLOW_MASK_ALL

  If a bit mask is specified, which is outside the supported range, errmask
  is updated to those bits
===========================================================================*/
static boolean ps_flowi_validate_qos_flow_param
(
  ip_flow_type                   * ip_flow_ptr,
  qos_spec_field_mask_enum_type    flow_type
)
{
  boolean  is_flow_valid = TRUE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ip_flow_ptr->err_mask = 0;

  if (ip_flow_ptr->field_mask == IPFLOW_MASK_NONE)
  {
    LOG_MSG_ERROR_2("ps_flowi_validate_qos_flow_param(): "
                    "Invalid IP_FLOW field mask: 0x%x in QOS flow type: 0x%x",
                    ip_flow_ptr->field_mask, flow_type);
    ip_flow_ptr->err_mask = IPFLOW_MASK_ALL;
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    If a bit mask is specified, which is outside the supported range, flag
    error and continue with validation so that other parameters are validated
  -------------------------------------------------------------------------*/
  if (ip_flow_ptr->field_mask & ~IPFLOW_MASK_ALL)
  {
    ip_flow_ptr->err_mask = (ip_flow_ptr->field_mask & ~IPFLOW_MASK_ALL);
    is_flow_valid = FALSE;
  }

  /*-------------------------------------------------------------------------
    For enums make sure that field in between MIN and MAX values. For
    others make sure that field's value is not 0 if specified in field mask.
    Currently these fields are unsigned so bounds checking MIN causes
    compilation warnings. Validation will need to be re-added here for the
    MIN values if they are increased to anything above 0.
  -------------------------------------------------------------------------*/
  if ((ip_flow_ptr->field_mask & IPFLOW_MASK_TRF_CLASS) &&
      (ip_flow_ptr->trf_class >= IP_TRF_CLASS_MAX))
  {
    ip_flow_ptr->err_mask |= IPFLOW_MASK_TRF_CLASS;
    is_flow_valid = FALSE;
  }

  if (ip_flow_ptr->field_mask & IPFLOW_MASK_DATA_RATE)
  {
    if (ip_flow_ptr->data_rate.format_type == DATA_RATE_FORMAT_MIN_MAX_TYPE)
    {
      if (  ip_flow_ptr->data_rate.format.min_max.guaranteed_rate >
            ip_flow_ptr->data_rate.format.min_max.max_rate)
      {
        ip_flow_ptr->err_mask |= IPFLOW_MASK_DATA_RATE;
        is_flow_valid = FALSE;
      }
    }
    else if (ip_flow_ptr->data_rate.format_type == DATA_RATE_FORMAT_MIN_MAX_EX_TYPE)
    {
      if (  ip_flow_ptr->data_rate.format.min_max_ex.guaranteed_rate >
            ip_flow_ptr->data_rate.format.min_max_ex.max_rate)
      {
        ip_flow_ptr->err_mask |= IPFLOW_MASK_DATA_RATE;
        is_flow_valid = FALSE;
      }
    }
    else if (ip_flow_ptr->data_rate.format_type ==
               DATA_RATE_FORMAT_TOKEN_BUCKET_TYPE)
    {
      if (ip_flow_ptr->data_rate.format.token_bucket.peak_rate == 0 ||
          ip_flow_ptr->data_rate.format.token_bucket.token_rate == 0 ||
          ip_flow_ptr->data_rate.format.token_bucket.size == 0)
      {
        ip_flow_ptr->err_mask |= IPFLOW_MASK_DATA_RATE;
        is_flow_valid = FALSE;
      }
    }
    else
    {
      ip_flow_ptr->err_mask |= IPFLOW_MASK_DATA_RATE;
      is_flow_valid = FALSE;
    }
  }

  if ((ip_flow_ptr->field_mask & IPFLOW_MASK_LATENCY) &&
      ip_flow_ptr->latency == 0)
  {
    ip_flow_ptr->err_mask |= IPFLOW_MASK_LATENCY;
    is_flow_valid = FALSE;
  }

  if ((ip_flow_ptr->field_mask & IPFLOW_MASK_LATENCY_VAR) &&
      ip_flow_ptr->latency_var == 0)
  {
    LOG_MSG_INFO2_1("ps_flowi_validate_qos_flow_param(): "
                    "Warning latency variance = 0 on flow type 0x%x",
                    flow_type);
  }

  if ((ip_flow_ptr->field_mask & IPFLOW_MASK_MAX_ALLOWED_PKT_SIZE) &&
      ip_flow_ptr->max_allowed_pkt_size == 0)
  {
    ip_flow_ptr->err_mask |= IPFLOW_MASK_MAX_ALLOWED_PKT_SIZE;
    is_flow_valid = FALSE;
  }

  /*-------------------------------------------------------------------------
    Validation will need to be re-added here for the MIN values if they are
    increased to anything above 0.
  -------------------------------------------------------------------------*/
  if ((ip_flow_ptr->field_mask & IPFLOW_MASK_UMTS_RES_BER) &&
      (ip_flow_ptr->umts_params.res_ber >= UMTS_RES_BIT_ERR_RATE_MAX))
  {
    ip_flow_ptr->err_mask |= IPFLOW_MASK_UMTS_RES_BER;
    is_flow_valid = FALSE;
  }

  if ((ip_flow_ptr->field_mask & IPFLOW_MASK_UMTS_TRF_PRI) &&
      (ip_flow_ptr->umts_params.trf_pri >= UMTS_TRF_HANDLING_PRI_MAX))
  {
    ip_flow_ptr->err_mask |= IPFLOW_MASK_UMTS_TRF_PRI;
    is_flow_valid = FALSE;
  }

  if ((ip_flow_ptr->field_mask & IPFLOW_MASK_WLAN_USER_PRI) &&
      (ip_flow_ptr->wlan_params.user_priority >= WLAN_USER_PRI_MAX))
  {
    ip_flow_ptr->err_mask |= IPFLOW_MASK_WLAN_USER_PRI;
    is_flow_valid = FALSE;
  }

  if ((ip_flow_ptr->field_mask & IPFLOW_MASK_WLAN_MIN_SERVICE_INTERVAL) &&
      ip_flow_ptr->wlan_params.min_service_interval == 0)
  {
    ip_flow_ptr->err_mask |= IPFLOW_MASK_WLAN_MIN_SERVICE_INTERVAL;
    is_flow_valid = FALSE;
  }

  if ((ip_flow_ptr->field_mask & IPFLOW_MASK_WLAN_MAX_SERVICE_INTERVAL) &&
      ip_flow_ptr->wlan_params.max_service_interval == 0)
  {
    ip_flow_ptr->err_mask |= IPFLOW_MASK_WLAN_MAX_SERVICE_INTERVAL;
    is_flow_valid = FALSE;
  }

  if ((ip_flow_ptr->field_mask & IPFLOW_MASK_WLAN_INACTIVITY_INTERVAL) &&
      ip_flow_ptr->wlan_params.inactivity_interval == 0)
  {
    ip_flow_ptr->err_mask |= IPFLOW_MASK_WLAN_INACTIVITY_INTERVAL;
    is_flow_valid = FALSE;
  }

  if (ip_flow_ptr->field_mask & IPFLOW_MASK_NOMINAL_SDU_SIZE)
  {
    if (ip_flow_ptr->nominal_sdu_size.size == 0)
    {
      ip_flow_ptr->err_mask |= IPFLOW_MASK_NOMINAL_SDU_SIZE;
      is_flow_valid = FALSE;
    }
    /*-----------------------------------------------------------------------
      Make sure that nominal size is not greater than max packet size if the
      latter is specified. If nominal size is constant, then it should be
      equal to max packet size because packet size is always the same
    -----------------------------------------------------------------------*/
    else if (ip_flow_ptr->field_mask & IPFLOW_MASK_MAX_ALLOWED_PKT_SIZE)
    {
      if (ip_flow_ptr->nominal_sdu_size.is_fixed == TRUE &&
          ip_flow_ptr->nominal_sdu_size.size !=
            ip_flow_ptr->max_allowed_pkt_size)
      {
        ip_flow_ptr->err_mask |=
          IPFLOW_MASK_NOMINAL_SDU_SIZE | IPFLOW_MASK_MAX_ALLOWED_PKT_SIZE;
        is_flow_valid = FALSE;
      }
      else if (ip_flow_ptr->nominal_sdu_size.is_fixed == FALSE &&
               ip_flow_ptr->nominal_sdu_size.size >
                 ip_flow_ptr->max_allowed_pkt_size)
      {
        ip_flow_ptr->err_mask |=
          IPFLOW_MASK_NOMINAL_SDU_SIZE | IPFLOW_MASK_MAX_ALLOWED_PKT_SIZE;
        is_flow_valid = FALSE;
      }
    }
  }

  if (is_flow_valid == FALSE)
  {
    LOG_MSG_ERROR_3("ps_flowi_validate_qos_flow_param(): "
                    "IP_FLOW field_mask = 0x%x, Type of FLOW processed: 0x%x, "
                    "IP_FLOW err_mask = 0x%x",
                    ip_flow_ptr->field_mask, flow_type, ip_flow_ptr->err_mask);
  }

  return is_flow_valid;

} /* ps_flowi_validate_qos_flow_param() */



/*===========================================================================
FUNCTION PS_FLOWI_VALIDATE_FLTR_SPEC()

DESCRIPTION
  Validates fltr spec in one direction

PARAMETERS
  fltr_spec_ptr       : Address where fltr spec is stored. Could be rx or tx
                        from QOS_REQUEST or QOS_MODIFY
  iface_addr_family   : addr family of iface to which this QoS request
                        belongs to
  fltr_validate_f_ptr : function which would perform extended validation
  flow_type           : Specifies direction of flow spec
  is_fltr_spec_valid  : OUT PARAM specifying if validation is successful
  ps_errno            : error returned back if function fails

RETURN VALUE
  None

DEPENDENCIES
  Parameters must be valid. Validation is not performed as this is an internal
  function

SIDE EFFECTS
  None
===========================================================================*/
static int32 ps_flowi_validate_fltr_spec
(
  const ip_filter_spec_type          * fltr_spec_ptr,
  ps_iface_addr_family_type            iface_addr_family,
  qos_fltr_validation_ex_f_ptr_type    fltr_validate_f_ptr,
  qos_spec_field_mask_enum_type        flow_type,
  boolean                            * is_fltr_spec_valid,
  int16                              * ps_errno
)
{
  uint8                                fltr_index;
  ps_iface_ipfltr_client_id_enum_type  client_id;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-----------------------------------------------------------------------
    Validate that number of filters is in between (0, MAX_FLTR_PER_REQ]
  -----------------------------------------------------------------------*/
  if (fltr_spec_ptr->num_filters == 0)
  {
    LOG_MSG_INFO1_1("ps_flowi_validate_fltr_spec(): "
                    "Zero filters specified for mask 0x%x", flow_type);
    *ps_errno = DS_EFAULT;
    return -1;
  }
  else if (fltr_spec_ptr->num_filters > MAX_FLTR_PER_REQ)
  {
    LOG_MSG_INFO1_3("ps_flowi_validate_fltr_spec(): "
                    "Too many filters specified: %d, max = %d, for mask 0x%x",
                    fltr_spec_ptr->num_filters, MAX_FLTR_PER_REQ, flow_type);
    *ps_errno = DS_EFAULT;
    return -1;
  }
  else if (fltr_spec_ptr->list_ptr == NULL)
  {
    LOG_MSG_INFO1_1("ps_flowi_validate_fltr_spec(): "
                    "NULL list_ptr in fltr spec for mask 0x%x", flow_type);
    *ps_errno = DS_EFAULT;
    return -1;
  }

  /*-------------------------------------------------------------------------
    Find out IP fltr client based on flow_type
  -------------------------------------------------------------------------*/
  if (flow_type == QOS_MASK_TX_FLOW)
  {
    client_id = IP_FLTR_CLIENT_QOS_OUTPUT;
  }
  else if (flow_type == QOS_MASK_RX_FLOW)
  {
    client_id = IP_FLTR_CLIENT_QOS_INPUT;
  }
  else
  {
    LOG_MSG_ERROR_1("ps_flowi_validate_fltr_spec(): "
                    "Invalid flow_type 0x%x is passed", flow_type);
    ASSERT(0);
    *ps_errno = DS_EFAULT;
    return -1;
  }

  /*-------------------------------------------------------------------------
    Validate individual filters in fltr spec
    As fltr spec is invalid if any of the filters are invalid,
    "is_fltr_spec_valid &=" is used below
  -------------------------------------------------------------------------*/
  *is_fltr_spec_valid &=
    ps_iface_ipfltr_validate_fltr_param(client_id,
                                        fltr_spec_ptr->list_ptr,
                                        fltr_spec_ptr->num_filters);
  if (fltr_validate_f_ptr != NULL)
  {
    for (fltr_index = 0; fltr_index < fltr_spec_ptr->num_filters; fltr_index++)
    {
        *is_fltr_spec_valid &=
          fltr_validate_f_ptr(fltr_spec_ptr->list_ptr + fltr_index, flow_type);
    }
  }

  return 0;
} /* ps_flowi_validate_fltr_spec() */



/*===========================================================================
FUNCTION PS_FLOWI_STORE_FLOW_SPEC()

DESCRIPTION
  Stores flow spec at specified location

PARAMETERS
  qos_info_ptr : Address where flow spec needs to be stored. Could be
                 qos_info_ptr or modify_qos_info_ptr of ps_flow
  qos_spec_ptr : QOS spec containing flow spec to be stored

RETURN VALUE
   None

DEPENDENCIES
  Parameters must be valid. Validation is not performed as this is an internal
  function

SIDE EFFECTS
  None
===========================================================================*/
static int32 ps_flowi_store_flow_spec
(
  qos_info_type        * qos_info_ptr,
  const qos_spec_type  * qos_spec_ptr,
  int16                * ps_errno
)
{
  ps_flow_ip_flow_spec_type  * new_flow_spec_ptr;
  uint8                        flow_index;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  qos_info_ptr->field_mask = qos_spec_ptr->field_mask;

  /*-------------------------------------------------------------------------
    Store Rx flow spec
  -------------------------------------------------------------------------*/
  if (qos_spec_ptr->field_mask & QOS_MASK_RX_FLOW)
  {
    LOG_MSG_INFO2_0("ps_flowi_store_flow_spec(): "
                    "Storing Rx flow spec");

    qos_info_ptr->rx.ipflow.req = qos_spec_ptr->rx.flow_template.req_flow;

    if (qos_spec_ptr->field_mask & QOS_MASK_RX_MIN_FLOW)
    {
      qos_info_ptr->rx.ipflow.min_req =
        qos_spec_ptr->rx.flow_template.min_req_flow;
    }

    /*-----------------------------------------------------------------------
      Store auxiliary Rx flow specs
    -----------------------------------------------------------------------*/
    if (qos_spec_ptr->field_mask & QOS_MASK_RX_AUXILIARY_FLOWS)
    {
      list_init(&(qos_info_ptr->rx.ipflow.aux_flow_list));

      for (flow_index = 0;
           flow_index < qos_spec_ptr->rx.flow_template.num_aux_flows;
           flow_index++)
      {
        new_flow_spec_ptr = (ps_flow_ip_flow_spec_type *)
                             ps_mem_get_buf(PS_MEM_PS_FLOW_IP_FLOW_SPEC_TYPE);
        if (new_flow_spec_ptr == NULL)
        {
          *ps_errno = DS_ENOMEM;
          return -1;
        }

        new_flow_spec_ptr->flow_spec =
          qos_spec_ptr->rx.flow_template.aux_flow_list_ptr[flow_index];
        list_push_back(&(qos_info_ptr->rx.ipflow.aux_flow_list),
                       &(new_flow_spec_ptr->link));
      }
    }
  }

  /*-------------------------------------------------------------------------
    Store Tx flow spec
  -------------------------------------------------------------------------*/
  if (qos_spec_ptr->field_mask & QOS_MASK_TX_FLOW)
  {
    LOG_MSG_INFO2_0("ps_flowi_store_flow_spec(): "
                    "Storing Tx flow spec");

    qos_info_ptr->tx.ipflow.req = qos_spec_ptr->tx.flow_template.req_flow;

    if (qos_spec_ptr->field_mask & QOS_MASK_TX_MIN_FLOW)
    {
      qos_info_ptr->tx.ipflow.min_req =
        qos_spec_ptr->tx.flow_template.min_req_flow;
    }

    /*-------------------------------------------------------------------------
      Store auxiliary Tx flow specs
    -------------------------------------------------------------------------*/
    if (qos_spec_ptr->field_mask & QOS_MASK_TX_AUXILIARY_FLOWS)
    {
      list_init(&(qos_info_ptr->tx.ipflow.aux_flow_list));

      for (flow_index = 0;
           flow_index < qos_spec_ptr->tx.flow_template.num_aux_flows;
           flow_index++)
      {
        new_flow_spec_ptr = (ps_flow_ip_flow_spec_type *)
                             ps_mem_get_buf(PS_MEM_PS_FLOW_IP_FLOW_SPEC_TYPE);
        if (new_flow_spec_ptr == NULL)
        {
          *ps_errno = DS_ENOMEM;
          return -1;
        }

        new_flow_spec_ptr->flow_spec =
          qos_spec_ptr->tx.flow_template.aux_flow_list_ptr[flow_index];
        list_push_back(&(qos_info_ptr->tx.ipflow.aux_flow_list),
                       &(new_flow_spec_ptr->link));
      }
    }
  }

  return 0;

} /* ps_flowi_store_flow_spec() */



/*===========================================================================
FUNCTION PS_FLOWI_VALIDATE_AUX_FLOW_SPEC()

DESCRIPTION
  Validates auxiliary flow spec specified at given location

PARAMETERS
  ip_flow_spec_ptr       : Address where auxiliary flow spec is stored. Could
                           be rx or tx from QOS_REQUEST or QOS_MODIFY
  flow_validate_f_ptr    : function used by mode handler to perform extended
                           flow validation
  flow_type              : Specifies direction of flow spec
  is_aux_flow_spec_valid : OUT PARAM specifying if validation is successful
  ps_errno               : error returned back if function fails

RETURN VALUE
   0 : on sucess
  -1 : on failure

DEPENDENCIES
  Parameters must be valid. Validation is not performed as this is an internal
  function

SIDE EFFECTS
  None
===========================================================================*/
static int32 ps_flowi_validate_aux_flow_spec
(
  const ip_flow_spec_type            * ip_flow_spec_ptr,
  qos_flow_validation_ex_f_ptr_type    flow_validate_f_ptr,
  qos_spec_field_mask_enum_type        flow_type,
  boolean                            * is_aux_flow_spec_valid,
  int16                              * ps_errno
)
{
  uint8  aux_flow_index;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Validate that # of auxiliary flow specs are in range
    (0, MAX_ADDITIONAL_FLOWS_PER_REQ)
  -------------------------------------------------------------------------*/
  if (ip_flow_spec_ptr->num_aux_flows > MAX_ADDITIONAL_FLOWS_PER_REQ)
  {
    LOG_MSG_INFO1_3("ps_flowi_validate_aux_flow_spec(): "
                    "Too many auxiliary flows specified: %d, max = %d, for mask 0x%x",
                    ip_flow_spec_ptr->num_aux_flows,
                    MAX_ADDITIONAL_FLOWS_PER_REQ,
                    flow_type);
    *ps_errno = DS_EFAULT;
    return -1;
  }
  else if (ip_flow_spec_ptr->num_aux_flows == 0)
  {
    LOG_MSG_INFO1_1("ps_flowi_validate_aux_flow_spec(): "
                    "Zero auxiliary flows specified for mask 0x%x", flow_type);
    *ps_errno = DS_EFAULT;
    return -1;
  }
  else if (ip_flow_spec_ptr->aux_flow_list_ptr == NULL)
  {
    LOG_MSG_INFO1_1("ps_flowi_validate_aux_flow_spec(): "
                    "NULL aux_flow_list_ptr for mask 0x%x", flow_type);
    *ps_errno = DS_EFAULT;
    return -1;
  }

  /*-------------------------------------------------------------------------
    Validate each auxiliary flow spec
  -------------------------------------------------------------------------*/
  for (aux_flow_index = 0;
       aux_flow_index < ip_flow_spec_ptr->num_aux_flows;
       aux_flow_index++)
  {
    *is_aux_flow_spec_valid &=
      ps_flowi_validate_qos_flow_param
      (
        ip_flow_spec_ptr->aux_flow_list_ptr + aux_flow_index,
        flow_type
      );

    if (flow_validate_f_ptr)
    {
      *is_aux_flow_spec_valid &=
        flow_validate_f_ptr
        (
          ip_flow_spec_ptr->aux_flow_list_ptr + aux_flow_index,
          flow_type
        );
    }
  }

  return 0;

} /* ps_flowi_validate_aux_flow_spec() */



/*===========================================================================
FUNCTION PS_FLOWI_STORE_RX_FLTR_SPEC()

DESCRIPTION
  Stores Rx fltr spec at specified location

PARAMETERS
  qos_info_ptr : Address where Rx fltr spec needs to be stored. Could be
                 qos_info_ptr or modify_qos_info_ptr of ps_flow
  qos_spec_ptr : QOS spec containing Rx fltr spec to be stored

RETURN VALUE
  None

DEPENDENCIES
  Parameters must be valid. Validation is not performed as this is an internal
  function

SIDE EFFECTS
  None
===========================================================================*/
static int32 ps_flowi_store_rx_fltr_spec
(
  qos_info_type        * qos_info_ptr,
  const qos_spec_type  * qos_spec_ptr,
  int16                * ps_errno
)
{
  ps_flow_rx_fltr_buf_type  * new_filter_buf_ptr;
  uint8                       fltr_index;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO2_0("ps_flowi_store_rx_fltr_spec(): "
                  "Storing Rx fltr spec");

  list_init (&(qos_info_ptr->rx.fltr_list));

  /*-------------------------------------------------------------------------
    Store Rx filter spec
  -------------------------------------------------------------------------*/
  for (fltr_index = 0;
       fltr_index < qos_spec_ptr->rx.fltr_template.num_filters;
       fltr_index++)
  {
    new_filter_buf_ptr = (ps_flow_rx_fltr_buf_type *)
                           ps_mem_get_buf(PS_MEM_RX_QOS_FLTR_BUF_TYPE);
    if (new_filter_buf_ptr == NULL)
    {
      *ps_errno = DS_ENOMEM;
      return -1;
    }

    /*-----------------------------------------------------------------------
      Precedence is populated through ps_ifacei_rx_qos_fltr_add()
    -----------------------------------------------------------------------*/
    new_filter_buf_ptr->filter =
      qos_spec_ptr->rx.fltr_template.list_ptr[fltr_index];
    list_push_back(&(qos_info_ptr->rx.fltr_list),
                   &(new_filter_buf_ptr->link));
  }

  return 0;

} /* ps_flowi_store_rx_fltr_spec() */



/*===========================================================================
FUNCTION PS_FLOWI_IS_QOS_MODIFY_SPEC_WELL_FORMED()

DESCRIPTION
  This function checks if qos_modify_spec is well formed.

PARAMETERS
  flow_ptr        : flow whose QOS is gonna be modified
  qos_modify_mask : mask indicating what part of QOS is being changed and how
  field_mask      : mask specified in QOS spec of QOS_MODIFY
  ps_errno        : specific error code in case operation fails

RETURN VALUE
   0 : if qos_modify_spec is well formed
  -1 : otherwise

DEPENDENCIES
  flow_ptr must be valid
  Must be called inside a TASKLOCK()

SIDE EFFECTS
  None
===========================================================================*/
static int32 ps_flowi_is_qos_modify_spec_well_formed
(
  ps_flow_type         * flow_ptr,
  const qos_spec_type  * qos_spec_ptr,
  int16                * ps_errno
)
{
  qos_spec_field_mask_type  field_mask;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Validate that QOS_MODIFY is well formed. Rules are
      1. Some operation must be performed on atleast one of four blocks,
         i.e Tx flow, Tx fltr, Rx flow, and Rx fltr
      2. Reserved bits in qos_modify_mask must not be turned on
      3. If Tx flow is deleted, ensure that it was requested earlier. Also
         ensure that Tx min flow and Tx auxiliary flows are not specified.
         Also ensure that Rx flow either was requested earlier or is
         requested now
      4. If Tx fltrs are modified, make sure that Tx flow was requested
         earlier
      5. Rules 3, and 4 apply to Rx flow and Rx fltr also
      6. Both Tx flow and Rx flow can't be deleted
  -------------------------------------------------------------------------*/

  if (qos_spec_ptr->field_mask == QOS_MASK_INVALID)
  {
    LOG_MSG_INFO1_1("ps_flowi_is_qos_modify_spec_well_formed(): "
                    "App is not changing anything in QOS_MODIFY on "
                    "ps_flow 0x%p", flow_ptr);
    *ps_errno = DS_EFAULT;
    return -1;
  }

  if (qos_spec_ptr->field_mask & ~QOS_MODIFY_MASK_ALL)
  {
    LOG_MSG_INFO1_1("ps_flowi_is_qos_modify_spec_well_formed(): "
                    "Reserved bits are turned on by app in QOS_MODIFY on "
                    "ps_flow 0x%p", flow_ptr);
    *ps_errno = DS_EFAULT;
    return -1;
  }

  field_mask = PS_FLOWI_GET_QOS_FIELD_MASK(flow_ptr);

  /*-------------------------------------------------------------------------
    Validate Rule 3 for Tx flow spec
  -------------------------------------------------------------------------*/
  if (qos_spec_ptr->field_mask & QOS_MASK_TX_FLOW)
  {
    if (qos_spec_ptr->tx.flow_template.req_flow.field_mask ==
          IPFLOW_MASK_NONE)   /* if Tx flow is deleted */
    {
      if (!(field_mask & QOS_MASK_TX_FLOW))
      {
        LOG_MSG_INFO1_1("ps_flowi_is_qos_modify_spec_well_formed(): "
                        "Deleting Tx flow spec when it was not requested "
                        "previously on ps_flow 0x%p", flow_ptr);
        *ps_errno = DS_EFAULT;
        return -1;
      }

      if (qos_spec_ptr->field_mask &
            (QOS_MASK_TX_MIN_FLOW | QOS_MASK_TX_AUXILIARY_FLOWS))
      {
        LOG_MSG_INFO1_1("ps_flowi_is_qos_modify_spec_well_formed(): "
                        "Deleting Tx flow spec while specifying one of Tx min "
                        "flow spec or Tx auxiliary flow specs, on ps_flow 0x%p",
                        flow_ptr);
        *ps_errno = DS_EFAULT;
        return -1;
      }

      /*---------------------------------------------------------------------
        A QOS spec must have flow spec in at least one direction. So flag
        error if Rx flow is not requested now and wasn't requested earlier.
        Also flag error if Rx flow is also deleted.
      ---------------------------------------------------------------------*/
      if ((!(field_mask & QOS_MASK_RX_FLOW) &&
             !(qos_spec_ptr->field_mask & QOS_MASK_RX_FLOW)) ||
           ((qos_spec_ptr->field_mask & QOS_MASK_RX_FLOW) &&
              (qos_spec_ptr->rx.flow_template.req_flow.field_mask ==
                IPFLOW_MASK_NONE)))
      {
        LOG_MSG_INFO1_1("ps_flowi_is_qos_modify_spec_well_formed(): "
                        "Can't have QOS spec with out both Tx and Rx flow specs "
                        "on ps_flow 0x%p", flow_ptr);
        *ps_errno = DS_EFAULT;
        return -1;
      }

      if (qos_spec_ptr->field_mask & QOS_MODIFY_MASK_TX_FLTR_MODIFY)
      {
        LOG_MSG_INFO1_1("ps_flowi_is_qos_modify_spec_well_formed(): "
                        "Can't modify Tx filters while deleting Tx QoS, on "
                        "ps_flow 0x%p", flow_ptr);
        *ps_errno = DS_EFAULT;
        return -1;
      }
    } /* if Tx flow is deleted */
  } /* Some operation is performed on Tx flow spec */

  /*-------------------------------------------------------------------------
    Validate Rule 4 for Tx fltr spec
  -------------------------------------------------------------------------*/
  if (qos_spec_ptr->field_mask & QOS_MODIFY_MASK_TX_FLTR_MODIFY)
  {
    if (!(field_mask & QOS_MASK_TX_FLOW))
    {
      LOG_MSG_INFO1_1("ps_flowi_is_qos_modify_spec_well_formed(): "
                      "Modifying Tx fltr spec when it was not requested "
                      "previously on ps_flow 0x%p", flow_ptr);
      *ps_errno = DS_EFAULT;
      return -1;
    }
  }

  /*-------------------------------------------------------------------------
    Validate Rule 3 for Rx flow spec
  -------------------------------------------------------------------------*/
  if (qos_spec_ptr->field_mask & QOS_MASK_RX_FLOW)
  {
    if (qos_spec_ptr->rx.flow_template.req_flow.field_mask ==
          IPFLOW_MASK_NONE)   /* if Rx flow is deleted */
    {
      if (!(field_mask & QOS_MASK_RX_FLOW))
      {
        LOG_MSG_INFO1_1("ps_flowi_is_qos_modify_spec_well_formed(): "
                        "Deleting Rx flow spec when it was not requested "
                        "previously on ps_flow 0x%p", flow_ptr);
        *ps_errno = DS_EFAULT;
        return -1;
      }

      if (qos_spec_ptr->field_mask &
            (QOS_MASK_RX_MIN_FLOW | QOS_MASK_RX_AUXILIARY_FLOWS))
      {
        LOG_MSG_INFO1_1("ps_flowi_is_qos_modify_spec_well_formed(): "
                        "Deleting Rx flow spec while specifying one of Rx min "
                        "flow spec or Rx auxiliary flow specs, on ps_flow 0x%p",
                        flow_ptr);
        *ps_errno = DS_EFAULT;
        return -1;
      }

      /*---------------------------------------------------------------------
        A QOS spec must have flow spec in at least one direction. So flag
        error if Tx flow is not requested now and wasn't requested earlier.
        Also flag error if Tx flow is also deleted.
      ---------------------------------------------------------------------*/
      if ((!(field_mask & QOS_MASK_TX_FLOW) &&
             !(qos_spec_ptr->field_mask & QOS_MASK_TX_FLOW)) ||
           ((qos_spec_ptr->field_mask & QOS_MASK_TX_FLOW) &&
              (qos_spec_ptr->tx.flow_template.req_flow.field_mask ==
                IPFLOW_MASK_NONE)))
      {
        LOG_MSG_INFO1_1("ps_flowi_is_qos_modify_spec_well_formed(): "
                        "Can't have QOS spec with out both Tx and Rx flow specs "
                        "on ps_flow 0x%p", flow_ptr);
        *ps_errno = DS_EFAULT;
        return -1;
      }

      if (qos_spec_ptr->field_mask & QOS_MODIFY_MASK_RX_FLTR_MODIFY)
      {
        LOG_MSG_INFO1_1("ps_flowi_is_qos_modify_spec_well_formed(): "
                        "Can't modify Rx filters while deleting Rx QoS, on "
                        "ps_flow 0x%p", flow_ptr);
        *ps_errno = DS_EFAULT;
        return -1;
      }
    } /* if Rx flow is deleted */
  } /* Some operation is performed on Rx flow spec */

  /*-------------------------------------------------------------------------
    Validate Rule 4 for Rx fltr spec
  -------------------------------------------------------------------------*/
  if (qos_spec_ptr->field_mask & QOS_MODIFY_MASK_RX_FLTR_MODIFY)
  {
    if (!(field_mask & QOS_MASK_RX_FLOW))
    {
      LOG_MSG_INFO1_1("ps_flowi_is_qos_modify_spec_well_formed(): "
                      "Modifying Rx fltr spec when it was not requested "
                      "previously on ps_flow 0x%p", flow_ptr);
      *ps_errno = DS_EFAULT;
      return -1;
    }
  }

  return 0;

} /* ps_flowi_is_qos_modify_spec_well_formed() */
#endif /* FEATURE_DATA_PS_QOS */



/*===========================================================================

                        PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================
FUNCTION PS_FLOWI_CREATE()

DESCRIPTION
  This function creates a new flow and initializes it.

PARAMETERS
  is_qos_supported : is QOS supported on the flow being created?
  flow_ptr         : OUT PARAM. Flow that is created
  ps_errno         : error returned back in case of an error

RETURN VALUE
   0 : if flow can be created
  -1 : otherwise

DEPENDENCIES
  Pass FALSE to create a default flow or pass TRUE otherwise

SIDE EFFECTS
  None
===========================================================================*/
int32 ps_flowi_create
(
  boolean          is_qos_supported,
  ps_flow_type  ** flow_ptr_ptr,
  int16          * ps_errno
)
{
  uint8  ev_index;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  *flow_ptr_ptr = (ps_flow_type *) ps_mem_get_buf(PS_MEM_PS_FLOW_TYPE);
  if (*flow_ptr_ptr == NULL)
  {
    *ps_errno = DS_ENOMEM;
    return -1;
  }

  memset(*flow_ptr_ptr, 0, sizeof(ps_flow_type));

  /*-------------------------------------------------------------------------
    Initialize flow event queue
  -------------------------------------------------------------------------*/
  for (ev_index = 0; ev_index < MAX_EVENT_QUEUES; ev_index++)
  {
    (void) q_init(((*flow_ptr_ptr)->flow_private.event_q_array) + ev_index);
  }

  (*flow_ptr_ptr)->flow_private.state        = FLOW_NULL;
  (*flow_ptr_ptr)->flow_private.valid_cookie = PS_FLOW_VALID_COOKIE;

  /*-------------------------------------------------------------------------
    If QOS is supported, allocate qos info buffer from ps_mem pool
  -------------------------------------------------------------------------*/
  if (is_qos_supported)
  {
    (*flow_ptr_ptr)->flow_private.qos_info_ptr =
      (qos_info_type *) ps_mem_get_buf(PS_MEM_QOS_INFO_TYPE);

    if ((*flow_ptr_ptr)->flow_private.qos_info_ptr == NULL)
    {
      LOG_MSG_ERROR_1("ps_flowi_create(): "
                      "Couldn't create qos_info_ptr for flow, 0x%p",
                      *flow_ptr_ptr);
      *ps_errno = DS_ENOMEM;
      PS_MEM_FREE(*flow_ptr_ptr);
      return -1;
    }

    memset((*flow_ptr_ptr)->flow_private.qos_info_ptr,
           0,
           sizeof(qos_info_type));

  }

  return 0;

} /* ps_flowi_create() */



/*===========================================================================
FUNCTION PS_FLOWI_BIND_PHYS_LINK()

DESCRIPTION
  Binds a flow to a phys link

PARAMETERS
  flow_ptr      : pointer to the flow
  phys_link_ptr : flow_ptr's binding

RETURN VALUE
   0 : on success
  -1 : on failure

DEPENDENCIES
  parameters must be valid

SIDE EFFECTS
  None
===========================================================================*/
int ps_flowi_bind_phys_link
(
  ps_flow_type       * flow_ptr,
  ps_phys_link_type  * phys_link_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (PS_FLOWI_GET_PHYS_LINK(flow_ptr) != NULL)
  {
    LOG_MSG_INFO1_1("ps_flowi_bind_phys_link(): "
                    "Flow, 0x%p, is already bound", flow_ptr);

    if (PS_FLOWI_GET_PHYS_LINK(flow_ptr) == phys_link_ptr)
    {
      return 0;
    }

    return -1;
  }

  /* Sanity check */
  if(phys_link_ptr == NULL)
  {
    LOG_MSG_ERROR_0("ps_flowi_bind_phys_link(): "
                    "phys_link_ptr is NULL!");
    return -1;
  }

  LOG_MSG_INFO2_2("ps_flowi_bind_phys_link(): "
                  "Binding flow, 0x%p, to phys link, 0x%p",
                  flow_ptr, phys_link_ptr);

  flow_ptr->flow_private.phys_link_ptr = phys_link_ptr;
  PS_PHYS_LINKI_INC_REF_CNT(phys_link_ptr);

  /*-------------------------------------------------------------------------
    Register for phys link events if phys link is FLOW_COUPLED as flows are
    transparent to mode handlers if this capability is turned on
  -------------------------------------------------------------------------*/
  if (!PS_PHYS_LINKI_GET_CAPABILITY(phys_link_ptr,
                                    PS_PHYS_LINK_CAPABILITY_FLOW_DECOUPLED))
  {
    /*-----------------------------------------------------------------------
      When flows are COUPLED to phys link, flows are transparent to mode
      handler and at most only one flow is allowed to be bound to a
      phys link. Otherwise it is difficult to manage flow's state
      transparently
    -----------------------------------------------------------------------*/
    if (PS_PHYS_LINKI_GET_REF_CNT(phys_link_ptr) != 1)
    {
      LOG_MSG_ERROR_2("ps_flowi_bind_phys_link(): "
                      "Invalid ref cnt, %d, on FLOW_COUPLED phys link, 0x%p",
                      PS_PHYS_LINKI_GET_REF_CNT(phys_link_ptr), phys_link_ptr);

      flow_ptr->flow_private.phys_link_ptr = NULL;
      PS_PHYS_LINKI_DEC_REF_CNT(phys_link_ptr);
      ASSERT(0);
      return -1;
    }

    LOG_MSG_INFO2_2("ps_flowi_bind_phys_link(): "
                    "Registering for phys link, 0x%p, events on flow, 0x%p",
                    phys_link_ptr, flow_ptr);

    ps_flowi_phys_link_ev_cback_reg(flow_ptr);
  }

  return 0;

} /* ps_flowi_bind_phys_link() */



/*===========================================================================
FUNCTION PS_FLOWI_UNBIND_PHYS_LINK()

DESCRIPTION
  Unbinds a flow

PARAMETERS
  flow_ptr : pointer to the flow

RETURN VALUE
   0 : on success
  -1 : on failure

DEPENDENCIES
  flow must be valid and must be bound to a phys link

SIDE EFFECTS
  None
===========================================================================*/
int ps_flowi_unbind_phys_link
(
  ps_flow_type  * flow_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO2_2("ps_flowi_unbind_phys_link(): "
                  "Unbinding flow, 0x%p, from phys link, 0x%p",
                  flow_ptr, PS_FLOWI_GET_PHYS_LINK(flow_ptr));

  if (PS_FLOWI_GET_PHYS_LINK(flow_ptr) == NULL)
  {
    LOG_MSG_INFO2_1("ps_flowi_unbind_phys_link(): "
                    "Flow, 0x%p, is not bound to a phys link", flow_ptr);
    return 0;
  }

  /*-------------------------------------------------------------------------
    De-register for phys link events if phys link(flow's current binding) is
    FLOW_COUPLED
  -------------------------------------------------------------------------*/
  if (!PS_PHYS_LINKI_GET_CAPABILITY(PS_FLOWI_GET_PHYS_LINK(flow_ptr),
                                    PS_PHYS_LINK_CAPABILITY_FLOW_DECOUPLED))
  {
    LOG_MSG_INFO2_2("ps_flowi_unbind_phys_link(): "
                    "De-registering for phys link, 0x%p, events on flow, 0x%p",
                    PS_FLOWI_GET_PHYS_LINK(flow_ptr), flow_ptr);

    ps_flowi_phys_link_ev_cback_dereg(flow_ptr);
  }

  PS_PHYS_LINKI_DEC_REF_CNT(PS_FLOWI_GET_PHYS_LINK(flow_ptr));
  flow_ptr->flow_private.phys_link_ptr = NULL;
  return 0;

} /* ps_flowi_unbind_phys_link() */



/*===========================================================================
FUNCTION PS_FLOWI_REBIND_PHYS_LINK()

DESCRIPTION
  Binds a flow to a phys link, different from its current binding

PARAMETERS
  flow_ptr      : pointer to the flow
  phys_link_ptr : flow_ptr's new binding

RETURN VALUE
   0 : on success
  -1 : on failure

DEPENDENCIES
  flow must have been already bound and parameters must be valid

SIDE EFFECTS
  None
===========================================================================*/
int ps_flowi_rebind_phys_link
(
  ps_flow_type       * flow_ptr,
  ps_phys_link_type  * phys_link_ptr
)
{
  ps_phys_link_type  * orig_phys_link_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  orig_phys_link_ptr = PS_FLOWI_GET_PHYS_LINK(flow_ptr);

  if (!PS_PHYS_LINK_IS_VALID(orig_phys_link_ptr))
  {
    LOG_MSG_ERROR_1("ps_flowi_rebind_phys_link(): "
                    "Flow, 0x%p, is bound to invalid phys link", flow_ptr);
    ASSERT(0);
    return -1;
  }

  LOG_MSG_INFO2_3("ps_flowi_rebind_phys_link(): "
                  "Rebinding flow, 0x%p, from phys link, 0x%p to phys link, 0x%p",
                  flow_ptr, orig_phys_link_ptr, phys_link_ptr);

  (void) ps_flowi_unbind_phys_link(flow_ptr);
  return ps_flowi_bind_phys_link(flow_ptr, phys_link_ptr);

} /* ps_flowi_rebind_phys_link() */



/*===========================================================================
FUNCTION PS_FLOWI_GET_PHYS_LINK()

DESCRIPTION
  Returns a flow's current binding, which could be NULL if flow is not bound
  yet

PARAMETERS
  flow_ptr : pointer to the flow

RETURN VALUE
  phys_link_ptr, a flow is bound to

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
ps_phys_link_type * ps_flowi_get_phys_link(ps_flow_type * flow_ptr)
{
  if(PS_FLOW_IS_VALID(flow_ptr))
  {
    if(PS_FLOW_IS_VALID(PS_FLOWI_GET_ASSOC_PS_FLOW(flow_ptr)))
    {
      return(PS_FLOW_GET_PHYS_LINK(PS_FLOWI_GET_ASSOC_PS_FLOW(flow_ptr)));
    }
    else
    {
      return((flow_ptr)->flow_private.phys_link_ptr);
    }
  }
  else
  {
    LOG_MSG_ERROR("ps_flowi_get_phys_link(): "
                  "Invalid flow_ptr", 0, 0, 0);
    return NULL;
  }
} /* ps_flowi_get_phys_link() */

#ifdef FEATURE_DATA_PS_QOS
/*===========================================================================
FUNCTION PS_FLOWI_DELETE()

DESCRIPTION
  This function deletes a flow

  NOTE : A default flow MUST NOT be deleted

PARAMETERS
  flow_ptr : flow to be deleted

RETURN VALUE
   0 : on success
  -1 : otherwise

DEPENDENCIES
  None

SIDE EFFECTS
  All the event registrations on this flow are removed
===========================================================================*/
int ps_flowi_delete
(
  ps_flow_type  * flow_ptr
)
{
  ps_flow_event_buf_type  * flow_ev_buf_ptr;
  ps_flow_event_buf_type  * next_flow_ev_buf_ptr;
  ps_tx_meta_info_type    * meta_info_ptr;
  ps_flow_type            * assoc_flow_ptr;
  uint8                     loop;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (PS_FLOWI_GET_CAPABILITY(flow_ptr, PS_FLOW_CAPABILITY_DEFAULT))
  {
    LOG_MSG_ERROR_1("ps_flowi_delete(): "
                    "Default flow, 0x%p, must not be deleted", flow_ptr);
    ASSERT(0);
    return -1;
  }

  if (PS_FLOWI_GET_STATE(flow_ptr) != FLOW_NULL)
  {
    LOG_MSG_ERROR_2("ps_flowi_delete(): "
                    "Deleting flow, 0x%p, in invalid state, 0x%x",
                    flow_ptr, PS_FLOWI_GET_STATE(flow_ptr));
    ASSERT(0);
    return -1;
  }

  /*-------------------------------------------------------------------------
    If it is a logical flow, remove all the event buffers which are allocated
    to register for events with associated flow
  -------------------------------------------------------------------------*/
  assoc_flow_ptr = PS_FLOWI_GET_ASSOC_PS_FLOW(flow_ptr);

  if (assoc_flow_ptr == NULL || !PS_FLOW_IS_VALID(assoc_flow_ptr))
  {
    LOG_MSG_ERROR_2("ps_flowi_delete(): "
                    "Invalid assoc flow, 0x%p associated with flow, 0x%p",
                    assoc_flow_ptr, flow_ptr);
  }
  else
  {
    ps_flowi_assoc_flow_ev_cback_dereg(flow_ptr);
  }

  /*-------------------------------------------------------------------------
    Delete the tx_meta_info associated with this flow
  -------------------------------------------------------------------------*/
  meta_info_ptr = PS_FLOW_GET_META_INFO_FROM_FLOW( flow_ptr);
  PS_TX_META_INFO_FREE(&meta_info_ptr);

  /*-------------------------------------------------------------------------
    Remove all the event registrations on this flow
  -------------------------------------------------------------------------*/
  for (loop = 0; loop < MAX_EVENT_QUEUES; loop++)
  {
    flow_ev_buf_ptr = q_check((flow_ptr->flow_private.event_q_array) + loop);
    while (flow_ev_buf_ptr != NULL)
    {
      next_flow_ev_buf_ptr =
        q_next((flow_ptr->flow_private.event_q_array) + loop,
               &(flow_ev_buf_ptr->link));

#ifdef FEATURE_Q_NO_SELF_QPTR
      q_delete((flow_ptr->flow_private.event_q_array) + loop,
               &(flow_ev_buf_ptr->link));
#else
      q_delete(&(flow_ev_buf_ptr->link));
#endif

      ps_flow_free_event_cback_buf(flow_ev_buf_ptr);
      flow_ev_buf_ptr = next_flow_ev_buf_ptr;
    }

    /*-----------------------------------------------------------------------
      Destroy the event_q_array
    -----------------------------------------------------------------------*/
    q_destroy((flow_ptr->flow_private.event_q_array) + loop);
  }

  (void) ps_flowi_delete_qos_spec(flow_ptr);

  flow_ptr->flow_private.this_flow    = NULL;
  flow_ptr->flow_private.valid_cookie = 0;

  PS_MEM_FREE(flow_ptr);
  return 0;

} /* ps_flowi_delete() */


/*===========================================================================
FUNCTION PS_FLOWI_VALIDATE_NET_INITIATED_QOS_SPEC()

DESCRIPTION
This function validates the requested QOS spec. Both flow spec and filter
spec are validated in each direction. A generic validation is performed on
the parameters and if mode handlers require extra validation they can pass
an optional validation function ptr to perform extended validation. Note
that generic validation will still be performed before calling extended
validaiton functions.

PARAMETERS
qos_req_param     : structure containing qos_spec and information necessary
to validate it.
iface_addr_family : addr family of iface on which QoS is requested
ps_errno          : specific error code in case operation fails

RETURN VALUE
0 : if QOS spec is valid
-1 : otherwise

DEPENDENCIES
None

SIDE EFFECTS
None
===========================================================================*/
int ps_flowi_validate_net_initiated_qos_spec
(
  const net_initiated_qos_spec_type  *   net_initiated_qos_spec,
  ps_iface_addr_family_type              iface_addr_family,
  int16                              *   ps_errno
)
{
  boolean  is_qos_spec_valid = TRUE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(iface_addr_family != IFACE_INVALID_ADDR_FAMILY);


  /*-----------------------------------------------------------------------
    Validate Rx fltr spec
  -----------------------------------------------------------------------*/
  if (net_initiated_qos_spec->rx_fltr_template.num_filters > 0)
  {
    if (ps_flowi_validate_fltr_spec
        (
          &(net_initiated_qos_spec->rx_fltr_template),
          iface_addr_family,
          NULL,
          QOS_MASK_RX_FLOW,
          &is_qos_spec_valid,
          ps_errno
        ) == -1)
    {
      LOG_MSG_INFO1_0("ps_flowi_validate_net_initiated_qos_spec(): "
                      "Invalid QOS Spec, Rx fltr spec is invalid");
      /* ps_errno is populated in ps_flowi_validate_fltr_spec() */
      return -1;
    }
  }

  /*-----------------------------------------------------------------------
    Validate Tx fltr spec
  -----------------------------------------------------------------------*/
  if (net_initiated_qos_spec->tx_fltr_template.num_filters > 0)
  {
    if (ps_flowi_validate_fltr_spec
        (
          &(net_initiated_qos_spec->tx_fltr_template),
          iface_addr_family,
          NULL,
          QOS_MASK_TX_FLOW,
          &is_qos_spec_valid,
          ps_errno
        ) == -1)
    {
      LOG_MSG_INFO1_0("ps_flowi_validate_net_initiated_qos_spec(): "
                      "Invalid QOS Spec, Tx fltr spec is invalid");
      /* ps_errno is populated in ps_flowi_validate_fltr_spec() */
      return -1;
    }
  }

  if (is_qos_spec_valid == FALSE)
  {
    LOG_MSG_INFO1_0("ps_flowi_validate_net_initiated_qos_spec(): "
                    "QOS Spec validation failed");
    *ps_errno = DS_EFAULT;
    return -1;
  }

  return 0;

} /* ps_flowi_validate_net_initiated_qos_spec() */

/*===========================================================================
FUNCTION PS_FLOWI_VALIDATE_QOS_SPEC()

DESCRIPTION
  This function validates the requested QOS spec. Both flow spec and filter
  spec are validated in each direction. A generic validation is performed on
  the parameters and if mode handlers require extra validation they can pass
  an optional validation function ptr to perform extended validation. Note
  that generic validation will still be performed before calling extended
  validaiton functions.

PARAMETERS
  qos_req_param     : structure containing qos_spec and information necessary
                      to validate it.
  iface_addr_family : addr family of iface on which QoS is requested
  ps_errno          : specific error code in case operation fails

RETURN VALUE
   0 : if QOS spec is valid
  -1 : otherwise

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_flowi_validate_qos_spec
(
  const ps_flow_create_param_type  * qos_req_param,
  ps_iface_addr_family_type          iface_addr_family,
  int16                            * ps_errno
)
{
  boolean  is_qos_spec_valid = TRUE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(iface_addr_family != IFACE_INVALID_ADDR_FAMILY);

  /*-------------------------------------------------------------------------
    One of Rx or Tx flow specs must be specified
  -------------------------------------------------------------------------*/
  if (!(qos_req_param->qos_spec->field_mask & QOS_MASK_RX_FLOW) &&
      !(qos_req_param->qos_spec->field_mask & QOS_MASK_TX_FLOW))
  {
    LOG_MSG_INFO1_1("ps_flowi_validate_qos_spec(): "
                    "Invalid QOS spec, mask=%d, specify Rx or Tx flow",
                    qos_req_param->qos_spec->field_mask);
    *ps_errno = DS_EFAULT;
    return -1;
  }

  /*-------------------------------------------------------------------------
    If a bit mask is specified, which is outside the supported range, flag
    error
  -------------------------------------------------------------------------*/
  if (qos_req_param->qos_spec->field_mask & ~QOS_MASK_ALL)
  {
    LOG_MSG_INFO1_1("ps_flowi_validate_qos_spec(): "
                    "Invalid QOS spec, mask=%d, Bits outside the allowed range "
                    "are set", qos_req_param->qos_spec->field_mask);
    *ps_errno = DS_EFAULT;
    return -1;
  }

  /*-------------------------------------------------------------------------
    Validate Rx flow spec and Rx fltr spec
  -------------------------------------------------------------------------*/
  if (qos_req_param->qos_spec->field_mask & QOS_MASK_RX_FLOW)
  {
    /*-----------------------------------------------------------------------
      Validate Rx required flow spec
    -----------------------------------------------------------------------*/
    is_qos_spec_valid &=
      ps_flowi_validate_qos_flow_param
      (
        &(qos_req_param->qos_spec->rx.flow_template.req_flow),
        QOS_MASK_RX_FLOW
      );

    if (qos_req_param->flow_validate_f_ptr)
    {
      is_qos_spec_valid &=
        qos_req_param->flow_validate_f_ptr
        (
          &(qos_req_param->qos_spec->rx.flow_template.req_flow),
          QOS_MASK_RX_FLOW
        );
    }

    /*-----------------------------------------------------------------------
      Validate Rx minimum required flow spec
    -----------------------------------------------------------------------*/
    if (qos_req_param->qos_spec->field_mask & QOS_MASK_RX_MIN_FLOW)
    {
      is_qos_spec_valid &=
        ps_flowi_validate_qos_flow_param
        (
          &(qos_req_param->qos_spec->rx.flow_template.min_req_flow),
          QOS_MASK_RX_MIN_FLOW
        );

      if (qos_req_param->flow_validate_f_ptr)
      {
        is_qos_spec_valid &=
          qos_req_param->flow_validate_f_ptr
          (
            &(qos_req_param->qos_spec->rx.flow_template.min_req_flow),
            QOS_MASK_RX_MIN_FLOW
          );
      }
    }

    /*-----------------------------------------------------------------------
      Validate auxiliary Rx flow specs
    -----------------------------------------------------------------------*/
    if (qos_req_param->qos_spec->field_mask & QOS_MASK_RX_AUXILIARY_FLOWS)
    {
      if (ps_flowi_validate_aux_flow_spec
          (
            &(qos_req_param->qos_spec->rx.flow_template),
            qos_req_param->flow_validate_f_ptr,
            QOS_MASK_RX_AUXILIARY_FLOWS,
            &is_qos_spec_valid,
            ps_errno
          ) == -1)
      {
        LOG_MSG_INFO1_0("ps_flowi_validate_qos_spec(): "
                        "Invalid QOS Spec, Rx aux flow spec is invalid");
        /* ps_errno is populated in ps_flowi_validate_aux_flow_spec() */
        return -1;
      }
    }

    /*-----------------------------------------------------------------------
      Validate Rx fltr spec
    -----------------------------------------------------------------------*/
    if (ps_flowi_validate_fltr_spec
        (
          &(qos_req_param->qos_spec->rx.fltr_template),
          iface_addr_family,
          qos_req_param->fltr_validate_f_ptr,
          QOS_MASK_RX_FLOW,
          &is_qos_spec_valid,
          ps_errno
        ) == -1)
    {
      LOG_MSG_INFO1_0("ps_flowi_validate_qos_spec(): "
                      "Invalid QOS Spec, Rx fltr spec is invalid");
      /* ps_errno is populated in ps_flowi_validate_fltr_spec() */
      return -1;
    }
  }
  else
  {
    /*-----------------------------------------------------------------------
      Validate that RX_MIN_FLOW or RX_AUXILIARY_FLOWS is not specified. It is
      mandatory to specify required flow spec if app wants to use QOS in a
      direction
    -----------------------------------------------------------------------*/
    if (qos_req_param->qos_spec->field_mask & QOS_MASK_RX_MIN_FLOW)
    {
      LOG_MSG_INFO1_1("ps_flowi_validate_qos_spec(): "
                      "RX_MIN_FLOW is specified without RX_FLOW, mask = %d",
                      qos_req_param->qos_spec->field_mask);
      *ps_errno = DS_EFAULT;
      return -1;
    }

    if (qos_req_param->qos_spec->field_mask & QOS_MASK_RX_AUXILIARY_FLOWS)
    {
      LOG_MSG_INFO1_1("ps_flowi_validate_qos_spec(): "
                      "RX_AUXILIARY_FLOWS is specified without RX_FLOW, mask = %d",
                      qos_req_param->qos_spec->field_mask);
      *ps_errno = DS_EFAULT;
      return -1;
    }
  }

  /*-------------------------------------------------------------------------
    Validate Tx flow spec and Tx fltr spec
  -------------------------------------------------------------------------*/
  if (qos_req_param->qos_spec->field_mask & QOS_MASK_TX_FLOW)
  {
    /*-----------------------------------------------------------------------
      Validate Tx required flow spec
    -----------------------------------------------------------------------*/
    is_qos_spec_valid &=
      ps_flowi_validate_qos_flow_param
      (
        &(qos_req_param->qos_spec->tx.flow_template.req_flow),
        QOS_MASK_TX_FLOW
      );

    if (qos_req_param->flow_validate_f_ptr)
    {
      is_qos_spec_valid &=
        qos_req_param->flow_validate_f_ptr
        (
          &(qos_req_param->qos_spec->tx.flow_template.req_flow),
          QOS_MASK_TX_FLOW
        );
    }

    /*-----------------------------------------------------------------------
      Validate Tx minimum required flow spec
    -----------------------------------------------------------------------*/
    if (qos_req_param->qos_spec->field_mask & QOS_MASK_TX_MIN_FLOW)
    {
      is_qos_spec_valid &=
        ps_flowi_validate_qos_flow_param
        (
          &(qos_req_param->qos_spec->tx.flow_template.min_req_flow),
          QOS_MASK_TX_MIN_FLOW
        );

      if (qos_req_param->flow_validate_f_ptr)
      {
        is_qos_spec_valid &=
          qos_req_param->flow_validate_f_ptr
          (
            &(qos_req_param->qos_spec->tx.flow_template.min_req_flow),
            QOS_MASK_TX_MIN_FLOW
          );
      }
    }

    /*-----------------------------------------------------------------------
      Validate auxiliary Tx flow specs
    -----------------------------------------------------------------------*/
    if (qos_req_param->qos_spec->field_mask & QOS_MASK_TX_AUXILIARY_FLOWS)
    {
      if (ps_flowi_validate_aux_flow_spec
          (
            &(qos_req_param->qos_spec->tx.flow_template),
            qos_req_param->flow_validate_f_ptr,
            QOS_MASK_TX_AUXILIARY_FLOWS,
            &is_qos_spec_valid,
            ps_errno
          ) == -1)
      {
        LOG_MSG_INFO1_0("ps_flowi_validate_qos_spec(): "
                        "Invalid QOS Spec, Tx aux flow spec is invalid");
        /* ps_errno is populated in ps_flowi_validate_aux_flow_spec() */
        return -1;
      }
    }

    /*-----------------------------------------------------------------------
      Validate Tx fltr spec
    -----------------------------------------------------------------------*/
    if (ps_flowi_validate_fltr_spec
        (
          &(qos_req_param->qos_spec->tx.fltr_template),
          iface_addr_family,
          qos_req_param->fltr_validate_f_ptr,
          QOS_MASK_TX_FLOW,
          &is_qos_spec_valid,
          ps_errno
        ) == -1)
    {
      LOG_MSG_INFO1_0("ps_flowi_validate_qos_spec(): "
                      "Invalid QOS Spec, Tx fltr spec is invalid");
      /* ps_errno is populated in ps_flowi_validate_fltr_spec() */
      return -1;
    }
  }
  else /* if TX_FLOW is not specified */
  {
    /*-----------------------------------------------------------------------
      Validate that TX_MIN_FLOW or TX_AUXILIARY_FLOWS is not specified. It is
      mandatory to specify required flow spec if app wants to use QOS in a
      direction
    -----------------------------------------------------------------------*/
    if (qos_req_param->qos_spec->field_mask & QOS_MASK_TX_MIN_FLOW)
    {
      LOG_MSG_INFO1_1("ps_flowi_validate_qos_spec(): "
                      "TX_MIN_FLOW is specified without TX_FLOW, mask = %d",
                      qos_req_param->qos_spec->field_mask);
      *ps_errno = DS_EFAULT;
      return -1;
    }

    if (qos_req_param->qos_spec->field_mask & QOS_MASK_TX_AUXILIARY_FLOWS)
    {
      LOG_MSG_INFO1_1("ps_flowi_validate_qos_spec(): "
                      "TX_AUXILIARY_FLOWS is specified without TX_FLOW, mask = %d",
                      qos_req_param->qos_spec->field_mask);
      *ps_errno = DS_EFAULT;
      return -1;
    }
  }

  if (is_qos_spec_valid == FALSE)
  {
    LOG_MSG_INFO1_0("ps_flowi_validate_qos_spec(): "
                    "QOS Spec validation failed");
    *ps_errno = DS_EFAULT;
    return -1;
  }

  return 0;

} /* ps_flowi_validate_qos_spec() */



/*===========================================================================
FUNCTION PS_FLOWI_VALIDATE_QOS_MODIFY_SPEC()

DESCRIPTION
  This function validates the modified QOS spec. If modified, flow spec and
  filter spec are validated in each direction. A generic validation is
  performed on the parameters. If mode handlers require specific validation
  they can pass an optional validation function ptr to perform extended
  validations. Note that generic validations will still be performed before
  calling extended validaiton functions.

PARAMETERS
  flow_ptr          : flow to be modified
  qos_modify_param  : structure containing qos_spec and information necessary
                      to validate it.
  iface_addr_family : addr family of iface to which "ps_flow" belongs to
  ps_errno          : specific error code in case operation fails

RETURN VALUE
   0 : if QOS_MODIFY spec is valid
  -1 : otherwise

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int32 ps_flowi_validate_qos_modify_spec
(
  ps_flow_type                     * flow_ptr,
  const ps_flow_modify_param_type  * qos_modify_param,
  ps_iface_addr_family_type          iface_addr_family,
  int16                            * ps_errno
)
{
  qos_spec_field_mask_type  qos_modify_mask;
  boolean                   is_qos_spec_valid = TRUE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(iface_addr_family != IFACE_INVALID_ADDR_FAMILY);

  if (ps_flowi_is_qos_modify_spec_well_formed(flow_ptr,
                                              qos_modify_param->qos_spec,
                                              ps_errno) == -1)
  {
    LOG_MSG_INFO1_1("ps_flowi_validate_qos_modify_spec(): "
                    "QOS spec in QOS_MODIFY is not well formed, ps_flow 0x%p",
                    flow_ptr);
    return -1;
  }

  qos_modify_mask = qos_modify_param->qos_spec->field_mask;

  /*-------------------------------------------------------------------------
    Validate Rx QOS spec, if it is added/modified. Setting QOS_MASK_RX_FLOW
    and field_mask to 0 indicates that app doesn't want Rx QOS any more
  -------------------------------------------------------------------------*/
  if (qos_modify_mask & QOS_MASK_RX_FLOW)
  {
    if (qos_modify_param->qos_spec->rx.flow_template.req_flow.field_mask !=
          IPFLOW_MASK_NONE)
    {
      /*---------------------------------------------------------------------
        Validate Rx required flow spec
      ---------------------------------------------------------------------*/
      is_qos_spec_valid &=
        ps_flowi_validate_qos_flow_param
        (
          &(qos_modify_param->qos_spec->rx.flow_template.req_flow),
          QOS_MASK_RX_FLOW
        );

      if (qos_modify_param->flow_validate_f_ptr)
      {
        is_qos_spec_valid &=
          qos_modify_param->flow_validate_f_ptr
          (
            &(qos_modify_param->qos_spec->rx.flow_template.req_flow),
            QOS_MASK_RX_FLOW
          );
      }

      /*---------------------------------------------------------------------
        Validate Rx minimum required flow spec
      ---------------------------------------------------------------------*/
      if (qos_modify_mask & QOS_MASK_RX_MIN_FLOW)
      {
        is_qos_spec_valid &=
          ps_flowi_validate_qos_flow_param
          (
            &(qos_modify_param->qos_spec->rx.flow_template.min_req_flow),
            QOS_MASK_RX_MIN_FLOW
          );

        if (qos_modify_param->flow_validate_f_ptr)
        {
          is_qos_spec_valid &=
            qos_modify_param->flow_validate_f_ptr
            (
              &(qos_modify_param->qos_spec->rx.flow_template.min_req_flow),
              QOS_MASK_RX_MIN_FLOW
            );
        }
      }

      /*---------------------------------------------------------------------
        Validate auxiliary Rx flow specs
      ---------------------------------------------------------------------*/
      if (qos_modify_param->qos_spec->field_mask &
            QOS_MASK_RX_AUXILIARY_FLOWS)
      {
        if (ps_flowi_validate_aux_flow_spec
            (
              &(qos_modify_param->qos_spec->rx.flow_template),
              qos_modify_param->flow_validate_f_ptr,
              QOS_MASK_RX_AUXILIARY_FLOWS,
              &is_qos_spec_valid,
              ps_errno
            ) == -1)
        {
          LOG_MSG_INFO1_0("ps_flowi_validate_qos_modify_spec(): "
                          "Invalid QOS_MODIFY Spec, Rx aux flow spec is invalid");
          /* ps_errno is populated in ps_flowi_validate_aux_flow_spec() */
          return -1;
        }
      }
    }
  }
  else /* if RX_FLOW is not specified */
  {
    /*-----------------------------------------------------------------------
      Validate that RX_MIN_FLOW or RX_AUXILIARY_FLOWS is not specified. It is
      mandatory to specify required flow spec if app wants to use QOS in a
      direction
    -----------------------------------------------------------------------*/
    if (qos_modify_param->qos_spec->field_mask & QOS_MASK_RX_MIN_FLOW)
    {
      LOG_MSG_INFO1_1("ps_flowi_validate_qos_modify_spec(): "
                      "RX_MIN_FLOW is specified without RX_FLOW, mask = %d",
                      qos_modify_param->qos_spec->field_mask);
      *ps_errno = DS_EFAULT;
      return -1;
    }

    if (qos_modify_param->qos_spec->field_mask & QOS_MASK_RX_AUXILIARY_FLOWS)
    {
      LOG_MSG_INFO1_1("ps_flowi_validate_qos_modify_spec(): "
                      "RX_AUXILIARY_FLOWS is specified without RX_FLOW, mask = %d",
                      qos_modify_param->qos_spec->field_mask);
      *ps_errno = DS_EFAULT;
      return -1;
    }
  }

  /*-------------------------------------------------------------------------
    Validate Rx fltr spec if it is either newly added or modified
  -------------------------------------------------------------------------*/
  if (((qos_modify_mask & QOS_MASK_RX_FLOW) &&
       !(PS_FLOWI_GET_QOS_FIELD_MASK(flow_ptr) & QOS_MASK_RX_FLOW)) ||
      (qos_modify_mask & QOS_MODIFY_MASK_RX_FLTR_MODIFY))
  {
    if (ps_flowi_validate_fltr_spec
        (
          &(qos_modify_param->qos_spec->rx.fltr_template),
          iface_addr_family,
          qos_modify_param->fltr_validate_f_ptr,
          QOS_MASK_RX_FLOW,
          &is_qos_spec_valid,
          ps_errno
        ) == -1)
    {
      LOG_MSG_INFO1_0("ps_flowi_validate_qos_modify_spec(): "
                      "Invalid QOS_MODIFY Spec, Rx fltr spec is invalid");
      /* ps_errno is populated in ps_flowi_validate_fltr_spec() */
      return -1;
    }
  }

  /*-------------------------------------------------------------------------
    Validate Tx QOS spec, if it is added/modified. Setting QOS_MASK_TX_FLOW
    and field_mask to 0 indicates that app doesn't want Tx QOS any more
  -------------------------------------------------------------------------*/
  if (qos_modify_mask & QOS_MASK_TX_FLOW)
  {
    if (qos_modify_param->qos_spec->tx.flow_template.req_flow.field_mask !=
          IPFLOW_MASK_NONE)
    {
      /*---------------------------------------------------------------------
        Validate Tx required flow spec
      ---------------------------------------------------------------------*/
      is_qos_spec_valid &=
        ps_flowi_validate_qos_flow_param
        (
          &(qos_modify_param->qos_spec->tx.flow_template.req_flow),
          QOS_MASK_TX_FLOW
        );

      if (qos_modify_param->flow_validate_f_ptr)
      {
        is_qos_spec_valid &=
          qos_modify_param->flow_validate_f_ptr
          (
            &(qos_modify_param->qos_spec->tx.flow_template.req_flow),
            QOS_MASK_TX_FLOW
          );
      }

      /*---------------------------------------------------------------------
        Validate Tx minimum required flow spec
      ---------------------------------------------------------------------*/
      if (qos_modify_mask & QOS_MASK_TX_MIN_FLOW)
      {
        is_qos_spec_valid &=
          ps_flowi_validate_qos_flow_param
          (
            &(qos_modify_param->qos_spec->tx.flow_template.min_req_flow),
            QOS_MASK_TX_MIN_FLOW
          );

        if (qos_modify_param->flow_validate_f_ptr)
        {
          is_qos_spec_valid &=
            qos_modify_param->flow_validate_f_ptr
            (
              &(qos_modify_param->qos_spec->tx.flow_template.min_req_flow),
              QOS_MASK_TX_MIN_FLOW
            );
        }
      }

      /*---------------------------------------------------------------------
        Validate auxiliary Tx flow specs
      ---------------------------------------------------------------------*/
      if (qos_modify_param->qos_spec->field_mask &
            QOS_MASK_TX_AUXILIARY_FLOWS)
      {
        if (ps_flowi_validate_aux_flow_spec
            (
              &(qos_modify_param->qos_spec->tx.flow_template),
              qos_modify_param->flow_validate_f_ptr,
              QOS_MASK_TX_AUXILIARY_FLOWS,
              &is_qos_spec_valid,
              ps_errno
            ) == -1)
        {
          LOG_MSG_INFO1_0("ps_flowi_validate_qos_modify_spec(): "
                          "Invalid QOS_MODIFY Spec, Tx aux flow spec is invalid");
          /* ps_errno is populated in ps_flowi_validate_aux_flow_spec() */
          return -1;
        }
      }
    } /* if Tx flow spec is not deleted */
  }
  else /* if TX_FLOW is not specified */
  {
    /*-----------------------------------------------------------------------
      Validate that TX_MIN_FLOW or TX_AUXILIARY_FLOWS is not specified. It is
      mandatory to specify required flow spec if app wants to use QOS in a
      direction
    -----------------------------------------------------------------------*/
    if (qos_modify_param->qos_spec->field_mask & QOS_MASK_TX_MIN_FLOW)
    {
      LOG_MSG_INFO1_1("ps_flowi_validate_qos_modify_spec(): "
                      "TX_MIN_FLOW is specified without TX_FLOW, mask = %d",
                      qos_modify_param->qos_spec->field_mask);
      *ps_errno = DS_EFAULT;
      return -1;
    }

    if (qos_modify_param->qos_spec->field_mask & QOS_MASK_TX_AUXILIARY_FLOWS)
    {
      LOG_MSG_INFO1_1("ps_flowi_validate_qos_modify_spec(): "
                      "TX_AUXILIARY_FLOWS is specified without TX_FLOW, mask = %d",
                      qos_modify_param->qos_spec->field_mask);
      *ps_errno = DS_EFAULT;
      return -1;
    }
  }

  /*-------------------------------------------------------------------------
    Validate Tx fltr spec if it is either newly added or modified
  -------------------------------------------------------------------------*/
  if (((qos_modify_mask & QOS_MASK_TX_FLOW) &&
       !(PS_FLOWI_GET_QOS_FIELD_MASK(flow_ptr) & QOS_MASK_TX_FLOW)) ||
      (qos_modify_mask & QOS_MODIFY_MASK_TX_FLTR_MODIFY))
  {
    if (ps_flowi_validate_fltr_spec
        (
          &(qos_modify_param->qos_spec->tx.fltr_template),
          iface_addr_family,
          qos_modify_param->fltr_validate_f_ptr,
          QOS_MASK_TX_FLOW,
          &is_qos_spec_valid,
          ps_errno
        ) == -1)
    {
      LOG_MSG_INFO1_0("ps_flowi_validate_qos_modify_spec(): "
                      "Invalid QOS_MODIFY Spec, Tx fltr spec is invalid");
      /* ps_errno is populated in ps_flowi_validate_fltr_spec() */
      return -1;
    }
  }

  if (is_qos_spec_valid == FALSE)
  {
    LOG_MSG_INFO1_0("ps_flowi_validate_qos_modify_spec(): "
                    "QOS_MODIFY Spec validation failed");
    *ps_errno = DS_EFAULT;
    return -1;
  }

  return 0;

} /* ps_flowi_validate_qos_modify_spec() */



/*===========================================================================
FUNCTION PS_FLOWI_VALIDATE_PRIMARY_QOS_MODIFY_SPEC()

DESCRIPTION
  This function validates primary QOS spec. Flow spec is validated in each
  direction. A generic validation is performed on the parameters and if mode
  handlers require extra validation they can pass an optional validation
  function ptr to perform extended validation. Note that generic validation
  will still be performed before calling extended validaiton functions.

PARAMETERS
  primary_qos_spec_ptr     : Primary QoS spec to be validated
  flow_validate_f_ptr      : Optional extended validation function
  requested_qos_field_mask : Field mask indicating the QoS that was requested
                             previously
  ps_errno                 : specific error code in case operation fails

RETURN VALUE
   0 : if primary QOS spec is valid
  -1 : otherwise

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_flowi_validate_primary_qos_modify_spec
(
  primary_qos_spec_type              * primary_qos_spec_ptr,
  qos_flow_validation_ex_f_ptr_type    flow_validate_f_ptr,
  qos_spec_field_mask_type             requested_qos_field_mask,
  int16                              * ps_errno
)
{
  boolean  is_qos_spec_valid = TRUE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO2_0("ps_flowi_validate_primary_qos_modify_spec(): "
                  "Validating primary qos spec");

  /*-------------------------------------------------------------------------
    Validate that PRIMARY_QOS_MODIFY is well formed. Rules are
      1. Some operation must be performed on atleast one of two blocks,
         i.e Tx flow, and Rx flow
      2. Reserved bits in qos_modify_mask must not be turned on
      3. If Tx flow is deleted, ensure that it was requested earlier. Also
         ensure that Tx min flow and Tx auxiliary flows are not specified.
         Also ensure that Rx flow either was requested earlier or is
         requested now
      4. Rule 3 apply to Rx flow also
      5. Both Tx flow and Rx flow can't be deleted
  -------------------------------------------------------------------------*/

  /*-------------------------------------------------------------------------
    One of Rx or Tx flow specs must be specified
  -------------------------------------------------------------------------*/
  if (!(primary_qos_spec_ptr->field_mask & QOS_MASK_RX_FLOW) &&
      !(primary_qos_spec_ptr->field_mask & QOS_MASK_TX_FLOW))
  {
    LOG_MSG_INFO1_1("ps_flowi_validate_primary_qos_modify_spec(): "
                    "Invalid primary QOS spec, mask=0x%x, specify Rx or Tx flow",
                    primary_qos_spec_ptr->field_mask);
    *ps_errno = DS_EFAULT;
    return -1;
  }

  /*-------------------------------------------------------------------------
    If a bit mask is specified, which is outside the supported range, flag
    error
  -------------------------------------------------------------------------*/
  if (primary_qos_spec_ptr->field_mask & ~QOS_MASK_ALL)
  {
    LOG_MSG_INFO1_1("ps_flowi_validate_primary_qos_modify_spec(): "
                    "Invalid primary QOS spec, mask=0x%x, Bits outside the allowed "
                    "range are set", primary_qos_spec_ptr->field_mask);
    *ps_errno = DS_EFAULT;
    return -1;
  }

  /*-------------------------------------------------------------------------
    Validate Rule 3 for Tx flow spec
  -------------------------------------------------------------------------*/
  if (primary_qos_spec_ptr->field_mask & QOS_MASK_TX_FLOW)
  {
    if (primary_qos_spec_ptr->tx_flow_template.req_flow.field_mask ==
          IPFLOW_MASK_NONE)   /* if Tx flow is deleted */
    {
      if (!(requested_qos_field_mask & QOS_MASK_TX_FLOW))
      {
        LOG_MSG_INFO1_0("ps_flowi_validate_primary_qos_modify_spec(): "
                        "Deleting Tx flow spec when it was not requested "
                        "previously");
        *ps_errno = DS_EFAULT;
        return -1;
      }

      if (primary_qos_spec_ptr->field_mask &
            (QOS_MASK_TX_MIN_FLOW | QOS_MASK_TX_AUXILIARY_FLOWS))
      {
        LOG_MSG_INFO1_0("ps_flowi_validate_primary_qos_modify_spec(): "
                        "Deleting Tx flow spec while specifying one of Tx min "
                        "flow spec or Tx auxiliary flow specs");
        *ps_errno = DS_EFAULT;
        return -1;
      }

      /*---------------------------------------------------------------------
        A QOS spec must have flow spec in at least one direction. So flag
        error if Rx flow is not requested now and wasn't requested earlier.
        Also flag error if Rx flow is also deleted.
      ---------------------------------------------------------------------*/
      if ((!(requested_qos_field_mask & QOS_MASK_RX_FLOW) &&
             !(primary_qos_spec_ptr->field_mask & QOS_MASK_RX_FLOW)) ||
           ((primary_qos_spec_ptr->field_mask & QOS_MASK_RX_FLOW) &&
              (primary_qos_spec_ptr->rx_flow_template.req_flow.field_mask ==
                IPFLOW_MASK_NONE)))
      {
        LOG_MSG_INFO1_0("ps_flowi_validate_primary_qos_modify_spec(): "
                        "Can't have QOS spec with out both Tx and Rx flow specs");
        *ps_errno = DS_EFAULT;
        return -1;
      }
    } /* if Tx flow is deleted */
  } /* Some operation is performed on Tx flow spec */

  /*-------------------------------------------------------------------------
    Validate Rule 3 for Rx flow spec
  -------------------------------------------------------------------------*/
  if (primary_qos_spec_ptr->field_mask & QOS_MASK_RX_FLOW)
  {
    if (primary_qos_spec_ptr->rx_flow_template.req_flow.field_mask ==
          IPFLOW_MASK_NONE)   /* if Rx flow is deleted */
    {
      if (!(requested_qos_field_mask & QOS_MASK_RX_FLOW))
      {
        LOG_MSG_INFO1_0("ps_flowi_validate_primary_qos_modify_spec(): "
                        "Deleting Rx flow spec when it was not requested "
                        "previously");
        *ps_errno = DS_EFAULT;
        return -1;
      }

      if (primary_qos_spec_ptr->field_mask &
            (QOS_MASK_RX_MIN_FLOW | QOS_MASK_RX_AUXILIARY_FLOWS))
      {
        LOG_MSG_INFO1_0("ps_flowi_validate_primary_qos_modify_spec(): "
                        "Deleting Rx flow spec while specifying one of Rx min "
                        "flow spec or Rx auxiliary flow specs");
        *ps_errno = DS_EFAULT;
        return -1;
      }

      /*---------------------------------------------------------------------
        A QOS spec must have flow spec in at least one direction. So flag
        error if Tx flow is not requested now and wasn't requested earlier.
        Also flag error if Tx flow is also deleted.
      ---------------------------------------------------------------------*/
      if ((!(requested_qos_field_mask & QOS_MASK_TX_FLOW) &&
             !(primary_qos_spec_ptr->field_mask & QOS_MASK_TX_FLOW)) ||
           ((primary_qos_spec_ptr->field_mask & QOS_MASK_TX_FLOW) &&
              (primary_qos_spec_ptr->tx_flow_template.req_flow.field_mask ==
                IPFLOW_MASK_NONE)))
      {
        LOG_MSG_INFO1_0("ps_flowi_validate_primary_qos_modify_spec(): "
                        "Can't have QOS spec with out both Tx and Rx flow specs");
        *ps_errno = DS_EFAULT;
        return -1;
      }
    } /* if Rx flow is deleted */
  } /* Some operation is performed on Rx flow spec */

  /*-------------------------------------------------------------------------
    Validate Rx flow spec
  -------------------------------------------------------------------------*/
  if (primary_qos_spec_ptr->field_mask & QOS_MASK_RX_FLOW)
  {
    /*-----------------------------------------------------------------------
      Validate Rx required flow spec
    -----------------------------------------------------------------------*/
    is_qos_spec_valid &=
      ps_flowi_validate_qos_flow_param
      (
        &(primary_qos_spec_ptr->rx_flow_template.req_flow),
        QOS_MASK_RX_FLOW
      );

    if (flow_validate_f_ptr)
    {
      is_qos_spec_valid &=
        flow_validate_f_ptr(&(primary_qos_spec_ptr->rx_flow_template.req_flow),
                            QOS_MASK_RX_FLOW);
    }

    /*-----------------------------------------------------------------------
      Validate Rx minimum required flow spec
    -----------------------------------------------------------------------*/
    if (primary_qos_spec_ptr->field_mask & QOS_MASK_RX_MIN_FLOW)
    {
      is_qos_spec_valid &=
        ps_flowi_validate_qos_flow_param
        (
          &(primary_qos_spec_ptr->rx_flow_template.min_req_flow),
          QOS_MASK_RX_MIN_FLOW
        );

      if (flow_validate_f_ptr)
      {
        is_qos_spec_valid &=
          flow_validate_f_ptr
          (
            &(primary_qos_spec_ptr->rx_flow_template.min_req_flow),
            QOS_MASK_RX_MIN_FLOW
          );
      }
    }

    /*-----------------------------------------------------------------------
      Validate auxiliary Rx flow specs
    -----------------------------------------------------------------------*/
    if (primary_qos_spec_ptr->field_mask & QOS_MASK_RX_AUXILIARY_FLOWS)
    {
      if (ps_flowi_validate_aux_flow_spec
          (
            &(primary_qos_spec_ptr->rx_flow_template),
            flow_validate_f_ptr,
            QOS_MASK_RX_AUXILIARY_FLOWS,
            &is_qos_spec_valid,
            ps_errno
          ) == -1)
      {
        LOG_MSG_INFO1_0("ps_flowi_validate_primary_qos_modify_spec(): "
                        "Invalid QOS Spec, Rx aux flow spec is invalid");
        /* ps_errno is populated in ps_flowi_validate_aux_flow_spec() */
        return -1;
      }
    }
  }
  else
  {
    /*-----------------------------------------------------------------------
      Validate that RX_MIN_FLOW or RX_AUXILIARY_FLOWS is not specified. It is
      mandatory to specify required flow spec if app wants to use QOS in a
      direction
    -----------------------------------------------------------------------*/
    if (primary_qos_spec_ptr->field_mask & QOS_MASK_RX_MIN_FLOW)
    {
      LOG_MSG_INFO1_1("ps_flowi_validate_primary_qos_modify_spec(): "
                      "RX_MIN_FLOW is specified without RX_FLOW, mask = 0x%x",
                      primary_qos_spec_ptr->field_mask);
      *ps_errno = DS_EFAULT;
      return -1;
    }

    if (primary_qos_spec_ptr->field_mask & QOS_MASK_RX_AUXILIARY_FLOWS)
    {
      LOG_MSG_INFO1_1("ps_flowi_validate_primary_qos_modify_spec(): "
                      "RX_AUXILIARY_FLOWS is specified without RX_FLOW, mask = 0x%x",
                      primary_qos_spec_ptr->field_mask);
      *ps_errno = DS_EFAULT;
      return -1;
    }
  }

  /*-------------------------------------------------------------------------
    Validate Tx flow spec
  -------------------------------------------------------------------------*/
  if (primary_qos_spec_ptr->field_mask & QOS_MASK_TX_FLOW)
  {
    /*-----------------------------------------------------------------------
      Validate Tx required flow spec
    -----------------------------------------------------------------------*/
    is_qos_spec_valid &=
      ps_flowi_validate_qos_flow_param
      (
        &(primary_qos_spec_ptr->tx_flow_template.req_flow),
        QOS_MASK_TX_FLOW
      );

    if (flow_validate_f_ptr)
    {
      is_qos_spec_valid &=
        flow_validate_f_ptr(&(primary_qos_spec_ptr->tx_flow_template.req_flow),
                            QOS_MASK_TX_FLOW);
    }

    /*-----------------------------------------------------------------------
      Validate Tx minimum required flow spec
    -----------------------------------------------------------------------*/
    if (primary_qos_spec_ptr->field_mask & QOS_MASK_TX_MIN_FLOW)
    {
      is_qos_spec_valid &=
        ps_flowi_validate_qos_flow_param
        (
          &(primary_qos_spec_ptr->tx_flow_template.min_req_flow),
          QOS_MASK_TX_MIN_FLOW
        );

      if (flow_validate_f_ptr)
      {
        is_qos_spec_valid &=
          flow_validate_f_ptr
          (
            &(primary_qos_spec_ptr->tx_flow_template.min_req_flow),
            QOS_MASK_TX_MIN_FLOW
          );
      }
    }

    /*-----------------------------------------------------------------------
      Validate auxiliary Tx flow specs
    -----------------------------------------------------------------------*/
    if (primary_qos_spec_ptr->field_mask & QOS_MASK_TX_AUXILIARY_FLOWS)
    {
      if (ps_flowi_validate_aux_flow_spec
          (
            &(primary_qos_spec_ptr->tx_flow_template),
            flow_validate_f_ptr,
            QOS_MASK_TX_AUXILIARY_FLOWS,
            &is_qos_spec_valid,
            ps_errno
          ) == -1)
      {
        LOG_MSG_INFO1_0("ps_flowi_validate_primary_qos_modify_spec(): "
                        "Invalid QOS Spec, Tx aux flow spec is invalid");
        /* ps_errno is populated in ps_flowi_validate_aux_flow_spec() */
        return -1;
      }
    }
  }
  else /* if TX_FLOW is not specified */
  {
    /*-----------------------------------------------------------------------
      Validate that TX_MIN_FLOW or TX_AUXILIARY_FLOWS is not specified. It is
      mandatory to specify required flow spec if app wants to use QOS in a
      direction
    -----------------------------------------------------------------------*/
    if (primary_qos_spec_ptr->field_mask & QOS_MASK_TX_MIN_FLOW)
    {
      LOG_MSG_INFO1_1("ps_flowi_validate_primary_qos_modify_spec(): "
                      "TX_MIN_FLOW is specified without TX_FLOW, mask = 0x%x",
                      primary_qos_spec_ptr->field_mask);
      *ps_errno = DS_EFAULT;
      return -1;
    }

    if (primary_qos_spec_ptr->field_mask & QOS_MASK_TX_AUXILIARY_FLOWS)
    {
      LOG_MSG_INFO1_1("ps_flowi_validate_primary_qos_modify_spec(): "
                      "TX_AUXILIARY_FLOWS is specified without TX_FLOW, mask = 0x%x",
                      primary_qos_spec_ptr->field_mask);
      *ps_errno = DS_EFAULT;
      return -1;
    }
  }

  if (is_qos_spec_valid == FALSE)
  {
    LOG_MSG_INFO1_0("ps_flowi_validate_primary_qos_modify_spec(): "
                    "QOS Spec validation failed");
    *ps_errno = DS_EFAULT;
    return -1;
  }

  return 0;

} /* ps_flow_validate_primary_qos_modify_spec() */



/*===========================================================================
FUNCTION PS_FLOWI_STORE_QOS_SPEC()

DESCRIPTION
  This function stores the requested flow spec, and Rx filter spec in the
  flow.

PARAMETERS
  flow_ptr           : flow where part of QOS spec is to be stored.
  qos_spec           : QOS spec to be stored.
  fltr_priority      : priority class of filters
  fltr_compare_f_ptr : fltr comparator
  ps_errno           : error code passed back when storage fails.

RETURN VALUE
   0 : on success
  -1 : on failure

DEPENDENCIES
  Must be called inside TASKLOCK()

SIDE EFFECTS
  If error occurs during storage, none of QOS spec is stored, i.e semantics
  is all or nothing
===========================================================================*/
int ps_flowi_store_qos_spec
(
  ps_flow_type                  * flow_ptr,
  const qos_spec_type           * qos_spec,
  fltr_priority_enum_type         fltr_priority,
  ipfltr_comparison_f_ptr_type    fltr_compare_f_ptr,
  int16                         * ps_errno
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ASSERT(ps_errno);
  if (qos_spec == NULL)
  {
    DATA_ERR_FATAL("ps_flowi_store_qos_spec(): QOS_SPEC is NULL");
    *ps_errno = DS_EFAULT;
    return -1;
  }

  if (flow_ptr == NULL || flow_ptr->flow_private.qos_info_ptr == NULL)
  {
    DATA_ERR_FATAL("ps_flowi_store_qos_spec(): flow_ptr or qos_info_ptr is NULL");
    *ps_errno = DS_EFAULT;
    return -1;
  }

  if (fltr_priority != PS_IFACE_IPFLTR_PRIORITY_FCFS)
  {
    LOG_MSG_ERROR_1("ps_flowi_store_qos_spec(): "
                    "Invalid fltr priority style, %d, is passed",
                    fltr_priority);
    ASSERT(0);
    *ps_errno = DS_EFAULT;
    return -1;
  }

  if (PS_FLOWI_GET_QOS_FIELD_MASK(flow_ptr) != QOS_MASK_INVALID)
  {
    LOG_MSG_ERROR_1("ps_flowi_store_qos_spec(): "
                    "Already stored QOS Spec on ps_flow 0x%p", flow_ptr);
    ASSERT(0);
    *ps_errno = DS_EFAULT;
    return -1;
  }

  /*-------------------------------------------------------------------------
    Store Rx and/or Tx flow specs in ps_flow
  -------------------------------------------------------------------------*/
  if (ps_flowi_store_flow_spec(flow_ptr->flow_private.qos_info_ptr,
                               qos_spec,
                               ps_errno) == -1)
  {
    LOG_MSG_ERROR_1("ps_flowi_store_qos_spec(): "
                    "Flow 0x%p: Couldn't store flow spec", flow_ptr);
    (void) ps_flowi_delete_qos_spec(flow_ptr);
    return -1;
  }

  /*-------------------------------------------------------------------------
    Store Rx fltr spec if Rx QOS is specified
  -------------------------------------------------------------------------*/
  if (qos_spec->field_mask & QOS_MASK_RX_FLOW)
  {
    if (ps_flowi_store_rx_fltr_spec(flow_ptr->flow_private.qos_info_ptr,
                                    qos_spec,
                                    ps_errno) == -1)
    {
      LOG_MSG_ERROR_1("ps_flowi_store_qos_spec(): "
                      "Flow 0x%p: Couldn't store Rx filter spec", flow_ptr);
      (void) ps_flowi_delete_qos_spec(flow_ptr);
      return -1;
    }
  }

  return 0;

} /* ps_flowi_store_qos_spec() */


/*===========================================================================
FUNCTION PS_FLOWI_STORE_QOS_MODIFY_SPEC()

DESCRIPTION
  This function stores the modified flow spec, and Rx filter spec in the
  flow.

PARAMETERS
  flow_ptr           : flow where part of QOS spec is to be stored.
  qos_modify_spec    : QOS spec to be stored.
  fltr_priority      : priority class of filters
  fltr_compare_f_ptr : fltr comparator
  ps_errno           : error code passed back when storage fails.

RETURN VALUE
   0 : on success
  -1 : on failure

DEPENDENCIES
  Must be called inside TASKLOCK()

SIDE EFFECTS
  If error occurs during storage, none of QOS spec is stored, i.e semantics
  is all or nothing
===========================================================================*/
int32 ps_flowi_store_qos_modify_spec
(
  ps_flow_type                        * flow_ptr,
  const qos_spec_type                 * qos_modify_spec_ptr,
  fltr_priority_enum_type               fltr_priority,
  ipfltr_comparison_f_ptr_type          fltr_compare_f_ptr,
  int16                               * ps_errno
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(ps_errno);
  if (qos_modify_spec_ptr == NULL)
  {
    DATA_ERR_FATAL("ps_flowi_store_qos_modify_spec(): QOS_MODIFY_SPEC is NULL");
    *ps_errno = DS_EFAULT;
    return -1;
  }

  if (fltr_priority != PS_IFACE_IPFLTR_PRIORITY_FCFS)
  {
    LOG_MSG_ERROR_1("ps_flowi_store_qos_modify_spec(): "
                    "Invalid fltr priority style, %d, is passed",
                    fltr_priority);
    ASSERT(0);
    *ps_errno = DS_EFAULT;
    return -1;
  }

  /*-------------------------------------------------------------------------
    There must be atmost only one pending QOS_MODIFY at any time
  -------------------------------------------------------------------------*/
  ASSERT(!PS_FLOWI_IS_QOS_MODIFY_IN_PROGRESS(flow_ptr));

  /*-------------------------------------------------------------------------
    Allocate ps_mem buffer to store modify QOS spec
  -------------------------------------------------------------------------*/
  flow_ptr->flow_private.qos_modify_info_ptr =
    (qos_info_type *) ps_mem_get_buf(PS_MEM_QOS_INFO_TYPE);

  if (flow_ptr->flow_private.qos_modify_info_ptr == NULL)
  {
    LOG_MSG_ERROR_1("ps_flowi_store_qos_modify_spec(): "
                    "Couldn't create qos_modify_info_ptr for flow, 0x%p",
                    flow_ptr);
    *ps_errno = DS_ENOMEM;
    return -1;
  }

  memset(flow_ptr->flow_private.qos_modify_info_ptr,
         0,
         sizeof(qos_info_type));

  /*-------------------------------------------------------------------------
    Store Rx and/or Tx modify flow specs in ps_flow
  -------------------------------------------------------------------------*/
  if (ps_flowi_store_flow_spec(flow_ptr->flow_private.qos_modify_info_ptr,
                               qos_modify_spec_ptr,
                               ps_errno) == -1)
  {
    LOG_MSG_ERROR_1("ps_flowi_store_qos_modify_spec(): "
                    "Flow 0x%p: Couldn't store modify_flow_spec", flow_ptr);
    (void) ps_flowi_delete_qos_modify_spec(flow_ptr);
    return -1;
  }

  /*-------------------------------------------------------------------------
    Store Rx modify fltr spec if Rx fltr spec is added/modified

    Since ps_flowi_store_flow_spec() stores qos_modify_mask in ps_flow, it is
    ok to use MACROs here
  -------------------------------------------------------------------------*/
  if (PS_FLOWI_IS_MODIFY_RX_FLTR_SPEC_CREATED(flow_ptr) ||
      PS_FLOWI_IS_MODIFY_RX_FLTR_SPEC_MODIFIED(flow_ptr))
  {
    if (ps_flowi_store_rx_fltr_spec
        (
          flow_ptr->flow_private.qos_modify_info_ptr,
          qos_modify_spec_ptr,
          ps_errno
        ) == -1)
    {
      LOG_MSG_ERROR_1("ps_flowi_store_qos_modify_spec(): "
                      "Flow 0x%p: Couldn't store modify_rx_fltr_spec",
                      flow_ptr);
      (void) ps_flowi_delete_qos_modify_spec(flow_ptr);
      return -1;
    }
  }

  return 0;

} /* ps_flowi_store_qos_modify_spec() */



/*===========================================================================
FUNCTION PS_FLOWI_DELETE_RX_FLTR_SPEC()

DESCRIPTION
  Deletes Rx fltr spec from specified location

PARAMETERS
  qos_info_ptr : Address where Rx fltr spec is stored. Could be qos_info_ptr
                 or modify_qos_info_ptr of ps_flow

RETURN VALUE
  None

DEPENDENCIES
  Parameters must be valid. Validation is not performed as this is an internal
  function

SIDE EFFECTS
  None
===========================================================================*/
void ps_flowi_delete_rx_fltr_spec
(
  qos_info_type  * qos_info_ptr
)
{
  ps_flow_rx_fltr_buf_type   * curr_fltr;
  ps_flow_rx_fltr_buf_type   * next_fltr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  curr_fltr = list_peek_front(&(qos_info_ptr->rx.fltr_list));
  while (curr_fltr != NULL)
  {
    next_fltr = list_peek_next(&(qos_info_ptr->rx.fltr_list),
                               &(curr_fltr->link));

    list_pop_item(&(qos_info_ptr->rx.fltr_list), &(curr_fltr->link));
    PS_MEM_FREE(curr_fltr);

    curr_fltr = next_fltr;
  } /* while there is a fltr in the list */

  list_destroy(&(qos_info_ptr->rx.fltr_list));

} /* ps_flowi_delete_rx_fltr_spec() */



/*===========================================================================
FUNCTION PS_FLOWI_DELETE_AUX_FLOW_SPEC()

DESCRIPTION
  Deletes auxiliary flow spec from specified location based on flow_type

PARAMETERS
  qos_info_ptr : Address where auxiliary flow spec is stored. Could be
                 qos_info_ptr or modify_qos_info_ptr of ps_flow
  flow_type    : Specifies direction of flow spec

RETURN VALUE
  None

DEPENDENCIES
  Parameters must be valid. Validation is not performed as this is an internal
  function

SIDE EFFECTS
  None
===========================================================================*/
void ps_flowi_delete_aux_flow_spec
(
  const qos_info_type            * qos_info_ptr,
  qos_spec_field_mask_enum_type    flow_type
)
{
  ps_flow_ip_flow_spec_type  * curr_flow_spec;
  ps_flow_ip_flow_spec_type  * next_flow_spec;
  list_type                    flow_spec_list;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (flow_type == QOS_MASK_RX_AUXILIARY_FLOWS)
  {
    flow_spec_list = qos_info_ptr->rx.ipflow.aux_flow_list;
  }
  else
  {
    flow_spec_list = qos_info_ptr->tx.ipflow.aux_flow_list;
  }

  curr_flow_spec = list_peek_front(&flow_spec_list);
  while (curr_flow_spec != NULL)
  {
    next_flow_spec = list_peek_next(&flow_spec_list, &(curr_flow_spec->link));

    list_pop_item(&flow_spec_list, &(curr_flow_spec->link));
    PS_MEM_FREE(curr_flow_spec);

    curr_flow_spec = next_flow_spec;
  } /* while flow specs exist */

  list_destroy(&flow_spec_list);

} /* ps_flowi_delete_aux_flow_spec() */



/*===========================================================================
FUNCTION PS_FLOWI_DELETE_QOS_SPEC()

DESCRIPTION
  This function deletes previously stored flow spec, Rx filter spec and
  frees up any memory buffers used.

PARAMETERS
  flow_ptr : ptr to flow, where flow spec is stored

RETURN VALUE
   0 : on success
  -1 : on failure

DEPENDENCIES
  Flow must not be a default flow as QOS is not supported on default flows.

SIDE EFFECTS
  None
===========================================================================*/
int ps_flowi_delete_qos_spec
(
  ps_flow_type  * flow_ptr
)
{
  qos_spec_field_mask_type     qos_field_mask;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(NULL == flow_ptr || NULL == flow_ptr->flow_private.qos_info_ptr)
  {
    DATA_ERR_FATAL("ps_flowi_delete_qos_spec(): QOS info pointer of flow pointer is NULL");
    return -1;
  }
  /*-------------------------------------------------------------------------
    Free auxiliary Rx flow specs and add them to ps_mem pool
  -------------------------------------------------------------------------*/
  qos_field_mask = PS_FLOWI_GET_QOS_FIELD_MASK(flow_ptr);

  if (qos_field_mask & QOS_MASK_RX_AUXILIARY_FLOWS)
  {
    ps_flowi_delete_aux_flow_spec(flow_ptr->flow_private.qos_info_ptr,
                                  QOS_MASK_RX_AUXILIARY_FLOWS);
  }

  /*-------------------------------------------------------------------------
    Free auxiliary Tx flow specs and add them to ps_mem pool
  -------------------------------------------------------------------------*/
  if (qos_field_mask & QOS_MASK_TX_AUXILIARY_FLOWS)
  {
    ps_flowi_delete_aux_flow_spec(flow_ptr->flow_private.qos_info_ptr,
                                  QOS_MASK_TX_AUXILIARY_FLOWS);
  }

  /*-------------------------------------------------------------------------
    Free Rx filters and add them to ps_mem pool
  -------------------------------------------------------------------------*/
  if (qos_field_mask & QOS_MASK_RX_FLOW)
  {
    ps_flowi_delete_rx_fltr_spec(flow_ptr->flow_private.qos_info_ptr);
  }

  /*-------------------------------------------------------------------------
    Free QoS spec
  -------------------------------------------------------------------------*/
  flow_ptr->flow_private.qos_info_ptr->field_mask = QOS_MASK_INVALID;
  PS_MEM_FREE(flow_ptr->flow_private.qos_info_ptr);

  LOG_MSG_INFO1_1("ps_flowi_delete_qos_spec(): "
                  "Flow 0x%p: QOS released", flow_ptr);

  return 0;

} /* ps_flowi_delete_qos_spec() */



/*===========================================================================
FUNCTION PS_FLOWI_DELETE_QOS_MODIFY_SPEC()

DESCRIPTION
  This function deletes previously stored modify_flow_spec, Rx filter spec and
  frees up any memory buffers used.

PARAMETERS
  flow_ptr : ptr to flow, where qos_modify_spec spec is stored

RETURN VALUE
   0 : on success
  -1 : on failure

DEPENDENCIES
  Flow must not be a default flow as QOS is not supported on default flows.
  rx.fltr_list of qos_modify_info_ptr must be initialized

SIDE EFFECTS
  None
===========================================================================*/
int32 ps_flowi_delete_qos_modify_spec
(
  ps_flow_type  * flow_ptr
)
{
  qos_spec_field_mask_type     qos_modify_mask;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(PS_FLOWI_IS_QOS_MODIFY_IN_PROGRESS(flow_ptr));

  qos_modify_mask = PS_FLOWI_GET_QOS_MODIFY_FIELD_MASK(flow_ptr);

  /*-------------------------------------------------------------------------
    Free auxiliary Rx flow specs and add them to ps_mem pool
  -------------------------------------------------------------------------*/
  if (qos_modify_mask & QOS_MASK_RX_AUXILIARY_FLOWS)
  {
    ps_flowi_delete_aux_flow_spec(flow_ptr->flow_private.qos_modify_info_ptr,
                                  QOS_MASK_RX_AUXILIARY_FLOWS);
  }

  /*-------------------------------------------------------------------------
    Free auxiliary Tx flow specs and add them to ps_mem pool
  -------------------------------------------------------------------------*/
  if (qos_modify_mask & QOS_MASK_TX_AUXILIARY_FLOWS)
  {
    ps_flowi_delete_aux_flow_spec(flow_ptr->flow_private.qos_modify_info_ptr,
                                  QOS_MASK_TX_AUXILIARY_FLOWS);
  }

  /*-------------------------------------------------------------------------
    Free Rx filters and add them to ps_mem pool
  -------------------------------------------------------------------------*/
  if (PS_FLOWI_IS_MODIFY_RX_FLTR_SPEC_CREATED(flow_ptr) ||
      PS_FLOWI_IS_MODIFY_RX_FLTR_SPEC_MODIFIED(flow_ptr))
  {
    ps_flowi_delete_rx_fltr_spec(flow_ptr->flow_private.qos_modify_info_ptr);
  }

  /*-------------------------------------------------------------------------
    Free QoS modify spec
  -------------------------------------------------------------------------*/
  flow_ptr->flow_private.qos_modify_info_ptr->field_mask = QOS_MASK_INVALID;
  PS_MEM_FREE(flow_ptr->flow_private.qos_modify_info_ptr);

  LOG_MSG_INFO1_1("ps_flowi_delete_qos_modify_spec(): "
                  "Flow 0x%p: QOS_MODIFY_SPEC released", flow_ptr);

  return 0;

} /* ps_flowi_delete_qos_modify_spec() */



/*===========================================================================
FUNCTION PS_FLOWI_GET_QOS_INFO_PTR()

DESCRIPTION
  Returns an address where QoS info is stored. This is different for
  QOS_REQUEST/active QoS, and pending QOS_MODIFY

PARAMETERS
  flow_ptr  : pointer to the flow serving the QOS
  is_modify : is the client interested in qos_info_ptr associated with
              pending QOS_MODIFY? If so value must be TRUE

RETURN VALUE
  qos_info_ptr : on success
  NULL         : otherwise

DEPENDENCIES
  Flow must be valid and must not be a default flow as QOS is not supported
  on default flows.

SIDE EFFECTS
  None
===========================================================================*/
qos_info_type *ps_flowi_get_qos_info_ptr
(
  ps_flow_type  * flow_ptr,
  boolean         is_modify
)
{
  qos_info_type  * qos_info_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    If "is_modify" is true fetch qos_info_ptr corresponding to pending
    QOS_MODIFY
  -------------------------------------------------------------------------*/
  if (is_modify)
  {
    if (!PS_FLOWI_IS_QOS_MODIFY_IN_PROGRESS(flow_ptr))
    {
      LOG_MSG_ERROR_1("ps_flowi_get_qos_info_ptr(): "
                      "QOS_MODIFY is not pending on ps_flow 0x%p", flow_ptr);
      ASSERT(0);
      return NULL;
    }

    qos_info_ptr = flow_ptr->flow_private.qos_modify_info_ptr;
  }
  else
  {
    qos_info_ptr = flow_ptr->flow_private.qos_info_ptr;
  }

  return qos_info_ptr;

} /* ps_flowi_get_qos_info_ptr() */

/*===========================================================================
FUNCTION PS_FLOWI_GET_RX_FLTR_HANDLE_EX()

DESCRIPTION
  This function returns a handle which can be used to fetch Rx filters
  associated with a flow independent of the IP version.

  Rx filters can pertain to either QOS_REQUEST/active QOS or pending
  QOS_MODIFY

PARAMETERS
  flow_ptr  : pointer to the flow whose filters need to be fetched
  is_modify : is the client interested in fetching Rx filters associated with
              pending QOS_MODIFY? If so value must be TRUE

RETURN VALUE
  A handle using which Rx filters can be fetched : on success
  NULL                                           : otherwise

DEPENDENCIES
  Flow must not be a default flow.

  This function must be called from the same task context in which mode handler
  is running. Otherwise filters could be deleted (for example because
  QOS_MODIFY is accepted/rejected) while client is fetching them which could
  lead to incorrect operations.

  Sample usage to fetch Rx ilters pertaining to QOS_REQUEST is
    handle = ps_flow_get_rx_fltr_handle(_exflow_ptr, FALSE)
    while (handle != NULL)
    {
      ps_flow_get_rx_fltr_by_handle_ex(flow_ptr, FALSE, handle,
                                    fltr, prcd, new_handle)
      handle = new_handle
    }

SIDE EFFECTS
  None
===========================================================================*/
void *ps_flowi_get_rx_fltr_handle_ex
(
  ps_flow_type          * flow_ptr,
  boolean                 is_modify
)
{
  qos_info_type             * qos_info_ptr;
  ps_flow_rx_fltr_buf_type  * curr_fltr_buf;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  if (!PS_FLOW_IS_VALID(flow_ptr))
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_1("ps_flowi_get_rx_fltr_handle_ex(): "
                    "Invalid ps_flow 0x%p is passed", flow_ptr);
    ASSERT(0);
    return NULL;
  }

  if (PS_FLOWI_GET_CAPABILITY(flow_ptr, PS_FLOW_CAPABILITY_DEFAULT))
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_1("ps_flowi_get_rx_fltr_handle_ex(): "
                    "QoS is not supported on default ps_flow 0x%p", flow_ptr);
    ASSERT(0);
    return NULL;
  }

  qos_info_ptr = ps_flowi_get_qos_info_ptr(flow_ptr, is_modify);
  if (qos_info_ptr == NULL)
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    /* Appropriate message is printed in ps_flowi_get_qos_info_ptr() */
    ASSERT(0);
    return NULL;
  }

  if (0 == list_is_initialized(&(qos_info_ptr->rx.fltr_list)))
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_INFO1_1("ps_flowi_get_rx_fltr_handle_ex(): "
                    "Uninitialized rx fltr list, ps_flow 0x%p", flow_ptr);
    return NULL;
  }

  /*-------------------------------------------------------------------------
    Return a handle to the filter
  -------------------------------------------------------------------------*/
  curr_fltr_buf = list_peek_front(&(qos_info_ptr->rx.fltr_list));

  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
  return curr_fltr_buf;

} /* ps_flowi_get_rx_fltr_handle_ex() */

/*===========================================================================
FUNCTION PS_FLOWI_GET_RX_FLTR_BY_HANDLE_EX()

DESCRIPTION
  Given a handle, this function returns a Rx filter, and also the
  precedence of the filter independent of the IP version.
  A new handle is returned so that next filter is fetched when
  this function is invoked with that handle.

  Rx filters can pertain to either QOS_REQUEST/active QOS or pending
  QOS_MODIFY

PARAMETERS
  flow_ptr       : pointer to the flow
  rx_fltr_handle : handle using which a filter is fetched
  is_modify      : is the client interested in fetching Rx filters associated
                   with pending QOS_MODIFY? If so value must be TRUE
  fltr           : OUT PARAM, pointer to Rx filter
  prcd           : OUT PARAM, precedence of Rx filter
  next_handle    : OUT PARAM, new handle which can be used to fetch next
                   Rx filter
RETURN VALUE
  TRUE  : on success
  FALSE : otherwise

DEPENDENCIES
  Flow must not be a default flow.

  This function must be called from the same task context in which mode handler
  is running. Otherwise filters could be deleted (for example because
  QOS_MODIFY is accepted/rejected) while client is fetching them which could
  lead to incorrect operations.

  Do not cache filter and access its contents later. Since a filter can be
  deleted, this may lead to memory corruption or data aborts.

  Sample usage to fetch Rx filters pertaining to QOS_REQUEST is
    handle = ps_flow_get_rx_fltr_handle_ex(flow_ptr, FALSE)
    while (handle != NULL)
    {
      ps_flow_get_rx_fltr_by_handle_ex(flow_ptr, FALSE, handle,
                                    fltr, prcd, new_handle)
      handle = new_handle
    }

SIDE EFFECTS
  next_handle is set so that if this function is called with that handle,
  next Rx filter is fetched
===========================================================================*/
boolean ps_flowi_get_rx_fltr_by_handle_ex
(
  ps_flow_type           * flow_ptr,
  void                   * rx_fltr_handle,
  boolean                  is_modify,
  ip_filter_type        ** fltr,
  uint8                  * prcd,
  void                  ** next_handle
)
{
  qos_info_type             * qos_info_ptr;
  ps_flow_rx_fltr_buf_type  * rx_fltr_buf;
  ps_flow_rx_fltr_buf_type  * tmp_rx_fltr_buf;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (fltr == NULL || prcd == NULL || next_handle == NULL)
  {
    DATA_ERR_FATAL("ps_flowi_get_rx_fltr_by_handle_ex(): Invalid parameters are passed");
    return FALSE;
  }

  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  if (!PS_FLOW_IS_VALID(flow_ptr))
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_1("ps_flowi_get_rx_fltr_by_handle_ex(): "
                    "Invalid ps_flow 0x%p is passed", flow_ptr);
    ASSERT(0);
    return FALSE;
  }

  if (PS_FLOWI_GET_CAPABILITY(flow_ptr, PS_FLOW_CAPABILITY_DEFAULT))
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_1("ps_flowi_get_rx_fltr_by_handle_ex(): "
                    "QoS is not supported on default ps_flow 0x%p", flow_ptr);
    ASSERT(0);
    return FALSE;
  }

  if (rx_fltr_handle == NULL)
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    *next_handle = NULL;
    *fltr        = NULL;
    return FALSE;
  }

  rx_fltr_buf = (ps_flow_rx_fltr_buf_type *) rx_fltr_handle;
  if (!ps_mem_is_valid(rx_fltr_buf, PS_MEM_RX_QOS_FLTR_BUF_TYPE))
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_1("ps_flowi_get_rx_fltr_by_handle_ex(): "
                    "Buffer (0x%X) doesn't belong to Rx filter's mem pool",
                    rx_fltr_handle);
    ASSERT(0);
    return FALSE;
  }

  qos_info_ptr = ps_flowi_get_qos_info_ptr(flow_ptr, is_modify);
  if (qos_info_ptr == NULL)
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    /* Appropriate message is printed in ps_flowi_get_qos_info_ptr() */
    ASSERT(0);
    return FALSE;
  }

  if (0 == list_is_initialized(&(qos_info_ptr->rx.fltr_list)))
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_INFO1_1("ps_flowi_get_rx_fltr_by_handle_ex(): "
                    "Uninitialized rx fltr list, ps_flow 0x%p", flow_ptr);
    return FALSE;
  }

  tmp_rx_fltr_buf = list_peek_front(&(qos_info_ptr->rx.fltr_list));
  while (tmp_rx_fltr_buf != NULL && tmp_rx_fltr_buf != rx_fltr_buf)
  {
    tmp_rx_fltr_buf =
      list_peek_next(&(qos_info_ptr->rx.fltr_list), &(tmp_rx_fltr_buf->link));
  }

  if (tmp_rx_fltr_buf == NULL)
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_1("ps_flowi_get_rx_fltr_by_handle_ex(): "
                    "Unknown buffer (0x%X) is passed", rx_fltr_handle);
    ASSERT(0);
    return FALSE;
  }

  *fltr = &(rx_fltr_buf->filter);
  *prcd = rx_fltr_buf->precedence;

  /*-------------------------------------------------------------------------
    Return a handle to the filter
  -------------------------------------------------------------------------*/
  tmp_rx_fltr_buf =
    list_peek_next(&(qos_info_ptr->rx.fltr_list), &(rx_fltr_buf->link));

  *next_handle = tmp_rx_fltr_buf;
  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
  return TRUE;

} /* ps_flowi_get_rx_fltr_by_handle_ex() */

/*===========================================================================
FUNCTION PS_FLOWI_GET_RX_FLTR_CNT_EX()

DESCRIPTION
  Returns number of Rx filters, independent of the IP version, specified as an
  argument. Tx filters can pertain to either QOS_REQUEST/active QoS or
  pending QOS_MODIFY

PARAMETERS
  flow_ptr  : pointer to the flow serving the QOS
  is_modify : is the client interested in fetching Rx filters associated with
              pending QOS_MODIFY? If so value must be TRUE

RETURN VALUE
  Number of Rx filters : on success
  -1                   : otherwise

DEPENDENCIES
  This function must be called from the same task context in which mode handler
  is running. Flow must not be a default flow.

SIDE EFFECTS
  None
===========================================================================*/
int8 ps_flowi_get_rx_fltr_cnt_ex
(
  ps_flow_type          * flow_ptr,
  boolean                 is_modify
)
{
  qos_info_type             * qos_info_ptr;
  ps_flow_rx_fltr_buf_type  * curr_fltr_buf;
  int8                        rx_fltr_cnt = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  if (!PS_FLOW_IS_VALID(flow_ptr))
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_1("ps_flowi_get_rx_fltr_cnt_ex(): "
                    "Invalid ps_flow 0x%p is passed", flow_ptr);
    ASSERT(0);
    return -1;
  }

  if (PS_FLOWI_GET_CAPABILITY(flow_ptr, PS_FLOW_CAPABILITY_DEFAULT))
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_1("ps_flowi_get_rx_fltr_cnt_ex(): "
                    "QoS is not supported on default ps_flow 0x%p", flow_ptr);
    ASSERT(0);
    return -1;
  }

  qos_info_ptr = ps_flowi_get_qos_info_ptr(flow_ptr, is_modify);
  if (qos_info_ptr == NULL)
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    /* Appropriate message is printed in ps_flowi_get_qos_info_ptr() */
    ASSERT(0);
    return -1;
  }

  if (0 == list_is_initialized(&(qos_info_ptr->rx.fltr_list)))
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_INFO1_1("ps_flowi_get_rx_fltr_cnt_ex(): "
                    "Uninitialized rx fltr list, ps_flow 0x%p", flow_ptr);
    return 0;
  }

  /*-------------------------------------------------------------------------
    Consider only those filters that belong to given ps_flow
  -------------------------------------------------------------------------*/
  curr_fltr_buf = list_peek_front(&(qos_info_ptr->rx.fltr_list));
  while (curr_fltr_buf != NULL)
  {
    rx_fltr_cnt++;
    curr_fltr_buf =
      list_peek_next(&(qos_info_ptr->rx.fltr_list), &(curr_fltr_buf->link));
  }

  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
  return rx_fltr_cnt;

} /* ps_flowi_get_rx_fltr_cnt_ex() */


/*===========================================================================
FUNCTION PS_FLOWI_GET_RX_GRANTED_FLOW2()

DESCRIPTION
  This macro returns a ptr to the previously granted Rx flow type (when
  ps_flow is suspended for example) if available, NULL otherwise.

  NOTE: Currently granted Rx flow must be fetched and accessed in one atomic
        operation as network may choose not to apply QOS at any time, in
        which case granted Rx flow is memset to 0

PARAMETERS
  flow_ptr : pointer to the flow serving the QOS.

RETURN VALUE
  ip_flow_ptr : Cached granted ip_flow type
===========================================================================*/
ip_flow_type * ps_flowi_get_rx_granted_flow2
(
  const ps_flow_type * flow_ptr
)
{
  ps_flow_type  * assoc_flow_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!PS_FLOW_IS_VALID(flow_ptr))
  {
    LOG_MSG_ERROR_1("ps_flowi_get_rx_granted_flow2(): "
                    "Invalid flow 0x%p", flow_ptr);
    return NULL;
  }

  assoc_flow_ptr = PS_FLOWI_GET_ASSOC_PS_FLOW(flow_ptr);
  while (assoc_flow_ptr != NULL)
  {
    if (!PS_FLOW_IS_VALID(assoc_flow_ptr))
    {
      LOG_MSG_ERROR_1("ps_flowi_get_rx_granted_flow2(): "
                      "Invalid flow 0x%p", assoc_flow_ptr);
      return NULL;
    }

    flow_ptr = assoc_flow_ptr;
    assoc_flow_ptr = PS_FLOWI_GET_ASSOC_PS_FLOW(flow_ptr);
  }

  if (flow_ptr->flow_private.qos_info_ptr != NULL)
  {
    return &(flow_ptr->flow_private.qos_info_ptr->rx.ipflow.granted);
  }
  else
  {
    return NULL;
  }

} /* ps_flowi_get_rx_granted_flow2() */


/*===========================================================================
FUNCTION PS_FLOWI_GET_TX_GRANTED_FLOW2()

DESCRIPTION
  This macro returns a ptr to the previously granted TX flow type (when
  ps_flow is suspended for example) if granted, NULL otherwise.

  NOTE: Currently granted Tx flow must be fetched and accessed in one atomic
        operation as network may choose not to apply QOS at any time, in
        which case granted Tx flow is memset to 0

PARAMETERS
  flow_ptr : pointer to the flow serving the QOS.

RETURN VALUE
  ip_flow_ptr : Cached granted ip_flow type
===========================================================================*/
ip_flow_type * ps_flowi_get_tx_granted_flow2
(
  const ps_flow_type * flow_ptr
)
{
  ps_flow_type  * assoc_flow_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!PS_FLOW_IS_VALID(flow_ptr))
  {
    LOG_MSG_ERROR_1("ps_flowi_get_tx_granted_flow2(): "
                    "Invalid flow 0x%p", flow_ptr);
    return FALSE;
  }

  assoc_flow_ptr = PS_FLOWI_GET_ASSOC_PS_FLOW(flow_ptr);
  while (assoc_flow_ptr != NULL)
  {
    if (!PS_FLOW_IS_VALID(assoc_flow_ptr))
    {
      LOG_MSG_ERROR_1("ps_flowi_get_tx_granted_flow2(): "
                      "Invalid flow 0x%p", assoc_flow_ptr);
      return NULL;
    }

    flow_ptr = assoc_flow_ptr;
    assoc_flow_ptr = PS_FLOWI_GET_ASSOC_PS_FLOW(flow_ptr);
  }

  if (flow_ptr->flow_private.qos_info_ptr != NULL)
  {
    return &(flow_ptr->flow_private.qos_info_ptr->tx.ipflow.granted);
  }
  else
  {
    return NULL;
  }

} /* ps_flowi_get_tx_granted_flow2() */

/*===========================================================================
FUNCTION PS_FLOWI_IS_TX_ENABLED()

DESCRIPTION
  Checks if flow can transmit traffic

PARAMETERS
  flow_ptr : ptr to flow

RETURN VALUE
  TRUE  : if flow is enabled for Tx traffic
  FALSE : otherwise
===========================================================================*/
boolean ps_flowi_is_tx_enabled
(
  ps_flow_type * flow_ptr
)
{
  uint32         tx_flow_mask = 0;
  ps_flow_type * assoc_flow_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( !PS_FLOW_IS_VALID(flow_ptr))
  {
    return FALSE;
  }

  assoc_flow_ptr = PS_FLOW_GET_ASSOC_PS_FLOW(flow_ptr);
  while (assoc_flow_ptr != NULL)
  {
    tx_flow_mask |= PS_FLOWI_GET_TX_MASK(flow_ptr);
    flow_ptr = assoc_flow_ptr;
    assoc_flow_ptr = PS_FLOW_GET_ASSOC_PS_FLOW(flow_ptr);
  }

  tx_flow_mask |= PS_FLOWI_GET_TX_MASK(flow_ptr);

  return (tx_flow_mask == ALL_FLOWS_ENABLED);
}
#endif /* FEATURE_DATA_PS_QOS */

