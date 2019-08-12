#ifndef SECCRYPTI_H
#define SECCRYPTI_H

/*===========================================================================

                      S E C U R I T Y    S E R V I C E S
 
              C R Y P T O G R A P H I C    I N T E R F A C E

                  I N T E R N A L    H E A D E R    F I L E

DESCRIPTION
  This is the header file for the ARM-QDSP2 Cryptographic Interface. It
  defines the external interfaces for key exchange, message digest, and
  encrypt/decrypt for various algorithms. Those applications who want to
  use crypto services should include this header file.


EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

  seccrypt_init() has to be called before using any of the other routines.
  This is done at task startup automatically.

 Copyright (c) 2000-2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE
  $Header: //components/rel/core.mpss/3.5.c12.3/securemsm/crypto/environment/inc/seccrypti.h#1 $ 
  $DateTime: 2016/12/13 07:59:23 $ 
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/21/13   jk      Sec instance pool increase to 55 
07/18/10   nk      Shared Library Framework - 
                    Removed FEATURE_SEC_DSP_CRYPTO and DSP related structures
06/10/09   ejt     Removed FEATURE_SEC_IPC_SVC & CS_ISIGNAL
04/15/09   ejt     added msg flag param to seccrypti_free_instance()
12/05/08   ejt     updated WB cache macros
09/24/08   EJT     added control to only allow DEVKEY and MODEXP cmds to use DSP
09/17/08   EJT     replaced static allocation of seccrypt_instance_pool with dynamic allocation
11/12/07   df      add support for HW Crypto Engine
10/05/07   rv      undefine 'errno'
04/06/07   df      defer hw key ops till dsp is available
03/27/07   df      migrating malloc to UxMalloc
12/11/05   df      removed OEMHeap dependency for thinUI builds
09/30/05   df      added macro for wb cache (fix for 75801)
08/31/05   df      changed iovec filename to seciovec
                   Changes to support IOVEC
05/31/05   df      Featurized the non support of cache lines for 6100, since
                     T_M6100 is carried over to 6500, 6550. (CR 63119)
02/25/05   sb      Added support for IPC based SEC, Rsa signing, MSM device key
               based DSP commands
09/21/04   df      added support for dsp-arm failover:
                     added support for mmu_invalidate_data_cache_lines for
                     6500/6250 (6100 does not support it)
03/02/04   rwh     Converted boolean to cmd_src enum value.
12/16/03   yhw     Added AES instance debug info.
01/06/03   jay     Added new command definitions
10/21/03   jay     Added guard timer default value
06/04/03   om      Fixed mac_info for cipher params.
06/02/03   om      Added debug shortcuts to enc structures.
04/04/03   om      Created module.

===========================================================================*/


/*===========================================================================

                             INCLUDE FILES

===========================================================================*/

#include "core_variation.h"
#include "comdef.h"
#include "rex.h"
#include "dsm.h"
#include "UxCommon.h"
#include "secerrno.h"
#include "seciovec.h"

/* Forward type-declare the inidividual structures inside an instance */
typedef struct seccrypt_instance_s         seccrypt_instance_type;
typedef struct seccrypts_state_info_s      seccrypts_state_info_type;
typedef struct seccryptarm_common_data_s   seccryptarm_common_data_type;
typedef struct seccrypthw_common_data_s    seccrypthw_common_data_type;
typedef struct secpkx_instance_s           secpkx_instance_type;
typedef struct secenc_instance_s           secenc_instance_type;
typedef struct sechsh_instance_s           sechsh_instance_type;
typedef struct secpkxarm_instance_s        secpkxarm_instance_type;
typedef struct secencarm_instance_s        secencarm_instance_type;
typedef struct sechsharm_instance_s        sechsharm_instance_type;
typedef struct secpkxhw_instance_s         secpkxhw_instance_type;
typedef struct secenchw_instance_s         secenchw_instance_type;
typedef struct sechshhw_instance_s         sechshhw_instance_type;

