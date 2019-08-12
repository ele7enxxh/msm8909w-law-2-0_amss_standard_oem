/*!
  @file
  ds_loopback_hdlr.c

  @brief
  This mode handler facilitates testing of datapath from USB to RMNET and viceversa.Using this
  mode handler obviates the need to integrate LTE/UMTS mode handlers into the target when 
  USB data transfer including RMNet needs to be tested
  
  @detail
  
*/

/*===========================================================================

  Copyright (c) 2009-2014 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/lteps/src/ds_loopback_hdlr.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/23/12   sach    Loopback iface selectable through PS sys API as well.
02/23/12   kv      Replaced FEATURE_DATA_TEST_LOOPBACK_HDLR with
                   FEATURE_DATA_TEST_LOOPBACK.
07/15/11   sa      Added NV item 3gpp_test_loopback_enable.
06/14/10   vk      Added support for dl data multiplication
04/03/10   vk      Added support for MSG2.0 based logging
08/12/09   sjakka  Initial version.

===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA_TEST_LOOPBACK

#include "ps_svc.h"
#include "ps_iface.h"
#include "ps_aclrules.h"
#include "ps_route.h"
#include "ps_phys_link.h"
#include "ps_dpm.h"
#include "ds3gsubsmgr.h"
#include "ds_flow_control.h"
#include "ds_um_lo.h"
#include "ds_loopback_hdlr.h"
#include "ds_3gppi_utils.h"
#include "modem_mem.h"
#include "ps_sys_conf.h"



/*===========================================================================

                  GLOBAL VARIABLES

===========================================================================*/

static rex_crit_sect_type ds_loopback_hdlr_global_crit_sect;
static ds_loopback_iface_s loopback_iface_tbl[DS_LOOPBACK_HDLR_NUM_IFACES];
static boolean is_loopback_enabled[DS_LOOPBACK_HDLR_NUM_IFACES] = {FALSE};
static uint8 ds_loopback_dl_mulf = 0; /* if set to non-zero n, packet will be duplicated
                            n times and enqueued on the dl wm */
static uint8 ds_loopback_dl_dupf = 0; /* if set to non-zero n, packet will be duplicated 
                            every n packets */

static uint8 ds_loopback_ul_only = 0; /* if set, nothing is looped back */

static unsigned char *tmp_pkt = NULL;
static byte *data_ptr = NULL;
static boolean loopback_flow_ctrl_disable = FALSE;
static ds_loopback_hdlr_wm_levels_s ds_loopback_hdlr_wm_levels;

/* PS Sys enabled Loopback */
static int16                            loopback_ps_errno  = 0;
static int16                            loopback_result = -1;
static ps_sys_conf_enable_loopback_type enable_loopback = FALSE;

/* PS Sys enabled Loopback */
#define DL_MULF (ds_loopback_dl_mulf)
#define DL_DUPF (ds_loopback_dl_dupf)
#define UL_ONLY (ds_loopback_ul_only)

/*===========================================================================

                                FUNCTIONS

===========================================================================*/
/*===========================================================================
FUNCTION DS_LOOPBACK_HDLR_ALLOC_DYN_BYTE

DESCRIPTION
  This function tries to allocate 2048 bytes memory in the heap.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/

void ds_loopback_hdlr_alloc_dyn_byte(void)
{
  /*--------------------------------------------------------------------------
    Allocate dynamic memory only if data_ptr is NULL, 
    This memory is not freed 
  ---------------------------------------------------------------------------*/
  if (data_ptr == NULL)
  {
#ifndef TEST_FRAMEWORK
    data_ptr = (byte *)modem_mem_alloc(2048 * sizeof(byte), 
                                         MODEM_MEM_CLIENT_DATA_CRIT);
#else
    #error code not present
#endif
    if (data_ptr == NULL)
    {
      DS_3GPP_MSG0_ERROR("data_ptr memory allocation failure");
    }
    else
    {
      DS_3GPP_MSG0_LOW("data_ptr allocated");
    }
  }
}

/*===========================================================================
FUNCTION DS_LOOPBACK_HDLR_ALLOC_DYN_UNSIGNED_CHAR

DESCRIPTION
 	This function tries to allocate 2048 blocks of unsigned char type in the
	heap.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  byte

SIDE EFFECTS
  None.

===========================================================================*/

void ds_loopback_hdlr_alloc_dyn_unsigned_char(void)
{
		/*--------------------------------------------------------------------------
		 	Allocate dynamic memory only if DS_3GPP_TEST_LOOPBACK_ENABLE is set, 
			This memory is not freed 
		---------------------------------------------------------------------------*/
		if(tmp_pkt == NULL)
		{
#ifndef TEST_FRAMEWORK
                  tmp_pkt = (unsigned char *)modem_mem_alloc(2048 * sizeof(unsigned char)
														,MODEM_MEM_CLIENT_DATA_CRIT);
#else 
                  #error code not present
#endif

			if (tmp_pkt == NULL)
			{
				DS_3GPP_MSG0_ERROR("tmp_pkt memory allocation failure");
			}
			else
			{
				DS_3GPP_MSG0_LOW("tmp_pkt allocated");
			}
		}
}

/*===========================================================================
FUNCTION DS_LOOPBACK_HDLR_FREE_DYN_BYTE

DESCRIPTION
  This function frees the 2048 bytes memory in the heap.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void ds_loopback_hdlr_free_dyn_byte(void)
{
  if (data_ptr)
  {
#ifndef TEST_FRAMEWORK
    modem_mem_free(data_ptr ,MODEM_MEM_CLIENT_DATA_CRIT);
#else
    #error code not present
#endif
  }
}

/*===========================================================================
FUNCTION DS_LOOPBACK_HDLR_FREE_DYN_UNSIGNED_CHAR

DESCRIPTION
  This function frees the 2048 blocks of unsigned char type in the
  heap.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  byte

SIDE EFFECTS
  None.

===========================================================================*/
void ds_loopback_hdlr_free_dyn_unsigned_char(void)
{
  if (tmp_pkt)
  {
#ifndef TEST_FRAMEWORK
    modem_mem_free(tmp_pkt ,MODEM_MEM_CLIENT_DATA_CRIT);
#else
    #error code not present
#endif
  }
}


/*===========================================================================
FUNCTION DS_LOOPBACK_HDLR_GET_DYN_BYTE

DESCRIPTION
  This function returns a pointer to a byte pointing to 2048 blocks of memory

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  byte*

SIDE EFFECTS
  None.

===========================================================================*/
byte *ds_loopback_hdlr_get_dyn_byte()
{
	return data_ptr;
}

/*===========================================================================
FUNCTION DS_LOOPBACK_HDLR_GET_DYN_UNSIGNED_CHAR

DESCRIPTION
  This function returns a unsigned char pointer pointing to 2048 blocks of memory

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  unsigned char*

SIDE EFFECTS
  None.

===========================================================================*/
unsigned char *ds_loopback_hdlr_get_dyn_unsigned_char()
{
	return tmp_pkt;
}

void ds_loopback_hdlr_set_cfg(const ds_loopback_hdlr_cfg_s * cfg_p)
{
  DL_MULF = cfg_p->dl_mulf;
  DL_DUPF = cfg_p->dl_dupf;
}

void ds_loopback_hdlr_enable_all(void)
{
  int i;
  boolean ds_3gpp_test_loopback_enable = FALSE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (ds_3gpp_cfg_get( DS_3GPP_TEST_LOOPBACK_ENABLE, 
                      (boolean *)&(ds_3gpp_test_loopback_enable), 
                       sizeof(ds_3gpp_test_loopback_enable)
                     ) > 0) 
  {
    DS_3GPP_MSG1_HIGH("ds_3gpp_test_loopback_enable set to %d in EFS", 
                      ds_3gpp_test_loopback_enable);
  }
  for (i = 0; i < DS_LOOPBACK_HDLR_NUM_IFACES; ++i) 
  {
    is_loopback_enabled[i] = ds_3gpp_test_loopback_enable;
  }
}


