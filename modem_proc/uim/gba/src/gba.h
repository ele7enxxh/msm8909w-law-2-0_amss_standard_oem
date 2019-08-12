#ifndef GBA_H
#define GBA_H
/*===========================================================================


                  G B A   D E F I N I T I O N S

                      A N D   F U N C T I O N S


===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2015-2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     -----------------------------------------------------------
03/28/16   tkl     Added support of DSS network down reason in GBA response
07/09/15   tkl     Added support for custom fields
07/09/15   tkl     Check EF GBABP for Ks validity
04/30/15   vdc     Added support for APN for bootstrapping
04/06/15   tl      Add support for GBA_U
04/03/15   tkl     Add TMPI support
03/27/15   tl      Add support for GBA cancel
03/11/15   tl      Add caching of the module tcb pointer
01/26/15   tl      Initial version

=============================================================================*/

/*=============================================================================

                     INCLUDE FILES FOR MODULE

=============================================================================*/
#include "uim_variation.h"
#include "comdef.h"
#include "rex.h"
#include "queue.h"
#include "rcinit.h"

#include "gba_lib.h"

/*=============================================================================

                       GLOBAL DECLARATIONS

=============================================================================*/
extern boolean                           gba_task_stopped;
extern rex_crit_sect_type                gba_task_state_crit_sect;
extern rex_crit_sect_type                gba_data_crit_sect;

/*=============================================================================

                       DEFINE DECLARATIONS

=============================================================================*/

extern rex_tcb_type         * gba_tcb_ptr;
#define UIM_GBA_TCB           gba_tcb_ptr

/*--------------------------------------------------------------------------
                             Signal Masks
--------------------------------------------------------------------------*/
#define GBA_CMD_SIG                      0x00000001
#define GBA_BOOTSTRAP_CMD_SIG            0x00000002
#define GBA_MMGSDI_CMD_SIG               0x00000004
#define GBA_HTTP_CMD_SIG                 0x00000008
#define GBA_SANITY_TIMER_EXPIRE_SIG      0x00000010
#define GBA_NV_READ_SIG                  0x00000020
#define GBA_TASK_STOP_SIG                0x00004000

/*--------------------------------------------------------------------------
                             Data Delimiters
--------------------------------------------------------------------------*/
#define GBA_KS_LEN                       32
#define GBA_RES_LEN                      16
#define GBA_CK_LEN                       16
#define GBA_IK_LEN                       16
#define GBA_AUTS_LEN                     14
#define GBA_RAND_LEN                     16
#define GBA_AUTN_LEN                     16
#define GBA_AUTH_LEN                     (GBA_RAND_LEN + GBA_AUTN_LEN + 2)
#define GBA_MAX_BSF_ADDRESS_LEN          256
#define GBA_MAX_APN_LEN                  101

/* ===========================================================================
   MACRO:       GBA_ENTER_TASK_STATE_CRIT_SECT
   DESCRIPTION: To enter rex critical section for GBA task state
   ===========================================================================*/
#define GBA_ENTER_TASK_STATE_CRIT_SECT                                   \
        rex_enter_crit_sect(&gba_task_state_crit_sect);

/* ===========================================================================
   MACRO:       GBA_LEAVE_TASK_STATE_CRIT_SECT
   DESCRIPTION: To leave rex critical section for GBA task state
   ===========================================================================*/
#define GBA_LEAVE_TASK_STATE_CRIT_SECT                                   \
        rex_leave_crit_sect(&gba_task_state_crit_sect);

/* ===========================================================================
   MACRO:       GBA_ENTER_DATA_CRIT_SECT
   DESCRIPTION: To enter rex critical section for cached and SFS data
   ===========================================================================*/
#define GBA_ENTER_DATA_CRIT_SECT                                          \
        rex_enter_crit_sect(&gba_data_crit_sect);

/* ===========================================================================
   MACRO:       GBA_LEAVE_DATA_CRIT_SECT
   DESCRIPTION: To leave rex critical section for cached and SFS data
   ===========================================================================*/
