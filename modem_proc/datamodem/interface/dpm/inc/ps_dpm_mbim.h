#ifndef _PS_DPM_MBIM_MBIM
#define _PS_DPM_MBIM_MBIM
/*===========================================================================

                     PS_DPM_MBIM . H
DESCRIPTION
Header file describing global DPM config definitions for mbim processing
packets

Copyright (c) 2013 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                            EDIT HISTORY FOR FILE
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dpm/inc/ps_dpm_mbim.h#1 $
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $
                            
when        who    what, where, why
--------    ---    ----------------------------------------------------------
11/19/13    bvd     Initial Version
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "ps_rm_defs.h"

/**
  @brief    This function processes mbim packets and extacts IP packets 
            out of it and enqueues into corresponding watermarks.

  @param[in] rx_sig        DPM signal
  @param[in] user_data     User data info pointing to physical port
  
  @return  TRUE      Stop packet processing
  @return  FALSE     pkt processing complete.
  
  @code
  @endcode  
*/
boolean ps_dpm_mbim_rx_sig_hdlr
(
  ps_rm_sig_enum_type      rx_sig,
  void                * user_data
);

/**
  @brief    This function is called when data is enqueued to 
             physical port watermark

  @param[in] wm        Pointer to watermark
  @param[in] cb_data   Cb data assocaited with mbim pkt
  
  @code
  @endcode  
*/
void ps_dpm_mbim_phys_rx_wmk_data_cb
(
 dsm_watermark_type  * wm,
 void                * cb_data
);

/**
  @brief    This function is called when set data format function
             is called on physical port. It sets the signal
			 for mbim pkts and also allocates ndp buffer

  @param[in] phys_xport           Pointer to phys_port
  @param[in] ul_data_agg_max_size  Max ndp buffer size
  
  @code
  @endcode  
*/
void ps_dpm_mbim_phys_port_format_set
(
  void             * phys_xport,
  uint32             ul_data_agg_max_size
);

/**
  @brief     It will be called either when there is data in 
             physical port rx watermark or when all logical ports 
             are flow enabled and physical port goes from flow 
             disbaled to flow enabled state. It sets the 
             signal PS_RX_RMNET2_SIGNAL. This siganl is 
             cleared when atleast one logical port goes to
             flow disabled state (which results in physical
             port going to flow disabled state). When we
             recieve data in ps_dpm_mbim_rx_sig_hdlr, we
             check the flow mask if set we return TRUE which
             would result in clearing of signal in task function.
  
  @code
  @endcode  
*/
void ps_dpm_mbim_enable_data_flow
(
  void
);

/**
  @brief    This function is called when data arrives in logical
            sio port and mbim is negotiated. 

  @param[in] rx_sig           PS signal
  @param[in] user_data        user data
  
  @return  TRUE      Stop packet processing
  @return  FALSE     pkt processing complete.
  
  @code
  @endcode  
*/
boolean ps_dpm_mbim_ul_opt_ip_rx_sig_hdlr
(
  ps_rm_sig_enum_type    rx_sig,
  void                 * user_data
);

/**
  @brief    This function is called to intialize dpm ul opt 
            tx meta info ptr.
  
  @code
  @endcode  
*/
void ps_dpm_mbim_ul_opt_init
(
  void
);
#endif /* _PS_DPM_MBIM_MBIM */
