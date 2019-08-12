/*===========================================================================

                          D S 7 0 7 _ J C D M A _ S O
GENERAL DESCRIPTION

 Copyright (c) 2002-2011 by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_so_jcdma.c_v   1.5   11 Dec 2002 22:26:28   akhare  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/jcdma/src/ds707_so_jcdma.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $
  
when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/30/14   niv     Added wrapper functions & API change for 
                   NV_REFRESH 
03/30/11   ms      Global variable cleanup
03/15/11   mg      Global variable cleanup
02/11/11   ack     Global Variable Cleanup
10/19/10   ms      Added debug messages for all Assert(0)s.
10/18/10   op      Migrated to MSG 2.0 macros
09/13/10   op      Changed DS707_CBACK_MSG macro to use MSG 2.0 format
07/06/10   gc      Fixed compiler warnings
01/22/09   sn      Added support to disable CS Service Options.
10/13/08   cs      Klocwork Fixes
08/15/07   ack     Fixed Lint Warnings
08/01/07 ack/squ   Fixed Lint Warnings
04/20/04   jd      Updated init to reflect new 707_cback_reg() prototype
02/04/04   ak      Updated for new Pkt Mgr Events
11/19/03   ak      get p_rev_index before trying to use it.
12/12/02   ak      Fixed typos - m511 -> m512
10/22/02   rsl     Updated FEATURE_JCDMA_DS_1X to FEATURE_JCDMA_1X
10/15/02   ak      Updated FEATURE_JCDMA_DS to FEATURE_JCDMA_DS_1X
10/10/02   ak      Updated for JCDMA M51/1X.
02/20/02   ak      First version of file.
===========================================================================*/


/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "target.h"
#include "customer.h"

#include "data_msg.h"
#include "ds707.h"
#include "ds707_cback.h"
#include "ds707_so_jcdma.h"
#include "ds707_jcdma_m51.h"
#include "ds707_jcdma_apprate.h"
#include "ds707_roaming.h"
#include "ds707_so_pkt.h"
#include "ds707_so_async.h"
#include "ds707_so_g3fax.h"
#include "ds707_p_rev.h"
#include "ds707_pkt_mgr.h"
#include "ps_phys_link.h"
#include "snm.h"
#include "sys.h"
#include "data_err_suppr.h"

/*===========================================================================
                        TYPEDEFS and VARIABLE DECLARATIONS
===========================================================================*/
typedef enum
{
  DS707_JCDMA_DORM_MIN = -1,
  DS707_JCDMA_DORM_IS_NOT_DORM,  
  DS707_JCDMA_DORM_IS_DORM,
  DS707_JCDMA_DORM_MAX
}ds707_jcdma_dorm_e_type;


/*===========================================================================
                       PACKET SERVICE OPTIONS
===========================================================================*/
/*---------------------------------------------------------------------------
  Packet SO's enable/disable table.  Based solely on P_REV.
---------------------------------------------------------------------------*/
LOCAL const boolean ds707_so_jcdma_pkt_tbl[DS707_NUM_P_REVS]
                                          [DS707_SO_PKT_NUM] =
{
#ifndef FEATURE_DATA_DISABLE_IS95B_MDR 
/*===========================================================================
  P _ R E V = 3  (Lowspeed data, RLP 1)
===========================================================================*/
/* 33,      25,    22,    0x8020,   15,    4103,     7                     */
  { FALSE,  FALSE,  FALSE,   TRUE,   TRUE,   TRUE,   TRUE },

/*===========================================================================
  P _ R E V = 5  (Medium data rate, RLP 2)
===========================================================================*/
/* 33,      25,    22,    0x8020,   15,    4103,     7                     */
  { FALSE,   TRUE,  TRUE,    TRUE,   TRUE,   TRUE,    TRUE },

/*===========================================================================
  P _ R E V = 6  (1X, RLP 3)
===========================================================================*/
/* 33,      25,    22,    0x8020,   15,    4103,     7                     */
  { TRUE,    TRUE,  TRUE,    TRUE,   TRUE,   TRUE,    TRUE }

#else /*Disables SO : 25, 22, 0x8020, 4103 For KDDI, MS needs to propose SO15 instead.*/

/*===========================================================================
  P _ R E V = 3  (Lowspeed data, RLP 1)
===========================================================================*/
/* 33,      25,    22,    0x8020,   15,    4103,     7                     */
  { FALSE,  FALSE,  FALSE,   FALSE,   TRUE,   FALSE,   TRUE },

/*===========================================================================
  P _ R E V = 5  (Medium data rate, RLP 2)
===========================================================================*/
/* 33,      25,    22,    0x8020,   15,    4103,     7                     */
    { FALSE,   FALSE,  FALSE,    FALSE,   TRUE,   FALSE,    TRUE },

/*===========================================================================
  P _ R E V = 6  (1X, RLP 3)
===========================================================================*/
/* 33,      25,    22,    0x8020,   15,    4103,     7                     */
    { TRUE,    FALSE,  FALSE,    FALSE,   TRUE,   FALSE,    TRUE }

#endif
};


