#ifndef PS_IFACE_LOGICAL_FLOWI_H
#define PS_IFACE_LOGICAL_FLOWI_H
/*===========================================================================
  @file ps_iface_logical_flowi.h

  This module defines the internal functions for logical flow framework
  TODO Enhance documentation

  Copyright (c) 2010 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/netiface/src/ps_iface_logical_flowi.h#1 $
  $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

===========================================================================*/

/*===========================================================================

                                INCLUDE FILES

===========================================================================*/
#include "ps_iface_defs.h"
#include "ps_iface.h"
#include "ps_flow.h"
#include "ps_iface_flow.h"

/*===========================================================================
FUNCTION ps_iface_logical_flowi_handle_nw_init_qos_flow

DESCRIPTION
  This function is used to handle network initiated QoS. The function reads
  the given associated flow ptr and creates a new logical iface for the
  given iface that is associated with the associated flow.
 
PARAMETERS
  iface_ptr         : corresponding iface on which logical flow corresponding
                      to the physical flow is to be created.
  assoc_flow_ptr    : associated flow ptr to create the logical flow based on
  ps_errno          : specific error code in case operation fails

RETURN VALUE
   0 : on success
  -1 : on failure

DEPENDENCIES 
  None
 
SIDE EFFECTS
  None
===========================================================================*/
int32 ps_iface_logical_flowi_handle_nw_init_qos_flow
(
 ps_iface_type  * ps_iface_ptr,
 ps_flow_type   * assoc_flow_ptr,
 int16          * ps_errno
);

/*===========================================================================
FUNCTION ps_iface_logical_flowi_handle_nw_init_qos

DESCRIPTION
  This function is used to handle network initiated QoS. The function reads
  the flows in the given associated iface. For each flow create a logical flow
  associating with it on the logical iface.
 
PARAMETERS
  iface_ptr         : corresponding iface on which logical flow corresponding
                      to the physical flow is to be created.
  ps_errno          : specific error code in case operation fails

RETURN VALUE
   0 : on success
  -1 : on failure

DEPENDENCIES 
  None
 
SIDE EFFECTS
  None
===========================================================================*/
int32 ps_iface_logical_flowi_handle_nw_init_qos
(
  ps_iface_type  * ps_iface_ptr
);

/*===========================================================================
FUNCTION ps_iface_logical_flowi_handle_nw_init_qos_selective

DESCRIPTION
  This function is used to handle network initiated QoS. The function reads
  the flows in the given associated iface. For each flow, checks if the flow
  is already associated, if not it creates a logical flow associated with it
  on the logical iface.
 
PARAMETERS
  iface_ptr         : corresponding iface on which logical flow corresponding
                      to the physical flow is to be created.
  ps_errno          : specific error code in case operation fails

RETURN VALUE
   0 : on success
  -1 : on failure

DEPENDENCIES 
  None
 
SIDE EFFECTS
  None
===========================================================================*/
int32 ps_iface_logical_flowi_handle_nw_init_qos_selective
(
  ps_iface_type  * ps_iface_ptr
);

/*===========================================================================
FUNCTION ps_iface_logical_flowi_handle_nw_init_qos_flow_selective

DESCRIPTION
  This function is used to handle network initiated QoS. The function reads
  the given associated flow ptr and creates a new logical iface for the
  given iface that is associated with the associated flow. The creation is
  performed only if the flow is not already associated with the iface.
 
PARAMETERS
  iface_ptr         : corresponding iface on which logical flow corresponding
                      to the physical flow is to be created.
  assoc_flow_ptr    : associated flow ptr to create the logical flow based on
  
RETURN VALUE
   0 : on success
  -1 : on failure

DEPENDENCIES 
  None
 
SIDE EFFECTS
  None
===========================================================================*/

int32 ps_iface_logical_flowi_handle_nw_init_qos_flow_selective
(
  ps_iface_type * ps_iface_ptr,
  ps_flow_type  * assoc_flow_ptr
);

/*===========================================================================
FUNCTION ps_iface_logical_flowi_process_fltr_aux_info_updated_ev

DESCRIPTION
  This function is used to handle network initiated QoS. The function reads
  the Tx filter spec array of the physical flow that had been updated and
  updates the logical flow accordingly.
 
PARAMETERS
  iface_ptr         : corresponding iface on which logical flow corresponding
                      to the physical flow is to be created.
  assoc_flow_ptr    : associated flow ptr to create the logical flow based on
  
RETURN VALUE
   0 : on success
  -1 : on failure

DEPENDENCIES 
  None
 
SIDE EFFECTS
  None
===========================================================================*/

int32 ps_iface_logical_flowi_process_fltr_aux_info_updated_ev
(
  ps_iface_type * ps_iface_ptr,
  ps_flow_type  * assoc_flow_ptr,
  ps_flow_type  * ps_flow_ptr,
  int16         * ps_errno
);

/*===========================================================================
FUNCTION ps_flowi_get_qos_spec_from_flow

DESCRIPTION
  This function constructs qos_spec_type given flow_ptr 
 
PARAMETERS 
  flow_ptr	    : the flow_ptr containing the qos_info_ptr
  is_modify	    : indicates if the qos_info_ptr or qos_modify_info_ptr
 					  is to be used for constructing the qos_spec
  qos_spec_ptr_ptr  : out param used to return created qos_spec_type
  ps_errno	    : used to return the error value in case of error
  
RETURN VALUE
   0 : on success
  -1 : on failure

DEPENDENCIES 
  None
 
SIDE EFFECTS
  None
===========================================================================*/
int32 ps_flowi_get_qos_spec_from_flow
(
  ps_flow_type    * flow_ptr,
  boolean           is_modify,
  qos_spec_type  ** qos_spec_ptr_ptr,  
  int16           * ps_errno
);

/*===========================================================================
FUNCTION ps_flowi_get_modify_qos_spec_from_flow

DESCRIPTION
  This function constructs qos_spec_type from the qos_modify_info_ptr of the
  given flow_ptr 
 
PARAMETERS 
  flow_ptr	    : the flow_ptr containing the qos_modify_info_ptr
 					  is to be used for constructing the qos_spec
  qos_spec_ptr_ptr  : out param used to return created qos_spec_type
  ps_errno	    : used to return the error value in case of error
  
RETURN VALUE
   0 : on success
  -1 : on failure

DEPENDENCIES 
  None
 
SIDE EFFECTS
  None
===========================================================================*/
int32 ps_flowi_get_modify_qos_spec_from_flow
(
  ps_flow_type   * logical_flow_ptr,
  qos_spec_type ** qos_spec_ptr_ptr,  
  int16          * ps_errno
);

/*===========================================================================
FUNCTION ps_iface_logical_flowi_free_qos_spec

DESCRIPTION
  This function frees the qos_spec_type
 
PARAMETERS 
  qos_spec_ptr  : qos_spec to free from system heap
  
RETURN VALUE
  None
 
DEPENDENCIES 
  None
 
SIDE EFFECTS
  None
===========================================================================*/
void ps_iface_logical_flowi_free_qos_spec
(
  qos_spec_type ** qos_spec_ptr
);
#endif /* PS_IFACE_LOGICAL_FLOWI_H */
