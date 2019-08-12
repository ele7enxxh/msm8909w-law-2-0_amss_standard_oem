/*!
  @file
  lte_l2_comdef.h

  @brief
  This file contains common defines for LTE L2 layer.


*/

/*===========================================================================

  Copyright (c) 2011 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/api/lte_l2_comdef.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/21/12   st      CR 384924: Scell MCE support added
01/05/12   st      Added definitions for eMBMS
07/18/11   mm      Added defintions for eMBMS
01/26/10   bn      Added in the invalid LCID 
11/06/09   bn      Moved LTE_L2_MALLOC_A2_MEMORY/LTE_L2_FREE_A2_MEMORY to
                   lte_l2_common.h 
04/29/09   ax      Added log subpkt id base for MAC,RLC, and PDCP.
03/12/09   yg      Added in macros for allocation/free memory for A2 from 
                   QDR memory for Slimcat platform 
02/09/09   bn      Added in macros for allocation/free memory for A2
07/07/08   bqiu    Initial version
===========================================================================*/

#ifndef LTE_L2_COMDEF_H
#define LTE_L2_COMDEF_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include <comdef.h>
#include <amssassert.h>
#include <IxErrno.h>
#include <lte.h>
#include <lte_as.h>

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! @brief subpkt ID base for MAC
*/
#define LTE_MAC_LOG_SUBPKT_ID_BASE                              0x00

/*! @brief subpkt ID MAX for MAC
*/
#define LTE_MAC_LOG_SUBPKT_ID_MAX                               0x3F

/*! @brief subpkt ID base for RLC
*/
#define LTE_RLC_LOG_SUBPKT_ID_BASE                              0x40

/*! @brief subpkt ID MAX for RLC
*/
#define LTE_RLC_LOG_SUBPKT_ID_MAX                               0xBF

/*! @brief subpkt ID base for PDCP
*/
#define LTE_PDCP_LOG_SUBPKT_ID_BASE                             0xC0

/*! @brief subpkt ID MAX for PDCP
*/
#define LTE_PDCP_LOG_SUBPKT_ID_MAX                              0xFF

/*! @brief Total maximum number of supported active AM Data Radio Bears
*/
#define LTE_MAX_ACTIVE_AM_DRB   8

/*! @brief Total maximum number of supported active AM Signal Radio Bears
*/
#define LTE_MAX_ACTIVE_AM_SRB   2

/*! @brief Types of LCID found on DL-SCH.*/

/*<  CCCH LCID */
#define LTE_L2_DLSCH_LCID_CCCH                 0                              

/*<  Logical channel LCID range begin */
#define LTE_L2_DLSCH_LCID_CHAN_BEGIN           1

/*<  Logical channel LCID range begin */
#define LTE_L2_DLSCH_LCID_CHAN_END             10   
                           
/*<  UE Scell act/deact LCID */                           
#define LTE_L2_DLSCH_LCID_SCELL_ACT_DEACT      27
                           
/*<  UE Contention Resolution LCID */                           
#define LTE_L2_DLSCH_LCID_UE_CONT_RES          28

/*<  Timing Advance Command LCID */
#define LTE_L2_DLSCH_LCID_TA_CMD               29

/*<  DRX Command LCID */
#define LTE_L2_DLSCH_LCID_DRX_CMD              30

/*<  Padding LCID */
#define LTE_L2_LCID_PADDING                    31

/*<  Invalid LCID  */
#define LTE_L2_DLSCH_LCID_INVALID              32


/*! @brief Types of LCID found on MCH.*/

/*< MTCH LCID range begins */
#define LTE_L2_MCH_LCID_MTCH_BEGIN             0

/*< MTCH LCID range ends */
#define LTE_L2_MCH_LCID_MTCH_END               28

/*< MSI MAC control element */
#define LTE_L2_MCH_LCID_MSI                    30

/*<  Invalid LCID  */
#define LTE_L2_MCH_LCID_INVALID                32


/*! @brief L2 configuration reason
*/
typedef enum
{
  LTE_L2_CFG_RECFG = (0x1 << 0),          /*!< regular configuration */
  LTE_L2_CFG_HO = (0x1 << 1),             /*!< handover */
  LTE_L2_CFG_RELEASE  = (0x1 << 2),       /*!< connection release for unicast RBs/LCs */
  LTE_L2_CFG_RADIO_FAILURE  = (0x1 << 3), /*!< radio failure */
  LTE_L2_CFG_EMBMS  = (0x1 << 4),         /*!< eMBMS configuration */
  LTE_L2_CFG_RELEASE_ALL_EMBMBS  = (0x1 << 5),   /*!< Release all for multicast RBs/LCs */
  LTE_L2_CFG_RELEASE_ALL  = (0x1 << 6),   /*!< Release all for unicast and multicast RBs/LCs */
  LTE_L2_CFG_CONN_CANCEL  = (0x1 << 7)    /*!< connection cancel. Intenal use in L2 only*/
} lte_l2_cfg_reason_e;


#endif /* LTE_L2_COMDEF_H */

