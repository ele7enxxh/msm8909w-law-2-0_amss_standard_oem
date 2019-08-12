#ifndef HDRDSM_H
#define HDRDSM_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

  H D R   D S M   I T E M   H E A D E R   A C C E S S O R   S E R V I C E S       

GENERAL DESCRIPTION
  This module contains macros that provide access to the sub-fields within
  the 32-bit appl. defined field and the kind field in the dsm item header.  
  The app_field sub-fields include the link id, MAC trailer, connection layer 
  format, Physical layer revision, last item, slots to decode, DRC, broadcast,
  sync bit, hunt bit, channel type bit and half-slot packet timestamp. The 
  least significant seven bits of kind field stores the destination protocol 
  Id and the most significant bit stores the traffic bit field. 
  

EXTERNALIZED MACROS
  If these fields in every DSM item of a chain of DSM items needs to be set to
  same value, it would be more efficient to use the "HDRDSM_SET_USER_FIELD_XXXX"
  macros to set these fields into a temporary user field and then copying this 
  field to each of the DSM item in the chain using the macro 
  "HDRDSM_SET_USER_FIELD". The macros are arranged in the order of their bit 
  positions in the app_field.

  HDRDSM_CLR_USER_FIELD
    Clear dsm item appl. defined field.    
  
  HDRDSM_GET_USER_FIELD
    Get dsm item appl. defined field.
    
  HDRDSM_SET_USER_FIELD
    Set appl. defined field in specified dsm item.
  
  HDRDSM_GET_LINK_ID
    Get link ID from specified dsm item.
    
  HDRDSM_SET_LINK_ID
    Set link ID in specified dsm item.

  HDRDSM_SET_USER_FIELD_LINK_ID
    Set Link ID in the temporary user field. 

  HDRDSM_IS_CONN_FORMAT_A
    Finds if the enclosed connection layer packet is Format A.

  HDRDSM_SET_CONN_FORMAT
    Sets the connection layer format in specified dsm item.

  HDRDSM_GET_MAC_TRAILER  
    Gets MAC trailer from specified dsm item.

  HDRDSM_SET_MAC_TRAILER
    Sets the MAC trailer in specified dsm item.

  HDRDSM_SET_USER_FIELD_MAC_TRAILER
    Sets the MAC trailer in temporary user field. 

  HDRDSM_IS_REV_A
    Finds if the packet's physical layer version is Rev-A.

  HDRDSM_SET_REV_A
    Sets the physical layer version to Revision-A in the specified dsm item.

  HDRDSM_SET_USER_FIELD_REV_A
    Sets the physical layer version to Revision-A in temporary user field. 

  HDRDSM_IS_LAST_ITEM
    Finds if the specified dsm item is the last item of current packet's
    dsm chain.

  HDRDSM_CLR_LAST_ITEM
    Clears the specified dsm item as last item of current packet's dsm chain.

  HDRDSM_SET_LAST_ITEM
    Marks the specified dsm item as last item of current packet's dsm chain. 

  HDRDSM_GET_SLOTS_TO_DECODE
    Gets the number of slots taken to decode from the specified dsm item.

  HDRDSM_SET_SLOTS_TO_DECODE
    Sets the number of slots taken to decode in the specified dsm item.
    
  HDRDSM_SET_USER_FIELD_SLOTS_TO_DECODE
    Sets the number of slots taken to decode in the temporary user field. 
     
  HDRDSM_GET_DRC
    Get DRC rate from specified dsm item.

  HDRDSM_SET_DRC
    Set DRC rate in specified dsm item.

  HDRDSM_SET_USER_FIELD_DRC
    Set DRC in the specified temporary user field. 
     
  HDRDSM_GET_BCAST_BIT
    Get broadcast bit from specified dsm item.
    
  HDRDSM_SET_BCAST_BIT 
    Set broadcast bit in specified dsm item. 
    
  HDRDSM_GET_HUNT_BIT
    Get hunt status bit from specified dsm item.
    
  HDRDSM_SET_HUNT_BIT 
    Set hunt status bit in specified dsm item.
    
  HDRDSM_GET_CC_SYNC_BIT
    Get CC sync bit from specified dsm item.
    
  HDRDSM_SET_CC_SYNC_BIT 
    Set CC sync bit in specified dsm item.

  HDRDSM_GET_CHAN_BIT
    Get channel bit from specified dsm item.
    
  HDRDSM_SET_CHAN_BIT 
    Set channel bit in specified dsm item.

  HDRDSM_SET_USER_FIELD_CHAN
    Set channel in the temporary user field. 

  HDRDSM_GET_TIMESTAMP
    Get timestamp from specified dsm item.
    
  HDRDSM_SET_TIMESTAMP 
    Set timestamp in specified dsm item.

  HDRDSM_SET_USER_FIELD_TIMESTAMP
    Set the timestamp in the temporary user field. 
    
  HDRDSM_GET_TRAFFIC_BIT
    Gets the traffic bit from the specified dsm item.

  HDRDSM_SET_TRAFFIC_BIT
    Sets the traffic bit in the specified dsm item. 

  HDRDSM_GET_PACKET_SIZE
    Gets the size of the physical layer packet 

  HDRDSM_SET_PACKET_SIZE
    Sets the size of the physical layer packet

  HDRDSM_GET_PROT_ID
    Get protocol ID from specified dsm item.
    
  HDRDSM_SET_PROT_ID
    Set protocol ID in specified dsm item. 

  HDRDSM_CLEAR_APP_PTR
    This macro clears the app_ptr to NULL.

  HDRDSM_GET_CARRIER_ID
    Get the carrier ID field from the specified DSM item header.

  HDRDSM_SET_CARRIER_ID
    Set the Carrier ID field in the specified DSM item header to the 
    specified value.

  HDRDSM_GET_SCHED_GROUP_ID
    Get the Scheduler Group ID field from the specified DSM item header.

  HDRDSM_SET_SCHED_GROUP_ID
    Set the Scheduler Group ID field in the specified DSM item header to 
    the specified value.

  HDRDSM_MARK_LAST_OF_SIMUL_PACKETS
    Marks this DSM item as the last of all simultaneously detected packets.

  HDRDSM_IS_LAST_OF_SIMUL_PACKETS
    Checks if a DSM item is the last of all simultaneously detected packets.

  HDRDSM_GET_REUSE_DATA_PATH_ITEM 
    Obtains a reusable DSM item
  
  HDRDSM_FREE_REUSE_DATA_PATH_ITEM   
    Returns a reusable DSM item to the local re-use pool.
    
  HDRDSM_REUSE_DATA_PATH_ITEMS_CLEANUP 
    Cleans up the local DSM item re-use pool and returns it to the global 
    DSM item pool.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

  Copyright (c) 2000, 2001, 2002 by Qualcomm Technologies, Inc.  All Rights Reserved.
  Copyright (c) 2003, 2004, 2005 by Qualcomm Technologies, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/common/api/hdrdsm.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/01/14   vko     Added HDRDSM_TOUCH_ITEM.
