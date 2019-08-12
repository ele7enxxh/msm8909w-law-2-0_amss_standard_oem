/*===========================================================================

              UCSD Bearer Capability Implementation File

DESCRIPTION
  This file contains the implementation of the UMTS Circuit Switched Data
  Bearer Capability functions

Copyright (c) 2003 - 2014 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/



/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/csd/src/dsucsdbc.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/11/13   SS      TD coexistence on DSDS.
01/13/12   SA      Added support for CSVT for CSFB targets.
01/10/12   ND      Invalid calling party subaddress in call setup.
11/11/11   ND      Feature cleanup.
09/29/11   SA      Added support for TD-SCDMA.
08/20/11   TTV     GSDI to MMGSDI_SESSION api migration changes.
04/01/11   TTV     Support for Q6 free floating.
03/23/11   SN      Globals cleanup for free floating of tasks.
11/18/10   TTV     Added support for DSDS.
09/29/09   SA      Fixed KW errors.
03/07/09   MS      CMI Integration fixes
03/04/09   SA      AU level CMI modifications.
11/20/07   AR      Permit multimedia call in GSM per 3GPP TS24.008 CR962.
02/12/07   AR      Ensure fallback-only repeat_indicator handled correctly.
11/28/06   AR      Remove routing NV item from VT call type determination.
09/18/06   AR      Add support for bearer swap/strip for SCUDIF calls.
09/05/06   AR      Added routine ds_ucsd_validate_repeat_indicator.
06/21/06   AR      Add test for V.110 rate adaption for FTM negotiation.
03/16/06   AR      Add Fax detection to determine call type routine.
02/15/06   AR      Remove LLC initializtion for voice calls.
11/21/05   AR      Lint corrections.
09/15/05   AR      Add support for service change & fallback.
06/03/05   AR      Change subaddress query record number.
05/12/05   AR      Ensure WAIUR set undefined for MT sync/transparent call.
04/28/05   AR      Add support for exported ATCOP +CV120 parameters.
12/31/04   AR      For MT call set WAIUR to 9600 if FNUR is set likewise.
12/22/04   AR      Do not negotiate incoming UMTS sync/trans calls to 64k.
12/17/04   AR      Add explicit flag for use of LLC in SETUP message.
                   Adjust LLC structure for synchronous data calls. 
11/08/04   AR      Add support for Frame Tunneling Mode.
11/02/04   AR      Ensure assignmnet of ACC,MNTC,WAIUR for MT UMTS calls.
09/20/04   AR      Use call_details bearer capability structure for call
                   confirmation validation.  Make V.120 LLC consistent 
                   with BC IE.
08/30/04   AR      If modem type is autobaud, set FNUR to not applicable.
08/09/04   AR      Ensure LLC specified for UDI calls.
07/06/04   AR      Enable compression only for non-transparent calls.
06/17/04   AR      Fix handing of asynch GSDI results.
05/24/04   AR      Migrate GSDI file reads to asynchronous interface.
05/17/04   AR      Add assignmnet of ACC & MNTC and refine selection of
                   FNUR for GSM MT calls.
05/03/04   AR      Permit V.42bis compression in WCDMA mode.
03/30/04   TMR     Added orig_params->call_control_completed field 
                   initialization to ds_ucsd_bc_init_orig_params()
02/19/04   dwp     Mods for VT BCIE/LLC issues seen in field.
01/14/04   AR      Negotiate non-transparent connection element if network
                   gives UE choice on MT call.  Reject NT fax on MT call.
                   Removed defaulting of user rate when remote requests one
                   not supported; should reject call.  Lint cleanup.
08/08/03   TMR     Added additional FAX checks, and negotiating of rate down
                   to 9.6 when a 14.4 FAX call comes in.
08/07/03   TMR     Changed so we always return Acceptable channel codings of 
                   both 9600 and 14400 for WCDMA Async calls.  Also set WAIUR's
                   for 28.8 and 57.6 for MT calls.
06/20/03   dgy     Added support for UMTS CS Data API.
05/22/03   dgy     Negotiated FNUR and WAIUR for incoming WCDMA CS Data calls.
05/16/03   dgy     Added code under FEATURE_GSM_EXT_SPEECH_PREF_LIST to 
                   invalidate GSM speech version in BC-IE.
04/28/03   dgy     Added F3 messages.
03/25/03   dgy     Initial version. Merged GSM and WCDMA CS Data mode-
                   specific handlers. Added support for WCDMA Asynchronous
                   CS Data.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include "ds_3gppi_utils.h"

#if ( defined( FEATURE_DATA_GCSD ) || defined( FEATURE_DATA_WCDMA_CS ) )
#include "data_msg.h"
#ifdef FEATURE_DATA_GCSD_FAX
#include "err.h"
#endif
#include "mn_cm_exp.h"
#include "sys_cnst.h"
#include "sys_stru.h"
#include "sys_type.h"
#include "ghdi_exp.h"
#include "dstaski.h"
#include "dsat_v.h"
#include "dsati.h"
#include "ds3gmgr.h"
#include "ds3gmshif.h"
#include "ds3gcfgmgr.h"
#include "dsucsdi.h"
#include "dsucsdbc.h"
#include "dsucsdappif.h"
#include "dsucsdhdlr.h"
#include "ds3gmmgsdiif.h"
#include "ds_3gpp_nv_manager.h"

#define DS_UCSD_SIM_PADDING               0xFF
#define DS_UCSD_MSISDN_NONALPHA_LEN       14
#define DS_UCSD_PHONE_NUM_POS             0
#define DS_UCSD_PHONE_NUM_LEN             12
#define DS_UCSD_PHONE_NUM_EXT1_POS        13
#define DS_UCSD_EXT1_BODY_LEN             11
#define DS_UCSD_EXT1_BODY_POS             1
#define DS_UCSD_EXT1_TYPE_POS             0
#define DS_UCSD_EXT1_EXT1_POS             12
#define DS_UCSD_EXT1_SUBADDRESS_MASK      0x01
#define DS_UCSD_EXT1_OVERFLOW_MASK        0x02

#define DS_UCSD_MAX_SUB_ADDR_ARRAY_INDEX  DS_SUBSCRIPTION_MAX

#define DS_UCSD_INVALID_MMGSDI_SESSION_ID 0x00000000FFFFFFFF

#define ASSIGN_ARRAY_INDEX(session_type, array_index)\
{\
  if( DS_UCSD_MAX_SUB_ADDR_ARRAY_INDEX == 2 )\
  {\
    if( session_type == MMGSDI_GW_PROV_PRI_SESSION )\
    { \
      array_index = 0; \
    } \
    else \
    { \
      array_index = 1; \
    } \
  }\
  else\
  {\
    array_index = 0;\
  }\
}

#ifdef FEATURE_DUAL_SIM
/* G, W, T,LTE*/
#define DS_UCSD_GSM_ALLOWED             0x0001
#define DS_UCSD_WCDMA_ALLOWED           0x0002
#define DS_UCSD_TDS_ALLOWED             0x0004
#define DS_UCSD_ALL_ALLOWED             0xFFFF
#endif /* FEATURE_DUAL_SIM */

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/*===========================================================================
Define all of the bearer capability data static data structures
===========================================================================*/

/*declare supported options*/

static const uint8    ucsd_supported_bc_information_transfer_capability[] =
{
#ifdef FEATURE_DATA_GCSD_FAX
   ITC_FAX_G3,
#endif

   ITC_UDI,
   ITC_3_1K,
   ITC_OTHER,  /* RDI */
};

static const uint8    ucsd_supported_bc_structure[] =
{
   SERVICE_DATA_UNIT_INTEGRITY,
   UNSTRUCTURED
};


static const uint8    ucsd_supported_bc_duplex_mode[] =
{
   FULL_DUPLEX
};

static const uint8    ucsd_supported_bc_rate_adaption[] =
{
   V110_X30,
   X31_FLAGSTUFFING,
   NO_RATE_ADAPTION,
   OTHER_RATE_ADAPTION
};

static const uint8    ucsd_supported_bc_other_rate_adaption[] =
{
   V120,
   H223_H245
};

static const uint8    ucsd_supported_bc_signalling_access_protocol[] =
{
   I440_450
};



static const uint8    ucsd_supported_bc_number_of_data_bits[] =
{
   _7_BITS_PER_CHAR,
   _8_BITS_PER_CHAR                     /*negotiable*/
};

static const uint8    ucsd_supported_bc_number_of_stop_bits[] =
{
   _1_STOP_BIT,
   _2_STOP_BITS
};                                 /*negotiable*/

static const uint8    ucsd_supported_bc_user_rate[] =
{
/*   USER_RATE_300,
**   USER_RATE_1200,
**   USER_RATE_2400,
**   USER_RATE_4800,
*/
   USER_RATE_9600,
/*   USER_RATE_1200_75, */
   USER_RATE_14400
};

static const uint8    ucsd_supported_bc_parity[] =
{
   NAS_PARITY_NONE
};                 /*negotiable*/

static const uint8    ucsd_supported_bc_connection_element[] =
{
   TRANSPARENT_DATA,
   NON_TRANSPARENT_DATA,
   BOTH_TRANSPARENT_PREF,
   BOTH_NON_TRANSPARENT_PREF
};

/* The user_information_layer_2_protocol (UIL2P) is an optional parameter.
   It is not used for Connection Element = TRANSPARENT_DATA. When CE=Non-Transparent
   the UIL2P may or may not be defined. If it isn't defined, this indicates
   that the link will default to HW flow control (but only during SETUP).
   See 07.01 Annex B
   */
static const uint8    ucsd_supported_bc_user_information_layer_2_protocol[]=
{
   0,                             /* NAV - ie. HW flow control   */
   X25,                           /* X.25 - for FAX calls ? */
   ISO6429,                       /* ISO 6429 */
   CHARACTER_ORIENTED_PROTOCOL    /* COPnoFlCt - No flow control */
};

static const uint8    ucsd_supported_bc_coding_standard[] =
{
   GSM
};

static const uint8    ucsd_supported_bc_transfer_mode[] =
{
   CIRCUIT
};

static const uint8    ucsd_supported_bc_configuration[] =
{
   POINT_TO_POINT
};

static const uint8    ucsd_supported_bc_NIRR[] =
{
   NO_MEANING
};

static const uint8    ucsd_supported_bc_establishment[] =
{
   DEMAND
};

static const uint8    ucsd_supported_bc_user_information_layer_1_protocol[] =
{
   DEFAULT_LAYER_1_PROTOCOL
};

static const uint8    ucsd_supported_bc_negotiation[] =
{
   IN_BAND_NEGOTIATION_NOT_POSSIBLE
};

static const uint8    ucsd_supported_bc_intermediate_rate[] =
{
   _16_KBIT
};

static const uint8    ucsd_supported_bc_NIC_on_Tx[] =
{
   NIC_ON_TX_NOT_REQUIRED
};

static const uint8    ucsd_supported_bc_NIC_on_Rx[] =
{
   NIC_ON_RX_NOT_SUPPORTED
};

/* Phase 2+ supported bearer capabilities */

/* Supported Fixed Network User Rates for GSM */
static const uint8    gcsd_supported_bc_fixed_network_user_rate[] = 
{
   FNUR_UNDEFINED,
   FNUR_NOT_APPLICABLE,
   FNUR_9600,
   FNUR_14400
};

/* Supported Fixed Network User Rates for WCDMA */
static const uint8    wcsd_supported_bc_fixed_network_user_rate[] = 
{
   FNUR_UNDEFINED,
   FNUR_NOT_APPLICABLE,
   FNUR_19200,
   FNUR_28800,
   FNUR_38400,
   FNUR_48K,
   FNUR_56K,
   FNUR_64K
};

static const uint8    ucsd_supported_bc_acceptable_channel_codings[] =
{
   ACC_NONE,
   ACC_14400_ACCEPTABLE,
   ACC_9600_ACCEPTABLE,
   ACC_9600_ACCEPTABLE | ACC_14400_ACCEPTABLE
};

static const uint8    ucsd_supported_bc_maximum_number_of_trafic_channels[] =
{
   MAX_NUMBER_OF_TCH_UNDEFINED,
   MAX_NUMBER_OF_TCH_1
};

/* User Initiated Modification Indication */
static const uint8    ucsd_supported_bc_user_initiated_modification_indication[] = 
{
   /* Support only single-slot calls for now */
   UIMI_UNDEFINED,
   UIMI_NOT_ALLOWED
    
   /* Not supporting in call modification right now 
   ** UIMI_UP_TO_1_TCH
   */

};

/* Supported Wanted Air Interface User Rates for GSM */
static const uint8    gcsd_supported_bc_wanted_air_interface_user_rate[] =
{
   WAIUR_UNDEFINED,
   WAIUR_NOT_APPLICABLE,
   WAIUR_9600,
   WAIUR_14400
};                            

/* Supported Wanted Air Interface User Rates for WCDMA */
static const uint8    wcsd_supported_bc_wanted_air_interface_user_rate[] =
{
   WAIUR_UNDEFINED,
   WAIUR_NOT_APPLICABLE,
   WAIUR_14400,
   WAIUR_28800,
   WAIUR_57600
};                            



/*declare supported low layer compatibility store*/


static const uint8    ucsd_supported_llc_coding_standard[] =
{
   LLC_CCITT
};

static const uint8    ucsd_supported_llc_information_transfer_capability[] =
{
   LLC_SPEECH,
   LLC_UNRESTRICTED_DIGITAL,
   LLC_RESTRICTED_DIGITAL,
   LLC_3_1_KHZ_AUDIO
};

static const uint8    ucsd_supported_llc_transfer_mode[] =
{
   CIRCUIT
};

static const uint8    ucsd_supported_llc_information_transfer_rate[] =
{
   LLC_64KBIT
};

static const uint8    ucsd_supported_llc_user_information_layer_1_protocol[] =
{
   CCITT_V110,
   G711_MU_LAW,
   G711_A_LAW
#ifdef FEATURE_DATA_WCDMA_CS
   ,G7XX_VIDEO
#endif
};

static const uint8    ucsd_supported_llc_sync_async[] =
{
   LLC_SYNCHRONOUS,
   LLC_ASYNCHRONOUS
};

static const uint8    ucsd_supported_llc_negotiation[] =
{
   LLC_IN_BAND_NEGOTIATION_NOT_POSSIBLE
};

static const uint8    ucsd_supported_llc_intermediate_rate[] =
{
   LLC_8KBIT,
   LLC_16KBIT
};

static const uint8    ucsd_supported_llc_NIC_on_Tx[] =
{
   LLC_NIC_ON_TX_NOT_REQUIRED
};

static const uint8    ucsd_supported_llc_NIC_on_Rx[] =
{
   LLC_NIC_ON_RX_NOT_SUPPORTED
};

static const uint8    ucsd_supported_llc_flow_control_on_Tx[] =
{
   LLC_TX_FLOW_CONTROL_NOT_REQUIRED,
   LLC_TX_FLOW_CONTROL_REQUIRED
};

static const uint8    ucsd_supported_llc_flow_control_on_Rx[] =
{
   LLC_RX_FLOW_CONTROL_NOT_SUPPORTED,
   LLC_RX_FLOW_CONTROL_SUPPORTED
};

#ifdef FEATURE_MMGSDI_SESSION_LIB
static dsucsd_sim_addr_info_type   
              ds_ucsd_sim_addr_info[DS_UCSD_MAX_SUB_ADDR_ARRAY_INDEX] = {{NULL}};
#endif /* FEATURE_MMGSDI_SESSION_LIB */

/* IS CSVT call */
boolean ds_ucsd_is_csvt_call = FALSE;

/*===========================================================================

                      LOCAL DATA DECLARATIONS

===========================================================================*/

#define MAX_BCD_PHONE_NUM_LEN         27
#define MAX_SUBADDRESS_LEN            22

#ifdef FEATURE_MMGSDI_SESSION_LIB
static void ds_ucsd_mmgsdi_read_process_result
(
  mmgsdi_cnf_enum_type cnf_type,
  mmgsdi_cnf_type      *cnf_data
);

static void ds_ucsd_get_file_attr_process_result
(
  mmgsdi_cnf_enum_type cnf_type,
  mmgsdi_cnf_type      *cnf_data
);

static void ds_ucsd_mmgsdi_response_cb
(
  mmgsdi_return_enum_type  status, 
  mmgsdi_cnf_enum_type     cnf,
  const mmgsdi_cnf_type *  cnf_ptr
);

static void ds_ucsd_store_sub_addr
(
  mmgsdi_cnf_type      *cnf_data
);

static void ds_ucsd_store_bcd_number
(
  mmgsdi_cnf_type      *cnf_data
);

#ifdef FEATURE_DUAL_SIM
static boolean ds_ucsd_get_calling_party_subaddress
(
  sys_modem_as_id_e_type             sub_id,
  cm_calling_party_subaddress_s_type *sub_address
);
#else
static boolean ds_ucsd_get_calling_party_subaddress
(
  cm_calling_party_subaddress_s_type *sub_address
);
#endif /* FEATURE_DUAL_SIM */
#endif /* FEATURE_MMGSDI_SESSION_LIB */

/*===========================================================================

                          FUNCTION DEFINITIONS

===========================================================================*/


void 
ds_ucsd_print_bc_struct
(
   const cm_bearer_capability_s_type *bc
)
{
   if( NULL == bc )
   {
     DATA_MSG0_ERROR("No bearer capability structure specified");
     return;
   }
   
   DATA_MSG6(MSG_LEGACY_HIGH,"BC Valid=%d,RCR=%d,CodeSt=%d, BC XferMode=%d,ITC=%d,OITC=%d,",
                                   bc->present,
                                   bc->radio_channel_requirement,
                                   bc->coding_standard,
                                   bc->transfer_mode,
                                   bc->information_transfer_capability,
                                   bc->other_ITC);

#ifdef FEATURE_GSM_EFR
   MSG_HIGH("BC SpchVerPref#1=%d,#2=%d,#3=%d",bc->gsm_speech_ver_pref_1,
                                              bc->gsm_speech_ver_pref_2,
                                              bc->gsm_speech_ver_pref_3);
#endif

   DATA_MSG9(MSG_LEGACY_HIGH,"BC Comp=%d,Struct=%d,DplxMode=%d, BC Config=%d,NIRR=%d,Establish=%d,\
                              BC AccessID=%d,RateAdapt=%d,SAP=%d",
                              bc->compression,
                              bc->structure,
                              bc->duplex_mode,
                              bc->configuration,
                              bc->NIRR,
                              bc->establishment,
                              bc->access_id,
                              bc->rate_adaption,
                              bc->signalling_access_protocol);

   DATA_MSG9(MSG_LEGACY_HIGH,"BC Lay1ID=%d,UIL1Proto=%d,SyncAsync=%d, BC StopBits=%d,Neg=%d,DataBits=%d, \
                              BC UserRate=%d,IntRate=%d,Nic_Tx=%d",
                              bc->layer_1_id,
                              bc->user_information_layer_1_protocol,
                              bc->sync_async,
                              bc->number_of_stop_bits,
                              bc->negotiation,
                              bc->number_of_data_bits,
                              bc->user_rate,
                              bc->intermediate_rate,
                              bc->NIC_on_Tx);

   DATA_MSG9(MSG_LEGACY_HIGH,"BC Nic_Rx=%d,Parity=%d,CE=%d, BC MT=%d,OMT=%d,FNUR=%d,\
                              BC AccChnCod=%d,MaxTrfc=%d,UIMI=%d",
                              bc->NIC_on_Rx,
                              bc->parity,
                              bc->connection_element,
                              bc->modem_type,
                              bc->other_modem_type,
                              bc->fixed_network_user_rate,
                              bc->acceptable_channel_codings,
                              bc->maximum_number_of_trafic_channels,
                              bc->user_initiated_modification_indication);

   DATA_MSG6(MSG_LEGACY_HIGH,"BC WAIUR=%d,AsymInd=%d,Lay2ID=%d, BC UIL2Prot=%d,FallBackRate=%d,\
                              BC OtherRateAdapt=%d ",
                              bc->wanted_air_interface_user_rate,
                              bc->asymmetry_indication,
                              bc->layer_2_id,
                              bc->user_information_layer_2_protocol,
                              bc->mt_fall_back_rate,
                              bc->other_rate_adaption);

} /* ds_ucsd_print_bc_struct() */