/*===========================================================================
  FUNCTION DS_LOOPBACK_HDLR_IS_INSTANCE_ENABLED

  DESCRIPTION
    This function returns whether the loopback instance is enabled.
    
  PARAMETERS  
  iface_inst    Loopback call instance
      
  DEPENDENCIES 
    None.
  
  RETURN VALUE 
    boolean
   
  SIDE EFFECTS 
    None.
    
===========================================================================*/
boolean  ds_loopback_hdlr_is_instance_enabled(int instance)
{
  boolean ret_val = FALSE;
  if((instance <DS_LOOPBACK_HDLR_NUM_IFACES) && (instance>= 0))
  {
    ret_val = is_loopback_enabled[instance];
  }
  return ret_val;
} 
/*===========================================================================
FUNCTION DS_LOOPBACK_HDLR_IFACE_UP_CMD

DESCRIPTION
  This function puts a ds cmd for iface up event
  
PARAMETERS  
    
DEPENDENCIES 
  None.

RETURN VALUE 
 0 - SUCCESS
 
SIDE EFFECTS 
  None.
  
===========================================================================*/
LOCAL int ds_loopback_hdlr_iface_up_cmd
(
  ps_iface_type *iface_ptr, 
  void    *info_ptr
)
{
  ds_cmd_type                     *cmd_ptr;   /* DS Task msg */
  ds_loopback_hdlr_cmd_info_type  *cmd_info_ptr = NULL;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  DS_3GPP_MSG2_HIGH("IFACE UP CMD for loopback iface :0x%x info:%lx",
           (((ds_loopback_iface_s*)(iface_ptr->client_data_ptr))->client_data_ptr),
                    (uint32)info_ptr);

  cmd_ptr = ds_allocate_cmd_buf(sizeof(ds_loopback_hdlr_cmd_info_type));
  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    ASSERT(0);
    return (-1);
  }

  cmd_ptr->hdr.cmd_id = DS_CMD_3GPP_LOOPBACK_HDLR;    
  cmd_info_ptr = (ds_loopback_hdlr_cmd_info_type*)cmd_ptr->cmd_payload_ptr;

  cmd_info_ptr->id = DS_LOOPBACK_HDLR_IFACE_UP_CMD;
  cmd_info_ptr->user_data = (void*)iface_ptr;
  cmd_info_ptr->info_ptr = info_ptr;
  ds_put_cmd(cmd_ptr);

  return(0);
}


/*===========================================================================
FUNCTION DS_LOOPBACK_HDLR_IFACE_DOWN_CMD

DESCRIPTION
  This function puts a ds cmd for iface down event
  
PARAMETERS  
    
DEPENDENCIES 
  None.

RETURN VALUE 
 0 - SUCCESS
 
SIDE EFFECTS 
  None.
  
===========================================================================*/
LOCAL int ds_loopback_hdlr_iface_down_cmd 
(
  ps_iface_type *iface_ptr,
	void          *info_ptr
)
{
 
  ds_cmd_type                      *cmd_ptr;   /* DS Task msg */
  ds_loopback_hdlr_cmd_info_type   *cmd_info_ptr = NULL;
 
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 
  DS_3GPP_MSG2_HIGH("IFACE Down CMD for loopback iface :0x%x info:%lx",
			  (((ds_loopback_iface_s*)(iface_ptr->client_data_ptr))->client_data_ptr),
        (uint32)info_ptr);

  cmd_ptr = ds_allocate_cmd_buf(sizeof(ds_loopback_hdlr_cmd_info_type));
  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    ASSERT(0);
    return (-1);
  }
 
  cmd_ptr->hdr.cmd_id = DS_CMD_3GPP_LOOPBACK_HDLR;    
  cmd_info_ptr = (ds_loopback_hdlr_cmd_info_type*)cmd_ptr->cmd_payload_ptr;

  cmd_info_ptr->id = DS_LOOPBACK_HDLR_IFACE_DOWN_CMD;
  cmd_info_ptr->user_data = (void*)iface_ptr;
  cmd_info_ptr->info_ptr = info_ptr;
  ds_put_cmd(cmd_ptr);

  return(0);
 
}


/*===========================================================================
FUNCTION DS_LOOPBACK_HDLR_IPHYS_LINK_UP_CMD

DESCRIPTION
  This function puts a ds cmd for phys link up event
  
PARAMETERS  
    
DEPENDENCIES 
  None.

RETURN VALUE 
 0 - SUCCESS
 
SIDE EFFECTS 
  None.
  
===========================================================================*/
LOCAL int ds_loopback_hdlr_phys_link_up_cmd 
(
  ps_phys_link_type *phys_link_ptr_i,
  void              *info_ptr
)
{
  ds_cmd_type                      *cmd_ptr;   /* DS Task msg */
  ds_loopback_hdlr_cmd_info_type   *cmd_info_ptr = NULL;
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 
  cmd_ptr = ds_allocate_cmd_buf(sizeof(ds_loopback_hdlr_cmd_info_type));
  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    ASSERT(0);
    return (-1);
  }

  cmd_ptr->hdr.cmd_id = DS_CMD_3GPP_LOOPBACK_HDLR;
  cmd_info_ptr = (ds_loopback_hdlr_cmd_info_type*)cmd_ptr->cmd_payload_ptr;

  cmd_info_ptr->id = DS_LOOPBACK_HDLR_PHYS_LINK_UP_CMD;
  cmd_info_ptr->user_data = (void*)phys_link_ptr_i;
  cmd_info_ptr->info_ptr = info_ptr;
  ds_put_cmd(cmd_ptr);

  return(0);
 
}


/*===========================================================================
FUNCTION DS_LOOPBACK_HDLR_IPHYS_LINK_DOWN_CMD

DESCRIPTION
  This function puts a ds cmd for phys link down event
  
PARAMETERS  
    
DEPENDENCIES 
  None.

RETURN VALUE 
 0 - SUCCESS
 
SIDE EFFECTS 
  None.
  
===========================================================================*/    
LOCAL int ds_loopback_hdlr_phys_link_down_cmd 
(
  ps_phys_link_type *phys_link_ptr_i,
  void              *info_ptr
)
{
 
  ds_cmd_type                      *cmd_ptr;   /* DS Task msg */ 
  ds_loopback_hdlr_cmd_info_type   *cmd_info_ptr = NULL;
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 
  cmd_ptr = ds_allocate_cmd_buf(sizeof(ds_loopback_hdlr_cmd_info_type));
  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    ASSERT(0);
    return (-1);
  }

  cmd_ptr->hdr.cmd_id = DS_CMD_3GPP_LOOPBACK_HDLR;
  cmd_info_ptr = (ds_loopback_hdlr_cmd_info_type*)cmd_ptr->cmd_payload_ptr;

  cmd_info_ptr->id = DS_LOOPBACK_HDLR_PHYS_LINK_DOWN_CMD;
  cmd_info_ptr->user_data = (void*)phys_link_ptr_i;
  cmd_info_ptr->info_ptr = info_ptr;
  ds_put_cmd(cmd_ptr);

  return(0);
 
}


/*===========================================================================
FUNCTION DS_LOOPBACK_HDLR_PS_FLOW_LOW_WATERMARK_CB

DESCRIPTION
  This function puts a ds cmd for low water mark event
  
PARAMETERS  
    
DEPENDENCIES 
  None.

RETURN VALUE 
 0 - SUCCESS
 
SIDE EFFECTS 
  None.
  
===========================================================================*/
LOCAL int ds_loopback_hdlr_ps_flow_low_watermark_cb 
(
  dsm_watermark_type *wm,  
  void*              callback_data
)
{
 
  ds_cmd_type                      *cmd_ptr;   /* DS Task msg */ 
  ds_loopback_hdlr_cmd_info_type   *cmd_info_ptr = NULL;
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/ 

  cmd_ptr = ds_allocate_cmd_buf(sizeof(ds_loopback_hdlr_cmd_info_type));
  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    ASSERT(0);
    return (-1);
  }

  cmd_ptr->hdr.cmd_id = DS_CMD_3GPP_LOOPBACK_HDLR;
  cmd_info_ptr = (ds_loopback_hdlr_cmd_info_type*)cmd_ptr->cmd_payload_ptr;

  cmd_info_ptr->id = DS_LOOPBACK_HDLR_PHYS_LINK_FLOW_ENABLE_CMD;
  cmd_info_ptr->user_data = (void*)callback_data;
  ds_put_cmd(cmd_ptr);
 
  return(0);
 
}


/*===========================================================================
FUNCTION DS_LOOPBACK_HDLR_PS_FLOW_HIGH_WATERMARK_CB

DESCRIPTION
  This function puts a ds cmd for high water mark event
  
PARAMETERS  
    
DEPENDENCIES 
  None.

RETURN VALUE 
 0 - SUCCESS
 
SIDE EFFECTS 
  None.
  
===========================================================================*/
LOCAL int ds_loopback_hdlr_ps_flow_high_watermark_cb 
(
  dsm_watermark_type *wm,  
  void*              callback_data
)
{
 
  ds_cmd_type                      *cmd_ptr;   /* DS Task msg */ 
  ds_loopback_hdlr_cmd_info_type   *cmd_info_ptr = NULL;
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/ 

  cmd_ptr = ds_allocate_cmd_buf(sizeof(ds_loopback_hdlr_cmd_info_type));
  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    ASSERT(0);
    return (-1);
  }

  cmd_ptr->hdr.cmd_id = DS_CMD_3GPP_LOOPBACK_HDLR;
  cmd_info_ptr = (ds_loopback_hdlr_cmd_info_type*)cmd_ptr->cmd_payload_ptr;

  cmd_info_ptr->id = DS_LOOPBACK_HDLR_PHYS_LINK_FLOW_DISABLE_CMD;
  cmd_info_ptr->user_data = (void*)callback_data;
  ds_put_cmd(cmd_ptr);
 
  return(0); 
}