05/22/13   kss     Increased pkt_type field to 5 bits to match types in hdrdec
10/24/11   smd     Moved more to SU API header files.
09/23/11   smd     SU API cleanup.
12/03/08   kss     Removed DSM reuse optimization macros.
06/11/08   cc/etv  Moved PacketSize from DSM's Kind field to AppPtr field.
12/04/07   etv     Added HDRDSM_COPY_APP_PTR_TO_DUPD_ITEM.
10/10/07   etv     Minor updates after code review comments.
09/10/07   etv     Added a bunch of user field macros and macros for 
                   NumConcurrentPkts.
07/05/07   etv     Added new DSM macros for Rev-B.
03/09/07   pba     Removed obsolete tracer field, replaced with DSM_TOUCH_ITEM.
07/26/05   etv     Added macros to get/set packet type.
04/22/05   etv     Added two new macros to get/set physical layer packet size. 
03/29/05   etv     Added macros to support Rev-A and minor cleanup.
05/20/04   kss     Added HDRDSM_GET_FREE_COUNT() macro.
04/13/04   mpa     Added traffic bit field by using prot id's unused 8th bit
02/17/04   rsl     Fixed reuse_data_path_item to work with DSM 2.0.  Added new
                   function reuse_data_path_packet to free packet chains.
09/10/03   mpa     Converted F3 messages to use new MSG2.0 SSID
07/10/02   dna     Clear the app_field of re-used DSM items
05/20/02   kss/vr  Changed macros GET/SET_HUNT_BIT to use TC_HUNT_BIT instead
                   of FTC_HUNT_BIT
04/11/02   vas     Fixed INTLOCKing in the macros used for DSM reuse.
11/16/01   rao     Changed dsm reuse stack clean up code, to fix the 
                   remaining edge case issue.
11/15/01   rao     Fixed dsmreuse_stack.count becoming bogus problem.
11/13/01   rao     check for dsmreuse_stack.count > 0 before decrementing it.
11/08/01   rao     Added DSM REUSE Optimization Macros.
05/30/01   kss     Added DRC and CC Sync bit fields. 
03/30/00   st      Created.

===========================================================================*/
#include "hdr_variation.h"
#include "hdrcom_api.h"

/*===========================================================================

                LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/*---------------------------------------------------------------------------
                        DSM ITEM TRACER FIELD
  The tracer field is used by data services and signaling protocols.
  Values below 0x80 are reserved for data services.
  Whenever a protocol allocates a new item or dupes another item, it
  should set the tracer on the new/duped item according to the following
  values:
---------------------------------------------------------------------------*/

#define HDRDSM_RX_DEC         0x80
#define HDRDSM_RX_CMAC        0x81
#define HDRDSM_RX_FMAC        0x82
#define HDRDSM_RX_PCP         0x83
#define HDRDSM_RX_SLP         0x84
#define HDRDSM_RX_HMP         0x85

#define HDRDSM_TX_SIGPROT     0x90
#define HDRDSM_TX_HMP         0x91
#define HDRDSM_TX_SLP         0x92
#define HDRDSM_TX_SLP_FRG     0x93
#define HDRDSM_TX_SLP_REXMIT  0x94
#define HDRDSM_TX_STREAM1     0x95
#define HDRDSM_TX_STREAM2     0x96
#define HDRDSM_TX_STREAM3     0x97
#define HDRDSM_TX_PCP         0x98
#define HDRDSM_TX_AMAC        0x99
#define HDRDSM_TX_RMAC        0x9a

#define HDRDSM_RLP_RX          0xa0       /* RLP Receive */
#define HDRDSM_RLP_RXRSQ       0xa1       /* RLP Rx Resequencing Queue */
#define HDRDSM_RLP_RX_DELIVERY 0xa2       /* RLP Rx to upper layer */

/*---------------------------------------------------------------------------
                        DSM ITEM APP_FIELD FIELD

  |31|30|29|28|27|26|25|24|23|22|21|20|19|18|17|16|
  -------------------------------------------------
  |       Link ID         |AB|XX|RL|LT|slot to dec|
  -------------------------------------------------

  |15|14|13|12|11|10| 9| 8| 7| 6| 5| 4| 3| 2| 1| 0|
  -------------------------------------------------
  |    DRC    |BC|SH|CH|         Timestamp        |
  -------------------------------------------------

  Bit pos.    Field Description and Size
  =======     ==========================
  [31-24] -   Link ID (8 bits) 
  [23]    -   AB - Connection Layer format (0 - A, 1 - B). Used between 
              FMAC-PCP (1 bit)
  [22]    -   XX - Reserved (1 bit)
  [23-22] -   MAC Trailer. The bits 23-22 are overloaded with MAC trailer 
              between Rx-FMAC. (2 bits)
  [21]    -   RL - Physical Layer revision (0 - Release 0; 1 - Revision A)
              (1 bit)
  [20]    -   LT - Last bit. Indicates whether the DSM item is the last item 
              in the current packet's dsm chain.( 0 - Not last; 1 - Last item ) 
              (1 bit)
  [19-16] -   Slots to Decode - Number of slots taken by decoder to decode.
              (1 bit)
  [15-12] -   DRC (4 bits)
  [11]    -   BC - Broadcast (1 bit)
  [10]    -   SH - Sync / Hunt bit (1 bit)
  [9]     -   CH - Channel (1 bit)
  [8-0]   -   Timestamp (9 bits)

  Note
  The bit 23 is overloaded by FMAC to pass the connection layer format to PCP
---------------------------------------------------------------------------*/

#define MAC_TRAILER_MASK    0x00C00000UL
#define MAC_TRAILER_LSB     22

#define CONN_FORMAT_MASK    0x00800000UL
#define CONN_FORMAT_LSB     23

#ifdef FEATURE_HDR_REVA_L1

#define PHY_LAYER_REV_MASK  0x00200000UL
#define PHY_LAYER_REV_LSB   21

#endif /* FEATURE_HDR_REVA_L1 */

#define LAST_ITEM_MASK      0x00100000UL
#define LAST_ITEM_LSB       20

#define SLOTS_DEC_MASK      0x000F0000UL
#define SLOTS_DEC_LSB       16

#define BCAST_BIT_MASK      0x00000800UL
#define BCAST_BIT_LSB       11

#define TC_HUNT_BIT_MASK    0x00000400UL
#define TC_HUNT_BIT_LSB     10

#define CC_SYNC_BIT_MASK    0x00000400UL
#define CC_SYNC_BIT_LSB     10

#define TIMESTAMP_MASK      0x000001FFUL
#define TIMESTAMP_LSB       0


 /* EJECT */
/*---------------------------------------------------------------------------
                        DSM ITEM KIND FIELD

 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 | 
 ---------------------------------
 |TR |    Protocol ID (7 bits)   |
 ---------------------------------
     |  Pkt Type         | Rsvd  | 
     ----------------------------- 
 
 [7]   -  TR - Traffic bit - Indicates whether the packet is received while
          in traffic or while idle (1 bit)
 [6-2] -  Packet Type. The bits 2-6 are used to store the physical layer
          packet type. The packet types are encoded as defined in hdrdec.
 [6-0] -  Protocol ID (7 bits) 

 Note: Bit positions reserved for protocol ID are overloaded as Pkt type and 
 Pkt size fields since their usage is disjoint.
---------------------------------------------------------------------------*/

