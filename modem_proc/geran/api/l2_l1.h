#ifndef L2_L1_H
#define L2_L1_H
/*========================================================================
                 GSM L2 to L1 DEFINITIONS FILE
DESCRIPTION
   This file contains definitions used by both GSM Layer 2 and Layer 1.

Copyright (c) 2001-2014 Qualcomm Technologies, Inc.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
========================================================================== */

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/api/l2_l1.h#1 $ $DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     --------------------------------------------------------
dd/mm/yy    SJW     Sample text for edit history
========================================================================== */

#ifndef CUSTOMER_H
  #include "customer.h"
#endif
#ifndef COMDEF_H
  #include "comdef.h"
#endif

#include "geran_api_rr_l2.h" /* FRAME_NUMBER_T */
#include "rr_l2_l1.h"
#include "sys_cnst.h"
#include "sys_type.h"
#include "geran_dual_sim.h"
#include "timetick.h"

typedef enum
{
   PH_DATA_REQ,
   PH_EMPTY_FRAME_REQ,
   PH_RANDOM_ACCESS_REQ,
   PH_DATA_IND,
   PH_CONNECT_IND,
   PH_READY_TO_SEND_IND,
   PH_RANDOM_ACCESS_CNF,
   PH_STOP_SACCH_MEASUREMENT_REPORT,
   PH_DATA_CNF,
   PH_RANDOM_ACCESS_IND
} l2_l1_message_id_enum_T;


/**********************************************************/
/* PH_DATA_IND primitive :                                */
/* FROM :  LAYER 1                                        */
/* TO   :  LAYER 2                                        */
/* DESCRIPTION :                                          */
/*                                                        */
/*   Message sent from layer 1 to layer 2 when a frame    */
/* has been received.                                     */
/*                                                        */
/**********************************************************/

typedef struct
{
   IMH_T             message_header;
   boolean           own_paging_block;
   ARFCN_T           channel_number;
   l2_channel_type_T l2_channel_type;
   uint32            cbch_frame;
   byte              tc;
   byte              l2_repetition_order;
   gas_id_t          gas_id;
   boolean           dual_sim_mode_enabled;
   byte              layer2_frame[N201_MAX]; /* this field MUST be last because GL1 may truncate the message */
} ph_data_ind_T;

/**********************************************************/
/* PH_RANDOM_ACCESS_CNF primitive :                       */
/* FROM :  LAYER 1                                        */
/* TO   :  LAYER 2                                        */
/* DESCRIPTION :                                          */
/*                                                        */
/*   Message sent to layer 2 when layer 1 has either      */
/* transmitted a RACH burst or been denied by TRM/CXM     */
/*                                                        */
/**********************************************************/

typedef struct
{
   IMH_T             message_header;
   FRAME_NUMBER_T    frame_number;
   l2_channel_type_T l2_channel_type;
   uint16            random_access_content;
   geran_result_t    result;
   gas_id_t          gas_id;
} ph_random_access_cnf_T;

/**********************************************************/
/* PH_CONNECT_IND primitive :                             */
/* FROM :  LAYER 1                                        */
/* TO   :  LAYER 2                                        */
/* DESCRIPTION :                                          */
/*                                                        */
/*   Message from layer 1 to layer 2 indicating that a    */
/* specific type of channel has been connected at the     */
/* physical layer                                         */
/*                                                        */
/**********************************************************/

typedef struct
{
   IMH_T             message_header;
   channel_type_T    connected_channel;
   gas_id_t          gas_id;
} ph_connect_ind_T;

/**********************************************************/
/* PH_READY_TO_SEND_IND primitive :                       */
/* FROM :  LAYER 1                                        */
/* TO   :  LAYER 2                                        */
/* DESCRIPTION :                                          */
/*                                                        */
/*    Message from layer 1 to layer 2 indicating that the */
/* buffer in layer 1 corresponding to the channel type    */
/* is available to receive the next message to transmit   */
/* on that channel                                        */
/*                                                        */
/**********************************************************/

typedef struct
{
   IMH_T             message_header;
   l2_channel_type_T l2_channel_type;
   gas_id_t          gas_id;
} ph_ready_to_send_ind_T;

typedef enum
{
  GL2_CB_BLOCK_CNF,
  GL2_MAX_DATA_CNF
} ph_data_cnf_type_t;

/**********************************************************/
/* PH_DATA_CNF primitive :                                */
/* FROM :  LAYER 2                                        */
/* TO   :  LAYER 1                                        */
/* DESCRIPTION :                                          */
/*                                                        */
/*    Message from layer 2 to layer 1 indicating that the */
/* PH_DATA_IND primitive has been processed               */
/*                                                        */
/**********************************************************/
typedef struct
{
   IMH_T              message_header;
   ph_data_cnf_type_t data_type;
   uint32             frame_number;
   gas_id_t           gas_id;
} ph_data_cnf_T;

#endif
