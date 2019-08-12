#ifdef CONFIG_SECTIONS
#define SECTION(a) __attribute__((__section__(a)))
#else
#define SECTION(a)
#endif

#if !defined(__HEXAGON_ARCH__)
#if defined(__QDSP6_ARCH__)
#define __HEXAGON_ARCH__ __QDSP6_ARCH__
#else
#error Cannot determine architecture level
#endif
#endif


/* Bit mask to start HW threads, 0x3f to start all HW threads */
unsigned int QURTK_hthread_startup = HTHREADMASK;

/* Priority for main thread */
unsigned int QURTK_main_priority = QURTKMAINPRIORITY;
unsigned int QURTK_main_bitmask = QURTKMAINBITMASK;

/* Qurt PIMUTEX priority */
unsigned int QURTK_max_pi_prio = MAXPIMUTEXPRIO;

/* Stack size for main thread */
unsigned int QURTK_main_stack_size = MAINSTACKSIZE;

unsigned int QURTK_app_heap_size = HEAPSIZE;

/* Timetest port number */
unsigned int QURTK_timetest_port = TIMETESTPORT;

/* The address of IMEM that contains the flag for initializing ISDB */
unsigned int QURTK_isdb_flag_imem_addr __attribute__((section(".data"))) = ISDBIMEMADDR;

/* The address of memory-map ETM & ETB config registers */
unsigned int QURTK_etm_cfg_base __attribute__((section(".data"))) = ETMCFGBASEADDR;

/* Interrupts that wake up the processor during TCXO shutdown
 * Most significant bit is INT 0 */
unsigned int QURTK_tcxo_intmask = TCXOINTMASK;

#if __HEXAGON_ARCH__  >= 5
/* flag to enable DMT scheduling */
unsigned int QURTK_dmt_enabled __attribute__((section(".data"))) = DMT_ENABLED;

/* flag to enable bus resources sharing */
unsigned int QURTK_BQ_shared __attribute__((section(".data"))) = BQ_SHARED;
#endif

/* TCM dump */
unsigned int qurt_tcm_dump_size = (TCMSIZE);
unsigned int qurt_tcm_dump[(TCMSIZE)/4] __attribute__((aligned(32))) __attribute__((section(".data.user.config"))) = {[0 ... (TCMSIZE-4)/4] = 0xffffffff};

/****************************************************************************
 * Default SSR (Supervisor Status Register) settings                        *
 * Read Q6 Architecture system-level Specification before change            *
 * Incorrect setting of any bit will cause runtime system failure           *
 * bit 0 is LSB                                                             *
 * bit 16:  the privilege mode the SW thread will be running                *
 *          1->user mode (default); 0->supervisor mode                      *
 * bit 22:  BadVa selector.                                                 * 
 *          1->BadVA1; 0->BadVA0 (default)                                  *
 * bit 23:  enable/disable reading PCYCLE register                          *
 *          1->enabled (default); 0->disabled                               *
 * bit 24:  enable/disable reading PMU                                      *
 *          1->enabled (default); 0->disabled                               *
 *                                                                          * 
 *  Default SSR bits from MSB:  0000,0001,1000,0111, 0000,0000,0000,0000    *
 ****************************************************************************/

#ifdef CONFIG_MP
unsigned int QURTK_ssr_default = 0x018f0000; //enable guest os mode initially
#else
unsigned int QURTK_ssr_default = 0x01870000;
#endif


/****************************************************************************
 * Default CCR (Cache Control Register) settings                            *
 *                                                                          *
 *   bits 0-1, 3-4, 6-7 are cache CP bits, and set to 0 to use CCCC attr    *
 *                                                                          *
 *   bit 16:  HFI for L1 I cache hardware prefetch                          *
 *          1->honor the USR[HFI] settings(default); 0->disabled            *
 *   bit 17:  HFD. This bit is reserved in V4.                              *
 *   bit 18:  HFI-L2 for L2 I cache hardware prefetch                       *
 *          1->honor the USR[HFIL2] settings(default); 0->disabled          *
 *   bit 19:  HFD-L2 for L2 D cache hardware prefetch                       *
 *          1->honor the USR[HFDL2] settings(default); 0->disabled          *
 *   bit 20:  SFD for  DCFECTCH/L2FETCH                                     *
 *          1->enalble DCFETCH/L2FETCH based on CCCC and partition(default) *
 *          0->disable DCFETCH and L2FETCH                                  *
 *                                                                          * 
 *  Default CCR bits from MSB:  0000,0000,0001,0101, 0000,0000,0000,0000    *
 ****************************************************************************/
 
