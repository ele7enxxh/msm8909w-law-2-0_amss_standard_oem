/**
  @file bamcore_debug.c
  @brief
  This file contains the implementation of the debug interfaces 
  that provide BAM core dump. 

*/
/*
===============================================================================

                             Edit History

 $Header: 

when       who     what, where, why
--------   ---     ------------------------------------------------------------
09/03/14   SS      Created. 

===============================================================================
                   Copyright (c) 2014 QQUALCOMM Technologies Inc.
                          All Rights Reserved.
                        Qualcomm Confidential and Proprietary.
===============================================================================
*/
#include "bamhwio.h"
#include "bamcore.h"
#include "bamcore_debug.h"
#include "string.h"

#define BAM_TESTBUS_MAX_INDEX          0x23
#define BAM_CORE_DUMP_MINOR_REV        0x02
#define BAM_CORE_DUMP_MAJOR_REV        0x0100
#define BAM_CORE_DUMP_VER              0x010000
#define BAM_CORE_DUMP_REV              ((BAM_CORE_DUMP_VER)| \
                                       (BAM_CORE_DUMP_MAJOR_REV)|(BAM_CORE_DUMP_MINOR_REV))
                                     
/**
 * Bam Register structure for core dump
 */
typedef struct
{
    uint32 bam_dumptype;                         /**< -- Dump type, BAM or pipe. */
    bam_paddr bam_base_pa;                       /**< -- BAM physical base address */
    uint32 bam_ctrl;                             /**< -- BAM control regsiter value */
    uint32 bam_revision;                         /**< -- BAM revision regsiter value */
    uint32 bam_desc_cnt_trshld;                  /**< -- BAM descriptor count threshold regsiter value*/
    uint32 bam_irq_stts;                         /**< -- BAM interrupt status regsiter value */
    uint32 bam_irq_clr;                          /**< -- BAM interrupt clear  regsiter value*/
    uint32 bam_irq_en;                           /**< -- BAM interrupt enable regsiter value*/
    uint32 bam_ahb_master_err_ctrls;             /**< -- BAM AHB data error cntl regsiter value*/
    uint32 bam_ahb_master_err_addr;              /**< -- BAM AHB data error add regsiter value*/
    uint32 bam_ahb_master_err_data;              /**< -- BAM AHB data error regsiter value */
    uint32 bam_num_pipes;                        /**< -- BAM pipe count regsiter value*/
    uint32 bam_cnfg_bits;                        /**< -- BAM config regsiter */
    uint32 bam_pipe_attr;                        /**< -- BAM pipe attributes register */
    uint32 bam_sw_version;                       /**< -- BAM software version regsiter */
    uint32 bam_irq_srcs_een;                     /**< -- BAM interrupt source enable regsiter */
    uint32 bam_irq_srcs_msk_een;                 /**< -- BAM interrupt source enable mask regsiter */
    uint32 bam_irq_srcs_unmasked_een;            /**< -- BAM interrupt source unmask enb regsiter */
    uint32 bam_test_bus_sel;                     /**< -- BAM test bus selector regsiter */
    uint32 bam_test_bus_reg[BAM_TESTBUS_MAX_INDEX];/**< -- BAM test bus regsiter */
}bamdebug_registers;

/**
 *  Pipe register stucture for core dump
 */
