
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*==

            B R O A D C A S T   O V E R H E A D   P R O T O C O L 

GENERAL DESCRIPTION
   The Broadcast Overhead Protocol Suite.
   

EXTERNALIZED FUNCTIONS (Global)

EXTERNALIZED FUNCTIONS (Regional)

INITIALIZATION AND SEQUENCING REQUIREMENTS
  The protocol must be initialized after system startup prior to activating 
  the protocol.  The procotol must be activated before Control Channel 
  Broadcast Overhead messages can be processed.

Copyright (c) 2004 - 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

==*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/bc/src/hdrbcovhd.c#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/27/13   cnx     Remove unsupported channel in hash before session negotiation.
05/13/11   wsh     Fixed buffer overrun issue when unpacking phys channel
08/06/09   wsh     Support CMI API changes in sys.h 
03/23/09   wsh     CMI: featurized CMI under FEATURE_CMI
01/30/09   etv     Merged the following changes to main branch
                   Fixed Klocwork errors.
                   Added support for 8/16 Multiplexes
01/27/09   wsh     Merged BCMCS 2.0 onto mainbranch
11/10/08   smd     Replaced errno.h with IxErrno.h  
09/18/08   smd     Added fix for off-target build.
09/18/08   sju     Fixed compiler warning.
01/14/08   mpa     Allow program_id_lsb_length == 0.
                   Fixed some old warnings.
12/12/07   sju     Renamed channel record type structure and system type enum
09/11/07   wsh     Removed lint high warnings
08/16/07   wsh     Removed compiler and lint critical warnings
07/13/07   sju     Changed channel record type to hdr channel record type 
09/05/06   mpa     Fixed F3 message.
04/25/06   mpa/ks  Initialize logical channel info struct before unpacking
04/07/06   dna     Add support for programs with FlowDiscriminatorCount == 0
04/11/06   mpa     Added hdrbcovhd_msg_is_up_to_date().
04/05/06   mpa     Added hdrbcovhd_get_flow_format().
02/13/06   mpa     Added hdrbcovhd_get_program_id_lsbs().
11/07/05   mpa     Fixed Lint error
09/30/05   mpa     Fixed additional cdma channel list crit sect.
09/19/05   mpa     Added hdrbcovhd_is_channel_in_additional_cdma_channel_list()
08/28/05   dna     (mpa for) bcovhd redesign and caching.
11/03/04   mpa     Removed dependency on std.h (merged with sys.h)
09/17/04   kss     Print number of flows in ovhd on F3.
07/22/04   mpa     (ss) Store link on which the ovhd msg was received.
07/21/04   kss     Added accessor function for includeFCS field.
07/14/04   kss     Moved BC rate definitions to hdrbcslot.
07/02/04   mpa     Partial code cleanup.  Added session negotiation. Only
                   keep broadcast overhead related ops, move rest to hdrbccp.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "hdr_variation.h"
#include "customer.h"
#include "comdef.h"
#include "rex.h"
#include "err.h"
#include "msg.h"
#include "dsm.h"
#include "hdrhai.h"
#include "hdrind.h"
#include "IxErrno.h"
#include "sd.h"

#ifdef FEATURE_CMI
#include "sd_v.h"
#endif

#include "hdrbit.h"
#include "hdrdsm.h"
#include "hdrovhd.h"
#include "hdrutil.h"
#include "hdrlog.h"
#include "hdrdebug.h"
#include "hdrtrace.h"
/* <EJECT> */


#ifdef FEATURE_HDR_BCMCS
#include "hdrsrchdef.h"
#include "hdrbcovhd.h"
#include "hdrbcovhdi.h"
#include "hdrlmac.h"

/*=============================================================================

  A layering violation is made here to allow hdrbcovhd to find out the desired
  flow list so that it can only parse out information about desired flows.
  This is done because otherwise the cached information for the 
  BroadcastOverhead message would waste a lot of memory.  Rather than include
  hdrbccp.h here and have a circular include dependency, we just declare this
  one function to be extern from this file.
  
=============================================================================*/
  
/*=============================================================================
FUNCTION HDRBCCP_GET_DESIRED_FLOW_LIST

DESCRIPTION
  This function returns the list of desired flows.
  This function can only be called from hdrbcovhd.c.
  
  
DEPENDENCIES
  None.

PARAMETERS
  flow_id_list_ptr       - Pointer to array of flow_ids of desired flows
                           to be filled in by this function
  max_flow_id_list_size  - Maximum size of flow_id_list

RETURN VALUE
  Number of desired flows found.
  List and count of desired flows returned through parameters
  flow_id_list_ptr and flow_id_list_count_ptr.

SIDE EFFECTS
  None.
=============================================================================*/
extern uint8 hdrbccp_get_desired_flow_list
(
  hdrbcovhd_flow_id_type *flow_id_list_ptr,
  uint8 max_flow_id_list_size
);

/* Which "desired" list a flow was found in.  The "program" list is
 * desired flows that matched only their program (no discriminator).
*/
typedef enum
{
  HDRBCOVHD_DESIRED_FLOW,     /* desired_flow[] */
  HDRBCOVHD_DESIRED_PROGRAM   /* desired_program[] */
} hdrbcovhd_list_enum_type;

/*---------------------------------------------------------------------------
              BROADCAST OVERHEAD Message FIELD EXTRACTION MACROS
  
  Following macros are used for unpacking and extracting the individual
  fields of the BCAST_OVHD message.
---------------------------------------------------------------------------*/

typedef enum
{
  HDRBCOVHD_OVHD_MSG_ID_SIZE     =8,
  HDRBCOVHD_OVHD_OVHD_SIG_SIZE =16, 
  HDRBCOVHD_OVHD_SECTOR_SIG_SIZE =16,
  HDRBCOVHD_OVHD_NBR_COUNT_SIZE  =5,
  HDRBCOVHD_OVHD_MULTIPLEX_COUNT_SIZE =2,
  HDRBCOVHD_OVHD_FRAMING_ENABLED_SIZE =1,
  HDRBCOVHD_OVHD_INCLUDE_FCS_SIZE      =1,
  HDRBCOVHD_OVHD_BCMCS_FLOW_FORMAT_SIZE = 1,
  HDRBCOVHD_OVHD_INTR_INCLUDED_SIZE =1,
  HDRBCOVHD_OVHD_SAME_BURST_LENGTHS_SIZE =1,
  HDRBCOVHD_OVHD_TOTAL_BURST_LENGTH_SIZE =10,
  HDRBCOVHD_OVHD_BURST_LENGTH_SIZE  =4,
  /* Flow format 0 */
  HDRBCOVHD_OVHD_BCMCS_FLOW_ID_LENGTH_SIZE = 2,
  HDRBCOVHD_OVHD_BCMCS_FLOW_COUNT_SIZE =7,
  HDRBCOVHD_OVHD_BCMCS_FLOW_ID_SIZE    = 8, // it could be a multiple of 8 up to 32.
  /* end flow format 0 */

  /* Flow format 1 */
  HDRBCOVHD_OVHD_PROGRAM_COUNT_SIZE = 6,
  HDRBCOVHD_OVHD_PROGRAM_ID_LSB_LENGTH_SIZE = 5,
  /* HDRBCOVHD_OVHD_PROGRAM_ID_LSBS_SIZE = Var Length */ 
  HDRBCOVHD_OVHD_FLOW_DISCRIMINATOR_COUNT_SIZE = 6,
  HDRBCOVHD_OVHD_FLOW_DISCRIMINATOR_LENGTH_SIZE = 3,
  /* HDRBCOVHD_OVHD_FLOW_DISCRIMINATOR_SIZE = Var Length */
  /* end flow format 1 */
  HDRBCOVHD_OVHD_REGISTER_FOR_PAGING_SIZE   =1,
  HDRBCOVHD_OVHD_REGISTER_FOR_DYNAMIC_BROADCAST_SIZE  =1,
  HDRBCOVHD_OVHD_LOGICAL_CHAN_SAME_AS_PREV_SIZE  =1,
  HDRBCOVHD_OVHD_PHYS_CHAN_COUNT_SIZE   =7,
  HDRBCOVHD_OVHD_DATA_RATE_SIZE  =4,       
  HDRBCOVHD_OVHD_OUTER_CODE_SIZE =4,
  HDRBCOVHD_OVHD_MAC_PKTS_PER_ECB_ROW_SIZE =4,
  HDRBCOVHD_OVHD_INTR_SIZE  =2,
  HDRBCOVHD_OVHD_MUX_SIZE   =4,
  HDRBCOVHD_OVHD_SOFT_COMBINE_SAME_AS_PREV_SIZE  =1,
  HDRBCOVHD_OVHD_SOFT_COMBINE_SIZE               =1,
  HDRBCOVHD_OVHD_ADD_CDMA_CHAN_SAME_AS_PREV_SIZE     =1,
  HDRBCOVHD_OVHD_ADD_CDMA_CHAN_COUNT_SIZE     =3,
/* Channel Record Size 24 */
  HDRBCOVHD_OVHD_CHANNEL_RECORD_SIZE =24,
  HDRBCOVHD_OVHD_SYSTEM_TYPE_SIZE    =8, // System type
  HDRBCOVHD_OVHD_CHANNEL_BAND_CLASS_SIZE =5,
  HDRBCOVHD_OVHD_CHANNEL_NUMBER_SIZE =11,
  HDRBCOVHD_OVHD_RESERVED_SIZE      =7
}hdrbcovhd_ovhd_field_size_enum_type;


typedef enum
{
  HDRBCOVHD_REGN_MSG_ID_SIZE     =8,
  HDRBCOVHD_REGN_AUTH_SIGNATURE_INCLUDED_SIZE     =1,
  HDRBCOVHD_REGN_BCMCS_FLOW_ID_LENGTH_SIZE = 2,
  HDRBCOVHD_REGN_BCMCS_FLOW_COUNT_SIZE = 7,
  HDRBCOVHD_REGN_BCMCS_FLOW_ID_SIZE = 8,
  HDRBCOVHD_REGN_RESERVED_SIZE = 7
}hdrbcovhd_registration_field_enum_type;

/* <EJECT> */ 
/*===========================================================================
            GLOBAL DEFINITION
===========================================================================*/
typedef struct
{
  hdrbcovhd_bc_msg_type bc_msg; /* Current bc_msg from ovhd cache
                                 * Includes soft combine info
                                 */
  dsm_item_type        *item_ptr; /* Pointer to packed message */
  rex_crit_sect_type crit_sect; /* Critical Section */
  hdrbcovhd_logical_channel_info_type 
    logical_channel_info[HDRBCOVHD_BCMCS_MAX_FLOWS]; /* Info from current bc_msg */
}hdrbcovhd_data_type;

hdrbcovhd_data_type hdrbcovhd;

/*----------------------------------------------------------  
   MultiplexCount      MultiplexesPerInterface
          0         =>        4
          1         =>        8
          2         =>        16
          3         =>        INVALID
-----------------------------------------------------------*/
hdrbcovhd_multiplex_count_enum_type hdrbcovhd_ovhd_multiplex_count_field_to_value_map[4] = 
{
  HDRBCOVHD_MULTIPLEX_COUNT_4, 
  HDRBCOVHD_MULTIPLEX_COUNT_8, 
  HDRBCOVHD_MULTIPLEX_COUNT_16, 
  HDRBCOVHD_MULTIPLEX_COUNT_INVALID
};



hdrbcovhd_burst_length_enum_type hdrbcovhd_ovhd_burst_length_field_to_value_map[16] = 
{
  HDRBCOVHD_BURST_LENGTH_1 ,
  HDRBCOVHD_BURST_LENGTH_2 ,
  HDRBCOVHD_BURST_LENGTH_3 ,
  HDRBCOVHD_BURST_LENGTH_4 ,
  HDRBCOVHD_BURST_LENGTH_6 ,
  HDRBCOVHD_BURST_LENGTH_8 ,
  HDRBCOVHD_BURST_LENGTH_9 ,
  HDRBCOVHD_BURST_LENGTH_12,
  HDRBCOVHD_BURST_LENGTH_16,
  HDRBCOVHD_BURST_LENGTH_20,
  HDRBCOVHD_BURST_LENGTH_24,
  HDRBCOVHD_BURST_LENGTH_32,
  HDRBCOVHD_BURST_LENGTH_36,
  HDRBCOVHD_BURST_LENGTH_48,
  HDRBCOVHD_BURST_LENGTH_60,
  HDRBCOVHD_BURST_LENGTH_64
};

/* Each logical channel record can have max ~ 368 bits
   ( 368 * 8 )/8 ~= 368  bytes + Up to 20 more bytes for remaining
   fields + 16 bytes for sector_id */
#define HDRBCOVHD_MAX_OVHD_MSG_LENGTH  416

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/* <EJECT> */ 
/*=============================================================================

                      LOCAL FUNCTION DECLARATIONS

=============================================================================*/

/******************************************************************************
**                                                                           **
**  Helper functions to unpack the fields of the BroadcastOverhead message   **
**                                                                           **
******************************************************************************/

/*=============================================================================
FUNCTION HDRBCOVHD_CREATE_BCMCS_FLOW_ID

DESCRIPTION
  This function creates a FlowID as per 3GPP2 C.S0054 Ver 1.0 Section 2.4.4.3.1
  for a BroadcastOverhead message using Format 1.
  
  +----------------+-----------+-------------------+---------------+
  | FlowDisLen (3) | Zeros (n) | ProgramIDLSBs (x) | FlowDis (0-7) |
  +----------------+-----------+-------------------+---------------+
  
  ProgramIDLSBs is of length (x) bits where (x) is 
   
  MIN ((BCMCSFlowIDLength - 3 - FlowDiscriminatorLength), ProgramIDLSBLength)
   
  If required there are (n) pad bits.                                                                  
                    
DEPENDENCIES
  None

PARAMETERS
  flow_id_length_in_bits     - Length of the Flow ID to be created in bits
  flow_discriminator_length  - Length of flow discriminator in bits
  flow_discriminator         - Flow discriminator
  program_id_lsbs            - Program ID LSBs

RETURN VALUE
  The flow ID that was created

SIDE EFFECTS
  None
=============================================================================*/

