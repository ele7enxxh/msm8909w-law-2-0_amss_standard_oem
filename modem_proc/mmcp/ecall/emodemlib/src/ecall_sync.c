/*===========================================================================

                           E C A L L _ S Y N C . C

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/ecall/emodemlib/src/ecall_sync.c#1 $ 
  $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $


when        who     what, where, why
--------    ---     ----------------------------------------------------------
09/01/09    sar     Ported to AMSS by making the necessary changes
09/01/09    sar     initial version  8.1.0  eCall ANSI C fixed-point reference source code

===========================================================================*/
#include "comdef.h"
#include "customer.h"

#if (defined(FEATURE_ECALL_IVS) || defined(FEATURE_ECALL_PSAP))

#include "ecall_sync.h"
#include "ecall_defines.h"
#include "ecall_modem.h"
#include "ecall_rom.h"
#include "ecall_modem_apis.h"
#include "ecall_efs.h"
#include "msg.h"
#include "err.h"
#include <stringl/stringl.h>

/*==========================================================================

  FUNCTION    ECALL_SYNC_RESET

  DESCRIPTION
    main synchronization function

  DEPENDENCIES
    None.

  RETURN VALUE
   InOut:  ecall_sync_state* sync      <-> sync struct                               
   In:     int32*     mem        -> pointer to sync memory                   
   int32*     memWakeup  -> pointer to sync wakeup memory 
  SIDE EFFECTS
    None

===========================================================================*/
void ecall_sync_reset
(
  ecall_sync_state *sync,
  int32 *mem,
  int32 *memWakeup
)
{
  int16 i=0;

  /* assign buffer pointers */
  if (mem != NULL) {
    sync->mem = mem;
    sync->memWakeup = memWakeup;
  }
  /* reset variables */
  sync->flag      = FALSE;
  sync->invert    = Minus;
  sync->resync    = FALSE;
  sync->delay     = 0;
  sync->delayMem  = 0;
  sync->npeaks    = 0;
  sync->npeaksMem = 0;
  sync->events    = 0;
  sync->sign      = 0;

  /* clear memory */
  for (i = 0; i <= 2*ECALL_NRS_CP; i++) {
    sync->shape[i] = 0;
  }
  sync->amplitude[0] = 0;
  sync->amplitude[1] = 0;
  sync->amplitude[2] = 0;

  /* clear sync subsystem */
  ecall_sync_sub_reset(&sync->syncPos);
  ecall_sync_sub_reset(&sync->syncNeg);

  /* initialize control message sync */
  sync->check    = Zero;
  sync->checkCnt = 0;
  sync->index    = ECALL_NRF_DLMUTE1LL + ECALL_NRF_DLDATA - 1;

  /* clear sync buffers */
  if (sync->mem != NULL) {
    memset(sync->mem, 0, sizeof(int32)*ECALL_NRS_MEMSYNC);
  }
  if (sync->memWakeup != NULL) {
    memset(sync->memWakeup, 0, sizeof(int32)*2*(ECALL_NRF_SYNC+1));
  }
}

/*==========================================================================

  FUNCTION    ECALL_SYNC_SUB_RESET

  DESCRIPTION
    
  DEPENDENCIES
    None.

  RETURN VALUE

  SIDE EFFECTS
    None

===========================================================================*/
void ecall_sync_sub_reset(ecall_sync_sub *ssub)
{
  int16 i;

  for (i = 0; i <= 2*ECALL_NRS_CP; i++) {
    ssub->shape[i] = 0;
  }
  ssub->flag   = FALSE;
  ssub->delay  = 0;
  ssub->npeaks = 0;

  ssub->amplitude[0] = 0;
  ssub->amplitude[1] = 0;
  ssub->sign         = 0;
}

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
/*lint -esym(715,caller) */
void ecall_sync
(
  ecall_sync_state *sync,
  const int16 *pcm,
  const char *caller,
  boolean invert
)
{
  int16 offset = ecall_syncIndexPreamble[ECALL_SYNC_IDXLEN-1] + ECALL_PCM_LENGTH + 2*ECALL_NRS_CP;

  /* set buffer pointers */
  int32 *posP  = &sync->mem[offset +  0];
  int32 *posN  = &sync->mem[offset +  9];
  int32 *corrP = &sync->mem[offset + 18];
  int32 *corrN = &sync->mem[offset + 27 + 9*2*ECALL_NRS_CP];

  /* test for 500 Hz or 800 Hz sine tone and run sync filter */
  ecall_tone_detect(sync, pcm);
  ecall_sync_filter(sync, pcm, invert);

  /* copy data to subsystem */
  ecall_sync_sub_put(sync, &sync->syncPos);
  ecall_sync_sub_put(sync, &sync->syncNeg);

  /* peak evaluation for each subsystem */
  ecall_sync_sub_run(&sync->syncPos, caller, posP, corrP, posN, corrN);
  ecall_sync_sub_run(&sync->syncNeg, caller, posN, corrN, posP, corrP);

  /* decision logic */
  if (sync->syncPos.flag == TRUE) {
    if ( (sync->syncNeg.flag == TRUE) &&
         ((sync->syncNeg.npeaks >= 4) || ((sync->syncNeg.npeaks >= 3) && (sync->syncPos.npeaks > 0))) &&
         (sync->syncPos.sign < sync->syncNeg.sign) &&
         (sync->syncPos.sign > 0) &&
         (sync->syncNeg.sign > 0)
      )
    {
      sync->invert = TRUE;
      sync->sign = sync->syncNeg.sign;
      sync->npeaksMem = sync->syncNeg.npeaks;

      ecall_sync_sub_get(sync, &sync->syncNeg);
      ECALL_MSG_3( ECALL_HIGH, "sync detected; delay: %+4d; npeaks: %+4d (inverted sync) events %d (continue...)",
                                sync->delay,
                                sync->npeaksMem,
                                sync->events );
      ECALL_MSG_3( ECALL_HIGH, "sync detected; sign: %+4d, Pos.sign: %+4d, Neg.sign: %+4d (inverted sync)",
                                sync->sign,
                                sync->syncPos.sign,
                                sync->syncNeg.sign );
    }
    else if ( (sync->invert == FALSE) ||
              ( (sync->syncPos.flag == TRUE) && 
                ((sync->syncPos.npeaks >= 4) || ((sync->syncPos.npeaks >= 3) && ((sync->syncNeg.npeaks >0) || (sync->syncNeg.flag == FALSE)))) &&
                (sync->syncPos.sign > 0) ) )
    {
      sync->invert = FALSE;
      sync->sign = sync->syncPos.sign;
      sync->npeaksMem = sync->syncPos.npeaks;

      ecall_sync_sub_get(sync, &sync->syncPos);
      ECALL_MSG_3( ECALL_HIGH, "sync detected; delay: %+4d; npeaks: %+4d (regular sync) events %d (continue...)",
                                sync->delay,
                                sync->npeaksMem,
                                sync->events );
      ECALL_MSG_3( ECALL_HIGH, "sync detected; sign: %+4d, Pos.sign: %+4d, Neg.sign: %+4d (regular sync)",
                                sync->sign,
                                sync->syncPos.sign,
                                sync->syncNeg.sign );
    }
    else {
      sync->flag = FALSE;
    }
  }
  else if ( (sync->syncNeg.flag == TRUE) &&
            ( (sync->invert == TRUE) || 
              ( ((sync->syncNeg.npeaks >= 4) || ((sync->syncNeg.npeaks >= 3) && (sync->syncPos.npeaks > 0))) && (sync->syncNeg.sign > 0) )) )
  {
    sync->invert = TRUE;
    sync->sign = sync->syncNeg.sign;
    sync->npeaksMem = sync->syncNeg.npeaks;

    ecall_sync_sub_get(sync, &sync->syncNeg);
    ECALL_MSG_3( ECALL_HIGH, "[%c] sync detected; delay: %+4d; npeaks: %+4d (inverted sync) (continue...)",
                              &caller[0],
                              sync->delay,
                              sync->npeaksMem );
    ECALL_MSG_3( ECALL_HIGH, "sync detected; sign: %+4d, Pos.sign: %+4d, Neg.sign: %+4d (inverted sync)",
                              sync->sign,
                              sync->syncPos.sign,
                              sync->syncNeg.sign );
  }
  else {
    sync->flag = FALSE;
  }
  if (sync->flag == TRUE) {
    ecall_sync_sub_reset(&sync->syncPos);
    ecall_sync_sub_reset(&sync->syncNeg);
  }
}


