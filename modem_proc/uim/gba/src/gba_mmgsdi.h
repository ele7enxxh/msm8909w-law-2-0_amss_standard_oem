#ifndef GBA_MMGSDI_H
#define GBA_MMGSDI_H
/*===========================================================================


            G B A   M M G S D I   H E A D E R


===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2014 - 2015 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/23/15   vdc     Do not read EF GBABP while generating TMPI
07/09/15   tkl     Check EF GBABP for Ks validity
04/06/15   kv      Add GBA_U support
11/31/14   tl      Secondary changes
11/31/14   kv      Initial revision
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "mmgsdilib_common.h"

#include "gba.h"
#include "gba_bootstrap.h"
#include "gba_lib.h"

/*=============================================================================

                       DATA DECLARATIONS

=============================================================================*/

typedef struct {
  mmgsdi_event_data_type mmgsdi_evt_data;
} gba_mmgsdi_evt_data_type;

typedef struct {
  mmgsdi_return_enum_type status;
  mmgsdi_cnf_enum_type    cnf;
  mmgsdi_cnf_type         cnf_data;
} gba_mmgsdi_cmd_rsp_data_type;

typedef enum {
  GBA_MMGSDI_CMD_RSP,
  GBA_MMGSDI_EVENT_RPT
}gba_mmgsdi_cmd_enum_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      GBA_MMGSDI_CMD_TYPE

   DESCRIPTION:
     Contains information for handling MMGSDI event commands
-------------------------------------------------------------------------------*/
typedef struct {
  q_link_type                      link;
  gba_mmgsdi_cmd_enum_type         cmd_type;
  union {
    gba_mmgsdi_evt_data_type       event_data;
    gba_mmgsdi_cmd_rsp_data_type   rsp_data;
  } data;
} gba_mmgsdi_cmd_type;

/*=============================================================================

                       FUNCTION PROTOTYPES

=============================================================================*/

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
);

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
  void
);

/*===========================================================================
   FUNCTION:      GBA_MMGSDI_RETRIVE_IDENTITY

   DESCRIPTION:
     GBA_MMGSDI API triggered by identity  module to extract the IMPI or IMSI from MMGSDI

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
);

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
);

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
);


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
);

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
);

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
);

#endif /* GBA_MMGSDI_H */

