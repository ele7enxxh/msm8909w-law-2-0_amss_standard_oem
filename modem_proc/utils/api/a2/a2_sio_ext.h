#ifndef A2_SIO_EXT_H
#define A2_SIO_EXT_H
/*!
  @file a2_sio_ext.h

  @brief
   Interface to the A2 SIO module.

   This SIO interface is to make the A2 appear as a generic serial port
   to it's client (PS/DS).  This interface relates to data sent from PS
   out a USB port.  All data sent and received on this interface will not
   be sent the ipfiltering engine and is unciphered.

   One SIO port corresponds to one usb endpoint.

  @ingroup a2_ext_interface
*/

/*==============================================================================

  Copyright (c) 2013 QUALCOMM Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  QUALCOMM Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of QUALCOMM Technologies Incorporated.

==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/utils/api/a2/a2_sio_ext.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
07/29/14   ars     Single QMUX for embedded channel FR 20810/CR 665898
06/18/14   ars     MSM QMAP aggr feature for teth rmnet
01/21/14   ars     QMAP minimum pad negotiation 
11/22/13   vd      CR563027: a2_sio_clear_dl_bam() returns boolean
09/13/13   vd      CR469984: Support for mount/unmount feature
07/11/13   ars     QNX Bearer ID changes
05/29/13   yjz     CR493599, add a2_sio_update_unique_id
04/13/13   ar      CR 471031 Dissolved a2_sio_reset_sps_bam_pipe() api into 
                   a2_sio_disable_sps_bam_pipe() & a2_sio_enable_sps_bam_pipe()
03/20/13   ar      CR465878: Added QMAPv2 offload feature
01/23/13   ars     QMAP changes for muxing
09/14/12   ar      exported a2_sio_dl_bam_is_empty and a2_sio_ul_bam_is_empty 
                   apis
07/06/12   rp      CR371608 Added api's to enable/disable A2 peak data rate mode
06/08/12   yuw     Added A2_SIO_SPS_BAM_INVALID for BAM modem enum
04/13/12   ar      fixed compilation warning
04/11/12   bn      Added in support for DIME
04/08/12   st      CR 346025: Changed return type of a2_sio_set_hdr
                   for a given USB port (intended for Data Services)
02/13/12   ar      added support for A2 PC + SPS RESET feature
03/23/12   st      Fixed a compilation error due to previous check in
03/14/12   bn      CR343915: Call explicit func to intialize BAM after power-on
02/28/11   yuw     Removed dun signal handler prototype
02/08/11   yuw     Supported tethered RmNet and DUN call on CSFB Fusion
08/11/10   yuw     Fixed on-target compilation error
08/03/10   bn      Moved the definition of external hdrs to this header file
07/08/10   bn      Added in SW workaround to send frag-ip packet to endpoint
05/07/10   yuw     Added a feature to support FUSION loopback test
05/04/10   bn      Added in support for A2 DL muxing (FEATURE_A2_FUSION)
04/21/10   yuw     Initial Checkin

==============================================================================*/

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

#include <sio.h>
#include <a2_common.h>
#include <a2_sio.h>
#include <a2_ext.h>
#include <a2_dl_per.h>

/*==============================================================================

                               MACROS

==============================================================================*/


/*==============================================================================

                   EXTERNAL DEFINITIONS AND TYPES

==============================================================================*/

/*! Structure for storing the last configured lengths of the
  external packet headers to be added.
  */
typedef struct
{
  /*! external header data */
  uint8  hdr_data[A2_HW_MAX_HDR_SIZE_IN_BYTES];
  /*! Length of the header */
  uint8  hdr_len;
} a2_pkt_hdr_s;


/*! Enumeration for configuring QMAP v2 */
typedef enum
{
  /* Un-initialized */
  A2_CS_NONE = 0,
  /* Disable CS */
  A2_CS_DISABLE,
  /* Enable CS */
  A2_CS_ENABLE,
  /* Max for boundary check */
  A2_CS_MAX
}a2_cs_action_e;

