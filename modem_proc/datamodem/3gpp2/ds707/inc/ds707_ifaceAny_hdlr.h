#ifndef DS_DSSNETANY_H
#define DS_DSSNETANY_H
/*===========================================================================

                      D S G E N _ I F A C E IP _ H D L R . H
                   
DESCRIPTION
  The Data Services Sockets Network state machine header file for IPv6.
  (This is an INTERNAL sockets file)

EXTERNALIZED FUNCTIONS
  dssnetAny_sm_create()
    Creates an instance of the IPIface Handler State machine
  dssnetAny_sm_post_event()
    Post events to the IPIface Handler state machine
  dssnetAny_sm_process_event()
    Processes events posted to the IPIface Handler State machine. 

 Copyright (c)2004 - 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/
/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/inc/ds707_ifaceAny_hdlr.h#1 $
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when        who    what, where, why
--------    ---    ----------------------------------------------------------
03/15/11    mg     Global variable cleanup 
03/09/09    sa     AU level CMI modifications.
08/20/04    mvl    Restructured further to make module more stand alone.
08/18/04    kvd    Changed dssnetANy_sm_create prototype
07/21/04    kvd    Created Module
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#if defined (FEATURE_DATA_PS) && defined (FEATURE_DATA_PS_IPV6)
#include "dstask_v.h"
#include "ps_ppp_defs.h"
#include "ps_ppp.h"
#include "ps_ppp_fsm.h"
#include "ps_ip6_sm.h"
#include "ps_acl.h"
#include "ps_iface.h"
#include "ps_iface_defs.h"

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
                            FORWARD DECLARATIONS
=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
struct __dssnetany_sm_cb_s; /* the state machine data structure            */

/*===========================================================================

                          PUBLIC DATA DECLARATIONS

===========================================================================*/
/*---------------------------------------------------------------------------
TYPEDEF DSSNETANY_SM_EVENT_TYPE

DESCRIPTION
  Type and static variable defining events received by the dssnetIP sm.
---------------------------------------------------------------------------*/
typedef enum
{
  DSSNETANY_IFACE_UP_EV     = 0,
  DSSNETANY_IFACE_DOWN_EV   = 1,
  DSSNETANY_CONFIGURING_EV  = 2,
  DSSNETANY_OPEN_EV         = 3,
  DSSNETANY_TEARDOWN_EV     = 4,
  DSSNETANY_CLOSE_EV        = 5,
  DSSNETANY_MAX_EV
} dssnetAny_sm_event_type;

#define DSSNETANY_PREF_IFACE_DEFAULT DSSNETANY_PREF_IFACE_IPV4


/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                           PRIVATE DATA STRUCTURES

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*---------------------------------------------------------------------------
TYPEDEF dssnetAnyi_state_type

DESCRIPTION
  Type and static variable defining the state of the network.
---------------------------------------------------------------------------*/
typedef enum
{
  DSSNETANY_CLOSED_STATE                = 0,
  DSSNETANY_OPENING_STATE               = 1,
  DSSNETANY_FAILOVER_STATE              = 2,
  DSSNETANY_OPEN_STATE                  = 3,
  DSSNETANY_HANDOVER_STATE              = 4,
  DSSNETANY_HANDUP_STATE                = 5,
  DSSNETANY_HANDDOWN_STATE              = 6,
  DSSNETANY_CLOSING_STATE               = 7
} dssnetAnyi_state_type;

/*---------------------------------------------------------------------------
TYPEDEF dssnetAnyi_iface_preference_type

DESCRIPTION
  Type of the any iface :  primary ifcace preferred vs desired.
---------------------------------------------------------------------------*/
typedef enum
{
  DSSNETANY_IFACE_DESIRED               = 0,
  DSSNETANY_IFACE_PREFERRED             = 1
} dssnetAny_pref_e_type;

/*---------------------------------------------------------------------------
STRUCT __DSSNETANY_SM_CB_S

DESCRIPTION
  DSSNETIP SM control block.
---------------------------------------------------------------------------*/
typedef struct __dssnetAny_sm_cb_s dssnetAny_sm_cb_type;

