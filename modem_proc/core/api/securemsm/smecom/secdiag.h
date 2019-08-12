#ifndef SECDIAG_H
#define SECDIAG_H

/**
   @file secdiag.h

   @brief 
   Diagnostic Message Header File.

   This module consists of diagnostic message macros.
   The messages are output on qxdm utility.

   Externalized Functions:
   None.

   Initialization and sequencing requirements:
   None.
*/

/*===========================================================================
                                S E C  D I A G
                              H E A D E R  F I L E

Copyright (c) 2006-2011 by Qualcomm Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/


/*===========================================================================
                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.mpss/3.5.c12.3/api/securemsm/smecom/secdiag.h#1 $
$DateTime: 2016/12/13 07:59:23 $
$Author: mplcsds1 $


when       who      what, where, why
--------   ---      ------------------------------------
11/0111    nk       Added Qshrink changes as sugested by target team
                    to reduce memory foot print
01/17/11   leo      (Tech Pubs) Edited Doxygen comments and markup.
12/31/10   nk       Doxygenated.
12/07/10   yh       Added SECDIAG_SFP definition. Mapping to an existing SSID, till a new one is assigned for this
08/09/07   sm       Added SECDIAG_BCASTAUTH definition. Mapping to an existing SSID, till a new one is assigned for this
06/22/07   sb       Added SECDIAG_BCASTCNTAGENT definition
07/05/06   sl       Created

===========================================================================*/

#include "msg.h"

/** @addtogroup SMECOMUTIL_DIAG 
@{ 
*/

/**
   @name Diagnostic Message SSIDs

   The Service Set Identifiers (SSIDs) are centralized here to be used by all
   security modules.
   @{
*/

#define MSG_SSID_SECDIAG_CRYPTO        MSG_SSID_SEC_CRYPTO        /**< SSID crypto. */
#define MSG_SSID_SECDIAG_SSL           MSG_SSID_SEC_SSL           /**< SSID SSL. */
#define MSG_SSID_SECDIAG_IPSEC         MSG_SSID_SEC_IPSEC         /**< SSID Internet Protocol security. */
#define MSG_SSID_SECDIAG_SFS           MSG_SSID_SEC_SFS           /**< SSID SFS. */
#define MSG_SSID_SECDIAG_TEST          MSG_SSID_SEC_TEST          /**< SSID test. */ 
#define MSG_SSID_SECDIAG_CNTAGENT      MSG_SSID_SEC_CNTAGENT      /**< SSID content agent. */
#define MSG_SSID_SECDIAG_RIGHTSMGR     MSG_SSID_SEC_RIGHTSMGR     /**< SSID rights manager. */
#define MSG_SSID_SECDIAG_ROAP          MSG_SSID_SEC_ROAP          /**< SSID Rights Object Aquisition Protocol. */
#define MSG_SSID_SECDIAG_MEDIAMGR      MSG_SSID_SEC_MEDIAMGR      /**< SSID media manager. */
#define MSG_SSID_SECDIAG_IDSTORE       MSG_SSID_SEC_IDSTORE       /**< SSID ID store. */
#define MSG_SSID_SECDIAG_IXFILE        MSG_SSID_SEC_IXFILE        /**< SSID IxFile. */
#define MSG_SSID_SECDIAG_IXSTREAM      MSG_SSID_SEC_IXCOMMON      /**< SSID IxStream. */
#define MSG_SSID_SECDIAG_IXTIME        MSG_SSID_SEC_IXCOMMON      /**< SSID IxTime. */
#define MSG_SSID_SECDIAG_IXUTIL        MSG_SSID_SEC_IXCOMMON      /**< SSID IxUtil. */
#define MSG_SSID_SECDIAG_IXSQL         MSG_SSID_SEC_IXSQL         /**< SSID IxSql. */
#define MSG_SSID_SECDIAG_IXCOMMON      MSG_SSID_SEC_IXCOMMON      /**< SSID IxCommon. */
#define MSG_SSID_SECDIAG_BCASTCNTAGENT MSG_SSID_SEC_BCASTCNTAGENT /**< SSID broadcast content agent. */
#define MSG_SSID_SECDIAG_BCASTAUTH     MSG_SSID_SEC_BCASTCNTAGENT /**< SSID broadcast content authentication. */ 
                                                                  /* todo request appropriate SSID into msgcfg.h */
#define MSG_SSID_SECDIAG_SFP           MSG_SSID_SEC_BCASTCNTAGENT /**< SSID broadcast content SFP. */
                                                                  /* todo request appropriate SSID into msgcfg.h */
#define MSG_SSID_SECDIAG_PLAYREADY     MSG_SSID_SEC_PLAYREADY     /**< SSID PlayReady */

