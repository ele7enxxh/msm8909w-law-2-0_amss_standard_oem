/**
@file tzbsp_log.c
@brief Logging for Trustzone
*/
/*===========================================================================
   Copyright (c) 2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/kernel/hypervisor/src/hyp_log.c#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $


when       who      what, where, why
--------   ---      ------------------------------------
12/16/12   mg       Support new log buffer management
09/26/11   mm       Moved the log ring buffer into the diag table
04/29/10   ssm      Created

===========================================================================*/


/*===========================================================================

          INCLUDE FILES

============================================================================*/

#include <comdef.h>
#include <stdio.h>
#include <string.h>  /* for memset */
#include "hyp_asm.h"
#include "hyp_target.h"
#include "hyp_log.h"

/*=========================================================================== 
    Preprocessor Definitions and Constants
============================================================================*/
#define JTAG_TERM_MAX_NUM_RETRY  5
#define JTAG_TERM_TIME_OUT       "#0x10000"

#define MAXBUF (sizeof(long int) * 8)        /* enough for binary */
#define isdigit(d) ((d) >= '0' && (d) <= '9')
#define Ctod(c) ((c) - '0')

#define HYP_DEFAULT_LOG_SIZE     4096
/*===========================================================================
          TYPES
============================================================================*/
typedef struct hyp_log_pos_s
{
  /* Ring buffer wrap-around ctr */
  uint16 wrap;
  
  /* Ring buffer current position */
  uint16 offset;
} hyp_log_pos_t;


/* Boot Info Table */
typedef struct hyp_diag_boot_info_s
{
  /* Warmboot entry CPU Counter */
  uint32 warm_entry_cnt;

  /* Warmboot exit CPU Counter */
  uint32 warm_exit_cnt;
} hyp_diag_boot_info_t;

typedef struct hyp_diag_log_s
{
  /* Magic Number */
  uint32                  magic_num;

  /* Number of CPU's */
  uint32                  cpu_count;

  /* Boot Info */
  hyp_diag_boot_info_t    boot_info[NUM_CPUS];

  /* Ring Buffer Offset */
  uint32                  ring_off;

  /* Ring buffer position mgmt */
  hyp_log_pos_t   log_pos;
  uint32          log_len;
  
  /* Ring buffer pointer */
  char                    *log_buf_p;
} hyp_diag_log_t;

typedef struct hyp_excep_reg_log_type_s
{
   uint32 hifar, hdfar, dfar, ifar, hpfar, ifsr, ttbcr, elr_hyp;
   uint32 r0, r1, r2, r3;
   uint32 r4, r5, r6, r7;
   uint32 r8, r9, r10, r11;
   uint32 r12;
} hyp_excep_reg_log_type;

/*===========================================================================

          LOCAL VARIABLES

============================================================================*/

/* Flag indicating whether logging has been initialized (i.e. enabled) */
static boolean hyp_logging_initialized = FALSE;

/* Mutex to ensure only one CPU logs at a time */
static hyp_mutex_t log_lock;

/* Transient values for managing current log buffer */
static hyp_diag_log_t *g_pHyp_log;

static boolean hyp_allow_log          = TRUE;

/* Needed CMU junk. */
typedef unsigned long vm_offset_t;
typedef int boolean_t;
volatile char input_port, output_port;
boolean_t   _doprnt_truncates = FALSE;

static char hyp_log_area[HYP_DEFAULT_LOG_SIZE];

/*===========================================================================

          FUNCTIONS

============================================================================*/
void printnum(
    register unsigned long  u,
    register int        base,
    void            (*putc)( char ch, vm_offset_t fake),
    vm_offset_t     putc_arg)
{
    char    buf[MAXBUF];    /* build number here */
    register char * p = &buf[MAXBUF-1];
    static char digs[] = "0123456789abcdef";

    do {
        *p-- = digs[u % base];
        u /= base;
    } while (u != 0);

    while (++p != &buf[MAXBUF])
        (*putc)(*p, putc_arg);
}