/*---------------------------------------------------------------------------
  Packet SO's origination params.  Always assume Rateset 2.
---------------------------------------------------------------------------*/
LOCAL const ds707_so_type ds707_so_jcdma_pkt_orig_tbl[DS707_NUM_P_REVS]
                                                     [SYS_JCDMA_M512_MAX]
                                                     [DS707_JCDMA_APPRATE_MAX] =
{
/*-------------------------------------------------------------------------*/
/* PREV = 3                                                                */
/*-------------------------------------------------------------------------*/
/*        M51 = 0 (Lowspeed)                                 SCH RATE      */
  {
    {
    CAI_SO_PPP_PKT_DATA_13K_PRE707,                    /*    Standard      */
    CAI_SO_PPP_PKT_DATA_13K_PRE707,                    /*    Hi - Fwd      */
    CAI_SO_PPP_PKT_DATA_13K_PRE707                    /*    Hi - Both     */
    },
/*        M51 = 1 (Highspeed)                                SCH RATE      */
    {
    CAI_SO_PPP_PKT_DATA_13K_PRE707,                    /*    Standard      */
    CAI_SO_PPP_PKT_DATA_13K_PRE707,                    /*    Hi - Fwd      */
    CAI_SO_PPP_PKT_DATA_13K_PRE707                    /*    Hi - Both     */
    }
  },
/*-------------------------------------------------------------------------*/
/* PREV = 5                                                                */
/*-------------------------------------------------------------------------*/
/*        M51 = 0 (Lowspeed)                                 SCH RATE      */
  {
    {
    CAI_SO_PPP_PKT_DATA_13K_PRE707,                    /*    Standard      */
    CAI_SO_PPP_PKT_DATA_13K_PRE707,                    /*    Hi - Fwd      */
    CAI_SO_PPP_PKT_DATA_13K_PRE707                    /*    Hi - Both     */
    },

/*        M51 = 1 (Highspeed)                                SCH RATE      */
    {
    CAI_SO_PPP_PKT_DATA_13K_PRE707,                    /*    Standard      */
    CAI_SO_MDR_PKT_DATA_FRS2_RRS2,                     /*    Hi - Fwd      */
    CAI_SO_MDR_PKT_DATA_FRS2_RRS2                     /*    Hi - both     */
    }
  },
/*-------------------------------------------------------------------------*/
/* PREV = 6                                                                */
/*-------------------------------------------------------------------------*/
/*        M51 = 0 (Lowspeed)                                 SCH RATE      */
  {
    {
    CAI_SO_PPP_PKT_DATA_13K_PRE707,                    /*    Standard      */
    CAI_SO_PPP_PKT_DATA_13K_PRE707,                    /*    Hi - Fwd      */
    CAI_SO_PPP_PKT_DATA_13K_PRE707                    /*    Hi - Both     */
    },

/*        M51 = 1 (Highspeed)                                SCH RATE      */
    {
    CAI_SO_PPP_PKT_DATA_13K_PRE707,                    /*    Standard      */
    CAI_SO_PPP_PKT_DATA_3G,                            /*    Hi - Fwd      */
    CAI_SO_PPP_PKT_DATA_3G                             /*    Hi - Fwd      */
    }
  }
};


