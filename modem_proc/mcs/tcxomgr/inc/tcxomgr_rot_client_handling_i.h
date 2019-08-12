#ifndef TCXOMGR_ROT_CLIENT_HANDLING_I_H
#define TCXOMGR_ROT_CLIENT_HANDLING_I_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*=

                           T C X O    M A N A G E R

                           TCXO Manager Header File

GENERAL DESCRIPTION

  This module maintains the internal definitions for the txcomgr subsystem

EXTERNALIZED FUNCTIONS


REGIONAL FUNCTIONS

  None


INITIALIZATION AND SEQUENCING REQUIREMENTS


  Copyright (c) 2013 Qualcomm Technologies Incorporated.
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

=*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*=



==============================================================================

                           EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/tcxomgr/inc/tcxomgr_rot_client_handling_i.h#1 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when         who     what, where, why
--------     ---     ----------------------------------------------------------------
10/04/13     mn      Support to save and retrieve subscription and system
                     RGS to/from EFS.
09/12/13     mn      Initial version: New Rpush API to support multi-SIM targets.

============================================================================*/



/*============================================================================

                           INCLUDE FILES FOR MODULE

============================================================================*/

#include "mcs_variation.h"
#include "customer.h"

/*============================================================================

                   DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, typesdefs,
and other items needed by this module.

============================================================================*/

/*---------------------------------------------------------------------------
  TCXO Manager's ROT Clients
---------------------------------------------------------------------------*/
typedef enum
{
  TCXOMGR_ROT_CLIENT_GPS,
  TCXOMGR_ROT_CLIENT_GSM,
  TCXOMGR_ROT_CLIENT_WCDMA,
  TCXOMGR_ROT_CLIENT_CDMA_1X,
  TCXOMGR_ROT_CLIENT_CDMA_HDR,
  TCXOMGR_ROT_CLIENT_LTE,
  TCXOMGR_ROT_CLIENT_TDSCDMA,
  TCXOMGR_ROT_CLIENT_FREQ_EST,
  TCXOMGR_ROT_CLIENT_IFC,
  TCXOMGR_ROT_CLIENT_MAX
} tcxomgr_rot_client_enum_type;

/*---------------------------------------------------------------------------
  TCXO Manager's Rpush callback client information
---------------------------------------------------------------------------*/

typedef struct
{
  /* Function pointer to be used for callback */
  tcxomgr_rpush_cb_func_type func_ptr;

} tcxomgr_rpush_cb_client_info_type;

/*---------------------------------------------------------------------------
  TCXO Manager's Rpush callback client information
---------------------------------------------------------------------------*/

typedef struct
{
  /* Function pointer to be used for callback */
  tcxomgr_rpush_mode_cb_func_type func_ptr;

} tcxomgr_rpush_mode_cb_client_info_type;

/*---------------------------------------------------------------------------
  TCXO Manager's Rpush client information
---------------------------------------------------------------------------*/
typedef struct
{
  /* Denotes the current Rpush mode */
  tcxomgr_rpush_req_mode_enum     rpush_mode;

  /* The Rpush mask per AS_ID */
  uint32                          rpush_mask[TCXOMGR_AS_ID_MAX];

  /* Structure holding information about clients needing
     Rpush callbacks. */
  tcxomgr_rpush_cb_client_info_type       \
          rpush_cb_clients[TCXOMGR_AS_ID_MAX][TCXOMGR_ROT_CLIENT_MAX];

  /* Structure holding information about clients needing
     Rpush mode callbacks. */
  tcxomgr_rpush_mode_cb_client_info_type  \
          rpush_mode_cb_clients[TCXOMGR_AS_ID_MAX][TCXOMGR_ROT_CLIENT_MAX];

} tcxomgr_rpush_handling_info_type;

/*---------------------------------------------------------------------------
  TCXO Manager's RGS handling information
---------------------------------------------------------------------------*/
typedef struct
{
  /* Data structure to hold System RGS value */
  tcxomgr_rgs_struct_type          system_rgs;

  /* Data structure to hold RGS values per client technology */
  tcxomgr_rgs_struct_type          client_rgs[TCXOMGR_AS_ID_MAX][TCXOMGR_ROT_CLIENT_MAX];

  /* Data structure to hold the client Id with the latest RGS for each AS_ID */
  tcxomgr_rot_client_enum_type     subscription_rgs[TCXOMGR_AS_ID_MAX];

} tcxomgr_rgs_handling_info_type;

