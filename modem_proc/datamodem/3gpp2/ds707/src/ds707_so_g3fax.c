/*===========================================================================

                          D S 7 0 7 _ S O _ G 3 F A X
DESCRIPTION
  This file contains functions related to configuring service options for
  IS-707 circuit switched data calls (async data).

EXTERNALIZED FUNCTIONS
  ds707_so_g3fax_recal
    Re-does the async SO states, based on current settings.

  ds707_so_g3fax_init
    Any power-up initialization that needs to be performed for
    Group 3 fax service options should be done here.

  ds707_so_g3fax_get_orig
    Returns the SO to originate with.

  ds707_so_g3fax_so_is_g3fax
     Returns TRUE if passed in SO is a g3fax SO; else returns FALSE.

 Copyright (c) 2002-2013 by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_so_g3fax.c_v   1.4   06 Nov 2002 14:40:16   akhare  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/src/ds707_so_g3fax.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $
  
when        who    what, where, why
--------    ---    ----------------------------------------------------------
06/03/13    sd     ATCoP decoupling changes
07/22/11    dvk    Global Variable Cleanup
02/11/11    ack    Global Variable Cleanup
07/08/10    ash    Fixed compiler warnings.
02/10/09    ss     Off target Lint Fixes
01/22/09    sn     Added support to disable CS Service Options.
02/21/08    mga    Made changes to mainline FEATURE_SNM_ENHANCEMENTS
05/07/10    ac     Lint fix
09/08/04    fc     Service negotiation changes for DV.
10/28/03    ak     Now update JCDMA page rsp when recalculating SO info.
10/15/02    ak     Updated FEATURE_JCDMA_DS to FEATURE_JCDMA_1X
10/14/02    atp    Cleanup, added ds707_so_g3fax_so_is_g3fax().
10/10/02    ak     Updated for JCDMA M51/1X by making so_list public and
                   moving #define of number fax SO's into .h.
02/20/02    ak     First version of file.
===========================================================================*/


/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "target.h"
#include "customer.h"

#ifdef FEATURE_DATA_IS707_ASYNC
#include "cai.h"
#include "dec5000.h"
#include "ds707.h"
#include "ds707_so_g3fax.h"
#include "ds707_socm.h"
#include "dsat707vendctab.h"
#include "err.h"
#include "snm.h"

#include "ds707_so_jcdma.h"
#include "ds_1x_profile.h"

#include "ds707_roaming.h"
#include "dsat707mip.h"
#include "ds707_pkt_mgr.h"
#include "ds707_nv_util.h"
#include "ds_1x_profile.h"

/*===========================================================================
                                 TYPEDEFS
===========================================================================*/
typedef struct
{
  const snm_config_type   *default_cfg;        /* Default configuration    */
  const snm_neg_par_type  *neg_par;            /* Negotiation parameters   */
} ds707_so_g3fax_param_type;

/*===========================================================================
                          G3 FAX SERVICE OPTIONS
===========================================================================*/
static const ds707_so_type ds707_so_g3fax_list[] =
{
  CAI_SO_G3_FAX_PRE707,                 /* SO 5      = Pre-707, RS1        */
  CAI_SO_G3_FAX_IS707,                  /* SO 4101   = IS-707, RS1         */
  CAI_SO_G3_FAX_13K_PRE707,             /* SO 13     = Pre-707, RS2        */
  CAI_SO_G3_FAX_13K_PROPTRY             /* SO 0x8022 = QC Proprietary, RS2 */
};

/*---------------------------------------------------------------------------
  Table indicating if the SO is enabled/disabled under certain settings.
---------------------------------------------------------------------------*/
LOCAL const boolean ds707_so_g3fax_tbl[DS_SO_SET_COUNT]
                                      [DS707_SO_G3FAX_NUM] =
{
#ifndef FEATURE_DATA_DISABLE_CS_SO
/*                   5     4101     13     0x8022                          */
/* QCSO = 0 */   { TRUE,   FALSE,  TRUE,   FALSE },
/* QCSO = 1 */   { TRUE,   FALSE,  FALSE,  TRUE  },
/* QCSO = 2 */   { FALSE,  TRUE,   TRUE,   FALSE }
#else
/*                   5     4101     13     0x8022                          */
/* QCSO = 0 */   { FALSE,  FALSE,  FALSE,  FALSE },
/* QCSO = 1 */   { FALSE,  FALSE,  FALSE,  FALSE },
/* QCSO = 2 */   { FALSE,  FALSE,  FALSE,  FALSE }
#endif /* FEATURE_DATA_DISABLE_CS_SO */
};

