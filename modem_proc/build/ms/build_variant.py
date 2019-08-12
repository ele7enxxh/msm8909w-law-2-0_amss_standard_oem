#!/usr/bin/env python
'''
===============================================================================

  GENERAL DESCRIPTION:

     MPSS Top-Level Build Script for building a single variant.

  USAGE:

     python build_variant.py <variant_cluster> <options> <QC-SCons Options>

        <variant_cluster(s)>:

            Zero or more variant clusters may be specified to indicate what is
            to be built.  A variant cluster consists of a tri-mer of information
            as follows:

                  <chip_id>.<variant>.<purpose_name>

            The chip_id, variant and purpose name values will used to index into
            the build_cfg.xml file to find parameters for the build.

            The namespace for these values is mutually exclusive, so that you 
            can specify them in any order without confusion of meaning.  This 
            also means that you could leave any of them out, and the default 
            value will be used.

            The default value will be whatever is listed first in the 
            build_cfg.xml file.

        <options>:

            -h, --help:

                Print this message, and then call QC-SCons with this parameter
                so that it can print its help message.

            --strip

                Add USES_NO_STRIP_NO_ODM and USES_NO_DEBUG to the list of USES 
                flags.

            image=<image>

                Used to pass in images to QC-SCons.  Default images are read 
                from build_cfg.xml.  This parameter is only needed when 
                specifying images other than the default.

        <QC-SCons Options>:

            All other command line parameters are passed unchanged to QC-SCons

-------------------------------------------------------------------------------

  Copyright (c) 2009-2012 by QUALCOMM, Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary/GTDR

-------------------------------------------------------------------------------

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/build/ms/build_variant.py#1 $
  $DateTime: 2016/12/13 07:59:25 $
  $Change: 11985153 $

===============================================================================
'''

#================================
# Import Python Libraries
#================================
import sys
import os
import shutil
import time
from xml.etree import ElementTree as et
from glob import glob
from copy import copy

import build_cfg
from datetime import datetime

#================================
# Constants
#================================

def delta_time (start, end):
    '''Calculate the delta between two times'''
    delta = end - start
    minutes = delta / 60
    seconds = delta % 60
    delta_str = "%d minutes, %d seconds" % (minutes, seconds)
    return delta_str
# End of delta_time()

class InputError(Exception):
    def __init__(self, value):
        self.value = "Build Variant Input Error: " + value
    def __str__(self):
        return repr(self.value)
# End of class InputError

class InternalError(Exception):
    def __init__(self, value):
        self.value = "Build Variant Internal Error: " + value
    def __str__(self):
        return repr(self.value)
# End of class InputError

def get_cfg_data_item (pl_info, param, default):
    el = pl_info.find(param)
    if el is not None:
        val = el.text
        if val:
            val = val.strip()
            return val
    if type(default) == list:
        default = ','.join(default)
    elif type(default) == bool:
        if default:
            default = 'true'
        else:
            default = 'false'
    return default
# End of get_cfg_data_item()

class cfg_data:
    '''Object to store all of the config data for the PL'''

    def __init__ (self):
        '''Setup default values'''

        # Values from command line:
        self.opt_clean             = False
        self.opt_no_exec           = False
        self.opt_sconstruct        = None

        # Values from build_cfg.xml
        self.pl_name               = None
        self.chip_name             = None
        self.variant_name          = None
        self.purpose_name          = 'test'
        self.pl_subdir             = None
        self.image                 = ['mpss']
        self.build_id              = None
        self.chip_type             = None
        self.chip_id               = None
        self.chip_ver              = ''
        self.cflags                = ''
        self.lflags                = ''
        self.compiler_type         = 'Hexagon'
        self.hexagon_rtos_release  = '5.0.07'
        self.hexagon_q6version     = 'v4'
        self.hexagon_image_entry   = None
        self.uses_flags            = ''
        self.workflow              = False
        self.env_flags             = {}

    # End of cfg_data.__init__()

    def read_from_xml_element (self, el, ignore_image):

        self.pl_name              = build_cfg.get_pl_name(variant_info = el     , default = self.pl_name      )
        self.chip_name            = get_cfg_data_item(el, 'chip_name'           , self.chip_name              )
        self.variant_name         = get_cfg_data_item(el, 'variant_name'        , self.variant_name           )
        self.purpose_name         = get_cfg_data_item(el, 'purpose_name'        , self.purpose_name           )
        self.pl_subdir            = get_cfg_data_item(el, 'pl_subdir'           , self.pl_subdir              )
        if not ignore_image:
            self.image            = get_cfg_data_item(el, 'image'               , self.image                  ).split(',')
        self.build_id             = get_cfg_data_item(el, 'build_id'            , self.build_id               )
        self.chip_type            = get_cfg_data_item(el, 'chip_type'           , self.chip_type              )
        self.chip_id              = get_cfg_data_item(el, 'chip_id'             , self.chip_id                )
        self.chip_ver             = get_cfg_data_item(el, 'chip_ver'            , self.chip_ver               )
        self.cflags               = get_cfg_data_item(el, 'cflags'              , self.cflags                 )
        self.lflags               = get_cfg_data_item(el, 'lflags'              , self.lflags                 )
        self.compiler_type        = get_cfg_data_item(el, 'compiler_type'       , self.compiler_type          )
        self.hexagon_rtos_release = get_cfg_data_item(el, 'hexagon_rtos_release', self.hexagon_rtos_release   )
        self.hexagon_q6version    = get_cfg_data_item(el, 'hexagon_q6version'   , self.hexagon_q6version      )
        self.hexagon_image_entry  = get_cfg_data_item(el, 'hexagon_image_entry' , self.hexagon_image_entry    )
        temp                      = get_cfg_data_item(el, 'uses_flags'          , ''                          )
        if temp:
            if self.uses_flags:
                self.uses_flags   = ','.join([self.uses_flags, temp])
            else:
                self.uses_flags   = temp

        env_flags = el.find('env_flags')
        if env_flags != None:
            for env_flag in env_flags.findall('env_flag'):
                evar_name  = env_flag.find('name')
                evar_value = env_flag.find('value')
                if evar_name != None and evar_value != None:
                    self.env_flags[evar_name.text.strip()] = \
                    evar_value.text.strip()
                else:
                    raise InputError("build_cfg.xml has a <env_flag> \
                    element without a name or value.")

