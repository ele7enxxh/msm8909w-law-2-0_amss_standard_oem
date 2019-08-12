#ifndef RFCOMMON_ATUNER_MANAGER_FACTORY_H
#define RFCOMMON_ATUNER_MANAGER_FACTORY_H
/*!
  @file
  rfcommon_atuner_manager_factory.h
  
  @brief
  Declaration of antenna tuner manager factory
  This header file contains all type common logic and implementation related to
  antenna tuner (atuner) factory.
  
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/rf/atuner/inc/rfcommon_atuner_manager_factory.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
03/14/14   kg      Port from Dime
02/11/14   vb      Re-design asdiv_tuner to support multiple RF configurations
05/30/13   ndb     Clean-up the Atuner manager
05/06/13   vb      Initial version
============================================================================*/

#include "rfcommon_atuner_intf.h"

/*!
  @brief
  Enum for indicating the various type of Tuner Managers supported
  
  @detail
*/
typedef enum
{
  ATUNER_AOL_MANAGER = 0,
  ATUNER_CL_MANAGER = 1,
  ATUNER_DUMMY_MANAGER = 2,
  ATUNER_MANAGER_MAX =3
} rfcommon_atuner_manager_factory_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Antenna tuner manager factory class that creates the instance of the tuner 
  manager associated with Advanced Open Loop (AOL) and Closed Loop (CL) algorithms 
  for the associated antenna tuner device.
 
  @details
*/
class rfcommon_atuner_manager_factory : public rfdevice_class
{
public:

  /* Constructor for Atuner factory */
  rfcommon_atuner_manager_factory( void );

  /* Destructor for Atuner factory */
  ~rfcommon_atuner_manager_factory();

  /*Init from the rfdevice_class*/
  void init();

  /* Creates for Tuner manager */
  rfdevice_class *create_atuner_manager( rfcommon_atuner_manager_factory_type manager_type,
                                         rfdevice_class *tuner_dev_obj,
                                         uint32 instance_num = 0 );

protected:

private:

};

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This API provides the Port instance from the port number and the tuner
  manager object provided
*/
rfdevice_class *
rfcommon_atuner_manager_factory_get_port_instance
(
  rfdevice_class *tuner_mgr_obj,
  rfcommon_atuner_ca_port_type port_number
);

#endif /* RFCOMMON_ATUNER_MANAGER_FACTORY_H */

