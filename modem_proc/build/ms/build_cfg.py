#!/usr/bin/env python
#===============================================================================
#
# build_cfg.py
#
# GENERAL DESCRIPTION
#     Interface to build_cfg.xml
#
# Copyright (c) 2011-2013 by QUALCOMM Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#-------------------------------------------------------------------------------
#
#  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/build/ms/build_cfg.py#1 $
#  $DateTime: 2016/12/13 07:59:25 $
#  $Change: 11985153 $
#
#===============================================================================
'''%prog [options]

    This script provides an interface to build_cfg.py.'''


import sys
import os
import subprocess
import platform
import datetime
from xml.etree import ElementTree as et

try:
    # setenv defines specified company-specific path information.
    # since this information varies from company to company, it
    # isn't shipped with the build.  OEMs may either specify their
    # own version of this script, or specify their paths before
    # calling the build scripts.  Therefore, we need to be able to
    # run either with or without the setenv script.

    from setenv import setenv

    # If we don't get an exception, that means setenv is present.
    use_setenv = True

except:

    # an exception means that setenv isn't present.
    use_setenv = False


#######################################################################
# Exceptions
#######################################################################

#======================================================================
# VersionError
#======================================================================

class VersionError(Exception):
    def __init__(self, value):
        self.value = "Version Error: " + value
    def __str__(self):
        return repr(self.value)
# End of class VersionError


#======================================================================
# ConfigFormatError
#======================================================================

class ConfigFormatError(Exception):
    def __init__(self, value):
        self.value = "Config Format Error: " + value
    def __str__(self):
        return repr(self.value)
# End of class ConfigFormatError


#======================================================================
# ConfigError
#======================================================================

class ConfigError(Exception):
    def __init__(self, value):
        self.value = "Config Error: " + value
    def __str__(self):
        return repr(self.value)
# End of class ConfigError


#======================================================================
# ConfigError
#======================================================================

class InputError(Exception):
    def __init__(self, value):
        self.value = "Input Error: " + value
    def __str__(self):
        return repr(self.value)
# End of class InputError


#######################################################################
# Internal Functions
#######################################################################

#======================================================================
# get_boolean_attr
#======================================================================

def get_boolean_attr (element, attr_name, default = False):

    ret_val = default

    if element.attrib:
        if attr_name in element.attrib:

            attr_val = element.attrib[attr_name]
            # Convert do_compile_test from whatever type it is to a Boolean
            if type(attr_val) == str:
                ret_val = attr_val.upper() in ['TRUE', '1']
            elif type(attr_val) == int:
                ret_val = attr_val != 0

    return ret_val

# End of get_boolean_attr()

#======================================================================
# get_text_attr
#======================================================================

def get_text_attr (element, attr_name, default = ''):

    attr_val = default

    if element.attrib:
        if attr_name in element.attrib:

            attr_val = str(element.attrib[attr_name])

    return attr_val

# End of get_text_attr()

#======================================================================
# get_element_text
#======================================================================

def get_element_text (element, default = None):

    if element != None:
        ret_val = element.text
        if ret_val != None:
            ret_val = ret_val.strip()
        else:
            ret_val = default
    else:
        ret_val = default

    return ret_val

# End of get_element_text()

#======================================================================
# get_element_list
#======================================================================

def get_element_list (element, default = None):
    if element != None:
        if element.text != None:
            ret_vals = element.text.split(',')
        else:
            ret_vals = default
        if ret_vals != None:
            ret_vals = [ret_val.strip() for ret_val in ret_vals]
        else:
            ret_vals = default
    else:
        ret_vals = default

    return ret_vals

# End of get_element_text()

#======================================================================
# obj_to_str
#======================================================================

