/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                     H D R   B R O A D C A S T   M A C
       
                             P R O T O C O L

GENERAL DESCRIPTION
  This module contains the implementation of the HDR Broadcast MAC
  Protocol.
    
EXTERNALIZED FUNCTIONS
    
INITIALIZATION AND SEQUENCING REQUIREMENTS
  The protocol must be initialized after system startup prior to activating 
  the protocol.
  
  
  Copyright (c) 2002 - 2012 by Qualcomm Technologies, Inc.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* EJECT */
/*===========================================================================

                           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/bc/src/hdrbcmac.c#1 $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/04/13   sat     Removed unused parameters in F3s.
03/20/12   cnx     Resolved TCB compiler errors for Dime. 
09/30/11   cnx     Changed %P in F3 to 0x%x.
06/21/10   cnx     Fixed compiler warnings.
08/06/09   wsh     Support CMI API changes in sys.h 
03/23/09   wsh     CMI: featurized CMI under FEATURE_CMI
01/30/09   etv     Merged the following changes to main branch
                   Fixed Klocwork errors.
                   Added support for 8/16 Multiplexes
01/27/09   wsh     Merged BCMCS 2.0 onto mainbranch
12/10/08   smd     Replaced assert.h with amssassert.h
08/22/07   kss     Minor fix for lint.
03/21/07   etv     Avoid buffer overrun by masking right number of bits.
02/05/07   etv     Fixed bug with hdrbcmac_channel_resume
08/09/06   etv     Trigger protocols to check for sleep for bad CRC also.
04/24/06   kss     Recompute sleep config after overhead update; maintain
                   channel index on overhead update.
03/20/06   kss     Added BCMCS sleep support interface.
11/07/05   mpa     Fixed logical channel comparison.
10/28/05   dna     Add pilot pn as parameter to hdrbcmac_update_ovhd_info.
10/11/05   dna     Include hdrbcslot_update_phy_ch_info() in critical section
                   to avoid it getting out of sync with overhead info change.
09/30/05   mpa     Use command instead of indication to update ovhd info.
08/28/05   dna     (mpa for) Use bcovhd interface to retrieve ovhd info.
02/10/05   etv     Fixed lint errors.
02/02/05   etv     Fixed the coding style to match the convention.
11/19/04   etv     Renamed hdrbcmac_clear_statistics() to
                   hdrbcmac_clear_phy_chan_metrics().
11/04/04   kss     Removed hdrbcmac_suspend/resume which are no longer needed.
11/03/04   mpa     Removed dependency on std.h (merged with sys.h)
10/04/04   kss     Use flow enable/disable commands during overhead update.
09/17/04   kss     Added hdrbcmac_run_scheduler() and 
                   hdrbcmac_was_bcc_packet_scheduled() functions.
08/18/04   kss     Implemented BCMAC suspend/resume for hybrid operation.
08/16/04   kss     Added support for tracking flow status (moved from 
                   hdrbctask.c); comment cleanup and function reordering.
08/02/04   kss     Restored time parameter to hdrbcmac_queue_pkt().
07/14/04   kss     Keep data in dsm items when handing up the stack.
07/08/04   kss     Added include of hdrbcmaci.h.
07/06/04   kss     Added functions for logging, extern metrics structures.
07/02/04   kss     Removed time parameter from hdrbcmac_queue_pkt().
07/02/04   mpa     Renamed hdrbcast to hdrbcovhd
06/14/04   ss      (mpa for) Debug message clean up
06/07/04   kss     Minor change to support ECB status logging.
05/24/04   kss     Rename hdrbcmac type (minor cleanup).
04/24/04   kss     Ported from QC Corp.

===========================================================================*/
/* <EJECT> */
/*===========================================================================

                        INCLUDE FILES FOR MODULE

===========================================================================*/

#include "hdr_variation.h"
#include "memory.h"
#include "amssassert.h"
#include "hdrdebug.h"
#include "hdrtrace.h"
#include "err.h"
#include "dsm.h"
#include "hdrfmac.h"
#include "hdrcmac.h"
#include "sys.h"

#ifdef FEATURE_CMI
#include "sys_v.h"
#endif

#include "hdrlmac.h"
#include "hdrind.h"

#ifdef FEATURE_HDR_BCMCS

#include "hdrbcmac.h"
#include "hdrbcmaci.h"
#include "hdrbcsec.h"
#include "hdrbcovhd.h"
#include "hdrbctask.h"
#include "hdrlogi.h"
#include "hdrbcslot.h"
#include "hdrbcecb.h"


/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

typedef enum   /* general constants */
{

  HDRBCMAC_PACKET_DATA_MAX_SIZE = 125,  
    /* 3 bytes to tail/MAC overhead */

  HDRBCMAC_BCMCS_FLOWS_MAX = HDRBCMACI_BCMCS_MAX_FLOWS
    /* We can support up to 2^7 = 128. However
      we limit it artificially to 16 to save memory. */
   
} hdrbcmac_constants;


/*---------------------------------------------------------------------------
  MAC channel object.  Used to store MAC infomation for a specific channel.
---------------------------------------------------------------------------*/
typedef struct
{
   uint8    active;       /* 1 - the mac stack is active and is actively
                             decoding broadcast data.
                             0 - pending(inactive), the stack is waiting
                             for sector to begin transmission of a specific
                             logical channel */

   uint32   flow_id;      /* logical channel-ID (only used for debugging) */

   uint8    logical_channel_index;
   
   hdrbcecb ecb;          /* Error Control Block Decoder */
   uint16   im_ref_count; /* reference counter indicating how many
                             interlace+mulitplex pairs are directing 
                             content to this stack's ecb */

   uint16   reference_count;
   
   hdrlogbc_mac_metrics_type chan_metrics;

   uint16   chan_idx;

   uint16   crc_failures_per_ecb;

} hdrbcmac_channel_type;

typedef struct 
{
  /* Count of all flows that are currently ACTIVE */
  uint8  active_flow_count;

  /* Count of all logical channels that are currently ACTIVE */
  uint8  active_channel_count;
  
  /* Flow list. Each flow has a status associated with it:
     ACTIVE (currently being monitored), PENDING (waiting for flow
     to be added to overhead messaging), or INACTIVE, which means the table
     entry is not valid */
  struct
  {
    uint32                     flow_id;
    uint8                      chan_idx;
    boolean                    active;
  } flow_list[HDRBCMAC_BCMCS_FLOWS_MAX];


  /* interlace-multiplex stack assignment lookup table */
  hdrbcmac_channel_type* 
     im_chan_map[ HDRBCMAC_INTERLACE_COUNT ][ HDRBCMAC_MULTIPLEX_COUNT ];

  hdrbcmac_ovhd_info_type ovhd_info;

  /* array of allocated stacks */ /* ? */
  hdrbcmac_channel_type* channel_ptr[ HDRBCMAC_MAX_LOGICAL_CHANNELS ];

  hdrbcmac_channel_type channel[HDRBCMAC_MAX_LOGICAL_CHANNELS];

  /* Channel Metrics */
  hdrbcmac_static_metrics metrics;

  /* Global enable for BCMCS sleep */
  boolean  sleep_enabled;

  /* BCMCS sleep cycle */
  uint16  sleep_cycle;

} hdrbcmac_type;

hdrbcmac_type hdrbcmac;




/*=============================================================================
                           FUNCTION DEFINITIONS
=============================================================================*/