/*==========================================================================

  FUNCTION    ECALL_SYNC_CHECK

  DESCRIPTION
    main synchronization function

  DEPENDENCIES
    None.

  RETURN VALUE
        In:     const int16* pcm    -> input frame                                 
        InOut:  ecall_sync_state*   sync  <-> sync struct
                const char*  caller  -> modem identification                                  

  SIDE EFFECTS
    None

===========================================================================*/
void ecall_sync_check
(
  ecall_sync_state *sync,
  const int16 *pcm,
  const char *caller
)
{
  int16 i, ref, dmax, dmin, imax, icen, imin;
  //int16 dcen;
  int16 nCheck, maxCheck;
  int16 offset = ecall_syncIndexPreamble[ECALL_SYNC_IDXLEN-1] + ECALL_PCM_LENGTH + 2*ECALL_NRS_CP;

  int16 delay  = 0;
  int16 npeaks = 0;
  int32 pulseP = 0;
  int32 pulseN = 0;

  /* auxiliary struct */
  ecall_sync_sub ssub;

  /* set buffer pointers */
  int32 *posP  = &sync->mem[offset +  0];
  int32 *posN  = &sync->mem[offset +  9];
  int32 *corrP = &sync->mem[offset + 18];
  int32 *corrN = &sync->mem[offset + 27 + 9*2*ECALL_NRS_CP];

  /* initialize and compute search window */
  sync->flag = FALSE;

  if (sync->delayMem > 0 || sync->delayMem <= -ECALL_PCM_LENGTH) {
    ECALL_MSG_2( ECALL_ERROR, "[%c] sync delay (%+1d) out of range. Abort",
                               &caller[0],
                               sync->delayMem );
    ecall_modem_abort();
  }

  nCheck = ECALL_NRS_CHECK;

  ref  = ((sync->delayMem - nCheck) + 4 + ECALL_NRS_CP) - ECALL_PCM_LENGTH;
  imin = ref/ECALL_PCM_LENGTH + 1 + ECALL_NRF_DLCHUNK;
  dmin = (ref%ECALL_PCM_LENGTH - 4) - ECALL_NRS_CP;

  ref  = sync->delayMem + 4 + ECALL_NRS_CP - ECALL_PCM_LENGTH;
  icen = ref/ECALL_PCM_LENGTH + 1 + ECALL_NRF_DLCHUNK;
  //dcen = ref%ECALL_PCM_LENGTH - 4 - ECALL_NRS_CP;

  ref  = sync->delayMem + nCheck + 4 + ECALL_NRS_CP + ECALL_PCM_LENGTH;
  imax = (ref-1)/ECALL_PCM_LENGTH + ECALL_NRF_DLCHUNK;
  dmax = (((ref-1)%ECALL_PCM_LENGTH + 1 - ECALL_PCM_LENGTH) - 4) - ECALL_NRS_CP;

  /* sync filter function */
  ecall_sync_filter(sync, pcm, FALSE);

  /* run sync check */
  if (sync->index == icen) {
    ecall_sync_sub_chk(&sync->syncPos, caller,
      posP, corrP, posN, corrN, sync->delayMem);
    ecall_sync_sub_chk(&sync->syncNeg, caller,
      posN, corrN, posP, corrP, sync->delayMem);
  }

  /* run sync tracking */
  if (sync->index >= imin && sync->index <= imax) {

    /* check for regular sync */
    sync->syncPos.delay -= ECALL_PCM_LENGTH;

    ecall_sync_sub_cpy(&sync->syncPos, &ssub);
    ecall_sync_sub_reset(&sync->syncPos);

    sync->syncPos.amplitude[1] = sync->amplitude[1];
    ecall_sync_sub_run(&sync->syncPos, caller, posP, corrP, posN, corrN);

    if (sync->syncPos.flag == TRUE && (
      (sync->index == imin && sync->syncPos.delay < dmin) ||
      (sync->index == imax && sync->syncPos.delay > dmax)))
    {
      sync->syncPos.flag = FALSE;
    }
    if (ssub.flag == TRUE && sync->syncPos.flag == TRUE) {
      if (sync->syncPos.npeaks < ssub.npeaks ||
        (sync->syncPos.npeaks == ssub.npeaks &&
        sync->syncPos.amplitude[0] < ssub.amplitude[0]))
      {
        ecall_sync_sub_cpy(&ssub, &sync->syncPos);
      }
    } else if (ssub.flag == TRUE) {
      ecall_sync_sub_cpy(&ssub, &sync->syncPos);
    }

    /* check for inverted sync */
    sync->syncNeg.delay -= ECALL_PCM_LENGTH;

    ecall_sync_sub_cpy(&sync->syncNeg, &ssub);
    ecall_sync_sub_reset(&sync->syncNeg);

    sync->syncNeg.amplitude[1] = sync->amplitude[1];
    ecall_sync_sub_run(&sync->syncNeg, caller, posN, corrN, posP, corrP);

    if (sync->syncNeg.flag == TRUE && (
      (sync->index == imin && sync->syncNeg.delay < dmin) ||
      (sync->index == imax && sync->syncNeg.delay > dmax)))
    {
      sync->syncNeg.flag = FALSE;
    }
    if (ssub.flag == TRUE && sync->syncNeg.flag == TRUE) {
      if (sync->syncNeg.npeaks < ssub.npeaks ||
        (sync->syncNeg.npeaks == ssub.npeaks &&
        sync->syncNeg.amplitude[0] < ssub.amplitude[0]))
      {
        ecall_sync_sub_cpy(&ssub, &sync->syncNeg);
      }
    } else if (ssub.flag == TRUE) {
      ecall_sync_sub_cpy(&ssub, &sync->syncNeg);
    }
  }

  /* take final sync check and tracking decision */
  if (sync->index == imax) {
    sync->flag = TRUE;
    maxCheck = MAX(sync->syncPos.npeaksChk, sync->syncNeg.npeaksChk);

    if (sync->syncPos.flag == TRUE && sync->syncNeg.flag == TRUE) {
      for (i = 0; i <= 2*ECALL_NRS_CP; i++) {
        pulseP += (sync->shape[i])*(sync->syncPos.shape[i]);
        pulseN += (sync->shape[i])*(sync->syncNeg.shape[i]);
      }
      /*lint -e704*/
      pulseP = (pulseP >> 16)*(sync->syncPos.amplitude[0] >> 16);
      pulseN = (pulseN >> 16)*(sync->syncNeg.amplitude[0] >> 16);
      /*lint +e704*/

      if ((
        sync->syncPos.npeaks >= sync->syncNeg.npeaks && pulseP > pulseN) || (
        sync->syncPos.npeaks > sync->syncNeg.npeaks && pulseP > pulseN/2 &&
        sync->syncNeg.npeaksChk == 0))
      {
        sync->check = Plus;
        delay  = sync->syncPos.delay;
        npeaks = sync->syncPos.npeaks;
      }
      else if ((
        sync->syncPos.npeaks <= sync->syncNeg.npeaks && pulseP < pulseN) || (
        sync->syncPos.npeaks < sync->syncNeg.npeaks && pulseP/2 < pulseN &&
        sync->syncPos.npeaksChk == 0))
      {
        sync->check = Minus;
        delay  = sync->syncNeg.delay;
        npeaks = sync->syncNeg.npeaks;
      }
      else if (sync->syncPos.npeaksChk > sync->syncNeg.npeaksChk) {
        sync->check = Plus;
      }
      else if (sync->syncNeg.npeaksChk > sync->syncPos.npeaksChk) {
        sync->check = Minus;
      }
      else {
        sync->check = Zero;
      }
    }
    else if (sync->syncPos.flag == TRUE) {
      sync->check = Plus;
      delay  = sync->syncPos.delay;
      npeaks = sync->syncPos.npeaks;
    }
    else if (sync->syncNeg.flag == TRUE) {
      sync->check = Minus;
      delay  = sync->syncNeg.delay;
      npeaks = sync->syncNeg.npeaks;
    }
    else if (sync->syncPos.npeaksChk > sync->syncNeg.npeaksChk) {
      sync->check = Plus;
    }
    else if (sync->syncNeg.npeaksChk > sync->syncPos.npeaksChk) {
      sync->check = Minus;
    }
    else {
      sync->check = Zero;
    }

    if (delay != 0 &&
      delay != (i = (sync->delayMem)-(imax%ECALL_NRF_DLCHUNK)*ECALL_PCM_LENGTH))
    {
      if ((ABS(delay-i) == 2) ||(maxCheck < npeaks)) {
        sync->delayMem = delay + (imax%ECALL_NRF_DLCHUNK)*ECALL_PCM_LENGTH;

        ECALL_MSG_2( ECALL_HIGH, "[%c] sync check detects sample slip! [%+1d sample(s)]",
                                  &caller[0],
                                  delay-i );
      }
      else if (sync->syncPos.npeaksChk > sync->syncNeg.npeaksChk) {
        sync->check = Plus;
      }
      else if (sync->syncNeg.npeaksChk > sync->syncPos.npeaksChk) {
        sync->check = Minus;
      }
      else {
        sync->check = Zero;
      }
    }
    ecall_sync_sub_reset(&sync->syncPos);
    ecall_sync_sub_reset(&sync->syncNeg);

    if (sync->mem != NULL) {
      memset(sync->mem, 0, sizeof(int32)*ECALL_NRS_MEMSYNC);
    }
  }
}

