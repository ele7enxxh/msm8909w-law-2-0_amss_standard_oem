#ifndef QDSS_CONTROL_H
#define QDSS_CONTROL_H

/*=============================================================================

FILE:         qdss_control.h

DESCRIPTION:  QDSS control API for debug agent

================================================================================
            Copyright (c) 2013 Qualcomm Technologies, Inc.
                         All Rights Reserved.
          Qualcomm Technologies Proprietary and Confidential
==============================================================================*/

#include "DALStdDef.h"

#define QDSS_CONTROL_RVAL_SUCCESS      0
#define QDSS_CONTROL_RVAL_UNKNOWN_ERR  1

/*=========================================================================
  Trace Sink
 =========================================================================*/

/**
  @brief  Get the current trace sink

  @param               sinkid [out ] :  Current sink id is returned here

  @return 0 if successful, error code otherwise
 */
int qdss_control_get_sink(uint8 *sinkid);
/*-------------------------------------------------------------------------*/

/**
  @brief Set the trace sink

  @param               sinkid [in ] :  New trace sink

  @return 0 if successful, error code otherwise
 */
int qdss_control_set_sink(uint8 sinkid);
/*-------------------------------------------------------------------------*/

/**
  @brief  Return a list of supported trace sink ids

  @param          sinkid_list [out]  :  Pointer to a buffer where supported
                                        sinkids will be listed
  @param            list_size [in ]  :  Size of the input buffer
  @param               filled [out ] :  Number of trace sink ids returned

  @return 0 if successful, error code otherwise
 */
int qdss_control_get_sink_list(uint8 *sinkid_list, int list_size, int *filled);

/*=========================================================================
  ETM
 =========================================================================*/

/**
  @brief Get the current state of ETM on this core

  @param                state [out ] :  ETM state  0 (disabeld) or 1 (enabled)

  @return 0 if successful, error code otherwise
 */
int qdss_control_get_etm(uint8 *state);

/*-------------------------------------------------------------------------*/

/**
  @brief  Enable or disable ETM on this CPU

  @param                state [in ] :  1 (enable), 0 (disable)

  @return 0 if successful, error code otherwise
 */
int qdss_control_set_etm(uint8 state);

/*-------------------------------------------------------------------------*/

/**
  @brief  Set an ETM register

  @param                 addr [in ] :  addres offset for the register to set
  @param                  val [in ] :  value to be set

  @return 0 if successful, error code otherwise
 */
int qdss_control_set_etm_reg(unsigned int addr, unsigned int val);

/*========================================================================
 Funnel
=========================================================================*/

/**
  @brief Get the state of a funnel

  @param                name [in ]  :  Trace funnel name
                                       See qdss_tfunnel.h for supported names
  @param               state [out ] :  Current state of the funnel is returned here

  @return 0 if successful, error code otherwise
 */
int qdss_control_get_funnel(const char *name, uint8 *state);

/*-------------------------------------------------------------------------*/

/**
  @brief  Set the state of a funnel

  @param                 name [in ] :  Trace funnel name
  @param                state [in ] :   Enable(1) or disable (0)

  @return 0 if successful, error code otherwise
 */
int qdss_control_set_funnel(const char *name, uint8 state);

/*=========================================================================
 STM
 =========================================================================*/

/**
  @brief   Get the state of STM

  @param                state [in ] :  STM state is returned here

  @return 0 if successful, error code otherwise
 */
int qdss_control_get_stm(uint8 *state);

/*-------------------------------------------------------------------------*/

/**
  @brief Set STM state

  @param                state [in ] :  1 (enable) or  0 (disable)

  @return 0 if successful, error code otherwise
 */
int qdss_control_set_stm(uint8 state);

/*=========================================================================
 HWE
 =========================================================================*/
/*-------------------------------------------------------------------------*/

/**
  @brief Get the state of HW events

  @param                state [in ] :  HW events state is returned here.
  1 (HW events enabled)  0 HW events disabled

  @return 0 if successful, error code otherwise
 */
int qdss_control_get_hwe(uint8 *state);

/*-------------------------------------------------------------------------*/

/**
  @brief Set the state of HW events

  @param                state [in ] :  1 (enable) or 0 (disable)

  @return 0 if successful, error code otherwise
 */
int qdss_control_set_hwe(uint8 state);

/*-------------------------------------------------------------------------*/

/**
  @brief Set a HW event register

  @param                 addr [in ] :  address of register to set
  @param                  val [in ] :  value of register to set

  @return 0 if successful, error code otherwise
 */
int qdss_control_set_hwe_reg(unsigned long addr, unsigned long val);



/*-------------------------------------------------------------------------*/
/**
  @brief Set mask for HW event detection

  @param                 mask [in ] :  32-bit mask to be set

  @return 0 if successful, error code otherwise
 */
int qdss_control_set_hwe_detect_mask(unsigned long mask);

/*-------------------------------------------------------------------------*/

/**
  @brief Set mask for HW event trigger

  @param                 mask [in ] :  32-bit mask to be set

  @return 0 if successful, error code otherwise
 */

int qdss_control_set_hwe_trigger_mask(unsigned long mask);


/**
  @brief Get the current state of ETM on RPM

  @param    state [out ]            :  ETM state  0 (disabled)
                  or 1 (enabled)

  @return 0 if successful, error code otherwise
 */
int qdss_control_get_etm_rpm(uint8 *state);

/*-------------------------------------------------------------------------*/

/**
  @brief  Enable or disable ETM on RPM

  @param    state [in ]             :  1 (enable), 0 (disable)

  @return 0 if successful, error code otherwise
 */
int qdss_control_set_etm_rpm(uint8 state);

#endif //QDSS_CONTROL_H

