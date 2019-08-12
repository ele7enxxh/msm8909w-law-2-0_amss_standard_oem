#ifndef DSATCTAB_H
#define DSATCTAB_H
/*===========================================================================

                        D A T A   S E R V I C E S
                A T   C O M M A N D   P R O C E S S O R

               C O M M O N   C O M M A N D   T A B L E S                
                I N T E R N A L   H E A D E R   F I L E


DESCRIPTION
  This file contains the definitions of data structures, defined and
  enumerated constants, and function prototypes required for the
  data services AT command processor command tables that define
  commands common to all modes of operation.  This file only exports
  definitions internal to the AT command processor unit.

   Copyright (c) 2001 - 2014 by Qualcomm Technologies Incorporated.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/dsatctab.h_v   1.1   08 Nov 2002 12:46:28   wxie  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/atcop/inc/dsatctab.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/27/14   sc      Added support for Dynamic ATCoP.
12/03/12   tk      Mainlined the PMIC RTC features.
01/19/12   sk      Feature cleanup.
07/05/11   nc      Added support for ^MODE,^CARDMODE,^SYSCONFIG,^SYSINFO,^DSCI.
04/01/11   ttv     Added changes to use get/set functions.
02/16/11   ttv     Added DSDS changes for consolidated profile family.
01/11/11   ad      Removed pending variable .
01/11/11   ad      Remove extern usage.used get/set API for command associated  
                   value pointers.
10/25/10   ad      Init Changes for DSDS. 
05/10/10   kk      Mainlining pending GOBI changes.
01/20/10   kk      Extended +CFUN to CDMA/MM targets.
06/10/09   ua      Added support for ^PREFMODE command. 
07/16/09   ua      Added support for $QCBANDPREF command.
12/15/09   nc      Featurisation changes for LTE.
08/04/09   nc      Added support for *CNTI.
09/09/09   ua      Added support for EONS (spec 22.101).
10/23/08   bs      Added support for 1X AT Phonebook commands.
07/06/07   ar      Added export of dsat_ds_dflm variable.
09/08/05   ar      Add support for $QCSDFTEST vendor command.
01/11/05   ar      Remove unnecessary wrapper on +DR parameter
11/19/03   ar      Define limit for DNS values.
07/11/03   snb     Correct feature definitions.
05/28/03   sb      Fix for WCDMA CS Calls
11/08/02   wx      Move dsat_qcdns_val declaration to dsat.h to export it 
                   for ps.
04/17/02   rc      Removed FEATURE wrap FEATURE_DS_SOCKETS around dns 
                   variables.
08/28/01   sjd     Initial version of file to support new ATCOP VU structure.

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"


#include "dsati.h"
#include "time_svc.h"

/*===========================================================================

                        PUBLIC DATA DECLARATIONS
                      
===========================================================================*/

/*---------------------------------------------------------------------------
                        Command defaults and limits
---------------------------------------------------------------------------*/

/* Note: Command defaults and limits should not be exported in the common
   command table header file.  Any command in the common command table
   should behave exactly the same across all operating modes, as such
   it should never be necessary to create another target specific command
   table defining a different behavior for one of these common commands. */


/*---------------------------------------------------------------------------
                        Command Parameter Values
---------------------------------------------------------------------------*/
#define DEFAULT_CPB_STORAGE "SM"
#define DEFAULT_NV_CPB_STORAGE "ME"

/* Parameter shared across common ETSI and IS-707 +CRC commands. */
extern const dflm_type dsat_ds_dflm[];
/* Parameter shared across common ETSI and 1X for Phonebook commands. */
#if defined(FEATURE_ETSI_PBM) || defined(FEATURE_DSAT_CDMA_PBM)
extern const dsat_string_item_type dsat_cpbs_storage_valstr [][8];
extern const def_list_type dsat_cpbs_list;
#endif /* defined(FEATURE_ETSI_PBM) || defined(FEATURE_DSAT_CDMA_PBM) */

#ifdef FEATURE_DATA_UCSD_SCUDIF_TEST
extern def_list_type dsat_qcscftest_list[];
#endif /* FEATURE_DATA_UCSD_SCUDIF_TEST */

extern const dsat_string_item_type dsat_ipr_valstr [][8];

/*--------------------------------------------------------------------------
                    Common Command Tables and Table Sizes 
--------------------------------------------------------------------------*/

extern const dsati_cmd_type dsat_basic_table[ ];
extern const dsati_cmd_type dsat_basic_action_table[ ];
extern const dsati_cmd_type dsat_sreg_table[ ];
extern const dsati_cmd_type dsat_ext_table[ ];
extern const dsati_cmd_type dsat_vendor_table[ ];

extern const unsigned int dsat_basic_table_size;
extern const unsigned int dsat_basic_action_table_size;
extern const unsigned int dsat_sreg_table_size;
extern const unsigned int dsat_ext_table_size;
extern const unsigned int dsat_vendor_table_size;
extern const unsigned int dsat_abort_table_size;