/*============================================================================*/
/* IVS FUNCTION: SyncTrack                                                    */
/*----------------------------------------------------------------------------*/
/* Description: uplink sync tracker                                           */
/*                                                                            */
/* InOut:  SyncState* sync    <-> sync struct                                 */
/* In:     Bool       invert   -> port inversion flag                         */
/*----------------------------------------------------------------------------*/
void ecall_sync_track
(
  ecall_sync_state *sync,
  boolean invert
)
{
  int16 i, ref, dmin, dmax, imin, imax, nTrack, indP, indN;

  int16 offset = ecall_syncIndexPreamble[ECALL_SYNC_IDXLEN-1] + ECALL_PCM_LENGTH + 2*ECALL_NRS_CP;
  int16 npulse = 2*ECALL_NRS_CP+1;
  int16 npeaks = 0;

  int32 *posP, *posN, *corrP, *corrN, ampP, ampN;

  /* set buffer pointers */
  if (invert == TRUE) {
    posN  = &sync->mem[offset +  0];
    posP  = &sync->mem[offset +  9];
    corrN = &sync->mem[offset + 18];
    corrP = &sync->mem[offset + 27 + 9*2*ECALL_NRS_CP];
  } else {
    posP  = &sync->mem[offset +  0];
    posN  = &sync->mem[offset +  9];
    corrP = &sync->mem[offset + 18];
    corrN = &sync->mem[offset + 27 + 9*2*ECALL_NRS_CP];
  }

  /* initialize and compute search window */
  sync->flag = FALSE;

  if (sync->delay > 0 || sync->delay <= -ECALL_PCM_LENGTH) {
    ECALL_MSG_1( ECALL_ERROR, "[PSAP] sync delay (%+1d) out of range. Abort",
                               sync->delayMem );
    ecall_modem_abort();
  }

  nTrack = ECALL_NRS_TRACK;

  ref = sync->delay + 4 + ECALL_PCM_LENGTH + ECALL_NRS_CP;

  /* check peaks at expected position */
  indP = 7 + (i = (ref - 2*ECALL_PCM_LENGTH) - ECALL_PEAK_DIST_PN)/ECALL_PCM_LENGTH;
  if (posP[indP] == i) {
    npeaks++;
  }
  indN = 7 + (i = ref - 2*ECALL_PCM_LENGTH)/ECALL_PCM_LENGTH;
  if (posN[indN] == i) {
    npeaks++;
  }

  /* search for positive peak */
  dmin = ((ref - 2*ECALL_PCM_LENGTH) - ECALL_PEAK_DIST_PN) - nTrack;
  dmax = ((ref - 2*ECALL_PCM_LENGTH) - ECALL_PEAK_DIST_PN) + nTrack;

  imin = 0;
  while (posP[imin] < dmin) imin++;
  imax = 8;
  while (posP[imax] > dmax) imax--;

  for (ampP = 0, i = imin; i <= imax; i++) {
    if (corrP[i*npulse+ECALL_NRS_CP] > ECALL_SYNC_THRESHOLD &&
      ampP < corrP[i*npulse+ECALL_NRS_CP])
    {
      ampP = corrP[i*npulse+ECALL_NRS_CP];
      indP = i;
    }
  }

  /* search for negative peak */
  dmin = (ref - 2*ECALL_PCM_LENGTH) - nTrack;
  dmax = (ref - 2*ECALL_PCM_LENGTH) + nTrack;

  imin = 0;
  while (posN[imin] < dmin) imin++;
  imax = 8;
  while (posN[imax] > dmax) imax--;

  for (ampN = 0, i = imin; i <= imax; i++) {
    if (corrN[i*npulse+ECALL_NRS_CP] > ECALL_SYNC_THRESHOLD &&
      ampN < corrN[i*npulse+ECALL_NRS_CP])
    {
      ampN = corrN[i*npulse+ECALL_NRS_CP];
      indN = i;
    }
  }

  /* final tracking decision */
  if (npeaks == 0) {
    if ((
      posN[indN]-posP[indP] == ECALL_PEAK_DIST_PN && ampP > 0 && ampN > 0 &&
      MIN(ampP, ampN) > MAX(ampP, ampN)/3) || (
      ampN > sync->amplitude[0]/2 &&
      ampP < ampN/6))
    {
      sync->delayMem = sync->delay;
      sync->delay    = (int16)((posN[indN] - 4) - ECALL_NRS_CP) + ECALL_PCM_LENGTH;
      sync->flag     = TRUE;

      ECALL_MSG_1( ECALL_HIGH, "[PSAP] sync tracking detects sample slip! [%+1d sample(s)]",
                                (sync->delay)-(sync->delayMem) );
    }
    else if (
      ampP > sync->amplitude[0]/2 &&
      ampN < ampP/6)
    {
      sync->delayMem = sync->delay;
      sync->delay    = (int16)((posP[indP] - 4) - ECALL_NRS_CP) + ECALL_PCM_LENGTH + ECALL_PEAK_DIST_PN;
      sync->flag     = TRUE;

      ECALL_MSG_1( ECALL_HIGH, "[PSAP] sync tracking detects sample slip! [%+1d sample(s)]",
                                (sync->delay)-(sync->delayMem) );
    }
  }

  /* count consecutive unsuccessful tracking events */
  if (npeaks < 2 && sync->flag == FALSE) {
    sync->checkCnt++;
  } else {
    sync->checkCnt = 0;
  }
}