struct __dssnetAny_sm_cb_s
{
  ps_iface_type           local_iface;            /* the "Any" interface   */
  acl_type                local_acl;              /* the local acl         */
  ps_iface_type          *assoc_iface_ptr;        /* associated ps_iface   */
  ps_iface_type          *pref_iface_ptr;         /* Preferred iface       */
  ps_iface_type          *non_pref_iface_ptr;     /* Non Preferred iface   */
  void                   *assoc_iface_up_cbuf;    /* Iface Up Ev buf       */
  void                   *assoc_iface_down_cbuf;  /* Iface Down Ev buf     */
  void                   *assoc_iface_config_cbuf;/* Config Ev buf         */
  void                   *client_data_ptr;        /* cleint data           */
  dssnetAny_sm_cb_type   *this_sm_ptr;            /* pointer to ourself    */
  dssnetAnyi_state_type   state;                  /* Current state         */
  dssnetAny_pref_e_type   any_pref_mode;
  ppp_failover_e_type     ppp_failover_mode;
  ps_iface_ip_ver_failover_e_type failover_mode;
};

/*---------------------------------------------------------------------------
TYPEDEF DSSNETANY_SM_EVENT_TYPE

DESCRIPTION
  Type and static variable defining events received by the dssnetAny sm.
---------------------------------------------------------------------------*/

#define DS707_MAX_ANY_INSTS          1
#define DS707_DEF_ANY_INSTANCE       0

#if 0
extern dssnetAny_sm_cb_type   ds707_any_instances[DS707_MAX_ANY_INSTS];
/*===========================================================================
MACRO         GET_ANY_SM_PTR

DESCRIPTION   Given an instance, returns the ANY SM instance Ptr.

DEPENDENCIES  None.

RETURN VALUE  See description.

SIDE EFFECTS  None.
===========================================================================*/
#define GET_ANY_SM_PTR(x) &(ds707_any_instances[x])
#endif 

/*===========================================================================

                        PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION DSSNETANY_SM_CREATE()

DESCRIPTION
  This function is is used to initialize the IP Iface Handler state machine
  control block. Called by the mode specific handler. The mode specific
  handler allocates memory for the control block as well as the ps iface.

DEPENDENCIES
  None

RETURN VALUE
 -1 on failure
  0 on success

SIDE EFFECTS
  None
===========================================================================*/
int dssnetAny_sm_create
(
  dssnetAny_sm_cb_type  *instance_ptr,      /* Ptr to Any Iface SM instance*/
  void                  *client_data_ptr,
  acl_fptr_type          acl_f_ptr,
  ps_iface_type         *ipv4_iface_ptr,
  ps_iface_type         *ipv6_iface_ptr,
  ps_iface_ip_ver_failover_e_type failover_mode
);

/*===========================================================================
FUNCTION DSSNETANY_SM_POST_EVENT()

DESCRIPTION
  This function is used to post events to the Any Iface Handler State Machine
  by other modules

DEPENDENCIES
  None.

RETURN VALUE
 -1 on failure
  0 on success

SIDE EFFECTS
  None.
===========================================================================*/
int dssnetAny_sm_post_event
(
  dssnetAny_sm_cb_type     *dssnetany_cb_ptr,/* Ptr to AnyIfaceHdlr SM inst*/
  dssnetAny_sm_event_type  event             /* dssnetAny event type       */
);

/*===========================================================================
FUNCTION DSSNETANY_SM_PROCESS_EVENT()

DESCRIPTION
  This function is used to process events posted to the IPIface Handler state 
  machine. 

DEPENDENCIES
  None.

RETURN VALUE
 -1 on failure
  0 on success

SIDE EFFECTS
  None.
===========================================================================*/
void dssnetAny_sm_process_event
(
  ds_cmd_type *cmd_ptr                          /* Pointer to ds_cmd_type  */
);

#endif /* FEATURE_DATA_PS && FEATURE_DATA_PS_IPV6 */
#endif /* DS_DSSNETANY_H */
