#ifndef A2_HDLC_H
#define A2_HDLC_H
/*!
  @file a2_hdlc.h

  @brief
  External framing/deframing interface to the a2 driver.

  
  @ingroup 
  @ingroup 
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


  Written permission of Qualcomm Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/utils/api/a2/a2_hdlc.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/08/11   yuw     Supported tethered DUN call on CSFB Fusion
05/07/10   yuw     Added CRC status for deframer call back.
04/19/10   yuw     Modified HDLC API
03/30/10   yuw     Consolidate configuration of endpoint-based frm/dfrm to use
                   the same action param
01/07/10   bn      Updated comment in a2_hdlc_config_sio_port()
12/22/09   bn      Added in support for endpoint PPP framing 
12/22/09   yuw     Check in on-target changes for framer and deframer
04/19/09   yuw     Initial version

===========================================================================*/


/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <dsm.h>
#include <a2_common.h>

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*===========================================================================

                             MACROS

===========================================================================*/

/*===========================================================================

                       STRUCTURES AND UNIONS

===========================================================================*/

/*!
  @brief
  Define Endpoint PPP based stream ID max
  For Endpoint PPP based, stream ID from 0 to 3
  
*/
#define A2_HDLC_ENDPOINT_PPP_BASE_STREAM_ID_MAX   3

/*!
  @brief
  Define packet PPP based stream ID max
  For packet PPP based, stream ID from 4 to 31
  
*/
#define A2_HDLC_PACKET_PPP_BASE_STREAM_ID_MAX   31

/*!
  @brief
  Enum to enable or disable HDLC framing/defrming in A2 
  
*/
typedef enum
{
  A2_HDLC_DISABLE=0,
  A2_HDLC_ENABLE  
} a2_hdlc_oper_e;

/*!
  @brief
  Enum of clients of HDLC framer 
  
*/
typedef enum {
  A2_HDLC_MODULE_PS = 0,
  
  A2_HDLC_MODULE_MAX = 8
} a2_hdlc_module_e;


/*!
  @brief 
  Data structure defining the variables for framing call back functions.

*/
typedef struct
{
  /*! stream id: 4..31  */
  uint8          stream_id;
  /*! unframed data dsm pointer */
  dsm_item_type* src_dsm_ptr; 
  /*! Framed data pointer. It will be set to NULL if Framing fails. */
  dsm_item_type* dest_dsm_ptr;
  /*! Framed data length */
  uint32         dest_len;
  /*! user data provided as part of framer api call valid values 0..63 */
  uint8          usr_data;
}a2_hdlc_frame_cb_status_s;

/*!
  @brief 
  Data structure defining the variables for deframing call back functions.
*/
typedef struct
{
  /*! stream id: 4..31 */
  uint8          stream_id;
  /*! framed data dsm pointer */
  dsm_item_type* src_dsm_ptr;
  /*! Deframed dsm pointer. It will be set to NULL if deframing fails. */
  dsm_item_type* dest_dsm_ptr;
  /*! Deframed data length */
  uint32         dest_len;
  /*! Deframed CRC status   0:FAIL / 1:PASS */
  boolean        crc_status;
  /*! user data provided as part of deframer api call valid values 0..63 */
  uint8          usr_data;
}a2_hdlc_deframe_cb_status_s;

/*!
  @brief
  Definition of call back function for framing
*/
typedef void (*a2_hdlc_frame_cb)
(
  a2_hdlc_frame_cb_status_s* frame_cb_status_ptr
);

/*!
  @brief
  Definition of call back function for deframing
*/
typedef void (*a2_hdlc_deframe_cb)
(
  a2_hdlc_deframe_cb_status_s* deframe_cb_status_ptr
);


/*
{
   // 0..31 tag ids
   tag_id: 5;
   usr_data: 6;
   src_dsm_ptr: 32
}
*/

/*!
  @brief
  HDLC framing pre-configuration parameters (static values)

  */
typedef struct
{
  /*! This bit indicates whether to insert CRC (2 bytes) at the end of the 
  frame. */
  boolean  insert_crc_flag;
  /*! This bit indicates whether to insert the start flag (0x7E) in the 
  beginning of the frame.  */
  boolean  insert_start_flag;
  /*! This bit indicates whether to insert the flag (0x7E) at the end 
  of the frame (after CRC bytes).  */
  boolean  insert_end_flag;
  /*! PPP config action : enable or disable. This action is only
  applied when using the endpoint PPP API: a2_hdlc_config_sio_port() */
  a2_hdlc_oper_e  action;
} a2_hdlc_frame_config_s;

