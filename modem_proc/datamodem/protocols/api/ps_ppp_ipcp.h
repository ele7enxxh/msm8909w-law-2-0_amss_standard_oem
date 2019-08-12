#ifndef PS_PPP_IPCP_H
#define PS_PPP_IPCP_H
/*===========================================================================

                          P S _ P P P _ I P C P . H

DESCRIPTION

  Copyright 1992-1995 William Allen Simpson
  Licensed to Qualcomm -- 95 Jan 21

Copyright (c) 1995-2009 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/


/*===========================================================================

                            EDIT HISTORY FOR FILE

  $PVCSPath: L:/MM_DATA/ps_ppp_ipcp.h_v 1.3 12 Feb 2003 14:30:58   mlioy  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/api/ps_ppp_ipcp.h#1 $
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
05/07/09    pp     CMI Phase-4: SU Level API Effort.
05/06/09    ss     SLHC De-coupling effort.
01/22/08    scb    Mainlined features under FEATURE_JCDMA_1X
04/19/07    scb    Added a KDDI PPP compliance requirement to configure
                   Application specific IPCP constants
11/28/05  ifk/ssh  Added support for ROHC and IPHC over PPP under
                   FEATURE_DATA_PS_PPP_ROHC and FEATURE_DATA_PS_PPP_IPHC
11/01/04    rr/ks  Added primary_nbns and secondary_nbns IPCP options for
                   WINS
05/05/04    usb    Added prototpe for ipcp_setup_device.
10/24/03    ifk    Added prototpe for ipcp_set_device_constants.
09/25/03    ifk    Moved netmodel_resync_func from here to ps_ppp_defs.h
09/12/03    mvl    Added some JCDMA timing changes.
05/23/03    atp    Reverted number of request attempts to 20.
04/24/03    mvl    Removed duplicate definition of option bits.
02/06/03    mvl    Removed prototype for ipcp_net_model_in_resync().  Reduced
                   the number of Request attemps to 10 and the number or Term
                   attempts to 3.
10/17/02    aku    Increased IPCP_TIMEOUT to 1000ms from 500ms.
09/12/02    mvl    Mainlined netmodel code, generified the network model
                   resync callcack.
07/25/02    mvl    Updates for PPP renaming.
08/29/01  pjb/mvl  Modified the values of option bits to support DNS options.
07/30/01    mvl    Modified prototypes to match new call signatures.
07/28/01    mvl    moved the ipcp_values data struct to pppvalues.h
07/27/01    mvl    Modified to use pstimers and shortened retry to 500ms,
                   address is now a uint32 in ipcp value struct, code cleanup
07/25/01    mvl    Added prototypes for powerup initialization and timer
                   cancellation.
10/28/99    mvl    Added code review changes
08/20/99    mvl    Added declaration of ipcp_save_work_opt()
07/28/99    mvl    changed the NAK_TRY value to 3 from 10 as 10 nak cycles
                   will take more than 3 seconds.  Avoids infinite loop.
06/18/99    mvl    removed references to two address option which is not
                   supported.
06/08/99    mvl    Changed the size of the 'slots' field in the IPCP option
                   value field to a uint8 (which is what the protocol size is
                   for this field).
05/20/99    na     Changed header for ipcp_init() for network model builds.
05/20/99    mvl    Added typedef for various structures
11/09/95    jjw    IS-99 Compliant Release
===========================================================================*/

/*===========================================================================

                                INCLUDE FILES

===========================================================================*/
#include "ps_ppp_fsm.h"
#include "ps_slhc.h"
#include "ps_ppp_defs.h"

/*===========================================================================

                          PUBLIC DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  IPCP option types
---------------------------------------------------------------------------*/
#define IPCP_COMPRESS        2
#define IPCP_1ADDRESS        3
#define IPCP_PRIMARY_DNS     129
#define IPCP_PRIMARY_NBNS    130  /* For WINS support */
#define IPCP_SECONDARY_DNS   131
#define IPCP_SECONDARY_NBNS  132 /* For WINS support */
#define IPCP_OPTION_LIMIT    132  /* highest # we can handle */

/*---------------------------------------------------------------------------
  Size of the accepted options
---------------------------------------------------------------------------*/
#define IPCP_COMPRESS_SIZ      4
#define IPCP_1ADDRESS_SIZ      6
#define IPCP_PRIMARY_DNS_SIZ   IPCP_1ADDRESS_SIZ
#define IPCP_SECONDARY_DNS_SIZ IPCP_1ADDRESS_SIZ
#define IPCP_PRIMARY_NBNS_SIZ   IPCP_1ADDRESS_SIZ /* For WINS support */
#define IPCP_SECONDARY_NBNS_SIZ IPCP_1ADDRESS_SIZ /* For WINS support */
#define IPCP_COMPRESS_VJ_SIZ   6

#define IPCP_SLOT_DEFAULT      16                /* Default # of slots     */
#define IPCP_SLOT_HI           SLHC_MAX_SLOTS    /* Maximum # of slots	   */
#define IPCP_SLOT_LO           SLHC_MIN_SLOTS    /* Minimum # of slots     */
#define IPCP_SLOT_COMPRESS     1                 /* May compress slot id   */
#define IPCP_NO_SLOT_COMPRESS  0                 /* Don't compress slot id */

/*===========================================================================

                        PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================
FUNCTION IPCP_SAVE_WORK_OPT()

DESCRIPTION
   This function stores the value of the IPCP option that is passed in. It is
   being saved into the work and work_pdv the want values are saved by the
   caller.

DEPENDENCIES
  ipcp_init() and ipcp_init() should have been called before this function.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
void ipcp_save_work_opt
(
  struct fsm_side_s *opts,       /* pointer to where the values are stored */
  byte opt_num,                                       /* the option number */
  byte *opt_val,                                /* the value of the option */
  int   opt_val_len
);

/*===========================================================================
FUNCTION IPCP_SET_NET_MODEL_RESYNC_FNC()

DESCRIPTION
  This function sets the callback function pointer for the IPCP established
  function.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ipcp_set_net_model_resync_fnc
(
  net_model_resync_fptr_type fnc_ptr,
  ppp_dev_enum_type          device
);

/*===========================================================================
FUNCTION IPCP_SETUP_DEVICE()

DESCRIPTION
  This function performs the necessary IP configuration on the specified 
  PPP device and associated iface. Once configured, the device can
  send/receive IP packets to/from the stack above PPP for further 
  processing or forwarding. 

DEPENDENCIES
  IPCP negotiation on the device must be complete and negotiated 
  options/values must be stored in IPCP control block.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ipcp_setup_device
(
  ppp_dev_enum_type device
);

#endif /* PS_PPP_IPCP_H */
