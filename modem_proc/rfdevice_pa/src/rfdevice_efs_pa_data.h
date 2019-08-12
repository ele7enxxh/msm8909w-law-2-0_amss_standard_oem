#ifndef RFDEVICE_EFS_PA_DATA
#define RFDEVICE_EFS_PA_DATA


#ifdef __cplusplus
extern "C" {
#endif

  /*!
    @file
    rfdevice_efs_pa_data.h

    @brief
    Data settings for EFS based 3rd party pa devices

  */

  /*==============================================================================

    Copyright (c) 2014 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_pa/src/rfdevice_efs_pa_data.h#1 $
  $Author: mplcsds1 $

  when       who     what, where, why
  --------   ---     -------------------------------------------------------------
  06/24/14   sn      Initial creation
  ==============================================================================*/

  /*=============================================================================
                             INCLUDE FILES
  =============================================================================*/
#include "comdef.h"

#include "rfdevice_pa_data.h"

  typedef struct
  {
    uint16 mfg_id;
    uint8 prd_id;
    uint8 prd_rev;
    uint8 max_ports;
    uint8 num_pa_ranges;
  } dev_info_s;
  typedef struct
  {
    rfdevice_pa_reg_settings_type cfg;
    rfdevice_pa_reg_settings_type bias;
    rfdevice_pa_reg_settings_type range;
    rfdevice_pa_reg_settings_type on;
    rfdevice_pa_reg_settings_type off;
    rfdevice_pa_reg_settings_type trigger;
  } pa_data_s;

  typedef struct
  {
    dev_info_s device_info;
    pa_data_s pa_data;
  } efs_pa_s;


  class rfdevice_efs_pa_data:public rfdevice_pa_data
  {
  public:
    static rfdevice_pa_data * get_instance(uint8* data);
    boolean settings_data_get( rfdevice_pa_cfg_params_type *cfg,
                               rfdevice_pa_reg_settings_type *settings);
    boolean device_info_get( rfdevice_pa_info_type *pa_info );
    ~rfdevice_efs_pa_data(void);  /*  Destructor  */
  protected:
    rfdevice_efs_pa_data(uint8* data);  /*  Constructor  */

  private:

    efs_pa_s efs_pa;

    uint8* efs_pa_ptr;

  };



#ifdef __cplusplus
}
#endif



#endif

