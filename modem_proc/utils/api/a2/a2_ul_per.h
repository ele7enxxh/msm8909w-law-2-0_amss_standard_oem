#ifndef A2_UL_PER_H
#define A2_UL_PER_H
/*!
  @file a2_ul_per.h

  @brief
   Describes the interface to the UL Per driver block.

   This is expected to be used outside of A2

  @ingroup per_implementation
*/

/*==============================================================================

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

==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/utils/api/a2/a2_ul_per.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
08/20/14   am      QMAP FC changes for JO
07/19/13   ars     QMAP FC Feature
03/28/13   vd      Added new fucntion a2_ul_per_get_aggr_thresholds()
10/19/10   yuw     Initial Checkin

==============================================================================*/

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

#include <sio.h>
#include <a2_sio_ext.h>

/*==============================================================================

                   EXTERNAL DEFINITIONS AND TYPES

==============================================================================*/
typedef struct
{
  /*! maximum number of supported bytes */
  uint32 max_num_bytes;
  /*! maximum number of supported packets */
  uint32 max_num_pkts;
} a2_ul_per_aggr_thresholds_s;

/*!
    @brief
    Template for the callback DS/client registers to receive response to QMAP
    commands.
 
    @param
    stream_id:  Stream ID on which response was received
    mux_id :  Corresponding mux id for the port of interest
    cmd_name: Command name as indicated in the QMAP control command spec
              1 = Command to enable flow control, i.e. flow disable
              2 = Command to disable flow control, i.e. flow enable
 
    cmd_type: Command type as indicated in the QMAP control command spec

    dsm_ptr: Pointer to the DSM item which contains the command payload. 
             The QMAP control header is stripped out of the dsm item and passed
             to DS/client. DS is expected to free this item
 
    trans_id: Transaction ID generated when QMAP command was sent towards BAM.
 
*/
typedef void (*a2_ul_per_qmap_rsp_cb_type)
(
  sio_stream_id_type              stream_id,
  uint8                           mux_id,
  uint8                           cmd_name,   
  a2_common_qmap_ctrl_cmd_type_e  cmd_type,
  dsm_item_type*                  dsm_ptr,
  uint32                          trans_id
);

/*==============================================================================

                    EXTERNAL FUNCTION PROTOTYPES

==============================================================================*/


/*==============================================================================

  FUNCTION:  a2_ul_per_tlp_ctl

==============================================================================*/
/*!
  @brief
  This function enables or disables TLP on the specific A2 port
  a2_port:   SIO_PORT_A2_RMNET_1 to SIO_PORT_A2_RMNET_2
  cmd:       TRUE  --> Enable / FALSE --> Disable

  This function is called by QMI module 

  @return
  TRUE      Successful
  FALSE     Wrong port ID or Don't support TLP
*/
/*============================================================================*/
boolean a2_ul_per_tlp_ctl
(
  /*! The A2 port which TLP will be setup */
  sio_port_id_type a2_port,
  /*! Enable or disable */
  boolean     cmd
);

/*==============================================================================

  FUNCTION:  a2_ul_per_get_aggr_thresholds

==============================================================================*/
/*!
  @brief
  This function is called by client to get the maximum number of packets and
  bytes for a particular type of aggregation
 
  @note
  aggr_thresholds_ptr has be non-NULL
 
  @return
  None
 
*/
/*============================================================================*/
void a2_ul_per_get_aggr_thresholds 
( 
  a2_mux_type_e                 mux_type,
  a2_ul_per_aggr_thresholds_s*  aggr_thresholds_ptr
);

/*===========================================================================

  FUNCTION:  a2_ul_per_register_qmap_rsp_cb

===========================================================================*/
/*!
    @brief
    Used to register a call back function that A2 calls when a response for a
    command is received on the UL
 
    @param
    cb_fn:  A DS/client function
 
*/
/*=========================================================================*/
void  a2_ul_per_register_qmap_rsp_cb
(
  a2_ul_per_qmap_rsp_cb_type   cb_fn
);
#endif /* A2_UL_PER_H */

