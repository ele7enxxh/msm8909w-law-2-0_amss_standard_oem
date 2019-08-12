/**
  @file mhi_mmio.c
  @brief
  This file contains the MMIO register space initialization, configuration 
  and access routines. 
    

*/
/*
===============================================================================

                             Edit History

 $Header: 

when       who     what, where, why
--------   ---     ------------------------------------------------------------
05/01/14   nk      Reset MHISTATUS and MMIO in BOOT environment
03/20/14   nk      Restore all mmio
02/18/14   nk      Fix in get_ctrlinterrupts to handle multiple interrupts pending
10/09/13   am      Moved ipa routing to mhi_hwacc
10/01/13   am      Added mmio backup\restore functionality
06/36/13   nk      Formalize ipa routing via pcie driver
06/25/13   nk      Added interrupt clear/enable support
02/20/13   nk      Created
===============================================================================
                   Copyright (c) 2013-2014 QUALCOMM Technologies Incorporated.
                          All Rights Reserved.
                        Qualcomm Confidential and Proprietary.
===============================================================================
*/

#include "mhi_i.h"
#include "mhi_mmio.h"
#include "mhi_hwio.h"
#include "mhi_osal.h"
#include "mhi_core.h"

/** Mask macro */
#define MMIO_MASK(reg,field) ( reg##_##field##_MASK )
/** Shift Macro */
#define MMIO_SHFT(reg,field) ( reg##_##field##_SHIFT )

/** Shift MAsk Macro. Returns the shifted mask useful for testing bits */   
#define MMIO_SHFT_MASK(reg,field) (MMIO_MASK(reg,field)<<MMIO_SHFT(reg,field))

/** MMIO_OUT, used to write to a MMIO register **/
#define MMIO_OUT(REG,VAL) mhi_mmio_write_reg(mmio_base, REG, VAL )
/** MMIO_OUT, used to do a masked write to a MMIO register **/
#define MMIO_OUTM(REG,VAL, MASK) mhi_mmio_write_reg(mmio_base, REG, (mhi_mmio_read_reg(mmio_base,REG)&~(MASK))|VAL)
/** MMIO_IN used to read from a MMIO register **/
#define MMIO_IN(REG) mhi_mmio_read_reg(mmio_base,REG)
/** MMIO_OUTF, used to write to a field of a MMIO register **/
#define MMIO_OUTF(REG,FIELD,VAL) mhi_mmio_write_reg_field(mmio_base, REG, MMIO_MASK(##REG,##FIELD), MMIO_SHFT(##REG,##FIELD), VAL)
/** MMIO_INF, used to read a field from a MMIO register **/
#define MMIO_INF(REG,FIELD) mhi_mmio_read_reg_field(mmio_base, REG, MMIO_MASK(##REG,##FIELD), MMIO_SHFT(##REG,##FIELD))

/* Double Register reads/writes for quad words */
/** MMIO_OUT, used to write to a MMIO register **/
#define MMIO_OUTD(REG,VAL) mhi_mmio_write_regd(mmio_base, REG, VAL )
/** MMIO_IN used to read from a MMIO register **/
#define MMIO_IND(REG) mhi_mmio_read_regd(mmio_base,REG)
/** MMIO_OUTF, used to write to a field of a MMIO register **/
#define MMIO_OUTFD(REG,FIELD,VAL) mhi_mmio_write_regd_field(mmio_base, REG, MMIO_MASK(##REG,##FIELD), MMIO_SHFT(##REG,##FIELD), VAL)
/** MMIO_INF, used to read a field from a MMIO register **/
#define MMIO_INFD(REG,FIELD) mhi_mmio_read_regd_field(mmio_base, REG, MMIO_MASK(##REG,##FIELD), MMIO_SHFT(##REG,##FIELD))

/* Interrupt related macros */ 
#define MHICTRLINT 3

/* Size of MMIO 4K */
#define MHI_MMIO_SIZE_MAX 0x1000

/* Size of MMIO to backup including: MHI, BHI, CHDB and ERDB array */
#define MHI_MMIO_BACKUP_SIZE 0xB00 

/* read/write MMIO */ 
uint32 mhi_mmio_read_reg_field(uint32 base, uint32 offset, uint32 mask, uint32 shift);
uint32 mhi_mmio_read_reg(uint32 base, uint32 offset);
void mhi_mmio_write_reg_field(uint32 base, uint32 offset, uint32 mask, uint32 shift, uint32 field_val);
void mhi_mmio_write_reg(uint32 base, uint32 offset, uint32 val);