#        for uses_flag_el in el.findall('uses_flags'):
#            uses_flag_txt = uses_flag_el.text
#            if uses_flag_txt:
#                self.uses_flags.append(uses_flag_txt.strip())
#            else:
#                raise InputError("build_cfg.xml has empty uses_flags element.")

    # End of cfg_data.read_from_xml_element()

    def validate (self):
        if self.pl_name              is None: raise InputError("Missing value for pl_name/si_name")
        if self.chip_name            is None: raise InputError("Missing value for chip_name")
        if self.variant_name         is None: raise InputError("Missing value for variant_name")
        # pl_subdir is set later from the PL and Chip names, if not already set through build_cfg.xml
        #if self.pl_subdir           is None: raise InputError("Missing value for pl_subdir")
        if not self.image                   : raise InputError("Missing value for image")
        if self.build_id             is None: raise InputError("Missing value for build_id")
        if self.chip_type            is None: raise InputError("Missing value for chip_type")
        if self.chip_id              is None: raise InputError("Missing value for chip_id")
        if self.chip_ver             is None: raise InputError("Missing value for chip_ver")
        if self.cflags               is None: raise InputError("Missing value for cflags")
        if self.lflags               is None: raise InputError("Missing value for lflags")
        if self.compiler_type        is None: raise InputError("Missing value for compiler_type")
        if self.hexagon_rtos_release is None: raise InputError("Missing value for hexagon_rtos_release")
        if self.hexagon_q6version    is None: raise InputError("Missing value for hexagon_q6version")
        if self.hexagon_image_entry  is None: raise InputError("Missing value for hexagon_image_entry")
        #if self.purpose_name        is None: raise InputError("Missing value for purpose_name")
    # End of cfg_data.validate()

    def gen_image_cfg_str (self):

        image_cfg_template = '''
        <image_cfg>
          <build_flags>
             <!-- Build Flags -->
             <product_line>  {pl_name}             </product_line>
             <build_id>      {build_id}            </build_id>
             <build_asic>    {chip_id}             </build_asic>
             <msm_id>        {chip_id}             </msm_id>
             <hal_platform>  {chip_id}             </hal_platform>
             <target_family> {chip_id}             </target_family>
             <chipset>       {chip_type}{chip_id}  </chipset>
             <compiler_id>   RVCT221P593           </compiler_id>
             <t_cflags>      {cflags}              </t_cflags>
             <t_lflags>      {lflags}              </t_lflags>
          </build_flags>
          <env_vars> 
             <!-- Setup Compiler Environment Variables -->
             <env_var>
                <var_name>  COMPILER_TYPE </var_name>
                <var_value> {compiler_type}         </var_value>
             </env_var>
             <env_var>
                <var_name>  HEXAGON_RTOS_RELEASE </var_name>
                <var_value> {hexagon_rtos_release} </var_value>
             </env_var>
             <env_var>
                <var_name>  HEXAGON_Q6VERSION </var_name>
                <var_value> {hexagon_q6version} </var_value>
             </env_var>
             <env_var>
                <var_name>  HEXAGON_IMAGE_ENTRY </var_name>
                <var_value> {hexagon_image_entry} </var_value>
             </env_var>
             <env_var>
                <var_name>  VARIANT_NAME </var_name>
                <var_value> {variant_name} </var_value>
             </env_var>
          </env_vars>
        </image_cfg>
        '''

        return image_cfg_template.format(
            pl_name               = self.pl_name,
            build_id              = self.build_id,
            chip_type             = self.chip_type,
            chip_id               = self.chip_id,
            chip_ver              = self.chip_ver,
            cflags                = self.cflags,
            lflags                = self.lflags,
            compiler_type         = self.compiler_type,
            hexagon_rtos_release  = self.hexagon_rtos_release,
            hexagon_q6version     = self.hexagon_q6version,
            hexagon_image_entry   = self.hexagon_image_entry,
            # The meaning of the term 'variant' is overloaded here because it 
            # has changed over time.  It used to mean just the middle section of
            # the variant cluster, but it now refers to the entire variant 
            # cluster, and the middle part is referred to as the flavor.  In the
            # image_cfg.xml data, we use the term 'variant' to refer to the 
            # entire variant cluster.
            variant_name          = '.'.join([self.chip_name, self.variant_name,
                                               self.purpose_name])
            )

    # End of cfg_data.gen_image_cfg_str()

# End of class cfg_data