/*--------------------------------------------------------------------------
                    Common Command Tables
--------------------------------------------------------------------------*/

extern const dsati_cmd_ex_type dsat_basic_table_ex[ ];
extern const dsati_cmd_ex_type dsat_basic_action_table_ex[ ];
extern const dsati_cmd_ex_type dsat_sreg_table_ex[ ];
extern const dsati_cmd_ex_type dsat_ext_table_ex[ ];
extern const dsati_cmd_ex_type dsat_vendor_table_ex[ ];

/*--------------------------------------------------------------------------
                    Abort Command Table
--------------------------------------------------------------------------*/
extern const dsati_cmd_abort_type dsat_abort_table[ ];

#ifdef FEATURE_DSAT_ETSI_MODE
#ifdef FEATURE_MMGSDI
#define CPOL_INVALID_INDEX 0XFF
#define CPOL_NO_ACT        0x0
#endif /* FEATURE_MMGSDI */

/*---------------------------------------------------------------------------
                           NV interface data 
---------------------------------------------------------------------------*/

#ifdef FEATURE_DSAT_DEV_CMDS

extern dsati_cnti_tech_enum_type dsat_qccnti_tech;

#endif /* FEATURE_DSAT_DEV_CMDS */
#endif /* FEATURE_DSAT_ETSI_MODE */

extern nv_item_type         ds_nv_item;            /* actual data item     */

/* ^PREFMODE values */
#define DSAT_PREFMODE_SYSMODE_CDMA              2
#define DSAT_PREFMODE_SYSMODE_HDR               4
#define DSAT_PREFMODE_SYSMODE_HYBRID_CDMA_HDR   8


/* Max input data for selecting bands */
#define DSAT_MAX_BANDPREF_STRING_LENGTH 60
#define DSAT_MAX_BANDPREF DSAT_BAND_PREF_MAX

#ifdef FEATURE_DSAT_EXTENDED_CMD
#define DSAT_PREFMODE_SYSMODE_AUTOMATIC         DSAT_PREFMODE_SYSMODE_HYBRID_CDMA_HDR
#else
#define DSAT_PREFMODE_SYSMODE_AUTOMATIC         0
#endif /* FEATURE_DSAT_EXTENDED_CMD */

#define DSAT_SYSCONFIG_MODE_AUTOMATIC         2
#define DSAT_SYSCONFIG_MODE_GSM               13
#define DSAT_SYSCONFIG_MODE_WCDMA             14
#define DSAT_SYSCONFIG_MODE_TDSCDMA           15
#define DSAT_SYSCONFIG_MODE_NO_CHANGE         16
#define DSAT_SYSCONFIG_MODE_UNKNOWN           17

#define DSAT_SYSCONFIG_ORDER_AUTOMATIC        0
#define DSAT_SYSCONFIG_ORDER_GSM_WCDMA        1
#define DSAT_SYSCONFIG_ORDER_WCDMA_GSM        2
#define DSAT_SYSCONFIG_ORDER_NO_CHANGE        3
#define DSAT_SYSCONFIG_ORDER_UNKNOWN          4

#define DSAT_SYSCONFIG_ROAM_NONE              0
#define DSAT_SYSCONFIG_ROAM_ANY               1
#define DSAT_SYSCONFIG_ROAM_NO_CHANGE         2
#define DSAT_SYSCONFIG_ROAM_UNKNOWN           3

#define DSAT_SYSCONFIG_DOMAIN_CS              0
#define DSAT_SYSCONFIG_DOMAIN_PS              1
#define DSAT_SYSCONFIG_DOMAIN_CS_PS           2
#define DSAT_SYSCONFIG_DOMAIN_ANY             3
#define DSAT_SYSCONFIG_DOMAIN_NO_CHANGE       4
#define DSAT_SYSCONFIG_DOMAIN_UNKNOWN         5

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/


/*===========================================================================

FUNCTION DSATCTAB_DATA_INIT

DESCRIPTION
  This function initializes the AT command defaults, limits, and lists
  that depend on a run-time determination of the hardware.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/

void dsatctab_data_init( void );

#ifdef FEATURE_ATCOP_ECHO_CTL_MULTI_PORTS
/*===========================================================================

FUNCTION DSATCTAB_GET_MODEM_PORT_ID

DESCRIPTION
  This function returns the corresponding modem port id for AT terminal

DEPENDENCIES
  None
  
RETURN VALUE
  -1: if the input port is not modem port

SIDE EFFECTS
  None

===========================================================================*/
int dsatctab_get_modem_port_id(ds3g_siolib_port_e_type port);
#endif /*FEATURE_ATCOP_ECHO_CTL_MULTI_PORTS*/


#endif /* DSATCTAB_H */
