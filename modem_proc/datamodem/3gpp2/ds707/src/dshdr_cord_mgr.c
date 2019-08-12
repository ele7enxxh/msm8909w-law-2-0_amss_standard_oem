/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
                       D A T A   S E R V I C E S   C O R D

                                 M A N A G E R

GENERAL DESCRIPTION
  This file contains functions used for implementing FEATURE_DS_CORD.

EXTERNALIZED FUNCTIONS
  DSHDR_PKT_CORD_REORIG_AFTER_PHYS_LINK_DOWN    
    Triggers the bring up of traffic channel and takes care of edge conditions
  
INITIALIZATION AND SEQUENCING REQUIREMENTS
    None

   Copyright (c) 2006-2011 by Qualcomm Technologies Incorporated.  All Rights Reserved.
   
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/src/dshdr_cord_mgr.c#1 $ 
  $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/15/11   mg      Global variable cleanup
10/13/10   op      Migrated to MSG 2.0 macros
02/15/09   ls      Initialize epzid in ds3g_pkt_call_info_type
09/09/05   ssr     New file: contains code that implements FEATURE_DS_CORD
                   This feature provides for traffic channel origination on 
                   receiving incoming DoS messages. Note that this feature is 
                   valid only for EV-DO.
===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "target.h"
#include "customer.h"

#ifdef FEATURE_HDR_DOS
#ifdef FEATURE_DS_CORD

#include "amssassert.h"
#include "ds707.h"
#include "dshdr_cord_mgr.h"
#include "ds707_pkt_mgri.h"
#include "ds707_pkt_mgr.h"
#include "dstask.h"
#include "err.h"
#include "msg.h"
#include "data_msg.h"
#include "ds707_drs.h"
#include "cm.h"

/*===========================================================================
            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE
===========================================================================*/

/*---------------------------------------------------------------------------------
 * dshdr_pkt_cord_reorig_after_phys_link_down_ev - 
     This flag is TRUE if reorigination needs to be attempted after a physlink down event.
 ----------------------------------------------------------------------------------*/ 

LOCAL boolean dshdr_pkt_cord_reorig_after_phys_link_down_ev = FALSE;

/*===========================================================================
                        EXTERNAL FUNCTION DEFINTIONS
===========================================================================*/

/*===========================================================================
FUNCTION      dshdr_pkt_cord_reorig_after_phys_link_down

DESCRIPTION   This function is called whenever DS707_CBACK_PHYS_LINK_DOWN_EV
              happens while going dormant. It tries to bring up the traffic
              channel if dshdr_pkt_cord_reorig_after_phys_link_down_ev is TRUE              
  
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void dshdr_pkt_cord_reorig_after_phys_link_down (void)
{
  ps_phys_link_type        *phys_link_ptr;
  ds3g_pkt_call_info_type  pkt_info;
  int16                     errno;
  int                       ret_val;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  DATA_IS707_MSG0(MSG_LEGACY_MED, "dshdr_pkt_cord_reorig_after_phys_link_down called");

  phys_link_ptr = ds707_pkt_get_pri_phys_link_ptr();
  if ((dshdr_pkt_cord_reorig_after_phys_link_down_ev == TRUE) &&
       (ds707_pkt_is_dormant(phys_link_ptr) == TRUE))
  { 
    /*-------------------------------------------------------------------
    drs_bit set to TRUE, as it is expected that data will be sent after 
    doing CORD operations
    -------------------------------------------------------------------*/
    pkt_info.drs_bit         = TRUE;
    /*-------------------------------------------------------------------
    DO does not use holddown timer, nevertheless set to FALSE. 
    -------------------------------------------------------------------*/
    pkt_info.use_hdown_timer = FALSE;
   /*-------------------------------------------------------------------
    Need to Populate the new_network to be the current data sess 
    network. Otherwise the Orig handler will always set the current
    data session network to zero which is NO_SRVC
    -------------------------------------------------------------------*/
    pkt_info.new_network     = ds707_pkt_get_current_data_sess_network();
    /*-------------------------------------------------------------------
    Set force_cdma_only to FALSE as we don't want the call to come up on 1x
    -------------------------------------------------------------------*/
    pkt_info.force_cdma_only = FALSE; 
   /*-------------------------------------------------------------------
    Set the inc_call flag to FALSE for all Origination attempts
     -------------------------------------------------------------------*/
    pkt_info.inc_call        = FALSE; 

    /*-------------------------------------------------------------------
     Set the epzid flag to FALSE for all Origination attempts
    -------------------------------------------------------------------*/
    pkt_info.epzid           = FALSE;

    ret_val = ps_phys_link_up_cmd(phys_link_ptr, &errno, &pkt_info);
    DATA_IS707_MSG2(MSG_LEGACY_HIGH, "ps_phys_link_up_cmd() returned: retVal %d errno %d", ret_val, errno);

    if ((ret_val < 0) && (errno == DS_ENETGOINGDORMANT))
    {
      DATA_IS707_MSG0(MSG_LEGACY_ERROR, "phys_link reorig failed after "
                      "dormancy still going dormant");
    }    
    else
    {
       dshdr_pkt_cord_reorig_after_phys_link_down_ev = FALSE;
    }
  }  
} /* dshdr_pkt_cord_reorig_after_phys_link_down() */

