/*=============================================================================

             O N - B O A R D   D I A G N O S T I C S   A P P

GENERAL DESCRIPTION
  Implements OBDAPP logic to verify the secure sim

Copyright (c) 2012 - 2014 byQUALCOMM Technologies, Inc(QTI).
All Rights Reserved.

=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/obdapp/src/obdapp.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
01/10/14   df      Use v2.0 diag macros
11/07/13   df      Fix LLVM compiler warnings
10/04/13   sw      Reduced F3 messages
06/27/13   kb      Added OBD app malloc and free wrapper functions
05/06/13   sw      Changes made for secure version of memcpy
01/21/13   at      Fix to perform AUTH upon card power down & up or hotswap.
12/20/12   at      Cleanup before initial checkin.
11/01/12   ab      File created.

=============================================================================*/


/*=============================================================================

                           INCLUDE FILES

=============================================================================*/

#include "comdef.h"
#include "uim_msg.h"
#include "mmgsdilib.h"
#include "mmgsdilib_common.h"
#include "mmgsdisessionlib.h"
#include "secapi.h"
#include "AKAMilenage.h"
#include "mmgsdi.h"
#include "diag_dci_auth.h"
#include "ssm_key_handler.h"
#include "rex.h"
#include "obdapp_p.h"
#include <stringl/stringl.h>
#include "modem_mem.h"



/*=============================================================================
  Defines
=============================================================================*/
#define OBD_REF                   0xABABABAB
#define OBD_READ_IMSI             0xBCBCBCBC

/* IMSI Path & length */
#define OBD_IMSI_PATH_LEN         3
#define OBD_IMSI_PATH_BUF_1       0x3F00
#define OBD_IMSI_PATH_BUF_2       0x7FFF
#define OBD_IMSI_PATH_BUF_3       0x6F07
#define OBD_IMSI_PATH_BUF_4       0xFFFF
#define OBD_IMSI_PATH_BUF_5       0xFFFF

/* Key related defines */
#define OBD_SRES_LEN              4
#define OBD_SRES_INDEX            0
#define OBD_Kc_LEN                8
#define OBD_Kc_INDEX              5
#define OBD_MAX_IMSI_LEN_BYTES    8
#define OBD_KEY_PREAMBLE          0xABCDDCBA


/*=============================================================================
  Data types
=============================================================================*/

/* State table of OBDAPP */
typedef enum obd_state_q
{
  OBD_STATE_INIT             = 0xABCDABCD,
  OBD_STATE_MMGSDI_REGISTER,
  OBD_STATE_MMGSDI_REG_DONE,
  OBD_STATE_READY,
  OBD_STATE_IMSI_READ,
  OBD_STATE_AUTH_OBTAINED,
  OBD_STATE_AUTHENTICATED,
  OBD_STATE_ERR,
  OBD_STATE_UNKNOWN
} obd_state_t;

/* Error values */
typedef enum obd_err_type_q
{
  OBD_SUCCESS                = 0,
  OBD_FAIL,
  OBD_DIAG_CLIENT_REG_FAIL,
  OBD_NO_KEYS_IN_FS,
  OBD_CLIENT_ID_FAIL,
  OBD_AID_NOT_AVAILABLE,
  OBD_NO_CLIENT_ID,
  OBD_NO_SESSION_ID,
  OBD_PIN_FAIL,
  OBD_IMSI_FAIL,
  OBD_AUTH_FAIL,
  OBD_CARD_ERROR
} obd_err_type;

/* OBDAPP context structure */
typedef struct
{
  obd_state_t                state;
  mmgsdi_client_id_type      mmgsdi_client_id;
  mmgsdi_session_id_type     session_id;
  boolean                    session_id_allocated;
  boolean                    obd_aid_detected;
  mmgsdi_events_enum_type    mmgsdi_last_evt;
  mmgsdi_slot_id_enum_type   slot;
  uint8                      imsi[OBD_MAX_IMSI_LEN_BYTES];
  uint8                      K[MILENAGE_OP_LEN];
  uint8                      RAND[MILENAGE_OP_LEN];
  uint8                      SRES_uicc[4];
  uint8                      Kc_uicc[8];
  uint8                      SRES_obdapp[4];
  uint8                      Kc_obdapp[8];
  boolean                    uicc_diag_authenticated;
  obd_err_type               err_val;
} obd_type;

/* Keys provisioned should resemble below structure */
typedef struct
{
  uint32                     preamble;
  uint8                      OP_len;
  uint8                      OP[MILENAGE_OP_LEN];
  uint8                      Ks_len;
  uint8                      Ks[MILENAGE_OP_LEN];
  uint8                      AID_len;
  uint8                      AID[MMGSDI_MAX_AID_LEN];
} obd_keys_type;


/*=============================================================================
  Global Variables
=============================================================================*/
/* Global structure that has details of obdapp_ctxt */
static obd_type           obdapp_ctxt;
rex_tcb_type            * obd_task_ptr                  = NULL;
boolean                   obd_keys_present              = FALSE;
static mmgsdi_len_type    obd_aid_len                   = MILENAGE_OP_LEN;
static uint8              obd_aid_data[MILENAGE_OP_LEN] =
                           {0xA0, 0x00, 0x00, 0x00, 0x87, 0x10, 0x02, 0xf3,
                            0x10, 0xff, 0xff, 0x89, 0x08, 0x00, 0x00, 0xFF};


/*=============================================================================
  Function prototypes
=============================================================================*/
static void obd_mmgsdi_events_cb(const mmgsdi_event_data_type *evt_ptr);
static void obd_mmgsdi_cmd_cb(mmgsdi_return_enum_type  status,
                              mmgsdi_cnf_enum_type     cnf,
                              const mmgsdi_cnf_type  * cnf_ptr);
static void obd_open_non_prov_sess(void);
static void obd_close_session(void);
static void * obd_malloc(uint32 size);
static void obd_free(void * ptr);

/*=============================================================================
  Function Definitions
=============================================================================*/

/*===========================================================================

FUNCTION OBD_MALLOC

DESCRIPTION
  obd_malloc calls modem_mem_calloc to request memory from the Modem Heap.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
static void * obd_malloc(uint32 size)
{
  return modem_mem_calloc(1, (size), MODEM_MEM_CLIENT_UIM);
}/* obd_malloc */


/*===========================================================================

FUNCTION OBD_FREE

DESCRIPTION
  The obd_free frees the pointer from the Modem Heap.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void obd_free(void * ptr)
{
  modem_mem_free((void*)(ptr), MODEM_MEM_CLIENT_UIM);
}/* obd_free */


/*=============================================================================

FUNCTION OBD_LOOKUP_STATE_NAME

DESCRIPTION
  Maps the enum values to the respective string to help in logging.

DEPENDENCIES
  None

RETURN VALUE
  Pointer to the string value of the input enum value upon SUCCESS, else NULL.

SIDE EFFECTS
  None

=============================================================================*/
static char * obd_lookup_state_name
(
  obd_state_t   state
)
{
  switch(state)
  {
    case OBD_STATE_INIT:
      return "OBD_STATE_INIT";

    case OBD_STATE_MMGSDI_REGISTER:
      return "OBD_STATE_MMGSDI_REGISTER";

    case OBD_STATE_MMGSDI_REG_DONE:
      return "OBD_STATE_MMGSDI_REG_DONE";

    case OBD_STATE_READY:
      return "OBD_STATE_READY";

    case OBD_STATE_IMSI_READ:
      return "OBD_STATE_IMSI_READ";

    case OBD_STATE_AUTH_OBTAINED:
      return "OBD_STATE_AUTH_OBTAINED";

    case OBD_STATE_AUTHENTICATED:
      return "OBD_STATE_AUTHENTICATED";

    case OBD_STATE_ERR:
      return "OBD_STATE_ERR";

    case OBD_STATE_UNKNOWN:
      return "OBD_STATE_UNKNOWN";

    default:
      return NULL;
  }
  return NULL;
} /* obd_lookup_state_name */


