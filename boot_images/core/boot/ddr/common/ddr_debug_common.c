/**
 * @file boot_ddr_debug.c
 * @brief
 * Implementation for DDR Debug Mode.
 */
/*==============================================================================
                                EDIT HISTORY

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/boot/ddr/common/ddr_debug_common.c#1 $
$DateTime: 2015/09/01 00:30:35 $
$Author: pwbldsvc $
================================================================================
when       who     what, where, why
--------   ---     -------------------------------------------------------------
04/14/15   yps     Add ddr_error_report
04/14/15   yps     Add Per byte scan, DQ training
11/26/14   yps     Porting code to 8909 platform
05/11/14   yps     Porting code to 8916 platform
09/17/13   sl      Added more DDR tuning options.
09/13/13   sl      Fixed pslew/nslew order.
08/29/13   sl      Use USB APIs instead of Boot APIs.
08/20/13   sl      Added DDR tuning.
08/07/13   sr      Added watchdog reset support.
06/18/13   sl      Initial version.
================================================================================
                     Copyright 2013 Qualcomm Technologies Incorporated
                              All Rights Reserved
                     Qualcomm Confidential and Proprietary
==============================================================================*/
/*==============================================================================
                                  INCLUDES
==============================================================================*/
#include "stdio.h"
#include "string.h"
#include "qhsusb_al_bulk.h"
#include "ddr_common.h"
#include "ddr_drivers.h"
#include "ddr_test.h"
#include "ddr_debug_common.h"
/*==============================================================================
                                  MACROS
==============================================================================*/
extern uint32 failures[4];

/*==============================================================================
                                  DATA
==============================================================================*/
/* Echo and message buffers: must be addressable by USB and 4-byte aligned */
__attribute__((aligned(4)))
char rx_buf[BUFFER_LENGTH], tx_buf[BUFFER_LENGTH+1], msg_buf[BUFFER_LENGTH+1] = {0};

static volatile uint32 min_base = 0;

/* Number of bytes to be echoed */
static uint32 bytes_to_echo;

/* Indicator whether receiving callback function is called */
static boolean is_rx_called = FALSE;

uint32 DDR_TEST_SIZE = 1024;
int32 scan_start =100;
int32 scan_end =900;
int32 scan_step = 10;
int32 set_rout = 4;
extern int32  vref;
char str[BUFFER_LENGTH];
char str_error_log[BUFFER_LENGTH]={0};

/* Debug command of DDR Debug Mode */
static struct
{
  char     cmd[CMD_MAX_LENGTH+1];
  uint32   idx;
  boolean  ready;
} debug_cmd = {{0}, 0, FALSE};

/* DDR tuning pattern */
static const uint32 ddr_tuning_pattern[] =
{
  0xDEADBEEF, 0x5A5A5A5A, 0xA5A5A5A5, 0xFEEDFACE,
  0xCAFEBABE, 0xA5A5A5A5, 0x5A5A5A5A, 0x0BADF00D,

  0xA5A5A5A5, 0x5A5A5A5A, 0xA5A5A5A5, 0x5A5A5A5A,
  0x0F0F0F0F, 0xF0F0F0F0, 0x00000000, 0xFFFFFFFF,

  0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF,
  0x00000000, 0xFFFFFFFF, 0x00000000, 0x00000000,
};

/**
 * @brief function defect test
 *        name function test name
 *        fp   test function pointer
 */
typedef struct  {
    char *name;
    boolean (*fp)(const uint32, const uint32);
}funcDefectTest_s;

/* DDR tuning cookies */
__attribute__((section("DDR_DEBUG_TUNING_COOKIE")))
static void (*volatile reset_cb_func)(boolean) = NULL;

/*==============================================================================
                                  FUNCTIONS
==============================================================================*/
#ifdef SEC_WDOG_ENABLE
void ddr_debug_force_reset( void )
{
  ddr_debug_dog_init(FALSE);
  ddr_debug_dog_enable(TRUE);

  /* Wait for watchdog reset */
  for (; ;) {}
}
#endif

boolean qatoi(const char* pstr,int* data)  
{  
    int Ret_Integer = 0;  
    int Integer_sign = 1;  
    boolean result = FALSE;    
    if(pstr == NULL)  
    {  
        ddr_debug_output("Pointer is NULL\n");  
        return 0;  
    }  
    while((*pstr) == ' ')  
    {  
        pstr++;  
    }  

    if(*pstr == '-')  
    {  
        Integer_sign = -1;  
    }  
    if(*pstr == '-' || *pstr == '+')  
    {  
        pstr++;  
    }  
      
    while(*pstr >= '0' && *pstr <= '9')  
    {  
        Ret_Integer = Ret_Integer * 10 + *pstr - '0';  
        pstr++; 
		result = TRUE;
    }  
    Ret_Integer = Integer_sign * Ret_Integer;  
    *data = Ret_Integer;
    return result;  
}  


uint32 parse_input(const char* input_point,int32 *input_data)
{
  
  char string_buf[10];
  uint8 i=0;
  uint8 j =0;
  if(input_point == NULL)  
    {  
        ddr_debug_output("Pointer is NULL\n");  
        return 0;  
    }  
	j = 0;
	i = 0;
	memset(string_buf,0,10);
	
	while(*input_point!=0)
	{
    	if(*input_point>='0'&&*input_point<='9')
    	{
    	  string_buf[i]=*input_point;
    	  i++;
    	}
    	else
    	{
    	  if(string_buf[0]!=0)
    	 	{
    	     if(qatoi((const char*)string_buf,&input_data[j])!= TRUE)
			   return 0;
    	     i = 0;
    	     j ++;
    	     memset(string_buf,0,10);
    	 	}
    	}
    	input_point++;
	}
	if(string_buf[0]!=0)
		 	{
		     if(qatoi((const char*)string_buf,&input_data[j])!= TRUE)
			 	return 0;
		     j ++;
		 	}
	return j;
}

/* =============================================================================
**  Function : ddr_debug_rx_cb
** =============================================================================
*/
/**
*   @brief
*   Callback for transportation layer receiving.
*
*   @param[in]  bytes_read  Number of bytes read
*   @param[in]  err_code    Error code returned
*
*   @retval  None
*
*   @dependencies
*   None
*
*   @sideeffects
*   None
*
*   @sa
*   None
*/
void ddr_debug_rx_cb(uint32 bytes_read, uint32 err_code)
{
  uint32 read, write;

  if (!bytes_read)  { return; }

  for (read = 0, write = 0; read < bytes_read/sizeof(char); read++)
  {
    if (rx_buf[read] == '\r')
    {
      debug_cmd.cmd[debug_cmd.idx] = '\0';
      debug_cmd.idx = 0;
      debug_cmd.ready = TRUE;

      tx_buf[write++] = '\r';
      tx_buf[write++] = '\n';

      break;
    }
    else if (rx_buf[read] == '\b' || rx_buf[read] == '\x7F')
    {
      if (debug_cmd.idx > 0)
      {
        debug_cmd.idx--;

        tx_buf[write++] = rx_buf[read];
      }
    }
    else if ( rx_buf[read] >= 'a' && rx_buf[read] <= 'z' ||
              rx_buf[read] >= 'A' && rx_buf[read] <= 'Z' ||
              rx_buf[read] >= '0' && rx_buf[read] <= '9' ||
              rx_buf[read] >= ' ' && rx_buf[read] <= '\x2f')
    {
      if (debug_cmd.idx < CMD_MAX_LENGTH)
      {
        debug_cmd.cmd[debug_cmd.idx++] = rx_buf[read];

        tx_buf[write++] = rx_buf[read];
      }
    }
  }

  bytes_to_echo = write * sizeof(char);

  is_rx_called = TRUE;

} /* ddr_debug_rx_cb */

