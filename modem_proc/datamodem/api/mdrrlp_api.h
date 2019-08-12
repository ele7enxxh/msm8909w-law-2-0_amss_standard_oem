#ifndef MDRRLP_API_H
#define MDRRLP_API_H
/*===========================================================================

                        M E D I U M   D A T A   R A T E
                     R A D I O   L I N K   P R O T O C O L
                    E X T E R N A L   H E A D E R   F I L E

DESCRIPTION
  This header file contains the interface definition for the Medium Data Rate
  Radio Link Protocol (RLP Type 2) specified in IS-707-A. The implementation
  is backward compatible with the Radio Link Protocol (RLP Type 1) specified
  in IS-707.  The software can be configured at run time to use either RLP
  Type 1 or RLP Type 2.

  For Medium Data Rate operation (RLP Type 2), this implementation supports
  up to 7 forward supplemental channels, for both Rate Set 1 and Rate Set 2.
  Reverse supplemental channels are not supported.

  Only RLP carried as primary traffic is supported. Support for RLP carried
  as secondary or signaling traffic is not provided.

  Copyright (c)2009 - 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/COMMON/vcs/mdrrlp.h_v   1.0   23 Oct 2000 17:32:26   jeffd  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/mdrrlp_api.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
28/10/11   ms      Fix compiler errors when MDR feature is turned off. 
01/31/11   ttv     Removed feature wrapping in api files.
04/01/10   ms      CMI SU level API defeaturization.
09/04/09   ss      New file created for SU level CMI changes.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"

/* Only include this file for MDR builds */
#include "queue.h"
#include "dsm.h"
#include "dsrlp_api.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/


/*---------------------------------------------------------------------------
  Type for RLP traffic types.
---------------------------------------------------------------------------*/
typedef enum
{
  RLP_TRAFFIC_TYPE_PRI = 0,  /* Primary Traffic   */
  RLP_TRAFFIC_TYPE_SEC       /* Secondary Traffic */
} rlp_traf_type_enum_type;

/*---------------------------------------------------------------------------
  Type for Rate Sets. NOTE: Used as an index.
---------------------------------------------------------------------------*/
typedef enum
{
  RLP_RATE_SET_1 = 0,  /* Rate Set 1 (9.6 kbps)  */
  RLP_RATE_SET_2       /* Rate Set 2 (14.4 kbps) */
} rlp_rate_set_enum_type;

/*---------------------------------------------------------------------------
  Type for RLP Types.
---------------------------------------------------------------------------*/
typedef enum
{
  RLP_TYPE_I,  /* IS-707 RLP */
  RLP_TYPE_II  /* IS-707-A (Medium Data Rate) RLP */
} rlp_type_enum_type;


/*---------------------------------------------------------------------------
  Maximum number of forward supplemental channels supported.
---------------------------------------------------------------------------*/
#define RLP_MAX_FWD_SUP        7

/*---------------------------------------------------------------------------
  Type for Forward Traffic Channel Frame Block. This structure is used by
  RXC (i.e. the Mux Layer) to pass up RLP frames received over the forward
  traffic channel. Note that the first element of the fwd_frames[] array is
  always the Fundamental RLP frame, and hence must always be present.
---------------------------------------------------------------------------*/
typedef struct
{
  byte num_sup;                         /* Number of supp RLP frames rx'ed */
  struct
  {
    rlp_rate_enum_type   frame_rate;    /* Frame rate of the rx'ed RLP frame*/
    dsm_item_type       *rlp_item_ptr;  /* Pointer to the rx'ed RLP frame   */
  } fwd_frames[1+RLP_MAX_FWD_SUP];
} rlp_fwd_frame_block_type;

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION RLP_ESTABLISH_SRVC_TYPE

DESCRIPTION
  This function should invoked to establish the service configuration to be
  used for the RLP session. The service configuration includes the Traffic
  Type (primary or secondary), the Rate Set (1 or 2) and the RLP Type (IS-707
  IS-707-A). This function should be invoked at least once at the beginning
  of each RLP session, and subsequently, each time any of the configuration
  parameters changes. This function is typically invoked by MC at the end of
  service negotiation with the base station.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  This function will cause an RLP reset that will reset the RLP state.
===========================================================================*/
void rlp_establish_srvc_type
(
  rlp_traf_type_enum_type    traffic_typ,  /* Primary or Secondary Traffic */
  rlp_rate_set_enum_type     rate_set,     /* Rate Set 1 or Rate Set 2     */
  rlp_type_enum_type         rlp_typ       /* Type 1 = 707, Type 2 = 707-A */
);

/*===========================================================================
FUNCTION RLP_TX_GET_NEXT_FRAME

DESCRIPTION
  This function retrieves the next IS-707 RLP data frame to transmit on the 
  traffic channel. This function can operate on either primary or secondary
  CDMA channels.

DEPENDENCIES
  rlp_init should have been previously invoked to initialize the internal
  RLP data structures. rlp_establish_srvc_type() should have been previously
  invoked to set up the RLP Type, Rate Set and Traffic Type in use.

RETURN VALUE
  An enumerated value representing the CDMA frame rate for the RLP TX frame.

SIDE EFFECTS
  1) The 'flow_enable_func_ptr' function will be invoked each time the ring
     buffer goes below the lo water mark.
  2) The 'empty_rx_buf_func_ptr' function will be invoked each time the ring
     buffer becomes empty.
===========================================================================*/
rlp_rate_enum_type rlp_tx_get_next_frame
(
  byte               **frame_ptr,    /* address of ptr for the TX frame    */
  boolean            primary_frame,  /* Primary channel yes/no indicator   */
  rlp_rate_enum_type allowed_rate    /* required rate (for sig/sec. chans) */
);

/*===========================================================================
FUNCTION      DS_PROCESS_RLP_RX_FRAMES

DESCRIPTION   This function invokes the RLP RX function to process RLP
              frames received over the air, and terminates the call if the
              RLP protocol fails to synchronize. For packet data calls, it
              also maintains the dormancy state based on the return value
              from the RLP RX processing,

              This function should be called from the RX task, for both MDR
              and non-MDR calls.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_process_rlp_rx_frames
(
  rlp_fwd_frame_block_type  *fwd_frame_block,  /* pt to RLP fwd frame block */
  rlp_rate_enum_type        frame_rate,        /* Rate of RLP frame    */
  boolean                   pri_chan,          /* Primary Channel indicator */
  dsm_item_type             *rlp_item_ptr      /* ptr to RLP frame item  */
);

/*===========================================================================

FUNCTION RLP_ESTABLISH_RATE_SET

DESCRIPTION
  This function establishes the rate set to operate RLP. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  This function will cause an rlp_reset that will reset the RLP state.
===========================================================================*/
void rlp_establish_rate_set
(
  rlp_mux_option_type rate_set               /* Rate Set to operate RLP at */
);
#endif /* MDRRLP_API_H */