/*=============================================================================

FUNCTION OBD_CHANGE_STATE

DESCRIPTION
  Checks if OBD is in given state and if Yes will move to Next State

DEPENDENCIES
  None

RETURN VALUE
  Boolean: OBD_SUCCESS: Moved to next state successfully
           OBD_FAIL: If changing state is failed

SIDE EFFECTS
  None

=============================================================================*/
static obd_err_type obd_change_state
(
  obd_type    * obd_ptr,
  obd_state_t   curr_state,
  obd_state_t   next_state
)
{
  char * curr_state_str = NULL;
  char * next_state_str = NULL;

  if(obd_ptr == NULL)
  {
    UIM_MSG_ERR_0("OBD: NULL input obd_ptr !");
    return OBD_FAIL;
  }

  if(curr_state != OBD_STATE_UNKNOWN && obd_ptr->state != curr_state)
  {
    UIM_MSG_ERR_2("OBD: Mismatch in obd states: obd->state: 0x%x, curr_state: 0x %x",
                  obd_ptr->state, curr_state);
    return OBD_FAIL;
  }

  if(curr_state == next_state)
  {
    UIM_MSG_ERR_1("OBD: No state change needed: curr_state: 0x %x",
                  curr_state);
    return OBD_SUCCESS;
  }

  curr_state_str = obd_lookup_state_name(obd_ptr->state);
  next_state_str = obd_lookup_state_name(next_state);

  MSG_SPRINTF_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "OBD: State change transition from %s to %s",
                 (curr_state_str == NULL)? "NULL" : curr_state_str,
                 (next_state_str == NULL)? "NULL" : next_state_str);

  /* Update state */
  obd_ptr->state = next_state;
  return OBD_SUCCESS;
} /* obd_change_state */


/*=============================================================================

FUNCTION OBD_GET_KEYS_FROM_SSM

DESCRIPTION
  Will call SSM Key Handler to obtain keys from file system

DEPENDENCIES
  None

RETURN VALUE
  Boolean: OBD_SUCCESS: if keys are obtained successfully
           OBD_FAIL: In other cases

SIDE EFFECTS
  This function gets keys as a sync operation from file system,
  therefore suggested to do from own task context and out of intlocks

=============================================================================*/
static obd_err_type obd_get_keys_from_ssm
(
  obd_keys_type *keys_ptr
)
{
  /* Key handler request structure */
  ssm_key_handler_struct_t  key_request;
  ssm_key_handler_err_t     status;
  uint16                    key_len = 0;

  if(keys_ptr == NULL)
  {
    UIM_MSG_ERR_0("OBD: obtained input as NULL");
    return OBD_FAIL;
  }

  /* Update SSM Key Handler Key structure to get a key */
  key_request.req              = SSM_KEY_HANDLER_GET_KEY;
  key_request.cb               = NULL;
  key_request.info.usage       = SSM_KEY_HANDLER_USAGE_OBD_MILENAGE;
  key_request.info.source      = SSM_KEY_HANDLER_SRC_QCOM ;
  key_request.info.crypto_algo = SSM_KEY_HANDLER_CRYPTO_ALGO_MILENAGE_KS_OP_AID ;
  key_request.info.len_key     = 0;
  key_request.key              = NULL;

  status = ssm_key_handler_get_key( &key_request );
  if ( E_SSM_KEY_HANDLER_SUCCESS != status || key_request.key == NULL)
  {
    UIM_MSG_ERR_1("OBD: ssm_key_handler_get_key returned err: 0x%x", status);
    return OBD_FAIL;
  }

  /* Key is obtained in key_request.key and length is derived by key_request.info.len_key */
  key_len = key_request.info.len_key;

  /* Copy the keys after a check */
  if ((key_len == 0) ||
      (key_len > sizeof(obd_keys_type)))
  {
    UIM_MSG_ERR_1("OBD: Inalid key length returned, key_len: 0x%x", key_len);
    return OBD_FAIL;
  }

  memset(keys_ptr, 0, sizeof(obd_keys_type));
  memscpy(keys_ptr, sizeof(obd_keys_type), key_request.key, key_len);

  /* ssm_key_handler_get_key() api expects client to free key memory, securely
     clear the keys from stack too */
  (void)secure_memset(key_request.key, 0, key_len);

  /* Use native free as memory was not allocated by MODEM_MEM_CLIENT_UIM */
  free(key_request.key);

  /* Check if Preamble is as expected */
  if(keys_ptr->preamble != OBD_KEY_PREAMBLE)
  {
    UIM_MSG_ERR_1("OBD: Preamble (0x%x) doesnt match with expected",
                  keys_ptr->preamble);
    return OBD_FAIL;
  }

  if(keys_ptr->AID_len > MMGSDI_MAX_AID_LEN)
  {
    UIM_MSG_ERR_1("OBD: AID Len should be less than %d bytes.. Keys are not as expected",
                  MMGSDI_MAX_AID_LEN);
    return OBD_FAIL;
  }

  return OBD_SUCCESS;
} /* obd_get_keys_from_ssm */


/*=============================================================================

FUNCTION OBD_GENERATEK

DESCRIPTION
  To generate K out of IMSI and OP

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None

=============================================================================*/
static void obd_generateK
(
  void
)
{
  int             i;
  uint8           obd_randstar[MILENAGE_OP_LEN];
  uint8           res[MILENAGE_OP_LEN];
  uint8           ck[MILENAGE_OP_LEN];
  uint8           ik[MILENAGE_OP_LEN];
  uint8           ak[MILENAGE_OP_LEN];
  obd_keys_type   obd_keys;

  if(!obdapp_ctxt.obd_aid_detected)
  {
    obdapp_ctxt.err_val = OBD_AID_NOT_AVAILABLE;
    (void)obd_change_state(&obdapp_ctxt, OBD_STATE_UNKNOWN, OBD_STATE_ERR);
    (void)rex_set_sigs(obd_task_ptr, MMGSDI_OBDAPP_SIG);
    UIM_MSG_ERR_0("OBD: AID seems to be not valid by this time");
    return;
  }

  /* Obtain keys from SSM Key Handler */
  if(OBD_SUCCESS != obd_get_keys_from_ssm(&obd_keys))
  {
    obdapp_ctxt.err_val = OBD_NO_KEYS_IN_FS;
    (void)obd_change_state(&obdapp_ctxt, OBD_STATE_UNKNOWN, OBD_STATE_ERR);
    (void)rex_set_sigs(obd_task_ptr, MMGSDI_OBDAPP_SIG);
    UIM_MSG_ERR_0("OBD: No Keys in FS to call Milenage APIs");
    return;
  }

  /*  Concatenate IMSI to itself to get 16 bytes RANDstar   */
  memscpy(&(obd_randstar[0]), sizeof(obd_randstar),
          obdapp_ctxt.imsi, OBD_MAX_IMSI_LEN_BYTES);

  memscpy(&(obd_randstar[OBD_MAX_IMSI_LEN_BYTES]),
          (sizeof(obd_randstar) - OBD_MAX_IMSI_LEN_BYTES),
          obdapp_ctxt.imsi, OBD_MAX_IMSI_LEN_BYTES);

  /* Call Milenage APIs with OP, Ks, Randstar for f3 (ck), f4 (ik) outputs */
  AKA_milenage_f2345(obd_keys.Ks, obd_randstar, res, ck, ik, ak, obd_keys.OP, FALSE);

  /* Generate K from ck, ik.
     K is the xor of milenage f3, f4 outputs */
  for(i=0; i<MILENAGE_OP_LEN; i++)
  {
    obdapp_ctxt.K[i] = ck[i] ^ ik[i];
  }

  /* Clear the keys from stack too before exiting */
  (void)secure_memset(&obd_keys, 0, sizeof(obd_keys_type));
  (void)secure_memset(res,       0, sizeof(res));
  (void)secure_memset(ck,        0, sizeof(ck));
  (void)secure_memset(ik,        0, sizeof(ik));
  (void)secure_memset(ak,        0, sizeof(ak));

  return;
} /* obd_generateK */


