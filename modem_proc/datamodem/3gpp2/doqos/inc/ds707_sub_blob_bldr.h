
#ifndef DS707_SUB_BLOB_BLDR_H
#define DS707_SUB_BLOB_BLDR_H
/*===========================================================================

                        D S 7 0 7 _ S U B_ B L O B _ B L D R . C
GENERAL DESCRIPTION
         
  This file is builds the sub blob


EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2009 by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                           EDIT HISTORY FOR MODULE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_sub_blob_bldr.h_v   1.45   25 Feb 2003 14:25:02   skhatri  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/doqos/inc/ds707_sub_blob_bldr.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
02/19/09    sk     eHRPD: UE initiated QoS
05/01/07    sk     QoS Modify support
03/05/07    sk     Added support for ANSupported Profile change
03/30/06    spn    Fixed Featurization for FEATURE_HDR_QOS. 
07/13/05    sk     Modified input parameter of sub blob builder
04/04/05    sk     First version of file.
===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#if defined(FEATURE_DATA_IS707) && defined(FEATURE_HDR_QOS)
#include "ps_qos_defs.h"
#include "ps_flow.h"
#include "ds707_qosdefs.h"
#include "hdrmrlpcif.h"
#include "ds707_pkt_mgri.h"
/*===========================================================================
                            TYPEDEFS
===========================================================================*/

/*===========================================================================
                            VARIABLES
===========================================================================*/



/*===========================================================================
                       EXTERNAL FUNCTION DECLARATION
===========================================================================*/

/*==========================================================================
FUNCTION      DS707_BUILD_QOS_SUB_BLOB

DESCRIPTION   This is external function to build a QOS SUB BLOB. This module
              validates the input data before writing in buffer. 
  
DEPENDENCIES  None
  
RETURN VALUE  Error code

SIDE EFFECTS  The sub blob is build and the size of the sub blob is returned
===========================================================================*/
errno_enum_type ds707_build_qos_sub_blob
(
  ps_flow_type   *ps_flow_ptr,
  uint8           max_sub_blob_size,
  uint8          *sub_blob_ptr,
  uint8           *sub_blob_size,
  uint8           qos_attribute_set_id,
  uint8           *num_set_return,
  ds707_direction_enum_type direction,
  boolean         is_modify, /* TRUE = NEW, FALSE = MODIFY */
  boolean         is_ue_resync  /*Is triggered by UE resync*/
);

/*===========================================================================
FUNCTION      DS707_PARSE_QOS_RESPONSE_SUB_BLOB

DESCRIPTION   This function unpacks the sub blob pointer and gets the granted
              set id.

DEPENDENCIES  None
  
RETURN VALUE  Error code

SIDE EFFECTS  granted_set_id is stored.
===========================================================================*/
errno_enum_type ds707_parse_qos_response_sub_blob( 
  uint8   *sub_blob_ptr, 
  uint8   *granted_set_id, 
  uint16   len
);

/*===========================================================================
FUNCTION      DS707_SUB_BLOB_SET_GRANTED

DESCRIPTION   

DEPENDENCIES  None
  
RETURN VALUE  Error code

SIDE EFFECTS  
===========================================================================*/
boolean ds707_sub_blob_set_granted( 
  ps_flow_type   *ps_flow_ptr, 
  uint8           starting_set_id,
  uint8           granted_set_id, 
  ds707_direction_enum_type direction
);

/*===========================================================================
FUNCTION      DS707_SUB_BLOB_BLDR_FLOW_VALIDATION

DESCRIPTION   

DEPENDENCIES  None
  
RETURN VALUE  

SIDE EFFECTS  
===========================================================================*/
boolean ds707_sub_blob_bldr_flow_validation(
  const ip_flow_type      * ip_flow_ptr
);

/*===========================================================================
FUNCTION      DS707_SUB_BLOB_BLDR_MAX_ALLOWED_PROFILES

DESCRIPTION   

DEPENDENCIES  None
  
RETURN VALUE  

SIDE EFFECTS  
===========================================================================*/
uint8 ds707_sub_blob_bldr_max_allowed_profiles(void);

/*===========================================================================
FUNCTION      DS707_SUB_BLOB_BLDR_MAX_ALLOWED_FLOW_PRIORITY

DESCRIPTION   

DEPENDENCIES  None
  
RETURN VALUE  

SIDE EFFECTS  
===========================================================================*/
uint8 ds707_sub_blob_bldr_max_allowed_flow_priority(void);

/*===========================================================================
FUNCTION      DS707_SUB_BLOB_POWERUP_INIT

DESCRIPTION   

DEPENDENCIES  None
  
RETURN VALUE  

SIDE EFFECTS  
===========================================================================*/
void ds707_sub_blob_powerup_init(void);

/*===========================================================================
FUNCTION      DS707_SUB_BLOB_GET_AN_SUP_QOS_PROF

DESCRIPTION   

DEPENDENCIES  None
  
RETURN VALUE  

SIDE EFFECTS  
===========================================================================*/
void ds707_sub_blob_get_an_sup_qos_prof(hdrmrlpcif_an_supported_qos_profile_type *an_sup_profile_value);

/*===========================================================================
FUNCTION      DS707_SUB_BLOB_GET_AN_SUP_QOS_PROF_CNAHGE

DESCRIPTION   

DEPENDENCIES  None
  
RETURN VALUE  

SIDE EFFECTS  
===========================================================================*/
boolean ds707_sub_blob_an_sup_qos_prof_change(void);

#ifdef FEATURE_EHRPD
/*===========================================================================
FUNCTION      DS707_SUB_BLOB_BLDR_SAVE_AUTHENTICATED_PROFILES

DESCRIPTION   This is a utility saves the authenticated profiles

DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_sub_blob_bldr_save_authenticated_profiles(
  ds707_flow_type * ds_flow_ptr, 
  ds707_direction_enum_type direction,
  boolean is_modify
);

#ifdef FEATURE_NIQ_EHRPD
/*===========================================================================
FUNCTION      DS707_SUB_BLOB_BLDR_SAVE_CONF_PROFILES_FOR_NW_FLOW

DESCRIPTION   This is a utility stores the confirmed profiles from PS flow
              to DS flow for the NW init flow. It gets the profiles from
              the QOS spec requested from the network.

INPUT         ds_flow_ptr : DS flow 
              direction   : Direction

DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  Confirmed profiles are stored on DS flow structure
===========================================================================*/
void ds707_sub_blob_bldr_save_conf_profiles_for_nw_flow(
  ds707_flow_type              * ds_flow_ptr, 
  ds707_direction_enum_type      direction,
  boolean                        is_modify
);
#endif /*FEATURE_NIQ_EHRPD*/
#endif /* FEATURE_EHRPD */
#endif /* FEATURE_DATA_IS707 && FEATURE_HDR_QOS*/
#endif /* DS707_SUB_BLOB_BLDR_H    */
