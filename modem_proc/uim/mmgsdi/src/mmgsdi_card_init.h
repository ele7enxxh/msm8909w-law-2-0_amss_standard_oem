#ifndef MMGSDI_CARD_INIT_H
#define MMGSDI_CARD_INIT_H
/*===========================================================================


           M M G S D I  C A R D  I N I T I A L I Z A T I O N   H E A D E R S


===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2011-2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/mmgsdi/src/mmgsdi_card_init.h#1 $$ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
10/25/16   dd      Added support for auto reg SMS when ESN is changed
07/08/16   av      Send SELECT ISDR to determine if card is eUICC or not
06/27/16   bcho    Handle read failures for AUTH related EFs
06/20/16   ar      Don't RESET card if EF-DIR req is not processed by UIMDRV
01/07/16   kv      UIM PSM handling updates
09/07/15   av      Migrate to mcfg EFS APIs
07/09/15   ar      Validate PRL at diff steps if SD task not ready
09/29/14   tkl     Added support for persistent cache
08/22/14   tl      Remove unused preprocessor macros
07/15/14   kk      NV Refresh: MCFG - MMGSDI interactions
06/30/14   vv      Remove Perso code
06/18/14   av      Validate cdmahome EF attributes during power up
01/16/14   df      Add mmgsdi_card_init_check_ef_phase to header
12/13/13   vdc     Ignore error during validation of EF-ACC due to b3 in byte 1
11/05/13   tkl     Add IMSI check for TDS test SIM
10/02/13   av      Do not activate onchip for non-test IMSIs
09/23/13   df      Support for SIM busy
05/21/13   vdc     Fix compilation warnings for memory optimizations
04/16/13   tl      Clean up of radio access features from MMGSDI code
04/15/13   tl      Remove obsoleted featurization
04/04/13   bcho    Prototype added for mmgsdi_card_init_end_refresh_in_progress
10/31/12   tl      CSIM conformance updating EF MECRP
10/30/12   tl      CSIM conformance updating EF MODEL
03/21/12   bcho    Macro MMGSDI_HALT_SUBSCRIPTION_NV_PATH added
02/18/12   av      Cleaned up remaining ALWAYS ON features
12/21/11   kk      Added USIM ECC caching support
12/21/11   shr     Initial version

=============================================================================*/

/*=============================================================================

                     INCLUDE FILES FOR MODULE

=============================================================================*/

#include "uim_variation.h"
#include "comdef.h"
#include "mmgsdilib_p.h"
#include "mmgsdi.h"

/*=============================================================================

                       DATA DECLARATIONS

=============================================================================*/
/* Constants related to the proactive SIM service table entry */
#define MMGSDI_SST_PROACTIVE_SIM_OFFSET 0x07
#define MMGSDI_SST_PROACTIVE_MASK       0x03

#define MMGSDI_CDMA_SST_TOOLKIT_OFFSET  0x07
#define MMGSDI_CDMA_SST_TOOLKIT_MASK    0x03

/* Constants related to MCC verification */
#define MMGSDI_IMSI_HIGH_BYTE_MASK      0xF0
#define MMGSDI_IMSI_LOW_BYTE_MASK       0x0F

/* Size of EF ACC data in bytes */
#define MMGSDI_EF_ACC_LEN               2

/* EF ACC bit 3 mask */
#define MMGSDI_EF_ACC_BIT3_MASK         0x04

/* Constants related to HZI */
#define MMGSDI_HZI_IMSI_FIRST_BYTE      0x2F
#define MMGSDI_HZI_IMSI_SECOND_BYTE     0x26
#define MMGSDI_HZI_IMSI_THIRD_BYTE      0x70
#define MMGSDI_HZI_IMSI_THIRD_BYTE_ALT  0x80
#define MMGSDI_HZI_ACTIVATED            0x01
#define MMGSDI_HZI_SINGLE_MODE_SETTING  0x02
#define MMGSDI_HZI_DUAL_MODE_SETTING    0x01

/* Starting index values of EF MECRP */
#define MMGSDI_ME_CRP_SCM               0
#define MMGSDI_ME_CRP_MOB_REV           1
#define MMGSDI_ME_CRP_LOCAL_CONTROL     2
#define MMGSDI_ME_CRP_EF_SIZE           3