/*=============================================================================

FUNCTION OBD_VERIFY_AUTH

DESCRIPTION
  Verify the AUTH obtained from UICC card with the generated SRES, Kc values

DEPENDENCIES
  None

RETURN VALUE
  TRUE - Indicates Verification is successful
  FALSE - Indicates Verification failed

SIDE EFFECTS
  None

=============================================================================*/
static boolean obd_verify_AUTH
(
  void
)
{
  uint16         i    = 0;
  boolean        ret  = TRUE;
  uint8          res[MILENAGE_OP_LEN];
  uint8          ck[MILENAGE_OP_LEN];
  uint8          ik[MILENAGE_OP_LEN];
  uint8          ak[MILENAGE_OP_LEN];
  obd_keys_type  obd_keys;

  if(!obdapp_ctxt.obd_aid_detected)
  {
    UIM_MSG_ERR_1("OBD: no valid AID by this time.. state: 0x%x",
                  obdapp_ctxt.state);
    obdapp_ctxt.err_val = OBD_AID_NOT_AVAILABLE;
    (void)obd_change_state(&obdapp_ctxt, OBD_STATE_UNKNOWN, OBD_STATE_ERR);
    (void)rex_set_sigs(obd_task_ptr, MMGSDI_OBDAPP_SIG);
    return FALSE;
  }

  /* Obtain keys from SSM Key Handler */
  if(OBD_SUCCESS != obd_get_keys_from_ssm(&obd_keys))
  {
    obdapp_ctxt.err_val = OBD_NO_KEYS_IN_FS;
    (void)obd_change_state(&obdapp_ctxt, OBD_STATE_UNKNOWN, OBD_STATE_ERR);
    (void)rex_set_sigs(obd_task_ptr, MMGSDI_OBDAPP_SIG);
    UIM_MSG_ERR_0("OBD: No Keys in FS to call Milenage APIs");
    return FALSE;
  }

  /* Call Milenage APIs with OP, K, RAND for f2, f3, f4 outputs */
  AKA_milenage_f2345(obdapp_ctxt.K, obdapp_ctxt.RAND, res, ck, ik, ak, obd_keys.OP, TRUE);

  /* Clear the keys */
  (void)secure_memset(&obd_keys, 0, sizeof(obd_keys_type));

  /* Calculate SRES */
  for(i=0; i<OBD_SRES_LEN; i++)
  {
    /* Note: XRES is expected to be 16 bytes.. but as res will be only 8 bytes, other 8 bytes should be zeros
       using zeros xoring will anyway give same values.. So, XORing only with 8 bytes of RES */
    obdapp_ctxt.SRES_obdapp[i] = res[i] ^ res[i + OBD_SRES_LEN];
  }

  /* Verify SRES with SRES of UICC */
  if(memcmp(obdapp_ctxt.SRES_obdapp, obdapp_ctxt.SRES_uicc, OBD_SRES_LEN) != 0)
  {
    UIM_MSG_ERR_0("OBD: SRES doesnt match");
    obdapp_ctxt.err_val = OBD_AUTH_FAIL;
    (void)obd_change_state(&obdapp_ctxt, OBD_STATE_UNKNOWN, OBD_STATE_ERR);
    ret = FALSE;
    goto verify_end;
  }

  /* Calculate Kc */
  for(i=0; i<OBD_Kc_LEN; i++)
  {
    obdapp_ctxt.Kc_obdapp[i] = (ck[i] ^ ck[i + OBD_Kc_LEN] ^ ik[i] ^ ik[i + OBD_Kc_LEN]);
    UIM_MSG_HIGH_3("OBD: Kc[%d]: UICC 0x%x OBDAPP 0x%x",
                   i, obdapp_ctxt.Kc_uicc[i], obdapp_ctxt.Kc_obdapp[i]);
  }

  /* Verify Kc with Kc of UICC */
  if(memcmp(obdapp_ctxt.Kc_obdapp, obdapp_ctxt.Kc_uicc, OBD_Kc_LEN) != 0)
  {
    UIM_MSG_ERR_0("OBD: Kc doesnt match");
    obdapp_ctxt.err_val = OBD_AUTH_FAIL;
    (void)obd_change_state(&obdapp_ctxt, OBD_STATE_UNKNOWN, OBD_STATE_ERR);
    ret = FALSE;
    goto verify_end;
  }

  /* Authentication successful.. Indicate Clients */
  (void)obd_change_state(&obdapp_ctxt, OBD_STATE_AUTH_OBTAINED, OBD_STATE_AUTHENTICATED);
  obdapp_ctxt.uicc_diag_authenticated = TRUE;
  obdapp_ctxt.err_val = OBD_SUCCESS;

verify_end:
  (void)rex_set_sigs(obd_task_ptr, MMGSDI_OBDAPP_SIG);
  /* Securely clear keys on stack variables before exit */
  (void)secure_memset(res, 0, sizeof(res));
  (void)secure_memset(ck,  0, sizeof(ck));
  (void)secure_memset(ik,  0, sizeof(ik));
  (void)secure_memset(ak,  0, sizeof(ak));

  (void)secure_memset(obdapp_ctxt.K,           0, sizeof(obdapp_ctxt.K));
  (void)secure_memset(obdapp_ctxt.SRES_uicc,   0, sizeof(obdapp_ctxt.SRES_uicc));
  (void)secure_memset(obdapp_ctxt.Kc_uicc,     0, sizeof(obdapp_ctxt.Kc_uicc));
  (void)secure_memset(obdapp_ctxt.SRES_obdapp, 0, sizeof(obdapp_ctxt.SRES_obdapp));
  (void)secure_memset(obdapp_ctxt.Kc_obdapp,   0, sizeof(obdapp_ctxt.Kc_obdapp));
  return ret;

} /* obd_verify_AUTH */


/*=============================================================================

FUNCTION OBD_RETRIEVE_IMSI

DESCRIPTION
   Request for IMSI using mmgsdi_session_read_transparent() which
   will give response in READ_CNF.. If IMSI read is successful, will
   change the obdapp_ctxt state to OBD_STATE_IMSI_READ

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None

=============================================================================*/
static void obd_retrieve_imsi
(
  void
)
{
  mmgsdi_session_id_type  mmgsdi_session_id;
  mmgsdi_return_enum_type mmgsdi_status;
  mmgsdi_access_type      imsi_file;

  if ((obdapp_ctxt.obd_aid_detected) &&
      (obdapp_ctxt.session_id_allocated))
  {
    mmgsdi_session_id = obdapp_ctxt.session_id;
  }
  else
  {
     UIM_MSG_ERR_0("OBD: no valid AID to retrieve imsi"  );
     obdapp_ctxt.err_val = OBD_AID_NOT_AVAILABLE;
     (void)obd_change_state(&obdapp_ctxt, OBD_STATE_UNKNOWN, OBD_STATE_ERR);
     (void)rex_set_sigs(obd_task_ptr, MMGSDI_OBDAPP_SIG);
     return;
  }

  /* Fill in the path for IMSI Elementary File */
  imsi_file.access_method              = MMGSDI_BY_PATH_ACCESS;
  imsi_file.file.path_type.path_len    = OBD_IMSI_PATH_LEN;
  imsi_file.file.path_type.path_buf[0] = OBD_IMSI_PATH_BUF_1;
  imsi_file.file.path_type.path_buf[1] = OBD_IMSI_PATH_BUF_2;
  imsi_file.file.path_type.path_buf[2] = OBD_IMSI_PATH_BUF_3;
  imsi_file.file.path_type.path_buf[3] = OBD_IMSI_PATH_BUF_4;
  imsi_file.file.path_type.path_buf[4] = OBD_IMSI_PATH_BUF_5;

  /* Response will come in MMGSDI_READ_CNF.. Read IMSI EF */
  mmgsdi_status = mmgsdi_session_read_transparent(
                             mmgsdi_session_id,
                             imsi_file,
                             0,
                             OBD_MAX_IMSI_LEN_BYTES + 1,
                             obd_mmgsdi_cmd_cb,
                             OBD_READ_IMSI);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_1("OBD: mmgsdi_session_read_transparent() failed with status = 0x%x",
                  mmgsdi_status);
    obdapp_ctxt.err_val = OBD_IMSI_FAIL;
    (void)obd_change_state(&obdapp_ctxt, OBD_STATE_UNKNOWN, OBD_STATE_ERR);
    (void)rex_set_sigs(obd_task_ptr, MMGSDI_OBDAPP_SIG);
    return;
  }

  return;
}  /* obd_retrieve_imsi */


/*=============================================================================

FUNCTION OBD_IS_OBDAPP_AID

DESCRIPTION
   Checks with OBDAPP AID Data with the AID given as input.. If both matches
   then this function will return TRUE else FALSE

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None

=============================================================================*/
static boolean obd_is_obdapp_aid
(
  mmgsdi_aid_type  * x
)
{
  if(!x)
  {
    return FALSE;
  }

  /* Print AID Label and Details */
  /*MSG_SPRINTF_3(MSG_SSID_DFLT, MSG_LEGACY_HIGH, "OBD: app label: %s, app_type: %d data_len: %d",
                                                                   (char *) &(x->label.data_ptr[0]),
                                                                              x->app_type,
                                                                              x->aid.data_len);*/
  UIM_MSG_HIGH_3("OBD: app label: %s, app_type: %d data_len: %d",
                 (char*)&(x->label.data_ptr[0]),
                 x->app_type,
                 x->aid.data_len);

  if(x->aid.data_len != obd_aid_len)
  {
    UIM_MSG_ERR_3("OBD: aid len %d doesnt match with expected %d, app_type: %d",
                  x->aid.data_len, obd_aid_len, x->app_type);
    return FALSE;
  }

  /* Compare input AID with OBDAPP AID data */
  if(memcmp(x->aid.data_ptr, obd_aid_data, obd_aid_len) != 0)
  {
    UIM_MSG_ERR_0("OBD: aid data doesnt match with expected");
    return FALSE;
  }

  return TRUE;
} /* obd_is_obdapp_aid */