unsigned int QURTK_ccr_default __attribute__((section(".data"))) = CACHEATTR;

/****************************************************************************
 * Default USR Cache Prefetch settings                                      *
 *                                                                          *
 *   bit 16-15: HFI.  Two bits for L1 instruction cache prefetching.        *
 *          00: No prefetching                                              *
 *          01: prefetching of at most 1 additional cache line (default)    *
 *          10: prefetching of at most 2 additional cache lines             *
 *   bit 17:  HFD          This bit is reserved in V4.                      *
 *   bit l9-18: HFI-L2. Two bits for L2 prefetching for instructions.       *
 *          00 = Disable L2 instruction prefetching for this thread.        *
 *          01 = Enable L2 instruction prefetching of 1 line.               *
 *          10 = Enable L2 instruction prefetching of 2 lines (default).    *
 *          11 = Enable L2 instruction prefetching of 4 lines.              *
 *   bit 21-20: HFD-L2     This field is reserved.                          *
 *                                                                          * 
 *  Default USR bits from MSB:  0000,0000,0000,0101, 0000,0000,0000,0000    *
 ****************************************************************************/
 
unsigned int QURTK_usr_default __attribute__((section(".data"))) = USRATTR;



/****************************************************************************
 * Configure L2 cache size                                                  *
 *                                                                          *
 * Dependent Features:                                                      *
 *      N/A                                                                 *
 *                                                                          *
 * NOTES:                                                                   *
 *      The value in this variable doesn't have the exact size number       *
 *      in Kilobytes. Instead, this variable has the value that is          *
 *      assigned to register - SYSCFG:L2CFG. Please refer to Hexagon        *
 *      Architecture system-level specification for more information        *
 *      about system register SYSCFG and L2 cache size configuration.       *
 *      The valid L2 cache size is 0KB, 64KB, 128KB, 256KB, 512kB(V4M).     *
 *      The default size is set by Qurt to 128KB.                          *
 *      - 0 (sets 0KB L2 cache size)                                        *
 *      - 1 (sets 64KB L2 cache size)                                       *
 *      - 2 (sets 128KB L2 cache size)                                      *
 *      - 3 (sets 256KB L2 cache size)                                      *
 *      - 4 (sets 512KB L2 cache size)                                      *
 ****************************************************************************/
#define kb_0   0 
#define kb_64  1
#define kb_128 2
#define kb_256 3
#define kb_512 4

unsigned char  QURTK_l2cache_size __attribute__((section(".data"))) = L2SIZE;

/****************************************************************************
 * L2 cache write-back setting                                              *
 *                                                                          *
 * Dependent Features:                                                      *
 *      N/A                                                                 *
 *                                                                          *
 * NOTES:                                                                   *
 *      The L2 cache write-back support is only available on V3+ target.    *
 *      By default, L2 cache is write-through.                              *
 ****************************************************************************/
 
#define ENABLED     1
#define DISABLED    0
unsigned int QURTK_l2cache_wb __attribute__((section(".data"))) = L2CACHEWB;

/****************************************************************************
 * L1 cache size setting                                                    *
 *                                                                          *
 * Dependent Features:                                                      *
 *      N/A                                                                 *
 *                                                                          *
 * NOTES:                                                                   *
 *      Enabling Half Icache or Dcache is only available on V3+ target.     *
 *      By default, Full I/D cache is used.                                 *
 ****************************************************************************/
 
unsigned int QURTK_HI_cache __attribute__((section(".data"))) = DISABLED;
unsigned int QURTK_HD_cache __attribute__((section(".data"))) = DISABLED;

