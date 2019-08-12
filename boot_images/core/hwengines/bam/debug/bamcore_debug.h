#ifndef BAMCORE_DEBUG_H_ 
#define BAMCORE_DEBUG_H_

/**
  @file bamcore_debug.h
  @brief
  This file contains definitions of constants, data structures, and
  interfaces to support dumping of BAM and BAM pipe register.

*/
/*
===============================================================================

                             Edit History

 $Header: 

when       who     what, where, why
--------   ---     ------------------------------------------------------------
09/03/14   SS      Created
===============================================================================
                   Copyright (c) 2014 QUALCOMM Technologies Inc.
                          All Rights Reserved.
                        Qualcomm Confidential and Proprietary.
===============================================================================
*/


#include "DALStdDef.h"
#include "bam.h"
#include "bamtgtcfg.h"


/**@ Peer pipe and descriptor and data fifo info structure. 
 *
 */    
typedef struct 
{  
    uint32 pipenum;                    /**<-- Pipe index */
    uint32 peerpipenum;                /**<-- Peer pipe index  */
    bam_vaddr peerbase_va;             /**<-- Peer BAM virtual base address */ 
    bam_paddr peerbase_pa;             /**<-- Peer BAM physical address */     
    bam_vaddr desc_base_va;            /**<-- Descriptor virtual base address  */     
    void *desc_fifo;                   /**<-- Descriptor fifo buffer to hold desciptor info */    
    uint32 desc_fifo_size;             /**<-- Descriptor fifo buf size */    
    bam_vaddr data_base_va;            /**<-- Data fifo virtula base address */    
    void *data_fifo;                   /**<-- Data fifo buffer to fold data fifo data*/    
    uint32 data_fifo_size;             /**<-- Size of data fifo buffer. */     
}bamdebug_pipe_config;


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
uint32 bamcore_getheaderbufferinfo(void);

/* ============================================================================
**  Function : bamcore_headerdump
** ============================================================================
*/
/**
  Log the pipe,Pipepipe & revision in dump buffer header.

  Log the pipe,Pipepipe & revision in dump buffer header.

  @param[in]  buf          Buffer to log the header.
  @param[in]  pipemask      Pipe mask
  @param[in]  peerpmask      Peer pipe mask.

  @return
  None
 */
void bamcore_headerdump(uint32 *buf, uint32 pipemask, uint32 peerpmask);

/* ============================================================================
**  Function : bamcore_bamregdump
** ============================================================================
*/
/**
  Gets the Dump of BAM Core register.

  Gets the Dump of BAM Core register.

  @param[in]  base      BAM virtual base address
  @param[in]  buf        Buffer to store the register dump.
  @param[in]  base_pa   Bam pa.
  @param[in]  ee        Bam EE.

  @return
  None
 */
void bamcore_bamregdump(bam_vaddr base, uint32 *buf, bam_paddr base_pa, uint32 ee);

/* ============================================================================
**  Function : bamcore_piperegdump
** ============================================================================
*/
/**
  Gets the Dump of BAM Pipe Core register.

  Gets the Dump of BAM Pipe Core register.

  @param[in]  base       BAM virtual base address
  @param[in]  base_pa    Bam base PA.
  @param[in]  bufptr     Buffer to get Bam Pipe register dump.
  @param[in]  pipedump   Pointer to peer pipe ,desc & data fifo core dump structure

  @return
  None
 */
void bamcore_piperegdump(bam_vaddr base, bam_paddr base_pa, uint32 *bufptr, bamdebug_pipe_config *pipedump);

/* ============================================================================
**  Function : bamcore_getctrlregbufferinfo
** ============================================================================
*/
/**
  Gets the BAM control reg dump buffer size.

  Gets the BAM control reg dump buffer size.

  @return
  buffer size requied for bam reg.
  */
uint32 bamcore_getctrlregbufferinfo(void);

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
uint32 bamcore_getpiperegbufferinfo(void);
#endif /* BAMCORE_DEBUG_H_ */