typedef struct  bamdebug_p_register bamdebug_p_register;
struct  bamdebug_p_register
{
    uint32 bam_p_dumptype;
    uint32 bam_p_index;
    uint32 bam_p_ctrln;
    uint32 bam_p_irq_sttsn;
    uint32 bam_p_irq_enn;
    uint32 bam_p_prdcr_sdbndn;
    uint32 bam_p_cnsmr_sdbndn;
    uint32 bam_p_evnt_dest_addrn;
    uint32 bam_p_evnt_regn;
    uint32 bam_p_sw_ofstsn;
    uint32 bam_p_data_fifo_addrn;
    uint32 bam_p_desc_fifo_addrn;
    uint32 bam_p_evnt_gen_trshldn;
    uint32 bam_p_fifo_sizesn;
    uint32 bam_p_retr_cntxtn;
    uint32 bam_p_si_cntxtn;
    uint32 bam_p_df_cntxtn;
    uint32 bam_p_au_psm_cntxt_1n;
    uint32 bam_p_psm_cntxt_2n;
    uint32 bam_p_psm_cntxt_3n;
    uint32 bam_p_psm_cntxt_4n;
    uint32 bam_p_psm_cntxt_5n;
    uint32 *bam_p_desc_fifo;
    uint32 *bam_p_data_fifo;
    uint32  bam_p_desc_fifo_size;
    uint32 bam_p_data_fifo_size;
    bamdebug_registers *bam_ptr;
    bamdebug_p_register *bam_p_peer_ptr;  
};

/**
 *  BAM dump header stucture for core dump revision, pipe and peep pipe mask.
 *
 *  Header will have revision no., Pipe mask, peerpipe mask & size of header, bam , pipe reg dump.
 *  Revision is determine by following:
 *  LSB 8 bits: Minor rev. Changes as per SW changes
 *  bits 8-15:  Major rev.
 *  bits 16-23: Intial version.
 *  bits 24-31: reserved.
 *
 */
typedef struct
{
    uint32 revision;
    uint32 pipemask;
    uint32 peerpipemask;
    uint32 dump_header_size;
    uint32 bam_buf_size;
    uint32 pipe_buf_size;
}bam_dumpheader; 
                                        
                       
                   
/* ============================================================================
**  Function : bamcore_getpiperegdump
** ============================================================================
*/
/**
  Gets the  BAM Pipe Core register value.

  Gets the BAM Pipe Core register value.

  @param[in] base       BAM virtual base address
  @param[in] base_pa    Bam base PA.
  @param[in] bufptr     Buffer to get Bam Pipe register dump.
  @param[in] pipenum    Pipe Number to be dumped.
  @param[in] buf_base   Pointer start of bam core register dump.

  @return
  None
*/
static void  bamcore_getpiperegs(bam_vaddr base, bam_paddr base_pa, uint32 *bufptr, uint32 pipenum, bam_vaddr buf_base);
                                        
/* ============================================================================
**  Function : bamcore_testbusreg_dump
** ============================================================================
*/
/**
  Gets the dump of test bus reg of BAM device.
  Gets the dump of test bus reg of BAM device.
 
  @param[in]  base    BAM virtual base address.
  @param[in]  buf     Buffer to log the bam test bus reg.
 
  @return
  NOne
 
*/
static void bamcore_testbusreg_dump(bam_vaddr base, bamdebug_registers *buf);                                        

/* ============================================================================
**  Function : bamcore_getctrlregbufferinfo
** ============================================================================
*/
/**
   Gets the BAM control reg dump buffer size.

   Gets the BAM control reg dump buffer size.

   @return
   buffer size required for bam reg.
 */
uint32 bamcore_getctrlregbufferinfo(void)
{
    return(sizeof(bamdebug_registers));
}

/* ============================================================================
**  Function : bamcore_getpiperegbufferinfo
** ============================================================================
*/
/**
   Gets the Pipe register dump buffer size.

   Gets the Pipe register dump buffer size.

   @return
   buffer size requied for single pipe.
  */
uint32 bamcore_getpiperegbufferinfo(void)
{
    return(sizeof(bamdebug_p_register));
}

/* ============================================================================
**  Function : bamcore_getheaderbufferinfo
** ============================================================================
*/
/**
   Gets the Header dump buffer size.

   Gets the Header dump buffer size.

   @return
   buffer size requied for bam core dump header.
  */
uint32 bamcore_getheaderbufferinfo(void)
{
    return(sizeof(bam_dumpheader));
}