/* =============================================================================
**  Function : ddr_debug_tx_cb
** =============================================================================
*/
/**
*   @brief
*   Callback for transportation layer transmitting.
*
*   @param[in]  bytes_written  Number of bytes written
*   @param[in]  err_code       Error code returned
*
*   @retval  None
*
*   @dependencies
*   None
*
*   @sideeffects
*   None
*
*   @sa
*   None
*/
 void ddr_debug_tx_cb(uint32 bytes_written, uint32 err_code)
{
} /* ddr_debug_tx_cb */

/* =============================================================================
**  Function : ddr_debug_input
** =============================================================================
*/
/**
*   @brief
*   Get input command for DDR Debug Mode.
*
*   @param  None
*
*   @retval  Input command
*
*   @dependencies
*   None
*
*   @sideeffects
*   None
*
*   @sa
*   None
*/
const char *ddr_debug_input( void )
{
  uint32 err_code;

  debug_cmd.ready = FALSE;

  do
  {
    /* Receive user's inputs */
    qhsusb_al_bulk_receive( (byte *)rx_buf, sizeof(rx_buf),
                            ddr_debug_rx_cb, &err_code );

    while (!is_rx_called) {
      qhsusb_al_bulk_poll();
    }
    is_rx_called = FALSE;

    /* Echo user's inputs */
    qhsusb_al_bulk_transmit( (byte *)tx_buf, bytes_to_echo,
                              ddr_debug_tx_cb, &err_code );
  } while (!debug_cmd.ready);

  return debug_cmd.cmd;

} /* ddr_debug_input */

/* =============================================================================
**  Function : ddr_debug_output
** =============================================================================
*/
/**
*   @brief
*   Output message for DDR Debug Mode.
*
*   @param[in]  msg  Message to be output
*
*   @retval  None
*
*   @dependencies
*   None
*
*   @sideeffects
*   None
*
*   @sa
*   None
*/
void ddr_debug_output(const char *msg)
{
  uint32 err_code;

  strncpy(msg_buf, msg, BUFFER_LENGTH);

  qhsusb_al_bulk_transmit( (byte *)msg_buf, strlen(msg_buf) * sizeof(char),
                           ddr_debug_tx_cb, &err_code );

} /* ddr_debug_output */

boolean ddr_debug_test(uint32 *ddr_base, uint32 failures[4])
{
  uint32 idx;
  uint32 result;

  result = 0;

  for (idx = 0; idx < sizeof(ddr_tuning_pattern)/sizeof(uint32); idx++)
  {
    ddr_base[idx] = 0;
  }
  
  for (idx = 0; idx < sizeof(ddr_tuning_pattern)/sizeof(uint32); idx++)
  {
    ddr_base[idx] = ddr_tuning_pattern[idx];
  }

  for (idx = 0; idx < sizeof(ddr_tuning_pattern)/sizeof(uint32); idx++)
  {
    result |= ddr_base[idx] ^ ddr_tuning_pattern[idx];
	if(result!=0)
	{
	 snprintf(str_error_log,BUFFER_LENGTH, "[Patten test]write at address:0x%x, error data:0x%08x, expect data:0x%08x\r\n", 
  	   	                                               idx, ddr_base[idx],ddr_tuning_pattern[idx]);
	 break;
	}
  }

  for (idx = 0; idx < 4; idx++)
  {
    if ( result & (0xFF << (idx*8)) )  { failures[idx]++; }
  }
  if(result!=0)
  	return FALSE;
  else
  	return TRUE;

} /* ddr_debug_test */


boolean ddr_function_defect_test_bit_flip_tuning(const uint32 base, const uint32 size, uint32 failures[4])
{
  uint32 * buff = (uint32 *)base;
  const uint8 UINT32_LEN = 32;
  const uint8 LOOP_CYCLE = 2;
  const uint32 ONE = 0x1;
  uint8 k = 0, j = 0;
  uint32 i = 0;
  uint32 temp_value1 = 0;
  uint32 temp_value2 = 0;
  uint32 expect_value = 0;
  uint32 result = 0;
  uint32 idx = 0;
  uint32  * p1, * p2;
  uint32 cmp_sz = size >> 1;

  for (j = 0; j < UINT32_LEN; ++j)
  {
    uint32 val = ONE << j;	
   
    for (k = 0; k < LOOP_CYCLE; ++k)
    {
     val = ~val;	
     p1 = buff;
     p2 = buff + cmp_sz;
     for (i = 0; i < cmp_sz; ++i)
       *p1++ = *p2++ = (i % 2) == 0 ? val : ~val;

#ifdef SEC_WDOG_ENABLE
  	ddr_debug_dog_kick();
#endif
        /* verify */
	p1 = buff;
	p2 = buff + cmp_sz;
      for (i = 0; i < cmp_sz; ++i)
      {
	      // ddr_debug_output(" verify\r\n");
	       expect_value = (i % 2) == 0 ? val : ~val;
		   temp_value1 = p1[i];
		   temp_value2 = p2[i];
	       if ((temp_value1  != temp_value2) || (expect_value != temp_value1) || (expect_value != temp_value2))
		   {
		          memset(str_error_log,0,BUFFER_LENGTH);
		         // ddr_debug_output(" compare\r\n");
		          if(temp_value1 != expect_value)
		          {
			          if((temp_value1 == p1[i])&& (temp_value1 == p1[i])&& (temp_value1 == p1[i]))
					  {
					    snprintf(str_error_log,BUFFER_LENGTH, "[Bitflip]zone1 write failed at address:0x%x, error data:0x%08x, expect data:0x%08x\r\n", 
			  	   	                                               (p1+i), p1[i],expect_value);
				  	  }
				      else
	 			      {
	 				    snprintf(str_error_log,BUFFER_LENGTH, "[Bitflip]zone1 read failed at address:0x%x, error data:0x%08x, expect data:0x%08x\r\n", 
	 		   	                                               (p1+i), temp_value1,expect_value);
	 			      }
		          }
				  
			      if(temp_value2 != expect_value)
		          {
		            if((temp_value2 == p2[i])&& (temp_value2 == p2[i])&& (temp_value2== p2[i]))
				    {
		         	  snprintf(str_error_log,BUFFER_LENGTH, "[Bitflip]zone2 write failed at address:0x%x, error data:0x%08x, expect data:0x%08x\r\n", 
		  	   	                                               (p2+i), p2[i],expect_value);
				    }
				    else
				    {
					  snprintf(str_error_log,BUFFER_LENGTH, "[Bitflip]zone2 read failed at address:0x%x, error data:0x%08x, expect data:0x%08x\r\n", 
			   	                                               (p2+i), temp_value2,expect_value);
				    }
		          } 
		  	      result |= (p1[i] ^ expect_value) |(p2[i] ^ expect_value);
  		  	   
  		        for (idx = 0; idx < 4; idx++)
  		        {
  		              if ( result & (0xFF << (idx*8)) )  { failures[idx]++; }
  		        }				  
			    return FALSE;
		     }
				  					
	   }
      }
#ifdef SEC_WDOG_ENABLE
      ddr_debug_dog_kick();	
#endif
    }
	
  return TRUE;
}

