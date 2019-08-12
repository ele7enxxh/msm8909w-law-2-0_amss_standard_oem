/*!
  @file
  rflm_diag_power_rfa.h

  @brief
  RFLM power change logging RFA interfaces

  @detail

*/

/*===========================================================================

  Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rflm/services/diag/inc/rflm_diag_power_rfa.h#1 $

when         who     what, where, why
----------   ---     --------------------------------------------------------
2014-11-17   aw      Initial version
===========================================================================*/

#ifndef RFLM_DIAG_POWER_RFA_H
#define RFLM_DIAG_POWER_RFA_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/
#define RFLM_DIAG_POWER_ARGS_MAX_CNT         6

typedef struct
{
   uint8 tech: 8;
   uint8 device_id:8;
   uint8 sub_id:8;
   uint8 length:8;
} rflm_diag_power_buffer_item_header_t;

typedef struct
{
	int16 chg_type: 16;
	int16 chg_data: 16;
}rflm_diag_power_buffer_item_payload_t;

typedef struct
{
  rflm_diag_power_buffer_item_header_t  header;
  uint32 timestamp;
  rflm_diag_power_buffer_item_payload_t payload[RFLM_DIAG_POWER_ARGS_MAX_CNT];
}rflm_diag_power_buffer_item_t;

typedef enum
{
	RFLM_DIAG_POWER_TYPE_RGI, 
	RFLM_DIAG_POWER_TYPE_PA, 
	RFLM_DIAG_POWER_TYPE_MOD, 
	RFLM_DIAG_POWER_TYPE_FREQ,
	RFLM_DIAG_POWER_TYPE_BAND,
	RFLM_DIAG_POWER_TYPE_MAX
}rflm_diag_power_type_e; 


/* 
   uint32 * rbuf_ptr : starting address of a ping-pong buffer
   uint32 num_of_rd  : Number of valid reads
   RFA can use it to read out buffer data
*/
typedef void (*rflm_diag_power_read_callback_t)(uint32* rbuf_ptr, uint32 num_of_rd);

/*===========================================================================

                           FUNCTION DECLARATIONS

===========================================================================*/
#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================

                    EXTERNAL INTERFACE FUNCTIONS

===========================================================================*/

/* Used by RFA to notify power reporting thread 20ms switch */
extern void rflm_diag_power_switch_notify(void);

/* Used by RFA to set read data callback */
extern void rflm_diag_power_set_read_callback(rflm_diag_power_read_callback_t read_callback);


#ifdef __cplusplus
} // extern "C"
#endif

#endif
