/*!
  @file
  lte_mac_log_subparsers.h

  @brief
  This header file contains functions that parse the subpackets of
  LTE MAC module and generate text output representing the subpacket
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/L2/common/qxdm_parser/src/SubParsers/lte_mac_log_subparsers.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/02/08   yg      Initial version
===========================================================================*/

#ifndef LTE_MAC_LOG_SUBPARSERS_H
#define LTE_MAC_LOG_SUBPARSERS_H

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

  FUNCTION:  lte_mac_log_subparser_cfg_gensummarytext

===========================================================================*/
/*!
  @brief
  This function parses a the MAC Config log subpacket and generates
  the equivalent summary text output

  @details
  This function parses a the MAC Config log subpacket and generates
  the equivalent summary text output
 
  @return
*/
/*=========================================================================*/
void lte_mac_log_subparser_cfg_gensummarytext
(
  const LogPacketHeaderType   *pInBuf,
  UINT                        nBufSize,
  COutputBuffer               &OutBuf
);

/*===========================================================================

  FUNCTION:  lte_mac_log_subparser_cfg_type_gentext

===========================================================================*/
/*!
  @brief
  This function parses a the Config Type log subpacket and generates
  the equivalent text output

  @details
  This function parses a the Config Type log subpacket and generates
  the equivalent text output
 
  @return
*/
/*=========================================================================*/
void lte_mac_log_subparser_cfg_type_gentext
(
  const BYTE    *pInBuf,
  UINT          nBufSize,
  COutputBuffer &OutBuf
);

/*===========================================================================

  FUNCTION:  lte_mac_log_subparser_dl_cfg_gentext

===========================================================================*/
/*!
  @brief
  This function parses a the DL Config log subpacket and generates
  the equivalent text output

  @details
  This function parses a the DL Config log subpacket and generates
  the equivalent text output
 
  @return
*/
/*=========================================================================*/
void lte_mac_log_subparser_dl_cfg_gentext
(
  const BYTE    *pInBuf,
  UINT          nBufSize,
  COutputBuffer &OutBuf
);

/*===========================================================================

  FUNCTION:  lte_mac_log_subparser_ul_cfg_gentext

===========================================================================*/
/*!
  @brief
  This function parses a the UL Config log subpacket and generates
  the equivalent text output

  @details
  This function parses a the UL Config log subpacket and generates
  the equivalent text output
 
  @return
*/
/*=========================================================================*/
void lte_mac_log_subparser_ul_cfg_gentext
(
  const BYTE    *pInBuf,
  UINT          nBufSize,
  COutputBuffer &OutBuf
);

/*===========================================================================

  FUNCTION:  lte_mac_log_subparser_lc_cfg_gentext

===========================================================================*/
/*!
  @brief
  This function parses a the LC Config log subpacket and generates
  the equivalent text output

  @details
  This function parses a the LC Config log subpacket and generates
  the equivalent text output
 
  @return
*/
/*=========================================================================*/
void lte_mac_log_subparser_lc_cfg_gentext
(
  const BYTE    *pInBuf,
  UINT          nBufSize,
  COutputBuffer &OutBuf
);

/*===========================================================================

  FUNCTION:  lte_mac_log_subparser_rach_trigger_gensummarytext

===========================================================================*/
/*!
  @brief
  This function parses a the RACH trigger log subpacket and generates
  the equivalent summary text output

  @details
  This function parses a the RACH trigger log subpacket and generates
  the equivalent summary text output
 
  @return
*/
/*=========================================================================*/
void lte_mac_log_subparser_rach_trigger_gensummarytext
(
  const LogPacketHeaderType   *pInBuf,
  UINT                        nBufSize,
  COutputBuffer               &OutBuf
);