/*===========================================================================
FUNCTION DS_LOOPBACK_HDLR_IFACE_SETUP_BRIDGE_INTF_EVENT

DESCRIPTION
  This function calls the registration function to respond to rmnet's flow disable/enable events
  
PARAMETERS  
    
DEPENDENCIES 
  None.

RETURN VALUE 
 0 on SUCCESS
 
SIDE EFFECTS 
  None.
  
===========================================================================*/
int ds_loopback_hdlr_iface_setup_bridge_intf_event 
( 
  ps_iface_type *this_iface_ptr,
  ps_iface_type *target_iface_ptr,
  void *client_data
)
{
  ds_loopback_iface_s *ds_loopback_iface_ptr = NULL;
  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (target_iface_ptr != NULL)
  {
    ds_loopback_iface_ptr = ((ds_loopback_iface_s*)(this_iface_ptr->client_data_ptr));
    if (ps_iface_event_cback_reg(target_iface_ptr,  IFACE_FLOW_ENABLED_EV,
                               ds_loopback_iface_ptr->rm_flow_enable_buf_ptr) < 0)
    {
      DS_3GPP_MSG0_ERROR("IFACE_FLOW_ENABLED_EV = NULL");    
    }
    if (ps_iface_event_cback_reg(target_iface_ptr,  IFACE_FLOW_DISABLED_EV,
                               ds_loopback_iface_ptr->rm_flow_disable_buf_ptr) < 0)
    {
      DS_3GPP_MSG0_ERROR("IFACE_FLOW_DISABLED_EV = NULL");    
    }
  }
  return 0;
}


/*===========================================================================
FUNCTION DS_LOOPBACK_HDLR_RX_WM_NON_EMPTY_CB

DESCRIPTION
  Each enque function for rx_wm of loopback iface
  
PARAMETERS  
    
DEPENDENCIES 
  None.

RETURN VALUE 
  None
 
SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_loopback_hdlr_rx_wm_non_empty_cb (dsm_watermark_type *wm,
                                                         void* callback_data)
{
  ds_loopback_iface_s * loopback_iface_ptr;  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  loopback_iface_ptr = (ds_loopback_iface_s *) callback_data;

  PS_SET_SIGNAL(loopback_iface_ptr->rx_sig);  
}


/*===========================================================================
FUNCTION DS_LOOPBACK_HDLR_RX_UM_DATA_CB

DESCRIPTION
  This function is a Signal handler for rx_wm of loopback iface. It dequeues the packet from the 
  rx_wm queue and gives it to physlink
  
PARAMETERS  
    
DEPENDENCIES 
  None.

RETURN VALUE 
  FALSE - if queue needs to be checked again
  TRUE - if the queue is empty
 
SIDE EFFECTS 
  None.
  
===========================================================================*/
unsigned char ds_loopback_hdlr_rx_um_data_cb 
( 
  ps_sig_enum_type sig, 
  void *user_data_p
)           
{
  dsm_item_type        *item_ptr;       
  ds_loopback_iface_s  * loopback_iface_ptr;
  ds_loopback_iface_cntx_wm_s *rx_watermark_ptr;
  
  loopback_iface_ptr = (ds_loopback_iface_s *) user_data_p;
  
  if(loopback_iface_ptr->rm_flow_enabled == TRUE)
  {
    rx_watermark_ptr = &(loopback_iface_ptr->rx_wm);

    item_ptr = (dsm_item_type *)dsm_dequeue(&(rx_watermark_ptr->wm_item));      
    if (item_ptr != NULL)
    {
      DS_3GPP_MSG1_HIGH("ip rx um data cb. loopback iface instance:%d ",\
        (uint32)loopback_iface_ptr->client_data_ptr);

      (void)ps_phys_link_input(&(loopback_iface_ptr->phys_link), &item_ptr, NULL);      
      return FALSE;
    }
  }

  return TRUE;
}


/*===========================================================================
FUNCTION DS_LOOPBACK_HDLR_RT_ACL

DESCRIPTION:   
 This is the incoming routing ACL definition function for loopback iface.
 This ACL checks if the loopback mode handler is enabled for the requested iface type and if so
 returns the highest priority so that loopback iface is selected
 
PARAMETERS
 this_if_ptr      : ptr to the current interface.
 policy_info_ptr  : ptr to the ACL policy.
 pkt_info_ptr     : ptr to the pkt_info


RETURN VALUE 
  None
 
SIDE EFFECTS 
  None.
  
===========================================================================*/
ACL_DEF(ds_loopback_hdlr_rt_acl)
ACL_START
{
  DENY( DS_LOOPBACK_HDLR_IFACE_BEING_USED() );
  loopback_result = ps_sys_conf_get(PS_SYS_TECH_ALL, PS_SYS_CONF_ENABLE_LOOPBACK,
									                  (void*)&enable_loopback, &loopback_ps_errno);
  if ((loopback_result >= 0) && (enable_loopback == TRUE))
  {
    PASS_QUICK(ACL_LOOPBACK_CLASS, TRUE);
  }
  else
  {
    PASS_QUICK(ACL_LOOPBACK_CLASS, DS_LOOPBACK_HDLR_IFACE_LOOPBACK_ENABLED());
  }
  DENY(TRUE);
}
ACL_END


/*===========================================================================
FUNCTION DS_LOOPBACK_HDLR_RT_ACL_POST_PROC

DESCRIPTION
   This function performs the post routing lookup actions.  This funtion 
   is called for the interface which is picked up as the most preferred 
   interface during lookup based upon client requirements (policy) and 
   other system parameters. Here we set the in_use flag on the iface 
   indicating this instance of loopback iface is being used
 
PARAMETERS
 this_if_ptr      : ptr to the current interface.
 policy_info_ptr  : ptr to the ACL policy.
 pkt_info_ptr     : ptr to the pkt_info

RETURN VALUE 
  None
 
SIDE EFFECTS 
  None.  
===========================================================================*/
ACL_POST_PROC_DEF(ds_loopback_hdlr_rt_acl_post_proc)
{
  ps_iface_type *if_ptr = (ps_iface_type *) this_if_ptr;
  ds_loopback_iface_s *ds_loopback_iface_ptr = NULL;
  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(if_ptr != NULL);

  ds_loopback_iface_ptr = (ds_loopback_iface_s*) if_ptr->client_data_ptr;
  
  ASSERT(ds_loopback_iface_ptr != NULL);  
   
  rex_enter_crit_sect(&ds_loopback_hdlr_global_crit_sect);

  if(policy_info_ptr->bring_up)
  {
    ds_loopback_iface_ptr->in_use = TRUE;
    if (policy_info_ptr->ip_family == IPV6_ADDR )
    {
      ps_iface_set_addr_family(if_ptr,IFACE_IPV6_ADDR_FAMILY);
    }
    else
    {
      ps_iface_set_addr_family(if_ptr,IFACE_IPV4_ADDR_FAMILY);
    }
  }
  
  rex_leave_crit_sect(&ds_loopback_hdlr_global_crit_sect);
  
}


