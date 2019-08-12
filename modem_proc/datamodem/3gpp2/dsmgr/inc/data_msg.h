#ifndef DATA_MSG_H
/*lint -e750*/
#define DATA_MSG_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
         
                       D A T A   S E R V I C E S 
              DIAGNOSTIC MESSAGE 2.0 SERVICE HEADER FILE

GENERAL DESCRIPTION

	All the declarations and definitions necessary to support the reporting 
	of messages for errors and debugging.  This includes support for the 
	extended capabilities as well as the legacy messaging scheme.
  
  Copyright (c) 2003-2012,2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
  *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
  
  /*==================================================================
						  EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/dsmgr/inc/data_msg.h#1 $

when       who         what, where, why
--------   -----       ----------------------------------------------------------
03/14/14   tk          Optimized debug macros usage in ATCoP.
01/02/12   msh         Coan: Feature cleanup
05/12/11   op          Added support for HDR data subsystem
03/17/11   ack         Update to use Diag Macros
04/03/10   vk          Added support for 3GPP, LTE, WCDMA data subsystems
12/07/03   igt         Checkin for first version of data_msg.h

===========================================================================*/

/*===========================================================================

                              INCLUDE FILES

===========================================================================*/

/*---------------------------------------------------------------------------
  This will also make available all the Sub-System IDs and the build masks
---------------------------------------------------------------------------*/
#include "msg.h"

/*===========================================================================
  Sub-System specific  Macros for DS Diagnostic messages 2.0. These will
  support the following sub-systems and build-masks
  
    example from QTV: MSG_SSID_APPS_QTV, MSG_BUILD_MASK_MSG_SSID_APPS_QTV 
    example from DS:  MSG_BUILD_MASK_MSG_SSID_DATA e.t.c.
  
===========================================================================*/

/*--------------------------------------------------------------------------
ERR_FATAL messages
---------------------------------------------------------------------------*/
#define DATA_ERR_FATAL(xx_exp) ERR_FATAL("Assertion " #xx_exp " failed", 0, 0, 0 );

/*---------------------------------------------------------------------------
  These SS specific macros are to be used by the developers that want to 
  issue QXDM messages from the Data Services software. These call the general
  macros and pass the "build mask" as well as the SSID as parameters. 
  The developer only has to remember the sub-group ID.
---------------------------------------------------------------------------*/  
 
/*===========================================================================
  LEVEL1 SS related macros
===========================================================================*/

/*---------------------------------------------------------------------------
  Macro for messages with no parameters.
---------------------------------------------------------------------------*/
#define DATA_MSG0(xx_ss_mask, xx_fmt) \
  MSG(MSG_SSID_DS, xx_ss_mask, xx_fmt)  

/*---------------------------------------------------------------------------
  Macro for messages with 1 parameters.
---------------------------------------------------------------------------*/    
#define DATA_MSG1(xx_ss_mask, xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS, xx_ss_mask, xx_fmt, xx_arg1)

/*---------------------------------------------------------------------------
  Macro for messages with 2 parameters.
---------------------------------------------------------------------------*/
#define DATA_MSG2(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2)

/*---------------------------------------------------------------------------
  Macro for messages with 3 parameters.
---------------------------------------------------------------------------*/
#define DATA_MSG3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

/*---------------------------------------------------------------------------
  This is the macro for messages with 4 parameters.
---------------------------------------------------------------------------*/
#define DATA_MSG4(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4) \
  MSG_4(MSG_SSID_DS, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4)

/*---------------------------------------------------------------------------
  This is the macro for messages with 5 parameters.
---------------------------------------------------------------------------*/
#define DATA_MSG5(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                                               xx_arg5) \
  MSG_5(MSG_SSID_DS, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5)

/*---------------------------------------------------------------------------
  This is the macro for messages with 6 parameters 
---------------------------------------------------------------------------*/
#define DATA_MSG6(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                                          xx_arg5, xx_arg6) \
  MSG_6(MSG_SSID_DS, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6)

/*---------------------------------------------------------------------------
  This is the macro for messages with 7 parameters 
---------------------------------------------------------------------------*/
#define DATA_MSG7(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                                 xx_arg5, xx_arg6, xx_arg7) \
  MSG_7(MSG_SSID_DS, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6, xx_arg7)

/*---------------------------------------------------------------------------
  This is the macro for messages with 8 parameters 
---------------------------------------------------------------------------*/
#define DATA_MSG8(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                        xx_arg5, xx_arg6, xx_arg7, xx_arg8) \
  MSG_8(MSG_SSID_DS, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6, xx_arg7, xx_arg8)

/*---------------------------------------------------------------------------
  This is the macro for messages with 9 parameters 
---------------------------------------------------------------------------*/
#define DATA_MSG9(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                               xx_arg5, xx_arg6, xx_arg7, xx_arg8, xx_arg9) \
  MSG_9(MSG_SSID_DS, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6, xx_arg7, xx_arg8, xx_arg9)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 1 parameters
---------------------------------------------------------------------------*/
#define DATA_MSG_SPRINTF_1(xx_ss_mask, xx_fmt, xx_arg1) \
  MSG_SPRINTF_1(MSG_SSID_DS, xx_ss_mask, xx_fmt, xx_arg1)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 2 parameters
