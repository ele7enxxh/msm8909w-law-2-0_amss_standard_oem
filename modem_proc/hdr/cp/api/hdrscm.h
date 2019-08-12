#ifndef HDRSCM_H
#define HDRSCM_H

/*===========================================================================

     H D R   S E S S I O N   C O N F I G U R A T I O N   M A N A G E R

DESCRIPTION
  This contains the declarations for the Session Configuration Manager.

Copyright (c) 2005 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/api/hdrscm.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/19/13   ukl     Reduce Code Duplication HDR-CP
08/15/07   etv     Added support for PHY module.
03/27/07   wsh     Added support for new prior session handling
01/08/07   yll     Added support for CSNA attributes.
04/21/06   hal     Added hdrscm_clear_all_rsp_timers()
01/31/06   etv     Added support for AT-init Neg. for RMAC3.
12/14/05   etv     Bumped up the number of attribs in config req to 20
12/07/05   etv     Minor Data Structure cleanup.
12/06/05   etv     Added SCM support for AMAC, CMAC and GMMCDP.
11/07/05   pba     Moved hdrscm_reg_protocol_name_type to SCMDB
10/26/05   hal     Added support for configuring the ALMP module
10/21/05   pba     Added Broadcast protocol to SCM
10/10/05   etv     Added support for AN init GAUP
10/04/05   pba     Added support to read/write config parameters from SCMDB/EFS
08/30/05   etv     Added RTCMAC and AT-initiated GAUP support in SCM.
08/22/05   etv     Added Initial support for SCM.
07/18/05   dna     Created Module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "hdrhai.h"
#include "hdrind.h"
#include "hdrerrno.h"
#include "dsm.h"

/*===========================================================================

                    DATA DECLARATIONS

===========================================================================*/
#define HDRSCM_CONFIG_REQ_MSG           0x50
#define HDRSCM_CONFIG_RSP_MSG           0x51
#define HDRSCM_ATTRIB_UPDATE_REQ_MSG    0x52
#define HDRSCM_ATTRIB_UPDATE_ACPT_MSG   0x53
#define HDRSCM_ATTRIB_UPDATE_REJ_MSG    0x54

#define HDRSCM_MESSAGE_ID_SIZE          8
#define HDRSCM_TRANS_ID_SIZE            8
#define HDRSCM_LENGTH_SIZE              8

#define HDRSCM_MAX_ATTRIBS_IN_CREQ      40
#define HDRSCM_CONFIG_RSP_MSG_PRI       40
#define HDRSCM_CONFIG_REQ_MSG_PRI       40
#define HDRSCM_BITS_PER_BYTE            8
#define HDRSCM_RESPONSE_TIME_MS         2000

/* Align attribute offset to the next octet boundary */
#define HDRSCM_ALIGN_TO_NEXT_OCTET(attrib_offset)                 \
              ((attrib_offset) = ((((attrib_offset) + 7) / 8) * 8))

/* Information needed by registered parsing 
 * routines to parse a received attribute.
 * This is passed to the parsers as in_ptr
 */
typedef struct
{
  boolean is_gaup;
  uint16  attrib_id;
  uint16  attrib_len;         /* in bits */
  uint16  attrib_offset;      /* in bits */
  uint8   attrib_len_size;    /* in bits */
  uint8   value_id_size;      /* in bits */
} hdrscm_proposed_attrib_type;

/* Information returned to SCM from registered parsing 
 * routines to inform SCM of the outcome of the parsing,
 * and of the value that was accepted (if any).
 * This is passed to the parsers as out_ptr
 */
typedef struct
{
  uint16 attrib_id;       /* Reference field for SCM, not used by parsers */
  uint8 attrib_id_size;   /* Size of the attribute ID in bits */
  uint8 sattr_val_size;   /* Simple Attribute Value Size */
  boolean is_accepted;    /* If any proposed value was accepted */
  uint32 value_id;        /* or value, for simple attributes */
  void *complex_val_ptr;  /* Pointer to static memory allocated by parser */

  /* SCM copies the data from complex_val_ptr into the database within the
   * same thread, and since the parsers only run in SCM context, SCM guarantees
   * the parser can re-use this memory the next time it is called.
   * For simple attributes, the parser should set complex_val_ptr to NULL. */

  uint32 complex_val_size; /* Size of the complex attrib in bytes */

} hdrscm_accepted_attrib_type;
   
 /* EJECT */
/*=============================================================================
FUNCTION HDRSCM_POWERUP_INIT

DESCRIPTION
  This function should be called once upon system startup.  The protocol 
  configuration is initialized. 

DEPENDENCIES
  None.

PARAMETERS
  None
  
RETURN VALUE
  Returns a pointer to the current config data (always valid)

SIDE EFFECTS
  None.
=============================================================================*/
void hdrscm_powerup_init( void );


 /* EJECT */
/*===========================================================================
FUNCTION HDRSCM_CONFIG_MSG_CB

DESCRIPTION
  This function is registered to be called whenever a protocol gives a
  message to HDRAPP1, HDRAPP2, HDRAPP3 in-config instance. All Config 
  messages are processed in the HDRMC task. 

DEPENDENCIES
  None

PARAMETERS
  msg_ptr - Pointer to the DSM item chain holding the message

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrscm_config_msg_cb ( dsm_item_type *msg_ptr );


 /* EJECT */
