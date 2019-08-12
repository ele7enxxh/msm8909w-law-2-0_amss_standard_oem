/*===========================================================================

       H D R SIGNALING ADAPTATION P R O T O C O L TRANSMIT MODULE


DESCRIPTION
  This file implements the Signaling Adaptation Protocol Transmit module

EXTERNALIZED FUNCTIONS (Global)

EXTERNALIZED FUNCTIONS (Regional)

INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2011 - 2015 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/src/hdrsaptx.c#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
12/14/15   vko     Fixed KW issues 
07/09/14   cnx     Support NV refresh without reset.
12/10/13   sat     Changed memcpy() to assignment or memscpy() (safer version).
07/08/13   vko     Ignore L2 ACK Failure, when tunnel is not enabled
05/09/13   wsh     Fixed sending multiple UATI, not sending RTC msg in tunnel 
05/06/13   mbs     Fixed compiler errors for BOLT build
10/25/12   wsh     Do not flood NAS during TAU/SRB2 unavailability
09/11/12   cnx     Handle TX and RX messages during active handoff
04/24/12   cnx     Fixed compiler warnings.
03/21/12   cnx     Resolved TCB compiler errors for Dime. 
03/20/12   cnx     Resolved TCB compiler errors for Dime. 
02/13/12   cnx     Sanity check before calling hdrpcp_packet_failed().
01/05/12   sju     Check trans id when SAPTX receives ACK for UL messages.
11/08/11   cnx     Fixed compiler warnings.
10/28/11   pba     Optimized handoff phase 2 support
10/10/11   cnx     Fixed session token bugs in SAP header.
04/21/11   sju     Added L2 ack; added send rtc msg
03/31/11   cnx     Added hdrsaptx_add_header()
03/14/11   sju     Created module

===========================================================================*/

/*===========================================================================

                    INCLUDE  FILES  FOR  MODULE

===========================================================================*/
#include "hdr_variation.h"
#include "comdef.h"
#include "dsm.h"

#ifdef FEATURE_MEMCPY_REMOVAL
#include "stringl.h"
#endif /* FEATURE_MEMCPY_REMOVAL */

#include "hdrhai.h"
#include "task.h"
#include "err.h"
#include "hdrbit.h"
#include "amssassert.h"
#include "hdrtrace.h"
#include "IxErrno.h"
#include "hdrdebug.h"
#include "hdrutil.h"

#ifdef FEATURE_LTE_TO_HDR_OH
#include "hdrsaptx.h"
#include "hdrsap.h"
#include "hdrsaps.h"
#include "hdramp.h"
#include "hdrscp.h"
#include "hdrind.h"
#include "hdrtx.h"
#include "hdrpcp.h"
#include "lte_nas_common.h"

/* <EJECT> */
/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/*===========================================================================

             DEFINITIONS  FROM  THE  HDR  AIR  INTERFACE  SPEC

===========================================================================*/

/* Address of the TCB for the task in which this protocol runs */
#define HDRSAPTX_TASK_PTR hdrtx_tcb_ptr


/*===========================================================================

                        DATA  DECLARATIONS

===========================================================================*/

/*  */
#define HDRSAPTX_GET_PKT_TIME_MSEC  26

#define HDRSAPTX_MAX_PAYLOAD_SIZE_BYTES HDRPCP_FMT_B_MAX_PKT_SIZE 

/* maximum size  of rtc tx status table
 * for now, use the same number as HDRPCP_MAX_MARQ_BUFFERS */
#define HDRSAPTX_MAX_TX_STATUS_BUFFERS  13 

/* Maximum length of a SAP header */
#define HDRSAP_MAX_HEADER_LEN       7

#define HDRSAPTX_NUM_CARRIERS   1
#define HDRSAPTX_DEF_CARRIER_ID 0
#define HDRSAPTX_DEF_COMBO_ID   0

/* SAP header field macros. */
#define HDRSAP_CLR_HEADER( ptr )\
          ( *((byte*)(ptr)  ) = 0, \
            *((byte*)(ptr)+1) = 0, \
            *((byte*)(ptr)+2) = 0, \
            *((byte*)(ptr)+3) = 0, \
            *((byte*)(ptr)+4) = 0, \
            *((byte*)(ptr)+5) = 0, \
            *((byte*)(ptr)+6) = 0  )

#define HDRSAP_SET_PKT_SAP_STATE( ptr )\
          ( *(byte*)(ptr) |= 0x80 )

#define HDRSAP_SET_PKT_CONFIG_TOKEN( ptr, var )\
          ( *((byte*)(ptr)) |= (byte)((var) >> 9) & 0x7f, \
            *((byte*)(ptr)+1) |= (byte)((var) >> 1), \
            *((byte*)(ptr)+2) |= (byte)((var) << 7 ) & 0x80 )

#define HDRSAP_SET_PKT_SIZE_BIT( ptr, offset )\
          ( *((byte*)(ptr)+(offset)) |= 0x40 )

#define HDRSAP_SET_PKT_ATI_TYPE( ptr, var, offset )\
          ( *((byte*)(ptr)+(offset)) |= ((byte)(var) << 4) & 0x30 )

#define HDRSAP_SET_PKT_ATI_VAL( ptr, var, offset )\
          ( *((byte*)(ptr)+(offset)) |= (byte)((var) >> 28) & 0x0f, \
            *((byte*)(ptr)+1+(offset)) |= (byte)((var) >> 20), \
            *((byte*)(ptr)+2+(offset)) |= (byte)((var) >> 12), \
            *((byte*)(ptr)+3+(offset)) |= (byte)((var) >> 4), \
            *((byte*)(ptr)+4+(offset)) |= (byte)((var) << 4) & 0xf0 )

#define HDRSAPTX_CON_CLOSE_TIME_MS       5120

typedef struct
{
  boolean is_ac_pkt;        /* TRUE if this entry is used for a AC msg */                
  boolean awaiting_l2_ack;  /* TRUE means this buffer is used for tunnel msg*/                
  uint32 trans_id;          /* trans_id for the tunnel msg */
  hdrpcp_pkt_handle_type pkt_handle; /* pkt handle for the tunnel msg*/
  uint8 age;           /* used to find out the oldest entry */
}hdrsaptx_tx_status_type;

static struct
{
  dsm_item_type* tx_pkt_ptr;  /* Packet dsm item chain pointer.*/
  boolean is_fmt_b;           /* Is this a format B packet */
  uint16  elapsed_frames;     /* Elapsed time, in frames.( 26.66..ms) */
  rex_timer_type get_pkt_timer; /* Get packet timer */
  hdrsaptx_frame_cb_type              frame_timer_cb;
  hdrsaptx_tx_status_type tx_status[HDRSAPTX_MAX_TX_STATUS_BUFFERS];
  uint8 tx_status_cnt;
  uint8 verbose_debug;

  rex_timer_type con_close_timer; /* timer to close SAP */
} hdrsaptx;

/* <EJECT> */
/*===========================================================================

                      FUNCTION  DEFINITIONS

===========================================================================*/