#define TRAFFIC_BIT_MASK    0x80
#define TRAFFIC_BIT_LSB     7 

#define PKT_TYPE_MASK       0x7C
#define PKT_TYPE_LSB        2


/*---------------------------------------------------------------------------
                        DSM ITEM APP_PTR FIELD

  The app_ptr field is used as a 32-bit user field.


  |31|30|29|28|27|26|25|24|23|22|21|20|19|18|17|16|
  -------------------------------------------------
  |             R E S E R V E D       |  Pkt Size |
  -------------------------------------------------

  |15|14|13|12|11|10| 9| 8| 7| 6| 5| 4| 3| 2| 1| 0|
  -------------------------------------------------
  |E |        |XXXXX| CARRIER ID| SCHEDULER GRP ID |
  -------------------------------------------------
  |  CON PKT  |
  -------------

  Note: X denotes reserved bits.

  Bit pos.    Field Description and Size
  =======     ==========================
  [19-16] -   Packet size. The 4 bits are used to store the rev B
              physical layer packet size. The packet sizes are encoded with the 
              following scheme.
              0  - 128 bits
              1  - 256 bits
              2  - 512 bits
              3  - 1024 bits
              4  - 2048 bits
              5  - 3072 bits
              6  - 4096 bits
              7  - 5120 bits
              8  - 6144 bits
              9  - 7168 bits
              10 - 8192 bits
  [15]    -   End bit (1 bit). Set to 1, if this DSM item is the last of 
              all simultaneously detected preambles and zero otherwise. 
              If there are no other simultaneously detected preambles, 
              this bit is always set to 1, indicating that the DSM item
              is ready to be processed.
  [12-15] -   CON PKT(4 bits). The number of packets detected on the same 
              slot. Normal range is 1..DEMOD_CARRIERS.
  [9-6]   -   Carrier ID (4 bits)
  [5-0]   -   Scheduler Group ID (6 bits)

---------------------------------------------------------------------------*/
#define SIMUL_PKT_MASK        0x0000F000UL
#define SIMUL_PKT_LSB         12

#define END_BIT_MASK          0x00008000UL
#define END_BIT_LSB           15
  
#define PKT_SIZE_MASK         0x000F0000UL
#define PKT_SIZE_LSB          16

/*-------------------------------------------------------------------------*/

#ifdef FEATURE_DSM_MEM_CHK
  #define DSM_TAIL_COOKIE_LEN 4 /* sizeof(uint32), but preprocessor chokes */
  #define DSM_HEAD_COOKIE_LEN 4
#else
  #define DSM_TAIL_COOKIE_LEN 0 
  #define DSM_HEAD_COOKIE_LEN 0
#endif /* FEATURE_DSM_MEM_CHK */


/* Connection Layer Format A */
#define HDRDSM_PACKET_IS_FMT_A  0

/* Connection Layer Format B */
#define HDRDSM_PACKET_IS_FMT_B  1


 /* EJECT */
/*===========================================================================
MACRO HDRDSM_CLR_USER_FIELD

DESCRIPTION
  This macro sets the dsm item header appl. defined field to zero. 

PARAMETERS
  item_ptr - address of dsm item

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
#define HDRDSM_CLR_USER_FIELD( item_ptr )\
   ( (item_ptr)->app_field = 0 )


 /* EJECT */
/*===========================================================================
MACRO HDRDSM_GET_USER_FIELD_APP_FIELD

DESCRIPTION
  This macro gets the current value of the appl. defined field from the 
  specified dsm item header. 

PARAMETERS
  item_ptr - address of dsm item

DEPENDENCIES
  None.

RETURN VALUE
  app_field 

SIDE EFFECTS
  None.
===========================================================================*/
#define HDRDSM_GET_USER_FIELD_APP_FIELD( item_ptr )\
   ( (item_ptr)->app_field )


 /* EJECT */
/*===========================================================================
MACRO HDRDSM_SET_USER_FIELD_APP_FIELD

DESCRIPTION
  This macro sets the appl. defined field in the specified dsm item header to
  the specified value.
    
PARAMETERS
  item_ptr - address of dsm item
  val      - value to insert for appl. defined field

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Link ID field is updated.
===========================================================================*/
#define HDRDSM_SET_USER_FIELD_APP_FIELD( item_ptr, val )\
   ( (item_ptr)->app_field = (val) )


 /* EJECT */
/*===========================================================================
MACRO HDRDSM_SET_LINK_ID

DESCRIPTION
  This macro sets the link ID field in the specified dsm item header to the
  specified value.
    
PARAMETERS
  item_ptr - address of dsm item
  val      - value to insert for link ID

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Link ID field is updated.
===========================================================================*/
#define HDRDSM_SET_LINK_ID( item_ptr, val )\
   ( (item_ptr)->app_field = \
   ( ( (item_ptr)->app_field & ~LINK_ID_MASK ) \
   | ( ( (val) << LINK_ID_LSB ) & LINK_ID_MASK ) ) )


 /* EJECT */
/*===========================================================================
MACRO HDRDSM_SET_USER_FIELD_LINK_ID

DESCRIPTION
  This macro sets the link ID field in the specified temporary user defined 
  field.
    
PARAMETERS
  app_field - temporary user defined field.
  val      -  value to insert for link ID

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
#define HDRDSM_SET_USER_FIELD_LINK_ID( app_field, val ) \
  ( (app_field) = ( ( (app_field) & ~LINK_ID_MASK ) | \
                    ( ( (val) << LINK_ID_LSB ) & LINK_ID_MASK ) ) )

 /* EJECT */
/*===========================================================================
MACRO HDRDSM_IS_CONN_FORMAT_A

DESCRIPTION
  Checks if the packet contained in the DSM item is of type connection layer 
  format A.
    
PARAMETERS
  item_ptr     - address of dsm item

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - if the packet's connection layer format is A
  FALSE - otherwise. 

SIDE EFFECTS
  None.
===========================================================================*/
#define HDRDSM_IS_CONN_FORMAT_A( item_ptr ) \
     ( ( (item_ptr)->app_field & CONN_FORMAT_MASK )? FALSE: TRUE )


 /* EJECT */
/*===========================================================================
MACRO HDRDSM_SET_CONN_FORMAT

DESCRIPTION
  Sets the connection layer format of the DSM item to either format A or 
  format B.

PARAMETERS
  item_ptr      - address of dsm item
  conn_format   - Connection layer format - one of the following 
                    1. HDRDSM_PACKET_IS_FMT_A
                    2. HDRDSM_PACKET_IS_FMT_B

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
#define HDRDSM_SET_CONN_FORMAT( item_ptr, conn_format )\
  ( (item_ptr)->app_field = \
    ( ( (item_ptr)->app_field & ~CONN_FORMAT_MASK ) | \
      ( ( (conn_format) << CONN_FORMAT_LSB ) & CONN_FORMAT_MASK ) ) )


 /* EJECT */
