#ifndef ECALL_DEFINES_H_
#define ECALL_DEFINES_H_
/*==========================================================================

                           Ecall Definations

GENERAL DESCRIPTION
   C Reference code 3GPP TS  26.268 v8.5.0
   
EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2002 by Qualcomm Technologies Incorporated. All Rights Reserved.
============================================================================*/
/*===========================================================================
                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/ecall/emodemlib/inc/ecall_defines.h#1 $ 
$DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $


when        who     what, where, why
--------    ---     ----------------------------------------------------------
09/30/09    sar     ported to AMSS and renamed accordingly
09/01/09    sar     Initial version taken from Pat

============================================================================*/

/*============================================================================*/
/* General conventions                                                        */
/*----------------------------------------------------------------------------*/
typedef enum { Minus = -1, Zero, Plus } Tern;
#define ABS(a)	   (((a) < 0) ? -(a) : (a))  /* macro: absolute value            */
#define SIGN(a)    ((a)< 0 ? (-1) : (1))     /* macro: sign                      */

#define ECALL_TONE_OFFSET  0
//#define FEATURE_ECALL_21KHZ_TONE           /* enable 2.1 khz tone              */

#ifndef ECALL_PCM_LENGTH
#define ECALL_PCM_LENGTH        (160)        /* length of PCM frame in samples   */
#define ECALL_PCM_LENGTH_BYTES  (ECALL_PCM_LENGTH * sizeof(int16))
#endif

#ifndef ECALL_MSD_MAX_LENGTH
#define ECALL_MSD_MAX_LENGTH    (140)        /* length of MSD message in bytes   */
#endif

#ifndef ECALL_PCM_MIN
#define ECALL_PCM_MIN           (-32767-1)      /* minimum PCM value                */
#endif
#ifndef ECALL_PCM_MAX
#define ECALL_PCM_MAX           (32767)      /* maximum PCM value                */
#endif

/*============================================================================*/
/* Synchronization                                                            */
/*----------------------------------------------------------------------------*/

#define ECALL_SYNC_BADCHECK     (3)             /* sync consecutive bad checks      */
#define ECALL_SYNC_BADTRACK     (4)             /* sync consecutive bad track       */
#define ECALL_SYNC_IDXLEN       (75)            /* sync index length                */
#define ECALL_SYNC_THRESHOLD    (10e6)          /* sync threshold                   */

#define ECALL_LOCK_RESYNC       (2)             /* messages to lock after sync loss */
#define ECALL_LOCK_START_UL     (2)             /* START messages to lock sync (UL) */
#define ECALL_LOCK_START_DL     (3)             /* START messages to lock sync (DL) */
#define ECALL_FAIL_RESTART      (3)             /* START messages to restart        */

#define ECALL_NRF_WAKEUP        (3)            /* number of wakeup frames          */
#define ECALL_NRF_SYNC          (13)           /* number of sync frames            */
#define ECALL_NRF_OBSERVE       (10)           /* number of sync observe frames    */
#define ECALL_NRF_RESYNC        (60)           /* resync frames after sync loss    */

/*ECALL_NRS_CHECK should not be greater than 480*/
#define ECALL_NRS_CHECK         (480)           /* number of samples to check       */
/*ECALL_NRS_CHECK should not be greater than 240*/
#define ECALL_NRS_TRACK         (240)           /* number of samples to track       */
#define ECALL_NRS_CP            (2)             /* number of samples next to peaks  */

#define ECALL_PNSEQ_OSF         (22)                  /* oversampling of PN sequence      */
#define ECALL_PEAK_DIST_PP      (30*ECALL_PNSEQ_OSF)  /* distance outer positive peaks    */
#define ECALL_PEAK_DIST_NN      (54*ECALL_PNSEQ_OSF)  /* distance negative peaks          */
#define ECALL_PEAK_DIST_PN      (12*ECALL_PNSEQ_OSF)  /* distance positive to negative    */

/*============================================================================*/
/* Uplink/Downlink format                                                     */
/*----------------------------------------------------------------------------*/

#define ECALL_ARQ_MAX           (8)                   /* number of redundancy versions    */
#define ECALL_NRB_TAIL          (3)                   /* number of encoder tail bits      */
#define ECALL_NRB_CRC           (28)                  /* order of CRC polynomial          */

#define ECALL_NRB_INFO          (8*ECALL_MSD_MAX_LENGTH)
#define ECALL_NRB_INFO_CRC      (8*ECALL_MSD_MAX_LENGTH + ECALL_NRB_CRC)
#define ECALL_NRB_CODE_ARQ      (1380)
#define ECALL_NRB_CODE_BUFFER   (3*(8*ECALL_MSD_MAX_LENGTH + ECALL_NRB_CRC) + 4*ECALL_NRB_TAIL)

