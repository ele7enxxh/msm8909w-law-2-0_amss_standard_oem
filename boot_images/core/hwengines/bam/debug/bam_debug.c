/**
  @file bam_debug.c
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
09/03/14   SS      Added optimized BAM core dump API to support in low footprint image.
05/08/13   SS      Fix for 16 bytes desc buffer corruption in back to back core dump
02/28/13   SS      Added Peer BAM PA to VA mapping for Core dump
02/13/13   SS      Created 

===============================================================================
                   Copyright (c) 2013-2014 QUALCOMM Technologies Inc.
                          All Rights Reserved.
                        Qualcomm Confidential and Proprietary.
===============================================================================
*/

#include "bam.h"
#include "bami.h"
#include "bamtgtcfg.h"
#include "bamcore_debug.h"
#include "bamosal.h"
#include "string.h"

    
/* Globals common to the driver */
extern bam_dev **bams;
uint32 *bam_buf = NULL;

/**
 * @brief BAM pipe info
 *
 * This API will provide bam pipe numbers.
 *
 * @param nums_pipes    Bam Pipe supported
 * @param pipemask	   Pipemask for pipe info.
 *
 * @return              BAM Pipe count.
 */
static uint32 bam_get_pipecount(uint32 num_pipes, uint32 pipemask);

/**
 * @brief Peer bam instance & VA info
 *
 * This API will provide peer bam instance for requested pipe numbers.
 *
 * @param bam           Pointer to Bam device.
 * @param pmask         Bam Pipemask. 
 * @param peerpmask     pipe mask for which peer need to dump.
 * @param pipe_nums     Supported pipe numbers.
 * @param pipes         Holds Bam Peer Va & Pa.
 *
 * @return              Peer bam instance count.
 *
 */
#ifdef FEATURE_BAM_BAM2BAM
static uint32  bam_get_peerbamscount(bam_dev *bam, uint32 pmask, uint32 peerpmask, uint32 pipe_nums, bam_peer_info *pipes);


/**
 * @brief Bam get info
 *
 * This API will provide BAM, pipe & Descriptor buffer size info.
 *
 * @param bamhandle      BAM handle
 * @param info           Get bam dump buffer size info.
 *
 *
 */
bam_status_type bam_get_info(bam_handle bamhandle, bam_info_type *info)
{
    return BAM_FAILED;
}

/**
 * @brief Peer bam instance & VA info
 *
 * This API will provide peer bam instance for requested pipe numbers.
 *
 * @param bam          Bam handle
 * @param pmask        Pipemask. 
 * @param peerpmask    pipe mask for which peer need to dump. 
 * @param pipe_nums    Supported pipe numbers.
 * @param pipes        Holds Bam Peer Va & Pa.
 *
 * @return             Nums of peer bam instance.
 *
 */
static uint32 bam_get_peerbamscount(bam_dev *bam, uint32 pmask, uint32 peerpmask, uint32 pipe_nums, bam_peer_info *pipe_base)
{
    return 0;
}
#endif

/**
 * @brief bam_get_coredumpsize
 *
 * This API will provide required buffer size to dump bam and 
 * bampipe register. 
 *
 * @param bamcount      No of Bam instance.Sys mode = 1,B2B = 2.
 * @param pipecount	   Total no of bam pipe to be dumped.
 *
 * @return              Size of buffer.
 */
static uint32 bam_get_coredumpsize(uint32 bamcount, uint32 pipecount)
{
    uint32 bufsize = 0, totalpipecnt = pipecount;
    bufsize = bamcore_getheaderbufferinfo();
    bufsize += (bamcount * bamcore_getctrlregbufferinfo());
    bufsize += (totalpipecnt * bamcore_getpiperegbufferinfo());
    return bufsize;
}

/**
 * @brief bamdump_init
 *
 * This API will intilaize and allocate buffer for bam register 
 * dump. 
 *
 * @param bamhandle     Bam Handle.
 * @param pipecount	   Total no of bam pipe to be dumped. 
 * @param bamcnt        No.of Bam instance.Sys mode = 1,B2B = 2
 *
 * @return              Success/Failure. 
 *  
 * @note  Make sure BAM is intialized before calling to this 
 *        API.bamdump_init() should be called to init core dump
 *        when ever bam_deinit() is called.
 *  
 */
