/*===========================================================================

                     D S _ R M N E T _ U T I L S . C
DESCRIPTION

  The Data Services Rm Network interface Utility source file.

EXTERNALIZED FUNCTIONS

Copyright (c) 2012-2014 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                            EDIT HISTORY FOR FILE
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/rmifacectls/src/ds_rmnet_utils.c#1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
02/06/15    wc     Fix packet filtering issue
09/27/13    wc     Support dynamic port configuration
04/10/13    wc     MBIM mPDN support
01/28/12    ab     QMI WDA to support 32 filters 
07/26/12    sb     Fixed KW Errors.
02/01/12    cy     Initial implementation
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#include "amssassert.h"
#include "msg.h"
#include "err.h"
#include "ps_crit_sect.h"
#include "queue.h"
#include "dsm.h"
#include "ps_sys_ioctl.h"

#include "ds_rmnet_sm_ext.h"
#include "ds_rmnet_smi.h"
#include "ds_rmnet_defs.h"
#include "modem_mem.h"
#include "ps_system_heap.h"
#include "ps_iface_ipfltr.h"
#include "ps_iface_ipfltr_mbim.h"
#include "ps_iface_ipfltr_mbimi.h"
#include "ds_rmnet_meta_sm.h"

#include <stringl/stringl.h>



/*===========================================================================

                                DATA TYPES

===========================================================================*/
/*-----------------------------------------------------------------------
  Filter cache queue. Each entry is of type rmnet_utils_packet_filter_handle
   Since every port can have two ifaces, we add fitlers to both iface and 
   return just one handle to wda client. So when wda client sends request 
   to delete filter, we query cache and retrieve both the handles and ifaces 
   and then delete handles on both ifaces
    -----------------------------------------------------------------------*/
static q_type q_packet_filter;

/*===========================================================================

                               DEFINITIONS

===========================================================================*/

/*===========================================================================

                           FORWARD DECLARATIONS

===========================================================================*/

/*===========================================================================

                        INTERNAL FUNCTION DEFINITIONS

===========================================================================*/
/*===========================================================================
  FUNCTION rmnet_utilsi_packet_filter_get_handle_node()

  DESCRIPTION
    Retrieves entry from filter handle queue which matches the handle
    
  PARAMETERS
    handle       : filter handle

  RETURN VALUE
    Ptr of type rmnet_utils_packet_filter_handle

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static rmnet_utils_packet_filter_handle * 
  rmnet_utilsi_packet_filter_get_handle_node
(
  uint32 handle
)
{
  void                              * buf_ptr;
  void                              * next_buf_ptr;  
  rmnet_utils_packet_filter_handle  * packet_filter_item = NULL;
/*-------------------------------------------------------------------------*/

  /*-------------------------------------------------------------------------
    Iterate through filter queue and return matching filter item node
  -------------------------------------------------------------------------*/   
  buf_ptr = q_check(&q_packet_filter);
  while (buf_ptr != NULL)
  {
    next_buf_ptr =
      q_next(&q_packet_filter, &((q_generic_item_type *) buf_ptr)->link);
    packet_filter_item = (rmnet_utils_packet_filter_handle *)buf_ptr;
    if (packet_filter_item->handle1 == handle || 
        packet_filter_item->handle2 == handle)
    {
      break;
    }      
    buf_ptr = next_buf_ptr;
  } /* while (items in queue) */
  
  return packet_filter_item;  
} /* rmnet_utilsi_packet_filter_get_handle_node */


/*===========================================================================
  FUNCTION rmnet_utilsi_packet_filter_all_filters_disabled()

  DESCRIPTION
    Returns true if all filters are disabled or deleted
    
  PARAMETERS
    None

  RETURN VALUE
    TRUE : All filters disabled or deleted 
    FALSE : Otherwise
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static boolean rmnet_utilsi_packet_filter_all_filters_disabled
(
  void
)
{
  void                              * buf_ptr;
  void                              * next_buf_ptr;  
  rmnet_utils_packet_filter_handle  * packet_filter_item = NULL;
  int16                               ps_errno;
  ps_iface_ipfltr_control_status_enum_type  ipfltr_control_status;
/*-------------------------------------------------------------------------*/

  /*-------------------------------------------------------------------------
    Check if all filters are disabled. if so move to hardware path
  -------------------------------------------------------------------------*/   
  buf_ptr = q_check(&q_packet_filter);
  while (buf_ptr != NULL)
  {
    next_buf_ptr =
      q_next(&q_packet_filter, &((q_generic_item_type *) buf_ptr)->link);
      
    packet_filter_item = (rmnet_utils_packet_filter_handle *)buf_ptr;  
  
    /*-------------------------------------------------------------------------
      Only if enabled is false then we check if all filters are disabled. if
      one of the filter is enabled then break out of the while loop and donot
      switch to hardware path
    -------------------------------------------------------------------------*/     
    if (PS_IFACE_IS_VALID(packet_filter_item->rm_iface_ptr1))
    {
      (void)ps_iface_ipfltr_get_control_status(packet_filter_item->rm_iface_ptr1, 
                                                  IP_FLTR_CLIENT_MBIM,
                                                  &ipfltr_control_status,
                                                  &ps_errno);                                                                                                  
                                                  
      if (PS_IFACE_IPFLTR_ALL_FILTERS_STATUS_DISABLED != ipfltr_control_status)
      {
        break;
      }
    }
    
    if (PS_IFACE_IS_VALID(packet_filter_item->rm_iface_ptr2))
    {
       (void)ps_iface_ipfltr_get_control_status(packet_filter_item->rm_iface_ptr2, 
                                                  IP_FLTR_CLIENT_MBIM,
                                                  &ipfltr_control_status,
                                                  &ps_errno);                                                  
                     
      if (PS_IFACE_IPFLTR_ALL_FILTERS_STATUS_DISABLED != ipfltr_control_status)
      {
        break;
      }      
    }
    
    buf_ptr = next_buf_ptr;
    
    /*-------------------------------------------------------------------------
      Set it to NULL after every iteration. If its NULL after completing all 
      iteration, it means all filter for all rm ifaces are in disabled state
    -------------------------------------------------------------------------*/       
    packet_filter_item = NULL;    
  }

  LOG_MSG_INFO1_1("rmnet_utilsi_packet_filter_all_filters_disabled() : "
                  "packet_filter_item 0x%p", packet_filter_item);
  
  if (NULL == packet_filter_item)
  {
    return TRUE;
  }
  
  return FALSE;  
} /* rmnet_utilsi_packet_filter_all_filters_disabled */


