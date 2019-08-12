#ifndef CUSTDRM_MS_H
#define CUSTDRM_MS_H
/*===========================================================================

            " C u s t -D R M 2.0 "   H E A D E R   F I L E

DESCRIPTION
  Configuration for DRM 2.0 Library.

  Copyright (c) 2001-2006  by Qualcomm Technologies Incorporated. All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/build/cust/custdrm_ms.h#1 $ 
$DateTime: 2016/06/17 14:31:11 $ 
$Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/26/08   avm     undef FEATURE_DRM_DISABLE_SFS_STORAGE
11/11/07   avm     Undefied FEATURE_IXHTTPSVC
12/06/06    rv     #undef AMSS_PD on SEC_PD
09/19/06    rv     defined FEATURE_IXDESCRAMBLERSVC
08/25/06    rv     removed special charachter
08/17/06    rv     added features FEATURE_SEC_CONTENT_AGENT & 
                   FEATURE_SEC_PROCESS_INIT; moved FEATURE_IXTIMESVC
06/14/06    rv     Created file.

===========================================================================*/


/*===========================================================================

                         PUBLIC DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
                       DRM 2.0 Modules
---------------------------------------------------------------------------*/


/*------------------------------*/
/*          HTPP Service        */  
/*------------------------------*/
#define FEATURE_IXHTTPSVC

/*------------------------------*/
/*          File Service        */  
/*------------------------------*/
#ifndef FEATURE_IXFILESVC
    #define FEATURE_IXFILESVC
#endif

/*------------------------------*/
/*      SQL Service             */  
/*------------------------------*/
//#define FEATURE_IXSQLSVC

/*------------------------------*/
/*      IxContent Service       */  
/*------------------------------*/
#define FEATURE_IXCONTENTSVC

/*------------------------------*/
/*          Content Agent       */  
/*------------------------------*/
#define FEATURE_SEC_CONTENT_AGENT

/*------------------------------*/
/* SEC Services Initialization  */  
/*------------------------------*/
#define FEATURE_SEC_PROCESS_INIT

/*------------------------------*/
/*  Descrambler services        */  
/*------------------------------*/
//#define FEATURE_IXDESCRAMBLERSVC

/*------------------------------*/
/*     Diag  services           */  
/*------------------------------*/
#if defined(DUAL_PD)
    #define FEATURE_IXDIAGSVC
#endif

#if defined(DUAL_PD) && defined(SEC_PD)
    #undef AMSS_PD
    #undef FEATURE_ICONTENT
#endif 

#define FEATURE_SME_DRMMS

#define TELECA_WMDRM

/* The flag below is used to disable using USFS for storing the keys & certs */
#undef FEATURE_DRMMS_DISABLE_SFS_STORAGE

#endif //CUSTDRM_MS_H