/*============================================================================*/
/* FUNCTION: SyncFilter                                                       */
/*----------------------------------------------------------------------------*/
/* Description: sync filter implementation                                    */
/*                                                                            */
/* InOut:  SyncState*   sync    <-> sync struct                               */
/* In:     const Int16* pcm      -> input frame                               */
/*         Bool         invert   -> port inversion flag                       */
/*----------------------------------------------------------------------------*/
void ecall_sync_filter
(
  ecall_sync_state *sync,
  const int16 *pcm,
  boolean invert
)
{
  int16 j, jmax, nframes;
  int32 i;

  int16 idxmax = ecall_syncIndexPreamble[ECALL_SYNC_IDXLEN-1];
  int16 offset = idxmax + ECALL_PCM_LENGTH + 2*ECALL_NRS_CP;
  int16 npulse = 2*ECALL_NRS_CP+1;
  int32 sgnpcm = (invert == TRUE) ? -1 : 1;

  /* set buffer pointers */
  int32 *posP  = &sync->mem[offset +  0];
  int32 *posN  = &sync->mem[offset +  9];
  int32 *corrP = &sync->mem[offset + 18];
  int32 *corrN = &sync->mem[offset + 27 + 9*2*ECALL_NRS_CP];

  /* sync filter function */
  jmax = ECALL_SYNC_IDXLEN-1;
  for (i = -idxmax; i < ECALL_PCM_LENGTH; i++) {
    while (jmax > 0 && (ecall_syncIndexPreamble[jmax]+i) >= ECALL_PCM_LENGTH) {
      jmax--;
    }
    j = jmax;
    while (((j >= 0) && (ecall_syncIndexPreamble[j]+i) >=0)) {
      sync->mem[i+idxmax+2*ECALL_NRS_CP] +=
        sgnpcm*
        (int32)pcm[ecall_syncIndexPreamble[j]+i]*
        (int32)ecall_syncPulseForm[j%5]*
        (int32)ecall_syncSequence[j/5];
      j--;
    }
  }
  /* shift all peaks by one slot */
  /*lint --e{737}*/
  for (i = 1; i < 9; i++) {
    posP[i-1] = posP[i] - ECALL_PCM_LENGTH;
    posN[i-1] = posN[i] - ECALL_PCM_LENGTH;
    ecall_memscpy ( &corrP[(i-1)*npulse], sizeof(int32)*npulse,
                    &corrP[i*npulse], sizeof(int32)*npulse );
    ecall_memscpy ( &corrN[(i-1)*npulse], sizeof(int32)*npulse,
             &corrN[i*npulse], sizeof(int32)*npulse);
  }
  memset(&corrP[8*npulse], 0, sizeof(int32)*npulse);
  memset(&corrN[8*npulse], 0, sizeof(int32)*npulse);
  
  /* search for maximum and minimum */
  for (i = 0; i < ECALL_PCM_LENGTH; i++) {
    if (corrP[8*npulse+ECALL_NRS_CP] < sync->mem[i+ECALL_NRS_CP]) {
      ecall_memscpy ( &corrP[8*npulse], sizeof(int32)*npulse,
                      &sync->mem[i], sizeof(int32)*npulse);
      posP[8] = i+1;
    }
    if (corrN[8*npulse+ECALL_NRS_CP] > sync->mem[i+ECALL_NRS_CP]) {
      ecall_memscpy ( &corrN[8*npulse], sizeof(int32)*npulse,
                      &sync->mem[i], sizeof(int32)*npulse);
      posN[8] = i+1;
    }
  }
  for (i = 8*npulse; i < 8*npulse+npulse; i++) {
    corrN[i] = -corrN[i];
  }

  /* shift filter memory by one frame */
  nframes = (idxmax+2*ECALL_NRS_CP)/ECALL_PCM_LENGTH;

  for (i = 1; i <= nframes; i++)
  {
    ecall_memscpy ( &sync->mem[(i-1)*ECALL_PCM_LENGTH], sizeof(int32)*ECALL_PCM_LENGTH,
                    &sync->mem[i*ECALL_PCM_LENGTH], sizeof(int32)*ECALL_PCM_LENGTH);
  }
  ecall_memscpy ( &sync->mem[nframes*ECALL_PCM_LENGTH], sizeof(int32)*((idxmax+2*ECALL_NRS_CP)%ECALL_PCM_LENGTH),
                  &sync->mem[(nframes+1)*ECALL_PCM_LENGTH], sizeof(int32)*((idxmax+2*ECALL_NRS_CP)%ECALL_PCM_LENGTH) );
  memset(&sync->mem[idxmax+2*ECALL_NRS_CP], 0, sizeof(int32)*ECALL_PCM_LENGTH);

  /* compute global maximum amplitude */
  sync->amplitude[1] =
    MAX(sync->amplitude[1],
    MAX(corrP[8*npulse+ECALL_NRS_CP], corrN[8*npulse+ECALL_NRS_CP]));
}