/*=============================================================================

FUNCTION OBD_CLOSE_ALL

DESCRIPTION
   Deregisters with the MMGSDI and resets all variables required.. Will get
   called in error conditions

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None

=============================================================================*/
static void obd_close_all
(
  void
)
{
  mmgsdi_client_id_type  mmgsdi_client_id = 0;

  UIM_MSG_ERR_1("OBD: obd_close_all, error: 0x%x", obdapp_ctxt.err_val);

  /* Save the client id since we dont deregister with MMGSDI */
  mmgsdi_client_id = obdapp_ctxt.mmgsdi_client_id;

  /* Indicate Diag that OBD sim got error */
  diag_dci_auth_notify(DIAG_DCI_AUTH_CLIENT_UIM, DIAG_OBD_AUTHENTICATION_NOT_SUCCESSFUL);

  /* Close session, in case there was an obd state error before USIM Auth response */
  obd_close_session();

  /* Securely clear the global since it might have sensitive data */
  (void)secure_memset(&obdapp_ctxt, 0, sizeof(obd_type));

  /* Reset the global variable */
  obdapp_ctxt.obd_aid_detected        = FALSE;
  obdapp_ctxt.uicc_diag_authenticated = FALSE;
  obdapp_ctxt.slot                    = MMGSDI_MAX_SLOT_ID_ENUM;
  obdapp_ctxt.session_id              = (mmgsdi_session_id_type)0;
  obdapp_ctxt.session_id_allocated    = FALSE;
  obdapp_ctxt.state                   = OBD_STATE_ERR;

  /* We need to keep the client id back for us to re-open the session,
     for example in case of card power down / up, hotswap etc. */
  obdapp_ctxt.mmgsdi_client_id = mmgsdi_client_id;

  /* Now we can move the state back to registered */
  (void)obd_change_state(&obdapp_ctxt,
                         OBD_STATE_ERR,
                         OBD_STATE_MMGSDI_REG_DONE);

  return;
} /* obd_close_all */


/*===========================================================================
FUNCTION OBD_MMGSDI_EVENTS_CB

DESCRIPTION
  OBDAPP Callback for events from MMGSDI

PARAMETER
  evt_ptr [Input]: the MMGSDI event received

DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECTS
  None

COMMENTS
  None
===========================================================================*/
static void obd_mmgsdi_events_cb
(
  const mmgsdi_event_data_type *evt_ptr
)
{
  uint32 j = 0;

  UIM_MSG_HIGH_1("OBD: Received MMGSDI Event: 0x%x", evt_ptr->evt);

  switch (evt_ptr->evt)
  {
    case MMGSDI_CARD_INSERTED_EVT:

    obdapp_ctxt.mmgsdi_last_evt = MMGSDI_CARD_INSERTED_EVT;

    /* Only one secure sim is expected. Therefore, if already detected, ignore next sim */
    if(obdapp_ctxt.obd_aid_detected == FALSE)
    {
      /* Loop with all AIDs available on UICC card */
      UIM_MSG_HIGH_2("OBD: Received MMGSDI_CARD_INSERTED_EVT slot = 0x%x, num of avail_aids = %d",
                     evt_ptr->data.card_inserted.slot,
                     evt_ptr->data.card_inserted.num_aids_avail);

      for(j=0; j < evt_ptr->data.card_inserted.num_aids_avail; j++)
      {
        mmgsdi_aid_type *temp_aid;

        temp_aid = (mmgsdi_aid_type *) &(evt_ptr->data.card_inserted.aid_info[j]);

        /* Verify if AID data is matching with OBDAPP AID data */
        UIM_MSG_HIGH_1("OBD: Checking aid[%d]", j);
        if(obd_is_obdapp_aid(temp_aid))
        {
          /* Found matching AID */
          obdapp_ctxt.slot = evt_ptr->data.card_inserted.slot;
          obdapp_ctxt.obd_aid_detected = TRUE;
          UIM_MSG_HIGH_0("OBD: Found matching app label");

          break;
        }
      }

      if(obdapp_ctxt.obd_aid_detected)
      {
        obd_open_non_prov_sess();
      }
      else
      {
        UIM_MSG_ERR_1("OBD: OBDAPP AID is not found in slot: %d",
                      evt_ptr->data.card_inserted.slot);
      }
    }
    else
    {
      UIM_MSG_HIGH_0("OBD: Already OBDAPP UICC got detected ");
    }
    break;

  /* Session Changed Event */
  case MMGSDI_SESSION_CHANGED_EVT:
    if ((obdapp_ctxt.session_id == evt_ptr->session_id) &&
        (obdapp_ctxt.slot       == evt_ptr->data.session_changed.app_info.slot))
    {
       obdapp_ctxt.mmgsdi_last_evt = MMGSDI_SESSION_CHANGED_EVT;
       UIM_MSG_HIGH_3("OBD: Received MMGSDI_SESSION_CHANGED_EVT activated = 0x%x, session = 0x%x%x",
                      evt_ptr->data.session_changed.activated,
                      evt_ptr->session_id>>32,
                      (0xFFFFFFFF & evt_ptr->session_id));
    }
    break;

  /* Subscription Event.. For Pure Non-Prov AIDs we may not get this */
  case MMGSDI_SUBSCRIPTION_READY_EVT:
    if ((obdapp_ctxt.session_id == evt_ptr->session_id) &&
        (obdapp_ctxt.slot       == evt_ptr->data.session_changed.app_info.slot))
    {
       obdapp_ctxt.mmgsdi_last_evt = MMGSDI_SUBSCRIPTION_READY_EVT;

       UIM_MSG_HIGH_3("OBD: Received MMGSDI_SUBSCRIPTION_READY_EVT slot = 0x%x, session = 0x%x%x",
                      evt_ptr->data.subscription_ready.app_info.slot,
                      obdapp_ctxt.session_id>>32,
                      (0xFFFFFFFF & obdapp_ctxt.session_id));
    }
    break;

  /* PIN1_EVT: We will receive on verification of PIN */
  case MMGSDI_PIN1_EVT:
  {
    mmgsdi_pin_enum_type pin;
    mmgsdi_pin_status_enum_type pin_status;

    obdapp_ctxt.mmgsdi_last_evt = MMGSDI_PIN1_EVT;

    UIM_MSG_HIGH_2("OBD: Received PIN1_EVT for session: 0x%x%x",
                   (evt_ptr->session_id>>32),
                   (evt_ptr->session_id & 0xFFFFFFFF));
    pin = evt_ptr->data.pin.pin_info.pin_id;
    pin_status = evt_ptr->data.pin.pin_info.status;

    /* Check if PIN is enabled-verified/disabled */
    if(pin == MMGSDI_PIN1 && (pin_status == MMGSDI_PIN_ENABLED_VERIFIED
                           || pin_status == MMGSDI_PIN_DISABLED
                           || pin_status == MMGSDI_PIN_UNBLOCKED))
    {
      UIM_MSG_HIGH_3("OBD: Pin %d status is good: 0x%x, good to go.. retries on this PIN: %d",
                     pin, pin_status, evt_ptr->data.pin.pin_info.num_retries);

      if(OBD_SUCCESS == obd_change_state(&obdapp_ctxt, OBD_STATE_MMGSDI_REG_DONE, OBD_STATE_READY))
      {
        (void)rex_set_sigs(obd_task_ptr, MMGSDI_OBDAPP_SIG);
      }
      else
      {
        UIM_MSG_HIGH_1("OBD: Reached PIN1_EVT in wrong state 0x%x",
                       obdapp_ctxt.state);
      }
    }
    else if(pin == MMGSDI_PIN1 && pin_status == MMGSDI_PIN_ENABLED_NOT_VERIFIED)
    {
      /* PIN not verified Case */
      UIM_MSG_HIGH_0("OBD: Pin state is yet to be verified.. Need to wait");
    }
    else
    {
      /* Ignoring all other cases too */
      UIM_MSG_ERR_1("OBD: PIN1_EVT different status.. Wait for valid Status",
                    pin_status);
    }
  }
  break;

  case MMGSDI_REFRESH_EVT:
    obdapp_ctxt.mmgsdi_last_evt = MMGSDI_REFRESH_EVT;
    UIM_MSG_HIGH_1("OBD: Received MMGSDI_REFRESH_EVT on slot: %d",
                   evt_ptr->data.refresh.slot);
  break;

  /* Card Error event.. Close All and indicate OBDAPP clients about error */
  case MMGSDI_CARD_ERROR_EVT:
    if(obdapp_ctxt.slot == evt_ptr->data.card_error.slot)
    {
      obdapp_ctxt.mmgsdi_last_evt  = MMGSDI_CARD_ERROR_EVT;
      obdapp_ctxt.obd_aid_detected = FALSE;
      obdapp_ctxt.err_val          = OBD_CARD_ERROR;
      (void)obd_change_state(&obdapp_ctxt, OBD_STATE_UNKNOWN, OBD_STATE_ERR);
      (void)rex_set_sigs(obd_task_ptr, MMGSDI_OBDAPP_SIG);
      UIM_MSG_ERR_1("OBD: Received MMGSDI_CARD_ERROR_EVT on slot: %d",
                    evt_ptr->data.card_error.slot);
    }
  break;

  /* Card Removed event.. Close All and indicate OBDAPP clients about error */
  case MMGSDI_CARD_REMOVED_EVT:
    if(obdapp_ctxt.slot == evt_ptr->data.card_removed.slot)
    {
      obdapp_ctxt.mmgsdi_last_evt  = MMGSDI_CARD_REMOVED_EVT;
      obdapp_ctxt.obd_aid_detected = FALSE;
      obdapp_ctxt.err_val          = OBD_CARD_ERROR;
      (void)obd_change_state(&obdapp_ctxt, OBD_STATE_UNKNOWN, OBD_STATE_ERR);
      (void)rex_set_sigs(obd_task_ptr, MMGSDI_OBDAPP_SIG);

      UIM_MSG_ERR_1("OBD: Received MMGSDI_CARD_REMOVED_EVT on slot: %d",
                    evt_ptr->data.card_removed.slot);
    }
  break;

  /* Session closed due to some error.. Close All and indicate OBDAPP clients about error */
  case MMGSDI_SESSION_CLOSE_EVT:
    if(obdapp_ctxt.slot == evt_ptr->data.session_close.slot)
    {
      obdapp_ctxt.mmgsdi_last_evt = MMGSDI_SESSION_CLOSE_EVT;
      obdapp_ctxt.obd_aid_detected = FALSE;
      obdapp_ctxt.err_val = OBD_CARD_ERROR;
      (void)obd_change_state(&obdapp_ctxt, OBD_STATE_UNKNOWN, OBD_STATE_ERR);
      (void)rex_set_sigs(obd_task_ptr, MMGSDI_OBDAPP_SIG);
      UIM_MSG_ERR_1("OBD: Received MMGSDI_SESSION_CLOSE_EVT on slot: %d",
                    evt_ptr->data.session_close.slot);
    }
  break;

  default:
    break;
  }
} /* obd_mmgsdi_events_cb*/


