/*===========================================================================

               P S _ I F A C E _ I P F L T R _ I P A . C

DESCRIPTION
  This file provides functions to install, delete filters to IPA Hardware


Copyright (c) 2014-2015 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/netiface/src/ps_iface_ipfltr_ipa.c#1 $
  $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
05/27/15    vrk    Added support for filter info logging
===========================================================================*/


/*===========================================================================

                       INCLUDE FILES FOR THE MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include "ps_iface.h"
#include "ps_ifacei_utils.h"
#include "ps_iface_ipfltr_hw.h"
#include "ds_Utils_DebugMsg.h"
#include <stringl/stringl.h>

#include "ipa_ipfltr.h"
#include "ps_ifacei_addr_mgmt.h"
#include "ps_sys_event.h"
#include "ps_dpm.h"


/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/
#define IPA_OFFSET_IPV4_SRC_ADDR       12
#define IPA_OFFSET_IPV4_DST_ADDR       16
#define IPA_OFFSET_IPV4_NEXT_HDR_PROT  9
#define IPA_OFFSET_IPV6_SRC_ADDR       8
#define IPA_OFFSET_IPV6_DST_ADDR       24
#define IPA_OFFSET_TCP_SRC_PORT        0
#define IPA_OFFSET_TCP_DST_PORT        2
#define IPA_OFFSET_UDP_SRC_PORT        0
#define IPA_OFFSET_UDP_DST_PORT        2
#define IPA_OFFSET_UDP_CHKSUM          6
#define IPA_OFFSET_ICMP_TYPE           0
#define IPA_OFFSET_ICMP_CODE           0
#define IPA_OFFSET_ICMP_ID             4
#define IPA_OFFSET_IPV6_HEADER         40
#define IPA_MASK_ICMP_TYPE             0xFF000000
#define IPA_MASK_ICMP_CODE             0x00FF0000
#define IPA_OFFSET_IPV4_TOTAL_LEN      2
#define IPA_OFFSET_IPV6_TOTAL_LEN      4
#define IPA_OFFSET_IPV6_NEXT_HDR_PROT  4
#define IPA_MASK_IPV6_NEXT_HDR         0x0000FF00
#define IPA_OFFSET_ESP_SPI             0
#define IPA_OFFSET_AH_SPI              4


/*===========================================================================

                      INTERNAL FUNCTION DEFINITIONS

===========================================================================*/
#define PS_IFACE_IPFLTRI_IPA_ADD_PORT_RULE ps_iface_ifptri_ipa_add_port_rule
ipa_ipfltr_eq_result_e ps_iface_ifptri_ipa_add_port_rule
(
  ipa_ipfltr_rule_handle_t    ipa_rule_handle,
  uint16                      ipa_port_type,  
  uint16                      port,
  uint16                      port_range
)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return ipa_ipfltr_add_ihl_offset_range_eq_16
        (
          ipa_rule_handle,
          ipa_port_type,
          port,
          port_range
        );
} /* ps_iface_ifptri_ipa_add_port_rule */

#define PS_IFACE_IPFLTRI_IPA_ADD_ICMP_RULE ps_iface_ifptri_ipa_add_icmp_rule
ipa_ipfltr_eq_result_e ps_iface_ifptri_ipa_add_icmp_rule
(
  ipa_ipfltr_rule_handle_t    ipa_rule_handle, 
  uint8                       ipa_icmp_offset,
  uint32                      ipa_icmp_mask,
  uint32                      ipa_icmp_value
)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return
      ipa_ipfltr_add_ihl_meq_32(ipa_rule_handle,
                                ipa_icmp_offset,
                                ipa_icmp_mask,
                                ipa_icmp_value);
} /* ps_iface_ifptri_ipa_add_icmp_rule */


#define PS_IFACE_IPFLTRI_IPA_ADD_SPI_RULE ps_iface_ifptri_ipa_add_spi_rule
ipa_ipfltr_eq_result_e ps_iface_ifptri_ipa_add_spi_rule
(
  ipa_ipfltr_rule_handle_t    ipa_rule_handle,                     
  uint16                      spi_protocol,
  uint32                      spi
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/ 
  return ipa_ipfltr_add_ihl_offset_eq_32(ipa_rule_handle,
                                         spi_protocol,
                                         ps_ntohl(spi));
} /* ps_iface_ifptri_ipa_add_spi_rule */

/*===========================================================================
FUNCTION PS_IFACE_IPFLTRI_ADD_IPA_FILTER_V4()

DESCRIPTION
  This function adds ipv4 based filters to ipa

DEPENDENCIES
  None
  
RETURN VALUE
  IPA_IPFLTR_SUCCESS  for success else failure

SIDE EFFECTS
  None.

===========================================================================*/
static ipa_ipfltr_eq_result_e ps_iface_ipfltri_ipa_add_v4_param
(
  ipa_ipfltr_rule_handle_t    ipa_rule_handle,
  ipa_ipfltr_rule_type_e      ipa_rule_type,
  ip_filter_type            * fltr_ptr,
  uint8                     * next_hdr_prot
)
{
  uint32                  tmp_mask;
  ipa_ipfltr_eq_result_e  ipa_ret_val = IPA_IPFLTR_SUCCESS;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  tmp_mask = fltr_ptr->ip_hdr.v4.field_mask;

  if (tmp_mask & IPFLTR_MASK_IP4_SRC_ADDR)
  {
    ipa_ret_val = ipa_ipfltr_add_meq_32
                  (
                    ipa_rule_handle,
                    IPA_OFFSET_IPV4_SRC_ADDR,
                    ps_ntohl(fltr_ptr->ip_hdr.v4.src.subnet_mask.ps_s_addr),
                    ps_ntohl(fltr_ptr->ip_hdr.v4.src.subnet_mask.ps_s_addr) &
                      ps_ntohl(fltr_ptr->ip_hdr.v4.src.addr.ps_s_addr)
                  );
  }

  if ((tmp_mask & IPFLTR_MASK_IP4_DST_ADDR) &&
      ipa_ret_val == IPA_IPFLTR_SUCCESS)
  {
    ipa_ret_val = ipa_ipfltr_add_meq_32
                  (
                    ipa_rule_handle,
                    IPA_OFFSET_IPV4_DST_ADDR,
                    ps_ntohl(fltr_ptr->ip_hdr.v4.dst.subnet_mask.ps_s_addr),
                    ps_ntohl(fltr_ptr->ip_hdr.v4.dst.subnet_mask.ps_s_addr) &
                      ps_ntohl(fltr_ptr->ip_hdr.v4.dst.addr.ps_s_addr)
                  );
  }

  if ((tmp_mask & IPFLTR_MASK_IP4_NEXT_HDR_PROT) &&
      ipa_ret_val == IPA_IPFLTR_SUCCESS)
  {
    *next_hdr_prot = fltr_ptr->ip_hdr.v4.next_hdr_prot;
    if (*next_hdr_prot != PS_IPPROTO_TCP_UDP)
    {
      ipa_ret_val = ipa_ipfltr_add_protocol_eq(ipa_rule_handle, *next_hdr_prot);
    }
  }

  if ((tmp_mask & IPFLTR_MASK_IP4_TOS) && ipa_ret_val == IPA_IPFLTR_SUCCESS)
  {
    //MSR TODO IPA has to support mask for TOS as well
    ipa_ret_val = ipa_ipfltr_add_tos_eq(ipa_rule_handle,
                                        fltr_ptr->ip_hdr.v4.tos.val);
  }

  return ipa_ret_val;
} /* ps_iface_ipfltri_ipa_add_v4_param() */