#define MSG_BUILD_MASK_MSG_SSID_SECDIAG_CRYPTO            MSG_BUILD_MASK_MSG_SSID_SEC_CRYPTO    
#define MSG_BUILD_MASK_MSG_SSID_SECDIAG_SSL               MSG_BUILD_MASK_MSG_SSID_SEC_SSL        
#define MSG_BUILD_MASK_MSG_SSID_SECDIAG_IPSEC             MSG_BUILD_MASK_MSG_SSID_SEC_IPSEC      
#define MSG_BUILD_MASK_MSG_SSID_SECDIAG_SFS               MSG_BUILD_MASK_MSG_SSID_SEC_SFS        
#define MSG_BUILD_MASK_MSG_SSID_SECDIAG_TEST              MSG_BUILD_MASK_MSG_SSID_SEC_TEST       
#define MSG_BUILD_MASK_MSG_SSID_SECDIAG_CNTAGENT          MSG_BUILD_MASK_MSG_SSID_SEC_CNTAGENT   
#define MSG_BUILD_MASK_MSG_SSID_SECDIAG_RIGHTSMGR         MSG_BUILD_MASK_MSG_SSID_SEC_RIGHTSMGR  
#define MSG_BUILD_MASK_MSG_SSID_SECDIAG_ROAP              MSG_BUILD_MASK_MSG_SSID_SEC_ROAP       
#define MSG_BUILD_MASK_MSG_SSID_SECDIAG_MEDIAMGR          MSG_BUILD_MASK_MSG_SSID_SEC_MEDIAMGR   
#define MSG_BUILD_MASK_MSG_SSID_SECDIAG_IDSTORE           MSG_BUILD_MASK_MSG_SSID_SEC_IDSTORE    
#define MSG_BUILD_MASK_MSG_SSID_SECDIAG_IXFILE            MSG_BUILD_MASK_MSG_SSID_SEC_IXFILE     
#define MSG_BUILD_MASK_MSG_SSID_SECDIAG_IXSQL             MSG_BUILD_MASK_MSG_SSID_SEC_IXSQL      
#define MSG_BUILD_MASK_MSG_SSID_SECDIAG_IXCOMMON          MSG_BUILD_MASK_MSG_SSID_SEC_IXCOMMON   
#define MSG_BUILD_MASK_MSG_SSID_SECDIAG_BCASTCNTAGENT     MSG_BUILD_MASK_MSG_SSID_SEC_BCASTCNTAGENT 
#define MSG_BUILD_MASK_MSG_SSID_SECDIAG_PLAYREADY         MSG_BUILD_MASK_MSG_SSID_SEC_PLAYREADY 

/** @} */ /* end_name diagnostic message */

/**
   @name Security Diagnostic Message Masks

   DIAG MSG 2.0 allows security message masks (beyond five legacy masks)
   to be defined within a subsystem or across common subsystems.
   @{
*/

#define MSG_SEC_LOW                 (MSG_MASK_0)      /**< Message mask 0; low. */
#define MSG_SEC_MED                 (MSG_MASK_1)      /**< Message mask 1; medium. */
#define MSG_SEC_HIGH                (MSG_MASK_2)      /**< Message mask 2; high. */
#define MSG_SEC_ERROR               (MSG_MASK_3)      /**< Message mask 3; error. */
#define MSG_SEC_FATAL               (MSG_MASK_4)      /**< Message mask 4; fatal. */
#define MSG_SEC_DEBUG               (MSG_MASK_5)      /**< Message mask 5; debug. */

/** @} */ /* end_name diagnostic message masks */

/**
   @name Message Macros to Send Debug Message to DIAG.
   @{
*/

