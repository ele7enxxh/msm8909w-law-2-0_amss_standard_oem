#ifndef PS_PPPI_LCP_H
#define PS_PPPI_LCP_H
/*===========================================================================

                           P S _ P P P I _ L C P . H

DESCRIPTION

Copyright (c) 2011 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/


/*===========================================================================

                            EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_ppp_lcp.h_v   1.2   12 Feb 2003 14:31:50   mlioy  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/linklayer/inc/ps_pppi_lcp.h#1 $
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
02/24/11    ash    Cleaning up global variables for Q6 free floating effort.
05/07/09    pp     Initial release. Created as part of CMI Phase-4: SU Level 
                   API Effort [Split from ps_ppp_lcp.h].
===========================================================================*/

/*===========================================================================

                                INCLUDE FILES

===========================================================================*/
#include "customer.h"       /* Customer Specific Features */
#include "ps_ppp_fsm.h"
#include "ps_ppp_defs.h"
#include "ps_ppp.h"

/*===========================================================================

                          PUBLIC DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  LCP control block
---------------------------------------------------------------------------*/
struct lcp_pdv_s
{
  int dummy;
};

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================
FUNCTION LCP_POWERUP_INIT()

DESCRIPTION
  This function is called at system initialization time.  It initializes the
  FSM for LCP on the interface specified, this includes getting a timer.

RETURN VALUE
  int: -1 on failure
        0 on success

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int lcp_powerup_init
(
  ppp_dev_enum_type iface /* the interface to initialize */
);

/*===========================================================================
FUNCTION LCP_SET_DEVICE_CONSTANTS()

DESCRIPTION
  This function is called to set the configurable protocol constants for LCP
  on the interface specified.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  Set configurable protocol constants for LCP.
===========================================================================*/
void lcp_set_device_constants
(
  ppp_dev_enum_type           device,       /* the interface to initialize */
  nv_ppp_configure_data_type *config
);

/*===========================================================================
FUNCTION PPP_LCP_INIT_DEVICE_CONSTANT()

DESCRIPTION
  This function is called to set the configurable protocol constants for
  LCP on the interface specified.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  Set configurable protocol constants for LCP.
===========================================================================*/
void ppp_lcp_init_device_constant
(
  ppp_dev_enum_type           device,       /* the interface to initialize */
  ppp_init_consts_enum_type constant,
  uint32 value
);

/*===========================================================================
FUNCTION LCP_INIT()

DESCRIPTION
  This function will initialize the LCP configuration structure for the given
  interface (Rm or Um).

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void lcp_init
(
  ppp_dev_enum_type     iface,   /* The interface to init      */
  ppp_dev_opts_type    *opts_ptr /* options to initialize with */
);

/*===========================================================================
FUNCTION LCP_MRU_GET()

DESCRIPTION
  This function is passed a PPP device and uses that to return the LCP MRU
  associated with the device.

RETURN VALUE
  -1 in case of error or if no MRU is set
  The MRU of the device in case of success

DEPENDENCIES
  LCP should be initialized before this function is called

SIDE EFFECTS
  None
===========================================================================*/
int32 lcp_mru_get
(
  ppp_dev_enum_type device
);

/*===========================================================================
FUNCTION LCP_MRU_SET()

DESCRIPTION
  This function is passed a PPP device and a value of MRU to set.  It uses
  this information to set the LCP MRU associated with the device.

RETURN VALUE
  -1 in case of error
   0 in case of success

DEPENDENCIES
  LCP should be initialized before this function is called

SIDE EFFECTS
  None
===========================================================================*/
int32 lcp_mru_set
(
  ppp_dev_enum_type device,
  int32             mru
);

/*===========================================================================
FUNCTION LCP_UP()

DESCRIPTION
  This function is called when link layer configuration is completed and
  we are ready to move on to the network phase.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void lcp_up
(
  ppp_dev_enum_type device
);

/*===========================================================================
FUNCTION LCP_GET_LOCAL_WANT_CB_VALS()

DESCRIPTION
  Access function LCP local want control block
 
PARAMETERS
  Index: Array index in LCP local want control block array.
 
RETURN VALUE
  LCP local want control block

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
lcp_value_type* lcp_get_local_want_cb_vals
( 
  uint32 index 
);

/*===========================================================================
FUNCTION LCP_GET_LOCAL_WORK_CB_VALS()

DESCRIPTION
  Access function LCP local work control block
 
PARAMETERS
  Index: Array index in LCP local work control block array.
 
RETURN VALUE
  LCP local work control block

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
lcp_value_type* lcp_get_local_work_cb_vals
( 
  uint32 index 
);

/*===========================================================================
FUNCTION LCP_GET_REMOTE_WANT_CB_VALS()

DESCRIPTION
  Access function LCP remote want control block
 
PARAMETERS
  Index: Array index in LCP remote want control block array.
 
RETURN VALUE
  LCP remote want control block

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
lcp_value_type* lcp_get_remote_want_cb_vals
( 
  uint32 index 
);

/*===========================================================================
FUNCTION LCP_GET_REMOTE_WORK_CB_VALS()

DESCRIPTION
  Access function LCP remote work control block
 
PARAMETERS
  Index: Array index in LCP remote work control block array.
 
RETURN VALUE
  LCP remote work control block

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
lcp_value_type* lcp_get_remote_work_cb_vals
( 
  uint32 index 
);

/*===========================================================================
FUNCTION LCP_GET_DEFAULT()

DESCRIPTION
  Access function LCP default values.
 
PARAMETERS
  None
 
RETURN VALUE
  LCP default values

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
lcp_value_type* lcp_get_default_opts_vals_ptr(void);

/*===========================================================================
FUNCTION LCP_GET_ACCEPT()

DESCRIPTION
  Access function LCP accept bitmask.
 
PARAMETERS
  None
 
RETURN VALUE
  LCP accept bitmask.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
negotiate_t lcp_get_accept_mask(void);

#endif /* _PS_PPP_LCP_H */