/*===========================================================================
FUNCTION PS_IFACE_IPFLTRI_ADD_IPA_FLTR_v6()

DESCRIPTION
  This function adds ipv6 based filters to ipa

DEPENDENCIES
  None

RETURN VALUE
  IPA_IPFLTR_SUCCESS  for success else failure

SIDE EFFECTS
  None.

===========================================================================*/
static ipa_ipfltr_eq_result_e ps_iface_ipfltri_ipa_add_v6_param
(
  ipa_ipfltr_rule_handle_t    ipa_rule_handle,
  ipa_ipfltr_rule_type_e      ipa_rule_type,  
  ip_filter_type            * fltr_ptr,
  uint8                     * next_hdr_prot
)
{
  uint32                  tmp_mask;
  uint8                   src_ipa_mask[16] = {0xFF, 0xFF, 0xFF, 0xFF,
                                              0xFF, 0xFF, 0xFF, 0xFF,
                                              0xFF, 0xFF, 0xFF, 0xFF,
                                              0xFF, 0xFF, 0xFF, 0xFF};
                                          
  uint8                   dst_ipa_mask[16] = {0xFF, 0xFF, 0xFF, 0xFF,
                                              0xFF, 0xFF, 0xFF, 0xFF,
                                              0xFF, 0xFF, 0xFF, 0xFF,
                                              0xFF, 0xFF, 0xFF, 0xFF};
  ipa_ipfltr_eq_result_e  ipa_ret_val = IPA_IPFLTR_SUCCESS;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  tmp_mask = fltr_ptr->ip_hdr.v6.field_mask;

  if (tmp_mask & IPFLTR_MASK_IP6_SRC_ADDR)
  {
    if (0 != fltr_ptr->ip_hdr.v6.src.prefix_len)
    {
      PS_IFACE_IPFLTR_HW_CALCULATE_V6_ADDR_MASK(src_ipa_mask, 
                                                fltr_ptr->ip_hdr.v6.src.prefix_len);
    }

    ipa_ret_val = ipa_ipfltr_add_meq_128
                  (
                    ipa_rule_handle,
                    IPA_OFFSET_IPV6_SRC_ADDR,
                    src_ipa_mask,
                    fltr_ptr->ip_hdr.v6.src.addr.in6_u.u6_addr8
                  );
  }

  if ((tmp_mask & IPFLTR_MASK_IP6_DST_ADDR) &&
      ipa_ret_val == IPA_IPFLTR_SUCCESS)
  {
    if (0 != fltr_ptr->ip_hdr.v6.dst.prefix_len)
    {
      PS_IFACE_IPFLTR_HW_CALCULATE_V6_ADDR_MASK(dst_ipa_mask, 
                                                fltr_ptr->ip_hdr.v6.dst.prefix_len);
    }
    
    ipa_ret_val = ipa_ipfltr_add_meq_128
                  (
                    ipa_rule_handle,
                    IPA_OFFSET_IPV6_DST_ADDR,
                    dst_ipa_mask,
                    fltr_ptr->ip_hdr.v6.dst.addr.in6_u.u6_addr8
                  );
  }

  if ((tmp_mask & IPFLTR_MASK_IP6_NEXT_HDR_PROT) &&
      ipa_ret_val == IPA_IPFLTR_SUCCESS)
  {
    *next_hdr_prot = fltr_ptr->ip_hdr.v6.next_hdr_prot;
    if (*next_hdr_prot != PS_IPPROTO_TCP_UDP)
    {
      ipa_ret_val = ipa_ipfltr_add_protocol_eq(ipa_rule_handle, *next_hdr_prot);  
    }
  }

  if ((tmp_mask & IPFLTR_MASK_IP6_TRAFFIC_CLASS) &&
      ipa_ret_val == IPA_IPFLTR_SUCCESS)
  {
    //MSR TODO IPA has to support mask for TC as well
    ipa_ret_val = ipa_ipfltr_add_tc_eq(ipa_rule_handle,
                                       fltr_ptr->ip_hdr.v6.trf_cls.val);
  }

  if ((tmp_mask & IPFLTR_MASK_IP6_FLOW_LABEL) &&
      ipa_ret_val == IPA_IPFLTR_SUCCESS)
  {
    ipa_ret_val = ipa_ipfltr_add_flow_eq(ipa_rule_handle,
                                         fltr_ptr->ip_hdr.v6.flow_label);
  }

  return ipa_ret_val;
} /* ps_iface_ipfltri_ipa_add_v6_param() */


/*===========================================================================
FUNCTION PS_IFACE_IPFLTRI_ADD_IPA_FLTR_NEXT_HDR()

DESCRIPTION
  This function adds next header protocol based filters to ipa
  
DEPENDENCIES
  *assumes the filter and iface have already been verified

RETURN VALUE
  IPA_IPFLTR_SUCCESS  for success else failure

SIDE EFFECTS
  None.

===========================================================================*/
static ipa_ipfltr_eq_result_e ps_iface_ipfltri_ipa_add_next_hdr_prot_param
(
  ipa_ipfltr_rule_handle_t    ipa_rule_handle,
  ip_filter_type            * fltr_ptr,
  uint8                       next_hdr_prot
)
{
  uint32                  tmp_mask;
  ipa_ipfltr_eq_result_e  ipa_ret_val = IPA_IPFLTR_SUCCESS;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch (next_hdr_prot)
  {
    case PS_IPPROTO_TCP:
    {
      tmp_mask = fltr_ptr->next_prot_hdr.tcp.field_mask;

      if (tmp_mask & IPFLTR_MASK_TCP_SRC_PORT)
      {
        ipa_ret_val = PS_IFACE_IPFLTRI_IPA_ADD_PORT_RULE
                      (
                        ipa_rule_handle,
                        IPA_OFFSET_TCP_SRC_PORT,
                        ps_ntohs(fltr_ptr->next_prot_hdr.tcp.src.port),
                        ps_ntohs(fltr_ptr->next_prot_hdr.tcp.src.port) +
                          fltr_ptr->next_prot_hdr.tcp.src.range
                      );
      }

      if ((tmp_mask & IPFLTR_MASK_TCP_DST_PORT) &&
          ipa_ret_val == IPA_IPFLTR_SUCCESS)
      {
        ipa_ret_val = PS_IFACE_IPFLTRI_IPA_ADD_PORT_RULE
                      (
                        ipa_rule_handle,
                        IPA_OFFSET_TCP_DST_PORT,
                        ps_ntohs(fltr_ptr->next_prot_hdr.tcp.dst.port),
                        ps_ntohs(fltr_ptr->next_prot_hdr.tcp.dst.port) +
                          fltr_ptr->next_prot_hdr.tcp.dst.range
                      );
      }

      break;
    }

    case PS_IPPROTO_UDP:
    {
      tmp_mask = fltr_ptr->next_prot_hdr.udp.field_mask;

      if (tmp_mask & IPFLTR_MASK_UDP_SRC_PORT)
      {
        ipa_ret_val = PS_IFACE_IPFLTRI_IPA_ADD_PORT_RULE
                      (
                        ipa_rule_handle,
                        IPA_OFFSET_UDP_SRC_PORT,
                        ps_ntohs(fltr_ptr->next_prot_hdr.udp.src.port),
                        ps_ntohs(fltr_ptr->next_prot_hdr.udp.src.port) +
                          fltr_ptr->next_prot_hdr.udp.src.range
                      );
      }

      if ((tmp_mask & IPFLTR_MASK_UDP_DST_PORT) &&
          ipa_ret_val == IPA_IPFLTR_SUCCESS)
      {
        ipa_ret_val = PS_IFACE_IPFLTRI_IPA_ADD_PORT_RULE
                      (
                        ipa_rule_handle,
                        IPA_OFFSET_UDP_DST_PORT,
                        ps_ntohs(fltr_ptr->next_prot_hdr.udp.dst.port),
                        ps_ntohs(fltr_ptr->next_prot_hdr.udp.dst.port) +
                          fltr_ptr->next_prot_hdr.udp.dst.range
                      );
      }

      if ((tmp_mask & IPFLTR_MASK_UDP_CHKSUM) &&
          ipa_ret_val == IPA_IPFLTR_SUCCESS)
      {
        ipa_ret_val = ipa_ipfltr_add_ihl_offset_eq_16
                      (ipa_rule_handle,
                       IPA_OFFSET_UDP_CHKSUM,
                       ps_ntohs(fltr_ptr->next_prot_hdr.udp.chksum));
      }

      break;
    }

    case PS_IPPROTO_ICMP:
    case PS_IPPROTO_ICMP6:
    {
      tmp_mask = fltr_ptr->next_prot_hdr.icmp.field_mask;

      if (tmp_mask & IPFLTR_MASK_ICMP_MSG_TYPE)
      {
        ipa_ret_val =
          PS_IFACE_IPFLTRI_IPA_ADD_ICMP_RULE
          (
            ipa_rule_handle,
            IPA_OFFSET_ICMP_TYPE,
            IPA_MASK_ICMP_TYPE,
            (fltr_ptr->next_prot_hdr.icmp.type) << 24
          );
      }

      if ((tmp_mask & IPFLTR_MASK_ICMP_MSG_CODE) &&
          ipa_ret_val == IPA_IPFLTR_SUCCESS)
      {
        ipa_ret_val =
          PS_IFACE_IPFLTRI_IPA_ADD_ICMP_RULE
          (
            ipa_rule_handle,
            IPA_OFFSET_ICMP_CODE,
            IPA_MASK_ICMP_CODE,
            (fltr_ptr->next_prot_hdr.icmp.code) << 16
          );
      }

      if ((tmp_mask & IPFLTR_MASK_ICMP_MSG_ID) &&
          ipa_ret_val == IPA_IPFLTR_SUCCESS)
      {
        ipa_ret_val = ipa_ipfltr_add_ihl_offset_eq_16
                      (ipa_rule_handle,
                       IPA_OFFSET_ICMP_ID,
                       ps_ntohs(fltr_ptr->next_prot_hdr.icmp.id));
      }
      break;
    }

    case PS_IPPROTO_ESP :
    {
      tmp_mask = fltr_ptr->next_prot_hdr.esp.field_mask;      
      if (tmp_mask & IPFLTR_MASK_ESP_SPI)
      {
        ipa_ret_val = PS_IFACE_IPFLTRI_IPA_ADD_SPI_RULE
                      (
                       ipa_rule_handle,
                       IPA_OFFSET_ESP_SPI,
                       fltr_ptr->next_prot_hdr.esp.spi
                      );
      }      
      break;
    }    
    
    case PS_IPPROTO_AH : 
    {
      tmp_mask = fltr_ptr->next_prot_hdr.ah.field_mask;      
      if (tmp_mask & IPFLTR_MASK_AH_SPI)
      {
        ipa_ret_val = PS_IFACE_IPFLTRI_IPA_ADD_SPI_RULE
                     (
                       ipa_rule_handle,
                       IPA_OFFSET_AH_SPI,
                       fltr_ptr->next_prot_hdr.ah.spi
                     );
      }      
      
      break;
    }

    case PS_IPPROTO_FRAG_HDR:
    case PS_IPPROTO_IGMP:
    {
      break;
    }
    
    default:
    {
      ipa_ret_val = IPA_IPFLTR_TOO_MANY_EQNS_OF_THAT_TYPE;
      DATA_ERR_FATAL("ps_iface_ipfltri_ipa_add_next_hdr_prot_param(): Unsupported next hdr protocol");
      break;
    }
  }

  return ipa_ret_val;
} /* ps_iface_ipfltri_ipa_add_next_hdr_prot_param() */