/* These read 2 registers for quad word addressing */
uint64 mhi_mmio_read_regd_field(uint32 base, uint32 offset, uint64 mask, uint32 shift);
uint64 mhi_mmio_read_regd(uint32 base, uint32 offset);
void mhi_mmio_write_regd_field(uint32 base, uint32 offset, uint64 mask, uint32 shift, uint64 field_val);
void mhi_mmio_write_regd(uint32 base, uint32 offset, uint64 val);

/** MHI Base, initialized on init */
static uint32 mmio_base;

/* MHI Spec register base */
static uint32 mmio_mhi_reg_base;

/* MMIO backup */ 
static uint32 *mmio_backup = NULL; 

/* Current Event ring doorbell mask */
static uint32 erdbmask[]={0,0,0,0}; 

/* Current Channel doorbell mask */ 
static uint32 chdbmask[]={0,0,0,0}; 

/** 
 * @brief      mhi_mmio_read_regd_field
 *
 *             Read double register field
 *              
 * 
 * @param[in]  base   - Base address
 * @param[in]  offset - Offset
 * @param[in]  mask   - mask 
 * @param[in]  shift  - Shift of field
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             Field value
 * 
 * @sideeffect None 
 */
uint64 mhi_mmio_read_regd_field(uint32 base, uint32 offset, uint64 mask, uint32 shift)
{
   uint32 addr = base + offset;
   uint64 regval = 0; 

#ifdef MHI_CHECK_HW_ENABLED
   if (!mhiCtx.mhi_link_info.mhi_link_hw_enabled)
   {
      mhi_osal_debug(MSG_FATAL,"mhi_mmio_read_regd_field: Attempted mmio access when link hw disabled");
   }
#endif
   regval = (*((volatile uint64 *)(addr)));
   regval &= mask;
   regval >>= shift;

   return regval;
}

/** 
 * @brief      mhi_mmio_read_regd
 *
 *             Read double register 
 *              
 * 
 * @param[in]  base - Base address 
 * @param[in]  offset - Offset 
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             Value read
 * 
 * @sideeffect None 
 */
uint64 mhi_mmio_read_regd(uint32 base, uint32 offset)
{
   uint32 addr = base + offset;

#ifdef MHI_CHECK_HW_ENABLED
   if (!mhiCtx.mhi_link_info.mhi_link_hw_enabled)
   {
      mhi_osal_debug(MSG_FATAL,"mhi_mmio_read_regd: Attempted mmio access when link hw disabled");
   }
#endif
   return (*((volatile uint64 *)(addr)));
}

/** 
 * @brief      mhi_mmio_write_regd_field
 *
 *             Write double register field
 *              
 * 
 * @param[in]  base      - Base address
 * @param[in]  offset    - Offset
 * @param[in]  mask      - Mask
 * @param[in]  shift     - Shift 
 * @param[in]  field_val - Value of field to write
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             None
 * 
 * @sideeffect None 
 */
void mhi_mmio_write_regd_field(uint32 base, uint32 offset, uint64 mask, uint32 shift, uint64 field_val)
{
   uint32 addr = base + offset;
   uint64 regval = 0;

#ifdef MHI_CHECK_HW_ENABLED
   if (!mhiCtx.mhi_link_info.mhi_link_hw_enabled)
   {
      mhi_osal_debug(MSG_FATAL,"mhi_mmio_write_regd_field: Attempted mmio access when link hw disabled");
   }  
#endif
   regval = (*((volatile uint64 *)(addr)));
   regval &= ~mask;
   regval = regval | (field_val << shift);
   (*((volatile uint64 *)(addr))) = regval;
}

/** 
 * @brief      mhi_mmio_write_regd
 *
 *             Write double register 
 *              
 * 
 * @param[in]  base      - Base address
 * @param[in]  offset    - Offset
 * @param[in]  val       - Value to write
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             None
 * 
 * @sideeffect None 
 */
void mhi_mmio_write_regd(uint32 base, uint32 offset, uint64 val)
{
   uint32 addr = base + offset;

#ifdef MHI_CHECK_HW_ENABLED
   if (!mhiCtx.mhi_link_info.mhi_link_hw_enabled)
   {
      mhi_osal_debug(MSG_FATAL,"mhi_mmio_write_regd: Attempted mmio access when link hw disabled");
   }
#endif    
   (*((volatile uint64 *)(addr))) = val;
}

/** 
 * @brief      mhi_mmio_read_reg_field
 *
 *             Read register field
 *              
 * 
 * @param[in]  base      - Base address
 * @param[in]  offset    - Offset
 * @param[in]  mask      - Mask
 * @param[in]  shift     - Shift 
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             Value read
 * 
 * @sideeffect None 
 */