/*===========================================================================
FUNCTION      dshdr_pkt_cord_orig_hdlr

DESCRIPTION   This function is called whenever incoming DoS packet is received. 
              It tries to bring up the traffic channel if phys_link is
              dormant. Else, tries to reorig after phys_link goes dormant.
  
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void dshdr_pkt_cord_orig_hdlr (void)
{
  ps_phys_link_type        *phys_link_ptr; /* ptr to PRI phys_link */
  ps_iface_type            *iface_ptr;     /* ptr to IPV4 iface */
  ps_iface_state_enum_type  iface_state;
  ds3g_pkt_call_info_type  pkt_info;
  int16                     errno;
  int                       ret_val;
  phys_link_state_type      pri_phys_link_state;

  dshdr_pkt_cord_reorig_after_phys_link_down_ev = FALSE;
  

  /*-------------------------------------------------------------------------
    Get CDMA_SN_IFACE
  -------------------------------------------------------------------------*/
  iface_ptr = ps_iface_get_handle(PS_IFACE_GET_ID3(CDMA_SN_IFACE, 0, 0));
  if(iface_ptr == NULL)
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH, "dshdr_pkt_cord_orig_hdlr: CMDA_SN_IFACE pointer is NULL. \
             Returning");
    return;
  }  

  iface_state = ps_iface_state(iface_ptr);  
  /*-------------------------------------------------------------------
   If the IFACE itself is DOWN or GOING_DOWN, then really no point 
   doing CORD operations and bringing up phys_link (or traffic channel)
   -------------------------------------------------------------------*/
  if ((iface_state == IFACE_GOING_DOWN) || (iface_state == IFACE_DOWN))
  {
    DATA_IS707_MSG1(MSG_LEGACY_HIGH, "Returning due to IFACE state %d DOWN or GOING_DOWN", iface_state);
    return;
  }

  phys_link_ptr = ds707_pkt_get_pri_phys_link_ptr();  
  /*-------------------------------------------------------------------
   Make sure physlink is not NULL. Just a sanity test
   -------------------------------------------------------------------*/
  if (phys_link_ptr == NULL)
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH, "dshdr_pkt_cord_orig_hdlr: phys_link is NULL. Returning");
    return;
  }

  pri_phys_link_state = PS_PHYS_LINK_GET_STATE(phys_link_ptr);

  /*------------------------------------------------------------------------
   No need to do CORD operations if IFACE is DOWN or GOING_DOWN.   
   The logic below implements this table.
   __________________________________________________________________________
      IFACE State        DOWN   COMING_UP    UP	    GOING_DOWN CONFIGURING
    Physlink state
   __________________________________________________________________________
    UP                    I	     No, F	   No, F	  No, F	      No, F
    DOWN	              I	       I       Yes, F     Yes, F      Yes, F
    GOING_DOWN	          I	       I       No, T      No, T	      No, T
    GOING_NULL	          I	       I         I        No, F	        I
    NULL	            No, F	 No, F       I        No, F	        I
    COMING_UP	          I	     No, F       I        No, F	        I
    RESUMING	          I	       I       No, F      No, F	        I
    __________________________________________________________________________

    I - invalid state combination, cannot occur
    Yes or No refers to whether TC is brough up or not
    T or F refers to whether reorigination is attempted after TC goes dormant 
    ---------------------------------------------------------------------------*/
  switch (pri_phys_link_state) 
  {
    case PHYS_LINK_NULL:
    case PHYS_LINK_GOING_NULL:
      DATA_IS707_MSG1(MSG_LEGACY_HIGH,"dshdr_pkt_cord_orig_hdlr():"
                 "phys_link state %d - not originating TC", 
                pri_phys_link_state);
      break;

    case PHYS_LINK_COMING_UP:
    case PHYS_LINK_RESUMING:
      DATA_IS707_MSG1(MSG_LEGACY_MED,"dshdr_pkt_cord_orig_hdlr():"
                 "phys_link state %d: orig already attempted - no need to \
               attempt again", pri_phys_link_state);
      break;

    case PHYS_LINK_UP:
      DATA_IS707_MSG1(MSG_LEGACY_MED,"dshdr_pkt_cord_orig_hdlr():"
                   "phys_link state %d: already up!", pri_phys_link_state);
      break;

    case PHYS_LINK_GOING_DOWN:
      DATA_IS707_MSG1(MSG_LEGACY_MED, "dshdr_pkt_cord_orig_hdlr():"
                "phys_link state %d: - wait and reorig", 
               pri_phys_link_state);
      dshdr_pkt_cord_reorig_after_phys_link_down_ev = TRUE;     
      break;

    case PHYS_LINK_DOWN:
      /*-------------------------------------------------------------------
       Do CORD operations => Origniate TC 
       -------------------------------------------------------------------*/
      /*-------------------------------------------------------------------
       drs_bit set to TRUE, as it is expected that data will be sent after 
       doing CORD operations
      -------------------------------------------------------------------*/
      pkt_info.drs_bit         = TRUE;
      /*-------------------------------------------------------------------
       DO does not use holddown timer, nevertheless set to FALSE. 
      -------------------------------------------------------------------*/
      pkt_info.use_hdown_timer = FALSE;
     /*-------------------------------------------------------------------
       Need to Populate the new_network to be the current data sess 
       network. Otherwise the Orig handler will always set the current
       data session network to zero which is NO_SRVC
      -------------------------------------------------------------------*/
      pkt_info.new_network     = ds707_pkt_get_current_data_sess_network();
      /*-------------------------------------------------------------------
       Set force_cdma_only to FALSE as we don't want the call to come up on 1x
      -------------------------------------------------------------------*/
      pkt_info.force_cdma_only = FALSE; 
      /*-------------------------------------------------------------------
       Set the inc_call flag to FALSE for all Origination attempts
      -------------------------------------------------------------------*/      
      pkt_info.inc_call        = FALSE;     
      
      /*-------------------------------------------------------------------
       Set the epzid flag to FALSE for all Origination attempts
      -------------------------------------------------------------------*/
      pkt_info.epzid           = FALSE;

      ret_val = ps_phys_link_up_cmd(phys_link_ptr, &errno, &pkt_info);      
      if ((ret_val < 0) && (errno != DS_EWOULDBLOCK))
      {
        DATA_IS707_MSG2(MSG_LEGACY_ERROR, "ps_phys_link_up_cmd triggered by "
                        "MT DoS failed - retVal %d errno %d",
                        ret_val, errno);
      }
      break;  

    default:
     DATA_IS707_MSG1(MSG_LEGACY_ERROR, "Invalid phys_link state %d",
                     PS_PHYS_LINK_GET_STATE(phys_link_ptr));
     ASSERT(0);
     break;
  }
} /* dshdr_pkt_cord_orig_hdlr() */

#endif /* FEATURE_DS_CORD */
#endif /* FEATURE_HDR_DOS */
