#ifndef DS_PROFILE_MUTEX_H
#define DS_PROFILE_MUTEX_H
/*===========================================================================
  @file ds_Profile_Mutex.h

  This file defines abstract class for mutex used in ds profile db
  
                Copyright (c) 2013 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2013-06-30 am Created module  

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

    
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
namespace ds
{
  namespace Profile
  {
    class Mutex
    {
      public:        
                
        virtual int Lock() = 0;
        virtual int TryLock() = 0;
        virtual int Release() = 0;

        virtual ~Mutex() = 0;
        
      protected:
        
        Mutex() {};

      private:            

        /* do not define, this is to prevent copying the object */
        Mutex(const Mutex&);
        Mutex& operator=(const Mutex&);        

    }; /* class Mutex */

    /* wrapper to auto release mutex upon leave of context. The mutex still have to be taken 
       prior to creating this wrapper */
    class AutoReleseMutex 
    {
      public:

        AutoReleseMutex(Mutex* _mutex):mutex(_mutex){};
        ~AutoReleseMutex() { mutex->Release();}

      private:
        Mutex* mutex;
    }; /* class AutoReleseMutex */
  } /* namespace Profile */
} /* namespace ds */

inline ds::Profile::Mutex::~Mutex(){}

#endif /* DS_PROFILE_MUTEX_H */