void _doprnt(
    register char   *fmt,
    va_list     *argp,
                    /* character output routine */
    void        (*putc)( char, vm_offset_t),
    int     radix,      /* default radix - for '%r' */
    vm_offset_t putc_arg)
{
    int     length;
    int     prec;
    boolean_t   ladjust;
    char        padc;
    long        n;
    unsigned long   u;
    int     plus_sign;
    int     sign_char;
    boolean_t   altfmt, truncate;
    int     base;
    register char   c;

    while ((c = *fmt) != '\0') {
        if (c != '%') {
        (*putc)(c, putc_arg);
        fmt++;
        continue;
        }

        fmt++;

        length = 0;
        prec = -1;
        ladjust = FALSE;
        padc = ' ';
        plus_sign = 0;
        sign_char = 0;
        altfmt = FALSE;

        while (TRUE) {
        c = *fmt;
        if (c == '#') {
            altfmt = TRUE;
        }
        else if (c == '-') {
            ladjust = TRUE;
        }
        else if (c == '+') {
            plus_sign = '+';
        }
        else if (c == ' ') {
            if (plus_sign == 0)
            plus_sign = ' ';
        }
        else
            break;
        fmt++;
        }

        if (c == '0') {
        padc = '0';
        c = *++fmt;
        }

        if (isdigit(c)) {
        while(isdigit(c)) {
            length = 10 * length + Ctod(c);
            c = *++fmt;
        }
        }
        else if (c == '*') {
        length = va_arg(*argp, int);
        c = *++fmt;
        if (length < 0) {
            ladjust = !ladjust;
            length = -length;
        }
        }

        if (c == '.') {
        c = *++fmt;
        if (isdigit(c)) {
            prec = 0;
            while(isdigit(c)) {
            prec = 10 * prec + Ctod(c);
            c = *++fmt;
            }
        }
        else if (c == '*') {
            prec = va_arg(*argp, int);
            c = *++fmt;
        }
        }

        if (c == 'l')
        c = *++fmt; /* need it if sizeof(int) < sizeof(long) */

        truncate = FALSE;

        switch(c) {
        case 'b':
        case 'B':
        {
            register char *p;
            boolean_t     any;
            register int  i;

            u = va_arg(*argp, unsigned long);
            p = va_arg(*argp, char *);
            base = *p++;
            printnum(u, base, putc, putc_arg);

            if (u == 0)
            break;

            any = FALSE;
            while ((i = *p++) != 0) {
            /* NOTE: The '32' here is because ascii space */
            if (*p <= 32) {
                /*
                 * Bit field
                 */
                register int j;
                if (any)
                (*putc)(',', putc_arg);
                else {
                (*putc)('<', putc_arg);
                any = TRUE;
                }
                j = *p++;
                for (; (c = *p) > 32; p++)
                (*putc)(c, putc_arg);
                printnum((unsigned)( (u>>(j-1)) & ((2<<(i-j))-1)),
                    base, putc, putc_arg);
            }
            else if (u & (1<<(i-1))) {
                if (any)
                (*putc)(',', putc_arg);
                else {
                (*putc)('<', putc_arg);
                any = TRUE;
                }
                for (; (c = *p) > 32; p++)
                (*putc)(c, putc_arg);
            }
            else {
                for (; *p > 32; p++)
                continue;
            }
            }
            if (any)
            (*putc)('>', putc_arg);
            break;
        }

        case 'c':
            c = va_arg(*argp, int);
            (*putc)(c, putc_arg);
            break;

        case 's':
        {
            register char *p;
            register char *p2;

            if (prec == -1)
            prec = 0x7fffffff;  /* MAXINT */

            p = va_arg(*argp, char *);

            if (p == (char *)0)
            p = "";

            if (length > 0 && !ladjust) {
            n = 0;
            p2 = p;

            for (; *p != '\0' && n < prec; p++)
                n++;

            p = p2;

            while (n < length) {
                (*putc)(' ', putc_arg);
                n++;
            }
            }

            n = 0;

            while (*p != '\0') {
            if (++n > prec)
                break;

            (*putc)(*p++, putc_arg);
            }

            if (n < length && ladjust) {
            while (n < length) {
                (*putc)(' ', putc_arg);
                n++;
            }
            }

            break;
        }

        case 'o':
            truncate = _doprnt_truncates;
        case 'O':
            base = 8;
            goto print_unsigned;

        case 'd':
            truncate = _doprnt_truncates;
        case 'D':
            base = 10;
            goto print_signed;

        case 'u':
            truncate = _doprnt_truncates;
        case 'U':
            base = 10;
            goto print_unsigned;

        case 'x':
            truncate = _doprnt_truncates;
        case 'X':
            base = 16;
            goto print_unsigned;

        case 'z':
            truncate = _doprnt_truncates;
        case 'Z':
            base = 16;
            goto print_signed;

        case 'r':
            truncate = _doprnt_truncates;
        case 'R':
            base = radix;
            goto print_signed;

        case 'n':
            truncate = _doprnt_truncates;
        case 'N':
            base = radix;
            goto print_unsigned;

        print_signed:
            n = va_arg(*argp, long);
            if (n >= 0) {
            u = n;
            sign_char = plus_sign;
            }
            else {
            u = -n;
            sign_char = '-';
            }
            goto print_num;

        print_unsigned:
            u = va_arg(*argp, unsigned long);
            goto print_num;

        print_num:
        {
            char    buf[MAXBUF];    /* build number here */
            register char * p = &buf[MAXBUF-1];
            static char digits[] = "0123456789abcdef";
            char *prefix = 0;

            if (truncate) u = (long)((int)(u));

            if (u != 0 && altfmt) {
            if (base == 8)
                prefix = "0";
            else if (base == 16)
                prefix = "0x";
            }

            do {
            *p-- = digits[u % base];
            u /= base;
            } while (u != 0);

            length -= (&buf[MAXBUF-1] - p);
            if (sign_char)
            length--;
            if (prefix)
            length -= strlen(prefix);

            if (padc == ' ' && !ladjust) {
            /* blank padding goes before prefix */
            while (--length >= 0)
                (*putc)(' ', putc_arg);
            }
            if (sign_char)
            (*putc)(sign_char, putc_arg);
            if (prefix)
            while (*prefix)
                (*putc)(*prefix++, putc_arg);
            if (padc == '0') {
            /* zero padding goes after sign and prefix */
            while (--length >= 0)
                (*putc)('0', putc_arg);
            }
            while (++p != &buf[MAXBUF])
            (*putc)(*p, putc_arg);

            if (ladjust) {
            while (--length >= 0)
                (*putc)(' ', putc_arg);
            }
            break;
        }

        case '\0':
            fmt--;
            break;

        default:
            (*putc)(c, putc_arg);
        }
    fmt++;
    }

}