/*===========================================================================
MACRO HDRDSM_GET_MAC_TRAILER

DESCRIPTION
  Gets the MAC trailer from the specified dsm item. 
    
PARAMETERS
  item_ptr - address of the dsm item

DEPENDENCIES
  None.

RETURN VALUE
  MAC trailer (00) 0 - Multi-user packet
              (01) 1 - Single-user Simplex Format-A packet
              (10) 2 - Single-user Multiplex packet
              (11) 3 - Single-user Simplex Format-B packet

SIDE EFFECTS
  None.
===========================================================================*/
#define HDRDSM_GET_MAC_TRAILER( item_ptr ) \
  ( ( (item_ptr)->app_field & MAC_TRAILER_MASK ) >> MAC_TRAILER_LSB )


 /* EJECT */
/*===========================================================================
MACRO HDRDSM_SET_MAC_TRAILER

DESCRIPTION
  Sets the MAC trailer in the specified dsm item.
    
PARAMETERS
  item_ptr - address of the dsm item
  mac_trailer - MAC trailer to set.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
#define HDRDSM_SET_MAC_TRAILER( item_ptr, mac_trailer ) \
  ( (item_ptr)->app_field = ( ( (item_ptr)->app_field & ~MAC_TRAILER_MASK ) | \
                              ( ( (mac_trailer) << MAC_TRAILER_LSB ) & \
                                  MAC_TRAILER_MASK ) ) )


 /* EJECT */
/*===========================================================================
MACRO HDRDSM_SET_USER_FIELD_MAC_TRAILER

DESCRIPTION
  Sets the MAC trailer in temporary user field. 
    
PARAMETERS  
  app_field   - Temporary user defined field.
  mac_trailer - MAC trailer

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
#define HDRDSM_SET_USER_FIELD_MAC_TRAILER( app_field, mac_trailer ) \
  ( (app_field) = ( ( (app_field) & ~MAC_TRAILER_MASK ) | \
                    ( ( (mac_trailer) << MAC_TRAILER_LSB ) & \
                          MAC_TRAILER_MASK ) ) )


 /* EJECT */
#ifdef FEATURE_HDR_REVA_L1
/*===========================================================================
MACRO HDRDSM_IS_REV_A

DESCRIPTION
  Finds if the packet's physical layer version is Rev-A.
    
PARAMETERS
  item_ptr - address of the dsm item

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - if the packet's physical layer version is Rev-A. 
  FALSE - otherwise.

SIDE EFFECTS
  None.
===========================================================================*/
#define HDRDSM_IS_REV_A( item_ptr ) \
    ( ( (item_ptr)->app_field & PHY_LAYER_REV_MASK )? TRUE: FALSE )


 /* EJECT */
/*===========================================================================
MACRO HDRDSM_SET_REV_A

DESCRIPTION
  Sets the physical layer version to Revision-A in the specified dsm item.
    
PARAMETERS
  item_ptr - address of the dsm item

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
#define HDRDSM_SET_REV_A( item_ptr ) \
  ( (item_ptr)->app_field |= PHY_LAYER_REV_MASK )


 /* EJECT */
/*===========================================================================
MACRO HDRDSM_SET_USER_FIELD_REV_A

DESCRIPTION
  Sets the physical layer version to Revision-A in temporary user field. 
    
PARAMETERS
  item_ptr - address of the dsm item

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
#define HDRDSM_SET_USER_FIELD_REV_A( app_field ) \
  ( (app_field) |= PHY_LAYER_REV_MASK )


#endif /* FEATURE_HDR_REVA_L1 */


 /* EJECT */
/*===========================================================================
MACRO HDRDSM_IS_LAST_ITEM

DESCRIPTION
  Finds if the specified dsm item is the last item in the current packet's 
  dsm chain.
    
PARAMETERS
  item_ptr     - address of dsm item

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - If the specified dsm item is the last item.
  FALSE - otherwise. 

SIDE EFFECTS
  None.
===========================================================================*/
#define HDRDSM_IS_LAST_ITEM( item_ptr ) \
  ( ( (item_ptr)->app_field & LAST_ITEM_MASK )? TRUE: FALSE )

 
 /* EJECT */
/*===========================================================================
MACRO HDRDSM_CLR_LAST_ITEM

DESCRIPTION
  Clears the specified dsm item as the last item in the current packet's 
  dsm chain.

PARAMETERS
  item_ptr     - address of dsm item

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
#define HDRDSM_CLR_LAST_ITEM( item_ptr ) \
  ( (item_ptr)->app_field &= ~LAST_ITEM_MASK )

 /* EJECT */
/*===========================================================================
MACRO HDRDSM_CLR_USER_FIELD_LAST_ITEM

DESCRIPTION
  Clears the specified app_field's bit denoting last item in the current 
  packet's dsm chain.

PARAMETERS
  app_field

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
#define HDRDSM_CLR_USER_FIELD_LAST_ITEM( app_field )        \
          ((app_field) &= ~LAST_ITEM_MASK)

 /* EJECT */
/*===========================================================================
MACRO HDRDSM_SET_LAST_ITEM

DESCRIPTION
  Marks the specified dsm item as the last item in the current packet's dsm 
  chain.

PARAMETERS
  item_ptr - address of dsm item

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
#define HDRDSM_SET_LAST_ITEM( item_ptr ) \
  ( (item_ptr)->app_field |= LAST_ITEM_MASK )


 /* EJECT */
/*===========================================================================
MACRO HDRDSM_GET_SLOTS_TO_DECODE

DESCRIPTION
  Gets the number of slots taken to decode from the specified dsm item.
    
PARAMETERS
  item_ptr - pointer to dsm item

DEPENDENCIES
  None.

RETURN VALUE
  Slots to decode. 

SIDE EFFECTS
  None.
===========================================================================*/
#define HDRDSM_GET_SLOTS_TO_DECODE( item_ptr ) \
  ( ( (item_ptr)->app_field & SLOTS_DEC_MASK ) >> SLOTS_DEC_LSB )


 /* EJECT */
/*===========================================================================
MACRO HDRDSM_SET_SLOTS_TO_DECODE

DESCRIPTION
  Sets the number of slots taken to decode in the specified dsm item.
    
PARAMETERS
  item_ptr - pointer to dsm item
  slots_to_decode - Number of slots taken to decode this packet.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
#define HDRDSM_SET_SLOTS_TO_DECODE( item_ptr, slots_to_decode ) \
  ( (item_ptr)->app_field = ( ( (item_ptr)->app_field & ~SLOTS_DEC_MASK ) | \
                              ( ( (slots_to_decode) << SLOTS_DEC_LSB ) & \
                                    SLOTS_DEC_MASK ) ) )


 /* EJECT */
/*===========================================================================
MACRO HDRDSM_SET_USER_FIELD_SLOTS_TO_DECODE

DESCRIPTION
  Sets the number of slots taken to decode in the specified temporary 
  user field. 
    
PARAMETERS
  app_field       - Temporary user field. 
  slots_to_decode - Number of slots taken to decode this packet.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
#define HDRDSM_SET_USER_FIELD_SLOTS_TO_DECODE( app_field, slots_to_decode ) \
  ( (app_field) = ( ( (app_field) & ~SLOTS_DEC_MASK ) | \
                      ( ( (slots_to_decode) << SLOTS_DEC_LSB ) & \
                            SLOTS_DEC_MASK ) ) )

 /* EJECT */