/*===========================================================================

FUNCTION ds_ucsd_bc_get_subaddress

DESCRIPTION
  This function derives the subaddress from the dial string. 
         
DEPENDENCIES
  None
  
RETURN VALUE
  SUCCESS if the subaddress is successfully derived. FAILURE otherwise.
    
SIDE EFFECTS
  None

===========================================================================*/
boolean
ds_ucsd_bc_get_called_party_subaddress
( 
  const uint8                   *dial_string,    /* ATD dial string */

  uint8                         dial_string_len, /* Length of the dial string */

  cm_called_party_subaddress_s_type *subaddress      /* Place to store Subaddress */
)
{
   /*
   ** CONVERTS A PHONE NUMBER FROM AT-STYLE ASCII INTO
   ** GSM-STYLE subaddress e.g.
   **
   ** dial_string   = "PT+441202318161C*#!W@1234,56,78xxx"
   **
   **  into
   **
   ** GSM_subaddress = IEI:     07
   **                  length:  04
   **                  data:    80.50.21.43
   **
   ** char    action
   ** ----    ------
   ** 0-9     0-9
   ** *       A
   ** #       B
   ** A       C
   ** B       D
   ** C       E
   ** +       0x91 (else 0x81)
   ** ,       ignore
   ** T       ignore
   ** P       ignore
   ** !       ignore
   ** W       ignore
   ** @       start of sub-address
   ** >       return error
   ** I       return error
   ** i       return error
   ** G       return error
   ** g       return error
   ** other   start of next AT command
   */

   uint8    n = 0;
   uint8    AT_chars_read_so_far = 0;
   uint8    sub_len = 0;
   boolean  dial_string_ok = TRUE;
   boolean  dial_string_finished = FALSE;
   boolean  subaddress_found = FALSE;
   
   

   /*
   ** INITIALISE
   ** ----------
   */

   subaddress->present = FALSE;
   subaddress->length = 0;
   memset( subaddress->data, 0xFF, CM_CALLED_PARTY_SUBADDRESS_LENGTH );

   /*
   ** LOOP THROUGH THE dial_string to find subaddress
   ** ---------------------------------------------------
   */
   
   for( n = 0; n < dial_string_len; n++ )
   {
      /*
      ** FIND THE NEXT INTERESTING CHAR
      */

      dial_string_ok = TRUE;
      subaddress_found = FALSE;

      switch( dial_string[n] )
      {
         case '+':
         case '0':
         case '1':
         case '2':
         case '3':
         case '4':
         case '5':
         case '6':
         case '7':
         case '8':
         case '9':
         case '*':
         case '#':
         case 'A':
         case 'B':
         case 'C':
            break;
         case '>':
         case 'I':
         case 'i':
         case 'G':
         case 'g':
            /*
            ** NOT SUPPORTED OPTIONS, RETURN AN ERROR
            */
            dial_string_ok = FALSE;
            break;
         case '@':
            subaddress_found = TRUE;
            break;
         case ',':
         case 'T':
         case ' ':
         case 'P':
         case '!':
         case 'W':
            /*
            ** IGNORE THESE
            */
            break;
         default:
            /*
            ** UNRECOGNISED CHAR, IT MUST BE THE START OF THE NEXT AT COMMAND
            */
            dial_string_finished = TRUE;
            break;
      }

      if( !dial_string_ok || subaddress_found || dial_string_finished )
      {
         /*
         ** A PHONE-NUM HAS BEEN DECODED (POSSIBLY ZERO LENGTH)
         */
         
         break;
      }
   }

   AT_chars_read_so_far = n;


   /*
   ** OK, THAT'S THE PHONE NUMBER DONE, NOW READ THE SUBADDRESS
   ** ---------------------------------------------------------
   */

   if( subaddress_found )
   {
      for( n = AT_chars_read_so_far + 1; n < dial_string_len; n++ )
      {
         /*
         ** FIND THE NEXT INTERESTING CHAR
         */
         
         switch( dial_string[n] )
         {
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
            case '*':
            case '#':
            case 'A':
            case 'B':
            case 'C':
               /*
               ** CONVERT TO IA5 - 32
               */
               if( CM_CALLED_PARTY_SUBADDRESS_LENGTH > (sub_len + 2) )
               {
                 subaddress->data[sub_len + 2] = dial_string[n] - ' ';
                 sub_len++;
               }
               else
               {
                 /*
                 ** LENGTH  INCORRECT, RETURN AN ERROR
                 */
                 dial_string_ok = FALSE;
               }
               break;
            case '>':
            case 'I':
            case 'i':
            case 'G':
            case 'g':
               /*
               ** NOT SUPPORTED OPTIONS, RETURN AN ERROR
               */
               dial_string_ok = FALSE;
               break;
            case ',':
            case 'T':
            case 'P':
            case '!':
            case 'W':
               /*
               ** IGNORE THESE
               */
               break;
            default:
               /*
               ** UNRECOGNISED CHAR, IT MUST BE THE START OF THE NEXT AT COMMAND
               */
               dial_string_finished = TRUE;
               break;
         }

         if( !dial_string_ok || dial_string_finished )
         {
            if( sub_len != 0 )
            {
               /*
               ** A SUBADDRESS HAS BEEN FOUND AND DECODED
               */

               subaddress->present = TRUE;
               subaddress->length = sub_len + 2;
               subaddress->data[0] = SUBADDRESS_NSAP;
               subaddress->data[1] = SUBADDRESS_AFI;
            }
            break;
         }
      }
   }

   return ( subaddress_found );
} /* ds_ucsd_bc_get_called_party_subaddress() */

/*===========================================================================

FUNCTION ds_ucsd_bc_get_llc_data

DESCRIPTION
  This function translates the low level compatibility information
  from the OTA format to the internal structure format.  Note that
  this function was taken unmodified from CCWW (ds_get_llc_data()). 
         
DEPENDENCIES
  None
  
RETURN VALUE
  Number of bytes of OTA format data translated
    
SIDE EFFECTS
  None

===========================================================================*/

int ds_ucsd_bc_get_llc_data
(
   uint8                          *data_ptr,
   uint8                          ie_length,
   ds_ucsd_low_layer_compatibility_T  *ds_low_layer_compatibility
)
{
   int   index;


   /*NB - data_pointer should be a pointer of type byte */

   index = 0;

   ds_low_layer_compatibility->IEI = DSUCSD_NULL_INFO;

   /*decode octet 3 */

   ds_low_layer_compatibility->
      coding_standard = (uint8)(*data_ptr & 0x60) >> 5;
   ds_low_layer_compatibility->information_transfer_capability =
      (uint8)((*data_ptr & 0x1F));

   if ((*data_ptr & 0x80) == 0)
   {
      /*octet 3a exists*/

      data_ptr++;
      index++;

      /*decode octet 3a*/

      ds_low_layer_compatibility->negotiation_indicator =
         (uint8)(*data_ptr & 0x40) >> 6;
   }

   data_ptr++;
   index++;

   /*decode octet 4 */

   ds_low_layer_compatibility->transfer_mode = (uint8)(*data_ptr & 0x60) >> 5;
   ds_low_layer_compatibility->information_transfer_rate =
      (uint8)(*data_ptr & 0x1F);

   if ((*data_ptr & 0x80) == 0)
   {
      /*octet 4a exists*/

      data_ptr++;
      index++;

      /*decode octet 4a*/

      ds_low_layer_compatibility->structure = (uint8)(*data_ptr & 0x70) >> 4;
      ds_low_layer_compatibility->configuration =
         (uint8)(*data_ptr & 0x0C) >> 2;
      ds_low_layer_compatibility->establishment = (uint8)(*data_ptr & 0x03);
   }

   if ((*data_ptr & 0x80) == 0)
   {
      /*octet 4b exists*/

      data_ptr++;
      index++;

      /*decode octet 4b*/

      ds_low_layer_compatibility->symmetry = (uint8)(*data_ptr & 0x60) >> 5;
      ds_low_layer_compatibility->information_transfer_rate_d_to_o =
         (uint8)(*data_ptr & 0x1F);
   }

   data_ptr++;
   index++;

   if (index < ie_length)
   {
      /*more octets exist...*/

      /*decode octet 5*/

      ds_low_layer_compatibility->user_information_layer_1_protocol =
         (uint8)(*data_ptr & 0x1F);

      if ((*data_ptr & 0x80) == 0)
      {
         /*octet 5a exists*/

         data_ptr++;
         index++;

         /*decode octet 5a*/

         ds_low_layer_compatibility->sync_async =
            (uint8)(*data_ptr & 0x40) >> 6;
         ds_low_layer_compatibility->negotiation =
            (uint8)(*data_ptr & 0x20) >> 5;
         ds_low_layer_compatibility->user_rate =
            (uint8)(*data_ptr & 0x1F);
      }

      if ((*data_ptr & 0x80) == 0)
      {
         /*according to ISDN spec ETS 300 102-1, existence of octet 5b depends
          *upon L1 protocol in octet 5 (exists if V.110 or V.120)
          *but GSM spec ETS 300 582 contradicts this! - We will follow GSM spec*/

         data_ptr++;
         index++;

         /*check if protocol is V.110*/

         if (ds_low_layer_compatibility->
             user_information_layer_1_protocol == CCITT_V110)
         {
            /*V.110 info*/

            ds_low_layer_compatibility->
               intermediate_rate = (uint8)(*data_ptr & 0x60) >> 5;
            ds_low_layer_compatibility->
               NIC_on_Tx = (uint8)(*data_ptr & 0x10) >> 4;
            ds_low_layer_compatibility->
               NIC_on_Rx = (uint8)(*data_ptr & 0x08) >> 3;
            ds_low_layer_compatibility->
               flow_control_on_Tx = (uint8)(*data_ptr & 0x04) >> 2;
            ds_low_layer_compatibility->
               flow_control_on_Rx = (uint8)(*data_ptr & 0x02) >> 1;
         }

         /*check if protocol is V.120*/

         else if (ds_low_layer_compatibility->
                  user_information_layer_1_protocol == CCITT_V120)
         {
            /*V.120 info*/

            ds_low_layer_compatibility->rate_adaption_header =
               (uint8)(*data_ptr & 0x40) >> 6;
            ds_low_layer_compatibility->
               multiple_frame_establishment_support =
               (uint8)(*data_ptr & 0x20) >> 5;
            ds_low_layer_compatibility->mode_of_operation =
               (uint8)(*data_ptr & 0x10) >> 4;
            ds_low_layer_compatibility->logical_link_identifier_negotiation =
               (uint8)(*data_ptr & 0x08) >> 3;
            ds_low_layer_compatibility->assignor_assignee =
               (uint8)(*data_ptr & 0x04) >> 2;
            ds_low_layer_compatibility->in_band_out_band_negotiation =
               (uint8)(*data_ptr & 0x02) >> 1;
         }
      }

      if ((*data_ptr & 0x80) == 0)
      {
         /*octet 5c exists*/

         data_ptr++;
         index++;

         ds_low_layer_compatibility->number_of_stop_bits =
            (uint8)(*data_ptr & 0x60) >> 5;
         ds_low_layer_compatibility->number_of_data_bits =
            (uint8)(*data_ptr & 0x18) >> 3;
         ds_low_layer_compatibility->parity = (*data_ptr & 0x07);
      }

      if ((*data_ptr & 0x80) == 0)
      {
         /*octet 5d exists*/

         data_ptr++;
         index++;

         ds_low_layer_compatibility->duplex_mode =
            (uint8)(*data_ptr & 0x40) >> 6;
         ds_low_layer_compatibility->modem_type =
            (uint8)(*data_ptr & 0x3F);
      }

      data_ptr++;
      index++;

      if (index < ie_length)
      {
         /*more octets exist...*/

         /*decode octet 6*/

         ds_low_layer_compatibility->user_information_layer_2_protocol =
            (uint8)(*data_ptr & 0x1F);

         if ((*data_ptr & 0x80) == 0)
         {
            /*octet 6a exists*/

            data_ptr++;
            index++;

            /*decode octet 6a*/

            ds_low_layer_compatibility->
               optional_layer_2_protocol_information = (*data_ptr & 0x7F);
         }

         data_ptr++;
         index++;

         if (index < ie_length)
         {
            /*more octets exist...*/

            /*decode octet 7*/

            ds_low_layer_compatibility->user_information_layer_3_protocol =
               (uint8)(*data_ptr & 0x1F);

            if ((*data_ptr & 0x80) == 0)
            {
               /*octet 7a exists*/

               data_ptr++;
               index++;

               /*decode octet 7a*/

               ds_low_layer_compatibility->
                  optional_layer_3_protocol_information =
                  (uint8)(*data_ptr & 0x7F);
            }

         }
      }
   }

   return index;
} /* ds_ucsd_bc_get_llc_data() */



/*===========================================================================

FUNCTION ds_ucsd_bc_put_llc_data

DESCRIPTION
  This function translates the low level compatibility information
  from the internal structure format to the OTA format. 
         
DEPENDENCIES
  None
  
RETURN VALUE
  Number of bytes of OTA format data generated.
    
SIDE EFFECTS
  None

===========================================================================*/

int ds_ucsd_bc_put_llc_data
(
  uint8                              *data_ptr,
  ds_ucsd_low_layer_compatibility_T  *ds_low_layer_compatibility,
  const ds_ucsd_bearer_service_type  *service_ptr
)
{
   int   index;
   uint8 octet = 0;

   ASSERT( NULL != service_ptr );

   /*initialise count of bytes in LLC*/

   index = 0;

   /*start at octet 3*/

   octet = (ds_low_layer_compatibility->information_transfer_capability & 0x1F);
   octet |= (ds_low_layer_compatibility->coding_standard & 0x03) << 5;
   octet |= 0x80;

   *data_ptr = octet;

   index++;
   data_ptr++;

   /*octet 4*/

   octet = (ds_low_layer_compatibility->information_transfer_rate & 0x1F);
   octet |= (ds_low_layer_compatibility->transfer_mode & 0x03) << 5;
   octet |= 0x80;

   *data_ptr = octet;
   index++;
   data_ptr++;

   /*octet 5*/
   
   /* Standard call settings */
   octet = (ds_low_layer_compatibility->
            user_information_layer_1_protocol & 0x1F);
   /*set layer 1 ident code*/
   octet |= 0x20;

   if (SPEED_64K_MULTIMEDIA_VAL != service_ptr->cbst_info.speed)
   {
     *data_ptr = octet;
     index++;
     data_ptr++;

     /*octet 5a*/

     octet = (ds_low_layer_compatibility->user_rate & 0x1F);
     octet |= (ds_low_layer_compatibility->negotiation & 0x01) << 5;
     octet |= (ds_low_layer_compatibility->sync_async & 0x01) << 6;

     /* FTM and synchronous calls stop at octet 5a */
     if ( ( CCITT_X31 !=
            ds_low_layer_compatibility->user_information_layer_1_protocol ) &&
          (LLC_ASYNCHRONOUS == ds_low_layer_compatibility->sync_async) )
     {
       *data_ptr = octet;
       index++;
       data_ptr++;

       /*octet 5b*/
       /* ITU Q.931 dictates octet 5b has different coding depending on
        * rate adoption V.110 or V.120 */
       octet = 0;
       if ( CCITT_V110 ==
            ds_low_layer_compatibility->user_information_layer_1_protocol )
       {
         /* V.110 rate adoption */
         octet |= (ds_low_layer_compatibility->flow_control_on_Rx & 0x01) << 1;
         octet |= (ds_low_layer_compatibility->flow_control_on_Tx & 0x01) << 2;
         octet |= (ds_low_layer_compatibility->NIC_on_Rx & 0x01) << 3;
         octet |= (ds_low_layer_compatibility->NIC_on_Tx & 0x01) << 4;
         octet |= (ds_low_layer_compatibility->intermediate_rate & 0x03) << 5;
       }
       else
       {
         /* V.120 rate adoption */
         octet |= (ds_low_layer_compatibility->
                   in_band_out_band_negotiation & 0x01) << 1;
         octet |= (ds_low_layer_compatibility->assignor_assignee & 0x01) << 2;
         octet |= (ds_low_layer_compatibility->
                   logical_link_identifier_negotiation & 0x01) << 3;
         octet |= (ds_low_layer_compatibility->mode_of_operation & 0x01) << 4;
         octet |= (ds_low_layer_compatibility->
                   multiple_frame_establishment_support & 0x01) << 5;
         octet |= (ds_low_layer_compatibility->rate_adaption_header & 0x01) << 6;
       }

       *data_ptr = octet;
       index++;
       data_ptr++;

       /*octet 5c*/

       octet = (ds_low_layer_compatibility->parity & 0x07);
       octet |= (ds_low_layer_compatibility->number_of_data_bits & 0x03) << 3;
       octet |= (ds_low_layer_compatibility->number_of_stop_bits & 0x03) << 5;

       *data_ptr = octet;
       index++;
       data_ptr++;

       /*octet 5d*/

       octet = (ds_low_layer_compatibility->modem_type & 0x3F);
       octet |= (ds_low_layer_compatibility->duplex_mode & 0x01) << 6;
     }

     octet |= 0x80;                     /*set extension bit (end of octet 5)*/
   }
   else
   {
     octet |= 0x80;                     /*set extension bit (end of octet 5)*/
   }

   *data_ptr = octet;
   index++;
   data_ptr++;

   return index;
} /* ds_ucsd_bc_put_llc_data() */



