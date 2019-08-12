/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

    R O U T E  U P D A T E  P R O T O C O L   C O N F I G U R A T I O N

GENERAL DESCRIPTION
  This module contains code related to the configuration negotiation of the
  route update protocol (RUP).

EXTERNALIZED FUNCTIONS (Global)
  hdrrupc_powerup_init - perform powerup initialization

EXTERNALIZED FUNCTIONS (Regional)
  hdrrupc_process_msg  - process message for the configuration module
  hdrrupc_process_ind  - process indication for the configuration module
 
INITIALIZATION AND SEQUENCING REQUIREMENTS
  The protocol must be initialized after system startup prior to activating
  the protocol.


Copyright (c) 2000 - 2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */ 
/*===========================================================================

                        EDIT HISTORY FOR MODULE

$PVCSPath: O:/src/asw/COMMON/vcs/hdrrupc.c_v   1.11   11 Oct 2002 12:13:18   maldinge  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/src/hdrrupc.c#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/07/14   gga     Fixed a crash while processing Attribute overhead message
                   when negotiating inter-RAT RUP.
05/21/14   cnx     Format LTE 1x capability req with the band provided by LTE
04/17/12   cnx     Support rfm_is_band_chan_supported_v2().
07/27/11   cnx     Merged Optimized Handoff changes.
04/15/11   cnx     Removed hdrutil_band_lists if RF_HAS_RFM_BAND_CHAN_V2_API defined.
01/24/11   cnx     Support listing valid subclasses in BC0.  
01/14/11   cnx     Support listing all subclasses within a band supported by RF.   
09/16/10   cnx     Fixed compiler warnings.    
03/25/10   pba     Populate the 1X-HRPD capabilities container with CDMA band 
                   information.
03/09/10   sju     Mainlined OFS RouteUpdateRequest support.
03/18/09   rkc     Added RF SVDO API changes.
03/23/09   wsh     CMI: featurized CMI under FEATURE_CMI
11/05/08   wsh     Fixed hashing when SP includes BC15
12/14/07   sju     Added FwdFeedBackMultiplexingSupported attrib negotiation
10/17/07   pba     Updates based on code review comments.
10/09/07   sju     Added support for non-GAUPable revB attributes
08/23/07   sju     Added support to handle attributes which should
                   not be proposed by the AN
08/15/07   sju     Added support for Gauping revB SetManagementParams
07/31/07   wsh     Fixed RuR report timer such that search will not ignore an
                   RuR while processing the previous one.
07/19/07   sju     Added hdrrupc_config_is_valid()
07/13/07   sju     Added support for revB Multicarrier RUP
06/18/07   hal     Use hdrutil_chan_is_in_band_with_all_subclasses() in cases
                   where all subclasses of a particular band are supported
06/04/07   yll     Added support for SupportRouteUpdateEnhancements = 0x2 
                   and 0x3.
03/06/07   wsh     Removal of RVCT compiler warnings
01/12/06   pba     Reject Rev-A default subtype attributes when Phy is not ST2
11/21/06   wsh     Fixed potential problem that if a AN proposed value is 
                   rejected by AT during AN_INIT state, it might be rolled
                   by to the AT_INIT stage value, instead of the fallback.
11/20/06   hal     Updated feature for SupportedCDMAChannels to silently close
                   session if band preferences are changed
10/03/06   pba     Fixed potential bug in ConfigReq message unpacking routine.
10/02/06   pba     Propose AT initiated attributes for RUP only if the current 
                   values are not the AT's desired values.
07/24/06   hal     Fixed possible infinite loop in hdrrupc_build_supp_cdma_chan_attrib()
05/31/06   hal     Modified hdrrupc_band_is_supported() to calculate the
                   band subclass support in a more robust manner
04/07/06   hal     Changed hdrutil_new_buffer() to use hdrutil_new_msg_buffer()
03/22/06   pba     Send Session close if AT could not accept AN's AUR message.
02/10/06   pba     Update database when an attribute is GAUP'ed.
01/26/06   hal     Minor change to fix simple attrib negotiation
01/24/06   hal     Fix to avoid returning sign-extended RouteUpdateRadiusAdd attrib
01/04/06   hal     Added #defines for several default attrib values
12/16/05   hal     Fixed RouteUpdateRadiusAdd attrib parsing
                   Fixed SupportedCDMAChannel attrib reporting
12/13/05   pba     Changes to support multiple personality negotiation
12/08/05   etv     Added the header file for db.
12/07/05   hal     Changed the offset for determining the SupportedCDMAChannels
                   attrib to ignore multiple occurrences of Band Class 0
12/06/05   hal     Added support for AT-init cfg of SupportedCDMAChannels attrib
11/28/05   hal     Fixed RouteUpdateRadiusAdd attrib length (12 bits, not 16)
11/07/05   hal     Fixed parsing of CReq messages
10/26/05   hal     Fixed determination of supported band sub classes
                   Added support for discarding improper GAUP requests
10/04/05   pba     Added support to read/write config parameters from SCMDB/EFS
09/12/05   hal     Added hdrrupc_config_is_needed() for use during AT-init
                   configuration
                   Added support for SupportedCDMAChannels attribute (no AT-
                   initiated config)
                   Added support for AN-initiated GAUP of simple attributes
                   Added simple attributes RouteUpdateRadiusMultiply, 
                   RouteUpdateRadiusAdd and SupportRouteUpdateEnhancements
04/04/05   pba     Removed Lint errors
03/24/05   pba     Lint cleanup
12/07/04   hal     Changed dsm_offset_new_buffer to hdrutil_new_buffer.
09/10/03   mpa     Converted F3 messages to use new MSG2.0 SSID
10/09/02   mpa     Added Set Management Override Allowed functionality
03/28/02   dna     Fixed default NeighborMaxAge to be 0, not 20
03/18/02   nmw     Fixed NV interaction
10/17/02   vas     Changed pilot_inc macro back to unpack16 from unpack8 
09/11/01   vas     Macros changed. Changed updating of negotiated parameters
                   in Config Request message processing.
09/10/01   vas     Made config request/response handling identical to others
06/22/01   vas     Corrected default search window sizes.
06/04/01   vas     Created module.

===========================================================================*/

/* <EJECT> */ 
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "hdr_variation.h"
#include "hdrtrace.h"
#include "rex.h"
#include "hdrdebug.h"
#include "err.h"
#include "task.h"
#include "hdrrupi.h"
#include "hdrcmac.h"
#include "hdrlmac.h"
#include "hdrerrno.h"
#include "hdrrupc.h"
#include "hdrrups.h"
#include "hdrhmp.h"
#include "hdrmci.h"
#include "cm.h"

#ifdef FEATURE_CMI
#include "cm_v.h"
#endif

#include "ran.h"
#include "hdrscp.h"
#include "hdramp.h"
#include "hdrutil.h"
#include "dsm.h"
#include "hdrhai.h"
#include "hdrbit.h"
#include "nv.h"
#include "sys.h"

#include "hdrscmdb.h"
#include "hdrscmrup.h"

#ifdef FEATURE_RF_SVDO_API
#include "rfm_hdr.h"
#include "rfm_common.h"
#else
#include "rfm.h"  /* Needed for SupportedCDMAChannels attribute */
#endif /* FEATURE_RF_SVDO_API */
/* <EJECT> */ 

/*===========================================================================

             DEFINITIONS  FROM  THE  HDR  AIR  INTERFACE  SPEC

===========================================================================*/
#define HDRRUPC_CON_RSP_MSG_PRIORITY 40  /* Config Response Message priority */
#define HDRRUPC_AOR_MSG_PRIORITY  40

/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains defidleions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/
#define HDRRUPC_DEFAULT_PILOT_INC            4 
#define HDRRUPC_DEFAULT_SEARCH_WIN_ACTIVE    HDRSRCH_WINSIZE_60_CHIPS 
#define HDRRUPC_DEFAULT_SEARCH_WIN_NEIGHBOR  HDRSRCH_WINSIZE_100_CHIPS 
#define HDRRUPC_DEFAULT_SEARCH_WIN_REMAINING HDRSRCH_WINSIZE_100_CHIPS 

#define HDRRUPC_DEFAULT_SAME_CHAN_PILOT_ADD            0x0e
#define HDRRUPC_DEFAULT_SAME_CHAN_PILOT_COMPARE        0x05
#define HDRRUPC_DEFAULT_SAME_CHAN_PILOT_DROP           0x12
#define HDRRUPC_DEFAULT_SAME_CHAN_PILOT_DROP_TIMER     HDRSRCH_DROPTIMER_4_SEC 
#define HDRRUPC_DEFAULT_SAME_CHAN_SOFT_SLOPE           0x00 
#define HDRRUPC_DEFAULT_SAME_CHAN_ADD_INTERCEPT        0x00 
#define HDRRUPC_DEFAULT_SAME_CHAN_DROP_INTERCEPT       0x00 
#define HDRRUPC_DEFAULT_SAME_CHAN_NEIGHBOR_MAX_AGE     0 

#define HDRRUPC_DEFAULT_DIFF_CHAN_PILOT_ADD            0x0e
#define HDRRUPC_DEFAULT_DIFF_CHAN_PILOT_COMPARE        0x05
#define HDRRUPC_DEFAULT_DIFF_CHAN_PILOT_DROP           0x12
#define HDRRUPC_DEFAULT_DIFF_CHAN_PILOT_DROP_TIMER     HDRSRCH_DROPTIMER_4_SEC 
#define HDRRUPC_DEFAULT_DIFF_CHAN_SOFT_SLOPE           0x00 
#define HDRRUPC_DEFAULT_DIFF_CHAN_ADD_INTERCEPT        0x00 
#define HDRRUPC_DEFAULT_DIFF_CHAN_DROP_INTERCEPT       0x00 
#define HDRRUPC_DEFAULT_DIFF_CHAN_NEIGHBOR_MAX_AGE     0 
                                                     
#define HDRRUPC_DEFAULT_OVERRIDE_ALLOWED_ATTRIB_VAL    0x00
#define HDRRUPC_ATTRIB_OVERRIDE_NOT_ALLOWED 0
#define HDRRUPC_ATTRIB_OVERRIDE_ALLOWED 1

#ifdef FEATURE_LTE_TO_HDR
#define HDRRUPC_A21_MOBILE_SUBSC_ELEMENT_ID 0x0B
#define HDRRUPC_A21_MOBILE_SUBSC_RECORD_ID 0x00
#define HDRRUPC_A21_MOBILE_SUBSC_ALL_BAND_SUBCLASS_INCL 0x01

#define HDRRUPC_A21_MOBILE_SUBSC_ELEMENT_ID_LENGTH_SIZE 0x08
#define HDRRUPC_A21_MOBILE_SUBSC_LENGTH_SIZE 0x08
#define HDRRUPC_A21_MOBILE_SUBSC_RECORD_ID_SIZE 0x08
#define HDRRUPC_A21_MOBILE_SUBSC_RECORD_LENGTH_SIZE 0x08
#define HDRRUPC_A21_MOBILE_SUBSC_ALL_BAND_CLASS_INCL_SIZE 0x01
#define HDRRUPC_A21_MOBILE_SUBSC_CURR_BAND_SUBCLASS_SIZE 0x07
#define HDRRUPC_A21_MOBILE_SUBSC_BAND_CLASS_SIZE 0x08
#define HDRRUPC_A21_MOBILE_SUBSC_ALL_BAND_SUBCLASS_INCL_SIZE 0x01
#define HDRRUPC_A21_MOBILE_SUBSC_RESV1_SIZE 0x03
#define HDRRUPC_A21_MOBILE_SUBSC_BAND_SUBCLASS_LENGTH_SIZE 0x04
#define HDRRUPC_A21_MOBILE_SUBSC_BAND_SUBCLASS_SIZE 0x01

#define HDRRUPC_MAX_SUB_BANDS 32
#define HDRRUPC_MAX_SUB_BANDS_OCTETS 4
#endif /* FEATURE_LTE_TO_HDR */

/*--------------------------------------------------------------------------
                   CONFIGURATION RESPONSE MESSAGE STRUCTURE

--------------------------------------------------------------------------*/

/*===========================================================================

          MACROS FOR EXTRACING CONFIG REQUEST MESSAGE FIELDS

===========================================================================*/

#define HDRRUPC_BITS_PER_BYTE 8

/* Round to the nearest integer number of bytes */
#define HDRRUPC_ROUND_NEAREST_BYTE( bits )  \
                     ((bits) % HDRRUPC_BITS_PER_BYTE) == 0 ? \
                     ((bits) / HDRRUPC_BITS_PER_BYTE) :      \
                     ((bits) / HDRRUPC_BITS_PER_BYTE) + 1    \

#define HDRRUPC_MSG_ID_SIZE              8
#define HDRRUPC_TRANSACTION_ID_SIZE      8
#define HDRRUPC_ATTRIB_LEN_SIZE 8
#define HDRRUPC_ATTRIB_ID_SIZE 8
#define HDRRUPC_VALUE_ID_SIZE 8

/* For a complex attribute, ATTRIB_VAL_SIZE is the size of the 
 * attribute value structure, including the ValueID
 */

/* Search Paramteres Attribute Record Length */
#define HDRRUPC_SRCH_PARAMS_ATTRIB_SIZE 24

/* Set Management Parameters record size excluding dynamic threshholds */
#define HDRRUPC_CON_REQ_SET_MNGMT_PARAMS_DYN_THRESHHOLDS_EXCL_SIZE 40

/* Set Management Parameters record size including dynamic threshholds */
#define HDRRUPC_CON_REQ_SET_MNGMT_PARAMS_DYN_THRESHHOLDS_INCL_SIZE 56

/* The minimum attribute has Length, attribute ID and attribute record */
/* Minimum Search Parameters Attribute Size */
#define HDRRUPC_SRCH_PARAMS_MIN_ATTRIB_SIZE  \
        ( HDRRUPC_SRCH_PARAMS_ATTRIB_SIZE + \
         HDRRUPC_ATTRIB_LEN_SIZE + \
         HDRRUPC_ATTRIB_ID_SIZE )


#define HDRRUPC_SET_MNGMT_PARAMS_MIN_ATTRIB_SIZE \
        ( HDRRUPC_CON_REQ_SET_MNGMT_PARAMS_DYN_THRESHHOLDS_EXCL_SIZE + \
         HDRRUPC_ATTRIB_LEN_SIZE + \
         HDRRUPC_ATTRIB_ID_SIZE )

#define HDRRUPC_SET_MNGMT_OVERRIDE_ALLOWED_ATTRIB_SIZE 8

#define HDRRUPC_ROUTE_UPDATE_RADIUS_MULT_ATTRIB_SIZE 8
#define HDRRUPC_ROUTE_UPDATE_RADIUS_ADD_ATTRIB_SIZE 16
#define HDRRUPC_SUPPORT_ROUTE_UPDATE_ENH_ATTRIB_SIZE 8

#define HDRRUPC_ROUTE_UPDATE_RADIUS_MULT_ATTRIB_LEN \
        ( HDRRUPC_ATTRIB_LEN_SIZE + \
          HDRRUPC_ATTRIB_ID_SIZE + \
          HDRRUPC_ROUTE_UPDATE_RADIUS_MULT_ATTRIB_SIZE )
        
#define HDRRUPC_ROUTE_UPDATE_RADIUS_ADD_ATTRIB_LEN \
        ( HDRRUPC_ATTRIB_LEN_SIZE + \
          HDRRUPC_ATTRIB_ID_SIZE + \
          HDRRUPC_ROUTE_UPDATE_RADIUS_ADD_ATTRIB_SIZE )

#define HDRRUPC_SUPPORT_ROUTE_UPDATE_ENH_ATTRIB_LEN \
        ( HDRRUPC_ATTRIB_LEN_SIZE + \
          HDRRUPC_ATTRIB_ID_SIZE + \
          HDRRUPC_SUPPORT_ROUTE_UPDATE_ENH_ATTRIB_SIZE )

#define HDRRUPC_ROUTE_UPDATE_RADIUS_MULT_ATTRIB_LEN_BYTES \
        ( (HDRRUPC_ATTRIB_ID_SIZE + \
          HDRRUPC_ROUTE_UPDATE_RADIUS_MULT_ATTRIB_SIZE) / \
          HDRRUPC_BITS_PER_BYTE )
          
#define HDRRUPC_ROUTE_UPDATE_RADIUS_ADD_ATTRIB_LEN_BYTES \
        ( (HDRRUPC_ATTRIB_ID_SIZE + \
          HDRRUPC_ROUTE_UPDATE_RADIUS_ADD_ATTRIB_SIZE) / \
          HDRRUPC_BITS_PER_BYTE )
          
#define HDRRUPC_SUPPORT_ROUTE_UPDATE_ENH_ATTRIB_LEN_BYTES \
        ( (HDRRUPC_ATTRIB_ID_SIZE + \
          HDRRUPC_SUPPORT_ROUTE_UPDATE_ENH_ATTRIB_SIZE) / \
          HDRRUPC_BITS_PER_BYTE )

/* The smallest attribute for RUP is the Override Allowed attribute */
#define HDRRUPC_MIN_ATTRIB_SIZE HDRRUPC_SET_MNGMT_OVERRIDE_ALLOWED_ATTRIB_SIZE

/* Unpacking Config Request */
#define HDRRUPC_CON_REQ_UNPACK_TRANSACTION_ID( ptr, val_ptr )  \
          HDRBIT_UNPACK8( (ptr), (val_ptr), 8, 8 )

/* Attribute fields are offset from the beginning of the attribute record */
#define HDRRUPC_CON_REQ_UNPACK_LEN( ptr, val_ptr, aoffset ) \
          HDRBIT_UNPACK8( (ptr), (val_ptr), ((aoffset) + 0), 8 )

#define HDRRUPC_CON_REQ_UNPACK_ATTRIBUTE_ID( ptr, val_ptr, aoffset )  \
          HDRBIT_UNPACK8( (ptr), (val_ptr), ((aoffset) + 8), 8 )

#define HDRRUPC_CON_REQ_UNPACK_VALUE_ID( ptr, val_ptr, aoffset, voffset ) \
   HDRBIT_UNPACK8( (ptr), (val_ptr), ((aoffset) + (voffset) + 0), 8 )
   
#define HDRRUPC_CON_REQ_UNPACK_OVERRIDE_ALLOWED( ptr, val_ptr, aoffset, voffset ) \
   HDRBIT_UNPACK8( (ptr), (val_ptr), ((aoffset) + (voffset) + 0), 8 )

/* Search Parameters
*/ 
/* Pilot Increment */
#define HDRRUPC_CON_REQ_UNPACK_PILOT_INC( ptr, val_ptr, aoffset, voffset ) \
   HDRBIT_UNPACK16( (ptr), (val_ptr), ((aoffset) + (voffset) + 8), 4 )

/* Search Window Active */
#define HDRRUPC_CON_REQ_UNPACK_SEARCH_WIN_ACTIVE( ptr, val_ptr, aoffset,\
      voffset ) \
   HDRBIT_UNPACK8( (ptr), (val_ptr), ((aoffset) + (voffset) + 12), 4 )

/* Search Window Nighbor */
#define HDRRUPC_CON_REQ_UNPACK_SEARCH_WIN_NEIGHBOR( ptr, val_ptr, aoffset, \
      voffset) \
   HDRBIT_UNPACK8( (ptr), (val_ptr), ((aoffset) + (voffset) + 16), 4 )

/* Search Window Remaning */
#define HDRRUPC_CON_REQ_UNPACK_SEARCH_WIN_REMAIN(ptr, val_ptr, aoffset, \
      voffset) \
   HDRBIT_UNPACK8( (ptr), (val_ptr), ((aoffset) + (voffset) + 20), 4 )