/*===========================================================================
 
FUNCTION HDRBCMAC_INIT

DESCRIPTION
  Initializes the HDR Broadcast MAC to known state (at powerup).

DEPENDENCIES
  None.

PARAMETERS
  None. 

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrbcmac_init( void )
{

  int i;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  hdrbcmac.active_flow_count = 0;
  hdrbcmac.active_channel_count = 0;
  hdrbcmac.sleep_enabled = FALSE;

  /* Init to not active */
  for ( i=0; i<HDRBCMAC_MAX_LOGICAL_CHANNELS; i++ )
  {
    hdrbcmac.channel[i].active=0;
  }


} /* hdrbcmac_init */


/*===========================================================================
 
FUNCTION HDRBCMAC_CONFIG_SLEEP

DESCRIPTION
  Attempts to configure sleep based on the current flow configuration.
  If sleep is supported, it will config the BCMCS cycle in the HDR searcher.

DEPENDENCIES
  None.

PARAMETERS
  None. 

RETURN VALUE
  None.

SIDE EFFECTS
  If sleep is supported, it will config the BCMCS cycle in the HDR searcher.

===========================================================================*/
void hdrbcmac_config_sleep( void )
{
  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Any flows active, and is sleep enabled? */
  if ((hdrbcmac.active_channel_count) && 
      (hdrbcmac.sleep_enabled == TRUE))
  {
    /* Get the supported sleep cycle. */
    hdrbcmac.sleep_cycle = hdrbcslot_config_sleep();
    
    HDR_MSG_PROT_1(MSG_LEGACY_HIGH,
                   "BCMCS Sleep cycle: %d", hdrbcmac.sleep_cycle);
  }
  else
  {

    /* Disable sleep cycle. */
    hdrbcmac.sleep_cycle = 0;
  }

  /* Set BCMCS sleep cycle based in determined value. */
  hdrlmac_set_bcmcs_sleep_cycle( (uint8) hdrbcmac.sleep_cycle);
  
}

 /* EJECT */
/*===========================================================================
 
FUNCTION HDRBCMAC_SEND_DATA                                        INTERNAL

DESCRIPTION
  Moves the ECB data to the higher layer.  The function applies
  Reed solomon, passes data up, and resets the ECB.
  
DEPENDENCIES
  None.

PARAMETERS
  mac - stack containing ecb+data for channel
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrbcmac_send_data( hdrbcmac_channel_type* mac )
{
  static hdrbcecb_packet_array array;
  int i;
  uint16 length;
  uint16 security_layer_packet_length;
  dsm_item_type* packet;
  dsm_item_type* sec_pkt;
  uint16 max_packets;
  hdrbcecb* ecb = &(mac->ecb);
  uint8 ecb_header;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* completes the ECB by running the the RS decoder */
  hdrbcecb_flush( ecb );  
  
  HDR_MSG_PROT(MSG_LEGACY_HIGH,"PCM SendToHigherLayer");

  /* get the packet stream for this decoder and pass it up the stack */
  hdrbcecb_get_packet_array( ecb, &array );

  /* process each packetin the array */
  max_packets = array.count;

  {
    for ( i=0; i<max_packets; i++ )
    {
      /* the packet array may contain nulls (0).  A null would indicate that
         the packet was not received in the desired ECB position and thus
         should be treated as a CRC failure */
      packet = hdrbcecb_packet_array_get_next_packet( &array );
      
      if (packet)
      { 
        mac->chan_metrics.packets_delivered++;
        length = 125;

        if ( i == 0 )
        {
          /* Security layer payload is stuffed into the 1st ecb row 
             Block header (1 byte) = Bits 7-1: length  
                                     Bit 0: connection layer format */
          (void) dsm_pullup(&packet, &ecb_header, 1);
          length--;
          security_layer_packet_length = (ecb_header & 0xFE) >> 1;
          
          if ( security_layer_packet_length > 0 )
          {
            mac->chan_metrics.security_layer_packets++; 

            length = length - security_layer_packet_length;
            sec_pkt = packet;
            packet = NULL;
            
            (void) dsm_dup_packet( &packet, 
                                   sec_pkt, 
                                   security_layer_packet_length, 
                                   length );
            
            dsm_trim_packet(&sec_pkt, security_layer_packet_length);

            /* hdrpcp_put_pkt(packet, ecb_header & 1); */
            dsm_free_packet(&sec_pkt);

          }
        }

        /* dsm_free_packet(&packet); */

        /* send data to higher layer */
        hdrbcsec_put_pkt( packet, length, mac->chan_idx );
 
      }
    }
  }

  /* Reset ecb */
  hdrbcecb_reset( ecb );

  HDR_MSG_PROT_1(MSG_LEGACY_HIGH,"Reset ECB. Free=%d", 
                 DSM_POOL_FREE_CNT(DSM_DS_SMALL_ITEM_POOL));

} /* hdrbcmac_send_data */


 /* EJECT */
/*===========================================================================
 
FUNCTION HDRBCMAC_DEASSIGN

DESCRIPTION
  Decrements the reference count for the mac stack object assigned to the
  specifed interlace and multiplex.  If the reference count decrements to
  0, then the higher layer callback will be notified that the stack is
  no longer decoding packets.  If the stack is free of all i+m then it's
  moved to the pending state.
  
DEPENDENCIES
  None.

PARAMETERS
  interlace - interlace of decoder
  multiplex - multiplex of decoder
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrbcmac_deassign( uint16 interlace, uint16 multiplex )
{
  hdrbcmac_channel_type* mac = hdrbcmac.im_chan_map[interlace][multiplex];

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if ( mac != 0 )
  {
    /* adjust the reference count for the decoder object allocated to 
       this i+m */
    HDR_MSG_PROT_3(MSG_LEGACY_HIGH," _StackFree( %u, %u ) RF:%u\n",
                   interlace, multiplex, mac->im_ref_count );

    if ( mac->im_ref_count == 0 )
    {
      /* safety check, i hope it doesn't happen */
      hdrbcmac.metrics.decoder_ref_count_error++;
      
      /* we do this so we can free it */
      mac->im_ref_count = 1;  

      HDR_MSG_PROT_2(MSG_LEGACY_HIGH," _StackFree(%u, %u )  RefCount ERROR\n",
                     interlace, multiplex );
    }

    if ( --(mac->im_ref_count) == 0 )
    {
      HDR_MSG_PROT_2(MSG_LEGACY_HIGH,
                     " _StackFree(%u, %u ) Freeing Decoder RF=0\n",
                     interlace, multiplex );

      /* if this is the only interlace multiplex supporting this decoder
         move this stack to the pending state */
      mac->active = 0;

      hdrbcecb_reset(&(mac->ecb));

      HDR_MSG_PROT_1(MSG_LEGACY_HIGH,
                     " chId:%d moved to PENDING\n",mac->flow_id );

    }

    /* clear decoder entry in the lookup table */
    hdrbcmac.im_chan_map[interlace][multiplex] = 0;
  }

} /* hdrbcmac_deassign */


 /* EJECT */
