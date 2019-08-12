#ifndef _DSRLP_H
#define _DSRLP_H

/*===========================================================================

  D A T A   S E R V I C E S   R L P   E X T E R N A L   H E A D E R  F I L E

DESCRIPTION
   This file contains external variable and function declarations for the
   Radio Link Protocol.

   This should be included by files which interface with RLP, like the
   MuxLayer and Call Processing.

   Note, order of fields in structs is optimized to save memory in 32bit
   ARM environment. This should be kept in mind when changing the ordering
   of fields.

Copyright (c) 1999-2009 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: O:/src/asw/COMMON/vcs/dsrlp.h_v   1.29   31 Jan 2003 14:39:18   sarithay  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/public/dsrlp.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/28/09   vs      Removed the inclusion of customer.h
07/03/09   ms      New file created as part of CMI effort.
===========================================================================*/

/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "qw.h"
/*===========================================================================

             DEFINITIONS  FOR THE MODULE

===========================================================================*/

/* Maximum number of RLP packets that can be received in a single slot */
#define   DSRLP_MAX_PKTS_IN_SLOT           64

/* Max QN instances required per RLP = MAX_NUM_CARRIERS(3) x MAX_ACTIVE_SETS(6) */
#define   DSRLP_MAX_QN_INSTANCES_PER_RLP   18   

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  Typedef that identifies the enahanced version of RLP.
---------------------------------------------------------------------------*/
typedef enum
{
  DSRLP_VER_MFPA = 0,
  DSRLP_ENHANCED_VER_EMPA,
  DSRLP_ENHANCED_VER_MMPA
} dsrlp_enhanced_ver_enum_type;

#define DSRLP_MAX_NAK_ROUNDS    3
/*---------------------------------------------------------------------------
  Number of RLPs of each version that can be supported And their base indices
---------------------------------------------------------------------------*/
/* Only 1 RLP for 1X is supported */
#define DSRLP_FIXED_SR_ID_INDEX                 1

/* 2 RLPs on Default HDR supported. The rscb indices for HDR are different 
   than the 1X RLP indices
*/
#define DSRLP_HDR_SN_RLP_SR_ID_INDEX    2
#define DSRLP_HDR_AN_AUTH_SR_ID_INDEX     3


/* The size for the MMPA RLP log buffer */
#define DSRLP_HDR_MULTILINK_LOG_BUFF_FWD (DSRLP_MAX_NUM_HDR_MRLP_FWD * DSRSP_NUM_ROUTES)
#define DSRLP_HDR_MULTILINK_LOG_BUFF_REV (DSRLP_MAX_NUM_HDR_MRLP_REV * DSRSP_NUM_ROUTES)

/* The number of subframes for which the MARQ info needs to be stored. */
#define DSRLP_MARQ_NUM_SUBFRAMES 14

