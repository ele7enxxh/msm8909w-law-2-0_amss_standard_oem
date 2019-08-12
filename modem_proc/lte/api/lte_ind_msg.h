/*!
  @file
  lte_ind_msg.h

  @brief
  This file contains definitions for all the LTE broastcast indications

*/

/*=============================================================================

  Copyright (c) 2009 Qualcomm Technologies Incorporated. All Rights Reserved

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

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/api/lte_ind_msg.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
08/31/11   isalman Don't use q6 headers when compiling for UMPLT.
08/13/09   bn      Added API for MAC indicates to ML1 when failed sending PHR
08/03/09   mm      Added UMID definition for LTE_RRC_HANDOVER_COMPLETED_IND
07/28/09   bn      Put back the changes for lte common indication
07/09/09   bn      initial version
=============================================================================*/

#ifndef LTE_IND_MSG_H
#define LTE_IND_MSG_H

/*=============================================================================

                   INCLUDE FILES

=============================================================================*/
#include <comdef.h>
#include <msgr.h>
#include <msgr_lte.h>

/*=============================================================================

                   EXTERNAL CONSTANT/MACRO DEFINITIONS

=============================================================================*/

/*-----------------------------------------------------------------------*/
/*     LTE broastcast Indication messages                                */
/*-----------------------------------------------------------------------*/

/*! @brief Broastcast Indications that the LTE MAC module sends. Note that
there are 2 enum defines for MAC Indications. The ones defined here are
truely broastcast indications*/
enum
{
  /*! MAC sends this to RRC, MAC DL, ML1 */
  MSGR_DEFINE_UMID(LTE,MAC,IND,CONTENTION_RESULT,0x00,
                   lte_mac_contention_result_ind_msg_s),

  /*! MAC sends this to ML1 in case MAC can't include the 
  PHR in the UL TB. This could happen when there is not enough grant */
  MSGR_DEFINE_UMID(LTE,MAC,IND,SEND_PHR_FAILED,0x01,
                   lte_mac_send_phr_failed_ind_msg_s),

  /*! RRC sends this when Handover is successful */
  MSGR_DEFINE_UMID(LTE, RRC, IND, HANDOVER_COMPLETED, 0x08,
                   lte_rrc_handover_completed_ind_s),

  /*! RRC sends this when Reconfig message processing is successful */
  MSGR_DEFINE_UMID(LTE, RRC, IND, CONFIG_COMPLETED, 0x09,
                   lte_rrc_config_completed_ind_s),

};

#endif /* LTE_IND_MSG_H */