/*---------------------------------------------------------------------------
  Packet SO Page Rsp SO's.  Use a set of values to indicate what the
  response should be.  #define's not used because there is no space to
  do this cleanly in the table below.
  
     0  = respond with 0
     1  = respond with same SO
     15 = respond with SO 15
     33 = respond with SO 33
---------------------------------------------------------------------------*/
LOCAL const byte ds707_so_jcdma_pkt_so_rsp_tbl[DS707_NUM_P_REVS]
                                              [SYS_JCDMA_M511_MAX]
                                              [SYS_JCDMA_M512_MAX]
                                              [DS707_JCDMA_DORM_MAX]
                                              [DS707_JCDMA_APPRATE_MAX]
                                              [DS707_SO_PKT_NUM] =
{
/*===========================================================================
    P _ R E V = 3  (Lowspeed data, RLP 1)
===========================================================================*/
/*-----M511 = 0 (PACKET)-------- M512 = 0 (Lowspeed)-----------NULL -------*/
  {
    /*  33,   25,    22,    0x8020,   15,    4103,     7           SCH RATE    */            
    {
      {
        { 
          { 0,     0,     0,       0,      1,      0,      1 },    /*    Standard    */
          { 0,     0,     0,       0,      1,      0,      1 },    /*    Hi - Fwd    */
          { 0,     0,     0,       0,      1,      0,      1 }    /*    Hi - Both   */
        },
        /*-----DORMANT------*/
        {
          { 0,     0,     0,       0,      1,      0,      1 },    /*    Standard    */
          { 0,     0,     0,       0,      1,      0,      1 },    /*    Hi - Fwd    */
          { 0,     0,     0,       0,      1,      0,      1 }    /*    Hi - Both   */
        }
      },

      {
        /*---- M512 = 1 (Highspeed)---------- NULL ------*/
        {
          { 0,     0,     0,       0,      1,      0,      1 },    /*    Standard    */
          { 0,     0,     0,       0,      1,      0,      1 },    /*    Hi - Fwd    */
          { 0,     0,     0,       0,      1,      0,      1 }    /*    Hi - Both   */
        },

        /*-----DORMANT------*/
        {
          { 0,     0,     0,       0,      1,      0,      1 },    /*    Standard    */
          { 0,     0,     0,       0,      1,      0,      1 },    /*    Hi - Fwd    */
          { 0,     0,     0,       0,      1,      0,      1 }    /*    Hi - Both   */
        }
      }
    },
    /*-----M511 = 1 (ASYNC)--------- M512 = 0 (Lowspeed)-----------NULL -------*/
    /*  33,   25,    22,    0x8020,   15,    4103,     7           SCH RATE    */            
    {
      {
        {
          { 0,     0,     0,       0,      1,      0,      1 },    /*    Standard    */
          { 0,     0,     0,       0,      1,      0,      1 },    /*    Hi - Fwd    */
          { 0,     0,     0,       0,      1,      0,      1 }    /*    Hi - Both   */
        },
        /*-----DORMANT------*/
        {
          { 0,     0,     0,       0,      1,      0,      1 },    /*    Standard    */
          { 0,     0,     0,       0,      1,      0,      1 },    /*    Hi - Fwd    */
          { 0,     0,     0,       0,      1,      0,      1 }    /*    Hi - Both   */
        }
      },

      {
        /*---- M512 = 1 (Highspeed)---------- NULL ------*/
        {
          { 0,     0,     0,       0,      1,      0,      1 },    /*    Standard    */
          { 0,     0,     0,       0,      1,      0,      1 },    /*    Hi - Fwd    */
          { 0,     0,     0,       0,      1,      0,      1 }    /*    Hi - Both   */
        },
        /*-----DORMANT------*/
        {
          { 0,     0,     0,       0,      1,      0,      1 },    /*    Standard    */
          { 0,     0,     0,       0,      1,      0,      1 },    /*    Hi - Fwd    */
          { 0,     0,     0,       0,      1,      0,      1 }    /*    Hi - Both   */
        }
      }
    }
  },
/*===========================================================================
  P _ R E V = 5  (Medium data rate, RLP 2)
===========================================================================*/
/*-----M511 = 0 (PACKET)-------- M512 = 0 (Lowspeed)-----------NULL -------*/
  {
    /*  33,   25,    22,    0x8020,   15,    4103,     7           SCH RATE    */            
    {
      {
        {
          { 0,     1,     1,       0,      1,      0,      1 },    /*    Standard    */
          { 0,     1,     1,       0,      1,      0,      1 },    /*    Hi - Fwd    */
          { 0,     1,     1,       0,      1,      0,      1 }    /*    Hi - Both   */
        },

        /*-----DORMANT------*/
        {
          { 0,     1,     1,       0,      1,      0,      1 },    /*    Standard    */
          { 0,     1,     1,       0,      1,      0,      1 },    /*    Hi - Fwd    */
          { 0,     1,     1,       0,      1,      0,      1 }    /*    Hi - Both   */
        }
      },

      {
        /*---- M512 = 1 (Highspeed)---------- NULL ------*/
        {
          { 0,     1,     1,       0,      1,      0,      1 },    /*    Standard    */
          { 0,     1,     1,       0,      1,      0,      1 },    /*    Hi - Fwd    */
          { 0,     1,     1,       0,      1,      0,      1 }    /*    Hi - Both   */
        }, 
        /*-----DORMANT------*/
        {
          { 0,     1,     1,       0,      1,      0,      1 },    /*    Standard    */
          { 0,     1,     1,       0,      1,      0,      1 },    /*    Hi - Fwd    */
          { 0,     1,     1,       0,      1,      0,      1 }    /*    Hi - Both   */
        }
      }
    },
    /*-----M511 = 1 (ASYNC)--------- M512 = 0 (Lowspeed)-----------NULL -------*/
    /*  33,   25,    22,    0x8020,   15,    4103,     7           SCH RATE    */            
    {
      {
        {
          { 0,     1,     1,       0,      1,      0,      1 },    /*    Standard    */
          { 0,     1,     1,       0,      1,      0,      1 },    /*    Hi - Fwd    */
          { 0,     1,     1,       0,      1,      0,      1 }    /*    Hi - Both   */
        },
        /*-----DORMANT------*/
        {
          { 0,     1,     1,       0,      1,      0,      1 },    /*    Standard    */
          { 0,     1,     1,       0,      1,      0,      1 },    /*    Hi - Fwd    */
          { 0,     1,     1,       0,      1,      0,      1 }    /*    Hi - Both   */
        }
      },

      {
        /*---- M512 = 1 (Highspeed)---------- NULL ------*/
        {
          { 0,     1,     1,       0,      1,      0,      1 },    /*    Standard    */
          { 0,     1,     1,       0,      1,      0,      1 },    /*    Hi - Fwd    */
          { 0,     1,     1,       0,      1,      0,      1 }    /*    Hi - Both   */
        },
        /*-----DORMANT------*/
        {
          { 0,     1,     1,       0,      1,      0,      1 },    /*    Standard    */
          { 0,     1,     1,       0,      1,      0,      1 },    /*    Hi - Fwd    */
          { 0,     1,     1,       0,      1,      0,      1 }    /*    Hi - Both   */
        }
      }
    }
  },
/*===========================================================================
  P _ R E V = 6  (1X, RLP 3)
===========================================================================*/
/*-----M511 = 0 (PACKET)-------- M512 = 0 (Lowspeed)-----------NULL -------*/
  {
    /*  33,   25,    22,    0x8020,   15,    4103,     7           SCH RATE    */            
    {
      {
        {
          { 1,     1,     1,       0,      1,      0,      1 },    /*    Standard    */
          { 1,     1,     1,       0,      1,      0,      1 },    /*    Hi - Fwd    */
          { 1,     1,     1,       0,      1,      0,      1 }    /*    Hi - Both   */
        },
        /*-----DORMANT------*/
        {
          { 15,    15,    15,      0,     15,      0,     15 },    /*    Standard    */
          { 15,    15,    15,      0,     15,      0,     15 },    /*    Hi - Fwd    */
          { 15,    15,    15,      0,     15,      0,     15 }    /*    Hi - Both   */
        }
      },

      /*---- M512 = 1 (Highspeed)---------- NULL ------*/
      {
        {
          { 1,     1,     1,       0,      1,      0,      1 },    /*    Standard    */
          { 1,     1,     1,       0,      1,      0,      1 },    /*    Hi - Fwd    */
          { 1,     1,     1,       0,      1,      0,      1 }    /*    Hi - Both   */
        },
        /*-----DORMANT------*/
        {
          { 15,    15,    15,      0,     15,      0,     15 },    /*    Standard    */
          { 33,    33,    33,      0,     33,      0,     33 },    /*    Hi - Fwd    */
          { 33,    33,    33,      0,     33,      0,     33 }    /*    Hi - Both   */
        }
      }
    },
    /*-----M511 = 1 (ASYNC)--------- M512 = 0 (Lowspeed)-----------NULL -------*/
    /*  33,   25,    22,    0x8020,   15,    4103,     7           SCH RATE    */            
    {
      {
        {
          { 1,     1,     1,       0,      1,      0,      1 },    /*    Standard    */
          { 1,     1,     1,       0,      1,      0,      1 },    /*    Hi - Fwd    */
          { 1,     1,     1,       0,      1,      0,      1 }    /*    Hi - Both   */
        },
        /*-----DORMANT------*/
        {
          { 1,     1,     1,       0,      1,      0,      1 },    /*    Standard    */
          { 1,     1,     1,       0,      1,      0,      1 },    /*    Hi - Fwd    */
          { 1,     1,     1,       0,      1,      0,      1 }    /*    Hi - Both   */
        }
      },
      /*---- M512 = 1 (Highspeed)---------- NULL ------*/
      {
        {
          { 1,     1,     1,       0,      1,      0,      1 },    /*    Standard    */
          { 1,     1,     1,       0,      1,      0,      1 },    /*    Hi - Fwd    */
          { 1,     1,     1,       0,      1,      0,      1 }    /*    Hi - Both   */
        },
        /*-----DORMANT------*/
        {
          { 1,     1,     1,       0,      1,      0,      1 },    /*    Standard    */
          { 1,     1,     1,       0,      1,      0,      1 },    /*    Hi - Fwd    */
          { 1,     1,     1,       0,      1,      0,      1 }    /*    Hi - Both   */
        }
      }
    }
  }
};