/*===========================================================================
FUNCTION HDRSAPTX_TIMER_CB                                 INTERNAL

DESCRIPTION
  This function is the timer callback function. 

DEPENDENCIES
  None.

PARAMETERS
  timer_id - The ID of the timer that has expired

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrsaptx_timer_cb
(
  uint32 timer_id
)
{
  /* queue the timer to RMAC protocol */
  hdrtx_queue_timer(HDRHAI_SAP_TX_MODULE, timer_id);

  return;

} /* hdrsaptx_timer_cb */

/* EJECT */
/*===========================================================================

FUNCTION HDRSAPTX_CLEAR_TX_STATUS_TABLE

DESCRIPTION
  This function clears all entries in tx_status table.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void hdrsaptx_clear_tx_status_table(void)
{
  HDR_MSG_PROT( MSG_LEGACY_MED, "TX status table cleared" ); 
  memset( &hdrsaptx.tx_status[0], 0, 
          sizeof(hdrsaptx_tx_status_type) * HDRSAPTX_MAX_TX_STATUS_BUFFERS );
  hdrsaptx.tx_status_cnt = 0;
  rex_clr_timer( &hdrsaptx.con_close_timer );
} /* hdrsaptx_clear_tx_status_table() */

/* EJECT */
/*===========================================================================
FUNCTION HDRSAPTX_POWERUP_INIT

DESCRIPTION
  This function initializes the HDRSAPTX .
  
DEPENDENCIES
  This function needs to be called after hdrplog_powerup_init() to have 
  correct verbose debug setting.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
  
===========================================================================*/
void hdrsaptx_powerup_init ( void )
{

  HDR_MSG_PROT(MSG_LEGACY_MED, "Powerup init"); 
  hdrsaptx.elapsed_frames = 0;
  hdrsaptx.frame_timer_cb = NULL;

  if ( hdrmc_is_first_init() )
  {
    rex_def_timer_ex( &hdrsaptx.get_pkt_timer,
                      hdrsaptx_timer_cb,
                      HDRSAPTX_GET_PKT_TIMER);

    rex_def_timer_ex( &hdrsaptx.con_close_timer,
                      hdrsaptx_timer_cb,
                      HDRSAPTX_CON_CLOSE_TIMER);
  }

  hdrsaptx_clear_tx_status_table();
}
/* EJECT */
/*===========================================================================
FUNCTION HDRSAPTX_SET_VERBOSE_DEBUG

DESCRIPTION
  This function initializes the SAPTX verbose debug setting .
  
DEPENDENCIES
  None.

PARAMETERS
  verbose_debug_setting - current verbose debug setting.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
  
===========================================================================*/
void hdrsaptx_set_verbose_debug 
( 
  uint8 verbose_debug_setting 
)
{
  hdrsaptx.verbose_debug = verbose_debug_setting;
} /* hdrsaptx_set_verbose_debug() */

/* EJECT */
/*===========================================================================
FUNCTION HDRSAPTX_INIT                                               EXTERNAL

DESCRIPTION
  This function initializes the protocol prior to use.  It should be called
  once upon system startup.  

DEPENDENCIES
  Should be run in HDRTx context 
  Should be run AFTER hdrsaptx_powerup_init().

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrsaptx_init( void )
{

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
 /* Empty  now */
}

/* EJECT */
/*===========================================================================

FUNCTION HDRSAPTX_GET_PENDING_TX_CNT

DESCRIPTION
  This function returns the number of TXed UL msg pending confirmation
  or failure reply.

DEPENDENCIES
  None

PARAMETERS
  None
 
RETURN VALUE
  Number of pending TXed UL msg 

SIDE EFFECTS
  None
===========================================================================*/
uint8 hdrsaptx_get_pending_tx_cnt( void )
{
  return hdrsaptx.tx_status_cnt;
}

/* EJECT */
/*===========================================================================

FUNCTION HDRSAPTX_GET_AC_PKT

DESCRIPTION
  This function gets the AC packets  
  
DEPENDENCIES
  None

PARAMETERS
  
RETURN VALUE
  Length of AC packet in bytes
   
SIDE EFFECTS
  None
===========================================================================*/
static uint16 hdrsaptx_get_ac_pkt (void)
{
  uint16 pkt_len;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (hdrsaptx.verbose_debug)
  {
    HDR_MSG_PROT_1(MSG_LEGACY_MED, "=TX= before get pkt, tx_pkt_ptr addr %x", 
                 &(hdrsaptx.tx_pkt_ptr));
  }

  hdrsaptx.tx_pkt_ptr = NULL;

  /* Get the payload from PCP */
  pkt_len = hdrpcp_get_pkt( HDRSAPTX_MAX_PAYLOAD_SIZE_BYTES,
                            HDRHAI_AC,
                            hdrsaptx.elapsed_frames,
                            &hdrsaptx.tx_pkt_ptr,
                            &hdrsaptx.is_fmt_b );

  hdrsaptx.elapsed_frames = 0;

  HDR_MSG_PROT_2(MSG_LEGACY_HIGH, "=TX= AC pkt: received %d bytes, fmt %d", 
                 pkt_len, hdrsaptx.is_fmt_b); 

  return pkt_len;

} /* hdrsaptx_get_ac_pkt() */

/* EJECT */
/*===========================================================================

FUNCTION HDRSAPTX_GET_RTC_PKT

DESCRIPTION
  This function gets the RTC packets  
  
DEPENDENCIES
  None

PARAMETERS
  pkt_handle  - The packet handle of the last transmission 
  
RETURN VALUE
  Length of AC packet in bytes
   
SIDE EFFECTS
  None
===========================================================================*/
static uint16 hdrsaptx_get_rtc_pkt 
(
  hdrpcp_pkt_handle_type *pkt_handle
)
{
  uint16 pkt_len;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  hdrsaptx.tx_pkt_ptr = NULL;

  pkt_len = hdrpcp_get_pkt( HDRSAPTX_MAX_PAYLOAD_SIZE_BYTES,
                            HDRHAI_RTC,
                            hdrsaptx.elapsed_frames,
                            &hdrsaptx.tx_pkt_ptr,
                            &hdrsaptx.is_fmt_b );

  hdrpcp_get_curr_pkt_handle(pkt_handle);

  hdrsaptx.elapsed_frames = 0;

  HDR_MSG_PROT_3(MSG_LEGACY_HIGH, 
                 "=TX= RTC pkt, rxed %d bytes, fmt %d, pkt handle 0x%04x", 
                 pkt_len, hdrsaptx.is_fmt_b, *pkt_handle); 

  return pkt_len;

} /* hdrsaptx_get_rtc_pkt() */