/* Definition of a single command and command queue */
typedef enum
{
   /* The default in no command */
   SECCRYPT_NO_CMD = 0,

   SECCRYPT_NEW_CMD,
   SECCRYPT_DELETE_CMD,
   SECCRYPT_OP_COMPLETION_CMD,

   /* PKX related commands */
   SECCRYPT_PKX_MODEXP_CMD,
   SECCRYPT_PKX_CREATE_KEY_CMD,
   SECCRYPT_PKX_ENCRYPT_CMD,
   SECCRYPT_PKX_DECRYPT_CMD,
   SECCRYPT_PKX_SIGN_CMD,
   SECCRYPT_PKX_AUTHENTICATE_CMD,

   /* Ciphering commands */
   SECCRYPT_ENC_ENCRYPT_CMD,
   SECCRYPT_ENC_DECRYPT_CMD,

   /* Hashing commands */
   SECCRYPT_HSH_CREATE_DIGEST_CMD,
   SECCRYPT_HSH_CREATE_HMAC_CMD,

   /* Internal commands */
   SECCRYPT_INTERNAL_CMD_START,
   SECCRYPT_HSH_CREATE_HMAC_PREP_INNER_DATA_CMD = SECCRYPT_INTERNAL_CMD_START,
   SECCRYPT_HSH_CREATE_HMAC_INNER_HASH_1_CMD,
   SECCRYPT_HSH_CREATE_HMAC_INNER_HASH_LAST_CMD,
   SECCRYPT_HSH_CREATE_HMAC_PREP_OUTER_DATA_CMD,
   SECCRYPT_HSH_CREATE_HMAC_OUTER_HASH_1_CMD,
   SECCRYPT_HSH_CREATE_HMAC_OUTER_HASH_LAST_CMD,
   SECCRYPT_HSH_CREATE_HMAC_SNOW3G_CMD,
   SECCRYPT_HSH_CREATE_CMAC_AES_CMD,
   SECCRYPT_HSH_CREATE_HMAC_ZUC_CMD,
   SECCRYPT_HSH_CREATE_HMAC_KASUMI_CMD,
   SECCRYPT_MAX_CMD
} seccrypt_cmd_enum_type;

/* Label to distinguish internal command vs. external command */
typedef enum
{
   /* Internal commands */
   SECCRYPT_EXTERNAL_CMD = 0,
   SECCRYPT_INTERNAL_CMD = 1
} seccrypt_cmd_src_enum_type;

typedef struct seccrypt_cmd_s   seccrypt_cmd_type;
typedef seccrypt_cmd_type**     seccrypt_cmd_q_type;

/* Keep track of command, start, and finish time for logging */
typedef struct seccrypt_exec_time_s  seccrypt_exec_time_type;

/* Define the instance state information tables */
#include "seccrypts.h"

/* Now define all platform specific instance data */
#include "seccryptarm.h"
#include "seccrypthw.h"

/* Then define all algorithm-specific instances */
#include "secpkx.h"
#include "secenc.h"
#include "sechsh.h"


/*===========================================================================

                            TYPE DECLARATIONS

===========================================================================*/
/* map malloc to UxMalloc */
#define SEC_REALLOC UxRealloc
#define SEC_MALLOC(m) UxMalloc(m, UX_ALLOC_LOCAL)
#define SEC_FREE UxFree

/* SLF: ALIGNMENT on QDSP6 */
#ifdef FEATURE_QDSP6
#define ALIGN8 __attribute__ ((aligned(8)))
#else
#define ALIGN8 __align (8)
#endif

/* -------------------------------------------------------------------- */
/* Task running the security services command processing                */
/* -------------------------------------------------------------------- */
#define SECCRYPT_TCB_PTR             &sec_tcb

/* -------------------------------------------------------------------- */
/* Pool size of available crypto instances                              */
/* -------------------------------------------------------------------- */
#define SECCRYPT_INSTANCE_POOL_SIZE  55