#ifdef FEATURE_DATA_IS707_ASYNC
/*===========================================================================
                       ASYNC SERVICE OPTIONS
===========================================================================*/
/*---------------------------------------------------------------------------
  Async SO's enable/disable table.
---------------------------------------------------------------------------*/
static const boolean ds707_so_jcdma_async_tbl[DS707_SO_ASYNC_NUM] =
{
#ifndef FEATURE_DATA_DISABLE_CS_SO
/*   4      4100      12      0x8021                                       */
   TRUE,    TRUE,    TRUE,     TRUE
#else
/*   4      4100      12      0x8021                                       */
   FALSE,   FALSE,   FALSE,   FALSE
#endif /* FEATURE_DATA_DISABLE_CS_SO */
};

/*---------------------------------------------------------------------------
  Async SO to originate with.  Always assume Rateset 2
---------------------------------------------------------------------------*/
static const ds707_so_type ds707_so_jcdma_async_orig_so = CAI_SO_ASYNC_DATA_13K_PRE707;

/*---------------------------------------------------------------------------
  Async SO's to respond to pages with.
---------------------------------------------------------------------------*/
static const byte ds707_so_jcdma_async_so_rsp_tbl[DS707_SO_ASYNC_NUM] = 
{
/*   4      4100      12      0x8021                                       */
     4,       0,      12,        0
};


/*===========================================================================
                          G3 FAX SERVICE OPTIONS
===========================================================================*/
/*---------------------------------------------------------------------------
  FAX SO's enable/disable table.
---------------------------------------------------------------------------*/
static const boolean ds707_so_jcdma_g3fax_tbl[DS707_SO_G3FAX_NUM] =
{                            
#ifndef FEATURE_DATA_DISABLE_CS_SO
/*   5      4101     13      0x8022                                        */
   TRUE,    TRUE,   TRUE,     TRUE
#else
/*   5      4101     13      0x8022                                        */
   FALSE,   FALSE,  FALSE,   FALSE
#endif /* FEATURE_DATA_DISABLE_CS_SO */
};

/*---------------------------------------------------------------------------
  FAX SO to originate with.
---------------------------------------------------------------------------*/
static const ds707_so_type ds707_so_jcdma_g3fax_orig_so = CAI_SO_G3_FAX_13K_PRE707;

/*---------------------------------------------------------------------------
  FAX SO's to respond to pages with.
---------------------------------------------------------------------------*/
static const byte ds707_so_jcdma_g3fax_so_rsp_tbl[DS707_SO_G3FAX_NUM] = 
{                            
/*   5      4101     13      0x8022                                        */
     5,       0,     13,       0
};
#endif /* FEATURE_DATA_IS707_ASYNC */


