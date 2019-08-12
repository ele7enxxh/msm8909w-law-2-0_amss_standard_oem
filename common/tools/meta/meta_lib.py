'''
===============================================================================

 Meta Library

 GENERAL DESCRIPTION
    This script contains code for extracting meta-information from the
    contents.xml file.

 Copyright (c) 2011-2012 by QUALCOMM, Incorporated.
 All Rights Reserved.
 QUALCOMM Proprietary/GTDR

-------------------------------------------------------------------------------

  $Header: //source/qcom/qct/tools/meta/00/01/common/tools/meta/meta_lib.py#52 $
  $DateTime: 2014/10/13 18:37:48 $

===============================================================================
'''

import sys
import os
import subprocess
from xml.etree import ElementTree as et
from xml.dom import minidom
import re
from glob import glob
import meta_log

FINDABLE = True
try:
   import fb_info_lib
except:
    FINDABLE = False


# Fix for minidom prettyprint function from:
#   http://ronrothman.com/public/leftbraned/xml-dom-minidom-toprettyxml-and-silly-whitespace/
# (with some editing to suit out purposes.)
def fixed_writexml(self, writer, indent="", addindent="", newl=""):
   # indent = current indentation
   # addindent = indentation to add to higher levels
   # newl = newline string
   writer.write(indent+"<" + self.tagName)

   attrs = self._get_attributes()
   a_names = attrs.keys()
   #donb: a_names.sort()

   for a_name in a_names:
      writer.write(" %s=\"" % a_name)
      minidom._write_data(writer, attrs[a_name].value)
      writer.write("\"")
   if self.childNodes:
      if len(self.childNodes) == 1 \
         and self.childNodes[0].nodeType == minidom.Node.TEXT_NODE:
         writer.write(">")
         self.childNodes[0].writexml(writer, "", "", "")
         writer.write("</%s>%s" % (self.tagName, newl))
         return
      writer.write(">%s"%(newl))
      for node in self.childNodes:
         if node.nodeType != minidom.Node.TEXT_NODE:
            node.writexml(writer,indent+addindent,addindent,newl)
      writer.write("%s</%s>%s" % (indent,self.tagName,newl))
   else:
      writer.write("/>%s"%(newl))
# replace minidom's function with ours
minidom.Element.writexml = fixed_writexml

class meta_info:

   #----------------------------------------------------------#
   # Public Methods                                           #
   #----------------------------------------------------------#
   default_file_types = ['file_ref','download_file', 'dload_boot_image','partition_file','partition_patch_file','dir_ref']

   def __init__ (self, logger = None, file_pfn = None):
      '''Initialize by reading from the meta_file.'''

      # Set the path for contents.xml
      meta_file_pfn = ''
      root_dir =  os.path.join( os.path.dirname( __file__ ), '../../../' )
      if not file_pfn:
        meta_file_pfn = os.path.join( root_dir , 'contents.xml')
      else:
         meta_file_pfn = file_pfn
      
      # Set path for contents_oem.xml
      path_oem_file = os.getenv('SCONS_BUILD_SCRIPTS_OEM_ROOT')
      if not path_oem_file:
         if os.path.exists(os.path.join( os.path.dirname( __file__ ), '../../../contents_oem.xml')):
            path_oem_file = os.path.join( os.path.dirname( __file__ ), '../../../')    # if oem variable is not se, looking for contents_oem.xml in root directory
      self.meta_info_pfn = os.path.abspath(meta_file_pfn)   # 'pfn' => path/file-name
      self.meta_info_fn  = os.path.basename(meta_file_pfn)  # 'fn' => file name
      self.logger = logger
      self.meta_oem_file_pfn = '' # path to OEM customer's contents_oem.xml, "necessary" to initialize it to empty string

      # Initialize the XML tree
      if os.path.exists(self.meta_info_pfn):
         self.XmlTree = et.ElementTree(file = self.meta_info_pfn)
         self.XmlRoot = self.XmlTree.getroot()

         if path_oem_file: # check if the environment variable SCONS_BUILD_SCRIPTS_OEM_ROOT is defined ( usually it is only defined in OEM customer's environment )
            self.log('SCONS_BUILD_SCRIPTS_OEM_ROOT is defined as: ' + path_oem_file )
            self.meta_oem_file_pfn = os.path.abspath( os.path.join( path_oem_file , 'contents_oem.xml') )
         
            if os.path.exists( self.meta_oem_file_pfn ):  # check if contents_oem.xml exists at the path specified by the system variable SCONS_BUILD_SCRIPTS_OEM_ROOT
               self.log('contents_oem.xml found at path: ' + self.meta_oem_file_pfn )
               self.OemXmlTree = et.ElementTree( file = self.meta_oem_file_pfn )
               self.OemXmlRoot = self.OemXmlTree.getroot()
               
               # check if OEM merge is already done, if not, merge the OEM flavor information
               oem_merge_already_done = self.__is_oem_merge_alreadydone()
               if not oem_merge_already_done:
                  self.log('Performing OEM flavors merge...')
                  self.__merge_oem()
                  self.save( os.path.join( root_dir , 'contents.xml') )
                  self.log('contents.xml saved with OEM customer information')
               else:
                  self.log('OEM flavors are already merged with contents.xml...')
      else:
         # Generate default structure
         self.XmlRoot = et.Element('contents')
         self.XmlTree = et.ElementTree(element = self.XmlRoot)
         et.SubElement(self.XmlRoot, 'builds_flat')

      # Set platform flag
      self.on_linux = sys.platform.startswith("linux")

      # Initialize the findbuild object
      if FINDABLE:
         self.fb = fb_info_lib.fbInfo(self.on_linux)
      
      # Build an internal flavor dictionary for easier access
      self.flavor_data = self.__build_flavor_data()
      self.__do_integrity_check()

      
   def save ( self , fName = None ):

      # Do stuff here that we only want to do once we know that the
      # contents data is fully updated.
      self.flavor_data = self.__build_flavor_data()

      # -- This method doesn't insert whitespace for readability
      #self.XmlTree.write(self.meta_info_pfn)
      # -- Minidom pretty printing is inserting white space in the strings,
      # -- so commenting it out for now.
      txt = et.tostring(self.XmlRoot)
      txt = minidom.parseString(txt).toprettyxml(indent = '  ')
