#ifndef ECALL_ROM_H_
#define ECALL_ROM_H_
/*===========================================================================

                           E C A L L _ R O M . H

DESCRIPTION
        Rom Data Arrays

REFERENCES
  C Reference code 3GPP TS  26.268

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


  Copyright(c) 2009 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/
/*===========================================================================
                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/ecall/emodemlib/inc/ecall_rom.h#1 $ 
  $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $


when        who     what, where, why
--------    ---     ----------------------------------------------------------
09/30/09    sar     Ported to AMSS by making the necessary changes
09/01/09    sar     initial version  8.1.0  eCall ANSI C fixed-point reference source code

===========================================================================*/
#if (defined(FEATURE_ECALL_IVS) || defined(FEATURE_ECALL_PSAP))

#include "ecall_defines.h"

/*============================================================================*/
/* Synchronization                                                            */
/*----------------------------------------------------------------------------*/

extern const int16 ecall_wakeupSin500[16];
extern const int16 ecall_wakeupCos500[16];
extern const int16 ecall_wakeupSin800[10];
extern const int16 ecall_wakeupCos800[10];

extern const int32 ecall_syncPulseSequenceForms[ECALL_SYNC_IDXLEN];
extern const int16 ecall_syncPulseForm[5];
extern const int16 ecall_syncSequence[15];
extern const int16 ecall_syncIndexPreamble[ECALL_SYNC_IDXLEN];
extern const int16 ecall_syncFrame[10*ECALL_PCM_LENGTH];

/*============================================================================*/
/* Uplink/Downlink format                                                     */
/*----------------------------------------------------------------------------*/

extern const int16 ecall_indexBits[24];

extern const int16 ecall_m4smp_ulPulse[16];
extern const int16 ecall_m8smp_ulPulse[32];
extern const int16 ecall_m4smp_ulPulseMatch[64];
extern const int16 ecall_m8smp_ulPulseMatch[128];

extern const int16 ecall_m4smp_mgTable[66];
extern const int16 ecall_m8smp_mgTable[116];

extern const int16 ecall_dlPcmData[ECALL_NUM_STORED_DLMSG][ECALL_NRF_DLDATA*ECALL_PCM_LENGTH];
extern const int16 ecall_dlPcmDataMatch[ECALL_NUM_STORED_DLMSG][ECALL_NRF_DLDATA*ECALL_PCM_LENGTH];

/*============================================================================*/
/* FEC encoder/decoder                                                        */
/*----------------------------------------------------------------------------*/

extern const uint16 ecall_stateTrans[16];
extern const uint16 ecall_revStateTrans[16];
extern const uint1  ecall_outputParity[16];

extern const uint1  ecall_crcPolynomial[ECALL_NRB_CRC+1];
extern const uint32  ecall_crcPolyWordRev[ECALL_CRC_MAXPOLY];

extern const uint1  ecall_scramblingSeq[ECALL_NRB_INFO_CRC];
extern const uint32 ecall_scrSeqWordRev[ECALL_NRW_INFO_CRC];

extern const uint16 ecall_interleaverSeq[ECALL_NRB_INFO_CRC];
extern const uint16 ecall_redVerIndex[8][ECALL_NRB_CODE_ARQ];

#if ECALL_LLR_TYPE == 0
extern const ecall_IntLLR ecall_logExpTable[ECALL_LOGEXP_RES];
#endif 

#if defined (FEATURE_ECALL_21KHZ_TONE) && defined (FEATURE_ECALL_PSAP)
extern const int16 ecall_tone2100[ECALL_PCM_LENGTH/2];
#endif

#else

void ecall_rom_dummy_func(void);

#endif  /* End FEATURE_ECALL_IVS or FEATURE_ECALL_PSAP */

#endif

