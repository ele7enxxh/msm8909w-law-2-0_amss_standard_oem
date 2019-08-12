#ifndef MDSP_UTIL_H
#define MDSP_UTIL_H
/*===========================================================================
                          M D S P _ U T I L . H


GENERAL DESCRIPTION
  This header file contains macros and funtion prototypes for general
  utility functionality for the mDSP.

EXTERNALIZED FUNCTIONS

INTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS


  Copyright (c) 2000, 2013 by Qualcomm Technologies, Inc.  All Rights Reserved.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/l1/vcs/mdsputil.h_v   1.5   19 Apr 2002 09:55:42   yshi  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/inc/mdsputil.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     -------------------------------------------------------- 
02/24/15    ap      Initial check-in for WL1 QSH framework 
11/14/13    pj      Wrapped HWIO access with WL1_HWIO macro
04/01/13    hk      Mainlined FEATURE_SHO_TPC_THRESH
05/22/12    pr      STMR changes for Dime
03/28/12    zr      Added feature definitions for Dime
02/13/12    dm      Nikel feature cleanup
02/12/12    hk      Nikel feature cleanup
01/30/12    vs      Feature cleanup.
01/20/12    zr      Mainlined FEATURE_WCDMA_GENESIS_SW, removed FEATURE_AL1
01/24/11    stk     Renamed HWIO regs for nikel
08/23/10    ks      Removed inclusion of mdsp header for Genesis (featurized)
12/13/07    rmak    Use clkregim API to determine IS_MDSP_CLOCK_ENABLED
05/31/07    am      TX precision optimizations
06/13/06    bbd     Featurized micro to MDSP interrupt for MSM7600.
08/24/05    vb      Externed mdsp_set_cltd_vars
03/10/05    mg/asr  Changed IS_MDSP_CLOCK_ENABLED() macro for 6275.
11/04/04    src     Merged in 6275-specific changes.
06/02/2004  bbd     Removed mdsp_write_pre_powerup_vars() because it is no longer
                    supported. Added mdsp_set_hw_cpp_pf_gains() to set the HW CPP
                    pilot filter gains.
04/12/04    scm     Implement IS_MDSP_CLOCK_ENABLED() macro.
02/12/04    bbd     Added mdsp_write_pre_powerup_vars() prototype.
07/24/03    yus     Change OUTP32 to HWIO_OUT.
02/04/03    yus     Added a IIR filter macro. 
                    It's defined under FEATURE_SHO_TPC_THRESH.
07/30/02    m       Added new set of macro definitions to
                          MDSP_16BIT_READ
                          MDSP_16BIT_WRITE
                          MDSP_32BIT_READ
                          MDSP_32BIT_WRITE
                    to work with MDSP service
05/21/01    jrs     Define away the ISSUE_CMD_INTR_TO_MDSP macro for FEATURE_AL1
                    builds
04/19/2001  sh      Changed FEATURE_VST to FEATURE_AL1.
03/21/2001  mdl     Include vstmdsputil.h and #ifndef out the mDSP
                    read/write macros for for FEATURE_VST

10/18/2000  mdl     first cut
===========================================================================*/


#include "wcdma_variation.h"
#include "comdef.h"
#include "msm.h"
#include "l1msm.h"

#if defined (FEATURE_QSH_DUMP) || defined(FEATURE_QSH_EVENT_METRIC)
#include "wl1_qsh_ext.h"
#endif /* defined (FEATURE_QSH_DUMP) || defined(FEATURE_QSH_EVENT_METRIC) */

#ifdef UNICORE_MODEM
#define ISSUE_CMD_INTR_TO_MDSP() \
 MSG_HIGH("Issue cmd interrupt to MDSP", 0, 0, 0) \
 wfw_sw_int_handler(0xA)

/* This will go away with IPC
*/
#else
#define ISSUE_CMD_INTR_TO_MDSP() WL1_HWIO_OUTI(SW_QDSP6SS_INTn, 13, 0x1);
#endif

/*--------------------------------------------------------------------------
Macros for one pole IIR filter
    y(n) = (N/M)*x(n) + (1-N/M)*y(n-1)
--------------------------------------------------------------------------*/
#define COEFF_N 1
#define COEFF_M 2

#define IIR_FILT(x,y)  ((COEFF_N*x + (COEFF_M-COEFF_N)*y)/COEFF_M)

/*-----------------------------------------------------------------------------
--------------------- READ AND WRITE VALUES TO THE MDSP -----------------------
-----------------------------------------------------------------------------*/

