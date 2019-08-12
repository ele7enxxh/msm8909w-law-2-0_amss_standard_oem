#ifndef HDRMDSP_H
#define HDRMDSP_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   H D R       M D S P     D R I V E R S

GENERAL DESCRIPTION

  This file contains the HDR modem DSP functions which are to be
  used for code download, DSP reset, MDSP command interrupt processing.


REFERENCES
  QDSP2 Firmware Interface Specification: 80-24597-1
  MSM3100 Device Specification: 93-V0104-1

EXTERNALIZED FUNCTIONS

  hdrmdsp_download
    Downloads MDSP firmware

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright(c) 2000 - 2013 by Qualcomm Technologies, Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/drivers/api/hdrmdsp.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------
04/28/14   wsh     Added NV control to pass MMSE config to FW
09/03/13   wsh     Replaced FEATURE_DIME/TRITON_BOLT_MODEM with HDR version
04/22/13   arm     Added changes for DSDA coexistence manager. 
03/18/13   kss     Remove unused legacy code.
01/25/13   vke     Added prototype for hdrmdsp_msgr_send()
10/19/12   smd     Told fw whether the standby to active transition is for wakeup.
03/19/12   ljl     Fixed Dime compiler errors.
06/17/11   smd     Added a flag for HDR RTC being started or not.  
09/27/10   lyl     Supported FW power management with suspend/resume.
04/06/10   ljl     Added hdrmdsp_fws_enable_app().
03/11/10   ljl     Supported the fw timing state.
12/11/09   grl     Added FW State Config Support
11/20/09   kss     Revert last check-in.
11/16/09   kss     Restore removed types temporarily.
11/11/09   ljl/lyl Renamed hdrmdsp_process_enable_rsp() to 
                   hdrmdsp_process_hdr_app_cfg_rsp()
09/30/09   kss     Removed obsolete types.
08/12/09   wsh     Reverted CMI related change due to restructuring
07/29/09   etv     Included hdrfw_msg.h instead of hdrmsg.h
07/15/09   etv     Moved sw module definitions to hdrmsg.h
06/12/09   ljl     Updated for firmware MSGR interface.
05/22/09   etv     Removed obsolete ISRs.
05/07/09   etv     Obsoleted hdrmdsp_disable_hdr_app_and_dec.
04/16/09   rkc     Mainlined T_MSM6500.
04/14/09   wsh     Mainlined T_MSM6800 to solve compile issue
03/23/09   wsh     CMI: featurized CMI under FEATURE_CMI
11/10/08   Vish    Added support for wideband saturation detection interrupt.
06/12/08   rmg     Added support to write Tx carriers ordered by ranks.
12/05/07   etv     Added support for mDSP Rx and Tx ISR.
10/20/07   rmg     Added Tx log buffer definition.
10/29/07   ljl     Updated for firmware release 0016.
09/27/07   ljl     Added hdrmdsp_handoff_c.
09/21/07   etv     Removed hdrmdps_rl_packet_order.
07/18/07   rmg     Added TxAGC control structure.
04/18/07   rmg     Added MAC channel info structure.
04/17/07   rmg     Added HDR_MDSP_LOG_ISR interrupt to enum.
04/13/07   kss     Added antenna structures.
04/09/07   rmg     RevB macro changes - MAC
04/02/07   ljl     Added RevB equalizer firmware register definitions.
03/31/07   rmg     Added RevB encoder firmware register definitions.
03/19/07   ljl     Added RevB finger firmware register definitions.
09/07/06   ljl     Added hdrmdsp_disable_hdr_app_and_dec().
04/18/06   ds      Replace hdr mdsp logging with mdsp services logging 
07/05/05   ds      Added support for generic MDSP debug logging
11/03/03   remi    Removed obsolete MDSP logging interface.
10/29/03   aaj     Use HDRMDSP API for enabling/disabling hdr application
07/22/03   sq      Changed DSP interrupt ENUM definitions for 6500.
02/14/03   kss     Added HDR_MDSP_SCC_LOST_ISR interrupt to enum.
03/25/02   aaj     Add dsp_clk_on flag to indicate status of dsp clocks
02/13/02   kss     Added prototypes for hdrmdsp_enable_clocks() and 
                   hdrmdsp_disable_clocks().
09/13/01   aaj     Added support for BestASPChange Interrupt
04/18/01   aaj     Log table support
04/09/01   aaj     Created this file
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "hdr_variation.h"
#include "hdrfw_msg.h"

