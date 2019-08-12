/*=======================================================================
* FILE:        flash_log.c
*
* SERVICES:    Routines to support flash logging
*
* DESCRIPTION :
*
* Copyright (c) 2010, 2015 Qualcomm Technologies Incorporated.
* All Rights Reserved.
* QUALCOMM Confidential and Proprietary
*=======================================================================*/

/*===================================================================
*
*                       EDIT HISTORY FOR FILE
*
*   This section contains comments describing changes made to the
*   module. Notice that changes are listed in reverse chronological
*   order.
*
*   $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/flash/tools/src/nor/flash_log.c#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $
*
* when         who     what, where, why
* ----------   ---     ----------------------------------------------
* 01/28/15     sb      Fix Klocwork errors
* 09/10/10     sv      Initial version
*==================================================================*/

/*===================================================================
*
*                     Include Files
*
====================================================================*/

#include "comdef.h"
#include "string.h"
#include "stdarg.h"

#define FLASH_LOG_UNSIGNED 0
#define FLASH_LOG_SIGNED  1
#define FLASH_LOG_SIZE (sizeof(long int) * 8) /* char array size for number 
                                                format printing */
#define FLASH_LOG_DIGIT_TO_ASCII(digit) ((digit > 9)? \
  (digit + 'a' - 10) : (digit + '0' ))                                 

/* The following structure is used to pass the parameters from the
 * process_and_print function to the print_number_format function 
 */
struct print_number_params 
{  
  char padding; /* used to pass char to be used as padding eg '0' or ' ' */
  int padding_length; /* length of the padding based on %<padding> eg %5d */
  int is_left_aligned; /* flag to set left vs right alignment */
  int is_signed; /* to print signed vs unsigned number in ascii */
  int base; /* the base of the number system being printed eg 0xa for %d */
};

static void print_number_format(uint32, struct print_number_params *);
int jprintf(const char*, ...);
static void putc(char);

static int char_count = 0; /* to handle the return value of printf */

/*
 * The following function looks through the string passed and prints all 
 * chars that are not used after a dlimiter ie '%' directly and processes the 
 * chars after a delimiter to prints the additional args passed in the va_args
 * it ensures proper formating sign and padding as specified.

 * Examples:
 * 1) %x or %X prints the number pased as the corresponding arg as a hex number
 * 2) %o or %O prints the number pased as the corresponding arg as an octal 
 *    number
 * 3) %d or %D prints the number pased as the corresponding arg as a 
 *    signed decimal number.
 * 4) %ld is same as %d as the size of int is same as long int
 * 5) %s prints a string
 * 6) %6d  %5x etc ensure padded printing ie spaces are printed to the right 
 * 7) %06 puts zero padding to the left
 * 8) %- aligns to the left
 */
static int process_and_print(register char  *string,va_list  *arguments)
{  
   /* Declarations go here */
  int string_length, counter, extra, is_number_format, ret_val;
  uint32 arg_int = 0;
  struct print_number_params param;
  static char dlimit = '%';
  char error[] = "\n\nError-flash_log.c- Unsupported parameter detected!! - '%";
  char *p;

  char_count = 0;
  is_number_format = 0;
  string_length = 0;
  param.padding = ' ';  /* defines various types of padding */
  param.padding_length =  0;  /* length of padding */
  param.is_left_aligned = 0;   /* left alligment true vs false */

  string_length = strlen(string);
  
  if(string_length == 0) 
  {
    return char_count; /* handle empty strings */
  }
  
  for(counter = 0; counter < string_length; counter++)
  {
    char ch = *(string + counter);
    
    if(ch != dlimit)
    {
      putc(ch);
      continue;
    }

    counter++;
    ch = *(string + counter);

    param.padding = ' ';
    param.padding_length = 0;
    param.is_left_aligned = 0;

    if(ch == '-')
    {
      param.is_left_aligned = 1;
      counter++;
      ch = *(string + counter);
    }
    
    if(ch == '0') 
    {
      param.padding = '0';
      counter++;
      ch = *(string + counter);
    }

    while(ch >= '0' && ch <= '9')
    {
      param.padding_length = (param.padding_length * 10) + (ch - '0');
      counter++;
      ch = *(string + counter);
    }
    
    if(ch == 'l') 
    {
      counter++;
      ch = *(string + counter);
    }
    
    switch(ch)
    {
      case 'x' :
      case 'X' :
        arg_int = va_arg(*arguments, unsigned long);
        param.base = 16;
        param.is_signed = FLASH_LOG_UNSIGNED;
        is_number_format = 1;
        break;

      case 'o' :
      case 'O' :
        arg_int = va_arg(*arguments, unsigned long);
        param.base = 8;
        param.is_signed = FLASH_LOG_UNSIGNED;
        is_number_format = 1;
        break;

      case 'D' :
      case 'd' :
        arg_int = va_arg(*arguments, long);
        param.base = 10;
        param.is_signed = FLASH_LOG_SIGNED;
        is_number_format = 1;
        break;

      case 'S' :
      case 's' :
        p = va_arg(*arguments, char *);
        extra = param.padding_length-strlen(p);
        is_number_format = 0;
        
        if(!param.is_left_aligned)
        {  
          while(extra > 0)
          {
            putc(' ');
            extra--;
          }
        }
        
        while (*p != '\0')
        {
          putc(*p);
          p++;
        }
        
        if(param.is_left_aligned)
        {
          while(extra > 0)
          {
            putc(' ');
            extra--;
          }
        }
        break;
        
      default: 
        /* Unsupported % format - Printing error message */
        ret_val = char_count;
        p = error;
        
        while (*p != '\0')
        {
          putc(*p);
          p++;
        }
        
        putc(ch);
        putc('\'');
        putc('\n');
        putc('\n');
        return ret_val;          
    } /* switch(ch) */
    
    if (is_number_format == 1)
    {
      print_number_format(arg_int,&param);
    }
  }

  return char_count;
}