/* Set Management Parameters
*/

/* Pilot Add */
#define HDRRUPC_CON_REQ_UNPACK_PILOT_ADD( ptr, val_ptr, aoffset, voffset ) \
   HDRBIT_UNPACK8( (ptr), (val_ptr), ((aoffset) + (voffset) + 8), 6 )

/* Pilot Compare */
#define HDRRUPC_CON_REQ_UNPACK_PILOT_COMPARE(ptr, val_ptr, aoffset, voffset) \
   HDRBIT_UNPACK8((ptr),(val_ptr),((aoffset) + (voffset) + 14),6); \
   (*val_ptr = HDRRUP_EXTEND6(*(val_ptr)))

/* Pilot Drop */
#define HDRRUPC_CON_REQ_UNPACK_PILOT_DROP( ptr, val_ptr, aoffset, voffset ) \
   HDRBIT_UNPACK8( (ptr), (val_ptr), ((aoffset) + (voffset) + 20), 6 )

/* Pilot Drop Timer */
#define HDRRUPC_CON_REQ_UNPACK_PILOT_DROP_TIMER(ptr,val_ptr,aoffset,voffset) \
   HDRBIT_UNPACK8( (ptr), (val_ptr), ((aoffset) + (voffset) + 26), 4 )

/* Dynamic Threshholds */
#define HDRRUPC_CON_REQ_UNPACK_DYNAMIC_THRESHOLDS(ptr,val_ptr,aoffset,voffset)\
   HDRBIT_UNPACK8( (ptr), (val_ptr), ((aoffset) + (voffset) + 30), 1 )

/* Soft Slope */
#define HDRRUPC_CON_REQ_UNPACK_SOFT_SLOPE( ptr, val_ptr, aoffset, voffset ) \
   HDRBIT_UNPACK8( (ptr), (val_ptr), ((aoffset) + (voffset) + 31), 6 )

/* Add Intercept */
#define HDRRUPC_CON_REQ_UNPACK_ADD_INTERCEPT(ptr, val_ptr, aoffset, voffset) \
   HDRBIT_UNPACK8((ptr), (val_ptr),((aoffset) + (voffset) + 37),6); \
   (*(val_ptr) = HDRRUP_EXTEND6(*(val_ptr)))

/* Drop Intercept */
#define HDRRUPC_CON_REQ_UNPACK_DROP_INTERCEPT(ptr, val_ptr, aoffset, voffset) \
   HDRBIT_UNPACK8((ptr), (val_ptr), ((aoffset) + (voffset) + 43), 6); \
   (*(val_ptr) = HDRRUP_EXTEND6(*(val_ptr)))

/* Neighbor Max Age */
#define HDRRUPC_CON_REQ_UNPACK_NEIGHBOR_MAX_AGE( ptr, val_ptr, aoffset, \
          voffset, dyn_threshhold_incl ) \
   HDRBIT_UNPACK8( (ptr), (val_ptr), \
         ((aoffset) + (voffset) + 31 + (dyn_threshhold_incl * 18)), 4 )

/* <EJECT> */
/*===========================================================================

          MACROS FOR PACKING CONFIG RESPONSE MESSAGE FIELDS

===========================================================================*/

#define HDRRUPC_CON_RSP_ATTRIB_VAL_SIZE 8   

/* Config response attributes are always just Attribute ID, AttributeValue */
#define HDRRUPC_CON_RSP_ATTRIB_LEN       ( HDRRUPC_ATTRIB_LEN_SIZE + \
                                       HDRRUPC_ATTRIB_ID_SIZE + \
                                       HDRRUPC_CON_RSP_ATTRIB_VAL_SIZE )

/* The field in the config response is length in bytes,
 * excluding the length of length field.
 */
#define HDRRUPC_CON_RSP_ATTRIB_LEN_BYTES  ( (HDRRUPC_ATTRIB_ID_SIZE + \
          HDRRUPC_CON_RSP_ATTRIB_VAL_SIZE) / HDRRUPC_BITS_PER_BYTE )        

/* Message ID  */
#define HDRRUPC_CON_RSP_PACK_MSG_ID( ptr, val_ptr )  \
          HDRBIT_PACK8( ptr, val_ptr, 0, 8 )

/* Transaction ID  */
#define HDRRUPC_CON_RSP_PACK_TRANSACTION_ID( ptr, val_ptr )  \
          HDRBIT_PACK8( (ptr), (val_ptr), 8, 8 )

/* Length of Config Response  */
#define HDRRUPC_CON_RSP_PACK_LEN( ptr, val_ptr, offset )  \
          HDRBIT_PACK8( (ptr), (val_ptr), ((offset) + 0), 8 )

/* Attribute ID  */
#define HDRRUPC_CON_RSP_PACK_ATTRIBUTE_ID( ptr, val_ptr, offset )  \
          HDRBIT_PACK8( (ptr), (val_ptr), ((offset) + 8), 8 )

/* Attribute Value for simple param */
#define HDRRUPC_CON_RSP_PACK_ATTRIBUTE_VALUE( ptr, val_ptr, offset ) \
          HDRBIT_PACK8( (ptr), (val_ptr), ((offset) + 16), 8 )

/* Value ID  */
#define HDRRUPC_CON_RSP_PACK_VALUE_ID( ptr, val_ptr, offset )  \
          HDRBIT_PACK8( (ptr), (val_ptr), ((offset) + 16), 8 )
          
typedef struct
{
  boolean need_init;
  uint8 value;
}hdrrupc_att_ovd_msg_seq_num;

/* The following flags are used to indicate whether the Set Mgmt Params are
   overwriten during connected state and need to be restored */
typedef struct
{
  boolean smsc;
  boolean smdc;
}hdrrupc_restore_params;

typedef enum
{
  HDRRUPC_CONFIG_REQ,
  HDRRUPC_ATTRIB_OVERRIDE,
  HDRRUPC_GAUP_REQ
}hdrrupc_type_of_msg_enum_type;

/* The following structure contains all the global variables used by the
   RUP Configuration Module */
LOCAL struct
{
  hdrrupc_config_params_type current;      /* Current Session Parameters  */
  hdrrupc_config_params_type negotiated;   /* Negotiated Parameters       */

  /* The following fields are used for attribute Override handling */
  hdrrupc_restore_params restore; /* Are set management params overridden */ 
  hdrrupc_att_ovd_msg_seq_num att_ovd_seq_num; /* attribute override msg seq num
                                               validation */
}hdrrupc;

/*=============================================================================

                       LOCAL FUNCTION DECLARATIONS

=============================================================================*/
LOCAL void hdrrupc_set_srch_param_default
(
  hdrsrch_search_params_struct_type *srch_params_ptr
);

LOCAL void hdrrupc_set_smsc_param_default
(
  hdrsrch_chan_mgmt_params_struct_type *smsc_params_ptr
);

LOCAL void hdrrupc_set_smdc_param_default
(
  hdrsrch_chan_mgmt_params_struct_type *smdc_params_ptr
);

LOCAL void hdrrupc_process_config_req_msg
( 
  dsm_item_type *item_ptr,
  hdrrupc_type_of_msg_enum_type msg_type
);

LOCAL void hdrrupc_process_attrib_override_msg
( 
  dsm_item_type *item_ptr
);

LOCAL boolean hdrrupc_process_srch_params_attrib
( 
  dsm_item_type *item_ptr,
  uint16 attrib_len, 
  uint16 attrib_offset,
  hdrsrch_search_params_struct_type *srch_params_ptr,
  uint8  *value_id_ptr
);

LOCAL boolean hdrrupc_process_set_mngmt_attrib
( 
  dsm_item_type *item_ptr,
  uint16 attrib_len, 
  uint16 attrib_offset,
  hdrsrch_chan_mgmt_params_struct_type *sm_params_ptr,
  uint8  *value_id_ptr
);

LOCAL boolean hdrrupc_process_override_allowed_attrib
(
  dsm_item_type * item_ptr,
  uint16 attrib_len,
  uint16 attrib_offset,
  uint8 *value_ptr
);

LOCAL void hdrrupc_process_session_changed( void );

boolean hdrrupc_process_rup_rad_mult_attrib
( 
  dsm_item_type *item_ptr,
  uint16 attrib_len,
  uint16 attrib_offset,
  uint8 *value_ptr 
);

boolean hdrrupc_process_rup_rad_add_attrib
(
  dsm_item_type *item_ptr,
  uint16 attrib_len,
  uint16 attrib_offset,
  uint16 *value_ptr
);

boolean hdrrupc_process_supp_rup_enh_attrib
(
  dsm_item_type *item_ptr,
  uint16 attrib_len,
  uint16 attrib_offset,
  uint8 *value_ptr
);

/*============================================================================

                        FUNCTION DEFINITIONS

=============================================================================*/

#ifdef FEATURE_LTE_TO_HDR
static __inline uint8 hdrrupc_reverse_byte( byte b )  
{ 
  uint8 rev = (b >> 4) | ((b & 0xf) << 4); 

  rev = ((rev & 0xcc) >> 2) | ((rev & 0x33) << 2); 
  rev = ((rev & 0xaa) >> 1) | ((rev & 0x55) << 1); 

  return rev; 
}
#endif

/*===========================================================================

FUNCTION HDRRUPC_POWERUP_INIT

DESCRIPTION
  This function performs the initialization of the Route Update 
  configuration negotiation module.  This funciton initializes all elements
  of the current and negotiated instances of hdrrupc_config_params_type.

DEPENDENCIES
  This function must called only once after powerup

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  All elements of the current and negotiated instances of 
  hdrrupc_config_params_type are initialized.

===========================================================================*/
void hdrrupc_powerup_init ( void )
{
  hdrrupc.restore.smdc = FALSE;
  hdrrupc.restore.smsc = FALSE;
  hdrrupc.att_ovd_seq_num.need_init = TRUE;

  /* Get the current configuration parameters from SCMDB */
  hdrscmdb_get_inuse_config(HDRHAI_ROUTE_UPDATE_PROTOCOL, &hdrrupc.current);

  /*-------------------------------------------------------------------------
  Set the searcher to the current configuration.
  -------------------------------------------------------------------------*/
  hdrlmac_set_search_params( &hdrrupc.current.srch, 
                            HDRHAI_ROUTE_UPDATE_PROTOCOL ); 

  hdrlmac_set_mgmt_same_channel_params( &hdrrupc.current.smsc, 
                            HDRHAI_ROUTE_UPDATE_PROTOCOL ); 

  hdrlmac_set_mgmt_diff_channel_params( &hdrrupc.current.smdc, 
                            HDRHAI_ROUTE_UPDATE_PROTOCOL ); 

} /* hdrrupc_powerup_init() */

/* <EJECT> */

/*===========================================================================

FUNCTION HDRRUPC_SET_SRCH_PARAM_DEFAULT

DESCRIPTION
  This function sets the  search parameters specified to their default values.

DEPENDENCIES
  The paramter should point to valid memory locations. Should not be NULL.
  
PARAMETERS
  srch_params_ptr - Location into which to fill in the Search Parameters 

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
===========================================================================*/
LOCAL void hdrrupc_set_srch_param_default
(
  hdrsrch_search_params_struct_type *srch_params_ptr 
)
{
  srch_params_ptr->pilot_inc = HDRRUPC_DEFAULT_PILOT_INC;
  srch_params_ptr->aset_win_size = HDRRUPC_DEFAULT_SEARCH_WIN_ACTIVE;
  srch_params_ptr->nset_win_size = HDRRUPC_DEFAULT_SEARCH_WIN_NEIGHBOR;
  srch_params_ptr->rset_win_size = HDRRUPC_DEFAULT_SEARCH_WIN_REMAINING;
} /* hdrrupc_set_srch_param_default() */

/* <EJECT> */

/*===========================================================================

FUNCTION HDRRUPC_SET_SMSC_PARAMS_DEFAULT

DESCRIPTION
  This function sets the Set Management Same Channel parameters specified to
  their default values.

DEPENDENCIES
  The paramter should point to valid memory locations. Should not be NULL.
  
PARAMETERS
  smsc_params_ptr - Location into which to fill the Set Management Same Channel
                    Params

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
===========================================================================*/
LOCAL void hdrrupc_set_smsc_param_default
(
  hdrsrch_chan_mgmt_params_struct_type *smsc_params_ptr
)
{
  smsc_params_ptr->pilot_add = HDRRUPC_DEFAULT_SAME_CHAN_PILOT_ADD;
  smsc_params_ptr->pilot_compare = HDRRUPC_DEFAULT_SAME_CHAN_PILOT_COMPARE;
  smsc_params_ptr->pilot_drop = HDRRUPC_DEFAULT_SAME_CHAN_PILOT_DROP;
  smsc_params_ptr->pilot_drop_timer =
                                HDRRUPC_DEFAULT_SAME_CHAN_PILOT_DROP_TIMER;
    smsc_params_ptr->dynamic_thresholds = FALSE;
  smsc_params_ptr->soft_slope = HDRRUPC_DEFAULT_SAME_CHAN_SOFT_SLOPE;
  smsc_params_ptr->add_intercept = HDRRUPC_DEFAULT_SAME_CHAN_ADD_INTERCEPT;
  smsc_params_ptr->drop_intercept = HDRRUPC_DEFAULT_SAME_CHAN_DROP_INTERCEPT;
  smsc_params_ptr->nset_max_age = HDRRUPC_DEFAULT_SAME_CHAN_NEIGHBOR_MAX_AGE;
} /* hdrrupc_set_smsc_param_default() */

/* <EJECT> */

/*===========================================================================

FUNCTION HDRRUPC_SET_SMDC_PARAMS_DEFAULT

DESCRIPTION
  This function sets the Set Management Different Channel parameters specified
  to their default values.

DEPENDENCIES
  The paramter should point to valid memory locations. Should not be NULL.
  
PARAMETERS
  smdc_params_ptr - Location into which to fill the Set Management Same Channel
                    Params

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
===========================================================================*/
LOCAL void hdrrupc_set_smdc_param_default
(
  hdrsrch_chan_mgmt_params_struct_type *smdc_params_ptr
)
{
  smdc_params_ptr->pilot_add = HDRRUPC_DEFAULT_DIFF_CHAN_PILOT_ADD;
  smdc_params_ptr->pilot_compare = HDRRUPC_DEFAULT_DIFF_CHAN_PILOT_COMPARE;
  smdc_params_ptr->pilot_drop = HDRRUPC_DEFAULT_DIFF_CHAN_PILOT_DROP;
  smdc_params_ptr->pilot_drop_timer = 
                                HDRRUPC_DEFAULT_DIFF_CHAN_PILOT_DROP_TIMER;
  smdc_params_ptr->dynamic_thresholds = FALSE;
  smdc_params_ptr->soft_slope = HDRRUPC_DEFAULT_DIFF_CHAN_SOFT_SLOPE;
  smdc_params_ptr->add_intercept = HDRRUPC_DEFAULT_DIFF_CHAN_ADD_INTERCEPT;
  smdc_params_ptr->drop_intercept = HDRRUPC_DEFAULT_DIFF_CHAN_DROP_INTERCEPT;
  smdc_params_ptr->nset_max_age = HDRRUPC_DEFAULT_DIFF_CHAN_NEIGHBOR_MAX_AGE;
} /* hdrrupc_set_smdc_param_default() */

/* <EJECT> */

/*===========================================================================

FUNCTION HDRRUPC_PROCESS_MSG

DESCRIPTION
  This function processes messages for RUPC.

DEPENDENCIES
  None

PARAMETERS
  msg_ptr - Pointer to the DSM item chain holding the message
  msg_id - The message ID already extracted from the DSM item

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrrupc_process_msg
(
  dsm_item_type   *msg_ptr,
  uint8           msg_id
)
{

  switch (msg_id)
  {

    case HDRRUPC_CON_REQ_MSG_ID:
      /*---------------------------------------------------------------------
      ConfigRequest message
      ---------------------------------------------------------------------*/
      HDR_MSG_PROT (MSG_LEGACY_HIGH, "Config request message received");
      hdrrupc_process_config_req_msg( msg_ptr, HDRRUPC_CONFIG_REQ );
      break;

    case HDRRUPC_AO_MSG_ID:
      /*---------------------------------------------------------------------
      AttributeOverride message
      ---------------------------------------------------------------------*/
      HDR_MSG_PROT (MSG_LEGACY_HIGH, "Attribute Override message received");
      hdrrupc_process_attrib_override_msg( msg_ptr );
      break;

    case HDRRUPC_ATTRIB_UPDATE_REQ_MSG_ID:
      HDR_MSG_PROT( MSG_LEGACY_HIGH, "Config request message received (GAUP)" );
      hdrrupc_process_config_req_msg( msg_ptr, HDRRUPC_GAUP_REQ );
      break;

    default:
      /*-----------------------------------------------------------------------
      Default: message not recognized.
      -----------------------------------------------------------------------*/
      ERR_FATAL( "Unknown message", 0, 0, 0 );
      break;
  }
} /* hdrrupc_process_msg() */

/* <EJECT> */

/*===========================================================================

FUNCTION HDRRUPC_PROCESS_CONFIG_REQ_MSG

DESCRIPTION
  This function processes the Configuration Request message and also the 
  Attribute Override message since it just overrides the current config.

DEPENDENCIES
  None
                      
PARAMETERS
  item_ptr - Pointer to the DSM item chain holding the message
  msg_type - Which type of msg are we processing
             (HDRRUPC_CONFIG_REQ or HDRRUPC_ATTRIB_OVERRIDE)
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void hdrrupc_process_config_req_msg
( 
  dsm_item_type *item_ptr,
  hdrrupc_type_of_msg_enum_type msg_type
)
{
  int total_bits_remaining = 0; /* Number of bits remaining in the message */
  uint8  creq_tran_id_or_attr_ovrd_seq; /* Trans ID of the config req or seq
                                            num of the attribute override */
  uint8  attrib_len_in_bytes = 0;
  uint16 attrib_len; /* Bit length of attribute, including the length field */
  uint16 attrib_offset = 0; /* Bit offset of current attribute */
  uint8  attrib_id; /* ID field of attribute */

  /* If during processing we end up in a situation where we don't know what
   * attribute ID the AN included, or aren't sure if they meant to include
   * one, we can't send a response because we may be accepting to use the 
   * default value for an attribute that we are not actually using the 
   * default value for (e.g. if we negotiated a non-default value during
   * AT initiated negotiation).  
   */
  boolean send_response = TRUE;

  /* As we parse the message we update the negotiated parameters in a 
   * local copy as we agree to accept new values.  That way, if we 
   * later encounter an error and decide not to send a response, or if 
   * the same attribute is negotiated twice and we accept the first but
   * decline the second, we don't corrupt the real negotiated parameters.
   */

  /* For each simple attribute we need a flag if we are accpeting it
   * and the value we are accepting.
   */
  struct
  {
    boolean is_accepted;
    uint8 value_id;
    hdrsrch_search_params_struct_type value;
  } srch_params_attrib;

  struct
  {
    boolean is_accepted;
    uint8 value_id;
    hdrsrch_chan_mgmt_params_struct_type value;
  } smsc_params_attrib;

  struct
  {
    boolean is_accepted;
    uint8 value_id;
    hdrsrch_chan_mgmt_params_struct_type value;
  } smdc_params_attrib;

  struct
  {
    boolean is_accepted;
    uint8 value;
  } smoa_config_attrib;

  struct
  {
    boolean is_accepted;
    uint8 value;
  } route_update_radius_mult_attrib;

  struct
  {
    boolean is_accepted;
    uint16 value;
  } route_update_radius_add_attrib;

  struct
  {
    boolean is_accepted;
    uint8 value;
  } support_route_update_enh_attrib;

  dsm_item_type *send_msg_ptr; /* dsm buffer for sent message */

  boolean srch_params_attrib_is_included = FALSE;
  boolean smsc_params_attrib_is_included = FALSE;
  boolean smdc_params_attrib_is_included = FALSE;
  boolean smoa_params_attrib_is_included = FALSE;

  boolean route_update_radius_mult_attrib_is_included = FALSE;
  boolean route_update_radius_add_attrib_is_included = FALSE;
  boolean support_route_update_enh_attrib_is_included = FALSE;
  boolean unknown_attrib_is_included = FALSE;

  boolean max_num_fl_supported_attrib_is_included = FALSE;
  boolean max_num_rl_supported_attrib_is_included = FALSE;
  boolean max_fl_bdwidth_no_jammer_attrib_is_included = FALSE;
  boolean max_fl_bdwidth_w_jammer_attrib_is_included = FALSE;
  boolean max_rl_bdwidth_attrib_is_included = FALSE;
  boolean max_num_sub_asets_attrib_is_included = FALSE;
  boolean fwd_feedback_mux_supported_attrib_is_included = FALSE;

  hdrhai_protocol_instance_enum_type protocol_instance;

  /* Retrieve the current SupoprtedRouteUpdateEnhancements attrib */
  const uint8 support_rup_enhancements = hdrrupc_get_supp_rup_enh_attrib();
  const uint16 rup_subtype = hdrscp_get_config_subtype(HDRHAI_ROUTE_UPDATE_PROTOCOL);

  boolean gaup_is_accepted = TRUE;
    /* Flag to indicate if GAUP message from AN is accepted or not */  
  uint8 gaup_failed_attrib_id = 0;
    /* Attribute that AN GAUP'ed and AT rejected */
  static hdrscp_scp_fail_ind_type gaup_fail_data;
    /* Data for AN GAUP failed indication */
  hdrind_ind_data_union_type ind_data;
