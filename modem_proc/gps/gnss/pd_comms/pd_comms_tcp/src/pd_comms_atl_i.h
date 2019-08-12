#ifndef PD_COMMS_ATL_I_H
#define PD_COMMS_ATL_I_H

/*=============================================================================

                        PD-COMMS ATL interface Header File

GENERAL DESCRIPTION
  This file contains contains the structure definitions and function prototypes for 
  the PD-COMMS ATL interface to be called by PD-COMM-TCP task.

 
EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
  

  Copyright (c) 2006 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

=============================================================================*/


/*=============================================================================

                           EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $$ 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
 10/11/07   bp     changes for incorporating ATL based DNS lookup functionality
=============================================================================*/


/*=============================================================================

                                INCLUDE FILES

=============================================================================*/
#include "gps_variation.h"
#include "customer.h"  /* Customer configuration file */
#include "comdef.h"    /* Definition for basic types and macros */

#ifdef FEATURE_CGPS_PDCOMM 
#include "pdsm_atl.h"
#include "pd_comms_api.h"
#include "pd_comms_tcp_data.h"
 

#define PD_COMMS_ATL_TYPE_NUM                  1
#define PD_COMMS_ATL_TYPE_IP_ARR_INDEX         0

typedef struct
{
  sint15                       x_app_id;  
  pdsm_atl_open_params_type z_open_params;  
} pd_comms_atl_open_params_type;

typedef	pd_comms_return_type (pd_comms_atl_i_open_f_type)
(
  pd_comms_handle_type           z_session_handle,
  pd_comms_atl_open_params_type* z_open_params 
);


typedef	pd_comms_return_type (pd_comms_atl_i_close_f_type)
(
  pd_comms_handle_type       z_session_handle, 
  boolean                    u_e911_close
);

/*===========================================================================
FUNCTION: pdsm_atl_dns_int_lookup_f_type

DESCRIPTION:
  This function type is used to retrieve the IP address for a given host name.
  This 'internal' version (as given by_int_lookup_f_type)is called by the PDCOMM layer, 
  and operates in conjunction with pdsm_atl_dns_lookup_f_type (defined in pdsm_atl.h) 
  The difference between the two types is a pointer in which
  an error value (if any) is stored and returned. A 'shim' dns lookup function  
  is defined of this internal type, which calls the 'actual' dns lookup function 
  registered through ATL and fills in the error value once the call completes.
============================================================================*/

typedef pd_comms_tcp_task_operation_status_type (pdsm_atl_dns_int_lookup_f_type) 
(
  pdsm_atl_session_handle_type    session_handle,
  const pdsm_atl_dns_params_type        *p_dns_lookup_params,
  pd_comms_connect_result_type    *p_conn_error
 );

/*===========================================================================
FUNCTION TYPE: pdsm_atl_i_dns_lookup_f_type

DESCRIPTION:
  The type of the dns lookup function call corresponds to the 'default' means by
  which to perform DNS lookups - which is by using the data services layer.
  A function of this default type is invoked when no ATL based DNS lookup 
  functions are registered.
===========================================================================*/
typedef pd_comms_tcp_task_operation_status_type (pd_comms_atl_i_dns_lookup_f_type) 
(
  uint32    q_handle_id,
  pd_comms_connect_result_type    *p_conn_error
 );

typedef struct
{
  /* Open Physical Path fn*/
  pdsm_atl_open_f_type *p_open_fnc;
  
  /* Close Physical Path fn */
  pdsm_atl_close_f_type *p_close_fnc;  

 /* dns lookup fn */
  pdsm_atl_dns_int_lookup_f_type *p_dns_lookup_fnc;
  
} pd_comms_atl_func_table_struct_type;

typedef struct
{
  /* Open Physical Path fn*/
  pd_comms_atl_i_open_f_type *p_open_fnc;
  
  /* Close Physical Path fn */
  pd_comms_atl_i_close_f_type *p_close_fnc;  

  
  /* dns lookup fn */
  pd_comms_atl_i_dns_lookup_f_type *p_dns_lookup_fnc;
} pd_comms_atl_dflt_func_table_struct_type;

/*===========================================================================

FUNCTION pd_comms_atl_i_dflt_proxy_reg

DESCRIPTION
   This function is called by pd_comms_tcp task to register default PPP 
   open/close functions with the ATL layer. If a PPP open/close function
   is not provided by the PPP open/close proxy, the default functions will
   be used.
DEPENDENCIES

RETURN VALUE
  TRUE - Successfully Registered default functions
  FALSE - Error

SIDE EFFECTS

===========================================================================*/
boolean pd_comms_atl_i_dflt_proxy_reg
( 
  pdsm_atl_type                                  q_transport_layer_type, 
  pd_comms_atl_i_open_f_type                     *p_open,
  pd_comms_atl_i_close_f_type                    *p_close,
  pd_comms_atl_i_dns_lookup_f_type               *p_dns_lookup
);


/*===========================================================================

FUNCTION pd_comms_atl_i_open

DESCRIPTION
  This function is called by PD-COMMS-TCP task to establish a 
  physical link.  

DEPENDENCIES

RETURN VALUE
  PDCOMM_RESULT_OK: If physical link is established.
  PDCOMM_RESULT_ERROR_UNKNOWN:    If physical link can not be established.
  PDCOMM_RESULT_WAIT:    If physical link is in the process of being stablished.

SIDE EFFECTS

===========================================================================*/
extern pd_comms_return_type pd_comms_atl_i_open
(
  pd_comms_handle_type       z_session_handle,
  pd_comms_atl_open_params_type* z_open_params 
);

/*===========================================================================

FUNCTION pd_comms_atl_i_close

DESCRIPTION
  This function is called by PD-COMMS-TCP task to tear down a 
  physical link.  

DEPENDENCIES

RETURN VALUE
  PDCOMM_RESULT_OK: If physical link is down.
  PDCOMM_RESULT_ERROR_UNKNOWN:    If physical link can not be torn down.
  PDCOMM_RESULT_WAIT:    If physical link is in the process of being torn down.

SIDE EFFECTS

===========================================================================*/
extern pd_comms_return_type pd_comms_atl_i_close
(
  pd_comms_handle_type       z_session_handle, 
  boolean                    u_e911_close
);


/*==================================================================================

FUNCTION pd_comms_atl_i_dns_lookup

DESCRIPTION
  This function is called by PD-COMMS-TCP task to resolve a name to an IP address.
  The _i_ signifies that the function is internal to the PDSM module.

DEPENDENCIES

RETURN VALUE
  PDCOMM_RESULT_OK: If physical link is established.
  PDCOMM_RESULT_ERROR_UNKNOWN:    If physical link can not be established.
  PDCOMM_RESULT_WAIT:    If physical link is in the process of being stablished.

SIDE EFFECTS


===========================================================================*/
extern pd_comms_tcp_task_operation_status_type pd_comms_atl_i_dns_lookup
(
  pd_comms_handle_type           z_session_handle,
  pdsm_atl_dns_params_type       *p_dns_lookup_params,
  pd_comms_connect_result_type   *p_conn_error
 );

/*===========================================================================

FUNCTION pd_comms_atl_i_ppp_proxy_exists

DESCRIPTION
  This function is called by PD-COMMS-TCP task to determine if an ATL based
  open function is registered

DEPENDENCIES

RETURN VALUE
  TRUE : if no ATL open function is registered
  FALSE : if one is registered
SIDE EFFECTS

===========================================================================*/
boolean pd_comms_atl_i_ppp_proxy_exists(void);

#endif /* FEATURE_CGPS_PDCOMM */
#endif /* PD_COMMS_ATL_I_H */