---------------------------------------------------------------------------*/
#define DATA_MSG_SPRINTF_2(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2) \
  MSG_SPRINTF_2(MSG_SSID_DS, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 3 parameters
---------------------------------------------------------------------------*/
#define DATA_MSG_SPRINTF_3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_SPRINTF_3(MSG_SSID_DS, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 3 parameters. This Macro is 
  used when xx_fmt is passed at runtime.
---------------------------------------------------------------------------*/
#define DATA_MSG_SPRINTF_FMT_VAR_3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_SPRINTF_FMT_VAR_3(MSG_SSID_DS, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3)


/*===========================================================================
  LEVEL1 SS related convenience macros. These will substitute the SSID as
  well as the sub-group ID.
===========================================================================*/


/*---------------------------------------------------------------------------
  DATA SSID (LEVEL1) Macros that auto-include the sub-group
---------------------------------------------------------------------------*/
#define DATA_MSG0_LOW(xx_fmt) \
  MSG(MSG_SSID_DS, MSG_LEGACY_LOW, xx_fmt)

#define DATA_MSG0_MED(xx_fmt) \
  MSG(MSG_SSID_DS, MSG_LEGACY_MED, xx_fmt)

#define DATA_MSG0_HIGH(xx_fmt) \
  MSG(MSG_SSID_DS, MSG_LEGACY_HIGH, xx_fmt)
  
#define DATA_MSG0_ERROR(xx_fmt) \
  MSG(MSG_SSID_DS, MSG_LEGACY_ERROR, xx_fmt)
  
#define DATA_MSG0_FATAL(xx_fmt) \
  MSG(MSG_SSID_DS, MSG_LEGACY_FATAL, xx_fmt)


/*---------------------------------------------------------------------------
  Macro for messages with 1 parameters.
---------------------------------------------------------------------------*/    
#define DATA_MSG1_LOW(xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS, MSG_LEGACY_LOW, xx_fmt, xx_arg1)

#define DATA_MSG1_MED(xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS, MSG_LEGACY_MED, xx_fmt, xx_arg1)

#define DATA_MSG1_HIGH(xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS, MSG_LEGACY_HIGH, xx_fmt, xx_arg1)
  
#define DATA_MSG1_ERROR(xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS, MSG_LEGACY_ERROR, xx_fmt, xx_arg1)
  
#define DATA_MSG1_FATAL(xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS, MSG_LEGACY_FATAL, xx_fmt, xx_arg1)

/*---------------------------------------------------------------------------
  Macro for messages with 2 parameters.
---------------------------------------------------------------------------*/
#define DATA_MSG2_LOW( xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS, MSG_LEGACY_LOW, xx_fmt, xx_arg1, xx_arg2)

#define DATA_MSG2_MED( xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS, MSG_LEGACY_MED, xx_fmt, xx_arg1, xx_arg2)

#define DATA_MSG2_HIGH( xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS, MSG_LEGACY_HIGH, xx_fmt, xx_arg1, xx_arg2)
  
#define DATA_MSG2_ERROR( xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS, MSG_LEGACY_ERROR, xx_fmt, xx_arg1, xx_arg2)
                                             
#define DATA_MSG2_FATAL( xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS, MSG_LEGACY_FATAL, xx_fmt, xx_arg1, xx_arg2)
  
/*---------------------------------------------------------------------------
  Macro for messages with 3 parameters.
---------------------------------------------------------------------------*/
#define DATA_MSG3_LOW(xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS, MSG_LEGACY_LOW, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

#define DATA_MSG3_MED(xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS, MSG_LEGACY_MED, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

#define DATA_MSG3_HIGH(xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS, MSG_LEGACY_HIGH, xx_fmt, xx_arg1, xx_arg2, xx_arg3)
  
#define DATA_MSG3_ERROR(xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS, MSG_LEGACY_ERROR, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

#define DATA_MSG3_FATAL(xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS, MSG_LEGACY_FATAL, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

/*---------------------------------------------------------------------------
  Macro for messages with 4 parameters.
---------------------------------------------------------------------------*/
#define DATA_MSG4_LOW(xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4) \
  MSG_4(MSG_SSID_DS, MSG_LEGACY_LOW, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4)

#define DATA_MSG4_MED(xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4) \
  MSG_4(MSG_SSID_DS, MSG_LEGACY_MED, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4)

#define DATA_MSG4_HIGH(xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4) \
  MSG_4(MSG_SSID_DS, MSG_LEGACY_HIGH, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4)
  
#define DATA_MSG4_ERROR(xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4) \
  MSG_4(MSG_SSID_DS, MSG_LEGACY_ERROR, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4)

#define DATA_MSG4_FATAL(xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4) \
  MSG_4(MSG_SSID_DS, MSG_LEGACY_FATAL, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4)

/*---------------------------------------------------------------------------
  Macro for messages with 5 parameters.
---------------------------------------------------------------------------*/
#define DATA_MSG5_LOW(xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5) \
  MSG_5(MSG_SSID_DS, MSG_LEGACY_LOW, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5)

#define DATA_MSG5_MED(xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5) \
  MSG_5(MSG_SSID_DS, MSG_LEGACY_MED, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5)

#define DATA_MSG5_HIGH(xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5) \
  MSG_5(MSG_SSID_DS, MSG_LEGACY_HIGH, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5)
  
#define DATA_MSG5_ERROR(xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5) \
  MSG_5(MSG_SSID_DS, MSG_LEGACY_ERROR, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5)

#define DATA_MSG5_FATAL(xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5) \
  MSG_5(MSG_SSID_DS, MSG_LEGACY_FATAL, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5)

/*---------------------------------------------------------------------------
  Macro for messages with 6 parameters.
---------------------------------------------------------------------------*/
#define DATA_MSG6_LOW(xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6) \
  MSG_6(MSG_SSID_DS, MSG_LEGACY_LOW, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6)

#define DATA_MSG6_MED(xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6) \
  MSG_6(MSG_SSID_DS, MSG_LEGACY_MED, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6)

#define DATA_MSG6_HIGH(xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6) \
  MSG_6(MSG_SSID_DS, MSG_LEGACY_HIGH, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6)
  
#define DATA_MSG6_ERROR(xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6) \
  MSG_6(MSG_SSID_DS, MSG_LEGACY_ERROR, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6)

#define DATA_MSG6_FATAL(xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6) \
  MSG_6(MSG_SSID_DS, MSG_LEGACY_FATAL, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6)

/*===========================================================================
  RLP SS related macros
===========================================================================*/


/*---------------------------------------------------------------------------
  Macro for messages with no parameters.
---------------------------------------------------------------------------*/
#define DATA_RLP_MSG0(xx_ss_mask, xx_fmt) \
  MSG(MSG_SSID_DS_RLP, xx_ss_mask, xx_fmt)  

/*---------------------------------------------------------------------------
  Macro for messages with 1 parameters.
---------------------------------------------------------------------------*/    
#define DATA_RLP_MSG1(xx_ss_mask, xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS_RLP, xx_ss_mask, xx_fmt, xx_arg1)

/*---------------------------------------------------------------------------
  Macro for messages with 2 parameters.
---------------------------------------------------------------------------*/
#define DATA_RLP_MSG2(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS_RLP, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2)

/*---------------------------------------------------------------------------
  Macro for messages with 3 parameters.
---------------------------------------------------------------------------*/
#define DATA_RLP_MSG3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS_RLP, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

/*---------------------------------------------------------------------------
  This is the macro for messages with 4 parameters.
---------------------------------------------------------------------------*/
#define DATA_RLP_MSG4(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4) \
  MSG_4(MSG_SSID_DS_RLP, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4)

/*---------------------------------------------------------------------------
  This is the macro for messages with 5 parameters.
---------------------------------------------------------------------------*/
#define DATA_RLP_MSG5(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                                               xx_arg5) \
  MSG_5(MSG_SSID_DS_RLP, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5)

/*---------------------------------------------------------------------------
  This is the macro for messages with 6 parameters 
---------------------------------------------------------------------------*/
#define DATA_RLP_MSG6(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                                          xx_arg5, xx_arg6) \
  MSG_6(MSG_SSID_DS_RLP, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6)

/*---------------------------------------------------------------------------
  This is the macro for messages with 7 parameters 
---------------------------------------------------------------------------*/
#define DATA_RLP_MSG7(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                                 xx_arg5, xx_arg6, xx_arg7) \
  MSG_7(MSG_SSID_DS_RLP, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6, xx_arg7)

/*---------------------------------------------------------------------------
  This is the macro for messages with 8 parameters 
---------------------------------------------------------------------------*/
#define DATA_RLP_MSG8(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                        xx_arg5, xx_arg6, xx_arg7, xx_arg8) \
  MSG_8(MSG_SSID_DS_RLP, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6, xx_arg7, xx_arg8)

/*---------------------------------------------------------------------------
  This is the macro for messages with 9 parameters 
---------------------------------------------------------------------------*/
#define DATA_RLP_MSG9(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                               xx_arg5, xx_arg6, xx_arg7, xx_arg8, xx_arg9) \
  MSG_9(MSG_SSID_DS_RLP, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6, xx_arg7, xx_arg8, xx_arg9)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 1 parameters
---------------------------------------------------------------------------*/
#define DATA_RLP_MSG_SPRINTF_1(xx_ss_mask, xx_fmt, xx_arg1) \
  MSG_SPRINTF_1(MSG_SSID_DS_RLP, xx_ss_mask, xx_fmt, xx_arg1)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 2 parameters
---------------------------------------------------------------------------*/
#define DATA_RLP_MSG_SPRINTF_2(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2) \
  MSG_SPRINTF_2(MSG_SSID_DS_RLP, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 3 parameters
---------------------------------------------------------------------------*/
#define DATA_RLP_MSG_SPRINTF_3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_SPRINTF_3(MSG_SSID_DS_RLP, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 3 parameters. This Macro is 
  used when xx_fmt is passed at runtime.
---------------------------------------------------------------------------*/
#define DATA_RLP_MSG_SPRINTF_FMT_VAR_3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_SPRINTF_FMT_VAR_3(MSG_SSID_DS_RLP, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

//IGT
/*===========================================================================
  RLP SS related convenience macros. These will substitute the SSID as
  well as the desired sub-group ID.
===========================================================================*/

/*---------------------------------------------------------------------------
  Macro for messages with no parameters.
---------------------------------------------------------------------------*/
#define DATA_RLP_MSG0_GEN(xx_fmt) \
  MSG(MSG_SSID_DS_RLP, DATA_RLP_GENERAL, xx_fmt)
  
#define DATA_RLP_MSG0_DBG(xx_fmt) \
  MSG(MSG_SSID_DS_RLP, DATA_RLP_DEBUG, xx_fmt)
  
#define DATA_RLP_MSG0_TRC(xx_fmt) \
  MSG(MSG_SSID_DS_RLP, DATA_RLP_TRACE, xx_fmt)

/*---------------------------------------------------------------------------
  Macro for messages with 1 parameters.
---------------------------------------------------------------------------*/    
#define DATA_RLP_MSG1_GEN(xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS_RLP, DATA_RLP_GENERAL, xx_fmt, xx_arg1)

#define DATA_RLP_MSG1_DBG(xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS_RLP, DATA_RLP_DEBUG, xx_fmt, xx_arg1)
  
#define DATA_RLP_MSG1_TRC(xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS_RLP, DATA_RLP_TRACE, xx_fmt, xx_arg1)

/*---------------------------------------------------------------------------
  Macro for messages with 2 parameters.
---------------------------------------------------------------------------*/
#define DATA_RLP_MSG2_GEN(xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS_RLP, DATA_RLP_GENERAL, xx_fmt, xx_arg1, xx_arg2)

#define DATA_RLP_MSG2_DBG(xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS_RLP, DATA_RLP_DEBUG, xx_fmt, xx_arg1, xx_arg2)

#define DATA_RLP_MSG2_TRC(xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS_RLP, DATA_RLP_TRACE, xx_fmt, xx_arg1, xx_arg2)

/*---------------------------------------------------------------------------
  Macro for messages with 3 parameters.
---------------------------------------------------------------------------*/
#define DATA_RLP_MSG3_GEN(xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS_RLP, DATA_RLP_GENERAL, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

#define DATA_RLP_MSG3_DBG(xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS_RLP, DATA_RLP_DEBUG, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

#define DATA_RLP_MSG3_TRC(xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS_RLP, DATA_RLP_TRACE, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

    



/*===========================================================================
  PPP SS related macros
===========================================================================*/



/*---------------------------------------------------------------------------
  Macro for messages with no parameters.
---------------------------------------------------------------------------*/
#define DATA_PPP_MSG0(xx_ss_mask, xx_fmt) \
  MSG(MSG_SSID_DS_PPP, xx_ss_mask, xx_fmt)  

/*---------------------------------------------------------------------------
  Macro for messages with 1 parameters.
---------------------------------------------------------------------------*/    
#define DATA_PPP_MSG1(xx_ss_mask, xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS_PPP, xx_ss_mask, xx_fmt, xx_arg1)

/*---------------------------------------------------------------------------
  Macro for messages with 2 parameters.
---------------------------------------------------------------------------*/
#define DATA_PPP_MSG2(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS_PPP, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2)

/*---------------------------------------------------------------------------
  Macro for messages with 3 parameters.
---------------------------------------------------------------------------*/
#define DATA_PPP_MSG3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS_PPP, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

/*---------------------------------------------------------------------------
  This is the macro for messages with 4 parameters.
---------------------------------------------------------------------------*/
#define DATA_PPP_MSG4(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4) \
  MSG_4(MSG_SSID_DS_PPP, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4)

/*---------------------------------------------------------------------------
  This is the macro for messages with 5 parameters.
---------------------------------------------------------------------------*/
#define DATA_PPP_MSG5(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                                               xx_arg5) \
  MSG_5(MSG_SSID_DS_PPP, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5)

/*---------------------------------------------------------------------------
  This is the macro for messages with 6 parameters 
---------------------------------------------------------------------------*/
#define DATA_PPP_MSG6(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                                          xx_arg5, xx_arg6) \
  MSG_6(MSG_SSID_DS_PPP, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6)

/*---------------------------------------------------------------------------
  This is the macro for messages with 7 parameters 
---------------------------------------------------------------------------*/
#define DATA_PPP_MSG7(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                                 xx_arg5, xx_arg6, xx_arg7) \
  MSG_7(MSG_SSID_DS_PPP, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6, xx_arg7)

/*---------------------------------------------------------------------------
  This is the macro for messages with 8 parameters 
---------------------------------------------------------------------------*/
#define DATA_PPP_MSG8(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                        xx_arg5, xx_arg6, xx_arg7, xx_arg8) \
  MSG_8(MSG_SSID_DS_PPP, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6, xx_arg7, xx_arg8)

/*---------------------------------------------------------------------------
  This is the macro for messages with 9 parameters 
---------------------------------------------------------------------------*/
#define DATA_PPP_MSG9(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                               xx_arg5, xx_arg6, xx_arg7, xx_arg8, xx_arg9) \
  MSG_9(MSG_SSID_DS_PPP, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6, xx_arg7, xx_arg8, xx_arg9)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 1 parameters
---------------------------------------------------------------------------*/
#define DATA_PPP_MSG_SPRINTF_1(xx_ss_mask, xx_fmt, xx_arg1) \
  MSG_SPRINTF_1(MSG_SSID_DS_PPP, xx_ss_mask, xx_fmt, xx_arg1)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 2 parameters
---------------------------------------------------------------------------*/
#define DATA_PPP_MSG_SPRINTF_2(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2) \
  MSG_SPRINTF_2(MSG_SSID_DS_PPP, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 3 parameters
---------------------------------------------------------------------------*/
#define DATA_PPP_MSG_SPRINTF_3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_SPRINTF_3(MSG_SSID_DS_PPP, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 3 parameters. This Macro is 
  used when xx_fmt is passed at runtime.
---------------------------------------------------------------------------*/
#define DATA_PPP_MSG_SPRINTF_FMT_VAR_3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_SPRINTF_FMT_VAR_3(MSG_SSID_DS_PPP, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

/*===========================================================================
  PPP SS related convenience macros. These will substitute the SSID as
  well as the desired sub-group ID.
===========================================================================*/

/*---------------------------------------------------------------------------
  Macro for messages with no parameters.
---------------------------------------------------------------------------*/
#define DATA_PPP_MSG0_GEN(xx_fmt) \
  MSG(MSG_SSID_DS_PPP, DATA_PPP_GENERAL, xx_fmt)
  
#define DATA_PPP_MSG0_DBG(xx_fmt) \
  MSG(MSG_SSID_DS_PPP, DATA_PPP_DEBUG, xx_fmt)
  
#define DATA_PPP_MSG0_TRC(xx_fmt) \
  MSG(MSG_SSID_DS_PPP, DATA_PPP_TRACE, xx_fmt)

/*---------------------------------------------------------------------------
  Macro for messages with 1 parameters.
---------------------------------------------------------------------------*/    
#define DATA_PPP_MSG1_GEN(xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS_PPP, DATA_PPP_GENERAL, xx_fmt, xx_arg1)

#define DATA_PPP_MSG1_DBG(xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS_PPP, DATA_PPP_DEBUG, xx_fmt, xx_arg1)
  
#define DATA_PPP_MSG1_TRC(xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS_PPP, DATA_PPP_TRACE, xx_fmt, xx_arg1)

/*---------------------------------------------------------------------------
  Macro for messages with 2 parameters.
---------------------------------------------------------------------------*/
#define DATA_PPP_MSG2_GEN(xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS_PPP, DATA_PPP_GENERAL, xx_fmt, xx_arg1, xx_arg2)

#define DATA_PPP_MSG2_DBG(xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS_PPP, DATA_PPP_DEBUG, xx_fmt, xx_arg1, xx_arg2)

#define DATA_PPP_MSG2_TRC(xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS_PPP, DATA_PPP_TRACE, xx_fmt, xx_arg1, xx_arg2)

/*---------------------------------------------------------------------------
  Macro for messages with 3 parameters.
---------------------------------------------------------------------------*/
#define DATA_PPP_MSG3_GEN(xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS_PPP, DATA_PPP_GENERAL, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

#define DATA_PPP_MSG3_DBG(xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS_PPP, DATA_PPP_DEBUG, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

#define DATA_PPP_MSG3_TRC(xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS_PPP, DATA_PPP_TRACE, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

/*===========================================================================
  IP related macros
===========================================================================*/



/*---------------------------------------------------------------------------
  Macro for messages with no parameters.
---------------------------------------------------------------------------*/
#define DATA_TCPIP_MSG0(xx_ss_mask, xx_fmt) \
  MSG(MSG_SSID_DS_TCPIP, xx_ss_mask, xx_fmt)  

/*---------------------------------------------------------------------------
  Macro for messages with 1 parameters.
---------------------------------------------------------------------------*/    
#define DATA_TCPIP_MSG1(xx_ss_mask, xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS_TCPIP, xx_ss_mask, xx_fmt, xx_arg1)

/*---------------------------------------------------------------------------
  Macro for messages with 2 parameters.
---------------------------------------------------------------------------*/
#define DATA_TCPIP_MSG2(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS_TCPIP, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2)

/*---------------------------------------------------------------------------
  Macro for messages with 3 parameters.
---------------------------------------------------------------------------*/
#define DATA_TCPIP_MSG3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS_TCPIP, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

/*---------------------------------------------------------------------------
  This is the macro for messages with 4 parameters.
---------------------------------------------------------------------------*/
#define DATA_TCPIP_MSG4(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4) \
  MSG_4(MSG_SSID_DS_TCPIP, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4)

/*---------------------------------------------------------------------------
  This is the macro for messages with 5 parameters.
---------------------------------------------------------------------------*/
#define DATA_TCPIP_MSG5(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                                               xx_arg5) \
  MSG_5(MSG_SSID_DS_TCPIP, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5)

/*---------------------------------------------------------------------------
  This is the macro for messages with 6 parameters 
---------------------------------------------------------------------------*/
#define DATA_TCPIP_MSG6(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                                          xx_arg5, xx_arg6) \
  MSG_6(MSG_SSID_DS_TCPIP, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6)

/*---------------------------------------------------------------------------
  This is the macro for messages with 7 parameters 
---------------------------------------------------------------------------*/
#define DATA_TCPIP_MSG7(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                                 xx_arg5, xx_arg6, xx_arg7) \
  MSG_7(MSG_SSID_DS_TCPIP, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6, xx_arg7)

/*---------------------------------------------------------------------------
  This is the macro for messages with 8 parameters 
---------------------------------------------------------------------------*/
#define DATA_TCPIP_MSG8(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                        xx_arg5, xx_arg6, xx_arg7, xx_arg8) \
  MSG_8(MSG_SSID_DS_TCPIP, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6, xx_arg7, xx_arg8)

/*---------------------------------------------------------------------------
  This is the macro for messages with 9 parameters 
---------------------------------------------------------------------------*/
#define DATA_TCPIP_MSG9(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                               xx_arg5, xx_arg6, xx_arg7, xx_arg8, xx_arg9) \
  MSG_9(MSG_SSID_DS_TCPIP, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6, xx_arg7, xx_arg8, xx_arg9)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 1 parameters
---------------------------------------------------------------------------*/
#define DATA_TCPIP_MSG_SPRINTF_1(xx_ss_mask, xx_fmt, xx_arg1) \
  MSG_SPRINTF_1(MSG_SSID_DS_TCPIP, xx_ss_mask, xx_fmt, xx_arg1)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 2 parameters
---------------------------------------------------------------------------*/
#define DATA_TCPIP_MSG_SPRINTF_2(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2) \
  MSG_SPRINTF_2(MSG_SSID_DS_TCPIP, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 3 parameters
---------------------------------------------------------------------------*/
#define DATA_TCPIP_MSG_SPRINTF_3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_SPRINTF_3(MSG_SSID_DS_TCPIP, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 3 parameters. This Macro is 
  used when xx_fmt is passed at runtime.
---------------------------------------------------------------------------*/
#define DATA_TCPIP_MSG_SPRINTF_FMT_VAR_3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_SPRINTF_FMT_VAR_3(MSG_SSID_DS_TCPIP, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3)
  
/*===========================================================================
  TCPIP SS related convenience macros. These will substitute the SSID as
  well as the desired sub-group ID.
===========================================================================*/

/*---------------------------------------------------------------------------
  Macro for messages with no parameters.
---------------------------------------------------------------------------*/
#define DATA_TCPIP_MSG0_GEN(xx_fmt) \
  MSG(MSG_SSID_DS_TCPIP, DATA_TCPIP_GENERAL, xx_fmt)
  
#define DATA_TCPIP_MSG0_DBG(xx_fmt) \
  MSG(MSG_SSID_DS_TCPIP, DATA_TCPIP_DEBUG, xx_fmt)
  
#define DATA_TCPIP_MSG0_TRC(xx_fmt) \
  MSG(MSG_SSID_DS_TCPIP, DATA_TCPIP_TRACE, xx_fmt)

/*---------------------------------------------------------------------------
  Macro for messages with 1 parameters.
---------------------------------------------------------------------------*/    
#define DATA_TCPIP_MSG1_GEN(xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS_TCPIP, DATA_TCPIP_GENERAL, xx_fmt, xx_arg1)

#define DATA_TCPIP_MSG1_DBG(xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS_TCPIP, DATA_TCPIP_DEBUG, xx_fmt, xx_arg1)
  
#define DATA_TCPIP_MSG1_TRC(xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS_TCPIP, DATA_TCPIP_TRACE, xx_fmt, xx_arg1)

/*---------------------------------------------------------------------------
  Macro for messages with 2 parameters.
---------------------------------------------------------------------------*/
#define DATA_TCPIP_MSG2_GEN(xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS_TCPIP, DATA_TCPIP_GENERAL, xx_fmt, xx_arg1, xx_arg2)

#define DATA_TCPIP_MSG2_DBG(xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS_TCPIP, DATA_TCPIP_DEBUG, xx_fmt, xx_arg1, xx_arg2)

#define DATA_TCPIP_MSG2_TRC(xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS_TCPIP, DATA_TCPIP_TRACE, xx_fmt, xx_arg1, xx_arg2)

/*---------------------------------------------------------------------------
  Macro for messages with 3 parameters.
---------------------------------------------------------------------------*/
#define DATA_TCPIP_MSG3_GEN(xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS_TCPIP, DATA_TCPIP_GENERAL, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

#define DATA_TCPIP_MSG3_DBG(xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS_TCPIP, DATA_TCPIP_DEBUG, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

#define DATA_TCPIP_MSG3_TRC(xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS_TCPIP, DATA_TCPIP_TRACE, xx_fmt, xx_arg1, xx_arg2, xx_arg3)
  
/*===========================================================================
  DSIS707  SS related macros
===========================================================================*/



/*---------------------------------------------------------------------------
  Macro for messages with no parameters.
---------------------------------------------------------------------------*/
#define DATA_IS707_MSG0(xx_ss_mask, xx_fmt) \
  MSG(MSG_SSID_DS_IS707, xx_ss_mask, xx_fmt)  

/*---------------------------------------------------------------------------
  Macro for messages with 1 parameters.
---------------------------------------------------------------------------*/    
#define DATA_IS707_MSG1(xx_ss_mask, xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS_IS707, xx_ss_mask, xx_fmt, xx_arg1)

/*---------------------------------------------------------------------------
  Macro for messages with 2 parameters.
---------------------------------------------------------------------------*/
#define DATA_IS707_MSG2(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS_IS707, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2)

/*---------------------------------------------------------------------------
  Macro for messages with 3 parameters.
---------------------------------------------------------------------------*/
#define DATA_IS707_MSG3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS_IS707, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

/*---------------------------------------------------------------------------
  This is the macro for messages with 4 parameters.
---------------------------------------------------------------------------*/
#define DATA_IS707_MSG4(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4) \
  MSG_4(MSG_SSID_DS_IS707, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4)

/*---------------------------------------------------------------------------
  This is the macro for messages with 5 parameters.
---------------------------------------------------------------------------*/
#define DATA_IS707_MSG5(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                                               xx_arg5) \
  MSG_5(MSG_SSID_DS_IS707, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5)

/*---------------------------------------------------------------------------
  This is the macro for messages with 6 parameters 
---------------------------------------------------------------------------*/
#define DATA_IS707_MSG6(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                                          xx_arg5, xx_arg6) \
  MSG_6(MSG_SSID_DS_IS707, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6)

/*---------------------------------------------------------------------------
  This is the macro for messages with 7 parameters 
---------------------------------------------------------------------------*/
#define DATA_IS707_MSG7(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                                 xx_arg5, xx_arg6, xx_arg7) \
  MSG_7(MSG_SSID_DS_IS707, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6, xx_arg7)

/*---------------------------------------------------------------------------
  This is the macro for messages with 8 parameters 
---------------------------------------------------------------------------*/
#define DATA_IS707_MSG8(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                        xx_arg5, xx_arg6, xx_arg7, xx_arg8) \
  MSG_8(MSG_SSID_DS_IS707, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6, xx_arg7, xx_arg8)

/*---------------------------------------------------------------------------
  This is the macro for messages with 9 parameters 
---------------------------------------------------------------------------*/
#define DATA_IS707_MSG9(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                               xx_arg5, xx_arg6, xx_arg7, xx_arg8, xx_arg9) \
  MSG_9(MSG_SSID_DS_IS707, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6, xx_arg7, xx_arg8, xx_arg9)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 1 parameters
---------------------------------------------------------------------------*/
#define DATA_IS707_MSG_SPRINTF_1(xx_ss_mask, xx_fmt, xx_arg1) \
  MSG_SPRINTF_1(MSG_SSID_DS_IS707, xx_ss_mask, xx_fmt, xx_arg1)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 2 parameters
---------------------------------------------------------------------------*/
#define DATA_IS707_MSG_SPRINTF_2(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2) \
  MSG_SPRINTF_2(MSG_SSID_DS_IS707, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 3 parameters
---------------------------------------------------------------------------*/
#define DATA_IS707_MSG_SPRINTF_3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_SPRINTF_3(MSG_SSID_DS_IS707, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 3 parameters. This Macro is 
  used when xx_fmt is passed at runtime.
---------------------------------------------------------------------------*/
#define DATA_IS707_MSG_SPRINTF_FMT_VAR_3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_SPRINTF_FMT_VAR_3(MSG_SSID_DS_IS707, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

/*===========================================================================
  IS707 SS related convenience macros. These will substitute the SSID as
  well as the desired sub-group ID.
===========================================================================*/

/*---------------------------------------------------------------------------
  Macro for messages with no parameters.
---------------------------------------------------------------------------*/
#define DATA_IS707_MSG0_GEN(xx_fmt) \
  MSG(MSG_SSID_DS_IS707, DATA_IS707_GENERAL, xx_fmt)
  
#define DATA_IS707_MSG0_DBG(xx_fmt) \
  MSG(MSG_SSID_DS_IS707, DATA_IS707_DEBUG, xx_fmt)
  
#define DATA_IS707_MSG0_TRC(xx_fmt) \
  MSG(MSG_SSID_DS_IS707, DATA_IS707_TRACE, xx_fmt)

/*---------------------------------------------------------------------------
  Macro for messages with 1 parameters.
---------------------------------------------------------------------------*/    
#define DATA_IS707_MSG1_GEN(xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS_IS707, DATA_IS707_GENERAL, xx_fmt, xx_arg1)

#define DATA_IS707_MSG1_DBG(xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS_IS707, DATA_IS707_DEBUG, xx_fmt, xx_arg1)
  
#define DATA_IS707_MSG1_TRC(xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS_IS707, DATA_IS707_TRACE, xx_fmt, xx_arg1)

/*---------------------------------------------------------------------------
  Macro for messages with 2 parameters.
---------------------------------------------------------------------------*/
#define DATA_IS707_MSG2_GEN(xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS_IS707, DATA_IS707_GENERAL, xx_fmt, xx_arg1, xx_arg2)

#define DATA_IS707_MSG2_DBG(xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS_IS707, DATA_IS707_DEBUG, xx_fmt, xx_arg1, xx_arg2)

#define DATA_IS707_MSG2_TRC(xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS_IS707, DATA_IS707_TRACE, xx_fmt, xx_arg1, xx_arg2)

/*---------------------------------------------------------------------------
  Macro for messages with 3 parameters.
---------------------------------------------------------------------------*/
#define DATA_IS707_MSG3_GEN(xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS_IS707, DATA_IS707_GENERAL, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

#define DATA_IS707_MSG3_DBG(xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS_IS707, DATA_IS707_DEBUG, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

#define DATA_IS707_MSG3_TRC(xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS_IS707, DATA_IS707_TRACE, xx_fmt, xx_arg1, xx_arg2, xx_arg3)
  
/*===========================================================================
  DS 3GMGR SS related macros
===========================================================================*/


/*---------------------------------------------------------------------------
  Macro for messages with no parameters.
---------------------------------------------------------------------------*/
#define DATA_3GMGR_MSG0(xx_ss_mask, xx_fmt) \
  MSG(MSG_SSID_DS_3GMGR, xx_ss_mask, xx_fmt)  

/*---------------------------------------------------------------------------
  Macro for messages with 1 parameters.
---------------------------------------------------------------------------*/    
#define DATA_3GMGR_MSG1(xx_ss_mask, xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS_3GMGR, xx_ss_mask, xx_fmt, xx_arg1)

/*---------------------------------------------------------------------------
  Macro for messages with 2 parameters.
---------------------------------------------------------------------------*/
#define DATA_3GMGR_MSG2(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS_3GMGR, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2)

/*---------------------------------------------------------------------------
  Macro for messages with 3 parameters.
---------------------------------------------------------------------------*/
#define DATA_3GMGR_MSG3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS_3GMGR, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

/*---------------------------------------------------------------------------
  This is the macro for messages with 4 parameters.
---------------------------------------------------------------------------*/
#define DATA_3GMGR_MSG4(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4) \
  MSG_4(MSG_SSID_DS_3GMGR, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4)

/*---------------------------------------------------------------------------
  This is the macro for messages with 5 parameters.
---------------------------------------------------------------------------*/
#define DATA_3GMGR_MSG5(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                                               xx_arg5) \
  MSG_5(MSG_SSID_DS_3GMGR, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5)

/*---------------------------------------------------------------------------
  This is the macro for messages with 6 parameters 
---------------------------------------------------------------------------*/
#define DATA_3GMGR_MSG6(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                                          xx_arg5, xx_arg6) \
  MSG_6(MSG_SSID_DS_3GMGR, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6)

/*---------------------------------------------------------------------------
  This is the macro for messages with 7 parameters 
---------------------------------------------------------------------------*/
#define DATA_3GMGR_MSG7(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                                 xx_arg5, xx_arg6, xx_arg7) \
  MSG_7(MSG_SSID_DS_3GMGR, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6, xx_arg7)

/*---------------------------------------------------------------------------
  This is the macro for messages with 8 parameters 
---------------------------------------------------------------------------*/
#define DATA_3GMGR_MSG8(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                        xx_arg5, xx_arg6, xx_arg7, xx_arg8) \
  MSG_8(MSG_SSID_DS_3GMGR, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6, xx_arg7, xx_arg8)

/*---------------------------------------------------------------------------
  This is the macro for messages with 9 parameters 
---------------------------------------------------------------------------*/
#define DATA_3GMGR_MSG9(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                               xx_arg5, xx_arg6, xx_arg7, xx_arg8, xx_arg9) \
  MSG_9(MSG_SSID_DS_3GMGR, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6, xx_arg7, xx_arg8, xx_arg9)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 1 parameters
---------------------------------------------------------------------------*/
#define DATA_3GMGR_MSG_SPRINTF_1(xx_ss_mask, xx_fmt, xx_arg1) \
  MSG_SPRINTF_1(MSG_SSID_DS_3GMGR, xx_ss_mask, xx_fmt, xx_arg1)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 2 parameters
---------------------------------------------------------------------------*/
#define DATA_3GMGR_MSG_SPRINTF_2(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2) \
  MSG_SPRINTF_2(MSG_SSID_DS_3GMGR, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 3 parameters
---------------------------------------------------------------------------*/
#define DATA_3GMGR_MSG_SPRINTF_3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_SPRINTF_3(MSG_SSID_DS_3GMGR, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 3 parameters. This Macro is 
  used when xx_fmt is passed at runtime.
---------------------------------------------------------------------------*/
#define DATA_3GMGR_MSG_SPRINTF_FMT_VAR_3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_SPRINTF_FMT_VAR_3(MSG_SSID_DS_3GMGR, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

/*===========================================================================
  3GMGR SS related convenience macros. These will substitute the SSID as
  well as the desired sub-group ID.
===========================================================================*/

/*---------------------------------------------------------------------------
  Macro for messages with no parameters.
---------------------------------------------------------------------------*/
#define DATA_3GMGR_MSG0_GEN(xx_fmt) \
  MSG(MSG_SSID_DS_3GMGR, DATA_3GMGR_GENERAL, xx_fmt)
  
#define DATA_3GMGR_MSG0_DBG(xx_fmt) \
  MSG(MSG_SSID_DS_3GMGR, DATA_3GMGR_DEBUG, xx_fmt)
  
#define DATA_3GMGR_MSG0_TRC(xx_fmt) \
  MSG(MSG_SSID_DS_3GMGR, DATA_3GMGR_TRACE, xx_fmt)

/*---------------------------------------------------------------------------
  Macro for messages with 1 parameters.
---------------------------------------------------------------------------*/    
#define DATA_3GMGR_MSG1_GEN(xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS_3GMGR, DATA_3GMGR_GENERAL, xx_fmt, xx_arg1)

#define DATA_3GMGR_MSG1_DBG(xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS_3GMGR, DATA_3GMGR_DEBUG, xx_fmt, xx_arg1)
  
#define DATA_3GMGR_MSG1_TRC(xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS_3GMGR, DATA_3GMGR_TRACE, xx_fmt, xx_arg1)

/*---------------------------------------------------------------------------
  Macro for messages with 2 parameters.
---------------------------------------------------------------------------*/
#define DATA_3GMGR_MSG2_GEN(xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS_3GMGR, DATA_3GMGR_GENERAL, xx_fmt, xx_arg1, xx_arg2)

#define DATA_3GMGR_MSG2_DBG(xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS_3GMGR, DATA_3GMGR_DEBUG, xx_fmt, xx_arg1, xx_arg2)

#define DATA_3GMGR_MSG2_TRC(xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS_3GMGR, DATA_3GMGR_TRACE, xx_fmt, xx_arg1, xx_arg2)

/*---------------------------------------------------------------------------
  Macro for messages with 3 parameters.
---------------------------------------------------------------------------*/
#define DATA_3GMGR_MSG3_GEN(xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS_3GMGR, DATA_3GMGR_GENERAL, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

#define DATA_3GMGR_MSG3_DBG(xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS_3GMGR, DATA_3GMGR_DEBUG, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

#define DATA_3GMGR_MSG3_TRC(xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS_3GMGR, DATA_3GMGR_TRACE, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

/*===========================================================================
  PS SS related macros
===========================================================================*/

/*---------------------------------------------------------------------------
  Macro for messages with no parameters.
---------------------------------------------------------------------------*/
#define DATA_PS_MSG0(xx_ss_mask, xx_fmt) \
  MSG(MSG_SSID_DS_PS, xx_ss_mask, xx_fmt)  

/*---------------------------------------------------------------------------
  Macro for messages with 1 parameters.
---------------------------------------------------------------------------*/    
#define DATA_PS_MSG1(xx_ss_mask, xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS_PS, xx_ss_mask, xx_fmt, xx_arg1)

/*---------------------------------------------------------------------------
  Macro for messages with 2 parameters.
---------------------------------------------------------------------------*/
#define DATA_PS_MSG2(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS_PS, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2)

/*---------------------------------------------------------------------------
  Macro for messages with 3 parameters.
---------------------------------------------------------------------------*/
#define DATA_PS_MSG3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS_PS, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

/*---------------------------------------------------------------------------
  This is the macro for messages with 4 parameters.
---------------------------------------------------------------------------*/
#define DATA_PS_MSG4(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4) \
  MSG_4(MSG_SSID_DS_PS, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4)

/*---------------------------------------------------------------------------
  This is the macro for messages with 5 parameters.
---------------------------------------------------------------------------*/
#define DATA_PS_MSG5(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                                               xx_arg5) \
  MSG_5(MSG_SSID_DS_PS, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5)

/*---------------------------------------------------------------------------
  This is the macro for messages with 6 parameters 
---------------------------------------------------------------------------*/
#define DATA_PS_MSG6(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                                          xx_arg5, xx_arg6) \
  MSG_6(MSG_SSID_DS_PS, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6)

/*---------------------------------------------------------------------------
  This is the macro for messages with 7 parameters 
---------------------------------------------------------------------------*/
#define DATA_PS_MSG7(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                                 xx_arg5, xx_arg6, xx_arg7) \
  MSG_7(MSG_SSID_DS_PS, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6, xx_arg7)

/*---------------------------------------------------------------------------
  This is the macro for messages with 8 parameters 
---------------------------------------------------------------------------*/
#define DATA_PS_MSG8(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                        xx_arg5, xx_arg6, xx_arg7, xx_arg8) \
  MSG_8(MSG_SSID_DS_PS, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6, xx_arg7, xx_arg8)

/*---------------------------------------------------------------------------
  This is the macro for messages with 9 parameters 
---------------------------------------------------------------------------*/
#define DATA_PS_MSG9(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                               xx_arg5, xx_arg6, xx_arg7, xx_arg8, xx_arg9) \
  MSG_9(MSG_SSID_DS_PS, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6, xx_arg7, xx_arg8, xx_arg9)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 1 parameters
---------------------------------------------------------------------------*/
#define DATA_PS_MSG_SPRINTF_1(xx_ss_mask, xx_fmt, xx_arg1) \
  MSG_SPRINTF_1(MSG_SSID_DS_PS, xx_ss_mask, xx_fmt, xx_arg1)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 2 parameters
---------------------------------------------------------------------------*/
#define DATA_PS_MSG_SPRINTF_2(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2) \
  MSG_SPRINTF_2(MSG_SSID_DS_PS, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 3 parameters
---------------------------------------------------------------------------*/
#define DATA_PS_MSG_SPRINTF_3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_SPRINTF_3(MSG_SSID_DS_PS, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 3 parameters. This Macro is 
  used when xx_fmt is passed at runtime.
---------------------------------------------------------------------------*/
#define DATA_PS_MSG_SPRINTF_FMT_VAR_3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_SPRINTF_FMT_VAR_3(MSG_SSID_DS_PS, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3)
  
/*===========================================================================
  PS SS related convenience macros. These will substitute the SSID as
  well as the desired sub-group ID.
===========================================================================*/

/*---------------------------------------------------------------------------
  Macro for messages with no parameters.
---------------------------------------------------------------------------*/
#define DATA_PS_MSG0_GEN(xx_fmt) \
  MSG(MSG_SSID_DS_PS, DATA_PS_GENERAL, xx_fmt)
  
#define DATA_PS_MSG0_DBG(xx_fmt) \
  MSG(MSG_SSID_DS_PS, DATA_PS_DEBUG, xx_fmt)
  
#define DATA_PS_MSG0_TRC(xx_fmt) \
  MSG(MSG_SSID_DS_PS, DATA_PS_TRACE, xx_fmt)

/*---------------------------------------------------------------------------
  Macro for messages with 1 parameters.
---------------------------------------------------------------------------*/    
#define DATA_PS_MSG1_GEN(xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS_PS, DATA_PS_GENERAL, xx_fmt, xx_arg1)

#define DATA_PS_MSG1_DBG(xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS_PS, DATA_PS_DEBUG, xx_fmt, xx_arg1)
  
#define DATA_PS_MSG1_TRC(xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS_PS, DATA_PS_TRACE, xx_fmt, xx_arg1)

/*---------------------------------------------------------------------------
  Macro for messages with 2 parameters.
---------------------------------------------------------------------------*/
#define DATA_PS_MSG2_GEN(xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS_PS, DATA_PS_GENERAL, xx_fmt, xx_arg1, xx_arg2)

#define DATA_PS_MSG2_DBG(xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS_PS, DATA_PS_DEBUG, xx_fmt, xx_arg1, xx_arg2)

#define DATA_PS_MSG2_TRC(xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS_PS, DATA_PS_TRACE, xx_fmt, xx_arg1, xx_arg2)

/*---------------------------------------------------------------------------
  Macro for messages with 3 parameters.
---------------------------------------------------------------------------*/
#define DATA_PS_MSG3_GEN(xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS_PS, DATA_PS_GENERAL, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

#define DATA_PS_MSG3_DBG(xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS_PS, DATA_PS_DEBUG, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

#define DATA_PS_MSG3_TRC(xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS_PS, DATA_PS_TRACE, xx_fmt, xx_arg1, xx_arg2, xx_arg3)
  
/*===========================================================================
  MIP SS related macros
===========================================================================*/


/*---------------------------------------------------------------------------
  Macro for messages with no parameters.
---------------------------------------------------------------------------*/
#define DATA_MIP_MSG0(xx_ss_mask, xx_fmt) \
  MSG(MSG_SSID_DS_MIP, xx_ss_mask, xx_fmt)  

/*---------------------------------------------------------------------------
  Macro for messages with 1 parameters.
---------------------------------------------------------------------------*/    
#define DATA_MIP_MSG1(xx_ss_mask, xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS_MIP, xx_ss_mask, xx_fmt, xx_arg1)

/*---------------------------------------------------------------------------
  Macro for messages with 2 parameters.
---------------------------------------------------------------------------*/
#define DATA_MIP_MSG2(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS_MIP, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2)

/*---------------------------------------------------------------------------
  Macro for messages with 3 parameters.
---------------------------------------------------------------------------*/
#define DATA_MIP_MSG3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS_MIP, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

/*---------------------------------------------------------------------------
  This is the macro for messages with 4 parameters.
---------------------------------------------------------------------------*/
#define DATA_MIP_MSG4(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4) \
  MSG_4(MSG_SSID_DS_MIP, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4)

/*---------------------------------------------------------------------------
  This is the macro for messages with 5 parameters.
---------------------------------------------------------------------------*/
#define DATA_MIP_MSG5(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                                               xx_arg5) \
  MSG_5(MSG_SSID_DS_MIP, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5)

/*---------------------------------------------------------------------------
  This is the macro for messages with 6 parameters 
---------------------------------------------------------------------------*/
#define DATA_MIP_MSG6(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                                          xx_arg5, xx_arg6) \
  MSG_6(MSG_SSID_DS_MIP, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6)

/*---------------------------------------------------------------------------
  This is the macro for messages with 7 parameters 
---------------------------------------------------------------------------*/
#define DATA_MIP_MSG7(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                                 xx_arg5, xx_arg6, xx_arg7) \
  MSG_7(MSG_SSID_DS_MIP, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6, xx_arg7)

/*---------------------------------------------------------------------------
  This is the macro for messages with 8 parameters 
---------------------------------------------------------------------------*/
#define DATA_MIP_MSG8(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                        xx_arg5, xx_arg6, xx_arg7, xx_arg8) \
  MSG_8(MSG_SSID_DS_MIP, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6, xx_arg7, xx_arg8)

/*---------------------------------------------------------------------------
  This is the macro for messages with 9 parameters 
---------------------------------------------------------------------------*/
#define DATA_MIP_MSG9(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                               xx_arg5, xx_arg6, xx_arg7, xx_arg8, xx_arg9) \
  MSG_9(MSG_SSID_DS_MIP, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6, xx_arg7, xx_arg8, xx_arg9)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 1 parameters
---------------------------------------------------------------------------*/
#define DATA_MIP_MSG_SPRINTF_1(xx_ss_mask, xx_fmt, xx_arg1) \
  MSG_SPRINTF_1(MSG_SSID_DS_MIP, xx_ss_mask, xx_fmt, xx_arg1)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 2 parameters
---------------------------------------------------------------------------*/
#define DATA_MIP_MSG_SPRINTF_2(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2) \
  MSG_SPRINTF_2(MSG_SSID_DS_MIP, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 3 parameters
---------------------------------------------------------------------------*/
#define DATA_MIP_MSG_SPRINTF_3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_SPRINTF_3(MSG_SSID_DS_MIP, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 3 parameters. This Macro is 
  used when xx_fmt is passed at runtime.
---------------------------------------------------------------------------*/
#define DATA_MIP_MSG_SPRINTF_FMT_VAR_3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_SPRINTF_FMT_VAR_3(MSG_SSID_DS_MIP, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

/*===========================================================================
  MIP SS related convenience macros. These will substitute the SSID as
  well as the desired sub-group ID.
===========================================================================*/

/*---------------------------------------------------------------------------
  Macro for messages with no parameters.
---------------------------------------------------------------------------*/
#define DATA_MIP_MSG0_GEN(xx_fmt) \
  MSG(MSG_SSID_DS_MIP, DATA_MIP_GENERAL, xx_fmt)
  
#define DATA_MIP_MSG0_DBG(xx_fmt) \
  MSG(MSG_SSID_DS_MIP, DATA_MIP_DEBUG, xx_fmt)
  
#define DATA_MIP_MSG0_TRC(xx_fmt) \
  MSG(MSG_SSID_DS_MIP, DATA_MIP_TRACE, xx_fmt)

/*---------------------------------------------------------------------------
  Macro for messages with 1 parameters.
---------------------------------------------------------------------------*/    
#define DATA_MIP_MSG1_GEN(xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS_MIP, DATA_MIP_GENERAL, xx_fmt, xx_arg1)

#define DATA_MIP_MSG1_DBG(xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS_MIP, DATA_MIP_DEBUG, xx_fmt, xx_arg1)
  
#define DATA_MIP_MSG1_TRC(xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS_MIP, DATA_MIP_TRACE, xx_fmt, xx_arg1)

/*---------------------------------------------------------------------------
  Macro for messages with 2 parameters.
---------------------------------------------------------------------------*/
#define DATA_MIP_MSG2_GEN(xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS_MIP, DATA_MIP_GENERAL, xx_fmt, xx_arg1, xx_arg2)

#define DATA_MIP_MSG2_DBG(xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS_MIP, DATA_MIP_DEBUG, xx_fmt, xx_arg1, xx_arg2)

#define DATA_MIP_MSG2_TRC(xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS_MIP, DATA_MIP_TRACE, xx_fmt, xx_arg1, xx_arg2)

/*---------------------------------------------------------------------------
  Macro for messages with 3 parameters.
---------------------------------------------------------------------------*/
#define DATA_MIP_MSG3_GEN(xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS_MIP, DATA_MIP_GENERAL, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

#define DATA_MIP_MSG3_DBG(xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS_MIP, DATA_MIP_DEBUG, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

#define DATA_MIP_MSG3_TRC(xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS_MIP, DATA_MIP_TRACE, xx_fmt, xx_arg1, xx_arg2, xx_arg3)
        
/*===========================================================================
  UMTS SS related macros
===========================================================================*/

/*---------------------------------------------------------------------------
  Macro for messages with no parameters.
---------------------------------------------------------------------------*/
#define DATA_UMTS_MSG0(xx_ss_mask, xx_fmt) \
  MSG(MSG_SSID_DS_UMTS, xx_ss_mask, xx_fmt)  

/*---------------------------------------------------------------------------
  Macro for messages with 1 parameters.
---------------------------------------------------------------------------*/    
#define DATA_UMTS_MSG1(xx_ss_mask, xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS_UMTS, xx_ss_mask, xx_fmt, xx_arg1)

/*---------------------------------------------------------------------------
  Macro for messages with 2 parameters.
---------------------------------------------------------------------------*/
#define DATA_UMTS_MSG2(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS_UMTS, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2)

/*---------------------------------------------------------------------------
  Macro for messages with 3 parameters.
---------------------------------------------------------------------------*/
#define DATA_UMTS_MSG3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS_UMTS, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

/*---------------------------------------------------------------------------
  This is the macro for messages with 4 parameters.
---------------------------------------------------------------------------*/
#define DATA_UMTS_MSG4(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4) \
  MSG_4(MSG_SSID_DS_UMTS, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4)

/*---------------------------------------------------------------------------
  This is the macro for messages with 5 parameters.
---------------------------------------------------------------------------*/
#define DATA_UMTS_MSG5(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                                               xx_arg5) \
  MSG_5(MSG_SSID_DS_UMTS, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5)

/*---------------------------------------------------------------------------
  This is the macro for messages with 6 parameters 
---------------------------------------------------------------------------*/
#define DATA_UMTS_MSG6(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                                          xx_arg5, xx_arg6) \
  MSG_6(MSG_SSID_DS_UMTS, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6)

/*---------------------------------------------------------------------------
  This is the macro for messages with 7 parameters 
---------------------------------------------------------------------------*/
#define DATA_UMTS_MSG7(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                                 xx_arg5, xx_arg6, xx_arg7) \
  MSG_7(MSG_SSID_DS_UMTS, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6, xx_arg7)

/*---------------------------------------------------------------------------
  This is the macro for messages with 8 parameters 
---------------------------------------------------------------------------*/
#define DATA_UMTS_MSG8(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                        xx_arg5, xx_arg6, xx_arg7, xx_arg8) \
  MSG_8(MSG_SSID_DS_UMTS, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6, xx_arg7, xx_arg8)

/*---------------------------------------------------------------------------
  This is the macro for messages with 9 parameters 
---------------------------------------------------------------------------*/
#define DATA_UMTS_MSG9(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                               xx_arg5, xx_arg6, xx_arg7, xx_arg8, xx_arg9) \
  MSG_9(MSG_SSID_DS_UMTS, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6, xx_arg7, xx_arg8, xx_arg9)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 1 parameters
---------------------------------------------------------------------------*/
#define DATA_UMTS_MSG_SPRINTF_1(xx_ss_mask, xx_fmt, xx_arg1) \
  MSG_SPRINTF_1(MSG_SSID_DS_UMTS, xx_ss_mask, xx_fmt, xx_arg1)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 2 parameters
---------------------------------------------------------------------------*/
#define DATA_UMTS_MSG_SPRINTF_2(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2) \
  MSG_SPRINTF_2(MSG_SSID_DS_UMTS, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 3 parameters
---------------------------------------------------------------------------*/
#define DATA_UMTS_MSG_SPRINTF_3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_SPRINTF_3(MSG_SSID_DS_UMTS, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 3 parameters. This Macro is 
  used when xx_fmt is passed at runtime.
---------------------------------------------------------------------------*/
#define DATA_UMTS_MSG_SPRINTF_FMT_VAR_3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_SPRINTF_FMT_VAR_3(MSG_SSID_DS_UMTS, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3)
  
/*===========================================================================
  UMTS SS related convenience macros. These will substitute the SSID as
  well as the desired sub-group ID.
===========================================================================*/

/*---------------------------------------------------------------------------
  Macro for messages with no parameters.
---------------------------------------------------------------------------*/
#define DATA_UMTS_MSG0_GEN(xx_fmt) \
  MSG(MSG_SSID_DS_UMTS, DATA_UMTS_GENERAL, xx_fmt)
  
#define DATA_UMTS_MSG0_DBG(xx_fmt) \
  MSG(MSG_SSID_DS_UMTS, DATA_UMTS_DEBUG, xx_fmt)
  
#define DATA_UMTS_MSG0_TRC(xx_fmt) \
  MSG(MSG_SSID_DS_UMTS, DATA_UMTS_TRACE, xx_fmt)

/*---------------------------------------------------------------------------
  Macro for messages with 1 parameters.
---------------------------------------------------------------------------*/    
#define DATA_UMTS_MSG1_GEN(xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS_UMTS, DATA_UMTS_GENERAL, xx_fmt, xx_arg1)

#define DATA_UMTS_MSG1_DBG(xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS_UMTS, DATA_UMTS_DEBUG, xx_fmt, xx_arg1)
  
#define DATA_UMTS_MSG1_TRC(xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS_UMTS, DATA_UMTS_TRACE, xx_fmt, xx_arg1)

/*---------------------------------------------------------------------------
  Macro for messages with 2 parameters.
---------------------------------------------------------------------------*/
#define DATA_UMTS_MSG2_GEN(xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS_UMTS, DATA_UMTS_GENERAL, xx_fmt, xx_arg1, xx_arg2)

#define DATA_UMTS_MSG2_DBG(xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS_UMTS, DATA_UMTS_DEBUG, xx_fmt, xx_arg1, xx_arg2)

#define DATA_UMTS_MSG2_TRC(xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS_UMTS, DATA_UMTS_TRACE, xx_fmt, xx_arg1, xx_arg2)

/*---------------------------------------------------------------------------
  Macro for messages with 3 parameters.
---------------------------------------------------------------------------*/
#define DATA_UMTS_MSG3_GEN(xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS_UMTS, DATA_UMTS_GENERAL, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

#define DATA_UMTS_MSG3_DBG(xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS_UMTS, DATA_UMTS_DEBUG, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

#define DATA_UMTS_MSG3_TRC(xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS_UMTS, DATA_UMTS_TRACE, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

/*===========================================================================
  GPRS SS related macros
===========================================================================*/


/*---------------------------------------------------------------------------
  Macro for messages with no parameters.
---------------------------------------------------------------------------*/
#define DATA_GPRS_MSG0(xx_ss_mask, xx_fmt) \
  MSG(MSG_SSID_DS_GPRS, xx_ss_mask, xx_fmt)  

/*---------------------------------------------------------------------------
  Macro for messages with 1 parameters.
---------------------------------------------------------------------------*/    
#define DATA_GPRS_MSG1(xx_ss_mask, xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS_GPRS, xx_ss_mask, xx_fmt, xx_arg1)

/*---------------------------------------------------------------------------
  Macro for messages with 2 parameters.
---------------------------------------------------------------------------*/
#define DATA_GPRS_MSG2(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS_GPRS, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2)

/*---------------------------------------------------------------------------
  Macro for messages with 3 parameters.
---------------------------------------------------------------------------*/
#define DATA_GPRS_MSG3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS_GPRS, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

/*---------------------------------------------------------------------------
  This is the macro for messages with 4 parameters.
---------------------------------------------------------------------------*/
#define DATA_GPRS_MSG4(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4) \
  MSG_4(MSG_SSID_DS_GPRS, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4)

/*---------------------------------------------------------------------------
  This is the macro for messages with 5 parameters.
---------------------------------------------------------------------------*/
#define DATA_GPRS_MSG5(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                                               xx_arg5) \
  MSG_5(MSG_SSID_DS_GPRS, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5)

/*---------------------------------------------------------------------------
  This is the macro for messages with 6 parameters 
---------------------------------------------------------------------------*/
#define DATA_GPRS_MSG6(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                                          xx_arg5, xx_arg6) \
  MSG_6(MSG_SSID_DS_GPRS, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6)

/*---------------------------------------------------------------------------
  This is the macro for messages with 7 parameters 
---------------------------------------------------------------------------*/
#define DATA_GPRS_MSG7(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                                 xx_arg5, xx_arg6, xx_arg7) \
  MSG_7(MSG_SSID_DS_GPRS, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6, xx_arg7)

/*---------------------------------------------------------------------------
  This is the macro for messages with 8 parameters 
---------------------------------------------------------------------------*/
#define DATA_GPRS_MSG8(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                        xx_arg5, xx_arg6, xx_arg7, xx_arg8) \
  MSG_8(MSG_SSID_DS_GPRS, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6, xx_arg7, xx_arg8)

/*---------------------------------------------------------------------------
  This is the macro for messages with 9 parameters 
---------------------------------------------------------------------------*/
#define DATA_GPRS_MSG9(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                               xx_arg5, xx_arg6, xx_arg7, xx_arg8, xx_arg9) \
  MSG_9(MSG_SSID_DS_GPRS, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6, xx_arg7, xx_arg8, xx_arg9)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 1 parameters
---------------------------------------------------------------------------*/
#define DATA_GPRS_MSG_SPRINTF_1(xx_ss_mask, xx_fmt, xx_arg1) \
  MSG_SPRINTF_1(MSG_SSID_DS_GPRS, xx_ss_mask, xx_fmt, xx_arg1)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 2 parameters
---------------------------------------------------------------------------*/
#define DATA_GPRS_MSG_SPRINTF_2(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2) \
  MSG_SPRINTF_2(MSG_SSID_DS_GPRS, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 3 parameters
---------------------------------------------------------------------------*/
#define DATA_GPRS_MSG_SPRINTF_3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_SPRINTF_3(MSG_SSID_DS_GPRS, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 3 parameters. This Macro is 
  used when xx_fmt is passed at runtime.
---------------------------------------------------------------------------*/
#define DATA_GPRS_MSG_SPRINTF_FMT_VAR_3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_SPRINTF_FMT_VAR_3(MSG_SSID_DS_GPRS, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3)
  
/*===========================================================================
  GPRS SS related convenience macros. These will substitute the SSID as
  well as the desired sub-group ID.
===========================================================================*/

/*---------------------------------------------------------------------------
  Macro for messages with no parameters.
---------------------------------------------------------------------------*/
#define DATA_GPRS_MSG0_GEN(xx_fmt) \
  MSG(MSG_SSID_DS_GPRS, DATA_GPRS_GENERAL, xx_fmt)
  
#define DATA_GPRS_MSG0_DBG(xx_fmt) \
  MSG(MSG_SSID_DS_GPRS, DATA_GPRS_DEBUG, xx_fmt)
  
#define DATA_GPRS_MSG0_TRC(xx_fmt) \
  MSG(MSG_SSID_DS_GPRS, DATA_GPRS_TRACE, xx_fmt)

/*---------------------------------------------------------------------------
  Macro for messages with 1 parameters.
---------------------------------------------------------------------------*/    
#define DATA_GPRS_MSG1_GEN(xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS_GPRS, DATA_GPRS_GENERAL, xx_fmt, xx_arg1)

#define DATA_GPRS_MSG1_DBG(xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS_GPRS, DATA_GPRS_DEBUG, xx_fmt, xx_arg1)
  
#define DATA_GPRS_MSG1_TRC(xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS_GPRS, DATA_GPRS_TRACE, xx_fmt, xx_arg1)

/*---------------------------------------------------------------------------
  Macro for messages with 2 parameters.
---------------------------------------------------------------------------*/
#define DATA_GPRS_MSG2_GEN(xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS_GPRS, DATA_GPRS_GENERAL, xx_fmt, xx_arg1, xx_arg2)

#define DATA_GPRS_MSG2_DBG(xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS_GPRS, DATA_GPRS_DEBUG, xx_fmt, xx_arg1, xx_arg2)

#define DATA_GPRS_MSG2_TRC(xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS_GPRS, DATA_GPRS_TRACE, xx_fmt, xx_arg1, xx_arg2)

/*---------------------------------------------------------------------------
  Macro for messages with 3 parameters.
---------------------------------------------------------------------------*/
#define DATA_GPRS_MSG3_GEN(xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS_GPRS, DATA_GPRS_GENERAL, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

#define DATA_GPRS_MSG3_DBG(xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS_GPRS, DATA_GPRS_DEBUG, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

#define DATA_GPRS_MSG3_TRC(xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS_GPRS, DATA_GPRS_TRACE, xx_fmt, xx_arg1, xx_arg2, xx_arg3)
  
/*===========================================================================
  GSM SS related macros
===========================================================================*/


/*---------------------------------------------------------------------------
  Macro for messages with no parameters.
---------------------------------------------------------------------------*/
#define DATA_GSM_MSG0(xx_ss_mask, xx_fmt) \
  MSG(MSG_SSID_DS_GSM, xx_ss_mask, xx_fmt)  

/*---------------------------------------------------------------------------
  Macro for messages with 1 parameters.
---------------------------------------------------------------------------*/    
#define DATA_GSM_MSG1(xx_ss_mask, xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS_GSM, xx_ss_mask, xx_fmt, xx_arg1)

/*---------------------------------------------------------------------------
  Macro for messages with 2 parameters.
---------------------------------------------------------------------------*/
#define DATA_GSM_MSG2(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS_GSM, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2)

/*---------------------------------------------------------------------------
  Macro for messages with 3 parameters.
---------------------------------------------------------------------------*/
#define DATA_GSM_MSG3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS_GSM, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

/*---------------------------------------------------------------------------
  This is the macro for messages with 4 parameters.
---------------------------------------------------------------------------*/
#define DATA_GSM_MSG4(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4) \
  MSG_4(MSG_SSID_DS_GSM, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4)

/*---------------------------------------------------------------------------
  This is the macro for messages with 5 parameters.
---------------------------------------------------------------------------*/
#define DATA_GSM_MSG5(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                                               xx_arg5) \
  MSG_5(MSG_SSID_DS_GSM, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5)

/*---------------------------------------------------------------------------
  This is the macro for messages with 6 parameters 
---------------------------------------------------------------------------*/
#define DATA_GSM_MSG6(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                                          xx_arg5, xx_arg6) \
  MSG_6(MSG_SSID_DS_GSM, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6)

/*---------------------------------------------------------------------------
  This is the macro for messages with 7 parameters 
---------------------------------------------------------------------------*/
#define DATA_GSM_MSG7(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                                 xx_arg5, xx_arg6, xx_arg7) \
  MSG_7(MSG_SSID_DS_GSM, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6, xx_arg7)

/*---------------------------------------------------------------------------
  This is the macro for messages with 8 parameters 
---------------------------------------------------------------------------*/
#define DATA_GSM_MSG8(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                        xx_arg5, xx_arg6, xx_arg7, xx_arg8) \
  MSG_8(MSG_SSID_DS_GSM, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6, xx_arg7, xx_arg8)

/*---------------------------------------------------------------------------
  This is the macro for messages with 9 parameters 
---------------------------------------------------------------------------*/
#define DATA_GSM_MSG9(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                               xx_arg5, xx_arg6, xx_arg7, xx_arg8, xx_arg9) \
  MSG_9(MSG_SSID_DS_GSM, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6, xx_arg7, xx_arg8, xx_arg9)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 1 parameters
---------------------------------------------------------------------------*/
#define DATA_GSM_MSG_SPRINTF_1(xx_ss_mask, xx_fmt, xx_arg1) \
  MSG_SPRINTF_1(MSG_SSID_DS_GSM, xx_ss_mask, xx_fmt, xx_arg1)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 2 parameters
---------------------------------------------------------------------------*/
#define DATA_GSM_MSG_SPRINTF_2(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2) \
  MSG_SPRINTF_2(MSG_SSID_DS_GSM, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 3 parameters
---------------------------------------------------------------------------*/
#define DATA_GSM_MSG_SPRINTF_3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_SPRINTF_3(MSG_SSID_DS_GSM, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 3 parameters. This Macro is 
  used when xx_fmt is passed at runtime.
---------------------------------------------------------------------------*/
#define DATA_GSM_MSG_SPRINTF_FMT_VAR_3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_SPRINTF_FMT_VAR_3(MSG_SSID_DS_GSM, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3)
  
/*===========================================================================
  GSM SS related convenience macros. These will substitute the SSID as
  well as the desired sub-group ID.
===========================================================================*/

/*---------------------------------------------------------------------------
  Macro for messages with no parameters.
---------------------------------------------------------------------------*/
#define DATA_GSM_MSG0_GEN(xx_fmt) \
  MSG(MSG_SSID_DS_GSM, DATA_GSM_GENERAL, xx_fmt)
  
#define DATA_GSM_MSG0_DBG(xx_fmt) \
  MSG(MSG_SSID_DS_GSM, DATA_GSM_DEBUG, xx_fmt)
  
#define DATA_GSM_MSG0_TRC(xx_fmt) \
  MSG(MSG_SSID_DS_GSM, DATA_GSM_TRACE, xx_fmt)

/*---------------------------------------------------------------------------
  Macro for messages with 1 parameters.
---------------------------------------------------------------------------*/    
#define DATA_GSM_MSG1_GEN(xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS_GSM, DATA_GSM_GENERAL, xx_fmt, xx_arg1)

#define DATA_GSM_MSG1_DBG(xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS_GSM, DATA_GSM_DEBUG, xx_fmt, xx_arg1)
  
#define DATA_GSM_MSG1_TRC(xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS_GSM, DATA_GSM_TRACE, xx_fmt, xx_arg1)

/*---------------------------------------------------------------------------
  Macro for messages with 2 parameters.
---------------------------------------------------------------------------*/
#define DATA_GSM_MSG2_GEN(xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS_GSM, DATA_GSM_GENERAL, xx_fmt, xx_arg1, xx_arg2)

#define DATA_GSM_MSG2_DBG(xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS_GSM, DATA_GSM_DEBUG, xx_fmt, xx_arg1, xx_arg2)

#define DATA_GSM_MSG2_TRC(xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS_GSM, DATA_GSM_TRACE, xx_fmt, xx_arg1, xx_arg2)

/*---------------------------------------------------------------------------
  Macro for messages with 3 parameters.
---------------------------------------------------------------------------*/
#define DATA_GSM_MSG3_GEN(xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS_GSM, DATA_GSM_GENERAL, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

#define DATA_GSM_MSG3_DBG(xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS_GSM, DATA_GSM_DEBUG, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

#define DATA_GSM_MSG3_TRC(xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS_GSM, DATA_GSM_TRACE, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

/*===========================================================================
  3GPP SS related macros
===========================================================================*/

/*---------------------------------------------------------------------------
  Macro for messages with no parameters.
---------------------------------------------------------------------------*/
#define DATA_3GPP_MSG0(xx_ss_mask, xx_fmt) \
  MSG(MSG_SSID_DS_3GPP, xx_ss_mask, xx_fmt)  

/*---------------------------------------------------------------------------
  Macro for messages with 1 parameters.
---------------------------------------------------------------------------*/    
#define DATA_3GPP_MSG1(xx_ss_mask, xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS_3GPP, xx_ss_mask, xx_fmt, xx_arg1)

/*---------------------------------------------------------------------------
  Macro for messages with 2 parameters.
---------------------------------------------------------------------------*/
#define DATA_3GPP_MSG2(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS_3GPP, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2)

/*---------------------------------------------------------------------------
  Macro for messages with 3 parameters.
---------------------------------------------------------------------------*/
#define DATA_3GPP_MSG3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS_3GPP, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

/*---------------------------------------------------------------------------
  This is the macro for messages with 4 parameters.
---------------------------------------------------------------------------*/
#define DATA_3GPP_MSG4(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4) \
  MSG_4(MSG_SSID_DS_3GPP, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4)

/*---------------------------------------------------------------------------
  This is the macro for messages with 5 parameters.
---------------------------------------------------------------------------*/
#define DATA_3GPP_MSG5(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                                               xx_arg5) \
  MSG_5(MSG_SSID_DS_3GPP, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5)

/*---------------------------------------------------------------------------
  This is the macro for messages with 6 parameters 
---------------------------------------------------------------------------*/
#define DATA_3GPP_MSG6(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                                          xx_arg5, xx_arg6) \
  MSG_6(MSG_SSID_DS_3GPP, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6)

/*---------------------------------------------------------------------------
  This is the macro for messages with 7 parameters 
---------------------------------------------------------------------------*/
#define DATA_3GPP_MSG7(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                                 xx_arg5, xx_arg6, xx_arg7) \
  MSG_7(MSG_SSID_DS_3GPP, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6, xx_arg7)

/*---------------------------------------------------------------------------
  This is the macro for messages with 8 parameters 
---------------------------------------------------------------------------*/
#define DATA_3GPP_MSG8(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                        xx_arg5, xx_arg6, xx_arg7, xx_arg8) \
  MSG_8(MSG_SSID_DS_3GPP, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6, xx_arg7, xx_arg8)

/*---------------------------------------------------------------------------
  This is the macro for messages with 9 parameters 
---------------------------------------------------------------------------*/
#define DATA_3GPP_MSG9(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                               xx_arg5, xx_arg6, xx_arg7, xx_arg8, xx_arg9) \
  MSG_9(MSG_SSID_DS_3GPP, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6, xx_arg7, xx_arg8, xx_arg9)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 1 parameters
---------------------------------------------------------------------------*/
#define DATA_3GPP_MSG_SPRINTF_1(xx_ss_mask, xx_fmt, xx_arg1) \
  MSG_SPRINTF_1(MSG_SSID_DS_3GPP, xx_ss_mask, xx_fmt, xx_arg1)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 2 parameters
---------------------------------------------------------------------------*/
#define DATA_3GPP_MSG_SPRINTF_2(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2) \
  MSG_SPRINTF_2(MSG_SSID_DS_3GPP, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 3 parameters
---------------------------------------------------------------------------*/
#define DATA_3GPP_MSG_SPRINTF_3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_SPRINTF_3(MSG_SSID_DS_3GPP, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 3 parameters. This Macro is 
  used when xx_fmt is passed at runtime.
---------------------------------------------------------------------------*/
#define DATA_3GPP_MSG_SPRINTF_FMT_VAR_3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_SPRINTF_FMT_VAR_3(MSG_SSID_DS_3GPP, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3)
  
/*===========================================================================
  3GPP SS related convenience macros. These will substitute the SSID as
  well as the desired sub-group ID.
===========================================================================*/

/*---------------------------------------------------------------------------
  Macro for messages with no parameters.
---------------------------------------------------------------------------*/
#define DATA_3GPP_MSG0_GEN(xx_fmt) \
  MSG(MSG_SSID_DS_3GPP, DATA_3GPP_GENERAL, xx_fmt)
  
#define DATA_3GPP_MSG0_DBG(xx_fmt) \
  MSG(MSG_SSID_DS_3GPP, DATA_3GPP_DEBUG, xx_fmt)
  
#define DATA_3GPP_MSG0_TRC(xx_fmt) \
  MSG(MSG_SSID_DS_3GPP, DATA_3GPP_TRACE, xx_fmt)

/*---------------------------------------------------------------------------
  Macro for messages with 1 parameters.
---------------------------------------------------------------------------*/    
#define DATA_3GPP_MSG1_GEN(xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS_3GPP, DATA_3GPP_GENERAL, xx_fmt, xx_arg1)

#define DATA_3GPP_MSG1_DBG(xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS_3GPP, DATA_3GPP_DEBUG, xx_fmt, xx_arg1)
  
#define DATA_3GPP_MSG1_TRC(xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS_3GPP, DATA_3GPP_TRACE, xx_fmt, xx_arg1)

/*---------------------------------------------------------------------------
  Macro for messages with 2 parameters.
---------------------------------------------------------------------------*/
#define DATA_3GPP_MSG2_GEN(xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS_3GPP, DATA_3GPP_GENERAL, xx_fmt, xx_arg1, xx_arg2)

#define DATA_3GPP_MSG2_DBG(xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS_3GPP, DATA_3GPP_DEBUG, xx_fmt, xx_arg1, xx_arg2)

#define DATA_3GPP_MSG2_TRC(xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS_3GPP, DATA_3GPP_TRACE, xx_fmt, xx_arg1, xx_arg2)

/*---------------------------------------------------------------------------
  Macro for messages with 3 parameters.
---------------------------------------------------------------------------*/
#define DATA_3GPP_MSG3_GEN(xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS_3GPP, DATA_3GPP_GENERAL, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

#define DATA_3GPP_MSG3_DBG(xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS_3GPP, DATA_3GPP_DEBUG, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

#define DATA_3GPP_MSG3_TRC(xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS_3GPP, DATA_3GPP_TRACE, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

/*===========================================================================
  LTE SS related macros
===========================================================================*/

/*---------------------------------------------------------------------------
  Macro for messages with no parameters.
---------------------------------------------------------------------------*/
#define DATA_LTE_MSG0(xx_ss_mask, xx_fmt) \
  MSG(MSG_SSID_DS_LTE, xx_ss_mask, xx_fmt)  

/*---------------------------------------------------------------------------
  Macro for messages with 1 parameters.
---------------------------------------------------------------------------*/    
#define DATA_LTE_MSG1(xx_ss_mask, xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS_LTE, xx_ss_mask, xx_fmt, xx_arg1)

/*---------------------------------------------------------------------------
  Macro for messages with 2 parameters.
---------------------------------------------------------------------------*/
#define DATA_LTE_MSG2(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS_LTE, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2)

/*---------------------------------------------------------------------------
  Macro for messages with 3 parameters.
---------------------------------------------------------------------------*/
#define DATA_LTE_MSG3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS_LTE, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

/*---------------------------------------------------------------------------
  This is the macro for messages with 4 parameters.
---------------------------------------------------------------------------*/
#define DATA_LTE_MSG4(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4) \
  MSG_4(MSG_SSID_DS_LTE, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4)

/*---------------------------------------------------------------------------
  This is the macro for messages with 5 parameters.
---------------------------------------------------------------------------*/
#define DATA_LTE_MSG5(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                                               xx_arg5) \
  MSG_5(MSG_SSID_DS_LTE, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5)

/*---------------------------------------------------------------------------
  This is the macro for messages with 6 parameters 
---------------------------------------------------------------------------*/
#define DATA_LTE_MSG6(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                                          xx_arg5, xx_arg6) \
  MSG_6(MSG_SSID_DS_LTE, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6)

/*---------------------------------------------------------------------------
  This is the macro for messages with 7 parameters 
---------------------------------------------------------------------------*/
#define DATA_LTE_MSG7(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                                 xx_arg5, xx_arg6, xx_arg7) \
  MSG_7(MSG_SSID_DS_LTE, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6, xx_arg7)

/*---------------------------------------------------------------------------
  This is the macro for messages with 8 parameters 
---------------------------------------------------------------------------*/
#define DATA_LTE_MSG8(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                        xx_arg5, xx_arg6, xx_arg7, xx_arg8) \
  MSG_8(MSG_SSID_DS_LTE, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6, xx_arg7, xx_arg8)

/*---------------------------------------------------------------------------
  This is the macro for messages with 9 parameters 
---------------------------------------------------------------------------*/
#define DATA_LTE_MSG9(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                               xx_arg5, xx_arg6, xx_arg7, xx_arg8, xx_arg9) \
  MSG_9(MSG_SSID_DS_LTE, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6, xx_arg7, xx_arg8, xx_arg9)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 1 parameters
---------------------------------------------------------------------------*/
#define DATA_LTE_MSG_SPRINTF_1(xx_ss_mask, xx_fmt, xx_arg1) \
  MSG_SPRINTF_1(MSG_SSID_DS_LTE, xx_ss_mask, xx_fmt, xx_arg1)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 2 parameters
---------------------------------------------------------------------------*/
#define DATA_LTE_MSG_SPRINTF_2(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2) \
  MSG_SPRINTF_2(MSG_SSID_DS_LTE, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 3 parameters
---------------------------------------------------------------------------*/
#define DATA_LTE_MSG_SPRINTF_3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_SPRINTF_3(MSG_SSID_DS_LTE, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 3 parameters. This Macro is 
  used when xx_fmt is passed at runtime.
---------------------------------------------------------------------------*/
#define DATA_LTE_MSG_SPRINTF_FMT_VAR_3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_SPRINTF_FMT_VAR_3(MSG_SSID_DS_LTE, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3)
  
/*===========================================================================
  LTE SS related convenience macros. These will substitute the SSID as
  well as the desired sub-group ID.
===========================================================================*/

/*---------------------------------------------------------------------------
  Macro for messages with no parameters.
---------------------------------------------------------------------------*/
#define DATA_LTE_MSG0_GEN(xx_fmt) \
  MSG(MSG_SSID_DS_LTE, DATA_LTE_GENERAL, xx_fmt)
  
#define DATA_LTE_MSG0_DBG(xx_fmt) \
  MSG(MSG_SSID_DS_LTE, DATA_LTE_DEBUG, xx_fmt)
  
#define DATA_LTE_MSG0_TRC(xx_fmt) \
  MSG(MSG_SSID_DS_LTE, DATA_LTE_TRACE, xx_fmt)

/*---------------------------------------------------------------------------
  Macro for messages with 1 parameters.
---------------------------------------------------------------------------*/    
#define DATA_LTE_MSG1_GEN(xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS_LTE, DATA_LTE_GENERAL, xx_fmt, xx_arg1)

#define DATA_LTE_MSG1_DBG(xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS_LTE, DATA_LTE_DEBUG, xx_fmt, xx_arg1)
  
#define DATA_LTE_MSG1_TRC(xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS_LTE, DATA_LTE_TRACE, xx_fmt, xx_arg1)

/*---------------------------------------------------------------------------
  Macro for messages with 2 parameters.
---------------------------------------------------------------------------*/
#define DATA_LTE_MSG2_GEN(xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS_LTE, DATA_LTE_GENERAL, xx_fmt, xx_arg1, xx_arg2)

#define DATA_LTE_MSG2_DBG(xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS_LTE, DATA_LTE_DEBUG, xx_fmt, xx_arg1, xx_arg2)

#define DATA_LTE_MSG2_TRC(xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS_LTE, DATA_LTE_TRACE, xx_fmt, xx_arg1, xx_arg2)

/*---------------------------------------------------------------------------
  Macro for messages with 3 parameters.
---------------------------------------------------------------------------*/
#define DATA_LTE_MSG3_GEN(xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS_LTE, DATA_LTE_GENERAL, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

#define DATA_LTE_MSG3_DBG(xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS_LTE, DATA_LTE_DEBUG, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

#define DATA_LTE_MSG3_TRC(xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS_LTE, DATA_LTE_TRACE, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

/*===========================================================================
  WCDMA SS related macros
===========================================================================*/

/*---------------------------------------------------------------------------
  Macro for messages with no parameters.
---------------------------------------------------------------------------*/
#define DATA_WCDMA_MSG0(xx_ss_mask, xx_fmt) \
  MSG(MSG_SSID_DS_WCDMA, xx_ss_mask, xx_fmt)  

/*---------------------------------------------------------------------------
  Macro for messages with 1 parameters.
---------------------------------------------------------------------------*/    
#define DATA_WCDMA_MSG1(xx_ss_mask, xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS_WCDMA, xx_ss_mask, xx_fmt, xx_arg1)

/*---------------------------------------------------------------------------
  Macro for messages with 2 parameters.
---------------------------------------------------------------------------*/
#define DATA_WCDMA_MSG2(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS_WCDMA, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2)

/*---------------------------------------------------------------------------
  Macro for messages with 3 parameters.
---------------------------------------------------------------------------*/
#define DATA_WCDMA_MSG3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS_WCDMA, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

/*---------------------------------------------------------------------------
  This is the macro for messages with 4 parameters.
---------------------------------------------------------------------------*/
#define DATA_WCDMA_MSG4(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4) \
  MSG_4(MSG_SSID_DS_WCDMA, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4)

/*---------------------------------------------------------------------------
  This is the macro for messages with 5 parameters.
---------------------------------------------------------------------------*/
#define DATA_WCDMA_MSG5(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                                               xx_arg5) \
  MSG_5(MSG_SSID_DS_WCDMA, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5)

/*---------------------------------------------------------------------------
  This is the macro for messages with 6 parameters 
---------------------------------------------------------------------------*/
#define DATA_WCDMA_MSG6(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                                          xx_arg5, xx_arg6) \
  MSG_6(MSG_SSID_DS_WCDMA, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6)

/*---------------------------------------------------------------------------
  This is the macro for messages with 7 parameters 
---------------------------------------------------------------------------*/
#define DATA_WCDMA_MSG7(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                                 xx_arg5, xx_arg6, xx_arg7) \
  MSG_7(MSG_SSID_DS_WCDMA, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6, xx_arg7)

/*---------------------------------------------------------------------------
  This is the macro for messages with 8 parameters 
---------------------------------------------------------------------------*/
#define DATA_WCDMA_MSG8(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                        xx_arg5, xx_arg6, xx_arg7, xx_arg8) \
  MSG_8(MSG_SSID_DS_WCDMA, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6, xx_arg7, xx_arg8)

/*---------------------------------------------------------------------------
  This is the macro for messages with 9 parameters 
---------------------------------------------------------------------------*/
#define DATA_WCDMA_MSG9(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                               xx_arg5, xx_arg6, xx_arg7, xx_arg8, xx_arg9) \
  MSG_9(MSG_SSID_DS_WCDMA, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6, xx_arg7, xx_arg8, xx_arg9)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 1 parameters
---------------------------------------------------------------------------*/
#define DATA_WCDMA_MSG_SPRINTF_1(xx_ss_mask, xx_fmt, xx_arg1) \
  MSG_SPRINTF_1(MSG_SSID_DS_WCDMA, xx_ss_mask, xx_fmt, xx_arg1)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 2 parameters
---------------------------------------------------------------------------*/
#define DATA_WCDMA_MSG_SPRINTF_2(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2) \
  MSG_SPRINTF_2(MSG_SSID_DS_WCDMA, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 3 parameters
---------------------------------------------------------------------------*/
#define DATA_WCDMA_MSG_SPRINTF_3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_SPRINTF_3(MSG_SSID_DS_WCDMA, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 3 parameters. This Macro is 
  used when xx_fmt is passed at runtime.
---------------------------------------------------------------------------*/
#define DATA_WCDMA_MSG_SPRINTF_FMT_VAR_3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_SPRINTF_FMT_VAR_3(MSG_SSID_DS_WCDMA, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

/*===========================================================================
  WCDMA SS related convenience macros. These will substitute the SSID as
  well as the desired sub-group ID.
===========================================================================*/

/*---------------------------------------------------------------------------
  Macro for messages with no parameters.
---------------------------------------------------------------------------*/
#define DATA_WCDMA_MSG0_GEN(xx_fmt) \
  MSG(MSG_SSID_DS_WCDMA, DATA_WCDMA_GENERAL, xx_fmt)
  
#define DATA_WCDMA_MSG0_DBG(xx_fmt) \
  MSG(MSG_SSID_DS_WCDMA, DATA_WCDMA_DEBUG, xx_fmt)
  
#define DATA_WCDMA_MSG0_TRC(xx_fmt) \
  MSG(MSG_SSID_DS_WCDMA, DATA_WCDMA_TRACE, xx_fmt)

/*---------------------------------------------------------------------------
  Macro for messages with 1 parameters.
---------------------------------------------------------------------------*/    
#define DATA_WCDMA_MSG1_GEN(xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS_WCDMA, DATA_WCDMA_GENERAL, xx_fmt, xx_arg1)

#define DATA_WCDMA_MSG1_DBG(xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS_WCDMA, DATA_WCDMA_DEBUG, xx_fmt, xx_arg1)
  
#define DATA_WCDMA_MSG1_TRC(xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS_WCDMA, DATA_WCDMA_TRACE, xx_fmt, xx_arg1)

/*---------------------------------------------------------------------------
  Macro for messages with 2 parameters.
---------------------------------------------------------------------------*/
#define DATA_WCDMA_MSG2_GEN(xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS_WCDMA, DATA_WCDMA_GENERAL, xx_fmt, xx_arg1, xx_arg2)

#define DATA_WCDMA_MSG2_DBG(xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS_WCDMA, DATA_WCDMA_DEBUG, xx_fmt, xx_arg1, xx_arg2)

#define DATA_WCDMA_MSG2_TRC(xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS_WCDMA, DATA_WCDMA_TRACE, xx_fmt, xx_arg1, xx_arg2)

/*---------------------------------------------------------------------------
  Macro for messages with 3 parameters.
---------------------------------------------------------------------------*/
#define DATA_WCDMA_MSG3_GEN(xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS_WCDMA, DATA_WCDMA_GENERAL, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

#define DATA_WCDMA_MSG3_DBG(xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS_WCDMA, DATA_WCDMA_DEBUG, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

#define DATA_WCDMA_MSG3_TRC(xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS_WCDMA, DATA_WCDMA_TRACE, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

/*===========================================================================
  SOCKETS SS related macros
===========================================================================*/


/*---------------------------------------------------------------------------
  Macro for messages with no parameters.
---------------------------------------------------------------------------*/
#define DATA_SOCKETS_MSG0(xx_ss_mask, xx_fmt) \
  MSG(MSG_SSID_DS_SOCKETS, xx_ss_mask, xx_fmt)  

/*---------------------------------------------------------------------------
  Macro for messages with 1 parameters.
---------------------------------------------------------------------------*/    
#define DATA_SOCKETS_MSG1(xx_ss_mask, xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS_SOCKETS, xx_ss_mask, xx_fmt, xx_arg1)

/*---------------------------------------------------------------------------
  Macro for messages with 2 parameters.
---------------------------------------------------------------------------*/
#define DATA_SOCKETS_MSG2(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS_SOCKETS, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2)

/*---------------------------------------------------------------------------
  Macro for messages with 3 parameters.
---------------------------------------------------------------------------*/
#define DATA_SOCKETS_MSG3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS_SOCKETS, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

/*---------------------------------------------------------------------------
  This is the macro for messages with 4 parameters.
---------------------------------------------------------------------------*/
#define DATA_SOCKETS_MSG4(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4) \
  MSG_4(MSG_SSID_DS_SOCKETS, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4)

/*---------------------------------------------------------------------------
  This is the macro for messages with 5 parameters.
---------------------------------------------------------------------------*/
#define DATA_SOCKETS_MSG5(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                                               xx_arg5) \
  MSG_5(MSG_SSID_DS_SOCKETS, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5)

/*---------------------------------------------------------------------------
  This is the macro for messages with 6 parameters 
---------------------------------------------------------------------------*/
#define DATA_SOCKETS_MSG6(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                                          xx_arg5, xx_arg6) \
  MSG_6(MSG_SSID_DS_SOCKETS, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6)

/*---------------------------------------------------------------------------
  This is the macro for messages with 7 parameters 
---------------------------------------------------------------------------*/
#define DATA_SOCKETS_MSG7(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                                 xx_arg5, xx_arg6, xx_arg7) \
  MSG_7(MSG_SSID_DS_SOCKETS, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6, xx_arg7)

/*---------------------------------------------------------------------------
  This is the macro for messages with 8 parameters 
---------------------------------------------------------------------------*/
#define DATA_SOCKETS_MSG8(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                        xx_arg5, xx_arg6, xx_arg7, xx_arg8) \
  MSG_8(MSG_SSID_DS_SOCKETS, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6, xx_arg7, xx_arg8)

/*---------------------------------------------------------------------------
  This is the macro for messages with 9 parameters 
---------------------------------------------------------------------------*/
#define DATA_SOCKETS_MSG9(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                               xx_arg5, xx_arg6, xx_arg7, xx_arg8, xx_arg9) \
  MSG_9(MSG_SSID_DS_SOCKETS, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6, xx_arg7, xx_arg8, xx_arg9)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 1 parameters
---------------------------------------------------------------------------*/
#define DATA_SOCKETS_MSG_SPRINTF_1(xx_ss_mask, xx_fmt, xx_arg1) \
  MSG_SPRINTF_1(MSG_SSID_DS_SOCKETS, xx_ss_mask, xx_fmt, xx_arg1)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 2 parameters
---------------------------------------------------------------------------*/
#define DATA_SOCKETS_MSG_SPRINTF_2(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2) \
  MSG_SPRINTF_2(MSG_SSID_DS_SOCKETS, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 3 parameters
---------------------------------------------------------------------------*/
#define DATA_SOCKETS_MSG_SPRINTF_3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_SPRINTF_3(MSG_SSID_DS_SOCKETS, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 3 parameters. This Macro is 
  used when xx_fmt is passed at runtime.
---------------------------------------------------------------------------*/
#define DATA_SOCKETS_MSG_SPRINTF_FMT_VAR_3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_SPRINTF_FMT_VAR_3(MSG_SSID_DS_SOCKETS, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

/*===========================================================================
  SOCKETS SS related convenience macros. These will substitute the SSID as
  well as the desired sub-group ID.
===========================================================================*/

/*---------------------------------------------------------------------------
  Macro for messages with no parameters.
---------------------------------------------------------------------------*/
#define DATA_SOCKETS_MSG0_GEN(xx_fmt) \
  MSG(MSG_SSID_DS_SOCKETS, DATA_SOCKETS_GENERAL, xx_fmt)
  
#define DATA_SOCKETS_MSG0_DBG(xx_fmt) \
  MSG(MSG_SSID_DS_SOCKETS, DATA_SOCKETS_DEBUG, xx_fmt)
  
#define DATA_SOCKETS_MSG0_TRC(xx_fmt) \
  MSG(MSG_SSID_DS_SOCKETS, DATA_SOCKETS_TRACE, xx_fmt)

/*---------------------------------------------------------------------------
  Macro for messages with 1 parameters.
---------------------------------------------------------------------------*/    
#define DATA_SOCKETS_MSG1_GEN(xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS_SOCKETS, DATA_SOCKETS_GENERAL, xx_fmt, xx_arg1)

#define DATA_SOCKETS_MSG1_DBG(xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS_SOCKETS, DATA_SOCKETS_DEBUG, xx_fmt, xx_arg1)
  
#define DATA_SOCKETS_MSG1_TRC(xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS_SOCKETS, DATA_SOCKETS_TRACE, xx_fmt, xx_arg1)

/*---------------------------------------------------------------------------
  Macro for messages with 2 parameters.
---------------------------------------------------------------------------*/
#define DATA_SOCKETS_MSG2_GEN(xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS_SOCKETS, DATA_SOCKETS_GENERAL, xx_fmt, xx_arg1, xx_arg2)

#define DATA_SOCKETS_MSG2_DBG(xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS_SOCKETS, DATA_SOCKETS_DEBUG, xx_fmt, xx_arg1, xx_arg2)

#define DATA_SOCKETS_MSG2_TRC(xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS_SOCKETS, DATA_SOCKETS_TRACE, xx_fmt, xx_arg1, xx_arg2)

/*---------------------------------------------------------------------------
  Macro for messages with 3 parameters.
---------------------------------------------------------------------------*/
#define DATA_SOCKETS_MSG3_GEN(xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS_SOCKETS, DATA_SOCKETS_GENERAL, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

#define DATA_SOCKETS_MSG3_DBG(xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS_SOCKETS, DATA_SOCKETS_DEBUG, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

#define DATA_SOCKETS_MSG3_TRC(xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS_SOCKETS, DATA_SOCKETS_TRACE, xx_fmt, xx_arg1, xx_arg2, xx_arg3)
  
/*===========================================================================
  ATCOP SS related macros
===========================================================================*/


/*---------------------------------------------------------------------------
  Macro for messages with no parameters.
---------------------------------------------------------------------------*/
#define DATA_ATCOP_MSG0(xx_ss_mask, xx_fmt) \
  MSG(MSG_SSID_DS_ATCOP, xx_ss_mask, xx_fmt)  

/*---------------------------------------------------------------------------
  Macro for messages with 1 parameters.
---------------------------------------------------------------------------*/    
#define DATA_ATCOP_MSG1(xx_ss_mask, xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS_ATCOP, xx_ss_mask, xx_fmt, xx_arg1)

/*---------------------------------------------------------------------------
  Macro for messages with 2 parameters.
---------------------------------------------------------------------------*/
#define DATA_ATCOP_MSG2(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS_ATCOP, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2)

/*---------------------------------------------------------------------------
  Macro for messages with 3 parameters.
---------------------------------------------------------------------------*/
#define DATA_ATCOP_MSG3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS_ATCOP, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

/*---------------------------------------------------------------------------
  This is the macro for messages with 4 parameters.
---------------------------------------------------------------------------*/
#define DATA_ATCOP_MSG4(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4) \
  MSG_4(MSG_SSID_DS_ATCOP, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4)

/*---------------------------------------------------------------------------
  This is the macro for messages with 5 parameters.
---------------------------------------------------------------------------*/
#define DATA_ATCOP_MSG5(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                                               xx_arg5) \
  MSG_5(MSG_SSID_DS_ATCOP, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5)

/*---------------------------------------------------------------------------
  This is the macro for messages with 6 parameters 
---------------------------------------------------------------------------*/
#define DATA_ATCOP_MSG6(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                                          xx_arg5, xx_arg6) \
  MSG_6(MSG_SSID_DS_ATCOP, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6)

/*---------------------------------------------------------------------------
  This is the macro for messages with 7 parameters 
---------------------------------------------------------------------------*/
#define DATA_ATCOP_MSG7(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                                 xx_arg5, xx_arg6, xx_arg7) \
  MSG_7(MSG_SSID_DS_ATCOP, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6, xx_arg7)

/*---------------------------------------------------------------------------
  This is the macro for messages with 8 parameters 
---------------------------------------------------------------------------*/
#define DATA_ATCOP_MSG8(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                        xx_arg5, xx_arg6, xx_arg7, xx_arg8) \
  MSG_8(MSG_SSID_DS_ATCOP, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6, xx_arg7, xx_arg8)

/*---------------------------------------------------------------------------
  This is the macro for messages with 9 parameters 
---------------------------------------------------------------------------*/
#define DATA_ATCOP_MSG9(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                               xx_arg5, xx_arg6, xx_arg7, xx_arg8, xx_arg9) \
  MSG_9(MSG_SSID_DS_ATCOP, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6, xx_arg7, xx_arg8, xx_arg9)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 1 parameters
---------------------------------------------------------------------------*/
#define DATA_ATCOP_MSG_SPRINTF_1(xx_ss_mask, xx_fmt, xx_arg1) \
  MSG_SPRINTF_1(MSG_SSID_DS_ATCOP, xx_ss_mask, xx_fmt, xx_arg1)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 2 parameters
---------------------------------------------------------------------------*/
#define DATA_ATCOP_MSG_SPRINTF_2(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2) \
  MSG_SPRINTF_2(MSG_SSID_DS_ATCOP, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 3 parameters
---------------------------------------------------------------------------*/
#define DATA_ATCOP_MSG_SPRINTF_3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_SPRINTF_3(MSG_SSID_DS_ATCOP, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 4 parameters
---------------------------------------------------------------------------*/
#define DATA_ATCOP_MSG_SPRINTF_4(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
	                                                                     xx_arg4) \
  MSG_SPRINTF_4(MSG_SSID_DS_ATCOP, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                                                       xx_arg4)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 5 parameters
---------------------------------------------------------------------------*/
#define DATA_ATCOP_MSG_SPRINTF_5(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
	                                                            xx_arg4, xx_arg5) \
  MSG_SPRINTF_5(MSG_SSID_DS_ATCOP, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                                              xx_arg4, xx_arg5)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 6 parameters
---------------------------------------------------------------------------*/
#define DATA_ATCOP_MSG_SPRINTF_6(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
	                                                   xx_arg4, xx_arg5, xx_arg6) \
  MSG_SPRINTF_6(MSG_SSID_DS_ATCOP, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                                     xx_arg4, xx_arg5, xx_arg6)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 7 parameters
---------------------------------------------------------------------------*/
#define DATA_ATCOP_MSG_SPRINTF_7(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
	                                           xx_arg4, xx_arg5, xx_arg6, xx_arg7) \
  MSG_SPRINTF_7(MSG_SSID_DS_ATCOP, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                             xx_arg4, xx_arg5, xx_arg6, xx_arg7)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 8 parameters
---------------------------------------------------------------------------*/
#define DATA_ATCOP_MSG_SPRINTF_8(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
	                                 xx_arg4, xx_arg5, xx_arg6, xx_arg7, xx_arg8) \
  MSG_SPRINTF_8(MSG_SSID_DS_ATCOP, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                   xx_arg4, xx_arg5, xx_arg6, xx_arg7, xx_arg8)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 3 parameters. This Macro is 
  used when xx_fmt is passed at runtime.
---------------------------------------------------------------------------*/
#define DATA_ATCOP_MSG_SPRINTF_FMT_VAR_3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_SPRINTF_FMT_VAR_3(MSG_SSID_DS_ATCOP, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

/*===========================================================================
  ATCOP SS related convenience macros. These will substitute the SSID as
  well as the desired sub-group ID.
===========================================================================*/

/*---------------------------------------------------------------------------
  Macro for messages with no parameters.
---------------------------------------------------------------------------*/
#define DATA_ATCOP_MSG0_GEN(xx_fmt) \
  MSG(MSG_SSID_DS_ATCOP, DATA_ATCOP_GENERAL, xx_fmt)
  
#define DATA_ATCOP_MSG0_DBG(xx_fmt) \
  MSG(MSG_SSID_DS_ATCOP, DATA_ATCOP_DEBUG, xx_fmt)
  
#define DATA_ATCOP_MSG0_TRC(xx_fmt) \
  MSG(MSG_SSID_DS_ATCOP, DATA_ATCOP_TRACE, xx_fmt)

/*---------------------------------------------------------------------------
  Macro for messages with 1 parameters.
---------------------------------------------------------------------------*/    
#define DATA_ATCOP_MSG1_GEN(xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS_ATCOP, DATA_ATCOP_GENERAL, xx_fmt, xx_arg1)

#define DATA_ATCOP_MSG1_DBG(xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS_ATCOP, DATA_ATCOP_DEBUG, xx_fmt, xx_arg1)
  
#define DATA_ATCOP_MSG1_TRC(xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS_ATCOP, DATA_ATCOP_TRACE, xx_fmt, xx_arg1)

/*---------------------------------------------------------------------------
  Macro for messages with 2 parameters.
---------------------------------------------------------------------------*/
#define DATA_ATCOP_MSG2_GEN(xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS_ATCOP, DATA_ATCOP_GENERAL, xx_fmt, xx_arg1, xx_arg2)

#define DATA_ATCOP_MSG2_DBG(xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS_ATCOP, DATA_ATCOP_DEBUG, xx_fmt, xx_arg1, xx_arg2)

#define DATA_ATCOP_MSG2_TRC(xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS_ATCOP, DATA_ATCOP_TRACE, xx_fmt, xx_arg1, xx_arg2)

/*---------------------------------------------------------------------------
  Macro for messages with 3 parameters.
---------------------------------------------------------------------------*/
#define DATA_ATCOP_MSG3_GEN(xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS_ATCOP, DATA_ATCOP_GENERAL, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

#define DATA_ATCOP_MSG3_DBG(xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS_ATCOP, DATA_ATCOP_DEBUG, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

#define DATA_ATCOP_MSG3_TRC(xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS_ATCOP, DATA_ATCOP_TRACE, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

/*===========================================================================
  SIO SS related macros
===========================================================================*/


/*---------------------------------------------------------------------------
  Macro for messages with no parameters.
---------------------------------------------------------------------------*/
#define DATA_SIO_MSG0(xx_ss_mask, xx_fmt) \
  MSG(MSG_SSID_DS_SIO, xx_ss_mask, xx_fmt)  

/*---------------------------------------------------------------------------
  Macro for messages with 1 parameters.
---------------------------------------------------------------------------*/    
#define DATA_SIO_MSG1(xx_ss_mask, xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS_SIO, xx_ss_mask, xx_fmt, xx_arg1)

/*---------------------------------------------------------------------------
  Macro for messages with 2 parameters.
---------------------------------------------------------------------------*/
#define DATA_SIO_MSG2(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS_SIO, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2)

/*---------------------------------------------------------------------------
  Macro for messages with 3 parameters.
---------------------------------------------------------------------------*/
#define DATA_SIO_MSG3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS_SIO, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

/*---------------------------------------------------------------------------
  This is the macro for messages with 4 parameters.
---------------------------------------------------------------------------*/
#define DATA_SIO_MSG4(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4) \
  MSG_4(MSG_SSID_DS_SIO, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4)

/*---------------------------------------------------------------------------
  This is the macro for messages with 5 parameters.
---------------------------------------------------------------------------*/
#define DATA_SIO_MSG5(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                                               xx_arg5) \
  MSG_5(MSG_SSID_DS_SIO, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5)

/*---------------------------------------------------------------------------
  This is the macro for messages with 6 parameters 
---------------------------------------------------------------------------*/
#define DATA_SIO_MSG6(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                                          xx_arg5, xx_arg6) \
  MSG_6(MSG_SSID_DS_SIO, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6)

/*---------------------------------------------------------------------------
  This is the macro for messages with 7 parameters 
---------------------------------------------------------------------------*/
#define DATA_SIO_MSG7(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                                 xx_arg5, xx_arg6, xx_arg7) \
  MSG_7(MSG_SSID_DS_SIO, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6, xx_arg7)

/*---------------------------------------------------------------------------
  This is the macro for messages with 8 parameters 
---------------------------------------------------------------------------*/
#define DATA_SIO_MSG8(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                        xx_arg5, xx_arg6, xx_arg7, xx_arg8) \
  MSG_8(MSG_SSID_DS_SIO, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6, xx_arg7, xx_arg8)

/*---------------------------------------------------------------------------
  This is the macro for messages with 9 parameters 
---------------------------------------------------------------------------*/
#define DATA_SIO_MSG9(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                               xx_arg5, xx_arg6, xx_arg7, xx_arg8, xx_arg9) \
  MSG_9(MSG_SSID_DS_SIO, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6, xx_arg7, xx_arg8, xx_arg9)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 1 parameters
---------------------------------------------------------------------------*/
#define DATA_SIO_MSG_SPRINTF_1(xx_ss_mask, xx_fmt, xx_arg1) \
  MSG_SPRINTF_1(MSG_SSID_DS_SIO, xx_ss_mask, xx_fmt, xx_arg1)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 2 parameters
---------------------------------------------------------------------------*/
#define DATA_SIO_MSG_SPRINTF_2(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2) \
  MSG_SPRINTF_2(MSG_SSID_DS_SIO, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 3 parameters
---------------------------------------------------------------------------*/
#define DATA_SIO_MSG_SPRINTF_3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_SPRINTF_3(MSG_SSID_DS_SIO, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 3 parameters. This Macro is 
  used when xx_fmt is passed at runtime.
---------------------------------------------------------------------------*/
#define DATA_SIO_MSG_SPRINTF_FMT_VAR_3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_SPRINTF_FMT_VAR_3(MSG_SSID_DS_SIO, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

/*===========================================================================
  SIO SS related convenience macros. These will substitute the SSID as
  well as the desired sub-group ID.
===========================================================================*/

/*---------------------------------------------------------------------------
  Macro for messages with no parameters.
---------------------------------------------------------------------------*/
#define DATA_SIO_MSG0_GEN(xx_fmt) \
  MSG(MSG_SSID_DS_SIO, DATA_SIO_GENERAL, xx_fmt)
  
#define DATA_SIO_MSG0_DBG(xx_fmt) \
  MSG(MSG_SSID_DS_SIO, DATA_SIO_DEBUG, xx_fmt)
  
#define DATA_SIO_MSG0_TRC(xx_fmt) \
  MSG(MSG_SSID_DS_SIO, DATA_SIO_TRACE, xx_fmt)

/*---------------------------------------------------------------------------
  Macro for messages with 1 parameters.
---------------------------------------------------------------------------*/    
#define DATA_SIO_MSG1_GEN(xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS_SIO, DATA_SIO_GENERAL, xx_fmt, xx_arg1)

#define DATA_SIO_MSG1_DBG(xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS_SIO, DATA_SIO_DEBUG, xx_fmt, xx_arg1)
  
#define DATA_SIO_MSG1_TRC(xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS_SIO, DATA_SIO_TRACE, xx_fmt, xx_arg1)

/*---------------------------------------------------------------------------
  Macro for messages with 2 parameters.
---------------------------------------------------------------------------*/
#define DATA_SIO_MSG2_GEN(xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS_SIO, DATA_SIO_GENERAL, xx_fmt, xx_arg1, xx_arg2)

#define DATA_SIO_MSG2_DBG(xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS_SIO, DATA_SIO_DEBUG, xx_fmt, xx_arg1, xx_arg2)

#define DATA_SIO_MSG2_TRC(xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS_SIO, DATA_SIO_TRACE, xx_fmt, xx_arg1, xx_arg2)

/*---------------------------------------------------------------------------
  Macro for messages with 3 parameters.
---------------------------------------------------------------------------*/
#define DATA_SIO_MSG3_GEN(xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS_SIO, DATA_SIO_GENERAL, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

#define DATA_SIO_MSG3_DBG(xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS_SIO, DATA_SIO_DEBUG, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

#define DATA_SIO_MSG3_TRC(xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS_SIO, DATA_SIO_TRACE, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

/*===========================================================================
  BCMCS SS related macros
===========================================================================*/


/*---------------------------------------------------------------------------
  Macro for messages with no parameters.
---------------------------------------------------------------------------*/
#define DATA_BCMCS_MSG0(xx_ss_mask, xx_fmt) \
  MSG(MSG_SSID_DS_BCMCS, xx_ss_mask, xx_fmt)  

/*---------------------------------------------------------------------------
  Macro for messages with 1 parameters.
---------------------------------------------------------------------------*/    
#define DATA_BCMCS_MSG1(xx_ss_mask, xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS_BCMCS, xx_ss_mask, xx_fmt, xx_arg1)

/*---------------------------------------------------------------------------
  Macro for messages with 2 parameters.
---------------------------------------------------------------------------*/
#define DATA_BCMCS_MSG2(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS_BCMCS, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2)

/*---------------------------------------------------------------------------
  Macro for messages with 3 parameters.
---------------------------------------------------------------------------*/
#define DATA_BCMCS_MSG3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS_BCMCS, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

/*---------------------------------------------------------------------------
  This is the macro for messages with 4 parameters.
---------------------------------------------------------------------------*/
#define DATA_BCMCS_MSG4(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4) \
  MSG_4(MSG_SSID_DS_BCMCS, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4)

/*---------------------------------------------------------------------------
  This is the macro for messages with 5 parameters.
---------------------------------------------------------------------------*/
#define DATA_BCMCS_MSG5(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                                               xx_arg5) \
  MSG_5(MSG_SSID_DS_BCMCS, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5)

/*---------------------------------------------------------------------------
  This is the macro for messages with 6 parameters 
---------------------------------------------------------------------------*/
#define DATA_BCMCS_MSG6(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                                          xx_arg5, xx_arg6) \
  MSG_6(MSG_SSID_DS_BCMCS, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6)

/*---------------------------------------------------------------------------
  This is the macro for messages with 7 parameters 
---------------------------------------------------------------------------*/
#define DATA_BCMCS_MSG7(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                                 xx_arg5, xx_arg6, xx_arg7) \
  MSG_7(MSG_SSID_DS_BCMCS, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6, xx_arg7)

/*---------------------------------------------------------------------------
  This is the macro for messages with 8 parameters 
---------------------------------------------------------------------------*/
#define DATA_BCMCS_MSG8(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                        xx_arg5, xx_arg6, xx_arg7, xx_arg8) \
  MSG_8(MSG_SSID_DS_BCMCS, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6, xx_arg7, xx_arg8)

/*---------------------------------------------------------------------------
  This is the macro for messages with 9 parameters 
---------------------------------------------------------------------------*/
#define DATA_BCMCS_MSG9(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                               xx_arg5, xx_arg6, xx_arg7, xx_arg8, xx_arg9) \
  MSG_9(MSG_SSID_DS_BCMCS, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6, xx_arg7, xx_arg8, xx_arg9)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 1 parameters
---------------------------------------------------------------------------*/
#define DATA_BCMCS_MSG_SPRINTF_1(xx_ss_mask, xx_fmt, xx_arg1) \
  MSG_SPRINTF_1(MSG_SSID_DS_BCMCS, xx_ss_mask, xx_fmt, xx_arg1)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 2 parameters
---------------------------------------------------------------------------*/
#define DATA_BCMCS_MSG_SPRINTF_2(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2) \
  MSG_SPRINTF_2(MSG_SSID_DS_BCMCS, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 3 parameters
---------------------------------------------------------------------------*/
#define DATA_BCMCS_MSG_SPRINTF_3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_SPRINTF_3(MSG_SSID_DS_BCMCS, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 3 parameters. This Macro is 
  used when xx_fmt is passed at runtime.
---------------------------------------------------------------------------*/
#define DATA_BCMCS_MSG_SPRINTF_FMT_VAR_3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_SPRINTF_FMT_VAR_3(MSG_SSID_DS_BCMCS, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3)


//IGT
/*===========================================================================
  BSMCS SS related convenience macros. These will substitute the SSID as
  well as the desired sub-group ID.
===========================================================================*/

/*---------------------------------------------------------------------------
  Macro for messages with no parameters.
---------------------------------------------------------------------------*/
#define DATA_BCMCS_MSG0_GEN(xx_fmt) \
  MSG(MSG_SSID_DS_BCMCS, DATA_BCMCS_GENERAL, xx_fmt)
  
#define DATA_BCMCS_MSG0_DBG(xx_fmt) \
  MSG(MSG_SSID_DS_BCMCS, DATA_BCMCS_DEBUG, xx_fmt)
  
#define DATA_BCMCS_MSG0_TRC(xx_fmt) \
  MSG(MSG_SSID_DS_BCMCS, DATA_BCMCS_TRACE, xx_fmt)

/*---------------------------------------------------------------------------
  Macro for messages with 1 parameters.
---------------------------------------------------------------------------*/    
#define DATA_BCMCS_MSG1_GEN(xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS_BCMCS, DATA_BCMCS_GENERAL, xx_fmt, xx_arg1)

#define DATA_BCMCS_MSG1_DBG(xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS_BCMCS, DATA_BCMCS_DEBUG, xx_fmt, xx_arg1)
  
#define DATA_BCMCS_MSG1_TRC(xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS_BCMCS, DATA_BCMCS_TRACE, xx_fmt, xx_arg1)

/*---------------------------------------------------------------------------
  Macro for messages with 2 parameters.
---------------------------------------------------------------------------*/
#define DATA_BCMCS_MSG2_GEN(xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS_BCMCS, DATA_BCMCS_GENERAL, xx_fmt, xx_arg1, xx_arg2)

#define DATA_BCMCS_MSG2_DBG(xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS_BCMCS, DATA_BCMCS_DEBUG, xx_fmt, xx_arg1, xx_arg2)

#define DATA_BCMCS_MSG2_TRC(xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS_BCMCS, DATA_BCMCS_TRACE, xx_fmt, xx_arg1, xx_arg2)

/*---------------------------------------------------------------------------
  Macro for messages with 3 parameters.
---------------------------------------------------------------------------*/
#define DATA_BCMCS_MSG3_GEN(xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS_BCMCS, DATA_BCMCS_GENERAL, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

#define DATA_BCMCS_MSG3_DBG(xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS_BCMCS, DATA_BCMCS_DEBUG, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

#define DATA_BCMCS_MSG3_TRC(xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS_BCMCS, DATA_BCMCS_TRACE, xx_fmt, xx_arg1, xx_arg2, xx_arg3)
    
/*===========================================================================
  MRLP SS related macros
===========================================================================*/


/*---------------------------------------------------------------------------
  Macro for messages with no parameters.
---------------------------------------------------------------------------*/
#define DATA_MLRLP_MSG0(xx_ss_mask, xx_fmt) \
  MSG(MSG_SSID_DS_MLRLP, xx_ss_mask, xx_fmt)  

/*---------------------------------------------------------------------------
  Macro for messages with 1 parameters.
---------------------------------------------------------------------------*/    
#define DATA_MLRLP_MSG1(xx_ss_mask, xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS_MLRLP, xx_ss_mask, xx_fmt, xx_arg1)

/*---------------------------------------------------------------------------
  Macro for messages with 2 parameters.
---------------------------------------------------------------------------*/
#define DATA_MLRLP_MSG2(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS_MLRLP, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2)

/*---------------------------------------------------------------------------
  Macro for messages with 3 parameters.
---------------------------------------------------------------------------*/
#define DATA_MLRLP_MSG3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS_MLRLP, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

/*---------------------------------------------------------------------------
  This is the macro for messages with 4 parameters.
---------------------------------------------------------------------------*/
#define DATA_MLRLP_MSG4(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4) \
  MSG_4(MSG_SSID_DS_MLRLP, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4)

/*---------------------------------------------------------------------------
  This is the macro for messages with 5 parameters.
---------------------------------------------------------------------------*/
#define DATA_MLRLP_MSG5(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                                               xx_arg5) \
  MSG_5(MSG_SSID_DS_MLRLP, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5)

/*---------------------------------------------------------------------------
  This is the macro for messages with 6 parameters 
---------------------------------------------------------------------------*/
#define DATA_MLRLP_MSG6(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                                          xx_arg5, xx_arg6) \
  MSG_6(MSG_SSID_DS_MLRLP, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6)

/*---------------------------------------------------------------------------
  This is the macro for messages with 7 parameters 
---------------------------------------------------------------------------*/
#define DATA_MLRLP_MSG7(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                                 xx_arg5, xx_arg6, xx_arg7) \
  MSG_7(MSG_SSID_DS_MLRLP, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6, xx_arg7)

/*---------------------------------------------------------------------------
  This is the macro for messages with 8 parameters 
---------------------------------------------------------------------------*/
#define DATA_MLRLP_MSG8(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                        xx_arg5, xx_arg6, xx_arg7, xx_arg8) \
  MSG_8(MSG_SSID_DS_MLRLP, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6, xx_arg7, xx_arg8)

/*---------------------------------------------------------------------------
  This is the macro for messages with 9 parameters 
---------------------------------------------------------------------------*/
#define DATA_MLRLP_MSG9(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                               xx_arg5, xx_arg6, xx_arg7, xx_arg8, xx_arg9) \
  MSG_9(MSG_SSID_DS_MLRLP, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6, xx_arg7, xx_arg8, xx_arg9)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 1 parameters
---------------------------------------------------------------------------*/
#define DATA_MLRLP_MSG_SPRINTF_1(xx_ss_mask, xx_fmt, xx_arg1) \
  MSG_SPRINTF_1(MSG_SSID_DS_MLRLP, xx_ss_mask, xx_fmt, xx_arg1)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 2 parameters
---------------------------------------------------------------------------*/
#define DATA_MLRLP_MSG_SPRINTF_2(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2) \
  MSG_SPRINTF_2(MSG_SSID_DS_MLRLP, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 3 parameters
---------------------------------------------------------------------------*/
#define DATA_MLRLP_MSG_SPRINTF_3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_SPRINTF_3(MSG_SSID_DS_MLRLP, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 3 parameters. This Macro is 
  used when xx_fmt is passed at runtime.
---------------------------------------------------------------------------*/
#define DATA_MLRLP_MSG_SPRINTF_FMT_VAR_3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_SPRINTF_FMT_VAR_3(MSG_SSID_DS_MLRLP, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3)


//IGT
/*===========================================================================
  MLRLP SS related convenience macros. These will substitute the SSID as
  well as the desired sub-group ID.
===========================================================================*/

/*---------------------------------------------------------------------------
  Macro for messages with no parameters.
---------------------------------------------------------------------------*/
#define DATA_MLRLP_MSG0_GEN(xx_fmt) \
  MSG(MSG_SSID_DS_MLRLP, DATA_MLRLP_GENERAL, xx_fmt)
  
#define DATA_MLRLP_MSG0_DBG(xx_fmt) \
  MSG(MSG_SSID_DS_MLRLP, DATA_MLRLP_DEBUG, xx_fmt)
  
#define DATA_MLRLP_MSG0_TRC(xx_fmt) \
  MSG(MSG_SSID_DS_MLRLP, DATA_MLRLP_TRACE, xx_fmt)

/*---------------------------------------------------------------------------
  Macro for messages with 1 parameters.
---------------------------------------------------------------------------*/    
#define DATA_MLRLP_MSG1_GEN(xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS_MLRLP, DATA_MLRLP_GENERAL, xx_fmt, xx_arg1)

#define DATA_MLRLP_MSG1_DBG(xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS_MLRLP, DATA_MLRLP_DEBUG, xx_fmt, xx_arg1)
  
#define DATA_MLRLP_MSG1_TRC(xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS_MLRLP, DATA_MLRLP_TRACE, xx_fmt, xx_arg1)

/*---------------------------------------------------------------------------
  Macro for messages with 2 parameters.
---------------------------------------------------------------------------*/
#define DATA_MLRLP_MSG2_GEN(xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS_MLRLP, DATA_MLRLP_GENERAL, xx_fmt, xx_arg1, xx_arg2)

#define DATA_MLRLP_MSG2_DBG(xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS_MLRLP, DATA_MLRLP_DEBUG, xx_fmt, xx_arg1, xx_arg2)

#define DATA_MLRLP_MSG2_TRC(xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS_MLRLP, DATA_MLRLP_TRACE, xx_fmt, xx_arg1, xx_arg2)

/*---------------------------------------------------------------------------
  Macro for messages with 3 parameters.
---------------------------------------------------------------------------*/
#define DATA_MLRLP_MSG3_GEN(xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS_MLRLP, DATA_MLRLP_GENERAL, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

#define DATA_MLRLP_MSG3_DBG(xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS_MLRLP, DATA_MLRLP_DEBUG, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

#define DATA_MLRLP_MSG3_TRC(xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS_MLRLP, DATA_MLRLP_TRACE, xx_fmt, xx_arg1, xx_arg2, xx_arg3)
    
/*===========================================================================
  RTTP SS related macros
===========================================================================*/


/*---------------------------------------------------------------------------
  Macro for messages with no parameters.
---------------------------------------------------------------------------*/
#define DATA_RTP_MSG0(xx_ss_mask, xx_fmt) \
  MSG(MSG_SSID_DS_RTP, xx_ss_mask, xx_fmt)  

/*---------------------------------------------------------------------------
  Macro for messages with 1 parameters.
---------------------------------------------------------------------------*/    
#define DATA_RTP_MSG1(xx_ss_mask, xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS_RTP, xx_ss_mask, xx_fmt, xx_arg1)

/*---------------------------------------------------------------------------
  Macro for messages with 2 parameters.
---------------------------------------------------------------------------*/
#define DATA_RTP_MSG2(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS_RTP, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2)

/*---------------------------------------------------------------------------
  Macro for messages with 3 parameters.
---------------------------------------------------------------------------*/
#define DATA_RTP_MSG3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS_RTP, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

/*---------------------------------------------------------------------------
  This is the macro for messages with 4 parameters.
---------------------------------------------------------------------------*/
#define DATA_RTP_MSG4(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4) \
  MSG_4(MSG_SSID_DS_RTP, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4)

/*---------------------------------------------------------------------------
  This is the macro for messages with 5 parameters.
---------------------------------------------------------------------------*/
#define DATA_RTP_MSG5(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                                               xx_arg5) \
  MSG_5(MSG_SSID_DS_RTP, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5)

/*---------------------------------------------------------------------------
  This is the macro for messages with 6 parameters 
---------------------------------------------------------------------------*/
#define DATA_RTP_MSG6(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                                          xx_arg5, xx_arg6) \
  MSG_6(MSG_SSID_DS_RTP, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6)

/*---------------------------------------------------------------------------
  This is the macro for messages with 7 parameters 
---------------------------------------------------------------------------*/
#define DATA_RTP_MSG7(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                                 xx_arg5, xx_arg6, xx_arg7) \
  MSG_7(MSG_SSID_DS_RTP, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6, xx_arg7)

/*---------------------------------------------------------------------------
  This is the macro for messages with 8 parameters 
---------------------------------------------------------------------------*/
#define DATA_RTP_MSG8(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                        xx_arg5, xx_arg6, xx_arg7, xx_arg8) \
  MSG_8(MSG_SSID_DS_RTP, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6, xx_arg7, xx_arg8)

/*---------------------------------------------------------------------------
  This is the macro for messages with 9 parameters 
---------------------------------------------------------------------------*/
#define DATA_RTP_MSG9(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                               xx_arg5, xx_arg6, xx_arg7, xx_arg8, xx_arg9) \
  MSG_9(MSG_SSID_DS_RTP, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6, xx_arg7, xx_arg8, xx_arg9)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 1 parameters
---------------------------------------------------------------------------*/
#define DATA_RTP_MSG_SPRINTF_1(xx_ss_mask, xx_fmt, xx_arg1) \
  MSG_SPRINTF_1(MSG_SSID_DS_RTP, xx_ss_mask, xx_fmt, xx_arg1)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 2 parameters
---------------------------------------------------------------------------*/
#define DATA_RTP_MSG_SPRINTF_2(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2) \
  MSG_SPRINTF_2(MSG_SSID_DS_RTP, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 3 parameters
---------------------------------------------------------------------------*/
#define DATA_RTP_MSG_SPRINTF_3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_SPRINTF_3(MSG_SSID_DS_RTP, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 3 parameters. This Macro is 
  used when xx_fmt is passed at runtime.
---------------------------------------------------------------------------*/
#define DATA_RTP_MSG_SPRINTF_FMT_VAR_3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_SPRINTF_FMT_VAR_3(MSG_SSID_DS_RTP, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3)


//IGT
/*===========================================================================
  RTP SS related convenience macros. These will substitute the SSID as
  well as the desired sub-group ID.
===========================================================================*/

/*---------------------------------------------------------------------------
  Macro for messages with no parameters.
---------------------------------------------------------------------------*/
#define DATA_RTP_MSG0_GEN(xx_fmt) \
  MSG(MSG_SSID_DS_RTP, DATA_RTP_GENERAL, xx_fmt)
  
#define DATA_RTP_MSG0_DBG(xx_fmt) \
  MSG(MSG_SSID_DS_RTP, DATA_RTP_DEBUG, xx_fmt)
  
#define DATA_RTP_MSG0_TRC(xx_fmt) \
  MSG(MSG_SSID_DS_RTP, DATA_RTP_TRACE, xx_fmt)

/*---------------------------------------------------------------------------
  Macro for messages with 1 parameters.
---------------------------------------------------------------------------*/    
#define DATA_RTP_MSG1_GEN(xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS_RTP, DATA_RTP_GENERAL, xx_fmt, xx_arg1)

#define DATA_RTP_MSG1_DBG(xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS_RTP, DATA_RTP_DEBUG, xx_fmt, xx_arg1)
  
#define DATA_RTP_MSG1_TRC(xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS_RTP, DATA_RTP_TRACE, xx_fmt, xx_arg1)

/*---------------------------------------------------------------------------
  Macro for messages with 2 parameters.
---------------------------------------------------------------------------*/
#define DATA_RTP_MSG2_GEN(xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS_RTP, DATA_RTP_GENERAL, xx_fmt, xx_arg1, xx_arg2)

#define DATA_RTP_MSG2_DBG(xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS_RTP, DATA_RTP_DEBUG, xx_fmt, xx_arg1, xx_arg2)

#define DATA_RTP_MSG2_TRC(xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS_RTP, DATA_RTP_TRACE, xx_fmt, xx_arg1, xx_arg2)

/*---------------------------------------------------------------------------
  Macro for messages with 3 parameters.
---------------------------------------------------------------------------*/
#define DATA_RTP_MSG3_GEN(xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS_RTP, DATA_RTP_GENERAL, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

#define DATA_RTP_MSG3_DBG(xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS_RTP, DATA_RTP_DEBUG, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

#define DATA_RTP_MSG3_TRC(xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS_RTP, DATA_RTP_TRACE, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

/*===========================================================================
  SIP SS related macros
===========================================================================*/


/*---------------------------------------------------------------------------
  Macro for messages with no parameters.
---------------------------------------------------------------------------*/
#define DATA_SIPSTACK_MSG0(xx_ss_mask, xx_fmt) \
  MSG(MSG_SSID_DS_SIPSTACK, xx_ss_mask, xx_fmt)  

/*---------------------------------------------------------------------------
  Macro for messages with 1 parameters.
---------------------------------------------------------------------------*/    
#define DATA_SIPSTACK_MSG1(xx_ss_mask, xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS_SIPSTACK, xx_ss_mask, xx_fmt, xx_arg1)

/*---------------------------------------------------------------------------
  Macro for messages with 2 parameters.
---------------------------------------------------------------------------*/
#define DATA_SIPSTACK_MSG2(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS_SIPSTACK, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2)

/*---------------------------------------------------------------------------
  Macro for messages with 3 parameters.
---------------------------------------------------------------------------*/
#define DATA_SIPSTACK_MSG3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS_SIPSTACK, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

/*---------------------------------------------------------------------------
  This is the macro for messages with 4 parameters.
---------------------------------------------------------------------------*/
#define DATA_SIPSTACK_MSG4(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4) \
  MSG_4(MSG_SSID_DS_SIPSTACK, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4)

/*---------------------------------------------------------------------------
  This is the macro for messages with 5 parameters.
---------------------------------------------------------------------------*/
#define DATA_SIPSTACK_MSG5(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                                               xx_arg5) \
  MSG_5(MSG_SSID_DS_SIPSTACK, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5)

/*---------------------------------------------------------------------------
  This is the macro for messages with 6 parameters 
---------------------------------------------------------------------------*/
#define DATA_SIPSTACK_MSG6(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                                          xx_arg5, xx_arg6) \
  MSG_6(MSG_SSID_DS_SIPSTACK, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6)

/*---------------------------------------------------------------------------
  This is the macro for messages with 7 parameters 
---------------------------------------------------------------------------*/
#define DATA_SIPSTACK_MSG7(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                                 xx_arg5, xx_arg6, xx_arg7) \
  MSG_7(MSG_SSID_DS_SIPSTACK, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6, xx_arg7)

/*---------------------------------------------------------------------------
  This is the macro for messages with 8 parameters 
---------------------------------------------------------------------------*/
#define DATA_SIPSTACK_MSG8(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                        xx_arg5, xx_arg6, xx_arg7, xx_arg8) \
  MSG_8(MSG_SSID_DS_SIPSTACK, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6, xx_arg7, xx_arg8)

/*---------------------------------------------------------------------------
  This is the macro for messages with 9 parameters 
---------------------------------------------------------------------------*/
#define DATA_SIPSTACK_MSG9(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                               xx_arg5, xx_arg6, xx_arg7, xx_arg8, xx_arg9) \
  MSG_9(MSG_SSID_DS_SIPSTACK, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6, xx_arg7, xx_arg8, xx_arg9)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 1 parameters
---------------------------------------------------------------------------*/
#define DATA_SIPSTACK_MSG_SPRINTF_1(xx_ss_mask, xx_fmt, xx_arg1) \
  MSG_SPRINTF_1(MSG_SSID_DS_SIPSTACK, xx_ss_mask, xx_fmt, xx_arg1)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 2 parameters
---------------------------------------------------------------------------*/
#define DATA_SIPSTACK_MSG_SPRINTF_2(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2) \
  MSG_SPRINTF_2(MSG_SSID_DS_SIPSTACK, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 3 parameters
---------------------------------------------------------------------------*/
#define DATA_SIPSTACK_MSG_SPRINTF_3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_SPRINTF_3(MSG_SSID_DS_SIPSTACK, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 3 parameters. This Macro is 
  used when xx_fmt is passed at runtime.
---------------------------------------------------------------------------*/
#define DATA_SIPSTACK_MSG_SPRINTF_FMT_VAR_3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_SPRINTF_FMT_VAR_3(MSG_SSID_DS_SIPSTACK, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3)


//IGT
/*===========================================================================
  SIPSTACK SS related convenience macros. These will substitute the SSID as
  well as the desired sub-group ID.
===========================================================================*/

/*---------------------------------------------------------------------------
  Macro for messages with no parameters.
---------------------------------------------------------------------------*/
#define DATA_SIPSTACK_MSG0_GEN(xx_fmt) \
  MSG(MSG_SSID_DS_SIPSTACK, DATA_SIPSTACK_GENERAL, xx_fmt)
  
#define DATA_SIPSTACK_MSG0_DBG(xx_fmt) \
  MSG(MSG_SSID_DS_SIPSTACK, DATA_SIPSTACK_DEBUG, xx_fmt)
  
#define DATA_SIPSTACK_MSG0_TRC(xx_fmt) \
  MSG(MSG_SSID_DS_SIPSTACK, DATA_SIPSTACK_TRACE, xx_fmt)

/*---------------------------------------------------------------------------
  Macro for messages with 1 parameters.
---------------------------------------------------------------------------*/    
#define DATA_SIPSTACK_MSG1_GEN(xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS_SIPSTACK, DATA_SIPSTACK_GENERAL, xx_fmt, xx_arg1)

#define DATA_SIPSTACK_MSG1_DBG(xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS_SIPSTACK, DATA_SIPSTACK_DEBUG, xx_fmt, xx_arg1)
  
#define DATA_SIPSTACK_MSG1_TRC(xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS_SIPSTACK, DATA_SIPSTACK_TRACE, xx_fmt, xx_arg1)

/*---------------------------------------------------------------------------
  Macro for messages with 2 parameters.
---------------------------------------------------------------------------*/
#define DATA_SIPSTACK_MSG2_GEN(xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS_SIPSTACK, DATA_SIPSTACK_GENERAL, xx_fmt, xx_arg1, xx_arg2)

#define DATA_SIPSTACK_MSG2_DBG(xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS_SIPSTACK, DATA_SIPSTACK_DEBUG, xx_fmt, xx_arg1, xx_arg2)

#define DATA_SIPSTACK_MSG2_TRC(xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS_SIPSTACK, DATA_SIPSTACK_TRACE, xx_fmt, xx_arg1, xx_arg2)

/*---------------------------------------------------------------------------
  Macro for messages with 3 parameters.
---------------------------------------------------------------------------*/
#define DATA_SIPSTACK_MSG3_GEN(xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS_SIPSTACK, DATA_SIPSTACK_GENERAL, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

#define DATA_SIPSTACK_MSG3_DBG(xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS_SIPSTACK, DATA_SIPSTACK_DEBUG, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

#define DATA_SIPSTACK_MSG3_TRC(xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS_SIPSTACK, DATA_SIPSTACK_TRACE, xx_fmt, xx_arg1, xx_arg2, xx_arg3)
    
/*===========================================================================
  ROHC SS related macros
===========================================================================*/


/*---------------------------------------------------------------------------
  Macro for messages with no parameters.
---------------------------------------------------------------------------*/
#define DATA_ROHC_MSG0(xx_ss_mask, xx_fmt) \
  MSG(MSG_SSID_DS_ROHC, xx_ss_mask, xx_fmt)  

/*---------------------------------------------------------------------------
  Macro for messages with 1 parameters.
---------------------------------------------------------------------------*/    
#define DATA_ROHC_MSG1(xx_ss_mask, xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS_ROHC, xx_ss_mask, xx_fmt, xx_arg1)

/*---------------------------------------------------------------------------
  Macro for messages with 2 parameters.
---------------------------------------------------------------------------*/
#define DATA_ROHC_MSG2(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS_ROHC, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2)

/*---------------------------------------------------------------------------
  Macro for messages with 3 parameters.
---------------------------------------------------------------------------*/
#define DATA_ROHC_MSG3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS_ROHC, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

/*---------------------------------------------------------------------------
  This is the macro for messages with 4 parameters.
---------------------------------------------------------------------------*/
#define DATA_ROHC_MSG4(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4) \
  MSG_4(MSG_SSID_DS_ROHC, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4)

/*---------------------------------------------------------------------------
  This is the macro for messages with 5 parameters.
---------------------------------------------------------------------------*/
#define DATA_ROHC_MSG5(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                                               xx_arg5) \
  MSG_5(MSG_SSID_DS_ROHC, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5)

/*---------------------------------------------------------------------------
  This is the macro for messages with 6 parameters 
---------------------------------------------------------------------------*/
#define DATA_ROHC_MSG6(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                                          xx_arg5, xx_arg6) \
  MSG_6(MSG_SSID_DS_ROHC, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6)

/*---------------------------------------------------------------------------
  This is the macro for messages with 7 parameters 
---------------------------------------------------------------------------*/
#define DATA_ROHC_MSG7(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                                 xx_arg5, xx_arg6, xx_arg7) \
  MSG_7(MSG_SSID_DS_ROHC, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6, xx_arg7)

/*---------------------------------------------------------------------------
  This is the macro for messages with 8 parameters 
---------------------------------------------------------------------------*/
#define DATA_ROHC_MSG8(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                        xx_arg5, xx_arg6, xx_arg7, xx_arg8) \
  MSG_8(MSG_SSID_DS_ROHC, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6, xx_arg7, xx_arg8)

/*---------------------------------------------------------------------------
  This is the macro for messages with 9 parameters 
---------------------------------------------------------------------------*/
#define DATA_ROHC_MSG9(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                               xx_arg5, xx_arg6, xx_arg7, xx_arg8, xx_arg9) \
  MSG_9(MSG_SSID_DS_ROHC, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6, xx_arg7, xx_arg8, xx_arg9)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 1 parameters
---------------------------------------------------------------------------*/
#define DATA_ROHC_MSG_SPRINTF_1(xx_ss_mask, xx_fmt, xx_arg1) \
  MSG_SPRINTF_1(MSG_SSID_DS_ROHC, xx_ss_mask, xx_fmt, xx_arg1)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 2 parameters
---------------------------------------------------------------------------*/
#define DATA_ROHC_MSG_SPRINTF_2(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2) \
  MSG_SPRINTF_2(MSG_SSID_DS_ROHC, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 3 parameters
---------------------------------------------------------------------------*/
#define DATA_ROHC_MSG_SPRINTF_3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_SPRINTF_3(MSG_SSID_DS_ROHC, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 3 parameters. This Macro is 
  used when xx_fmt is passed at runtime.
---------------------------------------------------------------------------*/
#define DATA_ROHC_MSG_SPRINTF_FMT_VAR_3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_SPRINTF_FMT_VAR_3(MSG_SSID_DS_ROHC, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3)


//IGT
/*===========================================================================
  ROHC SS related convenience macros. These will substitute the SSID as
  well as the desired sub-group ID.
===========================================================================*/

/*---------------------------------------------------------------------------
  Macro for messages with no parameters.
---------------------------------------------------------------------------*/
#define DATA_ROHC_MSG0_GEN(xx_fmt) \
  MSG(MSG_SSID_DS_ROHC, DATA_ROHC_GENERAL, xx_fmt)
  
#define DATA_ROHC_MSG0_DBG(xx_fmt) \
  MSG(MSG_SSID_DS_ROHC, DATA_ROHC_DEBUG, xx_fmt)
  
#define DATA_ROHC_MSG0_TRC(xx_fmt) \
  MSG(MSG_SSID_DS_ROHC, DATA_ROHC_TRACE, xx_fmt)

/*---------------------------------------------------------------------------
  Macro for messages with 1 parameters.
---------------------------------------------------------------------------*/    
#define DATA_ROHC_MSG1_GEN(xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS_ROHC, DATA_ROHC_GENERAL, xx_fmt, xx_arg1)

#define DATA_ROHC_MSG1_DBG(xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS_ROHC, DATA_ROHC_DEBUG, xx_fmt, xx_arg1)
  
#define DATA_ROHC_MSG1_TRC(xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS_ROHC, DATA_ROHC_TRACE, xx_fmt, xx_arg1)

/*---------------------------------------------------------------------------
  Macro for messages with 2 parameters.
---------------------------------------------------------------------------*/
#define DATA_ROHC_MSG2_GEN(xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS_ROHC, DATA_ROHC_GENERAL, xx_fmt, xx_arg1, xx_arg2)

#define DATA_ROHC_MSG2_DBG(xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS_ROHC, DATA_ROHC_DEBUG, xx_fmt, xx_arg1, xx_arg2)

#define DATA_ROHC_MSG2_TRC(xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS_ROHC, DATA_ROHC_TRACE, xx_fmt, xx_arg1, xx_arg2)

/*---------------------------------------------------------------------------
  Macro for messages with 3 parameters.
---------------------------------------------------------------------------*/
#define DATA_ROHC_MSG3_GEN(xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS_ROHC, DATA_ROHC_GENERAL, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

#define DATA_ROHC_MSG3_DBG(xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS_ROHC, DATA_ROHC_DEBUG, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

#define DATA_ROHC_MSG3_TRC(xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS_ROHC, DATA_ROHC_TRACE, xx_fmt, xx_arg1, xx_arg2, xx_arg3)
    
/*===========================================================================
  DOQOS SS related macros
===========================================================================*/


/*---------------------------------------------------------------------------
  Macro for messages with no parameters.
---------------------------------------------------------------------------*/
#define DATA_DOQOS_MSG0(xx_ss_mask, xx_fmt) \
  MSG(MSG_SSID_DS_DOQOS, xx_ss_mask, xx_fmt)  

/*---------------------------------------------------------------------------
  Macro for messages with 1 parameters.
---------------------------------------------------------------------------*/    
#define DATA_DOQOS_MSG1(xx_ss_mask, xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS_DOQOS, xx_ss_mask, xx_fmt, xx_arg1)

/*---------------------------------------------------------------------------
  Macro for messages with 2 parameters.
---------------------------------------------------------------------------*/
#define DATA_DOQOS_MSG2(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS_DOQOS, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2)

/*---------------------------------------------------------------------------
  Macro for messages with 3 parameters.
---------------------------------------------------------------------------*/
#define DATA_DOQOS_MSG3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS_DOQOS, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

/*---------------------------------------------------------------------------
  This is the macro for messages with 4 parameters.
---------------------------------------------------------------------------*/
#define DATA_DOQOS_MSG4(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4) \
  MSG_4(MSG_SSID_DS_DOQOS, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4)

/*---------------------------------------------------------------------------
  This is the macro for messages with 5 parameters.
---------------------------------------------------------------------------*/
#define DATA_DOQOS_MSG5(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                                               xx_arg5) \
  MSG_5(MSG_SSID_DS_DOQOS, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5)

/*---------------------------------------------------------------------------
  This is the macro for messages with 6 parameters 
---------------------------------------------------------------------------*/
#define DATA_DOQOS_MSG6(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                                          xx_arg5, xx_arg6) \
  MSG_6(MSG_SSID_DS_DOQOS, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6)

/*---------------------------------------------------------------------------
  This is the macro for messages with 7 parameters 
---------------------------------------------------------------------------*/
#define DATA_DOQOS_MSG7(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                                 xx_arg5, xx_arg6, xx_arg7) \
  MSG_7(MSG_SSID_DS_DOQOS, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6, xx_arg7)

/*---------------------------------------------------------------------------
  This is the macro for messages with 8 parameters 
---------------------------------------------------------------------------*/
#define DATA_DOQOS_MSG8(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                        xx_arg5, xx_arg6, xx_arg7, xx_arg8) \
  MSG_8(MSG_SSID_DS_DOQOS, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6, xx_arg7, xx_arg8)

/*---------------------------------------------------------------------------
  This is the macro for messages with 9 parameters 
---------------------------------------------------------------------------*/
#define DATA_DOQOS_MSG9(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                               xx_arg5, xx_arg6, xx_arg7, xx_arg8, xx_arg9) \
  MSG_9(MSG_SSID_DS_DOQOS, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6, xx_arg7, xx_arg8, xx_arg9)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 1 parameters
---------------------------------------------------------------------------*/
#define DATA_DOQOS_MSG_SPRINTF_1(xx_ss_mask, xx_fmt, xx_arg1) \
  MSG_SPRINTF_1(MSG_SSID_DS_DOQOS, xx_ss_mask, xx_fmt, xx_arg1)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 2 parameters
---------------------------------------------------------------------------*/
#define DATA_DOQOS_MSG_SPRINTF_2(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2) \
  MSG_SPRINTF_2(MSG_SSID_DS_DOQOS, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 3 parameters
---------------------------------------------------------------------------*/
#define DATA_DOQOS_MSG_SPRINTF_3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_SPRINTF_3(MSG_SSID_DS_DOQOS, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 3 parameters. This Macro is 
  used when xx_fmt is passed at runtime.
---------------------------------------------------------------------------*/
#define DATA_DOQOS_MSG_SPRINTF_FMT_VAR_3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_SPRINTF_FMT_VAR_3(MSG_SSID_DS_DOQOS, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3)


//IGT
/*===========================================================================
  DOQOS SS related convenience macros. These will substitute the SSID as
  well as the desired sub-group ID.
===========================================================================*/

/*---------------------------------------------------------------------------
  Macro for messages with no parameters.
---------------------------------------------------------------------------*/
#define DATA_DOQOS_MSG0_GEN(xx_fmt) \
  MSG(MSG_SSID_DS_DOQOS, DATA_DOQOS_GENERAL, xx_fmt)
  
#define DATA_DOQOS_MSG0_DBG(xx_fmt) \
  MSG(MSG_SSID_DS_DOQOS, DATA_DOQOS_DEBUG, xx_fmt)
  
#define DATA_DOQOS_MSG0_TRC(xx_fmt) \
  MSG(MSG_SSID_DS_DOQOS, DATA_DOQOS_TRACE, xx_fmt)

/*---------------------------------------------------------------------------
  Macro for messages with 1 parameters.
---------------------------------------------------------------------------*/    
#define DATA_DOQOS_MSG1_GEN(xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS_DOQOS, DATA_DOQOS_GENERAL, xx_fmt, xx_arg1)

#define DATA_DOQOS_MSG1_DBG(xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS_DOQOS, DATA_DOQOS_DEBUG, xx_fmt, xx_arg1)
  
#define DATA_DOQOS_MSG1_TRC(xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS_DOQOS, DATA_DOQOS_TRACE, xx_fmt, xx_arg1)

/*---------------------------------------------------------------------------
  Macro for messages with 2 parameters.
---------------------------------------------------------------------------*/
#define DATA_DOQOS_MSG2_GEN(xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS_DOQOS, DATA_DOQOS_GENERAL, xx_fmt, xx_arg1, xx_arg2)

#define DATA_DOQOS_MSG2_DBG(xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS_DOQOS, DATA_DOQOS_DEBUG, xx_fmt, xx_arg1, xx_arg2)

#define DATA_DOQOS_MSG2_TRC(xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS_DOQOS, DATA_DOQOS_TRACE, xx_fmt, xx_arg1, xx_arg2)

/*---------------------------------------------------------------------------
  Macro for messages with 3 parameters.
---------------------------------------------------------------------------*/
#define DATA_DOQOS_MSG3_GEN(xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS_DOQOS, DATA_DOQOS_GENERAL, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

#define DATA_DOQOS_MSG3_DBG(xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS_DOQOS, DATA_DOQOS_DEBUG, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

#define DATA_DOQOS_MSG3_TRC(xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS_DOQOS, DATA_DOQOS_TRACE, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

    
    
/*===========================================================================
  IPC SS related macros
===========================================================================*/


/*---------------------------------------------------------------------------
  Macro for messages with no parameters.
---------------------------------------------------------------------------*/
#define DATA_IPC_MSG0(xx_ss_mask, xx_fmt) \
  MSG(MSG_SSID_DS_IPC, xx_ss_mask, xx_fmt)  

/*---------------------------------------------------------------------------
  Macro for messages with 1 parameters.
---------------------------------------------------------------------------*/    
#define DATA_IPC_MSG1(xx_ss_mask, xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS_IPC, xx_ss_mask, xx_fmt, xx_arg1)

/*---------------------------------------------------------------------------
  Macro for messages with 2 parameters.
---------------------------------------------------------------------------*/
#define DATA_IPC_MSG2(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS_IPC, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2)

/*---------------------------------------------------------------------------
  Macro for messages with 3 parameters.
---------------------------------------------------------------------------*/
#define DATA_IPC_MSG3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS_IPC, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

/*---------------------------------------------------------------------------
  This is the macro for messages with 4 parameters.
---------------------------------------------------------------------------*/
#define DATA_IPC_MSG4(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4) \
  MSG_4(MSG_SSID_DS_IPC, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4)

/*---------------------------------------------------------------------------
  This is the macro for messages with 5 parameters.
---------------------------------------------------------------------------*/
#define DATA_IPC_MSG5(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                                               xx_arg5) \
  MSG_5(MSG_SSID_DS_IPC, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5)

/*---------------------------------------------------------------------------
  This is the macro for messages with 6 parameters 
---------------------------------------------------------------------------*/
#define DATA_IPC_MSG6(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                                          xx_arg5, xx_arg6) \
  MSG_6(MSG_SSID_DS_IPC, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6)

/*---------------------------------------------------------------------------
  This is the macro for messages with 7 parameters 
---------------------------------------------------------------------------*/
#define DATA_IPC_MSG7(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                                 xx_arg5, xx_arg6, xx_arg7) \
  MSG_7(MSG_SSID_DS_IPC, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6, xx_arg7)

/*---------------------------------------------------------------------------
  This is the macro for messages with 8 parameters 
---------------------------------------------------------------------------*/
#define DATA_IPC_MSG8(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                        xx_arg5, xx_arg6, xx_arg7, xx_arg8) \
  MSG_8(MSG_SSID_DS_IPC, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6, xx_arg7, xx_arg8)

/*---------------------------------------------------------------------------
  This is the macro for messages with 9 parameters 
---------------------------------------------------------------------------*/
#define DATA_IPC_MSG9(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                               xx_arg5, xx_arg6, xx_arg7, xx_arg8, xx_arg9) \
  MSG_9(MSG_SSID_DS_IPC, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6, xx_arg7, xx_arg8, xx_arg9)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 1 parameters
---------------------------------------------------------------------------*/
#define DATA_IPC_MSG_SPRINTF_1(xx_ss_mask, xx_fmt, xx_arg1) \
  MSG_SPRINTF_1(MSG_SSID_DS_IPC, xx_ss_mask, xx_fmt, xx_arg1)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 2 parameters
---------------------------------------------------------------------------*/
#define DATA_IPC_MSG_SPRINTF_2(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2) \
  MSG_SPRINTF_2(MSG_SSID_DS_IPC, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 3 parameters
---------------------------------------------------------------------------*/
#define DATA_IPC_MSG_SPRINTF_3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_SPRINTF_3(MSG_SSID_DS_IPC, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 3 parameters. This Macro is 
  used when xx_fmt is passed at runtime.
---------------------------------------------------------------------------*/
#define DATA_IPC_MSG_SPRINTF_FMT_VAR_3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_SPRINTF_FMT_VAR_3(MSG_SSID_DS_IPC, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3)


//IGT
/*===========================================================================
  IPC SS related convenience macros. These will substitute the SSID as
  well as the desired sub-group ID.
===========================================================================*/

/*---------------------------------------------------------------------------
  Macro for messages with no parameters.
---------------------------------------------------------------------------*/
#define DATA_IPC_MSG0_GEN(xx_fmt) \
  MSG(MSG_SSID_DS_IPC, DATA_IPC_GENERAL, xx_fmt)
  
#define DATA_IPC_MSG0_DBG(xx_fmt) \
  MSG(MSG_SSID_DS_IPC, DATA_IPC_DEBUG, xx_fmt)
  
#define DATA_IPC_MSG0_TRC(xx_fmt) \
  MSG(MSG_SSID_DS_IPC, DATA_IPC_TRACE, xx_fmt)

/*---------------------------------------------------------------------------
  Macro for messages with 1 parameters.
---------------------------------------------------------------------------*/    
#define DATA_IPC_MSG1_GEN(xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS_IPC, DATA_IPC_GENERAL, xx_fmt, xx_arg1)

#define DATA_IPC_MSG1_DBG(xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS_IPC, DATA_IPC_DEBUG, xx_fmt, xx_arg1)
  
#define DATA_IPC_MSG1_TRC(xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS_IPC, DATA_IPC_TRACE, xx_fmt, xx_arg1)

/*---------------------------------------------------------------------------
  Macro for messages with 2 parameters.
---------------------------------------------------------------------------*/
#define DATA_IPC_MSG2_GEN(xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS_IPC, DATA_IPC_GENERAL, xx_fmt, xx_arg1, xx_arg2)

#define DATA_IPC_MSG2_DBG(xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS_IPC, DATA_IPC_DEBUG, xx_fmt, xx_arg1, xx_arg2)

#define DATA_IPC_MSG2_TRC(xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS_IPC, DATA_IPC_TRACE, xx_fmt, xx_arg1, xx_arg2)

/*---------------------------------------------------------------------------
  Macro for messages with 3 parameters.
---------------------------------------------------------------------------*/
#define DATA_IPC_MSG3_GEN(xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS_IPC, DATA_IPC_GENERAL, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

#define DATA_IPC_MSG3_DBG(xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS_IPC, DATA_IPC_DEBUG, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

#define DATA_IPC_MSG3_TRC(xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS_IPC, DATA_IPC_TRACE, xx_fmt, xx_arg1, xx_arg2, xx_arg3)
 
    
    
/*===========================================================================
  SHIM SS related macros
===========================================================================*/


/*---------------------------------------------------------------------------
  Macro for messages with no parameters.
---------------------------------------------------------------------------*/
#define DATA_SHIM_MSG0(xx_ss_mask, xx_fmt) \
  MSG(MSG_SSID_DS_SHIM, xx_ss_mask, xx_fmt)  

/*---------------------------------------------------------------------------
  Macro for messages with 1 parameters.
---------------------------------------------------------------------------*/    
#define DATA_SHIM_MSG1(xx_ss_mask, xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS_SHIM, xx_ss_mask, xx_fmt, xx_arg1)

/*---------------------------------------------------------------------------
  Macro for messages with 2 parameters.
---------------------------------------------------------------------------*/
#define DATA_SHIM_MSG2(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS_SHIM, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2)

/*---------------------------------------------------------------------------
  Macro for messages with 3 parameters.
---------------------------------------------------------------------------*/
#define DATA_SHIM_MSG3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS_SHIM, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

/*---------------------------------------------------------------------------
  This is the macro for messages with 4 parameters.
---------------------------------------------------------------------------*/
#define DATA_SHIM_MSG4(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4) \
  MSG_4(MSG_SSID_DS_SHIM, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4)

/*---------------------------------------------------------------------------
  This is the macro for messages with 5 parameters.
---------------------------------------------------------------------------*/
#define DATA_SHIM_MSG5(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                                               xx_arg5) \
  MSG_5(MSG_SSID_DS_SHIM, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5)

/*---------------------------------------------------------------------------
  This is the macro for messages with 6 parameters 
---------------------------------------------------------------------------*/
#define DATA_SHIM_MSG6(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                                          xx_arg5, xx_arg6) \
  MSG_6(MSG_SSID_DS_SHIM, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6)

/*---------------------------------------------------------------------------
  This is the macro for messages with 7 parameters 
---------------------------------------------------------------------------*/
#define DATA_SHIM_MSG7(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                                 xx_arg5, xx_arg6, xx_arg7) \
  MSG_7(MSG_SSID_DS_SHIM, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6, xx_arg7)

/*---------------------------------------------------------------------------
  This is the macro for messages with 8 parameters 
---------------------------------------------------------------------------*/
#define DATA_SHIM_MSG8(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                        xx_arg5, xx_arg6, xx_arg7, xx_arg8) \
  MSG_8(MSG_SSID_DS_SHIM, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6, xx_arg7, xx_arg8)

/*---------------------------------------------------------------------------
  This is the macro for messages with 9 parameters 
---------------------------------------------------------------------------*/
#define DATA_SHIM_MSG9(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                               xx_arg5, xx_arg6, xx_arg7, xx_arg8, xx_arg9) \
  MSG_9(MSG_SSID_DS_SHIM, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6, xx_arg7, xx_arg8, xx_arg9)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 1 parameters
---------------------------------------------------------------------------*/
#define DATA_SHIM_MSG_SPRINTF_1(xx_ss_mask, xx_fmt, xx_arg1) \
  MSG_SPRINTF_1(MSG_SSID_DS_SHIM, xx_ss_mask, xx_fmt, xx_arg1)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 2 parameters
---------------------------------------------------------------------------*/
#define DATA_SHIM_MSG_SPRINTF_2(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2) \
  MSG_SPRINTF_2(MSG_SSID_DS_SHIM, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 3 parameters
---------------------------------------------------------------------------*/
#define DATA_SHIM_MSG_SPRINTF_3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_SPRINTF_3(MSG_SSID_DS_SHIM, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 3 parameters. This Macro is 
  used when xx_fmt is passed at runtime.
---------------------------------------------------------------------------*/
#define DATA_SHIM_MSG_SPRINTF_FMT_VAR_3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_SPRINTF_FMT_VAR_3(MSG_SSID_DS_SHIM, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3)


//IGT
/*===========================================================================
  SHIM SS related convenience macros. These will substitute the SSID as
  well as the desired sub-group ID.
===========================================================================*/

/*---------------------------------------------------------------------------
  Macro for messages with no parameters.
---------------------------------------------------------------------------*/
#define DATA_SHIM_MSG0_GEN(xx_fmt) \
  MSG(MSG_SSID_DS_SHIM, DATA_SHIM_GENERAL, xx_fmt)
  
#define DATA_SHIM_MSG0_DBG(xx_fmt) \
  MSG(MSG_SSID_DS_SHIM, DATA_SHIM_DEBUG, xx_fmt)
  
#define DATA_SHIM_MSG0_TRC(xx_fmt) \
  MSG(MSG_SSID_DS_SHIM, DATA_SHIM_TRACE, xx_fmt)

/*---------------------------------------------------------------------------
  Macro for messages with 1 parameters.
---------------------------------------------------------------------------*/    
#define DATA_SHIM_MSG1_GEN(xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS_SHIM, DATA_SHIM_GENERAL, xx_fmt, xx_arg1)

#define DATA_SHIM_MSG1_DBG(xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS_SHIM, DATA_SHIM_DEBUG, xx_fmt, xx_arg1)
  
#define DATA_SHIM_MSG1_TRC(xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS_SHIM, DATA_SHIM_TRACE, xx_fmt, xx_arg1)

/*---------------------------------------------------------------------------
  Macro for messages with 2 parameters.
---------------------------------------------------------------------------*/
#define DATA_SHIM_MSG2_GEN(xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS_SHIM, DATA_SHIM_GENERAL, xx_fmt, xx_arg1, xx_arg2)

#define DATA_SHIM_MSG2_DBG(xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS_SHIM, DATA_SHIM_DEBUG, xx_fmt, xx_arg1, xx_arg2)

#define DATA_SHIM_MSG2_TRC(xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS_SHIM, DATA_SHIM_TRACE, xx_fmt, xx_arg1, xx_arg2)

/*---------------------------------------------------------------------------
  Macro for messages with 3 parameters.
---------------------------------------------------------------------------*/
#define DATA_SHIM_MSG3_GEN(xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS_SHIM, DATA_SHIM_GENERAL, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

#define DATA_SHIM_MSG3_DBG(xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS_SHIM, DATA_SHIM_DEBUG, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

#define DATA_SHIM_MSG3_TRC(xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS_SHIM, DATA_SHIM_TRACE, xx_fmt, xx_arg1, xx_arg2, xx_arg3)
  
/*===========================================================================
  HDR SS related macros
===========================================================================*/


/*---------------------------------------------------------------------------
  Macro for messages with no parameters.
---------------------------------------------------------------------------*/
#define DATA_HDR_MSG0(xx_ss_mask, xx_fmt) \
  MSG(MSG_SSID_DS_HDR, xx_ss_mask, xx_fmt)  

/*---------------------------------------------------------------------------
  Macro for messages with 1 parameters.
---------------------------------------------------------------------------*/    
#define DATA_HDR_MSG1(xx_ss_mask, xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS_HDR, xx_ss_mask, xx_fmt, xx_arg1)

/*---------------------------------------------------------------------------
  Macro for messages with 2 parameters.
---------------------------------------------------------------------------*/
#define DATA_HDR_MSG2(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS_HDR, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2)

/*---------------------------------------------------------------------------
  Macro for messages with 3 parameters.
---------------------------------------------------------------------------*/
#define DATA_HDR_MSG3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS_HDR, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

/*---------------------------------------------------------------------------
  This is the macro for messages with 4 parameters.
---------------------------------------------------------------------------*/
#define DATA_HDR_MSG4(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4) \
  MSG_4(MSG_SSID_DS_HDR, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4)

/*---------------------------------------------------------------------------
  This is the macro for messages with 5 parameters.
---------------------------------------------------------------------------*/
#define DATA_HDR_MSG5(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                                               xx_arg5) \
  MSG_5(MSG_SSID_DS_HDR, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5)

/*---------------------------------------------------------------------------
  This is the macro for messages with 6 parameters 
---------------------------------------------------------------------------*/
#define DATA_HDR_MSG6(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                                          xx_arg5, xx_arg6) \
  MSG_6(MSG_SSID_DS_HDR, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6)

/*---------------------------------------------------------------------------
  This is the macro for messages with 7 parameters 
---------------------------------------------------------------------------*/
#define DATA_HDR_MSG7(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                                 xx_arg5, xx_arg6, xx_arg7) \
  MSG_7(MSG_SSID_DS_HDR, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6, xx_arg7)

/*---------------------------------------------------------------------------
  This is the macro for messages with 8 parameters 
---------------------------------------------------------------------------*/
#define DATA_HDR_MSG8(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                        xx_arg5, xx_arg6, xx_arg7, xx_arg8) \
  MSG_8(MSG_SSID_DS_HDR, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6, xx_arg7, xx_arg8)

/*---------------------------------------------------------------------------
  This is the macro for messages with 9 parameters 
---------------------------------------------------------------------------*/
#define DATA_HDR_MSG9(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                               xx_arg5, xx_arg6, xx_arg7, xx_arg8, xx_arg9) \
  MSG_9(MSG_SSID_DS_HDR, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6, xx_arg7, xx_arg8, xx_arg9)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 1 parameters
---------------------------------------------------------------------------*/
#define DATA_HDR_MSG_SPRINTF_1(xx_ss_mask, xx_fmt, xx_arg1) \
  MSG_SPRINTF_1(MSG_SSID_DS_HDR, xx_ss_mask, xx_fmt, xx_arg1)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 2 parameters
---------------------------------------------------------------------------*/
#define DATA_HDR_MSG_SPRINTF_2(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2) \
  MSG_SPRINTF_2(MSG_SSID_DS_HDR, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 3 parameters
---------------------------------------------------------------------------*/
#define DATA_HDR_MSG_SPRINTF_3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_SPRINTF_3(MSG_SSID_DS_HDR, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 3 parameters. This Macro is 
  used when xx_fmt is passed at runtime.
---------------------------------------------------------------------------*/
#define DATA_HDR_MSG_SPRINTF_FMT_VAR_3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_SPRINTF_FMT_VAR_3(MSG_SSID_DS_HDR, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

//IGT
/*===========================================================================
  RLP SS related convenience macros. These will substitute the SSID as
  well as the desired sub-group ID.
===========================================================================*/

/*---------------------------------------------------------------------------
  Macro for messages with no parameters.
---------------------------------------------------------------------------*/
#define DATA_HDR_MSG0_GEN(xx_fmt) \
  MSG(MSG_SSID_DS_HDR, DATA_RLP_GENERAL, xx_fmt)
  
#define DATA_HDR_MSG0_DBG(xx_fmt) \
  MSG(MSG_SSID_DS_HDR, DATA_RLP_DEBUG, xx_fmt)
  
#define DATA_HDR_MSG0_TRC(xx_fmt) \
  MSG(MSG_SSID_DS_HDR, DATA_RLP_TRACE, xx_fmt)

/*---------------------------------------------------------------------------
  Macro for messages with 1 parameters.
---------------------------------------------------------------------------*/    
#define DATA_HDR_MSG1_GEN(xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS_HDR, DATA_RLP_GENERAL, xx_fmt, xx_arg1)

#define DATA_HDR_MSG1_DBG(xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS_HDR, DATA_RLP_DEBUG, xx_fmt, xx_arg1)
  
#define DATA_HDR_MSG1_TRC(xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS_HDR, DATA_RLP_TRACE, xx_fmt, xx_arg1)

/*---------------------------------------------------------------------------
  Macro for messages with 2 parameters.
---------------------------------------------------------------------------*/
#define DATA_HDR_MSG2_GEN(xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS_HDR, DATA_RLP_GENERAL, xx_fmt, xx_arg1, xx_arg2)

#define DATA_HDR_MSG2_DBG(xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS_HDR, DATA_RLP_DEBUG, xx_fmt, xx_arg1, xx_arg2)

#define DATA_HDR_MSG2_TRC(xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS_HDR, DATA_RLP_TRACE, xx_fmt, xx_arg1, xx_arg2)

/*---------------------------------------------------------------------------
  Macro for messages with 3 parameters.
---------------------------------------------------------------------------*/
#define DATA_HDR_MSG3_GEN(xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS_HDR, DATA_RLP_GENERAL, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

#define DATA_HDR_MSG3_DBG(xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS_HDR, DATA_RLP_DEBUG, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

#define DATA_HDR_MSG3_TRC(xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS_HDR, DATA_RLP_TRACE, xx_fmt, xx_arg1, xx_arg2, xx_arg3)  
  
  

/*===========================================================================
  DS EPC  SS related macros
===========================================================================*/



/*---------------------------------------------------------------------------
  Macro for messages with no parameters.
---------------------------------------------------------------------------*/
#define DATA_EPC_MSG0(xx_ss_mask, xx_fmt) \
  MSG(MSG_SSID_DS_EPC, xx_ss_mask, xx_fmt)  

/*---------------------------------------------------------------------------
  Macro for messages with 1 parameters.
---------------------------------------------------------------------------*/    
#define DATA_EPC_MSG1(xx_ss_mask, xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS_EPC, xx_ss_mask, xx_fmt, xx_arg1)

/*---------------------------------------------------------------------------
  Macro for messages with 2 parameters.
---------------------------------------------------------------------------*/
#define DATA_EPC_MSG2(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS_EPC, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2)

/*---------------------------------------------------------------------------
  Macro for messages with 3 parameters.
---------------------------------------------------------------------------*/
#define DATA_EPC_MSG3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS_EPC, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

/*---------------------------------------------------------------------------
  This is the macro for messages with 4 parameters.
---------------------------------------------------------------------------*/
#define DATA_EPC_MSG4(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4) \
  MSG_4(MSG_SSID_DS_EPC, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4)

/*---------------------------------------------------------------------------
  This is the macro for messages with 5 parameters.
---------------------------------------------------------------------------*/
#define DATA_EPC_MSG5(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                                               xx_arg5) \
  MSG_5(MSG_SSID_DS_EPC, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5)

/*---------------------------------------------------------------------------
  This is the macro for messages with 6 parameters 
---------------------------------------------------------------------------*/
#define DATA_EPC_MSG6(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                                          xx_arg5, xx_arg6) \
  MSG_6(MSG_SSID_DS_EPC, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6)

/*---------------------------------------------------------------------------
  This is the macro for messages with 7 parameters 
---------------------------------------------------------------------------*/
#define DATA_EPC_MSG7(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                                 xx_arg5, xx_arg6, xx_arg7) \
  MSG_7(MSG_SSID_DS_EPC, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6, xx_arg7)

/*---------------------------------------------------------------------------
  This is the macro for messages with 8 parameters 
---------------------------------------------------------------------------*/
#define DATA_EPC_MSG8(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                        xx_arg5, xx_arg6, xx_arg7, xx_arg8) \
  MSG_8(MSG_SSID_DS_EPC, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6, xx_arg7, xx_arg8)

/*---------------------------------------------------------------------------
  This is the macro for messages with 9 parameters 
---------------------------------------------------------------------------*/
#define DATA_EPC_MSG9(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                               xx_arg5, xx_arg6, xx_arg7, xx_arg8, xx_arg9) \
  MSG_9(MSG_SSID_DS_EPC, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6, xx_arg7, xx_arg8, xx_arg9)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 1 parameters
---------------------------------------------------------------------------*/
#define DATA_EPC_MSG_SPRINTF_1(xx_ss_mask, xx_fmt, xx_arg1) \
  MSG_SPRINTF_1(MSG_SSID_DS_EPC, xx_ss_mask, xx_fmt, xx_arg1)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 2 parameters
---------------------------------------------------------------------------*/
#define DATA_EPC_MSG_SPRINTF_2(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2) \
  MSG_SPRINTF_2(MSG_SSID_DS_EPC, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 3 parameters
---------------------------------------------------------------------------*/
#define DATA_EPC_MSG_SPRINTF_3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_SPRINTF_3(MSG_SSID_DS_EPC, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 3 parameters. This Macro is 
  used when xx_fmt is passed at runtime.
---------------------------------------------------------------------------*/
#define DATA_EPC_MSG_SPRINTF_FMT_VAR_3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_SPRINTF_FMT_VAR_3(MSG_SSID_DS_EPC, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

/*===========================================================================
  DS_EPC SS related convenience macros. These will substitute the SSID as
  well as the desired sub-group ID.
===========================================================================*/

/*---------------------------------------------------------------------------
  Macro for messages with no parameters.
---------------------------------------------------------------------------*/
#define DATA_EPC_MSG0_GEN(xx_fmt) \
  MSG(MSG_SSID_DS_EPC, DATA_EPC_GENERAL, xx_fmt)
  
#define DATA_EPC_MSG0_DBG(xx_fmt) \
  MSG(MSG_SSID_DS_EPC, DATA_EPC_DEBUG, xx_fmt)
  
#define DATA_EPC_MSG0_TRC(xx_fmt) \
  MSG(MSG_SSID_DS_EPC, DATA_EPC_TRACE, xx_fmt)

/*---------------------------------------------------------------------------
  Macro for messages with 1 parameters.
---------------------------------------------------------------------------*/    
#define DATA_EPC_MSG1_GEN(xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS_EPC, DATA_EPC_GENERAL, xx_fmt, xx_arg1)

#define DATA_EPC_MSG1_DBG(xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS_EPC, DATA_EPC_DEBUG, xx_fmt, xx_arg1)
  
#define DATA_EPC_MSG1_TRC(xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS_EPC, DATA_EPC_TRACE, xx_fmt, xx_arg1)

/*---------------------------------------------------------------------------
  Macro for messages with 2 parameters.
---------------------------------------------------------------------------*/
#define DATA_EPC_MSG2_GEN(xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS_EPC, DATA_EPC_GENERAL, xx_fmt, xx_arg1, xx_arg2)

#define DATA_EPC_MSG2_DBG(xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS_EPC, DATA_EPC_DEBUG, xx_fmt, xx_arg1, xx_arg2)

#define DATA_EPC_MSG2_TRC(xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS_EPC, DATA_EPC_TRACE, xx_fmt, xx_arg1, xx_arg2)

/*---------------------------------------------------------------------------
  Macro for messages with 3 parameters.
---------------------------------------------------------------------------*/
#define DATA_EPC_MSG3_GEN(xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS_EPC, DATA_EPC_GENERAL, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

#define DATA_EPC_MSG3_DBG(xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS_EPC, DATA_EPC_DEBUG, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

#define DATA_EPC_MSG3_TRC(xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS_EPC, DATA_EPC_TRACE, xx_fmt, xx_arg1, xx_arg2, xx_arg3)



/*===========================================================================
  DS APPSRV SS related macros
===========================================================================*/

/*---------------------------------------------------------------------------
  Macro for messages with no parameters.
---------------------------------------------------------------------------*/
#define DATA_APPSRV_MSG0(xx_ss_mask, xx_fmt) \
  MSG(MSG_SSID_DS, xx_ss_mask, xx_fmt)

/*---------------------------------------------------------------------------
  Macro for messages with 1 parameters.
---------------------------------------------------------------------------*/
#define DATA_APPSRV_MSG1(xx_ss_mask, xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS, xx_ss_mask, xx_fmt, xx_arg1)

/*---------------------------------------------------------------------------
  Macro for messages with 2 parameters.
---------------------------------------------------------------------------*/
#define DATA_APPSRV_MSG2(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2)

/*---------------------------------------------------------------------------
  Macro for messages with 3 parameters.
---------------------------------------------------------------------------*/
#define DATA_APPSRV_MSG3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

/*---------------------------------------------------------------------------
  This is the macro for messages with 4 parameters.
---------------------------------------------------------------------------*/
#define DATA_APPSRV_MSG4(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4) \
  MSG_4(MSG_SSID_DS, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4)

/*---------------------------------------------------------------------------
  This is the macro for messages with 5 parameters.
---------------------------------------------------------------------------*/
#define DATA_APPSRV_MSG5(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                                               xx_arg5) \
  MSG_5(MSG_SSID_DS, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5)

/*---------------------------------------------------------------------------
  This is the macro for messages with 6 parameters
---------------------------------------------------------------------------*/
#define DATA_APPSRV_MSG6(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                                          xx_arg5, xx_arg6) \
  MSG_6(MSG_SSID_DS, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6)

/*---------------------------------------------------------------------------
  This is the macro for messages with 7 parameters
---------------------------------------------------------------------------*/
#define DATA_APPSRV_MSG7(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                                 xx_arg5, xx_arg6, xx_arg7) \
  MSG_7(MSG_SSID_DS, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6, xx_arg7)

/*---------------------------------------------------------------------------
  This is the macro for messages with 8 parameters
---------------------------------------------------------------------------*/
#define DATA_APPSRV_MSG8(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                        xx_arg5, xx_arg6, xx_arg7, xx_arg8) \
  MSG_8(MSG_SSID_DS, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6, xx_arg7, xx_arg8)

/*---------------------------------------------------------------------------
  This is the macro for messages with 9 parameters
---------------------------------------------------------------------------*/
#define DATA_APPSRV_MSG9(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                               xx_arg5, xx_arg6, xx_arg7, xx_arg8, xx_arg9) \
  MSG_9(MSG_SSID_DS, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6, xx_arg7, xx_arg8, xx_arg9)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 1 parameters
---------------------------------------------------------------------------*/
#define DATA_APPSRV_MSG_SPRINTF_1(xx_ss_mask, xx_fmt, xx_arg1) \
  MSG_SPRINTF_1(MSG_SSID_DS, xx_ss_mask, xx_fmt, xx_arg1)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 2 parameters
---------------------------------------------------------------------------*/
#define DATA_APPSRV_MSG_SPRINTF_2(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2) \
  MSG_SPRINTF_2(MSG_SSID_DS, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 3 parameters
---------------------------------------------------------------------------*/
#define DATA_APPSRV_MSG_SPRINTF_3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_SPRINTF_3(MSG_SSID_DS, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 3 parameters. This Macro is
  used when xx_fmt is passed at runtime.
---------------------------------------------------------------------------*/
#define DATA_APPSRV_MSG_SPRINTF_FMT_VAR_3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_SPRINTF_FMT_VAR_3(MSG_SSID_DS, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

/*===========================================================================
  APPSRV SS related convenience macros. These will substitute the SSID as
  well as the desired sub-group ID.
===========================================================================*/

/*---------------------------------------------------------------------------
  Macro for messages with no parameters.
---------------------------------------------------------------------------*/
#define DATA_APPSRV_MSG0_GEN(xx_fmt) \
  MSG(MSG_SSID_DS, DATA_APPSRV_GENERAL, xx_fmt)

#define DATA_APPSRV_MSG0_DBG(xx_fmt) \
  MSG(MSG_SSID_DS, DATA_APPSRV_DEBUG, xx_fmt)

#define DATA_APPSRV_MSG0_TRC(xx_fmt) \
  MSG(MSG_SSID_DS, DATA_APPSRV_TRACE, xx_fmt)

/*---------------------------------------------------------------------------
  Macro for messages with 1 parameters.
---------------------------------------------------------------------------*/
#define DATA_APPSRV_MSG1_GEN(xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS, DATA_APPSRV_GENERAL, xx_fmt, xx_arg1)

#define DATA_APPSRV_MSG1_DBG(xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS, DATA_APPSRV_DEBUG, xx_fmt, xx_arg1)

#define DATA_APPSRV_MSG1_TRC(xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS, DATA_APPSRV_TRACE, xx_fmt, xx_arg1)

/*---------------------------------------------------------------------------
  Macro for messages with 2 parameters.
---------------------------------------------------------------------------*/
#define DATA_APPSRV_MSG2_GEN(xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS, DATA_APPSRV_GENERAL, xx_fmt, xx_arg1, xx_arg2)

#define DATA_APPSRV_MSG2_DBG(xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS, DATA_APPSRV_DEBUG, xx_fmt, xx_arg1, xx_arg2)

#define DATA_APPSRV_MSG2_TRC(xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS, DATA_APPSRV_TRACE, xx_fmt, xx_arg1, xx_arg2)

/*---------------------------------------------------------------------------
  Macro for messages with 3 parameters.
---------------------------------------------------------------------------*/
#define DATA_APPSRV_MSG3_GEN(xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS, DATA_APPSRV_GENERAL, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

#define DATA_APPSRV_MSG3_DBG(xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS, DATA_APPSRV_DEBUG, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

#define DATA_APPSRV_MSG3_TRC(xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS, DATA_APPSRV_TRACE, xx_fmt, xx_arg1, xx_arg2, xx_arg3) 




/*===========================================================================
  Here are all the DS message Sub-group IDs for all the Sub-systems of
  Data Services software. These IDs need to be in this file only.
===========================================================================*/


/*---------------------------------------------------------------------------
  The first 5 masks are allocated for legacy messages, so all the other
  sub-groups start from MSG_MASK_5.
---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
  DS Message Sub-Group Ids for SS LEVEL1
---------------------------------------------------------------------------*/
// None, it will only have the first 3 legacy sub-groups

/*---------------------------------------------------------------------------
  DS Message Sub-Group Ids for SS RLP 
---------------------------------------------------------------------------*/
#define DATA_RLP_GENERAL           (MSG_MASK_5)
#define DATA_RLP_DEBUG             (MSG_MASK_6)
#define DATA_RLP_TRACE             (MSG_MASK_7)

/*---------------------------------------------------------------------------
  DS Message Sub-Group Ids for SS PPP
---------------------------------------------------------------------------*/
#define DATA_PPP_GENERAL           (MSG_MASK_5)
#define DATA_PPP_DEBUG             (MSG_MASK_6)
#define DATA_PPP_TRACE             (MSG_MASK_7)

/*---------------------------------------------------------------------------
  DS Message Sub-Group Ids for SS TCP/IP
---------------------------------------------------------------------------*/
#define DATA_TCPIP_GENERAL         (MSG_MASK_5)
#define DATA_TCPIP_DEBUG           (MSG_MASK_6)
#define DATA_TCPIP_TRACE           (MSG_MASK_7)

/*---------------------------------------------------------------------------
  DS Message Sub-Group Ids for SS DS IS707
---------------------------------------------------------------------------*/
#define DATA_IS707_GENERAL         (MSG_MASK_5)
#define DATA_IS707_DEBUG           (MSG_MASK_6)
#define DATA_IS707_TRACE           (MSG_MASK_7)

/*---------------------------------------------------------------------------
  DS Message Sub-Group Ids for SS DS 3G MGR
---------------------------------------------------------------------------*/
#define DATA_3GMGR_GENERAL         (MSG_MASK_5)
#define DATA_3GMGR_DEBUG           (MSG_MASK_6)
#define DATA_3GMGR_TRACE           (MSG_MASK_7)

/*---------------------------------------------------------------------------
  DS Message Sub-Group Ids for SS PS
---------------------------------------------------------------------------*/
#define DATA_PS_GENERAL            (MSG_MASK_5)
#define DATA_PS_DEBUG              (MSG_MASK_6)
#define DATA_PS_TRACE              (MSG_MASK_7)

/*---------------------------------------------------------------------------
  DS Message Sub-Group Ids for SS MIP
---------------------------------------------------------------------------*/
#define DATA_MIP_GENERAL           (MSG_MASK_5)
#define DATA_MIP_DEBUG             (MSG_MASK_6)
#define DATA_MIP_TRACE             (MSG_MASK_7)


/*---------------------------------------------------------------------------
  DS Message Sub-Group Ids for SS UMTS
---------------------------------------------------------------------------*/
#define DATA_UMTS_GENERAL          (MSG_MASK_5)
#define DATA_UMTS_DEBUG            (MSG_MASK_6)
#define DATA_UMTS_TRACE            (MSG_MASK_7)

/*---------------------------------------------------------------------------
  DS Message Sub-Group Ids for SS GPRS
---------------------------------------------------------------------------*/
#define DATA_GPRS_GENERAL          (MSG_MASK_5)
#define DATA_GPRS_DEBUG            (MSG_MASK_6)
#define DATA_GPRS_TRACE            (MSG_MASK_7)

/*---------------------------------------------------------------------------
  DS Message Sub-Group Ids for SS GSM
---------------------------------------------------------------------------*/
#define DATA_GSM_GENERAL           (MSG_MASK_5)
#define DATA_GSM_DEBUG             (MSG_MASK_6)
#define DATA_GSM_TRACE             (MSG_MASK_7)

/*---------------------------------------------------------------------------
  DS Message Sub-Group Ids for SS 3GPP
---------------------------------------------------------------------------*/
#define DATA_3GPP_GENERAL           (MSG_MASK_5)
#define DATA_3GPP_DEBUG             (MSG_MASK_6)
#define DATA_3GPP_TRACE             (MSG_MASK_7)

/*---------------------------------------------------------------------------
  DS Message Sub-Group Ids for SS LTE
---------------------------------------------------------------------------*/
#define DATA_LTE_GENERAL           (MSG_MASK_5)
#define DATA_LTE_DEBUG             (MSG_MASK_6)
#define DATA_LTE_TRACE             (MSG_MASK_7)

/*---------------------------------------------------------------------------
  DS Message Sub-Group Ids for SS WCDMA
---------------------------------------------------------------------------*/
#define DATA_WCDMA_GENERAL           (MSG_MASK_5)
#define DATA_WCDMA_DEBUG             (MSG_MASK_6)
#define DATA_WCDMA_TRACE             (MSG_MASK_7)

/*---------------------------------------------------------------------------
  DS Message Sub-Group Ids for SS SOCKETS
---------------------------------------------------------------------------*/
#define DATA_SOCKETS_GENERAL       (MSG_MASK_5)
#define DATA_SOCKETS_DEBUG         (MSG_MASK_6)
#define DATA_SOCKETS_TRACE         (MSG_MASK_7)

/*---------------------------------------------------------------------------
  DS Message Sub-Group Ids for SS ATCOP
---------------------------------------------------------------------------*/
#define DATA_ATCOP_GENERAL         (MSG_MASK_5)
#define DATA_ATCOP_DEBUG           (MSG_MASK_6)
#define DATA_ATCOP_TRACE           (MSG_MASK_7)

/*---------------------------------------------------------------------------
  DS Message Sub-Group Ids for SS SIO
---------------------------------------------------------------------------*/
#define DATA_SIO_GENERAL           (MSG_MASK_5)
#define DATA_SIO_DEBUG             (MSG_MASK_6)
#define DATA_SIO_TRACE             (MSG_MASK_7)

/*---------------------------------------------------------------------------
  DS Message Sub-Group Ids for SS HDR 
---------------------------------------------------------------------------*/
#define DATA_HDR_GENERAL           (MSG_MASK_5)
#define DATA_HDR_DEBUG             (MSG_MASK_6)
#define DATA_HDR_TRACE             (MSG_MASK_7)

/*---------------------------------------------------------------------------
  DS Message Sub-Group Ids for SS APPSRV
---------------------------------------------------------------------------*/
#define DATA_APPSRV_GENERAL        (MSG_MASK_5)
#define DATA_APPSRV_DEBUG          (MSG_MASK_6)
#define DATA_APPSRV_TRACE          (MSG_MASK_7)
#endif  // DATA_MSG_H
