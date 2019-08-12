/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

            A D D R E S S   M A N A G E M E N T   P R O T O C O L
        A C C E S S   T E R M I N A L   I D E N T I F I E R   M O D U L E 

GENERAL DESCRIPTION
  The Address Management Protocol Access Terminal Identifier (AMPATI) module
  maintains a structure of all possible receive addresses valid at any time.

EXTERNALIZED FUNCTIONS (Global)

hdrampati_receive_ati_insert - Inserts an ATI in the receive ATI structure
hdrampati_receive_ati_get()  - Retrieves an ATI from the rec ATI structure
hdrampati_receive_ati_delete() - Deletes an ATI from the rec ATI structure
hdrampati_receive_ati_clear()  - Resets the receive ATI structure
hdrampati_receive_rati_is_valid()     - Indicates whether the rati is valid
hdrampati_set_last_acmac_ati()        - last ATI given to ACMAC as xmit ATI 
hdrampati_match_receive_ati_address() - Provides address matching on the 
                                        received ATI
hdrampati_powerup_init()              - Powerup initialization command

EXTERNALIZED FUNCTIONS (Regional)

  
INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2002-2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */ 
/*===========================================================================

                        EDIT HISTORY FOR MODULE

$PVCSPath: O:/src/asw/COMMON/vcs/hdrampati.c_v   1.3   06 Mar 2003 09:14:30   maldinge  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/src/hdrampati.c#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/05/14   cnx     Do not redefine timer if it is NV refresh.
06/10/10   pba     IRAT Measurement feature.
12/01/09   suren   Added QChat optimization changes.
11/20/08   kss     Removed/replaced intlocks.
10/01/08   wsh     Fixed critical/high lint warnings
12/10/07   wsh     DCM feature changes
07/07/05   dna     Added support for Enhanced Idle State protocol
04/04/05   pba     Removed Lint errors
03/24/05   pba     Lint cleanup
11/17/04   mpa     Check for empty list before deleting a UATI.
03/05/03   mpa     REX timer callbacks centralized in hdramp.c
09/25/02   mpa     Included rcv_ati_list logging
08/05/02   mpa     Remember last ATI given to ACMAC as transmit ATI. Match it
                   if we are in access attempt.
03/29/02   mpa     Initial revision

===========================================================================*/

/* <EJECT> */ 
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "hdr_variation.h"
#include "err.h"
#include "comdef.h"
#include "nv.h"
#include "hdrhai.h"
#include "hdramac.h"
#include "hdrampati.h"
#include "hdramps.h"
#include "hdrtrace.h"
#include "hdrdebug.h"
#include "hdrplog.h"
#include "hdrmc_v.h"
#if defined (FEATURE_DEDICATED_C2K_MODE) || defined (FEATURE_IRAT_PILOT_MEAS)
#include "hdrsmp.h"
#endif

#ifdef FEATURE_HDR_QCHAT
#include "hdrqchatupk.h"
#include "hdrqchatupk_opts.h"
#endif /*FEATURE_HDR_QCHAT*/

/* <EJECT> */ 
/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/* N-Address Matching time */
#define HDRAMPATI_ADDR_MATCH_LENGTH_MSEC (180UL * 1000UL)

/* <EJECT> */ 
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/
hdrampati_struct hdrampati;  

/* <EJECT> */ 
/*--------------------------------------------------------------------------
                              
                       UATI logging helper function
                              
--------------------------------------------------------------------------*/
LOCAL uint32* hdrampati_log_uati_list(void)
{
  uint8 index = 0;
    /* FOR loop index */

  uint8 *uati_count = &hdrampati.receive_uati_count;
    /* convenience local variable */ 

  static uint32 rcv_uati_list[HDRAMPATI_MAX_NUM_REC_UATI_ELTS];
    /* receive uati list values to be returned */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  for (index = 0; index < *uati_count; index++)
  {
    rcv_uati_list[index] = hdrampati.receive_uati[index].uati;
  }

  return rcv_uati_list;
} /* hdrampati_log_uati_list() */

