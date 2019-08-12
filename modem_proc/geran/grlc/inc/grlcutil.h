/*****************************************************************************
***
*** TITLE
***
***  GPRS LAYER 2 RLC UTILITY FUNCTIONS & MACROS
***
***
*** DESCRIPTION
***
***  Utility macros and functions for RLC operations.
***
***
*** Copyright (c) 2001-2013 Qualcomm Technologies, Inc.
*** All Rights Reserved. QUALCOMM Proprietary.
***
*** Export of this technology or software is regulated by the U.S.
*** Government. Diversion contrary to U.S. law prohibited.
***
*****************************************************************************/


/*****************************************************************************
***
***                        EDIT HISTORY FOR MODULE
***
***  This section contains comments describing changes made to the module.
***  Notice that changes are listed in reverse chronological order.
***
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grlc/inc/grlcutil.h#1 $
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 10-04-01   tjw     Add customer.h and comdef.h to all SU API files
*** 03/15/01    hv     RLC util fns and macros.
*** 07/17/01    hv     Added DIF macro
*** 05/11/04    hv     Added CHECK_RANGE()
*** 07/01/04    hv     Added macros for GRLC_DL_PL1_EPDAN_REQ_SIG
*** 08/03/04    hv     Added EGPRS code
*** 09/19/12    hv     Added dual data space functionality
***
*****************************************************************************/

#ifndef INC_GRLCUTIL_H
#define INC_GRLCUTIL_H

/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/

#include "geran_variation.h"
#ifndef CUSTOMER_H
  #include "customer.h"
#endif
#ifndef COMDEF_H
  #include "comdef.h"
#endif


#include "gprsdef.h"
#include "gprsdef_g.h"
#include "grlctsk.h"
#include "grlcmaccmn.h"
#include "rex.h"
#include "task.h"
#include "geran_multi_sim.h"


/*****************************************************************************
***
***     Public Type Definitions
***
*****************************************************************************/



/*****************************************************************************
***
***     Public Defines & Constants
***
*****************************************************************************/


/*****************************************************************************
***
***     Public Data References
***
*****************************************************************************/

/*****************************************************************************
***
***     Public Macros
***
*****************************************************************************/


/*===========================================================================
===
===  MACRO         BSIZE
===
===  DESCRIPTION
===
===    Return size of item in bytes
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
#define BSIZE(x)  (sizeof(x)/sizeof(byte))

/*----------------------------------------
** Macro to return modulo-SNS of a value
**----------------------------------------
*/
#define MOD_SNS(x)    ((x)%SNS)


/*===========================================================================
===
===  MACRO   INC_MOD()
===          DEC_MOD()
===          ADD_MOD()
===
===          MOD_DIF()
===
===  DESCRIPTION
===     Modulo operation macros.
===
===    increment the variable 'x' modulo 'mod'
===    decrement the variable 'x' modulo 'mod'
===    add the variable 'x' to 'a' modulo 'mod'
===    subtract the variable 'a' from 'x' modulo 'mod'
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===    INC_MOD(), DEC_MOD() and ADD_MOD() assign the result back into the 'x'
===    variable. MOD_DIF() simply returns the result without assigning it to
===    the 'x' variable.
===
===  SIDE EFFECTS
===
===========================================================================*/
#define INC_MOD(x,mod)    { (x)=((x)+1) % (mod);    }
#define DEC_MOD(x,mod)    { (x)=((x) + (mod) -1) % (mod);    }

/* Not used and clashed with def in gtmrs.h, so remove for now */
/* #define ADD_MOD(x,a,mod)  { (x)=((x)+(a)) % (mod);  } */

#define MOD_DIF(x,a,mod)  ( (x)>=(a) ? (x)-(a) : (mod)+(x)-(a))


/* Already defined elsewhere but is repeated here for convenience
*/
#define ESNS  2048

/*----------------------------------------
** Macro to return modulo-SNS of a value
**----------------------------------------
*/
#define MOD_ESNS(x)    ((x)%ESNS)

