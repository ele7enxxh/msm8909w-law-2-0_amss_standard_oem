#ifndef REG_ECALL_H
#define REG_ECALL_H
/*===========================================================================
          R E G  -  E C A L L   H E A D E R   F I L E

DESCRIPTION
  This header file contains message definitions for the REG and ECALL MODEM
  Task interface.

Copyright (c) 2009,2010 Qualcomm Technologies, Incorporated and its licensors.  All Rights 
Reserved.  QUALCOMM Proprietary.  Export of this technology or software 
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/reg_ecall.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who       what, where, why
--------   ---       ----------------------------------------------------------
02/11/10    ajt       Created file
===========================================================================*/
/* <EJECT> */

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "customer.h"



#include "sys_stru.h"
#include "reg_mm.h"

/*===========================================================================
                     M A C R O  D E F I N I T I O N S
===========================================================================*/


/*===========================================================================
                   T Y P E  D E C L A R A T I O N S
===========================================================================*/
typedef enum
{
  /* ECALL ==> REG */
  ECALL_REG_CMD_NONE,
  ECALL_REG_MODE_IND
} ecall_to_reg_cmd_type;

typedef enum reg_ecall_mode_e
{
  REG_ECALL_MODE_NONE = -1,               /* FOR INTERNAL USE ONLY! */
  REG_ECALL_MODE_NORMAL,
  REG_ECALL_MODE_ECALL_ONLY,
  REG_ECALL_MODE_MAX                      /* FOR INTERNAL USE ONLY! */
} reg_ecall_mode_e_type;

/*******************************************************************************
********************************************************************************
**                                                                            **
**                  ECALL MODEM to REG Primitives                             **
**                                                                            **
********************************************************************************
*******************************************************************************/

/*******************************************************************************
**                                                                            **
**  ECALL_REG_MODE_IND                                                        **
**                                                                            **
*******************************************************************************/

typedef struct ecall_reg_mode_ind_s
{
  IMH_T                                               message_header;
  reg_ecall_mode_e_type                               mode_ind;
  sys_modem_as_id_e_type                              as_id;
} ecall_reg_mode_ind_s_type;





#endif /* #ifndef REG_ECALL_H */
