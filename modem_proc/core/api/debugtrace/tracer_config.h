#ifndef TRACER_CONFIG_H
#define TRACER_CONFIG_H
/** =========================================================================
  @file tracer_config.h
  @brief QUALCOMM Debug Subsystem (QDSS) Tracer configuration

               Copyright (c) 2012 Qualcomm Technologies Incorporated.
               All rights reserved.
               Qualcomm Confidential and Proprietary
===========================================================================*/
/* $Header: //components/rel/core.mpss/3.5.c12.3/api/debugtrace/tracer_config.h#1 $ */

/*---------------------------------------------------------------------------
 * Include Files
 * ------------------------------------------------------------------------*/
#include "tracer.h"

/*---------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ------------------------------------------------------------------------*/
/**
  @brief Return values for status control API calls.

 MSB indicates 0=Successful, 1=Failure
 Other bits: On success, additional info on tracer state.
             On failure, reason for failure.
 Used in the "Result" field of diag pkt interface.
 See tracer_cmdresp_t
 */
#define tracer_cmdresp_t uint8

#define TRACER_CMDRESP_SUCCESS         0  /**< Command successfully
                                               completed. */
#define TRACER_CMDRESP_S_UNCHANGED     1  /**< Command redundant. Already
                                               configured as requested. */
#define TRACER_CMDRESP_S_NO_OUTPUT     2  /**< Command completed but because
                                               of other settings no output
                                               generated. */

#define TRACER_CMDRESP_FAIL         0x80  /**< Command failed. */
#define TRACER_CMDRESP_F_AVAIL      0x81  /**< QDSS not available or cannot
                                               turn QDSS (clock) on */
#define TRACER_CMDRESP_F_INIT       0x82  /**< Tracer not initialized or
                                               unable to initialize */
#define TRACER_CMDRESP_F_INVALID    0x83  /**< ID requested is unknown. */

/*---------------------------------------------------------------------------
 * Type Declarations
 * ------------------------------------------------------------------------*/
/**
  @brief Values for controlling all tracer outputs.
 */
typedef enum
{
   TRACER_OUTPUT_DISABLE   = 0,  /**< Disable tracer outputs. */
   TRACER_OUTPUT_ENABLE    = 1   /**< Enable tracer outputs. */

} tracer_op_state_t;

/**
  @brief Values for controlling tracer output by entity id.
 */
typedef enum
{
   TRACER_ENTITY_OFF   = 0,  /**< Do not generate trace outputs for entity. */
   TRACER_ENTITY_ON    = 1   /**< Generate trace outputs for entity. */
} tracer_entity_state_t;

/**
  @brief Values for controlling tracer output by event id.
 */
typedef enum
{
   TRACER_EVENT_OFF = 0,  /**< Do not generate trace outputs for sw event. */
   TRACER_EVENT_ON  = 1   /**< Generate trace outputs for sw event. */
} tracer_event_state_t;

/*---------------------------------------------------------------------------
 * Function Declarations and Documentation
 * ------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------*/
/**
  @brief Enable or disable tracer from generating trace output.

  When trace output is enabled, all trace requests for this process are
  processed and trace output is generated.

  When trace output is disabled, all trace requests for this process are
  silently discarded and no trace output is generated. This is the default.

  Actual trace output also depends on tracer being initialized as well as
  outputs for tracer, entity, and event being enabled.

  @param state [in]: State to set the tracer output configuration
     - TRACER_OUTPUT_ENABLE: All trace output requests processed.
     - TRACER_OUTPUT_DISABLE: All trace output requests discarded.

  @return TRACER_CMDRESP_...
 */
tracer_cmdresp_t tracer_cfg_op_ctrl(tracer_op_state_t state);

/*-------------------------------------------------------------------------*/
/**
  @brief Set tracer to silently discarded (disabled) or generated
  (enabled) of clients associated with the specified entity.

  When disabled, any tracing requests from clients with the specified
  entity are silently discarded  and no trace output is generated.

  Actual trace output also depends on tracer being initialized as well as
  outputs for tracer, entity, and event being enabled.

  Only one entity can be set at a time to one state. For setting
  multiple entities and/or multiple states consider writing
  directly to the variable tracer_config.entity_op_ctrl.

  @param eid [in]: Identifier of the entity to be configured.
  @param state [in]: State to set trace output for the give eid.
     - TRACER_ENTITY_ON: Requests associated with entity are processed.
     - TRACER_ENTITY_OFF: Requests  associated with entity are discarded.

  @return TRACER_CMDRESP_...
 */
tracer_cmdresp_t tracer_cfg_entity_ctrl(tracer_ost_entity_id_enum_t eid,
                                    tracer_entity_state_t state);

/*-------------------------------------------------------------------------*/
/**
  @brief Set tracer to silently discarded (disabled) or generated
  (enabled) output for all entity ids.

  Actual trace output also depends on tracer being initialized as well as
  outputs for tracer, entity, and event being enabled.

  @param state [in]: State to set trace output for the all entities.
     - TRACER_ENTITY_ON: Requests associated with any entity are processed.
     - TRACER_ENTITY_OFF: Requests  associated with any entity are discarded.

  @return TRACER_CMDRESP_...
 */
tracer_cmdresp_t tracer_cfg_entity_ctrl_all(tracer_entity_state_t state);

/*-------------------------------------------------------------------------*/
/**
  @brief Set tracer to silently discarded (disabled) or generated
  (enabled) output for the specified sw event.

  When disabled, any sw event requests for the event id are silently
  discarded and no trace output is generated.

  Actual trace output also depends on tracer being initialized as well as
  outputs for tracer, entity, and event being enabled.

  Only one event id can be set at a time to one state.

  @param event_id [in]: Identifier of the sw event to be configured.
  @param state [in]: State to set output for the given event_id.
     - TRACER_EVENT_ON: Requests for the event_id are processed.
     - TRACER_EVENT_OFF: Requests for the event_id are discarded.

  @return TRACER_CMDRESP_...
 */
tracer_cmdresp_t tracer_event_ctrl(tracer_event_id_t event_id,
                                  tracer_event_state_t state);

/*-------------------------------------------------------------------------*/
/**
  @brief Set tracer to silently discarded (disabled) or generated
  (enabled) output for the all sw event.

  Actual trace output also depends on tracer being initialized as well as
  outputs for tracer, entity, and event being enabled.

  @param state [in]: State to set output for all sw events.
     - TRACER_EVENT_ON: Requests for all SW events are processed.
     - TRACER_EVENT_OFF: Requests for all SW events are discarded.

  @return TRACER_CMDRESP_...
 */
tracer_cmdresp_t tracer_event_ctrl_all(tracer_event_state_t state);

#endif /* #ifndef TRACER_CONFIG_H */