/*===========================================================================
MACRO HDRDSM_SET_DRC

DESCRIPTION
  This macro sets the DRC field in the specified dsm item header to the
  specified value.
    
PARAMETERS
  item_ptr - address of dsm item
  val      - value to insert for DRC

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
#define HDRDSM_SET_DRC( item_ptr, val )\
   ( (item_ptr)->app_field = \
   ( ( (item_ptr)->app_field & ~DRC_MASK ) \
   | ( ( (val) << DRC_LSB ) & DRC_MASK ) ) )


 /* EJECT */
/*===========================================================================
MACRO HDRDSM_SET_USER_FIELD_DRC

DESCRIPTION
  This macro sets the DRC field in the specified temporary user defined field 
  to specified value. 
    
PARAMETERS
  app_field - Temporary user defined field. 
  val       - value to insert for DRC

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None. 
===========================================================================*/
#define HDRDSM_SET_USER_FIELD_DRC( app_field, val ) \
  ( (app_field) = ( ( (app_field) & ~DRC_MASK ) | \
                    ( ( (val) << DRC_LSB ) & DRC_MASK ) ) )

 /* EJECT */
/*===========================================================================
MACRO HDRDSM_GET_BCAST_BIT

DESCRIPTION
  This macro gets the current value of the broadcast bit from the specified 
  dsm item structure. 

PARAMETERS
  item_ptr - address of dsm item

DEPENDENCIES
  None.

RETURN VALUE
  Broadcast bit. ( 1 = message was addressed with a BATI, 
                   0 = message was addressed with a UATI )

SIDE EFFECTS
  None.
===========================================================================*/
#define HDRDSM_GET_BCAST_BIT( item_ptr )\
   ( ( (item_ptr)->app_field & BCAST_BIT_MASK ) >> BCAST_BIT_LSB )


 /* EJECT */
/*===========================================================================
MACRO HDRDSM_SET_BCAST_BIT

DESCRIPTION
  This macro sets the broadcast bit in the specified dsm item header to the
  specified value.  It should be set to 1 if the message was addressed with
  a broadcast ATI (BATI).
    
PARAMETERS
  item_ptr - address of dsm item structure

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Bcast bit is updated.
===========================================================================*/
#define HDRDSM_SET_BCAST_BIT( item_ptr )\
   ( (item_ptr)->app_field |= BCAST_BIT_MASK )


 /* EJECT */
/*===========================================================================
MACRO HDRDSM_GET_HUNT_BIT

DESCRIPTION
  This macro gets the current value of the hunt bit from the specified dsm 
  item structure. 
  
  Note that this bit is only valid for forward traffic channel packets.

PARAMETERS
  item_ptr - address of dsm item

DEPENDENCIES
  Only valid for forward traffic channel packets.

RETURN VALUE
  Hunt bit. ( 1 = hunt byte present, 0 = hunt byte not present )

SIDE EFFECTS
  None.
===========================================================================*/
#define HDRDSM_GET_HUNT_BIT( item_ptr )\
   ( ( (item_ptr)->app_field & TC_HUNT_BIT_MASK ) >> TC_HUNT_BIT_LSB )


 /* EJECT */
/*===========================================================================
MACRO HDRDSM_SET_HUNT_BIT

DESCRIPTION
  This macro sets the hunt bit in the specified dsm item header to the
  specified value.
  
  Note that this bit is only valid for forward traffic channel packets.
    
PARAMETERS
  item_ptr - address of dsm item structure

DEPENDENCIES
  Only valid for forward traffic channel packets.

RETURN VALUE
  None.

SIDE EFFECTS
  Hunt bit is updated.
===========================================================================*/
#define HDRDSM_SET_HUNT_BIT( item_ptr )\
   ( (item_ptr)->app_field |= TC_HUNT_BIT_MASK )


 /* EJECT */
/*===========================================================================
MACRO HDRDSM_GET_CC_SYNC_BIT

DESCRIPTION
  This macro gets the current value of the sync bit from the specified dsm 
  item structure. 
  
  Note that this bit is only valid for traffic channel packets.

PARAMETERS
  item_ptr - address of dsm item

DEPENDENCIES
  Only valid for control channel packets.

RETURN VALUE
  Sync bit. ( 1 = synchronous control channel packet,
              0 = asynchronous control channel packet )

SIDE EFFECTS
  None.
===========================================================================*/
#define HDRDSM_GET_CC_SYNC_BIT( item_ptr )\
   ( ( (item_ptr)->app_field & CC_SYNC_BIT_MASK ) >> CC_SYNC_BIT_LSB )


 /* EJECT */
/*===========================================================================
MACRO HDRDSM_SET_CC_SYNC_BIT

DESCRIPTION
  This macro sets the sync bit in the specified dsm item header to the
  specified value.
  
  Note that this bit is only valid for control channel packets.
    
PARAMETERS
  item_ptr - address of dsm item structure

DEPENDENCIES
  Only valid for control channel packets.

RETURN VALUE
  None.

SIDE EFFECTS
  Hunt bit is updated.
===========================================================================*/
#define HDRDSM_SET_CC_SYNC_BIT( item_ptr )\
   ( (item_ptr)->app_field |= CC_SYNC_BIT_MASK )

 /* EJECT */
/*===========================================================================
MACRO HDRDSM_SET_CHAN_BIT

DESCRIPTION
  This macro sets the channel bit in the specified dsm item header to 
  indicate that the corresponding packet is a Control Channel Packet.
    
PARAMETERS
  item_ptr - address of dsm item structure.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Channel bit is updated.
===========================================================================*/
#define HDRDSM_SET_CHAN_BIT( item_ptr )\
   ( (item_ptr)->app_field |= CHAN_BIT_MASK )


 /* EJECT */
/*===========================================================================
MACRO HDRDSM_SET_USER_FIELD_CHAN

DESCRIPTION
  This macro sets the channel bit in a temporary user defined field. 
    
PARAMETERS
  app_field - Temporary user field. 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None. 
===========================================================================*/
#define HDRDSM_SET_USER_FIELD_CHAN( app_field ) \
  ( (app_field) |= CHAN_BIT_MASK )


 /* EJECT */
/*===========================================================================
MACRO HDRDSM_GET_TIMESTAMP

DESCRIPTION
  This macro gets the current value of the timestamp from the specified dsm 
  item structure. 

PARAMETERS
  item_ptr - address of dsm item

DEPENDENCIES
  None.

RETURN VALUE
  Half slot timestamp.

SIDE EFFECTS
  None.
===========================================================================*/
#define HDRDSM_GET_TIMESTAMP( item_ptr ) \
   ( ( (item_ptr)->app_field & TIMESTAMP_MASK ) >> TIMESTAMP_LSB )


 /* EJECT */