/*===========================================================================

  FUNCTION:  lte_mac_log_subparser_rach_cfg_gentext

===========================================================================*/
/*!
  @brief
  This function parses a the RACH Config log subpacket and generates
  the equivalent text output

  @details
  This function parses a the RACH Config log subpacket and generates
  the equivalent text output
 
  @return
*/
/*=========================================================================*/
void lte_mac_log_subparser_rach_cfg_gentext
(
  const BYTE    *pInBuf,
  UINT          nBufSize,
  COutputBuffer &OutBuf
);

/*===========================================================================

  FUNCTION:  lte_mac_log_subparser_rach_reason_gentext

===========================================================================*/
/*!
  @brief
  This function parses a the RACH Reason log subpacket and generates
  the equivalent text output

  @details
  This function parses a the RACH Reason log subpacket and generates
  the equivalent text output
 
  @return
*/
/*=========================================================================*/
void lte_mac_log_subparser_rach_reason_gentext
(
  const BYTE    *pInBuf,
  UINT          nBufSize,
  COutputBuffer &OutBuf
);

/*===========================================================================

  FUNCTION:  lte_mac_log_subparser_rach_attempt_gensummarytext

===========================================================================*/
/*!
  @brief
  This function parses a the RACH attempt log subpacket and generates
  the equivalent summary text output

  @details
  This function parses a the RACH attempt log subpacket and generates
  the equivalent summary text output
 
  @return
*/
/*=========================================================================*/
void lte_mac_log_subparser_rach_attempt_gensummarytext
(
  const LogPacketHeaderType   *pInBuf,
  UINT                        nBufSize,
  COutputBuffer               &OutBuf
);

/*===========================================================================

  FUNCTION:  lte_mac_log_subparser_rach_attempt_gentext

===========================================================================*/
/*!
  @brief
  This function parses a the RACH Attempt log subpacket and generates
  the equivalent text output

  @details
  This function parses a the RACH Attempt log subpacket and generates
  the equivalent text output
 
  @return
*/
/*=========================================================================*/
void lte_mac_log_subparser_rach_attempt_gentext
(
  const BYTE    *pInBuf,
  UINT          nBufSize,
  COutputBuffer &OutBuf
);


/*===========================================================================

  FUNCTION:  lte_mac_log_subparser_dl_tb_gensummarytext

===========================================================================*/
/*!
  @brief
  This function parses a the DL transport block log subpacket and generates
  the equivalent text output

  @details
  This function parses a the DL transport block log subpacket and generates
  the equivalent text output
 
  @return
*/
/*=========================================================================*/
extern void lte_mac_log_subparser_dl_tb_gensummarytext
(
  const LogPacketHeaderType   *pInBuf,
  UINT                        nBufSize,
  COutputBuffer               &OutBuf
);

/*===========================================================================

  FUNCTION:  lte_mac_log_subparser_dl_tb_gentext

===========================================================================*/
/*!
  @brief
  This function parses a the DL transport block log subpacket and generates
  the equivalent text output

  @details
  This function parses a the DL transport block log subpacket and generates
  the equivalent text output
 
  @return
*/
/*=========================================================================*/
extern void lte_mac_log_subparser_dl_tb_gentext
(
  const BYTE    *pInBuf,
  UINT          nBufSize,
  COutputBuffer &OutBuf
);

/*===========================================================================

  FUNCTION:  lte_mac_log_subparser_ul_tb_gensummarytext

===========================================================================*/
/*!
  @brief
  This function parses a the UL transport block log subpacket and generates
  the equivalent text output

  @details
  This function parses a the UL transport block log subpacket and generates
  the equivalent text output
 
  @return
*/
/*=========================================================================*/
extern void lte_mac_log_subparser_ul_tb_gensummarytext
(
  const LogPacketHeaderType   *pInBuf,
  UINT                        nBufSize,
  COutputBuffer               &OutBuf
);

