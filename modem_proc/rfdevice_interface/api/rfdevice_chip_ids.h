#ifndef RFDEVICE_CHIP_IDS_H
#define RFDEVICE_CHIP_IDS_H

/*!
  @file
  rfdevice_chip_ids.h
 
  @brief
  Defines the enumeration of chips that can be supported by any version of
  the RF driver.
*/

/*==============================================================================

  Copyright (c) 2012 - 2015 Qualcomm Technologies Incorporated. All Rights Reserved

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

==============================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_interface/api/rfdevice_chip_ids.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
09/16/16   dh      Added QPA4361, QPA4361 GSM
08/16/16   dh      Added QPA4360, QPA4360 GSM
02/18/16   dbc     Added QPA4340_ET 
01/04/16   dbc     Added QPA4351 
10/28/15   dbc     Added QFE4373FC_EPT 
10/16/15   mmd     Added QFE4373FC, QDE2085FC, QFE4373_GSM 
09/15/15   smi     Added WTR2965 enums
04/07/15   tks     Added WTR3950 & WXE device support  
03/24/15   bm      Added WTR2955V110 enums
01/05/15   ndb     Added Third party Coupler 
11/25/14   vv      Added GEN_LNA for third party LNA devices
10/14/14   sd      Added WTR4905V100 enums
10/10/14   kg      Added QFE3100 
09/03/14   ry      Added WTR2955 enums.
05/05/14   vv      Added QFE1045 & GEN_DEVICE enums
04/02/14   Saul    Added WTR4905 enum.
12/19/13   ndb     Added QFE2520 & QFE2550 to the list
09/23/13   shb     Added WTR3925_WXE
09/17/13   sd      Added QFE2310
09/06/13   aca     BOLT QFE update
08/29/13   pl      Remove GEN_ASD device for ASDIV, not needed 
08/27/13   aca     BOLT QFE support
08/08/13   pl      Added a GEN_ASD device for AsDiv
06/07/13   vs      Updated changes for third party tuner
04/26/13   tks     Added WTR3925 to the list 
04/03/13   sd      Added QFE1101
03/12/13   vrb     Adding support for new RF devices: QFE1550 and QFE2340
02/13/13   sn      Added QFE2720
12/27/12   sn      Added QFE2320
12/13/12   shb     Added QFE1520 for WTR1625 RFC compilation
12/05/12   vrb     Added TUNER_HDET device ID
11/26/12   sr      changed the last entry of the enum to 0xFFFFFFFF to force the 
                   compiler to treat the enum as int32 value to support rfc efs data.
11/06/12   bmg     Created

===========================================================================*/

/*----------------------------------------------------------------------------*/
/*!
  @brief
  RF device type
*/
typedef enum
{
  DEFAULT_RFDEVICE,
  RTR8600,
  RTR9205,
  WTR1605,
  WTR2605,
  WFR2600,
  WTR1625,
  WFR1620,
  WGR7640,
  GEN_PA,
  PWR_TRACKER,
  QFE1510,
  QFE1550,
  QFE2340,
  QFE1100,
  QFE3100,
  GEN_ASM,
  TRX_HDET,
  TRX_THERM,
  QFE1320,
  QFE2310,
  QFE2320,
  QFE2720,
  QFE1520,
  TUNER_HDET,
  QFE1101,
  WTR3925,
  /* BOLT Low/Mid band PA*/
  QFE3320,
  /* BOLT EPT PA */
  QFE3320_EPT,
  /* BOLT TDD PA */
  QFE3320_TDD,
  /* BOLT GSM PA */
  QFE3320_GSM,
  /* BOLT Low Band ASM */
  QFE3320_LB,
  /* BOLT Mid band ASM */
  QFE3320_MB,
  /* BOLT TX ASM */
  QFE3320_TX,
  /*Bolt Low band PA*/
  QFE3335FC,
  /*Bolt Low band ASM*/
  QFE3335FC0,
  /*Bolt Low band ASM*/
  QFE3335FC1,
  /* BOLT Low band PA */
  QFE3335,
  /* BOLT Low band ET PA */
  QFE3335_ET,  
  /* BOLT Mid band PA */
  QFE3345,
  /* BOLT Mid band ET PA */
  QFE3345_ET,  
  /* BOLT Low band ASM */
  QFE1035,
  /* BOLT Diversity switch matrix */
  QFE1040,
/* BOLT Low band ASM for WTR0*/
  QFE1040_LB_W0,
  /* BOLT Mid band ASM for WTR0*/
  QFE1040_MB_W0,
  /* BOLT High band ASM for WTR0*/
  QFE1040_HB_W0,
  /* BOLT Low band ASM for WTR1*/
  QFE1040_LB_W1,
  /* BOLT Mid band ASM for WTR1*/
  QFE1040_MB_W1,
  /* BOLT High band ASM for WTR1*/
  QFE1040_HB_W1,
  /* BOLT Mid band ASM */
  QFE1045,
  /* BOLT High band ASM */
  QFE1045_MB,
  /* BOLT High band ASM */
  QFE1045_HB,
  /* BOLT ASM */
  QFE1036,
  /* BOLT ASM */
  QFE1055,
  TP_TUNER,
  TP_BUFFER,
  TEST_DEVICE,
  /* WTR3925 WXE device */
  WTR3925_WXE,
  /*Second Generation Qtuner*/
  QFE2520,
  QFE2550,
  QFE2101,
  WTR4905,
  WTR4905_WXE,
  WTR4905V100,
  WTR4905V100_WXE,
  QFE2320EPT,
  WTR2955,
  WTR2955_WXE,
  WTR2955V110,
  WTR2955V110_WXE,
  WTR2965,
  WTR2965_WXE,
  QFE4335,
  QFE4345,
  QFE2340FC,
  QFE2082FC,
  QFE4435FC,
  QFE3340FC,
  QFE4320,
  QFE4320_ET,
  QFE4320_EPT,
  QFE4320_TDD,
  QFE4320_GSM,
  QFE4320_LB,
  QFE4320_MB,
  QFE4320_TX,
  QFE4373FC,
  QFE4373FC_GSM,
  QFE4373FC_EPT,
  QFE2085FC,
  QPA4340,
  QPA4340_ET,
  QPA4340_TDD,
  QPA4351,
  QPA4360,
  QPA4360_GSM,
  QPA4361,
  QPA4361_GSM,
  GEN_DEVICE,  /*Third party devices with multiple logical components*/
  GEN_LNA,     /*Third party LNA devices*/
  GEN_COUPLER,
  GEN_ASM_GRFC_TUNER,
  WTR3950,      
  WTR3950_WXE,  
  RFDEVICE_MAX_NUM,
  RFDEVICE_INVALID = 0xFFFFFFFF
} rfdevice_id_enum_type;

#endif /* RFDEVICE_CHIP_IDS_H */

