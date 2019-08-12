/*****************************************************************************
***
*** TITLE
***
***  GPRS MAC TIMER FUNCTION(S) DEFINITION
***
***
*** DESCRIPTION
***
***  This file contains definitions of signals and data structures used
***  for the prepose of controling MAC timers and sending MAC timer signals to MAC.
***
***
*** Copyright (c) 2001-2013 Qualcomm Technologies, Inc.
*** All Rights Reserved. QUALCOMM Proprietary.
***
*** Export of this technology or software is regulated by the U.S.
*** Government. Diversion contrary to U.S. law prohibited.
***
*****************************************************************************/


/*****************************************************************************
***
***                        EDIT HISTORY FOR MODULE
***
***  This section contains comments describing changes made to the module.
***  Notice that changes are listed in reverse chronological order.
***
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gmac/inc/gmactimers.h#1 $
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 10-04-01   tjw     Add customer.h and comdef.h to all SU API files
*** 10-03-30   tjw     Explicitly include dependent headers to make SU API files
***                    stand alone
*** 07/20/01   tlx     Created

*****************************************************************************/

#ifndef INC_GMACTIMERS_H
#define INC_GMACTIMERS_H

/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/
#include "geran_variation.h"
#ifndef CUSTOMER_H
  #include "customer.h"
#endif
#ifndef COMDEF_H
  #include "comdef.h"
#endif


#include "rex.h"
#include "gmacmacsig.h"
#include "geran_multi_sim.h"

/*****************************************************************************
***
***     Public Type Definitions
***
*****************************************************************************/

/*************************************************************************
*   signal struct for t3200 timer signal
**************************************************************************/
typedef struct
{
  rex_timer_type  timer_t3200;
}t3200_t;

/*****************************************************************************
***
***     Public Defines & Constants
***
*****************************************************************************/

/*************************************************************************
*   Mac Timer durations .see 4.60 chapter 13.1
**************************************************************************/

#define T3162_DURATION   5000   /* 5 secs*/
#define T3164_DURATION   5000   /* 5 secs*/
#define T3166_DURATION   5000   /* 5 secs*/
#define T3180_DURATION   5000   /* 5 secs*/
#define T3184_DURATION   5000   /* 5 secs*/
#define T3186_DURATION   5000   /* 5 secs*/
#define T3164_TRANSFER_DURATION   5000   /* 5 secs*/

#ifdef FEATURE_GSM_EDTM
#define T3196_DURATION   1500 /*1.5 secs*/
#define PKT_CS_REQ_RETRY 750 /*750 ms*/
#endif /*FEATURE_GSM_EDTM*/
#ifdef FEATURE_GPRS_PS_HANDOVER
#define T3216_DURATION   1000 /*1 secs*/
#define T3218_DURATION   1000 /*1 sec*/
#endif /*FEATURE_GPRS_PS_HANDOVER*/

/* value of t3168 is received in PSI 1 and PSI 13 as part of the GPRS cell option IE*/
/* t is the gprs cell option value of T3168                                         */
/* the unit of time is in millisecs                                                 */
#define T3168_DURATION(t)   ( ((t) + 1) * 500)

/* value of t3172 is calculated from WAIT_INDICATION and WAIT_INDICATION_SIZE       */
/* recived in Packet Access Reject message                                          */
/* the unit of time is in seconds or millisecs depending onWAIT_INDICATION_SIZE     */
/* w is WAIT_INDICATION                                                                                         */
/* t is the unit of time ie seconds or millseconds                                  */
#define T3172_DURATION(w,t)  ((t)==0 ? (w)*1000 : (w)*20)


/* value of T3200 is calculated from BS_CV_MAX or DRX_PERIOD. BS_CV_MAX is recived  */
/* in PSI1 and PSI13 as part of the GPRS cell option IE and is used when in non-DRX */
/* mode.  DRX_PERIOD is a fixed value abtained via GMM and is used in DRX mode      */
/* see 4.60 chapter 9.1.11b                                                         */
/* the unit of time is the block period (4 * tdma frames = 18.46 ms)                */
#define T3200_DURATION(t)   (int)( (t) * 4 * 18.46 )

typedef struct
{
  rex_timer_type  timer_t3164;
  rex_timer_type  timer_t3166;
  rex_timer_type  timer_t3168;
  rex_timer_type  timer_t3184;

#ifdef FEATURE_GSM_EDTM
  rex_timer_type timer_t3196; /*EDTM Packet CS Request timer*/
  rex_timer_type edtm_pkt_cs_req_retry_timer; /*Pkt CS Retry timer, used when T3196 is running*/
#endif /*FEATURE_GSM_EDTM*/
#ifdef FEATURE_GPRS_PS_HANDOVER
  rex_timer_type timer_t3216; 
  rex_timer_type timer_t3218; 
#endif /*FEATURE_GPRS_PS_HANDOVER*/
  rex_timer_type  timer_t3172;
  rex_timer_type  timer_t3164_transfer;
  rex_timer_type  timer_t3180;

  /* array of t3200 timers */
  t3200_t gmac_t3200_array[8];
}gmac_timers_t;

/*****************************************************************************
***
***     Public Data References
***
*****************************************************************************/

/*****************************************************************************
***
***     Public Functions
***
*****************************************************************************/