uint32 mhi_mmio_read_reg_field(uint32 base, uint32 offset, uint32 mask, uint32 shift)
{
   uint32 addr = base + offset;
   uint32 regval = 0;

#ifdef MHI_CHECK_HW_ENABLED   
   if (!mhiCtx.mhi_link_info.mhi_link_hw_enabled)
   {
      mhi_osal_debug(MSG_FATAL,"mhi_mmio_read_reg_field: Attempted mmio access when link hw disabled");
   }
#endif   
   regval = (*((volatile uint32 *)(addr)));
   regval &= mask;
   regval >>= shift;
   
   return regval;
}

/** 
 * @brief      mhi_mmio_read_reg
 *
 *             Read register
 *              
 * 
 * @param[in]  base   - Base address
 * @param[in]  offset - Offset
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             Value read
 * 
 * @sideeffect None 
 */
uint32 mhi_mmio_read_reg(uint32 base, uint32 offset)
{
   uint32 addr = base + offset;

#ifdef MHI_CHECK_HW_ENABLED
   if (!mhiCtx.mhi_link_info.mhi_link_hw_enabled)
   {
      mhi_osal_debug(MSG_FATAL,"mhi_mmio_read_reg: Attempted mmio access when link hw disabled");
   }
#endif
   return (*((volatile uint32 *)(addr)));
}

/** 
 * @brief      mhi_mmio_write_reg_field
 *
 *             Write register field
 *              
 * 
 * @param[in]  base      - Base address
 * @param[in]  offset    - Offset
 * @param[in]  mask      - Mask
 * @param[in]  shift     - Shift 
 * @param[in]  field_val - Value of field to write
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             None
 * 
 * @sideeffect None 
 */
void mhi_mmio_write_reg_field(uint32 base, uint32 offset, uint32 mask, uint32 shift, uint32 field_val)
{
   uint32 addr = base + offset;
   uint32 regval = 0;

#ifdef MHI_CHECK_HW_ENABLED
   if (!mhiCtx.mhi_link_info.mhi_link_hw_enabled)
   {
      mhi_osal_debug(MSG_FATAL,"mhi_mmio_write_reg_field: Attempted mmio access when link hw disabled");
   }
#endif
   regval = (*((volatile uint32 *)(addr)));
   regval &= ~mask;
   regval = regval | (field_val << shift);
   (*((volatile uint32 *)(addr))) = regval;
}

/** 
 * @brief      mhi_mmio_write_reg
 *
 *             Write register
 *              
 * 
 * @param[in]  base   - Base address
 * @param[in]  offset - Offset
 * @param[in]  val    - Value to write
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             None
 * 
 * @sideeffect None 
 */
void mhi_mmio_write_reg(uint32 base, uint32 offset, uint32 val)
{
   uint32 addr = base + offset;

#ifdef MHI_CHECK_HW_ENABLED
   if (!mhiCtx.mhi_link_info.mhi_link_hw_enabled)
   {  
      mhi_osal_debug(MSG_FATAL,"mhi_mmio_write_reg: Attempted mmio access when link hw disabled");
   }
#endif      
   (*((volatile uint32 *)(addr))) = val;
}

/** 
 * @brief      mhi_mmio_enablechdbinterrupts
 *
 *             Enable specified channel doorbell interrupts 
 *              
 * 
 * @param[in]  *mask - mask representing interrupts to enable
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             None
 * 
 * @sideeffect None 
 */
void mhi_mmio_enablechdbinterrupts(mhi_interrupts_t *mask)
{
   uint32 i; 

   for(i=0; i < 4; i++)
   {
      chdbmask[i]|=mask->chdb[i]; 
      MMIO_OUT(MHI_CHDB_INT_MASK_A7_n(i), chdbmask[i]);
   }
}

/** 
 * @brief      mhi_mmio_disablechdbinterrupts
 *
 *             Disable specified channel doorbell interrupts 
 *              
 * 
 * @param[in]  *mask - Mask representing interrupts to disable
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             None
 * 
 * @sideeffect None 
 */
void mhi_mmio_disablechdbinterrupts(mhi_interrupts_t *mask)
{
   uint32 i; 

   for(i=0; i < 4; i++)
   {
      chdbmask[i]&=~mask->chdb[i]; 
      MMIO_OUT(MHI_CHDB_INT_MASK_A7_n(i), chdbmask[i]);
   }
}

/** 
 * @brief      mhi_mmio_disablectrlinterrupt
 *
 *             Disable control interrupts
 *              
 *
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             None
 * 
 * @sideeffect None 
 */
