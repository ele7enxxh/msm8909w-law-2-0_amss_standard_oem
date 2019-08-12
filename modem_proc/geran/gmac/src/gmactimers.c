/*****************************************************************************
***
*** TITLE
***
*** GPRS MAC TIMERS
***
***
*** DESCRIPTION
***
***  This module provides the access call back function for MAC tmers
***  and the sending to MAC signals to indicated expiry of tmers.
***
***  Plus the associted defintion,set and clear functionality
***
*** EXTERNALIZED FUNCTIONS
***
***
***
*** INITIALIZATION AND SEQUENCING REQUIREMENTS
***
***  none
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
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gmac/src/gmactimers.c#1 $
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 07/20/01   tlx     Created
***
*****************************************************************************/

/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/

#include "geran_variation.h"
#include "customer.h"

#include "ms.h"
#include "err.h"
#include "grlcutil.h"
#include "gmacdef.h"
#include "gmacmac.h"
#include "gmacsig.h"
#include "gmacidle_if.h"
/*event logging changes*/
#include "event_defs.h"
#include "gmaclog.h"
#include "gmactransfer_if.h"
#include "gmacproc.h"
#include "gmacds.h"

/*****************************************************************************
***
***     Private Data Declarations
***
*****************************************************************************/

/*****************************************************************************
***
***     Private Function Prototypes
***
*****************************************************************************/
void mac_timer_callback(const uint32 timer_id);

/*****************************************************************************
***
***     Types, Defines
***
***     
***
*****************************************************************************/

/* The timer id used to in rex_def_timer_ex is 32 bits. Consists of:
   MSB
   octet 4 - not used
   octet 3 - not used
   octet 2 - gas_id
   octet 1 - Timer Id from mac_mac_sig_id_t - range 0x80-0xff
   LSB*/

#define SET_REX_TIMER(timer_id, gas_id) ( (uint32)(timer_id & 0xff) | ((uint32)(gas_id & 0xff) << 8) )

/*************************************************************************
*   timer structures used by REX to link MAC timers into REX's timer queue
**************************************************************************/

/*****************************************************************************
***
***     Private Data
***
*****************************************************************************/
static gmac_timers_t gmac_timers[NUM_GERAN_DATA_SPACES];

/*****************************************************************************
***
***     Functions
***
*****************************************************************************/
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
gmac_timers_t* gmac_get_timers_ptr(gas_id_t gas_id)
{
  gmac_timers_t *timers_ptr = &gmac_timers[0];

   /* validate the GAS ID */
  if (gas_id <= GERAN_ACCESS_STRATUM_ID_MAX)
  {
    timers_ptr = &gmac_timers[GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id)];
  }
  else
  {    
    MSG_GERAN_ERROR_1_G("Timers bad gas_id:%d",gas_id);
  }

  return timers_ptr;
}

#ifdef PERLUTF
#error code not present
#endif /*PERLUTF*/



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
void gmac_reset_stop_all_timers( gas_id_t gas_id )
{
  uint8 i = 0;
  gmac_timers_t* mac_timers_ptr = gmac_get_timers_ptr(gas_id);
  rex_timer_type* mac_timers[7] =
  {
     &mac_timers_ptr->timer_t3164,
     &mac_timers_ptr->timer_t3166,
     &mac_timers_ptr->timer_t3168,
     &mac_timers_ptr->timer_t3172,
     &mac_timers_ptr->timer_t3180,
     &mac_timers_ptr->timer_t3184,
     &mac_timers_ptr->timer_t3164_transfer };

  for ( i=0; i<ARR_SIZE(mac_timers); i++)
  {
    if( rex_get_timer(mac_timers[i]) != 0 )
    {
      (void)rex_clr_timer(mac_timers[i]);
    }
  }

#ifdef FEATURE_GSM_EDTM
  if( rex_get_timer(&mac_timers_ptr->timer_t3196) != 0 )
  {
    (void)rex_clr_timer(&mac_timers_ptr->timer_t3196);
  }

  if( rex_get_timer(&mac_timers_ptr->edtm_pkt_cs_req_retry_timer) != 0 )
  {
    (void)rex_clr_timer(&mac_timers_ptr->edtm_pkt_cs_req_retry_timer);
  }
#endif /*FEATURE_GSM_EDTM*/

}