/*===========================================================================
 
FUNCTION HDRBCMAC_ASSIGN

DESCRIPTION
  Enables and configures the stack for a specified interlace+multiplex.
  
DEPENDENCIES
  None.

PARAMETERS
  mac - mac stack to assign.
  ovhd_info_ptr - Pointer to BCMAC's copy of the broadcast overhead info.  
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
uint8 hdrbcmac_assign
( 
  hdrbcmac_channel_type* mac,
  const hdrbcmac_ovhd_info_type * const ovhd_info_ptr
)
{
  uint8 rc = 0; 
    /* Return code */

  uint8 logical_channel_index = 0;
    /* logical channel index associated with "mac" */

  const hdrbcovhd_logical_channel_info_type *logical_channel = 0;

  uint8 phys_chan = 0;
    /* Loop variable */

  uint16 intr;
    /* Loop variable */

  uint16 mux;
    /* Loop variable */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* SENTHIL_TBD
     This needs to be modified to reflect the new overhead message structure.
     Find out the LogicalChannelIndex of the BCMCSFlow associated with this
     MACLayer object. */
  logical_channel_index = mac->logical_channel_index;
  if ( logical_channel_index >= ovhd_info_ptr->num_flows )
  {
    HDR_MSG_PROT_2(MSG_LEGACY_HIGH,
                   " LogicalChIndex %d exceeds Max %d",logical_channel_index,
                   ovhd_info_ptr->num_flows - 1  );
    rc = 0;
    return rc;
  }

  /* Retrieve the pointer to the Logicalchannel. */
  logical_channel = 
    &(ovhd_info_ptr->log_chan_info[logical_channel_index]);

  /* Assign this MacLayer object to all the physical Channels that comprise
     this LogicalChannel */
  for ( phys_chan = 0; 
      phys_chan < logical_channel->phys_chan_count; phys_chan++ )
  {
    intr = logical_channel->phys_chan[phys_chan].intr;
    mux  = logical_channel->phys_chan[phys_chan].mux;

    /* make sure the interlace+multiplex is deallocated. */
    hdrbcmac_deassign(intr, mux);

    hdrbcmac.im_chan_map[intr][mux] = mac;
    mac->im_ref_count++;

    /* Init ecb -- move to ecb block */
    mac->crc_failures_per_ecb = 0;


    if ( mac->active == 0 )
    {
      /* move macLayer state */
      mac->active = 1;
      HDR_MSG_PROT_2( MSG_LEGACY_HIGH,
                      "chId:%d mac:0x%x moved to ACTIVE", mac->flow_id, mac );

      /* configure the ecb decoder */
      (void) hdrbcecb_configure( &(mac->ecb), 
                                 logical_channel->outer_code,
                                 logical_channel->mac_pkts_per_ecb_row );

      HDR_MSG_PROT_5( MSG_LEGACY_HIGH,
                      "ECB Initialzed mac:0x%x ecb:0x%x chId:%d oc:%d macPerRow:%d",
                      mac,&(mac->ecb), mac->flow_id,
                      logical_channel->outer_code,
                      logical_channel->mac_pkts_per_ecb_row );
    }
  }
  rc = 1;

  return rc;

} /* hdrbcmac_assign */

 /* EJECT */
/*===========================================================================

FUNCTION HDRBCMAC_UPDATE_OVHD_INFO

DESCRIPTION
  This command updates the Broadcast MAC related Overhead info.
  The command will be queued and handled by the broadcast task context.
  
DEPENDENCIES
  None.

PARAMETERS
  sender - Protocol sending the command
   
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrbcmac_update_ovhd_info
(
  hdrhai_protocol_name_enum_type sender,
  sys_pilot_pn_type              pilot_pn
)
{
  /* Temporary command buffer. */
  hdrbcmac_cmd_union_type cmd_buf;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  HDR_MSG_PROT( MSG_LEGACY_MED, "Send hdrbcmac_update_ovhd cmd.");

  cmd_buf.name   = HDRBCMAC_UPDATE_OVHD;
  cmd_buf.sender = sender;
  cmd_buf.pilot_pn = pilot_pn;

  hdrbctask_queue_cmd( HDRHAI_BROADCAST_MAC_PROTOCOL, 
                       &cmd_buf, sizeof(cmd_buf) );

} /* hdrbcmac_update_ovhd() */


 /* EJECT */
/*===========================================================================
 
FUNCTION HDRBCMAC_INSERT_FLOW                                    (INTERNAL)

DESCRIPTION
  Adds a flow to the flow table.
  
DEPENDENCIES
  None.

PARAMETERS
  flow_id - The flow to add.
  chan_idx - The channel index the flow is assigned to.
  
RETURN VALUE
  -1 if failure occurs.

SIDE EFFECTS
  None.

===========================================================================*/
int hdrbcmac_insert_flow
(
  uint32 flow_id,
  int    chan_idx
)
{
  int i,rc = -1;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  for ( i=0; i<HDRBCMAC_BCMCS_FLOWS_MAX; i++ )
  {
    if ( hdrbcmac.flow_list[i].active == FALSE ) 
        
    {
      hdrbcmac.flow_list[i].flow_id  = flow_id;
      hdrbcmac.flow_list[i].chan_idx = (uint8) chan_idx;
      hdrbcmac.flow_list[i].active   = TRUE;
      rc = i;
      break;
    }
  }

  return rc;

} /* hdrbcmac_insert_flow */


 /* EJECT */
/*===========================================================================
 
FUNCTION HDRBCMAC_CHANNEL_RESUME

DESCRIPTION
  The function enables a new flow in the HDR Broadcast MAC Protocol.
  A flow_id is taken as a parameter to open a new channel.  The return value
  is a handle (or index) for the assigned logical channel.
  
DEPENDENCIES
  None.

PARAMETERS
  flow_id - The desired flow.
  
RETURN VALUE
  logical channel index to use to refer to this channel.
  -1 if flow cannot be enabled.

SIDE EFFECTS
  None.

===========================================================================*/
int hdrbcmac_channel_resume
( 
  uint32 flow_id,
  uint8  chan_idx
)
{
  int rc = -1; 
  uint8 i;
  hdrbcmac_channel_type* mac;
  uint8 flow_available = 0;
  uint8 logical_channel_index = 0xFF;  

  const hdrbcmac_ovhd_info_type * const ovhd_info_ptr = &hdrbcmac.ovhd_info;
    /* Current broadcast overhead information */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Does flow exist already? */
  for ( i=0; i<HDRBCMAC_BCMCS_FLOWS_MAX; i++ )
  {
    if ((hdrbcmac.flow_list[i].active) &&
        (hdrbcmac.flow_list[i].flow_id == flow_id))
    {
       /* the Flow ID already has a channel allocated. */
       HDR_MSG_PROT_1(MSG_LEGACY_HIGH,"Flow %d already enabled", 
                      flow_id );
       return -1;
    }
  }
 
  /* If the flow exists in the current message then find the corresponding 
     logical channel associated. */
  for ( i = 0; i < ovhd_info_ptr->num_flows; i++ )
  {
    if (ovhd_info_ptr->bcmcs_flow_id[i] == flow_id )
    {
      /* this flow can be decoded (is available) */
      flow_available = 1;

      /* Find the logical Ch Index associated with this flow. */
      logical_channel_index = i;

      HDR_MSG_PROT_2(MSG_LEGACY_HIGH," Flow %d Available. LogCh = %d\n", 
                     flow_id, logical_channel_index);
      break;
    }
  }

  /* Let caller track requested/pending flows since we don't know if they
     will be on the same logical channels. */
  if (flow_available == 0)
  {
    /* Flow is not available! */
    return -1;
  }

  /* If flow is available, check to see if any other flow is assigned to the 
     same logical channel & is currently being decoded. If so return with a 
     failed code. */
  for ( i=0; i<HDRBCMAC_BCMCS_FLOWS_MAX; i++ )
  {
    if ( hdrbcmac.channel[i].active == 1)
    {
      if (hdrbcovhd_flows_have_same_logical_channel( 
            flow_id,
            hdrbcmac.channel[i].flow_id ))
      {
        HDR_MSG_PROT_2(MSG_LEGACY_MED," LogCh %d For Flow %d already in Use\n", 
                       logical_channel_index, flow_id );

        /* The LogicalChannel associated with the requested BCMCSFlow is 
           already being decoded. */
        hdrbcmac.channel[i].reference_count++; 

        hdrbcmac.active_flow_count++;  

        (void) hdrbcmac_insert_flow(flow_id, i);

        rc = -1;
        return rc;
      }
    }
  }

  i = chan_idx;

  if ( hdrbcmac.channel[i].active == 0 )
  {
    /* Found available channel, fill out structure */
    mac = &hdrbcmac.channel[i];

    hdrbcmac.channel_ptr[i] = mac;

    mac->flow_id = flow_id;
    mac->logical_channel_index = logical_channel_index;
    mac->chan_idx=i;
    hdrbcecb_initialize( &(mac->ecb), flow_id, i, &(mac->chan_metrics) );

    mac->reference_count = 1; 
    hdrbcmac.active_channel_count++; 

    /* Add flow to flow table */
    (void) hdrbcmac_insert_flow(flow_id, i);

    rc = hdrbcmac_assign( mac, ovhd_info_ptr );
    HDR_MSG_PROT_1(MSG_LEGACY_HIGH," StackAssign Successful rc=%d",rc);
      
    hdrbcmac.active_flow_count++;  
      
    rc = i;
  }
  
  return rc;  

} /* hdrbcmac_channel_resume */


 /* EJECT */