#      txt  = '<?xml version="1.0" encoding="utf-8"?>\n'
#      txt += XmlPrettyPrint(self.XmlRoot)
      
      if not fName:
         fName = self.meta_info_pfn

      meta_file = open( fName, 'w')
      meta_file.write(txt)
      meta_file.close()

   def remove_files (self, rm_list):
      file_type_list = self.default_file_types
      builds_flat = self.XmlRoot.find('builds_flat')

      for build in builds_flat.getchildren():
         for ftl in file_type_list:
            file_types = build.findall(ftl)
            for file_type in file_types:
               file_name_elem = file_type.find('file_name')
               if file_name_elem.text:
                  file_name = file_name_elem.text.strip()
                  for rm_str in rm_list:
                     if fnmatch.fnmatch(file_name, rm_str):
                        self.log('File type element was removed for ' + file_name )
                        build.remove(file_type)
      self.save( self.meta_info_pfn )
   
   def remove_files_with_tag(self,tag_list = ["clean_for_release"],desc = "HY11"):
      file_types = self.default_file_types
      builds_flat = self.XmlRoot.find('builds_flat')
      for build in builds_flat.getchildren():
         build_file_list = get_all_files_for_build(build, file_types)
         for tag in tag_list: 
            build_file_tag_list = filter_files_by_attr(build_file_list,tag,desc)       
            for file in build_file_tag_list:
               build.remove(file)    
      self.save( self.meta_info_pfn )      

   def clean_file_paths(self):
      file_types = self.default_file_types
      builds_flat = self.XmlRoot.find('builds_flat')
      for build in builds_flat.getchildren():
         build.find('linux_root_path').text = "./"
         build.find('windows_root_path').text = ".\\"
         for file in get_all_files_for_build(build,file_types):
            file.find('file_path').text = clean_vars(file.find('file_path').text.strip())                 
      self.save( self.meta_info_pfn )


   def update_pl_from_buildid(self, build_id):
      if not FINDABLE:
         self.log('meta_lib.py: Information for build ' + build_id + ' cannot be found.')
         self.log('meta_lib.py: Skipping PL/CHIP update.')
         return
      
      fbInfo = self.fb.get_fbInfo(build_id)
      try:
         product_info = self.XmlRoot.find( 'product_info')
         pl = fbInfo['ProductLine']
         product_info.find ('product_name').text = pl
         self.log('meta_lib.py: PL is updated to ' + pl)
      except:
         self.log('meta_lib.py: Unable to update PL info.')
         if self.logger:
            self.logger.log_exception()
      try:
         if (product_info.find ('chipid').text is None):
            chip_id = fbInfo['HWPlatform']
            product_info.find ('chipid').text = chip_id
            self.log('meta_lib.py: Chip ID is updated to ' + chip_id)
      except:
         self.log('meta_lib.py: Unable to update Chip info.')
         if self.logger:
            self.logger.log_exception()
   #end of update_pl_from_buildid()
   
   
   def clean_product_name(self):
      product_info = self.XmlRoot.find('product_info')
      product_name_text = product_info.find('product_name').text
      filtered_product_name = re.search (r'.+(?=\.r\d+)', product_name_text)
      if filtered_product_name:
         product_info.find('product_name').text = filtered_product_name.group()
      else: 
         product_info.find('product_name').text = product_name_text
   #end of clean_product_name()

   def update_build_info (self, tag, loc_list):
      # "loc" must be a list of paths
      if type(loc_list) is not list:
         loc_list = [loc_list]
         
      # Loop through each path and update corresponding element
      for loc in loc_list:
         # Find build IDs, path info, from pair
         if loc.count('/'):
            # It's a Linux path
            self.log('meta_lib.py: Linux path provided for ' + tag + ' - ' + loc)
            build_id = None
            lin_path = loc
            win_path = None
            if not os.path.exists(loc):
               return 1  #path not there
         elif loc.count('\\'):
            # It's a Windows path
            self.log('meta_lib.py: Windows path provided for ' + tag + ' - ' + loc)
            build_id = None
            lin_path = None
            win_path = loc
            if not os.path.exists(loc):
               return 1  #path not there
         else:
            # It's a Build ID
            # Assume we're running in QC environment, and
            # findbuild is present.  Customer environments should only specify
            # paths for this function, not build IDs.
            self.log('meta_lib.py: Build ID provided for ' + tag + ' - ' + loc)
            build_id = loc
            if tag!='common':
               self.log('meta_lib.py: Getting windows and linux paths for ' + build_id)
               mpath = self.__get_new_path(build_id)
               lin_path = mpath.lin_path
               win_path = mpath.win_path
               if not lin_path and not win_path:
                  return 1 #build id not there
            
      # Update build ID, paths in meta data
      # Read each CRM build from 'flat builds' and add it.
      builds_flat = self.XmlRoot.find('builds_flat')
      flat_build  = find_named_element(builds_flat, tag)
      flat_build.find('build_id').text = build_id
      if tag != 'common':
         self.log('meta_lib.py: Updating ' + tag + ' root paths in contents.xml')
         flat_build.find('windows_root_path').text = win_path
         flat_build.find('linux_root_path').text   = lin_path
               
         # Get build commands from images and update in contents
         self.log('meta_lib.py: Updating ' + tag + ' make commands in contents.xml')
         if self.on_linux:
            mainmake_string = self.get_image_mainmake(lin_path, build_id)
         else:
            mainmake_string = self.get_image_mainmake(win_path, build_id)
         
         # Split the mainmake into two parts, relative path and build command
         mainmake_split = mainmake_string.split(' ',1)            # separate the arguments
         mainmake_split[0] = mainmake_split[0].replace('\\','/')   # change all to forward slashes
         buildfile_path = mainmake_split[0].rsplit('/',1)[0]      # now separate relative path and build filename
         build_command = mainmake_split[0].rsplit('/',1)[-1] + " " + mainmake_split[-1] # -1 works when mainmake_string is empty
         # Save the filepath and build command to contents.xml
         find_element_or_create(flat_build,'buildfile_path').text = buildfile_path + '/'
         find_element_or_create(flat_build,'build_command').text = build_command


   def update_var_values (self, var, val):
      '''Update variable values in meta-data:
   var: String name of variable to update.
   val: String value to update variable with.'''

      # Search text of every element in the tree to look for instances of the
      # variable.  Variables are surrounded by ${var_name}, and may already
      # contain a value, such as: ${var_name:value}.
      # Currently, this pattern can only match one var/value pair per string.
      pattern = "\A(.*\${%s*)(?::[^}]*)?(}.*)\Z" % var
      re_obj = re.compile(pattern)
      self.__update_var_values_recurse(re_obj, self.XmlRoot, var, val)

   def __update_var_values_recurse (self, re_obj, cur_elem, var, val):
      '''Private function called by update_var_values()'''
      text = cur_elem.text
      # Look for "${" first, since this is much faster than a regex search
      if ((text != None) and (text.count('${') >= 1)):
         # Look for a matching variable in our string
         match_obj = re_obj.search(cur_elem.text)
         if match_obj:
            groups = match_obj.groups()
            new_text = groups[0] + ':' + val + groups[1]
            cur_elem.text = new_text

      # Now recurse to all children
      for child in cur_elem.getchildren():
         self.__update_var_values_recurse(re_obj, child, var, val)

   def update_to_release_path(self, tag, attr='src'):
       '''Updates the build path with release folder path'''
       builds_flat = self.XmlRoot.find('builds_flat')
       flat_build  = find_named_element(builds_flat, tag) 
       linux_path_elem     = flat_build.find('linux_root_path')
       windows_path_elem   = flat_build.find('windows_root_path')
       if linux_path_elem.text != None:
           linux_path = linux_path_elem.text.strip()
       else:
           linux_path = None
       
       if windows_path_elem.text:
           windows_path = windows_path_elem.text.strip()
       else:
           windows_path = None
   
       release_path=self.get_release_path(tag, attr)
       
       if release_path:
           if windows_path:
              flat_build.find('windows_root_path').text = join_paths([windows_path, release_path], '\\') + '\\'
           if linux_path:
              flat_build.find('linux_root_path').text = join_paths([linux_path, release_path], '/') + '/'

   #End of update_to_release_path
   
   def filter_product_flavors(self, selected_pf_list):
      '''
      Removes unselected flavors from the list of all flavors
      Reads the used flavors from <product_flavor> section
      and removes any files in this build which are of other flavors
      '''
      self.log("===== Filtering unused product flavors =====")
      self.log("Selected flavors = " + ', '.join(selected_pf_list))
      product_flavors = self.XmlRoot.find('product_flavors')
      component_flavor_dict = {}
      unwanted_pf_elems = []
      if product_flavors is not None:

         # Remove <pf> sections of unspecified flavors from <product_flavors>
         for pf_elem in product_flavors:
            pf_name = pf_elem.find('name').text.strip()
            if pf_name not in selected_pf_list:
               unwanted_pf_elems.append(pf_elem)
            else: #If flavor is selected, create a dictionary
               for component_elem in pf_elem.findall('component'):
                  name = component_elem.find('name').text
                  flavor = component_elem.find('flavor').text
                  # Check and add to dictionary as {build : [flavors]}
                  if name in component_flavor_dict:
                     component_flavor_dict[name] = set(component_flavor_dict[name]).add(flavor)
                  else:
                     component_flavor_dict[name] = set([flavor])
         if len(unwanted_pf_elems) > 0:
            for pf_elem in unwanted_pf_elems:
               self.log("Deleting " + pf_elem.find('name').text.strip())
               product_flavors.remove(pf_elem)
            
            # After cleaning up <product_flavors> delete files of those removed flavors from <builds_flat>
            self.log("Checking for unused flavor files")
            build_root = self.XmlRoot.find('builds_flat')
            for build_elem in build_root:
               tag = build_elem.find('name').text
               for file_elem in build_elem:
                  # Loop only through the default file types
                  if (file_elem.tag in self.default_file_types):
                     # Check if flavor is specified in the file element itself
                     if not (file_elem.get('flavor') is None or file_elem.get('flavor') in component_flavor_dict[tag]):
                        self.log("   Deleting " + file_elem.tag + " " + file_elem.find('file_name').text)
                        build_elem.remove(file_elem)
                     else:
                        # Check if flavor is specified in file name or path
                        for file_property in file_elem:
                           if not (file_property.get('flavor') is None or file_property.get('flavor') in component_flavor_dict[tag]):
                              self.log("   Deleting " + file_property.tag + " " + file_property.text + " in " + file_elem.find('file_name').text)
                              file_elem.remove(file_property)
                        # After filtering, check if the file element has atleast one name and one path left
                        if file_elem.find('file_name') is None or file_elem.find('file_path') is None:
                           self.log("   Deleting parent " + file_elem.tag + " " + file_elem.find('file_name').text)
                           build_elem.remove(file_elem)
   #End of filter_product_flavors


   def get_build_path (self, tag, get_attrs=False):
      '''Find the current path for the specified build tag'''
      builds_flat = self.XmlRoot.find('builds_flat')
      flat_build  = find_named_element(builds_flat, tag)
      return self.__get_build_path_from_build_elem(flat_build, get_attrs)
   
   #Gets build id from builds_flat   
   #Since nested builds feature is removed, build_base is Unused, but kept to maintain symmetry  
   def get_build_id(self, tag,build_base='Unused',get_attrs=False):
      ''' Find the build id from the specified build tag '''
      build_root = self.XmlRoot.find('builds_flat')
      build_elem  = find_named_element(build_root, tag)
      build_id = build_elem.find('build_id').text     
                
      if build_id:
         build_id = build_id.strip()

      return build_id
   
   def get_build_field (self, build_name, field):
      ''' Find the build id from the specified build tag '''
      build_root = self.XmlRoot.find('builds_flat')
      build_elem  = find_named_element(build_root, build_name)
      ret = build_elem.find(field).text     

      if ret:
         ret = ret.strip()

      return ret
       
   def __get_build_path_from_build_elem(self, build, get_attrs=False):
      if self.on_linux:
         path_elem = build.find('linux_root_path')
      else:
         path_elem = build.find('windows_root_path')

      path = path_elem.text
      if path:
         path = path.strip()
         # If the path is relative, turn it into an absolute path
         # using the directory of the meta info file as the base.
         if ((self.on_linux and not path.startswith('/'))
              or (not self.on_linux and not(path.startswith('\\') or path.find(':\\', 1, 3) == 1))):
            base_path = os.path.realpath(os.path.dirname(self.meta_info_pfn))
            path = os.path.normpath(os.path.join(base_path, path)) + os.path.sep # Keeping slash at the end

      image_dir = build.find('image_dir').text
      build_id = build.find('build_id').text
      if image_dir:
         image_dir = image_dir.strip()
      image_path = os.path.join(path,image_dir)
      if build_id:
         build_id = build_id.strip()
         image_archive_file=build_id+'.7z'
         image_archive_filepath= os.path.join(path,image_archive_file)
      
         if (os.path.exists(image_archive_filepath) and not os.path.exists(image_path)) or not path or not os.path.exists(path) :
            self.log('Build path' + image_path + ' is not accessible either because build ' + build_id + ' is expired or archived. Checking for a new path')
            mpath = self.__get_new_path(build_id)
            if self.on_linux:
               path = mpath.lin_path
            else:
               path = mpath.win_path
            image_path = os.path.join(path,image_dir)
            self.log('Image path ' + image_path)
            if not path or not os.path.exists(path):
               self.log('The build_id ' + build_id + ' is not accessible. Please check if the server on which build is present is not accessible or if the build is expired')
               raise NameError,"The build_id " + build_id + " is not accessible. Please check if the server on which build is present is not accessible or if the build is expired"
            elif not os.path.exists(image_path):
               self.log('The build_id ' + build_id + ' is archived. Please de-archive the image build and try again')
               raise NameError,"The build_id " + build_id + " is archived. Please de-archive the image build and try again"
            elif os.path.exists(image_path):
               self.log('Found new path for build_id ' + build_id)

      if get_attrs:
         return [path, path_elem.attrib]
      else:
         return path


   def get_image_dir (self, tag):
      '''Find the image directory for the specified build tag'''
      builds_flat = self.XmlRoot.find('builds_flat')
      flat_build  = find_named_element(builds_flat, tag)
      image_dir   = flat_build.find('image_dir')
      if image_dir != None:
         return image_dir.text.strip()
      else:
         return None

   def get_release_path (self, tag, attr='src'):

      '''Find the release folder path for the specified build tag''' 

      src_search_paths = ['HY11_CompileTest/SRC*',
                          'HY11_CompileTest/HY11',
                          'HY11_CompileTest/HY11_1',
                          'HY11_CompileTest',
                          'PCT/SRC*',
                          'PCT/HY11*']
      bin_search_paths = ['HK11*',
                          'BIN*']

      builds_flat = self.XmlRoot.find('builds_flat')
      flat_build  = find_named_element(builds_flat, tag)
      build_root_dir = self.__get_build_path_from_build_elem(flat_build)

      release_paths = flat_build.findall('release_path')
      release_path = None
      for rel_path in release_paths:
         if rel_path.attrib is not None and 'type' in rel_path.attrib:
            if rel_path.attrib['type'] == attr:
               release_path = rel_path
               break
         elif attr == 'src':
            release_path = rel_path
            break
      if attr == 'src':
         search_paths = src_search_paths
      elif attr == 'bin':
         search_paths = bin_search_paths
      else:
         search_paths = None

      rel_path_text = None
      if release_path != None:
         if release_path.text != None:
            rel_path_text = clean_vars(release_path.text.strip())
         else:
            rel_path_text = ""
      '''
      #Removing auto-detection for now
      elif search_paths != None:
         # Try to do some reasonable auto-detection.
         for search_path in search_paths:
            rel_path_texts = sorted(glob(os.path.join(build_root_dir, search_path)))
            if len(rel_path_texts) > 0:
               rel_path_text = os.path.relpath(rel_path_texts[0], build_root_dir)
               break
      '''
      if rel_path_text is not None and os.path.isdir(os.path.join(build_root_dir, rel_path_text)):
         return rel_path_text

      if rel_path_text is not None:
         raise InputError('Release directory does not exist or is not a directory: ' + rel_path_text)

      raise InputError('Could not find <release_path> element with type="' + attr + '" for ' + tag + ' build.')

   def get_build_list (self):
      '''Return a list of build names'''
      builds_flat = self.XmlRoot.find('builds_flat')
      build_names = list()
      for child in builds_flat.getchildren():
         if (has_element(child, 'build_id') or
             has_element(child, 'windows_root_path') or
             has_element(child, 'linux_root_path')):
            build_names.append(child.find('name').text.strip())
      return build_names

   def get_product_flavors(self):
      '''
      Return a list of product flavors in the same order they
      appeared in contents.xml.
      '''
      pf_list = []
      product_flavors = self.XmlRoot.find('product_flavors')
      if product_flavors is not None:
         for pf_elem in product_flavors.getchildren():
            prod_flav = pf_elem.find('name').text.strip()
            pf_list.append(prod_flav)

      return pf_list
   
   def get_tool_build (self, tool, root='build_tools'):
      '''Look in the tools tag for the specified tool, and return the
contents of the 'build' child.'''
      tools_elem = self.XmlRoot.find(root)
      tool_elem = find_named_element(tools_elem, tool)
      if tool_elem is not None:
         build_elem = tool_elem.find('build')
         return build_elem.text.strip()
      else:
         return None

   def get_tool_path (self, tool, root='build_tools'):
      '''Look in the tools tag for the specified tool, and return the
contents of the 'path' child.'''
      tools_elem = self.XmlRoot.find(root)
      tool_elem = find_named_element(tools_elem, tool)
      path_elem = tool_elem.find('path')
      return path_elem.text.strip()

   def get_tool_params (self, tool, flavor=None):
      '''Look in the tools tag for the specified tool, and return the
contents of the 'params' child.'''
      tools_elem = self.XmlRoot.find('build_tools')
      tool_elem = find_named_element(tools_elem, tool)
      params_elem = tool_elem.findall('params')
      params_list = []
      
      flavors = self.get_product_flavors()
      if flavor != None:
         # Check if its a valid flavor
         if flavor in flavors:
            # Take this flavor as the only flavor required
            flavors = [flavor]
         else:
            # Return empty list
            self.log("Specified flavor is either invalid or not part of this product")
            return []
         for each_param in params_elem:
            if "flavor" in each_param.attrib and each_param.attrib["flavor"] == flavor:
               params_list.append(clean_vars(each_param.text.strip()))
      else:
         # Find only the element with no flavor attribute
         for each_param in params_elem:
            if "flavor" not in each_param.attrib:
               params_list.append(clean_vars(each_param.text.strip()))
      
      return params_list
               

   def get_var_values (self, attr, cur_elem=None, var_values=None, flavor=None):
      '''Return a dictionary of var/values for the specified attribute.
      This handles the format where an attribute value specifies the name
      of a variable, and the value of the variable is the value of the
      element containing the attribute.

      This is a recursive function.  It is initially called with only
      the attribute name.  The cur_elem and var_values parameters are
      used in recursive calls to sub-elements.'''

      # Initialize
      if var_values is None:
         var_values = dict()
      if cur_elem is None:
         cur_elem = self.XmlRoot

      if cur_elem.attrib and attr in cur_elem.attrib:
         if flavor :
            flavor_list = get_flavor_list(cur_elem)
            if (len(flavor_list) == 0) or (flavor in flavor_list) :
               var_values[cur_elem.attrib[attr]] = clean_vars( cur_elem.text )
         else:
            var_values[cur_elem.attrib[attr]] = clean_vars( cur_elem.text )
      # Now recurse to sub-elements
      for elem in cur_elem.getchildren():
         self.get_var_values(attr, elem, var_values, flavor)

      return var_values

   def get_file_vars (self,
                      attr,
                      file_types=default_file_types,
                      flavor = None,
                      abs = True):
      '''
      Return a list dictionary of attribute value to a list of relative paths to files.

      The idea here is the attribute values are names that are mapped to files
      listed in contents.xml.  The first usage of this is for cmm variables.

      Parameters:
      
         file_types (optional, defaults to 'file_ref'):
      
            A list of types of file to return.  The default is all file types.
      
            Supported file types are:
      
               default_file_types = ['download_file', 'file_ref','dload_boot_image']

         attr (mandatory):
      
            Filter based on the specified attribute.  An attribute is required because
            the return value contains a mapping of the values of the attributes to the
            list of files paths for that value.
      
         flavor (optional, defaults to None):
      
            Filter based on the specified flavor.  If not specified, all files are returned
            regardless of flavor.
      
         abs (optional, defaults to True):
      
            Specifies whether to return absolute or relative paths.  Relative paths are
            relative to the root directory for the build.  Defaults to absolute paths.
            Set to False to return relative paths.
      '''

      if type(file_types) is not list:
         file_types = [file_types]
      builds_flat = self.XmlRoot.find('builds_flat')
      var_list = dict()
      for build in builds_flat.getchildren():
         build_flavor = None
         build_file_list = get_all_files_for_build(build, file_types)

         if attr != None:
            # Filter on attribute
            build_file_list = filter_files_by_attr(build_file_list, attr)

         if flavor != None:
            # Filter on flavor
            build_name = build.find('name').text.strip()
            if (flavor in self.flavor_data and
                build_name in self.flavor_data[flavor]) :
               build_flavor = self.flavor_data[flavor][build_name]
            build_file_list = filter_files_by_flavor(build_file_list, build_flavor)

         for file in build_file_list:
            var = file.attrib[attr]
            if var not in var_list:
               var_list[var] = list()
            base_path = self.__get_build_path_from_build_elem(build)
            var_list[var] += build_file_path_name_list_from_flavor(base_path, file, build_flavor, abs)

      return var_list
   def get_files_detailed (self,
                               attr,
                               file_types=default_file_types):
        '''
        Return a list of files listed in contents.xml as DetailedInfo class, 
        which matches the attr value specified. 
        The DetailedInfo contains the following information for the file element:
         - Base path of the file element 
         - All the attributes of the file element
         - All the names and attributes of the children of the file element
        
        The first usage of this is for sign_id.
        
        Parameters:
        
           file_types (optional, defaults to default_file_types):
        
              A list of types of file to return.  The default is all file types.
        
              Supported file types are:
        
                 default_file_types=['file_ref','download_file', 'dload_boot_image','partition_file','partition_patch_file'] 
        
           attr (mandatory):
        
              Filter based on the specified attribute.  An attribute is required because
              the return value contains list of file path elements that matched the
              specified attribute.
         
   Usage example:
         <file_ref chipset="8084" sign_id="appsbl_mbn" minimized="true">
      <file_name>emmc_appsboot.mbn</file_name>
      <file_path sign_source="true">LINUX/android/out/target/product/apq8084/unsigned/</file_path>
      <file_path sign_dest="true">LINUX/android/out/target/product/apq8084/</file_path>
         </file_ref> 
   
      To retrieve the attribute values in examples above, the following
      code can be used:          
   
      detailed_info_list = meta_info.get_files_detailed('sign_id')
      for detailed_info in detailed_info_list:
         base_path = detailed_info.image_dir_base 
         chipset = detailed_info.chipset
         file_name = detailed_info.file_name[0].value
         for path in detailed_info.file_path:
            file_path = path.value
       isSource = path.sign_source
       isDestination = path.sign_dest
         
        '''
        class DetailedInfo(object):
            def __init__(self):
                pass
                
        if type(file_types) is not list:
            file_types = [file_types]
        builds_flat = self.XmlRoot.find('builds_flat')
        obj_list = []
        
        for build in builds_flat.getchildren():
            build_file_list = get_all_files_for_build(build, file_types)
            
            if attr != None:
                # Filter on attribute
                build_file_list = filter_files_by_attr(build_file_list, attr)
            
            for _file in build_file_list:
                obj = DetailedInfo()
                
                # Save the base path of the image 
                base_path = self.__get_build_path_from_build_elem(build)
                setattr(obj, 'image_dir_base', base_path)
                
                # Save all the attributes in the object
                for e_attr, e_value in _file.attrib.items(): 
                    setattr(obj, e_attr, e_value)
                
                # Save all the attributes of the children
                for child in _file.getchildren():
                    child_data = getattr(obj, child.tag, None)
                    if child_data is None:
                        child_data = []
                    this_child = DetailedInfo()
                    setattr(this_child, 'value', clean_vars(child.text.strip()))
                    for e_attr, e_value in child.attrib.items(): 
                        setattr(this_child, e_attr, e_value)
                    child_data.append(this_child)
                    setattr(obj, child.tag, child_data)
                
                # Add this object to the list
                obj_list.append(obj)
        return obj_list


   def get_files (self,
                  file_types=default_file_types,
                  build=None,
                  attr=None,
                  flavor=None,
                  abs=True,
                  expand_wildcards = True ):
      '''
      Return a list of file path/name strings for the given file_type, optionally filtered
      by build name, attribute or flavor.  The path defaults to an absolute path, but can be
      specified to be relative by setting the abs parameter to False.
      
      Parameters:
      
         file_types (optional, defaults to 'file_ref'):
      
            A list of types of file to return.  The default is all file types.
      
            Supported file types are:
      
               ['download_file', 'file_ref','dload_boot_image']
      
         build (optional, defaults None):

            Filter based on the specified build.  If not specified, files from all builds are
            returned.
      
         attr (optional, defaults to None):
      
            Filter based on the specified attribute.  If not specified, all files are returned
            regardless of attributes.
      
         flavor (optional, defaults to None):

            Filter based on the specified flavor.  If not specified, all files are returned
            regardless of flavor.
      
         abs (optional, defaults to True):
      
            Specifies whether to return absolute or relative paths.  Relative paths are
            relative to the root directory for the build.  Defaults to absolute paths.
            Set to False to return relative paths.
'''

      if type(file_types) is not list:
         file_types = [file_types]

      builds_flat = self.XmlRoot.find('builds_flat')
      if build:
         build_list = [find_named_element(builds_flat, build)]
      else:
         build_list = builds_flat.getchildren()

      file_list = list()
      for build in build_list:
         build_flavor = None
         build_name = build.find('name').text.strip()
         build_base_path = self.__get_build_path_from_build_elem(build)
         build_file_list = get_all_files_for_build(build, file_types)
         
         # Filter on attribute
         if attr != None:
            build_file_list = filter_files_by_attr(build_file_list, attr)
         
         # Filter on flavor
         flavors = self.get_product_flavors()
         if flavor != None:
            # Check if its a valid flavor
            if flavor in flavors:
               # Take this flavor as the only flavor required
               flavors = [flavor]
            else:
               # Return empty list
               self.log("Specified flavor is either invalid or not part of this product")
               return []
         else:
            # Take all available flavors and also add 'None' as a flavor 
            # to get files with atleast one unflavored child when filtered
            flavors.append(None)
            
         # Find files for each flavor separately and then combine
         for flavor in flavors:               
            # Filter on flavor
            if (flavor in self.flavor_data) and (build_name in self.flavor_data[flavor]) :
               build_flavor = self.flavor_data[flavor][build_name]
            build_file_list = filter_files_by_flavor(build_file_list, build_flavor)
            # Get file list for each flavor
            for each_file in build_file_list:
               file_list += build_file_path_name_list_from_flavor(build_base_path, each_file, build_flavor, abs , expand_wildcards)
      
      # Remove duplicates from final file list and return
      return sorted(list(set(file_list)))

   def set_minimized_build_flag( self , flag_value ):
      '''
      This function sets value of the minimized build flag to specified value.
      The <minimized_build_flag> is a child element of <product_info>
      '''
      
      # check if the contents.xml has the element <minimized_build_flag> under the <product_info>
      product_info_el = self.XmlRoot.find('product_info' )
      minimized_build_flag_el = product_info_el.find('minimized_build_flag' )
      if minimized_build_flag_el == None:
         minimized_build_flag_el = et.SubElement( product_info_el, 'minimized_build_flag' )

      #if the <minimized_build_flag> element does not have cmm_var attribute, then add it and set its values as "MINIMIZED_BUILD"
      cmm_var_value = minimized_build_flag_el.get('cmm_var')
      if not cmm_var_value:
         minimized_build_flag_el.set('cmm_var','MINIMIZED_BUILD')
         self.log('<minimized_build_flag> element did not have cmm_var so added it')
      else:
         self.log('<minimized_build_flag> element already has cmm_var attribute')

      #set the value of <minimized_build_flag> to the specified value
      if( flag_value ):
         minimized_build_flag_el.text = 'TRUE'
         self.log('<minimized_build_flag> element set to TRUE')
      else:
         minimized_build_flag_el.text = 'FALSE'
         self.log('<minimized_build_flag> element set to FALSE')
         
   #end of function set_minimized_build_flag()

   def get_product_name(self):
      product_info = self.XmlRoot.find('product_info')
      return product_info.find('product_name').text

   def update_product_name(self, product_name):
      product_info = self.XmlRoot.find('product_info')
      product_info.find('product_name').text = product_name

   def get_product_info(self, field=None, flavor=None):
      product_info = self.XmlRoot.find('product_info')
      ret = None
      if product_info is not None:
         if flavor:
            pf_elem_list = product_info.getchildren()
            pf_elem_list = filter_files_by_flavor(pf_elem_list,flavor)
            if field : 
               for each_pf_elem_list in pf_elem_list:
                  if field == each_pf_elem_list.tag : 
                     ret = each_pf_elem_list.text.strip()
            else :
               ret = pf_elem_list
         else:
            if field:
               field_el = product_info.find(field)
               if field_el is not None and field_el.text:
                  ret = field_el.text.strip()
            else:
               ret = XmlToDict(product_info)
      return ret
   # End of get_product_info

   def get_image_mainmake(self, buildLocation, buildID):
      # Remove excess characters from the string
      buildLocation = buildLocation.replace('\r', '')
      
      notes_lines = self.__read_image_notes(buildLocation, buildID)
      for line in notes_lines:
         # Look through the lines for Host
         currLine = line.split(":",1)
         if currLine[0] == "Make File Name":
            return currLine[1].strip()
      
      # "Make File Name" not found in the notes.txt
      return ""
                           
   def __read_image_notes(self, buildLocation, buildID):
      lines = []   # Holds the lines read in from the file      
      try:
         # If buildID is None, buildID_notes.txt doesn't exist anyway
         if buildID == None:
            raise IOError
         # If buildID is provided, open the file at buildLocation\buildID_notes.txt
         f = open(buildLocation + buildID + "_notes.txt", 'r')
         lines = f.readlines()
         f.close()
         return lines
      except IOError:
         try:
            # File probably didn't exist. Trying with the containing folder name.
            derp = buildLocation.split("\\")
            f = open(buildLocation + derp[len(derp)-1] + "_notes.txt", 'r')
            lines = f.readlines()
            f.close()
            return lines
         except IOError:
            try:
               # File didn't exist with foldername either. Globbing for any "_notes.txt".
               file_list = glob(buildLocation + "*_notes.txt")
               if len(file_list) == 1:   
                  # Found one notes.txt file, can give it a try
                  f = open(file_list[0], 'r')
                  lines = f.readlines()
                  f.close()
                  return lines
               else:
                  # Found none or more than one notes file. Cannot proceed.
                  raise IOError
            except IOError:
               # Either file IO failed or the files didn't exist
               # If naming conventions change, more try:excepts can be placed here.
               return ""
   

   #----------------------------------------------------------#
   # Private Methods                                          #
   #----------------------------------------------------------#


   def __get_builds_flat (self):
      '''Return the 'builds_flat' element'''
      return self.XmlRoot.find('builds_flat')

   def __get_builds_flat_as_dict (self):
      '''Return a dictionary of builds with all their info.
Attributes, if any, are not returned.'''
      builds_flat = self.XmlRoot.find('builds_flat')
      return XmlToDict(builds_flat)

   class __multi_path:
      def __init__(self):
         self.lin_path = None
         self.win_path = None

   def __get_new_path (self, build_id):
      mpath = self.__multi_path()
      try:
         if not FINDABLE:
            raise Exception("Cannot search path for " + build_id)
            
         fbInfo = self.fb.get_fbInfo(build_id)
         win_path = fbInfo['Location'] + "\\"
         lin_path = fbInfo['LinuxPath'] + "/"
         if not win_path:
            raise Exception("No Windows path found for " + build_id)
         if not lin_path:
            raise Exception("No Linux path found for " + build_id)
         
         mpath.win_path = win_path
         mpath.lin_path = lin_path
              
      except:
         sys.exit('Build path error. Exiting !')
      return mpath


   def __is_oem_merge_alreadydone( self ):
      oem_merge_done = self.XmlRoot.find('oem_merge_done')
      if oem_merge_done == None:
         return False
      else:
         if oem_merge_done.text.strip() in ['true', 'True', 'TRUE', 1]:
            return True
         else:
            return False
      #end of function __is_oem_merge_alreadydone()
      
   def __build_flavor_data (self):
      '''Build dictionary of flavor information for easier access later.'''
      flavor_data = dict()

      product_flavors = self.XmlRoot.find('product_flavors')
      if product_flavors is not None:
         for pf_elem in product_flavors.getchildren():
            prod_flav = pf_elem.find('name').text.strip()
            flavor_data[prod_flav] = dict()
            for child in pf_elem.getchildren():
               if child.tag == 'component':
                  comp_name = child.find('name').text.strip()
                  comp_flav = child.find('flavor').text.strip()
                  flavor_data[prod_flav][comp_name] = comp_flav

      return flavor_data

   def __merge_oem(self):
      ''' This function merges OEM's product flavor information from contents_oem.xml into the main XML data structure.
      OEM changes are not made persistent in this function.'''

      # merging of top level OEM product flavors and removing the specified Qualcomm flavor
      xml_elem_list_oem_product_flavors = self.__get_oem_product_flavors()
      for flavor in xml_elem_list_oem_product_flavors:
         # if the OEM product flavor element does not have 'oem_remove' attribute or if it exists and has a value 'False'
         # then that flavor is a candidate to be added.
         # else ( OEM product flavor element has attribute 'oem_remove'==True ) then that product flavor is a candidate to be removed.

         if ( flavor.get( 'oem_remove' ) == None ) or ( flavor.get( 'oem_remove' ) == 'False' ):
            self.__add_product_flavor( flavor )
         else:
            self.__remove_product_flavor( flavor )
      
      # ensure that <product_flavors> is at the begining of the XML tree
      product_flavor_element = self.XmlRoot.find('product_flavors')
      if product_flavor_element:
         self.XmlRoot.remove( product_flavor_element )
         self.XmlRoot.insert( 0 , product_flavor_element )


      # merging of OEM build flavor into main XML tree
      oem_build_elem_list = self.__get_oem_build_list()
      
      # process each <build> element in contents_oem.xml
      # if the resultant file type element in the main XML tree has a <file_path> without any 'flavor' attribute it will be removed.
      
      for build in oem_build_elem_list:
         list_file_type = self.default_file_types
         for file_type in list_file_type:
            # check if the OEM <build> element has 'flavor' attribute in <file_path> sub-element
            list_oem_build_file_ref_elements = build.findall( file_type )
            for oem_build_filref_elem in list_oem_build_file_ref_elements:
               oem_build_filename_elem = oem_build_filref_elem.find('file_name')
               oem_build_filepath_elem_list = oem_build_filref_elem.findall('file_path')
               
               for oem_build_filepath_elem in oem_build_filepath_elem_list:               
                  # if the OEM <build> element has file-path flavor information
                  if oem_build_filepath_elem.get('flavor') != None:
                     # get <build> element from main XML tree which matches the name of the current OEM <build> element
                     matching_build_elem = self.__get_build_elem_by_name( build.find('name').text.strip() )
                     
                     # get the list of all file_ref elements in that matching <build> element
                     file_ref_list = matching_build_elem.findall( file_type )
                     
                     # check each <file_ref> element in the list if its <file_name> matches against OEM <build> element's <file_name>
                     for file_ref_elem in file_ref_list:
                        # if the <file_name> of current OEM <build> element matches against <file_name> of the main <build> element
                        if( file_ref_elem.find('file_name').text.strip() == oem_build_filename_elem.text.strip() ):
                           # before adding the oem <file_path> element into main XML tree, check if the current <file_ref> has a <file_path> element pointing to the same flavoered path
                           if not self.__flavored_path_exists( file_ref_elem , oem_build_filepath_elem ):
                              file_ref_elem.append( oem_build_filepath_elem )
                              
                           # check if the current main <file_ref> element has a <file_path> sub-element without any flavor, then remove it
                           list_file_path_elements = file_ref_elem.findall('file_path')
                           for file_path_el in list_file_path_elements:
                              # if the file-path element does not have any flavor, remove it
                              if not file_path_el.get('flavor'):
                                 file_ref_elem.remove( file_path_el )

      # check if the contents.xml has the element <oem_merge_done> under the root element
      oem_merge_done_el = self.XmlRoot.find('oem_merge_done')
      if oem_merge_done_el == None:
         oem_merge_done_el = et.SubElement( self.XmlRoot, 'oem_merge_done' )
         oem_merge_done_el.text = 'True'
      else:
         oem_merge_done_el.text = 'True'
         
   # end of function __merge_oem()

   def __flavored_path_exists( self, file_ref_elem , oem_build_filepath_elem ):
      ''' This function returns True if the given file_ref_elem has a <file_path> element same as oem_build_filepath_elem'''
      list_file_path_elem = file_ref_elem.findall('file_path')
      for file_path_el in list_file_path_elem:
         if ( file_path_el.get('flavor') == oem_build_filepath_elem.get('flavor') ) and ( file_path_el.text.strip() == oem_build_filepath_elem.text.strip() ):
            return True

      return False

   def __get_oem_build_list( self ):
      '''This function gets a list of all <build> XML elements from contents_oem.xml'''
      builds_flat = self.OemXmlRoot.find('builds_flat')
      return builds_flat.getchildren()
   # end of __get_oem_build_list()

   def __get_build_elem_by_name( self , name ):
      '''This function returns a <build> XML element which has the specified name'''
      builds_flat = self.XmlRoot.find('builds_flat')
      for child in builds_flat.getchildren():
         if ( child.find('name').text.strip() == name ):
            return child
   # end of __get_build_elem_by_name()  

   def __product_flavor_exists( self, product_flavor_element ):
      ''' This function checks if there is an already existing product flavor of the same name as the given product flavor element'''
      product_flavors = self.XmlRoot.find('product_flavors')
      if product_flavors:
         for child in product_flavors.getchildren():
            if self.__compare_product_flavor_elements( child, product_flavor_element ):
               return True

      return False

   def __add_product_flavor( self , product_flavor_element ):
      '''Adds a product flavor XML element to product_flavors in contents.xml'''
      product_flavors = self.XmlRoot.find('product_flavors')
      product_flavors.attrib['cmm_pf_var'] = "PRODUCT_FLAVORS"
      # before adding product flavor, check if there is all ready a product flavor of the same name, if not, only then add the given product flavor
      if not self.__product_flavor_exists( product_flavor_element ):
         product_flavors.append( product_flavor_element )

   def __remove_product_flavor( self , product_flavor_element ):
      '''Removes a product flavor XML element to product_flavors in contents.xml'''
      product_flavors = self.XmlRoot.find('product_flavors')
      if product_flavors:
         for child in product_flavors.getchildren():
            if self.__compare_product_flavor_elements( child, product_flavor_element ):
               product_flavors.remove( child )

   def __compare_product_flavor_elements( self, product_flavor_element_a, product_flavor_element_b ):
      '''Given two product flavor elements, if their 'name' element values are same, this function returns True , else False'''
      if self.__get_product_flavor_name( product_flavor_element_a ) == self.__get_product_flavor_name( product_flavor_element_b ):
         return True
      else:
         return False

   def __get_oem_product_flavors( self ):
      '''Return a list of oem product flavors XML elements'''
      flavor_names = list()
      product_flavors = self.OemXmlRoot.find('product_flavors')
      return product_flavors.getchildren()
   
   def __get_product_flavor_name( self, product_flavor ):
      '''This function returns the name of a given product flavor'''
      return product_flavor.find('name').text.strip()

   def __do_integrity_check (self):
      '''
      Function __do_integrity_check:

      This function run various tests on the database to make sure the
      information is consistent.

      Make sure the __build_flavor_data() function is called before calling
      __do_integrity_check.
      '''

      # Verify that all builds listed in the product flavors also exist
      # in the <builds_flat> element.
      builds_flat =  self.XmlRoot.find('builds_flat')
      build_names = [x.find('name').text.strip() for x in builds_flat.getchildren()] 
      for prod_flav in self.flavor_data:
         for component in self.flavor_data[prod_flav]:
            if component not in build_names:
               raise DataIntegrityException(value = "Product flavor '" + prod_flav + "' referenced invalid build '" + component + "'",
                                            logger = self.logger)

   def log(self, logstmt):
      if self.logger:
         self.logger.log(logstmt)
      else:
         print logstmt

   def remove_element(self, build):
      builds_flat = self.XmlRoot.find('builds_flat')
      if build :
         build_elem = find_named_element(builds_flat, build)
         if(build_elem) :
            builds_flat.remove( build_elem )
   # End of remove_element() 
   
   def find_root_element (self, tag):
      '''Return the '<tag>' element under xml root'''
      return self.XmlRoot.find(tag)
      
