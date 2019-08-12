#ifndef PS_LO_H
#define PS_LO_H
/*===========================================================================

                                  P S _ L O. H

GENERAL DESCRIPTION
  The DMSS loopback interface allows applications on the phone to
  communicate with each other via DS sockets.
  The loopback interface is implemented as a ps_iface, i.e. the
  loopback traffic traverses the complete IP stack.

EXTERNALIZED FUNCTIONS
  lo_powerup_init()
  lo_init()
  lo_tx_cmd()

INITIALIZATION AND SEQUENCING REQUIREMENTS
  lo_powerup_init() and lo_init() should be called as part of the PS initialization.

Copyright (c) 2002-2008 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/inet/inc/ps_lo.h#1 $

when       who   what, where, why
--------   ---   ----------------------------------------------------------
09/12/08   pp    Metainfo optimizations.
01/29/07   hm    Split lo_init into two functions to do Q initializations
                 before PS task starts.
04/18/05   mct   Added support for v6 loopback.
10/30/04   ks    Changed ptr to ptr of ps_meta_info_type to single pointer
                 in lo_tx_cmd().
01/31/04   usb   Removed externalized /dev/lo ACL.
05/27/03   om    Externalized the /dev/lo ACL.
02/13/03   ss    Included ps_aclrules.h
11/01/02   om    Created Module.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "target.h"
#include "ps_acl.h"
#include "ps_aclrules.h"
#include "ps_tx_meta_info.h"
#include "ps_iface_defs.h"


/*===========================================================================

                     DATA TYPES AND DEFINITIONS

===========================================================================*/



/*===========================================================================

                        FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================
FUNCTION LO_POWERUP_INIT()

DESCRIPTION
  This function is called at powerup (PS initialization) to create
  the loopback interface. This function does all the independent
  initializations without having to read NV.

DEPENDENCIES
  ps_mem and Q subsystems must be initialized.

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void lo_powerup_init( void );


#ifdef FEATURE_DATA_PS_IPV6
/*===========================================================================
FUNCTION LO_INIT()

DESCRIPTION
  This function is called at powerup (PS initialization) to create
  the loopback interface. This function reads the NV to do IPv6 loopback
  iface initializations.

DEPENDENCIES
  lo_init() should be called before.

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void lo_init( void );
#endif

/*===========================================================================
FUNCTION LO_TX_CMD()

DESCRIPTION
  This function is used as transmit command for the loopback interface.
  It discards any meta info and send the IP datagramm back to the stack.

DEPENDENCIES
  None

PARAMETERS
  this_iface_ptr     - Pointer to transmitting interface
  pkt_chain_ptr      - Reference pointer to IP datagramm
  meta_info_ptr      - Pointer to packet's meta info
  tx_cmd_info        - User data pointer (not used)

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
int lo_tx_cmd
(
  ps_iface_type*          this_iface_ptr,
  dsm_item_type**         pkt_chain_ptr,
  ps_tx_meta_info_type*   meta_info_ptr,
  void*                   tx_cmd_info
);

/*===========================================================================

                    ACCESS CONTROL LIST DEFINITION

===========================================================================*/

ACL_DEF( lo4_rt_acl );

#ifdef FEATURE_DATA_PS_IPV6
ACL_DEF( lo6_rt_acl );
#endif /* FEATURE_DATA_PS_IPV6 */
#endif /* PS_LO_H */