/*============================================================================*/
/* UTILITY FUNCTION: SYNCSUBPUT                                               */
/*----------------------------------------------------------------------------*/
/* Description: sync peak evaluation                                          */
/*                                                                            */
/* InOut:  SyncSub*     ssub    <-> sync subsystem                            */
/* In:     const char*  caller   -> modem identification                      */
/*         const Int32* pPos     -> positive peaks positions                  */
/*         const Int32* pCorr    -> positive peaks correlation values         */
/*         const Int32* nPos     -> negative peaks positions                  */
/*         const Int32* nCorr    -> negative peaks correlation values         */
/*----------------------------------------------------------------------------*/
void ecall_sync_sub_put
(
  const ecall_sync_state *sync,
  ecall_sync_sub *ssub
)
{
  int16 i;
  for (i = 0; i <= 2*ECALL_NRS_CP; i++) {
    ssub->shape[i] = sync->shape[i];
  }
  ssub->flag         = sync->flag;
  ssub->delay        = sync->delay;
  ssub->npeaks       = sync->npeaks;
  ssub->amplitude[0] = sync->amplitude[0];
  ssub->amplitude[1] = sync->amplitude[1];
}

/*============================================================================*/
/* UTILITY FUNCTION: ECALL_SYNC_SUB_GET                                       */
/*----------------------------------------------------------------------------*/
/* Description: sync peak evaluation                                          */
/*                                                                            */
/* InOut:  SyncSub*     ssub    <-> sync subsystem                            */
/* In:     const char*  caller   -> modem identification                      */
/*         const Int32* pPos     -> positive peaks positions                  */
/*         const Int32* pCorr    -> positive peaks correlation values         */
/*         const Int32* nPos     -> negative peaks positions                  */
/*         const Int32* nCorr    -> negative peaks correlation values         */
/*----------------------------------------------------------------------------*/
void ecall_sync_sub_get
(
  ecall_sync_state *sync,
  const ecall_sync_sub *ssub
)
{
  int16 i;
  for (i = 0; i <= 2*ECALL_NRS_CP; i++) {
    sync->shape[i] = ssub->shape[i];
  }
  sync->flag         = ssub->flag;
  sync->delay        = ssub->delay;
  sync->npeaks       = ssub->npeaks;
  sync->amplitude[0] = ssub->amplitude[0];
  sync->amplitude[1] = ssub->amplitude[1];
}

