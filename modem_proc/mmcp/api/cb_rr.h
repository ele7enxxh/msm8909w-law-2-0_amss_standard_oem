
/*===========================================================================

  Copyright (c) 2013 Qualcomm Technologies Incorporated. All Rights Reserved

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

===========================================================================*/

#ifndef cb_rr_h
#define cb_rr_h
#include "sys.h"



/***********************************************************************/
/*                                                                     */
/*                         RR to CB Primitive IDs                      */
/*                                                                     */
/***********************************************************************/

enum        /* message_id of inter-task messages */
{
   RR_CELL_CHANGE_IND    = 0x81
};


/**********************************************/
/*                                            */
/*         RR_CELL_CHANGE_IND                 */
/*                                            */
/**********************************************/

typedef struct
{
   IMH_T   message_header;        /* 4 bytes */
   byte    plmn_id[3];            /* 3 bytes */
   byte    lac[2];                /* 2 bytes */
   byte    cell_id[2];            /* 2 bytes */
   boolean CBCH_present ;

   sys_modem_as_id_e_type as_id;

}
rr_cell_change_ind_T;

#endif


