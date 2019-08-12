#ifndef DS_QMI_IF_H
#define DS_QMI_IF_H
/*===========================================================================

                            D S _ Q M I _ I F . H

DESCRIPTION

  The Data Services WWAN external interface file.

  This module will externalize the minimal but complete API for modules
  outside of data services to interface to WWAN logic.

  JD - Note some internal APIs exist in here currently - need to break this
       out into external and internal inter-module API files once logic is
       fleshed out.


Copyright (c) 2004-2014 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/
/*===========================================================================

                            EDIT HISTORY FOR FILE
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/qmidata/inc/ds_qmi_if.h#1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
04/04/14    vrk    CM SS events handling for SGLTE+G CM redesign
03/13/14    sah    Added support for tertiary subscription
09/27/13    wc     Support dynamic port configuration
06/07/13    rk     add support for multiple mmgsdi sessions
11/02/11    wc     Add qmi_dms_get_current_prl_info
10/04/11    sa     Q6 free floating changes for QMI.
03/08/11    ua     Retreiving WCDMA/HSDPA/HSUPA default data rates from RRC.
06/19/10    vs     Mapping to RMNET instances changed due to Dual IP support
06/04/10    mib    Removed dependency from QMI CAT
10/22/09    kk     Windows7 compliance - WDS changes and BER/PER/FER support.
10/09/09    kk     Windows7 compliance - NAS changes.
10/08/09    kk     Windows7 compliance - DMS changes.
09/06/06    ks     Featurized pin support code, under FEATURE_MMGSDI.
09/05/06    ks     Added mmgsdi_client_id to qmi_if_info struct.
08/23/06    ks     Changes to support multiple qmi/rmnet instances.
07/06/06    ks     Added functions to process DS cmds posted for CM SS and 
                   PH events. Saving only subset of CM ss_info and ph_info
                   needed by QMI in qmi_if_info.
06/07/06    ks     Added a constant for MIN signal strength.
05/01/06    ks     Added some phone_info parameters to qmi_if_info_type.
04/18/06    ks     Changed return value of qmi_if_get_signal_strength() to 
                   void.
03/06/06    ks     Changed qmi_if_get_current_ss_max_channel_rate() to 
                   qmi_if_get_max_channel_rate().
09/08/05    ks     Major clean up. Added support to listen to CM phone_events.
                   Changed name to qmi_if.
05/31/05   jd/ks   Code review updates
05/11/05    ks     Clean up.
03/14/05    ks     Removed featurization
10/27/04    jd     Added media connect/disconnect apis.
07/12/04    jd     Added qmi_if_init, qmi_if_reg_cm, qmi_if_dereg_cm,
                   qmi_if_service, qmi_if_home prototypes
05/12/04    jd     added qmi_if_uim_used() prototype
03/16/04    jd     added qmi_if_connected() prototype
03/16/04    jd     Added qmi_if_get_link_speed prototype
02/20/04    jd     Initial implementation
===========================================================================*/


/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#include "cm.h"
#include "ds_qmi_defs.h"
#include "ds_qmi_if_ext.h"
#include "ds_rmnet_defs.h"
#include "ps_iface_defs.h"

#ifdef FEATURE_MMGSDI_SESSION_LIB
  #include "mmgsdilib.h"
#endif /* FEATURE_MMGSDI_SESSION_LIB */

/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
                            EXPORTED DEFINITIONS
= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/

/*---------------------------------------------------------------------------
  Definitions
---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
  WWAN device radio information
---------------------------------------------------------------------------*/
#ifdef FEATURE_MMGSDI_SESSION_LIB
typedef enum
{
  QMI_IF_MMGSD_GW_PROV_PRI_SESSION = 0x000000000,
  QMI_IF_MMGSD_1X_PROV_PRI_SESSION,
  QMI_IF_MMGSD_GW_PROV_SEC_SESSION,
  QMI_IF_MMGSD_GW_PROV_TER_SESSION,
  QMI_IF_MMGSD_MAX_SESSION
}qmi_if_mmgsd_session_e_type;

typedef struct
{
  mmgsdi_session_id_type        session_id;
  mmgsdi_app_enum_type          app_type;
}qmi_mmgsdi_session_info_type;
#endif /* FEATURE_MMGSDI_SESSION_LIB */