/* EJECT */
/*===========================================================================

FUNCTION HDRSAPTX_ADD_HEADER

DESCRIPTION
  This function adds the SAP header value into a DSM item.

DEPENDENCIES
  This function is currently only to be used by Route Update Protocol and
  RLP when they directly give messages to SLP without using hdrhmp_send_msg.

PARAMETERS
  msg_ptr - Pointer to Pointer of the message to pre-pend the SAP header to

RETURN VALUE
  TURE - Add SAP header successfully, packet is ready to send.
  FALSE - Cannot insert SAP header, packet is not available to send.

SIDE EFFECTS
  None
===========================================================================*/
boolean hdrsaptx_add_header( dsm_item_type **msg_ptr )
{
  /* Buffer holding SAP header. */
  byte hdrsap_header[HDRSAP_MAX_HEADER_LEN];

  /* Real SAP header size. */
  uint8 sap_header_len;

  /* Session config token, which is public data of the SCP. */
  uint16 session_config_token;

  /* ATI record. */
  hdrhai_ati_record_type ati;
  
  /* Whether packet is Format B. */
  boolean is_size_incl;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  HDRSAP_CLR_HEADER(hdrsap_header);

  /* Get transmit ATI first. */
  if ( hdramp_get_transmit_ati(&ati, HDRHAI_SAP_TX_MODULE) != E_SUCCESS ) 
  {
    HDR_MSG_PROT( MSG_LEGACY_ERROR, "Could not get ATI for transmit" );
    return FALSE;
  }

  is_size_incl = hdrsaptx.is_fmt_b;

  /* A full-size SAP header's size is 7 bytes. */
  sap_header_len = HDRSAP_MAX_HEADER_LEN;

  if ( hdrsaps_get_state() == HDRSAPS_OPEN ) 
  {
    /* Set SAPState */
    HDRSAP_SET_PKT_SAP_STATE(hdrsap_header);
    
    /* Set ConnectionLayerFormat */
    if (is_size_incl) 
    {
      HDRSAP_SET_PKT_SIZE_BIT(hdrsap_header, 0);
    }

    /* Set ATI Record */
    HDRSAP_SET_PKT_ATI_TYPE(hdrsap_header, ati.ati_type, 0);
    HDRSAP_SET_PKT_ATI_VAL(hdrsap_header, ati.ati_value, 0);

    /* Config token is not in the SAP header. SAP header length is 5 bytes. */
    sap_header_len -= 2;

  } /* hdrsaps_get_state() == HDRSAPS_OPEN */
  else 
  {
    /* Set SessionConfigurationToken */
    session_config_token = 
        hdrscp_get_session_config_token(HDRHAI_SAP_TX_MODULE);
    HDRSAP_SET_PKT_CONFIG_TOKEN(hdrsap_header, session_config_token);

    /* Set ConnectionLayerFormat */
    if (is_size_incl) 
    {
      HDRSAP_SET_PKT_SIZE_BIT(hdrsap_header, 2);
    }

    /* Set ATI Record */
    HDRSAP_SET_PKT_ATI_TYPE(hdrsap_header, ati.ati_type, 2);
    HDRSAP_SET_PKT_ATI_VAL(hdrsap_header, ati.ati_value, 2);

  } /* hdrsaps_get_state() != HDRSAPS_OPEN */

  /* Add the SAP header. */
  if ( !(hdrutil_pushdown_head( msg_ptr, hdrsap_header, sap_header_len )) )
  {
    HDR_MSG_PROT( MSG_LEGACY_ERROR, "Add header, pushdown failed" ); 
    return FALSE;
  }

  return TRUE;

} /* hdrsaptx_add_header() */

/* EJECT */
/*===========================================================================

FUNCTION HDRSAPTX_PRINT_TX_STATUS_TABLE

DESCRIPTION
  This function prints tx status table.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void hdrsaptx_print_tx_status_table(void)
{
  uint8 i =0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  HDR_MSG_PROT_1( MSG_LEGACY_MED, "pending tx:%d", 
                  hdrsaptx.tx_status_cnt );

  for ( i = 0; i < HDRSAPTX_MAX_TX_STATUS_BUFFERS; i++ )
  {
    HDR_MSG_PROT_5( MSG_LEGACY_MED, "ac: %d, tid:%d, ph:0x%04x, aw:%d, age:%d",
                  hdrsaptx.tx_status[i].is_ac_pkt,
                  hdrsaptx.tx_status[i].trans_id,
                  hdrsaptx.tx_status[i].pkt_handle,
                  hdrsaptx.tx_status[i].awaiting_l2_ack,
                  hdrsaptx.tx_status[i].age ); 
  }
} /* hdrsaptx_print_tx_status_table() */

/* EJECT */
/*===========================================================================

FUNCTION HDRSAPTX_ADD_MSG_TO_TX_STATUS_TABLE

DESCRIPTION
  This function adds the a RTC pkt (pkt_handle) to tx_status table.

DEPENDENCIES
  None

PARAMETERS
  trans_id - trans_id for rtc pkt sent to tunnel
  pkt_handle - pkt_handle for rct pkt  sent to tunnel
  tx_chan - Channel the message must go out on (AC or RTC)

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void hdrsaptx_add_msg_to_tx_status_table
(  
  uint32 trans_id,
  hdrpcp_pkt_handle_type pkt_handle,
  hdrhai_channel_enum_type tx_chan 
)
{
  uint8 i = 0; 
  boolean found_free_entry = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Find a free entry */
  for ( i = 0; i < HDRSAPTX_MAX_TX_STATUS_BUFFERS; i++ )
  {
    if ((found_free_entry == FALSE) &&
        (hdrsaptx.tx_status[i].awaiting_l2_ack == FALSE))
    {
      /* Only update the first found free entry */
      found_free_entry = TRUE;

      hdrsaptx.tx_status[i].is_ac_pkt = (tx_chan == HDRHAI_AC) ? 1 : 0;
      hdrsaptx.tx_status[i].awaiting_l2_ack = TRUE;
      hdrsaptx.tx_status[i].pkt_handle = pkt_handle;
      hdrsaptx.tx_status[i].trans_id = trans_id;
      hdrsaptx.tx_status[i].age = 1;
    }
    else if (hdrsaptx.tx_status[i].awaiting_l2_ack == TRUE)
    {
      /* udpating age for other used entries */
      hdrsaptx.tx_status[i].age += 1;
    }
  }

  if (hdrsaptx.tx_status_cnt < HDRSAPTX_MAX_TX_STATUS_BUFFERS)
  {
    hdrsaptx.tx_status_cnt++;
  }
  else
  {
    HDR_MSG_PROT_1(MSG_LEGACY_ERROR, "pending TX exceeding max %d",
                   hdrsaptx.tx_status_cnt);
  }

  if (hdrsaptx.tx_status_cnt == HDRSAPTX_MAX_TX_STATUS_BUFFERS)
  {
    rex_set_timer( &hdrsaptx.con_close_timer,
                   HDRSAPTX_CON_CLOSE_TIME_MS );
  }

#if 0 
  /* temp. disable tx_status re-use */
  /* If no free entry, find the oldest entry and fail the packet */
  if (found_free_entry == FALSE)
  {
    HDR_MSG_PROT( MSG_LEGACY_ERROR, "Tx status table full" ); 

    for (i = 0; i < HDRSAPTX_MAX_TX_STATUS_BUFFERS; i++)
    {
      HDR_MSG_PROT_5( MSG_LEGACY_MED, "ac: %d, tid:%d, ph:0x%04x, aw:%d, age:%d",
                    hdrsaptx.tx_status[i].is_ac_pkt,
                    hdrsaptx.tx_status[i].trans_id,
                    hdrsaptx.tx_status[i].pkt_handle,
                    hdrsaptx.tx_status[i].awaiting_l2_ack,
                    hdrsaptx.tx_status[i].age ); 

      if (hdrsaptx.tx_status[i].age > hdrsaptx.tx_status[oldest_id].age)
      {
        oldest_id = i;
      }
    }

    /* Fail the oldest msg */
    if ( !hdrsaptx.tx_status[oldest_id].is_ac_pkt &&
         ( hdrsaptx.tx_status[oldest_id].pkt_handle != 0 ) )
    {
      /* Packet handle is only used for RTC msg */
      hdrpcp_packet_failed( hdrsaptx.tx_status[oldest_id].pkt_handle );
    }

    /* Update the oldest entry */
    hdrsaptx.tx_status[oldest_id].is_ac_pkt = (tx_chan == HDRHAI_AC) ? 1 : 0;
    hdrsaptx.tx_status[oldest_id].awaiting_l2_ack = TRUE;
    hdrsaptx.tx_status[oldest_id].pkt_handle = pkt_handle;
    hdrsaptx.tx_status[oldest_id].trans_id = trans_id;
    hdrsaptx.tx_status[oldest_id].age = 1;
  }
