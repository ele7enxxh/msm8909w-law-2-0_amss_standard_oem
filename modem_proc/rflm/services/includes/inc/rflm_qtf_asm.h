/*! @brief No operation, useful for inserting delays. */
STATIC_INLINE void nop(void) { 
}


/*! @brief Return the current 64-bit PCYCLEHI:LO register count. 
    SUPERVISOR MODE ONLY. See fw_prof_get_pcycle() for user mode.
*/
STATIC_INLINE uint64 fw_get_pcycle_reg(void) {
  return 0;
}


/*! @brief Hexagon USR register bits. Refer to the Hexagon reference manual.
*/
typedef union
{
  uint32  val;
  struct {
    uint32 OVF   : 1;
    uint32       : 7;
    uint32 LPCFG : 2;
    uint32       : 5;
    uint32 HFI   : 2;
    uint32       : 1;
    uint32 HFIL2 : 2;
    uint32       : 11;
    uint32 PFA   : 1;
  };
} fw_usr_reg_t;


/*! @brief Read the Hexagon USR register.
*/
STATIC_INLINE fw_usr_reg_t fw_get_usr_reg(void)
{
  fw_usr_reg_t reg;  
  reg.val = 0;
  return reg;
} 

/*! @brief Write the Hexagon USR register.
*/
STATIC_INLINE void fw_set_usr_reg(fw_usr_reg_t reg)
{
} 


/*! @brief Data cache line prefetch. */
STATIC_INLINE void dcfetch(void *addr) {
}

/*! @brief Data cache line clean.
    Write the cache line containing "addr" from cache if dirty. */
STATIC_INLINE void dccleana(void *addr) {
}

/*! @brief Data cache line clean and invalidate.
    Write the cache line containing "addr" from cache if dirty, then 
    invalidate that line so it is no longer present in cache. */
STATIC_INLINE void dccleaninva(void *addr) {
}

/*! @brief Data cache line invalidate. 
    Invalidate the cache line containing "addr" from cache, discarding
    its contents if dirty. 
    NOTE: If addr is not cache line aligned, unrelated data may be
          discarded accidentally. */
STATIC_INLINE void dcinva(void *addr) {
}


/*! @brief Data cache line zero-allocate. addr must be 32-byte aligned.
    Clean the cache line if dirty, then set all 32 bytes to 0 and mark dirty. */
STATIC_INLINE void dczeroa(void *addr) {
}


/*! @brief Instruction cache line invalidate. 
    Invalidate the cache line containing "addr" from cache.
    NOTE: If addr is not cache line aligned, unrelated data may be
          discarded accidentally. */
STATIC_INLINE void icinva(void *addr) {
}


/*! @brief Issue an L2 fetch.
 
    This maps directly to the l2fetch() instruction. Refer to the Hexagon
    programmer's reference for details.
 
    For simple linear transfers, you may want to use l2fetch_buffer() instead,
    as it has simpler inputs.
*/
STATIC_INLINE void l2fetch
(
  void *addr,    /*!< (Virtual) address to start fetch */
  uint16 width,  /*!< Width (in bytes) of memory block */
  uint16 height, /*!< Height (number of width-sized blocks) */
  uint16 stride, /*!< Offset to increment after each width-sized block. */
  boolean direction  /*!< 0=row major, 1=column major */
)
{
}


/*! @brief Initiate a fetch from external memory to L2 cache. 
 
    This function does a one-dimensional transfer, and has a length limit of
    65535*32=2097120 bytes.  
    For 2-dimensional transfers, or for longer lengths, use l2fetch().
   
    A length of 0 *may* do a single line fetch, depending on the address alignment. 

    Also see l2fetch_is_active() to check if the fetch is complete.
 
    Refer to the Hexagon programmers manual for details on the l2fetch instruction.
    Note that a thread switch, or triggering another l2fetch, may cancel the current
    fetch operation.
    In general, this should be issued before processing a section of data from
    external memory, and whether the data is available when it is accessed
    is best-effort.
*/
STATIC_INLINE void l2fetch_buffer
(
  void *addr,  /*!< (Virtual) address to start fetch */
  uint32 len   /*!< Length (in bytes) to fetch. */
)
{
} /* l2fetch_buffer() */


/*! @brief Check if l2fetch operation is active.
 
    Return TRUE is an l2fetch is in progress, FALSE otherwise.
    Note that if there was a thread switch, the fetch in progress may not be
    the one which is expected.
 
    Polling for a fetch to be complete is counter-productive - the intended
    use of l2fetch is to run in the background while processing a buffer.
    Polling for completion is no better than doing individual loads to each
    buffer line.
    This function would typically be used only for debug/profiling.
*/
STATIC_INLINE boolean l2fetch_is_active(void)
{
  return (TRUE);
}


/*! @brief Ensure that previous instructions have committed. */
STATIC_INLINE void isync(void) {
}

/*! @brief Force all pending transactions to complete. */
STATIC_INLINE void syncht(void) {
}

/*! @brief A compiler barrier. Inserting this into the code guarantees
    that the compiler will schedule all instructions preceding it
    before any instructions following it.
    It does not necessarily guarantee that those instructions have completed
    (i.e. memory accesses) as seen by a separate CPU.
*/
STATIC_INLINE void compiler_barrier(void) {
}


/*! @brief Wait for the specified number of thread cycles.
    The actual duration is approximate due to effects of DMT and compiler optimization.
    DO NOT use this for precise wait intervals.
*/
STATIC_INLINE void fw_wait_tcycles(uint32 tcycles) {
  while (tcycles--)
  { // Usually the compiler will generate a 2-cycle loop for this, so the wait
    // is overestimated. But for smaller loop counts, it will be unrolled.
  }
}