/*===========================================================================
                      INTERNAL FUNCTION DEFINITIONS
===========================================================================*/
/*===========================================================================
FUNCTION      DS707_SO_JCDMA_PKTI_CBACK

DESCRIPTION   Callback for 707 pkt phys link events.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None
===========================================================================*/
void ds707_so_jcdma_pkti_cback
(
  ds707_cback_event_type              event,
  const ds707_cback_event_info_type   *event_info_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DATA_IS707_MSG2( MSG_LEGACY_HIGH, "PHY LINK change ev %d, nest level %d, "
                   "recal pkt Serv Opts",
                   event,
                   ds707_cback_get_nest_level() );

  switch(event)
  {
    case DS707_CBACK_PHYS_LINK_DOWN_EV:
    case DS707_CBACK_PHYS_LINK_UP_EV:
    case DS707_CBACK_ABORT_SESSION_EV:

      ds707_so_pkt_recal();
      break;

    default:

      ASSERT(0);
      break;
  }
}

/*===========================================================================
                      EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/
/*===========================================================================
FUNCTION      DS707_SO_JCDMA_PKT_SET_PAGE_RSP

DESCRIPTION   JCDMA Pkt SO's

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_so_jcdma_pkt_set_page_rsp(void)
{
  ds707_so_type  pkt_so;                           /* pkt so from table    */ 
  ds707_so_type  page_rsp_so;                      /* page response so     */
  byte           i;                                /* for...loop invariant */
  word           p_rev_index;                      /* system p_rev         */
  ds707_jcdma_dorm_e_type  dorm_state;

  ps_phys_link_type  *phys_link_ptr;
  sys_jcdma_m511_e_type     jcdma_m511_val;
  sys_jcdma_m512_e_type     jcdma_m512_val;
  ds707_jcdma_apprate_type  jcdma_apprate_val;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  jcdma_m511_val = (sys_jcdma_m511_e_type)
                              ds707_jcdma_m51_get_val(JCDMA_VAL_511);

  jcdma_m512_val = (sys_jcdma_m512_e_type)
                              ds707_jcdma_m51_get_val(JCDMA_VAL_512);

  jcdma_apprate_val = ds707_jcdma_apprate_get_rate();

  p_rev_index = ds707_p_rev_get_index();

  phys_link_ptr = ds707_pkt_get_pri_phys_link_ptr();

  if ((phys_link_ptr != NULL) &&
      (ds707_pkt_is_dormant(phys_link_ptr) == TRUE)

     )
  {
    dorm_state = DS707_JCDMA_DORM_IS_DORM;
  }
  else
  {
    dorm_state = DS707_JCDMA_DORM_IS_NOT_DORM;
  }

  /*lint -save -e641 converting enum's varibale to int 641*/
  if ((jcdma_m511_val < 0) || (jcdma_m512_val < 0) ||
      (dorm_state < 0) || (jcdma_apprate_val < 0)) 
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "invalid jcdma parameters");
    return;
  }
  /*lint -restore 641*/
  /*-------------------------------------------------------------------------
    Setup the page response service option for each packet data service
    option in the pkt_so_list[], based on system values.
  -------------------------------------------------------------------------*/
  for( i = 0; i < DS707_SO_PKT_NUM; i++ )
  {
    pkt_so = ds707_get_so_pkt_type(i);

    switch(ds707_so_jcdma_pkt_so_rsp_tbl[p_rev_index]
                                        [jcdma_m511_val]
                                        [jcdma_m512_val]
                                        [dorm_state]
                                        [jcdma_apprate_val]
                                        [i]
          )
    {
      case 0:
        page_rsp_so = 0;
        break;

      case 1:
        page_rsp_so = pkt_so;
        break;

      case 15:
        page_rsp_so = 15;
        break;

      case 33:
        page_rsp_so = 33;
        break;

      default:	  	
        ASSERT(0);
        page_rsp_so = pkt_so;
        break;
    } /* switch */
   /*lint -save -e641 Suppression this as this is not data error 641*/
    if (snm_set_page_response_so((word)pkt_so, (word)page_rsp_so)==0)
    {
      DATA_IS707_MSG1( MSG_LEGACY_ERROR, 
                       "SO %d not found in SNP", 
                       page_rsp_so );
    }
	/*lint -restore*/

  }  /* for i = 0 to DS707_SO_PKT_NUM */
} /* ds707_so_jcdma_pkt_set_page_rsp() */