/* -------------------------------------------------------------------- */
/* Pool size of available crypto commands                               */
/* -------------------------------------------------------------------- */
#define SECCRYPT_CMD_POOL_SIZE       55

/* -------------------------------------------------------------------- */
/* Maximum default execution time                                       */
/* -------------------------------------------------------------------- */
#define SECCRYPT_MAX_EXEC_TIME       2000

/* -------------------------------------------------------------------- */
/* Common Bit Mask def using MSM dev Key                                */
/*    for Secure HMAC, and Secure AES for SFS                           */
/* -------------------------------------------------------------------- */
#define SECCRYPT_DEVKEY_PARAM_MASK   0x00000008
#define SECCRYPT_HIPRI_PARAM_MASK    0x00000010
#define SECCRYPT_IS_DEVKEY_CMD \
        ( (instance_ptr->algorithm == SECAPI_AES128 ||  \
           instance_ptr->algorithm == SECAPI_SHA) &&    \
           seccrypti_msm_dev_key_supported &&           \
          (instance_ptr->common.set_params_mask & SECCRYPT_DEVKEY_PARAM_MASK)\
        )

/* -------------------------------------------------------------------- */
/* State definitions for a crypto instance                              */
/* -------------------------------------------------------------------- */
/* See state definition in seccrypts.h */

/* -------------------------------------------------------------------- */
/* Execution platform information for a crypto instance                 */
/* -------------------------------------------------------------------- */

/* -------------------------------------------------------------------- */
/* Execution status and metainfo used for asynchronous operations       */
/* -------------------------------------------------------------------- */
/* Info about calling back the application */
#ifdef errno
#undef errno
#endif /* errno */

typedef struct
{
   secerrno_enum_type  sec_errnum;
   rex_tcb_type*       caller_tcb_ptr;
   rex_sigs_type       caller_sig;
} seccrypt_exec_info_type;

/* Function to call on a specific instance upon completion */
/* of execution on a platform (algo-platform specific).    */
typedef void (*seccrypt_op_completion_cb_fptr_type)
(
seccrypt_instance_type*
);

