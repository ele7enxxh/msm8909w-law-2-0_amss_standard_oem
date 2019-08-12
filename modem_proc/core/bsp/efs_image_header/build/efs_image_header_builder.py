#============================================================================
#
# EFS Image Header builder.
#
# GENERAL DESCRIPTION
#     Defines the builder function which calls the Image header generation
#     utility. Also installs the images created into the build/ms/bin/<id>
#     directory.
#
# Copyright (c) 2013 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#----------------------------------------------------------------------------
#
#  $Header: //components/rel/core.mpss/3.5.c12.3/bsp/efs_image_header/build/efs_image_header_builder.py#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $
#
#                      EDIT HISTORY FOR FILE
#
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#
# when        who  what, where, why
# ----------  ---  ----------------------------------------------------------
# 2013-03-05  dks  Removing env to builder until possible scons issue is fixed.
# 2013-02-01  nr   Rewrite MBN generation with EFS internal scripts.
# 2012-07-03  dks  Protect image header creation based on correct makeonly flag.
# 2012-07-02  dks  Create dummy headers and correct headers based on build flag.
# 2012-02-27  dks  Add source file dependency to the builder.
# 2012-02-17  dks  Create.
#
#============================================================================

import os
import efs_image_header_gen
import efs_image_dictionary

EFS_IMAGE_SECTOR_SIZE = 512

def generate(env):
    #--------------------------------------------------------------------------
    # Publish methods and define builder action
    #--------------------------------------------------------------------------
    efs_image_hdr_act = env.GetBuilderAction(fs_image_hdr_builder)
    efs_image_hdr_bld = env.Builder(action=efs_image_hdr_act)
    env.Append(BUILDERS={'EfsImgHdrBuilder' : efs_image_hdr_bld})

    efs_image_dictionary_act = env.GetBuilderAction(fs_image_hdr_dict_builder)
    efs_image_dictionary_bld = env.Builder(action=efs_image_dictionary_act)
    env.Append(BUILDERS={'EfsImgDictBuilder' : efs_image_dictionary_bld})

    env.AddMethod(fs_generate_efs_img_headers, "EfsGenerateImageHeaders")
    env.AddMethod(fs_image_header_generate_install, "EfsGenInstallImgHdr")
    #---------------------------------------------------------------------------
    # Define source and target paths
    #---------------------------------------------------------------------------
    SRCPATH = "${BUILD_ROOT}/core/bsp/efs_image_header/src"
    env.VariantDir('${BUILDPATH}', SRCPATH, duplicate=0)

def exists(env):
    return env.Detect('efs_image_header_builder')

#-----------------------------------------------------------------------------
# Method that is called from main scons to invoke creation of efs image
# headers. The IMAGE_IDs are as expected by Boot and KICKSTART.
# The IMAGE_TYPE if not supplied needs to be as defined in the image_id_tbl
# in mbn_tools.py or if no entry exists for it, then the image type needs to
# be supplied such that concatenation of IMAGE_KEY_<IMAGE_TYPE>_DEST_ADDRESS
# is the definition of RAM address in the .builds file. If Image type exists
# in the image_id_table the corresponding image ID is picked up from the
# table, else, will be as supplied here.
#-----------------------------------------------------------------------------
def fs_generate_efs_img_headers (env):
    efs_img_hdrs = []

    # Change defines through enviroment. Remove it once cust file is removed
    # and dependency through env is tested.
    env.Append(CPPDEFINES=["CHECK_EFS_IMG_HDR_ENV_CHANGE"])

    efs_img_hdr_pp = env.PreProcess("${BUILDPATH}/efs_image_header.pp",
                                    "${BUILDPATH}/efs_image_header.c")

    # We need to tell scons that pp file depends on CUST_H
    if 'CUST_H' in env:
        cust_h_file = env.FindFile(env['CUST_H'], env['CPPPATH'])
        if cust_h_file:
            env.Depends(efs_img_hdr_pp, cust_h_file)

    efs_dict_file = env.RealPath('${BUILDPATH}') + "/" + "efs_image_meta.bin"
    efs_dict_install_path = env.RealPath('${MBN_ROOT}') + "/" + "efs_image_meta.bin"

    env.EfsImgDictBuilder (efs_dict_file, efs_img_hdr_pp)
    efs_img_hdrs += env.InstallAs(env.RealPath(efs_dict_install_path),
                                    env.RealPath(efs_dict_file))

    efs_img_hdrs += env.EfsGenInstallImgHdr('efs1.bin', efs_dict_file,
                                            EFS_IMAGE_SECTOR_SIZE,
                                            "MODEM_RAM_ADDR",
                                            "DUMMY_MODEM_RAM_SIZE",
                                            "DUMMY-1", "FS1")

    efs_img_hdrs += env.EfsGenInstallImgHdr('efs2.bin', efs_dict_file,
                                            EFS_IMAGE_SECTOR_SIZE,
                                            "SHARED_RAM_ADDR",
                                            "DUMMY_SHARED_RAM_SIZE",
                                            "DUMMY-2", "FS2")

    efs_img_hdrs += env.EfsGenInstallImgHdr('efs3.bin', efs_dict_file,
                                            EFS_IMAGE_SECTOR_SIZE,
                                            "GOLDEN_RAM_ADDR",
                                            "DUMMY_GOLDEN_RAM_SIZE",
                                            "DUMMY-3", "FSG")
    env.AddArtifact(['EFS_IMG_HDR'], efs_img_hdrs)


    return efs_img_hdrs

