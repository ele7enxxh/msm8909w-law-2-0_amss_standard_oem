/*!
  @file
  lrrc_qsh_ext.h

  @brief
  This file contains definitions for the Common type and constants required
  by different modules within LTE RRC

*/

/*=============================================================================

  Copyright (c) 2015 QUALCOMM Technologies Incorporated. All Rights Reserved

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

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order. 

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/api/lrrc_qsh_ext.h#1 $

when       who           what, where, why
--------   ---           ------------------------------------------------------------
05/10/16   rtindola      Initial Version: CL reference #941365
=============================================================================*/

#ifndef LRRC_QSH_EXT_H
#define LRRC_QSH_EXT_H


/*=============================================================================

                   INCLUDE FILES

=============================================================================*/
#include <qsh.h>


#define LRRC_QSH_MAJOR_VER 0
#define LRRC_QSH_MINOR_VER 1

typedef enum
{  
    LRRC_QSH_METRIC_CONN_END_INFO = 0, /* lrrc_qsh_metric_conn_end_info_s*/
    LRRC_QSH_METRIC_MAX
} lrrc_qsh_metric_e;


typedef struct
{
  qsh_metric_hdr_s hdr;

  uint8 call_end_type;

  uint8 cause;

}lrrc_qsh_metric_conn_end_info_s;

#endif /* LRRC_QSH_EXT_H */

