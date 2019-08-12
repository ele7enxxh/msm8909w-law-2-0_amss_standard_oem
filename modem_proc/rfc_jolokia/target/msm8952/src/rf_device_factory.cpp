/*!
  @file rf_device_factory.cpp 

  @brief
  This is the rf device factory which creates all devices.



*/

/*===========================================================================

  Copyright (c) 2009 - 2015 QUALCOMM Technologies Inc. All Rights Reserved

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

===========================================================================*/


/*===========================================================================


                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfc_jolokia/target/msm8952/src/rf_device_factory.cpp#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
05/13/15   fhuo    Add support for QFE2082FC and QFE3340FC
04/28/15   dbc     Add support for QFE4320
04/16/15   ot      Remove 4905 support for TA
04/09/15   sg      Add missing QFE3320 and QFE1040 device instance creation
03/27/15   ot      related changes to disable rfdevice_wtr2955.mpss.1.0 for RUMI
03/24/15   bm      Added support for WTR2955V110 device ID
10/15/14   dr      Featurization for Tabasco
09/17/14   bm      Added support for WTR2955 
09/10/14   vr     Jolokia Rumi Changes
09/10/14   ndb     Remove the Support of QFE1520
09/02/14   vv      Wrapper api to create 3rd party logical devices.
07/18/14   dr      [RUMI] Changes for JOLOKIA RUMI bringup
06/16/14   vv      Enabling physical device creation for all devices except WTR2605,WTR1605 & TRX_HDET
07/01/14   yb     Add support to make tuner transactions non-extended via rfc
06/09/14   vv      Update physical device creation with new physical device structure for WTR,QFE15xx and QFE25xx
06/09/14   dr      Featurizing WTR2605 and qfe2320ept occurences out of JOLOKIA
05/15/14   php     Change RFFE scan to non-extended to fix RFFE scan issue for QFE2340 
04/30/14   aro     Removed QFE Devies not used for DimePM
04/24/14   lp      Ported rf_device_factory_create_asd_tuner_device changes
03/24/14   zhw     wtr4905 hdet support
03/02/14   sd      Initial revision.


===========================================================================*/

/*===========================================================================

                            INCLUDE FILES

===========================================================================*/
#include "comdef.h"
#include "rfcommon_atuner_manager_factory.h"
#include "rf_device_factory.h"
#include "rfdevice_test_device.h"
#include "rfdevice_rxtx_common_adapter.h"
#ifndef FEATURE_GNSS_ONLY_NO_WWAN
#include "rfdevice_pa_common.h"
#include "rfdevice_papm_common.h"
#include "rfdevice_asm_common.h" 
#include "rfdevice_papm_physical_device.h"
#endif
#include "rfc_class.h"
#include "DALSys.h" /* DALSYS_BusyWait() */