#==============================================================================
# 
# End of Class meta_info
#
#==============================================================================


def find_named_element (parent_elem, tag):
   for child in parent_elem.getchildren():
      name = child.find('name')
      if ((name is not None) and (name.text == tag)):
         return child
   return None

def find_element_or_create (parent_elem, tag):
   '''This method will search and create an element if one doesn't already exist.'''
   new_elem = parent_elem.find(tag)
   if new_elem is None:
      new_elem = et.SubElement(parent_elem, tag)
   return new_elem
   
def has_element (parent_elem, tag):
   '''Return True if the parent contains a name 'tag', and that element
has text or sub-elements.'''
   child = parent_elem.find(tag)
   if child is not None:
      if child.text:
         return True
      if child.getchildren():
         return True
   return False

def merge_builds (dest_builds, src_builds):
   for src_build in src_builds.getchildren():
      dest_build = find_named_element(dest_builds, src_build.find('name').text.strip())
      if dest_build is not None:
         # Delete the old element
         dest_builds.remove(dest_build)
         # Now append the new element to dest_builds
         dest_builds.append(src_build)


def clean_vars (text):
   '''Clean variables from the specified text.'''
   if text.count('${'):
      pattern = "\A(.*)\${[^:]*(?::(.*))?}(.*)\Z"
      re_obj = re.compile(pattern)
      match_obj = re_obj.search(text)
      while match_obj:
         text = ''.join(match_obj.groups())
         match_obj = re_obj.search(text)
   return text