class build_cfg_xml:

    def __init__ (self,lg,targ_root):

        self.xml_data = et.ElementTree(file = build_cfg_filename).getroot()

        self.pl_info = None
        self.chip_info = None
        self.variant_info = None
        self.purpose_info = None
        self.lg=lg
        self.targ_root = targ_root

        # Build a dictionary mapping various name strings to what type they are,
        # e.g. mapping '8974' to 'chip_name'.  Available types are:
        #     'pl_name'
        #     'chip_name'
        #     'variant_name'
        self.name_mapping = {}

        pl_info= self.xml_data.find('variant_info')
        if pl_info is None:
            pl_info = self.xml_data.find('pl_info')
        if pl_info != None:
            cur_pl_name = build_cfg.get_pl_name(pl_info)
            if cur_pl_name:
                cur_pl_name = cur_pl_name.strip()
                if cur_pl_name not in self.name_mapping:
                    self.name_mapping[cur_pl_name] = 'pl_name'
                else:
                    raise InputError("pl_name multiply defined in " 
                                     + build_cfg_filename + ": <" 
                                     +cur_pl_name + ">")
            else:
                raise InputError(build_cfg_filename + 
                                 " has a <pl_info>/<variant_info> element with \
                                 no name.")

            for chip_info in pl_info.findall('chip_info'):
                cur_chip_name = chip_info.find('chip_name').text
                if cur_chip_name:
                    cur_chip_name = cur_chip_name.strip()
                    if cur_chip_name not in self.name_mapping:
                        self.name_mapping[cur_chip_name] = 'chip_name'
                    elif self.name_mapping[cur_chip_name] != 'chip_name':
                        raise InputError("<" + cur_pl_name 
                                         +"> is defined as both a 'chip_name' \
                                         and a '" 
                                         +self.name_mapping[cur_chip_name] 
                                         +"' in " + build_cfg_filename)
                else:
                    raise InputError(build_cfg_filename 
                                     + " has a <chip_info> element with no \
                                     name.")

                for var_info in chip_info.findall('build_variant'):
                    cur_var_name = var_info.find('variant_name').text
                    if cur_var_name:
                        cur_var_name = cur_var_name.strip()
                        if cur_var_name not in self.name_mapping:
                            self.name_mapping[cur_var_name] = 'variant_name'
                        elif self.name_mapping[cur_var_name] != 'variant_name':
                            raise InputError("<" + cur_var_name 
                                             +"> is defined as both a \
                                             'variant_name' and a '" 
                                             +self.name_mapping[cur_var_name] 
                                             +"' in " + build_cfg_filename)
                    else:
                        raise InputError(build_cfg_filename 
                                         + " has a <variant_info> element with \
                                         no name.")

                    for purpose_info in var_info.findall('purpose'):
                        cur_purpose_name = \
                        purpose_info.find('purpose_name').text
                        if cur_purpose_name:
                            cur_purpose_name = cur_purpose_name.strip()
                            if cur_purpose_name not in self.name_mapping:
                                self.name_mapping[cur_purpose_name] = \
                                'purpose_name'
                            elif self.name_mapping[cur_purpose_name] != \
                            'purpose_name':
                                raise InputError("<" + cur_purpose_name 
                                                 +"> is defined as both a \
                                                 'purpose_name' and a '" 
                                                 +self.name_mapping\
                                                 [cur_purpose_name] 
                                                 +"' in build_cfg.xml")
                        else:
                            raise InputError("build_cfg.xml has a \
                            <purpose_info> element with no name.")

    # End of build_cfg_xml.__init__()

    def pre_script_exec (self,pre_script,str_level):
        
        pre_script = os.path.join(self.targ_root, pre_script)
        pre_script = "python " + pre_script
        self.lg.log("pre-script started: " + pre_script + " level=" + str_level)
        #pre-script execution
        exec_return = self.lg.log_exec(pre_script, shell=True)
        self.lg.log("pre-script completed: " + pre_script + " level="
                     + str_level)
        if exec_return != 0:
            self.lg.log("pre-script failed in execution")
            sys.exit(exec_return)

    # End of build_cfg_data.pre_script_exec()

    def seek_to_pl (self,pl_name):

        pl_info = self.xml_data.find('variant_info')
        if pl_info is None:
            pl_info = self.xml_data.find('pl_info')
        if pl_info != None:
            cur_pl_name = build_cfg.get_pl_name(pl_info)
            if cur_pl_name:
                if pl_name == cur_pl_name.strip():
                    #execute pre-script at PL level
                    for pscripts in pl_info.findall('pre-script'):
                        pre_script = pscripts.find('script').text
                        if pre_script:
                            self.pre_script_exec(pre_script,"PL")
                    self.pl_info = pl_info
                    return self.pl_info
            else:
                raise InputError(build_cfg_filename 
                                 + " is missing <pl_name>/<si_name> in \
<pl_info>/<variant_info> element.")

        # It's an error if we can't find the named PL.
        raise InputError(build_cfg_filename + " is missing data for PL " 
                         + pl_name)

    # End of build_cfg_data.seek_to_pl()

    def seek_to_chip (self,chip_name):

        if self.pl_info != None:
            for chip_info in self.pl_info.findall('chip_info'):
                cur_chip_name = chip_info.find('chip_name').text
                if cur_chip_name:
                    if chip_name == cur_chip_name.strip():
                        #execute pre-script at Chip level
                        for pscripts in chip_info.findall('pre-script'):
                            pre_script = pscripts.find('script').text
                            if pre_script:
                                self.pre_script_exec(pre_script,"Chip")
                        self.chip_info = chip_info
                        return self.chip_info
                else:
                    raise InputError(build_cfg_filename 
                                     + " is missing <chip_name> in <chip_info> \
element.")

            # It's an error if we can't find the named PL.
            raise InputError(build_cfg_filename 
                             + " is missing data for chip " + chip_name)
        else:
            raise InternalError('Looking for chip info before determining the \
PL.')

    # End of build_cfg_data.seek_to_chip()

    def get_default_chip (self):

        if self.pl_info:
            self.chip_info = self.pl_info.find('chip_info')
            return self.chip_info
        else:
            raise InternalError('Looking for chip info before determining the\
PL.')

    # End of build-cfg_data.get_default_chip()

    def seek_to_variant (self, var_name):

        if self.chip_info != None:
            for var_info in self.chip_info.findall('build_variant'):
                cur_var_name = var_info.find('variant_name').text
                if cur_var_name:
                    if var_name == cur_var_name.strip():
                        #execute pre-script at Variant level
                        for pscripts in var_info.findall('pre-script'):
                            pre_script = pscripts.find('script').text
                            if pre_script:
                                self.pre_script_exec(pre_script,"Variant")
                        self.variant_info = var_info
                        return self.variant_info
                else:
                    raise InputError(build_cfg_filename 
                                     + " is missing <variant_name> in \
<build_variant> element.")

            # It's an error if we can't find the named PL.
            raise InputError(build_cfg_filename 
                             + " is missing data for variant " + var_name)
        else:
            raise InternalError('Looking for variant info before determining \
the chip.')


    # End of build_cfg_data.seek_to_variant()

    def get_default_variant (self):

        if self.chip_info:
            self.variant_info = self.chip_info.find('build_variant')
            return self.variant_info
        else:
            raise InternalError('Looking for variant info before determining \
the chip.')

    # End of build-cfg_data.get_default_chip()

