#ifndef WCI2_CORE_H
#define WCI2_CORE_H
/*!
  @file
  wci2_core.h

  @brief
  Service for packing and sending messages/data in WCI-2 format

*/

/*===========================================================================

  Copyright (c) 2015 Qualcomm Technologies Incorporated.
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/wci2/inc/wci2_core.h#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
01/14/15   ckk     Add support for type7[ant_tx]
07/14/14   btl     Consolidate WCI2 TX definitions
07/12/14   ckk     Add support for type4 & type7[pwr state]
02/26/14   btl     Add type 7
10/15/13   ckk     Moving over to wci2
07/01/13   btl     Initial version

==========================================================================*/
/*=============================================================================

                      INCLUDE FILES

=============================================================================*/
#include <IxErrno.h>
#include <wci2.h>

/*===========================================================================

                        DATA TYPES AND MACROS

===========================================================================*/
#define WCI2_BITS_TO_SHIFT_DATA       3
#define WCI2_TYPE1_RESEND_REAL_TIME   0x01 //0b00001
#define WCI2_TYPE2_HSN_BIT            0x08 //0b01000
#define WCI2_TYPE2_DATA_MASK          0xf0
#define WCI2_TYPE2_BITS_TO_SHIFT_DATA 4
#define WCI2_TYPE2_BITS_TO_SHIFT_LSN  4
#define WCI2_TYPE2_BITS_TO_SHIFT_MSN  0
#define WCI2_TYPE3_MAX_INACT_DURN_MS  155
#define WCI2_TYPE_BITS_MASK           0x07 //0b00000111

/* Enum of all possible WCI2 types */
typedef enum 
{
  WCI2_TYPE0,
  WCI2_TYPE1,
  WCI2_TYPE2,
  WCI2_TYPE3,
  WCI2_TYPE4,
  WCI2_TYPE5,
  WCI2_TYPE6,
  WCI2_TYPE7,
} wci2_data_type_e;

/* TX Type 2 WCI2 substructure */
typedef struct
{
  uint16       length;
  const uint8* buffer;
} wci2_type2_type_s;

/* TX type 6 WCI2 substructure */
typedef struct
{
  uint8   sfn;
  boolean tx;
} wci2_type6_type_s;

/* TX type 7 WCI2 substructure */
typedef struct
{
  boolean wwan_tx_active;
  boolean wwan_tx_pwr_active;
  boolean tx_ant_sel;
} wci2_type7_type_s;

/* Union of all WCI2 TX types */
typedef union 
{
  uint8              unused;
  uint8              type1_byte;
  wci2_type2_type_s  type2_msg;
  uint16             type3_inact_durn;
  uint8              type4_freq_index;
  wci2_type6_type_s  type6_adv_tx_sfn;
  wci2_type7_type_s  type7_wwan_state;
} wci2_supported_data_type_u;

/* WCI2 TX message structure */
typedef struct
{
  wci2_data_type_e           type;
  wci2_supported_data_type_u data;
} wci2_msg_type_s;

/*=============================================================================

                      EXTERNAL FUNCTION DEFINITIONS

=============================================================================*/
/*=============================================================================

  FUNCTION:  wci2_send_msg

=============================================================================*/
/*!
    @brief
    send a message over CXM_UART in WCI2 format

    @return
    void
*/
/*===========================================================================*/
void wci2_send_msg( 
  const wci2_msg_type_s* msg
);

#endif /* WCI2_CORE_H */