/*===========================================================================
FUNCTION      DS707_SO_JCDMA_PKT_SET__STATE

DESCRIPTION   

DEPENDENCIES  

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_so_jcdma_pkt_set_state(void)
{
  ds707_so_type  pkt_so;                 /* service option from table      */
  byte           i;                      /* loop invariant                 */
  word           p_rev_index;            /* system p_rev                   */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  p_rev_index = ds707_p_rev_get_index();

  /*-------------------------------------------------------------------------
    Enable or disable each packet data service option, based on the
    appropriate settings.
  -------------------------------------------------------------------------*/
  for( i = 0; i < DS707_SO_PKT_NUM; i++ )
  {
    pkt_so = ds707_get_so_pkt_type(i);

    if (ds707_so_jcdma_pkt_tbl[p_rev_index][i] == TRUE)
    {
      /*---------------------------------------------------------------------
        Invoke the SNM function to enable this service option.
      ---------------------------------------------------------------------*/
	  /*lint -save -e641 Suppress non data code*/
      if (snm_enable_so((word)pkt_so)!=0)
      {
        continue;
      }
	  /*lint -restore*/
    }
    else
    {
      /*---------------------------------------------------------------------
        Invoke the SNM function to disable this service option.
      ---------------------------------------------------------------------*/
	 /*lint -save -e641 Suppress non data code*/
      if(snm_disable_so((word)pkt_so)!= SNM_FAILURE)
     {
        continue;
     }
	 /*lint -restore*/
    }
    DATA_IS707_MSG1(MSG_LEGACY_ERROR, "SO %d not found in SNP", pkt_so);
  } /* for i = 0 to DS707_SO_PKT_NUM */
} /* ds707_so_jcdma_pkt_set_state() */

/*===========================================================================
FUNCTION      DS707_SO_JCDMA_PKT_GET_ORIG

DESCRIPTION   Returns the packet service option to originate with.

DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
ds707_so_type ds707_so_jcdma_pkt_get_orig
(
  ds707_rateset_type         rateset
)
{
  word  p_rev_index;                     /* system p_rev                   */
  sys_jcdma_m512_e_type     jcdma_m512_val;
  ds707_jcdma_apprate_type  jcdma_apprate_val;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  jcdma_m512_val = (sys_jcdma_m512_e_type)
                              ds707_jcdma_m51_get_val(JCDMA_VAL_512);

  jcdma_apprate_val = ds707_jcdma_apprate_get_rate();
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*Lint error handler*/
  DATA_MDM_ARG_NOT_USED(rateset);

  /*lint -save -e641 enum to int suppression*/
  if ((jcdma_m512_val < 0) || (jcdma_apprate_val < 0))
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "invalid jcdma parameters");
    return DS707_INVALID_SO;
  }
  /*lint -restore*/
  p_rev_index = ds707_p_rev_get_index();

  return(ds707_so_jcdma_pkt_orig_tbl[p_rev_index]
                                    [jcdma_m512_val]
                                    [jcdma_apprate_val]);
} /* ds707_so_jcdma_pkt_get_orig() */

/*===========================================================================
FUNCTION      DS707_SO_JCDMA_VERIFY_INC_PKT_SO

DESCRIPTION   Given an SO, returns TRUE or FALSE, indicating if the SO
              should be accepted or rejected.  

DEPENDENCIES  None.

RETURN VALUE  TRUE - accept SO.   FALSE - Reject SO.  Does not indicate if
              ORDQ should indicate NULL or dormant.

SIDE EFFECTS  None
===========================================================================*/
boolean ds707_so_jcdma_verify_inc_pkt_so
(
  ds707_so_type     inc_so
)
{
  byte                     i;                      /* loop invariant       */
  word                     p_rev_index;            /* system p_rev         */
  ds707_jcdma_dorm_e_type  dorm_state;
  sys_jcdma_m511_e_type     jcdma_m511_val;
  sys_jcdma_m512_e_type     jcdma_m512_val;
  ds707_jcdma_apprate_type  jcdma_apprate_val;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  jcdma_m511_val = (sys_jcdma_m511_e_type)
                              ds707_jcdma_m51_get_val(JCDMA_VAL_511);

  jcdma_m512_val = (sys_jcdma_m512_e_type)
                              ds707_jcdma_m51_get_val(JCDMA_VAL_512);

  jcdma_apprate_val = ds707_jcdma_apprate_get_rate();
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    If M511 indicates Async/FAX, then can't accept incoming PKT SO.
  -------------------------------------------------------------------------*/
  if (ds707_jcdma_m51_is_pkt_allowed() == FALSE)
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH, "M51 indicates pkt disabled");
    return(FALSE);
  }

  /*-------------------------------------------------------------------------
    Find the index of the incoming service option.
  -------------------------------------------------------------------------*/
  for (i = 0; i < DS707_SO_PKT_NUM; i++)
  {
    if (ds707_get_so_pkt_type(i) == inc_so)
    {
      break;
    }
  }

  if (i == DS707_SO_PKT_NUM)
  {
    DATA_IS707_MSG1(MSG_LEGACY_ERROR, "Unknown pkt so %d", inc_so);
    return(FALSE);
  }

  /*-------------------------------------------------------------------------
    Get various parameters.  If SO is disabled in SNM or page response was
    zero, then must reject incoming call.
  -------------------------------------------------------------------------*/
  p_rev_index = ds707_p_rev_get_index();

  if (ds707_pkt_is_dormant(ds707_pkt_get_pri_phys_link_ptr()) ==
                                      TRUE)
  {
    dorm_state = DS707_JCDMA_DORM_IS_DORM;
  }
  else
  {
    dorm_state = DS707_JCDMA_DORM_IS_NOT_DORM;
  }
  /*lint -save -e641 Suppress enum to int conv*/
  if ((jcdma_m511_val < 0) || (jcdma_m512_val < 0) ||
      (dorm_state < 0 ) || (jcdma_apprate_val < 0))
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "invalid jcdma parameters");
    return(FALSE);
  } 
  /*lint -restore 641*/
  if ((ds707_so_jcdma_pkt_tbl[p_rev_index][i] == FALSE) ||
      (ds707_so_jcdma_pkt_so_rsp_tbl[p_rev_index]
                                    [jcdma_m511_val]
                                    [jcdma_m512_val]
                                    [dorm_state]
                                    [jcdma_apprate_val]
                                    [i] == 0)
     )
  {
    DATA_IS707_MSG1(MSG_LEGACY_HIGH, "cant accept incoming so  %d",inc_so);
    return(FALSE);
  }
    
  
  return(TRUE);
} /* ds707_so_jcdma_verify_inc_pkt_so() */


