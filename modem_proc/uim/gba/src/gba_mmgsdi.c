/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


            G B A   M M G S D I   F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains the GBA MMGSDI/UIM handling functions.

                        COPYRIGHT INFORMATION

Copyright (c) 2015-2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/gba/src/gba_mmgsdi.c#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/22/16   vdc     Remove F3 messages for memory allocation failure
10/26/15   bcho    Allow GBA to proceed if fail to read EF-GBABP content
09/23/15   vdc     Do not read EF GBABP while generating TMPI
07/20/15   tkl     Added HTTP Cookies support
07/09/15   tkl     Check EF GBABP for Ks validity
06/18/15   tkl     Fixed stack overflow
04/06/15   tl      Add GBA_U support
04/27/15   tl      Change IMPI domain tag from ics to ims for spec compliance
01/26/15   tl      Secondary changes
01/26/15   kv      Initial revision
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"
#include "customer.h"
#include "queue.h"
#include <stringl/stringl.h>

#include "mmgsdilib_common.h"
#include "mmgsdilib_v.h"
#include "mmgsdisessionlib_v.h"

#include "gba.h"
#include "gba_bootstrap.h"
#include "gba_http_cookie.h"
#include "gba_lib.h"
#include "gba_mmgsdi.h"
#include "gba_platform.h"
#include "gba_util.h"
#include "gba_storage.h"
#include "gba_state.h"

/*=============================================================================

                       DATA DECLARATIONS

=============================================================================*/

static q_type                    gba_mmgsdi_cmd_q;


#define MNC_START_POS 8
#define MCC_START_POS 15
#define GBA_IMSI_DIGIT_LEN 15

#define GBA_IMSI_HIGH_BYTE_MASK      0xF0
#define GBA_IMSI_LOW_BYTE_MASK       0x0F

#define GBA_3G_AUTH_SUCCESS 0xDB
#define GBA_AUTH_SYNC_FAIL  0xDC

#define GBA_MMGSDI_UST_SIZE  9
#define GBA_MMGSDI_AD_SIZE   4
#define GBA_MMGSDI_IMSI_SIZE 9

typedef struct{
  uint8 imsi_digits_len;
  uint8 imsi_digits[GBA_IMSI_DIGIT_LEN];
} gba_imsi_data_type;

typedef struct {
  mmgsdi_client_id_type  gba_mmgsdi_client_id;
  mmgsdi_session_id_type session_id[GBA_SESSION_MAX];
}gba_mmgsdi_client_type;

static gba_mmgsdi_client_type gba_mmgsdi_client_info;

typedef struct {
  gba_session_type              session_type;
  gba_bootstrap_callback_type   call_back;
  uint32                        reference_id;
  gba_aid_type                  aid;
  gba_mode_enum_type            mode;
  mmgsdi_autn_context_enum_type auth_context;
  gba_naf_id_type               naf_id;
  gba_impi_type                 impi;
  gba_btid_type                 btid;
  gba_lifetime_type             lifetime;
}gba_mmgsdi_current_cmd_type;

static gba_mmgsdi_current_cmd_type gba_mmgsdi_cmd_info;

/*=============================================================================

                       FUNCTION PROTOTYPES

=============================================================================*/

/*===========================================================================
   FUNCTION:      GBA_QUEUE_MMGSDI_EVT_CMD

   DESCRIPTION:
     This function allows  to queue MMGSDI responses and events to GBA task to be
     processed.

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     gba_result_enum_type

   SIDE EFFECTS:
     None
===========================================================================*/
static gba_result_enum_type  gba_queue_mmgsdi_evt_cmd (
  gba_mmgsdi_cmd_type    * task_cmd_ptr
)
{
  if (task_cmd_ptr == NULL)
  {
    GBA_MSG_ERR_0("Attempt to use NULL POINTER");
    return GBA_INCORRECT_PARAMS;
  }

  GBA_ENTER_TASK_STATE_CRIT_SECT;

  /* Do not queue command if GBA task has been stopped */
  if(gba_task_stopped)
  {
    GBA_MSG_ERR_0("GBA task stopped, cannot queue command");
    GBA_LEAVE_TASK_STATE_CRIT_SECT;
    return GBA_GENERIC_ERROR;
  }

  (void)q_link(task_cmd_ptr, &task_cmd_ptr->link);

  /* Put the message on the queue */
  q_put(&gba_mmgsdi_cmd_q, &task_cmd_ptr->link);

  /* Set the command queue signal */
  (void) rex_set_sigs(UIM_GBA_TCB, GBA_MMGSDI_CMD_SIG);

  GBA_LEAVE_TASK_STATE_CRIT_SECT;
  return GBA_SUCCESS;
} /* gba_queue_mmgsdi_evt_cmd */


/*===========================================================================
   FUNCTION:      GBA_MMGSDI_EVT_CB

   DESCRIPTION:
     Callback function for MMGSDI events. Posts data back to GBA
     task to be processed.

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     None

   SIDE EFFECTS:
     None
===========================================================================*/
static void gba_mmgsdi_evt_cb (
  const mmgsdi_event_data_type  * event_ptr
)
{

  gba_mmgsdi_cmd_type *gba_mmgsdi_cmd_ptr = NULL;

  if(event_ptr == NULL)
  {
    GBA_MSG_ERR_0("Invalid MMGSDI event pointer");
    return;
  }

  if (event_ptr->evt != MMGSDI_CARD_INSERTED_EVT &&
      event_ptr->evt != MMGSDI_CARD_ERROR_EVT &&
      event_ptr->evt != MMGSDI_CARD_REMOVED_EVT &&
      event_ptr->evt != MMGSDI_SESSION_CHANGED_EVT)
  {
    /* Ignore other MMGSDI events as they are not required */
    return;
  }

  gba_mmgsdi_cmd_ptr = (gba_mmgsdi_cmd_type*)GBA_MALLOC(sizeof(gba_mmgsdi_cmd_type));
  if (gba_mmgsdi_cmd_ptr == NULL)
  {
    return;
  }

  gba_mmgsdi_cmd_ptr->cmd_type = GBA_MMGSDI_EVENT_RPT;
  memscpy((void *)&gba_mmgsdi_cmd_ptr->data.event_data.mmgsdi_evt_data,
          sizeof(mmgsdi_event_data_type),
          event_ptr,
          sizeof(mmgsdi_event_data_type));

  (void)gba_queue_mmgsdi_evt_cmd(gba_mmgsdi_cmd_ptr);


} /* gba_mmgsdi_evt_cb */


/*===========================================================================
   FUNCTION:      GBA_MMGSDI_CMD_RSP_CB

   DESCRIPTION:
     GBA MMGSDI callback function for various MMGSDI APIs

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     None

   SIDE EFFECTS:
     None
===========================================================================*/
static void gba_mmgsdi_cmd_rsp_cb (
  mmgsdi_return_enum_type   status,
  mmgsdi_cnf_enum_type      cnf,
  const mmgsdi_cnf_type    *cnf_ptr
)
{
  gba_mmgsdi_cmd_type      * gba_mmgsdi_cmd_ptr = NULL;

  if (cnf_ptr == NULL)
  {
    return;
  }

  gba_mmgsdi_cmd_ptr = (gba_mmgsdi_cmd_type*)GBA_MALLOC(sizeof(gba_mmgsdi_cmd_type));
  if (gba_mmgsdi_cmd_ptr == NULL)
  {
    return;
  }

  /* deep copy */
  switch (cnf)
  {
    case MMGSDI_CLIENT_ID_AND_EVT_REG_CNF:
      gba_mmgsdi_cmd_ptr->data.rsp_data.cnf_data.client_id_and_evt_reg_cnf =
        cnf_ptr->client_id_and_evt_reg_cnf;
      break;

    case MMGSDI_SESSION_OPEN_CNF:
      gba_mmgsdi_cmd_ptr->data.rsp_data.cnf_data.session_open_cnf =
        cnf_ptr->session_open_cnf;
      break;

    case MMGSDI_WRITE_CNF:
      gba_mmgsdi_cmd_ptr->data.rsp_data.cnf_data.write_cnf =
        cnf_ptr->write_cnf;
      break;

    case MMGSDI_READ_CNF:
      gba_mmgsdi_cmd_ptr->data.rsp_data.cnf_data.read_cnf =
        cnf_ptr->read_cnf;

      if (cnf_ptr->read_cnf.read_data.data_len > 0)
      {
        gba_mmgsdi_cmd_ptr->data.rsp_data.cnf_data.read_cnf.read_data.data_ptr =
          GBA_MALLOC(cnf_ptr->read_cnf.read_data.data_len);
        if (gba_mmgsdi_cmd_ptr->data.rsp_data.cnf_data.read_cnf.read_data.data_ptr == NULL)
        {
          return;
        }
        gba_mmgsdi_cmd_ptr->data.rsp_data.cnf_data.read_cnf.read_data.data_len =
          cnf_ptr->read_cnf.read_data.data_len;
        memscpy(gba_mmgsdi_cmd_ptr->data.rsp_data.cnf_data.read_cnf.read_data.data_ptr,
                cnf_ptr->read_cnf.read_data.data_len,
                cnf_ptr->read_cnf.read_data.data_ptr,
                cnf_ptr->read_cnf.read_data.data_len);
      }
      break;
    case MMGSDI_ISIM_AUTH_CNF:
      gba_mmgsdi_cmd_ptr->data.rsp_data.cnf_data.isim_auth_cnf =
        cnf_ptr->isim_auth_cnf;

      if (cnf_ptr->isim_auth_cnf.res.data_len > 0)
      {
        gba_mmgsdi_cmd_ptr->data.rsp_data.cnf_data.isim_auth_cnf.res.data_ptr =
          GBA_MALLOC(cnf_ptr->isim_auth_cnf.res.data_len);
        if (gba_mmgsdi_cmd_ptr->data.rsp_data.cnf_data.isim_auth_cnf.res.data_ptr == NULL)
        {
          return;
        }
        gba_mmgsdi_cmd_ptr->data.rsp_data.cnf_data.isim_auth_cnf.res.data_len =
          cnf_ptr->isim_auth_cnf.res.data_len;
        memscpy(gba_mmgsdi_cmd_ptr->data.rsp_data.cnf_data.isim_auth_cnf.res.data_ptr,
                cnf_ptr->isim_auth_cnf.res.data_len,
                cnf_ptr->isim_auth_cnf.res.data_ptr,
                cnf_ptr->isim_auth_cnf.res.data_len);
      }

      if (cnf_ptr->isim_auth_cnf.ck.data_len > 0)
      {
        gba_mmgsdi_cmd_ptr->data.rsp_data.cnf_data.isim_auth_cnf.ck.data_ptr =
          GBA_MALLOC(cnf_ptr->isim_auth_cnf.ck.data_len);
        if (gba_mmgsdi_cmd_ptr->data.rsp_data.cnf_data.isim_auth_cnf.ck.data_ptr == NULL)
        {
          return;
        }
        gba_mmgsdi_cmd_ptr->data.rsp_data.cnf_data.isim_auth_cnf.ck.data_len =
          cnf_ptr->isim_auth_cnf.ck.data_len;
        memscpy(gba_mmgsdi_cmd_ptr->data.rsp_data.cnf_data.isim_auth_cnf.ck.data_ptr,
                cnf_ptr->isim_auth_cnf.ck.data_len,
                cnf_ptr->isim_auth_cnf.ck.data_ptr,
                cnf_ptr->isim_auth_cnf.ck.data_len);
      }

      if (cnf_ptr->isim_auth_cnf.ik.data_len > 0)
      {
        gba_mmgsdi_cmd_ptr->data.rsp_data.cnf_data.isim_auth_cnf.ik.data_ptr =
          GBA_MALLOC(cnf_ptr->isim_auth_cnf.ik.data_len);
        if (gba_mmgsdi_cmd_ptr->data.rsp_data.cnf_data.isim_auth_cnf.ik.data_ptr == NULL)
        {
          return;
        }
        gba_mmgsdi_cmd_ptr->data.rsp_data.cnf_data.isim_auth_cnf.ik.data_len =
          cnf_ptr->isim_auth_cnf.ik.data_len;
        memscpy(gba_mmgsdi_cmd_ptr->data.rsp_data.cnf_data.isim_auth_cnf.ik.data_ptr,
                cnf_ptr->isim_auth_cnf.ik.data_len,
                cnf_ptr->isim_auth_cnf.ik.data_ptr,
                cnf_ptr->isim_auth_cnf.ik.data_len);
      }

      if (cnf_ptr->isim_auth_cnf.auts.data_len > 0)
      {
        gba_mmgsdi_cmd_ptr->data.rsp_data.cnf_data.isim_auth_cnf.auts.data_ptr =
          GBA_MALLOC(cnf_ptr->isim_auth_cnf.auts.data_len);
        if (gba_mmgsdi_cmd_ptr->data.rsp_data.cnf_data.isim_auth_cnf.auts.data_ptr == NULL)
        {
          return;
        }
        gba_mmgsdi_cmd_ptr->data.rsp_data.cnf_data.isim_auth_cnf.auts.data_len =
          cnf_ptr->isim_auth_cnf.auts.data_len;
        memscpy(gba_mmgsdi_cmd_ptr->data.rsp_data.cnf_data.isim_auth_cnf.auts.data_ptr,
                cnf_ptr->isim_auth_cnf.auts.data_len,
                cnf_ptr->isim_auth_cnf.auts.data_ptr,
                cnf_ptr->isim_auth_cnf.auts.data_len);
      }
      break;

    case MMGSDI_USIM_AUTH_CNF:
      gba_mmgsdi_cmd_ptr->data.rsp_data.cnf_data.usim_auth_cnf =
        cnf_ptr->usim_auth_cnf;
      if(cnf_ptr->usim_auth_cnf.auth_response.data_len > 0)
      {
        gba_mmgsdi_cmd_ptr->data.rsp_data.cnf_data.usim_auth_cnf.auth_response.data_ptr =
          GBA_MALLOC(cnf_ptr->usim_auth_cnf.auth_response.data_len);
        if (gba_mmgsdi_cmd_ptr->data.rsp_data.cnf_data.usim_auth_cnf.auth_response.data_ptr == NULL)
        {
          return;
        }
        gba_mmgsdi_cmd_ptr->data.rsp_data.cnf_data.usim_auth_cnf.auth_response.data_len =
          cnf_ptr->usim_auth_cnf.auth_response.data_len;
        memscpy(gba_mmgsdi_cmd_ptr->data.rsp_data.cnf_data.usim_auth_cnf.auth_response.data_ptr,
                cnf_ptr->usim_auth_cnf.auth_response.data_len,
                cnf_ptr->usim_auth_cnf.auth_response.data_ptr,
                cnf_ptr->usim_auth_cnf.auth_response.data_len);
      }
      break;

    case MMGSDI_SESSION_CLOSE_CNF:
      /* Do nothing for session close */
      GBA_MSG_HIGH_3("Session close cnf: status 0x%x, for session id 0x%x and app type 0x%x",status,
                      cnf_ptr->session_close_cnf.session_id,
                      cnf_ptr->session_close_cnf.app_info.app_type);
      return;

    default:
      GBA_MSG_ERR_1("Invalid MMGSDI CNF: 0x%x", cnf);
      return;
  }

  gba_mmgsdi_cmd_ptr->cmd_type = GBA_MMGSDI_CMD_RSP;
  gba_mmgsdi_cmd_ptr->data.rsp_data.cnf    = cnf;
  gba_mmgsdi_cmd_ptr->data.rsp_data.status = status;

  (void)gba_queue_mmgsdi_evt_cmd(gba_mmgsdi_cmd_ptr);
}/*gba_mmgsdi_cmd_rsp_cb*/


