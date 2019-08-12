# -*- coding: us-ascii -*-
# vim:ts=2:sw=2:softtabstop=0:tw=74:smarttab:expandtab
#
# Copyright The Android Open Source Project
#
# Original version 2010 by Keith Dart <keith@kdart.com>
# Modified by 2012 Pekka Nikander <pekka.nikander@senseg.com>

"""Python client for using android adb.

Provides primitive methods for interacting with or controlling an Android
device over adb (the Android debug interface). This module uses the adb
server's socket protocol directly, and can be used in place of the adb
commandline client.

For running the Robot Framework remote server with a given fixture at
an Android table, the following should work:

>>> adb = AdbClient()
>>> device = adb.GetDevice(1)
>>> device.Push("%s/%s.py" % (library, fixture), "/data/local/tmp")
>>> device.Push("%s/%s" % (library, "robotremoteserver.py"), "/data/local/tmp")
>>> device.ShellCommand("TMPDIR=/data/local/tmp PYTHONPATH=/data/local/tmp python -c import robotremoteserver; import %s; robotremoteserver.RobotRemoteServer(%s.%s())" % (fixture, fixture, fixture))

"""

import os
import zipfile
import struct
import stat
import subprocess
import socket
import time
from cStringIO import StringIO

#ADB = r"c:\android\sdk\platform-tools\adb.exe"
ADB = r"c:\windows\system32\adb.exe"
ADB_PORT = 5037

class Error(Exception):
  pass

class AdbError(Error):
  pass

class AdbQuit(Error):
  """Signals that the device connection must quit."""


# device states
OFFLINE, BOOTLOADER, DEVICE, HOST, RECOVERY, UNKNOWN = (
  "offline", "bootloader", "device", "host", "recovery", "unknown")


def StartServer():
  subprocess.call([ADB, "start-server"])

def KillServer():
  subprocess.call([ADB, "kill-server"])

class AdbClient(object):
  """Python client for the adb server using socket interface."""

  def __init__(self):
    self._adb_version = int(self.HostQuery("host:version"), 16)

  def _Connect(self):
    retries = 3
    while retries:
      try:
        sock = socket.create_connection(("localhost", ADB_PORT))
      except socket.error:
        StartServer()
        retries -= 1
      else:
        return sock
    else:
      raise AdbError("Could not connect to server.")

  def DoCommand(self, sock, cmd):
    cmd = "%04x%s" % (len(cmd), cmd)
    sock.sendall(cmd)
    try:
      self._CheckStatus(sock)
    except AdbError:
      sock.close()
      raise

  def Connect(self, cmd):
    sock = self._Connect()
    self.DoCommand(sock, cmd)
    return sock

  def Forward(self, port, remote=None):
    """Forwards the given port from the host to the device."""
    if not remote:
      remote = port
    sock = self.Connect("host:forward:tcp:%d;tcp:%d" % (port, remote))
    sock.close()

  def HostQuery(self, cmd):
    sock = self._Connect()
    self.DoCommand(sock, cmd)
    size = int(sock.recv(4), 16)
    resp = sock.recv(size)
    sock.close()
    return resp

  def _CheckStatus(self, sock):
    stat = sock.recv(4)
    if stat == "OKAY":
      return True
    elif stat == "FAIL":
      size = int(sock.recv(4), 16)
      val = sock.recv(size)
      raise AdbError(val)
    else:
      raise AdbError("Bad response: %r" % (stat,))

  def GetDevices(self):
    dm = DeviceManager()
    resp = self.HostQuery("host:devices")
    for n, line in enumerate(resp.splitlines()):
      parts = line.split("\t")
      if self._adb_version >= 19:
        device = AdbDeviceClient(n+1, parts[0], parts[1], self._adb_version)
      else:
        device = AdbDeviceClient(int(parts[0]), parts[1], parts[2], 
                                 self._adb_version)
      dm.Add(device)
    return dm

  def GetDevice(self, identifier=1):
    try:
      return self.GetDevices()[identifier]
    except (KeyError, IndexError):
      raise AdbError("Could not get device with identifier %r." % (identifier,))

  def Kill(self):
    sock = self.Connect("host:kill")
    sock.close()


