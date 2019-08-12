/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                           M P I 

GENERAL DESCRIPTION


EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2004-2010 by Qualcomm Technologies, Inc. All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/src/wl1ssiconfigi.h#1 $
$DateTime: 2016/12/13 08:00:12 $
$Author: mplcsds1 $

when       who    what, where, why
--------   ---   ----------------------------------------------------------
11/14/13   pj     Wrapped HWIO access with WL1_HWIO macro
03/01/10   rvs    Add P4 info to header.
===========================================================================*/

#include "comdef.h"
#include "msm.h"
#include "l1msm.h"


/* =============================================================
    QDSP6FW_L2TCM_SIZE : 256kb (Si) / 512kb (Emu)
    QDSP6FW_L2TCM_BASE : 0xB1000000 (old 0xb0800000)
    QDSP6FW_L2TCM_END  : 0xB1040000 (old 0xb0840000) (Si)
 =============================================================*/



#define  TQA_MEM0_BASE_SA        0x40000000
#define  TQA_MEM0_SZ             0x3FFC      /*16kb*/
#define  TQA_MEM1_BASE_SA        0x40004000
#define  TQA_MEM1_SZ             0x3FFC      /*16kb*/
#define  TQA_MEM2_BASE_SA        0x40008000
#define  TQA_MEM2_SZ             0x3FFC      /*16kb*/

#define  TQA_TL_SIZE             0x1000
#define  TQA_TL_SIZE1            0x0100
#define  TQ_ARRAY                10

#define  TQ0_TL_SA               0x40000000
#define  TQ0_TL_SZ               TQA_TL_SIZE-4   /*4kb*/
#define  TQ1_TL_SA               0x40001000          
#define  TQ1_TL_SZ               0x8000-4 //TQA_TL_SIZE-4   /*4kb*/
#define  TQ2_TL_SA               0x40009000   //0x40002000          
#define  TQ2_TL_SZ               TQA_TL_SIZE1-4//TQA_TL_SIZE-4   /*4kb*/
#define  TQ3_TL_SA               0x40009100   //0x40003000          
#define  TQ3_TL_SZ               TQA_TL_SIZE1-4//TQA_TL_SIZE-4   /*4kb*/
#define  TQ4_TL_SA               0x40009200   //0x40004000          
#define  TQ4_TL_SZ               TQA_TL_SIZE1-4//TQA_TL_SIZE-4   /*4kb*/
#define  TQ5_TL_SA               0x40009300   //0x40005000          
#define  TQ5_TL_SZ               TQA_TL_SIZE1-4//TQA_TL_SIZE-4   /*4kb*/
#define  TQ6_TL_SA               0x40009400   //0x40006000          
#define  TQ6_TL_SZ               TQA_TL_SIZE1-4//TQA_TL_SIZE-4   /*4kb*/
#define  TQ7_TL_SA               0x40009500   //0x40007000          
#define  TQ7_TL_SZ               TQA_TL_SIZE1-4//TQA_TL_SIZE-4   /*4kb*/
#define  TQ8_TL_SA               0x40009600   //0x40008000          
#define  TQ8_TL_SZ               TQA_TL_SIZE1-4//TQA_TL_SIZE-4   /*4kb*/
#define  TQ9_TL_SA               0x40009700   //0x40009000          
#define  TQ9_TL_SZ               TQA_TL_SIZE1-4//TQA_TL_SIZE-4   /*4kb*/



#define  TQ0_DM_MIN              0x08002000  /* 0x40010000  */
#define  TQ0_DM_MAX              0x08002100  /* 0x40010800  */
#define  TQ0_RM_MIN              0x08002100  /* 0x40010800  */
#define  TQ0_RM_MAX              0x08002200  /* 0x40011000  */

#define  TQ1_DM_MIN              0x08002200  /* 0x40011000  */
#define  TQ1_DM_MAX              0x08002300  /* 0x40011800  */
#define  TQ1_RM_MIN              0x08002300  /* 0x40011800  */
#define  TQ1_RM_MAX              0x08002400  /* 0x40012000  */

#define  TQ2_DM_MIN              0x08002400  /* 0x40012000  */
#define  TQ2_DM_MAX              0x08002500  /* 0x40012800  */
#define  TQ2_RM_MIN              0x08002500  /* 0x40012800  */
#define  TQ2_RM_MAX              0x08002600  /* 0x40013000  */

#define  TQ3_DM_MIN              0x08002600  /* 0x40013000  */
#define  TQ3_DM_MAX              0x08002700  /* 0x40013800  */
#define  TQ3_RM_MIN              0x08002700  /* 0x40013800  */
#define  TQ3_RM_MAX              0x08002800  /* 0x40014000  */

#define  TQ4_DM_MIN              0x08002800  /* 0x40014000  */
#define  TQ4_DM_MAX              0x08002900  /* 0x40014800  */
#define  TQ4_RM_MIN              0x08002900  /* 0x40014800  */
#define  TQ4_RM_MAX              0x08002a00  /* 0x40015000  */


/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MACROS