void mhi_mmio_disablectrlinterrupt(void)
{
   MMIO_OUT(MHI_MMIO_CTRL_INT_MASK_A7, 0);
}

/** 
 * @brief      mhi_mmio_enablectrlinterrupts
 *
 *             Enable control interrupts
 *              
 *
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             None
 * 
 * @sideeffect None 
 */
void mhi_mmio_enablectrlinterrupts(void)
{
   MMIO_OUT(MHI_MMIO_CTRL_INT_MASK_A7, MHICTRLINT );
}

/** 
 * @brief      mhi_mmio_enableerdbinterrupt
 *
 *             Enable doorbell interrupt of specified event ring
 *              
 * 
 * @param[in]  ernum - Event ring number
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             None
 * 
 * @sideeffect None 
 */
void mhi_mmio_enableerdbinterrupt(uint32 ernum)
{
   erdbmask[ernum/32]|=1<<(ernum%32); 
   MMIO_OUT(MHI_ERDB_INT_MASK_A7_n(ernum/32), erdbmask[ernum/32]); 
}

/** 
 * @brief      mhi_mmio_enableerdbinterrupts
 *
 *             Enable specified event ring doorbell interrupts
 *              
 * 
 * @param[in]  *mask - Mask representing interrupts to enable
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             None
 * 
 * @sideeffect None 
 */
void mhi_mmio_enableerdbinterrupts(mhi_interrupts_t *mask)
{
   uint32 i; 

   for(i=0; i < 4; i++)
   {
      erdbmask[i]|=mask->erdb[i]; 
      MMIO_OUT(MHI_ERDB_INT_MASK_A7_n(i), erdbmask[i]); 
   }
}

/** 
 * @brief      mhi_mmio_disableerdbinterrupts
 *
 *             Disable specified event ring doorbell interrupts
 *              
 * 
 * @param[in]  *mask
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             None
 * 
 * @sideeffect None 
 */
void mhi_mmio_disableerdbinterrupts(mhi_interrupts_t *mask)
{
   uint32 i; 

   for(i=0; i < 4; i++)
   {
      erdbmask[i]&=~mask->erdb[i]; 
      MMIO_OUT(MHI_ERDB_INT_MASK_A7_n(i), erdbmask[i]); 
   }
}

/** 
 * @brief      mhi_mmio_disableerdbinterrupt
 *
 *             Disable event ring doorbell interrupt of specified event ring
 *              
 * 
 * @param[in]  ernum - Event ring number
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             None
 * 
 * @sideeffect None 
 */
void mhi_mmio_disableerdbinterrupt(uint32 ernum)
{
   erdbmask[ernum/32]&=~(1<<(ernum%32)); 
   MMIO_OUT(MHI_ERDB_INT_MASK_A7_n(ernum/32), erdbmask[ernum/32]); 
}

/** 
 * @brief      mhi_mmio_unmaskinterrupts
 *
 *             Unmask all interrupts 
 *              
 *
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             None
 * 
 * @sideeffect None 
 */
void mhi_mmio_unmaskinterrupts(void)
{
   uint32 i; 
    
   /* Enable all DB interrupts for SW channels */ 
 
   /* Enable all CHDB interrupts */ 
   for(i=0; i < 4; i++)
   {
      MMIO_OUT(MHI_CHDB_INT_MASK_A7_n(i), chdbmask[i]);

   }

   /* Enable all ERDB interrupts */ 
   for(i=0; i < 4; i++)
   {
      MMIO_OUT(MHI_ERDB_INT_MASK_A7_n(i), erdbmask[i]); 
   }

   MMIO_OUT(MHI_MMIO_CTRL_INT_MASK_A7, MHICTRLINT );
}

/** 
 * @brief      mhi_mmio_maskinterrupts
 *
 *             Mask all interrupts
 *              
 *
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             None
 * 
 * @sideeffect None 
 */
void mhi_mmio_maskinterrupts(void)
{
   uint32 i; 
    
   /* Disable all DB interrupts. 
    * Note disabling it will not clear a pending interrupt 
    * nor will it prevent a interrupt from getting set. 
    * The mask is used to only control whether the interrupt can go to the processor */ 
   for(i = 0; i < 4; i++)
   {
      MMIO_OUT(MHI_CHDB_INT_MASK_A7_n(i), 0);
      MMIO_OUT(MHI_ERDB_INT_MASK_A7_n(i), 0);
   }

   MMIO_OUT(MHI_MMIO_CTRL_INT_MASK_A7, 0x0 );
}

