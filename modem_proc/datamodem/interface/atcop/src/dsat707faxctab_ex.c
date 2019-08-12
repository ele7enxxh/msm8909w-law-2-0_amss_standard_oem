/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        D A T A   S E R V I C E S
                A T   C O M M A N D   P R O C E S S O R

               I S - 7 0 7   C O M M A N D   T A B L E S

GENERAL DESCRIPTION
  This module contains the command tables and data definitions required
  to define the FAX related AT modem commands for the IS-707 mode.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2001 - 2014 Qualcomm Technologies Incorporated.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/atcop/src/dsat707faxctab_ex.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/27/14   sc     Initial revision (created file for Dynamic ATCoP).

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#ifdef FEATURE_DATA_IS707_ASYNC
#include "dsati.h"
#include "dsat707fax.h"
#include "dsat707faxctab.h"
#include "dsatparm.h"
#include "msg.h"

#include "ds707_roaming.h"

/*  Flag to indicate that an AT+FDT command has been   */
/*  and that a land to mobile FAX image is about to start */
boolean dsat707_fdt_seen = FALSE;
const unsigned int dsat707_sizeof_fns_val = 181;

/*=========================================================================*/
/* Defaults & Limits for FAX commands */
/*=========================================================================*/

LOCAL const dflm_type dsat707_faa_dflm [] =
{
  { 0  , 0  , 1    },
} ;

LOCAL const dflm_type dsat707_fap_dflm [] =
{
  { 0  , 0  , 1    },
  { 0  , 0  , 1    },
  { 0  , 0  , 1    },
} ;

LOCAL const dflm_type dsat707_fbo_dflm [] =
{
  { 0  , 0  , 3    },
} ;

LOCAL const dflm_type dsat707_fbu_dflm [] =
{
  { 0  , 0  , 1    },
} ;

LOCAL dflm_type dsat707_fcc_dflm [] =
{
  { 0  , 0  , 1    },
  { 1  , 0  , 5    },
  { 0  , 0  , 4    },
  { 0  , 0  , 2    },
  { 0  , 0  , 3    },
  { 0  , 0  , 1    },
  { 0  , 0  , 1    },
  { 0  , 0  , 7    },
} ;

LOCAL const dflm_type dsat707_fcq_dflm [] =
{
  { 1  , 0  , 2    },
  { 0  , 0  , 2    },
} ;

LOCAL const dflm_type dsat707_fcr_dflm [] =
{
  { 0,   0  , 1    },
} ;


LOCAL const dflm_type dsat707_fct_dflm [] =
{
  { 0x1E, 0x0, 0xFF  },
} ;

LOCAL const dflm_type dsat707_fea_dflm [] =
{
  { 0  , 0  , 1    },
} ;

LOCAL const dflm_type dsat707_ffc_dflm [] =
{
  { 0  , 0  , 3    },
  { 0  , 0  , 2    },
  { 0  , 0  , 3    },
  { 0  , 0  , 2    },
} ;

LOCAL const dflm_type dsat707_fhs_dflm [] =
{
  { 0  , 00 , 0xFF  },
} ;

LOCAL const dflm_type dsat707_fie_dflm [] =
{
  { 0  , 0  , 1    },
} ;

LOCAL const dflm_type dsat707_fip_dflm [] =
{
  { 0  , 0  , 0    },
} ;

LOCAL const dflm_type dsat707_fis_dflm [] =
{
  { 0  , 0  , 1    },
  { 1  , 0  , 5    },
  { 0  , 0  , 4    },
  { 0  , 0  , 2    },
  { 0  , 0  , 3    },
  { 0  , 0  , 1    },
  { 0  , 0  , 1    },
  { 0  , 0  , 7    },
} ;

LOCAL const dflm_type dsat707_flo_dflm [] =
{
  { 1  , 0  , 2    },
} ;


LOCAL const dflm_type dsat707_flp_dflm [] =
{
  { 0  , 0  , 1    },
} ;

LOCAL const dflm_type dsat707_fms_dflm [] =
{
  { 0  , 0  , 5    },
} ;

LOCAL const dflm_type dsat707_fnr_dflm [] =
{
  { 0  , 0  , 1    },
  { 0  , 0  , 1    },
  { 0  , 0  , 1    },
  { 0  , 0  , 1    },
} ;

LOCAL const dflm_type dsat707_fpp_dflm [] =
{
  { 0  , 0  , 0    },
} ;

LOCAL const dflm_type dsat707_fpr_dflm [] =
{
  { 8  , 8  , 8  },
} ;

LOCAL const dflm_type dsat707_fps_dflm [] =
{
  { 1  , 0  , 5    },
} ;

