 /*======================================================================

                         Location Manager GPS DIAG interaction file

 GENERAL DESCRIPTION
  This file contains functions by which Location Manager(LM) handles
  messages from GPS DIAG task.

 EXTERNALIZED FUNCTIONS


 INITIALIZATION AND SEQUENCING REQUIREMENTS


  Copyright (c) 2005 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 

 Export of this technology or software is regulated by the U.S. Government.
 Diversion contrary to U.S. law prohibited.
======================================================================*/

/*=====================================================================

 EDIT HISTORY FOR MODULE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.

  $Header $$DateTime $$Author $ 

 when      who   what, where, why
 ----      ----  --- ---------------------------------------------------
 -
06/18/08   jlp     Added messaging support so tm_diag can set the Sv No Exist
                    list in cd.
05/07/07   jw      Added Diag GPS Intelliciever support.
 7/27/06   mr   Initial creation of header file.
======================================================================*/

#include "aries_os_api.h"
#include "aries_os_globals.h"
#include "cgps_api.h"
#include "mgp_api.h"

/*MRAWAL: The packet definations have to be moved later on 
to a common GPS DIAG definition header file*/

#define LM_DIAG_GPS_SRCH_MODE_HIGH 0x10
#define LM_DIAG_GPS_SRCH_MODE_FTCAL 0x11
#define LM_DIAG_GPS_SRCH_MODE_HIGH_DMED 0x13
#define LM_DIAG_GPS_SRCH_MODE_HIGH_SLOW 0x1b
#define LM_DIAG_TURN_OFF_JDA 0x23
#define LM_DIAG_TURN_ON_JDA 0x24
#define LM_DIAG_RESET_TIMON_CARD 0x22
#define LM_DIAG_GPS_INTELLICEIVER 0xA0



/* Definition of GPS Session Control Diag Pkt */
/*============================================================================

PACKET   ARIESDIAG_SESS_CTRL_F

PURPOSE  Sent from the DM to the DMSS with GPS session control commands

RESPONSE The DMSS sends back a ariesDiag_SessCtrlRspType packet

============================================================================*/
typedef PACKED struct PACKED_POST
{
  byte cmd_code;             /* Command code */
  byte gps_control;          /* Subcommand code */
  word data_length;          /* Length of data payload */
  byte supporting_data[36];  /* Supporting data buffer */
} aries_diag_sess_ctrl_req_type;

typedef PACKED struct PACKED_POST
{
  byte  cmd_code;            /* Command code  */
  byte  gps_control;         /* Subcommand code */
} aries_diag_sess_ctrl_rsp_type;


/****************** Global variables *************************/


/****************** Function Definitions**************************************/

/*
 ******************************************************************************
 * lm_diag_handle_diag_gsc_msg 
 *
 * Function description:
 *
 * This function handles the IPC messages coming from GPS DIAG task.
 *
 * Parameters: 
 * *p_msg - IPC message from GPSDIAG
 * 
 *
 * Return value: 
 *
 *
 ******************************************************************************
*/
void lm_diag_handle_diag_gsc_msg(os_IpcMsgType* p_msg);

/*
 ******************************************************************************
 * lm_presc_dwell_handler
 *
 * Function description:
 *
 *  lm_presc_dwell_handler simply transfers IPC payload to Presc dwell struct. Its then
 *  injected into MGP.
 *
 * Parameters: 
 *
 * *p_msg - IPC message from TM diag
 *
 * Return value: 
 *
 *  void
 *
 ******************************************************************************
*/
void lm_presc_dwell_handler( os_IpcMsgType* p_msg );

/*
 ******************************************************************************
 * lm_spectrum_analyzer_handler
 *
 * Function description:
 *
 *  This function transfers the IPC payload to Spectrum Analyzer struct. It is then
 *  injected into MGP.
 *
 * Parameters: 
 *
 * *p_msg - IPC message from TM diag
 *
 * Return value: 
 *
 *  void
 *
 ******************************************************************************
*/
void lm_spectrum_analyzer_handler( os_IpcMsgType* p_msg );

/*
 ******************************************************************************
 * lm_Rf_VerifModeSwitch_handler
 *
 * Function description:
 *
 *  lm_Rf_VerifModeSwitch_handler essentially switches MGP ON in "SARF" operation mode.
 *  This mode is same in every respect as a regular MGP ON except that search 
 *  strategy is disabled. SARF mode is turned OFF by turning MGP OFF.
 *
 * Parameters: 
 *
 * *p_msg - IPC message from TM diag
 *
 * Return value: 
 *
 *  void
 *
 ******************************************************************************
*/
void lm_Rf_VerifModeSwitch_handler( os_IpcMsgType* p_msg );

/*
 ******************************************************************************
 * lm_IqTestCmd
 *
 * Function description:
 *
 *  lm_IqTestCmd transfers IPC payload to Iqtest struct and sends it to MGP.
 *
 * Parameters: 
 *
 * *p_msg - IPC message from TM diag
 *
 * Return value: 
 *
 *  void
 *
 ******************************************************************************
*/
void lm_IqTestCmd( os_IpcMsgType* p_msg );

/*
 ******************************************************************************
 * lm_StandaloneTestCmd_handler
 *
 * Function description:
 *
 *  lm_StandaloneTestCmd_handler starts a single SV or standalone operation. 
 *  If the cmd includes clear all, then database of SV (eph, alm, health etc.)
 *  are deleted. 
 *
 * Parameters: 
 *
 * *p_msg - IPC message from TM diag
 *
 * Return value: 
 *
 *  void
 *
 ******************************************************************************
*/
void lm_StandaloneTestCmd_handler( os_IpcMsgType* p_msg );

