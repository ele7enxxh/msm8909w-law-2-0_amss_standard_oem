/*==================================================================
 *
 * FILE:        deviceprogrammer_utils.c
 *
 * DESCRIPTION:
 *   
 *
 *        Copyright © 2008-2013 Qualcomm Technologies Incorporated.
 *               All Rights Reserved.
 *               QUALCOMM Proprietary
 *==================================================================*/

/*===================================================================
 *
 *                       EDIT HISTORY FOR FILE
 *
 *   This section contains comments describing changes made to the
 *   module. Notice that changes are listed in reverse chronological
 *   order.
 *
 *   $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/tools/deviceprogrammer_ddr/src/firehose/deviceprogrammer_utils.c#1 $
 *   $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $
 *
 * YYYY-MM-DD   who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 2013-06-03   ah      Added legal header
 * 2013-05-31   ab      Initial checkin
 */


/*==========================================================================

                               INCLUDE FILES

===========================================================================*/
#include <limits.h>
#include "deviceprogrammer_utils.h"
#include "msm.h"
#include "stringl.h"
#include "crc.h"
 
/*=========================================================================
                            
                            FUNCTION DEFINITIONS

=========================================================================*/

/*===========================================================================

**  Function :  get_time_count

** ==========================================================================
*/
/*!
* 
* @brief
*  Returns the time count since bootup. Internal use only.
*
* @retval
*   a 32 bits counter value as the current value in TIMETICK_CLK register.
* 
* @par Side Effects
*   None
* 
*/
static uint32 get_time_count()
{
  uint32 curr_count;
  uint32 last_count;

  /*Grab current time count*/
  curr_count = HWIO_IN(TIMETICK_CLK);

  /*Keep grabbing the time until a stable count is given*/
  do 
  {
    last_count = curr_count;
    curr_count = HWIO_IN(TIMETICK_CLK);
  } while (curr_count != last_count);
  
  return curr_count;
}

/* returns the time in microseconds */
long get_time()
{
  uint32 curr_time_count = get_time_count();
  
  /* check if the count overflows*/
  /*
  if(curr_time_count < LOG_MAX_COUNT_VAL)
  {
  */
    /*Convert current time count into microseconds*/
    curr_time_count = CALCULATE_TIMESTAMP(curr_time_count);
  /*
  }
  else
  {
    curr_time_count = LOG_TIMESTAMP_OVERFLOW;
  }
  */
  
  return curr_time_count;
}

long get_rollover_time()
{
    return ((long)1000 * (long)LOG_TIMESTAMP_OVERFLOW);
}

void delaySeconds(unsigned int seconds) {
    long delay_time = seconds * MICROSECONDS_PER_SECOND;
    long timer_end_time = get_time() + delay_time;
    while (get_time() < timer_end_time);
}

boolean simpleStrReplace(char* haystack, const char* needle, const char* thorn) {
    char* current_haystack = haystack;
    char* pos;
    SIZE_T len_haystack = 0;
    SIZE_T len_needle   = 0;
    SIZE_T len_thorn    = 0;

    if (haystack == NULL || needle == NULL || thorn == NULL) {
        return FALSE;
    }
    
    len_haystack = strlen(haystack);
    len_needle = strlen(needle);
    len_thorn = strlen(thorn);
    
    if (len_thorn > len_needle) {
        return FALSE;
    }

    while ((pos = strstr(current_haystack, needle)) != NULL) {
        current_haystack = pos;
        memscpy(pos, len_haystack, thorn, len_thorn);
        current_haystack += len_thorn;
        memsmove(current_haystack, len_haystack,
                current_haystack + (len_needle - len_thorn),
                haystack + len_haystack - current_haystack);
    }
    return TRUE;
}