#define GBA_LEAVE_DATA_CRIT_SECT                                          \
        rex_leave_crit_sect(&gba_data_crit_sect);

/*=============================================================================

                       DATA TYPE DEFINITIONS

=============================================================================*/
typedef uint8 gba_ks_type[GBA_KS_LEN];
typedef uint8 gba_rand_type[GBA_RAND_LEN];

/* ----------------------------------------------------------------------------
   ENUM:      GBA_AUTH_RESULT_ENUM_TYPE

   DESCRIPTION:
     Indicates type of authentication error.
-------------------------------------------------------------------------------*/
typedef enum {
  GBA_AUTH_SUCCESS,
  GBA_AUTH_GENERIC_FAILURE,
  GBA_AUTH_SYNC_FAILURE,
  GBA_AUTH_MAC_FAILURE
} gba_auth_result_enum_type;

/* ----------------------------------------------------------------------------
   ENUM:      GBA_AUTH_CONTEXT_ENUM_TYPE

   DESCRIPTION:
     Indicates type of authentication context.
-------------------------------------------------------------------------------*/
typedef enum {
  GBA_AUTH_CONTEXT_3G_AKA,
  GBA_AUTH_CONTEXT_GBA_BOOTSTRAP,
  GBA_AUTH_CONTEXT_GBA_NAF
} gba_auth_context_enum_type;

/* ----------------------------------------------------------------------------
   ENUM:      GBA_MODE_ENUM_TYPE

   DESCRIPTION:
     Indicates type of gba mode.
-------------------------------------------------------------------------------*/
typedef enum {
  GBA_ME,
  GBA_U
} gba_mode_enum_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      GBA_RES_TYPE

   DESCRIPTION:
     Structure which stores the authentication response data.
-------------------------------------------------------------------------------*/
typedef struct {
  uint16                              data_len;
  char                                data[GBA_RES_LEN];
} gba_res_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      GBA_CK_TYPE

   DESCRIPTION:
     Structure which stores the cipher key data.
-------------------------------------------------------------------------------*/
typedef struct {
  uint16                              data_len;
  char                                data[GBA_CK_LEN];
} gba_ck_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      GBA_IK_TYPE

   DESCRIPTION:
     Structure which stores the integrity key data.
-------------------------------------------------------------------------------*/
typedef struct {
  uint16                              data_len;
  char                                data[GBA_IK_LEN];
} gba_ik_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      GBA_AUTS_TYPE

   DESCRIPTION:
     Structure which stores the authentication token.
-------------------------------------------------------------------------------*/
typedef struct {
  uint16                              data_len;
  char                                data[GBA_AUTS_LEN];
} gba_auts_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      GBA_AUTH_TYPE

   DESCRIPTION:
     Structure which stores the RES data concatenated with the AUTN data.
-------------------------------------------------------------------------------*/
typedef struct {
  uint16                              data_len;
  uint8                               data[GBA_AUTH_LEN];
} gba_auth_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      GBA_BSF_ADDRESS_TYPE

   DESCRIPTION:
     Structure which stores the BSF address data.
-------------------------------------------------------------------------------*/
typedef struct {
  uint16                              data_len;
  char                                data[GBA_MAX_BSF_ADDRESS_LEN];
} gba_bsf_address_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      GBA_3G_AUTH_SUCCESS_DATA

   DESCRIPTION:
     Structure contains the data required in case of 3G authentication success.
-------------------------------------------------------------------------------*/
typedef struct {
  gba_res_type                        res;
  gba_ck_type                         ck;
  gba_ik_type                         ik;
} gba_3g_auth_rsp_data;

/* ----------------------------------------------------------------------------
   STRUCTURE:      GBA_AUTH_SUCCESS_DATA

   DESCRIPTION:
     Structure contains the data required in case of authentication success.
-------------------------------------------------------------------------------*/
typedef struct {
  gba_auth_context_enum_type  auth_mode;
  union {
    gba_3g_auth_rsp_data      auth_3g_rsp;
    gba_res_type              auth_gba_bootstrap_rsp;
    gba_ks_naf_type           ks_ext_naf;
  }auth_rsp_data;
} gba_auth_success_data;

