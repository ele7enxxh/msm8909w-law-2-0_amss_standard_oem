/*===========================================================================

                         D S _ Q M I _ S V C _ E X T . C

DESCRIPTION

  The Data Services Qualcomm MSM Interface QMI Service External source file
EXTERNALIZED FUNCTIONS

  Upper interface APIs:


INTERNAL FUNCTIONS:

Copyright (c) 2004-2012 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/qmicore/src/ds_qmi_svc_ext.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
11/08/12    rk     Removed QCMAP service init.
02/28/12    sj     Add support for TDSCDMA
03/06/12    wc     Remove QMI UIM/CAT from Data
02/01/12    am     Added QCMAP service init.
01/31/12    wc     Support QSAP registration on multiple QMUX instances
01/05/12    sa     Changes for mew QMI CSVT service
10/14/11    wc     Featurize QSAP
10/09/11    wc     QMUX to QMI bridge (QSAP)
03/08/11    ua     Retreiving WCDMA/HSDPA/HSUPA default data rates from RRC.
02/09/11    ua     Added support for LTE Data rates.
12/24/10    kk     Featurizing calls to QMI VOICE APIs.
10/22/09    kk     Windows7 compliance - WDS changes and BER/PER/FER support.
12/12/06    ks     Created module
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/


#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA_QMI
#include "amssassert.h"
#include "msg.h"
#include "ds_qmi_svc.h"
#include "ds_qmi_svc_ext.h"
#include "ds_qmux_ext.h"
#include "ds707_extif.h"
#include "ds_Utils_DebugMsg.h"

#include "ds_qmi_nv_util.h"
#include "dcc_task_svc.h"

#include "ps_system_heap.h"

#ifdef FEATURE_QSAP_SERVICE
#include "ds_qmuxi.h"
#include "qmi_sap_xport.h"
#endif /* FEATURE_QSAP_SERVICE */
/*---------------------------------------------------------------------------
  Definitions
---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------
   Device manufacturer name
---------------------------------------------------------------------------*/
/* Max length of the manufacturer name string cannot exceed 65535 bytes    */
#define QMI_DEVICE_MFR_STR  "QUALCOMM INCORPORATED"

#define DEFAULT_MIP_PROFILE_INDEX (0)

/*===========================================================================

                                GLOBAL DATA

===========================================================================*/
/*---------------------------------------------------------------------------
   Device manufacturer name
---------------------------------------------------------------------------*/
char * qmi_device_mfr_str = QMI_DEVICE_MFR_STR;

/*===========================================================================

                           FORWARD DECLARATIONS

===========================================================================*/

#ifdef FEATURE_DATA_QMI_PDS_COUPLED
extern void qmi_pds_init
(
  qmi_instance_e_type  qmi_inst
);
#endif /* FEATURE_DATA_QMI_PDS_COUPLED */

#ifdef FEATURE_QSAP_SERVICE
extern qmi_sap_xport_ops_type qsap_qmux_ops;
extern void qmi_sap_service_init(void);
#endif /* FEATURE_QSAP_SERVICE */

/*---------------------------------------------------------------------------
   Vendors have no access to the QMI library source code.
   They cannot recompile the library, but library needs to be aware
   of the number of vendor services present. The following variables
   will store the IDs of the first and last vendor specific service.
   These variables will be referenced in the QMI library.
---------------------------------------------------------------------------*/
uint8 qmi_vs_services_min;
uint8 qmi_vs_services_max;

/*-------------------------------------------------------------------------
  Global variable to determine which profile family (rmnet/atcop) is used
  for Rmnet calls
-------------------------------------------------------------------------*/
uint16 rmnet_umts_profile_family;

/*---------------------------------------------------------------------------
  ATTENTION VENDORS: See below for Vendor-specific service sample code.
   Only after the vs-list is initialized, the intialization functions can be
   called for individual services to register with QMUX.
   Note that memory allocated for service list should be enough to facilitate
   all the services.
 --------------------------------------------------------------------------*/