/*===========================================================================
FUNCTION DS_LOOPBACK_HDLR_TX_DATA_CB

DESCRIPTION
  This function modifies and loops ping and iperf pkts received back to
  RX WM
  This function is registered with ps_iface for a PDP-IP call 
  
PARAMETERS  
  *iface_ptr       - Interface ptr for the call.
  **item_ptr       - Payload
  *meta_info_ptr   - Not used.
  *pkt_instance    - Call instance.
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  0 - Success
  
SIDE EFFECTS 
  None.

===========================================================================*/
int ds_loopback_hdlr_tx_data_cb
(
  ps_iface_type     *iface_ptr,
  dsm_item_type     **item_ptr,
  ps_meta_info_type *meta_info_ptr,
  void              *pkt_instance
  /*lint -esym(715,pkt_instance), -esym(818,pkt_instance)*/
)
{
  int ndup = 0;
  static int npkt;
  dsm_item_type * dup_pkt_ptr;
  uint16 lpkt = 0;

  ds_loopback_iface_s  *ds_loopback_iface_ptr;  
    
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ds_loopback_iface_ptr = (ds_loopback_iface_s*)iface_ptr->client_data_ptr;

  if (UL_ONLY) 
  {
    dsm_free_packet(item_ptr);
    goto quick_ret;
  }
  
  (void)dsumtsps_loopback_convert_packet(item_ptr, NULL);

  /*-------------------------------------------------------------------------
    If DUP factor is non zero, then dup every n'th packet and enqueue it 
  -------------------------------------------------------------------------*/
  if (DL_DUPF) 
  {
    if (++npkt == DL_DUPF) 
    {
      lpkt = (uint16)dsm_length_packet(*item_ptr);

      if(lpkt != dsm_dup_packet(&dup_pkt_ptr, *item_ptr, 0, lpkt))
      {
        ASSERT(0);
        dsm_free_packet(item_ptr);
        goto quick_ret;
      }

      dsm_enqueue(&(ds_loopback_iface_ptr->rx_wm.wm_item),&dup_pkt_ptr);

      npkt = 0;
    }
  }

  /*-------------------------------------------------------------------------
    If MUL factor is non zero, then make n copies and enqueue them
  -------------------------------------------------------------------------*/
  if (DL_MULF) 
  {
    if (lpkt == 0) 
    {
      lpkt = (uint16)dsm_length_packet(*item_ptr);
    }

    for (ndup = 0; ndup < DL_MULF; ++ndup) 
    {
      if(lpkt != dsm_dup_packet(&dup_pkt_ptr, *item_ptr, 0, lpkt))
      {
        ASSERT(0);
        dsm_free_packet(item_ptr);
        goto quick_ret;
      }
      dsm_enqueue(&(ds_loopback_iface_ptr->rx_wm.wm_item),&dup_pkt_ptr);
    }
  }

  /*-------------------------------------------------------------------------
    Enqueue the packet on RX watermark and set a signal to PS to pick it up
    or send it to a registered handler
  -------------------------------------------------------------------------*/
  dsm_enqueue(&(ds_loopback_iface_ptr->rx_wm.wm_item),item_ptr);  

quick_ret:
  /*lint -e506 -e774*/
  PS_META_INFO_FREE(&meta_info_ptr); 
  /*lint +e506 +e774*/
  return 0;  
}  


/*===========================================================================
FUNCTION DS_LOOPBACK_HDLR_INIT_WMS

DESCRIPTION
  This function initializes the rx_wm queue
  
PARAMETERS  
    
DEPENDENCIES 
  None.

RETURN VALUE 
 0 on SUCCESS
 
SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_loopback_hdlr_init_wms(int iface_inst)
{
  
  dsm_queue_init(&loopback_iface_tbl[iface_inst].rx_wm.wm_item,
                  120000,
                  &loopback_iface_tbl[iface_inst].rx_wm.wm_queue);  

  return;
}  


/*===========================================================================
  FUNCTION DS_LOOPBACK_HDLR_SET_NONEMPTY_CALLBACK
  
  DESCRIPTION
    This function sets the loopback mode handler's non empty callback for the 
    iface_inst provided
    
  PARAMETERS  
  iface_inst    Loopback call instance
      
  DEPENDENCIES 
    None.
  
  RETURN VALUE 
    None
   
  SIDE EFFECTS 
    None.
    
  ===========================================================================*/
void  ds_loopback_hdlr_set_nonempty_callback(int iface_inst)
{
  if((iface_inst <DS_LOOPBACK_HDLR_NUM_IFACES) && (iface_inst>= 0))
  {
    DS_3GPP_MSG1_HIGH("Set Nonempty callback for loopback iface instance %d",
                        iface_inst);
  
    loopback_iface_tbl[iface_inst].rx_wm.wm_item.non_empty_func_ptr = 
                                          ds_loopback_hdlr_rx_wm_non_empty_cb;

    loopback_iface_tbl[iface_inst].rx_wm.wm_item.non_empty_func_data = 
                                               &loopback_iface_tbl[iface_inst];
  }
}


/*===========================================================================
  FUNCTION DS_LOOPBACK_HDLR_RESET_NONEMPTY_CALLBACK
  
  DESCRIPTION
    This function resets the loopback mode handler's non empty callback for the 
    iface_inst provided
    
  PARAMETERS  
  iface_inst    Loopback call instance
      
  DEPENDENCIES 
    None.
  
  RETURN VALUE 
    None
   
  SIDE EFFECTS 
    None.
    
  ===========================================================================*/
void  ds_loopback_hdlr_reset_nonempty_callback(int iface_inst)
{

  if((iface_inst <DS_LOOPBACK_HDLR_NUM_IFACES) && (iface_inst>= 0))
  {
    DS_3GPP_MSG1_HIGH("ReSet Nonempty callback for loopback iface instance %d",
                        iface_inst);  

    loopback_iface_tbl[iface_inst].rx_wm.wm_item.non_empty_func_ptr = NULL;

    loopback_iface_tbl[iface_inst].rx_wm.wm_item.non_empty_func_data = NULL;
  }
}  
/*===========================================================================
FUNCTION DS_LOOPBACK_HDLR_RM_EVENT_HANDLER_CB

DESCRIPTION

PARAMETERS  
    
DEPENDENCIES 
  None.

RETURN VALUE 
  None
 
SIDE EFFECTS 
  None.
  
===========================================================================*/
LOCAL void ds_loopback_hdlr_rm_event_handler_cb
(
  ps_iface_type               *this_iface_ptr,
  ps_iface_event_enum_type     event,
  ps_iface_event_info_u_type   event_info,
  void                        *user_data_ptr
)
{
  ds_loopback_iface_s *ds_loopback_iface_ptr; 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_loopback_iface_ptr = (ds_loopback_iface_s*)user_data_ptr;

  ASSERT(ds_loopback_iface_ptr != NULL);

  /*---------------------------------------------------------------------------
    Take action based on the event. Set the Rm_flow_controlled flag in the 
    bearer context according to the event received. This flag is used in the 
    rx function call back from PDCP to determine if we should dequeue data from
    the DL wm.
  ---------------------------------------------------------------------------*/
  switch(event)
  {
    case IFACE_FLOW_ENABLED_EV:

      ds_loopback_iface_ptr->rm_flow_enabled = TRUE;
      DS_3GPP_MSG1_HIGH("IFACE_FLOW_ENABLED_EV for loopback iface instance %d",
                        ds_loopback_iface_ptr->client_data_ptr);
      PS_SET_SIGNAL(ds_loopback_iface_ptr->rx_sig);
      break;

    case IFACE_FLOW_DISABLED_EV:
      
      ds_loopback_iface_ptr->rm_flow_enabled = FALSE;
      DS_3GPP_MSG1_HIGH("IFACE_FLOW_DISABLED_EV for loopback iface instance %d",
                        ds_loopback_iface_ptr->client_data_ptr);
      break;
      
    default:
      {
        DS_3GPP_MSG1_HIGH("Unknown event: %d. Ignoring",event);
      }
  }
}


/*===========================================================================
FUNCTION DS_LOOPBACK_HDLR_PHYS_LINK_FLOW_DISABLE_CMD_HDLR

DESCRIPTION

Called in DS TASK context in response to an flow disable command
on the loopback iface. 

PARAMETERS  

ds_loopback_iface_ptr
    
DEPENDENCIES 
  None.

RETURN VALUE 
  None
 
SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_loopback_hdlr_phys_link_flow_disable_cmd_hdlr
(
  ds_loopback_iface_s *     ds_loopback_iface_ptr
)
{
   if (ds_loopback_iface_ptr == NULL) {
      return;
   }
  /*----------------------------------------------------------------------- 
  Disable flow only if the NV is not turned on,
  This is to support an IPA request
  -----------------------------------------------------------------------*/
  if (!loopback_flow_ctrl_disable)
  {
    ps_phys_link_disable_flow (&ds_loopback_iface_ptr->phys_link, DS_FLOW_UMTS_MASK);  
  }
  else
  {
    DS_3GPP_MSG0_HIGH("In flow disable cmd hdlr, ignoring.. ");
  }
}