#Obtain the purpose xml element with the purpose_name=input  
    def seek_to_purpose (self, purpose_name):
        if self.variant_info != None:
            for purpose_info in self.variant_info.findall('purpose'):
                cur_purpose_name = purpose_info.find('purpose_name').text
                if cur_purpose_name:
                    if purpose_name == cur_purpose_name.strip():
                        #execute pre-script at Variant level
                        for pscripts in purpose_info.findall('pre-script'):
                            pre_script = pscripts.find('script').text
                            if pre_script:
                                self.pre_script_exec(pre_script,"Purpose")
                        self.purpose_info = purpose_info
                        return self.purpose_info
                else:
                    raise InputError("build_cfg.xml is missing <purpose_name> \
in <purpose> element.")

            # It's an error if we can't find the named PL.
            raise InputError("build_cfg.xml is missing data for purpose " \
+ purpose_name)
        else:
            raise InternalError('Looking for purpose info before determining \
the variant.')

#If no purpose specified, get the default purpose
    def get_default_purpose (self):
        if self.variant_info:
            self.purpose_info = self.variant_info.find('purpose')
            return self.purpose_info
        else:
            raise InternalError('Looking for purpose info before determining \
the variant.')

    def parse_var_cluster (self, var_cluster_name):

        var_cluster_names = var_cluster_name.split('.')

        chip_name = None
        variant_name = None
        purpose_name = None

        for name in var_cluster_names:
            if name in self.name_mapping:
                if self.name_mapping[name] == 'variant_name':
                    variant_name = name
                elif self.name_mapping[name] == 'chip_name':
                    chip_name = name
                elif self.name_mapping[name] == 'purpose_name':
                    purpose_name = name
            else:
                self.printBuildCommands()
                errmsg = ('Build command line error: "' + name 
                          + '" unrecognized.\n')
                errmsg += 'Choose a supported variant or alias from the '
                errmsg += 'list above.'
                raise InputError(errmsg)
        if chip_name:
            chip_info = self.seek_to_chip(chip_name)
        else:
            chip_info = self.get_default_chip()

        if variant_name:
            variant_info = self.seek_to_variant(variant_name)
        else:
            variant_info = self.get_default_variant()

        if purpose_name:
            purpose_info = self.seek_to_purpose(purpose_name)
        else:
            purpose_info = self.get_default_purpose()

        return (chip_info, variant_info, purpose_info)
    # End of parse_var_cluster()


    #Parse the internal_build_cfg.xml, and return dictionary that
    #contains available build commands by product line
    def processBuildCommands(self):
        buildcmds= dict()

        pl_info = self.xml_data.find('variant_info')
        if pl_info is None:
            pl_info = self.xml_data.find('pl_info')
        if pl_info != None:
            pl_variant_cluster = []
            pl_alias = []
            pl_name = build_cfg.get_pl_name(pl_info)

            for chip_info in pl_info.findall('chip_info'):
                chip_name = chip_info.find('chip_name').text.strip()
                for var_info in chip_info.findall('build_variant'):
                    variant_name = var_info.find('variant_name').text.strip()
                    purpose_name = 'unavailable'
                    for purpose_info in var_info.findall('purpose'):
                        purpose_name = purpose_info.find('purpose_name').text
                        pl_variant_cluster.append(chip_name.strip()+'.'
                                                  +variant_name.strip() + '.' 
                                                  + purpose_name.strip())
                    if purpose_name == 'unavailable':
                        pl_variant_cluster.append(chip_name.strip()
                                                  +'.'+variant_name.strip())
                  
            for alias in pl_info.findall('alias') :
                alias_name = alias.find('alias_name').text
                alias_variants = alias.find('alias_variants').text
                pl_alias.append((alias_name, alias_variants ))

            buildcmds[pl_name] = {'Alias':pl_alias, 'Variant Cluster'
                                  :pl_variant_cluster}

        return buildcmds


    #print available build commands by product line to command prompt
    def printBuildCommands (self):
        Buildcmds = self.processBuildCommands()
        self.lg.log('### Valid build command listing ###')
        for pl_name in Buildcmds :
            self.lg.log('\n\n---------Product Line:' + pl_name + '---------')
            pl_names = Buildcmds[pl_name]

            self.lg.log('\n\n\t\tAliases:')
            for alias in pl_names['Alias']:
                if ',' in alias[1]:
                    self.lg.log('\t\t\tbuild.cmd ' + alias[0] 
                                + '------> build.cmd '+ alias[1] )

            self.lg.log('\n\n\t\tVariant Clusters:')
            for variant_cluster in pl_names['Variant Cluster'] :
                self.lg.log('\t\t\tbuild.cmd ' + variant_cluster )