/* Sample for Vendors
// extern void qmi_vs1_init
// (
//  qmi_instance_e_type  qmi_inst
// );
// void qmi_vs_init
// (
//   qmi_instance_e_type qmi_instance
// )
// {
//    qmi_vs1_init(qmi_instance);
// }
*/

void qmi_svc_ext_get_gprs_edge_slot_count
(
  uint8   multislot_class,
  uint8 * rx_slots,
  uint8 * tx_slots
);


/*===========================================================================

                        EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================
  FUNCTION QMI_SVC_EXT_INIT()

  DESCRIPTION
    Initialize specified QMUX instance.  SIO control IO path will be set up,
    and QMUX instance state will be reset to default.

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_svc_ext_init
(
  qmi_instance_e_type i
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-----------------------------------------------------------------------
    Initialize the QMI SVC powersave critical section
  -----------------------------------------------------------------------*/
  qmi_svc_common_svc_powerup_init();

  /*-------------------------------------------------------------------------
    Initialize the QMI services
  -------------------------------------------------------------------------*/

#ifdef FEATURE_DATA_QMI_PDS_COUPLED
  qmi_pds_init(i);
#endif /* FEATURE_DATA_QMI_PDS_COUPLED */

#ifdef FEATURE_QSAP_SERVICE
  qmi_sap_xport_start(&qsap_qmux_ops, (void*)i);
  qmi_sap_service_init();
#endif /* FEATURE_QSAP_SERVICE  */

  /*-------------------------------------------------------------------------
   ATTENTION VENDORS: This is the place to initialize vendor specific
   services. Vendors can selectively choose which QMI instance calls
   qmi_vs_init() which is described in the sample code and which instance
   includes or excludes particular services. As an example, the below snippet
   shows the vendor specific service initialization only on the first QMI
   instance.
     if (i == 0)
     {
       qmi_vs_init(i);
     }
   ------------------------------------------------------------------------*/
}