/* -------------------------------------------------------------------- */
/* Union of all possible parameters of all commands to the crypto layer */
/* - this storage is used to transfer the API parameters across the SEC */
/* command and scheduler queues.                                        */
/* -------------------------------------------------------------------- */
typedef union
{
   /* --- New/Delete parameters ---------------------------------------- */
   struct 
   {
      secapi_handle_type*      handle_ptr;
      secapi_algo_enum_type    algorithm;
      seccrypt_exec_info_type  __exec_info;
   } new_handle;

   struct 
   {
      secapi_handle_type*      handle_ptr; 
      seccrypt_exec_info_type  __exec_info;
   } delete_handle;

   /* --- PKX parameters ----------------------------------------------- */
   struct
   {
      uint8*                         result_ptr;
      seccrypt_exec_info_type        __exec_info;
   } pkx_mod_exp;

   struct
   {
      secapi_pkx_key_enum_type     key;
      uint8*                       key_ptr;
      seccrypt_exec_info_type      __exec_info;
   } pkx_create_key;

   struct
   {
      secapi_pkx_padding_enum_type   padding_type;
      uint8*                         plaintext_ptr;
      uint16                         plaintext_len;
      uint8*                         ciphertext_ptr;
      uint16*                        ciphertext_len_ptr;
      seccrypt_exec_info_type        __exec_info;
   } pkx_encrypt;

   struct
   {
      secapi_pkx_padding_enum_type   padding_type;
      uint8*                         ciphertext_ptr;
      uint16                         ciphertext_len;
      uint8*                         plaintext_ptr;
      uint16*                        plaintext_len_ptr;
      seccrypt_exec_info_type        __exec_info;
   } pkx_decrypt;

   struct
   {
      secapi_pkx_padding_enum_type   padding_type;
      uint8*                         msg_ptr;
      uint16                         msg_len;
      uint8*                         signature_ptr;
      uint16*                        signature_len_ptr;
      uint8*                         iv_ptr;              /* Used to AES decrypt private key. */
      uint16                         iv_len;              /* Length of IV. */
      seccrypt_exec_info_type        __exec_info;
   } pkx_sign;

   struct
   {
      secapi_pkx_padding_enum_type   padding_type;
      uint8*                         signature_ptr;
      uint16                         signature_len;
      uint8*                         msg_ptr;
      uint16*                        msg_len_ptr;
      seccrypt_exec_info_type        __exec_info;
   } pkx_authenticate;

   /* --- Cipher parameters ------------------------------------------- */
   struct enc_encrypt_s
   {
      IOVEC_T*                     in_msg;
      IOVEC_T*                     out_msg;
      boolean                      consume_input;
      secapi_mac_pkt_info_type*    mac_pkt_info;
      seccrypt_exec_info_type      __exec_info;
   } enc_encrypt;

   struct
   {
      /* We cannot overlay en- and decryption as they might get */
      /* enqueued at the same time.                             */
      struct enc_encrypt_s     __reserved_s;

      /* Actual decryption parameters */
      IOVEC_T*                     in_msg;
      IOVEC_T*                     out_msg;
      boolean                      consume_input;
      secapi_mac_pkt_info_type*    mac_pkt_info;
      seccrypt_exec_info_type      __exec_info;
   } enc_decrypt;

   /* --- Hashing parameters ------------------------------------------ */
   struct hsh_create_digest_s
   {
      IOVEC_T*                 msg;
      uint8*                   msg_digest;
      seccrypt_exec_info_type  __exec_info;
   } hsh_create_digest;

   struct
   {
      /* We cannot overlay these as the hmac handle is used for */
      /* the individual hashing operations as well.             */
      struct hsh_create_digest_s    __reserved_s;

      /* Actual HMAC parameters */
      secapi_hsh_hmac_enum_type     hmac_type;
      secapi_algo_enum_type         hmac_algo;
      IOVEC_T*                      inner_msg;
      IOVEC_T*                      outer_msg;
      uint8*                        hmac;
      seccrypt_exec_info_type       __exec_info;
   } hsh_create_hmac;

} seccrypt_op_param_type;

/* Structure to keep trck of comand, start, and finish time for logging */
struct seccrypt_exec_time_s
{
   uint32  op_cmd_time;
   uint32  op_start_time;
   uint32  op_finish_time;
};

/* -------------------------------------------------------------------- */
/* Top-level struct for all instances                                   */
/* -------------------------------------------------------------------- */
struct seccrypt_instance_s
{
   /* The type and execution platform of the instance */
   secapi_algo_enum_type                algorithm;
   boolean                              sfs_request;

   /* Instance state information */
   seccrypts_state_info_type            state_info;

   /* The execution platform of the instance:               */
   /* platform - Platform the current operation executes on */
   /* previous_platform - Platform the last op completed on */
   /* preferred_platform - Platform selected by application */
   secapi_platform_enum_type            op_platform;
   secapi_platform_enum_type            previous_platform;
   secapi_platform_enum_type            preferred_platform;

   /* Parameters for the current operation requested */
   seccrypt_cmd_enum_type               op;
   seccrypt_op_param_type               op_params;
   seccrypt_op_completion_cb_fptr_type  op_complete_cb_fptr;

   /* All common data collected here */
   struct
   {
      /* Instance number */
      uint32  index;

      /* Keep track of mandatory parameters */
      uint32  set_params_mask;
      uint32  required_params_mask;

      /* Execution mode (sync. or async.) */
      secapi_exec_mode_enum_type  execution_mode;
      uint32                      execution_timeout_ms;