/*===========================================================================

FUNCTION HDRBCMAC_PROCESS_UPDATE_OVHD_INFO

DESCRIPTION
  Retrieves overhead message. Disables and reenables the existing active 
  BCMCS flows if their configuration is changed by the new overhead info.
  
DEPENDENCIES
  None.

PARAMETERS
  ovhd_info_ptr - Pointer to BCMAC's copy of the broadcast overhead info.  
                  This is used as both an input and output parameter.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void hdrbcmac_process_update_ovhd_info
( 
  hdrbcmac_ovhd_info_type * const ovhd_info_ptr,
  sys_pilot_pn_type               pilot_pn
)
{

  int i, j;
    /* loop variables */

  uint32 changed_flows[HDRBCMAC_BCMCS_FLOWS_MAX] = {0};
    /* FlowIDs of active flows whose configuration has changed */
  
  uint8 changed_flow_chan[HDRBCMAC_BCMCS_FLOWS_MAX];
    /* Channel index of active flows whose configuration has changed */

  int num_changed_flows = 0;
    /* How many flowIDs are in changed_flows[] */

  errno_enum_type err_num;
    /* Return value from BCOVHD when retrieving overhead information */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Determine what has changed about the BroadcastOverhead message
   * and re-assign flows as necessary.
   */
  if (ovhd_info_ptr->is_valid)
  {
    /* Our stored copy of the old overhead info is valid.  Have hdrbcovhd
     * compare its new info with our old stored info to see what changed.
     */
    if (hdrbcovhd_basic_info_has_changed( &ovhd_info_ptr->bc_msg_head ))
    {
      HDR_MSG_PROT(MSG_LEGACY_HIGH,
                   "Basic broadcast channel configuration has changed");

      /* Essentially all the flows have changed */

      for (i = 0; i < HDRBCMAC_BCMCS_FLOWS_MAX; i++)
      {
        if (hdrbcmac.flow_list[i].active)
        {
          changed_flows[num_changed_flows++] = hdrbcmac.flow_list[i].flow_id;
        }
      }
    }
    else /* The basic structure of the broadcast channel is unchanged */
    {
      for (i = 0; i < ovhd_info_ptr->num_flows; i++)
      {
        if (hdrbcovhd_flow_info_has_changed( ovhd_info_ptr->bcmcs_flow_id[i],
                                           &ovhd_info_ptr->log_chan_info[i] ))
        {
          /* The flow info has changed.  Confirm it is an active flow before
           * putting it into the changed_flows list.
           */

          for (j = 0; j < HDRBCMAC_BCMCS_FLOWS_MAX; j++)
          {
            if ((hdrbcmac.flow_list[j].flow_id == 
                   ovhd_info_ptr->bcmcs_flow_id[i]) &&
                (hdrbcmac.flow_list[j].active))
            {
              changed_flows[num_changed_flows++] = 
                ovhd_info_ptr->bcmcs_flow_id[i];

              HDR_MSG_PROT_1( MSG_LEGACY_HIGH, 
                              "Active flow 0x%x config has changed",
                              ovhd_info_ptr->bcmcs_flow_id[i] );

              break;
            }
          }

          if (j == HDRBCMAC_BCMCS_FLOWS_MAX)
          {
            HDR_MSG_PROT_1( MSG_LEGACY_HIGH, "Desired flow 0x%x not active", 
                            ovhd_info_ptr->bcmcs_flow_id[i] );
          }
        }
      }
    }
  }

  if (num_changed_flows == 0)
  {
    HDR_MSG_PROT( MSG_LEGACY_MED,
                  "New BC Overhead message:  No change to active flows" );
  }

  /* Update BCMAC's stored copy of the overhead info */

  err_num = hdrbcovhd_get_bc_msg_head( &ovhd_info_ptr->bc_msg_head );
  if (err_num != E_SUCCESS)
  {
    HDR_MSG_PROT( MSG_LEGACY_ERROR, "No broadcast overhead information" );
  }

  err_num = hdrbcovhd_get_logical_chan_info( HDRBCMAC_BCMCS_FLOWS_MAX,
                                           &ovhd_info_ptr->num_flows,
                                           ovhd_info_ptr->bcmcs_flow_id,
                                           ovhd_info_ptr->log_chan_info,
                                           ovhd_info_ptr->soft_combine_info );

  ovhd_info_ptr->is_valid = TRUE;

  if (err_num != E_SUCCESS)
  {
    HDR_MSG_PROT( MSG_LEGACY_ERROR, "No broadcast overhead information" );
  }
  
  ASSERT( num_changed_flows <= hdrbcmac.active_flow_count );

  /* Remove flows */
  for (i = 0; i < num_changed_flows; i++)
  {
    changed_flow_chan[i] = hdrbcmac_channel_disable( changed_flows[i] );
  }  

  /* Update the scheduler's overhead message info */
  hdrbcslot_update_phy_ch_info( ovhd_info_ptr ); 

  /* Update overhead PN */
  hdrbcslot_update_current_pn( pilot_pn );

  /* Reassign flows */
  for (i = 0; i < num_changed_flows; i++)
  {
    (void) hdrbcmac_channel_resume( changed_flows[i], changed_flow_chan[i]);
  }

  /* Update config */
  hdrbcmac_config_sleep();


} /* hdrbcmac_process_update_ovhd_info() */


 /* EJECT */
