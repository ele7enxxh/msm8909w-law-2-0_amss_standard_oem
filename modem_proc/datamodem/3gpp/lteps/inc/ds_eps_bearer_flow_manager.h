
/*!
  @file
  ds_eps_bearer_flow_manager.h

  @brief
  Manages Flow specific information related to bearer. provides interface to 
  the bearer and applications to extract and configure EPS flow information.

  @detail
  OPTIONAL detailed description of this C header file.
  - DELETE this section if unused.

*/

/*===========================================================================

  Copyright (c) 2009-2011 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/lteps/inc/ds_eps_bearer_flow_manager.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/04/11   ttv     Added changes needed for SESSION_PARAMS IOCTL and NSAPI api.
02/04/10   vd      Changed function naming format.
11/07/09   vd      Initial version.
===========================================================================*/

#ifndef DSEPSBEARERFLOWMANAGER_H
#define DSEPSBEARERFLOWMANAGER_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "datamodem_variation.h"
#ifdef FEATURE_DATA_LTE

#include "ds_3gpp_flow_manager.h"
#include "ds_3gpp_bearer_flow_manager.h"
/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

#define DS_EPS_1KB_BITRATE_MINIMUM_VALUE      01
#define DS_EPS_1KB_BITRATE_START_RANGE        01
#define DS_EPS_1KB_BITRATE_END_RANGE          63

#define DS_EPS_8KB_BITRATE_MINIMUM_VALUE      64
#define DS_EPS_8KB_BITRATE_START_RANGE        64
#define DS_EPS_8KB_BITRATE_END_RANGE          127
#define DS_EPS_8KB_BITRATE_MULTIPLE           8 

#define DS_EPS_64KB_BITRATE_MINIMUM_VALUE     576
#define DS_EPS_64KB_BITRATE_START_RANGE       128
#define DS_EPS_64KB_BITRATE_END_RANGE         254
#define DS_EPS_64KB_BITRATE_MULTIPLE          64 

#define DS_EPS_100KB_BITRATE_MINIMUM_VALUE    8700
#define DS_EPS_100KB_BITRATE_BASE_RATE        8600
#define DS_EPS_100KB_BITRATE_START_RANGE      01
#define DS_EPS_100KB_BITRATE_END_RANGE        74
#define DS_EPS_100KB_BITRATE_MULTIPLE         100 

#define DS_EPS_1MB_BITRATE_MINIMUM_VALUE      17000
#define DS_EPS_1MB_BITRATE_START_RANGE        75
#define DS_EPS_1MB_BITRATE_END_RANGE          186
#define DS_EPS_1MB_BITRATE_MULTIPLE           1000 

#define DS_EPS_2MB_BITRATE_MINIMUM_VALUE      130000
#define DS_EPS_2MB_BITRATE_START_RANGE        187
#define DS_EPS_2MB_BITRATE_END_RANGE          250
#define DS_EPS_2MB_BITRATE_MULTIPLE           2000

#define DS_EPS_4MB_BITRATE_MINIMUM_VALUE      260000
#define DS_EPS_4MB_BITRATE_START_RANGE        01
#define DS_EPS_4MB_BITRATE_END_RANGE          61
#define DS_EPS_4MB_BITRATE_MULTIPLE           4000

#define DS_EPS_10MB_BITRATE_MINIMUM_VALUE      510000
#define DS_EPS_10MB_BITRATE_START_RANGE        62
#define DS_EPS_10MB_BITRATE_END_RANGE          161
#define DS_EPS_10MB_BITRATE_MULTIPLE           10000

#define DS_EPS_100MB_BITRATE_MINIMUM_VALUE      1600000
#define DS_EPS_100MB_BITRATE_START_RANGE        162
#define DS_EPS_100MB_BITRATE_END_RANGE          246
#define DS_EPS_100MB_BITRATE_MULTIPLE           100000

