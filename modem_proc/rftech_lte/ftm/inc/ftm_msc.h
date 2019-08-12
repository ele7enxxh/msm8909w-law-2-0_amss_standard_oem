/*!
  @file         ftm_msc.h

  @brief
  This file contains FTM call flows undere different scenarios
  Common call flows are technologies independent and should be 
  applied to all technologies
  Technology specific call flow is also shown
*/

/*============================================================================
                Copyright (c) 2008 - 2009 QUALCOMM Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
==============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_lte/ftm/inc/ftm_msc.h#1 $
$DateTime: 2016/12/13 07:59:25 $
$Author: mplcsds1 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when         who     what, where, why
----------   ---     ----------------------------------------------------------
04/21/2009   pl      Iniital release
=============================================================================*/
#ifndef FTM_MSC_H
#define FTM_MSC_H

/*! @defgroup FTM_SW_Message_Sequence_Charts
    @detail
    The following MSCs are grouped by different airlink technologies that the
    Factory Test Mode (FTM) SW supports. The common procedues that are technology 
    independent are shown in FTM_COMMON_MSC

    @see
    FTM_COMMON_MSC\n
    FTM_LTE_MSC\n
*/

/*! @interface FTM_COMMON_MSC 
    @detail
    The following MSC describes the processing of FTM commands
    that are common to all technologies. For technology specific 
    MSC see corresponding MSC.

    @see 
    FTM_COMMON_MSC_FTM_Task_Power_Up_and_initialization\n
    FTM_COMMON_MSC_Get_REX_Signal_for_LTE_NS\n
    FTM_COMMON_MSC_Get_MSGR_Message_for_LTE_NS\n
    FTM_COMMON_MSC_FTM_Task_Shutdown\n
    top: FTM_SW_Message_Sequence_Charts\n
    LTE Specifics: FTM_LTE_MSC
*/

/*! @interface FTM_COMMON_MSC_FTM_Task_Power_Up_and_initialization 
    @detail
    The following MSC describes the FTM Task initialization procedure
    The FTM task is common to all technologies. The initialization procedure
    here shows how each techonology is initialized when it is supported
    by the FTM SW build

    @see
    top: FTM_SW_Message_Sequence_Charts\n
    Up: FTM_COMMON_MSC\n
    LTE Specifics: FTM_LTE_MSC\n

    @msc
    TMC, REX, "FTM Task", "FTM LTE NS", "Q Service", MSGR;
    TMC => REX [label = "rex_define_task( ftm_task_main )"];
    TMC << REX;
    --- [label = "TMC wait for ACK signal from FTM"];
    REX => "FTM Task" [label = "ftm_task_main()"];
    "FTM Task" => "Q Service" [label = "q_init( Commands )"];
    "FTM Task" << "Q Service";
    "FTM Task" => "Q Service" [label = "q_init( LTE )"];
    "FTM Task" << "Q Service";
    "FTM Task" => "Q Service" [label = "q_init( Free_Q )"];
    "FTM Task" << "Q Service";
    "FTM Task" => MSGR [label = "msgr_client_create()"];
    "FTM Task" << MSGR;
    "FTM Task" => MSGR [label = "msgr_client_add_rex_q( LTE, FTM_LTE_CMD_SIG_Q )"];
    "FTM Task" << MSGR;
    "FTM Task" => "FTM LTE NS" [label = "ftm_lte_ns_msgr_register()"];
    "FTM LTE NS" => MSGR [label = "msgr_register( LTE NS messages from LTE SW )"];
    "FTM LTE NS" << MSGR;
    "FTM Task" << "FTM LTE NS";
    TMC <- "FTM Task" [label = "ACK Signal"];
    --- [label = "FTM Task wait for START Signal"];
    ...;
    TMC -> "FTM Task" [label = "START Signal"];
    --- [label = "TMC wait for ACK signal"];
    TMC <- "FTM Task" [label = "ACK Signal"];
    --- [label = "TMC call rex_define_task for another thread"];
    --- [label = "FTM Task starts Main loop"];
    @endmsc
*/