/*===========================================================================
 
FUNCTION HDRBCMAC_CHANNEL_ENABLE

DESCRIPTION
  The function enables a new flow in the HDR Broadcast MAC Protocol.
  A flow_id is taken as a parameter to open a new channel.  The return value
  is a handle (or index) for the assigned logical channel.
  
DEPENDENCIES
  None.

PARAMETERS
  flow_id - The desired flow.
  
RETURN VALUE
  logical channel index to use to refer to this channel.
  -1 if flow cannot be enabled.

SIDE EFFECTS
  None.

===========================================================================*/
int hdrbcmac_channel_enable
( 
  uint32 flow_id
)
{
  int rc = -1; 
  uint8 i;
  hdrbcmac_channel_type* mac;
  uint8 flow_available = 0;
  uint8 logical_channel_index = 0xFF;  

  const hdrbcmac_ovhd_info_type * const ovhd_info_ptr = &hdrbcmac.ovhd_info;
    /* Current broadcast overhead information */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Does flow exist already? */
  for ( i=0; i<HDRBCMAC_BCMCS_FLOWS_MAX; i++ )
  {
    if ((hdrbcmac.flow_list[i].active) &&
        (hdrbcmac.flow_list[i].flow_id == flow_id))
    {
       /* the Flow ID already has a channel allocated. */
       HDR_MSG_PROT_1(MSG_LEGACY_HIGH,"Flow %d already enabled", 
                      flow_id );
       return -1;
    }
  }
 
  /* If the flow exists in the current message then find the corresponding 
     logical channel associated. */
  for ( i = 0; i < ovhd_info_ptr->num_flows; i++ )
  {
    if (ovhd_info_ptr->bcmcs_flow_id[i] == flow_id )
    {
      /* this flow can be decoded (is available) */
      flow_available = 1;

      /* Find the logical Ch Index associated with this flow. */
      logical_channel_index = i;

      HDR_MSG_PROT_2(MSG_LEGACY_HIGH," Flow %d Available. LogCh = %d\n", 
                     flow_id, logical_channel_index);
      break;
    }
  }

  /* Let caller track requested/pending flows since we don't know if they
     will be on the same logical channels. */
  if (flow_available == 0)
  {
    /* Flow is not available! */
    return -1;
  }

  /* If flow is available, check to see if any other flow is assigned to the 
     same logical channel & is currently being decoded. If so return with a 
     failed code. */
  for ( i=0; i<HDRBCMAC_MAX_LOGICAL_CHANNELS; i++ )
  {
    if ( hdrbcmac.channel_ptr[i] != 0
         && hdrbcmac.channel_ptr[i]->active == 1)
    {
      if (hdrbcovhd_flows_have_same_logical_channel( 
            flow_id,
            hdrbcmac.channel_ptr[i]->flow_id ))
      {
        HDR_MSG_PROT_2(MSG_LEGACY_MED," LogCh %d For Flow %d already in Use\n", 
                       logical_channel_index, flow_id );

        /* The LogicalChannel associated with the requested BCMCSFlow is 
           already being decoded. */
        hdrbcmac.channel[i].reference_count++; 

        hdrbcmac.active_flow_count++;  

        (void) hdrbcmac_insert_flow(flow_id, i);

        rc = -1;
        return rc;
      }
    }
  }

  /* Look for unused channel and populate it with this flow information. */
  for ( i=0; i<HDRBCMAC_MAX_LOGICAL_CHANNELS; i++ )
  {
    if ( hdrbcmac.channel_ptr[i] == NULL )
    {
      /* Found available channel, fill out structure */
      mac = &hdrbcmac.channel[i];

      hdrbcmac.channel_ptr[i] = mac;
      mac->flow_id = flow_id;
      mac->logical_channel_index = logical_channel_index;
      mac->chan_idx=i;
      hdrbcmac.channel_ptr[i]->active = 0;  //?
      hdrbcecb_initialize( &(mac->ecb), flow_id, i, &(mac->chan_metrics) );

      hdrbcmac.channel[i].reference_count = 1; 
      hdrbcmac.active_channel_count++; 

      /* Add flow to flow table */
      (void) hdrbcmac_insert_flow(flow_id, i);

      rc = hdrbcmac_assign( mac, ovhd_info_ptr );
      HDR_MSG_PROT_1(MSG_LEGACY_HIGH," StackAssign Successful rc=%d",rc);
      
      hdrbcmac.active_flow_count++;  
      
      rc = i;
      break;
    }
  }

  /* Update config */
  hdrbcmac_config_sleep();
  
  return rc;  

} /* hdrbcmac_channel_enable */


 /* EJECT */
/*===========================================================================
 
FUNCTION HDRBCMAC_CHANNEL_DISABLE

DESCRIPTION
  The function disables a flow in the HDR Broadcast MAC Protocol.
  
DEPENDENCIES
  None.

PARAMETERS
  flow_id - The flow to disable.
  
RETURN VALUE
  The channel index of the logical channel being disabled (-1 if no channel
  is being disabled)

SIDE EFFECTS
  None.

===========================================================================*/
int hdrbcmac_channel_disable
(
  uint32 flow_id
)
{
  uint16 i, j, m;     /* Loop counters */
  int rc = -1;        /* Return value: logical channel to disable, or -1 */
  uint8 chan_idx;     /* Logical channel the flow is assigned to */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Find the flow in the flow table. */
  for( j=0; j<HDRBCMAC_BCMCS_FLOWS_MAX; j++ )
  {
    if ((hdrbcmac.flow_list[j].flow_id == flow_id) &&
        (hdrbcmac.flow_list[j].active == TRUE))
    {
      chan_idx = hdrbcmac.flow_list[j].chan_idx;
      
      /* Remove flow listing and decrement reference count */
      hdrbcmac.active_flow_count--;
      hdrbcmac.flow_list[j].active = FALSE;
      hdrbcmac.channel[chan_idx].reference_count--;     

      /* If no flows reference this channel, remove it. */
      if (hdrbcmac.channel[chan_idx].reference_count == 0)
      {
        hdrbcmac.active_channel_count--;

        /* Remove each interlace-multiplex pair assigned to this channel */
        for ( i=0; i<HDRBCMAC_INTERLACE_COUNT; i++ )
        {
          for ( m=0; m<HDRBCMAC_MULTIPLEX_COUNT; m++ )
          {
            if ( ( hdrbcmac.im_chan_map[i][m] != 0 ) &&
                 ( hdrbcmac.im_chan_map[i][m]->chan_idx == chan_idx ) )
            {
              /* Found an interlace+multiplex mapped to this channel. 
                 Deassign it. */
              hdrbcmac_deassign( i, m );
            }
          }
        }

        /* Remove channel  */
        hdrbcmac.channel_ptr[chan_idx] = 0;

        rc = chan_idx;
      }
      break;
    }
  }

  /* Update config */
  hdrbcmac_config_sleep();

  return rc;
  
} /* hdrbcmac_channel_disable */


 /* EJECT */