def obj_to_str (obj, pad=0):
    result = ''
    indent = "    "
    small_types = [int, str, bool, datetime.datetime]
    obj_typ = type(obj)
    if obj_typ == str:
        result += pad*indent + "'" + obj + "'"
    elif obj_typ in small_types:
        result += pad*indent + str(obj)
    elif obj_typ == dict:
        result += pad*indent + "{\n"
        need_a_comma = False
        for key in obj.keys():
            if need_a_comma:
                result += ',\n'
            need_a_comma = True
            result += obj_to_str (key, pad+1)
            ty2 = type(obj[key])
            if ty2 in small_types:
                result += ' : ' + obj_to_str (obj[key])
            else:
                result += ':\n' + obj_to_str (obj[key], pad+1)
        result += '\n' + pad*indent + "}"
    elif obj_typ in [list, set]:
        result += pad*indent + "[\n"
        need_a_comma = False
        for elem in obj:
            if need_a_comma:
                result += ',\n'
            need_a_comma = True
            ty2 = type(elem)
            result += obj_to_str (elem, pad+1)
        result += '\n' + pad*indent + "]"
    else:
        # For now, assume this is a class object
        result += pad*indent + "{\n"
        need_a_comma = False
        # The vars() function returns a list of object members
        try:
            fields = vars(obj)
            for field in fields:
                if need_a_comma:
                    result += ',\n'
                need_a_comma = True
                value = fields[field]
                result += (pad+1)*indent + field
                ty2 = type(value)
                if ty2 in small_types:
                    result += ' : ' + obj_to_str (value)
                else:
                    result += ':\n' + obj_to_str (value, pad+1)
        except TypeError:
            # It's not a class object, so treat as a small type
            result += pad*indent + str(obj)

        result += '\n' + pad*indent + "}"

    return result

# End of obj_to_str()


#######################################################################
# Setup global paths
#######################################################################

#======================================================================
# Initialize some global paths.
#======================================================================

build_dir         = os.path.dirname(__file__).replace('\\', '/')
targ_root         = os.path.realpath(os.path.join(build_dir, "../../..")).replace('\\', '/')
build_root        = os.path.realpath(os.path.join(build_dir, "../..")).replace('\\', '/')
config_root       = os.path.join(build_root, "config").replace('\\', '/')
tools_root        = os.path.join(build_root, "tools").replace('\\', '/')
qcscons_root      = os.path.join(tools_root, "build/scons").replace('\\', '/')
qcscons_build_dir = os.path.join(qcscons_root, "build").replace('\\', '/')
default_ms_dir    = os.path.join(config_root, "default/ms").replace('\\', '/')
default_dir       = os.path.join(config_root, "default").replace('\\', '/')

rel_build_dir     = os.path.relpath(build_dir, targ_root)

#List of legal paths for internal_build_cfg.xml
build_cfg_dir_list = [default_dir,default_ms_dir,config_root]

#======================================================================
# Functions to get global paths
#======================================================================

def get_build_dir         (): return build_dir
def get_targ_root         (): return targ_root
def get_build_root        (): return build_root
def get_config_root       (): return config_root
def get_qcscons_build_dir (): return qcscons_build_dir
def get_rel_build_dir     (): return rel_build_dir

#======================================================================
# Get Target.builds file
#======================================================================

def get_target_builds():
    for path in build_cfg_dir_list:
        target_builds_fn  = os.path.join(path, 'target.builds')
        if os.path.exists(target_builds_fn):
            return target_builds_fn
    return None
# End of get_target_builds()


#######################################################################
# Environ Data
#######################################################################

#======================================================================
# get_tool_versions
#======================================================================

def get_tool_versions ():

    tool_versions = {}

    if sys.platform.startswith('win'):
        version_elem_name = 'windows_version'
    else:
        version_elem_name = 'linux_version'

    environ = build_cfg_root.find('environ')
    if environ != None:
        for elem in environ:
            try:
                tool = get_text_attr(elem, 'name')
                ver  = get_element_text(elem.find(version_elem_name))
                tool_versions[tool] = ver
            except:
                raise ConfigFormatError ("Missing <" + version_elem_name +
                                          "> element for tool " + tool)
    else:
        # Default to Python 2.7.5
        tool_versions = None

    return tool_versions

# End of get_tool_versions()


#======================================================================
# check_tool_version
#======================================================================