/*===========================================================================
===
===  FUNCTION      mac_timer_callback()
===
===  DESCRIPTION
===    MAC's timer call back function . On timer expiry the timer ID is
===    passed back to the call function. It's this ID thats used by MAC
===    to sends itself a timer signal.
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
void mac_timer_callback(const uint32 timer_id)
{
  mac_int_sig_t timer_msg_ptr;

  gas_id_t gas_id = (gas_id_t)((timer_id & 0x0000ff00UL) >> 8);

  /*event logging changes*/
  mac_log_timer_event(timer_id, gas_id);

  /* copy the expired timer (timer_id) into the timer signal */
  
  
  memset(&timer_msg_ptr,0,sizeof(mac_int_sig_t));
  
  timer_msg_ptr.sig_id = (mac_mac_sig_id_t)(timer_id & 0x000000ffUL);
  timer_msg_ptr.task_id = MS_MAC_MAC;

  /* use internal access function to send MAC_TIMER_EXPIRED signal */
  gprs_mac_mac_acc_func(&timer_msg_ptr,gas_id);
}

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
void mac_timer_init(gas_id_t gas_id)
{
  gmac_timers_t* mac_timers_ptr = gmac_get_timers_ptr(gas_id);
  uint8 i;

  rex_def_timer_ex(&mac_timers_ptr->timer_t3164,mac_timer_callback, SET_REX_TIMER(MAC_TIMER_T3164,gas_id));
  rex_def_timer_ex(&mac_timers_ptr->timer_t3166,mac_timer_callback, SET_REX_TIMER(MAC_TIMER_T3166,gas_id));
  rex_def_timer_ex(&mac_timers_ptr->timer_t3168,mac_timer_callback, SET_REX_TIMER(MAC_TIMER_T3168,gas_id));
  rex_def_timer_ex(&mac_timers_ptr->timer_t3172,mac_timer_callback, SET_REX_TIMER(MAC_TIMER_T3172,gas_id));
  rex_def_timer_ex(&mac_timers_ptr->timer_t3180,mac_timer_callback, SET_REX_TIMER(MAC_TIMER_T3180,gas_id));
  rex_def_timer_ex(&mac_timers_ptr->timer_t3184,mac_timer_callback, SET_REX_TIMER(MAC_TIMER_T3184,gas_id));
#ifdef FEATURE_GPRS_PS_HANDOVER
  rex_def_timer_ex(&mac_timers_ptr->timer_t3216,mac_timer_callback, SET_REX_TIMER(MAC_TIMER_T3216,gas_id));
  rex_def_timer_ex(&mac_timers_ptr->timer_t3218,mac_timer_callback, SET_REX_TIMER(MAC_TIMER_T3218,gas_id));
#endif /*FEATURE_GPRS_PS_HANDOVER  */
  rex_def_timer_ex(&mac_timers_ptr->timer_t3164_transfer,mac_timer_callback, SET_REX_TIMER(MAC_TIMER_T3164_TRANSFER,gas_id));

  /*t3200 timer used to control segmented DL control messages, 8 in all*/
  for(i=0; i<8; i++)
  {
    rex_def_timer_ex(&mac_timers_ptr->gmac_t3200_array[i].timer_t3200,mac_timer_callback, i + SET_REX_TIMER(MAC_TIMER_T3200_1,gas_id));
  }

#ifdef FEATURE_GSM_EDTM
  rex_def_timer_ex(&mac_timers_ptr->timer_t3196,mac_timer_callback,SET_REX_TIMER(MAC_TIMER_T3196,gas_id));
  rex_def_timer_ex(&mac_timers_ptr->edtm_pkt_cs_req_retry_timer,mac_timer_callback,SET_REX_TIMER(MAC_TIMER_PKT_CS_REQ_RETRY,gas_id));
#endif /*FEATURE_GSM_EDTM*/
}


/*===========================================================================
===
===  FUNCTION      gmac_start_t3180()
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
void gmac_start_t3180(gas_id_t gas_id)
{
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);
  gmac_timers_t* mac_timers_ptr = gmac_get_timers_ptr(gas_id);

  /*inhibit t3180 if in EGPRS SRB test mode*/
  if ( global_data_ptr->current_test_mode != TEST_MODE_SRB )
  {
    MSG_GERAN_LOW_0_G("MAC Timer start T3180");
    (void)rex_set_timer(&mac_timers_ptr->timer_t3180, T3180_DURATION);
  }
  else
  {
    MSG_GERAN_HIGH_0_G("GMAC T3180 Timer inhibited for SRB mode");
  }
}

