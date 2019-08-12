#ifndef mn_ss_h
#define mn_ss_h
/*===========================================================================
                       COPYRIGHT INFORMATION

Copyright (c) 2001, 2002 Qualcomm Technologies, Incorporated and its licensors.  All Rights 
Reserved.  QUALCOMM Proprietary.  Export of this technology or software 
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                       EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mn/vcs/mn_ss.h_v   1.20   13 May 2002 15:15:36   abanh  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/mn_ss.h#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------

7/12/01    CD      Transferred SS data from mn.h
7/18/01    CD      Moved MAX_COMPONENTS to sys_const.h
7/24/01    CD      Transferred SS operation codes from mn_cm.h
8/06/01    CD      Added ss_invoke_data_T, ss_ForBS_T and registerSS_ARG_T 
              
                   Added MN specific values for ss error

8/8/01     CD      Deleted call_ss_operation_req_T and ss_problem_T since they are
                   no longer used

                   Added network ss codes for possible future use

08/16/01   AB      Renamed MMI references to CM.

08/21/01   CD      Moved external data declarations to mnglobal.h

09/07/01   AB      Restored the network SS code for used in SS unit drivers.

10/08/01   AB      Added include file that contains referenced to its data type per Lint.

10/12/01   AB      Changed hold to Hold to remove conflicting definition w/ the sound
                    task defines.

11/1/01    CD      Fixed lint error

12/11/01   AB      Added component type structure for testing.

01/25/02   CD      Moved some #defs to mn_cm_exp.h per CM/UI request
                   Updated Copyright

04/02/02   AB      Added Context Specific Class for the type tags, and CNAP
                     unit test structure ss_cnap_T.

04/30/02   AB      Added #ifdef FEATURE_MN_UNIT_TEST for structure that
                     used for unit test.  Added Multicall_Status session literal.
                   Adjusted the SS_MAX_USS_DATA_LENGTH to match that of 
                     CM defined value in mn_cm_exp.h 

04/15/03   sbs     Added support for Location Services related SS messages

04/20/05   HS      Added support for FEATURE_CCBS

06/14/05   AB      Increase the SS_MAX_ADD_LOC_EST_LENGTH by 1, per TS23.032,
                     encoding of polygon shape.
===========================================================================*/


#include "nas_exp.h"
#include "cc_iei.h"
#include "mn_cm_exp.h"

/* Multicall_Status_Index Session */

typedef struct
{
   CM_IEI_T                       IEI;
   byte                            ss_code;
   basic_service_T                 basic_service;

} ss_ForBS_T;

typedef struct
{ 
   byte                            ss_code;
   basic_service_T                 basic_service;
   cm_called_party_bcd_no_T       cm_forwarded_to_number;
   cm_called_party_subaddress_T   cm_forwarded_to_subaddress;
   ie_no_reply_condition_timer_T   no_reply_condition_timer;

} registerSS_ARG_T;



typedef struct
{
   molr_type_e_type        molr_type;
   location_method_T       location_method;
   lcs_qos_T               lcs_qos;
   lcs_client_external_id_T external_id;
   mlc_number_T            mlc_number;
   gps_assistance_data_T   gps_assistance_data;
   supported_gad_shapes_T  supported_gad_shapes;
} lcs_molr_ARG_T;






typedef struct
{
   word                  length;
   word                  increment;
} return_size_T;


typedef struct
{
   byte             problem_code_tag;
   byte             problem_code;
} ss_reject_T;





/************************************************************************
 *                                                                      *
 *                       MNSS Message Identity values                   *
 *                                                                      *
 ************************************************************************
 * To be loaded into MessageId field of Message Header
 */


/************************************************************************
 *                                                                      *
 *                       MN to SS PRIMITIVES                            *
 *                       -------------------                            *
 *                                                                      *
 ************************************************************************
 */

/************************************************************************
 *                                                                      *
 *                       MNSS_BEGIN_REQ                                 *
 *                                                                      *
 ************************************************************************
 */

typedef struct 
{
   IMH_T              message_header;
   sys_modem_as_id_e_type   as_id;
   connection_id_T    connection_id;
   byte               data[ sizeof(ie_facility_T) ];
} MNSS_BEGIN_REQ_T;

/************************************************************************
 *                                                                      *
 *                       MNSS_END_REQ                                   *
 *                                                                      *
 ************************************************************************
 */

typedef struct 
{
   IMH_T              message_header;
   sys_modem_as_id_e_type   as_id;
   connection_id_T    connection_id;
   byte               data[ sizeof(ie_facility_T) ];
} MNSS_END_REQ_T;

/************************************************************************
 *                                                                      *
 *                       MNSS_FACILITY_REQ                              *
 *                                                                      *
 ************************************************************************
 */

typedef struct 
{
   IMH_T              message_header;
   sys_modem_as_id_e_type   as_id;
   connection_id_T    connection_id;
   byte               data[ sizeof(ie_facility_T) ];
} MNSS_FACILITY_REQ_T;



/************************************************************************
 *                                                                      *
 *                       SS to MN PRIMITIVES                            *
 *                       -------------------                            *
 *                                                                      *
 ************************************************************************
*/

/************************************************************************
 *                                                                      *
 *                       MNSS_BEGIN_IND                                 *
 *                                                                      *
 ************************************************************************
 */

typedef struct 
{
   IMH_T              message_header;
   sys_modem_as_id_e_type   as_id;
   connection_id_T    connection_id;
   byte               data[ sizeof(ie_facility_T) ];
} MNSS_BEGIN_IND_T;

/************************************************************************
 *                                                                      *
 *                       MNSS_END_IND                                   *
 *                                                                      *
 ************************************************************************
 */

typedef struct 
{
   IMH_T              message_header;
   sys_modem_as_id_e_type   as_id;
   connection_id_T    connection_id;
   byte               data[ GSM_CAUSE_SIZE + 
                            sizeof(ie_facility_T) ];
} MNSS_END_IND_T;

/************************************************************************
 *                                                                      *
 *                       MNSS_FACILITY_IND                              *
 *                                                                      *
 ************************************************************************
 */

typedef struct 
{
   IMH_T              message_header;
   sys_modem_as_id_e_type   as_id;
   connection_id_T    connection_id;
   byte               data[ sizeof(ie_facility_T) ];
} MNSS_FACILITY_IND_T;

/************************************************************************
 *                                                                      *
 *                       MNSS_REJ_IND                                   *
 *                                                                      *
 ************************************************************************
 */

typedef struct 
{
   IMH_T              message_header;
   sys_modem_as_id_e_type   as_id;
   connection_id_T    connection_id;
   byte               mm_cause;
   byte               lower_layer_cause;
} MNSS_REJ_IND_T;




/************************************************************************
*   IE:  Information Element Used for Testing only                                                    *  
*                                                                       *
*************************************************************************/

#endif