/*===========================================================================
 
FUNCTION HDRBCMAC_PROCESS_PKT

DESCRIPTION
  Processes a Broadcast MAC packet by placing it in the ECB, and decoding
  if necessary.
  
  Runs in HDR Broadcast task context.

PARAMETERS
  item_ptr - MAC packet to be processed.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrbcmac_process_pkt
(
  dsm_item_type* item_ptr 
    /* Mac packet to be processed */
)
{
  hdrbcecb_state state;
  uint16 interlace, multiplex;
  hdrbcmac_channel_type* mac = NULL;
  uint8 drop_packet = 0; /* if true, drop packet, no ecb processing required */
  
  uint16 bc_pkt_num;
    /* 10-bit broadcast packet number */

  uint16 bc_pkt_info; 
    /* Stores interlace [Bits 10,11], multiplex[Bits 12,13], and 
     packet number [Bits 0-9] in a 16-bit field. */

  void* tmp_ptr;
    /* temporary pointer */

  const hdrbcmac_ovhd_info_type * const ovhd_info_ptr = &hdrbcmac.ovhd_info;
    /* Current broadcast overhead information */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Error checking */
  if (item_ptr == NULL)
  {
    HDR_MSG_PROT(MSG_LEGACY_FATAL,"Got Null Packet"); 
    return;
  }

  /* kss: must be halfword aligned (it is). 
     Use void pointer as a bridge to avoid the following compiler warning: 
     "cast increases required alignment of target type". */
  tmp_ptr = (void *)&(item_ptr->data_ptr[126]);
  bc_pkt_info = *(uint16*)tmp_ptr;

  interlace = (bc_pkt_info >> 10) & 0x3;

#ifdef FEATURE_HDR_BCMCS_2p0
  #error code not present
#else
  multiplex = (bc_pkt_info >> 12) & 0x3;
#endif /* FEATURE_HDR_BCMCS_2p0 */

  bc_pkt_num = (bc_pkt_info & 0x3FF);
  mac = hdrbcmac.im_chan_map[interlace][multiplex];

  HDR_MSG_PROT_3(MSG_LEGACY_MED,"Received BCC packet. (%d i, %d m) Pkt Num=%d", 
                 interlace,
                 multiplex,
                 bc_pkt_num);

  /* See if we should process this packet */
  if ( !ovhd_info_ptr->is_valid )
  {
    /* we have not been configured properly, drop the packet */
    drop_packet = 1;
  }
  else if (ovhd_info_ptr->bc_msg_head.interlace_included[interlace] == 0 )
  {
    /* interlace not enabled, drop packet */
    drop_packet = 1;
  }
  else
  {
    /* packet is on good interlace, multiplex, ready to be processed */

    /* Is channel for this packet enabled? */
    if ( mac == NULL )
    {
      HDR_MSG_PROT(MSG_LEGACY_MED,"Channel not assigned, dropping pkt");
      drop_packet = 1;
    }
  }

  /* Now we know if the packet requires processing or should be dropped */
  if ( drop_packet )
  {

    dsm_free_packet(&item_ptr);

    HDR_MSG_PROT_1(MSG_LEGACY_MED,"Dropped Pkt. Free=%d", 
                   DSM_POOL_FREE_CNT(DSM_DS_SMALL_ITEM_POOL));
  }
  else
  {

    while ( item_ptr )  /* while packet processing required */
    {

      /* Add the packet to associated ECB and determine the state of the 
         ECB. */
      state = hdrbcecb_add_packet( &(mac->ecb), item_ptr, bc_pkt_num);

      switch ( state )
      {
        case HDRBCECB_STATE_COLLECTING:
          /* the ECB is not complete, wait for more packets */

          item_ptr = NULL; 
          break;

        case HDRBCECB_STATE_DONE:
          /* ECB is done, time to pass the stack's ECB up to the higher layer */
          hdrbcmac_send_data( mac );

          item_ptr = NULL; /* (done with packet) */
          break;

        case HDRBCECB_STATE_OUT_OF_SYNC:
          /* We just got a packet that contains data for the specified 
             interlace+multiplex that already exists in the ECB.  So we must 
             early decode the current ECB, reset the ecb, and finally add 
             the new packet to the ecb to start a new decode. */
          hdrbcmac.metrics.ecb_out_of_sync++;

          /* move the ecb data to the higher layers (triggers decode) */
          hdrbcmac_send_data( mac );

          /* Note: item_ptr is not cleared, so we will loop arouind again */
          break;

        case HDRBCECB_STATE_ERROR:
        default:

          HDR_MSG_PROT(MSG_LEGACY_HIGH," ====== ERROR ===== bad state!!!!\n");
          item_ptr = NULL; /* (done with packet) */
      }

    } /* while not done processing packet */

  } 

} /* hdrbcmac_process_pkt */


 /* EJECT */
/*===========================================================================
 
FUNCTION HDRBCMAC_CLEAR_PHY_CHAN_METRICS

DESCRIPTION
  Clears metrics that do not relate to specific channels (interlace-
  multiplex crc counts, for example).

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrbcmac_clear_phy_chan_metrics()
{
  memset( (void*)(&(hdrbcmac.metrics)),0, sizeof(hdrbcmac_static_metrics) );

} /* hdrbcmac_clear_phy_chan_metrics */


 /* EJECT */
/*===========================================================================
 
FUNCTION HDRBCMAC_RESET

DESCRIPTION
  Resets this protocol layer.  The layer will require a new channel map
  inorder to begin processing physical layer packets.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrbcmac_reset()
{
  rex_init_crit_sect( &hdrbcmac.ovhd_info.crit_sect );
  hdrbcmac.ovhd_info.is_valid = FALSE;

} /* hdrbcmac_reset */


/*===========================================================================
 
FUNCTION HDRBCMAC_GET_CHANNEL_METRICS

DESCRIPTION
  Retrieves logical channel metrics for a particular channel.

PARAMETERS 
  chan_idx - index of channel to retrieve metrics for.
  chan_metrics - pointer to metrics data returned.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrbcmac_get_channel_metrics
(
  uint8 chan_idx,
    /* Index of channel to retrieve metrics for */

  hdrlogbc_mac_metrics_type *chan_metrics
    /* Destination for metric data */
)
{
  memcpy( (void*)chan_metrics,
          (void*)&hdrbcmac.channel[chan_idx].chan_metrics,  
          sizeof(hdrlogbc_mac_metrics_type));

} /* hdrbcmac_get_channel_metrics */


 /* EJECT */
/*===========================================================================
 
FUNCTION HDRBCMAC_CLEAR_CHANNEL_METRICS

DESCRIPTION
  Clears logical channel metrics for a particular channel.

PARAMETERS 
  chan_idx - index of channel to clear.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrbcmac_clear_channel_metrics
(
  uint8 chan_idx 
    /* Index of channel to clear */
)
{
  memset( (void*)&hdrbcmac.channel[chan_idx].chan_metrics, 0, 
          sizeof(hdrlogbc_mac_metrics_type));

} /* hdrbcmac_clear_channel_metrics */




 /* EJECT */
