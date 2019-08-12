#ifndef FFPC_I_H
#define FFPC_I_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  FAST FORWARD POWER CONTROL PROCESSING

GENERAL DESCRIPTION
  This module is responsible for low level processing of data received
  from the base station in CDMA mode of operation for Fast Forward
  Power Control.

  Copyright (c) 1999 - 2013 Qualcomm Technologies, Inc. 
  All Rights Reserved Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies, Inc. and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies, Inc.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath: O:/src/asw/COMMON/vcs/ffpc.h_v   1.14   15 Oct 2002 18:30:50   donb  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/mux/inc/ffpc_i.h#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/05/16   eye     1x2GTA Feature changes
10/04/13   vks     Export fpc sch iq scale value
09/06/13   srk     Remove featurization around APIs which return current and
                   max FCH setpoint.
01/02/13   ab      Added Enhanced RDDS Support
08/02/12   srk     Created an API to return current FPC mode.
02/24/12   srk     Mainlined FEATURE_IS2000_P2
09/28/11   srk     Mainlined Always On features and removed Always Off Features
04/22/11   jtm     CMI Phase 4 - Final SU API changes.
02/11/11   jtm     Added RCPM FPC_MODE support for 1xA.
12/15/10   jtm     Feature cleanup.
12/09/10   jtm     Remove obsolete function declarations.
11/01/10   jtm     Private file clean up.
10/30/10   jtm     Modem 1x SU API changes.
07/23/10   jtm     Added support for 1xA FFPC modes.
06/10/10   jtm     Eliminated dependency on log_dmss.h.
04/12/10   jtm     Added FCH FPC setpoint freeze support.
02/04/10   jtm     Removed FEATURE_PLT.
01/20/10   mca     Updated PACKED macro
06/06/09   vlc     Added support for 1x Advanced (RC11.)
04/01/09   jtm     Added customer.h
03/20/09   jtm     Cleaned up header file inclusions.
11/19/08   jtm     Split various header files into internal and external
                   declarations for Common Modem Interface.
10/11/06   tjc     Added ffpc_set_setpt_offset_dbq4()
09/11/06   vlc     Added function convert_dBq8_to_lin() as it will be used
                   for RLGC as well.
04/27/05   vlc     Main lined code in current targets, cleaned out obsolete targets,
                   cleaned out obsolete features.
09/23/04   sr      Exposed prototype for ffpc_get_current_fch_setpoint()
06/16/04   sr      Merged in changes from 6500 5004 build
05/26/04   jrp/rdh Removed featurization around ffpc_get_subchan_gain_db().
04/09/04   bn      Added new interface functions to get the SCH FFPC related info
12/02/03   jrp     Mainlined db-only maintained state from Jaguar orphans
                   branch #6.
11/05/03   ejv     Removed prototype for ffpc_program_walsh_gain.
10/07/03   ejv     Removed old implementation.
09/15/03   ejv/bb  Changed curr_setpt_db from dword to int32.
08/21/03   ejv/bb  Added new FFPC implementation.
04/07/03   jrp     Lint changes.
10/15/02   dlb     Added new power control logging to combine power control
                   and frame type logging.
08/01/02   jrp     Added featurization for FPC modes 4,5,6.  Changed FPC_MODE enums
                   to be more descriptive due to SVD.
07/15/02   bgc     Added ffpc_calculate_pcbit_data prototype.
07/11/02   bn      Added function prototype to get DCCH HW setpoint
06/18/02   jrp     Added enums to fpc_mode_type for FFPC Modes 4,5,6.
04/19/02   jrp     Added ffpc_get_setpt().
11/13/01   bn      Added the enhance DCCH DTX detection and channel supervision logics
09/26/01   bgc     Added FEATURE_IS2000_SCH for ffpc_program_walsh_gain().
08/13/01   hrk     defined walsh gains for 32X.
06/15/01   day     Mainlined FEATURE_FFPC
04/20/01   sr      Merged in from MSM5100 archive
04/10/01   hrk     Added support for FEATURE_IS2000_P2
02/20/01   hrk     Renamed ffpc_convert_db_2_lin() to ffpc_sch_lin_setpt() and
                   changed prototype.
                   Renamed ffpc_lin_setpt() to ffpc_fch_lin_setpt()
