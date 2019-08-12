/*!
  @file msgr_lte.h

  @brief
   Describes the modules using the message router

  Details...
  
*/

/*===========================================================================

  Copyright (c) 2015 QUALCOMM Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
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

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/api/msgr_lte.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
09/08/15   ar      CR894006: UDC Reset Error Handling
05/13/15   sb      CR 803880: modified PDCP interface for UDC feature
04/17/12   tmai    Added support for T2L
08/31/11   isalman Don't use q6 headers when compiling for UMPLT.
05/25/11   tjc     Added LTE POS module  
05/18/10   pa      Added LTE AFC module
04/28/10   ax      added flow control msgr module 
04/21/10   jyw     added msgr module ML1_HRPDMEAS
03/26/10   tjc     Added support for GPS timetag 
02/02/10   ejv     Remove MSGR_MODULE_x definitions.
02/03/10   sureshs Removed MSGR_NAS_EMM
02/03/10   sureshs Added MSGR_NAS_EMM
01/08/10   sah     Added the irat->lte messages
12/28/09   svk     Added support for lte->wcdma
12/23/09   jyw     Added support for the gap manager
11/08/09   pa      Added support for sleep manager
11/03/09   fhu     Added ML1 BPLMN support
08/09/09   vk      Moving DS to its own MSGR technology space
03/26/09   pa      Adding ML1 TEST with PLT autogen issues fixed
03/26/09   pa      Revert changes till build issues are sorted out
03/25/09   pa      Added ML1 TEST module
03/13/09   anm     Added ML1 IMP module
01/09/08   hj      Added Firmware Driver support
12/10/08   jyw     Added ML1 scheduler support
09/18/08   gsc     Added Grant Manager support
08/07/08   ahe     Added QAL and DDEHW.
03/24/08   sah     Initial Checkin
===========================================================================*/

#ifndef __MSGR_LTE_H__
#define __MSGR_LTE_H__

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/


#include <msgr_umid.h>  /* UMID definitions */

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

#define MSGR_LTE_ML1           MSGR_TECH_MODULE( MSGR_TECH_LTE, 0x01 )
#define MSGR_LTE_ML1_SM        MSGR_TECH_MODULE( MSGR_TECH_LTE, 0x02 )
#define MSGR_LTE_ML1_DLM       MSGR_TECH_MODULE( MSGR_TECH_LTE, 0x03 )
#define MSGR_LTE_ML1_ULM       MSGR_TECH_MODULE( MSGR_TECH_LTE, 0x04 )
#define MSGR_LTE_ML1_MGR       MSGR_TECH_MODULE( MSGR_TECH_LTE, 0x05 )
#define MSGR_LTE_PLT           MSGR_TECH_MODULE( MSGR_TECH_LTE, 0x06 )

#define MSGR_LTE_LL1_SRCH      MSGR_TECH_MODULE( MSGR_TECH_LTE, 0x07 )
#define MSGR_LTE_LL1_DL        MSGR_TECH_MODULE( MSGR_TECH_LTE, 0x08 )
#define MSGR_LTE_LL1_UL        MSGR_TECH_MODULE( MSGR_TECH_LTE, 0x09 )
#define MSGR_LTE_LL1_SYS       MSGR_TECH_MODULE( MSGR_TECH_LTE, 0x0a )
#define MSGR_LTE_LL1_ASYNC     MSGR_TECH_MODULE( MSGR_TECH_LTE, 0x0b )

#define MSGR_LTE_CPHY          MSGR_TECH_MODULE( MSGR_TECH_LTE, 0x0c )
#define MSGR_LTE_RRC           MSGR_TECH_MODULE( MSGR_TECH_LTE, 0x0d )
#define MSGR_LTE_A2_SIM        MSGR_TECH_MODULE( MSGR_TECH_LTE, 0x0e )
#define MSGR_LTE_MAC           MSGR_TECH_MODULE( MSGR_TECH_LTE, 0x0f )
#define MSGR_LTE_MAC_EXT       MSGR_TECH_MODULE( MSGR_TECH_LTE, 0x10 )  // TEMPORARY: Used until messages from A2, ML1 modules are defined
#define MSGR_LTE_RLCDL         MSGR_TECH_MODULE( MSGR_TECH_LTE, 0x11 )
#define MSGR_LTE_RLCUL         MSGR_TECH_MODULE( MSGR_TECH_LTE, 0x12 )
#define MSGR_LTE_PDCPDL        MSGR_TECH_MODULE( MSGR_TECH_LTE, 0x13 )
#define MSGR_LTE_PDCPUL        MSGR_TECH_MODULE( MSGR_TECH_LTE, 0x14 )
#define MSGR_LTE_MSIM_QAL      MSGR_TECH_MODULE( MSGR_TECH_LTE, 0x15 ) // Used for VST simulation.
#define MSGR_LTE_DDEHW         MSGR_TECH_MODULE( MSGR_TECH_LTE, 0x16 ) // Used for MSIM simulation.
#define MSGR_LTE_A2_DL_PHY     MSGR_TECH_MODULE( MSGR_TECH_LTE, 0x17 )
#define MSGR_LTE_STARTUP       MSGR_TECH_MODULE( MSGR_TECH_LTE, 0x18 )
#define MSGR_LTE_EMM           MSGR_TECH_MODULE( MSGR_TECH_LTE, 0x19 )
#define MSGR_LTE_CPHY_TEST     MSGR_TECH_MODULE( MSGR_TECH_LTE, 0x1a )
#define MSGR_LTE_ESM           MSGR_TECH_MODULE( MSGR_TECH_LTE, 0x1c )
#define MSGR_LTE_CM            MSGR_TECH_MODULE( MSGR_TECH_LTE, 0x1d )

