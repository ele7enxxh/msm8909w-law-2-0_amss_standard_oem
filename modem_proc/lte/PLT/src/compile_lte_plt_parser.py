# Py2Exe version 0.6.3 setup file for console programs.
# If this is a windows GUI application replace the last line with
# windows = [{"script": 'myFile.py'}] )
#
# Enter the file name of your own .py code file in the last line, lets say it's test1.py
# so the last line should be: console = [{"script": 'test1.py'}] )
# then save this program as   p2e_test1.py  to the same directory where your code file is located
# Now run p2e_test1.py ...
#
# Two subfolders will be created, called  build and  dist.
# The dist folder contains your .exe file, MSVCR71.dll and w9xpopen.exe (needed for os.popen() only)
# Your .exe file contains your byte code, all needed modules and the Python interpreter.
# The MSVCR71.dll can be distributed, but is often already in the system32 folder.
# The build folder can be deleted.
# tested with Python24 and Py2Exe v0.6.3 by   vegaseat   27may2006

from distutils.core import setup
import py2exe
import sys

# Handle missing args
if len(sys.argv) == 1:
    sys.argv.append("py2exe")

# creates a standalone .exe file, no zip files
setup( options = {"py2exe":
                  { #"compressed":   1,
                    #"optimize":     2,
                    #"ascii":        1,
                    "bundle_files": 1,
                    #"packages" : ["encodings"],
                    #"includes" : ["suds"],
                    },
                  },
       zipfile = None,
       # replace myFile.py with your own code filename here ...
       console = ['lte_plt_parser.py'] )