sys_bcmcs_flow_id_type hdrbcovhd_create_bcmcs_flow_id
(
  uint16 flow_id_length_in_bits,
  uint8  flow_discriminator_length,
  uint8  flow_discriminator,
  uint32 program_id_lsbs
)
{
  sys_bcmcs_flow_id_type bcmcs_flow_id = 0; /* FlowID to be returned */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT( (flow_id_length_in_bits >= 16) && (flow_id_length_in_bits <= 32) );
  ASSERT( flow_discriminator_length <= 7 );

  /* Start with the flow discriminator, at most 7 bits */
  bcmcs_flow_id = flow_discriminator;
  bcmcs_flow_id &= (1 << flow_discriminator_length) - 1;

  /* OR in the Program ID, which can be up to 32 bits.  
   * Some bits may get shifted off the end. 
   */
  bcmcs_flow_id |= (program_id_lsbs << flow_discriminator_length);
  bcmcs_flow_id &= (1 << (flow_id_length_in_bits - 3)) - 1;

  /* Now OR in flow discriminator length and make sure we zero out any
   * upper bits of the flow_id if its overall length is less than 32 bits 
   */
  bcmcs_flow_id |= (flow_discriminator_length << 
                    (flow_id_length_in_bits - 3));
  bcmcs_flow_id &= (1 << flow_id_length_in_bits) - 1;

  return (bcmcs_flow_id);

} /* hdrbcovhd_create_bcmcs_flow_id */

/* <EJECT> */ 
/*=============================================================================
FUNCTION HDRBCOVHD_OVHD_COMPUTE_MULTIPLEX_COUNT

DESCRIPTION
  Broadcast Overhead Message helper function.
  Maps the multiplex count field to its associated value.
    
DEPENDENCIES
  None.

PARAMETERS
  multiplex_count_field

RETURN VALUE
  multiplex_count - mapped multiplex count value

SIDE EFFECTS
  None.
=============================================================================*/
static errno_enum_type hdrbcovhd_ovhd_compute_multiplex_count
(
  uint8 multiplex_count_field,
  hdrbcovhd_multiplex_count_enum_type *multiplex_count
)
{
  errno_enum_type err_no = E_NOT_SUPPORTED;
    /* Error number */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If unpacked value from message is with-in bounds */
  if ( multiplex_count_field <= 2 )
  {
    /* Use the translation table to get the Multiplex count */
    *multiplex_count =
        hdrbcovhd_ovhd_multiplex_count_field_to_value_map[multiplex_count_field];

    /* Check for valid Multiplex count  */
    if (*multiplex_count > HDRBCOVHD_MAX_SUPPORTED_MULTIPLEXES)
    {
      /* Not Valid */
      HDR_MSG_PROT_1( MSG_LEGACY_ERROR, "multiplex_count %d not supported",
                      *multiplex_count );
    }
    else
    {
      /* Valid! Set the error code to success */
      err_no = E_SUCCESS;

    } /* If translated value is with-in bounds */

  } /* if unpacked value is with-in bounds */

  return err_no;

} /* hdrbcovhd_ovhd_compute_multiplex_count() */
                                              
/* <EJECT> */ 
/*=============================================================================
FUNCTION HDRBCOVHD_OVHD_DEDUCE_FRAMING_TYPE

DESCRIPTION
  Broadcast Overhead Message Unpacking helper function.
  Returns the framing type.
  
DEPENDENCIES
  None.

PARAMETERS
  framing_enabled_field -

RETURN VALUE
  framing_type -

SIDE EFFECTS
  None.
=============================================================================*/
static errno_enum_type hdrbcovhd_ovhd_deduce_framing_type
(
  uint8 framing_enabled_field,
  hdrbcovhd_framing_type* framing_type
)
{
  if ( framing_enabled_field == 1 )
  {
    *framing_type = HDRBCOVHD_FRAMING_SEGMENT;
  }
  else if ( framing_enabled_field == 0 )
  {
    *framing_type = HDRBCOVHD_FRAMING_HDLC;
  }
  else
  {
    return E_DATA_INVALID;
  }

  return E_SUCCESS;
} /* hdrbcovhd_ovhd_deduce_framing_type() */

/* <EJECT> */ 
/*=============================================================================
FUNCTION HDRBCOVHD_OVHD_COMPUTE_BURST_LENGTH

DESCRIPTION
  Broadcast Overhead Message helper function.  Computes the burst length.
  
DEPENDENCIES
  None.

PARAMETERS
  burst_length_field - 

RETURN VALUE
  burst_length - 
  (returned through param)

SIDE EFFECTS
  None.
=============================================================================*/
static errno_enum_type hdrbcovhd_ovhd_compute_burst_length
(
  uint8                             burst_length_field,
  hdrbcovhd_burst_length_enum_type* burst_length_ptr
)
{
  *burst_length_ptr = HDRBCOVHD_BURST_LENGTH_INVALID;

  if ( burst_length_field <= 15 )
  {
    *burst_length_ptr = 
    hdrbcovhd_ovhd_burst_length_field_to_value_map[burst_length_field];
    return E_SUCCESS;
  }
  else
  {
    return E_DATA_INVALID;
  }
} /* hdrbcovhd_ovhd_compute_burst_length() */

/* <EJECT> */ 
/*=============================================================================
FUNCTION HDRBCOVHD_UNPACK_LOGICAL_CHANNEL_INFO

DESCRIPTION
  Helper

DEPENDENCIES
  None

PARAMETERS
  item_ptr       - DSM item pointer to received message
  info_ptr       - Pointer where to place unpapcked info
  logical_channel_offset - logical channel offset

RETURN VALUE
  E_SUCCESS if the flow record was parsed successfully
  E_NO_DATA if there was an error extracting a field
  
  If E_SUCCESS is returned, info_ptr has the logical channel info for the 
  given flow.  Otherwise info_ptr may have inconsistent data.

SIDE EFFECTS
  None
=============================================================================*/

static errno_enum_type hdrbcovhd_unpack_logical_channel_info
(
   dsm_item_type                       *item_ptr,
   hdrbcovhd_logical_channel_info_type *info_ptr,
   uint16                              logical_channel_offset
)
{
    
  errno_enum_type err_number = E_SUCCESS; /* Error code returned during unpacking */
  int err = 0; /* ORed return values from unpack calls */
  uint16 offset; /* Keep offset local and return it at the end */
  int phy; /* index into physical channel array */
  
  /* Variables to hold key values that determine variable length */
  uint8 phys_chan_count;           /* Physical channel count */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  rex_enter_crit_sect( &hdrbcovhd.crit_sect );

  offset = logical_channel_offset;
  /*-----------------------------------------------------------------------
    PhysicalChannelCount
  -----------------------------------------------------------------------*/
  err |= HDRBIT_UNPACK8( item_ptr,
                         &phys_chan_count,
                         offset, 
                         HDRBCOVHD_OVHD_PHYS_CHAN_COUNT_SIZE );

  offset += HDRBCOVHD_OVHD_PHYS_CHAN_COUNT_SIZE;

  if (phys_chan_count > HDRBCOVHD_MAX_PHYSICAL_CHANNEL_COUNT)
  {
    rex_leave_crit_sect( &hdrbcovhd.crit_sect );

    HDR_MSG_PROT_1(MSG_LEGACY_ERROR,
                   "Phys chan exceeded max %d", phys_chan_count);

    return E_DATA_INVALID;
  }

  info_ptr->phys_chan_count = phys_chan_count;

  if ((err == 0) && (phys_chan_count != 0))
  {
    /*---------------------------------------------------------------------
      DataRate
    ---------------------------------------------------------------------*/
    err |= HDRBIT_UNPACK8( item_ptr,
                           &info_ptr->data_rate,
                           offset, 
                           HDRBCOVHD_OVHD_DATA_RATE_SIZE );
    offset += HDRBCOVHD_OVHD_DATA_RATE_SIZE;

    /*---------------------------------------------------------------------
      OuterCode
    ---------------------------------------------------------------------*/
    err |= HDRBIT_UNPACK8( item_ptr,
                           &info_ptr->outer_code,
                           offset, 
                           HDRBCOVHD_OVHD_OUTER_CODE_SIZE );
    offset += HDRBCOVHD_OVHD_OUTER_CODE_SIZE;

    /*---------------------------------------------------------------------
      MACPacketsPerECBRow
    ---------------------------------------------------------------------*/
    err |= HDRBIT_UNPACK8( item_ptr,
                           &info_ptr->mac_pkts_per_ecb_row,
                           offset, 
                           HDRBCOVHD_OVHD_MAC_PKTS_PER_ECB_ROW_SIZE );
    offset += HDRBCOVHD_OVHD_MAC_PKTS_PER_ECB_ROW_SIZE;

    /* This field is set to one less than the total MAC Packets / ECB row. 
       So add 1. */
    info_ptr->mac_pkts_per_ecb_row += 1;

    /*---------------------------------------------------------------------
      Interlace and Multiplex for each physical channel
    ---------------------------------------------------------------------*/
    for ( phy = 0; phy < phys_chan_count; phy++ )
    {
      /*---------------------------------------------------------------------
        Interlace
      ---------------------------------------------------------------------*/

      err |= HDRBIT_UNPACK8( item_ptr,
                             &info_ptr->phys_chan[phy].intr,
                             offset,
                             HDRBCOVHD_OVHD_INTR_SIZE);
      offset += HDRBCOVHD_OVHD_INTR_SIZE;

      /*---------------------------------------------------------------------
        Multiplex
      ---------------------------------------------------------------------*/
      err |= HDRBIT_UNPACK8( item_ptr,
                             &info_ptr->phys_chan[phy].mux,
                             offset,
                             HDRBCOVHD_OVHD_MUX_SIZE );
      offset += HDRBCOVHD_OVHD_MUX_SIZE;
    }
  } /* if (phys_chan_count != 0) */

  rex_leave_crit_sect( &hdrbcovhd.crit_sect );

  if (err != 0)
  {
    err_number = E_NO_DATA;
  }

  return err_number;

} /* hdrbcovhd_unpack_logical_channel_info() */

/* <EJECT> */ 
/*=============================================================================
FUNCTION HDRBCOVHD_UNPACK_SOFT_COMBINE_INFO

DESCRIPTION
  Helper

DEPENDENCIES
  None

PARAMETERS
  item_ptr       - DSM item pointer to received message
  sp_msg_ptr     - Pointer to SP message in cache
  neighbor_count - Number of neighbors in corresponding SectorParameters msg
  info_ptr       - Pointer where to place unpapcked info
  soft_combine_offset - soft combine offset

RETURN VALUE
  E_SUCCESS if the flow record was parsed successfully
  E_NO_DATA if there was an error extracting a field
  E_DATA_INVALID if there were more soft combine neighbors than expected
  
  If E_SUCCESS is returned, info_ptr has the soft combine neighbor info for the 
  given flow.  Otherwise info_ptr may have inconsistent data.

SIDE EFFECTS
  None
=============================================================================*/

errno_enum_type hdrbcovhd_unpack_soft_combine_info
(
   dsm_item_type         *item_ptr,
   hdrovhd_sp_msg_type   *sp_msg_ptr,
   uint8                 neighbor_count,
   hdrbcovhd_soft_combine_info_type *info_ptr,
   uint16                soft_combine_offset
)
{
    
  errno_enum_type err_number = E_SUCCESS; /* Error code returned during unpacking */
  int err = 0; /* ORed return values from unpack calls */
  uint16 offset; /* Keep offset local and return it at the end */
  int nbr_idx; /* index of neighbor in soft combine list */
  sys_link_type bc_link; /* Link BroadcastOverhead message was received on */
  
  /* Variables to hold key values that determine variable length */
  uint8 soft_combine_nbr;          /* If we can soft combine this neighbor */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  rex_enter_crit_sect( &hdrbcovhd.crit_sect );

  offset = soft_combine_offset;
  /*--------------------------------------------------------------------
  Get channel that BroadcastOverhead message was received on.
  --------------------------------------------------------------------*/
  hdrlmac_get_link_info( (HDRDSM_GET_LINK_ID( item_ptr )), &bc_link );

  info_ptr->soft_comb_pn_count = 0;

  for (nbr_idx = 0; nbr_idx < neighbor_count; nbr_idx++)
  {
    /*------------------------------------------------------------------
      SoftCombine
    ------------------------------------------------------------------*/
    /* unpack 1 bit*/
    err |= HDRBIT_UNPACK8( item_ptr,
                           &soft_combine_nbr,
                           offset, 
                           HDRBCOVHD_OVHD_SOFT_COMBINE_SIZE );
    offset += HDRBCOVHD_OVHD_SOFT_COMBINE_SIZE;

    if (err != 0) { break; }

    if (nbr_idx > sp_msg_ptr->num_neighbors - 1)
    {
      HDR_MSG_PROT( MSG_LEGACY_ERROR, "Too many SoftCombine neighbors!");
      err_number = E_DATA_INVALID;
      break;
    }

    if (soft_combine_nbr == 1)
    {
      if (SYS_CHAN_CMP_EQUAL( bc_link.channel,
                              sp_msg_ptr->nset[nbr_idx].link.channel ))
      {
        info_ptr->soft_comb_pn_list[info_ptr->soft_comb_pn_count] = 
          sp_msg_ptr->nset[nbr_idx].link.pilot_pn;
        info_ptr->soft_comb_pn_count++;
      }
      else
      {
        HDR_MSG_PROT_4( MSG_LEGACY_HIGH, 
          "SoftCombine neighbor (%d,%d) not on current frequency (%d->%d)",
          nbr_idx, sp_msg_ptr->nset[nbr_idx].link.pilot_pn, 
          bc_link.channel.chan_num, 
          sp_msg_ptr->nset[nbr_idx].link.channel.chan_num );
      }
    }
  } /* for (nbr_idx) */

  rex_leave_crit_sect( &hdrbcovhd.crit_sect );

  if (err != 0)
  {
    err_number = E_NO_DATA;
  }

  return err_number;

} /* hdrbcovhd_unpack_soft_combine_info() */