/* <EJECT> */
/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains definitions for constants, macros, types, variables
  and other items needed by this module.

===========================================================================*/

/* these are byte addresses.            hdr.cfg has word addresses */
#define   RAMA_OFFSET  0x00040000
#define   RAMB_OFFSET  0x00050000
#define   RAMC_OFFSET  0x0005c000


/* ISRs for sharing MDSP to ARM interrupt for different originations from the
   DSP firmware */

/* MSM6500 has removed some of the MDSP interrupts which were supported 
   in MSM5500 */
typedef enum
{
  HDR_MDSP_BASP_ISR      = 6,   /* Best ASP Change ISR */
  HDR_MDSP_TIMER_ISR     = 7,   /* General Purpose Slot timer counters */
  /* HDR_MDSP_SCC_LOST_ISR  = 8,   SyncCC preamble missed. Obsolete */
  /* HDR_MDSP_DEC_ISR       = 9,   Decoder interrupt Obsolete */
  HDR_MDSP_RX_LOG_ISR    =10,   /* Rx Logging interrupt */
  HDR_MDSP_TX_LOG_ISR    =11,   /* Tx Logging Interrupt */
  HDR_MDSP_INT_CNT       
  
} hdrmdsp_isr_type;

typedef int8  hdrmdsp_timer_handle_type;


/*==========================================================================

                     Firmware register definitions

==========================================================================*/

extern volatile hdrfw_shared_memory_interface_t *hdrmdsp_shared_mem_interface;
  /* Shared memory interface */


/*===========================================================================

FUNCTION     : HDRMDSP_INIT

DESCRIPTION  : TBD

DEPENDENCIES : None.

RETURN VALUE : None.

SIDE EFFECTS : None

===========================================================================*/

void hdrmdsp_init( void );


/*===========================================================================

FUNCTION     : HDRMDSP_DISABLE_CLOCKS

DESCRIPTION  : This function turns off the HDR DSP clocks for low power
               when not in use (1x, sleep, etc.)

DEPENDENCIES : None.

RETURN VALUE : None.

SIDE EFFECTS : None

===========================================================================*/

void hdrmdsp_disable_clocks( void );


/*===========================================================================

FUNCTION     : HDRMDSP_ENABLE_CLOCKS

DESCRIPTION  : This function turns on the HDR DSP clocks.

DEPENDENCIES : None.

RETURN VALUE : None.

SIDE EFFECTS : None.

===========================================================================*/

void hdrmdsp_enable_clocks( void );

/*===========================================================================

FUNCTION     : HDRMDSP_CHECK_DSP_CLK

DESCRIPTION  : This function returns the status of DSP clocks

DEPENDENCIES : 

RETURN VALUE : True : if DSP clocks are ON. 
               FALSE: if DSP clocks are OFF

SIDE EFFECTS : None.

===========================================================================*/

boolean hdrmdsp_check_dsp_clk( void );


/*===========================================================================

FUNCTION     : HDRMDSP_FWS_APP_IS_ENABLED

DESCRIPTION  : This function returns whether fws hdr app is enabled.

DEPENDENCIES : None

RETURN VALUE : TRUE:  enabled 
               FALSE: disabled 

SIDE EFFECTS : None

===========================================================================*/
boolean hdrmdsp_fws_app_is_enabled( void );


/*===========================================================================

FUNCTION     : HDRMDSP_FWS_ENABLE_APP

DESCRIPTION  : This function enables fws hdr app.

DEPENDENCIES : None

RETURN VALUE : None.

SIDE EFFECTS : None

===========================================================================*/
void hdrmdsp_fws_enable_app( void );


/*===========================================================================

FUNCTION     : HDRMDSP_ENABLE_HDR_APP

DESCRIPTION  : This function enables HDR application

DEPENDENCIES : Enables HDR DSP application

RETURN VALUE : None.

SIDE EFFECTS : May perform rex_wait 
               HDRSRCH_ADHOC_GP_SIG may be set after a while

===========================================================================*/

void hdrmdsp_enable_hdr_app
( 
  boolean                          inline_wait
    /* perform inline wait until DSP application is enabled */
);


/*===========================================================================

FUNCTION     : HDRMDSP_CONFIG_HDR_APP

DESCRIPTION  : This function configs HDR application

DEPENDENCIES : None.

RETURN VALUE : None.

SIDE EFFECTS : None.

===========================================================================*/