void ddr_debug_tune_cdc(boolean in_progress)
{
  /* Install reset callback function */
  reset_cb_func = &ddr_debug_tune_cdc;

  /* Do tuning */
  ddr_debug_do_tuning(in_progress, FALSE,FALSE);

  /* Uninstall reset callback function */
  reset_cb_func = NULL;
#ifdef SEC_WDOG_ENABLE
  /* Force watchdog reset */
  ddr_debug_force_reset();
#endif
} /* ddr_debug_tune_cdc */

void ddr_debug_tune_all(boolean in_progress)
{
  /* Install reset callback function */
  reset_cb_func = &ddr_debug_tune_all;

  /* Do tuning */
  ddr_debug_do_tuning(in_progress, TRUE,FALSE);

  /* Uninstall reset callback function */
  reset_cb_func = NULL;
#ifdef SEC_WDOG_ENABLE
  /* Force watchdog reset */
  ddr_debug_force_reset();
#endif
} /* ddr_debug_tune_all */


void ddr_debug_dq_training(boolean in_progress)
{
  /* Install reset callback function */
  reset_cb_func = &ddr_debug_tune_cdc;

  /* Do tuning */
  ddr_debug_do_tuning(in_progress, FALSE,TRUE);

  /* Uninstall reset callback function */
  reset_cb_func = NULL;
#ifdef SEC_WDOG_ENABLE
  /* Force watchdog reset */
  //ddr_debug_force_reset();
#endif
} /* ddr_debug_tune_cdc */

/* =============================================================================
**  Function : ddr_function_defect_test_solid_bits
** =============================================================================
*/
/**
 * @brief DDR function defect test API
 *        Write the data 0x00000000 and 0x11111111 alternately to the tested 
 *  memory area, and read back to check if the same value is fetched.
 *
 *
 * @param[in] base  test memory base start pointer
 * @param[in] size  the type of space used to be test
 *
 * @return TRUE test pass, FALSE test fail
 *
 *   @sideeffects
 *   Memory is corrupted after this function is called. Error is logged if any.
 */
boolean ddr_function_defect_test_solid_bits(uint32 base, uint32 size)
{
  volatile uint32 * base_addr = (uint32 *)base;
  const uint32 UINT32_ZERO = 0x0;
  const uint8 LOOP_CYCLES = 64;
  uint8  j = 0;
  uint32 i = 0;
  uint32 result =0;
  uint8 idx;
  for (j = 0; j < LOOP_CYCLES; ++j)
  {
    uint32 val = (j % 2) == 0 ? (~UINT32_ZERO) : UINT32_ZERO;
  	 
    for (i = 0; i <= size; i++)
    {  
      base_addr[i] = (i % 2) == 0 ? val : ~val;
    }
	#ifdef DEFECT_LOG_ENABLE
    if(j == LOOP_CYCLES / 2)	
      ddr_debug_output("\r\n"); 
    else
      ddr_debug_output("."); 
    #endif
    /* verify */
    for (i = 0; i <= size; i++)
    {			
      if (base_addr[i] != ((i % 2) == 0 ? val : ~val))
      {
        #ifdef DEFECT_LOG_ENABLE
  	    snprintf(str,BUFFER_LENGTH, "\r\naddress:0x%08x, error data:0x%08x, expect data:0x%08x\r\n", 
  	   	                                               base_addr + i, base_addr[i], ((i % 2) == 0 ? val : ~val));
	
		#endif
		result |= (base_addr[i] ^ ((i % 2) == 0 ? val : ~val));
  		  	   
  	     for (idx = 0; idx < 4; idx++)
  		        {
  		              if ( result & (0xFF << (idx*8)) )  { failures[idx]++; }
  		        }
		 #ifndef DEFECT_LOG_ENABLE
		 snprintf(str_error_log,BUFFER_LENGTH, "[solid bits]address:0x%08x, error data:0x%08x,expect data:0x%08x\r\n", 
  	   	                                               base_addr + i, base_addr[i], ((i % 2) == 0 ? val : ~val));
		 #else
		 ddr_debug_output(str); 
		 #endif
        return FALSE;
      }
    }
  }
  return TRUE;
}

/* =============================================================================
**  Function : ddr_function_defect_test_check_board
** =============================================================================
*/
/**
 * @brief DDR function defect test API
 *        CheckBoard test on the memory test size given
 *        with partten (0x55555555, 0xAAAAAAAA)
 *
 * @param[in] base  test memory base start pointer
 * @param[in] size  the type of space used to be test
 *
 * @return TRUE test pass, FALSE test fail
 *
 *   @sideeffects
 *   Memory is corrupted after this function is called. Error is logged if any.
 */
boolean ddr_function_defect_test_check_board(uint32 base, uint32 size)
{
  volatile uint32 * base_addr = (uint32 *)base;
  const uint32 CHECK_BOARD = 0x55555555;
  const uint8 LOOP_CYCLES = 64;
  uint8  j = 0;
  uint32 i = 0;
  uint32 result =0;
  uint8 idx;
  for (j = 0; j < LOOP_CYCLES; ++j)
  {
    uint32 val = (j % 2) == 0 ? (~CHECK_BOARD) : CHECK_BOARD;
    for (i = 0; i <= size; i++)
    {						
        base_addr[i] = (i % 2) == 0 ? val : ~val;
    }
  	#ifdef DEFECT_LOG_ENABLE
    if(j == LOOP_CYCLES / 2)	
      ddr_debug_output("\r\n"); 
    else
   	  ddr_debug_output("."); 	
    #endif
      /* verify */
    for (i = 0; i <= size; i++)
    {			
     if (base_addr[i] != ((i % 2) == 0 ? val : ~val))
     {
       #ifdef DEFECT_LOG_ENABLE
   	   snprintf(str,BUFFER_LENGTH, "address:0x%08x, error data:0x%08x, expect data:0x%08x\r\n", 
  	   	                                               base_addr + i, base_addr[i], ((i % 2) == 0 ? val : ~val));
       ddr_debug_output(str);
	   #endif
	   result |= (base_addr[i] ^ ((i % 2) == 0 ? val : ~val));
  		  	   
  	     for (idx = 0; idx < 4; idx++)
  		        {
  		              if ( result & (0xFF << (idx*8)) )  { failures[idx]++; }
  		        }
       #ifndef DEFECT_LOG_ENABLE
	   snprintf(str_error_log,BUFFER_LENGTH, "[check_board]address:0x%08x, error data:0x%08x, expect data:0x%08x\r\n", 
  	   	                                               base_addr + i, base_addr[i], ((i % 2) == 0 ? val : ~val));
	   #endif
	   return FALSE;
     }
    }
  }

  return TRUE;
}

