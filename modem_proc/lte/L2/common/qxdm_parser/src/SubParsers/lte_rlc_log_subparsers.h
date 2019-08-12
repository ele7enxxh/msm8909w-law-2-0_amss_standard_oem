/*!
  @file
  lte_rlc_log_subparsers.h

  @brief
  This header file contains functions that parse the subpackets of
  LTE RLC module and generate text output representing the subpacket
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/L2/common/qxdm_parser/src/SubParsers/lte_rlc_log_subparsers.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/27/09   ax      Initial version
===========================================================================*/

#ifndef LTE_RLC_LOG_SUBPARSERS_H
#define LTE_RLC_LOG_SUBPARSERS_H

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
extern void lte_rlc_log_subparser_dl_pdu_gensummarytext
(
  const LogPacketHeaderType   *pInBuf,
  UINT                        nBufSize,
  COutputBuffer               &OutBuf
);

extern void lte_rlc_log_subparser_ul_pdu_gensummarytext
(
  const LogPacketHeaderType   *pInBuf,
  UINT                        nBufSize,
  COutputBuffer               &OutBuf
);

extern void lte_rlc_log_subparser_dl_cfg_gensummarytext
(
  const LogPacketHeaderType   *pInBuf,
  UINT                        nBufSize,
  COutputBuffer               &OutBuf
);

extern void lte_rlc_log_subparser_ul_cfg_gensummarytext
(
  const LogPacketHeaderType   *pInBuf,
  UINT                        nBufSize,
  COutputBuffer               &OutBuf
);

extern void lte_rlc_log_subparser_dl_stats_gensummarytext
(
  const LogPacketHeaderType   *pInBuf,
  UINT                        nBufSize,
  COutputBuffer               &OutBuf
);

extern void lte_rlc_log_subparser_profile_gensummarytext
(
  const LogPacketHeaderType   *pInBuf,
  UINT                        nBufSize,
  COutputBuffer               &OutBuf
);

extern void lte_rlc_log_subparser_ul_stats_gensummarytext
(
  const LogPacketHeaderType   *pInBuf,
  UINT                        nBufSize,
  COutputBuffer               &OutBuf
);

extern void lte_rlc_log_subparser_dl_cfg_gentext
(
  const BYTE    *pInBuf,
  UINT          nBufSize,
  COutputBuffer &OutBuf
);

extern void lte_rlc_log_subparser_ul_cfg_gentext
(
  const BYTE    *pInBuf,
  UINT          nBufSize,
  COutputBuffer &OutBuf
);

extern void lte_rlc_log_subparser_dl_stat_gentext
(
  const BYTE    *pInBuf,
  UINT          nBufSize,
  COutputBuffer &OutBuf
);

extern void lte_rlc_log_subparser_profile_gentext
(
  const BYTE    *pInBuf,
  UINT          nBufSize,
  COutputBuffer &OutBuf
);

extern void lte_rlc_log_subparser_ul_stat_gentext
(
  const BYTE    *pInBuf,
  UINT          nBufSize,
  COutputBuffer &OutBuf
);

extern void lte_rlc_log_subparser_rb_cfg_gentext
(
  const BYTE    *pInBuf,
  UINT          nBufSize,
  COutputBuffer &OutBuf
);

extern void lte_rlc_log_subparser_dl_pdu_gentext
(
  const BYTE    *pInBuf,
  UINT          nBufSize,
  COutputBuffer &OutBuf
);

extern void lte_rlc_log_subparser_ul_pdu_gentext
(
  const BYTE    *pInBuf,
  UINT          nBufSize,
  COutputBuffer &OutBuf
);
#endif /* LTE_RLC_LOG_SUBPARSERS_H */
