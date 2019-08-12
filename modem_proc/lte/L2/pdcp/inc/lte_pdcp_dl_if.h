/*!
  @file
  lte_pdcp_dl_if.h

  @brief
  This file contains all External interfaces exported by PDCP DL layer.

  @author
  bqiu, gxiao
*/

/*===========================================================================

  Copyright (c) 2015 QUALCOMM Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regdlated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  QUALCOMM Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of QUALCOMM Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODDLE

This section contains comments describing changes made to the moddle.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/L2/pdcp/inc/lte_pdcp_dl_if.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/08/15   sb      CR775692: Reduce threshold of OFFLOAD-PDCP Wm
11/20/14   sb      CR756271: Configure A2 Wm correspond to CAT4
12/13/13   mg      CR589750: Parameter Tweaking for BOLT - making PDCPDL_A2_WM_DNE_COUNT
                   dependent on LTE_PDCPDL_A2_WM_DNE
12/13/13   mg      CR589750: Parameter Tweaking for BOLT considering high 
                   throughput requirement and RLC BLER - LTE side
09/16/13   mg      CR501406: Packet based flow control required for DSM items to 
                   efficiently support Small IP packet tests at high data rates 
05/13/13   sb      CR484633: Avoid crash because of PDCPDL message Q full with 
                   CRYPTO_DONE_IND message
05/18/10   ax      consolidate pdcp external APIs and place them under lte/api/
04/26/10   ax      Added support for API to query the DL DRB activities
03/05/10   ax      added #include <pthread.h> making header file self-contained      
02/07/10   sm      Added data indicaiton optimization in L2 path
01/31/10   ax      updated watermark HI, LO and DNE due to reduced pool size      
08/25/09   ax      updated watermark HI, LO and DNE per flow control design      
08/24/09   bq      Move DL DNE and set to MAX
02/10/09   bq      Initial version
===========================================================================*/

#ifndef LTE_PDCP_DL_IF_H
#define LTE_PDCP_DL_IF_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <comdef.h>
#include <lte_l2_common.h>
#include <pthread.h>
#include "lte_pdcp_ext_api.h"

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! @brief HI for DL PDCP->A2 watermark
     reasoning: 100ms*100Mbpsetx 
*/
#define LTE_PDCPDL_A2_WM_HI    750000 

/*! @brief LO for DL PDCP->A2 watermark
    reasoning:  HI-20ms*100Mbps
*/
#define LTE_PDCPDL_A2_WM_LO    600000 

/*! @brief DNE for DL PDCP->A2 watermark
    reasoning:  1.5*HI, flow control drop packet at MAC
*/
#define LTE_PDCPDL_A2_WM_DNE    900000

/*! @brief DNE for DL PDCP->A2 watermark
    reasoning: Run out of DSM items after that
*/
/*LTE_PDCPDL_A2_WM_DNE_COUNT  1500 for LTE_PDCPDL_A2_WM_DNE    900000*/
#define LTE_PDCPDL_A2_WM_DNE_COUNT    1500 

#define LTE_PDCPUL_RLC_WM_DNE_COUNT   0xFFFFFFFF
/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

/*===========================================================================

  FUNCTION:  lte_pdcpdl_task_init

===========================================================================*/
/*!
  @brief
  This function create PDCP DL Task with the given priority.

  @detail
  
  @return
  the PDCPDL thread of type pthread_t.
*/
/*=========================================================================*/
extern pthread_t lte_pdcpdl_task_init
(
  int priority  /*!< thread priority passed in by app mgr */
);


/*===========================================================================

  FUNCTION:  lte_pdcpdl_get_rlc_data_ind_info

===========================================================================*/
/*!
  @brief
  Get rlc data ind info pointer
*/
/*=========================================================================*/
lte_l2_indication_info_s *lte_pdcpdl_get_rlc_data_ind_info(void);

#endif /* LTE_PDCP_DL_IF_H */
