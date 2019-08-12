#ifndef TRACER_DIAG_H
#define TRACER_DIAG_H
/** =========================================================================
  @file tracer_diag.h

  @brief QUALCOMM Debug Subsystem (QDSS) Tracer interface with
         diag's packet service

               Copyright (c) 2012 Qualcomm Technologies Incorporated.
               All rights reserved.
               Qualcomm Confidential and Proprietary
===========================================================================*/
/* $Header: //components/rel/core.mpss/3.5.c12.3/debugtrace/tracer/src/tracer_diag.h#1 $ */

/*---------------------------------------------------------------------------
 * Include Files
 * ------------------------------------------------------------------------*/
#include "comdef.h"
#include "tracer.h"

/*---------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
 * Type Declarations
 * ------------------------------------------------------------------------*/
/**
  @brief Porcessor ID: upper 8-bits of subsysCmdCode
 */
#define QDSSDIAG_PROCESSOR_APPS   0x0100
#define QDSSDIAG_PROCESSOR_MODEM  0x0200
#define QDSSDIAG_PROCESSOR_WCNSS  0x0300
#define QDSSDIAG_PROCESSOR_RPM    0x0400
#define QDSSDIAG_PROCESSOR_LPASS  0x0500
#define QDSSDIAG_PROCESSOR_VENUS  0x0600
#define QDSSDIAG_PROCESSOR_GNSS   0x0700
#define QDSSDIAG_PROCESSOR_SENSOR 0x0800
#define QDSSDIAG_PROCESSOR_AUDIO  0x0900

/**
  @brief Tracer command codes: lower 8-bits of subsysCmdCode
 */
typedef enum
{
   TRACER_DIAG_QRY_TRACER_STATUS = 0x05, /**< Query of tracer status */
   TRACER_DIAG_CTRL_OP           = 0x06, /**< Enable/Disable all tracer output */
   TRACER_DIAG_QRY_ENTITY_STATE  = 0x07, /**< State of all entities */
   TRACER_DIAG_CTRL_ENTITY       = 0x08, /**< Enable/Disable tracer output associated with an entity*/
   TRACER_DIAG_QRY_SWE_TAG       = 0x09, /**< Tag identifying the sw event table */
   TRACER_DIAG_QRY_SWE_STATE     = 0x0A, /**< State of a group of 128 events */
   TRACER_DIAG_CTRL_SWE          = 0x0B, /**< Enable/Disable tracer output for given sw event */
   TRACER_DIAG_CTRL_RPMSWE       = 0x0C, /**< Configure RPM sw events */
   TRACER_DIAG_SET_SYSTESTCTRL   = 0x0D, /**< Set QDSS test mode */
   TRACER_DIAG_QRY_SWT_FLOW      = 0x0E, /**< Query a SW trace flow control parameter */
   TRACER_DIAG_CFG_SWT_FLOW      = 0x0F, /**< Configure a SW trace flow control parameter */
   TRACER_DIAG_PLACEHOLDER = 0x7FFF,
} tracer_ctrl_t;

/*-------------------------------------------------------------------------*/
/**
  Request/Response header for DIAG packet service.
  Common header which starts all request and response packets.
 */
typedef PACK(struct)
{
  uint8 cmdCode;        /**< Diag Message ID */
  uint8 subsysId;       /**< Subsystem ID (DIAG_SUBSYS_QDSS)*/
  uint16 subsysCmdCode; /**< Subsystem command code */
                        /* Upper 8-bits is the processor ID
                          (TRACER_DIAG_PROC_ID). Lower 8-bits is
                          the command code. */
} tracer_diag_pkt_hdr;

/*-------------------------------------------------------------------------*/
/**
  Response packet: Generic reponse with result.
  Common packet when only returning a result.
 */
typedef PACK(struct)
{
  tracer_diag_pkt_hdr hdr; /**< Header */
  uint8 result;            /**< See TRACER_CMDRESP_... definitions */
} tracer_diag_pkt_resp;

/*-------------------------------------------------------------------------*/
/**
  Request packet: Query for tracer output control and info.
  TRACER_DIAG_QRY_TRACER_STATUS

  Same as tracer_diag_pkt_hdr
 */

/**
  Response packet: Query for tracer output control and info.
  TRACER_DIAG_QRY_TRACER_STATUS

  Status information (when bit is set)
     Bit 0 = tracer output on
         1 = QDSS is On
         2 = SW evt is initialzied
         3 = tracer is initialized
         4 - 7 = Undefined
 */