/*===========================================================================
FUNCTION DS_LOOPBACK_HDLR_INIT_WMS

DESCRIPTION
  This function initializes the loopback mode handler, creates loopback ifaces, sets the cmd handlers
  for the ifaces, initializes acl
  
PARAMETERS  
    
DEPENDENCIES 
  None.

RETURN VALUE 
  None
 
SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_loopback_hdlr_init()
{
  int i =0 ;
  int iface_inst = 0;
  acl_type      *acl_ptr = NULL;  
  ps_iface_type *iface_ptr = NULL;
  boolean ds_3gpp_test_loopback_enable = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  rex_init_crit_sect(&ds_loopback_hdlr_global_crit_sect);
  if (ds_3gpp_cfg_get( DS_3GPP_TEST_LOOPBACK_ENABLE, 
                      (boolean *)&(ds_3gpp_test_loopback_enable), 
                       sizeof(ds_3gpp_test_loopback_enable)
                     ) > 0)
  {
    DS_3GPP_MSG1_HIGH("ds_3gpp_test_loopback_enable set to %d in EFS", 
                       ds_3gpp_test_loopback_enable);
  }
  for (i = 0; i < DS_LOOPBACK_HDLR_NUM_IFACES; i++)
  {
    memset(&loopback_iface_tbl[i], 0, sizeof(ds_loopback_iface_s));
    
    ds_loopback_hdlr_init_wms(i);

    // initialize IP addresses for each interface
    loopback_iface_tbl[i].client_data_ptr = (void *) i;    

    acl_ptr = &loopback_iface_tbl[i].acl;
    acl_ptr->acl_fptr = ds_loopback_hdlr_rt_acl;
    acl_ptr->post_proc_fptr = ds_loopback_hdlr_rt_acl_post_proc;

    iface_ptr = &(loopback_iface_tbl[i].ps_iface);
    
    iface_inst = ps_iface_create(iface_ptr, UMTS_IFACE, acl_ptr, NULL,
                    &(loopback_iface_tbl[i].phys_link), 1); 

    ASSERT(iface_inst >= 0);
    
    DS_3GPP_MSG1_HIGH("Loopback iface created inst:%d",iface_inst);    

    /*-----------------------------------------------------------------------
      These bring up/tear down the Interface.
    -----------------------------------------------------------------------*/
    iface_ptr->bring_up_cmd_f_ptr   = ds_loopback_hdlr_iface_up_cmd;
    iface_ptr->tear_down_cmd_f_ptr  = ds_loopback_hdlr_iface_down_cmd;

    /*-----------------------------------------------------------------------
      Active iface in_use/out_use cmd callback.
    -----------------------------------------------------------------------*/      
    iface_ptr->bridge_proc_f_ptr = ds_loopback_hdlr_iface_setup_bridge_intf_event;  

    iface_ptr->client_data_ptr = (void *) &loopback_iface_tbl[i];       
    
    /*-----------------------------------------------------------------------
      Reset iface tx cmd as well
    -----------------------------------------------------------------------*/
    ps_iface_set_tx_function(iface_ptr, ds_loopback_hdlr_tx_data_cb, &loopback_iface_tbl[i]);

    ps_iface_enable_ind(iface_ptr);

    /*-------------------------------------------------------------*/
    loopback_iface_tbl[i].rx_sig = 
      (ps_sig_enum_type)((uint32)PS_3GPP_UM_RX_Q_SIGNAL_0 + (i));

    /*-----------------------------------------------------------------------
      Enable flow on the interface.
    -----------------------------------------------------------------------*/    
    ps_iface_enable_flow(iface_ptr, DS_FLOW_UMTS_MASK);

    loopback_iface_tbl[i].phys_link.phys_link_down_cmd_f_ptr = 
                                ds_loopback_hdlr_phys_link_down_cmd;
    
    loopback_iface_tbl[i].phys_link.phys_link_up_cmd_f_ptr = 
                                ds_loopback_hdlr_phys_link_up_cmd;
    if (ds_3gpp_test_loopback_enable == TRUE)
    {
      is_loopback_enabled[i] = TRUE;
    }
  }
  /*---------------------------------------------------------------------------
  If ds_3gpp_test_loopback is enabled allocate heap memory on Init
  ----------------------------------------------------------------------------*/
#ifndef TEST_FRAMEWORK
  if(ds_3gpp_test_loopback_enable)
  {
    ds_loopback_hdlr_alloc_dyn_byte();
    ds_loopback_hdlr_alloc_dyn_unsigned_char();
    
   /*------------------------------------------------------------------------- 
    Make a decision to read the Flow control disable NV only if loopback
    is enabled and populate the appropriate variable
   ------------------------------------------------------------------------*/
    if(ds_3gpp_cfg_get(DS_3GPP_LOOPBACK_FLOW_CTRL_DISABLE,
              (boolean *) &loopback_flow_ctrl_disable,
               sizeof(loopback_flow_ctrl_disable)) == -1)
    {
      DS_3GPP_MSG0_ERROR("Cannot read EFS Config item DS_3GPP_LOOPBACK_FLOW_CTRL_DISABLE"
			 "_DISABLE");
      loopback_flow_ctrl_disable = FALSE;
    }
    else{
       DS_3GPP_MSG1_HIGH("EFS Config item DS_3GPP_LOOPBACK_FLOW_CTRL_DISABLE read: %d",loopback_flow_ctrl_disable);
    }
  }
#else
  #error code not present
#endif
/*--------------------------------------------------------------------------- 
  Initialize the WM levels based on the NV loopback_flow_ctrl_disable•
  --------------------------------------------------------------------------*/
  if (loopback_flow_ctrl_disable)
  {
    ds_loopback_hdlr_wm_levels.ul_wm_hi = DS_LOOPBACK_HDLR_UL_WM_HI_1;
    ds_loopback_hdlr_wm_levels.ul_wm_dne = DS_LOOPBACK_HDLR_UL_WM_DNE_1;
  }
  else
  {
    ds_loopback_hdlr_wm_levels.ul_wm_hi =  DS_LOOPBACK_HDLR_UL_WM_HI;
    ds_loopback_hdlr_wm_levels.ul_wm_dne = DS_LOOPBACK_HDLR_UL_WM_DNE;
  }

  ds_loopback_hdlr_wm_levels.ul_wm_lo = DS_LOOPBACK_HDLR_UL_WM_LO;

}

/*===========================================================================
FUNCTION DS_LOOPBACK_HDLR_CHECK_ALL_OTHER_IFACES_ARE_DOWN

DESCRIPTION
This function checks if all of the loopback ifaces are in down state

PARAMETERS  
input iface ptr
 
DEPENDENCIES 
  None.

RETURN VALUE 
  boolean TRUE -- If all loopback ifaces are down
	   		  FALSE -- Otherwise
 
SIDE EFFECTS 
  None.
  
===========================================================================*/
static boolean ds_loopback_hdlr_check_all_other_ifaces_are_down
(
  ps_iface_type *input_iface_ptr
)
{ 
  uint8 iface_count = 0;
  boolean all_other_ifaces_down = TRUE;
  ps_iface_type *iface_ptr = NULL;
/*------------------------------------------------------------------------*/
  rex_enter_crit_sect(&ds_loopback_hdlr_global_crit_sect);

  /*-------------------------------------------------------------------------- 
    Loop through every iface except for the passed iface to determine if
	 any iface is UP
  ---------------------------------------------------------------------------*/
  for (iface_count = 0;iface_count < DS_LOOPBACK_HDLR_NUM_IFACES;iface_count++)
  {
  
  	iface_ptr = &(loopback_iface_tbl[iface_count].ps_iface);

    if((iface_ptr != input_iface_ptr) && 
		((ps_iface_state(iface_ptr) == IFACE_UP) || 
		(ps_iface_state(iface_ptr) == IFACE_ROUTEABLE)))

	{
		  DS_3GPP_MSG0_MED("Not all loopback ifaces are down");
		  all_other_ifaces_down = FALSE;
		  break;
	}
	
  }

  rex_leave_crit_sect(&ds_loopback_hdlr_global_crit_sect);

  return all_other_ifaces_down;
}


/*===========================================================================
FUNCTION DS_LOOPBACK_HDLR_CHECK_IF_LOOPBACK_IFACE

DESCRIPTION
This function checks the input iface is loopback iface or not 
 
PARAMETERS  
input iface ptr
 
DEPENDENCIES 
  None.

RETURN VALUE 
  boolean TRUE -- iface is found in the loopback iface table
          FALSE -- otherwise
 
SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean ds_loopback_hdlr_check_if_loopback_iface
(
  ps_iface_type *input_iface_ptr
)
{ 
  uint8 iface_count = 0;
  boolean iface_found = FALSE;
  ps_iface_type *iface_ptr = NULL;
/*------------------------------------------------------------------------*/

  /*-------------------------------------------------------------------------- 
    Loop through every iface to check if any of the ifaces matches with input
    iface
  ---------------------------------------------------------------------------*/
  for (iface_count = 0;iface_count < DS_LOOPBACK_HDLR_NUM_IFACES;iface_count++)
  {
  
    iface_ptr = &(loopback_iface_tbl[iface_count].ps_iface);
    if(iface_ptr == input_iface_ptr)
    {
      DS_3GPP_MSG1_MED("iface :%x is loopback iface ", iface_ptr);
      iface_found = TRUE;
      break;
    }
  }

  return iface_found;
}
/*ds_loopback_hdlr_check_if_loopback_iface*/