/* !@brief A2 BAM dynamic configuration enumeration type*/
typedef enum
{
  /*! BAM to BAM mode */
  A2_SIO_SPS_BAM2BAM = 0,
  /*! BAM system mdoe */
  A2_SIO_SPS_BAM2SYS,
  A2_SIO_SPS_BAM_INVALID
} a2_sio_bam_dynamic_bam_e;

/*! @brief define enumuration for Call state */
typedef enum
{
  A2_SIO_CALL_STATE_TETH_DOWN = 0,
  A2_SIO_CALL_STATE_TETH_UP,
  A2_SIO_CALL_STATE_MAX
} a2_sio_call_state_e;

typedef struct
{
  /*! Stream_id for the port */
  sio_stream_id_type  open_port_stream_id;
  /*! SIO port ID for each stream_id */
  sio_port_id_type   sio_port_id;
  /*! Type of MUX */
  a2_mux_type_e  mux_type;
  /*! MUX index */
  uint8  mux_idx;
  /*! QMAP v2 DL action */
  a2_cs_action_e  dl_cs_action;
  /*! QMAP v2 UL action */
  a2_cs_action_e  ul_cs_action;
  /*! Stream_id for the master endpoint that openning port is associated */
  sio_stream_id_type  master_port_stream_id;
  /*! sio ul watermark */
  dsm_watermark_type   *ul_wm;
  /* action for DL aggregation */
  a2_dl_per_tlp_action_e  dl_aggregation;
  /*! NCM header sig value */
  uint32                 ncm_sig;
  /*! NDP header sig value */
  uint32                 ndp_sig;
  /*! Negotiated minimum pad that must be added at the end of every packet. 
    Max value can be 64 bytes */
  uint8                  nego_min_pad;
} a2_sio_port_cfg_s;

typedef struct
{
  /*! SIO port ID for each stream_id */
  sio_port_id_type   sio_port_id;
  /*! Corresponding USB/SMD port to be opened */
  sio_port_id_type   a2_to_usb_port_id;
  /*! MUX index */
  uint8  mux_idx;
  /* BAM logical endpoint ID.*/
  uint8 ep_id;
  /*! index of port_info table  */
  uint8 subchan_index;
  /*! The endpoint we save the port info. This could
  be Endpoint 0,1,2 or ext_mem */
  uint8 stored_endpoint_index;
  /*! The physical endpoint of the port */
  uint8 bam_endpoint_index;
  /*! Type of MUX */
  a2_mux_type_e  mux_type;
  /*! QMAP v2 DL action */
  a2_cs_action_e  dl_cs_action;
  /*! QMAP v2 UL action */
  a2_cs_action_e  ul_cs_action;
  /* action for DL aggregation */
  a2_dl_per_tlp_action_e  dl_aggregation;
  /*! Stream id of the master port for an endpoint */
  sio_stream_id_type master_stream_id;
  /*! Lookup table from a2 stream to usb stream */
  sio_stream_id_type a2_to_usb_stream_id;
  /*! Negotiated minimum pad bytes */
  uint8              nego_min_pad;
} a2_sio_port_mapping_info_s;

/*==============================================================================

                    EXTERNAL FUNCTION PROTOTYPES

==============================================================================*/

/*==============================================================================

  FUNCTION:  a2_sio_set_hdr

==============================================================================*/
/*!
  @brief
  Sets the header to be added for the specified usb port.

  @return Returns True if this request to set hdr succeeded and False
  otherwise

*/
/*============================================================================*/
boolean a2_sio_set_hdr
(
  /* The destination on which this header is to be applied */
  sio_port_id_type port_id,
  /*! The type of traffic to apply this header to */
  a2_ip_type_e ip_type,
  /*! Pointer to the header to be applied */
  uint8* hdr,
  /*! The length of the header must be less than
    A2_HW_MAX_HDR_SIZE_IN_BYTES */
  uint8 hdr_length
);

/*==============================================================================

  FUNCTION:  a2_sio_lookup_mux_idx

==============================================================================*/
/*!
  @brief Looks up the mux index number, based upon the embedded call port ID.

  @return The MUX index.
*/
/*============================================================================*/
uint8 a2_sio_lookup_mux_idx
(
  sio_port_id_type a2_port
);