/*---------------------------------------------------------------------------
  TCXO Manager's structure to store RGS in EFS
---------------------------------------------------------------------------*/

typedef PACKED struct
{
  /* Temperature measurement when VCO & ROT values were captured */
  tcxomgr_temp_type               temp;

  /* Time when most recent good system values were captured */
  uint32                          time;

  /* Time when most recent good system values were captured (in msec) */
  uint32                          time_in_msec;

  /* VCTCXO accumulator of the most recent good system */
  tcxomgr_vco_type                vco;

  /* VCTCXO accumulator of the most recent good system */
  tcxomgr_rotator_type            rot;

  /* System that set the most recent good system values */
  tcxomgr_client_id_type          id;
}
tcxomgr_efs_rgs_struct_type;

/*---------------------------------------------------------------------------
  TCXO Manager's structure to represent RGS data in EFS
---------------------------------------------------------------------------*/
typedef PACKED struct
{
  /* RGS data version */
  uint32                           version;

  /* Data structure to hold System RGS value */
  tcxomgr_efs_rgs_struct_type      system_rgs;

  /* Data structure to hold subscription RGS data */
  tcxomgr_efs_rgs_struct_type      subscription_rgs[TCXOMGR_AS_ID_MAX];

} tcxomgr_efs_rgs_data_type;

/*---------------------------------------------------------------------------
  TCXO Manager's Rotator clients handling information
---------------------------------------------------------------------------*/
typedef struct
{
  /* Data structure to hold the information for RGS handling */
  tcxomgr_rgs_handling_info_type   rgs_handling_info;

  /* Data structure to hold the information for clients registered for Rpush callbacks. */
  tcxomgr_rpush_handling_info_type rpush_handling_info;
} tcxomgr_rot_handling_data_type;


/*============================================================================

                             DEBUG MESSAGE MACROS

============================================================================*/

/*============================================================================

                            FUNCTION DECLARATIONS

============================================================================*/

/*============================================================================

FUNCTION TCXOMGR_GET_STORED_VCO_FROM_RGS

DESCRIPTION
  This API resets the RGS values from the TCXOMGR data structure.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
tcxomgr_vco_info_type tcxomgr_get_stored_vco_from_rgs
(
  uint32 time,
  tcxomgr_temp_set_type temp_struct,
  tcxomgr_log_rgs_retrieval_data_struct_type* log_ptr,
  tcxomgr_rgs_struct_type*                    rgs_ptr
);

/*===========================================================================

FUNCTION TCXOMGR_EXTRAPOLATE_RGS

DESCRIPTION    Extrapolates the temperature stored in the RGS values
               if required.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
void tcxomgr_extrapolate_rgs
(
  tcxomgr_temp_set_type temp,
  uint32                read_time
);

/*============================================================================

FUNCTION TCXOMGR_INIT_ROT_CLIENT_DATA

DESCRIPTION
  This API initializes the rotator client data

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
void tcxomgr_init_rot_client_data( void );

/*============================================================================

FUNCTION TCXOMGR_GET_SYSTEM_RGS

DESCRIPTION
  This function retrieves the System RGS values.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/

void tcxomgr_get_system_rgs(tcxomgr_rgs_struct_type* system_rgs);

/*============================================================================

FUNCTION TCXOMGR_ROTATOR_PUSH_CMD_HANDLER

DESCRIPTION
  This function handles Rpush data command for TCXOMGR task. As a part of the
  handling, this function will iterate over the clients registered for Rpush
  report callbacks and invoke their callback functions.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
void tcxomgr_rotator_push_cmd_handler
(
  tcxomgr_rpush_data_type* rpush_data
);

/*============================================================================

FUNCTION TCXOMGR_RESET_ALL_RGS_VALUES

DESCRIPTION
  This API resets the central and client RGS values from the TCXOMGR
  data structure.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
void tcxomgr_reset_all_rgs_values( boolean flush_to_efs );

/*============================================================================

FUNCTION TCXOMGR_EFS_RGS_DATA_WRITE

DESCRIPTION
  This function writes RGS data from EFS.

DEPENDENCIES
  tcxomgr.nv

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
void tcxomgr_efs_rgs_data_write( void );

#endif /* !TCXOMGR_ROT_CLIENT_HANDLING_I_H */

