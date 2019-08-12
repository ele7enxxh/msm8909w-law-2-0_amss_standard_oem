/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

              H D R   B R O A D C A S T   M A C  P R O T O C O L       
              
                B R O A D C A S T   S L O T   S C H E D U L E R 

GENERAL DESCRIPTION
  This module contains the implementation of the HDR Broadcast MAC
  Protocol slot scheduling algorithm.
    
EXTERNALIZED FUNCTIONS
    
INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2002 - 2010 by Qualcomm Technologies, Inc.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* EJECT */
/*===========================================================================

                           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/bc/src/hdrbcslot.c#1 $
   
when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
12/04/13   sat     Removed unused parameters in F3s.
08/16/10   kss     Fixed bug with softcombine mask generation. 
06/21/10   cnx     Fixed compiler warnings.
09/30/09   wsh     Fixed KW warnings
03/23/09   wsh     CMI: featurized CMI under FEATURE_CMI
02/04/09   etv     Fixed bug with 8/16 multiplex support.
01/30/09   etv     Merged the following changes to main branch
                   Fixed Klocwork errors.
                   Added support for 8/16 Multiplexes
01/27/09   wsh     Merged BCMCS 2.0 to main branch
12/05/08   kss     Replaced INTLOCKs. 
08/07/06   kss     Fixed possible array overrun.
07/10/06   kss     Fix for BCMCS sleep cycle determination.
10/28/05   dna     Add pilot pn as parameter to hdrbcslot_update_phy_ch_info.
08/28/05   dna     (mpa for) Use new interface from hdrbcovhd to retrieve data.
04/04/05   kss     Fixed possible race condition with DSP clock enable check,
                   use #defines for interlace and slots per frame counts.
02/10/05   etv     Fixed lint errors.
02/02/05   etv     Fixed the coding style to match the convention.
11/04/04   kss     Removed hdrbcslot_start/stop functions (no longer needed).
11/03/04   mpa     Removed dependency on std.h (merged with sys.h)
10/20/04   kss     Make sure time is valid before running update.
09/17/04   kss     Use PN from overhead message for decoding broadcast.
08/19/04   kss     Removed extraneous F3 messages.
07/19/04   kss     More cleanup; removed unused code.
07/14/04   kss     Some cleanup; added BC rate definitions.
07/08/04   kss     Removed some "noisy" F3 messages.
07/08/04   kss     Added include of hdrbcmaci.h; removed dependency on 
                   hdrdec.h
07/06/04   kss     Added soft combine enable/disable interface;
                   use lmac to get PN/aset mappings.
07/02/04   mpa     Renamed hdrbcast to hdrbcovhd.  
06/14/04   ss      (mpa for) Updated hdrbcslot_bcc_phys_ch_info_type
06/07/04   kss     Removed DRC rate cap to support DRCs > 6.
04/24/04   kss     Ported from QC Corp.

===========================================================================*/

/* EJECT */
/*===========================================================================

                          INCLUDE FILES FOR MODULE
                     
===========================================================================*/
#include "hdr_variation.h"
#include "qw.h"
#include "hdrlmac.h"
#include "hdrmdspmcr.h"
#include "hdrmdsp.h"
#include "hdrts.h"
#include "hdrbcslot.h"
#include "hdrbcovhd.h"
#include "hdrbcmaci.h"
#include "hdrdebug.h"
#include "hdrbctask.h"
#include "hdrsrchdef.h"
#include "sys.h"

#ifdef FEATURE_CMI
#include "sys_v.h"
#endif


/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

#ifdef FEATURE_HDR_BCMCS

/* Useful definitions */
#define HDRBCSLOT_SLOTS_PER_FRAME   16

typedef struct
{
  boolean enabled;
    /* Whether this interlace is used for broadcast */

  uint16  period;
    /* Period for this interlace. Equal to total burst length for this
       interlace */

  uint16  last_active_mux;
    /* Last active mux after active muxes; used for BCMCS sleep. */

} hdrbcslot_interlace_info_type;

typedef struct
{
  boolean active;

  boolean waiting_for_last_pkt;
    /* Whether the data in this structure is valid and being waited for. */

  boolean ready_to_sleep;
    /* Whether interlace is ready to sleep. */

  uint16  last_hstr;
    /* Last scheduled packet time, in hstr. */
 
} hdrbcslot_active_interlace_status_type;


#ifdef FEATURE_HDR_BCMCS_2p0
#error code not present
#else
qword hdrbcslot_packet_num[SYS_BCMCS_MAX_FLOWS]; 
#endif /* FEATURE_HDR_BCMCS_2p0 */

boolean hdrbcslot_initialized = FALSE;

/* Time in slots that was saved last. */
qword  hdrbcslot_last_saved_time;

/* max interlace period can be 256 * 4 
   interlace period = SumOf ( BurstLengths[i]) * 4 */
hdrbcslot_interlace_info_type 
  hdrbcslot_interlace_info[HDRBCMAC_INTERLACE_COUNT]; 
hdrbcslot_active_interlace_status_type 
  hdrbcslot_active_interlace_status[HDRBCMAC_INTERLACE_COUNT];


/* ModCounter[i] = (currentTime/4) % (InterlacePeriod[i]/4)
   In a slot belonging to interlace i, modCounter[i] gives us 
   which mux we are expecting and what burst# in that mux we
   are expecting. */
uint16 hdrbcslot_interlace_mod_counter[HDRBCMAC_INTERLACE_COUNT] =
{ 
  0,0,0,0
};


hdrbcslot_bcc_phys_ch_info_type 
  hdrbcslot_bcc_phys_ch_table[HDRBCMAC_INTERLACE_COUNT]
                             [HDRBCMAC_MULTIPLEX_COUNT];

boolean hdrbcslot_bcc_ovhd_info_changed = FALSE;

/* Circular buffer with  128 elements of bcc decode information.
   For a bcc packet decoded at slot s, decode information is found
   at index = ( s & 0x7f ).
   This buffer used by the hdrrx task to derive additional information
   about a bcc packet after it has been decoded. 
   With 128 elements, the information will be kept for 128 slots ( 8 frames
   before it is overwritten. So hdrrx task has to look up this information
   within thse 128 slots. Note that this information is updated 1 frame
   before the dsp attempts to decode the packet */
hdrbcslot_bcc_decode_info_type hdrbcslot_bcc_decode_info[16*8];      

boolean hdrbcslot_bcc_soft_comb = TRUE;

/* Holds current PN and related ASP */
uint16 hdrbcslot_current_asp_idx;
uint16 hdrbcslot_current_pn;


/* Struct holds info pertaining to BCMCS sleep */
typedef struct
{
  uint16  sleep_cycle;
    /* BCMCS Sleep cycle supported by current configuration. */

} hdrbcslot_sleep_info_type;
hdrbcslot_sleep_info_type hdrbcslot_sleep_info;


/* Holds BCMCS metrics for logging purposes */
hdrbcslot_bcast_metrics_type hdrbcslot_bcast_metrics;


