#ifndef HDRRUPI_H
#define HDRRUPI_H

/*===========================================================================

                R O U T E  U P D A T E   P R O T O C O L
                I N T E R N A L   D E F I N I T I O N S

DESCRIPTION
  This contains the declarations for regional types and variables used
  in the different files of RUP.

Copyright (c) 2000 - 2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$PVCSPath: O:/src/asw/COMMON/vcs/hdrrupi.h_v   1.18   09 Jan 2003 11:01:54   maldinge  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/api/hdrrupi.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
03/14/14   vko/dsp FR19338:Reduce call setup time by skipping RF Tx shutdown.
11/02/12   wsh     Disabled tuneaway after active HO for 3 sec.  
06/10/10   pba     IRAT Measurement feature.
10/17/07   sju     Added HDRRUP_SEND_RUP_MSG_CMD.
12/13/05   pba     Changes to support multiple personality negotiation
09/12/05   hal     Added new cmd HDRRUP_CONFIG_CMD for AT-init config
09/09/05   etv     Added support for adding high prio traffic mode.
03/07/05   pba     Added new cmd HDRRUP_CONTINUE_CONNECTION_SETUP_CMD, to 
                   facilitate synchronization between RUP and PAC
05/13/03   mpa     Removed reset_tca_seq_num().  TCAM init moved to hdrrup.c
01/08/03   mpa     Removed unnecessary dependency on hdrsrch.h
09/05/01   dna     Added separate handling for in-use vs. in_config messages.
06/01/01   vas     Re-naming for consistency. Code cleanup. Some re-organizing
06/27/00   vas     Created module.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "hdr_variation.h"
#include "comdef.h"
#include "hdrrup.h"
#include "hdrbit.h"
#include "dsm.h"
#include "rex.h"
#include "hdrerrno.h"
#include "hdrhai.h"
#include "hdrlmac.h"

/* <EJECT> */ 
/*=============================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/
#define HDRRUP_BIT6  0x20
#define HDRRUP_XTEND6 0xE0

/* sign extend 22 bit signed integer to signed long */
#define HDRRUP_EXTEND6(num6) \
  ( int8 ) (( num6 & HDRRUP_BIT6 ) ? ( num6 | HDRRUP_XTEND6 ) : num6 )

/*===========================================================================

             DEFINITIONS  FROM  THE  HDR  AIR  INTERFACE  SPEC

===========================================================================*/

/* <EJECT> */ 

/*===========================================================================

                             DATA DECLARATIONS

===========================================================================*/
/*--------------------------------------------------------------------------
                               RUP COMMANDS

--------------------------------------------------------------------------*/

/* Command types */
/* 
   Note: The cmd enum values are fixed and they are used by the QXDM database
   New enum tag should be added after the current enum tag and also
   assign a fixed value
*/

typedef enum
{
  HDRRUP_ACTIVATE_CMD                  = 0, /* RUP activate */
  HDRRUP_DEACTIVATE_CMD                = 1, /* RUP deactivate */
  HDRRUP_CLOSE_CMD                     = 2, /* RUP close */
  HDRRUP_CONTINUE_CONNECTION_SETUP_CMD = 3, /* RUP continue connection setup */
  HDRRUP_SET_TRAFFIC_MODE_CMD          = 4, /* RUP set traffic mode */
  HDRRUP_CONFIG_CMD                    = 5, /* RUP configure */
  HDRRUP_CONTINUE_AFTER_CONN_CLOSE_CMD = 6, /* RUP continue after con. close */
  HDRRUP_SET_TC_HANDDOWN_MODE_CMD      = 7, /* RUP set TC handdown mode */
  HDRRUP_SEND_RUP_MSG_CMD              = 8, /* RUP send RouteUpdate message */
#ifdef FEATURE_IRAT_PILOT_MEAS
  HDRRUP_IRAT_MEAS_REQ_CMD             = 9  /* RUP get pilot measurement */
#endif /* FEATURE_IRAT_PILOT_MEAS */

} hdrrup_cmd_name_enum_type;

typedef struct
{
  hdrrup_cmd_name_enum_type name;         /* Name of the command */
  hdrhai_protocol_name_enum_type sender;  /* Protocol giving command, 
                                             for debug */
  hdrsrch_tc_mode_enum_type traffic_mode; /* The new traffic mode */
  hdrsrch_tc_handdown_mode_enum_type handdown_mode;
                                          /* TC handdown mode */
#ifdef FEATURE_IRAT_PILOT_MEAS
  hdrsrch_pilot_meas_req_params_type pilot_meas_req; 
                                          /* IRAT pilot measurement req */
#endif /* FEATURE_IRAT_PILOT_MEAS */
} hdrrup_cmd_type;