/* =============================================================================
**  Function : ddr_function_defect_test_bit_spread
** =============================================================================
*/
/**
 * @brief DDR function defect test API
 *        BitSpread test on the memory test size given
 *        with address:   00000101b -> 00001010b -> 00010100b  -> ...
 *             address++: 11111010b -> 11110101b -> 11101011b -> ...
 *             ...      :
 *        shmoo
 * @param[in] base  test memory base start pointer
 * @param[in] size  the type of space used to be test
 *
 * @return TRUE test pass, FALSE test fail
 *
 * @sideeffects
 * Memory is corrupted after this function is called. Error is logged if any.
 */
static boolean ddr_function_defect_test_bit_spread(const uint32 base, const uint32 size)
{
  volatile uint32 * base_addr = (uint32 *)base;
  const uint8 UINT32_LEN = 32;
  const uint32 ONE = 0x1;
  uint8  j = 0;
  uint32 i = 0;
  uint32 result =0;
  uint8 idx;
  for (j = 0; j < UINT32_LEN * 2; ++j)
  {
    uint32 val = 0;

   if (j < UINT32_LEN)            
   	val = (ONE << j) | (ONE << (j + 2));   
   else            
   	val = (ONE << (UINT32_LEN * 2 - 1 - j)) |(ONE << (UINT32_LEN * 2 + 1 - j));
   
    for (i = 0; i <= size; i++)
    {
        base_addr[i] = (i % 2) == 0 ? val : ~val;
    }
#ifdef  DEFECT_LOG_ENABLE 
    if(j == UINT32_LEN)	
      ddr_debug_output("\r\n"); 
    else
   	  ddr_debug_output("."); 	
#endif  		 
    /* verify */
    for (i = 0; i <= size; i++)
    {
      if (base_addr[i] != ((i % 2) == 0 ? val : ~val))
      {
        #ifdef DEFECT_LOG_ENABLE
   	    snprintf(str,BUFFER_LENGTH, "address:0x%08x, error data:0x%08x, expect data:0x%08x\r\n", 
  	   	                                               base_addr + i, base_addr[i], ((i % 2) == 0 ? val : ~val));
		ddr_debug_output(str); 
		#endif
		result |= (base_addr[i] ^ ((i % 2) == 0 ? val : ~val));
  		  	   
  	     for (idx = 0; idx < 4; idx++)
  		        {
  		              if ( result & (0xFF << (idx*8)) )  { failures[idx]++; }
  		        }
	   #ifndef DEFECT_LOG_ENABLE
	   snprintf(str_error_log,BUFFER_LENGTH, "[bit_spread]address:0x%08x, error data:0x%08x, expect data:0x%08x\r\n", 
  	   	                                               base_addr + i, base_addr[i], ((i % 2) == 0 ? val : ~val));
	   #endif
        return FALSE;
      }
    }
  }

   return TRUE;
}

/* =============================================================================
**  Function : ddr_function_defect_test_bit_flip
** =============================================================================
*/
/**
 * @brief DDR function defect test API
 *        BitFlip test on the memory test size given
 *
 * @param[in] base  test memory base start pointer
 * @param[in] size  the type of space used to be test
 *
 * @return TRUE test pass, FALSE test fail
 *
 *   @sideeffects
 *   Memory is corrupted after this function is called. Error is logged if any.
 */
boolean ddr_function_defect_test_bit_flip(const uint32 base, const uint32 size)
{

  const uint8 UINT32_LEN = 32;
  const uint8 LOOP_CYCLE = 2;
  const uint32 ONE = 0x1;
  uint8 k = 0, j = 0;
  uint32 i = 0;
  uint32 result =0;
  uint8 idx;
  uint32  * p1, * p2;
  uint32 cmp_sz = size >> 1;
  uint32 * buff = (uint32 *)base;
  uint32 temp_value1 = 0;
  uint32 temp_value2 = 0;
  uint32 expect_value = 0;

  for (j = 0; j < UINT32_LEN; ++j)
  {
    uint32 val = ONE << j;	
   
    for (k = 0; k < LOOP_CYCLE; ++k)
    {

   //  ddr_debug_output("LOOP_CYCLE\r\n");
     val = ~val;	
     p1 = buff;
     p2 = buff + cmp_sz;
     for (i = 0; i < cmp_sz; ++i)
         *p1++ = *p2++ = (i % 2) == 0 ? val : ~val;

#ifdef SEC_WDOG_ENABLE
  	ddr_debug_dog_kick();
#endif
        /* verify */
	p1 = buff;
	p2 = buff + cmp_sz;
      for (i = 0; i < cmp_sz; ++i)
      {
	      // ddr_debug_output(" verify\r\n");
	       expect_value = (i % 2) == 0 ? val : ~val;
		   temp_value1 = p1[i];
		   temp_value2 = p2[i];
	       if ((temp_value1  != temp_value2) || (expect_value != temp_value1) || (expect_value != temp_value2))
		   {
		          memset(str_error_log,0,BUFFER_LENGTH);
		         // ddr_debug_output(" compare\r\n");
		          if(temp_value1 != expect_value)
		          {
			          if((temp_value1 == p1[i])&& (temp_value1 == p1[i])&& (temp_value1 == p1[i]))
					  {
					    snprintf(str_error_log,BUFFER_LENGTH, "[Bitflip]zone1 write failed at address:0x%x, error data:0x%08x, expect data:0x%08x\r\n", 
			  	   	                                               (p1+i), p1[i],expect_value);
				  	  }
				      else
	 			      {
	 				    snprintf(str_error_log,BUFFER_LENGTH, "[Bitflip]zone1 read failed at address:0x%x, error data:0x%08x, expect data:0x%08x\r\n", 
	 		   	                                               (p1+i), temp_value1,expect_value);
	 			      }
		          }
				  
			      if(temp_value2 != expect_value)
		          {
		            if((temp_value2 == p2[i])&& (temp_value2 == p2[i])&& (temp_value2== p2[i]))
				    {
		         	  snprintf(str_error_log,BUFFER_LENGTH, "[Bitflip]zone2 write failed at address:0x%x, error data:0x%08x, expect data:0x%08x\r\n", 
		  	   	                                               (p2+i), p2[i],expect_value);
				    }
				    else
				    {
					  snprintf(str_error_log,BUFFER_LENGTH, "[Bitflip]zone2 read failed at address:0x%x, error data:0x%08x, expect data:0x%08x\r\n", 
			   	                                               (p2+i), temp_value2,expect_value);
				    }
		          } 
		  	      result |= (p1[i] ^ expect_value) |(p2[i] ^ expect_value);
  		  	   
  		        for (idx = 0; idx < 4; idx++)
  		        {
  		              if ( result & (0xFF << (idx*8)) )  { failures[idx]++; }
  		        }				  
			    return FALSE;
		     }
				  					
	   }
      }
#ifdef SEC_WDOG_ENABLE
      ddr_debug_dog_kick();	
#endif
    }
  
  return TRUE;
}

