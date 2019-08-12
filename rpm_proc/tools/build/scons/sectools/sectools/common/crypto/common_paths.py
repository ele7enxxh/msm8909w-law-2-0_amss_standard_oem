#===============================================================================
#
# Copyright (c) 2014 Qualcomm Technologies, Inc. All Rights Reserved.
# Qualcomm Technologies Proprietary and Confidential.
#
#===============================================================================

import os
import sys
from subprocess import Popen,PIPE
from __secfile__ import DIR_PATH

openssl_binary_path = os.path.join(DIR_PATH, '..', '..', '..', 'bin', 'WIN','openssl.exe')
if sys.platform=='linux2':
    openssl_binary_path = Popen(["which", "openssl"], stdout=PIPE).communicate()[0].strip()