/*===========================================================================
===
===  MACRO   MOD_DIF_ESNS
===
===  DESCRIPTION
===     Returns the difference between a and b modulo 2048
===     eg. MOD_DIF_ESNS(20,1) returns 19
===         MOD_DIF_ESNS(1,20) returns 2029
===
===  DEPENDENCIES
===    All params of the same type (uint16)
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
#define MOD_DIF_ESNS(a,b)   ( (a) >= (b) ? (a)-(b) : (ESNS)+(a)-(b) )

/*===========================================================================
===
===  MACRO   IS_GE_MOD()
===          IS_GT_MOD()
===
===  DESCRIPTION
===     IS_GE_MOD() Returns if a is greater than or equal to b modulo ESNS in
===     the context of window size WS
===
===     IS_GT_MOD() Returns if a is greater than b modulo ESNS in the context
===     of window size WS
===
===  DEPENDENCIES
===
===  RETURN VALUE
===    boolean
===
===  SIDE EFFECTS
===    none
===
===========================================================================*/
#define IS_GE_MOD(a,b,WS)  ( (a)==(b) || MOD_DIF_ESNS((a),(b)) < (WS) )
#define IS_GT_MOD(a,b,WS)  ( (a) != (b) && MOD_DIF_ESNS((a),(b)) < (WS) )



/*===========================================================================
===
===  MACRO      DIF()
===
===  DESCRIPTION
===
===     Return the difference between two values
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
#define DIF(x,y)          ( (x) > (y) ? (x)-(y) : (y)-(x) )


/*===========================================================================
===
===  MACRO      MIN3()
===             MIN4()
===
===  DESCRIPTION
===
===    Macro to return the minimum of the three/four values
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
#define MIN3(a,b,c)     MIN(MIN((a),(b)),(c))
#define MIN4(a,b,c,d)   MIN(MIN(MIN((a),(b)),(c)),(d))
#define MIN5(a,b,c,d,e) MIN(MIN(MIN(MIN((a),(b)),(c)),(d)),(e))


/*===========================================================================
===
===  MACRO         IS_GE_MODULO_COMP()
===
===  DESCRIPTION
===
===    Return TRUE if modulo 'mod' comparison of 'A' is greater than or
===    equal to 'B'. Return FALSE otherwise.
===
===  DEPENDENCIES
===
===    all parameters must be of the same type
===
===  RETURN VALUE
===
===    boolean
===
===  SIDE EFFECTS
===
===========================================================================*/
#define IS_GE_MODULO_COMP(a,b,mod)    ((((a) - (b)) & ((mod)/2)) == 0)


/*===========================================================================
===
===  MACRO         IS_GT_MODULO_COMP()
===
===  DESCRIPTION
===
===    Return TRUE if modulo 'mod' comparison of 'A' is greater than 'B'.
===    Return FALSE otherwise.
===
===  DEPENDENCIES
===
===    all parameters must be of the same type
===
===  RETURN VALUE
===
===    boolean
===
===  SIDE EFFECTS
===
===========================================================================*/
#define IS_GT_MODULO_COMP(a,b,mod)    ( ((a) != (b)) && IS_GE_MODULO_COMP((a),(b),(mod)))


