/*===========================================================================

                           E C A L L _ M O D E M . C

DESCRIPTION
         modulator and demodulator functions

REFERENCES
  C Reference code 3GPP TS  26.268

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2009-2011 Qualcomm Technologies Incorporated.
All rights reserved. 
Qualcomm Confidential and Proprietary 
===========================================================================*/
/*===========================================================================
                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/ecall/emodemlib/src/ecall_modem.c#1 $ 
  $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $


when        who     what, where, why
--------    ---     ---------------------------------------------------------- 
12/06/12    DL      Fixed Klocwork error. ecall_redVerIndex might be out of bound 
09/01/09    sar     Ported to AMSS by making the necessary changes
09/01/09    sar     initial version  8.1.0  eCall ANSI C fixed-point reference source code

===========================================================================*/

#include "customer.h"
#include "comdef.h"
#include "msg.h"
#include "err.h"
#include <stringl/stringl.h>

#ifdef FEATURE_ECALL_IVS
#include "ecall_modem_ivs.h"
#include "ecall_efs.h"
#endif
#ifdef FEATURE_ECALL_PSAP
#include "ecall_modem_psap.h"
#endif


#if (defined(FEATURE_ECALL_IVS) || defined(FEATURE_ECALL_PSAP))

#include "ecall_modem.h"
#include "ecall_defines.h"
#include "ecall_fec.h"
#include "ecall_rom.h"

/*==========================================================================

  FUNCTION    ECALL_SET_MOD_STATE

  DESCRIPTION
    Processes commands off of the command queue, and returns the command
    buffers to the free queue.

  DEPENDENCIES
    None.

  RETURN VALUE
    In:     int16     ecall_mod_type   -> type of modulator to use
    InOut:  ecall_mod_state* ms       <-> modulator struct 

  SIDE EFFECTS
    None

===========================================================================*/
void ecall_set_mod_state
(
  ecall_mod_state *ms, ecall_mod_type modType
)
{
  switch (modType) {
    case ModUndef:
      ms->type         = ModUndef;
      ms->bpsym        = 0;
      ms->spmf         = 0;
      ms->mfpf         = 0;
      ms->decpos1      = 0;
      ms->decpos2      = 0;
      ms->wutperiod    = 0;
      ms->nfmute1      = 0;
      ms->nfmute4      = 0;
      ms->nfmuteall    = 0;
      ms->nfdata       = 0;
      ms->ulPulse      = NULL;
      ms->ulPulseMatch = NULL;
      ms->mgTable      = NULL;
      ms->wakeupSin    = NULL;
      ms->wakeupCos    = NULL;
      break;

    case Mod3bit4smp:
      ms->type         = Mod3bit4smp;
      ms->bpsym        = 3;
      ms->spmf         = 16;
      ms->mfpf         = ECALL_PCM_LENGTH/(ms->spmf);
      ms->decpos1      = 15;
      ms->decpos2      = 30;
      ms->wutperiod    = 16;
      ms->nfmute1      = 1;
      ms->nfmute4      = 7;
      ms->nfmuteall    = 20;
      ms->nfdata       = ECALL_NRB_CODE_ARQ/((ms->mfpf)*(ms->bpsym));
      ms->ulPulse      = ecall_m4smp_ulPulse;
      ms->ulPulseMatch = ecall_m4smp_ulPulseMatch;
      ms->mgTable      = ecall_m4smp_mgTable;
      ms->wakeupSin    = ecall_wakeupSin500;
      ms->wakeupCos    = ecall_wakeupCos500;
      break;

    case Mod3bit8smp:
      ms->type         = Mod3bit8smp;
      ms->bpsym        = 3;
      ms->spmf         = 32;
      ms->mfpf         = ECALL_PCM_LENGTH/(ms->spmf);
      ms->decpos1      = 30;
      ms->decpos2      = 60;
      ms->wutperiod    = 10;
      ms->nfmute1      = 1;
      ms->nfmute4      = 7;
      ms->nfmuteall    = 24;
      ms->nfdata       = ECALL_NRB_CODE_ARQ/((ms->mfpf)*(ms->bpsym));
      ms->ulPulse      = ecall_m8smp_ulPulse;
      ms->ulPulseMatch = ecall_m8smp_ulPulseMatch;
      ms->mgTable      = ecall_m8smp_mgTable;
      ms->wakeupSin    = ecall_wakeupSin800;
      ms->wakeupCos    = ecall_wakeupCos800;
      break;
  }
}

