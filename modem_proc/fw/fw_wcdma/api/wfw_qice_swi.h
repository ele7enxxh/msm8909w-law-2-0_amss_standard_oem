/*
  FILE wfw_qice_swi.h

  QICE software interface.
*/

/*===========================================================================

  Copyright (c) 2009-2011 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/fw/fw_wcdma/api/wfw_qice_swi.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
03/29/12   RP      added ARD V2 support
02/13/12   RP      ARD logging intf update CR 336555
10/20/11   RP      update interface for ARD feature
10/07/11   RP      support ARD feature
05/10/11   RP      EQ logging interface update
04/28/11   YL      logging interface for QICE and cleanup
02/15/11   JC      Remove obsolete micorKernelParam from QICE FFInv log.
01/18/11   JC      Add txdEn in QICE Debug log.
12/03/10   JC      Add QICE Debug logging interface.
10/18/10   JC      Add QICE FFInv logging interface. 

===========================================================================*/

#ifndef __WFW_QICE_SWI_H__
#define __WFW_QICE_SWI_H__

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "wfw_demod_intf.h"

/*===========================================================================

                        DEFINITIONS AND TYPES

===========================================================================*/

/*! @brief Number of QICE debug config log buffers */
#define WFW_QICE_DEBUG_CONFIG_LOG_BUF_SIZE 2             /* for double buffering */

/*! @brief Number of QICE debug log packet buffers */
#define WFW_QICE_DEBUG_LOG_PACKET_BUF_SIZE (3 * 25)  /* 3 cell per carrier x 25 samples per log packet */

/*! @brief Number of QICE ARD log packet buffers */
#define WFW_QICE_ARD_LOG_PACKET_BUF_SIZE (2*16)  /* 2 elements per subframe * 16 subframes */

/*! @brief Number of QICE ARD V2 log packet buffers */
#define WFW_QICE_ARD_V2_LOG_PACKET_BUF_SIZE (8)  /* 8 subframes */

/*! @brief Cell Config info that goes into QICE Debug Info Log packet (Log Code 0x4189) */
typedef struct {

  /* Byte 0 -1 : */
  UWord16 txdEnBmsk; /* txdEn flag per carrier for per cell: bits [0-2] for car0, [4-6]for car 1 and [8-10] for car 2*/

  UWord8 :8; /*reserved*/

} wfwQiceDebugConfigLogStruct;


/*! @brief QICE Debug Info Log packet structure (Log Code 0x4189) */
typedef struct {

  /* Byte 0 - 1 : General Info */
  UWord16 subframe         : 11; /* subframe */
  UWord16 cellIdx          : 3;  /* cell Index bitmask */
  UWord16 carrierIdx       : 2;  /* carrier index */

  /* Byte 2 - 29 : Rnn per Tx Tx1|Tx0 per iteration (total 6 iter + 1 for last iteration) */
  UWord16 rnnTx[2][7];

  /* Byte 30 - 85 : raw Pilot per Tx, per I|Q, per iternation (total 5 iter), 16bits each */
  UWord16  rawPilotPerTx[2][2][7];

  /* Byte 86 - 89 : Raw cpich SNR  per Tx for first iteration */
  UWord16  rawCpichSNR[2];

  /* Byte 90 - 101 : CPICH SNR  per Tx(100,2ab,3ab) where a,b are number of iterations on non serving cells */
  UWord16  cpichSNR[2][3];

  /* Byte 102 - 113 : CPICH SNR  per Tx(100,2ab,3ab) where a,b are number of iterations on non serving cells */
  UWord16  decCpichSNR[2][3];

  /* Byte 114 : number of QICE iterations per cell, 2MSB reserved, cell2, cell1, cell0 each 2bits */
  UWord8 iterations;

  /* Byte 115 : IRPM mode that QICE is in (0 - Tm ; 1- Td ; 2 - Ts) */
  UWord8 irpmMode;

  /* Byte 116 - 119 : Adding 4 reserved bytes to preserve alignment */
  UWord32 reserved;

  /* Byte 120 - 123 : Delta Value */
  UWord32 delta;

} WfwQiceDebugLogStruct;


