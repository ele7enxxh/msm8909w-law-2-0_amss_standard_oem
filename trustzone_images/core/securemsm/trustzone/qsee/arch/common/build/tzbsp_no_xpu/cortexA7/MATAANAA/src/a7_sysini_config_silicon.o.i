;-------------------------------------------------------------------------------
;-- QUALCOMM Proprietary
;-- Copyright (c) 2010, Qualcomm Technologies Incorporated. All rights reserved.
;--
;-- All data and information contained in or disclosed by this document are
;-- confidential and proprietary information of Qualcomm Technologies Incorporated, and
;-- all rights therein are expressly reserved. By accepting this material,
;-- the recipient agrees that this material and the information contained
;-- therein are held in confidence and in trust and will not be used,
;-- copied, reproduced in whole or in part, nor its contents revealed in
;-- any manner to others without the express written permission of QUALCOMM
;-- Incorporated.
;--
;-- This technology was exported from the United States in accordance with
;-- the Export Administration Regulations. Diversion contrary to U.S. law
;-- prohibited.
;-------------------------------------------------------------------------------
;-- RCS File : $Source: /local/syncdata2/3522/server_vault/Projects/code/startup/src/kr_sysini_config_silicon.s.rca $
;-- Revision : $Revision: #1 $
;-- Id : $Id: 
;-- Last Check In : $Date: 2016/06/17 $
;-- ----------------------------------------------------------------------------
;-- TestName    : a7_sysini_config_silicon.s
;-- Author      :
;-- Created     : 2012/12/21
;-- Description : This assembly file declares data areas for persistent storage
;--             : data that configures the power down and up process. Set these
;--             : memory locations to tell the power down/up routines what to do
;--             : e.g. cache maintenance, enable interrupts, etc.
;-------------------------------------------------------------------------------
;--R- Revision Log
;--R----------------------------------------------------------------------------
;--R  $Log: a7_sysini_config_silicon.s.rca $
;--R
;--R   Revision: 1.0 Fri Dec 21 16:17:36 2012 ahossein
;--R   Initial version
;--R
;-------------------------------------------------------------------------------
        EXPORT      L2_INIT_SEMAPHORE_LOC
;------------------------------------------------------------------------------
        AREA sysini_config_global, DATA, READWRITE, ALIGN=5
;------------------------------------------------------------------------------
SYSINI_CONFIG_GLOBAL
;------------------------------------------------------------------------------
L2_INIT_SEMAPHORE_LOC           DCD     0x0f
        END