class AdbDeviceClient(AdbClient):
  """ADB Client for a specific device.
  """

  def __init__(self, dev_id, serial_no, state, adb_version):
    self.device_id = dev_id
    self.serial = serial_no
    self.state = state
    self._adb_version = adb_version
    self._build = None

  def Connect(self, cmd):
    if self.state == DEVICE:
      if cmd.startswith("bootloader:"):
        raise AdbError("Sending bootloader command %r in DEVICE state." % (
            cmd,))
      if self._adb_version >= 19:
        transport = "host:transport:%s" % self.serial
      else:
        transport = "host:transport:%d" % self.device_id
    elif self.state == BOOTLOADER:
      transport = "host:transport-bl:%d" % self.device_id
    else:
      raise AdbError("bad device state: %r" % (self.state,))
    sock = self._Connect()
    self.DoCommand(sock, transport)
    self.DoCommand(sock, cmd)
    return sock

  def Command(self, cmd):
    sock = self.Connect(cmd)
    sock.close()

  def __str__(self):
    return "%s\t%s\t%s" % (
        self.device_id, self.serial, self.state)

  def __repr__(self):
    return "%s(%r, %r, %r, %r)" % (self.__class__.__name__,
        self.device_id, self.serial, self.state, self._adb_version)

  def GetBuild(self):
    """Return a dictionary of build information from phone.
    """
    if self._build is None:
      text = self.ShellCommandOutput("cat system/build.prop")
      rv = dict()
      for line in text.splitlines():
        if line.startswith("#"):
          continue
        try:
          l, r = line.split("=", 1)
        except ValueError:
          continue
        else:
          rv[l.strip()] = r.strip()
      self._build = rv
    return self._build

  build = property(GetBuild)

#  def GetShell(self, logfile=None):
#    s = self.Connect("shell:")
#    exp = ExpectAdb(s.makefile("w+", 1), prompt="# ", logfile=logfile)
#    exp.send("\n") # sync with prompt
#    exp.wait_for_prompt()
#    return exp

  def ShellCommand(self, cmd):
    sock = self.Connect("shell:%s" % (cmd,))
    # Closing the socket too early sends SIGHUP to the subprocess.  Give it some time
    time.sleep(2)
    data = sock.recv(4096) # eat any output
    sock.close()
    return data

  def ShellCommandOutput(self, cmd):
    io = StringIO()
    self.RunCommand(cmd, io)
    return io.getvalue()

  def RunCommand(self, cmd, stream):
    sock = self.Connect("shell:%s" % (cmd,))
    data = sock.recv(4096)
    while data:
      stream.write(data)
      data = sock.recv(4096)
    sock.close()

  def Remount(self):
    """Remounts the /system partition on the device read-write."""
    sock = self.Connect("remount:")
    sock.recv(4096)
    sock.close()

  def IsRunning(self):
    return self.state == DEVICE

  def IsOffline(self):
    return self.state == OFFLINE

  def IsBootloader(self):
    return self.state == BOOTLOADER

  def GetProduct(self):
    if self._adb_version >= 19:
      return self.HostQuery("host-serial:%s:get-product" % self.serial)
    else:
      return self.HostQuery("host:get-product")

  def GetState(self):
    if self._adb_version >= 19:
      st = self.HostQuery("host-serial:%s:get-state" % self.serial)
    else:
      st = self.HostQuery("host:get-state")
    self.state = st
    return st

  def WaitForBootloader(self):
    sock = self._Connect()
    self.DoCommand(sock, "host:wait-for-bootloader")
    try:
      sock.recv(4) # server blocks until bootloader available
      self.state = BOOTLOADER
    finally:
      sock.close()

  def WaitForDevice(self):
    sock = self._Connect()
    self.DoCommand(sock, "host:wait-for-device")
    try:
      sock.recv(4)
      self.state = DEVICE
    finally:
      sock.close()

  def Reboot(self, bootloader=False):
    if self.state == BOOTLOADER:
      sock = self.Connect("bootloader:reboot")
      sock.close()
    elif self.state == DEVICE:
      if bootloader:
        cmd = "reboot bootloader"
      else:
        cmd = "reboot"
      self.ShellCommand(cmd)

  def UploadData(self, name, data):
    cmd = "bootloader:flash:%s:%s" % (name, len(data))
    sock = self.Connect(cmd)
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_RCVBUF, 65536)
    try:
      sock.sendall(data)
      self._CheckStatus(sock)
    finally:
      sock.close()

  def GetLogcatIOHandler(self, stream, args=""):
    sock = self._ConnectLogcat(args)
    return LogcatIOHandler(sock, stream)

  def _ConnectLogcat(self, args):
    tags = os.environ.get("ANDROID_LOG_TAGS", "")
    return self.Connect('shell:export ANDROID_LOG_TAGS="%s" ; logcat %s' % (
        tags, args))

  # The following methods are analogs of the adb tool commands.
  def Logcat(self, stream, args=""):
    sock = self._ConnectLogcat(args)
    data = sock.recv(4096)
    try:
      while data:
        stream.write(data)
        data = sock.recv(4096)
    finally:
      sock.close()

  def DumpState(self, stream):
    self.RunCommand("dumpstate -", stream)

  def Buildprop(self, stream):
    self.RunCommand("cat /system/build.prop", stream)

  def DumpSys(self, stream, section=None):
    if section:
      self.RunCommand("dumpsys %s" % section, stream)
    else:
      self.RunCommand("dumpsys", stream)

  def BugReport(self, stream):
    if self._adb_version >= 19:
      self.RunCommand("dumpstate -", stream)
    else:
      self._Separator(stream, "dumpstate")
      self.RunCommand("dumpstate", stream)
      self._Separator(stream, "build.prop")
      self.RunCommand("cat /system/build.prop", stream)
      self._Separator(stream, "dumpsys")
      self.RunCommand("dumpsys", stream)


  @staticmethod
  def _Separator(stream, name):
    stream.write("""
  ========================================================
  == %s
  ========================================================
  """ % name)

  def List(self, path):
    """List directory on device.

    Returns a list of tuples:
      (mode, size, time, name, cookie)
    """
    sync = AdbSync(self)
    try:
      result = sync.List(path)
    finally:
      sync.Quit()
    return result

  def Push(self, loc, rem, verifyApk=0):
    """Push a file from local system to device."""

    st = os.stat(loc)
    if not stat.S_ISREG(st.st_mode):
      raise AdbError("Push: source is not a regular file.")

    sync = AdbSync(self)
    try:
      mode, fsize, ftime = sync.Readmode(rem)
      if mode != 0 and stat.S_ISDIR(mode):
        name = os.path.basename(loc)
        rem = "%s/%s" % (rem, name)

      sync.Send(loc, rem, st.st_mtime, st.st_mode, verifyApk)
    finally:
      sync.Quit()

  def Pull(self, loc, rem):

    # Given "loc" variable may be file or directory.
    try:
      lstat = os.stat(loc)
    except OSError:
      lpath, lname = os.path.split(loc)
      os.stat(lpath) # will raise OSError again if directory also does not
                     # exist.
    else: # some file or directory exists.
      # If directory, append remote name and use that as local name.
      # Otherwise use local name as-is.
      if stat.S_ISDIR(lstat.st_mode):
        rpath, rname = os.path.split(rem)
        loc = os.path.join(loc, rname)

    sync = AdbSync(self)
    try:
      mode, fsize, ftime = sync.Readmode(rem)
      if mode == 0:
        raise AdbError("Pull: remote file %r does not exist." % rem)
      if not (stat.S_ISREG(mode) or stat.S_ISCHR(mode) or stat.S_ISBLK(mode)):
        raise AdbError("Pull: remote file %r is not a file." % rem)

      sync.Recv(loc, rem)
    finally:
      sync.Quit()