/*===========================================================================

FUNCTION HDRRUP_PROCESS_CMD

DESCRIPTION
  This function processes commands given to RUP.

DEPENDENCIES
  None

PARAMETERS
  rup_cmd_ptr - Pointer to the command for RUP to process

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrrup_process_cmd
( 
  hdrrup_cmd_type *rup_cmd_ptr 
);

/*===========================================================================

FUNCTION HDRRUP_PROCESS_IND

DESCRIPTION
  This function processes indications given to RUP.

DEPENDENCIES
  None.

PARAMETERS
  ind_name     - Name of the indication for RUP to process
  ind_data_ptr - Any data associated with the indication
  
RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrrup_process_ind
(
  hdrind_ind_name_enum_type   ind_name, 
  hdrind_ind_data_union_type *ind_data_ptr 
);

/*===========================================================================

FUNCTION HDRRUP_PROCESS_MSG

DESCRIPTION
  This function processes messages for RUP.

DEPENDENCIES
  None

PARAMETERS
  item_ptr - Pointer to the DSM item chain holding the message
  protocol_instance - Instance of the protocol the message is for

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrrup_process_msg
( 
  dsm_item_type * item_ptr, 
  hdrhai_protocol_instance_enum_type protocol_instance
);

/*===========================================================================
FUNCTION HDRRUP_START_CONN_SETUP_TIMER

DESCRIPTION
  This function starts the connection setup timer
  
DEPENDENCIES
  None.

PARAMETERS
  None
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrrup_start_conn_setup_timer( void );

/*===========================================================================
FUNCTION HDRRUP_STOP_CONN_SETUP_TIMER

DESCRIPTION
  This function stops the connection setup timer
  
DEPENDENCIES
  None.

PARAMETERS
  None
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrrup_stop_conn_setup_timer( void );

/*===========================================================================
FUNCTION HDRRUP_PROCESS_TIMER

DESCRIPTION
  This function processes all timer expiration events for the Route Update
  protocol.
  
DEPENDENCIES
  None.

PARAMETERS
  timer_id - The ID of the timer that has expired
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrrup_process_timer
( 
  uint32 timer_id
);

/*===========================================================================
FUNCTION HDRRUP_START_STATE_TIMER

DESCRIPTION
  This function starts the state timer
  
DEPENDENCIES
  None.

PARAMETERS
  None
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrrup_start_state_timer( void );

/*===========================================================================
FUNCTION HDRRUP_STOP_STATE_TIMER

DESCRIPTION
  This function stops the state timer
  
DEPENDENCIES
  None.

PARAMETERS
  None
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrrup_stop_state_timer( void );

#ifdef FEATURE_LTE_TO_HDR_OH
/*===========================================================================

FUNCTION HDRRUP_SET_TUNEAWAY_ALLOWED

DESCRIPTION
  This function enable/disables tuneaway by setting HPT mode. When
  tuneaway is disallowed, HPT mode is forced, and settings from
  hdrrup_set_traffic_mode() are overriden. When tuneaway is allowed,
  setting from hdrrup_set_traffic_mode() is restored.
   
DEPENDENCIES
  None.

PARAMETERS 
  allowed - if tuneaway is allowed 
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrrup_set_tuneaway_allowed
(
  boolean allowed
);
#endif /* FEATURE_LTE_TO_HDR_OH */

/*===========================================================================

FUNCTION HDRRUP_RESET_CP_SKIP_DISABLE_TX

DESCRIPTION
  This function resets the skip disable RF TX flag.

DEPENDENCIES
  None.

PARAMETERS 
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrrup_reset_cp_skip_disable_tx(void);

/*===========================================================================

FUNCTION HDRRUP_CP_SKIP_DISABLE_TX_ENABLED

DESCRIPTION
  This function sets the skip disable TX flag, to not to disable RF TX.
   
DEPENDENCIES
  None.

PARAMETERS 
  None.

RETURN VALUE
  TRUE - if skip disabling RF TX is allowed.
  FALSE - if skip disabling RF TX is not allowed.

SIDE EFFECTS
  None.

===========================================================================*/
boolean hdrrup_cp_skip_disable_tx_enabled(void);
#endif               /* HDRRUPI_H */
