/*==========================================================================

                           E C A L L   F E C . C

GENERAL DESCRIPTION
   Turbo encoder and decoder
   C Reference code 3GPP TS  26.268 

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2002 by Qualcomm Technologies Incorporated. All Rights Reserved.
===========================================================================*/
/*===========================================================================
                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/ecall/emodemlib/src/ecall_fec.c#1 $ 
$DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $


when        who     what, where, why
--------    ---     ----------------------------------------------------------
09/30/09    sar     ported to AMSS and renamed accordingly
09/01/09    sar     Initial version  8.1.0  taken from Pat

==========================================================================*/

#include "customer.h"
#include "comdef.h"

#include "ecall_fec.h"
#include "ecall_defines.h"
#include "ecall_rom.h"
#include "ecall_efs.h"
#include "msg.h"

#ifdef FEATURE_ECALL_PSAP
static ecall_IntLLR ecall_chLLRbuffer[ECALL_NRB_CODE_BUFFER];
static ecall_IntLLR Le12       [ECALL_NRB_INFO_CRC + ECALL_NRB_TAIL];
static ecall_IntLLR Le21       [ECALL_NRB_INFO_CRC + ECALL_NRB_TAIL];
static ecall_IntLLR alphaQ     [ECALL_FEC_STATES * 2];
static ecall_IntLLR betaQ      [ECALL_FEC_STATES * (ECALL_NRB_INFO_CRC + ECALL_NRB_TAIL + 1)];
#endif

#ifdef FEATURE_ECALL_IVS

/*===========================================================================

FUNCTION    ECALL_FEC_ENCODE

DESCRIPTION
            encoding of MSD 
    
DEPENDENCIES

RETURN VALUE
        InOut:  uint1* buffer  <-> takes info bits and returns coded bits

SIDE EFFECTS
  None

============================================================================*/
void ecall_fec_encode 
(
  uint1 *buffer
)
{
  /* I> in: 1120 | out: 1148 -> (input bits to reside at buffer start) */
  ecall_attach_crc(buffer, &buffer[ECALL_NRB_INFO_CRC]);

  /* -> in: 1148 | out: 1148 -> */
  ecall_scramble(&buffer[ECALL_NRB_INFO_CRC], buffer);

  /* -> in: 1148 | out: 3456 O> */
  ecall_encode_two(buffer);
}


/*===========================================================================

FUNCTION    ECALL_ATTACH_CRC

DESCRIPTION
            attaches CRC bits 
    
DEPENDENCIES

RETURN VALUE
        In:     const uint1* infoBits     -> input information bits      
        Out:    uint1*       infoWithCrc  <- bits with CRC attached               

SIDE EFFECTS
  None

============================================================================*/
void ecall_attach_crc
(
  const uint1 *infoBits, 
  uint1 *infoWithCrc
)
{
  int16 i =0, j =0;

  /* copy over */
  for (i = 0; i < ECALL_NRB_INFO_CRC; i++)
    {
      if (i < ECALL_NRB_INFO)
         infoWithCrc[i] = infoBits[i];
      else
         infoWithCrc[i] = 0;
    }

  /* generate parity part */
  for (i = 0; i < ECALL_NRB_INFO; i++) 
    {
      if (infoWithCrc[i] == 1) 
        {
          for (j = 0; j < ECALL_NRB_CRC+1; j++)
            infoWithCrc[j+i] ^= ecall_crcPolynomial[j];
        }
    }

  /* revert info part */
  for (i = 0; i < ECALL_NRB_INFO; i++)
    infoWithCrc[i] = infoBits[i];

}

/*===========================================================================

FUNCTION    ECALL_SCRAMBLE

DESCRIPTION
            attaches CRC bits 
    
DEPENDENCIES

RETURN VALUE
        In:     const uint1* in   -> non scrambled input bit sequence     
        Out:    uint1*       out  <- scrambled output bit sequence               

SIDE EFFECTS
  None

============================================================================*/
void ecall_scramble
(
  const uint1 *in, 
  uint1 *out
)
{
  int16 k =0;
  for (k = 0; k < ECALL_NRB_INFO_CRC; k++) 
    {
      out[k] = in[k]^ecall_scramblingSeq[k];
    }
}

