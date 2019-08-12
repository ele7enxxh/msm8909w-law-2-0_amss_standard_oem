#ifndef PS_PPPI_IPCP_H
#define PS_PPPI_IPCP_H
/*===========================================================================

                          P S _ P P P I _ I P C P . H

DESCRIPTION

Copyright (c) 2011 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/


/*===========================================================================

                            EDIT HISTORY FOR FILE

  $PVCSPath: L:/MM_DATA/ps_ppp_ipcp.h_v 1.3 12 Feb 2003 14:30:58   mlioy  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/linklayer/inc/ps_pppi_ipcp.h#1 $
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
09/27/11    op     Compiler warning fixes
02/24/11    ash    Cleaning up global variables for Q6 free floating effort.
05/07/09    pp     Initial release. Created as part of CMI Phase-4: SU Level 
                   API Effort [Split from ps_ppp_ipcp.h].
===========================================================================*/

/*===========================================================================

                                INCLUDE FILES

===========================================================================*/
#include "datamodem_variation.h"
#include "ps_ppp.h"
#include "ps_ppp_fsm.h"
#include "ps_ppp_defs.h"
#include "ps_slhc.h"

/*===========================================================================

                          PUBLIC DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  IPCP control block
---------------------------------------------------------------------------*/
typedef struct ipcp_pdv_s
{
  slhc_handle_type slhcp;  /* Handle to the slhc instance */
} ipcp_pdv_type;

/*===========================================================================

                        PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================
FUNCTION IPCP_POWERUP_INIT()

DESCRIPTION
  This function is called at system initialization time.  It initializes the
  FSM for IPCP on the interface specified, this includes getting a timer.

RETURN VALUE
  int: -1 on failure
        0 on success

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ipcp_powerup_init
(
  ppp_dev_enum_type iface /* the interface to initialize */
);

/*===========================================================================
FUNCTION IPCP_SET_DEVICE_CONSTANTS()

DESCRIPTION
  This function is called at system initialization time to set the
  configurable protocol constants for IPCP on the interface specified.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  Set configurable protocol constants for IPCP
===========================================================================*/
void ipcp_set_device_constants
(
  ppp_dev_enum_type           device,       /* the interface to initialize */
  nv_ppp_configure_data_type *config
);

/*===========================================================================
FUNCTION PPP_IPCP_INIT_DEVICE_CONSTANT()

DESCRIPTION
  This function is called to set the configurable protocol constants for
  IPCP on the interface specified.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  Set configurable protocol constants for IPCP.
===========================================================================*/
void ppp_ipcp_init_device_constant
(
  ppp_dev_enum_type           device,       /* the interface to initialize */
  ppp_init_consts_enum_type constant,
  uint32 value
);

/*===========================================================================
FUNCTION IPCP_INIT

DESCRIPTION
  This function will Initialize the IPCP configuration structure on the
  given interface( Rm or Um).

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ipcp_init
(
  ppp_dev_enum_type     iface,    /* The interface to init      */
  ppp_dev_opts_type  *opts_ptr  /* options to initialize with */
);

/*===========================================================================
FUNCTION IPCP_START

DESCRIPTION
  This function will Check for PPP Network-Layer Protocol Phase

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ipcp_start
(
  struct ppp_s *ppp_p
);
/*===========================================================================
FUNCTION IPCP_GET_LOCAL_WANT_CB_VALS()

DESCRIPTION
  Access function IPCP local want control block
 
PARAMETERS
  Index: Array index in IPCP local want control block array.
 
RETURN VALUE
  IPCP local want control block

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
ipcp_value_type* ipcp_get_local_want_cb_vals
( 
  uint32 index 
);

/*===========================================================================
FUNCTION IPCP_GET_LOCAL_WORK_CB_VALS()

DESCRIPTION
  Access function IPCP local work control block
 
PARAMETERS
  Index: Array index in IPCP local work control block array.
 
RETURN VALUE
  IPCP local work control block

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
ipcp_value_type* ipcp_get_local_work_cb_vals
( 
  uint32 index 
);

/*===========================================================================
FUNCTION IPCP_GET_REMOTE_WANT_CB_VALS()

DESCRIPTION
  Access function IPCP remote want control block
 
PARAMETERS
  Index: Array index in IPCP remote want control block array.
 
RETURN VALUE
  IPCP remote want control block

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
ipcp_value_type* ipcp_get_remote_want_cb_vals
( 
  uint32 index 
);

/*===========================================================================
FUNCTION IPCP_GET_REMOTE_WORK_CB_VALS()

DESCRIPTION
  Access function IPCP remote work control block
 
PARAMETERS
  Index: Array index in IPCP remote work control block array.
 
RETURN VALUE
  IPCP remote work control block

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
ipcp_value_type* ipcp_get_remote_work_cb_vals
( 
  uint32 index 
);

/*===========================================================================
FUNCTION IPCP_GET_DEFAULT()

DESCRIPTION
  Access function IPCP default values.
 
PARAMETERS
  None
 
RETURN VALUE
  IPCP default values

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
ipcp_value_type* ipcp_get_default_opts_vals_ptr(void);

/*===========================================================================
FUNCTION IPCP_GET_ACCEPT()

DESCRIPTION
  Access function IPCP accept bitmask.
 
PARAMETERS
  None
 
RETURN VALUE
  IPCP accept bitmask.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
negotiate_t ipcp_get_accept_mask(void);

/*===========================================================================
FUNCTION IPCP_STOPPING()

DESCRIPTION
  This function will cleanup IPCP & post down event to MH

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ipcp_stopping
(
  ppp_fsm_type *fsm_ptr
);

#ifdef FEATURE_STA_PPP
/*===========================================================================
FUNCTION PPP_IPCP_SET_INPUT_FUNCS()

DESCRIPTION
  Set the function pointers in the iface to input packets and config the
  iface.  A value of NULL in the passed funtion pointer causes the function
  pointer in the IPCP control block to be set to default.

DEPENDENCIES
  None

RETURN VALUE
  -1 in case of error
   0 in case of success

SIDE EFFECTS
  None
===========================================================================*/
int ppp_ipcp_set_input_funcs
(
  ppp_dev_enum_type        device,
  ppp_iface_input_f_type   iface_input,
  ppp_iface_config_f_type  iface_config
);
#endif /* FEATURE_STA_PPP */
#endif /* PS_PPPI_IPCP_H */