typedef struct
{
  cm_client_id_type       cm_client_id;
  qmi_if_cm_ss_info_type  ss_info[QMI_IF_AS_ID_MAX];
  qmi_if_cm_ph_info_type  ph_info;
  qmi_if_cm_call_info_type call_info;
#ifdef FEATURE_MMGSDI_SESSION_LIB
  mmgsdi_client_id_type   mmgsdi_client_id;
  qmi_mmgsdi_session_info_type session_info[QMI_IF_MMGSD_MAX_SESSION];
#endif /* FEATURE_MMGSDI_SESSION_LIB */
  qmi_if_gw_info_type     gw_info;
  qmi_if_tds_info_type    tds_info;
  boolean inited;
  boolean                 otksl_valid;
  boolean                 prl_info_valid;
  nv_sec_code_type        nv_sec_code;
  nv_stat_enum_type       sec_code_nv_status;
  nv_sec_code_type        nv_otksl;
  nv_stat_enum_type       otksl_nv_status;
  uint8                   nv_otksl_flag;
  nv_stat_enum_type       otksl_flag_nv_status;
  boolean                 spc_inited;
} qmi_if_info_type;

extern qmi_if_info_type  qmi_if_info;

/*---------------------------------------------------------------------------
  Minimum value of signal strength to be returned when NO sginal
---------------------------------------------------------------------------*/
#define QMI_IF_SIG_STRENGTH_MIN   (-128)

#define QMI_IF_GET_RMNET_INSTANCE(qmi_instance, is_default)           \
          (rmnet_instance_e_type) (((int32)qmi_instance)*2 + 1 - is_default)


/*---------------------------------------------------------------------------
  Constants for No Signal -- similar to CMSS_RSSI_NO_SIGNAL (125)
---------------------------------------------------------------------------*/
#define QMI_IF_RSSI_NO_SIGNAL           (128)
#define QMI_IF_ECIO_VALUE_NO_SIGNAL     (5)
#define QMI_IF_IO_VALUE_NO_SIGNAL       (-106)
#define QMI_IF_SINR_VALUE_NO_SIGNAL     (8)

/*---------------------------------------------------------------------------
  Max number of radio interfaces that can be acquired simulataneously
---------------------------------------------------------------------------*/
#define QMI_MAX_N_ACQ_RADIO_IFS (2)
/*---------------------------------------------------------------------------
  Invalid RF band/channel values
---------------------------------------------------------------------------*/
#define QMI_IF_BAND_INVALID (SYS_BAND_CLASS_NONE)
#define QMI_IF_CHAN_INVALID (0xFFFF)
/*---------------------------------------------------------------------------
  Macros to extract the low/high 32bits of a QWORD (64 bits).
---------------------------------------------------------------------------*/
#define QWORD_LOW( qword )  ( (dword) ((uint64) qword & (uint64) 0xFFFFFFFF))
#define QWORD_HIGH( qword ) ( (dword) ((uint64) qword >> 32) )

/*---------------------------------------------------------------------------
  BCD digit extraction
---------------------------------------------------------------------------*/
#define QMI_BCD_LOW_DIGIT(a) ((a) & 0x0F)
#define QMI_BCD_HIGH_DIGIT(a) (((a) >> 4) & 0x0F)

/*---------------------------------------------------------------------------
  Event that triggers channel rate indication enum type
---------------------------------------------------------------------------*/
typedef enum
{
  QMI_IF_CH_RATE_TRIGGER_SS_CHANGE          = 0,
  QMI_IF_CH_RATE_TRIGGER_DATA_BEARER_CHANGE = 1
} qmi_if_ch_rate_trigger_e_type;

/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
                        EXTERNAL FUNCTION DEFINTIONS
= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/

/*===========================================================================
  FUNCTION QMI_IF_INIT()

  DESCRIPTION
    Initialize the WWAN interface layer

    This layer contains all the logic required for WWAN to interface with
    external modules, and any logic required for external modules to
    interface with WWAN.

  PARAMETERS
    None

  RETURN VALUE
    TRUE  - initialization succeeded
    FALSE - initialization failed

  DEPENDENCIES
    none

  SIDE EFFECTS
    None
===========================================================================*/
extern boolean qmi_if_init
(
  void
);

