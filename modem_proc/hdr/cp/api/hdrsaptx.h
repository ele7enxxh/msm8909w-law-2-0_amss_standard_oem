#ifndef HDRSAPTX_H
#define HDRSAPTX_H

/*===========================================================================

       H D R SIGNALING ADAPTATION P R O T O C O L TRANSMIT MODULE


DESCRIPTION
  This file implements the Signaling Adaptation Protocol Transmit module

EXTERNALIZED FUNCTIONS (Global)

EXTERNALIZED FUNCTIONS (Regional)

INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2011 - 2013  by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
07/08/13   vko     Ignore L2 ACK Failure, when tunnel is not enabled
10/25/12   wsh     Do not flood NAS during TAU/SRB2 unavailability 
09/11/12   cnx     Handle TX and RX messages during active handoff
04/24/12   cnx     Fixed compiler warnings.
01/05/12   sju     Added verbose debug f3 support for SAP protocols. 
04/21/11   sju     Added support for L2 Ack. 
04/11/11   cnx     Fixed compiler errors.
03/22/11   sju     Created module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "hdr_variation.h"
#include "qw.h"
#include "comdef.h"
#include "rex.h"
#include "dsm.h"
#include "hdrhai.h"
#include "hdrerrno.h"
#include "hdrbit.h"


#ifdef FEATURE_LTE_TO_HDR_OH
#include "hdrind.h"
#include "emm_irat_if_msg.h"

/*===========================================================================

          DEFIDLEIONS  FROM  THE  HDR  AIR  INTERFACE  SPEC

===========================================================================*/


/*===========================================================================

               DATA DECLARATIONS

===========================================================================*/
      
typedef enum
{
  HDRSAPTX_GET_PKT_TIMER,   /* Get packet timer */
  HDRSAPTX_CON_CLOSE_TIMER  /* Silent ConClose timer */
} hdrsaptx_timer_name_enum_type;

typedef enum
{
  HDRSAPTX_START_TX_CMD = 0,
  HDRSAPTX_STOP_TX_CMD = 1,
  HDRSAPTX_CLR_INFO_TX_CMD = 2
} hdrsaptx_cmd_enum_type;

typedef struct
{
   hdrsaptx_cmd_enum_type          cmd;    /* command id */
   hdrhai_protocol_name_enum_type sender; /* protocol giving command */
   union {
     hdrhai_channel_enum_type    tx_chan; 
   } data;
}hdrsaptx_cmd_type;

/*  A callback function that is invoked when get pkt timer expires. */
typedef void (*hdrsaptx_frame_cb_type)( void );

/*===========================================================================
FUNCTION HDRSAPTX_POWERUP_INIT

DESCRIPTION
  This function initializes the HDRSAPTX .
  
DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
  
===========================================================================*/
void hdrsaptx_powerup_init ( void );

