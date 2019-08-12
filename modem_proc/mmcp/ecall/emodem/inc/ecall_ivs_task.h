#ifndef ECALL_IVS_TASK_H_
#define ECALL_IVS_TASK_H_
/*===========================================================================

                           E C A L L _ T A S K . h

DESCRIPTION
  eCall task source file

REFERENCES
  3GPP TS 26.267

EXTERNALIZED FUNCTIONS
       - ecall_ivs_task(dword ignored)
       - void ecall_put_cmd( ecall_cmd_type  *cmd_ptr)
       - ecall_cmd_type *ecall_get_cmd_buf(void)

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2009-2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/ecall/emodem/inc/ecall_ivs_task.h#1 $
$DateTime: 2016/12/13 08:00:05 $
$Author: mplcsds1 $ 
===========================================================================*/

/*===========================================================================

                         I N C L U D E   F I L E S

===========================================================================*/
#include "customer.h"

#ifdef FEATURE_ECALL_IVS
#include "comdef.h"
#include "ecall_defines.h"
#include "ecall_modem_apis.h"
#include "ecall_modem_clients.h"
#include "ecall_sim.h"
#include "queue.h"
#include "rex.h"
#include "sys.h"
#ifndef FEATURE_ECALL_LCU45
#include "npa.h"
#endif /* FEATURE_ECALL_LCU45 */

#ifndef FEATURE_ECALL_HAS_V1_HEADER
#include "aprv2_packet.h"
#endif /* FEATURE_ECALL_HAS_V1_HEADER */

#ifdef FEATURE_ECALL_HAS_MMOC
/*============================================================================== 
 
                 M S G R   M o d u l e   D E F I N I T I O N S
 
==============================================================================*/
#include "mmoc_msgr.h"
#define MSGR_MODULE_IVS        0x02
#define MSGR_ECALL_IVS         MSGR_TECH_MODULE( MSGR_TECH_ECALL, MSGR_MODULE_IVS )
typedef struct
{
  mmoc_offline_ind_s_type   offline_ind;
}ecall_ivs_msgr_cmd_type;

typedef struct
{
  q_link_type          link;         /* Queue link */
  byte                 reserved[4];
  ecall_ivs_msgr_cmd_type  cmd;
} ecall_ivs_msgr_cmd_s_type;
#endif /* FEATURE_ECALL_HAS_MMOC */

#ifdef FEATURE_MMGSDI_SESSION_LIB
#include "mmgsdilib.h"
#endif /* FEATURE_MMGSDI */
/*===========================================================================

                    D A T A   D E F I N I T I O N S

===========================================================================*/

/* Signal on which the IVS task would wait and monitor*/
typedef enum
{

  ECALL_IVS_TASK_OFFLINE_SIG             = 0x2000, /* TASK_OFFLINE_SIG */ 
  ECALL_IVS_TASK_STOP_SIG                = 0x4000, /* TASK_STOP_SIG */ 
  ECALL_IVS_TASK_START_SIG               = 0x8000, /* TASK_START_SIG */ 

  ECALL_IVS_CMD_Q_SIG                    = 0x0001,
  ECALL_IVS_DOG_RPT_TIMER_SIG            = 0x0002,
  ECALL_IVS_VOC_TIMER_SIG                = 0x0004,
  ECALL_IVS_MSGR_Q_SIG                   = 0x0008

} ecall_ivs_sig_enum_type;


/*These are the commands which IVS task commands can expect*/
typedef enum {
  ECALL_CMD_MIN = 0,
  ECALL_CMD_PROCESS_RX_PCM,
  ECALL_CMD_MODEM_START_TX_MSD,
  ECALL_CMD_MODEM_ABORT_TX,
  ECALL_CMD_MODEM_ABORT_ALL,
  ECALL_CMD_SIM_CONFIG_UPDATE,
  ECALL_CMD_SIM_CLIENT_ID_REG_SUCCESS,
  ECALL_CMD_SIM_CLIENT_SESSION_EVENTS,
  ECALL_CMD_SIM_READ_SUCCESS,
  ECALL_CMD_SIM_SESSION_OPEN_SUCCESS,
  ECALL_CMD_CLIENT_REG,
  ECALL_CMD_CLIENT_DEREG, 
  ECALL_CMD_CLIENT_NOTIFY,
  ECALL_CMD_SIM_READ_FAILED,
  ECALL_CMD_SET_EST_FDN_ENABLED,
  ECALL_CMD_SET_CPU_HDL,
  ECALL_CMD_APR_PACKET,
  ECALL_CMD_SET_AND_SEND_OPERATING_MODE,
  ECALL_CMD_UPDATE_CM_SUBS_INFO,
  ECALL_CMD_MAX
} ecall_cmd_id_e_type;

