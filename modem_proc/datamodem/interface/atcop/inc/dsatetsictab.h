#ifndef DSATETSICTAB_H
#define DSATETSICTAB_H
/*===========================================================================

                        D A T A   S E R V I C E S
                A T   C O M M A N D   P R O C E S S O R

                 E T S I   C O M M A N D   T A B L E S
                I N T E R N A L   H E A D E R   F I L E


DESCRIPTION
  This file contains the definitions of data structures, defined and
  enumerated constants, and function prototypes required for the
  data services AT command processor command tables that define
  commands specific to the ETSI mode of operation.  This file only
  exports definitions internal to the AT command processor unit.

  Copyright (c) 2001 - 2014 by Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath:  L:/src/asw/MSM5200/data/vcs/dsatetsictab.h_v   1.5   12 Jul 2002 10:06:32   randrew  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/atcop/inc/dsatetsictab.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/27/14   sc      Added support for Dynamic ATCoP.
01/19/12   sk      Feature cleanup.
01/11/11   ad      Remove extern usage.used get/set API for command associated  
                   value pointers.
10/25/10   ad      Init Changes for DSDS. 
02/08/10   ua      Removing support for CLVL and CMUT. 
01/20/10   kk      Moved +CFUN to CommonATCoP.
12/15/09   nc      Featurisation changes for LTE.
09/09/09   ua      Added support for EONS (spec 22.101).
10/23/08   bs      Added support for 1X AT Phonebook commands.
10/31/07   ua      Added support for +CSAS and +CRES commands. 
08/24/07   ar      Fix feature wrapping for non-GCSD builds.
08/22/07   sa      Added support for +CGANS and +CGAUTO commands.
08/17/07   ua      Added extern for $QCCNMI support. 
03/24/07   ua      Correcting Banned API changes. 
01/16/06   ua      Added extern for +CDIP support
12/08/05   snb     Add IPV6 support
04/01/05   tkk     Added extern for +CLVL & CMUT support.
04/01/05   ar      Added support for +CIND command and export for +CMER.
03/15/05   tkk     Added export for +CGPADDR and +CGEREP commands.
03/15/05   snb     Remove +CSIM command from &V response, handle all processing
                   in command handler.
03/02/05   snb     Remove DSAT_TIMEZONE_SUPPORTED feature define.
02/28/05   hap     Added support for +CSSN command
02/15/05   ar      Export keypad emulation variables.
02/14/05   tkk     Added extern for +CVIB support.
02/03/05   tkk     Added extern variable for CLIR.
01/27/05   pdv     Merge from Sirius-ATCoP development.
01/27/05   pdv     Rebase from Mainline.
12/30/04   tkk     Added extern variables for CLIP related reporting.
12/29/04   snb     Make dsat_csca_sca_val and dsat_csca_tosca_val external.
12/15/04   snb     Make +CVHU value external.
12/15/04   ar      Add exports for +COPS and +CFUN parameters.
10/21/04   snb     Add support for GSM and UCS2 character sets to +CSCS command
09/03/04   snb     Add support for +CSIM and +CMMS command.
03/19/04   snb     Add +CCWA <n> parm, controlling display of unsolicited 
                   result code.
07/30/03   ar      Adjusted export of dsat_crlp_dflm variable.
06/05/03   ar      Add featurization required for CDMA only MSM6300 build
04/25/03   snb     Split etsi extended command table into parm and action tables.
03/07/03   ar      Removed mode-specific +CBST limit array externs
                   Remove FEATURE_DATA_ETSI_SUPSERV wrappers
02/25/03   wx      Use mixed_param to implement +ES's parameters
02/10/03   wx      Add suppot for +CGSMS command
01/17/03   wx      Added export of +CCUG parameters
01/09/03   wx      Added support for +ES and +ESA
07/03/02   ar      Add support for GPRS command parameters
06/27/02   ar      Add suppot for QoS profile parameters
10/08/01   wx      Add +cpbs,r,f,w support.
08/28/01   sjd     Initial version of file to support new ATCOP VU structure.

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"


#include "dsati.h"


/*===========================================================================

                        PUBLIC DATA DECLARATIONS
                      
===========================================================================*/

/*---------------------------------------------------------------------------
                        Defines
---------------------------------------------------------------------------*/