#endif 

  if (hdrsaptx.verbose_debug)
  {
    hdrsaptx_print_tx_status_table();
  }
} /* hdrsaptx_add_msg_to_tx_status_table() */

/* EJECT */
/*===========================================================================

FUNCTION HDRSAPTX_CLEAR_TX_STATUS_ENTRY

DESCRIPTION
  This function clear a tx status entry.

DEPENDENCIES
  None

PARAMETERS
  index - index to the tx_status table  

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void hdrsaptx_clear_tx_status_entry
( 
  uint8 index
)
{
  if (hdrsaptx.verbose_debug)
  {
    HDR_MSG_PROT_1( MSG_LEGACY_MED, "=TX= clear tx status entry %d", index ); 
  }

  rex_clr_timer( &hdrsaptx.con_close_timer );

  if ( hdrsaptx.tx_status[index].awaiting_l2_ack &&
       ( hdrsaptx.tx_status_cnt != 0 ) )
  {
    hdrsaptx.tx_status_cnt--;
  }

  hdrsaptx.tx_status[index].is_ac_pkt = FALSE;
  hdrsaptx.tx_status[index].age = 0;
  hdrsaptx.tx_status[index].awaiting_l2_ack = FALSE;
  hdrsaptx.tx_status[index].pkt_handle = 0;
  hdrsaptx.tx_status[index].trans_id = 0;
} /* hdrsaptx_clear_tx_status_entry() */

/* EJECT */
/*===========================================================================

FUNCTION HDRSAPTX_ADD_MSG_TO_TX_STATUS_TABLE

DESCRIPTION
  This function adds the SAP header value into a DSM item.

DEPENDENCIES
  This function is currently only to be used by Route Update Protocol and
  RLP when they directly give messages to SLP without using hdrhmp_send_msg.

PARAMETERS
  trans_id - trans id  
  tx_status - tx status for the trans id 

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void hdrsaptx_update_tx_status_table
( 
  uint32 trans_id,
  errno_enum_type tx_status 
)
{
  uint8 i =0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (hdrsaptx.verbose_debug)
  {
    hdrsaptx_print_tx_status_table();
  }

  /* Find if trans_id is awaiting for l2 ack */
  for ( i = 0; i < HDRSAPTX_MAX_TX_STATUS_BUFFERS; i++ )
  {
    if ( trans_id == hdrsaptx.tx_status[i].trans_id ) 
    {
      if (hdrsaptx.tx_status[i].awaiting_l2_ack)
      {
        if ( tx_status == E_SUCCESS )
        {
          /* If tx is successful, we do not need to do anyting, just clear entry*/
        }
        else
        {
          if (hdrsaptx.verbose_debug)
          {
            HDR_MSG_PROT_1( MSG_LEGACY_HIGH, "=TX= packet failed, ph 0x%04x",
                            hdrsaptx.tx_status[i].pkt_handle ); 
          }
          /* SLP does not register to packet failed cb, instead, slp uses
           * HDRIND_SAPTX_TRANSMISSION_FAILED_AC to return slp tx cb to msg sender */
          if ( !hdrsaptx.tx_status[i].is_ac_pkt && 
               ( hdrsaptx.tx_status[i].pkt_handle != 0 ) )
          {
            /* Packet handle is only used for RTC msg */
            hdrpcp_packet_failed( hdrsaptx.tx_status[i].pkt_handle );
          }
        }
      }
      else
      {
        /* trans_id is not in awaiting for l2 ack state, this should not happen */
        HDR_MSG_PROT( MSG_LEGACY_ERROR, "Found trans_id in incorrect state" ); 
      }

      hdrsaptx_clear_tx_status_entry(i);

      break;
    }
  }

  if (i == HDRSAPTX_MAX_TX_STATUS_BUFFERS)
  {
    /* Couldn't find trans_id in tx_status table 
     * This may because l2 ack comes too late and entry has been cleared due to, 
     * for example, tx_status table was full
     */
    HDR_MSG_PROT_1( MSG_LEGACY_ERROR, "Could not find trans_id %d", trans_id ); 
  }
} /* hdrsaptx_update_tx_status_table() */

/* EJECT */
/*===========================================================================
FUNCTION HDRSAPTX_IS_WAITING_FOR_AC_ACK

DESCRIPTION
  This function checks if we are waiting for L2 ack for AC msg 
  
DEPENDENCIES
  None.

PARAMETERS
   HDRHAI_AC  - tx AC channel packet
   HDRHAI_RTC - tx RTC channel packet

RETURN VALUE
  TRUE - waiting for L2 ack for AC msg
  FALSE - otherwise 

SIDE EFFECTS
  None.
  
===========================================================================*/
boolean hdrsaptx_is_waiting_for_ac_ack( void )
{
  uint8 i =0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (hdrsaptx.verbose_debug)
  {
    hdrsaptx_print_tx_status_table();
  }

  for ( i = 0; i < HDRSAPTX_MAX_TX_STATUS_BUFFERS; i++ )
  {
    if ( hdrsaptx.tx_status[i].is_ac_pkt )
    {
      if (!hdrsaptx.tx_status[i].awaiting_l2_ack)
      {
        /* This should not happen. 
         * To recover from this error, reset set entry and continue searching 
         */
        HDR_MSG_PROT( MSG_LEGACY_ERROR, "=TX= incorrect state" ); 
        hdrsaptx_clear_tx_status_entry(i);
      }
      else
      {
        return TRUE;
      }
    }
  }              

  return FALSE;
} /* hdrsaptx_is_waiting_for_ac_ack() */