def check_tool_version (tool, cur_ver):

    '''
Check the specified tool version against what's in the build_cfg.xml file.
The ver parameter is expected to be a string.  The check_tool_version will
return True if the version specified in the build_cfg.xml file matches the
start of the given version, up to the length of the version specified in
build_cfg.xml.  In other words, if build_cfg.xml only specifies a major and
minor version, and the version given as a parameter specifies a major, minor
and micro version, the micro version will be ignored.
'''

    # Default to False if the version info doesn't exist.
    ret_val = True

    environ = build_cfg_root.find('environ')
    if environ != None:
        for elem in environ:
            if get_text_attr(elem, 'name') == tool:
                if sys.platform.startswith('win'):
                    need_ver = get_element_text(elem.find('windows_version'))
                else:
                    need_ver = get_element_text(elem.find('linux_version'))
                if need_ver and not cur_ver.startswith(need_ver):
                    ret_val = False
                break

    return ret_val

# End of check_tool_version()


#======================================================================
# setenv_and_restart_if_needed
#======================================================================

def setenv_and_restart_if_needed (cmd_line, logger=None):

    # Get tool versions
    tool_versions = get_tool_versions()

    # Setenv will setup our path and other environment variables appropriately
    # for the tool versions specified in build_cfg.xml.
    if use_setenv:
        setenv (tool_versions)

    # If the python version isn't the expected version, and we've run
    # setenv (which may have updated the path environment to allow us to
    # find the correct Python version), and we haven't already restarted,
    # then try re-starting our tool.  We'll add the --restarted option to
    # allow us to avoid recursively restarting the tool if the correct version
    # can't be found.
    if not check_tool_version ('python', platform.python_version()):
        ret = True
        if use_setenv and '--restarted' not in cmd_line:
            # If the stars align, try re-starting.
            if logger:
                logger.log("Restarting Python to get the right version")
                # Delete the logger so that the restarted process can use it.
                logger.close()
            else:
                print "Restarting Python to get the right version"
            index = 0
            for line in cmd_line:
               if line.find('bparams') != -1:
                  bparams = line.split('=', 1)
                  cmd_line[index] = bparams[0] + '="' + bparams[1] + '"'
                  break
               index += 1
            ret = subprocess.call(' '.join(['python'] + cmd_line + 
                                           ['--restarted']), shell=True)

            # Don't return to our caller if we restarted
            sys.exit(ret)

        elif 'PYTHONBIN' not in os.environ:
            # SCons uses 'PYTHONBIN' to override the default Python version,
            # so if this is defined, let the build continue.
            # Else, raise an exception.
            raise VersionError ('Unsupported Python Version: ' +
                                platform.python_version() + 
                                ', expected ' +
                                tool_versions['python'])

    else:
        ret = True

    # Remove '--restarted' from the cmd_line if present.
    try:
        cmd_line.remove('--restarted')
    except ValueError:
        # Ignore exception if '--restarted' isn't present in the cmd_line
        pass

    return ret

# End of setenv_and_restart_if_needed()


#######################################################################
# Build Data
#######################################################################

#======================================================================
# get_pl_name
#======================================================================

def get_pl_name (variant_info=None,default=None):

    pl_name = None

    parent_el = build_cfg_root.find('build_data')
    if parent_el == None:
        parent_el = variant_info
    
    if parent_el != None:
        pl_name = get_element_text (parent_el.find('si_name'), '')
        if pl_name == '':
            pl_name = get_element_text (parent_el.find('pl_name'), '')
    
    if pl_name == '':
        pl_name = default
        
    return pl_name

# End of get_pl_name()

#######################################################################
# Client Info
#######################################################################

#======================================================================
# Data structure with client data
#======================================================================