/*===========================================================================

FUNCTION ds_ucsd_init_orig_bc_data

DESCRIPTION
   Initializes bearer capability origination data.  If the udi parameter
   is set to TRUE, then set the bearer capability data is set to 
   defaults for a UDI call, otherwise set defaults for 3.1KHz call. 
   
         
DEPENDENCIES
  None
  
RETURN VALUE
  None
    
SIDE EFFECTS
  Sets the data at the passed in bearer capability pointer
===========================================================================*/
void
ds_ucsd_init_orig_bc_data 
(
   cm_bearer_capability_s_type  *bc, 
  const ds_ucsd_chosen_capability_T  *chosen_capability,
  const ds_ucsd_bearer_service_type  *service_ptr
)
{
   ASSERT( NULL != bc );
   ASSERT( NULL != chosen_capability );
   ASSERT( NULL != service_ptr );  
  
   /* Initialze structure to 0's */
   memset( bc, 0x0, sizeof (cm_bearer_capability_s_type) );
   
#ifdef FEATURE_GSM_EXT_SPEECH_PREF_LIST
   /* Invalidate speech version */ 
   bc->gsm_speech_ver_pref_1                    = GSM_INVALID_SPEECH_VER;
#endif

   /* Initialize all of the bearer capability values that are common
   ** between UDI and 3.1KHz 
   */
   
   bc->present                                  = TRUE;    
   bc->radio_channel_requirement                = FULL_RATE_ONLY;                    
   bc->coding_standard                          = GSM;
   bc->transfer_mode                            = CIRCUIT;                               
   bc->compression                              = DATA_COMPRESSION_NOT_POSSIBLE_OR_ALLOWED;
   bc->structure                                = UNSTRUCTURED;
   bc->duplex_mode                              = FULL_DUPLEX;
   bc->configuration                            = POINT_TO_POINT;
   bc->NIRR                                     = NO_MEANING;
   bc->establishment                            = DEMAND;
   bc->access_id                                = 0x00;
   bc->signalling_access_protocol               = I440_450;
   
   /* layer_1_id -- 1 as per 24.008 V3.6.0 */
   bc->layer_1_id                               = 0x01;
   bc->user_information_layer_1_protocol        = DEFAULT_LAYER_1_PROTOCOL;
   
   bc->number_of_stop_bits                      = _1_STOP_BIT;
   bc->negotiation                              = IN_BAND_NEGOTIATION_NOT_POSSIBLE; 
   bc->number_of_data_bits                      = _8_BITS_PER_CHAR;
   
   bc->intermediate_rate                        = _16_KBIT;
   bc->NIC_on_Tx                                = NIC_ON_TX_NOT_REQUIRED;
   bc->NIC_on_Rx                                = NIC_ON_RX_NOT_SUPPORTED;   
   bc->parity                                   = NAS_PARITY_NONE;
   
   bc->asymmetry_indication                     = ASYMMETRY_INDICATION_UNUSED;
   bc->other_ITC                                = OITC_RDI;

   /* Applicable to V.120 only */
   bc->rate_adaption_header                     = chosen_capability->chosen_bc.v120_info.ra_header;
   bc->multiple_frame_establishment             = chosen_capability->chosen_bc.v120_info.multi_frame_est;
   bc->mode_of_operation                        = chosen_capability->chosen_bc.v120_info.oper_mode;
   bc->logical_link_identifier_negotiation      = chosen_capability->chosen_bc.v120_info.llink_id_neg;
   bc->assignor_assignee                        = chosen_capability->chosen_bc.v120_info.assignor;
   bc->inband_outband_negotiation               = chosen_capability->chosen_bc.v120_info.band_neg;

   /* These are used for Phase2+ (i.e.14400bd) */

   bc->maximum_number_of_trafic_channels        = MAX_NUMBER_OF_TCH_1;
   bc->layer_2_id                               = 0x02;
   bc->user_information_layer_2_protocol        = 0x00;
   bc->mt_fall_back_rate                        = 0x00;
   
   /* Now initialize fields according to chosen capability */
   bc->user_rate                         = chosen_capability->chosen_bc.user_rate;
   bc->fixed_network_user_rate           = chosen_capability->chosen_bc.fnur;
   bc->other_modem_type                  = chosen_capability->chosen_bc.other_modem_type;
   bc->sync_async                        = chosen_capability->chosen_bc.sync_async;
   bc->connection_element                = chosen_capability->chosen_bc.connection_element;
   bc->modem_type                        = chosen_capability->chosen_bc.modem_type;
   bc->information_transfer_capability   = chosen_capability->chosen_bc.itc;
   bc->rate_adaption                     = chosen_capability->chosen_bc.rate_adaption;
   bc->other_rate_adaption               = chosen_capability->chosen_bc.other_rate_adaption;

   
   if (SPEED_64K_MULTIMEDIA_VAL != service_ptr->cbst_info.speed)
   {
     /* Standard call settings */
     bc->acceptable_channel_codings               = ACC_9600_ACCEPTABLE | 
                                                    ACC_14400_ACCEPTABLE;
                                                  
     bc->user_initiated_modification_indication   = UIMI_NOT_ALLOWED;;
     bc->wanted_air_interface_user_rate           = chosen_capability->chosen_bc.waiur;
   }
   else
   {
     /* Video call settings */
     bc->acceptable_channel_codings               = ACC_NONE;
     bc->user_initiated_modification_indication   = UIMI_UNDEFINED;
     bc->wanted_air_interface_user_rate           = WAIUR_UNDEFINED;
   }

   /* Change the structure element if we are doing non-transparent data */
   if (bc->connection_element == NON_TRANSPARENT_DATA)
   {
      bc->structure = SERVICE_DATA_UNIT_INTEGRITY;
   }
   
} /* ds_ucsd_init_orig_bc_data() */ 



/*===========================================================================

FUNCTION ds_ucsd_init_orig_llc_data

DESCRIPTION
   Initializes low level capability origination data.
         
DEPENDENCIES
  None
  
RETURN VALUE
  None
    
SIDE EFFECTS
  Sets the data at the passed in bearer capability pointer
===========================================================================*/
void
ds_ucsd_init_orig_llc_data 
(
   ds_ucsd_low_layer_compatibility_T *llc, 
  const cm_bearer_capability_s_type *bc,
  const ds_ucsd_bearer_service_type *service_ptr
)
{
  /* Initialize all of the bearer capability values; they may be
  ** overwritten later.
  */
  llc->IEI                                  = DSUCSD_LOW_LAYER_COMPATIBILITY;
  llc->coding_standard                      = LLC_CCITT;
  llc->negotiation_indicator                = 0;  /*not used*/
  llc->transfer_mode                        = LLC_CIRCUIT;
  llc->information_transfer_rate            = LLC_64KBIT;
  llc->structure                            = 0;  /*not used*/
  llc->configuration                        = 0;  /*not used*/
  llc->establishment                        = 0;  /*not used*/
  llc->symmetry                             = 0;  /*not used*/
  llc->information_transfer_rate_d_to_o     = 0;  /*not used*/
  llc->sync_async                           = LLC_ASYNCHRONOUS;
  llc->negotiation                          = LLC_IN_BAND_NEGOTIATION_NOT_POSSIBLE;
  llc->user_rate                            = LLC_64000;
  llc->intermediate_rate                    = LLC_16KBIT;                    
  llc->number_of_stop_bits                  = LLC_1_STOP;
  llc->number_of_data_bits                  = LLC_8_DATA;
  llc->parity                               = LLC_PARITY_NONE;
  llc->duplex_mode                          = LLC_FULL_DUPLEX;
  llc->modem_type                           = LLC_V32;
  llc->user_information_layer_2_protocol    = 0;
  llc->optional_layer_2_protocol_information= 0;
  llc->user_information_layer_3_protocol    = 0;
  llc->optional_layer_3_protocol_information= 0;

  /*V.110 only*/
  llc->NIC_on_Tx                            = LLC_NIC_ON_TX_NOT_REQUIRED;
  llc->NIC_on_Rx                            = LLC_NIC_ON_RX_NOT_SUPPORTED;
  llc->flow_control_on_Tx                   = LLC_TX_FLOW_CONTROL_NOT_REQUIRED;
  llc->flow_control_on_Rx                   = LLC_RX_FLOW_CONTROL_NOT_SUPPORTED;

  /*V.120 only*/
  llc->rate_adaption_header                 = bc->rate_adaption_header;
  llc->multiple_frame_establishment_support = bc->multiple_frame_establishment;
  llc->mode_of_operation                    = bc->mode_of_operation;
  llc->logical_link_identifier_negotiation  = bc->logical_link_identifier_negotiation;
  llc->assignor_assignee                    = bc->assignor_assignee;
  llc->in_band_out_band_negotiation         = bc->inband_outband_negotiation;

  /* Set ITC UDI or RDI */
  llc->information_transfer_capability      = (ITC_OTHER == bc->information_transfer_capability)?
                                              LLC_RESTRICTED_DIGITAL : LLC_UNRESTRICTED_DIGITAL;

  /* Set Layer 1 protcol */
  if ( SPEED_64K_MULTIMEDIA_VAL == service_ptr->cbst_info.speed )
  {
    llc->user_information_layer_1_protocol    = G7XX_VIDEO;
  }
  else
  {
    if ( V110_X30 == bc->rate_adaption )
    {
      llc->user_information_layer_1_protocol  = CCITT_V110;
    }
    else if ( X31_FLAGSTUFFING == bc->rate_adaption )
    {
      llc->user_information_layer_1_protocol  = CCITT_X31;
    }
    else if (( OTHER_RATE_ADAPTION == bc->rate_adaption ) &&
             ( V120 == bc->other_rate_adaption ))
    {
      llc->user_information_layer_1_protocol  = CCITT_V120;
    }
    else
    {
       DATA_MSG1_ERROR("Unsupported rate adaption %d, assuming V.110",
                  bc->rate_adaption);
       llc->user_information_layer_1_protocol  = CCITT_V110;
    }
  }

} /* ds_ucsd_init_orig_llc_data() */

/*-----------------------------------------**
**                                         **
**                                         **
** BEARER CAPABILITY FUNCTION DEFINITIONS  **
**                                         **
**                                         **
**-----------------------------------------*/



/*===========================================================================

FUNCTION ds_ucsd_bc_bcs_are_different

DESCRIPTION
  This function compares the two input bearer capabiliy structures. 
         
DEPENDENCIES
  None
  
RETURN VALUE
  TRUE if the the input bearer capability structures are different,
  FALSE if they are identical.
    
SIDE EFFECTS
  None

===========================================================================*/

static boolean 
ds_ucsd_bc_bcs_are_different 
(
   cm_bearer_capability_s_type  *cap1,
   cm_bearer_capability_s_type  *cap2
)
{
   boolean different;
   short   status;

   status = (short)memcmp (cap1, cap2, sizeof (cm_bearer_capability_s_type));

   if (status == 0)
   {
      different = FALSE;
   }
   else
   {
      different = TRUE;
   }

   return different;
}



/*===========================================================================

FUNCTION ds_ucsd_bc_check_for_support

DESCRIPTION
  This function checks a bearer capability received as a request
  against the corresponding bearer capability stored in the
  supported list. 
         
DEPENDENCIES
  None
  
RETURN VALUE
  TRUE if capability is supported, FALSE if not.
    
SIDE EFFECTS
  None

===========================================================================*/
static boolean 
ds_ucsd_bc_check_for_support
(
   uint8       capability_to_check,
   const uint8 ucsd_supported_capability[],
   int         number_of_capabilities
)
{
   boolean supported = FALSE;

   int option;

   for (option = 0; option < number_of_capabilities; option++)
   {
      if (capability_to_check == ucsd_supported_capability[option])
         supported = TRUE;
   }

   return supported;
}


/*===========================================================================

FUNCTION ds_ucsd_bc_negotiate_bearer_capability

DESCRIPTION
 This function checks negotiable fields of the input bearer capability.
 If any of the values are not supported, then they are changed
 (negotiated) to supported ones.  The chosen_capability structure
 which contains the users preferences is also factored into the
 negotiation process.
         
DEPENDENCIES
  None
  
RETURN VALUE
  TRUE if all of the negotiable parameters are supported, FALSE if 
  not. 
    
SIDE EFFECTS
  Potentially changes the information in the input capability structure
===========================================================================*/

static boolean  
ds_ucsd_bc_negotiate_bearer_capability
(
   cm_bearer_capability_s_type   *capability,
   sys_sys_mode_e_type           network_mode
)
{
   boolean c_capability_ok = TRUE;

   /* Always set radio channel requirement to FULL_RATE_ONLY */
   capability->radio_channel_requirement = FULL_RATE_ONLY;
   
   
   /* Check the negotiable parameters. If these are not supported 
   **  then negotiate them we will use the 3.1 KHz values since 
   ** they are most likely to be successful
   */

   if (!ds_ucsd_bc_check_for_support(capability->number_of_data_bits,
                             ucsd_supported_bc_number_of_data_bits,
                             sizeof(ucsd_supported_bc_number_of_data_bits)))
   {
      capability->number_of_data_bits = _8_BITS_PER_CHAR;
   }

   if (!ds_ucsd_bc_check_for_support(capability->number_of_stop_bits,
                             ucsd_supported_bc_number_of_stop_bits,
                             sizeof(ucsd_supported_bc_number_of_stop_bits)))
   {
      capability->number_of_stop_bits = _1_STOP_BIT;
   }

   if (!ds_ucsd_bc_check_for_support(capability->parity,
                             ucsd_supported_bc_parity,
                             sizeof(ucsd_supported_bc_parity)))
   {
      capability->parity = NAS_PARITY_NONE;
   }

   /* Check to see if the network has tried to set-up a call with 
      more slots than we are capable of supporting */
   if (!ds_ucsd_bc_check_for_support(capability->user_initiated_modification_indication,
                             ucsd_supported_bc_user_initiated_modification_indication,
                             sizeof(ucsd_supported_bc_user_initiated_modification_indication)))
   {

      capability->user_initiated_modification_indication = UIMI_NOT_ALLOWED;
   }

   /* Negotiate connection_element */
   if ( DS_UCSD_IS_WCDMA_TDS_MODE(network_mode))
   {
     /* Only Async NT and Sync T are supported in WCDMA */
     if ( capability->sync_async == ASYNCHRONOUS ) 
     {
       capability->connection_element = NON_TRANSPARENT_DATA;
     }
     else
     {
       /* Check for Frame Tunneling Mode per 3GPP TS27.001 B.4e,f */
       /* Negotiation requires ITC=UDI/RDI, FNUR=56k/64k, RA=V.110/X.30, CE=NT/Both */
       if ( ( capability->connection_element != TRANSPARENT_DATA ) &&  /* NT or Both */
            ( ( capability->information_transfer_capability == ITC_UDI ) ||
              ( capability->information_transfer_capability == ITC_OTHER ) ) &&
            ( ( capability->fixed_network_user_rate == FNUR_56K) ||
              ( capability->fixed_network_user_rate == FNUR_64K) ) &&
            ( capability->rate_adaption == V110_X30 ) )
       {
         capability->connection_element = NON_TRANSPARENT_DATA;
         capability->sync_async = ASYNCHRONOUS;
       }
       /* Assume synchronous, transparent */
       else
       {
         capability->connection_element = TRANSPARENT_DATA;
       }
     }
   }
   else if ( network_mode == SYS_SYS_MODE_GSM ) 
   {
      if ( capability->information_transfer_capability == ITC_FAX_G3 ) 
      {
         /* Only support synchronous, transparent GSM FAX calls */
         capability->connection_element = TRANSPARENT_DATA;
      }
      else
      {
         /* Use non-transparent if given choice */
         if (( capability->connection_element == BOTH_TRANSPARENT_PREF ) ||
             ( capability->connection_element == BOTH_NON_TRANSPARENT_PREF ))
         {
            capability->connection_element = NON_TRANSPARENT_DATA;
         }
      }
   }

   /* Negotiate structure and user_information_layer_2_protocol */
   if ( capability->connection_element == TRANSPARENT_DATA )
   {
      capability->structure = UNSTRUCTURED;
      capability->user_information_layer_2_protocol = 0;
   }
   else if ( capability->connection_element == NON_TRANSPARENT_DATA ) 
   {
      capability->structure = SERVICE_DATA_UNIT_INTEGRITY;
      /*
       * user_information_layer_2_protocol is an optional parameter.
       * A value of zero indicates it was unspecified,
       * i.e. Not Available (NAV),
       * see 07.01, Annex B. In fact this means OutOfBand flow control.
       */
      if(!ds_ucsd_bc_check_for_support(capability->user_information_layer_2_protocol,
                             ucsd_supported_bc_user_information_layer_2_protocol,
                    sizeof(ucsd_supported_bc_user_information_layer_2_protocol)))
      {
         capability->user_information_layer_2_protocol = 0;
      }
   }

   /* Negotiate rate_adaption: no rate adaption or other rate adaption for
   ** WCDMA Sync Transparent UDI; no rate adaption for 3.1k or Fax; V.110/X.30
   ** otherwise
   */
   if ( ( DS_UCSD_IS_WCDMA_TDS_MODE(network_mode)) &&
        ( capability->sync_async == SYNCHRONOUS ) &&
        ( capability->connection_element == TRANSPARENT_DATA ) &&
        ( capability->information_transfer_capability == ITC_UDI ) )
   {
      /* WCDMA Synchronous Transparent UDI */

      if ( ( capability->rate_adaption == NO_RATE_ADAPTION ) ||
           ( capability->rate_adaption == OTHER_RATE_ADAPTION ) )
      {
            /* Negotiate other_rate_adaption */
            if( capability->rate_adaption == OTHER_RATE_ADAPTION )
            {
               capability->other_rate_adaption = H223_H245;
            }
      }
      else
      {
         capability->rate_adaption = NO_RATE_ADAPTION;
      }
   }
   
   /* X.31 flagstuffing rate adaption for WCDMA NonTransparent UDI/RDI
   ** at FNUR 56k/64k, giving Frame Tunneling Mode (FTM).
   */
   else if ( ( DS_UCSD_IS_WCDMA_TDS_MODE(network_mode) ) &&
             ( capability->sync_async == ASYNCHRONOUS ) &&
             ( capability->connection_element == NON_TRANSPARENT_DATA ) && 
             ( ( capability->information_transfer_capability == ITC_UDI ) ||
               ( capability->information_transfer_capability == ITC_OTHER ) ) &&
             ( ( capability->fixed_network_user_rate == FNUR_56K) ||
               ( capability->fixed_network_user_rate == FNUR_64K) ) )
   {
      /* WCDMA Async NonTransparent UDI/RDI */

      /* Negotiate X.31 flag stugging */
      if ( capability->rate_adaption == V110_X30 )
      {
         capability->rate_adaption = X31_FLAGSTUFFING;
         DATA_MSG0_HIGH( "Negotiate RA to X.31FS");
      }
   }
   else if ( ( capability->information_transfer_capability == ITC_3_1K ) ||
             ( capability->information_transfer_capability == ITC_FAX_G3 ) )
   {
      capability->rate_adaption = NO_RATE_ADAPTION;
   }
   else 
   {
      if ( !ds_ucsd_bc_check_for_support(capability->rate_adaption,
                                ucsd_supported_bc_rate_adaption,
                                sizeof(ucsd_supported_bc_rate_adaption)))
      {
         capability->rate_adaption = V110_X30;
      }

      /*
      ** DATA BC: check other_rate_adaption
      */
      else if( ( capability->rate_adaption == OTHER_RATE_ADAPTION ) &&
               !ds_ucsd_bc_check_for_support(capability->other_rate_adaption,
                                ucsd_supported_bc_other_rate_adaption,
                                sizeof(ucsd_supported_bc_other_rate_adaption)))
      {
         capability->other_rate_adaption = V120;
      }
   }

   
   /* Setup the elements for a phase2+ high-speed call:
      see 07.01 para. 8.3.3.1 */
   if ( network_mode == SYS_SYS_MODE_GSM ) 
   {
      /* FNUR is specified */
      if ( (capability->fixed_network_user_rate != FNUR_UNDEFINED) &&
           (capability->fixed_network_user_rate != FNUR_NOT_APPLICABLE) )
      {
         /* If FAX call, try to negotiate down to 9600 */
         if( capability->information_transfer_capability == ITC_FAX_G3)
         {
            capability->user_rate                      = USER_RATE_9600;
            capability->fixed_network_user_rate        = FNUR_UNDEFINED;
            capability->wanted_air_interface_user_rate = WAIUR_UNDEFINED;
            capability->acceptable_channel_codings     = ACC_9600_ACCEPTABLE;
            MSG_HIGH ("DS: Negotiating 14400 MT FAX to 9600",0,0,0);
         }
         else if (ds_ucsd_bc_check_for_support(capability->fixed_network_user_rate,
                                  gcsd_supported_bc_fixed_network_user_rate,
                                  sizeof(gcsd_supported_bc_fixed_network_user_rate)))
         {
            if (capability->fixed_network_user_rate == FNUR_14400)
            {
               capability->wanted_air_interface_user_rate = WAIUR_14400;
            }
            /* Explicit FNUR needs explicit WAIUR specified */
            else if (capability->fixed_network_user_rate == FNUR_9600)
            {
               capability->wanted_air_interface_user_rate = WAIUR_9600;
            }
         }
         else
         {
           /* FNUR not supported, default to 14400 if proposed rate
            * was higher, otherwise use 9600.
            */
           if (FNUR_14400 < capability->fixed_network_user_rate)
           {
             DATA_MSG0_HIGH("MT FNUR not supported, negotiating to FNUR_14400\n");
             capability->fixed_network_user_rate = FNUR_14400;
             capability->wanted_air_interface_user_rate = WAIUR_14400;
           }
           else
           {
             DATA_MSG0_HIGH("MT FNUR not supported, negotiating to FNUR_9600\n");
             capability->fixed_network_user_rate = FNUR_9600;
             capability->wanted_air_interface_user_rate = WAIUR_UNDEFINED;
           }
         }

         /* Assign the max number of traffic channels and acceptable channel
          * codings per 3GPP 27.001 8.3.3.1.  */
         capability->maximum_number_of_trafic_channels = MAX_NUMBER_OF_TCH_1;

         /*  Fax call handled above, handling general case. */
         if ( capability->information_transfer_capability != ITC_FAX_G3 )
         {
           capability->acceptable_channel_codings = ACC_9600_ACCEPTABLE |
                                                    ACC_14400_ACCEPTABLE;
         }
      }
   } /* network_mode == SYS_SYS_MODE_GSM */

   /* Negotiate the Fixed Network User Rate and Wanted Air Interface User Rate 
   ** for WCDMA.
   */
   if ( DS_UCSD_IS_WCDMA_TDS_MODE(network_mode) ) 
   {
     /* FNUR is specified */
     if ( (capability->fixed_network_user_rate != FNUR_UNDEFINED) &&
          (capability->fixed_network_user_rate != FNUR_NOT_APPLICABLE) )
     {
      /* Negotiate the Fixed Network User Rate for Synchronous Transparent
      ** CS Data.
      */
       if ( ( capability->sync_async == SYNCHRONOUS ) &&
            ( capability->connection_element == TRANSPARENT_DATA ) )
       {
         if ( ds_ucsd_bc_check_for_support(
                capability->fixed_network_user_rate,
                wcsd_supported_bc_fixed_network_user_rate,
                sizeof(wcsd_supported_bc_fixed_network_user_rate) ) == FALSE )
         {
           /* FNUR not supported */
           /* We used to negotiate unsupported FNUR up to 64k value but 
            * this is not spec compliant.  Now we fail the config. */
           /* capability->fixed_network_user_rate = FNUR_64K; */
           DATA_MSG1_ERROR("FNUR not supported for sync/transparent: %d",
                      capability->fixed_network_user_rate);
           c_capability_ok = FALSE;        
         }

         /* Applies only for non-transparent, per 3GPP 27.007 8.3.3.1 */
         capability->wanted_air_interface_user_rate = WAIUR_UNDEFINED;
       }
       
       /* Assign the max number of traffic channels and acceptable channel
        * codings per 3GPP 27.001 8.3.3.1.  */
       capability->maximum_number_of_trafic_channels = MAX_NUMBER_OF_TCH_1;
       capability->acceptable_channel_codings = ACC_9600_ACCEPTABLE |
                                                ACC_14400_ACCEPTABLE;
     }

     /* Negotiate the Wanted Air Interface User Rate for Asynchronous 
     ** Non-Transparent CS Data.
     */
     if ( ( TRUE == c_capability_ok ) &&
          ( capability->sync_async == ASYNCHRONOUS ) &&
          ( capability->connection_element == NON_TRANSPARENT_DATA ))
     {        
         /* Set WAIUR based on FNUR */
         if (capability->fixed_network_user_rate <= FNUR_14400)
         { 
            capability->wanted_air_interface_user_rate = WAIUR_14400;
         }
         else if (capability->fixed_network_user_rate <= FNUR_28800)
         {
            capability->wanted_air_interface_user_rate = WAIUR_28800;
         }
         else  /* FNUR > 28.8 */
         {
            capability->wanted_air_interface_user_rate = WAIUR_57600;
         }
         DATA_MSG1_HIGH( "Negotiate WAIUR to %d",
                   capability->wanted_air_interface_user_rate );
             
      }
   } /* network_mode == SYS_SYS_MODE_WCDMA */

   return c_capability_ok;
} /* ds_ucsd_bc_negotiate_bearer_capability() */