#define DS_EPS_EXT_BITRATE_MAXIMUM_VALUE      256000
#define DS_EPS_EXT2_BITRATE_MAXIMUM_VALUE     10000000
#define DS_EPS_APP_BITRATE_MAXIMUM_VALUE      DS_EPS_EXT2_BITRATE_MAXIMUM_VALUE
#define DS_EPS_INVALID_EXT_BITRATE            0xFFFFFF
#define DS_EPS_MAX_1_OCTET_BITRATE            8640

#define DS_EPS_MAX_1_OCTET_BIT_VALUE          254
#define DS_EPS_0KB_BITRATE_VALUE              255
#define DS_EPS_EXT_MAX_BIT_VALUE              250
#define DS_EPS_EXT2_MAX_BIT_VALUE             250
#define DS_EPS_EXT2_MIN_BIT_VALUE             0

#define DS_EPS_QOS_QCI_MIN_LEN_FOR_DATA       1
#define DS_EPS_QOS_QCI_LEN_FOR_DATA           5
#define DS_EPS_QOS_QCI_LEN_FOR_EXT_DATA       9 
#define DS_EPS_QOS_QCI_LEN_FOR_EXT2_DATA      13 
#define EPS_BEARER_FLOW_MGR_VALID_COOKIE       0xebeafebe
/*-----------------------------------------------------------------------------
  This structure contains EPS specific info for the Bearer Flow manager.
-----------------------------------------------------------------------------*/
typedef struct
{ 
  uint32 			   cookie;
  sdf_qos_T      sdf_qos;  /* QOS in EPS Format */
  qos_T          neg_qos;  /* QOS in 3G Format */
  int 				   index;
} ds_eps_bearer_flow_manager_dyn_s;

typedef struct
{
  ds_eps_bearer_flow_manager_dyn_s *eps_bearer_flow_manager_dyn_p;
}ds_eps_bearer_flow_manager_s;

extern ds_eps_bearer_flow_manager_s ds_eps_bflow_manager_tbl[DS_3GPP_MAX_BEARER_CONTEXT];
extern uint32 ds_eps_kbps_to_bps_multiplier_32bit;
extern uint64 ds_eps_kbps_to_bps_multiplier_64bit;
/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/
/*===========================================================================
FUNCTION DS_EPS_BEARER_FLOW_MANAGER_INIT

DESCRIPTION
  Register EPS specific functions

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  
SIDE EFFECTS
  None.

===========================================================================*/
void ds_eps_bearer_flow_manager_init( void );

/*===========================================================================
FUNCTION DS_EPS_BEARER_FLOW_MANAGER_CONVERT_NW_BIT_RATE

DESCRIPTION
  Converts the NW passed bit rate to uint64.

PARAMETERS
  input         -  network octet to be converted

DEPENDENCIES
  None.

RETURN VALUE
uint64          - return converted bit rate
  
SIDE EFFECTS
  None.

===========================================================================*/
uint64 ds_eps_bearer_flow_manager_convert_nw_bit_rate
(
  byte input
);

/*===========================================================================
FUNCTION DS_EPS_BEARER_FLOW_MANAGER_CONVERT_EXT_NW_BIT_RATE

DESCRIPTION
  Converts the NW passed extended bit rate to uint64.

PARAMETERS
  input         -  network octet to be converted

DEPENDENCIES
  None.

RETURN VALUE
uint64          - return converted bit rate
  
SIDE EFFECTS
  None.

===========================================================================*/
uint64 ds_eps_bearer_flow_manager_convert_ext_nw_bit_rate
(
  byte input
);

/*===========================================================================
FUNCTION DS_EPS_BEARER_FLOW_MANAGER_CONVERT_EXT2_NW_BIT_RATE

DESCRIPTION
  Converts the NW passed extended bit rate to uint64.

PARAMETERS
  input         -  network octet to be converted

DEPENDENCIES
  None.

RETURN VALUE
uint64          - return converted bit rate
  
SIDE EFFECTS
  None.

===========================================================================*/
uint64 ds_eps_bearer_flow_manager_convert_ext2_nw_bit_rate
(
  byte input
);