/* <EJECT> */ 
/*=============================================================================
FUNCTION HDRBCOVHD_OVHD_SKIP_TO_NEXT_FLOW

DESCRIPTION
  Helper

DEPENDENCIES
  None

PARAMETERS
  item_ptr        - DSM item pointer to receive message
  neighbor_count - Number of neighbors in corresponding SectorParameters msg
  is_first_flow  - TRUE if this is the first flow record in the message
  offset_ptr     - Pointer to return the new message offset in
  logical_channel_offset_ptr         - logical channel offset
  soft_combine_offset_ptr            - soft combine offset
  additional_cdma_channel_offset_ptr - additional cdma channel offset

RETURN VALUE
  E_SUCCESS if the flow record was parsed successfully
  E_NO_DATA if there was an error extracting a field
  E_DATA_INVALD if something was "same as previous" for the first flow record
  
  If E_SUCCESS is returned, the bit offset of the next field after this flow
  record is returned via offset_ptr.
  
  If this flow record contained actual information about logical channels, soft
  combine pilots, or additional CDMA channels, then the bit offsets of where 
  this information is located witin the message are returned via 
  logical_channel_offset_ptr, soft_combine_offset_ptr and 
  additional_cdma_channel_offset_ptr.  
  
  If this flow record has the "same as previous" bit set for one of these 
  items, then the corresponding offset pointer is not modified.

SIDE EFFECTS
  None
=============================================================================*/

static errno_enum_type hdrbcovhd_ovhd_skip_to_next_flow
(
   dsm_item_type         *item_ptr,
   uint8                 neighbor_count,
   boolean               is_first_flow,
   uint16                *offset_ptr,
   uint16                *logical_channel_offset_ptr,
   uint16                *soft_combine_offset_ptr,
   uint16                *additional_cdma_channel_offset_ptr
)
{
    
  int err = 0; /* ORed return values from unpack calls */
  uint16 offset = *offset_ptr; /* Keep offset local and return it at the end */
  
  /* Previous logical channel had SoftCombineSameAsPreviousLogicalChannel 
   * field included :) */
  boolean plc_had_sc_info;
  static uint8 last_phys_chan_count = 0; /* Remember last PhysChannelCount */

  /* Variables to hold key values that determine variable length */
  uint8 logical_chan_same_as_prev; /* Logical channel same as previous */
  uint8 phys_chan_count;           /* Physical channel count */
  uint8 soft_comb_same_as_prev;    /* Soft combine same as previous */
  uint8 add_cdma_chan_same_as_prev;/* Additional CDMA channels same as prev */
  uint8 add_cdma_chan_count;       /* Additional channel count */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    
  if (is_first_flow)
  {
  }
  /*-------------------------------------------------------------------------
     RegisterForPaging and RegisterForDynamicBroadcast
  -------------------------------------------------------------------------*/
  offset += HDRBCOVHD_OVHD_REGISTER_FOR_PAGING_SIZE;
  offset += HDRBCOVHD_OVHD_REGISTER_FOR_DYNAMIC_BROADCAST_SIZE;

  /*-------------------------------------------------------------------------
     LogicalChannelSameAsPreviousBCMCSFlow 
  -------------------------------------------------------------------------*/
  err |= HDRBIT_UNPACK8( item_ptr,
                         &logical_chan_same_as_prev,
                         offset, 
                         HDRBCOVHD_OVHD_LOGICAL_CHAN_SAME_AS_PREV_SIZE );

  offset += HDRBCOVHD_OVHD_LOGICAL_CHAN_SAME_AS_PREV_SIZE;

  if (err != 0){ return E_NO_DATA; }

  /* For the first bcmcs flow the logical_chan_same_as_prev
  should be Zero. */
  if ((is_first_flow) && (logical_chan_same_as_prev == 1))
  {

    HDR_MSG_PROT( MSG_LEGACY_ERROR, "No logical chan info for first flow!");
    return E_DATA_INVALID;
  }

  if (logical_chan_same_as_prev == 0)
  {
    *logical_channel_offset_ptr = offset;
    /*-----------------------------------------------------------------------
      PhysicalChannelCount
    -----------------------------------------------------------------------*/
    err |= HDRBIT_UNPACK8( item_ptr,
                           &phys_chan_count,
                           offset, 
                           HDRBCOVHD_OVHD_PHYS_CHAN_COUNT_SIZE );

    offset += HDRBCOVHD_OVHD_PHYS_CHAN_COUNT_SIZE;

    if (err != 0){ return E_NO_DATA; }

    if (phys_chan_count > HDRBCOVHD_MAX_PHYSICAL_CHANNEL_COUNT)
    {
      HDR_MSG_PROT_1(MSG_LEGACY_ERROR,
                     "Phys chan exceeded max %d", phys_chan_count);
      return E_DATA_INVALID;
    }

    if (is_first_flow)
    {
      /* Keep track of the PhysicalChannelCount of the last record that 
       * had LogicalChannelSameAsPrevious = 0, to make sure
       * SoftCombineSameAsPreviousLogicalChannel field is used correctly.
       */
      plc_had_sc_info = FALSE;
    }
    else
    {
      /* If this is not the first flow (which must contain a logical channel
       * record), then the previous logical channel will have contained 
       * a SoftCombineSameAsPreviousLogicalChannel field if it had at least
       * one physical channel.  Otherwise it did not.
       */
      plc_had_sc_info = (last_phys_chan_count == 0) ? FALSE : TRUE;
    }

    last_phys_chan_count = phys_chan_count;

    if (phys_chan_count != 0)
    {
      /*---------------------------------------------------------------------
        DataRate, OuterCode and MACPacketsPerECBRow
      ---------------------------------------------------------------------*/
      offset += HDRBCOVHD_OVHD_DATA_RATE_SIZE;
      offset += HDRBCOVHD_OVHD_OUTER_CODE_SIZE;
      offset += HDRBCOVHD_OVHD_MAC_PKTS_PER_ECB_ROW_SIZE;

      /*---------------------------------------------------------------------
        Interlace and Multiplex for each physical channel
      ---------------------------------------------------------------------*/
      offset += (HDRBCOVHD_OVHD_INTR_SIZE + HDRBCOVHD_OVHD_MUX_SIZE) * 
                phys_chan_count;

      /*---------------------------------------------------------------------
        SoftCombineSameAsPrevLogicalChannel
      ---------------------------------------------------------------------*/
      err |= HDRBIT_UNPACK8( item_ptr,
                             &soft_comb_same_as_prev,
                             offset, 
                             HDRBCOVHD_OVHD_SOFT_COMBINE_SAME_AS_PREV_SIZE);

      offset += HDRBCOVHD_OVHD_SOFT_COMBINE_SAME_AS_PREV_SIZE;
      
      if (err != 0){ return E_NO_DATA; }

      if ((is_first_flow) && (soft_comb_same_as_prev == 1))
      {
        HDR_MSG_PROT( MSG_LEGACY_ERROR, 
                      "No soft combine info for first flow!");

        return E_DATA_INVALID;
      }

      if (soft_comb_same_as_prev == 0)
      {
        *soft_combine_offset_ptr = offset;

        /*------------------------------------------------------------------
          SoftCombine list
        ------------------------------------------------------------------*/
        offset += HDRBCOVHD_OVHD_SOFT_COMBINE_SIZE * neighbor_count;
      } 
      else /* (soft_comb_same_as_prev != 0) */
      {
        if (!plc_had_sc_info)
        {
          HDR_MSG_PROT( MSG_LEGACY_ERROR, 
                        "No soft combine info for previous logical channel!");

          return E_DATA_INVALID;
        }
      } /* else (soft_comb_same_as_prev != 0) */
    } /* if (phys_chan_count != 0) */
  }/* if (logical_chan_same_as_prev == 0) */

  /*---------------------------------------------------------------------
    AdditionalCDMAChannelsSameAsPreviousBCMCSFlow
  ---------------------------------------------------------------------*/

  err |= HDRBIT_UNPACK8( item_ptr,
                         &add_cdma_chan_same_as_prev,
                         offset, 
                         HDRBCOVHD_OVHD_ADD_CDMA_CHAN_SAME_AS_PREV_SIZE );

  offset += HDRBCOVHD_OVHD_ADD_CDMA_CHAN_SAME_AS_PREV_SIZE;

  if (err != 0){ return E_NO_DATA; }

  if ((is_first_flow) && (add_cdma_chan_same_as_prev == 1))
  {
    HDR_MSG_PROT( MSG_LEGACY_ERROR, 
                  "No CDMA channel count included in first flow!");

    return E_DATA_INVALID;
  }

  /* Additional CDMA channel information */
  if (add_cdma_chan_same_as_prev == 0)
  {
    *additional_cdma_channel_offset_ptr = offset;

    /*---------------------------------------------------------------------
      AdditionalCDMAChannelCount
    ---------------------------------------------------------------------*/
    err |= HDRBIT_UNPACK8( item_ptr,
                           &add_cdma_chan_count,
                           offset, 
                           HDRBCOVHD_OVHD_ADD_CDMA_CHAN_COUNT_SIZE );

    offset += HDRBCOVHD_OVHD_ADD_CDMA_CHAN_COUNT_SIZE;

    if (err != 0){ return E_NO_DATA; }

    offset += HDRBCOVHD_OVHD_CHANNEL_RECORD_SIZE * add_cdma_chan_count;
  }

  *offset_ptr = offset;
  return E_SUCCESS;

} /* hdrbcovhd_ovhd_skip_to_next_flow() */

/* <EJECT> */ 
/*=============================================================================
FUNCTION HDRBCOVHD_OVHD_PARSE_FLOW_FORMAT_0

DESCRIPTION
  This function finds the bit offset of desired flows and stores them in the
  cache entry to be used later to extract flow or channel information as
  needed.

DEPENDENCIES
  None

PARAMETERS
  item_ptr      - DSM item pointer to receive message
  bc_msg_ptr - Overhead cache entry to store flow information in
  offset       - Offset in message where format 0 specific information starts

RETURN VALUE
  E_SUCCESS if the flow record was parsed successfully
  E_NO_DATA if there was an error extracting a field
  E_DATA_INVALD if values encountered were not standard compliant

SIDE EFFECTS
  The cache entry pointed to by bc_msg_ptr is modified.
  

=============================================================================*/
static errno_enum_type hdrbcovhd_ovhd_parse_flow_format_0
(
  dsm_item_type         *const item_ptr,
  hdrbcovhd_bc_msg_type *bc_msg_ptr,
  uint16                offset
)
{
  int i; /* loop counter */
  int err = 0; /* ORing of all errno values returned from unpack calls */
  int flow_record_index  = 0; /* Flow record in overhead message */  
  int desired_flow_index = 0; /* Where to put desired flow info in bc_entry */

  /* Convenience pointer to access head of BC message */
  hdrbcovhd_bc_msg_head_type *msg_head_ptr = &bc_msg_ptr->head; 

  hdrbcovhd_flow_offsets_type *desired_entry_ptr; /* Convenience ptr */
  uint16 flow_id_len_in_bits; /* Flow ID length, in bits */

  /* List of desired flow ids */
  hdrbcovhd_flow_id_type desired_flow_id_list[HDRBCOVHD_BCMCS_MAX_FLOWS];
  uint8                  num_desired_flows; /* Number of desired flows */
  sys_bcmcs_flow_id_type current_flow_id; /* Temporary flow ID storage */
  uint16                 current_flow_offset; /* Offset of current flow rec */

  /* These three offset values keep track of the offset into the message for 
   * the part of the last flow record that contained the logical channel, 
   * soft combine or additional channel information.  When we find a record 
   * for a desired flow, these are where this information starts for that flow.
   */
  uint16 last_logical_channel_offset = 0; /* logical channel offset */
  uint16 last_soft_combine_offset = 0;    /* soft combine offset */
  uint16 last_additional_cdma_channel_offset = 0; /* another offset */

  errno_enum_type return_errno = E_SUCCESS; /* Value to be returned */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Note that this is a layering violation since hdrbccp is a client of 
   * hdrbcovhd.  It is needed to limit the size of the unpacked 
   * Broadcast Overhead message to only contain info on the the desired flows.
   */
  num_desired_flows = hdrbccp_get_desired_flow_list( desired_flow_id_list, 
                                                     HDRBCOVHD_BCMCS_MAX_FLOWS );

  /*---------------------------------------------------------------------------
     Unpack Flow Count
  ---------------------------------------------------------------------------*/
  err |= HDRBIT_UNPACK8(item_ptr, 
                        &msg_head_ptr->flow_count, 
                        offset,
                        HDRBCOVHD_OVHD_BCMCS_FLOW_COUNT_SIZE);
  offset += HDRBCOVHD_OVHD_BCMCS_FLOW_COUNT_SIZE;
                                             
  if (err != 0){ return E_NO_DATA; }

  /*---------------------------------------------------------------------------
     Unpack BCMCS Flow Information
  ---------------------------------------------------------------------------*/

  for (flow_record_index = 0; 
       flow_record_index < msg_head_ptr->flow_count; 
       flow_record_index++)
  {            
    /*-------------------------------------------------------------------------
      BCMCSFlowID 
    -------------------------------------------------------------------------*/
    flow_id_len_in_bits = (msg_head_ptr->bcmcs_flow_id_length) *
                          HDRBCOVHD_OVHD_BCMCS_FLOW_ID_SIZE;

    err |= HDRBIT_UNPACK32( item_ptr,
                            &current_flow_id,
                            offset,
                            flow_id_len_in_bits );
    offset += flow_id_len_in_bits;
            
    if (err != 0){ return E_NO_DATA; }

    /* Save offset of this flow in case it is a desired flow */
    current_flow_offset = offset;

    /*-------------------------------------------------------------------------
      BCMCS Flow and Logical Channel Info (same params for both format 0 & 1)
    -------------------------------------------------------------------------*/
    return_errno = 
      hdrbcovhd_ovhd_skip_to_next_flow( 
        item_ptr, 
        msg_head_ptr->neighbor_count,
        ((flow_record_index == 0)? TRUE : FALSE),
        &offset,
        &last_logical_channel_offset,
        &last_soft_combine_offset,
        &last_additional_cdma_channel_offset ); 

    if (return_errno != E_SUCCESS)
    {
      return( return_errno );
    }

    for (i = 0; i < num_desired_flows; i++)
    {
      if (desired_flow_id_list[i].flow_id == current_flow_id)
      {
        desired_entry_ptr = &bc_msg_ptr->desired_flow[desired_flow_index];
        desired_flow_index++;

        desired_entry_ptr->bcmcs_flow_id = current_flow_id;
        desired_entry_ptr->program_offset = 0; /* Not used for format 0 */
        desired_entry_ptr->flow_offset = current_flow_offset;
        desired_entry_ptr->logical_channel_offset = last_logical_channel_offset;
        desired_entry_ptr->soft_combine_offset = last_soft_combine_offset;
        desired_entry_ptr->additional_cdma_channel_offset = 
          last_additional_cdma_channel_offset;
        break;
      }
    }
  } /* for (flow_record_index) */

  bc_msg_ptr->num_desired_flows = desired_flow_index;

  return( return_errno );

} /* hdrbcovhd_ovhd_parse_flow_format_0 */