# End of class build_cfg_xml

class build_stage_config:
    def __init__ (self,lg,config_dir,features,chip_id,build_id):
        self.lg = lg
        self.cfg_subdir = config_dir
        self.full_feature_string = features
        self.chip_id = chip_id
        self.build_id = build_id
#        self.bsp_files = []
        self.bsp_files = ['rcinit_task_prio.csv', 
                          'rcinit_task_govern.csv',
                          'rcinit_legacy_info.csv',
                          'modem_task_stksz.csv',
                          'modem_proc.scons']
        self.cwd_files = ['target.builds']

        if os.path.dirname(__file__):
            self.build_ms_dir = os.path.dirname(__file__)
        else:
            self.build_ms_dir = '.'
        
        self.build_dir = os.path.join(self.build_ms_dir, '..') 

    # End of __init__()

    def copy_files_to_cwd (self, path):

        for file in glob(os.path.join(path, '*')):
            if not os.path.isdir(file):
                dest_file = None
                if os.path.split(file)[1] in self.bsp_files:
                    dest_file = os.path.normpath(os.path.join
                                                 (self.build_ms_dir, 
                                                  '../bsp/modem_proc_img/build',
                                                  os.path.split(file)[1]))
                elif os.path.split(file)[1] in self.cwd_files:
                    dest_file = os.path.join(self.build_ms_dir, 
                                             os.path.split(file)[1])
                else:
                    dest_file = os.path.join(self.build_ms_dir, 
                                             os.path.split(file)[1])
                self.lg.log("dest_file: " + dest_file)
                if os.path.isfile(dest_file):
                    os.chmod(dest_file, 0777)
                self.lg.log("Copying file " + file + " to " + self.build_ms_dir)
                if os.path.split(file)[1] in self.bsp_files:
                    shutil.copy(file, os.path.normpath(os.path.join
                                                       (self.build_ms_dir, 
                                                        '../bsp/modem_proc_img/build')))
                else:
                    shutil.copy(file, self.build_ms_dir)
                os.chmod(dest_file, 0777)


    def copy_tree_to_build(self, src_path, dst_path):

        if not os.path.exists(src_path) or os.path.isfile(src_path):
            return
        
        list_dir = os.listdir(src_path)

        for name in list_dir:
            src_path2 = os.path.join(src_path, name)
            dst_path2 = os.path.join(dst_path, name)

            if os.path.isdir(src_path2):
                self.copy_tree_to_build(src_path2, dst_path2)
            else:
                if os.path.isfile(dst_path2):
                    os.chmod(dst_path2, 0777)
                elif (os.path.dirname(dst_path2) and 
                      not os.path.exists(os.path.dirname(dst_path2))):
                    os.makedirs(os.path.dirname(dst_path2))

                self.lg.log("dest_file: " + dst_path2)
                self.lg.log("Copying file " + src_path2 + " to " + dst_path2)
                shutil.copy(src_path2, dst_path2 )
                os.chmod(dst_path2, 0777)

    def copy_scoping_config_files (self, new_config, features):
        if features.split('.')[0] != features:
            self.copy_scoping_config_files(new_config, features.rsplit('.',1)[0])
            if new_config:
                self.copy_tree_to_build(os.path.join(self.cfg_subdir, 
                                                     '.'.join(features.rsplit\
                                                              ('.',1))), 
                                        self.build_dir )
            else:
                self.copy_files_to_cwd(os.path.join(self.cfg_subdir, 
                                                    '.'.join(features.rsplit\
                                                             ('.',1))))

    def copy_config_files (self):

        new_config = os.path.isdir(os.path.join(self.cfg_subdir, 'default'))
        if new_config:
            self.copy_tree_to_build(os.path.join(self.cfg_subdir, 
                                                 'default'), self.build_dir )
        else:
            # copy the root files first.
            self.copy_files_to_cwd(self.cfg_subdir)
        # copy the first level files next.
        for atom in self.full_feature_string.split('.'):
            # if the directory exists...
            if os.path.isdir(os.path.join(self.cfg_subdir, atom)):
                if new_config:
                    self.copy_tree_to_build(os.path.join(self.cfg_subdir, 
                                                         atom), self.build_dir )
                else:
                    # copy the files to the current (build/ms) directory.
                    self.copy_files_to_cwd(os.path.join(self.cfg_subdir, atom))
        # copy higher scoping levels last.
        self.copy_scoping_config_files(new_config, self.full_feature_string)

    def remove_config_files (self, dir):
        for file in glob(os.path.join(dir, '*')):
            if file.split('/')[-1].split('\\')[-1] in self.bsp_files:
                # Remove the corresponding files from the bsp directory
                self.lg.log("Removing file " 
                            + file.split('/')[-1].split('\\')[-1] +".")
                os.remove(os.path.join(os.path.normpath
                                       ('../bsp/modem_proc_img/build'), 
                                       file.split('/')[-1].split('\\')[-1]))
            elif os.path.isfile(file.split('/')[-1].split('\\')[-1]):
                # Remove the corresponding files from the current directory
                self.lg.log("Removing file " 
                            + file.split('/')[-1].split('\\')[-1] +".")
                os.remove(file.split('/')[-1].split('\\')[-1])
            elif (os.path.isdir(file) and 
                  self.full_feature_string.count(file.split('/')[-1].\
                                                 split('\\')[-1])):
                self.remove_config_files(file)