11/13/00   hrk     Added curr_setpt_lin to ffpc_chan_info_type to track the
                   current linear setpoint.
                   Changed prototype for ffpc_log_frame_init(), ffpc_log_frame().
09/11/00   hrk     Changed prototype to fix errors indicated by Lint.
09/06/00   bgc     Added FEATURE_FACTORY_TESTMODE
06/20/00   hrk     Removed unnecessary include files.
06/15/00   bgc     Changed PLT to support fch and sch setpoint.
06/09/00   hrk     Removed inclusion of memory.h, log.h - not needed.
06/05/00   hrk     Added prototype for ffpc_program_walsh_gain().
06/05/00   hrk     Updated prototype definitions.
05/30/00   hrk     Added support for new MC-RXC transaction mechanism.
04/28/00   hrk     Added new enum fpc_mode_type.
04/25/00   hrk     Added FFPC support for SCH.
04/11/00   lad     Added prototypes for FFPC and PLT logging.
12/02/99   hrk     Initial revision.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

//common
#include "1x_variation.h"
#include "comdef.h"
#include "customer.h"

//mux
#include "ffpc_i.h"
#include "rxc.h"
#include "rxc_i.h"

//drivers
#include "dec5000.h"
#include "deint_i.h"

//other
#include "qw.h"
#include "log.h"


/* <EJECT> */
/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

typedef enum
{
  FPC_MODE_PRI_800Hz = 0,
  FPC_MODE_PRI_400Hz_SEC_400Hz,
  FPC_MODE_PRI_200Hz_SEC_600Hz,
  FPC_MODE_PRI_EIB,

  FPC_MODE_PRI_QIB,
  FPC_MODE_PRI_QIB_SEC_EIB,
  FPC_MODE_PRI_400Hz_SEC_EIB,

  FPC_MODE_1XA_PRI_400Hz,
  FPC_MODE_1XA_PRI_200Hz,
  FPC_MODE_1XA_PRI_200Hz_SEC_200Hz,

  FPC_MODE_FFPC_DISABLED
} fpc_mode_type;

typedef struct
{
  /* new variables, to be made permanent */
  int32          setpt_adj_dn_dbq20;   /* (units=dB Q20) down adjustment (on GOOD frame) */
  int32          setpt_adj_up_dbq20;   /* (units=dB Q20) up adjustment (on BAD frame) */
  int32          setpt_adjust_dbq4;    /* (units=dB Q4) semi-static set point adjustment for HW.
                                        * This is PCCH subchan gain and RC5 adjustment
                                        */
  int16          setpt_offset_dbq4;    /* (units=dB Q4) offset to the setpoint as applied to the inner
                                        * that can be asserted by other units of SW.
                                        */
  int32          setpt_adjusted_dbq20; /* (units=dB Q20) set point in dB adjusted for HW write.
                                        * This is adjusted by the PCCH subchannel gain and the RC.
                                        */
  byte           init_setpt_db;        /* (units=dBq3, same as cdma2000 signalling) */
  int32          curr_setpt_db;        /* (units=dBq20) [used: converted to Q3 and logged (frame logging),
                                        * also used to conpute the delta (FCH<->SCH, DCCH<->SCH) in Q3 for logging
                                        * also used (FUNCTIONALLY) for delta computations and initialization] */
  byte           min_setpt_db;         /* (units=dBq3, same as cdma2000 signalling) */
  byte           max_setpt_db;         /* (units=dBq3, same as cdma2000 signalling) */
  byte           target_fer;           /* FER encoded according to the signalling format (IS-2000.5-A-2 table 3.7.3.3.2.25-2) */
  word           fer_scaled;           /* (units=0.001 frame errors/frame) */
  boolean        olpc_enabled;
  boolean        ilpc_enabled;
  word           hw_eb_no_setpt;       /* (units=mag^2 Q8, as used by the DSP) set point as passed to the DSP */
  deint_rc_type  fwd_rc;
  byte           incl_status;
} ffpc_chan_info_type;