typedef PACK(struct)
{
  tracer_diag_pkt_hdr hdr;
  uint8 status;     /**< Bit mask of status information */
  uint8 entityCnt;  /**< Count of entity ids with outputs turned on */
  uint16 eventCnt;  /**< Count of event ids with outputs turned on */
} tracer_diag_resp_tracer_status;

/*-------------------------------------------------------------------------*/
/**
  Request packet: Command to set tracer output control.
  TRACER_DIAG_CTRL_OP

  Generation of all tracer output (both data, client swevents, and
  non-client sw events) is enabled or disabled.
 */
typedef PACK(struct)
{
  tracer_diag_pkt_hdr hdr;
  uint8 state;  /**< 0 = Enable tracer outputs, 1 = Disable tracer outputs */
} tracer_diag_op_ctrl;

/**
  Response packet: Command to set tracer output control.
  TRACER_DIAG_CTRL_OP

  Same as tracer_diag_pkt_resp
*/

/*-------------------------------------------------------------------------*/
/**
  Request packet: Query for entity id's output state.
  TRACER_DIAG_QRY_ENTITY_STATE

  Same as tracer_diag_pkt_hdr
*/

/**
  Response packet: Query for entity id's output state.
  TRACER_DIAG_QRY_ENTITY_STATE

  Contains a 256-bit bit mask where bit position maps to entity
  id value. Bit value indicates that output for that entity id
  is off (0) or on (1).
 */
typedef PACK(struct)
{
  tracer_diag_pkt_hdr hdr;
  uint8 mask[32];   /**< 256-bit mask indicating entity id's output
                         is off (0) or on(1) */
} tracer_diag_resp_entity_state;

/*-------------------------------------------------------------------------*/
/**
  Request packet: Command to set entity id's output off or on.
  TRACER_DIAG_CTRL_ENTITY

  Generation of client sw events and non-client (simple) sw event are
  turned on or off.
 */
typedef PACK(struct)
{
  tracer_diag_pkt_hdr hdr;
  uint8 state;    /**< Turn output for entity id off (0) or on (1) */
  uint8 entityId; /**< Entity id (0xFF will turn all ids on */
} tracer_diag_entity_ctrl;

/**
  Response packet: Command to set entity id's output off or on.
  TRACER_DIAG_CTRL_ENTITY

  Same as tracer_diag_pkt_resp
 */

/*-------------------------------------------------------------------------*/
/**
  Request packet: Query sw event table tag.
  TRACER_DIAG_QRY_SWE_TAG

  No parameters so same as tracer_diag_pkt_hdr
 */

/**
  Response packet: Query sw event table tag.
  TRACER_DIAG_QRY_SWE_TAG

  Contains alphanumeric string identifying the event table
  associated with this build
 */
typedef PACK(struct)
{
  tracer_diag_pkt_hdr hdr;
  char tag[12]; /**< alphanumeric string of event table */
} tracer_diag_resp_swe_tag;

/*-------------------------------------------------------------------------*/
/**
  Request packet: Query for sw event id's output state.
  TRACER_DIAG_QRY_SWE_STATE

  Query made for given group of 256 event ids. For example, Group 0
  corresponds to event ids 0 to 255. Group 1 corresponds to the next
  group of 256 ids (256 through 511).
 */
typedef PACK(struct)
{
  tracer_diag_pkt_hdr hdr;
  uint16 group; /**< Group of 256 event ids */
} tracer_diag_qry_swe_state;

/**
  Response packet: Query for sw event id's output state.
  TRACER_DIAG_QRY_SWE_STATE

  Contains a 256-bit bit mask where bit position maps to event
  id value. Bit value indicates that output for that event id
  is off (0) or on (1).
 */
typedef PACK(struct)
{
  tracer_diag_pkt_hdr hdr;
  uint16 group;   /**< Group of 256 event ids */
  uint8 mask[32]; /**< 256-bit mask indicating event id's output
                       is off (0) or on (1) */
} tracer_diag_resp_swe_state;

/*-------------------------------------------------------------------------*/
/**
  Request packet: Command to set tracer event id's output on or off.
  TRACER_DIAG_CTRL_SWE

  Generation of simple sw events with the given event id are
  turned on or off.
 */
typedef PACK(struct)
{
  tracer_diag_pkt_hdr hdr;
  uint8 state;    /**< Turn output for event id off (0) or on (1) */
  uint32 eventId; /**< Event id */
} tracer_diag_swe_ctrl;