/*===========================================================================
FUNCTION PS_IFACE_IPFLTRI_IPA_DELETE()

DESCRIPTION
  This function deletes filters to ipa

DEPENDENCIES
  None
  
RETURN VALUE
  IPA_IPFLTR_SUCCESS  for success else failure

SIDE EFFECTS
  None.

===========================================================================*/
static void ps_iface_ipfltri_ipa_delete
(
  ipa_ipfltr_rule_handle_t  ipa_rule_handle
)
{
  ipa_err_code_e  ret_val = IPA_SUCCESS; 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL != ipa_rule_handle)
  {
    ret_val = ipa_ipfltr_remove_rule(ipa_rule_handle);
    if (IPA_SUCCESS != ret_val && 
        IPA_ERR_IPFLTR_RULE_TBL_PENDING_ACTIVATION != ret_val)
    {
      LOG_MSG_ERROR_2("ps_iface_ipfltri_ipa_delete(): "
                      "Couldn't delete rule handle 0x%p, err %d",
                      ipa_rule_handle, ret_val);
      ASSERT(0);
      return;
    }
  }

  LOG_MSG_INFO2_1("ps_iface_ipfltri_ipa_delete(): Deleted rule handle 0x%p",
                  ipa_rule_handle);

  return;
} /* ps_iface_ipfltri_ipa_delete() */


/*===========================================================================
FUNCTION PS_IFACE_IPFLTRI_IPA_DELETE()

DESCRIPTION
  This function deletes filters to ipa

DEPENDENCIES
  None
  
RETURN VALUE
  IPA_IPFLTR_SUCCESS  for success else failure

SIDE EFFECTS
  None.

===========================================================================*/
static void ps_iface_ipfltri_ipa_delete_all
(
  ipa_ipfltr_rule_handle_t    * handle_arr,
  uint16                        num_rules
)
{
  ipa_err_code_e  ret_val = IPA_SUCCESS; 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ret_val = ipa_ipfltr_remove_rules(num_rules, handle_arr);
  if (IPA_SUCCESS != ret_val)
  {
    ASSERT(0);
  }

  LOG_MSG_INFO2_1("ps_iface_ipfltri_ipa_delete_all(): ret_val %d",
                  ret_val);

  return;
} /* ps_iface_ipfltri_ipa_delete_all() */

/*===========================================================================
FUNCTION PS_IFACE_IPFLTRI_IPA_ADD()

DESCRIPTION
  This function adds filter to IPA driver
  
DEPENDENCIES
  *assumes the filter and iface have already been verified

RETURN VALUE
  Filter handle  for success else failure

SIDE EFFECTS
  None.

===========================================================================*/
ipa_ipfltr_rule_handle_t ps_iface_ipfltri_ipa_add
(
  ip_filter_type           * fltr_ptr,
  ipa_ipfltr_rule_type_e     ipa_rule_type,
  uint32                     ipa_fltr_result,
  uint32                   * usr_data_ptr
)
{
  ipa_ipfltr_rule_handle_t  ipa_rule_handle = NULL;
  ipa_ipfltr_ip_type_e      ipa_ipfltr_ip_type;
  uint8                     next_hdr_prot = PS_NO_NEXT_HDR;
  ipa_ipfltr_eq_result_e    ipa_ret_val;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  do
  {
    ipa_ipfltr_ip_type = (IP_V4 == fltr_ptr->ip_vsn) ? IPA_IPFLTR_IPV4
                                                     : IPA_IPFLTR_IPV6;

    ipa_rule_handle = ipa_ipfltr_alloc_rule_handle(ipa_rule_type,
                                                   ipa_ipfltr_ip_type,
                                                   ipa_fltr_result,
                                                   (uint32)(*usr_data_ptr));

    if (NULL == ipa_rule_handle)
    {
      LOG_MSG_ERROR_0("ps_iface_ipfltri_ipa_add(): "
                      "Couldn't alloc IPA rule handle");
      
      /*---------------------------------------------------------------------
        For qos filters, dont assert, just fallback to software path for
        performing qos filtering
      ---------------------------------------------------------------------*/       
      if (IPA_IPFLTR_RULE_UL_QOS != ipa_rule_type)
      {
        ASSERT(0);
      }
      break;
    }

    if (IP_V4 == fltr_ptr->ip_vsn)
    {
      ipa_ret_val = ps_iface_ipfltri_ipa_add_v4_param(ipa_rule_handle,
                                                      ipa_rule_type,
                                                      fltr_ptr,
                                                      &next_hdr_prot);
      if (IPA_IPFLTR_SUCCESS != ipa_ret_val)
      {
        break;
      }
    }
    else
    {
      ipa_ret_val = ps_iface_ipfltri_ipa_add_v6_param(ipa_rule_handle,
                                                      ipa_rule_type,
                                                      fltr_ptr,
                                                      &next_hdr_prot);
      if (IPA_IPFLTR_SUCCESS != ipa_ret_val)
      {
        break;
      }
    }

    if (PS_NO_NEXT_HDR != next_hdr_prot)
    {
      ipa_ret_val = ps_iface_ipfltri_ipa_add_next_hdr_prot_param(ipa_rule_handle,
                                                                 fltr_ptr,
                                                                 next_hdr_prot);
      if (IPA_IPFLTR_SUCCESS != ipa_ret_val)
      {
        break;
      }
    }

    return ipa_rule_handle;
  } while(0);

  
  /*-------------------------------------------------------------------------
    Because of error code path reached here, so do the cleanup
  -------------------------------------------------------------------------*/  
  ps_iface_ipfltri_ipa_delete(ipa_rule_handle);

  return NULL;
} /* ps_iface_ipfltri_ipa_add() */


/*===========================================================================
FUNCTION ps_iface_ipfltr_hw_commit_rules()

DESCRIPTION
  This function commits all hardware rule handles to hardware
  
DEPENDENCIES
  *assumes the filter and iface have already been verified

RETURN VALUE
  0  for success else failure

SIDE EFFECTS
  None.

===========================================================================*/
int32 ps_iface_ipfltr_hw_commit_rules
(
  ps_iface_ipfltr_hw_rule_handle_type   * hw_rule_handles,
  uint32                                  num_hw_rule_handles,
  ps_iface_ipfltr_client_id_enum_type     client_id
)
{
  ipa_err_code_e            ipa_err_code;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  do
  {
    ipa_err_code = ipa_ipfltr_commit_rules(num_hw_rule_handles, hw_rule_handles);

    /*-------------------------------------------------------------------------
    IPA would return Pending Activation  whenver HW is busy. IPA will later 
    commit the rule. Hence pending activation will be treated as success.
    -------------------------------------------------------------------------*/ 
    if (IPA_SUCCESS != ipa_err_code &&
        IPA_ERR_IPFLTR_RULE_TBL_PENDING_ACTIVATION != ipa_err_code)
    {
      LOG_MSG_ERROR_1("ps_iface_ipfltri_ipa_add(): "
                      "Couldn't commit rule handles err %d", ipa_err_code);
      /*---------------------------------------------------------------------
        For qos filters, dont assert, just fallback to software path for
        performing qos filtering
      ---------------------------------------------------------------------*/       
      if (IP_FLTR_CLIENT_QOS_OUTPUT != client_id)
      {
        ASSERT(0);
      }
      break;
    }

    return 0;
  } while(0);

  return -1;    
} /* ps_iface_ipfltr_hw_commit_rules() */