/* 
   Broadcast ovhd message specifies so called, "broadcast data rate".
   This corresponds to a particular DRC & SlotReduction.
   If slot reduction is 0, then it means the Slot length for this
   packet is what is normally associated with this DRC. If slot
   reduction is set to 1, then it means the SlotLength for this
   packet is one slot less.
  
   CAUTION: If you change the order of these fields
   make sure you change the initialization order.
*/
typedef struct
{ 
  byte slot_len;
  byte drc;
  byte slot_red;
  byte packet_len;
}hdrbcslot_data_rate_to_drc_map_type;

/*-----------------------------------------------------------------------------
 Broadcast ovhd message specifies so called, "broadcast data rate".
 This corresponds to a particular DRC & SlotReduction.
 If slot reduction is 0, then it means the Slot length for this
 packet is what is normally associated with this DRC. If slot
 reduction is set to 1, then it means the SlotLength for this
 packet is one slot less.

 BcastDataRateField     ActualDataRate     Slots      DRC     SlotRed
 -----------------      -------------      -----      ----    -------
   0                         38.4            16        1         0
   1                         76.8             8        2         0
   2                        153.6             4        3         0
   3                        204.8 (new)       3        3         1 * 
   ( 204.8 rate above is really a 153.6 rate with one slot less )
   4                        307.2             2        4         0
   5                        307.2             4        5         0
   6                        409.6             3        5         1 *
   ( 409.6 rate above is really a 307.2 rate (drc 5) with one slot less )
   7                        614.4             1        6         0
   8                        614.4             2        7         0
   9                        921.6             2        8         0
   
 NOTE: These are reduced slot rates
-----------------------------------------------------------------------------*/

/* Broadcast Data rate to DRC mapping */
const hdrbcslot_data_rate_to_drc_map_type hdrbcslot_data_rate_to_drc[16] =
{ 
  { 
    16,  1, 0, 1          /*   38_4_RATE */
  },                      
  {                       
     8,  2, 0, 1          /*   76_8_RATE */
  },                      
  {                       
     4,  3, 0, 1          /*  153_6_RATE */
  },                      
  {                       
     3,  3, 1, 1          /*  204_8_RATE */
  },                       
  {                       
     2,  4, 0, 1          /*  307_2_RATE */
  },
  {
     4,  5, 0, 2          /*  307_2_LONG_RATE */
  },
  {                       
     3,  5, 1, 2          /*  409_6_RATE */
  },                      
  {                       
     1,  6, 0, 1          /*  614_4_RATE */
  },
  {
     2,  7, 0, 2          /*  614_4_LONG_RATE */
  },
  {
     2,  8, 0, 3          /*  921_6_RATE */
  },
  {
     1,  9, 0, 2          /* 1228_8_RATE */
  },
  {
     2, 10, 0, 4          /* 1228_8_LONG_RATE */
  },
  {
     1, 11, 0, 3          /* 1843_2_RATE */
  },
  {
     1, 12, 0, 4          /* 2457_6_RATE */
  },
  {
     0,  0, 0, 0          /* Reserved */
  },
  {
     0,  0, 0, 0          /* Reserved */
  }
};


/*=============================================================================
      Static Function Prototypes
=============================================================================*/

LOCAL void hdrbcslot_update_decode_info
(
  uint8   intr, 
  uint8   mux, 
  uint8   data_rate,
  qword   abs_frm_st_time, 
  uint8   slot_num,
  boolean first_slot,
  boolean sleep_slot
);


/*=============================================================================
                           FUNCTION DEFINITIONS
=============================================================================*/

/*========================================================================
 
FUNCTION HDRBCSLOT_INITIALIZE_PHY_CH_INFO

DESCRIPTION
  Initialize all the fields of bcc_phy_ch_info table to zero ( defaults).
  All interlaces/muxes are disabled.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void hdrbcslot_initialize_phy_ch_info( void )
{
  uint8 intr = 0; /* interlace */
  uint8 mux = 0;  /* multiplex */
  uint8 nbr = 0;  /* neighbor */
 
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
 
  for ( intr = 0; intr < HDRBCMAC_INTERLACE_COUNT; intr++ )
  {
    for ( mux = 0; mux < HDRBCMAC_MULTIPLEX_COUNT; mux++ )
    {
      hdrbcslot_bcc_phys_ch_table[intr][mux].available = FALSE;
      hdrbcslot_bcc_phys_ch_table[intr][mux].log_chan_index = 0xFF;
      hdrbcslot_bcc_phys_ch_table[intr][mux].interested = 0;
      hdrbcslot_bcc_phys_ch_table[intr][mux].drc = 0;
      hdrbcslot_bcc_phys_ch_table[intr][mux].slot_length = 0;
      hdrbcslot_bcc_phys_ch_table[intr][mux].burst_length = 0;
      hdrbcslot_bcc_phys_ch_table[intr][mux].slot_red = FALSE;
      hdrbcslot_bcc_phys_ch_table[intr][mux].num_mac_pkts = 0;

      hdrbcslot_bcc_phys_ch_table[intr][mux].soft_comb_pn_count = 0;

      for ( nbr = 0; nbr < HDRHAI_MAX_SP_NEIGHBORS; nbr++ )
      {
        hdrbcslot_bcc_phys_ch_table[intr][mux].soft_comb_pn_list[nbr] 
           = HDR_NO_PN_OFFSET; 
      }

    }

    hdrbcslot_interlace_info[intr].enabled = FALSE;
    hdrbcslot_interlace_info[intr].period = 0;
  }

  /*  Reset sleep info */
  hdrbcslot_sleep_info.sleep_cycle = 0;
  
} /* hdrbcslot_initialize_phy_ch_info */


 /* EJECT */
/*========================================================================
 *
FUNCTION     HDRBCSLOT_UPDATE_CURRENT_PN

DESCRIPTION
  Updates the current PN (the one we are receiving overhead from).

DEPENDENCIES
  None.

PARAMETERS
  pilot_pn - The current PN.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void hdrbcslot_update_current_pn
( 
  sys_pilot_pn_type         pilot_pn
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  hdrbcslot_current_pn = pilot_pn;

} /* hdrbcslot_update_current_pn */



 /* EJECT */
