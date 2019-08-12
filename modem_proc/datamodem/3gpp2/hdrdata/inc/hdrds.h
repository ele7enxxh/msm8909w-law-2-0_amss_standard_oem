#ifndef HDRDS_H
#define HDRDS_H

/*=========================================================================

          H D R  D A T A  S E R V I C E S  T A S K  H A N D L E R

DESCRIPTION
  This module is the task level handler for HDR data service modules. The
  module handles queuing of events (messages, commands & indications) for the
  ds task and de-queuing and calling the appropriate processing functions
  from the ds task.

Copyright (c) 2001 - 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$PVCSPath: O:/src/asw/COMMON/vcs/hdrds.h_v   1.4   09 Dec 2001 20:05:50   vramaswa  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/hdrdata/inc/hdrds.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $
                     
when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/18/11   vpk    HDR SU API cleanup
02/08/11   ls      Global variable cleanup
09/04/09   ss      CMI SU level modifications.
03/09/09   sa      AU level CMI modifications.
02/10/05   sy      Made HDRDS_NETMDL_PS_Q_LIM 5000, same as relay model.
08/28/03   vas     Code cleanup
12/09/01   vr      Made hdrds_ps_q_lim a variable so that we can have
                   different limits for relay and net. model calls
12/09/01   vr      Added hdrds_ps_q_lim_exceeded and HDRDS_PS_Q_LIM 
                   for FL flow control
11/09/01   rao     Added DSM REUSE Optimization support.
10/17/01   vas     Changes to use ds command queue instead of signal. Removed 
                   hdrds_get_event_q_cnt(). Changed hdrds_process_event()
07/30/01   vas     Created Module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "hdrcom_api.h"
#include "dsm.h"

#ifdef FEATURE_DS
#include "dstask_v.h"
#include "hdrds_api.h"
#endif /* FEATURE_DS */


#if defined(FEATURE_HDR_DSMREUSE_OPT)
/*---------------------------------------------------------------------------
  Structure of reusable DSM items and the item count used in DSM REUSE
  Optimization    
---------------------------------------------------------------------------*/
typedef struct
{
  dsm_item_type  
       *item[HDRDSM_FREE_DSM_REUSE_STACK_MAX];   /* Reusable DSM item ptrs */
  uint16 count;                                  /* Item count */
} hdrds_dsmreuse_stack_type;

#endif /* (FEATURE_HDR_DSMREUSE_OPT) */

/*--------------------------------------------------------------------------
  Flag to flow control FL based on number of items in PS PPP RX queue.
--------------------------------------------------------------------------*/
/*UNFINISHED - tune this for 6500 once USB is functional */
#define HDRDS_NETMDL_PS_Q_LIM    5000 
#define HDRDS_RELAYMDL_PS_Q_LIM  5000 // Will never reach this, so
                                      // essentially infinite
/*===========================================================================

                      FUNCTION  DECLARATIONS

===========================================================================*/
/*===========================================================================
FUNCTION      GET_HDRDS_PS_Q_LIM_PTR

DESCRIPTION   Accessor function for hdrds_ps_q_lim
  
DEPENDENCIES  None

RETURN VALUE  Address of hdrds_ps_q_lim

SIDE EFFECTS  None
===========================================================================*/
uint16* get_hdrds_ps_q_lim_ptr 
(
  void
);

/*===========================================================================
FUNCTION      GET_HDRDS_PS_Q_LIM_EXCEEDED_PTR

DESCRIPTION   Accessor function for hdrds_ps_q_lim_exceeded
  
DEPENDENCIES  None

RETURN VALUE  Address of hdrds_ps_q_lim_exceeded

SIDE EFFECTS  None
===========================================================================*/
boolean* get_hdrds_ps_q_lim_exceeded_ptr 
(
  void
);

/*===========================================================================
FUNCTION HDRDS_INIT

DESCRIPTION
  This function initializes the HDR DS module.

DEPENDENCIES
  None.

PARAMETERS
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrds_init( void );

/*===========================================================================
FUNCTION HDRDS_QUEUE_MSG

DESCRIPTION
  This function queues a message on the hdrds task event queue and sends a 
  command to the main ds task command handler. Along with the message, the
  stream that identifies the instance of the protocol for which the message
  is meant, is stored.  This function is called in the task context of 
  whoever is queuing the received message.
  
DEPENDENCIES
  None.

PARAMETERS
  protocol_name - Who the message is for
  item_ptr      - Pointer to the message to be queued

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrds_queue_msg
(
  hdrcom_hai_protocol_name_enum_type protocol_name,
  dsm_item_type *item_ptr
);

/*===========================================================================
FUNCTION HDRDS_QUEUE_IND

DESCRIPTION
  This function queues an indication on the hdrds task event queue and sends
  a command to the main ds task command handler. If there is any data 
  associated with the indication, it is copied into the buffer before it is 
  queued.  This function is called in the task context of whoever is giving 
  the indication. 
  
DEPENDENCIES
  None.

PARAMETERS
  protocol_name - Who the indication is for
  ind_name - Global enumeration of indications
  ind_data_ptr - Pointer to the indication data

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrds_queue_ind
(
  hdrcom_hai_protocol_name_enum_type protocol_name,
  hdrcom_ind_name_enum_type ind_name,
  void *ind_data_ptr
);

/*===========================================================================
FUNCTION HDRDS_QUEUE_CMD

DESCRIPTION            
  This function queues an indication on the hdrds task event queue and sends
  a command to the main ds task command handler. The command data is copied
  into the command buffer before it is queued. This function is called in the
  task context of whoever is giving the command.
  
DEPENDENCIES
  None.

PARAMETERS
  protocol_name - Who the command is for
  cmd_ptr - Pointer to a the command
  cmd_size - Size of the command being given

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrds_queue_cmd
(
  hdrcom_hai_protocol_name_enum_type protocol_name,
  void *cmd_ptr,
  uint32 cmd_size
);

#ifdef FEATURE_DS
/*===========================================================================
FUNCTION HDRDS_PROCESS_HDR_EVENT_CMD

DESCRIPTION
  The function is called from the ds task whenever there is a event
  (message, indication or command) for a HDR DS protocol. The function
  determines the particular event and routes it to the appropriate
  protocol processing function.
  
DEPENDENCIES
  None.

PARAMETERS
  ds_cmd_ptr - Pointer to event being processed 

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrds_process_hdr_event_cmd
(
  ds_cmd_type  *ds_cmd_ptr
);

#endif /* FEATURE_DS */
#endif /* HDRDS_H */