def XmlPrettyPrint (element, indent=0):
   text = indent*'  ' + '<' + element.tag
   # Print attributes on the same line as the tag
   for attr in element.attrib:
      text += ' ' + attr + '=' + element.attrib[attr]
   # Close the opening tag
   text += '>'
   if element.text and element.text.strip():
      text += element.text
   else:
      child_found = False
      for child in element.getchildren():
         if not child_found:
            text += '\n'
            child_found = True
         text += XmlPrettyPrint (child, indent+1)
      if child_found:
         text += indent*'  '
   text += '</' + element.tag + '>\n'
   return text

def XmlToDict (element):
   '''Return the content of an xml sub-tree in dictionary format.
This method ignores attributes and only returns tags and associated text.'''
   if element.text and element.text.strip():
      # Leaf node
      ret = element.text
   else:
      ret = dict()
      for child in element.getchildren():
         ret[child.tag] = XmlToDict(child)
   return ret

def get_all_files_for_build (build, file_types):
   '''Return a list of all file elements for a given build. 
      File elements that don't have name/path specified will NOT be included.
   '''
   file_list = []
   for ft in file_types:
      for elem in build.findall(ft):
         empty_child = False
         for child in elem.getchildren():
            # Check if all the child elements have value
            if child.text == None or child.text.strip() == '':
               empty_child = True
               break
         if not empty_child:
            # Take only the files that have name/path values
            file_list.append( elem )
   return file_list