/* @copydoc tzbsp_log_put */
void hyp_log_put_ch(const char c)
{
  if(g_pHyp_log != NULL)
  {
    g_pHyp_log->log_buf_p[g_pHyp_log->log_pos.offset] = c;
    g_pHyp_log->log_pos.offset = (g_pHyp_log->log_pos.offset + 1) % g_pHyp_log->log_len;

    if (0 == g_pHyp_log->log_pos.offset)
      ++g_pHyp_log->log_pos.wrap;
  }
}

void term_put(char character)
{
  if(hyp_allow_log)
  {
    hyp_log_put_ch(character); /* Log to ring buffer. */
  }
#if 0
  if(g_tzbsp_log_target & TZBSP_LOG_TARGET_JTAG_BIT)
  {
    tzbsp_jtag_put(character); /* Log to JTAG term.view. */
  }
#endif
}

void char_out (char c)
{
  term_put (c);
}

static void
cnputc (char ch, vm_offset_t bogus)
{
  if (ch == '\n')
    char_out ('\r');

  char_out (ch);
}

void hyp_vprintf(const char* fmt, va_list *plistp)
{
  _doprnt((char *)fmt, plistp, cnputc, 16, 0);
}

/* @copydoc hyp_put_log_into_buffer */
void hyp_put_log_into_buffer(const char* fmt, va_list *pList)
{
  if(hyp_logging_initialized)
  {      
    uint64          mask;

    if (NULL == fmt || NULL == g_pHyp_log) return;

    /* ISRs use logging function, hence the ring buffer and JTAG accesses are
     * intlocked. */
    mask = hyp_int_disable_all();

    /* Grab the logging mutex, incase multiple CPUs are attempting to LOG
     * (i.e. multiple FIQs being serviced with debug info) 
     */
    hyp_mutex_lock(&log_lock);  
   
    /* Log the message */
    hyp_vprintf(fmt, pList);

    /* Done updating shared info */
    hyp_mutex_unlock(&log_lock);
    
    hyp_int_restore(mask);
  }
}


/* @copydoc hyp_log */
void hyp_log(uint32 pri, const char* fmt, ...)
{
  va_list args;

  if (g_pHyp_log->ring_off)
  {
    return;
  }

  va_start(args, fmt);
  hyp_put_log_into_buffer(fmt, &args);
  va_end(args);
}

