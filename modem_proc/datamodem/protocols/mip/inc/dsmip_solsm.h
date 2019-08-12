#ifndef DSMIP_SOLSM_H
#define DSMIP_SOLSM_H
/*===========================================================================

                          D S M I P _ S O L S M . H

DESCRIPTION

 The Data Services Mobile IP Agent Solicitation State Machine header file.

EXTERNALIZED FUNCTIONS
   mip_sol_sm_init()
     Initialize the Solicitation State Machine.
   mip_sol_sm_post_event()
     Post an event to the Solicitation State Machine.

Copyright (c) 2000-2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: O:/src/asw/COMMON/vcs/dsmip_solsm.h_v   1.7   10 Dec 2002 23:13:20   jeffd  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/mip/inc/dsmip_solsm.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
02/24/11    ash    Cleaning up global variables for Q6 free floating effort.
08/21/07    vp     MIP CCoA related changes.
03/21/05    ssh    Changes pursuant to the new typedef ps_timer_handle_type
09/23/03    jd     Added max ASM retries field to ssmi_info block
                   Added SSM_INITIAL_SOL_RETRIES (1) and 
                   SSM_HANDOFF_SOL_RETRIES (19) constants.
12/10/02    jd     Added mip_sol_sm_active so regsm can determine if mobile
                   is awaiting AAM.
11/21/01    mvl    Added new event, SSMI_IMME_SOL_EV to facilitate immediate
                   solicitation. Various minor readability changes. 
11/14/01    ss     Added mip_sol_sm_get_random_byte() prototype
10/24/01    js     Modified to support dsmip_info structure
07/06/01    mvl    Added the iface starting event
06/14/01    mvl    Modified to support new design
11/29/00    mvl    Created module
===========================================================================*/


/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "pstimer.h"

/*===========================================================================

                     REGIONAL CONSTANT DEFINTIONS

===========================================================================*/
#ifndef FEATURE_DATA_PS_MIP_CCOA
#define SSM_INITIAL_SOL_RETRIES   1  /* # of ASM retries during call setup */
#else
#define SSM_INITIAL_SOL_RETRIES   5
#endif
#define SSM_HANDOFF_SOL_RETRIES  19  /* # of ASM retries during handoff    */

/*===========================================================================

                      REGIONAL TYPE DEFINITIONS

===========================================================================*/
typedef enum
{
  SSM_MIN_EV = -1,
  SSM_SOL_TRIGGER_EV  = 0,   /* Solicitation trigger event                 */
  SSM_IMMED_RETRY_EV  = 1,   /* Resend solicitation immediately            */
  SSM_RXED_AGT_ADV_EV = 2,   /* RXed mobility agent advert                 */
  SSM_EXIT_MIP_EV     = 3,   /* Told to exit M.IP                          */
  SSMI_IMMED_SOL_EV   = 4,   /* Immediate solicitation                     */
  SSMI_TIMER_EXP_EV   = 5,   /* Sol timeout event (internal event only!)   */
  SSMI_SOL_FAILED_EV  = 6,   /* Solicitatin failed (internal event only!)  */
  SSM_MAX_EV                 /* THIS MUST BE THE LAST LISTED EVENT         */
} mip_sol_sm_event_type;

/*---------------------------------------------------------------------------
  Declaration of the states of the solicitation state machine.
---------------------------------------------------------------------------*/
typedef enum
{
  SSMI_MIN_STATE = -1,
  SSMI_NULL_STATE,         /* The null state - waiting for a trigger event */
  SSMI_WAIT_TO_SOL_STATE,  /* Waiting to solicit state - wait a short time
                              for AA before sending solicitation           */
  SSMI_SEND_SOL_STATE,     /* Solicitation send state                      */
  SSMI_MAX_STATE
} ssmi_state_type;

/*---------------------------------------------------------------------------
  Structure containing solicitation state machine information
---------------------------------------------------------------------------*/
typedef struct
{
  ps_timer_handle_type timer;
  ssmi_state_type state;
  int8 retry_cnt;
  int8 max_retries;
} ssmi_info_type;

/*===========================================================================

                      REGIONAL DATA DECLARATIONS

===========================================================================*/

/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

                      PUBLIC FUNCTION DECLARATIONS

= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/
/*===========================================================================
MACROS MIP_SOL_SM_SET_MAX_RETRIES

DESCRIPTION
  Macro to centralize the setting of the max ASMs sent by SSM.

PARAMETERS
  retries: new max ASM retries 

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define MIP_SOL_SM_SET_MAX_RETRIES( retries ) \
  mip_get_ssmi_info_ptr()->max_retries = retries;


/*===========================================================================
FUNCTION MIP_SOL_SM_INIT()

DESCRIPTION
  This function initializes the agent solicitation state machine.

  THIS SHOULD ONLY BE CALLED AT POWER UP.

PARAMETERS
  None

RETURN VALUE
  0 on success
 -1 on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int mip_sol_sm_init
(
  void
);


/*===========================================================================
FUNCTION MIP_SOL_SM_POST_EVENT()

DESCRIPTION
  This function posts an event to the Solicitation State Machine.

PARAMETERS
  event: The event that is being posted to the state machine.

RETURN VALUE
  None

DEPENDENCIES
  mip_sol_sm_init() has to have been called first.

SIDE EFFECTS
  None
===========================================================================*/
void mip_sol_sm_post_event
(
  mip_sol_sm_event_type event
);

/*===========================================================================
FUNCTION MIP_SOL_SM_GET_RANDOM_BYTE()

DESCRIPTION
  This function will return a random value between 0 and 255.

PARAMETERS
  None

RETURN VALUE
  uint8: A random value in the range of 0..255.

DEPENDENCIES
  As a pre-condition, this function assumes that ran_seed() has been called
  at least once. Currently, ran_seed() is called by the main control task 
  as part of initialization.

SIDE EFFECTS
  None
===========================================================================*/
uint8 mip_sol_sm_get_random_byte(void);

/*===========================================================================
FUNCTION MIP_SOL_SM_ACTIVE()

DESCRIPTION
  Checks if the solicitation state machine is running

PARAMETERS
  None

RETURN VALUE
  TRUE  if solsm is active
  FALSE if solsm is not active

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
extern boolean mip_sol_sm_active (void);

/*============================================================================
FUNCTION MIP_GET_SSMI_INFO()

DESCRIPTION
  Access function for ssmi_info.

PARAMETERS
  None

RETURN VALUE
  pointer to ssmi_info.

DEPENDENCIES
  None

============================================================================*/
ssmi_info_type* mip_get_ssmi_info_ptr(void);

#endif /* DSMIP_SOLSM_H */