/* =============================================================================
**  Function : ddr_function_defect_test_walking_one
** =============================================================================
*/
/**
 * @brief DDR function defect test API
 *        WalkingOne test on the memory test size given
 *        with address:   0000001b -> 0000010b
 *                     -> 0000100b -> 0001000b
 *             ...      :
 *        shmoo
 *
 * @param[in] base  test memory base start pointer
 * @param[in] size  the type of space used to be test
 *
 * @return TRUE test pass, FALSE test fail
 *
 *   @sideeffects
 *   Memory is corrupted after this function is called. Error is logged if any.
 */
boolean ddr_function_defect_test_walking_one(const uint32 base, const uint32 size)
{
  volatile uint32 * base_addr = (uint32 *)base;
  const uint8 UINT32_LEN = 32;
  const uint32 ONE = 0x1;
  uint8  j = 0;
  uint32 i = 0;
  uint32 result =0;
  uint8 idx;
  for (j = 0; j < UINT32_LEN * 2; ++j)
  {
    uint32 val = 0;
    if (j < UINT32_LEN)
      val = (ONE << j) ;
    else
      val = (ONE << (UINT32_LEN * 2 - 1 - j));
  
    for (i = 0; i <= size; i++)
    {
      base_addr[i] = val;
    }
#ifdef DEFECT_LOG_ENABLE  
    if(j == UINT32_LEN)	
      ddr_debug_output("\r\n"); 
    else
      ddr_debug_output("."); 
#endif   
    /* verify */
    for (i = 0; i <= size; i++)
    {
      if (base_addr[i] != val)
      {
        #ifdef DEFECT_LOG_ENABLE
        snprintf(str,BUFFER_LENGTH, "address:0x%08x, error data:0x%08x, expect data:0x%08x\r\n", 
  	   	                                               base_addr + i, base_addr[i], ((i % 2) == 0 ? val : ~val));
        ddr_debug_output(str); 
		#endif
		result |= (base_addr[i] ^ ((i % 2) == 0 ? val : ~val));
  		  	   
  	     for (idx = 0; idx < 4; idx++)
  		        {
  		              if ( result & (0xFF << (idx*8)) )  { failures[idx]++; }
  		        }
		#ifndef DEFECT_LOG_ENABLE
	   snprintf(str_error_log,BUFFER_LENGTH, "[walking_one]address:0x%08x, error data:0x%08x, expect data:0x%08x\r\n", 
  	   	                                               base_addr + i, base_addr[i], ((i % 2) == 0 ? val : ~val));
	   #endif
        return FALSE;
      }
    }
  }

  return TRUE;
}

/* =============================================================================
**  Function : ddr_function_defect_test_walking_zero
** =============================================================================
*/
/**
 * @brief DDR function defect test API
 *        WalkingZero test on the memory test size given
 *        with address:   1111110b -> 1111101b
 *                     -> 1111011b -> 1110111b
 *             ...      :
 *        shmoo
 * @param[in] base  test memory base start pointer
 * @param[in] size  the type of space used to be test
 *
 * @return TRUE test pass, FALSE test fail
 *
 *   @sideeffects
 *   Memory is corrupted after this function is called. Error is logged if any.
 */
boolean ddr_function_defect_test_walking_zero(const uint32 base, const uint32 size)
{
  volatile uint32 * base_addr = (uint32 *)base;
  const uint8 UINT32_LEN = 32;
  const uint32 ONE = 0x1;
  uint8  j = 0;
  uint32 i = 0;
  uint32 result =0;
  uint8 idx;
  for (j = 0; j < UINT32_LEN * 2; ++j)
  {
    uint32 val = 0;
    if (j < UINT32_LEN)
      val = ~(ONE << j) ;
    else
      val = ~(ONE << (UINT32_LEN * 2 - 1 - j));
    for (i = 0; i <= size; i++)
    {
      base_addr[i] = val;
    }
#ifdef DEFECT_LOG_ENABLE  
    if(j == UINT32_LEN)	
      ddr_debug_output("\r\n"); 
    else
   	  ddr_debug_output("."); 
#endif    
    /* verify */
    for (i = 0; i <= size; i++)
    {
      if(base_addr[i] != val)
      {
      #ifdef DEFECT_LOG_ENABLE
   	    snprintf(str,BUFFER_LENGTH, "address:0x%08x, error data:0x%08x, expect data:0x%08x\r\n", 
  	   	                                               base_addr + i, base_addr[i], ((i % 2) == 0 ? val : ~val));
        ddr_debug_output(str);
	  #endif
		result |= (base_addr[i] ^ ((i % 2) == 0 ? val : ~val));
  		  	   
  	     for (idx = 0; idx < 4; idx++)
  		        {
  		              if ( result & (0xFF << (idx*8)) )  { failures[idx]++; }
  		        }
	   #ifndef DEFECT_LOG_ENABLE
	   snprintf(str_error_log,BUFFER_LENGTH, "[walking_one]address:0x%08x, error data:0x%08x, expect data:0x%08x\r\n", 
  	   	                                               base_addr + i, base_addr[i], ((i % 2) == 0 ? val : ~val));
	   #endif
        return FALSE;
      }
    }
  }

  return TRUE;
}
/* =============================================================================
**  Function : pseudo_rand_value_generate
** =============================================================================
*/
/**
 * @brief pseudo_rand_value_generate
 *        use a pseudo method to generate the random data from the seed
 *
 * @param[in] seed
 *
 * @return the random value
 */
uint32 pseudo_rand_value_generate(const uint32 seed)
{
  uint32 randVal = (seed * 123 + 59) % 0xFFFFFFFF;
  return randVal;
}

/* =============================================================================
**  Function : ddr_function_defect_test_rand_val
** =============================================================================
*/
/**
 * @brief DDR function defect test API
 *        ddr_function_defect_test_rand_val will fill the data test area with random 
 *        test data,the 1st data will come from data 0, the remaining data will be 
 *        generated from a pesudo random generation function
 *
 * @param[in] base  test memory base start pointer
 * @param[in] size  the type of space used to be test
 *
 * @return TRUE test pass, FALSE test fail
 *
 * @sideeffects
 * Memory is corrupted after this function is called. Error is logged if any.
 */