      /* Callback for asynchronous execution and event handling */
      secapi_async_exec_cb_param_data_type  async_exec_cb;

      /* funcptr for getting sensitive data */
      secapi_data_get_func_param_data_type  data_get;

      /* Pointer to the exec_info to notify the caller */
      seccrypt_exec_info_type*  __exec_info_ptr;

      /* Queue of pending commands for this instance */
      seccrypt_cmd_type*    __pending_cmd_q;
      seccrypt_cmd_q_type   pending_cmd_q;
      rex_sigs_type         pending_cmd_q_sig;

      /* For operations involving multiple instances */
      seccrypt_instance_type*  associated_instance_ptr;

   } common;


   /* Any data pertaining to control of an execution platform */
   union
   {
      seccryptarm_common_data_type  arm;
      seccrypthw_common_data_type   hw;
   } platform;

   /* Any data pertaining to a specific algorithm */
   union
   {
      secpkx_instance_type  pkx;
      secenc_instance_type  enc;
      sechsh_instance_type  hsh;
   } algo;
};

/* -------------------------------------------------------------------- */
/* Crypto command queue related types                                   */
/* -------------------------------------------------------------------- */

struct seccrypt_cmd_s
{
   seccrypt_cmd_enum_type   cmd;
   seccrypt_instance_type*  instance_ptr;
   seccrypt_op_param_type   params;

   boolean                  __used;
   seccrypt_cmd_type*       __previous_ptr;
   seccrypt_cmd_type*       __next_ptr;
};


/*===========================================================================

                            DATA DECLARATIONS

===========================================================================*/

/* Flag to indicate whether the crypto layer was initialized or not */
extern boolean seccrypt_initialized;

/* Pool of crypto instances ptrs and count */
extern seccrypt_instance_type  *seccrypt_instance_ptr_pool[SECCRYPT_INSTANCE_POOL_SIZE];
extern int                      seccrypt_instance_cnt;

/* Queue of commands for the crypto layer */
extern const seccrypt_cmd_q_type   seccrypt_cmd_q;

extern const seccrypt_cmd_q_type   seccrypt_pri_cmd_q;

extern boolean seccrypti_msm_dev_key_supported;


/*===========================================================================

                            MACRO DEFINITIONS

===========================================================================*/

/* Helper macros to set the parameters up for crypto operations */
/* NOTE: THESE SHOULD ONLY BE USED INTERNALLY TO THE CRYPTO-    */
/*       GRAPHIC SERVICES LIBRARY !                             */
#define SECCRYPTI_SET_HSH_CREATE_DIGEST_PARAMS( __i_ptr,                    \
                                                __m_ptr,                    \
                                                __md_ptr )                  \
        {                                                                   \
          (__i_ptr)->op_params.hsh_create_digest.msg =        __m_ptr;      \
          (__i_ptr)->op_params.hsh_create_digest.msg_digest = __md_ptr;     \
        }

/* Note the algorithm param is left out as it should be part of the instance 
   when created */
#define SECCRYPTI_SET_HSH_CREATE_HMAC_PARAMS( __i_ptr,                      \
                                              __h_type,                     \
                                              __in_m_ptr,                   \
                                              __out_m_ptr,                  \
                                              __hmac_ptr )                  \
   {                                                                        \
        (__i_ptr)->op_params.hsh_create_hmac.hmac_type      = __h_type;     \
        (__i_ptr)->op_params.hsh_create_hmac.inner_msg  = __in_m_ptr;   \
        (__i_ptr)->op_params.hsh_create_hmac.outer_msg  = __out_m_ptr;  \
        (__i_ptr)->op_params.hsh_create_hmac.hmac       = __hmac_ptr;   \
   }

/*===========================================================================

                          FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION SECCRYPTI_POWERUP_INIT()

DESCRIPTION
  This function does one-time initialization.

DEPENDENCIES
  None

RETURN VALUE
  Pointer to the new command structure.

SIDE EFFECTS
  All registered entries are cleared.
===========================================================================*/
void seccrypti_powerup_init( void );