/*===========================================================================
FUNCTION DS_LOOPBACK_HDLR_IFACE_UP_CMD_HDLR

DESCRIPTION

Called in DS TASK context in response to an iface up command on the loopback iface. Brings up 
the iface requested after setting ip address, rx_wm signal handlers high and low water mark 
call back functions

PARAMETERS  
    
 ps_iface_type *iface_ptr: ptr to iface which is going down
 void * info_ptr:
 
DEPENDENCIES 
  None.

RETURN VALUE 
  None
 
SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_loopback_hdlr_iface_up_cmd_hdlr
(
  ps_iface_type *iface_ptr,
  void * info_ptr
)
{
  int16 ps_errno = 0;
  ps_ip_addr_type ip_addr;
  int loopback_iface_inst = 0;
  ps_phys_link_type *phys_link_ptr = NULL;         /* Ptr to the phys link of call*/
  ds_loopback_iface_s * ds_loopback_iface_ptr = NULL;
  ps_phys_link_link_protocol_handle_type protocol_handle;  
  ps_dpm_um_reg_bearer_info_type   reg_info;
  int32 ps_dpm_reg_ret = 0;
  uint64  v6_gateway_iid = 0x70F70F70F70F70FULL;
  uint64  prefix_address = 0x80F80F80F80F80FULL;  
  uint32  ipv4_addr = 0xAB30A8C0;
  uint64  ipv6_addr = 0xB30A8C0AB30A8C0ULL;
  uint32  prefix_valid_life_time = 2147483647;
  uint32  prefix_pref_life_time = 2147483647;
  uint16  router_life_time = 65535;
  uint8   prefix_length = 64;
  ps_iface_down_mh_event_info_type   down_info;

  ds_loopback_iface_ptr = (ds_loopback_iface_s*)(iface_ptr->client_data_ptr);
  loopback_iface_inst = (int) ds_loopback_iface_ptr->client_data_ptr;
  memset(&reg_info, 0, sizeof(ps_dpm_um_reg_bearer_info_type));
  memset (&protocol_handle, 0,sizeof(ps_phys_link_link_protocol_handle_type));
  DS_3GPP_MSG1_HIGH("DSLOOPBACK iface up cmd hdlr loopback Iface Inst:%d",\
                     loopback_iface_inst);  

  memset(&down_info, 0, sizeof(ps_iface_down_mh_event_info_type));

  down_info.down_reason = PS_NET_DOWN_REASON_VALIDATION_FAILURE;
  down_info.bearer_tech.technology = DS_SYS_NETWORK_3GPP;
  down_info.bearer_tech.rat_value = DS_SYS_RAT_UNKNOWN;
  down_info.bearer_tech.so_mask = DS_SYS_SO_EX_UNSPECIFIED;

  /*-------------------------------------------------------------------------
    Make sure the iface state is down. Can bring the primary call up only
    for the iface which is in down state.
  -------------------------------------------------------------------------*/
  if (ps_iface_state(iface_ptr) == IFACE_UP )
  {
    DS_3GPP_MSG1_ERROR("Invalid iface state for iface Inst %d",loopback_iface_inst);
    ASSERT(0);
    return;
  }
  
  /*-------------------------------------------------------------------------
    Get the Primary Phys link pointer and make sure the phys link is down
  -------------------------------------------------------------------------*/
  phys_link_ptr = PS_IFACE_GET_PHYS_LINK(iface_ptr);

  if (phys_link_ptr == NULL) 
  {
      DATA_ERR_FATAL("phys_link_ptr = NULL");
      return;
  }
  
  if( PS_PHYS_LINK_GET_STATE(phys_link_ptr) == PHYS_LINK_UP )
  {
    DATA_ERR_FATAL("Phys link already UP");
    return;
  }  
  
	/*-------------------------------------------------------------------------- 
	 	Check if dynamic memory is allocated, If not try allocating it again
	 	If unsuccessful don't bring up the call
	 	--------------------------------------------------------------------------*/
	if (tmp_pkt == NULL)
	{
		ds_loopback_hdlr_alloc_dyn_unsigned_char();
		if (tmp_pkt == NULL)
		{
			DS_3GPP_MSG0_ERROR("Memory allocation failure,iface_up failure");
                         ps_iface_down_ind(iface_ptr,&down_info);
			return;
		}
		else
		{
			DS_3GPP_MSG0_HIGH("Dynamic memory allocated in iface_up_cmd_hdlr");
		}
	}


	if (data_ptr == NULL)
	{
		ds_loopback_hdlr_alloc_dyn_byte();
		if (data_ptr == NULL)
		{	
			DS_3GPP_MSG0_ERROR("Memory allocation failure,iface_up failure");
      ps_iface_down_ind(iface_ptr,&down_info);
			return;
		}
		else
		{
			DS_3GPP_MSG0_HIGH("Dynamic memory allocated in iface_up_cmd_hdlr");
		}
	}
  /*-------------------------------------------------------------------------
   Set the handler for the downlink signal for the call.
   -------------------------------------------------------------------------*/
  (void)ps_set_sig_handler(
                  ds_loopback_iface_ptr->rx_sig,
                  ds_loopback_hdlr_rx_um_data_cb, 
                  (void *)ds_loopback_iface_ptr);
  
  /*-------------------------------------------------------------------------  
    Enable it.
    -------------------------------------------------------------------------*/
  ps_enable_sig(ds_loopback_iface_ptr->rx_sig);  
  
  /*----------------------------------------------------------------------
      Assume primary IP call and hardcode the address
      -----------------------------------------------------------------------*/ 
  if (ps_iface_get_addr_family(iface_ptr) == IFACE_IPV6_ADDR_FAMILY)
  {
    iface_ptr->addr_cb_f_ptr = NULL;
    iface_ptr->dad_f_ptr = NULL;
    /*---------------------------------------------------------------------
      Set V6 IP Addr to Iface
    ---------------------------------------------------------------------*/
    if (FALSE == ps_iface_set_v6_iid(iface_ptr,ipv6_addr+loopback_iface_inst))
    {
      DS_3GPP_MSG0_HIGH("ps_iface_set_ip_config_to_iface(): "
                    "Set v6 IID to iface failed");
    } 
    if (-1 == ps_iface_apply_v6_prefix(
                       iface_ptr,
                       v6_gateway_iid,   
                       prefix_address,
                       prefix_valid_life_time,
                       prefix_pref_life_time, 
                       prefix_length))
    {
      DS_3GPP_MSG0_HIGH("ps_iface_set_ip_config_to_iface(): "
        "Apply V6 Prefix to iface failed");
    }

    /*--------------------------------------------------------------------
     Applied prefix successfully Generate a valid RA indication
    --------------------------------------------------------------------*/
     ps_iface_valid_ra_ind(iface_ptr,router_life_time);
     protocol_handle.none_handle.handle.ip_proto_handle.v6_iface_ptr = iface_ptr;

  }
  else
  {
    ip_addr.type = IPV4_ADDR;
    ip_addr.addr.v4.ps_s_addr = ipv4_addr + loopback_iface_inst;
    if(ps_iface_set_addr(iface_ptr, &ip_addr)!=0)
    {
      DS_3GPP_MSG1_HIGH("LPBK: Setting address failed for iface_ptr: %d",iface_ptr);
    }  
    protocol_handle.none_handle.handle.ip_proto_handle.v4_iface_ptr = iface_ptr;
  }

  if ((ds_loopback_iface_ptr->rm_flow_enable_buf_ptr =\
          ps_iface_alloc_event_cback_buf(ds_loopback_hdlr_rm_event_handler_cb,
                                       (void*)ds_loopback_iface_ptr)) == NULL)
  {
      /* Need to see how else to handler this */
    DS_3GPP_MSG0_ERROR(" Could not allocate event buffers");
  }

  if ((ds_loopback_iface_ptr->rm_flow_disable_buf_ptr =\
          ps_iface_alloc_event_cback_buf(ds_loopback_hdlr_rm_event_handler_cb,
                                       (void*)ds_loopback_iface_ptr)) == NULL)
  {
      /* Need to see how else to handler this */
    DS_3GPP_MSG0_ERROR(" Could not allocate event buffers");
  }
  
  dsm_empty_queue(&ds_loopback_iface_ptr->rx_wm.wm_item);
  
  ds_loopback_iface_ptr->rx_wm.wm_item.total_rcvd_cnt = 0;
  ds_loopback_iface_ptr->rx_wm.wm_item.lowater_func_ptr = 
                              (wm_cb_type) ds_loopback_hdlr_ps_flow_low_watermark_cb;
  
  ds_loopback_iface_ptr->rx_wm.wm_item.lowater_func_data = 
                              (void *) ds_loopback_iface_ptr;
  
  ds_loopback_iface_ptr->rx_wm.wm_item.hiwater_func_ptr = 
                              (wm_cb_type) ds_loopback_hdlr_ps_flow_high_watermark_cb;
  
  ds_loopback_iface_ptr->rx_wm.wm_item.hiwater_func_data = 
                              (void *) ds_loopback_iface_ptr;

  dsm_set_low_wm(&ds_loopback_iface_ptr->rx_wm.wm_item,ds_loopback_hdlr_wm_levels.ul_wm_lo);
  dsm_set_hi_wm(&ds_loopback_iface_ptr->rx_wm.wm_item,ds_loopback_hdlr_wm_levels.ul_wm_hi);
  dsm_set_dne(&ds_loopback_iface_ptr->rx_wm.wm_item,ds_loopback_hdlr_wm_levels.ul_wm_dne);

   
                              
  protocol_handle.none_handle.high_protocol = PS_PHYS_LINK_HIGHER_LAYER_PROTOCOL_IP;

  /*---------------------------------------------------------------------------
    Set the link layer protocol to NONE and pass the protocol handle into the
    function that will do this. The protocol handle has just been populated
    with the higher layer protocol and the pointers to the ifaces that might
    communicate over this physlink.
  ---------------------------------------------------------------------------*/
 (void) ps_phys_link_set_link_protocol(&(ds_loopback_iface_ptr->phys_link),
                                         PS_PHYS_LINK_LINK_PROTOCOL_NONE,
                                         protocol_handle);    

  switch(PS_PHYS_LINK_GET_STATE(phys_link_ptr))
  {
    case PHYS_LINK_DOWN:
    case PHYS_LINK_NULL:
      
      (void)ps_phys_link_up_cmd(phys_link_ptr,
                                &ps_errno,
                                info_ptr);
        
      ps_phys_link_enable_flow(phys_link_ptr, DS_FLOW_UMTS_MASK);
      ps_phys_link_up_ind(phys_link_ptr);  
      
      break;
      
    default:
      DS_3GPP_MSG1_ERROR("Iface_up_cmd called in unexpected state: %d.", PS_PHYS_LINK_GET_STATE(phys_link_ptr));
      return;
  }/* Switch */

  ps_flow_activate_ind(PS_IFACE_GET_DEFAULT_FLOW(iface_ptr),
                      PS_EIC_NOT_SPECIFIED);  
  reg_info.rat = PS_SYS_RAT_EX_3GPP_WCDMA;
  /* Enhancement Needed to support all rats */
  if (ps_iface_get_addr_family(iface_ptr) == IFACE_IPV6_ADDR_FAMILY)
  {
    reg_info.v6_iface_ptr = iface_ptr;
  }
  {
    reg_info.v4_iface_ptr = iface_ptr;
  }
  reg_info.l2_to_ps_wm_ptr  =  &(ds_loopback_iface_ptr->rx_wm.wm_item);
  reg_info.ps_phys_link_ptr = phys_link_ptr;
  
  ps_dpm_reg_ret = ps_dpm_um_reg_bearer(&reg_info);

  if (0 == ps_dpm_reg_ret)
  {
    DS_3GPP_MSG0_HIGH("Dpm Registration Succesful");
  }
  ps_iface_up_ind(iface_ptr);

  /* Calling A2 API to enable peak throughput for loopback */	
}