/*===========================================================================
===
===  FUNCTION      gmac_stop_t3180()
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
void gmac_stop_t3180(gas_id_t gas_id)
{
  gmac_timers_t* mac_timers_ptr = gmac_get_timers_ptr(gas_id);

  MSG_GERAN_LOW_0_G("MAC Timer stop T3180");
  (void)rex_clr_timer(&mac_timers_ptr->timer_t3180);
}

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
void gmac_start_t3166(gas_id_t gas_id)
{
  gmac_timers_t* mac_timers_ptr = gmac_get_timers_ptr(gas_id);

  if (gmac_idle_get_idle_state(gas_id) == UL_1P_CONTENTION_RES)
  {
     MSG_GERAN_MED_0_G("MAC 1p Timer start T3166");
     (void)rex_set_timer(&mac_timers_ptr->timer_t3166, T3166_DURATION);
  }
}

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
void gmac_stop_t3166(gas_id_t gas_id)
{
  gmac_timers_t* mac_timers_ptr = gmac_get_timers_ptr(gas_id);

  MSG_GERAN_LOW_0_G("MAC Timer stopped T3166");
  (void)rex_clr_timer(&mac_timers_ptr->timer_t3166);
}

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
void gmac_start_t3164(gas_id_t gas_id)
{
  gmac_timers_t* mac_timers_ptr = gmac_get_timers_ptr(gas_id);

  MSG_GERAN_LOW_0_G("MAC Timer start T3164");
  (void)rex_set_timer(&mac_timers_ptr->timer_t3164, T3164_DURATION);
}

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
void gmac_stop_t3164(gas_id_t gas_id)
{
  gmac_timers_t* mac_timers_ptr = gmac_get_timers_ptr(gas_id);

  MSG_GERAN_LOW_0_G("MAC Timer stopped T3164");
  (void)rex_clr_timer(&mac_timers_ptr->timer_t3164);
}

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
/*lint -save -e715 Quieten lint */
void gmac_start_t3168(uint8 period, gas_id_t gas_id)
{
  gmac_timers_t* mac_timers_ptr = gmac_get_timers_ptr(gas_id);

  MSG_GERAN_LOW_0_G("MAC Timer start T3168");
  (void)rex_set_timer(&mac_timers_ptr->timer_t3168, T3168_DURATION(period));
}
/*lint -restore */

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
void gmac_stop_t3168(gas_id_t gas_id)
{
  gmac_timers_t* mac_timers_ptr = gmac_get_timers_ptr(gas_id);

  MSG_GERAN_LOW_0_G("MAC Timer stop T3168");
  (void)rex_clr_timer(&mac_timers_ptr->timer_t3168);
}

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
/*lint -save -e715 Quieten lint */
void gmac_start_t3172(uint8 wait_ind, uint8 time_base, gas_id_t gas_id)
{
  gmac_timers_t* mac_timers_ptr = gmac_get_timers_ptr(gas_id);

  MSG_GERAN_LOW_0_G("MAC Timer start T3172");
  (void)rex_set_timer(&mac_timers_ptr->timer_t3172, T3172_DURATION(wait_ind, time_base));
}
/*lint -restore */

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
void gmac_stop_t3172(gas_id_t gas_id)
{
  gmac_timers_t* mac_timers_ptr = gmac_get_timers_ptr(gas_id);

  MSG_GERAN_LOW_0_G("MAC Timer stop T3172");
  (void)rex_clr_timer(&mac_timers_ptr->timer_t3172);
}

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
/*lint -save -e715 Quieten lint */
void gmac_start_t3164_transfer(gas_id_t gas_id)
{
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);
  gmac_timers_t* mac_timers_ptr = gmac_get_timers_ptr(gas_id);

  /*inhibit t3164 if in EGPRS SRB test mode*/
  if ( global_data_ptr->current_test_mode != TEST_MODE_SRB )
  {
    MSG_GERAN_LOW_0_G("MAC Timer start T3164_transfer");
    (void)rex_set_timer(&mac_timers_ptr->timer_t3164_transfer, T3164_TRANSFER_DURATION );
  }
  else
  {
    MSG_GERAN_HIGH_0_G("GMAC T3164 Timer inhibited for SRB mode");
  }
}
/*lint -restore */

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
void gmac_stop_t3164_transfer(gas_id_t gas_id)
{
  gmac_timers_t* mac_timers_ptr = gmac_get_timers_ptr(gas_id);

  MSG_GERAN_LOW_0_G("MAC Timer stop T3164_transfer");
  (void)rex_clr_timer(&mac_timers_ptr->timer_t3164_transfer);
}


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
void gmac_start_t3196(gas_id_t gas_id)
{
  gmac_timers_t* mac_timers_ptr = gmac_get_timers_ptr(gas_id);

  MSG_GERAN_MED_0_G("MAC Timer start T3196");
  (void)rex_set_timer(&mac_timers_ptr->timer_t3196, T3196_DURATION);
}

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
void gmac_stop_t3196(gas_id_t gas_id)
{
  gmac_timers_t* mac_timers_ptr = gmac_get_timers_ptr(gas_id);

  MSG_GERAN_MED_0_G("MAC Timer stop T3196");
  (void)rex_clr_timer(&mac_timers_ptr->timer_t3196);
}

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
void gmac_start_edtm_pkt_cs_req_retry_timer(gas_id_t gas_id)
{
  gmac_timers_t* mac_timers_ptr = gmac_get_timers_ptr(gas_id);

  MSG_GERAN_MED_0_G("MAC Timer start EDTM MAC_TIMER_PKT_CS_REQ_RETRY");
  (void)rex_set_timer(&mac_timers_ptr->edtm_pkt_cs_req_retry_timer, PKT_CS_REQ_RETRY);
}

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
void gmac_stop_edtm_pkt_cs_req_retry_timer(gas_id_t gas_id)
{
  gmac_timers_t* mac_timers_ptr = gmac_get_timers_ptr(gas_id);

  MSG_GERAN_MED_0_G("MAC Timer stop EDTM MAC_TIMER_PKT_CS_REQ_RETRY");
  (void)rex_clr_timer(&mac_timers_ptr->edtm_pkt_cs_req_retry_timer);
}