/*===========================================================================

                        EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/


/*===========================================================================
  FUNCTION RMNET_UTILS_PACKET_FILTER_CHANGE()

  DESCRIPTION
    Enable or disable packet filter setting

  PARAMETERS
    filter_id  : packet filter id
    enabled    : enabled(1) or disabled(0)
    restrictive: Whitelist(1) or blacklist(0)

  RETURN VALUE
    boolean    : TRUE for success, FALSE otherwise

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean rmnet_utils_packet_filter_change
(
  uint16                filter_id,
  boolean               enabled,
  boolean               restrictive
)
{
  void                 * buf_ptr;
  void                 * next_buf_ptr; 
  int16                  ps_errno; 
  int                    retval = -1;
  ps_iface_type        * rm_iface_ptr1;
  ps_iface_type        * rm_iface_ptr2;  
  rmnet_utils_packet_filter_handle  * packet_filter_item = NULL; 
/*-------------------------------------------------------------------------*/

  /*-----------------------------------------------------------------------
    Retrieve two rm ifaces associated with ips id. Request comes per port,
    so API needs to be applied on all valid ifaces associated with the port
  -----------------------------------------------------------------------*/ 
  rmnet_smi_retrieve_rm_ifaces_from_netparam(filter_id, 
                                             &rm_iface_ptr1,
                                             &rm_iface_ptr2);  
                                             
  if (!PS_IFACE_IS_VALID(rm_iface_ptr1) &&
      !PS_IFACE_IS_VALID(rm_iface_ptr2))
  {
    LOG_MSG_ERROR_2("Invalid rm iface1 0x%p rm iface2 0x%p", 
                     rm_iface_ptr1, rm_iface_ptr2); 
    return FALSE;  
  }
  
  /*-------------------------------------------------------------------------
    Apply filter controls to filters in ifaces.
    If enable == false and if one of the rm iface has filter enable then 
    donot switch to hardware data path else switch to hardware data path.
  -------------------------------------------------------------------------*/   
  buf_ptr = q_check(&q_packet_filter);
  while (buf_ptr != NULL)
  {
    next_buf_ptr =
      q_next(&q_packet_filter, &((q_generic_item_type *) buf_ptr)->link);
      
    packet_filter_item = (rmnet_utils_packet_filter_handle *)buf_ptr;
    
   /*---------------------------------------------------------------------
    Apply the filter controls to filters in ifaces.
  ----------------------------------------------------------------------*/    
      if (PS_IFACE_IS_VALID(packet_filter_item->rm_iface_ptr1))
      {
        packet_filter_item->rm_iface_ptr1->powersave_restrictive = restrictive;
               
        retval = ps_iface_ipfltr_control(packet_filter_item->rm_iface_ptr1,                                          
                                         IP_FLTR_CLIENT_MBIM, 
                                         packet_filter_item->handle1,
                                         enabled, 
                                         &ps_errno); 
        /*-----------------------------------------------------------------
          Return false, if filter control cannot be applied
         -----------------------------------------------------------------*/   
        if (-1 == retval)
        {
          ASSERT(0);
          return FALSE;
        }                                         
      }  
      
      if (PS_IFACE_IS_VALID(packet_filter_item->rm_iface_ptr2))
      {
        packet_filter_item->rm_iface_ptr2->powersave_restrictive = restrictive;
        retval = ps_iface_ipfltr_control(packet_filter_item->rm_iface_ptr2,                                          
                                         IP_FLTR_CLIENT_MBIM, 
                                         packet_filter_item->handle2,
                                         enabled, 
                                         &ps_errno);    
        /*-----------------------------------------------------------------
          Return false, if filter control cannot be applied
         -----------------------------------------------------------------*/   
        if (-1 == retval)
        {
          ASSERT(0);        
          return FALSE;
        }                                         
      }

    buf_ptr = next_buf_ptr;
  }
  
  if (TRUE == enabled)
  {
    /* Switch to software path */	
    if ( 0 != ps_sys_ioctl( PS_SYS_TECH_ALL, 
                            PS_SYS_IOCTL_SWITCH_DATA_PATH_TO_SW,
                            NULL, 
                            &ps_errno) )
   {
     LOG_MSG_ERROR_0("Failed IOCTL SWITCH_DATA_PATH_TO_SW");
   }
  } 
  else  // disabled == true
  {  
    if (TRUE == rmnet_utilsi_packet_filter_all_filters_disabled())
    {
      /* Switch ipa Bridge back on when packet filter is disabled */
      if ( 0 != ps_sys_ioctl( PS_SYS_TECH_ALL, 
                              PS_SYS_IOCTL_SWITCH_DATA_PATH_TO_HW,
                              NULL, 
                              &ps_errno) )
      {
        LOG_MSG_ERROR_0("Failed IOCTL SWITCH_DATA_PATH_TO_HW");
      }
    }
  }


  LOG_MSG_INFO2_3("filter_id 0x%x, enabled %d restrictive %d",
                   filter_id, enabled, restrictive);

  return TRUE;
} /*rmnet_utils_packet_filter_change()*/