/*===========================================================================

  FUNCTION:  lte_mac_log_subparser_ul_tb_gentext

===========================================================================*/
/*!
  @brief
  This function parses a the UL transport block log subpacket and generates
  the equivalent text output

  @details
  This function parses a the UL transport block log subpacket and generates
  the equivalent text output
 
  @return
*/
/*=========================================================================*/
extern void lte_mac_log_subparser_ul_tb_gentext
(
  const BYTE    *pInBuf,
  UINT          nBufSize,
  COutputBuffer &OutBuf
);

/*===========================================================================

  FUNCTION:  lte_mac_log_subparser_ul_qos_gensummarytext

===========================================================================*/
/*!
  @brief
  This function parses a the UL QOS log subpacket and generates
  the equivalent text output

  @details
  This function parses a the UL QOS log subpacket and generates
  the equivalent text output
 
  @return
*/
/*=========================================================================*/
extern void lte_mac_log_subparser_ul_qos_gensummarytext
(
  const LogPacketHeaderType   *pInBuf,
  UINT                        nBufSize,
  COutputBuffer               &OutBuf
);

/*===========================================================================

  FUNCTION:  lte_mac_log_subparser_ul_qos_gentext

===========================================================================*/
/*!
  @brief
  This function parses a the UL QOS log subpacket and generates
  the equivalent text output

  @details
  This function parses a the UL QOS log subpacket and generates
  the equivalent text output
 
  @return
*/
/*=========================================================================*/
extern void lte_mac_log_subparser_ul_qos_gentext
(
  const BYTE    *pInBuf,
  UINT          nBufSize,
  COutputBuffer &OutBuf
);

/*===========================================================================

  FUNCTION:  lte_mac_log_subparser_ul_buf_status_gensummarytext

===========================================================================*/
/*!
  @brief
  This function parses a the UL Buffer Status log subpacket and generates
  the equivalent text output

  @details
  This function parses a the UL Buffer Status log subpacket and generates
  the equivalent text output
 
  @return
*/
/*=========================================================================*/
extern void lte_mac_log_subparser_ul_buf_status_gensummarytext
(
  const LogPacketHeaderType   *pInBuf,
  UINT                        nBufSize,
  COutputBuffer               &OutBuf
);

/*===========================================================================

  FUNCTION:  lte_mac_log_subparser_ul_buf_status_gentext

===========================================================================*/
/*!
  @brief
  This function parses a the UL Buffer Status log subpacket and generates
  the equivalent text output

  @details
  This function parses a the UL Buffer Status log subpacket and generates
  the equivalent text output
 
  @return
*/
/*=========================================================================*/
extern void lte_mac_log_subparser_ul_buf_status_gentext
(
  const BYTE    *pInBuf,
  UINT          nBufSize,
  COutputBuffer &OutBuf
);

/*===========================================================================

  FUNCTION:  lte_mac_log_subparser_ul_tx_stats_gensummarytext

===========================================================================*/
/*!
  @brief
  This function parses a the UL Tx Stats log subpacket and generates
  the equivalent summary text output

  @details
  This function parses a the UL Tx Stats log subpacket and generates
  the equivalent summary text output
 
  @return
*/
/*=========================================================================*/
void lte_mac_log_subparser_ul_tx_stats_gensummarytext
(
  const LogPacketHeaderType   *pInBuf,
  UINT                        nBufSize,
  COutputBuffer               &OutBuf
);

/*===========================================================================

  FUNCTION:  lte_mac_log_subparser_ul_tx_stats_gentext

===========================================================================*/
/*!
  @brief
  This function parses a the UL Tx Stats log subpacket and generates
  the equivalent text output

  @details
  This function parses a the UL Tx Stats log subpacket and generates
  the equivalent text output
 
  @return
*/
/*=========================================================================*/
void lte_mac_log_subparser_ul_tx_stats_gentext
(
  const BYTE    *pInBuf,
  UINT          nBufSize,
  COutputBuffer &OutBuf
);
#endif /* LTE_MAC_LOG_SUBPARSERS_H */