/*===========================================================================

FUNCTION hdrsaptx_start_tx

DESCRIPTION
  This function commands SAPTX to start transmitting packet. 

DEPENDENCIES
  None

PARAMETERS
  caller - The protocol calling this function 
  tx_chan - Channel the message must go out on (AC or RTC)

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrsaptx_start_tx 
(  
  hdrhai_protocol_name_enum_type caller,
  hdrhai_channel_enum_type tx_chan 
);

/* EJECT */
/*===========================================================================

FUNCTION HDRSAPTX_STOP_TX

DESCRIPTION
  This function stops SAP transmission.

DEPENDENCIES

PARAMETERS
  caller - The protocol calling this function 

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void hdrsaptx_stop_tx 
( 
  hdrhai_protocol_name_enum_type caller
);

/*===========================================================================
FUNCTION HDRSAPTX_IND_CB

DESCRIPTION
  This function processes incoming indications for this protocol.
  
DEPENDENCIES
  None.

PARAMETERS
  ind_name     - Name of the indication from hdrhai.h
  ind_data_ptr - Pointer to any indication-specific data
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrsaptx_ind_cb
(
  hdrind_ind_name_enum_type ind_name,
  void *ind_data_ptr 
);

/*===========================================================================

FUNCTION HDRSAPTX_PROCESS_CMD

DESCRIPTION
  This function processes commands given to the saptx protocol.

DEPENDENCIES
  None

PARAMETERS
  cmd_ptr - Pointer to the command for the saptx protocol to process

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void hdrsaptx_process_cmd
(
  hdrsaptx_cmd_type *cmd_ptr
);

/*===========================================================================
FUNCTION HDRSAPTX_TIMER_CB

DESCRIPTION
  This function is the callback for all saptx module timers.
  This function is called by REX when the timer expires, and runs 
  in the context of the task that defined the timer (HDRTX).

DEPENDENCIES
  None.

PARAMETERS
  timer_id - The ID of the timer that has expired

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrsaptx_timer_cb
(
  uint32 timer_id
);

/*===========================================================================
FUNCTION HDRSAPTX_PROCESS_TIMER                                       EXTERNAL

DESCRIPTION
  This function processes timer expiry event for this protocol.

DEPENDENCIES
  None.

PARAMETERS
  timer_id: Timer Id

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrsaptx_process_timer
(
 uint32 timer_id
);

/*===========================================================================

FUNCTION HDRSAPTX_PROCESS_IND

DESCRIPTION
  This function processes indications given to SAPTX.

DEPENDENCIES
  None.

PARAMETERS
  ind_name - Name of the indication for SAPTX to process
  ind_data_ptr - pointer to data associated with the indication

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void hdrsaptx_process_ind
(
  hdrind_ind_name_enum_type ind_name,
  hdrind_ind_data_union_type * ind_data_ptr
);

/*===========================================================================

FUNCTION HDRSAPTX_PROCESS_IRAT_UL_MSG_FAILURE_IND

DESCRIPTION
  This function processes the indication sent from NAS indicating the 
  successful transmission of the message on LTE aire interface. 

DEPENDENCIES
  None

PARAMETERS
  ind_data_ptr - pointer to the indication data

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrsaptx_process_irat_ul_msg_failure_ind
(
  emm_irat_failure_ind_type       *ul_msg_failure_ptr
);

/*===========================================================================

FUNCTION HDRSAPTX_PROCESS_IRAT_UL_MSG_CNF_IND

DESCRIPTION
  This function processes the indication sent from NAS indicating the 
  successful transmission of the message on LTE aire interface. 

DEPENDENCIES
  None

PARAMETERS
  ind_data_ptr - pointer to the indication data

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrsaptx_process_irat_ul_msg_cnf_ind
(
#ifdef FEATURE_IRAT_DSM_SUPPORT
  emm_irat_3gpp2_ul_msg_cnf_type *ul_msg_cnf_ptr
#else
  emm_irat_ul_msg_cnf_type       *ul_msg_cnf_ptr
#endif /* FEATURE_IRAT_DSM_SUPPORT */
);

/*===========================================================================

FUNCTION HDRTS_REGISTER_FRAME_TIMER_CALLBACK

DESCRIPTION
  This function registers frame timer callback function which will be called
  after frame timer (26.66ms) expires.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrsaptx_register_frame_timer_callback
(
  hdrsaptx_frame_cb_type              cb
    /* callback function to be registered */
);

/*===========================================================================
FUNCTION HDRSAPTX_SET_VERBOSE_DEBUG

DESCRIPTION
  This function initializes the SAPTX verbose debug setting .
  
DEPENDENCIES
  None.

PARAMETERS
  verbose_debug_setting - current verbose debug setting.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
  
===========================================================================*/
void hdrsaptx_set_verbose_debug 
( 
  uint8 verbose_debug_setting 
);

/*===========================================================================

FUNCTION HDRSAPTX_CLR_INFO_TX

DESCRIPTION
  This function clears the SAP tx status table and pkt_timer

DEPENDENCIES

PARAMETERS
  caller - The protocol calling this function 

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrsaptx_clr_info_tx 
( 
  hdrhai_protocol_name_enum_type caller
);


#endif /* FEATURE_LTE_TO_HDR_OH */

#endif /* HDRSAPTX_H */