/*! @interface FTM_COMMON_MSC_Get_REX_Signal_for_LTE_NS 
    @detail
    The following MSC describes the FTM Task processing procedure
    When a REX signal is received from DIAG that is destined for
    FTM LTE NS processing

    @see
    top: FTM_SW_Message_Sequence_Charts\n
    Up: FTM_COMMON_MSC\n
    LTE Specifics: FTM_LTE_MSC\n

    @msc
    DIAG, "FTM Task", "FTM LTE Common", "FTM LTE NS", "Q Service", LTE;
    DIAG -> "FTM Task" [label = "FTM_CMD_Q_SIG( FTM_LTE_NS_START_LTE_MODE_REQ_C )"];
    "FTM Task" => "Q Service" [label = "q_get( FTM_LTE_NS_START_LTE_MODE_REQ_C )"];
    "FTM Task" << "Q Service";
    --- [label = "FTM Task recognize that FTM_LTE_NS_START_LTE_MODE_REQ_C is a command for LTE Non-Signalling"];
    "FTM Task" => "FTM LTE Common" [label = "ftm_lte_build_msgr_msg ( FTM_LTE_NS_START_MODE_REQ_C, msgr_buf)"];
    "FTM Task" << "FTM LTE Common";
    "FTM Task" => "FTM LTE Common" [label = "ftm_lte_msg_dispatch( msgr_buf )"];
    --- [label = "FTM LTE Dispatch understand this is a LTE NS command"];

    "FTM LTE Common" => "FTM LTE NS" [label = "ftm_lte_ns_processor( msgr_buf )"];
    "FTM LTE NS" -> LTE [label = "LTE SW specific messages"];
    "FTM LTE Common" << "FTM LTE NS";
    "FTM Task" << "FTM LTE Common";
    DIAG <- "FTM Task" [label = "RSP PKT"];

    @endmsc
*/
/*
 */

/*! @interface FTM_COMMON_MSC_Get_MSGR_Message_for_LTE_NS 
    @detail
    The following MSC describes the FTM Task processing procedure
    a LTE message router message is received by the REX task

    @see
    top: FTM_SW_Message_Sequence_Charts\n
    Up: FTM_COMMON_MSC\n
    LTE Specifics: FTM_LTE_MSC\n

    @msc
    DIAG, "FTM Task", "FTM LTE Common", "FTM LTE NS", MSGR, "Q Service", LTE;
    LTE -> MSGR [label = "Some LTE Message"];
    --- [label = "MSGR recognize the at the client is a REX queue"];
    MSGR => "Q Service" [label = "q_get( REX free queue)"];
    MSGR << "Q Service";
    --- [label = "MSGR copies message into buffer from q_get()"];
    MSGR => "Q Service" [label = "q_put( REX LTE Queue)"];
    MSGR << "Q Service";
    MSGR -> "FTM Task" [label = "FTM_LTE_CMD_Q_SIG"];
    "FTM Task" => "Q Service" [label = "q_get( REX LTE Queue )"];
    "FTM Task" << "Q Service";
    --- [label = "FTM Task recognize this is a FTM LTE message"];
    "FTM Task" => "FTM LTE Common" [label = "ftm_lte_msg_dispatch( msgr_buf )"];
    --- [label = "FTM LTE Dispatch understand this is a LTE NS command"];
    "FTM LTE Common" => "FTM LTE NS" [label = "ftm_lte_ns_processor( msgr_buf )"];
    "FTM LTE NS" -> LTE [label = "LTE SW specific messages"];
    "FTM LTE Common" << "FTM LTE NS";
    "FTM Task" << "FTM LTE Common";
    "FTM Task" => "Q Service" [label = "q_put( REX free queue )"];
    "FTM Task" << "Q Service";
    DIAG <- "FTM Task" [label = "RSP PKT"];
    @endmsc
*/