/*==========================================================================
  FUNCTION    ECALL_MODEM_ABORT

  DESCRIPTION
    Wrapper to call the ecall_modem api and post
    a command to APP for Aborting current transmission.

  DEPENDENCIES
    None.

  RETURN VALUE
    None.

  SIDE EFFECTS
    None
===========================================================================*/
void ecall_modem_abort(void )
{
#ifdef FEATURE_ECALL_IVS
   ecall_modem_ivs_err();
#elif defined(FEATURE_ECALL_PSAP)
   ecall_modem_psap_err();
#endif 
}

#endif

#ifdef FEATURE_ECALL_IVS 
/*==========================================================================

  FUNCTION    ECALL_IVS_TRANSMITTER

  DESCRIPTION
    Processes commands off of the command queue, and returns the command
    buffers to the free queue.

  DEPENDENCIES
    None.

  RETURN VALUE
    In:      const ecall_mod_state* ms      -> modulator struct                       
             const uint1*     buffer  -> code bit buffer                    
             int16           rv      -> redundancy version                  
             int16           index   -> position within uplink frame        

    Out:    int16*          pcm     <- output data    

  SIDE EFFECTS
    None

===========================================================================*/
void ecall_ivs_transmitter
(
  const ecall_mod_state *ms,
  const uint1 *buffer,
  int16 *pcm,
  uint32 pcm_size,
  int16 rv,
  int16 index
)
{
  int16 i, j, mgPos, symIdx;
  int32 k;
  int16 mgTableVal =0;
  int16 redIdx = ((rv%2) ? 0 : 1)*(ECALL_ARQ_MAX/2) + (rv/2)%(ECALL_ARQ_MAX/2);

  /* Boundary check */
  if (redIdx < 0 || redIdx > 7) 
  {
    ECALL_MSG_1( ECALL_ERROR, "redIdx %d is out of bound. Should be 0..7",
                               redIdx );
    return;
  }
  
  /* treat first redundancy version differently */
  if (rv == 0) {
    if (index < ECALL_NRF_WAKEUP) {
      /* output wakeup tone */
      mgPos = -1;
      for (i = 0; i < ECALL_PCM_LENGTH; i++) {
        pcm[i] = ms->wakeupSin[i%(ms->wutperiod)];
      }
    } else if (index < ECALL_NRF_SYNC) {
      /* output sync frame portion */
      mgPos = -1;
      ecall_memscpy ( pcm, pcm_size,
                      &ecall_syncFrame[ECALL_PCM_LENGTH*(index-ECALL_NRF_WAKEUP)],
        sizeof(int16)*ECALL_PCM_LENGTH);
       ECALL_MSG_0( ECALL_HIGH, "IVS tx sync Frame" );
      /* wakeup tone extends into the first frame of the preamble */
      /*lint -e774 -e506*/
      if ((ECALL_NRF_WAKEUP > 0) && (index == ECALL_NRF_WAKEUP)) {
        for (i = 0; i < 32; i++)
        {
           pcm[i] = ms->wakeupSin[i%(ms->wutperiod)];
        }
      }
      /*lint +e774 +e506*/
    } else {
      mgPos = index - ECALL_NRF_SYNC;
    }
  } else {
    /* no sync frames in subsequent redundancy versions */
    mgPos = index;
  }
  if (mgPos >= 0)
  {
    mgTableVal = (ms->mgTable[mgPos])%512-7;

    /* no longer sync, but check for muting gaps */
    if (mgTableVal >= 0)
    {
      /* convert code bits to symbol indices and modulate */
      for (i = 0; i < ms->mfpf; i++)
      {
        k = (mgTableVal*(ms->mfpf)+i)*(ms->bpsym);
        for (j = 0, symIdx = 0; j < ms->bpsym; j++)
        { 
          /* Boundary check */
          if( (k+j < 0) || (k+j > ECALL_NRB_CODE_ARQ-1) ) 
          {
            ECALL_MSG_0( ECALL_ERROR, "k+j is out of bound. Should be 0..1379" );
            break;
          }
          /*lint -e701*/
          symIdx += (int16)(buffer[ecall_redVerIndex[redIdx][k+j]] 
            << (((ms->bpsym)-1)-j));
          /*lint +e701*/
        }
        ecall_symbol_mod(ms, symIdx, &pcm[i*(ms->spmf)]);  
      }
    }
    else if (mgTableVal == -2) {
      memset(pcm, 0, sizeof(int16)*64);
      ecall_memscpy ( &pcm[64], pcm_size-(sizeof(int16)*64),
                      &ecall_syncFrame[64+ECALL_PCM_LENGTH*(4-(mgTableVal))], sizeof(int16)*(ECALL_PCM_LENGTH-64) );
    }
    else if (mgTableVal > -6 && mgTableVal < -2) {
      ecall_memscpy ( pcm, pcm_size,
                      &ecall_syncFrame[ECALL_PCM_LENGTH*(4-(mgTableVal))], sizeof(int16)*ECALL_PCM_LENGTH );
    } 
    else
    {
      memset(pcm, 0, sizeof(int16)*ECALL_PCM_LENGTH);
    }
  }
}


