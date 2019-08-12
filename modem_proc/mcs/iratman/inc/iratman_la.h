#ifndef IRATMAN_LA_H
#define IRATMAN_LA_H

/*!
  @file   iratman_la.h

  @brief  IRATMAN LTE Availability Module

  @details
  This module is used to track LTE Availablity Data during power on and to keep
  this data persistent over power cycles. The information is used to Dynamically
  calculate a value for Tbackoff.

*/

/*===========================================================================

  Copyright (c) 2013 - 2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/iratman/inc/iratman_la.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/05/14   cjb     Avoid accessing de-allocated memory and code clean-up
12/06/13   bph     FR3839 - dynamic tbackoff

===========================================================================*/

#include "iratman.h"

#define IRATMAN_LA_MAX_NODES         1024

/* scaling factor of 1.4, but to avoid FP, x10, so divide by ten when used */
#define IRATMAN_LA_FAIL_MULT_FACTOR_10 14
#define IRATMAN_LA_SUCC_MULT_FACTOR     2
#define IRATMAN_LA_UK_MULT_FACTOR       2
#define IRATMAN_LA_MAX_ATTEMPTS        10
#define IRATMAN_LA_THRESH_LTE_CLOSE      .5 /* dB use shift right to avoid FP */

/* Struct to contain all LTE Availability node data */
typedef struct
{
  uint32  key;
  uint32  timestamp;
  uint16  last_tbackoff_dur_s;
  uint8   failure_count;
  boolean is_available;
} iratman_la_node_data_type;

/* LTE Availability Node Struct, contains data and ptr for DLL and RBT */
typedef struct iratman_la_node
{
  struct iratman_la_node    *left;
  struct iratman_la_node    *right;
  struct iratman_la_node    *next;
  struct iratman_la_node    *prev;
  iratman_la_node_data_type  data;
} iratman_la_node_type;

/* LTE Availability Data Structure */
typedef struct iratman_la
{
  iratman_la_node_type *root;
  iratman_la_node_type *head;
  iratman_la_node_type *tail;
  iratman_la_node_type *node_memory;
  uint32                node_count;
  boolean               initialized;
} iratman_la_type;

/* forward declarations - required input for API function */
struct iratman_rat_node_type;
enum iratman_lte_meas_result_type;

/*===========================================================================

  FUNCTION:  IRATMAN_LA_INIT

===========================================================================*/
/*!
  @brief
  Initialize the IRATMAN LTE Availability Module

  @details
    This function is called from iratman_init to setup the resources for the
    LTE Availability Module. Initialization includes allocating memory for the
    LTE Availability data struct and reading the content of IRATMAN_LA_FILE_PATH
    into the data struct.

  @note
    None

  @return
    void

*/
/*=========================================================================*/
void iratman_la_init
(
  iratman_la_type *la
);

/*===========================================================================

  FUNCTION:  IRATMAN_LA_DEINIT

===========================================================================*/
/*!
  @brief
  De-initialize the IRATMAN LTE Availability Module

  @details
    This function is called from iratman_flush_persistent_data to flush the LTE
    Availability data struct to a file and to free the resources from the LTE
    Availability Module.

  @note
    None

  @return
    void

*/
/*=========================================================================*/
void iratman_la_deinit
(
  iratman_la_type *la
);

/*===========================================================================

  FUNCTION:  IRATMAN_LA_REPORT_EVENT

===========================================================================*/
/*!
  @brief
  Used to report a measurement event to the LTE Availability Module.

  @details
    This function is called by iratman when a measure fails to meet reselection
    criteria, or when an iratman node has passes reselection.

  @note
    None

  @return
    void

*/
/*=========================================================================*/
void iratman_la_report_event
(
  iratman_la_type                   *la,
  enum iratman_lte_meas_result_type  result,
  struct iratman_rat_node_type      *rat_node,
  iratman_rat_e_type                 home_rat
);

#endif /* IRATMAN_LA_H */
