#ifndef HDRMRLP_API_H
#define HDRMRLP_API_H
/*===========================================================================

          M U L T I - F L O W   R A D I O   L I N K   P R O T O C O L 

                  E X T E R N A L   H E A D E R   F I L E

DESCRIPTION
  This contains the declarations for the Multiflow Radio Link Protocol.
  
Copyright (c) 2009-2011 by Qualcomm Technologies, Incorporated. All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/hdrmrlp_api.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/18/11   vpk     HDR SU API cleanup
09/04/09   ss      New file created for CMI SU level changes.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "hdrcom_api.h"
#include "hdrcp_api.h"
#include "rex.h"
/*===========================================================================

                          H D R M R L P
                    
===========================================================================*/

/*===========================================================================

                        PUBLIC FUNCTION  DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION HDRMRLP_CLEAR_STATS

DESCRIPTION
  This function clears the MRLP log statistics for all registered RLPs.
  
DEPENDENCIES
   None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void hdrmrlp_clear_stats
( 
  void 
);


/*===========================================================================

FUNCTION HDRMRLP_QUEUE_MSG

DESCRIPTION
  This function builds up any pending RLP signaling messages and queues them
  directly to SLP, bypassing the conventional HMP layer queueing.
      
  In each case, the message is built-up, and passed directly to SLP.  SLP is
  responsible for de-allocating the dsm memory items used to build up the
  signaling message.

  This function builds up messages that are specific to a single RLP Flow
  (for example Nak) as well as messages that are meant for more than one Flow
  such as RLPOnRequest. The messages meant for more than one Flow ( Group
  messages) are queued after the RLP specific messages. This is okay as the 
  priority of these messages is lower than the priority of the RLP specific
  messages.

  In addition, since this function is called once per frame build interrupt,
  the function is used to update RLP timer values.  Timing information is 
  derived from the paramater time_since_prev_req.

DEPENDENCIES
  Assumes that a connection is open.  HMP need not know the message is being
  transmitted.

RETURN VALUE
  None

SIDE EFFECTS
  RLP signaling messages are queued on an SLP Tx queue.  

===========================================================================*/
void hdrmrlp_queue_msg
(
  uint16 time_since_prev_req  /* increment of 26.667 ms since last tx req */
);

/*===========================================================================

                          H D R M R L P R E S V
                   
===========================================================================*/

/*===========================================================================

                        PUBLIC DATA STRUCTURES

===========================================================================*/

/* The following definions for commands should STRICTLY be used within the 
   hdrmrlpresv module. */

typedef enum
{
   HDRMRLPRESV_BLD_SIG_MSG_TIMER_EXPIRED    = 0, /* Sig MSG Timer Expiry */
   HDRMRLPRESV_RESERVATION_ON_SEND_SUCCESS  = 1, /* Reservation ON is 
                                                    successfully sent    */
   HDRMRLPRESV_RESERVATION_ON_SEND_FAILURE  = 2, /* Reservation ON is not 
                                                    sent successfully    */
   HDRMRLPRESV_RESERVATION_OFF_SEND_SUCCESS = 3, /* Reservation OFF is 
                                                    successfully sent    */
   HDRMRLPRESV_RESERVATION_OFF_SEND_FAILURE = 4, /* Reservation OFF is not 
                                                    sent successfully    */
   HDRMRLPRESV_TRANS_TIMER_EXPIRED          = 5  /* Transactoin Timer
                                                    Expired              */
} hdrmrlpresv_cmd_enum_type;

/* Parameters for the RASM Callback */
typedef struct 
{
  void *data_ptr;
} hdrmrlpresv_rasm_cb_type;

/* Command structure that is queued up to HDR MC */
typedef struct
{
   hdrmrlpresv_cmd_enum_type          cmd; /* command id */
   hdrcom_hai_protocol_name_enum_type  sender; /* protocol giving command, 
                                             for debug */
   union {
    hdrmrlpresv_rasm_cb_type rasm_cb;      /* Used for SLP Callbacks */
   } data;
}hdrmrlpresv_cmd_type;


/*===========================================================================

                        PUBLIC FUNCTION DECLARATIONS

===========================================================================*/


/*===========================================================================
FUNCTION  HDRMRLPRESV_POWERUP_INIT

DESCRIPTION
  Called once at powerup. Does the powerup initialization for the MRLP
  Reservation.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrmrlpresv_powerup_init
(
  void 
);

/*===========================================================================
FUNCTION  HDRMRLPRESV_PROCESS_CMD

DESCRIPTION       
  This function is called by the hdrmc, in hdrmc task. To Process the 
  commands that have been queued for hdrmrlpresv module. 
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrmrlpresv_process_cmd
( 
  hdrmrlpresv_cmd_type * resv_cmd_ptr 
);

/*===========================================================================
FUNCTION      HDRMRLPRESV_PROCESS_MSG

DESCRIPTION   

DEPENDENCIES  NONE

RETURN VALUE  NONE

SIDE EFFECTS  NONE

===========================================================================*/
void hdrmrlpresv_process_msg
(
  dsm_item_type *msg_ptr
);

/*===========================================================================
FUNCTION      HDRMRLPRESV_IND_CB

DESCRIPTION   Called from hdrind module to give indication to reservation 
              module

DEPENDENCIES  NONE

RETURN VALUE  NONE

SIDE EFFECTS  NONE

===========================================================================*/
void hdrmrlpresv_ind_cb
(
  hdrcom_ind_name_enum_type ind_name,
  void *ind_data
);


/*===========================================================================
FUNCTION      HDRMRLPRESV_PROCESS_IND

DESCRIPTION   Called from hdrmc task 

DEPENDENCIES  NONE

RETURN VALUE  NONE

SIDE EFFECTS  NONE

===========================================================================*/
void hdrmrlpresv_process_ind
(
  hdrcom_ind_name_enum_type ind_name,
  hdrcom_ind_data_union_type * ind_data_ptr
);

/*===========================================================================

                          H D R M R L P C I F
               
===========================================================================*/


/*===========================================================================

                        PUBLIC DATA DECLARATIONS

===========================================================================*/
typedef enum
{
  HDRMRLPCIF_CLIENT_MRLP_SN = 0,
  HDRMRLPCIF_NUM_CLIENTS = 1
} hdrmrlpcif_client_id_type;

typedef struct
{
  uint8 profile_count;
  uint16 profiles[ HDRCP_MRLP_MAX_PROFILE_COUNT ];
} hdrmrlpcif_an_supported_qos_profile_type;


/*===========================================================================

                          H D R M R L P S P D
                    
===========================================================================*/

/*===========================================================================

                     PUBLIC FUNCTION  DECLARATIONS

===========================================================================*/


/*=========================================================================== 
 
FUNCTION       HDRMRLPSPD_INIT
 
DESCRIPTION    Initializes the SPD module
   
DEPENDENCIES   None.
 
RETURN VALUE   None.
 
SIDE EFFECTS   None.
===========================================================================*/ 
void hdrmrlpspd_init(void);

/*=========================================================================== 
FUNCTION       HDRMRLPSPD_TIMER_CB 
 
DESCRIPTION    Function is called if the SPD timer expires and sets a signal
               for the HDRTX task.  It loops through the RLPs that SPD is 
               polling and removes all the stale packets.
                
DEPENDENCIES   NONE 
 
RETURN VALUE   NONE 
 
SIDE EFFECTS   All the stale packets in the RLPs that SPD is polling are 
               removed
===========================================================================*/ 
void hdrmrlpspd_process_sig( rex_sigs_type signal);

#endif /* HDRMRLP_API_H */