/*===========================================================================
  FUNCTION QMI_IF_REG_CM()

  DESCRIPTION
    Register the CM SS & PH event notification

  PARAMETERS
    None

  RETURN VALUE
    TRUE  - registration succeeded
    FALSE - registration failed

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
extern boolean qmi_if_reg_cm
(
  void
);

/*===========================================================================
  FUNCTION QMI_IF_DEREG_CM()

  DESCRIPTION
    Deregister the CM SS & PH event notification

  PARAMETERS
    None

  RETURN VALUE
    TRUE  - Deregistration succeeded
    FALSE - Deregistration failed

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
extern boolean qmi_if_dereg_cm
(
  void
);

/*===========================================================================
  FUNCTION QMI_IF_GET_WCDMA_DEFAULT_CHANNEL_RATE()

  DESCRIPTION
    Get the WCDMA including HSDPA/HSUPA default/maximum data channel rate 
    from RRC Api.

  PARAMETERS
    tx_rate      : Tx rate to be reported
    rx_rate      : Rx rate to be reported

  RETURN VALUE
    none

  DEPENDENCIES
    none

  SIDE EFFECTS
    None
===========================================================================*/
 extern void qmi_if_get_wcdma_default_channel_rate
(
  uint32 *            tx_rate, 
  uint32 *            rx_rate 
);

/*===========================================================================
  FUNCTION QMI_IF_GET_TDSCDMA_DEFAULT_CHANNEL_RATE()

  DESCRIPTION
    Get the TDSCDMA default/maximum data channel rate 

  PARAMETERS
    tx_rate      : Tx rate to be reported
    rx_rate      : Rx rate to be reported

  RETURN VALUE
    none

  DEPENDENCIES
    none

  SIDE EFFECTS
    None
===========================================================================*/
 extern void qmi_if_get_tdscdma_default_channel_rate
(
  uint32 *            tx_rate, 
  uint32 *            rx_rate 
);

/*===========================================================================
  FUNCTION QMI_IF_GET_CURRENT_CHANNEL_RATE()

  DESCRIPTION
    Get the current data channel rate.

  PARAMETERS
    None

  RETURN VALUE
    none

  DEPENDENCIES
    none

  SIDE EFFECTS
    None
===========================================================================*/
extern void qmi_if_get_current_channel_rate
(
  ps_iface_type *      iface_ptr,
  uint32 *             tx_rate,
  uint32 *             rx_rate
);

/*===========================================================================
  FUNCTION QMI_IF_GET_MAX_CHANNEL_RATE()

  DESCRIPTION
    Get the current serving system's maximum data channel rate.

  PARAMETERS
    None

  RETURN VALUE
    none

  DEPENDENCIES
    none

  SIDE EFFECTS
    None
===========================================================================*/
extern void qmi_if_get_max_channel_rate
(
  ps_iface_type *      iface_ptr,
  uint32 *             tx_rate,
  uint32 *             rx_rate
);

/*===========================================================================
  FUNCTION QMI_IF_GET_POWER_STATE()

  DESCRIPTION
    It returns the current power status and battery level after querying them
    from the external charger module

  PARAMETERS - Output
   power status  : current power status (bitmask)
   battery level : current battery level
   
  RETURN VALUE
    
  DEPENDENCIES
    none

  SIDE EFFECTS
    None
===========================================================================*/
extern void qmi_if_get_power_state
(
  uint8 * power_status,
  uint8 * battery_lvl
);

/*===========================================================================
FUNCTION QMI_IF_CHECK_SPC()

  DESCRIPTION
  	This function checks the SPC and OTKSL for r/w to NV items like ACCOLC 

  PARAMETERS - spc code, bCheck_otksl - if need to check for OTKSL if SPC fails 

  RETURN VALUE
    SPC Check Result 

  DEPENDENCIES
    none

  SIDE EFFECTS
    None
===========================================================================*/
extern qmi_if_spc_result_e_type qmi_if_check_spc
(
  uint8 * spc_code, boolean bCheck_otksl
);