/*===========================================================================
FUNCTION PS_IFACE_IPFLTRI_IPA_ENABLE_TCP_ACK_PRIORITIZATION()

DESCRIPTION
  This function adds tcp ack filter to IPA driver
  
DEPENDENCIES
  *assumes the filter and iface have already been verified

RETURN VALUE
  0  for success else failure

SIDE EFFECTS
  None.

===========================================================================*/
static void ps_iface_ipfltri_ipa_enable_tcp_ack_prioritization
(
  void
)
{
  ipa_err_code_e  err_code;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  err_code = ipa_ipfltr_enable_tcp_ack_prioritization();
  if (IPA_SUCCESS  != err_code &&
      IPA_ERR_IPFLTR_RULE_TBL_PENDING_ACTIVATION != err_code)
  {
    LOG_MSG_ERROR_1("ps_iface_ipfltri_ipa_enable_tcp_ack_prioritization(): "
                    "Failed to enable TCP Ack fltr, err %d", err_code);
    ASSERT(0);
  }

  return;
} /* ps_iface_ipfltri_ipa_enable_tcp_ack_prioritization() */


/*===========================================================================
FUNCTION PS_IFACE_IPFLTRI_IPA_ADD_FRAG_FLTR()

DESCRIPTION
  This function adds v4 frag rule and v6 frag rule to IPA
  
DEPENDENCIES
  *assumes the filter and iface have already been verified

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
static void ps_iface_ipfltri_ipa_add_frag_fltr
(
  void
)
{
  ipa_ipfltr_rule_handle_t         ipa_rule_handle[2];
  ipa_ipfltr_eq_result_e    ipa_ret_val;
  ipa_err_code_e                   ipa_err_code = IPA_SUCCESS;
  ps_iface_error_enum_type         err_type = PS_IFACE_SUCCESS;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  do
  {
    /*---------------------------------------------------------------------
      Setup a v4 frag filter
    ---------------------------------------------------------------------*/  
    ipa_rule_handle[0] =
      ipa_ipfltr_alloc_rule_handle(IPA_IPFLTR_RULE_UL_STATIC_FRAG,
                                   IPA_IPFLTR_IPV4,
                                   PS_IFACE_IPFLTR_RESULT_UL_STATIC_LOCAL,
                                   0);
    if (NULL == ipa_rule_handle[0])
    {
      err_type = PS_IFACE_ERROR_HANDLE_ALLOC_FAILED;
      break;
    }

    ipa_ret_val = ipa_ipfltr_add_frag_eq(ipa_rule_handle[0]);
    if (IPA_IPFLTR_SUCCESS != ipa_ret_val)
    {
      err_type = PS_IFACE_ERROR_HW_FILTER_ERR;
      (void) ipa_ipfltr_remove_rule(ipa_rule_handle[0]);
      break;
    }

    /*---------------------------------------------------------------------
      Setup a v6 frag filter
    ---------------------------------------------------------------------*/      
    ipa_rule_handle[1] =
    ipa_ipfltr_alloc_rule_handle(IPA_IPFLTR_RULE_UL_STATIC_FRAG,
                                 IPA_IPFLTR_IPV6,
                                 PS_IFACE_IPFLTR_RESULT_UL_STATIC_LOCAL,
                                 0);
    if (NULL == ipa_rule_handle[1])
    {
      err_type = PS_IFACE_ERROR_HANDLE_ALLOC_FAILED;
      break;
    }

    ipa_ret_val = ipa_ipfltr_add_frag_eq(ipa_rule_handle[1]);
    if (IPA_IPFLTR_SUCCESS != ipa_ret_val)
    {
      err_type = PS_IFACE_ERROR_HW_FILTER_ERR;
      (void) ipa_ipfltr_remove_rule(ipa_rule_handle[1]);
      break;
    }

    /*---------------------------------------------------------------------
      Commit both the rules
    ---------------------------------------------------------------------*/     
    ipa_err_code = ipa_ipfltr_commit_rules(2, ipa_rule_handle);
    if (! (IPA_SUCCESS == ipa_err_code || 
         IPA_ERR_IPFLTR_RULE_TBL_PENDING_ACTIVATION == ipa_err_code))
    {
      err_type = PS_IFACE_ERROR_HW_FILTER_ERR;
      (void) ipa_ipfltr_remove_rules(2, ipa_rule_handle);
      break;
    }

  } while(0);

  LOG_MSG_INFO2_2("ps_iface_ipfltri_ipa_add_frag_fltr(): "
                  "status %d ipa_err_code %d", err_type, ipa_err_code);  
  return;
} /* ps_iface_ipfltri_ipa_add_frag_fltr() */

/*===========================================================================
FUNCTION PS_IFACE_IPFLTRI_LOW_LATENCY_TRAFFIC_STATUS_CB()

DESCRIPTION
  When the packets match the low latency filters installed, this callback is
  called for indicating the start.When there are no low latency filter matches,
  after timer expiry this callback is called indicating the end of low latency 
  traffic

PARAMETERS
  is_traffic_start  : TRUE -Low latency traffic start 
                      FALSE -End of the low latency traffic
  filter_index      : Handle of the filter matched

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/

static void ps_iface_ipfltri_ipa_low_latency_traffic_status_cb
(
  boolean   is_traffic_start,
  uint32    filter_index
)
{
  ps_sys_low_latency_traffic_info_type  low_latency_traffic_info;
  int16                                 ps_errno;
  int16                                 ret_val;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  memset( &low_latency_traffic_info, 0x0, 
          sizeof(ps_sys_low_latency_traffic_info_type) );
  low_latency_traffic_info.is_traffic_start = is_traffic_start;
  low_latency_traffic_info.filter_handle    = filter_index;

  ret_val = ps_sys_event_ind( PS_SYS_TECH_ALL,
                              PS_SYS_EVENT_LOW_LATENCY_TRAFFIC_STATUS,
                              (void *)&low_latency_traffic_info,
                              &ps_errno );

  if( 0 != ret_val )
  {
    LOG_MSG_ERROR_1( "ps_iface_ipfltri_low_latency_traffic_status_cb:"
                     "Event indication failed with error %d"
                     "Discarding the event",ps_errno );
  }
} /* ps_iface_ipfltri_low_latency_traffic_status_cb() */

#ifdef FEATURE_DATA_PS_464XLAT    
/*===========================================================================
FUNCTION PS_IFACE_IPFLTRI_IPA_ADD_DL_CLAT_ICMPV6_FLTR()

DESCRIPTION
  This function adds ICMPV6 pkt exception rule to IPA. ICMPV6 CLAT pkts are
  cannot be converted by IPA so IPA gives it to framework for conversion.
  
DEPENDENCIES

RETURN VALUE
  Filter handle for success else failure

SIDE EFFECTS
  None.

===========================================================================*/
static ipa_ipfltr_rule_handle_t ps_iface_ipfltri_ipa_add_dl_clat_icmpv6_fltr
(
  void
)
{
  ipa_ipfltr_rule_handle_t  ipa_rule_handle;
  ipa_ipfltr_eq_result_e    ipa_ret_val;
  ipa_err_code_e            ipa_err_code;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ipa_rule_handle =
    ipa_ipfltr_alloc_rule_handle(IPA_IPFLTR_RULE_DL_CLAT_ICMPV6,
                                 IPA_IPFLTR_IPV6,
                                 0,
                                 0);
  if (NULL == ipa_rule_handle)
  {
    LOG_MSG_ERROR_0("ps_iface_ipfltri_ipa_add_dl_clat_icmpv6_fltr(): "
                    "Couldn't alloc IPA rule handle");
    return NULL;
  }

  ipa_ret_val = ipa_ipfltr_add_protocol_eq(ipa_rule_handle, PS_IPPROTO_ICMP6); 
  if (IPA_IPFLTR_SUCCESS != ipa_ret_val)
  {
    LOG_MSG_ERROR_1("ps_iface_ipfltri_ipa_add_dl_clat_icmpv6_fltr(): "
                    "icmpv6 clat rule filter, err %d", ipa_ret_val);
    (void) ipa_ipfltr_remove_rule(ipa_rule_handle);
    return NULL;
  }

  ipa_err_code = ipa_ipfltr_commit_rules(1, &ipa_rule_handle);
  if (! (IPA_SUCCESS == ipa_err_code || 
         IPA_ERR_IPFLTR_RULE_TBL_PENDING_ACTIVATION == ipa_err_code))
  {
    LOG_MSG_ERROR_1("ps_iface_ipfltri_ipa_add_dl_clat_icmpv6_fltr(): "
                    "Couldn't commit icmpv6 clat filter, err %d", 
                    ipa_err_code);
    (void) ipa_ipfltr_remove_rule(ipa_rule_handle);
    return NULL;
  }

  return ipa_rule_handle;
} /* ps_iface_ipfltri_ipa_add_dl_clat_icmpv6_fltr() */  
#endif /* FEATURE_DATA_PS_464XLAT */