/*===========================================================================

FUNCTION HDRBCMAC_QUEUE_PKT

DESCRIPTION
  Queues a physical layer broadcast packet (1,2,3 or 4 mac packets) for 
  processing in the HDR broadcast task. Note that multiple physical
  layer packets should NOT be chained together.
  
  Runs outside HDR Broadcast task context.

PARAMETERS 
  < fill out >

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrbcmac_queue_pkt
(
  dsm_item_type* bc_pkt_ptr,
    /* Broacast packet data */

  int16          slot_time
    /* Time data was received, in slots (mod 256) */
)
{
  uint8 slot_index;
  uint8 num_macs;
  uint8 intr, mux;
   
  hdrbcmac_channel_type* mac;
  dsm_item_type* mac_pkt = NULL;
  dsm_item_type* next_mac_pkt = NULL;

  uint16 bc_pkt_info;
  /* Stores interlace [Bits 10,11], multiplex[Bits 12,13], and 
     packet number [Bits 0-9] in a 16-bit field. */

  uint16 bc_pkt_num;
    /* 10-bit broadcast packet number */

  uint16 bc_im_info;  
    /* Interlace and multiplex info */

  uint64 temp;

  void* tmp_ptr;
    /* temporary pointer */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  slot_index = (slot_time) & 0x7f;

  num_macs = hdrbcslot_dec_info_get_num_mac_pkts(slot_index);

  mac_pkt = bc_pkt_ptr;

  intr = hdrbcslot_bcc_decode_info[slot_index].intr;
  mux  = hdrbcslot_bcc_decode_info[slot_index].mux;
  mac = hdrbcmac.im_chan_map[intr][mux];

  /* Check whether we can do BCMCS sleep. */
  hdrbcslot_track_packet_arrival(slot_time * 2);

  /* Trigger protocols to see if sleep can be invoked. */
  if (hdrbcslot_is_ready_to_sleep())
  {
    hdrind_give_ind(HDRIND_BCMCS_SLEEP_OK, NULL);
  }

  /* Update metrics */
  if (mac_pkt)
  {
    hdrbcmac.metrics.multiplex[intr][mux].crc_pass+=num_macs;

    if (mac!=NULL)
    {
      mac->chan_metrics.packets_crc_passed+=num_macs;
    }
    else
    {
      /* Nothing to do if MAC is NULL */
      dsm_free_packet(&mac_pkt);

      HDR_MSG_PROT(MSG_LEGACY_MED,"Dropped Pkt.");
      return;
    }
  }
  else
  {
    /* For bad CRC, just update metrics and return */
    hdrbcmac.metrics.multiplex[intr][mux].crc_fail+=num_macs;

    if (mac!=NULL)
    {
      mac->chan_metrics.packets_crc_failed+=num_macs;
      mac->crc_failures_per_ecb+=num_macs;
    }

    return;
  }

  memcpy((void*) &(temp),
         (void * )hdrbcslot_bcc_decode_info[slot_index].packet_num,
         sizeof (qc_qword));

  /*----------------------------------------------------------------------
     Making the following Broadcast packet info available at HDRBC task 
     context by replacing the CRC field in the DSM item: 
     1. Packet number needs a max of ten bits [Bits 0-9]. The ECB block 
        size is given by the following formula

        block_size = ( Number of Rows in ECB x MacPacketsPerECBRow ) 

        Maximum Number of Rows in ECB = 32 and 
        Maximum MacPacketPerECBRow = 16. 
        So, the max value for block size is 512. The packet number wraps
        around after two ECBs (after a max of 1024 packets). 
     2. Bits [10-11] - Interlace 
     3. Bits [12-15] - Multiplex
   ----------------------------------------------------------------------*/
  bc_pkt_num = temp % ((uint16) (mac->ecb.block_size * 2));
  bc_im_info = (uint16) ((intr << 10) | (mux <<12));
  
  HDR_MSG_PROT_3(MSG_LEGACY_HIGH, "BCC Rx'd pkt# %d, slot %d. im 0x%x",
                 bc_pkt_num,
                 slot_time,
                 bc_im_info);

  /* Get individual mac packets, separate them and add broadcast info
     to them.  Queue the mac_pkt for processing in hdrbc task context.
  */
  while (mac_pkt != NULL)
  {
    /* Move to the next mac_pkt in the bcc phy_pkt */
    next_mac_pkt = mac_pkt->pkt_ptr;
    mac_pkt->pkt_ptr = NULL;
    
    /* Set used field -- TODO: do this outside library? */
    mac_pkt->used = 125;
    
    /* Store interlace, multiplex, and packet number in 16-bit field
       where CRC & Tail is stored. -- TODO: do this outside library? 
       MAC Layer Packet Format:
       [Pkt: 1002 bits, (FCS: 16 bits, Tail: 6 bits) or (22 bits of Padding)]
       The trailing 22 Bits ([CRC,Tail] or Padding) could be overwritten with
       this information. We use the trailing two bytes (which could comprises
       the Tail and 10 LSBs of CRC or 16 bits of Padding ) */
    bc_pkt_info = bc_pkt_num | bc_im_info;

    /* Use void pointer as a bridge to avoid the following compiler warning: 
       "cast increases required alignment of target type". */
    tmp_ptr = (void *)&(mac_pkt->data_ptr[126]);
    *(uint16*)tmp_ptr = bc_pkt_info;

    bc_pkt_num++;  /* Mode: mod (%) not necessary (see 2-41 of standard) */
   
    /* Queue for processing. */
    hdrbctask_queue_mac_pkt(mac_pkt);

    /* Move to the next mac_pkt in chain. */
    mac_pkt = next_mac_pkt;
  }         

} /* hdrbcmac_queue_pkt */


 /* EJECT */
/*===========================================================================
 
FUNCTION HDRBCMAC_UPDATE_DECODE_INFO

DESCRIPTION
  Update CRC info for broadcast packets and packets that arrive when
  broadcast packets are scheduled.  Note that this info is informational, 
  and doesn't serve a functional purpose.  (May be useful for profiling,
  characterization, etc.)

PARAMETERS 
  str - 
  bcc_flag - 
  crc_flag - 

DEPENDENCIES
  None.

RETURN VALUE
  Returns whether a BCC packet was expected in this slot.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrbcmac_update_decode_info
(
  uint16 str,
  boolean bcc_flag,
  boolean crc_flag // remove in future, not used.
)
{
  uint16 slot_index;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Mask off upper bits to get value % 128 */
  slot_index = (str & 0x7f);

  if (!bcc_flag)
  {
    /* Were we expecting a packet here? */
    if (hdrbcslot_bcc_decode_info[slot_index].valid == TRUE)
    {
      hdrbcslot_bcast_metrics.hdrrx_punctured_pkts++;
    }
  }

} /* hdrbcmac_update_decode_info */



 /* EJECT */
/*===========================================================================
 
FUNCTION HDRBCMAC_GET_MULTIPLEX_COUNT

DESCRIPTION
  Return the current multiplex count (4, 8 or 16).

DEPENDENCIES
  None.

PARAMETERS 
  None.

RETURN VALUE
  The current multiplex count.

SIDE EFFECTS
  None.

===========================================================================*/
int hdrbcmac_get_multiplex_count( void )
{
  return hdrbcmac.ovhd_info.bc_msg_head.multiplex_count;

} /* hdrbcmac_get_multiplex_count */


 /* EJECT */
/*===========================================================================
 
FUNCTION HDRBCMAC_IS_PHYS_CHAN_ACTIVE

DESCRIPTION
  Returns whether a physical channel (a specific interlace-multiplex pair) 
  is currently active.  (Active=being monitored for broadcast packets)

PARAMETERS 
  intr - 
  mux - 

DEPENDENCIES
  None.

RETURN VALUE
  TRUE  - Interlace/multiplex pair channel is active.
  FALSE - Interlace/multiplex pair channel is not active.

SIDE EFFECTS
  None.

===========================================================================*/
boolean hdrbcmac_is_phys_chan_active( uint8 intr, uint8 mux)
{
  return !(hdrbcmac.im_chan_map[intr][mux] == NULL);

} /* hdrbcmac_is_phys_chan_active */


 /* EJECT */
/*===========================================================================
 
FUNCTION HDRBCMAC_GET_METRICS_PTR

DESCRIPTION
  Returns a pointer to the metrics maintained by the MAC.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to structure containing metrics maintained by MAC.

SIDE EFFECTS
  None.

===========================================================================*/
hdrbcmac_static_metrics * hdrbcmac_get_metrics_ptr( void )
{
  return &(hdrbcmac.metrics);

} /* hdrbcmac_get_metrics_ptr */


 /* EJECT */
/*===========================================================================
 
FUNCTION HDRBCMAC_GET_CHANNEL_COUNT

DESCRIPTION
  This function returns the number of active logical channels.
  
DEPENDENCIES
  None.

PARAMETERS
  None.  
  
RETURN VALUE
  Number of active broadcast channels.

SIDE EFFECTS
  None.

===========================================================================*/
uint8 hdrbcmac_get_channel_count( void )
{
  /* Return number of active channels */
  return hdrbcmac.active_channel_count;

} /* hdrbcmac_get_channel_count */


 /* EJECT */
/*===========================================================================
 
FUNCTION HDRBCMAC_GET_FLOW_ID

DESCRIPTION
  This function returns the flow id associated with a logical channel.
  
DEPENDENCIES
  None.

PARAMETERS
  chan_idx - Logical channel
  flow_id  - Pointer to uint32 to place the flow_id value.
  
RETURN VALUE
  TRUE - Logical channel is active and has a flow_id associated with it.
  FALSE - Logical channel is not active.

SIDE EFFECTS
  None.

===========================================================================*/
boolean hdrbcmac_get_flow_id
(
  uint8 chan_idx,
  uint32* flow_id
)
{

  /* Check that the logical channel is active */
  if (hdrbcmac.channel[chan_idx].reference_count)
  {
    *flow_id = hdrbcmac.channel[chan_idx].flow_id;

    return TRUE;
  }
  else
  { 
    return FALSE;
  }

} /* hdrbcmac_get_flow_id */


 /* EJECT */