/*===========================================================================
  FUNCTION RMNET_UTILS_PACKET_FILTER_GET_STATE()

  DESCRIPTION
    Retrieve current packet filter setting for one of valid ifaces

  PARAMETERS
    filter_id  : packet filter id
    enabled    : enabled(TRUE) or disabled(FALSE)
    restrictive: Whitelist(TRUE) or blacklist(FALSE)

  RETURN VALUE
    boolean    : TRUE for success, FALSE otherwise

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean rmnet_utils_packet_filter_get_state
(
  uint16                filter_id,
  uint8               * enabled,
  uint8               * restrictive
)
{
  void                              * buf_ptr;
  void                              * next_buf_ptr;  
  rmnet_utils_packet_filter_handle  * packet_filter_item = NULL;  
  int16                               ps_errno;
  ps_iface_type                     * rm_iface_ptr1;
  ps_iface_type                     * rm_iface_ptr2;  
  int                                 retval;
  boolean                             is_enabled;
/*-------------------------------------------------------------------------*/

  /*-----------------------------------------------------------------------
    Retrieve two rm ifaces associated with ips id. Request comes per port,
    so API needs to be applied on all valid ifaces associated with the port
  -----------------------------------------------------------------------*/   
  rmnet_smi_retrieve_rm_ifaces_from_netparam(filter_id, 
                                             &rm_iface_ptr1,
                                             &rm_iface_ptr2);  
                                              
  if (!PS_IFACE_IS_VALID(rm_iface_ptr1) &&
      !PS_IFACE_IS_VALID(rm_iface_ptr2))
  {
    LOG_MSG_ERROR_2("Invalid rm iface1 0x%p rm iface12 0x%p", 
                     rm_iface_ptr1, rm_iface_ptr2); 
    return FALSE;  
  }

  LOG_MSG_INFO1_4("rm iface1 0x%x:%d rm iface2 0x%x:%d", 
                   PS_IFACE_IS_VALID(rm_iface_ptr1)?rm_iface_ptr1->name:0,
                   PS_IFACE_IS_VALID(rm_iface_ptr1)?rm_iface_ptr1->instance:0,
                   PS_IFACE_IS_VALID(rm_iface_ptr2)?rm_iface_ptr2->name:0,
                   PS_IFACE_IS_VALID(rm_iface_ptr2)?rm_iface_ptr2->instance:0);
  
  /*-------------------------------------------------------------------------
    Iterate through packet filter cache and retrieve the entry matching
    rm ifaces.
  -------------------------------------------------------------------------*/ 
  buf_ptr = q_check(&q_packet_filter);
  while (buf_ptr != NULL)
  {
    next_buf_ptr =
      q_next(&q_packet_filter, &((q_generic_item_type *) buf_ptr)->link);
    packet_filter_item = (rmnet_utils_packet_filter_handle *)buf_ptr;
    if ((NULL != rm_iface_ptr1 && (packet_filter_item->rm_iface_ptr1 == rm_iface_ptr1 
         || packet_filter_item->rm_iface_ptr2 == rm_iface_ptr1)) ||
        (NULL != rm_iface_ptr2 && (packet_filter_item->rm_iface_ptr1 == rm_iface_ptr2 
        || packet_filter_item->rm_iface_ptr2 == rm_iface_ptr2) ))
    {
      break;
    }      
    buf_ptr = next_buf_ptr;
  } /* while (items in queue) */
  
  /*-------------------------------------------------------------------------
    No entry found in cache so return false
  -------------------------------------------------------------------------*/ 
  if (NULL == packet_filter_item)
  {
    return FALSE;
  }
  
  /*-------------------------------------------------------------------------
    Retrieve handles from packet filter item and query mbim filter for the
    state of filter rule matching those handles
  -------------------------------------------------------------------------*/
  if (0 != packet_filter_item->handle1 && PS_IFACE_IS_VALID(rm_iface_ptr1))
  {
    retval = ps_iface_ipfltr_is_handle_enabled(packet_filter_item->rm_iface_ptr1,
                                               IP_FLTR_CLIENT_MBIM,
                                               packet_filter_item->handle1,
                                               &is_enabled,
                                               &ps_errno);
    if (0 != retval)
    {
      ASSERT(0);
      return FALSE;
    }    
    
    *restrictive = rm_iface_ptr1->powersave_restrictive;                                      
  }
  else if(0 != packet_filter_item->handle2 && PS_IFACE_IS_VALID(rm_iface_ptr2))
  {
    retval = ps_iface_ipfltr_is_handle_enabled(packet_filter_item->rm_iface_ptr2,
                                               IP_FLTR_CLIENT_MBIM,
                                               packet_filter_item->handle2,
                                               &is_enabled,
                                               &ps_errno);
                                               
    if (0 != retval)
    {
      ASSERT(0);
      return FALSE;
    }
    
    *restrictive = rm_iface_ptr2->powersave_restrictive;                                       
  }
  else
  {
    ASSERT(0);
    return FALSE;
  }

  if (TRUE == is_enabled)
  {
    *enabled = 1;
  }
  else
  {
    *enabled = 0;
  }

  LOG_MSG_INFO1_3("filter_id 0x%x enabled %d restrictive %d", 
                   filter_id, *enabled, *restrictive);

  return TRUE;
} /*rmnet_utils_packet_filter_get_state()*/

