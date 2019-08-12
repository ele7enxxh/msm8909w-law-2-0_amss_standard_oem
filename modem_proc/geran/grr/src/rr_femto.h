#ifndef RR_FEMTO_H
#define RR_FEMTO_H

/*============================================================================
  rr_femto.h

  This is the header file for the RR-FEMTO module.
  See below for a detailed description.

                Copyright (c) 2010-2013 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
============================================================================*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_femto.h#1 $ */
/* $DateTime: 2016/12/13 08:00:21 $$Author: mplcsds1 $ */

/** 
  @file rr_femto.h
  @brief This is the header file for the RR-FEMTO module
 
  This header file contains the definitions and external function prototypes
  needed to interact with the RR-FEMTO module.\n
  Also see the associated interface file, rr_femto_if.h. \n \n
 
  There are two procedures which this module will perform: \n \n
 
  1. Neighbour Discovery \n \n
 
  This procedure is started upon receipt of a RRC_RR_NBR_DISC_REQ message from
  RRC. RR will perform a power-scan and parallel BCCH decodes to determine valid
  neighbour cells, and then report these back to RRC in a RR_RRC_NBR_DISC_CNF
  message. \n
  This completes the procedure. \n \n
 
  2. Camp-On-Specific-Cell \n \n
 
  This procedure is started upon receipt of a RRC_RR_START_COSC_REQ message from
  RRC. The message contains a BCCH ARFCN to camp onto. \n
  If successful then RR reads SI3 and SI13 to obtain the cell identification
  informtion. It reports success/failure in RR_RRC_START_COSC_CNF message. RR
  remains camped on the cell until instructed to stop by RRC with a
  RRC_RR_STOP_COSC_REQ message. RR uncamps and responds with a
  RR_RRC_STOP_COSC_CNF message. \n
  This completes the procedure.
 */

/*----------------------------------------------------------------------------
   Include Files
 * -------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"

#ifdef FEATURE_FEMTO_GSM_NL

#include "comdef.h"
#include "rr_task.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/**
  @brief RR-FEMTO module initialisation function. Should be called once at RR
  task start
 */
extern void rr_femto_init(void);

/**
  @brief The main RR-FEMTO module state machine. 
   
  This state machine coordinates the RR-Femto procedures. Messages passed in can 
  be any that are a member of rr_cmd_bdy_type, including RR_RR internal 
  messages. \n
  Note: Legacy RR events are not used in this module. 

  @param msg_ptr 
 */
extern void rr_femto_control(rr_cmd_bdy_type *msg_ptr);

/**
  @brief Returns a character string representing the given RR_FEMTO_IMSG or 
         RR_FEMTO_OMSG message

  @param msg_ptr     - pointer to message
  @param message_set - (not used) 
  @param message_id  - (not used) 
  
  @return char* 
 */
extern char *rr_femto_message_name
(
  const rr_cmd_bdy_type *msg_ptr,
  const int message_set,
  const int message_id
);

#endif /* FEATURE_FEMTO_GSM_NL */
#endif /* RR_FEMTO_H */

/* EOF */