def fs_image_hdr_dict_builder (target, source, env):

    target = str (target [0])
    pre_processor_file = env.RealPath (str (source[0]))

    efs_image_dic = efs_image_dictionary.Efs_Image_Dict ()
    efs_image_dic.parse_preprocessed (pre_processor_file)
    efs_image_dic.write_dictionary_to_file (target)


#-----------------------------------------------------------------------------
# This method invokes the image header builder to create the image headers
# and uses InstallAs method to copy the images into the build/ms/bin/<id> dir.
#-----------------------------------------------------------------------------
def fs_image_header_generate_install (env, target_bin, source, image_header_size,
                                      image_parti_ram_addr_key,
                                      image_parti_size_key,
                                      image_magic_string,
                                      image_partition_name):

    target_path = env.RealPath('${BUILDPATH}') + "/" + target_bin
    install_path = env.RealPath('${MBN_ROOT}') + "/" + target_bin

    env.EfsImgHdrBuilder(target_path, source,
                         IMAGE_PARTI_RAM_ADDRESS_KEY=image_parti_ram_addr_key,
                         IMAGE_PARTI_SIZE_KEY=image_parti_size_key,
                         IMAGE_HEADER_SIZE=image_header_size,
                         IMAGE_HEADER_MAGIC=image_magic_string,
                         IMAGE_PARTITION_NAME=image_partition_name)
    efs_install_img = env.InstallAs(env.RealPath(install_path), env.RealPath(target_path))

    return efs_install_img

#-----------------------------------------------------------------------------
# Builder function that calls ImageHeader utility to generate the image
# headers.
#
# Parameters:
# env        Environment. Passed automatically and has Image Type, Image Id and
#                Image size passed in it.
# target Output binary file path.
# source Preprocessed output referencing the defines needed from .builds file
#-----------------------------------------------------------------------------
def fs_image_hdr_builder(target, source, env):
    target = str (target [0])
    if env.has_key('IMAGE_PARTI_RAM_ADDRESS_KEY'):
        image_parti_ram_addr_key = env.get('IMAGE_PARTI_RAM_ADDRESS_KEY')
    else:
        raise RuntimeError, ("EFS Image Header : IMAGE_PARTI_RAM_ADDRESS_KEY" +
                            " is not defined")

    if env.has_key('IMAGE_PARTI_SIZE_KEY'):
        image_parti_size_key = env.get('IMAGE_PARTI_SIZE_KEY')
    else:
        raise RuntimeError, "EFS Image Header : IMAGE_SIZE_KEY is not defined"

    if env.has_key('IMAGE_HEADER_SIZE'):
        image_header_size = env.get('IMAGE_HEADER_SIZE')
    else:
        raise RuntimeError, "EFS Image Header : IMAGE_OFFSET is not defined"

    if env.has_key('IMAGE_HEADER_MAGIC'):
        image_magic_string = env.get('IMAGE_HEADER_MAGIC')
    else:
        raise RuntimeError, "EFS Image Header : IMAGE_HEADER_MAGIC is not defined"

    if env.has_key('IMAGE_PARTITION_NAME'):
        image_partition_name = env.get('IMAGE_PARTITION_NAME')
    else:
        raise RuntimeError, "EFS Image Header : IMAGE_PARTITION_NAME is not defined"

    efs_image_dic = efs_image_dictionary.Efs_Image_Dict ()
    efs_image_dic.parse_dict_file (str (source[0]))

    image_parti_ram_addr = efs_image_dic.lookup_key(image_parti_ram_addr_key)
    image_parti_size = efs_image_dic.lookup_key(image_parti_size_key)


    efs_image_header_gen.generate_elf_header_file(env.RealPath(target),
                                              image_parti_ram_addr, image_parti_size,
                                              image_header_size, image_magic_string,
                                              image_partition_name)