/*===========================================================================

FUNCTION    ECALL_ENCODE_TWO

DESCRIPTION
            encoding of bit sequence

                
DEPENDENCIES

RETURN VALUE
        InOut:  uint1* codedBits  <-> scrambled bits to coded bits            

SIDE EFFECTS
  None

============================================================================*/
void ecall_encode_two
(
  uint1 *codedBits
)
{
  int16 i =0, j =0, k =0;
  uint1 *input = codedBits;

  /* buffer structure: info1|itail1 itail2 parity1|ptail1 parity2|ptail2 */
  i = ECALL_NRB_INFO_CRC + ECALL_NRB_TAIL*2;
  j = ECALL_NRB_INFO_CRC + ECALL_NRB_TAIL + i;

  /* input sequence expected at buffer position 0 */
  for (k = 0; k < ECALL_NRB_INFO_CRC; k++)
    {
      codedBits[i++] = input[k];
      codedBits[j++] = input[ecall_interleaverSeq[k]];
    }

  /* encoder one (info1: non-interleaved) */
  ecall_encode_one(codedBits, 1);

  /* encoder two (info2: interleaved)*/
  ecall_encode_one(codedBits, 2);

}

/*===========================================================================

FUNCTION    ECALL_ENCODE_ONE

DESCRIPTION
            convolutional encoding of each component 

                
DEPENDENCIES

RETURN VALUE
        In:     int16 encNr       -> component number            
        InOut:  uint1* codedBits  <-> bits to be encoded 

SIDE EFFECTS
  None

============================================================================*/
void ecall_encode_one
(
  uint1 *codedBits, 
  int16 encNr
)
{
  int32 i =0, j =0, k =0;
  uint16 encState =0, targetState =0;
  uint1  encOut =0, tail =0;

  /* start parity(info) */
  i = ECALL_NRB_INFO_CRC*(encNr) + ECALL_NRB_TAIL*(encNr+1);
  
  /* input is overwritten by output */
  for (k = 0; k < ECALL_NRB_INFO_CRC; k++) 
    {
      encOut = ecall_outputParity[encState*2 + codedBits[i]];
      encState = ecall_stateTrans[encState*2 + codedBits[i]];
      codedBits[i++] = encOut;
    }

  /* start parity(tail) and tail */
  i = ECALL_NRB_INFO_CRC*(encNr+1) + ECALL_NRB_TAIL*(encNr+1);
  j = ECALL_NRB_INFO_CRC+(encNr-1)*ECALL_NRB_TAIL;

  /* terminate trellis */
  for (k = 0; k < ECALL_NRB_TAIL; k++)
    {
      targetState = (encState << 1) & ((1 << ECALL_NRB_TAIL)-1);
      tail = (ecall_stateTrans[encState*2] == targetState) ? 0 : 1;

      codedBits[i++] = ecall_outputParity[encState*2 + tail];
      codedBits[j++] = tail;

      encState = targetState;
    }
}

#endif

#ifdef FEATURE_ECALL_PSAP
/*===========================================================================

FUNCTION    ECALL_FEC_DECODE

DESCRIPTION
            convolutional encoding of each component 
                
DEPENDENCIES

RETURN VALUE
         In:     const ecall_IntLLR* in      -> received soft bits                        
         int16           rv      -> redundancy version                        
         int16           decStep -> current decoding step                     
                                 -> negative vector length (update buffer) 
         Out:    uint1*  out     <- decoded MSD in binary representation
         boolean                 <- result of CRC check  

SIDE EFFECTS
  None

============================================================================*/
boolean ecall_fec_decode
(
  uint32 *msdWord, 
  int16 decStep
)
{
  boolean crcOk = FALSE;
    
    /* decode chLLRbuffer */
    ecall_decode_buffer(
        &ecall_chLLRbuffer[0],
        &ecall_chLLRbuffer[  ECALL_NRB_INFO_CRC +   ECALL_NRB_TAIL],
        &ecall_chLLRbuffer[  ECALL_NRB_INFO_CRC + 2*ECALL_NRB_TAIL],
        &ecall_chLLRbuffer[2*ECALL_NRB_INFO_CRC + 3*ECALL_NRB_TAIL], msdWord, decStep);

    if ((++decStep)%ECALL_DEC_STEPS == 0) {
      /* descramble and run CRC check */
      ecall_descramble(msdWord);
      crcOk = ecall_decode_crc(msdWord);
    }
 
  return crcOk;
}