boolean ddr_function_defect_test_rand_val(const uint32 base, const uint32 size)
{
  volatile uint32 * base_addr = (uint32 *)base;
  uint32 j = 0;
  uint32 randVal = 0;
  uint32 result =0;
  uint8 idx;
  for (j = 0; j <= size; ++j)
  {
    base_addr[j] = randVal;
    randVal = pseudo_rand_value_generate(randVal);
  }
#ifdef DEFECT_LOG_ENABLE  
  ddr_debug_output("."); 
#endif  
  /* verify */
  randVal = 0;
  for (j = 0; j <= size; ++j)
  {
    if (base_addr[j] != randVal)
    {
    #ifdef DEFECT_LOG_ENABLE
      snprintf(str,BUFFER_LENGTH, "address:0x%08x, error data:0x%08x, expect data:0x%08x\r\n", 
  	   	                                               base_addr + j, base_addr[j], randVal);
      ddr_debug_output(str); 
	#endif
	  result |= (base_addr[j] ^ randVal);
  		  	   
  	     for (idx = 0; idx < 4; idx++)
  		        {
  		              if ( result & (0xFF << (idx*8)) )  { failures[idx]++; }
  		        }
	#ifndef DEFECT_LOG_ENABLE
	   snprintf(str_error_log,BUFFER_LENGTH, "[test_rand]address:0x%08x, error data:0x%08x, expect data:0x%08x\r\n", 
  	   	                                               base_addr + j, base_addr[j], randVal);
	#endif
      return FALSE;
    }
    randVal = pseudo_rand_value_generate(randVal);
  }

  return TRUE;
}

/* =============================================================================
**  Function : ddr_function_defect_test_seq_incr
** =============================================================================
*/
/**
 * @brief DDR function defect test API
 *  The value wrote to the address is increased 
 *  continuously with address increased
 *
 * @param[in] base  test memory base start pointer
 * @param[in] size  the type of space used to be test
 *
 * @return TRUE test pass, FALSE test fail
 *
 *   @sideeffects
 *   Memory is corrupted after this function is called. Error is logged if any.
 */
boolean ddr_function_defect_test_seq_incr(const uint32 base, const uint32 size)
{
  volatile uint32 * base_addr = (uint32 *)base;
  uint32 j = 0;
  uint32 val = 0x0;
  uint32 result =0;
  uint8 idx;
  for (j = 0; j <= size; ++j)
  {
    base_addr[j] = val;
    ++val;
  }
#ifdef DEFECT_LOG_ENABLE  
  ddr_debug_output("."); 
#endif
  
  /* verify */	
  val = 0;
  for (j = 0; j <= size; ++j)
  {
    if (base_addr[j] != val)
    {
    #ifdef DEFECT_LOG_ENABLE
      snprintf(str,BUFFER_LENGTH, "address:0x%08x, error data:0x%08x, expect data:0x%08x\r\n", 
  	   	                                               base_addr + j, base_addr[j], val);
      ddr_debug_output(str); 
	#endif
	  result |= (base_addr[j] ^ val);
  		  	   
  	     for (idx = 0; idx < 4; idx++)
  		        {
  		              if ( result & (0xFF << (idx*8)) )  { failures[idx]++; }
  		        }
	 #ifndef DEFECT_LOG_ENABLE
	 snprintf(str_error_log,BUFFER_LENGTH, "[seq_incr]address:0x%08x, error data:0x%08x, expect data:0x%08x\r\n", 
  	   	                                               base_addr + j, base_addr[j], val);
	 #endif
      return FALSE;
    }
    ++val;
  }

  return TRUE;
}

/* =============================================================================
**  Function : ddr_defect_interval_test_mem_map
** =============================================================================
*/
/**
*   @brief
*   Test if memory size in words is power of 2, and base address is aligned with
*   size.
*
*   Memory size in words is power of 2 if and only if size in bytes is power of
*   2 (i.e., (size & (size-1)) == 0x0) and is no smaller than one word. Base
*   address is aligned with size if and only if (base & (size-1)) == 0x0.
*
*   Notice this function takes memory limit as input, where limit = (size-1).
*
*   @param[in]  base     Base address of memory
*   @param[in]  limit    Maximum offset (in bytes) of memory
*
*   @return TRUE test pass, FALSE test fail
*/

boolean ddr_defect_interval_test_mem_map
(
 uint32 base,
 uint32 limit
)
{
  return ( ((limit + 1) & limit) == 0x0 &&
           limit >= sizeof(uint32) - 1 &&
           (base & limit) == 0x0 );
}

boolean ddr_function_defect_test(const uint32  ddr_base, const uint32 size)
{
  static funcDefectTest_s function_defect_test[] =
  {
    {"Solid bits", ddr_function_defect_test_solid_bits},
    {"Checkerboard", ddr_function_defect_test_check_board},
    {"Bit Spread", ddr_function_defect_test_bit_spread},
    {"Bit Flip", ddr_function_defect_test_bit_flip},
//    {"Walking ones", ddr_function_defect_test_walking_one},
//    {"Walking zeros", ddr_function_defect_test_walking_zero},
//    {"Random value", ddr_function_defect_test_rand_val},
//    {"Seq incr", ddr_function_defect_test_seq_incr},
  };
	
  uint8 ddr_function_defect_test_num = sizeof(function_defect_test) / sizeof(*function_defect_test);
  uint8 i= 0;
  
  for(i=0; i < ddr_function_defect_test_num; i++)
  {    
    if(function_defect_test[i].name && function_defect_test[i].fp)
    {
    #ifdef DEFECT_LOG_ENABLE
      ddr_debug_output(function_defect_test[i].name);
      ddr_debug_output(":\r\n"); 
    #endif
      if(!(function_defect_test[i].fp(ddr_base, size >> 2)))
      {
      	return 	FALSE;
      }
      else
      	{
      	#ifdef DEFECT_LOG_ENABLE
        ddr_debug_output("ok\r\n"); 
		#endif
      	}
    }
   }
	
  return TRUE;
}

/**
 * @brief DDR memory test by step
 *        test method:
 *        Step 0: write/read every double word until whole space looped
 *        Step 1: write/read every second double word until whole space looped
 *                totally 2 cycles.
 *        Step 2: write/read every four double word until whole space looped
 *                totally 4 cycles.
 *        ......
 *        Step n: write/read every 2^n double word until whole space looped
 *                totally 2^n cycles.
 *        NOTE: request at least 40MiB for test, and the memory space size
 *              should be n * 64KiByte
 *
 * @param base test memory space start pointer
 * @param length test memory space length in byte
 * @param stepPower the power of the step number
 * @param sampleSize the repeat times of this step test*
 * @return True test pass, FALSE test failure at read verification
 */