/*===========================================================================
  FUNCTION QMI_IF_PROCESS_CM_SS_EVENT()

  DESCRIPTION
    Process the CM SS event generated and call coresponding functions in
    QMI services to generate indications
  PARAMETERS
    ss_event  :  CM SS event
    ss_info   :  serving system info obtained from CM SS event
    asubs_id  :  On which subscription event was received
    
  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
extern void qmi_if_process_cm_ss_event
(
  cm_ss_event_e_type      ss_event,
  qmi_if_cm_ss_info_type  ss_info,
  sys_modem_as_id_e_type  asubs_id
);

/*===========================================================================
  FUNCTION QMI_IF_PROCESS_CM_PH_EVENT()

  DESCRIPTION
    Process the CM SS event generated and call coresponding functions in
    QMI services to generate indications

  PARAMETERS
    ph_event  :  CM PH event
    ph_info   :  Phone info obtained from CM PH event
    
  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
extern void qmi_if_process_cm_ph_event
(
  cm_ph_event_e_type      ph_event,
  qmi_if_cm_ph_info_type  ph_info    
);

/*===========================================================================
  FUNCTION QMI_IF_PROCESS_CM_CALL_EVENT()

  DESCRIPTION
    Process the CM call event generated and call coresponding functions in
    QMI services to generate indications

  PARAMETERS
    call_event  :  CM call event
    call_info   :  call info obtained from CM call event
    
  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
extern void qmi_if_process_cm_call_event
(
  cm_call_event_e_type      call_event,
  qmi_if_cm_call_info_type  call_info    
);

/*===========================================================================
  FUNCTION QMI_IF_PROCESS_OTKSL_FLAG_UPDATE()

  DESCRIPTION
    Updates the otksl flag NV item in DCC task

  PARAMETERS
    otksl_flag  :  otksl flag value
    
  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
extern void qmi_if_process_otksl_flag_update
(
  uint8      otksl_flag
);

#ifdef FEATURE_MMGSDI_SESSION_LIB
/*===========================================================================
  FUNCTION QMI_IF_PROCESS_MMGSDI_OPERATION_COMPLETE()

  DESCRIPTION
    The demultiplexing routine to send out MMGSDI confirmations to the
    appropriate clients.
    
  PARAMETERS
    cnf_type : Type of MMGSDI confirmation
    cnf_data : Response data

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_if_process_mmgsdi_operation_complete
(
  mmgsdi_cnf_enum_type    cnf_type,
  mmgsdi_cnf_type        *cnf_data
);

/*===========================================================================
  FUNCTION QMI_IF_MMGSDI_RESP_CBACK()

  DESCRIPTION
    Confirmation response callback for MMGSDI read requests

  PARAMETERS
    status  : return status
    cnf     : type of cnf response
    cnf_ptr : response data

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_if_mmgsdi_resp_cback
(
  mmgsdi_return_enum_type  status, 
  mmgsdi_cnf_enum_type     cnf,
  const mmgsdi_cnf_type *  cnf_ptr
);

/*===========================================================================
  FUNCTION QMI_IF_MMGSDI_EVT_CBACK()

  DESCRIPTION
    Event handler callback for MMGSDI events

  PARAMETERS
    event   : MMGSDI event

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_if_mmgsdi_evt_cback
(
  const mmgsdi_event_data_type *event
);

/*===========================================================================
  FUNCTION QMI_IF_MAP_MMGSDI_STATUS_TO_ERRVAL

  DESCRIPTION
    To map each MMGSDI error value to the corresponding QMI error code
    
  PARAMETERS
    mmgsdi_status : MMGSDI error value
    line : Line number where MMGSDI status is not success

  RETURN VALUE
    errval : QMI-specific error code

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_e_type qmi_if_map_mmgsdi_status_to_errval
(
  mmgsdi_return_enum_type  mmgsdi_status,
  unsigned int line
);

#endif /* FEATURE_MMGSDI_SESSION_LIB */

/*===========================================================================
  FUNCTION  QMI_IF_EDGE_SUPPORTED

  DESCRIPTION
    Determines whether EDGE is configured as enabled or disabled

  PARAMETERS
    None

  RETURN VALUE
    True if EGPRS is supported by the current configuration, false otherwise

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
extern boolean qmi_if_edge_supported
(
  void
);

/*===========================================================================
FUNCTION  QMI_IF_INFO_GET_PTR

DESCRIPTION
  Returns the ptr to qmi_if_info.

DEPENDENCIES
  None

RETURNS
  Ptr to global variable qmi_if_info.

SIDE EFFECTS
  None

===========================================================================*/
qmi_if_info_type * qmi_if_info_get_ptr
(
  void
);
/*===========================================================================
FUNCTION  QMI_IF_GET_DEFAULT_SUB_ID

DESCRIPTION
  to get the default data subscription id.

DEPENDENCIES
  None

RETURNS
 Returns the default data subscription id.

SIDE EFFECTS
  None

===========================================================================*/
qmi_if_asubs_id_type qmi_if_get_default_sub_id
(
  void
);

#endif /* DS_QMI_IF_H */
