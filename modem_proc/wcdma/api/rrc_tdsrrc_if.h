#ifndef RRC_TDSRRC_IF_H
#define RRC_TDSRRC_IF_H
/*===========================================================================
         R R C  -  T D S R R C   P R I M I T I V E / T Y P E   D E F I N I T I O N S  

DESCRIPTION

  This module is the header module that contains the type definitions for
  the primitives between RRC and TDS RRC.
  
Copyright (c) 2002-2012 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

09/05/12   gv     Created file.
===========================================================================*/

/* ==========================================================================
** Includes and Public Data Declarations
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */

#include "comdef.h"
#include "rrc_rr_types.h"

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

/***************************************************************************/
/********************** MSG ID's Rxd by RRC **********************************/
/***************************************************************************/

/* Command identifier base for the commands received by RRC from TDS RRC */
#define RRC_TDSRRC_CMD_ID_BASE 0x0E000000 /* NOTE: This value should not be changed.
                                         The same value is also used in rrccmd.h */

/* RRC - TDSRRC Command Id type */
typedef enum
{
  /**********************************************************************/
  /* TDS RRC to RRC command base. Note: starting value is RRC_TDSRRC_CMD_ID_BASE */
  TDSRRC_TO_RRC_BASE = RRC_TDSRRC_CMD_ID_BASE,

  /* Add new TDS RRC to RRC commands below */

  /**********************************************************************/
  /*********** Responses from TDS RRC for the requests sent by RRC ************/
  /**********************************************************************/

  /****** Response for Stop Mode from RRC -> TDS RRC***/
  TDSRRC_RRC_STOP_TDS_CNF,

  /*********************************************************************/
  /*************** Requests from TDS RRC received by RRC  ******************/
  /*********************************************************************/

  /****** TDS RRC -> RRC Stop Mode request ***********/
  RRC_TDSRRC_STOP_WCDMA_REQ
} rrc_tdsrrc_cmd_e_type;

typedef struct
{
  /* Made use of the existing enum between RRC and RR interface */
  rrc_rr_stop_wcdma_cause_e_type	stop_req_cause;
}rrc_tdsrrc_stop_wcdma_req_type;

typedef struct
{
  rrc_rr_stop_wcdma_cause_e_type	stop_cnf_cause;
}tdsrrc_rrc_stop_tds_cnf_type;

  /* Add below command structures in rrc_cmd_data_u_type (rrccmd.h) as well to prevent typecasting */
typedef union
{
  /* Requests from TDS RRC received by RRC */
  rrc_tdsrrc_stop_wcdma_req_type rrc_tdsrrc_stop_wcdma_req;

  /* Responses from TDS RRC for the requests sent by RRC */
  tdsrrc_rrc_stop_tds_cnf_type tdsrrc_rrc_stop_tds_cnf;

} rrc_tdsrrc_cmd_data_u_type;

typedef struct
{
  q_link_type           link;

  rrc_tdsrrc_cmd_e_type cmd_id;

  uint8 seq_num;
} rrc_tdsrrc_cmd_hdr_type;

typedef struct
{
  /* Command header common for all commands */
  rrc_tdsrrc_cmd_hdr_type    cmd_hdr;

  rrc_tdsrrc_cmd_data_u_type cmd;

}rrc_tdsrrc_cmd_type;

/*===========================================================================

FUNCTION RRC_PUT_TDSRRC_CMD

DESCRIPTION

  This function puts a command buffer into RRC's TDS RRC
  command queue. Note that the command buffer must have been
  requested through a call to rrc_get_tdsrrc_cmd_buf(). Also,
  The calling function must have filled all the necessary
  data including the command id before passing the buffer
  to this function.

  This function also sets the appropriate signal to the RRC
  task to indicate that a command has been placed in the
  command queue.

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  A signal is set for the TDSCDMA RRC task and this could cause a context
  switch.

===========================================================================*/
void rrc_put_tdsrrc_cmd
(
   rrc_tdsrrc_cmd_type *cmd_buf  /* The command buffer to be put in the queue */
);
/*===========================================================================

FUNCTION RRC_GET_TDSRRC_CMD_BUF

DESCRIPTION

  This function returns a RRC command buffer to the calling function.
  This function dynamically allocates memory for the necessary buffer.
  Note that this buffer MUST be freed by calling rrc_free_tdsrrc_cmd_buf().

  The freeing is done by the function that dequeues and processes this
  command buffer.

  Calling function MUST check for a NULL return value and take necessary
  action.

DEPENDENCIES

  None.

RETURN VALUE

  A buffer of type rrc_tdsrrc_cmd_type. If memory is not available, NULL
  is returned.

SIDE EFFECTS

  None.

===========================================================================*/
rrc_tdsrrc_cmd_type * rrc_get_tdsrrc_cmd_buf( void );

#endif /* RRC_TDSRRC_IF_H */
