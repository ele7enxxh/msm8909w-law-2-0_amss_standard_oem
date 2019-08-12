#ifndef RFCOMMON_ASDIV_TUNER_MANAGER_H
#define RFCOMMON_ASDIV_TUNER_MANAGER_H
/*!
  @file
  rfcommon_asdiv_tuner_manager.h
  
  @brief
  Declaration of antenna switch diversity tuner manager interface class
  This header file contains all type common logic and implementation related to
  antenna switch diversity tuner manager.
  
*/

/*===========================================================================

  Copyright (c) 2013 Qualcomm Technologies Incorporated. All Rights Reserved

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

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/rf/atuner/inc/rfcommon_asdiv_tuner_manager.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
11/18/14   dyc     Support updating rfc_asdiv_config_info table from RFC info
03/14/14   kg      Port changes from Dime
03/10/14   as      Added callback function as part of antenna switch
03/10/14   as      Deprecated asdiv_obj from get_switch_script API
02/18/14   ndb     Deprecate the API set_tuner_state() in ASD (Moved to Tuner manager )
02/11/14   as/sr   Re-design asdiv_tuner to support multiple RF configurations
01/16/14   vs      ASDIV featurization merge from Dime 2.1r6
12/18/13   vs      Updated API to read tuner manager instance and do switching
01/15/14   vs      Added support for TP tuner asdiv APIs
01/02/14   as      Handled ASDiv position correctly while updating init state
11/29/13   yb      Removed low power API
10/28/13   vb      Support for force_detune()
09/27/13   yb      Added tune_code_override API
08/29/13   pl      Update asdiv tuner manager creation interface
08/14/13   pl      Added support for generation Tune switch scripts
08/08/13   pl      Added support to create AsDiv Tuner Manager
08/06/13   vb      Optimized update_tune_code()
07/31/13   pl      Fix ineterface type mismatch
07/26/13   as      init version.
============================================================================*/

#include "rfdevice_cmn_type_defs.h"
#include "rfa.h"
#include "rfcommon_atuner_intf.h"
#include "rfdevice_class.h"
#include "rfcommon_atuner_manager_intf.h"
#include "rfcommon_asdiv_manager.h"
#include "rfc_common.h"


#if defined(FEATURE_RF_ASDIV) || defined(FEATURE_RF_HAS_TPTUNER_SWITCH)

  
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Interface antenna switch diversity tuner manager class that contains all
  the APIs for antenna tuner.
 
  @details
*/
class rfcommon_asdiv_tuner_manager : public rfa
{
public:

  /* Constructor for asdiv tuner manager */
  rfcommon_asdiv_tuner_manager();

  /* Destructor for asdiv tuner manager */
  ~rfcommon_asdiv_tuner_manager();
  

  /*Init from the rfdevice_class*/
  void init();


  /* return a switch script to switch to given position*/
  boolean get_switch_script( rfcommon_asdiv_position_type position,
                      rf_buffer_intf *script_buffer,
                           rf_device_execution_type dev_action,
                             int16 script_timing);


  /* return Devices to configure based on the ASDiv Switch position for specific Antenna*/
  rfdevice_class** get_asdiv_device_list(uint8 antenna,
                                         rfdevice_type_enum_type dev_type, 
                                         rfdevice_class **dev_list);
  /* check for dummy tuner need */
  boolean is_dummy_tuner_needed(uint8 antenna);

  /* set/reset force_update_tune_code_flag*/
  void override_update_tune_code_flag(boolean flag_val);

  /* process the asdiv config info from RFC */
  boolean process_rfc_asdiv_config_info( void );

  /* update the asdiv config table */
  boolean update_rfc_asdiv_config_info( rfc_req_type req,
                                        uint32 ant_path,
                                        rfdevice_type_enum_type device,
                                        uint32 instance);
protected:

  /* asdiv config information table */
  rfc_asdiv_config_info_type rfc_asd_config_info;
  rfc_asdiv_config_info_type *rfc_asd_config_info_ptr;

private:

  rfc_asdiv_config_info_type *get_rfc_asdiv_config_info_ptr( void );

  boolean populate_asdiv_tuner_port_objects(uint8 antenna, rfc_asdiv_dev_info_type *devices);

  rfcommon_atuner_intf *asdiv_tuner_port_objects[RFC_MAX_ANTENNA][ATUNER_CA_PORT_MAX][RFC_ASDIV_MAX_DEVICES_PER_ANT_PATH];

  uint8 antenna_pair[RFC_MAX_ANTENNA];

};


/* create the ASD Tuner manager object */
rfdevice_class * create_rfdevice_asd_tuner_manager(void);

/*----------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C"
{
#endif


boolean
rfcommon_asdiv_tuner_get_switch_script
(
   rf_device_execution_type dev_action,
   rf_buffer_intf *script_buffer,
   int16 script_timing,
   rfcommon_asdiv_position_type position
);

void
rfcommon_asdiv_tuner_update_tuner_code
( 
  rfcommon_asdiv_update_tuner_type update_tuner_cb
);


#ifdef __cplusplus
}
#endif

#endif /*FEATURE_RF_ASDIV*/

#endif /* RFCOMMON_ASDIV_TUNER_MANAGER_H */

