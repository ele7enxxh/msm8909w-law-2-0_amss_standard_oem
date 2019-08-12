/*!
   @file
   rfc_gnss.h

   @brief
   This file contains all the gnss mode specific RFC class definition

*/

/*===========================================================================

Copyright (c) 2008 - 2013 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$DateTime: 2016/12/13 07:59:49 $ $Author: mplcsds1 $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_gnss/api/rfc_gnss.h#1 $

when       who     what, where, why
-------------------------------------------------------------------------------
05/06/13   sr/pv   Triton Support for multiple transceivers for GPS.
10/30/12   sr      changes create tech rfc object if the tech is supported by 
                   the specific rf-card.
07/17/12   jr       Added support for non-ELNA configuration.
05/23/12   sr       Move the manage_vregs() to base-class from rf-card specific files.
05/18/12   sr       RFC-EFS code clean-up changes.
05/08/12   sr       Added binary RFC support for GNSS.  
03/23/12   sr       RFC Clean-up.  
05/26/11   dbz      Initial Version 
============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
#ifdef __cplusplus
extern "C" {
#endif
#include "rfc_card.h"
#include "rfc_card_gnss.h"
#include "rfa.h"
#include "rfc_class.h"

/*----------------------------------------------------------------------------*/
/*!
  @name GNSS interface for concrete RF card classes.

  @brief Concrete RF card container classes must derive from this class in order
         to support GNSS operation.
          
*/
/*! @{ */

/*!
  @brief
  Interface definition for RF cards supporting GNSS operation.
*/
class rfc_gnss : public rfa
{
public:
  static rfc_gnss * get_instance(void);
  static rfc_gnss * create_instance(void);

  virtual rfc_gnss_attribute_type *get_mode_config(rf_path_enum_type path);

  virtual void config_mode_data(rf_path_enum_type path);

  virtual void command_dispatch(rf_path_enum_type path, int32 cmd, void* data);

  virtual void manage_vregs(rf_path_enum_type path,
                            rfcom_mode_enum_type mode,
                            rfm_path_state state);

  virtual void get_LNA_config(rfc_gnss_lna_config_type *lna);

  rfdevice_class *get_rf_device_obj(rfdevice_type_enum_type dev_type, uint8 instance);

  /*! destructor */
  virtual ~rfc_gnss();

protected:
  /*! hide constructor to disallow instantiation except when 
    implementing this interface through derivation */
  rfc_gnss(void);

  static rfc_gnss *rfc_gnss_ptr;

private:

  void init_rf_devices(void);
  rfdevice_class *gnss_trx_devices[RFC_MAX_DEVICE_INSTANCES_PER_BAND];


};

#ifdef __cplusplus
}
#endif