/* <EJECT> */ 
/*=============================================================================
FUNCTION HDRBCOVHD_OVHD_PARSE_FLOW_FORMAT_1

DESCRIPTION
  This function finds the bit offset of desired flows and stores them in the
  cache entry to be used later to extract flow or channel information as
  needed.

DEPENDENCIES
  None

PARAMETERS
  item_ptr      - DSM item pointer to receive message
  bc_msg_ptr - Overhead cache entry to store flow information in
  offset       - Offset in message where format 1 specific information starts

RETURN VALUE
  E_SUCCESS if the flow record was parsed successfully
  E_NO_DATA if there was an error extracting a field
  E_DATA_INVALD if values encountered were not standard compliant

SIDE EFFECTS
  The cache entry pointed to by bc_msg_ptr is modified.

=============================================================================*/
static errno_enum_type hdrbcovhd_ovhd_parse_flow_format_1
(
  dsm_item_type         *item_ptr,
  hdrbcovhd_bc_msg_type *bc_msg_ptr,
  uint16                offset
)
{
  int i; /* loop counter */
  int err = 0; /* ORing of all errno values returned from unpack calls */
  int program_index        = 0; /* Program record in overhead message */
  int flow_record_index    = 0; /* Flow record in overhead message */  
  int desired_flow_index   = 0; /* Index of desired flow info in bc_entry */
  int desired_program_index = 0; /* Index of desired program in bc_entry */

  /* Convenience pointer to access head of BC message */
  hdrbcovhd_bc_msg_head_type *msg_head_ptr = &bc_msg_ptr->head; 

  hdrbcovhd_flow_offsets_type *desired_entry_ptr; /* Convenience ptr */
  hdrbcovhd_program_only_offsets_type *desired_pid_entry_ptr; /* same */

  /* List of desired flow ids */
  hdrbcovhd_flow_id_type desired_flow_id_list[HDRBCOVHD_BCMCS_MAX_FLOWS];
  uint8                  num_desired_flows; /* Number of desired flows */
  sys_bcmcs_flow_id_type current_flow_id; /* Temporary flow ID storage */
  uint16                 current_flow_offset; /* Offset of current flow rec */
  uint32                 desired_program_id_lsbs; /* lsbs of desired f1 flow */
  /* These three offset values keep track of the offset into the message for 
   * the part of the last flow record that contained the logical channel, 
   * soft combine or additional channel information.  When we find a record 
   * for a desired flow, these are where this information starts for that flow.
   */
  uint16 last_program_offset         = 0; /* program offset */
  uint16 last_logical_channel_offset = 0; /* logical channel offset */
  uint16 last_soft_combine_offset    = 0; /* soft combine offset */
  uint16 last_additional_cdma_channel_offset = 0; /* another offset */
  uint16 last_additional_cdma_channel_count1_offset = 0; /* another offset */

  /* Variables to hold key values specific to format 1 ... */
  uint8 program_id_lsb_length;
  uint32 program_id_lsbs;
  uint8 flow_discriminator_count;
  uint8 additional_cdma_channel_count1;
  uint8 flow_discriminator_length = 0;
  uint8 flow_discriminator = 0;

  errno_enum_type return_errno = E_SUCCESS; /* Value to be returned */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Note that this is a layering violation since hdrbccp is a client of 
   * hdrbcovhd.  It is needed to limit the size of the unpacked 
   * Broadcast Overhead message to only contain info on the the desired flows.
   */
  num_desired_flows = hdrbccp_get_desired_flow_list( desired_flow_id_list, 
                                                     HDRBCOVHD_BCMCS_MAX_FLOWS );

  /* Because there is no actual flow_count number in the format 1 records,
   * we initialize it to zero then increment it every time we find a
   * flow discriminator for a program.  So it is the sum of all the 
   * flow discriminators of all the programs in the message.
   */
  msg_head_ptr->flow_count = 0; 

  /*-------------------------------------------------------------------------
    ProgramCount
  -------------------------------------------------------------------------*/
  err |= HDRBIT_UNPACK8( item_ptr, 
                         &msg_head_ptr->program_count,
                         offset,
                         HDRBCOVHD_OVHD_PROGRAM_COUNT_SIZE );

  offset += HDRBCOVHD_OVHD_PROGRAM_COUNT_SIZE;

  if (err != 0){ return E_NO_DATA; }

  for ( program_index = 0; 
        program_index < msg_head_ptr->program_count; 
        program_index++ )
  {

    last_program_offset = offset;

    /*-------------------------------------------------------------------------
      ProgramIDLSBLength
    -------------------------------------------------------------------------*/
    err |= HDRBIT_UNPACK8( item_ptr, 
                           &program_id_lsb_length, 
                           offset,
                           HDRBCOVHD_OVHD_PROGRAM_ID_LSB_LENGTH_SIZE );

    offset += HDRBCOVHD_OVHD_PROGRAM_ID_LSB_LENGTH_SIZE;

    if (err != 0){ return E_NO_DATA; }

    if (program_id_lsb_length != 0)
    {
      /*-------------------------------------------------------------------------
        ProgramIDLSBs
      -------------------------------------------------------------------------*/
      err |= HDRBIT_UNPACK32( item_ptr, 
                              &program_id_lsbs, 
                              offset,
                              program_id_lsb_length );

      offset += program_id_lsb_length;
    }                            
    else
    {
      program_id_lsbs = 0;
    }

    /*-------------------------------------------------------------------------
      FlowDiscriminatorCount
    -------------------------------------------------------------------------*/
    err |= HDRBIT_UNPACK8( item_ptr, 
                           &flow_discriminator_count, 
                           offset,
                           HDRBCOVHD_OVHD_FLOW_DISCRIMINATOR_COUNT_SIZE );

    offset += HDRBCOVHD_OVHD_FLOW_DISCRIMINATOR_COUNT_SIZE;

    if (err != 0){ return E_NO_DATA; }

    if ( flow_discriminator_count == 0 )
    {
      HDR_MSG_PROT( MSG_LEGACY_ERROR, 
        "FlowDiscriminatorCount = 0; Desired flow may never be monitored" );

      /* This overhead doesn't contain any FlowDiscriminators for
       * this Program, so the flowDiscriminator length is unspecified.
       * This means we cannot construct BCMCS flow ID for flows under this 
       * Program. This program is listed here as an advertisement and doesn't
       * have any physical channels associated. It typically has additional 
       * CDMA channels on which it is available.
       */

      /*-----------------------------------------------------------------------
        RegisterForPaging1, RegisterForDynamicBroadcast1
      -----------------------------------------------------------------------*/
      offset += HDRBCOVHD_OVHD_REGISTER_FOR_PAGING_SIZE;
      offset += HDRBCOVHD_OVHD_REGISTER_FOR_DYNAMIC_BROADCAST_SIZE;

      last_additional_cdma_channel_count1_offset = offset;

      /*-----------------------------------------------------------------------
        AdditionalCDMAChannelCount1
      -----------------------------------------------------------------------*/
      err |= HDRBIT_UNPACK8( item_ptr,
                             &additional_cdma_channel_count1,
                             offset, 
                             HDRBCOVHD_OVHD_ADD_CDMA_CHAN_COUNT_SIZE );

      offset += HDRBCOVHD_OVHD_ADD_CDMA_CHAN_COUNT_SIZE;
      offset += HDRBCOVHD_OVHD_CHANNEL_RECORD_SIZE * 
                additional_cdma_channel_count1;

      if (err != 0){ return E_NO_DATA; }

      /* If one of the desired flows is from this program we won't find 
       * an exact match, so just remember the additional channels where
       * this program can be found.
       */

      for (i = 0; i < num_desired_flows; i++)
      {
        /* First shift the program_id_lsbs down */
        desired_program_id_lsbs = desired_flow_id_list[i].flow_id >> 
                                  desired_flow_id_list[i].flow_discrim_len;
        /* Then mask off the upper bits */
        desired_program_id_lsbs &= (1 << program_id_lsb_length) - 1;

        if (desired_program_id_lsbs == program_id_lsbs)
        {
          desired_pid_entry_ptr = 
            &bc_msg_ptr->desired_program[desired_program_index];
          desired_program_index++;

          /* The list is of desired FlowIDs that match by "program only" 
           * The actual FlowID from the BOM is unknown but assumed it will
           * match once we go the channel this program is on and get the BOM
           * with the actual flow discriminator included.
           */
          desired_pid_entry_ptr->bcmcs_flow_id = 
            desired_flow_id_list[i].flow_id;

          desired_pid_entry_ptr->program_offset = last_program_offset;
          desired_pid_entry_ptr->additional_cdma_channel_offset = 
            last_additional_cdma_channel_count1_offset;
          break;
        }
      }
    }
    else /* flow discriminator count != 0 */
    {

      /*-----------------------------------------------------------------------
        FlowDiscriminatorLength
      -----------------------------------------------------------------------*/
      err |= HDRBIT_UNPACK8( item_ptr, 
                             &flow_discriminator_length,
                             offset,  
                             HDRBCOVHD_OVHD_FLOW_DISCRIMINATOR_LENGTH_SIZE );

      offset += HDRBCOVHD_OVHD_FLOW_DISCRIMINATOR_LENGTH_SIZE;

      if (err != 0){ return E_NO_DATA; }

      for ( flow_record_index = 0; 
            flow_record_index < flow_discriminator_count;
            flow_record_index++ )
      {
        msg_head_ptr->flow_count++;

        if ( flow_discriminator_length == 0 )
        {
          HDR_MSG_PROT( MSG_LEGACY_ERROR, 
          "FlowDiscriminatorLength = 0; Desired flow may never be monitored" );
        }
        else
        {
          /*-------------------------------------------------------------------
            FlowDiscriminator
          -------------------------------------------------------------------*/
          err |= HDRBIT_UNPACK8( item_ptr, 
                                 &flow_discriminator,
                                 offset,  
                                 flow_discriminator_length );

          offset += flow_discriminator_length;

          if (err != 0){ return E_NO_DATA; }
        }

        /* Create BCMCS Flow ID */
        current_flow_id = hdrbcovhd_create_bcmcs_flow_id(
                            (msg_head_ptr->bcmcs_flow_id_length * 8),
                            flow_discriminator_length,
                            flow_discriminator,
                            program_id_lsbs );

        /* Save offset of this flow in case it is a desired flow */
        current_flow_offset = offset;
    
        /*---------------------------------------------------------------------
          BCMCS Flow and Logical Channel Info (same for both format 0 & 1)
        ---------------------------------------------------------------------*/
        return_errno = 
          hdrbcovhd_ovhd_skip_to_next_flow( 
            item_ptr, 
            msg_head_ptr->neighbor_count,
            ((program_index == 0)? TRUE : FALSE),
            &offset,
            &last_logical_channel_offset,
            &last_soft_combine_offset,
            &last_additional_cdma_channel_offset ); 
    
        if (return_errno != E_SUCCESS)
        {
          return( return_errno );
        }
    
        for (i = 0; i < num_desired_flows; i++)
        {
          if (desired_flow_id_list[i].flow_id == current_flow_id)
          {
            desired_entry_ptr = &bc_msg_ptr->desired_flow[desired_flow_index];
            desired_flow_index++;
    
            desired_entry_ptr->bcmcs_flow_id = current_flow_id;
            desired_entry_ptr->program_offset = last_program_offset;
            desired_entry_ptr->flow_offset = current_flow_offset;
            desired_entry_ptr->logical_channel_offset = 
              last_logical_channel_offset;
            desired_entry_ptr->soft_combine_offset = last_soft_combine_offset;
            desired_entry_ptr->additional_cdma_channel_offset = 
              last_additional_cdma_channel_offset;
            break;
          }
        }
      } /* for (flow_record_index) */
    } /* else (flow discriminator count != 0) */
  } /* for (program_index) */

  bc_msg_ptr->num_desired_flows = desired_flow_index;
  bc_msg_ptr->num_desired_programs = desired_program_index;

  return( return_errno );

} /* hdrbcovhd_ovhd_parse_flow_format_1 */

/******************************************************************************
**                                                                           **
**             Externalized functions called by hdrovhd.c                    **
**                                                                           **
******************************************************************************/

/* <EJECT> */ 
/*===========================================================================
FUNCTION HDRBCOVHD_POWERUP_INIT

DESCRIPTION
  Powerup initialization for the HDRBCOVHD module.
  
DEPENDENCIES
  None.
  
PARAMETERS
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  Initializes static data on power up.
  
===========================================================================*/
void hdrbcovhd_powerup_init( void )
{
  rex_init_crit_sect( &hdrbcovhd.crit_sect );
  hdrbcovhd.item_ptr = NULL;
}

