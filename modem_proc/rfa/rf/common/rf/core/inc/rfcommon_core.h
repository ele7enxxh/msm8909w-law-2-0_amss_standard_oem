#ifndef RFCOMMON_CORE_H
#define RFCOMMON_CORE_H

/*!
   @file
   rfcommon_core.h

   @brief

   @details

*/

/*===========================================================================
Copyright (c) 2008 - 2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.


                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$DateTime: 2016/12/13 07:59:52 $ $Author: mplcsds1 $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/rf/core/inc/rfcommon_core.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
09/23/14   zbz     MCS-RF interface changes for GSM and LTE/TDS coex support 
10/03/13   sb      Add changes to receive MSGR cmd from MCS to switch to filtered/unfiltered 
                   path for SGLTE coexistence (GSM DCS +LTE B39/TDS B39/B34) 
09/17/13   sb      Support for SGLTE/SGTDS coexistence 
08/28/13   vs      Added rfcommon_core_path_to_receive_chain API
05/03/13   rmb     Added rfcommon_core_convert_rfm_device_to_cell_type() API.
11/27/12   aki     Added rfcommon_core_deinit
09/19/12   sbm     Clean up un-used. Remove rfcommon_core_set_tx_on(), 
06/17/12   pl      Update interface to use device instead of path
01/16/12   vrb/pv  RF Common Core Device Manager Implementation.  
09/21/11   aro     Changed rfcommon_core_init() to return boolean
08/10/11   tnt     Initial Ant Tuner support 
08/10/11   pl      Remove all references to legacy RFC structure and related API
06/08/11   tnt     Fix compiler warning 
06/08/11   tnt     Add rfcommon_core_gnd_pa_on()  
04/22/11   tnt     Add  rfcommon_core_get_rf_spi_id() to retrieve SPI ID from RFC
04/18/11   xw      Deleted VCO coarse tune 
02/28/11           Merge to MDM9K PLF110
02/08/11   sty     Deleted rfcommon_custom_config_pa_temp
02/04/11   pl      Mainline RF_GRFC_COMMON_DESIGN_INTERIM_FEATURE
02/03/11   pl      Move rfcommon_core_config_card_gpio() back to enter_mode()
                   GPIO config in boot up time is not possible due to IRC dsetup delay.
02/03/11   sty     Added rfcommon_custom_config_pa_temp()
01/24/11   whc     Gaurantee 250uS timing for SMPS_EN, disable at tx_disable
01/18/11   pl      Remove obsolete interfaces
01/17/11   pl      Move functionalities of rfcommon_core_config_card_gpio() to
                   constructor of rfc_intf class, retiring rfcommon_core_config_card_gpio()
01/05/11   pl      Added mode parameter to rfcommon_core_set_rx_on
01/04/11   aro     Added Extern C
09/17/10   tnt     Add rfcommon_core_config_card_gpio()
08/22/10   tnt     Add rfcommon_core_set_gpio_based_on_tx_band()
07/08/10   tnt     RFC re-architecture to support GRFC
11/24/09   sr      Added an accessor function to get the DAL handle for TLMM
10/01/09   ckl     Added rfcommon_core_config_gpio() and rfcommon_core_set_gpio_based_on_rx_band( )
08/12/09   xw      Added rfcommon_vco_coarse_tune().
03/18/09   dyc     Added rfcom_to_path[] extern reference.
10/14/08   sr      Initial version.

============================================================================*/


#include "rfcom.h"
#include "DDITlmm.h"
#include "DALSys.h"
#include "rfc_card.h"
#include "rfumts.h"
#include "rfc_common.h"
#include "rfm_umid_list.h"

#ifdef __cplusplus
   extern "C"{
#endif

typedef void (*wait_func_ptr_type) ( rex_sigs_type sig );

typedef struct
{
  rex_tcb_type *tcb_ptr;
  rex_sigs_type wait_sig;
  wait_func_ptr_type wait_func_fp;
} rf_common_rex_data_type;




typedef struct
{
    boolean rx_filter_path_flag;
    boolean tx_filter_path_flag;
    rfc_coex_filter_status_type coex_tech_filter_status[RFCOM_NUM_MODES];
}rf_common_coex_info_type;


extern rf_path_enum_type rfcom_to_path[];

/*----------------------------------------------------------------------------*/
boolean rfcommon_core_init(rf_path_enum_type path);

/*----------------------------------------------------------------------------*/
void rfcommon_core_deinit(void);

/*----------------------------------------------------------------------------*/
uint16 rfcommon_core_get_rf_spi_id (void);

/*----------------------------------------------------------------------------*/
rf_path_enum_type rfcommon_core_device_to_path(rfcom_device_enum_type device );

/*----------------------------------------------------------------------------*/
rfcom_device_enum_type rfcommon_core_path_to_device( rf_path_enum_type path );

/*----------------------------------------------------------------------------*/
void rfcommon_core_calibrate_therm( uint8 therm_min, uint8 therm_max );

/*----------------------------------------------------------------------------*/
rfcom_cell_type rfcommon_core_convert_rfm_device_to_cell_type( rfcom_device_enum_type device );

/*----------------------------------------------------------------------------*/
rfagc_receive_chain_type rfcommon_core_path_to_receive_chain(rf_path_enum_type path);



rfc_coex_filter_status_type rfcommon_core_get_coex_filter_status(rfm_mode_enum_type mode);


void rfcommon_core_set_filtered_path_status(boolean rx_flag, boolean tx_flag);

boolean rfcommon_core_get_tx_filter_status(void);

boolean rfcommon_core_get_rx_filter_status(void);

rfm_dispatch_outcome_type rfcommon_core_use_filtered_path( rfm_rf_cmn_bypass_filtered_path_req_s* rfm_msg_ptr, uint32 umid_cnf);


#ifdef __cplusplus
   }
#endif

#endif /* RFCOMMON_CORE_H */