/*- - - - - - - - - - - - -- - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (msg_type == HDRRUPC_ATTRIB_OVERRIDE)
  {
    /* AttributeOverride message is only supported in rev0/A.
     * In hdrrup_process_msg(), AttributeOverride msg is dropped if MC RUP
     * is negotiated.*/
#ifdef FEATURE_LTE_TO_HDR_OH
    ASSERT((rup_subtype == HDRSCP_DEFAULT_RUP)||(rup_subtype == HDRSCP_IRAT_RUP));
#else /* FEATURE_LTE_TO_HDR_OH */
    ASSERT(rup_subtype == HDRSCP_DEFAULT_RUP);
#endif /* FEATURE_LTE_TO_HDR_OH */
  }

  /* Initialize local varariables */
  memset( &srch_params_attrib, 0, sizeof(srch_params_attrib) );
  memset( &smsc_params_attrib, 0, sizeof(smsc_params_attrib) );
  memset( &smdc_params_attrib, 0, sizeof(smdc_params_attrib) );

  srch_params_attrib.is_accepted = FALSE;
  smsc_params_attrib.is_accepted = FALSE;
  smdc_params_attrib.is_accepted = FALSE;
  smoa_config_attrib.is_accepted = FALSE;

  route_update_radius_mult_attrib.is_accepted = FALSE;
  route_update_radius_add_attrib.is_accepted = FALSE;
  support_route_update_enh_attrib.is_accepted = FALSE;

  total_bits_remaining = (dsm_length_packet(item_ptr) * 
                               HDRRUPC_BITS_PER_BYTE );

  /*---------------------------------------------------------------------
  Extract the Transaction ID or Sequence Number from the message 
  ---------------------------------------------------------------------*/
  if( HDRRUPC_CON_REQ_UNPACK_TRANSACTION_ID( 
        item_ptr,&creq_tran_id_or_attr_ovrd_seq ) != E_SUCCESS )
  {
    if (msg_type == HDRRUPC_ATTRIB_OVERRIDE)
    {
      ERR("Error extracting Seq Num from AttOvd Msg", 0, 0, 0 );
    }
    else
    {
      ERR("Error extracting TransactionID from ConfigReq Msg", 0, 0, 0 );
    }
    return;
  }

  /*---------------------------------------------------------------------
  Validate the sequence number if this is an override msg
  ---------------------------------------------------------------------*/
  if (msg_type == HDRRUPC_ATTRIB_OVERRIDE)
  {
    /* We initialize the receive ptr to the MessageSequence field of the
       first AttribOvrd msg that is received in the Connected State */
    if ( hdrrupc.att_ovd_seq_num.need_init )
    {
      hdrrupc.att_ovd_seq_num.value = creq_tran_id_or_attr_ovrd_seq;
      hdrrupc.att_ovd_seq_num.need_init = FALSE;
    }
    /* For subsequent override msgs, validate the seq num according to 
       normal rules */
    else
    {
      if(hdrutil_msg_seq_num_is_greater(creq_tran_id_or_attr_ovrd_seq, 
                                        hdrrupc.att_ovd_seq_num.value, 8))
      {
        hdrrupc.att_ovd_seq_num.value = creq_tran_id_or_attr_ovrd_seq;
      }
      else
      {
        HDRTRACE_RXMSG_IGNORED_SEQ( HDRHAI_ROUTE_UPDATE_PROTOCOL,
                 HDRRUPC_AO_MSG_ID, hdrrups_get_state()); //lint !e506
        return;
      }
    }
  }

  /* Initialize attrib_len so the initial attrib_offset will be
   * immediately after the transaction ID of the config request.
   */
  attrib_len = HDRRUPC_MSG_ID_SIZE + HDRRUPC_TRANSACTION_ID_SIZE; 

  /*---------------------------------------------------------------------
  Unpack the configuration attributes 
  ---------------------------------------------------------------------*/

  /* While there are at least enough bits for the smallest attribute... */ 
  while ((total_bits_remaining - attrib_len) >= HDRRUPC_MIN_ATTRIB_SIZE)
  {
    /* Move to the next attribute record */
    attrib_offset += attrib_len;
    total_bits_remaining -= attrib_len;

    /* Get the length of the new attribute */
    if( HDRRUPC_CON_REQ_UNPACK_LEN( item_ptr, &attrib_len_in_bytes, 
                                    attrib_offset ) != 
        E_SUCCESS )
    {
      if (msg_type == HDRRUPC_ATTRIB_OVERRIDE)
      {
        ERR("Error unpacking AttribOvrd Msg", 0, 0, 0 );
      }
      else
      {
        ERR("Error unpacking ConfigReq Msg", 0, 0, 0 );
      }
      /* Without the length we can't continue to parse the message. */
      send_response = FALSE;
      break;
    }

    /* The AttributeLength field in the message is the attribute length in 
     * bytes, excluding the length of the length field.  For our purposes, 
     * we want attrib_len to be the total length of the attribute in bits, 
     * including the length field.
     */
    attrib_len = attrib_len_in_bytes * HDRRUPC_BITS_PER_BYTE;
    attrib_len += HDRRUPC_ATTRIB_LEN_SIZE;

    /* Any attribute record must have at least an attribute ID and a value */
    if ((attrib_len < HDRRUPC_MIN_ATTRIB_SIZE) ||
        (attrib_len > total_bits_remaining ))
    {
      /* If the length is something unexpected, we can't be sure of what 
       * the AN intended and it is safest to not send a response
       */
      send_response = FALSE;
      break;
    }

    /* There is at least one more attribute record in the message */

    if( HDRRUPC_CON_REQ_UNPACK_ATTRIBUTE_ID( item_ptr, &attrib_id, 
        attrib_offset ) != E_SUCCESS )
    {
      if (msg_type == HDRRUPC_ATTRIB_OVERRIDE)
      {
        ERR("Error extracting AttributeID from AttrOvrd Msg", 0, 0, 0 );
      }
      else
      {
        ERR("Error extracting AttributeID from ConfigReq Msg", 0, 0, 0 );
      }
      /* If we can't get the attribute_id, don't send a response */
      send_response = FALSE;
      break;
    }
    
    if( msg_type == HDRRUPC_ATTRIB_OVERRIDE &&
        attrib_id != HDRRUPC_SET_MNGMT_DIFF_CHAN_PARAMS_ATTRIB_ID && 
        attrib_id != HDRRUPC_SET_MNGMT_SAME_CHAN_PARAMS_ATTRIB_ID )
    {
      ERR( "Attrib not allowed in AttrOvrd msg (%d)", attrib_id, 0, 0 );
      continue;
    }

    /* Process the attribute.  Note that if the same attribute appears 
     * multiple times in the message, the last one overwrites the others.
     */
    switch( attrib_id )
    {
      case HDRRUPC_SRCH_PARAMS_ATTRIB_ID: 
          srch_params_attrib_is_included = TRUE;
          if (attrib_len >= HDRRUPC_SRCH_PARAMS_MIN_ATTRIB_SIZE)
          {
            srch_params_attrib.is_accepted = hdrrupc_process_srch_params_attrib( 
                    item_ptr, attrib_len, attrib_offset, 
                    &srch_params_attrib.value, &srch_params_attrib.value_id );
          }
          else
          {
            ERR( "Not enough bits for attribute 0x%x", attrib_id, 0, 0 );
            /* If the AN didn't include enough bits for at least one complete
             * attribute value we don't know what the AN intended, so it is 
             * safest not to send a response.
             */
            send_response = FALSE;
          }

        break;

      case HDRRUPC_SET_MNGMT_SAME_CHAN_PARAMS_ATTRIB_ID: 
        smsc_params_attrib_is_included = TRUE;
        if (attrib_len >= HDRRUPC_SET_MNGMT_PARAMS_MIN_ATTRIB_SIZE)
        {
          smsc_params_attrib.is_accepted = hdrrupc_process_set_mngmt_attrib(
                  item_ptr, attrib_len, attrib_offset, 
                  &smsc_params_attrib.value, &smsc_params_attrib.value_id );
        }
        else
        {
          ERR( "Not enough bits for attribute 0x%x", attrib_id, 0, 0 );
          /* If the AN didn't include enough bits for at least one complete
           * attribute value we don't know what the AN intended, so it is 
           * safest not to send a response.
           */
          send_response = FALSE;
        }

        break;

      case HDRRUPC_SET_MNGMT_DIFF_CHAN_PARAMS_ATTRIB_ID: 
        if (rup_subtype == HDRSCP_MC_RUP
#ifdef FEATURE_LTE_TO_HDR_OH
            || rup_subtype == HDRSCP_IRAT_MC_RUP
#endif /* FEATURE_LTE_TO_HDR_OH */               
           )
        {
          unknown_attrib_is_included = TRUE;
          ERR("MC RUP: Unknown Attribute ID 0x%x", attrib_id, 0, 0 );
        }
        else
        {
          smdc_params_attrib_is_included = TRUE;
          if (attrib_len >= HDRRUPC_SET_MNGMT_PARAMS_MIN_ATTRIB_SIZE)
          {
            smdc_params_attrib.is_accepted = hdrrupc_process_set_mngmt_attrib(
                    item_ptr, attrib_len, attrib_offset, 
                    &smdc_params_attrib.value, &smdc_params_attrib.value_id );
          }
          else
          {
            ERR( "Not enough bits for attribute 0x%x", attrib_id, 0, 0 );
            /* If the AN didn't include enough bits for at least one complete
             * attribute value we don't know what the AN intended, so it is 
             * safest not to send a response.
             */
            send_response = FALSE;
          }
        }

        break;

      case HDRRUPC_SET_MNGMT_OVERRIDE_ALLOWED_ATTRIB_ID:
        if (rup_subtype == HDRSCP_MC_RUP
#ifdef FEATURE_LTE_TO_HDR_OH
            || rup_subtype == HDRSCP_IRAT_MC_RUP
#endif /* FEATURE_LTE_TO_HDR_OH */   
            )
        {
          unknown_attrib_is_included = TRUE;
          ERR("MC RUP: Unknown Attribute ID 0x%x", attrib_id, 0, 0 );
        }
        else
        {
          smoa_params_attrib_is_included = TRUE;
          if (attrib_len >= HDRRUPC_SET_MNGMT_OVERRIDE_ALLOWED_ATTRIB_SIZE)
          {
            smoa_config_attrib.is_accepted = 
              hdrrupc_process_override_allowed_attrib(
                    item_ptr, attrib_len, attrib_offset, 
                    &smoa_config_attrib.value);
          }
          else
          {
            ERR( "Not enough bits for attribute 0x%x", attrib_id, 0, 0 );
            /* If the AN didn't include enough bits for at least one complete
             * attribute value we don't know what the AN intended, so it is 
             * safest not to send a response.
             */
            send_response = FALSE;
          }
        }

        break;

      case HDRRUPC_ROUTE_UPDATE_RADIUS_MULT_ATTRIB_ID:
        route_update_radius_mult_attrib_is_included = TRUE;
        if( attrib_len >= HDRRUPC_ROUTE_UPDATE_RADIUS_MULT_ATTRIB_SIZE )
        {
          route_update_radius_mult_attrib.is_accepted =
            hdrrupc_process_rup_rad_mult_attrib( 
              item_ptr, attrib_len, attrib_offset, 
              &route_update_radius_mult_attrib.value );
        }
        else
        {
          ERR( "Not enough bits for attribute 0x%x", attrib_id, 0, 0 );
          /* If the AN didn't include enough bits for at least one complete
           * attribute value we don't know what the AN intended, so it is 
           * safest not to send a response.
           */
          send_response = FALSE;
        }
        
        break;

      case HDRRUPC_ROUTE_UPDATE_RADIUS_ADD_ATTRIB_ID:
        route_update_radius_add_attrib_is_included = TRUE;
        if( attrib_len >= HDRRUPC_ROUTE_UPDATE_RADIUS_ADD_ATTRIB_SIZE )
        {
          route_update_radius_add_attrib.is_accepted =
            hdrrupc_process_rup_rad_add_attrib(
                    item_ptr, attrib_len, attrib_offset, 
              &route_update_radius_add_attrib.value );
        }
        else
        {
          ERR( "Not enough bits for attribute 0x%x", attrib_id, 0, 0 );
          /* If the AN didn't include enough bits for at least one complete
           * attribute value we don't know what the AN intended, so it is 
           * safest not to send a response.
           */
          send_response = FALSE;
        }

        break;

      case HDRRUPC_SUPPORT_ROUTE_UPDATE_ENH_ATTRIB_ID:
        support_route_update_enh_attrib_is_included = TRUE;
        if( attrib_len >= HDRRUPC_SUPPORT_ROUTE_UPDATE_ENH_ATTRIB_SIZE )
        {
          support_route_update_enh_attrib.is_accepted = 
            hdrrupc_process_supp_rup_enh_attrib(
              item_ptr, attrib_len, attrib_offset,
              &support_route_update_enh_attrib.value );
        }
        else
        {
          ERR( "Not enough bits for attribute 0x%x", attrib_id, 0, 0 );
          /* If the AN didn't include enough bits for at least one complete
           * attribute value we don't know what the AN intended, so it is 
           * safest not to send a response.
           */
          send_response = FALSE;
        }
        
        break;

    /* For following revB attributes that shouldn't be proposed by the AN, if AN proposed, 
     * we just simply "reject" them and fall back to "in used" values. We don't even 
     * unpack their values.
     * If the AN does not pack attribute per standard ( e.g. pack a 8 bits value as 16 bits
     * value), the AT may get incorrect information. This shouldn't happen. 
     */
    case HDRRUPC_MAX_NUMBER_FL_SUPPORTED_ATTRIB_ID:
      max_num_fl_supported_attrib_is_included = TRUE;
      if (rup_subtype == HDRSCP_MC_RUP
#ifdef FEATURE_LTE_TO_HDR_OH
          || rup_subtype == HDRSCP_IRAT_MC_RUP
#endif /* FEATURE_LTE_TO_HDR_OH */   
         )
      {
        ERR("MC RUP: AN should not propose Attribute ID 0x%x", attrib_id, 0, 0 );
        hdrrupc.negotiated.max_num_fl_supported_attrib = 
                                       hdrrupc.current.max_num_fl_supported_attrib;
      }
      else
      {
        unknown_attrib_is_included = TRUE;
        ERR("RUP: Unknown Attribute ID 0x%x", attrib_id, 0, 0 );
      }

      break;

    case HDRRUPC_MAX_NUMBER_RL_SUPPORTED_ATTRIB_ID:
      max_num_rl_supported_attrib_is_included = TRUE;
      if (rup_subtype == HDRSCP_MC_RUP
#ifdef FEATURE_LTE_TO_HDR_OH
          || rup_subtype == HDRSCP_IRAT_MC_RUP
#endif /* FEATURE_LTE_TO_HDR_OH */   
         )
      {
        ERR("MC RUP: AN should not propose Attribute ID 0x%x", attrib_id, 0, 0 );
        hdrrupc.negotiated.max_num_rl_supported_attrib = 
                                       hdrrupc.current.max_num_rl_supported_attrib;
      }
      else
      {
        unknown_attrib_is_included = TRUE;
        ERR("RUP: Unknown Attribute ID 0x%x", attrib_id, 0, 0 );
      }

      break;

    case HDRRUPC_MAX_FL_BANDWIDTH_NO_JAMMER_ATTRIB_ID:
      max_fl_bdwidth_no_jammer_attrib_is_included = TRUE;
      if (rup_subtype == HDRSCP_MC_RUP
#ifdef FEATURE_LTE_TO_HDR_OH
          || rup_subtype == HDRSCP_IRAT_MC_RUP
#endif /* FEATURE_LTE_TO_HDR_OH */   
         )
      {
        ERR("MC RUP: AN should not propose Attribute ID 0x%x", attrib_id, 0, 0 );
        hdrrupc.negotiated.max_fl_bdwidth_no_jammer_attrib = 
                                       hdrrupc.current.max_fl_bdwidth_no_jammer_attrib;
      }
      else
      {
        unknown_attrib_is_included = TRUE;
        ERR("RUP: Unknown Attribute ID 0x%x", attrib_id, 0, 0 );
      }

      break;

    case HDRRUPC_MAX_FL_BANDWIDTH_W_JAMMER_ATTRIB_ID:
      max_fl_bdwidth_w_jammer_attrib_is_included = TRUE;
      if (rup_subtype == HDRSCP_MC_RUP
#ifdef FEATURE_LTE_TO_HDR_OH
          || rup_subtype == HDRSCP_IRAT_MC_RUP
#endif /* FEATURE_LTE_TO_HDR_OH */   
         )
      {
        ERR("MC RUP: AN should not propose Attribute ID 0x%x", attrib_id, 0, 0 );
        hdrrupc.negotiated.max_fl_bdwidth_w_jammer_attrib = 
                                       hdrrupc.current.max_fl_bdwidth_w_jammer_attrib;
      }
      else
      {
        unknown_attrib_is_included = TRUE;
        ERR("RUP: Unknown Attribute ID 0x%x", attrib_id, 0, 0 );
      }

      break;

    case HDRRUPC_MAX_RL_BANDWIDTH_ATTRIB_ID:
      max_rl_bdwidth_attrib_is_included = TRUE;
      if (rup_subtype == HDRSCP_MC_RUP
#ifdef FEATURE_LTE_TO_HDR_OH
          || rup_subtype == HDRSCP_IRAT_MC_RUP
#endif /* FEATURE_LTE_TO_HDR_OH */   
         )
      {
        ERR("MC RUP: AN should not propose Attribute ID 0x%x", attrib_id, 0, 0 );
        hdrrupc.negotiated.max_rl_bdwidth_attrib = 
                                       hdrrupc.current.max_rl_bdwidth_attrib;
      }
      else
      {
        unknown_attrib_is_included = TRUE;
        ERR("RUP: Unknown Attribute ID 0x%x", attrib_id, 0, 0 );
      }

      break;

    case HDRRUPC_MAX_NUMBER_SUB_ASETS_ATTRIB_ID:
      max_num_sub_asets_attrib_is_included = TRUE;
      if (rup_subtype == HDRSCP_MC_RUP
#ifdef FEATURE_LTE_TO_HDR_OH
          || rup_subtype == HDRSCP_IRAT_MC_RUP
#endif /* FEATURE_LTE_TO_HDR_OH */   
         )
      {
        ERR("MC RUP: AN should not propose Attribute ID 0x%x", attrib_id, 0, 0 );
        hdrrupc.negotiated.max_num_sub_asets_attrib = 
                                       hdrrupc.current.max_num_sub_asets_attrib;
      }
      else
      {
        unknown_attrib_is_included = TRUE;
        ERR("RUP: Unknown Attribute ID 0x%x", attrib_id, 0, 0 );
      }

      break;

    case HDRRUPC_FWD_FEEDBACK_MUX_SUPPORTED_ATTRIB_ID: 
      fwd_feedback_mux_supported_attrib_is_included = TRUE;
      if (rup_subtype == HDRSCP_MC_RUP
#ifdef FEATURE_LTE_TO_HDR_OH
          || rup_subtype == HDRSCP_IRAT_MC_RUP
#endif /* FEATURE_LTE_TO_HDR_OH */   
         )
      {
        ERR("MC RUP: AN should not propose Attribute ID 0x%x", attrib_id, 0, 0 );
        hdrrupc.negotiated.fwd_fb_mux_supp_attrib = 
                                       hdrrupc.current.fwd_fb_mux_supp_attrib;
      }
      else
      {
        unknown_attrib_is_included = TRUE;
        ERR("RUP: Unknown Attribute ID 0x%x", attrib_id, 0, 0 );
      }

      break;

      default:
        unknown_attrib_is_included = TRUE;
        ERR("Unknown Attribute ID 0x%x", attrib_id, 0, 0 );
        break;
    }

    if (!send_response)
    {
      /* No need to continue parsing once we decide not to send a response */
      break;
    }
  } /* while( there are more attributes in the config request ) */

  /**************************************************************************
   * Per EV-DO standard, if AT rejected AN's proposal for a particular      *
   * attribute, AT needs to set the value to the fallback value for         *
   * that attribute, ie, that of the current in_use stance value.           *
   * The hdrrupc.negotiated structures contains the value after AT_INIT     *
   * negotiation, and might be different to the in_use value. The           *
   * negotiated variable will be copied into hdrrupc.current structure,     *
   * overwriting the in_use stance value after Processing of ConfigReq.     *
   * Therefore, rolling back is needed if 1) AT proposed a value during     *
   * AT_INIT state, 2) AT does not use scmdb functions to manage            *
   * configuration. To rollback a value if an attribute is not accepted:    *                                                        *
   *   if (route_update_radius_mult_attrib_is_included &&                   *
   *       !route_update_radius_mult_attrib.is_accepted)                    *
   *   {                                                                    *
   *     hdrrupc.negotiated.rup_rad_mult_attrib =                           *
   *             hdrrupc.current.rup_rad_mult_attrib;                       *
   *       }                                                                *
   *  }                                                                     *
   **************************************************************************/
  /*---------------------------------------------------------------------
  Override set management parameters if attrib override msg. 
  ---------------------------------------------------------------------*/
  if (msg_type == HDRRUPC_ATTRIB_OVERRIDE)
  {
    /*---------------------------------------------------------------------
    Send diff channel override parameters directly to search if allowed.
    ---------------------------------------------------------------------*/
    if (smdc_params_attrib.is_accepted && 
        hdrrupc.current.smoa == HDRRUPC_ATTRIB_OVERRIDE_ALLOWED )
    {
      HDR_MSG_PROT (MSG_LEGACY_MED, "Override Set Mgmt Diff Channel Params");
      hdrlmac_set_mgmt_diff_channel_params( &smdc_params_attrib.value, 
                                            HDRHAI_ROUTE_UPDATE_PROTOCOL ); 
      hdrrupc.restore.smdc = TRUE;
    }
    
    /*---------------------------------------------------------------------
    Send same channel override parameters directly to search if allowed
    ---------------------------------------------------------------------*/
    if (smsc_params_attrib.is_accepted && 
        hdrrupc.current.smoa == HDRRUPC_ATTRIB_OVERRIDE_ALLOWED )
    {
      HDR_MSG_PROT (MSG_LEGACY_MED, "Override Set Mgmt Same Channel Params");
      hdrlmac_set_mgmt_same_channel_params( &smsc_params_attrib.value, 
                                            HDRHAI_ROUTE_UPDATE_PROTOCOL );     
      hdrrupc.restore.smsc = TRUE;
    }
  }
  else if (msg_type == HDRRUPC_GAUP_REQ)
  {
    if (support_rup_enhancements == 0x00 && 
        (route_update_radius_mult_attrib_is_included ||
         route_update_radius_add_attrib_is_included))
    {
     /* If support_rup_enhancements is not set then AN is not allowed to GAUP
      * AT with RouteUpdateRadiusMultiply or RouteUpdateRadiusAdd,
      * in this case just ignore the message do not send session close */
      ERR( "SupportRouteUpdateEnhancements is not negotiated to accept GAUP Radius attrib",
            0, 0, 0 );
      send_response = FALSE;
    }
    else if(srch_params_attrib_is_included || 
            smdc_params_attrib_is_included || 
            smoa_params_attrib_is_included ||
            support_route_update_enh_attrib_is_included ||
            unknown_attrib_is_included ||
            (route_update_radius_mult_attrib_is_included && 
            !route_update_radius_mult_attrib.is_accepted) ||
            (route_update_radius_add_attrib_is_included && 
            !route_update_radius_add_attrib.is_accepted) ||
            max_num_fl_supported_attrib_is_included ||
            max_num_rl_supported_attrib_is_included ||
            max_fl_bdwidth_no_jammer_attrib_is_included ||
            max_fl_bdwidth_w_jammer_attrib_is_included ||
            max_rl_bdwidth_attrib_is_included ||
            max_num_sub_asets_attrib_is_included ||
            fwd_feedback_mux_supported_attrib_is_included ||
            (smsc_params_attrib_is_included &&
             (!smsc_params_attrib.is_accepted || 
              rup_subtype == HDRSCP_DEFAULT_RUP)))
    {
      send_response = FALSE;
      gaup_is_accepted = FALSE;

      if (route_update_radius_mult_attrib_is_included && 
          !route_update_radius_mult_attrib.is_accepted)
      {
        gaup_failed_attrib_id = HDRRUPC_ROUTE_UPDATE_RADIUS_MULT_ATTRIB_ID;
      }
      else if (route_update_radius_add_attrib_is_included && 
              !route_update_radius_add_attrib.is_accepted)
      {
        gaup_failed_attrib_id = HDRRUPC_ROUTE_UPDATE_RADIUS_ADD_ATTRIB_ID;
      }
      else if (srch_params_attrib_is_included) 
      {
        gaup_failed_attrib_id = HDRRUPC_SRCH_PARAMS_ATTRIB_ID;
      }
      else if (smdc_params_attrib_is_included)
      {
        gaup_failed_attrib_id = HDRRUPC_SET_MNGMT_DIFF_CHAN_PARAMS_ATTRIB_ID;
      }
      else if (smoa_params_attrib_is_included)
      {
        gaup_failed_attrib_id = HDRRUPC_SET_MNGMT_OVERRIDE_ALLOWED_ATTRIB_ID;
      }
      else if (support_route_update_enh_attrib_is_included)
      {
        gaup_failed_attrib_id = HDRRUPC_SUPPORT_ROUTE_UPDATE_ENH_ATTRIB_ID;
      }
      else if (max_num_fl_supported_attrib_is_included)
      {
        gaup_failed_attrib_id = HDRRUPC_MAX_NUMBER_FL_SUPPORTED_ATTRIB_ID;
      }
      else if (max_num_rl_supported_attrib_is_included)
      {
        gaup_failed_attrib_id = HDRRUPC_MAX_NUMBER_RL_SUPPORTED_ATTRIB_ID;
      }
      else if (max_fl_bdwidth_no_jammer_attrib_is_included)
      {
        gaup_failed_attrib_id = HDRRUPC_MAX_FL_BANDWIDTH_NO_JAMMER_ATTRIB_ID;
      }
      else if (max_fl_bdwidth_w_jammer_attrib_is_included)
      {
        gaup_failed_attrib_id = HDRRUPC_MAX_FL_BANDWIDTH_W_JAMMER_ATTRIB_ID;
      }
      else if (max_rl_bdwidth_attrib_is_included)
      {
        gaup_failed_attrib_id = HDRRUPC_MAX_RL_BANDWIDTH_ATTRIB_ID;
      }
      else if (max_num_sub_asets_attrib_is_included)
      {
        gaup_failed_attrib_id = HDRRUPC_MAX_NUMBER_SUB_ASETS_ATTRIB_ID;
      }
      else if (smsc_params_attrib_is_included) 
      {
        gaup_failed_attrib_id = HDRRUPC_SET_MNGMT_SAME_CHAN_PARAMS_ATTRIB_ID;
      }
      else if (fwd_feedback_mux_supported_attrib_is_included) 
      {
        gaup_failed_attrib_id = HDRRUPC_FWD_FEEDBACK_MUX_SUPPORTED_ATTRIB_ID;
      }
    }
  }

  /*---------------------------------------------------------------------
  Form and send the Configuration Response message. 
  ---------------------------------------------------------------------*/
  if (send_response)
  {
    /* get new buffer */
    send_msg_ptr = hdrutil_new_msg_buffer();
    
    /*=====================================================================
    Pack and send response for Attribute Override Msg
    =====================================================================*/
    if (msg_type == HDRRUPC_ATTRIB_OVERRIDE)
    {
      /*---------------------------------------------------------------------
      Pack msg ID
      ---------------------------------------------------------------------*/
      HDRRUPC_CON_RSP_PACK_MSG_ID( send_msg_ptr, HDRRUPC_AOR_MSG_ID);

      /*---------------------------------------------------------------------
      Pack msg seq number or transaction ID.
      ---------------------------------------------------------------------*/
      HDRRUPC_CON_RSP_PACK_TRANSACTION_ID( send_msg_ptr, 
                                           creq_tran_id_or_attr_ovrd_seq);

      /*---------------------------------------------------------------------
      Send config response on RTC. Attribute Override msg is best effort.
      ---------------------------------------------------------------------*/
      hdrhmp_send_msg( HDRHAI_ROUTE_UPDATE_PROTOCOL,
                       HDRHAI_RTC,
                       FALSE,
                       HDRRUPC_AOR_MSG_PRIORITY,
                       HDRHAI_IN_USE, 
                       send_msg_ptr,
                       NULL,
                       NULL );

    }
    /*=====================================================================
    Pack and send response for Config Request Msg
    =====================================================================*/
    else
    {
      /*---------------------------------------------------------------------
      Pack message ID
      ---------------------------------------------------------------------*/
      if( msg_type == HDRRUPC_GAUP_REQ )
      {
        HDRRUPC_CON_RSP_PACK_MSG_ID( send_msg_ptr, 
                                     HDRRUPC_ATTRIB_UPDATE_ACCEPT_MSG_ID );
      }
      else
      {
        HDRRUPC_CON_RSP_PACK_MSG_ID( send_msg_ptr, HDRRUPC_CON_RSP_MSG_ID);
      }

      /*---------------------------------------------------------------------
      Pack msg seq number or transaction ID.
      ---------------------------------------------------------------------*/
      HDRRUPC_CON_RSP_PACK_TRANSACTION_ID( send_msg_ptr, 
                                           creq_tran_id_or_attr_ovrd_seq);

      /* Now pack in the attribute ID and value of all attributes we have 
       * flagged that we want to accept.
       */
  
      /* Re-use attrib_offset variable for message packing */
      attrib_offset = HDRRUPC_MSG_ID_SIZE + HDRRUPC_TRANSACTION_ID_SIZE;

      /*---------------------------------------------------------------------
      Pack response for the Search Parameters attribute
      ---------------------------------------------------------------------*/
      if (srch_params_attrib.is_accepted)
      {
        /* Include that we are accepting the search param attribute */
        HDRRUPC_CON_RSP_PACK_LEN( send_msg_ptr, HDRRUPC_CON_RSP_ATTRIB_LEN_BYTES,
                               attrib_offset );
        HDRRUPC_CON_RSP_PACK_ATTRIBUTE_ID( send_msg_ptr, 
          HDRRUPC_SRCH_PARAMS_ATTRIB_ID, attrib_offset );
        HDRRUPC_CON_RSP_PACK_VALUE_ID( send_msg_ptr, 
          srch_params_attrib.value_id,
          attrib_offset );
  
        attrib_offset += HDRRUPC_CON_RSP_ATTRIB_LEN;

        /* Update our regional structure to reflect the newly accepted values.
         * This is done per attribute because if an attribute was included twice
         * and the second was not accepted, negotiated_values still has the
         * values from the first one, but those were not actually negotiated.
         */
        hdrrupc.negotiated.srch = srch_params_attrib.value;
  
      }
  
      /*---------------------------------------------------------------------
      Pack response for the SetManagementSameChannel Parameters attribute
      ---------------------------------------------------------------------*/
      if (smsc_params_attrib.is_accepted)
      {
        if( (msg_type == HDRRUPC_GAUP_REQ) && 
            ((rup_subtype == HDRSCP_MC_RUP)
#ifdef FEATURE_LTE_TO_HDR_OH
            || (rup_subtype == HDRSCP_IRAT_MC_RUP)
#endif /* FEATURE_LTE_TO_HDR_OH */   
          ))
        {
          /* If this configuration was in response to a GAUP request, begin using
              the value immediately */
          hdrrupc.current.smsc = smsc_params_attrib.value;

          HDR_MSG_PROT (MSG_LEGACY_MED, "GAUP Set Mgmt Params");

          /* Call both interfaces based on request from srch team*/
          hdrlmac_set_mgmt_same_channel_params( &smsc_params_attrib.value, 
                                                HDRHAI_ROUTE_UPDATE_PROTOCOL );     
          hdrlmac_set_mgmt_diff_channel_params( &smsc_params_attrib.value, 
                                             HDRHAI_ROUTE_UPDATE_PROTOCOL ); 
        }
        else
        {
          /* Include that we are accepting the set mngmt same channel attribute */
          HDRRUPC_CON_RSP_PACK_LEN( send_msg_ptr, HDRRUPC_CON_RSP_ATTRIB_LEN_BYTES,
                                 attrib_offset );
          HDRRUPC_CON_RSP_PACK_ATTRIBUTE_ID( send_msg_ptr, 
            HDRRUPC_SET_MNGMT_SAME_CHAN_PARAMS_ATTRIB_ID, attrib_offset );
          HDRRUPC_CON_RSP_PACK_VALUE_ID( send_msg_ptr, 
            smsc_params_attrib.value_id,
            attrib_offset );

          attrib_offset += HDRRUPC_CON_RSP_ATTRIB_LEN;

          /* Update our regional structure to reflect the newly accepted values.
           * This is done per attribute because if an attribute was included twice
           * and the second was not accepted, negotiated_values still has the
           * values from the first one, but those were not actually negotiated.
           */
          hdrrupc.negotiated.smsc = smsc_params_attrib.value;
        }
      }
     
      /*---------------------------------------------------------------------
      Pack response for the SetManagementDiffChannel Parameters attribute 
      ---------------------------------------------------------------------*/
      if (smdc_params_attrib.is_accepted)
      {
        /* Include that we are accepting the set mngmt diff channel attribute */
        HDRRUPC_CON_RSP_PACK_LEN( send_msg_ptr, HDRRUPC_CON_RSP_ATTRIB_LEN_BYTES,
                               attrib_offset );
        HDRRUPC_CON_RSP_PACK_ATTRIBUTE_ID( send_msg_ptr, 
          HDRRUPC_SET_MNGMT_DIFF_CHAN_PARAMS_ATTRIB_ID, attrib_offset );
        HDRRUPC_CON_RSP_PACK_VALUE_ID( send_msg_ptr, 
          smdc_params_attrib.value_id,
          attrib_offset );
  
        attrib_offset += HDRRUPC_CON_RSP_ATTRIB_LEN;

        /* Update our regional structure to reflect the newly accepted values.
         * This is done per attribute because if an attribute was included twice
         * and the second was not accepted, negotiated_values still has the
         * values from the first one, but those were not actually negotiated.
         */
        hdrrupc.negotiated.smdc = smdc_params_attrib.value;
      }

      /*---------------------------------------------------------------------
      Pack response for the Attribute Override Allowed Parameter attribute
      ---------------------------------------------------------------------*/
      if (smoa_config_attrib.is_accepted)
      {
        /* Include that we are accepting the set mngmt ovrd allowed attribute */
        HDRRUPC_CON_RSP_PACK_LEN( send_msg_ptr, HDRRUPC_CON_RSP_ATTRIB_LEN_BYTES,
                               attrib_offset );
        HDRRUPC_CON_RSP_PACK_ATTRIBUTE_ID( send_msg_ptr, 
          HDRRUPC_SET_MNGMT_OVERRIDE_ALLOWED_ATTRIB_ID, attrib_offset );
        HDRRUPC_CON_RSP_PACK_ATTRIBUTE_VALUE( send_msg_ptr,
          smoa_config_attrib.value, attrib_offset);

        attrib_offset += HDRRUPC_CON_RSP_ATTRIB_LEN;

        /* Update our regional structure to reflect the newly accepted values.
         * This is done per attribute because if an attribute was included twice
         * and the second was not accepted, negotiated_values still has the
         * values from the first one, but those were not actually negotiated.
         */
        hdrrupc.negotiated.smoa = smoa_config_attrib.value;
      }
                                                                            
      /*---------------------------------------------------------------------
      Pack response for the RouteUpdateRadiusMultiply attribute
      ---------------------------------------------------------------------*/
      if( route_update_radius_mult_attrib.is_accepted )
      {
        /* If this configuration was in response to a GAUP request, begin using
           the value immediately */
        if( msg_type == HDRRUPC_GAUP_REQ )
        {
          hdrrupc.current.rup_rad_mult_attrib = route_update_radius_mult_attrib.value;
        }
        else
        {
          HDRRUPC_CON_RSP_PACK_LEN( send_msg_ptr, 
            HDRRUPC_ROUTE_UPDATE_RADIUS_MULT_ATTRIB_LEN_BYTES, attrib_offset );
          HDRRUPC_CON_RSP_PACK_ATTRIBUTE_ID( send_msg_ptr, 
            HDRRUPC_ROUTE_UPDATE_RADIUS_MULT_ATTRIB_ID, attrib_offset );
          HDRRUPC_CON_RSP_PACK_ATTRIBUTE_VALUE( send_msg_ptr,
            route_update_radius_mult_attrib.value, attrib_offset );
  
          attrib_offset += HDRRUPC_ROUTE_UPDATE_RADIUS_MULT_ATTRIB_LEN;
  
          /* Update our regional structure to reflect the newly accepted values.
           * This is done per attribute because if an attribute was included twice
           * and the second was not accepted, negotiated_values still has the
           * values from the first one, but those were not actually negotiated.
           */
          hdrrupc.negotiated.rup_rad_mult_attrib = route_update_radius_mult_attrib.value;
        }
      }
      
      /*---------------------------------------------------------------------
      Pack response for the RouteUpdateRadiusAdd attribute
      ---------------------------------------------------------------------*/
      if( route_update_radius_add_attrib.is_accepted )
      {
        /* If this configuration was in response to a GAUP request, begin using
           the value immediately */
        if( msg_type == HDRRUPC_GAUP_REQ )
        {
          hdrrupc.current.rup_rad_add_attrib = route_update_radius_add_attrib.value;
        }
        else
        {
          HDRRUPC_CON_RSP_PACK_LEN( send_msg_ptr,
            HDRRUPC_ROUTE_UPDATE_RADIUS_ADD_ATTRIB_LEN_BYTES, attrib_offset );
          HDRRUPC_CON_RSP_PACK_ATTRIBUTE_ID( send_msg_ptr, 
            HDRRUPC_ROUTE_UPDATE_RADIUS_ADD_ATTRIB_ID, attrib_offset );
          /* RouteUpdateRadiusAdd is a 16 bit simple attrib; therefore packing 
             this attrib requires HDRBIT_PACK16() instead of the standard 
             HDRBIT_PACK8() used by HDRRUPC_CON_RSP_PACK_ATTRIBUTE_VALUE() */
          HDRBIT_PACK16( send_msg_ptr, route_update_radius_add_attrib.value,
            attrib_offset + HDRRUPC_ATTRIB_LEN_SIZE + HDRRUPC_ATTRIB_ID_SIZE, 
            HDRRUPC_ROUTE_UPDATE_RADIUS_ADD_ATTRIB_SIZE );
  
          attrib_offset += HDRRUPC_ROUTE_UPDATE_RADIUS_ADD_ATTRIB_LEN;
  
          /* Update our regional structure to reflect the newly accepted values.
           * This is done per attribute because if an attribute was included twice
           * and the second was not accepted, negotiated_values still has the
           * values from the first one, but those were not actually negotiated.
           */
          hdrrupc.negotiated.rup_rad_add_attrib = route_update_radius_add_attrib.value;
        }
      }

      /*---------------------------------------------------------------------
      Pack response for the SupportRouteUpdateEnhancements attribute
      ---------------------------------------------------------------------*/
      if( support_route_update_enh_attrib.is_accepted )
      {
        HDRRUPC_CON_RSP_PACK_LEN( send_msg_ptr,
          HDRRUPC_SUPPORT_ROUTE_UPDATE_ENH_ATTRIB_LEN_BYTES, attrib_offset );
        HDRRUPC_CON_RSP_PACK_ATTRIBUTE_ID( send_msg_ptr, 
          HDRRUPC_SUPPORT_ROUTE_UPDATE_ENH_ATTRIB_ID, attrib_offset );
        HDRRUPC_CON_RSP_PACK_ATTRIBUTE_VALUE( send_msg_ptr,
          support_route_update_enh_attrib.value, attrib_offset );

        attrib_offset += HDRRUPC_SUPPORT_ROUTE_UPDATE_ENH_ATTRIB_LEN;

        /* Update our regional structure to reflect the newly accepted values.
         * This is done per attribute because if an attribute was included twice
         * and the second was not accepted, negotiated_values still has the
         * values from the first one, but those were not actually negotiated.
         */
        hdrrupc.negotiated.supp_rup_enh_attrib = support_route_update_enh_attrib.value;
      }

      if( msg_type == HDRRUPC_GAUP_REQ )
      {
        protocol_instance = HDRHAI_IN_USE;

        /* Update the GAUP'ed values in database */
        hdrscmdb_set_gauped_config(HDRHAI_ROUTE_UPDATE_PROTOCOL,
                                   &hdrrupc.current);
      }
      else
      {
        protocol_instance = HDRHAI_IN_CONFIG;
      }

      /*---------------------------------------------------------------------
      Send config response on RTC.  All config messages are reliable. 
      ---------------------------------------------------------------------*/
      hdrhmp_send_msg( HDRHAI_ROUTE_UPDATE_PROTOCOL,
                       HDRHAI_RTC,
                       TRUE,
                       HDRRUPC_CON_RSP_MSG_PRIORITY,
                       protocol_instance,
                       send_msg_ptr,
                       NULL,
                       NULL );

    } /* end else (msg_type) */

  } /* end if (send_response) */
  else
  {
    /* AT could not accept the AttributeUpdateRequest message so send session
     * close as AT is not allowed to send AttributeUpdateReject */
    if (!gaup_is_accepted)
    {
      gaup_fail_data.reason = HDRHAI_CLOSE_AN_GAUP_FAIL;
      gaup_fail_data.type = HDRHAI_ROUTE_UPDATE_PROTOCOL;
      gaup_fail_data.subtype = HDRSCP_DEFAULT_RUP;
      gaup_fail_data.attribute_id_length = HDRRUPC_ATTRIB_ID_SIZE/HDRRUPC_BITS_PER_BYTE;
      gaup_fail_data.attribute_id = gaup_failed_attrib_id;

      ind_data.scp_fail_ptr = (void *)&gaup_fail_data;

      hdrind_give_ind(HDRIND_RUP_AN_GAUP_FAILED, (void*)&ind_data);
    }
  }

} /* hdrrupc_process_config_req_msg() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRRUPC_PROCESS_ATTRIB_OVERRIDE_MSG

DESCRIPTION
  This function processes the Attribute Override message.

DEPENDENCIES
  None
                      
PARAMETERS
  item_ptr - Pointer to the DSM item chain holding the message

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void hdrrupc_process_attrib_override_msg
( 
  dsm_item_type *item_ptr
)
{
  uint16 rup_subtype = hdrscp_get_config_subtype(HDRHAI_ROUTE_UPDATE_PROTOCOL);

/*- - - - - - - - - - - - -- - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* AttributeOverride message is only supported in rev0/A.
   * In hdrrup_process_msg(), AttributeOverride msg is dropped if MC RUP
   * is negotiated.
   */