/*===========================================================================
MACRO HDRDSM_SET_TIMESTAMP

DESCRIPTION
  This macro sets the timestamp in the specified dsm item header to the
  specified value.
    
PARAMETERS
  item_ptr - address of dsm item structure
  val      - value to insert for timestamp

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Timestamp is updated.
===========================================================================*/
#define HDRDSM_SET_TIMESTAMP( item_ptr, val )\
   ( (item_ptr)->app_field = \
   ( ( (item_ptr)->app_field & ~TIMESTAMP_MASK ) \
   | ( ( (val) << TIMESTAMP_LSB ) & TIMESTAMP_MASK ) ) )


 /* EJECT */
/*===========================================================================
MACRO HDRDSM_SET_USER_FIELD_TIMESTAMP

DESCRIPTION
  This macro sets the packet timestamp(HSTR) in the specified user defined 
  field.
    
PARAMETERS
  app_field - Temporary user defined uint32 field. 
  val       - value to insert for timestamp

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
#define HDRDSM_SET_USER_FIELD_TIMESTAMP( app_field, val ) \
  ( (app_field) = ( ( (app_field) & ~TIMESTAMP_MASK ) | \
                    ( ( (val) << TIMESTAMP_LSB ) & TIMESTAMP_MASK ) ) )
                  
 /* EJECT */
/*===========================================================================
MACRO HDRDSM_GET_USER_FIELD_KIND

DESCRIPTION
  This macro gets the current value of the appl. defined field (kind) from 
  the specified dsm item header. 

PARAMETERS
  item_ptr - address of dsm item

DEPENDENCIES
  None.

RETURN VALUE
  app_field 

SIDE EFFECTS
  None.
===========================================================================*/
#define HDRDSM_GET_USER_FIELD_KIND( item_ptr )\
   ( (item_ptr)->kind )


 /* EJECT */
/*===========================================================================
MACRO HDRDSM_SET_USER_FIELD_KIND

DESCRIPTION
  This macro sets the appl. defined field (kind) in the specified dsm item 
  header to the specified value.
    
PARAMETERS
  item_ptr - address of dsm item
  val      - value to insert for appl. defined field

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Link ID field is updated.
===========================================================================*/
#define HDRDSM_SET_USER_FIELD_KIND( item_ptr, val )\
   ( (item_ptr)->kind = (val) )

 /* EJECT */
/*===========================================================================
MACRO HDRDSM_GET_TRAFFIC_BIT

DESCRIPTION
  This macro gets the current value of the traffic bit from the specified dsm
  item structure.

PARAMETERS
  item_ptr - address of dsm item

DEPENDENCIES
  None.

RETURN VALUE
  Traffic bit. ( 1 = Packet received either on CC or FTC while in traffic,
                 0 = Packet received when idle )

SIDE EFFECTS
  None.
===========================================================================*/
#define HDRDSM_GET_TRAFFIC_BIT( item_ptr )\
   ( ( (item_ptr)->kind & TRAFFIC_BIT_MASK ) >> TRAFFIC_BIT_LSB )


 /* EJECT */
/*===========================================================================
MACRO HDRDSM_SET_TRAFFIC_BIT

DESCRIPTION
  This macro sets the traffic bit in the specified dsm item header to
  indicate that the corresponding packet was received while connected
  (either on CC or FTC).

PARAMETERS
  item_ptr - address of dsm item structure.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Traffic bit is updated.
===========================================================================*/
#define HDRDSM_SET_TRAFFIC_BIT( item_ptr )\
   ( (item_ptr)->kind |= TRAFFIC_BIT_MASK )


 /* EJECT */
/*===========================================================================
MACRO HDRDSM_GET_PACKET_SIZE

DESCRIPTION
  This macro gets the size of the physical layer packet from the app_ptr field. 
  The returned packet size is encoded as below. 
  0 - 128 bits
  1 - 256 bits
  2 - 512 bits
  3 - 1024 bits
  4 - 2048 bits
  5 - 3072 bits
  6 - 4096 bits
  7 - 5120 bits
  8  - 6144 bits
  9  - 7168 bits
  10 - 8192 bits

PARAMETERS
  item_ptr - address of dsm item structure.

DEPENDENCIES
  None.

RETURN VALUE
  pkt_size - A value from 0-7 indicating the packet size as per the above table.

SIDE EFFECTS
  None.
===========================================================================*/
#define HDRDSM_GET_PACKET_SIZE( item_ptr ) \
  ( ( (uint32)((item_ptr)->app_ptr) & PKT_SIZE_MASK ) >> PKT_SIZE_LSB )


 /* EJECT */
/*===========================================================================
MACRO HDRDSM_SET_PACKET_SIZE

DESCRIPTION
  This macro sets the size of the physical layer packet in the app_ptr field 
  using the following coding scheme. 
  0 - 128 bits
  1 - 256 bits
  2 - 512 bits
  3 - 1024 bits
  4 - 2048 bits
  5 - 3072 bits
  6 - 4096 bits
  7 - 5120 bits
  8  - 6144 bits
  9  - 7168 bits
  10 - 8192 bits

PARAMETERS
  item_ptr - address of dsm item structure.
  pkt_size - A value from 0-7 indicating the packet size as per the above table.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
#define HDRDSM_SET_PACKET_SIZE( item_ptr, pkt_size )\
  ( (item_ptr)->app_ptr = ( ( (item_ptr)->app_ptr & ~PKT_SIZE_MASK ) | \
                         ( ( (pkt_size) << PKT_SIZE_LSB ) & PKT_SIZE_MASK ) ) )


 /* EJECT */
/*===========================================================================
MACRO HDRDSM_SET_USER_FIELD_PACKET_SIZE

DESCRIPTION
  This macro sets the size of the physical layer packet in the last three
  bits of the user field using the following coding scheme. 
  0 - 128 bits
  1 - 256 bits
  2 - 512 bits
  3 - 1024 bits
  4 - 2048 bits
  5 - 3072 bits
  6 - 4096 bits
  7 - 5120 bits
  8  - 6144 bits
  9  - 7168 bits
  10 - 8192 bits

PARAMETERS
  app_ptr_field - User field representing the "app_ptr" field of the DSM item 
  pkt_size - A value from 0-7 indicating the packet size as per the above table.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
#define HDRDSM_SET_USER_FIELD_PACKET_SIZE( app_ptr_field, pkt_size )      \
  ((app_ptr_field) = (((app_ptr_field) & ~PKT_SIZE_MASK) |                \
             (((pkt_size) << PKT_SIZE_LSB) & PKT_SIZE_MASK)))


 /* EJECT */
/*===========================================================================
MACRO HDRDSM_GET_PACKET_TYPE

DESCRIPTION
  This macro gets the packet type field from bits 2-6 of kind field in the 
  given DSM item. 

PARAMETERS
  item_ptr - address of dsm item structure.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
#define HDRDSM_GET_PACKET_TYPE( item_ptr ) \
  ( ( (item_ptr)->kind & PKT_TYPE_MASK ) >> PKT_TYPE_LSB )


 /* EJECT */
