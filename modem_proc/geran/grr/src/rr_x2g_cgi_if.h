#ifndef RR_X2G_CGI_IF_H
#define RR_X2G_CGI_IF_H

/*! \file rr_x2g_cgi_if.h 
 
  Signalling interface for the RR X2G CGI module.
 
                Copyright (c) 2011-2013 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_x2g_cgi_if.h#1 $ */
/* $DateTime: 2016/12/13 08:00:21 $$Author: mplcsds1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"

#ifdef FEATURE_LTE_TO_GSM_CGI

#include "comdef.h"
#include "rr_message_header.h"
#include "rr_defs.h"
#include "mm_rr.h"
#include "geran_multi_sim.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

#define RR_X2G_CGI_ARFCN_LIST_SIZE 32

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/* Input messages. */
typedef enum
{
  RR_X2G_CGI_IMSG_CGI_REQ,
  RR_X2G_CGI_IMSG_ABORT_REQ,
  RR_X2G_CGI_IMSG_L1_DEACTIVATED_IND,
  RR_X2G_CGI_IMSG_L1_ACTIVATED_IND,
  RR_X2G_CGI_IMSG_COUNT
} rr_x2g_cgi_imsg_e;

typedef struct
{
  uint32  count;
  ARFCN_T entries[RR_X2G_CGI_ARFCN_LIST_SIZE];
} rr_x2g_cgi_arfcn_list_t;


typedef enum
{
  RR_X2G_CGI_INFO_MASK_NONE    = 0x00,
  RR_X2G_CGI_INFO_MASK_RXLEV   = 0x01,
  RR_X2G_CGI_INFO_MASK_CELL_ID = 0x02,
  RR_X2G_CGI_INFO_MASK_LAI     = 0x04,
  RR_X2G_CGI_INFO_MASK_RAC     = 0x08,
  RR_X2G_CGI_INFO_MASK_ALL     = 0x0f
} rr_x2g_cgi_info_mask_e;

typedef struct
{
  rr_x2g_cgi_info_mask_e mask;
  uint8                  rxlev;
  uint16                 cell_id;
  LAI_T                  lai;
  /* rac_present will be FALSE if the cell does not support GPRS, TRUE
  otherwise. */
  boolean                rac_present;
  uint8                  rac;
} rr_x2g_cgi_info_t;

typedef struct
{
  rr_message_header_t     header;
  rr_x2g_cgi_arfcn_list_t arfcn_list;
  BSIC_T                  bsic;
  uint32                  time_available;
  rr_x2g_cgi_info_t       info;
} rr_x2g_cgi_imsg_cgi_req_t;

typedef union
{
  rr_message_header_t       header;
  rr_x2g_cgi_imsg_cgi_req_t cgi_req;
} rr_x2g_cgi_imsg_u;

typedef enum
{
  RR_X2G_CGI_OMSG_CGI_CNF,
  RR_X2G_CGI_OMSG_ABORT_CNF,
  RR_X2G_CGI_OMSG_COUNT
} rr_x2g_cgi_omsg_e;

typedef struct
{
  rr_message_header_t header;
  ARFCN_T             arfcn;
  BSIC_T              bsic;
  rr_x2g_cgi_info_t   info;
} rr_x2g_cgi_omsg_cgi_cnf_t;

typedef struct
{
  rr_message_header_t header;
  ARFCN_T             arfcn;
  BSIC_T              bsic;
  rr_x2g_cgi_info_t   info;
} rr_x2g_cgi_omsg_abort_cnf_t;

typedef union
{
  rr_message_header_t         header;
  rr_x2g_cgi_omsg_cgi_cnf_t   cgi_cnf;
  rr_x2g_cgi_omsg_abort_cnf_t abort_cnf;
} rr_x2g_cgi_omsg_u;

/* Union of all messages. */
typedef union
{
  rr_x2g_cgi_imsg_u imsg;
  rr_x2g_cgi_omsg_u omsg;
} rr_x2g_cgi_msg_u;

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

extern const char * rr_x2g_cgi_imsg_name(
  rr_x2g_cgi_imsg_e imsg
);

extern void rr_x2g_cgi_send_imsg(
  rr_x2g_cgi_imsg_e imsg,
  const gas_id_t gas_id
);

extern void rr_x2g_cgi_send_imsg_cgi_req(
  rr_x2g_cgi_arfcn_list_t * arfcn_list,
  BSIC_T                    bsic,
  uint32                    time_available,
  rr_x2g_cgi_info_t       * info,
  const gas_id_t            gas_id
);

extern const char * rr_x2g_cgi_omsg_name(
  rr_x2g_cgi_omsg_e omsg
);

extern void rr_x2g_cgi_send_omsg(
  rr_x2g_cgi_omsg_e omsg,
  const gas_id_t gas_id
);

extern void rr_x2g_cgi_send_omsg_cgi_cnf(
  ARFCN_T             arfcn,
  BSIC_T              bsic,
  rr_x2g_cgi_info_t * info,
  const gas_id_t      gas_id
);

extern void rr_x2g_cgi_send_omsg_abort_cnf(
  ARFCN_T             arfcn,
  BSIC_T              bsic,
  rr_x2g_cgi_info_t * info,
  const gas_id_t      gas_id
);

#endif /* FEATURE_LTE_TO_GSM_CGI */

#endif /* RR_X2G_CGI_IF_H */

/* EOF */