/*===========================================================================

FUNCTION ds_ucsd_bc_check_params

DESCRIPTION
 This function checks the non-negotiable fields of the input bearer capability
 and low layer capability structures to determine if they are supported.  If
 the check_all input parameter is set to TRUE, then the negotiable parameters
 are checked also.
         
DEPENDENCIES
  None
  
RETURN VALUE
  TRUE if all of the input bearer capability and low layer compatibility fields
  are supported, FALSE if not
    
SIDE EFFECTS
  None

===========================================================================*/
static boolean 
ds_ucsd_bc_check_params
(    
   cm_bearer_capability_s_type       *requested_bc,
   ds_ucsd_low_layer_compatibility_T *requested_llc,
   sys_sys_mode_e_type               network_mode,
   boolean                           check_all
)
{
   /*locals*/

   boolean capability_ok = TRUE;

   /*
   ** DATA BC: check information_transfer_capability
   */
   if ( ( ( DS_UCSD_IS_WCDMA_TDS_MODE(network_mode) ) && 
          ( requested_bc->information_transfer_capability != ITC_UDI ) &&
          ( requested_bc->information_transfer_capability != ITC_OTHER ) &&
          ( requested_bc->information_transfer_capability != ITC_3_1K ) 
        ) ||
        ( ( network_mode == SYS_SYS_MODE_GSM ) &&
          !ds_ucsd_bc_check_for_support(
              requested_bc->information_transfer_capability,
              ucsd_supported_bc_information_transfer_capability,
              sizeof(ucsd_supported_bc_information_transfer_capability) ) 
        ) 
      )
   {
      DATA_MSG2_HIGH("UCSD BC reject ITC = %d (mode=%d)",
                        requested_bc->information_transfer_capability, network_mode);

      capability_ok = FALSE;
   }
   
      
   /*
   ** DATA BC: check user_rate for GSM
   */
   else if( ( network_mode == SYS_SYS_MODE_GSM ) &&
            !ds_ucsd_bc_check_for_support(requested_bc->user_rate,
                             ucsd_supported_bc_user_rate,
                             sizeof(ucsd_supported_bc_user_rate)))
   {
      DATA_MSG1_HIGH("UCSD BC reject user_rate = %d",
                         requested_bc->user_rate);
                         
      capability_ok = FALSE;
   }
      

   /*
   ** DATA BC: check structure
   */
   else if( !ds_ucsd_bc_check_for_support(requested_bc->structure,
                             ucsd_supported_bc_structure,
                             sizeof(ucsd_supported_bc_structure)))
   {

      DATA_MSG1_HIGH("UCSD BC reject structure = %d",
                          requested_bc->structure);
                          
      capability_ok = FALSE;
   }


   /*
   ** DATA BC: check duplex_mode
   */
   else if( !ds_ucsd_bc_check_for_support(requested_bc->duplex_mode,
                             ucsd_supported_bc_duplex_mode,
                             sizeof(ucsd_supported_bc_duplex_mode)))
   {

      DATA_MSG1_HIGH( "UCSD BC reject duplex_mode = %d",
                        requested_bc->duplex_mode );

      capability_ok = FALSE;
   }


   /*
   ** DATA BC: check signalling_access_protocol
   */

   else if( !ds_ucsd_bc_check_for_support(requested_bc->signalling_access_protocol,
                             ucsd_supported_bc_signalling_access_protocol,
                             sizeof(ucsd_supported_bc_signalling_access_protocol)))
   {

      DATA_MSG1_HIGH( "UCSD BC reject Signalling Access Protocol = %d",
                requested_bc->signalling_access_protocol);

      capability_ok = FALSE;
   }


   /*
   ** DATA BC: If we get a request for SYNCHRONOUS data, and it isn't
   ** a FAX call, reject it.
   */
   else if( (network_mode == SYS_SYS_MODE_GSM) &&
            (requested_bc->sync_async == SYNCHRONOUS) &&
            (
	       (requested_bc->information_transfer_capability != ITC_FAX_G3)
#ifdef FEATURE_MULTIMEDIA_FALLBACK
	       #error code not present
#endif /* FEATURE_MULTIMEDIA_FALLBACK */
	    )
          )
   {

      DATA_MSG1_HIGH("UCSD BC reject sync_async = %d",
                requested_bc->sync_async);

      capability_ok = FALSE;
   }
   
   /*
   ** Reject FAX in WCDMA mode
   */
   else if( ( DS_UCSD_IS_WCDMA_TDS_MODE(network_mode) ) &&
            (requested_bc->information_transfer_capability == ITC_FAX_G3)
          )
   {

      DATA_MSG0_HIGH("UCSD BC reject WCDMA FAX");

      capability_ok = FALSE;
   }
   
   /*
   ** Reject ASYNC or NON-TRANSPARENT FAX 
   */
   else if( (requested_bc->information_transfer_capability == ITC_FAX_G3) &&
            ( (requested_bc->sync_async == ASYNCHRONOUS ) ||
              (requested_bc->connection_element == NON_TRANSPARENT_DATA)
            ) )
   {

      DATA_MSG0_HIGH( "UCSD BC reject ASYNC/NT FAX");

      capability_ok = FALSE;
   }
   
   /*
   ** DATA BC: check coding_standard
   */
   else if( !ds_ucsd_bc_check_for_support(requested_bc->coding_standard,
                             ucsd_supported_bc_coding_standard,
                             sizeof(ucsd_supported_bc_coding_standard)))
   {
                             
      DATA_MSG1_HIGH("UCSD BC reject coding_standard = %d",
                requested_bc->coding_standard);

      capability_ok = FALSE;
   }


   /*
   ** DATA BC: check transfer_mode
   */
   else if( !ds_ucsd_bc_check_for_support(requested_bc->transfer_mode,
                             ucsd_supported_bc_transfer_mode,
                             sizeof(ucsd_supported_bc_transfer_mode)))
   {

      DATA_MSG1_HIGH( "UCSD BC reject transfer_mode = %d",
                requested_bc->transfer_mode );

      capability_ok = FALSE;
   }


   /*
   ** DATA BC: check configuration
   */
   else if( !ds_ucsd_bc_check_for_support(requested_bc->configuration,
                             ucsd_supported_bc_configuration,
                             sizeof(ucsd_supported_bc_configuration)))
   {

      DATA_MSG1_HIGH( "UCSD BC reject configuration = %d",
                requested_bc->configuration);

      capability_ok = FALSE;
   }


   /*
   ** DATA BC: check NIRR
   */
   else if( !ds_ucsd_bc_check_for_support(requested_bc->NIRR,
                             ucsd_supported_bc_NIRR,
                             sizeof(ucsd_supported_bc_NIRR)))
   {

      DATA_MSG1_HIGH("UCSD BC reject NIRR = %d",
                requested_bc->NIRR);

      capability_ok = FALSE;
   }


   /*
   ** DATA BC: check establishment
   */
   else if( !ds_ucsd_bc_check_for_support(requested_bc->establishment,
                             ucsd_supported_bc_establishment,
                             sizeof(ucsd_supported_bc_establishment)))
   {

      DATA_MSG1_HIGH( "UCSD BC reject establishment = %d",
                requested_bc->establishment);

      capability_ok = FALSE;
   }


   /*
   ** DATA BC: check user_information_layer_1_protocol
   */
   else if(!ds_ucsd_bc_check_for_support(requested_bc->user_information_layer_1_protocol,
                            ucsd_supported_bc_user_information_layer_1_protocol,
                            sizeof(ucsd_supported_bc_user_information_layer_1_protocol)))
   {

      DATA_MSG1_HIGH( "UCSD BC reject user_information_layer_1_protocol = %d",
                requested_bc->user_information_layer_1_protocol);

      capability_ok = FALSE;
   }


   /*
   ** DATA BC: check negotiation
   */
   else if( !ds_ucsd_bc_check_for_support(requested_bc->negotiation,
                             ucsd_supported_bc_negotiation,
                             sizeof(ucsd_supported_bc_negotiation)))
   {
      DATA_MSG1_HIGH( "UCSD BC reject negotiation = %d",
                requested_bc->negotiation );

      capability_ok = FALSE;
   }


   /*
   ** DATA BC: check intermediate_rate for GSM
   */
   else if( ( network_mode == SYS_SYS_MODE_GSM ) &&
            !ds_ucsd_bc_check_for_support(requested_bc->intermediate_rate,
                             ucsd_supported_bc_intermediate_rate,
                             sizeof(ucsd_supported_bc_intermediate_rate)))
   {

      DATA_MSG1_HIGH( "UCSD BC reject intermediate_rate = %d",
                requested_bc->intermediate_rate );

      capability_ok = FALSE;
   }


   /*
   ** DATA BC: check NIC_on_Tx for GSM
   */
   else if( ( network_mode == SYS_SYS_MODE_GSM ) &&
            !ds_ucsd_bc_check_for_support(requested_bc->NIC_on_Tx,
                             ucsd_supported_bc_NIC_on_Tx,
                             sizeof(ucsd_supported_bc_NIC_on_Tx)))
   {

     DATA_MSG1_HIGH( "UCSD BC reject NIC_on_Tx = %d",
                requested_bc->NIC_on_Tx );

      capability_ok = FALSE;
   }


   /*
   ** DATA BC: check NIC_on_Rx for GSM
   */
   else if( ( network_mode == SYS_SYS_MODE_GSM ) &&
            !ds_ucsd_bc_check_for_support(requested_bc->NIC_on_Rx,
                             ucsd_supported_bc_NIC_on_Rx,
                             sizeof(ucsd_supported_bc_NIC_on_Rx)))
   {

      DATA_MSG1_HIGH( "UCSD BC reject NIC_on_Rx = %d",
                requested_bc->NIC_on_Rx );

      capability_ok = FALSE;
   }
   
      
   
   /* THIS WAS NOT IN CCWW CODE... NEEDS TESTING !!! */
      
   /* If requested, check the negotiable parameters to make sure they are valid */
  
   if (capability_ok && check_all)
   {
      /*
      ** DATA BC: check radio_channel_requirement
      */
      if( ( ( requested_bc->information_transfer_capability == ITC_FAX_G3 ) ||
            ( ( DS_UCSD_IS_WCDMA_TDS_MODE(network_mode) ) &&
              ( requested_bc->sync_async == SYNCHRONOUS ) &&
              ( requested_bc->connection_element == TRANSPARENT_DATA ) &&
              ( requested_bc->information_transfer_capability == ITC_UDI ) 
            ) 
          ) &&
          ( requested_bc->radio_channel_requirement != FULL_RATE_ONLY ) )             
      {
         DATA_MSG2_HIGH( "UCSD BC reject RCR = %d (ITC=%d)",
                   requested_bc->radio_channel_requirement, 
                   requested_bc->information_transfer_capability );

         capability_ok = FALSE;
      }

      /*
      ** DATA BC: check structure
      */
      if( !ds_ucsd_bc_check_for_support(requested_bc->structure,
                                ucsd_supported_bc_structure,
                                sizeof(ucsd_supported_bc_structure)))
      {

         DATA_MSG1_HIGH( "UCSD BC reject structure = %d",
                   requested_bc->structure );

         capability_ok = FALSE;

      }   

      /*
      ** DATA BC: check rate_adaption
      */
      else if( !ds_ucsd_bc_check_for_support(requested_bc->rate_adaption,
                                ucsd_supported_bc_rate_adaption,
                                sizeof(ucsd_supported_bc_rate_adaption)))
      {

         DATA_MSG1_HIGH( "UCSD BC reject rate_adaption = %d",
                   requested_bc->rate_adaption);

         capability_ok = FALSE;
      }

      /*
      ** DATA BC: check other_rate_adaption
      */
      else if( ( requested_bc->rate_adaption == OTHER_RATE_ADAPTION ) &&
               !ds_ucsd_bc_check_for_support(requested_bc->other_rate_adaption,
                                ucsd_supported_bc_other_rate_adaption,
                                sizeof(ucsd_supported_bc_other_rate_adaption)))
      {

        DATA_MSG1_HIGH( "UCSD BC reject other_rate_adaption = %d",
                   requested_bc->other_rate_adaption );

         capability_ok = FALSE;
      }

      /*
      ** DATA BC: check number_of_data_bits
      */
      else if( !ds_ucsd_bc_check_for_support(requested_bc->number_of_data_bits,
                                ucsd_supported_bc_number_of_data_bits,
                                sizeof(ucsd_supported_bc_number_of_data_bits)))
      {

         DATA_MSG1_HIGH( "UCSD BC reject number_of_data_bits = %d",
                   requested_bc->number_of_data_bits );

         capability_ok = FALSE;
      }
      
      /*
      ** DATA BC: check number_of_stop_bits
      */
      else if( !ds_ucsd_bc_check_for_support(requested_bc->number_of_stop_bits,
                                ucsd_supported_bc_number_of_stop_bits,
                                sizeof(ucsd_supported_bc_number_of_stop_bits)))
      {

         DATA_MSG1_HIGH( "UCSD BC reject number_of_stop_bits = %d",
                   requested_bc->number_of_stop_bits );

         capability_ok = FALSE;
      }
      

      /*
      ** DATA BC: check parity
      */
      else if( !ds_ucsd_bc_check_for_support(requested_bc->parity,
                                ucsd_supported_bc_parity,
                                sizeof(ucsd_supported_bc_parity)))
      {

         DATA_MSG1_HIGH( "UCSD BC reject parity = %d",
                   requested_bc->parity );

         capability_ok = FALSE;
      }
      
      /*
      ** DATA BC: check connection_element. Async Transparent and Sync NT are
      ** not supported for WCDMA.
      */
      else if( !ds_ucsd_bc_check_for_support(requested_bc->connection_element,
                                ucsd_supported_bc_connection_element,
                                sizeof(ucsd_supported_bc_connection_element)) ||
               ( ( DS_UCSD_IS_WCDMA_TDS_MODE(network_mode) ) &&
                 ( ( (requested_bc->sync_async == ASYNCHRONOUS) &&
                     ( requested_bc->connection_element == TRANSPARENT_DATA ) 
                   ) ||
                   ( (requested_bc->sync_async == SYNCHRONOUS) &&
                     ( requested_bc->connection_element == NON_TRANSPARENT_DATA )
                   )
                 )
               )
             )
      {
         DATA_MSG2_HIGH( "UCSD BC reject CE=%d (sync_async=%d)",
                   requested_bc->connection_element, 
                   requested_bc->sync_async );

         capability_ok = FALSE;
      }
      
      /*
      ** DATA BC: check user_information_layer_2_protocol
      */
      else if( !ds_ucsd_bc_check_for_support(requested_bc->user_information_layer_2_protocol,
                                ucsd_supported_bc_user_information_layer_2_protocol,
                                sizeof(ucsd_supported_bc_user_information_layer_2_protocol)))
      {

         DATA_MSG1_HIGH( "UCSD BC reject user_information_layer_2_protocol = %d",
                   requested_bc->user_information_layer_2_protocol);

         capability_ok = FALSE;
      }
      
      /* NOW DO PHASE 2+ ELEMENTS... THIS WAS NOT IN CCWW CODE... NEEDS TESTING !!! */
       
      /* Check to see if the network has tried to set-up a call with 
      more slots than we are capable of supporting */
      else if (!ds_ucsd_bc_check_for_support(requested_bc->
                                user_initiated_modification_indication,
                                ucsd_supported_bc_user_initiated_modification_indication,
                                sizeof(ucsd_supported_bc_user_initiated_modification_indication)))
      {

         DATA_MSG1_HIGH( "UCSD BC reject user_initiated_modification_indic=%d",
                   requested_bc->user_initiated_modification_indication);

         capability_ok = FALSE;
      }
           
      /*
      ** DATA BC: check acceptable_channel_codings
      */
      else if( !ds_ucsd_bc_check_for_support(requested_bc->acceptable_channel_codings,
                                ucsd_supported_bc_acceptable_channel_codings,
                                sizeof(ucsd_supported_bc_acceptable_channel_codings)))
      {

         DATA_MSG1_HIGH( "UCSD BC reject acceptable_channel_codings = %d",
                   requested_bc->acceptable_channel_codings );

         capability_ok = FALSE;
      }
      
      /*
      ** DATA BC: check maximum_number_of_trafic_channels
      */
      else if( !ds_ucsd_bc_check_for_support(requested_bc->maximum_number_of_trafic_channels,
                                ucsd_supported_bc_maximum_number_of_trafic_channels,
                                sizeof(ucsd_supported_bc_maximum_number_of_trafic_channels)))
      {

         DATA_MSG1_HIGH( "UCSD BC reject maximum_number_of_trafic_channels = %d",
                   requested_bc->maximum_number_of_trafic_channels);

         capability_ok = FALSE;
      }
      
      /*
      ** DATA BC: check wanted_air_interface_user_rate if CE is not Transparent
      */
      else if( ( requested_bc->connection_element != TRANSPARENT_DATA ) &&
               ( ( ( network_mode == SYS_SYS_MODE_GSM ) &&
                   !ds_ucsd_bc_check_for_support(requested_bc->wanted_air_interface_user_rate,
                      gcsd_supported_bc_wanted_air_interface_user_rate,
                      sizeof(gcsd_supported_bc_wanted_air_interface_user_rate))
                 ) ||
                 ( ( DS_UCSD_IS_WCDMA_TDS_MODE(network_mode) ) &&
                   !ds_ucsd_bc_check_for_support(requested_bc->wanted_air_interface_user_rate,
                      wcsd_supported_bc_wanted_air_interface_user_rate,
                      sizeof(wcsd_supported_bc_wanted_air_interface_user_rate))
                 )
               )
             )
      {
         DATA_MSG2_HIGH( "UCSD BC reject WAIUR = %d (mode=%d)",
                   requested_bc->wanted_air_interface_user_rate, network_mode);

         capability_ok = FALSE;
      }
   }
      


   if( capability_ok )
   {
      /*BC matches, so check LLC*/

      if (requested_llc->IEI != DSUCSD_LOW_LAYER_COMPATIBILITY) /* llc exists ? */
      {
         DATA_MSG0_HIGH( "UCSD LLC not present");
      }
      else
      {
         DATA_MSG0_HIGH( "UCSD LLC present" );


         /*check if requested low_layer_compatibility matches
          *stored MS supported capability
          *see GSM 07.01 v4.10.0 sect. 8.3.1*/

         capability_ok = FALSE;

         if(! ds_ucsd_bc_check_for_support(requested_llc->coding_standard,
                                 ucsd_supported_llc_coding_standard,
                                 sizeof(ucsd_supported_llc_coding_standard)))
         {
            DATA_MSG1_HIGH( "UCSD LLC coding_standard wrong = %x",
                         requested_llc->coding_standard );
         }

         else if (!ds_ucsd_bc_check_for_support
                  (requested_llc->information_transfer_capability,
                   ucsd_supported_llc_information_transfer_capability,
                   sizeof(ucsd_supported_llc_information_transfer_capability)))
         {
            DATA_MSG1_HIGH( "UCSD LLC itc wrong = %x",
                       requested_llc->information_transfer_capability );
         }
         
         else if ( !ds_ucsd_bc_check_for_support(requested_llc->transfer_mode,
                                         ucsd_supported_llc_transfer_mode,
                                         sizeof(ucsd_supported_llc_transfer_mode)))
         {

            DATA_MSG1_HIGH( "UCSD LLC transfer_mode wrong = %x",
                         requested_llc->transfer_mode );

         }
         else if (!ds_ucsd_bc_check_for_support
                  (requested_llc->information_transfer_rate,
                   ucsd_supported_llc_information_transfer_rate,
                   sizeof(ucsd_supported_llc_information_transfer_rate)))
         {
            DATA_MSG1_HIGH( "UCSD LLC ucsd_supported_llc_information_transfer_rate wrong = %x",
                         requested_llc->information_transfer_rate);
         }
         
         else if ( !ds_ucsd_bc_check_for_support
                   (requested_llc->
                    user_information_layer_1_protocol,
                    ucsd_supported_llc_user_information_layer_1_protocol,
                    sizeof(ucsd_supported_llc_user_information_layer_1_protocol)))
         {
            DATA_MSG1_HIGH( "UCSD LLC user_information_layer_1_protocol wrong = %x",
                         requested_llc->user_information_layer_1_protocol);

         }
         
         else if (!ds_ucsd_bc_check_for_support(requested_llc->sync_async,
                                        ucsd_supported_llc_sync_async,
                                        sizeof(ucsd_supported_llc_sync_async)))
         {

            DATA_MSG1_HIGH( "UCSD LLC sync_async wrong = %x",
                         requested_llc->sync_async);
    
         }
         else if   (!ds_ucsd_bc_check_for_support(requested_llc->negotiation,
                                          ucsd_supported_llc_negotiation,
                                          sizeof(ucsd_supported_llc_negotiation)))
         {

            DATA_MSG1_HIGH( "UCSD LLC negotiation wrong = %x",
                         requested_llc->negotiation );

         }

         /*OK so far, check params specific to V.110*/

         else if (requested_llc->user_information_layer_1_protocol == CCITT_V110)
         {
            if (!ds_ucsd_bc_check_for_support
                (requested_llc->intermediate_rate,
                 ucsd_supported_llc_intermediate_rate,
                 sizeof(ucsd_supported_llc_intermediate_rate)))
            {
               DATA_MSG1_HIGH( "UCSD LLC intermediate_rate wrong = %x",
                            requested_llc->intermediate_rate);

            }
            
            else if ( !ds_ucsd_bc_check_for_support(requested_llc->NIC_on_Tx,
                                            ucsd_supported_llc_NIC_on_Tx,
                                            sizeof(ucsd_supported_llc_NIC_on_Tx)))
            {
               DATA_MSG1_HIGH( "UCSD LLC intermediate_rate wrong = %x",
                            requested_llc->intermediate_rate );
            }
            
            else if (!ds_ucsd_bc_check_for_support(requested_llc->NIC_on_Rx,
                                           ucsd_supported_llc_NIC_on_Rx,
                                           sizeof(ucsd_supported_llc_NIC_on_Rx)))
            {

               DATA_MSG1_HIGH( "UCSD LLC NIC_on_Rx wrong = %x",
                            requested_llc->NIC_on_Rx );
            }
            
            else if (!ds_ucsd_bc_check_for_support
                     (requested_llc->flow_control_on_Tx,
                      ucsd_supported_llc_flow_control_on_Tx,
                      sizeof(ucsd_supported_llc_flow_control_on_Tx)))
            {
               DATA_MSG1_HIGH( "UCSD LLC flow_control_on_Tx wrong = %x",
                            requested_llc->flow_control_on_Tx);

            }
            
            else if (!ds_ucsd_bc_check_for_support
                     (requested_llc->flow_control_on_Rx,
                      ucsd_supported_llc_flow_control_on_Rx,
                      sizeof(ucsd_supported_llc_flow_control_on_Rx)))
            {
               DATA_MSG1_HIGH( "UCSD LLC flow_control_on_Rx wrong = %x",
                            requested_llc->flow_control_on_Rx );

            }
            else

            {
               /*no need to check HLC for transparent async data*/

               capability_ok = TRUE;
            }
         }

         else
         {
            /*
            ** protocol other than V.110,
            ** so OK without checking IR, NIC, flow cont
            */
            capability_ok = TRUE;
         }
         
      } /* LLC present */
      
   } /* capability_ok is TRUE */

   return capability_ok;
} /* ds_ucsd_bc_check_params() */