/*---------------------------------------------------------------------------
  FAX SO to originate with.
---------------------------------------------------------------------------*/
LOCAL const ds707_so_type ds707_so_g3fax_orig_tbl[DS_SO_SET_COUNT]
                                                 [DS_RATESET_COUNT] =
{
  /*    RATE SET 1                         RATE SET 2                      */
  { CAI_SO_G3_FAX_PRE707,        CAI_SO_G3_FAX_13K_PRE707  }, /* QCSO = 0  */
  { CAI_SO_G3_FAX_PRE707,        CAI_SO_G3_FAX_13K_PROPTRY }, /* QCSO = 1  */
  { CAI_SO_G3_FAX_IS707,         CAI_SO_G3_FAX_13K_PRE707  }  /* QCSO = 2  */
};


/*---------------------------------------------------------------------------
  Definitions for Service Option 5, 9.6 kbps G3 Fax (IS-99).
---------------------------------------------------------------------------*/
LOCAL snm_config_type  ds707_so_g3fax5_default_cfg =
{
  {
    /* Forward link defaults */
    CAI_MUX1,                      /* Default forward multiplex option = 1 */
    CAI_RATE_9600,                 /* Forward rate set = rate set 1        */
    CAI_RS1_9600 | CAI_RS1_4800 | CAI_RS1_1200, /* Use Rate 1, 1/2 & 1/8   */
    CAI_PRI_TRAF                   /* Primary traffic only                 */
  },
  {
    /* Reverse link defaults */
    CAI_MUX1,                      /* Default reverse multiplex option = 1 */
    CAI_RATE_9600,                 /* Reverse rate set = 1                 */
    CAI_RS1_9600 | CAI_RS1_4800 | CAI_RS1_1200, /* Use Rate 1, 1/2 & 1/8   */
    CAI_PRI_TRAF                   /* Primary traffic only                 */
  }
};

LOCAL const snm_neg_par_type  ds707_so_g3fax5_neg_par = 
{
  1,                                    /* Minimum PREV allowed = 1        */
  {                                     /* Alternate service options       */
    CAI_SO_G3_FAX_13K_PRE707,           /* SO 13                           */
    CAI_SO_G3_FAX_13K_PROPTRY,          /* SO 0x8022                       */
    CAI_SO_G3_FAX_IS707,                /* SO 4101                         */
    CAI_SO_NULL,
    0,
    0,
    0
  },
  {                                     /* Compatible service options      */
    CAI_SO_NULL,
    0
  },
  {CAI_MUX1, CAI_MUX1},                 /* Maximum fwd and rev mux options */
  {
    3,                                  /* number of rc's in the list      */
    {                                   /* forward radio channel           */
      CAI_RC_3,                         /* 1X, RS = 1 (preferred)          */ 
      CAI_RC_4,                         /* 1X, RS = 1                      */ 
      CAI_RC_1,                          /* IS-95, RS = 1                   */ 
      CAI_INIT_RC,
      CAI_INIT_RC
    }
  },
 {                                      /* reverse radio channel           */
    2,                                  /* number of rc's in the list      */
    {
      CAI_RC_3,                         /* IS-2000, 9600 bps               */
      CAI_RC_1,                         /* IS-95, RS = 1                   */ 
      (cai_radio_config_type)0,
      CAI_INIT_RC,
      CAI_INIT_RC
    }
  },
  {
    (byte)0,                              /* # of mux options for fwd sch  */
    {
      (word)0, (word)0, (word)0, (word)0, /* preferences for fwd sch mos   */
      (word)0, (word)0, (word)0, (word)0,
      (word)0, (word)0, (word)0, (word)0,
      (word)0, (word)0, (word)0, (word)0,
      (word)0, (word)0
    }
  },
  {
    (byte)0,                              /* # of mux options for rev sch  */
    {
      (word)0, (word)0, (word)0, (word)0, /* preferences for rev sch mos   */
      (word)0, (word)0, (word)0, (word)0,
      (word)0, (word)0, (word)0, (word)0,
      (word)0, (word)0, (word)0, (word)0,
      (word)0, (word)0
    }
  }
  , {SNM_FCH, SNM_RESERVED, SNM_RESERVED}

#ifdef FEATURE_IS2000_REL_A
  , CAI_SO_GROUP_DIGITAL_FACSIMILE
  , CAI_SO_BITMAP_SO_G3_FAX_PRE707
#endif /* FEATURE_IS2000_REL_A */
};