def _MKID(a, b, c, d):
  return ((a) | ((b) << 8) | ((c) << 16) | ((d) << 24))
# x86
ID_STAT = _MKID(ord('S'), ord('T'), ord('A'), ord('T'))
ID_LIST = _MKID(ord('L'), ord('I'), ord('S'), ord('T'))
ID_ULNK = _MKID(ord('U'), ord('L'), ord('N'), ord('K'))
ID_SEND = _MKID(ord('S'), ord('E'), ord('N'), ord('D'))
ID_RECV = _MKID(ord('R'), ord('E'), ord('C'), ord('V'))
ID_DENT = _MKID(ord('D'), ord('E'), ord('N'), ord('T'))
ID_DONE = _MKID(ord('D'), ord('O'), ord('N'), ord('E'))
ID_DATA = _MKID(ord('D'), ord('A'), ord('T'), ord('A'))
ID_OKAY = _MKID(ord('O'), ord('K'), ord('A'), ord('Y'))
ID_FAIL = _MKID(ord('F'), ord('A'), ord('I'), ord('L'))
ID_QUIT = _MKID(ord('Q'), ord('U'), ord('I'), ord('T'))

del _MKID

_SYNCMSG = {
  "id": ("I", struct.calcsize("I")),
  "req": ("II", struct.calcsize("II")),        # id, namelen
  "stat": ("IIII", struct.calcsize("IIII")),   # id, mode, size, time
  "dent": ("IIIII", struct.calcsize("IIIII")), # id, mode, size, time, namelen
  "data": ("II", struct.calcsize("II")),       # id, size
  "status": ("II", struct.calcsize("II")),     # id, msglen
}

SYNC_DATA_MAX = 64 * 1024

