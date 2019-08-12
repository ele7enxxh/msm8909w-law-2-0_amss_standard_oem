#ifndef HDRAMPATI_H
#define HDRAMPATI_H

/*===========================================================================

            A D D R E S S   M A N A G E M E N T   P R O T O C O L
        A C C E S S   T E R M I N A L   I D E N T I F I E R   M O D U L E
                           D E F I N I T I O N S

DESCRIPTION
  This contains the declarations for the Address Management Protocol 
  Access Terminal Identifier (AMPATI) module.

Copyright (c) 2002 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$PVCSPath: O:/src/asw/COMMON/vcs/hdrampati.h_v   1.1   05 Aug 2002 09:53:20   maldinge  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/inc/hdrampati.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/01/09   suren   Added QChat optimization changes.
07/07/05   dna     Added support for Enhanced Idle State protocol
08/05/02   mpa     Added hdrampati_set_last_acmac_ati().
03/29/02   mpa     Initial revision

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "hdrhai.h"
#include "comdef.h"

/* <EJECT> */ 
/*===========================================================================

             DEFINITIONS  FROM  THE  HDR  AIR  INTERFACE  SPEC

===========================================================================*/

/* <EJECT> */ 
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/
/* Number of Receive ATI elements */
#define HDRAMPATI_MAX_NUM_REC_UATI_ELTS 4

/* Receive UATI structure */
typedef struct
{
  /*------------------------------------------------------------------------
  Unicast ATI
  ------------------------------------------------------------------------*/
  uint32 uati;       /* 32-bit address used to validate received msgs */

  uint32 time_valid; /* The amount of time that UATI "n-1" is valid for,
                        relative to when the n-address timer was last started
                        This field will always be zero for the first UATI 
                        in the list */

} hdrampati_receive_uati_record_type;

  /*------------------------------------------------------------------------
  Here is an example to illustrate how the time_valid field works:
  
  Assume for this example that address matching duration (amd) is 3 minutes

  * no timer - Assignment UATI A, transition from SETUP to OPEN state
  
  * @Time 0' - Assignment UATI B, add B to our list and start the n-address 
                                timer (time_valid(A) = amd = 3')
                                
                             0            1      
  +--------------+    +------------+------------+
  | N_UATI_TIMER |    |   UATI A   |   UATI B   |
  +--------------+    +------------+------------+
  | 3 min. left  |    | TIME_VALID | TIME_VALID |
  +--------------+    |   0 min    |   3 min    |
                      +------------+------------+
                      
  * @Time 1' - Assignment UATI C, add C to our list, n-address timer is still
                                running, so save time_valid for B
                  time_valid(B) = amd + ( time_valid(A) - time_left_on_timer)
                                = 3' + (3' - 2') = 4'
                                
                             0            1            2
  +--------------+    +------------+------------+------------+
  | N_UATI_TIMER |    |   UATI A   |   UATI B   |   UATI C   |
  +--------------+    +------------+------------+------------+
  | 2 min. left  |    | TIME_VALID | TIME_VALID | TIME_VALID |
  +--------------+    |   0 min    |   3 min    |   4 min    |
                      +------------+------------+------------+

                      
  * @Time 3' - Timer expires, -> delete A (invalid)
                              -> update time_valid(A) & time_valid(B)
                         time_valid(A) = 0' (time_valid(A) - time_valid(A))
                         time_valid(B) = 1' (time_valid(B) - time_valid(A))
                              -> restart the n-address timer for 1'
                              -> only B & C are valid
                              
                             0            1      
  +--------------+    +------------+------------+
  | N_UATI_TIMER |    |   UATI B   |   UATI C   |
  +--------------+    +------------+------------+
  | 1 min. left  |    | TIME_VALID | TIME_VALID |
  +--------------+    |   0 min    |   1 min    |
                      +------------+------------+
                      
  * @Time 4' - Timer expires, -> delete B (invalid)
                                 (invalidated 3' after UATI C was assigned)                           
                              -> only C is valid
  
                             0      
  +--------------+    +------------+
  | N_UATI_TIMER |    |   UATI C   |
  +--------------+    +------------+
  | Not Running  |    | TIME_VALID |
  +--------------+    |   0 min    |
                      +------------+
  ------------------------------------------------------------------------*/
/* Max Number of MATI elements that can be stored*/
#define HDRAMPATI_MAX_NUM_REC_MATI_ELTS 25

/* Receive MATI structure */
typedef struct
{
  /*------------------------------------------------------------------------
    Multicast ATI
  ------------------------------------------------------------------------*/
  uint32 mati;       

  uint32 time_valid; /* The amount of time that MATI "n-1" is valid for,
                        relative to when the n-address timer was last started
                        This field will always be zero for the first MATI 
                        in the list */

} hdrampati_receive_mati_record_type;
    