/*===========================================================================
FUNCTION DS_EPS_BEARER_FLOW_MANAGER_CONVERT_NW_FLOW_TO_APP_FLOW

DESCRIPTION
  Converts the EPS passed Flow spec to App flow spec.

PARAMETERS
  nw_flow         -  EPS flow to be converted
  app_flow        -  App flow to store the converted info.

DEPENDENCIES
  None.

RETURN VALUE
boolean          - returns if conversion is successfull
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_eps_bearer_flow_manager_convert_nw_flow_to_app_flow
(
  sdf_qos_T              *nw_flow,
  qos_spec_type           *app_flow,
  ds_3gpp_internal_cause_code_enum_type *cause_code_p
);

/*===========================================================================
FUNCTION DS_EPS_BEARER_FLOW_MANAGER_CONVERT_APP_FLOW_TO_NW_FLOW

DESCRIPTION
  Converts the EPS passed Flow spec to App flow spec.

PARAMETERS
  nw_flow         -  EPS flow to be converted
  app_flow        -  App flow to store the converted info.

DEPENDENCIES
  None.

RETURN VALUE
boolean          - returns if conversion is successfull
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_eps_bearer_flow_manager_convert_app_flow_to_nw_flow
(
  sdf_qos_T              *nw_flow,
  qos_spec_type           *app_flow
);

/*===========================================================================
FUNCTION DS_EPS_BEARER_FLOW_MANAGER_CONVERT_APP_DATA_RATE_TO_NW
DESCRIPTION
  Converts the APP passed data rate to EPS NW data rate.

PARAMETERS
app_qos         -  Application qos to be converted
nw_qos          -  EPS network qos
DEPENDENCIES
  None.

RETURN VALUE
boolean         - return DS3G_SUCCESS or DS3G_FAILURE
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_eps_bearer_flow_manager_convert_app_data_rate_to_nw
(
   qos_spec_type   *app_qos,
   void            *nw_qos
);

/*===========================================================================
FUNCTION DS_EPS_BEARER_FLOW_MANAGER_CONVERT_NW_DATA_RATE_TO_APP
DESCRIPTION
  Converts the EPS NW passed data rate to App data rate.

PARAMETERS
nw_qos          -  EPS network qos to be converted
app_qos         -  Application qos structure
DEPENDENCIES
  None.

RETURN VALUE
boolean         - return DS3G_SUCCESS or DS3G_FAILURE
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_eps_bearer_flow_manager_convert_nw_data_rate_to_app
(
  ds_bearer_flow_manager_s *bflow_manager_p,
  void                     *nw_qos,
  qos_spec_type            *app_qos,
  ds_3gpp_internal_cause_code_enum_type *cause_code_p
);

/*===========================================================================
FUNCTION DS_EPS_BEARER_FLOW_MANAGER_CONVERT_APP_BIT_RATE_TO_NW

DESCRIPTION
  Converts the NW passed bit rate to uint32.

PARAMETERS
  input         -  network octet to be converted

DEPENDENCIES
  None.

RETURN VALUE
uint32          - return converted bit rate
  
SIDE EFFECTS
  None.

===========================================================================*/
void ds_eps_bearer_flow_manager_convert_app_bit_rate_to_nw
(
  uint32 app_rate,
  byte   *nw_rate,
  byte   *nw_rate_ex
);

/*===========================================================================
FUNCTION DS_EPS_BEARER_FLOW_MANAGER_CONVERT_APP_BIT_RATE_TO_NW_EXT

DESCRIPTION
  Converts the NW passed bit rate to uint32.

PARAMETERS
  input         -  network octet to be converted

DEPENDENCIES
  None.

RETURN VALUE
uint32          - return converted bit rate
  
SIDE EFFECTS
  None.

===========================================================================*/
void ds_eps_bearer_flow_manager_convert_app_bit_rate_to_nw_ext
(
  uint64 app_rate,
  byte   *nw_rate,
  byte   *nw_rate_ex,
  byte   *nw_rate_ex2
);

