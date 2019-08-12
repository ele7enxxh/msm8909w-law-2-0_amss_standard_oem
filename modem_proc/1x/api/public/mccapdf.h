#ifndef MCCAPDF_H
#define MCCAPDF_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                    C D M A   M A I N   C O N T R O L
               C A P A B I L I T Y   D Y N A M I C   F E A T U R E

GENERAL DESCRIPTION
  This file contains the MC Dynamic Feature functions. The Dynamic Feature
  is to control the feature capability via the NV item at run-time, rather
  than the feature flags during compile time. The Dynamic Feature includes
  optional features in Release 0 and A. When performing the capability
  reporting and service negotiation, it should consult the Dynamic Feature
  capability in this module, rather than rely on the feature flags.
  
  The Dynamic Feature item can be programmed using service programming
  tools. A verification routine is provided to ensure the Dynamic Feature
  item is programmed correctly.
  
  Upon power up, MC reads and verifies the Dynamic Feature item. If
  verifications fails, the mobile goes offline/FTM mode, prompting for
  service programming.
  
  Copyright (c) 2003 - 2013 Qualcomm Technologies, Inc. 
  All Rights Reserved Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies, Inc. and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies, Inc.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/14/09   ag      Split the file as part of CMI phase-II
04/08/07   pg      Featurization and general code cleanup for memory reduction
                   effort.
                   Re-featurized FEATURE_IS2000_DYNAMIC_FEATURE.
08/09/06   fh      AKA/AES: renamed MCCAP_DF_AES_UI to MCCAP_DF_AES_UI_VOICE
08/04/06   fh      AKA/AES: changed AES related DF items
07/13/06   fh      Added AES DF support under AES feature
05/12/05   an      Changed function attributes to PACKED
03/25/05   an      Fixed lint errors.

===========================================================================*/

typedef enum
{
  MCCAP_DF_P_REV = 0,
  MCCAP_DF_QPCH, 
  MCCAP_DF_SLOTTED_TIMER,
  MCCAP_DF_CHS,
  MCCAP_DF_SDB,
  MCCAP_DF_CS,
  MCCAP_DF_OTD,
  MCCAP_DF_STS,
  MCCAP_DF_DCCH,
  MCCAP_DF_FOR_SCH,
  MCCAP_DF_FOR_MAX_RATE,
  MCCAP_DF_REV_SCH,
  MCCAP_DF_REV_MAX_RATE,
  MCCAP_DF_MUXPDU5,
  MCCAP_DF_REV_FCH_GATING,
  MCCAP_DF_SYNC_ID,
  MCCAP_DF_RECONNECT_MSG,
  MCCAP_DF_NEG_SCI,
  MCCAP_DF_AEHO,
  MCCAP_DF_APHO,
  MCCAP_DF_AES_SIG,
  MCCAP_DF_AES_UI_VOICE,
  MCCAP_DF_MAX
} mccap_df_type;

#define MAX_DF_STR_LEN                           15    /* MAX Length of DF String */
#endif /* MCCAPDF_H */