===========================================================================*/
#define TQA_MEMn_CFG(n) \
    WL1_HWIO_OUT(TQA_MEM##n##_BASE_MIN,TQA_MEM##n##_BASE_SA); \
    WL1_HWIO_OUT(TQA_MEM##n##_BASE_MAX,TQA_MEM##n##_BASE_SA+TQA_MEM##n##_SZ);

#define TQn_TL_CFG(n) \
    WL1_HWIO_OUTI(TQn_TL_START_ADDR,n,TQ##n##_TL_SA); \
    WL1_HWIO_OUTI(TQn_TL_END_ADDR,n,TQ##n##_TL_SA+TQ##n##_TL_SZ);

/*#define TQn_TL_CFG(n) \
    HWIO_OUTI(TQn_TL_START_ADDR,n,HWIO_TQn_TL_START_ADDR_ADDR(n)); \
    HWIO_OUTI(TQn_TL_END_ADDR,n,HWIO_TQn_TL_END_ADDR_ADDR(n));*/


#define TQn_START_ADDR(n)  (TQ##n##_TL_SA)

#define TQn_INT(n)  (TASK_QUEUE_##n##_IRQ)


/* total 46 pe's */
typedef enum 
{
    TASK_CTL = 0,  /*00d  0x00*/
    O_DEINT_CCH,   /*01d  0x01*/
    O_DEINT_SCH,   /*02d  0x02*/
    O_DMP_DEINT,   /*03d  0x03*/
    O_TM,          /*04d  0x04*/
    O_TX_ENC,      /*05d  0x05*/
    O_TX_MOD,      /*06d  0x06*/
    O_TX_WMORE,    /*07d  0x07*/
    O_DEC_SCH,     /*08d  0x08*/
    O_DEC_CCH,     /*09d  0x09*/
    TC,            /*10d  0x0a*/
    QICE,          /*11d  0x0b*/
    RFD0,          /*12d  0x0c*/
    RFD1,          /*13d  0x0d*/
    QICE_DC,       /*14d  0x0e*/
    CME,           /*15d  0x0f*/
    CCC,           /*16d  0x10*/
    PEI,           /*17d  0x11*/
    HBBR0_BR,      /*18d  0x12*/
    SRCH_BR,       /*19d  0x13*/
    DM_BR,         /*20d  0x14*/
    PTM,           /*21d  0x15*/
    NT_EST,        /*22d  0x16*/
    MEM_POOL,      /*23d  0x17*/
    DMAP_BR,       /*24d  0x18*/
    HBNS_BR,       /*25d  0x19*/
    NEST_BR,       /*26d  0x1a*/
    SSI_BR,        /*27d  0x1b*/
    EXT_RD_0,      /*28d  0x1c*/
    EXT_RD_1,      /*29d  0x1d*/
    EXT_WR_0,      /*30d  0x1e*/
    EXT_WR_1,      /*31d  0x1f*/
    AHBI,          /*32d  0x20*/
    FFTE_A,        /*33d  0x21*/
    FFTE_B,        /*34d  0x22*/
    GE_CORR,       /*35d  0x23*/
    NCAS,          /*36d  0x24*/
    TONE_SPLIT,    /*37d  0x25*/
    UMTS_SRCH,     /*38d  0x26*/
    CME_DC,        /*39d  0x27*/
    CCC_DC,        /*40d  0x28*/
    W_DBACK_NONHS, /*41d  0x29*/
    W_DBACK_HS,    /*42d  0x2a*/
    HKP0,          /*43d  0x2b*/
    HKP1,          /*44d  0x2c*/
    HKP2,          /*45d  0x2d*/
    
    CRA_NUM_PEs,
} MPIResourceType;


typedef enum 
{
    RESERVE = 0,
    FREE,
    NOP,
    WAIT_EVT,
    SET_EVT,
    CLR_EVT,
    SKIP_IF_TRUE,
    SKIP_IF_FALSE,
    SKIP_MULTIPLE,
    WAIT_TMR,
    ASSERT_TIM,
    SYNC,

    TASK_CTL_NUM_IDs,
} TaskCTLType;


typedef enum 
{
    RESET = 0,
    CONFIG,
    REGISTER_DUMP,
    REGISTER_WRITE,

    TASK_LIST_NUM_IDs,
} TaskListType;


typedef struct
{
   dword         src_addr;  /*src addr    */
   dword         t_addr;  /*task addr   */
   dword         d_addr;  /*data addr   */
   dword         c_leng;  /*cmd length  */
   dword         d_leng;  /*data length */
} mpi_module_interface_type;


typedef struct
{
   dword         sadr;    /*start addr    */
   dword         eadr;    /*end addr      */
   dword         badr;    /*base addr     */
} mpi_tqa_qaddr_type;


typedef struct
{
   dword     ntq;             /*no of queues  */
   dword     qport[TQ_ARRAY]; /*tqa port      */
} mpi_tqa_qparam_type;




/*===========================================================================

FUNCTION    mpi_hw_init

DESCRIPTION
    micro isr

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void mpi_hw_init (void);