#ifdef FEATURE_LTE_TO_HDR_OH
  ASSERT((rup_subtype == HDRSCP_DEFAULT_RUP)||(rup_subtype == HDRSCP_IRAT_RUP));
#else /* FEATURE_LTE_TO_HDR_OH */
  ASSERT(rup_subtype == HDRSCP_DEFAULT_RUP);
#endif /* FEATURE_LTE_TO_HDR_OH */

  /* Ignore the msg if we are not in connected state */
  if ( hdrrups_get_state() != HDRRUPS_CONNECTED )
  {
    HDRTRACE_RXMSG_IGNORED_STATE( HDRHAI_ROUTE_UPDATE_PROTOCOL,
      HDRRUPC_AOR_MSG_ID, hdrrups_get_state() ); //lint !e506
    return;
  }

  /* Since we are overriding config attributes, this message is processed like
     a config request message */
  hdrrupc_process_config_req_msg(item_ptr, HDRRUPC_ATTRIB_OVERRIDE);

} /* hdrrupc_process_attrib_override_msg() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRRUPC_PROCESS_SRCH_PARAMS_ATTRIB

DESCRIPTION
  This function processes the Search Parameters attribute of a Configuration 
  Request message.

DEPENDENCIES
  None

PARAMETERS
  item_ptr - Pointer to the DSM item chain holding the message
  attrib_len - Length, in bits, of the attribute, including the length field
  attrib_offset - Bit offset of the attribute from the beginning of the message
  srch_params_ptr - Pointer to struct to put negotiated srch parameters into
  value_id_ptr - Pointer to the struct to put the accepted Value ID

RETURN VALUE
  TRUE if a new, acceptable value for the attribute was found, FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
LOCAL boolean hdrrupc_process_srch_params_attrib
( 
  dsm_item_type *item_ptr,
  uint16 attrib_len, 
  uint16 attrib_offset,
  hdrsrch_search_params_struct_type *srch_params_ptr,
  uint8  *value_id_ptr
)
{
  /* Bit offset of current attribute value from the beginning of the 
   * current attribute */
  uint16 attrib_val_offset; 
  boolean val_is_acceptable = FALSE;  /* If we accept the attribute */
  int err = 0;        /* Error Value, 0 = E_SUCCESS */

