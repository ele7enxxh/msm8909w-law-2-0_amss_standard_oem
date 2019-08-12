#ifndef PS_PPP_IPV6CP_H
#define PS_PPP_IPV6CP_H
/*===========================================================================

                          P S _ P P P _ I P V 6 C P . H

DESCRIPTION
  PPP IPV6CP protocol implementation

  Copyright (c) 2003-2011 Qualcomm Technologies Incorporated. 
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/api/ps_ppp_ipv6cp.h#1 $ 
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
02/24/11    ash    Cleaning up global variables for Q6 free floating effort.
05/06/09    ss     SLHC De-coupling effort.
01/22/08    scb    Mainlined features under FEATURE_JCDMA_1X
05/13/07    scb    Removed the IPv6CP constants and transferred to 
                   ps_ppp_defs.h
04/19/07    scb    Added a KDDI PPP compliance requirement to configure
                   Application specific IPv6CP constants
04/05/06    ifk    Added support for IPv6 compression option.
08/18/04    mvl    Added prototype for ipv6cp_set_device_constants()
09/25/03    ifk    Created module.
===========================================================================*/
/*===========================================================================

                                INCLUDE FILES

===========================================================================*/
#include "datamodem_variation.h"
#ifdef FEATURE_DATA_PS_IPV6
#include "ps_ppp.h"
#include "ps_ppp_fsm.h"
#include "ps_slhc.h"
#include "ps_ppp_defs.h"


/*===========================================================================

                          PUBLIC DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  IPV6CP option types
---------------------------------------------------------------------------*/
#define IPV6CP_IFACE_ID_OPTION        1
#define IPV6CP_COMPRESS_OPTION        2
#define IPV6CP_OPTION_LIMIT           2    /* highest # we can handle */

/*---------------------------------------------------------------------------
  Size of the accepted options
---------------------------------------------------------------------------*/
#define IPV6CP_COMPRESS_OPTION_SIZE       4
#define IPV6CP_IFACE_ID_OPTION_SIZE      10


#define IPV6CP_SLOT_DEFAULT      16                /* Default # of slots   */
#define IPV6CP_SLOT_HI           SLHC_MAX_SLOTS    /* Maximum # of slots   */
#define IPV6CP_SLOT_LO           SLHC_MIN_SLOTS    /* Minimum # of slots   */
#define IPV6CP_SLOT_COMPRESS     1                 /* May compress slot id */

#ifdef FEATURE_DATA_PS_PPP_ROHC
#define IPV6CP_ROHC_MAX_CID_DEFAULT     15
#define IPV6CP_ROHC_MAX_CID_HI          16383
#define IPV6CP_ROHC_MAX_CID_LO          1

#define IPV6CP_ROHC_MRRU_DEFAULT        0
#define IPV6CP_ROHC_MRRU_HI             1500
#define IPV6CP_ROHC_MRRU_LO             0

#define IPV6CP_ROHC_MAX_HEADER_DEFAULT  168
#define IPV6CP_ROHC_MAX_HEADER_HI       168
#define IPV6CP_ROHC_MAX_HEADER_LO       20
#endif /* FEATURE_DATA_PS_PPP_ROHC */

#ifdef FEATURE_DATA_PS_PPP_IPHC
#define IPV6CP_IPHC_TCP_SPACE_DEFAULT     0xf
#define IPV6CP_IPHC_TCP_SPACE_HI          0xff
#define IPV6CP_IPHC_TCP_SPACE_LO          1

#define IPV6CP_IPHC_NON_TCP_SPACE_DEFAULT     0xf
#define IPV6CP_IPHC_NON_TCP_SPACE_HI          0xffff
#define IPV6CP_IPHC_NON_TCP_SPACE_LO          1

#define IPV6CP_IPHC_F_MAX_PERIOD_DEFAULT        256
#define IPV6CP_IPHC_F_MAX_PERIOD_HI             0xffff
#define IPV6CP_IPHC_F_MAX_PERIOD_LO             1

#define IPV6CP_IPHC_F_MAX_TIME_DEFAULT        5
#define IPV6CP_IPHC_F_MAX_TIME_HI             0xffff
#define IPV6CP_IPHC_F_MAX_TIME_LO             1

#define IPV6CP_IPHC_MAX_HEADER_DEFAULT  0xA8
#define IPV6CP_IPHC_MAX_HEADER_HI       0xffff
#define IPV6CP_IPHC_MAX_HEADER_LO       20
#endif /* FEATURE_DATA_PS_PPP_IPHC */

/*---------------------------------------------------------------------------
  IPV6CP control block
---------------------------------------------------------------------------*/
struct ipv6cp_pdv_s
{
  slhc_handle_type slhcp;  /* Handle to SLHC instance */
#ifdef FEATURE_DATA_PS_PPP_ROHC
  void              *rohcp;  /* pointer to ROHC compression block */
#endif /* FEATURE_DATA_PS_PPP_ROHC */
#ifdef FEATURE_DATA_PS_PPP_IPHC
  void              *iphcp;  /* pointer to IPHC compression block */
#endif /* FEATURE_DATA_PS_PPP_IPHC */
};
typedef struct ipv6cp_pdv_s ipv6cp_pdv_type;