# End of build_stage_config

def call_scons (lg, scons, cfg, cleaned_args, cfg_subdir):

    #================================
    # Make sure that everything had a value
    #================================

    cfg.validate()

    #================================
    # Build image config data
    #================================

    image_cfg_str = cfg.gen_image_cfg_str()

    stage = None
    #================================
    # Copy PL Specific files into build\ms directory
    #================================
    if not cfg.pl_subdir:
        cfg.pl_subdir = '-'.join([cfg.pl_name, cfg.chip_name])
    build_ms_dir = os.path.dirname(__file__)
    if build_ms_dir:
        build_ms_dir = os.path.relpath(build_ms_dir)
    else:
        build_ms_dir = '.'
    pl_dir = os.path.join(build_ms_dir, cfg.pl_subdir)

    #================================
    # Call QC-SCons Build System
    #================================
    if '--no-exec' in cleaned_args and '--clean' in cleaned_args:
        cmd_args = cleaned_args + ['/'] + cfg.image
    else:
        cmd_args = cleaned_args + cfg.image
    lg.log('Passing image_cfg info to QC-SCons Build System:')
    lg.log(image_cfg_str)
    if cfg.uses_flags:
        cmd_args.append('USES_FLAGS=' + cfg.uses_flags)
    cfg_target_builds = build_cfg.get_target_builds()
    if (cfg_target_builds is not None and
         os.path.exists(cfg_target_builds)):
        # Point scons to the target.builds file in the config directory
        cmd_args.append('--tcfgf=' + cfg_target_builds)
    lg.log("Calling QC-SCons with the following additional arguments:")
    for arg in cmd_args:
        lg.log("    " + arg)

    if '--cleanpack' in cmd_args:
        #------------------------------------------------------
        # Save .dblite files for restoration later
        #
        # We're running the build with slightly different params, and this throws
        # SCons off, so rename the old .dblite file before running our build,
        # then we'll restore it after the build completes.
        #------------------------------------------------------
        dblite_file = ".sconsign_{build_id}.dblite".\
        format(build_id=cfg.build_id)
        saved_dblite_file = dblite_file + '.save_cleanpack'
        if os.path.exists(saved_dblite_file):
            os.remove(saved_dblite_file)   # Remove old saved file, just in case
        if os.path.exists(dblite_file):
            os.rename(dblite_file, saved_dblite_file)

    ret = scons.build(lg = lg, image_cfg_str = image_cfg_str, args = cmd_args)
    
    if '--cleanpack' in cmd_args:  
        #------------------------------------------------------
        # Restore the .dblite files
        #------------------------------------------------------
        if os.path.exists(dblite_file):
            os.remove(dblite_file)    # Remove old file, just in case
        if os.path.exists(saved_dblite_file):
            os.rename(saved_dblite_file, dblite_file)

    if not stage:
        #================================
        # Cleanup PL subdir files
        #================================
        if (ret == 0 and
             os.path.isdir(cfg.pl_subdir) and
             cfg.opt_clean and not cfg.opt_no_exec):
                lg.log("Cleaning PL-specific files")
                for file in glob(os.path.join(cfg.pl_subdir, '*')):
                    # Remove the corresponding files from the current directory
                    os.remove(file.split('/')[-1].split('\\')[-1])

    return ret

# End of call_scons()

def alias (alias_name, pl_info):
    alias_variants = []
    for alias in pl_info.findall("alias"):
        cur_alias_name = alias.find('alias_name').text
        if cur_alias_name:
            if alias_name == cur_alias_name.strip():
                cur_alias_variants = alias.find('alias_variants').text
                if cur_alias_variants:
                    alias_variants= cur_alias_variants.split(',')
                else:
                    raise InputError("build_cfg.xml is missing <alias_variants>\
in <alias> element.")
                return alias_variants
        else:
            raise InputError("build_cfg.xml is missing <alias_name> in <alias>\
element.")
    return alias_variants

