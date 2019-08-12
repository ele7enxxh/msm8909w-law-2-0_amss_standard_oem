#ifndef DS_FLOW_CONTROL_H
#define DS_FLOW_CONTROL_H

/** 
  @file ds_flow_control.h
  @brief
   Contains common data definitions related to flow control masks.
*/

/*=============================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The dms_mainpage.dox file contains all file/group descriptions 
      that are in the output PDF generated using Doxygen and Latex. To edit or 
      update any of the file/group text in the PDF, edit the 
      dms_mainpage.dox file or contact Tech Pubs.

      The above description for this file is part of the DSFlowControl
      group description in the dms_mainpage.dox file. 
=============================================================================*/
/*===========================================================================

                    D S   F L O W   C O N T R O L
                        H E A D E R  F I L E

Copyright (c) 2002-2011 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/public/ds_flow_control.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
03/22/11    sn     Added Doxygen documentation.
08/13/09    vk     Initial merge of LTE support.
03/04/09    sa     AU level CMI modifications.
07/15/08    ar     Added DS_FLOW_PROXY_MASK
08/16/07    as     Added DS_FLOW_NO_SERVICE_MASK
10/09/06    ks     Added DS_FLOW_RMSM_MASK for any RmSm.
08/31/06    rsl    PDCP Featurization changes.
10/03/05    vas    Added DS_FLOW_PS_FLOW_TX_WM_MASK
01/12/05    aku    Added DS_FLOW_WLAN_IFACE_HDLR_MASK
11/15/04    kvd    Added Flow control masks for CAM module.
03/09/04    ak     Fixed flow masks which had same bit mask.
02/04/04    ak     Added flow control for phys links and for 707 task.
02/03/04    aku    Added support for logical ifaces
05/15/03    vas    Added DS_FLOW_HDRAN_RLP_MASK
05/05/03    usb    Added DS_FLOW_DSM_LEVEL_MASKS which constitutes all 
                   memory level masks.
04/18/03    vsk    added DS_FLOW_UMTS_RMSM_MASK instead of DS_FLOW_GEN_RMSM_MASK
02/12/03    atp    Added flow mask for 707 circuit.
12/29/02    jd     Added DS_FLOW_DSSNET_REG_MASK
12/18/02    TMR    Added DS_FLOW_GCSD_MASK and DS_FLOW_WCDMACS_MASK
11/14/02    usb    Added bogus flow mask for multiple mip sessions (future)
11/12/02    usb    Reversed the flow control enable/disable logic and defined 
                   a new overriede mask.
10/11/02    usb    Added flow mask for generic RMSM.
10/03/02    rc     Added flow masks for UMTS.
08/26/02    mvl    Added flow mask for ps_iface, modified M.IP flow masks.
07/19/02    aku    Added flow mask for dssnet.
05/17/02    ak     Added holddown timer mask.
05/15/02    rsl    Initial version of file.
===========================================================================*/
/* group: dsflowcontrol */
/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "comdef.h"


/** @addtogroup constants 
  @{
*/
/**
  @brief Masks to enable/disable flow control.
*/
#define DS_FLOW_ALL_MASK          0xFFFFFFFF  /**< Flow enable/disable 
                                                   override. */
#define DS_FLOW_IS_ENABLED        0x00000000  /**< All flows enabled flag. */
#define DS_FLOW_DSM_LARGE_Q_MASK  0x00000001  /**< Out of large items mask. */
#define DS_FLOW_DSM_SMALL_Q_MASK  0x00000002  /**< Out of small items mask. */
#define DS_FLOW_PKT_WM_MASK       0x00000004  /**< Packet mode watermark 
                                                   mask. */
#define DS_FLOW_707_CKT_TCP_MASK  0x00000008  /**< 707 circuit TCP Sendq 
                                                   mask. */
#define DS_FLOW_NETMODEL_MASK     0x00000010  /**< Mask for netmodel. */
#define DS_FLOW_707_PKT_MASK      0x00000020  /**< 707 packet controller 
                                                   mask. */
#define DS_HOLDDOWN_TIMER_MASK    0x00000040  /**< 707 packet holddown timer 
                                                   mask. */
#define DS_FLOW_707_RLP_MASK      0x00000080  /**< RLP high watermark flow 
                                                   mask. */
#define DS_FLOW_CTRL_DSSNET4_MASK 0x00000100  /**< DSSNET flow mask. */
#define DS_FLOW_PS_IFACE_MASK     0x00000200  /**< PS IFACE self mask. */
#define DS_FLOW_UMTS_MASK         0x00000400  /**< UMTS packet controller 
                                                   mask. */
#define DS_FLOW_PROXY_MASK        0x00000400  /**< Proxy IFACE controller 
                                                   mask. */