/*===========================================================================
FUNCTION    ECALL_UPDATE_BUFFER

DESCRIPTION
            update channel LLR buffer with new soft bit 
                
DEPENDENCIES

RETURN VALUE
         In:     const ecall_IntLLR* softInBits    -> received soft bits                         
         int16         rv            -> redundancy version                  
         int16         length        -> length of redundancy version        
         InOut:  ecall_IntLLR*       chLLRbuffer  <-> decoder buffer        
                       
SIDE EFFECTS
  None
===========================================================================*/
void ecall_update_buffer
(
  const ecall_IntLLR *softInBits, 
  int16 rv,
  int16 bpos,
  int16 blen
)
{
  int32 temp;
  int16 i, j = ((rv%2) ? 0 : 1)*(ECALL_ARQ_MAX/2) + (rv/2)%(ECALL_ARQ_MAX/2);

  /* buffer is reinitialized every ARQ_MAX transmissions */
  if (rv%ECALL_ARQ_MAX == 0 && bpos == 0) {
    memset(ecall_chLLRbuffer, 0, sizeof(ecall_IntLLR)*ECALL_NRB_CODE_BUFFER);
  }
  /* update chLLRbuffer */
  for (i = 0; i < blen; i++) {
    temp = (int32)ecall_chLLRbuffer[ecall_redVerIndex[j][bpos+i]] + (int32)softInBits[i];

    ecall_chLLRbuffer[ecall_redVerIndex[j][bpos+i]] = (ABS(temp) < ECALL_LLR_MAX) ?
      (ecall_IntLLR)temp : (ecall_IntLLR)(SIGN(temp)*ECALL_LLR_MAX);
  }
}

