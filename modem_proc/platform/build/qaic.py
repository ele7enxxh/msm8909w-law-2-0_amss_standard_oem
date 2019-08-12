#===============================================================================
# Copyrigha (c) 2012 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#===============================================================================

import sys
import os
import subprocess
import string
import SCons.Action
from SCons.Script import *

def qaic_header_gen(target, source, env, for_signature ):
    qaic_exe = env.FindQaicExe()
    flags = env.subst('$_CPPINCFLAGS')
    target_dir  = os.path.dirname(str(target[0]))
    source_file = str(source[0])
    env.Depends(target, qaic_exe)
    return ( "%s %s -ho -o %s %s" % (qaic_exe, flags, target_dir, source_file))

def qaic_ro_gen(target, source, env, for_signature ):
    qaic_exe = env.FindQaicExe()
    flags = env.subst('$_CPPINCFLAGS')
    target_dir  = os.path.dirname(str(target[0]))
    source_file = str(source[0])
    env.Depends(target, qaic_exe)
    return ("%s %s -ro -o %s %s" % (qaic_exe, flags, target_dir, source_file))

#find a file in cpppath array, errors out if there are multiple results
#TODO: cache the results?
def find_in_cpppath(env, fname):
    paths = env['CPPPATH']
    path = None
    tried = []
    for dname in paths:
        dname = env.subst(dname)
        tpath = os.path.join(dname , fname)
        tried.append(tpath)
        if(os.path.isfile(tpath)):
            if(path != None):
                env.PrintError("find_in_cpppath: found duplicate path for %s, in %s and %s, adjust your required apis" % (fname, path, tpath))
                Exit(1)
            else:
                path = tpath
    return (path,tried)

#Finds the build folder that contains the scons file
# responsible for building the input idl
def find_build_folder(fname):
    path  = os.path.dirname(fname)
    while True:
        newdir = os.path.normpath(os.path.join(path ,'build'))
        if os.path.isdir(newdir):
            return newdir
        else:
            path = os.path.join(path,os.pardir)

def exists(env):
   return env.Detect('qaic')

def add_qaic_header_dep(env, deps, idlname):
    if env['IMAGE_NAME'] == 'DEVCFG_IMG':
        return

    idls = idlname
    if type(idls) != list:
        idls = [idls]

    hfiles = []
    for idl in idls:
        idlfile,tried = env.FindInCPPPath(idl + ".idl")

        if None == idlfile:
            env.PrintError("qaic builder: couldn't find idl file %s.idl. tried: %s" % (idl, str(tried)))
            Exit(1)

        build_folder = os.path.join(find_build_folder(idlfile), env['SHORT_BUILDPATH'])
        hfiles.append(os.path.join(build_folder , idl + '.h'))

        # Add path
        env.AppendUnique(CPPPATH = [os.path.normpath(build_folder)])

    deplist = deps
    if type(deps) != list:
        deplist = [deps]
    for depname in deplist:
        for hfile in hfiles:
            env.Depends(depname, hfile)
    return deps

#adds a header builder dependency to sources
#returns the dependency(ies)
def qaic_header_from_idl(env, idlname):
    if env['IMAGE_NAME'] == 'DEVCFG_IMG':
        return
    idlfile,tried = env.FindInCPPPath(idlname + ".idl")
    if None == idlfile:
        env.PrintError("qaic builder: couldn't find idl file %s.idl. tried: %s" % (idlname, str(tried)))
        Exit(1)

    hfile = os.path.join(find_build_folder(idlfile), env['SHORT_BUILDPATH'], idlname + ".h")
    return env.QaicHeaderBuilder(target = hfile, source = idlfile)

def qaic_stub_from_idl(env, idlname):
    if env['IMAGE_NAME'] == 'DEVCFG_IMG':
        return
    idlfile,tried = env.FindInCPPPath(idlname + ".idl")
    if None == idlfile:
        env.PrintError("qaic builder: couldn't find idl file %s.idl. tried: %s" % (idlname, str(tried)))
        Exit(1)
    stub = os.path.join(find_build_folder(idlfile), env['QC_BUILDPATH'], idlname + "_stub.c")
    env.QaicHeaderFromIdl(idlname)
    env.QaicStubBuilder(target = stub, source = idlfile)
    return env.AddQaicHeaderDep(stub, idlname)

def qaic_skel_from_idl(env, idlname):
    if env['IMAGE_NAME'] == 'DEVCFG_IMG':
        return
    idlfile,tried = env.FindInCPPPath(idlname + ".idl")
    if None == idlfile:
        env.PrintError("qaic builder: couldn't find idl file %s.idl. tried: %s" % (idlname, str(tried)))
        Exit(1)
    skel = os.path.join(find_build_folder(idlfile), env['QC_BUILDPATH'], idlname + "_skel.c")
    env.QaicSkelBuilder(target = skel, source = idlfile)
    env.QaicHeaderFromIdl(idlname)
    return env.AddQaicHeaderDep(skel, idlname)

#is there a way to cache this for all the envs, or is it ok to do this per env?
def find_qaic_exe(env):
    if not env.subst('$QAIC_EXE'):
        env.RequirePublicApi('QAIC')
        qaic_exes = ["Darwin/qaic", "Ubuntu10/qaic", "Ubuntu12/qaic", "Linux_DoNotShip/qaic", "WinNT/qaic.exe"]
        qaic_exe = None
        tried = []
        for ename in qaic_exes:
            tn,tns = env.FindInCPPPath(ename)
            if tn != None:
                try:
                   #using execmds generates a bunch of error output, it wouild be nice if that could be supressed
                   #data, err, rv = env.ExecCmds("%s -v" % tn)
                   tried.append(tn)
                   rv = subprocess.Popen([("%s" % tn), "-v"], stderr = subprocess.PIPE, stdout = subprocess.PIPE)
                   rv.wait()
                   if 0 == rv.returncode:
                      qaic_exe = tn
                      break;
                except:
                   pass
        if None == qaic_exe:
            env.PrintError("qaic builder: couldn't find qaic exe for your host.  tried: %s" % str(tried))
            Exit(1)
        env.Append(QAIC_EXE = qaic_exe)
    return env.subst('$QAIC_EXE')

def generate(env):
    #public methods
    env.AddMethod(add_qaic_header_dep, "AddQaicHeaderDep")

    env.AddMethod(qaic_header_from_idl, "QaicHeaderFromIdl")
    env.AddMethod(qaic_stub_from_idl, "QaicStubFromIdl")
    env.AddMethod(qaic_skel_from_idl, "QaicSkelFromIdl")

    #private
    #do these need a _ prefix?
    env.AddMethod(find_in_cpppath, "FindInCPPPath")
    env.AddMethod(find_qaic_exe, "FindQaicExe")

    qaic_ho = Builder(name = "QaicHeaderBuilder", generator = qaic_header_gen)
    qaic_stub = Builder(name = "QaicStubBuilder", generator = qaic_ro_gen)
    qaic_skel = Builder(name = "QaicSkelBuilder", generator = qaic_ro_gen)

    env['BUILDERS']['QaicHeaderBuilder'] = qaic_ho
    env['BUILDERS']['QaicStubBuilder'] = qaic_stub
    env['BUILDERS']['QaicSkelBuilder'] = qaic_skel