/*===========================================================================
   FUNCTION:      GBA_PROCESS_ISIM_ACTIVATION

   DESCRIPTION:
     GBA MMGSDI handling for ISIM app

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     gba_result_enum_type

   SIDE EFFECTS:
     None
===========================================================================*/
static gba_result_enum_type gba_process_isim_activation(
  gba_aid_type               aid,
  gba_session_type           session_type
)
{
  mmgsdi_session_type_enum_type    mmgsdi_session_type  = MMGSDI_MAX_SESSION_TYPE_ENUM;
  mmgsdi_non_prov_app_info_type    non_prov_app_data;
  mmgsdi_return_enum_type          mmgsdi_status        = MMGSDI_SUCCESS;
  gba_result_enum_type             gba_status           = GBA_SUCCESS;
  mmgsdi_available_apps_type     * available_apps_ptr   = NULL;

  memset(&non_prov_app_data, 0x00, sizeof(mmgsdi_non_prov_app_info_type));

  available_apps_ptr = (mmgsdi_available_apps_type*)GBA_MALLOC(sizeof(mmgsdi_available_apps_type));

  if (available_apps_ptr == NULL)
  {
    return GBA_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  /* check if the card is available and is a uicc card */
  mmgsdi_status = mmgsdi_get_all_available_apps_sync(gba_mmgsdi_client_info.gba_mmgsdi_client_id,
                                                     available_apps_ptr);

  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    gba_status = GBA_GENERIC_ERROR;
  }
  /* Check if the card that is required is initialized */
  else if((session_type == GBA_NON_PROV_SESSION_SLOT_1 &&
           available_apps_ptr->card1_num_app == 0) ||
          (session_type == GBA_NON_PROV_SESSION_SLOT_2 &&
           available_apps_ptr->card2_num_app == 0))
  {
    GBA_MSG_ERR_0("Card not initialized yet");
    gba_status = GBA_GENERIC_ERROR;
  }
  else if((session_type == GBA_NON_PROV_SESSION_SLOT_1 &&
           (available_apps_ptr->card1_num_app > 0 &&
            (available_apps_ptr->card1_app_info[0].app_data.app_type == MMGSDI_APP_SIM ||
             available_apps_ptr->card1_app_info[0].app_data.app_type == MMGSDI_APP_RUIM ))) ||
          (session_type == GBA_NON_PROV_SESSION_SLOT_2 &&
           (available_apps_ptr->card2_num_app > 0 &&
            (available_apps_ptr->card2_app_info[0].app_data.app_type == MMGSDI_APP_SIM ||
             available_apps_ptr->card2_app_info[0].app_data.app_type == MMGSDI_APP_RUIM ))))
  {
    GBA_MSG_ERR_0("Card initialized but the card is ICC card");
    gba_status = GBA_GENERIC_ERROR;
  }

  GBA_FREE(available_apps_ptr);

  if (gba_status != GBA_SUCCESS)
  {
    return gba_status;
  }

  gba_status = gba_util_get_session_index_and_mmgsdi_session_type(session_type,
                                                                  NULL,
                                                                  &mmgsdi_session_type);
  if (gba_status != GBA_SUCCESS)
  {
    return gba_status;
  }

  non_prov_app_data.app_id_data.data_len = aid.data_len;
  non_prov_app_data.app_id_data.data_ptr = aid.data;
  non_prov_app_data.exclusive_channel    = FALSE;

  /* open non prov session */
  mmgsdi_status = mmgsdi_session_open(gba_mmgsdi_client_info.gba_mmgsdi_client_id,
                                      mmgsdi_session_type,
                                      &non_prov_app_data,
                                      gba_mmgsdi_evt_cb,
                                      FALSE,
                                      gba_mmgsdi_cmd_rsp_cb,
                                      (mmgsdi_client_data_type)gba_mmgsdi_cmd_info.reference_id);

  return gba_convert_mmgsdi_status_to_gba(mmgsdi_status);
}/*gba_process_isim_activation*/


/*===========================================================================
   FUNCTION:      GBA_PROCESS_USIM_ACTIVATION

   DESCRIPTION:
     GBA MMGSDI handling for USIM app

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     gba_result_enum_type

   SIDE EFFECTS:
     None
===========================================================================*/
static gba_result_enum_type gba_process_usim_activation(
  gba_session_type           session_type
)
{
  mmgsdi_session_type_enum_type    mmgsdi_session_type  = MMGSDI_MAX_SESSION_TYPE_ENUM;
  mmgsdi_return_enum_type          mmgsdi_status        = MMGSDI_SUCCESS;
  gba_result_enum_type             gba_status           = GBA_GENERIC_ERROR;

  gba_status = gba_util_get_session_index_and_mmgsdi_session_type(session_type,
                                                                  NULL,
                                                                  &mmgsdi_session_type);
  if (gba_status != GBA_SUCCESS)
  {
    return gba_status;
  }

  /* open  prov session */
  mmgsdi_status = mmgsdi_session_open(gba_mmgsdi_client_info.gba_mmgsdi_client_id,
                                      mmgsdi_session_type,
                                      NULL,
                                      gba_mmgsdi_evt_cb,
                                      FALSE,
                                      gba_mmgsdi_cmd_rsp_cb,
                                      (mmgsdi_client_data_type)gba_mmgsdi_cmd_info.reference_id);

  return gba_convert_mmgsdi_status_to_gba(mmgsdi_status);
}/*gba_process_usim_activation*/


/*===========================================================================
  FUNCTION GBA_MMGSDI_UTIL_CONVERT_TO_IMSI_DATA_TYPE()

  DESCRIPTION
    Convert IMSI from bytes to digits

  PARAMETERS
    imsi_data_ptr  : value of imsiin byte format from card
    imsi_digit_ptr : value of imsi in digit format

  RETURN VALUE
    gba_result_enum_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static gba_result_enum_type gba_mmgsdi_util_convert_to_imsi_data_type
(
  const mmgsdi_data_type *imsi_data_ptr,
  gba_imsi_data_type  *imsi_digit_ptr
)
{
  uint8                 i     = 0;

  if((imsi_data_ptr == NULL) ||
     (imsi_digit_ptr == NULL) ||
     (imsi_data_ptr->data_ptr == NULL) ||
     (imsi_data_ptr->data_len < 4) ||  /* atleast mcc mnc should be present in imsi */
    (((imsi_data_ptr->data_len - 1) * 2) - 1 > sizeof(imsi_digit_ptr->imsi_digits)))
  {
    UIM_MSG_ERR_0("Invalid imsi_data_ptr");
    return GBA_GENERIC_ERROR;
  }

  imsi_digit_ptr->imsi_digits_len = 0;
  memset(&imsi_digit_ptr->imsi_digits[0], 0xF, sizeof(imsi_digit_ptr->imsi_digits));

  /* -----------------------------------------------------------
    Parse IMSI:
    B0     B1       B2      B3     B4       B5      B6          B7        B8
    LEN  D1PB   D3D2 D5D4 D7D6  D9D8 D11D10 D13D12  D15D14
  -------------------------------------------------------------*/
  for(i = 0; (i < (imsi_data_ptr->data_len - 2)); i++)
  {
    imsi_digit_ptr->imsi_digits[i*2] = imsi_data_ptr->data_ptr[i+1] & GBA_IMSI_HIGH_BYTE_MASK;
    imsi_digit_ptr->imsi_digits[i*2] >>= 4;
    imsi_digit_ptr->imsi_digits_len++;
    imsi_digit_ptr->imsi_digits[i*2 + 1] = imsi_data_ptr->data_ptr[i+2] & GBA_IMSI_LOW_BYTE_MASK;
    imsi_digit_ptr->imsi_digits_len++;
  }

  /* process last digit only if it is not 0XF */
  if (imsi_data_ptr->data_ptr[(imsi_data_ptr->data_len) - 1] != GBA_IMSI_HIGH_BYTE_MASK)
  {
    imsi_digit_ptr->imsi_digits[i * 2] =
      imsi_data_ptr->data_ptr[(imsi_data_ptr->data_len) - 1] & GBA_IMSI_HIGH_BYTE_MASK;
    imsi_digit_ptr->imsi_digits[i * 2] >>= 4;
    imsi_digit_ptr->imsi_digits_len++;
  }

  return GBA_SUCCESS;
} /* gba_mmgsdi_util_convert_to_imsi_data_type */