/*===========================================================================

                      EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/
void ps_iface_ipfltr_hw_init
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ps_iface_ipfltri_ipa_enable_tcp_ack_prioritization();

  ps_iface_ipfltri_ipa_add_frag_fltr();

  if( IPA_SUCCESS != ipa_ipfltr_register_low_latency_ind_cb
                     (
                       ps_iface_ipfltri_ipa_low_latency_traffic_status_cb
                     ))
  {
    DATA_ERR_FATAL("ps_iface_ipfltr_hw_init : Low latency indication cb failed");
    return;
  }
#ifdef FEATURE_DATA_PS_464XLAT  
  if (NULL == ps_iface_ipfltri_ipa_add_dl_clat_icmpv6_fltr())
  {
    ASSERT(0);
    return;
  }  
#endif /* FEATURE_DATA_PS_464XLAT */

} /* ps_iface_ipfltr_hw_init */

int32 ps_iface_ipfltr_hw_add
(
  ps_iface_type                         * iface_ptr,
  ps_iface_ipfltr_client_id_enum_type     client_id,
  ps_iface_ipfilteri_type               * filter_buf_ptr
)
{
  int32                      ret_val = -1;
  ip_filter_type           * fltr_ptr;
  ipa_ipfltr_rule_type_e     ipa_rule_type;
  uint32                     ipa_fltr_result = 0;
  uint32                     user_data = 0;
  int16                      ps_errno;
  ps_iface_type            * bridge_iface_ptr = NULL;
  ps_iface_type            * base_iface_ptr = NULL;
  struct ps_in6_addr         v6_addr;
  uint8                    * next_hdr_prot_ptr = NULL;
  boolean                    is_src_addr_added_to_fltr = FALSE;
  boolean                    is_dst_addr_added_to_fltr = FALSE;
  ps_iface_error_enum_type   status = PS_IFACE_SUCCESS;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  do
  {
    if (NULL == filter_buf_ptr ||
        NULL != filter_buf_ptr->hw_rule_handle || 
        NULL != filter_buf_ptr->hw_rule_handle_2)
    {
      status = PS_IFACE_ERROR_INVALID_FILTER_PARAM;                      
      break;                      
    }  
    
    fltr_ptr = &(filter_buf_ptr->filter.fi_default);

    /*-------------------------------------------------------------------------
      Don't add the filters in HW if the filter & the iface are not of the 
      same addr type (i.e v4 ifaces should add only v4 filters) else it would
      result in some filter which wont be ran on iface and it would just 
      occupy space in hardware and software
    -------------------------------------------------------------------------*/
    if (PS_IFACE_IS_VALID(iface_ptr) && 
        (((IP_V4 == fltr_ptr->ip_vsn) && 
         (IPV4_ADDR != ps_iface_get_addr_family(iface_ptr))) ||
        ((IP_V6 == fltr_ptr->ip_vsn) && 
         (IPV6_ADDR != ps_iface_get_addr_family(iface_ptr))))
       )
    {
      status = PS_IFACE_ERROR_VERSION_MISMATCH; 
      ret_val = 0;
      break;      
    }

    if ((client_id == IP_FLTR_CLIENT_SOCKETS) || 
        (client_id == IP_FLTR_CLIENT_CLAT))
    {
      /*---------------------------------------------------------------------
          Skip application filters Installation with IPA if Software path is enabled
      ---------------------------------------------------------------------*/
      if ( PS_IFACE_IS_VALID (iface_ptr) && (FALSE == ps_dpm_is_bridging_allowed (iface_ptr)))
      {
        ret_val = 0;
        LOG_MSG_INFO2_0("ps_iface_ipfltr_hw_add(): Not adding fltr to IPA since "
                      "Data path is software");
        break;
      }
      if (PS_IFACE_IS_VALID(iface_ptr))
      {
        base_iface_ptr = PS_IFACE_GET_BASE_IFACE(iface_ptr);

        /*-------------------------------------------------------------------------
          Install filters in Hardware for hardware capable ifaces (3gpp, rm ifaces)
        -------------------------------------------------------------------------*/
        if(!PS_IFACE_GET_CAPABILITY(base_iface_ptr,PS_IFACE_CAPABILITY_HW_CAPABLE))
        {
          status = PS_IFACE_ERROR_IFACE_HARDWARE_INCAPBALE;
          ret_val = 0;
          break;
        }
        /*-------------------------------------------------------------------------
          For unbridged scenario, do not install filters on hardware.
          In order to check if call is in bridged state or not, the framework  
          check if base iface has an rm iface because it may happen that rm 
          iface might not be bridged to epc. For eg if there is modem embedded call
          up and if tethered call comes and wants to latch on same um iface. It will
          call ps_dpm_rm_reg_iface where it tries to install all software filters to
          hardware and that moment, rm iface is not yet bridged with epc iface
        -------------------------------------------------------------------------*/
        if(NULL == PS_IFACEI_GET_OPT_RM_IFACE(base_iface_ptr) &&
           iface_ptr->name != SIO_IFACE)
        {
          status = PS_IFACE_ERROR_IFACE_NOT_BRIDGED;      
          ret_val = 0;
          break;
        }
      }

      if ((filter_buf_ptr->iface_group == RM_GROUP) &&
           (client_id == IP_FLTR_CLIENT_CLAT))
      {
        /*-------------------------------------------------------------------
          ICMP/IGMP packets need to be filtered before frag filters are
          applied, hence they are added as a seperate rule type
        -------------------------------------------------------------------*/        
        if ((IP_V4 == fltr_ptr->ip_vsn) && 
            ((PS_IPPROTO_ICMP == fltr_ptr->ip_hdr.v4.next_hdr_prot) ||
             (PS_IPPROTO_IGMP == fltr_ptr->ip_hdr.v4.next_hdr_prot)))
        {
          ipa_rule_type   = IPA_IPFLTR_RULE_UL_CLAT_STATIC_PRE_FRAG;
        }

        else
        {
          ipa_rule_type   = IPA_IPFLTR_RULE_UL_CLAT_STATIC_LOCAL;
        }

        ipa_fltr_result = PS_IFACE_IPFLTR_RESULT_UL_CLAT;
      }

      else if (filter_buf_ptr->iface_group == RM_GROUP)
      {
        ipa_rule_type   = IPA_IPFLTR_RULE_UL_STATIC_LOCAL;
        ipa_fltr_result = PS_IFACE_IPFLTR_RESULT_UL_STATIC_LOCAL;
      }

      else
      {
        ipa_rule_type   = IPA_IPFLTR_RULE_DL_SOCKET;
        ipa_fltr_result = filter_buf_ptr->fi_result;
      }

      /*---------------------------------------------------------------------
        For iface specific DL socket filters, do following
          1. Install application registered IPv6 filters with IPA only if
             destination addr is an external address. There is no need to
             install other filters as IPv6 module installs destination addr
             based filters for default IID and private IIDs
          2. Since filters are installed in to a global table which is used
             for filtering data from all PDNs, add destination address to IPv4
             filters so that data from other PDN connection do not match
             this filter. This logic is indirectly in place for IPv6 filters
             due to IID based filtering logic
      ---------------------------------------------------------------------*/
      if (iface_ptr != NULL && IPA_IPFLTR_RULE_DL_SOCKET == ipa_rule_type)
      {
        if (IP_V6 == fltr_ptr->ip_vsn)
        {
          if ((IPFLTR_MASK_IP6_DST_ADDR & fltr_ptr->ip_hdr.v6.field_mask))
          {
            if (FALSE == ps_iface_is_v6_address_external
                         (
                           iface_ptr,
                           &(fltr_ptr->ip_hdr.v6.dst.addr)
                         ))
            {              
              status = PS_IFACE_ERROR_V6_ADDRESS_NOT_EXTERNAL;     
              ret_val = 0;
              break;
            }

            /*---------------------------------------------------------------
              Since there is no special logic in IPA to handle IPv6 fragments,
              install a filter to get IPv6 fragments, which are destined to
              the address specified in the filter, in to Q6
            ---------------------------------------------------------------*/
            v6_addr.ps_s6_addr64[0] =
              fltr_ptr->ip_hdr.v6.dst.addr.ps_s6_addr64[0];
            v6_addr.ps_s6_addr64[1] =
              fltr_ptr->ip_hdr.v6.dst.addr.ps_s6_addr64[1];

            ps_iface_ipv6_add_frag_filter(iface_ptr, &v6_addr);
          }
        }
        else if (!(IPFLTR_MASK_IP4_DST_ADDR & fltr_ptr->ip_hdr.v4.field_mask))
        {      
          base_iface_ptr = PS_IFACE_GET_BASE_IFACE(iface_ptr);
          if (PS_IFACE_IS_VALID(base_iface_ptr))
          {
            fltr_ptr->ip_hdr.v4.dst.addr.ps_s_addr = 
              base_iface_ptr->iface_private.ipv4_addr;

            fltr_ptr->ip_hdr.v4.field_mask |= IPFLTR_MASK_IP4_DST_ADDR;          
            fltr_ptr->ip_hdr.v4.dst.subnet_mask.ps_s_addr = 0xFFFFFFFF;
            is_dst_addr_added_to_fltr = TRUE;
          }
        }
      }     
    }
    
    else if(client_id == IP_FLTR_CLIENT_QOS_OUTPUT)
    {
    
      /*-----------------------------------------------------------------------
        1. Check if iface is physical iface, if not dont install filter
        2. Check if bridge iface is valid, if not donot install fitler
        3. Install filter to IPA, if it fails Set iface 
           qos_fltr_offloaded_to_hw to false
      -----------------------------------------------------------------------*/      
      if (NULL == iface_ptr || !PS_IFACE_IS_PHYSICAL_IFACE(iface_ptr))
      {
        status = PS_IFACE_ERROR_IFACE_INVALID;  
        break;
      }
      else
      {
        bridge_iface_ptr = PS_IFACEI_GET_OPT_RM_IFACE(iface_ptr); 
      }
     
      if (! PS_IFACE_IS_VALID(bridge_iface_ptr))
      {
        filter_buf_ptr->hw_rule_handle = NULL;
        filter_buf_ptr->hw_rule_handle_2 = NULL;
        status = PS_IFACE_ERROR_IFACE_NOT_BRIDGED;
        ret_val = 0;
        break;         
      }
      
      /*-------------------------------------------------------------------
         Retrieve sio stream id to be sent along with Qos Filter to IPA
      -------------------------------------------------------------------*/ 
      ipa_rule_type   = IPA_IPFLTR_RULE_UL_QOS;
      ipa_fltr_result = filter_buf_ptr->fi_result;
      (void )ps_iface_ioctl (bridge_iface_ptr,
                             PS_IFACE_IOCTL_RM_GET_STREAM_ID,
                             (sio_stream_id_type *) &user_data,
                             &ps_errno);

      /*-----------------------------------------------------------------------
        IPA expects src address to be specified for each qos filters to 
        distinguish same kind of filter belonging to same pdn but different
        ifaces.
      -----------------------------------------------------------------------*/			
      if (PS_IFACE_IS_VALID(iface_ptr))
      {   
        base_iface_ptr = PS_IFACE_GET_BASE_IFACE(iface_ptr);
        if (PS_IFACE_IS_VALID(base_iface_ptr))
        {
          if (IP_V4 == fltr_ptr->ip_vsn && 
             (0 == (IPFLTR_MASK_IP4_SRC_ADDR & fltr_ptr->ip_hdr.v4.field_mask)))
          {
            /*-------------------------------------------------------------------
              Add IPV4 address based filters to IPA hardware.
            -------------------------------------------------------------------*/ 
            fltr_ptr->ip_hdr.v4.src.addr.ps_s_addr = 
              base_iface_ptr->iface_private.ipv4_addr;
            fltr_ptr->ip_hdr.v4.field_mask |= IPFLTR_MASK_IP4_SRC_ADDR;	
            fltr_ptr->ip_hdr.v4.src.subnet_mask.ps_s_addr = 0xFFFFFFFF;
            
            /*-------------------------------------------------------------------
              Need to reset the filter mask else it would result in mismatch
              between logical and physical iface filters
            -------------------------------------------------------------------*/             
            is_src_addr_added_to_fltr = TRUE;
          }
          else if (IP_V6 == fltr_ptr->ip_vsn && 
                  (0 == (IPFLTR_MASK_IP6_SRC_ADDR & fltr_ptr->ip_hdr.v6.field_mask)))
          {
            fltr_ptr->ip_hdr.v6.src.addr.ps_s6_addr64[0] = 
                     (PS_IFACE_GET_V6_PRIMARY_ADDR_PTR(iface_ptr))->prefix;
            fltr_ptr->ip_hdr.v6.src.addr.ps_s6_addr64[1] =  
                     (PS_IFACE_GET_V6_PRIMARY_ADDR_PTR(iface_ptr))->iid;
            fltr_ptr->ip_hdr.v6.src.prefix_len = 
                     (PS_IFACE_GET_V6_PRIMARY_ADDR_PTR(iface_ptr))->prefix_len;
            fltr_ptr->ip_hdr.v6.field_mask |= IPFLTR_MASK_IP6_SRC_ADDR;				 
            
            /*-------------------------------------------------------------------
              Need to reset the filter mask else it would result in mismatch
              between logical and physical iface filters
            -------------------------------------------------------------------*/              
            is_src_addr_added_to_fltr = TRUE;            
          }		   
        }
      }	   
    }    
    else
    {
      status = PS_IFACE_ERROR_HW_OP_NOT_SUPPORTED;
      ret_val = 0;  
      break;
    }

    /*-----------------------------------------------------------------------
      Add filter to IPA. If next_hdr_prot is TCP_UDP, filter needs to be
      installed twice, once as TCP and once more as UDP since there is no
      such protocol as TCP_UDP in real world.
    -----------------------------------------------------------------------*/    
#ifdef TEST_FRAMEWORK
    #error code not present
#endif
    {
      if (IP_V4 == fltr_ptr->ip_vsn)
      {
        next_hdr_prot_ptr = &(fltr_ptr->ip_hdr.v4.next_hdr_prot);
      }
      else 
      {
        next_hdr_prot_ptr = &(fltr_ptr->ip_hdr.v6.next_hdr_prot);
      }

      if (PS_IPPROTO_TCP_UDP == *next_hdr_prot_ptr)
      {
        *next_hdr_prot_ptr = PS_IPPROTO_TCP;
        filter_buf_ptr->hw_rule_handle =
          ps_iface_ipfltri_ipa_add(fltr_ptr,
                                   ipa_rule_type,
                                   ipa_fltr_result,
                                   &user_data);

        *next_hdr_prot_ptr = PS_IPPROTO_UDP;
        filter_buf_ptr->hw_rule_handle_2 =
          ps_iface_ipfltri_ipa_add(fltr_ptr,
                                   ipa_rule_type,
                                   ipa_fltr_result,
                                   &user_data);

        /*-------------------------------------------------------------------
          Restore next header protocol as tcp udp in order to keep it 
          consistent with software filter
        -------------------------------------------------------------------*/                                    
        *next_hdr_prot_ptr = PS_IPPROTO_TCP_UDP;
      }
      else
      {
        filter_buf_ptr->hw_rule_handle =
          ps_iface_ipfltri_ipa_add(fltr_ptr,
                                   ipa_rule_type,
                                   ipa_fltr_result,
                                   &user_data);
      }

      if (NULL == filter_buf_ptr->hw_rule_handle || 
         (PS_IPPROTO_TCP_UDP == *next_hdr_prot_ptr &&
         NULL == filter_buf_ptr->hw_rule_handle_2))
      {
        status = PS_IFACE_ERROR_HW_FILTER_ERR;
        break;
      }
      else
      {
        /*---------------------------------------------------------------
          For qos filter installed on hardware, set offload flag to TRUE
        ---------------------------------------------------------------*/
        if (client_id == IP_FLTR_CLIENT_QOS_OUTPUT)
        {
          iface_ptr->qos_fltr_offloaded_to_hw = TRUE;
        }   
      }
    }
    ret_val = 0;
  } while (0);

  LOG_MSG_INFO2_5("ps_iface_ipfltr_hw_add(): "
                  "status %d client %d iface 0x%x:%d qos_fltr_offloaded_to_hw flag %d",
                  status, client_id,
                  PS_IFACE_IS_VALID(iface_ptr)?iface_ptr->name:0, 
                  PS_IFACE_IS_VALID(iface_ptr)?iface_ptr->instance:0,
                  PS_IFACE_IS_VALID(iface_ptr)?iface_ptr->qos_fltr_offloaded_to_hw:0);  
                  
  LOG_MSG_INFO2_3("ps_iface_ipfltr_hw_add(): "
                  "hw_rule_handle 0x%x hw_rule_handle_2 0x%x iface group %d",
                  (NULL != filter_buf_ptr)?filter_buf_ptr->hw_rule_handle:NULL,
                  (NULL != filter_buf_ptr)?filter_buf_ptr->hw_rule_handle_2:NULL,
                  (NULL != filter_buf_ptr)?filter_buf_ptr->iface_group:0);          

  /*-------------------------------------------------------------------
    Reset the filter mask else it would result in mismatch
    between logical and physical iface filters
  -------------------------------------------------------------------*/       
  if (TRUE == is_src_addr_added_to_fltr)
  {
    if (IP_V4 == fltr_ptr->ip_vsn)
    {
      fltr_ptr->ip_hdr.v4.src.addr.ps_s_addr = 0;
      fltr_ptr->ip_hdr.v4.field_mask &= ~IPFLTR_MASK_IP4_SRC_ADDR;
    }
    else
    {
      fltr_ptr->ip_hdr.v6.src.addr.ps_s6_addr64[0] = 0;
      fltr_ptr->ip_hdr.v6.src.addr.ps_s6_addr64[1] = 0;
      fltr_ptr->ip_hdr.v6.field_mask &= ~IPFLTR_MASK_IP6_SRC_ADDR;
    }
  }   
  
  /*------------------------------------------------------------------------- 
    Deatination IPV4 address will be added to HW filter only in case of 
    V4 filter. So reset the filter mask
  -------------------------------------------------------------------------*/
  if (TRUE == is_dst_addr_added_to_fltr)
  {
    fltr_ptr->ip_hdr.v4.dst.addr.ps_s_addr = 0;
    fltr_ptr->ip_hdr.v4.field_mask &= ~IPFLTR_MASK_IP4_DST_ADDR;
    fltr_ptr->ip_hdr.v4.dst.subnet_mask.ps_s_addr = 0;
  }
  
  return ret_val;
} /* ps_iface_ipfltr_hw_add() */