/*===========================================================================
===
===  FUNCTION      mac_timer_init()
===
===  DESCRIPTION
===
===  Initiates all MAC's timers
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void mac_timer_init(gas_id_t gas_id);

#ifdef FEATURE_GSM_EDTM

/*===========================================================================
===
===  FUNCTION      gmac_start_t3196()
===
===  DESCRIPTION
===  Start T3196
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_start_t3196(gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION      gmac_stop_t3196()
===
===  DESCRIPTION
===  Stop T3196
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_stop_t3196(gas_id_t gas_id);


/*===========================================================================
===
===  FUNCTION      gmac_start_edtm_pkt_cs_req_retry_timer()
===
===  DESCRIPTION
===  Start MAC_TIMER_PKT_CS_REQ_RETRY
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_start_edtm_pkt_cs_req_retry_timer(gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION      gmac_stop_edtm_pkt_cs_req_retry_timer()
===
===  DESCRIPTION
===  Stop MAC_TIMER_PKT_CS_REQ_RETRY
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_stop_edtm_pkt_cs_req_retry_timer(gas_id_t gas_id);

#endif /*FEATURE_GSM_EDTM*/
#ifdef FEATURE_GPRS_PS_HANDOVER
/*===========================================================================
===
===  FUNCTION      gmac_start_t3216()
===
===  DESCRIPTION
===  Start T3196
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_start_t3216(gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION      gmac_stop_t3216()
===
===  DESCRIPTION
===  Stop T3196
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_stop_t3216(gas_id_t gas_id);
/*===========================================================================
===
===  FUNCTION      gmac_start_t3218()
===
===  DESCRIPTION
===  Start T3196
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_start_t3218(gas_id_t gas_id);
/*===========================================================================
===
===  FUNCTION      gmac_stop_t3218()
===
===  DESCRIPTION
===  Stop T3196
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_stop_t3218(gas_id_t gas_id);
#endif /*FEATURE_GPRS_PS_HANDOVER*/

/*===========================================================================
===
===  FUNCTION      gmac_start_t3180()
===
===  DESCRIPTION
===
===  Start T3184
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_start_t3180(gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION      gmac_stop_t3180()
===
===  DESCRIPTION
===
===  Stop T3184
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_stop_t3180(gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION      gmac_start_t3186()
===
===  DESCRIPTION
===
===  Start T3186
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_start_t3186(gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION      gmac_stop_t3186()
===
===  DESCRIPTION
===  Stop T3186
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_stop_t3186(gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION      gmac_start_t3170()
===
===  DESCRIPTION
===  Start T3170
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_start_t3170(uint32 timer_val, gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION      gmac_stop_t3170()
===
===  DESCRIPTION
===  Stop T3170
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_stop_t3170(gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION      gmac_start_t3166()
===
===  DESCRIPTION
===  Start T3166  :- started when the first rlc data block is sent for a fixed
===                  ul tbf est.
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_start_t3166(gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION      gmac_stop_t3166()
===
===  DESCRIPTION
===  Stop T3166   :- stopped On receipt of a PUAN
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_stop_t3166(gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION      gmac_start_t3162()
===
===  DESCRIPTION
===  Start T3162    :- Wait for DL msg after reception of PQN msg
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_start_t3162(gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION      gmac_stop_t3162()
===
===  DESCRIPTION
===  Stop T3164   :- Wait for DL msg after reception of PQN msg.
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_stop_t3162(gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION      gmac_start_t3164()
===
===  DESCRIPTION
===  Start T3164    :- for dynamic ul 1_ph est, started when pua received
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_start_t3164(gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION      gmac_stop_t3164()
===
===  DESCRIPTION
===  Stop T3164   :- stopped  on the sending of the first RLC data block.
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_stop_t3164(gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION      gmac_start_t3168()
===
===  DESCRIPTION
===  Start T3184
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_start_t3168(uint8 period, gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION      gmac_stop_t3168()
===
===  DESCRIPTION
===  Stop T3184
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_stop_t3168(gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION      gmac_start_t3172()
===
===  DESCRIPTION
===  Start T3184
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_start_t3172(uint8 wait_ind, uint8 time_base, gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION      gmac_stop_t3168()
===
===  DESCRIPTION
===  Stop T3168
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_stop_t3172(gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION      gmac_start_t3164_transfer()
===
===  DESCRIPTION
===  Start T3164_transfer
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_start_t3164_transfer(gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION      gmac_stop_t3164_transfer()
===
===  DESCRIPTION
===  Stop T3164_transfer
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_stop_t3164_transfer(gas_id_t gas_id);


/*===========================================================================
===
===  FUNCTION    gmac_reset_stop_all_timers()
===
===  DESCRIPTION
===  Used for Lower layer resets.  Stops all running timers.
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_reset_stop_all_timers( gas_id_t gas_id );

/*===========================================================================
===
===  FUNCTION    gmac_get_timers_ptr()
===
===  DESCRIPTION
===  Returns a pointer to timers data space using the current gas_id
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===    gmac_timers_t*
=== 
===  SIDE EFFECTS
===
===
===========================================================================*/
gmac_timers_t* gmac_get_timers_ptr(gas_id_t gas_id);

#endif /* INC_GMACTIMERS_H */
/*** EOF: don't remove! ***/