#ifdef FEATURE_DSAT_ETSI_DATA

  #ifdef FEATURE_DATA_PS_IPV6
    /* 16 x 4 char hex nums + 15 x ':' + NULL */
    #define MAX_IPADDR_STR_LEN 128 
    /* IPv6 address with subnet "2001:0DB8:0000:CD30:0000:0000:0000:0000 FFFF:FFFF:FFFF:FFF0:0000:0000:0000:0000" */
    /*27.007 8.62 Printing IP address format +CGPIAF*/
    /*"a1.a2.a3.a4.a5.a6.a7.a8.a9.a10.a11.a12.a13.a14.a15.a16.m1.m2.m3.m4.m5.m6.m7.m8.m9.m10.m11.m12.m13.m14.m15.m16", for IPv6.*/

    /* 32 x 3 char dec nums + 31 x '.' + NULL */
    #define MAX_ADDR_SUBNET_STRING_LEN 128

  #else  /* FEATURE_DATA_PS_IPV6 */
    /* 4 x 3 char dec nums + 3 x '.' + NULL */
    #define MAX_IPADDR_STR_LEN 16 
    /* 8 x 3 char dec nums + 7 x '.' + NULL */
    #define MAX_ADDR_SUBNET_STRING_LEN 32  

  #endif /* FEATURE_DATA_PS_IPV6 */

#endif /* FEATURE_DSAT_ETSI_DATA */
/*---------------------------------------------------------------------------
                        Command defaults and limits
---------------------------------------------------------------------------*/

extern const def_list_type dsat_csta_list;
extern const def_list_type dsat_cr_list;
extern const def_list_type dsat_crc_list;
extern const def_list_type dsat_cmee_list;

/* Externing CNMI variables to be used by QCCNMI also */
extern const def_list_type dsat_cnmi_list[];

#if defined(FEATURE_DATA_GCSD) || defined(FEATURE_DATA_WCDMA_CS)
extern const def_list_type dsat_cbst_list[];
extern const dsat_string_item_type dsat_cbst_speed_valstr[][8];
extern const dsat_string_item_type dsat_cbst_name_valstr[][8];
extern const dsat_string_item_type dsat_cbst_ce_valstr[][8];

extern const dflm_type dsat_crlp_dflm[][MAX_CRLP_PARAMS];
#endif /* defined(FEATURE_DATA_GCSD) || defined(FEATURE_DATA_WCDMA_CS) */
extern const dflm_type dsat_ccwa_dflm; 
/*---------------------------------------------------------------------------
                        Command Parameter Values
---------------------------------------------------------------------------*/
#ifdef FEATURE_ETSI_SMS
extern const def_list_type dsat_cpms_list[];

#ifdef FEATURE_ETSI_SMS_PS
extern const dflm_type    dsat_cgsms_dflm;
#endif /* FEATURE_ETSI_SMS_PS */

#endif /* FEATURE_ETSI_SMS */

/* Command parameters that are used in SMS file */
extern const dflm_type dsat_cmms_dflm;

#ifdef FEATURE_DSAT_ETSI_DATA
#ifdef FEATURE_DATA_TE_MT_PDP
extern dsat_num_item_type dsat_cgauto_val;
#endif /* FEATURE_DATA_TE_MT_PDP */
#endif /* FEATURE_DSAT_ETSI_DATA */

#ifdef FEATURE_WCDMA
extern const dflm_type dsat_es_orig_dflm;
extern const dflm_type dsat_es_ans_fbk_dflm;
#endif /* FEATURE_WCDMA */
extern dsat_string_item_type dsat_csca_sca_val[][CSCA_SCA_STR_MAX_LEN+1];
extern dsat_string_item_type dsat_csca_tosca_val[][CSCA_TOSCA_STR_MAX_LEN+1];

#define CSMP_FO_STR_MAX_LEN 3
#define CSMP_VP_STR_MAX_LEN 22
/* Macros to be used as indices for the command variables */
#define CSCA_SCA 0
#define CSCA_TOSCA 1
#define CSMP_FO 0
#define CSMP_VP 1
#define CSMP_PID 2
#define CSMP_DCS 3
#ifdef FEATURE_ETSI_SMS_CB
#define CSCB_MODE 0
#define CSCB_MIDS 1
#define CSCB_DCSS 2
#endif /* FEATURE_ETSI_SMS_CB */
/* End of Macros as indexes */

/*--------------------------------------------------------------------------
                    ETSI Command Tables and Table Sizes 
--------------------------------------------------------------------------*/

extern const dsati_cmd_type dsatetsi_ext_table[ ];
extern const dsati_cmd_ex_type dsatetsi_ext_table_ex[ ];
extern const unsigned int dsatetsi_ext_table_size;

#ifdef FEATURE_DSAT_ETSI_MODE
extern const dsati_cmd_type dsatetsi_ext_action_table[ ];
extern const dsati_cmd_ex_type dsatetsi_ext_action_table_ex[ ];
extern const unsigned int dsatetsi_ext_action_table_size;
/* Sound related commands information */
extern dsat_num_item_type dsat_crsl_val;
extern dsat_num_item_type dsat_calm_val;
extern dsat_num_item_type dsat_cvib_val;

#endif /* FEATURE_DSAT_ETSI_MODE */


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/



#endif /* DSATETSICTAB_H */