/* ============================================================================
**  Function : bamcore_testbusreg_dump
** ============================================================================
*/
/**
  Gets the dump of test bus reg of BAM device.
  Gets the dump of test bus reg of BAM device.
 
  @param[in]  base    BAM virtual base address.
  @param[in]  buf     Buffer to log the bam test bus reg.
 
  @return
  NOne
 
*/
static void bamcore_testbusreg_dump(bam_vaddr base, bamdebug_registers *buf)
{
    uint32 offset_val;
    for(offset_val = 0; offset_val < BAM_TESTBUS_MAX_INDEX; offset_val++)
    {
        bamdev_write_reg_field(base, BAM_TEST_BUS_SEL, BAM_MNS(BAM_TEST_BUS_SEL,BAM_TESTBUS_SEL), offset_val);
        buf->bam_test_bus_reg[offset_val] = bamdev_read_reg(base, BAM_TEST_BUS_REG);
    }
    buf->bam_test_bus_sel = bamdev_read_reg(base, BAM_TEST_BUS_SEL);
    return;
}

/* ============================================================================
**  Function : bamcore_headerdump
** ============================================================================
*/
/**
  Log the  revision, mask of pipe, peerpipe and size of pipe, peerpipe and header in dump buffer header.

  Log the  revision, mask of pipe,Peerpipe and size of pipe, peerpipe and header in dump buffer header.
  
  @param[in] buf          Buffer to log the header.
  @param[in] pipemask     Pipe mask
  @param[in] peerpmask    Peer pipe mask.

  @return
  None
 */
void bamcore_headerdump(uint32 *buf, uint32 pipemask, uint32 peerpmask)
{
    bam_dumpheader *ptr;
    if(NULL != buf)
    {
        ptr = (bam_dumpheader *)buf;
        ptr->revision = BAM_CORE_DUMP_REV;
        ptr->pipemask = pipemask;
        ptr->peerpipemask = peerpmask;
        ptr->dump_header_size = bamcore_getheaderbufferinfo();
        ptr->bam_buf_size = bamcore_getctrlregbufferinfo();
        ptr->pipe_buf_size = bamcore_getpiperegbufferinfo();
    }
    return;
}

/* ============================================================================
**  Function : bamcore_bamregdump
** ============================================================================
*/
/**
  Gets the Dump of BAM Core register.

  Gets the Dump of BAM Core register.

  @param[in] base     BAM virtual base address
  @param[in] buf      Buffer to store the register dump.
  @param[in] base_pa  Bam pa.
  @param[in] ee       Bam EE.

  @return
  None
 */
/* @ Brief about Dump buffer strucure.
 * ========================
 * @Header: Header will have revision no.(0x00010000),Pipe mask, peerpipe mask & size of header, bam , pipe reg dump.
 *                 More details check the header structure documentation.
 *                
 *=========================================================================
 * @BAM Dump: It will dump first Top level BAM.If peerpipe info is required in BAM2BAM mode,we will dump all peer BAM.
 * @Dump buffer structure looks like:
 *               Header-->Top BAM--->PeerBAM-->peerBAM...... goes on
 *  if peerpipe info in header is null, No peerBAM dump will be taken.
 *=========================================================================
 * @Pipe Dump:Pipe dump will be taken after Header, Bam and peerbam(if peerpipe info is not null).
 * @  In B2B mode, both pipe info will be logged on one after another.In system mode it will be                
 *     logged on as sequence of pipe mask.
 *                    
 * DUMP Buffer looks like:
 *   Header-->BAM-->Peer BAM(can be multiple peer BAM)-->Pipe-->peerpipe-->pipe-->pipe so on
 *  (can be multiple entries).
 *
 *              =========
 *              | Revision     |------> rev.
 *              =========
 *              |   Pipe       |------>  Pipe mask.
 *              =========                    
 *              |Peer pipe     |------> maskof peer pipe
 *              =========
 *              |header size   |------> size of header struct
 *              =========
 *              |BAM size      |------> Size of bam core reg. dump
 *              =========
 *              |Pipe size     |------> size of bam pipe reg. dump
 *              =========
 *              | Top BAM      |------> After Header Top BAM Always.Only one top BAM.
 *              =========
 *              |Peer BAM      |------> Only in BAM2BAM Mode & peerpipe mask != NULL. Can be multiple entries.
 *              =========
 *              |  Pipe        |--- ---> Can be Multiple entries.
 *              =========
 *              | Peer Pipe    |------> Only in BAM2 BAMmode & Peerpipe mask != Null in header.
 *              =========
 *
 */
