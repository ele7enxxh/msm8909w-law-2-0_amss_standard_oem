/*!
  @file
  cfcm_msgr.h

  @brief
  Centralized Flow-control Manager(CFCM) MSGR header file.

  @detail
  OPTIONAL detailed description of this C header file.
  - DELETE this section if unused.

  @author
  rohitj

*/

/*==============================================================================

  Copyright (c) 2014 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/cfcm/inc/cfcm_msgr.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
10/09/14   rj      CFCM changes to Process monitor's input in its task context
04/07/14   rj      initial version
==============================================================================*/

#ifndef CFCM_MSGR_H
#define CFCM_MSGR_H

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

#include "msgr.h"
#include "cfcm.h"
#include "cfcm_msg_internal.h"

/*==============================================================================

                   INTERNAL DEFINITIONS AND TYPES

==============================================================================*/

/* This is a union of all the messages that CFCM is interested in */
typedef union
{
  msgr_spr_loopback_struct_type    loop_back;

  cfcm_reg_req_msg_type_s          cfcm_reg;

  cfcm_dereg_req_msg_type_s        cfcm_dereg;

  cfcm_monitor_ind_msg_s           monitor_ind;

  /* Used for Empty message packet */
  msgr_hdr_struct_type             msg_hdr;
} cfcm_msgr_rcv_msg_u;


/*==============================================================================

                    EXTERNAL FUNCTION PROTOTYPES

==============================================================================*/

/*=======================================================================
FUNCTION CFCM_MSGR_INIT

DESCRIPTION
  To initialize MSGR client & setup associated queue(s)

DEPENDENCIES
  None

RETURN VALUE
  errno_enum_type

SIDE EFFECTS
  None
========================================================================*/
errno_enum_type cfcm_msgr_init ( void );

/*=======================================================================
FUNCTION CFCM_MSGR_DEINIT

DESCRIPTION
  To de-initialize MSGR client & associated queue(s)

DEPENDENCIES
  None

RETURN VALUE
  errno_enum_type

SIDE EFFECTS
  None
========================================================================*/
errno_enum_type cfcm_msgr_deinit ( void );

/*=======================================================================
FUNCTION CFCM_MSGR_SEND_MSG

DESCRIPTION
  Send a message via MSGR

DEPENDENCIES
  None

RETURN VALUE
  errno_enum_type

SIDE EFFECTS
  None
========================================================================*/
errno_enum_type cfcm_msgr_send_msg (
  msgr_hdr_struct_type *msg_hdr_ptr,  /*!< Pointer to MSG Header  */
  msgr_umid_type        umid,         /*!< UMID of the message    */
  int32                 size          /*!< Size of the message    */
);

/*=======================================================================
FUNCTION CFCM_MSGR_PROCESS_SIG

DESCRIPTION
  Process the CFCM's MSGR REX signal & associated queue

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
========================================================================*/
void cfcm_msgr_process_sig ( void );

#endif /* CFCM_MSGR_H */