/*===========================================================================
===
===  MACROS
===                RLC_UL_SET_PL1_SIG()
===                RLC_UL_SET_MAC_SIG()
===                RLC_UL_SET_GRR_SIG()
===                RLC_UL_SET_GAN_SIG()
===                RLC_UL_SET_LLC_PDU_SIG()
===                RLC_UL_SET_LLC_TMP_PDU_SIG()
===                RLC_UL_SET_LLC_MSG_SIG()
===                RLC_UL_SET_TMR_SIG()
===
===                RLC_UL_CLR_PL1_SIG()
===                RLC_UL_CLR_MAC_SIG()
===                RLC_UL_CLR_GRR_SIG()
===                RLC_UL_CLR_GAN_SIG()
===                RLC_UL_CLR_LLC_PDU_SIG()
===                RLC_UL_CLR_LLC_TMP_PDU_SIG()
===                RLC_UL_CLR_LLC_MSG_SIG()
===                RLC_UL_CLR_TMR_SIG()
===
===                RLC_UL_IS_PL1_SIG_SET()
===                RLC_UL_IS_MAC_SIG_SET()
===                RLC_UL_IS_GRR_SIG_SET()
===                RLC_UL_IS_GAN_SIG_SET()
===                RLC_UL_IS_LLC_PDU_SIG_SET()
===                RLC_UL_IS_LLC_TMP_PDU_SIG_SET()
===                RLC_UL_IS_LLC_MSG_SIG_SET()
===                RLC_UL_IS_TMR_SIG_SET()
===
===                RLC_DL_SET_PL1_SIG()
===                RLC_DL_SET_MAC_SIG()
===                RLC_DL_SET_TMR_SIG()
===
===                RLC_DL_CLR_PL1_SIG()
===                RLC_DL_CLR_MAC_SIG()
===                RLC_DL_CLR_TMR_SIG()
===
===                RLC_DL_IS_PL1_SIG_SET()
===                RLC_DL_IS_MAC_SIG_SET()
===                RLC_DL_IS_TMR_SIG_SET()
===
===                GET_HEADER_TYPE()
===                GET_K()
===                TX_ARR()
===                RX_ARR(gas_id, )
===
===  DESCRIPTION
===
===    A bunch of macros to set and clear and test individual RLC UL/DL
===    signal masks.
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===  none
===========================================================================*/

/* Macros to set and clear a RLC UL signal mask
**
** HARD associating the following
** GERAN_ACCESS_STRATUM_ID_1 --> GERAN_TASK_ID_GRLC_UL
** GERAN_ACCESS_STRATUM_ID_2 --> GERAN_TASK_ID_GRLC_UL_2
** GERAN_ACCESS_STRATUM_ID_3 --> GERAN_TASK_ID_GRLC_UL_3
**
** Note if 'gas_id' is not ID_1 then ID_2 is assumed
** If a new ID follows _ID_2 then the tertiaries below must be changed accordingly
**
** 'x' represents 
*/

#define RLC_UL_SET_SIG(gas_id, x)                                         \
  {                                                                       \
    (void)rex_set_sigs( geran_tcb_read(                                   \
       (GERAN_ACCESS_STRATUM_ID_1 == gas_id) ? GERAN_TASK_ID_GRLC_UL :    \
       (GERAN_ACCESS_STRATUM_ID_2 == gas_id) ? GERAN_TASK_ID_GRLC_UL_2 :  \
        GERAN_TASK_ID_GRLC_UL_3       ),((x) & GRLC_UL_MASTER_SIG) );     \
    grlc_ul_task_sigs[gas_id] = rex_get_sigs(                             \
                        geran_tcb_read(                                   \
       (GERAN_ACCESS_STRATUM_ID_1 == gas_id) ? GERAN_TASK_ID_GRLC_UL :    \
       (GERAN_ACCESS_STRATUM_ID_2 == gas_id) ? GERAN_TASK_ID_GRLC_UL_2 :  \
        GERAN_TASK_ID_GRLC_UL_3       ));                                 \
  }

#define RLC_UL_CLR_SIG(gas_id, x)                                         \
  {                                                                       \
    (void)rex_clr_sigs( geran_tcb_read(                                   \
       (GERAN_ACCESS_STRATUM_ID_1 == gas_id) ? GERAN_TASK_ID_GRLC_UL :    \
       (GERAN_ACCESS_STRATUM_ID_2 == gas_id) ? GERAN_TASK_ID_GRLC_UL_2 :  \
        GERAN_TASK_ID_GRLC_UL_3       ),((x) & GRLC_UL_MASTER_SIG) );     \
    grlc_ul_task_sigs[gas_id] = rex_get_sigs(                             \
                        geran_tcb_read(                                   \
       (GERAN_ACCESS_STRATUM_ID_1 == gas_id) ? GERAN_TASK_ID_GRLC_UL :    \
       (GERAN_ACCESS_STRATUM_ID_2 == gas_id) ? GERAN_TASK_ID_GRLC_UL_2 :  \
        GERAN_TASK_ID_GRLC_UL_3       ));                                 \
  }

