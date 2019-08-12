#ifndef CM_LOG_H
#define CM_LOG_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                            CM Logging Facilities

GENERAL DESCRIPTION
   This module contains functions for logging CM packets.

Copyright (c) 2002  - 2009 by Qualcomm Technologies Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/cmlog.h#1 $

when       who      what, where, why
--------   ---      ---------------------------------------------------------
04/13/11   rm       Moving LOG_CM_CALL_EVENT_C to cmlog.h 
05/06/10   rm      Make srv_status sent to Diag consistent with what is sent
                   in ss event 
05/07/09   sv      Correct lint error
02/02/09   ks      Adding support for QDSP 6 based compiler
01/29/09   fj      Remove FEATURE_HDR_HYBRID for call events
03/17/08   am      Added two diag commands to support 
                   commands for CM_CALL_CMD_MODIFY_REQ / RSP
01/03/08   ka      Adding handset based plus code dialing feature.
04/15/07   rp       Removing FEATURE_MMGPS
11/17/06   jqi     Added support for HDR active protocol revision information.
11/09/06   rp      Added support for SS req/response packet
                   CM_GET_CELL_SERVICE_IND.
04/19/06   jqi     Added diag subsystem command(select system preference 
                   command2) to process 64-bit band preference. 
03/10/06   pk       Added support for IMSI and MCC for IS 95 systems. Changed
                    the changed fields of the ss log packet to 64 bits
01/05/06   pk       Modified the SS log packet structure. Replaced constants
                    with hard coded values, repalced boolean with uint8
===========================================================================*/

#include "comdef.h"
#include "log.h"