/****************************************************************************
 * L1 cache partition settings                                              *
 *                                                                          *
 * Dependent Features:                                                      *
 *      N/A                                                                 *
 *                                                                          *
 * NOTES:                                                                   *
 *      The L1 data and instruction caches can be independently             *
 *      partitioned into two regions, a main partition and an auxiliary     *
 *      partition. The partitions are controlled by the ICPART and          *
 *      DCPART RW-bits in SYSCFG register. The feature is only              *
 *      available on v3+ targets.                                           *
 *                                                                          *
 *      The actual size of each cache partition also depends on the L1      *
 *      cache size setting. The following values are allowed for            *
 *      QURTK_DC_partition and QURTK_IC_partition:                        *
 *      - 0 (L1 cache is shared by all threads)                             *
 *      - 1 (L1 cache is split into two equal parts. Both main and          *
 *          auxiliary sections use 1/2 the cache)                           *
 *      - 2 (L1 cache is split into two parts. The main section is 3/4      *
 *          of the capacity, while the auxiliary section uses 1/4)          *
 *      - 3 (reserved)                                                      *
 ****************************************************************************/
#define full          0
#define half          1
#define three_quarter 2
#define resv          3

unsigned char  QURTK_DC_partition __attribute__((section(".data"))) = L1DP; /* data cache partition */

unsigned char  QURTK_IC_partition __attribute__((section(".data"))) = L1IP; /* instruction cache partition */


/****************************************************************************
 * L2 cache partition settings                                              *
 *                                                                          *
 * NOTES:                                                                   *
 *      L2 cache is partitioined into two regions, a main partition and     *
 *      an auxiliary. This are controlled by the L2PART in SYSCFG           *
 *      This feature is only available on v4+ targets.                      *
 *                                                                          *
 *      - 0 (L2 cache is fully shared)                                      *
 *      - 1 (L2 cache is split into two equal parts. Both main and          *
 *          auxiliary sections use 1/2 the cache)                           *
 *      - 2 (L2 cache is split into two parts. The main section is 3/4      *
 *          of the capacity, while the auxiliary section uses 1/4)          *
 *      - 3 (L2 cache is split into two parts. The main section is 7/8      *
 *          of the capacity, while the auxiliary section uses 1/8)          *
 ****************************************************************************/

// reuse the definitions on the above from 0 to 2, and add definition for 7/8
#define seven_eighths    3

unsigned char QURTK_L2_partition __attribute__((section(".data"))) = L2CP; /* L2 cache partition *


/****************************************************************************
 * Chicken bit settings (for HW bug workaround)                             *
 *                                                                          *
 * Dependent Features:                                                      *
 *      CONFIG_CHICKEN_BITS                                                 *
 *                                                                          *
 * NOTES:                                                                   *
 ****************************************************************************/
/* Chicken bit settings */

unsigned int QURTK_chicken_bits_rgdr __attribute__((section(".data"))) = QURTKCHICKENBITSRGDR;
unsigned int QURTK_chicken_bits_acc0 __attribute__((section(".data"))) = QURTKCHICKENBITSACC0;
unsigned int QURTK_chicken_bits_acc1 __attribute__((section(".data"))) = QURTKCHICKENBITSACC1;
unsigned int QURTK_chicken_bits_chicken __attribute__((section(".data"))) = QURTKCHICKENBITSCHICKEN;

/****************************************************************************
 * Qurt Timer or Qtimer definitions                                        *
 *                                                                          *
 * Dependent Features:                                                      *
 *      CONFIG_QTIMERS                                                      *
 *                                                                          *
 * NOTES:                                                                   *
 *      Refer to your target HW reference manual for configuration          *
 *      of RGPT timer. Make sure the timer base address is one of           *
 *      the physical memory pools. Interrupt number does not conflict       *
 *      with RTOS reserved interrupts (check sched_config.h).               *
 ****************************************************************************/
unsigned int QDSP6_QURT_TIMER_BASE = QDSP6QTIMERBASE;
unsigned int QURT_timer_intno = TIMERINTNO;
unsigned int QURT_timer_priority = TIMERPRIORITY; //255;
unsigned int QURT_timer_bitmask = TIMERBITMASK; //0x3f;
unsigned int QURT_timerIST_priority = TIMERISTPRIORITY; //254;
unsigned int QURT_timerIST_bitmask = TIMERISTBITMASK; //0x3f;


