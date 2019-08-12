/*===========================================================================

                      DS707_PKT_MGR_FLOW
                      
DESCRIPTION
  This file handles all commands and notification related to a PS Flow. If the
  command is for the default flow, the module handles it. If it is for a 
  secondary flow, then it routes it to the secondary packet manager.  

EXTERNALIZED FUNCTIONS
  ds707_pkt_flow_register_callback_func  

INITIALIZING AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2005-2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/src/ds707_pkt_mgr_flow.c#1 $
  $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when      who    what, where, why
--------  ---    ----------------------------------------------------------
01/02/12  msh     Coan: Feature cleanup
03/15/11  mg     Global variable cleanup
10/01/10  ls     Set default flow for rev and fwd links
07/07/08  ms     Klocwork Fixes
10/13/06  az     Support for IPv6
07/18/06  az     IPV6 changes to populate the IpV6 watermark. Submitted for
                   Mike Tremaine
07/17/06  spn    Removed wrong Featurization.
05/30/06  ksu    EMPA: RLP init code review comments
02/27/06  sk     Added handoff support
11/07/05  sk     Added API bundling for QoS
08/30/05  ks     Added a paramter to PS_FLOW_SET_CB_F_PTR macro.
06/04/05  vas    Created module
===========================================================================*/

/*===========================================================================

                       INCLUDE FILES FOR THE MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#include "msg.h"
#include "ds707_pkt_mgr_ioctl.h"
#include "ps_flow.h"
#include "ps_iface.h"
#include "ds707_pkt_mgri.h"
#include "ds707_pkt_mgr_flow.h"

#ifdef FEATURE_HDR_QOS
#include "ds707_sec_pkt_mgr.h"
#include "ds707_sec_pkt_mgr_util.h"
#endif /* FEATURE_HDR_QOS */


/*===========================================================================
                            TYPEDEFS
===========================================================================*/

/*===========================================================================
                            VARIABLES
===========================================================================*/
/*---------------------------------------------------------------------------
 Pool of DS 707 FLows
---------------------------------------------------------------------------*/
static ds707_flow_type ds707_flow_pool[DS707_NUM_FWD_PS_FLOWS + DS707_NUM_REV_PS_FLOWS];

/*===========================================================================

                               FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================
FUNCTION     DS707_PKT_FLOW_REGISTER_CALLBACK_FUNC

DESCRIPTION

PARAMETERS
  flow_ptr - Pointer to the PS Flow to register callbacks for.

RETURN VALUE

DEPENDENCIES
  None

SIDE EFFECTS
  None.
===========================================================================*/
void ds707_pkt_flow_register_callback_func
(
  ps_flow_type  *flow_ptr
)
{

  /*-------------------------------------------------------------------------
    Set the function pointers to call for the default PS Flow
  -------------------------------------------------------------------------*/
  PS_FLOW_SET_CB_F_PTR( flow_ptr,
                        NULL,
                        NULL,
                        NULL,
                        NULL,
                        ds707_pkt_ps_flow_ioctl);
} /* ds707_pkt_flow_register_callback_func */


/*===========================================================================
FUNCTION      DS707_PKT_FLOW_INIT

DESCRIPTION   Called once at mobile power-up. 

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None
===========================================================================*/
void ds707_pkt_flow_init
(
  void
)
{
  ds707_tc_state_type *tc_state_ptr;
  uint8                i;
  uint8                curr_iface_index;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  tc_state_ptr = ds707_get_pri_tc_ptr();

  /*-------------------------------------------------------------------------
  Set up the first few elements in the Flow pool to be the Default Flows for 
  different ifaces present. 
  This flow is set up as bidirectional, it points to the ps_flow instance
  created when the iface is created and it points to the Primary TC. The
  ps_flow_ptr is set to point to the DS flow that it belongs to.
  -------------------------------------------------------------------------*/


  for ( curr_iface_index = 0; 
        curr_iface_index < DS707_MAX_IFACES;
        curr_iface_index++ )
  {        
    ds707_flow_pool[curr_iface_index].ds_flow_state = 
                                             DS707_DS_FLOW_STATE_ALLOCATED;
    ds707_flow_pool[curr_iface_index].dirn = DS707_DIRECTION_BIDIRECTIONAL;

    ds707_flow_pool[curr_iface_index].ps_iface_ptr =     
              ds707_pdncntx_get_iface_ptr_from_iface_tbl_index( curr_iface_index);
    ds707_flow_pool[curr_iface_index].ps_flow_ptr = 
                        PS_IFACE_GET_DEFAULT_FLOW( 
						  ds707_flow_pool[curr_iface_index].ps_iface_ptr);
    ds707_flow_pool[curr_iface_index].fwd_tc_ptr = tc_state_ptr;
    ds707_flow_pool[curr_iface_index].rev_tc_ptr = tc_state_ptr;
    ds707_flow_pool[curr_iface_index].fwd_resv_label = 0xFF;
    ds707_flow_pool[curr_iface_index].rev_resv_label = 0xFF;

    ds707_pkt_mgr_set_ps_to_ds_flow_map( 
                  ds707_flow_pool[curr_iface_index].ps_flow_ptr,
                  &ds707_flow_pool[curr_iface_index]);

    ds707_pkt_set_iface_ptr(
                  ds707_flow_pool[curr_iface_index].ps_flow_ptr, 
                  ds707_pdncntx_get_iface_ptr_from_iface_tbl_index( 
                                              curr_iface_index));

  }     

  /*-----------------------------------------------------------------------
    Set the default Flow pointers for both forward and reverse direction of
    the primary TC to point to the corresponding default flows
  -----------------------------------------------------------------------*/
  tc_state_ptr->fwd_rlp.flow_ptr[0] = &ds707_flow_pool[0];
  tc_state_ptr->rev_rlp.flow_ptr[0] = &ds707_flow_pool[0];

  /*-------------------------------------------------------------------------
    Initialize all the rest of the ds flows
  -------------------------------------------------------------------------*/

  for( i= curr_iface_index; 
       i<(DS707_NUM_FWD_PS_FLOWS + DS707_NUM_REV_PS_FLOWS); 
       i++)
  {
    ds707_flow_pool[i].ds_flow_state  = DS707_DS_FLOW_STATE_FREE;
    ds707_flow_pool[i].dirn = DS707_DIRECTION_NONE;
  }

} /* ds707_pkt_flow_init() */

/*===========================================================================
FUNCTION      DS707_GET_FLOW_POOL_PTR

DESCRIPTION   Given index, returns flow pool pointer. 

DEPENDENCIES  None.

RETURN VALUE  ds707_flow_type*.

SIDE EFFECTS  None
===========================================================================*/
ds707_flow_type* ds707_get_flow_pool_ptr(uint8 index)
{
  return (&ds707_flow_pool[index]);
}