extern "C"
{
#include "msg.h"
#include "rfcommon_nv_mm.h"

#if defined(FEATURE_CDMA1X) || defined(FEATURE_HDR)
#if (!( defined(FEATURE_JOLOKIA_MODEM) || defined(FEATURE_TABASCO_MODEM)))
#include "wtr2605_cdma_device_intf.h"
#endif
#include "rfdevice_trx_cdma_rx_adapter.h"
#include "rfdevice_trx_cdma_tx_adapter.h"
#endif

#ifdef FEATURE_GSM 
#if (!( defined(FEATURE_JOLOKIA_MODEM) || defined(FEATURE_TABASCO_MODEM)))
#include "wtr2605_gsm_device_intf.h"
#endif
#include "rfdevice_trx_gsm_rx_adapter.h"
#include "rfdevice_trx_gsm_tx_adapter.h"
#endif

#ifdef FEATURE_WCDMA
#if (!( defined(FEATURE_JOLOKIA_MODEM) || defined(FEATURE_TABASCO_MODEM)))
#include "wtr2605_wcdma_device_intf.h"
#endif
#include "rfdevice_trx_wcdma_rx_adapter.h"
#include "rfdevice_trx_wcdma_tx_adapter.h"
#endif

#ifdef FEATURE_TDSCDMA
#if (!( defined(FEATURE_JOLOKIA_MODEM) || defined(FEATURE_TABASCO_MODEM)))
#include "wtr2605_tdscdma_device_intf.h"
#endif
#include "rfdevice_trx_tdscdma_rx_adapter.h"
#include "rfdevice_trx_tdscdma_tx_adapter.h"
#endif

#ifdef FEATURE_CGPS
#if (!( defined(FEATURE_JOLOKIA_MODEM) || defined(FEATURE_TABASCO_MODEM)))
#include "wtr2605_gnss_device_intf.h"
#endif
#endif

#ifdef FEATURE_CDMA
#if defined(FEATURE_CDMA1X) || defined(FEATURE_HDR)
#include "rfdevice_trx_cdma_rx_adapter.h"
#include "rfdevice_trx_cdma_tx_adapter.h"
#include "wtr2955_cdma_device_intf.h"
#include "wtr2955v110_cdma_device_intf.h"
#endif
#endif 

#ifdef FEATURE_GSM 
#include "wtr2955_gsm_device_intf.h"
#include "wtr2955v110_gsm_device_intf.h"
#include "rfdevice_trx_gsm_rx_adapter.h"
#include "rfdevice_trx_gsm_tx_adapter.h"
#endif 

#ifdef FEATURE_WCDMA
#include "rfdevice_trx_wcdma_rx_adapter.h"
#include "rfdevice_trx_wcdma_tx_adapter.h"
#include "wtr2955_wcdma_device_intf.h"
#include "wtr2955v110_wcdma_device_intf.h"
#endif

#ifdef FEATURE_LTE
#include "rfdevice_trx_lte_rx_adapter.h"
#include "rfdevice_trx_lte_tx_adapter.h"
#include "wtr2955_lte_device_intf.h"
#include "wtr2955v110_lte_device_intf.h"
#endif /* #ifdef FEATURE_LTE */

#ifdef FEATURE_TDSCDMA
#include "rfdevice_trx_tdscdma_rx_adapter.h"
#include "rfdevice_trx_tdscdma_tx_adapter.h"
#include "wtr2955_tdscdma_device_intf.h"
#include "wtr2955v110_tdscdma_device_intf.h"
#endif 
}
#ifdef FEATURE_CGPS
#include "wtr2955_gnss_device_intf.h"
#include "wtr2955v110_gnss_device_intf.h"
#endif 

#ifdef FEATURE_RF_HAS_QFE2340
#include "qfe2340_pa.h"
#include "qfe2340_asm.h"
#include "qfe2340_typedef_ag.h"
#include "qfe2340_physical_device_ag.h"
#endif

#include "qfe2320_pa.h"
#include "qfe2320_asm.h"
#include "qfe2320_typedef_ag.h"
#include "qfe2320_physical_device_ag.h"

#if (!( defined(FEATURE_JOLOKIA_MODEM) || defined(FEATURE_TABASCO_MODEM)))
#include "qfe2320ept_pa.h"
#include "qfe2320ept_asm.h"
#include "qfe2320ept_typedef_ag.h"
#endif 

#include "rfdevice_rxtx_common_class.h"
#include "rfc_cmn_transceiver_hdet_adapter.h"

#include "wtr2955_common_device_intf.h"
#include "wtr2955_physical_device.h"
#include "wtr2955v110_common_device_intf.h"
#include "wtr2955v110_physical_device.h"
#include "qfe2520_physical_device.h"
#include "qfe2550_physical_device.h"
#include "qfe1040_physical_device_ag.h"
#include "qfe3320_physical_device_ag.h"
#include "qfe2082fc_physical_device_ag.h"
#include "qfe4320_physical_device_ag.h"
#include "qfe3340fc_physical_device.h"

#if (!( defined(FEATURE_JOLOKIA_MODEM) || defined(FEATURE_TABASCO_MODEM)))
#include "wtr2605_rxtx_common_class.h"
#endif
#ifdef FEATURE_RF_HAS_QTUNER

