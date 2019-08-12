/*!
  @file 
  rf_hal_common.h

  @brief
  Header file for common RF HAL Bus functions

*/

/*============================================================================== 
   
  Copyright (c) 2012-2013 Qualcomm Technologies Incorporated. All Rights Reserved 
   
  Qualcomm Proprietary 
   
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
   
==============================================================================*/ 

/*==============================================================================
   
                        EDIT HISTORY FOR MODULE 
   
This section contains comments describing changes made to the module. 
Notice that changes are listed in reverse chronological order. 
 
$DateTime: 2016/12/13 07:59:54 $ $Author: mplcsds1 $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rflm/api/ccs/rf_hal_common.h#1 $ 
   
when       who     what, where, why 
--------   ---     -------------------------------------------------------------
03/10/14   vws     Added GPS as mission mode client
10/16/13   ra      Support CCS portability
05/03/13   tws     Add GSM2 HAL client ID
03/27/13   php     Add LTE tech to RF_HAL_CLIENT_MISSION_MODE_MASK 
03/15/13   ec      Add GSM tech to RF_HAL_CLIENT_MISSION_MODE_MASK 
03/09/13   jyu     Added TDSCDMA in mission mode mask
03/01/13   zhw     Add 1x/EVDO in mission mode mask
02/25/13   dw      Add FTM in mission mode mask
02/08/13   kcj     Apply WCDMA to MCPM mask.
02/05/13   jfc     Add non-mission mode support for RFCMD voting 
12/10/12   jfc     Refactor code to allow clients to send events to TQ 
11/29/12   jfc     Update doc for rf_hal_issue_script_generic() 
11/19/12   aki     Added 'rf_hal_bus_deinit' function
11/16/12   jfc     Add error has a possible TQ return type  
11/16/12   jfc     Finalize the on-demand TQ allocation and use 
11/13/12   pl      Fix warnings -- change issue_script_generic to accept a const resource script pointer
11/05/12   jfc     Don't count multiple votes of the same handle for RFCMD app voter 
11/02/12   jfc     Clean up old types related to rf_hal_enable_rfcmd_app
11/01/12   jfc     Revamp the RF HAL (RFCMD app) voting 
09/28/12   jfc     Allow events to be sent to on-demand task queues 
09/27/12   jfc     Add API to request non-dedicated TQ 
09/20/12   sty     Split enum for CDMA into 1x and EXVO - need separate votes 
                   for SV scenarios to work
09/17/12   jfc     Enable voting for RFCMD 
08/24/12   jfc     Export rf_hal_enable_rfcmd_app() in header
08/05/12   jfc     Add includes
06/25/12   jfc     Add rf_hal_bus_init
06/22/12   jfc     Add generic script interface header
01/09/12   jfc     Initial version
==============================================================================*/ 


#ifndef RF_HAL_COMMON_H
#define RF_HAL_COMMON_H