/* <EJECT> */ 
/*===========================================================================
FUNCTION HDRBCOVHD_UPDATE_OVHD_INFO

DESCRIPTION
  Updates the currently stored version of the BroadcastOverhead message.
  
DEPENDENCIES
  
PARAMETERS
  item_ptr      - Pointer to the DSM item chain holding the message 
  bc_msg_ptr   - Pointer to the cache entry.

RETURN VALUE
  
SIDE EFFECTS
  None.
===========================================================================*/
void hdrbcovhd_update_ovhd_info
(
  dsm_item_type            *item_ptr,
  hdrbcovhd_bc_msg_type    *bc_msg_ptr
)
{
  int desired_flow_index; /* Index of Flow Record in bc_msg */
  hdrbcovhd_bc_msg_type *new_bc_ptr; /* Convenience pointer */
  errno_enum_type err_number; /* Value returned from extracting logical chans */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  new_bc_ptr = &hdrbcovhd.bc_msg;

  /*---------------------------------------------------------------------------
  Copy cache entry and dup the pointer to the current BroadcastOverhead message
  ---------------------------------------------------------------------------*/
  rex_enter_crit_sect( &hdrbcovhd.crit_sect );
  memcpy( new_bc_ptr, bc_msg_ptr, sizeof(hdrbcovhd_bc_msg_type));

  dsm_free_packet(&hdrbcovhd.item_ptr);
  (void) dsm_dup_packet( &hdrbcovhd.item_ptr, item_ptr,
                         0, dsm_length_packet (item_ptr));
  rex_leave_crit_sect( &hdrbcovhd.crit_sect );

  if ( hdrbcovhd.item_ptr == NULL )
  {
    ERR_FATAL("No more duplicate dsm items available!",0,0,0);
  }

  /*---------------------------------------------------------------------------
  For now we just unpack the logical channel info on each handoff, which can
  occur every 426ms while in traffic.  We could optimize this later if we want.
  ---------------------------------------------------------------------------*/
  memset(hdrbcovhd.logical_channel_info, 0 , 
         HDRBCOVHD_BCMCS_MAX_FLOWS * sizeof(hdrbcovhd_logical_channel_info_type));

  for (desired_flow_index = 0; 
       desired_flow_index < bc_msg_ptr->num_desired_flows; 
       desired_flow_index++)
  { 
    err_number = hdrbcovhd_unpack_logical_channel_info( 
      hdrbcovhd.item_ptr, 
      &hdrbcovhd.logical_channel_info[desired_flow_index],
      new_bc_ptr->desired_flow[desired_flow_index].logical_channel_offset );

    if (err_number != E_SUCCESS)
    {
      break;
    }
  }
} /* hdrbcovhd_update_ovhd_info() */

/* <EJECT> */
/*=============================================================================

FUNCTION HDROVHD_UNPACK_OVHD_MSG

DESCRIPTION
  This function unpacks parts of the received Broadcast message into the
  overhead message cache. It saves indexes into the message in the cache
  so other routines can extract values on an as-needed basis without needing
  to re-parse the entire message.


DEPENDENCIES

PARAMETERS
  item_ptr      - Pointer to the DSM item chain holding the message 
  bc_msg_ptr - Pointer to the cache entry.

RETURN VALUE
  E_SUCCESS  - If extraction & unpacking of message succeeds.
  E_NO_DATA  - Otherwise.

SIDE EFFECTS
  None

=============================================================================*/
errno_enum_type hdrbcovhd_unpack_ovhd_msg
(
  dsm_item_type            *item_ptr,
  hdrbcovhd_bc_msg_type    *bc_msg_ptr
)
{

  uint16 offset = 0; /* Offset in to the overhead message */
  int err = 0;       /* Error value. 0 = E_SUCCESS */

  /* Convenience pointer to access head of BC message */
  hdrbcovhd_bc_msg_head_type *msg_head_ptr = &bc_msg_ptr->head; 

  /* These variables are for fields that are converted before being stored */
  uint8  multiplex_count;  /* MultiplexCount */
  uint8  framing_enabled;  /* FramingEnabled */
  uint8  flow_format;      /* FlowFormat */
  uint8  burst_length;     /* BurstLength */

  uint16 burst_length_sum; /* Used to compute BurstLength of last multiplex */

  /* index variables */
  uint8 intr;  /* interlaces */
  uint8 mux;   /* muxltiplexes */

  errno_enum_type return_errno; /* Value to be returned */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*---------------------------------------------------------------------------
  The return values from macros are cast to int & OR-ed with the previous error
  If any of the macros return an error, the final value of err will be non-zero
  err is an int type & not errno_enum_type, because OR-ing two 
  errno_enum_type values can result in an invalid errno_enum_type value.
  ---------------------------------------------------------------------------*/

  /* MessageID has already been extracted */
  offset += HDRBCOVHD_OVHD_MSG_ID_SIZE;

  /*---------------------------------------------------------------------------
    BroadcastOverheadSignature 
  ---------------------------------------------------------------------------*/
  err |= (int) HDRBIT_UNPACK16( item_ptr, 
                                &msg_head_ptr->broadcast_overhead_signature, 
                                offset,
                                HDRBCOVHD_OVHD_OVHD_SIG_SIZE);
  offset += HDRBCOVHD_OVHD_OVHD_SIG_SIZE;

  /*---------------------------------------------------------------------------
    SectorSignature
  ---------------------------------------------------------------------------*/
  err |= (int) HDRBIT_UNPACK16( item_ptr, 
                                &msg_head_ptr->sector_signature, 
                                offset,
                                HDRBCOVHD_OVHD_SECTOR_SIG_SIZE);
  offset += HDRBCOVHD_OVHD_SECTOR_SIG_SIZE;

  /*---------------------------------------------------------------------------
    NeighborCount
  ---------------------------------------------------------------------------*/
  err |= (int) HDRBIT_UNPACK8( item_ptr, 
                               &msg_head_ptr->neighbor_count, 
                               offset,
                               HDRBCOVHD_OVHD_NBR_COUNT_SIZE );
  offset += HDRBCOVHD_OVHD_NBR_COUNT_SIZE;

  /*---------------------------------------------------------------------------
    MultiplexCount
  ---------------------------------------------------------------------------*/
  err |= (int) HDRBIT_UNPACK8( item_ptr, 
                               &multiplex_count, 
                               offset,
                               HDRBCOVHD_OVHD_MULTIPLEX_COUNT_SIZE );
  offset += HDRBCOVHD_OVHD_MULTIPLEX_COUNT_SIZE;

  if (err != 0){ return E_NO_DATA; }

  return_errno = hdrbcovhd_ovhd_compute_multiplex_count( multiplex_count, 
                   &msg_head_ptr->multiplex_count );

  if ( return_errno != E_SUCCESS )
  {
    return( return_errno );
  }

  /*---------------------------------------------------------------------------
    FramingEnabled 
  ---------------------------------------------------------------------------*/
  err |= (int) HDRBIT_UNPACK8( item_ptr, 
                               &framing_enabled, 
                               offset,
                               HDRBCOVHD_OVHD_FRAMING_ENABLED_SIZE );
  offset += HDRBCOVHD_OVHD_FRAMING_ENABLED_SIZE;

  if (err != 0){ return E_NO_DATA; }
  
  return_errno = hdrbcovhd_ovhd_deduce_framing_type( framing_enabled, 
                                               &msg_head_ptr->framing_type );

  if ( return_errno != E_SUCCESS )
  {
    return( return_errno );
  }

  /*---------------------------------------------------------------------------
    IncludeFCS
  ---------------------------------------------------------------------------*/
  err |= (int) HDRBIT_UNPACK8( item_ptr, 
                               &msg_head_ptr->include_fcs, 
                               offset,
                               HDRBCOVHD_OVHD_INCLUDE_FCS_SIZE );
  offset += HDRBCOVHD_OVHD_INCLUDE_FCS_SIZE;

  /*---------------------------------------------------------------------------
    BCMCSFlowFormat
  ---------------------------------------------------------------------------*/
  err |= (int) HDRBIT_UNPACK8( item_ptr, 
                               &flow_format, 
                               offset, 
                               HDRBCOVHD_OVHD_BCMCS_FLOW_FORMAT_SIZE );
  offset += HDRBCOVHD_OVHD_BCMCS_FLOW_FORMAT_SIZE;

  if (err != 0){ return E_NO_DATA; }

  msg_head_ptr->bcmcs_flow_format = (flow_format == 0) ? 
                          HDRBCOVHD_FLOW_FORMAT_0 : HDRBCOVHD_FLOW_FORMAT_1;


  /* Burst Lengths */
  for ( intr=0; intr < HDRBCOVHD_MAX_INTERLACES; intr++ )
  {
    /*-------------------------------------------------------------------------
      InterlaceXIncluded
    -------------------------------------------------------------------------*/
    err |= (int) HDRBIT_UNPACK8(item_ptr,
                                &msg_head_ptr->interlace_included[intr], 
                                offset,
                                HDRBCOVHD_OVHD_INTR_INCLUDED_SIZE);
    offset += HDRBCOVHD_OVHD_INTR_INCLUDED_SIZE;


    if ( msg_head_ptr->interlace_included[intr] )
    {
      /*-----------------------------------------------------------------------
        SameBurstLengthX
      -----------------------------------------------------------------------*/
      err |= (int) HDRBIT_UNPACK8
             ( item_ptr,
               &msg_head_ptr->same_burst_lengths[intr],
               offset,
               HDRBCOVHD_OVHD_SAME_BURST_LENGTHS_SIZE 
             );
      offset += HDRBCOVHD_OVHD_SAME_BURST_LENGTHS_SIZE;

      /* Not Same burst lenghts */
      if ( !msg_head_ptr->same_burst_lengths[intr] )
      {
        /*---------------------------------------------------------------------
          TotalBurstLengthX
        ---------------------------------------------------------------------*/
        err |= (int) HDRBIT_UNPACK16
               ( item_ptr, 
                 &msg_head_ptr->total_burst_length[intr], 
                 offset,
                 HDRBCOVHD_OVHD_TOTAL_BURST_LENGTH_SIZE 
               );
        offset += HDRBCOVHD_OVHD_TOTAL_BURST_LENGTH_SIZE;

        /* Message field value is one less than the total burst length */
        msg_head_ptr->total_burst_length[intr]++;

        burst_length_sum = 0;

        /* Extract Burst length for all multiplexes except last */
        for ( mux = 0; mux <= msg_head_ptr->multiplex_count - 2; mux++ )
        {
          /*-------------------------------------------------------------------
            BurstLengthX
          -------------------------------------------------------------------*/
          err |= (int) HDRBIT_UNPACK8( item_ptr, 
                                       &burst_length, 
                                       offset,
                                       HDRBCOVHD_OVHD_BURST_LENGTH_SIZE );
          offset += HDRBCOVHD_OVHD_BURST_LENGTH_SIZE;

          if (err != 0){ return E_NO_DATA; }

          return_errno = hdrbcovhd_ovhd_compute_burst_length( 
                           burst_length, 
                           &msg_head_ptr->burst_length[intr][mux] );

          if ( return_errno != E_SUCCESS )
          {
            return( return_errno );
          }

          burst_length_sum += msg_head_ptr->burst_length[intr][mux];

        }

        /* Last multiplex is a special multiplex and is used for cushioning.
        It can take values outside the enumerated burst_length values */

        msg_head_ptr->burst_length[intr][(msg_head_ptr->multiplex_count-1)] = 
          (hdrbcovhd_burst_length_enum_type)
          (msg_head_ptr->total_burst_length[intr] - burst_length_sum);

      } 
      /* Same Burst Lengths */
      else /* if (msg_head_ptr->same_burst_lengths[intr] == 1) */
      {
        /*---------------------------------------------------------------------
          BurstLengthX
        ---------------------------------------------------------------------*/
        err |= (int) HDRBIT_UNPACK8( item_ptr, 
                                     &burst_length, 
                                     offset,
                                     HDRBCOVHD_OVHD_BURST_LENGTH_SIZE );
        offset += HDRBCOVHD_OVHD_BURST_LENGTH_SIZE;

        return_errno = hdrbcovhd_ovhd_compute_burst_length( 
                         burst_length, 
                         &msg_head_ptr->burst_length[intr][0] );

        if ( return_errno != E_SUCCESS )
        {
          return( return_errno );
        }

        for ( mux = 1; mux <= msg_head_ptr->multiplex_count - 1; mux++)
        {
          msg_head_ptr->burst_length[intr][mux] = 
            msg_head_ptr->burst_length[intr][0];
        }
      } /* else (msg_head_ptr->same_burst_lengths[intr] == 1) */
    } /* if ( msg_head_ptr->interlace_included[intr] ) */
  } /* for ( each interlace ) */

  /*---------------------------------------------------------------------------
    BCMCSFlowIDLength
  ---------------------------------------------------------------------------*/
  err |= HDRBIT_UNPACK8(item_ptr, 
                        &msg_head_ptr->bcmcs_flow_id_length, 
                        offset,
                        HDRBCOVHD_OVHD_BCMCS_FLOW_ID_LENGTH_SIZE);
  offset += HDRBCOVHD_OVHD_BCMCS_FLOW_ID_LENGTH_SIZE;
  
  /* Increment by 1: unpacked value is one less than the length of the BCMCS 
     Flow Identifier in untis of octets */
  if (msg_head_ptr->bcmcs_flow_id_length == 0)
  {
    ERR("BCMCSFlowIDLength shall not be set to '00', ignore bcovhd msg ",0,0,0);
    return( E_DATA_INVALID ); 
  }
  else
  {
    msg_head_ptr->bcmcs_flow_id_length++;
  }

  
  /*---------------------------------------------------------------------------
    RemainingBCMCSOverheadFields
  
    Unpacks information related to flow format 0 or 1. 
    It extracts info, from offset onwards and updates offset to the latest 
    position.
  ---------------------------------------------------------------------------*/
  if (err != 0)
  {
    /* We got some error unpacking fields.  No need to continue */
    return_errno = E_NO_DATA;
  }
  else if ( msg_head_ptr->bcmcs_flow_format == 0 )
  {
    return_errno = hdrbcovhd_ovhd_parse_flow_format_0( item_ptr,
                                                       bc_msg_ptr, 
                                                       offset );
  }
  else
  {
    return_errno = hdrbcovhd_ovhd_parse_flow_format_1( item_ptr, 
                                                       bc_msg_ptr, 
                                                       offset );
  }

  return( return_errno );
} /* hdrbcovhd_unpack_ovhd_msg() */

/* <EJECT> */ 

/******************************************************************************
**                                                                           **
** The rest of this file is externalized functions to access various fields  **
** if the most recent BroadcastOverhead message, as needed.  Each function   **
** must run inside the hdrbcovhd.crit_sect critical section to avoid the     **
** race condition where we are updating the message while one of these       **
** routines is being run by another task.                                    **
**                                                                           **
******************************************************************************/

