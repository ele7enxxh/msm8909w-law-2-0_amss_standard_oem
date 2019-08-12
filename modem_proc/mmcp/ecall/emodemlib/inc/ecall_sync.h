#ifndef ECALL_SYNC_H_
#define ECALL_SYNC_H_
/*===========================================================================

                           E C A L L _ S Y N C . H

DESCRIPTION
        synchronization functions                                 

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/ecall/emodemlib/inc/ecall_sync.h#1 $ 
  $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $


when        who     what, where, why
--------    ---     ----------------------------------------------------------
09/01/09    sar     Ported to AMSS by making the necessary changes
09/01/09    sar     initial version  8.1.0  eCall ANSI C fixed-point reference source code

===========================================================================*/

#if (defined(FEATURE_ECALL_IVS) || defined(FEATURE_ECALL_PSAP))

#include "ecall_defines.h"

typedef struct {
  int32 amplitude[2];   /* amplitudes (average, maximum) */
  int32 shape[2*ECALL_NRS_CP+1];  /* shape of peak causing a sync event */
  int32 sign;           /* Sync Sign Marker */

  boolean  flag;           /* indicates sync success */
  int16 delay;          /* synchronization delay */
  int16 npeaks;         /* number of sync peaks detected */
  int16 npeaksChk;          /* number of sync peaks detected by sync check */
} ecall_sync_sub;

typedef struct {
  int32 *mem;
  int32 *memWakeup;        /* memory for wakeup tone detector */

  ecall_sync_sub syncPos;  /* regular sync (non-inverted) */
  ecall_sync_sub syncNeg;  /* inverted sync */

  int32 amplitude[3];      /* amplitudes (average, maximum, memory) */
  int32 shape[2*ECALL_NRS_CP+1];  /* shape of peak causing a sync event */
  int32 sign;              /* Sync Sign Marker */
  //int16 peakPos[4];        /* position of sync peaks within feedback message */

  boolean  flag;           /* indicates successful sync */
  Tern     invert;         /* indicates sync inversion */
  boolean  resync;         /* indicates resynchronization event */
  
  int16 delay;             /* synchronization delay (position) */
  int16 delayMem;          /* synchronization delay (memory) */
  int16 npeaks;            /* number of sync peaks detected */
  int16 npeaksMem;         /* number of sync peaks detected (memory) */
  int16 events;            /* number of subsequent equal sync events */

  Tern  check;             /* indicates sync check result (ternary variable) */
  int16 checkCnt;          /* counter for subsequent sync check failures */
  int16 index;             /* frame reference for sync evaluation */
} ecall_sync_state;


/*============================================================================*/
/* Sync functions                                                             */
/*----------------------------------------------------------------------------*/
/*==========================================================================

  FUNCTION    ECALL_SYNC_RESET

  DESCRIPTION
    main synchronization function

  DEPENDENCIES
    None.

  RETURN VALUE

  SIDE EFFECTS
    None

===========================================================================*/
void ecall_sync_reset
(
  ecall_sync_state *sync,
  int32 *mem,
  int32 *memWakeup
);

/*==========================================================================

  FUNCTION    ECALL_SYNC_SUB_RESET

  DESCRIPTION
    

  DEPENDENCIES
    None.

  RETURN VALUE

  SIDE EFFECTS
    None

===========================================================================*/
void ecall_sync_sub_reset
(
  ecall_sync_sub *ssub
);

/*==========================================================================

  FUNCTION    ECALL_SYNC

  DESCRIPTION
    main synchronization function

  DEPENDENCIES
    None.

  RETURN VALUE
   In:     const int16* pcm      -> input frame                               
           const char*  caller   -> text to identify PSAP or IVS              
   InOut:  ecall_sync_state*   sync    <-> sync struct                               

  SIDE EFFECTS
    None

===========================================================================*/
void ecall_sync
(
  ecall_sync_state *sync,
  const int16 *pcm,
  const char *caller,
  boolean invert
);

/*==========================================================================

  FUNCTION    ECALL_SYNC_SUB_PUT

  DESCRIPTION
              sync peak evaluation

  DEPENDENCIES
    None.

  RETURN VALUE
    InOut:  ecall_sync_state*   sync    <-> sync struct  
            ecall_sync_sub *ssub    <-> sync subsystem                            

  SIDE EFFECTS
    None

===========================================================================*/
void ecall_sync_sub_put
(
  const ecall_sync_state *sync, 
  ecall_sync_sub *ssub
);

/*==========================================================================

  FUNCTION    ECALL_SYNC_SUB_GET

  DESCRIPTION
              sync peak evaluation

  DEPENDENCIES
    None.

  RETURN VALUE
       InOut:  ecall_sync_state*   sync    <-> sync struct  
               ecall_sync_sub *ssub    <-> sync subsystem                           

  SIDE EFFECTS
    None

===========================================================================*/
void ecall_sync_sub_get
(
  ecall_sync_state *sync, 
  const ecall_sync_sub *ssub
);

/*==========================================================================

  FUNCTION    ECALL_SYNC_SUB_RUN

  DESCRIPTION
              sync peak evaluation

  DEPENDENCIES
    None.

  RETURN VALUE
    InOut:  ecall_sync_state*   sync    <-> sync struct  
            ecall_sync_sub *ssub  <-> sync subsystem                            
    In:     const char*  caller   -> modem identification                      
            const Int32* pPos     -> positive peaks positions                  
            const Int32* pCorr    -> positive peaks correlation values         
            const Int32* nPos     -> negative peaks positions                  
            const Int32* nCorr    -> negative peaks correlation values         

  SIDE EFFECTS
    None

===========================================================================*/
void ecall_sync_sub_run
(
  ecall_sync_sub *ssub,
  const char *caller,
  const int32 *pPos,
  const int32 *pCorr,
  const int32 *nPos,
  const int32 *nCorr
);