/* EJECT */
/*===========================================================================
FUNCTION HDRSAPTX_START_TX

DESCRIPTION
  This function commands the SAPTX into tx mode 
  
DEPENDENCIES
  None.

PARAMETERS
   HDRHAI_AC  - tx AC channel packet
   HDRHAI_RTC - tx RTC channel packet

RETURN VALUE
  None.

SIDE EFFECTS
  None.
  
===========================================================================*/
void hdrsaptx_process_start_tx 
(
  hdrhai_protocol_name_enum_type sender,
  hdrhai_channel_enum_type tx_chan 
)
{
#ifndef FEATURE_IRAT_DSM_SUPPORT
  uint32  lte_byte_stream_len = 0;
#endif /* FEATURE_IRAT_DSM_SUPPORT */
  static uint16  trans_id = 0;
  uint16  pkt_len;

#ifdef FEATURE_IRAT_DSM_SUPPORT
  emm_irat_3gpp2_ul_msg_req_type  tunnel_ul_msg;
  msgr_attach_struct_type   *att_ptr;
#else
  emm_irat_ul_msg_req_type  tunnel_ul_msg;
  static uint8 lte_byte_stream[HDRSAPTX_MAX_PAYLOAD_SIZE_BYTES];
#endif /* FEATURE_IRAT_DSM_SUPPORT */
  errno_enum_type           estatus = E_FAILURE;

  hdrpcp_pkt_handle_type pkt_handle = HDRCP_PCP_LEGACY_PKT_HANDLE; /* used for RTC packet */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( hdrsaptx_get_pending_tx_cnt() >= 
       HDRSAPTX_MAX_TX_STATUS_BUFFERS )
  {
    HDR_MSG_PROT( MSG_LEGACY_HIGH, "Max pending tx msg, skip get pkt" );

    if (tx_chan == HDRHAI_AC)
    {
      /* if queue full, and need to send AC msg, it's
         likely AlternativeLinkCloseReq, siliently close */
      HDR_MSG_PROT(MSG_LEGACY_HIGH, 
                   "Q full w/ AC msg, Silent close");

      hdrsap_close_connection(HDRHAI_ABORT_UNSPECIFIED,
                              HDRHAI_SAP_TX_MODULE,
                              TRUE);
    }

    return;
  }

  if (tx_chan == HDRHAI_AC)
  {
    if (hdrsaptx_is_waiting_for_ac_ack())
    {
      /* Only one UL MSG for AC messages can be transmitted at any given time.
       * (It's similar to native HDR Mode and also it's simple to implement, if 
       * we allow multiple UL MSG(AC), we need to know the mappting between 
       * transmitted UL MSG and AC message(s) so we can call l2 ack (slp tx ack)
       * to correct senders)
       * If we are still waiting for L2 Ack, Do not try to get AC message now.
       * AC messages newly queued to SLP Tx queue will be transmitted after 
       * we receive L2 Ack for the previous UL MSG(AC). 
       */
      HDR_MSG_PROT(MSG_LEGACY_HIGH, "=TX= TX AC in progress"); 
      return;
    }

    HDR_MSG_PROT(MSG_LEGACY_HIGH, "=TX= Getting AC Pkt..."); 
    pkt_len = hdrsaptx_get_ac_pkt();

    if (pkt_len == 0)
    {
      HDR_MSG_PROT(MSG_LEGACY_HIGH, "End of AC transmission"); 

      if (hdrsaps_get_state() == HDRSAPS_OPEN)
      {
        /* End of AC transmission, check RTC */
        (void) rex_set_timer( &hdrsaptx.get_pkt_timer, 
                              HDRSAPTX_GET_PKT_TIME_MSEC );
      }
      return;
    }
    else
    {
      (void) rex_set_timer( &hdrsaptx.get_pkt_timer, 
                            HDRSAPTX_GET_PKT_TIME_MSEC );
    }
  }
  else if (tx_chan == HDRHAI_RTC)
  {
    HDR_MSG_PROT(MSG_LEGACY_HIGH, "=TX= Getting RTC Pkt..."); 
    pkt_len = hdrsaptx_get_rtc_pkt(&pkt_handle);

    /* Wait for next frame to get RTC packet again */
    (void) rex_set_timer( &hdrsaptx.get_pkt_timer, 
                          HDRSAPTX_GET_PKT_TIME_MSEC );

    if (pkt_len == 0)
    {
      HDR_MSG_PROT(MSG_LEGACY_MED, "=TX= NO pending RTC PKT"); 
      return;
    }
  }
  else
  {
    HDR_MSG_PROT_1(MSG_LEGACY_ERROR, "Incorrect chan type %d", tx_chan); 
  }

  /* Add SAP Header */
  if (!hdrsaptx_add_header(&hdrsaptx.tx_pkt_ptr))
  {
	/* TBD callbacks when add header fails. */
    HDR_MSG_PROT(MSG_LEGACY_HIGH, "Adding SAP header fails"); 
  }

#ifdef FEATURE_IRAT_DSM_SUPPORT
  memset(&tunnel_ul_msg, 0, sizeof(emm_irat_3gpp2_ul_msg_req_type));

  /* Send to LTE */
  msgr_init_hdr_attach ( &tunnel_ul_msg.msg_hdr, 
                         MSGR_HDR_CP, 
                         NAS_EMM_IRAT_HDR_UL_MSG_REQ,
						 MSGR_PRIORITY_HIGH, 
						 1 );

  att_ptr = msgr_get_attach(&tunnel_ul_msg.msg_hdr, 0);
  ASSERT(att_ptr != NULL);

  msgr_set_dsm_attach(att_ptr, hdrsaptx.tx_pkt_ptr);

  /* Populate transaction ID */
  tunnel_ul_msg.trans_id |= trans_id;

  tunnel_ul_msg.trans_id |= ((uint8)NAS_HDR) << 24; 
  /* add msg(pkt_handle) to tx_status table */
  hdrsaptx_add_msg_to_tx_status_table(tunnel_ul_msg.trans_id, pkt_handle, tx_chan);

  trans_id++;

  if (hdrsap_lte_to_hdr_active_ho_is_in_progress())
  {
    tunnel_ul_msg.msg.ul_msg_type = LTE_IRAT_TUNNEL_UL_MSG_UL_HO_PREP_TX_REQ;
  }
  else
  {
    tunnel_ul_msg.msg.ul_msg_type = LTE_IRAT_TUNNEL_UL_MSG_INFO_TRANSFER_REQ;
  }
  tunnel_ul_msg.msg.is_meid_present = FALSE;

  HDR_MSG_PROT_1( MSG_LEGACY_MED, "=TX= Sending UL MSG, trans_id %d", 
                  tunnel_ul_msg.trans_id ); 
  estatus = msgr_send ( &tunnel_ul_msg.msg_hdr, 
                        sizeof ( emm_irat_3gpp2_ul_msg_req_type ) );
#else
  /* Clean byte stream */
  memset(lte_byte_stream, 0, HDRSAPTX_MAX_PAYLOAD_SIZE_BYTES);

  /* Convert dsm to byte stream */
  if ( E_SUCCESS != hdrutil_dsm_to_byte( &hdrsaptx.tx_pkt_ptr, 
					                      lte_byte_stream, 
										  HDRSAPTX_MAX_PAYLOAD_SIZE_BYTES,
					                      &lte_byte_stream_len ) )
  {
    ERR_FATAL( "Packet size (%d) is too large", 
			   dsm_length_packet(hdrsaptx.tx_pkt_ptr), 0, 0 );
  }

  /* Send to LTE */
  msgr_init_hdr ( &tunnel_ul_msg.msg_hdr, 
                  MSGR_HDR_CP, 
                  NAS_EMM_IRAT_UL_MSG_REQ );

  if (hdrsap_lte_to_hdr_active_ho_is_in_progress())
  {
    tunnel_ul_msg.msg.ul_msg_type = LTE_IRAT_TUNNEL_UL_MSG_UL_HO_PREP_TX_REQ;
  }
  else
  {
    tunnel_ul_msg.msg.ul_msg_type = LTE_IRAT_TUNNEL_UL_MSG_INFO_TRANSFER_REQ;
  }
  
  tunnel_ul_msg.msg.ul_msg_u.ul_info_tx.type  = LTE_IRAT_RAT_TYPE_HRPD;
  tunnel_ul_msg.msg.ul_msg_u.ul_info_tx.msg_container.msg_size = lte_byte_stream_len;
#ifdef FEATURE_MEMCPY_REMOVAL
  memscpy ( &tunnel_ul_msg.msg.ul_msg_u.ul_info_tx.msg_container.msg_payload,
           sizeof(tunnel_ul_msg.msg.ul_msg_u.ul_info_tx.msg_container.msg_payload), 
           &lte_byte_stream,
           lte_byte_stream_len );
#else
  memcpy ( &tunnel_ul_msg.msg.ul_msg_u.ul_info_tx.msg_container.msg_payload,
           &lte_byte_stream,
           lte_byte_stream_len );
#endif /* FEATURE_MEMCPY_REMOVAL */

  estatus = msgr_send ( &tunnel_ul_msg.msg_hdr, 
                        sizeof ( emm_irat_ul_msg_req_type ) );
#endif /* FEATURE_IRAT_DSM_SUPPORT */

  ASSERT ( estatus == E_SUCCESS );

}