/*===========================================================================

FUNCTION ds_ucsd_bc_copy_chosen_cap

DESCRIPTION
   Similar (but strangely, not the same... not sure why) to the call
   origination function in that it copies default capability information
   to the input negotiated_capability structure.  The information copied
   depends on user preference setting passed in via the chosen_capability
   parameter
   
         
DEPENDENCIES
  None
  
RETURN VALUE
  None
    
SIDE EFFECTS
  Possibly changes the data pointed to by the negotiated_capability 
  structure.
===========================================================================*/
static void
ds_ucsd_bc_copy_chosen_cap
(
   ds_ucsd_negotiated_capability_T    *negotiated_capability,
   ds_ucsd_chosen_capability_T        *chosen_capability,
   ds_ucsd_nt_rlp_parameters_T        *rlp_parameters, /*Current RLP parameters*/
   const ds_ucsd_bearer_service_type  *service_ptr,
   sys_sys_mode_e_type                 network_mode 
)
{
   ds_ucsd_low_layer_compatibility_T   temp_llc;

   /*
   ** SIMILAR TO A MO CALL SETUP.
   ** THE NET HAS GIVEN US FREE CHOICE OF THE BC TO RETURN IN
   ** A MT CALL
   */

   negotiated_capability->bc_repeat_indicator.present = FALSE;
   negotiated_capability->llc_repeat_indicator.present = FALSE;
   negotiated_capability->hlc_repeat_indicator.present = FALSE;
   negotiated_capability->high_layer_compatibility_1.present = FALSE;
   
   /********************/
   /* DATA CALL        */
   /********************/
 
   /* Initialize the BC and LLC structures */
   ds_ucsd_init_orig_bc_data (&negotiated_capability->bearer_capability_1,
                              chosen_capability,
                              service_ptr );
   
   ds_ucsd_init_orig_llc_data( &temp_llc, 
                               &negotiated_capability->bearer_capability_1,
                               service_ptr );
   
   /* Format IE */
   negotiated_capability->low_layer_compatibility_1.length =
     (uint8)ds_ucsd_bc_put_llc_data(
                  negotiated_capability->low_layer_compatibility_1.data,
                  &temp_llc,
                  service_ptr );
 

#ifdef FEATURE_DATA_GCSD_FAX
   /* Check to see if we need to originate a FAX call and change 
   ** the appropriate parameters 
   */  
   if ( (dsat_num_item_type)dsatutil_get_val(
                                  DSAT_EXT_FCLASS_IDX,0,0,NUM_TYPE) == 1 )
   {
      if ( network_mode == SYS_SYS_MODE_GSM ) 
      {
         negotiated_capability->
            bearer_capability_1.information_transfer_capability = ITC_FAX_G3;

         chosen_capability->chosen_bc.connection_element = TRANSPARENT_DATA;

         negotiated_capability->bearer_capability_1.connection_element = TRANSPARENT_DATA;
         negotiated_capability->bearer_capability_1.structure          = UNSTRUCTURED;
         negotiated_capability->bearer_capability_1.sync_async         = SYNCHRONOUS;
         negotiated_capability->bearer_capability_1.modem_type         = NO_MODEM;
         negotiated_capability->bearer_capability_1.other_modem_type   = OMT_NONE;
         negotiated_capability->bearer_capability_1.rate_adaption      = NO_RATE_ADAPTION;

         /* Currently only support 9.6 for FAX */
         chosen_capability->chosen_bc.user_rate = USER_RATE_9600;

         negotiated_capability->bearer_capability_1.user_rate = USER_RATE_9600;
         negotiated_capability->bearer_capability_1.fixed_network_user_rate  = FNUR_UNDEFINED;
         negotiated_capability->bearer_capability_1.user_initiated_modification_indication   
                                                                   = UIMI_UNDEFINED;                                                          
         negotiated_capability->bearer_capability_1.wanted_air_interface_user_rate           
                                                                   = WAIUR_UNDEFINED;
         negotiated_capability->bearer_capability_1.acceptable_channel_codings = ACC_9600_ACCEPTABLE;
      }
      else
      {
         DATA_MSG1_ERROR( "Originate fax call in wrong mode: mode=%d", 
                    network_mode);
      }
   }
#endif /* FEATURE_DATA_GCSD_FAX */

   /* Now set the compression value... no compression if TRANSPARENT, 
   ** compression possible only for non-transparent. 
   */
   if ( (chosen_capability->chosen_bc.connection_element == TRANSPARENT_DATA) ||
        (rlp_parameters->rlp_P0 == 0) )
   {
      negotiated_capability->bearer_capability_1.compression = 
         DATA_COMPRESSION_NOT_POSSIBLE_OR_ALLOWED;
   }
   else
   {
      negotiated_capability->bearer_capability_1.compression = 
         DATA_COMPRESSION_POSSIBLE_OR_ALLOWED;
   }
  

} /* ds_ucsd_bc_copy_chosen_cap() */


/*===========================================================================

FUNCTION ds_ucsd_bc_init_orig_params

DESCRIPTION
   Copies default capability and other origination information to the
   input orig_params structure.  The information copied depends on
   user preference setting passed in via the chosen_capability
   parameter.  In addition, the bearer capability, low layer
   compatibility and high layer compatibility information filled in to
   the orig_params structure is copied to the input
   negotiated_capability structure if this parameter is not set to
   NULL.
   
   For dual mode calls, it is assumed the preferred mode is Data while
   the alternate mode is Voice.  The call_type parameter indicates the
   modes required for the call.
         
DEPENDENCIES
  None
  
RETURN VALUE
  None
    
SIDE EFFECTS
  Changes the orig_params and possibly the negotiated_capability structure
  information.
===========================================================================*/
void 
ds_ucsd_bc_init_orig_params 
(
   ds_ucsd_call_tbl_entry_type  *call_info_ptr,        /* Call instance
                                                       */
   const uint8                  *dial_str,             /* Pointer to the dial
                                                       ** string for
                                                       ** the ATD command
                                                       */
                
   const uint8                   dial_str_len,         /* Length of dial string
                                                       */
   
   
   const sys_sys_mode_e_type     network_mode,         /* Current network mode
                                                        */
   
   const ds_ucsd_nt_rlp_parameters_T  *rlp_parameters, /* Current RLP parameters 
                                                       */ 

   cm_gw_cs_orig_params_s_type  *orig_params,          /* Pointer to origination 
                                                       ** parameters
                                                       */
   
   ds_ucsd_chosen_capability_T  *chosen_capability,    /* User set parameters
                                                       ** via UI or AT CBST
                                                       ** commands.
                                                       */

   ds_ucsd_negotiated_capability_T *negotiated_capability
                                                       /* Filled in with all bearer
                                                       ** capability info including
                                                       ** LLC in structure format.
                                                       ** Can be set to NULL if you
                                                       ** don't want a copy of it.
                                                       */

)
{
   ds_ucsd_low_layer_compatibility_T     temp_llc;
   int                                   llc_length;
   
   
   /* Logic below will set bearer_capability_1 and
   ** low_layer_compatibility_1 for data call
   */

   /* Invalidate BC repeat indicator and BC #2 */   
   orig_params->bc_repeat_indicator.present        = FALSE;
   orig_params->bearer_capability_2.present        = FALSE;
   
   /* Invalidate LLC repeat indicator and LLC #2 */
   orig_params->llc_repeat_indicator.present       = FALSE;
   orig_params->low_layer_compatibility_2.present  = FALSE;
   
   /* Invalidate HLC repeat indicator and HLC #1 & #2 */
   orig_params->hlc_repeat_indicator.present       = FALSE;
   orig_params->high_layer_compatibility_2.present = FALSE;
   
   /* Set cm_clir to invalid */
   orig_params->cm_clir = 0;

   /* Initialize call_control_completed to FALSE */
#ifdef FEATURE_GSTK
   orig_params->call_control_completed = FALSE;
#endif

      
   /*********************************/
   /* SINGLE MODE - DATA CALL SETUP */
   /*********************************/

   /* Initialize the BC and LLC structures */
   ds_ucsd_init_orig_bc_data (&orig_params->bearer_capability_1,
                               chosen_capability,
                               &call_info_ptr->bearer_params );
   
   memset ((void*)&temp_llc, 0x0, sizeof (ds_ucsd_low_layer_compatibility_T));
   
   /* LLC specified for UDI/RDI ITC per 3GPP TS27.001 B.2.1.1 */
   orig_params->low_layer_compatibility_1.present =
     chosen_capability->chosen_llc.present;
   
   if (orig_params->low_layer_compatibility_1.present)
   {
     /* Initialize LLC structure */
     ds_ucsd_init_orig_llc_data ( &temp_llc, 
                                  &orig_params->bearer_capability_1,
                                  &call_info_ptr->bearer_params );

     /* Overrwrite select elements to match BC */
     temp_llc.sync_async =
       (ASYNCHRONOUS == chosen_capability->chosen_bc.sync_async)?
          LLC_ASYNCHRONOUS : LLC_SYNCHRONOUS;

     temp_llc.user_rate = chosen_capability->chosen_llc.user_rate;
      
     /* Format IE */
     llc_length = ds_ucsd_bc_put_llc_data( orig_params->
                                           low_layer_compatibility_1.data,
                                           &temp_llc,
                                           &call_info_ptr->bearer_params );
                              
     orig_params->low_layer_compatibility_1.length = (uint8)llc_length;
   }

   /* HLC ununsed at this time */
   orig_params->high_layer_compatibility_1.present = FALSE;

   
#ifdef FEATURE_DATA_GCSD_FAX
   /* Check to see if we need to originate a FAX call and change 
   ** the appropriate parameters 
   */  
   if ( (dsat_num_item_type)dsatutil_get_val(
                                  DSAT_EXT_FCLASS_IDX,0,0,NUM_TYPE) == 1 )
   {
      if ( network_mode == SYS_SYS_MODE_GSM ) 
      {
         orig_params->bearer_capability_1.information_transfer_capability = 
                                                                  ITC_FAX_G3;

         chosen_capability->chosen_bc.connection_element = TRANSPARENT_DATA;

         orig_params->bearer_capability_1.connection_element = TRANSPARENT_DATA;
         orig_params->bearer_capability_1.structure          = UNSTRUCTURED;
         orig_params->bearer_capability_1.sync_async         = SYNCHRONOUS;
         orig_params->bearer_capability_1.modem_type         = NO_MODEM;
         orig_params->bearer_capability_1.other_modem_type   = OMT_NONE;
         orig_params->bearer_capability_1.rate_adaption      = NO_RATE_ADAPTION;

         /* Currently only support 9.6 for FAX */
         chosen_capability->chosen_bc.user_rate = USER_RATE_9600;

         orig_params->bearer_capability_1.user_rate = USER_RATE_9600;
         orig_params->bearer_capability_1.fixed_network_user_rate  = FNUR_UNDEFINED;
         orig_params->bearer_capability_1.user_initiated_modification_indication   
                                                                   = UIMI_UNDEFINED;                                                          
         orig_params->bearer_capability_1.wanted_air_interface_user_rate           
                                                                   = WAIUR_UNDEFINED;
         orig_params->bearer_capability_1.acceptable_channel_codings = ACC_9600_ACCEPTABLE;
      }
      else
      {
         DATA_MSG1_ERROR("Originate fax call in wrong mode: mode=%d", 
                       network_mode);
      }
   }
#endif /* FEATURE_DATA_GCSD_FAX */

   /* Now set the compression value... no compression if TRANSPARENT, 
   ** compression possible only for non-transparent. 
   */
   if ( (chosen_capability->chosen_bc.connection_element == TRANSPARENT_DATA) ||
        (rlp_parameters->rlp_P0 == 0) )
   {
      orig_params->bearer_capability_1.compression = 
         DATA_COMPRESSION_NOT_POSSIBLE_OR_ALLOWED;
   }
   else
   {
      orig_params->bearer_capability_1.compression = 
         DATA_COMPRESSION_POSSIBLE_OR_ALLOWED;
   }
         
   /* Handle case where user indicates autobaud for modem type.  Note
    * for UMTS the WAIUR must be specified per 3GPP TS27.001 section
    * 8.3.3.2 so up to octet 6f must be included in the setup message.
    * Set FNUR to 'not applicable' as using 'undefined' will suppress
    * octets 6d-f.  For GSM, use 'undefined' to suppress unnecessary
    * octets.
    */
   if ( AUTO_TYPE_1 == orig_params->bearer_capability_1.modem_type )
   {
     orig_params->bearer_capability_1.fixed_network_user_rate =
       ( network_mode == SYS_SYS_MODE_GSM )? FNUR_UNDEFINED :
                                             FNUR_NOT_APPLICABLE;
   }
   
   /* Now work on the the other information in the orig_params structure */
   
   /* Start with the called party sub-address that needs to be derived 
   ** from the incoming number.  Note that the called party bcd number
   ** will be set outside of the UCSD subsystem -- probably by CM.
   */
   (void)ds_ucsd_bc_get_called_party_subaddress
                          (dial_str, 
                           dial_str_len, 
                           &orig_params->called_party_subaddress);
   
   
   /* Now get the calling party information */ 
#ifdef FEATURE_MMGSDI_SESSION_LIB
#ifdef FEATURE_DUAL_SIM
   if( ds_ucsd_get_calling_party_subaddress(
               ds3g_get_cs_subs_id(),
               &orig_params->calling_party_subaddress) != TRUE )
   {
     DATA_MSG0_ERROR("Failed to get calling party subaddress");
   }
#else
   if( ds_ucsd_get_calling_party_subaddress(
               &orig_params->calling_party_subaddress ) != TRUE )
   {
     DATA_MSG0_ERROR( "Failed to get calling party subaddress");
   }
#endif /* FEATURE_DUAL_SIM */
#endif /* FEATURE_MMGSDI_SESSION_LIB */
   
  
   /* Set the call instance call type if unknown */
   if( DS_UCSD_UNKNOWN_CALL == call_info_ptr->call_type )
   {
     const cm_bearer_capability_s_type  *data_bc_ptr;
     
     call_info_ptr->call_type =
       ds_ucsd_determine_call_type( &orig_params->bearer_capability_1,
                                    NULL,
                                    NULL,
                                    &data_bc_ptr,
                                    NULL );
   }

#ifdef FEATURE_MULTIMEDIA_FALLBACK
   #error code not present
#endif /* FEATURE_MULTIMEDIA_FALLBACK */

   /* Setup negotiated_capability data if needed */
   if (negotiated_capability != NULL)
   {
     COPY_BEARER_CONFIG( negotiated_capability, orig_params );
   }


} /* ds_ucsd_bc_init_orig_params() */