/*===========================================================================
MACRO HDRDSM_SET_PACKET_TYPE

DESCRIPTION
  This macro sets the given packet type field in the bits 2-6 of the kind 
  field of given DSM item. 

PARAMETERS
  item_ptr - address of dsm item structure.
  pkt_type - Indicates the type of the packet contained in the DSM item. It
             should use values defined by hdrdec.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
#define HDRDSM_SET_PACKET_TYPE( item_ptr, pkt_type )\
  ( (item_ptr)->kind = ( ( (item_ptr)->kind & ~PKT_TYPE_MASK ) | \
                         ( ( (pkt_type) << PKT_TYPE_LSB ) & PKT_TYPE_MASK ) ) )


 /* EJECT */
/*===========================================================================
MACRO HDRDSM_SET_USER_FIELD_PACKET_TYPE

DESCRIPTION
  This macro sets the given packet type field in the bits 3-6 of the given 
  kind field 

PARAMETERS
  kind - User field representing the kind field of the DSM item.
  pkt_type - Indicates the type of the packet contained in the DSM item.
             It is intended to use values defined by hdrdec.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
#define HDRDSM_SET_USER_FIELD_PACKET_TYPE( kind, pkt_type )               \
  ((kind) = (((kind) & ~PKT_TYPE_MASK) |                                  \
             (((pkt_type) << PKT_TYPE_LSB) & PKT_TYPE_MASK)))


 /* EJECT */
/*===========================================================================
MACRO HDRDSM_SET_PROT_ID

DESCRIPTION
  This macro sets the protocol ID field in the specified dsm item header to 
  the specified value.
    
PARAMETERS
  item_ptr - address of dsm item structure.
  val      - value to insert for protocol ID

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Link ID field is updated.
===========================================================================*/
#define HDRDSM_SET_PROT_ID( item_ptr, val )\
   ( (item_ptr)->kind = \
   ( ( (item_ptr)->kind & ~PROT_ID_MASK ) \
   | ( ( (val) << PROT_ID_LSB ) & PROT_ID_MASK ) ) )


 /* EJECT */
/*===========================================================================
MACRO HDRDSM_CLEAR_APP_PTR

DESCRIPTION
  This macro clears the app_ptr to NULL.
    
PARAMETERS
  item_ptr - address of dsm item

DEPENDENCIES
  None.

RETURN VALUE
  Carrier ID

SIDE EFFECTS
  None
===========================================================================*/
#define HDRDSM_CLEAR_APP_PTR( item_ptr ) ( (item_ptr)->app_ptr = 0 )


 /* EJECT */
/*===========================================================================
MACRO HDRDSM_GET_USER_FIELD_APP_PTR

DESCRIPTION
  This macro gets the current value of the app_ptr field from the 
  specified dsm item header. 
    
PARAMETERS
  item_ptr - address of dsm item

DEPENDENCIES
  None.

RETURN VALUE
  Carrier ID

SIDE EFFECTS
  None
===========================================================================*/
#define HDRDSM_GET_USER_FIELD_APP_PTR(item_ptr)                     \
          ( (uint32) ((item_ptr)->app_ptr) )

 /* EJECT */
/*===========================================================================
MACRO HDRDSM_SET_USER_FIELD_APP_PTR

DESCRIPTION
  This macro sets the current value of the app_ptr field from the 
  specified dsm item header to the given value.
    
PARAMETERS
  item_ptr - address of dsm item

DEPENDENCIES
  None.

RETURN VALUE
  Carrier ID

SIDE EFFECTS
  None
===========================================================================*/
#define HDRDSM_SET_USER_FIELD_APP_PTR(item_ptr, app_ptr_field)           \
          ((item_ptr)->app_ptr = (uint32*) app_ptr_field)




 /* EJECT */
/*===========================================================================
MACRO HDRDSM_SET_CARRIER_ID

DESCRIPTION
  This macro sets the Carrier ID field in the specified DSM item header to 
  the specified value.
    
PARAMETERS
  item_ptr - address of dsm item structure.
  val      - value to insert for carrier ID

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
#define HDRDSM_SET_CARRIER_ID( item_ptr, val )    \
          ((item_ptr)->app_ptr = \
           (void*) (((uint32) ((item_ptr)->app_ptr) & ~CARRIER_ID_MASK) | \
                    (((val) << CARRIER_ID_LSB) & CARRIER_ID_MASK)))

 /* EJECT */
/*===========================================================================
MACRO HDRDSM_SET_USER_FIELD_CARRIER_ID

DESCRIPTION
  This macro sets the Carrier ID field in the specified user field to 
  the specified value.
    
PARAMETERS
  user_field - User field.
  val      - value to insert for carrier ID

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
#define HDRDSM_SET_USER_FIELD_CARRIER_ID( user_field, val )         \
          ((user_field) = (((user_field) & ~CARRIER_ID_MASK) |      \
           (((val) << CARRIER_ID_LSB) & CARRIER_ID_MASK)))


 /* EJECT */
/*===========================================================================
MACRO HDRDSM_SET_SCHED_GROUP_ID

DESCRIPTION
  This macro sets the Scheduler Group ID field in the specified DSM item 
  header to the specified value.
    
PARAMETERS
  item_ptr - address of dsm item structure.
  val      - value to insert for Scheduler Group ID

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
#define HDRDSM_SET_SCHED_GROUP_ID( item_ptr, val )    \
          ((item_ptr)->app_ptr = \
           (void*) (((uint32) ((item_ptr)->app_ptr) & ~SCHED_GROUP_ID_MASK) | \
                    (((val) << SCHED_GROUP_ID_LSB) & SCHED_GROUP_ID_MASK)))

 /* EJECT */
/*===========================================================================
MACRO HDRDSM_SET_USER_FIELD_SCHED_GROUP_ID

DESCRIPTION
  This macro sets the Scheduler Group ID field in the specified user field 
  to the specified value.
    
PARAMETERS
  user_field - User field.
  val      - value to insert for SchedulerGroupID

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
#define HDRDSM_SET_USER_FIELD_SCHED_GROUP_ID( user_field, val )     \
          ((user_field) = (((user_field) & ~SCHED_GROUP_ID_MASK) |      \
           (((val) << SCHED_GROUP_ID_LSB) & SCHED_GROUP_ID_MASK)))

 /* EJECT */
/*===========================================================================
MACRO HDRDSM_GET_NUM_SIMUL_PACKETS

DESCRIPTION
  This macro gets the number of packets detected on the same slot.

PARAMETERS
  item_ptr - address of dsm item structure.

DEPENDENCIES
  None.

RETURN VALUE
  Returns the number of simultaneously detected packets.

SIDE EFFECTS
  None
===========================================================================*/
#define HDRDSM_GET_NUM_SIMUL_PACKETS(item_ptr)      \
          (((uint32)((item_ptr)->app_ptr) & SIMUL_PKT_MASK) >> SIMUL_PKT_LSB)

 /* EJECT */
