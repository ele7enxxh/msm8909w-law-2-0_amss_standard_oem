#ifndef ECALL_MODEM_H_
#define ECALL_MODEM_H_
/*===========================================================================

                           E C A L L _ M O D E M . H

DESCRIPTION
         modulator and demodulator functions

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/ecall/emodemlib/inc/ecall_modem.h#1 $ 
  $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $


when        who     what, where, why
--------    ---     ----------------------------------------------------------
09/01/09    sar     Ported to AMSS by making the necessary changes
09/01/09    sar     initial version  8.1.0  eCall ANSI C fixed-point reference source code

===========================================================================*/
#include "ecall_defines.h"
#include "DDIChipInfo.h"

#if (defined(FEATURE_ECALL_IVS) || defined(FEATURE_ECALL_PSAP))

typedef enum {
  ModUndef,
  Mod3bit4smp,
  Mod3bit8smp
} ecall_mod_type;

typedef struct {
  ecall_mod_type type;      /* identifies modulator type */

  int16 bpsym;       /* bits per symbol */
  int16 spmf;        /* samples per modulation frame */
  int16 mfpf;        /* modulation frames per frame = ECALL_PCM_LENGTH/spmf */
  int16 decpos1;     /* position 1st decoding trial */
  int16 decpos2;     /* position 2nd decoding trial */
  int16 wutperiod;   /* wakeup tone period in samples */
  int16 nfmute1;     /* number of muting frames 1st interval */
  int16 nfmute4;     /* number of muting frames 4th interval */
  int16 nfmuteall;   /* number of muting frames total */
  int16 nfdata;      /* number of data frames = ECALL_NRB_CODE_ARQ/(mfpf*bpsym) */

  const int16 *ulPulse;
  const int16 *ulPulseMatch;
  const int16 *mgTable;
  const int16 *wakeupSin;
  const int16 *wakeupCos;
} ecall_mod_state;

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
  ecall_mod_state*,
  ecall_mod_type
);

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
void ecall_modem_abort(void );
#define ECALL_MODEM_CONFIG DALCHIPINFO_ID_MDM9628
#endif

#ifdef FEATURE_ECALL_IVS
/*============================================================================*/
/* Modem functions                                                            */
/*----------------------------------------------------------------------------*/
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
  const ecall_mod_state*,
  const uint1*,
  int16*,
  uint32,
  int16,
  int16
);

/*==========================================================================

  FUNCTION    ECALL_IVS_RECEIVER

  DESCRIPTION
    IVS receiver function

  DEPENDENCIES
    None.

  RETURN VALUE
   In:     const int16* pcm     -> input to downlink receiver                 
   Out:    int32*       metric  <- metric[k]: k = 0..3 correlation values     
                                <- metric[4]: reliability factor (-1: skip)   
   Return: int16                <- demodulated message                        

  SIDE EFFECTS
    None

===========================================================================*/
int16 ecall_ivs_receiver
(
  const int16*,
  int32*
);

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
  const ecall_mod_state*,
  int16,
  int16*
);

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
  const uint8*,
  uint1*,
  int16
);

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
  const ecall_mod_state*,
  const int16*,
  ecall_IntLLR*
);

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
  const ecall_mod_state*,
  const int16*,
  ecall_IntLLR*
);

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
  const uint32*,
  uint1*,
  int16
);

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
  int32,
  int16
);

#ifdef FEATURE_ECALL_21KHZ_TONE
/*==========================================================================
  FUNCTION    ECALL_PCM_TONE

  DESCRIPTION
        transmits the 2.1 Khz tone to disable network Echo Cancellors

  DEPENDENCIES
    None.

  RETURN VALUE
    In:     uint8* pcmrev                                                                     
    out:    none 
    
  SIDE EFFECTS
    None
===========================================================================*/
void ecall_pcm_tone
(
  int16* pcmrev
);

#endif  /*end FEATURE_ECALL_21KHZ_TONE*/

#else

void ecall_modem_dummy_func(void);

#endif /* End Featureisation */
#endif