/*- - - - - - - - - - - - -- - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* The first attribute value is after the attribute length and ID fields */
  attrib_val_offset = HDRRUPC_ATTRIB_LEN_SIZE + HDRRUPC_ATTRIB_ID_SIZE;

  /* Loop through all the values given for this attribute. */
  while ((attrib_val_offset + HDRRUPC_SRCH_PARAMS_ATTRIB_SIZE) <=
         attrib_len)
  {
    /* There is at least one more attribute value in this attribute.
     * Unpack this */
    err |= (int) HDRRUPC_CON_REQ_UNPACK_VALUE_ID( item_ptr, 
                   value_id_ptr, 
                   attrib_offset,
                   attrib_val_offset );

    err |= (int) HDRRUPC_CON_REQ_UNPACK_PILOT_INC( item_ptr, 
                   &srch_params_ptr->pilot_inc, 
                   attrib_offset,
                   attrib_val_offset );
    /* ASET, NSET, and RSET window sizes are enum types that must start at
     * zero.  We can safely unpack into the field as if it were a byte
     * since the field size is 4 bits.
     */
    err |= (int) HDRRUPC_CON_REQ_UNPACK_SEARCH_WIN_ACTIVE( item_ptr, 
                   (uint8 *) &srch_params_ptr->aset_win_size, 
                   attrib_offset,
                   attrib_val_offset );

    err |= (int) HDRRUPC_CON_REQ_UNPACK_SEARCH_WIN_NEIGHBOR( item_ptr, 
                   (uint8 *) &srch_params_ptr->nset_win_size,
                   attrib_offset,
                   attrib_val_offset );

    err |= (int) HDRRUPC_CON_REQ_UNPACK_SEARCH_WIN_REMAIN( item_ptr, 
                   (uint8 *) &srch_params_ptr->rset_win_size,
                   attrib_offset,
                   attrib_val_offset );

    if( err == 0 )
    {
      /* If successful in unpacking, decide if we want to use this value. 
       * Currently we just accept the first value and do not unpack the
       * others */
      val_is_acceptable = TRUE;
      break;
    }
    else
    {
      attrib_val_offset += HDRRUPC_SRCH_PARAMS_ATTRIB_SIZE;
    }
  }

  return (val_is_acceptable);

} /* hdrrupc_process_srch_params_attrib() */

/* <EJECT> */ 

/*===========================================================================

FUNCTION HDRRUPC_PROCESS_SET_MNGMT_ATTRIB

DESCRIPTION
  This function processes the Set Management Parameters attribute of a 
  Configuration Request message.

DEPENDENCIES
  None

PARAMETERS
  item_ptr - Pointer to the DSM item chain holding the message
  attrib_len - Length, in bits, of the attribute, including the length field
  attrib_offset - Bit offset of the attribute from the beginning of the message
  sm_params_ptr - Pointer to struct to put negotiated set management params 
                  into
  value_id_ptr - Pointer to the struct to put the accepted Value ID

RETURN VALUE
  TRUE if a new, acceptable value for the attribute was found, FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
LOCAL boolean hdrrupc_process_set_mngmt_attrib
( 
  dsm_item_type *item_ptr,
  uint16 attrib_len, 
  uint16 attrib_offset,
  hdrsrch_chan_mgmt_params_struct_type *sm_params_ptr,
  uint8  *value_id_ptr
)
{
  uint16 attrib_val_offset; /* Bit offset of current attribute value from the
                             * beginning of the current attribute */
  boolean val_is_acceptable = FALSE;  /* If we accept the attribute */
  int err = 0;        /* Error Value, 0 = E_SUCCESS */
  uint8 dynamic_thresholds; /* Dynamic Thresholds */
  uint8 attrib_size; /* Size of a particular attribute set. Variable based on 
                       whether it contains dynamic threshholds or not */

/*- - - - - - - - - - - - -- - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* The first attribute value is after the attribute length and ID fields */
  attrib_val_offset = HDRRUPC_ATTRIB_LEN_SIZE + HDRRUPC_ATTRIB_ID_SIZE;

  /* Loop through all the values given for this attribute. */
  while ((attrib_val_offset +
    HDRRUPC_CON_REQ_SET_MNGMT_PARAMS_DYN_THRESHHOLDS_EXCL_SIZE) <= attrib_len)
  {

    /* There is at least one more attribute value in this attribute.
     * Unpack this */
    err |= (int) HDRRUPC_CON_REQ_UNPACK_VALUE_ID( item_ptr, 
                       value_id_ptr,
                       attrib_offset,
                       attrib_val_offset );

    err |= (int) HDRRUPC_CON_REQ_UNPACK_PILOT_ADD( item_ptr, 
                       (uint8 *) &sm_params_ptr->pilot_add, 
                       attrib_offset,
                       attrib_val_offset );
   
    err |= (int) HDRRUPC_CON_REQ_UNPACK_PILOT_COMPARE( item_ptr, 
                       ((uint8 *) &sm_params_ptr->pilot_compare),
                       attrib_offset,
                       attrib_val_offset );

    err |= (int) HDRRUPC_CON_REQ_UNPACK_PILOT_DROP( item_ptr, 
                      (uint8 *) &sm_params_ptr->pilot_drop, 
                       attrib_offset,
                       attrib_val_offset );

    /* Pilot drop timer is an enum, but is only 4 bits in the message,
     * so we can safely unpack it as a byte
     */
    err |= (int) HDRRUPC_CON_REQ_UNPACK_PILOT_DROP_TIMER( item_ptr, 
                       (uint8 *) &sm_params_ptr->pilot_drop_timer,
                       attrib_offset,
                       attrib_val_offset );

    err |= (int) HDRRUPC_CON_REQ_UNPACK_DYNAMIC_THRESHOLDS( item_ptr, 
                       &dynamic_thresholds,
                       attrib_offset,
                       attrib_val_offset );

    if( dynamic_thresholds == 1 )
    {
      sm_params_ptr->dynamic_thresholds = TRUE;
      err |= (int) HDRRUPC_CON_REQ_UNPACK_SOFT_SLOPE( item_ptr, 
                         &sm_params_ptr->soft_slope,
                         attrib_offset,
                         attrib_val_offset );

      err |= (int) HDRRUPC_CON_REQ_UNPACK_ADD_INTERCEPT( item_ptr, 
                         (uint8 *) &sm_params_ptr->add_intercept,
                         attrib_offset,
                         attrib_val_offset );

      err |= (int) HDRRUPC_CON_REQ_UNPACK_DROP_INTERCEPT( item_ptr, 
                         (uint8 *) &sm_params_ptr->drop_intercept,
                         attrib_offset,
                         attrib_val_offset );
       
      attrib_size = HDRRUPC_CON_REQ_SET_MNGMT_PARAMS_DYN_THRESHHOLDS_INCL_SIZE;
    }
    else /* ConfigReq.DynamicThreshold = 0 */
    {
        sm_params_ptr->dynamic_thresholds = FALSE;
      attrib_size = HDRRUPC_CON_REQ_SET_MNGMT_PARAMS_DYN_THRESHHOLDS_EXCL_SIZE;
    }

    err |= (int) HDRRUPC_CON_REQ_UNPACK_NEIGHBOR_MAX_AGE( item_ptr, 
                         &sm_params_ptr->nset_max_age,
                         attrib_offset,
                         attrib_val_offset,
                         dynamic_thresholds );
    /* If unpacking is successful, accept this value
    */
    if( err == 0 )
    {
      /* If successful in unpacking, decide if we want to use this value. 
       * Currently we just accept the first value and do not unpack the
       * others */
      val_is_acceptable = TRUE;
      break;
    }
    else
    {
      attrib_val_offset += attrib_size; 
    }
  }

  return (val_is_acceptable);

} /* hdrrupc_process_set_mngmt_attrib() */

 /* <EJECT> */ 
/*===========================================================================

FUNCTION HDRRUPC_PROCESS_OVERRIDE_ALLOWED_ATTRIB

DESCRIPTION
  This function processes the Set Management Override Attribute of the 
  Configuration Request Message
    
DEPENDENCIES
  None.

PARAMETERS
  item_ptr - Pointer to the DSM item chain holding the message
  attrib_len - Length, in bits, of the attribute, including the length field
  attrib_offset - Bit offset of the attribute from the beginning of the message
  value_ptr - Pointer to location to write value of accepted attribute

RETURN VALUE
  TRUE if a new, acceptable value for the attribute was found, FALSE otherwise.
  If return value is TRUE, value is returned through value_ptr

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL boolean hdrrupc_process_override_allowed_attrib
(
  dsm_item_type * item_ptr,
  uint16 attrib_len,
  uint16 attrib_offset,
  uint8 *value_ptr
)
{
  /* Bit offset of current attribute value from the beginning of the 
   * current attribute */
  uint16 attrib_val_offset; 
  boolean val_is_acceptable = FALSE;  /* If we accept the attribute */
  uint8 cur_value=0; /* current value being extracted */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* The first attribute value is after the attribute length and ID fields */
  attrib_val_offset = HDRRUPC_ATTRIB_LEN_SIZE +
                      HDRRUPC_ATTRIB_ID_SIZE;

  while ((attrib_val_offset + 
          HDRRUPC_SET_MNGMT_OVERRIDE_ALLOWED_ATTRIB_SIZE) <= attrib_len)
  {
    if (HDRRUPC_CON_REQ_UNPACK_OVERRIDE_ALLOWED(
          item_ptr, 
          &cur_value,
          attrib_offset, 
          attrib_val_offset) != E_SUCCESS)
    {
      ERR("Error unpacking Configuration Request message, ignoring",0,0,0);
      break;
    }

    /* currently, we just accept the first value and don't unpack the others */
    val_is_acceptable = TRUE;
    break;

    /* If we ever want to look for more than one value... 
    attrib_val_offset += HDRRUPC_SET_MNGMT_OVERRIDE_ALLOWED_ATTRIB_SIZE; */
  }

  *value_ptr = cur_value;
  return (val_is_acceptable);
} /* hdrrupc_process_override_allowed_attrib */
  
/* <EJECT> */ 

/*===========================================================================

FUNCTION HDRRUPC_PROCESS_IND

DESCRIPTION
  This function processes indications given to RUPC.

DEPENDENCIES
  None

PARAMETERS
  ind_name     - Name of the indication for LUP to process
  ind_data_ptr - Any data associated with the indication

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrrupc_process_ind
(
  hdrind_ind_name_enum_type    ind_name,
  hdrind_ind_data_union_type   *ind_data_ptr
)
{
  SYS_ARG_NOT_USED(ind_data_ptr); /* remove this line when ind_data_ptr
                                   * is used */

  switch (ind_name)
  {
    case HDRIND_SCM_SESSION_CHANGED:
      hdrrupc_process_session_changed();
      break;

    default:
      ERR_FATAL( "Unknown indication", 0, 0, 0 );
      break;
  }
} /* hdrrupc_process_ind() */
/*lint +esym(715, ind_data_ptr) ind_data_ptr not referenced */

/* <EJECT> */


/*===========================================================================

FUNCTION HDRRUPC_PROCESS_SESSION_CHANGED

DESCRIPTION
  This function processes the session changed indication.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void hdrrupc_process_session_changed( void )
{

  /* Update the local copy of current configuration from SCMDB */
  hdrscmdb_get_inuse_config(HDRHAI_ROUTE_UPDATE_PROTOCOL, &hdrrupc.current);

  /*-------------------------------------------------------------------------
  Set the searcher to the current configuration.
  -------------------------------------------------------------------------*/
  hdrlmac_set_search_params( &hdrrupc.current.srch, 
                            HDRHAI_ROUTE_UPDATE_PROTOCOL ); 

  hdrlmac_set_mgmt_same_channel_params( &hdrrupc.current.smsc, 
                            HDRHAI_ROUTE_UPDATE_PROTOCOL ); 

  hdrlmac_set_mgmt_diff_channel_params( &hdrrupc.current.smdc, 
                            HDRHAI_ROUTE_UPDATE_PROTOCOL ); 
} /* hdrrupc_process_session_changed() */