class client_data_type:

    '''Fields include:
    name:               Text field holding the name of the client.
    strip:              Boolean flag indictating whether or not strip needs to be done for this client.
    pack:               Boolean flag indictating whether or not pack needs to be done for this client.
    variants:           A list of variants to be built for this client.
    pre-scripts:        A list of python scripts to run for this client.
    formats:            A set of all formats for the client
    parent_formats:     A set of formats that form the parent of other formats.
    child_formats:      A set of formats that depend on other formats.
    source_formats:     A set of all source formats for the client
    binary_formats:     A set of all binary formats for the client
    recompile_formats:  A set of all source formats requiring a recompile test
    validate_formats:   A set of all source formats requiring validation of RSRC file compilation
    recompile_variants: A dictionary of variants that needs to be recompiled for a given format'''

    
    def __init__ (self, client_et):

        self.name               = get_text_attr    (client_et, 'name')
        self.strip              = get_boolean_attr(client_et, 'strip')
        self.pack               = get_boolean_attr(client_et, 'pack')
        self.strip_audio_avs    = get_boolean_attr(client_et, 'strip_audio_avs')
        self.variants           = []
        self.pre_scripts        = []
        self.formats            = set()
        self.parent_formats     = set()
        self.child_formats      = set()
        self.source_formats     = set()
        self.binary_formats     = set()
        self.recompile_formats  = set()
        self.validate_formats   = set()
        self.recompile_variants = {}

        #------------------------------------------------------
        # Initialize variants data
        #------------------------------------------------------

        variants = get_element_text(client_et.find('variants'))
        if variants:
            valid_variants = sorted([x.strip() for x in variants.split(',')])
        variants = os.environ.get('SERVICE_VARIANTS')
        if variants != None:
            service_variants = sorted([x.strip() for x in variants.split(',')])
            for variant in service_variants:
                if variant in valid_variants:
                    self.variants.append(variant)
                else:
                    raise ConfigError("Variant " + variant + 
                                      " specified in SERVICE_VARIANTS is not supported by client " +
                                      self.name)
        else:
            self.variants = valid_variants

        #------------------------------------------------------
        # Initialize pre-script data
        #------------------------------------------------------
        for pscripts in client_et.findall('pre-script'):
            self.pre_scripts.append([get_element_text(pscripts.find('name')),
                                     get_element_text(pscripts.find('script'))])

        #------------------------------------------------------
        # Initialize formats data
        #------------------------------------------------------

        if self.pack:

            #------------------------------------------------------
            # Get a list of all formats configured for this client
            #------------------------------------------------------

            formats = get_element_text(client_et.find('formats'))
            if formats:
                valid_formats = set([x.strip() for x in formats.split(',')])
            else:
                raise ConfigError ("Client " + self.name + 
                                   " requires a list of formats.")

            client_formats = []
            service_formats = os.environ.get('SERVICE_FORMATS')
            if service_formats != None:
                service_formats = set([x.strip() for x in 
                                       service_formats.split(',')])
                for format in service_formats:
                    if format in valid_formats:
                        client_formats.append(format)
                    else:
                        raise ConfigError("Format " +
                                          format +
                                          " specified in SERVICE_FORMATS is not supported by client " +
                                          self.name)
            else:
                client_formats = valid_formats
                
            format_dict = get_format_dict()

            for format_name in client_formats:
                format = format_dict[format_name]
                self.formats.add(format)
                if format.parent:
                    parent_format = format_dict[format.parent]
                    self.parent_formats.add(parent_format)
                if format.source:
                    self.source_formats.add(format)
                elif format.binary:
                    self.binary_formats.add(format)
                if format.recompile:
                    self.recompile_formats.add(format)
                if format.validate_rs_files:
                    self.validate_formats.add(format)
                    
            self.child_formats  = self.source_formats - self.parent_formats
        
        #------------------------------------------------------
        # Initialize recompile_variants data
        #------------------------------------------------------
        format_overrides_et = client_et.find('format_overrides')
        if format_overrides_et != None:
            for format_et in format_overrides_et.findall('format'):
                recomp_variants = get_element_list(format_et.find('variants'))
                if recomp_variants:
                    self.recompile_variants[format_et.get('name')] = recomp_variants
                else:
                    self.recompile_variants[format_et.get('name')] = set()
    # End of __init__()


    #======================================================================
    # str function used for debugging.
    #======================================================================

    def __str__ (self):

        return obj_to_str(self)

    # End of __str__()

# End of class client_data_type

#======================================================================
# Get Client Data
#======================================================================