/*============================================================================*/
/* UTILITY FUNCTION: ECALL_SYNC_SUB_CPY                                       */
/*----------------------------------------------------------------------------*/
/* Description: sync peak evaluation                                          */
/*                                                                            */
/* InOut:  SyncSub*     ssub    <-> sync subsystem                            */
/* In:     const char*  caller   -> modem identification                      */
/*         const Int32* pPos     -> positive peaks positions                  */
/*         const Int32* pCorr    -> positive peaks correlation values         */
/*         const Int32* nPos     -> negative peaks positions                  */
/*         const Int32* nCorr    -> negative peaks correlation values         */
/*----------------------------------------------------------------------------*/
void ecall_sync_sub_cpy
(
  const ecall_sync_sub *ssubIn,
  ecall_sync_sub *ssubOut
)
{
  int16 i;
  for (i = 0; i <= 2*ECALL_NRS_CP; i++) {
    ssubOut->shape[i] = ssubIn->shape[i];
  }
  ssubOut->flag         = ssubIn->flag;
  ssubOut->delay        = ssubIn->delay;
  ssubOut->npeaks       = ssubIn->npeaks;
  ssubOut->amplitude[0] = ssubIn->amplitude[0];
  ssubOut->amplitude[1] = ssubIn->amplitude[1];
}

/*============================================================================*/
/* UTILITY FUNCTION: ECALL_SYNC_SUB_RUN                                       */
/*----------------------------------------------------------------------------*/
/* Description: sync peak evaluation                                          */
/*                                                                            */
/* InOut:  SyncSub*     ssub    <-> sync subsystem                            */
/* In:     const char*  caller   -> modem identification                      */
/*         const Int32* pPos     -> positive peaks positions                  */
/*         const Int32* pCorr    -> positive peaks correlation values         */
/*         const Int32* nPos     -> negative peaks positions                  */
/*         const Int32* nCorr    -> negative peaks correlation values         */
/*----------------------------------------------------------------------------*/
void ecall_sync_sub_run
(
  ecall_sync_sub *ssub,
  const char *caller,
  const int32 *posP,
  const int32 *corrP,
  const int32 *posN,
  const int32 *corrN
)
{
  int16 i, tgtP, tgtN;
  int16 p[5];

  boolean detP[5];
  boolean detN[5];

  /* initialize */
  int16 npeaksP = 0;
  int16 npeaksN = 0;
  int16 npulse  = 2*ECALL_NRS_CP+1;

  /* reset sync flag for peak detection */
  ssub->flag = FALSE;

  /* frame numbers for peak positions */
  p[0] = ecall_peak_update(posN, corrN, -2*ECALL_PEAK_DIST_PN - ECALL_PEAK_DIST_PP);
  p[1] = ecall_peak_update(posP, corrP, -ECALL_PEAK_DIST_PN - ECALL_PEAK_DIST_PP);
  p[2] = ecall_peak_update(posP, corrP, -ECALL_PEAK_DIST_PN - ECALL_PEAK_DIST_PP/2);
  p[3] = ecall_peak_update(posP, corrP, -ECALL_PEAK_DIST_PN);
  p[4] = 8;

  /* target delays */
  for (i = 0; i < 5; i++) {
    detP[i] = FALSE;
    detN[i] = FALSE;
  }
  tgtP = (int16)(((posP[p[3]] - 4) - ECALL_PCM_LENGTH) - ECALL_NRS_CP) + ECALL_PEAK_DIST_PN;
  tgtN = (int16)(((posN[p[4]] - 4) - ECALL_PCM_LENGTH) - ECALL_NRS_CP);

  /* check peak distances with respect to p[3] */
  if (posP[p[3]]-posP[p[1]] == ECALL_PEAK_DIST_PP &&
    corrP[p[1]*npulse+ECALL_NRS_CP] > ECALL_SYNC_THRESHOLD &&
    corrP[p[3]*npulse+ECALL_NRS_CP] > ECALL_SYNC_THRESHOLD)
  {
    npeaksP = 2;
    detP[1] = detP[3] = TRUE;

    if (posP[p[3]]-posN[p[0]] == ECALL_PEAK_DIST_PN + ECALL_PEAK_DIST_PP &&
      corrN[p[0]*npulse+ECALL_NRS_CP] > ECALL_SYNC_THRESHOLD)
    {
      npeaksP++;
      detP[0] = TRUE;
    }
    if (posP[p[3]]-posP[p[2]] == ECALL_PEAK_DIST_PP/2 &&
      corrP[p[2]*npulse+ECALL_NRS_CP] > ECALL_SYNC_THRESHOLD)
    {
      npeaksP++;
      detP[2] = TRUE;
    }
    if (posN[p[4]]-posP[p[3]] == ECALL_PEAK_DIST_PN &&
      corrN[p[4]*npulse+ECALL_NRS_CP] > ECALL_SYNC_THRESHOLD)
    {
      npeaksP++;
      detP[4] = TRUE;
    }
  }

  /* check peak distances with respect to p[4] */
  if (posN[p[4]]-posN[p[0]] == ECALL_PEAK_DIST_NN &&
    corrN[p[0]*npulse+ECALL_NRS_CP] > ECALL_SYNC_THRESHOLD &&
    corrN[p[4]*npulse+ECALL_NRS_CP] > ECALL_SYNC_THRESHOLD)
  {
    npeaksN = 2;
    detN[0] = detN[4] = TRUE;

    if (posN[p[4]]-posP[p[1]] == ECALL_PEAK_DIST_PN + ECALL_PEAK_DIST_PP &&
      corrP[p[1]*npulse+ECALL_NRS_CP] > ECALL_SYNC_THRESHOLD)
    {
      npeaksN++;
      detN[1] = TRUE;
    }
    if (posN[p[4]]-posP[p[2]] == ECALL_PEAK_DIST_PN + ECALL_PEAK_DIST_PP/2 &&
      corrP[p[2]*npulse+ECALL_NRS_CP] > ECALL_SYNC_THRESHOLD)
    {
      npeaksN++;
      detN[2] = TRUE;
    }
    if (posN[p[4]]-posP[p[3]] == ECALL_PEAK_DIST_PN &&
      corrP[p[3]*npulse+ECALL_NRS_CP] > ECALL_SYNC_THRESHOLD)
    {
      npeaksN++;
      detN[3] = TRUE;
    }
  }

  /* evaluate detection result */
  if (npeaksP < npeaksN) {
    ecall_peak_check(ssub, caller, detN, p,
      corrP, corrN, corrN, p[0], p[4], npeaksN, tgtN);

    if (npeaksP > 0) {
      ecall_peak_check(ssub, caller, detP, p,
        corrP, corrN, corrP, p[1], p[3], npeaksP, tgtP);
    }
  }
  else {
    if (npeaksP > 0) {
      ecall_peak_check(ssub, caller, detP, p,
        corrP, corrN, corrP, p[1], p[3], npeaksP, tgtP);
    }
    if (npeaksN > 0) {
      ecall_peak_check(ssub, caller, detN, p,
        corrP, corrN, corrN, p[0], p[4], npeaksN, tgtN);
    }
  }
}

