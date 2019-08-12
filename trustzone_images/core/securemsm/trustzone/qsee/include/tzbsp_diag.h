#ifndef TZBSP_DIAG_H
#define TZBSP_DIAG_H

/**
@file tzbsp_diag.h
@brief TZ Diagnostic
*/

/*===========================================================================
   Copyright (c) 2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsee/include/tzbsp_diag.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $

when       who      what, where, why
--------   ---      ------------------------------------
12/16/12    mg      Support new log buffer management
09/26/11    mm      Updated with boot info, reset info etc.
10/20/10    rv      Initial version.

===========================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <comdef.h>
#include "tzbsp_target.h"
#include "tzbsp_isr.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
/* TZBSP Diag Magic number */
#define TZBSP_DIAG_MAGIC_NUM 0x747A6461

/* Diagnostic area Version number */
#define TZBSP_DIAG_MAJOR_VERSION    3
#define TZBSP_DIAG_MINOR_VERSION    0

/* Diagnostics Table Size */
#define TZBSP_DIAG_TBL_SIZE TZBSP_DIAG_SIZE

/* Ring buffer log should be at least 1KB. If this limit is reached, then the
 * diag area header has grown too big. */
#ifndef FIXME_8936 
#define TZBSP_DIAG_RING_BUF_MIN_SIZE 1024
#else
#define TZBSP_DIAG_RING_BUF_MIN_SIZE 768
#endif

/* Number of VMID Tables */
#define TZBSP_DIAG_NUM_OF_VMID 16

/* VMID Description length */
#define TZBSP_DIAG_VMID_DESC_LEN 7

/* Number of Interrupts */
#define TZBSP_DIAG_INT_NUM  TZBSP_MAX_ISR_COUNT

/* Length of descriptive name associated with Interrupt*/
#define TZBSP_MAX_INT_DESC 16

/* Enumeration order for VMID's */
typedef enum
{
  TZBSP_VM_ORDER_RPM = 0,
  TZBSP_VM_ORDER_TZ,
  TZBSP_VM_ORDER_AP,
  TZBSP_VM_ORDER_MMSS,
  TZBSP_VM_ORDER_MSS,
  TZBSP_VM_ORDER_LPASS,
  TZBSP_VM_ORDER_SMC_JTAG,
  TZBSP_VM_ORDER_SMC_ACC,
  TZBSP_VM_ORDER_SMC_RIVA,
  TZBSP_VM_ORDER_SLMB,
  TZBSP_VM_ORDER_MAX,
}tzbsp_vmid_e;

/* Extra NULL terminators added to make all strings
 * of equal length
 */
#define TZBSP_VM_STR_RPM               "RPM\0\0\0\0"
#define TZBSP_VM_STR_TZ                "TZ\0\0\0\0\0"
#define TZBSP_VM_STR_AP                "AP\0\0\0\0\0"
#define TZBSP_VM_STR_MMSS              "MMSS\0\0\0"
#define TZBSP_VM_STR_MSS               "MSS\0\0\0\0"
#define TZBSP_VM_STR_LPASS             "LPASS\0\0"
#define TZBSP_VM_STR_SMC_JTAG          "JTAG\0\0\0"
#define TZBSP_VM_STR_SMC_ACC           "ACC\0\0\0\0"
#define TZBSP_VM_STR_SMC_RIVA          "RIVA\0\0\0"
#define TZBSP_VM_STR_SLMB              "SLMB\0\0\0"
#define TZBSP_VM_STR_UNDEF             "\0\0\0\0\0\0\0"

/* VMID Table*/
typedef PACKED struct tzbsp_diag_vmid_s
{
  /* Virtual Machine Identifier */
  uint8 vmid;

  /* ASCII Text */
  uint8 desc[TZBSP_DIAG_VMID_DESC_LEN];
} tzbsp_diag_vmid_t;

/* Boot Info Table */
typedef struct tzbsp_diag_boot_info_s
{
  /* Warmboot entry CPU Counter */
  uint32 warm_entry_cnt;

  /* Warmboot exit CPU Counter */
  uint32 warm_exit_cnt;

  /* Power collapse termination entry counter. */
  uint32 term_entry_cnt;

  /* Power collapse termination exit counter. The number of times CPU fell
   * through the WFI without entering power collapse. */
  uint32 term_exit_cnt;

  /* Last Warmboot Jump Address */
  uint32 warm_jmp_addr;

  /* Last Warmboot Jump Address Instruction */
  uint32 warm_jmp_instr;
} tzbsp_diag_boot_info_t;

/* Reset Infor Table */
typedef struct tzbsp_diag_reset_info_s
{
  /* Reset Reason - Security Violation */
  uint32 reset_type;

  /* Number of resets that occured for above CPU */
  uint32 reset_cnt;
} tzbsp_diag_reset_info_t;