/****************************************************************************
 * Qurt Memory Pool Configuration                                          *
 *                                                                          *
 * Dependent Features:                                                      *
 *      CONFIG_DYNAMIC_MEMORY                                               *
 *                                                                          *
 * NOTES:                                                                   *
 *      QURT can have unlimited number of pools and each pool have a name  *
 *      and it can have up to 16 ranges. No gaps are allowed in ranges, e.g *
 *      if RANGE 2 is not defined in a pool the following range definitions,*
 *      e.g RANGE 3, RANGE 4 etc., are ignored                              *
 ****************************************************************************/
#ifdef CONFIG_DYNAMIC_MEMORY
struct phys_mem_pool_config{
    char name[32];
    struct range {
        unsigned int start;
        unsigned int size;
    } ranges[16];
};

struct phys_mem_pool_config pool_configs[] = {
    PHYSPOOLS
    {"",{{0,0}}} //Last physpool marked by empty name
};
#endif /* QURT_DYNAMIC_MEMORY */

/* Number of entries to be locked in TLB */
unsigned int QURTK_tlblock_entries __attribute__((section(".data"))) = TLBLOCKNUM; 

unsigned int config_tlb_first_replaceable = TLBFIRSTREPL;

/*
 * Configure static mappings array to add device memory mappings or shared
 * memory mapping
 *
 * vpn:         virtual page number
 * ppn:         physical page number
 * perm:        permission
 * cached_attr: cacheability
 * pgsize:      page size
 * others:      un-used
 *
 * Note: vpn and ppn has to be number of 4KB
 *
 * Note: please contact Qualcomm Technologies Inc. to verify the change.
 *       Incorrect change will crash the build in run time!
 */

#define R    1
#define W    2
#define RW   3
#define WR   3
#define X    4
#define RX   5
#define XR   5
#define WX   6
#define XW   6
#define RWX  7
#define XWR  7

#define MAIN  0
#define AUX   1

#define MEMORY_MAP(glob, asid, vpn, busattr, perm, cache_attr, pgsize, cache_part, ppn) \
   ( ((unsigned long long)((1 << 31) | (glob << 30) | (busattr << 27) | (asid << 26)|vpn) << 32 ) \
   | ( ((perm << 29) & 0xe0000000) | (1 << 28) | (cache_attr << 24) | (ppn << 1) | pgsize))

unsigned long long QURTK_mmap_table[] __attribute__((weak)) = {
    // Empty1 0x008000- 0x070000 size = 0x068000 (416k)
    MEMORY_MAPS
    0LL // last entry must be zero
};

/****************************************************************************
 * Qurt L1 interrupt Configuration                                         *
 *                                                                          *
 * Dependent Features:                                                      *
 *      N/A                                                                 *
 *                                                                          *
 * NOTES:                                                                   *
 *      Use QURTK_IEL to configure interrupt is                            *
 *      - level triggered (bit value = 0)                                   *
 *      - edge triggered (bit value = 1)                                    *
 *      Use QURTK_IAHL to configure interrupt is                           *
 *      - Active high (or rising edge) (bit value = 1)                      *
 *      - Active low (or falling edge) (bit value = 0)                      *
 *      interrupt 0 is at MSB.                                              *
 ****************************************************************************/
 
/* 0 - Level, 1 - Edge. Interrupt 21 is level triggered for testing purpose */

/* 0 - 23 are edge triggered 
 * 24 - 29 are level triggered
 * 30 - 31 are edge triggered
 */
unsigned int QURTK_IEL =  QURTKIEL; //0xffffff03;

/* 1 - Active High, 0 - Active Low. All bits are 1. Rising edge or active high */
unsigned int QURTK_IAHL = QURTKIAHL; //0xffffffff

/****************************************************************************
 * Qurt L2 interrupt Configuration                                         *
 *                                                                          *
 * Dependent Features:                                                      *
 *      CONFIG_L2_INTERRUPT_ENABLED                                         *
 *                                                                          *
 * NOTES:                                                                   *
 *      Qurt kernel currently support up to 8 SIRC (Second level           *
 *      Interrupt Controller). They shall connects to consecutive           *
 *      first level interrupts. In following template, first SIRC           *
 *      connects to first level interrupt 23. Second SIRC connects          *
 *      to first level interrupt 24                                         *
 ****************************************************************************/