/*===========================================================================
  FUNCTION GBA_MMGSDI_PREPARE_IMPI_FROM_IMSI

  DESCRIPTION
    Prepare IMPI From USIM IMSI

  PARAMETERS
    imsi_digit_ptr : value of imsi in a gba_imsi_data_type digit format
    impi_data_ptr : IMPI after conversion from IMSI

  RETURN VALUE
    gba_result_enum_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static gba_result_enum_type gba_mmgsdi_prepare_impi_from_imsi(
  const gba_imsi_data_type  *imsi_digit_ptr,
  gba_impi_type             *impi_data_ptr,
  uint8                      mnc_length
)
{
  char  part2[] = {"@ims.mnc000.mcc000.3gppnetwork.org"};
  uint8 index   = 0;

  /* For example if the IMSI is 234150999999999 (MCC = 234, MNC = 15), the
     private user identity then takes the form
     234150999999999@ims.mnc015.mcc234.3gppnetwork.org as specified in TS.23.003
     section 13. */

  for(index = 0; index < imsi_digit_ptr->imsi_digits_len; index++)
  {
    impi_data_ptr->data[index] = '0' + imsi_digit_ptr->imsi_digits[index];
  }

  impi_data_ptr->data_len = imsi_digit_ptr->imsi_digits_len;

  memscpy(&impi_data_ptr->data[index],GBA_MAX_IMPI_LEN-impi_data_ptr->data_len,
          part2,strlen(part2));

  /*Update MNC */
  if(mnc_length == 3)
  {
    impi_data_ptr->data[index + MNC_START_POS]   = impi_data_ptr->data[3];
    impi_data_ptr->data[index + MNC_START_POS+1] = impi_data_ptr->data[4];
    impi_data_ptr->data[index + MNC_START_POS+2] = impi_data_ptr->data[5];

  }else
  {
    impi_data_ptr->data[index + MNC_START_POS] = '0';
    impi_data_ptr->data[index + MNC_START_POS+1] = impi_data_ptr->data[3];
    impi_data_ptr->data[index + MNC_START_POS+2] = impi_data_ptr->data[4];
  }

  impi_data_ptr->data[index + MCC_START_POS]   = impi_data_ptr->data[0];
  impi_data_ptr->data[index + MCC_START_POS+1] = impi_data_ptr->data[1];
  impi_data_ptr->data[index + MCC_START_POS+2] = impi_data_ptr->data[2];

  impi_data_ptr->data[index + strlen(part2)] = '\0';

  impi_data_ptr->data_len += strlen(part2);

  return GBA_SUCCESS;

}/*gba_mmgsdi_prepare_impi_from_imsi*/


/*===========================================================================
   FUNCTION:      GBA_PROCESS_USIM_SESSION_OPEN

   DESCRIPTION:
     GBA MMGSDI USIM session open response handling.

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     gba_result_enum_type

   SIDE EFFECTS:
     None
===========================================================================*/
static gba_result_enum_type gba_process_usim_session_open(
  void
)
{
  mmgsdi_return_enum_type          mmgsdi_status        = MMGSDI_SUCCESS;
  gba_result_enum_type             gba_status           = GBA_GENERIC_ERROR;
  mmgsdi_session_info_query_type   session_info_query   = {0};
  mmgsdi_session_get_info_type     session_get_info     = {0};
  gba_bootstrap_callback_data_type callback_data        = {0};
  mmgsdi_data_type                 read_data;
  mmgsdi_len_type                  file_size = 0;
  uint8                            ad_data[GBA_MMGSDI_AD_SIZE]     = {0};
  uint8                            ust_data[GBA_MMGSDI_UST_SIZE]   = {0};
  uint8                            imsi_data[GBA_MMGSDI_IMSI_SIZE] = {0};
  uint8                            mnc_length = 0;
  gba_imsi_data_type               imsi_digits;
  gba_impi_type                    impi_data;

  /*
   1. Check if Session type that is passed is active and assosciated to a USIM app
   2. Read IMSI and AD
   3. Prepare IMPI from IMSI
   4. Trigger CB
  */
  GBA_MSG_HIGH_0("GBA MMGSDI process USIM session open");

  session_info_query.query_type = MMGSDI_SESSION_INFO_QUERY_BY_ID;
  session_info_query.query_key.session_id = gba_mmgsdi_client_info.session_id[gba_mmgsdi_cmd_info.session_type];

  mmgsdi_status = mmgsdi_session_get_info_sync(session_info_query,&session_get_info);
  if(mmgsdi_status != MMGSDI_SUCCESS ||
     session_get_info.app_state != MMGSDI_APP_STATE_READY ||
     session_get_info.app_data.app_type != MMGSDI_APP_USIM)
  {
    GBA_MSG_ERR_2("GBA MMGSDI USIM session info failed App state - 0x%x invalid of app type - 0x%x do not match",
                    session_get_info.app_state, session_get_info.app_data.app_type);
    return GBA_GENERIC_ERROR;
  }

  read_data.data_ptr = ust_data;
  read_data.data_len = GBA_MMGSDI_UST_SIZE;

  mmgsdi_status = mmgsdi_session_read_cache(gba_mmgsdi_client_info.session_id[gba_mmgsdi_cmd_info.session_type],
                                            MMGSDI_USIM_UST, read_data);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    GBA_MSG_ERR_0("GBA MMGSDI GBA service in UST is set, cannot proceed with GBA_ME");
    return GBA_GENERIC_ERROR;
  }

  if(ust_data[8] & 0x08)
  {
    gba_mmgsdi_cmd_info.mode = GBA_U;
  }

  read_data.data_ptr = ad_data;
  read_data.data_len = GBA_MMGSDI_AD_SIZE;

  mmgsdi_status = mmgsdi_session_read_cache(gba_mmgsdi_client_info.session_id[gba_mmgsdi_cmd_info.session_type],
                                            MMGSDI_USIM_AD, read_data);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return GBA_GENERIC_ERROR;
  }

  if (ad_data[3] == 0x02)
  {
    mnc_length = 2;
  }
  else if (ad_data[3] == 0x03)
  {
    mnc_length = 3;
  }

  mmgsdi_status = mmgsdi_session_read_cache_file_size(gba_mmgsdi_client_info.session_id[gba_mmgsdi_cmd_info.session_type],
                                                      MMGSDI_USIM_IMSI, &file_size);
  if(mmgsdi_status != MMGSDI_SUCCESS ||
     file_size <= 0 ||
     file_size > GBA_MMGSDI_IMSI_SIZE)
  {
    return GBA_GENERIC_ERROR;
  }

  read_data.data_ptr = imsi_data;
  read_data.data_len = file_size;

  mmgsdi_status = mmgsdi_session_read_cache(gba_mmgsdi_client_info.session_id[gba_mmgsdi_cmd_info.session_type],
                                            MMGSDI_USIM_IMSI, read_data);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return GBA_GENERIC_ERROR;
  }

  gba_status = gba_mmgsdi_util_convert_to_imsi_data_type(&read_data, &imsi_digits);
  if(gba_status != GBA_SUCCESS)
  {
    return GBA_GENERIC_ERROR;
  }

  memset(impi_data.data,0x0, sizeof(impi_data.data));
  impi_data.data_len = 0;
  gba_status = gba_mmgsdi_prepare_impi_from_imsi(&imsi_digits, &impi_data, mnc_length);
  if(gba_status != GBA_SUCCESS)
  {
    return GBA_GENERIC_ERROR;
  }

  callback_data.cmd_type                    = GBA_RETRIEVE_IDENTITY_CMD;
  callback_data.data.identity.session_type  = gba_mmgsdi_cmd_info.session_type;
  callback_data.data.identity.mode          = gba_mmgsdi_cmd_info.mode;
  callback_data.data.identity.impi.data_len = impi_data.data_len;

  memscpy(callback_data.data.identity.impi.data,
          GBA_MAX_IMPI_LEN,
          impi_data.data,
          impi_data.data_len);

  GBA_MSG_HIGH_0("GBA MMGSDI USIM IMPI retrivel successful");

  (gba_mmgsdi_cmd_info.call_back)(GBA_SUCCESS,
                                  &callback_data,
                                  gba_mmgsdi_cmd_info.reference_id);

  return GBA_SUCCESS;
}/*gba_process_usim_session_open*/


/*===========================================================================
   FUNCTION:      GBA_MMGSDI_GET_CARD_INFO

   DESCRIPTION:
     This function extracts the card information to determine the app availability

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     gba_result_enum_type

   SIDE EFFECTS:
     None
===========================================================================*/
static gba_result_enum_type gba_mmgsdi_get_card_info(
  gba_aid_type                  aid,
  gba_session_type              session_type
)
{
  switch (session_type)
  {
    case GBA_NON_PROV_SESSION_SLOT_1:
    case GBA_NON_PROV_SESSION_SLOT_2:
      return gba_process_isim_activation(aid, session_type);
    case GBA_3GPP_PROV_SESSION_PRI:
    case GBA_3GPP_PROV_SESSION_SEC:
      return gba_process_usim_activation(session_type);
    default:
      break;
  }

  return GBA_GENERIC_ERROR;
}/* gba_mmgsdi_get_card_info */


/*===========================================================================
   FUNCTION:      GBA_MMGSDI_TRIGGER_GBA_U_NAF_DERIVATION

   DESCRIPTION:
     This function triggers GBA_NAF derivation auth procedure for GBA_U in MMGSDI

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     gba_result_enum_type

   SIDE EFFECTS:
     None
===========================================================================*/
static gba_result_enum_type gba_mmgsdi_trigger_gba_u_naf_derivation(
  void
)
{
  mmgsdi_data_type        auth_req      = {0, NULL};
  mmgsdi_return_enum_type mmgsdi_status = MMGSDI_ERROR;
  uint32                  len_offset    = 0;

  auth_req.data_len += gba_mmgsdi_cmd_info.naf_id.fqdn.data_len;
  auth_req.data_len += sizeof(gba_mmgsdi_cmd_info.naf_id.security_protocol);
  /* for length field */
  auth_req.data_len += 1;

  /* IMPI only required for USIM */
  if (gba_mmgsdi_cmd_info.session_type == GBA_3GPP_PROV_SESSION_PRI ||
      gba_mmgsdi_cmd_info.session_type == GBA_3GPP_PROV_SESSION_SEC)
  {
    auth_req.data_len += gba_mmgsdi_cmd_info.impi.data_len;
    /* for length field */
    auth_req.data_len += 1;
  }

  auth_req.data_ptr = GBA_MALLOC(auth_req.data_len);
  if(auth_req.data_ptr == NULL)
  {
    return GBA_GENERIC_ERROR;
  }

  auth_req.data_ptr[len_offset] = gba_mmgsdi_cmd_info.naf_id.fqdn.data_len +
                                  sizeof(gba_mmgsdi_cmd_info.naf_id.security_protocol);
  len_offset++;
  memscpy(&(auth_req.data_ptr[len_offset]),
          (auth_req.data_len - len_offset),
          gba_mmgsdi_cmd_info.naf_id.fqdn.data,
          gba_mmgsdi_cmd_info.naf_id.fqdn.data_len);

  len_offset += gba_mmgsdi_cmd_info.naf_id.fqdn.data_len;

  memscpy(&(auth_req.data_ptr[len_offset]),
          (auth_req.data_len - len_offset),
          gba_mmgsdi_cmd_info.naf_id.security_protocol,
          sizeof(gba_mmgsdi_cmd_info.naf_id.security_protocol));

  len_offset += sizeof(gba_mmgsdi_cmd_info.naf_id.security_protocol);

  /* IMPI only required for USIM */
  if (gba_mmgsdi_cmd_info.session_type == GBA_3GPP_PROV_SESSION_PRI ||
      gba_mmgsdi_cmd_info.session_type == GBA_3GPP_PROV_SESSION_SEC)
  {
    auth_req.data_ptr[len_offset] = gba_mmgsdi_cmd_info.impi.data_len;
    len_offset++;
    memscpy(&(auth_req.data_ptr[len_offset]),
            (auth_req.data_len - len_offset),
            gba_mmgsdi_cmd_info.impi.data,
            gba_mmgsdi_cmd_info.impi.data_len);

    len_offset += gba_mmgsdi_cmd_info.impi.data_len;
  }

  /* perform NAF Derivation */
  if (gba_mmgsdi_cmd_info.session_type == GBA_NON_PROV_SESSION_SLOT_1 ||
      gba_mmgsdi_cmd_info.session_type == GBA_NON_PROV_SESSION_SLOT_2)
  {
    mmgsdi_status = mmgsdi_session_uicc_authenticate(
                      gba_mmgsdi_client_info.session_id[gba_mmgsdi_cmd_info.session_type],
                      MMGSDI_AUTN_ISIM_GBA_NAF_DERIVATION_CONTEXT,
                      auth_req,
                      gba_mmgsdi_cmd_rsp_cb,
                      (mmgsdi_client_data_type)gba_mmgsdi_cmd_info.reference_id);
    gba_mmgsdi_cmd_info.auth_context = MMGSDI_AUTN_ISIM_GBA_NAF_DERIVATION_CONTEXT;
  }
  else if (gba_mmgsdi_cmd_info.session_type == GBA_3GPP_PROV_SESSION_PRI ||
           gba_mmgsdi_cmd_info.session_type == GBA_3GPP_PROV_SESSION_SEC)
  {
    mmgsdi_status = mmgsdi_session_usim_authenticate(
                      gba_mmgsdi_client_info.session_id[gba_mmgsdi_cmd_info.session_type],
                      MMGSDI_AUTN_USIM_GBA_NAF_DERIVATION_CONTEXT,
                      auth_req,
                      gba_mmgsdi_cmd_rsp_cb,
                      (mmgsdi_client_data_type)gba_mmgsdi_cmd_info.reference_id);
    gba_mmgsdi_cmd_info.auth_context = MMGSDI_AUTN_USIM_GBA_NAF_DERIVATION_CONTEXT;

  }
  GBA_FREE(auth_req.data_ptr);

  return gba_convert_mmgsdi_status_to_gba(mmgsdi_status);
}/*gba_mmgsdi_trigger_gba_u_naf_derivation*/