/*===========================================================================
MACRO HDRDSM_SET_NUM_SIMUL_PACKETS 

DESCRIPTION
  This macro sets the number of packets detected on the same slot.

PARAMETERS
  item_ptr - address of dsm item structure.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
#define HDRDSM_SET_NUM_SIMUL_PACKETS(item_ptr, val)                       \
          ((item_ptr)->app_ptr =                                          \
            (void*) (((uint32) ((item_ptr)->app_ptr) & ~SIMUL_PKT_MASK) | \
                     (((val) << SIMUL_PKT_LSB) & SIMUL_PKT_MASK)))

 /* EJECT */
/*===========================================================================
MACRO HDRDSM_SET_USER_FIELD_NUM_SIMUL_PACKETS

DESCRIPTION
  This macro sets the Scheduler Group ID field in the specified user field 
  to the specified value.
    
PARAMETERS
  user_field - User field.
  val      - value to insert for SchedulerGroupID

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
#define HDRDSM_SET_USER_FIELD_NUM_SIMUL_PACKETS( user_field, val )      \
          ((user_field) = (((user_field) & ~SIMUL_PKT_MASK) |           \
           (((val) << SIMUL_PKT_LSB) & SIMUL_PKT_MASK)))

 /* EJECT */
/*===========================================================================
MACRO HDRDSM_MARK_LAST_OF_SIMUL_PACKETS

DESCRIPTION
  This macro marks this DSM item as the last of all simultaneously detected
  packets. 

PARAMETERS
  item_ptr - address of dsm item structure.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
#define HDRDSM_MARK_LAST_OF_SIMUL_PACKETS( item_ptr )    \
          ((item_ptr)->app_ptr = \
           (void*) ((uint32) ((item_ptr)->app_ptr) | END_BIT_MASK))

 /* EJECT */
/*===========================================================================
MACRO HDRDSM_MARK_USER_FIELD_LAST_OF_SIMUL_PACKETS

DESCRIPTION
  This macro marks the user field as the last of all simultaneously detected
  packets. 

PARAMETERS
  item_ptr - address of dsm item structure.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
#define HDRDSM_MARK_USER_FIELD_LAST_OF_SIMUL_PACKETS( user_field )    \
          ((user_field) = ((user_field) | END_BIT_MASK))

 /* EJECT */
/*===========================================================================
MACRO HDRDSM_CLR_LAST_OF_SIMUL_PACKETS

DESCRIPTION
  This macro marks this DSM item as the last of all simultaneously detected
  packets. 

PARAMETERS
  item_ptr - address of dsm item structure.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
#define HDRDSM_CLR_LAST_OF_SIMUL_PACKETS( item_ptr )    \
          ((item_ptr)->app_ptr = \
           (void*) ((uint32) ((item_ptr)->app_ptr) & ~END_BIT_MASK))

 /* EJECT */
/*===========================================================================
MACRO HDRDSM_CLR_USER_FIELD_LAST_OF_SIMUL_PACKETS

DESCRIPTION
  This macro marks this user_field as the last of all simultaneously detected
  packets. 

PARAMETERS
  item_ptr - address of dsm item structure.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
#define HDRDSM_CLR_USER_FIELD_LAST_OF_SIMUL_PACKETS( user_field )    \
          ((user_field) = ((user_field) & ~END_BIT_MASK))

 /* EJECT */
/*===========================================================================
MACRO HDRDSM_IS_LAST_OF_SIMUL_PACKETS

DESCRIPTION
  This macro checks if a DSM item is the last of all simultaneously detected
  packets.
    
PARAMETERS
  item_ptr - address of dsm item structure.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - If the DSM item is the last of all simultaneously detected preambles.
  FALSE - otherwise.

SIDE EFFECTS
  None
===========================================================================*/
#define HDRDSM_IS_LAST_OF_SIMUL_PACKETS( item_ptr )    \
           (((uint32)((item_ptr)->app_ptr) & END_BIT_MASK)? TRUE: FALSE)

/*===========================================================================
MACRO HDRDSM_COPY_APP_PTR_TO_DUPD_ITEM

DESCRIPTION
  The dsm_dup_packet function doesn't copy the app_ptr field. AMSS uses
  app_ptr field to send some information about the decoded packet to 
  the upper layers. This macro copies this field from original DSM
  item to DUPd DSM item.
    
PARAMETERS
  item_ptr - address of the original DSM item
  dup_ptr - address of the DUPd DSM item

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
#define HDRDSM_COPY_APP_PTR_TO_DUPD_ITEM( item_ptr, dup_ptr ) \
           ((dup_ptr)->app_ptr = (item_ptr)->app_ptr)

 /* EJECT */
/*===========================================================================
MACRO HDRDSM_SET_TRACER, HDRDSM_TRACE_NEW, HDRDSM_TRACE_DUP

DESCRIPTION
  These macros allow setting the tracer field in the dsm item header
  for existing, newly allocated, or dup'ed items.
    
  HDRDSM_SET_TRACER - intended to set tracer value on an existing 
                      item, does sanity check for NULL
  HDRDSM_TRACE_NEW  - intended to set tracer after allocating a
                      new dsm item, assumes valid pointer
  HDRDSM_TRACE_DUP  - intended to set tracer after duping an
                      existing item, assumes valid pointer for
                      duped item

PARAMETERS
  item - address of dsm item structure
  tid  - tracer value

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
#ifdef FEATURE_DSM_MEM_CHK
  #ifdef HDRDSM_TRACE_DBG
    #define HDRDSM_TRACE_NEW( item, tid )                         \
      DSM_TOUCH_ITEM( item )                                      \
      HDR_MSG_PROT_2 ( MSG_LEGACY_LOW,                            \
                      "Allcd dsm 0x%08x (%d)", item, tid);

    #define HDRDSM_TRACE_DUP( item, tid )                         \
      DSM_TOUCH_ITEM( item )                                      \
      HDR_MSG_PROT_2 (MSG_LEGACY_LOW,                             \
                      "Duped dsm 0x%08x (%d)", item, tid);
  #else
    #define HDRDSM_TRACE_NEW( item, tid )                         \
      HDRDSM_SET_TRACER( item, tid )

    #define HDRDSM_TRACE_DUP( item, tid )                         \
      HDRDSM_SET_TRACER( item, tid )
  #endif
#else
  #define HDRDSM_TRACE_NEW( item, tid )
  #define HDRDSM_TRACE_DUP( item, tid )
#endif




 /* EJECT */
/*===========================================================================
MACRO  HDRDSM_GET_FREE_POOL_COUNT()

DESCRIPTION

  Returns the number of items in the free pool that HDR is using.        
      
PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  Number of available dsm items.

SIDE EFFECTS
  None
===========================================================================*/
#define HDRDSM_GET_FREE_COUNT()                                             \
  DSM_POOL_FREE_CNT( DSM_DS_SMALL_ITEM_POOL )

    
/*===========================================================================
MACRO  HDRDSM_TOUCH_ITEM()

DESCRIPTION

  Calls DSM_TOUCH_ITEM to update the current location of DSM item.
      
PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
#define HDRDSM_TOUCH_ITEM(x) \
  if((x)!= NULL){ DSM_TOUCH_ITEM(x); }else (void)0
  
#endif /* HDRDSM_H */