/* <EJECT> */
/*--------------------------------------------------------------------------
                              
                       UATI list helper functions
                              
--------------------------------------------------------------------------*/
/*===========================================================================

FUNCTION HDRAMPATI_UATI_DELETE

DESCRIPTION
  Deletes the oldest UATI and does list maintenance
  
DEPENDENCIES
  None

PARAMETERS
  None 
  
RETURN VALUE
  None
  
SIDE EFFECTS
  May restart the n-address timer.

===========================================================================*/
LOCAL void hdrampati_uati_delete ( void )
{
  uint8 index;
    /* FOR loop index */

  rex_timer_cnt_type update_timer = 0;    
    /* new interval (in msec) for the n-address timer */

  uint8 *uati_count = &hdrampati.receive_uati_count;
  hdrampati_receive_uati_record_type *uati_list_ptr = hdrampati.receive_uati;
    /* convenience local variables */ 
                                                 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (*uati_count == 0)
  {
     ERR("UATI list is already empty",0,0,0);
     return;
  }

  /* update the uati list */
  (*uati_count)--; /* update the list size */

  /* if we have more than one uati, get the next saved time until expiration */
  if ( *uati_count >= 1 )
  {
    update_timer = uati_list_ptr[1].time_valid;
  }

  REX_DISABLE_PREMPTION(); /* Block interrupts during this update */

  /* Move every record up by one position to preserve the chronological order. 
     This has for effect to delete the first element in the list (i.e.
     oldest uati) */ 
  for (index = 0; index < *uati_count; index++)
  {
    uati_list_ptr[index].uati = uati_list_ptr[index + 1].uati;

    /* update the duration for which the corresponding uati will be valid for
    (after the current timer expires) */
    uati_list_ptr[index].time_valid =
      uati_list_ptr[index + 1].time_valid - update_timer;
  } /* end for */

  /* Reset the last element */
  uati_list_ptr[*uati_count].uati       = HDRHAI_ATI_NO_VALUE;
  uati_list_ptr[*uati_count].time_valid = 0;

  REX_ENABLE_PREMPTION();  /* Unblock interrupts */

  /* We also get the time left on the address timer. If it is still running, then this 
  means that our uati list is full and we just made room for the new incoming 
  item. Our oldest uati became invalid prematurely. Note that this does not 
  affect the remaining uatis */
  update_timer =   uati_list_ptr[1].time_valid 
                 + rex_get_timer(&hdrampati.n_addr_timer);

  /* Start (or re-start) the n-address timer */ 
  if (update_timer != 0)
  {
    (void)rex_set_timer(&hdrampati.n_addr_timer, update_timer);
  }

  /* log rcv ATI list */
  hdrplog_log_rcv_ati_list(hdrampati_log_uati_list(),hdrampati.receive_uati_count);

} /* hdrampati_uati_delete() */
  
/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRAMPATI_UATI_INSERT

DESCRIPTION
  Inserts a new UATI in the received uati list
  
DEPENDENCIES
  None

PARAMETERS
  ati_value - UATI to be added to the uati list 
  
RETURN VALUE
  None
  
SIDE EFFECTS
  May restart the n-address timer.

===========================================================================*/
LOCAL void hdrampati_uati_insert 
( 
  uint32 ati_value
)
{
  uint32 previous_uati_duration = 0; 
    /* */

  uint8 *uati_count = &hdrampati.receive_uati_count;
  hdrampati_receive_uati_record_type *uati_list_ptr = hdrampati.receive_uati;
    /* convenience local variables */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the uati list is full, delete the oldest UATI and reset 
     the address timer */
  if ( *uati_count == HDRAMPATI_MAX_NUM_REC_UATI_ELTS )
  {    
    hdrampati_uati_delete();
  }

  /* Compute the uati duration (how long our previous uati record
     is valid for) */
  if ( *uati_count != 0 )
  {
    previous_uati_duration = HDRAMPATI_ADDR_MATCH_LENGTH_MSEC
                             + ( uati_list_ptr[1].time_valid
                             - rex_get_timer(&hdrampati.n_addr_timer) );               
  }

  /* update the last record */                   
  REX_DISABLE_PREMPTION(); /* Block interrupts during this update */

  uati_list_ptr[*uati_count].uati       = ati_value;
  uati_list_ptr[*uati_count].time_valid = previous_uati_duration;

  REX_ENABLE_PREMPTION(); /* unblock interrupts */

  /* increment the list size */            
  (*uati_count)++;

  /* Start our n-address timer if we are adding a second concurrent uati 
     Make sure that we are in open state */
  if ( *uati_count == 2 )
  {
    if (hdramps_get_state() == HDRAMP_OPEN )
    {
      (void) rex_set_timer(&hdrampati.n_addr_timer, previous_uati_duration);
    }
  }

  /* log rcv ATI list */
  hdrplog_log_rcv_ati_list(hdrampati_log_uati_list(), hdrampati.receive_uati_count);

} /* hdrampati_uati_insert() */