/* Macros to set and clear a RLC DL signal mask
*/
#define RLC_DL_SET_SIG(gas_id, x)                                         \
  {                                                                       \
    (void)rex_set_sigs( geran_tcb_read(                                   \
       (GERAN_ACCESS_STRATUM_ID_1 == gas_id) ? GERAN_TASK_ID_GRLC_DL :    \
       (GERAN_ACCESS_STRATUM_ID_2 == gas_id) ? GERAN_TASK_ID_GRLC_DL_2 :  \
        GERAN_TASK_ID_GRLC_DL_3       ),((x) & GRLC_DL_MASTER_SIG) );     \
    grlc_dl_task_sigs[gas_id] = rex_get_sigs(                             \
                        geran_tcb_read(                                   \
       (GERAN_ACCESS_STRATUM_ID_1 == gas_id) ? GERAN_TASK_ID_GRLC_DL :    \
       (GERAN_ACCESS_STRATUM_ID_2 == gas_id) ? GERAN_TASK_ID_GRLC_DL_2 :  \
        GERAN_TASK_ID_GRLC_DL_3       ));                                 \
  }

#define RLC_DL_CLR_SIG(gas_id, x)                                         \
  {                                                                       \
    (void)rex_clr_sigs( geran_tcb_read(                                   \
       (GERAN_ACCESS_STRATUM_ID_1 == gas_id) ? GERAN_TASK_ID_GRLC_DL :    \
       (GERAN_ACCESS_STRATUM_ID_2 == gas_id) ? GERAN_TASK_ID_GRLC_DL_2 :  \
        GERAN_TASK_ID_GRLC_DL_3       ),((x) & GRLC_DL_MASTER_SIG) );     \
    grlc_dl_task_sigs[gas_id] = rex_get_sigs(                             \
                        geran_tcb_read(                                   \
       (GERAN_ACCESS_STRATUM_ID_1 == gas_id) ? GERAN_TASK_ID_GRLC_DL :    \
       (GERAN_ACCESS_STRATUM_ID_2 == gas_id) ? GERAN_TASK_ID_GRLC_DL_2 :  \
        GERAN_TASK_ID_GRLC_DL_3       ));                                 \
  }

/* Macros to set and clear specific RLC UL signal mask
*/
#define RLC_UL_SET_LLC_TMP_PDU_SIG(gas_id)  RLC_UL_SET_SIG(gas_id, GRLC_UL_LLC_TMP_PDU_SIG)
#define RLC_UL_CLR_LLC_TMP_PDU_SIG(gas_id)  RLC_UL_CLR_SIG(gas_id, GRLC_UL_LLC_TMP_PDU_SIG)

#define RLC_UL_SET_LLC_PDU_SIG(gas_id)      RLC_UL_SET_SIG(gas_id, GRLC_UL_LLC_PDU_SIG)
#define RLC_UL_CLR_LLC_PDU_SIG(gas_id)      RLC_UL_CLR_SIG(gas_id, GRLC_UL_LLC_PDU_SIG)

#define RLC_UL_SET_LLC_MSG_SIG(gas_id)      RLC_UL_SET_SIG(gas_id, GRLC_UL_LLC_MSG_SIG)
#define RLC_UL_CLR_LLC_MSG_SIG(gas_id)      RLC_UL_CLR_SIG(gas_id, GRLC_UL_LLC_MSG_SIG)

#define RLC_UL_SET_PL1_SIG(gas_id)          RLC_UL_SET_SIG(gas_id, GRLC_UL_PL1_SIG)
#define RLC_UL_CLR_PL1_SIG(gas_id)          RLC_UL_CLR_SIG(gas_id, GRLC_UL_PL1_SIG)

#define RLC_UL_SET_MAC_SIG(gas_id)          RLC_UL_SET_SIG(gas_id, GRLC_UL_MAC_SIG)
#define RLC_UL_CLR_MAC_SIG(gas_id)          RLC_UL_CLR_SIG(gas_id, GRLC_UL_MAC_SIG)


