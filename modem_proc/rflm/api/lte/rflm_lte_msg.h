/*!
  @file
  rflm_lte_msg.h

  @brief
  Describes modules available for GSM using message router under Tech RFLM. 

  @details
  This file contains LTE Tech RFLM MSG router commands
  that needs to use the message router. Also the data structure associated with
  that command need to be provided. The CMD type is to used carefully as it defines
  the need of that msg router command.
  
*/


/*===========================================================================

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

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rflm/api/lte/rflm_lte_msg.h#1 $ 
 
when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
02/04/16   am      Apply WTR PAR correction values in LTE while running Autopin
12/31/15   am      Remove unused variables from par_update_msg
11/02/15   kg      PAR Auto-pin support
06/26/14   bar     Added support for Therm read.
06/18/14   sbm     Added support for WTR HDET
04/08/14   kab     Add support for Slot Based scheduling
03/04/14   kab     Add support for PRACH Tx path off Scheduling
02/18/14    pv     Initial checkin RFLM msgr LTE definition
===========================================================================*/
 
#ifndef rflm_lte_msg_H
#define rflm_lte_msg_H

/*=============================================================================

                   INCLUDE FILES

=============================================================================*/
#include "rflm_msg.h"


/*! @brief Asynchronous FBRx Gain correction Indication message

*/
typedef struct
{
  /*! This param provides the start RB info when this Gain error was captured */
  uint8 start_rb;
  /*! This param provides the RB block info when this Gain error was captured */
  uint8 rb_block;
  /*! This param provides the Gain error in dB10 */
  int16 gain_err;
  /*! This param provides the system bandwidth info when this Gain error was captured */
  uint8 sys_bw;
  /*! This param provides the txagc value when this Gain error was captured */
  int16 txagc_val;
  /*! This param provides the carrier index of the Tx for which this Gain error was captured */
  uint32 carrier_idx;

} rflm_lte_fbrx_gain_err_params_s;

typedef struct
{
  msgr_hdr_struct_type   hdr;
  rflm_lte_fbrx_gain_err_params_s tx_params; 
} rflm_lte_fbrx_gain_err_update_ind_s;

typedef struct
{
  msgr_hdr_struct_type   hdr;
  rflm_lte_fbrx_gain_err_params_s tx_params; 
} rflm_lte_hdet_read_update_ind_s;
typedef struct
{
  msgr_hdr_struct_type   hdr;
  uint32 carrier_idx;
  uint16 num_rbs;
} rflm_lte_hdet_par_read_update_ind_s;

typedef struct
{
  uint32              wake_up_time;   /* In ustmr time.*/
  uint32              sf_num;         /* Subframe number.*/
  uint32              cell_index;     /* Cell Index */
  void               *lte_dm_ptr;      /* LTE DM memory pointer*/
  msgr_umid_type      umid;  /* UMID MSG call to invoke*/
  uint32              action_time;   /* Current Time.*/
  rflm_lte_ul_chan_type_e sf_type;
} rflm_lte_fed_params_s;
  
typedef struct
{
  msgr_hdr_struct_type     hdr;              /* MSGR Message Header */
  rflm_lte_fed_params_s  fed_lte_params;    
} rflm_lte_fed_ind_s;

typedef struct
{
  msgr_hdr_struct_type     hdr; 

  /*! This param provides the carrier index of the Tx for which this Therm is being read */
  uint32 carrier_idx;
} rflm_lte_fed_therm_ind_s;


/*! @brief     */
enum
{
  /*! RFLM uses this message to send async commands to RF TASK */
  MSGR_DEFINE_UMID( RFLM, LTE, IND, FBRX_GAIN_ERR_UPDATE,  
                    0x01, rflm_lte_fbrx_gain_err_update_ind_s) , 

  MSGR_DEFINE_UMID( RFLM, LTE, IND, FED_SET_TIMER,  
                    0x02, rflm_lte_fed_ind_s)  ,

  MSGR_DEFINE_UMID( RFLM, LTE, IND, FED_PRACH_WAKEUP,  
                    0x03, rflm_lte_fed_ind_s)  ,
					
  MSGR_DEFINE_UMID( RFLM, LTE, IND, FED_SCHEDULER_WAKEUP,  
                    0x04, rflm_lte_fed_ind_s)  ,

  MSGR_DEFINE_UMID( RFLM, LTE, IND, HDET_READ_UPDATE,  
                    0x05, rflm_lte_hdet_read_update_ind_s)  ,

  MSGR_DEFINE_UMID( RFLM, LTE, IND, FED_THERM_UPDATE,  
                    0x06, rflm_lte_fed_therm_ind_s)  ,

   MSGR_DEFINE_UMID( RFLM, LTE, IND, HDET_PAR_READ_UPDATE,  
                    0x07, rflm_lte_hdet_par_read_update_ind_s)  ,							
};
#endif /*rflm_lte_msg_H*/