/*===========================================================================
  FUNCTION RMNET_UTILS_PACKET_FILTER_ADD_RULE()

  DESCRIPTION
    Add one filter rule to both ifaces
    
  PARAMETERS
    filter_id: packet filter id
    handle   : pointer to rule handle
    len      : size of filter pattern/mask
    pattern  : pointer to filter pattern array
    mask     : pointer to filter mask array

  RETURN VALUE
    boolean  : TRUE for success, FALSE otherwise

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean rmnet_utils_packet_filter_add_rule
(
  uint16                filter_id,
  uint32              * handle,
  uint8                 len,
  uint8               * pattern,
  uint8               * mask
)
{ 
  ps_iface_ipfltr_add_param_type      fltr_param;
  ip_filter_mbim_type               * filter_node;
  int16                               ps_errno;
  uint32                              handle1 = PS_IFACE_IPFLTR_INVALID_HANDLE;
  uint32                              handle2 = PS_IFACE_IPFLTR_INVALID_HANDLE;
  ps_iface_type                     * rm_iface_ptr1 = NULL;
  ps_iface_type                     * rm_iface_ptr2 = NULL;
  ps_iface_type                     * rm_iface_ptr = NULL;
  rmnet_utils_packet_filter_handle  * packet_filter;
  int                                 retval;
  ps_iface_ipfltr_control_status_enum_type  ipfltr_control_status;
/*-------------------------------------------------------------------------*/

  if (NULL == pattern || NULL == mask || 
      len > PS_IFACE_IPFLTR_MBIM_PACKET_FILTER_MAX_SIZE)
  {
    LOG_MSG_ERROR_3("Invalid arg pattern 0x%p mask 0x%p len %d",
            pattern, mask, len);
    ASSERT(0);   
    return FALSE;      
  }
  
  /*-----------------------------------------------------------------------
    Retrieve two rm ifaces associated with ips id. Request comes per port,
    so API needs to be applied on all valid ifaces associated with the port
  -----------------------------------------------------------------------*/   
  rmnet_smi_retrieve_rm_ifaces_from_netparam(filter_id, 
                                             &rm_iface_ptr1,
                                             &rm_iface_ptr2);  
                                             
  if (!PS_IFACE_IS_VALID(rm_iface_ptr1) &&
      !PS_IFACE_IS_VALID(rm_iface_ptr2))
  {
    LOG_MSG_ERROR_2("Invalid rm iface 1 0x%p rm iface 2 0x%p", 
                     rm_iface_ptr1, rm_iface_ptr2); 
    return FALSE;  
  }  
  
  LOG_MSG_INFO1_4("rm iface1 0x%x:%d rm iface2 0x%x:%d", 
                   PS_IFACE_IS_VALID(rm_iface_ptr1)?rm_iface_ptr1->name:0,
                   PS_IFACE_IS_VALID(rm_iface_ptr1)?rm_iface_ptr1->instance:0,
                   PS_IFACE_IS_VALID(rm_iface_ptr2)?rm_iface_ptr2->name:0,
                   PS_IFACE_IS_VALID(rm_iface_ptr2)?rm_iface_ptr2->instance:0);
  do
  { 
    /*-------------------------------------------------------------------------
      Allocate the memory for filter node, pattern and mask
    -------------------------------------------------------------------------*/   
    filter_node = ps_iface_ipfltr_mbimi_alloc(len);

    if((NULL == filter_node) || (NULL == filter_node->pattern) || (NULL == filter_node->mask))
    {
      LOG_MSG_ERROR_0("Invalid Allocation failed for filter");    
      break;
    }
    /*-------------------------------------------------------------------------
      Fill up the filter paramaters and call api to add filter to iface
    -------------------------------------------------------------------------*/ 
    filter_node->len = len;
    memscpy(filter_node->pattern, len, pattern, len);
    memscpy(filter_node->mask, len, mask, len);

    
    if (PS_IFACE_IS_VALID(rm_iface_ptr1))
    {
      rm_iface_ptr = rm_iface_ptr1;
    }
    else
    {
      rm_iface_ptr = rm_iface_ptr2;
    }
    
    /*-----------------------------------------------------------------------
      Retrieve enabled and restrictive flag from previous filters if 
      there is already filter installed on given ips id
    -----------------------------------------------------------------------*/    
    if (PS_IFACEI_NUM_FILTERS(rm_iface_ptr, IP_FLTR_CLIENT_MBIM) > 0)
    {
      fltr_param.powersave_restrictive = rm_iface_ptr->powersave_restrictive;            
      retval = ps_iface_ipfltr_get_control_status(rm_iface_ptr, 
                                                  IP_FLTR_CLIENT_MBIM,
                                                  &ipfltr_control_status,
                                                  &ps_errno);
                                                  
      if (ipfltr_control_status == PS_IFACE_IPFLTR_ALL_FILTERS_STATUS_DISABLED)
      {
        fltr_param.enable = FALSE;
      }
      else
      {
        fltr_param.enable = TRUE;
      }      
    }
    else
    {      
      fltr_param.powersave_restrictive = FALSE;
      fltr_param.enable                = TRUE;
    }
    

    *handle = PS_IFACE_IPFLTR_INVALID_HANDLE;
    fltr_param.is_validated      = TRUE;
    fltr_param.num_filters       = 1;
    fltr_param.filter_type       = IPFLTR_MBIM_TYPE;
    fltr_param.fi_ptr_arr        = filter_node;
    fltr_param.fltr_priority     = PS_IFACE_IPFLTR_PRIORITY_DEFAULT;
    fltr_param.fltr_compare_f_ptr = NULL;
    fltr_param.fi_result         = PS_IFACE_IPFLTR_RESULT_DL_MBIM;
    
    if (PS_IFACE_IS_VALID(rm_iface_ptr1))
    {  
      handle1 = ps_iface_ipfltr_add (rm_iface_ptr1,
                                     IP_FLTR_CLIENT_MBIM,
                                     &fltr_param,
                                     &ps_errno);
                                              
      LOG_MSG_INFO2_2("rm_iface_ptr1 0x%x handle %d", 
                      rm_iface_ptr1, handle1);

      if (PS_IFACE_IPFLTR_INVALID_HANDLE != handle1)
      {  
        *handle = handle1;        
      }
      else
      {
        break;
      }
    }
    
    /*-------------------------------------------------------------------------
      Add Filter to second iface if its valid. MBIM filters are processed
      at port level wrt QMI, so we can have two rm ifaces associated with
      same port
    -------------------------------------------------------------------------*/                  
    if (PS_IFACE_IS_VALID(rm_iface_ptr2))
    {
      handle2 =  ps_iface_ipfltr_add (rm_iface_ptr2,
                                      IP_FLTR_CLIENT_MBIM,
                                      &fltr_param,
                                      &ps_errno);
      LOG_MSG_INFO2_2("rm_iface_ptr2 0x%x handle %d", 
                      rm_iface_ptr2, handle2);  
      /*-----------------------------------------------------------------------
        Since we return just one handle to wda client, we choose the first 
        handle
      -----------------------------------------------------------------------*/
      if (*handle == PS_IFACE_IPFLTR_INVALID_HANDLE && 
          PS_IFACE_IPFLTR_INVALID_HANDLE != handle2)
      {
        *handle = handle2; 
      }
      else
      {
        /*-------------------------------------------------------------------
          If second filter cannot be added for second iface the delete the
          first filter added.
        -------------------------------------------------------------------*/      
        if (PS_IFACE_IPFLTR_INVALID_HANDLE == handle2 &&
            PS_IFACE_IPFLTR_INVALID_HANDLE != handle1)
        {
          retval = ps_iface_ipfltr_delete ( rm_iface_ptr1, 
                                            IP_FLTR_CLIENT_MBIM, 
                                            handle1,
                                            &ps_errno);
          if (0 != retval)
          {  
            LOG_MSG_ERROR_3("Delete failed for iface 0x%x:%d and handle 0x%x",
                            rm_iface_ptr1->name,
                            rm_iface_ptr1->instance,
                            handle1);    
            ASSERT(0);            
          }
          break;
        }        
      }      
    }
    
    if (*handle == PS_IFACE_IPFLTR_INVALID_HANDLE)
    {    
      break;
    }
    
    /*-----------------------------------------------------------------------
      Create new entry for filter cache where we store handles and rm ifaces.
      The reason being is since every port can have two ifaces, we add
      fitlers to both iface and return just one handle to wda client. So
      when wda client sends request to delete filter, we query cache and
      retrieve both the handles and ifaces and then delete handles on both 
      ifaces
    -----------------------------------------------------------------------*/
    PS_SYSTEM_HEAP_MEM_ALLOC(packet_filter, sizeof(rmnet_utils_packet_filter_handle),
                            rmnet_utils_packet_filter_handle*);
    
    if(!packet_filter)
    {
      break;
    }

    /*-----------------------------------------------------------------------
      Add handles and rm ifaces to the queue.
    -----------------------------------------------------------------------*/     
    memset(packet_filter, 0, sizeof(rmnet_utils_packet_filter_handle));
    (void) q_link ( packet_filter, &packet_filter->link );
    packet_filter->handle1 = handle1;
    packet_filter->rm_iface_ptr1 = rm_iface_ptr1;
    packet_filter->handle2 = handle2;
    packet_filter->rm_iface_ptr2 = rm_iface_ptr2;

    q_put( &q_packet_filter, &(packet_filter->link) ); 
        
    /*-----------------------------------------------------------------------
      Enabled software path if filter is installed first time for given
      ips id
    -----------------------------------------------------------------------*/     
    if (PS_IFACEI_NUM_FILTERS(rm_iface_ptr, IP_FLTR_CLIENT_MBIM) <= 1)
    {    
      /* Switch to software path */	
      if ( 0 != ps_sys_ioctl( PS_SYS_TECH_ALL, 
                              PS_SYS_IOCTL_SWITCH_DATA_PATH_TO_SW,
                              NULL, 
                              &ps_errno) )
     {
       LOG_MSG_ERROR_0("Failed IOCTL SWITCH_DATA_PATH_TO_SW");
     }
    }
        
    if (NULL != filter_node)
    {
      ps_iface_ipfltr_mbimi_free(filter_node);
    }
    
    return TRUE;
  } while(0);  
  
  if (NULL != filter_node)
  {
    ps_iface_ipfltr_mbimi_free(filter_node);
  }
  
  return FALSE;
} /*ps_iface_ipfltr_mbim_packet_filter_add_rule()*/