/* Starting index values of EF Device Model Info */
#define MMGSDI_MODEL_CHAR_ENCODE        0
#define MMGSDI_MODEL_LANG_IND           1
#define MMGSDI_MODEL_INFORMATION        2
#define MMGSDI_MODEL_MANUFACTURER_NAME  34
#define MMGSDI_MODEL_VERSION_INFO       66
#define MMGSDI_MODEL_EF_SIZE            126

/* Default value for CSIM EF parameter */
#define MMGSDI_CSIM_CHAR_ENCODE_DEFAULT 0

/* Default Size of USIM EF KEYS/KEYSPS data in bytes */
#define MMGSDI_USIM_EF_KEYS_LEN         33

/* Default Size of GSM EF KC/KCGPRS data in bytes */
#define MMGSDI_GSM_EF_KEYS_LEN           9

/* Default Size of USIM EF EPSNSC data in bytes */
#define MMGSDI_EF_EPSNSC_LEN            54

/* Default Size of USIM EF EPSLOCI data in bytes */
#define MMGSDI_EF_EPSLOCI_LEN           18


/*=============================================================================

                       STRUCTURE DECLARATIONS

=============================================================================*/
typedef mmgsdi_return_enum_type (*mmgsdi_init_cache_func_type) (
  mmgsdi_session_id_type    session_id,
  mmgsdi_app_enum_type      app_type,
  mmgsdi_slot_id_enum_type  slot,
  mmgsdi_file_enum_type     file,
 mmgsdi_data_type         *data_ptr);

typedef mmgsdi_return_enum_type (*mmgsdi_init_validation_func_type) (
  mmgsdi_session_id_type   session_id,
  mmgsdi_return_enum_type  mmgsdi_status,
  const mmgsdi_data_type  *data_ptr);

/* ===========================================================================
   STRUCTURE:      MMGSDI_CACHE_INIT_ORDER_TYPE

   DESCRIPTION:
     This structure contains information on the file to be cached
===========================================================================*/
typedef struct {
  mmgsdi_file_enum_type            mmgsdi_file;
  boolean                          mandatory;
  mmgsdi_init_cache_func_type      init_cache_func;
  mmgsdi_init_validation_func_type validation_func;
}mmgsdi_cache_init_order_type;

/*=============================================================================

                                   MMGSDI MACROS

=============================================================================*/


/*=============================================================================

                       FUNCTION PROTOTYPES

=============================================================================*/

#ifdef FEATURE_GSTK
/*===========================================================================
FUNCTION   MMGSDI_CARD_INIT_GSTK_TERMINAL_PROFILE_DOWNLOAD

DESCRIPTION
  This function is used to notfiy GSTK to perform a Terminal Profile
  download

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  Triggers the GSTK task to send down a Terminal Profile Download
===========================================================================*/
void mmgsdi_card_init_gstk_terminal_profile_download (
  mmgsdi_session_id_type   session_id,
  mmgsdi_slot_id_enum_type slot,
  mmgsdi_app_enum_type     app_type
);
#endif /* FEATURE_GSTK */