/*
** Data structure used to log CM call events
** We'll log current CM call event and what's in cm_mm_call_info_s_type
** (except for mode_info field)
*/
LOG_RECORD_DEFINE(LOG_CM_CALL_EVENT_C)
  
  uint8                  version;
    /* Version of the packet (CM_LOG_CALL_EVENT_LOG_PACKET_VERSION)
    */ 

  int32                  call_event;
    /* Call event, as defined by cm_call_event_e_type
    */

  int32                  call_client_id;
    /* Call client - originating/answering client (cm_client_id_type)
    */

  int32                  call_id;
    /* call ID of the call (cm_call_id_type) 
    */

  int32                  call_state;
    /* Call state - idle, orig, incom, conv (cm_call_state_e_type)
    */

  int32                  call_type;
    /* Call type - VOICE, DATA, OTASP, etc (cm_call_type_e_type) 
    */

  PACKED struct PACKED_POST
  {

    uint8                buf[ 64 ];
      /* Number buffer
      ** Hardcoded to the value of CM_MAX_NUMBER_CHARS
      ** at the time log packet was declared. 
      ** This is intentional as otherwise changes to the constant would
      ** change byte ordering in the log packet.           
      */

    uint8                len;
      /* Length of number stored in num_buf 
      */

    int32                digit_mode;
      /* Origination digit mode (cm_digit_mode_e_type)
      */

    uint8                number_type;
      /* Number type 
      */

    uint8                number_plan;
      /* Numbering Plan 
      */

    uint8                pi;
      /* Presentation Indicator (cm_npi_type)
      */

    uint8                si;
      /* Screening Indicator (cm_si_type) 
      */

    PACKED struct PACKED_POST
    {
      boolean            extension_bit;
        /* The extension bit 
        */

      uint8              subadd_type;
        /* Type of subaddress - NSAP/USER 
        */

      uint8              odd_even_ind;
        /* The indicator of even/odd bits - EVEN/ODD 
        */

      uint8              num_char;
        /* Number of characters in subaddress array 
        */

      uint8              chari[ 21 ];
        /* 
        ** Array of subaddress digits 
        ** Hardcoded to the value of CM_MAX_SUBADDRESS_DIGITS
        ** at the time log packet was declared. 
        ** This is intentional as otherwise changes to the constant would
        ** change byte ordering in the log packet.          
        */

    } subaddr;
      /* Subaddress (cm_subaddress_s_type) 
      */

  } num;
    /* 
    ** Number buffer (cm_num_s_type)
    ** 
    ** Contents: 
    **
    ** CM_CALL_EVENT_ORIG      - contains dialed digits string.
    ** CM_CALL_EVENT_FLASH     - contains dialed digits string.
    ** CM_CALL_EVENT_INCOM     - contains caller ID string.
    ** CM_CALL_EVENT_CALLER_ID - contains caller ID string. 
    */

  PACKED struct PACKED_POST
  {

    PACKED struct PACKED_POST
    {

      uint8              buf[ 64 ];
        /* Number buffer 
        ** Hardcoded to the value of CM_MAX_NUMBER_CHARS
        ** at the time log packet was declared. 
        ** This is intentional as otherwise changes to the constant would
        ** change byte ordering in the log packet.          
        */

      uint8              len;
        /* Length of number stored in num_buf 
        */

      int32              digit_mode;
       /* Origination digit mode (cm_digit_mode_e_type) 
       */

      uint8              number_type;
        /* Number type 
        */

      uint8              number_plan;
        /* Numbering Plan 
        */

      uint8              pi;
        /* Presentation Indicator (cm_npi_type)
        */

      uint8              si;
        /* Screening Indicator (cm_si_type) 
        */

      PACKED struct PACKED_POST
      {
        boolean          extension_bit;
          /* The extension bit 
          */

        uint8            subadd_type;
          /* Type of subaddress - NSAP/USER 
          */

        uint8            odd_even_ind;
          /* The indicator of even/odd bits - EVEN/ODD 
          */

        uint8            num_char;
          /* Number of characters in subaddress array 
          */

        uint8            chari[ 21 ];
          /* 
          ** Array of subaddress digits 
          ** Hardcoded to the value of CM_MAX_SUBADDRESS_DIGITS
          ** at the time log packet was declared. 
          ** This is intentional as otherwise changes to the constant would
          ** change byte ordering in the log packet.              
          */

      }  subaddr;
        /* Subaddress (cm_subaddress_s_type)
        */

    } num;
      /* Number that is redirecting (cm_num_s_type) 
      */

    int32                redirecting_reason;
      /* 
      ** Optional param -- redirecting reason
      ** REDIRECTING_REASON_UNKNOWN if no included 
      ** (cm_redirecting_reason_e_type).
      */

  } redirecting_number;
    /* Number buffer (cm_redirecting_num_s_type)
    */

  PACKED struct PACKED_POST
  {

    uint8                buf[ 64 ];
      /* 
      ** Alpha buffer 
      ** Hardcoded to the value of CM_MAX_ALPHA_TAG_CHARS
      ** at the time log packet was declared. 
      ** This is intentional as otherwise changes to the constant would
      ** change byte ordering in the log packet.          
      */

    uint8                len;
      /* Length of alpha tag stored in alpha_buf 
      */

  } alpha;
    /* 
    ** Alpha tag to be displayed along with dialed address
    ** of an originated call (cm_alpha_s_type).
    */

  PACKED struct PACKED_POST
  {

    boolean              is_signal_info_avail;
      /* Indicate whether signal information is available.
      **
      ** NOTE! if this variable is FALSE, the values found in other
      ** will be set to CM_LOG_PACKET_CONTENTS_NOT_DEFINED 
      */

    uint8                signal_type;
      /* Signal type - Tone Signal, ISDN Alerting or IS-54B Alerting 
      */

    uint8                alert_pitch;
      /* Alert pitch - Medium, High or Low 
      */

    uint8                signal;
      /* Indicate which Tone signals, ISDN Alerting or IS_54B Alerting 
      */

  } signal;
    /* 
    ** Alert signal information to be played for the incoming call 
    ** (cm_signal_s_type)
    */

  int32                  end_status;
    /* 
    ** Call end status (i.e. reason for ending a call in origination,
    ** incoming or conversation state) 
    ** (cm_call_end_e_type)
    */

  uint8                  num_call_ids_ended;
    /* number of calls ended 
    */

  PACKED struct PACKED_POST
  {

    uint8                call_id;
      /* Call ID to end (cm_call_id_type)
      */

    int32                info_type;
      /* Mode type (cm_call_mode_info_e_type) 
      */

    PACKED union PACKED_POST
    {

      PACKED struct PACKED_POST
      {

        uint8            reserved;
          /* empty for now 
          */

      } gw_cs_end;
        /* 
        ** GSM/WCDMA circuit switched call end parameters 
        ** (cm_gw_cs_end_params_s_type)
        */

      PACKED struct PACKED_POST
      {

        PACKED struct PACKED_POST
        {
          boolean        valid;
          byte           nsapi;
        } nsapi;
          /* NSAPI (nsapi_T) 
          */

        PACKED struct PACKED_POST
        {
          boolean        valid;
          boolean        ti_flag;
        } tear_down_ind;
          /* 
          ** Tear down indicator (ti_T)              
          */

        uint8            cause;
          /* 
          ** Cause for ending the call (sm_status_T)
          */

      } gw_ps_end;
        /* 
        ** GSM/WCDMA packet switched call end parameters 
        ** (cm_gw_ps_end_params_s_type)
        */

      PACKED struct PACKED_POST
      {
        boolean          end_reason_included;
          /* End reason included or not 
          */

        uint8            end_reason;
          /* End reson 
          */

      } cdma_end;
        /* CDMA call end parameters (cm_cdma_end_params_s_type)
        */

    } end_params;
      /* mode specific end parameters (cm_end_params_u_type) 
      */

  } end_params;
    /* call end information (cm_end_params_s_type) 
    */

  int32                  sys_mode;
    /* Indicates the system on which the call is made (sys_sys_mode_e_type) 
    */

  dword                  call_connect_time;
    /* The time the call was connected.  The units is seconds since
    ** the phone has powered on (clock uptime).
    **/

  dword                  call_end_time;
    /* The time the call was ended.  The units is seconds since
    ** the phone has powered on (clock uptime).
    **/

  uint32                 bytes_received;
    /* The number of bytes received during the call (PS data call only).
    ** This field is populated after the call has ended.
    */

  uint32                 bytes_transmitted;
    /* The number of bytes transmitted during the call (PS data call only).
    ** This field is populated after the call has ended.
    */


  int32                  line;
    /* What line is being used for this call (FEATURE_ALS)
    ** (cm_als_line_e_type)
    */

  int32                  requesting_client_id;
    /* Requesting client (cm_client_id_type)
    */

  int32                  msm_id;
    /* 
    ** Identifies the MSM which generated this event.
    ** Only set when FEATURE_UMTS_1X_HANDOVER_1XMSM or
    ** FEATURE_UMTS_1X_HANDOVER_UMTSMSM is defined
    ** otherwise set to 0
    ** (sys_msm_id_e_type)
    */

  int32                 direction;
    /* Holds direction of the current call.
    ** It can be Mobile originated or Mobile
    ** Terminated
    ** (cm_call_direction_e_type)
    */

  int32                 otasp_status;
    /* 
    ** OTASP status passed from 1xCP to CM,
    ** during OTASP/OTAPA call 
    ** (cm_otasp_status_e_type)
    */

  byte                  asubs_id;
    /**< Holds the active subscription id on which this call is originated 
    ** or recieved. 
    */

  PACKED struct PACKED_POST
  {
      boolean             call_ss_success;
      /* Flag to indicate if multiple call management is successful. 
      */

      int32                info_type;
      /* Mode type (cm_call_mode_info_e_type) 
      */

      byte                 size;
      /* Number of active calls
      */

      byte                 table[14];
      /* Active call list
      */     

  }active_call_info;