/*============================================================================*/
/* UTILITY FUNCTION: SyncSubChk                                               */
/*----------------------------------------------------------------------------*/
/* Description: sync peak evaluation for sync check                           */
/*                                                                            */
/* InOut:  SyncSub*     ssub    <-> sync subsystem                            */
/* In:     const char*  caller   -> modem identification                      */
/*         const Int32* posP     -> positive peaks positions                  */
/*         const Int32* corrP    -> positive peaks correlation values         */
/*         const Int32* posN     -> negative peaks positions                  */
/*         const Int32* corrN    -> negative peaks correlation values         */
/*         Int16        delay    -> reference delay                           */
/*----------------------------------------------------------------------------*/
void ecall_sync_sub_chk
(
  ecall_sync_sub *ssub,
  const char *caller,
  const int32 *posP,
  const int32 *corrP,
  const int32 *posN,
  const int32 *corrN,
  int16 delay
)
{
  int16 p[5];
  int16 q[5];
  int16 ref = (delay + 4 + ECALL_PCM_LENGTH + ECALL_NRS_CP - 1)%ECALL_PCM_LENGTH + 1;
  int16 npulse = 2*ECALL_NRS_CP+1;

  /* frame numbers for peak positions */
  q[0] = (ref - 2*ECALL_PEAK_DIST_PN) - ECALL_PEAK_DIST_PP;
  q[1] = (ref - ECALL_PEAK_DIST_PN) - ECALL_PEAK_DIST_PP;
  q[2] = (ref - ECALL_PEAK_DIST_PN) - ECALL_PEAK_DIST_PP/2;
  q[3] = (ref - ECALL_PEAK_DIST_PN);
  q[4] = ref;

  p[0] = 7 + q[0]/ECALL_PCM_LENGTH;
  p[1] = 7 + q[1]/ECALL_PCM_LENGTH;
  p[2] = 7 + q[2]/ECALL_PCM_LENGTH;
  p[3] = 7 + q[3]/ECALL_PCM_LENGTH;
  p[4] = 8;

  ssub->npeaksChk = 0;

  if (posN[p[0]] == q[0] && corrN[p[0]*npulse+ECALL_NRS_CP] > ECALL_SYNC_THRESHOLD) {
    ssub->npeaksChk++;
  }
  if (posP[p[1]] == q[1] && corrP[p[1]*npulse+ECALL_NRS_CP] > ECALL_SYNC_THRESHOLD) {
    ssub->npeaksChk++;
  }
  if (posP[p[2]] == q[2] && corrP[p[2]*npulse+ECALL_NRS_CP] > ECALL_SYNC_THRESHOLD) {
    ssub->npeaksChk++;
  }
  if (posP[p[3]] == q[3] && corrP[p[3]*npulse+ECALL_NRS_CP] > ECALL_SYNC_THRESHOLD) {
    ssub->npeaksChk++;
  }
  if (posN[p[4]] == q[4] && corrN[p[4]*npulse+ECALL_NRS_CP] > ECALL_SYNC_THRESHOLD) {
    ssub->npeaksChk++;
  }
}