/*===========================================================================
  FUNCTION RMNET_UTILS_PACKET_FILTER_DELETE_ALL_RULES()

  DESCRIPTION
    Delete all filter rules for valid ifaces.

  PARAMETERS
    filter_id: packet filter id

  RETURN VALUE
    boolean  : TRUE for success, FALSE otherwise

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean rmnet_utils_packet_filter_delete_all_rules
(
  uint16                filter_id
)
{
  int16                               ps_errno;
  ps_iface_type                     * rm_iface_ptr1 = NULL;
  ps_iface_type                     * rm_iface_ptr2 = NULL;
  void                              * buf_ptr;
  void                              * next_buf_ptr;
  int                                 retval;
  rmnet_utils_packet_filter_handle  * packet_filter_item;
/*-------------------------------------------------------------------------*/
  
  do
  {                                
    /*---------------------------------------------------------------------
      Retrieve two rm ifaces associated with ips id. Request comes per port,
      so API needs to be applied on all valid ifaces associated with the port
    ---------------------------------------------------------------------*/   
    rmnet_smi_retrieve_rm_ifaces_from_netparam(filter_id, 
                                               &rm_iface_ptr1,
                                               &rm_iface_ptr2);  
                                             
    if (!PS_IFACE_IS_VALID(rm_iface_ptr1) &&
        !PS_IFACE_IS_VALID(rm_iface_ptr2))
    {
      LOG_MSG_ERROR_2("Invalid rm iface 1 0x%p rm iface 2 0x%p", 
                       rm_iface_ptr1, rm_iface_ptr2);
      break;
    }

    LOG_MSG_INFO1_4("rm iface1 0x%x:%d rm iface2 0x%x:%d", 
                     PS_IFACE_IS_VALID(rm_iface_ptr1)?rm_iface_ptr1->name:0,
                     PS_IFACE_IS_VALID(rm_iface_ptr1)?rm_iface_ptr1->instance:0,
                     PS_IFACE_IS_VALID(rm_iface_ptr2)?rm_iface_ptr2->name:0,
                     PS_IFACE_IS_VALID(rm_iface_ptr2)?rm_iface_ptr2->instance:0);

    if (PS_IFACE_IS_VALID(rm_iface_ptr1))
    {
      retval = ps_iface_ipfltr_delete_all(rm_iface_ptr1,      
                                          IP_FLTR_CLIENT_MBIM,
                                          &ps_errno);
      if (0 != retval)
      {  
        LOG_MSG_ERROR_2("Delete failed for iface 0x%x:%d ",
                        rm_iface_ptr1->name,
                        rm_iface_ptr1->instance);  
        ASSERT(0);                        
        break;
      }
    }
    
    if (PS_IFACE_IS_VALID(rm_iface_ptr2))
    {
      retval = ps_iface_ipfltr_delete_all(rm_iface_ptr2,
                                          IP_FLTR_CLIENT_MBIM,
                                          &ps_errno);
      if (0 != retval)
      {  
        LOG_MSG_ERROR_2("Delete failed for iface 0x%x:%d ",
                        rm_iface_ptr2->name,
                        rm_iface_ptr2->instance);    
        ASSERT(0);                        
        break;
      }    
    }
    
    /*-------------------------------------------------------------------------
      Delete all cached entry for rm_iface_ptr1 and rm_iface_ptr2.
    -------------------------------------------------------------------------*/ 
    buf_ptr = q_check(&q_packet_filter);
    
    while (buf_ptr != NULL)
    {
      next_buf_ptr =
        q_next(&q_packet_filter, &((q_generic_item_type *) buf_ptr)->link);
      
      packet_filter_item = (rmnet_utils_packet_filter_handle *)buf_ptr;
      if ((NULL != rm_iface_ptr1 && (packet_filter_item->rm_iface_ptr1 == rm_iface_ptr1 
           || packet_filter_item->rm_iface_ptr2 == rm_iface_ptr1)) ||
          (NULL != rm_iface_ptr2 && (packet_filter_item->rm_iface_ptr1 == rm_iface_ptr2 
          || packet_filter_item->rm_iface_ptr2 == rm_iface_ptr2) ))
      {
        /*-------------------------------------------------------------------------
          Delete the cached entry of handle and free it.
        -------------------------------------------------------------------------*/     
        q_delete(&q_packet_filter, &(packet_filter_item->link));
        PS_SYSTEM_HEAP_MEM_FREE(packet_filter_item);
      }      
      buf_ptr = next_buf_ptr;
    } /* while (items in queue) */
    
  } while(0);
 
  if (TRUE == rmnet_utilsi_packet_filter_all_filters_disabled())
  {
    /* Switch ipa Bridge back on when packet filter is disabled */
    if ( 0 != ps_sys_ioctl( PS_SYS_TECH_ALL, 
                            PS_SYS_IOCTL_SWITCH_DATA_PATH_TO_HW,
                            NULL, 
                            &ps_errno) )
    {
      LOG_MSG_ERROR_0("Failed IOCTL SWITCH_DATA_PATH_TO_HW");
    }
  }  
 
  return TRUE;
} /*rmnet_utils_packet_filter_delete_all_rules()*/

