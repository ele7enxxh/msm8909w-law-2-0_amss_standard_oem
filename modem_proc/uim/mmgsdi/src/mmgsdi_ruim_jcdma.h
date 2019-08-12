#ifndef MMGSDI_RUIM_JCDMA_H
#define MMGSDI_RUIM_JCDMA_H
/*===========================================================================


           M M G S D I   U I M   H E A D E R S


===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2007, 2010-11, 2014 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.  
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/mmgsdi/src/mmgsdi_ruim_jcdma.h#1 $$ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
10/12/14   hh      Support slot refresh for NV 69687 JCDMA Mode
07/15/14   yt      Support for NV refresh
08/01/11   av      Slot-specific handling of JCDMA apis
01/31/11   ssr     Decoupled FEATURE_UIM_JCDMA_RUIM_SUPPORT feature
07/01/10   nmb     Updates for JCDMA CSIM support
05/17/07   nk      First revision

=============================================================================*/

#include "mmgsdi.h"
#include "mmgsdilib.h"

/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/
/* Type of locks */
#define  MMGSDI_JCDMA_LINE_LOCK                    0
#define  MMGSDI_JCDMA_NW_LOCK                      1
#define  MMGSDI_JCDMA_SP_LOCK                      2
#define  MMGSDI_JCDMA_UG_LOCK                      3
/* The value read from EF to see if the card is test card */
#define  MMGSDI_JCDMA_UIM_TEST_VERSION             1
#define  MMGSDI_JCDMA_UIM_LOCK_IND              0x01
#define  MMGSDI_JCDMA_UIM_IMSI_PROGRAMMED       0x80
#define  MMGSDI_JCDMA_UIM_IMSI_STATUS           0xF7
#define  MMGSDI_JCDMA_UIMID_DEFAULT       0xFFFFFFFF
#define  MMGSDI_JCDMA_UIM_SP_UG_DEFAULT         0xFF
#define  MMGSDI_JCDMA_8BITS                        8
#define  MMGSDI_JCDMA_RUIMID_DEFAULT            0xFF
#define  MMGSDI_JCDMA_IMSI_MCC1_DEFAULT          440
#define  MMGSDI_JCDMA_IMSI_MCC2_DEFAULT          441
#define  MMGSDI_JCDMA_SP_ID_DEFAULT                0
#define  MMGSDI_JCDMA_UG_ID_DEFAULT                0
#define  MMGSDI_JCDMA_INDICATOR_DEFAULT            1
#define  MMGSDI_JCDMA_JCSIM1_ID                 0x00
#define  MMGSDI_JCDMA_JCSIM2_ID                 0x02
#define  MMGSDI_JCDMA_SIG_SIZE                  0x10
#define  MMGSDI_JCDMA_CSIM_DISC_DATA_TAG        0x53
#define  MMGSDI_JCDMA_AID_LEN_INDEX              0x3
#define  MMGSDI_JCDMA_AID_START_INDEX            0x4
#define  MMGSDI_JCDMA_TAG_SIZE                   0x1
#define  MMGSDI_JCDMA_LEN_SIZE                   0x1

typedef enum {
  MMGSDI_JCDMA_IMSI_STATUS_0,
  MMGSDI_JCDMA_IMSI_STATUS_1,
  MMGSDI_JCDMA_MAX_IMSI_STATUS_ENUM = 0x7FFFFFFF
} mmgsdi_jcdma_imsi_status_enum_type;

typedef enum {
  MMGSDI_JCDMA_LOCK_DISABLED,
  MMGSDI_JCDMA_LOCK_ENABLED,
  MMGSDI_JCDMA_MAX_LOCK_STATE_ENUM = 0x7FFFFFFF
} mmgsdi_jcdma_lock_state_enum_type;

typedef struct {
  mmgsdi_jcdma_lock_state_enum_type   lock_state;
  boolean                             imsi_programmed;
  mmgsdi_jcdma_imsi_status_enum_type  imsi_status;
  uint16                              imsi_mcc;
  uint32                              ruim_id;
  uint8                               sp_id;
  uint8                               ug_id;
} mmgsdi_jcdma_session_attr_type;