#define DS_FLOW_LL_MASK           0x00000800  /**< Mask for all LL events. */
#define DS_FLOW_PDCP_MASK         DS_FLOW_LL_MASK  /**< PDCP high watermark 
                                                        flow mask. */
#define DS_FLOW_LTE_PDCP_MASK     DS_FLOW_LL_MASK  /**< LTE PDCP mask. */
#define DS_FLOW_RLC_MASK          DS_FLOW_LL_MASK  /**< RLC mask. */
#define DS_FLOW_SNDCP_MASK        DS_FLOW_LL_MASK  /**< SNDCP mask. */
#define DS_FLOW_UMTS_RMSM_MASK    0x00001000  /**< UMTS RMSM mask. */
#define DS_FLOW_707_RMSM_MASK     0x00002000  /**< 707 Packet RMSM mask. */
#define DS_FLOW_RMSM_MASK         0x00004000  /**< RMNET mask. */
#define DS_FLOW_GCSD_MASK         0x00008000  /**< GSM CS data flow mask. */
#define DS_FLOW_WCDMACS_MASK      0x00010000  /**< WCDMA CS data flow mask. */
#define DS_FLOW_DSSNET_REG_MASK   0x00020000  /**< DSSNET in MIP registration 
                                                   mask. */
#define DS_FLOW_707_CKT_MASK      0x00040000  /**< 707 circuit switched 
                                                   mask. */
#define DS_FLOW_HDRAN_RLP_MASK    0x00080000  /**< HDRAN RLP high watermark 
                                                   flow mask. */
#define DS_FLOW_PHYS_LINK_MASK    0x00100000  /**< Phys link flow control 
                                                   mask. */
#define DS_FLOW_707_LOCK_MASK     0x00200000  /**< 707 lock flow control 
                                                   mask. */
#define DS_FLOW_CTRL_DSSNET6_MASK 0x00400000  /**< DSSNET6 flow mask. */
#define DS_FLOW_ASSOC_IFACE_MASK  0x00800000  /**< Associated IFACE flow 
                                                   control mask. */
#define DS_FLOW_PS_FLOW_MASK      0x01000000  /**< Flow mask for PS FLOW. */
#define DS_FLOW_CAM_MASK          0x02000000  /**< Flow mask used for CAM 
                                                   module initiated flow 
                                                   control. */
#define DS_FLOW_CTRL_CAM_MASK     0x04000000  /**< Flow mask used for CAM
                                                   module initiated flow ctrl */
#define DS_FLOW_WLAN_IFACE_HDLR_MASK 0x08000000 /**< WLAN mode flow control 
                                                     mask. */
#define DS_FLOW_PS_FLOW_TX_WM_MASK 0x10000000 /**< Mask for transmit watermark 
                                                   associated with a PS flow. */
#define DS_FLOW_NO_SERVICE_MASK   0x20000000  /**< Mask used to control flow 
                                                   during service 
                                                   interruption. */
#define DS_FLOW_COEX_MASK         0x40000000  /**< Mask for DSDA Co-ex issue */
#define DS_FLOW_MIP_SOL_MASK      0x80000000  /**< MIP solicitation mask */

/** @} */ /* end_addtogroup constants */

/** @addtogroup macros_dsFlowcontrol
  @{
*/
/**
  @brief Macro that represents memory level flow masks.
*/

#define DS_FLOW_DSM_LEVEL_MASKS (DS_FLOW_DSM_LARGE_Q_MASK |     \
                                 DS_FLOW_DSM_SMALL_Q_MASK)


/**
  @brief Based on whether flow needs to be enabled or not, sets the value of static mask.
*/
#define DS_FLOW_CTRL_SET_MASK( flow, static_mask, in_mask)      \
  if (flow == DS_FLOW_ENABLE)                                   \
  {                                                             \
    static_mask &= ~(in_mask);                                  \
  }                                                             \
  else                                                          \
  {                                                             \
    static_mask |= (in_mask);                                   \
  }

/**
  @brief Macro which indicates if a particular bit in the flow mask is set.
*/
#define DS_FLOW_CTRL_IS_BIT_SET(flow_mask, bit)                 \
  ((((flow_mask) & (bit)) == (bit)) ? TRUE : FALSE)

/** @} */ /* end_addtogroup macros_ipUtilities */

/** @addtogroup datatypes
  @{
*/
/**
  @brief Flow control variable to be passed to set SIO INBOUND flow.
*/
typedef enum
{
  DS_FLOW_ENABLE  = 0,    /**< Use this to enable FLOW */
  DS_FLOW_DISABLE = 1     /**< Use this to disable FLOW */
} ds3g_flow_e_type;

/** @} */ /* end_addtogroup datatypes */

#endif /* DS_FLOW_CONTROL_H */