/*===========================================================================
  FUNCTION rmnet_utils_packet_filter_handle_iface_down()

  DESCRIPTION
    Delete  filter rule using rule handle

  PARAMETERS
    filter_id    :   packet filter id
    rm_iface_ptr :   rm iface ptr

  RETURN VALUE
    void 

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void rmnet_utils_packet_filter_handle_iface_down
(
  uint16                filter_id,
  ps_iface_type       * rm_iface_ptr
)
{
  rmnet_utils_packet_filter_handle  * packet_filter_item;
  ps_iface_type                     * rm_iface_ptr1;
  ps_iface_type                     * rm_iface_ptr2;
  ps_iface_type                     * rm_iface_ptr3;
  void                              * buf_ptr;
  void                              * next_buf_ptr;  
/*-------------------------------------------------------------------------*/

  LOG_MSG_INFO2_2("FLTR delete  rm_iface_ptr 0x%x:%d",
                   PS_IFACE_IS_VALID(rm_iface_ptr)?rm_iface_ptr->name:0,
                   PS_IFACE_IS_VALID(rm_iface_ptr)?rm_iface_ptr->instance:0);

  do
  {                            
    /*-----------------------------------------------------------------------
      Retrieve two rm ifaces associated with ips id. Request comes per port,
      so API needs to be applied on all valid ifaces associated with the port
    -----------------------------------------------------------------------*/   
    rmnet_smi_retrieve_rm_ifaces_from_netparam(filter_id, 
                                               &rm_iface_ptr1,
                                               &rm_iface_ptr2);  
                                               
    if (!PS_IFACE_IS_VALID(rm_iface_ptr1) &&
        !PS_IFACE_IS_VALID(rm_iface_ptr2))
    {
      LOG_MSG_ERROR_2("Invalid rm iface 1 0x%p rm iface 1 0x%p", 
                       rm_iface_ptr1, rm_iface_ptr2);  
      return;  
    }
    
    LOG_MSG_INFO1_4("rm iface1 0x%x:%d rm iface2 0x%x:%d", 
                     PS_IFACE_IS_VALID(rm_iface_ptr1)?rm_iface_ptr1->name:0,
                     PS_IFACE_IS_VALID(rm_iface_ptr1)?rm_iface_ptr1->instance:0,
                     PS_IFACE_IS_VALID(rm_iface_ptr2)?rm_iface_ptr2->name:0,
                     PS_IFACE_IS_VALID(rm_iface_ptr2)?rm_iface_ptr2->instance:0);

    if (rm_iface_ptr == rm_iface_ptr1)
    {
      rm_iface_ptr3 = rm_iface_ptr2;
    }
    else
    {
      rm_iface_ptr3 = rm_iface_ptr1;
    }
    
    if (PS_IFACE_IS_VALID(rm_iface_ptr3) &&
        (IFACE_UP == PS_IFACEI_GET_STATE(rm_iface_ptr3) ||
        IFACE_ROUTEABLE == PS_IFACEI_GET_STATE(rm_iface_ptr3)))
    {
      break;
    }
        
    /*-------------------------------------------------------------------------
      If we reach here, both call on ips id are down so delete all the handles
      and filters for both the ifaces
    -------------------------------------------------------------------------*/ 
    buf_ptr = q_check(&q_packet_filter);
    
    while (buf_ptr != NULL)
    {
      next_buf_ptr =
        q_next(&q_packet_filter, &((q_generic_item_type *) buf_ptr)->link);
      
      packet_filter_item = (rmnet_utils_packet_filter_handle *)buf_ptr;
      if (packet_filter_item->rm_iface_ptr1 == rm_iface_ptr ||
          packet_filter_item->rm_iface_ptr2 == rm_iface_ptr)
      {
        if (0 != packet_filter_item->handle1)
        {
          (void)rmnet_utils_packet_filter_delete_rule(packet_filter_item->handle1);
        }        
        else if (0 != packet_filter_item->handle2)
        {
          (void)rmnet_utils_packet_filter_delete_rule(packet_filter_item->handle2);
        }
      }      
      buf_ptr = next_buf_ptr;
    } /* while (items in queue) */
    
  } while(0);
  
  return;
} /*rmnet_utils_packet_filter_handle_iface_down()*/

