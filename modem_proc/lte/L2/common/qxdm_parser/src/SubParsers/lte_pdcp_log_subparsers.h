/*!
  @file
  lte_pdcp_log_subparsers.h

  @brief
  This header file contains functions that parse the subpackets of
  LTE PDCP module and generate text output representing the subpacket
  contents.


*/

/*===========================================================================

  Copyright (c) 2008 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/L2/common/qxdm_parser/src/SubParsers/lte_pdcp_log_subparsers.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/02/08   bqiu      Initial version
===========================================================================*/

#ifndef LTE_PDCP_LOG_SUBPARSERS_H
#define LTE_PDCP_LOG_SUBPARSERS_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "Common.h"

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/


/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

/*===========================================================================

  FUNCTION:  lte_pdcp_log_subparser_dl_pdu_gensummarytext

===========================================================================*/
/*!
  @brief
  This function parses a the PDU log subpacket and generates
  the equivalent text output

  @details
  This function parses a the PDU log subpacket and generates
  the equivalent text output
 
  @return
*/
/*=========================================================================*/
extern void lte_pdcp_log_subparser_dl_pdu_gensummarytext
(
  const LogPacketHeaderType   *pInBuf,
  UINT                        nBufSize,
  COutputBuffer               &OutBuf
);

/*===========================================================================

  FUNCTION:  lte_pdcp_log_subparser_set_dl_pdu

===========================================================================*/
/*!
  @brief
  This function set the DL PDU log to TRUE before process PDCP PDU Sub Pkt
  because PDCP UL and DL PDU share the same sub pkt id

  @details
 
  @return
*/
/*=========================================================================*/
extern void lte_pdcp_log_subparser_set_dl_pdu(bool pdcp_dl_pdu_log);

/*===========================================================================

  FUNCTION:  lte_pdcp_log_subparser_ul_pdu_gensummarytext

===========================================================================*/
/*!
  @brief
  This function parses a the PDU log subpacket and generates
  the equivalent text output

  @details
  This function parses a the PDU log subpacket and generates
  the equivalent text output
 
  @return
*/
/*=========================================================================*/
extern void lte_pdcp_log_subparser_ul_pdu_gensummarytext
(
  const LogPacketHeaderType   *pInBuf,
  UINT                        nBufSize,
  COutputBuffer               &OutBuf
);

extern void lte_pdcp_log_subparser_dl_cfg_gensummarytext
(
  const LogPacketHeaderType   *pInBuf,
  UINT                        nBufSize,
  COutputBuffer               &OutBuf
);

extern void lte_pdcp_log_subparser_ul_cfg_gensummarytext
(
  const LogPacketHeaderType   *pInBuf,
  UINT                        nBufSize,
  COutputBuffer               &OutBuf
);

extern void lte_pdcp_log_subparser_dl_stats_gensummarytext
(
  const LogPacketHeaderType   *pInBuf,
  UINT                        nBufSize,
  COutputBuffer               &OutBuf
);

extern void lte_pdcp_log_subparser_ul_stats_gensummarytext
(
  const LogPacketHeaderType   *pInBuf,
  UINT                        nBufSize,
  COutputBuffer               &OutBuf
);

/*===========================================================================

  FUNCTION:  lte_pdcp_log_subparser_pdu_gentext

===========================================================================*/
/*!
  @brief
  This function parses a the PDU log subpacket and generates
  the equivalent text output

  @details
  This function parses a the PDU log subpacket and generates
  the equivalent text output
 
  @return
*/
/*=========================================================================*/
extern void lte_pdcp_log_subparser_pdu_gentext
(
  const BYTE    *pInBuf,
  UINT          nBufSize,
  COutputBuffer &OutBuf
);

/*===========================================================================

  FUNCTION:  lte_pdcp_log_subparser_rb_cfg_gentext

===========================================================================*/
/*!
  @brief
  This function parses a the rb cfg log subpacket and generates
  the equivalent text output

  @details
  This function parses a the rb cfg log subpacket and generates
  the equivalent text output
 
  @return
*/
/*=========================================================================*/
extern void lte_pdcp_log_subparser_rb_cfg_gentext
(
  const BYTE    *pInBuf,
  UINT          nBufSize,
  COutputBuffer &OutBuf
);

extern void lte_pdcp_log_subparser_dl_cfg_gentext
(
  const BYTE    *pInBuf,
  UINT          nBufSize,
  COutputBuffer &OutBuf
);

extern void lte_pdcp_log_subparser_ul_cfg_gentext
(
  const BYTE    *pInBuf,
  UINT          nBufSize,
  COutputBuffer &OutBuf
);

extern void lte_pdcp_log_subparser_dl_stats_gentext
(
  const BYTE    *pInBuf,
  UINT          nBufSize,
  COutputBuffer &OutBuf
);

extern void lte_pdcp_log_subparser_ul_stats_gentext
(
  const BYTE    *pInBuf,
  UINT          nBufSize,
  COutputBuffer &OutBuf
);

#endif /* LTE_PDCP_LOG_SUBPARSERS_H */