/*========================================================================
 *
FUNCTION     HDRBCSLOT_UPDATE_PHY_CH_INFO

DESCRIPTION
   Overhead message organizes most of the information on a per Logical 
   channel basis Except, its burst_length info is organized on int, mux basis.
   This function essentially extracts  information from the Overhead
   message and stores it in a separate structure hdrbcslot_bcc_phy_ch_table, 
   which organizes information on a int,mux basis. This organization is 
   important to update the interface with the DSP.

DEPENDENCIES
  None.

PARAMETERS
  ovhd_info_ptr - Pointer to BCMAC's copy of the broadcast overhead info.  

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void hdrbcslot_update_phy_ch_info
( 
  const hdrbcmac_ovhd_info_type *ovhd_info_ptr
)
{
  uint8 log_ch = 0;    /* logical chan index */
  uint8 phys = 0;      /* phys ch index */
  uint8 intr = 0;      /* interlace */
  uint8 mux  = 0;      /* multiplex */
  uint8 bcast_rate;
  uint8 drc  = 0;
  uint8 slot_len = 0;
  const sys_pilot_pn_type * soft_comb_info = NULL;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Make sure the Multiplex count is within bounds to make KW happy.
     The multiplex_count is received from overhead module which parses the 
     BOM message. There are checks in the code that unpacks this message
     to avoid an out-of-bounds multiplex count. So, if multiplex count is
     out-of-bounds here, it could only be due to some internal error (possibly
     a memory corruption). So ASSERT make sense here. */
  ASSERT(ovhd_info_ptr->bc_msg_head.multiplex_count <= 
         HDRBCOVHD_MAX_SUPPORTED_MULTIPLEXES);

  /* Mark overhead as changed. */
  hdrbcslot_bcc_ovhd_info_changed = TRUE;

  /* Initialize everything to Default values - not active */
  hdrbcslot_initialize_phy_ch_info();
  
  /* BURST
     Initialize the burst_length field of all int/mux pairs based on 
     burst_length info in the ovhd message. Other fields shouldn't be 
     initialized. The int/mux pair still remains unavailable.
     PS: Burst Length of an intr/mux is known even though the intr/mux may
     not be assigned to any advertised logical channels. So we initialize 
     burst length here, whereas other fields are initialized while 
     we examine logical channel info. */
  for ( intr=0; intr<HDRBCMAC_INTERLACE_COUNT; intr++ )
  {
    if ( ovhd_info_ptr->bc_msg_head.interlace_included[intr] == TRUE )
    {
      hdrbcslot_interlace_info[intr].enabled = TRUE;

      for ( mux=0; mux<ovhd_info_ptr->bc_msg_head.multiplex_count; mux++ )
      {
        hdrbcslot_bcc_phys_ch_table[intr][mux].burst_length = 
          ovhd_info_ptr->bc_msg_head.burst_length[intr][mux];
        hdrbcslot_interlace_info[intr].period += 
          ovhd_info_ptr->bc_msg_head.burst_length[intr][mux];
      }
      hdrbcslot_interlace_info[intr].period *= HDRBCMAC_INTERLACE_COUNT;
    }
  }

  /* LOGICAL CHANNEL INFO 
     Find out which physChannels ( int,mux pairs) are assigned to a Logical 
     Channel. Then store each physical channel attributes in the corresponding
     entry in the hdrbcslot_bcc_phys_ch_table
     It is important that we retrieve the attributes on a per physical channel 
     basis (interlace- mux pair basis). This info is important for bcast slot
     scheduling. */
  for ( log_ch=0; log_ch< ovhd_info_ptr->num_flows; log_ch++ )
  {
    for ( phys=0; 
         phys<ovhd_info_ptr->log_chan_info[log_ch].phys_chan_count; 
         phys++ )
    {
      /* Get the Pth Intr Mux pair on the Lth Logical channel */
      intr = ovhd_info_ptr->log_chan_info[log_ch].phys_chan[phys].intr;
      mux  = ovhd_info_ptr->log_chan_info[log_ch].phys_chan[phys].mux;

      bcast_rate = ovhd_info_ptr->log_chan_info[log_ch].data_rate;


      /* DRC */
      drc = hdrbcslot_data_rate_to_drc[bcast_rate].drc;

      /* Ensure this is one of the rates that we support */
      if ( drc == 0 )
      {
        HDR_MSG_PROT_1(MSG_LEGACY_ERROR, "Bcast.Unsupported Rate = %d",bcast_rate);

        /* Make this intr, mux pair Not active */
        hdrbcslot_bcc_phys_ch_table[intr][mux].available = FALSE;
        continue;
      }

      hdrbcslot_bcc_phys_ch_table[intr][mux].drc = drc;

      /* *  * * Slot Length */
      slot_len =  hdrbcslot_data_rate_to_drc[bcast_rate].slot_len;

      hdrbcslot_bcc_phys_ch_table[intr][mux].slot_length = slot_len;

      /* * * * Slot Red */
      hdrbcslot_bcc_phys_ch_table[intr][mux].slot_red  =   
         hdrbcslot_data_rate_to_drc[bcast_rate].slot_red;

      /* * * * Num Mac Packets / physical Layer packet */
      hdrbcslot_bcc_phys_ch_table[intr][mux].num_mac_pkts  = 
         hdrbcslot_data_rate_to_drc[bcast_rate].packet_len;
          

      /* * * * BurstLength 
         Burst Length should have been initialized already in ==Burst section. 
         But let us verify it and reinitialize it - just for the sake of 
         completion. */
      if ( ovhd_info_ptr->bc_msg_head.interlace_included[intr] == 0 )
      {
        /* Logical channel is associated with a bunch of intr,mux pairs.
           Ovhd message however is missing burst_length info for one of those
           intr. If this happens then this is a Spec violation or SW bug
           NOTE: intr = interlace */
        HDR_MSG_PROT_1(MSG_LEGACY_ERROR,"OvhdMsg. LogicalCh intr %d missing",intr);

        /* Make this intr, mux pair Not active */
        hdrbcslot_bcc_phys_ch_table[intr][mux].available = FALSE;

        continue;

      }
         
      if ( hdrbcslot_bcc_phys_ch_table[intr][mux].burst_length % 
           hdrbcslot_bcc_phys_ch_table[intr][mux].slot_length != 0 )
      {
        HDR_MSG_PROT_2(MSG_LEGACY_ERROR,"Bcast.Invalid slotLen=%d, bcast_rate=%d ",
                       slot_len, bcast_rate);

        /* Make this intr, mux pair Not active */
        hdrbcslot_bcc_phys_ch_table[intr][mux].available = FALSE;
        continue;

      }

      /*  * * * Softcombine  */
      hdrbcslot_bcc_phys_ch_table[intr][mux].soft_comb_pn_count =
        ovhd_info_ptr->soft_combine_info[log_ch].soft_comb_pn_count;

      /* Soft combine information should be obtained from current logical 
         channel */
      soft_comb_info = 
        ovhd_info_ptr->soft_combine_info[log_ch].soft_comb_pn_list;
 
      memcpy ( hdrbcslot_bcc_phys_ch_table[intr][mux].soft_comb_pn_list, 
               soft_comb_info, 
               HDRHAI_MAX_SP_NEIGHBORS );

      /* * * * If we have come this far, make this intr, mux pair active */
      hdrbcslot_bcc_phys_ch_table[intr][mux].available = TRUE;

      hdrbcslot_bcc_phys_ch_table[intr][mux].log_chan_index = log_ch;

      HDR_MSG_PROT_3 ( MSG_LEGACY_MED,
                       "BcastDebug. intr=%d, mux=%d, active=%d",
                       intr,
                       mux, 
                       hdrbcslot_bcc_phys_ch_table[intr][mux].available );

    } /* physical channel info */

  } /* logical channel info */
} /* hdrbcslot_update_phy_ch_info */


 /* EJECT */