/** 
 * @brief      mhi_mmio_clearinterrupts
 *
 *             Clear all interrupts
 *              
 *
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             None
 * 
 * @sideeffect None 
 */
void mhi_mmio_clearinterrupts(void)
{
   int i; 

   /* Clear all DB interrupts */
   for(i = 0; i < 8; i++)
   {
      MMIO_OUT(MHI_DB_INT_CLEAR_A7_n(i), 0xFFFFFFFF);
   }
  

   MMIO_OUT(MHI_MMIO_CTRL_INT_CLEAR_A7, MHICTRLINT );
}

/** 
 * @brief      mhi_mmio_reset
 *
 *             Reset mmio 
 *              
 * 
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             None
 * 
 * @sideeffect None 
 */
void mhi_mmio_reset(void)
{
   /* Clear Control/state */
   MMIO_OUT(MHICTRL, 0);
   MMIO_OUT(MHISTATUS, 0); 

   /* Clear interrupts */ 
   mhi_mmio_clearinterrupts();

   mhi_osal_debug(MSG_HIGH, "mhi_mmio_reset: Initializing mmio complete"); 
}


/**
 * @brief      mhi_mmio_init
 *
 *             Performs any initialization necessary for mmio register operation 
 *
 * @param[in]  base - Address of MHI register base
 * @param[in]  mhiConfig - MHI HW/SW Device configuration 
 *              
 *
 * @dependencies
 *              None 
 * 
 * @return       
 *              MHI_SUCCESS if operation succeeded  
 *              MHI_ERROR if operation failed 
 * 
 * @sideeffect  None 
*/
mhi_status_t mhi_mmio_init(uint32 base, mhi_config_t *mhiConfig)
{
   mhi_osal_meminfo mmio_mem; 
   boolean mmio_initialized=FALSE; 
   
   mhi_osal_debug(MSG_HIGH, "mhi_mmio_init: Initializing mmio"); 

   mmio_mem.pa = base;  
   mmio_mem.va = 0;  
   mmio_mem.size = MHI_MMIO_SIZE_MAX;

   mhi_osal_mallocmemregion(&mmio_mem, mmio_mem.size);
   if (mmio_mem.handle == NULL)
   {
      mhi_osal_debug(MSG_ERR, "mhi_mmio_init: Could not allocate memory for mmio "); 
      return MHI_ERROR;  
   }

   mmio_base = mmio_mem.va;
   mmio_mhi_reg_base = mmio_base + MHIREGLEN;

#ifndef MHI_BOOT
   /* Set the version of MHI spec we support */
   if(MMIO_IN(MHIVER) == mhiConfig->version)
   {
      mmio_initialized=TRUE; 
   }
   else
#endif
   {
      MMIO_OUT(MHIVER,mhiConfig->version);  
   }
   /* Read in the MHI HW initialized configuration */
   mhiConfig->mhiRegLen = MMIO_IN(MHIREGLEN);
   mhiConfig->nEr = MMIO_INF(MHICFG, NER);
   mhiConfig->nCh = SWCHEND;                  
   mhiConfig->chdboff = MMIO_IN(CHDBOFF); 
   mhiConfig->erdboff = MMIO_IN(ERDBOFF);  

#ifdef MHI_BOOT
   /* Override channels that are not needed */
   mhiConfig->nCh = 0x4;
#endif
   
   /* If MMIO was not initialized reset MMIO */
   if(mmio_initialized == FALSE)
   {
      /* Reset rest of mmio */ 
      mhi_mmio_reset();
   }

   /* Allocate a MMIO register backup space. This can also be used for dumping
    * MMIO for debug. Size is the maximum space that can be occupied by MMIO
    */ 
   mmio_backup = mhi_osal_malloc(MHI_MMIO_BACKUP_SIZE);
   if(!mmio_backup)
   {
     mhi_osal_debug(MSG_ERR, "mhi_mmio_init: Could not allocate memory for mmio backup");
     return MHI_ERROR;
   }

   /* Initialize the backup space to 0s */
   mhi_osal_memset(mmio_backup,0,MHI_MMIO_BACKUP_SIZE);
   
   return MHI_SUCCESS;
}

