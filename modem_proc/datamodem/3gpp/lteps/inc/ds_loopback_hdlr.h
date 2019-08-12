/*!
  @file
  ds_loopback_hdlr.h

  
*/

/*===========================================================================

  Copyright (c) 2009 - 2013 Qualcomm Technologies Incorporated. All Rights Reserved

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


when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/14/10   vk      Added support for dl data multiplication
08/12/09   sjakka      Initial version.

===========================================================================*/
#ifndef DS_LOOPBACK_HDLR_H
#define DS_LOOPBACK_HDLR_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include "ps_svc.h" 
#ifdef FEATURE_DATA_TEST_LOOPBACK
#define FEATURE_DSM_WM_CB

#include "ps_iface.h"
#include "ps_aclrules.h"
#include "ps_phys_link.h"
#include "dsm_queue.h"
#include "dstask_v.h"

/*===========================================================================

                           STRUCTURE DECLARATIONS

===========================================================================*/

typedef struct
{
  q_type             wm_queue;       /*!< Transmit Queue          */
  dsm_watermark_type wm_item;        /*!< Transmit watermark      */  
} ds_loopback_iface_cntx_wm_s;


typedef struct 
{  
  boolean               in_use;
  boolean               orig_from_sock; /* Not used right now */
  boolean               rm_flow_enabled;  
  ps_iface_type         ps_iface;     
  ps_phys_link_type     phys_link;    
  acl_type              acl;       
  ps_sig_enum_type      rx_sig;
  void                  *client_data_ptr;  
  void                  *rm_flow_enable_buf_ptr;
  void                  *rm_flow_disable_buf_ptr;
  ds_loopback_iface_cntx_wm_s    rx_wm;
} ds_loopback_iface_s;


typedef struct _ds_loopback_hdlr_ip_addr_type {  
  boolean  is_set;
  uint32   ip_addr;
} ds_loopback_hdlr_ip_addr_type;

typedef struct {
  uint8 dl_mulf;
  uint8 dl_dupf;
} ds_loopback_hdlr_cfg_s;

typedef struct{
   uint32 ul_wm_lo;
   uint32 ul_wm_hi;
   uint32 ul_wm_dne;
}ds_loopback_hdlr_wm_levels_s;

typedef struct
{
  int     id;
  void *  info_ptr;
  void *  user_data;
}ds_loopback_hdlr_cmd_info_type;

/*===========================================================================

                           MACRO DEFINITIONS

===========================================================================*/

#define DS_LOOPBACK_HDLR_UL_WM_LO   40000
#define DS_LOOPBACK_HDLR_UL_WM_HI   65000
#define DS_LOOPBACK_HDLR_UL_WM_DNE 190000

#define DS_LOOPBACK_HDLR_UL_WM_HI_1  120000
#define DS_LOOPBACK_HDLR_UL_WM_DNE_1 240000

#define DS_LOOPBACK_HDLR_NUM_IFACES 6
#define DS_LOOPBACK_HDLR_NUM_V4_IFACES 3
#define DS_LOOPBACK_HDLR_NUM_V6_IFACES 3
#define DS_LOOPBACK_HDLR_IFACE_UP_CMD 1
#define DS_LOOPBACK_HDLR_IFACE_DOWN_CMD 2
#define DS_LOOPBACK_HDLR_PHYS_LINK_UP_CMD 3
#define DS_LOOPBACK_HDLR_PHYS_LINK_DOWN_CMD 4
#define DS_LOOPBACK_HDLR_PHYS_LINK_FLOW_ENABLE_CMD 5
#define DS_LOOPBACK_HDLR_PHYS_LINK_FLOW_DISABLE_CMD 6
#define DS_LOOPBACK_HDLR_TMP_PKT_SIZE 2048
#define DS_LOOPBACK_HDLR_DATA_PTR_SIZE 2048

#define DS_LOOPBACK_HDLR_IFACE_BEING_USED()    \
        (((ds_loopback_iface_s*)(((ps_iface_type *) this_if_ptr)->client_data_ptr))->in_use)
  
#define DS_LOOPBACK_HDLR_GET_LOOPBACK_IFACE_FROM_PS_IFACE( ps_iface )            \
      ((ds_loopback_iface_s*)(((ps_iface_s *) ps_iface)->client_data_ptr))

#define  DS_LOOPBACK_HDLR_IFACE_LOOPBACK_ENABLED()                                         \
  USE_POL_INFO(                                                           \
    IFACE_TYPE_IS_ID() ?                                                  \
      (ds_loopback_hdlr_check_enable_status(PS_IFACE_GET_NAME_FROM_ID(policy_info_ptr->iface.info.id))) \
      : ((IFACE_TYPE_IS_NAME() || (IFACE_TYPE_IS_GROUP(ANY_IFACE_GROUP)))?                                            \
        (ds_loopback_hdlr_check_enable_status((ps_iface_name_enum_type)policy_info_ptr->iface.info.name))          \
        :FALSE ))


/*===========================================================================

                         API DECLARATIONS

===========================================================================*/


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
void ds_loopback_hdlr_init(void);  

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

void  ds_loopback_hdlr_set_nonempty_callback(int iface_inst);

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

void  ds_loopback_hdlr_reset_nonempty_callback(int iface_inst);

/*===========================================================================
  FUNCTION DS_LOOPBACK_HDLR_RESET_NONEMPTY_CALLBACK


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



boolean  ds_loopback_hdlr_is_instance_enabled(int instance);


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
void ds_loopback_process_ps_cmd (const ds_cmd_type *cmd_ptr); 


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
int ds_loopback_hdlr_check_enable_status(ps_iface_name_enum_type iface_type);


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
int ds_loopback_hdlr_enable(ps_iface_name_enum_type iface_type);


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
int ds_loopback_hdlr_disable(ps_iface_name_enum_type iface_type);       

void ds_loopback_hdlr_enable_all(void);

void ds_loopback_hdlr_set_cfg(const ds_loopback_hdlr_cfg_s * cfg_p);


/*===========================================================================
FUNCTION DS_LOOPBACK_HDLR_GET_DYN_UNSIGNED_CHAR

DESCRIPTION
  This function returns a unsigned char pointer pointing to 2048 blocks of memory

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  unsingned char*

SIDE EFFECTS
  None.

===========================================================================*/


unsigned char *ds_loopback_hdlr_get_dyn_unsigned_char(void);


/*===========================================================================
FUNCTION DS_LOOPBACK_HDLR_GET_DYN_BYTE

DESCRIPTION
  This function returns a pointer to a byte pointing to 2048 blocks of memory

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  byte ptr

SIDE EFFECTS
  None.

===========================================================================*/

byte *ds_loopback_hdlr_get_dyn_byte(void);

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
);


#endif /* FEATURE_DATA_TEST_LOOPBACK */
#endif