ps_iface_ipfltr_hw_rule_handle_type  ps_iface_ipfltr_hw_add_v6_frag_filter
(
  struct ps_in6_addr     * v6_addr_ptr,
  ps_iface_type          * iface_ptr
)
{
  ip_filter_type                   v6_frag_fltr;
  uint32                           usr_data = 0;
  ps_iface_ipfltr_hw_rule_handle_type  hw_rule_handle;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (NULL == iface_ptr || NULL == v6_addr_ptr)
  {
    ASSERT(0);
    return NULL;
  }

  LOG_MSG_INFO2_4("ps_iface_ipfltri_ipa_add_v6_frag_fltr(): "
                  "iface 0x%x:%d Prefix 0x%llx iid 0x%llx",
                  iface_ptr->name, 
                  iface_ptr->instance,                  
                  v6_addr_ptr->ps_s6_addr64[0],
                  v6_addr_ptr->ps_s6_addr64[1]);

  /*-------------------------------------------------------------------------
    Setup a v6 frag filter
  -------------------------------------------------------------------------*/
  v6_frag_fltr.ip_vsn               = IP_V6;
  v6_frag_fltr.ip_hdr.v6.field_mask =
    IPFLTR_MASK_IP6_NEXT_HDR_PROT | IPFLTR_MASK_IP6_DST_ADDR;

  v6_frag_fltr.ip_hdr.v6.next_hdr_prot = PS_IPPROTO_FRAG_HDR;
  
  v6_frag_fltr.ip_hdr.v6.dst.addr.in6_u.u6_addr64[0] =
    v6_addr_ptr->ps_s6_addr64[0];
  v6_frag_fltr.ip_hdr.v6.dst.addr.in6_u.u6_addr64[1] =
    v6_addr_ptr->ps_s6_addr64[1];
  v6_frag_fltr.ip_hdr.v6.dst.prefix_len = 128;    

  /*-------------------------------------------------------------------------
    Add v6 frag filter to IPA
  -------------------------------------------------------------------------*/
  hw_rule_handle = ps_iface_ipfltri_ipa_add(&v6_frag_fltr,
                                            IPA_IPFLTR_RULE_DL_SOCKET,
                                            PS_IFACE_GET_IFACE_INDEX(iface_ptr),
                                            &usr_data);

  if (0 != ps_iface_ipfltr_hw_commit_rules
       (&hw_rule_handle, 1, IP_FLTR_CLIENT_SOCKETS))
  {
    hw_rule_handle = NULL;
  }  

  return hw_rule_handle;  
} /* ps_iface_ipfltri_ipa_add_v6_frag_fltr() */