#endif /*FEATURE_GSM_EDTM*/

#ifdef FEATURE_GPRS_PS_HANDOVER
/*===========================================================================
===
===  FUNCTION      gmac_start_t3216()
===
===  DESCRIPTION
===  Start T3216
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
void gmac_start_t3216(gas_id_t gas_id)
{
  gmac_timers_t* mac_timers_ptr = gmac_get_timers_ptr(gas_id);

  MSG_GERAN_MED_0_G("MAC Timer start T3216");
  (void)rex_set_timer(&mac_timers_ptr->timer_t3216, T3216_DURATION);
}

/*===========================================================================
===
===  FUNCTION      gmac_stop_t3216()
===
===  DESCRIPTION
===  Stop T3216
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
void gmac_stop_t3216(gas_id_t gas_id)
{
  gmac_timers_t* mac_timers_ptr = gmac_get_timers_ptr(gas_id);

  MSG_GERAN_MED_0_G("MAC Timer stop T3216");
  (void)rex_clr_timer(&mac_timers_ptr->timer_t3216);
}

/*===========================================================================
===
===  FUNCTION      gmac_start_t3218()
===
===  DESCRIPTION
===  Start T3218
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
void gmac_start_t3218(gas_id_t gas_id)
{
  gmac_timers_t* mac_timers_ptr = gmac_get_timers_ptr(gas_id);

  MSG_GERAN_MED_0_G("MAC Timer start T3218");
  (void)rex_set_timer(&mac_timers_ptr->timer_t3218, T3218_DURATION);
}

/*===========================================================================
===
===  FUNCTION      gmac_stop_t3218()
===
===  DESCRIPTION
===  Stop T3218
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
void gmac_stop_t3218(gas_id_t gas_id)
{
  gmac_timers_t* mac_timers_ptr = gmac_get_timers_ptr(gas_id);

  MSG_GERAN_MED_0_G("MAC Timer stop T3218");
  (void)rex_clr_timer(&mac_timers_ptr->timer_t3218);
}
#endif /*FEATURE_GPRS_PS_HANDOVER*/
/*** EOF: don't remove! ***/