typedef enum
{
  FFPC_FCH=0x0,
  FFPC_DCCH,
  FFPC_SCH
} ffpc_chan_type;

typedef struct
{
  byte  fpc_subchan_gain_db; /* (units=dBq2, same as cdma2000 signalling) */
  boolean setpt_thresh_init;
  uint8 sch_thresh;
  int16 pri_sch_init_delta;
  fpc_mode_type fpc_mode;
  ffpc_chan_type fpc_pri_chan;
} ffpc_info_type;

extern ffpc_info_type       ffpc_info;
extern ffpc_chan_info_type  ffpc_fch_params;
extern ffpc_chan_info_type  ffpc_sch_params;
extern ffpc_chan_info_type  ffpc_dcch_params;

typedef PACK( struct )
{
  word pc_acc;
  word fpc_history;
  word rpc_history;
  word fch_fpc_setpt;
  boolean erasure;
  word erasure_cnt;
  word sch_fpc_setpt;
} plt_ffpc_log_type;

#define FFPC_RATCHET_FREEZE_FRAMES     3

#define FFPC_RC_SCALING_FACTOR         8
#define FFPC_LINEAR_RESOLUTION_FACTOR  16
#define FFPC_LINEAR_SCALING_FACTOR \
          (FFPC_RC_SCALING_FACTOR + FFPC_LINEAR_RESOLUTION_FACTOR)
#define FFPC_DB_SCALING_FACTOR         19
#define FFPC_LIN_SETPT_SATURATE_VALUE  0xFFFFFFFF
#define FFPC_HW_EB_NO_MAX_SETPT        0x7FFF

#define FFPC_DEFAULT_SUBCHAN_GAIN        12  /* 3 dB :units 0.25 dB  */

#define FFPC_DEFAULT_FCH_INIT_SETPT_VAL  40  /* 5 dB :units 0.125 dB */
#define FFPC_DEFAULT_FCH_MIN_SETPT_VAL   0   /* 0 dB :units 0.125 dB */
#define FFPC_DEFAULT_FCH_MAX_SETPT_VAL   80  /* 10 dB:units 0.125 dB */
#define FFPC_DEFAULT_FCH_TARGET_FER      2   /* 1 %                  */

#define FFPC_DEFAULT_DCCH_INIT_SETPT_VAL  40  /* 5 dB :units 0.125 dB */
#define FFPC_DEFAULT_DCCH_MIN_SETPT_VAL   0   /* 0 dB :units 0.125 dB */
#define FFPC_DEFAULT_DCCH_MAX_SETPT_VAL   80  /* 10 dB:units 0.125 dB */
#define FFPC_DEFAULT_DCCH_TARGET_FER      2   /* 1 %                  */

#define FFPC_DEFAULT_SCH_INIT_SETPT_VAL  56  /* 7 dB :units 0.125 dB */
#define FFPC_DEFAULT_SCH_MIN_SETPT_VAL   16  /* 2 dB :units 0.125 dB */
#define FFPC_DEFAULT_SCH_MAX_SETPT_VAL   96  /* 12 dB:units 0.125 dB */
#define FFPC_DEFAULT_SCH_TARGET_FER      2   /* 1 %                  */
#define FFPC_DEFAULT_SCH_THRESH          255 /* set to a 8 bit max   */


#define FFPC_FWD_RC_M           0x01
#define FFPC_FWD_RC_INCL        0x01

#define FFPC_MODE_M             0x02
#define FFPC_MODE_INCL          0x02

#define FFPC_INIT_SETPT_M       0x04
#define FFPC_INIT_SETPT_INCL    0x04

#define FFPC_OLPC_M             0x08
#define FFPC_OLPC_INCL          0x08

