#ifndef UL_STATES_H
#define UL_STATES_H


/*===========================================================================
                           U L _ S T A T E S . H

DESCRIPTION
This file contains the prototypes for all of the state processing routines
for the uplink controller.


Copyright (c) 2000-2004 by Qualcomm Technologies, Inc.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/l1/vcs/ulstates.h_v   1.8   29 May 2002 11:40:34   msatyana  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/src/ulstates.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     --------------------------------------------------------
08/04/15    ar      Ensure that HSRACH cleanup routine allows connected state cleanup to 
                    continue in case HSRACH is not configured.
07/08/13    vr      Enhanced API to check if UL is active, to accomodate for HSRACH state machine.
06/25/13    yw      Code cleanup for IDLE REQ, EUL/EDL MODULE_CLEANUP
08/09/12    hk      Streamlined FACH and DCH module cleanup procedures
02/01/12   gnk      Nikel feature cleanup
05/08/09   sup      Changing the includes for the renaming of header files  dec.h, enci.h, l1m.h, srch.h, srchi.h, 
                    srchlog.h to wdec.h, wenci.h, wl1m_v.h, wsrch.h, wsrchi.h, wsrchlog.h
03/02/07    am      Added support for log on demand for 4303 logpkt
08/14/06    yiz     Mainline HSUPA code
10/22/04    yiz     Add ul_dch_sync_a_pending flag.
05/29/02    m       Added prototypes for LOD functions
01/14/01    sh      Added flag to indicate a module cleanup is pending.
01/18/2001  mdl     get next L1M state as input parameter to cleanup routines
09/22/00    sk      file created.
===========================================================================*/

#include "wcdma_variation.h"
#include "wl1m.h"
#include "eulcfg.h"

/* UL states */
typedef enum {
  /* un-initialized */
  UL_UNINIT,
  /* initialized */
  UL_INIT,
  /* PRACH active */
  UL_PRACH_TX,
  /* UL DPCH active */
  UL_DPCH_TX,
  /* un-recoverable error */
  UL_ERROR
} ul_state_enum_type;

/* Initial UL state is Un-Initialized */
extern ul_state_enum_type ul_state;

/* Flag to indicate a module cleanup is pending */
extern boolean ul_module_cleanup_flg;

/* Flag to indicate that UL sync procedure A is required
   when UE enters DCH state from other states
   when UE goes from DCH to DCH via HHO
   when UE resumes DCH fater inter-RAT HHO or HHO failure */
extern boolean ul_dch_sync_a_pending;

/*===========================================================================
FUNCTION        UL_INIT

FILE NAME       

DESCRIPTION     This is the one time initialization function for the uplink
                controller.  It just calls the associated uplink control and 
                encoder driver initializers.

DEPENDENCIES    

RETURN VALUE    None

SIDE EFFECTS    
===========================================================================*/
void ul_init(void);


/*===========================================================================
FUNCTION        UL_CLEANUP

FILE NAME       

DESCRIPTION     This is the one time cleanup routine for the uplink 
                controller. It calls the associated uplink control and
                encoder driver cleanup routines.
                
DEPENDENCIES    

RETURN VALUE    None

SIDE EFFECTS    
===========================================================================*/
boolean ul_cleanup(l1m_state_enum_type next_state);


/*===========================================================================
FUNCTION        UL_IDLE_INIT

FILE NAME       

DESCRIPTION     This is the IDLE state initializer for the uplink controller.

DEPENDENCIES    

RETURN VALUE    

SIDE EFFECTS    
===========================================================================*/
void ul_idle_init(void);


/*===========================================================================
FUNCTION        UL_IDLE_CLEANUP

FILE NAME       

DESCRIPTION     This is the IDLE state cleanup routine for the uplink 
                controller.

DEPENDENCIES    

RETURN VALUE    

SIDE EFFECTS    
===========================================================================*/
boolean ul_idle_cleanup(l1m_state_enum_type next_state);


/*===========================================================================
FUNCTION        UL_ACQ_INIT

FILE NAME       

DESCRIPTION     This is the ACQ state initializer for the uplink controller.

DEPENDENCIES    

RETURN VALUE    

SIDE EFFECTS    
===========================================================================*/
void ul_acq_init(void);


/*===========================================================================
FUNCTION        UL_ACQ_CLEANUP

FILE NAME       

DESCRIPTION     This is the ACQ state cleanup routine for the uplink 
                controller.

DEPENDENCIES    

RETURN VALUE    

SIDE EFFECTS    
===========================================================================*/
boolean ul_acq_cleanup(l1m_state_enum_type next_state);


/*===========================================================================
FUNCTION        UL_BCH_INIT

FILE NAME       

DESCRIPTION     This is the BCH state initializer for the uplink controller.

DEPENDENCIES    

RETURN VALUE    

SIDE EFFECTS    
===========================================================================*/
void ul_bch_init(void);