/*=============================================================================

FUNCTION HDRBCOVHD_GET_FRAMING_TYPE

DESCRIPTION
  Accessor function to query the broadcast framing type.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  HDRBCOVHD_FRAMING_INVALID if hdrbcovhd has no BroadcastOverhead message
  HDRBCOVHD_FRAMING_SEGMENT or HDRBCOVHD_FRAMING_HDLC otherwise, depending
  on the specified framing type in the BroadcastOverhead message

SIDE EFFECTS
  None

=============================================================================*/
hdrbcovhd_framing_type hdrbcovhd_get_framing_type( void )
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (hdrbcovhd.item_ptr == NULL)
  {
    return HDRBCOVHD_FRAMING_INVALID;
  }

  return (hdrbcovhd.bc_msg.head.framing_type);

} /* hdrbcovhd_ovhd_get_framing_type() */

/* <EJECT> */ 
/*=============================================================================

FUNCTION HDRBCOVHD_IS_FCS_INCLUDED

DESCRIPTION
  Accessor function to query the broadcast "include fcs" field, which 
  indicates whether an FCS is used in framing.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  TRUE if the IncludeFCS field of the BroadcastOverhead message is set to one,
  FALSE otherwise, including if hdrbcovhd has no BroadcastOverhead message

SIDE EFFECTS
  None

=============================================================================*/
boolean hdrbcovhd_is_fcs_included( void )
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (hdrbcovhd.item_ptr == NULL)
  {
    return FALSE;
  }

  return (hdrbcovhd.bc_msg.head.include_fcs);

} /* hdrbcovhd_is_fcs_included() */

/* <EJECT> */ 
/*=============================================================================

FUNCTION HDRBCOVHD_GET_FLOW_ID_LEN

DESCRIPTION
  Accessor function to query the FlowIDLength

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  zero if hdrbcovhd has no BroadcastOverhead message
  2, 3, or 4 is the number of octets used for the FlowID

SIDE EFFECTS
  None

=============================================================================*/
uint8 hdrbcovhd_get_flow_id_len( void )
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (hdrbcovhd.item_ptr == NULL)
  {
    return 0;
  }

  return (hdrbcovhd.bc_msg.head.bcmcs_flow_id_length);

} /* hdrbcovhd_ovhd_get_flow_id_len() */

/* <EJECT> */ 
/*===========================================================================
FUNCTION HDRBCOVHD_FLOW_IS_DESIRED

DESCRIPTION
  Helper function to find a flow in the desired_flow or desired_program list
  
DEPENDENCIES
  
PARAMETERS
  flow_id   - The FlowID of "desired flow" to look for.
  type_ptr  - Output parameter to tell which list flow was found on
  index_ptr - Output parameter to tell what index to use

RETURN VALUE
  TRUE  if the desired FlowID passed in is listed as being available.
  FALSE if the FlowID is not a desired flow, or if the flow is desired but
        not available or if hdrbcovhd has no BroadcastOverhead message
  
SIDE EFFECTS
  None.
===========================================================================*/
static boolean hdrbcovhd_flow_is_desired
(
  uint32 flow_id,
  hdrbcovhd_list_enum_type *type_ptr, /* output parameter */
  int *index_ptr
)
{
  int i; /* index */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for (i = 0; i < hdrbcovhd.bc_msg.num_desired_flows; i++)
  {
    if (hdrbcovhd.bc_msg.desired_flow[i].bcmcs_flow_id == flow_id)
    {
      if (type_ptr != NULL){ *type_ptr = HDRBCOVHD_DESIRED_FLOW; }
      if (index_ptr != NULL){ *index_ptr = i; }
      return TRUE;
    }
  }

  /* Flow wasn't in the desired_flow list, try the program only list */
  if (hdrbcovhd.bc_msg.head.bcmcs_flow_format == HDRBCOVHD_FLOW_FORMAT_1)
  {
    for (i = 0; i < hdrbcovhd.bc_msg.num_desired_programs; i++)
    {
      if (hdrbcovhd.bc_msg.desired_program[i].bcmcs_flow_id == flow_id)
      {
        if (type_ptr != NULL){ *type_ptr = HDRBCOVHD_DESIRED_PROGRAM; }
        if (index_ptr != NULL){ *index_ptr = i; }
        return TRUE;
      }
    }
  }
  return FALSE;
} /* hdrbcovhd_flow_is_desired() */

/*===========================================================================
FUNCTION HDRBCOVHD_IS_FLOW_AVAILABLE

DESCRIPTION
  Tells if a flow is available in the most recentely received 
  BroadcastOverhead message.
  
DEPENDENCIES
  
PARAMETERS
  flow_id - The FlowID of "desired flow" to look for.  This must be a flow
            that has been requested by an application.

RETURN VALUE
  TRUE  if the desired FlowID passed in is listed as being available.
  FALSE if the FlowID is not a desired flow, or if the flow is desired but
        not available or if hdrbcovhd has no BroadcastOverhead message
  
SIDE EFFECTS
  None.
===========================================================================*/
boolean hdrbcovhd_is_flow_available
(
  uint32 flow_id
)
{
  boolean flow_was_found = FALSE; /* return value of this function */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (hdrbcovhd.item_ptr == NULL)
  {
    return FALSE;
  }

  rex_enter_crit_sect( &hdrbcovhd.crit_sect );

  flow_was_found = hdrbcovhd_flow_is_desired( flow_id, NULL, NULL );

  rex_leave_crit_sect( &hdrbcovhd.crit_sect );

  HDR_MSG_PROT_2(MSG_LEGACY_MED,"Flow %d available in parsed bcovhd (%d)",
                 flow_id,flow_was_found);

  return flow_was_found;
} /* hdrbcovhd_is_flow_available() */

/* <EJECT> */ 
/*===========================================================================
FUNCTION HDRBCOVHD_BASIC_INFO_HAS_CHANGED

DESCRIPTION
  Determines if the structure of the BCMCS physical channel (interlaces,
  multiplexes and burst lengths) has changed. 
  
  Specifically, it compares these values:
  
  multiplex_count
  framing_type
  include_fcs
  bcmcs_flow_format
  interlace_included[HDRBCOVHD_MAX_INTERLACES]
  same_burst_lengths[HDRBCOVHD_MAX_INTERLACES]
  total_burst_length[HDRBCOVHD_MAX_INTERLACES]
  burst_length[HDRBCOVHD_MAX_INTERLACES][HDRBCOVHD_MAX_MULTIPLEXES]
  bcmcs_flow_id_length
  
  
DEPENDENCIES
  
PARAMETERS
  bc_msg_head_ptr - Pointer to an older version of the BroadcastOverhead 
                    message head, to be compared with the latest version.

RETURN VALUE
  TRUE if the information has changed
  FALSE otherwise
  
SIDE EFFECTS
  None.
===========================================================================*/
boolean hdrbcovhd_basic_info_has_changed
(
  const hdrbcovhd_bc_msg_head_type  *bc_msg_head_ptr
)
{
  int result; /* Return value of memcmp().  0 means they match */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT (hdrbcovhd.item_ptr != NULL);

  ASSERT (bc_msg_head_ptr != NULL);

  /* We only care about the fields starting with 
   * multiplex_count, up to but not including flow_count 
   */

  rex_enter_crit_sect( &hdrbcovhd.crit_sect );

  result = memcmp( &hdrbcovhd.bc_msg.head.multiplex_count, 
                   &bc_msg_head_ptr->multiplex_count,
                   ( FPOS( hdrbcovhd_bc_msg_head_type, flow_count ) -
                     FPOS( hdrbcovhd_bc_msg_head_type, multiplex_count )));

  rex_leave_crit_sect( &hdrbcovhd.crit_sect );

  /* If the result is zero, no fields have changed.
   * If the result is not zero, one or more fields have changed.
   */

  return ((result != 0) ? TRUE : FALSE);

} /* hdrbcovhd_basic_info_has_changed() */

/* <EJECT> */ 
/*===========================================================================
FUNCTION HDRBCOVHD_GET_BC_MSG_HEAD

DESCRIPTION
  Returns the head of the BroadcstOverhead information, up to the flow
  specific information.
  
DEPENDENCIES
  
PARAMETERS
  bc_msg_head_ptr - Location to return BroadcastOverhead message info into.          

RETURN VALUE
  E_SUCCESS if all data was extracted successfully
  E_FAILURE if hdrbcovhd has no BroadcastOverhead message
  
  If E_SUCCESS is returned, the buffer at address bc_msg_head_ptr is filled 
  in.  Otherwise it is unchanged.
  
SIDE EFFECTS
  None.
===========================================================================*/
errno_enum_type hdrbcovhd_get_bc_msg_head
(
  hdrbcovhd_bc_msg_head_type  *bc_msg_head_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (hdrbcovhd.item_ptr == NULL)
  {
    return E_FAILURE;
  }

  rex_enter_crit_sect( &hdrbcovhd.crit_sect );
  memcpy( bc_msg_head_ptr, &hdrbcovhd.bc_msg, 
          sizeof(hdrbcovhd_bc_msg_head_type) );
  rex_leave_crit_sect( &hdrbcovhd.crit_sect );

  return E_SUCCESS;
} /* hdrbcovhd_get_bc_msg_head() */

/* <EJECT> */ 
/*===========================================================================
FUNCTION HDRBCOVHD_FLOW_INFO_HAS_CHANGED

DESCRIPTION
  Determines if the structure of the BCMCS logical channel for a given
  desired flow has changed.
  
  Specifically, this function compares these fields:
  
  phys_chan_count
  data_rate
  outer_code
  mac_pkts_per_ecb_row
  phys_chan[HDRBCOVHD_MAX_PHYSICAL_CHANNEL_COUNT] 
  (i.e. the list of interlace/multiplex pairs assigned to the flow)
  
  
DEPENDENCIES
  
PARAMETERS
  bcmcs_flow_id         - FlowID of the flow to compare
  logical_chan_info_ptr - Pointer to older logical channel info for the given 
                          FlowID, to be compared with the latest.

RETURN VALUE
  TRUE if the information has changed or if the FlowID is not a desired flow
  FALSE otherwise
  
SIDE EFFECTS
  None.
===========================================================================*/
boolean hdrbcovhd_flow_info_has_changed
(
  uint32                               bcmcs_flow_id,
  const hdrbcovhd_logical_channel_info_type  *logical_chan_info_ptr
)
{
  int     i; /* index */
  int     result;
  boolean info_has_changed = TRUE; /* assume the info has changed */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT (hdrbcovhd.item_ptr != NULL);

  rex_enter_crit_sect( &hdrbcovhd.crit_sect );

  for (i = 0; i < hdrbcovhd.bc_msg.num_desired_flows; i++)
  {
    if ( hdrbcovhd.bc_msg.desired_flow[i].bcmcs_flow_id == bcmcs_flow_id )
    {
      result = memcmp( &hdrbcovhd.logical_channel_info[i],
                       logical_chan_info_ptr,
                       sizeof( hdrbcovhd_logical_channel_info_type ) );

      if (result == 0)
      {
        /* The two flows have the same logical channel information */
        info_has_changed = FALSE;
      }
      break;
    }
  }

  rex_leave_crit_sect( &hdrbcovhd.crit_sect );

  if (i == hdrbcovhd.bc_msg.num_desired_flows)
  {
    /* The given flow_id is no longer a desired flow */
  }

  return info_has_changed;

} /* hdrbcovhd_flow_info_has_changed() */


/* <EJECT> */ 
/*===========================================================================
FUNCTION HDRBCOVHD_GET_LOGICAL_CHAN_INFO

DESCRIPTION
  Returns the flow IDs, logical channel and soft combine information for all
  desired flows.  We want to give the all three lists here so it is all 
  within the same critical section.  Otherwise, since they are parallel
  arrays, it would be bad to pull one from a different message than the others.
  
DEPENDENCIES
  
PARAMETERS
  max_bcmcs_flow_ids    - Size of the flow ID array pointer passed in
  num_flows_ptr         - Number of desired flows
  bcmcs_flow_id_ptr     - Array to store desired FlowIDs into.
  logical_chan_info_ptr - Location to return logical channel information
                          for desired flows.
  soft_combine_info_ptr - Location to return soft combine information for
                          desired flows.

RETURN VALUE
  E_SUCCESS if all data was extracted successfully
  E_FAILURE if hdrbcovhd has no BroadcastOverhead message
  
  If E_SUCCESS is returned, the buffers pointed to by flow_id_ptr, 
  logical_chan_info_ptr and soft_combine_info_ptr are updated.
  Otherwise they are unchanged.
  
SIDE EFFECTS
  None.
===========================================================================*/
errno_enum_type hdrbcovhd_get_logical_chan_info
(
  uint8                                max_bcmcs_flow_ids,
  uint8                                *num_flows_ptr,
  uint32                               *bcmcs_flow_id_ptr,
  hdrbcovhd_logical_channel_info_type  *logical_chan_info_ptr,
  hdrbcovhd_soft_combine_info_type     *soft_combine_info_ptr
)
{
  int i; /* index */
  uint8 num_flows; /* Local variable storing how many flows there are */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (hdrbcovhd.item_ptr == NULL)
  {
    return E_FAILURE;
  }

  rex_enter_crit_sect( &hdrbcovhd.crit_sect );

  num_flows = hdrbcovhd.bc_msg.num_desired_flows;

  for (i = 0; i < hdrbcovhd.bc_msg.num_desired_flows; i++)
  {
    if (i >= max_bcmcs_flow_ids)
    {
      num_flows = max_bcmcs_flow_ids;
      break;
    }

    bcmcs_flow_id_ptr[i] = 
      hdrbcovhd.bc_msg.desired_flow[i].bcmcs_flow_id;

    soft_combine_info_ptr[i] = 
      hdrbcovhd.bc_msg.desired_flow[i].soft_comb_pn_info;
  }

  memcpy( logical_chan_info_ptr, hdrbcovhd.logical_channel_info, 
          num_flows * sizeof(hdrbcovhd_logical_channel_info_type) );

  *num_flows_ptr = num_flows;

  rex_leave_crit_sect( &hdrbcovhd.crit_sect );

  return E_SUCCESS;
} /* hdrbcovhd_get_logical_chan_info() */