/*===========================================================================

                        PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================
FUNCTION IPV6CP_POWERUP_INIT()

DESCRIPTION
  This function is called at system initialization time.  It initializes the
  FSM for IPV6CP on the interface specified, this includes getting a timer.

RETURN VALUE
  int: -1 on failure
        0 on success

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ipv6cp_powerup_init
(
  ppp_dev_enum_type device                 /* the PPP device to initialize */
);


/*===========================================================================
FUNCTION IPV6CP_INIT

DESCRIPTION
  This function will Initialize the IPV6CP configuration structure on the
  given interface( Rm or Um).

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ipv6cp_init
(
  ppp_dev_enum_type     iface,    /* The interface to init      */
  ppp_dev_opts_type  *opts_ptr  /* options to initialize with */
);


/*===========================================================================
FUNCTION IPV6CP_SET_DEVICE_CONSTANTS()

DESCRIPTION
  This function is called at system initialization time to set the
  configurable protocol constants for ipv6cp on the interface specified.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  Set configurable protocol constants for ipv6cp
===========================================================================*/
void ipv6cp_set_device_constants
(
  ppp_dev_enum_type           device,       /* the interface to initialize */
  nv_ppp_configure_data_type *config
);

/*===========================================================================
FUNCTION PPP_IPV6CP_INIT_DEVICE_CONSTANT()

DESCRIPTION
  This function is called to set the configurable protocol constants for
  IPV6CP on the interface specified.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  Set configurable protocol constants for IPV6CP.
===========================================================================*/
void ppp_ipv6cp_init_device_constant
(
  ppp_dev_enum_type           device,       /* the interface to initialize */
  ppp_init_consts_enum_type constant,
  uint32 value
);

/*===========================================================================
FUNCTION IPV6CP_START

DESCRIPTION
  This function will Check for PPP Network-Layer Protocol Phase

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ipv6cp_start
(
  struct ppp_s *ppp_p
);


/*===========================================================================
FUNCTION IPV6CP_SAVE_WORK_OPT()

DESCRIPTION
   This function stores the value of the IPV6CP option that is passed in. It
   is being saved into the work and work_pdv the want values are saved by the
   caller.

DEPENDENCIES
  ipv6cp_init() and ipv6cp_init() should have been called before this 
  function.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
void ipv6cp_save_work_opt
(
  struct fsm_side_s *opts,       /* pointer to where the values are stored */
  byte opt_num,                                       /* the option number */
  byte *opt_val,                                /* the value of the option */
  int   opt_val_len
);


/*===========================================================================
FUNCTION IPV6CP_SET_NET_MODEL_RESYNC_FNC()

DESCRIPTION
  This function sets the callback function pointer for the IPV6CP established
  function.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ipv6cp_set_net_model_resync_fnc
(
  net_model_resync_fptr_type fnc_ptr,
  ppp_dev_enum_type          device
);

/*===========================================================================
FUNCTION IPV6CP_GET_LOCAL_WANT_CB_VALS()

DESCRIPTION
  Access function IPV6CP local want control block
 
PARAMETERS
  Index: Array index in IPV6CP local want control block array.
 
RETURN VALUE
  IPV6CP local want control block

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
ipv6cp_value_type* ipv6cp_get_local_want_cb_vals
( 
  uint32 index 
);

/*===========================================================================
FUNCTION IPV6CP_GET_LOCAL_WORK_CB_VALS()

DESCRIPTION
  Access function IPV6CP local work control block
 
PARAMETERS
  Index: Array index in IPV6CP local work control block array.
 
RETURN VALUE
  IPV6CP local work control block

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
ipv6cp_value_type* ipv6cp_get_local_work_cb_vals
( 
  uint32 index 
);

/*===========================================================================
FUNCTION IPV6CP_GET_REMOTE_WANT_CB_VALS()

DESCRIPTION
  Access function IPV6CP remote want control block
 
PARAMETERS
  Index: Array index in IPV6CP remote want control block array.
 
RETURN VALUE
  IPV6CP remote want control block

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
ipv6cp_value_type* ipv6cp_get_remote_want_cb_vals
( 
  uint32 index 
);

/*===========================================================================
FUNCTION GET_IPV6CP_REMOTE_WORK_CB_VALS()

DESCRIPTION
  Access function IPV6CP remote work control block
 
PARAMETERS
  Index: Array index in IPV6CP remote work control block array.
 
RETURN VALUE
  IPV6CP remote work control block

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
ipv6cp_value_type* ipv6cp_get_remote_work_cb_vals
( 
  uint32 index 
);

/*===========================================================================
FUNCTION IPV6CP_GET_DEFAULT()

DESCRIPTION
  Access function IPV6CP default values.
 
PARAMETERS
  None
 
RETURN VALUE
  IPV6CP default values

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
ipv6cp_value_type* ipv6cp_get_default_opts_vals_ptr(void);

/*===========================================================================
FUNCTION IPV6CP_GET_ACCEPT()

DESCRIPTION
  Access function IPV6CP accept bitmask.
 
PARAMETERS
  None
 
RETURN VALUE
  IPV6CP accept bitmask.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
negotiate_t ipv6cp_get_accept_mask(void);

/*===========================================================================
FUNCTION IPV6CP_STOPPING()

DESCRIPTION
  This function will cleanup IPv6CP & post down event to MH

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ipv6cp_stopping
(
  ppp_fsm_type *fsm_ptr
);

#endif /* FEATURE_DATA_PS_IPV6 */
#endif /* PS_PPP_IPV6CP_H */