/*=============================================================================
FUNCTION:      MMGSDI_CARD_INIT_GET_FILE_ATTR

DESCRIPTION
  This function build a synchronous command to UIM to obtain the file
  attribute.  Should be used for initialization purposes only.

DEPENDENCIES
  None

LIMITATIONS
  NONE

RETURN VALUE
  mmgsdi_return_enum_type
  MMGSDI_SUCCESS          - Command processed,
  MMGSDI_INCORRECT_PARAMS - Wrong input parameters provided to function
  MMGSDI_ERROR            - Other unknown failure

SIDE EFFECTS
  NONE
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_card_init_get_file_attr(
  mmgsdi_session_id_type           session_id,
  mmgsdi_app_enum_type             app_type,
  mmgsdi_slot_id_enum_type         slot,
  mmgsdi_file_enum_type            file,
  mmgsdi_file_status_type         *file_status_ptr,
  mmgsdi_rec_num_type             *num_of_rec_ptr,
  mmgsdi_len_type                 *rec_len_ptr,
  boolean                         *increase_allowed_ptr,
  mmgsdi_file_structure_enum_type *file_type_ptr
);

/*=============================================================================
FUNCTION:      MMGSDI_CARD_INIT_CACHE_BINARY

DESCRIPTION
  This function will initialize data in the cache during the initialization
  process for a binary file. The third argument data_ptr is used to retrieve
  the value of the file: this function allocates the required memory and the
  caller is responsible for freeing it. data_ptr can be NULL in case the
  value is not required.

DEPENDENCIES
  None

LIMITATIONS
  NONE

RETURN VALUE
  mmgsdi_return_enum_type
  MMGSDI_SUCCESS          - Command processed,
  MMGSDI_INCORRECT_PARAMS - Wrong input parameters provided to function
  MMGSDI_ERROR            - Other unknown failure

SIDE EFFECTS
  NONE
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_card_init_cache_binary(
  mmgsdi_session_id_type    session_id,
  mmgsdi_app_enum_type      app_type,
  mmgsdi_slot_id_enum_type  slot,
  mmgsdi_file_enum_type     file,
  mmgsdi_data_type         *data_ptr
);

/*=============================================================================
FUNCTION:      MMGSDI_CARD_INIT_CACHE_RECORD

DESCRIPTION
  This function will initialize data in the cache during the initialization
  process for a record based file.

DEPENDENCIES
  None

LIMITATIONS
  NONE

RETURN VALUE
  mmgsdi_return_enum_type
  MMGSDI_SUCCESS          - Command processed,
  MMGSDI_INCORRECT_PARAMS - Wrong input parameters provided to function
  MMGSDI_ERROR            - Other unknown failure

SIDE EFFECTS
  NONE
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_card_init_cache_record(
  mmgsdi_session_id_type    session_id,
  mmgsdi_app_enum_type      app_type,
  mmgsdi_slot_id_enum_type  slot,
  mmgsdi_file_enum_type     file,
  mmgsdi_data_type         *data_ptr
);

/*===========================================================================
FUNCTION:      MMGSDI_CARD_INIT_PROCESS_LINK_EST

DESCRIPTION:
  This function will handle the Link Established notification

DEPENDENCIES:
  NONE

LIMITATIONS:
  NONE

RETURN VALUE:
  mmgsdi_return_enum_type
  MMGSDI_SUCCESS:          The command processing was successful.
  MMGSDI_ERROR:            The command processing was not successful.
  MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                           within appropriate ranges.
SIDE EFFECTS:
  NONE
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_card_init_process_link_est(
  const mmgsdi_notify_link_est_req_type *req_ptr
);

/*===========================================================================
FUNCTION:      MMGSDI_CARD_INIT_PROCESS_CARD_ERROR

DESCRIPTION:
  This function will handle the Card Error indication

DEPENDENCIES:
  NONE

LIMITATIONS:
  NONE

RETURN VALUE:
  mmgsdi_return_enum_type
  MMGSDI_SUCCESS:          The command processing was successful.
  MMGSDI_ERROR:            The command processing was not successful.
  MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
SIDE EFFECTS:
  NONE
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_card_init_process_card_error(
  const mmgsdi_notify_card_error_req_type *req_ptr
);

/*===========================================================================
FUNCTION:      MMGSDI_CARD_INIT_PROCESS_SIM_BUSY

DESCRIPTION:
  This function will handle the SIM busy message originating from the UIM
  driver.

  If the sim busy status of a slot has changed then the function saves the new
  status internally and sends a sim busy event to all MMGSDI clients

DEPENDENCIES:
  NONE

LIMITATIONS:
  NONE

RETURN VALUE:
  mmgsdi_return_enum_type
  MMGSDI_SUCCESS:          The command processing was successful.
  MMGSDI_ERROR:            The command processing was not successful.
  MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
SIDE EFFECTS:
  NONE
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_card_init_process_sim_busy(
  const mmgsdi_notify_sim_busy_req_type *req_ptr
);

/*===========================================================================
FUNCTION MMGSDI_CARD_INIT_BUILD_INTERNAL_PUP_REQ

DESCRIPTION
  Function to build the MMGSDI Internal power up Request and put into the
  MMGSDI  command queue so that MMGSDI can be kicked into the various
  card and provisioning application initialization procedures

DEPENDENCIES
  NONE

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_card_init_build_internal_pup_req(
  mmgsdi_session_id_type      session_id,
  mmgsdi_slot_id_enum_type    slot_id,
  mmgsdi_events_enum_type     evt,
  mmgsdi_card_state_enum_type card_state,
  mmgsdi_app_state_enum_type  app_state
);

/*===========================================================================
FUNCTION:      MMGSDI_CARD_INIT_PROCESS_INTERNAL_PUP

DESCRIPTION:
  This function will handle the Internal Power-up requests w.r.t. card and
  provsioning application initialization

DEPENDENCIES:
  NONE

LIMITATIONS:
  NONE

RETURN VALUE:
  mmgsdi_return_enum_type
  MMGSDI_SUCCESS:          The command processing was successful.
  MMGSDI_ERROR:            The command processing was not successful.
  MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                           within appropriate ranges.
SIDE EFFECTS:
  NONE
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_card_init_process_internal_pup(
  const mmgsdi_internal_pup_init_req_type *req_ptr
);

/*===========================================================================
FUNCTION:      MMGSDI_CARD_INIT_END_REFRESH_IN_PROGRESS

DESCRIPTION:
  This function is used to end any REFRESH in progress on the application
  whose initalization has failed.

DEPENDENCIES:
  NONE

LIMITATIONS:
  NONE

RETURN VALUE:
  mmgsdi_return_enum_type

SIDE EFFECTS:
  NONE
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_card_init_end_refresh_in_progress(
  mmgsdi_session_id_type   session_id,
  mmgsdi_slot_id_enum_type slot_id
);

/*===========================================================================
FUNCTION:      MMGSDI_CARD_INIT_POPULATE_APP_PIN_TABLE

DESCRIPTION
  This function will populate the MMGSDI App table.

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_card_init_populate_app_pin_table(
  mmgsdi_slot_id_enum_type        slot_id,
  uint32                          app_index,
  mmgsdi_key_ref_values_enum_type slot_pin2_key_ref
);

/*===========================================================================
FUNCTION:      MMGSDI_CARD_INIT_IS_IMSI_TEST_IMSI

DESCRIPTION
  This function compares the MCC value in IMSI against set of reserved MCC
  values to figure out if it is a test IMSI.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: If it is  a test IMSI
  FALSE: Otherwise

SIDE EFFECTS
  None
===========================================================================*/
boolean mmgsdi_card_init_is_imsi_test_imsi(
  mmgsdi_imsi_data_type     imsi
);