/*---------------------------------------------------------------------------
  RLP statistics information definition. This structure contains all of the
  RLP statistical information for the currently active RLP session.
---------------------------------------------------------------------------*/
typedef struct
{
  uint32 at_reset_request_count;   /* # of resets requested by AT           */
  uint32 an_reset_ack_count;       /* # of Acks received from AN            */
  uint32 an_reset_request_count;   /* # times reset was requested by AN     */
  uint32 at_nak_bytes_requested;   /* Total # of bytes requested by AT NAKs */
  uint32 at_nak_records_requested; /* # of NAK records AT sent out */
  uint64 an_nak_bytes_requested;   /* Total # of bytes requested by AN NAKs */
  
  uint64 at_quick_nak_ind_cnt;
  uint64 at_quick_nak_cnt; 	        /* # of bytes requested by AT NAKs */
  uint64 at_delayed_nak_cnt;		      /* # of NAK records AT sent out */
  uint64 at_delayed_nak_data_unit_cnt;

  uint64 rx_dup_fr_cnt;             /* # of received duplicate frames        */
  uint64 rx_dup_frames;             /* # of received duplicate frames        */
  uint32 rx_rexmit_fr_cnt;          /* # of received re-transmitted frames */ 
  uint32 rx_rexmit_frames;			    /* # of received re-transmitted frames (used by empa) */
  uint32 rx_new_data_fr_cnt;        /* total # of received new data frames */ 
  uint32 rx_new_data_frames;        /* total # of received new data frames (used by empa) */ 

  uint32 rx_total_bytes;            /* total # of bytes received           */ 
  uint32 rx_total_frames;           /* total # of frames received          */ 
  uint32 tx_total_bytes;            /* total # of bytes transmitted        */
  uint32 tx_total_frames;           /* total # of frames transmitted        */

  uint32 tx_rexmit_fr_cnt;          /*# transmitted re-transmitted frames  */
  uint32 tx_rexmit_frames;          /*# transmitted re-transmitted frames  (used by empa) */
  uint32 tx_rexmit_data_units;       /*Number of Retransmitted Data Units */
  uint32 tx_new_data_fr_cnt;        /*total #  new transmitted data frames */
  uint32 tx_new_data_frames;        /*total #  new transmitted data frames (used by empa */
  uint32 tx_new_data_units;         /*Number of New Data Units transmitted */

  uint32 rx_first_data_unit;		/* # of RLP frames received with first_data_unit flag on */
  uint32 rx_last_data_unit;			/* # of RLP frames received with last_data_unit flag on */
  uint32 tx_first_data_unit;		/* # of RLP frames transmitted with first_data_unit flag on */
  uint32 tx_last_data_unit;			/* # of RLP frames transmitted with last_data_unit flag on */

  uint32 rx_idle_fr_cnt;           /* # of received idle frames            */
  uint32 rx_fill_fr_cnt;           /* # of received fill frames            */
  uint32 rx_blank_fr_cnt;          /* # of blank frames received           */
  uint32 rx_null_fr_cnt;           /* # of null frames received            */
  uint32 rx_20ms_fr_cnt;           /* # of received Fundamental data frame */
  
  uint32 tx_idle_fr_cnt;           /* # of transmitted idle frames         */
  uint32 tx_20ms_fr_cnt;           /* # of txed Fundamental data frames    */

  uint32 naks_received ;           /* total # of nak frames received       */
  
  uint32 re_xmits_not_found;       /* # of Rexmits not found               */

  uint32 rx_rlp_erasures;          /* # of rlp erasures received           */
  uint32 rx_mux_erasures;          /* # of mux erasures received           */
  uint16 largest_contig_erasure;   /* largest block of consecutive erasures*/

  uint8  resets;                   /* # of resets                          */
  uint8  aborts;                   /* # of aborts    (nak aborts)          */
  qword last_rlp_reset_time;      /* Time when RLP was last reset         */
  uint64 nak_abort_data_units_approx; /* approx data units aborted         */

  uint32 last_rtt;                 /* round trip time                      */
  
  qword reset_timestamp;          /* Timestamp when these stats were reset*/
  
  uint8  RLP_BLOB_used;            /* keep track of how RLP_BLOB is used   */
  uint8  curr_tx_nak_rounds;       /* # tx nak rounds used at this time    */
  
  uint32 tx_naks[DSRLP_MAX_NAK_ROUNDS];
                                   /* single, double etc NAKs sent array   */
  
  uint16 state;                    /* current state of the rlp             */

  uint32 rx_contig_idle_fr_cnt;    /* # of contiguous idle frames received */
  uint32 tx_contig_idle_fr_cnt;    /* # of contig idle frames transmitted  */
  uint32 rx_throuput;              /* thput in bps on fwd link             */
  uint32 tx_throuput;              /* thout in bps on rev link             */

  uint64 num_marq_instances;       /* Count of Marq Instances */ 
  uint64 num_new_data_units_marqed; /* Number of New Data units MARQed */
  uint64 num_retx_data_units_marqed; /* Number of retx Data units MARQed */
  uint64 num_ooo_instances;        /* Number of instances of Out of Order 
                                      transmissions */
  uint64 num_bytes_tx_ooo;         /* Number of bytes transmitted 
                                      out of order */
  uint64 num_spd_instances;        /* Number of instances of a higher layer packet 
                                      being dropped because of TransmitAbortTimer */
  uint64 num_spd_bytes;            /* Number of bytes of a higher layer packet 
                                      dropped because of TransmitAbortTimer */

  uint32 rx_max_reseq_q_size;      /* Max count of elements in reseq_q     */
  uint32 rx_max_nak_abort_q_size;  /* Max count of elements in nak_q       */
  uint32 rx_max_delayed_nak_q_size;/* Max count of elements in delayed_nak_q */
  uint32 rx_max_wm_q_size;         /* Max size in bytes of rx wmk queue */

  uint16 max_tx_q_size;            /* Max Count of tx Queue elements */ 
  uint16 max_retx_q_size;          /* Max Count of retx Queue Elements */
  uint16 max_new_data_handles_used;/* Max New Data Handles used within a slot */
  uint16 max_retx_data_handles_used;/* Max retx data handles used within a slot */

} dsrlp_stats_info_type;

/*===========================================================================
                      PUBLIC FUNCTION DECLARATIONS
===========================================================================*/


/*===========================================================================
FUNCTION DSRLP_GET_STATS

DESCRIPTION
  This function gets an sr_id and then returns a pointer to the appropriate
  structure containing the related statistics.

  Input Parameters:

  sr_id:         used to find the appropriate session control block pointer

  stats_ptr_ptr: is a pointer to a pointer to the stats structure

DEPENDENCIES
  A valid sr_id value must be passed into this function.

  Also the session control block pointed to by the rscb_ptr should have been
  initialized and have valid values as this function uses that structure.

RETURN VALUE
  stats_ptr_ptr:  is the pointer to the pointer to the stats structure
  return TRUE if the *stats_ptr_ptr points to a valid memory location,
  otherwise FALSE

SIDE EFFECTS
  None
===========================================================================*/
boolean dsrlp_get_stats(
  byte                      rscb_index,
  dsrlp_stats_info_type   **stats_ptr_ptr
);


#endif  /* _DSRLP_H */