/* QURTK_l2_bases[N] has base address of SIRC that connects to first level interrupt N */
unsigned int QURTK_l2_bases[32] = {
           /* interrupt 0 -> QURTK_l2_bases[0] */
           QURTKSIRCBASES
           };
            
/* QURTK_l2_type[N] defines second level interrupt types, in which each bit
 * sets up the type of a L2 interrupt 
 */
unsigned int QURTK_l2_type[32] = {
           /* interrupt 0 -> QURTK_l2_type[0] 
            * define L2 interrupt 0's type in LSB of QURTK_l2_type[N]
            * 0->level triggered; 1->edge triggered */
            QURTKSIRCTYPE
            };
            
/* QURTK_l2_polarity[N] defines second level interrupt polarity, in which each bit 
 * sets up the polarity of a L2 interrupt 
 */
#if __HEXAGON_ARCH__  == 4
unsigned int QURTK_l2_polarity[32] = {
           /* interrupt 0 -> QURTK_l2_polarity[0] 
            * define L2 interrupt 0's polarity in LSB of QURTK_l2_polarity[N]
            * 0->falling active; 1->rising active */
            QURTKSIRCPOLARITY
            };
#endif

/* Define stack memory for fast interrupt callback */
// TBD: FASTINT_STACKSIZE will become a xml item in next step

unsigned int qurt_fastint_stack_size SECTION("QURTK.INTERRUPT.data") = FASTINTSTACKSIZE;
unsigned char qurt_fastint_stack[FASTINTSTACKSIZE * QURT_CONFIG_MAX_HTHREADS]  SECTION("QURT.FASTINT.data") __attribute__((aligned(32)));

unsigned int QURTK_int_max SECTION("QURTK.INTERRUPT.data") = MAXINT;
unsigned int qdsp6ss_base_addr = QDSP6SSBASE;
char QURTK_l2_interrupt_table[(MAXINT+1) * 16]  SECTION("QURTK.INTERRUPT.data") __attribute__((aligned(16)));


unsigned int QURTK_int_elem_size = 16;
int QURTK_CONTEXT_SIZE = THREAD_CONTEXT_TOTALSIZE;

#ifdef CONFIG_DYNAMIC_MEMORY
unsigned int QURTK_KHEAP_SIZE = KERNELHEAPSIZE;
unsigned int QURTK_heap[KERNELHEAPSIZE/4] __attribute__((aligned(4096))) = {[0 ... (KERNELHEAPSIZE-4)/4] = 0xffffffff};
#endif /* CONFIG_DYNAMIC_MEMORY */

/* Threads */
int QURTK_MAX_THREADS = MAXTHREADS;
int QURTK_MAX_THREADS_IN_TCM = MAXTHREADSINTCM;
unsigned long long QURTK_thread_objs[MAXTHREADS] SECTION("QURTK.CONTEXTS.data"); //Each id has two words
/* Regular TCB storage */
char QURTK_thread_contexts[THREAD_CONTEXT_TOTALSIZE  * (MAXTHREADS - MAXTHREADSINTCM)] SECTION("QURTK.CONTEXTS.data") __attribute__ ((aligned (32)));
char QURTK_thread_contexts_tcm[THREAD_CONTEXT_TOTALSIZE * MAXTHREADSINTCM] SECTION("QURTK.CONTEXTS.TCM.data") __attribute__ ((aligned (32)));

/* Futexes */
unsigned int QURTK_futex_objs[MAXTHREADS*4] SECTION("QURTK.FUTEX.data") __attribute__((aligned(16))) = { 0 };

/* Trace buffer */
#ifdef CONFIG_TRACEBUFFER 
#define TBUF_BUFFER_SIZE TRACESIZE
unsigned int QURTK_trace_buffer_size = TBUF_BUFFER_SIZE;
unsigned long long QURTK_trace_mask SECTION("QURTK.SCHEDULER.data") = ((unsigned long long)QURTKDEFAULTTRACEMASK << 32) | TBUF_BUFFER_SIZE;
#ifndef CONFIG_DYNAMIC_MEMORY
unsigned long long QURTK_trace_memory[(TBUF_BUFFER_SIZE/8)*QURT_CONFIG_MAX_HTHREADS] SECTION("QURTK.TRACEBUFFER.data");
#endif //CONFIG_DYNAMIC_MEMORY
#endif