/* <EJECT> */

/*===========================================================================

FUNCTION COPY_CHAN_MGMT_TO_NV_STRUCT

DESCRIPTION
  This is a helper function to avoid code repetition for assigning fields.

DEPENDENCIES
  None

PARAMETERS
  nv_ptr - Address of structure to have fields assigned to
  srch_ptr - Address of structure to get field values from

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

LOCAL void copy_chan_mgmt_to_nv_struct
( 
  nv_hdr_chan_mgmt_params_type *nv_ptr,
  hdrsrch_chan_mgmt_params_struct_type *srch_ptr
)
{
  nv_ptr->pilot_add          = srch_ptr->pilot_add;
  nv_ptr->pilot_drop         = srch_ptr->pilot_drop;
  nv_ptr->pilot_compare      = srch_ptr->pilot_compare;
  nv_ptr->pilot_drop_timer   = (uint8)(srch_ptr->pilot_drop_timer);
  nv_ptr->dynamic_thresholds = srch_ptr->dynamic_thresholds;
  nv_ptr->soft_slope         = srch_ptr->soft_slope;
  nv_ptr->add_intercept      = srch_ptr->add_intercept;
  nv_ptr->drop_intercept     = srch_ptr->drop_intercept;
  nv_ptr->nset_max_age       = srch_ptr->nset_max_age;
} /* copy_chan_mgmt_to_nv_struct() */

/* <EJECT> */

/*===========================================================================

FUNCTION COPY_CHAN_MGMT_FROM_NV_STRUCT

DESCRIPTION
  This is a helper function to avoid code repitition for assigning fields.

 *srch_ptr = *nv_ptr;

DEPENDENCIES
  None

PARAMETERS
  srch_ptr - Address of structure to get field values from
  nv_ptr - Address of structure to have fields assigned to

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

LOCAL void copy_chan_mgmt_from_nv_struct
( 
  hdrsrch_chan_mgmt_params_struct_type *srch_ptr,
  nv_hdr_chan_mgmt_params_type *nv_ptr
)
{
  srch_ptr->pilot_add          = nv_ptr->pilot_add;
  srch_ptr->pilot_drop         = nv_ptr->pilot_drop;
  srch_ptr->pilot_compare      = nv_ptr->pilot_compare;
  srch_ptr->pilot_drop_timer   = 
      (hdrsrch_drop_timer_enum_type) nv_ptr->pilot_drop_timer;
  srch_ptr->dynamic_thresholds = nv_ptr->dynamic_thresholds;
  srch_ptr->soft_slope         = nv_ptr->soft_slope;
  srch_ptr->add_intercept      = nv_ptr->add_intercept;
  srch_ptr->drop_intercept     = nv_ptr->drop_intercept;
  srch_ptr->nset_max_age       = nv_ptr->nset_max_age;
} /* copy_chan_mgmt_from_nv_struct() */

/* <EJECT> */

/*===========================================================================

FUNCTION HDRRUPC_RESTORE_CURRENT_SRCH_PARAMS

DESCRIPTION
  This function restores config params if they were overwriten while in 
  connected state.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrrupc_restore_current_srch_params ( void )
{
  /*-------------------------------------------------------------------------
  The Override Msg sequence number will need to be re-initialized next time
  we go back to connected state
  -------------------------------------------------------------------------*/
  hdrrupc.att_ovd_seq_num.need_init = TRUE;

  /*-------------------------------------------------------------------------
  Restore the current set management diff channel parameters 
  -------------------------------------------------------------------------*/
  if ( hdrrupc.restore.smdc )
  {
    HDR_MSG_PROT (MSG_LEGACY_MED, "Restore Set Mgmt Diff Channel Params");

    hdrrupc.restore.smdc = FALSE;
    hdrlmac_set_mgmt_diff_channel_params( &hdrrupc.current.smdc, 
                                          HDRHAI_ROUTE_UPDATE_PROTOCOL ); 
  }

  /*-------------------------------------------------------------------------
  Restore the current set management same channel parameters 
  -------------------------------------------------------------------------*/
  if ( hdrrupc.restore.smsc )
  {
    HDR_MSG_PROT (MSG_LEGACY_MED, "Restore Set Mgmt Same Channel Params");
    
    hdrrupc.restore.smsc = FALSE;
    hdrlmac_set_mgmt_same_channel_params( &hdrrupc.current.smsc, 
                                          HDRHAI_ROUTE_UPDATE_PROTOCOL ); 
  }

} /* hdrrupc_restore_current_srch_params() */

/*===========================================================================

FUNCTION HDRRUPC_PROCESS_RUP_RAD_MULT_ATTRIB

DESCRIPTION
  This function processes the RouteUpdateRadiusMultiply simple attribute in the 
  ConfigurationRequest message.

DEPENDENCIES
  None.

PARAMETERS
  item_ptr - The DSM item containing the message.
  attrib_len - The length of the attribute being processed.
  attrib_offset - The offset into the DSM item of the current attribute.
  value_ptr - A reference parameter to be set once the function processes
    an acceptable attribute.
  
RETURN VALUE
  TRUE if the attribute was accepted; FALSE if the attribute was rejected.
  If the return value is TRUE, the accepted attribute value is written back
  through the value_ptr reference parameter.

SIDE EFFECTS
  None.

===========================================================================*/
boolean hdrrupc_process_rup_rad_mult_attrib
(
  dsm_item_type *item_ptr,
  uint16 attrib_len,
  uint16 attrib_offset,
  uint8 *value_ptr
)
{
  /* Bit offset of current attribute value from the beginning of the 
   * current attribute */
  uint16 attrib_val_offset; 
  boolean val_is_acceptable = FALSE;  /* If we accept the attribute */
  uint8 cur_value = 0; /* Current value being extracted */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* The first attribute value is after the attribute length and ID fields */
  attrib_val_offset = HDRRUPC_ATTRIB_LEN_SIZE +
                      HDRRUPC_ATTRIB_ID_SIZE;

  while( (attrib_val_offset + 
          HDRRUPC_ROUTE_UPDATE_RADIUS_MULT_ATTRIB_SIZE) <= attrib_len )
  {
    if( HDRBIT_UNPACK8(
          item_ptr, 
          &cur_value,
          attrib_val_offset + attrib_offset, 
          HDRRUPC_ROUTE_UPDATE_RADIUS_MULT_ATTRIB_SIZE ) != E_SUCCESS )
    {
      ERR( "Error unpacking Configuration Request message, ignoring", 0, 0, 0 );
      break;
    }

    if ((cur_value <= 0x64) && 
        (hdrscp_phone_is_set_to_revA_mode() || 
         (hdrscp_get_config_subtype(HDRHAI_ROUTE_UPDATE_PROTOCOL) == HDRSCP_MC_RUP)
#ifdef FEATURE_LTE_TO_HDR_OH
         || (hdrscp_get_config_subtype(HDRHAI_ROUTE_UPDATE_PROTOCOL) == HDRSCP_IRAT_MC_RUP)
#endif /* FEATURE_LTE_TO_HDR_OH */   
       ))
    {
      val_is_acceptable = TRUE;
      *value_ptr = cur_value;
      break;
    }

    /* Keep looking for more an acceptable value; if this is a GAUP request, 
       incrementing the attrib_val_offset will set exceed the attrib_len
       (since there is only one attribute per GAUP request) and will
       quit out automatically */
    attrib_val_offset += HDRRUPC_ROUTE_UPDATE_RADIUS_MULT_ATTRIB_SIZE;
  }

  return val_is_acceptable;

} /* hdrrupc_process_rup_rad_mult_attrib() */

/*===========================================================================

FUNCTION HDRRUPC_PROCESS_RUP_RAD_ADD_ATTRIB

DESCRIPTION
  This function processes the RouteUpdateRadiusAdd simple attribute in the
  ConfigurationRequest message.

DEPENDENCIES
  None.

PARAMETERS
  item_ptr - The DSM item containing the message.
  attrib_len - The length of the attribute being processed.
  attrib_offset - The offset into the DSM item of the current attribute.
  value_ptr - A reference parameter to be set once the function processes
    an acceptable attribute.
  
RETURN VALUE
  TRUE if the attribute was accepted; FALSE if the attribute was rejected.
  If the return value is TRUE, the accepted attribute value is written back
  through the value_ptr reference parameter.

SIDE EFFECTS
  None.

===========================================================================*/
boolean hdrrupc_process_rup_rad_add_attrib
(
  dsm_item_type *item_ptr,
  uint16 attrib_len,
  uint16 attrib_offset,
  uint16 *value_ptr
)
{
  /* Bit offset of current attribute value from the beginning of the 
   * current attribute */
  uint16 attrib_val_offset; 
  boolean val_is_acceptable = FALSE;  /* If we accept the attribute */
  uint16 cur_value = 0; /* Current value being extracted */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* The first attribute value is after the attribute length and ID fields */
  attrib_val_offset = HDRRUPC_ATTRIB_LEN_SIZE +
                      HDRRUPC_ATTRIB_ID_SIZE;

  while( (attrib_val_offset + 
          HDRRUPC_ROUTE_UPDATE_RADIUS_ADD_ATTRIB_SIZE) <= attrib_len )
  {
    if( HDRBIT_UNPACK16(
          item_ptr, 
          &cur_value,
          attrib_val_offset + attrib_offset, 
          HDRRUPC_ROUTE_UPDATE_RADIUS_ADD_ATTRIB_SIZE ) != E_SUCCESS )
    {
      ERR( "Error unpacking Configuration Request message, ignoring", 0, 0, 0 );
      break;
    }

    if ((cur_value <= 0x0fff) && 
        (hdrscp_phone_is_set_to_revA_mode() ||
         (hdrscp_get_config_subtype(HDRHAI_ROUTE_UPDATE_PROTOCOL) == HDRSCP_MC_RUP)
#ifdef FEATURE_LTE_TO_HDR_OH
         || (hdrscp_get_config_subtype(HDRHAI_ROUTE_UPDATE_PROTOCOL) == HDRSCP_IRAT_MC_RUP)
#endif /* FEATURE_LTE_TO_HDR_OH */   
       ))
    {
      val_is_acceptable = TRUE;
      *value_ptr = cur_value;
      break;
    }
    
    /* Keep looking for an acceptable value; if this is a GAUP request, 
       incrementing the attrib_val_offset will set exceed the attrib_len
       (since there is only one attribute per GAUP request) and will
       quit out automatically */
    attrib_val_offset += HDRRUPC_ROUTE_UPDATE_RADIUS_ADD_ATTRIB_SIZE;
  }

  return val_is_acceptable;

} /* hdrrupc_process_rup_rad_add_attrib() */

/*===========================================================================

FUNCTION HDRRUPC_PROCESS_SUPP_RUP_ENH_ATTRIB

DESCRIPTION
  This function processes the SupportRouteUpdateEnhancements simple attribute
  in the ConfigurationRequest message.

DEPENDENCIES
  None.

PARAMETERS
  item_ptr - The DSM item containing the message.
  attrib_len - The length of the attribute being processed.
  attrib_offset - The offset into the DSM item of the current attribute.
  value_ptr - A reference parameter to be set once the function processes
    an acceptable attribute.
  
RETURN VALUE
  TRUE if the attribute was accepted; FALSE if the attribute was rejected.
  If the return value is TRUE, the accepted attribute value is written back
  through the value_ptr reference parameter.

SIDE EFFECTS
  None.

===========================================================================*/
boolean hdrrupc_process_supp_rup_enh_attrib
(
  dsm_item_type *item_ptr,
  uint16 attrib_len,
  uint16 attrib_offset,
  uint8 *value_ptr
)
{
  /* Bit offset of current attribute value from the beginning of the 
   * current attribute */
  uint16 attrib_val_offset; 
  boolean val_is_acceptable = FALSE;  /* If we accept the attribute */
  uint8 cur_value = 0; /* Current value being extracted */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* The first attribute value is after the attribute length and ID fields */
  attrib_val_offset = HDRRUPC_ATTRIB_LEN_SIZE +
                      HDRRUPC_ATTRIB_ID_SIZE;

  while( (attrib_val_offset + 
          HDRRUPC_SUPPORT_ROUTE_UPDATE_ENH_ATTRIB_SIZE) <= attrib_len )
  {
    if( HDRBIT_UNPACK8(
          item_ptr, 
          &cur_value,
          attrib_val_offset + attrib_offset, 
          HDRRUPC_SUPPORT_ROUTE_UPDATE_ENH_ATTRIB_SIZE ) != E_SUCCESS )
    {
      ERR( "Error unpacking Configuration Request message, ignoring", 0, 0, 0 );
      break;
    }

    /* Only accept values of 0x00, 0x01, 0x02, and 0x03
       ------------------------------------------------
       0x00: No support for GAUPing of RouteUpdateRadiusMultiply or 
         RouteUpdateRadiusAdd and no support for RouteUpdateRequest messages.
       0x01: Support for GAUPing of RouteUpdateRequestMultiply,
         RouteUpdateRadiusAdd and support for RouteUpdateRequest messages
         without Channel Records (ChannelIncluded = 0),
       0x02: Support for GAUPing of RouteUpdateRadiusMultiply,
         RouteUpdateRadiusAdd and support for RouteUpdateRequest messages
         with Channel Record whose System Type is equal to 0x00 (other
         DO channels).
       0x03: Support for GAUPing of RouteUpdateRadiusMultiply,
         RouteUpdateRadiusAdd and support for RouteUpdateRequest messages
         with Channel Record whose System Type is equal to 0x00 (other
         DO channels) and 0x01 (other 1x channels)
     */

    if ( hdrscp_phone_is_set_to_revA_mode() ||
         (hdrscp_get_config_subtype(HDRHAI_ROUTE_UPDATE_PROTOCOL) == HDRSCP_MC_RUP)
#ifdef FEATURE_LTE_TO_HDR_OH
         || (hdrscp_get_config_subtype(HDRHAI_ROUTE_UPDATE_PROTOCOL) == HDRSCP_IRAT_MC_RUP)
#endif /* FEATURE_LTE_TO_HDR_OH */   
       )
    {
      if ( cur_value == 0x00 || cur_value == 0x01 || cur_value == 0x02 
           #ifdef FEATURE_ATOM
           /* We only support value 0x03 in the VoIP builds */
           || cur_value == 0x03
           #endif /* FEATURE_ATOM */
         )
      {
        val_is_acceptable = TRUE;
        *value_ptr = cur_value;
        break;
      }
    }

    /* Keep looking for an acceptable value */
    attrib_val_offset += HDRRUPC_SUPPORT_ROUTE_UPDATE_ENH_ATTRIB_SIZE;
  }

  return val_is_acceptable;

} /* hdrrupc_process_supp_rup_enh_attrib() */

/*===========================================================================

FUNCTION HDRRUP_GET_RUP_RAD_MULT_ATTRIB

DESCRIPTION
  This function is an accessor for the RouteUpdateRadiusMultiply simple 
  attribute.
  
DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  The negotiated value of the RouteUpdateRadiusMultiply attribute.
  Valid return values are:
    0x00 - Indicates distance based registration is disabled
    0x0a - Default multiplier for the Route Update radius (x1)
    0x01 to 0x64 - Multiplier for the Route Update radius in units of 0.1
  

SIDE EFFECTS
  None.

===========================================================================*/
uint8 hdrrupc_get_rup_rad_mult_attrib( void )
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return hdrrupc.current.rup_rad_mult_attrib;

} /* hdrrupc_get_rup_rad_mult_attrib() */

/*===========================================================================

FUNCTION HDRRUP_GET_RUP_RAD_ADD_ATTRIB

DESCRIPTION
  This function is an accessor for the RouteUpdateRadiusAdd simple attribute.
  
DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  The negotiated value of the RouteUpdateRadiusAdd attribute.
  Valid return values are:
    0x00 - Default; addition to the Route Update radius of 0
    0x0001 to 0xffff - Addition to the Route Update radius expressed as a
      12 bit two's complement number sign-extended to a 16 bit number space

SIDE EFFECTS
  None.

===========================================================================*/
int16 hdrrupc_get_rup_rad_add_attrib( void )
{
  uint16 cur_value = hdrrupc.current.rup_rad_add_attrib;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* This attribute is a 12 bit value in a 16 bit field so we need to sign
     extend the value before storing it */
  return (int16) ( (cur_value & 0x0800) ? cur_value | 0xf000 : cur_value );

} /* hdrrupc_get_rup_rad_add_attrib() */

/*===========================================================================

FUNCTION HDRRUPC_GET_SUPP_RUP_ENH_ATTRIB

DESCRIPTION
  This function is an accessor for the SupportRouteUpdateEnhancements simple
  attribute.
  
DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  The negotiated value of the SupportRouteUpdateEnhancements attribute.
  Valid return values are:
    0x00 - Indicates no support for RouteUpdateRequest messages or GAUPing
      of attributes RouteUpdateRadiusMultiply and RouteUpdateRadiusAdd
    0x01 - Indicates support for RouteUpdateRequest messages with no 
      Channel Records and GAUPing of attributes RouteUpdateRadiusMultiply and
      RouteUpdateRadiusAdd
    0x02 - Indicates support for GAUPing of RouteUpdateRadiusMultiply,
      RouteUpdateRadiusAdd and support for RouteUpdateRequest messages
      with Channel Record whose System Type is equal to 0x00 (other
      DO channels)
    0x03 - Indicates support for GAUPing of RouteUpdateRadiusMultiply,
      RouteUpdateRadiusAdd and support for RouteUpdateRequest messages
      with Channel Record whose System Type is equal to 0x00 (other
      DO channels) and 0x01 (other 1x channels)
      

SIDE EFFECTS
  None.

===========================================================================*/
uint8 hdrrupc_get_supp_rup_enh_attrib( void )
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return hdrrupc.current.supp_rup_enh_attrib;

} /* hdrrupc_get_rte_upd_enh_attrib() */

/*===========================================================================

FUNCTION HDRRUPC_GET_MAX_NUM_SUB_ASETS_ATTRIB

DESCRIPTION
  This function is an accessor for the MaxNumberOfSub-ActiveSets simple
  attribute.
  
DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  The negotiated value of the MaxNumberOfSub-ActiveSets attribute.
  Valid return values are:
    0x01 - Indicates The access terminal supports a single sub-Active Set only. 
    0x02 to 0x10 - Indicates the maximum number of sub-Active Sets that the 
                   access terminal can support. 
      

SIDE EFFECTS
  None.

===========================================================================*/
uint8 hdrrupc_get_max_num_sub_asets_attrib( void )
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return hdrrupc.current.max_num_sub_asets_attrib;

} /* hdrrupc_get_max_num_sub_asets_attrib() */