/* EJECT */
/*===========================================================================
FUNCTION HDRSAPTX_PROCESS_STOP_TX

DESCRIPTION
  This function commands the SAPTX to stop tx mode 
  
DEPENDENCIES
  None.

PARAMETERS
  None.
 
RETURN VALUE
  None.

SIDE EFFECTS
  None.
  
===========================================================================*/
void hdrsaptx_process_stop_tx 
(
  hdrhai_protocol_name_enum_type sender
)
{
  HDR_MSG_PROT_1( MSG_LEGACY_MED, "Trasmission is stopped by p0x%x", sender );

  (void) rex_clr_timer( &hdrsaptx.get_pkt_timer );

  hdrind_give_ind( HDRIND_SAP_TRANSMISSION_STOPPED, NULL );

  hdrsaptx_clear_tx_status_table();

} /* hdrsaptx_process_stop_tx() */

/*===========================================================================
FUNCTION HDRSAPTX_PROCESS_CLR_INFO_TX

DESCRIPTION
  This function process to clear  the SAP tx status table and pkt_timer
  
DEPENDENCIES
  None.

PARAMETERS
  None.
 
RETURN VALUE
  None.

SIDE EFFECTS
  None.
  
===========================================================================*/
void hdrsaptx_process_clr_info_tx 
(
  hdrhai_protocol_name_enum_type sender
)
{
  HDR_MSG_PROT_1( MSG_LEGACY_MED, "Clear info is indicated by p0x%x", sender );

  (void) rex_clr_timer( &hdrsaptx.get_pkt_timer );

  hdrsaptx_clear_tx_status_table();

} /* hdrsaptx_process_stop_tx() */

/* EJECT */
/*===========================================================================
FUNCTION HDRSAPTX_PROCESS_GET_PKT_TIMER                              EXTERNAL

DESCRIPTION
  This function processes timer expiry event for this protocol.

DEPENDENCIES
  None.

PARAMETERS
  timer_id: Timer Id

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrsaptx_process_get_pkt_timer( void )
{
  /* Timer expired means one frames has passed*/
  hdrsaptx.elapsed_frames++;

  /* If tx is still in progress, restart timer to increase elapsed_frames */
  if (hdrsaptx_is_waiting_for_ac_ack()) 
  {
    HDR_MSG_PROT(MSG_LEGACY_MED, "Restart get pkt timer(AC)"); 

    (void) rex_set_timer( &hdrsaptx.get_pkt_timer, 
                          HDRSAPTX_GET_PKT_TIME_MSEC );
  }
  else if ( hdrsap_connection_is_open(HDRHAI_SAP_TX_MODULE)) 
  {
    if (hdrsaptx.frame_timer_cb != NULL)
    {
      hdrsaptx.frame_timer_cb();
    }

    if (hdrsaptx.verbose_debug)
    {
      HDR_MSG_PROT(MSG_LEGACY_MED, "Restart get pkt timer(RTC)"); 
    }

    (void) rex_set_timer( &hdrsaptx.get_pkt_timer, 
                          HDRSAPTX_GET_PKT_TIME_MSEC );
    /* Get RTC pkt again 
     * Get pkt every frame until connection is closed
     */
    hdrsaptx_process_start_tx(HDRHAI_SAP_TX_MODULE, HDRHAI_RTC);
  }

  else
  {
    /* For AC pkt, timer should be cleared when received L2 Acked */
    HDR_MSG_PROT(MSG_LEGACY_ERROR, "Timer should not be running"); 
    rex_clr_timer(&hdrsaptx.get_pkt_timer);
  }

} /* hdrsaptx_get_pkt_timer */

/* EJECT */
/*===========================================================================

FUNCTION HDRTS_REGISTER_FRAME_TIMER_CALLBACK

DESCRIPTION
  This function registers frame timer callback function which will be called
  after frame timer (26.66ms) expires.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrsaptx_register_frame_timer_callback
(
  hdrsaptx_frame_cb_type              cb
    /* callback function to be registered */
)
{
/* TODO, when to un-register*/
  hdrsaptx.frame_timer_cb = cb;

} /* hdrsaptx_register_frame_timer_callback */

/* EJECT */
/*===========================================================================
FUNCTION HDRSAPTX_PROCESS_TIMER                                       EXTERNAL

DESCRIPTION
  This function processes timer expiry event for this protocol.

DEPENDENCIES
  None.

PARAMETERS
  timer_id: Timer Id

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrsaptx_process_timer(uint32 timer_id)
{

  switch ( timer_id )
  {
  case HDRSAPTX_GET_PKT_TIMER:
    hdrsaptx_process_get_pkt_timer();
    break;

  case HDRSAPTX_CON_CLOSE_TIMER:
     /* if timer expired and UL msg is still stuck,
        silent close connection. Otherwise, do nothing */
    if ( ( hdrsaps_get_state() == HDRSAPS_OPEN ) &&
         ( hdrsaptx_get_pending_tx_cnt() >= 
           HDRSAPTX_MAX_TX_STATUS_BUFFERS ) )
    {
      HDR_MSG_PROT( MSG_LEGACY_HIGH, "Silent close connection");

      hdrsap_close_connection(HDRHAI_ABORT_UNSPECIFIED,
                              HDRHAI_SAP_TX_MODULE,
                              TRUE);
    }
    break;

  default :
    ERR("Invalid SAPTX timer id !",0,0,0);
    break;
  }

} /* hdrsaptx_process_timer */

/* EJECT */
/*===========================================================================

FUNCTION HDRSAPTX_PROCESS_CMD

DESCRIPTION
  This function processes commands given to the saptx protocol.

DEPENDENCIES
  None

PARAMETERS
  cmd_ptr - Pointer to the command for the saptx protocol to process

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrsaptx_process_cmd
(
  hdrsaptx_cmd_type *cmd_ptr
)
{
  HDRTRACE_CMD( HDRHAI_SAP_TX_MODULE, cmd_ptr->cmd,
    cmd_ptr->sender, 0 );

  switch ( cmd_ptr->cmd )
  {
    case HDRSAPTX_START_TX_CMD:
    {
      hdrsaptx_process_start_tx(cmd_ptr->sender, cmd_ptr->data.tx_chan);
      break;
    }

    case HDRSAPTX_STOP_TX_CMD:
    {
      hdrsaptx_process_stop_tx(cmd_ptr->sender);
      break;
    }

    case HDRSAPTX_CLR_INFO_TX_CMD:
    {
      hdrsaptx_process_clr_info_tx(cmd_ptr->sender);
      break;
    }

    default:
    {
      ERR("Invalid command! = %x", cmd_ptr->cmd, 0, 0);
    }
  }
} /* hdrsaptx_process_cmd */

