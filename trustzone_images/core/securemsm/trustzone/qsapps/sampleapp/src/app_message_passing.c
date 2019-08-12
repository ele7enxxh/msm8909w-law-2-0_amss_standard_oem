/*
@file app_message_passing.c
@brief This file is holds tests cases for QSEE message passinge. 

*/
/*===========================================================================
   Copyright (c) 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE
  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsapps/sampleapp/src/app_message_passing.c#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $

# when       who     what, where, why
# --------   ---     ---------------------------------------------------------

===========================================================================*/
#include "qsee_log.h"
#include "qsee_heap.h"
#include "qsee_message.h"

int run_message_passing_test(void)
{
  int retval;

  char* appname = "sampleapp";
  char mod_name[32];
  char* sometext = "Hello this is the message buffer and it is exactly 65 bytes long."; //Null character makes it 66
  int sometext_len = 65+1;

  void* message;
  uint32 message_len;

  void* encapsulated;
  uint32 encapsulated_len;

  void* plaintext;
  uint32 plaintext_len;

  message_len = sometext_len;
  encapsulated_len = sometext_len+160; //Allow room for header and mac. 
  plaintext_len = sometext_len+160;

  message = qsee_malloc(message_len);
  encapsulated = qsee_malloc(encapsulated_len);
  plaintext = qsee_malloc(plaintext_len);

  if(message && encapsulated && plaintext)
  {
    strlcpy(message, sometext, sometext_len);
    memset(encapsulated, 0, encapsulated_len);
    memset(plaintext, 2, plaintext_len);

    retval = qsee_encapsulate_inter_app_message(appname, message, message_len, encapsulated, &encapsulated_len);
    if(retval)
    {
      qsee_free(message);
      qsee_free(encapsulated);
      qsee_free(plaintext);

      QSEE_LOG(QSEE_LOG_MSG_DEBUG, "qsee_encapsulate_inter_app_message() FAILED : %x", retval);
     
      return retval;
    }
    retval = qsee_decapsulate_inter_app_message(mod_name, encapsulated, encapsulated_len, plaintext, &plaintext_len);
    if(retval)
    {
      qsee_free(message);
      qsee_free(encapsulated);
      qsee_free(plaintext);

      QSEE_LOG(QSEE_LOG_MSG_DEBUG, "qsee_decapsulate_inter_app_message() FAILED : %x", retval);
     
      return retval;
    }

    qsee_free(message);
    qsee_free(encapsulated);
    qsee_free(plaintext);
    
    QSEE_LOG(QSEE_LOG_MSG_DEBUG, "qsee message passing tests PASSED");

    return 0;
  }
  else
  {
    // free memory if allocation was partial
    if(message)
      qsee_free(message);
	if(encapsulated)
      qsee_free(encapsulated);
	if(plaintext)
      qsee_free(plaintext);

    QSEE_LOG(QSEE_LOG_MSG_DEBUG, "qsee malloc fail - message passing FAILED");
    return -1;
  }

}