/*===========================================================================
FUNCTION DS_LOOPBACK_HDLR_IFACE_DOWN_CMD_HDLR

DESCRIPTION

Called in DS TASK context in response to an iface down command
on the loopback iface. 

PARAMETERS  

 ps_iface_type *iface_ptr: ptr to iface which is going down
 void * info_ptr:
 
DEPENDENCIES 
  None.

RETURN VALUE 
  None
 
SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_loopback_hdlr_iface_down_cmd_hdlr
(
  ps_iface_type *iface_ptr,
  void * info_ptr
)
{ 
  int16                     ps_errno;        /* Errno value                */
  ps_phys_link_type         *phys_link_ptr; /* Ptr to the phys link of call*/
  ds_loopback_iface_s *     ds_loopback_iface_ptr = NULL;
  ps_dpm_um_dereg_bearer_info_type  dereg_info;
  int32 ps_dpm_dereg_ret = 0;
  ps_iface_down_mh_event_info_type   down_info;
  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  memset(&down_info, 0, sizeof(ps_iface_down_mh_event_info_type));
  down_info.down_reason = PS_NET_DOWN_REASON_CLIENT_END;

  down_info.bearer_tech.technology = DS_SYS_NETWORK_3GPP;
  down_info.bearer_tech.so_mask = DS_SYS_SO_EX_UNSPECIFIED;
  down_info.bearer_tech.rat_value = DS_SYS_RAT_UNKNOWN;

  ds_loopback_iface_ptr = (ds_loopback_iface_s*)(iface_ptr->client_data_ptr);

  DS_3GPP_MSG1_HIGH("In LOOPBACK IFACE DOWN hdlr: Iface Inst:%d",
    (int) (((ds_loopback_iface_s*)(iface_ptr->client_data_ptr))->client_data_ptr));

  ds_loopback_iface_ptr->in_use = FALSE;

  /*-------------------------------------------------------------------------
    We are going to use the zeroth instance of phys_links in umtsps_iface_tbl.
    Get the pointer and make sure the phys link is down
  -------------------------------------------------------------------------*/
  phys_link_ptr = PS_IFACE_GET_PHYS_LINK(iface_ptr);

  if (phys_link_ptr == NULL) 
  {
     DATA_ERR_FATAL("phys_link_ptr = NULL");
      return;
  }

  /*-------------------------------------------------------------------------
    Send command to bring down the traffic channel.
  -------------------------------------------------------------------------*/
  if ((ps_iface_state(iface_ptr) != IFACE_DOWN) &&
      (ps_iface_state(iface_ptr) != IFACE_DISABLED))
  {
    /*-------------------------------------------------------------------------
     Disable flow on the primary phys link
    -------------------------------------------------------------------------*/
    ps_phys_link_disable_flow(phys_link_ptr,DS_FLOW_UMTS_MASK);
    ps_flow_go_null_ind(PS_IFACE_GET_DEFAULT_FLOW(iface_ptr),
                        PS_EIC_NOT_SPECIFIED);
    
    if((ps_iface_bridge_iface(iface_ptr) != NULL))
    {    
      ps_iface_event_cback_dereg(ps_iface_bridge_iface(iface_ptr), 
                                 IFACE_FLOW_ENABLED_EV,
       	                         ds_loopback_iface_ptr->rm_flow_enable_buf_ptr);

      ps_iface_event_cback_dereg(ps_iface_bridge_iface(iface_ptr), 
                                 IFACE_FLOW_DISABLED_EV,
       	                         ds_loopback_iface_ptr->rm_flow_disable_buf_ptr);
    }
    
    ps_iface_set_bridge(iface_ptr, NULL);     
    

    if(ds_loopback_iface_ptr->rm_flow_enable_buf_ptr != NULL)
    {
      ps_iface_free_event_cback_buf(ds_loopback_iface_ptr->rm_flow_enable_buf_ptr);
      ds_loopback_iface_ptr->rm_flow_enable_buf_ptr = NULL;
    }

    if(ds_loopback_iface_ptr->rm_flow_disable_buf_ptr != NULL)
    {
      ps_iface_free_event_cback_buf(ds_loopback_iface_ptr->rm_flow_disable_buf_ptr);
      ds_loopback_iface_ptr->rm_flow_disable_buf_ptr = NULL;
    }

      DS_3GPP_MSG3_HIGH("Flush Route: Last phys link of the interface",0,0,0);
      (void)route_flush(iface_ptr);            
    
    (void)ps_phys_link_down_cmd(phys_link_ptr, &ps_errno, NULL);

    ps_phys_link_gone_ind(phys_link_ptr);
    
    memset (&dereg_info, 0, sizeof(ps_dpm_um_dereg_bearer_info_type));
    dereg_info.ps_phys_link_ptr = phys_link_ptr;

    if (ps_iface_get_addr_family(iface_ptr) == IFACE_IPV6_ADDR_FAMILY)
    {
      dereg_info.v6_iface_ptr = iface_ptr;
    }
    {
      dereg_info.v4_iface_ptr = iface_ptr;
    }

    ps_dpm_dereg_ret = ps_dpm_um_dereg_bearer(&dereg_info);
    if (0 == ps_dpm_dereg_ret)
    {
      DS_3GPP_MSG0_HIGH("Dpm DeRegistration Succesful");
    }

    ps_iface_down_ind(iface_ptr,&down_info);

    /*------------------------------------------------------------------------ 
       Disable A2 SIO peak data rate only if all other ifaces are down.
     ------------------------------------------------------------------------*/
     if (ds_loopback_hdlr_check_all_other_ifaces_are_down(iface_ptr))
    {
      DS_3GPP_MSG0_MED("Disabling A2 SIO peak data rate");
    }
  }
  return;  
}