/**
 * @brief      mhi_mmio_deinit
 *
 *             Cleanup mhi_mmio_init operation 
 *
 *
 * @dependencies
 *             mhi_mmio_init needs to be called first 
 * 
 * @return       
 *              MHI_SUCCESS if operation succeeded  
 *              MHI_ERROR if operation failed 
 * 
 * @sa
 * mhi_mmio_init 
*/
mhi_status_t mhi_mmio_deinit(void)
{
   /* Set ready to false */
   MMIO_OUTF(MHISTATUS,READY,0); 

   mhi_osal_free(mmio_backup);
   
   return MHI_SUCCESS;
}
/**
 * @brief      mhi_mmio_get_mhistate
 *
 *             Get MMIO MHI state from MHICTRL  
 *
 *
 * @dependencies
 *              None 
 * 
 * @return       
 *             MHI state.  
 * 
 * @sa
 * mhi_mmio_init 
*/
mhi_state_t mhi_mmio_get_mhistate(void)
{
  mhi_state_t state; 

  state=(mhi_state_t)MMIO_INF(MHICTRL, MHISTATE);
  return state; 
}

/** 
 * @brief      mhi_mmio_is_reset
 *
 *             Return the reset bit of MMIO
 *              
 *
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             MMIO reset bit
 * 
 * @sideeffect None 
 */
uint32 mhi_mmio_is_reset(void)
{
  return MMIO_INF(MHICTRL,RESET);
}

/** 
 * @brief      mhi_mmio_clear_reset
 *
 *             Clear the reset bit
 *              
 *
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             None
 * 
 * @sideeffect None 
 */
void mhi_mmio_clear_reset(void)
{
  MMIO_OUTF(MHICTRL,RESET,0);
}

/** 
 * @brief      mhi_mmio_get_chbase
 *
 *             Get the channel context base address
 *              
 *
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             Channel context base address
 * 
 * @sideeffect None 
 */
uint64 mhi_mmio_get_chbase(void)
{
  return MMIO_IND(CCABAP_LOWER); 
}

/** 
 * @brief      mhi_mmio_get_erbase
 *
 *             Get the event context base address
 *              
 *
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             Event context base address
 * 
 * @sideeffect None 
 */
uint64 mhi_mmio_get_erbase(void)
{
  return MMIO_IND(ECABAP_LOWER);
}

/** 
 * @brief      mhi_mmio_get_crbase
 *
 *             Get the command ring context base address
 *              
 *
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             Return the command ring context base address
 * 
 * @sideeffect None 
 */
uint64 mhi_mmio_get_crbase(void)
{
  return MMIO_IND(CRCBAP_LOWER);
}

/** 
 * @brief      mhi_mmio_get_mhiaddr
 *
 *             Get the MHI address information
 *              
 * 
 * @param[out]  *mhiaddr - Received MHI address info configured by host
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             None
 * 
 * @sideeffect None 
 */
void mhi_mmio_get_mhiaddr(mhi_host_addr_t *mhiaddr)
{
   mhiaddr->mhictrlbase=MMIO_IND(MHICTRLBASE_LOWER);
   mhiaddr->mhictrllimit=MMIO_IND(MHICTRLLIMIT_LOWER);
   mhiaddr->mhidatabase=MMIO_IND(MHIDATABASE_LOWER); 
   mhiaddr->mhidatalimit=MMIO_IND(MHIDATALIMIT_LOWER);
}

/** 
 * @brief      mhi_mmio_get_crdb
 *
 *             Get the command ring doorbell value
 *              
 *
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             Command ring doorbell value
 * 
 * @sideeffect None 
 */
uint64 mhi_mmio_get_crdb(void)
{
  return MMIO_IND(CRDB_LOWER);

}

/** 
 * @brief      mhi_mmio_get_chdb
 *
 *             #desc 
 *              
 * 
 * @param[in]  cci
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             None
 * 
 * @sideeffect None 
 */
uint64 mhi_mmio_get_chdb(uint32 cci)
{
  return MMIO_IND(CHDB_LOWER_n(cci)); 

}

/** 
 * @brief      mhi_mmio_get_erdb
 *
 *             Get the event ring doorbell of specified event ring
 *              
 * 
 * @param[in]  eci - Event ring index
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             Event ring doorbell
 * 
 * @sideeffect None 
 */
uint64 mhi_mmio_get_erdb(uint32 eci)
{
  return MMIO_IND(ERDB_LOWER_n(eci)); 
}

/** 
 * @brief      mhi_mmio_get_mhictrlinterrupts
 *
 *             Get MHI Control interrupts. The first parameter is or-red with new interrupts
 *             so as to not clear any existing ones. The second parameter changed is set to 
 *             new interrupts
 *              
 * 
 * @param[out]  *interrupts - Interrupt object to receive interrupts
 * @param[out]  *changed    - Interrupt object to receive new interrupts
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             None
 * 
 * @sideeffect Clears control interrupts 
 */