/*===========================================================================

FUNCTION SECCRYPTI_PLATFORM_INIT()

DESCRIPTION
  This function initializes an instance for a specific platform.

DEPENDENCIES
  None

RETURN VALUE
  Pointer to the new command structure.

SIDE EFFECTS
  All registered entries are cleared.
===========================================================================*/
void seccrypti_platform_init
(
seccrypt_instance_type*    instance_ptr,
secapi_platform_enum_type  new_platform
);

/*===========================================================================

FUNCTION SECCRYPTI_SET_QUEUE_SIG()

DESCRIPTION
  This function sets the signal associated with the passed-in queue.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void seccrypti_set_queue_sig
(
const seccrypt_cmd_q_type*  q_ptr
);

/*===========================================================================

FUNCTION SECCRYPTI_NEW_CMD()

DESCRIPTION
  This function allocates a new crypto command.

DEPENDENCIES
  None

RETURN VALUE
  Pointer to the new command structure.

SIDE EFFECTS
  All registered entries are cleared.
===========================================================================*/
seccrypt_cmd_type*  seccrypti_new_cmd
(
seccrypt_cmd_enum_type   cmd,
seccrypt_instance_type*  instance_ptr,
seccrypt_op_param_type*  params_ptr
);

/*===========================================================================

FUNCTION SECCRYPTI_DELETE_CMD()

DESCRIPTION
  This function frees a crypto command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  All registered entries are cleared.
===========================================================================*/
void seccrypti_delete_cmd
(
seccrypt_cmd_type*  cmd_ptr
);

/*===========================================================================

FUNCTION SECCRYPTI_FREE_INSTANCE()

DESCRIPTION
  This function frees an instance. All intermediate data in the
  instance is cleared.

DEPENDENCIES
  None

RETURN VALUE
  Security services error codes.

SIDE EFFECTS
  None
===========================================================================*/
secerrno_enum_type seccrypti_free_instance
(
seccrypt_instance_type*  instance_ptr,
boolean                  msg_flag
);

/*===========================================================================

FUNCTION SECCRYPTI_ENQUEUE_CMD()

DESCRIPTION
  This function enqueues a command to the passed-in command queue
  associating the instance and parameters with it.

  NOTE: This function uses DSM items to store the command data.
        Small DSM items could be too small ;-), which would require
        a dedicated pool of command structures - TBD.

DEPENDENCIES
  None

RETURN VALUE
  Security services error codes.

SIDE EFFECTS
  None
===========================================================================*/
secerrno_enum_type seccrypti_enqueue_cmd
(
const seccrypt_cmd_q_type*  cmd_q,
seccrypt_cmd_enum_type      cmd,
seccrypt_instance_type*     instance_ptr,
seccrypt_op_param_type*     params_ptr,
seccrypt_cmd_src_enum_type  cmd_src
);

/*===========================================================================

FUNCTION SECCRYPTI_DEQUEUE_CMD()

DESCRIPTION
  This function dequeues a command from the passed-in command queue.

DEPENDENCIES
  None

RETURN VALUE
  Pointer to the next command, NULL if queue is empty.

SIDE EFFECTS
  None
===========================================================================*/
seccrypt_cmd_type* seccrypti_dequeue_cmd
(
const seccrypt_cmd_q_type*  cmd_q
);

/*===========================================================================

FUNCTION SECCRYPTI_PROCESS_CMD_SIG()

DESCRIPTION
  This function processes the comannd queue related signal of the
  security services task (SEC).

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void seccrypti_process_cmd_sig( void );

void seccrypti_process_pri_cmd_sig( void );


/*===========================================================================

FUNCTION SECCRYPTI_PROCESS_CMD()

DESCRIPTION
  This function processes entries from the passed-in command queue.

DEPENDENCIES
  None

RETURN VALUE
  Security services error codes.

SIDE EFFECTS
  None
===========================================================================*/
secerrno_enum_type seccrypti_process_cmd
(
seccrypt_cmd_type*  cmd_ptr
);


