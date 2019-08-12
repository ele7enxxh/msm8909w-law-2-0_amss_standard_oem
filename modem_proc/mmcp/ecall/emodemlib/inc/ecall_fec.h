#ifndef ECALL_FEC_H_
#define ECALL_FEC_H_
/*==========================================================================

                           E C A L L   F E C . H

GENERAL DESCRIPTION
   Turbo encoder and decoder 

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2002 by Qualcomm Technologies Incorporated. All Rights Reserved.
===========================================================================*/
/*==========================================================================
                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/ecall/emodemlib/inc/ecall_fec.h#1 $ 
$DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $


when        who     what, where, why
--------    ---     ---------------------------------------------------------
09/30/09    sar     ported to AMSS and renamed accordingly
09/01/09    sar     Initial version  8.1.0  taken from Pat

==========================================================================*/
#include "ecall_defines.h"
#include "ecall_rom.h"


/*============================================================================*/
/* Encoder functions                                                          */
/*----------------------------------------------------------------------------*/
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
  uint1*
);

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
  const uint1*,
  uint1*
);

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
  const uint1*,
  uint1*
);

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
  uint1*
);

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
  uint1*,
  int16
);

#endif

#ifdef FEATURE_ECALL_PSAP
/*============================================================================*/
/* Decoder functions                                                          */
/*----------------------------------------------------------------------------*/

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
  uint32*,
  int16
);

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

============================================================================*/
void ecall_update_buffer
(
  const ecall_IntLLR*,
  int16,
  int16,
  int16
);

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
  const ecall_IntLLR*,
  const ecall_IntLLR*,
  const ecall_IntLLR*,
  const ecall_IntLLR*, 
  uint32*,
  int16
);



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
  const ecall_IntLLR*,
  ecall_IntLLR*,
  int16
);

/*===========================================================================

FUNCTION    ECALL_BCJR_FWD_IT    

DESCRIPTION
           
                
DEPENDENCIES

RETURN VALUE
          
        
SIDE EFFECTS
  None

============================================================================*/
void ecall_bcjr_fwd_it
(
  const ecall_IntLLR*,
  ecall_IntLLR*
);

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
  const ecall_IntLLR*,
  ecall_IntLLR*
);

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
  const ecall_IntLLR*,
  ecall_IntLLR*
);

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
  ecall_IntLLR*,
  uint32
);

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
  uint32*
);


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
  const uint32*
);

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
  int16,
  int16,
  const ecall_IntLLR*,
  const ecall_IntLLR*
);

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
  int32,
  int32
);



#else

void ecall_fec_dummy_func(void);

#endif  /* END feature FEATURE_ECALL_IVS or FEATURE_ECALL_PSAP */

#endif