/*--------------------------------------------------------------------------

                     Receive UATI Structure Utilities
                     
--------------------------------------------------------------------------*/
/*===========================================================================

FUNCTION HDRAMPATI_RECEIVE_ATI_INSERT

DESCRIPTION
  Inserts an ATI in the ATI structure.

DEPENDENCIES
  None

PARAMETERS
  ati_item - receive ati record to be added to the structure

RETURN VALUE
  None

SIDE EFFECTS
  May expire the n_addr_timer if the list is full.

===========================================================================*/
void hdrampati_receive_ati_insert
( 
  hdrhai_ati_record_type ati_item 
)
{
  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  switch ( ati_item.ati_type )
  {
    case HDRHAI_RATI:
      /*--------------------------------------------------------------------
        Insert a Random ATI
      --------------------------------------------------------------------*/
      if (ati_item.ati_value != HDRHAI_ATI_NO_VALUE)
      {
        hdrampati.receive_rati           = ati_item.ati_value;
        
        /* if we restore our session data from nv (in open state), we would
           like to retrieve the latest rati used, but make sure that it is
           not tagged as a valid RATI */
        if (hdramps_get_state() != HDRAMP_OPEN)
        {
          hdrampati.receive_rati_is_valid = TRUE;
        }
      }
      break;

    case HDRHAI_UATI:
      /*--------------------------------------------------------------------
        Insert a Unicast ATI
      --------------------------------------------------------------------*/
      hdrampati_uati_insert( ati_item.ati_value );
      break;

    case HDRHAI_BATI:
    case HDRHAI_MATI:
    case HDRHAI_ATI_INACTIVE:

      break;

    default:

      ERR("Attempting to insert an invalid ATI",0,0,0);
      break;

  } /* switch ati_item.ati_type */

} /* hdrampati_receive_ati_insert */

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
uint32 hdrampati_receive_ati_get
( 
  hdrhai_ati_enum_type ati_type
)
{
  uint32 ati; 
    /* returned element if found */

  const uint8 *uati_count = &hdrampati.receive_uati_count;
  const hdrampati_receive_uati_record_type *uati_list_ptr 
                                                    = hdrampati.receive_uati;
    /* convenience local variables */   

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the return ati */
  ati = HDRHAI_ATI_NO_VALUE;
  
  switch ( ati_type )
  {
    case HDRHAI_BATI:
      /*--------------------------------------------------------------------
        Retrieve the Broadcast ATI (always valid)
      --------------------------------------------------------------------*/
      ati = 0x00000000;
      break;

    case HDRHAI_UATI:
      /*--------------------------------------------------------------------
        Retrieve the most recent Unicast ATI
      --------------------------------------------------------------------*/
      if ( *uati_count != 0 )
      {
        ati = uati_list_ptr[ (*uati_count) - 1].uati;
      }
          
      break;

    case HDRHAI_RATI:
      /*--------------------------------------------------------------------
        Retrieve the Random ATI (the latest RATI used is returned, even if
        the RATI is currently in an invalid state)
      --------------------------------------------------------------------*/
      ati  = hdrampati.receive_rati;
      break;

    case HDRHAI_MATI:
    case HDRHAI_ATI_INACTIVE:

      break;

    default:

      ERR("Requesting an invalid receive ATI",0,0,0);
      break;
  } /* switch ( ati_type ) */

  return ( ati );

} /* hdrampati_receive_ati_get */

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
void hdrampati_receive_ati_delete 
(
  hdrhai_ati_enum_type ati_type
)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch ( ati_type )
  {
    case HDRHAI_BATI:
      /*--------------------------------------------------------------------
        Invalidate the Broadcast ATI
      --------------------------------------------------------------------*/
      break;

    case HDRHAI_RATI:
      /*--------------------------------------------------------------------
        Render the Random ATI invalid (we don't reset its value at this 
        point still we still want to be able to save the latest RATI used
        to nv)
      --------------------------------------------------------------------*/
      hdrampati.receive_rati_is_valid = FALSE;
      break;

    case HDRHAI_UATI:
      /*--------------------------------------------------------------------
        Delete the oldest UATI
      --------------------------------------------------------------------*/
      hdrampati_uati_delete();
      break;
  
    case HDRHAI_MATI:
    case HDRHAI_ATI_INACTIVE:

      break;

    default:

      ERR("Attempting to delete an invalid receive ATI",0,0,0);
      break;

  } /* switch ( ati_type ) */

} /* hdrampati_receive_ati_delete */

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
void hdrampati_receive_ati_clear( void )
{
  uint8 index; 
    /* FOR loop index */

  uint8 *uati_count = &hdrampati.receive_uati_count;
  hdrampati_receive_uati_record_type *uati_list_ptr = hdrampati.receive_uati;
    /* convenience local variables */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Clear the n-address timer
  -------------------------------------------------------------------------*/
  (void) rex_clr_timer(&hdrampati.n_addr_timer);

  /*-------------------------------------------------------------------------
    Reset the UATI list
  -------------------------------------------------------------------------*/
  *uati_count = 0;

  for (index = 0; index < HDRAMPATI_MAX_NUM_REC_UATI_ELTS; index++) 
  {
    uati_list_ptr[index].uati       = HDRHAI_ATI_NO_VALUE;
    uati_list_ptr[index].time_valid = 0;
  }

  /*-------------------------------------------------------------------------
    Clear the RATI
  -------------------------------------------------------------------------*/
  hdrampati.receive_rati_is_valid  = FALSE;
  hdrampati.receive_rati           = HDRHAI_ATI_NO_VALUE;  
  
  /*-------------------------------------------------------------------------
    Clear the last ACMAC ATI
  -------------------------------------------------------------------------*/
  hdrampati.last_acmac_ati_is_valid = FALSE;
  hdrampati.last_acmac_ati          = HDRHAI_ATI_NO_VALUE;

} /* hdrampati_receive_ati_clear */

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
boolean hdrampati_receive_rati_is_valid ( void )
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  return ( hdrampati.receive_rati_is_valid );

} /* hdrampati_receive_rati_is_valid */

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
void hdrampati_set_last_acmac_ati 
( 
  uint32 last_acmac_ati_value
)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  hdrampati.last_acmac_ati = last_acmac_ati_value;
  hdrampati.last_acmac_ati_is_valid = TRUE;

} /* hdrampati_set_last_acmac_ati */