/*===========================================================================
  FUNCTION QMI_SVC_EXT_GET_DEFAULT_CHANNEL_RATES()

  DESCRIPTION
    Get the current serving system's default MAX data channel rate values
    given the channel type supported.
    These rates are reported by QMI as a fallback mechanism when
    not in a call or for cases when lower layers do not return the correct
    current rates

  PARAMETERS
    supp_channel : Type of channel supported
    tx_rate      : Tx rate to be reported
    rx_rate      : Rx rate to be reported

  RETURN VALUE
    none

  DEPENDENCIES
    none

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_svc_ext_get_default_channel_rates
(
  qmi_channel_info_s_type supp_channel_info,
  uint32 *                tx_rate,
  uint32 *                rx_rate
)
{
  uint8 rx_slots;
  uint8 tx_slots;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ASSERT (rx_rate);
  ASSERT (tx_rate);

  *rx_rate = 0;
  *tx_rate = 0;

  switch( supp_channel_info.supp_channel )
  {
    case QMI_CHANNEL_SUPP_MIN:
    {
      /* *rx_rate = 0; *tx_rate = 0; */
      break;
    }

    case QMI_CHANNEL_SUPP_CDMA:
    case QMI_CHANNEL_SUPP_HDR_REV0:
    case QMI_CHANNEL_SUPP_HDR_REVA:
    {
#ifdef FEATURE_CDMA
      ds707_extif_bearer_tech_rate_type rate_query;

      if (QMI_CHANNEL_SUPP_CDMA == supp_channel_info.supp_channel)
      {
        rate_query.bearer_tech = DS707_EXTIF_1X;
      }
      else if (QMI_CHANNEL_SUPP_HDR_REV0 == supp_channel_info.supp_channel)
      {
        rate_query.bearer_tech = DS707_EXTIF_HDR_REV0;
      }
      else
      {
        /* Default to the highest rates (RevA) when in HDR and not explicitly Rev0*/
        rate_query.bearer_tech = DS707_EXTIF_HDR_REVA;
      }
      rate_query.max_tx_bearer_tech_rate = 0;
      rate_query.max_rx_bearer_tech_rate = 0;
      if (ds707_extif_get_bearer_tech_max_rate(&rate_query))
      {
        *tx_rate = rate_query.max_tx_bearer_tech_rate;
        *rx_rate = rate_query.max_rx_bearer_tech_rate;
      }
#endif
      break;
    }

    case QMI_CHANNEL_SUPP_GPRS:
    {
      /* determine the max number of slots based on the multislot-class */
      qmi_svc_ext_get_gprs_edge_slot_count(
                             supp_channel_info.extend.gprs_edge.multislot_class,
                             &rx_slots,
                             &tx_slots);

      /* maintain the previous default values set if the multislot_class lookup
         fails for either the rx or tx slots */
      if (0 == rx_slots)
      {
        /* default to 4 slots */
        rx_slots = 4;
      }
      if (0 == tx_slots)
      {
        /* default to 1 slot */
        tx_slots = 1;
      }

      /* max data rate is the maximum number of slots for Rx/Tx (based on the
         multislot_class lookup above) multiplied by the maximum rate for each
         slot which is 20000 for GPRS */
      *rx_rate = 20000 * rx_slots;
      *tx_rate = 20000 * tx_slots;
      break;
    }

    case QMI_CHANNEL_SUPP_EDGE:
    {
      /* determine the max number of slots based on the multislot-class */
      qmi_svc_ext_get_gprs_edge_slot_count(
                             supp_channel_info.extend.gprs_edge.multislot_class,
                             &rx_slots,
                             &tx_slots);

      /* maintain the previous default values set if the multislot_class lookup
         fails for either the rx or tx slots */
      if (0 == rx_slots)
      {
        /* default to 4 slots */
        rx_slots = 4;
      }
      if (0 == tx_slots)
      {
        /* default to 4 slot */
        tx_slots = 4;
      }

      /* max data rate is the maximum number of slots for Rx/Tx (based on the
         multislot_class lookup above) multiplied by the maximum rate for each
         slot which is 59200 for GPRS */
      *rx_rate = 59200 * rx_slots;
      *tx_rate = 59200 * tx_slots;
      break;
    }

    case QMI_CHANNEL_SUPP_WCDMA:
    {
      qmi_if_get_wcdma_default_channel_rate(tx_rate, rx_rate );
      break;
    }
     case QMI_CHANNEL_SUPP_TDSCDMA:
    {
      qmi_if_get_tdscdma_default_channel_rate(tx_rate, rx_rate );
      break;
    }
#ifdef FEATURE_LTE
    case QMI_CHANNEL_SUPP_LTE:
    {
      *tx_rate = 50000000;
      *rx_rate = 100000000;
    }
    break;
#endif /* FEATURE_LTE  */

    default:
    {
      *rx_rate = 0;
      *tx_rate = 0;
      LOG_MSG_ERROR_0 ("Invalid channel support type!");
      break;
    }
  }
} /* qmi_svc_ext_get_default_channel_rates() */