/*===========================================================================
  FUNCTION RMNET_UTILS_PACKET_FILTER_DELETE_RULE()

  DESCRIPTION
    Delete  filter rule using rule handle

  PARAMETERS
    handle: filter handle

  RETURN VALUE
    boolean  : TRUE for success, FALSE otherwise

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean rmnet_utils_packet_filter_delete_rule
(
  uint32                handle
)
{
  rmnet_utils_packet_filter_handle  * packet_filter_item;
  int                                 retval;
  int16                               ps_errno;
/*-------------------------------------------------------------------------*/

  do
  {                            
    packet_filter_item = rmnet_utilsi_packet_filter_get_handle_node(handle);    
    if (NULL == packet_filter_item)
    {
      LOG_MSG_ERROR_0("No entry found in pkt filter queue");     
      break;
    }    
    
    /*-------------------------------------------------------------------------
      Delete from first iface only if its valid.
    -------------------------------------------------------------------------*/    
    if (PS_IFACE_IS_VALID(packet_filter_item->rm_iface_ptr1))
    {  
      retval = ps_iface_ipfltr_delete ( packet_filter_item->rm_iface_ptr1, 
                                        IP_FLTR_CLIENT_MBIM, 
                                        packet_filter_item->handle1,
                                        &ps_errno);
      if (0 != retval)
      {  
        LOG_MSG_ERROR_3("Delete failed for iface 0x%x:%d and handle 0x%x",
                        packet_filter_item->rm_iface_ptr1->name,
                        packet_filter_item->rm_iface_ptr1->instance,
                        packet_filter_item->handle1);    
        ASSERT(0);                        
        break;
      }
    }
    
    /*-------------------------------------------------------------------------
      Delete from second iface only if its valid.
    -------------------------------------------------------------------------*/    
    if (PS_IFACE_IS_VALID(packet_filter_item->rm_iface_ptr2))
    {
      retval = ps_iface_ipfltr_delete ( packet_filter_item->rm_iface_ptr2, 
                                        IP_FLTR_CLIENT_MBIM, 
                                        packet_filter_item->handle2,
                                        &ps_errno);
      if (0 != retval)
      {
        LOG_MSG_ERROR_3("Delete failed for iface 0x%x:%d and handle 0x%x",
                        packet_filter_item->rm_iface_ptr2->name,
                        packet_filter_item->rm_iface_ptr2->instance,
                        packet_filter_item->handle2);    
        ASSERT(0);                        
        break;
      }  
    } 
    
    /*-------------------------------------------------------------------------
      Delete the cached entry of handle and free it.
    -------------------------------------------------------------------------*/     
    q_delete(&q_packet_filter, &(packet_filter_item->link));
    
    PS_SYSTEM_HEAP_MEM_FREE(packet_filter_item);
    
    /*-------------------------------------------------------------------------
      Switch to hardware path if all filters are disabled
    -------------------------------------------------------------------------*/    
    if (TRUE == rmnet_utilsi_packet_filter_all_filters_disabled())
    {
      /* Switch ipa Bridge back on when packet filter is disabled */
      if ( 0 != ps_sys_ioctl( PS_SYS_TECH_ALL, 
                              PS_SYS_IOCTL_SWITCH_DATA_PATH_TO_HW,
                              NULL, 
                              &ps_errno) )
      {
        LOG_MSG_ERROR_0("Failed IOCTL SWITCH_DATA_PATH_TO_HW");
      }
    }    
    
    return TRUE;
  } while(0);
  
  return FALSE;
} /*ps_iface_ipfltr_mbim_filter_delete_rule()*/