SIZE_T stringToNumber(const char* buffer, boolean *retval) {
    char* endptr;
    SIZE_T number = 0;
    
    if (NULL == retval)
        return number;
    if (*buffer == '\0') {
        *retval = FALSE;
        return number;
    }
    number = SIZE_T_CONVERTER(buffer, &endptr, 0);
    if ('\0' != *endptr || LONG_MIN == number || LONG_MAX == number) {
        *retval = FALSE;
        return number;
    }
    *retval = TRUE;
    return number;
}

static int operatorHigherPrecedenceThan(char left, char right) {
    char operator_string[] = "-+*/";
    char* left_pos = strchr(operator_string, left);
    char* right_pos = strchr(operator_string, right);
    if (left_pos != NULL && right_pos != NULL && left_pos > right_pos) {
        return 1;
    }
    return 0;
}

boolean parseExpression(char* buffer, SIZE_T* result) {
    SIZE_T i = 0;
    char ch = 0;
    boolean num_conversion;
    // The length of this buffer dictates how large a number we
    // can handle in the expression
    char number_buffer[16];
    SIZE_T number_length = 0, number_start = 0, number_end = 0;
    SIZE_T number = 0;

    // Parsing stack
    // We will use the token stack to store chars such as (, +, -, n, *, etc.
    // where n represents a number which must be popped off the number stack
    char postfix_string[16];
    SIZE_T postfix_string_number_list[8];
    char operator_stack[16];
    SIZE_T postfix_string_size = 0,
           postfix_string_number_list_size = 0,
           operator_stack_size = 0;

    // Declare variables and stacks used in calculating the final result
    SIZE_T calculation_stack[8];
    SIZE_T calculation_stack_size = 0;
    SIZE_T operand_first, operand_second;

    enum {
        EXPR_NOT_IN_NUMBER,
        EXPR_IN_NUMBER
    } expr_state;
    expr_state = EXPR_NOT_IN_NUMBER;

    while (1) {
        ch = buffer[i];
        if (expr_state == EXPR_NOT_IN_NUMBER) {
            if (isdigit(ch)) {
                number_start = i;
                expr_state = EXPR_IN_NUMBER;
            }
            else if (ch == '(') {
                operator_stack[operator_stack_size++] = ch;
            }
            else if (ch == ')') {
                while (operator_stack_size > 0) {
                    ch = operator_stack[--operator_stack_size];
                    if (ch == '(') {
                        break;
                    }
                    postfix_string[postfix_string_size++] = ch;
                }
            }
            else if (ch == '+' || ch == '-' || ch == '*' || ch == '/') {
                while (operator_stack_size > 0 && operatorHigherPrecedenceThan(operator_stack[operator_stack_size - 1], ch)) {
                    postfix_string[postfix_string_size++] = operator_stack[--operator_stack_size];
                }
                operator_stack[operator_stack_size++] = ch;
            }
        }
        else if (expr_state == EXPR_IN_NUMBER) {
            if (!isdigit(ch)) {
                number_end = i;
                expr_state = EXPR_NOT_IN_NUMBER;
                // Roll back one position to act as though we have not seen this character
                // This is so that we do not have to duplicate the token handling logic
                // from the EXPR_NOT_IN_NUMBER state
                i--;
                number_length = number_end - number_start;
                if (number_length > sizeof(number_buffer) - 1) {
                    return FALSE;
                }
                memscpy(number_buffer, sizeof(number_buffer), &buffer[number_start], number_length);
                number_buffer[number_length] = '\0';
                number = stringToNumber(number_buffer, &num_conversion);
                if (FALSE == num_conversion)
                    return FALSE;
                postfix_string[postfix_string_size++] = postfix_string_number_list_size + '0';
                postfix_string_number_list[postfix_string_number_list_size++] = number;
            }
        }
        if (ch == '\0') {
            break;
        }
        i++;
    }
    while (operator_stack_size > 0) {
        postfix_string[postfix_string_size++] = operator_stack[--operator_stack_size];
    }

    for (i = 0; i < postfix_string_size; i++) {
        switch (postfix_string[i]) {
            case '/':
                operand_second = calculation_stack[--calculation_stack_size];
                operand_first = calculation_stack[--calculation_stack_size];
                calculation_stack[calculation_stack_size++] = operand_first / operand_second;
                break;

            case '*':
                operand_second = calculation_stack[--calculation_stack_size];
                operand_first = calculation_stack[--calculation_stack_size];
                calculation_stack[calculation_stack_size++] = operand_first * operand_second;
                break;

            case '+':
                operand_second = calculation_stack[--calculation_stack_size];
                operand_first = calculation_stack[--calculation_stack_size];
                calculation_stack[calculation_stack_size++] = operand_first + operand_second;
                break;

            case '-':
                operand_second = calculation_stack[--calculation_stack_size];
                operand_first = calculation_stack[--calculation_stack_size];
                calculation_stack[calculation_stack_size++] = operand_first - operand_second;
                break;

            default:
                calculation_stack[calculation_stack_size++] = postfix_string_number_list[postfix_string[i] - '0'];
                break;
        }
    }
    if (result != NULL && calculation_stack_size > 0) {
        *result = calculation_stack[--calculation_stack_size];
        return TRUE;
    }
    else {
        return FALSE;
    }
}

