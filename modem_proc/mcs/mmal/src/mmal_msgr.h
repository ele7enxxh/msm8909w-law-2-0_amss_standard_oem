#ifndef MMAL_MSGR_H
#define MMAL_MSGR_H
/*!
  @file
  mmal_msgr.h

  @brief
  APIs for MMAL's MSGR interface

*/

/*===========================================================================

  Copyright (c) 2013 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

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

===========================================================================*/

/*==========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/mmal/src/mmal_msgr.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/21/11   ckk     Initial implementation

==========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <msgr.h>

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/



/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

errno_enum_type mmal_msgr_init( void );

errno_enum_type mmal_msgr_deinit( void );

void mmal_msgr_deregister_msgs( void );

errno_enum_type mmal_msgr_send_msg
(
  msgr_hdr_struct_type *msg_hdr_ptr,
  msgr_umid_type        umid,
  int32                 size
);

void mmal_msgr_process_rcvd_signal( void );



#endif /* MMAL_MSGR_H */