/*===========================================================================
   FUNCTION:      GBA_MMGSDI_UPDATE_EF_GBABP

   DESCRIPTION:
     This function updates BTID and lifetime fields of EF GBABP

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     gba_result_enum_type

   SIDE EFFECTS:
     None
===========================================================================*/
static gba_result_enum_type gba_mmgsdi_update_ef_gbabp(
  const gba_mmgsdi_cmd_rsp_data_type * rsp_data_ptr
)
{
  mmgsdi_write_data_type  write_data         = {0, NULL};
  uint32                  len_to_write       = 0;
  uint32                  len_offset         = 0;
  mmgsdi_return_enum_type mmgsdi_status      = MMGSDI_ERROR;

  /* Format of EF GBABP
          -----------------------------------------------------------------------
          |LEN of RAND (1) | RAND |LEN of BTID (1) | BTID | LEN of LIFETIME (1) | LIFETIME |
          -----------------------------------------------------------------------

         After successful Read on GBABP, copy back the RAND part from read response data
         and append BTID and LIFETIME Data. These parameters are further used by UICC
         for KS NAF derivation

     */

  GBA_MSG_HIGH_0("GBA MMGSDI GBABP Read done");

  /* get length of rand from EF GBABP */
  if ((rsp_data_ptr->cnf_data.read_cnf.read_data.data_ptr[0] >
      rsp_data_ptr->cnf_data.read_cnf.read_data.data_len) ||
      rsp_data_ptr->cnf_data.read_cnf.read_data.data_ptr[0] == 0xFF)
  {
    /* invalid RAND length */
    GBA_MSG_HIGH_0("GBA MMGSDI EF GBABP Invalid data");
    return GBA_GENERIC_ERROR;
  }

  len_to_write = rsp_data_ptr->cnf_data.read_cnf.read_data.data_ptr[0];

  /* determine total length of EF GBABP */
  len_to_write += gba_mmgsdi_cmd_info.btid.data_len;
  len_to_write += gba_mmgsdi_cmd_info.lifetime.data_len;
  /* for length fields */
  len_to_write += 3;

  if (len_to_write > rsp_data_ptr->cnf_data.read_cnf.read_data.data_len)
  {
    GBA_MSG_HIGH_0("GBA MMGSDI EF GBABP Invalid data");
    return GBA_GENERIC_ERROR;
  }

  /* allocate data ptr */
  write_data.data_ptr = GBA_MALLOC(len_to_write);
  if(write_data.data_ptr == NULL)
  {
    return GBA_GENERIC_ERROR;
  }

  /* copy back rand including length */
  memscpy(write_data.data_ptr,
          len_to_write,
          rsp_data_ptr->cnf_data.read_cnf.read_data.data_ptr,
          (rsp_data_ptr->cnf_data.read_cnf.read_data.data_ptr[0] + 1));

  len_offset += rsp_data_ptr->cnf_data.read_cnf.read_data.data_ptr[0] + 1;

  write_data.data_ptr[len_offset] = gba_mmgsdi_cmd_info.btid.data_len;
  len_offset++;

  /* copy btid */
  memscpy(&(write_data.data_ptr[len_offset]),
          (len_to_write - len_offset),
          gba_mmgsdi_cmd_info.btid.data,
          gba_mmgsdi_cmd_info.btid.data_len);

  len_offset += gba_mmgsdi_cmd_info.btid.data_len;

  write_data.data_ptr[len_offset] = gba_mmgsdi_cmd_info.lifetime.data_len;
  len_offset++;

  /* copy lifetime */
  memscpy(&(write_data.data_ptr[len_offset]),
          (len_to_write - len_offset),
          gba_mmgsdi_cmd_info.lifetime.data,
          gba_mmgsdi_cmd_info.lifetime.data_len);

  len_offset += gba_mmgsdi_cmd_info.lifetime.data_len;

  write_data.data_len = len_to_write;

  /* Write into EF GBABP */
  mmgsdi_status = mmgsdi_session_write_transparent(
                    gba_mmgsdi_client_info.session_id[gba_mmgsdi_cmd_info.session_type],
                    rsp_data_ptr->cnf_data.read_cnf.access,
                    0,
                    write_data,
                    gba_mmgsdi_cmd_rsp_cb,gba_mmgsdi_cmd_info.reference_id);
  GBA_FREE(write_data.data_ptr);
  return gba_convert_mmgsdi_status_to_gba(mmgsdi_status);
}/*gba_mmgsdi_update_ef_gbabp */


/*===========================================================================
   FUNCTION:      GBA_MMGSDI_PROCESS_GBABP_RSP

   DESCRIPTION:
     This function shall process read response of EF GBABP for Ks validity

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     gba_result_enum_type

   SIDE EFFECTS:
     None
===========================================================================*/
static gba_result_enum_type gba_mmgsdi_process_gbabp_rsp(
  const gba_mmgsdi_cmd_rsp_data_type * rsp_data_ptr
)
{
  gba_bootstrap_callback_data_type callback_data      = {0};
  uint16                           gbabp_len           = 0;
  uint16                           offset              = 0;

  /* After successful read of EF GBABP, Parse EF and extract the BTID and Lifetime
     Format of EF GBABP is,
     |Len of RAND(1) | RAND |
     |Len of BTID (1) | BTID |
     |Len of Lifetime (1) | Lifetime |

     note that in case EF GBABP is not initialized, or with invalid content,
     then we should return empty lifetime/btid back, this way will trigger a new Ub
     and EF GBABP will be updated by UICC during GBA Auth Request.
   */
   
  /* check minimum EF GBABP size */
  if (rsp_data_ptr->cnf_data.read_cnf.read_data.data_len < 3)
  {
    GBA_MSG_ERR_0("GBA EF GBABP Invalid data");
    return GBA_GENERIC_ERROR;
  }
   
  do
  {
    /* process GBABP data and parse and skip the RAND length from first byte */
    gbabp_len = rsp_data_ptr->cnf_data.read_cnf.read_data.data_ptr[offset++];
    offset = offset + gbabp_len;

    /* check BTID data len and copy 
       no need to check BTID len <= GBA_MAX_BTID_LEN as BTID len is 1 byte of uint8,
       which is always <= GBA_MAX_BTID_LEN */
    if (offset >= rsp_data_ptr->cnf_data.read_cnf.read_data.data_len ||
        /* this also include the check of lifetime len byte */
        (offset + rsp_data_ptr->cnf_data.read_cnf.read_data.data_ptr[offset] + 1 >=
           rsp_data_ptr->cnf_data.read_cnf.read_data.data_len))
    {
      GBA_MSG_ERR_0("GBA EF GBABP Invalid BTID len");
      break;
    }

    callback_data.data.ks_validity.btid.data_len =
      rsp_data_ptr->cnf_data.read_cnf.read_data.data_ptr[offset++];

    memscpy(callback_data.data.ks_validity.btid.data,
            sizeof(callback_data.data.ks_validity.btid.data),
            &rsp_data_ptr->cnf_data.read_cnf.read_data.data_ptr[offset],
            callback_data.data.ks_validity.btid.data_len);

    /* check Lifetime data len and copy */
    offset = offset + callback_data.data.ks_validity.btid.data_len;

    if (rsp_data_ptr->cnf_data.read_cnf.read_data.data_ptr[offset] > GBA_MAX_LIFETIME_LEN ||
        (offset + rsp_data_ptr->cnf_data.read_cnf.read_data.data_ptr[offset] >=
         rsp_data_ptr->cnf_data.read_cnf.read_data.data_len))
    {
      GBA_MSG_ERR_0("GBA EF GBABP Invalid BTID len");
      memset(&callback_data, 0x00, sizeof(gba_bootstrap_callback_data_type));
      break;
    }

    callback_data.data.ks_validity.lifetime.data_len =
                       rsp_data_ptr->cnf_data.read_cnf.read_data.data_ptr[offset++];
    memscpy(callback_data.data.ks_validity.lifetime.data,
            sizeof(callback_data.data.ks_validity.lifetime.data),
            &rsp_data_ptr->cnf_data.read_cnf.read_data.data_ptr[offset],
            callback_data.data.ks_validity.lifetime.data_len);
  } while(0);

  callback_data.cmd_type = GBA_RETRIEVE_KS_VALIDITY_CMD;
  (gba_mmgsdi_cmd_info.call_back)(GBA_SUCCESS,
                                  &callback_data,
                                  gba_mmgsdi_cmd_info.reference_id);

  return GBA_SUCCESS;
}/* gba_mmgsdi_process_gbabp_rsp */