LOG_RECORD_END

/***************************************************************************
                             Function definitions
****************************************************************************/


/*===========================================================================
FUNCTION  cm_get_call_state

DESCRIPTION
  This function should be called to retrieve the call state.
  It can be used by DIAG, for example, to fill status packet responses.

RETURN VALUE
  byte -  The call state

SIDE EFFECTS
  None

===========================================================================*/
byte cmlog_get_call_state ( void );

/*===========================================================================
FUNCTION  cmlog_get_srv_status

DESCRIPTION
  This function should be called to retrieve the service status.
  It can be used by DIAG, for example, to fill status packet responses.

RETURN VALUE
  byte -  The system mode

SIDE EFFECTS
  None

===========================================================================*/
byte cmlog_get_srv_status ( void );

/*===========================================================================
FUNCTION  cmlog_get_operating_mode

DESCRIPTION
  This function should be called to retrieve the operating mode.
  It can be used by DIAG, for example, to fill status packet responses.

RETURN VALUE
  byte -  The operating mode

SIDE EFFECTS
  None

===========================================================================*/
byte cmlog_get_operating_mode ( void );

/*===========================================================================
FUNCTION  cmlog_get_system_mode

DESCRIPTION
  This function should be called to retrieve the system mode.
  It can be used by DIAG, for example, to fill status packet responses.

RETURN VALUE
  byte -  The system mode

SIDE EFFECTS
  None

===========================================================================*/
byte cmlog_get_system_mode ( void );

#endif /* CM_LOG_H */