#ifdef FEATURE_DATA_IS707_ASYNC
/*===========================================================================
FUNCTION      DS707_SO_JCDMA_ASYNC_SET_PAGE_RSP

DESCRIPTION   JCDMA Pkt SO's

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_so_jcdma_async_set_page_rsp(void)
{
  byte                     i;
  ds707_so_type            async_so;
  ds707_so_type            page_rsp_so;            /* page response so     */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  for( i = 0; i < DS707_SO_ASYNC_NUM; i++ )
  {
    async_so    = ds707_get_so_async_type(i);
    page_rsp_so = ds707_so_jcdma_async_so_rsp_tbl[i];
	/*lint -save -e641 Suppress non data code*/
    if (snm_set_page_response_so((word)async_so, (word)page_rsp_so)== 0)
    {
      DATA_IS707_MSG0(MSG_LEGACY_ERROR, "SO not found in SNP");
    }
	/*lint -restore 641*/
  }

} /* ds707_so_jcdma_async_set_page_rsp() */

/*===========================================================================
FUNCTION      DS707_SO_JCDMA_ASYNC_SET_STATE

DESCRIPTION   Lookup the state of each async data service option based on the
              current service option set in use. Enable or disable each async
              data service option as per the table.

DEPENDENCIES  Uses the global variable ds_atcop_qcso_val to determine the
              current setting of the service option set.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_so_jcdma_async_set_state(void)
{
  ds707_so_type  async_so;
  byte           i;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for( i = 0; i < DS707_SO_ASYNC_NUM; i++ )
  {
    async_so = ds707_get_so_async_type(i);

    if (ds707_so_jcdma_async_tbl[i] == TRUE)
    {
      /*---------------------------------------------------------------------
        Invoke the SNM function to enable this service option.
      ---------------------------------------------------------------------*/
	  /*lint -save -e641 Suppress non data code*/
      if (snm_enable_so((word)async_so)!=0)
      {
        continue;
      }
	  /*lint -restore 641*/
    }
    else
    {
      /*---------------------------------------------------------------------
        Invoke the SNM function to disable this service option.
      ---------------------------------------------------------------------*/
	  /*lint -save -e641 Suppress non data code*/
      if (snm_disable_so((word)async_so)!= SNM_FAILURE)
      {
        continue;
      }
	  /*lint -restore*/
    }
    DATA_IS707_MSG1(MSG_LEGACY_ERROR, "SO %d not found in SNP", async_so);
  } /* for i = 0 to DS707_SO_ASYNC_NUM */
} /* ds707_so_jcdma_async_set_state() */

/*===========================================================================
FUNCTION      DS707_SO_JCDMA_ASYNC_GET_ORIG

DESCRIPTION   Returns the async service option to originate with.

DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
ds707_so_type ds707_so_jcdma_async_get_orig
(
  ds707_rateset_type         rateset
)
{
  /*Lint error handler*/
  DATA_MDM_ARG_NOT_USED(rateset);
  return(ds707_so_jcdma_async_orig_so);
} /* ds707_so_jcdma_async_get_orig() */


/*===========================================================================
FUNCTION      DS707_SO_JCDMA_G3FAX_SET_PAGE_RSP

DESCRIPTION   JCDMA FAX SO's

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_so_jcdma_g3fax_set_page_rsp(void)
{
  byte                     i;
  ds707_so_type            g3fax_so;
  ds707_so_type            page_rsp_so;            /* page response so     */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  for( i = 0; i < DS707_SO_G3FAX_NUM; i++ )
  {
    g3fax_so    = ds707_get_so_g3fax_type(i);
    page_rsp_so = ds707_so_jcdma_g3fax_so_rsp_tbl[i];
	/*lint -save -e641 Suppress non data code*/
    if (snm_set_page_response_so((word)g3fax_so, (word)page_rsp_so)==0)
    {
      DATA_IS707_MSG0(MSG_LEGACY_ERROR, "SO not found in SNP");
    }
	/*lint -restore*/
  }

} /* ds707_so_jcdma_g3fax_set_page_rsp() */

/*===========================================================================
FUNCTION      DS707_SO_JCDMA_G3FAX_SET_STATE

DESCRIPTION   Lookup the state of each g3 fax service option based on the
              current Service Option Set in use. Enable or disable each G3
              fax service option as per the table.

DEPENDENCIES  Uses the global variable ds_atcop_qcso_val to determine the
              current setting of the service option set.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_so_jcdma_g3fax_set_state(void)
{
  ds707_so_type  fax_so;
  byte           i;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  for( i = 0; i < DS707_SO_G3FAX_NUM; i++ )
  {
    fax_so = ds707_get_so_g3fax_type(i);
    if (ds707_so_jcdma_g3fax_tbl[i] == TRUE)
    {
      /*---------------------------------------------------------------------
        Invoke the SNM function to enable this service option.
      ---------------------------------------------------------------------*/
	  /*lint -save -e641 Suppress non data code*/
      if (snm_enable_so((word)fax_so)!=0)
      {
        continue;
      }
	  /*lint -restore*/
    }
    else
    {
      /*---------------------------------------------------------------------
        Invoke the SNM function to disable this service option.
      ---------------------------------------------------------------------*/
	  /*lint -save -e641 Suppress non data code*/
      if (snm_disable_so((word)fax_so)!= SNM_FAILURE)
      {
        continue;
      }
	  /*lint -restore 641*/
    }
    DATA_IS707_MSG1(MSG_LEGACY_ERROR, "SO %d not found in SNP", fax_so);
  } /* for i = 0 to DS707_SO_JCDMA_G3FAX_NUM */
} /* ds707_so_jcdma_g3fax_set_state() */


