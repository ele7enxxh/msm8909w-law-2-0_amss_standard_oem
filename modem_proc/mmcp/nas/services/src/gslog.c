/*===========================================================================
      COPYRIGHT INFORMATION

Copyright (c) 2001-2003,2007 Qualcomm Technologies, Incorporated and its licensors.
All Rights Reserved.  QUALCOMM Proprietary.  Export of this technology
or software is regulated by the U.S. Government. Diversion contrary to
U.S. law prohibited.

      EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/nas/vcs/gslog.c_v   1.10   18 Apr 2002 17:18:16   amcintos  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/services/src/gslog.c#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/17/12   gps      Created EFS item for SS-Screening_Indicator
03/18/09  abhi   Removing variables used only by GRR, they will now be declared in 
                         GRR files.
11/13/07   AR      Replace wrapper FEATURE_CS_DATA_NOT_SUPPORTED with
                     FEATURE_DATA_GCSD for consistency.
05/06/03   AB      Conditionally turned on GSM AMR speech version with 
                     FEATURE_GSM_AMR   
03/21/03   AB      Added new conditional FEATURE_GSM_HALF_RATE around ms_capability.
03/20/03   ks      Changed the sm_capability_gprs to TRUE as GPRS SMS is supporeted
02/27/03   AB      Changed the MS capability to DUAL_RATE_SUPPORT.
08/29/02   JC      Removed definition of default_power[], not used anywhere.
08/27/02   rmc     Fixed supported values for various channel modes
04/10/02   ATM     Added Front-End Filter to CFA
02/26/02   ATM     Standardized GS_SEND_MESSAGE / GS_READ_MESSAGES
11/29/01   ATM     Modifications for CFA
7/12/01    CD      Deleted unnecessary MN header files and added mn.h

7/13/01    CD      Replaced GS_QUEUE_MMI by GS_QUEUE_CM and GS_QUEUE_UASMS
                   
8/15/01    AB      Renamed all MMI references to CM.
8/23/01    TS      Removed uneeded include files.
09/18/01   sbk     GPRS related system constants defined
10/21/01   sbk     revision level is set to WCDMA_GSM_24008 

===========================================================================*/


/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "mmcp_variation.h"
#include "gs_v.h"
#include "ms.h"
#include "aconfig.h"
#include "ghdi_exp.h"
#include "ghdi_exp_int.h"
#include "cfa_v.h"


/* Defines and Constants */


/* Static Functions      */

/* Global Data            */

char *queue_name_table[GS_MAX_QUEUE + 8] =
{
   "00",
   "L1",
   "L2",
   "RR",
   "MM",
   "CM",
   "UASMS",
   "MN_CM",
   "REG",
   "9",
   "TIMERS",
   "11",
   "12",
   "13",
   "14",
   "GHDI",
   "16",
   "17",
   "DS",
   "19",
   "20",
   "GSDI_TASK",
   "GSDI_LIB",
   "CB",
   "SERIAL_OUT",
   "SERIAL_IN",
   "DIAG",
   "GHDI_SIM",
   "SIM_POLL",
   "SM",
   "RRC",
   "31",
   "32",
   "33",
   "34"
};

byte sm_capability              = PP_SMS_SUPPORTED;
byte ps_capability              = PS_CAPABILITY_PRESENT;
byte revision_level             = WCDMA_GSM_24008;
byte ES_IND                     = CECS_IMPLEMENTED;

boolean SIM_3_volt_supported    = TRUE;
boolean SIM_5_volt_supported    = FALSE;

boolean sm_capability_gprs      = TRUE;
boolean ucs2_support_default_preferred = TRUE;
boolean solsa_supported = FALSE;

#ifdef FEATURE_GSM_GPRS_PFC
boolean PFC_feature_mode_supported = TRUE;
#else
boolean PFC_feature_mode_supported = FALSE;
#endif

boolean VGCS_supported = FALSE;
boolean VBS_supported  = FALSE;

boolean compact_interference_caps = FALSE;
boolean UMTS_FDD_RAT_supported    = TRUE;
boolean UMTS_TDD_RAT_supported    = FALSE;
boolean CDMA2000_RAT_supported    = FALSE;


const char * dai_mode_name[] =
{
   "DAI_NORMAL_OPERATION",
   "DAI_SPEECH_DECODER_TEST",
   "DAI_SPEECH_ENCODER_TEST",
   "DAI_ACOUSTIC_TEST",
   "DAI_????"
};

const char * loopback_type_name[] =
{
   "NO_LOOPBACK",
   "LOOPBACK_TYPE_A",
   "LOOPBACK_TYPE_B",
   "LOOPBACK_TYPE_C",
   "LOOPBACK_TYPE_D",
   "LOOPBACK_TYPE_E",
   "LOOPBACK_TYPE_F",
   "LOOPBACK_TYPE_????"
};


/*===========================================================================
                         FUNCTION DECLARATIONS
===========================================================================*/
/*===========================================================================

FUNCTION  gs_log_message

DESCRIPTION
  

RETURN VALUE
  none

===========================================================================*/
void gs_log_message( IMH_T *          msg_ptr,
                     gs_queue_id_T    queue_id,
                     word             message_length )
{
    /* Log using CFA packet */
    cfa_log_packet( queue_id, message_length, (byte*)msg_ptr );
}