/*===========================================================================
  FUNCTION RMNET_UTILS_PACKET_FILTER_GET_RULE_HANDLES()

  DESCRIPTION
    Get the list of rule handles from one of the valid ifaces.

  PARAMETERS
    filter_id: packet filter id
    p_num    : pointer to number of filters
    p_handle : pointer to array of handles

  RETURN VALUE
    boolean  : TRUE for success, FALSE otherwise

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean rmnet_utils_packet_filter_get_rule_handles
(
  uint16                 filter_id,
  uint8                * p_num,
  uint32               * p_handle
)
{
  ps_iface_type        * rm_iface_ptr1;
  ps_iface_type        * rm_iface_ptr2;
  rmnet_utils_packet_filter_handle  * packet_filter_item = NULL;
  void                              * buf_ptr;
  void                              * next_buf_ptr;
/*-------------------------------------------------------------------------*/

  /*---------------------------------------------------------------------
    Retrieve two rm ifaces associated with ips id. Request comes per port,
    so API needs to be applied on all valid ifaces associated with the port
  ---------------------------------------------------------------------*/   
  rmnet_smi_retrieve_rm_ifaces_from_netparam(filter_id, 
                                             &rm_iface_ptr1,
                                             &rm_iface_ptr2);  
                                             
      
  if (!PS_IFACE_IS_VALID(rm_iface_ptr1) &&
      !PS_IFACE_IS_VALID(rm_iface_ptr2))
  {
    LOG_MSG_ERROR_2("Invalid rm iface 1 0x%p rm iface 2 0x%p", 
                     rm_iface_ptr1, rm_iface_ptr2);
    return FALSE;
  } 
  
  LOG_MSG_INFO1_4("rm iface1 0x%x:%d rm iface2 0x%x:%d", 
                   PS_IFACE_IS_VALID(rm_iface_ptr1)?rm_iface_ptr1->name:0,
                   PS_IFACE_IS_VALID(rm_iface_ptr1)?rm_iface_ptr1->instance:0,
                   PS_IFACE_IS_VALID(rm_iface_ptr2)?rm_iface_ptr2->name:0,
                   PS_IFACE_IS_VALID(rm_iface_ptr2)?rm_iface_ptr2->instance:0);
  
  /*-------------------------------------------------------------------------
    Iterate through all enteries in cache and return handle associated
    with each entry if iface matches.
  -------------------------------------------------------------------------*/   
  buf_ptr = q_check(&q_packet_filter);
  while (buf_ptr != NULL)
  {
    next_buf_ptr =
      q_next(&q_packet_filter, &((q_generic_item_type *) buf_ptr)->link);
      
    packet_filter_item = (rmnet_utils_packet_filter_handle *)buf_ptr;
    
    if (PS_IFACE_IS_VALID(rm_iface_ptr1))
    {
       /*---------------------------------------------------------------
        Apply the filter controls to filters in ifaces.
      ----------------------------------------------------------------*/    
      if (packet_filter_item->rm_iface_ptr1 == rm_iface_ptr1 ||
          packet_filter_item->rm_iface_ptr2 == rm_iface_ptr1)
      {
         /*-----------------------------------------------------------
          If enabled is true then break out of while loop to switch to 
          software path
        ------------------------------------------------------------*/
        *p_handle = packet_filter_item->handle1;
        p_handle++;
        (*p_num)++;        
      }
    }
    else if(PS_IFACE_IS_VALID(rm_iface_ptr2))
    {
       /*---------------------------------------------------------------
        Apply the filter controls to filters in ifaces.
      ----------------------------------------------------------------*/    
      if (packet_filter_item->rm_iface_ptr1 == rm_iface_ptr2 ||
          packet_filter_item->rm_iface_ptr2 == rm_iface_ptr2)
      {
         /*-----------------------------------------------------------
          If enabled is true then break out of while loop to switch to 
          software path
        ------------------------------------------------------------*/
        *p_handle = packet_filter_item->handle1;
        p_handle++;
        (*p_num)++;        
      }
    }
    
    buf_ptr = next_buf_ptr;
  }

  LOG_MSG_INFO2_1("num handles %d ", *p_num);
  
  return TRUE;
} /*rmnet_utils_packet_filter_get_rule_handles()*/

/*===========================================================================
  FUNCTION RMNET_UTILS_PACKET_FILTER_GET_RULE()

  DESCRIPTION
    Get filter rule using rule handle

  PARAMETERS
    handle   : rule handle
    p_buf    : pointer to bytes array in sequence of len/pattern/len/mask

  RETURN VALUE
    boolean  : TRUE for success, FALSE otherwise

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean rmnet_utils_packet_filter_get_rule
(
  uint32                handle,
  uint8                *p_buf
)
{
  rmnet_utils_packet_filter_handle  * packet_filter_item = NULL;
  ip_filter_mbim_type               * mbim_filter_ptr = NULL;
/*-------------------------------------------------------------------------*/
  LOG_MSG_INFO2_1("rmnet_utils_packet_filter_get_rule(): "
                  "FLTR  handle 0x%x", handle);
                            
  packet_filter_item = rmnet_utilsi_packet_filter_get_handle_node(handle);
  
  if (NULL != packet_filter_item && 
      PS_IFACE_IS_VALID(packet_filter_item->rm_iface_ptr1))
  {
    mbim_filter_ptr = ps_iface_ipfltr_mbim_get_filter
                      (
                        packet_filter_item->rm_iface_ptr1,
                        packet_filter_item->handle1
                      );
  }
  
  if (NULL == mbim_filter_ptr &&  NULL != packet_filter_item && 
          PS_IFACE_IS_VALID(packet_filter_item->rm_iface_ptr2))
  {
    mbim_filter_ptr = ps_iface_ipfltr_mbim_get_filter
                      (
                        packet_filter_item->rm_iface_ptr2,
                        packet_filter_item->handle2
                      );
  }
    
  if (NULL == mbim_filter_ptr)
  {
    return FALSE;
  }
  
  /*-----------------------------------------------------------------------
    Update the buffer with rule associated with the handle
  -----------------------------------------------------------------------*/ 
  // Patten Len
  *p_buf = mbim_filter_ptr->len;
  
  //Pattern
  p_buf++;
  memscpy(p_buf, mbim_filter_ptr->len, 
          mbim_filter_ptr->pattern, mbim_filter_ptr->len);

  // Mask Len
  p_buf += mbim_filter_ptr->len;
  *p_buf = mbim_filter_ptr->len;
    
  // Mask 
  p_buf++;
  memscpy(p_buf, mbim_filter_ptr->len, 
          mbim_filter_ptr->mask, mbim_filter_ptr->len);  
  
  return TRUE;
} /*rmnet_utils_packet_filter_get_rule()*/

/*===========================================================================
  FUNCTION RMNET_UTILS_INIT()

  DESCRIPTION
    Initializes rmnet utils module

  PARAMETERS
    
  RETURN VALUE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void rmnet_utils_init
(
  void
)
{
  (void) q_init(&q_packet_filter);
}
