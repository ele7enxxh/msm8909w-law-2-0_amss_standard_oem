#===============================================================================
# Copyrigha (c) 2012 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#===============================================================================

import sys
import os, errno
import string
import SCons.Action
from SCons.Script import *

def pl_register(env, name):
   if env.GetOption('clean') or env['IMAGE_NAME'] == 'DEVCFG_IMG':
     return
   list_file = os.path.join(env['BUILD_ROOT'],
                            'platform',
                            'platform_libs_list',
                            'build',
                            env['BUILDPATH'],
                            'libs_list.txt')
   with FileLock(list_file):
      f = open(list_file, "a")
      f.write("%s\n" % name)
      f.close()

def pl_register_default(env, name, location):
   # Only platform libs can register for default libs
   if env.GetOption('clean') or env['IMAGE_NAME'] != 'PLATFORM_LIBS':
      return
   default_list_file = os.path.join(env['BUILD_ROOT'],
                                'platform',
                                'platform_libs_list',
                                'build',
                                env['BUILDPATH'],
                                'libs_list_default.txt')
   if location < 0:
      raise ValueError('pl_register_default called with value:%d' %location )

   with FileLock(default_list_file):
      lines = []

      # Ensure that we dont have a library on that line already
      if (os.path.exists(default_list_file)):
         f = open(default_list_file)
         lines = f.readlines()
         f.close()

         if(len(lines) >= location+1):
            if(lines[location] != '\n'):
                # If this is a duplicate, then the file did not get deleted for
                # some reason. Ignore the call
                if lines[location][:-1] == name:
                    return
                else:
                    raise ValueError('Library %s already exists at location: %d' % (lines[location][:-1], location))

      f = open(default_list_file, "w")

      # Pad the list if required
      if len(lines) < int(location)+1 :
          while len(lines) < location+1:
              lines.extend('\n')

      lines[location] = name + lines[location]
      f.writelines(lines)
      f.close()

def pl_generate_builder(target, source, env):
   if env.GetOption('clean') or env['IMAGE_NAME'] == 'DEVCFG_IMG':
     return
   # Read the default list
   default_list_file = os.path.join(env['BUILD_ROOT'],
                                    'platform',
                                    'platform_libs_list',
                                    'build',
                                     env['BUILDPATH'],
                                    'libs_list_default.txt'
                                    )
   f = open(default_list_file, "r")
   default_libs = f.readlines()
   f.close()

   # Read the list file into a set
   list_file = os.path.join(env['BUILD_ROOT'],
                            'platform',
                            'platform_libs_list',
                            'build',
                            env['BUILDPATH'],
                            'libs_list.txt')
   f = open(list_file, "r")
   lines = f.readlines()
   f.close()

   #Remove duplicates
   libs = list(set(lines))

   # Write output source
   f = open(str(target[0]), "w")
   f.write('#include "platform_libs_list.h"\n')
   f.write('#include "platform_libs.h"\n\n')

   # Default libs first
   for name in default_libs:
      if name != '\n':
         f.write("PL_DEP(%s)\n" % name.strip())

   for name in libs:
      f.write("PL_DEP(%s)\n" % name.strip())

   f.write( '\nstruct platform_lib* (*pl_list[])(void) = {\n')
   for name in default_libs:
      if name != '\n':
         f.write("\tPL_ENTRY(%s),\n" % name.strip())

   for name in libs:
      f.write("\tPL_ENTRY(%s),\n" % name.strip())

   f.write('\t0\n')
   f.write('};\n')
   f.close()

   # Delete the list file so it gets recreated the next time
   os.remove(list_file)
   os.remove(default_list_file)

def pl_generate(env, name):
   env.RequirePublicApi("PLATFORM_LIBS")
   return env.AlwaysBuild(env.PLGenerateBuilder(target = name, source = None))

def exists(env):
   return env.Detect('platform_libs')

def generate(env):
   if env.GetOption('clean'):
     print 'Platform builders CLEANING'
     list_file = os.path.join(env['BUILD_ROOT'],
                              'platform',
                              'platform_libs_list',
                              'build',
                               env['BUILDPATH'],
                               'libs_list.txt')
     if os.path.exists(list_file):
       print '\t removing %s', list_file
       os.remove(list_file)

     default_list_file = os.path.join(env['BUILD_ROOT'],
                                      'platform',
                                      'platform_libs_list',
                                      'build',
                                       env['BUILDPATH'],
                                      'libs_list_default.txt')
     if os.path.exists(default_list_file):
       print '\t removing %s', default_list_file
       os.remove(default_list_file)

   env.AddMethod(pl_register, "PLRegister")
   env.AddMethod(pl_register_default, "PLRegisterDefault")
   env['BUILDERS']['PLGenerateBuilder'] = Builder(name = "PLGenerateBuilder", action = pl_generate_builder)
   env.AddMethod(pl_generate, "PLGenerate")

# BSD Licensed Cross-platform file locking support in Python
# See http://www.evanfosmark.com/2009/01/cross-platform-file-locking-support-in-python/
# License information - http://www.evanfosmark.com/2009/01/cross-platform-file-locking-support-in-python/#comment-1054

class FileLockException(Exception):
    pass

class FileLock(object):
    """ A file locking mechanism that has context-manager support so
        you can use it in a with statement. This should be relatively cross
        compatible as it doesn't rely on msvcrt or fcntl for the locking.
    """

    def __init__(self, file_name, timeout=10, delay=.05):
        """ Prepare the file locker. Specify the file to lock and optionally
            the maximum timeout and the delay between each attempt to lock.
        """
        self.is_locked = False
        self.lockfile = os.path.join(os.getcwd(), "%s.lock" % file_name)
        self.file_name = file_name
        self.timeout = timeout
        self.delay = delay


    def acquire(self):
        """ Acquire the lock, if possible. If the lock is in use, it check again
            every `wait` seconds. It does this until it either gets the lock or
            exceeds `timeout` number of seconds, in which case it throws
            an exception.
        """
        start_time = time.time()
        while True:
            try:
                #Create enclosing folder (if necessary)
                if not os.path.exists(os.path.dirname(self.lockfile)):
                    os.makedirs(os.path.dirname(self.lockfile))
                self.fd = os.open(self.lockfile, os.O_CREAT|os.O_EXCL|os.O_RDWR)
                break;
            except OSError as e:
                if e.errno != errno.EEXIST:
                    raise
                if (time.time() - start_time) >= self.timeout:
                    raise FileLockException("Timeout occured.")
                time.sleep(self.delay)
        self.is_locked = True


    def release(self):
        """ Get rid of the lock by deleting the lockfile.
            When working in a `with` statement, this gets automatically
            called at the end.
        """
        if self.is_locked:
            os.close(self.fd)
            os.unlink(self.lockfile)
            self.is_locked = False


    def __enter__(self):
        """ Activated when used in the with statement.
            Should automatically acquire a lock to be used in the with block.
        """
        if not self.is_locked:
            self.acquire()
        return self


    def __exit__(self, type, value, traceback):
        """ Activated at the end of the with statement.
            It automatically releases the lock if it isn't locked.
        """
        if self.is_locked:
            self.release()


    def __del__(self):
        """ Make sure that the FileLock instance doesn't leave a lockfile
            lying around.
        """
        self.release()