#ifdef FEATURE_RF_ASDIV
#include "rfcommon_asdiv_tuner_manager.h"
#endif

extern "C"
{
  #include "rfdevice_antenna_tuner_intf.h"
  #include "rfdevice_hdet_cmn_intf.h"
}
#endif /*FEATURE_RF_HAS_QTUNER */


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Device factory handler for creating WTR2605 instance

  @details
  This is an internal device factory handler for creating WTR1605. The common
  and tech specific instances are created. Common instances will have references
  to tech specific instances. All tech specific instance creation and header
  file includes need to be featurized.
 
  Please note that the device specific factory handlers should be static and
  not be exposed to other modules for direct call.
  
  @param cfg
  Device configuration settings of the device which needs to be created.
  Typically, this configuration should be autogenerated in RF card specific
  autogen file.
 
  @return rfdevice_class*
  Returns an instance of rfdevice_class (super class of the devices)  
*/
#if (!( defined(FEATURE_JOLOKIA_MODEM) || defined(FEATURE_TABASCO_MODEM)))

static rfdevice_class* 
rf_device_factory_create_wtr2605_device
( 
  rfc_device_cfg_info_type* cfg 
)
{
  rfdevice_class* instance = NULL;

  /* Get Common NV table Pointer */
  rfcommon_nv_tbl_type *rfnv_common_nv_tbl_ptr = rfcommon_nv_get_tbl_ptr();

  rfdevice_cmn_int_dev_cal_data_type cal_data; /* Cal Data */
  rfdevice_bus_info_type wtr2605_bus_info;
  uint32 rc_process_index = 0;

  rc_process_index = (cfg->rf_asic_id) * 8;
  /* Populate Calibration Data with pointer to NV */
  if(rc_process_index < RFCOMMON_MAX_RC_TUNER_CAL_ITEMS)
  {
    cal_data.rc_tuner.rc_process_errors = 
        &rfnv_common_nv_tbl_ptr->rc_process_error[rc_process_index];
  }
  cal_data.rc_tuner.number_of_bytes = 8;
  
  if( cfg->rf_asic_id == 0 )
  {
    cal_data.hdet_autocal = &rfnv_common_nv_tbl_ptr->hdet_autocal.dev0;

    /*Get this information from device dispatch*/
    cal_data.process = &device_process[0];
  }
  else if( cfg->rf_asic_id == 1 )
  {
    cal_data.hdet_autocal = &rfnv_common_nv_tbl_ptr->hdet_autocal.dev1;

    /*Get this information from device dispatch*/
    cal_data.process = &device_process[1];
  }
  else
  {
    RF_MSG_1( RF_ERROR, "rf_device_factory_create_wtr_device. Invalid"
                        "asic id %d, using hdet autocal from dev0",
              cfg->rf_asic_id );
    cal_data.hdet_autocal = &rfnv_common_nv_tbl_ptr->hdet_autocal.dev0;
  }

  wtr2605_bus_info.phy_rx_bus = cfg->bus[1];
  wtr2605_bus_info.phy_tx_bus = cfg->bus[0];

  //create WTR2605 common device
  rfdevice_rxtx_common_class* legacy_cmn_device =
    new wtr2605_rxtx_common_class(wtr2605_bus_info, cal_data);
  instance = new rfdevice_rxtx_common_adapter( legacy_cmn_device, cfg->rf_asic_id );

  wtr2605_rxtx_common_class* wtr2605_common =\
       (wtr2605_rxtx_common_class*) instance;

  /*! create CDMA device*/
  #if defined(FEATURE_CDMA1X) || defined(FEATURE_HDR)
  wtr2605_cdma_create_device(wtr2605_common->get_instance());
  new rfdevice_trx_cdma_rx_adapter((rfdevice_rxtx_common_class*)instance,
                                   RFDEVICE_TRX_PATH_0);
  new rfdevice_trx_cdma_tx_adapter((rfdevice_rxtx_common_class*)instance, 
                                   RFDEVICE_TRX_PATH_0);
  #endif
  /*! create GSM device*/
  #ifdef FEATURE_GSM 
  wtr2605_gsm_create_device(wtr2605_common->get_instance());
  new rfdevice_trx_gsm_rx_adapter((rfdevice_rxtx_common_class*)instance,
                                  RFDEVICE_TRX_PATH_0);
  new rfdevice_trx_gsm_tx_adapter((rfdevice_rxtx_common_class*)instance, 
                                  RFDEVICE_TRX_PATH_0);
  #endif
  /*! create WCDMA device*/
  #ifdef FEATURE_WCDMA
  wtr2605_wcdma_create_device(wtr2605_common->get_instance());
  new rfdevice_trx_wcdma_rx_adapter((rfdevice_rxtx_common_class*)instance,
                                   RFDEVICE_TRX_PATH_0, cfg->rf_asic_id);
  new rfdevice_trx_wcdma_tx_adapter((rfdevice_rxtx_common_class*)instance, 
                                   RFDEVICE_TRX_PATH_0);
#endif
  /*! create TDSCDMA device*/
  #ifdef FEATURE_TDSCDMA
  wtr2605_tdscdma_create_device(wtr2605_common->get_instance());
  new rfdevice_trx_tdscdma_rx_adapter((rfdevice_rxtx_common_class*)instance,
                                      RFDEVICE_TRX_PATH_0);
  new rfdevice_trx_tdscdma_tx_adapter((rfdevice_rxtx_common_class*)instance, 
                                      RFDEVICE_TRX_PATH_0);
  #endif
  /*! create GNSS device*/
  #ifdef FEATURE_CGPS
  rfdevice_wtr2605_gnss_config(RFGNSS_PATH_0);
  #endif

  return instance;
}
#endif