/*===========================================================================
   FUNCTION:      GBA_MMGSDI_PROCESS_ISIM_AUTH_RSP

   DESCRIPTION:
     This function shall process ISIM AUTH response for various AUTH modes

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     gba_result_enum_type

   SIDE EFFECTS:
     None
===========================================================================*/
static gba_result_enum_type gba_mmgsdi_process_isim_auth_rsp(
  const gba_mmgsdi_cmd_rsp_data_type * rsp_data_ptr
)
{
  gba_bootstrap_callback_data_type   callback_data      = {0};
  gba_result_enum_type               gba_status         = GBA_GENERIC_ERROR;

  callback_data.cmd_type = GBA_UICC_AUTHENTICATE_RESPONSE_CMD;
  callback_data.data.auth_status.session_type = gba_mmgsdi_cmd_info.session_type;

  if (rsp_data_ptr->status == MMGSDI_SUCCESS)
  {
    if (gba_mmgsdi_cmd_info.auth_context == MMGSDI_AUTN_ISIM_CONTEXT)
    {
      callback_data.data.auth_status.data.auth_success.auth_mode = GBA_AUTH_CONTEXT_3G_AKA;

      if (rsp_data_ptr->cnf_data.isim_auth_cnf.res.data_ptr != NULL &&
          rsp_data_ptr->cnf_data.isim_auth_cnf.res.data_len <= GBA_RES_LEN)
      {
        memscpy(callback_data.data.auth_status.data.auth_success.auth_rsp_data.auth_3g_rsp.res.data,
                sizeof(callback_data.data.auth_status.data.auth_success.auth_rsp_data.auth_3g_rsp.res.data),
                rsp_data_ptr->cnf_data.isim_auth_cnf.res.data_ptr,
                rsp_data_ptr->cnf_data.isim_auth_cnf.res.data_len);
        callback_data.data.auth_status.data.auth_success.auth_rsp_data.auth_3g_rsp.res.data_len =
          rsp_data_ptr->cnf_data.isim_auth_cnf.res.data_len;
      }
      else
      {
        return GBA_GENERIC_ERROR;
      }

      if (rsp_data_ptr->cnf_data.isim_auth_cnf.ck.data_ptr != NULL &&
          rsp_data_ptr->cnf_data.isim_auth_cnf.ck.data_len <= GBA_CK_LEN)
      {
        memscpy(callback_data.data.auth_status.data.auth_success.auth_rsp_data.auth_3g_rsp.ck.data,
                sizeof(callback_data.data.auth_status.data.auth_success.auth_rsp_data.auth_3g_rsp.ck.data),
                rsp_data_ptr->cnf_data.isim_auth_cnf.ck.data_ptr,
                rsp_data_ptr->cnf_data.isim_auth_cnf.ck.data_len);
        callback_data.data.auth_status.data.auth_success.auth_rsp_data.auth_3g_rsp.ck.data_len =
          rsp_data_ptr->cnf_data.isim_auth_cnf.ck.data_len;
      }
      else
      {
        return GBA_GENERIC_ERROR;
      }

      if (rsp_data_ptr->cnf_data.isim_auth_cnf.ik.data_ptr != NULL &&
          rsp_data_ptr->cnf_data.isim_auth_cnf.ik.data_len <= GBA_IK_LEN)
      {
        memscpy(callback_data.data.auth_status.data.auth_success.auth_rsp_data.auth_3g_rsp.ik.data,
                sizeof(callback_data.data.auth_status.data.auth_success.auth_rsp_data.auth_3g_rsp.ik.data),
                rsp_data_ptr->cnf_data.isim_auth_cnf.ik.data_ptr,
                rsp_data_ptr->cnf_data.isim_auth_cnf.ik.data_len);
        callback_data.data.auth_status.data.auth_success.auth_rsp_data.auth_3g_rsp.ik.data_len =
          rsp_data_ptr->cnf_data.isim_auth_cnf.ik.data_len;
      }
      else
      {
        return GBA_GENERIC_ERROR;
      }
      callback_data.data.auth_status.result = GBA_AUTH_SUCCESS;
    }
    else if (gba_mmgsdi_cmd_info.auth_context == MMGSDI_AUTN_ISIM_GBA_BOOTSTRAPPING_CONTEXT)
    {
      callback_data.data.auth_status.data.auth_success.auth_mode = GBA_AUTH_CONTEXT_GBA_BOOTSTRAP;

      if (rsp_data_ptr->cnf_data.isim_auth_cnf.res.data_ptr != NULL &&
          rsp_data_ptr->cnf_data.isim_auth_cnf.res.data_len <= GBA_RES_LEN)
      {
        memscpy(callback_data.data.auth_status.data.auth_success.auth_rsp_data.auth_gba_bootstrap_rsp.data,
                sizeof(callback_data.data.auth_status.data.auth_success.auth_rsp_data.auth_gba_bootstrap_rsp.data),
                rsp_data_ptr->cnf_data.isim_auth_cnf.res.data_ptr,
                rsp_data_ptr->cnf_data.isim_auth_cnf.res.data_len);
        callback_data.data.auth_status.data.auth_success.auth_rsp_data.auth_gba_bootstrap_rsp.data_len =
          rsp_data_ptr->cnf_data.isim_auth_cnf.res.data_len;
      }
      else
      {
        return GBA_GENERIC_ERROR;
      }
      callback_data.data.auth_status.result = GBA_AUTH_SUCCESS;
    }
    else if (gba_mmgsdi_cmd_info.auth_context == MMGSDI_AUTN_ISIM_GBA_NAF_DERIVATION_CONTEXT)
    {
      callback_data.cmd_type = GBA_GEN_KEY_CMD;

      if (rsp_data_ptr->cnf_data.isim_auth_cnf.res.data_ptr != NULL &&
          rsp_data_ptr->cnf_data.isim_auth_cnf.res.data_len <= GBA_KS_NAF_LEN)
      {
        memscpy(callback_data.data.ks_ext_naf,
                sizeof(callback_data.data.ks_ext_naf),
                rsp_data_ptr->cnf_data.isim_auth_cnf.res.data_ptr,
                rsp_data_ptr->cnf_data.isim_auth_cnf.res.data_len);
      }
      else
      {
        return GBA_GENERIC_ERROR;
      }
    }
    else
    {
      return GBA_GENERIC_ERROR;
    }
    GBA_MSG_MED_0("GBA MMGSDI ISIM AUTH SUCCESS");
    gba_status = GBA_SUCCESS;
  }
  else if (rsp_data_ptr->status == MMGSDI_AUTS_FAIL)
  {
    gba_status = GBA_SUCCESS;
    callback_data.data.auth_status.result = GBA_AUTH_SYNC_FAILURE;

    if (rsp_data_ptr->cnf_data.isim_auth_cnf.auts.data_ptr != NULL &&
        rsp_data_ptr->cnf_data.isim_auth_cnf.auts.data_len <= GBA_AUTS_LEN)
    {
      memscpy(callback_data.data.auth_status.data.auth_sync_failure.auts.data,
              sizeof(callback_data.data.auth_status.data.auth_sync_failure.auts.data),
              rsp_data_ptr->cnf_data.isim_auth_cnf.auts.data_ptr,
              rsp_data_ptr->cnf_data.isim_auth_cnf.auts.data_len);
      callback_data.data.auth_status.data.auth_sync_failure.auts.data_len =
        rsp_data_ptr->cnf_data.isim_auth_cnf.auts.data_len;
    }
    else
    {
      return GBA_GENERIC_ERROR;
    }
    GBA_MSG_HIGH_0("GBA MMGSDI ISIM AUTH SYNC FAILURE");
  }
  else if (rsp_data_ptr->status == MMGSDI_AUTH_FAIL ||
           rsp_data_ptr->status == MMGSDI_AUTH_ERROR_INCORRECT_MAC)
  {
    gba_status = GBA_SUCCESS;
    callback_data.data.auth_status.result = GBA_AUTH_GENERIC_FAILURE;
  }
  else
  {
    return GBA_GENERIC_ERROR;
  }

  /* AUTH command or KS NAF derivation command  is successul,
     trigger gba_bootstrap callback - for error cases the caller handles it
     and sends out error callback */
  if (gba_status == GBA_SUCCESS)
  {
    (gba_mmgsdi_cmd_info.call_back)(gba_status,
      &callback_data, gba_mmgsdi_cmd_info.reference_id);
  }

  return GBA_SUCCESS;
}/* gba_mmgsdi_process_isim_auth_rsp */


/*===========================================================================
   FUNCTION:      GBA_MMGSDI_PROCESS_USIM_AUTH_RSP

   DESCRIPTION:
     This function shall process USIM AUTH response for various AUTH modes

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     gba_result_enum_type

   SIDE EFFECTS:
     None
===========================================================================*/
static gba_result_enum_type gba_mmgsdi_process_usim_auth_rsp(
  const gba_mmgsdi_cmd_rsp_data_type * rsp_data_ptr
)
{
  gba_result_enum_type               gba_status         = GBA_GENERIC_ERROR;
  gba_bootstrap_callback_data_type   callback_data      = {0};
  uint8                              offset             = 1;
  const mmgsdi_data_type            *auth_rsp_data_ptr  = NULL;

  callback_data.cmd_type = GBA_UICC_AUTHENTICATE_RESPONSE_CMD;
  callback_data.data.auth_status.session_type = gba_mmgsdi_cmd_info.session_type;

  auth_rsp_data_ptr = &(rsp_data_ptr->cnf_data.usim_auth_cnf.auth_response);

  if (rsp_data_ptr->status        == MMGSDI_SUCCESS &&
      auth_rsp_data_ptr->data_ptr != NULL &&
      auth_rsp_data_ptr->data_len >  0)
  {
    if (auth_rsp_data_ptr->data_ptr[0] == GBA_3G_AUTH_SUCCESS)
    {
      if(auth_rsp_data_ptr->data_len <= offset)
      {
        return GBA_GENERIC_ERROR;
      }

      if (gba_mmgsdi_cmd_info.auth_context == MMGSDI_AUTN_USIM_3G_CONTEXT)
      {
        callback_data.data.auth_status.data.auth_success.auth_mode = GBA_AUTH_CONTEXT_3G_AKA;
        /* Copy RES */
        callback_data.data.auth_status.data.auth_success.auth_rsp_data.auth_3g_rsp.res.data_len =
          auth_rsp_data_ptr->data_ptr[offset];
        offset++;
        if(callback_data.data.auth_status.data.auth_success.auth_rsp_data.auth_3g_rsp.res.data_len >
             sizeof(callback_data.data.auth_status.data.auth_success.auth_rsp_data.auth_3g_rsp.res.data))
        {
          return GBA_GENERIC_ERROR;
        }

        memscpy(callback_data.data.auth_status.data.auth_success.auth_rsp_data.auth_3g_rsp.res.data,
                sizeof(callback_data.data.auth_status.data.auth_success.auth_rsp_data.auth_3g_rsp.res.data),
                &auth_rsp_data_ptr->data_ptr[offset],
                callback_data.data.auth_status.data.auth_success.auth_rsp_data.auth_3g_rsp.res.data_len);
        offset += callback_data.data.auth_status.data.auth_success.auth_rsp_data.auth_3g_rsp.res.data_len;


        /* Copy CK */
        if(auth_rsp_data_ptr->data_len <= offset)
        {
          return GBA_GENERIC_ERROR;
        }

        callback_data.data.auth_status.data.auth_success.auth_rsp_data.auth_3g_rsp.ck.data_len =
          auth_rsp_data_ptr->data_ptr[offset];
        offset++;

        if(callback_data.data.auth_status.data.auth_success.auth_rsp_data.auth_3g_rsp.res.data_len >
             sizeof(callback_data.data.auth_status.data.auth_success.auth_rsp_data.auth_3g_rsp.res.data))
        {
          return GBA_GENERIC_ERROR;
        }

        memscpy(callback_data.data.auth_status.data.auth_success.auth_rsp_data.auth_3g_rsp.ck.data,
                sizeof(callback_data.data.auth_status.data.auth_success.auth_rsp_data.auth_3g_rsp.ck.data),
                &auth_rsp_data_ptr->data_ptr[offset],
                callback_data.data.auth_status.data.auth_success.auth_rsp_data.auth_3g_rsp.ck.data_len);
        offset += callback_data.data.auth_status.data.auth_success.auth_rsp_data.auth_3g_rsp.ck.data_len;

        /* Copy IK */
        if(auth_rsp_data_ptr->data_len <= offset)
        {
          return GBA_GENERIC_ERROR;
        }

        callback_data.data.auth_status.data.auth_success.auth_rsp_data.auth_3g_rsp.ik.data_len =
          auth_rsp_data_ptr->data_ptr[offset];
        offset++;
        if(callback_data.data.auth_status.data.auth_success.auth_rsp_data.auth_3g_rsp.res.data_len >
             sizeof(callback_data.data.auth_status.data.auth_success.auth_rsp_data.auth_3g_rsp.res.data))
        {
          return GBA_GENERIC_ERROR;
        }

        memscpy(callback_data.data.auth_status.data.auth_success.auth_rsp_data.auth_3g_rsp.ik.data,
                sizeof(callback_data.data.auth_status.data.auth_success.auth_rsp_data.auth_3g_rsp.ik.data),
                &auth_rsp_data_ptr->data_ptr[offset],
                callback_data.data.auth_status.data.auth_success.auth_rsp_data.auth_3g_rsp.ik.data_len);
        offset += callback_data.data.auth_status.data.auth_success.auth_rsp_data.auth_3g_rsp.ik.data_len;
        callback_data.data.auth_status.result = GBA_AUTH_SUCCESS;
      }
      else if (gba_mmgsdi_cmd_info.auth_context == MMGSDI_AUTN_USIM_GBA_BOOTSTRAPPING_CONTEXT)
      {
        callback_data.data.auth_status.data.auth_success.auth_mode = GBA_AUTH_CONTEXT_GBA_BOOTSTRAP;

        /* Copy RES */
        callback_data.data.auth_status.data.auth_success.auth_rsp_data.auth_gba_bootstrap_rsp.data_len =
          auth_rsp_data_ptr->data_ptr[offset];
        offset++;
        if(callback_data.data.auth_status.data.auth_success.auth_rsp_data.auth_gba_bootstrap_rsp.data_len >
             sizeof(callback_data.data.auth_status.data.auth_success.auth_rsp_data.auth_gba_bootstrap_rsp.data))
        {
          return GBA_GENERIC_ERROR;
        }

        memscpy(callback_data.data.auth_status.data.auth_success.auth_rsp_data.auth_gba_bootstrap_rsp.data,
                sizeof(callback_data.data.auth_status.data.auth_success.auth_rsp_data.auth_gba_bootstrap_rsp.data),
                &auth_rsp_data_ptr->data_ptr[offset],
                callback_data.data.auth_status.data.auth_success.auth_rsp_data.auth_gba_bootstrap_rsp.data_len);
        offset += callback_data.data.auth_status.data.auth_success.auth_rsp_data.auth_gba_bootstrap_rsp.data_len;
        callback_data.data.auth_status.result = GBA_AUTH_SUCCESS;
      }
      else if (gba_mmgsdi_cmd_info.auth_context == MMGSDI_AUTN_USIM_GBA_NAF_DERIVATION_CONTEXT)
      {
        callback_data.cmd_type = GBA_GEN_KEY_CMD;

        /* Copy ks ext naf */
        if(auth_rsp_data_ptr->data_ptr[1] >
           sizeof(callback_data.data.ks_ext_naf))
        {
          return GBA_GENERIC_ERROR;
        }

        memscpy(callback_data.data.ks_ext_naf,
                sizeof(callback_data.data.ks_ext_naf),
                &auth_rsp_data_ptr->data_ptr[2],
                auth_rsp_data_ptr->data_ptr[1]);
      }
      else
      {
        return GBA_GENERIC_ERROR;
      }
      GBA_MSG_MED_0("GBA MMGSDI USIM AUTH SUCCESS");

      gba_status = GBA_SUCCESS;
    }
    else if (rsp_data_ptr->cnf_data.usim_auth_cnf.auth_response.data_ptr[0] == GBA_AUTH_SYNC_FAIL)
    {
      if(auth_rsp_data_ptr->data_len <= offset)
      {
        return GBA_GENERIC_ERROR;
      }

      callback_data.data.auth_status.data.auth_sync_failure.auts.data_len = auth_rsp_data_ptr->data_ptr[offset];
      offset++;

      if(callback_data.data.auth_status.data.auth_sync_failure.auts.data_len >
           sizeof(callback_data.data.auth_status.data.auth_sync_failure.auts.data))
      {
        return GBA_GENERIC_ERROR;
      }

      memscpy(callback_data.data.auth_status.data.auth_sync_failure.auts.data,
              sizeof(callback_data.data.auth_status.data.auth_sync_failure.auts.data),
              &auth_rsp_data_ptr->data_ptr[offset],
              callback_data.data.auth_status.data.auth_sync_failure.auts.data_len);

      gba_status = GBA_SUCCESS;
      callback_data.data.auth_status.result = GBA_AUTH_SYNC_FAILURE;
      GBA_MSG_HIGH_0("GBA MMGSDI USIM AUTH SYNC FAILURE");

    }
  }
  else if (rsp_data_ptr->status == MMGSDI_AUTH_FAIL ||
           rsp_data_ptr->status == MMGSDI_AUTH_ERROR_INCORRECT_MAC)
  {
    gba_status = GBA_SUCCESS;
    callback_data.data.auth_status.result = GBA_AUTH_GENERIC_FAILURE;
  }
  else
  {
    return GBA_GENERIC_ERROR;
  }

  /* AUTH command or KS NAF derivation command  is successul,
     trigger gba_bootstrap callback - for error cases the caller handles it
     and sends out error callback */
  if (gba_status == GBA_SUCCESS)
  {
    (gba_mmgsdi_cmd_info.call_back)(gba_status,
      &callback_data, gba_mmgsdi_cmd_info.reference_id);
  }

  return GBA_SUCCESS;
}/* gba_mmgsdi_process_usim_auth_rsp */