def get_client (client_name):

    client_data = None

    client_list = build_cfg_root.find('clients')

    if client_list != None:
        for client_et in client_list:
            if ('name' in client_et.attrib and 
                client_et.attrib['name'] == client_name):
                client_data = client_data_type(client_et)
                break

    if not client_data:
        raise ConfigError("Client '" + client_name + "' not found.")

    return client_data

# End of get_client()

#======================================================================
# List clients
#======================================================================

def list_clients ():

    client_list = build_cfg_root.find('clients')
    if client_list != None:
        print "Available clients are:"
        for client in client_list:
            print '    ', get_text_attr(client, 'name')
    else:
        print "No clients are available."

# End of list_clients()

#======================================================================
# List client/??? mappings
#======================================================================

def list_client_mappings (map_to):

    row_div = '|' + 22*'-' + '|' + 27*'-' + '|'
    template = "| {client:20} | {mapee:25} |"

    # Print headings
    print row_div
    print template.format(client = 'clients', mapee = map_to)
    print row_div

    client_list = build_cfg_root.find('clients')
    if client_list != None:
        for client in build_cfg_root.find('clients'):
            client_name = get_text_attr(client, 'name')
            mapees = [x.strip() for x in 
                      get_element_text(client.find(map_to), '').split(',')]

            for mapee in mapees:
                print template.format(client = client_name, mapee = mapee)
                # Only list client name on the first row for the client
                client_name = ''

            print row_div     # End the client

    else:
        print "No clients are available."

# End of list_client_mappings()

#======================================================================
# List client/variant mappings
#======================================================================

def list_client_variant_mappings ():

    list_client_mappings('variants')
#    print "Client/Version mappings are:"
#    for client in build_cfg_root.find('clients'):
#        print '    ', get_text_attr(client, 'name')
#        variants = get_element_text(client.find('variants'), '').split(',')
#        for variant in variants:
#            print '        ', variant

# End of list_client_variant_mappings()

#======================================================================
# List client/format mappings
#======================================================================

def list_client_format_mappings ():

    list_client_mappings('formats')
#    print "Client/Version mappings are:"
#    for client in build_cfg_root.find('clients'):
#        print '    ', get_text_attr(client, 'name')
#        variants = get_element_text(client.find('formats'), '').split(',')
#        for variant in variants:
#            print '        ', variant

# End of list_client_format_mappings()


#######################################################################
# Workflows
#######################################################################

#======================================================================
# Get Workflow for client
#======================================================================

def get_workflow (client_name):

    workflow_root = None

    client_workflow_et_node = build_cfg_root.find('client_workflows')
    if client_workflow_et_node != None:
        for client in client_workflow_et_node:
            if client_name in client.attrib['name']:
                workflow_root = client

    return workflow_root

# End of get_workflow()


#######################################################################
# Binary Files - Files that get shipped in BIN format directory
#######################################################################

#======================================================================
# Get Bin Files
#======================================================================

def get_bin_files (format):

    bin_files = None
    bin_formats = build_cfg_root.find('bin_formats')
    if bin_formats is None:
        bin_files = build_cfg_root.find('bin_files')
    else:
        for bin_format in bin_formats:
            if (bin_format.get('name') == format.name):
                bin_files = bin_format
                break

    if bin_files is None or len(bin_files) == 0:
        raise Exception("No binary files found for format '" + 
                        format.name + "'")

    return bin_files

# End of get_build_dir()


#######################################################################
# Format Data
#######################################################################

#======================================================================
# class format_data_type
#======================================================================

class format_data_type:

    def __init__(self, format_etree):

        self.name                   = get_element_text(format_etree.find('name'))
        self.binary                 = get_boolean_attr(format_etree, 'binary')
        self.primary                = get_boolean_attr(format_etree, 'primary')
        self.full_build             = get_boolean_attr(format_etree, 'full_build')
        self.parent                 = get_element_text(format_etree.find('parent'))
        self.update_uses            = get_boolean_attr(format_etree, 'update_uses')
        self.recompile              = get_boolean_attr(format_etree, 'do_compile_test')
        self.validate_rs_files      = get_boolean_attr(format_etree, 'validate_rs_files')
        self.validate_private_files = get_boolean_attr(format_etree, 'validate_private_files')
        self.build_flags            = get_element_list(format_etree.find('build_flags'))
        self.source                 = get_boolean_attr(format_etree, 'source')

    # End of __init__()


    #======================================================================
    # str function used for debugging.
    #======================================================================

    def __str__ (self):

        return obj_to_str(self)

    # End of __str__()