/*===========================================================================
FUNCTION    ECALL_DECODE_BUFFER    

DESCRIPTION
            decoding of LLR buffer  
                
DEPENDENCIES

RETURN VALUE
   In:     const ecall_IntLLR* syst1    -> RX systematic soft bits             
           const ecall_IntLLR* syst2    -> interleaved RX systematic tail bits 
           const ecall_IntLLR* parity1  -> RX parity soft bits                 
           const ecall_IntLLR* parity2  -> interleaved RX parity soft bits     
   Out:    uint1*         decBits  <- decoded bits                                 
                       
SIDE EFFECTS
  None

============================================================================*/
void ecall_decode_buffer
(
  const ecall_IntLLR *syst1,
  const ecall_IntLLR *syst2,
  const ecall_IntLLR *parity1,
  const ecall_IntLLR *parity2, 
  uint32 *decWord,
  int16 decStep
) 
{
  int16 j, k, n;
  int32 temp =0;

  if(decStep == 0)
  {
    /* initialize memory */
    memset(Le12, 0, sizeof(ecall_IntLLR)*(ECALL_NRB_INFO_CRC + ECALL_NRB_TAIL));
    memset(Le21, 0, sizeof(ecall_IntLLR)*(ECALL_NRB_INFO_CRC + ECALL_NRB_TAIL));
  }
/* 1st pass */
  if (decStep%ECALL_DEC_STEPS == 0) {
    ecall_memscpy ( Le12, sizeof(Le12), 
                    Le21, sizeof(ecall_IntLLR)*(ECALL_NRB_INFO_CRC + ECALL_NRB_TAIL) );

    /* add received systematic bits to extrinsic information */
    for (j = 0; j < ECALL_NRB_INFO_CRC+ECALL_NRB_TAIL; j++) {
      temp = (int32)Le12[j] + (int32)syst1[j];
      Le12[j] = (ABS(temp) < ECALL_LLR_MAX) ?
        (ecall_IntLLR)temp : (ecall_IntLLR)(SIGN(temp)*ECALL_LLR_MAX);
    }
    /* backward iteration of decoding code one (to produce Le12) */
    ecall_bcjr_split(parity1, Le12, 0);
  }

  /* 2nd pass */
  if (decStep%ECALL_DEC_STEPS == 2*ECALL_DEC_STEPS/8) {
    /* forward iteration of decoding code one (to produce Le12) */
    ecall_bcjr_split(parity1, Le12, 1);
    /* interleave extrinsic information (produces interleaved Le12) */
    ecall_interleave(Le12, Le21);
  }

  /* 3rd pass */
  if (decStep%ECALL_DEC_STEPS == 4*ECALL_DEC_STEPS/8) {
    /* add received systematic bits to extrinsic information */
    for (j = 0; j < ECALL_NRB_INFO_CRC; j++) {
      temp = (int32)Le21[j] + (int32)syst1[ecall_interleaverSeq[j]];
      Le21[j] = (ABS(temp) < ECALL_LLR_MAX) ?
        (ecall_IntLLR)temp : (ecall_IntLLR)((SIGN(temp))*ECALL_LLR_MAX);
    }
    for (j = 0; j < ECALL_NRB_TAIL; j++) {
      Le21[j+ECALL_NRB_INFO_CRC] = syst2[j];
    }
    /* backward iteration of decoding code two (to produce Le21) */
    ecall_bcjr_split(parity2, Le21, 0);
  }

  /* 4th pass */
  if (decStep%ECALL_DEC_STEPS == 6*ECALL_DEC_STEPS/8) {
    /* forward iteration of decoding code two (to produce Le21) */
    ecall_bcjr_split(parity2, Le21, 1);
    /* deinterleave extrinsic information (produces Le21) */
    ecall_deinterleave(Le21, sizeof(Le21));
  }

  /* take final decision on bits */
  if((++decStep)%ECALL_DEC_STEPS == 0) {
    for (n = 0, j = 0; j < ECALL_NRB_INFO_CRC; n++) {
      decWord[n] = 0;
      for (k = 0; k < 32 && j < ECALL_NRB_INFO_CRC; k++, j++) {
        temp = ((int32)syst1[j] + (int32)Le21[j] + (int32)Le12[j] > 0) ? 0 : 1;
        decWord[n] ^= ((uint32)temp) << (31-k);
      }
    }
  }
}


/*===========================================================================

FUNCTION    ECALL_BCJR_SPLIT    

DESCRIPTION
            
                
DEPENDENCIES

RETURN VALUE
    In:     const ecall_IntLLR* parity      -> received parity soft bits
    InOut:  ecall_IntLLR*       extrinsic  <-> extrinsic information             
        
SIDE EFFECTS
  None

============================================================================*/
void ecall_bcjr_split
(
  const ecall_IntLLR *parity, 
  ecall_IntLLR *extrinsic,
  int16 decStep
)
{    
  if((decStep%2) > 0) 
     ecall_bcjr_fwd_it( parity, extrinsic); 
  else 
     ecall_bcjr_bwd_it( parity, extrinsic);
}


