#ifndef RR_L1_IDLE_MODE_IF_H
#define RR_L1_IDLE_MODE_IF_H

/*============================================================================
  rr_l1_idle_mode_if.h

  This header file defines the signalling interface to the RR-L1-IDLE module.

                Copyright (c) 2011-2013 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
============================================================================*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_l1_idle_mode_if.h#1 $ */
/* $DateTime: 2016/12/13 08:00:21 $ */

/** 
  @file rr_l1_idle_mode_if.h
  @brief This header file defines the signalling interface to the RR-L1-IDLE module.
 */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "customer.h"
#include "comdef.h"
#include "gprsdef.h"
#include "gprsdef_g.h"
#include "rr_message_header.h"
#include "rr_gprs_defs.h"
#include "sys_type.h"
#include "rr_l1.h"
#include "rr_lai.h"
#include "rr_defs.h"
#include "geran_multi_sim.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/** 
  @enum rr_l1_idle_imsg_e
  @brief The RR-L1-IDLE IMSGs which can be sent to rr_l1_idle_control

  RR_L1_IDLE_IMSG_START_EARLY_CAMP_REQ
  @see rr_l1_idle_send_imsg_start_early_camp_req

  RR_L1_IDLE_IMSG_START_IDLE_REQ
  @see rr_l1_idle_send_imsg_start_idle_req

  RR_L1_IDLE_IMSG_CHANGE_IDLE_REQ
  @see rr_l1_idle_send_imsg_change_idle_req
 */
typedef enum
{
  RR_L1_IDLE_IMSG_START_EARLY_CAMP_REQ,
  RR_L1_IDLE_IMSG_START_IDLE_REQ,
  RR_L1_IDLE_IMSG_CHANGE_IDLE_REQ
} rr_l1_idle_imsg_e;

/** 
  @struct rr_l1_idle_imsg_start_early_camp_req_t
  @brief Definition of the RR_L1_IDLE_IMSG_START_EARLY_CAMP_REQ message
 */
typedef struct
{
  rr_message_header_t header;
  ARFCN_T arfcn;
  BSIC_T bsic;
  LAI_T lai;
  word cell_identity;
  control_channel_description_T control_channel_descr;
  boolean control_channel_descr_valid;
} rr_l1_idle_imsg_start_early_camp_req_t;

/** 
  @struct rr_l1_idle_imsg_start_idle_req_t
  @brief Definition of the RR_L1_IDLE_IMSG_START_IDLE_REQ message
 */
typedef struct
{
  rr_message_header_t header;
} rr_l1_idle_imsg_start_idle_req_t;

/** 
  @struct rr_l1_idle_imsg_change_idle_req_t
  @brief Definition of the RR_L1_IDLE_IMSG_CHANGE_IDLE_REQ message
 */
typedef struct
{
  rr_message_header_t header;
} rr_l1_idle_imsg_change_idle_req_t;

/**
  @union rr_l1_idle_imsg_u
  @brief Union of all the RR-L1-IDLE IMSGs which can be sent to rr_l1_idle_control
 */
typedef union
{
  rr_message_header_t                     header;
  rr_l1_idle_imsg_start_early_camp_req_t  early_camp_req;
  rr_l1_idle_imsg_start_idle_req_t        start_idle_req;
  rr_l1_idle_imsg_change_idle_req_t       change_idle_req;
} rr_l1_idle_imsg_u;

/** 
  @enum rr_l1_idle_omsg_e
  @brief The RR-L1-IDLE OMSGs which can be output by rr_l1_idle_control

  RR_L1_IDLE_OMSG_START_EARLY_CAMP_CNF
  @see rr_l1_idle_send_omsg_start_early_camp_cnf

  RR_L1_IDLE_OMSG_START_IDLE_CNF
  @see rr_l1_idle_send_omsg_start_idle_cnf
 */
typedef enum
{
  RR_L1_IDLE_OMSG_START_EARLY_CAMP_CNF,
  RR_L1_IDLE_OMSG_START_IDLE_CNF,
} rr_l1_idle_omsg_e;

/**
  @struct rr_l1_idle_omsg_start_early_camp_cnf_t
  @brief Definition of the RR_L1_IDLE_OMSG_START_EARLY_CAMP_CNF message.
 */
typedef struct
{
  rr_message_header_t header;
} rr_l1_idle_omsg_start_early_camp_cnf_t;

/**
  @struct rr_l1_idle_omsg_start_idle_cnf_t
  @brief Definition of the RR_L1_IDLE_OMSG_START_IDLE_CNF message.
 */
typedef struct
{
  rr_message_header_t header;
  boolean             gprs_allowed;
} rr_l1_idle_omsg_start_idle_cnf_t;

/**
  @union rr_l1_idle_omsg_u
  @brief Union of all the RR-L1-IDLE OMSGs which can be output by rr_l1_idle_control
 */
typedef union
{
  rr_message_header_t                     header;
  rr_l1_idle_omsg_start_early_camp_cnf_t  start_early_camp_cnf;
  rr_l1_idle_omsg_start_idle_cnf_t        start_idle_cnf;
} rr_l1_idle_omsg_u;

/**
  @union rr_l1_idle_msg_u
  @brief Union of all the RR-L1-IDLE IMSGs and OMSGs
 */
typedef union
{
  rr_l1_idle_imsg_u  imsg;
  rr_l1_idle_omsg_u  omsg;
} rr_l1_idle_msg_u;

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/**
  @brief Sends RR_L1_IDLE_IMSG_START_EARLY_CAMP_REQ to rr_l1_idle_control.

  This message request that L1 be put in EARLY CAMPING
 */
extern void rr_l1_idle_send_imsg_start_early_camp_req(
  ARFCN_T arfcn,
  BSIC_T bsic,
  LAI_T lai,
  word cell_identity,
  control_channel_description_T control_channel_descr,
  boolean control_channel_descr_valid,
  const gas_id_t gas_id
);

/**
  @brief Sends RR_L1_IDLE_IMSG_START_IDLE_REQ to rr_l1_idle_control.

  This message requests that L1 be put in IDLE / PACKET IDLE
 */
extern void rr_l1_idle_send_imsg_start_idle_req(const gas_id_t gas_id);

/**
  @brief Sends RR_L1_IDLE_IMSG_CHANGE_IDLE_REQ to rr_l1_idle_control.

  This message request that L1 idle mode be re-started due to a change in the cell configuration (i.e. GPRS
  INDICATOR in SI3 changes), or a TBF establishment is abandoned, etc.
 */
extern void rr_l1_idle_send_imsg_change_idle_req(const gas_id_t gas_id);

/**
  @brief Returns a character string representing the given RR_L1_IDLE_IMSG.
 */
extern const char *rr_l1_idle_imsg_name(
  rr_l1_idle_imsg_e msg
);

/**
  @brief Sends RR_L1_IDLE_OMSG_START_EARLY_CAMP_CNF to rr_main.

  This message confirms that L1 has started EARLY CAMPING
 */
extern void rr_l1_idle_send_omsg_start_early_camp_cnf(const gas_id_t gas_id);

/**
  @brief Sends RR_L1_IDLE_OMSG_START_IDLE_CNF to rr_main.

  This message confirms that L1 has started IDLE
 */
extern void rr_l1_idle_send_omsg_start_idle_cnf(
  boolean gprs_allowed,
  const gas_id_t gas_id
);

/**
  @brief Returns a character string representing the given RR_L1_IDLE_OMSG.
 */
extern const char *rr_l1_idle_omsg_name(rr_l1_idle_omsg_e msg);

#endif /* RR_L1_IDLE_MODE_IF_H */

/* EOF */