typedef struct 
{
  /*------------------------------------------------------------------------
  Unicast ATI(s) - Multiple UATIs may be valid at the same time.
  When a UATI is first assigned (in SETUP state), it is considered the
  "current" UATI.  All subsequent UATIs during the same session are
  first considered as "new" UATIs.  For the period of time when the 
  n-address timer is running, the address matching will accept a message
  addressed to either one of the UATIs.  When the n-address timer expires, 
  the oldest UATI becomes invalid.  If more than one UATI are still valid
  at this point, then the n-address timer needs to be restarted.
  => See example above
  ------------------------------------------------------------------------*/
  uint8 receive_uati_count; /* current number of valid uatis in the list */
  hdrampati_receive_uati_record_type 
        receive_uati[HDRAMPATI_MAX_NUM_REC_UATI_ELTS];

  uint8 receive_mati_count; /* current number of valid matis in the list */
  hdrampati_receive_mati_record_type 
        receive_mati[HDRAMPATI_MAX_NUM_REC_MATI_ELTS];

  /*------------------------------------------------------------------------
  Broadcast ATI - Always valid, there is no need to keep track of it
  ------------------------------------------------------------------------*/

  /*------------------------------------------------------------------------
  Random ATI - Valid when no UATI has been received yet (setup state)
  ------------------------------------------------------------------------*/
  boolean receive_rati_is_valid;
  uint32  receive_rati;

  /*------------------------------------------------------------------------
  Last ACMAC ATI - last ATI given to ACMAC as valid transmit ATI.
                   (matched if in access attempt)
  ------------------------------------------------------------------------*/
  boolean last_acmac_ati_is_valid;
  uint32  last_acmac_ati;

  /*------------------------------------------------------------------------
  Timer for monitoring how long to honor multiple UATI addresses in the 
  receive ATI list.  The n-address matching concept involves the use of
  multiple receive UATIs at once, the current UATI and a set of new UATIs.
  ------------------------------------------------------------------------*/
  rex_timer_type n_addr_timer;

} hdrampati_struct;

/*===========================================================================

FUNCTION HDRAMPATI_RECEIVE_ATI_INSERT

DESCRIPTION
  Inserts an ATI in the ATI structure.

DEPENDENCIES
  None

PARAMETERS
  ati_item - receive ati record to be added to the list

RETURN VALUE
  None

SIDE EFFECTS
  May expire the n_addr_timer if the list is full.

===========================================================================*/
extern void hdrampati_receive_ati_insert
( 
  hdrhai_ati_record_type ati_item 
);

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRAMPATI_RECEIVE_ATI_GET

DESCRIPTION
  Returns the latest ATI of type specified if one exists in the receive 
  structure

DEPENDENCIES
  None

PARAMETERS
  ati_type - type of ATI to be fetched from the receive ATI structure

RETURN VALUE
  ati - returns an ATI value of type ati_type, if found

SIDE EFFECTS
  None

===========================================================================*/
extern uint32 hdrampati_receive_ati_get
( 
  hdrhai_ati_enum_type ati_type
);

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRAMPATI_RECEIVE_ATI_DELETE

DESCRIPTION
  Deletes an ATI of specified type from the receive ATI structure
  
DEPENDENCIES
  None

PARAMETERS
  ati_type - receive ATI type to be deleted
  
RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/
extern void hdrampati_receive_ati_delete 
(
  hdrhai_ati_enum_type ati_type
);

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRAMPATI_RECEIVE_ATI_CLEAR

DESCRIPTION
  ATI list helper function: clears the receive ati structure.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrampati_receive_ati_clear( void );

/* <EJECT> */ 

/*===========================================================================

FUNCTION HDRAMPATI_RECEIVE_RATI_IS_VALID

DESCRIPTION
  This function indicates whether or not the RATI is currently valid.
  (i.e. if no UATI has been assigned yet)

DEPENDENCIES
  None

PARAMETERS
  None
  
RETURN VALUE
  TRUE  - RATI is valid
  FALSE - RATI is invalid

SIDE EFFECTS
  None

===========================================================================*/
extern boolean hdrampati_receive_rati_is_valid ( void );

/* <EJECT> */ 
 
/*===========================================================================

FUNCTION HDRAMPATI_SET_LAST_ACMAC_ATI

DESCRIPTION
  This function indicates whether or not the last ACMAC ATI is currently valid.

DEPENDENCIES
  None

PARAMETERS
  last_acmac_ati_value - last ATI given to ACMAC as transmit ATI
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrampati_set_last_acmac_ati 
( 
  uint32 last_acmac_ati_value
);
/*--------------------------------------------------------------------------

                      HDRAMPATI Main Functions

--------------------------------------------------------------------------*/
/*===========================================================================

FUNCTION HDRAMPATI_MATCH_RECEIVE_ATI_ADDRESS

DESCRIPTION
  This function provides address matching on the received ATI.

DEPENDENCIES
  None

PARAMETERS
  receive_ati_ptr - received ATI
  sender          - protocol requesting address matching
  
RETURN VALUE
  TRUE  - ATI matches, address is valid
  FALSE - ATI does not match, address is not valid

SIDE EFFECTS
  None

===========================================================================*/
extern boolean hdrampati_match_receive_ati_address
(
  hdrhai_ati_record_type *receive_ati_ptr,
  hdrhai_protocol_name_enum_type sender
);

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRAMPATI_POWERUP_INIT

DESCRIPTION
  This function commands AMPATI to initialize.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrampati_powerup_init ( void );

#endif               /* HDRAMPATI_H */