/*==========================================================================

  FUNCTION    ECALL_SYNC_CHECK

  DESCRIPTION
    main synchronization function

  DEPENDENCIES
    None.

  RETURN VALUE
        In:     const int16* pcm    -> input frame                                 
        InOut:  ecall_sync_state*   sync  <-> sync struct                                 

  SIDE EFFECTS
    None

===========================================================================*/
void ecall_sync_check
(
  ecall_sync_state *sync,
  const int16 *pcm,
  const char *caller
);

/*==========================================================================

  FUNCTION    ECALL_SYNC_TRACK

  DESCRIPTION
    

  DEPENDENCIES
    None.

  RETURN VALUE
   InOut:  SyncState* sync    <-> sync struct                                 
   In:     Bool       invert   -> port inversion flag                                                      

  SIDE EFFECTS
    None

===========================================================================*/
void ecall_sync_track
(
  ecall_sync_state *sync,
  boolean invert
);

/*==========================================================================

  FUNCTION    ECALL_SYNC_FILTER

  DESCRIPTION
    main synchronization function

  DEPENDENCIES
    None.

  RETURN VALUE
   InOut:  SyncState*   sync    <-> sync struct                              
   In:     const Int16* pcm      -> input frame                             
           Bool         invert   -> port inversion flag                                                   

  SIDE EFFECTS
    None

===========================================================================*/
void ecall_sync_filter
(
  ecall_sync_state *sync,
  const int16 *pcm,
  boolean invert
);

/*==========================================================================

  FUNCTION    ECALL_SYNC_SUB_CPY

  DESCRIPTION
    main synchronization function

  DEPENDENCIES
    None.

  RETURN VALUE
        In:     const int16* pcm    -> input frame                                 
        InOut:  ecall_sync_state*   sync  <-> sync struct                                 

  SIDE EFFECTS
    None

===========================================================================*/
void ecall_sync_sub_cpy
(
  const ecall_sync_sub *ssubIn,
  ecall_sync_sub *ssubOut
);

/*==========================================================================

  FUNCTION    ECALL_SYNC_SUB_CHK

  DESCRIPTION
    main synchronization function

  DEPENDENCIES
    None.

  RETURN VALUE
   InOut:  SyncSub*     ssub    <-> sync subsystem                            
   In:     const char*  caller   -> modem identification                      
           const Int32* posP     -> positive peaks positions                  
           const Int32* corrP    -> positive peaks correlation values         
           const Int32* posN     -> negative peaks positions                  
           const Int32* corrN    -> negative peaks correlation values         
           Int16        delay    -> reference delay                           
  SIDE EFFECTS
    None

===========================================================================*/
void ecall_sync_sub_chk
(
  ecall_sync_sub *ssub,
  const char *caller,
  const int32 *posP,
  const int32 *corrP,
  const int32 *posN,
  const int32 *corrN,
  int16 delay
);



/*==========================================================================

  FUNCTION    ECALL_PEAK_UPDATE

  DESCRIPTION
    update sync peak position  

  DEPENDENCIES
    None.

  RETURN VALUE
  In:     const int32* pos   -> vector of positions                          
          const int32* corr  -> vector of correlation values                 
          int16        dist  -> distance to be checked                       
  Return: int16              <- updated peak position                          

  SIDE EFFECTS
    None

===========================================================================*/
int16 ecall_peak_update
(
  const int32 *pos,
  const int32 *corr,
  int16 dist
);

/*==========================================================================

  FUNCTION    ECALL_CHECK_POS_PEAKS

  DESCRIPTION
    check positive sync peaks

  DEPENDENCIES
    None.

  RETURN VALUE
  InOut:  SyncSub*     ssub    <-> sync subsystem                            
  In:     const char*  caller   -> modem identification                      
          const Bool*  pdet     -> vector of peak detection flags            
          const Int16* p        -> vector of frame numbers                   
          const Int32* corr(X)  -> vector of correlation values              
          Int16        pos1     -> peak position 1                           
          Int16        pos2     -> peak position 2                           
          Int16        npeaks   -> number of detected peaks                  
          Int16        delay    -> target delay if sync successful                          

  SIDE EFFECTS
    None

===========================================================================*/
void ecall_peak_check
(
  ecall_sync_sub *ssub,
  const char *caller,
  const boolean *pdet,
  const int16 *p,
  const int32 *corrP,
  const int32 *corrN,
  const int32 *corr,
  int16 pos1, 
  int16 pos2,
  int16 npeaks,
  int16 delay
);

/*==========================================================================

  FUNCTION    ECALL_TONE_DETECT

  DESCRIPTION
    tone detection at 500 Hz or 800 Hz  

  DEPENDENCIES
    None.

  RETURN VALUE
         In:     const int16* pcm    -> input frame                                 
         InOut:  ecall_sync_state*   sync  <-> sync struct

  SIDE EFFECTS
    None

===========================================================================*/
void ecall_tone_detect
(
  ecall_sync_state *sync,  
  const int16 *pcm
);

#else

void ecall_sync_dummy_func(void);

#endif /* End FEATURE_ECALL_IVS or FEATURE_ECALL_PSAP */

#endif