/*==========================================================================

  FUNCTION    ECALL_SYMBOL_MOD

  DESCRIPTION
    IVS receiver function

  DEPENDENCIES
    None.

  RETURN VALUE
   In:     const ecall_mod_state* ms      -> modulator struct                        
           int16           symbol  -> symbol index                            
   Out:    int16*          mPulse  <- modulated output sequence

  SIDE EFFECTS
    None

===========================================================================*/
void ecall_symbol_mod
(
  const ecall_mod_state *ms,
  int16 symbol,
  int16 *mPulse
)
{
  int16 i, shifter;
  /*lint -e701*/
  int16 nsym  = (int16)(1 << (ms->bpsym));    /* number of symbols */
  int16 nsym2 = (int16)(1 << (ms->bpsym-1));  /* half of the number of symbols */
  /*lint +e701*/

  if (symbol < 0 || symbol > nsym-1) {
    ECALL_MSG_1( ECALL_ERROR, "symbol %d index out of range, Abort Tx.",
                               symbol );
    symbol = 0;
    ecall_modem_abort();
  }
  /* initialize memory */
  for (i = 0; i < ms->spmf; i++) {
    mPulse[i] = 0;
  }
  if (symbol < nsym2) {
    /* creates positive symbols */
    shifter = (int16)(symbol*(ms->spmf)/nsym2);
    for (i = 0; i < ms->spmf; i++)
      mPulse[(i+shifter)%(ms->spmf)] = ms->ulPulse[i];
  }
  else {
    /* creates negative symbols */
    shifter = (int16)(((nsym-1)-symbol)*(ms->spmf)/nsym2);
    for (i = 0; i < ms->spmf; i++)
    {
       mPulse[(i+shifter)%(ms->spmf)] = (int16)((-1)*(ms->ulPulse[i]));
    }
  }
}

/*==========================================================================

  FUNCTION    ECALL_BYTE2BIT

  DESCRIPTION
    IVS receiver function

  DEPENDENCIES
    None.

  RETURN VALUE
    In:     uint8* in      -> vector of input bytes                            
            int16 length  -> length of input                                   
    Out:    uint1* out     <- vector of output bits   

  SIDE EFFECTS
    None

===========================================================================*/
void ecall_byte2bit
(
  const uint8 *in,
  uint1 *out,
  int16 length
)
{
  int16 i, j;

  for (i = length-1; i >= 0; i--) {
    for (j = 0; j < 8; j++)
      out[j+i*8] = (uint1)(0x1 & (in[i]>>(7-j)));
  }
}

#endif

#ifdef FEATURE_ECALL_PSAP
/*==========================================================================

  FUNCTION    ECALL_PSAP_RECEIVER

  DESCRIPTION
        PSAP receiver function (decoding is done outside)

  DEPENDENCIES
    None.

  RETURN VALUE
   In:     const ecall_mod_state* ms        -> modulator struct                      
           const int16*    pcm       -> input data for demodulation           
   Out:    ecall_IntLLR*         softBits  <- demodulated soft bit sequence   
      
  SIDE EFFECTS
    None

===========================================================================*/
void ecall_psap_receiver
(
  const ecall_mod_state *ms,
  const int16 *pcm,
  ecall_IntLLR *softBits
)
{
  int16 m;
  for (m = 0; m < ms->mfpf; m++) {
    ecall_symbol_demod(ms, &pcm[m*(ms->spmf)], &softBits[m*(ms->bpsym)]);
  }
}