/*===========================================================================

FUNCTION HDRRUPC_POPULATE_BAND_SUBCLASSES

DESCRIPTION
  This function determines the band subclasses supported for a particular
  band class and populates the band class entry appropriately.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
uint32 hdrrupc_populate_band_subclasses
(
  hdrrupc_band_class_rec_type *band_class_entry_ptr,
  uint8 band_class
)
{
  /* The mask representing the current subclasses of the given band */
  uint32 band_sc_mask;

  uint32 attrib_length = 0;

  /* Counter for the number of subclasses */
  int sc_count;

  /* Counter for the ith subclass */
  int sc_index;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Basic sanity check */
  if( band_class > HDRRUPC_MAX_BANDS )
  {
    return attrib_length;
  }

  /* Get the currently supported band subclasses of the given band */
  band_sc_mask = hdrlmac_get_band_subclass( band_class );

  /* If no band subclasses are supported, then we list none of the subclasses */
  if( band_sc_mask == 0 )
  {
    band_class_entry_ptr->band_sub_class_cnt = 0;
  }
  else /* otherwise */
  {
    sc_count = 0;
    sc_index = 0;

    /* Loop through the current subclass mask and determine which of
       the possible band subclasses are supported */
    while( band_sc_mask != 0 )
    {
      if( band_sc_mask & 1 )
      {
        /* Hard limit of 11 supported band subclasses */
        ASSERT( (sc_count + 1) < HDRRUPC_MAX_SUPPORTED_SUB_BANDS );
        if( (sc_count + 1) >= HDRRUPC_MAX_SUPPORTED_SUB_BANDS )
        {
          HDR_MSG_PROT_2( MSG_LEGACY_ERROR, 
            "Too many band subclasses; %d supported out of %d max",
            sc_count + 1, HDRRUPC_MAX_SUPPORTED_SUB_BANDS );

          break;
        }

        band_class_entry_ptr->band_sub_class[sc_count] = (uint8)sc_index;
        ++sc_count;
        attrib_length += HDRSCMRUP_SUCC_BAND_SUB_CLASS_SIZE;
      }
      band_sc_mask >>= 1;
      ++sc_index;
    }

    band_class_entry_ptr->band_sub_class_cnt = sc_count;
  }

  return attrib_length;

} /* hdrrupc_populate_band_subclasses() */

/*===========================================================================

FUNCTION HDRRUPC_BUILD_SUPP_CDMA_CHAN_ATTRIB_WITH_BAND_PREF

DESCRIPTION
  This function queries the RF and SD modules to determine which band
  classes and band subclasses are supported by the AT.

DEPENDENCIES
  None.

PARAMETERS 
  band_pref - Band preference
  succ_attrib_ptr - A pointer to a SupportedCDMAChannels attribute
    structure.

RETURN VALUE
  The final length of the attribute and the succ_attrib_ptr is updated to 
  reflect the supported band classes.

SIDE EFFECTS
  None.

===========================================================================*/
uint8 hdrrupc_build_supp_cdma_chan_attrib_with_band_pref
(
  sd_ss_band_pref_e_type supp_band_pref,
  hdrrupc_supp_cdma_chan_attrib_type *succ_attrib_ptr
)
{
  sd_ss_band_pref_e_type band_pref;
  sd_band_e_type band_class;

  uint8 band_class_count = 0;
  uint32 attrib_length = HDRRUPC_ATTRIB_ID_SIZE + 
                         HDRRUPC_VALUE_ID_SIZE + 
                         HDRSCMRUP_SUCC_BAND_CLASS_CNT_SIZE;

  /* Convenience pointer */
  hdrrupc_band_class_rec_type *band_class_list;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Clear the attrib pointer */
  memset( succ_attrib_ptr, 0, sizeof( hdrrupc_supp_cdma_chan_attrib_type ) );

  /* Set the convenience pointer */
  band_class_list = succ_attrib_ptr->band_class_list;

  for( band_class = SD_BAND_BC0; band_class < HDRRUPC_MAX_BANDS; band_class++ )
  {
#ifdef FEATURE_CMI
    band_pref = sd_map_band_to_band_pref(band_class);
#else
    band_pref = sdss_map_band_to_band_pref(band_class);
#endif
    if (band_pref > 0)
    {
      if( band_pref & supp_band_pref )
      {
        band_class_count++;
        if( band_class_count > HDRRUPC_MAX_SUPPORTED_BANDS )
        {
          HDR_MSG_PROT_2( MSG_LEGACY_ERROR, 
            "Too many band classes; %d supported of out %d max",
            band_class_count, HDRRUPC_MAX_SUPPORTED_BANDS );
                     
          return HDRRUPC_ROUND_NEAREST_BYTE( attrib_length );
        }
        else
        {
          band_class_list[band_class_count-1].band_class = (uint8) band_class;
          attrib_length += HDRSCMRUP_SUCC_BAND_CLASS_SIZE;

          attrib_length += HDRSCMRUP_SUCC_BAND_SUB_CLASS_CNT_SIZE;

          if (hdrscp_get_config_subtype(HDRHAI_ROUTE_UPDATE_PROTOCOL) == HDRSCP_MC_RUP
#ifdef FEATURE_LTE_TO_HDR_OH
              || hdrscp_get_config_subtype(HDRHAI_ROUTE_UPDATE_PROTOCOL) == HDRSCP_IRAT_MC_RUP
#endif /* FEATURE_LTE_TO_HDR_OH */              
             )
          {
            attrib_length += HDRSCMRUP_SUCC_INTER_FLEX_DUPLEX_TAG_SIZE;
            attrib_length += HDRSCMRUP_SUCC_INTRA_FLEX_DUPLEX_FLAG_SIZE;
          }

          attrib_length += 
            hdrrupc_populate_band_subclasses( 
              &band_class_list[band_class_count-1], (uint8)band_class );
        }        
      }
    }
  }

  succ_attrib_ptr->band_class_cnt = band_class_count;

  return HDRRUPC_ROUND_NEAREST_BYTE( attrib_length );

} /* hdrrupc_build_supp_cdma_chan_attrib_with_band_pref() */

/*===========================================================================

FUNCTION HDRRUPC_BUILD_SUPP_CDMA_CHAN_ATTRIB

DESCRIPTION
  This function queries the RF and SD modules to determine which band
  classes and band subclasses are supported by the AT.

DEPENDENCIES
  None.

PARAMETERS
  succ_attrib_ptr - A pointer to a SupportedCDMAChannels attribute
    structure.

RETURN VALUE
  The final length of the attribute and the succ_attrib_ptr is updated to 
  reflect the supported band classes.

SIDE EFFECTS
  None.

===========================================================================*/
uint8 hdrrupc_build_supp_cdma_chan_attrib
(
  hdrrupc_supp_cdma_chan_attrib_type *succ_attrib_ptr
)
{
  return hdrrupc_build_supp_cdma_chan_attrib_with_band_pref(
           sd_misc_get_supp_band_pref(), succ_attrib_ptr);
} /* hdrrupc_build_supp_cdma_chan_attrib() */

/*===========================================================================

FUNCTION HDRRUPC_BAND_CLASS_ENTRIES_ARE_EQUAL

DESCRIPTION
  Determines if two band class entries of the SupportedCDMAChannels 
  attribute are equivalent.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static boolean hdrrupc_band_class_entries_are_equal
(
  hdrrupc_band_class_rec_type bc_entry1,
  hdrrupc_band_class_rec_type bc_entry2
)
        {
  int sc_entry; /* Band SubClass entry */
  boolean entries_are_equal = TRUE;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( (bc_entry1.band_class == bc_entry2.band_class) &&
      (bc_entry1.band_sub_class_cnt == bc_entry2.band_sub_class_cnt) )
  {
    for( sc_entry = 0; sc_entry < bc_entry1.band_sub_class_cnt; sc_entry++ )
    {
      if( bc_entry1.band_sub_class[sc_entry] != bc_entry2.band_sub_class[sc_entry] )
      {
        entries_are_equal = FALSE;
        break;
      }
    }
  }
  else
  {
    entries_are_equal = FALSE;
  }

  return entries_are_equal;
} /* hdrrupc_band_class_entries_are_equal() */

/*===========================================================================

FUNCTION HDRRUPC_BAND_PREFS_ARE_EQUAL

DESCRIPTION
  Determines if two band preferences are equivalent.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static boolean hdrrupc_band_prefs_are_equal
(
  hdrrupc_supp_cdma_chan_attrib_type *succ1_ptr,
  hdrrupc_supp_cdma_chan_attrib_type *succ2_ptr
)
{
  boolean attribs_are_equal = TRUE;
  int bc_entry; /* Band Class entry */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( succ1_ptr->band_class_cnt == 0 || succ2_ptr->band_class_cnt == 0 )
  {
    /* A band class count of 0 implies that either the AN has rejected the
       negotiated attribute or there was an error building the attribute. In
       either scenario, the safest route is the assume they are equal and do
       nothing */
    attribs_are_equal = TRUE;
  }
  else if( succ1_ptr->band_class_cnt != succ2_ptr->band_class_cnt )
  {
    attribs_are_equal = FALSE;
  }
  else
  {
    for( bc_entry = 0; 
         bc_entry < succ1_ptr->band_class_cnt; /* or succ2's band_class_cnt */
         bc_entry ++ )
    {
      if( hdrrupc_band_class_entries_are_equal( 
            succ1_ptr->band_class_list[bc_entry], 
            succ2_ptr->band_class_list[bc_entry] ) == FALSE )
      {
        attribs_are_equal = FALSE;
        break;
      }
    }
  }

  return attribs_are_equal;
} /* hdrrupc_band_prefs_are_equal()*/

/*===========================================================================

FUNCTION HRRUPC_BAND_PREF_IS_CHANGED

DESCRIPTION
  Determins if the band preference was changed.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
boolean hdrrupc_band_pref_is_changed( void )
{
  hdrrupc_supp_cdma_chan_attrib_type succ;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  (void) hdrrupc_build_supp_cdma_chan_attrib( &succ );
  return ( !hdrrupc_band_prefs_are_equal( &succ, &(hdrrupc.current.succ) ) );
}

/* <EJECT> */
/*===========================================================================

FUNCTION HDRRUPC_SUCC_ATTRIB_CONFIG_IS_NEEDED

DESCRIPTION
  This function checks whether SupportedCDMAChannels  attribute
  needs to be re-negotiated.
  
DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  True - config is needed

SIDE EFFECTS
  None

===========================================================================*/
boolean hdrrupc_succ_attrib_config_is_needed(void)
{
  boolean config_is_needed = FALSE; /* whether config is needed */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (hdrrupc.negotiated.succ.band_class_cnt == HDRRUPC_DEFAULT_BAND_CLASS_COUNT)
  {
    /* AT initiated configuration is needed only if the AT does not have its 
       desired values already. By default band_class_cnt is zero, so propose
       SupportedCDMAChannels whenever the current values are default instead
       of the AT's desired values */
    config_is_needed = TRUE;
  }

  return config_is_needed;
} /* hdrrupc_succ_attrib_config_is_needed */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRRUPC_MAX_NUM_FL_SUPPORTED_ATTRIB_CONFIG_IS_NEEDED

DESCRIPTION
  This function checks whether MaxNumberofFLSupported attribute
  needs to be re-negotiated.

DEPENDENCIES
  None

PARAMETERS
  none

RETURN VALUE
  Ture - config is needed

SIDE EFFECTS
  None

===========================================================================*/
boolean hdrrupc_max_num_fl_supported_attrib_config_is_needed(void)
  {
  boolean config_is_needed = FALSE; /* whether config is needed */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (((hdrscp_get_config_subtype(HDRHAI_ROUTE_UPDATE_PROTOCOL) == HDRSCP_MC_RUP)
#ifdef FEATURE_LTE_TO_HDR_OH
        || (hdrscp_get_config_subtype(HDRHAI_ROUTE_UPDATE_PROTOCOL) == HDRSCP_IRAT_MC_RUP)
#endif /* FEATURE_LTE_TO_HDR_OH */   
      ) && (hdrrupc.negotiated.max_num_fl_supported_attrib != 
         HDRRUPC_DESIRED_MAX_NUM_FL_SUPPORTED))
  {
    config_is_needed = TRUE;
  }

  return config_is_needed;
} /* hdrrupc_max_num_fl_supported_attrib_config_is_needed */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRRUPC_MAX_NUM_RL_SUPPORTED_ATTRIB_CONFIG_IS_NEEDED

DESCRIPTION
  This function checks whether MaxNumberofRLSupported  attribute
  needs to be re-negotiated.

DEPENDENCIES
  None

PARAMETERS
  none

RETURN VALUE
  Ture - config is needed

SIDE EFFECTS
  None

===========================================================================*/
boolean hdrrupc_max_num_rl_supported_attrib_config_is_needed(void)
{
  boolean config_is_needed = FALSE; /* whether config is needed */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (((hdrscp_get_config_subtype(HDRHAI_ROUTE_UPDATE_PROTOCOL) == HDRSCP_MC_RUP) 
#ifdef FEATURE_LTE_TO_HDR_OH
        || (hdrscp_get_config_subtype(HDRHAI_ROUTE_UPDATE_PROTOCOL) == HDRSCP_IRAT_MC_RUP)
#endif /* FEATURE_LTE_TO_HDR_OH */   
      )&& (hdrrupc.negotiated.max_num_rl_supported_attrib != 
         HDRRUPC_DESIRED_MAX_NUM_RL_SUPPORTED))
  {
    config_is_needed = TRUE;
  }

  return config_is_needed;
} /* hdrrupc_succ_attrib_config_is_needed */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRRUPC_MAX_FL_BDWIDTH_NO_JAMMER_ATTRIB_CONFIG_IS_NEEDED

DESCRIPTION
  This function checks whether MaxForwardLnikBandwidthNoJammer attribute
  needs to be re-negotiated.

DEPENDENCIES
  None

PARAMETERS
  none

RETURN VALUE
  Ture - config is needed

SIDE EFFECTS
  None

===========================================================================*/
boolean hdrrupc_max_fl_bdwidth_no_jammer_attrib_config_is_needed(void)
{
  boolean config_is_needed = FALSE; /* whether config is needed */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (((hdrscp_get_config_subtype(HDRHAI_ROUTE_UPDATE_PROTOCOL) == HDRSCP_MC_RUP) 
#ifdef FEATURE_LTE_TO_HDR_OH
        || (hdrscp_get_config_subtype(HDRHAI_ROUTE_UPDATE_PROTOCOL) == HDRSCP_IRAT_MC_RUP)
#endif /* FEATURE_LTE_TO_HDR_OH */   
      )&& (hdrrupc.negotiated.max_fl_bdwidth_no_jammer_attrib !=
         HDRRUPC_DESIRED_MAX_FL_BANDWIDTH_NO_JAMMER ))
  {
    config_is_needed = TRUE;
  }

  return config_is_needed;
} /* hdrrupc_max_fl_bdwidth_no_jammer_attrib_config_is_needed */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRRUPC_MAX_FL_BDWIDTH_W_JAMMER_ATTRIB_CONFIG_IS_NEEDED

DESCRIPTION
  This function checks whether MaxForwardLnikBandwidthWithJammer attribute
  needs to be re-negotiated.

DEPENDENCIES
  None

PARAMETERS
  none

RETURN VALUE
  Ture - config is needed

SIDE EFFECTS
  None

===========================================================================*/
boolean hdrrupc_max_fl_bdwidth_w_jammer_attrib_config_is_needed(void)
{
  boolean config_is_needed = FALSE; /* whether config is needed */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (((hdrscp_get_config_subtype(HDRHAI_ROUTE_UPDATE_PROTOCOL) == HDRSCP_MC_RUP) 
#ifdef FEATURE_LTE_TO_HDR_OH
        || (hdrscp_get_config_subtype(HDRHAI_ROUTE_UPDATE_PROTOCOL) == HDRSCP_IRAT_MC_RUP)
#endif /* FEATURE_LTE_TO_HDR_OH */
      )&& (hdrrupc.negotiated.max_fl_bdwidth_w_jammer_attrib != 
         HDRRUPC_DESIRED_MAX_FL_BANDWIDTH_W_JAMMER))
  {
    config_is_needed = TRUE;
  }

  return config_is_needed;
} /* hdrrupc_max_fl_bdwidth_w_jammer_attrib_config_is_needed */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRRUPC_MAX_RL_BDWIDTH_ATTRIB_CONFIG_IS_NEEDED

DESCRIPTION
  This function checks whether MaxReverseLinkBandwidth attribute
  needs to be re-negotiated.

DEPENDENCIES
  None

PARAMETERS
  none

RETURN VALUE
  Ture - config is needed

SIDE EFFECTS
  None

===========================================================================*/
boolean hdrrupc_max_rl_bdwidth_attrib_config_is_needed(void)
{
  boolean config_is_needed = FALSE; /* whether config is needed */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (((hdrscp_get_config_subtype(HDRHAI_ROUTE_UPDATE_PROTOCOL) == HDRSCP_MC_RUP) 
#ifdef FEATURE_LTE_TO_HDR_OH
        || (hdrscp_get_config_subtype(HDRHAI_ROUTE_UPDATE_PROTOCOL) == HDRSCP_IRAT_MC_RUP)
#endif /* FEATURE_LTE_TO_HDR_OH */
      )&& (hdrrupc.negotiated.max_rl_bdwidth_attrib != HDRRUPC_DESIRED_MAX_RL_BANDWIDTH))
  {
    config_is_needed = TRUE;
  }

  return config_is_needed;
} /* hdrrupc_max_rl_bdwidth_attrib_config_is_needed */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRRUPC_MAX_NUM_SUB_ASETS_ATTRIB_CONFIG_IS_NEEDED

DESCRIPTION
  This function checks whether MaxNumberOfSub-ActiveSets attribute
  needs to be re-negotiated.

DEPENDENCIES
  None

PARAMETERS
  none

RETURN VALUE
  Ture - config is needed

SIDE EFFECTS
  None

===========================================================================*/
boolean hdrrupc_max_num_sub_asets_attrib_config_is_needed(void)
{
  boolean config_is_needed = FALSE; /* whether config is needed */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (((hdrscp_get_config_subtype(HDRHAI_ROUTE_UPDATE_PROTOCOL) == HDRSCP_MC_RUP) 
#ifdef FEATURE_LTE_TO_HDR_OH
        || (hdrscp_get_config_subtype(HDRHAI_ROUTE_UPDATE_PROTOCOL) == HDRSCP_IRAT_MC_RUP)
#endif /* FEATURE_LTE_TO_HDR_OH */
      )&& (hdrrupc.negotiated.max_num_sub_asets_attrib != 
         HDRRUPC_DESIRED_MAX_NUM_SUB_ASETS))
  {
    config_is_needed = TRUE;
  }

  return config_is_needed;
} /* hdrrupc_succ_attrib_config_is_needed */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRRUPC_FWD_FEEDBACK_MUX_SUPPORTED_ATTRIB_CONFIG_IS_NEEDED

DESCRIPTION
  This function checks whether ForwardFeedbackMultilexingSupported attribute
  needs to be re-negotiated.

DEPENDENCIES
  None

PARAMETERS
  none

RETURN VALUE
  Ture - config is needed

SIDE EFFECTS
  None

===========================================================================*/
boolean hdrrupc_fwd_feedback_mux_supported_attrib_config_is_needed(void)
{
  boolean config_is_needed = FALSE; /* whether config is needed */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (((hdrscp_get_config_subtype(HDRHAI_ROUTE_UPDATE_PROTOCOL) == HDRSCP_MC_RUP) 
#ifdef FEATURE_LTE_TO_HDR_OH
        || (hdrscp_get_config_subtype(HDRHAI_ROUTE_UPDATE_PROTOCOL) == HDRSCP_IRAT_MC_RUP)
#endif /* FEATURE_LTE_TO_HDR_OH */
      )&& (hdrrupc.negotiated.fwd_fb_mux_supp_attrib != 
         HDRRUPC_DESIRED_FWD_FEEDBACK_MUX_SUPPORTED))
  {
    config_is_needed = TRUE;
  }

  return config_is_needed;
} /* hdrrupc_fwd_feedback_mux_supported_attrib_config_is_needed */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRRUPC_CONFIG_IS_NEEDED