/* @copydoc hyp_log_init */
void hyp_log_init(void)
{
  uint32* addr_p; 
  uint32* size_p;

  if(!hyp_logging_initialized)
  {
    /* Marking logging enabled and initialize the mutex */
    hyp_logging_initialized = TRUE;

    hyp_mutex_init(&log_lock);

    addr_p = (uint32 *)IMEM_HYP_LOG_ADDR;
    size_p = (uint32 *)IMEM_HYP_LOG_SIZE;

    //if (*addr_p == NULL)
    {
       *addr_p = (uint32)hyp_log_area; 
    }

    //if (*size_p == NULL)
    {
       *size_p = (uint32)HYP_DEFAULT_LOG_SIZE;
    }

    if (*size_p > sizeof(hyp_diag_log_t) )
    {
       g_pHyp_log = (hyp_diag_log_t *)(uint32)(*addr_p);
       g_pHyp_log->log_len   = HYP_DEFAULT_LOG_SIZE - HYP_DEFAULT_LOG_HDR_SIZE;
       g_pHyp_log->log_buf_p = (char *)((char*)g_pHyp_log + HYP_DEFAULT_LOG_HDR_SIZE);
    }
    else
    {
       g_pHyp_log->ring_off = TRUE;
  }
    g_pHyp_log->magic_num = HYP_DIAG_MAGIC_NUM;
  } /* if(!hyp_logging_initialized) */
}

void hyp_milestone(uint32 code)
{
  HYP_LOG_CODE(HYP_MSG_DEBUG, code);
}

void hyp_log_warmboot_entry(uint32 cpu)
{
   if (cpu < NUM_CPUS)
   {
      g_pHyp_log->boot_info[cpu].warm_entry_cnt++;
   }
   else
   {
       HYP_LOG_ERR("WaEn:(%u)",cpu);
   }
}

void hyp_log_warmboot_exit(uint32 cpu)
{
   if (cpu < NUM_CPUS)
   {
      g_pHyp_log->boot_info[cpu].warm_exit_cnt++;
   }
   else
   {
      HYP_LOG_ERR("Ex:(%u)",cpu);
   }
}

void hyp_log_exception(uint32 ex, uint32 addr)
{
   switch (ex)
   {
   case HYP_LOG_EC_PREFETCH_HANDLER:
      HYP_LOG_ERR("Exception Prefetch add %x.", addr);
      break;
   case HYP_LOG_EC_ABORT_HANDLER:
      HYP_LOG_ERR("Exception Abort add %x.", addr);
      break;
   case HYP_LOG_EC_TRAP_HANDLER:
      HYP_LOG_ERR("Exception Trap(%x).", addr);
      break;
   case HYP_LOG_EC_UND_HANDLER:
   case HYP_LOG_EC_SVC_HANDLER:
   case HYP_LOG_EC_FIQ_HANDLER:
   case HYP_LOG_EC_IRQ_HANDLER:
   default:
      HYP_LOG_ERR("Exception %u(%x).", ex, addr);
      break;

   }
}

void hyp_log_registers(uint32 *ptr)
{
   hyp_excep_reg_log_type *ctx_p=(hyp_excep_reg_log_type *)ptr;
   HYP_LOG_ERR("reg_log ptr(%X)\n", ptr);

   HYP_LOG_ERR("r0(%X),r1(%X),r2(%X),r3(%X),r4(%X),r5(%X),r6(%X),r7(%X)",
               ctx_p->r0, ctx_p->r1, ctx_p->r2, ctx_p->r3,
               ctx_p->r4, ctx_p->r5, ctx_p->r6, ctx_p->r7);
  
   HYP_LOG_ERR("r8(%X),r9(%X),r10(%X),r11(%X),r12(%X),HIFAR(%X),HDFAR(%X),DFAR(%X)",
               ctx_p->r8, ctx_p->r9, ctx_p->r10, ctx_p->r11,
               ctx_p->r12, ctx_p->hifar, ctx_p->hdfar, ctx_p->dfar);

   HYP_LOG_ERR("IFAR(%X),HPFAR(%X),IFSR(%X),TTBCR(%X),ELR_HYP(%X)\n",
               ctx_p->ifar, ctx_p->hpfar, ctx_p->ifsr, ctx_p->ttbcr, ctx_p->elr_hyp);
}