void mhi_mmio_get_mhictrlinterrupts(mhi_interrupts_t *interrupts, mhi_interrupts_t *changed )
{
  uint32 int_status; 
  int_status = MMIO_IN(MHI_MMIO_CTRL_INT_STATUS_A7);

  /* Clear the interrupts */
  MMIO_OUT(MHI_MMIO_CTRL_INT_CLEAR_A7, int_status );

  changed != NULL? changed->ctrl=MHI_CTRL_CLEAR:1;

  /* Bit 1 is MHICTRL Interrupt */ 
  if(int_status & 1)
  {
     changed!= NULL?changed->ctrl|=MHI_CTRL_INT:1; 
     interrupts->ctrl|=MHI_CTRL_INT;
  }

  /* Bit 2 is CRDB */ 
  if(int_status & 2)
  {
     changed!=NULL?changed->ctrl|=MHI_CRDB_INT:1; 
     interrupts->ctrl|=MHI_CRDB_INT;
  }

}

/** 
 * @brief      mhi_mmio_get_chdbinterrupts
 *
 *             Get MHI channel doorbell interrupts. The first parameter is or-red with new interrupts
 *             so as to not clear any existing ones. The second parameter changed is set to 
 *             new interrupts
 *              
 * 
 * @param[out]  *interrupts - Interrupt object to receive interrupts
 * @param[out]  *changed    - Interrupt object to receive new interrupts
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             None
 * 
 * @sideeffect Clears only enabled interrupts 
 */
void mhi_mmio_get_chdbinterrupts(mhi_interrupts_t *interrupts, mhi_interrupts_t *changed)
{
   uint32 i;
   uint32 status;  

   /* Read the channel doorbell interrupt status registers */
   for(i = 0; i < 4; i++)
   {
     status=MMIO_IN(MHI_DB_INT_STATUS_A7_n(i));

     status&=chdbmask[i]; 

     MMIO_OUT(MHI_DB_INT_CLEAR_A7_n(i), status);

     interrupts->chdb[i]|=status; 
     if(changed != NULL)
        changed->chdb[i]=status; 
   }

}

/** 
 * @brief      mhi_mmio_get_erdbinterrupt
 *
 *             Return single event ring doorbell interrupt. 
 *              
 * 
 * @param[in]  erdb - Event ring 
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             None
 * 
 * @sideeffect Clears interrupt
 */
boolean mhi_mmio_get_erdbinterrupt(uint32 ernum)
{
   uint32 status;  

   /* ER starts from 4  */
   status=MMIO_IN(MHI_DB_INT_STATUS_A7_n(ernum/32+4)); 
   MMIO_OUT(MHI_DB_INT_CLEAR_A7_n(ernum/32+4), status);

   if(status & (1 << ernum%32))
      return TRUE; 
   else
      return FALSE;
}

/** 
 * @brief      mhi_mmio_get_erdbinterrupts
 *
 *             Get MHI event ring doorbell interrupts. The first parameter is or-red with new interrupts
 *             so as to not clear any existing ones. The second parameter changed is set to 
 *             new interrupts
 *              
 * 
 * @param[out]  *interrupts - Interrupt object to receive interrupts
 * @param[out]  *changed    - Interrupt object to receive new interrupts
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             None
 * 
 * @sideeffect Clears enabled event ring interrupts 
 */
void mhi_mmio_get_erdbinterrupts(mhi_interrupts_t *interrupts, mhi_interrupts_t *changed )
{
   uint32 i;
   uint32 status;  

   /* REad the event ring doorbell interrupt status registers */
   for(i = 0; i < 4; i++)
   {
     /* ER starts from 4. In latest HWIO well have a seperate register  */
     status=MMIO_IN(MHI_DB_INT_STATUS_A7_n(i+4)); /* ER starts from 4. In latest HWIO well have a seperate register  */
      
     /* Apply mask and clear */ 
     status &=erdbmask[i];
     MMIO_OUT(MHI_DB_INT_CLEAR_A7_n(i+4), status);
     interrupts->erdb[i]|=status; 

     /* If changed field specified, set which db's are new */
     if(changed != NULL)
        changed->erdb[i]=status; 
   }
}