/*---------------------------------------------------------------------------
  Definitions for Service Option 4101, 9.6 kbps Group 3 Fax (IS-707-A.3).
---------------------------------------------------------------------------*/
LOCAL snm_config_type  ds707_so_g3fax4101_default_cfg =
{
  {
    /* Forward link defaults */
    CAI_MUX1,                      /* Default forward multiplex option = 1 */
    CAI_RATE_9600,                 /* Forward rate set = rate set 1        */
    CAI_RS1_9600 | CAI_RS1_4800 | CAI_RS1_1200, /* Use Rate 1, 1/2 & 1/8   */
    CAI_PRI_TRAF                   /* Primary traffic only                 */
  },
  {
    /* Reverse link defaults */
    CAI_MUX1,                      /* Default reverse multiplex option = 1 */
    CAI_RATE_9600,                 /* Reverse rate set = 1                 */
    CAI_RS1_9600 | CAI_RS1_4800 | CAI_RS1_1200, /* Use Rate 1, 1/2 & 1/8   */
    CAI_PRI_TRAF                   /* Primary traffic only                 */
  }
};

LOCAL const snm_neg_par_type  ds707_so_g3fax4101_neg_par = 
{
  1,                                    /* Minimum PREV allowed = 1        */
  {                                     /* Alternate service options       */
    CAI_SO_G3_FAX_13K_PRE707,           /* SO 13                           */
    CAI_SO_G3_FAX_13K_PROPTRY,          /* SO 0x8022                       */
    CAI_SO_G3_FAX_PRE707,               /* SO 5                            */
    CAI_SO_NULL,
    0,
    0,
    0
  },
  {                                     /* Compatible service options      */
    CAI_SO_NULL,
    0
  },
  {CAI_MUX1, CAI_MUX1}                  /* Maximum fwd and rev mux options */
 ,{
    3,                                  /* number of rc's in the list      */
    {                                   /* forward radio channel           */
      CAI_RC_3,                         /* 1X, RS = 1 (preferred)          */ 
      CAI_RC_4,                         /* 1X, RS = 1                      */ 
      CAI_RC_1,                          /* IS-95, RS = 1                   */ 
      CAI_INIT_RC,
      CAI_INIT_RC
    }
  },
 {                                      /* reverse radio channel           */
    2,                                  /* number of rc's in the list      */
    {
      CAI_RC_3,                         /* IS-2000, 9600 bps               */
      CAI_RC_1,                         /* IS-95, RS = 1                   */ 
      (cai_radio_config_type)0,
      CAI_INIT_RC,
      CAI_INIT_RC
    }
  },
  {
    (byte)0,                              /* # of mux options for fwd sch  */
    {
      (word)0, (word)0, (word)0, (word)0, /* preferences for fwd sch mos   */
      (word)0, (word)0, (word)0, (word)0,
      (word)0, (word)0, (word)0, (word)0,
      (word)0, (word)0, (word)0, (word)0,
      (word)0, (word)0
    }
  },
  {
    (byte)0,                              /* # of mux options for rev sch  */
    {
      (word)0, (word)0, (word)0, (word)0, /* preferences for rev sch mos   */
      (word)0, (word)0, (word)0, (word)0,
      (word)0, (word)0, (word)0, (word)0,
      (word)0, (word)0, (word)0, (word)0,
      (word)0, (word)0
    }
  }
  , {SNM_FCH, SNM_RESERVED, SNM_RESERVED}

#ifdef FEATURE_IS2000_REL_A
  , CAI_SO_GROUP_DIGITAL_FACSIMILE
  , CAI_SO_BITMAP_SO_G3_FAX_IS707
#endif /* FEATURE_IS2000_REL_A */
};