/*===========================================================================
FUNCTION OBD_MMGSDI_CMD_CB

DESCRIPTION
  OBDAPP Callback for Client Id Reg and Refresh from MMGSDI

PARAMETER
  status  [Input]: status of command returned from MMGSDI
  cnf     [Input]: confirmation type
  cnf_ptr [Input]: confirmation pointer

DEPENDENCIES
  None

RETURN VALUE
  void

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
static void obd_mmgsdi_cmd_cb
(
  mmgsdi_return_enum_type     status,
  mmgsdi_cnf_enum_type        cnf,
  const mmgsdi_cnf_type     * cnf_ptr
)
{
  mmgsdi_return_enum_type    mmgsdi_status  = MMGSDI_SUCCESS;

  if(cnf_ptr == NULL)
  {
    UIM_MSG_ERR_0("cnf_ptr is NULL");
    return;
  }

  switch(cnf)
  {
    case MMGSDI_CLIENT_ID_AND_EVT_REG_CNF:
      if(status != MMGSDI_SUCCESS)
      {
        obdapp_ctxt.err_val = OBD_CLIENT_ID_FAIL;
        (void)obd_change_state(&obdapp_ctxt, OBD_STATE_UNKNOWN, OBD_STATE_ERR);
        (void)rex_set_sigs(obd_task_ptr, MMGSDI_OBDAPP_SIG);
        UIM_MSG_ERR_1("OBD: MMGSDI_CLIENT_ID_AND_EVT_REG_CNF returned error: %x",
                      status);
        return;
      }

      /* Save Client ID */
      obdapp_ctxt.mmgsdi_client_id = cnf_ptr->client_id_and_evt_reg_cnf.response_header.client_id;
      if(OBD_SUCCESS == obd_change_state(&obdapp_ctxt, OBD_STATE_MMGSDI_REGISTER, OBD_STATE_MMGSDI_REG_DONE))
      {
        UIM_MSG_HIGH_3("OBD: MMGSDI_CLIENT_ID_AND_EVT_REG_CNF client_id: 0x%x%x, client_data: 0x%x",
                       (cnf_ptr->client_id_and_evt_reg_cnf.response_header.client_id>>32),
                       cnf_ptr->client_id_and_evt_reg_cnf.response_header.client_id,
                       cnf_ptr->client_id_and_evt_reg_cnf.response_header.client_data);
      }
      break;

    case MMGSDI_SESSION_OPEN_CNF:
      UIM_MSG_HIGH_3("OBD: MMGSDI_SESSION_OPEN_CNF status 0x%x, session_id 0x%x, client_data: 0x%x",
                     status,
                     cnf_ptr->session_open_cnf.session_id,
                     cnf_ptr->session_open_cnf.response_header.client_data);

      if (status != MMGSDI_SUCCESS)
      {
        obdapp_ctxt.err_val = OBD_FAIL;
        (void)obd_change_state(&obdapp_ctxt, OBD_STATE_UNKNOWN, OBD_STATE_ERR);
        (void)rex_set_sigs(obd_task_ptr, MMGSDI_OBDAPP_SIG);
        UIM_MSG_ERR_2("OBD: MMGSDI_SESSION_OPEN_CNF returned error status: 0x%x, client_data: %x",
                      status,
                      cnf_ptr->session_open_cnf.response_header.client_data);
        return;
      }

      /* Non-Provision Session opening.. OBDAPP is Non-Prov type.. So this is only imp for us */
      if(MMGSDI_NON_PROV_SESSION_SLOT_1 == cnf_ptr->session_open_cnf.response_header.client_data)
      {
        obdapp_ctxt.session_id = cnf_ptr->session_open_cnf.session_id;
        obdapp_ctxt.session_id_allocated = TRUE;

        /* Call mmgsdi_session_get_all_pin_status() to know pin status.
           Non-prov sessions will become READY immediately after open_cnf */
        UIM_MSG_HIGH_0("OBD: Calling mmgsdi_session_get_all_pin_status from OPEN_CNF");
        mmgsdi_status = mmgsdi_session_get_all_pin_status(obdapp_ctxt.session_id, obd_mmgsdi_cmd_cb, OBD_REF);
        if(mmgsdi_status != MMGSDI_SUCCESS)
        {
          UIM_MSG_ERR_1("OBD: mmgsdi_session_get_all_pin_status returned error: 0x%x",
                        mmgsdi_status);
          obdapp_ctxt.err_val = OBD_PIN_FAIL;
          (void)obd_change_state(&obdapp_ctxt, OBD_STATE_UNKNOWN, OBD_STATE_ERR);
          (void)rex_set_sigs(obd_task_ptr, MMGSDI_OBDAPP_SIG);
          return;
        }
      }
      /* Provision Session opening */
      else if(MMGSDI_GW_PROV_PRI_SESSION == cnf_ptr->session_open_cnf.response_header.client_data)
      {
        /* As OBDAPP is a non-prov type, we should not get here */
        UIM_MSG_HIGH_0("OBD: SESSION_OPEN_CNF for MMGSDI_GW_PROV_PRI_SESSION, Ignoring ... ");
      }
      else
      {
        UIM_MSG_HIGH_1("OBD: SESSION_OPEN_CNF wrong client_data: %x, which is not expected ",
                       cnf_ptr->session_open_cnf.response_header.client_data);
        obdapp_ctxt.err_val = OBD_FAIL;
        (void)obd_change_state(&obdapp_ctxt, OBD_STATE_UNKNOWN, OBD_STATE_ERR);
        (void)rex_set_sigs(obd_task_ptr, MMGSDI_OBDAPP_SIG);
      }
      break;

    case MMGSDI_REFRESH_CNF:
      UIM_MSG_HIGH_0("OBD: received MMGSDI_REFRESH_CNF");
      break;

    case MMGSDI_READ_CNF:
      UIM_MSG_HIGH_2("OBD: READ_CNF status: 0x%x, file_enum: 0x%x",
                     status, cnf_ptr->read_cnf.access.file.file_enum);

      /* Check if IMSI file details match to this CNF */
      if (status == MMGSDI_SUCCESS &&
          cnf_ptr->read_cnf.access.access_method == MMGSDI_BY_PATH_ACCESS &&
          cnf_ptr->read_cnf.access.file.path_type.path_len == 0x3 &&
          cnf_ptr->read_cnf.access.file.path_type.path_buf[0] == OBD_IMSI_PATH_BUF_1 &&
          cnf_ptr->read_cnf.access.file.path_type.path_buf[1] == OBD_IMSI_PATH_BUF_2 &&
          cnf_ptr->read_cnf.access.file.path_type.path_buf[2] == OBD_IMSI_PATH_BUF_3)
      {
        uint8 imsi_len = 0;

        if(!cnf_ptr->read_cnf.read_data.data_ptr)
        {
          UIM_MSG_ERR_0("OBD: READ_CNF returned NULL pointer");
          obdapp_ctxt.err_val = OBD_IMSI_FAIL;
          (void)obd_change_state(&obdapp_ctxt, OBD_STATE_UNKNOWN, OBD_STATE_ERR);
          (void)rex_set_sigs(obd_task_ptr, MMGSDI_OBDAPP_SIG);
          return;
        }

        imsi_len = cnf_ptr->read_cnf.read_data.data_ptr[0];

        UIM_MSG_HIGH_3("OBD: IMSI data_len = %d, imsi_len = %d, max len = %d",
                       cnf_ptr->read_cnf.read_data.data_len,
                       imsi_len,
                       OBD_MAX_IMSI_LEN_BYTES);

        if(imsi_len > sizeof(obdapp_ctxt.imsi))
        {
          UIM_MSG_ERR_1("OBD: Ivalid IMSI length returned: 0x%x", imsi_len);
          obdapp_ctxt.err_val = OBD_IMSI_FAIL;
          (void)obd_change_state(&obdapp_ctxt, OBD_STATE_UNKNOWN, OBD_STATE_ERR);
          (void)rex_set_sigs(obd_task_ptr, MMGSDI_OBDAPP_SIG);
          return;
        }

        /* Copy the bytes of read file except first length byte as it is to obdapp_ctxt.imsi */
        memset(obdapp_ctxt.imsi, 0, sizeof(obdapp_ctxt.imsi));
        memscpy(obdapp_ctxt.imsi, sizeof(obdapp_ctxt.imsi), &(cnf_ptr->read_cnf.read_data.data_ptr[1]), imsi_len);

        /* Indicate that IMSI reading is successful */
        if(OBD_SUCCESS == obd_change_state(&obdapp_ctxt, OBD_STATE_READY, OBD_STATE_IMSI_READ))
        {
          (void)rex_set_sigs(obd_task_ptr, MMGSDI_OBDAPP_SIG);
        }
        else
        {
          UIM_MSG_ERR_1("OBD: Got to READ_CNF in wrong state",
                        obdapp_ctxt.state);
        }
      }
      else
      {
        UIM_MSG_ERR_2("OBD: in READ_CNF, err data-status for client_data: 0x%x, status: 0x%x",
                      cnf_ptr->read_cnf.response_header.client_data, status);
        if(cnf_ptr->read_cnf.response_header.client_data == OBD_READ_IMSI)
        {
          obdapp_ctxt.err_val = OBD_IMSI_FAIL;
          (void)obd_change_state(&obdapp_ctxt, OBD_STATE_UNKNOWN, OBD_STATE_ERR);
          (void)rex_set_sigs(obd_task_ptr, MMGSDI_OBDAPP_SIG);
        }
      }
      break;

    case MMGSDI_GET_ALL_PIN_STATUS_CNF:

      UIM_MSG_HIGH_1("OBD: MMGSDI_GET_ALL_PIN_STATUS_CNF status: 0x%x",
                     cnf_ptr->get_all_pin_status_cnf.pin1.status);

      if(status != MMGSDI_SUCCESS)
      {
        obdapp_ctxt.err_val = OBD_PIN_FAIL;
        (void)obd_change_state(&obdapp_ctxt, OBD_STATE_UNKNOWN, OBD_STATE_ERR);
        (void)rex_set_sigs(obd_task_ptr, MMGSDI_OBDAPP_SIG);
        return;
      }

      /* Check for Verified-Enabled or Disabled */
      switch(cnf_ptr->get_all_pin_status_cnf.pin1.status)
      {
         case MMGSDI_PIN_DISABLED:
         case MMGSDI_PIN_ENABLED_VERIFIED:
         case MMGSDI_PIN_UNBLOCKED:

           UIM_MSG_HIGH_0("OBD: PIN is Disabled or Enabled Verified, Can go ahead");
           if(OBD_SUCCESS == obd_change_state(&obdapp_ctxt, OBD_STATE_MMGSDI_REG_DONE, OBD_STATE_READY))
           {
             (void)rex_set_sigs(obd_task_ptr, MMGSDI_OBDAPP_SIG);
           }
           else
           {
             UIM_MSG_ERR_1("OBD: Got to ALL_PIN_STATUS_CNF with wrong state: 0x%x",
                           obdapp_ctxt.state);
           }
           break;

         case MMGSDI_PIN_ENABLED_NOT_VERIFIED:
           UIM_MSG_HIGH_0("OBD: PIN is not yet verified.. Wait for MMGSDI_PIN1_EVT");
           break;

         default:
           UIM_MSG_ERR_1("Unhandled PIN STATUS status: 0x%x",
                         cnf_ptr->get_all_pin_status_cnf.pin1.status);
           break;
      }
      break;

    /* USIM Authentication Response comes here */
    case MMGSDI_USIM_AUTH_CNF:
    {
      uint32 i = 0;
      uint8  max_auth_len = OBD_Kc_LEN + OBD_Kc_INDEX + 1;

      UIM_MSG_HIGH_2("OBD: MMGSDI_USIM_AUTH_CNF data_len: %d, status: 0x%x",
                     cnf_ptr->usim_auth_cnf.auth_response.data_len, status);

      if (status != MMGSDI_SUCCESS ||
          cnf_ptr->usim_auth_cnf.response_header.client_data != OBD_REF)
      {
        UIM_MSG_ERR_3("OBD: MMGSDI_USIM_AUTH_CNF err: 0x%x, and client_data 0x%x doesnt match expected: 0x%x",
                      status,
                      cnf_ptr->usim_auth_cnf.response_header.client_data,
                      OBD_REF);
        obdapp_ctxt.err_val = OBD_AUTH_FAIL;
        (void)obd_change_state(&obdapp_ctxt, OBD_STATE_UNKNOWN, OBD_STATE_ERR);
        (void)rex_set_sigs(obd_task_ptr, MMGSDI_OBDAPP_SIG);
        return;
      }

      /* Check pointer & sizes before dereferencing */
      if ((cnf_ptr->usim_auth_cnf.auth_response.data_ptr == NULL) ||
          (cnf_ptr->usim_auth_cnf.auth_response.data_len <  max_auth_len))
      {
        UIM_MSG_HIGH_2("OBD: Invalid pointer or length in response, data_ptr: 0x%x, data_len: 0x%x",
                       cnf_ptr->usim_auth_cnf.auth_response.data_ptr,
                       cnf_ptr->usim_auth_cnf.auth_response.data_len );
        obdapp_ctxt.err_val = OBD_AUTH_FAIL;
        (void)obd_change_state(&obdapp_ctxt, OBD_STATE_UNKNOWN, OBD_STATE_ERR);
        (void)rex_set_sigs(obd_task_ptr, MMGSDI_OBDAPP_SIG);
        return;
      }

      /* Verify lengths of response data is as expected */
      if(cnf_ptr->usim_auth_cnf.auth_response.data_ptr[OBD_SRES_INDEX] != OBD_SRES_LEN ||
         cnf_ptr->usim_auth_cnf.auth_response.data_ptr[OBD_Kc_INDEX]   != OBD_Kc_LEN)
      {
        UIM_MSG_HIGH_2("OBD: XRES len %d and Kc len %d are wrong respectively",
                       cnf_ptr->usim_auth_cnf.auth_response.data_ptr[OBD_SRES_INDEX],
                       cnf_ptr->usim_auth_cnf.auth_response.data_ptr[OBD_Kc_INDEX]);
        obdapp_ctxt.err_val = OBD_AUTH_FAIL;
        (void)obd_change_state(&obdapp_ctxt, OBD_STATE_UNKNOWN, OBD_STATE_ERR);
        (void)rex_set_sigs(obd_task_ptr, MMGSDI_OBDAPP_SIG);
        return;
      }

      /* Copy SRES of UICC */
      for(i=0; i<OBD_SRES_LEN; i++)
      {
        obdapp_ctxt.SRES_uicc[i] = cnf_ptr->usim_auth_cnf.auth_response.data_ptr[i + OBD_SRES_INDEX + 1];
      }

      /* Copy Kc of UICC */
      for(i=0; i<OBD_Kc_LEN; i++)
      {
        obdapp_ctxt.Kc_uicc[i] = cnf_ptr->usim_auth_cnf.auth_response.data_ptr[i + OBD_Kc_INDEX + 1];
      }

      if(OBD_SUCCESS == obd_change_state(&obdapp_ctxt, OBD_STATE_IMSI_READ, OBD_STATE_AUTH_OBTAINED))
      {
        (void)rex_set_sigs(obd_task_ptr, MMGSDI_OBDAPP_SIG);
      }
      else
      {
        UIM_MSG_ERR_1("OBD: Got to MMGSDI_USIM_AUTH_CNF with wrong state: 0x%x",
                      obdapp_ctxt.state);
      }
    }
    break;

    default:
      UIM_MSG_HIGH_1("OBD: Unhandled mmgsdi cnf 0x%x", cnf);
      break;
  }
} /* obd_mmgsdi_cmd_cb */