/*===========================================================================
   FUNCTION:      GBA_MMGSDI_PROCESS_ISIM_IST_READ_RSP

   DESCRIPTION:
     This function shall process ISIM IST Read response

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     gba_result_enum_type

   SIDE EFFECTS:
     None
===========================================================================*/
static gba_result_enum_type gba_mmgsdi_process_isim_ist_read_rsp(
  const gba_mmgsdi_cmd_rsp_data_type * rsp_data_ptr
)
{
  mmgsdi_access_type                 file_access        = {0};
  mmgsdi_return_enum_type            mmgsdi_status      = MMGSDI_ERROR;

  /* Based on the IST Service n2 status choose GBA_ME or GBA_U */
  if (rsp_data_ptr->cnf_data.read_cnf.read_data.data_len >= 1 &&
      !(rsp_data_ptr->cnf_data.read_cnf.read_data.data_ptr[0] & 0x02))
  {
    gba_mmgsdi_cmd_info.mode = GBA_ME;
  }
  else
  {
    gba_mmgsdi_cmd_info.mode = GBA_U;
  }

  file_access.access_method = MMGSDI_EF_ENUM_ACCESS;
  file_access.file.file_enum = MMGSDI_ISIM_IMPI;
  mmgsdi_status = mmgsdi_session_read_transparent(
                    gba_mmgsdi_client_info.session_id[gba_mmgsdi_cmd_info.session_type],
                    file_access,
                    0,
                    0,
                    gba_mmgsdi_cmd_rsp_cb,gba_mmgsdi_cmd_info.reference_id);

  return gba_convert_mmgsdi_status_to_gba(mmgsdi_status);
}/* gba_mmgsdi_process_isim_ist_read_rsp */


/*===========================================================================
   FUNCTION:      GBA_MMGSDI_PROCESS_ISIM_IMPI_READ_RSP

   DESCRIPTION:
     This function shall process ISIM IMPI Read response

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     gba_result_enum_type

   SIDE EFFECTS:
     None
===========================================================================*/
static gba_result_enum_type gba_mmgsdi_process_isim_impi_read_rsp(
  const gba_mmgsdi_cmd_rsp_data_type * rsp_data_ptr
)
{
  gba_bootstrap_callback_data_type callback_data      = {0};
  uint8                            impi_len = 0;

  /* After successful read of EF IMPI, Parse EF IMPI and extract the IMPI
     Format of EF IMPI is,
     | 0x80(1) | Len of IMPI(1) | IMPI |
   */
  callback_data.cmd_type = GBA_RETRIEVE_IDENTITY_CMD;
  memscpy((void *)&callback_data.data.identity.aid, sizeof(gba_aid_type),
           (void *)&gba_mmgsdi_cmd_info.aid, sizeof(gba_aid_type));
  callback_data.data.identity.session_type = gba_mmgsdi_cmd_info.session_type;

  /* check if the IMPI tag is valid in EF IMPI */
  if (rsp_data_ptr->cnf_data.read_cnf.read_data.data_len > GBA_MAX_IMPI_LEN ||
      rsp_data_ptr->cnf_data.read_cnf.read_data.data_len < 3 ||
      rsp_data_ptr->cnf_data.read_cnf.read_data.data_ptr[0] != 0x80 ||
      rsp_data_ptr->cnf_data.read_cnf.read_data.data_ptr[1] == 0x00)
  {
    GBA_MSG_ERR_0("GBA MMGSDI ISIM EF IMPI Invalid data");
    return GBA_GENERIC_ERROR;
  }

  /* process IMPI data and parse the IMPI length from second byte */
  impi_len = rsp_data_ptr->cnf_data.read_cnf.read_data.data_ptr[1];
  if(impi_len > (rsp_data_ptr->cnf_data.read_cnf.read_data.data_len - 2))
  {
    impi_len = rsp_data_ptr->cnf_data.read_cnf.read_data.data_len - 2;
  }

  /* Copy IMPI data and send the confirmation to gba_bootstrap */
  callback_data.data.identity.impi.data_len = impi_len;
  callback_data.data.identity.mode = gba_mmgsdi_cmd_info.mode;
  memscpy(callback_data.data.identity.impi.data,
          GBA_MAX_IMPI_LEN,
          &rsp_data_ptr->cnf_data.read_cnf.read_data.data_ptr[2],
          impi_len);

  (gba_mmgsdi_cmd_info.call_back)(GBA_SUCCESS,
                                  &callback_data,
                                  gba_mmgsdi_cmd_info.reference_id);
  return GBA_SUCCESS;
}/* gba_mmgsdi_process_isim_impi_read_rsp */

/*===========================================================================
   FUNCTION:      GBA_PROCESS_MMGSDI_CMD_RSP

   DESCRIPTION:
     GBA MMGSDI processing function for MMGSDI responses

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     None

   SIDE EFFECTS:
     None
===========================================================================*/
void gba_process_mmgsdi_cmd_rsp (
  const gba_mmgsdi_cmd_rsp_data_type * rsp_data_ptr
)
{
  gba_result_enum_type               gba_status         = GBA_GENERIC_ERROR;
  mmgsdi_access_type                 file_access        = {0};
  mmgsdi_return_enum_type            mmgsdi_status      = MMGSDI_ERROR;

  if (rsp_data_ptr == NULL)
  {
    return;
  }

  /* Check if reference id matches */
  if (gba_mmgsdi_cmd_info.reference_id != (uint32)rsp_data_ptr->cnf_data.response_header.client_data)
  {
    /* Since ref id did not match, do not proceed, just drop the request */
    GBA_MSG_HIGH_3("While processing response for 0x%x, ref id 0x%x did not match with current ref id 0x%x",
                    rsp_data_ptr->cnf, gba_mmgsdi_cmd_info.reference_id,
                    rsp_data_ptr->cnf_data.response_header.client_data);
    return;
  }

  if ( rsp_data_ptr->cnf != MMGSDI_ISIM_AUTH_CNF &&
       rsp_data_ptr->cnf != MMGSDI_USIM_AUTH_CNF &&
       rsp_data_ptr->status != MMGSDI_SUCCESS)
  {
    /* for AUTH, handle error cases differently - see switch case */
    /* invoke the call back with error and cleanup cmd data */
    (gba_mmgsdi_cmd_info.call_back)(GBA_GENERIC_ERROR,
                                    NULL,
                                    gba_mmgsdi_cmd_info.reference_id);
    memset(&gba_mmgsdi_cmd_info, 0x00, sizeof(gba_mmgsdi_current_cmd_type));
    GBA_MSG_ERR_2("rsp_data_ptr->cnf 0x%x, rsp_data_ptr->status 0x%x",
                  rsp_data_ptr->cnf, rsp_data_ptr->status);
    return;
  }

  switch (rsp_data_ptr->cnf)
  {
    case MMGSDI_CLIENT_ID_AND_EVT_REG_CNF:
      GBA_MSG_LOW_0("GBA MMGSDI client id reg");
      gba_mmgsdi_client_info.gba_mmgsdi_client_id =
        rsp_data_ptr->cnf_data.client_id_and_evt_reg_cnf.response_header.client_id;
      gba_status = gba_mmgsdi_get_card_info(gba_mmgsdi_cmd_info.aid, gba_mmgsdi_cmd_info.session_type);
      break;

    case MMGSDI_SESSION_OPEN_CNF:
      gba_mmgsdi_client_info.session_id[gba_mmgsdi_cmd_info.session_type] =
       rsp_data_ptr->cnf_data.session_open_cnf.session_id;

      /* Process USIM */
      if (gba_mmgsdi_cmd_info.session_type == GBA_3GPP_PROV_SESSION_PRI ||
          gba_mmgsdi_cmd_info.session_type == GBA_3GPP_PROV_SESSION_SEC)
      {
        gba_status = gba_process_usim_session_open();
      }
      else
      {
        /* Process ISIM */
        GBA_MSG_HIGH_0("GBA MMGSDI ISIM session open done");
        file_access.access_method = MMGSDI_BY_PATH_ACCESS;
        file_access.file.path_type.path_len = 3;
        file_access.file.path_type.path_buf[0] = 0x3F00;
        file_access.file.path_type.path_buf[1] = 0x7FFF;
        file_access.file.path_type.path_buf[2] = 0x6F07;

        mmgsdi_status = mmgsdi_session_read_transparent(
                          gba_mmgsdi_client_info.session_id[gba_mmgsdi_cmd_info.session_type],
                          file_access,
                          0,
                          0,
                          gba_mmgsdi_cmd_rsp_cb,gba_mmgsdi_cmd_info.reference_id);
        gba_status = gba_convert_mmgsdi_status_to_gba(mmgsdi_status);
      }
      break;

    case MMGSDI_READ_CNF:
      if (rsp_data_ptr->cnf_data.read_cnf.access.access_method == MMGSDI_BY_PATH_ACCESS &&
          rsp_data_ptr->cnf_data.read_cnf.access.file.path_type.path_len    == 3 &&
          rsp_data_ptr->cnf_data.read_cnf.access.file.path_type.path_buf[2] == 0x6F07)
      {
        GBA_MSG_HIGH_0("GBA MMGSDI ISIM IST Read done");
        gba_status = gba_mmgsdi_process_isim_ist_read_rsp(rsp_data_ptr);
      }
      else if (rsp_data_ptr->cnf_data.read_cnf.access.access_method == MMGSDI_EF_ENUM_ACCESS &&
               rsp_data_ptr->cnf_data.read_cnf.access.file.file_enum == MMGSDI_ISIM_IMPI)
      {
        GBA_MSG_HIGH_0("GBA MMGSDI ISIM IMPI Read done");
        gba_status = gba_mmgsdi_process_isim_impi_read_rsp(rsp_data_ptr);
      }
      else if (rsp_data_ptr->cnf_data.read_cnf.access.access_method == MMGSDI_EF_ENUM_ACCESS &&
               (rsp_data_ptr->cnf_data.read_cnf.access.file.file_enum == MMGSDI_ISIM_GBABP ||
                rsp_data_ptr->cnf_data.read_cnf.access.file.file_enum == MMGSDI_USIM_GBABP))
      {
        GBA_MSG_HIGH_1("GBA MMGSDI read GBABP status 0x%x", rsp_data_ptr->status);

        if(gba_state_current() == GBA_STATE_GBABP)
        {
          /* return GBABP data back to bootstrap */
          gba_status = gba_mmgsdi_process_gbabp_rsp(rsp_data_ptr);
        }
        else
        {
          /* After a successful read on GBABP, peform updates to EF GBABP */
          gba_status = gba_mmgsdi_update_ef_gbabp(rsp_data_ptr);
        }
      }
      break;

    case MMGSDI_WRITE_CNF:
      if (rsp_data_ptr->cnf_data.write_cnf.access.access_method == MMGSDI_EF_ENUM_ACCESS &&
         (rsp_data_ptr->cnf_data.write_cnf.access.file.file_enum == MMGSDI_ISIM_GBABP ||
          rsp_data_ptr->cnf_data.write_cnf.access.file.file_enum == MMGSDI_USIM_GBABP))
      {
        /* EF GBABP Update done successfully
           Trigger KS NAF derivation with
           - NAFID and IMPI as inputs for USIM app (3GPP 31.102 Sec 7.1.2.4)
           - only NAF ID as the input for ISIM app (3gpp 31.103 sec 7.1.2.4) */
        gba_status = gba_mmgsdi_trigger_gba_u_naf_derivation();
      }
      break;

    case MMGSDI_ISIM_AUTH_CNF:
      GBA_MSG_HIGH_1("GBA MMGSDI ISIM AUTH status 0x%x", rsp_data_ptr->status);
      gba_status = gba_mmgsdi_process_isim_auth_rsp(rsp_data_ptr);
      break;

    case MMGSDI_USIM_AUTH_CNF:
      GBA_MSG_HIGH_1("GBA MMGSDI USIM AUTH status 0x%x", rsp_data_ptr->status);
      gba_status = gba_mmgsdi_process_usim_auth_rsp(rsp_data_ptr);
      break;
    default:
      break;
  }

  if (gba_status != GBA_SUCCESS)
  {
    /* invoke the call back with error and cleanup cmd data */
    (gba_mmgsdi_cmd_info.call_back)(GBA_GENERIC_ERROR,
                                    NULL,
                                    gba_mmgsdi_cmd_info.reference_id);
    memset(&gba_mmgsdi_cmd_info, 0x00, sizeof(gba_mmgsdi_current_cmd_type));
  }
}/*gba_process_mmgsdi_cmd_rsp*/