def build (args):
    '''
    MPSS-specific script for invoking QC-SCons with the correct parameters
    for the current PL.
    '''

    #================================
    # Get our bearings
    #================================

    on_linux           = sys.platform != 'win32'
    qcscons_cmd        = os.path.realpath(os.path.join(qcscons_root, 
                                                        "SCons/scons"))
    if not on_linux:
        qcscons_cmd   += '.bat'
    qcscons_script     = os.path.realpath(os.path.join(qcscons_root, 
                                                       "build/start.scons"))

    #================================
    # Initialize Config Data
    #================================

    base_cfg = cfg_data()

    #================================
    # Parse command line parameters
    # Multiple Build IDs can be specified with the syntax:
    #        BUILD_ID=bid1,bid2,...
    # Command line parameters override build_cfg.xml parameters,
    # and build_cfg.xml parameters override default values.
    #================================
    var_clusters = []
    cmd_line_image_found = False
    cleaned_args  = []
    metric_type = 'compile'
    path_count = '3'
    format = None
    # Different behavior for workflow builds. Default is False (legacy behavior)

    #if command line is empty, use the arguments present in latest_command.txt
    if not args and os.path.isfile('latest_command.txt'):
        latest_cmd_file = open('latest_command.txt', 'r')
        args = latest_cmd_file.read().split()
        latest_cmd_file.close()

    for arg in args:
        if arg.count('\x96'):
            raise InputError('''Invalid Character: (0x96) seen.  
            This character looks like a dash, but is not.This usually happens 
            when the build command line is copy/pasted from an e-mail.Outlook 
            will convert both dashes and double-dashes to this character.  If 
            thebuild command has been copy/pasted, please try typing the build 
            command in manually.''')
        if arg.lower().startswith('image='):
            base_cfg.image = arg.split('=')[1].split(',')
            cmd_line_image_found = True
        elif arg.lower().startswith('uses_flags='):
            if base_cfg.uses_flags:
                base_cfg.uses_flags += ','
            base_cfg.uses_flags += arg.split('=')[1]
        elif arg.lower() == '--strip':
            if base_cfg.uses_flags:
                base_cfg.uses_flags += ','
            base_cfg.uses_flags += 'USES_NO_STRIP_NO_ODM,USES_NO_DEBUG'
        elif arg.lower() == '--no_stdout':
            log_to_stdout = False
        elif arg.lower().startswith('--log_name'):
            log_file_name = arg.rsplit('=', 1)[1]
        elif arg.lower().startswith('--metric_type='):
            metric_type=arg.split('=')[1]
        elif arg.lower().startswith('--format='):
            format=arg.split('=')[1]
        elif arg.lower().startswith('--path_count='):
            path_count=arg.split('=')[1]
        elif arg.lower() == '--workflow':
            base_cfg.workflow = True
        elif arg.count('=') or arg.startswith('-'):
            # These get passed to QC-SCons unchanged
            if arg.lower() in ['-h', '--help']:
                print __doc__
                raw_input("Press <Enter> to run QC-SCons for it's help message,\
                 or <Ctrl>-C to exit.")
                print "Running QC-SCons:"
            elif arg.lower() in ['-c', '--clean']:
                base_cfg.opt_clean = True
            elif arg.lower() in ['-n', '--no-exec']:
                base_cfg.opt_no_exec = True
            elif arg.lower() in ['-f', '--file', '--makefile', '--sconstruct']:
                base_cfg.opt_sconstruct += arg.split('=')[1]
            elif arg.startswith('BUILD_ID'):
                build_ids = arg.split('=')[1].split(',')
            cleaned_args.append(arg)
        else:
            var_clusters.append(arg)


    # Create latest_command.txt to contain previous valid build command in 
    # build/ms folder
    if base_cfg.opt_no_exec == False and args:
        latest_cmd_file = open('latest_command.txt', 'w')
        latest_cmd_file.write(' '.join(args))
        latest_cmd_file.close()


    lg.log("#---------------------------------------------------------------\
----------------")
    lg.log("# MPSS BUILD SCRIPT START")
    lg.log("#---------------------------------------------------------------\
----------------")
    start_time = time.time()
    lg.log("Start Time: " + time.ctime(start_time))

    lg.log("MPSS Build Command Line Parameters:")
    for arg in args:
        lg.log("    " + arg)

    #================================
    # Read PL Configuration
    #================================
    XmlPlCfg = et.ElementTree(file = build_cfg_filename).getroot()
    tmp_pl_info = XmlPlCfg.find('variant_info')
    if tmp_pl_info is None:
        tmp_pl_info = XmlPlCfg.find('pl_info')
    pl_name = build_cfg.get_pl_name(tmp_pl_info)
    if not pl_name:
        raise InputError("PL Name not found.")
    base_cfg.pl_name = pl_name.strip()


    #================================
    # Load QC-SCons starting script
    #================================

    import build as scons


    #================================
    # Read Build Configuration
    #================================

    cfg_xml = build_cfg_xml(lg,targ_root)
    
    pl_info = cfg_xml.seek_to_pl(base_cfg.pl_name)
    base_cfg.read_from_xml_element(pl_info, cmd_line_image_found)

    for env_flag in base_cfg.env_flags:
        os.putenv(env_flag, base_cfg.env_flags[env_flag])

    return_values = dict()

    if not var_clusters:

        # Initialize with defaults
        chip_info = cfg_xml.get_default_chip()
        base_cfg.read_from_xml_element(chip_info, cmd_line_image_found)
        variant_info = cfg_xml.get_default_variant()
        base_cfg.read_from_xml_element(variant_info, cmd_line_image_found)
        purpose_info = cfg_xml.get_default_purpose()
        base_cfg.read_from_xml_element(purpose_info, cmd_line_image_found)
        variant = '.'.join([base_cfg.chip_name, 
                            base_cfg.variant_name, 
                            base_cfg.purpose_name])
        #================================
        # Build this var cluster.
        #================================
        lg.log("==> Single call to scons")
        return_values[variant] = call_scons (lg, scons, base_cfg, cleaned_args,
                                              config_root)

    else:

        for var_cluster in var_clusters:

            var_cfg = copy(base_cfg)

            alias_variants = alias(var_cluster, pl_info)
            if not alias_variants:
                alias_variants.append(var_cluster)

            for variant in alias_variants :
                build_start_datetime = datetime.now()
                variant = variant.strip()
                #================================
                # Read our pl/chip/variant configuration info
                #================================
                chip_info, variant_info, purpose_info = \
                cfg_xml.parse_var_cluster(variant)
                var_cfg.read_from_xml_element(chip_info, cmd_line_image_found)
                var_cfg.read_from_xml_element(variant_info, 
                                              cmd_line_image_found)
                if purpose_info != None:
                    var_cfg.read_from_xml_element(purpose_info, 
                                                  cmd_line_image_found)

                #================================
                # Build this var cluster.
                #================================
                if not base_cfg.workflow:
                    try:
                        from rec_timing_info import rec_timing_info
                        rec_timing_info_flag = True
                    except:
                        rec_timing_info_flag = False
                else:
                    rec_timing_info_flag = False
    
                lg.log("==> Calling scons: " + str(var_cluster) + ", " 
                       + str(variant))
                return_values[variant] = call_scons(lg, scons, 
                                                    var_cfg, 
                                                    cleaned_args, 
                                                    config_root)
                if rec_timing_info_flag:
                    if metric_type!='pack':
                        rec_timing_info(metric_type,
                                        build_start_datetime,
                                        datetime.now(),
                                        variant,
                                        format,
                                        path_count)
                # We no longer wish to break if there is an error. All builds will be att
                if (return_values[variant] != 0 and 
                    '-k' not in cleaned_args and 
                    '--keepalive' not in cleaned_args):
                    break

            if (return_values[variant] != 0 and 
                '-k' not in cleaned_args and 
                '--keepalive' not in cleaned_args):
                 break

    # Legacy
    # This functionality should be moved to build_client.py, but is left here
    # to support legacy products.
    if not base_cfg.workflow:
        build_cfg.clean_build_config(pl_name = base_cfg.pl_name)
    # End of Legacy code

    ret = 0
    lg.log("#-----------------------------------------------------------------\
--------------")
    for variant in return_values:
        lg.log("Build " + variant + " returned code " 
               + str(return_values[variant]) + ".")
        if return_values[variant] != 0:
            ret = return_values[variant]
    lg.log("#----------------------------------------------------------------\
---------------")

    #================================
    # Record End Time
    #================================

    end_time = time.time()
    lg.log("Overall Start Time: " + time.ctime(start_time) + 
           ",  Overall End Time: " + time.ctime(end_time))
    lg.log("Overall Delta Time: " + delta_time(start_time, end_time))
    lg.log("#-----------------------------------------------------------------\
--------------")

    return ret

