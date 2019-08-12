#ifndef     MACULDL_H
#define     MACULDL_H
/*=========================================================================================

                   U P L I N K   D O W N L I N K   M A C  H E A D E R   F I L E

This file contains functions that would be used both by the UL and DL task.

Copyright (c) 2001 by Qualcomm Technologies, Inc.  All Rights Reserved.
Copyright (c) 2002-2006, 2008-2009 by Qualcomm Technologies, Inc.  All Rights Reserved.

========================================================================================*/


/*========================================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/msm5200/mac/vcs/maculdl.h_v   1.0   11 Jun 2002 18:37:16   skrishna  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/mac/inc/maculdl.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------
07/13/12   grk     MBMS feature cleanup at L2(MBMS_DEBUG_1, FEATURE_MODEM_MBMS)
01/02/12   grk     Feature cleanup
07/09/09   mrg     Lint errors fixed
07/02/09   mrg   Coade added under feature FEATURE_TC_CONSTANT_DELAY_ENH
                for the TS 34.121 TC 7.9.1A fix
                with this fix we are maintaing a constant delay for the LB-2, 
                if the delay corssed more than 10 TTIs we are dropingthe SDUs.
05/07/09   ssg     Updated Copyright Information
04/26/06   gsc     Added support for HSDPA re-config optimizations
12/05/05   sk      Corrected the name for the prototype for function
                   l1_post_dl_ul_mac_config_sig_at_l1_failure(). Remove ul from the name.
11/03/05   sk/gsc  Added prototype for fucntion l1_post_dl_ul_mac_config_sig_at_l1_failure
12/30/04   sk      Changed the value of MAX_NUM_CFN from 4 to 10.
04/29/03   sk      The structure lb_delay_struct_type is defined under FEATURE_TC_CONSTANT_DELAY.
08/24/02   sk      Removed function declaration l1_post_mac_config_sig() and added
                   declarations for l1_post_ul_mac_config_sig(),l1_post_dl_mac_config_sig()
08/06/02   sk      Changed the name l1_post_ul_mac_config_sig to l1_post_mac_config_sig().
06/06/02   sk      Created the file.

=========================================================================================*/
#include "wcdma_variation.h"
#include  "customer.h"
#include  "rex.h"
#include  "msg.h"
#include  "task.h"
#include  "l2dltask.h"
#include  "l2ultask.h"
#include  "l1sapcommon.h"
#include  "l1task_v.h"
#include "l2ultask.h"
#include "l2dltask.h"

#ifdef FEATURE_TC_CONSTANT_DELAY

#define MAX_NUM_CFN 10

#endif

/*==============================================================================================
 FUNCTION NAME  l1_post_ul_mac_config_sig

 FUNCTION   This function is called by physical layer and it posts the config signal to l2 command
            queue.
===============================================================================================*/
void l1_post_ul_mac_config_sig (void) ;


/*==============================================================================================
 FUNCTION NAME  l1_post_dl_mac_config_sig

 FUNCTION   This function is called by physical layer and it posts the config signal to l2 command
            queue.
===============================================================================================*/
void l1_post_dl_mac_config_sig (void) ;

/*==============================================================================================
 FUNCTION NAME  l1_post_dl_mac_config_sig_at_l1_failure

 FUNCTION   This function is called by physical layer and it posts the config signal to l2 command
            queue when there is failure to process CPHY_SETUP_REQ and MAC UL or DL sync for MAC
            config is requested
===============================================================================================*/
void l1_post_dl_mac_config_sig_at_l1_failure (void);

/*==============================================================================================
 FUNCTION NAME  l1_post_mac_hs_config_sig

 FUNCTION   This function is called by physical layer and it posts the HS config signal to l2 command.
===============================================================================================*/
void l1_post_mac_hs_config_sig (void);

/*==============================================================================================
 FUNCTION NAME  maci_post_l1_config_ind

 FUNCTION   This function posts to L1 the configuration sucess message. This function can be
            called by both the UL and DL MAC to post a configuration sucess message to L1.

===============================================================================================*/
void  maci_post_l1_config_ind (l1_cmd_enum_type cmd_id);

#ifdef FEATURE_TC_CONSTANT_DELAY
#ifdef FEATURE_TC_CONSTANT_DELAY_ENH
#define LB_DELAY_INVALID_CFN 0XFFFF
#endif
typedef struct{
 boolean first_tb; /* True Indicates if this is the first transport block that is received */
 uint16   dl_cfn[MAX_NUM_CFN]; /* Downlink CFN */
 uint8   delay;     /* The constant delay which is applied to every transport block.*/
 uint8   cur_idx;   /* Index where the downlink cfn is stored */
 uint8   start_idx; /* Index where the Uplink task starts reading */
}lb_delay_struct_type;

/* Stores the current ul cfn */
extern uint8  cur_ul_cfn;

#endif /* feature_tc_constant_delay */

#endif