/**
   Macro for terse messages with no parameters.
*/
#define SEC_DIAG_MSG(xx_ss_mask, xx_prio, xx_fmt) \
  MSG(MSG_SSID_ ## xx_ss_mask, xx_prio, xx_fmt)

/**
   Macro for messages with one parameter.
*/
#define SEC_DIAG_MSG1(xx_ss_mask, xx_prio, xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_ ## xx_ss_mask, xx_prio, xx_fmt, xx_arg1)

/**
   Macro for messages with two parameters.
*/
#define SEC_DIAG_MSG2(xx_ss_mask, xx_prio, xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_ ## xx_ss_mask, xx_prio, xx_fmt, xx_arg1, xx_arg2)

/**
   Macro for messages with three parameters.
*/
#define SEC_DIAG_MSG3(xx_ss_mask, xx_prio, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_ ## xx_ss_mask, xx_prio, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

/**
   Macro for messages with four parameters.
*/
#define SEC_DIAG_MSG4(xx_ss_mask, xx_prio, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                            xx_arg4) \
  MSG_4(MSG_SSID_ ## xx_ss_mask, xx_prio, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4)

/**
   Macro for messages with five parameters.
*/
#define SEC_DIAG_MSG5(xx_ss_mask, xx_prio, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                            xx_arg4, xx_arg5) \
  MSG_5(MSG_SSID_ ## xx_ss_mask, xx_prio, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5)

/**
   Macro for messages with six parameters.
*/
#define SEC_DIAG_MSG6(xx_ss_mask, xx_prio, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                            xx_arg4, xx_arg5, xx_arg6) \
  MSG_6(MSG_SSID_ ## xx_ss_mask, xx_prio, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6)

/**
   Macro for messages with seven parameters.
*/
#define SEC_DIAG_MSG7(xx_ss_mask, xx_prio, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                            xx_arg4, xx_arg5, xx_arg6, \
                                            xx_arg7) \
  MSG_7(MSG_SSID_ ## xx_ss_mask, xx_prio, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6, xx_arg7)

/**
   Macro for messages with eight parameters.
*/
#define SEC_DIAG_MSG8(xx_ss_mask, xx_prio, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                            xx_arg4, xx_arg5, xx_arg6, \
                                            xx_arg7, xx_arg8) \
  MSG_8(MSG_SSID_ ## xx_ss_mask, xx_prio, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6, xx_arg7, xx_arg8)     
  
/**
  Macro for messages with 9 parameter.
*/
#define SEC_DIAG_MSG9(xx_ss_mask, xx_prio, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                            xx_arg4, xx_arg5, xx_arg6, \
                                            xx_arg7, xx_arg8, xx_arg9) \
  MSG_9(MSG_SSID_ ## xx_ss_mask, xx_prio, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6, xx_arg7, xx_arg8, xx_arg9)

/** @} */ /* end_name debug message to diag */


/**
   @name Message Macros to Send Debug Messages in Extended Format to DIAG.
   @{
*/


/**
  Macro for sprintf messages with one parameter. msg_sprintf() uses a variable
  argument list supported by the compiler. This macro is used when xx_fmt is
  passed as a literal.
*/
#define SEC_DIAG_SPRINTF1(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1) \
  MSG_SPRINTF_1(MSG_SSID_ ## xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1)

/**
  Macro for sprintf messages with two parameters. msg_sprintf() uses a variable
  argument list supported by the compiler. This macro is used when xx_fmt is
  passed as a literal.
*/
#define SEC_DIAG_SPRINTF2(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2) \
  MSG_SPRINTF_2(MSG_SSID_ ## xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2)


/**
  Macro for sprintf messages with three parameters. msg_sprintf() uses a
  variable argument list supported by the compiler. This macro is used when
  xx_fmt is passed as a literal.
*/
#define SEC_DIAG_SPRINTF3(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_SPRINTF_3(MSG_SSID_ ## xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3)


/**
  Macro for sprintf messages with four parameters. msg_sprintf() uses a
  variable argument list supported by the compiler. This macro is used when
  xx_fmt is passed as a literal.
*/
#define SEC_DIAG_SPRINTF4(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                                    xx_arg4 ) \
  MSG_SPRINTF_4(MSG_SSID_ ## xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                                    xx_arg4 )

/**
  Macro for sprintf messages with five parameters. msg_sprintf() uses a
  variable argument list supported by the compiler. This macro is used when
  xx_fmt is passed as a literal.
*/
#define SEC_DIAG_SPRINTF5(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                                    xx_arg4, xx_arg5 ) \
  MSG_SPRINTF_5(MSG_SSID_ ## xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                                    xx_arg4, xx_arg5 )


/**
  Macro for sprintf messages with six parameters. msg_sprintf() uses a variable
  argument list supported by the compiler. This macro is used when xx_fmt is
  passed as a literal.
*/
#define SEC_DIAG_SPRINTF6(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                                    xx_arg4, xx_arg5, xx_arg6 ) \
  MSG_SPRINTF_6(MSG_SSID_ ## xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                                    xx_arg4, xx_arg5, xx_arg6 )


/**
  Macro for sprintf messages with seven parameters. msg_sprintf() uses a
  variable argument list supported by the compiler. This macro is used when
  xx_fmt is passed as a literal.
*/
#define SEC_DIAG_SPRINTF7(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                                    xx_arg4, xx_arg5, xx_arg6, \
                                                    xx_arg7 ) \
  MSG_SPRINTF_7(MSG_SSID_ ## xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                                    xx_arg4, xx_arg5, xx_arg6, \
                                                    xx_arg7 )


/**
  Macro for sprintf messages with eight parameters. msg_sprintf() uses a
  variable argument list supported by the compiler. This macro is used when
  xx_fmt is passed as a literal.
*/
#define SEC_DIAG_SPRINTF8(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                                    xx_arg4, xx_arg5, xx_arg6, \
                                                    xx_arg7, xx_arg8 ) \
  MSG_SPRINTF_8(MSG_SSID_ ## xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                                    xx_arg4, xx_arg5, xx_arg6, \
                                                    xx_arg7, xx_arg8 )


/**
   Macro for sprintf messages with nine parameters. This macro is used when
   xx_fmt is passed at runtime.
*/
/*Need to decide about xx_ss_id, currently not used in build anywhere*/
#define SEC_DIAG_SPRINTF_FMT_VAR_3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3)/**/

/** @} */ /* end_name debug message extended format to diag */

/** @} */  /* end_addtogroup SMECOMUTIL_DIAG */

#endif /* SECDIAG_H */