/*===========================================================================

FUNCTION    ECALL_BCJR_FWD_IT    

DESCRIPTION
           
                
DEPENDENCIES

RETURN VALUE
          
        
SIDE EFFECTS
  None

============================================================================*/
/*lint --e{701}*/
void ecall_bcjr_fwd_it
(
  const ecall_IntLLR *parity,
  ecall_IntLLR *extrinsic
)
{ 
  int16  i, k;
  int32  temp;
  ecall_IntLLR nom, den, norm, g1, g2;
  /* pointers */
  ecall_IntLLR *alpha1, *alpha2, *bTemp1;
 
  /* initialize alphaQ */
  alpha2 = &alphaQ[ECALL_FEC_STATES];
  alpha1 = &alphaQ[0];
  alpha1[0] = 0;
  for (i = 1; i < ECALL_FEC_STATES; i++) {
    alpha1[i] = -ECALL_LLR_MAX;
  }
  /* compute alphaQ (forward iteration) */
  for (k = 1; k <= ECALL_NRB_INFO_CRC+ECALL_NRB_TAIL; k++) {
    if (k-1 < ECALL_NRB_INFO_CRC) {
      bTemp1 = &betaQ[k*ECALL_FEC_STATES];
      norm   = -ECALL_LLR_MAX;

      for (i = 0; i < ECALL_FEC_STATES; i++) {
        /*lint -e734*/
        g1 = ecall_gamma_q(k, (ecall_revStateTrans[i<<1] << 1), parity, extrinsic);
        g2 = ecall_gamma_q(k, (ecall_revStateTrans[(i<<1)+1] << 1)+1, parity, extrinsic);
        /*lint +e734*/

        alpha2[i] = ecall_jac_log(
          (int32)alpha1[ecall_revStateTrans[i<<1]] + (int32)g1,
          (int32)alpha1[ecall_revStateTrans[(i<<1)+1]] + (int32)g2);
        norm = ecall_jac_log(
          (int32)alpha2[i], (int32)norm);
      }
      /* normalization of alphaQ */
      for (i = 0; i < ECALL_FEC_STATES; i++) {
        temp = (int32)alpha2[i] - (int32)norm;
        alpha2[i] = (temp < (-ECALL_LLR_MAX)) ? (ecall_IntLLR)(-ECALL_LLR_MAX) : (ecall_IntLLR)temp;
      }
      /* calculate extrinsic output */
      nom = -ECALL_LLR_MAX;
      den = -ECALL_LLR_MAX;

      for (i = 0; i < ECALL_FEC_STATES; i++) {
        if (ecall_outputParity[i<<1] == 0) {
          nom = ecall_jac_log(
            (int32)nom,
            (int32)alpha1[i] +
            (int32)bTemp1[ecall_stateTrans[i<<1]] +
            (int32)parity[k-1]/2);
        } else {
          nom = ecall_jac_log(
            (int32)nom,
            (int32)alpha1[i] +
            (int32)bTemp1[ecall_stateTrans[i<<1]] -
            (int32)parity[k-1]/2);
        }
        if (ecall_outputParity[(i<<1)+1] == 0) {
          den = ecall_jac_log(
            (int32)den,
            (int32)alpha1[i] +
            (int32)bTemp1[ecall_stateTrans[(i<<1)+1]] +
            (int32)parity[k-1]/2);
        } else {
          den = ecall_jac_log(
            (int32)den,
            (int32)alpha1[i] +
            (int32)bTemp1[ecall_stateTrans[(i<<1)+1]] -
            (int32)parity[k-1]/2);
        }
      }
      temp = (int32)nom - (int32)den;
      extrinsic[k-1] = (ABS(temp) < ECALL_LLR_MAX) ?
        (ecall_IntLLR)temp : (ecall_IntLLR)(SIGN(temp)*ECALL_LLR_MAX);

      /* swap alphaQ states */
      alpha1 = &alphaQ[ECALL_FEC_STATES*(k%2)];
      alpha2 = &alphaQ[ECALL_FEC_STATES*((k-1)%2)];
    } else {
      extrinsic[k-1] = 0;
    }
  }
}/* END forward iteration */