/*
 ******************************************************************************
 * lm_MemBwTestCmd_handler
 *
 * Function description:
 *
 *  lm_MemBwTestCmd_handler simply transfers IPC payload to mem-BW struct 
 *  injected into MGP.
 *
 * Parameters: 
 *
 * *p_msg - IPC message from TM diag
 *
 * Return value: 
 *
 *  void
 *
 ******************************************************************************
*/
void lm_MemBwTestCmd_handler( os_IpcMsgType* p_msg );

/*
 ******************************************************************************
 * lm_SvNoExistCmd_handler
 *
 * Function description:
 *
 *  lm_SvNoExistCmd_handler Updates the Sv No Exist List in cd.
 *
 * Parameters: 
 *
 * *p_msg - IPC message from TM diag
 *
 * Return value: 
 *
 *  void
 *
 ******************************************************************************
*/
void lm_SvNoExistCmd_handler( const os_IpcMsgType * p_msg );

/*
 ******************************************************************************
 * lm_ForceDpoForPowerMeasCmd_handler
 *
 * Function description:
 *
 *  lm_ForceDpoForPowerMeasCmd_handler simply transfers IPC payload to inject the
 *  DPO Force command into MGP 
 *  injected into MGP.
 *
 * Parameters: 
 *
 * *p_msg - IPC message from TM diag
 *
 * Return value: 
 *
 *  void
 *
 ******************************************************************************
*/
void lm_ForceDpoForPowerMeasCmd_handler( const os_IpcMsgType * p_msg );

/*
 ******************************************************************************
 * lm_ext_mode_switch
 *
 * Function description:
 *
 *  Mode-switch requests coming from non-GPS tasks (through cgps_ModeSwitch()) will
 *  flow into this function. This will run in the client task's context.
 *
 * Parameters: 
 *
 *  e_new_state - Desired MGP state.
 *  p_cb        - Pointer to CB function.
 *
 * Return value: 
 *
 *  void
 *
 ******************************************************************************
*/
void lm_ext_mode_switch( cgps_sarf_mode_switch_enum_type e_new_state, void (*p_cb) (boolean) );

/*
 ******************************************************************************
 * Function lm_IsMgpInSarfMode
 *
 * Description:
 *
 *  Return TRUE if MGP is in SARF mode, else FALSE.
 *
 * Parameters: 
 *
 * Return value: 
 *
 *  TRUE  - MGP is in SARF mode
 *  FALSE - If MGP is NOT in SARF mode
 *
 ******************************************************************************
*/
boolean lm_IsMgpInSarfMode( void );


/*
 ******************************************************************************
 * Function lm_DiagSbasStart
 *
 * Description:
 *
 * Starts SBAS mode
 *
 * Parameters: 
 *
 * Return value: 
 *
 *
 ******************************************************************************
*/
void lm_DiagSbasStart( mgp_ConfigStructType *p_MgpConfig );

/*
 ******************************************************************************
 * Function lm_DiagSbasStop
 *
 * Description:
 *
 * Stops SBAS mode
 *
 * Parameters: 
 *
 * Return value: 
 *
 *
 ******************************************************************************
*/
void lm_DiagSbasStop( void );

/*
 ******************************************************************************
 * Function lm_DiagTsgCmd
 *
 * Description:
 *
 *  Sets TSG configuration.
 *
 * Parameters: 
 *  None
 *
 * Return value: 
 *  None
 *
 ******************************************************************************
*/
void lm_DiagTsgCmd( os_IpcMsgType* p_msg );

/*
 ******************************************************************************
 * Function lm_DiagGnssRfCmd
 *
 * Description:
 *
 *  Sets RF configuration and GNSS core registers depending on the RF config.
 *
 * Parameters: 
 *  None
 *
 * Return value: 
 *  None
 *
 ******************************************************************************
*/
void lm_DiagGnssRfCmd( os_IpcMsgType* p_msg );

/*
 ******************************************************************************
 * Function lm_DiagGnssPrxRfCmd
 *
 * Description:
 *
 *  Sets Prx RF configuration and GNSS core registers depending on the Prx RF config.
 *
 * Parameters:
 *  None
 *
 * Return value:
 *  None
 *
 ******************************************************************************
*/
void lm_DiagGnssPrxRfCmd( os_IpcMsgType* p_msg );

/*
 ******************************************************************************
 * Function lm_DiagGnssRfLinearityCmd
 *
 * Description:
 *
 *  Sets RF Linearity configuration.
 *
 * Parameters:
 *  None
 *
 * Return value:
 *  None
 *
 ******************************************************************************
*/
void lm_DiagGnssRfLinearityCmd( os_IpcMsgType* p_msg );

/*
 ******************************************************************************
 * lm_RfDevTestCmd_handler
 *
 * Function description:
 *
 *  lm_RfDevTestCmd_handler starts a multi-channel tracking operation. 
 *  If the cmd includes clear all, then database of SV (eph, alm, health etc.)
 *  are deleted. 
 *
 * Parameters: 
 *
 * *p_msg - IPC message from TM diag
 *
 * Return value: 
 *
 *  void
 *
 ******************************************************************************
*/
void lm_RfDevTestCmd_handler( os_IpcMsgType* p_msg );


