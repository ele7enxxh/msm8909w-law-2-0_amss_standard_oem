#ifndef CUSTDRM20_H
#define CUSTDRM20_H
/*===========================================================================

            " C u s t -D R M 2.0 "   H E A D E R   F I L E

DESCRIPTION
  Configuration for DRM 2.0 Library.

  Copyright (c) 2001-2010  by Qualcomm Technologies Incorporated. All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/build/cust/custdrm20.h#1 $ 
$DateTime: 2016/06/17 14:31:11 $ 
$Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/16/08    gr     HTPP Support for new DNS API
06/26/08   avm     undef FEATURE_DRM_DISABLE_SFS_STORAGE
04/04/08   avm     disabled : FEATURE_DYNAMIC_RDB_SFS_ENABLE
12/07/07    rv     undef FEATURE_PROTECT_CEK
08/07/07    gf     Add FEATURE_SME_DRMOMA
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

/*----------------------------------*/
/*    HTPP Support for new DNS API  */
/*----------------------------------*/
#define FEATURE_SEC_IXHTTP_DNS_SESSION_API

/*------------------------------*/
/*          File Service        */  
/*------------------------------*/
#ifndef FEATURE_IXFILESVC
    #define FEATURE_IXFILESVC
#endif

/*------------------------------*/
/*      IxContent Service       */  
/*------------------------------*/
#ifndef FEATURE_IXCONTENTSVC
    #define FEATURE_IXCONTENTSVC
#endif

/*------------------------------*/
/*          Content Agent       */  
/*------------------------------*/
#define FEATURE_SEC_CONTENT_AGENT

/*------------------------------*/
/* SEC Services Initialization  */  
/*------------------------------*/
#define FEATURE_SEC_PROCESS_INIT

/*------------------------------*/
/*     Feature OMA DRM          */  
/*------------------------------*/
#define FEATURE_SME_DRMOMA

#endif //CUSTDRM20_H