#define FFPC_SUBCHAN_GAIN_M     0x10
#define FFPC_SUBCHAN_GAIN_INCL  0x10

/* RC 3*/
#define FFPC_RC3_1X_SCH_WALSH_GAIN   8192
#define FFPC_RC3_2X_SCH_WALSH_GAIN   5793
#define FFPC_RC3_4X_SCH_WALSH_GAIN   4096
#define FFPC_RC3_8X_SCH_WALSH_GAIN   2896
#define FFPC_RC3_16X_SCH_WALSH_GAIN  2048

/* RC 4*/
#define FFPC_RC4_1X_SCH_WALSH_GAIN   8192
#define FFPC_RC4_2X_SCH_WALSH_GAIN   5793
#define FFPC_RC4_4X_SCH_WALSH_GAIN   4096
#define FFPC_RC4_8X_SCH_WALSH_GAIN   2896
#define FFPC_RC4_16X_SCH_WALSH_GAIN  2048
#define FFPC_RC4_32X_SCH_WALSH_GAIN  1448

/*RC 5*/
#define FFPC_RC5_1X_SCH_WALSH_GAIN   6689
#define FFPC_RC5_2X_SCH_WALSH_GAIN   4730
#define FFPC_RC5_4X_SCH_WALSH_GAIN   3344
#define FFPC_RC5_8X_SCH_WALSH_GAIN   2365
#define FFPC_RC5_16X_SCH_WALSH_GAIN  1672

#ifdef FEATURE_IS2000_1X_ADV
/* RC 11*/
#define FFPC_RC11_1X_SCH_WALSH_GAIN   8192
#define FFPC_RC11_2X_SCH_WALSH_GAIN   5793
#define FFPC_RC11_4X_SCH_WALSH_GAIN   4096
#define FFPC_RC11_8X_SCH_WALSH_GAIN   2896
#define FFPC_RC11_16X_SCH_WALSH_GAIN  2048
#define FFPC_RC11_32X_SCH_WALSH_GAIN  1448
#endif /* FEATURE_IS2000_1X_ADV */

/* <EJECT> */
/*===========================================================================

                      FUNCTION PROTOTYPES

===========================================================================*/

boolean ffpc_sch_fpc_enabled     (void);
void ffpc_update_setpt           (rxc_frame_type frame_status,
                                  ffpc_chan_type);
word ffpc_scale_fer              (byte fer_bin);
void ffpc_program_hw             (void);
void ffpc_program_fpc_mode       (fpc_mode_type);
void ffpc_program_setpt          (word, ffpc_chan_type);
void ffpc_tci_cmd                (rxc_cmd_msg_type *cmd_ptr);
void ffpc_check_setpt_thresh     (void);
void ffpc_send_olrm              (void);
void ffpc_disable                (void);
void ffpc_disable_chan_olpc      (ffpc_chan_type channel);
void ffpc_trans_cmd              (mctrans_action_packet_type *);

#ifdef FEATURE_IS2000_1X_ADV
void ffpc_rcpm_trans_cmd         (mctrans_action_packet_type *);
#endif /* FEATURE_IS2000_1X_ADV */

void ffpc_sch_init_params        (mctrans_ffpc_action_type *);
void ffpc_dcch_init_params       (mctrans_ffpc_action_type *);
boolean ffpc_rc_changed          (deint_rc_type, deint_rc_type);
void ffpc_fch_init_params        (mctrans_ffpc_action_type *);
void ffpc_init_setpts            (ffpc_chan_type);
void ffpc_init_outer_loop_params (mctrans_fpc_olpc_param_type *, ffpc_chan_type);
fpc_mode_type ffpc_get_current_fpc_mode( void );
void ffpc_update_fpc_mode ( boolean, fpc_mode_type );
void ffpc_init_subchan_gain ( byte );
void ffpc_init_rc ( deint_rc_type, ffpc_chan_type );
void ffpc_config_fch_ratchet_setpt_freeze (boolean enable);