#include "rf_hal_bus_types.h"
#include "rf_hal_ccs.h"
#ifdef __cplusplus
extern "C" {
#endif


#define RF_HAL_CLIENT_MASK  (0xFFFF)
#define RF_HAL_CLIENT(tech) (1<<(RF_HAL_CLIENT_##tech))
//#define RF_HAL_RFCMD_APP_CLIENT(tech) (#tech)
#define RF_HAL_RFCMD_APP_CLIENT(tech) (RF_HAL_CLIENT_##tech)

/*! @brief Mask of clients that don't need MCPM to handle their resources 
 
 @detail In mission-mode (online mode) L1 is active and there is
 no need for RF HAL toe request for resources from MCPM. This additional
 check saves the time it takes to call MPCM and have MCPM process the vote
 
 Add your client to the mask below if L1 is active when you vote for RF HAL
 
 If your client is in this list, it is assumed that clk_bus and FWS resources
 are available and no additional MPCM vote is necessary (aka mission mode)
*/
#define RF_HAL_CLIENT_MISSION_MODE_MASK \
  0x0                    | \
  RF_HAL_CLIENT(WCDMA)   | \
  RF_HAL_CLIENT(FTM)     | \
  RF_HAL_CLIENT(1X)      | \
  RF_HAL_CLIENT(EVDO)    | \
  RF_HAL_CLIENT(TDSCDMA) | \
  RF_HAL_CLIENT(GSM)     | \
  RF_HAL_CLIENT(GSM2)    | \
  RF_HAL_CLIENT(GPS)     | \
  RF_HAL_CLIENT(LTE)

/*! @brief Opaque structure for RF HAL bus client voting handle */
typedef struct _rf_hal_bus_client_handle rf_hal_bus_client_handle;






/*! @brief Issues sequence of generic transactions (of either RFFE/SSBI/GRFC)
 Boundaries are auto detected and script is broken on bus flips, read/write boundaries, etc.
 
 @detail Order is preserved but timing is not guaranteed. Note that for RFFE,
 this function minimizes the number of tasks generated by any given script
 (at the expense of a lower per transaction throughput)
 
 @param script
 Pointer to the generic script type
 
 @param settings
 Pointer to structure that will hold various settings like priority, etc.
 
 @return 
 RF_HAL_BUS_SUCCESS if successful, otherwise a non-zero enum code
*/
rf_hal_bus_result_type rf_hal_issue_script_generic
(
  const rf_hal_bus_resource_script_type* script,
  rf_hal_bus_resource_script_settings_type* settings
);

/*! @brief Initialize things for RF HAL interface
*/
boolean rf_hal_bus_init
(
  void
);

/*! @brief Deinitialize things for RF HAL interface
*/
void rf_hal_bus_deinit
(
  void
);


/*! Feature to enable multiple votes on the same handle. E.g. If enable is called three times 
  with the same handle, the vote is not removed unless there are three disables */
#undef RF_HAL_BUS_ENABLE_SUPPORT_MULTIPLE_COUNTS_PER_HANDLE

/*! @brief Vote for the RF HAL Bus interface (SBI, RFFE). Replaces old rf_hal_enable_rfcmd_app() API 
 
  @details Multiple votes with the same handle are only counted once (See feature RF_HAL_BUS_ENABLE_SUPPORT_MULTIPLE_COUNTS_PER_HANDLE)
 
  @param enable
  TRUE if voting RF HAL bus on
  FALSE if voting RF HAL bus off
 
  @param client_handle
  Pointer to rf_hal_bus_client_handle* type. Will be allocated by this function
 
  @param client_name
  Any string that identifies the calling client. Keep it short (less than 16 chars)
 
  @return TRUE if call was successful. FALSE if there was an error
  */
boolean rf_hal_bus_enable
(
  boolean enable,
  rf_hal_bus_client_handle** client_handle,
  //const char* client_name
  rf_hal_client_enum client_enum
);

/*! @brief Reserve the Command Interface Task Queue Pair
 
  @return RF_HAL_RFCMD_TQ_NONE
    If enabling, no TQ can be allocated, or if freeing TQ was freed
 
  @return RF_HAL_RFCMD_TQ_ERR
    There was an error
 
  @return RF_HAL_RFCMD_TQ_PAIR0
    If enabling, TQ pair 0 was allocated for the caller
 
  @return RF_HAL_RFCMD_TQ_PAIR1
    If enabling, TQ pair 1 was allocated for the caller
*/
rf_hal_rfcmd_tq_type rf_hal_reserve_rfcmd_tq_pair
(
  boolean enable,
  rf_hal_rfcmd_tq_enum tq,
  uint32 client
);

rf_hal_bus_result_type rf_hal_issue_event_generic
(
  rf_hal_rfcmd_tq_enum which_pair, /*! TQ pair */
  boolean force_immediate_timing /*! Make every task in this event IMMEDIATE trigger */
);

#ifdef __cplusplus
}
#endif

#endif /* RF_HAL_COMMON_H */