secerrno_enum_type seccrypti_process_pri_cmd
(
seccrypt_cmd_type*  cmd_ptr
);

/*===========================================================================

FUNCTION SECCRYPTI_ENQUEUE_OP_COMPLETION_CMD()

DESCRIPTION
  This function is used by the platform-specific code to notify
  the SEC task about completion of an operation.
  It is called from ISR context of the SECCRYPTARM task.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void seccrypti_enqueue_op_completion_cmd
(
seccrypt_instance_type*  instance_ptr
);

/*===========================================================================

FUNCTION SECCRYPTI_FINISH_OP()

DESCRIPTION
  This function does common operation completion work, i.e. unblock
  the caller respectively call the app's async callback.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  All registered entries are cleared.
===========================================================================*/
void seccrypti_finish_op
(
seccrypt_instance_type*  instance_ptr
);

/*===========================================================================

FUNCTION SECCRYPTI_WAIT_FUNCTION_REGISTRY_CLEAR()

DESCRIPTION
  This function initializes the SEC wait function registry.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  All registered entries are cleared.
===========================================================================*/
void seccrypti_wait_function_registry_clear( void );

/*===========================================================================

FUNCTION SECCRYPTI_REGISTER_WAIT_FUNC()

DESCRIPTION
  This function allows an application to register wait functions
  for all TCB's it might be calling the crypto API from.
  A registration of a NULL wait function pointer will clear the
  entry for this task, causing rex_wait() to be used subsequently.

DEPENDENCIES
  Only one wait function can be registered per TCB.

RETURN VALUE
  Status of request:
    E_SUCCESS       if successful
    E_ALREADY_DONE  if a wait function is already registered for this TCB
    E_INVALID_ARG   if called w/ a NULL fptr and this TCB is not registered
    E_NOT_ALLOWED   if the registry is full

SIDE EFFECTS
  None.
===========================================================================*/
secerrno_enum_type seccrypti_register_wait_func
(
rex_tcb_type*  tcb_ptr,
rex_sigs_type  (*wait_fptr)( rex_sigs_type )
);

/*===========================================================================

FUNCTION SECCRYPTI_CMD_WAIT()

DESCRIPTION
  Use this function to allow padding a watchdog of the caller's task
  while blocking her/him for synchronous operations.

DEPENDENCIES
  None

RETURN VALUE
  Rex signals returned from caller's wait routine.

SIDE EFFECTS
  None.
===========================================================================*/
rex_sigs_type seccrypti_cmd_wait
(
rex_tcb_type*  tcb_ptr,
rex_sigs_type  sigs
);

/*===========================================================================

FUNCTION SECCRYPTI_SET_COMMON_PARAM()

DESCRIPTION
  This function is used by the algorithm-specific set_param() functions
  to set the instance-common parameters.

DEPENDENCIES
  None

RETURN VALUE
  Security services error codes.

SIDE EFFECTS
  None
===========================================================================*/
secerrno_enum_type seccrypti_set_common_param
(
seccrypt_instance_type*   instance_ptr,
secapi_param_enum_type    param,
secapi_param_type*        param_data_ptr
);

/*===========================================================================

FUNCTION SECCRYPTI_CHECK_PARAMS()

DESCRIPTION
  This function is the exit function of the INST_INITIALIZED state.

DEPENDENCIES
  None

RETURN VALUE
  Security services error codes.

SIDE EFFECTS
  None
===========================================================================*/
secerrno_enum_type seccrypti_check_params
(
seccrypt_instance_type*             instance_ptr,
seccrypts_instance_state_enum_type  old_state,
seccrypts_instance_state_enum_type  new_state
);

#endif /* SECCRYPTI_H */