/*---------------------------------------------------------------------------
  Definitions for Service Option 13, 14.4 kbps Group 3 Fax (IS-707-A.3).
---------------------------------------------------------------------------*/
LOCAL const snm_config_type  ds707_so_g3fax13_default_cfg =
{
  {
    /* Forward link defaults */
    CAI_MUX2,                      /* Default forward multiplex option = 2 */
    CAI_RATE_14400,                /* Forward rate set = rate set 2        */
    CAI_RS2_ALL,                   /* All transmission rates used          */
    CAI_PRI_TRAF                   /* Primary traffic only                 */
  },
  {
    /* Reverse link defaults */
    CAI_MUX2,                      /* Default reverse multiplex option = 2 */
    CAI_RATE_14400,                /* Reverse rate set = 2                 */
    CAI_RS2_ALL,                   /* All transmission rates used          */
    CAI_PRI_TRAF                   /* Primary traffic only                 */
  }
};

LOCAL const snm_neg_par_type  ds707_so_g3fax13_neg_par = 
{
  1,                                    /* Minimum PREV allowed = 1        */
  {                                     /* Alternate service options       */
    CAI_SO_G3_FAX_13K_PROPTRY,          /* SO 0x8022                       */
    CAI_SO_G3_FAX_IS707,                /* SO 4101                         */
    CAI_SO_G3_FAX_PRE707,               /* SO 5                            */
    CAI_SO_NULL,
    0,
    0,
    0
  },
  {                                     /* Compatible service options      */
    CAI_SO_NULL,
    0
  },
  {CAI_MUX2, CAI_MUX2}                  /* Maximum fwd and rev mux options */
 ,{
    2,                                  /* number of rc's in the list      */
    {                                   /* forward radio channel           */
      CAI_RC_5,                         /* IS-2000, 14,400 bps             */
      CAI_RC_2,                         /* IS-95, RS = 2                   */ 
      (cai_radio_config_type)0,
      CAI_INIT_RC,
      CAI_INIT_RC
    }
  },
 {                                      /* reverse radio channel           */
    2,                                  /* number of rc's in the list      */
    {
      CAI_RC_4,                         /* 1X, RS = 2                      */  
      CAI_RC_2,                         /* IS-95, RS = 2                   */ 
      (cai_radio_config_type)0,
      CAI_INIT_RC,
      CAI_INIT_RC
    }
  },
  {
    (byte)0,                              /* # of mux options for fwd sch  */
    {
      (word)0, (word)0, (word)0, (word)0, /* preferences for fwd sch mos   */
      (word)0, (word)0, (word)0, (word)0,
      (word)0, (word)0, (word)0, (word)0,
      (word)0, (word)0, (word)0, (word)0,
      (word)0, (word)0
    }
  },
  {
    (byte)0,                              /* # of mux options for rev sch  */
    {
      (word)0, (word)0, (word)0, (word)0, /* preferences for rev sch mos   */
      (word)0, (word)0, (word)0, (word)0,
      (word)0, (word)0, (word)0, (word)0,
      (word)0, (word)0, (word)0, (word)0,
      (word)0, (word)0
    }
  }
  , {SNM_FCH , SNM_RESERVED ,SNM_RESERVED}

#ifdef FEATURE_IS2000_REL_A
  , CAI_SO_GROUP_DIGITAL_FACSIMILE
  , CAI_SO_BITMAP_SO_G3_FAX_13K_PRE707
#endif /* FEATURE_IS2000_REL_A */
};


/*---------------------------------------------------------------------------
  Definitions for Service Option 0x8022, 14.4 kbps Group 3 Fax (Proprietary).
---------------------------------------------------------------------------*/
LOCAL const snm_config_type  ds707_so_g3fax8022h_default_cfg =
{
  {
    /* Forward link defaults */
    CAI_MUX2,                      /* Default forward multiplex option = 2 */
    CAI_RATE_14400,                /* Forward rate set = rate set 2        */
    CAI_RS2_ALL,                   /* All transmission rates used          */
    CAI_PRI_TRAF                   /* Primary traffic only                 */
  },
  {
    /* Reverse link defaults */
    CAI_MUX2,                      /* Default reverse multiplex option = 2 */
    CAI_RATE_14400,                /* Reverse rate set = 2                 */
    CAI_RS2_ALL,                   /* All transmission rates used          */
    CAI_PRI_TRAF                   /* Primary traffic only                 */
  }
};