def filter_files_by_attr (file_list_in, attr,desc="true"):
   '''From a list of file elements (or any element really), filter out
those file elements that either don't contain the given attribute,
or for which the given attributes value is 'false', 0 or None.  Return
the resulting list.'''
   file_list_out = []
   for file in file_list_in:
      if desc != "true":   
         if (attr in file.attrib and
             (file.attrib[attr] in ['TRUE', 'True', 'true', 0, None] or file.attrib[attr] == desc)):
            file_list_out.append(file)
      else:
         if (attr in file.attrib and
             file.attrib[attr] not in ['false', 'False', 'FALSE', 0, None]):
            file_list_out.append(file)     
   return file_list_out

def get_flavor_list (element):
   '''Parse a comma-separated list of flavors from the 'flavors' attribute
   of the given element, and return a Python list of these flavors.'''
   if 'flavor' in element.attrib:
      flavors = element.attrib['flavor'].split(',')
      for i in (range(len(flavors))):
         flavors[i] = flavors[i].strip()
   else:
      flavors = []
   return flavors

def filter_files_by_flavor (file_list_in, flavor):
   '''From a list of file elements, filter out those file elements that
   contain flavor information, but the given flavor.  Flavor information
   can eithre be in the file_type element, or in the file name or file
   path elements.  File elements that don't contain any flavor information
   are also returned because these files are considered to be valid for
   any flavor.  Return the resulting list.'''
   file_list_out = []
   for file in file_list_in:
      flavors_found = False
      if 'flavor' in file.attrib:
         flavors_found = True
         if flavor in get_flavor_list(file):
            file_list_out.append(file)
      else:
         # Check name and path elements for flavors attribute
         for child in file.getchildren():
            if 'flavor' in child.attrib:
               flavors_found = True
               if flavor in get_flavor_list(child):
                  file_list_out.append(file)
                  break

      if not flavors_found:
         # If no flavors attributes found, then file is for all flavors.
         file_list_out.append(file)

   return file_list_out