/* <EJECT> */  
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

boolean hdrampati_match_receive_ati_address
(
  hdrhai_ati_record_type *receive_ati_ptr,
  hdrhai_protocol_name_enum_type sender
)
{
  uint8 index; 
    /* FOR loop index */

  boolean uati_found = FALSE;
    /* indicates whether or not the ATI was found */ 
    
  uint8 *uati_count = &hdrampati.receive_uati_count;
  hdrampati_receive_uati_record_type *uati_list_ptr = hdrampati.receive_uati;
    /* convenience local variables */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
                   
  if (receive_ati_ptr == NULL)
  {
    return (uati_found);
  }

  switch ( receive_ati_ptr->ati_type )
  {
    case HDRHAI_BATI:
      /*--------------------------------------------------------------------
        Match a broadcast ATI: always TRUE.  No trace msg needed, return.
      --------------------------------------------------------------------*/
      return ( TRUE );

    case HDRHAI_UATI:
      /*--------------------------------------------------------------------
        Match a Unicast ATI
      --------------------------------------------------------------------*/
      /* go through all receive UATI addresses.  If any matches, return TRUE */
      #if defined (FEATURE_DEDICATED_C2K_MODE) || defined (FEATURE_IRAT_PILOT_MEAS)
      if (!hdrsmp_is_to_maintain_evdo_session())
      {
        HDR_MSG_PROT(MSG_LEGACY_HIGH, "DCM enabled, ignore UATI");
        return FALSE;
      }
      #endif /* FEATURE_DEDICATED_C2K_MODE || FEATURE_IRAT_PILOT_MEAS */

      for (index = 0; index < *uati_count; index++)
      {
        if ( uati_list_ptr[index].uati == receive_ati_ptr->ati_value )
        {
          uati_found = TRUE;
          break;
        }
      }
      break;

#ifdef FEATURE_HDR_QCHAT
    case HDRHAI_MATI:
       if (hdrqchatupk_mcs_enabled())
       {
         return hdrqchatupk_ampati_handle_hdrhai_mati(receive_ati_ptr->ati_value);
       }
       break;    
#endif /*FEATURE_HDR_QCHAT*/

    case HDRHAI_RATI:
      /*--------------------------------------------------------------------
        Match the Random ATI, if valid
      --------------------------------------------------------------------*/
      if ( (hdrampati.receive_rati_is_valid)
       && receive_ati_ptr->ati_value == hdrampati.receive_rati)
      {
        uati_found = TRUE;
      }
      break;

    default:
      /*--------------------------------------------------------------------
        Invalid
      --------------------------------------------------------------------*/
      break;
  } /* switch (receive_ati_ptr->ati_type) */

  if ( (hdrampati.last_acmac_ati_is_valid) 
   && hdramac_is_in_attempt(HDRHAI_ADDRESS_MANAGEMENT_PROTOCOL)
   && hdrampati.last_acmac_ati == receive_ati_ptr->ati_value )
  {
    uati_found = TRUE;
  }
  
  HDRTRACE_HDRAMP_RECEIVE_ATI_MATCH(sender, uati_found); //lint !e506
  return ( uati_found );

} /* hdrampati_match_address */

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

void hdrampati_powerup_init ( void )
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( hdrmc_is_first_init() )
  {
    /* define the n-address timer */
    rex_def_timer_ex( &hdrampati.n_addr_timer,
                      hdramp_timer_cb,
                      HDRAMP_N_ADDR_TIMER);
  }

  /* initialize the receive ati structure */
  hdrampati_receive_ati_clear();

} /* hdrampati_powerup_init */

/* <EJECT> */ 