void ps_iface_ipfltr_hw_delete
(
  ps_iface_ipfltr_hw_rule_handle_type    hw_rule_handle,
  ps_iface_ipfltr_client_id_enum_type    client_id,
  ip_version_enum_type                   ip_vsn  
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ps_iface_ipfltri_ipa_delete(hw_rule_handle);
} /* ps_iface_ipfltr_hw_delete */

int32 ps_iface_ipfltr_hw_add_all_iface_filters
(
  ps_iface_type                         * iface_ptr,
  ps_iface_ipfltr_client_id_enum_type     client_id
)
{
  ps_iface_ipfilteri_type      * filter_buf_ptr;
  q_type                       * q_ptr;  
  int32                                  ret_val = -1;
  ps_iface_ipfltr_hw_rule_handle_type    hw_rule_handles[PS_IFACE_IPFLTR_BUF_NUM];  
  uint16                                 hw_fltr_cnt = 0;
  ps_iface_error_enum_type               err_val = PS_IFACE_SUCCESS;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_INFO2_3("ps_iface_ipfltr_hw_add_all_iface_filters(): "
                  "FLTR ADD called by client %d, iface 0x%x:%d",
                  client_id, iface_ptr->name, iface_ptr->instance);

  do
  {
    q_ptr = &(iface_ptr->iface_private.ipfltr_info[client_id]);
    for (filter_buf_ptr = q_check(q_ptr);
         ((filter_buf_ptr != NULL) && (PS_IFACE_IPFLTR_BUF_NUM-1 > hw_fltr_cnt));
         filter_buf_ptr = q_next(q_ptr, &(filter_buf_ptr->link)))
    {
      if (NULL == filter_buf_ptr->hw_rule_handle && 
          FALSE == filter_buf_ptr->disabled) 
      {   
        ret_val =  ps_iface_ipfltr_hw_add ( iface_ptr,
                                            client_id,
                                            filter_buf_ptr);
                                                 
        if (0 == ret_val)   
        {   
          /*---------------------------------------------------------------
            Add hardware rule handles to array. All hardware rules are
            commited togather.
          ---------------------------------------------------------------*/         
          if (NULL != filter_buf_ptr->hw_rule_handle)
          {
            hw_rule_handles[hw_fltr_cnt++] = 
              filter_buf_ptr->hw_rule_handle;
          }
          if (NULL != filter_buf_ptr->hw_rule_handle_2)
          {
            hw_rule_handles[hw_fltr_cnt++] = 
              filter_buf_ptr->hw_rule_handle_2;
          }
        }
        else
        {
          err_val = PS_IFACE_ERROR_HW_FILTER_ERR;  
          break;
        }
      }
    }
    /*-------------------------------------------------------------------------
      Commit all hardware rules
    -------------------------------------------------------------------------*/    
    if (err_val == PS_IFACE_SUCCESS)      
    {
      if (hw_fltr_cnt > 0 && 0 != ps_iface_ipfltr_hw_commit_rules
           (hw_rule_handles, hw_fltr_cnt, client_id))
      {
        err_val = PS_IFACE_ERROR_HW_FILTER_ERR;  
        break;
      }
    }
    ret_val = 0;
  } while (0);

  /*-------------------------------------------------------
    if IPA ran out of memory and couldnt install filter,
    delete all filters installed on ipa for given iface
  -------------------------------------------------------*/                                               
  if (err_val != PS_IFACE_SUCCESS)   
  {
    ps_iface_ipfltr_hw_delete_all_iface_filters 
    (
      iface_ptr, 
      client_id
    );
  }

  return ret_val;  
} /* ps_iface_ipfltr_hw_add_all_iface_filters() */