class AdbSync(object):
  def __init__(self, client):
    self._sock = client.Connect("sync:")

  def Quit(self):
    self._sock.sendall(struct.pack("II", ID_QUIT, 0))
    self._sock.close()

  def ReadMessage(self, msgtype):
    fmt, sizeof = _SYNCMSG[msgtype]
    return struct.unpack(fmt, self._sock.recv(sizeof))

  def WriteMessage(self, command, path):
    self._sock.sendall(struct.pack("II", command, len(path)))
    if len(path) > 0:
      self._sock.sendall(path)

  def Readmode(self, path):
    self.WriteMessage(ID_STAT, path)
    rid, mode, size, time = self.ReadMessage("stat")
    if rid != ID_STAT:
      raise AdbError("ID_STAT not returned.")
    return mode, size, time

  def List(self, path):
    self.WriteMessage(ID_LIST, path)
    result = []
    while 1:
      rid, mode, size, time, namelen = self.ReadMessage("dent")
      if rid == ID_DONE:
        break
      if rid != ID_DENT:
        raise AdbError("ID_DENT not returned for List")
      name = self._sock.recv(namelen)
      result.append((mode, size, time, name))
    return result

  def Send(self, lpath, rpath, mtime, mode, verifyApk=0):
    self.WriteMessage(ID_SEND, "%s,%d" % (rpath, mode))
    fo = open(lpath)
    data = fo.read(SYNC_DATA_MAX)
    try:
      while data:
        self.WriteMessage(ID_DATA, data)
        data = fo.read(SYNC_DATA_MAX)
    finally:
      fo.close()
    self._sock.sendall(struct.pack("II", ID_DONE, mtime))
    rid, msglen = self.ReadMessage("status")
    self.CheckError(rid, msglen)

  def Recv(self, lpath, rpath):
    self.WriteMessage(ID_RECV, rpath)
    rid, size = self.ReadMessage("data")
    if rid == ID_DATA or rid == ID_DONE:
      try:
        os.unlink(lpath)
        os.makedirs(os.path.split(lpath)[0])
      except OSError: # most likely exists
        pass
      outf = open(lpath, "w")
      try:
        while rid == ID_DATA:
          while size > 0:
            data = self._sock.recv(size)
            outf.write(data)
            size -= len(data)
          rid, size = self.ReadMessage("data")
      finally:
        outf.close()
    self.CheckError(rid, size)

  def CheckError(self, rid, size):
    if rid == ID_DONE or rid == ID_OKAY:
      return
    if rid == ID_FAIL:
      reason = self._sock.recv(size)
    else:
      reason = "unknown reason (%lx)" % (rid,)
    raise AdbError("Recv error: %s" % (reason,))


class LogcatIOHandler(object):
  def __init__(self, sock, stream):
    self._sock = sock
    self._file = stream

  def fileno(self):
    return self._sock.fileno()

  def readable(self):
    return True

  def writable(self):
    return False

  def priority(self):
    return False

  def read_handler(self):
    data = self._sock.recv(4096)
    self._file.write(data)

  def write_handler(self):
    pass

  def pri_handler(self):
    pass

  def hangup_handler(self):
    pass

  def error_handler(self, ex, val, tb):
    print >>sys.stderr, "LogcatHandler error: %s (%s)" % (ex, val)

class DeviceManager(object):
  """Manage and select from multiple devices."""
  def __init__(self):
    self.Clear()

  def Clear(self):
    self._device_map = {}
    self._serial_map = {}

  def Add(self, device):
    self._device_map[device.device_id] = device
    self._serial_map[device.serial] = device

  def Remove(self, device):
    del self._device_map[device.device_id]
    del self._serial_map[device.serial]

  def GetSerialNumbers(self):
    return self._serial_map.keys()

  def __str__(self):
    s = ["List of devices attached:"]
    s.append("seq\tserial  \tstate")
    for dev in self._device_map.values():
      s.append(str(dev))
    return "\n".join(s)

  def GetById(self, dev_id):
    return self._device_map[dev_id]

  def GetBySerial(self, serno):
    return self._serial_map[serno]

  def Get(self, dev_or_serial):
    if isinstance(dev_or_serial, int):
      return self._device_map[dev_or_serial]
    else: # serno is a string
      return self._serial_map[dev_or_serial]

  def __getitem__(self, dev_or_serial):
    return self.Get(dev_or_serial)

if __name__=="__main__":
  StartServer()
  adb = AdbClient()
  devices = adb.GetDevices()
  device = devices[1]
  print device
  print device.ShellCommandOutput("env")
  print device.GetState()
  print device.List("/")
  device.Push("/etc/group", "/data/local/tmp/group")
  device.Pull("/tmp/group", "/data/local/tmp/group")