LOCAL const snm_neg_par_type  ds707_so_g3fax8022h_neg_par = 
{
  1,                                    /* Minimum PREV allowed = 1        */
  {                                     /* Alternate service options       */
    CAI_SO_G3_FAX_13K_PRE707,           /* SO 13                           */
    CAI_SO_G3_FAX_IS707,                /* SO 4101                         */
    CAI_SO_G3_FAX_PRE707,               /* SO 5                            */
    CAI_SO_NULL,
    0,
    0,
    0
  },
  {                                     /* Compatible service options      */
    CAI_SO_NULL,
    0
  },
  {CAI_MUX2, CAI_MUX2}                  /* Maximum fwd and rev mux options */
 ,{
    2,                                  /* number of rc's in the list      */
    {                                   /* forward radio channel           */
      CAI_RC_5,                         /* IS-2000, 14,400 bps             */
      CAI_RC_2,                         /* IS-95, RS = 2                   */ 
      (cai_radio_config_type)0,
      CAI_INIT_RC,
      CAI_INIT_RC
    }
  },
 {                                      /* reverse radio channel           */
    2,                                  /* number of rc's in the list      */
    {
      CAI_RC_4,                         /* 1X, RS = 2                      */  
      CAI_RC_2,                         /* IS-95, RS = 2                   */ 
      (cai_radio_config_type)0,
      CAI_INIT_RC,
      CAI_INIT_RC
    }
  },
  {
    (byte)0,                              /* # of mux options for fwd sch  */
    {                                     
      (word)0, (word)0, (word)0, (word)0, /* preferences for fwd sch mos   */
      (word)0, (word)0, (word)0, (word)0,
      (word)0, (word)0, (word)0, (word)0,
      (word)0, (word)0, (word)0, (word)0,
      (word)0, (word)0
    }
  },
  {
    (byte)0,                              /* # of mux options for rev sch  */
    {
      (word)0, (word)0, (word)0, (word)0, /* preferences for rev sch mos   */
      (word)0, (word)0, (word)0, (word)0,
      (word)0, (word)0, (word)0, (word)0,
      (word)0, (word)0, (word)0, (word)0,
      (word)0, (word)0
    }
  }
  , {SNM_FCH, SNM_RESERVED, SNM_RESERVED}

#ifdef FEATURE_IS2000_REL_A
  , CAI_SO_GROUP_UNASSIGNED
  , CAI_SO_BITMAP_POSITION_UNASSIGNED
#endif /* FEATURE_IS2000_REL_A */
};

/*---------------------------------------------------------------------------
  Array to store the default config and negotiation parameter data structures
  for each g3 fax service option (from the g3fax_so_list[]).
  NOTE: The order and number of entries in this table must correspond to the
        order in which the service options are listed in the g3fax_so_list[],
        and the number of entries in the g3fax_so_list[], respectively.
---------------------------------------------------------------------------*/
LOCAL ds707_so_g3fax_param_type ds707_so_g3fax_param[DS707_SO_G3FAX_NUM] =
{
  {&ds707_so_g3fax5_default_cfg,     &ds707_so_g3fax5_neg_par},   /* 5     */
  {&ds707_so_g3fax4101_default_cfg,  &ds707_so_g3fax4101_neg_par},/* 4101  */
  {&ds707_so_g3fax13_default_cfg,    &ds707_so_g3fax13_neg_par},  /* 13    */
  {&ds707_so_g3fax8022h_default_cfg, &ds707_so_g3fax8022h_neg_par}/* 0x8022*/
};

/*===========================================================================
                   INTERNAL FUNCTION DEFINITIONS
===========================================================================*/
/*===========================================================================
FUNCTION      

DESCRIPTION   

DEPENDENCIES  

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
LOCAL void ds707_so_g3fax5i_default_cfg_init( void)
{

 if(ds707_roaming_is_curr_mode_jcdma() == TRUE)
 {
   ds707_so_g3fax5_default_cfg.forward.tx_rates = CAI_RS1_ALL;
   ds707_so_g3fax5_default_cfg.reverse.tx_rates = CAI_RS1_ALL;
 }
 else
 {
  ds707_so_g3fax5_default_cfg.forward.tx_rates =
     CAI_RS1_9600 | CAI_RS1_4800 | CAI_RS1_1200;/* Use Rate 1, 1/2 & 1/8   */
  ds707_so_g3fax5_default_cfg.reverse.tx_rates = 
    CAI_RS1_9600 | CAI_RS1_4800 | CAI_RS1_1200; /* Use Rate 1, 1/2 & 1/8   */
 }
}