DESCRIPTION
  This function determines whether RUP requires any configuration.
  
DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  TRUE if RUP needs configuring; FALSE if RUP does not need any configuration.

SIDE EFFECTS
  None.

===========================================================================*/
boolean hdrrupc_config_is_needed( void )
{
  boolean config_is_needed;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  config_is_needed = (hdrrupc_succ_attrib_config_is_needed() ||
                      hdrrupc_max_num_fl_supported_attrib_config_is_needed() ||
                      hdrrupc_max_num_rl_supported_attrib_config_is_needed() ||
                      hdrrupc_max_fl_bdwidth_no_jammer_attrib_config_is_needed() ||
                      hdrrupc_max_fl_bdwidth_w_jammer_attrib_config_is_needed() ||
                      hdrrupc_max_rl_bdwidth_attrib_config_is_needed() ||
                      hdrrupc_max_num_sub_asets_attrib_config_is_needed() ||
                      hdrrupc_fwd_feedback_mux_supported_attrib_config_is_needed());

  return config_is_needed;
  
} /* hdrrupc_config_is_needed() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRRUPC_SET_CONFIG_TO_DEFAULT

DESCRIPTION
  This function initializes Route Update Protocol's configuration parameters
  to default. 

DEPENDENCIES
  None

PARAMETERS
  current_config - Pointer to Route Update Protocol's current configuration 
                   structure in SCMDB
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrrupc_set_config_to_default
( 
  hdrrupc_config_params_type *current_config 
)
{

  hdrrupc_set_srch_param_default( &current_config->srch );
  hdrrupc_set_smsc_param_default( &current_config->smsc );
  hdrrupc_set_smdc_param_default( &current_config->smdc );
  current_config->smoa = HDRRUPC_DEFAULT_OVERRIDE_ALLOWED_ATTRIB_VAL;

  current_config->rup_rad_mult_attrib = 
    HDRRUPC_DEFAULT_ROUTE_UPDATE_RADIUS_MULT_ATTRIB_VAL;

  current_config->rup_rad_add_attrib  = 
    HDRRUPC_DEFAULT_ROUTE_UPDATE_RADIUS_ADD_ATTRIB_VAL;

  current_config->supp_rup_enh_attrib = 
    HDRRUPC_DEFAULT_SUPPORT_ROUTE_UPDATE_ENH_ATTRIB_VAL;

  memset( &current_config->succ, 0, 
          sizeof( hdrrupc_supp_cdma_chan_attrib_type ) );

  current_config->max_num_fl_supported_attrib =
    HDRRUPC_DEFAULT_MAX_NUM_FL_SUPPORTED;

  current_config->max_num_rl_supported_attrib =
    HDRRUPC_DEFAULT_MAX_NUM_RL_SUPPORTED;

  current_config->max_fl_bdwidth_no_jammer_attrib =
    HDRRUPC_DEFAULT_MAX_FL_BANDWIDTH_NO_JAMMER;

  current_config->max_fl_bdwidth_w_jammer_attrib =
    HDRRUPC_DEFAULT_MAX_FL_BANDWIDTH_W_JAMMER;

  current_config->max_rl_bdwidth_attrib = 
    HDRRUPC_DEFAULT_MAX_RL_BANDWIDTH;

  current_config->max_num_sub_asets_attrib =
    HDRRUPC_DEFAULT_MAX_NUM_SUB_ASETS;

  current_config->fwd_fb_mux_supp_attrib =
    HDRRUPC_DEFAULT_FWD_FEEDBACK_MUX_SUPPORTED;

} /* hdrrupc_set_config_to_default() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRRUPC_CONFIG_IS_VALID

DESCRIPTION
  This function checks if negotiated values are valid.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
boolean hdrrupc_config_is_valid( void )
{
  boolean config_is_valid = TRUE;

  if (hdrscp_get_config_subtype(HDRHAI_ROUTE_UPDATE_PROTOCOL) == HDRSCP_MC_RUP 
#ifdef FEATURE_LTE_TO_HDR_OH
      || hdrscp_get_config_subtype(HDRHAI_ROUTE_UPDATE_PROTOCOL) == HDRSCP_IRAT_MC_RUP
#endif /* FEATURE_LTE_TO_HDR_OH */ 
     )
  {
    /* Following three RUP1 attributes need to have same value */
    if ((hdrrupc.negotiated.max_fl_bdwidth_no_jammer_attrib != 
        hdrrupc.negotiated.max_fl_bdwidth_w_jammer_attrib) ||
        (hdrrupc.negotiated.max_fl_bdwidth_w_jammer_attrib !=
         hdrrupc.negotiated.max_rl_bdwidth_attrib))
    {
      HDR_MSG_PROT(MSG_LEGACY_ERROR,
        "Unsupported Config: MCRUP: MAX FL_NO_JAMM/W_JAMM,RL BD should have same values");
      config_is_valid = FALSE;
    }
  }

  return config_is_valid;

} /* hdrrupc_config_is_valid() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRRUPC_START_CONFIG

DESCRIPTION
  This function begins the AT initialized configuration process.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
boolean hdrrupc_start_config()
{

  return hdrscmrup_start_config();

} /* hdrrupc_start_config() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRRUPC_GET_INCONFIG_HANDLE

DESCRIPTION
  This function returns a pointer to inconfig instance of RUP

DEPENDENCIES
  None

PARAMETERS
  None
                     
RETURN VALUE
  Pointer to inconfig instance of RUP

SIDE EFFECTS
  None

===========================================================================*/
void* hdrrupc_get_inconfig_handle(void)
{
  return (void*) &hdrrupc.negotiated;
} /* hdrrupc_get_inconfig_handle() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRRUPC_GET_MAX_FL_BDWIDTH_NO_JAMMER_ATTRIB

DESCRIPTION
  This function is an accessor for the MaxForwardLinkBandwidthNoJammer simple
  attribute.
  
DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  The negotiated value of the MaxForwardLinkBandwidthNoJammer attribute.

SIDE EFFECTS
  None.

===========================================================================*/
uint16 hdrrupc_get_max_fl_bdwidth_no_jammer_attrib( void )
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return hdrrupc.current.max_fl_bdwidth_no_jammer_attrib;

} /* hdrrupc_get_max_fl_bdwidth_no_jammer_attrib() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRRUPC_GET_MAX_FL_BDWIDTH_W_JAMMER_ATTRIB

DESCRIPTION
  This function is an accessor for the MaxForwardLinkBandwidthWithJammer simple
  attribute.
  
DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  The negotiated value of the MaxForwardLinkBandwidthNoJammer attribute.

SIDE EFFECTS
  None.

===========================================================================*/
uint16 hdrrupc_get_max_fl_bdwidth_w_jammer_attrib( void )
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return hdrrupc.current.max_fl_bdwidth_w_jammer_attrib;

} /* hdrrupc_get_max_fl_bdwidth_w_jammer_attrib() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRRUPC_GET_MAX_RL_BDWIDTH_ATTRIB

DESCRIPTION
  This function is an accessor for the MaxForwardLinkBandwidthNoJammer simple
  attribute.
  
DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  The negotiated value of the MaxForwardLinkBandwidthNoJammer attribute.

SIDE EFFECTS
  None.

===========================================================================*/
uint16 hdrrupc_get_max_rl_bdwidth_attrib( void )
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return hdrrupc.current.max_rl_bdwidth_attrib;

} /* hdrrupc_get_max_rl_bdwidth_attrib() */


/* <EJECT> */
/*===========================================================================

FUNCTION HDRRUPC_GET_MAX_NUM_FL_SUPPORTED_ATTRIB

DESCRIPTION
  This function is an accessor for the MaxNumberofFLSupported simple
  attribute.
  
DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  The negotiated value of the MaxNumberofFLSupported attribute.

SIDE EFFECTS
  None.

===========================================================================*/
uint8 hdrrupc_get_max_num_fl_supported_attrib( void )
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return hdrrupc.current.max_num_fl_supported_attrib;

} /* hdrrupc_get_max_num_fl_supported_attrib() */


/* <EJECT> */
/*===========================================================================

FUNCTION HDRRUPC_GET_MAX_NUM_RL_SUPPORTED_ATTRIB

DESCRIPTION
  This function is an accessor for the MaxNumberofRLSupported simple
  attribute.
  
DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  The negotiated value of the MaxNumberofRLSupported attribute.

SIDE EFFECTS
  None.

===========================================================================*/
uint8 hdrrupc_get_max_num_rl_supported_attrib( void )
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return hdrrupc.current.max_num_rl_supported_attrib;

} /* hdrrupc_get_max_num_rl_supported_attrib() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRRUPC_GET_FWD_FB_MUX_SUPP_ATTRIB

DESCRIPTION
  This function is an accessor for the ForwardFeedbackMultiplexingSupported 
  simple attribute.
  
DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  The negotiated value of the MaxForwardLinkBandwidthNoJammer attribute.

SIDE EFFECTS
  None.

===========================================================================*/
uint8 hdrrupc_get_fwd_fb_mux_supp_attrib( void )
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return hdrrupc.current.fwd_fb_mux_supp_attrib;

} /* hdrrupc_get_fwd_fb_mux_supp_attrib() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRRUPC_GET_PILOT_INC

DESCRIPTION
  This function is an accessor for the PilotIncrement field in 
  SearchParamters attribute.
  
DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  The negotiated value of the the PilotIncrement field in SearchParamters
  attribute.
      

SIDE EFFECTS
  None.

===========================================================================*/
uint8 hdrrupc_get_pilot_inc( void )
{
  return (uint8) hdrrupc.current.srch.pilot_inc;
} /* hdrrupc_get_pilot_inc() */

#ifdef FEATURE_LTE_TO_HDR
/*===========================================================================

FUNCTION HDRRUPC_GET_PACKED_CDMA_BANDCLASS_INFO

DESCRIPTION
  This function is called to pack the A21 mobile subscription information 
  requested by LTE.
  
DEPENDENCIES
  None.

PARAMETERS
  band_pref - Band preference
  capabilities_container_ptr - Location where to store the packed values.

RETURN VALUE
  Size of the packed data.

SIDE EFFECTS
  None.

===========================================================================*/
uint32 hdrrupc_get_packed_cdma_bandclass_info
( 
  sd_ss_band_pref_e_type band_pref,
  uint8 *capabilities_container_ptr 
)
{
  hdrrupc_supp_cdma_chan_attrib_type cdma_bc_info;
    /* HRPD capabilities - Supported band class */
  dsm_item_type *bc_info_ptr;               
    /* DSM item pointer to store packed message */
  uint32 offset = 0;
    /* Offset in the dsm item at which to pack */
  uint16 i, j, k;
    /* Loop counter */
  uint32 record_len = 0;
    /* Length of band class/band sub class record*/
  uint32 a21_element_length = 0;
  uint32 capabilities_container_length = 0;
  uint32 byte_mask_in_uint32 = 0;
  uint32 sub_bc_bit_mask = 0;
  byte byte_in_uint32 = 0;
  uint8 rev_byte[HDRRUPC_MAX_SUB_BANDS_OCTETS];
  uint8 num_sub_bands_octets = 0;
  uint8 index = 0;
  uint8 max_sub_bc_len = 0;
  uint8 largest_sub_bc = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Currently LTE module that sends this request is not able to pass a DSM 
     item. However for HDR it is preferred to receive a DSM item so that 
     packing can be done with existing routines. For this reason HDRMC 
     temporarily allocates a DSM item packs the data and in the end pull the
     data to the container variable. the DSM item is eventually freed. Later
     date when LTE can pass a DSM item the temporary item can be replaced 
     with the one from DSM item without changing any of the packing. 
  */

  /* Get new DSM buffer */
  bc_info_ptr = hdrutil_new_msg_buffer();

  /* Get supported CDMA channel/band information */
  (void) hdrrupc_build_supp_cdma_chan_attrib_with_band_pref( band_pref, &cdma_bc_info );

  /* Compute length of band class/band sub class record */
  for ( i = 0; i < cdma_bc_info.band_class_cnt; i++ )
  {
    /* This includes 1 byte for BCn and 1 byte for All band subbandclass 
       included + reserved + BC Subclass length */
    record_len += 2;

    /* If there are Sub BC for the given BC then find number octets needed 
       to pack the Sub BC info */
    if (cdma_bc_info.band_class_list[i].band_sub_class_cnt != 0)
    {
      largest_sub_bc = cdma_bc_info.band_class_list[i].band_sub_class[0];

      for ( j = 1; j < cdma_bc_info.band_class_list[i].band_sub_class_cnt; j++ )
      {
        if (cdma_bc_info.band_class_list[i].band_sub_class[j] > largest_sub_bc)
        {
          largest_sub_bc = cdma_bc_info.band_class_list[i].band_sub_class[j];
        }
      }

      /* Number of octets needed to fit the Sub BC for given BC */
      max_sub_bc_len = (largest_sub_bc / 8) + 1;

      /* Increase the record length by number of octets needed to fit 
         the Sub BC */
      record_len += max_sub_bc_len;
    }
  }

  /* Increase record_len by 1 to fit the current sub BC info */
  record_len += 1;

  /* A21 element length is record size + "record length" field -1 + 
     "record identifier" field  -1 */
  a21_element_length = record_len + 2;

  capabilities_container_length = a21_element_length + 2;

  /* Pack A21 element identiier */
  HDRBIT_PACK8( bc_info_ptr, HDRRUPC_A21_MOBILE_SUBSC_ELEMENT_ID, 
                offset, HDRRUPC_A21_MOBILE_SUBSC_ELEMENT_ID_LENGTH_SIZE );
  offset += HDRRUPC_A21_MOBILE_SUBSC_ELEMENT_ID_LENGTH_SIZE;

  /* Pack length (record length + length of record-1 + record identifier-1) */
  HDRBIT_PACK8( bc_info_ptr, a21_element_length,
                offset, HDRRUPC_A21_MOBILE_SUBSC_LENGTH_SIZE );
  offset += HDRRUPC_A21_MOBILE_SUBSC_LENGTH_SIZE;

  /* Pack record identifier */
  HDRBIT_PACK8( bc_info_ptr, HDRRUPC_A21_MOBILE_SUBSC_RECORD_ID, 
                offset, HDRRUPC_A21_MOBILE_SUBSC_RECORD_ID_SIZE );
  offset += HDRRUPC_A21_MOBILE_SUBSC_RECORD_ID_SIZE; 

  /* Pack record length */
  HDRBIT_PACK8( bc_info_ptr, record_len, 
                offset, HDRRUPC_A21_MOBILE_SUBSC_RECORD_LENGTH_SIZE );
  offset += HDRRUPC_A21_MOBILE_SUBSC_RECORD_LENGTH_SIZE; 

  /* All band classes included */
  HDRBIT_PACK8( bc_info_ptr, 1, offset, 
                HDRRUPC_A21_MOBILE_SUBSC_ALL_BAND_CLASS_INCL_SIZE );
  offset += HDRRUPC_A21_MOBILE_SUBSC_ALL_BAND_CLASS_INCL_SIZE; 

  /* Current Band Subclass */
  HDRBIT_PACK8( bc_info_ptr, 0, 
                offset, HDRRUPC_A21_MOBILE_SUBSC_CURR_BAND_SUBCLASS_SIZE );
  offset += HDRRUPC_A21_MOBILE_SUBSC_CURR_BAND_SUBCLASS_SIZE; 

  for ( i = 0; i < cdma_bc_info.band_class_cnt; i++ )
  {
    /* Band Class n*/
    HDRBIT_PACK8( bc_info_ptr, cdma_bc_info.band_class_list[i].band_class, 
                  offset, HDRRUPC_A21_MOBILE_SUBSC_BAND_CLASS_SIZE );
    offset += HDRRUPC_A21_MOBILE_SUBSC_BAND_CLASS_SIZE; 

    /* All Band Subclasses Included */
    HDRBIT_PACK8( bc_info_ptr, 
                  HDRRUPC_A21_MOBILE_SUBSC_ALL_BAND_SUBCLASS_INCL, 
                  offset, HDRRUPC_A21_MOBILE_SUBSC_ALL_BAND_SUBCLASS_INCL_SIZE );
    offset += HDRRUPC_A21_MOBILE_SUBSC_ALL_BAND_SUBCLASS_INCL_SIZE; 

    /* Reserved */
    HDRBIT_PACK8( bc_info_ptr, 0,
                  offset, HDRRUPC_A21_MOBILE_SUBSC_RESV1_SIZE );
    offset += HDRRUPC_A21_MOBILE_SUBSC_RESV1_SIZE; 

    /* Reset temporary sub band class data structures */
    index = 0;
    num_sub_bands_octets = 0;
    memset ( rev_byte, 0, sizeof ( rev_byte ) );

    /* Pack sub band class bit mask */
    if (cdma_bc_info.band_class_list[i].band_sub_class_cnt != 0)
    {
      /* uint32 is used to represent the sub BC info MSB is Sub BC 0 */
      sub_bc_bit_mask = 0;

      for ( j = 0; j < cdma_bc_info.band_class_list[i].band_sub_class_cnt; j++ )
      {
        /* Set the bit in a uint32 bit mask */
        sub_bc_bit_mask |= (uint32)1 << 
            (31 - cdma_bc_info.band_class_list[i].band_sub_class[j]);
      }

      /* The format for sub band class bit mask is s follows

         SC7 SC6   SC5   SC4   SC3   SC2   SC1   SC0
         ...   ......   ......   ......
         SCn SCn-1 SCn-2 SCn-3 SCn-4 SCn-5 SCn-6 SCn-7

         so each byte in the uint32 need to invidually flipped
       */

      /* Bit mask to extract the first byte of uint32 */
      byte_mask_in_uint32 = 0xFF000000;

      /* For each byte in the uint32 */
      for ( k = 4; k > 0; k-- )
      {
        /* Extract each byte in the uint32 */
        byte_in_uint32 = (byte)((sub_bc_bit_mask & byte_mask_in_uint32) >> (8*(k-1)));

        /* Reverse the byte and store in uint32 */
        rev_byte[index] = hdrrupc_reverse_byte( byte_in_uint32 );

        /* Increase the num sub band count if there is altleast on valid 
           sub BC in the octet*/
        if (rev_byte[index] != 0)
        {
          num_sub_bands_octets++;
        }

        /* Shift the bit mask to extract the next byte */
        byte_mask_in_uint32 = byte_mask_in_uint32 >> 8;

        index++;
      }
    }

    /* Band Class n Subclass Length */
    HDRBIT_PACK8( bc_info_ptr, num_sub_bands_octets,
                  offset, HDRRUPC_A21_MOBILE_SUBSC_BAND_SUBCLASS_LENGTH_SIZE );
    offset += HDRRUPC_A21_MOBILE_SUBSC_BAND_SUBCLASS_LENGTH_SIZE; 

    /* If the num sub band count is greater than zero then pack the sub 
       band info */
    if (num_sub_bands_octets > 0)
    {
      for ( k = 0; k < num_sub_bands_octets; k++ )
      {
        /* Pack the sub band class info */
        HDRBIT_PACK8( bc_info_ptr, rev_byte[k], offset, HDRRUPC_MAX_SUB_BANDS/4 );
        offset += (HDRRUPC_MAX_SUB_BANDS/4); 
      }
    }
  }

  /* Place remanining packed message in the capabilities container */
  dsm_pullup( &bc_info_ptr, capabilities_container_ptr, 
              capabilities_container_length );

  /* Free the DSM item pointer */
  dsm_free_packet(&bc_info_ptr);

  return capabilities_container_length;

} /* hdrrupc_get_packed_cdma_bandclass_info() */
#endif /* FEATURE_LTE_TO_HDR */