/*===========================================================================
FUNCTION      DS707_SO_JCDMA_G3FAX_GET_ORIG

DESCRIPTION   Returns the G3/FAX service option to originate with.

DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  None
==========================================================================*/
ds707_so_type ds707_so_jcdma_g3fax_get_orig
(
  ds707_rateset_type         rateset
)
{
  /*Lint error handler*/
  DATA_MDM_ARG_NOT_USED(rateset);
  return(ds707_so_jcdma_g3fax_orig_so);
} /* ds707_so_jcdma_g3fax_get_orig() */


/*===========================================================================
FUNCTION      DS707_SO_JCDMA_VERIFY_INC_ASYNC_FAX_SO

DESCRIPTION   Given an SO, returns TRUE or FALSE, indicating if the SO
              should be accepted or rejected.    Works for either ASYNC
              or FAX.

DEPENDENCIES  None.

RETURN VALUE  TRUE - accept SO.   FALSE - Reject SO

SIDE EFFECTS  None
===========================================================================*/
boolean ds707_so_jcdma_verify_inc_async_fax_so
(
  ds707_so_type     inc_so
)
{
  byte           i;
  boolean        so_found = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Must be in ASYNC/FAX mode to accept incoming call.
  -------------------------------------------------------------------------*/
  if (ds707_jcdma_m51_is_async_fax_allowed() == FALSE)
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH, "M51 indicates async_fax disabled");
    return(FALSE);
  }

  /*-------------------------------------------------------------------------
    Do ASYNC first.  If page response for this so was 0, then must reject.
  -------------------------------------------------------------------------*/
  for( i = 0; i < DS707_SO_ASYNC_NUM; i++ )
  {
    if (ds707_get_so_async_type(i) == inc_so)
    {
      so_found = TRUE;
      break;
    }
  }
  /*lint -e661 Warning suppressed because the variable 'i' will 
  never go out of bounds*/
  if (so_found == TRUE)
  {
    if (ds707_so_jcdma_async_so_rsp_tbl[i] == 0)
    {
      DATA_IS707_MSG1(MSG_LEGACY_HIGH, "must reject inc so %d", inc_so);
      return(FALSE);
    }
    else
    {
      return(TRUE);
    }
  }
  /*lint +e661 */
  /*-------------------------------------------------------------------------
    If here then it is not an ASYNC SO. Check to see if it is a FAX SO.
  -------------------------------------------------------------------------*/
  for( i = 0; i < DS707_SO_G3FAX_NUM; i++ )
  {
    if (ds707_get_so_g3fax_type(i) == inc_so)
    {
      so_found = TRUE;
      break;
    }
  }
  /*lint -e661 Warning suppressed because the variable 'i' will 
  never go out of bounds*/
  if (so_found == TRUE)
  {
    if (ds707_so_jcdma_g3fax_so_rsp_tbl[i] == 0)
    {
      DATA_IS707_MSG1(MSG_LEGACY_HIGH, "must reject inc so %d", inc_so);
      return(FALSE);
    }
    else
    {
      return(TRUE);
    }
  }
  /*lint +e661 */
  else
  {
    DATA_IS707_MSG1(MSG_LEGACY_ERROR, "Unknown incoming SO %d", inc_so);
    return(FALSE);
  }
} /* ds707_so_jcdma_verify_inc_async_fax_so() */
#endif /* FEATURE_DATA_IS707_ASYNC */

/*===========================================================================
FUNCTION      DS707_SO_JCDMA_INIT

DESCRIPTION   Initialie jcdma module.  Register for 707 pkt events.

DEPENDENCIES  None.

RETURN VALUE  TRUE - accept SO.   FALSE - Reject SO

SIDE EFFECTS  None
===========================================================================*/
void ds707_so_jcdma_init(void)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(ds707_roaming_is_curr_mode_jcdma() == FALSE )
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR,"JCDMA NV not enabled");
    return;
  }
  
  if (ds707_cback_register(
                        DS707_CBACK_PHYS_LINK_DOWN_EV,
                        ds707_so_jcdma_pkti_cback
                          )==0)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "callback could not be inserted");
  }

  if (ds707_cback_register(
                        DS707_CBACK_PHYS_LINK_UP_EV,
                        ds707_so_jcdma_pkti_cback
                          )== 0)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "callback could not be inserted");
  }

  if (ds707_cback_register(
                        DS707_CBACK_ABORT_SESSION_EV,
                        ds707_so_jcdma_pkti_cback
                          )== 0)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "callback could not be inserted");
  }

  return;
} /* ds707_so_jcdma_init() */

