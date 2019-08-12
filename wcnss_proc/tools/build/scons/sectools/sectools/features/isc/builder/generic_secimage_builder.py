#===============================================================================
#
# Copyright (c) 2014 Qualcomm Technologies, Inc. All Rights Reserved.
# Qualcomm Technologies Proprietary and Confidential.
#
#===============================================================================

from sectools.common.builder.builderutils import BuilderUtil
import os

BUILDER_DIR = os.path.dirname(os.path.realpath(__file__))
SECTOOLS_DIR = os.path.join(BUILDER_DIR, "..", "..", "..", "..")

class GenericSecImageBuilder(object):

    def __init__(self, env):
        self.env = env
        self.builderutil = BuilderUtil(env)

    def build(self,
                target,
                source,
                sign_id,
                config,
                target_base_dir,
                chipset,
                sec_image_policy):

        #import pdb; pdb.set_trace()
        self.builderutil.printinfo("sectools_signer_builder: SECTOOLS_DIR = %s" % SECTOOLS_DIR)

        # Issuing sectools command
        cmds = ["python", os.path.join(SECTOOLS_DIR, "sectools.py"), "secimage", "-i", source,
                "-o", target_base_dir, "-g", sign_id, "-c", config]

        cmds.append(sec_image_policy.cmd_options)

        self.builderutil.printinfo("sectools_signer_builder: %s" % " ".join(cmds))
        self.builderutil.execcmds(cmds, target=target)

        return self.builderutil.getReturnValueInBuilder(target)