/*=============================================================================
 
FUNCTION  HDRBCSLOT_FIND_CURR_MUX

DESCRIPTION
  Finds the current mux  in the hdrbcslot_bcc_phys_ch_table table.

DEPENDENCIES
  None.

PARAMETERS
  intr - 
  intr_period_offset - 
  mux -
  mux-offset - 
   
RETURN VALUE
  None.
   
SIDE EFFECTS
  None.

=============================================================================*/
void hdrbcslot_bcc_find_curr_mux
( 
   uint8 intr,
   uint16 intr_period_offset,
   uint8* mux,
   uint16* mux_offset
)
{
  *mux = 0;
  *mux_offset = intr_period_offset;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  while( *mux_offset >= 
         (hdrbcslot_bcc_phys_ch_table[intr][*mux]).burst_length )
  {
    *mux_offset = *mux_offset - 
      hdrbcslot_bcc_phys_ch_table[intr][*mux].burst_length;
    (*mux)++;
  }

} /* hdrbcslot_bcc_find_curr_mux */


 /* EJECT */
/*========================================================================
 
FUNCTION  HDRBCSLOT_COMPUTE_BCC_PKT_NUM

DESCRIPTION
  < fill out >

DEPENDENCIES
  None.

PARAMETERS
  no_intr_periods - 
  ovhd_info_ptr - Pointer to overhead info retrieved from hdrbcovhd.c

RETURN VALUE
  None.

=============================================================================*/
void hdrbcslot_compute_bcc_pkt_num
(
  qword no_intr_periods[],
  const hdrbcmac_ovhd_info_type * const ovhd_info_ptr
)
{
  uint8 l,p,intr,mux, m = 0;
  uint8 bcc_num_mac_pkts = 0;
  uint8 bcc_data_rate;
  qword bccPhysPkts, temp;
  uint16 burstPkts, mux_offset, n;


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Compute Logical channel packet number for all the current logical 
     channels. */
  for ( l=0; l< ovhd_info_ptr->num_flows; l++)
  {
    bcc_data_rate = ovhd_info_ptr->log_chan_info[l].data_rate;

    bcc_num_mac_pkts = hdrbcslot_data_rate_to_drc[bcc_data_rate].packet_len;

    qw_set(bccPhysPkts, 0,0);
    for (p=0; p<ovhd_info_ptr->log_chan_info[l].phys_chan_count; 
         p++)
    {
      /* Get the Pth Intr Mux pair on the Lth Logical channel */
      intr = ovhd_info_ptr->log_chan_info[l].phys_chan[p].intr;
      mux  = ovhd_info_ptr->log_chan_info[l].phys_chan[p].mux;

      /* Compute the total number of packets within the burst length of this 
         intr, mux pair */
      burstPkts = hdrbcslot_bcc_phys_ch_table[intr][mux].burst_length /
        hdrbcslot_bcc_phys_ch_table[intr][mux].slot_length;


      /* In the first slot after dsp_write_time, when  interlace, 'intr' occurs
         ------------------------------------------------------
         no_intr_periods[intr]  interlace periods would have elapsed.
         ==> On this intr, mux pair no_intr_periods[intr]*burstPkts physical 
         layer packets would have been already transmitted. */
      qw_mul(temp,no_intr_periods[intr],burstPkts);
      qw_add(bccPhysPkts,bccPhysPkts,temp);

      /* In the first slot after dsp_write_time, when  interlace, 'intr' occurs:
         ------------------------------------------------------
         Find out which multiplex we will be at */
      hdrbcslot_bcc_find_curr_mux(intr, 
                                  hdrbcslot_interlace_mod_counter[intr],
                                  &m,&mux_offset);

      if (m < mux)
      {
        /* This "mux" hasn't occured again within the current interlace 
           period. Do nothing */
      }
      else if (m == mux)
      {
        /* Find out the burstOffset within this mux = muxOffset 
           Find out how many packets would have been transmitted within that 
           muxOffset. */
        n = (mux_offset/hdrbcslot_bcc_phys_ch_table[intr][mux].slot_length);
        qw_inc(bccPhysPkts, n);

        /* If the division had a remainder then add an additional packet,
           This packet has already started transmission. It has not completed 
           but. */
        if(n*hdrbcslot_bcc_phys_ch_table[intr][mux].slot_length < mux_offset)
        {
          qw_inc(bccPhysPkts,1);
        }
      }
      else
      {
        /* This "mux" has already completed within pOffset. Add BurstPkts */
        qw_inc(bccPhysPkts,burstPkts);
      }
    }

    /* This will be the BCC mac packet number associated with the logical 
       channel in the first slot assigned to it after dsp_write_st_time. */
    qw_mul( hdrbcslot_packet_num[l], bccPhysPkts, bcc_num_mac_pkts );

  }

} /* hdrbcslot_compute_bcc_pkt_num */


 /* EJECT */
/*==========================================================================
 
FUNCTION  HDRBCSLOT_BROADCAST_WRITE_DSP

DESCRIPTION
  Fills in 16 slots (one frame) of the BCC buffer. This buffer tells the 
  DSP what types of Broadcast packets to look for in each slot.  The buffer
  spans 32 slots.  

DEPENDENCIES
  None.

PARAMETERS
  dsp_array - Array of words to write to the DSP's BCC buffer.

RETURN VALUE
  None.

SIDE EFFECTS
  None

==========================================================================*/
void hdrbcslot_broadcast_write_dsp 
(
  uint16* bcc_dsp_array,
    /* array of 16 values to write to BCC buffer */

  uint16  bcc_offset
    /* offset into BCC buffer */
)
{

  int i;
    /* Loop variable */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*  Only update if DSP clocks are on. */
  REX_DISABLE_PREMPTION();
  if (hdrmdsp_check_dsp_clk())
  {

    /* Write values to DSP */
    for (i = 0; i < HDRBCSLOT_SLOTS_PER_FRAME; i++)
    {
      /* Use AND to wrap around at 32 */
      HDRMDSP_SET_BCC_DETECT(((i+bcc_offset) & 0x1f), bcc_dsp_array[i]);
    }
  }
  REX_ENABLE_PREMPTION();

} /* hdrbcslot_broadcast_write_dsp */


 /* EJECT */
/*========================================================================
 
FUNCTION     HDRBCSLOT_CONVTO_DSP_BCCDETPKT

DESCRIPTION:
  < fill out >

DEPENDENCIES
  < fill out >

PARAMETERS
  i - 
  m - 
  first_slot - 
  aset_count - 
  aset_pn - 

RETURN VALUE
  < fill out > 

SIDE EFFECTS
  None

=============================================================================*/
uint16 hdrbcslot_convTo_dsp_bccDetPkt
(
  int i, 
  int m, 
  boolean first_slot,
  int aset_count,
  hdrsrch_asp_to_pn_info_type * aset_pn
)
{
  uint16 dspWord = 0x0000;
  uint8  soft_ho_mask = 0;  
    /* bit map for softHoMask. Bit0 represents asp_idx 0 and so on. */

  int n, j;                     
    /* Loop variables */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (hdrbcslot_bcc_soft_comb == TRUE)
  {                       
    /* -- Enable bits in sho_mask for soft_combinable sectors -- */
    for (n=0; n< hdrbcslot_bcc_phys_ch_table[i][m].soft_comb_pn_count; n++ )
    {
      /* Look for PN in Active Set */
      for (j=0; j<aset_count;j++) 
      {
        if (hdrbcslot_bcc_phys_ch_table[i][m].soft_comb_pn_list[n] == 
            aset_pn[j].pn_offset) 
        {
          /* PN found, add ASP to soft combine mask */
          soft_ho_mask |= ( 1 << (aset_pn[j].asp_idx) );
        }
      }
    }
  }

  /* Enable bit in mask for current serving sector.
     If aspindex is n we should set the n th bit of the
     soft_ho_mask (soft_ho_mask bits are 0 - 5). */
  soft_ho_mask |= 1 << (hdrbcslot_current_asp_idx);


  /* If it is a first slot of a packet write the whole dsp word other wise
     write only soft combine mask */
  if (first_slot == TRUE)
  {
    /* 4 Bits @ Pos 0:4 = DRC */
    dspWord |= hdrbcslot_bcc_phys_ch_table[i][m].drc & 0xF;
      
    /* 1 Bit @ Pos 5   = slot_red */
    dspWord |= ( hdrbcslot_bcc_phys_ch_table[i][m].slot_red & 0x1 ) << 4;

  }
   
  /* 6 bits @ Pos 8:13 = soft_ho_mask */
  dspWord |=  ( soft_ho_mask & 0x3F ) << 8;
   
  return dspWord;

} /* hdrbcslot_convTo_dsp_bccDetPkt */


 /* EJECT */
