#include "sm_prt_1x_cp.h"


/* Different timer ID's used by TM thread.
*/
#define TM_TIMER_ID_DOG           0x001
#define TM_TIMER_ID_DOG_COUNT     DOG_PDSMSCLIENT_RPT_TIME
#define ARIES_MSG_ID_SM_TM_TIMER  0x3432

/* For now assume only 1
*/
#define MAX_PRT_TYPE      1

/* pre-define the prt index.
*/
#define PRT_INDEX_CP_1X   0
#define PRT_INDEX_UP_1X   1

/* Union of all different types of prt types.
*/
typedef union {
 prt_cp_1x_s_type     cp_1x;
// prt_up_1x_s_type     up_1x;
}sm_prt_xx_info_u_type; 

/* Enum for different protocol entities.
*/
typedef enum {
  SM_PRT_CP_1X,
  SM_PRT_UP_1X,
  SM_PRT_CP_UMTS,
  SM_PRT_UP_UMTS,
  SM_PRT_UP_LDC
} sm_prt_e_type;

/* generic protocol layer state; need to add more state.
*/
typedef enum {
  SM_PRT_STATE_IDLE,
  SM_PRT_STATE_ACTIVE
} sm_prt_state_e_type;
/* Structure for protocol data common to all prt types
*/
typedef struct {
  uint32                        handle;
    /* Handle associated with this prt module */
  sm_prt_e_type                 prt_type;
    /* Type of this protocol sub-module */
  boolean                       enabled;
    /* Is this type enabled by user/NV/etc */
  boolean                       active;
    /* Is this type active or idle at this time*/
  sm_prt_state_e_type           state;
    /* State corsp to being active */
  uint32             lm_handle;
    /* LM handle assigned to this PRT layer */
  uint32           comm_handle;
    /* Comm handle used by the PRT type */
  sm_prt_xx_info_u_type         info;
    /* Information relevant to particular prt type */
} sm_prt_xx_s_type;

/* Structure to hold common coniguration for PRT module
*/
typedef struct {
  boolean            cp;
    /* Anything which enables/disables CP processing */
  boolean            up;
    /* Anything which enbles/disables UP processing */
} tm_prt_prt_config_s_type;


/*============================================================================*/
/*============================================================================*/




typedef struct {
  is801_dbm_msg_state_e_type  msg_state;  /* State of this messages */
  uint8                       *data_ptr;  /* data pointer */
  uint16                      size;       /* Size of data */
} is801_msg_s_type;

/* ==================================================================*/
/* Start TM defines. Called from PD API to TM module */
/* ==================================================================*/

typedef enum {
  TM_GENERAL_RESPONSE_ACK,
  TM_GENERAL_RESPONSE_NACK,
  TM_GENERAL_RESPONSE_WAIT
} tm_general_response_e_type;

/* Prt states
*/
typedef enum {
  TM_PRT_STATE_IDLE,
  TM_PRT_STATE_WAIT_NET_RESP,
  TM_PRT_STATE_ACTIVE
} tm_prt_state_e_type;
/* General Info for every prt type.
*/
typedef struct {
  uint32                  handle;
    /* Handle associated with this prt entry */
  uint32                  comm_handle;
    /* Command handle used this entity */
  tm_prt_e_type           type;
    /* Prt type */
  tm_prt_state_e_type     state;
    /* State corsp to this prt entity */
  sm_prt_xx_info_u_type   info;
    /* prt specific information stored */
  boolean                 active;
}tm_prt_info_s_type;

/* TM session related states.
*/
typedef enum {
  TM_SESSION_STATE_NONE,
  TM_SESSION_STATE_ACTIVE,
  TM_SESSION_STATE_WAIT_LM_START_RESP,
  TM_SESSION_STATE_WAIT_USER_INPUT
} tm_session_state_e_type;

/* Different data to hold..
*/
typedef struct {
  uint32        dummy_entry;
} tm_session_data_s_type;

typedef struct {
  tm_session_req_e_type     mask;
  tm_session_param_s_type   params;
    /* Session params */
  tm_session_data_s_type    data;
    /* Data on hold for this session */
} tm_session_info_s_type;

/* TM level state information.
*/
typedef enum {
  TM_STATE_IDLE,
  TM_STATE_ACTIVE
}tm_state_e_type;

/* Structure to hold TM layer information.
*/
typedef struct {
  tm_prt_prt_config_s_type            prt_config;
    /* CP/up enabled or not */
  uint32                              active_handle;
    /* Active prt handle at this time */
  tm_state_e_type                     state;
    /* general high level TM state */
  tm_e911_state_e_type                e911;
    /* geenal high level TM state */
  uint16                              num_prt;
    /* num of prt entities interfacing with TM at this time */
  tm_prt_info_s_type                  prt[MAX_PRT_TYPE];
    /* PRT configuration common to everything */
  tm_session_info_s_type              session_info;
    /* session info at TM level stored here */
  os_TimerMsgType                     *dog_timer;
} tm_prt_s_type;


extern boolean is801_incl_msto;

extern tm_prt_s_type   sm_tm;
/*==================================================*/
/* End Generic TM data structures                   */
/*==================================================*/


extern uint32  tm_session_continue(tm_session_start_continue_s_type *msg_ptr);