#ifdef FEATURE_MULTIMEDIA_FALLBACK 
#error code not present
#endif /* FEATURE_MULTIMEDIA_FALLBACK */


/*===========================================================================

FUNCTION ds_ucsd_bc_check_and_negotiate_bc

DESCRIPTION
   Checks the input bearer capabilities and low level compatibilities 
   (contained in the call_info parameter) and determines if the
   non-negotiable parameters are supported, and also negotiates any
   negotiable parameters that are not supported.  If any negotiation
   is required, the bc_changed parameter will be set to TRUE, and the
   negotiated_capabilities parameter will contain the negotiated set
   of capabilities.  If negotiation is not required bc_changed will be
   set to FALSE, and negotiated_capabilities will contain the set
   of capabilities for the call.
   
         
DEPENDENCIES
  None
  
RETURN VALUE
  TRUE if all of the non-negotiable parameters are supported, FALSE if 
  not. 
    
SIDE EFFECTS
  Changes the orig_params and possibly the negotiated_capability structure
  information.
===========================================================================*/
boolean 
ds_ucsd_bc_check_and_negotiate_bc 
(
   cm_call_mode_info_s_type           *call_info,
   const cm_bearer_capability_s_type  *data_bc_ptr,
   ds_ucsd_chosen_capability_T        *chosen_capability,
   ds_ucsd_negotiated_capability_T    *negotiated_capability,
   ds_ucsd_nt_rlp_parameters_T        *rlp_parameters,
   const ds_ucsd_bearer_service_type  *service_ptr,
   boolean                            *bc_changed,
   boolean                             chosen_capability_is_valid,
   sys_sys_mode_e_type                 network_mode,
   sys_modem_as_id_e_type              subs_id
)
{
   boolean c_capability_ok = FALSE;
   cm_bearer_capability_s_type        *neg_bc_ptr;
   cm_bearer_capability_s_type        *req_bc_ptr;
   cm_low_layer_compatibility_s_type  *req_llc_ptr;

   /* Set a pointer to the bearer capability information */
   cm_gw_cs_call_info_s_type *requested_capability =
      &call_info->info.gw_cs_call;

   /* Initialize the bc_changed parameter */
   *bc_changed             = FALSE;

   /*check that requested service for MT call is supported by the MS
   *high layer compatibility not relevant for async transparent services
   *see GSM 07.01 sect 8.3.3, Annex B1.1, B1.2, B2.1 and GSM 04.08
   *sect 5.3.4.2.1 and Annex B3.1, Annex C*/

#ifdef SIMULATE_ROHDE  /* for testing only */
   requested_capability->bearer_capability_1.present = FALSE;
#endif

   /* Setup pointers for the data mode parameters */
   if( (data_bc_ptr == &call_info->info.gw_cs_call.bearer_capability_1) ||
       (data_bc_ptr == NULL) )
   {
     neg_bc_ptr  = &negotiated_capability->bearer_capability_1;
     req_bc_ptr  = &requested_capability->bearer_capability_1;
     req_llc_ptr = &requested_capability->low_layer_compatibility_1;
   }
   else
   {
     neg_bc_ptr  = &negotiated_capability->bearer_capability_2;
     req_bc_ptr  = &requested_capability->bearer_capability_2;
     req_llc_ptr = &requested_capability->low_layer_compatibility_2;
   }

   if( req_bc_ptr->present == FALSE )
   {
      /*no bearer capability in message*/
      
      DATA_MSG0_HIGH( "No Bearer Capability in SETUP message");

      if (chosen_capability_is_valid)
      {
         ds_ucsd_bc_copy_chosen_cap(negotiated_capability,
                                    chosen_capability,
                                    rlp_parameters,
                                    service_ptr,
                                    network_mode);
      
         c_capability_ok = TRUE;
      
         *bc_changed = TRUE;
      }
      else
      {
         DATA_MSG0_ERROR( "Cannot fill in Bearer Capability" );

         c_capability_ok = FALSE;
      
         *bc_changed = FALSE;
      }
      
   }
   else /* Check and negotiate */
   {
      ds_ucsd_low_layer_compatibility_T   temp_llc;
   
      /* Copy and convert LLC data from requested structure
      ** to negotiated structure.  This gets the data in the
      ** format to be used by below ds_ucsd_bc_check_non_neg_params()
      */
      (void)ds_ucsd_bc_get_llc_data( req_llc_ptr->data,
                                     req_llc_ptr->length,
                                     &temp_llc );
          
      /* Check to see if non-negotiable parameters are supported */
      /* Check only non-negotiable params */
      if( TRUE == ds_ucsd_bc_check_params( req_bc_ptr,
                                           &temp_llc,
                                           network_mode,
                                           FALSE ) )
      {
         /* non-negotiable parameters OK */
         c_capability_ok = TRUE;

         /* copy requested bearer capability to 
         ** negotiated_capability structure... LLC has
         ** already been copied to negotiated_capability
         */
         *neg_bc_ptr = *req_bc_ptr; 

         /* Negotiate the negotiable values. */
         c_capability_ok  =
           ds_ucsd_bc_negotiate_bearer_capability( neg_bc_ptr,
                                                   network_mode);

         if (TRUE == c_capability_ok)
         {
           /* Check to see if negotiated BC is different from the original
           ** requested BC
           */
           *bc_changed = ds_ucsd_bc_bcs_are_different( req_bc_ptr,
                                                       neg_bc_ptr );
                            
           /* If we haven't changed the bearer capabilities, check to see 
           ** if the network has sent the Phase2+ elements, these
           ** must always be returned (see 07.01 para. 8.3.3.1) 
           */
           if ((*bc_changed == FALSE) && 
               (req_bc_ptr->fixed_network_user_rate != FNUR_UNDEFINED))
           {
#if defined LOGDEBUG
             ds_log("DS:FNUR defined, returning bc1\n");
#endif
             *bc_changed = TRUE;
           }
         }
      }
      else
      {
         /* Fill in reject cause? */
         /*
         call_info->info.gsm_wcdma_call_info.cc_cause = INCOMPATIBLE_DESTINATION;
         */
      }
   }

#if defined(FEATURE_MULTIMEDIA_FALLBACK) && \
    !defined(FEATURE_DATA_UCSD_SCUDIF_NOBEARERSWAP)
   #error code not present
#endif /* FEATURE_MULTIMEDIA_FALLBACK && !FEATURE_DATA_UCSD_SCUDIF_NOBEARERSWAP */
   
#if defined LOGDEBUG
   if(debug_control.ds_debug)
   {
      qsprintf(ds_debug_out,
               "DS:neg_cap.bc1.ce=%d ur=%d\n",
               neg_bc_ptr->connection_element,
               neg_bc_ptr->user_rate);
      ds_log(ds_debug_out);
   }
#endif

   return c_capability_ok;
} /* ds_ucsd_bc_check_and_negotiate_bc() */



/*===========================================================================

FUNCTION ds_ucsd_bc_check_bc

DESCRIPTION
   Checks the input bearer capabilities and low level compatibilities 
   (contained in the call_info parameter) and determines if the
   all parameters are supported.   If all the parameters are supported (i.e. the
   function returns TRUE), the
   negotiated_capabilities will contain the set of capabilities for the call.
   If the parameters are not supported, the info.gsm_wcdma_call_info.cc_cause
   field of call_info will be updated with a rejection cause value.
   
         
DEPENDENCIES
  None
  
RETURN VALUE
  TRUE if all of the parameters are supported, FALSE if 
  not. 
    
SIDE EFFECTS
  Changes the call_info and possibly the negotiated_capability structure
  information.
===========================================================================*/
boolean 
ds_ucsd_bc_check_bc 
(
   cm_call_mode_info_s_type         *call_info,
   const cm_bearer_capability_s_type  *data_bc_ptr,
   sys_sys_mode_e_type               network_mode,
   ds_ucsd_negotiated_capability_T  *negotiated_capability
)
{
   boolean c_capability_ok = FALSE;
   cm_bearer_capability_s_type        *neg_bc_ptr;
   cm_bearer_capability_s_type        *req_bc_ptr;
   cm_low_layer_compatibility_s_type  *req_llc_ptr;
   ds_ucsd_low_layer_compatibility_T   temp_llc;

   /* Set a pointer to the bearer capability information */
   /* Note: the possibly modified bearer capability is 
    *       found in the 'call_details' member.  */
   cm_gw_cs_call_info_s_type *requested_capability =
      &call_info->info.gw_cs_call;


   /*check that requested service for MT call is supported by the MS
   *high layer compatibility not relevant for async transparent services
   *see GSM 07.01 sect 8.3.3, Annex B1.1, B1.2, B2.1 and GSM 04.08
   *sect 5.3.4.2.1 and Annex B3.1, Annex C*/


    /*set all IEIs in negotiated_capability to null by default*/
   negotiated_capability->bc_repeat_indicator.present        = FALSE;
   negotiated_capability->bearer_capability_1.present        = FALSE;
   negotiated_capability->bearer_capability_2.present        = FALSE;
   
   negotiated_capability->llc_repeat_indicator.present       = FALSE;
   negotiated_capability->low_layer_compatibility_1.length   = 0;
   negotiated_capability->low_layer_compatibility_2.length   = 0;
   
   negotiated_capability->hlc_repeat_indicator.present       = FALSE;
   negotiated_capability->high_layer_compatibility_1.present = FALSE;
   negotiated_capability->high_layer_compatibility_2.present = FALSE;

   /* Setup pointers for the data mode parameters */
   if( (data_bc_ptr == &call_info->info.gw_cs_call.bearer_capability_1) ||
       (data_bc_ptr == NULL) )
   {
     neg_bc_ptr  = &negotiated_capability->bearer_capability_1;
     req_bc_ptr  = &requested_capability->bearer_capability_1;
     req_llc_ptr = &requested_capability->low_layer_compatibility_1;
   }
   else
   {
     neg_bc_ptr  = &negotiated_capability->bearer_capability_2;
     req_bc_ptr  = &requested_capability->bearer_capability_2;
     req_llc_ptr = &requested_capability->low_layer_compatibility_2;
   }


   /* Copy an convert LLC data from requested structure
   ** to negotiated structure.  This gets the data in the
   ** format to be used by below ds_ucsd_bc_check_params()
   */
   (void)ds_ucsd_bc_get_llc_data( req_llc_ptr->data,
                                  req_llc_ptr->length,
                                  &temp_llc );
       
   /* check to see if non-negotiable parameters are supported */
   if ( TRUE == ds_ucsd_bc_check_params( req_bc_ptr,
                                         &temp_llc,
                                         network_mode,
                                         TRUE /* Check all parameters*/) )
   {
      /*non-negotiable parameters OK */
      c_capability_ok = TRUE;

      /* copy requested bearer capability to 
      ** negotiated_capability structure... LLC has
      ** already been copied to negotiated_capability
      */
      *neg_bc_ptr = *req_bc_ptr;
   }   
   else
   {
      /* Fill in reject cause? */
      /*
      call_info->info.gsm_wcdma_call_info.cc_cause = INCOMPATIBLE_DESTINATION;
      */
   }

#if defined LOGDEBUG
   if(debug_control.ds_debug)
   {
      qsprintf(ds_debug_out,
               "DS:neg_cap.bc1.ce=%d ur=%d\n",
               neg_bc_ptr->connection_element,
               neg_bc_ptr->user_rate);
      ds_log(ds_debug_out);
   }
#endif

   return c_capability_ok;
} /* ds_ucsd_bc_check_bc() */

/*===========================================================================

FUNCTION ds_ucsd_determine_call_type

DESCRIPTION
  This function determine the internal CS Data call type based on the
  setup paramaters.  The supported call types are defined in
  ds_ucsd_call_type.

  Only single-mode calls (one bearer capability defined) are supported
  at this time.
  
DEPENDENCIES
  NV items for internal multimedia call routing and synchronous call test 
  applicaiotn must be read before calling this routine.
  
RETURN VALUE
  TRUE if operation successful; FALSE otherwise.
    
SIDE EFFECTS
  None

===========================================================================*/
ds_ucsd_call_type
ds_ucsd_determine_call_type
(
  const cm_bearer_capability_s_type  *bc1_ptr,     /* Preferred mode BCIE */
  const cm_bearer_capability_s_type  *bc2_ptr,     /* Secondary mode BCIE */
  const cm_repeat_indicator_s_type   *bc_ri_ptr,   /* BC Repeat Indicator */
  const cm_bearer_capability_s_type **data_bc_pptr,/* Data mode BCIE      */
  const cm_bearer_capability_s_type **voice_bc_pptr/* Voice mode BCIE     */
)
{
  ds_ucsd_call_type result = DS_UCSD_UNKNOWN_CALL;
  /* Unset CSVT call global variable at start of each call
     function ds_ucsd_determine_call_type is called for all MO MT calls */
  ds_ucsd_is_csvt_call = FALSE;
  
  ASSERT( NULL != bc1_ptr );
  ASSERT( NULL != data_bc_pptr );

  /* Check first (preferred) bearer capability */
  if( TRUE == bc1_ptr->present )
  {
    switch( bc1_ptr->information_transfer_capability )
    {
    case ITC_UDI:
    case ITC_3_1K:
    case ITC_FAX_G3:
    case ITC_OTHER:
      *data_bc_pptr = bc1_ptr;
      
      /* Check for the supported call types */
      if( SYNCHRONOUS == (*data_bc_pptr)->sync_async )
      {
        /* Case: Synchronous call */
        /* Case: WCDMA Video Telephony (multimedia) call */
        /* Determine if this is an internal VT call, which must meet these 
        ** conditions: 
        ** - the BC-IE other_rate_adaption field is H.223 & H.245
        ** - the network mode is WCDMA 
        ** - Synchronous test app is disabled
        */
        if( (DS_UCSD_IS_WCDMA_TDS_MODE(ds_ucsd_get_network_mode())) &&
            (H223_H245 == (*data_bc_pptr)->other_rate_adaption) )
        {
          /* Check NV parameters for test app disabled to consider
           * call internal VT. */
          if( FALSE == ds_ucsd_is_sync_test_app_enabled() )
          {
            result = DS_UCSD_VIDEO_TELEPHONY_CALL;
            /* Set CSVT call global variable */
            ds_ucsd_is_csvt_call = TRUE;
          }
          else
          {
            result = DS_UCSD_SYNC_CSD_CALL;
          }
        }
        else
        if( (SYS_SYS_MODE_GSM == ds_ucsd_get_network_mode()) &&
            (ITC_FAX_G3 == (*data_bc_pptr)->information_transfer_capability) )
        {
          result = DS_UCSD_FAX_CALL;
        }
        else
        {
          result = DS_UCSD_SYNC_CSD_CALL;
        }
      
      }
      else if( ASYNCHRONOUS == (*data_bc_pptr)->sync_async )
      {
        /* Case: Asynchronous call */
        result = DS_UCSD_ASYNC_CSD_CALL;
      }
      else
      {
        DATA_MSG1_ERROR( "Unrecognized call connection_element: %d",
             (*data_bc_pptr)->sync_async);
      }

      DATA_MSG1_MED("CS Data Call Type: %d", result);

      /* Voice mode must be less-preferred BCIE if present */
      if( NULL != voice_bc_pptr )
      {
        if( (NULL != bc2_ptr) && (TRUE == bc2_ptr->present) &&
            (NULL != bc_ri_ptr) && (TRUE == bc_ri_ptr->present) )
        {
          *voice_bc_pptr = bc2_ptr;
        }
        else
        {
          *voice_bc_pptr = NULL;
        }
      }
      break;

    case ITC_SPEECH:
      /* Voice call case */
      result = DS_UCSD_VOICE_CALL;
      if( NULL != voice_bc_pptr )
      {
        *voice_bc_pptr = bc1_ptr;
      }
     
      /* Data mode must be less-preferred BCIE if present */
      if( (NULL != bc2_ptr) && (TRUE == bc2_ptr->present) &&
          (NULL != bc_ri_ptr) && (TRUE == bc_ri_ptr->present) )
      {
        *data_bc_pptr = bc2_ptr;
      }
      else
      {
        *data_bc_pptr = NULL;
      }
      break;

    default:
      DATA_MSG1_ERROR( "Unsupported ITC: %d",
                bc1_ptr->information_transfer_capability);
    
    }
  }

  return result;
} /* ds_ucsd_determine_call_type() */

#ifdef FEATURE_MULTIMEDIA_FALLBACK 
#error code not present
#endif /* FEATURE_MULTIMEDIA_FALLBACK */