/*===========================================================================
FUNCTION DSEPSBEARERFLOWMANAGER_CONVERTAPPBITRATETONW

DESCRIPTION
  Converts the NW passed bit rate to uint32.

PARAMETERS
  input         -  network octet to be converted

DEPENDENCIES
  None.

RETURN VALUE
uint32          - return converted bit rate
  
SIDE EFFECTS
  None.

===========================================================================*/
void dsEpsBearerFlowmanager_convertAppBitRateToNw
(
  uint32 app_rate,
  byte   *nw_rate,
  byte   *nw_rate_ex
);

/*===========================================================================
FUNCTION DS_EPS_BEARER_FLOW_MANAGER_FLOW_VALIDATION

DESCRIPTION
  Verify the app did not pass invalid flow parameters.

PARAMETERS
 input_qos           - Pointer to App passed Qos.

DEPENDENCIES
  None.

RETURN VALUE
 boolean              - Indicates true or false.
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_eps_bearer_flow_manager_flow_validation
( 
  qos_spec_type      *input_qos
);

/*===========================================================================
FUNCTION DS_EPS_BEARER_FLOW_GET_LTE_NETW_ALLOC_QOS

DESCRIPTION
  Convert app QoS and stores the results in lte_netw_alloc_qos.

PARAMETERS
  bearer_context_p   - bearer context pointer.
  lte_netw_alloc_qos - pointer to hold the converted values

DEPENDENCIES
  None.

RETURN VALUE
  None
  
SIDE EFFECTS
  None.

===========================================================================*/
void
ds_eps_bearer_flow_manager_get_lte_netw_alloc_qos
(
  ds_bearer_context_s          *bearer_context_p,
  ds_3gpp_lte_qos_params_type  *lte_netw_alloc_qos
);

/*===========================================================================
FUNCTION DS_EPS_BEARER_FLOW_MANAGER_UPDATE_FLOW_ON_RESUME

DESCRIPTION
  Updates flow from W->L handoff with L params

PARAMETERS
  bflow_manager_p   - bearer flow manager ptr.

DEPENDENCIES
  None.

RETURN VALUE
  boolean - True if success, false otherwise.
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_eps_bearer_flow_manager_update_flow_on_resume
(
  ds_bearer_flow_manager_s *bflow_manager_p
);

/*===========================================================================
FUNCTION ds_eps_bearer_flow_manager_allocate_eps_bearer_flow_manager

DESCRIPTION
  This function allocates dynamic memory for the index in the eps bearer flow manager table

PARAMETERS
int alloc_index
    
DEPENDENCIES
  None.

RETURN VALUE
  TRUE 
  FALSE

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_eps_bearer_flow_manager_alloc_eps_bearer_flow_manager
(
   int alloc_index
);

/*===========================================================================
FUNCTION ds_eps_bearer_flow_manager_validate_eps_bearer_flow_manager

DESCRIPTION
  This function validates dynamic memory for the index in the eps bearer
  flow manager table

PARAMETERS
int alloc_index
    
DEPENDENCIES
  None.

RETURN VALUE
  TRUE 
  FALSE

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_eps_bearer_flow_manager_validate_eps_bearer_flow_manager
(
   ds_eps_bearer_flow_manager_s *eps_bearer_flow_manager_p
);

/*===========================================================================
FUNCTION ds_eps_bearer_flow_manager_free_eps_bearer_flow_manager

DESCRIPTION
  This function frees dynamic memory for the index in the
  eps bearer flow manager table

PARAMETERS
int index
    
DEPENDENCIES
  None.

RETURN VALUE
  TRUE 
  FALSE

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_eps_bearer_flow_manager_free_eps_bearer_flow_manager
(
   int index
);
#endif /* FEATURE_DATA_LTE */
#endif /* DSEPSBEARERFLOWMANAGER_H */