#define RLC_UL_SET_TMR_SIG(gas_id)          RLC_UL_SET_SIG(gas_id, GRLC_UL_TMR_SIG)
#define RLC_UL_CLR_TMR_SIG(gas_id)          RLC_UL_CLR_SIG(gas_id, GRLC_UL_TMR_SIG)

#ifndef PERLUTF
#define RLC_UL_SET_ITN_SIG(gas_id)          RLC_UL_SET_SIG(gas_id, GRLC_UL_ITN_SIG)
#else
  #error code not present
#endif

#define RLC_UL_CLR_ITN_SIG(gas_id)          RLC_UL_CLR_SIG(gas_id, GRLC_UL_ITN_SIG)

#define RLC_UL_SET_LOG_SIG(gas_id)          RLC_UL_SET_SIG(gas_id, GRLC_UL_LOG_SIG)
#define RLC_UL_CLR_LOG_SIG(gas_id)          RLC_UL_CLR_SIG(gas_id, GRLC_UL_LOG_SIG)


/* Macros to test for a specific signal mask being set
*/
#define RLC_UL_IS_LLC_PDU_SIG_SET(gas_id)     (grlc_ul_task_sigs[gas_id] & GRLC_UL_LLC_PDU_SIG)
#define RLC_UL_IS_LLC_TMP_PDU_SIG_SET(gas_id) (grlc_ul_task_sigs[gas_id] & GRLC_UL_LLC_TMP_PDU_SIG)
#define RLC_UL_IS_LLC_MSG_SIG_SET(gas_id)     (grlc_ul_task_sigs[gas_id] & GRLC_UL_LLC_MSG_SIG)
#define RLC_UL_IS_PL1_SIG_SET(gas_id)         (grlc_ul_task_sigs[gas_id] & GRLC_UL_PL1_SIG)
#define RLC_UL_IS_MAC_SIG_SET(gas_id)         (grlc_ul_task_sigs[gas_id] & GRLC_UL_MAC_SIG)


#define RLC_UL_IS_TMR_SIG_SET(gas_id)         (grlc_ul_task_sigs[gas_id] & GRLC_UL_TMR_SIG)
#define RLC_UL_IS_ITN_SIG_SET(gas_id)         (grlc_ul_task_sigs[gas_id] & GRLC_UL_ITN_SIG)
#define RLC_UL_IS_LOG_SIG_SET(gas_id)         (grlc_ul_task_sigs[gas_id] & GRLC_UL_LOG_SIG)

#define RLC_DL_IS_PL1_SIG_SET(gas_id)           (grlc_dl_task_sigs[gas_id] & GRLC_DL_PL1_SIG)
#define RLC_DL_IS_PL1_EPDAN_REQ_SIG_SET(gas_id) (grlc_dl_task_sigs[gas_id] & GRLC_DL_PL1_EPDAN_REQ_SIG)
#define RLC_DL_IS_MAC_SIG_SET(gas_id)           (grlc_dl_task_sigs[gas_id] & GRLC_DL_MAC_SIG)
#define RLC_DL_IS_TMR_SIG_SET(gas_id)           (grlc_dl_task_sigs[gas_id] & GRLC_DL_TMR_SIG)

#define RLC_DL_IS_LOG_SIG_SET(gas_id)           (grlc_dl_task_sigs[gas_id] & GRLC_DL_LOG_SIG)


/* Macros to set and clear specific RLC DL signal mask
*/
#define RLC_DL_SET_PL1_SIG(gas_id)            RLC_DL_SET_SIG(gas_id, GRLC_DL_PL1_SIG)
#define RLC_DL_CLR_PL1_SIG(gas_id)            RLC_DL_CLR_SIG(gas_id, GRLC_DL_PL1_SIG)

#define RLC_DL_SET_PL1_EPDAN_REQ_SIG(gas_id)  RLC_DL_SET_SIG(gas_id, GRLC_DL_PL1_EPDAN_REQ_SIG)
#define RLC_DL_CLR_PL1_EPDAN_REQ_SIG(gas_id)  RLC_DL_CLR_SIG(gas_id, GRLC_DL_PL1_EPDAN_REQ_SIG)

