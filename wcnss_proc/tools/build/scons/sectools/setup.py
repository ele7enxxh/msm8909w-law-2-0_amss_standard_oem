#===============================================================================
#
# Copyright (c) 2014 Qualcomm Technologies, Inc. All Rights Reserved.
# Qualcomm Technologies Proprietary and Confidential.
#
#===============================================================================

"""Generates the code for the xml parsers and the data model obj representation
using generateDs and the datautils module.

.. data:: SETUP_LIST

    List of classes to run setup on
"""


import traceback

from sectools.scripts.setup.fuseblower_default import DistSetupFuseBlower
from sectools.scripts.setup.fuseblower_build import DistSetupFuseBlowerBuild
from sectools.scripts.setup.secimage_default import DistSetupSecImage
from sectools.scripts.setup.secimage_build import DistSetupSecImageBuild
from sectools.scripts.setup.sectools_default import DistSetupSectools
from sectools.scripts.setup.sectools_build import DistSetupSectoolsBuild
from sectools.scripts.setup.secimage_integration import DistSetupSecImageIntegration


SETUP_LIST = [
              DistSetupSectools,
              DistSetupSectoolsBuild,
              DistSetupSecImage,
              DistSetupSecImageBuild,
              DistSetupSecImageIntegration,
              DistSetupFuseBlower,
              DistSetupFuseBlowerBuild,
             ]

if __name__ == '__main__':
    # Setup all packages
    for setup in SETUP_LIST:
        try:
            obj = setup()
        except Exception as e:
            print e
            print traceback.format_exc()