/* Command queue type*/
typedef struct {
  q_link_type               link;
  ecall_cmd_id_e_type       cmd_id;
  union 
  {
    int16                            pcm[ECALL_PCM_LENGTH];
    uint8                            ecall_msd[ECALL_MSD_MAX_LENGTH];
    boolean                          discard_msd;
    ecall_client_type               *client_ptr;
    int16                            clientid;
    ecall_cb_data_type               ivs_status;            /* ECALL_CMD_CLIENT_NOTIFY */
    mmgsdi_client_id_type            gsdi_client_id;        /* ECALL_SIM_CLIENT_ID_REG_SUCCESS */
    ecall_operating_mode_type        ecall_operating_mode;  /* ECALL_CMD_SIM_CONFIG_UPDATE */
    sys_modem_as_id_e_type           as_id;                 /* ECALL_CMD_SET_AND_SEND_OPERATING_MODE */
    #ifndef FEATURE_ECALL_LCU45
    npa_client_handle                cpu_hdl;               /* ECALL_CMD_SET_CPU_HDL */
    #endif /* FEATURE_ECALL_LCU45 */
    #ifndef FEATURE_ECALL_HAS_V1_HEADER
    aprv2_packet_t                   *apr_packet;           /* ECALL_CMD_APR_PACKET */
    #endif /* FEATURE_ECALL_HAS_V1_HEADER */
    struct
    {
      mmgsdi_session_id_type         session_id;
      mmgsdi_session_type_enum_type  session_type;
      mmgsdi_slot_id_enum_type       slot_id;
      mmgsdi_app_enum_type           app_type;
    }mmgsdi_session_open_data;                              /* ECALL_CMD_SIM_SESSION_OPEN_SUCCESS */
    struct
    {
      mmgsdi_event_data_type         event;
      uint8                          bit89_value;     /* ECALL Avail in UST */
      uint8                          bit2_value;      /* FDN Avail in UST   */
      uint8                          bit4_value;      /* SDN Avail in UST   */
      uint8                          est_fdn_enabled; /* Enabled services table */
      ecall_prov_app_info_s_type    *appInfo_ptr;     /* Provision App info pointer */
    }sim_data;
  } data;
} ecall_cmd_type;

/*===========================================================================

                  F U N C T I O N   D E C L A R A T I O N S

===========================================================================*/



/*===========================================================================

FUNCTION ECALL_IVS_TASK

DESCRIPTION
    Starts up the eCall task, waits for the signals

DEPENDENCIES
    FEATURE_ECALL_IVS_TASK is defined.

RETURN VALUE
  This function does not return.

SIDE EFFECTS
  None

===========================================================================*/
/*lint -esym(715,ignored) -e762 */
extern void  ecall_ivs_task
( 
  dword ignored    
);
/*lint +esym(715,ignored) +e762 */

/*==========================================================================

  FUNCTION ECALL_GET_CMD_BUF

  DESCRIPTION
    Returns a command pointer off of the free queue, or NULL on failure.

  DEPENDENCIES
    command queue needs to be initalised

  RETURN VALUE
    Command pointer or NULL

  SIDE EFFECTS
    None

===========================================================================*/
ecall_cmd_type *ecall_get_cmd_buf
(
  void
);


/*==========================================================================

  FUNCTION ECALL_PUT_CMD

  DESCRIPTION
    Adds a command pointer to the command queue and signals the eCall task
    to process it.

  DEPENDENCIES
    command queue needs to be initalised

  RETURN VALUE
    None

  SIDE EFFECTS
    eCall task is signaled that there is a pending command

===========================================================================*/
void ecall_put_cmd
(
  ecall_cmd_type  *cmd_ptr
);

/*=========================================================================
              Internal functions for eCall modem clients 
===========================================================================*/
/*=========================================================================
FUNCTION     ECALL_EMODEM_CLIENTS_INITIALISE

DESCRIPTION
   initialise the queue for ecall_modem_clients

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ecall_emodem_clients_initialise
(
  void
);

/*=========================================================================
FUNCTION     ECALL_PROCESS_CLIENT_REGISTER

DESCRIPTION
    registers the client callback function to get ivs notifications
    overwrites if the client is already registered

DEPENDENCIES
    Need to process the ecall_emodem_client_init before calling register

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ecall_process_client_register
(
  ecall_client_type  *client_ptr
);


/*=========================================================================
FUNCTION    ECALL_PROCESS_CLIENT_NOTIFY

DESCRIPTION
  notifies all the registered clients with current ivs status

DEPENDENCIES
  None

RETURN VALUE
  tx_status and user_data for every registered client

SIDE EFFECTS
  None
===========================================================================*/
void ecall_process_client_notify
(
  ecall_cb_data_type ivs_status
);


/*=========================================================================
FUNCTION     ECALL_PROCESS_CLIENT_DEREGISTER

DESCRIPTION
     deregisters the client, cannot get any further notifications from ivs

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ecall_process_client_deregister
(
  int16  clientid
);

/*===========================================================================

  FUNCTION ECALL_IVS_SET_VOC_TIMER

  DESCRIPTION
    This function will set the vocoder timer with the input time duration to
    identify missed rx_pcm().
      
  DEPENDENCIES
    None.

  RETURN VALUE
    None.

  SIDE EFFECTS
    None

===========================================================================*/
void ecall_ivs_set_voc_timer
(
  uint32 duration
);

/*===========================================================================

  FUNCTION ECALL_IVS_CLR_VOC_TIMER

  DESCRIPTION
    This function will clear the vocoder timer.
      
  DEPENDENCIES
    None.

  RETURN VALUE
    None.

  SIDE EFFECTS
    None

===========================================================================*/
void ecall_ivs_clr_voc_timer
(
  void
);

/*==========================================================================

  FUNCTION    ECALL_IVS_TASK_INIT_BEFORE_TASK_START

  DESCRIPTION
     Initializes queues and defualt values for the global data. 

  DEPENDENCIES
    None.

  RETURN VALUE
  
  SIDE EFFECTS
    None

===========================================================================*/
void ecall_ivs_task_init_before_task_start
(
  void
);

/*==========================================================================

  FUNCTION    ECALL_IVS_TASK_INIT

  DESCRIPTION
     Initializes queues and defualt values for the global data.

  DEPENDENCIES
    None.

  RETURN VALUE
  
  SIDE EFFECTS
    None

===========================================================================*/
void ecall_ivs_task_init
(
  void
);



#else
 
  void ecall_ivs_task_dummy_func(void);

#endif   //end FEATURE_ECALL_IVS

#endif 