LOCAL const dflm_type dsat707_frq_dflm [] =
{
  { 0,   0x0, 0x64  },
  { 0,   0x0, 0xFF  },
} ;

LOCAL const dflm_type dsat707_fry_dflm [] =
{
  { 0x0, 0x0, 0xFF  },
} ;

LOCAL const dflm_type dsat707_fsp_dflm [] =
{
  { 0  , 0  , 1    },
} ;

/*=========================================================================*/
/* IS-707 Fax related AT command table */
/*=========================================================================*/
const dsati_cmd_type dsat707_fax_table [] =
{
  /*lint -save -e785 */
	{ "+FAA",     
    EXTENDED | CONFIG | HEX | FCLASS2P0 | DOWN_LOAD,
    SPECIAL_NONE,   
    1,
    DSAT707_FAX_FAA_IDX,
    &dsat707_faa_dflm[0] },
/*-------------------------------------------------------------------------*/
  { "+FAP",
    EXTENDED | CONFIG | HEX | FCLASS2P0 | DOWN_LOAD,
    SPECIAL_NONE,
    3,
    DSAT707_FAX_FAP_IDX,
    &dsat707_fap_dflm[0] },
/*-------------------------------------------------------------------------*/
  { "+FBO",
    EXTENDED | CONFIG | HEX | FCLASS2P0 | DOWN_LOAD,
    SPECIAL_NONE,
    1,
    DSAT707_FAX_FBO_IDX,
    &dsat707_fbo_dflm[0] },
/*-------------------------------------------------------------------------*/
  { "+FBS", 
    EXTENDED | READ_ONLY | FCLASS2P0,
    SPECIAL_FBS,
    0, 
    DSAT707_FAX_FBS_IDX,
    NULL },
/*-------------------------------------------------------------------------*/
  { "+FBU",
    EXTENDED | CONFIG | HEX | FCLASS2P0 | DOWN_LOAD,
    SPECIAL_NONE,
    1,
    DSAT707_FAX_FBU_IDX,
    &dsat707_fbu_dflm[0] },
/*-------------------------------------------------------------------------*/
  { "+FCQ",
    EXTENDED | CONFIG | HEX | FCLASS2P0 | DOWN_LOAD,
    SPECIAL_NONE,
    2, 
    DSAT707_FAX_FCQ_IDX,
    &dsat707_fcq_dflm[0] },
/*-------------------------------------------------------------------------*/
  { "+FCC",
    EXTENDED | CONFIG | HEX | ALWAYS | FCLASS2P0 | DOWN_LOAD | LOCAL_TEST,
    SPECIAL_FCC,
    8,   
    DSAT707_FAX_FCC_IDX,
    &dsat707_fcc_dflm[0]},
/*-------------------------------------------------------------------------*/
  { "+FCR",
    EXTENDED | CONFIG | HEX | ALWAYS | FCLASS2P0 | DOWN_LOAD,
    SPECIAL_NONE,
    1,
    DSAT707_FAX_FCR_IDX,
    &dsat707_fcr_dflm[0] },
/*-------------------------------------------------------------------------*/
  { "+FCT", 
    EXTENDED | CONFIG | HEX | FCLASS2P0 | DOWN_LOAD,
    SPECIAL_NONE,
    1, 
    DSAT707_FAX_FCT_IDX,
    &dsat707_fct_dflm[0] },
/*-------------------------------------------------------------------------*/
  { "+FDR",  
    EXTENDED | FCLASS2P0,
    SPECIAL_FDR,
    0,    
    DSAT707_FAX_FDR_IDX,
    NULL },
/*-------------------------------------------------------------------------*/
  { "+FDT",  
    EXTENDED | FCLASS2P0,
    SPECIAL_FDT,
    0,    
    DSAT707_FAX_FDT_IDX,
    NULL },
/*-------------------------------------------------------------------------*/
  { "+FEA",
    EXTENDED | CONFIG | HEX | FCLASS2P0 | DOWN_LOAD,
    SPECIAL_NONE,
    1,
    DSAT707_FAX_FEA_IDX ,
    &dsat707_fea_dflm[0] },
/*-------------------------------------------------------------------------*/
  { "+FFC", 
    EXTENDED | CONFIG | HEX | ALWAYS | FCLASS2P0 | DOWN_LOAD,
    SPECIAL_NONE, 
    4, 
    DSAT707_FAX_FFC_IDX,
    &dsat707_ffc_dflm[0] },
/*-------------------------------------------------------------------------*/
  { "+FHS", 
    EXTENDED | CONFIG | HEX | FCLASS2P0 | DOWN_LOAD,
    SPECIAL_FHS,
    1,   
    DSAT707_FAX_FHS_IDX,
    &dsat707_fhs_dflm[0] },
/*-------------------------------------------------------------------------*/
  { "+FIE",
    EXTENDED | CONFIG | HEX | FCLASS2P0 | DOWN_LOAD,
    SPECIAL_NONE,
    1,
    DSAT707_FAX_FIE_IDX,
    &dsat707_fie_dflm[0] },
/*-------------------------------------------------------------------------*/
  { "+FIP",
    EXTENDED | CONFIG | HEX | FCLASS2P0 | DOWN_LOAD,
    SPECIAL_NONE, 
    1,
    DSAT707_FAX_FIP_IDX,
    &dsat707_fip_dflm[0] },
/*-------------------------------------------------------------------------*/
  { "+FIS",
    EXTENDED | CONFIG | HEX | ALWAYS | FCLASS2P0 | DOWN_LOAD,
    SPECIAL_NONE,
    8,
    DSAT707_FAX_FIS_IDX,
    &dsat707_fis_dflm[0] },
/*-------------------------------------------------------------------------*/
  { "+FKS",     
    EXTENDED | FCLASS2P0,
    SPECIAL_NONE, 
    0,
    DSAT707_FAX_FKS_IDX,
    NULL },
/*-------------------------------------------------------------------------*/
  { "+FLI",  
    EXTENDED | CONFIG | STRING | FCLASS2P0 | DOWN_LOAD,
    SPECIAL_NONE,
    20,
    DSAT707_FAX_FLI_IDX,
    NULL },
/*-------------------------------------------------------------------------*/
  { "+FLO", 
    EXTENDED | CONFIG | HEX | FCLASS2P0 | LOCAL_TEST,
    SPECIAL_NONE,
    1, 
    DSAT707_FAX_FLO_IDX,
    &dsat707_flo_dflm[0] },
/*-------------------------------------------------------------------------*/
  { "+FLP",
    EXTENDED | CONFIG | HEX | FCLASS2P0 | DOWN_LOAD,
    SPECIAL_NONE,
    1, 
    DSAT707_FAX_FLP_IDX,
    &dsat707_flp_dflm[0] },
/*-------------------------------------------------------------------------*/
  { "+FMS",
    EXTENDED | CONFIG | HEX | FCLASS2P0 | DOWN_LOAD,
    SPECIAL_NONE,
    1,
    DSAT707_FAX_FMS_IDX,
    &dsat707_fms_dflm[0] },
/*-------------------------------------------------------------------------*/
  { "+FNR",
    EXTENDED | CONFIG | HEX | FCLASS2P0 | DOWN_LOAD,
    SPECIAL_NONE,
    4,
    DSAT707_FAX_FNR_IDX,
    &dsat707_fnr_dflm[0] },
/*-------------------------------------------------------------------------*/
  { "+FNS",
    EXTENDED | CONFIG | STRING | FCLASS2P0 | DOWN_LOAD,
    SPECIAL_FNS,
    180,  
    DSAT707_FAX_FNS_IDX,
    NULL },
/*-------------------------------------------------------------------------*/
  { "+FPA",
    EXTENDED | CONFIG | STRING | FCLASS2P0 | DOWN_LOAD,
    SPECIAL_NONE,
    20,
    DSAT707_FAX_FPA_IDX,
    NULL },
/*-------------------------------------------------------------------------*/
  { "+FPI", 
    EXTENDED | CONFIG | STRING | FCLASS2P0 | DOWN_LOAD,
    SPECIAL_NONE, 
    20,
    DSAT707_FAX_FPI_IDX,
    NULL },
/*-------------------------------------------------------------------------*/
  { "+FPP",
    EXTENDED | CONFIG | HEX | FCLASS2P0 | DOWN_LOAD,
    SPECIAL_NONE,  
    1,
    DSAT707_FAX_FPP_IDX,
    &dsat707_fpp_dflm[0] },
/*-------------------------------------------------------------------------*/
  { "+FPR",  
    EXTENDED | CONFIG | HEX | FCLASS2P0 | LOCAL_TEST,
    SPECIAL_NONE,  
    1,
    DSAT707_FAX_FPR_IDX,
    &dsat707_fpr_dflm[0] },
/*-------------------------------------------------------------------------*/
  { "+FPS", 
    EXTENDED | CONFIG | HEX | FCLASS2P0 | DOWN_LOAD,
    SPECIAL_NONE, 
    1,
    DSAT707_FAX_FPS_IDX,
    &dsat707_fps_dflm[0] },
/*-------------------------------------------------------------------------*/
  { "+FPW",
    EXTENDED | CONFIG | STRING | FCLASS2P0 | DOWN_LOAD,
    SPECIAL_NONE,  
    20,
    DSAT707_FAX_FPW_IDX,
    NULL },
/*-------------------------------------------------------------------------*/
  { "+FRQ",
    EXTENDED | CONFIG | HEX | ALWAYS | FCLASS2P0 | DOWN_LOAD,
    SPECIAL_NONE, 
    2,
    DSAT707_FAX_FRQ_IDX,
    &dsat707_frq_dflm[0] },
/*-------------------------------------------------------------------------*/
  { "+FRY",
    EXTENDED | CONFIG | HEX | FCLASS2P0 | DOWN_LOAD,
    SPECIAL_NONE,
    1, 
    DSAT707_FAX_FRY_IDX,
    &dsat707_fry_dflm[0] },
/*-------------------------------------------------------------------------*/
  { "+FSA",
    EXTENDED | CONFIG | STRING | FCLASS2P0 | DOWN_LOAD,
    SPECIAL_NONE, 
    20, 
    DSAT707_FAX_FSA_IDX,
    NULL },
/*-------------------------------------------------------------------------*/
  { "+FSP", 
    EXTENDED | CONFIG | HEX | FCLASS2P0 | DOWN_LOAD,
    SPECIAL_NONE,
    1,
    DSAT707_FAX_FSP_IDX,
    &dsat707_fsp_dflm[0] },
/*-------------------------------------------------------------------------*/
  /*lint -restore */
};