/*===========================================================================

                    FUNCTION PROTOTYPES

===========================================================================*/

/*===========================================================================

  FUNCTION:  a2_hdlc_register_frame_cb

===========================================================================*/
/*!
    @brief
    For clients of A2 framer to register call back function.

    @return
    None
*/
/*=========================================================================*/
void a2_hdlc_register_frame_cb
(
  a2_hdlc_module_e module,
  a2_hdlc_frame_cb frame_cb_fn_ptr
);

/*===========================================================================

  FUNCTION:  a2_hdlc_register_deframe_cb

===========================================================================*/
/*!
    @brief
    For clients of A2 deframer to register call back function.

    @return
    None
*/
/*=========================================================================*/
void a2_hdlc_register_deframe_cb
(
  a2_hdlc_module_e module,
  a2_hdlc_deframe_cb deframe_cb_fn_ptr
);


/*===========================================================================

  FUNCTION:  a2_hdlc_frame_config

===========================================================================*/
/*!
    @brief
    Interface to pre-configure framing processing with static parameters

    @return
    None

    @notes
    Caller needs to provide full configuration in a2_hdlc_frame_config_s
*/
/*=========================================================================*/
void a2_hdlc_frame_config
(
  a2_hdlc_module_e     frm_module,
  a2_hdlc_frame_config_s  frm_config
);

/*===========================================================================

  FUNCTION:  a2_hdlc_frame

===========================================================================*/
/*!
    @brief
    Interface to start to frame

    @return
    none
    
    @notes
    Caller only needs to stream_id, all other parameters are pre-configured.
*/
/*=========================================================================*/
void a2_hdlc_frame
(
  a2_hdlc_module_e     frm_module,
  /*! valid range 4..31 */
  uint8                hdlc_stream_id,
  /*! bit valid range [31:0] */
  uint32               hdlc_accm,    
  /*! unframed dsm pointer */
  dsm_item_type*       src_dsm_ptr,
  uint16               src_len_in_bytes,
  /*! valid bits of usr_data are LSB 6 bits. This is will returned to the
      caller as part of the call back function registered by the module */
  uint8                usr_data      
);


/*!
    @brief
    Interface to start deframing the data with pre-configured parameters

    @return
    none

    @notes
    Caller only needs to stream_id.
*/
/*=========================================================================*/
void a2_hdlc_deframe
(
  a2_hdlc_module_e       defrm_module,
  /*! valid range 4..31 */
  uint8                  hdlc_stream_id,
  /*! framed dsm pointer */
  dsm_item_type *        src_dsm_ptr,
  uint16                 src_len,
  /*! valid bits of usr_data are LSB 6 bits. This will returned to the
      caller as part of the call back function registered by the module */
  uint8                  usr_data
);

/*===========================================================================

  FUNCTION:  a2_hdlc_config_sio_mdm

===========================================================================*/
/*!
    @brief
    Interface to config on flight PPP framing/Defaming  for a requested 
    SIO port SIO_PORT_A2_MDM only

    @argument list
    framed client module    a2_hdlc_module_e       frm_module,
    framer config           a2_hdlc_frame_config_s * frm_config_ptr,
    the hdlc_accm           uint32                 hdlc_accm

    @return
    none
    
    @notes
*/
/*=========================================================================*/
void a2_hdlc_config_sio_mdm
(
  /*! framed client module */
  a2_hdlc_module_e        frm_module,
  /*! framer config */
  a2_hdlc_frame_config_s  *frm_config,
  /*! bit valid range [31:0]. Note that frm_config->action is disable,
  the hdlc_accm is not used */
  uint32                  hdlc_accm    
);

/*===========================================================================

  FUNCTION:  a2_hdlc_config_sio_tethered

===========================================================================*/
/*!
    @brief
    Interface to config HDLC processing for tethered DUN call and a requested 
    SIO port SIO_PORT_SDIO_MUX_A2_DUN_0 only

    @argument list
    framed client module    a2_hdlc_module_e       frm_module,
    framer config           a2_hdlc_frame_config_s * frm_config_ptr,
    the hdlc_accm           uint32                 hdlc_accm

    @return
    none
    
    @notes
*/
/*=========================================================================*/
void a2_hdlc_config_sio_tethered
(
  /*! framed client module */
  a2_hdlc_module_e        frm_module,
  /*! framer config */
  a2_hdlc_frame_config_s  *frm_config_ptr,
  /*! bit valid range [31:0]. Note that frm_config->action is disable,
  the hdlc_accm is not used */
  uint32                  hdlc_accm    
);

#endif /* A2_HDLC_H */