typedef struct {
  mmgsdi_session_id_type          session_id;
  boolean                         session_opened;
  mmgsdi_data_type                aid;
  mmgsdi_jcdma_session_attr_type  session_attr;
  boolean                         attr_avail;
} mmgsdi_jcdma_session_info_type;

typedef struct {
  boolean                           lock_enabled;
  mmgsdi_jcdma_card_info_enum_type  card_type;
  uint16                            imsi_mcc;
  uint32                            ruim_id;
  uint8                             sp_id;
  uint8                             ug_id;
}mmgsdi_jcdma_card_attr_type;

typedef struct {
  mmgsdi_jcdma_session_info_type    jcsim1;
  mmgsdi_jcdma_session_info_type    jcsim2;
  mmgsdi_jcdma_card_attr_type       card_attr;
} mmgsdi_jcdma_uicc_info_type;


/*===========================================================================
FUNCTION MMGSDI_PROCESS_JCDMA_CARD_GET_INFO_CMD

DESCRIPTION
  This function, called in mgmsdi_process_command.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE:
  MMGSDI_SUCCESS: upon succssesful processing of command.
  MMGSDI_ERROR: upon error

SIDE EFFECTS
  None
===========================================================================*/
extern mmgsdi_return_enum_type mmgsdi_process_jcdma_card_get_info_cmd (
  const mmgsdi_jcdma_get_card_info_req_type *req_ptr
);


/*===========================================================================
FUNCTION MMGSDI_JCDMA_RSP_GET_CARD_INFO

DESCRIPTION
  This function sends response

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE:
   MMGSDI_SUCCESS: upon successful queueing of the conf.
   MMGSDI_ERROR: upon error

SIDE EFFECTS
  None
===========================================================================*/
extern mmgsdi_return_enum_type mmgsdi_jcdma_rsp_get_card_info (
  const mmgsdi_uim_report_rsp_type*   uim_rsp_ptr
);


/*===========================================================================
FUNCTION MMGSDI_JCDMA_OPEN_NON_PROV_SESSION

DESCRIPTION
  This function openss non provisioning sessions to the JCDMA CSIM Apps

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE:
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void mmgsdi_jcdma_open_non_prov_session (
  const mmgsdi_app_info_type*      app_info
);


/*===========================================================================
FUNCTION MMGSDI_JCDMA_FIND_CSIM_APP_FOR_COLOR_AND_LOCK

DESCRIPTION
  This function determines if a CSIM APP is present to be used for
  color and lock status check

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE:
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
extern mmgsdi_return_enum_type mmgsdi_jcdma_find_csim_app_for_color_and_lock (
  const mmgsdi_read_cnf_type  *read_cnf_ptr
);


/*===========================================================================
FUNCTION MMGSDI_JCDMA_INIT

DESCRIPTION
  This function is called from MMGSDI init to allocated memory for the jcdma
  data structure and initialize the members

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE:
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
extern mmgsdi_return_enum_type mmgsdi_jcdma_init (
  uint8  slot_index
);
#endif /* MMGSDI_RUIM_JCDMA_H */


/*===========================================================================
FUNCTION MMGSDI_JCDMA_FREE

DESCRIPTION
  This function is called to free memory previously allocated to keep track
  of JCDAM info data

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE:
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void mmgsdi_jcdma_free (
  uint8  slot_index
);


/*===========================================================================
FUNCTION MMGSDI_JCDMA_GET_UICC_CARD_ATTR

DESCRIPTION
  This function gets the info from the CSIM Apps for color and lock
  determination

DEPENDENCIES
  None

LIMITATIONS
  NONE

RETURN VALUE:
  mmgsdi_return_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
extern mmgsdi_return_enum_type mmgsdi_jcdma_get_uicc_card_attr (
  mmgsdi_app_enum_type      app_type,
  mmgsdi_slot_id_enum_type  slot
);


/*===========================================================================
FUNCTION MMGSDI_JCDMA_CLOSE_NON_PROV_SESSION

DESCRIPTION
  This function closes the non-prov sessions to the CSIM apps

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE:
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void mmgsdi_jcdma_close_non_prov_session (
  const mmgsdi_app_info_type*      app_info_ptr
);