/* EJECT */
/*===========================================================================

FUNCTION HDRSAPTX_SET_IND_DATA_INFO

DESCRIPTION
  This function set indication data 

DEPENDENCIES
  None

PARAMETERS
  ul_msg_failure_ptr - pointer to the ul msg failure ptr 
  ind_data_ptr - pointer to the indication data

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrsaptx_set_ind_data_info
(
  emm_irat_failure_ind_type    *ul_msg_failure_ptr,
  hdrind_irat_failure_ind_type *irat_failure_info_ptr
)
{

  /* Set Default behavior */
  irat_failure_info_ptr->cause_type = HDRIND_IRAT_TEMP_FAILURE;
  irat_failure_info_ptr->backoff_time = 0;

  switch ( ul_msg_failure_ptr->cause_type )
  {
    case LTE_NAS_IRAT_NONE:
    case LTE_NAS_IRAT_INVALID_STATE:
    case LTE_NAS_IRAT_CONN_REL:
    case LTE_NAS_IRAT_UL_MSG_CNF_FAILURE:
    case LTE_NAS_IRAT_LOWER_LAYER_FAILURE:
    case LTE_NAS_IRAT_T3417_EXPIRY:
    case LTE_NAS_IRAT_SR_REJECT:
      break;

    case LTE_NAS_IRAT_ACCESS_BARRING: 
     irat_failure_info_ptr->cause_type = HDRIND_IRAT_TEMP_FAILURE_TIMER;
     irat_failure_info_ptr->backoff_time = 
       ul_msg_failure_ptr->access_barring_rem_time;
      break;

    case LTE_NAS_IRAT_CONN_EST_FAILURE:
      if (ul_msg_failure_ptr->cause.est_status == 
          LTE_RRC_CONN_EST_FAILURE_ACCESS_BARRED)
      {
        irat_failure_info_ptr->cause_type = HDRIND_IRAT_TEMP_FAILURE_TIMER;
        irat_failure_info_ptr->backoff_time = 
          ul_msg_failure_ptr->access_barring_rem_time;
      }
      break;

    default :
      ERR("Unknown cause_type %d !",ul_msg_failure_ptr->cause_type,0,0);
      break;
  }
  HDR_MSG_PROT_2(MSG_LEGACY_MED, "cause_type %d, timer %d",
               irat_failure_info_ptr->cause_type,
               irat_failure_info_ptr->backoff_time);

} /* hdrsaptx_process_irat_ul_msg_cnf_ind() */

/* EJECT */
/*===========================================================================

FUNCTION HDRSAPTX_PROCESS_IRAT_UL_MSG_CNF_IND

DESCRIPTION
  This function processes the indication sent from NAS indicating the 
  successful transmission of the message on LTE aire interface. 

DEPENDENCIES
  None

PARAMETERS
  ind_data_ptr - pointer to the indication data

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrsaptx_process_irat_ul_msg_cnf_ind
(
#ifdef FEATURE_IRAT_DSM_SUPPORT
  emm_irat_3gpp2_ul_msg_cnf_type *ul_msg_cnf_ptr
#else
  emm_irat_ul_msg_cnf_type       *ul_msg_cnf_ptr
#endif /* FEATURE_IRAT_DSM_SUPPORT */
)
{
  HDR_MSG_PROT_1(MSG_LEGACY_HIGH, "=RX= L2 ACK trans_id %d",
                 ul_msg_cnf_ptr->trans_id); 

  if (hdrsaptx_is_waiting_for_ac_ack())
  {
    HDR_MSG_PROT(MSG_LEGACY_HIGH, "=RX= L2 ACK (AC)");

    /* Update tx status table */
    hdrsaptx_update_tx_status_table(ul_msg_cnf_ptr->trans_id, E_SUCCESS);
	if ( hdrsap_tunnel_mode_is_enabled( HDRHAI_SAP_TX_MODULE ) )
	{
		/* Force SLP to call the callbacks for messages in this probe with success
		 * Must occur before indication so SLP will process them correctly.
		 */
		if (hdrsaptx.elapsed_frames == 0)
		{
		  HDR_MSG_PROT(MSG_LEGACY_HIGH, "set elapse frames to 1");
		  /* Force slp to flush cb */
		  hdrsaptx.elapsed_frames = 1;
		}

		(void) hdrpcp_get_pkt( 0, HDRHAI_AC, hdrsaptx.elapsed_frames,
						NULL, NULL );
		hdrsaptx.elapsed_frames=0;

		if (hdrsaptx.verbose_debug)
		{
		  HDR_MSG_PROT(MSG_LEGACY_HIGH, "Clear timer, reason: ac cnf");
		}
		rex_clr_timer(&hdrsaptx.get_pkt_timer);

		hdrind_give_ind( HDRIND_SAPTX_TRANSMISSION_CNF_AC, NULL );

		/* check if we still have pending message */
		hdrsaptx_process_start_tx(HDRHAI_SAP_TX_MODULE, HDRHAI_AC);
	}
	else
	{
	    HDR_MSG_PROT(MSG_LEGACY_HIGH, "Ignore : =RX= L2 SUCCESS when tunnel is not enabled");
	}
  }
  else if (hdrsaps_get_state() == HDRSAPS_OPEN )
  {
    HDR_MSG_PROT(MSG_LEGACY_HIGH, "=RX= L2 ACK (RTC)");

    /* Update tx status table */
    hdrsaptx_update_tx_status_table(ul_msg_cnf_ptr->trans_id, E_SUCCESS);

    /* Do not need to do anything, just wait for timer expires to get 
     * more RTC packets from PCP
     */
#ifdef FEATURE_HDR_SAP_QTF
    /* TODO, hack, remove later since timer is not working? */
    hdrsaptx_process_start_tx(HDRHAI_SAP_TX_MODULE, HDRHAI_RTC);
#endif /* FEATURE_HDR_SAP_QTF */
  }
  else
  {
    HDR_MSG_PROT(MSG_LEGACY_ERROR, 
                   "=RX= L2 ACKED but no msg pending"); 
    hdrsaptx_print_tx_status_table();
    rex_clr_timer(&hdrsaptx.get_pkt_timer);
    hdrsaptx.elapsed_frames=0;
  }

} /* hdrsaptx_process_irat_ul_msg_cnf_ind() */