/*===========================================================================
FUNCTION OBD_OPEN_NON_PROV_SESS

DESCRIPTION
  Calls mmgsdi api to open Non-Prov session

PARAMETER


DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECTS
  None

COMMENTS
  None
===========================================================================*/
static void obd_open_non_prov_sess
(
  void
)
{
  mmgsdi_return_enum_type         mmgsdi_status;
  mmgsdi_non_prov_app_info_type   mmgsdi_np_app;
  mmgsdi_session_type_enum_type   sess_type = MMGSDI_NON_PROV_SESSION_SLOT_1;

  if(obdapp_ctxt.mmgsdi_client_id == 0)
  {
    UIM_MSG_ERR_0("OBD: No valid client id.. cannot open non prov session");
    obdapp_ctxt.err_val = OBD_NO_CLIENT_ID;
    (void)obd_change_state(&obdapp_ctxt, OBD_STATE_UNKNOWN, OBD_STATE_ERR);
    (void)rex_set_sigs(obd_task_ptr, MMGSDI_OBDAPP_SIG);
    return;
  }

  /* This function will get called only if OBDAPP AID gets detected in INSERTED_EVT */
  mmgsdi_np_app.app_id_data.data_len = obd_aid_len;
  mmgsdi_np_app.app_id_data.data_ptr = obd_aid_data;
  mmgsdi_np_app.exclusive_channel = FALSE;

  if(obdapp_ctxt.slot == MMGSDI_SLOT_1)
  {
    sess_type = MMGSDI_NON_PROV_SESSION_SLOT_1;
  }
  else
  {
    sess_type = MMGSDI_NON_PROV_SESSION_SLOT_2;
  }

  UIM_MSG_HIGH_0("OBD: Calling mmgsdi_session_open() for NON_PROV..");

  /* Open session to the OBD App on the card */
  mmgsdi_status = mmgsdi_session_open(
                             obdapp_ctxt.mmgsdi_client_id,
                             sess_type,
                             &mmgsdi_np_app,
                             obd_mmgsdi_events_cb,
                             FALSE,
                             obd_mmgsdi_cmd_cb,
                             sess_type);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    obdapp_ctxt.err_val = OBD_FAIL;
    (void)obd_change_state(&obdapp_ctxt, OBD_STATE_UNKNOWN, OBD_STATE_ERR);
    (void)rex_set_sigs(obd_task_ptr, MMGSDI_OBDAPP_SIG);
    UIM_MSG_ERR_1("OBD: mmgsdi_session_open() failed with status = 0x%x for non-prov session opening",
                  mmgsdi_status);
  }
  return;
} /* obd_open_non_prov_sess */


