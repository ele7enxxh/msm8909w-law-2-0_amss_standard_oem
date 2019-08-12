/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

               REX  Callback Registry

GENERAL DESCRIPTION
   This functionality enables client subsystems to register callback 
   functions with REX. 
   These callback functions will be called when certain events occur such as:
     REX Initialization (Ex. CS initialization when REX is first initialized)
     REX Thread Creation
     REX Thread Deletion

INITIALIZATION AND SEQUENCING REQUIREMENTS
  NA

Copyright (c) 2004-2011 by Qualcomm Technologies Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                           EDIT HISTORY FOR FILE
$Header: //components/rel/core.mpss/3.5.c12.3/kernel/rex_core/common/src/rex_cb_registry.cpp#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $                           

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/02/09   msr     Initial implementation of the callback registry APIs
===========================================================================*/


/*=================================================================================================
             INCLUDE FILES FOR MODULE
=================================================================================================*/
#include "rexcbregistry.h"
#include "rex_private.h"

/*=================================================================================================
                     DEFINITIONS AND CONSTANTS
=================================================================================================*/
/* Initialize Class Constants */
const unsigned int REXCBRegistry::MAX_SUBSYSTEM_CNT = 10;
const int REXCBRegistry::SUCCESS = 0x0;
const int REXCBRegistry::ERRREGISTRYOVERFLOW = 0x1;

/* Initialize Class Members */
unsigned int REXCBRegistry::initCBCount = 0;
unsigned int REXCBRegistry::thrCreateCBCount = 0;
unsigned int REXCBRegistry::thrDestroyCBCount = 0;

rex_init_cb_fp REXCBRegistry::initCBList[REXCBRegistry::MAX_SUBSYSTEM_CNT + 1] = {NULL, NULL, NULL, NULL, NULL,
                                                                                  NULL, NULL, NULL, NULL, NULL,
                                                                                  NULL};

rex_thread_create_cb_fp REXCBRegistry::thrCreateCBList[REXCBRegistry::MAX_SUBSYSTEM_CNT + 1] = {NULL, NULL, NULL, NULL, NULL,
                                                                                                NULL, NULL, NULL, NULL, NULL,
                                                                                                NULL};

rex_thread_destroy_cb_fp REXCBRegistry::thrDestroyCBList[REXCBRegistry::MAX_SUBSYSTEM_CNT + 1] = {NULL, NULL, NULL, NULL, NULL,
                                                                                                  NULL, NULL, NULL, NULL, NULL,
                                                                                                  NULL};

/*=================================================================================================
                      FUNCTION DEFINITIONS
=================================================================================================*/
REXCBRegistry::REXCBRegistry(rex_init_cb_fp init_fp, 
                     rex_thread_create_cb_fp thr_create_fp,
                     rex_thread_destroy_cb_fp thr_destr_fp)
{
   boolean flag = rex_check_if_initialized();
   if (FALSE == flag)
   {
      REXCBRegistry::addREXCBFuncs(init_fp, thr_create_fp, thr_destr_fp);
   }
}

REXCBRegistry::~REXCBRegistry()
{}

int REXCBRegistry::addREXCBFuncs(rex_init_cb_fp init_fp, 
                                 rex_thread_create_cb_fp thr_create_fp,
                                 rex_thread_destroy_cb_fp thr_destr_fp)
{
   boolean updateFlag = FALSE;
   unsigned int index = REXCBRegistry::thrDestroyCBCount;

   do
   {
      if(REXCBRegistry::initCBCount >= REXCBRegistry::MAX_SUBSYSTEM_CNT)
      {
         break;
      }

      if(REXCBRegistry::thrCreateCBCount >= REXCBRegistry::MAX_SUBSYSTEM_CNT)
      {
         break;
      }
      
      if(REXCBRegistry::thrDestroyCBCount >= REXCBRegistry::MAX_SUBSYSTEM_CNT)
      {
         break;
      }
      updateFlag = TRUE;
   } while(0);
   
   if(updateFlag)
   {
      if(NULL != init_fp)
      {
         REXCBRegistry::initCBList[REXCBRegistry::initCBCount++] = init_fp;
      }
      
      if(NULL != thr_create_fp)
      {
         REXCBRegistry::thrCreateCBList[REXCBRegistry::thrCreateCBCount++] = thr_create_fp;
      }

      if(NULL != thr_destr_fp)
      {
         while(index)
         {
            index--;
            REXCBRegistry::thrDestroyCBList[index + 1] = REXCBRegistry::thrDestroyCBList[index];
         }
         REXCBRegistry::thrDestroyCBList[0] = thr_destr_fp;
         REXCBRegistry::thrDestroyCBCount++;
      }
      return REXCBRegistry::SUCCESS;
   }   
   return REXCBRegistry::ERRREGISTRYOVERFLOW;
}

const rex_init_cb_fp* rex_get_init_cb_list(void)
{
   return REXCBRegistry::initCBList;
}

const rex_thread_create_cb_fp* rex_get_thr_create_cb_list(void)
{
   return REXCBRegistry::thrCreateCBList;
}

const rex_thread_destroy_cb_fp* rex_get_thr_destroy_cb_list(void)
{
   return REXCBRegistry::thrDestroyCBList;
}