/*==========================================================================
 
FUNCTION  HDRBCSLOT_UPDATE_DSP_BROADCAST_INFO

DESCRIPTION
  Updates DSP with broadcast info for next frame.

DEPENDENCIES
  None.

PARAMETERS
  ovhd_info_ptr - Pointer to overhead info retrieved from hdrbcovhd.c

RETURN VALUE
  None.

SIDE EFFECTS
  None.

==========================================================================*/
void hdrbcslot_update_dsp_broadcast_info
(
  const hdrbcmac_ovhd_info_type * const ovhd_info_ptr
)
{
  qword curr_frm_st_time;
    /* First (starting) slot time of the current frame */

  qword elapsed_time;
    /* time elapsed between PN rolls */

  qword dsp_write_st_time;
    /* Starting slot time, from which we will be writing to the DSP. */

  static const uint8 dsp_write_offset = 12;
    /* Offset time of dsp_write_st_time with respect to curr_frame_st_time */


  hdrsrch_asp_to_pn_info_type aset_pn[6];
    /* For storing PN to ASP mapping info */

  int aset_count;

  dword elapsed_time_dw = 0;

  qword no_intr_periods[HDRBCMAC_INTERLACE_COUNT];

  uint16 dsp_word;
    /* Holds info for each broadcast slot, computed for DSP */

  uint16 offset;
    /* burst offset within a particular mux */
  
  uint8 first_mux[HDRBCMAC_INTERLACE_COUNT];
    /* Mux associated with an interlace at the first time slot for that 
       interlace ( in next frame ) */

  uint8 curr_mux[HDRBCMAC_INTERLACE_COUNT] = {0xff,0xff,0xff,0xff};
    /* Mux in the current slot */

  uint8 burst_offset_curr_mux[HDRBCMAC_INTERLACE_COUNT] = {0xff,0xff,0xff,0xff};
    /* What offset we are at in the current mux. */
  
  byte i,sl; 
    /* loop variables */

  boolean first_slot, sleep_slot;

  uint8 lci; /* Convenience variable for current logical channel index */

  uint16 bcc_array[HDRBCSLOT_SLOTS_PER_FRAME];
    /* Temporary storage for buffer to write to DSP */

  uint16 bcc_offset;
    /* Indicates which slot in the 2-frame (32-slot) DSP BCC buffer 
       to start writing to. */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Get curr_frm_st_time in frame units. */
  if ( hdrts_get_frame_time2(curr_frm_st_time) == FALSE)
  {
    /* Time is not valid, bail. */
    return;
  }

  /* Convert the frame time to slot units. */
  qw_shift(curr_frm_st_time,4);
  
  /* Copy the slot time and increment by offset for DSP */
  qw_equ(dsp_write_st_time, curr_frm_st_time);
  qw_inc(dsp_write_st_time, dsp_write_offset);
  
  /* If it has been more than 1 frame since the last update, we need to 
     recompute timing. */ 
  if (!((!hdrbcslot_initialized) || (hdrbcslot_bcc_ovhd_info_changed)))
  {

    /* Elapsed time calculation, when Initialized == true. */
    qw_sub(elapsed_time,curr_frm_st_time,hdrbcslot_last_saved_time);
    
    /* Time_elapsed should be really small. I can safely assume that it is
       within a dword.  If we get PN Rolls regularly. time_elapsed_dw should 
       be exactly 16. */
    elapsed_time_dw = qw_lo(elapsed_time);
    
    if ( (qw_hi(elapsed_time)!=0) || 
         (elapsed_time_dw > HDRBCSLOT_SLOTS_PER_FRAME) )
    {
      HDR_MSG_PROT_1(MSG_LEGACY_MED, "Skipped PN rolls: %u",
                       elapsed_time_dw / 16);

      hdrbcslot_initialized = FALSE;
    }

  }

  /* Recompute timing if necessary */
  if ( (!hdrbcslot_initialized)  || (hdrbcslot_bcc_ovhd_info_changed) )
  {
    hdrbcslot_bcc_ovhd_info_changed = FALSE;

    /* Reset sleep flags since we skipped some updates. */
    hdrbcslot_reset_sleep_flags();
    
    /* Find the state of mod counter at the first time slot after 
       dsp_write_st_time when the corresponding interlaces begin..  */
    for ( i=0; i<HDRBCMAC_INTERLACE_COUNT; i++ )
    {  
      /* This means none of the muxes in this interlace are enabled. */
      if ( hdrbcslot_interlace_info[i].enabled != FALSE )
      {
        /* hdrbcslot_interlace_mod_counter[i] will have the remainder of this 
           division. no_intr_periods[i] will have the number of Quotient of 
           this division. This quotient will be used in Packet number 
           computation below.Upcoming Slot time for interlace i - Interlace 
           Offset */
        hdrbcslot_interlace_mod_counter[i] = 
          qw_div( no_intr_periods[i], 
                  dsp_write_st_time,
                  hdrbcslot_interlace_info[i].period);
      
        /* Since dsp_write_st_time & interlace_period are multiples of 4
           interlace_mod_counter (remainder of the division ) will also be a 
           multiple of 4. */
        hdrbcslot_interlace_mod_counter[i] /= HDRBCMAC_INTERLACE_COUNT;

      }
      else
      {
        hdrbcslot_interlace_mod_counter[i] = 0;
      }

    }
    
    /* Note that only at the start we compute the state of the mod counter by 
       doing a quad word division. After that, we simply increment the mod 
       counter by the time elapsed. We dont do division any more.
       Compute Logical channel packet number for all the current logical 
       channels and store it. We do this computation only once at startup or 
       when we have to resynchronize. At othertimes packet_num is simply 
       incremented by one. */
    hdrbcslot_compute_bcc_pkt_num( no_intr_periods, ovhd_info_ptr );

    /* Calculate elapsed time. */
    elapsed_time_dw = 0;

    hdrbcslot_initialized = TRUE;

    HDR_MSG_PROT(MSG_LEGACY_MED,"Timing update complete");
    
  }

  qw_equ(hdrbcslot_last_saved_time, curr_frm_st_time);

  /* Find the state of mod counter at the first time slot when their 
     corresponding interlaces begin in next frame. */
  for ( i=0; i < HDRBCMAC_INTERLACE_COUNT; i++ )
  {
    if ( hdrbcslot_interlace_info[i].enabled != FALSE )
    {
      /* Time elapsed is always a multiple of 4 since we get PNRolls every 
         16 slots. Advance mod counter by ( elapsed slots / 4). Mod counters 
         are already offset by their interlace index during initialization.
         elapsed_time is artificially limited to 256. Interlace_period doesnt 
         exceed 256. Hence mod_counter value will not overflow during these 
         arithmetic operations. */
      hdrbcslot_interlace_mod_counter[i] += 
        (uint16)( elapsed_time_dw / HDRBCMAC_INTERLACE_COUNT );

      hdrbcslot_interlace_mod_counter[i] = 
        hdrbcslot_interlace_mod_counter[i] %
        ( hdrbcslot_interlace_info[i].period / HDRBCMAC_INTERLACE_COUNT );

    }
    else
    {
      hdrbcslot_interlace_mod_counter[i] = 0;
    }
  }
  
  /* Once we know the mod counter states we can figure out what mux
     the slots belong to in the next frame.
     Find the mux for each interlace & the bursts remaining for that mux. */
  for ( i = 0; i < HDRBCMAC_INTERLACE_COUNT; i++ )
  {
    if (hdrbcslot_interlace_info[i].enabled != FALSE )
    {
       hdrbcslot_bcc_find_curr_mux( i,
                                    hdrbcslot_interlace_mod_counter[i],
                                    &first_mux[i],
                                    &offset );

      /* To start with the curr_mux of interlace i = first_mux[i]. 
         Curr_mux[i] can change as we iterate through the next 16 slots. */
      curr_mux[i] = first_mux[i];
      burst_offset_curr_mux[i] = (uint8) offset;

    }
  }

  /* Reset current ASP index */
  hdrbcslot_current_asp_idx = 0;
  
  /* Get ASP info */
  aset_count = hdrlmac_get_asp_pn_mapping(aset_pn);
  ASSERT(aset_count <= 6);

  /* Look for current PN in Active Set */
  for ( i=0; i<aset_count; i++ ) 
  {
    if ( hdrbcslot_current_pn == aset_pn[i].pn_offset ) 
    {
      /* PN found, add ASP to soft combine mask */
      hdrbcslot_current_asp_idx = aset_pn[i].asp_idx;
      break;
    }
  }

  /* Calculate where we should start writing in the DSP Array ( 32 elements). 
     We only write 16 elements at a time starting from index 
     (dsp_write_st_time & 0x1f ).  */
  bcc_offset = qw_lo(dsp_write_st_time) & 0x1f;

  for (sl = 0; sl < HDRBCSLOT_SLOTS_PER_FRAME; sl++)
  {
    i = sl % HDRBCMAC_INTERLACE_COUNT;

    first_slot = FALSE;

    sleep_slot = FALSE;

    dsp_word = 0x0000;

    /* Write valid dsp words only when the mux is available with valid slot 
       length... It is absolutely possible for burst length to be non zero, 
       but the physical channel may not be available..In those cases write 
       Zeros until we complete the burst period. */
    if ((hdrbcslot_interlace_info[i].enabled != FALSE ) &&
        (hdrbcslot_bcc_phys_ch_table[i][curr_mux[i]].available == TRUE ))
    {
      
      if ( burst_offset_curr_mux[i] % 
           hdrbcslot_bcc_phys_ch_table[i][curr_mux[i]].slot_length == 0 )
      {
        first_slot = TRUE;


        if ((curr_mux[i] == hdrbcslot_interlace_info[i].last_active_mux) &&
            (((burst_offset_curr_mux[i] + 
              hdrbcslot_bcc_phys_ch_table[i][curr_mux[i]].slot_length ) >= 
              hdrbcslot_bcc_phys_ch_table[i][curr_mux[i]].burst_length)))

        {
          sleep_slot = TRUE;
        }

        hdrbcslot_active_interlace_status[i].ready_to_sleep = FALSE;


      }

      /* We need to write a valid word to DSP only at the beginning of each packet,
         not every slot of the packet. */
      dsp_word = hdrbcslot_convTo_dsp_bccDetPkt( i,
                                                 curr_mux[i], 
                                                 first_slot,
                                                 aset_count,
                                                 aset_pn );
    }
    else
    {
      if (hdrbcslot_active_interlace_status[i].active)
      {
        if ((curr_mux[i] > hdrbcslot_interlace_info[i].last_active_mux) &&
            (!hdrbcslot_active_interlace_status[i].waiting_for_last_pkt))
        {
          hdrbcslot_active_interlace_status[i].ready_to_sleep = TRUE;
        }
        else
        {
          hdrbcslot_active_interlace_status[i].ready_to_sleep = FALSE;
        }
      }
    }

    /* Update decode info */
    lci = hdrbcslot_bcc_phys_ch_table[i][curr_mux[i]].log_chan_index;

    hdrbcslot_update_decode_info ( i, 
                                   curr_mux[i],
                                   ovhd_info_ptr->log_chan_info[lci].data_rate,
                                   dsp_write_st_time,
                                   sl,
                                   first_slot,
                                   sleep_slot );

    /* Save info to write to DSP later */
    bcc_array[sl] = dsp_word;

    if (hdrbcslot_interlace_info[i].enabled == TRUE)
    {
      burst_offset_curr_mux[i]++;

      if (burst_offset_curr_mux[i] >= 
          hdrbcslot_bcc_phys_ch_table[i][curr_mux[i]].burst_length)
      {
        /* We completed one mux. Go to the next mux. */
        curr_mux[i] = 
          (curr_mux[i]+1) % ovhd_info_ptr->bc_msg_head.multiplex_count;
        burst_offset_curr_mux[i] = 0;
      }

    }
  } /* for loop (over 16 slots) */

  /* Write BCC buffer info to DSP */
  hdrbcslot_broadcast_write_dsp(bcc_array, bcc_offset);

} /* hdrbcslot_update_dsp_broadcast_info */


 /* EJECT */