/* ----------------------------------------------------------------------------
   STRUCTURE:      GBA_AUTH_SYNC_FAILURE_DATA

   DESCRIPTION:
     Structure contains the data required in case of authentication failure.
-------------------------------------------------------------------------------*/
typedef struct {
  gba_auts_type                       auts;
} gba_auth_sync_failure_data;

/* ----------------------------------------------------------------------------
   STRUCTURE:      GBA_IDENTITY_DATA_TYPE

   DESCRIPTION:
     Structure contains the data require for GBA identity state processing.
-------------------------------------------------------------------------------*/
typedef struct {
  gba_aid_type                        aid;
  gba_session_type                    session_type;
  gba_impi_type                       impi;
  gba_mode_enum_type                  mode;
} gba_identity_data_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      GBA_KS_VALIDITY_DATA_TYPE

   DESCRIPTION:
     Structure contains the data require for checking GBA_U Ks validity.
-------------------------------------------------------------------------------*/
typedef struct {
  gba_btid_type                       btid;
  gba_lifetime_type                   lifetime;
} gba_ks_validity_data_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      GBA_HTTP_RESPONSE_ERROR_DATA_TYPE

   DESCRIPTION:
     Structure contains optional error codes when GBA HTTP returns GBA_SERVER_ERROR
	 or GBA_NETWROK_ERROR. 
-------------------------------------------------------------------------------*/
typedef struct {
  union
  {
    struct
    {
      boolean                             http_status_present;
      uint16                              http_status;
    } server_error;
    struct
    {
      boolean                             netdown_reason_present;
      uint32                              netdown_reason;
    } network_error;
  } data;
} gba_http_response_error_data_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      GBA_HTTP_AUTHENTICATE_DATA_TYPE

   DESCRIPTION:
     Structure conatins the authentication result data.
-------------------------------------------------------------------------------*/
typedef struct {
  gba_rand_type                       rand;
  gba_auth_type                       auth_challenge;
  gba_http_response_error_data_type   error_data;
} gba_http_authenticate_data_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:        GBA_AUTH_STATUS_DATA_TYPE

   DESCRIPTION:
     Structure contains the authentication status data returned by UIM.
-------------------------------------------------------------------------------*/
typedef struct {
  gba_auth_result_enum_type           result;
  gba_session_type                    session_type;
  union {
    gba_auth_success_data             auth_success;
    gba_auth_sync_failure_data        auth_sync_failure;
  } data;
} gba_auth_status_data_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:        GBA_HTTP_200_OK_DATA_TYPE

   DESCRIPTION:
     Structure conatins the required 200 OK data from Data Services.
-------------------------------------------------------------------------------*/
typedef struct {
  gba_btid_type                       btid;
  gba_lifetime_type                   lifetime;
  boolean                             support_tmpi;
  gba_http_response_error_data_type   error_data;
} gba_http_200_ok_data_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      GBA_APN_TYPE

   DESCRIPTION:
     Contains information for GBA APN
-------------------------------------------------------------------------------*/
typedef struct {
  boolean        apn_available;
  char           apn[GBA_MAX_APN_LEN];
}gba_apn_type;

/*=============================================================================

                       FUNCTION PROTOTYPES

=============================================================================*/

/*===========================================================================
FUNCTION GBA_WAIT

DESCRIPTION
  Main wait routine for the GBA task.

DEPENDENCIES
  None

RETURN VALUE
  A rex_sigs_type value indicating all signals set (the current state
  of all signals). The calling unit can then check for the reason for
  exiting the wait.

SIDE EFFECTS
  None
===========================================================================*/
rex_sigs_type gba_wait (
  rex_sigs_type requested_mask
);

/* ==========================================================================
FUNCTION GBA_RESET

DESCRIPTION:
  Performs a reset of all modules.

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
==========================================================================*/
void gba_reset (
  void
);

#endif /* GBA_H */