/*===========================================================================

FUNCTION    ECALL_BCJR_BWD_IT    

DESCRIPTION
           
                
DEPENDENCIES

RETURN VALUE
          
        
SIDE EFFECTS
  None

============================================================================*/
void ecall_bcjr_bwd_it
(
  const ecall_IntLLR *parity,
  ecall_IntLLR *extrinsic
)
{
  int16  i, k;
  int32  temp;
  ecall_IntLLR norm, g1, g2;
  /* pointers */
  ecall_IntLLR *bTemp1, *bTemp2;

  /* START backward */
  /* initialize betaQ */
  bTemp1 = &betaQ[(ECALL_NRB_INFO_CRC+ECALL_NRB_TAIL)*ECALL_FEC_STATES];
  bTemp1[0] = 0;
  for (i = 1; i < ECALL_FEC_STATES; i++) {
    bTemp1[i] = -ECALL_LLR_MAX;
  }
  /* compute betaQ (backward iteration) */
  for (k = ECALL_NRB_INFO_CRC+ECALL_NRB_TAIL; k >= 1; k--) {
    bTemp1 = &betaQ[(k-1)*ECALL_FEC_STATES];
    bTemp2 = &bTemp1[ECALL_FEC_STATES];
    norm   = -ECALL_LLR_MAX;

    for (i = 0; i < ECALL_FEC_STATES; i++)
    {
      /*lint -e734*/
      g1 = ecall_gamma_q(k, (i<<1), parity, extrinsic);
      g2 = ecall_gamma_q(k, (i<<1)+1, parity, extrinsic);
      /*lint +e734*/

      bTemp1[i] = ecall_jac_log(
        (int32)bTemp2[ecall_stateTrans[i<<1]] + (int32)g1,
        (int32)bTemp2[ecall_stateTrans[(i<<1)+1]] + (int32)g2);
      norm = ecall_jac_log(
        (int32)bTemp1[i], (int32)norm);
    }
    /* normalization of betaQ */
    for (i = 0; i < ECALL_FEC_STATES; i++) {
      temp = (int32)bTemp1[i] - (int32)norm;
      bTemp1[i] = (temp < (-ECALL_LLR_MAX)) ? (ecall_IntLLR)(-ECALL_LLR_MAX) : (ecall_IntLLR)temp;
    }
  }
}/* END ecall_bcjr_bwd_it */



/*===========================================================================

FUNCTION    ECALL_INTERLEAVE    

DESCRIPTION
           
                
DEPENDENCIES

RETURN VALUE
        In:     const ecall_IntLLR* in   -> input sequence
        Out:    ecall_IntLLR*       out  <- output sequence   

SIDE EFFECTS
  None

============================================================================*/
void ecall_interleave
(
  const ecall_IntLLR *in,
  ecall_IntLLR *out
)
{
  int16 i =0;
  for (i = 0; i < ECALL_NRB_INFO_CRC; i++) {
    out[i] = in[ecall_interleaverSeq[i]];
  }
}

/*===========================================================================

FUNCTION    ECALL_DEINTERLEAVE    

DESCRIPTION
            Turbo code deinterleaver 
                
DEPENDENCIES

RETURN VALUE
       InOut:  ecall_IntLLR* inout  <-> input and deinterleaved output sequence  

SIDE EFFECTS
  None

============================================================================*/
void ecall_deinterleave
(
  ecall_IntLLR *inout,
  uint32 inout_size
)
{
  int16 i =0;
  ecall_IntLLR temp[ECALL_NRB_INFO_CRC];

  for (i = 0; i < ECALL_NRB_INFO_CRC; i++) 
    {
      temp[ecall_interleaverSeq[i]] = inout[i];
    }
  ecall_memscpy ( inout, inout_size, 
                  temp, sizeof(ecall_IntLLR)*ECALL_NRB_INFO_CRC);
}

/*===========================================================================

FUNCTION    ECALL_DESCRAMBLE    

DESCRIPTION
            descrambles decoded bits
                
DEPENDENCIES

RETURN VALUE
       InOut:  uint1* inout  <-> input and output bit sequence   

SIDE EFFECTS
  None

============================================================================*/
void ecall_descramble
(
  uint32 *inout
)
{
  int16 k;

  for (k = 0; k < ECALL_NRW_INFO_CRC; k++) {
    inout[k] ^= ecall_scrSeqWordRev[k];
  }
}