/*==========================================================================

  FUNCTION    ECALL_SYMBOL_DEMOD

  DESCRIPTION
        symbol demodulator

  DEPENDENCIES
    None.

  RETURN VALUE
   In:     const ecall_mod_state* ms        -> modulator struct                      
           const int16*    mPulse    -> received pulse train                  
   Out:    ecall_IntLLR*   softBits  <- demodulated soft bit sequence         
   
  SIDE EFFECTS
    None

===========================================================================*/
void ecall_symbol_demod
(
  const ecall_mod_state *ms,
  const int16 *mPulse,
  ecall_IntLLR *softBits
)
{
  int16 i, j, k, m;
  /*lint -e734*/
  int16 nsym  = 1 << (ms->bpsym);    /* number of symbols */
  int16 nsym2 = 1 << (ms->bpsym-1);  /* half of the number of symbols */

  int32 Lacc, r;
  int32 lp[3]; 
  uint1 bp[3] = {0, 0, 0};
  int32 lm[3];
  uint1 bm[3] = {0, 0, 0};

  for (i = 0; i < nsym2; i++)
  {
    for (j = 0, r = 0; j < ms->spmf; j++)
    {
      r += ((int32)mPulse[j])*((int32)(ms->ulPulseMatch[i*(ms->spmf)+j]));
    }
    /*lint +e734*/

    /* consider positive and negative copy */
    for (j = 0; j <= 1; j++) {
      m = (j) ? ((nsym-1)-i)*(ms->bpsym) : i*(ms->bpsym); /*lint !e734*/
      r = (j) ? -r : r;

      Lacc = ecall_mpy_lacc(r, ECALL_FEC_VAR) - ECALL_FEC_MEAN;
      Lacc = Lacc >> (21-ECALL_LOGEXP_QIN);   /*lint !e704*/

      if (ABS(Lacc) > ECALL_LLR_MAX) {
        Lacc = SIGN(Lacc)*ECALL_LLR_MAX;
      }
      /* cumulative sum of logexp values */
      for (k = 0; k < ms->bpsym; k++) {
        if (ecall_indexBits[m+k]) {
          if (!bp[k]) {
            lp[k] = Lacc;
            bp[k] = 1;
          } else {
            lp[k] = (int32)ecall_jac_log(lp[k], Lacc);
          }
        } else {
          if (!bm[k]) {
            lm[k] = Lacc;
            bm[k] = 1;
          } else {
            lm[k] = (int32)ecall_jac_log(lm[k], Lacc);
          }
        }
      }
    }
  }
  /* determine final LLR values */
  for (k = 0; k < ms->bpsym; k++) {
    Lacc = lm[k] - lp[k];

    if (ABS(Lacc) > ECALL_LLR_MAX) {
      softBits[k] = (ecall_IntLLR)(SIGN(Lacc)*ECALL_LLR_MAX);
    } else {
      softBits[k] = (ecall_IntLLR)Lacc;
    }
  }
}

/*==========================================================================

  FUNCTION    ECALL_BIT2BYTE

  DESCRIPTION
        conversion bit vector to byte vector

  DEPENDENCIES
    None.

  RETURN VALUE
   In:     const uint1* in      -> vector of input bits                       
   int16   length               -> length of output                            
   Out:    uint8*       out     <- vector of output bytes    

  SIDE EFFECTS
    None

===========================================================================*/
void ecall_word2byte
(
  const uint32 *in,
  uint8 *out,
  int16 length
)
{
  int16 i, j;
  //memset(out, 0, sizeof(uint8)*length);  /*lint !e737*/

  for (i = 0; i < length; i++) {
    for (j = 0; j < 4; j++) {
      out[i*4+j] = (uint8)((in[i] >> (3-j)*8) & 0xFF);
    }
  }
}

/*==========================================================================

  FUNCTION    ECALL_MPY_LACC

  DESCRIPTION
        multiply 32bit number with 16bit number (32bit result)

  DEPENDENCIES
    None.

  RETURN VALUE
    In:     int32 var32  -> 32bit number                                   
            int16 var16  -> 16bit number                                       

    Return: int32        <- result  

  SIDE EFFECTS
    None

===========================================================================*/
int32 ecall_mpy_lacc
(
  int32 var32,
  int16 var16
)
{
  int16 temp;
  int32 result;

  temp = (int16)var32;
  temp = temp >> 1;       /*lint !e702*/
  temp = (int16)0x7fff & (int16)temp;

  /*lint -e704*/
  result = (int32)var16 * (int32)temp;
  result = result >> 15;   
  result = result + (int32)var16*(var32 >> 16);
  /*lint +e704*/

  return result;
}


#else

void ecall_modem_dummy_func(void);

#endif /* End FEATURE_ECALL_IVS or FEATURE_ECALL_PSAP */

