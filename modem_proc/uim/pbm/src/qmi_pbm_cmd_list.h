#ifndef _QMI_PBM_CMD_LIST_H
#define _QMI_PBM_CMD_LIST_H
/*===========================================================================

                         D S _ Q M I _ P B M _ C M D _ L I S T . H

DESCRIPTION

 The Data Services Qualcomm PBM command list management header.


EXTERNALIZED FUNCTIONS



Copyright (c) 2009QUALCOMM Technologies, Inc(QTI).
All Rights Reserved.
QUALCOMM Technologies Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header:

when        who    what, where, why
--------    ---    ----------------------------------------------------------
03/29/10    sk     Initial revision.
===========================================================================*/

#include "IxErrno.h"
#include "qmi_pbm_if.h"




/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/


/* For simplicity just use a C union to hold the subsystem specific data
   that is to be kept in the cmdlist */
typedef enum
{
  QMI_PBM_CMD_FREE                 = 1,  /*!< This cmd buffer is not in use */
  QMI_PBM_CMD_AWAITING_CALLBACK          /*!< Cmd is waiting for callback */
} qmi_pbm_cmd_state_e_type;


typedef struct
{
  uint32                      hdl;  /* Command Handle */
  uint8                       cmd;  /* PBM cmd */
  uint32                      sub_id;  /* Subsystem ID (e.g Record ID) */
  qmi_pbm_cmd_state_e_type    state;  /* State of cmd list Entry */
} qmi_pbm_cmd_list_public_type;


/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/


void qmi_pbm_cmd_list_init
(
  void
);

IxErrnoType qmi_pbm_cmd_list_new
(
  int cmd,
  uint32 hdl,
  qmi_pbm_cmd_state_e_type state,
  uint32 sub_id
);

IxErrnoType qmi_pbm_cmd_list_free
(
  uint32 hdl
);

IxErrnoType qmi_pbm_cmd_list_update_sub_id
(
  uint32 hdl,
  uint32 sub_id
);

IxErrnoType qmi_pbm_cmd_list_query
(
  uint32 hdl,
  qmi_pbm_cmd_list_public_type *info_ptr /*!< Output parameter */
);

#endif /* _QMI_PBM_CMD_LIST_H */