boolean ddr_interval_stress_test_per_step(uint32  const base, const uint32 length,
        const uint32 stepPower, const uint32 sampleSize)
{
  uint32 sampleCycle   = 0;
  uint32 stepStart     = 0;
  uint32 stepCycle     = 0;
  uint32 stepLength    = 0;
  uint32 dwordLength   = 0;
  volatile uint32 *base_addr;
  
  /* init function var*/
  stepLength = 1 << stepPower;
  dwordLength = length >> 2;
  base_addr = (uint32 *) base;
  	 
  /* write speed test by step defined*/
  for (sampleCycle = 0; sampleCycle != sampleSize; ++sampleCycle)
  {
    for (stepStart = 0; stepStart != stepLength; ++stepStart)
    {
      uint32 stepEnd = dwordLength - stepStart;
      for (stepCycle = 0; stepCycle < stepEnd;
          stepCycle += stepLength)
      {
        base_addr[stepStart + stepCycle] = stepStart + stepCycle;
      }
    }
  }	
  
  /* read verification for ram data retention */
  for (stepStart = 0; stepStart != stepLength; ++stepStart)
  {
    uint32 stepEnd = dwordLength - stepStart;
    for (stepCycle = 0; stepCycle < stepEnd;
        stepCycle += stepLength)
    {
      if (base_addr[stepStart + stepCycle] != stepStart + stepCycle)
      {
        snprintf(str,BUFFER_LENGTH, "address:0x%08x, error data:0x%08x, expect data:0x%08x\r\n", 
  	   	                                      base_addr + stepStart + stepCycle, base_addr[stepStart + stepCycle],stepStart + stepCycle);
        ddr_debug_output(str); 
        return FALSE;
      }
    }
  }
  return TRUE;
}

boolean ddr_interval_stress_test(uint32 const base, const uint32 size)
{
  uint32 i = 0;
  uint32 stepShmooBeg = 0;
  uint32 stepShmooEnd = MAX_STEP_POWER;
  
  /* print out related test information */
  snprintf(str,BUFFER_LENGTH, "\r\nMemory Test Space: 0x%x++(0x%x) %%BYTE\r\n"
        "\rInterval step ran ge: 0x%x~0x%x \r\n",
        base, size, stepShmooBeg, stepShmooEnd);
  ddr_debug_output(str);	
  ddr_debug_output("\rstep\t\tResult\r\n"); 
  
  for (i = stepShmooBeg; i != stepShmooEnd; i++)
  {
    if (!(ddr_interval_stress_test_per_step(base, size, i, DEFAULT_SAMPLE_SIZE)))
    {
      return FALSE;
    }
    else
    {
      snprintf(str,BUFFER_LENGTH, "\r\n%d\t\tok\r\n", 1 << i);
      ddr_debug_output(str); 	   	
    }
  }
  return TRUE;
}


boolean ddr_defect_interval_test
(  
  uint32            test_select,
  SDRAM_INTERFACE   interface,
  SDRAM_CHIPSELECT  chip_select
)
{   
  uint32 ddr_base = 0, ddr_size = 0;
  ddr_size_info ddr_info = ddr_get_size();
  
  /* get base address and size */
  if (interface == SDRAM_INTERFACE_0)
  {
    if (chip_select == SDRAM_CS0)
    {
      ddr_base = ddr_info.sdram0_cs0_addr;
      ddr_size = ddr_info.sdram0_cs0;
    }
    else if (chip_select == SDRAM_CS1)
    {
      ddr_base = ddr_info.sdram0_cs1_addr;
      ddr_size = ddr_info.sdram0_cs1;
    }
    else if (chip_select == SDRAM_BOTH)
    {
      ddr_base = (ddr_info.sdram0_cs0_addr < ddr_info.sdram0_cs1_addr) ?
                  ddr_info.sdram0_cs0_addr : ddr_info.sdram0_cs1_addr;
      ddr_size = ddr_info.sdram0_cs0 + ddr_info.sdram0_cs1;
    }
  }
  else if (interface == SDRAM_INTERFACE_1)
  {
    if (chip_select == SDRAM_CS0)
    {
      ddr_base = ddr_info.sdram1_cs0_addr;
      ddr_size = ddr_info.sdram1_cs0;
    }
    else if (chip_select == SDRAM_CS1)
    {
      ddr_base = ddr_info.sdram1_cs1_addr;
      ddr_size = ddr_info.sdram1_cs1;
    }
    else if (chip_select == SDRAM_BOTH)
    {
      ddr_base = (ddr_info.sdram1_cs0_addr < ddr_info.sdram1_cs1_addr) ?
                  ddr_info.sdram1_cs0_addr : ddr_info.sdram1_cs1_addr;
      ddr_size = ddr_info.sdram1_cs0 + ddr_info.sdram1_cs1;
    }
  }
  ddr_size = DDR_TEST_SIZE/1024;    //test 64M 
  /* convert size from megabyte to byte */
  ddr_size <<= 20;
  
  if (ddr_size == 0)
  {
    return FALSE;
  }
  else if (!ddr_defect_interval_test_mem_map(ddr_base, ddr_size-1))
  {
    ddr_debug_output("\r\nThe memory size in words is not power of 2\r\n");
    return FALSE;
  }
  else if (ddr_base + ddr_size > min_base)
  {
    /* adjust base address and size based on minimum base address for test */
    if (ddr_base < min_base)
    {
      ddr_size -= (min_base - ddr_base);
      ddr_base = min_base;
    }	 
  
    ddr_size >>= 2;
  		
  	if(test_select == 0)
  	{
  	  if(!(ddr_function_defect_test(ddr_base, ddr_size - 1)))	
  	    return FALSE;
  	}
  	if (test_select == 1)
  	{
  	  if(!(ddr_interval_stress_test(ddr_base, ddr_size - 1)))
  	    return FALSE;
  	}		
  }
  
  return TRUE;
}

/* =============================================================================
**  Function : ddr_debug
** =============================================================================
*/
/**
*   @brief
*   Command interpreter of DDR Debug Mode.
*
*   @param  None
*
*   @retval  None
*
*   @dependencies
*   Transportation layer (e.g., USB) must be set up, and all data must be moved
*   out of DDR (otherwise would be corrupted).
*
*   @sideeffects
*   DDR could be corrupted.
*
*   @sa
*   None
*/
extern void boot_busywait(uint32 pause_time_us);