/**
 * @brief      mhi_mmio_set_mhistate
 *
 *             Set MMIO registers to reflect corresponding state 
 *
 * @param[in]  state - MHI state to set 
 *
 * @dependencies
 *              None 
 * 
 * @return       
 *              MHI_SUCCESS if operation succeeded  
 *              MHI_ERROR if operation failed 
 * 
 * @sideeffect None
*/
mhi_status_t mhi_mmio_set_mhistate(mhi_state_t state)
{
   switch(state)
   {
      case MHI_STATE_RESET: 
         /* Reset MHI state bits, and CTRL bits */
         MMIO_OUTF(MHISTATUS,READY,0); 
         MMIO_OUTF(MHISTATUS,SYSERR,0);
         MMIO_OUTF(MHISTATUS,MHISTATE,0);
         break;
      case MHI_STATE_READY:
         MMIO_OUTF(MHISTATUS,READY,1);
         MMIO_OUTF(MHISTATUS,MHISTATE,state);
         break;
      case MHI_STATE_M0:
      case MHI_STATE_M1:
      case MHI_STATE_M2:
      case MHI_STATE_M3:  
         MMIO_OUTF(MHISTATUS,MHISTATE,state);
         break;
      case MHI_STATE_SYSERR:
         MMIO_OUTF(MHISTATUS,SYSERR,1); 
         MMIO_OUTF(MHISTATUS,MHISTATE,state);
         break;
      default:
         mhi_osal_debug(MSG_FATAL,"mhi_mmio_set_mhistate: Invalid M state %d",state);
   } 
   
   return MHI_SUCCESS;
}

/** 
 * @brief      mhi_mmio_set_execenv
 *
 *             Set the execution environment
 *              
 * 
 * @param[in]  env - Environment
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             None
 * 
 * @sideeffect None 
 */
void mhi_mmio_set_execenv(uint32 env)
{
   MMIO_OUT(BHI_EXECENV,env); 
}

/**
 * @brief       mhi_mmio_backup
 *
 *              Restore MMIO from RAM.
 *
 *@param[in]    *mhiConfig - MHI HW/SW Device configuration 
 *
 * @dependencies
 *              None 
 * 
 * @return       
 *              None
 *
 * 
 * @sideeffect  None 
 */
void mhi_mmio_backup(mhi_config_t *mhiConfig)
{
  mhi_osal_debug(MSG_HIGH, "mhi_mmio_backup");
  mhi_osal_memcpy(mmio_backup,(uint32 *)mmio_mhi_reg_base,MHI_MMIO_BACKUP_SIZE);
}

/**
 * @brief       mhi_mmio_restore
 *
 *              Restore MMIO from RAM.
 *
 *@param[in]    *mhiConfig - MHI HW/SW Device configuration 
 *
 * @dependencies
 *              None 
 * 
 * @return       
 *              None
 *
 * 
 * @sideeffect  None 
 */
void mhi_mmio_restore(mhi_config_t *mhiConfig)
{
  mhi_osal_debug(MSG_MEDIUM, "mhi_mmio_restore");

  /* Mask all MHI interrupts as restoring MMIO may trigger certain interrupts */
  mhi_mmio_maskinterrupts();
  
  mhi_osal_memcpy((uint32 *)mmio_mhi_reg_base, (uint32 *)mmio_backup,MHI_MMIO_BACKUP_SIZE);

  /* Clear and enable interrupts */ 
  mhi_mmio_clearinterrupts();
  
  /* Restore interrupts */ 
  mhi_mmio_enablechdbinterrupts((mhi_interrupts_t*)chdbmask);
  mhi_mmio_enableerdbinterrupts((mhi_interrupts_t*)erdbmask);
  mhi_mmio_enablectrlinterrupts();
}

/**
 * @brief       mhi_mmio_set_mhictrl_reg
 *
 *              Set the MHI state in the MHICTRL register. Used for test ONLY.
 *
 *@param[in]    state - MHI state to set.
 *
 *
 * @dependencies
 *              None 
 * 
 * @return       
 *              None
 *
 * 
 * @sideeffect  MHI control interrupt will be generated. 
 */
void mhi_mmio_set_mhictrl_mhistate(mhi_state_t state)
{
  MMIO_OUTF(MHICTRL,MHISTATE,state);
}

/**
 * @brief       mhi_mmio_set_reset
 *
 *              Set the reset bit in the MHICTRL register. Used for test ONLY.
 *
 *
 * @dependencies
 *              None 
 * 
 * @return       
 *              None
 *
 * 
 * @sideeffect  MHI control interrupt will be generated. 
 */
void mhi_mmio_set_reset(void)
{
  MMIO_OUTF(MHICTRL,RESET,1);
}

/**
 * @brief       mhi_mmio_get_mhistate_from_status_reg
 *
 *              Reads MHI status only from the MHISTATUS register. Used only in test
 *              for now.
 *
 * @dependencies
 *              None 
 * 
 * @return       
 *              MHI State
 *
 * 
 * @sideeffect  None 
 */
mhi_state_t mhi_mmio_get_mhistate_from_status_reg(void)
{
  return (mhi_state_t)MMIO_INF(MHISTATUS, MHISTATE);
}