/*==============================================================================

  FUNCTION:  a2_sio_lookup_usb_port

==============================================================================*/
/*!
  @brief Looks up the USB port number, based upon the A2 port number.

  @return The usb port id.
*/
/*============================================================================*/
sio_port_id_type a2_sio_lookup_usb_port
(
  sio_port_id_type a2_port
);

/*==============================================================================

  FUNCTION:  a2_sio_lookup_tbl_by_port_id

==============================================================================*/
/*!
  @brief Function to get the point of the matched lookup table by port id. This
  is used only to lookup by data port Id. 

*/
/*============================================================================*/
a2_sio_port_mapping_info_s * a2_sio_lookup_tbl_by_port_id
(
  sio_port_id_type port_id
);

/*==============================================================================

  FUNCTION:  a2_sio_lookup_tbl_by_stream_id

==============================================================================*/
/*!
  @brief Function to get the point of the matched lookup table by stream id.

*/
/*============================================================================*/
a2_sio_port_mapping_info_s * a2_sio_lookup_tbl_by_stream_id
(
  sio_stream_id_type a2_stream_id
);

/*==============================================================================

  FUNCTION:  a2_sio_find_usb_idx_by_portid

==============================================================================*/
/*!
  @brief Converts a SIO Port id to a usb index.

  The core of the driver does not understand port ids, only usb indices.

*/
/*============================================================================*/
uint8 a2_sio_find_usb_idx_by_portid
(
  sio_port_id_type   port_id
);

/*==============================================================================

  FUNCTION:  a2_sio_get_tx_watermark

==============================================================================*/
/*!
  @brief
  Return the tx watermark of the endpoint and mux index.


*/
/*============================================================================*/
dsm_watermark_type* a2_sio_get_tx_watermark
(
  uint8 endpoint,
  uint8 mux_index
);

/*==============================================================================

  FUNCTION:  a2_sio_disable_sps_bam_pipe

==============================================================================*/
/*!
  @brief 
  Disables the SPS BAM pipe for a port.
  NOTE: diabling the bam pipe will also result in reseting that pipe

*/
/*============================================================================*/
void a2_sio_disable_sps_bam_pipe
(
  sio_port_id_type port_id
);

/*==============================================================================

  FUNCTION:  a2_sio_enable_sps_bam_pipe

==============================================================================*/
/*!
  @brief 
  Enables the SPS BAM pipes for a port.

*/
/*============================================================================*/
void a2_sio_enable_sps_bam_pipe
(
  sio_port_id_type port_id
);

/*==============================================================================

  FUNCTION:  a2_sio_reset_sps_trigger

==============================================================================*/
/*!
  @brief 

*/
/*============================================================================*/
void a2_sio_reset_sps_trigger(void);

/*==============================================================================

  FUNCTION:  a2_sio_get_dl_pipe_handle_for_bam2sys

==============================================================================*/
/*!
  @brief Get BAM DL pipe handle in BAM2SYS mode/8974. 
*/
/*============================================================================*/
void a2_sio_get_dl_pipe_handle_for_bam2sys(void);

/*==============================================================================

  FUNCTION:  a2_sio_clear_dl_bam

==============================================================================*/
/*!
  @brief Clears out A2 DL BAM by SPS hack function in BAM2SYS mode/8974. Returns
  true if the SPS hack function called successfully  
*/
/*============================================================================*/
boolean a2_sio_clear_dl_bam(void);

/*==============================================================================

  FUNCTION:  a2_sio_dl_bam_is_empty

==============================================================================*/
/*!
  @brief  Check if downlink SPS pipes are empty

*/
/*============================================================================*/
boolean a2_sio_dl_bam_is_empty(void);

/*==============================================================================

  FUNCTION:  a2_sio_ul_bam_is_empty

==============================================================================*/
/*!
  @brief  Check if uplink SPS pipes are empty

*/
/*============================================================================*/
boolean a2_sio_ul_bam_is_empty(void);