def build_file_path_name_list_from_flavor (root_path, file_elem, flavor, abs , expand_wildcards = True ):
   '''
   Function: build_file_path_name_list_from_flavor()

   Description:

      Build a list of relative path/file name for given file element, using the
      first path and file name that match the given flavor.  If the path contains
      wildcards, expand the wild-cards using glob and the given root path.
      Wild-card expansion includes recursively entering directories.

      If abs is True, the root path will be pre-pended to each of the file paths,
      creating absolute paths for each file.  If abs is False, the resulting list
      will contain relative paths, relative to the root path.

      The clean_vars function is run on each path to resolve any variables before
      running glob.

      If a path or name element doesn't contain flavor information, it is considered
      a match for any flavor.

      This should work, even if flavor is None, since flavor should only
      be None if there is no flavor information in contents.xml, in which
      case, path and file name should not contain 'flavors' attributes.

   Parameters:

      root_path

         Root path from which to resolve wild-cards.  Prepended to resulting paths
         if abs is True.  This path should not contain wild-cards.

      file_elem

         A file element from which file/path information is extracted.

      flavor

         Flavor of path to extract from file_elem.

      abs

         If true, return absolute paths.  Else, return relative paths.
   '''
   name = None
   path = None
   temp_name = None
   temp_path = None
   
   if flavor!= None:
      for child in file_elem.getchildren():
         if (flavor in get_flavor_list(child)):
            if not name and child.tag == 'file_name':
               name = child.text.strip()
            elif not path and child.tag == 'file_path':
               path = child.text.strip()
         elif ('flavor' not in child.attrib):
            if not temp_name and child.tag == 'file_name':
               temp_name = child.text.strip()
            elif not temp_path and child.tag == 'file_path':
               temp_path = child.text.strip()
   else:
      for child in file_elem.getchildren():
         if ('flavor' not in child.attrib):
            if not name and child.tag == 'file_name':
               name = child.text.strip()
            elif not path and child.tag == 'file_path':
               path = child.text.strip()
         else:
            if not temp_name and child.tag == 'file_name':
               temp_name = child.text.strip()
            elif not temp_path and child.tag == 'file_path':
               temp_path = child.text.strip()
   
   if not name:
      name = temp_name
   if not path:
      path = temp_path
            
   if not name or not path:
      raise DataIntegrityException()

   path_name = os.path.join (path, name)

   # Clean any variables from the path_name
   path_name = clean_vars(path_name)

   # Expand wildcards
   if( expand_wildcards ):
      if file_elem.tag == 'dir_ref':
         path_names = my_glob(root_path, path_name, abs, recursion=False)
      else : 
         path_names = my_glob(root_path, path_name, abs)
      return path_names
   else:
      abs_path = os.path.join( root_path, path_name )
      return [ abs_path ]