static int reflect
(
  int data,
  const uint32 len
)
{
    int ref = 0;
    uint32 i;

    for(i=0; i < len; i++) {
        if(data & 0x1) {
            ref |= (1 << ((len - 1) - i));
        }
        data = (data >> 1);
    }

   return ref;
}

/*===========================================================================

FUNCTION  calc_crc32

DESCRIPTION
  This function calculate CRC32 on input data.

DEPENDENCIES
  None

RETURN VALUE
  Returns CRC32 of given data

SIDE EFFECTS
  None

===========================================================================*/
uint32 calc_crc32
(
  const uint8   *data_in,
  const uint32  nbytes_in
)
{
    uint32 k = 8;                   // length of unit (i.e. byte)
    int MSB = 0;
    int gx = 0x04C11DB7;         // IEEE 32bit polynomial
    int regs = 0xFFFFFFFF;       // init to all ones
    int regsMask = 0xFFFFFFFF;   // ensure only 32 bit answer
    int regsMSB = 0;
    uint32 i, j;
    uint8 DataByte;

    if ( (data_in == NULL) || (nbytes_in == 0) )
        return 0;

    for( i=0; i < nbytes_in; i++) {
        DataByte = data_in[i];
        DataByte = reflect(DataByte,8);
        for(j=0; j < k; j++) {
            MSB = DataByte >> (k-1);  // get MSB
            MSB &= 1;                 // ensure just 1 bit
            regsMSB = (regs>>31) & 1; // MSB of regs
            regs = regs<<1;           // shift regs for CRC-CCITT
            if(regsMSB ^ MSB) {       // MSB is a 1
                regs = regs ^ gx;       // XOR with generator poly
            }
            regs = regs & regsMask;   // Mask off excess upper bits
            DataByte <<= 1;           // get to next bit
        }
    }

    regs = regs & regsMask;       // Mask off excess upper bits
    return reflect(regs,32) ^ 0xFFFFFFFF;
}

word calc_crc16_wrapper(byte *data_buf,
                        SIZE_T bytes,
                        word crc_value) {
    SIZE_T bytes_to_crc;
    SIZE_T num_bytes = bytes;
    byte* buf = data_buf;

    // Defines the number of bytes the CRC API can take
    SIZE_T crc_chunk_size = USHRT_MAX;

    while (num_bytes > 0) {
        bytes_to_crc = MIN(crc_chunk_size, num_bytes);
        crc_value = crc_16_step(crc_value, buf, bytes_to_crc);
        buf += bytes_to_crc;
        num_bytes -= bytes_to_crc;
    }
    return crc_value;
}

