#ifndef RR_FEMTO_IF_H
#define RR_FEMTO_IF_H

/*============================================================================
  rr_femto_if.h

  Signalling interface for the rr_femto module.

                Copyright (c) 2010-2013 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
============================================================================*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_femto_if.h#1 $ */
/* $DateTime: 2016/12/13 08:00:21 $$Author: mplcsds1 $ */

/** 
  @file rr_femto_if.h 
  @brief This header file specifies the RR IMSG/OMSG signalling interface for
         the RR-FEMTO module
    
  This header file specifies the RR IMSG/OMSG signalling interface for the
  RR-FEMTO module. Also see the associated module header file, rr_femto.h. This 
  contains other external functions which are required for the module to 
  function correctly, such as the initialisation function and main state machine 
  message input function. \n \n 
    
  External functions are provided to generate the RR-FEMTO IMSGs that this
  module understands, and the RR-FEMTO OMSGs to be processed by the parent
  module (which is expected to be RR-CONTROL).
 */

/*---------------------------------------------------------------------------
 * Include Files
 *--------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"

#ifdef FEATURE_FEMTO_GSM_NL

#include "comdef.h"
#include "rr_message_header.h"
#include "rr_defs.h"

/*---------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *--------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
 * Type Declarations
 *--------------------------------------------------------------------------*/

/** 
  @enum rr_femto_imsg_e
  @brief The RR-FEMTO IMSGs which can be sent to rr_femto_control
 
  RR_FEMTO_IMSG_L1_ACTIVATED_IND
  @see rr_femto_send_imsg_l1_activated_ind
   
  RR_FEMTO_IMSG_L1_DEACTIVATED_IND
  @see rr_femto_send_imsg_l1_deactivated_ind
 */
typedef enum
{
  RR_FEMTO_IMSG_L1_ACTIVATED_IND,
  RR_FEMTO_IMSG_L1_DEACTIVATED_IND,
  RR_FEMTO_IMSG_COUNT
} rr_femto_imsg_e;

/** 
  @struct rr_femto_imsg_l1_activated_ind_t
  @brief Definition of the RR_FEMTO_IMSG_L1_ACTIVATED_IND message 
  This message contains no information 
 */ 
typedef struct
{
  rr_message_header_t                 header;
} rr_femto_imsg_l1_activated_ind_t;

/** 
  @struct rr_femto_imsg_l1_deactivated_ind_t
  @brief Definition of the RR_FEMTO_IMSG_L1_DEACTIVATED_IND message
  This message contains no information 
 */ 
typedef struct
{
  rr_message_header_t                 header;
} rr_femto_imsg_l1_deactivated_ind_t;

/** 
  @union rr_femto_imsg_u
  @brief Union of all the RR-FEMTO IMSGs which can be sent to rr_femto_control
 */ 
typedef union
{
  rr_message_header_t                 header;
  rr_femto_imsg_l1_activated_ind_t    rr_femto_imsg_l1_activated_ind;
  rr_femto_imsg_l1_deactivated_ind_t  rr_femto_imsg_l1_deactivated_ind;
} rr_femto_imsg_u;

/** 
  @enum rr_femto_omsg_e
  @brief The RR-FEMTO OMSGs which can be output by rr_femto_control
 
  RR_FEMTO_OMSG_ACTIVE_IND
  @see rr_femto_send_omsg_active_ind   
   
  RR_FEMTO_OMSG_INACTIVE_IND
  @see rr_femto_send_omsg_inactive_ind   
 */ 
typedef enum
{
  RR_FEMTO_OMSG_ACTIVE_IND,
  RR_FEMTO_OMSG_INACTIVE_IND,
  RR_FEMTO_OMSG_COUNT
} rr_femto_omsg_e;

/** 
  @struct rr_femto_omsg_active_ind_t
  @brief Definition of the RR_FEMTO_OMSG_ACTIVE_IND message.
 */ 
typedef struct
{
  rr_message_header_t                 header;
} rr_femto_omsg_active_ind_t;

/** 
  @struct rr_femto_omsg_inactive_ind_t
  @brief Definition of the RR_FEMTO_OMSG_INACTIVE_IND message.
 */ 
typedef struct
{
  rr_message_header_t                 header;
} rr_femto_omsg_inactive_ind_t;

/** 
  @union rr_femto_omsg_u 
  @brief Union of all the RR-FEMTO OMSGs which can be output by rr_femto_control 
 */ 
typedef union
{
  rr_message_header_t                 header;
  rr_femto_omsg_active_ind_t          rr_femto_omsg_active_ind;
  rr_femto_omsg_inactive_ind_t        rr_femto_omsg_inactive_ind;
} rr_femto_omsg_u;

/** 
  @union rr_femto_msg_u 
  @brief Union of all the RR-FEMTO IMSGs and OMSGs
 */ 
typedef union
{
  rr_femto_imsg_u                     imsg;
  rr_femto_omsg_u                     omsg;
} rr_femto_msg_u;

/*---------------------------------------------------------------------------
 * Function Declarations and Documentation
 *--------------------------------------------------------------------------*/

/**
  @brief Sends RR_FEMTO_IMSG_L1_ACTIVATED_IND to rr_femto_control.
 
  This message indicates that L1 has been activated
 */
extern void rr_femto_send_imsg_l1_activated_ind(void);

/**
  @brief Sends RR_FEMTO_IMSG_L1_DEACTIVATED_IND to rr_femto_control.
   
  This message indicates that L1 has been deactivated
 */ 
extern void rr_femto_send_imsg_l1_deactivated_ind(void);

/**
  @brief Outputs RR_FEMTO_OMSG_ACTIVE_IND from rr_femto_control.
   
  This message indicates that the RR-FEMTO module is active and a procedure is 
  in progress 
 */ 
extern void rr_femto_send_omsg_active_ind(void);

/**
  @brief Outputs RR_FEMTO_OMSG_INACTIVE_IND from rr_femto_control.
   
  This message indicates that a procedure has completed and the RR-FEMTO module 
  is now inactive 
 */ 
extern void rr_femto_send_omsg_inactive_ind(void);


#endif // FEATURE_FEMTO_GSM_NL
#endif // RR_FEMTO_IF_H

/* EOF */