/*===========================================================================
FUNCTION DS_LOOPBACK_HDLR_PHYS_LINK_UP_CMD_HDLR

DESCRIPTION

Called in DS TASK context in response to an phys link up command
on the loopback iface. 

PARAMETERS  
    
DEPENDENCIES 
  None.

RETURN VALUE 
  None
 
SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_loopback_hdlr_phys_link_up_cmd_hdlr 
(
  ps_phys_link_type *phys_link_ptr_i,
  void              *info_ptr
)
{  
  ;
}


/*===========================================================================
FUNCTION DS_LOOPBACK_HDLR_PHYS_LINK_DOWN_CMD_HDLR

DESCRIPTION

Called in DS TASK context in response to an phys link down command
on the loopback iface. 

PARAMETERS  
    
DEPENDENCIES 
  None.

RETURN VALUE 
  None
 
SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_loopback_hdlr_phys_link_down_cmd_hdlr 
(
  ps_phys_link_type *phys_link_ptr_i,
  void              *info_ptr
)
{
  ;
}

/*===========================================================================
FUNCTION DS_LOOPBACK_HDLR_PHYS_LINK_FLOW_ENABLE_CMD_HDLR

DESCRIPTION

Called in DS TASK context in response to an flow enable command
on the loopback iface. 

PARAMETERS  

ds_loopback_iface_ptr
    
DEPENDENCIES 
  None.

RETURN VALUE 
  None
 
SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_loopback_hdlr_phys_link_flow_enable_cmd_hdlr
(
  ds_loopback_iface_s *     ds_loopback_iface_ptr
)
{
  ps_phys_link_enable_flow (&ds_loopback_iface_ptr->phys_link, DS_FLOW_UMTS_MASK);  
}


/*===========================================================================
FUNCTION DS_LOOPBACK_PROCESS_PS_CMD

DESCRIPTION
  This function demultiplexes different loopback commands and calls the respective command handlers 
  
PARAMETERS  
    
DEPENDENCIES 
  None.

RETURN VALUE 
  None
 
SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_loopback_process_ps_cmd(const ds_cmd_type *cmd_ptr )
{
  ds_loopback_hdlr_cmd_info_type *cmd_info_ptr;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    DS_3GPP_MSG0_ERROR("NULL ptr passed, return");
    return;
  }

  cmd_info_ptr = (ds_loopback_hdlr_cmd_info_type*)cmd_ptr->cmd_payload_ptr;
  
  /*-------------------------------------------------------------------------
    Invoke the appropriate function to process the command, based on the
    command id.
  -------------------------------------------------------------------------*/
  switch(cmd_info_ptr->id)
  {
    case DS_LOOPBACK_HDLR_IFACE_UP_CMD:
      ds_loopback_hdlr_iface_up_cmd_hdlr
          ((ps_iface_type*)(cmd_info_ptr->user_data),cmd_info_ptr->info_ptr);
      
      break;
      
    case DS_LOOPBACK_HDLR_IFACE_DOWN_CMD:
      ds_loopback_hdlr_iface_down_cmd_hdlr
        ((ps_iface_type*)(cmd_info_ptr->user_data),cmd_info_ptr->info_ptr);
      
      break;      
      
    case DS_LOOPBACK_HDLR_PHYS_LINK_UP_CMD:
      ds_loopback_hdlr_phys_link_up_cmd_hdlr
        ((ps_phys_link_type*)(cmd_info_ptr->user_data),cmd_info_ptr->info_ptr);

      break;
      
    case DS_LOOPBACK_HDLR_PHYS_LINK_DOWN_CMD:
      ds_loopback_hdlr_phys_link_down_cmd_hdlr
        ((ps_phys_link_type*)(cmd_info_ptr->user_data),cmd_info_ptr->info_ptr);

      break;    

    case DS_LOOPBACK_HDLR_PHYS_LINK_FLOW_ENABLE_CMD:      
      ds_loopback_hdlr_phys_link_flow_enable_cmd_hdlr
        ((ds_loopback_iface_s *) (cmd_info_ptr->user_data));      

      break;
      
    case DS_LOOPBACK_HDLR_PHYS_LINK_FLOW_DISABLE_CMD:  
      ds_loopback_hdlr_phys_link_flow_disable_cmd_hdlr
        ((ds_loopback_iface_s *) (cmd_info_ptr->user_data));      

      break;      
  } /* switch( cmd_id ) */

  return;  
} /* ds_loopback_process_ps_cmd() */


/*===========================================================================
FUNCTION DS_LOOPBACK_HDLR_GET_INDEX_FROM_IFACE_TYPE

DESCRIPTION

This function maps an iface_type to loopback enabled array index

PARAMETERS  

iface_type: iface type for which the status is needed
index: o/p parameter used to return the array index
    
DEPENDENCIES 
  None.

RETURN VALUE 
    0  : Success
  -1   : Error 
 
SIDE EFFECTS 
  None.
  
===========================================================================*/  
int ds_loopback_hdlr_get_index_from_iface_type
(
  ps_iface_name_enum_type iface_type,     
  int * index
)
{
  if(!index)
    return -1;
  
  switch(iface_type)
  {
    case UMTS_IFACE:       
      *index = 0;        
      break;

    case WWAN_GROUP:       
      *index = 1;        
      break;
      
    case ANY_DEFAULT_GROUP:
    case ANY_IFACE_GROUP:       
      *index = 2;        
      break;

    default :
      return -1;
  } 
  
  return 0;
}  


/*===========================================================================
FUNCTION DS_LOOPBACK_HDLR_ENABLE

DESCRIPTION

This function enables loopback mode handler for the specified iface_type. This means when an app
/ RMNET/RMSM requests an iface of iface_type they get an instance of loopback iface

PARAMETERS  

ps_iface_name_enum_type iface_type
    
DEPENDENCIES 
  None.

RETURN VALUE 
  None
 
SIDE EFFECTS 
  None.
  
===========================================================================*/
int ds_loopback_hdlr_enable(ps_iface_name_enum_type iface_type)
{
  int index = 0;
  
  if (ds_loopback_hdlr_get_index_from_iface_type(iface_type, &index) != 0)
  {
    return -1;
  }

  is_loopback_enabled[index] = TRUE;

  return 0;
}


/*===========================================================================
FUNCTION DS_LOOPBACK_HDLR_DISABLE

DESCRIPTION

This function disables loopback mode handler for the specified iface_type. 

PARAMETERS  

ds_loopback_iface_ptr
    
DEPENDENCIES 
  None.

RETURN VALUE 
  None
 
SIDE EFFECTS 
  None.
  
===========================================================================*/
int ds_loopback_hdlr_disable(ps_iface_name_enum_type iface_type)
{
  int index = 0;
  
  if (ds_loopback_hdlr_get_index_from_iface_type(iface_type, &index) != 0)
  {
    return -1;
  }

  is_loopback_enabled[index] = FALSE;

  /*TBD: cleanup for already enabled ifaces of this type may need to be done */
  return 0;
}


/*===========================================================================
FUNCTION DS_LOOPBACK_HDLR_CHECK_ENABLE_STATUS

DESCRIPTION

This function returns the status of loopback mode handler for the specified iface_type. 

PARAMETERS  

iface_type: iface type for which the status is needed
    
DEPENDENCIES 
  None.

RETURN VALUE 
  None
 
SIDE EFFECTS 
  None.
  
===========================================================================*/
int ds_loopback_hdlr_check_enable_status(ps_iface_name_enum_type iface_type)
{
  int index = 0;
  
  if (ds_loopback_hdlr_get_index_from_iface_type(iface_type, &index) != 0)
  {
    return FALSE;
  }

  return is_loopback_enabled[index];
}  



#endif /* FEATURE_DATA_TEST_LOOPBACK */