/*===========================================================================
FUNCTION        UL_BCH_CLEANUP

FILE NAME       

DESCRIPTION     This is the BCH state cleanup routine for the uplink 
                controller.

DEPENDENCIES    

RETURN VALUE    

SIDE EFFECTS    
===========================================================================*/
boolean ul_bch_cleanup(l1m_state_enum_type next_state);


/*===========================================================================
FUNCTION        UL_PCH_INIT

FILE NAME       

DESCRIPTION     This is the PCH state initializer for the uplink controller.

DEPENDENCIES    

RETURN VALUE    

SIDE EFFECTS    
===========================================================================*/
void ul_pch_init(void);

/*===========================================================================
FUNCTION        UL_PCH_CLEANUP

FILE NAME       

DESCRIPTION     This is the PCH state cleanup routine for the uplink 
                controller.

DEPENDENCIES    

RETURN VALUE    

SIDE EFFECTS    
===========================================================================*/
boolean ul_pch_cleanup(l1m_state_enum_type next_state);

/*===========================================================================
FUNCTION        UL_FACH_INIT

FILE NAME       

DESCRIPTION     This is the FACH state initializer for the uplink controller.

DEPENDENCIES    

RETURN VALUE    

SIDE EFFECTS    
===========================================================================*/
void ul_fach_init(void);

/*===========================================================================
FUNCTION        UL_FACH_CLEANUP

FILE NAME       

DESCRIPTION     This is the FACH state cleanup routine for the uplink 
                controller.

DEPENDENCIES    

RETURN VALUE    

SIDE EFFECTS    
===========================================================================*/
boolean ul_fach_cleanup(l1m_state_enum_type next_state);

/*===========================================================================
FUNCTION        WL1_UL_HSRACH_CLEANUP

FILE NAME       

DESCRIPTION     This is the HSRAC cleanup routing called during 
                connected state cleanup

DEPENDENCIES    

RETURN VALUE    TRUE : HS RACH is not configured, continue with connected state cleanup
                FALSE : HS RACH is configured, process the cleanup action

SIDE EFFECTS    Will cause any ongoing HSRACH procedure to be aborted
===========================================================================*/
boolean wl1_ul_hsrach_cleanup(void);

/*===========================================================================
FUNCTION        UL_DCH_INIT

FILE NAME       

DESCRIPTION     This is the DCH state initializer for the uplink controller.

DEPENDENCIES    

RETURN VALUE    

SIDE EFFECTS    
===========================================================================*/
void ul_dch_init(void);

/*===========================================================================
FUNCTION        UL_DCH_CLEANUP

FILE NAME       

DESCRIPTION     This is the DCH state cleanup routine for the uplink 
                controller.

DEPENDENCIES    

RETURN VALUE    

SIDE EFFECTS    
===========================================================================*/
boolean ul_dch_cleanup(l1m_state_enum_type next_state);

/*===========================================================================
FUNCTION        UL_CONN_STATE_CLEANUP

FILE NAME

DESCRIPTION     This is the connected (FACH/DCH) state cleanup routine for Uplink
                module.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
===========================================================================*/
extern boolean ul_conn_state_cleanup(l1m_state_enum_type next_state);

 /*===========================================================================
FUNCTION        E_UL_DCH_CLEANUP

FILE NAME       

DESCRIPTION     
  This is the DCH state cleanup routine for the EUL uplink module. if eul state
  is E_UL_EDPCH_TX ,enable the module clean up flags and the edpch cfg event.
      
DEPENDENCIES    

RETURN VALUE    

SIDE EFFECTS    
===========================================================================*/
extern boolean e_ul_dch_cleanup(void);

/*=========================================================================

FUNCTION UL_PHYCHAN_IS_UL_ACTIVE_IN_FACH

DESCRIPTION
  This API returns the state of UL PHY channel, whether it is active or not. 
  - In legacy RACH case UL_STATE will be set to PRACH_TX. 
  - In HSRACH case UL_STATE will be set to PRACH_TX when channels are added 
    (and when resuming HSPRACH). It remains in this state until FDPCH 
    interrupt is received. After FDPCH interrupt is received UL_STATE is 
    moved to DPCH_TX.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE  == DPCH active when HSRACH is configured or PRACH active in FACH.
  FALSE == PRACH or DPCH not active

SIDE EFFECTS
  None.

=========================================================================*/
extern boolean ul_phychan_is_ul_active_in_fach (void);

#ifdef FEATURE_L1_LOG_ON_DEMAND
/*===========================================================================
FUNCTION        UL_LOG_INIT

FILE NAME

DESCRIPTION
  This function registers all the UL log on demand handlers to
  corresponding log code given out by QXDM

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
===========================================================================*/
void ul_log_init (void);

/*===========================================================================
FUNCTION     EUL_LOG_INIT

DESCRIPTION  This function registers all the EUL log on demand handlers to
             corresponding log code given out by QXDM

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
===========================================================================*/
void eul_log_init (void);
#endif

#endif