# End build_file_path_name_list_from_flavor()

def my_glob (root_path, file_path, abs, recursion = True):
   '''
   Recursivly glob through directory tree returning a list of all
   matching files.

   file_path is a relative path.  Glob is done using root_path as the root
   for the relative path.  If abs is True, return absolute paths, else return
   relative paths.
   '''
   abs_path = os.path.join(root_path, file_path)
   file_list = glob(abs_path)

   # Expand directories
   new_file_list = []
   for file in file_list:
      if os.path.isdir(file) and recursion :
         if not os.path.islink(file):
            new_file_list += my_glob(file, '*', True)
      else:
         new_file_list.append(file)
   file_list = new_file_list

   if not abs:
      # Revert back to relative paths
      rel_file_list = []
      for file in file_list:
         rel_file_list.append(os.path.relpath(file, root_path))
      file_list = rel_file_list

   return file_list

def join_paths (path_list, targ_os_sep=None):
   '''Like os.path.join, but let's you specify the target OS, and makes
   sure resulting path separators are all consistent with that OS.'''

   # First, work exclusively with the local OS separator
   if 'linux' in sys.platform:
      alt_sep = '\\'
   else:
      alt_sep = '/'
   path_list = [x.replace(alt_sep, os.sep) for x in path_list]
   
   # Join and normalize path to collapse redundant separators
   new_path = os.path.normpath(os.path.join(*path_list))
   
   # Replace with target separator if provided
   if targ_os_sep:
      new_path = new_path.replace(os.sep, targ_os_sep)

   return new_path



#==============================================================================
# 
# Exceptions
#
#==============================================================================

class DataIntegrityException( Exception ):
   '''An exception class for invalid meta data.'''
   def __init__(self, value = None, logger = None):
      if value:
         self.value = "Error: " + value
      else:
         self.value = None
      if logger:
         logger.log(self.value)
      else:
         print self.value
   def __str__(self):
      return repr(self.value)

class InvalidFilePathException( Exception ):
   '''An exception class for invalid file path.'''
   def __init__(self, list_invalid_files ):
      Exception.__init__(self)
      self.invalid_files_list = list_invalid_files

class InputError(Exception):
   def __init__(self, value):
      self.value = "meta_lib.py: Input Error: " + value
   def __str__(self):
      return repr(self.value)
# End of class InputError