/* Size of FAX command table */
const unsigned int dsat707_fax_table_size = ARR_SIZE( dsat707_fax_table );


/*===========================================================================

FUNCTION   

DESCRIPTION
 For JCDMA 
  { 0  , 0  , 1    },  // VR: R8*3.85l/mm, R8*7.7l/mm 
  { 1  , 0  , 5    },  // BR: 2400,4800,7200,9600,12000,14400bps 
  { 0  , 0  , 2    },  // WD: 1728,2048,2432d/mm 
  { 0  , 0  , 2    },  // LN: A4(297mm), B4(364mm), Unlimited 
  { 0  , 0  , 1    },  // DF: MH, MR 
  { 0  , 0  , 0    },  // EC: disable 
  { 0  , 0  , 0    },  // BF: disable 
  { 0  , 0  , 7    },  // ST: 0,5,10,20,40ms 

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void dsat707faxctab_update_fcc_dflm(void)
{
  dsat707_fcc_dflm[0].default_v = 0;
  dsat707_fcc_dflm[0].lower = 0;
  dsat707_fcc_dflm[0].upper =1;

  dsat707_fcc_dflm[1].default_v = 1;
  dsat707_fcc_dflm[1].lower = 0;
  dsat707_fcc_dflm[1].upper =5;

  dsat707_fcc_dflm[2].default_v = 0;
  dsat707_fcc_dflm[2].lower = 0;
  dsat707_fcc_dflm[2].upper =4;

  dsat707_fcc_dflm[3].default_v = 0;
  dsat707_fcc_dflm[3].lower = 0;
  dsat707_fcc_dflm[3].upper =2;

  dsat707_fcc_dflm[4].default_v = 0;
  dsat707_fcc_dflm[4].lower = 0;
  dsat707_fcc_dflm[4].upper =3;

  dsat707_fcc_dflm[5].default_v = 0;
  dsat707_fcc_dflm[5].lower = 0;
  dsat707_fcc_dflm[5].upper =1;

  dsat707_fcc_dflm[6].default_v = 0;
  dsat707_fcc_dflm[6].lower = 0;
  dsat707_fcc_dflm[6].upper =1;

  dsat707_fcc_dflm[7].default_v = 0;
  dsat707_fcc_dflm[7].lower = 0;
  dsat707_fcc_dflm[7].upper =7;

  /*
    These parameters are different for JCDMA. 
    So update them.
  */
  if(ds707_roaming_is_curr_mode_jcdma() == TRUE)
  {
    dsat707_fcc_dflm[2].default_v = 0;
    dsat707_fcc_dflm[2].lower = 0;
    dsat707_fcc_dflm[2].upper =2;

    dsat707_fcc_dflm[4].default_v = 0;
    dsat707_fcc_dflm[4].lower = 0;
    dsat707_fcc_dflm[4].upper =1;

    dsat707_fcc_dflm[5].default_v = 0;
    dsat707_fcc_dflm[5].lower = 0;
    dsat707_fcc_dflm[5].upper =0;

    dsat707_fcc_dflm[6].default_v = 0;
    dsat707_fcc_dflm[6].lower = 0;
    dsat707_fcc_dflm[6].upper =0;
  }

}
#endif /* FEATURE_DATA_IS707_ASYNC */