/*! @interface FTM_COMMON_MSC_FTM_Task_Shutdown 
    @detail
    The following MSC describes the FTM Task Shutdown procedure
    The shutdown procedure here shows how each techonology is cleaned up

    @see
    top: FTM_SW_Message_Sequence_Charts\n
    Up: FTM_COMMON_MSC\n
    LTE Specifics: FTM_LTE_MSC\n

    @msc
    TMC, REX, "FTM Task", "FTM LTE NS", "Q Service", MSGR;
    TMC => REX [label = "rex_set_signal( TASK_STOP_SIG )"];
    TMC << REX;
    --- [label = "TMC wait for ACK signal from FTM"];
    REX -> "FTM Task" [label = "TAK_STOP_SIG"];
    --- [label = "FTM Task quits infinite loop"];
    "FTM Task" => "FTM LTE NS" [label = "ftm_lte_ns_msgr_deregister()"];
    "FTM LTE NS" => MSGR [label = "msgr_deregister( LTE NS messages from LTE SW )"];
    "FTM LTE NS" << MSGR;
    "FTM Task" << "FTM LTE NS";
    "FTM Task" => MSGR [label = "msgr_client_delete()"];
    "FTM Task" << MSGR;
    TMC <- "FTM Task" [label = "ACK Signal"];
    --- [label = "FTM TASK cease to exitst"];
    --- [label = "TMC kills another task"];
    @endmsc
*/

/*! @interface FTM_LTE_MSC
    @detail
    The following groups of MSC corresponds to the commands the LTE FTM SW 
    supports. The MSC details the sequence of how the commands shall be used,
    the effects of the command, up to the RF Device driver (RF device driver behavior is
    HW specific) and the response of the LTE FTM SW to the module issuing the 
    command.

    @see
    FTM_LTE_ENTER_MODE_MSC\n
    FTM_LTE_ACQ_MSC\n
    FTM_LTE_STOP_MODE_MSC\n
    top: FTM_SW_Message_Sequence_Charts
*/

/*! @interface FTM_LTE_ENTER_MODE_MSC
    @detail

    @see
    up: FTM_LTE_MSC\n
    top: FTM_SW_Message_Sequence_Charts\n

    @msc
    DIAG, FTM, "FTM LTE NS", "LTE L1", "LTE SRCH", RF, "RF LTE MC", "RF Device", RTR8600;
    DIAG -> FTM [label = "FTM_CMD_Q_SIQ( FTM_LTE_NS_START_LTE_MODE_REQ_C )"];
    FTM => "FTM LTE NS" [label = "ftm_lte_build_msgr_msg(FTM_LTE_NS_START_LTE_MODE_REQ_C)"];
    --- [label = "ftm_lte_build_msgr_msg() converts the received REX command to MSGR message"];
    FTM << "FTM LTE NS";
    FTM => "FTM LTE NS" [label = "ftm_lte_msg_dispatch( FTM_LTE_NS_START_LTE_MODE_REQ )"];
    "FTM LTE NS" -> "LTE L1";
    FTM << "FTM LTE NS" [label = "RSP PKT( NO RSP )"];
    DIAG <- FTM [label = "RSP PKT (NO RSP)"];
    "LTE L1" -> "LTE SRCH";
    "LTE SRCH" -> RF;
    RF => "RF LTE MC";
    "RF LTE MC" => "RF Device";
    "RF Device" => RTR8600;
    "RF Device" << RTR8600;
    "RF LTE MC" << "RF Device";
    RF << "RF LTE MC";
    "LTE SRCH" <- RF;
    "LTE L1" <- "LTE SRCH";
    FTM <- "LTE L1";
    FTM => "FTM LTE NS" [label = "ftm_lte_msg_dispatch( FTM_LTE_NS_START_LTE_MODE_CNF )"];
    FTM << "FTM LTE NS" [label = "RSP PKT( SUCCESS RSP )"];
    DIAG <- FTM [label = "RSP PKT ( SUCCESS RSP )"];
    @endmsc
*/

/*

 */
/*-----------------------------------------------------------------------------
 * Include Files
 * ---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 * Preprocessor Definition and Constants
 * ---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 * Type Declarations
 * ---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 * Enumerated typedefs
 * ---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 * Structure Typedefs
 * ---------------------------------------------------------------------------*/

/*===========================================================================

                    FUNCTION PROTOTYPES

===========================================================================*/

#endif /* #ifndef FTM_MSC_H */