/* <EJECT> */ 
/*===========================================================================
FUNCTION HDRBCOVHD_FLOWS_HAVE_SAME_LOGICAL_CHANNEL

DESCRIPTION
  Determines if the two FlowIDs given have the same logical channel
  
DEPENDENCIES
  
PARAMETERS
  bcmcs_flow_id1 - FlowID to compare
  bcmcs_flow_id2 - FlowID to compare

RETURN VALUE
  TRUE if the two flows are desired flows that both reference the same
  logcial channel information
  FALSE if either flow is not a desired flow or if the two flows do not
  share the same logical channel information
  
SIDE EFFECTS
  None.
===========================================================================*/
boolean hdrbcovhd_flows_have_same_logical_channel
(
  uint32 bcmcs_flow_id1,
  uint32 bcmcs_flow_id2
)
{
  int i; /* index */
  uint16 logical_channel_offset1 = 0; /* offset of one flow's lc info */
  uint16 logical_channel_offset2 = 0; /* offset of one flow's lc info */
  boolean flows_match = FALSE; /* If the logical channel info matches */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (hdrbcovhd.item_ptr == NULL)
  {
    return FALSE;
  }

  rex_enter_crit_sect( &hdrbcovhd.crit_sect );

  /* The same FlowID does not appear twice in the desired_flow array */

  for (i = 0; i < hdrbcovhd.bc_msg.num_desired_flows; i++)
  {
    if (hdrbcovhd.bc_msg.desired_flow[i].bcmcs_flow_id == bcmcs_flow_id1)
    {
      logical_channel_offset1 = 
        hdrbcovhd.bc_msg.desired_flow[i].logical_channel_offset;
    }

    if (hdrbcovhd.bc_msg.desired_flow[i].bcmcs_flow_id == bcmcs_flow_id2)
    {
      logical_channel_offset2 = 
        hdrbcovhd.bc_msg.desired_flow[i].logical_channel_offset;
    }

    /* Zero is not a valid logical_channel_offset */

    if ((logical_channel_offset1 != 0) &&
        (logical_channel_offset2 != 0))
    {
      break;
    }
  }

  if ((i < hdrbcovhd.bc_msg.num_desired_flows) &&
      (logical_channel_offset1 == logical_channel_offset2))
  {
    /* We found both flows and their logical channels were the same */
    flows_match = TRUE;
  }
  rex_leave_crit_sect( &hdrbcovhd.crit_sect );

  return flows_match;

} /* hdrbcovhd_flows_have_same_logical_channel() */

/* <EJECT> */ 
/*===========================================================================
FUNCTION HDRBCOVHD_GET_PHYS_CHAN_COUNT

DESCRIPTION
  Returns the phys_chan_count for the given FlowID
  
DEPENDENCIES
  
PARAMETERS
  flow_id - The FlowID of "desired flow" to look for.  This must be a flow
            that has been requested by an application.
  phys_chan_count_ptr - Location to return phys_chan_count into.          

RETURN VALUE
  E_SUCCESS if all data was extracted successfully
  E_FAILURE if hdrbcovhd has no BroadcastOverhead message
  E_NOT_AVAILABLE if the FlowID was not a desired available flow 
  E_NO_DATA if there was problem unpacking the data
  
  If E_SUCCESS is returned, phys_chan_count_ptr is set to the phys_chan_count 
  of the given FlowID. Otherwise phys_chan_count_ptr is unchanged.
  desired.
  
SIDE EFFECTS
  None.
===========================================================================*/
errno_enum_type hdrbcovhd_get_phys_chan_count
(
  uint32 flow_id,
  uint8  *phys_chan_count_ptr
)
{
  int i; /* index */
  hdrbcovhd_flow_offsets_type *desired_flow_ptr; /* Convenience pointer */
  uint8 phys_chan_count = 0; /* Physical channel count for given FlowID */
  errno_enum_type err_number = E_FAILURE; /* error to be returned */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (hdrbcovhd.item_ptr == NULL)
  {
    return E_FAILURE;
  }

  rex_enter_crit_sect( &hdrbcovhd.crit_sect );
  for (i = 0; i < hdrbcovhd.bc_msg.num_desired_flows; i++)
  {
    if (hdrbcovhd.bc_msg.desired_flow[i].bcmcs_flow_id == flow_id)
    {
      desired_flow_ptr = &hdrbcovhd.bc_msg.desired_flow[i];

      err_number = HDRBIT_UNPACK8( hdrbcovhd.item_ptr, 
                              &phys_chan_count,
                              desired_flow_ptr->logical_channel_offset, 
                              HDRBCOVHD_OVHD_PHYS_CHAN_COUNT_SIZE );
      *phys_chan_count_ptr = phys_chan_count;
      break;
    }
  }

  if (i == hdrbcovhd.bc_msg.num_desired_flows)
  {
    err_number = E_NOT_AVAILABLE;
  }
  rex_leave_crit_sect( &hdrbcovhd.crit_sect );

  return err_number;
} /* hdrbcovhd_get_phys_chan_count() */

/* <EJECT> */ 
/*===========================================================================
FUNCTION HDRBCOVHD_GET_FLOW_REG_INFO

DESCRIPTION
  Tells if a flow specified to be registered for paging and/or dynamic
  broadcast.
  
DEPENDENCIES
  None.
  
PARAMETERS
  flow_id - The FlowID of "desired flow" to look for.  This must be a flow
            that has been requested by an application.
  reg_for_paging_ptr     - Variable to return reg_for_paging value in
  reg_for_dynamic_bc_ptr - Variable to return reg_for_dynamic_bc value in          

RETURN VALUE
  E_SUCCESS if all data was extracted successfully
  E_FAILURE if hdrbcovhd has no BroadcastOverhead message
  E_NOT_AVAILABLE if the FlowID was not a desired available flow 
  E_NO_DATA if there was problem unpacking the data
  
  If E_SUCCESS is returned, the values for reg_for_paging and
  reg_for_dynamic_bc for the given FlowID are returned through the pointers
  provided.
  
SIDE EFFECTS
  None.
===========================================================================*/
errno_enum_type hdrbcovhd_get_flow_reg_info
(
  uint32 flow_id,
  boolean *reg_for_paging_ptr,
  boolean *reg_for_dynamic_bc_ptr
)
{
  int index;                                                      /* index */
  uint16 offset;                             /* Offset into packed message */
  errno_enum_type err_number = E_SUCCESS;               /* error to be returned */
  uint8 reg_for_paging;                            /* register for paging? */ 
  uint8 reg_for_dynamic_bc;                    /* register for dynamic_bc? */ 
  boolean flow_was_found = FALSE;                  /* If we found the flow */
  hdrbcovhd_list_enum_type type;          /* Is desired flow "program only */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (hdrbcovhd.item_ptr == NULL)
  {
    return E_FAILURE;
  }

  rex_enter_crit_sect( &hdrbcovhd.crit_sect );

  flow_was_found = hdrbcovhd_flow_is_desired( flow_id, &type, &index );

  if (flow_was_found)
  {
    if (type == HDRBCOVHD_DESIRED_FLOW)
    {
      offset = hdrbcovhd.bc_msg.desired_flow[index].flow_offset;
    }
    else /* type == HDRBCOVHD_DESIRED_PROGRAM */
    {
      ASSERT( type == HDRBCOVHD_DESIRED_PROGRAM );
      offset = 
        hdrbcovhd.bc_msg.desired_program[index].additional_cdma_channel_offset;

      /* additional_cdma_channel_offset, for "program only" entries, points
       * to AdditionalCDMAChannelCount1.  The registration bits
       * RegisterForPaging1 and RegisterForDynamicBroadcast1 precede this
       * by two bits, so just subtract 2.
       */
      offset -= HDRBCOVHD_OVHD_REGISTER_FOR_DYNAMIC_BROADCAST_SIZE;
      offset -= HDRBCOVHD_OVHD_REGISTER_FOR_PAGING_SIZE;
    }

    do
    {
      /*-----------------------------------------------------------------------
         RegisterForPaging 
      -----------------------------------------------------------------------*/
      err_number = HDRBIT_UNPACK8( hdrbcovhd.item_ptr,
                              &reg_for_paging,
                              offset, 
                              HDRBCOVHD_OVHD_REGISTER_FOR_PAGING_SIZE);
      offset +=  HDRBCOVHD_OVHD_REGISTER_FOR_PAGING_SIZE;
      if (err_number != E_SUCCESS ) { break; };

      /*-----------------------------------------------------------------------
         RegisterForDynamicBroadcast 
      -----------------------------------------------------------------------*/
      err_number = HDRBIT_UNPACK8( hdrbcovhd.item_ptr,
                              &reg_for_dynamic_bc,
                              offset, 
                              HDRBCOVHD_OVHD_REGISTER_FOR_DYNAMIC_BROADCAST_SIZE);
      offset +=  HDRBCOVHD_OVHD_REGISTER_FOR_DYNAMIC_BROADCAST_SIZE;
      if (err_number != E_SUCCESS ) { break; };

      *reg_for_paging_ptr     =  ((reg_for_paging == 0)? FALSE : TRUE);
      *reg_for_dynamic_bc_ptr = ((reg_for_dynamic_bc == 0)? FALSE : TRUE);
    } while (0); /* use break; as goto */
  }

  if (!flow_was_found)
  {
    err_number = E_NOT_AVAILABLE;
  }
  else if (err_number != E_SUCCESS )
  {
    err_number = E_NO_DATA;
  }
  rex_leave_crit_sect( &hdrbcovhd.crit_sect );

  return err_number;
} /* hdrbcovhd_get_flow_reg_info() */

/* <EJECT> */ 
/*===========================================================================
FUNCTION HDRBCOVHD_GET_PROGRAM_ID_LSBS

DESCRIPTION
  Returns the ProgramIDLSBs for the given flow (format 1)
  
DEPENDENCIES
  None.
  
PARAMETERS
  flow_id - The FlowID of "desired flow" to look for.  This must be a flow
            that has been requested by an application.
  program_id_lsbs_ptr - Place to return ProgramIDLSBs info          

RETURN VALUE
  E_SUCCESS if all data was extracted successfully
  E_FAILURE if hdrbcovhd has no BroadcastOverhead message
  E_NOT_AVAILABLE if the FlowID was not a desired available flow 
  E_NO_DATA if there was problem unpacking the data
  
  If E_SUCCESS is returned, the ProgramIDLSBs value for the given FlowID 
  is returned through the pointer provided.
  
SIDE EFFECTS
  None.
===========================================================================*/
errno_enum_type hdrbcovhd_get_program_id_lsbs
(
  uint32 flow_id,
  uint32 *program_id_lsbs_ptr
)
{
  int index;                              /* index */
  uint16 offset;                          /* Offset into packed message */
  errno_enum_type err_number = E_SUCCESS;      /* error to be returned */
  uint8 program_id_lsb_length = 0;        /* */
  uint32 program_id_lsbs = 0;             /* */
  boolean flow_was_found = FALSE;         /* If we found the flow */
  hdrbcovhd_list_enum_type type;          /* Is desired flow "program only */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (hdrbcovhd.item_ptr == NULL)
  {
    return E_FAILURE;
  }

  if (hdrbcovhd.bc_msg.head.bcmcs_flow_format != 
      HDRBCOVHD_FLOW_FORMAT_1)
  {
    HDR_MSG_PROT(MSG_LEGACY_ERROR,"No pid lsbs in format 0!");
    return E_FAILURE;
  }

  rex_enter_crit_sect( &hdrbcovhd.crit_sect );
  flow_was_found = hdrbcovhd_flow_is_desired( flow_id, &type, &index );

  if (flow_was_found)
  {
    if (type == HDRBCOVHD_DESIRED_FLOW)
    {
      offset = hdrbcovhd.bc_msg.desired_flow[index].program_offset;
    }
    else /* type == HDRBCOVHD_DESIRED_PROGRAM */
    {
      ASSERT( type == HDRBCOVHD_DESIRED_PROGRAM );
      offset = hdrbcovhd.bc_msg.desired_program[index].program_offset;
    }

    do
    {
      /*-----------------------------------------------------------------------
        ProgramIDLSBLength
      -----------------------------------------------------------------------*/
      err_number = HDRBIT_UNPACK8( hdrbcovhd.item_ptr, 
                              &program_id_lsb_length, 
                              offset,
                              HDRBCOVHD_OVHD_PROGRAM_ID_LSB_LENGTH_SIZE );

      offset += HDRBCOVHD_OVHD_PROGRAM_ID_LSB_LENGTH_SIZE;

      if (err_number != E_SUCCESS ) { break; }

      if ( program_id_lsb_length != 0 )
      {
        /*-----------------------------------------------------------------------
          ProgramIDLSBs
        -----------------------------------------------------------------------*/
        err_number = HDRBIT_UNPACK32( hdrbcovhd.item_ptr, 
                                 &program_id_lsbs, 
                                 offset,
                                 program_id_lsb_length );

        offset += program_id_lsb_length;
        if (err_number != E_SUCCESS ) { break; }
      }

      *program_id_lsbs_ptr = program_id_lsbs;
    } while (0);  /* Use break; as "goto" */
  }

  if (!flow_was_found)
  {
    err_number = E_NOT_AVAILABLE;
  }
  else if (err_number != E_SUCCESS )
  {
    err_number = E_NO_DATA;
  }
  rex_leave_crit_sect( &hdrbcovhd.crit_sect );

  return err_number;
} /* hdrbcovhd_get_program_id_lsbs() */