void bamcore_bamregdump(bam_vaddr base, uint32 *buf, bam_paddr base_pa, uint32 ee)
{
    uint32 *temp = buf + (bamcore_getheaderbufferinfo()/sizeof(uint32 *));
    bamdebug_registers *bambuf = (bamdebug_registers *)temp;
    while(BAM_DUMP_TOP == bambuf->bam_dumptype)
    {
        if(base_pa  != bambuf->bam_base_pa)
        {
            bambuf++;
        }
        else
        {
            return; /* bam dump is already logged so return */
        }
    }
     
    if(0 != base)
    {
        /* Bam registers dump */
        bambuf->bam_dumptype = BAM_DUMP_TOP;
        bambuf->bam_base_pa = base_pa;
        bambuf->bam_ctrl = bamdev_read_reg(base, BAM_CTRL);
        bambuf->bam_revision = bamdev_read_reg(base, BAM_REVISION);
        bambuf->bam_desc_cnt_trshld = bamdev_read_reg(base,BAM_DESC_CNT_TRSHLD);
        bambuf->bam_irq_stts = bamdev_read_reg(base,BAM_IRQ_STTS);
        bambuf->bam_irq_clr = bamdev_read_reg(base,BAM_IRQ_CLR);
        bambuf->bam_irq_en = bamdev_read_reg(base,BAM_IRQ_EN);
        bambuf->bam_ahb_master_err_ctrls = bamdev_read_reg(base,BAM_AHB_MASTER_ERR_CTRLS);
        bambuf->bam_ahb_master_err_addr = bamdev_read_reg(base,BAM_AHB_MASTER_ERR_ADDR);
        bambuf->bam_ahb_master_err_data = bamdev_read_reg(base,BAM_AHB_MASTER_ERR_DATA);
        bambuf->bam_num_pipes = bamdev_read_reg(base, BAM_NUM_PIPES);
        bambuf->bam_cnfg_bits = bamdev_read_reg(base, BAM_CNFG_BITS);
        bambuf->bam_sw_version = bamdev_read_reg(base, BAM_SW_VERSION);
        /* BAM test bus reg dump */
        bamcore_testbusreg_dump(base, bambuf);
        /* Acess only for Top level BAM dump */
        if(ee != BAM_MAX_EES)
        {
            bambuf->bam_pipe_attr = bamdev_read_reg(base, BAM_PIPE_ATTR_EEn (ee));
            bambuf->bam_irq_srcs_een = bamdev_read_reg(base,BAM_IRQ_SRCS_EEn (ee));
            bambuf->bam_irq_srcs_msk_een = bamdev_read_reg(base,BAM_IRQ_SRCS_MSK_EEn(ee));
            bambuf->bam_irq_srcs_unmasked_een = bamdev_read_reg(base,BAM_IRQ_SRCS_UNMASKED_EEn(ee));
        }
    }
    return;
}

/* ============================================================================
**  Function : bamcore_getpiperegdump
** ============================================================================
*/
/**
  Gets the  BAM Pipe Core register value.

  Gets the BAM Pipe Core register value.

  @param[in] base       BAM virtual base address
  @param[in] base_pa    Bam base PA.
  @param[in] bufptr     Buffer to get Bam Pipe register dump.
  @param[in] pipenum    Pipe Number to be dumped.
  @param[in] buf_base   Pointer start of bam core register dump.

  @return
  None
 */