/*===========================================================================

FUNCTION    ECALL_DECODE_CRC    

DESCRIPTION
            check CRC of decoded bits 
                
DEPENDENCIES

RETURN VALUE
       In:     const uint1* codedBits  -> decoded bit sequence to be checked 
       boolean                         <- result of CRC check 

SIDE EFFECTS
  None

============================================================================*/
boolean ecall_decode_crc
(
  const uint32 *decWord
)
{
  int16 i =0, j =0;
  uint32 inChunk, myChunk, mem = decWord[0];

  for (i = 1; i < ECALL_NRW_INFO_CRC; i++) {
    for (j = 0; j < ECALL_CRC_BPWORD/ECALL_CRC_BPCHUNK; j++) {

      myChunk = mem >> (ECALL_CRC_BPWORD-ECALL_CRC_BPCHUNK);
      inChunk = ((1 << ECALL_CRC_BPCHUNK)-1) &
        (decWord[i] >> (ECALL_CRC_BPWORD/ECALL_CRC_BPCHUNK-1-j)*ECALL_CRC_BPCHUNK);

      /* step 1) shift register one chunk left towards MSB */
      /* step 2) determine new chunk and write into memory */
      /* step 3) XOR with respective resulting poly */

      mem = mem << ECALL_CRC_BPCHUNK;
      mem ^= inChunk;
      mem ^= ecall_crcPolyWordRev[myChunk];
    }
  }
  return (mem == 0) ? TRUE : FALSE;
}

/*===========================================================================

FUNCTION    ECALL_GAMMA_Q    

DESCRIPTION
           compute gamma values for BCJR algorithm 
                
DEPENDENCIES

RETURN VALUE
   In:     int16         k      -> bit position 
           int16         l      -> state  
           const ecall_IntLLR* parity     -> received parity bits 
           const ecall_IntLLR* extrinsic  -> sum of extrinsic and systematic bits   
           Return: ecall_IntLLR           <- value of gamma(k,l)  

SIDE EFFECTS
  None

============================================================================*/
ecall_IntLLR ecall_gamma_q
(
  int16 k, int16 l,
  const ecall_IntLLR *parity,
  const ecall_IntLLR *extrinsic
)
{
  /* extrinsic must include received systematic bits */
  if (l%2) {
    if (ecall_outputParity[l] == 0) {
      return (ecall_IntLLR)((-(int32)extrinsic[k-1] + (int32)parity[k-1])/2);
    } else {
      return (ecall_IntLLR)((-(int32)extrinsic[k-1] - (int32)parity[k-1])/2);
    }
  } else {
    if (ecall_outputParity[l] == 0) {
      return (ecall_IntLLR)(((int32)extrinsic[k-1] + (int32)parity[k-1])/2);
    } else {
      return (ecall_IntLLR)(((int32)extrinsic[k-1] - (int32)parity[k-1])/2);
    }
  }
}


/*===========================================================================

FUNCTION    ECALL_JAC_LOG    

DESCRIPTION
           Jacobian logarithm
                
DEPENDENCIES

RETURN VALUE
    In:    ecall_IntLLR a  -> value one 
           ecall_IntLLR b  -> value two

    Return: ecall_IntLLR    <- Jacobian logarithm  

SIDE EFFECTS
  None

============================================================================*/
ecall_IntLLR ecall_jac_log
(
  int32 a,
  int32 b
)
{
  int32 maxab, ind;

#if ECALL_LLR_TYPE == 0
  if (a > b) {
    maxab = a;
    ind = (a-b) >> (ECALL_LOGEXP_DELTA + ECALL_LOGEXP_QIN);
  } else {
    maxab = b;
    ind = (b-a) >> (ECALL_LOGEXP_DELTA + ECALL_LOGEXP_QIN);
  }
  maxab += (ind >= ECALL_LOGEXP_RES) ? 0 : ecall_logExpTable[ind];
#elif ECALL_LLR_TYPE == 1 
  if (a > b) {
    maxab = a;
    ind = (a-b);
  } else {
    maxab = b;
    ind = (b-a);
  }
  maxab += (ind >= (ECALL_LOGEXP_RES-1)) ? 0 : 1;
#endif   
  if (maxab >= ECALL_LLR_MAX) {
    return ECALL_LLR_MAX;
  } else {
    return (ecall_IntLLR)maxab;
  }
}


#else

void ecall_fec_dummy_func(void);

#endif /* End FEATURE_ECALL IVS or PSAP */