#define MSGR_LTE_APP_DSPSW     MSGR_TECH_MODULE( MSGR_TECH_LTE, 0x1e )
#define MSGR_LTE_APP_DSPFW     MSGR_TECH_MODULE( MSGR_TECH_LTE, 0x1f )

#define MSGR_LTE_ML1_GM        MSGR_TECH_MODULE( MSGR_TECH_LTE, 0x1b )
#define MSGR_LTE_ML1_SCHDLR    MSGR_TECH_MODULE( MSGR_TECH_LTE, 0x20 )
#define MSGR_LTE_ML1_FWD       MSGR_TECH_MODULE( MSGR_TECH_LTE, 0x22 )
#define MSGR_LTE_A2T           MSGR_TECH_MODULE( MSGR_TECH_LTE, 0x23 )
#define MSGR_LTE_ML1_IMP       MSGR_TECH_MODULE( MSGR_TECH_LTE, 0x24 )
#define MSGR_LTE_ML1_TEST      MSGR_TECH_MODULE( MSGR_TECH_LTE, 0x25 )

#define MSGR_LTE_Q6_MSG_GW     MSGR_TECH_MODULE( MSGR_TECH_LTE, 0x26 )
#define MSGR_LTE_TLB           MSGR_TECH_MODULE( MSGR_TECH_LTE, 0x27 )
#define MSGR_LTE_QAL           MSGR_TECH_MODULE( MSGR_TECH_LTE, 0x28 ) // Used for VST simulation.

#define MSGR_LTE_ML1_RFMGR     MSGR_TECH_MODULE( MSGR_TECH_LTE, 0x29 )
#define MSGR_LTE_ML1_BPLMN     MSGR_TECH_MODULE( MSGR_TECH_LTE, 0x2a )
#define MSGR_LTE_ML1_SLEEPMGR  MSGR_TECH_MODULE( MSGR_TECH_LTE, 0x2b )
#define MSGR_LTE_ML1_MD_GSM    MSGR_TECH_MODULE( MSGR_TECH_LTE, 0x2c )
#define MSGR_LTE_ML1_GAPMGR    MSGR_TECH_MODULE( MSGR_TECH_LTE, 0x2d )
#define MSGR_LTE_ML1_MD_WCDMA  MSGR_TECH_MODULE( MSGR_TECH_LTE, 0x2e )
#define MSGR_LTE_CPHY_IRAT_MEAS_W2L  MSGR_TECH_MODULE( MSGR_TECH_LTE, 0x2f)
#define MSGR_LTE_CPHY_IRAT_MEAS_G2L  MSGR_TECH_MODULE( MSGR_TECH_LTE, 0x30)
#define MSGR_LTE_ML1_HRPDMEAS  MSGR_TECH_MODULE( MSGR_TECH_LTE, 0x31 )
#define MSGR_LTE_ML1_POS       MSGR_TECH_MODULE( MSGR_TECH_LTE, 0x32 )
#define MSGR_LTE_FC            MSGR_TECH_MODULE( MSGR_TECH_LTE, 0x33 )
#define MSGR_LTE_ML1_AFC       MSGR_TECH_MODULE( MSGR_TECH_LTE, 0x34 )
#define MSGR_LTE_CPHY_IRAT_MEAS_D2L  MSGR_TECH_MODULE( MSGR_TECH_LTE, 0x35)
#define MSGR_LTE_CPHY_IRAT_MEAS_C2L  MSGR_TECH_MODULE( MSGR_TECH_LTE, 0x36)
#define MSGR_LTE_CPHY_IRAT_MEAS_F2L  MSGR_TECH_MODULE( MSGR_TECH_LTE, 0x37)
#define MSGR_LTE_ML1_COEX      MSGR_TECH_MODULE( MSGR_TECH_LTE, 0x38)
#define MSGR_LTE_ML1_CXM       MSGR_TECH_MODULE( MSGR_TECH_LTE, 0x39)
#define MSGR_LTE_ML1_COMMON    MSGR_TECH_MODULE( MSGR_TECH_LTE, 0x3a)
#define MSGR_LTE_PDCPOFFLOAD   MSGR_TECH_MODULE( MSGR_TECH_LTE, 0x3b)
#define MSGR_LTE_CPHY_IRAT_MEAS_T2L  MSGR_TECH_MODULE( MSGR_TECH_LTE, 0x3c)
#define MSGR_LTE_ML1_MCLK      MSGR_TECH_MODULE( MSGR_TECH_LTE, 0x3d)
#define MSGR_LTE_ML1_OFFLOAD   MSGR_TECH_MODULE( MSGR_TECH_LTE, 0x3e)
#define MSGR_LTE_ML1_COEX_DSDA MSGR_TECH_MODULE( MSGR_TECH_LTE, 0x3f)
#define MSGR_LTE_ML1_NLIC      MSGR_TECH_MODULE( MSGR_TECH_LTE, 0x40)
#define MSGR_LTE_ML1_MSMGR     MSGR_TECH_MODULE( MSGR_TECH_LTE, 0x41 )
#define MSGR_LTE_PDCPCOMP      MSGR_TECH_MODULE( MSGR_TECH_LTE, 0x42)
#define MSGR_LTE_L2            MSGR_TECH_MODULE( MSGR_TECH_LTE, 0x44)
#define MSGR_LTE_ML1_SM_FSCAN  MSGR_TECH_MODULE( MSGR_TECH_LTE, 0x45)
#endif /* __MSGR_LTE_H__ */