/* read a 16 bit value from the MDSP */
#define MDSP_16BIT_READ(port) (uint16) mdsp_read(((volatile uint16 *)(port)))


/* read a 32 bit value from the MDSP */
#define MDSP_32BIT_READ(port) (uint32) mdsp_read(((volatile uint32 *)(port)))


#ifdef FEATURE_MDSP_SVC
/* write a 16 bit value from the MDSP */
#define MDSP_16BIT_WRITE(port, val) ERR_FATAL("MDSP_16BIT_WRITE",0,0,0);


/* write a 32 bit value from the MDSP */
#define MDSP_32BIT_WRITE(port, val) ERR_FATAL("MDSP_32BIT_WRITE",0,0,0);
#else

/* read a 16 bit value from the MDSP */
#define MDSP_16BIT_READ(port) (uint16) (*((volatile uint16 *) ((uint32)(port))))

/* write a 16 bit value from the MDSP */
#define MDSP_16BIT_WRITE(port, val) \
  (*((volatile uint16 *) (((uint32)(port)))) = ((uint16) (val)))

/* read a 32 bit value from the MDSP */
#define MDSP_32BIT_READ(port) (uint32) (*((volatile uint32 *) (((uint32)(port)))))

/* write a 32 bit value from the MDSP */
#define MDSP_32BIT_WRITE(port, val) \
  (*((volatile uint32 *) (((uint32)(port)))) = ((uint32) (val)))
/*---------------------------------------------------------------------------*/
#endif /* FEATURE_MDSP_SVC */




/*--------------------------------------------------------------------------
Macros for reading the mDSP version information
--------------------------------------------------------------------------*/
#define READ_MDSP_ROM_VERSION_INFO() MDSP_READ(MDSP_VERSION_ROM)

#define MDSP_MAJOR_ROM_VERSION(version) (((version) & 0xFF00) >> 8)

#define MDSP_MINOR_ROM_VERSION(version) ((version) & 0x00FF)

#define READ_MDSP_RAM_VERSION_INFO() MDSP_16BIT_READ(MDSP_VERSION_RAM)

#define MDSP_MAJOR_RAM_VERSION(version) (((version) & 0xFF00) >> 8)

#define MDSP_MINOR_RAM_VERSION(version) ((version) & 0x00FF)

/* Macro to determine if mDSP clock is enabled or not. */
#define IS_MDSP_CLOCK_ENABLED()  clk_regime_is_on(CLK_RGM_MDSP_M)

/****************************************************************************
 ****************************************************************************
                            FUNCTION PROTOTYPES
 ****************************************************************************
 ***************************************************************************/

/*===========================================================================

FUNCTION        MDSP_SET_CLTD_VARS

DESCRIPTION     This routine writes the CLTD threshold and related vars to MDSP

DEPENDENCIES    NONE

RETURN VALUE    none

SIDE EFFECTS    none

===========================================================================*/
void mdsp_set_cltd_vars(void);

/*===========================================================================

FUNCTION        MSM16CPY

DESCRIPTION     This routine copies 16 bit data between the ARM memory and the
                DDSP memory. This routine may be moved to another file once
    the files for common services are defined.

DEPENDENCIES    none

RETURN VALUE    none

SIDE EFFECTS    none

===========================================================================*/
void mem16cpy ( uint16 *src, uint16 *dest, uint16 len );

/*===================================================================

FUNCTION        GETSQRT(x)

DESCRIPTION     This routine returns (sqrt(x)*2^11)
                
DEPENDENCIES    none

RETURN VALUE    sqrt(x)*2^11

SIDE EFFECTS    none

===========================================================================*/
uint32 getsqrt(uint32 x);

/*===================================================================

FUNCTION        GETSQRT_ROUND(x)

DESCRIPTION     This routine returns (sqrt(x)*2^11) rounding towardfs
                +infinity
                
DEPENDENCIES    none

RETURN VALUE    sqrt(x)*2^11

SIDE EFFECTS    none

===========================================================================*/
uint32 getsqrt_round(uint32 x);

#ifdef FEATURE_QSH_DUMP
/*===========================================================================

FUNCTION        MDSP_QSH_POPULATE_COMMON_DUMP_INFO

DESCRIPTION     This function calls the MDSP API to fill in the MDSP cmd related info for QSH

DEPENDENCIES    NONE

RETURN VALUE    none

SIDE EFFECTS    none

===========================================================================*/
void mdsp_qsh_populate_common_dump_info(wl1_common_mini_debug_info_struct_type *common_dump_ptr);
#endif /* FEATURE_QSH_DUMP */
#endif