/*===========================================================================
FUNCTION      

DESCRIPTION   

DEPENDENCIES  

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
LOCAL void ds707_so_g3fax4101i_default_cfg_init( void)
{

 if(ds707_roaming_is_curr_mode_jcdma() == TRUE)
 {
   ds707_so_g3fax4101_default_cfg.forward.tx_rates = CAI_RS1_ALL;
   ds707_so_g3fax4101_default_cfg.reverse.tx_rates = CAI_RS1_ALL;
 }
 else
 {
  ds707_so_g3fax4101_default_cfg.forward.tx_rates =
     CAI_RS1_9600 | CAI_RS1_4800 | CAI_RS1_1200; /* Use Rate 1, 1/2 & 1/8   */
  ds707_so_g3fax4101_default_cfg.reverse.tx_rates = 
    CAI_RS1_9600 | CAI_RS1_4800 | CAI_RS1_1200; /* Use Rate 1, 1/2 & 1/8   */
 }
}
/*===========================================================================
FUNCTION      DS707_SO_G3FAXI_REGISTER

DESCRIPTION   Register each defined g3 fax service option (from the 
              g3fax_so_list[]) with the Service Negotiation Manager, and
              check that the registration was successful.

DEPENDENCIES  An instance of snm_config_type (default configuration) and 
              snm_neg_par_type (negotiation parameters) should have been
              defined for each service option being registered, and should
              have been initialized with meaningful values prior to calling
              this function.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
LOCAL void ds707_so_g3faxi_register(void)
{
  byte  i;
  uint8 index=0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds707_so_g3fax5i_default_cfg_init();
  ds707_so_g3fax4101i_default_cfg_init();
  /*-------------------------------------------------------------------------
    Add each service option in the g3fax_so_list[] to the SNM.
  -------------------------------------------------------------------------*/
  for( i = 0; i < DS707_SO_G3FAX_NUM; i++ )
  {
    if( !ds707_nv_util_is_refresh_in_progress(ds707_curr_subs_id()))
    {
    if( snm_add_so((word)ds707_so_g3fax_list[i],
                   (snm_neg_par_type*)(ds707_so_g3fax_param[i].neg_par),
                   ds707_so_g3fax_param[i].default_cfg) == SNM_SUCCESS )
    {
      /*---------------------------------------------------------------------
        The service option was added successfully, register the service
        option control message handler (the same handler is used for all
        packet data service options).
      ---------------------------------------------------------------------*/
      (void)snm_set_cont_msg_handler((word)ds707_so_g3fax_list[i],
                               ds707_socm_parse_socm);
    }
    else
    {
      /*---------------------------------------------------------------------
        The service option registration failed.
      ---------------------------------------------------------------------*/
      ERR_FATAL("Couldn't add SO %d to SNM", ds707_so_g3fax_list[i], 0, 0);
    }
  }
  }
} /* ds707_so_g3faxi_register() */

/*===========================================================================
FUNCTION      DS707_SO_G3FAXI_SET_STATE

DESCRIPTION   Lookup the state of each g3 fax service option based on the
              current Service Option Set in use. Enable or disable each G3
              fax service option as per the table.

DEPENDENCIES  Uses the global variable ds707_qcso_val to determine the
              current setting of the service option set.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
LOCAL void ds707_so_g3faxi_set_state(void)
{
  ds707_so_type        fax_so;
  byte                 i;
  ds707_mip_profile_type  *ds_static_profile_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_static_profile_ptr = ds707_pkt_mgr_get_1x_data_profile_ptr();
  if( ds_static_profile_ptr == NULL )
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR, 
                     "NULL pointer received from "
                     "ds707_pkt_mgr_get_1x_data_profile_ptr" );
    ASSERT(0);
  }

  for( i = 0; i < DS707_SO_G3FAX_NUM; i++ )
  {
    fax_so = ds707_so_g3fax_list[i];
    if (ds707_so_g3fax_tbl[ds_static_profile_ptr->ds707.ds707_qcso_val][i]
                           == TRUE)
    {
      /*---------------------------------------------------------------------
        Invoke the SNM function to enable this service option.
      ---------------------------------------------------------------------*/
      (void) snm_enable_so((word)fax_so);
    }
    else
    {
      /*---------------------------------------------------------------------
        Invoke the SNM function to disable this service option.
      ---------------------------------------------------------------------*/
      (void) snm_disable_so((word)fax_so);
    }
  } /* for i = 0 to DS707_SO_G3FAX_NUM */
} /* ds707_so_g3faxi_set_state() */