/*
 * Printing the number based on the base for correct format with correct 
 * formating/alignment,padding and sign
 */
static void print_number_format(register uint32 number,
  struct print_number_params * param)
{
  char ch;
  long signed_number = 0;
  char number_string[FLASH_LOG_SIZE + 1];
  register uint32 curr = FLASH_LOG_SIZE;
  register int remainder;
  
  if((param->is_signed))
  {
    signed_number = ((long) number);
    
    if(signed_number < 0)
    {
      number = (unsigned long) (-signed_number);
    }
  }

  number_string[curr--] = '\0';

  while(1)
  {
    remainder = number % (param->base);
    remainder = FLASH_LOG_DIGIT_TO_ASCII(remainder);
    number_string[curr--] = remainder;
    number /= (param->base);
    (param->padding_length )--;
    
    if(number <= 0) 
    {
      break;
    }    
  }
  
  if((param->padding == '0') && (param->is_signed))
  {  
    if (signed_number < 0) 
    {
      putc('-');
    }
  }

  if(signed_number < 0) 
  {
    (param->padding_length)--;
  }
  
  if(!(param->is_left_aligned))
  {  
    while(param->padding_length > 0) 
    {
      putc(param->padding);
      (param->padding_length)--;
    }
  }
  
  if((param->padding == ' ') && (param->is_signed))
  {  
    if (signed_number < 0)
    {
      putc('-');
    }
  }

  curr++;
  ch = number_string[curr];
  
  while(ch != '\0')
  {
    putc(ch);
    ch = number_string[++curr];
  }
  
  if(param->is_left_aligned)
  {
    while(param->padding_length > 0)
    {
      putc(' ');
      (param->padding_length)--;
    }
  }
}

/*
 * Printing (to console)
 */
#ifdef USE_BUFFERED_TERMIO

extern volatile char input_port[], output_port[];
extern void term_blip(void);
static int out_count;

static void term_flush(void)
{  
  if(out_count > 0)
  {
    output_port[0] = out_count;
    out_count = 0;
    term_blip();
  }
}

static void char_out(char c)
{  
  if(out_count > 252)
  {
    term_flush();
  }

  output_port[out_count + 4] = c;
  out_count++;
}

/*  prints the char and increments the count */
static void putc_jprinf(char ch)
{  
  if (ch == '\n')
  {
    char_out('\r');
    char_out('\n');
    term_flush();
  }
  else
  {
    char_out(ch);
  }
  char_count++;
}

#else  /* not USE_BUFFERED_TERMIO */

extern void term_put(char);
volatile char input_port, output_port;

static void char_out(char c)
{
  term_put(c);
}

/* prints the char and increments the count */
static void putc_jprinf(char ch)
{  
  if (ch == '\n')
  {
    char_out('\r');
  }
  char_out(ch);
  char_count++;
}

#endif /* USE_BUFFERED_TERMIO */

/* wraper for putc_jprintf */
static void putc(char ch)
{
  putc_jprinf(ch);
}

int jprintf(const char* string, ...)
{
  int return_value;
  va_list arguments;
  va_start(arguments, string);
  return_value = process_and_print((char *) string, &arguments);
  va_end(arguments);
  return return_value;
}
