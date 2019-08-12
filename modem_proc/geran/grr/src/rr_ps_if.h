#ifndef RR_PS_IF_H
#define RR_PS_IF_H

/*! \file rr_ps_if.h 
 
  Signalling interface for the RR-PS modules.
 
                Copyright (c) 2011-2013 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_ps_if.h#1 $ */
/* $DateTime: 2016/12/13 08:00:21 $$Author: mplcsds1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"
#include "comdef.h"

#include "rr_message_header.h"
#include "gprsdef_g.h"
#include "geran_dual_sim.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

typedef enum
{
  RR_PS_IMSG_PS_ACCESS_REQ,
  RR_PS_IMSG_NO_PS_ACCESS_REQ
} rr_ps_imsg_e;

typedef struct
{
  rr_message_header_t header;
  boolean             purge_gmm_signalling;
} rr_ps_imsg_ps_access_req_t;

typedef struct
{
  rr_message_header_t header;
  boolean             suspend_gmm;
  delete_pdu_cause_t  delete_pdu;
} rr_ps_imsg_no_ps_access_req_t;

typedef union
{
  rr_message_header_t            header;
  rr_ps_imsg_ps_access_req_t     ps_access_req;
  rr_ps_imsg_no_ps_access_req_t  no_ps_access_req;
} rr_ps_imsg_u;

typedef enum
{
  RR_PS_OMSG_PS_ACCESS_CNF,
  RR_PS_OMSG_NO_PS_ACCESS_CNF
} rr_ps_omsg_e;

typedef struct
{
  rr_message_header_t header;
} rr_ps_omsg_ps_access_cnf_t;

typedef struct
{
  rr_message_header_t header;
} rr_ps_omsg_no_ps_access_cnf_t;

typedef union
{
  rr_message_header_t            header;
  rr_ps_omsg_ps_access_cnf_t     ps_access_cnf;
  rr_ps_omsg_no_ps_access_cnf_t  no_ps_access_cnf;
} rr_ps_omsg_u;

/* Union of all messages. */
typedef union
{
  rr_ps_imsg_u  imsg;
  rr_ps_omsg_u  omsg;
} rr_ps_msg_u;

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/*!
 * \brief Returns a text string representing 'imsg'.
 * 
 * \param imsg 
 * 
 * \return const char* 
 */
extern const char *rr_ps_imsg_name(
  rr_ps_imsg_e imsg
);

/*!
 * \brief Sends RR_PS_IMSG_PS_ACCESS_REQ.
 * 
 * \param purge_gmm_signalling - TRUE if GMM PDUs should be deleted
 * \param gas_id 
 */
extern void rr_ps_send_imsg_ps_access_req(
  boolean purge_gmm_signalling,
  const gas_id_t gas_id
);

/*!
 * \brief Sends RR_PS_IMSG_NO_PS_ACCESS_REQ.
 * 
 * \param suspend_gmm - TRUE if GMM should be suspended
 * \param delete_pdu - PDU deletion action {NO_DEL, DEL_ONE, DEL_ALL}
 * \param gas_id
 */
extern void rr_ps_send_imsg_no_ps_access_req(
  boolean suspend_gmm,
  delete_pdu_cause_t delete_pdu,
  const gas_id_t gas_id
);

/*!
 * \brief Returns a text string representing 'omsg'.
 * 
 * \param imsg 
 * 
 * \return const char* 
 */
extern const char *rr_ps_omsg_name(
  rr_ps_omsg_e omsg
);

/*!
 * \brief Sends RR_OMSG_PS_ACCESS_CNF.
 * 
 * \param gas_id
 */
extern void rr_ps_send_omsg_ps_access_cnf(
  const gas_id_t gas_id
);

/*!
 * \brief Sends RR_OMSG_NO_PS_ACCESS_CNF.
 * 
 * \param gas_id 
 */
extern void rr_ps_send_omsg_no_ps_access_cnf(
  const gas_id_t gas_id
);


#endif /* RR_PS_ACCESS_IF_H */

/* EOF */