/*==========================================================================

  FUNCTION    ECALL_UPDATE_PEAK

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
)
{
  int16 npulse = 2*ECALL_NRS_CP+1;
  int16 i = dist + ECALL_PCM_LENGTH;
  int16 j = dist + 1;
  int16 peak;

  if (pos[7+i/ECALL_PCM_LENGTH] <= i) {
    if (pos[7+j/ECALL_PCM_LENGTH] < j) {
      peak = 7+i/ECALL_PCM_LENGTH;
    } else {
      peak = (corr[npulse*(7+j/ECALL_PCM_LENGTH)+ECALL_NRS_CP] <
        corr[npulse*(7+i/ECALL_PCM_LENGTH)+ECALL_NRS_CP]) ?
        7+i/ECALL_PCM_LENGTH : 7+j/ECALL_PCM_LENGTH;
    }
  } else {
    peak = (pos[7+j/ECALL_PCM_LENGTH] < j) ? 0 : 7+j/ECALL_PCM_LENGTH;
  }
  return peak;
}

/*============================================================================*/
/* UTILITY FUNCTION: PeakCheck                                                */
/*----------------------------------------------------------------------------*/
/* Description: check sync peaks                                              */
/*                                                                            */
/* InOut:  SyncSub*     ssub    <-> sync subsystem                            */
/* In:     const char*  caller   -> modem identification                      */
/*         const Bool*  pdet     -> vector of peak detection flags            */
/*         const Int16* p        -> vector of frame numbers                   */
/*         const Int32* corr(X)  -> vector of correlation values              */
/*         Int16        pos1     -> peak position 1                           */
/*         Int16        pos2     -> peak position 2                           */
/*         Int16        npeaks   -> number of detected peaks                  */
/*         Int16        delay    -> target delay if sync successful           */
/*----------------------------------------------------------------------------*/
/*lint -e704*/
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
)
{
  int16 i, cond1, cond2, jmax = 0, npulse = 2*ECALL_NRS_CP+1;
  int32 atmp, amax;
  int32 ssum = 0;

  /* run only if sync has not yet been successful */
  if (ssub->flag == FALSE) {
    atmp = corr[pos1*npulse+ECALL_NRS_CP]/2 + corr[pos2*npulse+ECALL_NRS_CP]/2;

    if (npeaks > MAX(ssub->npeaks, 3)) {
      ssub->flag = TRUE;
    }
    else {
      amax = MAX(corr[pos2*npulse+ECALL_NRS_CP], corr[pos1*npulse+ECALL_NRS_CP]);
      for (i = 0; i < 9; i++) {
        if (corrP[i*npulse+ECALL_NRS_CP] >= amax) jmax++;
      }
      if (jmax <= 3) {
        cond1 =
          ssub->amplitude[0] < atmp &&
          ssub->amplitude[1]/2 < atmp &&
          ssub->npeaks < npeaks &&
          MAX(corr[pos1*npulse+ECALL_NRS_CP], corr[pos2*npulse+ECALL_NRS_CP])/3 <
          MIN(corr[pos1*npulse+ECALL_NRS_CP], corr[pos2*npulse+ECALL_NRS_CP]);

        cond2 = ssub->amplitude[0] < atmp && npeaks >= MAX(ssub->npeaks, 4);

        if (cond1 || cond2) {
          ssub->flag = TRUE;
        }
      }
    }
    if (ssub->flag == TRUE) {
      /* compute normalized reference pulse shape */
      for (i = 0; i < npulse; i++)
      { 
        ssub->shape[i] =
          pdet[0]*(corrN[p[0]*npulse+i] >> 3) +
          pdet[1]*(corrP[p[1]*npulse+i] >> 3) +
          pdet[2]*(corrP[p[2]*npulse+i] >> 3) +
          pdet[3]*(corrP[p[3]*npulse+i] >> 3) +
          pdet[4]*(corrN[p[4]*npulse+i] >> 3);
      }
      amax = 0;
      for (i = 0; i < npulse; i++) {
        amax += (ssub->shape[i] >> 16)*(ssub->shape[i] >> 16);
      }
      /* avoid division by zero */
      if ((amax = amax >> 12) == 0) {
        amax = 1;
      }
      /*lint -e568*/
      for (i = 0; i < npulse; i++)  
      {
        ssub->shape[i] =
          (ABS(ssub->shape[i] >> 16)*(ssub->shape[i] >> 16))/amax;
        ssum = ssum+ssub->shape[i]/npulse;
      }
      if (ABS(ssum) < 10)
      {
        ssub->flag = FALSE;
      }

      /*lint +e568*/
      ssub->delay        = delay;
      ssub->npeaks       = npeaks;
      ssub->amplitude[0] = atmp;
      ssub->sign         = ssum;
    }
  }
}
/*lint +e704*/
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
/*lint -save -e704 -e702 -e818*/
void ecall_tone_detect
(
  ecall_sync_state *sync,  
  const int16 *pcm
)
{
  int16 i;
  int32 sumSin500 = 0;
  int32 sumCos500 = 0;
  int32 sumSin800 = 0;
  int32 sumCos800 = 0;

  if (sync->memWakeup != NULL) {
    for (i = 0; i < ECALL_NRF_SYNC; i++) {
      sync->memWakeup[2*i]   = sync->memWakeup[2*(i+1)];
      sync->memWakeup[2*i+1] = sync->memWakeup[2*(i+1)+1];
    }
    for (i = 0; i < ECALL_PCM_LENGTH; i++) {
      sumSin500 += (ecall_wakeupSin500[i%16] >> 5)*pcm[i];
      sumCos500 += (ecall_wakeupCos500[i%16] >> 5)*pcm[i];
      sumSin800 += (ecall_wakeupSin800[i%10] >> 5)*pcm[i];
      sumCos800 += (ecall_wakeupCos800[i%10] >> 5)*pcm[i];
    }
    sync->memWakeup[2*ECALL_NRF_SYNC] =
      (sumSin500 >> 18)*(sumSin500 >> 18) +
      (sumCos500 >> 18)*(sumCos500 >> 18);

    sync->memWakeup[2*ECALL_NRF_SYNC+1] =
      (sumSin800 >> 18)*(sumSin800 >> 18) +
      (sumCos800 >> 18)*(sumCos800 >> 18);
  }
}
/*lint -restore*/

#else

void ecall_sync_dummy_func(void);

#endif /* End FEATURE_ECALL_IVS or FEATURE_ECALL_PSAP  */

