#ifndef ECALL_PSAP_TASK_H_
#define ECALL_PSAP_TASK_H_
/*===========================================================================

                         E C A L L  _  P S A P  _  T A S K . C

DESCRIPTION
  eCall task source file

REFERENCES
  

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2009-2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/ecall/emodem/inc/ecall_psap_task.h#1 $
$DateTime: 2016/12/13 08:00:05 $
$Author: mplcsds1 $ 
===========================================================================*/

/*===========================================================================

                         I N C L U D E   F I L E S

===========================================================================*/

#include "comdef.h"
#include "customer.h"

#include "cm.h"
#include "ecall_defines.h"
#include "queue.h"
#include "rex.h"

#ifndef FEATURE_ECALL_HAS_V1_HEADER
#include "aprv2_packet.h"
#include "apr_errcodes.h"
#include "aprv2_api_inline.h"
#endif /* FEATURE_ECALL_HAS_V1_HEADER */
/*===========================================================================

                    D A T A   D E F I N I T I O N S

===========================================================================*/

typedef enum
{

  ECALL_PSAP_TASK_OFFLINE_SIG             = 0x2000, /* TASK_OFFLINE_SIG */ 
  ECALL_PSAP_TASK_STOP_SIG                = 0x4000, /* TASK_STOP_SIG */ 
  ECALL_PSAP_TASK_START_SIG               = 0x8000, /* TASK_START_SIG */ 

  ECALL_PSAP_CMD_Q_SIG                    = 0x0001,
  ECALL_PSAP_DOG_RPT_TIMER_SIG            = 0x0002,
  ECALL_PSAP_CM_AUTO_ANSWER_SIG           = 0x0004,
  ECALL_PSAP_RESTART_TIMER_SIG            = 0x0008      
} ecall_psap_sig_enum_type;

typedef enum {
  ECALL_CMD_MIN = 0,
  ECALL_CMD_PROCESS_RX_PCM,
  ECALL_CMD_PROCESS_RESTART_TIME,
  ECALL_CMD_PROCESS_START_PSAP,
  ECALL_CMD_CALL_ORIG,
  ECALL_CMD_INCOMING_CALL,
  ECALL_CMD_END_CALL,
  ECALL_CMD_APR_PACKET,
  ECALL_CMD_MAX
} ecall_cmd_id_e_type;

typedef struct {
  q_link_type           link;
  ecall_cmd_id_e_type   cmd_id;
  union {
    int16                    pcm[ECALL_PCM_LENGTH];  /* ECALL_CMD_PROCESS_RX_PCM */
    uint32                   restart_time;           /* ECALL_CMD_PROCESS_RESTART_TIME */
    cm_call_id_type          call_id;                /* ECALL_CMD_INCOMING_CALL */
    #if ( (defined FEATURE_ECALL_HAS_VOICE_A) || (defined FEATURE_ECALL_HAS_VOICE_B) )
    aprv2_packet_t           *apr_packet;            /* ECALL_CMD_APR_PACKET */
    #endif /* FEATURE_ECALL_HAS_VOICE_A || FEATURE_ECALL_HAS_VOICE_B */
  } data;
} ecall_cmd_type;

#ifdef FEATURE_ECALL_HAS_MMOC
/*============================================================================== 
 
                 M S G R   M o d u l e   D E F I N I T I O N S
 
==============================================================================*/

#define MSGR_MODULE_PSAP        0x03
#define MSGR_ECALL_PSAP         MSGR_TECH_MODULE( MSGR_TECH_ECALL, MSGR_MODULE_PSAP )
#endif /* FEATURE_ECALL_HAS_MMOC */

/*===========================================================================

                  F U N C T I O N   D E C L A R A T I O N S

===========================================================================*/


/*===========================================================================

FUNCTION ECALL_PSAP_TASK

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  This function does not return.

SIDE EFFECTS
  None

===========================================================================*/

extern void  ecall_psap_task
(
  dword ignored
    /* lint -esym(715,ignored) */
);

/*===========================================================================

FUNCTION ECALL_PSAP_WAIT

DESCRIPTION
  This function suspends execution of the ukcc task until one (or
  more) signals from the requested signal mask is set.

  Watchdog kicking is performed in this function.

DEPENDENCIES
  None

RETURN VALUE
  The signal mask returned by rex_wait().

SIDE EFFECTS
  None

===========================================================================*/
rex_sigs_type  ecall_psap_wait
(
   rex_sigs_type  requested_signal_mask       /* Signal mask to suspend on */
);




/*==========================================================================

  FUNCTION ecall_get_cmd_buf

  DESCRIPTION
    Returns a command pointer off of the free queue, or NULL on failure.

  DEPENDENCIES
    None.

  RETURN VALUE
    Command pointer or NULL

  SIDE EFFECTS
    None

===========================================================================*/
ecall_cmd_type *ecall_get_cmd_buf(void);


/*==========================================================================

  FUNCTION ecall_put_cmd

  DESCRIPTION
    Adds a command pointer to the command queue and signals the eCall task
    to process it.

  DEPENDENCIES
    None.

  RETURN VALUE
    None

  SIDE EFFECTS
    eCall task is signaled that there is a pending command

===========================================================================*/
void ecall_put_cmd(
  ecall_cmd_type  *cmd_ptr
  );


/*==========================================================================

  FUNCTION    ECALL_PSAP_TASK_INIT_BEFORE_TASK_START

  DESCRIPTION
     Initializes queues and defualt values for the global data.

  DEPENDENCIES
    None.

  RETURN VALUE
  
  SIDE EFFECTS
    None

===========================================================================*/
void ecall_psap_task_init_before_task_start
(
  void
);



/*==========================================================================

  FUNCTION    ECALL_PSAP_TASK_INIT

  DESCRIPTION
     Initializes queues and defualt values for the global data.

  DEPENDENCIES
    None.

  RETURN VALUE
  
  SIDE EFFECTS
    None

===========================================================================*/
void ecall_psap_task_init
(
  void
);




void ecall_call_err_cb(void *data_ptr, cm_call_cmd_e_type call_cmd,
                    cm_call_cmd_err_e_type call_cmd_err);


#endif /* ECALL_PSAP_TASK_H_ */