/*===========================================================================
  FUNCTION QMI_SVC_EXT_GET_GPRS_EDGE_SLOT_COUNT()

  DESCRIPTION
    Get the maximum number of GPRS/EDGE slots based on a given multislot class
    value.

  PARAMETERS
    multislot_class : Multislot class to lookup
    rx_slot         : Rx slot count to be reported
    tx_slot         : Tx slot count to be reported

  RETURN VALUE
    none

  DEPENDENCIES
    none

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_svc_ext_get_gprs_edge_slot_count
(
  uint8   multislot_class,
  uint8 * rx_slots,
  uint8 * tx_slots
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ASSERT (rx_slots);
  ASSERT (tx_slots);

  /* lookup the maximum number of slots for Tx channel based on the multislot
    class as documented in TS 3GPP 45.002

     valid multislot-class range from 45.002 is 1-45 (inclusive) but currently
     only 1-34 (inclusive) are supported in the GMM Utils code that reads
     the NV_MULTISLOT_CLASS_I and NV_EDGE_MULTISLOT_CLASS_I values */
  switch( multislot_class )
  {
    case  1:
      *rx_slots = 1;
      break;

    case  2:
    case  3:
    case  5:
      *rx_slots = 2;
      break;

    case  4:
    case  6:
    case  7:
    case  9:
    case 13:
      *rx_slots = 3;
      break;

    case  8:
    case 10:
    case 11:
    case 12:
    case 14:
      *rx_slots = 4;
      break;

    case 15:
    case 30:
    case 31:
    case 32:
    case 33:
    case 34:
      *rx_slots = 5;
      break;

    case 16:
      *rx_slots = 6;
      break;

    case 17:
      *rx_slots = 7;
      break;

    case 18:
      *rx_slots = 8;
      break;

    default:
      *rx_slots = 0;
      break;
  }

  /* lookup the maximum number of slots for Tx channel based on the multislot
    class as documented in TS 3GPP 45.002

     valid multislot-class range from 45.002 is 1-45 (inclusive) but currently
     only 1-34 (inclusive) are supported in the GMM Utils code that reads
     the NV_MULTISLOT_CLASS_I and NV_EDGE_MULTISLOT_CLASS_I values */
  switch( multislot_class )
  {
    case  1:
    case  2:
    case  4:
    case  8:
    case 30:
      *tx_slots = 1;
      break;

    case  3:
    case  5:
    case  6:
    case  9:
    case 10:
    case 31:
      *tx_slots = 2;
      break;

    case  7:
    case 11:
    case 13:
    case 32:
      *tx_slots = 3;
      break;

    case 12:
    case 14:
    case 33:
      *tx_slots = 4;
      break;

    case 15:
    case 34:
      *tx_slots = 5;
      break;

    case 16:
      *tx_slots = 6;
      break;

    case 17:
      *tx_slots = 7;
      break;

    case 18:
      *tx_slots = 8;
      break;

    default:
      *tx_slots = 0;
      break;
  }
} /* qmi_svc_ext_get_gprs_edge_slot_count() */

/*===========================================================================
  FUNCTION QMI_SVC_EXT_ALLOW_AUTO_CONNECT()

  DESCRIPTION
    Determines whether an auto-connect request should be allowed to be serviced

  PARAMETERS
    none

  RETURN VALUE
    TRUE  - request allowed
    FALSE - request denied

  DEPENDENCIES
    none

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_svc_ext_allow_auto_connect
(
  void
)
{
  boolean allow_auto = TRUE;
  boolean nv_enable_check = FALSE;
  nv_item_type      *dcc_nv_item_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Allocate temporary memory for the NV item
  -------------------------------------------------------------------------*/
  PS_SYSTEM_HEAP_MEM_ALLOC(dcc_nv_item_ptr, sizeof(nv_item_type), nv_item_type*);
  if( dcc_nv_item_ptr == NULL )
  {
    return FALSE;
  }

  if (QMI_NV_STATUS_OK == qmi_nv_read(QMI_NV_ITEM_RESTRICT_AUTOCONNECT, 0, 0,
                                      &nv_enable_check,
                                      sizeof(nv_enable_check)))
  {
    /* read the active MIP profile index */
    if (NV_DONE_S == dcc_get_nv_item(NV_DS_MIP_ACTIVE_PROF_I, dcc_nv_item_ptr))
    {
      /* only allow auto-connect to be enabled if the active MIP profile is not
         the factory-provisioned default value */
      if (DEFAULT_MIP_PROFILE_INDEX == dcc_nv_item_ptr->ds_mip_active_prof)
        allow_auto = FALSE;
    }
  }
  /*-------------------------------------------------------------------------
    Free the temporary memory allocated for NV item.
  -------------------------------------------------------------------------*/
  PS_SYSTEM_HEAP_MEM_FREE(dcc_nv_item_ptr);

  return allow_auto;
}
#endif /* FEATURE_DATA_QMI */