/* <EJECT> */ 
/*===========================================================================
FUNCTION HDRBCOVHD_GET_ADDITIONAL_CDMA_CHANNEL_LIST

DESCRIPTION
  Returns the AdditionalCDMAChannel fields for the given flow
  
DEPENDENCIES
  
PARAMETERS
  flow_id - The FlowID of "desired flow" to look for.  This must be a flow
            that has been requested by an application.
  add_chan_count_ptr - Place to return AdditionalChannelCount into
  add_chan_ptr - Pointer to array to put AdditionalChannel records into.
  add_chan_max - Maximum number of AdditionalRecords to return.

RETURN VALUE
  E_SUCCESS if all the channels were extracted successfully
  E_FAILURE if hdrbcovhd has no BroadcastOverhead message
  E_NOT_AVAILABLE if the FlowID was not a desired available flow 
  E_NO_DATA if there was a problem extracting values
  E_DATA_TOO_LARGE if there were more channels than add_chan_max
  
  If E_SUCCESS or E_DATA_TOO_LARGE is returned, add_chan_count_ptr is set 
  to the number of valid additional channels that were extracted and 
  add_chan_ptr array is the list of channels.
  Otherwise, add_chan_count_ptr is set to zero and add_chan_ptr array
  may have been overwritten with inconsistent data.
  
SIDE EFFECTS
  None.
===========================================================================*/
errno_enum_type hdrbcovhd_get_additional_cdma_channel_list
(
  uint32 flow_id,
  uint8  *add_chan_count_ptr,
  sys_hdr_channel_record_s_type *add_chan_ptr,
  uint8 add_chan_max
)
{
  int index;                                                      /* index */
  int add_chan;                     /* Index into additional channel array */
  errno_enum_type err_number = E_FAILURE;               /* error to be returned */
  boolean channel_is_valid;   /* If the channel is a supported HDR channel */
  uint16 offset = 0;                         /* Offset into packed message */
  uint8 add_chan_index = 0;         /* Valid additional channels to return */
  hdrbcovhd_list_enum_type type;          /* Is desired flow "program only */

  /* These variables are temporary storage for extracted message fields    */
  uint8  add_cdma_chan_count;     /* Additional channel count */
  uint8  system_type;             /* SystemType               */
  uint8  channel_band;            /* Band                     */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (hdrbcovhd.item_ptr == NULL)
  {
    return E_FAILURE;
  }

  rex_enter_crit_sect( &hdrbcovhd.crit_sect );

  if (hdrbcovhd_flow_is_desired( flow_id, &type, &index ))
  {
    if (type == HDRBCOVHD_DESIRED_FLOW)
    {
      offset = hdrbcovhd.bc_msg.desired_flow[index].additional_cdma_channel_offset;
    }
    else /* type == HDRBCOVHD_DESIRED_PROGRAM */
    {
      ASSERT( type == HDRBCOVHD_DESIRED_PROGRAM );
      offset = hdrbcovhd.bc_msg.desired_program[index].additional_cdma_channel_offset;
    }

    /*---------------------------------------------------------------------
      AdditionalCDMAChannelCount
    ---------------------------------------------------------------------*/
    err_number = HDRBIT_UNPACK8( hdrbcovhd.item_ptr,
                            &add_cdma_chan_count,
                            offset, 
                            HDRBCOVHD_OVHD_ADD_CDMA_CHAN_COUNT_SIZE );

    offset += HDRBCOVHD_OVHD_ADD_CDMA_CHAN_COUNT_SIZE;
    if (err_number != E_SUCCESS)
    { 
      add_cdma_chan_count = 0;
    }

    for ( add_chan = 0; add_chan < add_cdma_chan_count; add_chan++ )
    {
      channel_is_valid = TRUE;
      /*---------------------------------------------------------------------
        ChannelMode
      ---------------------------------------------------------------------*/
      err_number = HDRBIT_UNPACK8( hdrbcovhd.item_ptr, &system_type, offset,                                  
                                    HDRBCOVHD_OVHD_SYSTEM_TYPE_SIZE );
      offset += HDRBCOVHD_OVHD_SYSTEM_TYPE_SIZE;
      if (err_number != E_SUCCESS){ break; };

      /*---------------------------------------------------------------------
        ChannelBand
      ---------------------------------------------------------------------*/
      err_number = HDRBIT_UNPACK8( hdrbcovhd.item_ptr, &channel_band, offset,
                                    HDRBCOVHD_OVHD_CHANNEL_BAND_CLASS_SIZE );
      offset += HDRBCOVHD_OVHD_CHANNEL_BAND_CLASS_SIZE;
      if (err_number != E_SUCCESS){ break; };

      /* Validate system type and band before proceeding */
      if( system_type != (uint8) SYS_HDR_CHAN_REC_MODE_IS856 )
      {
        HDR_MSG_PROT_1( MSG_LEGACY_ERROR,
                        "BC Ovhd. Invalid system_type:%d", system_type );
        channel_is_valid = FALSE;
      }
      else if ((channel_band >= SYS_BAND_CLASS_MAX) ||
               (!hdrutil_is_band_supported((sys_band_class_e_type)channel_band)))
      {
        HDR_MSG_PROT_1( MSG_LEGACY_ERROR,
                        "BC Ovhd. Invalid band:%d", channel_band );
        channel_is_valid = FALSE;
      }

      if (channel_is_valid)
      {
        /* OK, channel is valid */
        add_chan_index++;
        if (add_chan_index > add_chan_max)
        {
          /* We have more channels than we can return.  Stop now. */
          err_number = E_DATA_TOO_LARGE;
          break;
        }
        add_chan_ptr->mode =  (sys_hdr_chan_rec_mode_e_type) system_type;
        add_chan_ptr->channel.band = (sys_band_class_e_type) channel_band;
        /*-------------------------------------------------------------------
          ChannelNumber
        -------------------------------------------------------------------*/
        err_number = HDRBIT_UNPACK16( hdrbcovhd.item_ptr, 
                                       &add_chan_ptr->channel.chan_num,
                                       offset,
                                       HDRBCOVHD_OVHD_CHANNEL_NUMBER_SIZE );
        if (err_number != E_SUCCESS){ break; };
        add_chan_ptr++;
      }
      offset += HDRBCOVHD_OVHD_CHANNEL_NUMBER_SIZE;
    }
  }
  rex_leave_crit_sect( &hdrbcovhd.crit_sect );

  *add_chan_count_ptr = add_chan_index;

  if ((err_number != E_SUCCESS) && (err_number != E_DATA_TOO_LARGE))
  {
    err_number = E_NO_DATA;
  }

  return err_number;

} /* hdrbcovhd_get_additional_cdma_channel_list() */

/*===========================================================================
FUNCTION HDRBCOVHD_IS_CHANNEL_IN_ADDITIONAL_CDMA_CHANNEL_LIST

DESCRIPTION
  Checks to see if a channel is available in the additional cdma channel list
  for a particular flow id.
  
DEPENDENCIES
  
PARAMETERS
  flow_id - The FlowID of "desired flow" to look for.  This must be a flow
            that has been requested by an application.
  channel - The channel to look up in a flow's additional CDMA channel list.

RETURN VALUE
  TRUE  - Channel was found in the channel list.
  FALSE - Channel was not found or there was an error in unpacking the
          channel info record.

SIDE EFFECTS
  None.
===========================================================================*/
boolean hdrbcovhd_is_channel_in_additional_cdma_channel_list
(
  uint32 flow_id,
  /*lint -e{550} suppress "parameter set but not used" */
  sys_channel_type channel
)
{
  int index;                                                      /* index */
  int add_chan;                     /* Index into additional channel array */
  errno_enum_type err_number = E_FAILURE;               /* error to be returned */
  boolean channel_is_valid;   /* If the channel is a supported HDR channel */
  uint16 offset = 0;                         /* Offset into packed message */
  hdrbcovhd_list_enum_type type;          /* Is desired flow "program only */

  /* These variables are temporary storage for extracted message fields    */
  uint8  add_cdma_chan_count;     /* Additional channel count */
  uint8  system_type;             /* SystemType               */
  uint8  channel_band;            /* Band                     */
  uint16 channel_number;          /* Channel Number */

  boolean channel_found = FALSE;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (hdrbcovhd.item_ptr == NULL)
  {
    return channel_found;
  }

  rex_enter_crit_sect( &hdrbcovhd.crit_sect );

  if (hdrbcovhd_flow_is_desired( flow_id, &type, &index ))
  {
    if (type == HDRBCOVHD_DESIRED_FLOW)
    {
      offset = hdrbcovhd.bc_msg.desired_flow[index].additional_cdma_channel_offset;
    }
    else /* type == HDRBCOVHD_DESIRED_PROGRAM */
    {
      ASSERT( type == HDRBCOVHD_DESIRED_PROGRAM );
      offset = hdrbcovhd.bc_msg.desired_program[index].additional_cdma_channel_offset;
    }

    /*---------------------------------------------------------------------
      AdditionalCDMAChannelCount
    ---------------------------------------------------------------------*/
    err_number = HDRBIT_UNPACK8( hdrbcovhd.item_ptr,
                            &add_cdma_chan_count,
                            offset, 
                            HDRBCOVHD_OVHD_ADD_CDMA_CHAN_COUNT_SIZE );

    offset += HDRBCOVHD_OVHD_ADD_CDMA_CHAN_COUNT_SIZE;
    if (err_number != E_SUCCESS)
    { 
      add_cdma_chan_count = 0;
    }

    for ( add_chan = 0; add_chan < add_cdma_chan_count; add_chan++ )
    {
      channel_is_valid = TRUE;
      /*---------------------------------------------------------------------
        ChannelMode
      ---------------------------------------------------------------------*/
      err_number = HDRBIT_UNPACK8( hdrbcovhd.item_ptr, &system_type, offset,                                  
                                    HDRBCOVHD_OVHD_SYSTEM_TYPE_SIZE );
      offset += HDRBCOVHD_OVHD_SYSTEM_TYPE_SIZE;
      if (err_number != E_SUCCESS){ break; };

      /*---------------------------------------------------------------------
        ChannelBand
      ---------------------------------------------------------------------*/
      err_number = HDRBIT_UNPACK8( hdrbcovhd.item_ptr, &channel_band, offset,
                                    HDRBCOVHD_OVHD_CHANNEL_BAND_CLASS_SIZE );
      offset += HDRBCOVHD_OVHD_CHANNEL_BAND_CLASS_SIZE;
      if (err_number != E_SUCCESS){ break; };

      /* Validate system type and band before proceeding */
      if( system_type != (uint8) SYS_HDR_CHAN_REC_MODE_IS856 )
      {
        HDR_MSG_PROT_1( MSG_LEGACY_ERROR,
                        "BC Ovhd. Invalid system_type:%d", system_type );
        channel_is_valid = FALSE;
      }
      else if ((channel_band >= SYS_BAND_CLASS_MAX) ||
               (!hdrutil_is_band_supported((sys_band_class_e_type)channel_band)))
      {
        HDR_MSG_PROT_1( MSG_LEGACY_ERROR,
                        "BC Ovhd. Invalid band:%d", channel_band );
        channel_is_valid = FALSE;
      }

      if (channel_is_valid)
      {
        /*-------------------------------------------------------------------
          ChannelNumber
        -------------------------------------------------------------------*/
        err_number = HDRBIT_UNPACK16( hdrbcovhd.item_ptr,
                                 &channel_number,
                                 offset,
                                 HDRBCOVHD_OVHD_CHANNEL_NUMBER_SIZE );
        if (err_number != E_SUCCESS){ break; };

        if ( (channel.band == (sys_band_class_e_type) channel_band) &&
             (channel.chan_num == (sys_channel_num_type) channel_number) )
        {
          /* The channel we were looking for was found in the additional
             CDMA channel list of this flow. */
          channel_found = TRUE;
          break;
        }

      }
      offset += HDRBCOVHD_OVHD_CHANNEL_NUMBER_SIZE;
    }
  }
  rex_leave_crit_sect( &hdrbcovhd.crit_sect );

  return channel_found;

} /* hdrbcovhd_is_channel_in_cdma_channel_list() */

/* <EJECT> */ 
/*=============================================================================

FUNCTION HDRBCOVHD_GET_FLOW_FORMAT

DESCRIPTION
  Accessor function to query the flow format

DEPENDENCIES
  None

PARAMETERS
  flow_format_ptr - returns the flow format

RETURN VALUE
  E_SUCCESS - hdr ovhd has a Broadcast Overhead message
  E_FAILURE - no broadcast overhead message

SIDE EFFECTS
  None

=============================================================================*/
errno_enum_type hdrbcovhd_get_flow_format
( 
  uint8 *flow_format_ptr 
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (hdrbcovhd.item_ptr == NULL)
  {
    return E_FAILURE;
  }

  *flow_format_ptr = hdrbcovhd.bc_msg.head.bcmcs_flow_format;

  return E_SUCCESS;

} /* hdrbcovhd_get_flow_format() */

/* <EJECT> */ 
/*=============================================================================

FUNCTION HDRBCOVHD_MSG_IS_UP_TO_DATE

DESCRIPTION
  Returns whether or not the bc ovhd message is up to date.

DEPENDENCIES
  None

PARAMETERS
  bc_item_ptr - Pointer to the bc item pointer

RETURN VALUE
  TRUE  - bc ovhd message is up to date
  FALSE - otherwise

SIDE EFFECTS
  None

=============================================================================*/
boolean hdrbcovhd_msg_is_up_to_date
(
  dsm_item_type *bc_item_ptr
)
{

  if ( hdrbcovhd.item_ptr == NULL )    
  {
    HDR_MSG_PROT(MSG_LEGACY_MED,"bcovhd not up to date: NULL hdrbcovhd item_ptr");
    return FALSE;
  }
  else if (bc_item_ptr == NULL)
  {
    HDR_MSG_PROT(MSG_LEGACY_MED,"bcovhd not up to date: NULL bc_item_ptr cache entry");
    return FALSE;
  }


  /* The bc ovhd message is stored as a dupped version of the bc ovhd message
     stored in the overhead cache. If the dup pointer of the bcovhd broadcast 
     overhead message points to the same dsm item as the Broadcast Overhead 
     dsm item currently stored in the ovhd cache, then the message is current*/
  if (bc_item_ptr->dup_ptr != NULL)
  {
    HDR_MSG_PROT_2(MSG_LEGACY_MED,
                   "msg up to date? bc_item dup_ptr:%d hdrbcovhd dup_ptr:%d",
                   bc_item_ptr->dup_ptr, hdrbcovhd.item_ptr->dup_ptr);
    return ( (bc_item_ptr->dup_ptr == 
                hdrbcovhd.item_ptr->dup_ptr) ? TRUE : FALSE);
  }
  else
  {
    HDR_MSG_PROT_2(MSG_LEGACY_MED,
                   "msg up to date? bc_item ptr:%d hdrbcovhd dup_ptr:%d",
                   bc_item_ptr, hdrbcovhd.item_ptr->dup_ptr);
    return ( (bc_item_ptr == 
              hdrbcovhd.item_ptr->dup_ptr) ? TRUE : FALSE);
  }


} /* hdrbcovhd_msg_is_up_to_date() */

#endif /* FEATURE_HDR_BCMCS */

#ifdef FEATURE_HDR_BCMCS_2p0
#error code not present
#endif /* FEATURE_HDR_BCMCS_2p0 */