/*===========================================================================
FUNCTION OBD_USIM_GSM_AUTHENTICATE

DESCRIPTION
  Send USIM GSM AUTHENTICATE command with Random Number to current session

PARAMETER

DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECTS
  None

COMMENTS
  None
===========================================================================*/
static boolean obd_usim_gsm_authenticate
(
  void
)
{
  mmgsdi_return_enum_type           mmgsdi_status;
  mmgsdi_data_type                  rand_data;
  mmgsdi_session_id_type            sess_id;
  mmgsdi_session_info_query_type    session_info_query;
  mmgsdi_session_get_info_type      session_get_info_ptr;

  UIM_MSG_HIGH_0("OBD: in obd_usim_gsm_authenticate");

  if(obdapp_ctxt.session_id_allocated)
  {
    sess_id = obdapp_ctxt.session_id;
  }
  else
  {
    sess_id = 0;
  }

  if(sess_id)
  {
    /* Get App State.. only for debug purpose */
    {
      memset( (void*)&session_info_query, 0, sizeof(mmgsdi_session_info_query_type));
      session_info_query.query_type           = MMGSDI_SESSION_INFO_QUERY_BY_ID;
      session_info_query.query_key.session_id = sess_id;
      mmgsdi_status = mmgsdi_session_get_info_sync(session_info_query, &session_get_info_ptr);
      if (MMGSDI_SUCCESS != mmgsdi_status)
      {
        UIM_MSG_ERR_1("OBD: mmgsdi_session_get_info_sync returned error: 0x%x",
                      mmgsdi_status);
      }
      else
      {
        UIM_MSG_HIGH_1("OBD: app_state: 0x%x", session_get_info_ptr.app_state);
      }
    }

    /* Send AUTHENTICATE Command.
       For AUTHENTICATE in GSM Context we need to send only RAND, (Rand_len, RAND) as data
       For AUTHENTICATE in 3G Context, we need to send RAND + AUTN, (Rand_len, RAND, AUTN_Len, AUTN) */
    rand_data.data_len = MILENAGE_OP_LEN + 1;
    rand_data.data_ptr = (uint8 *)obd_malloc(rand_data.data_len);
    if(!rand_data.data_ptr)
    {
      UIM_MSG_ERR_1("OBD: couldnt allocate memory of %d bytes, cant call authenticate",
                    rand_data.data_len);
      obdapp_ctxt.err_val = OBD_FAIL;
      (void)obd_change_state(&obdapp_ctxt, OBD_STATE_UNKNOWN, OBD_STATE_ERR);
      (void)rex_set_sigs(obd_task_ptr, MMGSDI_OBDAPP_SIG);
      return FALSE;
    }

    /* Generate Random Number to obdapp_ctxt.RAND */

    if(E_SUCCESS != secapi_get_random(SECAPI_SECURE_RANDOM,
                                      (uint8*)obdapp_ctxt.RAND,
                                      MILENAGE_OP_LEN))
    {
      UIM_MSG_ERR_0("OBD: couldnt get random number, cant call authenticate");
      obdapp_ctxt.err_val = OBD_FAIL;
      (void)obd_change_state(&obdapp_ctxt, OBD_STATE_UNKNOWN, OBD_STATE_ERR);
      (void)rex_set_sigs(obd_task_ptr, MMGSDI_OBDAPP_SIG);
      return FALSE;
    }

    /* Copy Random Number */
    rand_data.data_ptr[0] = MILENAGE_OP_LEN;
    memscpy(&(rand_data.data_ptr[1]), (rand_data.data_len - 1),
            obdapp_ctxt.RAND, MILENAGE_OP_LEN);

    UIM_MSG_HIGH_3("OBD: calling mmgsdi_session_uicc_authenticate with rand length: %d, sess_id: 0x%x%x",
                   rand_data.data_len, sess_id>>32, (sess_id & 0xFFFFFFFF));

    /* Response comes in MMGSDI_USIM_AUTH_CNF */
    mmgsdi_status = mmgsdi_session_uicc_authenticate(
                                        sess_id,
                                        MMGSDI_AUTN_USIM_GSM_CONTEXT,
                                        rand_data,
                                        obd_mmgsdi_cmd_cb,
                                        OBD_REF);
    /* Free the allocated memory */
    obd_free(rand_data.data_ptr);
    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      UIM_MSG_ERR_1("OBD: mmgsdi_session_uicc_authenticate returned error: 0x%x",
                    mmgsdi_status);
      obdapp_ctxt.err_val = OBD_AUTH_FAIL;
      (void)obd_change_state(&obdapp_ctxt, OBD_STATE_UNKNOWN, OBD_STATE_ERR);
      (void)rex_set_sigs(obd_task_ptr, MMGSDI_OBDAPP_SIG);
      return FALSE;
    }
  }
  else
  {
    UIM_MSG_ERR_0("OBD: No valid session for sending authenticate");
    obdapp_ctxt.err_val = OBD_NO_SESSION_ID;
    (void)obd_change_state(&obdapp_ctxt, OBD_STATE_UNKNOWN, OBD_STATE_ERR);
    (void)rex_set_sigs(obd_task_ptr, MMGSDI_OBDAPP_SIG);
    return FALSE;
  }

  return TRUE;
} /* obd_usim_gsm_authenticate */


