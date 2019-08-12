#ifndef PS_PPP_SNOOP_H
#define PS_PPP_SNOOP_H
/*===========================================================================

                         P S _ P P P _ S N O O P . H

DESCRIPTION

 The Data Services Snoop Header File. Contains shared variables and enums, as
 well as declarations for functions.

Copyright (c) 1998-2011 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_ppp_snoop.h_v   1.2   13 Feb 2003 14:12:46   ubabbar  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/api/ps_ppp_snoop.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/23/11    op     Data SU API cleanup
05/07/09    pp     CMI Phase-4: SU Level API Effort.
09/12/08    pp     Metainfo optimizations.
10/14/04   ifk     Added meta info ** argument to snoop callback function
02/11/03    usb    Added SNOOP_CB_FORWARD_PKT to snoop_cb_ret_val_enum_type
                   to indicate that pkt should be forwarded to bridged dev
02/02/03    usb    Changed return value of snoop_proto_msg_detect() from 
                   boolean to snoop_cb_ret_val_enum_type.
07/25/02    mvl    removed netmodel dependency. Updates for PPP renaming.
05/23/02    mvl    Renamed iface_stack_type.
11/07/01    vr     added FEATURE_DS_MOBILE_IP wrapper
11/05/01    vr     added function snoop_is_registration_pkt for increasing
                   laptop throughput during re-registration
10/27/99   mvl     Code review updates: general cleanup.
08/31/98  na/mvl   Created module.
===========================================================================*/


/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "ps_ppp_defs.h"
#include "dsm.h"
#include "ps_rx_meta_info.h"

/*===========================================================================

                      REGIONAL DATA DECLARATIONS

===========================================================================*/
#define SNOOP_ANY_MSG      0     /* snoop all messages of a protocol       */
#define PPP_PROTO_SIZ      2     /* Size of PPP protocol field             */
#define PPP_ACF_PROTO_SIZ  4     /* Proto field without Addr & Ctl compres */

/*---------------------------------------------------------------------------
  Type that is used as the return value for all the snooping callback 
  functions. It is used to determine what action needs to be taken on the 
  return from that function.
---------------------------------------------------------------------------*/
typedef enum
{
  SNOOP_CB_IGNORE_PKT,     /* packet processing complete, ignore it        */
  SNOOP_CB_SNOOP_AGAIN,    /* call snoop callback again                    */
  SNOOP_CB_PROCESS_PKT,    /* send pkt up the stack for further processing */
  SNOOP_CB_FORWARD_PKT     /* forward pkt to the bridged PPP device        */
} snoop_cb_ret_val_enum_type;

/*---------------------------------------------------------------------------
  This is the type declaration for all of the callback functions that are
  registered for snooping.
---------------------------------------------------------------------------*/
typedef snoop_cb_ret_val_enum_type 
          (*snoop_cb_f_ptr_type)(ppp_dev_enum_type iface,
                                 uint16 protocol,
                                 dsm_item_type ** item_head_ptr,
                                 ps_rx_meta_info_type ** meta_info_ref_ptr);

/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

                      PUBLIC FUNCTION DECLARATIONS

= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/

/*===========================================================================
FUNCTION  SNOOP_REG_EVENT()

DESCRIPTION
  This function stores a function pointer that will be used as a callback
  when a particular PPP protocol and message are seen.

DEPENDENCIES
  None

RETURN VALUE
  A 32bit opaque identifier that can be used to unregister the function that
  has been registered.

SIDE EFFECTS
  If this function is called for a protocol and message combination that
  already has a function call associated with it, it will remove the old
  function and replace it with the new function that is being added.
===========================================================================*/
uint32 snoop_reg_event
(
  ppp_dev_enum_type     iface,    /* the stack the registration is for     */
  uint16                protocol, /* the protocol the registration is for  */
  byte                  msg_type, /* the message the registration is for   */
  snoop_cb_f_ptr_type   f_ptr     /* the callback funtion being registered */
);


/*===========================================================================
FUNCTION SNOOP_UNREG_EVENT()

DESCRIPTION
  This function removes a particular callback function from the snoop
  registration data structure.  This stops it from being called when its
  associated protocol and message are found.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void snoop_unreg_event
(
  uint32 reg_tag        /* the xparent reg tag returned by snoop_reg_event */
);

#endif /* PS_PPP_SNOOP_H */
