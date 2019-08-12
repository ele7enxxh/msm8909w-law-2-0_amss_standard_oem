#ifndef PS_PPP_NETMODEL_H
#define PS_PPP_NETMODEL_H
/*===========================================================================

                      P S _ P P P _ N E T M O D E L . H

DESCRIPTION

 The PPP Network model header file. Contains shared variables and enums, as
 well as function prototypes.

Copyright (c) 1998-2011 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_ppp_netmodel.h_v   1.3   15 Nov 2002 23:37:30   jeffd  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/api/ps_ppp_netmodel.h#1 $
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
03/23/11    op     Data SU API cleanup
05/07/09    pp     CMI Phase-4: SU Level API Effort.
09/02/08    rnp    Added STA related changes.
08/16/05    ks     Merged Kavitha's netmodel changes from tip.
03/26/03    mvl    removed authentication featurization
11/14/02   jd/usb  Added protoype for netmdl_set_resync_behaviour() and
                   netmdl_drop_behav_enum_type for parameter
11/04/02    mvl    Moved some #defines that were only used in .c to the .c.
08/13/02    mvl    Removed the iface_cb_array (all the other items are in the
                   PPP cb) moved the set/get_mode() functions to pppi.h.
07/25/02    mvl    Moved input params to ppp control block.  Updates for PPP
                   renaming. 
07/11/02    mvl    Updated for new PPP architecture.
05/23/02    mvl    Renamed iface_stack_type.
05/20/02     vr    Removed hdlc_enabled flag from iface_cb_type
02/07/02    vsk    Added ip_pkt_hdlr_f_ptr IP packet handler callback
                   to iface_cb_type.
11/12/01    dwp    Added hdlc_enabled to iface_cb_type.
07/24/01    sjy    Added prototypes for netmdl_compare_lcp_opts() and 
                   netmdl_compare_ipcp_opts().
07/22/01    mvl    removed iface_stack_enum_type and accompanying macro as
                   they have been moved to dsiface_def.h
06/21/01    mvl    Added prototypes for netmdl_get_meta_state() and
                   netmdl_reset_ppp()
05/18/01    mvl    Added prototype for netmdl_get_mode().
04/24/00    snn    Added support for the CHAP resync.
10/27/99    mvl    Code review changes and clean up.
08/20/99    mvl    Changed OUT_IFACE() macro so compiler doesn't throw
                   warnings.
08/10/99    snn    added support for DS_PAP feature in network model.
04/12/99    mvl    moved the OUT_IFACE() macro from the C file to this file.
10/04/98    mvl    created file
===========================================================================*/


/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "ps_ppp_defs.h"
#include "dsm.h"
#include "ps_ppp_snoop.h"


/*===========================================================================

                      REGIONAL DATA DECLARATIONS

===========================================================================*/
/*---------------------------------------------------------------------------
  Enum for the PPP state.
---------------------------------------------------------------------------*/
typedef enum
{
  PPP_OOC_META_STATE,                             /* the out of call state */
  PPP_INIT_META_STATE,                            /* the init state        */
  PPP_UP_META_STATE,                              /* the UP state          */
  PPP_RESYNC_META_STATE,                          /* the resync state      */
  PPP_RELAY_META_STATE                            /* the relay state       */
} netmdl_meta_state_enum_type;

/*---------------------------------------------------------------------------
  Enum for the PPP resync behaviour.
---------------------------------------------------------------------------*/
typedef enum
{
  NETMDL_RESYNC_MIN = 0,
  NETMDL_RESYNC_DFLT = 0,        /* default behaviour */
  NETMDL_RESYNC_DROP_CALL = 1,   /* force call to drop on resync opt mismatch
 */
  NETMDL_RESYNC_MAX
} netmdl_drop_behav_enum_type;

/*---------------------------------------------------------------------------
   Generic error return values.
---------------------------------------------------------------------------*/
#define RET_ERROR -1
#define RET_OK     0

/*---------------------------------------------------------------------------
  'typedef' for the config ack callbacks.
---------------------------------------------------------------------------*/
typedef enum
{
  NO_ACK_RCVD    = 0x0,
  Rm_ACK_RCVD    = 0x1,
  Um_ACK_RCVD    = 0x2,
  BOTH_ACKS_RCVD = 0x3
} ack_status_enum_type;

/*---------------------------------------------------------------------------
  'typedef' for the LCP option comparison routine

  NOTE: the values allow assignment to a bit mask, so it is possible to
  compose multiple values together to return more than one value.
---------------------------------------------------------------------------*/
typedef enum
{
  NO_LCP_CHANGED     = 0x0,
  MOBILE_LCP_CHANGED = 0x1,
  IWF_LCP_CHANGED    = 0x2
} lcp_opt_change_enum_type;

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================
FUNCTION   NETMDL_SET_META_STATE()

DESCRIPTION
  This function sets the state of the PPP Meta state machine.  This is mostly
  registereing the callbacks that are associated with that state.  The
  notable exception to this is the Resync State, which doesn't have any
  callback associated with it, but does have a fairly complex algorithm.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void netmdl_set_meta_state
(
  netmdl_meta_state_enum_type new_ppp_meta_state /* the new meta state     */
);

/*===========================================================================
FUNCTION NETMDL_GET_META_STATE()

DESCRIPTION
  This function returns the current state of the network meta state machine.

PARAMETERS
  None

RETURN VALUE
  The current network model meta state machine state.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
netmdl_meta_state_enum_type netmdl_get_meta_state
(
  void
);

/*===========================================================================
FUNCTION NETMDL_SNOOP_REG()

  DESCRIPTION
  Wrapper to register snoop callbacks and remember what was registered so
  we can unregister only those that were registered by netmdl later

DEPENDENCIES
  None

  RETURN VALUE
    None

SIDE EFFECTS
  None
===========================================================================*/
void netmdl_snoop_reg
(
  ppp_dev_enum_type      iface,       /* Interface to snoop on             */
  uint16                 protocol,    /* Procotol to snoop                 */
  byte                   msg_type,    /* Protocol spec. msg to snoop       */
  snoop_cb_f_ptr_type    callback     /* Function to call when msg arrives */
);

/*===========================================================================
FUNCTION NETMDL_SNOOP_UNREG_ALL()

DESCRIPTION
  Unregister all callbacks previously snooped by netdml.

  DEPENDENCIES
    None

RETURN VALUE
  None

  SIDE EFFECTS
    None
===========================================================================*/
void netmdl_snoop_unreg_all
(
  void
);

#endif /* PS_PPP_NETMODEL_H */