/*===========================================================================
FUNCTION OBD_CLOSE_SESSION

DESCRIPTION
  Closes current MMGSDI session

PARAMETER

DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECTS
  None

COMMENTS
  Here we dont need to deregister for events as we need to know when UICC
  is removed.. Therefore, we still get events..
===========================================================================*/
static void obd_close_session
(
  void
)
{
  mmgsdi_return_enum_type mmgsdi_status;
  mmgsdi_session_id_type  sess_id;

  UIM_MSG_HIGH_0("OBD: in obd_close_session");
  if(obdapp_ctxt.session_id_allocated)
  {
    sess_id = obdapp_ctxt.session_id;
  }
  else
  {
    sess_id = 0;
  }

  if(sess_id)
  {
    UIM_MSG_HIGH_2("OBD: calling mmgsdi_session_close for session id: 0x%x%x",
                   sess_id>>32, sess_id & 0xFFFFFFFF);
    mmgsdi_status = mmgsdi_session_close(sess_id,
                                         obd_mmgsdi_cmd_cb,
                                         OBD_REF);

    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      UIM_MSG_ERR_1("OBD: mmgsdi_session_close call returned error: 0x%x",
                    mmgsdi_status);
    }
    obdapp_ctxt.session_id = 0;
    obdapp_ctxt.session_id_allocated = FALSE;
  }
  else
  {
    UIM_MSG_ERR_0("OBD: There is no valid session to close");
  }
  return;
} /* obd_close_session */


/*===========================================================================
FUNCTION OBD_REG_WITH_MMGSDI

DESCRIPTION
  Register with MMGSDI For Events

PARAMETER

DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECTS
  None

COMMENTS
  None
===========================================================================*/
static obd_err_type obd_reg_with_mmgsdi
(
  void
)
{
  mmgsdi_return_enum_type mmgsdi_status;

  UIM_MSG_HIGH_0("OBD: Registering with mmgsdi_client_id_and_evt_reg");

  /* Get client id and register for mmgsdi sim events */
  mmgsdi_status = mmgsdi_client_id_and_evt_reg(
                    obd_mmgsdi_events_cb,
                    (mmgsdi_callback_type)obd_mmgsdi_cmd_cb,
                     OBD_REF);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    obdapp_ctxt.err_val = OBD_FAIL;
    (void)obd_change_state(&obdapp_ctxt, OBD_STATE_UNKNOWN, OBD_STATE_ERR);
    (void)rex_set_sigs(obd_task_ptr, MMGSDI_OBDAPP_SIG);
    UIM_MSG_ERR_1("OBD: mmgsdi_client_id_reg() result 0x%x", mmgsdi_status);
    return OBD_FAIL;
  }

  return OBD_SUCCESS;
} /* obd_reg_with_mmgsdi */


/*========================================================================
FUNCTION OBD_DIAG_RETURN_STATUS

DESCRIPTION
   This function will return the status to diag about the authentication
   of secure sim. This will be registered with Diag in the obdapp_init
   so thatDiag can call into this api when it wants to know the OBD status

DEPENDENCIES
  None

RETURN VALUE
  diag_obd_auth_status_t

SIDE EFFECTS
  None

==========================================================================*/
static diag_obd_auth_status_t obd_diag_return_status
(
  void
)
{
  if(obdapp_ctxt.uicc_diag_authenticated)
  {
    return DIAG_OBD_AUTHENTICATION_SUCCESSFUL;
  }

  return DIAG_OBD_AUTHENTICATION_NOT_SUCCESSFUL;
}


/*==================================================================
FUNCTION OBD_HANDLE_NEW_STATE

DESCRIPTION
   This functions depending on the new state of the obdapp_ctxt
   will invoke required functions to acheive the authentication

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

==================================================================*/
void obd_handle_new_state
(
  void
)
{
  switch(obdapp_ctxt.state)
  {
    case OBD_STATE_INIT:
      /* Check keys if available in FS and get client id from Diag */
      (void)obd_init(obd_task_ptr);
      break;

    case OBD_STATE_MMGSDI_REGISTER:
      /* Register with MMGSDI */
      if(obd_keys_present)
      {
        (void) obd_reg_with_mmgsdi();
      }
      break;

    case OBD_STATE_MMGSDI_REG_DONE:
      /* Nothing to do as we need to wait untill OBDApp's pin is verified */
      break;

    case OBD_STATE_READY:
      /* After OBDAPP state became ready, query for IMSI */
      obd_retrieve_imsi();
     break;

    case OBD_STATE_IMSI_READ:
      /* After IMSI is obtained successfully, send AUTH command */
      (void)obd_usim_gsm_authenticate();
      break;

    case OBD_STATE_AUTH_OBTAINED:
      /* Close non-prov session as we dont require the session further */
      obd_close_session();

      /* Generate Key K */
      obd_generateK();

      /* Received AUTH.. Go ahead to evaluate it */
      (void)obd_verify_AUTH();
      break;

    case OBD_STATE_AUTHENTICATED:
      /* Inform Diag about the AUTH success */
      diag_dci_auth_notify(DIAG_DCI_AUTH_CLIENT_UIM, DIAG_OBD_AUTHENTICATION_SUCCESSFUL);
      UIM_MSG_HIGH_0("OBD: OBDAPP AUTHENTICATION successfull!!");
     break;

    case OBD_STATE_ERR:
      obd_close_all();
      break;

    default:
      UIM_MSG_ERR_1("OBD: Obtained wrong state 0x%x", obdapp_ctxt.state);
      break;
  }
  return;
} /* obd_handle_new_state */


/*===========================================================================
FUNCTION OBD_INIT

DESCRIPTION
  Initialisation function of OBDAPP.. Initializes required varaibles, obdapp
  context structure and any other registrations with Diag etc.. Checks if OBDAPP
  Keys are present with key hanlder otherwise OBDAPP doesn't go forward..


PARAMETER

DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECTS
  None

COMMENTS
  None
===========================================================================*/
boolean obd_init
(
  rex_tcb_type   * gsdi_task_tcb_ptr
)
{
  uint8                    i;
  obd_keys_type            obd_keys;

  if((gsdi_task_tcb_ptr == NULL)||(obd_task_ptr != NULL))
  {
    /*gsdi_task_tcb_ptr should not be NULL*/
    /*obd_task_ptr should be NULL, since it can be called only once*/
    UIM_MSG_ERR_2("obd_init: gsdi_task_tcb_ptr=0x%x, obd_task_ptr=0x%x",
                  gsdi_task_tcb_ptr, obd_task_ptr);
    return FALSE;
  }

  obd_task_ptr = gsdi_task_tcb_ptr;

  memset(&obdapp_ctxt, 0, sizeof(obdapp_ctxt));
  obdapp_ctxt.state                   = OBD_STATE_INIT;
  obdapp_ctxt.obd_aid_detected        = FALSE;
  obdapp_ctxt.uicc_diag_authenticated = FALSE;
  obdapp_ctxt.slot                    = MMGSDI_MAX_SLOT_ID_ENUM;
  for(i=0; i<MILENAGE_OP_LEN; i++)
  {
    obdapp_ctxt.RAND[i] = i;
  }

  /* Obtain keys from SSM Key Handler */
  if(OBD_SUCCESS != obd_get_keys_from_ssm(&obd_keys))
  {
    UIM_MSG_ERR_0("OBD: No Keys in FS, so wil not go ahead");
    obdapp_ctxt.err_val = OBD_NO_KEYS_IN_FS;
    (void)obd_change_state(&obdapp_ctxt, OBD_STATE_INIT, OBD_STATE_ERR);
    (void)rex_set_sigs(obd_task_ptr, MMGSDI_OBDAPP_SIG);
    return FALSE;
  }

  /* Copy OBD AID data, len from keys structure */
  memscpy(obd_aid_data, sizeof(obd_aid_data), obd_keys.AID, MILENAGE_OP_LEN);
  obd_aid_len = (mmgsdi_len_type)obd_keys.AID_len;
  obd_keys_present = TRUE;

  /* Clear the keys */
  (void)secure_memset(&obd_keys, 0, sizeof(obd_keys_type));

  if(DIAG_DCI_REG_FAILED == diag_dci_auth_reg_client(DIAG_DCI_AUTH_CLIENT_UIM,
                                                       (diag_dci_cb_type) &obd_diag_return_status))
  {
    UIM_MSG_ERR_0("OBD: Registration with Diag failed");
    obdapp_ctxt.err_val = OBD_DIAG_CLIENT_REG_FAIL;
    (void)obd_change_state(&obdapp_ctxt, OBD_STATE_UNKNOWN, OBD_STATE_ERR);
    (void)rex_set_sigs(obd_task_ptr, MMGSDI_OBDAPP_SIG);
    return FALSE;
  }

  /* Proceed to register with MMGSDI */
  if(OBD_SUCCESS == obd_change_state(&obdapp_ctxt, OBD_STATE_INIT, OBD_STATE_MMGSDI_REGISTER))
  {
    (void)rex_set_sigs(obd_task_ptr, MMGSDI_OBDAPP_SIG);
  }
  else
  {
    UIM_MSG_ERR_1("OBD: Got to obd_init with wrong state: 0x%x",
                  obdapp_ctxt.state);
  }

  return TRUE;
} /* obd_init */