/*===========================================================================
 
FUNCTION HDRBCMAC_RUN_SCHEDULER

DESCRIPTION
  Updates DSP with broadcast info for next frame.

PARAMETERS
  None.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrbcmac_run_scheduler( void )
{
  hdrbcmac_ovhd_info_type * const ovhd_info_ptr = &hdrbcmac.ovhd_info;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* We need to protect the usage of ovhd_info_ptr because this routine is
   * called in HDRSRCH task contetxt, whereas the overhead info is updated
   * in HDRBC task context. 
   */
   
  rex_enter_crit_sect( &ovhd_info_ptr->crit_sect );

  /* Only run scheduler if we want to receive packets.
     If FTC MAC flow control is enabled, we are running low on buffer
     space so we should disable broadcast. */
  if (hdrfmac_is_flow_ctrl_enabled() == FALSE)
  { 
    hdrbcslot_update_dsp_broadcast_info( ovhd_info_ptr );
  }

  rex_leave_crit_sect( &ovhd_info_ptr->crit_sect );

} /* hdrbcmac_run_scheduler */

 /* EJECT */
/*===========================================================================

FUNCTION HDRBCMAC_PROCESS_CMD

DESCRIPTION
  This function processes the commands for this protocol that were queued
  on the task command queue from a different task context.

DEPENDENCIES
  None.

PARAMETERS
  cmd_ptr - address of command structure.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrbcmac_process_cmd
(
  hdrbcmac_cmd_union_type *cmd_ptr
)
{
  hdrbcmac_ovhd_info_type * const ovhd_info_ptr = &hdrbcmac.ovhd_info;
    /* Current broadcast overhead information */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  HDRTRACE_CMD(HDRHAI_BROADCAST_MAC_PROTOCOL, cmd_ptr->name, 
               cmd_ptr->sender, 0);

  switch ( cmd_ptr->name )
  {
    case HDRBCMAC_UPDATE_OVHD:
      HDR_MSG_PROT( MSG_LEGACY_HIGH, "BCMAC rxed HDRBCMAC_UPDATE_OVHD_CMD");
      /* We need to protect the updating the contents of the ovhd_info_ptr 
       * because it is referenced from HDRSRCH task context via 
       * hdrbcmac_run_scheduler(). We would not want to change it while 
       * HDRSRCH is reading it. We also need to include 
       * hdrbcslot_update_phy_ch_info() in the critical section since 
       * it populates hdrbcslot_bcc_phys_ch_table from ovhd_info_ptr.
       */

      rex_enter_crit_sect( &ovhd_info_ptr->crit_sect );

      /* Process overhead message for MAC. */
      hdrbcmac_process_update_ovhd_info( ovhd_info_ptr,
                                         cmd_ptr->pilot_pn );

      rex_leave_crit_sect( &ovhd_info_ptr->crit_sect );
    break;

    default:
      ERR_FATAL( "Unknown command", cmd_ptr->name, 0, 0 );
    break;
  }

} /* hdrbcmac_process_cmd() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRBCMAC_PROCESS_IND

DESCRIPTION
  This function processes indications given to BCMAC.

DEPENDENCIES
  None

PARAMETERS
  ind_name     - Name of the indication for BCMAC to process
  ind_data_ptr - Any data associated with the indication

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrbcmac_process_ind
( 
  hdrind_ind_name_enum_type ind_name,
  hdrind_ind_data_union_type * ind_data_ptr 
)
{

  switch ( ind_name )
  {
    case HDRIND_LMAC_AWAKE:

      /* If any flows are active, run the scheduler to prepare for
         BCMCS packets. */
      if (hdrbcmac_get_channel_count())
      {
        hdrbcmac_run_scheduler();
      }

      break;

    default:
      ERR_FATAL( "Unknown indication (%d)", ind_name, 0, 0 );
      break;
  }

} /* hdrbcmac_process_ind() */   /*lint !e715 ind_data_ptr not used */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRBCMAC_IND_CB

DESCRIPTION
  This function is registered to be called whenever a protocol gives an
  indication of interest to HDRBCMAC.

DEPENDENCIES
  None

PARAMETERS
  ind_name - Indication to process is a global enum for all possible 
             indications in HDR signaling
  ind_data - Optional data pointer for indication specific data

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrbcmac_ind_cb
( 
  hdrind_ind_name_enum_type ind_name, 
  void *ind_data 
)
{
  /* This protocol runs in the HDRBC task. */
  if (( rex_self() == HDRBC_TASK_PTR ) && !rex_is_in_irq_mode())
  {
    /* Indication is from another protocol in HDRBC task, so consider it
     * an internal indication
     */
    hdrbcmac_process_ind( ind_name, (hdrind_ind_data_union_type *)ind_data );
  }
  else
  {
    hdrbctask_queue_ind( HDRHAI_BROADCAST_MAC_PROTOCOL, ind_name, ind_data );
  }
} /* hdrbcmac_ind_cb() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRBCMAC_GET_OVHD_INFO_PTR

DESCRIPTION
  This function externalizes a pointer to the current overhead information
  stored by HDRBCMAC.

DEPENDENCIES
  This function should only be called from within HDRBC task context.

PARAMETERS

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
const hdrbcmac_ovhd_info_type * hdrbcmac_get_ovhd_info_ptr( void )
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return &hdrbcmac.ovhd_info;

}

/* <EJECT> */
/*===========================================================================

FUNCTION HDRBCMAC_IS_OK_TO_SLEEP

DESCRIPTION
  This function reports on whether HDR BCMCS MAC is ok to 
  sleep.

  BCMCS isn't negotiated or if no flows are requested, this function returns
  TRUE.  If flows are active, this function returns TRUE or FALSE based on 
  the current flow configuration and on whether there are BCC packets 
  expected in the current burst length cycle.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  Boolean - Whether HDR BC MAC is ok to sleep immediately.

SIDE EFFECTS
  None

===========================================================================*/
boolean hdrbcmac_is_ok_to_sleep( void )
{

  boolean ready_to_sleep;
    /* Return value; whether BCMCS air link is ready to sleep. */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if active. */
  if (hdrbcmac_get_channel_count())
  {
    if (hdrbcmac.sleep_enabled)
    {
      ready_to_sleep = hdrbcslot_is_ready_to_sleep();

      HDR_MSG_PROT_1(MSG_LEGACY_HIGH,
                     "BCMCS active; Ready to sleep = %d", 
                     ready_to_sleep);
    }
    else
    {
      ready_to_sleep = FALSE;
    }
  }
  else
  {
    /* Not active: always ready to sleep. */
    ready_to_sleep = TRUE;
  }

  return ready_to_sleep;
}


/*===========================================================================

FUNCTION HDRBCMAC_ENABLE_BCMCS_SLEEP

DESCRIPTION
  This function enables support for BCMCS sleep.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrbcmac_enable_bcmcs_sleep( void )
{
  hdrbcmac.sleep_enabled = TRUE;
}


/*===========================================================================

FUNCTION HDRBCMAC_DISABLE_BCMCS_SLEEP

DESCRIPTION
  This function disables support for BCMCS sleep.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrbcmac_disable_bcmcs_sleep( void )
{
  hdrbcmac.sleep_enabled = FALSE;
}


#endif /* FEATURE_HDR_BCMCS */