/*===========================================================================
   FUNCTION:      GBA_PROCESS_MMGSDI_EVENT

   DESCRIPTION:
     GBA MMGSDI processing function for MMGSDI Events

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     None

   SIDE EFFECTS:
     None
===========================================================================*/
void gba_process_mmgsdi_event (
  const gba_mmgsdi_evt_data_type *event_data_ptr
)
{

  GBA_MSG_HIGH_1("GBA MMGSDI received MMGSDI event 0x%x",
                 event_data_ptr->mmgsdi_evt_data.evt);

  switch (event_data_ptr->mmgsdi_evt_data.evt)
  {
    case MMGSDI_CARD_ERROR_EVT:
      if(event_data_ptr->mmgsdi_evt_data.data.card_error.slot == MMGSDI_SLOT_1)
      {
        if (gba_mmgsdi_cmd_info.session_type == GBA_NON_PROV_SESSION_SLOT_1 &&
            gba_mmgsdi_client_info.session_id[GBA_NON_PROV_SESSION_SLOT_1])
        {
          /* reset gba mmgsdi cmd info if command is in progress */
          gba_mmgsdi_client_info.session_id[gba_mmgsdi_cmd_info.session_type] = 0;
          memset(&gba_mmgsdi_cmd_info, 0x00, sizeof(gba_mmgsdi_cmd_info));
        }
        gba_storage_clear_profile(GBA_NON_PROV_SESSION_SLOT_1);
      }
      else if(event_data_ptr->mmgsdi_evt_data.data.card_error.slot == MMGSDI_SLOT_2)
      {
        if (gba_mmgsdi_cmd_info.session_type == GBA_NON_PROV_SESSION_SLOT_2 &&
            gba_mmgsdi_client_info.session_id[GBA_NON_PROV_SESSION_SLOT_2])
        {
          /* reset gba mmgsdi cmd info if command is in progress */
          gba_mmgsdi_client_info.session_id[gba_mmgsdi_cmd_info.session_type] = 0;
          memset(&gba_mmgsdi_cmd_info, 0x00, sizeof(gba_mmgsdi_cmd_info));
        }
        gba_storage_clear_profile(GBA_NON_PROV_SESSION_SLOT_2);
      }
      break;

    case MMGSDI_SESSION_CHANGED_EVT:
      if (!event_data_ptr->mmgsdi_evt_data.data.session_changed.activated)
      {
        if (gba_mmgsdi_client_info.session_id[GBA_3GPP_PROV_SESSION_PRI] ==
            event_data_ptr->mmgsdi_evt_data.session_id)
        {
          gba_storage_clear_profile(GBA_3GPP_PROV_SESSION_PRI);
        }
        else if (gba_mmgsdi_client_info.session_id[GBA_3GPP_PROV_SESSION_SEC] ==
            event_data_ptr->mmgsdi_evt_data.session_id)
        {
          gba_storage_clear_profile(GBA_3GPP_PROV_SESSION_SEC);
        }
        gba_state_reset();
      }
      break;

    default:
      return;
  }

}/* gba_process_mmgsdi_event */

/*===========================================================================
FUNCTION GBA_MMGSDI_FREE_CMD_DATA

DESCRIPTION
  This function frees the command data allocated for command processing

DEPENDENCIES
  GBA task must be finished initialization.

LIMITATIONS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void gba_mmgsdi_free_cmd_data (
  gba_mmgsdi_cmd_type          *task_cmd_ptr
)
{
  if(task_cmd_ptr == NULL)
  {
    return;
  }

  switch (task_cmd_ptr->cmd_type)
  {
    case GBA_MMGSDI_CMD_RSP:
      switch (task_cmd_ptr->data.rsp_data.cnf)
      {
        case MMGSDI_READ_CNF:
          GBA_FREE(task_cmd_ptr->data.rsp_data.cnf_data.read_cnf.read_data.data_ptr);
          break;
        case MMGSDI_ISIM_AUTH_CNF:
          GBA_FREE(task_cmd_ptr->data.rsp_data.cnf_data.isim_auth_cnf.res.data_ptr);
          GBA_FREE(task_cmd_ptr->data.rsp_data.cnf_data.isim_auth_cnf.ck.data_ptr);
          GBA_FREE(task_cmd_ptr->data.rsp_data.cnf_data.isim_auth_cnf.ik.data_ptr);
          GBA_FREE(task_cmd_ptr->data.rsp_data.cnf_data.isim_auth_cnf.auts.data_ptr);
          break;
        case MMGSDI_USIM_AUTH_CNF:
          GBA_FREE(task_cmd_ptr->data.rsp_data.cnf_data.usim_auth_cnf.auth_response.data_ptr);
          break;
        default:
          break;
      }
      break;
    case GBA_MMGSDI_EVENT_RPT:
      /* Nothing to free for now */
      break;
    default:
      break;
  }

}/*gba_mmgsdi_free_cmd_data*/


/*===========================================================================
FUNCTION GBA_MMGSDI_HANDLE_CMD_SIG

DESCRIPTION
  This function, called in gba_main, is called to process an MMGSDI event

DEPENDENCIES
  GBA task must be finished initialization.

LIMITATIONS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void gba_mmgsdi_handle_cmd_sig (
  void
)
{
  gba_mmgsdi_cmd_type          * task_cmd_ptr  = NULL;

  task_cmd_ptr = (gba_mmgsdi_cmd_type*)(q_get(&gba_mmgsdi_cmd_q));

  /* If there is a command on either of the queues */
  if(task_cmd_ptr != NULL)
  {
    GBA_MSG_HIGH_1("GBA_MMGSDI_CMD_SIG received, cmd_type 0x%x",
                   task_cmd_ptr->cmd_type);

    switch (task_cmd_ptr->cmd_type)
    {
      case GBA_MMGSDI_CMD_RSP:
        gba_process_mmgsdi_cmd_rsp(&task_cmd_ptr->data.rsp_data);
        break;
      case GBA_MMGSDI_EVENT_RPT:
        gba_process_mmgsdi_event(&task_cmd_ptr->data.event_data);
        break;
      default:
        break;
    }
  }

  (void) rex_clr_sigs(rex_self(), GBA_MMGSDI_CMD_SIG);

  /* Set the signal if there are one or more commands to look at */
  if (q_cnt(&gba_mmgsdi_cmd_q) > 0)
  {
    /*  Items on the queue. Set the signal */
    (void) rex_set_sigs(rex_self(), GBA_MMGSDI_CMD_SIG);
  }

  gba_mmgsdi_free_cmd_data(task_cmd_ptr);
  GBA_FREE(task_cmd_ptr);
} /* gba_mmgsdi_handle_cmd_sig */


/*===========================================================================
   FUNCTION:      GBA_MMGSDI_INIT

   DESCRIPTION:
     GBA MMGSDI init function called during task init

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     None

   SIDE EFFECTS:
     None
===========================================================================*/
void gba_mmgsdi_init (
)
{
   memset(&gba_mmgsdi_cmd_info, 0x00, sizeof(gba_mmgsdi_current_cmd_type));
   memset(&gba_mmgsdi_client_info, 0x00, sizeof(gba_mmgsdi_client_type));

  (void) q_init(&gba_mmgsdi_cmd_q);
}/*gba_mmgsdi_init*/


/*===========================================================================
   FUNCTION:      GBA_MMGSDI_RETRIEVE_IDENTITY

   DESCRIPTION:
     GBA_MMGSDI API triggered by identity  module to extract the IMPI from MMGSDI

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     gba_result_enum_type

   SIDE EFFECTS:
     None
===========================================================================*/
gba_result_enum_type gba_mmgsdi_retrieve_identity (
  gba_aid_type                  aid,
  gba_session_type              session_type,
  gba_bootstrap_callback_type   call_back,
  uint32                        reference_id
)
{
  mmgsdi_return_enum_type    mmgsdi_status  = MMGSDI_ERROR;

  if (call_back == NULL)
  {
    return GBA_GENERIC_ERROR;
  }

  gba_mmgsdi_cmd_info.aid               = aid;
  gba_mmgsdi_cmd_info.session_type      = session_type;
  gba_mmgsdi_cmd_info.call_back         = call_back;
  gba_mmgsdi_cmd_info.reference_id      = reference_id;

  GBA_MSG_HIGH_2("GBA MMGSDI Retrive identity request session type:0x%x, ref_id:0x%x",
                  session_type,reference_id);

  if(!gba_mmgsdi_client_info.gba_mmgsdi_client_id)
  {
     /*cliend id reg */
    mmgsdi_status = mmgsdi_client_id_and_evt_reg(
                      gba_mmgsdi_evt_cb,
                      gba_mmgsdi_cmd_rsp_cb,
                      (mmgsdi_client_data_type)reference_id);

    return gba_convert_mmgsdi_status_to_gba(mmgsdi_status);
  }

  return gba_mmgsdi_get_card_info(aid, session_type);
} /* gba_mmgsdi_retrieve_identity */