#define TZBSP_INT_TYPE_DATA_ABT         0x01  /* Data Abort */
#define TZBSP_INT_TYPE_UND              0x02  /* Undefined Instruction */
#define TZBSP_INT_TYPE_PREFETCH_ABT     0x03  /* Prefetch Abort */
#define TZBSP_INT_TYPE_SMC              0x04  /* SMC */
#define TZBSP_INT_TYPE_IRQ              0x05  /* IRQ */
#define TZBSP_INT_TYPE_FIQ              0x06  /* FIQ */
#define TZBSP_INT_TYPE_IRQ_EXIT         0x06  /* IRQ EXIT */

typedef struct tzbsp_diag_int_s
{
  /* Type of Interrupt/exception */
  uint16 int_info;

  /* Availability of the slot */
  uint8 avail;

  /* Reserved for future use. */
  uint8 spare;

  /* Interrupt Number for IRQ and FIQ interrupts */
  uint32 int_num;

  /* ASCII text describing type of interrupt e.g. Secure Timer, EBI XPU. This
   * string is always null terminated, supporting at most \c TZBSP_MAX_INT_DESC
   * characters. Any additional characters are truncated. */
  uint8 int_desc[TZBSP_MAX_INT_DESC];

  /* Number of this interrupt seen per CPU. */
  uint64 int_count[TZBSP_CPU_COUNT_MAX];
} tzbsp_diag_int_t;

typedef struct tzbsp_log_pos_s
{
  /* Ring buffer wrap-around ctr */
  uint16 wrap;
  
  /* Ring buffer current position */
  uint16 offset;
} tzbsp_log_pos_t;

typedef struct tzbsp_diag_log_s
{
  /* Ring buffer position mgmt */
  tzbsp_log_pos_t   log_pos;
  
  /* open ended array to the end of the 4K IMEM buffer */
  uint8             log_buf[1];
} tzbsp_diag_log_t;

/* Diagnostic Table */
typedef struct tzbsp_diag_s
{
  /* Magic Number */
  uint32 magic_num;

  /* Major.Minor version */
  uint32 version;

  /* Number of CPU's */
  uint32 cpu_count;

  /* Offset of VMID Table */
  uint32 vmid_info_off;

  /* Offset of Boot Table */
  uint32 boot_info_off;

  /* Offset of Reset Table */
  uint32 reset_info_off;

  /* Offset of Interrupt Table */
  uint32 int_info_off;

  /* Ring Buffer Offset */
  uint32 ring_off;

  /* Ring Buffer Len */
  uint32 ring_len;

  /* VMID to EE Mapping. */
  tzbsp_diag_vmid_t vmid[TZBSP_DIAG_NUM_OF_VMID];

  /* Boot Info */
  tzbsp_diag_boot_info_t  boot_info[TZBSP_CPU_COUNT_MAX];

  /* Reset Info */
  tzbsp_diag_reset_info_t reset_info[TZBSP_CPU_COUNT_MAX];

  /* Length of the interrupt table */
  uint32 num_interrupts;
  tzbsp_diag_int_t  int_info[TZBSP_DIAG_INT_NUM];

  /* We need at least 2K for the ring buffer */
  /* TZ Ring Buffer */
  tzbsp_diag_log_t log;
} tzbsp_diag_t;

/*===========================================================================

                      GLOBAL VARIABLE DECLARATIONS

===========================================================================*/
extern tzbsp_diag_t *g_tzbsp_diag;

#define TZBSP_ADD_VMID(x)                                               \
do                                                                      \
{                                                                       \
  vmid_tbl[TZBSP_VM_ORDER_##x].vmid = TZBSP_VMID_##x;                   \
  memcpy(vmid_tbl[TZBSP_VM_ORDER_##x].desc,TZBSP_VM_STR_##x,            \
                TZBSP_DIAG_VMID_DESC_LEN);                              \
}                                                                       \
while (0)

/* Macros for logging diag data */
#define TZBSP_DIAG_INT_ADD(x, desc) tzbsp_diag_int_add(x, desc)
#define TZBSP_DIAG_INT_FREE(x) tzbsp_diag_int_free(x)
#define TZBSP_DIAG_INCREMENT_INT_COUNT(x) tzbsp_diag_int_incr(x)

#define TZBSP_DIAG_RESET(reason)                                        \
  do {                                                                  \
    g_tzbsp_diag->reset_info[tzbsp_cur_cpu_num()].reset_type = (reason);\
    g_tzbsp_diag->reset_info[tzbsp_cur_cpu_num()].reset_cnt++;          \
  } while(0)

#define TZBSP_DIAG_WARM_JMP_ADDR(addr) \
  g_tzbsp_diag->boot_info[tzbsp_cur_cpu_num()].warm_jmp_addr = (addr)

#define TZBSP_DIAG_WARM_JMP_INSTR(instr) \
  g_tzbsp_diag->boot_info[tzbsp_cur_cpu_num()].warm_jmp_instr = (instr)

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/**
  @brief The TZBSP Diag Init Handler called once during cold boot.
  The Diag area is not reset if we are in download mode.
*/
int tzbsp_diag_init(void);

void tzbsp_diag_int_add(int n, const char* desc);
void tzbsp_diag_int_free(uint32 n);
void tzbsp_diag_int_incr(uint32 n);

int tzbsp_dump_tz_diag_to_temp_for_uefi(void);

#endif /* TZBSP_DIAG_H */