static void  bamcore_getpiperegs(bam_vaddr base, bam_paddr base_pa, uint32 *bufptr, uint32 pipenum, bam_vaddr buf_base)
{
    bamdebug_registers *ptr = (bamdebug_registers*)buf_base;
    bamdebug_p_register *buf = (bamdebug_p_register*)bufptr;
    if(0!= base)
    {
        buf->bam_p_dumptype = BAM_DUMP_PIPE;
        /* Get the bam core register dump start pointer */
        while(ptr->bam_dumptype == BAM_DUMP_TOP)
         {
             if(ptr->bam_base_pa == base_pa)
             {
                 buf->bam_ptr = (bamdebug_registers*)ptr;
                 break;
             }
             ptr++;
         }
  
         buf->bam_p_index = pipenum;
         buf->bam_p_ctrln = bamdev_read_reg(base, BAM_P_CTRLn(pipenum));
         buf->bam_p_irq_sttsn = bamdev_read_reg(base, BAM_P_IRQ_STTSn(pipenum));
         buf->bam_p_irq_enn = bamdev_read_reg(base, BAM_P_IRQ_ENn(pipenum));
         buf->bam_p_prdcr_sdbndn = bamdev_read_reg(base, BAM_P_PRDCR_SDBNDn(pipenum));
         buf->bam_p_cnsmr_sdbndn = bamdev_read_reg(base, BAM_P_CNSMR_SDBNDn(pipenum));
         buf->bam_p_evnt_dest_addrn = bamdev_read_reg(base, BAM_P_EVNT_DEST_ADDRn(pipenum));
         buf->bam_p_evnt_regn = bamdev_read_reg(base, BAM_P_EVNT_REGn(pipenum));
         buf->bam_p_sw_ofstsn = bamdev_read_reg(base, BAM_P_SW_OFSTSn(pipenum));
         buf->bam_p_data_fifo_addrn = bamdev_read_reg(base, BAM_P_DATA_FIFO_ADDRn(pipenum));
         buf->bam_p_desc_fifo_addrn = bamdev_read_reg(base, BAM_P_DESC_FIFO_ADDRn(pipenum));
         buf->bam_p_evnt_gen_trshldn = bamdev_read_reg(base, BAM_P_EVNT_GEN_TRSHLDn(pipenum));
         buf->bam_p_fifo_sizesn = bamdev_read_reg(base, BAM_P_FIFO_SIZESn(pipenum));
         buf->bam_p_retr_cntxtn = bamdev_read_reg(base, BAM_P_RETR_CNTXT_n(pipenum));
         buf->bam_p_si_cntxtn = bamdev_read_reg(base, BAM_P_SI_CNTXT_n(pipenum));
         buf->bam_p_df_cntxtn = bamdev_read_reg(base, BAM_P_DF_CNTXT_n(pipenum));
         buf->bam_p_au_psm_cntxt_1n = bamdev_read_reg(base, BAM_P_AU_PSM_CNTXT_1_n(pipenum));
         buf->bam_p_psm_cntxt_2n = bamdev_read_reg(base, BAM_P_PSM_CNTXT_2_n(pipenum));
         buf->bam_p_psm_cntxt_3n = bamdev_read_reg(base, BAM_P_PSM_CNTXT_3_n(pipenum));
         buf->bam_p_psm_cntxt_4n = bamdev_read_reg(base, BAM_P_PSM_CNTXT_4_n(pipenum));
         buf->bam_p_psm_cntxt_5n = bamdev_read_reg(base, BAM_P_PSM_CNTXT_5_n(pipenum));
    }
    else 
    {
         buf->bam_p_dumptype = BAM_DUMP_PIPE;
         buf->bam_p_index = pipenum;
         buf->bam_ptr = 0;
    }
    return;
}