/*===========================================================================
FUNCTION:      MMGSDI_CARD_INIT_CHECK_EF_PHASE

DESCRIPTION
  Read and cache the EF Phase.

DEPENDENCIES
  None

RETURNS
  mmgsdi_return_enum_type
    MMGSDI_SUCCESS   - Successfully updated the slot's sim_phase
    MMGSDI_NOT_FOUND - EF Phase was not on the card and so sim_phase was
                       determined from IMSI attributes.
    MMGSDI_ERROR     - The slot's sim_phase could not be updated.

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_card_init_check_ef_phase(
  mmgsdi_session_id_type    session_id,
  mmgsdi_slot_id_enum_type  slot_id,
  mmgsdi_app_enum_type      app_type
);

/*===========================================================================
FUNCTION:      MMGSDI_CARD_INIT_TRIGGER_TP_DL_FOR_UICC

DESCRIPTION
  This function will trigger TP DL for UICC card.

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_card_init_trigger_tp_dl_for_uicc(
  mmgsdi_slot_id_enum_type        slot_id,
  boolean                        *fail_reason_card_error_ptr,
  boolean                        *ef_dir_not_found_ptr
);

/*===========================================================================
FUNCTION:      MMGSDI_CARD_INIT_1X_READ_PRL_INIT

DESCRIPTION
  This function initializes the PRL cache data

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_card_init_1x_read_prl_init(
  mmgsdi_session_id_type session_id
);

#ifdef FEATURE_MMGSDI_3GPP2
/*===========================================================================
FUNCTION:      MMGSDI_CARD_INIT_STORE_ESN

DESCRIPTION
  This function builds the store ESN command to the card based on NV and card
  MEID and ESN support.
  Upon successfully sending the store ESN command, it also initializes the
  cache for UIM ID Usage file.
  The procedure for ESN support is as follow:
  1) If ME assigns MEID and the card's service table bit #9 (MEID support) is
     enabled, the ME should issue a Store ESN with MEID (i.e., P1 == 1)
  2) Otherwise, the ME should issue a Store ESN with ESN (i.e., P1 == 0)
     Note, if ME assigns MEID but the card's service table bit #9 is not allocated
     and activated, the ME should store the ESN with pseudo ESN.  (ESN and pseudo
     ESN is abstracted from NV client)

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_card_init_store_esn(
  mmgsdi_session_store_esn_req_type *store_esn_req_ptr
);
#endif /* FEATURE_MMGSDI_3GPP2 */
#endif /* MMGSDI_CARD_INIT_H */
