/*============================================================================
  rr_femto_if.c

  This file contains the functionality to send the RR-FEMTO IMSG/OMSG messages.
  See comments below for more detail.

                Copyright (c) 2009-2013 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
============================================================================*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_femto_if.c#1 $ */
/* $DateTime: 2016/12/13 08:00:21 $$Author: mplcsds1 $ */

/** 
  @file rr_femto_if.c
  @brief This file contains the functionality to send the RR-FEMTO IMSG/OMSG 
         messages.
   
  Note that the RR-FEMTO procedures are started upon reception of messages from 
  RRC. The RR-FEMTO IMSG/OMSGs are used while these procedures are ongoing. 
 */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"

#ifdef FEATURE_FEMTO_GSM_NL

#include "comdef.h"
#include "rr_seg_load.h"
#include "rr_femto_if.h"
#include "rr_rr.h"


/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *--------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 *--------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Global Data Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
* Externalized Function Definitions
* --------------------------------------------------------------------------*/


/**
  @brief Sends RR_FEMTO_IMSG_L1_ACTIVATED_IND to rr_femto_control
 
  This message indicates that L1 has been activated.
*/ 
void rr_femto_send_imsg_l1_activated_ind(void)
{
  rr_message_header_t header;

  rr_rr_send(&header,
             (uint8) RR_FEMTO_IMSG_L1_ACTIVATED_IND,
             sizeof(header),
             RR_FEMTO_IMSG);
}


/**
  @brief Sends RR_FEMTO_IMSG_L1_DEACTIVATED_IND to rr_femto_control
 
  This message indicates that L1 has been deactivated.
*/ 
void rr_femto_send_imsg_l1_deactivated_ind(void)
{
  rr_message_header_t header;

  rr_rr_send(&header,
             (uint8) RR_FEMTO_IMSG_L1_DEACTIVATED_IND,
             sizeof(header),
             RR_FEMTO_IMSG);
}


/**
  @brief Outputs RR_FEMTO_OMSG_ACTIVE_IND from rr_femto_control
 
  This message indicates that the RR-FEMTO module is active and a procedure is 
  in progress. The expected processor of this message is rr_control.
*/ 
void rr_femto_send_omsg_active_ind(void)
{
  rr_message_header_t header;

  rr_rr_send(&header,
             (uint8) RR_FEMTO_OMSG_ACTIVE_IND,
             sizeof(header),
             RR_FEMTO_OMSG);
}


/**
  @brief Outputs RR_FEMTO_OMSG_INACTIVE_IND from rr_femto_control
 
  This message indicates that a procedure has completed and the RR-FEMTO module 
  is now inactive. The expected processor of this message is rr_control. 
*/ 
void rr_femto_send_omsg_inactive_ind(void)
{
  rr_message_header_t header;

  rr_rr_send(&header,
             (uint8) RR_FEMTO_OMSG_INACTIVE_IND,
             sizeof(header),
             RR_FEMTO_OMSG);
}


#endif /* FEATURE_FEMTO_GSM_NL */
/* EOF */

