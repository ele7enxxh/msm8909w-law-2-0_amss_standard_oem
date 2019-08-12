#ifndef DSATI_H
#define DSATI_H

/*===========================================================================

                        D A T A   S E R V I C E S

                A T   C O M M A N D   P R O C E S S O R

                I N T E R N A L   H E A D E R   F I L E


DESCRIPTION
  This file contains the definitions of data structures, defined and
  enumerated constants and function prototypes required for the 
  data services AT command processor.

  Copyright (c) 2001 - 2016 by Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/dsati.h_v   1.12   11 Nov 2002 14:02:32   sbandaru  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/atcop/inc/dsati.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/09/16   skc     Fixed PLMN display issue.
12/10/15   skc     Removing TUPLE_TYPE_IPv4_254
05/04/15   sc      Fixed +CHLD command issue to use requesting client id
                   instead of call client id. 
02/19/15   tk      Fixed issue in subscription not available event handling.
11/24/14   pg      Fixed ws46 featurization issue.
10/24/14   tk      ATCoP changes for dynamic memory optimizations.
09/11/14   tk      Fixed compilation errors in LTE compile out flavor.
08/27/14   tk      Fixed issue in RING URC reporting for MT CSD call.
08/12/14   pg      Modified DS Profile APIs to per subs.
07/29/14   sc      Added support for NV Refresh.
07/08/14   sc      Fixed SGLTE+G issue on SUB2.
06/27/14   tk      Added support for Dynamic ATCoP.
02/15/14   pg      Added support for SGLTE+G.
04/23/14   sc      Fixed +CFUN issue.
04/14/14   tk      Replaced __FILE__ with __FILENAME__ in DSM API wrappers.
03/27/14   pg      Fixed NOCARRIER response for command issue.
03/14/14   tk/sc   Optimized debug macros usage in ATCoP.
01/23/14   sc      Added support for $QCPDPCFGEXT command.
12/30/13   sc      Fixed static code bugs.
12/25/13   tk      Enhanced $QCSIMAPP command for better user experience.
11/13/13   sc      Added support for $QCCSGCOPS command.
10/23/13   sc      Adding support for $QCACQDBC command to clear acq_DB
10/15/13   sc      Fixed compilation errors when FEATURE_WCDMA is removed.
10/11/13   sc      Added support for $QCNSP, $QCSIMT, $QCRCIND commands and
                   REMOTE CALL END, REMOTE RING, REMOTE ANSWER
                   unsolicited result codes.
09/25/13   sc      Added support for +CNMPSD command.
09/13/13   tk      ATCoP changes for LTE and DSDS concurrency solution.
08/06/13   pg      Fixed ASSERT issue.
08/01/13   sc      Fixed KW errors.
07/16/13   pg      Fixed no ^MODE URC when UE goes out of service and comes back
07/02/13   pg      Migrated to MMGSDI API to find PLMN info in SE13 table
06/05/13   tk      ATCoP changes for SGLTE support on Dime Plus.
06/01/13   tk      Fixed call variables dynamic memory allocation issue.
05/16/13   sk      $QCCOPS command for Extension of network search.
04/19/13   tk      Fixed issue with both DSDS and thin UI enabled.
04/08/13   tk      Refactored the code in dsatutil_convert_tuple().
03/28/13   tk      Mainlined FEATURE_THIN_UI using new NV-EFS item.
03/27/13   tk      Fixed issues in dual stack commands.
02/22/13   tk      ATCoP - CM interface changes for Triton DSDA.
01/08/12   sk      Encoding type MMGSDI_EONS_ENC_UCS2_PACKED added.
01/23/13   sk      DS command buffer memory reduction.
01/04/13   tk      ATCoP changes for DSDX C/G+G with True World Mode Support.
11/30/12   tk      ATCoP changes for Triton DSDA.
09/24/12   sk      Added $QCRSRP and $QCRSRQ command Support.
09/24/12   tk      Migrated to CM APIs for MRU table access.
08/22/12   sk      Mainlining Nikel Changes.
08/06/12   tk      Added support for $QCDRX command.
07/16/12   nc      New interface changes for Manual PLMN selection.
07/16/12   tk      Added RAT input to lookup SE13 table.
05/18/12   tk      Migrated to MSG 2.0 macros
05/15/12   sk      Fixed IS_ETSI_MODE for TDSCDMA.
04/20/12   sk      DSAT_NIKEL featurization changes.
04/11/12   sk      Adding support for forwarding CFUN.
03/07/12   sk      Made CLCC common for all modes.
02/17/11   ua      Added support for $QCRMCALL.
02/06/12   ad      Added CSTF/CEN/CGPIAF/CVMOD command support.
02/06/12   nc      Added support for ^SPN and URC Queue.
01/19/12   sk      Feature cleanup.
10/20/11   nc      Added support for CUAD/CEAP/CERP commands.
10/17/11   ad      Added +CECALL support.
09/27/11   mk      Added support for $QCCLAC command.
09/14/11   ua      Extending uiutils network type to LTE. 
07/05/11   nc      Added support for ^MODE,^CARDMODE,^SYSCONFIG,^SYSINFO,^DSCI.
06/07/11   nc      Added support for $QCAPNE command.
05/13/11   ad      Added $QCPDPIMSCFGE support.
04/20/11   bs      NIKEL Phase I support.
03/16/11   bs      Restructured command tables to be mode independant.
03/03/11   nc      Fixed CLCC for Fusion.
01/11/11   ad      Removed pending variable .
01/11/11   ad      Remove extern usage.used get/set API for command associated  
                   value pointers.
10/25/10   ad      Init Changes for DSDS. 
07/23/10   nc      Added 1x SMS Support for FUSION Architecture.
06/04/10   bs      MMGSDI SESSION Migration.
05/31/10   ad      Added support for S7 command.
05/10/10   kk      Mainlining pending GOBI changes.
03/04/10   kk      Featurization fix for CDMA PIN commands.
01/12/10   sa      Added support for +CPIN,^CPIN and +CLCK for RUIM cards.
                   Made send_response_to_te a common file. 
11/16/09   ca      Added support for MMGSDI Auth commands and
                   added the utility function dsatutil_reversen_byte().
12/15/09   nc      Featurisation changes for LTE.
11/30/09   bs      Added +COPS support for LTE.
08/04/09   nc      Added support for *CNTI.
04/20/09   bs/sa   Added support for Modem Bridge Architecture.
05/19/09   vk      Defeaturization of PDP registry for CMI
04/29/09   ua      Fixed Lint Medium Errors.
03/04/09   sa      AU level CMI modifications.
02/02/09   bs      Added support for extended IRA table.
12/29/08   nc      Added support for +VTS Command.
10/23/08   bs      Added support for 1X AT Phonebook commands.
07/16/08   nc      Added utility function declarations for
                   dsatutil_swapn_word_msb_lsb() and 
                   dsatutil_reversen_word()
07/16/08   ua      Added support for external client support.
11/24/07   ua      Fixed Lint-lows and RVCT 2.2 compiler warnings. 
10/17/07   ua      Added struct for receiving call connection.
10/09/07   ss      Correcting the behaviour for GSM+1x targets.
08/22/07   sa      Added support for +CGANS and +CGAUTO commands.
07/20/07   ss      Full UCS2 character support for PBM commands.
04/23/07   pp      Lint Medium fixes.
03/12/07   pkp     Added a new def cmd_syntax_category_enum_type used in AT parsing.
01/29/06   ua      Added support for GSM 7bit '@' character (0x00).
02/15/07   ph      lint cleanup
01/30/07   ss      Changed prototype for dsatutil_fmt_mixed_param_val
08/21/06   ar      Add fax calls types  for multi-processor build.
06/28/06   au      Correct PS data feature wrapping.
04/14/06   snb     Add special processing code SPECIAL_QCHDRET and some 
                   rearrangement of codes.
03/30/06   snb     Correct PS data feature wrappings.
01/12/06   snb     Support for PBM 8bit alphabet conversions.
12/08/05   snb     Add IPV6 support and Lint corrections.
10/02/05   snb     Changes to implement a second layer of flow control, 
                   ATCOP to USB, to avoid exhaustion of DSM large items.
04/20/05   sb     Library function to send URC. Remove list_type typedef.
04/19/05   dvp     Added a global to capture the AT cmd in execution.
04/15/05   ar      Added dsatcmdp_set_restricted_mode prototype.
04/12/05   sb      Added DSATI_SMS_MSG_END_MODE state in ATCOP preprocessor
                   state.
04/05/05   ar      Add dsat_cmd_hdlr_state_s_type definition.
03/15/05   sb      Fixes from testing the serializer changes.
03/11/05   snb     Change MAX_LINE_SIZE to support +CRSM command.
03/07/05   ar      Added dsatutil_is_date_valid prototype.
02/23/05   sb      Fixed lint errors
02/10/05   sb      Accessories related changes.
12/29/04   snb     Further changes to allow character set conversion function 
                   with 8bit DCS SMSes.
11/18/04   snb     Add support for GSM and UCS2 character sets to +CSCS command
10/18/04   ar      Add generic timer support, relocate PIN defintiions
09/03/04   snb     Add +CSIM support.
05/16/04   vr      Added AT commands for BCMCS
01/15/04   sb      Added the extern declaration of dsat_report_rate_val
12/03/03   sb      Setting the preprocessor mode to SMS message entry should
                   be common to ETSI and CDMA
12/02/03   sb      open DSATI_SMS_MSG_ENTRY_MODE to both ETSI and CDMA modes
11/28/03   sb      CDMA SMS changes
11/19/03   ar      Added dsatutil_convert_tuple() routine.  Removed 
                   obsolete dsatutil_format_ipaddr().
06/10/03   ar      Added Packet Dial String Registry, read from NVRAM.
                   Added IS_ETSI_MODE macro
05/01/03   sb      new funtion declaration (dsat_finished_ath_cmd())
04/28/03   sb      AT command flow control. Removed FEATURE_DSAT_FLOW_CONTROL
03/17/03   ar      Add VALID_TOKEN macro.
02/14/03   wx      Add ABORTING state to the abortable command to 
                   handle the aborting in async command processing.
01/17/03   wx      Added dsatutil_fill_modifier_info()
01/07/03   rsl     Removed values relating to SPECIAL_QCPMA.
11/11/02   sb      Declaration of dsatutil_srch_list() function
10/22/02   sb      Merge of latest ETSI related changes
10/15/02   ak      Added #define SPECIAL_QCCHS
09/24/02   atp     Added support for QOS. (1x Release-A feature).
09/18/02   jay     Modified #define MAX_VAL_NUM_ITEM
09/10/02   ak      Compilation fix
09/05/02   sb      ATCOP changes based on new Multi mode Data services design
06/27/02   ar      Add support for more parameters & appending DSM items 
                   based on buffer length
04/17/02   rc      Removed FEATURE wrap FEATURE_DS_SOCKETS around DNS 
                   defines.
08/28/01   sjd     Modifications to support new ATCOP VU structure.
08/10/01   sjd     Initial release to MSM5200 archives.

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#include <stringl/stringl.h>
#include "dsat_v.h"
#include "dsm.h"
#include "nv.h"
#include "target.h"
#include "rex.h"
#include "ds_profile_3gpp.h"
#include "ds3gsiolib.h"
#include "modem_mem.h"
#ifdef FEATURE_COMPRESSED_HEAP
#include "compressed_modem_mem.h"
#endif /* FEATURE_COMPRESSED_HEAP */
#include "data_msg.h"
#include "ds_profile_3gpp2.h"
#include "ds_profile.h"

#include "ds3gcfgmgr.h"


#ifdef FEATURE_ECALL_APP
#include "ecall_app.h"
#endif /* FEATURE_ECALL_APP */

#if defined(FEATURE_ETSI_PBM) || defined(FEATURE_DSAT_CDMA_PBM)
#include "pbmlib.h"
#endif /* defined(FEATURE_ETSI_PBM) || defined(FEATURE_DSAT_CDMA_PBM) */

/* this file should be included only in GSM or WCDMA mode */
#if defined(FEATURE_ETSI_SMS) || defined(FEATURE_CDMA_SMS)
#include "wms.h"
#endif /* defined(FEATURE_ETSI_SMS) || defined(FEATURE_CDMA_SMS) */

#ifdef FEATURE_DATA_PS_EAP 
#include "ps_eap.h"
#include "ps_eap_sim_aka.h"
#endif /* FEATURE_DATA_PS_EAP */

/*===========================================================================

                      PUBLIC DATA DECLARATIONS
                      
===========================================================================*/


/*===========================================================================
                      AT Command Processor constants
===========================================================================*/

/*--------------------------------------------------------------------------
                   Common Limitation Constants
--------------------------------------------------------------------------*/
#define MIN_ROOM  270           /*  A data item must have at least this   */
                                /*  much room if a new item is to be put  */
                                /*  in it. Based on longest name, longest */
                                /*  response, formatting and some padding */

#define DSM_BUFFER_THRESHOLD 510 /*  Response buffer size at which a new   */
                                 /*  DSM item is requested and appended    */
                                 /*  to buffer chain                       */ 

#define MAX_BROKEN_RANGE 0xFFFF  /*  Denotes end of broken numeric range   */

#define UI_NETWORK_NAMETYPE_SHORT 1
#define UI_NETWORK_NAMETYPE_FULL  2
#define DSAT_COPS_CMD_NAME_SIZE      20

#define DSAT_CM_REQ_ID  100      /* Request ID that is sent across in CM API 
                                    for Manual PLMN selection */

/*===========================================================================
               N E T W O R K    U T I L I T I E S    T Y P E S
===========================================================================*/
/*
** Define a type for the type of network.
*/
#ifndef UIUTILS_H
#define UIUTILS_H
typedef enum uinetwk_network_type_e
{
   UI_NETWK_TYPE_UNKNOWN_TYPE,
   UI_NETWK_TYPE_GSM_900,
   UI_NETWK_TYPE_DCS_1800,
   UI_NETWK_TYPE_PCS_1900,
   UI_NETWK_TYPE_GSM_SAT,
   UI_NETWK_TYPE_UMTS,
   UI_NETWK_TYPE_LTE
} uinetwk_network_type_e_type;

/*
** Define a type that contains the networks, country code, network code
** and name.
*/
typedef struct uinetwk_network_info_s
{

   sys_mcc_type                  mcc;
     /* Mobile Network Code                                */

   sys_mnc_type                  mnc;
    /* Mobile Country Code                                 */

   uinetwk_network_type_e_type   network_type;

   char                         *short_name_ptr;
     /* Pointer to a null terminated string containing the */
     /* network's short name.                              */

   char                         *full_name_ptr;
     /* Pointer to a null terminated string containing the */
     /* network's full name.                               */

} uinetwk_network_info_s_type;

#endif /* UIUTILS_H */

/*===========================================================================
                      Special Processing Codes 
                        
    The following definitions represent the special processing codes that 
    must be assigned to each AT command that requires some special processing 
    in addition to, or instead of, the standard processing performed by the 
    AT command processing engine. 
    SPECIAL_NONE if no special processing is required. 
===========================================================================*/
#define SPECIAL_NONE              0x00
#define SPECIAL_FCLASS            0x01
#define SPECIAL_FCC               0x02
#define SPECIAL_CAD               0x03
#define SPECIAL_CBC               0x04
#define SPECIAL_CBIP              0x05
#define SPECIAL_CHV               0x06
#define SPECIAL_CMIP              0x07
#define SPECIAL_CMUX              0x08
#define SPECIAL_CSQ               0x09
#define SPECIAL_CSS               0x0A
#define SPECIAL_FBS               0x0B
#define SPECIAL_FDR               0x0C
#define SPECIAL_FDT               0x0D
#define SPECIAL_FHS               0x0E
#define SPECIAL_FKS               0x0F
#define SPECIAL_FMI               0x10
#define SPECIAL_FMM               0x11
#define SPECIAL_FMR               0x12
#define SPECIAL_FNS               0x13
#define SPECIAL_GCAP              0x14
#define SPECIAL_GMI               0x15
#define SPECIAL_GMM               0x16
#define SPECIAL_GMR               0x17
#define SPECIAL_GOI               0x18
#define SPECIAL_GSN               0x19
#define SPECIAL_IPR               0x1A
#define SPECIAL_QCDMG             0x1B
#define SPECIAL_QCDMR             0x1C
#define SPECIAL_AMP_F             0x1E
#define SPECIAL_AMP_V             0x1F
#define SPECIAL_Z                 0x20
#define SPECIAL_QCOTC             0x21
#define SPECIAL_QCRLPD            0x22  /*  Used only if DS_PSTATS defined */
#define SPECIAL_QCRLPR            0x23  /*  Used only if DS_PSTATS defined */
#define SPECIAL_QCPPPD            0x24  /*  Used only if DS_PSTATS defined */
#define SPECIAL_QCPPPR            0x25  /*  Used only if DS_PSTATS defined */
#define SPECIAL_QCIPD             0x26  /*  Used only if DS_PSTATS defined */
#define SPECIAL_QCIPR             0x27  /*  Used only if DS_PSTATS defined */
#define SPECIAL_QCCLR             0x28  /*  Used only if DS_PSTATS defined */

#define SPECIAL_QCUDPD            0x29  /*  Used only if DS_PSTATS defined */
#define SPECIAL_QCUDPR            0x2A  /*  Used only if DS_PSTATS defined */

#define SPECIAL_QCTCPD            0x2B  /*  Used only if DS_PSTATS defined */
#define SPECIAL_QCTCPR            0x2C  /*  Used only if DS_PSTATS defined */

#ifdef  FEATURE_DS_MTOM
#define SPECIAL_QCMTOM            0x2D  /*  Used only if FEATURE_DS_MTOM   */ 
#endif  /*  FEATURE_DS_MTOM */

#ifdef FEATURE_DS_QNC
#define SPECIAL_QCQNC             0x2E  /*  Used only if FEATURE_DS_QNC    */ 
#endif

#ifdef FEATURE_DS_CHOOSE_SO
#define SPECIAL_QCSO              0x2F  /*  Only if FEATURE_DS_CHOOSE_SO   */
#endif

#define SPECIAL_QCVAD             0x30

#ifdef FEATURE_QMIP 
#define SPECIAL_QCQMIP            0x31
#endif  /*  FEATURE_QMIP  */

#if defined(FEATURE_DS_DOTG_DATA)
#define SPECIAL_QCFAMP            0x32
#endif /* FEATURE_DS_DOTG_DATA */

#define SPECIAL_QCCAV             0x33  /*  Answer Voice call              */
#define SPECIAL_QCPKND            0x34  /*  Orig Packet without dialling   */

#if defined(FEATURE_IS95B_MDR) || defined(FEATURE_DS_IS2000)
#define SPECIAL_QCMDR             0x35
#endif /* FEATURE_IS95B_MDR || FEATURE_DS_IS2000 */

#define SPECIAL_QCRLPD33          0x36  /*used with DS_PSTATS && DS_IS2000 */
#define SPECIAL_QCRLPR33          0x37  /*used with DS_PSTATS && DS_IS2000 */

#define SPECIAL_QCDNSPRI          0x38
#define SPECIAL_QCDNSSEC          0x39


#if defined(FEATURE_DS_IS2000) && defined(FEATURE_IS2000_R_SCH)
#define SPECIAL_QCSCRM            0x3B
#endif /* FEATURE_DS_IS2000  && FEATURE_IS2000_R_SCH */

#if defined(FEATURE_DS_IS2000) && defined(FEATURE_IS2000_R_SCH)
#define SPECIAL_QCTRTL            0x3C
#endif /* FEATURE_DS_IS2000  && FEATURE_IS2000_R_SCH */

#ifdef FEATURE_HDR
#define SPECIAL_QCHDRC            0x40
#define SPECIAL_QCHDRR            0x41
#define SPECIAL_QCHDRT            0x42
#define SPECIAL_QCHDRET           0x43
#endif /* FEATURE_HDR */

#ifdef FEATURE_DS_MOBILE_IP
#define SPECIAL_QCMIPMASS         0x56
#define SPECIAL_QCMIPMHSS         0x57
#define SPECIAL_QCMIPPHA          0x58
#define SPECIAL_QCMIPSHA          0x59
#define SPECIAL_QCMIPMASPI        0x5A
#define SPECIAL_QCMIPMHSPI        0x5B
#define SPECIAL_QCMIPHA           0x5C
#define SPECIAL_QCMIPMASSX        0x5F
#define SPECIAL_QCMIPMHSSX        0x60
#endif  /*  FEATURE_DS_MOBILE_IP */

#ifdef FEATURE_IS2000_REL_A
#define SPECIAL_QCQOSPRI          0x70
#endif /* FEATURE_IS2000_REL_A */

#ifdef FEATURE_IS2000_CHS
#define SPECIAL_QCCHS             0x78
#endif /* FEATURE_IS2000_CHS */

#define SPECIAL_QCPREV            0x80
#define SPECIAL_QCCHV             0x81

/*---------------------------------------------------------------------------
  SIO data preprocessor mode enumeration type.

  DO NOT FEATURIZE THESE.
---------------------------------------------------------------------------*/
#define SPECIAL_ATD               0x84
#define SPECIAL_SMS               0x85
/*===========================================================================
               Bit-Mapped AT Command Attributes Constants 
===========================================================================*/
#define ATTRIB_NONE   0x00000000    /*  No attributes                      */

#define UMTS_CMD      0x08000000
#define CDMA_CMD      0x04000000
#define DO_PREF_CMD   0x02000000
#define COMMON_CMD    0x01000000

#define ABORT_CMD     0x00200000    /*  If the command is abortable */
#define NO_RESET      0x00100000    /*  ATZ or AT&F leave 
                                        the command value alone            */
#define BROKEN_RANGE  0x00080000    /*  Discontinuous numeric range        */
#define MULTI_INDEX   0x00040000    /*  Multi-dimensioned parameter set    */
#define NO_DISPLAY    0x00020000    /*  Value not displayed (eg  password) */
#define RESTRICTED    0x00010000    /*  Allowed in PIN restricted mode     */
#define CONFIG        0x00008000    /*  Variable: takes argument           */
#define FCLASS2P0     0x00004000    /*  Class 2.0 Fax item                 */
#define LOCAL_TEST    0x00002000    /*  Execute =? command locally         */
#define ALWAYS        0x00001000    /*  always transmit in config          */
#define SIMPLE_DEC    0x00000800    /*  3 digit, leading 0 decimal         */
#define STRING        0x00000400    /*  Value is string                    */
#define HEX           0x00000200    /*  Numeric value is Hex               */
#define NO_QUOTE      0x00000200    /*  String value has no '"'s           */
#define EXTENDED      0x00000100    /*  Extended syntax                    */
#define READ_ONLY     0x00000080    /*  Read only; may have '?' in syntax  */
#define DOWN_LOAD     0x00000040    /*  Transmit in config if not default  */
#define LIST          0x00000020    /*  def_lim_ptr points to a list       */
#define NOT_PROVISIONED 0x00000010  /*  Allow this command without provisioning      */
#define MULTI_SUBS    0x00000008    /*  Subscription/Session based command attribute */
#define MULTI_SLOT    0x00000004    /*  Slot based command attribute */
#define MIXED_PARAM   0x00000002    /*  Mixed parameter types              */
#define YES_QUOTE     0x00000001    /*  The string argument must be 
                                    quoted                            */


#define MAX_VAL_NUM_ITEM 0xFFFFFFFF /*  Max value of a numeric AT parm     */
#define MAX_BYTE_VALUE   0xFF
#define MAX_ITOA_LEN 33 /* Maximum length of ASCII string converted from integer */

/*---------------------------------------------------------------------------
    The following defines MUST match the values in                     
    "dsat_result_enum_type" in dsat.h".        
    MIN_CELL_RES must be LESS than MIN_CELL_ERR       
---------------------------------------------------------------------------*/
#define MIN_CELL_RES  10  /*  First cellular result code. */
#define MIN_CELL_ERR  20  /*  First cellular result code with +CERROR: prefix */


/*===========================================================================
  bit masks for dsati_dial_str_type's attrib field
===========================================================================*/
/* Dial string ends with ;                                      */
#define         DIAL_ATTRIB_SEMICOLON           0x0001

/* Dial string starts with > in etsi mode                       */
#define         DIAL_ATTRIB_GREAT               (0x0001 << 1)

/* Dial string ends with G at the end or 2nd end before ;       */
#define         DIAL_ATTRIB_UP_G                (0x0001 << 2)

/* Dial string ends with g at the end or 2nd end before ;       */
#define         DIAL_ATTRIB_DN_G                (0x0001 << 3)

/* Dial string ends with I at the end or 2nd end before ; 
   or 3rd end before ;[G|g]                                     */
#define         DIAL_ATTRIB_UP_I                (0x0001 << 4)

/* Dial string ends with i at the end or 2nd end before ; 
   or 3rd end before ;[G|g]                                     */
#define         DIAL_ATTRIB_DN_I                (0x0001 << 5)

/* Dial string has the pattern: *[98|99]...# in etsi mode or
   #777\0 in cdma mode.                                         */
#define         DIAL_ATTRIB_PKT                 (0x0001 << 6)

/* Dial string contains char other than digit * # +, 
   Specificly:  ABCD in etsi mode
                ABCDTP,W@!$; in cdma mode                       */
#define         DIAL_ATTRIB_ASCII               (0x0001 << 7)

/* Dial string'first char is '+'                                */
#define         DIAL_ATTRIB_START_PLUS          (0x0001 << 8)

/*===========================================================================
  Utility Functions
===========================================================================*/

/*===========================================================================

FUNCTION DSAT_IS_ETSI_MODE

DESCRIPTION
   Check if a particular mode passed is 3GPP based opertating mode or not
 
DEPENDENCIES

RETURN VALUE
  boolean: TRUE if valid, FALSE otherwise

SIDE EFFECTS
  None
  
===========================================================================*/
boolean dsat_is_etsi_mode
(
  dsati_mode_e_type mode
);

/*===========================================================================

FUNCTION DSAT_IS_PROFILE_ID_IN_RANGE

DESCRIPTION
   Check if a particular profile_id being passed is in valid range or not
 
DEPENDENCIES

RETURN VALUE
  boolean: TRUE if valid, FALSE otherwise

SIDE EFFECTS
  None
  
===========================================================================*/
boolean dsat_is_profile_id_in_range
(
  ds_profile_num_type profile_id
);

/*===========================================================================

FUNCTION DSAT_IS_VALID_TOKEN

DESCRIPTION
   Check if a particular token of the current token_ptr is 
   in valid

DEPENDENCIES

RETURN VALUE
  boolean: TRUE if valid, FALSE otherwise

SIDE EFFECTS
  None
  
===========================================================================*/
boolean dsat_is_valid_token
(
  const tokens_struct_type *tok_ptr,
  uint8                     token_index
);

/*===========================================================================

FUNCTION DSAT_PROFILE_SET_PARAM

DESCRIPTION
   Function to set the paramter in a profile created using ds_profile.
 
DEPENDENCIES

RETURN VALUE
  ds_profile_status_etype: DS_PROFILE_REG_RESULT_SUCCESS is set was
                           successful, FALSE otherwise

SIDE EFFECTS
  None
  
===========================================================================*/
ds_profile_status_etype dsat_profile_set_param
(
  ds_profile_hndl_type          profile_hndl,
  ds_profile_identifier_type    param_type,
  const ds_profile_info_type    param_info
);

/*===========================================================================

FUNCTION DSAT_PROFILE_GET_PARAM

DESCRIPTION
   Function to get the paramter in a profile created using ds_profile.
 
DEPENDENCIES

RETURN VALUE
  ds_profile_status_etype: DS_PROFILE_REG_RESULT_SUCCESS is set was
                           successful, FALSE otherwise

SIDE EFFECTS
  None
  
===========================================================================*/
ds_profile_status_etype dsat_profile_get_param
(
  ds_profile_hndl_type          profile_hndl,
  ds_profile_identifier_type    param_type,
  ds_profile_info_type          param_info
);

/*===========================================================================

FUNCTION DSAT_PROFILE_RESET_PARAM_TO_DEFAULT

DESCRIPTION
   Function to reset the paramter to default in a profile
   created using ds_profile.
 
DEPENDENCIES

RETURN VALUE
  ds_profile_status_etype: DS_PROFILE_REG_RESULT_SUCCESS is set was
                           successful, FALSE otherwise

SIDE EFFECTS
  None
  
===========================================================================*/
ds_profile_status_etype dsat_profile_reset_param_to_default
(
  ds_profile_tech_etype         tech,
  ds_profile_num_type           num,
  ds_profile_identifier_type    ident
);

/*===========================================================================

FUNCTION DSAT_PROFILE_RESET_PARAM_TO_INVALID

DESCRIPTION
   Function to reset the paramter to invalid in a profile
   created using ds_profile.
 
DEPENDENCIES

RETURN VALUE
  ds_profile_status_etype: DS_PROFILE_REG_RESULT_SUCCESS is set was
                           successful, FALSE otherwise

SIDE EFFECTS
  None
  
===========================================================================*/
ds_profile_status_etype dsat_profile_reset_param_to_invalid
(
  ds_profile_tech_etype         tech,
  ds_profile_num_type           num,
  ds_profile_identifier_type    ident
);

/*===========================================================================

FUNCTION DSAT_PROFILE_BEGIN_TRANSACTION

DESCRIPTION
   Function to begin a ds_profile transaction to perform action, such as -
   SET_PARAM, GET_PARAM, etc...
 
DEPENDENCIES

RETURN VALUE
  ds_profile_status_etype: DS_PROFILE_REG_RESULT_SUCCESS is set was
                           successful, FALSE otherwise

SIDE EFFECTS
  None
  
===========================================================================*/
ds_profile_status_etype dsat_profile_begin_transaction
(
  ds_profile_tech_etype         tech,
  ds_profile_num_type           num,
  ds_profile_hndl_type          *profile_hndl
);

/*===========================================================================

FUNCTION DSAT_PROFILE_BEGIN_TRANSACTION

DESCRIPTION
   Function to end a ds_profile transaction to perform action. This will
   release the profile_hndl created in ds_profile using atcop.
 
DEPENDENCIES

RETURN VALUE
  ds_profile_status_etype: DS_PROFILE_REG_RESULT_SUCCESS is set was
                           successful, FALSE otherwise

SIDE EFFECTS
  None
  
===========================================================================*/
ds_profile_status_etype dsat_profile_end_transaction
(
  ds_profile_hndl_type          profile_hndl
);

/*===========================================================================

FUNCTION DSAT_PROFILE_DESTROY_ITERATOR

DESCRIPTION
   Function to delete the iterator created. This will
   release the interator created in ds_profile using atcop for
   traversing the profiles.
 
DEPENDENCIES

RETURN VALUE
  ds_profile_status_etype: DS_PROFILE_REG_RESULT_SUCCESS is set was
                           successful, FALSE otherwise

SIDE EFFECTS
  None
  
===========================================================================*/
ds_profile_status_etype dsat_profile_destroy_iterator
(
  ds_profile_itr_type           itr
);
/*===========================================================================
  Utility macros
===========================================================================*/

#define TOKEN_PTR    tok_ptr
#define TOKEN_IDX_0  0
#define TOKEN_IDX_1  1
#define TOKEN_IDX_2  2
#define TOKEN_IDX_3  3
#define TOKEN_IDX_4  4
#define TOKEN_IDX_5  5
#define TOKEN_IDX_6  6
#define TOKEN_IDX_7  7
#define TOKEN_IDX_8  8
#define TOKEN_IDX_9  9
#define TOKEN_IDX_10 10
#define TOKEN_IDX_11 11

/* Test for null string token in parameter list */
#define VALID_TOKEN(tok) \
   dsat_is_valid_token(dsat_curr_cmd_var.tok_ptr, tok)

/* Test for null string token in parameter list. Then Convert to INT */
#define VALID_NUM_TOKEN(token_index, range_min, range_max, output_buf) \
    err_info.errval = dsat_is_valid_num_token(dsat_curr_cmd_var.tok_ptr, \
                            token_index, \
                            range_min, \
                            range_max, \
                            output_buf); \
    if(err_info.errval == DSAT_ERR_ATOI_CONVERSION_FAILURE) \
      goto send_error; \
    else if(err_info.errval == DSAT_ERR_INVALID_TOKENS) \
      valid_status = FALSE; \
    else if(err_info.errval == DSAT_ERR_NO_ERROR) \
      valid_status = TRUE;

/* Test for null string token in parameter list. Then remove quotes and convert to string */
#define VALID_STRING_TOKEN(token_index, input_buf, output_buf, str_len) \
    err_info.errval = dsat_is_valid_string_token(dsat_curr_cmd_var.tok_ptr, \
                            token_index, \
                            input_buf, \
                            output_buf, \
                            str_len); \
    if(err_info.errval == DSAT_ERR_QUOTE_STRIP_FAILURE) \
      goto send_error; \
    else if(err_info.errval == DSAT_ERR_INVALID_TOKENS) \
      valid_status = FALSE; \
    else if(err_info.errval == DSAT_ERR_NO_ERROR) \
      valid_status = TRUE;
    

/* Test for ETSI operating modes */
#define IS_ETSI_MODE(mode) \
    dsat_is_etsi_mode(mode)

#define IS_ETSI_SYS_MODE(mode) \
       ((mode == SYS_SYS_MODE_GSM) || \
        (mode == SYS_SYS_MODE_WCDMA) || \
        (mode == SYS_SYS_MODE_LTE) || \
        (mode == SYS_SYS_MODE_TDS))

/* Test for CDMA operating modes */
#define IS_CDMA_MODE(mode) \
    ((mode == DSAT_MODE_CDMA) || \
     (mode == DSAT_MODE_1XDO) || \
     (mode == DSAT_MODE_1XLTE) )

/* Test for Number */
#define IS_DIGIT( x ) ( ((x-'0') >= 0) && ((x-'0') <= 9) )

/*Test for Dual support*/
#define EXEC_VAL_PTR(parse_table) \
    ( dsatutil_get_val_from_cmd_id(parse_table->cmd_id,dsat_get_current_subs_id(FALSE))\
    )

#define dsat_dsm_new_buffer(pool_id, graceful) \
  dsatutil_dsm_new_buffer(pool_id, __FILENAME__, __LINE__, graceful)

#define dsat_dsm_pushdown_tail(pkt_head_ptr, buf, size, pool_id, graceful) \
  dsatutil_dsm_pushdown_tail(pkt_head_ptr, buf, size, pool_id, __FILENAME__, __LINE__, graceful)

#define dsat_dsm_create_packet(buf, size, graceful) \
  dsatutil_dsm_create_packet(buf, size, __FILENAME__, __LINE__, graceful)

#if defined(FEATURE_COMPRESSED_HEAP) && !defined(TEST_FRAMEWORK)
#define dsat_modem_mem_alloc modem_mem_alloc_ch
#define dsat_modem_mem_free  modem_mem_free_ch
#else
#define dsat_modem_mem_alloc modem_mem_alloc
#define dsat_modem_mem_free  modem_mem_free
#endif

#define dsat_alloc_memory(size, graceful) \
  (TRUE == graceful) ? \
  dsatutil_validate_memory_msg_error(dsat_modem_mem_alloc(size, MODEM_MEM_CLIENT_DATA), \
                                     size, __FILENAME__, __LINE__) \
  : \
  dsatutil_validate_memory_err_fatal(dsat_modem_mem_alloc(size, MODEM_MEM_CLIENT_DATA), \
                                     size, __FILENAME__, __LINE__)

#define dsat_get_cmd_buf(payload_size, graceful) \
  dsatutil_get_cmd_buf(payload_size, __FILENAME__, __LINE__, graceful)

#define dsat_get_cmd_payload_ptr(payload_size) \
  dsatutil_get_cmd_payload_ptr(payload_size)

#define dsat_alloc_cmd_mem_ptr(size) \
  dsatutil_alloc_cmd_mem_ptr(dsat_alloc_memory(size, FALSE))

#define dsat_get_cmd_mem_ptr() \
  dsatutil_get_cmd_mem_ptr()

#define dsat_free_cmd_mem_ptr() \
  dsatutil_free_cmd_mem_ptr()

#define dsat_get_base_addr(mem_type, graceful) \
  dsatutil_get_base_addr(mem_type, graceful)

#define dsat_get_base_addr_per_slot(slot_id, graceful) \
  dsatutil_get_base_addr_per_slot(slot_id, graceful)

#define dsat_get_base_addr_current_slot() \
  dsatutil_get_base_addr_current_slot()

#define dsat_get_base_addr_per_session(apps_id, graceful) \
  dsatutil_get_base_addr_per_session(apps_id, graceful)

#define dsat_get_base_addr_current_session() \
  dsatutil_get_base_addr_current_session()

#define dsat_get_base_addr_per_subs(mem_type, subs_id, graceful) \
  dsatutil_get_base_addr_per_subs(mem_type, subs_id, graceful)

#define dsat_get_base_addr_current_subs(mem_type) \
  dsatutil_get_base_addr_current_subs(mem_type)

#define DSAT_ABSOLUTE_VAL(x) ( (x < 0) ? -(x) : x )

#define DSAT_SYS_MODEM_AS_ID_MAX  SYS_MODEM_AS_ID_NO_CHANGE
#define DSAT_MMGSDI_SLOT_MAX      MMGSDI_SLOT_AUTOMATIC
#define DSAT_MAX_MMGSDI_SLOTS     (DSAT_MMGSDI_SLOT_MAX - MMGSDI_SLOT_1)
#define DSAT_QCSIMAPP_VAL_INVALID 255

#define DSAT_INDEX_0   0

#define DSAT_SLOT_INDEX(slot_id) (slot_id - MMGSDI_SLOT_1)

#define IS_VALID_SLOT_ID(slot_id) \
  ((MMGSDI_SLOT_1 <= slot_id) && (slot_id < DSAT_MMGSDI_SLOT_MAX))

#define IS_VALID_GW_APPS_ID(apps_id) \
  ((DSAT_APPS_ID_GW_PRIMARY <= apps_id) && (apps_id < DSAT_APPS_ID_GW_MAX))

#define IS_VALID_APPS_ID(apps_id) \
  ((DSAT_APPS_ID_GW_PRIMARY <= apps_id) && (apps_id < DSAT_APPS_ID_MAX))

#define IS_VALID_SUBS_ID(subs_id) \
  ((SYS_MODEM_AS_ID_1 <= subs_id) && (subs_id < DSAT_SYS_MODEM_AS_ID_MAX))

/* SIM slot indices */
typedef enum
{
  DSAT_SLOT_IDX_1 = 0,
  DSAT_SLOT_IDX_2,
  DSAT_SLOT_IDX_3,
  DSAT_SLOT_IDX_MAX
} dsat_slot_idx_e_type;

/* SIM applications */
typedef enum
{
  /* GW applications */
  DSAT_APPS_ID_GW_PRIMARY = 0,
  DSAT_APPS_ID_GW_SECONDARY,
  DSAT_APPS_ID_GW_TERTIARY,
  DSAT_APPS_ID_GW_MAX,

  /* 1X applications */
  DSAT_APPS_ID_1X_PRIMARY = DSAT_APPS_ID_GW_MAX,
  DSAT_APPS_ID_1X_MAX,

  DSAT_APPS_ID_MAX = DSAT_APPS_ID_1X_MAX
} dsat_apps_id_e_type;

/* Multi SIM classes */
typedef enum dsat_msim_class_e
{
  /* Multi SIM independent class */
  DSAT_MSIM_CLASS_INDEPENDENT = 0,

  /* Multi SIM slot based class */
  DSAT_MSIM_CLASS_SLOT,

  /* Multi SIM session based class */
  DSAT_MSIM_CLASS_SESSION,

  /* Multi SIM subscription based class */
  DSAT_MSIM_CLASS_SUBS,

  DSAT_MSIM_CLASS_MAX
} dsat_msim_class_e_type;

/* Multi SIM generic ID */
typedef struct dsat_msim_id_s
{
  dsat_msim_class_e_type class_val;
  union
  {
    mmgsdi_slot_id_enum_type slot_id;
    dsat_apps_id_e_type      apps_id;
    sys_modem_as_id_e_type   subs_id;
  } id;
} dsat_msim_id_s_type;

#define DSAT_BUF_SIZE(size,used) ((size)>(used)?((size) - (used)):(0))

/* Allow for quotes on USSD string */
#define CUSD_STR_MAX_LEN (MAX_USS_CHAR_ASCII)
#define CUSD_DCS_STR_MAX_LEN 3
#ifdef FEATURE_MMGSDI_PERSONALIZATION
  #define MAX_PIN_STRING_LEN  MMGSDI_PERSO_MAX_KEY
#else  /* FEATURE_MMGSDI_PERSONALIZATION */
  #define MAX_PIN_STRING_LEN  MMGSDI_PIN_MAX_LEN
#endif /* FEATURE_MMGSDI_PERSONALIZATION */
/* Allow for quotes on next string */
#define COPS_OPER_STR_MAX_LEN (DSAT_MAX_FULLNAME_STR_LEN+2)
#define CPOL_OPER_STR_MAX_LEN (DSAT_MAX_FULLNAME_STR_LEN+2)
#define COPS_CSG_ID_STR_MAX_LEN 8
#define MAX_PORT_STRING_LEN         11
#define MAX_TOS_TCLASS_STRING_LEN   11
#define MAX_QOSERR_STRING_LEN 4
#ifdef FEATURE_ETSI_SMS_CB 
#define CSCB_MIDS_STR_MAX_LEN 50
#define CSCB_DCSS_STR_MAX_LEN 50
#endif /* FEATURE_ETSI_SMS_CB */
#define ES_ORGI_RQST_MAX_LEN 1
#define ES_ORIG_FBK_MAX_LEN 0
#define ES_ANS_FBK_MAX_LEN 1
#define ESA_FRAMED_IDLE_MAX_LEN 0
#define ESA_FRAMED_UN_OV_MAX_LEN 0
#define ESA_HD_AUTO_MAX_LEN 0
#define ESA_SYN2_MAX_LEN 0
#define CSCA_SCA_STR_MAX_LEN 21
#define CSCA_TOSCA_STR_MAX_LEN 5

#define QCSMP_VP_STR_MAX_LEN 22
#define QCSMP_DDT_STR_MAX_LEN 22
/* UNLOCK */
#define UNLOCK_STRING_LEN NV_SEC_CODE_SIZE + NV_LOCK_CODE_SIZE + 2
/* LOCK change */
#define LOCK_OLD_STRING_LEN (NV_SEC_CODE_SIZE + NV_LOCK_CODE_SIZE + 2)
#define MAX_CCLK_STRING_LENGTH 20 /* yy/MM/dd,hh:mm:ss±zz format */
#define MAX_CIND_INDICATORS  8

#define MAX_1X_PROFILES                   1

/*===========================================================================
  +CGAUTO macros for MT PDP
===========================================================================*/
#ifdef FEATURE_DATA_TE_MT_PDP
/* turn off automatic response for Packet Domain only*/
#define DSAT_CGAUTO_AUTO_ANS_OFF                  0
/* turn on automatic response for Packet Domain only */
#define DSAT_CGAUTO_AUTO_ANS_ON                   1
/* modem compatibility mode, Packet Domain only */
#define DSAT_CGAUTO_MDM_COMPAT_PKT_DMN_ONLY       2
#endif /* FEATURE_DATA_TE_MT_PDP */

#define PLMN_STR_MCC_LEN 3
#define PLMN_STR_MNC_LEN 3

/*=====================================MUX==================================*/

typedef struct
{
  int                                  port_state;
  int                                  port_operation_state;
} ds_at_mux_notify_info_type;

typedef struct
{
  rdm_assign_status_type               status;
  rdm_service_enum_type                service;
  rdm_device_enum_type                 device;
} ds_at_rdm_notify_info_type;


/*=====================================CM===================================*/

/* DS_AT_CM_CALL_CMD data type,
   report call command execution status */
typedef struct
{
  void                                 *data_ptr;
  cm_call_cmd_e_type                    cmd;
  cm_call_cmd_err_e_type                cmd_err;
  cm_sups_cmd_err_cause_e_type          err_cause; 
  cm_alpha_s_type                       alpha;
} ds_at_cm_call_cmd_type;

typedef struct dsat_cm_gw_cs_call_info_s
{
  cm_bearer_capability_s_type         bearer_capability_1;
    /**< Bearer capability 1. */

  cm_called_party_bcd_no_s_type       called_party_bcd_number;
    /**< Called party Binary-Coded Decimal (BCD) number. */

  cm_called_party_subaddress_s_type   called_party_subaddress;
    /**< Called party subaddress. */

  cm_calling_party_bcd_no_s_type      calling_party_bcd_number;
    /**< Calling party BCD number. */

  cm_calling_party_subaddress_s_type  calling_party_subaddress;
    /**< Calling party subaddress. */

  ie_cm_cc_cause_s_type               cc_cause;
    /**< Call Control cause information. */

  ie_cm_cc_reject_s_type              cc_reject;
    /**< Call Control rejection information. */

  boolean                             call_ss_success;
    /**< Flag to indicate if multiple call management is successful. */

  active_calls_list_s_type            active_calls_list;
    /**< Active call ID list from multiple calls management confirmation. */

  cm_connected_subaddress_s_type      connected_subaddress;
    /**< Connected subaddress. */

  cm_cause_of_no_cli_s_type           cause_of_no_cli;
    /**< Cause of no Calling Line Information (CLI) in an MT call. */

} dsat_cm_gw_cs_call_info_s_type;

typedef struct dsat_cm_gw_ps_call_info_s
{
  nsapi_T                         nsapi;
    /**< NSAP information. */

  cause_enum_type_T               pdp_cause_type;
    /**< PDP cause type. */

  pdp_cause_T                     cause;
    /**< PDP cause. */

  cm_ps_sig_rel_status_T          ps_sig_rel_status;
    /**< Status of the PS signaling release request that was initiated
         by the client. */

} dsat_cm_gw_ps_call_info_s_type;

typedef struct dsat_cm_cdma_call_info_s
{
  uint16                          srv_opt;
    /**< Service option as defined in the cai.h file.\n
         @note1 The service option takes on the CM_SO_ANALOG value (defined in 
         this file) when the call is connected (or to be connected) over analog 
         service. */

} dsat_cm_cdma_call_info_s_type;

typedef struct dsat_cm_lte_call_info_s
{
  esm_cause_T                        esm_cause;
    /**< ESM cause. */

  lte_nas_local_cause_T              esm_local_cause;
    /**< ESM local cause. */

} dsat_cm_lte_call_info_s_type;

typedef union dsat_cm_call_mode_info_u
{

  dsat_cm_gw_cs_call_info_s_type     gw_cs_call;
      /**< CS GSM or WCDMA or TD-SCDMA-specific information. */

  dsat_cm_gw_ps_call_info_s_type     gw_ps_call;
      /**< PS GSM or WCDMA or TD-SCDMA-specific information. */

  dsat_cm_cdma_call_info_s_type      cdma_call;
      /**< CDMA-specific information. */

  dsat_cm_lte_call_info_s_type       lte_call;
      /**< PS LTE-specific information. */

} dsat_cm_call_mode_info_u_type;

typedef struct dsat_cm_call_mode_info_s
{
  cm_call_mode_info_e_type       info_type;
    /**< Which mode information is present. */

  dsat_cm_call_mode_info_u_type  info;
    /**< Mode information. */

} dsat_cm_call_mode_info_s_type;

typedef struct dsat_cm_call_state_info_s
{
  cm_call_id_type                call_id;
    /**< Call ID. */

  cm_call_type_e_type            call_type;
    /**< Call type: VOICE, DATA, OTASP, etc. */

  cm_call_state_e_type           call_state;
    /**< Call state: Idle, Originating, Incoming, or Conversation. */

  cm_call_substate_type          call_subst;
     /**< Call substates: Idle, Conversation, or Incoming.  */

} dsat_cm_call_state_info_s_type;

typedef struct dsat_cm_call_state_info_list_s
{
  byte                           number_of_active_calls;
    /**< The number of calls whose state is not idle. */

  dsat_cm_call_state_info_s_type info[CM_CALL_ID_MAX];
    /**< The state information for all call objects. */

} dsat_cm_call_state_info_list_s_type;

/* DS_AT_CM_CALL_INFO_CMD data type,
   convery call event info to ATCOP */
typedef struct
{
  cm_client_type_e_type                call_client_id;
  cm_call_id_type                      call_id;
  cm_call_type_e_type                  call_type;
  cm_call_state_e_type                 call_state;
  cm_call_direction_e_type             call_direction;
#ifdef FEATURE_DSAT_ETSI_MODE
  cm_call_ss_info_s_type               ss_info;  /* call related SS information */
#endif /* defined(FEATURE_DSAT_ETSI_MODE) */

  cm_call_sups_type_e_type             sups_type;
  uint16                               profile_number;
  cm_num_s_type                        num;
  dsat_cm_call_mode_info_s_type        dsat_mode_info;
  cm_call_end_e_type                   end_status;
  dsat_cm_call_state_info_list_s_type  dsat_cm_call_info_list;
  boolean                              dsat_info_list_avail;
  sys_sys_mode_e_type                  sys_mode;
  sys_modem_as_id_e_type               subs_id;
#ifdef FEATURE_DUAL_ACTIVE
  cm_call_hold_state_e_type            local_hold_state;
#endif /* FEATURE_DUAL_ACTIVE */
  boolean                              report_ccwa;
  boolean                              report_cssu;
  boolean                              is_call_id_found;
  boolean                              is_call_among_active;
  boolean                              call_ss_success;
  cm_client_id_type                    requesting_client_id;
} ds_at_call_info_s_type;

typedef struct
{
  cm_call_event_e_type                 event;
  ds_at_call_info_s_type               event_info;
} ds_at_cm_call_event_type;

#ifdef FEATURE_DSAT_ETSI_MODE
typedef struct
{
  cm_inband_cmd_e_type                 cmd;
  cm_inband_cmd_err_e_type             cmd_err;
} ds_at_cm_inband_cmd_type;
typedef struct
{
  cm_inband_event_e_type               event;
  const cm_inband_info_s_type*         event_info;
} ds_at_cm_inband_event_type;
#endif /* FEATURE_DSAT_ETSI_MODE */

/* DS_AT_CM_SS_CMD data type,
   report Serving System command execution status */
typedef struct
{
  void                                 *data_ptr;
  cm_ss_cmd_e_type                      cmd;
  cm_ss_cmd_err_e_type                  cmd_err;
} ds_at_cm_ss_cmd_type;
/* Cache of RX signal data */

/* DS_AT_CM_PH_CMD data type,           */
/* indicates in there is any error during */
/* ATTACH or DETACH of +CGATT command     */
typedef struct
{
  cm_ph_cmd_e_type                     cmd;
  cm_ph_cmd_err_e_type                 cmd_err;
  void                                *data_ptr;
} ds_at_cm_ph_cmd_type;

/* DS_AT_CM_PH_INFO_CMD data type,
   convey phone event info to ATCOP */
typedef struct
{
  cm_network_sel_mode_pref_e_type      network_sel_mode_pref;
  cm_mode_pref_e_type                  network_rat_mode_pref;
  cm_roam_pref_e_type                  network_roam_pref;
  cm_band_pref_e_type                  network_band_pref;
  cm_srv_domain_pref_e_type            service_domain_pref;
  cm_gw_acq_order_pref_e_type          acq_order_pref;
  cm_acq_pri_order_pref_s_type         acq_pri_order_pref;
} ds_at_cm_ph_pref_type;

typedef struct dsat_sys_csg_info_s
{
  sys_csg_id_type              csg_id;
  /**< CSG Identifier */

} dsat_sys_csg_info_s_type;

typedef struct dsat_sys_detailed_plmn_list_info_s
{

  sys_plmn_id_s_type                        plmn;
    /**< PLMN ID. */

  sys_radio_access_tech_e_type              rat;
    /**< Radio Access Technology of the PLMN. */

  boolean                                   plmn_forbidden;
    /**< Whether the PLMN is forbidden. */

  sys_detailed_plmn_list_category_e_type    list_category;
    /**< Type of PLMN. */

  dsat_sys_csg_info_s_type                  csg_info;
  /**< CSG information. */

} dsat_sys_detailed_plmn_list_info_s_type;

typedef struct dsat_sys_detailed_plmn_list_s
{

  uint32                                    length;
    /**< The number of PLMNs in the list. */

  dsat_sys_detailed_plmn_list_info_s_type   info[SYS_PLMN_LIST_MAX_LENGTH];
    /**< PLMN information. */

} dsat_sys_detailed_plmn_list_s_type;

typedef struct
{
  sys_oprt_mode_e_type                 oprt_mode;
  sys_modem_as_id_e_type               subs_id;
  ds_at_cm_ph_pref_type                pref_info;
  dsat_sys_detailed_plmn_list_s_type   available_networks;
#ifdef CM_API_AOC_INFO
  cm_ph_aoc_info_s_type                aoc_info;
#endif /* CM_API_AOC_INFO */
#ifdef FEATURE_UIM_RUN_TIME_ENABLE
  cm_rtre_control_e_type               rtre_control;
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
#if defined(FEATURE_HS_USB_SER3_PORT)
  cm_packet_state_e_type               packet_state;
#endif /* defined(FEATURE_HS_USB_SER3_PORT) */
#ifdef FEATURE_DSAT_LTE
  cm_lte_ue_mode_pref_e_type           eps_mode;
#endif /* FEATURE_DSAT_LTE */
#ifdef FEATURE_DATA_IS707
uint16                                 dsat_rtre_config;  /* CM RUIM CONFIG CHANGED event */
#endif /*FEATURE_DATA_IS707*/
#ifdef FEATURE_DUAL_SIM
sys_modem_as_id_e_type                 default_data_subs;
#endif /*FEATURE_DUAL_SIM*/
#ifdef FEATURE_DSAT_LTE
  sys_drx_cn_coefficient_s1_e_type     drx_coefficient;
  boolean                              set_drx_result;
#endif /*FEATURE_DSAT_LTE*/
  sys_modem_device_mode_e_type         device_mode;
  boolean                              is_gw_subscription_available;
  boolean                              is_hybr_gw_subscription_available;
  boolean                              is_hybr_gw3_subscription_available;
  /* Info passed between event handlers */
  boolean                              oprt_mode_changed;
} ds_at_ph_info_u_type;

/* DS_AT_CM_PH_EVENT data type. */
/* convey phone events from CM to ATCOP */
typedef struct
{
  cm_ph_event_e_type                   event;
  ds_at_ph_info_u_type                 event_info;
} ds_at_cm_ph_event_type;

#ifdef FEATURE_DSAT_ETSI_MODE
/* DS_AT_CM_SUPS_CMD data type,
   report Supplementary Services command execution status */
typedef struct
{
  void                                 *data_ptr;
  cm_sups_cmd_e_type                    cmd;
  cm_sups_cmd_err_e_type                cmd_err;
  cm_sups_cmd_err_cause_e_type          err_cause; 
  cm_alpha_s_type                       alpha;
} ds_at_cm_sups_cmd_type;

/* DS_AT_CM_SUPS_INFO_CMD data type,
   convey Supplementary Services event info to ATCOP */
typedef struct
{
  /* General */
  cm_client_id_type                    sups_client_id;  /* client */
  sys_modem_as_id_e_type               subs_id;
  uint8                                invoke_id;       /* Invocation Id */
  boolean                              ss_success;
  uint8                                ss_code;         /* SS code */
  uint8                                ss_operation;    /* SS operation */
  ie_ss_status_s_type                  ss_status;       /* SS status */
  ie_cm_ss_error_s_type                ss_error;        /* SS error */
  cm_ss_conf_e_type                    conf_type;       /* SS confirmation type */
  ss_password_value                    guidance_info;   /* Password guidance information */
  uss_data_s_type                      uss_data;        /* Unstructured SS data */
  cm_uss_data_e_type                   uss_data_type;   /* Type of USS message */
  cli_restriction_info_s_type          cli_restriction;
  basic_service_s_type                 basic_service;
  ie_forwarding_feature_list_s_type    fwd_feature_list;
  basic_service_group_list_s_type      bsg_list;
  ie_call_barring_info_s_type          call_barring_info;
} ds_at_sups_event_info_s_type;

typedef struct
{
  cm_sups_event_e_type                 event;
  ds_at_sups_event_info_s_type         event_info;
} ds_at_cm_sups_event_type;
#endif /* defined FEATURE_DSAT_ETSI_MODE */

typedef struct
{
  uint16                               rssi;   /* received signal strength indicator  */
  int16                                rssi2;   /* Unsigned RSSI from RRC(CM is a pass through) */
  int16                                rscp;  /* received signal code power */
  uint8                                bit_err_rate;
  int16                                ecio;
  int16                                pathloss; 
  int16                                sir;
  sys_sys_mode_e_type                  sys_mode;
  boolean                              hdr_hybrid;
  uint16                               hdr_rssi;
} dsatcmif_sigrep_s_type;

typedef struct dsat_sys_mm_information_s
{
  boolean                                   plmn_avail;
    /**< Whether the PLMN is available. */
  boolean                                   univ_time_and_time_zone_avail;
    /**< Whether the universal time is available. */
  boolean                                   time_zone_avail;
    /**< Whether the timezone is available. */
  sys_plmn_id_s_type                        plmn;
    /**< PLMN information. */
  sys_time_and_time_zone_s_type             univ_time_and_time_zone;
    /**< Universal Time Coordinated (UTC) time zone information. */
  sys_time_zone_type                        time_zone;
    /**< Current time zone information. */
} dsat_sys_mm_information_s_type;

/* DS_AT_CM_SS_INFO_CMD data type,
   convey Serving System event info to ATCOP */
typedef struct
{
  uint64                               changed_fields;
  boolean                              is_operational;
  sys_roam_status_e_type               roam_status;
  sys_srv_status_e_type                srv_status;
  sys_srv_domain_e_type                srv_domain;
  uint16                               rssi;
  uint16                               hdr_rssi;
  sys_sys_id_s_type                    sys_id;
  sys_csg_id_type                      csg_id;
  dsat_sys_mm_information_s_type       mm_info;
  sys_sim_state_e_type                 sim_state;
  cm_cell_srv_ind_s_type               cell_srv_ind;
  sys_sys_mode_e_type                  sys_mode;
  sys_srv_status_e_type                hdr_srv_status;
  sys_roam_status_e_type               hdr_roam_status;
  sys_active_prot_e_type               hdr_active_prot;
  boolean                              hdr_hybrid;
  boolean                              ps_data_suspend;
  boolean                              is_sys_forbidden;
  sys_cell_info_s_type                 cell_info;
  dsatcmif_sigrep_s_type               dsatcmif_signal_reporting;
  cm_emerg_num_list_s_type             *emerg_num_list;
  cm_reg_reject_info_s_type            reg_reject_info;
  sys_srv_domain_e_type                srv_capability;
#ifdef FEATURE_DSAT_ETSI_MODE
#ifdef FEATURE_WCDMA_L1_HS_CQI_STAT
cm_cqi_info_s_type                     dsat_cqi_info;
#endif /* FEATURE_WCDMA_L1_HS_CQI_STAT  */
#endif /* FEATURE_DSAT_ETSI_MODE */
#ifdef FEATURE_DSAT_LTE
  sys_lac_type                         lte_tac;
  byte                                 lte_rac_or_mme_code;
#endif /* FEATURE_DSAT_LTE */
#ifdef FEATURE_LTE_TO_1X
  boolean                              is_csfb_call_active;
#endif /* FEATURE_LTE_TO_1X */
  uint16                               cm_req_id;
  boolean                              last_reported_tz_changed;
  boolean                              report_emerg_num_list;
} ds_at_ss_info_s_type;

/*=====================================UIM==================================*/

#define DSAT_MAX_FULLNAME_STR_LEN 24
#define DSAT_MAX_SHORTNAME_STR_LEN 8
#define DSAT_MAX_NUM_REFRESH_FILES 6
#define MAX_MMGSDI_BUFFER_LEN  512

#ifdef FEATURE_DSAT_MMGSDI_AUTH
#define DSAT_MAX_VPM_DATA 70
#define DSAT_MAX_MD5_RSP 20
#endif /*  FEATURE_DSAT_MMGSDI_AUTH */

#ifdef FEATURE_DSAT_ETSI_MODE
#define DSAT_MAX_MMGSDI_BUFFER_LEN  255
#define DSAT_MMGSDI_ICC_IC_LEN  10
#endif /*FEATURE_DSAT_ETSI_MODE*/

typedef struct
{
  uint32                               client_ref; /* contains the status for the command */
  mmgsdi_return_enum_type              cmd_status;
  uint8                                sw1;     /*  Status Word 1. */
  uint8                                sw2;     /*  Status Word 2. */
  mmgsdi_data_type                     data;
} ds_at_crsm_info_type;

/* Type for DS_AT_STATUS_CMD */
typedef struct
{
  uint16                               cmd_id;  /* identify which cpb callback function
                             submitted this cmd */
  mmgsdi_return_enum_type              cmd_status; /* contains the data for the command */
  byte                                 cmd_data;   /* contains the data for the command */
  mmgsdi_session_id_type               session_id;
  boolean                              is_status_cmd_handling;
  mmgsdi_pin_info_type                 pin1;
  mmgsdi_pin_info_type                 pin2;
  mmgsdi_pin_info_type                 universal_pin;
  dsat_apps_id_e_type                  apps_id;
} ds_at_cmd_status_type;

/* dsat_fcn_ind_type holds the FCN details returned by MMGSDI_REFRESH_FCN call back */
/* MAX number of refresh files ( UMTS EF's +CDMA EF's) */

typedef struct
{
  uint32                               num_files; /* number of files in FCN ind */
  mmgsdi_file_enum_type                file_list[DSAT_MAX_NUM_REFRESH_FILES]; /* file list */
} dsat_fcn_ind_type;

typedef struct
{
  mmgsdi_events_enum_type              mmgsdi_event;  /* client event */
  int16                                cmd_status;  /* contains the status for the command */
  mmgsdi_pin_evt_info_type             pin;
  dsat_apps_id_e_type                  apps_id;       /* Internal application specific id  */
  mmgsdi_perso_evt_info_type           perso_info;
  mmgsdi_slot_id_enum_type             slot_id;
  mmgsdi_session_id_type               session_id;  /* session id obtained from mmgsdi */
  mmgsdi_card_err_evt_info_type        card_error; /*contain error information*/
  mmgsdi_app_enum_type                 app_type;
  mmgsdi_refresh_stage_enum_type       refresh_stage;
  mmgsdi_subscription_ready_evt_info_type  subscription_ready;
  boolean                              activated;
  
  union
  {
    dsat_fcn_ind_type                   fcn_ind;/* FCN details */
  }u;
} ds_at_mmgsdi_event_type;

#ifdef FEATURE_MMGSDI

typedef struct
{
   mmgsdi_len_type                     length;
   byte                                data[DSAT_MAX_MMGSDI_BUFFER_LEN];
} ds_at_sim_info_type;

/*  convery SIM read response from MMGSDI to ATCOP */

typedef struct
{
  mmgsdi_return_enum_type              cmd_status;  /* Status of the command */
  uint32                               client_ref;  /* Client requested */
  int32                                data_len;    /* Data length  */
  byte                                 data[MAX_MMGSDI_BUFFER_LEN]; /* actual data */
  mmgsdi_file_attributes_type          file_attr;  /* Complete file attributes */
  mmgsdi_srv_available_cnf_type        srv_cnf;    /* Service confirmation details */
  dsat_apps_id_e_type                  apps_id;
  mmgsdi_sw_type                       status_word;        /* Status word  */
  mmgsdi_rec_num_type                  accessed_rec_num;   /* Accessed Rec Num for Read Record */
  mmgsdi_offset_type                   accessed_offset;    /* Accessed Offset for Read Transparent */
  mmgsdi_session_id_type               session_id;    /**< Client session ID. */
  byte                                 iccid[DSAT_MMGSDI_ICC_IC_LEN];
}ds_at_mmgsdi_sim_info_type;

typedef struct
{
  mmgsdi_client_id_type                client_id;     /**< ID of the client to be notified. */
  mmgsdi_session_id_type               session_id;    /**< Client session ID. */
  mmgsdi_session_type_enum_type        session_type;  /**< Client session type. */
  mmgsdi_app_enum_type                 app_type;         /**< Client apps ID. */
} ds_at_mmgsdi_init_info;

typedef struct
{
  boolean                              long_plmn_name_ci;
  /*Flag that indicates  whether the country initials should be  added to plmn name*/
  boolean                              short_plmn_name_ci;
  byte                                 long_name[DSAT_MAX_FULLNAME_STR_LEN*2];
  /*long name, USS2 character are 2 byte long */
  byte                                 short_name[DSAT_MAX_SHORTNAME_STR_LEN*2];
  /*short name, USS2 character are 2 byte long */
  uint8                                long_plmn_name_spare_bits;
  /* Number of spare bits in the end octet of the   plmn name */
  uint8                                short_plmn_name_spare_bits;  
  uint16                               lac;/*lac info*/
  int32                                long_name_len;/*long name lenght*/
  int32                                short_name_len;/*short name lenght*/
  mmgsdi_plmn_id_type                  plmn_id;/* plmn identity*/
  mmgsdi_eons_encoding_enum_type       long_name_encoding; /*long name encoding type*/
  mmgsdi_eons_encoding_enum_type       short_name_encoding;/*short name encoding type*/
}ds_at_plmn_info;

typedef struct
{
  mmgsdi_cnf_enum_type	                cnf;
  mmgsdi_return_enum_type               mmgsdi_status;
  uint32                                client_ref;  /* Client requested */
  mmgsdi_session_id_type                session_id;
  mmgsdi_eons_name_type                 spn;              /**< Service provider name. */
  uint32                                num_of_plmn_ids;  /**< Number of plmn ids. */
  ds_at_plmn_info                       **plmn_info_ptr;  /**< Pointer to plmn info. */
}ds_at_mmgsdi_oper_name_type;

typedef struct
{
  mmgsdi_se13_plmn_info_type           plmn_info;    /*plmn info. */
  uint32                               client_ref;  /* Client requested */
  uint32                               num_of_plmn_ids;  /**< Number of plmn ids. */
  mmgsdi_cnf_enum_type                 cnf;
  mmgsdi_return_enum_type              mmgsdi_status;
}ds_at_mmgsdi_plmn_info_type;

#endif /* FEATURE_MMGSDI */

#ifdef FEATURE_DSAT_MMGSDI_AUTH

typedef struct
{
  int32                                data_len;
  uint8                                data[DSAT_MAX_MD5_RSP];

}dsat_mmgsdi_md5_cnf_s_type;

/* MD5/ATCOP Abort command handler */
typedef struct
{
  int16                                cmd_status;
  int                                  cme_error;
} ds_at_md5_abt_type;

typedef struct
{
  mmgsdi_key_type                      key_data; /*copy smekey for VPM key generation*/
  int32                                data_len;
  uint8                                data[DSAT_MAX_VPM_DATA];/*copy VPM*/

}dsat_mmgsdi_vpm_cnf_s_type;

typedef struct
{
  uint32                               client_ref;
  mmgsdi_return_enum_type              mmgsdi_status;
  mmgsdi_session_id_type               session_id;
  mmgsdi_sw_type                       status_word;
  union
  {
    boolean                            ssd_update_ok;          /* SSD Update confirm
                                                                    rsp */
    dword                              randbs;                 /* BS Chalenge  */
    boolean                            srv_available;          /* Service
                                                             Availability */
    dsat_mmgsdi_vpm_cnf_s_type         vpm_cnf;                /* VPM Responese */
    dsat_mmgsdi_md5_cnf_s_type         md5_cnf;                /* MD5 Response */
    mmgsdi_cave_auth_resp_type         run_cave_response_data; /* Cave response */
    dword                              authu;                  /* Authentication
                                                           challenge response */
  }auth_cmd;
}dsat707_mmgsdi_auth_cmd_type;

#endif /* FEATURE_DSAT_MMGSDI_AUTH  */

#ifdef FEATURE_DSAT_ETSI_MODE
#ifdef FEATURE_MMGSDI_ATCSIM
typedef struct
{
  mmgsdi_client_data_type              client_ref;
  mmgsdi_return_enum_type              cmd_status; /* contains the status for the command */
  mmgsdi_len_type                      apdu_len;
  uint16                               implicit_get_rsp_sw1;
  uint16                               implicit_get_rsp_sw2;
  int32                                channel_id;
  uint8                                *csim_apdu_data_ptr;
} ds_at_mmgsdi_apdu_resp_type;
#endif /* FEATURE_MMGSDI_ATCSIM */
#endif /* defined FEATURE_DSAT_ETSI_MODE */


/*=====================================PBM==================================*/
#if defined(FEATURE_ETSI_PBM) || defined(FEATURE_DSAT_CDMA_PBM)
/* DS_AT_PBM_CB_CMD data type,
   return pbm function cb results from pbm to ATCOP */
typedef struct
{
  pbm_device_type                      pbm_device; /* the storage type */
  int                                  used;       /* number of used entries for the storage type */
  int                                  total;      /* number of total entries forthe storage type */
  int                                  text_len;   /* Length of text field. */
} ds_at_pbm_file_info_type;

typedef struct
{
  pbm_field_s_type                     *fields;
  uint16                                Category;
  uint16                                ItemCount;
} dsat_pbm_addrbk_rec_s_type;

typedef struct
{
  int8                                  cmd;
  /* Pbm call back command identifier. It determines what type of cmd_info
        is contained in cmd_info union */
  pbm_return_T                          status; 
  /* Command status of the pbm cb function */
  union
  {
    ds_at_pbm_file_info_type           file_info; /* used for pbm_file_info_cb  function */
    pbm_record_s_type                  record;    /* used for pbm read or find  call back function */
    dsat_pbm_addrbk_rec_s_type         addrbk_rec; /* used to read address book info from PBM */
  } cmd_info;                 /* pbm call back command results */
} ds_at_pbm_info_type;
#endif /* defined(FEATURE_ETSI_PBM) || defined(FEATURE_DSAT_CDMA_PBM) */

/*====================External Client======================*/

typedef struct
{
  dsm_item_type                        *resp_buff_ptr;
  int32                                client_id;
  uint32                               atcop_info;
  boolean                              cmd_status;
  dsat_client_result_enum_type         result_code;
  dsat_fwd_resp_enum_type              resp_type;
}ds_at_ext_client_resp_s_type;

/*====================Command Forwarding ======================*/

typedef struct dsat_fwd_cmd
{
  uint32                               *user_info_ptr;
  dsat_cmd_list_type                   at_cmd_list[MAX_CMD_LIST];
  dsatclient_cmd_status_cb_type        status_cb;
  dsatclient_ext_at_cmd_exec_cb        ext_at_cmd_exec_cb;
  dsatclient_ext_at_cmd_abort_cb       ext_at_abort_cb;
  int32                                client_id;
  uint8                                num_valid_cmds;
}dsat_fwd_at_cmd_s_type;

typedef struct
{
  dsat_cmd_status_cb_fn_type           status_func_cb;
  void                                 *user_info_ptr ;
}ds_at_fwd_cmd_request_s_type;

typedef struct
{
  dsm_item_type                        *resp_buff_ptr;
  int32                                client_id;
  uint8                                cmd_status;
}ds_at_ext_client_urc_s_type;

/*====================Misc Strcuture ======================*/

/*====================QCECALL/ECALL ======================*/

#ifdef FEATURE_ECALL_APP
typedef struct
{
  ecall_session_status_type            session_status;
  ecall_type_of_call                   type_of_call;
  ecall_activation_type                activation_type;
}ds_at_ecall_cmd_s_type;
#endif /* FEATURE_ECALL_APP */

/*====================GPS Command ======================*/

#ifdef FEATURE_DSAT_GPS_CMDS
typedef struct
{
  pdsm_pd_info_s_type                  dsat_pd_info;
  pdsm_pd_event_type                   dsat_pd_event;
}ds_at_pdsm_info;
#endif /* FEATURE_DSAT_GPS_CMDS */

/*====================  RmSm  ======================*/

/* DS_AT_PDP_CMD data type, report PDP context activation/de-activation status */
typedef struct
{
  dsat_result_enum_type                response;
  dsat_rmsm_reason_type                reason_type;
  ps_iface_net_down_reason_type        iface_net_down;/* Primary PS call down reason */
  ps_extended_info_code_enum_type      ext_info_code;/* Secondary PS call down reason */
  dsat_rmsm_cmd_type                   dsat_rmsm_cmd;
} ds_at_pdp_cmd_type;

/*====================  FAX Command  ======================*/

#ifdef FEATURE_DATA_GCSD_FAX
/* Fax command status report */
typedef struct
{
  int16                                cmd_status;
} ds_at_fps_t31_status_type;
#endif  /* FEATURE_DATA_GCSD_FAX */

/*====================  EAP/AKA  ======================*/

#define DS_EAP_MAX_PRE_MASTER_KEY_SIZE  256

#ifdef FEATURE_DATA_PS_EAP 
typedef struct
{
  eap_sim_aka_task_srv_req_cb_type     req_cb;
  void *                               user_data;
}ds_at_eap_task_sw_info ;

typedef struct
{
  uint16                               cmd_id;
  uint8                                pmk[DS_EAP_MAX_PRE_MASTER_KEY_SIZE];
  uint16                               pmk_len;
  eap_handle_type                      handle;
  eap_result_enum_type                 result;
  void *                               user_data;
} ds_at_authi_eap_result_cmd_type;

typedef struct
{
  uint16                               cmd_id;
  uint8                                supp_auth[DS_EAP_MAX_PRE_MASTER_KEY_SIZE];
  uint16                               supp_auth_len;
  eap_sim_aka_supp_auth_mask_type      supp_auth_info_mask;
  eap_handle_type                      handle;
  eap_result_enum_type                 result;
  void *                               user_data;
} ds_at_authi_supp_eap_result_cmd_type;
typedef struct
{
  void                                 *user_data; 
  dsm_item_type                        *pkt;
}ds_at_eap_resp;
#endif /* FEATURE_DATA_PS_EAP */

/*=====================SMS=======================*/

/* SMS Event handler data structures */
#if defined(FEATURE_ETSI_SMS) || defined(FEATURE_CDMA_SMS)
typedef struct
{
  wms_cmd_id_e_type                    cmd;
  wms_cmd_err_e_type                   cmd_info;
}ds_at_sms_cmd_type;

/* SMS/ATCOP Abort command handler */
typedef struct
{
  int16                                cmd_status;
  int                                  cms_error;
} ds_at_sms_abt_type;
#endif /* defined(FEATURE_ETSI_SMS) || defined(FEATURE_CDMA_SMS) */

typedef enum cops_act_e
{
  DSAT_COPS_ACT_GSM    = 0,   /* User-specified GSM access technology */
  DSAT_COPS_ACT_GSM_COMP,            /* User-specified GSM Compact access technology */
  DSAT_COPS_ACT_UTRAN,               /* User-specified UMTS access technology */
  DSAT_COPS_ACT_GSM_EGPRS,           /* User-specified EGPRS access technology */
  DSAT_COPS_ACT_UTRAN_HSDPA,         /* User-specified HSDPA access technology */
  DSAT_COPS_ACT_UTRAN_HSUPA,         /* User-specified HSUPA access technology */
  DSAT_COPS_ACT_UTRAN_HSDPA_HSUPA, 
  DSAT_COPS_ACT_EUTRAN = 7,   /* User-specified LTE access technology */

  DSAT_COPS_ACT_AUTO,         /* Unspecified access technology */
  DSAT_COPS_ACT_MAX      /* Internal use only */
} dsat_cops_act_e_type;
/* network operator management */

typedef enum cops_csg_rat_e
{
  DSAT_COPS_CSG_RAT_GSM    = 0,
  DSAT_COPS_CSG_RAT_UMTS   = 1,
  DSAT_COPS_CSG_RAT_TDS    = 2,
  DSAT_COPS_CSG_RAT_LTE    = 3,
  DSAT_COPS_CSG_RAT_MAX
} dsat_cops_csg_rat_e_type;

typedef enum cops_mode_e
{
  DSAT_COPS_MODE_AUTO    = 0,  /* Automatic registration */
  DSAT_COPS_MODE_MANUAL  = 1,  /* Manual registration */
  DSAT_COPS_MODE_DEREG   = 2,  /* Force deregister */
  DSAT_COPS_MODE_SETFMT  = 3,  /* Set format only */
  DSAT_COPS_MODE_MANAUTO = 4,  /* Manual then auto registration */
  DSAT_COPS_MODE_MAX           /* Internal use only */
} dsat_cops_mode_e_type;
typedef enum cops_async_state_e
{
  DSAT_COPS_ASTATE_NULL,   /* No command active  */
  DSAT_COPS_ASTATE_ANETS,  /* Get available networks */
  DSAT_COPS_ASTATE_IMSI,   /* Get IMSI */
  DSAT_COPS_ASTATE_CMPH,   /* Get CM phone state */
  DSAT_COPS_ASTATE_PREF,   /* Network preference & PLMN changed */
  DSAT_COPS_ASTATE_PREF_ONLY, /* Network preference changed (not PLMN) */
  DSAT_COPS_ASTATE_DEREG,  /* Deregister from network */
  DSAT_COPS_ASTATE_WAIT,   /* Waiting for service change after pref changed */
  DSAT_COPS_ASTATE_WAIT_2, /* Waiting for second service change after pref changed */
  DSAT_COPS_ASTATE_ABORT,  /* User abort requested */
  DSAT_COPS_ASTATE_GET_NAME,/* Get Operator Name from MMGSDI */
  DSAT_COPS_ASTATE_MAX     /* Internal use only */
} dsat_cops_async_state_e_type;
#ifdef FEATURE_DSAT_DEV_CMDS
#ifdef FEATURE_DSAT_ETSI_MODE
/* CNTI technologies */
typedef enum
{
  DSAT_CNTI_NONE = 0,
  DSAT_CNTI_GSM,
  DSAT_CNTI_GPRS,
  DSAT_CNTI_EGPRS,
  DSAT_CNTI_UMTS,
  DSAT_CNTI_HSDPA,
  DSAT_CNTI_HSUPA,
  DSAT_CNTI_HSDPA_HSUPA,
  DSAT_CNTI_MAX
}dsati_cnti_tech_enum_type;
#endif /* FEATURE_DSAT_ETSI_MODE */
#endif /* FEATURE_DSAT_DEV_CMDS */
typedef enum 
{ 
  DSAT707_EXT_PARA_EB_IDX    = 0,
  DSAT707_EXT_PARA_EFCS_IDX  = 1,
  DSAT707_EXT_PARA_ER_IDX    = 2,
  DSAT707_EXT_PARA_ESR_IDX   = 3,
  DSAT707_EXT_PARA_ETBM_IDX  = 4,
  DSAT707_EXT_PARA_MA_IDX    = 5,
  DSAT707_EXT_PARA_MR_IDX    = 6,
  DSAT707_EXT_PARA_MS_IDX    = 7,
  DSAT707_EXT_PARA_MV18R_IDX = 8,
  DSAT707_EXT_PARA_MV18S_IDX = 9,
  DSAT707_EXT_PARA_CXT_IDX   = 10,
  DSAT707_EXT_PARA_CDR_IDX   = 11,
  DSAT707_EXT_PARA_CDS_IDX   = 12,
  DSAT707_EXT_PARA_CFC_IDX   = 13,
  DSAT707_EXT_PARA_CFG_IDX   = 14,
  DSAT707_EXT_PARA_CQD_IDX   = 15,
  DSAT707_EXT_PARA_CRC_IDX   = 16,
  DSAT707_EXT_PARA_CSQ_IDX   = 17,
  DSAT707_EXT_PARA_CMIP_IDX  = 18,
  DSAT707_EXT_PARA_CAD_IDX   = 19,
  DSAT707_EXT_PARA_CBIP_IDX   = 21,
  DSAT707_EXT_PARA_CMUX_IDX  = 22,
  DSAT707_EXT_PARA_CRM_IDX   = 23,
  DSAT707_EXT_PARA_CSS_IDX   = 24,
  DSAT707_EXT_PARA_CTA_IDX   = 25,
  DSAT707_EXT_PARA_GOI_IDX   = 27,
  DSAT707_EXT_PARA_ILRR_IDX  = 28,
  DSAT707_EXT_PARA_MAX_IDX   = 29
}dsat707_ext_para_index;

typedef enum 
{
  DSAT707_EXT_ACT_CDV_IDX   = 1000 ,
  DSAT707_EXT_ACT_CHV_IDX   = 1001 ,
  DSAT707_EXT_ACT_CHV0_IDX  = 1002 ,
  DSAT707_EXT_ACT_QCPIN_IDX = 1007 ,
  DSAT707_EXT_ACT_QCLCK_IDX = 1008 ,
  DSAT707_EXT_ACT_QCPWD_IDX = 1009 ,
  DSAT707_EXT_ACT_CLCC_IDX  = 1010 ,
  DSAT707_EXT_ACT_PZID_IDX  = 1011 ,
  DSAT707_EXT_ACT_CIMI_IDX  = 1015 ,
  DSAT707_EXT_ACT_MDN_IDX   = 1016 ,
  DSAT707_EXT_ACT_MAX_IDX   = 1017
}dsat707_ext_action_index_enum_type;

typedef enum
{
  DSAT707_FAX_FAA_IDX  = 2000 ,     
  DSAT707_FAX_FAP_IDX  = 2001 ,
  DSAT707_FAX_FBO_IDX  = 2002 ,
  DSAT707_FAX_FBS_IDX  = 2003 , 
  DSAT707_FAX_FBU_IDX  = 2004 ,
  DSAT707_FAX_FCQ_IDX  = 2005 ,
  DSAT707_FAX_FCC_IDX  = 2006 ,
  DSAT707_FAX_FCR_IDX  = 2007 ,
  DSAT707_FAX_FCT_IDX  = 2008 , 
  DSAT707_FAX_FDR_IDX  = 2009 ,  
  DSAT707_FAX_FDT_IDX  = 2010 ,  
  DSAT707_FAX_FEA_IDX  = 2011 ,
  DSAT707_FAX_FFC_IDX  = 2012 , 
  DSAT707_FAX_FHS_IDX  = 2013 , 
  DSAT707_FAX_FIE_IDX  = 2014 ,
  DSAT707_FAX_FIP_IDX  = 2015 ,
  DSAT707_FAX_FIS_IDX  = 2016 ,
  DSAT707_FAX_FKS_IDX  = 2017 ,     
  DSAT707_FAX_FLI_IDX  = 2018 ,  
  DSAT707_FAX_FLO_IDX  = 2019 , 
  DSAT707_FAX_FLP_IDX  = 2020 ,
  DSAT707_FAX_FMS_IDX  = 2021 ,
  DSAT707_FAX_FNR_IDX  = 2022 ,
  DSAT707_FAX_FNS_IDX  = 2023 ,
  DSAT707_FAX_FPA_IDX  = 2024 ,
  DSAT707_FAX_FPI_IDX  = 2025 , 
  DSAT707_FAX_FPP_IDX  = 2026 ,
  DSAT707_FAX_FPR_IDX  = 2027 ,  
  DSAT707_FAX_FPS_IDX  = 2028 , 
  DSAT707_FAX_FPW_IDX  = 2029 ,
  DSAT707_FAX_FRQ_IDX  = 2030 ,
  DSAT707_FAX_FRY_IDX  = 2031 ,
  DSAT707_FAX_FSA_IDX  = 2032 ,
  DSAT707_FAX_FSP_IDX  = 2033 , 
  DSAT707_FAX_MAX_IDX  = 2034
}dsat707_fax_index_enum_type;

typedef enum
{
  DSAT707_HDR_QCHDRT_IDX  = 3000, 
  DSAT707_HDR_QCHDRC_IDX  = 3001, 
  DSAT707_HDR_QCHDRR_IDX  = 3002,
  DSAT707_HDR_QCHDRET_IDX = 3003, 
  DSAT707_HDR_MAX_IDX =     3004
}dsat707_hdr_index_enum_type;

typedef enum 
{

  DSAT707_MIP_QCMIP_IDX      = 4000,
  DSAT707_MIP_QCMIPP_IDX     = 4001,
  DSAT707_MIP_QCMIPT_IDX     = 4002,
  DSAT707_MIP_QCMIPEP_IDX    = 4003,
  DSAT707_MIP_QCMIPMASS_IDX  = 4004,
  DSAT707_MIP_QCMIPMHSS_IDX  = 4005,
  DSAT707_MIP_QCMIPMASPI_IDX = 4006,
  DSAT707_MIP_QCMIPMHSPI_IDX = 4007,
  DSAT707_MIP_QCMIPRT_IDX    = 4008,
  DSAT707_MIP_QCMIPNAI_IDX   = 4009,
  DSAT707_MIP_QCMIPHA_IDX    = 4010,
  DSAT707_MIP_QCMIPPHA_IDX   = 4011,
  DSAT707_MIP_QCMIPSHA_IDX   = 4012,
  DSAT707_MIP_QCMIPGETP_IDX  = 4013,
  DSAT707_MIP_QCMIPMASSX_IDX = 4014,
  DSAT707_MIP_QCMIPMHSSX_IDX = 4015,
  DSAT707_MIP_MAX_IDX        = 4016
}dsat707_mip_index_enum_type;

typedef enum
{
  DSAT707_PSTATS_QCRLPD_IDX = 5000,  
  DSAT707_PSTATS_QCRLPR_IDX = 5001,
  DSAT707_PSTATS_QCRL3D_IDX = 5002,
  DSAT707_PSTATS_QCRL3R_IDX = 5003,
  DSAT707_PSTATS_QCPPPD_IDX = 5004 ,
  DSAT707_PSTATS_QCPPPR_IDX = 5005 , 
  DSAT707_PSTATS_QCIPD_IDX  = 5006 ,
  DSAT707_PSTATS_QCIPR_IDX  = 5007 , 
  DSAT707_PSTATS_QCUDPD_IDX = 5008 ,
  DSAT707_PSTATS_QCUDPR_IDX = 5009 ,
  DSAT707_PSTATS_QCTCPD_IDX = 5010 ,
  DSAT707_PSTATS_QCTCPR_IDX = 5011 ,
  DSAT707_PSTATS_MAX_IDX    = 5012
}dsat707_pstats_index_enum_type;

typedef enum
{
  DSAT707_SMS_QCNMI_IDX = 6000 ,
  DSAT707_SMS_QCSMP_IDX = 6001 ,
  DSAT707_SMS_QCPMS_IDX = 6002 ,
  DSAT707_SMS_QCMGR_IDX = 6003 ,
  DSAT707_SMS_QCMGS_IDX = 6004 ,
  DSAT707_SMS_QCMSS_IDX = 6005 ,
  DSAT707_SMS_QCMGD_IDX = 6006 ,
  DSAT707_SMS_QCMGL_IDX = 6007 ,
  DSAT707_SMS_QCMGF_IDX = 6008 ,
  DSAT707_SMS_QCMGW_IDX = 6009 ,
  DSAT707_SMS_HCMGW_IDX = 6010 ,
  DSAT707_SMS_HCMGL_IDX = 6011 ,
  DSAT707_SMS_HCMGS_IDX = 6012 ,
  DSAT707_SMS_HCMGR_IDX = 6013 ,
  DSAT707_SMS_HSMSSS_IDX= 6014 ,
  DSAT707_SMS_MAX_IDX   = 6015
}dsat707_sms_index_enum_type;

typedef enum
{
  DSAT707_VEND_PARA_QCPREV_IDX   =  7000 ,
  DSAT707_VEND_PARA_QCMDR_IDX    =  7002 ,
  DSAT707_VEND_PARA_QCSCRM_IDX   =  7003 ,
  DSAT707_VEND_PARA_QCTRTL_IDX   =  7004 ,
  DSAT707_VEND_PARA_QCQNC_IDX    =  7005 ,
  DSAT707_VEND_PARA_QCSO_IDX     =  7006 ,
  DSAT707_VEND_PARA_QCVAD_IDX    =  7007 ,
  DSAT707_VEND_PARA_QCCAV_IDX    =  7008 ,
  DSAT707_VEND_PARA_QCCHV_IDX    =  7009 ,
  DSAT707_VEND_PARA_QCQOSPRI_IDX =  7010 ,
  DSAT707_VEND_PARA_QCCHS_IDX    =  7011 ,
  DSAT707_VEND_PARA_HDRCSQ_IDX   =  7012 ,
  DSAT707_VEND_PARA_GSN_IDX      =  7013 , 
  DSAT707_VEND_PARA_CGSN_IDX     =  7014 ,
  DSAT707_VEND_PARA_MEID_IDX     =  7015 ,
  DSAT707_VEND_PARA_MAX_IDX      =  7016
} dsat707_vend_para_index_enum_type;

typedef enum
{

  DSAT707_VEND_ACT_CPBR_IDX      = 8003 ,
  DSAT707_VEND_ACT_CPBF_IDX      = 8004 ,
  DSAT707_VEND_ACT_CPBW_IDX      = 8005 ,
  DSAT707_VEND_ACT_QCOTC_IDX     = 8006 ,
  DSAT707_VEND_ACT_QCVAD_IDX     = 8007 ,
  DSAT707_VEND_ACT_QCCAV_IDX     = 8008 ,
  DSAT707_VEND_ACT_QCCHV_IDX     = 8009 ,
  DSAT707_VEND_ACT_HWVER_IDX     = 8010 ,
  DSAT707_VEND_ACT_RESET_IDX     = 8011 ,
  DSAT707_VEND_ACT_VOLT_IDX      = 8012 ,
  DSAT707_VEND_ACT_CPIN_IDX      = 8013 ,
  DSAT707_VEND_ACT_CAVE_IDX      = 8014 ,
  DSAT707_VEND_ACT_SSDUPD_IDX    = 8015 ,
  DSAT707_VEND_ACT_SSDUPDCFM_IDX = 8016 ,
  DSAT707_VEND_ACT_VPM_IDX       = 8017 ,
  DSAT707_VEND_ACT_UIMAUTH_IDX   = 8018 ,
  DSAT707_VEND_ACT_GSN_IDX       = 8019 ,
  DSAT707_VEND_ACT_MD5_IDX       = 8020 ,
  DSAT707_VEND_ACT_QCMSID_IDX    = 8021 ,
  DSAT707_VEND_ACT_SPSERVICE_IDX = 8022 ,
  DSAT707_VEND_ACT_SPSPC_IDX     = 8023 ,
  DSAT707_VEND_ACT_SPRESET_IDX   = 8024 ,
  DSAT707_VEND_ACT_SPFWREV_IDX   = 8025 ,
  DSAT707_VEND_ACT_SPPRL_IDX     = 8026 ,
  DSAT707_VEND_ACT_SPROAM_IDX    = 8027 ,
  DSAT707_VEND_ACT_SPERI_IDX     = 8028 ,
  DSAT707_VEND_ACT_SPSIGDBM_IDX  = 8029 ,
  DSAT707_VEND_ACT_SPLOCKED_IDX  = 8030 ,
  DSAT707_VEND_ACT_SPUNLOCK_IDX  = 8031 ,
  DSAT707_VEND_ACT_SPLOCK_IDX    = 8032 ,
  DSAT707_VEND_ACT_SPLOCKCHG_IDX = 8033 ,
  DSAT707_VEND_ACT_SPCURRENTLOCK_IDX    = 8034 ,
  DSAT707_VEND_ACT_SPNAI_IDX     = 8035 ,
  DSAT707_VEND_ACT_ACT_A_TRACKING_IDX   = 8036 ,
  DSAT707_VEND_ACT_SPGETLOCATION_IDX    = 8037 ,
  DSAT707_VEND_ACT_SPNMEA_IDX           = 8038 ,
  DSAT707_VEND_ACT_SPLOCATION_IDX       = 8039 ,
  DSAT707_VEND_ACT_ACT_GPS_MODE_IDX     = 8040 ,
  DSAT707_VEND_ACT_ACT_GPS_PORT_IDX     = 8041 ,
  DSAT707_VEND_ACT_ACT_PDE_TRS_IDX      = 8042 ,
  DSAT707_VEND_ACT_ACT_GPS_PDEADDR_IDX  = 8043 ,
  DSAT707_VEND_ACT_ACT_INIT_MASK_IDX    = 8044 ,
  DSAT707_VEND_ACT_ACT_GET_POS_IDX      = 8045 ,
  DSAT707_VEND_ACT_ACT_GPS_INFO_IDX     = 8046 ,
  DSAT707_VEND_ACT_ACT_GPS_STOP_IDX     = 8047 ,
  DSAT707_VEND_ACT_MAX_IDX              = 8048
}dsat707_vend_action_index_enum_type;

typedef enum
{
  DSAT_BASIC_N_C_IDX   = 9000, 
  DSAT_BASIC_N_D_IDX   = 9001, 
  DSAT_BASIC_N_E_IDX   = 9002,     
  DSAT_BASIC_N_F_IDX   = 9003,       
  DSAT_BASIC_N_S_IDX   = 9004,      
  DSAT_BASIC_N_V_IDX   = 9005,       
  DSAT_BASIC_N_W_IDX   = 9006,       
  DSAT_BASIC_E_IDX     = 9007,        
  DSAT_BASIC_I_IDX     = 9008,       
  DSAT_BASIC_L_IDX     = 9009,        
  DSAT_BASIC_M_IDX     = 9010,        
  DSAT_BASIC_Q_IDX     = 9011,       
  DSAT_BASIC_V_IDX     = 9012,       
  DSAT_BASIC_X_IDX     = 9013,       
  DSAT_BASIC_Z_IDX     = 9014,       
  DSAT_BASIC_T_IDX     = 9015,      
  DSAT_BASIC_P_IDX     = 9016,     
  DSAT_BASIC_DS_Q_IDX  = 9017,
  DSAT_BASIC_DS_S_IDX  = 9018,    
  DSAT_BASIC_DS_V_IDX  = 9019,    
  DSAT_BASIC_P_V_IDX   = 9020,    
  DSAT_BASIC_SIR_V_IDX = 9021, 
  DSAT_BASIC_SIR_Q_IDX = 9022, 
  DSAT_BASIC_B_IDX     = 9023,       
  DSAT_BASIC_MAX_IDX   = 9024
}dsat_basic_index_enum_type;

typedef enum
{
 DSAT_BASIC_ACT_D_IDX = 10000,       
 DSAT_BASIC_ACT_A_IDX = 10001,      
 DSAT_BASIC_ACT_H_IDX = 10002,      
 DSAT_BASIC_ACT_O_IDX = 10003,          
 DSAT_BASIC_ACT_MAX_IDX = 10004
}dsat_basic_action_index_enum_type;

typedef enum
{

  DSAT_SREG_S0_IDX   = 11000,    
  DSAT_SREG_S2_IDX   = 11001,    
  DSAT_SREG_S3_IDX   = 11002,  
  DSAT_SREG_S4_IDX   = 11003,     
  DSAT_SREG_S5_IDX   = 11004,      
  DSAT_SREG_S6_IDX   = 11005,      
  DSAT_SREG_S7_IDX   = 11006,    
  DSAT_SREG_S8_IDX   = 11007,      
  DSAT_SREG_S9_IDX   = 11008,    
  DSAT_SREG_S10_IDX  = 11009,     
  DSAT_SREG_S11_IDX  = 11010,       
  DSAT_SREG_S30_IDX  = 11011,   
  DSAT_SREG_S103_IDX = 11012,     
  DSAT_SREG_S104_IDX = 11013,     
  DSAT_SREG_MAX_IDX  = 11014
}dsat_sreg_index_enum_type;

typedef enum
{
  DSAT_EXT_FCLASS_IDX  = 12000,  
  DSAT_EXT_ICF_IDX     = 12001,    
  DSAT_EXT_IFC_IDX     = 12002, 
  DSAT_EXT_IPR_IDX     = 12003,    
  DSAT_EXT_GMI_IDX     = 12004,   
  DSAT_EXT_GMM_IDX     = 12005,  
  DSAT_EXT_GMR_IDX     = 12006,   
  DSAT_EXT_GCAP_IDX    = 12007,  
  DSAT_EXT_GSN_IDX     = 12008,    
  DSAT_EXT_DR_IDX      = 12009,     
  DSAT_EXT_DS_IDX      = 12010,   
  DSAT_EXT_CMEE_IDX    = 12011,   
  DSAT_EXT_WS46_IDX    = 12012,    
  DSAT_EXT_PACSP_IDX   = 12013,    
  DSAT_EXT_TST32_IDX   = 12014, 
  DSAT_EXT_CFUN_IDX    = 12016,   
  DSAT_EXT_CLCC_IDX    = 12017,
  DSAT_EXT_MAX_IDX     = 12018
}dsat_ext_index_enum_type;


typedef enum
{
  DSAT_VENDOR_QCSIMSTAT_IDX   = 13000,   
  DSAT_VENDOR_QCPBMPREF_IDX   = 13001,   
  DSAT_VENDOR_CREG_IDX        = 13002,   
  DSAT_VENDOR_CCLK_IDX        = 13003,   
  DSAT_VENDOR_QCCNMI_IDX      = 13004,    
  DSAT_VENDOR_CNTI_IDX        = 13005,  
  DSAT_VENDOR_QCCLR_IDX       = 13006,   
  DSAT_VENDOR_QCDMG_IDX       = 13007,   
  DSAT_VENDOR_QCDMR_IDX       = 13008,  
  DSAT_VENDOR_QCDOM_IDX       = 13009, 
  DSAT_VENDOR_QCDNSP_IDX      = 13010, 
  DSAT_VENDOR_QCDNSS_IDX      = 13011, 
  DSAT_VENDOR_QCTER_IDX       = 13012, 
  DSAT_VENDOR_QCSLOT_IDX      = 13013, 
  DSAT_VENDOR_QCSIMAPP_IDX    = 13014, 
  DSAT_VENDOR_QCSLIP_IDX      = 13015, 
  DSAT_VENDOR_QCPINSTAT_IDX   = 13016, 
  DSAT_VENDOR_QCPDPP_IDX      = 13017, 
  DSAT_VENDOR_QCPDPLT_IDX     = 13018, 
  DSAT_VENDOR_QCPDPFAM_IDX    = 13019, 
  DSAT_VENDOR_QCGANSM_IDX     = 13020,   
  DSAT_VENDOR_QCGARL_IDX      = 13021,    
  DSAT_VENDOR_QCGAPL_IDX      = 13022,   
  DSAT_VENDOR_QCPWRDN_IDX     = 13023, 
  DSAT_VENDOR_QCDGEN_IDX      = 13024,    
  DSAT_VENDOR_QCPDPCFGE_IDX   = 13025, 
  DSAT_VENDOR_BREW_IDX        = 13026,  
  DSAT_VENDOR_QCSCFTEST_IDX   = 13027,  
  DSAT_VENDOR_QCGSN_IDX       = 13028,
  DSAT_VENDOR_QCSKU_IDX       = 13029,  
  DSAT_VENDOR_QCANTE_IDX      = 13030,  
  DSAT_VENDOR_QCRPW_IDX       = 13031,  
  DSAT_VENDOR_QCSQ_IDX        = 13032, 
  DSAT_VENDOR_CSQ_IDX         = 13033, 
  DSAT_VENDOR_QCSYSMODE_IDX   = 13034,  
  DSAT_VENDOR_QCCTM_IDX       = 13035,  
  DSAT_VENDOR_QCBANDPREF_IDX  = 13036,        
  DSAT_VENDOR_PREFMODE_IDX    = 13037,   
  DSAT_VENDOR_SYSINFO_IDX     = 13038,
  DSAT_VENDOR_SYSCONFIG_IDX   = 13039,
  DSAT_VENDOR_CARDMODE_IDX    = 13040,
  DSAT_VENDOR_DSCI_IDX        = 13041,
  DSAT_VENDOR_QCVOLT_IDX      = 13042, 
  DSAT_VENDOR_QCHWREV_IDX     = 13043,
  DSAT_VENDOR_QCBOOTVER_IDX   = 13044,
  DSAT_VENDOR_QCTEMP_IDX      = 13045,
  DSAT_VENDOR_QCAGC_IDX       = 13046,
  DSAT_VENDOR_QCALLUP_IDX     = 13047,
  DSAT_VENDOR_ECALL_IDX       = 13048,
  DSAT_VENDOR_QCMRUE_IDX      = 13049,
  DSAT_VENDOR_QCMRUC_IDX      = 13050,
  DSAT_VENDOR_QCAPNE_IDX      = 13051,
  DSAT_VENDOR_QCDEFPROF_IDX   = 13052,
  DSAT_VENDOR_QCPDPIMSCFGE_IDX= 13053, 
  DSAT_VENDOR_QCCLAC_IDX      = 13054,
  DSAT_VENDOR_SPN_IDX         = 13055,
  DSAT_VENDOR_QCRMCALL_IDX    = 13056,
  DSAT_VENDOR_QCDRX_IDX       = 13057,
  DSAT_VENDOR_QCRSRP_IDX      = 13058,
  DSAT_VENDOR_QCRSRQ_IDX      = 13059,
  DSAT_VENDOR_QCATMOD_IDX     = 13060,
  DSAT_VENDOR_QCCOPS_IDX      = 13061,
  DSAT_VENDOR_QCHCOPS_IDX     = 13062,
  DSAT_VENDOR_QCHCREG_IDX     = 13063,
  DSAT_VENDOR_MODE_IDX        = 13064,
  DSAT_VENDOR_QCSIMT_IDX      = 13065,
  DSAT_VENDOR_QCNSP_IDX       = 13066,
  DSAT_VENDOR_QCRCIND_IDX     = 13067,
  DSAT_VENDOR_QCACQDBC_IDX    = 13068,
  DSAT_VENDOR_QCPDPCFGEXT_IDX = 13069,
  DSAT_VENDOR_QCVOIPM_IDX     = 13070,
  DSAT_VENDOR_QCPRFCRT_IDX    = 13071,
  DSAT_VENDOR_QCPRFMOD_IDX    = 13072,
  DSAT_VENDOR_MAX_IDX         = 13073
}dsat_vendor_index_enum_type;

typedef enum
{
  DSATETSI_EXT_ACT_CSQ_ETSI_IDX  = 14000,    
  DSATETSI_EXT_ACT_CBC_ETSI_IDX  = 14001,   
  DSATETSI_EXT_ACT_CPAS_ETSI_IDX = 14002,   
  DSATETSI_EXT_ACT_CPIN_ETSI_IDX = 14003,  
  DSATETSI_EXT_ACT_CMEC_ETSI_IDX = 14004,  
  DSATETSI_EXT_ACT_CKPD_ETSI_IDX = 14005,  
  DSATETSI_EXT_ACT_CIND_ETSI_IDX = 14006,  
  DSATETSI_EXT_ACT_CMER_ETSI_IDX = 14007, 
  DSATETSI_EXT_ACT_CGATT_ETSI_IDX  = 14008,
  DSATETSI_EXT_ACT_CGACT_ETSI_IDX  = 14009,
  DSATETSI_EXT_ACT_CGCMOD_ETSI_IDX = 14010,
  DSATETSI_EXT_ACT_CPBS_ETSI_IDX = 14011,  
  DSATETSI_EXT_ACT_CPBR_ETSI_IDX = 14012,  
  DSATETSI_EXT_ACT_CPBF_ETSI_IDX = 14013,  
  DSATETSI_EXT_ACT_CPBW_ETSI_IDX = 14014,   
  DSATETSI_EXT_ACT_CPMS_ETSI_IDX = 14015,  
  DSATETSI_EXT_ACT_CNMI_ETSI_IDX = 14016,  
  DSATETSI_EXT_ACT_CMGL_ETSI_IDX = 14017,   
  DSATETSI_EXT_ACT_CMGR_ETSI_IDX = 14018,   
  DSATETSI_EXT_ACT_CMGS_ETSI_IDX = 14019,   
  DSATETSI_EXT_ACT_CMSS_ETSI_IDX = 14020,  
  DSATETSI_EXT_ACT_CMGW_ETSI_IDX = 14021, 
  DSATETSI_EXT_ACT_CMGD_ETSI_IDX = 14022,  
  DSATETSI_EXT_ACT_CMGC_ETSI_IDX = 14023,  
  DSATETSI_EXT_ACT_CNMA_ETSI_IDX = 14024,  
  DSATETSI_EXT_ACT_CMMS_ETSI_IDX = 14025,   
  DSATETSI_EXT_ACT_FTS_ETSI_IDX = 14026, 
  DSATETSI_EXT_ACT_FRS_ETSI_IDX = 14027,  
  DSATETSI_EXT_ACT_FTH_ETSI_IDX = 14028,   
  DSATETSI_EXT_ACT_FRH_ETSI_IDX = 14029, 
  DSATETSI_EXT_ACT_FTM_ETSI_IDX = 14030,  
  DSATETSI_EXT_ACT_FRM_ETSI_IDX = 14031,   
  DSATETSI_EXT_ACT_CHUP_ETSI_IDX = 14032,    
  DSATETSI_EXT_ACT_CCFC_ETSI_IDX = 14033,  
  DSATETSI_EXT_ACT_CCUG_ETSI_IDX = 14034,  
  DSATETSI_EXT_ACT_COPS_ETSI_IDX = 14035,   
  DSATETSI_EXT_ACT_CLCK_ETSI_IDX = 14036,   
  DSATETSI_EXT_ACT_CPWD_ETSI_IDX = 14037,   
  DSATETSI_EXT_ACT_CUSD_ETSI_IDX = 14038,
  DSATETSI_EXT_ACT_CAOC_ETSI_IDX = 14039,   
  DSATETSI_EXT_ACT_CACM_ETSI_IDX = 14040,  
  DSATETSI_EXT_ACT_CAMM_ETSI_IDX = 14041,  
  DSATETSI_EXT_ACT_CPUC_ETSI_IDX = 14042,   
  DSATETSI_EXT_ACT_CCWA_ETSI_IDX = 14043,
  DSATETSI_EXT_ACT_CHLD_ETSI_IDX = 14044,   
  DSATETSI_EXT_ACT_CIMI_ETSI_IDX = 14045,   
  DSATETSI_EXT_ACT_CGMI_ETSI_IDX = 14046,   
  DSATETSI_EXT_ACT_CGMM_ETSI_IDX = 14047,  
  DSATETSI_EXT_ACT_CGMR_ETSI_IDX = 14048,   
  DSATETSI_EXT_ACT_CGSN_ETSI_IDX = 14049,   
  DSATETSI_EXT_ACT_CNUM_ETSI_IDX = 14050,   
  DSATETSI_EXT_ACT_CSIM_ETSI_IDX = 14051,   
  DSATETSI_EXT_ACT_CRSM_ETSI_IDX = 14052,   
  DSATETSI_EXT_ACT_CCLK_ETSI_IDX = 14053,  
  DSATETSI_EXT_ACT_CRSL_ETSI_IDX = 14054,  
  DSATETSI_EXT_ACT_CALM_ETSI_IDX = 14055,  
  DSATETSI_EXT_ACT_CVIB_ETSI_IDX = 14056,   
  DSATETSI_EXT_ACT_CRMP_ETSI_IDX = 14057,  
  DSATETSI_EXT_ACT_CLCC_ETSI_IDX = 14058, 
  DSATETSI_EXT_ACT_COPN_ETSI_IDX = 14059,  
  DSATETSI_EXT_ACT_CPOL_ETSI_IDX = 14060,  
  DSATETSI_EXT_ACT_CPLS_ETSI_IDX = 14061,   
  DSATETSI_EXT_ACT_CTZR_ETSI_IDX = 14062,   
  DSATETSI_EXT_ACT_CTZU_ETSI_IDX = 14063,   
  DSATETSI_EXT_ACT_CLAC_ETSI_IDX = 14064,   
  DSATETSI_EXT_ACT_CLIP_ETSI_IDX = 14065,  
  DSATETSI_EXT_ACT_COLP_ETSI_IDX = 14066,    
  DSATETSI_EXT_ACT_CDIP_ETSI_IDX = 14067,  
  DSATETSI_EXT_ACT_CTFR_ETSI_IDX = 14068,  
  DSATETSI_EXT_ACT_CLIR_ETSI_IDX = 14069,   
  DSATETSI_EXT_ACT_CGANS_ETSI_IDX = 14070, 
  DSATETSI_EXT_ACT_CQI_ETSI_IDX   = 14071,  
  DSATETSI_EXT_ACT_VTS_ETSI_IDX   = 14072,   
  DSATETSI_EXT_ACT_ICCID_ETSI_IDX = 14073,  
  DSATETSI_EXT_ACT_CUAD_ETSI_IDX  = 14074,
  DSATETSI_EXT_ACT_CEAP_ETSI_IDX  = 14075,
  DSATETSI_EXT_ACT_CERP_ETSI_IDX  = 14076,
  DSATETSI_EXT_ACT_CSTF_ETSI_IDX  = 14077,
  DSATETSI_EXT_ACT_CVMOD_ETSI_IDX = 14078,
  DSATETSI_EXT_ACT_CEN_ETSI_IDX   = 14079,
  DSATETSI_EXT_ACT_CSDF_ETSI_IDX  = 14080,
  DSATETSI_EXT_ACT_CMUX_ETSI_IDX  = 14081,
  DSATETSI_EXT_ACT_CNMPSD_ETSI_IDX = 14082,
  DSATETSI_EXT_ACT_MAX_ETSI_IDX    = 14083
  }dsatetsi_ext_action_index_enum_type;

typedef enum
{
  DSATETSI_EXT_CBST_IDX        = 15000, 
  DSATETSI_EXT_CRLP_IDX        = 15001, 
  DSATETSI_EXT_CV120_IDX       = 15002, 
  DSATETSI_EXT_CHSN_IDX        = 15003,  
  DSATETSI_EXT_CSSN_IDX        = 15004, 
  DSATETSI_EXT_CREG_IDX        = 15005, 
  DSATETSI_EXT_CGREG_IDX       = 15006,  
  DSATETSI_EXT_CEREG_IDX       = 15007, 
  DSATETSI_EXT_CSCS_IDX        = 15009, 
  DSATETSI_EXT_CSTA_IDX        = 15010,  
  DSATETSI_EXT_CR_IDX          = 15011,
  DSATETSI_EXT_CEER_IDX        = 15012,
  DSATETSI_EXT_CRC_IDX         = 15013,
  DSATETSI_EXT_CGDCONT_IDX     = 15014,
  DSATETSI_EXT_CGDSCONT_IDX    = 15015,
  DSATETSI_EXT_CGTFT_IDX       = 15016,
  DSATETSI_EXT_CGEQREQ_IDX     = 15017,
  DSATETSI_EXT_CGEQMIN_IDX     = 15018,
  DSATETSI_EXT_CGEQOS_IDX      = 15019,
  DSATETSI_EXT_CGCONTRDP_IDX   = 15020, 
  DSATETSI_EXT_CGSCONTRDP_IDX  = 15021, 
  DSATETSI_EXT_CGTFTRDP_IDX    = 15022,
  DSATETSI_EXT_CGEQOSRDP_IDX   = 15023,
  DSATETSI_EXT_CGQREQ_IDX      = 15024,
  DSATETSI_EXT_CGQMIN_IDX      = 15025,
  DSATETSI_EXT_CGEREP_IDX      = 15026,
  DSATETSI_EXT_CGPADDR_IDX     = 15027, 
  DSATETSI_EXT_CGDATA_IDX      = 15028, 
  DSATETSI_EXT_CGCLASS_IDX     = 15029, 
  DSATETSI_EXT_CGAUTO_IDX      = 15030, 
  DSATETSI_EXT_CGSMS_IDX       = 15031, 
  DSATETSI_EXT_CSMS_IDX        = 15032, 
  DSATETSI_EXT_CMGF_IDX        = 15033,  
  DSATETSI_EXT_CSAS_IDX        = 15034, 
  DSATETSI_EXT_CRES_IDX        = 15035, 
  DSATETSI_EXT_CSCA_IDX        = 15036, 
  DSATETSI_EXT_CSMP_IDX        = 15037, 
  DSATETSI_EXT_CSDH_IDX        = 15038, 
  DSATETSI_EXT_CSCB_IDX        = 15039, 
  DSATETSI_EXT_FDD_IDX         = 15040, 
  DSATETSI_EXT_FAR_IDX         = 15041, 
  DSATETSI_EXT_FCL_IDX         = 15042, 
  DSATETSI_EXT_FIT_IDX         = 15043, 
  DSATETSI_EXT_ES_IDX          = 15044,  
  DSATETSI_EXT_ESA_IDX         = 15045, 
  DSATETSI_EXT_CMOD_IDX        = 15046,  
  DSATETSI_EXT_CEMODE_IDX      = 15047, 
  DSATETSI_EXT_CVHU_IDX        = 15048,
  DSATETSI_EXT_CECALL_IDX      = 15049,
  DSATETSI_EXT_CGPIAF_IDX      = 15050,
  DSATETSI_EXT_MAX_IDX         = 15051
}dsatetsi_ext_index_enum_type;

/*Discribe possible value type */
typedef enum
{
  NUM_TYPE,
  STR_TYPE,
  ARRAY_OF_STR_TYPE,
  MIX_NUM_TYPE,
  MIX_STR_TYPE,
  MIX_INDEX_TYPE,
  ARRAY_BROKEN_RANGE_TYPE_VALUE,
  ADDR_V4_TYPE,
  ADDR_P2_V6_TYPE,
  ADDR_P_V6_TYPE,
  ADDR_C_V6_TYPE,
  LNGR_TMR_TYPE
}value_type_enum_type;

#define MAX_V4_ADDR_LEN 16

typedef uint32 dsat_v4_addr_type;

typedef union
{
  ds_profile_3gpp_pdp_addr_type_ipv6 v6_3gpp;
  ds_profile_3gpp2_in6_addr_type     v6_3gpp2;
  ds_profile_in6_addr_type           v6_common;
}dsat_v6_addr_u_type;

/* Enum for Parameter Related Errors */
typedef enum
{
  DSAT_ERR_NO_ERROR                  = 0,
  DSAT_ERR_INVALID_NUM_OF_ARGS       = 1,
  DSAT_ERR_INVALID_ARGUMENT          = 2,
  DSAT_ERR_ATOI_CONVERSION_FAILURE   = 3,
  DSAT_ERR_QUOTE_STRIP_FAILURE       = 4,
  DSAT_ERR_PARAMETER_OUT_OF_RANGE    = 5,
  DSAT_ERR_PARAMETER_MANDATORY       = 6,
  DSAT_ERR_INVALID_TOKENS            = 7,
  DSAT_ERR_INVALID_SYNTAX            = 8,
  DSAT_ERR_INVALID_OPERATION         = 9,
  DSAT_ERR_NO_VALID_PROFILE          = 10,
  DSAT_ERR_VALIDATION_FAIL           = 11,
  DSAT_ERR_INVALID_FORMAT            = 12,
} dsat_err_e_type;

typedef struct
{
  dsat_err_e_type errval;
  uint8           arg_num;
} dsat_error_info_s_type;

/*===========================================================================
                     External Client support MACROS
===========================================================================*/
#ifdef FEATURE_DSAT_EXT_CLIENT_SUPPORT 

#define  DSAT_EXT_CLIENT_ID_INVALID  DSAT_CLIENT_TYPE_INVALID
#define  DSAT_EXT_CLIENT_ID_MAX      DSAT_CLIENT_TYPE_MAX /* Client ID should be less than 3*/
#define  MAX_RESP_SIZE MAX_LINE_SIZE
#endif /* FEATURE_DSAT_EXT_CLIENT_SUPPORT */

/*---------------------------------------------------------------------------
  The arrays dsat_qcdnspri_val and dsat_qcdnssec_val are used to 
  store the incoming strings. Since the incoming string should contain a 
  " to start and end the string and "/0", the length 18 was chosen. 
  Example, "129.046.050.007"
---------------------------------------------------------------------------*/
#define MAX_DNS_SIZE 18


/*===========================================================================
                     Debugging Message Definitions
===========================================================================*/
#define DSATI_ERROR_MSG()  DS_AT_MSG0_MED("DSAT_ERROR result")

#define DS_AT_MSG0_LOW(fmtstr) \
            DATA_ATCOP_MSG0(MSG_LEGACY_LOW, fmtstr)

#define DS_AT_MSG0_MED(fmtstr) \
            DATA_ATCOP_MSG0(MSG_LEGACY_MED, fmtstr)

#define DS_AT_MSG0_HIGH(fmtstr) \
            DATA_ATCOP_MSG0(MSG_LEGACY_HIGH, fmtstr)

#define DS_AT_MSG0_ERROR(fmtstr) \
            DATA_ATCOP_MSG0(MSG_LEGACY_ERROR, fmtstr)

#define DS_AT_MSG0_FATAL(fmtstr) \
            DATA_ATCOP_MSG0(MSG_LEGACY_FATAL, fmtstr)

#define DS_AT_MSG1_LOW(fmtstr, arg1) \
          DATA_ATCOP_MSG1(MSG_LEGACY_LOW, fmtstr, arg1)

#define DS_AT_MSG1_MED(fmtstr, arg1) \
          DATA_ATCOP_MSG1(MSG_LEGACY_MED, fmtstr, arg1)

#define DS_AT_MSG1_HIGH(fmtstr, arg1) \
          DATA_ATCOP_MSG1(MSG_LEGACY_HIGH, fmtstr, arg1)

#define DS_AT_MSG1_ERROR(fmtstr, arg1) \
          DATA_ATCOP_MSG1(MSG_LEGACY_ERROR, fmtstr, arg1)

#define DS_AT_MSG1_FATAL(fmtstr, arg1) \
          DATA_ATCOP_MSG1(MSG_LEGACY_FATAL, fmtstr, arg1)

#define DS_AT_MSG2_LOW(fmtstr, arg1, arg2) \
          DATA_ATCOP_MSG2(MSG_LEGACY_LOW, fmtstr, arg1, arg2)

#define DS_AT_MSG2_MED(fmtstr, arg1, arg2) \
          DATA_ATCOP_MSG2(MSG_LEGACY_MED, fmtstr, arg1, arg2)

#define DS_AT_MSG2_HIGH(fmtstr, arg1, arg2) \
          DATA_ATCOP_MSG2(MSG_LEGACY_HIGH, fmtstr, arg1, arg2)

#define DS_AT_MSG2_ERROR(fmtstr, arg1, arg2) \
          DATA_ATCOP_MSG2(MSG_LEGACY_ERROR, fmtstr, arg1, arg2)

#define DS_AT_MSG2_FATAL(fmtstr, arg1, arg2) \
          DATA_ATCOP_MSG2(MSG_LEGACY_FATAL, fmtstr, arg1, arg2)

#define DS_AT_MSG3_LOW(fmtstr, arg1, arg2, arg3) \
          DATA_ATCOP_MSG3(MSG_LEGACY_LOW, fmtstr, arg1, arg2, arg3)

#define DS_AT_MSG3_MED(fmtstr, arg1, arg2, arg3) \
          DATA_ATCOP_MSG3(MSG_LEGACY_MED, fmtstr, arg1, arg2, arg3)

#define DS_AT_MSG3_HIGH(fmtstr, arg1, arg2, arg3) \
          DATA_ATCOP_MSG3(MSG_LEGACY_HIGH, fmtstr, arg1, arg2, arg3)

#define DS_AT_MSG3_ERROR(fmtstr, arg1, arg2, arg3) \
          DATA_ATCOP_MSG3(MSG_LEGACY_ERROR, fmtstr, arg1, arg2, arg3)

#define DS_AT_MSG3_FATAL(fmtstr, arg1, arg2, arg3) \
          DATA_ATCOP_MSG3(MSG_LEGACY_FATAL, fmtstr, arg1, arg2, arg3)

#define DS_AT_MSG4_LOW(fmtstr, arg1, arg2, arg3, arg4) \
          DATA_ATCOP_MSG4(MSG_LEGACY_LOW, fmtstr, arg1, arg2, arg3, arg4)

#define DS_AT_MSG4_MED(fmtstr, arg1, arg2, arg3, arg4) \
          DATA_ATCOP_MSG4(MSG_LEGACY_MED, fmtstr, arg1, arg2, arg3, arg4)

#define DS_AT_MSG4_HIGH(fmtstr, arg1, arg2, arg3, arg4) \
          DATA_ATCOP_MSG4(MSG_LEGACY_HIGH, fmtstr, arg1, arg2, arg3, arg4)

#define DS_AT_MSG4_ERROR(fmtstr, arg1, arg2, arg3, arg4) \
          DATA_ATCOP_MSG4(MSG_LEGACY_ERROR, fmtstr, arg1, arg2, arg3, arg4)

#define DS_AT_MSG4_FATAL(fmtstr, arg1, arg2, arg3, arg4) \
          DATA_ATCOP_MSG4(MSG_LEGACY_FATAL, fmtstr, arg1, arg2, arg3, arg4)

#define DS_AT_MSG5_LOW(fmtstr, arg1, arg2, arg3, arg4, arg5) \
          DATA_ATCOP_MSG5(MSG_LEGACY_LOW, fmtstr, arg1, arg2, arg3, arg4, \
                          arg5)

#define DS_AT_MSG5_MED(fmtstr, arg1, arg2, arg3, arg4, arg5) \
          DATA_ATCOP_MSG5(MSG_LEGACY_MED, fmtstr, arg1, arg2, arg3, arg4, \
                          arg5)

#define DS_AT_MSG5_HIGH(fmtstr, arg1, arg2, arg3, arg4, arg5) \
          DATA_ATCOP_MSG5(MSG_LEGACY_HIGH, fmtstr, arg1, arg2, arg3, arg4, \
                          arg5)

#define DS_AT_MSG5_ERROR(fmtstr, arg1, arg2, arg3, arg4, arg5) \
          DATA_ATCOP_MSG5(MSG_LEGACY_ERROR, fmtstr, arg1, arg2, arg3, arg4, \
                          arg5)

#define DS_AT_MSG5_FATAL(fmtstr, arg1, arg2, arg3, arg4, arg5) \
          DATA_ATCOP_MSG5(MSG_LEGACY_FATAL, fmtstr, arg1, arg2, arg3, arg4, \
                          arg5)

#define DS_AT_MSG6_LOW(fmtstr, arg1, arg2, arg3, arg4, arg5, arg6) \
          DATA_ATCOP_MSG6(MSG_LEGACY_LOW, fmtstr, arg1, arg2, arg3, arg4, \
                          arg5, arg6)

#define DS_AT_MSG6_MED(fmtstr, arg1, arg2, arg3, arg4, arg5, arg6) \
          DATA_ATCOP_MSG6(MSG_LEGACY_MED, fmtstr, arg1, arg2, arg3, arg4, \
                          arg5, arg6)

#define DS_AT_MSG6_HIGH(fmtstr, arg1, arg2, arg3, arg4, arg5, arg6) \
          DATA_ATCOP_MSG6(MSG_LEGACY_HIGH, fmtstr, arg1, arg2, arg3, arg4, \
                          arg5, arg6)

#define DS_AT_MSG6_ERROR(fmtstr, arg1, arg2, arg3, arg4, arg5, arg6) \
          DATA_ATCOP_MSG6(MSG_LEGACY_ERROR, fmtstr, arg1, arg2, arg3, arg4, \
                          arg5, arg6)

#define DS_AT_MSG6_FATAL(fmtstr, arg1, arg2, arg3, arg4, arg5, arg6) \
          DATA_ATCOP_MSG6(MSG_LEGACY_FATAL, fmtstr, arg1, arg2, arg3, arg4, \
                          arg5, arg6)

#define DS_AT_MSG7_LOW(fmtstr, arg1, arg2, arg3, arg4, arg5, arg6, arg7) \
          DATA_ATCOP_MSG7(MSG_LEGACY_LOW, fmtstr, arg1, arg2, arg3, arg4, \
                          arg5, arg6, arg7)

#define DS_AT_MSG7_MED(fmtstr, arg1, arg2, arg3, arg4, arg5, arg6, arg7) \
          DATA_ATCOP_MSG7(MSG_LEGACY_MED, fmtstr, arg1, arg2, arg3, arg4, \
                          arg5, arg6, arg7)

#define DS_AT_MSG7_HIGH(fmtstr, arg1, arg2, arg3, arg4, arg5, arg6, arg7) \
          DATA_ATCOP_MSG7(MSG_LEGACY_HIGH, fmtstr, arg1, arg2, arg3, arg4, \
                          arg5, arg6, arg7)

#define DS_AT_MSG7_ERROR(fmtstr, arg1, arg2, arg3, arg4, arg5, arg6, arg7) \
          DATA_ATCOP_MSG7(MSG_LEGACY_ERROR, fmtstr, arg1, arg2, arg3, arg4, \
                          arg5, arg6, arg7)

#define DS_AT_MSG7_FATAL(fmtstr, arg1, arg2, arg3, arg4, arg5, arg6, arg7) \
          DATA_ATCOP_MSG7(MSG_LEGACY_FATAL, fmtstr, arg1, arg2, arg3, arg4, \
                          arg5, arg6, arg7)

#define DS_AT_MSG8_LOW(fmtstr, arg1, arg2, arg3, arg4, arg5, arg6, arg7, \
                       arg8) \
          DATA_ATCOP_MSG8(MSG_LEGACY_LOW, fmtstr, arg1, arg2, arg3, arg4, \
                          arg5, arg6, arg7, arg8)

#define DS_AT_MSG8_MED(fmtstr, arg1, arg2, arg3, arg4, arg5, arg6, arg7, \
                       arg8) \
          DATA_ATCOP_MSG8(MSG_LEGACY_MED, fmtstr, arg1, arg2, arg3, arg4, \
                          arg5, arg6, arg7, arg8)

#define DS_AT_MSG8_HIGH(fmtstr, arg1, arg2, arg3, arg4, arg5, arg6, arg7, \
                        arg8) \
          DATA_ATCOP_MSG8(MSG_LEGACY_HIGH, fmtstr, arg1, arg2, arg3, arg4, \
                          arg5, arg6, arg7, arg8)

#define DS_AT_MSG8_ERROR(fmtstr, arg1, arg2, arg3, arg4, arg5, arg6, arg7, \
                         arg8) \
          DATA_ATCOP_MSG8(MSG_LEGACY_ERROR, fmtstr, arg1, arg2, arg3, arg4, \
                          arg5, arg6, arg7, arg8)

#define DS_AT_MSG8_FATAL(fmtstr, arg1, arg2, arg3, arg4, arg5, arg6, arg7, \
                         arg8) \
          DATA_ATCOP_MSG8(MSG_LEGACY_FATAL, fmtstr, arg1, arg2, arg3, arg4, \
                          arg5, arg6, arg7, arg8)

#define DS_AT_MSG9_LOW(fmtstr, arg1, arg2, arg3, arg4, arg5, arg6, arg7, \
                       arg8, arg9) \
          DATA_ATCOP_MSG9(MSG_LEGACY_LOW, fmtstr, arg1, arg2, arg3, arg4, \
                          arg5, arg6, arg7, arg8, arg9)

#define DS_AT_MSG9_MED(fmtstr, arg1, arg2, arg3, arg4, arg5, arg6, arg7, \
                       arg8, arg9) \
          DATA_ATCOP_MSG9(MSG_LEGACY_MED, fmtstr, arg1, arg2, arg3, arg4, \
                          arg5, arg6, arg7, arg8, arg9)

#define DS_AT_MSG9_HIGH(fmtstr, arg1, arg2, arg3, arg4, arg5, arg6, arg7, \
                        arg8, arg9) \
          DATA_ATCOP_MSG9(MSG_LEGACY_HIGH, fmtstr, arg1, arg2, arg3, arg4, \
                          arg5, arg6, arg7, arg8, arg9)

#define DS_AT_MSG9_ERROR(fmtstr, arg1, arg2, arg3, arg4, arg5, arg6, arg7, \
                         arg8, arg9) \
          DATA_ATCOP_MSG9(MSG_LEGACY_ERROR, fmtstr, arg1, arg2, arg3, arg4, \
                          arg5, arg6, arg7, arg8, arg9)

#define DS_AT_MSG9_FATAL(fmtstr, arg1, arg2, arg3, arg4, arg5, arg6, arg7, \
                         arg8, arg9) \
          DATA_ATCOP_MSG9(MSG_LEGACY_FATAL, fmtstr, arg1, arg2, arg3, arg4, \
                          arg5, arg6, arg7, arg8, arg9)

#define DS_AT_MSG_SPRINTF_1_LOW(fmtstr, arg1) \
          DATA_ATCOP_MSG_SPRINTF_1(MSG_LEGACY_LOW, fmtstr, arg1)

#define DS_AT_MSG_SPRINTF_1_MED(fmtstr, arg1) \
          DATA_ATCOP_MSG_SPRINTF_1(MSG_LEGACY_MED, fmtstr, arg1)

#define DS_AT_MSG_SPRINTF_1_HIGH(fmtstr, arg1) \
          DATA_ATCOP_MSG_SPRINTF_1(MSG_LEGACY_HIGH, fmtstr, arg1)

#define DS_AT_MSG_SPRINTF_1_ERROR(fmtstr, arg1) \
          DATA_ATCOP_MSG_SPRINTF_1(MSG_LEGACY_ERROR, fmtstr, arg1)

#define DS_AT_MSG_SPRINTF_1_FATAL(fmtstr, arg1) \
          DATA_ATCOP_MSG_SPRINTF_1(MSG_LEGACY_FATAL, fmtstr, arg1)

#define DS_AT_MSG_SPRINTF_2_LOW(fmtstr, arg1, arg2) \
          DATA_ATCOP_MSG_SPRINTF_2(MSG_LEGACY_LOW, fmtstr, arg1, arg2)

#define DS_AT_MSG_SPRINTF_2_MED(fmtstr, arg1, arg2) \
          DATA_ATCOP_MSG_SPRINTF_2(MSG_LEGACY_MED, fmtstr, arg1, arg2)

#define DS_AT_MSG_SPRINTF_2_HIGH(fmtstr, arg1, arg2) \
          DATA_ATCOP_MSG_SPRINTF_2(MSG_LEGACY_HIGH, fmtstr, arg1, arg2)

#define DS_AT_MSG_SPRINTF_2_ERROR(fmtstr, arg1, arg2) \
          DATA_ATCOP_MSG_SPRINTF_2(MSG_LEGACY_ERROR, fmtstr, arg1, arg2)

#define DS_AT_MSG_SPRINTF_2_FATAL(fmtstr, arg1, arg2) \
          DATA_ATCOP_MSG_SPRINTF_2(MSG_LEGACY_FATAL, fmtstr, arg1, arg2)

#define DS_AT_MSG_SPRINTF_3_LOW(fmtstr, arg1, arg2, arg3) \
          DATA_ATCOP_MSG_SPRINTF_3(MSG_LEGACY_LOW, fmtstr, arg1, arg2, arg3)

#define DS_AT_MSG_SPRINTF_3_MED(fmtstr, arg1, arg2, arg3) \
          DATA_ATCOP_MSG_SPRINTF_3(MSG_LEGACY_MED, fmtstr, arg1, arg2, arg3)

#define DS_AT_MSG_SPRINTF_3_HIGH(fmtstr, arg1, arg2, arg3) \
          DATA_ATCOP_MSG_SPRINTF_3(MSG_LEGACY_HIGH, fmtstr, arg1, arg2, arg3)

#define DS_AT_MSG_SPRINTF_3_ERROR(fmtstr, arg1, arg2, arg3) \
          DATA_ATCOP_MSG_SPRINTF_3(MSG_LEGACY_ERROR, fmtstr, arg1, arg2, arg3)

#define DS_AT_MSG_SPRINTF_3_FATAL(fmtstr, arg1, arg2, arg3) \
          DATA_ATCOP_MSG_SPRINTF_3(MSG_LEGACY_FATAL, fmtstr, arg1, arg2, arg3)

#define DS_AT_MSG_SPRINTF_4_LOW(fmtstr, arg1, arg2, arg3, arg4) \
          DATA_ATCOP_MSG_SPRINTF_4(MSG_LEGACY_LOW, fmtstr, arg1, arg2, arg3, arg4)

#define DS_AT_MSG_SPRINTF_4_MED(fmtstr, arg1, arg2, arg3, arg4) \
          DATA_ATCOP_MSG_SPRINTF_4(MSG_LEGACY_MED, fmtstr, arg1, arg2, arg3, arg4)

#define DS_AT_MSG_SPRINTF_4_HIGH(fmtstr, arg1, arg2, arg3, arg4) \
          DATA_ATCOP_MSG_SPRINTF_4(MSG_LEGACY_HIGH, fmtstr, arg1, arg2, arg3, arg4)

#define DS_AT_MSG_SPRINTF_4_ERROR(fmtstr, arg1, arg2, arg3, arg4) \
          DATA_ATCOP_MSG_SPRINTF_4(MSG_LEGACY_ERROR, fmtstr, arg1, arg2, arg3, arg4)

#define DS_AT_MSG_SPRINTF_4_FATAL(fmtstr, arg1, arg2, arg3, arg4) \
          DATA_ATCOP_MSG_SPRINTF_4(MSG_LEGACY_FATAL, fmtstr, arg1, arg2, arg3, arg4)

#define DS_AT_MSG_SPRINTF_5_LOW(fmtstr, arg1, arg2, arg3, arg4, arg5) \
          DATA_ATCOP_MSG_SPRINTF_5(MSG_LEGACY_LOW, fmtstr, arg1, arg2, arg3, arg4, \
                                   arg5)

#define DS_AT_MSG_SPRINTF_5_MED(fmtstr, arg1, arg2, arg3, arg4, arg5) \
          DATA_ATCOP_MSG_SPRINTF_5(MSG_LEGACY_MED, fmtstr, arg1, arg2, arg3, arg4, \
                                   arg5)

#define DS_AT_MSG_SPRINTF_5_HIGH(fmtstr, arg1, arg2, arg3, arg4, arg5) \
          DATA_ATCOP_MSG_SPRINTF_5(MSG_LEGACY_HIGH, fmtstr, arg1, arg2, arg3, arg4, \
                                   arg5)

#define DS_AT_MSG_SPRINTF_5_ERROR(fmtstr, arg1, arg2, arg3, arg4, arg5) \
          DATA_ATCOP_MSG_SPRINTF_5(MSG_LEGACY_ERROR, fmtstr, arg1, arg2, arg3, arg4, \
                                   arg5)

#define DS_AT_MSG_SPRINTF_5_FATAL(fmtstr, arg1, arg2, arg3, arg4, arg5) \
          DATA_ATCOP_MSG_SPRINTF_5(MSG_LEGACY_FATAL, fmtstr, arg1, arg2, arg3, arg4, \
                                   arg5)

#define DS_AT_MSG_SPRINTF_6_LOW(fmtstr, arg1, arg2, arg3, arg4, arg5, arg6) \
          DATA_ATCOP_MSG_SPRINTF_6(MSG_LEGACY_LOW, fmtstr, arg1, arg2, arg3, arg4, \
                                   arg5, arg6)

#define DS_AT_MSG_SPRINTF_6_MED(fmtstr, arg1, arg2, arg3, arg4, arg5, arg6) \
          DATA_ATCOP_MSG_SPRINTF_6(MSG_LEGACY_MED, fmtstr, arg1, arg2, arg3, arg4, \
                                   arg5, arg6)

#define DS_AT_MSG_SPRINTF_6_HIGH(fmtstr, arg1, arg2, arg3, arg4, arg5, arg6) \
          DATA_ATCOP_MSG_SPRINTF_6(MSG_LEGACY_HIGH, fmtstr, arg1, arg2, arg3, arg4, \
                                   arg5, arg6)

#define DS_AT_MSG_SPRINTF_6_ERROR(fmtstr, arg1, arg2, arg3, arg4, arg5, arg6) \
          DATA_ATCOP_MSG_SPRINTF_6(MSG_LEGACY_ERROR, fmtstr, arg1, arg2, arg3, arg4, \
                                   arg5, arg6)

#define DS_AT_MSG_SPRINTF_6_FATAL(fmtstr, arg1, arg2, arg3, arg4, arg5, arg6) \
          DATA_ATCOP_MSG_SPRINTF_6(MSG_LEGACY_FATAL, fmtstr, arg1, arg2, arg3, arg4, \
                                   arg5, arg6)

#define DS_AT_MSG_SPRINTF_7_LOW(fmtstr, arg1, arg2, arg3, arg4, arg5, arg6, arg7) \
          DATA_ATCOP_MSG_SPRINTF_7(MSG_LEGACY_LOW, fmtstr, arg1, arg2, arg3, arg4, \
                                   arg5, arg6, arg7)

#define DS_AT_MSG_SPRINTF_7_MED(fmtstr, arg1, arg2, arg3, arg4, arg5, arg6, arg7) \
          DATA_ATCOP_MSG_SPRINTF_7(MSG_LEGACY_MED, fmtstr, arg1, arg2, arg3, arg4, \
                                   arg5, arg6, arg7)

#define DS_AT_MSG_SPRINTF_7_HIGH(fmtstr, arg1, arg2, arg3, arg4, arg5, arg6, arg7) \
          DATA_ATCOP_MSG_SPRINTF_7(MSG_LEGACY_HIGH, fmtstr, arg1, arg2, arg3, arg4, \
                                   arg5, arg6, arg7)

#define DS_AT_MSG_SPRINTF_7_ERROR(fmtstr, arg1, arg2, arg3, arg4, arg5, arg6, arg7) \
          DATA_ATCOP_MSG_SPRINTF_7(MSG_LEGACY_ERROR, fmtstr, arg1, arg2, arg3, arg4, \
                                   arg5, arg6, arg7)

#define DS_AT_MSG_SPRINTF_7_FATAL(fmtstr, arg1, arg2, arg3, arg4, arg5, arg6, arg7) \
          DATA_ATCOP_MSG_SPRINTF_7(MSG_LEGACY_FATAL, fmtstr, arg1, arg2, arg3, arg4, \
                                   arg5, arg6, arg7)

#define DS_AT_MSG_SPRINTF_8_LOW(fmtstr, arg1, arg2, arg3, arg4, arg5, arg6, arg7, \
	                              arg8) \
          DATA_ATCOP_MSG_SPRINTF_8(MSG_LEGACY_LOW, fmtstr, arg1, arg2, arg3, arg4, \
                                   arg5, arg6, arg7, arg8)

#define DS_AT_MSG_SPRINTF_8_MED(fmtstr, arg1, arg2, arg3, arg4, arg5, arg6, arg7, \
	                              arg8) \
          DATA_ATCOP_MSG_SPRINTF_8(MSG_LEGACY_MED, fmtstr, arg1, arg2, arg3, arg4, \
                                   arg5, arg6, arg7, arg8)

#define DS_AT_MSG_SPRINTF_8_HIGH(fmtstr, arg1, arg2, arg3, arg4, arg5, arg6, arg7, \
	                               arg8) \
          DATA_ATCOP_MSG_SPRINTF_8(MSG_LEGACY_HIGH, fmtstr, arg1, arg2, arg3, arg4, \
                                   arg5, arg6, arg7, arg8)

#define DS_AT_MSG_SPRINTF_8_ERROR(fmtstr, arg1, arg2, arg3, arg4, arg5, arg6, arg7, \
	                                arg8) \
          DATA_ATCOP_MSG_SPRINTF_8(MSG_LEGACY_ERROR, fmtstr, arg1, arg2, arg3, arg4, \
                                   arg5, arg6, arg7, arg8)

#define DS_AT_MSG_SPRINTF_8_FATAL(fmtstr, arg1, arg2, arg3, arg4, arg5, arg6, arg7, \
	                                arg8) \
          DATA_ATCOP_MSG_SPRINTF_8(MSG_LEGACY_FATAL, fmtstr, arg1, arg2, arg3, arg4, \
                                   arg5, arg6, arg7, arg8)

/*===========================================================================
                 SIO Data Preprocessor Enumeration Types
===========================================================================*/

/*---------------------------------------------------------------------------
  SIO data preprocessor mode enumeration type.
---------------------------------------------------------------------------*/
typedef enum
{
  DSATI_AT_CMD_MODE,               /* AT command line entry mode      */
  DSATI_ABORT_CMD_MODE,            /* Abort AT command mode           */
  DSATI_SMS_MSG_ENTRY_MODE,        /* ETSI SMS message entry mode     */
  DSATI_SMS_MSG_END_MODE,          /* ETSI SMS message end mode       */
  DSATI_MD5_MSG_ENTRY_MODE,        /* ETSI MD5 message entry mode     */
  DSATI_MD5_MSG_END_MODE,          /* ETSI MD5 message end mode       */
#ifdef FEATURE_DSAT_MDM_FUSION
  DSATI_AT_PROXY_CMD_MODE,         /* FORWARD COMMAND mode  */
#endif /* FEATURE_DSAT_MDM_FUSION */
DSATI_INVALID_MODE
} dsati_preproc_mode_enum_type;

/*--------------------------------------------------------------------------
    The following type represents the states for the command line   
    parser:                                                                
---------------------------------------------------------------------------*/
typedef enum       
{                                                                      
  DSAT_CMD_PREP_STATE_HUNT,     /*  Initial:  looking for a or A                      */
  DSAT_CMD_PREP_STATE_FOUND_A,  /*  Found A, looking for t, T or /                    */
  DSAT_CMD_PREP_STATE_FOUND_AT, /*  Found AT: filling buffer and processing backspace */
  DSAT_CMD_PREP_STATE_ERROR     /*  Error!  loop until end of line                    */
} dsat_cmd_prep_state_enum_type;

/*===========================================================================
                 SIO Info Data Structure
===========================================================================*/

typedef struct dsat_sio_info_s
{
  byte                           cmd_line_buffer[MAX_LINE_SIZE];
  byte                           prev_line_buffer[MAX_LINE_SIZE];
  dsat_cmd_prep_state_enum_type  at_cmd_prep_state;
  byte                          *build_cmd_ptr;
  byte                          *prev_cmd_ptr;
  boolean                        block_indications_flag;
  dsat_num_item_type             qcsimapp_val;  /* current subscription */
  dsat_num_item_type             qcsimapp_val_updated;  /* updated subscription in events */
  boolean                        qcsimapp_val_user_flag; /* updated when user set */
  q_type                         urc_msg_q; /* URC queue */
} dsat_sio_info_s_type;

/*===========================================================================
                 AT Command Processor Data Structures
===========================================================================*/

/* The following definitions used in parsing of AT commands. */

typedef enum
{
  BASIC_SYNTAX_AT_CMD = 0,
  EXTENDED_SYNTAX_AT_CMD,
  UNKNOWN_SYNTAX_AT_CMD
} cmd_syntax_category_enum_type;

typedef enum
{                       
  ATOI_OK,              /*  conversion complete             */
  ATOI_NO_ARG,          /*  nothing to convert              */
  ATOI_OUT_OF_RANGE     /*  character or value out of range */
} atoi_enum_type;

typedef enum
{                       
  PARAM_MISS,           /*  Not found in values list        */
  PARAM_HIT,            /*  Found in values list            */
  PARAM_NULL            /*  No parameter specified          */
} param_srch_enum_type;

typedef enum
{
  ACTION_QUOTE_BOTH_OK,    /* don't care about the quote 
                              If the argument is quoted, trip 
                              out the quotes.
                              If the argument is not quoted,
                              pass it through */
  ACTION_QUOTE_YES_QUOTE,  /* the argument must be quoted */
  ACTION_QUOTE_NO_QUOTE    /* the argument must not be quoted */
} action_quote_e_type;

typedef struct {
  byte          *num;           /* points to dial cmd token's arg[0]    */
  uint16        len;            /* dial string length                   */
  uint16        attrib;         /* attributes                           */
} dsati_dial_str_type;

/*-------------------------------------------------------------------------
   Operating mode for selection of different AT command sets from
   command table.
-------------------------------------------------------------------------*/

typedef enum
{
  ETSI_CMD_MODE,
  CDMA_CMD_MODE,
  NUM_OPER_CMD_MODES
} dsati_operating_cmd_mode_type;

typedef enum
{
  DSAT_WS46_READ,      /* WS46 Read command */
  DSAT_WS46_NONE
}dsatact_ws46_res_e_type;

#define MAX_PORT  1
#define PORT_ZERO 0
/*
*Every command pending come in 5 interval
*/
typedef enum
{
  DSAT_PENDING_FALSE  = 0, 
  DSAT_PENDING_TRUE   = 1,

    /*dsat_bandpref_cmd_pending  and dsat_prefmode_cmd_pending;*/
  DSAT_PENDING_PREFMODE_NONE             = 20,
  DSAT_PENDING_PREFMODE_SYSMODE_WRITE    = 21,
  DSAT_PENDING_PREFMODE_SYSMODE_READ     = 22,
  DSAT_PENDING_PREFMODE_BANDPREF_VERIFY  = 23,
  DSAT_PENDING_PREFMODE_BANDPREF_WRITE   = 24,
  DSAT_PENDING_PREFMODE_BANDPREF_READ    = 25,
  DSAT_PENDING_PREFMODE_MAX              = 29,
  /*dsatvend_qcagc_pres_cmd*/
  DSAT_PENDING_QCAGC_GET_AGC       = 30,
  DSAT_PENDING_QCAGC_GET_SYNTH     = 31,
  DSAT_PENDING_QCAGC_SET_CHAN      = 32,
  DSAT_PENDING_QCAGC_SET_LNA       = 33,
  DSAT_PENDING_QCAGC_SET_MODE      = 34,
  DSAT_PENDING_QCAGC_SET_SEC_CHAIN = 35,
  DSAT_PENDING_QCAGC_NONE,
    /*dsatvend_qcallup_pres_cmd */
  DSAT_PENDING_QCALLUP_GET_SYNTH    = 40,
  DSAT_PENDING_QCALLUP_SET_CHAN     = 41,
  DSAT_PENDING_QCALLUP_SET_CW_WAVE  = 42,
  DSAT_PENDING_QCALLUP_SET_MODE     = 43,
  DSAT_PENDING_QCALLUP_SET_PA_RANGE = 44,
  DSAT_PENDING_QCALLUP_SET_PDM      = 45,
  DSAT_PENDING_QCALLUP_SET_TX       = 46,
  DSAT_PENDING_QCALLUP_NONE         = 49,
/*dsat_cpol_res_type*/
  DSAT_PENDING_CPOL_READ  = 50,
  DSAT_PENDING_CPOL_WRITE = 51,
  DSAT_PENDING_CPOL_TEST  = 52,
  DSAT_PENDING_CPOL_NONE  = 53,

  DSAT_PENDING_CUAD_READ  = 54,
  DSAT_PENDING_CUAD_NONE  = 55,
  
    /*dsat707_pending_mmgsdi_command*/
  DSAT_PENDING_CPIN_MMGSDI_CMD_NONE             = 60,
  DSAT_PENDING_CPIN_MMGSDI_QUERY_CMD            = 61,
  DSAT_PENDING_CPIN_MMGSDI_EXECUTE_CMD          = 62,
  DSAT_PENDING_CPIN_MMGSDI_VERIFY_CMD           = 63,
  DSAT_PENDING_CLCK_MMGSDI_FAC_LOCK_UNLOCK_CMD  = 64,
  DSAT_PENDING_CLCK_MMGSDI_FAC_QUERY_CMD        = 65,
  DSAT_PENDING_CPWD_MMGSDI_PWD_CHANGE           = 66,
  DSAT_PENDING_CPIN_MMGSDI_CMD_MAX              = 69,

   /*dsat_mmgsdi_auth_cmd_pending - To Do*/
  DSAT_PENDING_MMGSDI_AUTH_NONE            = 70,
  DSAT_PENDING_MMGSDI_AUTH_SSD_UPDATE      = 71,
  DSAT_PENDING_MMGSDI_AUTH_SSD_BS_CHAL     = 72,
  DSAT_PENDING_MMGSDI_AUTH_SSD_CNF         = 73,
  DSAT_PENDING_MMGSDI_AUTH_KEYGEN_VPM      = 74,
  DSAT_PENDING_MMGSDI_AUTH_CAVE            = 75,
  DSAT_PENDING_MMGSDI_AUTH_MD5             = 76,
  DSAT_PENDING_MMGSDI_AUTH_GSN             = 77,
  DSAT_PENDING_MMGSDI_AUTH_UIMAUTH         = 78,
  DSAT_PENDING_MMGSDI_AUTH_MAX             = 79,

  DSAT_PENDING_SYSCONFIG_NONE              = 81,
  DSAT_PENDING_SYSCONFIG_READ              = 82,
  DSAT_PENDING_SYSCONFIG_WRITE             = 83,
  DSAT_PENDING_SYSCONFIG_MAX               = 84,

  DSAT_PENDING_CEAP_PROCESS  = 90,
  DSAT_PENDING_CEAP_NONE  = 91,

  DSAT_PENDING_QCDRX_SET  = 96,
  DSAT_PENDING_QCDRX_GET  = 97,
  DSAT_PENDING_QCDRX_NONE = 98,

  DSAT_PENDING_CECALL_WRITE                = 100,
  DSAT_PENDING_CECALL_READ                 = 101,
  DSAT_PENDING_CECALL_NONE                 = 102,

  DSAT_PENDING_MRU_NONE                    = 110,
  DSAT_PENDING_MRU_WRITE                   = 111, /* Online -> LPM -> MRU write */
  DSAT_PENDING_MRU_READ                    = 112, /* MRU read multiple times -> Result */
  DSAT_PENDING_MRU_CLEAR                   = 113, /* Online -> LPM -> MRU clear */
  DSAT_PENDING_MRU_RESULT                  = 114, /* MRU update -> Online -> Result */
  DSAT_PENDING_QCNSP_READ                  = 121,
  DSAT_PENDING_QCNSP_GET_PRI_RAT           = 122,
  DSAT_PENDING_QCNSP_WRITE                 = 123,
  DSAT_PENDING_QCNSP_NONE                  = 124,
    /* dsat_cfun_res_type */
  DSAT_PENDING_CFUN_READ            = 130,     /* CFUN Read command */
  DSAT_PENDING_CFUN_WRITE_CB        = 131,     /* CFUN write command  with async call */
  DSAT_PENDING_CFUN_WRITE           = 132,     /* CFUN write command */
  DSAT_PENDING_CFUN_WRITE_MIN       = 133,     /* Min Functionality */
  DSAT_PENDING_CFUN_WRITE_FULL      = 134,    /* Normal functionality */
  DSAT_PENDING_CFUN_WRITE_RFOFF     = 135,    /* TX & Rx RF off */
  DSAT_PENDING_CFUN_WRITE_FTM       = 136,    /* Factory Test Mode */
  DSAT_PENDING_CFUN_WRITE_RESET     = 137,    /* Reset UE */
  DSAT_PENDING_CFUN_WRITE_OFFLINE   = 138,    /* Offline mode */
  DSAT_PENDING_CFUN_WRITE_CARD_PUP  = 139,    /* Card Power Down */
  DSAT_PENDING_CFUN_WRITE_CARD_DOWN = 140,    /* Card Power Up */
  DSAT_PENDING_CFUN_NONE            = 141,
  /*VoIP Pending States*/
  DSAT_PENDING_VOIP_READ            = 150,
  DSAT_PENDING_VOIP_WRITE           = 151,
  DSAT_PENDING_VOIP_NONE            = 152,

  DSAT_PENDING_RESET_WRITE                 = 160,
  DSAT_PENDING_RESET_NONE                  = 161,

  DSAT_PENDING_CMD_NONE             = 170
}dsat_cmd_pending_enum_type;

typedef enum
{
  DSAT_MS_FIRST_SUBS   = 0,
  DSAT_MS_SECOND_SUBS  = 1,
  DSAT_MS_TRIPLE_SUBS  = 2,
  DSAT_MS_MAX_SUBS     = 3,
}dsat_memory_subs_type;

typedef enum
{
  DSAT_ALL_ATCOP_STRUCT_TABLE       = 1,
  DSAT_TABLE_BASIC                  = 2,
  DSAT_TABLE_BASIC_ACTION           = 3,
  DSAT_TABLE_SREG                   = 4,
  DSAT_TABLE_EXT                    = 5,
  DSAT_TABLE_VEND_EXT_SS            = 6,
  DSAT_TABLE_VEND_EXT_MS            = 7,
  DSAT_TABLE_ETSI_EXT_ACT_SS        = 8,
  DSAT_TABLE_ETSI_EXT_ACT_MS        = 9,
  DSAT_TABLE_ETSI_EXT_SS            = 10,
  DSAT_TABLE_ETSI_EXT_MS            = 11,
  DSAT_TABLE_707_PARA_EXT           = 12,
  DSAT_TABLE_707_ACTION_EXT         = 13,
  DSAT_TABLE_707_FAX                = 14,
  DSAT_TABLE_707_HDR                = 15,
  DSAT_TABLE_707_SMS                = 16,
  DSAT_TABLE_707_VENDOR_PARA        = 17,
  DSAT_TABLE_707_VENDOR_ACTION      = 18,
  /*CM interface variables*/
  DSAT_SS_PH_SS_VALS                = 100,
  DSAT_SS_PH_MS_VALS                = 101,

  DSAT_CALL_SS_VALS                 = 110,
  DSAT_CALL_DA_VALS                 = 111,
  DSAT_CALL_MS_VALS                 = 112,

  DSAT_SUPS_SS_VALS                 = 120,
  DSAT_SUPS_MS_VALS                 = 121,

  DSAT_MMGSDI_SS_VALS               = 130,
  DSAT_MMGSDI_MS_VALS               = 131,
  DSAT_MMGSDI_MS_SLOT_VALS          = 132,

  DSAT_PBM_SS_VALS                  = 140,
  DSAT_PBM_MS_VALS                  = 141,

  DSAT_COMMON_SS_VALS               = 150,
  DSAT_MS_MD_VALS                   = 160,

  DSAT_STATE_CM_VARIABLE            = 100,
  DSAT_PENDING_VARIABLE             = 200,
  DSAT_MEMORY_OPERATION_MAX         = 1000,
}dsat_mem_type;

typedef enum debug_results_type
{
  DSAT_RESULT_FAILURE       = 0,
  DSAT_RESULT_SUCCESS       = 1,
  DSAT_FILE_NOT_FOUND       = 2,
  DSAT_FILE_WRITE_FAILED    = 4,
  DSAT_FILE_CREATE_FAILED   = 8,
  DSAT_FILE_OPEN_FAILED     = 16,
  DSAT_FILE_READ_FAILED     = 32,
  
}dsat_debug_result_type;

typedef struct debug_result_code
{
  dsat_debug_result_type  result_code;
  char                    result_string[100];
}dsat_debug_result_code;

typedef struct dsati_state_struct
{
  dsat_num_item_type            cmd_idx;
  dsat_num_item_type            port_id;
  void*                         user_data;
  dsat_cmd_pending_enum_type    cmd_pending[2]; /* For DsDs apps */ 
  void                         *cmd_mem_ptr;
} dsat_global_state;

extern dsat_global_state dsat_pending_state[MAX_PORT];

#define CHECK_PENDING(cmd_id,apps_id,pending)\
     dsat_is_pending_cmd_state(cmd_id,apps_id,pending)

#define CHECK_NOT_PENDING(cmd_id,apps_id,pending)\
     dsat_is_not_pending_cmd_state(cmd_id,apps_id,pending)

#define SET_PENDING(cmd_id ,apps_id, pending) \
     dsat_set_pending_cmd_state(cmd_id,apps_id,pending);
/*---------------------------------------------------------------------------
  The following type definition defines a pointer to an unrecognized
  command handler function called when the AT commmand processor does
  not recognize the command.

  cmd_line_ptr - Pointer to a NULL terminated command line with the "AT"
                 prefix removed.
---------------------------------------------------------------------------*/
typedef dsat_result_enum_type (*dsati_unrec_cmd_handler_ptr_type)
(
  byte* cmd_line_ptr
);


/*---------------------------------------------------------------------------
   The data type below is used to define the default value and the lower and 
   upper bounds of the arguments for AT commands with numeric arguments.         
---------------------------------------------------------------------------*/
typedef struct
{
  dsat_num_item_type default_v;   /* The default value for the argument*/
  dsat_num_item_type lower; /* Lower limit of the allowed range. */
  dsat_num_item_type upper; /* Upper limit of the allowed range. */
} dflm_type;
  

/*--------------------------------------------------------------------------
   The data type below is used to define the default value and the allowed 
   range of values of the arguments, for AT commands of type LIST.               

    default_v   - Default value of the argument.
    list_v      - A pointer to and array of 8-byte strings that represent 
                  all of the valid values that the argument can assume. The 
                  ATCoP performs a string match with the elements in this 
                  array order to determine whether or not an argument's 
                  value is within valid range.
    list_t      - Pointer to a string that is returned word for word in 
                  response to a test command. 
--------------------------------------------------------------------------*/ 
typedef struct
{
  dsat_num_item_type default_v;            
  const dsat_string_item_type (*list_v)[8];
  const dsat_string_item_type *list_t;     
} def_list_type;

/*--------------------------------------------------------------------------
   The data type below is used to define the default value and the allowed 
   range of values of the mixed arguments, for AT commands with attrib
   MIXED_PARAM .
--------------------------------------------------------------------------*/ 

/* mixed_def_u_type is used in mixed_def_s_type 
   The corresponding fields in the union takes effect
   depending on attrib in mixed_def_s_type */
typedef union
{
  const def_list_type def_list;   /* when attrib contains CONFIG and 
                                     LIST */
  const dflm_type     dflm;       /* when attrib contains CONFIG but no
                                     LIST */
  const word          string_len; /* when attrib is STRING, this is the 
                                     max length of the string val, 
                                     excluding trailing null terminator
                                  */
} mixed_def_u_type;

typedef struct
{
  const uint32 attrib;  /* QUOTE:  the argument must be quoted 
                           STRING: def is string_len, write command 
                                   writes the arument to the val */
  const mixed_def_u_type * def;
} mixed_def_s_type;

/*---------------------------------------------------------------------------
   The following data type is the data structure for a command table entry.      
   
   name        -  The name of the AT cmd, including the leading "+", "$" 
                  or "$" and the terminating NULL.
   attrib      -  A 32-bit mask that specifies one or more command 
                  attributes.       
   special     -  The "special processing code," if any, or SPECIAL_NONE. 
                  The special processing codes are defined in this file. 
                  A unique special processing code must be assigned to 
                  each AT cmd that requires some special processing. 
   compound    -  The number of arguments, for numeric commands, or the 
                  maximum string length (not including the terminating NULL), 
                  for string commands.
   val_ptr     -  Val pointers moved into strcuture and mapped via get/set API.
   def_lim_ptr -  Defines the default value and the allowed range for the 
                  arguments.
   proc_func   -  Pointer to function to be called to process the command.
   abort_func  -  Pointer to function to be called to abort the command.
                  If the pointer is NULL, command is not abortable.
---------------------------------------------------------------------------*/
typedef struct dsati_cmd_struct
{
  char *name;
  uint32 attrib;
  byte special;
  byte compound;
  uint16 cmd_id;
  const void *def_lim_ptr;
} dsati_cmd_type;

typedef struct dsati_cmd_ex_struct
{
  uint16 cmd_id;
  dsat_result_enum_type (*proc_func)( dsat_mode_enum_type,
                                      const struct dsati_cmd_struct*,
                                      const tokens_struct_type*,
                                      dsm_item_type* );
} dsati_cmd_ex_type;

typedef struct
{
  uint16 cmd_id;
  boolean (*abort_func)( const struct dsati_cmd_struct* );
} dsati_cmd_abort_type;

typedef struct dsat_qcsimapp_table_s
{
  boolean                apps_id_mapped[DSAT_APPS_ID_MAX];
  sys_modem_as_id_e_type subs_id;
} dsat_qcsimapp_table_s_type;

typedef struct dsat_qcsimapp_info_s
{
  mmgsdi_app_enum_type     app_type;
  mmgsdi_slot_id_enum_type slot_id;
  mmgsdi_session_id_type   session_id;
  boolean                  active;
  sys_modem_as_id_e_type   subs_id;
} dsat_qcsimapp_info_s_type;

typedef struct dsat_subs_info_s
{
  boolean              is_default_data;
  boolean              is_sglte_sub;
  sys_subs_feature_t   sub_feature_mode;
} dsat_subs_info_s_type;

typedef struct dsat_seqnum_s
{
  cm_call_id_type call_id;
  cm_call_type_e_type  call_type;
}dsat_seqnum_s_type;

extern dsat_qcsimapp_table_s_type dsat_qcsimapp_table[];
extern dsat_qcsimapp_info_s_type  dsat_qcsimapp_info[];
extern dsat_subs_info_s_type      dsat_subs_info[];

/*--------------------------------------------------------------------------
   The data types below is used to give information about value pointer
--------------------------------------------------------------------------*/
typedef struct
{
  dsat_num_item_type     index;
  dsat_num_item_type     size;
  void *                 val_ptr;
  value_type_enum_type   val_type;
   
} dsati_value_info_struct_type;

/*--------------------------------------------------------------------------
   The data types below is used to define command table structure.

    table_ptr   - Pointer to an array of command table entries.
    table_size  - Pointer to number of command table entries in array.

   When multiple operating modes are supported, the AT command table
   will be two dimensional, with rows representing AT command categories
   and columns representing operating command modes.
--------------------------------------------------------------------------*/
typedef struct
{
  const dsati_cmd_type *table_ptr;
  const unsigned int *table_size;
} dsati_at_cmd_table_entry_type;

typedef struct
{
  const dsati_cmd_ex_type *table_ptr;
} dsati_at_cmd_table_ex_entry_type;

typedef dsati_at_cmd_table_entry_type*
                                dsati_at_cmd_table_type[NUM_AT_CMD_CATEGORIES];

typedef dsati_at_cmd_table_ex_entry_type*
                                dsati_at_cmd_table_ex_type[NUM_AT_CMD_CATEGORIES];



/*--------------------------------------------------------------------------
   The data types below are used to define the asynchronous event handler
   table entry structure.

    event            - 3G Dsmgr sub-task command generating asynchronous
                       event
    event_handler[ ] - table of function pointers to handle event for
                       each mode

   When multiple operating modes are supported, the asynchronous event
   handler tables will contain more than one row, with each row
   representing one of the operating command modes.
--------------------------------------------------------------------------*/
typedef dsat_result_enum_type (*dsati_async_event_handler_type)
                                                   ( ds_cmd_type* );

typedef struct
{
  const ds_cmd_enum_type event;
  const dsati_async_event_handler_type event_handler;
} dsati_async_event_table_entry_type;

typedef struct
{
  const dsati_async_event_table_entry_type *base_ptr;
  const size_t                              size;
} dsati_async_event_table_type;

/*---------------------------------------------------------------------------
     Structure for command val pointers. Used in function dsat_srch_list.    
---------------------------------------------------------------------------*/
typedef struct 
{
  dsat_num_item_type dsat_amp_c_val;
  dsat_num_item_type dsat_amp_d_val;
  dsat_num_item_type dsat_amp_f_val;
  dsat_num_item_type dsat_amp_e_val; /* setting response time during connection */
  dsat_num_item_type dsat_amp_s_val; /* DSR control setting */
  dsat_num_item_type dsat_amp_w_val; /* store current setting value(s) in non-volatile memory */
#ifdef FEATURE_ATCOP_ECHO_CTL_MULTI_PORTS
  /*keep value for dual modem ports separately*/
  dsat_num_item_type dsat_e_val_multi_port[DSAT_MODEM_PORT_MAX];
#else 
  dsat_num_item_type dsat_e_val;
#endif /*FEATURE_ATCOP_ECHO_CTL_MULTI_PORTS*/
  dsat_num_item_type dsat_l_val;
  dsat_num_item_type dsat_m_val;
  dsat_num_item_type dsat_o_val;
  dsat_num_item_type dsat_q_val;
  dsat_num_item_type dsat_v_val;
  dsat_num_item_type dsat_x_val;
  dsat_num_item_type dsat_z_val;
  dsat_num_item_type dsat_slash_q_val; /* flow control setting */
  dsat_num_item_type dsat_slash_s_val; /* display of setting value */
  dsat_num_item_type dsat_slash_v_val; /* setting response code during connection */
}dsat_basic_table_ss_val_struct_type;

typedef struct
{
  dsat_num_item_type dsat_o_val;
  
}dsat_basic_action_table_ss_val_struct_type;
    
typedef struct
{
  dsat_num_item_type dsat_s0_val;
  dsat_num_item_type dsat_s2_val; /*setting escape character */  
  dsat_num_item_type dsat_s10_val;
  dsat_num_item_type dsat_s11_val;
  dsat_num_item_type dsat_s3_val;
  dsat_num_item_type dsat_s4_val;
  dsat_num_item_type dsat_s5_val;
  dsat_num_item_type dsat_s6_val;
  dsat_num_item_type dsat_s7_val;
  dsat_num_item_type dsat_s8_val;
  dsat_num_item_type dsat_s9_val;
  dsat_num_item_type dsat_s30_val; /* setting inactive timer value (min.)*/
  dsat_num_item_type dsat_s103_val; /* assigning an identifier in incoming-call sub address */
  dsat_num_item_type dsat_s104_val; /* assigning an identifier in outgoing-call sub address */
}dsat_basic_sreg_table_ss_val_struct_type;

typedef struct
{
  dsat_num_item_type dsat_fclass_val;
  dsat_num_item_type dsat_icf_val[2];
  dsat_num_item_type dsat_ifc_val[2];
  dsat_num_item_type dsat_ipr_val;
  dsat_num_item_type dsat_dr_val;
  dsat_num_item_type dsat_ds_val[4];
  dsat_num_item_type dsat_cmee_val;
#if defined(FEATURE_DSAT_ETSI_MODE)
  dsat_num_item_type dsat_ws46_val;
#endif /*defined(FEATURE_DSAT_ETSI_MODE)*/
#ifdef FEATURE_DSAT_TEST_32ARG 
  dsat_num_item_type dsat_tst32_val[2];
#endif /* FEATURE_DSAT_TEST_32ARG*/
  dsat_mixed_param_val_type dsat_cfun_val[2];
}dsat_basic_ext_table_ss_val_struct_type;

typedef struct
{
#ifdef FEATURE_DSAT_ETSI_MODE
  dsat_num_item_type dsat_qcsimstat_val;
#if defined(FEATURE_ETSI_PBM) || defined(FEATURE_DSAT_CDMA_PBM)
  dsat_num_item_type dsat_qcpbmpref_val;
#endif /*defined(FEATURE_ETSI_PBM) || defined(FEATURE_DSAT_CDMA_PBM)*/
#endif /* FEATURE_DSAT_ETSI_MODE*/
  dsat_num_item_type     dsat_qcdmr_val;
  dsat_string_item_type  dsat_qcdnspri_val[MAX_DNS_SIZE];
  dsat_num_item_type     dsat_qcter_val;
  dsat_num_item_type     dsat_qcsimapp_val;
  dsat_mixed_param_val_type dsat_qcpdpp_val[4];
  dsat_num_item_type dsat_qcpdplt_val;
#ifdef FEATURE_DATA_TE_MT_PDP
dsat_num_item_type dsat_qcgansm_val; /* Defaulting to NONE */
#endif /*FEATURE_DATA_TE_MT_PDP*/
#ifdef FEATURE_DATA_UCSD_SCUDIF_TEST
  dsat_num_item_type dsat_qcscftest_val[2];
#endif /*  FEATURE_DATA_UCSD_SCUDIF_TEST */
#ifdef FEATURE_TTY
  dsat_num_item_type dsat_qcctm_val;
#endif/* FEATURE_TTY */
#ifdef FEATURE_HDR
  dsat_num_item_type dsat_prefmode_val;
#endif /* FEATURE_HDR*/
#ifdef FEATURE_DSAT_ETSI_MODE 
  dsat_num_item_type dsat_dsci_val;
#endif /*FEATURE_DSAT_ETSI_MODE*/
#ifdef FEATURE_ECALL_APP 
  dsat_num_item_type dsat_ecall_val[3];
#endif /*FEATURE_ECALL_APP*/
#ifdef FEATURE_DSAT_ETSI_DATA
  dsat_num_item_type dsat_qcdefprof_val[3];
#endif /*FEATURE_DSAT_ETSI_DATA */
#ifdef FEATURE_DSAT_LTE
  dsat_num_item_type dsat_qcdrx_val;
#endif/* FEATURE_DSAT_LTE*/
  dsat_num_item_type dsat_mode_val;
  dsat_num_item_type dsat_qcrcind_val;
  dsat_num_item_type dsat_qcnsp_val[3];
}dsat_basic_vend_table_ss_val_struct_type;

/*Multi stack command variable*/

typedef struct
{
#ifdef FEATURE_DSAT_ETSI_MODE
#ifdef FEATURE_DSAT_DEV_CMDS
  dsat_num_item_type dsat_qccreg_val;
  dsat_num_item_type dsat_qccnti_val;
#endif /* FEATURE_DSAT_DEV_CMDS */
#endif /* FEATURE_DSAT_ETSI_MODE*/
}dsat_basic_vend_table_ms_val_struct_type;

typedef struct
{
  dsat_num_item_type        ref_count;
#ifdef FEATURE_ETSI_SMS
  dsat_num_item_type dsat_cnmi_val[5];
#endif /*FEATURE_ETSI_SMS*/
}dsat_basic_etsi_table_common_val;

typedef struct
{
  dsat_num_item_type dsat_cmec_val[4];
  dsat_num_item_type dsat_cmer_val[5];
#ifdef FEATURE_DSAT_ETSI_DATA
  dsat_num_item_type dsat_cgact_val;
#endif /* FEATURE_DSAT_ETSI_DATA*/
#ifdef FEATURE_ETSI_PBM
  dsat_num_item_type dsat_cpbs_val;
#endif /* FEATURE_ETSI_PBM*/
#ifdef FEATURE_DATA_GCSD_FAX
  dsat_num_item_type dsat_fts_val;
  dsat_num_item_type dsat_frs_val;
  dsat_num_item_type dsat_fth_val;
  dsat_num_item_type dsat_frh_val;
  dsat_num_item_type dsat_ftm_val;
  dsat_num_item_type dsat_frm_val;
#endif/*FEATURE_DATA_GCSD_FAX*/
  dsat_num_item_type dsat_ccug_val[3];
  dsat_mixed_param_val_type dsat_cusd_val[3];
  dsat_num_item_type dsat_caoc_val; 
  dsat_num_item_type dsat_ctzr_val;
  dsat_num_item_type dsat_ctzu_val;
  dsat_num_item_type dsat_clip_val;
  dsat_num_item_type dsat_colp_val;
  dsat_num_item_type dsat_cdip_val;
  dsat_num_item_type dsat_clir_val;
#ifdef FEATURE_DATA_TE_MT_PDP
  dsat_mixed_param_val_type dsat_cgans_val[2];
#endif /* FEATURE_DATA_TE_MT_PDP*/

#ifdef FEATURE_WCDMA_L1_HS_CQI_STAT
  dsat_num_item_type dsat_cqi_val;
#endif /*FEATURE_WCDMA_L1_HS_CQI_STAT*/
  dsat_num_item_type dsat_csdf_val;
}dsat_etsi_ext_act_table_ss_val_struct_type;

/*
 *
 *  Multi stack command variable
 *
 */

typedef struct
{
#ifdef FEATURE_DATA_ETSI_PIN 
  dsat_mixed_param_val_type dsat_cpin_val[2];
#endif/*FEATURE_DATA_ETSI_PIN*/
  dsat_num_item_type dsat_cind_val[MAX_CIND_INDICATORS];
#ifdef FEATURE_DSAT_ETSI_DATA
  dsat_num_item_type dsat_cgatt_val;
#endif /* FEATURE_DSAT_ETSI_DATA*/
#ifdef FEATURE_ETSI_SMS
  dsat_num_item_type dsat_cpms_val[3];
  dsat_num_item_type dsat_cmms_val;
#endif /* FEATURE_ETSI_SMS*/
  dsat_mixed_param_val_type dsat_cops_val[6];
  dsat_num_item_type dsat_ccwa_val;
#ifdef FEATURE_MMGSDI
#ifdef FEATURE_DSAT_LTE
  dsat_mixed_param_val_type dsat_cpol_val[7];
#else
  dsat_mixed_param_val_type dsat_cpol_val[6];
#endif /*FEATURE_DSAT_LTE*/
  dsat_num_item_type dsat_cpls_val;
#endif/* FEATURE_MMGSDI*/
  dsat_num_item_type dsat_cen_val;
}dsat_etsi_ext_act_table_ms_val_struct_type;

typedef struct
{
#if defined(FEATURE_DATA_GCSD) || defined(FEATURE_DATA_WCDMA_CS)
  dsat_num_item_type dsat_cbst_val[3];
  dsat_num_item_type dsat_crlp_val[MAX_CRLP_VERSIONS][MAX_CRLP_PARAMS];
  dsat_num_item_type dsat_cv120_val[DSAT_CV120_MAX_POS];
  dsat_num_item_type dsat_chsn_val[4];
#endif /* defined(FEATURE_DATA_GCSD) || defined(FEATURE_DATA_WCDMA_CS) */
  dsat_num_item_type dsat_cssn_val[2];
  dsat_num_item_type dsat_cscs_val;
  dsat_num_item_type dsat_csta_val;
  dsat_num_item_type dsat_cr_val;
  dsat_num_item_type dsat_crc_val;
#ifdef FEATURE_DSAT_ETSI_DATA
  dsat_mixed_param_val_type dsat_cgdcont_val[7];
#ifdef FEATURE_SECONDARY_PDP
  dsat_mixed_param_val_type dsat_cgdscont_val[4]; 
  dsat_mixed_param_val_type dsat_cgtft_val[10];
#endif /*FEATURE_SECONDARY_PDP*/
#ifdef FEATURE_DATA_WCDMA_PS
  dsat_mixed_param_val_type dsat_cgeqreq_val[15];
  dsat_mixed_param_val_type dsat_cgeqmin_val[15];
#endif /*FEATURE_DATA_WCDMA_PS*/
#ifdef FEATURE_DSAT_LTE
  dsat_mixed_param_val_type dsat_cgeqos_val[6];
#endif /*FEATURE_DSAT_LTE*/
  dsat_mixed_param_val_type dsat_cgqreq_val[6];
  dsat_mixed_param_val_type dsat_cgqmin_val[6];
  dsat_num_item_type dsat_cgerep_val[2]; 
  dsat_num_item_type dsat_cgdata_val;
#if defined(FEATURE_GSM)
  dsat_num_item_type dsat_cgclass_val;
#endif /* defined(FEATURE_GSM) */
#ifdef FEATURE_DATA_TE_MT_PDP
  dsat_num_item_type dsat_cgauto_val;
#endif /* FEATURE_DATA_TE_MT_PDP */
  dsat_num_item_type dsat_cgpiaf_val[4]; 
#endif /*FEATURE_DSAT_ETSI_DATA*/
#ifdef FEATURE_ETSI_SMS
  dsat_num_item_type dsat_cmgf_val;
  dsat_num_item_type dsat_csas_val;
  dsat_num_item_type dsat_cres_val;
  dsat_num_item_type dsat_csdh_val;
#ifdef FEATURE_ETSI_SMS_CB
  dsat_mixed_param_val_type dsat_cscb_val[3];
#endif /*FEATURE_ETSI_SMS_CB*/
#endif /* FEATURE_ETSI_SMS*/
#ifdef FEATURE_DATA_GCSD_FAX
  dsat_num_item_type dsat_fdd_val;
  dsat_num_item_type dsat_far_val;
  dsat_num_item_type dsat_fcl_val;
  dsat_num_item_type dsat_fit_val[2];
#endif  /* FEATURE_DATA_GCSD_FAX */
#if defined(FEATURE_WCDMA) || defined(FEATURE_TDSCDMA)
  dsat_mixed_param_val_type dsat_es_val[3];
  dsat_mixed_param_val_type dsat_esa_val[8];
#endif /* FEATURE_WCDMA || FEATURE_TDSCDMA*/
  dsat_num_item_type dsat_cmod_val;
#ifdef FEATURE_DSAT_LTE
  dsat_num_item_type dsat_cemode_val;
#endif /* FEATURE_DSAT_LTE*/
  dsat_num_item_type dsat_cvhu_val;
}dsat_etsi_ext_table_ss_val_struct_type;

/*
 *
 *  Multi stack command variable
 *
 */

typedef struct
{
  dsat_num_item_type dsat_creg_val;
  dsat_num_item_type dsat_cgreg_val;
#ifdef FEATURE_DSAT_LTE
  dsat_num_item_type dsat_cereg_val;
#endif /* FEATURE_DSAT_LTE*/

#ifdef FEATURE_ETSI_SMS
#ifdef FEATURE_ETSI_SMS_PS
  dsat_num_item_type dsat_cgsms_val;
#endif /*FEATURE_ETSI_SMS_PS*/
  dsat_num_item_type dsat_csms_val;
  dsat_mixed_param_val_type dsat_csca_val[2];
  dsat_mixed_param_val_type dsat_csmp_val[4];
#endif  /*FEATURE_ETSI_SMS*/
  
}dsat_etsi_ext_table_ms_val_struct_type;
#ifdef FEATURE_DATA_IS707 
/*707 Vals*/
typedef struct
{
  dsat_num_item_type dsat707_eb_val[3];
  dsat_num_item_type dsat707_cqd_val;
  dsat_num_item_type dsat707_crc_val;
  dsat_num_item_type dsat707_cta_val;
  dsat_num_item_type dsat707_cxt_val;
  dsat_num_item_type dsat707_efcs_val;
  dsat_num_item_type dsat707_er_val;
  dsat_num_item_type dsat707_esr_val;
  dsat_num_item_type dsat707_etbm_val[3];
  dsat_num_item_type dsat707_ilrr_val;
  dsat_string_item_type dsat707_ma_val[249];
  dsat_string_item_type dsat707_ms_val[249];
  dsat_num_item_type dsat707_mr_val;
  dsat_num_item_type dsat707_mv18r_val;
  dsat_num_item_type dsat707_mv18s_val[3];
#ifdef FEATURE_DS_IS707A_CMUX
  dsat_num_item_type dsat707_cmux_val[2];
#else
  dsat_num_item_type dsat707_cmux_val;
#endif /*FEATURE_DS_IS707A_CMUX*/
  dsat_num_item_type dsat707_cdr_val;
  dsat_num_item_type dsat707_cds_val[4];
  dsat_num_item_type dsat707_cfc_val;
  dsat_string_item_type dsat707_cfg_val[249];
}dsat707_ext_para_table_ss_val_struct_type;
#ifdef FEATURE_DSAT_CDMA_PIN 
typedef struct
{
  dsat_num_item_type        ref_count;
  dsat_mixed_param_val_type dsat707_cpin_val[2];
}dsat707_ext_vend_act_table_common_val;
#endif/* FEATURE_DSAT_CDMA_PIN */
typedef struct
{
  dsat_num_item_type dsat707_faa_val;
  dsat_num_item_type dsat707_fap_val[3];
  dsat_num_item_type dsat707_fbo_val;
  dsat_num_item_type dsat707_fbu_val;
  dsat_num_item_type dsat707_fcc_val[8];
  dsat_num_item_type dsat707_fcq_val[2];
  dsat_num_item_type dsat707_fcr_val;
  dsat_num_item_type dsat707_fct_val;
  dsat_num_item_type dsat707_fea_val;
  dsat_num_item_type dsat707_ffc_val[4];
  dsat_num_item_type dsat707_fhs_val;
  dsat_num_item_type dsat707_fie_val;
  dsat_num_item_type dsat707_fip_val;
  dsat_num_item_type dsat707_fis_val[8];
  dsat_string_item_type dsat707_fli_val[21];
  dsat_num_item_type dsat707_flp_val;
  dsat_num_item_type dsat707_fms_val;
  dsat_num_item_type dsat707_fnr_val[4];
  dsat_string_item_type dsat707_fns_val[181];
  dsat_string_item_type dsat707_fpa_val[21];
  dsat_string_item_type dsat707_fpi_val[21];
  dsat_num_item_type dsat707_fpp_val;
  dsat_num_item_type dsat707_fpr_val;
  dsat_num_item_type dsat707_fps_val;
  dsat_string_item_type dsat707_fpw_val[21];
  dsat_num_item_type dsat707_frq_val[2];
  dsat_num_item_type dsat707_fry_val;
  dsat_string_item_type dsat707_fsa_val[21];
  dsat_num_item_type dsat707_fsp_val;
  dsat_num_item_type dsat_flo_val;
  
}dsat707_fax_table_ss_val_struct_type;
#ifdef FEATURE_HDR
#ifdef FEATURE_HDR_AT_TEST_ONLY 
typedef struct
{
  /* HDR DRC values */
   dsat_num_item_type dsat707_qchdrc_val;
  /* HDR fixed reverse rate indicator */
   dsat_num_item_type dsat707_qchdrr_val;
  /* HDR Early termination on/off */
   dsat_num_item_type dsat707_qchdrt_val;
  /* HDR Reverse Link early termination configuration */
   dsat_num_item_type dsat707_qchdret_val;
  
}dsat707_hdr_table_ss_val_struct_type;
#endif /* FEATURE_HDR */
#endif/* FEATURE_HDR_AT_TEST_ONLY */
#ifdef FEATURE_CDMA_SMS
typedef struct
{
  dsat_num_item_type  dsat707sms_qcpms_val[3];
  dsat_num_item_type  dsat707sms_qcnmi_val[5];
  dsat_num_item_type  dsat707sms_qcmgf_val;
  dsat_num_item_type  dsat707sms_smsss_val[4];
  dsat_mixed_param_val_type dsat707sms_qcsmp_val[5];
}dsat707_sms_table_ss_val_struct_type;
#endif/* FEATURE_CDMA_SMS*/

typedef struct
{
  dsat_num_item_type dsat707_qcmdr_val;
#ifdef FEATURE_IS2000_R_SCH
/* TURN SCRM on/off */
  dsat_num_item_type dsat707_qcscrm_val;
/* TURN R-SCH throttle on/off */
  dsat_num_item_type dsat707_qctrtl_val;
#endif /* FEATURE_IS2000_R_SCH */
  dsat_num_item_type dsat707_qcvad_val;
#ifdef FEATURE_IS2000_REL_A
  dsat_num_item_type dsat707_qcqospri_val;
#endif /* FEATURE_IS2000_REL_A */
#ifdef FEATURE_IS2000_CHS
/* Enable/disable Mobile-originated Control/Hold */
  dsat_num_item_type dsat707_qcchs_val;
#endif /* FEATURE_IS2000_CHS */
}dsat707_vend_para_table_ss_val_struct_type;
typedef struct
{
  dsat_string_item_type dsat707_qcotc_val[241];
#if defined(FEATURE_DSAT_SP_CMDS)
  dsat_mixed_param_val_type dsat_spc_val;
  dsat_num_item_type        dsat707_roam_pref_val;
  dsat_mixed_param_val_type dsat_unlock_val;
  dsat_mixed_param_val_type dsat_lock_val;
  dsat_mixed_param_val_type dsat_lock_chg_val[2];
#endif/* FEATURE_DSAT_SP_CMDS */
#if defined(FEATURE_DSAT_GPS_CMDS)
  dsat_num_item_type      dsat707_gps_nmea_config_val[2];
  dsat_num_item_type      dsat707_sp_lcs_state;
  dsat_num_item_type      dsat707_gps_mode_val;
  dsat_num_item_type      dsat707_gps_port_val;
  dsat_num_item_type      dsat707_pde_trs_val;
  dsat_num_item_type      dsat707_gps_pdeaddr_val[5];
  dsat_num_item_type      dsat707_gps_init_mask_val;
#endif/* FEATURE_DSAT_GPS_CMDS */
}dsat707_vend_act_table_ss_val_struct_type;
#endif /* FEATURE_DATA_IS707 */

typedef struct
{
  dsat_num_item_type dsat_qcsimapp_val_updated;
  boolean            dsat_qcsimapp_val_user_flag;
} dsat_common_state_ss_info;

typedef struct
{
  dsat_common_state_ss_info ss;
} dsat_common_state_info;

#ifdef FEATURE_DSAT_LEGACY_GCF_SUPPORT		 
/*---------------------------------------------------------------------------
     Data type for a list strings. Used in function dsat_srch_list.    
---------------------------------------------------------------------------*/  
typedef byte list_type[][8];    
#endif /* FEATURE_DSAT_LEGACY_GCF_SUPPORT */

/*--------------------------------------------------------------------------
  AT command processing queue for token structures generated by parser.
---------------------------------------------------------------------------*/
typedef struct    
{
  q_link_type        link;                    /* Link for queue */
  tokens_struct_type token;                   /* Token structure */
} token_q_struct_type;

/*---------------------------------------------------------------------------
     Data type for command handler state.  Used for asynchronous processing.
---------------------------------------------------------------------------*/  
typedef struct
{
  dsat_mode_enum_type mode;                /*  AT command mode:            */
  const dsati_cmd_type *parse_table;       /*  Ptr to cmd in parse table   */
  const tokens_struct_type  *tok_ptr;       /*  Command tokens from parser  */
  token_q_struct_type *tok_q_item;       /*Pointer to the token in the q with the link*/
  dsm_item_type *res_buff_ptr;             /*  Place to put response       */
  dsat_result_enum_type (*proc_func)( dsat_mode_enum_type,
                                      const struct dsati_cmd_struct*,
                                      const tokens_struct_type*,
                                      dsm_item_type* );
} dsat_cmd_hdlr_state_s_type;

#define DSAT_SAVE_HDLR_STATE( var ) \
   var.mode = mode; \
   var.parse_table = parse_table; \
   var.tok_ptr = tok_ptr; \
   var.res_buff_ptr = res_buff_ptr;

extern dsat_cmd_hdlr_state_s_type dsat_curr_cmd_var;


/* Macro to handle response output */
#define DSAT_SEND_RESPONSE( buffer, rsp_type ) \
        if(dsatcmdp_block_indications() == FALSE) \
        { \
          (void)dsatcmdp_send_respose_to_te( buffer, rsp_type ); \
        } \
        else \
        { \
          dsatcmdp_handle_async_cmd_rsp( buffer, rsp_type ); \
        }

/*===========================================================================
                 ATCOP Utility Function Enumeration Types
===========================================================================*/

/*---------------------------------------------------------------------------
     Specifies the format type for displaying AT parameters.
---------------------------------------------------------------------------*/  
typedef enum        
{                   
  RESPONSE,             /*  Generates response to command - */
                        /*    No name for +Fxxx             */

  RESPONSE_ALL,         /*  Generates response to command - */
                        /*    Names all commands            */

  SET                   /*  Generates a SET command - used  */
                        /*    to transmit configuaration    */
                        /*    to the base station           */
} fmt_enum_type;


/*---------------------------------------------------------------------------
     Specifies mode of string argument storage.
---------------------------------------------------------------------------*/  
typedef enum
{
  STORE,                /*  stores argument                 */
  CAT                   /*  concatenates arg to existing val*/
} put_string_enum_type;


/*---------------------------------------------------------------------------
     Defines and enums for specifying mode and type of tuple conversion.
---------------------------------------------------------------------------*/  
#define IPV4_DELIMITER '.'
#define IPV6_DELIMITER ':'
#define DSAT_LEGACY_PROFILE_NUM_MIN 1
#define DSAT_LEGACY_PROFILE_NUM_MAX 24
#define DSAT_EXT_PROFILE_NUM_MIN 100
#define DSAT_EXT_PROFILE_NUM_MAX 179
#define DSAT_MAX_ALLOWED_PROFILES  DS_UMTS_MAX_PDP_PROFILE_NUM

#define DSAT_INVALID_PROFILE               11

#define DSAT_IS_PROFILE_ID_WITHIN_RANGE(x) \
            dsat_is_profile_id_in_range(x)

/*Below Macros are used to begin and end transactions using DS Profile api's.*/
#define DSAT_BEGIN_PROFILE_TRANSACTION(profile_id,tech_type,profile_hndl) \
          profile_status =  dsat_profile_begin_transaction(tech_type,\
                                                          (ds_profile_num_type)profile_id,\
                                                           &profile_hndl);
#define DSAT_END_PROFILE_TRANSACTION(profile_hndl) \
          profile_status =  dsat_profile_end_transaction(profile_hndl);

#define DSAT_PROFILE_DESTROY_ITERATOR(itr) \
	      profile_status = dsat_profile_destroy_iterator(itr); \
          if(profile_status != DS_PROFILE_REG_RESULT_SUCCESS) \
          { \
            return DSAT_ERROR; \
          } \

#define CHECK_APPEND_RESPONSE_BUFFER(buf_limit)\
          if (!(res_buff_ptr = etsipkt_check_response_buffer(res_buff_ptr, \
                                            buf_limit))) \
          { \
            return 0; \
          }
            
#define CHECK_APPEND_RESPONSE_BUFFER_NEW(buf_limit)\
          if (!(res_buff_ptr = etsipkt_check_response_buffer(res_buff_ptr, \
                                            buf_limit))) \
          { \
            return DSAT_ERROR; \
          }

typedef enum
{
  DSAT_CONVERT_IP    = 0,
  DSAT_CONVERT_MASK  = 1
} dsat_ip_mask_enum_type;


typedef enum
{
  STRING_TO_INT,
  INT_TO_STRING
} tuple_convert_enum_type;

typedef enum
{
  TUPLE_TYPE_IPv4,
  TUPLE_TYPE_IPv6,        /* IPV6 addr in RFC2732 preferred text mode */
  TUPLE_TYPE_PORT,
  TUPLE_TYPE_TMASK,
  TUPLE_TYPE_IPv6_OCTETS, /* IPV6 addr as '.' separated decimal octets */
  TUPLE_TYPE_MAX    /* Internal use */
} tuple_type_enum_type;

typedef  enum
{
  DSAT_APN_CLASS_ONE    = 0,
  DSAT_APN_CLASS_TWO    = 1,
  DSAT_APN_CLASS_THREE  = 2,
  DSAT_APN_CLASS_FOUR   = 3,
  DSAT_APN_CLASS_FIVE   = 4,
  DSAT_APN_CLASS_SIX    = 5,
  DSAT_APN_CLASS_MAX    = 16
}dsat_apn_class_e_type;


/*---------------------------------------------------------------------------
     Formatting options for mult-index read commands 
---------------------------------------------------------------------------*/  
typedef enum
{
  MULTI_INDEX_TEST,
  MULTI_INDEX_READ,
  MULTI_INDEX_AMPV
} multi_index_fmt_e_type;

#if defined(FEATURE_ETSI_SMS) || defined(FEATURE_CDMA_SMS)
/*-------------------------------------------------------------------------
  Enum for +CMSS ERRORS (3G TS 27.007 section 3.2.5)
---------------------------------------------------------------------------*/
typedef enum
{
  DSAT_CMS_NONE               = 0,
  DSAT_CMS_OTHER_GENERAL_PROBLEMS = 107,
  DSAT_CMS_ME_FAILURE         = 300,
  DSAT_CMS_SERVICE_RESERVED   = 301,
  DSAT_CMS_OP_NOT_ALLOWED     = 302,
  DSAT_CMS_OP_NOT_SUPPORTED   = 303,
  DSAT_CMS_INVALID_PDU_PARAM  = 304,
  DSAT_CMS_INVALID_TXT_PARAM  = 305,
  DSAT_CMS_SIM_NOT_INSERTED   = 310,
  DSAT_CMS_SIM_PIN_REQ        = 311,
  DSAT_CMS_PHSIM_PIN_REQ      = 312,
  DSAT_CMS_SIM_FAILURE        = 313,
  DSAT_CMS_SIM_BUSY           = 314,
  DSAT_CMS_SIM_WRONG          = 315,
  DSAT_CMS_SIM_PUK_REQ        = 316,
  DSAT_CMS_SIM_PIN2_REQ       = 317,
  DSAT_CMS_SIM_PUK2_REQ       = 318,
  DSAT_CMS_MEM_FAILURE        = 320,
  DSAT_CMS_INVALID_INDEX      = 321,
  DSAT_CMS_MEM_FULL           = 322,
  DSAT_CMS_SCA_ADDR_UNKNOWN   = 330, 
  DSAT_CMS_NO_SERVICE         = 331,
  DSAT_CMS_NETWORK_TIMEOUT    = 332,
  DSAT_CMS_CNMA_NOT_EXP       = 340,
#ifdef FEATURE_DSAT_EXTENDED_SMS
  DSAT_CMS_UNKNOWN_ERR        = 350,
#else
  DSAT_CMS_UNKNOWN_ERR        = 500,
#endif /* FEATURE_DSAT_EXTENDED_SMS */
  DSAT_CMS_MSG_BLOCKED        = 510,
  DSAT_CMS_MAX                = 512
} dsati_cms_err_e_type;
#endif /* (FEATURE_ETSI_SMS) || (FEATURE_CDMA_SMS) */

/*---------------------------------------------------------------------------
     Abortable command state
---------------------------------------------------------------------------*/
typedef enum {
  NOT_ABORTABLE,        /* current command is not an abortable command  */
  ABORTABLE,            /* current command is abortable but the aborting
                           hasn't happen                                */
  ABORTING              /* a char has been received to abort the 
                           current command                              */
} dsati_abort_type;

/*---------------------------------------------------------------------------
     Character sets between which translation is supported. Not neccesarily 
     all supported by +CSCS command, additional alphabets are for use in 
     conversion function. Values indices in array dsat_cscs_valstr should 
     match enum dsati_chset_type.
---------------------------------------------------------------------------*/
typedef enum {
  ALPHA_IRA = 0,        /* International Reference Alphabet T.50 */
  ALPHA_GSM,            /* GSM 7 bit alphabet, not packed to 7 bits
                          (will not contain @ (0x00); might have got mapped to 
                           0xe6 at the ATCOP parser ) */
  ALPHA_UCS2,           /* UCS2 Unicode, rep'd by "4 hex character"-tuplets */
  ALPHA_HEX,            /* HEX, rep'd by "2 hex character"-tuplets */
  ALPHA_8BIT,           /* Octets, of 0-255 value */
  ALPHA_PBM_8BIT,       /* PBM's 8 bit alphabet */
  ALPHA_GSM_WITH_AT,    /* GSM 7 bit alphabet, which contains @ (0x00)
                           This character set should be used when 
                           dsatutil_convert_chset has to be called for sending the
                           converted string to modules other than ATCOP. 
                           the output might contain GSM7 bit @ (0x00) so, str_len on 
                           output string might return a wrong value. Output_len has to be 
                           calculated from the input string only.
                           This is for internal use with in ATCOP. This character
                           set is exactly the same as ALPHA_GSM above */
  ALPHA_PBM_UCS2,       /* The 2 byte representation of UCS2 for PBM */
  ALPHA_GSM_UCS2,       /* Special GSM encoding of UCS2 as specified in 
                           GSM TS 11.11 Annex B */
  ALPHA_MAX
} dsati_chset_type;

/*--------------------------------------------------------------------------
   Macro's for GSM @ character support 
-------------------------------------------------------------------------- */
/* The replacement character for GSM @(0x00) is chosen to be 0xE6 as GSM
valid characters are only from 0x00 - 0X7F and 0xE6 doesnt fall into that. */
#define GSM_AT_CHAR_REPLACEMENT  0xE6
#define GSM_AT_CHAR  0x00

/* Replacement character (space) specified by 23.038 */
#define REPLACEMENT_CHAR 0x20

/*---------------------------------------------------------------------------
     Length of a single character in each of these character sets:
     e.g. 4 hexadecimal characters to represent a single UCS2 character, etc.
---------------------------------------------------------------------------*/
#define UCS2_CHAR_LEN      4
#define HEX_CHAR_LEN       2
#define GSM_CHAR_LEN       1
#define IRA_CHAR_LEN       1
#define EIGHT_BIT_CHAR_LEN 1
#define PBM_8BIT_CHAR_LEN  1
#define PBM_UCS2_CHAR_LEN  2

/*---------------------------------------------------------------------------
     Packet Dial String Lookup Table
---------------------------------------------------------------------------*/
#define MAX_DIAL_STRING_SIZE       NV_PKT_DIAL_STRING_SIZE
#define MAX_DIAL_STRING_LIST_SIZE  NV_PKT_DIAL_STRING_LIST_SIZE

typedef struct
{
  byte                           dial_string[MAX_DIAL_STRING_SIZE];
  nv_operating_mode_enum_type    call_type;
  nv_operating_mode_enum_type    modes_permitted;
} dsati_pkt_dial_string_s_type;

typedef struct
{
  byte                           size;
  dsati_pkt_dial_string_s_type   dial_string_info[MAX_DIAL_STRING_LIST_SIZE];
} dsati_pkt_dial_string_table_s_type;

typedef enum clck_mode_e
{
  DSAT_CLCK_MODE_UNLOCK     = 0,
  DSAT_CLCK_MODE_LOCK       = 1,
  DSAT_CLCK_MODE_QUERY      = 2,
  DSAT_CLCK_MODE_MAX
} clck_mode_e_type;

#ifdef FEATURE_MMGSDI

#define  DSAT_MMGSDI_STATUS_MASK  0xFF

#endif /* FEATURE_MMGSDI */

#if defined(FEATURE_DATA_ETSI_PIN) || defined(FEATURE_DSAT_CDMA_PIN)
/*---------------------------------------------------------------------------
  Personal Identity Number required by ME to permit operation by user
---------------------------------------------------------------------------*/
typedef enum 
{
  DSAT_CPIN_NONE_REQUIRED                    = 0x00000001,
  DSAT_CPIN_SIM_PIN_REQUIRED                 = 0x00000002,
  DSAT_CPIN_SIM_PIN2_REQUIRED                = 0x00000004,
  DSAT_CPIN_PH_SIM_PIN_REQUIRED              = 0x00000008,
#ifdef FEATURE_MMGSDI_PERSONALIZATION
  /* Personalization PINs for card in slot1 */
  DSAT_CPIN_NET_PIN_REQUIRED                 = 0x00000010, 
  DSAT_CPIN_NETSUB_PIN_REQUIRED              = 0x00000020,
  DSAT_CPIN_SP_PIN_REQUIRED                  = 0x00000040,   
  DSAT_CPIN_CORP_PIN_REQUIRED                = 0x00000080, 
  DSAT_CPIN_PH_FSIM_PIN_REQUIRED             = 0x00000100,
#endif /* FEATURE_MMGSDI_PERSONALIZATION */
  
  DSAT_CPIN_STATE_CHANGE_PENDING             = 0x40000000,
  DSAT_CPIN_MAX                              = 0x7FFFFFFF
} dsat_cpin_required_e_type;

/*--------------------------------------------------------------------------
  Macros to flag PIN state setting
---------------------------------------------------------------------------*/
#define IS_PIN_STATE(state) ((uint32)state == (me_ms_val->dsat_pin_required & (uint32)state))
#define IS_PUK_STATE(state) ((uint32)state == (me_ms_val->dsat_pin_blocked & (uint32)state))

#endif /* defined(FEATURE_DATA_ETSI_PIN) || defined(FEATURE_DSAT_CDMA_PIN) */
/* Max SC Address length */
#define SCA_MAX_LEN (((WMS_ADDRESS_MAX + 1) * UCS2_CHAR_LEN) + 1)

/*---------------------------------------------------------------------------
     Operating system timers
---------------------------------------------------------------------------*/
/* Delay x mS waiting for DSM large items to be freed when listing large 
   quantities of PB entries or SMSes */
#define DSAT_FREE_ITEM_DLY 5
#define DSAT_INBAND_DTMF_RING_DELAY 20 /* Inband DTMF Delay */

#define DSAT_CEAP_CMD_WAIT_DELAY 120 
#define DSAT_CEAP_PROCESS_WAIT_DELAY 180000 
#define DSAT_RDM_OPEN_TIMEOUT 500
typedef enum dsat_timer_id
{
  DSAT_TIMER_VOICE_RING,   /* Voice call RING response         */
#ifdef FEATURE_DUAL_ACTIVE
  DSAT_TIMER_VOICE_RING_2,  /* Voice call RING response for 2nd active subscription */
#endif /* FEATURE_DUAL_ACTIVE */
#ifdef FEATURE_DSAT_ETSI_MODE
  DSAT_TIMER_CAOC_REPORT,  /* Advice of charge meter reporting */
  DSAT_INBAND_DTMF_RING, /* Timer for Inband DTMF Ring */
#endif /* FEATURE_DSAT_ETSI_MODE */
  DSAT_TIMER_VOICE_CALL_TIMEOUT, /* Voice S7 Timer  */
#ifdef FEATURE_DUAL_ACTIVE
  DSAT_TIMER_VOICE_CALL_TIMEOUT_2, /* Voice S7 Timer for 2nd active subscription */
#endif /* FEATURE_DUAL_ACTIVE */
  DSAT_TIMER_DATA_CALL_TIMEOUT, /* Data S7 Timer  */
  DSAT_TIMER_DATA_CALL_TIMEOUT_2, /* Data S7 Timer for 2nd active Data call */
#ifdef FEATURE_DATA_MUX
  DSAT_TIMER_RDM_OPEN_TIMEOUT,
#endif /* FEATURE_DATA_MUX */
  DSAT_TIMER_CEAP_REPORT_TIMEOUT,
  DSAT_TIMER_CEAP_EAP_TIMEOUT,
  DSAT_TIMER_FREE_ITEM,
  DSAT_TIMER_MAX
}dsat_timer_id_e_type;

typedef dsat_result_enum_type (*dsat_timer_exp_hdlr_type)
(
  ds_cmd_type         * cmd_ptr              /* DS Command pointer         */
);


typedef struct dsat_timer
{
  dsat_timer_id_e_type      timer_id;    /* Timer identifier             */
  rex_timer_type *          timer_ptr;   /* Operating system timer       */
  rex_timer_cb_type         cb_func;     /* REX callback on timer expiry */
  dsat_timer_exp_hdlr_type  hdlr_func;   /* DS event handler             */
  dword                     duration;    /* Length of timer              */
  boolean                   is_defined;  /* Timer registered with OS     */
  boolean                   is_running;  /* Timer active flag            */
  void *                    user_data;   /* Timer specific data */
} dsat_timer_s_type;

typedef struct dsat_timer_ex
{
  dsat_timer_id_e_type      timer_id;    /* Timer identifier             */
  dsat_timer_exp_hdlr_type  hdlr_func;   /* DS event handler             */
} dsat_timer_ex_s_type;

/* Data structure that holds the function    */ 
/* pointers for  ATA, ATH,+CGANS call backs  */
typedef struct
{
  dsat_call_hangup_cb_type          hangup_cb;
  dsat_atz_cb_type                  atz_cb;
  dsat_incom_answer_cb_type         answer_cb;
#ifdef FEATURE_DATA_TE_MT_PDP
  dsat_pdp_incom_answer_cb_type     pdp_answer_cb;
  dsat_call_hangup_cb_type          pdp_hangup_cb;
#endif /* FEATURE_DATA_TE_MT_PDP */
} dsat_dial_string_type;

typedef struct
{
  ds_profile_3gpp_ip_version_enum_type ip_vsn;
  ds_profile_3gpp_pdp_addr_type        addr;
}dsat_ip_addr_type;

typedef struct {
  int32                    client_id;  /* Client Id             */
  ds3g_siolib_port_e_type  port;       /* port which is pending */
  boolean                  is_pending; /* pending flag          */ 
  byte                     cmd_name[MAX_CMD_SIZE]; /* command name */
}dsat_fwd_at_pending_type;

/* Variable that is used to retain the */
/* call back function pointers for ATD, ATA ,+CGANS */
/* ATH and abort handlers for ATA and ATD    */
extern dsat_dial_string_type dial_string_handler;

/* indicates the current abort handler */
extern dsat_call_abort_cb_type dsat_curr_abort_handler;
#ifdef FEATURE_DSAT_MDM_FUSION
/* indicates current Rm-Sm proxy abort handler */
extern dsat_proxy_call_abort_cb_type dsat_proxy_curr_abort_handler;
#endif /* FEATURE_DSAT_MDM_FUSION */

extern dsat_timer_s_type dsatutil_timer_table[];


/*---------------------------------------------------------------------------
    AT Command Table
---------------------------------------------------------------------------*/
extern const dsati_at_cmd_table_type at_cmd_table;
extern const dsati_at_cmd_table_ex_type at_cmd_table_ex;


/*---------------------------------------------------------------------------
    AT Command State
---------------------------------------------------------------------------*/
extern dsat_mode_enum_type dsatcmdp_at_state;  


/*---------------------------------------------------------------------------
    Asynchronous Event Handler Table
---------------------------------------------------------------------------*/
extern const dsati_async_event_table_type async_event_table;
#ifdef FEATURE_DSAT_DYNAMIC_LOADING
extern const dsati_async_event_table_type async_event_table_ex;
#endif /* FEATURE_DSAT_DYNAMIC_LOADING */

/*---------------------------------------------------------------------------
    Flag that indicates whether or not an abortable command is in progress.
---------------------------------------------------------------------------*/
extern dsati_abort_type dsatcmdp_abortable_state;


/*---------------------------------------------------------------------------
    Flag that indicates whether or not calling the function to send a
    result code to the TE terminates the abort handling for that command.
---------------------------------------------------------------------------*/
extern boolean dsatcmdp_end_abort_in_send_rsp;

/*---------------------------------------------------------------------------
    Flag that indicates whether or not ME is operating in restricted
    command mode due to authorization requirement.
---------------------------------------------------------------------------*/
extern boolean dsatcmdp_restricted_commands[];

/*---------------------------------------------------------------------------
    Flag that indicates a async cmd is under processing,
    Atcop preprocessor does not process any incoming char when it is TRUE.
---------------------------------------------------------------------------*/
extern boolean dsatcmdp_processing_async_cmd;

/*---------------------------------------------------------------------------
    Flag that indicates ATH cmd is under processing,
    continues to process the AT command line if TRUE.
---------------------------------------------------------------------------*/
extern boolean dsati_processing_ath_cmd;

/* Flag to indicate if tx (dsat_to_sio) watermark hit high.
   If this flag is high, ATCOP should halt current commands that
   requesting data from the subsystem
*/
extern boolean dsati_tx_wm_high;

/* Function to reenter current command processing which has halted
   when tx (dsat_to_sio) watermark hit high.
*/
extern dsati_async_event_handler_type dsati_to_sio_low_handler;

extern dsat_num_item_type dsatcmdp_dds_qcsimapp_val;

/* Trigger the second layer of flow control by the free item timer when
   DSM large item count goes below this quantity */
#define DSAT_FREE_ITEM_TRIGGER 3

/* Handler function and timer to delay when listing large quantities of PB 
   entries or SMSes and DSM large items are close to being exhausted: this 
   needed as USB dequeues from DS3GSIOLIB WM as quickly as queued so normal 
   flow control doesn't kick in, USB can acquire chain of all available DSM 
   large items and never have a chance to free them as its task doesn't get 
   a chance to run while DS and WMS or PBM are busy with this. */
extern rex_timer_type dsati_free_item_timer;

/* Handler function to set pointer in dsat_timer_s_type to.
   It calls function pointer below if not NULL. */
extern dsat_result_enum_type dsati_free_item_timer_expired_handler
(
  ds_cmd_type         * cmd_ptr 
);

/* Handlers for PB entry/SMS reads can set this function to reenter 
   processing following delay to allow DSM items to be freed by USB task. */

extern dsati_async_event_handler_type dsati_handle_free_item_timer_expiry;
extern dsat_connect_rate_s_type dsat_report_rate_val;

/*-------------------------------------
  Table to convert IRA characters to GSM 7 bit default alphabets
  Refer to 3G TS 27.005 Annex A for more details                 
---------------------------------------*/
extern const uint8 ira_to_gsm7[];
/*-------------------------------------
  Table to convert GSM 7 bit default alphabets to IRA characters 
---------------------------------------*/
extern const uint8 gsm7_to_ira[];


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
     
/*===========================================================================
 FUNCTION DSAT_PROCESS_EFS_DATA
 
 DESCRIPTION
   This function sends the data to the TE. Responsible for parsing the
   data handling special cases as well.
 
 DEPENDENCIES
   None.
 RETURN VALUE
   boolean: - TRUE in success case, FALSE otherwise
 
 SIDE EFFECTS
   None.
 ===========================================================================*/
boolean dsat_process_efs_data
(
  dsat_string_item_type * buffer,
  dsat_num_item_type      size,
  dsm_item_type         * res_buff_ptr,
  dsat_efs_data_e_type    efs_type,
  void                  * user_data
);

/*===========================================================================
 FUNCTION DSAT_READ_EFS_DATA
 
 DESCRIPTION
   This function gets the file_size of the file in EFS. Currently we support two
   types of efs_files
   1) Command Names: ALl command names are in EFS
   2) Strings: Strings in EFS. Read it Dynamically here
 
 DEPENDENCIES
   None.
 
 RETURN VALUE
   ret: Returns the number of bytes read for a given file
 
 SIDE EFFECTS
   None.
 ===========================================================================*/
dsat_num_item_type dsat_read_efs_data
(
  char                * file_name,
  dsat_efs_file_e_type  file_type,
  void                ** buffer
);

/*===========================================================================
 FUNCTION DSAT_GET_STRING_FILE_SIZE
 
 DESCRIPTION
   This function gets the file_size of the file in EFS. 
 
 DEPENDENCIES
   None.
 
 RETURN VALUE
   ret: Returns the number of bytes read for a given file
 
 SIDE EFFECTS
   None.
 ===========================================================================*/
dsat_num_item_type dsat_get_string_efs_file_size
(
  int fs_handle
);

/*===========================================================================
 FUNCTION DSAT_EFS_FILE_OPEN
 
 DESCRIPTION
   Open a given EFS file . Currently we support two types of efs_files
   1) Command Names: ALl command names are in EFS
   2) Strings: Over 500 bytes strings in EFS. Read it Dynamically here
 
 DEPENDENCIES
   None.
 RETURN VALUE
   fs_handle: -1 for error and fs_handle>=0 in success case
 
 SIDE EFFECTS
   None.
 ===========================================================================*/
int dsat_efs_file_open
(
  char                *file_name,
  dsat_efs_file_e_type file_type
);

/*===========================================================================

FUNCTION dsatprep_set_preprocessor_mode

DESCRIPTION
  Changes the SIO data preprocessor mode of operation.  Operating modes
  can be: AT command mode, abort command mode, or SMS message entry mode.
  AT command mode is the default mode of SIO data preprocessor operation.
  It handles normal AT command line entry.  Abort command mode provides
  processing to abort an abortable command when any character is received.
  SMS message entry mode handles input of SMS messages.

  A state machine in this function controls transitions between SMS
  message entry mode and abort command mode when abortable commands are
  being processed.  When an abortable command is being processed and
  current mode is SMS message entry mode, entry into abort command mode
  will be delayed until the next call to this function.  Once abort
  command mode is exited, AT command mode will always be entered.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Mode changes may be delayed when abortable SMS commands are being
  processed (see description above).  SIO AUTODETECT mode is rearmed
  on entry into AT command line mode.

===========================================================================*/
extern void dsatprep_set_preprocessor_mode
(
  dsati_preproc_mode_enum_type mode,    /* mode to set data preprocessor to */
  ds3g_siolib_port_e_type      port     /* port to set preprocessor mode    */
);

/*===========================================================================

FUNCTION dsatprepi_set_preprocessor_mode

DESCRIPTION
  Changes the SIO data preprocessor mode of operation.  Operating modes
  can be: AT command mode, abort command mode, or SMS message entry mode.
  AT command mode is the default mode of SIO data preprocessor operation.
  It handles normal AT command line entry.  Abort command mode provides
  processing to abort an abortable command when any character is received.
  SMS message entry mode handles input of SMS messages.

  A state machine in this function controls transitions between SMS
  message entry mode and abort command mode when abortable commands are
  being processed.  When an abortable command is being processed and
  current mode is SMS message entry mode, entry into abort command mode
  will be delayed until the next call to this function.  Once abort
  command mode is exited, AT command mode will always be entered.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Mode changes may be delayed when abortable SMS commands are being
  processed (see description above).  SIO AUTODETECT mode is rearmed
  on entry into AT command line mode.

===========================================================================*/

extern void dsatprepi_set_preprocessor_mode
(
  dsati_preproc_mode_enum_type mode,    /* mode to set data preprocessor to */
  ds3g_siolib_port_e_type      port     /* port to set preprocessor mode    */
);


/*===========================================================================

FUNCTION DSATPREP_GET_PREPROCESSOR_MODE

DESCRIPTION
  This function returns the current preprocessor mode.
 
DEPENDENCIES
  None

RETURN VALUE
  Current pre-processor mode.

SIDE EFFECTS
  None

===========================================================================*/
extern dsati_preproc_mode_enum_type dsatprep_get_preprocessor_mode(void);

/*===========================================================================

FUNCTION DSATPREP_PROCESS_SIO_COMMAND

DESCRIPTION
 This function invokes SIO data preprocessor that filters and echoes
 received characters, handles line editing, based on internal operating
 mode.  Operating modes can be AT command line, abort command, or SMS
 message entry mode.  Operating modes are changed during SMS command
 processing and when abortable commands are being processed.

 In AT command line mode, once AT command line parsing is completed
 AT commands are processed.  AT commands are processed based on AT
 state (Command or Online Command) and operating service mode.
 
 In abort command mode, any received character causes the command
 executing to be aborted.  The SIO data preprocessor returns a Boolean
 indicating the call should be aborted for call related commands or it
 calls a function to terminate other abortable commands.

 In SMS message entry mode, the execution of SMS command is completed
 after complete SMS message is received or the command is cancelled.

 Called from 3G DSMGR when data is received from SIO.

DEPENDENCIES
  None

RETURN VALUE
  Abort call result; TRUE if call should be aborted, FALSE otherwise.

SIDE EFFECTS
  There are side effects associated with processing a command line,
  aborting a command, and SMS message entry.

===========================================================================*/
boolean dsatprep_process_sio_command
(
  dsat_mode_enum_type at_state,         /* Command, online data, or online
                                           command state */
  dsm_watermark_type *wm_item_ptr       /* Pointer to watermark item to
                                           retrieve command line data from */
);

/*===========================================================================

FUNCTION DSATPAR_PARSE_CMD_LINE

DESCRIPTION
  This function parses a NULL terminated command line with the "AT"
  removed.  It parses each command and its associated arguments within
  the command line.  A token structure is produced for each command
  parsed.  Each token structure produced is placed on the AT command
  processor token queue for later processing.  

DEPENDENCIES
  None

RETURN VALUE
  Result code indicating whether an error occurred during parsing or
  parsing was successful (DSAT_OK or DSAT_ERROR).

SIDE EFFECTS
  None

===========================================================================*/
extern dsat_result_enum_type dsatpar_parse_cmd_line
(
  byte *cmd_line                /*  NULL terminated command line with
                                    "AT" prefix and <CR> removed. */
);
/*===========================================================================

FUNCTION DSATCMDP_INIT_CONFIG

DESCRIPTION
  This function initializes the complete set of AT command paramters
  to their default values.

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS


===========================================================================*/
extern void dsatcmdp_init_config( void );


/*===========================================================================

FUNCTION DSATCMDP_REG_UNREC_CMD_HANDLER

DESCRIPTION
  This function registers a handler that is called when an unrecognized
  AT command is received.  The handler is registered for the AT command
  operating mode specified.  If no handler is registered for a command
  operating mode, an error result code is returned by default when an
  unrecognized command is encountered.

  This functionality is intended to provide support for IS-707 unrecognized
  command handling.
  
  The handler may be deregistered for the AT command mode specified by
  passing in a NULL pointer.

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
extern void dsatcmdp_reg_unrec_cmd_handler
(
  dsati_unrec_cmd_handler_ptr_type func_ptr,    /* Pointer to mode specific
                                                   unrecognized command
                                                   handler */
  dsati_operating_cmd_mode_type oper_mode       /* AT command set operating
                                                   mode */
);


/*===========================================================================

FUNCTION DSATCMDP_QUEUE_TOKEN

DESCRIPTION
  Queues token structure generated by parser on token queue.  A token
  structure is generated by each command parsed by the AT command parser.

DEPENDENCIES
  Token queues must have been initialized.

RETURN VALUE
  DSAT_OK if token structure was successfully queued.  DSAT_ERROR
  if no buffers are available for token structure and it could not be
  placed on queue.

SIDE EFFECTS
  None

===========================================================================*/
extern dsat_result_enum_type dsatcmdp_queue_token
(
  const tokens_struct_type *token_ptr       /* Pointer to token structure */
);


/*===========================================================================

FUNCTION DSATCMDP_FLUSH_TOKEN_QUEUE

DESCRIPTION
  Flushes token queue.  Any token structures in queue are removed and
  placed back on the token free queue.
  
DEPENDENCIES
  Token queues must be initialized.

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
extern void dsatcmdp_flush_token_queue( void );


/*===========================================================================

FUNCTION DSATCMDP_QUEUE_CMD_RESULTS

DESCRIPTION
  This function places the results of AT commands in a queue,
  so they can be output (or discarded) at the end of the command
  line. It will output the contents of the queue if more than a
  defined number of DSM items are on the queue.  This is done
  to control DSM usage, as some responses can be large.

  Note: The queue count threshold may require adjustment for IS-707
  mode of operation, to insure the entire command line response
  can be buffered.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void dsatcmdp_queue_cmd_results
(
  dsm_item_type *item_ptr       /* Pointer to command results to queue */
);

/*===========================================================================

FUNCTION DSATCMDPI_QUEUE_CMD_RESULTS

DESCRIPTION
  This function places the results of AT commands in a queue,
  so they can be output (or discarded) at the end of the command
  line. It will output the contents of the queue if more than a
  defined number of DSM items are on the queue.  This is done
  to control DSM usage, as some responses can be large.

  Note: The queue count threshold may require adjustment for IS-707
  mode of operation, to insure the entire command line response
  can be buffered.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void dsatcmdpi_queue_cmd_results
(
  dsm_item_type *item_ptr       /* Pointer to command results to queue */
);

/*===========================================================================

FUNCTION DSATCMDP_SEND_QUEUED_CMD_RESULTS

DESCRIPTION
  This functions flushes the results queue to the serial port or PS
  protocol stack based on the registered response routing function.
    
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
extern void dsatcmdp_send_queued_cmd_results( void );


/*===========================================================================

FUNCTION DSATCMDP_DISCARD_QUEUED_CMD_RESULTS

DESCRIPTION
  This functions discards any queued up AT command results.  Required
  for IS-707 unrecognized command processing.
  
DEPENDENCIES
  AT command processor must have been initialized prior to calling
  this function.

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
extern void dsatcmdp_discard_queued_cmd_results( void );

/*===========================================================================

FUNCTION DSATCMDP_PROCESS_CURR_CMD

DESCRIPTION
  This function processes the current command.

DEPENDENCIES
  None
  
RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatcmdp_process_curr_cmd(void);


/*===========================================================================

FUNCTION DSATCMDP_HANDLE_ASYNC_CMD_RSP

DESCRIPTION
  Handles the raw unformatted AT command response generated by an
  asynchronous command handler function.  Formats the raw response
  contained in possibly chained DSM items and queues it up for
  transmission or sends it immediately.  The DSM item pointed to by
  the raw response buffer is freed.

  The response is formatted based on the input parameter "type of
  response".  This specifies whether the response contained in the
  DSM item is the start, end, complete, or intermediate response and
  in turn if a header, trailer, header and trailer, or no header and
  trailer should be added to the response, respectively.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Uses DSM item pointed to by formatted_rsp_ptr.

===========================================================================*/
void dsatcmdp_handle_async_cmd_rsp
(
  dsm_item_type *raw_rsp_ptr,      /* Ptr to raw unformatted AT cmd response */
  dsat_rsp_enum_type type_of_rsp   /* Type of response data in DSM item      */
);


/*===========================================================================

FUNCTION DSATCMDP_ADD_URC_TO_QUEUE

DESCRIPTION
  Adds the passed DSM item onto the URC queue.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsatcmdp_add_urc_to_queue
(
  ds3g_siolib_port_e_type  port_id,
  dsm_item_type           *dsm_item_ptr
);
/*===========================================================================
FUNCTION  DSATCMDP_FLUSH_URC

DESCRIPTION
  Flushes the buffered Unsolicited result codes

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsatcmdp_flush_urc
(
  ds3g_siolib_port_e_type port_id
);

/*===========================================================================

FUNCTION DSATCMDP_ABORT_COMMAND

DESCRIPTION
  Calls abort command handler located in command table to abort the
  abortable AT command currently being processed.  This function
  may be called multiple times during the command abort process,
  although only the first call to this function during a particular
  command's abort sequence will invoke the command abort handler.

DEPENDENCIES
  None

RETURN VALUE
  Value indicating whether or not call should be aborted:
      TRUE if call should be aborted, FALSE otherwise.

SIDE EFFECTS
  May set abortable command table entry pointer to NULL.

===========================================================================*/
extern boolean dsatcmdp_abort_command( void );


/*===========================================================================

FUNCTION DSATCMDP_COMMAND_ABORT_COMPLETE

DESCRIPTION
  Clears flag used during abort command processing and places SIO
  preprocessor in AT command mode.  Must be called when command abort
  processing has been completed, even if the command was not sucessfully
  aborted.  

DEPENDENCIES
  Should only be called when the process of aborting a command has
  completed, and must be called even if command was not aborted
  successfully.

RETURN VALUE
  None

SIDE EFFECTS
  Clears abort command flag and sets SIO preprocessor to AT command
  mode.

===========================================================================*/
extern void dsatcmdp_command_abort_complete( void );


/*===========================================================================

FUNCTION DSATCMDP_COMMAND_ABORT_COMPLETE_WITH_PORT

DESCRIPTION
  Clears flag used during abort command processing and places SIO
  preprocessor in AT command mode.  Must be called when command abort
  processing has been completed, even if the command was not sucessfully
  aborted.

DEPENDENCIES
  Should only be called when the process of aborting a command has
  completed, and must be called even if command was not aborted
  successfully.

RETURN VALUE
  None

SIDE EFFECTS
  Clears abort command flag and sets SIO preprocessor to AT command
  mode.

===========================================================================*/
extern void dsatcmdp_command_abort_complete_with_port
(
  ds3g_siolib_port_e_type     port  /* Port whose abortable state needs
                                       to be reset */
);


/*===========================================================================

FUNCTION DSATCMDP_DISABLE_CMD_ABORT_COMPLETE

DESCRIPTION
  Disables termination of abort command handling of current command
  being processed when call to function to send result code is made.
  By default, calls to function to send result code will end the abort
  handling of an abortable command in progress.

  Should be called by ATCoP command execution function requiring this
  behavior for abortable commands.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Clears end abort in send response flag.

===========================================================================*/
extern void dsatcmdp_disable_cmd_abort_complete( void );


/*===========================================================================

FUNCTION DSATCMDP_GET_OPERATING_CMD_MODE

DESCRIPTION
  Gets the current AT command processor operating service mode used for
  selection of different AT command sets from command table.

DEPENDENCIES
  None
  
RETURN VALUE
  Current operating service mode.

SIDE EFFECTS
  None
  
===========================================================================*/
extern dsati_operating_cmd_mode_type dsatcmdp_get_operating_cmd_mode( void );

/*===========================================================================

FUNCTION DSATCMDP_IS_ETSI_CMD

DESCRIPTION
  Returns TRUE if the current command in execution is ETSI otherwise FALSE.

DEPENDENCIES
  None

RETURN VALUE
  TRUE - If command in processing is ETSI command.
  FALSE - If command in processing is CDMA command.
 
SIDE EFFECTS
  None

===========================================================================*/
extern boolean dsatcmdp_is_etsi_cmd( void );

/*===========================================================================

FUNCTION DSATCMDP_IS_CDMA_CMD

DESCRIPTION
  Returns TRUE if the current command in execution is CDMA otherwise FALSE.

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
extern boolean dsatcmdp_is_cdma_cmd( void );

#ifdef FEATURE_DATA_IS707
/*===========================================================================

FUNCTION DSATCMDP_NOT_SUPPORTED_IN_JCDMA

DESCRIPTION
  This function checks the current mode for the commands that are not supported
  in JCDMA.

DEPENDENCIES
  None

RETURN VALUE
  Boolean

SIDE EFFECTS
  None

===========================================================================*/
extern boolean dsatcmdp_not_supported_in_jcmda( void );
#endif /*FEATURE_DATA_IS707*/

#ifdef FEATURE_DSAT_MDM_FUSION
/*===========================================================================

FUNCTION DSATCMDP_SEND_PROXY_RESPONSE

DESCRIPTION
  This function sends the proxy response from MSM to MDM. 

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
extern void dsatcmdp_send_proxy_response
(
  dsat_result_enum_type result_code
);
#endif /* FEATURE_DSAT_MDM_FUSION */

/*===========================================================================

FUNCTION DSATCMDP_GET_CURRENT_MODE_PER_SUBS

DESCRIPTION
  Gets the current AT command mode per subscription, GSM or WCDMA or CDMA.

DEPENDENCIES
  None

RETURN VALUE
  Current service mode per subscription.

SIDE EFFECTS
  None

===========================================================================*/
dsati_mode_e_type dsatcmdp_get_current_mode_per_subs
(
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================

FUNCTION DSATCMDP_GET_CURRENT_MODE

DESCRIPTION
  Gets the current AT command mode, GSM or WCDMA or CDMA.

DEPENDENCIES
  None

RETURN VALUE
  Current service mode.

SIDE EFFECTS
  None

===========================================================================*/
dsati_mode_e_type dsatcmdp_get_current_mode( void );

/*===========================================================================

FUNCTION DSATCMDP_BLOCK_INDICATIONS_EX

DESCRIPTION
  Returns value to indicate whether or not indications sent to the TE
  should be blocked.  No indications should be sent to TE from the
  time entering an AT command line begins to the time command line
  processing completes.  Command line processing is considered completed
  after the final result code or response is sent to the TE.  Also,
  indications should not be sent to the TE while in a data call.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if sending of indications to TE should be blocked or FALSE
  otherwise.

SIDE EFFECTS
  None

===========================================================================*/
extern boolean dsatcmdp_block_indications_ex( void );

/*===========================================================================

FUNCTION DSATCMDP_BLOCK_INDICATIONS

DESCRIPTION
  Returns value to indicate whether or not indications sent to the TE
  should be blocked.  No indications should be sent to TE from the
  time entering an AT command line begins to the time command line
  processing completes.  Command line processing is considered completed
  after the final result code or response is sent to the TE.  Also,
  indications should not be sent to the TE while in a data call.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if sending of indications to TE should be blocked or FALSE
  otherwise.

SIDE EFFECTS
  None

===========================================================================*/
extern boolean dsatcmdp_block_indications( void );

/*===========================================================================

FUNCTION DSATCMDP_GET_BLOCK_INDICATIONS

DESCRIPTION
  This function is used to get block indications flag on a serial port.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean dsatcmdp_get_block_indications
(
  ds3g_siolib_port_e_type port_id
);

/*===========================================================================

FUNCTION DSATCMDP_SET_BLOCK_INDICATIONS

DESCRIPTION
  This function is used to set block indications flag on a serial port.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsatcmdp_set_block_indications
(
  ds3g_siolib_port_e_type port_id,
  boolean                 flag_val
);

/*===========================================================================

FUNCTION DSATCMDP_SET_BLOCK_INDICATIONS_ALL_PORTS

DESCRIPTION
  This function is used to set block indications flag on all serial ports.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsatcmdp_set_block_indications_all_ports
(
  boolean flag_val
);


/*===========================================================================

FUNCTION  DSATRSP_SEND_RESPONSE 

DESCRIPTION
  Sends AT response based on routing function registered or to TE
  if no function is registerd.  Routing function routes response to TE
  or PS protocol stack based on operating service mode and AT command
  state.

DEPENDENCIES
  Routing is only performed if routing function is registered, otherwise
  response is sent to TE.
  
RETURN VALUE
  None
  
SIDE EFFECTS
  None
  
===========================================================================*/
extern void dsatrsp_send_response
(
  dsm_item_type *rsp_ptr,                 /* Ptr to response buffer to send */
  const boolean rearm_autodetect          /* Flag to rearm SIO autodetect */
);


/*===========================================================================

FUNCTION DSATRSP_FMT_RESULT_CODE

DESCRIPTION
  Places a formatted result code in the output buffer.  The type of
  result code is identified in the input parameter.  Formatting is
  based on whether or not the mode is verbose.  The result codes
  generated are based on mode specific result code generating
  functions.
  
DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
extern void dsatrsp_fmt_result_code
(
  dsat_result_enum_type result_code,   /* Result code                      */
  dsm_item_type **out_ptr_ptr          /* Location to store formatted
                                          response, output will NOT be NULL
                                          terminated                       */
);


/*===========================================================================

FUNCTION DSATRSP_SET_BASIC_RESULT_CODE

DESCRIPTION
  Produces and unformatted ITU-T V.25 ter basic result code and returns
  in DSM item.  Result code returned will be either numeric or verbose,
  based on input parameter.

DEPENDENCIES
  None
  
RETURN VALUE
  Pointer to DSM item containing unformatted basic result code.

SIDE EFFECTS
  None
  
===========================================================================*/
dsm_item_type *dsatrsp_set_basic_result_code
(
  dsat_result_enum_type result_code,    /* Basic result code               */
  boolean               verbose         /* verbose result code or numeric  */
);


/*===========================================================================

FUNCTION DSATRSP_SEND_CELL_RESULT

DESCRIPTION
  Sends ITU-T V.25 ter basic code or mode specific extended result code
  to TE or PS protocol stack based on response routing provided if ATQ
  is 0.

  If AT+CRC is 0, only the basic result codes are returned.
  The extended RING codes are translated to the basic code.
  Basic result codes are returned if ATQ is 0.

  Extended cellular result codes are returned if AT+CRC is 1.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void dsatrsp_send_cell_result
(
  dsat_result_enum_type result_code         /* Command result code */
);
/*===========================================================================

FUNCTION  DSAT_SEND_RESULT_CODE_EX

DESCRIPTION
  Sends ITU-T V.25 ter basic code or mode specific extended result code
  to TE or PS protocol stack based on response routing provided if ATQ
  is 0. Also, modifies ATCOP internal state variables.

  If AT+CRC is 0, only the basic result codes are returned.
  The extended RING codes are translated to the basic code.
  Basic result codes are returned if ATQ is 0.

  Extended cellular result codes are returned if AT+CRC is 1.

DEPENDENCIES
  None
  
RETURN VALUE
  None
  
SIDE EFFECTS
  May modify ATCOP internal state variables and SIO preprocessor mode.
===========================================================================*/
void dsat_send_result_code_ex
(
  dsat_result_enum_type result_code,         /* Command result code */
  ds3g_siolib_port_e_type port /*Port Id*/
);

/*===========================================================================

FUNCTION DSAT_SEND_UNSOLICITED_RESULT_CODE

DESCRIPTION
  Send unsolicited result code to TE.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void dsat_send_unsolicited_result_code
(
  dsat_result_enum_type result_code
);

/*===========================================================================

FUNCTION DSATRSP_SEND_CELL_RESULT_EX

DESCRIPTION
  Sends ITU-T V.25 ter basic code or mode specific extended result code
  to TE or PS protocol stack based on response routing provided if ATQ
  is 0.

  If AT+CRC is 0, only the basic result codes are returned.
  The extended RING codes are translated to the basic code.
  Basic result codes are returned if ATQ is 0.

  Extended cellular result codes are returned if AT+CRC is 1.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void dsatrsp_send_cell_result_ex
(
  dsat_result_enum_type result_code         /* Command result code */,
  ds3g_siolib_port_e_type port /*Port Id*/
);

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)

/*===========================================================================

FUNCTION  DSAT_SEND_CR_RESULT

DESCRIPTION
  Sends intermediate service reporting result code to TE or PS protocol
  stack based on response routing provided by response router.

  Should be called from mode specific protocol stack at time during
  connection negotiation that speed and quality of service is determined 
  and before compression and connect result code are sent.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void dsat_send_cr_result
(
  dsat_cr_service_e_type service
);
/*===========================================================================

FUNCTION  DSAT_SEND_CR_RESULT_EX

DESCRIPTION
  Sends intermediate service reporting result code to TE or PS protocol
  stack based on response routing provided by response router.

  Should be called from mode specific protocol stack at time during
  connection negotiation that speed and quality of service is determined 
  and before compression and connect result code are sent.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void dsat_send_cr_result_ex
(
  dsat_cr_service_e_type service,
  ds3g_siolib_port_e_type port 
);

#endif /* defined(FEATURE_WCDMA) || defined(FEATURE_GSM) */

/*===========================================================================
FUNCTION  DSATRSP_SEND_INTERMEDIATE_RESULT_CODE
DESCRIPTION
  Sends ITU-T V.25 ter basic code or mode specific extended result code
  to TE or PS protocol stack based on response routing provided if ATQ
  is 0. Also, modifies ATCOP internal state variables.  This function
  should be used when the result code to be sent is an intermediate code.
  That is, one that occurs before the final AT response is sent when 
  the call terminates such as with a T.30 FAX call which sends multiple
  AT result codes throughout the duration of the call.
  If AT+CRC is 0, only the basic result codes are returned.
  The extended RING codes are translated to the basic code.
  Basic result codes are returned if ATQ is 0.
  Extended cellular result codes are returned if AT+CRC is 1.
DEPENDENCIES
  None
  
RETURN VALUE
  None
  
SIDE EFFECTS
  May modify ATCOP internal state variables and SIO preprocessor mode.
===========================================================================*/
void dsatrsp_send_intermediate_result_code
(
  dsat_result_enum_type result_code         /* Command result code */
);

/*===========================================================================

FUNCTION  DSATRSP_SEND_RESULT_CODE

DESCRIPTION
  Sends ITU-T V.25 ter basic code or mode specific extended result code
  to TE or PS protocol stack based on response routing provided if ATQ
  is 0. Also, modifies ATCOP internal state variables.

  If AT+CRC is 0, only the basic result codes are returned.
  The extended RING codes are translated to the basic code.
  Basic result codes are returned if ATQ is 0.

  Extended cellular result codes are returned if AT+CRC is 1.

DEPENDENCIES
  None
  
RETURN VALUE
  None
  
SIDE EFFECTS
  May modify ATCOP internal state variables and SIO preprocessor mode.
===========================================================================*/
void dsatrsp_send_result_code
(
  dsat_result_enum_type result_code         /* Command result code */
);

/*===========================================================================

FUNCTION  DSATRSP_SEND_RESULT_CODE_EX

DESCRIPTION
  Sends ITU-T V.25 ter basic code or mode specific extended result code
  to TE or PS protocol stack based on response routing provided if ATQ
  is 0. Also, modifies ATCOP internal state variables.

  If AT+CRC is 0, only the basic result codes are returned.
  The extended RING codes are translated to the basic code.
  Basic result codes are returned if ATQ is 0.

  Extended cellular result codes are returned if AT+CRC is 1.

DEPENDENCIES
  None
  
RETURN VALUE
  None
  
SIDE EFFECTS
  May modify ATCOP internal state variables and SIO preprocessor mode.
===========================================================================*/
void dsatrsp_send_result_code_ex
(
  dsat_result_enum_type result_code,         /* Command result code */
  ds3g_siolib_port_e_type port /*Port Id*/
  
);

/*===========================================================================

FUNCTION  DSAT_SEND_DR_RESULT_EX

DESCRIPTION
  Sends intermediate data compression reporting result code to TE or
  PS protocol stack based on response routing provided by response
  router.

  Should be called from mode specific protocol stack when compression
  negotiation completed and before connect result code sent.
 
DEPENDENCIES
  None
  
RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void dsat_send_dr_result_ex
(
  dsat_dr_mode_e_type compression,          /*  Compression mode            */
  ds3g_siolib_port_e_type port /*Port Id*/
);

/*===========================================================================

FUNCTION  DSATRSP_SEND_DR_RESULT_EX

DESCRIPTION
  Sends intermediate data compression reporting result code to TE or
  PS protocol stack based on response routing provided by response
  router.

  Should be called from mode specific protocol stack when compression
  negotiation completed and before connect result code sent.
 
DEPENDENCIES
  None
  
RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void dsatrsp_send_dr_result_ex
(
  dsat_dr_mode_e_type compression,          /*  Compression mode            */
  ds3g_siolib_port_e_type port /*Port Id*/
);

/*===========================================================================

FUNCTION DSATRSP_REPORT_RATE_ON_CONNECT

DESCRIPTION
  This function is called by other modules to write the value to the current 
  rate variable dsat_report_rate_val. If dsat_x_val, set by the ATX command,
  is greater than 0 this rate will be reported with the next CONNECT result
  and during the call with any CONNECT result due to re-entry of online data
  mode. The dsat_report_rate_val variable will be reset to 0
  on the next call to dsat_sio_is_free(). 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Modifies the value of the dsat_report_rate_val.
===========================================================================*/
void dsatrsp_report_rate_on_connect
(
  dsat_connect_rate_s_type connect_rate   /* Rate to report with next CONNECT */
);

#ifdef FEATURE_DSAT_EXT_CLIENT_SUPPORT

/*===========================================================================

FUNCTION DSATRSP_SEND_CELL_RESULT_TO_EXT_CLIENT

DESCRIPTION
  This function is identical to dsatrsp_send_cell result but sends the result code
  only to external clients registered through SW API.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void dsatrsp_send_cell_result_to_ext_client
(
  dsat_result_enum_type result_code         /* Command result code */
);
#endif /* FEATURE_DSAT_EXT_CLIENT_SUPPORT*/

/*===========================================================================

FUNCTION DSATUTIL_FMT_NUM_VAL

DESCRIPTION
  This function formats the result of a value inquiry "?" for a normal
  numeric variable.  The return string is a null terminated 3 - digit
  decimal number with leading zeros.

DEPENDENCIES

RETURN VALUE
  Number of characters stored, not including the trailing null character.

SIDE EFFECTS

===========================================================================*/
extern unsigned int dsatutil_fmt_num_val
(
  const dsati_cmd_type *tab_entry, /*  entry in parse table  */
  byte *rb_ptr                     /*  point to buffer for result  */
);


/*===========================================================================

FUNCTION DSATUTIL_FMT_NUM_VAL_EX

DESCRIPTION
  This function formats the value of an extended format numeric
  variable.  The value may be formatted as the result of a READ
  "?" command, or in the form of a SET command for use in uploading
  the configuration.  For the READ command, the returned string is
  of the form:

      +NAME: <value>                            for a simple variable, or
      +NAME: <value1>, <value2>, ... , <valueN> for a compound variable.

  For the SET command, it is in the form

      +NAME=<value>;                            for a simple variable, or
      +NAME=<value1>, <value2>, ... , <valueN>; for a compound variable.


  The value will be in decimal, unless the parameter is specified with
  a HEX attribute in the parse table entry.  Leading zeros are suppressed.

  ".0" is added to the result for +FCLASS=2.0

  The formatted value is returned as a null terminated string.

DEPENDENCIES

RETURN VALUE
  Number of characters stored, not including the trailing null character.

SIDE EFFECTS

===========================================================================*/
extern unsigned int dsatutil_fmt_num_val_ex
(
  const dsati_cmd_type *tab_entry, /*  entry in parse table  */
  byte *rb_ptr,                    /*  point to buffer for result  */
  fmt_enum_type fmt_sw             /*  selects response or set fmt */
);


/*===========================================================================

FUNCTION DSATUTIL_FMT_NUM_RANGE

DESCRIPTION
  This function formats the result of a range inquiry "=?" command
  for a numeric variable.

  The formatted value is returned as a null terminated string.
DEPENDENCIES

RETURN VALUE
  Number of characters stored, not including the trailing null character.


SIDE EFFECTS

===========================================================================*/
extern unsigned int dsatutil_fmt_num_range
(
  const dsati_cmd_type *tab_entry, /*  entry in parse table  */
  byte *rb_ptr                     /*  point to buffer for result  */
);


/*===========================================================================

FUNCTION DSATUTIL_FMT_STRING_RANGE

DESCRIPTION
  This function formats the result of a range inquiry "=?" for a string
  variable.  The function always returns
      +NAME: (20,21,23-7E)
  which is the range of printable ASCII characters.

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/
extern void dsatutil_fmt_string_range
(
  const dsati_cmd_type *tab_entry,       /*  entry in parse table  */
  byte *rb_ptr,                          /*  pointer to buffer for result  */
  const dsat_string_item_type *range_ptr /*  pointer to range  */
);


/*===========================================================================

FUNCTION DSATUTIL_FMT_STRING_VAL

DESCRIPTION
  This function formats the value of an extended format string
  variable.  The value may be formatted as the result of a READ
  "?" command, or in the form of a SET command for use in uploading
  the configuration.  For the READ command, the returned string is
  of the form:

      +NAME: <string>

  For the SET command, it is in the form

      +NAME=<value>;

  The formatted value is returned as a null terminated string.

DEPENDENCIES

RETURN VALUE
  Number of characters stored, not including the trailing null character.

SIDE EFFECTS

===========================================================================*/
extern unsigned int dsatutil_fmt_string_val
(
  const dsati_cmd_type *tab_entry, /*  entry in parse table  */
  byte *rb_ptr,                    /*  point to buffer for result  */
  fmt_enum_type fmt_sw             /*  selects response or set fmt */
);


/*===========================================================================

FUNCTION DSATUTIL_FMT_MIXED_PARAM_VAL

DESCRIPTION
  This function formats the values of a mixed parameter.
  The values may be formatted as the result of a READ "?" command,
  or in the form of a SET command for use in uploading the configuration.
  For the READ command, the returned string is of the form:

      +NAME: <p1>,<p2>,...

  For the SET command, it is in the form

      +NAME=<value>;

  The formatted value is returned as a null terminated string.

DEPENDENCIES

RETURN VALUE
  Number of characters stored, not including the trailing null character.

SIDE EFFECTS

===========================================================================*/
extern unsigned int dsatutil_fmt_mixed_param_val
(
  const dsati_cmd_type *tab_entry,      /*  entry in parse table  */
  const dsm_item_type *res_buff_ptr,    /*  Place to put response       */
  fmt_enum_type fmt_sw                  /*  selects response or set fmt */
);


/*===========================================================================

FUNCTION DSATUTIL_PUT_NUM_ARG

DESCRIPTION
  This function checks the value and number of numeric arguments for
  a command, and, if they are correct, stores the value(s) in the
  parameter.  If the range or number of values is out of bounds, the
  global "dsat_result" will be set to ERROR.

DEPENDENCIES


RETURN VALUE
  returns a boolean. Returns FALSE if there is an error else returns
  TRUE

SIDE EFFECTS
  Describe here any side effects that may occur as a result of calling this
  function. This includes such things as global vars that are modified, etc.

===========================================================================*/
extern boolean dsatutil_put_num_arg
(
  const dsati_cmd_type *tab_entry,      /*  entry in parse table        */
  const tokens_struct_type *tok_ptr     /*  Command tokens from parser  */
);

/*===========================================================================

FUNCTION DSATUTIL_MIXED_PARAM_SET_NUM_ARG

DESCRIPTION
  This function checks the value of a string argument for
  a command in the def list.  It determines the string argument's index 
  into the string list.  Use this index to set the val_ptr.

DEPENDENCIES


RETURN VALUE
  returns a param_srch_enum_type:
    PARAM_MISS - Not found in values list 
    PARAM_HIT  - Found in values list            
    PARAM_NULL - No parameter specified          

SIDE EFFECTS
  None

===========================================================================*/
extern param_srch_enum_type dsatutil_mixed_param_set_num_arg
(
  dsat_num_item_type * val_num_ptr, /* the index number to be returned */
  const def_list_type * def_list_ptr, /* the def list */
  const byte * arg,                   /* the token argument */
  uint32 attrib                       /* the attrib of the MIXED_PARAM */
);


/*===========================================================================

FUNCTION DSATUTIL_PUT_STRING_ARG

DESCRIPTION
  This checks an extended format string WRITE command for correct
  syntax and string length.  If everthing is OK, the string is copied
  into the specified parameter.  Otherwise, the global dsat_result" is
  set to ERROR. Non space characters outside of the '"' are considered
  an error.

DEPENDENCIES


RETURN VALUE
  returns a boolean. Returns FALSE if there is an error else returns
  TRUE

SIDE EFFECTS


===========================================================================*/
extern boolean dsatutil_put_string_arg
(
  const dsati_cmd_type *tab_entry,      /*  entry in parse table        */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  put_string_enum_type action           /*  Store or concatenate  */
);


/*===========================================================================

FUNCTION DSATUTIL_ITOA

DESCRIPTION
  This function converts an integer to ASCII, using a specified
  radix. The resulting string is null terminated.

DEPENDENCIES
  The function does not work correctly if r < 2 or r > 36.
  (r is the radix argument.)

  The length of the returned string depends on the value and the
  radix.  The worst case is the value 0xFFFF and a radix of 2,
  which returns 17 bytes (16 one's and the null terminator.)
  The array pointed to by 's' must be large enough for the returned
  string.

RETURN VALUE
  A pointer to the null terminator is returned.

SIDE EFFECTS

===========================================================================*/
extern byte * dsatutil_itoa
(
  uint32 v,         /*  Value to convert to ASCII     */
  byte *s,        /*  Pointer to string for result  */
  uint16 r          /*  Conversion radix              */
);


/*===========================================================================

FUNCTION DSATUTIL_XITOA

DESCRIPTION
  Formats an unsigned long to hexadecimal.
  Supplies leading zeros.

DEPENDENCIES

RETURN VALUE
  A pointer to the null terminator is returned.


SIDE EFFECTS

===========================================================================*/
unsigned char * dsatutil_xitoa
(
  unsigned long val,  /*  value to convert  */
  byte * a_ptr        /*  pointer to null terminator of result  */
);


/*===========================================================================

FUNCTION DSATUTIL_STRCMP_IG_SP_CASE

DESCRIPTION
  This function compares two null terminated strings, ignoring case
  and spaces.

DEPENDENCIES
  None.

RETURN VALUE
  Returns   0 if the strings are identical, except for case and spaces,
            otherwise, it returns the difference between the first
            characters to disagree. The difference is first arg minus
            second arg.  Lower case letters are converted to upper
            case before the difference is taken.

SIDE EFFECTS
  None.

===========================================================================*/
extern int dsatutil_strcmp_ig_sp_case
(
  const byte *s1,             /*  Pointer to string 1 */
  const byte *s2              /*  Pointer to string 2 */
);

/*===========================================================================

FUNCTION DSATUTIL_STRNCMP_IG_SP_CASE

DESCRIPTION
  This function compares two strings for <n> characters, ignoring case.
  Lower case letters are converted to upper case before the 
  difference is taken.


DEPENDENCIES
  None.

RETURN VALUE
  Returns   0 if the strings are identical till <n> characters, except for 
            case , otherwise, it returns the difference between 
            the first characters to disagree. The difference is first arg 
            minus second arg.

SIDE EFFECTS
  None.

===========================================================================*/
extern int dsatutil_strncmp_ig_sp_case
(
  const byte *s1,             /*  Pointer to string 1 */
  const byte *s2,              /*  Pointer to string 2 */
  uint32      n               /* Number of characters to compare */
);


/*===========================================================================

FUNCTION DSATUTIL_ATOI

DESCRIPTION
  This function converts an ASCII string to an integer, using a specified
  radix.  Spaces are ignored. Letters used for radices above 10 may be of
  either case.

DEPENDENCIES


RETURN VALUE
  Returns 0 for a correct conversion,
          1 if no argument was found,
          2 if a character or the total value is out of range.

SIDE EFFECTS

===========================================================================*/
extern atoi_enum_type dsatutil_atoi
(
  dsat_num_item_type *val_arg_ptr,      /*  value returned  */
  const byte *s,                        /*  points to string to eval  */
  unsigned int r                        /*  radix */
);

/*===========================================================================
FUNCTION DSATUTIL_STRIP_QUOTES_OUT

DESCRIPTION
  This function strips the quotes from the quoted_str, the results
  are in out_str.  str_len is the length of the out_str string.

  The quoted_str is a null terminated string.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if the action succeeds.
  FALSE if the action fails.
        The reason could be 
        1. the str_len is less than the length of
           quoted_str -2, ie the out_str is too small to hold the result.
        2. the quoted_str is not quoted

SIDE EFFECTS
  None
===========================================================================*/
extern boolean dsatutil_strip_quotes_out
( 
  const byte * quoted_str, byte * out_str, word str_len
);



/*===========================================================================

FUNCTION DSATUTIL_CONVERT_TUPLE

DESCRIPTION
  This function checks for the validity of known tuple formats and converts
  between string and number representations.  For IPv6 addresses, the number
  pointer is assumed to be an array of uint32 values.
  
  Tuples supported:
     IPv4       range: 0.0.0.0 - 255.255.255.255
     IPv4_254   range: 0.0.0.0 - 254.254.254.254
     PORT       range: 0.0 - 65535.65535
     TMASK      range: 0.0 - 255.255
     IPv6       range: 0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0 -
                       255.255.255.255.255.255.255.255.
                       255.255.255.255.255.255.255.255

DEPENDENCIES
  For IPv6 addresses, the number pointer is assumed to be an array[4] of
  uint32 values.

RETURN VALUE
  returns an flag that describes the result of execution.
  possible values:
    TRUE : if validation successful
    FALSE : if error occurred

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatutil_convert_tuple 
(
   tuple_convert_enum_type mode,             /* Mode of conversion      */
   tuple_type_enum_type ttype,               /* Type of tuple           */
   uint32* num_ptr,                          /* Interger representation */
   byte* buf_ptr,                            /* String representation   */
   uint8 buf_len,                            /* String length           */
   dsat_ip_mask_enum_type ip_mask            /* IP or Mask Convertion   */
);


/*===========================================================================

FUNCTION DSATUTIL_RESET_PASSWORD

DESCRIPTION
  Clear the password variable or specified length.
  The password string is set to 0xFF, null terminated.  

  The provided 'length' should be the total length of the buffer pointed to 
  by 'password' including allocation for the trailing NULL

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void dsatutil_reset_password
(
  byte * password,            /* String containing password */
  word length                 /* Maximum length of string */
);

/*===========================================================================

FUNCTION DSATUTIL_DSM_NEW_BUFFER

DESCRIPTION
  This function is a wrapper for dsm_new_buffer() API.

DEPENDENCIES
  None

RETURN VALUE
  A pointer to a 'dsm_item_type'

SIDE EFFECTS
  None

===========================================================================*/
dsm_item_type *dsatutil_dsm_new_buffer
(
  dsm_mempool_id_type  pool_id,
  const char          *filename,
  uint32               line,
  boolean              graceful
);

/*===========================================================================

FUNCTION DSATUTIL_DSM_PUSHDOWN_TAIL

DESCRIPTION
  This function is a wrapper for dsm_pushdown_tail() API.

DEPENDENCIES
  None

RETURN VALUE
  The number of bytes that were pushed on to the tail end of the last item.
  Note this can be less than the requested amount.

SIDE EFFECTS
  If the pushdown requires an additional data item, the pkt_ptr field of the 
  last item in the passed packet chain may be changed.

===========================================================================*/
void dsatutil_dsm_pushdown_tail
(
  dsm_item_type       **pkt_head_ptr,
  const void           *buf,
  uint16                size,
  dsm_mempool_id_type   pool_id,
  const char           *filename,
  uint32                line,
  boolean               graceful
);

/*===========================================================================

FUNCTION DSATUTIL_DSM_CREATE_PACKET

DESCRIPTION
  This function creates a DSM packet.

DEPENDENCIES
  None

RETURN VALUE
  A pointer to a 'dsm_item_type'

SIDE EFFECTS
  None

===========================================================================*/
dsm_item_type *dsatutil_dsm_create_packet
(
  const void *buf,
  uint16      size,
  const char *filename,
  uint32      line,
  boolean     graceful
);

/*===========================================================================

FUNCTION DSATUTIL_APPEND_DSM_ITEM

DESCRIPTION
  This function checks the buffer length against the passed threshold
  value.  If the threshold is exceeded, a new DSM item is requested and
  appended to the buffer chain.  An error is generated if no dsm_items
  are available. A pointer to the new DSM item is returned as a parameter;
  it equals the input DSM item if no action taken.

DEPENDENCIES
  None

RETURN VALUE
  Pointer to current (appended) DSM item

SIDE EFFECTS
  A new dsm_item may be allocated.
  
===========================================================================*/
extern dsm_item_type * dsatutil_append_dsm_item
(
  dsm_item_type * dsm_item_ptr,          /* Pointer to head DSM item */
  word threshold                         /* Threshold length of buffer */
);


/*===========================================================================

FUNCTION  DSATI_TO_SIO_LOW_CMD_HANDLER

DESCRIPTION
  This is the handler function for DS_AT_TO_SIO_LOW_CMD cmd.

DEPENDENCIES
  None

RETURN VALUE
  DSAT_ASYNC_CMD: if the dsat_to_sio low hanlder succeeds.
  DSAT_ERROR    : if the dsat_to_sio low hanlder fails.

SIDE EFFECTS
  None
===========================================================================*/
dsat_result_enum_type dsati_to_sio_low_cmd_handler
(
  ds_cmd_type         * cmd_ptr              /* DS Command pointer         */
);

/*===========================================================================

FUNCTION DSATUTIL_SWAPN_WORD_MSB_LSB

DESCRIPTION
  This function is used to swap the msb and lsb of the given word array
DEPENDENCIES

RETURN VALUE
  Returns an enum that describes the result of the execution.
  Possible values:
    FALSE : if there was any problem in execution
    TRUE : if it is a success.

SIDE EFFECTS
  None
  
===========================================================================*/
boolean dsatutil_swapn_word_msb_lsb
(
  const word * source_arr ,                       /* Pointer to Source array */
  word * dest_arr,                          /* Pointer to Destination array */
  dsat_num_item_type source_length,         /* No of words in source */
  dsat_num_item_type dest_length            /* No of words in destination */
);

/*===========================================================================

FUNCTION DSATUTIL_REVERSEN_WORD

DESCRIPTION
  This function reverses the given word array by the specified number of words
DEPENDENCIES
  None.
RETURN VALUE
  Returns an enum that describes the result of the execution.
  Possible values:
    FALSE : if there was any problem in execution
    TRUE : if it is a success.

SIDE EFFECTS
  None
  
===========================================================================*/

boolean dsatutil_reversen_word
(
  word * wrd_arry,                        /* Word Array to be reversed */
  dsat_num_item_type len                 /* Number of words to be reversed */
);

/*===========================================================================

FUNCTION DSATUTIL_REVERSEN_BYTE

DESCRIPTION
  This function reverses the given byte array by the specified number of bytes.

DEPENDENCIES
  None.

RETURN VALUE
  Returns an enum that describes the result of the execution.
  Possible values:
    FALSE : if there was any problem in execution
    TRUE : if it is a success.

SIDE EFFECTS
  None
  
===========================================================================*/
boolean dsatutil_reversen_byte
(
  byte * byte_arry,                        /* Byte Array to be reversed */
  dsat_num_item_type len                 /* Number of bytes to be reversed */
);

/*===========================================================================

FUNCTION DSATUTIL_VALIDATE_DIAL_CHAR

DESCRIPTION
  This function examines the passed dial string charater and returns a
  validation result.  It is up to the caller to act upon the result as
  approppriate within its context.

  Refer to 3GPP TS 27.007 section 6.2 for details on the valid dialing 
  digits.  The supported set are summarized as follows:
    0 - 9 
    A - C
    + * ; #           
    D - E             (ETSI SMS PDU mode)
    W , $ T P ! @ D   (accepted but flagged for strip)
  
DEPENDENCIES
  None

RETURN VALUE
  Returns enum value indicating validation result.

SIDE EFFECTS
  None

===========================================================================*/
dsat_dial_val_e_type dsatutil_validate_dial_char
(
  const byte * c_ptr                 /*  Input character   */
);


/*===========================================================================
FUNCTION DSATUTIL_SRCH_LIST

DESCRIPTION
  This function searchs a list of strings, and returns the index to the
  item found.  (The first item index is 0).  Index set to NULL if 
  parameter is null, as with empty commas entered, 

  If ignore_quote is true, then srch_list will find quoted args in 
  unquoted lists.  For instance, \"ME\" will find itself as the first 
  item in list ME, MT, SM

DEPENDENCIES
  None

RETURN VALUE
  PARAM_HIT if found.
  PARAM_MISS if no match.
  PARAM_NULL if no parameter specified.

SIDE EFFECTS
  None
===========================================================================*/
param_srch_enum_type dsatutil_srch_list
(
  dsat_num_item_type *val_arg_ptr,  /*  value returned  */
  const byte l[][8],
  const byte * arg,
  action_quote_e_type action_quote
);

/*===========================================================================

FUNCTION DSATUTIL_STRCMP_PKT_DIAL_STR

DESCRIPTION
  This function performs a lookup in the Packet call dial string registry
  to see if the input string prefix is found.  Further validation is
  done base on call type mode.   Input mode may be overridden for some
  call types.  Returns flag indicating valid packet call type detected.

DEPENDENCIES
  Lookup table must have been initialized from NV prior to use.

RETURN VALUE
  Returns TRUE if match in lookup table, FALSE otherwise

SIDE EFFECTS
  None.

===========================================================================*/
boolean dsatutil_strcmp_pkt_dial_str
(
  byte               *orig_dial_str,   /*  Pointer to the dial string   */
  dsati_mode_e_type  *mode_ptr         /*  Pointer to Mode of Operation */
);


#ifdef FEATURE_DSAT_ETSI_MODE
#ifdef FEATURE_DSAT_ETSI_DATA
/*===========================================================================

FUNCTION DSATUTIL_CHECK_OPT_DIAL_STR

DESCRIPTION
  Parses a Packet data dial string. Only options {*<cid>,***<cid>}
  are supported.  If cid is specified but not defined (via +CGDCONT),
  an error is returned.  The cid is passed back to caller.
  
  Validation is skipped if packet call mode override is set, indicating
  a non-ETSI mode dial string is being processed in ETSI mode.

DEPENDENCIES
  The calling function is responsible for cid parameter initialization.

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command

SIDE EFFECTS
  None

===========================================================================*/

dsat_result_enum_type dsatutil_check_opt_dial_str
(
  const byte * in_ptr,                /*  Input string, null terminated   */
  byte * cid                    /*  Connection ID  */
);
#endif /* FEATURE_DSAT_ETSI_DATA */

/*===========================================================================

FUNCTION DSATUTIL_FILL_MODIFIER_INFO

DESCRIPTION
  This function fill out the struct pointed by mod_info_ptr according
  to attribute of dial_ptr and <index> and <info> of +CCUG command.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command

SIDE EFFECTS
  None.

===========================================================================*/
dsat_result_enum_type dsatutil_fill_modifier_info
(
  /* input, contains properties of dial string */
  const dsati_dial_str_type     * dial_ptr,

  /* output, contains modifier info of the dial string */
  dsat_dial_modifier_info_type  * mod_info_ptr
);
#endif /* FEATURE_DSAT_ETSI_MODE */


/*===========================================================================

FUNCTION DSATUTIL_INIT_PKT_DIAL_STRING_FROM_NV

DESCRIPTION
  This function reads Packet Dial Sting lookup table from the NV during
  initialization. If these items were never written in NV, the values are
  made as NULL.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void dsatutil_init_pkt_dial_string_from_nv 
( 
  sys_modem_as_id_e_type  subs_id
);
/*===========================================================================

FUNCTION DSAT_FINISHED_ATH_CMD

DESCRIPTION
  Resets the state variable. Continues processing the AT command line.
  Called by dsat_send_result_code(). Used when we are processing ATH cmd.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Does not resutn to CS data handler tillwe finifh processing the AT command
  line.

===========================================================================*/
void dsat_finished_ath_cmd 
(
 dsat_result_enum_type result_code         /* Command result code */
);


/*===========================================================================

FUNCTION DSATCMDP_SET_RESTRICTED_MODE

DESCRIPTION
  This function sets the ATCOP restricted command mode based on events
  from GSDI.  While in restricted command mode, the UE responds to a
  limitied set of comamnds indicated by the RESTRICTED attribute on
  the command table.  Restricted mode applies only to ETSI mode.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  May trigger events to ATCOP clients.

===========================================================================*/
void dsatcmdp_set_restricted_mode
(
  boolean             mode,  /* new mode setting */
  dsat_apps_id_e_type apps_id
);

/*===========================================================================
FUNCTION DSATUTIL_GET_NV_ITEM 

DESCRIPTION
  Get an item from the nonvolatile memory.  

RETURN VALUE
  The NV return code.
  
DEPENDENCIES
  NV task is expected to be available.

SIDE EFFECTS
  None

===========================================================================*/
nv_stat_enum_type dsatutil_get_nv_item 
(
  nv_items_enum_type  item,           /* Which item */
  nv_item_type       *data_ptr        /* Pointer to space for item */
);

/*===========================================================================
FUNCTION DSATUTIL_GET_NV_ITEM_PER_SLOT

DESCRIPTION
  Get an item from the nonvolatile memory per slot.

RETURN VALUE
  The NV return code.

DEPENDENCIES
  NV task is expected to be available.

SIDE EFFECTS
  None

===========================================================================*/
nv_stat_enum_type dsatutil_get_nv_item_per_slot
(
  nv_items_enum_type        item,           /* Which item */
  nv_item_type             *data_ptr,       /* Pointer to space for item */
  mmgsdi_slot_id_enum_type  slot_id         /* Slot ID */
);

/*===========================================================================
FUNCTION DSATUTIL_GET_NV_ITEM_PER_SUBS 

DESCRIPTION
  Get an item from the nonvolatile memory per subs.  

RETURN VALUE
  The NV return code.
  
DEPENDENCIES
  NV task is expected to be available.

SIDE EFFECTS
  None

===========================================================================*/
nv_stat_enum_type dsatutil_get_nv_item_per_subs
(
  nv_items_enum_type      item,           /* Which item */
  nv_item_type            *data_ptr,        /* Pointer to space for item */
  sys_modem_as_id_e_type  subs_id
);

/*===========================================================================
FUNCTION DSATUTIL_PUT_NV_ITEM 

DESCRIPTION
  Get an item from the nonvolatile memory.  

RETURN VALUE
  The NV return code.
  
DEPENDENCIES
  NV task is expected to be available.

SIDE EFFECTS
  None

===========================================================================*/
nv_stat_enum_type dsatutil_put_nv_item 
(
  nv_items_enum_type  item,           /* Which item */
  nv_item_type       *data_ptr        /* Pointer to space for item */
);

/*===========================================================================
FUNCTION DSATUTIL_PUT_NV_ITEM_PER_SUBS 

DESCRIPTION
  Put an item from the nonvolatile memory per subs.  

RETURN VALUE
  The NV return code.
  
DEPENDENCIES
  NV task is expected to be available.

SIDE EFFECTS
  None

===========================================================================*/
nv_stat_enum_type dsatutil_put_nv_item_per_subs 
(
  nv_items_enum_type      item,           /* Which item */
  nv_item_type            *data_ptr,        /* Pointer to space for item */
  sys_modem_as_id_e_type  subs_id
);

/*===========================================================================

FUNCTION DSATUTIL_CONVERT_ALPHA_GSM_WITH_AT_TO_ALPHA_GSM

DESCRIPTION
This function converts a GSM string which could contain a GSM '@' character (0x00)
to a GSM string with replacing '@' (0x00) to a special character (0xe6) which is known to 
ATCOP module alone. 

DEPENDENCIES
  This function should be invoked when the string (input to ATCOP) is comming from other
  than TE (eg: network or some gsdi modules etc)

RETURN VALUE
  possible values:
    TRUE: Successsfull conversion 
    FALSE: could not convert the string. 
SIDE EFFECTS
  None
  
===========================================================================*/
boolean dsatutil_convert_alpha_gsm_with_at_to_alpha_gsm
(
  const char *in_str, 
  char *out_str,
  uint16 in_len
);

/*===========================================================================

FUNCTION DSATUTIL_CONVERT_CHSET

DESCRIPTION
  This function performs conversion of a given null terminated string in one 
  supported character set to another supported character set, writing it into
  an output buffer provided and null terminating it. It will handle 
  inconvertible characters as indicated by drop_inconvertible, simply not 
  copying them if TRUE or stopping conversion and returning NULL if FALSE. 
  The character with value 0x00 is not supported in the GSM 7 bit or IRA 
  alphabet as NULL is used for terminating strings, neither will the function 
  output this character value except as a termination. When converting from 
  HEX/UCS2 for instance this counts as an inconvertible character.
  It will stop the conversion if indicated maximum output buffer length is 
  reached.

DEPENDENCIES
  None.

RETURN VALUE
  possible values:
    NULL : if drop_inconvertible FALSE and conversion fails
    pointer to terminating NULL in output buffer : if conversion successful

SIDE EFFECTS
  None
  
===========================================================================*/
char *dsatutil_convert_chset
(
  const char *in_str, 
  dsati_chset_type in_chset,
  const char *out_str,
  dsati_chset_type out_chset,
  uint16 out_max,
  boolean drop_inconvertible
);

/*===========================================================================

FUNCTION DSATUTIL_CONVERT_GSM_UCS2_TO_PBM_UCS2

DESCRIPTION
  This function performs the special conversion of one of the three GSM UCS2
  codings specified in 3GPP GSM TS 11.11 Annex B into PBM UCS2 format.  Sets 
  the terminating NULL (0x0000) character if there is enough room in out_ary
  for it; if there isn't then no error is thrown.

DEPENDENCIES
  None.

RETURN VALUE
  NULL if conversion failed, otherwise pointer to output array (out_ary 
  argument)

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_cme_error_e_type dsatutil_convert_gsm_ucs2_to_pbm_ucs2
(
  const char  *in_str,
  uint16      *out_ary,
  uint16       out_max /* size of the above array (in uint16's, NOT bytes) */
);

/*===========================================================================

FUNCTION dsatutil_ucs2_strlen

DESCRIPTION
  This function counts the number of UCS2 characters in a string.
  The  string must be terminated by 0x0000.

DEPENDENCIES
  None

RETURN VALUE
  The number of UCS2 characters in the given string.

SIDE EFFECTS
  None.

===========================================================================*/
uint16 dsatutil_ucs2_strlen
(
  const char* str                 /*  Unicode character string  */
);

/*===========================================================================

FUNCTION DSATUTIL_DISPATCH_TIMER_HANDLER

DESCRIPTION
  This function dispatches the common timer expiry event to the
  correct timer handler.  If the timer is not found, an error message
  is generated.

DEPENDENCIES
  None

RETURN VALUE
  Returns an enum that describes the result of the command execution.
  Possible values:
    DSAT_ASYNCH_EVENT : asynch event has been dispatched

SIDE EFFECTS
  None.

===========================================================================*/ 
dsat_result_enum_type dsatutil_dispatch_timer_handler
(
  ds_cmd_type         * cmd_ptr              /* DS Command pointer         */
);

/*===========================================================================

FUNCTION DSATUTIL_INIT_OS_TIMERS

DESCRIPTION
  This function registers timers with the opeating system.  Starting
  and clearing timers is done in individual command handlers.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void dsatutil_init_timers_ex ( void );

/*===========================================================================

FUNCTION DSATUTIL_INIT_TIMERS

DESCRIPTION

  This function registers timers with the opeating system.  Starting
  and clearing timers is done in individual command handlers.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void dsatutil_init_timers ( void );

/*===========================================================================

FUNCTION DSATUTIL_DEINIT_TIMERS

DESCRIPTION
  This function deinitializes timers.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void dsatutil_deinit_timers ( void );

/*===========================================================================

FUNCTION dsatutil_strtok

DESCRIPTION
  This function is used to return the tokens in tok_str string passed

DEPENDENCIES

RETURN VALUE
  Returns an enum that describes the result of the execution.
  Possible values:
    DSAT_ERROR : if there was any problem in execution
    DSAT_OK : if it is a success.

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatutil_strtok
(
  const byte *str,
  uint32 str_len,
  byte delimiter,
  byte *tok_str
);

/*===========================================================================

FUNCTION dsatutil_is_date_valid

DESCRIPTION
  This function is used to check whether the year/day/month 
  combination is correct or not

DEPENDENCIES

RETURN VALUE
  Returns an enum that describes the result of the execution.
  Possible values:
    FALSE : if there was any problem in execution
    TRUE : if it is a success.

SIDE EFFECTS
  None
  
===========================================================================*/
int dsatutil_is_date_valid
(
  int year, 
  int month, 
  int day
);

/*===========================================================================

FUNCTION  DSATCMDP_INIT

DESCRIPTION
 This Function will intiate ATcop intialization.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Initializes ATCOP internal variables and queues.

===========================================================================*/
void dsatcmdp_init( void );

/*===========================================================================

FUNCTION  DSATCMDP_DEINIT

DESCRIPTION
  This function deinitializes ATcoP.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsatcmdp_deinit( void );

/*===========================================================================

FUNCTION  DSATCMDP_INIT_SIO_INFO

DESCRIPTION
  This function initializes the SIO info for a given port ID if not
  already initialized.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

dsat_sio_info_s_type *dsatcmdp_init_sio_info
(
  ds3g_siolib_port_e_type port_id
);

/*===========================================================================

FUNCTION  DSATCMDP_IS_SIO_INFO_INITED

DESCRIPTION
  This function returns if the SIO info is initialized for a given port ID.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

boolean dsatcmdp_is_sio_info_inited
(
  ds3g_siolib_port_e_type port_id
);

/*===========================================================================

FUNCTION  DSATCMDP_GET_SIO_INFO_PTR

DESCRIPTION
  This function returns the SIO info pointer for a given port ID.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

dsat_sio_info_s_type *dsatcmdp_get_sio_info_ptr
(
  ds3g_siolib_port_e_type port_id
);

/*===========================================================================

FUNCTION DSAT_PROCESS_CMD_LINE

DESCRIPTION
  This function parses the AT command line, which may contain multiple
  commands, and processes the commands. Command line does not include
  leading "AT" and is NULL terminated.  Command processing is dependent on
  AT mode: command, online data, or online commmand.  

  Command response and command line result codes are generated.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Token queue is emptied.

===========================================================================*/
extern void dsat_process_cmd_line
(
  dsat_mode_enum_type at_state,     /* Command, online data, or online
                                           command */
  byte *cmd_line_ptr                    /* Pointer to NULL terminated
                                           command line. */
);

/*===========================================================================

FUNCTION DSATCMDP_PROCESS_CMD_LINE

DESCRIPTION
  This function parses the AT command line, which may contain multiple
  commands, and processes the commands. Command line does not include
  leading "AT" and is NULL terminated.  Command processing is dependent on
  AT mode: command, online data, or online commmand.  

  Command response and command line result codes are generated.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Token queue is emptied.

===========================================================================*/
extern void dsatcmdp_process_cmd_line
(
  dsat_mode_enum_type at_state,     /* Command, online data, or online
                                           command */
  byte *cmd_line_ptr                    /* Pointer to NULL terminated
                                           command line. */
);

/*===========================================================================

FUNCTION DSATCMDP_TRACK_ASYNC_CMD

DESCRIPTION
  Asynchronous ATCOP command tracking. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsatcmdp_track_async_cmd(void);

/*===========================================================================

FUNCTION DSAT_PROCESS_ASYNC_CMD

DESCRIPTION
  Asynchronous ATCOP command handler.  Processes asynchronous ATCOP commands
  received by data services task dispatcher.  Function looks up commands in
  mode specific command table.  Command processing function corresponding to
  current command and operating mode is called from table. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void dsat_process_async_cmd
(
  ds_cmd_type *cmd_ptr          /* Pointer to command */
);
/*===========================================================================

FUNCTION DSATCMDP_PROCESS_ASYNC_CMD

DESCRIPTION
  Asynchronous ATCOP command handler.  Processes asynchronous ATCOP commands
  received by data services task dispatcher.  Function looks up commands in
  mode specific command table.  Command processing function corresponding to
  current command and operating mode is called from table. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsatcmdp_process_async_cmd
(
  ds_cmd_type           *cmd_ptr,    /* Pointer to command */
  dsat_result_enum_type  result_code
);

/*===========================================================================

FUNCTION DSAT_PROCESS_ASYNC_SIGNAL

DESCRIPTION
  Asynchronous ATCOP REX signal handler.  Processes asynchronous ATCOP
  signals received by data services task dispatcher.  Function looks up
  signals in mode specific signal table.  Signal processing function
  corresponding to current signal and operating mode is called from table. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void dsat_process_async_signal
(
  rex_sigs_type set_sigs            /* ATCOP signals that are set */
);

/*===========================================================================

FUNCTION DSATCMDP_PROCESS_ASYNC_SIGNAL

DESCRIPTION
  Asynchronous ATCOP REX signal handler.  Processes asynchronous ATCOP
  signals received by data services task dispatcher.  Function looks up
  signals in mode specific signal table.  Signal processing function
  corresponding to current signal and operating mode is called from table. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsatcmdp_process_async_signal
(
  dsat_result_enum_type result_code
);

/*===========================================================================

FUNCTION DSAT_SIO_IS_BUSY

DESCRIPTION
  This function is used by SIOLib to tell ATCoP that SIO is busy now and 
  Mode Specific Handlers are in charge of SIO.

  This is called when Mode Specific Handlers register their SIO Handlers.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  ATCOP stops sending the AT command results to the TE. It will resume
  its normal operatioon only when dsat_sio_is_free() is called.

===========================================================================*/
void dsat_sio_is_busy
(
  ds3g_siolib_port_e_type port_id
);

/*===========================================================================

FUNCTION DSATCMDP_SIO_IS_BUSY

DESCRIPTION
  This function is used by SIOLib to tell ATCoP that SIO is busy now and 
  Mode Specific Handlers are in charge of SIO.

  This is called when Mode Specific Handlers register their SIO Handlers.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  ATCOP stops sending the AT command results to the TE. It will resume
  its normal operatioon only when dsat_sio_is_free() is called.

===========================================================================*/
void dsatcmdp_sio_is_busy
(
  ds3g_siolib_port_e_type port_id
);

/*===========================================================================

FUNCTION DSAT_SIO_IS_FREE

DESCRIPTION
  This function is used by SIOLib to tell ATCoP that SIO is free now. ATCoP
  Can now resume its normal operation.

  This is called when Mode Specific Handlers de-register their SIO Handlers.
  i.e. when the serial port is no longer in a call.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  In ETSI mode, the queued SMS indications are flushed.

===========================================================================*/
void dsat_sio_is_free
(
  ds3g_siolib_port_e_type port_id
);

/*===========================================================================

FUNCTION DSATCMDP_SIO_IS_FREE

DESCRIPTION
  This function is used by SIOLib to tell ATCoP that SIO is free now. ATCoP
  Can now resume its normal operation.

  This is called when Mode Specific Handlers de-register their SIO Handlers.
  i.e. when the serial port is no longer in a call.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  In ETSI mode, the queued SMS indications are flushed.

===========================================================================*/
void dsatcmdp_sio_is_free
(
  ds3g_siolib_port_e_type port_id
);

/*===========================================================================
FUNCTION  DSATCMDP_SEND_RESPOSE_TO_TE

DESCRIPTION
  If ATCOP gets an unsolicited asynchronous event, this function formats 
  the data that needs to be sent to TE.  The formatting flag controls
  response formatting and identifies if header, trailer, both header and
  trailer, or no header and trailer should be added to the response. 
  Note this routine is intended for use in asynchronous events only.

DEPENDENCIES
  None

RETURNS
  Returns an enum that describes the result of the execution.
  Possible values:
    DSAT_ASYNC_EVENT : if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatcmdp_send_respose_to_te
(
  dsm_item_type * res_buff_ptr,        /* Raw response buffer */
  dsat_rsp_enum_type type_of_rsp       /* Formatting flag     */
);

/*===========================================================================
FUNCTION  DSATCMDP_SEND_RESPOSE_TO_TE_EX

DESCRIPTION
  If ATCOP gets an unsolicited asynchronous event, this function formats 
  the data that needs to be sent to TE.  The formatting flag controls
  response formatting and identifies if header, trailer, both header and
  trailer, or no header and trailer should be added to the response. 
  Note this routine is intended for use in asynchronous events only.

DEPENDENCIES
  None

RETURNS
  Returns an enum that describes the result of the execution.
  Possible values:
    DSAT_ASYNC_EVENT : if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatcmdp_send_respose_to_te_ex
(
  void               *buf_ptr,
  size_t              buf_len,
  dsat_rsp_enum_type  type_of_rsp       /* Formatting flag     */
);

/*===========================================================================
FUNCTION DSATCMDP_RMSM_DONE_HANDLER

DESCRIPTION
  This function is called by Mode Handler when commands processing
  is complete.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None
  
======================================================================*/
void dsatcmdp_rmsm_done_handler
(
  dsat_rmsm_cmd_type            dsat_rmsm_cmd,
  dsat_rmsm_info_s_type         dsat_rmsm_info
);

/*===========================================================================

FUNCTION DSATCMDP_REFRESH_SUBS_BASED_NV

DESCRIPTION
  This function refreshes all the subscription based NVs 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void dsatcmdp_refresh_subs_based_nv
(
  sys_modem_as_id_e_type      subs_id
);

#ifdef FEATURE_DSAT_ETSI_DATA
/*===========================================================================

FUNCTION DSATUTIL_CONVERT_IPADDR

DESCRIPTION
  This function converts between IP V4/V6 addresses and ds_umts_pdp_addr_types.
  It identifies the type of conversion to be done and passes the rest of 
  the work off to the convert tuple function. In string to number mode the 
  presence of a colon in string identifies an IPV6 conversion else IPV4 is 
  assumed. In number to string mode the setting of 
  ds_umts_pdp_addr_type.ip_vsn identifies conversion type.

DEPENDENCIES
  Buffer pointed to by buf_ptr should be at least MAX_IPADDR_STR_LEN bytes 
  in length to hold NULL terminated, maximum length IPV6 address for 
  INT_TO_STRING conversion.

RETURN VALUE
  returns an flag that describes the result of execution.
  possible values:
    DSAT_OK : if validation successful
    DSAT_ERROR : if error occurred

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatutil_convert_ipaddr
(
   tuple_convert_enum_type mode,             /* Mode of conversion      */
   dsat_ip_addr_type *addr_ptr,          /* Number representation   */
   byte *buf_ptr                             /* NULL terminated string  */
);

/*===========================================================================

FUNCTION DSATUTIL_CONVERT_ADDR_MASK

DESCRIPTION
  This function converts between IP V4/V6 address & mask and 
  ds_umts_address_mask_types. It identifies the type of conversion to be done
  and passes the rest of the work off to the convert tuple function. 
  In string to number mode the presence of > 7 '.'s in string identifies an 
  IPV6 conversion else IPV4 is assumed. In number to string mode the setting 
  of ds_umts_pdp_addr_type.ip_vsn identifies conversion type.

DEPENDENCIES
  Buffer pointed to by buf_ptr should be at least MAX_IPADDR_STR_LEN bytes 
  in length to hold NULL terminated, maximum length IPV6 address for 
  INT_TO_STRING conversion.

RETURN VALUE
  returns an flag that describes the result of execution.
  possible values:
    DSAT_OK : if validation successful
    DSAT_ERROR : if error occurred

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatutil_convert_addr_mask
(
  tuple_convert_enum_type mode,             /* Mode of conversion      */
  ds_profile_3gpp_address_mask_type *addr_mask_ptr, /* Number representation   */
  ds_profile_3gpp_ip_version_enum_type* ip_vsn,
  byte *buf_ptr                             /* NULL terminated string  */
);

#endif /* FEATURE_DSAT_ETSI_DATA */

#ifdef FEATURE_DSAT_EXT_CLIENT_SUPPORT
/*===========================================================================

FUNCTION DSATCLIENT_SEND_URC_RESPONSE

DESCRIPTION
  This function is used to send the URC response to all the registered clients. 

DEPENDENCIES
  The client must have registered to ATCoP through 
  dsatclient_register_client and dsatclient_register_urc. 

RETURN VALUE
  NONE

SIDE EFFECTS
  None
  
===========================================================================*/
void dsatclient_send_urc_response
( 
  dsm_item_type             * tx_ptr, /* response data in dsm item */
  ds3g_siolib_status_e_type *ret_val,  /* return value of success/failure*/
  boolean       rearm_autodetect /* Done with one AT Command Line?*/
);


/*===========================================================================

FUNCTION DSATCLIENT_SEND_RESPONSE

DESCRIPTION
  This function is used to send the response to the client interface which 
  has issued an AT command through dsatclient_send_at_cmd.  

DEPENDENCIES
  The client must have had registered to ATCoP through 
  dsatclient_register_client.

RETURN VALUE
  NONE

SIDE EFFECTS
  None
  
===========================================================================*/
void dsatclient_send_response
( 
  dsm_item_type             * tx_ptr, /* response data in dsm item */
  ds3g_siolib_status_e_type *ret_val,  /* return value of success/failure*/
  boolean       rearm_autodetect       /* Rearm detect status */
);

/*===========================================================================

FUNCTION DSATCLIENT_PROCESS_SEND_AT_CMD

DESCRIPTION
  This function process the AT command from the client's command queue 
  buffer.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void dsatclient_process_send_at_cmd 
(
  char                       *at_cmd_buff, /* Buffer holding AT command */
  uint16                      cmd_len,     /* AT command length */
  dsat_ext_client_id_type     client_id,   /* Client which posted the command */
  dsat_client_cmd_rsp_cb_func cb_func,     /* Client's response call back */
  void                      *client_data  /* Cleint data to be sent back */
);

/*===========================================================================

FUNCTION DSATCLIENT_SERVICE_EXT_CLIENTS

DESCRIPTION
  This function checks the dsat_client_info[client_id].pending_flag to see 
  which client we  will process next. Round Robin Algorithm is used here. 
  The first client to be processed is the one next to the client just processed.
  This ensures fairness by not allowing a single port to hog the ATCOP engine.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void dsatclient_service_ext_clients(void);

/*===========================================================================

FUNCTION DSATCLIENT_IS_CMD_SUPPORTED

DESCRIPTION
  This command validates if the received command is supported
  through exter client interface.

DEPENDENCIES
  NONE

RETURN VALUE
  FALSE: If command is not in the supported list.
  TRUE: If command is in the supported list.

SIDE EFFECTS
  None
  
===========================================================================*/
boolean dsatclient_is_cmd_supported
( 
  const char *in_cmd
);
#endif /* FEATURE_DSAT_EXT_CLIENT_SUPPORT */

/*===========================================================================
FUNCTION DSATCMDP_PROCESS_CMD_FORWARDING

DESCRIPTION
  This function will forward the command if required and 
  sets the abort handling and result code accordingly.
  

DEPENDENCIES
  NONE 

RETURN VALUE
  DSAT_OK        -- if this is not forwarded to remote client. 
  DSAT_ASYNC_CMD -- if command forwarding to external client is SUCCESS
  DSAT_ERROR  -- if command forwarding is a FAILURE

SIDE EFFECTS
  None

===========================================================================*/

dsat_result_enum_type dsatcmdp_process_cmd_forwarding(void);

/*===========================================================================

FUNCTION DSATCLIENT_EXT_AT_RESP_HANDLER

DESCRIPTION
  This function is the handler to process the external client's AT command 
  response and forward it to correct port.
  
DEPENDENCIES
  None 

RETURN VALUE
  DSAT_ASYNC_EVENT

SIDE EFFECTS
  This will clear the curent async command processing.

===========================================================================*/
dsat_result_enum_type  dsatclient_ext_at_resp_handler 
(
  ds_cmd_type          *cmd_ptr              /* DS Command pointer         */
);
/*===========================================================================

FUNCTION DSATCLIENT_REGISTER_FWD_CLIENT_HANDLER

DESCRIPTION
  This function process the forward AT command registration from the client's 
  command queue buffer.

DEPENDENCIES
  None
  
RETURN VALUE
  DSAT_ASYNC_EVENT
  
SIDE EFFECTS
  None
  
===========================================================================*/
extern dsat_result_enum_type dsatclient_register_fwd_client_handler 
(
  ds_cmd_type          *cmd_ptr              /* DS Command pointer         */
);

/*===========================================================================

FUNCTION DSATCLIENT_REGISTER_FWD_AT_CMD_HANDLER

DESCRIPTION
  This function process the forward AT command registration from the client's 
  command queue buffer.

DEPENDENCIES
  None
  
RETURN VALUE
  DSAT_ASYNC_EVENT
  
SIDE EFFECTS
  None
  
===========================================================================*/
extern dsat_result_enum_type dsatclient_register_fwd_at_cmd_handler 
(
  ds_cmd_type          *cmd_ptr              /* DS Command pointer         */
);

/*===========================================================================

FUNCTION DSATCLIENT_DEREGISTER_FWD_AT_CMD_HANDLER

DESCRIPTION
  This function process the forward AT command deregistration from the
  client's command queue buffer.

DEPENDENCIES
  None

RETURN VALUE
  DSAT_ASYNC_EVENT

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatclient_deregister_fwd_at_cmd_handler
(
  ds_cmd_type         *cmd_ptr              /* DS Command pointer */
);

/*===========================================================================
FUNCTION DSATCLIENT_RESET_CMD_REQUEST_HANDLER

DESCRIPTION
  This function is handler function for reset request.

DEPENDENCIES
  None
  
RETURN VALUE
  DSAT_ERROR : if there was any problem .
  DSAT_OK : if it is a success.

SIDE EFFECTS
  None
  
======================================================================*/

dsat_result_enum_type  dsatclient_reset_cmd_request_handler
(
  ds_cmd_type         * cmd_ptr              /* DS Command pointer         */
);

/*===========================================================================
FUNCTION DSATCLIENT_SHUTDOWN_CMD_REQUEST_HANDLER

DESCRIPTION
  This function is the handler function for shutdown request. 

DEPENDENCIES
  None
  
RETURN VALUE
  DSAT_ERROR : if there was any problem .
  DSAT_OK : if it is a success.

SIDE EFFECTS
  None
  
======================================================================*/

dsat_result_enum_type  dsatclient_shutdown_cmd_request_handler
(
  ds_cmd_type         * cmd_ptr              /* DS Command pointer         */
);

/*===========================================================================

FUNCTION DSATCLIENT_DEREGISTER_FWD_CLIENT_HANDLER

DESCRIPTION
  This function process the forward AT command de-registration from the client's 
  command queue buffer.

DEPENDENCIES
  None
  
RETURN VALUE
  DSAT_ASYNC_EVENT
  
SIDE EFFECTS
  None
  
===========================================================================*/
extern dsat_result_enum_type dsatclient_deregister_fwd_client_handler 
(
  ds_cmd_type          *cmd_ptr              /* DS Command pointer         */
);

#ifdef FEATURE_DSAT_GOBI_MAINLINE
#define UNDP_CARRIER_MAJOR_MASK   0x00FFFE00
#define UNDP_CARRIER_DOCOMO_MAJOR 0x000D0000

/*===========================================================================

FUNCTION DSATUTIL_GET_UQCN_VERSION

DESCRIPTION
  This command reads the appropriate NV item containing the UQCN version 
  information for UMTS / CDMA.

DEPENDENCIES
  NONE

RETURN VALUE
  uint32 - UQCN Version

SIDE EFFECTS
  None
  
===========================================================================*/
uint32 dsatutil_get_uqcn_version( void );
#endif /* FEATURE_DSAT_GOBI_MAINLINE */
/*===========================================================================
FUNCTION  DSATUTIL_VALIDATE_MEMORY_MSG_ERROR

DESCRIPTION
  Validates an allocated block of memory from Modem Heap memory
  and calls an error F3 if invalid.

DEPENDENCIES
  None

RETURNS
  The Data Pointer to the allocated Memory.

SIDE EFFECTS
  None

===========================================================================*/
void *dsatutil_validate_memory_msg_error
(
  void       *data_ptr,
  uint32      data_size,
  const char *filename,
  uint32      line
);

/*===========================================================================
FUNCTION  DSATUTIL_VALIDATE_MEMORY_ERR_FATAL

DESCRIPTION
  Validates an allocated block of memory from Modem Heap memory
  and calls ERR_FATAL if invalid.

DEPENDENCIES
  None

RETURNS
  The Data Pointer to the allocated Memory.

SIDE EFFECTS
  None

===========================================================================*/
void *dsatutil_validate_memory_err_fatal
(
  void       *data_ptr,
  uint32      data_size,
  const char *filename,
  uint32      line
);

/*===========================================================================
FUNCTION  DSATUTIL_FREE_MEMORY

DESCRIPTION
  Frees a block of memory from Modem Heap memory

DEPENDENCIES
  FEATURE_DATA_MODEM_HEAP

RETURNS
  None

SIDE EFFECTS
  None


===========================================================================*/
void dsatutil_free_memory
(
  void* data_size   
);

/*===========================================================================
FUNCTION  DSATUTIL_FREE_MEMORY_EXT

DESCRIPTION
  Frees a block of memory from Modem Heap memory and makes the ptr to NULL

DEPENDENCIES
  FEATURE_DATA_MODEM_HEAP

RETURNS
  None

SIDE EFFECTS
  None


===========================================================================*/
void dsatutil_free_memory_ext
(
  void** data_size   
);

/*===========================================================================

FUNCTION DSATUTIL_GET_CMD_BUF

DESCRIPTION
  This function is a wrapper for ds_get_cmd_buf() API. It calls an error F3 in case of
  graceful handing and ERR_FATAL otherwise. The caller of this function must check
  for a NULL return value in case of graceful handing.

DEPENDENCIES
  None

RETURN VALUE
  Returns a pointer to a command buffer if avaiable or NULL in case of graceful handing
  if no command buffers are available.

SIDE EFFECTS
  None

===========================================================================*/

ds_cmd_type *dsatutil_get_cmd_buf
(
  size_t      payload_size, /* Number of bytes to allocate for cmd payload */
  const char *filename,
  uint32      line,
  boolean     graceful
);

/*===========================================================================

FUNCTION DSATUTIL_GET_CMD_PAYLOAD_PTR

DESCRIPTION
  This function is used to get command payload pointer using command pointer.
  It calls ERR_FATAL when either command pointer or command payload pointer is NULL.

DEPENDENCIES
  None

RETURN VALUE
  Returns command payload pointer.

SIDE EFFECTS
  None

===========================================================================*/

void *dsatutil_get_cmd_payload_ptr
(
  ds_cmd_type *cmd_ptr
);

/*===========================================================================

FUNCTION  DSATUTIL_ALLOC_CMD_MEM_PTR

DESCRIPTION
  This function can be used to allocate command memory pointer to maintain the
  info required during commands execution.

DEPENDENCIES
  None

RETURNS
  Command memory pointer.

SIDE EFFECTS
  None

===========================================================================*/
void *dsatutil_alloc_cmd_mem_ptr
(
  void *data_ptr
);

/*===========================================================================

FUNCTION  DSATUTIL_GET_CMD_MEM_PTR

DESCRIPTION
  This function can be used to get the command memory pointer which maintains the
  info required during commands execution.

DEPENDENCIES
  None

RETURNS
  Command memory pointer.

SIDE EFFECTS
  None

===========================================================================*/
void *dsatutil_get_cmd_mem_ptr(void);

/*===========================================================================

FUNCTION  DSATUTIL_FREE_CMD_MEM_PTR

DESCRIPTION
  This function can be used to free the command memory pointer which maintains the
  info required during commands execution.

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsatutil_free_cmd_mem_ptr(void);

/*===========================================================================
FUNCTION  DSATUTIL_MEMSCPY

DESCRIPTION
  This function will validate and copy data from src pointer to dest pointer.
  
DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None

===========================================================================*/

size_t dsatutil_memscpy
(
  void        *dst,
  size_t      dst_size,
  const void  *src,
  size_t      src_size

);

/*===========================================================================
FUNCTION  DSATUTIL_GET_BASIC_VALS

DESCRIPTION
  This function will take basic table command ID and will return val pointer corresponding .
  to the command ID.
DEPENDENCIES
  None

RETURNS
  val pointer

SIDE EFFECTS
  None

===========================================================================*/

void* dsatutil_get_basic_vals
(
  dsat_num_item_type       cmd_id,
  dsat_memory_subs_type    subs
);

/*===========================================================================
FUNCTION  DSATUTIL_GET_BASIC_ACTION_VALS

DESCRIPTION
  This function will take basic action table command ID and will return val pointer corresponding .
  to the command ID.
DEPENDENCIES
  None

RETURNS
  val pointer

SIDE EFFECTS
  None

===========================================================================*/

void* dsatutil_get_basic_action_vals
(
  dsat_num_item_type       cmd_id,
  dsat_memory_subs_type    subs
);
/*===========================================================================
FUNCTION  DSATUTIL_GET_BASIC_SREG_VALS

DESCRIPTION
  This function will take basic sreg table command ID and will return val pointer corresponding .
  to the command ID.
DEPENDENCIES
  None

RETURNS
  val pointer

SIDE EFFECTS
  None

===========================================================================*/

void* dsatutil_get_basic_sreg_vals
(
  dsat_num_item_type       cmd_id,
  dsat_memory_subs_type    subs
);
/*===========================================================================
FUNCTION  DSATUTIL_GET_BASIC_EXT_VALS

DESCRIPTION
  This function will take basic extended table command ID and will return val pointer corresponding .
  to the command ID.
DEPENDENCIES
  None

RETURNS
  val pointer

SIDE EFFECTS
  None

===========================================================================*/

void* dsatutil_get_basic_ext_vals
(
  dsat_num_item_type       cmd_id,
  dsat_memory_subs_type    subs
);
/*===========================================================================
FUNCTION  DSATUTIL_GET_BASIC_VEND_VALS

DESCRIPTION
  This function will take basic vender table command ID and will return val pointer corresponding .
  to the command ID.
DEPENDENCIES
  None

RETURNS
  val pointer

SIDE EFFECTS
  None

===========================================================================*/

void* dsatutil_get_basic_vend_vals
(
  dsat_num_item_type       cmd_id,
  dsat_memory_subs_type    subs
);

/*===========================================================================
FUNCTION  DSATUTIL_GET_ETSI_EXT_ACT_VALS

DESCRIPTION
  This function will take etsi ext action table command ID and will return val pointer corresponding .
  to the command ID.
DEPENDENCIES
  None

RETURNS
  val pointer

SIDE EFFECTS
  None

===========================================================================*/

void* dsatutil_get_etsi_ext_act_vals
(
  dsat_num_item_type       cmd_id,
  dsat_memory_subs_type    subs
);

/*===========================================================================
FUNCTION  DSATUTIL_GET_ETSI_EXT_VALS

DESCRIPTION
  This function will take etsi ext action table command ID and will return val pointer corresponding .
  to the command ID.
DEPENDENCIES
  None

RETURNS
  val pointer

SIDE EFFECTS
  None

===========================================================================*/

void* dsatutil_get_etsi_ext_vals
(
  dsat_num_item_type       cmd_id,
  dsat_memory_subs_type    subs
);

/*===========================================================================
FUNCTION  DSATUTIL_GET_707_EXT_VALS

DESCRIPTION
  This function will take CDMA(707) ext table command ID and will return val pointer corresponding .
  to the command ID.
DEPENDENCIES
  None

RETURNS
  val pointer

SIDE EFFECTS
  None

===========================================================================*/

void* dsatutil_get_707_ext_vals
(
  dsat_num_item_type       cmd_id,
  dsat_memory_subs_type    subs
);
/*===========================================================================
FUNCTION  DSATUTIL_GET_707_ACT_VALS

DESCRIPTION
  This function will take CDMA(707) ext table command ID and will return val pointer corresponding .
  to the command ID.
DEPENDENCIES
  None

RETURNS
  val pointer

SIDE EFFECTS
  None

===========================================================================*/

void* dsatutil_get_707_act_vals
(
  dsat_num_item_type       cmd_id,
  dsat_memory_subs_type    subs
);
/*===========================================================================
FUNCTION  DSATUTIL_GET_707_FAX_VALS

DESCRIPTION
  This function will take CDMA(707) ext table command ID and will return val pointer corresponding .
  to the command ID.
DEPENDENCIES
  None

RETURNS
  val pointer

SIDE EFFECTS
  None

===========================================================================*/

void* dsatutil_get_707_fax_vals
(
  dsat_num_item_type       cmd_id,
  dsat_memory_subs_type    subs
);
/*===========================================================================
FUNCTION  DSATUTIL_GET_707_HDR_VALS

DESCRIPTION
  This function will take  HDR(707) table command ID and will return val pointer corresponding .
  to the command ID.
DEPENDENCIES
  None

RETURNS
  val pointer

SIDE EFFECTS
  None

===========================================================================*/

void* dsatutil_get_707_hdr_vals
(
  dsat_num_item_type       cmd_id,
  dsat_memory_subs_type    subs
);
#ifdef FEATURE_CDMA_SMS
/*===========================================================================
FUNCTION  DSATUTIL_GET_707_SMS_VALS

DESCRIPTION
  This function will take  SMS(707) table command ID and will return val pointer corresponding .
  to the command ID.
DEPENDENCIES
  None

RETURNS
  val pointer

SIDE EFFECTS
  None

===========================================================================*/

void* dsatutil_get_707_sms_vals
(
  dsat_num_item_type       cmd_id,
  dsat_memory_subs_type    subs
);
#endif /* FEATURE_CDMA_SMS*/
/*===========================================================================
FUNCTION  DSATUTIL_GET_707_VEND_PARA_VALS

DESCRIPTION
  This function will take  Vender Parameter(707) table command ID and will return val pointer corresponding .
  to the command ID.
DEPENDENCIES
  None

RETURNS
  val pointer

SIDE EFFECTS
  None

===========================================================================*/

void* dsatutil_get_707_vend_para_vals
(
  dsat_num_item_type       cmd_id,
  dsat_memory_subs_type    subs
);
/*===========================================================================
FUNCTION  DSATUTIL_GET_707_VEND_ACT_VALS

DESCRIPTION
  This function will take  Vender Action(707) table command ID and will return val pointer corresponding .
  to the command ID.
DEPENDENCIES
  None

RETURNS
  val pointer

SIDE EFFECTS
  None

===========================================================================*/

void* dsatutil_get_707_vend_act_vals
(
  dsat_num_item_type       cmd_id,
  dsat_memory_subs_type    subs
);

/*===========================================================================
FUNCTION  DSATUTIL_GET_707_MIP_VALS

DESCRIPTION
  This function will take  MIP table command ID and will return val pointer corresponding .
  to the command ID.
DEPENDENCIES
  None

RETURNS
  val pointer

SIDE EFFECTS
  None

===========================================================================*/

void* dsatutil_get_707_mip_vals
(
  dsat_num_item_type       cmd_id,
  dsat_memory_subs_type    subs
);
/*===========================================================================
FUNCTION  DSATUTIL_GET_VAL_FROM_CMD_ID

DESCRIPTION
  This function will take valid command ID and will return val pointer corresponding .
  to the command ID.
DEPENDENCIES
  None

RETURNS
  val pointer

SIDE EFFECTS
  None

===========================================================================*/

void* dsatutil_get_val_from_cmd_id
(
  dsat_num_item_type       cmd_id,
  dsat_memory_subs_type    subs
);
/*===========================================================================
FUNCTION  DSATUTIL_DEFAULT_CONSTRUCTOR

DESCRIPTION
  This function will initialize the global variables.

DEPENDENCIES
  None

RETURNS
  TRUE  - SUCCESS
  FALSE - FAILURE

SIDE EFFECTS
  None

===========================================================================*/

void dsatutil_default_constructor(void);
/*===========================================================================
FUNCTION  DSATUTIL_DEFAULT_DESTRUCTOR

DESCRIPTION
  This function will de allocate  the dynamic memory.
  
DEPENDENCIES
  None

RETURNS
  TRUE  - SUCCESS
  FALSE - FAILURE

SIDE EFFECTS
  None

===========================================================================*/

boolean dsatutil_default_destructor
(
  dsat_mem_type     mem_type,
  uint8             index      /* Subs ID, slot ID, apps ID */
);

/*===========================================================================
FUNCTION  DSATUTIL_GET_BASE_ADDR

DESCRIPTION
  This function will allocate,deallocate and get the dynamic memory for mem_type
  which are subscription/slot/session independent.
  
DEPENDENCIES
  None

RETURNS


SIDE EFFECTS
  None

===========================================================================*/

void *dsatutil_get_base_addr
(
  dsat_mem_type           mem_type,
  boolean                 graceful
);

/*===========================================================================
FUNCTION  DSATUTIL_GET_BASE_ADDR_PER_SLOT

DESCRIPTION
  This function will allocate,deallocate and get the dynamic memory per slot.
  
DEPENDENCIES
  None

RETURNS

SIDE EFFECTS
  None

===========================================================================*/

void *dsatutil_get_base_addr_per_slot
(
  mmgsdi_slot_id_enum_type slot_id,
  boolean                 graceful
);

/*===========================================================================
FUNCTION  DSATUTIL_GET_BASE_ADDR_CURRENT_SLOT

DESCRIPTION
  This function will allocate,deallocate and get the dynamic memory
  for currently selected slot.
  
DEPENDENCIES
  None

RETURNS

SIDE EFFECTS
  None

===========================================================================*/

void *dsatutil_get_base_addr_current_slot();

/*===========================================================================
FUNCTION  DSATUTIL_GET_BASE_ADDR_PER_SESSION

DESCRIPTION
  This function will allocate,deallocate and get the dynamic memory per
  session.
  
DEPENDENCIES
  None

RETURNS

SIDE EFFECTS
  None

===========================================================================*/

void *dsatutil_get_base_addr_per_session
(
  dsat_apps_id_e_type     apps_id,
  boolean                 graceful
);

/*===========================================================================
FUNCTION  DSATUTIL_GET_BASE_ADDR_CURRENT_SESSION

DESCRIPTION
  This function will allocate,deallocate and get the dynamic memory 
  for currently selected session.
  
DEPENDENCIES
  None

RETURNS

SIDE EFFECTS
  None

===========================================================================*/

void *dsatutil_get_base_addr_current_session();

/*===========================================================================
FUNCTION  DSATUTIL_GET_BASE_ADDR_PER_SUBS

DESCRIPTION
  This function will allocate,deallocate and get the dynamic memory per subs
  
DEPENDENCIES
  None

RETURNS

SIDE EFFECTS
  None

===========================================================================*/

void *dsatutil_get_base_addr_per_subs
(
  dsat_mem_type           mem_type,
  sys_modem_as_id_e_type  subs_id,
  boolean                 graceful
);

/*===========================================================================
FUNCTION  DSATUTIL_GET_BASE_ADDR_CURRENT_SUBS

DESCRIPTION
  This function will allocate,deallocate and get the dynamic memory for currently
  selected subs.
  
DEPENDENCIES
  None

RETURNS

SIDE EFFECTS
  None

===========================================================================*/

void *dsatutil_get_base_addr_current_subs
(
  dsat_mem_type           mem_type
);

#ifdef FEATURE_DSAT_MDM_FUSION
/*===========================================================================

FUNCTION dsat_proxy_format_cmd

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
extern void dsat_proxy_format_cmd
(
  const tokens_struct_type *token_ptr,             /* Pointer to the Token  */
  const byte               *cmd_name,        /* Pointer to the cmd name as present in the Table */
  dsm_item_type          *format_cmd_ptr,         /* Resultant Raw command  */
  boolean                dial_cmd
);

/*===========================================================================
FUNCTION DSAT_GET_FORWARD_CMD_ATTRIB

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
uint32 dsat_get_forward_cmd_attrib( void );

/*===========================================================================
FUNCTION DSAT_GET_FORWARD_CMD_ATTRIB

DESCRIPTION
 This function gets the command attributes of the command that is forwarded to
 the other processor.

DEPENDENCIES
 This functionality is only applicable for FUSION based architecture.

RETURN VALUE
 Command table attribute.

SIDE EFFECTS
 None.

===========================================================================*/
uint32 dsat_get_forward_cmd_attrib( void );

/*===========================================================================
FUNCTION DSAT_GET_APPEND_RSP_STATE

DESCRIPTION
 This function gets the state of the MSM response to be appended or not.

DEPENDENCIES
 This functionality is only applicable for FUSION based architecture.

RETURN VALUE
 TRUE : MSM response is appended
 FALSE : MSM response is ignored 

SIDE EFFECTS
 NONE.

===========================================================================*/
boolean dsat_get_append_rsp_state( void );

/*===========================================================================
FUNCTION DSAT_SET_APPEND_RSP_STATE

DESCRIPTION
 This function sets the condition to either append the MSM response or not.

DEPENDENCIES
 This functionality is only applicable for FUSION based architecture.

RETURN VALUE
 NONE 

SIDE EFFECTS
 If used in any other parameter command, may have a side effect of doubling the o/p 
 response. Thus 'append' should only be TRUE only if necessary.

===========================================================================*/
void dsat_set_append_rsp_state( boolean append);

#endif /* FEATURE_DSAT_MDM_FUSION */

/*===========================================================================
FUNCTION  DSAT_CM_PH_SUBS_PREF_HANDLER

DESCRIPTION
  This function is the event handler invoked by CM for informing subscription
  specific info to ATCoP.
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsat_cm_ph_subs_pref_handler
(
  void         * cmd_ptr              /* DS Command pointer         */
);

/*===========================================================================

FUNCTION DSAT_GET_CURRENT_SLOT_ID

DESCRIPTION
  Returns the currently selected slot ID.

DEPENDENCIES
  None

RETURN VALUE
  Slot ID

SIDE EFFECTS
  None

===========================================================================*/
mmgsdi_slot_id_enum_type dsat_get_current_slot_id(void);

/*===========================================================================

FUNCTION DSAT_GET_CURRENT_GW_APPS_ID

DESCRIPTION
  Returns the currently selected GW apps ID.

DEPENDENCIES
  None

RETURN VALUE
  GW apps ID

SIDE EFFECTS
  None

===========================================================================*/
dsat_apps_id_e_type dsat_get_current_gw_apps_id
(
  boolean graceful
);

/*===========================================================================

FUNCTION DSAT_GET_CURRENT_1X_APPS_ID

DESCRIPTION
  Returns the currently selected 1X apps ID.

DEPENDENCIES
  None

RETURN VALUE
  1X apps ID

SIDE EFFECTS
  None

===========================================================================*/
dsat_apps_id_e_type dsat_get_current_1x_apps_id(void);

/*===========================================================================

FUNCTION DSAT_GET_CURRENT_APPS_ID

DESCRIPTION
  Returns the currently selected apps ID.

DEPENDENCIES
  None

RETURN VALUE
  Apps ID

SIDE EFFECTS
  None

===========================================================================*/
dsat_apps_id_e_type dsat_get_current_apps_id
(
  boolean graceful
);

/*===========================================================================

FUNCTION DSAT_GET_CURRENT_GW_APP_TYPE

DESCRIPTION
  Returns the currently selected GW app type.

DEPENDENCIES
  None

RETURN VALUE
  GW app type

SIDE EFFECTS
  None

===========================================================================*/
mmgsdi_app_enum_type dsat_get_current_gw_app_type
(
  boolean graceful
);

/*===========================================================================

FUNCTION DSAT_GET_CURRENT_1X_APP_TYPE

DESCRIPTION
  Returns the currently selected 1X app type.

DEPENDENCIES
  None

RETURN VALUE
  1X app type

SIDE EFFECTS
  None

===========================================================================*/
mmgsdi_app_enum_type dsat_get_current_1x_app_type(void);

/*===========================================================================

FUNCTION DSAT_GET_CURRENT_APP_TYPE

DESCRIPTION
  Returns the currently selected app type.

DEPENDENCIES
  None

RETURN VALUE
  App type

SIDE EFFECTS
  None

===========================================================================*/
mmgsdi_app_enum_type dsat_get_current_app_type
(
  boolean graceful
);

/*===========================================================================

FUNCTION dsat_get_current_gw_session_id

DESCRIPTION
  Returns the currently selected GW session ID.

DEPENDENCIES
  None

RETURN VALUE
  GW session ID

SIDE EFFECTS
  None

===========================================================================*/
mmgsdi_session_id_type dsat_get_current_gw_session_id
(
  boolean graceful
);

/*===========================================================================

FUNCTION dsat_get_current_1x_session_id

DESCRIPTION
  Returns the currently selected 1X session ID.

DEPENDENCIES
  None

RETURN VALUE
  1X session ID

SIDE EFFECTS
  None

===========================================================================*/
mmgsdi_session_id_type dsat_get_current_1x_session_id(void);

/*===========================================================================

FUNCTION DSAT_GET_CURRENT_1X_SUBS_ID

DESCRIPTION
  Returns the currently selected 1X subs ID.

DEPENDENCIES
  None

RETURN VALUE
  1X subs ID

SIDE EFFECTS
  None

===========================================================================*/
sys_modem_as_id_e_type dsat_get_current_1x_subs_id(void);

/*===========================================================================

FUNCTION DSAT_GET_CURRENT_SUBS_ID

DESCRIPTION
  Returns the currently selected subs ID.

DEPENDENCIES
  None

RETURN VALUE
  Subs ID

SIDE EFFECTS
  None

===========================================================================*/
sys_modem_as_id_e_type dsat_get_current_subs_id
(
  boolean graceful
);

#ifdef FEATURE_DSAT_LTE

/*===========================================================================
FUNCTION: DSATUTIL_INIT_SYS_SEL_PARAM

DESCRIPTION
  Function will initialize system selction parameters into default values.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void dsatutil_init_sys_sel_param
(
  cm_sys_sel_pref_params_s_type *sys_sel_pref_params_ptr
);
#endif /* FEATURE_DSAT_LTE*/
/*===========================================================================

FUNCTION DSATUTIL_GET_VAL

DESCRIPTION
  This FUNCTION take command index of particular command  and give value pointer
 .We have value pointer for each entry in our command table this is basic function 
  for retrieve value pointer associated to a command
  

DEPENDENCIES
  None.
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

 void * dsatutil_get_val
(
  dsat_num_item_type       cmd_id,            /*  Entry index in parse table  */
  dsat_num_item_type       apps_id,           /*  Subscription ID*/
  dsat_num_item_type       index,             /*  Index of the array*/
  value_type_enum_type     val_type           /*  Value type */
);

/*===========================================================================

FUNCTION DSATUTIL_GET_VAL_EXT

DESCRIPTION
  This FUNCTION take command index of particular command  and give value pointer
 .We have value pointer for each entry in our command table this is basic function 
  for retrieve value pointer associated to a command
  

DEPENDENCIES
  None.
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

 void * dsatutil_get_val_ext
(
  dsat_num_item_type       cmd_id,            /*  Entry index in parse table  */
  dsat_num_item_type       apps_id,           /*  Subscription ID*/
  dsat_num_item_type       index,             /*  Index of the array*/
  value_type_enum_type     val_type           /*  Value type */
);

/*===========================================================================

FUNCTION DSATUTIL_VALUE_GET

DESCRIPTION
  This FUNCTION copy value from source pointer into destination pointer .  

DEPENDENCIES
  None.
RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void * dsatutil_value_get
(
  const void *dest,                           /*  Destination pointer*/
  dsati_value_info_struct_type  *value_info         /*  value information*/
);


/*===========================================================================

FUNCTION DSATUTIL_GET_PTR_TO_VALUE

DESCRIPTION
  This FUNCTION take entry index of parse table and extract base address of table .
  according base table it will  search entry in the table, if record found then return
  value_ptr otherwise return NULL.

DEPENDENCIES
  None.
RETURN VALUE
  Return value pointer . 

SIDE EFFECTS
 None.
===========================================================================*/
 void * dsatutil_get_ptr_to_value
(
  dsat_num_item_type       cmd_id,            /*  entry index in parse table  */
  dsat_num_item_type       apps_id,           /* subscription ID*/
  dsati_value_info_struct_type  *value_info         /*  value information*/
);


/*===========================================================================

FUNCTION DSATUTIL_VALUE_COPY

DESCRIPTION
  This FUNCTION copy value from source pointer into destination pointer .  

DEPENDENCIES
  None.
RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void dsatutil_value_copy
(
  const void *dest,                           /*  Destination pointer*/
  dsati_value_info_struct_type  *value_info         /*  value information*/
);

/*===========================================================================

FUNCTION DSATUTIL_SET_VALUE

DESCRIPTION
  This FUNCTION take entry index of parse table and extract base address of table .
  according base table it will  search entry in  table, if record found then typecast
  value_ptr into value_type and set it into table value pointer .  

DEPENDENCIES
  None.
RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void dsatutil_set_value
(
  dsat_num_item_type       cmd_id,           /*  entry index in parse table  */
  dsat_num_item_type       apps_id,          /*  subscription ID*/ 
  dsati_value_info_struct_type  *value_info        /*  value information*/
);
/*===========================================================================

FUNCTION DSATUTIL_SET_VAL_INFO

DESCRIPTION
  This FUNCTION copy values into destination structure pointer .  

DEPENDENCIES
  None.
RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/

void dsat_set_val_info
(           
  dsat_num_item_type              index,
  dsat_num_item_type              size,
  void *                          val_ptr,
  value_type_enum_type            val_type,
  dsati_value_info_struct_type   *val_info
);

void dsat_mo_dailing_state_init(void);

boolean dsat_mo_dailing_state_get
(
  sys_modem_as_id_e_type subs_id
);

void dsat_mo_dailing_state_set
(
  sys_modem_as_id_e_type subs_id,
  boolean                value
);

void dsat_value_set
(
  dsat_num_item_type cmd_id,
  dsat_num_item_type apps_id,
  dsat_num_item_type index,
  dsat_num_item_type size,
  dsat_num_item_type value_num,
  value_type_enum_type value_type
); 
 

/*
*  Macro used for get/set value pointers from the table. 
*
*/
    /*===========================================================================
    
    MACRO DSATUTIL_SET_VAL
    
    DESCRIPTION
    
      This MACRO set val pointers assosiated to command index.  
    
    DEPENDENCIES
      None.
    RETURN VALUE
      None
    
    SIDE EFFECTS
      None.
    ===========================================================================*/

#define DSATUTIL_SET_VAL(cmd_id,apps_id,index,size,value_num,value_type) \
  {  \
    dsat_value_set(cmd_id,apps_id,index,size,value_num,value_type);\
  } 

/*===========================================================================

FUNCTION DSATUTIL_GET_VALUE

DESCRIPTION
  TThis MACRO get val pointers assosiated to command index. .  

DEPENDENCIES
  None.
RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/


#define DSATUTIL_GET_VALUE(cmd_id,apps_id,index,value_type,result_ptr)\
  {  \
    dsati_value_info_struct_type val_info;\
    memset((void *)&val_info,0,sizeof(val_info));\
    dsat_set_val_info( index, 0,0, value_type,&val_info);\
    result_ptr = dsatutil_get_ptr_to_value( cmd_id,apps_id,&val_info);\
  }

/*===========================================================================

FUNCTION DSAT_ASSERT

DESCRIPTION
  This MACRO is to handle FATAL scenarios. .  

DEPENDENCIES
  None.
RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/


#define DSAT_ASSERT(cond_flag)\
  {  \
    if(!cond_flag)  \
    {  \
      ERR_FATAL("ATCoP Assertion " #cond_flag " failed",0,0,0);  \
    }  \
  }

/*===========================================================================

FUNCTION DSAT_INIT_QCSIMAPP_TABLE

DESCRIPTION
  This function initializes the QCSIMAPP table.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None
  
======================================================================*/
void dsat_init_qcsimapp_table(void);

/*===========================================================================
FUNCTION: DSAT_UPDATE_QCSIMAPP_TABLE

DESCRIPTION:
  This function updates QCSIMAPP table in flex mapping scenarios.

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None

===========================================================================*/
void dsat_update_qcsimapp_table
(
  dsat_slot_idx_e_type slot_idx
);

/*===========================================================================

FUNCTION  DSAT_GET_QCSIMAPP_VAL

DESCRIPTION
  This function returns value of dsat_qcsimapp_val 

DEPENDENCIES
  None
  
RETURN VALUE
  value of dsat_qcsimapp_val

SIDE EFFECTS
  None

===========================================================================*/

dsat_num_item_type dsat_get_qcsimapp_val(void);

/*===========================================================================

FUNCTION  dsat_get_ds_profile_subs_id

DESCRIPTION
  This function returns value of dsat_get_ds_profile_subs_id 

DEPENDENCIES
  None
  
RETURN VALUE
  value of dsat_get_ds_profile_subs_id

SIDE EFFECTS
  None

===========================================================================*/

ds_profile_subs_etype dsat_get_ds_profile_subs_id
(
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================

FUNCTION  DSAT_GET_CURRENT_DS_PROFILE_SUBS_ID

DESCRIPTION
  This function returns value of dsat_get_current_ds_profile_subs_id 

DEPENDENCIES
  None
  
RETURN VALUE
  value of dsat_get_ds_profile_subs_id

SIDE EFFECTS
  None

===========================================================================*/

ds_profile_subs_etype dsat_get_current_ds_profile_subs_id( void );


/*===========================================================================

FUNCTION DSATCLIENT_EXT_AT_URC_HANDLER

DESCRIPTION
  This function is the handler to process the external client's AT command 
  URC
  
DEPENDENCIES
  None 

RETURN VALUE
  DSAT_ASYNC_EVENT

SIDE EFFECTS
  This will clear the curent async command processing.

===========================================================================*/
dsat_result_enum_type  dsatclient_ext_at_urc_handler 
(
  ds_cmd_type          *cmd_ptr              /* DS Command pointer         */
);

/*===========================================================================

FUNCTION  DSAT_SET_QCSIMAPP_VAL

DESCRIPTION
  This function set dsat_qcsimapp_val and dsat_qcsimapp_val_updated

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void dsat_set_qcsimapp_val
(
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================

FUNCTION  DSAT_UPDATE_QCSIMAPP_VAL

DESCRIPTION
  This function updates dsat_qcsimapp_val

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void dsat_update_qcsimapp_val(void);

/*===========================================================================

FUNCTION  DSAT_SET_QCSIMAPP_VAL_USER_FLAG

DESCRIPTION
  This function sets dsat_qcsimapp_val_user_flag

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void dsat_set_qcsimapp_val_user_flag(void);

/*===========================================================================

FUNCTION DSAT_IS_ATCOP_ALLOWED

DESCRIPTION
  This function will return either TRUE or FALSE based on  production environment requirement 
DEPENDENCIES
  

RETURN VALUE
  TRUE    - Allowed for reading  in production environment  
  FALSE  -  Blocked for reading  in production environment 

SIDE EFFECTS
  None

===========================================================================*/

dsat_num_item_type dsat_is_atcop_allowed(void);

/*===========================================================================

FUNCTION DSAT_ETSI_NV_SYNC

DESCRIPTION
  This function will read all ETSI related NV items.
  
DEPENDENCIES
  

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void dsat_etsi_nv_sync( void );

/*===========================================================================
FUNCTION  DSATUTIL_DECODE_UCS2_80_81_82_ENCODING

DESCRIPTION
  Extracts the UCS2 80, 81, or 82 encoded buffer and decodes into UCS2 little
  endian.

DEPENDENCIES
  None.

RETURN VALUE
  size_t len: Length of the output buffer, number of uint16 characters.
  
SIDE EFFECTS
  None
==============================================================================*/
size_t dsatutil_decode_ucs2_80_81_82_encoding
(
  uint8 *buf_in, 
  size_t buf_in_len,
  uint16 *buf_out, 
  size_t buf_out_max_len
);

/*===========================================================================

FUNCTION  DSATME_IS_THIN_UI

DESCRIPTION
  This fucntion tells whether thin UI is enabled or not.

DEPENDENCIES
  None

RETURN VALUE
  TRUE:  If thin UI is enabled.
  FALSE: If thin UI is disabled (default).

SIDE EFFECTS
  None

===========================================================================*/

boolean dsatme_is_thin_ui(void);

/*===========================================================================

FUNCTION  DSATCMDP_SEND_URC

DESCRIPTION


DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None

===========================================================================*/

void dsatcmdp_send_urc
(
  dsm_item_type      *buffer,
  dsat_rsp_enum_type  rsp_type
);

/*===========================================================================
FUNCTION  DSATCMDP_SEND_URC_PER_SLOT

DESCRIPTION
  This function is used to send an URC per slot.

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None

===========================================================================*/

void dsatcmdp_send_urc_per_slot
(
  mmgsdi_slot_id_enum_type  slot_id,
  dsm_item_type            *buffer,
  dsat_rsp_enum_type        rsp_type
);

/*===========================================================================
FUNCTION  DSATCMDP_SEND_URC_PER_SESSION

DESCRIPTION
  This function is used to send an URC per session.

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None

===========================================================================*/

void dsatcmdp_send_urc_per_session
(
  dsat_apps_id_e_type  apps_id,
  dsm_item_type       *buffer,
  dsat_rsp_enum_type   rsp_type
);

/*===========================================================================
FUNCTION  DSATCMDP_SEND_URC_PER_SUBS

DESCRIPTION
  This function is used to send an URC per subscription.

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None

===========================================================================*/

void dsatcmdp_send_urc_per_subs
(
  sys_modem_as_id_e_type  subs_id,
  dsm_item_type          *buffer,
  dsat_rsp_enum_type      rsp_type
);

/*===========================================================================

FUNCTION  DSAT_GET_MAX_SUBS

DESCRIPTION
  This function is used to get the maximum number of subscriptions based on
  device mode.

DEPENDENCIES
  None

RETURN VALUE
  Returns the maximum number of subscriptions.

SIDE EFFECTS
  None

===========================================================================*/

uint8 dsat_get_max_subs(void);

/*===========================================================================

FUNCTION DSATRSP_SEND_URC

DESCRIPTION
   Send result code to TE or Queues if command in process.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void dsatrsp_send_urc
(
  dsat_result_enum_type result_code
);

/*===========================================================================

FUNCTION DSATRSP_SEND_URC_PER_SUBS

DESCRIPTION
  Send result code per subscription to TE or Queues if command in process. 

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void dsatrsp_send_urc_per_subs
(
  sys_modem_as_id_e_type subs_id,
  dsat_result_enum_type  result_code
);

/*===========================================================================

FUNCTION DSAT_IS_VALID_NUM_TOKEN

DESCRIPTION
   Check if a particular token of the current token_ptr is 
   in valid and also converts the string to integer. It checks
   if the token passed is in range.

DEPENDENCIES

RETURN VALUE
  dsat_err_e_type

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_err_e_type dsat_is_valid_num_token
(
  const tokens_struct_type *tok_ptr,
  uint8                     token_index,
  dsat_num_item_type        range_min,
  dsat_num_item_type        range_max,
  void                     *output_buf
);
/*===========================================================================

FUNCTION DSAT_IS_VALID_STRING_TOKEN

DESCRIPTION
   Check if a particular token of the current token_ptr is 
   in valid and also strip the quotes from the token_ptr passed.

DEPENDENCIES

RETURN VALUE
  dsat_err_e_type

SIDE EFFECTS
  None
  
===========================================================================*/

dsat_err_e_type dsat_is_valid_string_token
(
  const tokens_struct_type *tok_ptr,
  uint8                     token_index,
  void                     *input_buf,
  void                     *output_buf,
  word                      str_len
);


/*===========================================================================
 FUNCTION DSAT_IS_PENDING_CMD_STATE
 
 DESCRIPTION
   This function checks the pending state of a command being passed
 
 DEPENDENCIES
   None.
 
 RETURN VALUE
   TRUE: Returns TRUE if found
   FALSE: otherwise
 
 SIDE EFFECTS
   None.
 ===========================================================================*/
boolean dsat_is_pending_cmd_state
(
  uint16             cmd_id,
  dsat_num_item_type apps_id, 
  dsat_num_item_type pending
);
/*===========================================================================
 FUNCTION DSAT_IS_NOT_PENDING_CMD_STATE
 
 DESCRIPTION
   This function checks the pending state of a command being passed
 
 DEPENDENCIES
   None.
 
 RETURN VALUE
   TRUE: Returns TRUE if found
   FALSE: otherwise
 
 SIDE EFFECTS
   None.
 ===========================================================================*/
boolean dsat_is_not_pending_cmd_state
(
  uint16             cmd_id,
  dsat_num_item_type apps_id, 
  dsat_num_item_type pending
);

/*===========================================================================
 FUNCTION DSAT_SET_PENDING_CMD_STATE
 
 DESCRIPTION
   This function sets the pending state of a command being passed.
 
 DEPENDENCIES
   None.
 
 RETURN VALUE
   TRUE: Returns TRUE if found
   FALSE: otherwise
 
 SIDE EFFECTS
   None.
 ===========================================================================*/
void dsat_set_pending_cmd_state
(
  uint16             cmd_id,
  dsat_num_item_type apps_id, 
  dsat_num_item_type pending
);

/*===========================================================================
  FUNCTION dsatutil_convert_endianness16()

  DESCRIPTION
    Convert endianness of 16bit character string by swapping first and second
    byte of each character

  PARAMETERS
   buf: ptr to buffer that will be converted.
   size: size of 'buf' in terms of number of bytes. 'size' is expected to be
          an even number. if not, last byte at the end will be left untouched
  
  DEPENDENCIES
    None.

  RETURN VALUE
    None
  
  SIDE EFFECTS
    None
===========================================================================*/
void dsatutil_convert_endianness16
(
  unsigned char *buf,
  unsigned int size
);

#endif /* DSATI_H */