# End of class format_data_type


#======================================================================
# get_format()
#======================================================================

def get_format (format_name):
    '''Return data for the specified format.'''

    return get_format_dict()[format_name]

# End of get_format()


#======================================================================
# get_format_dict()
#======================================================================

format_data_dict = {}

def get_format_dict ():
    '''Return a dictionary of format names to format data.'''

    global format_data_dict

    if len(format_data_dict) == 0:

        release_formats = build_cfg_root.find('release_formats')

        if release_formats != None:
            for format_etree in release_formats:
                format_data = format_data_type(format_etree)
                format_data_dict[format_data.name] = format_data

        else:
            raise ConfigError("No release formats in configuration data.")

    return format_data_dict

# End of get_format_dict()


#######################################################################
# Validation Rules
#######################################################################

#======================================================================
# get_proprietary_files()
#======================================================================

def get_private_files ():

    private_files = []

    validation_rules = build_cfg_root.find('validation_rules')

    if validation_rules != None:
        private_files = get_element_text( validation_rules.
                                          find('private_files'), '').split()

    return private_files

# End of get_proprietary_files()

#######################################################################
# Config File Maintenance
#######################################################################

#======================================================================
# clean_build_config()
#======================================================================

def clean_build_config (client_name = None, pl_name = None):

    if os.path.exists(internal_build_cfg_fn):

        # Create new xml trees so that the old trees aren' affected
        # when we remove things.

        tmp_tree_str = et.tostring(build_cfg_root)
        tmp_build_cfg_root = et.fromstring(tmp_tree_str)
        tmp_build_cfg_tree = et.ElementTree(tmp_build_cfg_root)

        clean_legacy             (tmp_build_cfg_root, pl_name)
        clean_qc_internal      (tmp_build_cfg_root)
        if client_name != None:
            clean_unused_clients_and_variants (tmp_build_cfg_root, client_name)

        tmp_build_cfg_tree.write (build_cfg_fn)
        
# End of clean_build_config()

#======================================================================
# clean_legacy
#======================================================================

def clean_legacy (e, pl_name):

    #remove pre-scripts node in pl,chip and variant levels and remove other PLs
    base_pl_name = get_pl_name()
    if base_pl_name and pl_name:
        if base_pl_name != pl_name:
            raise ConfigError("Mismatched PL names: " +
                               base_pl_name + " and " + pl_name)
    elif base_pl_name:
        pl_name = base_pl_name
    elif not pl_name:
        raise ConfigError("No PL name.")

    pl_info_internal = e.find('variant_info')
    if pl_info_internal is None:
        pl_info_internal = e.find('pl_info')
    if pl_info_internal != None:
        cur_pl_name = get_pl_name(pl_info_internal)
        if cur_pl_name:
            # MPSS.DI.1.0, MPSS.TR.1.0 and MPSS.DI.1.1 are two xml nodes in 
            # internal_build_cfg.xml. The following code will remove 
            # MPSS.TR.1.0 and MPSS.DI.1.1
            if pl_name != cur_pl_name.strip():
                e.remove(pl_info_internal)
            for pscripts in pl_info_internal.findall('pre-script'):
                #remove at chip level
                pl_info_internal.remove(pscripts)
        for chip_info in pl_info_internal.findall('chip_info'):
            for pscripts in chip_info.findall('pre-script'):
                #remove at chip level
                chip_info.remove(pscripts)
            for var_info in chip_info.findall('build_variant'):
                for pscripts in var_info.findall('pre-script'):
                    #remove at variant level
                    var_info.remove(pscripts)

# End of clean_legacy()

#======================================================================
# clean_qc_internal
#======================================================================

