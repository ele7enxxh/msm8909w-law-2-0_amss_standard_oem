#ifndef ECALL_PSAP_H_
#define ECALL_PSAP_H_
/*===========================================================================

                        E C A L L  _ P S A P . C

DESCRIPTION
    eCall PSAP modem                                      

REFERENCES
  3GPP TS 26.267 

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


  Copyright(c) 2009 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/ecall/emodem/src/ecall_psap.h#1 $ 
  $DateTime: 2016/12/13 08:00:05 $ 
  $Author: mplcsds1 $

when        who     what, where, why
--------    ---     ----------------------------------------------------------
09/01/09    sar     Ported to AMSS by making the necessary changes
09/01/09    sar     initial version  8.1.0  eCall ANSI C fixed-point reference source code

===========================================================================*/
#ifdef FEATURE_ECALL_PSAP

/*==========================================================================

  FUNCTION    ECALL_PSAP_RECEIVE_MSD

  DESCRIPTION
    
  DEPENDENCIES
    None.

  RETURN VALUE
  
  SIDE EFFECTS
    None

===========================================================================*/
void ecall_psap_receive_msd
(
  const uint8 *msd,
  int16 length
);

#else

void ecall_psap_dummy_func(void);

#endif //End FEATURE_ECALL_PSAP

#endif