/*=============================================================================
 
FUNCTION  HDRBCSLOT_UPDATE_DECODE_INFO

DESCRIPTION:
   When a broadcast packet is decoded , we need to know the physChan (intr, mux)
   it belongs to absolute time & the PacketNum of the logicalChan associated 
   with the physChan. Collectively we call this BccDecodeInformation. Since we 
   compute BccDecodeInformation AHEAD OF TIME to assist the DSP, we store this 
   information on a per slot basis in the hdrbcslot_bcc_decode_info array for 
   later use when bcc packet is decoded.
   
   Since there could be a time lapse of up to 4 frames from when we compute it 
   (for DSP) to when the packet is decoded, we have to store atleast 5 frames 
   worth of DecodeInformation, which is 5 * 16  = 90 slots. To be conservative 
   we store 128 slots worth of information.
   
   When we store the DecodeInformation for a slot, we store it in an index(=last
   7 bits of absolute time 2 ^ 7 = 128) within the hdrbcslot_bcc_decode_info 
   array. 
   
   When a Bcc packet arrives we look at the hstr and find the 8 bit-slot time, 
   take the last 7 bits of this slot time and index back in to the 
   hdrbcslot_bcc_decode_info array to retrieve the BCCDecodeInfo

DEPENDENCIES
  None.

PARAMETERS
  intr - 
  mux -
  abs_frm_st_time - 
  slot_num - 
  first_slot - 
  data_rate - The data rate for the given intr/mux

RETURN VALUE:
  None.
   
SIDE EFFECTS:
  None.

=============================================================================*/
LOCAL void hdrbcslot_update_decode_info
(
  uint8 intr, 
  uint8 mux, 
  uint8 data_rate,
  qword abs_frm_st_time, 
  uint8 slot_num,
  boolean first_slot,
  boolean sleep_slot
)
{
  uint16 slot_index;
    /* Slot index in to the hdrbcslot_bcc_decode_info array. */
    
  uint16 hstr; 
    /* hstr associated with this slot */
    
  uint8  num_mac_pkts = 0;
  qword  abs_slot_time;

  uint8 log_ch;/* Index of logical channel this intr, mux is mapped to. */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* CAUTION QWORD is a pointer. Modifying it will modify the caller's value 
     also. Only modify it after copying it. */
  qw_equ(abs_slot_time, abs_frm_st_time);
  qw_inc(abs_slot_time, slot_num);
  
  /* compute hstr corresponding to abs_frm_st_time.
     slot time to hstr units - then truncated to 9 bits */
  hstr      = (qw_lo(abs_slot_time) * 2 ) & 0x1ff;

  /* we just need the last 7 bits of slot time (hstr/2) to index in to the 
     decode_info array */
  slot_index = (hstr / 2 ) & 0x7f;

  /* For a multi slot packet only the soft combine bits of the dspword are set. 
     Other bits are set to Zero. Soft combine bits are in the MSByte. */
  if ((hdrbcslot_interlace_info[intr].enabled == TRUE) &&
      (hdrbcslot_bcc_phys_ch_table[intr][mux].available == TRUE) &&
      (first_slot == TRUE))
    
  {
    log_ch = hdrbcslot_bcc_phys_ch_table[intr][mux].log_chan_index;

    hdrbcslot_bcc_decode_info[slot_index].valid = TRUE;
    hdrbcslot_bcc_decode_info[slot_index].intr = intr;
    hdrbcslot_bcc_decode_info[slot_index].mux  = mux;
    hdrbcslot_bcc_decode_info[slot_index].hstr = hstr;
    qw_equ(hdrbcslot_bcc_decode_info[slot_index].packet_num,
           hdrbcslot_packet_num[log_ch]);
    
    /* Track last schduled packet by hstr and slot time. */
    if (sleep_slot)
    {
    
      hdrbcslot_active_interlace_status[intr].last_hstr = hstr;
      hdrbcslot_active_interlace_status[intr].waiting_for_last_pkt = TRUE;
     
    }
    else
    {
      hdrbcslot_active_interlace_status[intr].waiting_for_last_pkt = FALSE;
    }

    /* increment the NEXT EXPECTED packet number. */
    num_mac_pkts =  
        hdrbcslot_data_rate_to_drc[data_rate].packet_len;
    qw_inc( hdrbcslot_packet_num[log_ch], num_mac_pkts );

  }
  else
  {
    hdrbcslot_bcc_decode_info[slot_index].valid = FALSE;
   
  }

} /* hdrbcslot_update_decode_info */


 /* EJECT */