# End of build()


#================================
# Initialize directory paths
#================================

build_root         = os.path.realpath(os.path.join(os.path.dirname(__file__), "../.."))
targ_root          = os.path.realpath(os.path.join(os.path.dirname(__file__), "../../.."))
tools_root         = os.path.join(build_root, "tools")
config_root        = os.path.join(build_root, "config")
qcscons_root       = os.path.join(tools_root, "build/scons")
qcscons_build_dir  = os.path.join(qcscons_root, "build")

# Find the build config file:
search_paths = [os.path.join(config_root, 'default', 'internal_build_cfg.xml'),
                os.path.join(config_root, 'default/ms', 'internal_build_cfg.xml'),
                os.path.join(config_root, 'internal_build_cfg.xml'),
                'build_cfg.xml']
build_cfg_filename = None
for spath in search_paths:
    if os.path.exists(spath):
        build_cfg_filename = spath
        break

if build_cfg_filename == None:
    raise InputError("Can't find build_cfg.xml")

# Allow this script to be called from the OS command shell,
# or from another Python script.
if __name__ == "__main__":

    lg = None
    try:

        #================================
        # Initialize logging
        #================================

        log_file_name = 'build'
        log_to_stdout = True
        log_resume = False
        params = sys.argv[1:]
        for arg in sys.argv[1:]:
            if arg.lower().startswith('--log_name'):
                log_file_name = arg.rsplit('=', 1)[1]
                params.remove(arg)
            elif arg.lower() == '--no_stdout':
                log_to_stdout = False
                params.remove(arg)
            elif arg.lower() == '--restarted':
                log_resume = True
                params.remove(arg)

        sys.path.append(qcscons_build_dir)
        import logger
        lg = logger.Logger(log_file_name, 
                           log_to_stdout = log_to_stdout, 
                           resume=log_resume)

        # Log helpful debug info
        lg.log("Running $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/build/ms/build_variant.py#1 $")
        lg.log("Start directory is: " + os.getcwd())
        lg.log("Python Version is: " + sys.version)
        lg.log("Platform is: " + sys.platform)

        # Validate the environment against our build_cfg.
        # Restart after setting the PATH to the correct version of Python, 
        # if needed.
        build_cfg.setenv_and_restart_if_needed (sys.argv, lg)

        ret = build(params)
        if ret != 0:
            sys.exit(ret)

    except:
        # Log any crashes to the log file
        if lg:
            lg.log_crash()
        raise
    finally:
        if lg:
            lg.close()