/*===========================================================================
                   INTERNAL FUNCTION DEFINITIONS
===========================================================================*/
/*===========================================================================
FUNCTION      DS707_SO_G3FAX_RECAL

DESCRIPTION   Called when the G3 FAX SO's need to be recalibrated for
              enabling/disabling with SNM, when some mobile setting has
              changed.

DEPENDENCIES  This function should be called only after ATCOP has been
              initialized, since it invokes functions that use some AT
              command settings.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_so_g3fax_recal(void)
{
  if(ds707_roaming_is_curr_mode_jcdma() == TRUE)
  {
    ds707_so_jcdma_g3fax_set_state();
    ds707_so_jcdma_g3fax_set_page_rsp();
  }
  else
  {
    ds707_so_g3faxi_set_state();
  }
}

/*===========================================================================
FUNCTION      DS707_SO_G3FAX_INIT

DESCRIPTION   Wrapper function for register_g3fax_so() and set_g3fax_so
              state() functions, in order to simplify the external interface.
              Any power-up initialization that needs to be performed for
              Group 3 fax service options should be done here.

DEPENDENCIES  This function should be called only after ATCOP has been
              initialized, since it invokes functions that use some AT
              command settings.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_so_g3fax_init(void)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Register all the defined g3 fax service options with the service
    negotiation manager.
  -------------------------------------------------------------------------*/
  ds707_so_g3faxi_register();

  /*-------------------------------------------------------------------------
    Enable/disable the appropriate g3 fax service options as per the current
    operating mode.
  -------------------------------------------------------------------------*/
  ds707_so_g3fax_recal();
} /* ds707_so_g3fax_init() */


/*===========================================================================
FUNCTION       DS707_SO_G3FAX_GET_ORIG

DESCRIPTION    Returns the SO to orig with.

DEPENDENCIES   NONE

RETURN VALUE   Service option to originate with.

SIDE EFFECTS   None
===========================================================================*/
ds707_so_type ds707_so_g3fax_get_orig
(
  ds707_rateset_type  rateset
)
{
  ds707_mip_profile_type* ds_1x_runtime_profile_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ds_1x_runtime_profile_ptr = ds_get_1x_runtime_profile_ptr();
  if(ds707_roaming_is_curr_mode_jcdma() == TRUE)
  {
    return(ds707_so_jcdma_g3fax_get_orig(rateset));
  }
  else
  {
    return(ds707_so_g3fax_orig_tbl
             [ds_1x_runtime_profile_ptr->ds707.ds707_qcso_val][rateset]);
  }
} /* ds707_so_g3fax_get_orig() */


/*===========================================================================
FUNCTION       DS707_SO_G3FAX_SO_IS_G3FAX

DESCRIPTION    Returns TRUE if passed in SO is a g3fax SO.  Else returns
               FALSE.

DEPENDENCIES   NONE

RETURN VALUE   See DESCRIPTION

SIDE EFFECTS   None
===========================================================================*/
boolean ds707_so_g3fax_so_is_g3fax
(
  ds707_so_type    so
)
{
  int i;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  for (i = 0; i < DS707_SO_G3FAX_NUM; i++)
  {
    if (ds707_so_g3fax_list[i] == so)
    {
      return(TRUE);
    }
  }

  return(FALSE);
} /* ds707_so_g3fax_so_is_g3fax() */

/*===========================================================================
FUNCTION       DS707_GET_SO_G3FAX_TYPE

DESCRIPTION    Accessor function for G3 Fax Service Option

DEPENDENCIES   NONE

RETURN VALUE   Returns the requested G3 Fax SO

SIDE EFFECTS   None
===========================================================================*/
ds707_so_type ds707_get_so_g3fax_type
(
  byte index
)
{
  return (ds707_so_g3fax_list[index]);

} /* ds707_get_so_g3fax_type */


#endif /* FEATURE_DATA_IS707_ASYNC */