/*=============================================================================
 
FUNCTION  HDRBCSLOT_DEC_INFO_GET_NUM_MAC_PKTS

DESCRIPTION
   Retrieves the number of mac pkts associated with a phy layer packet from
   the dec_info array at index == slot_index

DEPENDENCIES
  None.
 
PARAMETERS
  slot_index - index of dec_info array.
   
RETURN VALUE:
  Number of MAC packes associated with a phy layer packet
   
SIDE EFFECTS:
  None.

=============================================================================*/
uint8 hdrbcslot_dec_info_get_num_mac_pkts(uint16 slot_index)
{

  uint8 intr = hdrbcslot_bcc_decode_info[slot_index].intr;
  uint8 mux  = hdrbcslot_bcc_decode_info[slot_index].mux;
  uint8 num_mac_pkts = hdrbcslot_bcc_phys_ch_table[intr][mux].num_mac_pkts;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  return num_mac_pkts;

} /* hdrbcslot_dec_info_get_num_mac_pkts */


 /* EJECT */
/*===========================================================================
 
FUNCTION HDRBCSLOT_DISABLE_SOFT_COMBINING

DESCRIPTION
  Disables soft combining in broadcast.

PARAMETERS
  None.
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrbcslot_disable_soft_combining( void )
{

  HDR_MSG_PROT(MSG_LEGACY_HIGH,"Broadcast Soft Combining disabled");
  hdrbcslot_bcc_soft_comb = FALSE;

} /* hdrbcslot_disable_soft_combining */


 /* EJECT */
/*===========================================================================
 
FUNCTION HDRBCSLOT_ENABLE_SOFT_COMBINING

DESCRIPTION
  Re-enables soft combining in broadcast (previous disabled by 
  hdrbcslot_disable_soft_combining).

DEPENDENCIES
  None

PARAMETERS
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrbcslot_enable_soft_combining( void )
{

  HDR_MSG_PROT(MSG_LEGACY_HIGH,"Broadcast Soft Combining re-enabled");
  hdrbcslot_bcc_soft_comb = TRUE;

} /* hdrbcslot_enable_soft_combining */