bam_status_type bamdump_init(bam_handle bamhandle, uint32 pipecount, uint32 bamcnt)
{
    bam_dev *bam = NULL;
    bam_status_type status = BAM_SUCCESS;
    uint32 bufsize = 0;

    if(((bam_handle)NULL==bamhandle) || (NULL == pipecount))
    {
        status = BAM_FAILED;
    }
    else
    {
        bam = BAM_GET_DEV_FROM_HANDLE(bamhandle);
        bufsize = bam_get_coredumpsize(bamcnt, pipecount);
        bam->bamdumpbuf = bam_osal_malloc(bufsize);
        bam->bufsize = bufsize;
    }
    return status;
}

/**
 * @brief BAM pipe info
 *
 * This API will provide bam pipe numbers.
 *
 * @param num_pipes   Number of supported BAM Pipe
 * @param pipemask	   pipemask for pipe info.
 *
 * @return             BAM Pipe count.
 */
static uint32 bam_get_pipecount(uint32 num_pipes, uint32 pipemask)
{
    uint32 pipecount = 0;
    /* clear out unused bit */
    pipemask = pipemask&~(~0 << num_pipes);
    for (;pipemask; pipemask &= pipemask-1, pipecount++);
    return pipecount;
}

/**
 * @brief bam_get_coredump
 *
 * This function get the Bam & pipe register dump after error condition.
 *
 * @param bamhandle     Pointer to client's handle to BAM.
 * @param pipemask      Mask of BAM pipe to be dumped. 
 * @param options       For future use. 
 *                     
 *
 * @return 				   Success/Failure.
 *
 *@Note: API bamdump_init()should be called before calling 
 *  bam_get_coredump API. 
 */
bam_status_type bam_get_coredump(bam_handle bamhandle, uint32 pipemask, uint32 options)
{
    bam_dev *bam = NULL;
    const bam_target_config_type* tgt_cfg;
    bam_caps_type bam_caps;
    bamdebug_pipe_config  pipedump = {0};
    uint32  bufsize = 0, mask = 1;
    static uint32 pipecnt = 0, pipeindex = 0;

    if(((bam_handle)NULL == bamhandle) || (0 == pipemask))
    {
         bam_osal_debug(MSG_ERR,"Invalid user input  bamhandle:0x%X, pmask:0x%X\n", bamhandle, pipemask);
         return BAM_FAILED;
    }
    bam = BAM_GET_DEV_FROM_HANDLE(bamhandle);
    /* check if BAM is enabled */
    if(BAM_SUCCESS != bamcore_check(bam->bam_cfg.bam_va, &bam_caps, bam->bam_tgt_cfg->ee)) 
    {
        bam_osal_debug(MSG_ERR, "bamcore_check: BAM is not enable\n");
        return BAM_FAILED;
    }
    pipecnt = bam_get_pipecount(bam_caps.num_pipes, pipemask);
    bufsize = bam_get_coredumpsize(0x1, pipecnt);
    if(bufsize > bam->bufsize)
    {
        bam_osal_debug(MSG_ERR, "bamcore_buf: core dump Buf size dont match init buf size\n");
        return BAM_FAILED;
    }
	if(NULL == bam->bamdumpbuf)
	{
	    bam_osal_debug(MSG_ERR, "core dump init not done, Null core dump buffer: buf:0x%X\n", bam->bamdumpbuf);
        return BAM_FAILED;
	}
    bam_buf = bam->bamdumpbuf;
    memset(bam_buf, 0, bam->bufsize);
    /* log the BAM header */
    bamcore_headerdump(bam_buf, pipemask, 0x0);
    /* Get EE info */
    tgt_cfg = bam_tgt_getcfg(bam->bam_cfg.bam_pa);
    if(tgt_cfg == NULL)
    {
        bam_osal_debug(MSG_ERR,"bam_tgt_getcfg has returned NULL  tgt_cfg:0x%X, user_buffer:0x%X\n", tgt_cfg, bam->bam_cfg.bam_pa);
        return BAM_FAILED;
    }
     /* get Top level BAM dump	 */ 
    bamcore_bamregdump(bam->bam_cfg.bam_va, bam_buf, bam->bam_cfg.bam_pa, tgt_cfg->ee);
    while(pipecnt != 0)
    {
        if ((pipemask & (mask << pipeindex)) != 0x0)
        {
            pipedump.pipenum = pipeindex;
            pipedump.peerpipenum = 0xffffffff;
            bamcore_piperegdump(bam->bam_cfg.bam_va, bam->bam_cfg.bam_pa, bam_buf, &pipedump);
            pipecnt--;
        }
        pipeindex += 1;
    }
    return BAM_SUCCESS;
}