/*! @brief QICE Debug async read structure */
typedef struct
{
  WfwQiceDebugLogStruct       qiceDebugLog[WFW_DEMOD_NUM_MAX_CARRIERS][WFW_QICE_DEBUG_LOG_PACKET_BUF_SIZE];
  wfwQiceDebugConfigLogStruct qiceDebugCfgLog[WFW_QICE_DEBUG_CONFIG_LOG_BUF_SIZE];
  UWord8 wrPtr;
  UWord8 configWrPtr;
  UWord8 numActiveCar;
} WfwQiceDebugAsyncReadStruct;

/*! @brief QICE Debug async write structure */
typedef struct
{
  UWord8 logEnable;
  UWord8 rdPtr;
} WfwQiceDebugAsyncWriteStruct;


/*! @brief QICE ARD Info Log packet structure */
typedef struct {

  /* Byte 0 - 1 : for carrier 0 */
  Word16 snrC0         : 15; /* averated cpich snr over 6 bpgs.  */
  UWord16 isRxd         : 1;  /* to indicate if is RxD subframe */

  /* Byte 2 - 3 : for carrier 1 */
  Word16 snrC1         : 15; /* snr */
  UWord16               : 1;  /* reserved  */
} WfwQiceArdLogStruct;

/*! @brief ARD async read structure: wrPtr and ardProcDone should not be swapped */
typedef struct
{
  WfwQiceArdLogStruct       ardLog[WFW_QICE_ARD_LOG_PACKET_BUF_SIZE];
  UWord8                    wrPtr;
  /* global subframe number during the Tm */
  UWord16                   subframe; 
  /*! @brief ARD ardProcDone flag */
  Word16                    ardProcDone;
} WfwQiceArdAsyncReadStruct;

#if 0
/*! @brief QICE ARD Info Log packet structure */
typedef struct {
  UWord16 subframe      ;   /* global subframe number during the Tm */

  /* QICE cpich SNR 16Q5 */
  Word16 snrC0          ; /* averated cpich snr over 1 subframe.  */

  /* mmse taps energy on Rx0*/
  UWord32 w0C0          ;
  /* mmse taps energy on Rx1 */
  UWord32 w1C0          ;

  /* tap/CQI filter ready */
  UWord16  mmseTapFiltReadyC0 :1 ;
  UWord16  mmseTapFiltReadyC1 :1 ;
  UWord16  CpichFiltReadyC0 :1 ;
  UWord16  CpichFiltReadyC1 :1 ;
  UWord16:  12; /*reserved */

  /* QICE cpich SNR C1  16Q5 */
  Word16 snrC1          ; /* averated cpich snr over 1 subframe.  */

  /* mmse taps energy on Rx0 C1*/
  UWord32 w0C1          ;

  /* mmse taps energy on Rx1 C1*/
  UWord32 w1C1          ;

} WfwQiceArdV2LogStruct;
#endif

/*! @brief QICE ARD Info Log packet structure */
typedef struct {
  UWord16 subframe      ;   /* global subframe number during the Tm */

  /* QICE cpich SNR 16Q5 */
  Word16 snr[WFW_DEMOD_NUM_MAX_CARRIERS]; /* averated cpich snr over 1 subframe.  */

  /* mmse taps energy*/
  UWord32 mmseTapWeight[2][WFW_DEMOD_NUM_MAX_CARRIERS]; //WFW_NUM_RX_ANTENNAS=2
 
  /* tap/CQI filter ready */
  UWord16  mmseTapFiltReady : 3 ;  /*one bit per carrier Bit0: c0, Bit1: c1, Bit2: c2 */
  UWord16  cpichFiltReady: 3; /*one bit per carrier, Bit0: c0, Bit1: c1, Bit2: c2 */
  UWord16: 10; /*reserved */

} WfwQiceArdV2LogStruct;


/*! @brief ARD async read structure: wrPtr and ardProcDone should not be swapped */
typedef struct
{
  WfwQiceArdV2LogStruct      ardLog[WFW_QICE_ARD_V2_LOG_PACKET_BUF_SIZE];
  UWord8                      wrPtr;
} WfwQiceArdV2AsyncReadStruct;
/*===========================================================================

                          GLOBAL VARIABLES

===========================================================================*/

/*===========================================================================

                         FUNCTION PROTOTYPES

===========================================================================*/

#endif /* __WFW_QICE_SWI_H__ */