/* EJECT */
/*===========================================================================

FUNCTION HDRSAPTX_PROCESS_IRAT_UL_MSG_FAILURE_IND

DESCRIPTION
  This function processes the indication sent from NAS indicating the 
  successful transmission of the message on LTE aire interface. 

DEPENDENCIES
  None

PARAMETERS
  ind_data_ptr - pointer to the indication data

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrsaptx_process_irat_ul_msg_failure_ind
(
  emm_irat_failure_ind_type       *ul_msg_failure_ptr
)
{
  hdrind_irat_failure_ind_type irat_failure_info;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  HDR_MSG_PROT_2(MSG_LEGACY_HIGH, "=RX= L2 FAILURE trans_id %d, e %d",
                 ul_msg_failure_ptr->trans_id,
                 ul_msg_failure_ptr->cause_type); 

  hdrsaptx_set_ind_data_info(ul_msg_failure_ptr, &irat_failure_info);

  if (hdrsaptx_is_waiting_for_ac_ack())
  {
    rex_clr_timer(&hdrsaptx.get_pkt_timer);
    hdrsaptx.elapsed_frames = 0;

    /* update tx status */
    hdrsaptx_update_tx_status_table(ul_msg_failure_ptr->trans_id, E_FAILURE);
	if ( hdrsap_tunnel_mode_is_enabled( HDRHAI_SAP_TX_MODULE ) )
	{

		hdrind_give_ind( HDRIND_SAPTX_TRANSMISSION_FAILED_AC, (void*) &irat_failure_info );

		hdrsaptx_process_start_tx(HDRHAI_SAP_TX_MODULE, HDRHAI_AC); 
	}
	else
	{
	    HDR_MSG_PROT(MSG_LEGACY_HIGH, "Ignore : =RX= L2 FAILURE when tunnel is not enabled");
	}
  }
  else if ( hdrsaps_get_state() == HDRSAPS_OPEN ) 
  {
    /* update tx status */
    hdrsaptx_update_tx_status_table(ul_msg_failure_ptr->trans_id, E_FAILURE);
  }
  else
  {
    HDR_MSG_PROT(MSG_LEGACY_ERROR, 
                   "=RX= L2 FAILURE but no msg pending"); 
    hdrsaptx_print_tx_status_table();
    rex_clr_timer(&hdrsaptx.get_pkt_timer);
    hdrsaptx.elapsed_frames = 0;
  }
} /* hdrsaptx_process_irat_ul_msg_failure_ind() */

/* EJECT */
/*===========================================================================

FUNCTION HDRSAPTX_PROCESS_IND

DESCRIPTION
  This function processes indications given to SAPTX.

DEPENDENCIES
  None.

PARAMETERS
  ind_name - Name of the indication for SAPTX to process
  ind_data_ptr - pointer to data associated with the indication

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrsaptx_process_ind
(
  hdrind_ind_name_enum_type ind_name,
  hdrind_ind_data_union_type * ind_data_ptr
)
{
  HDRTRACE_IND(HDRHAI_SAP_TX_MODULE, ind_name, 0);

  switch (ind_name)
  {
    case HDRIND_SAP_CONNECTION_CLOSED:
      hdrsaptx_clear_tx_status_table();
      rex_clr_timer(&hdrsaptx.get_pkt_timer);
      break;

    default:
      ERR_FATAL( "Unknown indication", 0, 0, 0 );
      break;
  } 
} /* hdrsaptx_process_ind */

/* EJECT */
/*===========================================================================

FUNCTION HDRSAPTX_START_TX

DESCRIPTION
  This function commands the SAPTX into tx mode:
  for AC transmission, HMP will be the caller
  for RTC transmission, SAP will be the caller

DEPENDENCIES

PARAMETERS
  caller - The protocol calling this function 
  tx_chan - Channel the message must go out on (AC or RTC)

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrsaptx_start_tx 
( 
  hdrhai_protocol_name_enum_type caller,
  hdrhai_channel_enum_type tx_chan 
)
{
  /* Temporary command buffer. */
  hdrsaptx_cmd_type cmd_buf;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  cmd_buf.cmd = HDRSAPTX_START_TX_CMD;
  cmd_buf.sender = caller;
  cmd_buf.data.tx_chan = tx_chan;

  if (rex_self() == HDRSAPTX_TASK_PTR)
  {
    /* call the processing function directly if in the same task */
    hdrsaptx_process_cmd( &cmd_buf );
  }
  else
  {
    /* Queue the command for execution in the home task. */
    hdrtx_queue_cmd( HDRHAI_SAP_TX_MODULE, &cmd_buf, sizeof( cmd_buf ) );
  }
} /* hdrsaptx_start_tx() */


/* EJECT */
/*===========================================================================

FUNCTION HDRSAPTX_STOP_TX

DESCRIPTION
  This function stops SAP transmission.

DEPENDENCIES

PARAMETERS
  caller - The protocol calling this function 

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrsaptx_stop_tx 
( 
  hdrhai_protocol_name_enum_type caller
)
{
  hdrsaptx_cmd_type cmd_buf;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  cmd_buf.cmd = HDRSAPTX_STOP_TX_CMD;
  cmd_buf.sender = caller;

  if (rex_self() == HDRSAPTX_TASK_PTR)
  {
    /* call the processing function directly if in the same task */
    hdrsaptx_process_cmd( &cmd_buf );
  }
  else
  {
    /* Queue the command for execution in the home task. */
    hdrtx_queue_cmd( HDRHAI_SAP_TX_MODULE, &cmd_buf, sizeof( cmd_buf ) );
  }
} /* hdrsaptx_stop_tx() */


/* <EJECT> */
/*===========================================================================

FUNCTION HDRSAPTX_IND_CB

DESCRIPTION
  This function is registered to be called whenever a protocol gives an
  indication of interest to SAPTX.

DEPENDENCIES
  None

PARAMETERS
  ind_name - Indication to process, is a global enum for all
                    possible indications in HDR signaling
  ind_data - Optional data pointer for indication specific data

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void hdrsaptx_ind_cb (hdrind_ind_name_enum_type ind_name, void *ind_data)
{
  HDR_MSG_PROT_1( MSG_LEGACY_MED, "hdrsaptx_ind_cb %d", ind_name );

  /* This protocol runs in the tx task. */
  if (rex_self () == HDRSAPTX_TASK_PTR)
  {
    /* Indication is from another protocol in the same task, so consider it
     * an internal indication
     */
    hdrsaptx_process_ind (ind_name, (hdrind_ind_data_union_type *)ind_data);
  }
  else
  {
    hdrtx_queue_ind (HDRHAI_SAP_TX_MODULE, ind_name, ind_data);
  }
} /* hdrsaptx_ind_cb() */

/*===========================================================================

FUNCTION HDRSAPTX_CLR_INFO_TX

DESCRIPTION
  This function commands to clear  the SAP tx status table and pkt_timer

DEPENDENCIES

PARAMETERS
  caller - The protocol calling this function 

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrsaptx_clr_info_tx 
( 
  hdrhai_protocol_name_enum_type caller
)
{
  hdrsaptx_cmd_type cmd_buf;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  cmd_buf.cmd = HDRSAPTX_CLR_INFO_TX_CMD;
  cmd_buf.sender = caller;

  if (rex_self() == HDRSAPTX_TASK_PTR)
  {
    /* call the processing function directly if in the same task */
    hdrsaptx_process_cmd( &cmd_buf );
  }
  else
  {
    /* Queue the command for execution in the home task. */
    hdrtx_queue_cmd( HDRHAI_SAP_TX_MODULE, &cmd_buf, sizeof( cmd_buf ) );
  }
} /* hdrsaptx_clr_info_tx() */


#endif /* FEATURE_LTE_TO_HDR_OH */
