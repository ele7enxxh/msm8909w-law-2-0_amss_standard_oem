#ifndef REXCBREGISTRY_H
#define REXCBREGISTRY_H
/*==========================================================================

                      REX CALLBACK REGISTRY HEADER FILE

DESCRIPTION
   API for the REX - Real Time Executive Callback Registry API

   This functionality enables client subsystems to register callback 
   functions with REX. 
   These callback functions will be called when certain events occur such as:
     REX Initialization (Ex. CS initialization when REX is first initialized)
     REX Thread Creation
     REX Thread Deletion

Copyright (c) 1990-2009 by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE
$Header: //components/rel/core.mpss/3.5.c12.3/api/kernel/rex/rexcbregistry.h#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $                           

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/02/09   msr     Initial specification of the callback registry APIs
===========================================================================*/


/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
#include "rex.h"


/*===========================================================================
                           DATA DECLARATIONS
===========================================================================*/
/* ------------------------------------------------------------------------
** Declaration of callback functions pointers that can be registered with 
** REX. These callback functions will be called when the following events
** occur:
** a) Clients can register a callback function corresponsding to the 
** (*rex_init_cb_fp) declaration below to be called when REX is first 
** initialized.
** 
** b) Clients can register the callback functions corresponsding to the 
** (*rex_thread_create_cb_fp) and (*rex_thread_destroy_cb_fp) declarations 
** below to be called when a REX thread is either created or destroyed
** respectively.
**
** The return value of these callback functions is merely informational and 
** must meet the following spec:
**    0 - Implies a SUCCESS
**    Non Zero - Implies a failure. The actual value depends on the subsystem
**               which executes the callback function.
** ------------------------------------------------------------------------ */
typedef int (*rex_init_cb_fp)(void*);
typedef int (*rex_thread_create_cb_fp)(rex_tcb_type*);
typedef int (*rex_thread_destroy_cb_fp)(rex_tcb_type*);


/*===========================================================================
                           FUNCTION DECLARATIONS
===========================================================================*/
#ifdef __cplusplus
   extern "C"
   {
#endif
      /*===========================================================================
      FUNCTION rex_get_init_cb_list

      DESCRIPTION
        This function returns the registry (or list) of callback functions which 
        are executed when a REX is initialized. These registered functions will be 
        called before any REX task is started.
      ===========================================================================*/
      const rex_init_cb_fp* rex_get_init_cb_list(void);

      /*===========================================================================
      FUNCTION rex_get_thr_create_cb_list

      DESCRIPTION
        This function returns the registry (or list) of callback functions which 
        are executed when a REX thread is created.
      ===========================================================================*/
      const rex_thread_create_cb_fp* rex_get_thr_create_cb_list(void);

      /*===========================================================================
      FUNCTION rex_get_thr_destroy_cb_list

      DESCRIPTION
        This function returns the registry (or list) of callback functions which 
        are executed when a REX thread is destroyed.

        Note: The thread destroy callback functions (*thread_destroy_cb_fp) will 
        be executed in reverse order of the thread create callback functions.  
      ===========================================================================*/
      const rex_thread_destroy_cb_fp* rex_get_thr_destroy_cb_list(void);

#ifdef __cplusplus
   }
#endif

#ifdef __cplusplus
class REXCBRegistry
{
   /*===========================================================================
   FRIEND METHODS
   ===========================================================================*/
   friend const rex_init_cb_fp* rex_get_init_cb_list(void);
   friend const rex_thread_create_cb_fp* rex_get_thr_create_cb_list(void);
   friend const rex_thread_destroy_cb_fp* rex_get_thr_destroy_cb_list(void);

   public:
      /*===========================================================================
      CONSTRUCTOR(S)
      ===========================================================================*/
      
      /*===========================================================================
      REXCBRegistry

      DESCRIPTION
         This constructor accepts three callback functions that client subsytems
         (ex. Component Services (CS)) can register with REX. Clients should ONLY 
         statically create a REXCBRegistry object and pass in their callback 
         function pointers as specified by this constructor. Clients that do not
         have a need to register a callback of a particular type can pass in NULL.

         The first parameter is a callback function pointer which will be called 
         when a REX is first initialized.

         The second parameter is a callback function pointer which will be called
         when a REX thread is spawned.

         The third parameter is a callback function pointer which will be called 
         when a REX thread is killed.

         Note: The order in which a subsystem can register its needed callback 
         functions cannot be guranteed since all the clients are required to 
         statically create a REXCBRegistry object.
      ===========================================================================*/
      REXCBRegistry(rex_init_cb_fp init_fp, 
                     rex_thread_create_cb_fp thr_create_fp,
                     rex_thread_destroy_cb_fp thr_destr_fp);

      /*===========================================================================
       DESTRUCTOR
      ===========================================================================*/
      ~REXCBRegistry();                     
   private:
      /*===========================================================================
      CLASS CONSTANTS
      ===========================================================================*/
      //Max. number of subsystems that can register callback function sets with REX
      static const unsigned int MAX_SUBSYSTEM_CNT;

      //Class Error Codes
      static const int SUCCESS;
      static const int ERRREGISTRYOVERFLOW;

      /*===========================================================================
      MEMBERS
      ===========================================================================*/
      //Static Members
      static unsigned int initCBCount;
      static unsigned int thrCreateCBCount;
      static unsigned int thrDestroyCBCount;
      
      static rex_init_cb_fp initCBList[];
      static rex_thread_create_cb_fp thrCreateCBList[];
      static rex_thread_destroy_cb_fp thrDestroyCBList[];

      /*===========================================================================
      METHODS
      ===========================================================================*/
      //Helper function to add callback functions to the registry
      static int addREXCBFuncs(rex_init_cb_fp init_fp, 
                                 rex_thread_create_cb_fp thr_create_fp,
                                 rex_thread_destroy_cb_fp thr_destr_fp);
};
#endif //__cplusplus

#endif //REXCBREGISTRY_H