/* Prototypes for logging functions. */
extern void ffpc_send_log        (void);
extern void ffpc_log_frame
(
  byte  frame_offset, /* Frame offset in 1.25 ms units. */
  word  so,           /* Service Option                 */
  word  dec_history,  /* FFPC Decision History          */
  boolean log_fch,
  boolean log_dcch,
  boolean log_sch
);

/* function to get subchan gain linear to use in DCCH DTX detection  */
byte ffpc_get_subchan_gain_lin( void );

int16 ffpc_get_sch_setpt_linQ8(void);

/* function to set programmable offset to applied inner loop FFPC set point */
void ffpc_set_setpt_offset_dbq4(int16 offset_dbq4);

/* function to get subchan gain db to use in initialization in QIB threshole  */
byte ffpc_get_subchan_gain_db( void );
/* function to get minimum set point db to use in initialization in QIB threshole  */
byte ffpc_get_dcch_min_setpt_db( void );
/* function to get DCCH hardware set point   */
word ffpc_get_dcch_hw_setpt( void );
extern const byte fpc_setpt_db_lin_0_132 [133];
extern const byte fpc_setpt_db_lin_133_255 [123];
/* <EJECT> */

#ifdef FEATURE_IS2000_REL_A
extern void ffpc_get_setpt
(
  boolean *fch_incl,
  byte *fpc_fch_curr_setpt,
  boolean *dcch_incl,
  byte *fpc_dcch_curr_setpt,
  byte *num_sup,
  byte *sch_id,
  byte *fpc_sch_curr_setpt
);
#endif //FEATURE_IS2000_REL_A

word ffpc_calculate_pcbit_data
(
  boolean fch_eib
#ifdef FEATURE_IS2000_REL_A
  ,boolean sch_eib
  ,boolean fch_qib
#endif /* FEATURE_IS2000_REL_A */
);

/*========================================================================

FUNCTION FFPC_GET_CURRENT_FCH_SETPOINT

DESCRIPTION
  This function retrieves the current FCH FFPC setpoint.

DEPENDENCIES
  None.

RETURN VALUE
  Signed Int32 setpoint in dBQ20.

SIDE EFFECTS
  None

=========================================================================*/
int32 ffpc_get_current_fch_setpoint( void );

/*========================================================================

FUNCTION FFPC_GET_CURRENT_FCH_SETPOINT_DBQ3

DESCRIPTION
  This function retrieves the current FCH FFPC setpoint.

DEPENDENCIES
  None.

RETURN VALUE
  Signed Int32 setpoint in dBQ3.

SIDE EFFECTS
  None

=========================================================================*/
int32 ffpc_get_current_fch_setpoint_dbq3( void );

/*========================================================================

FUNCTION FFPC_GET_MAX_FCH_SETPOINT_DBQ3

DESCRIPTION
  This function retrieves the max FCH FFPC setpoint.

DEPENDENCIES
  None.

RETURN VALUE
  Signed Int32 setpoint in dBQ3.

SIDE EFFECTS
  None

=========================================================================*/
int32 ffpc_get_max_fch_setpoint_dbq3( void );

/*=========================================================================
FUNCTION
  convert_dBq8_to_lin()

DESCRIPTION
  Converts dB to linear.

DEPENDENCE
  None.

RETURN VALUE
  10^(input/10) in Q factor given by input

SIDE EFFECTS
  None.
=========================================================================*/
extern int32 convert_dBq8_to_lin
(
 int32 x_dB_q8,          /* db value in Q8 (i.e. resolution of 1/256 dB) */
 int32 out_Q,            /* desired Q-factor of the output */
 int32 out_b             /* number of signed bits of output */
);

/*=========================================================================
FUNCTION FFPC_GET_IQACC_SCALE

DESCRIPTION
  This function will return the IQACC_SCALE MDSP-register based on the
  F-SCH Radio Configuration and Rate.

DEPENDENCIE
  None.

RETURN VALUE

SIDE EFFECTS
  None.
=========================================================================*/
extern uint16 ffpc_get_iqacc_scale (void);
#endif /* FFPC_I_H */