/*===========================================================================
FUNCTION HDRSCM_IN_USE_CONFIG_MSG_CB

DESCRIPTION
  This function is registered to be called whenever a protocol gives a
  message to HDRAPP1, HDRAPP2, HDRAPP3 in-config instance. All Inuse Config 
  messages are processed in the HDRMC task. 

DEPENDENCIES
  None

PARAMETERS
  msg_ptr - Pointer to the DSM item chain holding the message

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrscm_in_use_config_msg_cb
(
  dsm_item_type *msg_ptr
);

 /* EJECT */
/*===========================================================================

FUNCTION HDRSCM_PROCESS_MSG

DESCRIPTION
  This function processes messages for SCM.

DEPENDENCIES
  None

PARAMETERS
  protocol_name - Name of the protocol the message is destined to.
  msg_ptr   - Pointer to the DSM item chain holding the message
  protocol_instance - Instance of the protocol the message is for

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrscm_process_msg
(
  dsm_item_type   *msg_ptr, 
  hdrhai_protocol_instance_enum_type protocol_instance
);


 /* EJECT */
/*===========================================================================
FUNCTION HDRSCM_IND_CB

DESCRIPTION
  This function is registered to be called whenever a protocol gives an
  indication of interest to HDRAPP1, HDRAPP2, HDRAPP3. Indications given 
  to SCM are processed in HDRMC task context.

DEPENDENCIES
  None.

PARAMETERS
  ind_name - Indication to process, is a global enum for all
             possible indications in HDR signaling
  ind_data - Optional data pointer for indication specific data

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrscm_ind_cb
(
  hdrind_ind_name_enum_type ind_name,
  void *ind_data_ptr
);


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCM_RESERVE_RSP_TIMER

DESCRIPTION
  This function is used to reserve a response timer instance for either AT-init
  GAUP or Initial AT-initiated Session configuration. The reserved rsp timer 
  resource is released and returned to queue when,
    1. When the timer expires and
    2. When a response is received back before timer expiry.

  This routine needs to be re-entrant. Since it could be invoked from several 
  different task contexts interested in sending GAUP update request or Config
  requests. It has been made re-entrant by protecting the updates to global 
  variable using a critical section variable.

DEPENDENCIES
  None.

PARAMETERS
  protocol_name - Name of the protocol requesting reservation.
  p_timer_id    - Constant Pointer to reserved timer ID.
  trans_id      - Transaction ID of the message about to be sent.
  is_gaup       - Is the timer request for GAUP purposes? 

RETURN VALUE
  E_SUCCESS - If a timer was available for use.
  E_NOT_AVAILABLE - If No free timers are available.

SIDE EFFECTS
  There are only finite number of timers available in the pool. Appropriate 
  corrections to this finite count needs to be made before trying to add new
  protocols to use timers from this pool. Gives a Fatal error if all the timers
  are already in use.

===========================================================================*/
hdrerrno_enum_type hdrscm_reserve_rsp_timer
(
  hdrhai_protocol_name_enum_type const protocol_name,
  uint8 *const p_timer_id,
  uint8 const trans_id,
  boolean is_gaup
);

 /* EJECT */
/*===========================================================================

FUNCTION HDRSCM_TX_OUTCOME_CB

DESCRIPTION
  This function is as a callback funtion for transmission outcome of the
  configuration message.

DEPENDENCIES
  None

PARAMETERS
  data        - user data
  status      - message transmission status

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrscm_tx_outcome_cb
(
  void                 *data_ptr,
  hdrerrno_enum_type    err_val
);

 /* EJECT */
/*===========================================================================
FUNCTION HDRSCM_PROCESS_TIMER

DESCRIPTION
  This function processes all timer expiration events for the Session 
  Configuration Manager.
    
DEPENDENCIES
  None

PARAMETERS
  timer_id - Timer ID of the expired timer

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrscm_process_timer
(
  uint32 timer_id
);


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCM_IS_GAUP_PENDING

DESCRIPTION
  AT is not allowed to send an AttributeUpdateRequest message for an attribute 
  while a response for the GAUP of same attribute is already pending. This 
  function is used to find out if a GAUP update sent by this protocol is 
  already in the pipeline.

  Note: This API doesn't check for the attribute ID. It only checks if the 
  protocol has a pending GAUP request. Most protocols have AT-init GAUP support
  for atmost one attribute. Because of this fact, it makes sense to use this 
  API to find if a GAUP update is pending.

DEPENDENCIES
  None

PARAMETERS
  protocol - protocol name

RETURN VALUE
  TRUE  - If the protocol has a GAUP request pending
  FALSE - Otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean hdrscm_is_gaup_pending
(
  hdrhai_protocol_name_enum_type protocol
);

 /* <EJECT> */
/*===========================================================================

FUNCTION HDRSCM_CLEAR_ALL_RSP_TIMERS

DESCRIPTION
  If a SessionClose is received then all of the previously set ConfigResponse
  timers should be cleared to prevent them from expiring in the next opened
  session.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  All ConfigResponse timers will be cleared.

===========================================================================*/
extern void hdrscm_clear_all_rsp_timers( void );


/*===========================================================================
FUNCTION HDRSCM_CONFIG_IS_NEEDED

DESCRIPTION
  This function is used to find out if an AT-initiated Configuration is 
  needed by a Prtocol Subtype

DEPENDENCIES
  None.

PARAMETERS
  hdrcom_hai_protocol_name_enum_type - protocol

RETURN VALUE
  TRUE - If AT-init config is required.
  FALSE - Otherwise

SIDE EFFECTS
  None.
===========================================================================*/
boolean hdrscm_config_is_needed( hdrcom_hai_protocol_name_enum_type protocol );


#endif /* HDRSCM_H */