/*===========================================================================

FUNCTION ds_ucsd_validate_repeat_indicator

DESCRIPTION
  This function validates the SETUP message repeat indicator.
  Alternating modes are not supported at this time EXCEPT for service
  change & fallack for UDI VT calls if SCUDIF is enabled.

DEPENDENCIES
   None

RETURN VALUE
   TRUE if the repeat indicator is supported, FALSE otherwise.

SIDE EFFECTS
   None

===========================================================================*/
/* ARGSUSED */
boolean ds_ucsd_validate_repeat_indicator
(
  const cm_call_mode_info_s_type     *mode_info_ptr,
  const cm_bearer_capability_s_type  *data_bc_ptr,
  cm_setup_res_params_s_type         *setup_res_params
)
{
  boolean result = TRUE;
  
  /* Check for bearer repeat indicator specified */
  if( TRUE == mode_info_ptr->info.gw_cs_call.bc_repeat_indicator.present )
  {
    uint8 repeat_indication =
      mode_info_ptr->info.gw_cs_call.bc_repeat_indicator.repeat_indication;

    DATA_MSG1_MED( "Repeat indicator present - ri=%d", repeat_indication);

    /* We do not support alternating modes unless for VT call and
     * SCUDIF support enabled. */
    if( (REPEAT_SERVICE_CHANGE_FALLBACK == repeat_indication) ||
        (REPEAT_SUPPORTS_FALLBACK       == repeat_indication) )
    {
#ifdef FEATURE_MULTIMEDIA_FALLBACK
      #error code not present
#endif /* FEATURE_MULTIMEDIA_FALLBACK */
      {
        /* Reject the call with cause CONDITIONAL_IE_ERROR */
        /* See: 3GPP TS24.008 v6.8.0 section 5.5.3.2.2     */
        /*      3GPP TS23.172 v6.2.0 section 4.2.2         */
        REJECT_CALL( setup_res_params, CONDITIONAL_IE_ERROR );
        DATA_MSG0_ERROR( "CS Data call setup failed - conditional IE error" );
        result = FALSE;
      }
    }
    else
    {
      /* Reject the call with cause INCOMPATIBLE_DESTINATION */
      /* See 3GPP TS24.008 Annex B.3.2                       */
      REJECT_CALL( setup_res_params, INCOMPATIBLE_DESTINATION );
      DATA_MSG0_ERROR(  "CS Data call setup failed - incompatible destination");
      result = FALSE;
    }
  }
  
  return result;
} /* ds_ucsd_validate_repeat_indicator() */



/*===========================================================================

FUNCTION  ds_ucsd_bc_init

DESCRIPTION
  This function performs powerup initialization steps for this module.
         
DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void 
ds_ucsd_bc_init( void )
{
  
#ifdef FEATURE_MULTIMEDIA_FALLBACK 
 #error code not present
#endif /* FEATURE_MULTIMEDIA_FALLBACK */
#ifdef FEATURE_MMGSDI_SESSION_LIB
  memset( ds_ucsd_sim_addr_info,
          0x0,
          DS_UCSD_MAX_SUB_ADDR_ARRAY_INDEX*sizeof(dsucsd_sim_addr_info_type) );
#endif /* FEATURE_MMGSDI_SESSION_LIB */
  
} /* ds_ucsd_bc_init() */

#ifdef FEATURE_DUAL_SIM

/*===========================================================================
FUNCTION      DS_UCSD_GET_PREF_SUBS_ID_FOR_VT_CALL

DESCRIPTION   This function checks the mode preference and calculates 
              the preferred SUBS ID and sets UCSD Mode and subs id.

DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  None.
===========================================================================*/
void ds_ucsd_get_pref_subs_id_for_vt_call
(
  sys_sys_mode_e_type    *csd_mode,
  sys_modem_as_id_e_type *csd_subs_id
)
{
  sys_sys_mode_e_type  sys_mode_sub;
  cm_mode_pref_e_type  mode_pref;
  sys_modem_as_id_e_type subs_id;
  uint8 index = 0;
  uint32 allowed_flag[DS3G_MAX_SUBS];
  boolean preferred = FALSE;
  boolean any_allowed = FALSE;
  uint8 pref_index = 0xFF;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for( index = 0;index <DS3G_MAX_SUBS;index++)
  {
    mode_pref = 
         ds3gsubsmgr_get_mode_pref_for_subs_id((sys_modem_as_id_e_type)index);
    switch (mode_pref)
    {
      case CM_MODE_PREF_GSM_ONLY:
      case CM_MODE_PREF_GSM_WLAN:
      case CM_MODE_PREF_GSM_LTE:
      case CM_MODE_PREF_GSM_GPS:
      case CM_MODE_PREF_CDMA_GSM_LTE:
      case CM_MODE_PREF_CDMA_GSM:
      case CM_MODE_PREF_CDMA_GSM_AMPS:
      case CM_MODE_PREF_CDMA_GSM_GPS_AMPS:
      case CM_MODE_PREF_HDR_GSM_LTE:
      case CM_MODE_PREF_HDR_GSM:
      case CM_MODE_PREF_CDMA_HDR_GSM:
      case CM_MODE_PREF_CDMA_HDR_GSM_AMPS:
        allowed_flag[index]= DS_UCSD_GSM_ALLOWED;
        break;
        
      case CM_MODE_PREF_WCDMA_ONLY:
      case CM_MODE_PREF_WCDMA_WLAN:
      case CM_MODE_PREF_WCDMA_LTE:
      case CM_MODE_PREF_WCDMA_GPS:
      case CM_MODE_PREF_CDMA_WCDMA_LTE:
      case CM_MODE_PREF_HDR_WCDMA_LTE:
        allowed_flag[index]= DS_UCSD_WCDMA_ALLOWED;
        preferred = TRUE;
        any_allowed = TRUE;
        break;
        
      case CM_MODE_PREF_TDS_ONLY:
      case CM_MODE_PREF_TDS_LTE:
        allowed_flag[index]= DS_UCSD_TDS_ALLOWED;
        preferred = TRUE;
        any_allowed = TRUE;
        break;
        
      case CM_MODE_PREF_GSM_WCDMA_ONLY:
      case CM_MODE_PREF_GW_WLAN:
      case CM_MODE_PREF_GWL:
      case CM_MODE_PREF_GWL_WLAN:
      case CM_MODE_PREF_GW_GPS:
        allowed_flag[index]= (DS_UCSD_GSM_ALLOWED|DS_UCSD_WCDMA_ALLOWED);
        any_allowed = TRUE;
        break;

      case CM_MODE_PREF_CDMA_GW:
      case CM_MODE_PREF_CDMA_GSM_WCDMA_LTE:
        allowed_flag[index]= (DS_UCSD_GSM_ALLOWED|DS_UCSD_WCDMA_ALLOWED);
        any_allowed = TRUE;
        break;
        
      case CM_MODE_PREF_CDMA_HDR_GW:
      case CM_MODE_PREF_CDMA_HDR_GSM_WCDMA_LTE:
        allowed_flag[index]= (DS_UCSD_GSM_ALLOWED|DS_UCSD_WCDMA_ALLOWED);
        any_allowed = TRUE;
        break;

      case CM_MODE_PREF_TDS_GSM:
      case CM_MODE_PREF_TDS_GSM_LTE:
      case CM_MODE_PREF_CDMA_HDR_GSM_TDS_LTE:
        allowed_flag[index]= (DS_UCSD_GSM_ALLOWED|DS_UCSD_TDS_ALLOWED);
        any_allowed = TRUE;
        break;

      case CM_MODE_PREF_TDS_GSM_WCDMA_LTE:
      case CM_MODE_PREF_TDS_GSM_WCDMA:
      case CM_MODE_PREF_CDMA_GW_TDS:
      case CM_MODE_PREF_CDMA_HDR_GW_TDS:
        allowed_flag[index]= (DS_UCSD_GSM_ALLOWED|DS_UCSD_WCDMA_ALLOWED|
                              DS_UCSD_TDS_ALLOWED);
        any_allowed = TRUE;
        break;

      case CM_MODE_PREF_TDS_WCDMA:
      case CM_MODE_PREF_TDS_WCDMA_LTE:
        allowed_flag[index]= (DS_UCSD_WCDMA_ALLOWED|DS_UCSD_TDS_ALLOWED);
        any_allowed = TRUE;
        break;

      default:
        break;

    }/* switch case */
    if( TRUE == preferred)
    {
      subs_id = (sys_modem_as_id_e_type)index;
      *csd_subs_id = subs_id;
      *csd_mode = ds3g_get_current_network_mode_ex(subs_id);
      DATA_MSG2_HIGH("Preferred mode(%d) and SUBS id (%d) VT call ; W/T only",
                      *csd_mode,*csd_subs_id);
      return;
    }
  }/* for loop */

  if(any_allowed == FALSE)
  {
    /*-----------------------------------------------------------------------
       Update with CS subs id and mode, change if no 
       subs id is allowed for  WCDMA/TDS mode with default CS subs id 
    -----------------------------------------------------------------------*/
    *csd_subs_id = ds3g_get_voice_subs_id();
    *csd_mode     = ds3g_get_current_network_mode_ex(*csd_subs_id);
    DATA_MSG2_HIGH("default voice as Preferred mode(%d) and SUBS id(%d) VT call",
          *csd_mode,*csd_subs_id);
    return;
  }
 
  for( index = 0;index <DS3G_MAX_SUBS;index++)
  {
    subs_id = (sys_modem_as_id_e_type)index;
    sys_mode_sub = ds3g_get_current_network_mode_ex(subs_id);
    if ((sys_mode_sub == SYS_SYS_MODE_WCDMA) ||
        (sys_mode_sub == SYS_SYS_MODE_TDS))
    {
      if ( (allowed_flag[index]|DS_UCSD_WCDMA_ALLOWED) ||
           (allowed_flag[index]|DS_UCSD_TDS_ALLOWED) )
      {
        *csd_subs_id = subs_id;
        *csd_mode = sys_mode_sub;
        DATA_MSG2_HIGH("Preferred mode(%d)and SUBS id(%d) VT call camped to W/T",
                 *csd_mode,*csd_subs_id);
        return;
      }
      if(0xFF == pref_index)
      {
        /* Always stores the first preferred index where W or T allowed */
        pref_index= index;
      }
      
    }
  }

  if(0xFF != pref_index)
  {
    subs_id = (sys_modem_as_id_e_type)pref_index;
    *csd_subs_id = subs_id;
    *csd_mode = ds3g_get_current_network_mode_ex(subs_id);
    DATA_MSG2_HIGH("Preferred mode(%d) and SUBS id (%d) VT call ",
                 *csd_mode,*csd_subs_id);
    return;
  }

  return;
} /* ds_ucsd_get_pref_subs_id_for_vt_call */

/*===========================================================================
FUNCTION ds_ucsd_set_call_mode_and_subs_id

DESCRIPTION
   If the call type is video call then the subs id set to
   the WCDMA subs id and call mode to WCDMA.

DEPENDENCIES
   None
  
RETURN VALUE
   None
    
SIDE EFFECTS
   None

===========================================================================*/
boolean ds_ucsd_set_call_mode_and_subs_id
(
  ds_ucsd_call_type      call_type,
  sys_sys_mode_e_type    *csd_mode,
  sys_modem_as_id_e_type *csd_subs_id
)
{
 sys_modem_dual_standby_pref_e_type stand_by_pref = 
                                             ds3g_get_standby_pref();
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 
  if( csd_mode == NULL)
  {
    DATA_MSG0_ERROR(" Null pointer passed for mode");
    return FALSE;
  }

  if( csd_subs_id == NULL)
  {
    DATA_MSG0_ERROR( " Null pointer passed for subs id");
    return FALSE;
  }

  /*-----------------------------------------------------------------------
     Update with CS subs id and mode, change if any 
     subs id is in  WCDMA mode , else CS subs id as 
     default 
  -----------------------------------------------------------------------*/
  *csd_subs_id = ds3g_get_voice_subs_id();
  *csd_mode = ds3g_get_current_network_mode_ex(*csd_subs_id);

  if( DS_UCSD_VIDEO_TELEPHONY_CALL == call_type )
  {
    if( (SYS_MODEM_DS_PREF_DUAL_STANDBY == stand_by_pref) ||
        (SYS_MODEM_DS_PREF_TRIPLE_STANDBY == stand_by_pref))
    {
      DATA_MSG0_HIGH( "Multi SIM Standby preference");

      /* get the SUBS whose mode preference allows TDS or WCDMA */
      ds_ucsd_get_pref_subs_id_for_vt_call(csd_mode,csd_subs_id);
    }
  }
  if( *csd_subs_id == SYS_MODEM_AS_ID_NONE)
  {
    return FALSE;
  }
  else
  {
    return TRUE;
  }
}/* ds_ucsd_set_call_mode_and_subs_id */
#endif /* FEATURE_DUAL_SIM */

#ifdef FEATURE_MMGSDI_SESSION_LIB
/*===========================================================================
FUNCTION DS_UCSD_STORE_BCD_NUMBER()

DESCRIPTION
  This function stores the BCD number in the ds_ucsd_sim_addr_info.
  The BCD number may come from MDISDN file or EXT file.

  If it is from MSISDN, the structure of the record will be like following:
  ----------------------------------------------------------------------
  |   Alpha     | Length |   TON   | Dailing  | CC record | Ext Record |
  | Identifier  |of BCD  | and NPI |  Number  |    ID     |    ID      |
  ----------------------------------------------------------------------
  |  24 Bytes   | 1 Byte | 1 Byte  | 10 bytes |  1 byte   |   1 Byte   |
  ----------------------------------------------------------------------
  The last byte gives the information of the extension record. If the last 
  byte is 0xFF then there is no more extension.

  If is is EXT, the structure of the record will be like following:
  ----------------------------------------------------------------------
  |  Record Type  |            Extension Data           |  Identifier  |
  ----------------------------------------------------------------------
  |    1 Byte     |              11 Bytes               |    1 Byte    |
  ----------------------------------------------------------------------
  If the first byte value is 0x02 then the information present is the BCD
  number extension. Last byte will give the extension record number.
  If the last byte is 0xFF then there is no extension data after this.

  Refer spec TS 131 102.

PARAMETERS
  cnf_data : Data received from mmgsdi.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None

===========================================================================*/
static void ds_ucsd_store_bcd_number
(
  mmgsdi_cnf_type      *cnf_data
)
{
  ds_ucsd_client_ref_e_type     client_ref;
  uint8                         *non_alpha_pos = NULL;
  mmgsdi_data_type              *read_data;
  uint8                         ext_index = 0xFF;
  mmgsdi_access_type            file_access;
  mmgsdi_session_id_type        session_id = DS_UCSD_INVALID_MMGSDI_SESSION_ID;
  mmgsdi_return_enum_type       mmgsdi_status = MMGSDI_MAX_RETURN_ENUM;
  uint8                         array_index = 0;
  mmgsdi_session_type_enum_type session_type;

  session_id = cnf_data->response_header.session_id;
  session_type = ds3g_mmgsdi_get_session_type_from_session_id(session_id);

  if ( (session_type == MMGSDI_GW_PROV_PRI_SESSION) ||
       (session_type == MMGSDI_GW_PROV_SEC_SESSION) )
  {
    ASSIGN_ARRAY_INDEX(session_type, array_index);
  }
  else
  {
    DATA_MSG0_ERROR( "session_id not matched");
    return;
  }

  client_ref = (ds_ucsd_client_ref_e_type)cnf_data->read_cnf.response_header.client_data;
  read_data  = &(cnf_data->read_cnf.read_data);

  switch( client_ref )
  {
    case DS_UCSD_READ_MSISDN:
      /*
      ** non_alpha_pos IS THE POSITION OF THE FIRST NON-ALPHANUMERIC
      ** BYTE IN THE MSISDN.
      */
      non_alpha_pos = read_data->data_ptr+ (read_data->data_len - 
                                           DS_UCSD_MSISDN_NONALPHA_LEN);
      /*
      ** COPY THE BCD NUMBER INTO THE ADDRESS STRUCTURE
      */

      memscpy( ds_ucsd_sim_addr_info[array_index].sim_bcd_number,
               sizeof(ds_ucsd_sim_addr_info[array_index].sim_bcd_number),
               non_alpha_pos + DS_UCSD_PHONE_NUM_POS,
               DS_UCSD_PHONE_NUM_LEN );
      ds_ucsd_sim_addr_info[array_index].bcd_num_len = 
         DS_UCSD_PHONE_NUM_LEN;

      /*
      ** IS THERE AN EXTENSION RECORD IDENTIFIER ?
      ** IF SO, IT COULD MEAN THAT THERE IS MORE BCD NUMBER, AND/OR
      ** A SUBADDRESS
      */
      ext_index = non_alpha_pos[DS_UCSD_PHONE_NUM_EXT1_POS];
      break;
      
    case DS_UCSD_READ_EXT:
      /*
      ** BEFORE WE COPY THE NEXT BIT OF THE BCD NUMBER,
      ** CHECK THAT OUR SIM_ADDR_INFO IS BIG ENOUGH TO TAKE IT.
      */
      if( (ds_ucsd_sim_addr_info[array_index].bcd_num_len + 
                DS_UCSD_EXT1_BODY_LEN) <=
          DSUCSD_MAX_BCD_PHONE_NUM_LEN )
      {
        memscpy( ds_ucsd_sim_addr_info[array_index].sim_bcd_number +
                 ds_ucsd_sim_addr_info[array_index].bcd_num_len,
                 DSUCSD_MAX_BCD_PHONE_NUM_LEN - (ds_ucsd_sim_addr_info[array_index].bcd_num_len),
                 read_data->data_ptr + DS_UCSD_EXT1_BODY_POS,
                 DS_UCSD_EXT1_BODY_LEN );
        ds_ucsd_sim_addr_info[array_index].bcd_num_len += DS_UCSD_EXT1_BODY_LEN;
        ext_index = *(read_data->data_ptr + DS_UCSD_EXT1_EXT1_POS);
      }
      else
      {
        /*
        ** BCD NUMBER TOO BIG, SO QUIT & RESET LENGTH TO 0
        */
        ext_index = DS_UCSD_SIM_PADDING;
        memset( ds_ucsd_sim_addr_info[array_index].sim_bcd_number,
                0x00,sizeof(ds_ucsd_sim_addr_info[array_index].sim_bcd_number));
      }
      break;

    default:
      break;
  }

  if( ext_index != DS_UCSD_SIM_PADDING )
  {
    /*
    ** EXT1 IS VALID, IT MAY CONTAIN A SUBADDRESS.
    ** LET'S CHECK IT OUT.
    */

    file_access.access_method = MMGSDI_EF_ENUM_ACCESS;
    if( ds3g_mmgsdi_get_app_type_from_session_id(session_id)== MMGSDI_APP_SIM)
    {
      file_access.file.file_enum = MMGSDI_TELECOM_EXT1;
    }
    else
    {
      file_access.file.file_enum = MMGSDI_USIM_EXT5;
    }

    client_ref = DS_UCSD_READ_EXT;

    if( (mmgsdi_status = ds3g_mmgsdi_session_read_record(
                              session_id,
                              file_access,
                              ext_index,
                              MMGSDI_STANDARD_READ_LEN,
                              ds_ucsd_mmgsdi_response_cb,
                              (mmgsdi_client_data_type)client_ref)) != MMGSDI_SUCCESS )
    {
      DATA_MSG1_ERROR( "mmgsdi_read_record failes Status = %d", mmgsdi_status);
    }
  }
  else
  {
    /* ie if len > 0 */
    if( ds_ucsd_sim_addr_info[array_index].sim_subaddress[0] != 0 )  
    {
      ds_ucsd_sim_addr_info[array_index].valid = TRUE;
      DATA_MSG1_HIGH( "Sub Addreess Valid for Index[%d].", array_index);
    }
    else
    {
      ds_ucsd_sim_addr_info[array_index].valid = FALSE;
      DATA_MSG0_MED( "No calling party subaddress from MMGSDI");
    }
  }
} /* ds_ucsd_store_bcd_number */