/*===========================================================================
   FUNCTION:      GBA_MMGSDI_READ_EF_GBABP

   DESCRIPTION:
     GBA_MMGSDI API to read EF GBABP(applicable to GBA_U only)

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     gba_result_enum_type

   SIDE EFFECTS:
     None
===========================================================================*/
gba_result_enum_type gba_mmgsdi_read_ef_gbabp (
  uint32                        reference_id,
  gba_bootstrap_callback_type   call_back
)
{
  mmgsdi_return_enum_type       mmgsdi_status = MMGSDI_ERROR;
  mmgsdi_access_type            file_access   = {0};

  if (call_back == NULL)
  {
    return GBA_GENERIC_ERROR;
  }

  /* Check Referene id */
  if (gba_mmgsdi_cmd_info.reference_id != reference_id ||
      gba_mmgsdi_cmd_info.mode != GBA_U)
  {
    GBA_MSG_ERR_3("Reference id did not match 0x%x, 0x%x or invalid GBA mode 0x%x",
                  gba_mmgsdi_cmd_info.reference_id,
                  reference_id,
                  gba_mmgsdi_cmd_info.mode);

    return GBA_GENERIC_ERROR;
  }

  gba_mmgsdi_cmd_info.call_back = call_back;

  /* set EF GBABP file enum */
  if (gba_mmgsdi_cmd_info.session_type == GBA_NON_PROV_SESSION_SLOT_1 ||
      gba_mmgsdi_cmd_info.session_type == GBA_NON_PROV_SESSION_SLOT_2)
  {
    file_access.file.file_enum = MMGSDI_ISIM_GBABP;
  }
  else if (gba_mmgsdi_cmd_info.session_type == GBA_3GPP_PROV_SESSION_PRI ||
           gba_mmgsdi_cmd_info.session_type == GBA_3GPP_PROV_SESSION_SEC)
  {
    file_access.file.file_enum = MMGSDI_USIM_GBABP;
  }
  else
  {
    GBA_MSG_ERR_0("Invalid session type");
    return GBA_GENERIC_ERROR;
  }

  file_access.access_method = MMGSDI_EF_ENUM_ACCESS;

  mmgsdi_status = mmgsdi_session_read_transparent(
                    gba_mmgsdi_client_info.session_id[gba_mmgsdi_cmd_info.session_type],
                    file_access,
                    0,
                    0,
                    gba_mmgsdi_cmd_rsp_cb,
                    gba_mmgsdi_cmd_info.reference_id);

  return gba_convert_mmgsdi_status_to_gba(mmgsdi_status);
} /* gba_mmgsdi_read_ef_gbabp */


/*===========================================================================
   FUNCTION:      GBA_MMGSDI_AUTH

   DESCRIPTION:
     GBA_MMGSDI API triggered by identity module to execute ISIM or USIM AUTH

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     gba_result_enum_type

   SIDE EFFECTS:
     None
===========================================================================*/
gba_result_enum_type gba_mmgsdi_auth (
  gba_auth_type                * auth_data_ptr,
  gba_bootstrap_callback_type    call_back,
  uint32                         reference_id,
  gba_auth_context_enum_type     auth_mode
)
{
  mmgsdi_return_enum_type       mmgsdi_status = MMGSDI_ERROR;
  mmgsdi_data_type              auth_req      = {0, NULL};
  uint8                         session_index = 0;
  gba_result_enum_type          gba_status    = GBA_SUCCESS;
  mmgsdi_autn_context_enum_type auth_context  = MMGSDI_USIM_AUTN_MAX;

  GBA_MSG_HIGH_1("GBA MMGSDI AUTH ref id: 0x%x",reference_id);

  if (call_back     == NULL ||
      auth_data_ptr == NULL)
  {
    return GBA_GENERIC_ERROR;
  }

  /* Check Referene id */
  if (gba_mmgsdi_cmd_info.reference_id != reference_id)
  {
    GBA_MSG_ERR_2("Reference id did not match 0x%x, 0x%x",
                   gba_mmgsdi_cmd_info.reference_id ,reference_id);
    return GBA_GENERIC_ERROR;
  }
  gba_mmgsdi_cmd_info.call_back = call_back;

  auth_req.data_len = auth_data_ptr->data_len;
  auth_req.data_ptr = auth_data_ptr->data;

  gba_status = gba_util_get_session_index_and_mmgsdi_session_type(gba_mmgsdi_cmd_info.session_type,
                                                                  &session_index,
                                                                  NULL);
  if(gba_status != GBA_SUCCESS)
  {
    return gba_status;
  }

  if (gba_mmgsdi_cmd_info.session_type == GBA_NON_PROV_SESSION_SLOT_1 ||
      gba_mmgsdi_cmd_info.session_type == GBA_NON_PROV_SESSION_SLOT_2)
  {
    if (auth_mode == GBA_AUTH_CONTEXT_3G_AKA)
    {
      auth_context = MMGSDI_AUTN_ISIM_CONTEXT;
    }
    else if (auth_mode == GBA_AUTH_CONTEXT_GBA_BOOTSTRAP)
    {
      auth_context = MMGSDI_AUTN_ISIM_GBA_BOOTSTRAPPING_CONTEXT;
    }
    else
    {
      return GBA_GENERIC_ERROR;
    }

    mmgsdi_status = mmgsdi_session_uicc_authenticate(
                      gba_mmgsdi_client_info.session_id[session_index],
                      auth_context,
                      auth_req,
                      gba_mmgsdi_cmd_rsp_cb,
                      (mmgsdi_client_data_type)reference_id);
  }
  else if (gba_mmgsdi_cmd_info.session_type == GBA_3GPP_PROV_SESSION_PRI ||
           gba_mmgsdi_cmd_info.session_type == GBA_3GPP_PROV_SESSION_SEC)
  {
    if (auth_mode == GBA_AUTH_CONTEXT_3G_AKA)
    {
      auth_context = MMGSDI_AUTN_USIM_3G_CONTEXT;
    }
    else if (auth_mode == GBA_AUTH_CONTEXT_GBA_BOOTSTRAP)
    {
      auth_context = MMGSDI_AUTN_USIM_GBA_BOOTSTRAPPING_CONTEXT;
    }
    else
    {
      return GBA_GENERIC_ERROR;
    }

    mmgsdi_status = mmgsdi_session_usim_authenticate(
                      gba_mmgsdi_client_info.session_id[session_index],
                      auth_context,
                      auth_req,
                      gba_mmgsdi_cmd_rsp_cb,
                      (mmgsdi_client_data_type)reference_id);

  }

  gba_mmgsdi_cmd_info.auth_context = auth_context;

  return gba_convert_mmgsdi_status_to_gba(mmgsdi_status);
}/*gba_mmgsdi_auth*/


/*===========================================================================
   FUNCTION:      GBA_MMGSDI_CLEANUP

   DESCRIPTION:
     GBA_MMGSDI API to cleanup after the Algo is complete

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     gba_result_enum_type

   SIDE EFFECTS:
     None
===========================================================================*/
gba_result_enum_type gba_mmgsdi_cleanup(
  void
)
{
  mmgsdi_return_enum_type mmgsdi_status = MMGSDI_ERROR;

  GBA_MSG_HIGH_0("GBA MMGSDI Cleanup");

  /* close only ISIM session and not USIM */
  /* for USIM we need to listen to SESSION_CHANGED (false) event */
  if ((gba_mmgsdi_cmd_info.session_type == GBA_NON_PROV_SESSION_SLOT_1 ||
       gba_mmgsdi_cmd_info.session_type == GBA_NON_PROV_SESSION_SLOT_2 ) &&
      gba_mmgsdi_client_info.session_id[gba_mmgsdi_cmd_info.session_type])
  {
    /* handle cleanup of ISIM session if open*/
    mmgsdi_status = mmgsdi_session_close(gba_mmgsdi_client_info.session_id[gba_mmgsdi_cmd_info.session_type],
                                         gba_mmgsdi_cmd_rsp_cb,0);
  }

  /* reset gba mmgsdi cmd info */
  gba_mmgsdi_client_info.session_id[gba_mmgsdi_cmd_info.session_type] = 0;
  memset(&gba_mmgsdi_cmd_info, 0x00, sizeof(gba_mmgsdi_cmd_info));

  return gba_convert_mmgsdi_status_to_gba(mmgsdi_status);
}/*gba_sim_cleanup*/


/*===========================================================================
   FUNCTION:      GBA_CONVERT_MMGSDI_STATUS_TO_GBA

   DESCRIPTION:
     GBA mmgsdi utility to convert MMGSDI Status to GBA status

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     gba_result_enum_type

   SIDE EFFECTS:
     None
===========================================================================*/
gba_result_enum_type gba_convert_mmgsdi_status_to_gba(
  mmgsdi_return_enum_type   mmgsdi_status
)
{
  switch (mmgsdi_status)
  {
    case MMGSDI_SUCCESS:
      return GBA_SUCCESS;

    case MMGSDI_ERROR:
      return GBA_GENERIC_ERROR;

    default:
      break;
  }

  return GBA_GENERIC_ERROR;
} /* gba_convert_mmgsdi_status_to_gba */


/*===========================================================================
   FUNCTION:      GBA_MMGSDI_GENERATE_KS_EXT_NAF

   DESCRIPTION:
     Sends required data to the card to authenticate and construct the
     KS EXT NAF.

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     gba_result_enum_type

   SIDE EFFECTS:
     None
===========================================================================*/
gba_result_enum_type gba_mmgsdi_generate_ks_ext_naf(
  uint32                        reference_id,
  gba_bootstrap_callback_type   call_back,
  const gba_naf_id_type       * naf_id_ptr,
  const gba_impi_type         * impi_ptr,
  const gba_btid_type         * btid_ptr,
  const gba_lifetime_type     * lifetime_ptr,
  boolean                       tmpi_generation
)
{
  GBA_MSG_HIGH_1("GBA MMGSDI GENERATE KS NAF ref id: 0x%x", reference_id);

  if (call_back    == NULL ||
      naf_id_ptr   == NULL ||
      impi_ptr     == NULL ||
      btid_ptr     == NULL ||
      lifetime_ptr == NULL)
  {
    return GBA_GENERIC_ERROR;
  }

  if (gba_mmgsdi_cmd_info.mode == GBA_ME)
  {
    return GBA_GENERIC_ERROR;
  }

  /* Check Referene id */
  if (gba_mmgsdi_cmd_info.reference_id != reference_id)
  {
    GBA_MSG_ERR_2("Reference id did not match 0x%x, 0x%x",
                   gba_mmgsdi_cmd_info.reference_id ,reference_id);
    return GBA_GENERIC_ERROR;
  }

  gba_mmgsdi_cmd_info.call_back = call_back;

  /* Store impi and naf id */
  gba_mmgsdi_cmd_info.impi.data_len = impi_ptr->data_len;
  memscpy(gba_mmgsdi_cmd_info.impi.data,
          GBA_MAX_IMPI_LEN,
          impi_ptr->data,
          impi_ptr->data_len);

  gba_mmgsdi_cmd_info.naf_id.fqdn.data_len = naf_id_ptr->fqdn.data_len;
  memscpy(gba_mmgsdi_cmd_info.naf_id.fqdn.data,
          GBA_MAX_NAF_FQDN_LEN,
          naf_id_ptr->fqdn.data,
          naf_id_ptr->fqdn.data_len);

  memscpy(gba_mmgsdi_cmd_info.naf_id.security_protocol,
          sizeof(gba_mmgsdi_cmd_info.naf_id.security_protocol),
          naf_id_ptr->security_protocol,
          sizeof(naf_id_ptr->security_protocol));

  gba_mmgsdi_cmd_info.btid.data_len = btid_ptr->data_len;
  memscpy(gba_mmgsdi_cmd_info.btid.data,
          GBA_MAX_BTID_LEN,
          btid_ptr->data,
          btid_ptr->data_len);

  gba_mmgsdi_cmd_info.lifetime.data_len = lifetime_ptr->data_len;
  memscpy(gba_mmgsdi_cmd_info.lifetime.data,
          GBA_MAX_LIFETIME_LEN,
          lifetime_ptr->data,
          lifetime_ptr->data_len);

  /* Reading of EF GBABP is not required while generating TMPI */
  if(tmpi_generation == TRUE)
  {
    return gba_mmgsdi_trigger_gba_u_naf_derivation();
  }

  /* Read EF GBABP */
  return gba_mmgsdi_read_ef_gbabp(reference_id, call_back);
} /* gba_mmgsdi_generate_ks_ext_naf */