/*==============================================================================

  FUNCTION:  a2_sio_bam_init_at_a2_wakeup

==============================================================================*/
/*!
  @brief
  This function is called to initialize the BAM after A2 is woken up from the
  power collapse
  
  @detail
  First time after modem bootup, BAM is initialized via a2_sio_open() by data
  services. From that point on, whenever A2 goes into power collapse and wakes
  up, this api is used to initialize the BAM
 
  @todo
  Make this api compatible with BAM2BAM and BAM2SYS type pipe inits.
 
  @return
  none

*/
/*============================================================================*/
void a2_sio_bam_init_at_a2_wakeup
(
  void
);

/*==============================================================================

  FUNCTION:  a2_sio_bam_deinit_at_a2_pc

==============================================================================*/
/*!
  @brief
  This function is called to de-init the BAM after A2 is woken up from the
  power collapse
  
  @detail
  Existing connected pipes need to be disconnected before a2 goes into PC
 
  @todo
  Make this api compatible with BAM2BAM and BAM2SYS type pipe inits.
 
  @return
  none

*/
/*============================================================================*/
void a2_sio_bam_deinit_at_a2_pc
(
  void
);

/*==============================================================================

  FUNCTION:  a2_sio_reset_sps_bam

==============================================================================*/
/*!
  @brief This api resets the a2 bam
  
  @detail
  Each power on, A2 needs to call this api before sending the modem power
  wake up smsm indication to apps side. This way A2 BAM will always be in
  initialized state before apps starts configuring it.
 

*/
/*============================================================================*/
void a2_sio_reset_sps_bam
(
  void
);

/*==============================================================================

  FUNCTION:  a2_sio_enable_peak_data_rate_support

==============================================================================*/
/*!
  @brief 
   Configures the modem to run with the max resource configuration to support
   peak data rate.
 
  @detail
   enable_peak_q6_frequency = 1, sets
   1. Max Q6 clock frequency.
   2. Peak system fabric bus frequency.
   3. Peak daytona fabric frequency.
   4. Peak system FPB bus frequency.
   5. Peak chip FPB bus frequency.
   6. Peak ebi1 memory bus frequency.

   enable_peak_offline_clock = 1, sets
   1. Offline clock which feeds the modem/A2 to run at peak speed.

  @side effects
   Call to this API configures max resource allocation which would result in
   very high power consumption. Responsibility of the caller to disable,
   when not reqd.
*/
/*============================================================================*/
boolean a2_sio_enable_peak_data_rate_support
(
  boolean enable_peak_q6_frequency,
  boolean enable_peak_offline_clock
);

/*==============================================================================

  FUNCTION:  a2_sio_disable_peak_data_rate_support

==============================================================================*/
/*!
  @brief 
   Restores all the resource allocation done by the
   a2_sio_enable_peak_data_rate_support

*/
/*============================================================================*/
boolean a2_sio_disable_peak_data_rate_support
(
  void
);

/*==============================================================================

  FUNCTION:  a2_sio_set_dynamic_port_cfg

==============================================================================*/
/*!
  @brief API to configure port dynamically after port is opened by DS

  @return:
  A return code indicating the success of the function or not.  
  A2_SUCCESS = 0,
  A2_ERROR_INVALID_FIRST_ARGUMENT
  A2_ERROR_FEATURE_NOT_AVAILABLE,  Do not support TLP on the specific port 

*/
/*============================================================================*/
a2_error_e a2_sio_set_dynamic_port_cfg
(
  a2_sio_port_cfg_s *port_cfg_p
);

/*==============================================================================

  FUNCTION:  a2_sio_notify_call_state

==============================================================================*/
/*!
  @brief
  API to convey to A2 the nature of call and the state of call
  Called by DS to indicate if a tethered call is UP/DOWN

*/
/*============================================================================*/
void a2_sio_notify_call_state
(
  a2_sio_call_state_e   call_state,
  sio_port_id_type         port_id
);

#endif /* A2_SIO_EXT_H */


