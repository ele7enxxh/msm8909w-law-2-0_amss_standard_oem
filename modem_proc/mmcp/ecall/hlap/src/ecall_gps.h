#ifndef ECALL_GPS_H
#define ECALL_GPS_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                               E C A L L _ G P S 
                            H E A D E R   F I L E

GENERAL DESCRIPTION 
This file contains the GPS APIs needed by the Ecall module. These APIs internally call the GPS PDAPIs.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2009-2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
 
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/ecall/hlap/src/ecall_gps.h#1 $
$DateTime: 2016/12/13 08:00:05 $
$Author: mplcsds1 $
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                         I N C L U D E   F I L E S

===========================================================================*/
#include "comdef.h"
#include "customer.h"

#if (defined(FEATURE_ECALL_APP) && defined(FEATURE_CGPS))

#include "ecall_msd_defines.h"

typedef struct gps_data
{
  ecall_pos_type  latitude;
  ecall_pos_type  longitude;
  uint64_t        timestamp;
  uint8_t         vehicledirection;
  boolean         positionconfidence;
}ecall_gps_data_type;


/*===========================================================================
FUNCTION ECALL_GETGPSDATA

DESCRIPTION
  This function populates a GPS position structure for ecall, for MSD.
 
 
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ecall_getgpsdata
(
  ecall_msd_structure_s_type *ecall_msd
);

/*===========================================================================
FUNCTION ECALL_GPS_PROCESS_GPS_ERROR_RECVD

DESCRIPTION
  This function sets position confidence to False (Low Confidence)
  if GPS Timer Expires or GPS engine returns error
 
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ecall_gps_process_gps_error_recvd
(
  void
);

/*===========================================================================
FUNCTION ECALL_GETPOSITION

DESCRIPTION
  This function triggers pdsm_get_position() during an application tracking 
  session.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
===========================================================================*/
void ecall_getposition 
(
  uint32 gps_timeout_ms
);

/*===========================================================================
FUNCTION       ECALL_END_POSITION_SESSION

DESCRIPTION
  This function will send the QMI_LOC_STOP_REQ_V02 command to QMI_LOC
  or pdsm_end_session() to PDAPI to stop the location session.

DEPENDENCIES 
  None

RETURN VALUE 
  None

SIDE EFFECTS 
  None
===========================================================================*/
void ecall_end_position_session
(
  void
);

/*===========================================================================

          Q M I - L O C   F U N C T I O N   D E C L A R A T I O N S

===========================================================================*/
#ifdef FEATURE_ECALL_HAS_QMI_LOC

#include "location_service_v02.h"
#include "qmi_client.h"

/*===========================================================================
FUNCTION ECALL_SAVE_POSITION

DESCRIPTION
  This function tstores the GPS position in the GPS structure.

DEPENDENCIES
  FEATURE_ECALL_HAS_QMI_LOC

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ecall_save_position
(
  boolean         qmiloc_latitude_valid,
  boolean         qmiloc_longitude_valid,
  boolean         qmiloc_timestamp_valid,
  boolean         qmiloc_vehicledirection_valid,
  boolean         qmiloc_horConfidence_valid,
  boolean         qmiloc_horUncCircular_valid,
  double          qmiloc_latitude,
  double          qmiloc_longitude,
  uint64_t        qmiloc_timestamp,
  float           qmiloc_vehicledirection,
  uint8_t         qmiloc_horConfidence,
  float           qmiloc_horUncCircular
);

/*===========================================================================
FUNCTION ECALL_GPS_INIT

DESCRIPTION 
  Initializes the QMI-LOC client during power-up and register
  with QMI LOC service for system information indication

DEPENDENCIES 
  FEATURE_ECALL_HAS_QMI_LOC 

RETURN VALUE 
  None. 

SIDE EFFECTS 
  None. 
===========================================================================*/
boolean ecall_gps_init
(
  void
);

/*===========================================================================
FUNCTION  ECALL_QMI_LOC_CLIENT_RECV_LOC_START_RESP

DESCRIPTION 
  Handle callbacks response of QMI_LOC_START_REQ_V02 and
  QMI_LOC_REG_EVENTS_REQ_V02.

DEPENDENCIES 
  FEATURE_ECALL_HAS_QMI_LOC

RETURN VALUE 
  None 

SIDE EFFECTS 
  Release the memory assigned for response
===========================================================================*/
void ecall_qmi_loc_client_recv_loc_start_resp
(
  qmi_client_type                user_handle,
  unsigned int                   msg_id,
  void                          *resp_c_struct,
  unsigned int                   resp_c_struct_len,
  void                          *resp_cb_data,
  qmi_client_error_type          transp_err
);

/*==========================================================================
FUNCTION ECALL_QMI_LOC_CLIENT_PROCESS_SIGNAL

DESCRIPTION 
  Funtion to process LOC service notify signal. This function is
  used to process signal sent to ecall_ivs_task by QCCI notifying LOC
  service. 
    
DEPENDENCIES
  FEATURE_ECALL_HAS_QMI_LOC

RETURN VALUE
  None

SIDE EFFECTS 
  None 
===========================================================================*/
void ecall_qmi_loc_client_process_signal
(
  void
);

#else /* PDAPI */

/*===========================================================================

          P D A P I   F U N C T I O N   D E C L A R A T I O N S

===========================================================================*/

#include "pdapi.h"

#define ECALL_INIT_FIX_TIMEOUT 70 /* an arbitraty initial timeout value */
#define ECALL_INIT_FIX_ACCURACY_THRESHOLD 100; /* an arbitraty initial timeout value */
#define ECALL_PD_DEFAULT_WAIT_TIMEOUT  60000 // 1 minute

/*===========================================================================/
FUNCTION ECALL_GPS_INIT

DESCRIPTION
  This function is used to initialize a client object.
     To receive notification of PD events, a client must do the following as well:
       Register specific PD callbacks pdsm_client_pd_reg() and events
       Activate the client using pdsm_client_act()

DEPENDENCIES
   PDAPI

 PARAMETERS

RETURN VALUE
  None

SIDE EFFECTS

==============================================================================*/
boolean ecall_gps_init
(
  void
);

/*===========================================================================
FUNCTION ECALL_GPS_RELEASE

DESCRIPTION
   Release the Ecall client object.

  This function should be called if a client doesn't need PDSM anymore.
  This will release the client object.
  If the Ecall client needs to use PDSM again, it should PD
  pdsm_client_init function to establish connection.

DEPENDENCIES
  PDAPI

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ecall_gps_release
(
  void
);

/*===========================================================================
FUNCTION ECALL_SAVE_POSITION

DESCRIPTION
  This function tstores the GPS position in the GPS structure.

DEPENDENCIES
  PDAPI

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void ecall_save_position 
(
  pdsm_pd_data_type  ecall_gps_data
);

#endif /* FEATURE_ECALL_HAS_QMI_LOC */

#else

void ecall_gps_dummy_func(void);

#endif //End Featurisation

#endif /* ECALL_GPS_H */