/* ============================================================================
**  Function : bamcore_piperegdump
** ============================================================================
*/
/**
  Gets the Dump of BAM Pipe Core register.

  Gets the Dump of BAM Pipe Core register.

  @param[in] base       BAM virtual base address
  @param[in] base_pa    Bam base PA.
  @param[in] bufptr     Buffer to get Bam Pipe register dump.
  @param[in] pipedump   Pointer to peer pipe ,desc & data fifo core dump structure

  @return
  None
 */
void bamcore_piperegdump(bam_vaddr base, bam_paddr base_pa, uint32 *bufptr, bamdebug_pipe_config *pipedump)
{
    bam_vaddr buf_base = (bam_vaddr)(bufptr + (bamcore_getheaderbufferinfo()/sizeof(uint32 *)));
    uint32 *temp = bufptr + ((bamcore_getheaderbufferinfo()+ bamcore_getctrlregbufferinfo()) /sizeof(uint32 *));
    bamdebug_p_register *pipebuf = NULL;
    bamdebug_registers *ptr = (bamdebug_registers*)temp;
    /* find out end of bam reg dump */
    while(BAM_DUMP_TOP == ptr->bam_dumptype)
    {
        ptr++;
    }
     
    pipebuf=(bamdebug_p_register *)ptr;
    /* check if it first pipe logging or not */ 
    while(BAM_DUMP_PIPE == pipebuf->bam_p_dumptype)
    {
        pipebuf++;
    }
    temp=(uint32 *)pipebuf;
    /* Get the pipe dump */
    bamcore_getpiperegs(base, base_pa, temp, pipedump->pipenum, buf_base);
    
    if(pipedump->peerpipenum != 0xffffffff)
    { 
#ifdef FEATURE_BAM_BAM2BAM
        /* update the Descriptor & data fifo buffer pointer */ 
        pipebuf->bam_p_desc_fifo =  pipedump->desc_fifo;
        pipebuf->bam_p_desc_fifo_size = pipedump->desc_fifo_size;
        pipebuf->bam_p_data_fifo =  pipedump->data_fifo;
        pipebuf->bam_p_data_fifo_size = pipedump->data_fifo_size;
       
        /* copy desc & data fifo */
        if((NULL != pipedump->data_fifo) && (0 != pipedump->data_base_va))
        {
            bam_osal_memcpy(pipedump->data_fifo, (uint32 *)pipedump->data_base_va, pipedump->data_fifo_size);
        }
        if((NULL != pipedump->desc_fifo) && (0 != pipedump->desc_base_va))
        {
            bam_osal_memcpy(pipedump->desc_fifo, (uint32 *)pipedump->desc_base_va, pipedump->desc_fifo_size);
        }
        temp += (bamcore_getpiperegbufferinfo() /sizeof(uint32 *));
        /* Update the peer pipe pointer */
        pipebuf->bam_p_peer_ptr = (bamdebug_p_register *)temp;   
        /* Get Peer pipe core dump */
        bamcore_getpiperegs(pipedump->peerbase_va, pipedump->peerbase_pa, temp, pipedump->peerpipenum, buf_base);
        /* update the peer pipe pointer to peer pipe */
        temp =(uint32 *)pipebuf;
        (++pipebuf)->bam_p_peer_ptr = (bamdebug_p_register*)temp;

        /* update the Desc and Data FIFO info in peer pipe side as well */
        pipebuf->bam_p_desc_fifo =  pipedump->desc_fifo;
        pipebuf->bam_p_desc_fifo_size = pipedump->desc_fifo_size;
        pipebuf->bam_p_data_fifo =  pipedump->data_fifo;
        pipebuf->bam_p_data_fifo_size = pipedump->data_fifo_size;
#endif
    }
    else
    {
        /* Peer pipe info, Desc and Data FIFO info of client provided buffer will be NULL in system mode */
        pipebuf->bam_p_peer_ptr = NULL;
        pipebuf->bam_p_desc_fifo =  NULL;
        pipebuf->bam_p_desc_fifo_size = 0;
        pipebuf->bam_p_data_fifo =  NULL;
        pipebuf->bam_p_data_fifo_size = 0;
    }
    return;
}