extern void qhsusb_dci_write_scratch(uint32 core_id, uint8 offset, uint32 data);
void ddr_debug( void )
{
  static const char ddr_debug_msg_menu[] =
  "--------------------------------------------\r\n"
  "| DDR MUST - Memory Unit Shmoo Test(V1.0.1) |\r\n"
  "|    Copyright 2014 by QUALCOMM Inc.        |\r\n"
  "--------------------------------------------\r\n"
#ifdef SEC_WDOG_ENABLE
  "0: Restart System\r\n"
#endif
  "0: Set DDR Test Parameter [TestSize(1024,1024,65536);ROUT(0,1,7);START_CDC(90,5,1900);END_CDC(90,5,1900);SCAN_STEP(5,5,100)]\r\n"
  "1: DDR DATA Lines and ADD Lines Test (< 1sec)\r\n"
  "2: DDR Read/Write Test: Advanced (< 5min)\r\n"
  "3: Manual CDC Scan: CDC Only (< 30min)[Rout(0,1,7)]\r\n"
  "4: Automatic CDC Scan: All (overnight)\r\n"
  "5: Switch DDR VDDQ: 1.2V~1.3V[VDDQ(1,1,9)]\r\n"
  "6: The DDR Function Defect Test\r\n"
  "7: The DDR Interval Stress Test\r\n"
  "8: DQ Training[ENV(0,1,2)]\r\n"
  "9: FIX RD CDC Scan WR BYTE[Vref(300,10,1200);RDCDC(100,1,1000);Rout(0,1,7);ScanByte(0,1,3)]\r\n"
  "10: FIX WR CDC scan RD CDC[Vref(300,10,1200);WRCDC(100,1,1000);Rout(0,1,7)]\r\n"
  "11: FIX WR and RD CDC Stress Test[Rout(0,1,7)]\r\n";

  const char *cmd;
  int32 cmd_data;
  int32 input_data[10];
  uint8 data_num =0;

  /* Start USB enumeration */
  qhsusb_al_bulk_init(0x0, QHSUSB_MODE__SER_ONLY);

  qhsusb_dci_write_scratch(0, 0, 0x0);

#ifdef SEC_WDOG_ENABLE
  ddr_debug_dog_init(TRUE);
#endif

  memset(str,0,BUFFER_LENGTH);
  memset(str_error_log,0,BUFFER_LENGTH);

  if (reset_cb_func != NULL)
  {
    reset_cb_func(TRUE);
  }

  while (1)
  {
    ddr_debug_output("\r\nPlease press ENTER to continue: ");
    ddr_debug_input();
	boot_busywait(1000000);
    ddr_debug_output(ddr_debug_msg_menu);

    while (1)
    {
      boolean isbreak = TRUE;
	  dq_wr_scan_byte = 0xFFFFFFFF;
      ddr_debug_output("\r\nPlease select an option: ");
      cmd = ddr_debug_input();
	  if(sizeof(cmd))
	  {
      if(qatoi(cmd,&cmd_data))
      {
      if (cmd_data < 0 || cmd_data > 11) { continue; }
      switch(cmd_data)
      {
      #ifdef SEC_WDOG_ENABLE
        case '0':
	    case  0 :
          ddr_debug_output("\r\nRestarting system...\r\n");
          ddr_debug_force_reset();
          break;
      #else
	  	case '0':
		case  0 :
		  ddr_debug_output("\r\nPlease input parameter[DDRTESTSIZE,ROUT,scdc,ecdc,step]:");
		  cmd = ddr_debug_input();
		  data_num=parse_input(cmd,input_data);
		  if(data_num==5)
		  	{
		  	 DDR_TEST_SIZE = input_data[0];
			 set_rout = input_data[1];
			 scan_start = input_data[2];
			 scan_end = input_data[3];
			 scan_step = input_data[4];
		  	}
		  else
		  	{
		  	 ddr_debug_output("\r\n set parameter error\r");
		  	}

		  break;
	  #endif
        case '1':
		case  1 :
          ddr_debug_output("\r\nTesting data/address lines of DDR...\r\n");
          if (ddr_test(DDR_TEST_SUITE_ACCESS, 0, SDRAM_INTERFACE_0, SDRAM_BOTH, 0))
          {
            ddr_debug_output("DDR test passed.\r\n");
          }
          else
          {
            ddr_debug_output("DDR test failed.\r\n");
          }
          break;

        case '2':
		case  2 :
          ddr_debug_output("\r\nTesting all range of DDR by own-address algorithm...\r\n");
          if (ddr_test(DDR_TEST_SUITE_ACCESS, 1, SDRAM_INTERFACE_0, SDRAM_BOTH, 0))
          {
            ddr_debug_output("DDR test passed.\r\n");
          }
          else
          {
            ddr_debug_output("DDR test failed.\r\n");
          }
          break;

        case '3':
		case  3 :

          ddr_debug_cdc_tuning_manually();
          break;

        case '4':
		case  4 :
          ddr_debug_tune_all(FALSE);
          break;
		  
		case '5':
		case  5 :
		  ddr_debug_tuning_vddq();
		  break;
		
		case '6':
		case  6 :
		  ddr_debug_output("\r\nDDR Function Stress Test\r\n");
		  if(ddr_defect_interval_test(0,SDRAM_INTERFACE_0, SDRAM_BOTH))
		  {
			ddr_debug_output("DDR test passed\r\n");
		  }
		  else
		  {
			ddr_debug_output("DDR test failed\r\n");
		  }
		  break;
		case '7':
		case  7 :
		  ddr_debug_output("\r\nDDRInterval Stress Test\r\n");
		  if(ddr_defect_interval_test(1,SDRAM_INTERFACE_0, SDRAM_BOTH))
		  {
			ddr_debug_output("DDR test passed\r\n");
		  }
		  else
		  {
		    ddr_debug_output("DDR test failed\r\n");
		  }
		  break;	
		case '8':
		case  8 :
		  ddr_debug_ddr_training();
		  break;
		case  9 :
		  ddr_debug_output("\r\n");
		  ddr_debug_output("Please input [Vref(300,10,1200);RDCDC(100,10,1000);Rout(0,1,7);ScanByte(0,1,3)]:");
          cmd = ddr_debug_input();
          data_num=parse_input(cmd,input_data);
		  if(data_num == 4)
		  	{
		  	  vref = input_data[0];
		  	  dq_rd_cdc_value = input_data[1];
		      dq_rout = input_data[2];
			  dq_wr_scan_byte = input_data[3];
			  memset(input_data,0,10);
	          ddr_debug_tune_cdc(FALSE);
		  	}
		  else
		  	{
		  	  ddr_debug_output("input failed \n");
		  	}
		  break;
		case 10 :
		  ddr_debug_output("\r\n");
		  ddr_debug_output("Please input [Vref(300,10,1200);RDCDC(100,10,1000);Rout(0,1,7)]:");
          cmd = ddr_debug_input();
          data_num=parse_input(cmd,input_data);
		  if(data_num == 3)
		  	{
		  	  vref = input_data[0];
		  	  dq_wr_cdc_value = input_data[1];
		      dq_rout = input_data[2];
			  memset(input_data,0,10);
	          ddr_debug_tune_cdc(FALSE);
		  	}
		  else
		  	{
		  	  ddr_debug_output("input failed \n");
		  	}
		  break;
		case 11 :
		  dq_rd_cdc_value = dq_wr_cdc_value =496;
		  ddr_debug_cdc_tuning_manually();
        default:
          /* Should not reach here */
          isbreak = FALSE;
		  break;
      }
	  	}
	  else
	  	{
	  	  isbreak = FALSE;
	  	}
	  	}
      if(isbreak) break;
    }
  }
} /* ddr_debug */

void ddr_error_report(void)
{
	  /* Start USB enumeration */
    qhsusb_al_bulk_init(0x0, QHSUSB_MODE__SER_ONLY);

    qhsusb_dci_write_scratch(0, 0, 0x0);
	ddr_debug_output("ERROE:device crash! Please reset the phone!! \n");
}