#define ECALL_SET_LLMSG         (16)            /* set size lower-layer messages    */
#define ECALL_SET_HLMSG         (16)            /* set size higher-layer messages   */
#define ECALL_NUM_STORED_DLMSG  (4)             /* number of DL PCM messages stored in rom   */ 

#define ECALL_NRF_DLDATA        (3)             /* downlink frames data             */
#define ECALL_NRF_DLMUTE1LL     (3)             /* 1st muting lower-layer message   */
#define ECALL_NRF_DLMUTE1HL     (1)             /* 1st muting higher-layer message  */
#define ECALL_NRF_DLCHUNK       (ECALL_NRF_SYNC + ECALL_NRF_DLMUTE1HL + ECALL_NRF_DLDATA + ECALL_NRF_DLDATA)

#define ECALL_NUMACK_RECVD      (2)             /* TX completed after receving 2 ACKS        */
#define ECALL_NUMHLACK_RECVD    (2)             /* Session completed after receving 2 HL-ACKS*/

/*============================================================================*/
/* IVS/PSAP processing                                                        */
/*----------------------------------------------------------------------------*/

#define ECALL_NRF_MEMCTRL       (7)             /* buffer size in frames   */ 
#define ECALL_NRS_MEMSYNC       (508 + 38*ECALL_NRS_CP)           /* buffer size in samples           */

#define ECALL_IVS_NUMSEND       (5)             /* number of SEND messages          */
#define ECALL_IVS_THRESHOLD     (40000)         /* threshold feedback messages      */
#define ECALL_IVS_GOSTART       (6)             /* threshold for unreliable START   */
#define ECALL_IVS_TXFAST        (10)            /* fast modulator NACK condition    */
#define ECALL_IVS_TXINC         (87)            /* sample increment at restart      */
#define ECALL_IVS_TXDELAY       (73) 
#define ECALL_IVS_STATE_COUNTER (4)             /* count of START,ACK,NACK. Also if idle maintain a state */  

#define ECALL_PSAP_NUMSTART     (20)            /* number of START messages         */
#define ECALL_PSAP_NUMACK       (5)             /* number of ACK messages           */
#define ECALL_PSAP_NUMHLACK     (5)             /* number of HLACK messages         */
#define ECALL_PSAP_THRESHOLD    (40)            /* threshold for modulator type     */

#define ECALL_FEC_VAR           (30206)         /* variance: 1/4550000 in Q37       */
#define ECALL_FEC_MEAN          (0xB9999A)      /* mean: 5.8 in Q21                 */
#define ECALL_FEC_ITERATIONS    (8)             /* number of decoder iterations     */
#define ECALL_FEC_STATES        (8)             /* number of decoder states         */

#define ECALL_RX_VOLUME_LEVEL   (2)             /* Sets volume of incoming frames   */

#define ECALL_DEC_STEPS         2
#define ECALL_LLR_TYPE 1

#define ECALL_CRC_BPWORD        (32)
#define ECALL_CRC_BPCHUNK       (8)
#define ECALL_CRC_MAXPOLY       (256)

#define ECALL_NRW_INFO          (ECALL_NRB_INFO/32 + ((ECALL_NRB_INFO%32 > 0) ? 1 : 0))
#define ECALL_NRW_INFO_CRC      (ECALL_NRB_INFO_CRC/32 + ((ECALL_NRB_INFO_CRC%32 > 0) ? 1 : 0))

#if ECALL_LLR_TYPE == 0

#define ecall_IntLLR            int16
#define ECALL_LLR_MAX           ((int32)(0x7fff-1))
#define ECALL_LOGEXP_RES        (401)           /* resolution of logexp table       */
#define ECALL_LOGEXP_DELTA      (-6)            /* internal Q-factor                */
#define ECALL_LOGEXP_QIN        (8)             /* input Q-factor of LLR values     */

#elif ECALL_LLR_TYPE == 1

#define ecall_IntLLR            int8
#define ECALL_LOGEXP_RES        (4)
#define ECALL_LOGEXP_DELTA      (-1)
#define ECALL_LOGEXP_QIN        (1)
#define ECALL_LLR_MAX           ((int32)(0x7f-1))

#endif

#define ECALL_STEPS_PER_BCJR    (2)        /*  1 or 2, if 2 forward and backward
                                            *  iteration of each Bcjr execution                                           
                                            *  are performed in different steps, 
                                            *  distributing the decoding over 
                                            *  16 and 32 frame processing steps,  
		                            *  respectively. */						                       
#endif /* ECALL_DEFINES_H */