/**
  Response packet: Command to set tracer event id's output on or
  off.
  TRACER_DIAG_CTRL_SWE

  Same as tracer_diag_pkt_resp
 */

/*-------------------------------------------------------------------------*/
/**
  Request packet: Command for RPM to configure sw events
         (enable/disable and QDSS output sink).
  TRACER_DIAG_CTRL_RPMSWE

  Enable/disable RPM sw events for QDSS output sink of memory or
  QDSS.
 */
typedef PACK(struct)
{
  tracer_diag_pkt_hdr hdr;
  uint8 sink;         /**< Output sink for rpm log (0=memory, 1=QDSS) */
  uint8 evtGroup;     /**< Event group of 64 events (0 to 15)*/
  uint64 evtBitmask;  /**< Event group's bitmask (0=off, 1=on) */
} tracer_diag_rpmswe_ctrl;

/**
  Response packet: Command for RPM to configure sw events.
  TRACER_DIAG_CTRL_RPMSWE

  Same as tracer_diag_pkt_resp
 */

/*-------------------------------------------------------------------------*/
/**
  Request packet: Command to set the system test control
  register.
  TRACER_DIAG_SET_SYSTESTCTRL

  Write to a QDSS system test control register (system init time
  measurement)
 */
typedef PACK(struct)
{
  tracer_diag_pkt_hdr hdr;
  uint32 value;         /**< Value to set system test control register */
} tracer_diag_set_systestctrl;

/**
  Response packet: Command to set the system test control
  register.
  TRACER_DIAG_SET_SYSTESTCTRL

  Same as tracer_diag_pkt_resp
 */

/*-------------------------------------------------------------------------*/
//  SW trace flow parameter (param_id)
#define TRACER_DIAG_SWT_THROTTLE_THRESHOLD 1

/*-------------------------------------------------------------------------*/
/**
  Request packet: Query for a SW trace flow control parameter.
  TRACER_DIAG_QRY_SWT_FLOW

 */
typedef PACK(struct)
{
  tracer_diag_pkt_hdr hdr;
  uint16 param_id;
  uint16 reserved;
} tracer_diag_req_qry_swt_flow;

/**
  Response packet: Query for a SW trace flow control parameter.
  TRACER_DIAG_QRY_SWT_FLOW

 */
typedef PACK(struct)
{
  tracer_diag_pkt_hdr hdr;
  uint16 param_id;
  uint16 result;
  uint32 data1;
} tracer_diag_resp_qry_swt_flow1;


/*-------------------------------------------------------------------------*/
/**
  Request packet: Command to configure a SW trace flow control parameter.
  TRACER_DIAG_CFG_SWT_FLOW

 */
typedef PACK(struct)
{
  tracer_diag_pkt_hdr hdr;
  uint16 param_id;
  uint16 reserved;
  uint8 pPayload[1];
} tracer_diag_req_cfg_swt_flow;

/**
  Response packet: Command to configure a SW trace flow control
  parameter.
  TRACER_DIAG_CFG_SWT_FLOW

 */
typedef PACK(struct)
{
  tracer_diag_pkt_hdr hdr;
  uint16 param_id;
  uint16 result;
} tracer_diag_resp_cfg_swt_flow;

// typedef PACK(struct)
// {
//   tracer_diag_pkt_hdr hdr;
//   uint16 param_id;
//   uint16 results;
//   uint8 pPayload[1];
// } tracer_diag_resp_cfg_swt_flow_ext;


/*-------------------------------------------------------------------------*/
/**
  Request packet: Command to set the throttle threshold
  Set the throttle threshold of tracer_data

  TRACER_DIAG_CFG_SWT_FLOW parameter:
    TRACER_DIAG_SWT_THROTTLE_THRESHOLD

  Sets the throttle threshold in bytes per millisecond for
  throttling the bandwidth of SW trace message (tracer_data
  API). Threshold of zero (0) indicates no throttling.
 */

typedef PACK(struct)
{
  uint32 value;            /**< Value to set the throttle threshold to */
} tracer_diag_set_swt_throttle_threshold;

/**
  Response packet: Command to set the throttle threshold.
  TRACER_DIAG_CFG_SWT_FLOW

  Same as tracer_diag_resp_cfg_swt_flow
 */

/*---------------------------------------------------------------------------
 * Function Declarations and Documentation
 * ------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------*/
/**
  Initialize the interface for using diag's packet service.
 */
void tracer_diag_init(void);

#endif /* #ifndef TRACER_DIAG_H */

