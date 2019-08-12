#ifndef RR_DTM_IF_H
#define RR_DTM_IF_H

/*! \file rr_dtm_if.h 
 
  Signalling interface for the RR-DTM module.
 
                Copyright (c) 2014 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_dtm_if.h#1 $ */
/* $DateTime: 2016/12/13 08:00:21 $$Author: mplcsds1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"

#ifdef FEATURE_GSM_DTM

#include "comdef.h"
#include "rr_message_header.h"
#include "rr_defs.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/* Input messages. */
typedef enum
{
  RR_DTM_IMSG_COUNT,
  RR_DTM_IMSG_CS_RELEASE_REQ,
} rr_dtm_imsg_e;

typedef struct
{
  rr_message_header_t  header;
  boolean              edtm_cs_release_allowed;
} rr_dtm_imsg_cs_release_req_t;

typedef union
{
  rr_message_header_t           header;
  rr_dtm_imsg_cs_release_req_t  cs_release_req;
} rr_dtm_imsg_u;

/* Output messages. */
typedef enum
{
  RR_DTM_OMSG_CS_RELEASE_CNF,
  RR_DTM_OMSG_COUNT
} rr_dtm_omsg_e;

typedef enum
{
  RR_DTM_CE_REL_MODE_GSM,
  RR_DTM_CE_REL_MODE_DTM,
#ifdef FEATURE_GSM_EDTM
  RR_DTM_CE_REL_MODE_EDTM,
#endif
  RR_DTM_CE_REL_MODE_MAX
} rr_dtm_ce_rel_mode_e;

typedef struct
{
  rr_message_header_t   header;
  rr_dtm_ce_rel_mode_e  ce_rel_mode;
} rr_dtm_omsg_cs_release_cnf_t;

typedef union
{
  rr_message_header_t           header;
  rr_dtm_omsg_cs_release_cnf_t  cs_release_cnf;
} rr_dtm_omsg_u;

/* Union of all messages. */
typedef union
{
  rr_dtm_imsg_u imsg;
  rr_dtm_omsg_u omsg;
} rr_dtm_msg_u;

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/*!
 * \brief Returns the text string for the given message
 * 
 * \param imsg (in)
 * 
 * \return const char* 
 */
extern const char *rr_dtm_imsg_name(rr_dtm_imsg_e imsg);

/*!
 * \brief Sends RR_DTM_IMSG_CS_RELEASE_REQ
 * 
 * \param edtm_allowed (in)
 * \param gas_id (in)
 */
extern void rr_dtm_send_imsg_cs_release_req(
  boolean edtm_allowed,
  const gas_id_t gas_id
);

/*!
 * \brief Returns the text string for the given message
 * 
 * \return const char* 
 */
extern const char *rr_dtm_omsg_name(rr_dtm_omsg_e omsg);

/*!
 * \brief Sends RR_DTM_OMSG_CS_RELEASE_CNF
 * 
 * \param ce_rel_mode (in)
 * \param gas_id (in)
 */
extern void rr_dtm_send_omsg_cs_release_cnf(
  rr_dtm_ce_rel_mode_e ce_rel_mode,
  const gas_id_t gas_id
);

#endif // FEATURE_GSM_DTM

#endif // #ifndef RR_DTM_IF_H

/* EOF */