#ifdef FEATURE_RF_ASDIV
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Device factory handler for creating an ASD Tuner Manager instance
 
  @details
  
  @param cfg
  Device config settings like manafacturer id, product id, slave id, group id
  etc needed to create this device. This information is expected from RFC. 
 
  @return rfdevice_class*
  Returns instance of rfdevice_class
*/
rfdevice_class* 
rf_device_factory_create_asd_tuner_device ( void )
{
  rfdevice_class *instance = NULL;
  
//To avoid compilation errors. To be fixed properly later on.  
/* Instantiate an ASD Tuner Manager */  
  instance = create_rfdevice_asd_tuner_manager(); 
  return instance; 
} /* rf_device_facotry_create_asd_tuner_device */
#endif



/*----------------------------------------------------------------------------*/
/*!
  @brief
  Device factory handler to create TRX HDET adapter instance
 
  @details
 
  @param cfg
  Device config settings like manafacturer id, product id, slave id, group id
  etc needed to create this device. This information is expected from RFC.
 
  @return rfdevice_class*
  Returns instance of rfdevice_class
*/
static rfdevice_class* 
rf_device_factory_create_trx_hdet_adapter_device
( 
  rfc_device_cfg_info_type* cfg 
)
{
  rfdevice_class *instance = NULL;
  rfdevice_class *hdet_device = NULL;
  rfc_intf *rfc_cmn = rfc_intf::get_instance();
  rfdevice_rxtx_common_class* trx_ptr = NULL;

  RF_MSG_1( RF_MED, "rf_device_factory_create_trx_hdet_adapter_device() called:"
                    " dev_instance = rf_asic_id = %d", cfg->rf_asic_id);

  hdet_device = rfc_cmn->get_cmn_rf_device_object( RFDEVICE_HDET,
                                                   cfg->rf_asic_id);
  if( hdet_device != NULL)
  {
    RF_MSG( RF_MED, "TRX HDET adapter obj NOT created since "
                    "RFDEVICE_HDET obj has already been created");
    return NULL;
  }

  trx_ptr = (rfdevice_rxtx_common_class *)rfc_cmn->
      get_cmn_rf_device_object( cfg->associated_rf_device_type,   // RFDEVICE_TRANSCEIVER
                                (uint8)cfg->associated_rf_asic_id /* trx_instance */);

  if( trx_ptr != NULL )
  {

	#ifndef T_RUMI_EMULATION
    if ( trx_ptr->rfdevice_id_get() == WTR2955 )
    {
      instance = wtr2955_common_create_hdet_device( trx_ptr );
    }
	#endif
    else if ( trx_ptr->rfdevice_id_get() == WTR2955V110 )
    {
      instance = wtr2955v110_common_create_hdet_device( trx_ptr );
    }
    else
    {
      instance = new rfc_cmn_transceiver_hdet_adapter(
                                      trx_ptr->get_instance());
    }
    RF_MSG( RF_MED, "TRX HDET adapter obj created since "
                    "RFDEVICE_HDET obj has NOT been created");
  }
  else
  {
    RF_MSG( RF_ERROR, "rf_device_factory_create_trx_hdet_adapter_device:"
                      " NULL trx_ptr. TRX HDET adapter obj NOT created");
  }
  return instance;
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Programs the device's USID based on the device configuration.

  @details
  This is the external API to the RFC for programming device ID. This only applies
  to RFFE devices. Before creating the device objects, program each device's USID.
 
  @param cfg
  Device configuration settings of the device which needs to be created. Typically, this
  configuration should be autogenerated in RF card specific autogen file.
 
  @return TRUE if device id programming is successful else FALSE.
*/
boolean rf_device_factory_program_device_id(rfc_phy_device_info_type* cfg)
{
  boolean status = TRUE;  


  /*NULL pointer check*/
  if (cfg == NULL)
  {
    RF_MSG( RF_ERROR, "rf_device_factory_program_device_id  failed !  cfg structure is NULL");
    return FALSE;
  }

 /*Program the device USIDs for each physical device as per the phy cfg structure supplied*/
  switch(cfg->rf_device_id) 
  {

  case GEN_PA:
    status = rfdevice_pa_program_device_id_phy(cfg);
    break;

  case GEN_DEVICE:
  case GEN_ASM:
  case QFE2520:
  case QFE2550:
  case TP_TUNER:
  case QFE1101:
  case QFE1100:
  case QFE2101:
    status = rffe_program_assigned_usid(cfg);
    break;

  case QFE2720:
  case QFE2340:
  case QFE2320:
  case QFE2320EPT:
  case QFE3320:
  case QFE4320:
  case QFE1040:

    status = rffe_program_assigned_usid_ext(cfg);
    break;
  case QFE2082FC:
    status = rffe_program_assigned_usid_ext(cfg);
    break;
  case QFE3340FC:
    status = rffe_program_assigned_usid_ext_product_id(cfg);
    break;
  default:
    break;
  }

  return status;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Creates tuner manager based on the tuner instance provided

  @details
  This is a temporary function which would create a tuner manager based on the
  tuner device provided. This API needs to be removed when the tuner manager is
  part RFC AG.

  
  @param tuner_obj
  Instance of the tuner object
 
  @return rfdevice_class*
  Returns an instance of rfdevice_class (super class of the devices) which is to be stored
  by RFC.
*/
rfdevice_class* 
rf_device_factory_create_tuner_manager
(
  rfdevice_class* device_obj,
   rfcommon_atuner_manager_factory_type manager_type
)
{
  rfcommon_atuner_manager_factory *tuner_mgr_factory = NULL;
  rfdevice_class *tuner_mgr_obj = NULL;
  
  if(device_obj == NULL)
  {
    RF_MSG( RF_ERROR, "rf_device_factory_create_tuner_manager() failed as"
                      " NULL tuner device provided!");
    return tuner_mgr_obj;
  }

  /* Create the instance for Tuner mananger factory */
  tuner_mgr_factory = new rfcommon_atuner_manager_factory (  );

  /* Check if tuner_mgr_factory is valid*/
  if(tuner_mgr_factory != NULL)
  {
    /*Using the tuner manager factory, we can create the tuner 
      manager of interest*/
    /*!Note: By default creating AOL tuner manager for now*/
    tuner_mgr_obj = tuner_mgr_factory->create_atuner_manager( manager_type, 
                                                                 device_obj);
  }
  else
  {
    RF_MSG( RF_ERROR, "rf_device_factory_create_tuner_manager() failed as"
                      "Tuner manager factory instance cannot be created!");
  }

  /*Tuner manager factory*/
  delete tuner_mgr_factory;

  /*Return Tuner manager created */
  return tuner_mgr_obj;
  
} /*rf_device_factory_create_tuner_manager*/

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Creates device instance based on the device configuration provided.

  @details
  This is the external API to the RFC for creating device instances. The device
  factory instantiates the common and tech specific instances for a specific device
  based on the device configuration parameter passed in. Common instances will have
  references to tech specific instances. Every device type (ex: WTR1605,
  WTR2605) is allowed one handle. All tech specific instance creation and header file
  includes need to be featurized.
 
  This API will only dispatch the call to device specific create handler. All logic for
  creating device needs to be maintained in the device specific handler.
  
  @param cfg
  Device configuration settings of the device which needs to be created. Typically, this
  configuration should be autogenerated in RF card specific autogen file.
 
  @return
  Returns an instance of rfdevice_class (super class of the devices) which is to be stored
  by RFC.
*/
rfdevice_class* rf_device_factory_create_device
(
  rfc_device_cfg_info_type* cfg
)
{
  rfdevice_class* instance = NULL;
//*NULL Pointer Checks*/
  if (cfg == NULL)
  {
	 
	/* crashing phone as critical params missing */   
	ERR_FATAL("rf_device_factory_create_device creation failed. cfg == NULL", 0, 0, 0);
    
  }

  switch(cfg->rf_device_id) 
  {

  case WTR2605:
#if (!( defined(FEATURE_JOLOKIA_MODEM) || defined(FEATURE_TABASCO_MODEM)))
    instance = rf_device_factory_create_wtr2605_device( cfg );
#endif
    break;

    case TRX_HDET:
      instance = rf_device_factory_create_trx_hdet_adapter_device (cfg);
      break;

   default:
      break;
  }

  return instance;
}


/*!
  @brief
  Factory method to create instances of 3rd party logical device instances

  @details
  Third party devices could have multiple logical components.
  This API creates the logical components  for 3rd party devices
  and associates it with its parent physical device.

  @param rfdevice_physical_third_party_p
  Pointer to the parent phsyical device 
 
  @param logical_device_cfg
  Pointer to the logical device cfg of the 3rd party device
 
  @return
  Pointer to the logical device object of the 3rd party device, or NULL if no device could
  be created.

  @retval NULL
  The requested device could not be created successfully.
*/
rfdevice_logical_component* create_gen_device_object(
                                                        rfdevice_physical_device     *rfdevice_physical_third_party_p,
                                                        rfc_logical_device_info_type      *logical_device_cfg
                                                    )
{

  if ( (NULL == rfdevice_physical_third_party_p) || (NULL == logical_device_cfg))
  {
     RF_MSG( RF_ERROR, " create_gen_device_object() -  NULL pointer detected!" );
     return NULL;
  }

  switch(logical_device_cfg->rf_device_id) 
  {
  
   case GEN_ASM:
     return (create_gen_asm_object( rfdevice_physical_third_party_p , logical_device_cfg ) );

   case GEN_PA:
     return (create_gen_pa_object( rfdevice_physical_third_party_p , logical_device_cfg ) );
 
   default:
     return NULL;
	
  }

}

/*!
  @brief
  Factory method to create instances of physical device interfaces.

  @details
  The physical device object represents the hardware device in the system.
  This method will construct a software object that represents the physical
  device, and returns a pointer to the physical device abstraction.

  @param cfg
  Pointer to a device configuration structure that describes what type of
  device to try and create.

  @return
  Pointer to the physical device that was created, or NULL if no device could
  be created.

  @retval NULL
  The requested device could not be created successfully.
*/
rfdevice_physical_device*
rf_device_factory_create_phys_device
(
  rfc_phy_device_info_type* cfg
)
{
  /*NULL Pointer Checks*/
 if (cfg == NULL)
 {
		 
	/* crashing phone as critical params missing */  
	ERR_FATAL("rf_device_factory_create_phys_device creation failed. cfg == NULL", 0, 0, 0);

 }

 if ( cfg->rf_device_comm_protocol == RFDEVICE_COMM_PROTO_RFFE) 
 {
 if ( !rffe_scan(cfg) )
 {
   RF_MSG_1( RF_ERROR, "rf_device_factory_create_phys_device: Device %d not found",
                                                               cfg->rf_device_id );
   return NULL;
 }
 }
  switch(cfg->rf_device_id) 
  {
  case TEST_DEVICE:
    return new test_device_physical_device(cfg);

  #ifndef T_RUMI_EMULATION
  case WTR2955:
    return new wtr2955_physical_device(cfg);
  #endif
  case WTR2955V110:
    return new wtr2955v110_physical_device(cfg);

#ifdef FEATURE_RF_HAS_QTUNER
  case QFE2520:
    return new qfe2520_physical_device(cfg, FALSE);

  case QFE2550:
    return new qfe2550_physical_device(cfg, FALSE);

#endif /* FEATURE_RF_HAS_QTUNER */

  case QFE1100:
  case QFE1101:
  case QFE2101:
    return new papm_physical_device(cfg);



  case QFE2320:
  case QFE2320EPT:
    return new qfe2320_physical_device(cfg);

  case QFE2340:
    return new qfe2340_physical_device(cfg);

  case QFE1040:
    return new qfe1040_physical_device(cfg);
  case QFE2082FC:
    return new qfe2082fc_physical_device(cfg);
  case QFE3320:
    return new qfe3320_physical_device(cfg);
  case QFE3340FC:
    return new qfe3340fc_physical_device(cfg);
  case QFE4320:
    return new qfe4320_physical_device(cfg);

  case GEN_DEVICE:
  case GEN_PA:
  case GEN_ASM:
    return new rfdevice_physical_third_party(cfg);

  default:
    return NULL;
  }
}


/*!
  @brief

  @details
*/
boolean
rf_device_factory_create_gnss_device
( 
  rfc_device_info_type* cfg
)
{
  boolean status = TRUE;
  /*NULL Pointer Checks*/
  if (cfg == NULL)
  {
    /* crashing phone as critical params missing */   
    ERR_FATAL("rf_device_factory_create_gnss_device creation failed. cfg == NULL", 
              0, 0, 0);
    
  }
#ifdef FEATURE_CGPS
  switch( cfg->rf_asic_info[0].device_id )
  {
   #ifndef T_RUMI_EMULATION
   case WTR2955:
    status &= 
      rfdevice_wtr2955_gnss_set_instance_id( cfg->rf_asic_info[0].instance );
    break;
   #endif
  case WTR2955V110:
    status &= 
      rfdevice_wtr2955v110_gnss_set_instance_id( cfg->rf_asic_info[0].instance );
    break;

  default:
    RF_MSG_1( RF_ERROR, "rf_device_factory_create_gnss_device. Unsupported device_id %d ",
              cfg->rf_asic_info[0].device_id );\
    status = FALSE;
    break;

  } /* switch( cfg->rf_asic_info[0].device_id ) */

  if(status == FALSE)
  {
    RF_MSG_2( RF_ERROR, "rf_device_factory_create_gnss_device Failed!"
                        "device_id %d, instance %d ",
              cfg->rf_asic_info[0].device_id,
              cfg->rf_asic_info[0].instance );
  }
#endif
  return status;
} /* rf_device_factory_create_gnss_device */