def clean_qc_internal (e):

    # Remove all 'qc_internal' nodes

    for child in e.getchildren()[:]:
        if get_boolean_attr(child, 'qc_internal'):
            e.remove(child)
        else:
            clean_qc_internal(child)

# End of clean_qc_internal()

#======================================================================
# clean_unused_clients_variants
#======================================================================

def clean_unused_clients_and_variants (e, client_name):

    # Find the specified client element
    variants = []
    client_list = e.find('clients')
    if client_list != None:
        for client in client_list.findall('client'):
            if get_text_attr(client, 'name') == client_name:
                variant_list = get_element_text(client.find('variants'))
                if variant_list != None:
                    variants = [x.strip() for x in variant_list.split(',')]
            else:
                client_list.remove(client)

    # Create dictionary of chipset to flavor to purpose for all of the variants
    v_map = {}
    for var in variants:
        chipset, flavor, purpose = var.split('.')
        if chipset not in v_map.keys():
            v_map[chipset] = {}
        if flavor not in v_map[chipset].keys():
            v_map[chipset][flavor] = []
        if purpose not in v_map[chipset][flavor]:
            v_map[chipset][flavor].append(purpose)

    # Cycle through pl_info records to remove chipsets, flavors, purposes
    # not defined for this client.

    # Terminology has changed:
    #    Old, but still used in build_cfg.xml:
    #          Variant Cluster = <chip>.<variant>.<purpose>
    #    New, and used here in the code:
    #          Variant = <chipset>.<flavor>.<purpose>

    pl_info = e.find('variant_info')
    if pl_info is None:
        pl_info = e.find('pl_info')
    if pl_info != None:
        for chipset in pl_info.findall('chip_info'):
            chip_name = get_element_text(chipset.find('chip_name'))
            if chip_name not in v_map.keys():
                pl_info.remove(chipset)
            else:
                for flavor in chipset.findall('build_variant'):
                    flavor_name = get_element_text(flavor.find('variant_name'))
                    if flavor_name not in v_map[chip_name].keys():
                        chipset.remove(flavor)
                    else:
                        for purpose in flavor.findall('purpose'):
                            purpose_name = get_element_text(purpose.
                                                            find('purpose_name'))
                            if (purpose_name not in 
                                v_map[chip_name][flavor_name]):
                                flavor.remove(purpose)

# End of clean_unused_clients_and_variants()


#######################################################################
# Initial Script Execution
#######################################################################

#======================================================================
# Find and load the build_cfg.xml file.
#======================================================================

build_cfg_fn              = os.path.join(build_dir,    'build_cfg.xml')
common_build_cfg_fn     = os.path.join(build_dir,    'common_build_cfg.xml')

internal_build_cfg_exists = False
for path in build_cfg_dir_list:
    internal_build_cfg_fn  = os.path.join(path, 'internal_build_cfg.xml')
    if os.path.exists(internal_build_cfg_fn):
        internal_build_cfg_exists = True
        build_cfg_tree = et.ElementTree(file = internal_build_cfg_fn)
        build_cfg_root = build_cfg_tree.getroot()
        if os.path.exists(common_build_cfg_fn):
            common_build_cfg_tree = et.ElementTree(file = common_build_cfg_fn)
            common_build_cfg_root = common_build_cfg_tree.getroot()
            for common_build_cfg_element in common_build_cfg_root:
                duplicate_tag = build_cfg_root.find(common_build_cfg_element.tag)
                if duplicate_tag is None:
                    if build_cfg_root[-1].tag == "description" :
                        build_cfg_root.insert(-1,common_build_cfg_element)
                    else:
                        build_cfg_root.append(common_build_cfg_element)
        break

if not internal_build_cfg_exists:
    build_cfg_tree = et.ElementTree(file = build_cfg_fn)

build_cfg_root = build_cfg_tree.getroot()

#======================================================================
# This script is normally loaded through in import statement.  This is
# just a place holder in case we decide to add some functionality
# from the command line, such as dump some or all of the contents
# of build_cfg.xml or adding test code.
#======================================================================

if __name__ == "__main__":

    # Test code
    setenv_and_restart_if_needed(sys.argv)