void ps_iface_ipfltr_hw_delete_all_iface_filters
(
  ps_iface_type                         * iface_ptr,
  ps_iface_ipfltr_client_id_enum_type     client_id
)
{ 
  ps_iface_ipfltr_hw_rule_handle_type    hw_rule_handles[PS_IFACE_IPFLTR_BUF_NUM]; 
  uint16                                 hw_rule_cnt = 0;
  ps_iface_ipfilteri_type      * filter_buf_ptr;
  q_type                       * q_ptr;  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_INFO2_3("ps_iface_ipfltr_hw_delete_all_iface_filters(): "
                  "FLTR ADD called by client %d, iface 0x%x:%d",
                  client_id, iface_ptr->name, iface_ptr->instance);

  if (!PS_IFACE_IS_VALID(iface_ptr))    
  {
    LOG_MSG_ERROR_0("ps_iface_ipfltr_hw_delete_all_iface_filters() : "
                    "Invalid iface");
    return;
  }
  
  q_ptr = &(iface_ptr->iface_private.ipfltr_info[client_id]);
  for (filter_buf_ptr = q_check(q_ptr);
       ((filter_buf_ptr != NULL) && (PS_IFACE_IPFLTR_BUF_NUM-1 > hw_rule_cnt));
       filter_buf_ptr = q_next(q_ptr, &(filter_buf_ptr->link)))
  {
    /*---------------------------------------------------------------
      Add hardware rule handles to array. All hardware rules are
      commited togather.
    ---------------------------------------------------------------*/         
    if (PS_IFACE_IPFLTR_INVALID_HANDLE != filter_buf_ptr->hw_rule_handle)
    {
      hw_rule_handles[hw_rule_cnt++] = filter_buf_ptr->hw_rule_handle;
    }
    if (PS_IFACE_IPFLTR_INVALID_HANDLE != filter_buf_ptr->hw_rule_handle_2)
    {
      hw_rule_handles[hw_rule_cnt++] = filter_buf_ptr->hw_rule_handle_2;
    }     

    filter_buf_ptr->hw_rule_handle   = PS_IFACE_IPFLTR_INVALID_HANDLE;
    filter_buf_ptr->hw_rule_handle_2 = PS_IFACE_IPFLTR_INVALID_HANDLE;
  }

  if ( hw_rule_cnt > 0)
  {
    ps_iface_ipfltri_ipa_delete_all(hw_rule_handles, hw_rule_cnt);
  }
  /*-----------------------------------------------------------------
    For qos fitlers, reset qos_fltr_offloaded_to_hw flag
  -----------------------------------------------------------------*/        
  if (client_id == IP_FLTR_CLIENT_QOS_OUTPUT)
  {
    iface_ptr->qos_fltr_offloaded_to_hw = FALSE;
  }                                          

} /* ps_iface_ipfltr_hw_delete_all_iface_filters() */

#ifdef FEATURE_DATA_PS_464XLAT
ps_iface_ipfltr_hw_rule_handle_type ps_iface_ipfltr_hw_add_clat_v6_dst_addr_fltr
(
  ps_iface_type          * iface_ptr,
  struct ps_in6_addr     * v6_addr_ptr,
  uint8                    prefix_len
)
{
  ipa_ipfltr_rule_type_e           ipa_rule_type;
  uint32                           ipa_fltr_result;
  ip_filter_type                   filter; 
  uint32                           usr_data = 0;
  ps_iface_ipfltr_hw_rule_handle_type hw_rule_handle;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO1_2("ps_iface_ipfltr_hw_add_clat_v6_dst_addr_fltr(): "
                  "Installing IPV6 address based fltr to IPA"
                  " iface 0x%x:%d ",
                   iface_ptr->name, 
                   iface_ptr->instance);
                   
  IPV6_ADDR_MSG(v6_addr_ptr->ps_s6_addr64);                     
  
  memset(&filter, 0, sizeof(ip_filter_type));
  filter.ip_hdr.v6.dst.addr.in6_u.u6_addr64[0] =  v6_addr_ptr->ps_s6_addr64[0];
  filter.ip_hdr.v6.dst.addr.in6_u.u6_addr64[1] =  v6_addr_ptr->ps_s6_addr64[1];
  filter.ip_hdr.v6.dst.prefix_len = prefix_len;
  filter.ip_hdr.v6.field_mask = IPFLTR_MASK_IP6_DST_ADDR;
  filter.ip_vsn = IP_V6;
  
  ipa_rule_type = IPA_IPFLTR_RULE_DL_SOCKET;
  ipa_fltr_result = (uint32)iface_ptr;

                                                  
  hw_rule_handle =  ps_iface_ipfltri_ipa_add(&filter,
                                             ipa_rule_type,
                                             ipa_fltr_result,
                                             &usr_data);                               

  if (0 != ps_iface_ipfltr_hw_commit_rules
       (&hw_rule_handle, 1, IP_FLTR_CLIENT_SOCKETS))
  {
    hw_rule_handle = NULL;
  }  

  return hw_rule_handle;  
} /* ps_iface_ipfltr_hw_add_clat_v6_dst_addr_fltr() */
#endif /* FEATURE_DATA_PS_464XLAT */

ps_iface_ipfltr_hw_rule_handle_type ps_iface_ipfltr_hw_add_v6_dst_addr_fltr
(
  ps_iface_type          * iface_ptr,
  struct ps_in6_addr     * v6_addr_ptr
)
{
  ip_filter_type                   filter; 
  uint32                           usr_data = 0;
  ps_iface_ipfltr_hw_rule_handle_type hw_rule_handle;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO1_2("ps_iface_ipfltr_hw_add_v6_dst_addr_fltr(): "
                  "Installing IPV6 address based fltr to IPA"
                  " iface 0x%x:%d ",
                   iface_ptr->name, 
                   iface_ptr->instance);
                   
  IPV6_ADDR_MSG(v6_addr_ptr->ps_s6_addr64);                     
  
  memset(&filter, 0, sizeof(ip_filter_type));
  
  filter.ip_vsn = IP_V6;  
  filter.ip_hdr.v6.field_mask = IPFLTR_MASK_IP6_DST_ADDR;
  
  filter.ip_hdr.v6.dst.addr.in6_u.u6_addr64[0] =  v6_addr_ptr->ps_s6_addr64[0];
  filter.ip_hdr.v6.dst.addr.in6_u.u6_addr64[1] =  v6_addr_ptr->ps_s6_addr64[1];
  filter.ip_hdr.v6.dst.prefix_len              = 128;
  
  hw_rule_handle =  ps_iface_ipfltri_ipa_add(&filter,
                                             IPA_IPFLTR_RULE_DL_SOCKET,
                                             PS_IFACE_IPFLTR_RESULT_DL_HW_UNKNOWN,
                                             &usr_data);      

  if (0 != ps_iface_ipfltr_hw_commit_rules
       (&hw_rule_handle, 1, IP_FLTR_CLIENT_SOCKETS))
  {
    hw_rule_handle = NULL;
  } 
                                                  
  return hw_rule_handle;  
} /* ps_iface_ipfltr_hw_add_v6_dst_addr_fltr() */

void ps_iface_ipfltr_hw_delete_v6_dst_addr_fltr
(
  ps_iface_ipfltr_hw_rule_handle_type  v6_dst_fltr_handle_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ps_iface_ipfltri_ipa_delete (v6_dst_fltr_handle_ptr);
  
  return;                        
} /* ps_iface_ipfltr_hw_delete_v6_dst_addr_fltr() */

ps_iface_ipfltr_hw_rule_handle_type ps_iface_ipfltr_hw_add_ul_prefix_comp_fltr
(
  ps_iface_type * iface_ptr,
  uint32          stream_id,
  uint32          ipa_fltr_result
)
{
  uint8                           prefix_ipa[16];
  ps_iface_addr_mgmt_prefix_type  prefix_info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*----------------------------------------------------------------------
                        Get prefix from iface
  ----------------------------------------------------------------------*/
  ps_iface_addr_mgmt_get_prefix(iface_ptr, &prefix_info);

  LOG_MSG_INFO2_3("ps_iface_ipfltr_hw_add_ul_prefix_comp_fltr(): "
                  "SIO Stream ID %d prefix 0x%x, prefix length %d", 
                  stream_id, 
                  prefix_info.prefix.ps_s6_addr64[0],
                  prefix_info.prefix_len);

  memscpy(&prefix_ipa, 
          sizeof(prefix_ipa), 
          &prefix_info.prefix.ps_s6_addr64[0], 
          sizeof(prefix_info.prefix.ps_s6_addr64[0]));

#ifndef TEST_FRAMEWORK

  /*-------------------------------------------------------------------------
    Install UL prefix rule in to IPA

    Note that IPA expects prefix in host order
  -------------------------------------------------------------------------*/
  return ipa_ipfltr_add_ul_ipv6_prefix_rule((sio_stream_id_type) stream_id,
                                            prefix_info.prefix_len,
                                            prefix_ipa,
                                            ipa_fltr_result);

#else
  #error code not present
#endif /* TEST_FRAMEWORK */
} /* ps_iface_ipfltr_hw_add_ul_prefix_comp_fltr() */

void ps_iface_ipfltr_hw_delete_v6_prefix_comp_fltr
(
  ps_iface_ipfltr_hw_rule_handle_type  hw_rule_handle
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  (void) ps_iface_ipfltri_ipa_delete(hw_rule_handle);
} /* ps_iface_ipfltr_hw_delete_v6_prefix_comp_fltr */

void ps_iface_ipfltr_hw_log_filters()
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  (void) ipa_ipfltr_log_all_filters();
  return;
}/*ps_iface_ipfltr_hw_log_filters*/