void hdrmdsp_config_hdr_app
( 
  hdrfw_state_t              state,
    /* State */

  boolean                    inline_wait,
    /* perform inline wait until DSP application is enabled */

  boolean                    sleepToWakeup
    /* whether this is a sleep to wakeup transition. This is valid for active state only */
);

/*===========================================================================

FUNCTION     : HDRMDSP_PROCESS_HDR_STATE_CFG_RSP

DESCRIPTION  : This function process HDR State Cfg rsp.

DEPENDENCIES : None

RETURN VALUE : None.

SIDE EFFECTS : None

===========================================================================*/

void hdrmdsp_process_hdr_state_cfg_rsp
(
  hdrfw_state_cfg_rsp_msg_t *msg
);

/*===========================================================================

FUNCTION     : HDRMDSP_FWS_SUSPEND_APP

DESCRIPTION  : This function will send command to FWS to vote to suspend FW 
               if HDR is currently awake.

DEPENDENCIES : None

RETURN VALUE : None

SIDE EFFECTS : None

===========================================================================*/

void hdrmdsp_fws_suspend_app( void );


/*===========================================================================

FUNCTION     : HDRMDSP_FWS_RESUME_APP

DESCRIPTION  : This function will send command to FWS to resume/wake up FW if 
               HDR has voted to suspend previously.

DEPENDENCIES : None

RETURN VALUE : None

SIDE EFFECTS : None

===========================================================================*/

void hdrmdsp_fws_resume_app( void );


#if !( defined T_MSM8960 || defined FEATURE_HDR_DIME_MODEM )
/*===========================================================================

FUNCTION     : HDRMDSP_HDR_RTC_IS_STARTED

DESCRIPTION  : This function returns whether hdr rtc is started.

DEPENDENCIES : None

RETURN VALUE : TRUE:  being started
               FALSE: not being started

SIDE EFFECTS : None

===========================================================================*/
boolean hdrmdsp_hdr_rtc_is_started( void );

/*===========================================================================

FUNCTION     : HDRMDSP_SET_HDR_RTC_STARTED

DESCRIPTION  : This function set the flag to indicate HDR RTC is started.

DEPENDENCIES : None

RETURN VALUE : None.

SIDE EFFECTS : None

===========================================================================*/
void hdrmdsp_set_hdr_rtc_started ( void );
#endif /* T_MSM8960 */

#ifdef FEATURE_HDR_MODEM_COEXISTENCE_SW
/*===========================================================================

FUNCTION     : HDRMDSP_IS_MDSP_ACTIVE

DESCRIPTION  : This function returns if mdsp is active or not.

DEPENDENCIES : None

RETURN VALUE : TRUE if mdsp is active. 
               FALSE if not active. 

SIDE EFFECTS : None

===========================================================================*/
boolean hdrmdsp_is_mdsp_active ( void );
#endif /* FEATURE_HDR_MODEM_COEXISTENCE_SW */

/*===========================================================================

FUNCTION     : HDRMDSP_SEND_MMSE_CFG_MSG

DESCRIPTION  : This function sends MMSE CFG msg to FW

DEPENDENCIES : None

RETURN VALUE : None

SIDE EFFECTS : None

===========================================================================*/
void hdrmdsp_send_mmse_cfg_msg( void );

/*===========================================================================

FUNCTION     : HDRMDSP_SET_MMSE_CFG

DESCRIPTION  : Function is called to set the MMSE CFG. The config is cached 
               within hdrmdsp module and pushed to FW after state cfg msg. 

DEPENDENCIES : None

RETURN VALUE : None

SIDE EFFECTS : None

===========================================================================*/
void hdrmdsp_set_mmse_cfg
( 
  boolean rab,
  boolean arq 
);

/*===========================================================================

FUNCTION     : HDRMDSP_MSGR_SEND

DESCRIPTION  : This function sends message to MDSP.

DEPENDENCIES : None

RETURN VALUE : errno_enum_type

SIDE EFFECTS : None

===========================================================================*/
errno_enum_type hdrmdsp_msgr_send 
( 
  msgr_hdr_struct_type *msg,
    /* Firmware command to be sent */

  uint32  msg_size
  /* size of message to be sent */
);

#endif /* HDRMDSP_H */