/*===========================================================================

FUNCTION  HDRBCSLOT_CONFIG_SLEEP

DESCRIPTION
  Determines whether sleep can be supported with the current flow
  configuration, and if so, returns the sleep cycle supported. 
  Sleep cycle is specified in units of 426 ms sleep (i.e, Sync Control 
  Channel cycle).
  
DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  Returns the sleep cycle supported by the current configuration.
  0 means sleep is not supported.
  
SIDE EFFECTS
  Ready to sleep flags are reset (to not ok to sleep for active interlaces).

============================================================================*/
uint16 hdrbcslot_config_sleep( void )
{
  boolean sleep_supported = TRUE;  /* Flag to indicate whether sleep can 
                                      be supported at all. */

  uint16 common_sleep_cycle = 0;   /* Sleep cycle common across interlaces. */
  uint16 this_sleep_cycle;         /* Optimal cycle for this interlace */
  uint16 sleep_cycle = 0;          /* Overall optimal sleep cycle. */

  int intr, mux, i;                /* Loop variables. */
  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check each interlace */
  for ( intr=0; intr < HDRBCMAC_INTERLACE_COUNT; intr++ )
  {
    /* Set defaults */
    hdrbcslot_interlace_info[intr].last_active_mux = 0;
    hdrbcslot_active_interlace_status[intr].active = FALSE;
    hdrbcslot_active_interlace_status[intr].ready_to_sleep = TRUE;
    hdrbcslot_active_interlace_status[intr].waiting_for_last_pkt = FALSE;
      
    /* Check if interlace is included in overhead. If not, we can ignore 
       this interlace. */
    if ( hdrbcslot_interlace_info[intr].enabled == TRUE )
    {
      /* Look at each multiplex, going backwards, starting from mux 15... */
      mux = HDRBCMAC_MULTIPLEX_COUNT - 1;

      /* Find the last active mux for this interlace */
      while ((hdrbcmac_is_phys_chan_active(intr,mux) == FALSE) &&
             (mux >= 0))
      {
        mux--;
      }

      /* If any multiplexes are active */
      if (mux >= 0)
      {
        /* Mark interlace as active */
        hdrbcslot_active_interlace_status[intr].active = TRUE;
        hdrbcslot_active_interlace_status[intr].ready_to_sleep = FALSE;

        /* Track the last active mux for this interlace. */
        hdrbcslot_interlace_info[intr].last_active_mux = mux;

        /* If the total period is not a multiple of 256 (and thus
           not aligned with SCC), we can't sleep. 
           Or, if the last multiplex is active, we can't sleep. */
        if (((hdrbcslot_interlace_info[intr].period % 256) != 0) ||
            (mux == (HDRBCMAC_MULTIPLEX_COUNT - 1)))
        {
          sleep_supported = FALSE;

          HDR_MSG_PROT(MSG_LEGACY_HIGH,
                       "BCMCS: Sleep not supported in current config.");
        }
        else
        {  
          /* Find optimal sleep cycle for this interlace. Note that
             period cannot be zero if the interlace is active. */
          this_sleep_cycle = hdrbcslot_interlace_info[intr].period / 256;

          HDR_MSG_PROT_4(MSG_LEGACY_HIGH,
                "BCMCS: Interlace %d; Last active mux: %d; TotalBurst: %d (%d)",
                intr, hdrbcslot_interlace_info[intr].last_active_mux,
                hdrbcslot_interlace_info[intr].period, this_sleep_cycle);

          /* For first active interlace, just assign the value. */
          if (common_sleep_cycle == 0)
          {
            common_sleep_cycle = this_sleep_cycle;
          }
          else
          {
            /* Find the common sleep cycle between this and previous interlaces. 
               This method isn't particularly elegant:  Just start from the 
               smaller of this interlace's sleep cycle and the already found 
               common sleep cycle, and check if each value is evenly divisable
               by both. */
            for (i=MIN(common_sleep_cycle, this_sleep_cycle);i >= 1;i--)
            {
              if (((common_sleep_cycle % i) == 0) && 
                  ((this_sleep_cycle % i) == 0))
              {
                common_sleep_cycle = i;
                break;
              }
            }
          }
        }

      }  
    } /* if (interlace included) */

  } /* for each interlace (intr) */

  /* Compute slot_cycle. 0 means no sleep. */
  if (sleep_supported)
  {
    sleep_cycle = common_sleep_cycle;

    HDR_MSG_PROT_1(MSG_LEGACY_HIGH, "BCMCS: Sleep cycle: %d", sleep_cycle);

  }

  /* Remember sleep cycle. 0 means sleep is not supported */
  hdrbcslot_sleep_info.sleep_cycle = sleep_cycle;

  return sleep_cycle;

} /* hdrbcslot_config_sleep */


/*===========================================================================

FUNCTION  HDRBCSLOT_RESET_SLEEP_FLAGS

DESCRIPTION
  Resets BCMCS-releted internal ok-to-sleep type flags to false.
  
DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

============================================================================*/
void hdrbcslot_reset_sleep_flags( void )
{

  int intr;
    /* Interlace counter. */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Reset the ready_to_sleep flags for each interlace. For interlaces 
     that are not active, preset them to be ok to sleep */
  for (intr = 0; intr < HDRBCMAC_INTERLACE_COUNT; intr++ )
  {

    if (hdrbcslot_active_interlace_status[intr].active)
    { 
      hdrbcslot_active_interlace_status[intr].ready_to_sleep = FALSE;
    }
    else
    {
      hdrbcslot_active_interlace_status[intr].ready_to_sleep = TRUE;
    }
    hdrbcslot_active_interlace_status[intr].waiting_for_last_pkt = FALSE;

  }   

  HDR_MSG_PROT(MSG_LEGACY_HIGH,"BCMCS Sleep flags reset.");

} /* hdrbcslot_reset_sleep_flags */


/*===========================================================================

FUNCTION  HDRBCSLOT_TRACK_PACKET_ARRIVAL

DESCRIPTION
  Tracks the arrival of scheduled packets. This is needed to determine when 
  the AT can sleep (while still monitoring BCMCS flows).

DEPENDENCIES
  None.

PARAMETERS
  pkt_hstr - The 16-bit HSTR of the packet just received.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

============================================================================*/
void hdrbcslot_track_packet_arrival
(
  uint16 pkt_hstr
)
{

  int intr = (pkt_hstr / 2) % HDRBCMAC_INTERLACE_COUNT;
    /* Packet interlace, based on based packet HSTR. */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Check whether we have received the last scheduled packet for this
     cycle (total burst length). */
  if ((hdrbcslot_sleep_info.sleep_cycle != 0) &&
      (hdrbcslot_active_interlace_status[intr].waiting_for_last_pkt && 
      (pkt_hstr == hdrbcslot_active_interlace_status[intr].last_hstr)))
  {
    /* If so, this interlace is ready to sleep. 
       Reset the waiting for packet flag. */
    hdrbcslot_active_interlace_status[intr].ready_to_sleep = TRUE;
    hdrbcslot_active_interlace_status[intr].waiting_for_last_pkt = FALSE;

    HDR_MSG_PROT_1(MSG_LEGACY_HIGH, "Interlace ready to sleep! %d",
                       intr);
  }     

} /* hdrbcslot_track_packet_arrival */


/*===========================================================================

FUNCTION  HDRBCSLOT_IS_READY_TO_SLEEP

DESCRIPTION
  Returns whether the BCMCS air link layer is ready to sleep.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  boolean - TRUE if BCMCS air link is ready to sleep.

SIDE EFFECTS
  None.

============================================================================*/
boolean hdrbcslot_is_ready_to_sleep( void )
{
  boolean rc = TRUE;   
    /* Return value: whether BCMCS is ready to sleep. */

  int intr;
    /* interlace counter */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Check whether each interlace is ready to sleep.  We can only sleep
     if all interlaces agree. */
  for (intr = 0; intr < HDRBCMAC_INTERLACE_COUNT; intr++ )
  {
    if (hdrbcslot_active_interlace_status[intr].ready_to_sleep == FALSE)
    {
      rc = FALSE;
      break;
    }
  }   

  HDR_MSG_PROT_4(MSG_LEGACY_HIGH, "sleep: i0: %d i1: %d i2: %d i3: %d",
                     hdrbcslot_active_interlace_status[0].ready_to_sleep,
                     hdrbcslot_active_interlace_status[1].ready_to_sleep,
                     hdrbcslot_active_interlace_status[2].ready_to_sleep,
                     hdrbcslot_active_interlace_status[3].ready_to_sleep
                     );

  return rc;

} /* hdrbcslot_is_ready_to_sleep */


#endif /* FEATURE_HDR_BCMCS */