/*===========================================================================
FUNCTION DS_UCSD_STORE_SUB_ADDR()

DESCRIPTION
  This function stores the Sub address in the ds_ucsd_sim_addr_info.
  After reading the MSISDN file, if the last byte is a valid record number
  then we need to read that extension record.
  The structure of the EXT record will be like following:
  ----------------------------------------------------------------------
  |  Record Type  |            Extension Data           |  Identifier  |
  ----------------------------------------------------------------------
  |    1 Byte     |              11 Bytes               |    1 Byte    |
  ----------------------------------------------------------------------
  If the first byte value is 0x01 then the information present is the 
  subaddress. Last byte will give the extension record number.
  If the last byte is 0xFF then there is no extension data after this.

  Refer spec TS 131 102.

PARAMETERS
  cnf_data : Data received from mmgsdi.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None

===========================================================================*/
static void ds_ucsd_store_sub_addr
(
  mmgsdi_cnf_type      *cnf_data
)
{
  ds_ucsd_client_ref_e_type      client_ref;
  mmgsdi_data_type               *read_data;
  uint8                          ext_index = 0xFF;
  mmgsdi_access_type             file_access;
  mmgsdi_session_id_type         session_id = DS_UCSD_INVALID_MMGSDI_SESSION_ID;
  mmgsdi_return_enum_type        mmgsdi_status = MMGSDI_MAX_RETURN_ENUM;
  uint8                          array_index = 0;
  mmgsdi_session_type_enum_type  session_type;

  session_id = cnf_data->response_header.session_id;
  session_type = ds3g_mmgsdi_get_session_type_from_session_id(session_id);

  if ( (session_type == MMGSDI_GW_PROV_PRI_SESSION) ||
       (session_type == MMGSDI_GW_PROV_SEC_SESSION) )

  {
    ASSIGN_ARRAY_INDEX(session_type, array_index);
  }
  else
  {
    DATA_MSG0_ERROR( "session_id not matched");
    return;
  }

  client_ref = (ds_ucsd_client_ref_e_type)cnf_data->read_cnf.response_header.client_data;
  read_data = &(cnf_data->read_cnf.read_data);

  switch( client_ref )
  {
    case DS_UCSD_READ_EXT:
      /*
      ** BEFORE WE COPY THE NEXT BIT OF THE SUBADDRESS,
      ** CHECK THAT OUR SIM_ADDR_INFO IS BIG ENOUGH TO TAKE IT.
      */
      if( ds_ucsd_sim_addr_info[array_index].sub_addr_len + DS_UCSD_EXT1_BODY_LEN <=
          DSUCSD_MAX_SUBADDRESS_LEN )
      {
        memscpy( ds_ucsd_sim_addr_info[array_index].sim_subaddress +
                 ds_ucsd_sim_addr_info[array_index].sub_addr_len,
                 DSUCSD_MAX_SUBADDRESS_LEN,
                 read_data->data_ptr + DS_UCSD_EXT1_BODY_POS,
                 DS_UCSD_EXT1_BODY_LEN );
        ds_ucsd_sim_addr_info[array_index].sub_addr_len += DS_UCSD_EXT1_BODY_LEN;
        ext_index = *(read_data->data_ptr + DS_UCSD_EXT1_EXT1_POS);
      }
      else
      {
        /*
        ** SUBADDRESS TOO BIG, SO RESET LENGTH TO 0
        */
        ext_index = DS_UCSD_SIM_PADDING;
        memset( ds_ucsd_sim_addr_info[array_index].sim_subaddress,
                0x00,sizeof(ds_ucsd_sim_addr_info[array_index].sim_subaddress));
        /* gs_raise_warning */
      }
      break;

    default:
      break;
  }

  if( ext_index != DS_UCSD_SIM_PADDING )
  {
    /*
    ** EXT1 IS VALID, IT MAY CONTAIN A SUBADDRESS.
    ** LET'S CHECK IT OUT.
    */
    file_access.access_method = MMGSDI_EF_ENUM_ACCESS;
    if( ds3g_mmgsdi_get_app_type_from_session_id(session_id) == 
                                     MMGSDI_APP_SIM)
    {
      file_access.file.file_enum = MMGSDI_TELECOM_EXT1;
    }
    else
    {
      file_access.file.file_enum = MMGSDI_USIM_EXT5;
    }

    client_ref = DS_UCSD_READ_EXT;

    if( (mmgsdi_status = ds3g_mmgsdi_session_read_record(
                              session_id,
                              file_access,
                              ext_index,
                              MMGSDI_STANDARD_READ_LEN,
                              ds_ucsd_mmgsdi_response_cb,
                              (mmgsdi_client_data_type)client_ref)) != MMGSDI_SUCCESS )
    {
      DATA_MSG1_ERROR( "mmgsdi_read_record failes Status = %d", mmgsdi_status);
    }
  }
  else
  {
    if( ds_ucsd_sim_addr_info[array_index].sim_subaddress[0] != 0 )  /* ie if len > 0 */
    {
      ds_ucsd_sim_addr_info[array_index].valid = TRUE;
      DATA_MSG1_HIGH( "Sub Addr Data Valid for index [%d]", array_index);
    }
    else
    {
      ds_ucsd_sim_addr_info[array_index].valid = FALSE;
      DATA_MSG1_MED( "No calling party subaddress from MMGSDI for index[%d]",
                array_index);
    }
  }
} /* ds_ucsd_store_sub_addr */

/*===========================================================================
FUNCTION DS_UCSD_READ_CALLING_PARTY_SUBADDRESS

DESCRIPTION
  This function starts the calling party subaddress reading process.

PARAMETERS
  session_id : Session ID.
  slot       : Slot number.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds_ucsd_read_calling_party_subaddress
(
  mmgsdi_session_id_type    session_id
)
{
  mmgsdi_access_type             file_name;
  ds_ucsd_client_ref_e_type      client_ref;
  mmgsdi_return_enum_type        mmgsdi_status   = MMGSDI_MAX_RETURN_ENUM;
  uint8                          array_index = 0;
  mmgsdi_session_type_enum_type  session_type;

  session_type = ds3g_mmgsdi_get_session_type_from_session_id(session_id);

  if ( (session_type == MMGSDI_GW_PROV_PRI_SESSION) ||
       (session_type == MMGSDI_GW_PROV_SEC_SESSION)
#ifdef FEATURE_TRIPLE_SIM
       || (session_type == MMGSDI_GW_PROV_TER_SESSION)
#endif /* FEATURE_TRIPLE_SIM */
     )
  {
    ASSIGN_ARRAY_INDEX(session_type, array_index);
    ds_ucsd_sim_addr_info[array_index].session_type = session_type;

    file_name.access_method = MMGSDI_EF_ENUM_ACCESS;
    
    if( ds3g_mmgsdi_get_app_type_from_session_id(session_id) == 
             MMGSDI_APP_SIM )
    {
      file_name.file.file_enum = MMGSDI_TELECOM_MSISDN;
    }
    else
    {
      file_name.file.file_enum = MMGSDI_USIM_MSISDN;
    }

    client_ref = DS_UCSD_READ_FILE_ATTR;
    
    if ( (mmgsdi_status = ds3g_mmgsdi_session_get_file_attr(
                                session_id, 
                                file_name, 
                                ds_ucsd_mmgsdi_response_cb,
                                (mmgsdi_client_data_type)client_ref)) != MMGSDI_SUCCESS )
    {
      DATA_MSG2_ERROR("mmgsdi_session_get_file_attr fail."
                " status = %d, session_id = %d", 
                mmgsdi_status, session_id);
    }
  }
  else
  {
    DATA_MSG0_ERROR( "Invalid session id.");
  }
} /* ds_ucsd_read_calling_party_subaddress */

/*===========================================================================
  FUNCTION DS_UCSD_MMGSDI_RESPONSE_CB()

  DESCRIPTION
    Callback function called by mmgsdi 

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
static void ds_ucsd_mmgsdi_response_cb
(
  mmgsdi_return_enum_type  status, 
  mmgsdi_cnf_enum_type     cnf,
  const mmgsdi_cnf_type *  cnf_ptr
)
{
  ds_cmd_type                      *cmd_buf = NULL;
  ds_ucsd_mmgsdi_session_cnf_info  *data_ptr = NULL;

  if( status != MMGSDI_SUCCESS )
  {
    DATA_MSG1_ERROR("MMGSDI ERROR. Status = %d", 
              (int)status);
    return;
  }

  ASSERT( cnf_ptr != NULL );

  cmd_buf = ds_allocate_cmd_buf(sizeof(ds_ucsd_mmgsdi_session_cnf_info));
  if( (cmd_buf == NULL) || (cmd_buf->cmd_payload_ptr == NULL) )
  {
    return;
  }

  cmd_buf->hdr.cmd_id = DS_CMD_3GPP_UCSD_MMGSDI_PROC;
  data_ptr = (ds_ucsd_mmgsdi_session_cnf_info*)cmd_buf->cmd_payload_ptr;
  data_ptr->cnf_type = cnf;
  data_ptr->cnf_data = *cnf_ptr;
  if (cnf == MMGSDI_READ_CNF)
  {
    if (cnf_ptr->read_cnf.read_data.data_ptr != NULL)
    {
      memscpy(data_ptr->read_cnf_data,
              sizeof(data_ptr->read_cnf_data),
              cnf_ptr->read_cnf.read_data.data_ptr,
              cnf_ptr->read_cnf.read_data.data_len);
      data_ptr->cnf_data.read_cnf.read_data.data_ptr = 
                   &(data_ptr->read_cnf_data[0]);
    }
  }
  ds_put_cmd(cmd_buf);
} /* ds_ucsd_mmgsdi_response_cb */

/*===========================================================================
  FUNCTION DS_UCSD_GET_FILE_ATTR_PROCESS_RESULT()

  DESCRIPTION
    This function process the get file attributes result from mmgsdi.

  PARAMETERS
    cnf_type : Confirm type from mmgsdi.
    cnf_data : confirm data from mmgsdi.

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void ds_ucsd_get_file_attr_process_result
(
  mmgsdi_cnf_enum_type  cnf_type,
  mmgsdi_cnf_type       *cnf_data
)
{
  ds_ucsd_client_ref_e_type      client_ref;
  mmgsdi_return_enum_type        mmgsdi_status   = MMGSDI_MAX_RETURN_ENUM;
  mmgsdi_access_type             file_access;
  mmgsdi_session_id_type         session_id = DS_UCSD_INVALID_MMGSDI_SESSION_ID;

  if( cnf_data->response_header.mmgsdi_status != MMGSDI_SUCCESS)
  {
    DATA_MSG1_ERROR( "Get file attr failed. status = %d", 
              cnf_data->response_header.mmgsdi_status);
    return;
  }

  session_id = cnf_data->response_header.session_id;
  if ( ds3g_mmgsdi_get_session_type_from_session_id(session_id)
       == MMGSDI_MAX_SESSION_TYPE_ENUM )
  {
    DATA_MSG0_ERROR( "session_id not matched");
    return;
  }

  client_ref = DS_UCSD_READ_MSISDN;

  memscpy( &file_access,
           sizeof(mmgsdi_access_type),
           &(cnf_data->get_file_attr_cnf.access),
           sizeof(mmgsdi_access_type) );
  session_id = cnf_data->get_file_attr_cnf.response_header.session_id;

  if( (mmgsdi_status = ds3g_mmgsdi_session_read_record(
                            session_id,
                            file_access,
                            1,
                            MMGSDI_STANDARD_READ_LEN,
                            ds_ucsd_mmgsdi_response_cb,
                            (mmgsdi_client_data_type)client_ref)) != MMGSDI_SUCCESS )
  {
    DATA_MSG1_ERROR( "mmgsdi_read_record failes"
              "status = %d", mmgsdi_status);
  }
} /* ds_ucsd_get_file_attr_process_result */

/*===========================================================================
  FUNCTION DS_UCSD_MMGSDI_READ_PROCESS_RESULT()

  DESCRIPTION
    This function process the result got from mmgsdi as part of 
    read record request.

  PARAMETERS
    cnf_type : confirm type from mmgsdi.
    cnf_data : confirm data from mmgsdi.

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void ds_ucsd_mmgsdi_read_process_result
(
  mmgsdi_cnf_enum_type  cnf_type,
  mmgsdi_cnf_type       *cnf_data
)
{
  mmgsdi_data_type               *read_data;
  ds_ucsd_client_ref_e_type      client_ref;

  if( cnf_data->response_header.mmgsdi_status != MMGSDI_SUCCESS)
  {
    DATA_MSG1_ERROR( "SIM read failed. status = %d", 
              cnf_data->response_header.mmgsdi_status);
    return;
  }
  client_ref = (ds_ucsd_client_ref_e_type)cnf_data->read_cnf.response_header.client_data;
  read_data = &(cnf_data->read_cnf.read_data);

  switch( client_ref )
  {
    case DS_UCSD_READ_MSISDN:
      ds_ucsd_store_bcd_number(cnf_data);
      break;

    case DS_UCSD_READ_EXT:
      if ( ((*(read_data->data_ptr + DS_UCSD_EXT1_TYPE_POS)) & 
            DS_UCSD_EXT1_SUBADDRESS_MASK) != 0 )
      {
        ds_ucsd_store_sub_addr(cnf_data);
      }
      else if ( ( *(read_data->data_ptr + DS_UCSD_EXT1_TYPE_POS) & 
                DS_UCSD_EXT1_OVERFLOW_MASK) != 0 )
      {
        ds_ucsd_store_bcd_number(cnf_data);
      }
      else
      {
        DATA_MSG1_HIGH( "Neither SUBADDR/EXT BCD. Char = %d", 
                 (*(read_data->data_ptr + DS_UCSD_EXT1_TYPE_POS)));
      }
      break;

    default:
      DATA_MSG1_MED( "Invalid client ref. Ignoring read cnf."
              "client ref = %d", client_ref);
      break;
  }
} /* ds_ucsd_mmgsdi_read_process_result */

/*===========================================================================
  FUNCTION DS_UCSD_PROCESS_MMGSDI_CMD()

  DESCRIPTION
    This function is caleld when DS_UCSD_MMGSDI_CMD is received.

  PARAMETERS
    cmd_ptr : command info

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void ds_ucsd_process_mmgsdi_cmd
(
  ds_cmd_type  *cmd_ptr
)
{
  ds_ucsd_mmgsdi_session_cnf_info  *data_ptr = NULL;

  ASSERT (cmd_ptr != NULL);
  ASSERT (cmd_ptr->cmd_payload_ptr != NULL);

  data_ptr = (ds_ucsd_mmgsdi_session_cnf_info*)cmd_ptr->cmd_payload_ptr;

  DATA_MSG1_MED( "ds_ucsd_process_mmgsdi_cmd"
           "cnf_type = %d", (int)data_ptr->cnf_type);
  
  switch( data_ptr->cnf_type )
  {
    case MMGSDI_GET_FILE_ATTR_CNF:
      ds_ucsd_get_file_attr_process_result(
                          data_ptr->cnf_type,
                          &data_ptr->cnf_data );
      break;

    case MMGSDI_READ_CNF:
      ds_ucsd_mmgsdi_read_process_result(
                          data_ptr->cnf_type,
                          &data_ptr->cnf_data );
      break;

    default:
      break;
  }
} /* ds_ucsd_process_mmgsdi_cmd */

/*===========================================================================
FUNCTION DS_UCSD_GET_SUB_ADDR_INFO()

DESCRIPTION
  This function returns the calling party subaddress info.

PARAMETERS
  sub_id          : subscription info.
  sub_address info: sub address info pointer to store the result

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None

===========================================================================*/
#ifdef FEATURE_DUAL_SIM
void ds_ucsd_get_sub_addr_info
(
  sys_modem_as_id_e_type     sub_id,
  dsucsd_sim_addr_info_type  *mmgsdi_sim_addr_info
)
#else
void ds_ucsd_get_sub_addr_info
(
  dsucsd_sim_addr_info_type  *mmgsdi_sim_addr_info
)
#endif /* FEATURE_DUAL_SIM */
{
  uint8                  array_index = 0;
#ifdef FEATURE_DUAL_SIM
  sys_modem_as_id_e_type mmgsdi_session_sub_id;
#endif /* FEATURE_DUAL_SIM */

  if(mmgsdi_sim_addr_info == NULL)
  {
    return;
  }

  for( array_index = 0; 
       array_index < DS_UCSD_MAX_SUB_ADDR_ARRAY_INDEX; 
       array_index++ )
  {
#ifdef FEATURE_DUAL_SIM
    mmgsdi_session_sub_id = ds3g_mmgsdi_get_sub_id_from_session_type(
                                 ds_ucsd_sim_addr_info[array_index].session_type);
    if( mmgsdi_session_sub_id == sub_id )
    {
#endif /* FEATURE_DUAL_SIM */
      memscpy(mmgsdi_sim_addr_info,
              sizeof(dsucsd_sim_addr_info_type),
              &ds_ucsd_sim_addr_info[array_index],
              sizeof(dsucsd_sim_addr_info_type));
#ifdef FEATURE_DUAL_SIM
    }
#endif /* FEATURE_DUAL_SIM */
  }
} /* ds_ucsd_get_sub_addr_info */

/*===========================================================================
FUNCTION DS_UCSD_GET_CALLING_PARTY_SUBADDRESS()

DESCRIPTION
  This function returns the calling party subaddress.

PARAMETERS
  sub_id     : subscription info.
  sub_address: sub address pointer to store the result

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None

===========================================================================*/
#ifdef FEATURE_DUAL_SIM
boolean ds_ucsd_get_calling_party_subaddress
(
  sys_modem_as_id_e_type              sub_id,
  cm_calling_party_subaddress_s_type  *sub_address
)
#else
boolean ds_ucsd_get_calling_party_subaddress
(
  cm_calling_party_subaddress_s_type  *sub_address
)
#endif /* FEATURE_DUAL_SIM */
{
  boolean                ret_val = FALSE;
  uint8                  array_index = 0;
#ifdef FEATURE_DUAL_SIM
  sys_modem_as_id_e_type mmgsdi_session_sub_id;
#endif /* FEATURE_DUAL_SIM */

  if( sub_address == NULL )
  {
    DATA_MSG0_ERROR( "Null pointer passed.");
    return FALSE;
  }

  for( array_index = 0; 
       array_index < DS_UCSD_MAX_SUB_ADDR_ARRAY_INDEX; 
       array_index++ )
  {
    if( ds_ucsd_sim_addr_info[array_index].valid == TRUE )
    {
#ifdef FEATURE_DUAL_SIM
      mmgsdi_session_sub_id = ds3g_mmgsdi_get_sub_id_from_session_type(
                                ds_ucsd_sim_addr_info[array_index].session_type);
      if( mmgsdi_session_sub_id == sub_id )
#endif /* FEATURE_DUAL_SIM */
      {
        sub_address->present = TRUE;
        sub_address->length  = ds_ucsd_sim_addr_info[array_index].sub_addr_len;
        memscpy( sub_address->data,
                 sizeof(sub_address->data),
                 ds_ucsd_sim_addr_info[array_index].sim_subaddress,
                 ds_ucsd_sim_addr_info[array_index].sub_addr_len );
        ret_val = TRUE;
      }
    }
  }

  if( ret_val == FALSE )
  {
    sub_address->present = FALSE;
  }

  return ret_val;
} /* ds_ucsd_get_calling_party_subaddress */

/*===========================================================================
FUNCTION DS_UCSD_INVALIDATE_SUB_ADDR_INFO()

DESCRIPTION
  This function invalidates the mmgsdi info, because some card error occured.

PARAMETERS
  session_type : mmgsdi session type.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds_ucsd_invalidate_sub_addr_info
(
  mmgsdi_session_type_enum_type session_type
)
{
  uint8 array_index;

  for( array_index = 0;
       array_index < DS_UCSD_MAX_SUB_ADDR_ARRAY_INDEX; 
       array_index++)
  {
    if(ds_ucsd_sim_addr_info[array_index].session_type == session_type)
    {
      DATA_MSG1_HIGH( "CARD ERROR. Session_type = %d",
               session_type);
      ds_ucsd_sim_addr_info[array_index].valid = FALSE;
      break;
    }
  }
}/* ds_ucsd_invalidate_sub_addr_info */

#endif /* FEATURE_MMGSDI_SESSION_LIB */
#endif /* defined(FEATURE_GSM) && defined(FEATURE_DATA_GCSD) */