#define RLC_DL_SET_MAC_SIG(gas_id)            RLC_DL_SET_SIG(gas_id, GRLC_DL_MAC_SIG)
#define RLC_DL_CLR_MAC_SIG(gas_id)            RLC_DL_CLR_SIG(gas_id, GRLC_DL_MAC_SIG)

#define RLC_DL_SET_TMR_SIG(gas_id)            RLC_DL_SET_SIG(gas_id, GRLC_DL_TMR_SIG)
#define RLC_DL_CLR_TMR_SIG(gas_id)            RLC_DL_CLR_SIG(gas_id, GRLC_DL_TMR_SIG)

#define RLC_DL_SET_LOG_SIG(gas_id)            RLC_DL_SET_SIG(gas_id, GRLC_DL_LOG_SIG)
#define RLC_DL_CLR_LOG_SIG(gas_id)            RLC_DL_CLR_SIG(gas_id, GRLC_DL_LOG_SIG)


/* Macro returns the header type (1/2/3) given a valid MCS value in the range
** from MCS-1 to MCS-9 inclusive. See TS 04.60 10.3a.3/4.
** This macro does not expect the param to be MCS_5_7 or MCS_6_9
*/
#define GET_HEADER_TYPE(m) ( (m) >= RM_MCS_7 ? HDR_TYPE_1 :              \
                             (m) >= RM_MCS_5 ? HDR_TYPE_2 : HDR_TYPE_3   \
                           )

/* Macro to obtain the value of K for CV calculation as per TS 04.60 9.3.1
** K = 1 for MCS-1/2/3/4/5/6 and K = 2 for MCS-7/8/9
*/
#define GET_K(mcs)   (mcs >= RM_MCS_7 && mcs <= RM_MCS_9 ? 2 : 1)


/* This macro maps the virtual transmit array with value in the range of 0..2047
** to the physical transmit array of the range from 0..E_TX_WORKSPACE-1.
*/
#define TX_ARR(gas_id,ind)  ((*p_tx_arr[gas_id])[(ind)%E_TX_WORKSPACE])

/* This macro maps the virtual transmit array with value in the range of 0..127
** to the physical transmit array of the range from 0..RM_RLC_RX_WORKSPACE-1.
*/
#define RX_ARR(gas_id,ind)  ((*p_rx_arr[gas_id])[(ind)%RM_RLC_RX_WORKSPACE])

/* This macro maps the virtual transmit array with value in the range of 0..2047
** to the physical transmit array of the range from 0..E_RX_WORKSPACE-1.
*/
#define E_RX_ARR(gas_id, ind)  ((*p_rx_arr[gas_id])[(ind)%E_RX_WORKSPACE])

/*===========================================================================
===
===  MACRO         CHECK_RANGE()
===
===  DESCRIPTION
===
===    Macro to check a variable if it is out of a given range, and if so
===    print an error message and force the variable to a default, safe value
===
===  PARAMETERS
===
===    min: minimum value for the variable
===    max: maximum value for the variable
===    def: default value if to assign to the variable if it is out of range
===
===  DEPENDENCIES
===
===    None
===
===  RETURN VALUE
===
===    None
===
===  SIDE EFFECTS
===
===========================================================================*/
#define CHECK_RANGE(v,min,max,def)                                          \
    if ((v) < (min) || (v) > (max))                                         \
    {                                                                       \
      MSG_GRLC_ERROR_3(GRLC_F3_MSG_ENUM_0, "ERLU %u OOR MinMax: 0x%08x Force to %u",(v),               \
                (uint32)((uint32)(min)<<16) | (uint32)((uint32)(max) & 0xffff), (def)); \
      (v) = (def);                                                          \
    }

/*****************************************************************************
***
***     Public Functions
***
*****************************************************************************/

/*===========================================================================
===
===  FUNCTION      xxx
===
===  DESCRIPTION
===
===    xxx.
===
===  DEPENDENCIES
===
===    xxx.
===
===  RETURN VALUE
===
===    xxx.
===
===  SIDE EFFECTS
===
===    xxx.
===
===========================================================================*/


#endif /* INC_GRLCUTIL_H */

/*** EOF: don't remove! ***/
