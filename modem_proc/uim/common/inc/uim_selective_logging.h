#ifndef UIM_SELECTIVE_LOGGING_H
#define UIM_SELECTIVE_LOGGING_H
/*===========================================================================


           U I M   S E L E C T I V E   L O G   H E A D E R S


===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/common/inc/uim_selective_logging.h#1 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
07/04/16   ar      Initial version

=============================================================================*/

/*=============================================================================

                     INCLUDE FILES FOR MODULE

=============================================================================*/

#include "uim_msg.h"
#include "uim_common_efs.h"

/*=============================================================================

                       DATA DECLARATIONS

=============================================================================*/
#define UIM_MAX_ITEM_IN_SELECT_LOG        16
#define UIM_LOG_EONS                      0


/*=============================================================================

                                 EXTERN VARIABLES

=============================================================================*/

extern uint8   uim_selective_logging[UIM_MAX_ITEM_IN_SELECT_LOG];


/*=============================================================================

                           FUNCTION DECLARATIONS

=============================================================================*/
/*===========================================================================
   FUNCTION:      UIM_SELECTIVE_LOGGING_INIT

   DESCRIPTION:
     This function initialise the selective logging
     functionality by reading NV item 73880

   DEPENDENCIES:
     None

   LIMITATIONS:
     None


   RETURN VALUE:
    Void

   SIDE EFFECTS:
     None

=============================================================================*/
void uim_selective_logging_init
(
  void
);


/*=============================================================================

                                 MACROS
               MACRO Definitions used for logging throughout UIM

=============================================================================*/
#define UIM_SELECTIVE_MSG_HIGH_0(index, xx_fmt)                    \
  if (uim_selective_logging[index])                                \
  {                                                                \
    UIM_MSG_HIGH_0(xx_fmt);                                        \
  }

#define UIM_SELECTIVE_MSG_HIGH_1(index, xx_fmt, xx_arg1)          \
  if (uim_selective_logging[index])                               \
  {                                                               \
    UIM_MSG_HIGH_1(xx_fmt, xx_arg1);                              \
  }

#define UIM_SELECTIVE_MSG_HIGH_2(index, xx_fmt, xx_arg1, xx_arg2)          \
  if (uim_selective_logging[index])                                        \
  {                                                                        \
    UIM_MSG_HIGH_2(xx_fmt, xx_arg1, xx_arg2);                              \
  }

#define UIM_SELECTIVE_MSG_HIGH_3(index, xx_fmt, xx_arg1, xx_arg2, xx_arg3)      \
  if (uim_selective_logging[index])                                             \
  {                                                                             \
    UIM_MSG_HIGH_3(xx_fmt, xx_arg1, xx_arg2, xx_arg3);                          \
  }

#define UIM_SELECTIVE_MSG_HIGH_4(index, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4)   \
  if (uim_selective_logging[index])                                                   \
  {                                                                                   \
    UIM_MSG_HIGH_4(xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4);                       \
  }

#define UIM_SELECTIVE_MSG_HIGH_5(index, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5)   \
  if (uim_selective_logging[index])                                                            \
  {                                                                                            \
    UIM_MSG_HIGH_5(xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5);                       \
  }

#define UIM_SELECTIVE_MSG_ERR_0(index, xx_fmt)                     \
  if (uim_selective_logging[index])                                \
  {                                                                \
    UIM_MSG_ERR_0(xx_fmt);                                         \
  }

#define UIM_SELECTIVE_MSG_ERR_1(index, xx_fmt, xx_arg1)           \
  if (uim_selective_logging[index])                               \
  {                                                               \
    UIM_MSG_ERR_1(xx_fmt, xx_arg1);                               \
  }

#define UIM_SELECTIVE_MSG_ERR_2(index, xx_fmt, xx_arg1, xx_arg2)           \
  if (uim_selective_logging[index])                                        \
  {                                                                        \
    UIM_MSG_ERR_2(xx_fmt, xx_arg1, xx_arg2);                               \
  }

#define UIM_SELECTIVE_MSG_ERR_3(index, xx_fmt, xx_arg1, xx_arg2, xx_arg3)       \
  if (uim_selective_logging[index])                                             \
  {                                                                             \
    UIM_MSG_ERR_3(xx_fmt, xx_arg1, xx_arg2, xx_arg3);                           \
  }

#define UIM_SELECTIVE_MSG_ERR_4(index, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4)    \
  if (uim_selective_logging[index])                                                       \
  {                                                                                   \
    UIM_MSG_ERR_4(xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4);                        \
  }

#endif /* UIM_SELECTIVE_LOGGING_H */
