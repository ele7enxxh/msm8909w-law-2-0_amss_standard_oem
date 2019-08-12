#ifndef PS_PPP_NCP_H
#define PS_PPP_NCP_H
/*===========================================================================
                         P S _ P P P _ N C P . H

DESCRIPTION
  Header file for the PPP Network Control Protocol (NCP) Interface
  Contains support for the following network control protocols
  - Vendor-Specific Network Configuration Protocol (VSNCP)

  Copyright (c) 2009-2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/
/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/api/ps_ppp_ncp.h#1 $
  $Author: mplcsds1 $ 
  $DateTime: 2016/12/13 08:00:03 $

===========================================================================*/
/*===========================================================================

                                INCLUDE FILES

===========================================================================*/
#include "comdef.h"
#include "ps_ppp_defs.h"
#include "ps_ppp_vsncp_3gpp2.h"

/*===========================================================================

                              DATA DECLARATIONS

===========================================================================*/
typedef void *ppp_ncp_handle_type;

/*---------------------------------------------------------------------------
  Supported NCPs
---------------------------------------------------------------------------*/
typedef enum ppp_ncp_e_type
{
  PPP_NCP_MIN          = 0,
  PPP_NCP_IPCP         = PPP_NCP_MIN,
  PPP_NCP_IPV6CP       = 1,
  PPP_NCP_VSNCP_3GPP2  = 2,
  PPP_NCP_NAME_MAX
} ppp_ncp_e_type;

/*---------------------------------------------------------------------------
  Union type for NCP configuration
---------------------------------------------------------------------------*/
typedef union ppp_ncp_config_union_s
{
  uint32                      dummy_ipcp;
  uint32                      dummy_ipv6cp;
  ppp_vsncp_3gpp2_config_type vsncp_3gpp2;
} ppp_ncp_config_union_type;

/*---------------------------------------------------------------------------
  supported negotiated params for MH query
---------------------------------------------------------------------------*/
typedef enum ppp_ncp_negotiated_param_e_type
{
  PPP_NCP_NEGOTIATED_MIN          = 0,
  PPP_NCP_VSNCP_3GPP2_NEGOTIATED_BCM   = PPP_NCP_NEGOTIATED_MIN,
  PPP_NCP_VSNCP_3GPP2_NEGOTIATED_APN,
  PPP_NCP_NEGOTIATED_MAX
} ppp_ncp_negotiated_param_e_type;  


/*---------------------------------------------------------------------------
  The contrainer for query of apn information
----------------------------------------------------------------------------*/
typedef struct ppp_ncp_negotiated_apn_type
{
  boolean apn_is_default;
  uint8   apn_len;
  uint8   apn_name[VSNCP_3GPP2_APN_MAX_VAL_LEN];
} ppp_ncp_negotiated_apn_type;

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================
FUNCTION PPP_NCP_ALLOC()

DESCRIPTION
  This function is used to allocate an NCP instance. A handle to the 
  allocated instance is returned to the caller.

PARAMETERS
  (IN) device: PPP device
  (IN) ncp: The network control protocol 
  (IN) ncp_config_ptr: Pointer to the NCP configuration

RETURN VALUE
  ppp_ncp_handle_type: Handle to the allocated NCP instance on success
  NULL on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
ppp_ncp_handle_type ppp_ncp_alloc
(
  ppp_dev_enum_type           device,
  ppp_ncp_e_type              ncp,
  ppp_ncp_config_union_type * ncp_config_ptr   
);

/*===========================================================================
FUNCTION PPP_NCP_START()

DESCRIPTION
  Starts the specified NCP instance

PARAMETERS
  (IN) handle: The NCP handle allocated via ppp_ncp_alloc

RETURN VALUE
  0: Success
 -1: Failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ppp_ncp_start 
(
  ppp_ncp_handle_type handle
);

/*===========================================================================
FUNCTION PPP_NCP_STOP()

DESCRIPTION
  Stops the specified NCP instance. If this is the only NCP instance active,
  PPP will be terminated.

PARAMETERS
  (IN) handle: The NCP handle allocated via ppp_ncp_alloc

RETURN VALUE
  0: Success
 -1: Failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ppp_ncp_stop 
(
  ppp_ncp_handle_type handle
);

/*===========================================================================
FUNCTION PPP_NCP_GET_NEGOTIATED_PARAM

DESCRIPTION
  Get the negotiated parameter from NCP 

PARAMETERS
  (IN) handle: The NCP handle allocated via ppp_ncp_alloc
  (IN) param_name: the parameter name caller asks for
  (OUT) param_data: the value of parameter
  (IN) param_buf_size: the size of param buffer

RETURN VALUE
  0: Success
 -1: Failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ppp_ncp_get_negotiated_param
(
   ppp_ncp_handle_type handle,
   ppp_ncp_negotiated_param_e_type param_name,
   void * data_buf,
   uint8  data_buf_size
) ;

/*===========================================================================
FUNCTION PPP_NCP_RESYNC()

DESCRIPTION
  Resyncs the specified NCP instance

PARAMETERS
  (IN) handle: The NCP handle allocated via ppp_ncp_alloc

RETURN VALUE
  0: Success
 -1: Failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ppp_ncp_resync 
(
  ppp_ncp_handle_type handle
);

/*===========================================================================
FUNCTION PPP_NCP_ABORT()

DESCRIPTION
  Aborts the specified NCP instance

PARAMETERS
  (IN) handle: The NCP handle allocated via ppp_ncp_alloc

RETURN VALUE
  0: Success
 -1: Failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ppp_ncp_abort 
(
  ppp_ncp_handle_type handle
);

#endif /* PS_PPP_NCP_H */
