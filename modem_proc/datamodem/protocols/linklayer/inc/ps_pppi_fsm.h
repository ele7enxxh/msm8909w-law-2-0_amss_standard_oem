#ifndef PS_PPPI_FSM_H
#define PS_PPPI_FSM_H
/*===========================================================================

                           P S _ P P P I _ F S M . H

DESCRIPTION
  Header file for the PPP protocol suite Finite State Machine (Internal)

Copyright (c) 2011 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $PVCSPath: MM_DATA/ps_ppp_fsm.h_v   1.8   15 Nov 2002 23:40:02   jeffd  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/linklayer/inc/ps_pppi_fsm.h#1 $
  $Author: mplcsds1 $  $DateTime: 2016/12/13 08:00:03 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
02/24/11    ash    Cleaning up global variables for Q6 free floating effort.
08/31/09    mga    Merged from eHRPD branch
07/14/09    pp     ppp_cb_lock detached from ppp_type.
05/07/09    pp     Initial release. Created as part of CMI Phase-4: SU Level 
                   API Effort [Split from ps_ppp_fsm.h].
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "customer.h"       /* Customer Specific Features */
#include "comdef.h"
#include "dsm.h"
#include "ps_ppp_defs.h"
#include "ps_ppp_task.h"
#include "ps_iface_defs.h"
#include "ps_ppp.h" /* temp hack */

#include "ps_hdlc_lib.h"
#include "pstimer.h"
#include "rex.h"
#ifdef FEATURE_DATA_PS_PPP_ROHC
#include "ps_rohc.h"
#endif /* FEATURE_DATA_PS_PPP_ROHC */
#include "ps_pppi_vend.h"
#include "ps_crit_sect.h"

/*===========================================================================

                          PUBLIC DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  The mask values for LCP/IPCP packets
---------------------------------------------------------------------------*/
#define  PPP_PROTOCOL_LCP_LINGER_MASK   ( 1 << ((uint16)PPP_PROTOCOL_LCP))
#define  PPP_PROTOCOL_IPCP_MASK   ( 1 << ((uint16)PPP_PROTOCOL_IPCP))
#ifdef FEATURE_DATA_PS_IPV6
#define  PPP_PROTOCOL_IPV6CP_MASK ( 1 << ((uint16)PPP_PROTOCOL_IPV6CP))
#endif /* FEATURE_DATA_PS_IPV6 */

#ifdef FEATURE_STA_PPP
/*---------------------------------------------------------------------------
TYPEDEF PPP_FSM_PARAM_ENUM_TYPE

DESCRIPTION
  Enum defining a subset of PPP constants that can acessed by the sta 
  framework
---------------------------------------------------------------------------*/
typedef enum
{
  PPP_FSM_STATE                     = 0,
  PPP_FSM_RX_IFACE_PTR              = 1,
  PPP_FSM_CONFIGURE_TRIES           = 2,
  PPP_FSM_CONFIGURE_TRIES_REMAINING = 3,
  PPP_FSM_TERMINATE_TRIES           = 4,
  PPP_FSM_NAK_TRIES                 = 5,
  PPP_FSM_NAK_TRIES_REMAINING       = 6,
  PPP_FSM_ACK_TIMEOUT               = 7,
  PPP_FSM_TERM_TIMEOUT              = 8,
  PPP_FSM_MAX_PARAM                       /* max value - array sizing      */
} ppp_fsm_type_enum_type;

typedef struct 
{
  ppp_fsm_type         fsm;
  ppp_link_phase_type  phase;
} sta_ppp_type;

#define STA_PPP_MAX_DEV   1

extern sta_ppp_type sta_ppp_cb_array[STA_PPP_MAX_DEV];
extern boolean      sta_ppp_enable;

#endif /* FEATURE_STA_PPP */

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
int fsm_n_option
(
  struct fsm_constant_s *fsm_constants_p,
  uint8 option
);

int ntohcnf
(
  struct config_hdr *cnf,
  dsm_item_type    **bpp
);

dsm_item_type  *htoncnf
(
  struct config_hdr *cnf,
  dsm_item_type     *bp
);

void htonopt_copy
(
  dsm_item_type    **bpp,
  struct option_hdr *opt,
  dsm_item_type    **copy_bpp
);

int ntohopt
(
  struct option_hdr *opt,
  dsm_item_type    **bpp
);

void fsm_proc
(
  struct fsm_s  *fsm_ptr,
  dsm_item_type *bp
);

boolean fsm_timeout
(
  struct fsm_s *fsm_ptr
);

void fsm_passive_wakeup
(
  ppp_fsm_type *fsm_ptr
);

void fsm_up
(
  struct fsm_s *fsm_ptr
);

void fsm_down
(
  struct fsm_s *fsm_ptr
);

void fsm_open
(
  struct fsm_s *fsm_ptr
);

void fsm_close
(
  struct fsm_s *fsm_ptr
);

void fsm_init
(
  struct fsm_s *fsm_ptr
);

void fsm_free
(
  struct fsm_s *fsm_ptr
);

void fsm_irc
(
  ppp_fsm_type *fsm_ptr
);

int fsm_termreq
(
  ppp_fsm_type *fsm_ptr
);

/*===========================================================================
FUNCTION PPPI_GET_CRIT_SECT_LOCK()

DESCRIPTION
  Access function for PPP critical section array
 
PARAMETERS
  Device: Device for which the critical section structure is requested.
 
RETURN VALUE
  Critical section structure.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
ps_crit_sect_type* pppi_get_crit_sect_lock
( 
  ppp_dev_enum_type device 
);

#ifdef FEATURE_STA_PPP
int fsm_getconst
(
  ppp_fsm_type *fsm_ptr,
  ppp_fsm_type_enum_type ppp_fsm_enum
);
#endif /* FEATURE_STA_PPP */

#ifdef FEATURE_STA_PPP
int fsm_setconst
(
  ppp_fsm_type *fsm_ptr,
  ppp_fsm_type_enum_type ppp_fsm_enum,
  uint32 value
);
#endif /* FEATURE_STA_PPP */


/*===========================================================================
FUNCTION PS_PPP_IS_FSM_INACTIVE()

DESCRIPTION
  This macro returns true if FSM is inactive else returns false. The check
  is based on fsm states.

PARAMETERS
  state: FSM state

RETURN VALUE
  TRUE:  if FSM is inactive
  FALSE: if FSM is active
===========================================================================*/
#define PS_PPP_IS_FSM_INACTIVE ps_ppp_is_fsm_inactive
INLINE  boolean ps_ppp_is_fsm_inactive
(
  uint8  state
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( ( state == fsmSTOPPING ) || ( state == fsmCLOSING ) || 
      ( state == fsmSTOPPED )  || ( state == fsmCLOSED  ) || 
      ( state == fsmINITIAL  ) )
  {
    return TRUE;    
  }
  else
  {
    return FALSE;
  }
}

#endif /* _PS_PPPI_FSM_H */
